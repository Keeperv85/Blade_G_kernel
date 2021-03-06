/*drivers/input/keyboard/ft5x0x_ts.c
 *This file is used for FocalTech ft5x0x_ts touchscreen
 *
*/

/*
=======================================================================================================
When		Who	What,Where,Why		Comment			Tag
2011-07-11  xym  update ft driver    ZTE_TS_XYM_20110711
2011-04-20	zfj	add virtual key for P732A			ZFJ_TS_ZFJ_20110420     
2011-03-02	zfj	use create_singlethread_workqueue instead 	ZTE_TS_ZFJ_20110302 
2011-01-08	zfj	Create file			
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>
#include "ft5x0x_ts_new.h"
#include <linux/gpio.h>    //yaotieuri for JB2020 v2
#include <linux/fb.h>


#if defined(CONFIG_PROJECT_P865E01) || defined(CONFIG_PROJECT_P825A20)
#define GPIO_TOUCH_EN_OUT  13
#define GPIO_TOUCH_RST_OUT  12
#define GPIO_TOUCH_INT_WAKEUP	82
#else
#define GPIO_TOUCH_EN_OUT  107
#define GPIO_TOUCH_RST_OUT  26
#define GPIO_TOUCH_INT_WAKEUP	48
#endif


#define ABS_SINGLE_TAP	0x21	/* Major axis of touching ellipse */
#define ABS_TAP_HOLD	0x22	/* Minor axis (omit if circular) */
#define ABS_DOUBLE_TAP	0x23	/* Major axis of approaching ellipse */
#define ABS_EARLY_TAP	0x24	/* Minor axis (omit if circular) */
#define ABS_FLICK	0x25	/* Ellipse orientation */
#define ABS_PRESS	0x26	/* Major axis of touching ellipse */
#define ABS_PINCH 	0x27	/* Minor axis (omit if circular) */

#if defined(CONFIG_TOUCHSCREEN_VIRTUAL_KEYS)
#define virtualkeys virtualkeys.Fts-touchscreen

//P740A  800*480       MENU HOME BACK SEARCH  temp
static const char ts_keys_size[] = "0x01:139:52:858:94:85:0x01:172:173:858:108:85:0x01:158:302:858:110:85:0x01:217:433:858:110:85";

static ssize_t virtualkeys_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	sprintf(buf,"%s\n",ts_keys_size);
//	pr_info("%s\n",__FUNCTION__);
    return strlen(ts_keys_size)+2;
}
static DEVICE_ATTR(virtualkeys, 0444, virtualkeys_show, NULL);
extern struct kobject *android_touch_kobj;
static struct kobject * virtual_key_kobj;
static int ts_key_report_init(void)
{
	int ret;
	virtual_key_kobj = kobject_get(android_touch_kobj);
	if (virtual_key_kobj == NULL) {
		virtual_key_kobj = kobject_create_and_add("board_properties", NULL);
		if (virtual_key_kobj == NULL) {
			pr_err("%s: subsystem_register failed\n", __func__);
			ret = -ENOMEM;
			return ret;
		}
	}
 
	ret = sysfs_create_file(virtual_key_kobj, &dev_attr_virtualkeys.attr);
	if (ret) {
		pr_err("%s: sysfs_create_file failed\n", __func__);
		return ret;
	}
	pr_info("%s: virtual key init succeed!\n", __func__);
	return 0;
}
static void ts_key_report_deinit(void)
{
	sysfs_remove_file(virtual_key_kobj, &dev_attr_virtualkeys.attr);
	kobject_del(virtual_key_kobj);
}

#endif


static struct workqueue_struct *Fts_wq;
static struct i2c_driver Fts_ts_driver;

struct Fts_ts_data
{
	uint16_t addr;
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct Fts_finger_data finger_data[5];
	int touch_number;
	int touch_event;
	int use_irq;
	struct hrtimer timer;
	struct work_struct  work;
	uint16_t max[2];
	struct early_suspend early_suspend;
};


static u8 fwVer=0;
static u8 proc_fw_infomation[2];   //haoweiwei add 20121019 for proc_read_val
#if defined(CONFIG_SUPPORT_FTS_CTP_UPG) ||defined(CONFIG_FTS_USB_NOTIFY)
static struct i2c_client *update_client;
static int update_result_flag=0;
#endif

static int Fts_i2c_read(struct i2c_client *client, uint8_t reg, u8 * buf, int count) 
{
	int ret;
	struct i2c_msg msg[] = {
		{
			.addr = client->addr,
			.flags = 0,
			.len = 1,
			.buf = &reg,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = count,
			.buf = buf,
    }
	};

	ret = i2c_transfer(client->adapter, msg, 2);
  if ( ret != 2 )
    {
      dev_err(&client->dev, "%s: FAILED, read %d bytes from reg 0x%X\n", __func__, count, reg);
      return -1;
    }
	return 0;
}


static int Fts_i2c_write(struct i2c_client *client, int reg, u8 data)
{
	int ret;
	struct i2c_msg msg;
  u8 buf[2];
  buf[0] = reg;
  buf[1] = data;
	msg.addr = client->addr;
	msg.flags = client->flags & I2C_M_TEN;
	msg.len = 2;
	msg.buf = (char *)buf;

	ret = i2c_transfer(client->adapter, &msg, 1);
	if ( ret != 1 )
    {
        dev_err(&client->dev, "%s: FAILED, writing to reg 0x%X\n", __func__, reg);
        return -1;
    }

	return 0;
}

#if defined (CONFIG_FTS_USB_NOTIFY)
static bool esd_chg_fts_suspend = false;
unsigned long esd_fts_ts_event = 0;

static void Fts_Function_Handle(void)
{
	uint8_t buf,retry=0;
	if(esd_fts_ts_event&0x01){
		/******************haoweiwei 20121205 add for sky mod start*****************************/
		if ( proc_fw_infomation[1] <= 0x0f )
		{
			Fts_i2c_write(update_client, 0x0, 0x40);
			msleep(100);
			Fts_i2c_write(update_client, 0x07, 0x0f);
			msleep(20);
	Function_11_retry:
			Fts_i2c_write(update_client, 0x0, 0x0);
			msleep(20);
			Fts_i2c_read(update_client, 0x0, &buf,1);
			if ( buf != 0 )
			{
				if ( ++retry < 3 )
					goto Function_11_retry;
				else
				{
					gpio_direction_output(GPIO_TOUCH_RST_OUT, 0);
					msleep(5);
					gpio_direction_output(GPIO_TOUCH_RST_OUT, 1);
					msleep(5);
				}
			}
		}
		/******************haoweiwei 20121205 add for sky mod end*******************************/
		Fts_i2c_write( update_client, 0x86,0x3);
		printk("haowei charge on###\n");
	}
	else
	{
		/******************haoweiwei 20121205 add for sky mod start*****************************/
		if ( proc_fw_infomation[1] <= 0x0f )
		{
			Fts_i2c_write(update_client, 0x0, 0x40);
			msleep(100);
			Fts_i2c_write(update_client, 0x07, 0x09);
			msleep(20);
	Function_00_retry:
			Fts_i2c_write(update_client, 0x0, 0x0);
			msleep(20);
			Fts_i2c_read(update_client, 0x0, &buf,1);
			if ( buf != 0 )
			{
				if ( ++retry < 3 )
					goto Function_00_retry;
				else
				{
					gpio_direction_output(GPIO_TOUCH_RST_OUT, 0);
					msleep(5);
					gpio_direction_output(GPIO_TOUCH_RST_OUT, 1);
					msleep(5);
				}
			}		
		}
		/******************haoweiwei 20121205 add for sky mod end*******************************/
		Fts_i2c_write( update_client, 0x86,0x1);
		printk("haowei charge off###\n");
	}
}

static int Ft5x0x_ts_event(struct notifier_block *this, unsigned long event,void *ptr)
{
	int ret;
	static int status=0;
	esd_fts_ts_event = event;// | 0x10;
    if(esd_chg_fts_suspend){ 
         status = event;
         return NOTIFY_DONE ;
    }
#if 1
	Fts_Function_Handle();
#else
	switch(event)
		{
		case 0:
			//offline
			if(status!=0){
		 		status=0;
	   	/******************haoweiwei 20121205 add for sky mod start*****************************/
		  Fts_i2c_write(update_client, 0x0, 0x40);
		  msleep(100);
	      Fts_i2c_write(update_client, 0x07, 0x09);
		  msleep(20);
		  Fts_i2c_write(update_client, 0x0, 0x0);
		  msleep(20);
		/******************haoweiwei 20121205 add for sky mod end*******************************/
		  printk("###8\n");
				//printk("ts config change to offline status\n");
				Fts_i2c_write( update_client, 0x86,0x1);
			}
			break;
		case 1:
			//online
			if(status!=1){
		 		status=1;
	   	/******************haoweiwei 20121205 add for sky mod start*****************************/
		  Fts_i2c_write(update_client, 0x0, 0x40);
		  msleep(100);
	      Fts_i2c_write(update_client, 0x07, 0x0f);
		  msleep(20);
		  Fts_i2c_write(update_client, 0x0, 0x0);
		  msleep(20);
		/******************haoweiwei 20121205 add for sky mod end*******************************/
		  printk("###f\n");
				//printk("ts config change to online status\n");
				Fts_i2c_write( update_client, 0x86,0x3);
			}
			break;
		default:
			break;
		}
#endif
	ret = NOTIFY_DONE;

	return ret;
}

static struct notifier_block ts_notifier = {
	.notifier_call = Ft5x0x_ts_event,
};


static BLOCKING_NOTIFIER_HEAD(ts_chain_head);

int Ft5x0x_register_ts_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&ts_chain_head, nb);
}
EXPORT_SYMBOL_GPL(Ft5x0x_register_ts_notifier);

int Ft5x0x_unregister_ts_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&ts_chain_head, nb);
}
EXPORT_SYMBOL_GPL(Ft5x0x_unregister_ts_notifier);

int Ft5x0x_ts_notifier_call_chain(unsigned long val)
{
	return (blocking_notifier_call_chain(&ts_chain_head, val, NULL)
			== NOTIFY_BAD) ? -EINVAL : 0;
}

#endif
#ifdef CONFIG_SUPPORT_FTS_CTP_UPG

typedef enum
{
    ERR_OK,
    ERR_MODE,
    ERR_READID,
    ERR_ERASE,
    ERR_STATUS,
    ERR_ECC,
    ERR_DL_ERASE_FAIL,
    ERR_DL_PROGRAM_FAIL,
    ERR_DL_VERIFY_FAIL
}E_UPGRADE_ERR_TYPE;

typedef unsigned char         FTS_BYTE;     //8 bit
typedef unsigned short        FTS_WORD;    //16 bit
typedef unsigned int          FTS_DWRD;    //16 bit
typedef unsigned char         FTS_BOOL;    //8 bit

#define FTS_NULL                0x0
#define FTS_TRUE                0x01
#define FTS_FALSE              0x0

void delay_ms(FTS_WORD  w_ms)
{
    //platform related, please implement this function
    msleep( w_ms );
}

/*
[function]: 
    send a command to ctpm.
[parameters]:
    btcmd[in]        :command code;
    btPara1[in]    :parameter 1;    
    btPara2[in]    :parameter 2;    
    btPara3[in]    :parameter 3;    
    num[in]        :the valid input parameter numbers, if only command code needed and no parameters followed,then the num is 1;    
[return]:
    FTS_TRUE    :success;
    FTS_FALSE    :io fail;
*/
FTS_BOOL cmd_write(struct i2c_client *client,FTS_BYTE btcmd,FTS_BYTE btPara1,FTS_BYTE btPara2,FTS_BYTE btPara3,FTS_BYTE num)
{
    FTS_BYTE write_cmd[4] = {0};

    write_cmd[0] = btcmd;
    write_cmd[1] = btPara1;
    write_cmd[2] = btPara2;
    write_cmd[3] = btPara3;
    return i2c_master_send(client, write_cmd, num);
}

/*
[function]: 
    write data to ctpm , the destination address is 0.
[parameters]:
    pbt_buf[in]    :point to data buffer;
    bt_len[in]        :the data numbers;    
[return]:
    FTS_TRUE    :success;
    FTS_FALSE    :io fail;
*/
FTS_BOOL byte_write(struct i2c_client *client,FTS_BYTE* pbt_buf, FTS_DWRD dw_len)
{
    
    return i2c_master_send(client, pbt_buf, dw_len);
}

/*
[function]: 
    read out data from ctpm,the destination address is 0.
[parameters]:
    pbt_buf[out]    :point to data buffer;
    bt_len[in]        :the data numbers;    
[return]:
    FTS_TRUE    :success;
    FTS_FALSE    :io fail;
*/
FTS_BOOL byte_read(struct i2c_client *client,FTS_BYTE* pbt_buf, FTS_BYTE bt_len)
{
    return i2c_master_recv(client, pbt_buf, bt_len);
}


/*
[function]: 
    burn the FW to ctpm.
[parameters]:(ref. SPEC)
    pbt_buf[in]    :point to Head+FW ;
    dw_lenth[in]:the length of the FW + 6(the Head length);    
    bt_ecc[in]    :the ECC of the FW
[return]:
    ERR_OK        :no error;
    ERR_MODE    :fail to switch to UPDATE mode;
    ERR_READID    :read id fail;
    ERR_ERASE    :erase chip fail;
    ERR_STATUS    :status error;
    ERR_ECC        :ecc error.
*/


#define    FTS_PACKET_LENGTH        122//250//122//26/10/2

static unsigned char CTPM_FW[]=
{
#include "Ver0B_20110929_040_9980_app_P740A.i"
};

E_UPGRADE_ERR_TYPE  fts_ctpm_fw_upgrade(struct i2c_client *client, FTS_BYTE* pbt_buf, FTS_DWRD dw_lenth)
{
//    FTS_BYTE cmd_len     = 0;
    FTS_BYTE reg_val[2] = {0};
    FTS_DWRD i = 0;
//    FTS_BYTE ecc = 0;

    FTS_DWRD  packet_number;
    FTS_DWRD  j;
    FTS_DWRD  temp;
    FTS_DWRD  lenght;
    FTS_BYTE  packet_buf[FTS_PACKET_LENGTH + 6];
    FTS_BYTE  auc_i2c_write_buf[10];
    FTS_BYTE bt_ecc;

    /*********Step 1:Reset  CTPM *****/
    /*write 0xaa to register 0xfc*/
    Fts_i2c_write(client,0xfc,0xaa);
    delay_ms(50);
     /*write 0x55 to register 0xfc*/
    Fts_i2c_write(client,0xfc,0x55);
    printk("%s: Step 1: Reset CTPM test\n", __func__);

    delay_ms(40);

    /*********Step 2:Enter upgrade mode *****/
     auc_i2c_write_buf[0] = 0x55;
     auc_i2c_write_buf[1] = 0xaa;
     i2c_master_send(client, auc_i2c_write_buf, 2);
     printk("%s: Step 2: Enter update mode. \n", __func__);

    /*********Step 3:check READ-ID***********************/
    /*send the opration head*/
    do{
        if(i > 3)
        {
            return ERR_READID; 
        }
        /*read out the CTPM ID*/
        
        cmd_write(client,0x90,0x00,0x00,0x00,4);
        byte_read(client,reg_val,2);
        i++;
        printk("%s: Step 3: CTPM ID, ID1 = 0x%X, ID2 = 0x%X\n", __func__, reg_val[0], reg_val[1]);
    }while(reg_val[0] != 0x79 || reg_val[1] != 0x07);

     /*********Step 4:erase app*******************************/
    cmd_write(client,0x61,0x00,0x00,0x00,1);
    delay_ms(1500);
    printk("%s: Step 4: erase. \n", __func__);

    /*********Step 5:write firmware(FW) to ctpm flash*********/
    bt_ecc = 0;
    printk("%s: Step 5: start upgrade. \n", __func__);
    dw_lenth = dw_lenth - 8;
    packet_number = (dw_lenth) / FTS_PACKET_LENGTH;
    packet_buf[0] = 0xbf;
    packet_buf[1] = 0x00;
    for (j=0;j<packet_number;j++)
    {
        temp = j * FTS_PACKET_LENGTH;
        packet_buf[2] = (FTS_BYTE)(temp>>8);
        packet_buf[3] = (FTS_BYTE)temp;
        lenght = FTS_PACKET_LENGTH;
        packet_buf[4] = (FTS_BYTE)(lenght>>8);
        packet_buf[5] = (FTS_BYTE)lenght;

        for (i=0;i<FTS_PACKET_LENGTH;i++)
        {
            packet_buf[6+i] = pbt_buf[j*FTS_PACKET_LENGTH + i]; 
            bt_ecc ^= packet_buf[6+i];
        }
        
        byte_write(client,&packet_buf[0],FTS_PACKET_LENGTH + 6);
        delay_ms(FTS_PACKET_LENGTH/6 + 1);
        if ((j * (FTS_PACKET_LENGTH + 6) % 1024) == 0)
        {
              printk("%s: upgrade the 0x%X th byte.\n", __func__, ((unsigned int)j) * FTS_PACKET_LENGTH);
        }
    }

    if ((dw_lenth) % FTS_PACKET_LENGTH > 0)
    {
        temp = packet_number * FTS_PACKET_LENGTH;
        packet_buf[2] = (FTS_BYTE)(temp>>8);
        packet_buf[3] = (FTS_BYTE)temp;

        temp = (dw_lenth) % FTS_PACKET_LENGTH;
        packet_buf[4] = (FTS_BYTE)(temp>>8);
        packet_buf[5] = (FTS_BYTE)temp;

        for (i=0;i<temp;i++)
        {
            packet_buf[6+i] = pbt_buf[ packet_number*FTS_PACKET_LENGTH + i]; 
            bt_ecc ^= packet_buf[6+i];
        }

        byte_write(client,&packet_buf[0],temp+6);    
        delay_ms(20);
    }

    //send the last six byte
    for (i = 0; i<6; i++)
    {
        temp = 0x6ffa + i;
        packet_buf[2] = (FTS_BYTE)(temp>>8);
        packet_buf[3] = (FTS_BYTE)temp;
        temp =1;
        packet_buf[4] = (FTS_BYTE)(temp>>8);
        packet_buf[5] = (FTS_BYTE)temp;
        packet_buf[6] = pbt_buf[ dw_lenth + i]; 
        bt_ecc ^= packet_buf[6];

        byte_write(client,&packet_buf[0],7);  
        delay_ms(20);
    }

    /*********Step 6: read out checksum***********************/
    /*send the opration head*/
    cmd_write(client,0xcc,0x00,0x00,0x00,1);
    byte_read(client,reg_val,1);
    printk("%s: Step 6:  ecc read 0x%X, new firmware 0x%X. \n", __func__, reg_val[0], bt_ecc);
    if(reg_val[0] != bt_ecc)
    {
        return ERR_ECC;
    }

    /*********Step 7: reset the new FW***********************/
    cmd_write(client,0x07,0x00,0x00,0x00,1);

    return ERR_OK;
}

unsigned char fts_ctpm_get_upg_ver(void)
{
    unsigned int ui_sz;
    ui_sz = sizeof(CTPM_FW);
    if (ui_sz > 2)
    {
        return CTPM_FW[ui_sz - 2];
    }
    else
    {
        //TBD, error handling?
        return 0xff; //default value
    }
}


#endif

static int get_screeninfo(uint *xres, uint *yres)
{
	struct fb_info *info;

	info = registered_fb[0];
	if (!info) {
		pr_err("%s: Can not access lcd info \n",__func__);
		return -ENODEV;
	}

	*xres = info->var.xres;
	*yres = info->var.yres;
	printk("%s: xres=%d, yres=%d \n",__func__,*xres,*yres);

	return 1;
}


static int
proc_read_val(char *page, char **start, off_t off, int count, int *eof,
	  void *data)
{
	int len = 0;
	//len += sprintf(page + len, "%s\n", "touchscreen infomation");
	len += sprintf(page + len, "name: %s\n", "FocalTech");

	len += sprintf(page + len, "i2c address: 0x%X\n", 0x3E);

	len += sprintf(page + len, "IC type: %s\n", "FT53X6");

    len += sprintf(page + len, "firmware: 0x%X\n", fwVer);
/**********************************************************************/
switch (proc_fw_infomation[0]){
     
     case 0x51:
	 	len += sprintf(page + len, "module : %s\n", "Ofilm");
		break ;
	 case 0x53:
	 	len += sprintf(page + len, "module : %s\n", "Mutto");
		break ;
	 case 0x54:
	 	len += sprintf(page + len, "module : %s\n", "Eely");
		break ;
	 case 0x55:
	 	len += sprintf(page + len, "module : %s\n", "Laibao");
		break ;
	 case 0x57:
	 	len += sprintf(page + len, "module : %s\n", "Goworld");
		break ;
	 case 0x5a:
	 	len += sprintf(page + len, "module : %s\n", "Turly");
		break ;
	 case 0x5f:
	 	len += sprintf(page + len, "module : %s\n", "Success");
		break ;
	 case 0x60:
	 	len += sprintf(page + len, "module : %s\n", "Lead");
		break ;
	 case 0x85:
	 	len += sprintf(page + len, "module : %s\n", "Junda");
		break ;
	 case 0x87:
	 	len += sprintf(page + len, "module : %s\n", "LCE");
		break ;
	 case 0x8d:
	 	len += sprintf(page + len, "module : %s\n", "Tianma");
		break ;
	 default:
	 	len += sprintf(page + len, "module : %s\n", "I do not know");
}
//	len += sprintf(page + len, "module : %s\n", "FocalTech FT5x06 + Goworld");
/*********************************************************************/
#ifdef CONFIG_SUPPORT_FTS_CTP_UPG
{
        len += sprintf(page + len, "update flag : 0x%X\n", update_result_flag);
}
#endif
	if (off + count >= len)
		*eof = 1;
	if (len < off)
		return 0;
	*start = page + off;
	return ((count < len - off) ? count : len - off);
}

static int proc_write_val(struct file *file, const char *buffer,
           unsigned long count, void *data)
{
#ifdef CONFIG_SUPPORT_FTS_CTP_UPG
    int ret = 0;
#endif
    unsigned long val;
    
    sscanf(buffer, "%lu", &val);
    
#ifdef CONFIG_SUPPORT_FTS_CTP_UPG
    printk("%s: Fts Upgrade Start\n", __func__);
    update_result_flag=0;
    
    fts_ctpm_fw_upgrade(update_client, CTPM_FW, sizeof(CTPM_FW));
    //fts_ctpm_fw_upgrade_with_i_file(ts->client);//Update the CTPM firmware if need
    {
    //adjust begin
    u8 buf; 
    int ret = 0, retry = 0;
    msleep(2000);
    Fts_i2c_write(update_client, 0x00, 0x40);
    msleep(20);
    ret = Fts_i2c_read(update_client, 0x00, &buf, 1); 
    retry = 3;
    while(retry-- >0) 
    {   
        if(0x40 == buf)
            break;
        if(0x40 != buf)
        {   
            pr_info("%s: Enter test mode failed, retry = %d!\n", __func__, retry);
            Fts_i2c_write(update_client, 0x00, 0x40);
            msleep(20);
            ret = Fts_i2c_read(update_client, 0x00, &buf, 1); 
        }
    }   
    
    msleep(3000);
    Fts_i2c_write(update_client, 0x4D, 0x4);
    msleep(10000);
    Fts_i2c_read(update_client, 0x4D, &buf, 1); 
    retry=5;
    while(retry-- >0) 
    {   
        if(0x50 == buf)
            break;
        if(0x50 != buf)
        {   
            pr_info("%s: value of register 0x4D is 0x%X.\n", __func__, buf);
            Fts_i2c_read(update_client, 0x4D, &buf, 1); 
            if(buf == 0x30)
                pr_info("%s: Calibration failed! Retry!\n", __func__);
            msleep(2000);
        }   
    
    }   
    
    pr_info("%s: value of register 0x4D is 0x%X\n", __func__, buf);
    
    Fts_i2c_write(update_client, 0x00, 0x0);
    //adjust end
    }		
	
    ret = Fts_i2c_read(update_client, FT5X0X_REG_FIRMID, &fwVer,1) ;
    printk("%s: read New Fts FW ID = 0x%X, ret = 0x%X\n", __FUNCTION__, fwVer, ret);
	
    update_result_flag=2;
#endif		
    return -EINVAL;
}
static void release_all_fingers(struct Fts_ts_data *ts)
{
	int i;
	for(i =0; i< ts->touch_event; i ++ )
	{
		input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, ts->finger_data[i].touch_id);
		//input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0);
		//input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0 );
		input_report_abs(ts->input_dev, ABS_MT_PRESSURE, 0);		
		input_report_abs(ts->input_dev, ABS_MT_POSITION_X, ts->finger_data[i].x );
		input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, ts->finger_data[i].y );
		input_mt_sync(ts->input_dev);	
	}
	input_sync(ts->input_dev);
}
static int fIntMask=0;
static void Fts_ts_work_func(struct work_struct *work)
{
	int ret, i;
	uint8_t buf[33];
	struct Fts_ts_data *ts = container_of(work, struct Fts_ts_data, work);


	ret = Fts_i2c_read(ts->client, 0x00, buf, 33); 
	if (ret < 0){
   		printk(KERN_ERR "%s: Fts_i2c_read failed, go to poweroff.\n", __func__);
	    	gpio_direction_output(GPIO_TOUCH_EN_OUT, 0);
	    	msleep(200);
	    	gpio_direction_output(GPIO_TOUCH_EN_OUT, 1);
	    	msleep(220);
	}
	else
	{
		

		ts->touch_number = buf[2]&0x0f;
		ts->touch_event = buf[2] >> 4;
		for (i = 0; i< 5; i++)
		{
			ts->finger_data[i].x = (uint16_t)((buf[3 + i*6] & 0x0F)<<8 )| (uint16_t)buf[4 + i*6];
			ts->finger_data[i].y = (uint16_t)((buf[5 + i*6] & 0x0F)<<8 )| (uint16_t)buf[6 + i*6];
			ts->finger_data[i].z = buf[7 + i*6];
			ts->finger_data[i].w = buf[8 + i*6];
			ts->finger_data[i].touch_id = buf[5 + i*6] >> 4;
			ts->finger_data[i].event_flag = buf[3 + i*6] >> 6;

		}
		for (i = 0; i< ts->touch_event; i++)
		{
			input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, ts->finger_data[i].touch_id);
			//input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, ts->finger_data[i].z);
			//input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, ts->finger_data[i].w );
			input_report_abs(ts->input_dev, ABS_MT_POSITION_X, ts->finger_data[i].x );
			input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, ts->finger_data[i].y );
			input_report_abs(ts->input_dev, ABS_MT_PRESSURE, ts->finger_data[i].z);		
			input_mt_sync(ts->input_dev);
			//printk("%s: finger=%d, z=%d, event_flag=%d, touch_id=%d\n", __func__, i, 
			//ts->finger_data[i].z, ts->finger_data[i].event_flag,ts->finger_data[i].touch_id);
		}

		if(ts->finger_data[0].y >=810)
		{
			//input_report_key(ts->input_dev, BTN_9, !!ts->finger_data[0].z);// recovery mode use
		}	
		
		input_sync(ts->input_dev);
	}
	if (ts->use_irq)
		enable_irq(ts->client->irq);
}

static irqreturn_t Fts_ts_irq_handler(int irq, void *dev_id)
{
	struct Fts_ts_data *ts = dev_id;

if ( fIntMask )
{
	disable_irq_nosync(ts->client->irq);
	queue_work(Fts_wq, &ts->work);
}	
fIntMask = 1;

	return IRQ_HANDLED;
}

static int Fts_ts_suspend(struct i2c_client *client, pm_message_t mesg)
{
	int ret = 0;
	struct Fts_ts_data *ts;
/***********************haoweiwei add 20121121 start*****************************/
	esd_chg_fts_suspend = true;
	fIntMask = 0;
/***********************haoweiwei add 20121121 end*******************************/
	//ts = container_of(client, struct Fts_ts_data , client);
	ts = i2c_get_clientdata(client);
	disable_irq(client->irq);
	ret = cancel_work_sync(&ts->work);
	if(ret & ts->use_irq)
		enable_irq(client->irq);
	//flush_workqueue(ts->work);
	// ==set mode ==, 
	//ft5x0x_set_reg(FT5X0X_REG_PMODE, PMODE_HIBERNATE);
	Fts_i2c_write(client, FT5X0X_REG_PMODE, PMODE_HIBERNATE);
	gpio_direction_output(GPIO_TOUCH_INT_WAKEUP,1);

	return 0;
}

static int Fts_ts_resume(struct i2c_client *client)
{
	uint8_t buf,retry=0;
	struct Fts_ts_data *ts;
	ts = i2c_get_clientdata(client);
Fts_resume_start:	
	//gpio_direction_output(GPIO_TOUCH_EN_OUT,1);
	//gpio_direction_output(GPIO_TOUCH_INT_WAKEUP,1);
	//msleep(250);
	gpio_set_value(GPIO_TOUCH_INT_WAKEUP,0);
	msleep(5);
	gpio_set_value(GPIO_TOUCH_INT_WAKEUP,1);
	msleep(5);
	gpio_direction_input(GPIO_TOUCH_INT_WAKEUP);

	if ( Fts_i2c_read(client, FT5X0X_REG_FIRMID, &buf,1) < 0)
	{//I2C error read firmware ID
		printk("%s: Fts FW ID read Error: retry=0x%X\n", __func__, retry);
		if ( ++retry < 3 )
			goto Fts_resume_start;
	}
/***********************haoweiwei add 20121121 start*****************************/
    esd_chg_fts_suspend = false;
	Fts_Function_Handle();
	release_all_fingers(ts);
	enable_irq(client->irq);
/***********************haoweiwei add 20121121 end*******************************/
	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void Fts_ts_early_suspend(struct early_suspend *h)
{
	struct Fts_ts_data *ts;
	
	ts = container_of(h, struct Fts_ts_data, early_suspend);
	Fts_ts_suspend(ts->client, PMSG_SUSPEND);
}

static void Fts_ts_late_resume(struct early_suspend *h)
{
	struct Fts_ts_data *ts;
	ts = container_of(h, struct Fts_ts_data, early_suspend);
	Fts_ts_resume(ts->client);
}
#endif

static int Fts_ts_probe(
	struct i2c_client *client, const struct i2c_device_id *id)
{
	struct Fts_ts_data *ts;
	int ret = 0, retry = 0;
	//u8 fwVer;
	struct proc_dir_entry *refresh;//*dir, *refresh;
	//u8 buf;
	int xres=0, yres=0;	// lcd xy resolution

	printk("%s: enter----\n", __func__);


	ret = gpio_request(GPIO_TOUCH_RST_OUT, "touch voltage");
	if (ret)
	{	
		printk("%s: gpio %d request is error!\n", __func__, GPIO_TOUCH_RST_OUT);
		goto err_check_gpio1_failed;
	}   

	gpio_direction_output(GPIO_TOUCH_RST_OUT, 1);
	msleep(10);
	printk("%s: GPIO_TOUCH_RST_OUT(%d) = %d\n", __func__, GPIO_TOUCH_RST_OUT, gpio_get_value(GPIO_TOUCH_RST_OUT));


	ret = gpio_request(GPIO_TOUCH_EN_OUT, "touch voltage");
	if (ret)
	{	
		printk("%s: gpio %d request is error!\n", __func__, GPIO_TOUCH_EN_OUT);
		goto err_check_gpio2_failed;
	}   

	gpio_direction_output(GPIO_TOUCH_EN_OUT, 1);
	msleep(300);//xym
	printk("%s: GPIO_TOUCH_EN_OUT(%d) = %d\n", __func__, GPIO_TOUCH_EN_OUT, gpio_get_value(GPIO_TOUCH_EN_OUT));

	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
	{
		printk(KERN_ERR "%s: need I2C_FUNC_I2C\n", __func__);
		ret = -ENODEV;
		goto err_check_functionality_failed;
	}
	ts = kzalloc(sizeof(*ts), GFP_KERNEL);
	if (ts == NULL)
	{
		ret = -ENOMEM;
		goto err_alloc_data_failed;
	}

	INIT_WORK(&ts->work, Fts_ts_work_func);
	
	Fts_wq= create_singlethread_workqueue("Fts_wq");
	if(!Fts_wq)
	{
		ret = -ESRCH;
		pr_err("%s: creare single thread workqueue failed!\n", __func__);
		goto err_create_singlethread;
	}
	
	ts->client = client;
#if defined(CONFIG_SUPPORT_FTS_CTP_UPG) || defined(CONFIG_FTS_USB_NOTIFY)
	update_client = client;
	update_result_flag = 0;
#endif
	i2c_set_clientdata(client, ts);
	client->driver = &Fts_ts_driver;
	
	{
		retry = 3;
		while (retry-- > 0)
		{
			ret = Fts_i2c_read(client, FT5X0X_REG_FIRMID, &fwVer,1);
			pr_info("%s: read FT5X0X_REG_FIRMID = 0x%X.\n", __func__, fwVer);
			//fwVer = buf;
			if (0 == ret)
				break;
			msleep(10);
		}
		
		
		if (retry < 0)
		{
			ret = -1;
			goto err_detect_failed;
		}
		
	}
/***************************haoweiwei add start 20121019*************************************/
ret = Fts_i2c_read(client, FT5X0X_REG_FT5201ID, &proc_fw_infomation[0],1);
printk("TP Module infomation: 0x%x\n",proc_fw_infomation[0]);
msleep(30);
ret = Fts_i2c_read(client, FT5X0X_REG_FIRMID, &proc_fw_infomation[1],1);

/***************************haoweiwei add end 20121019*************************************/
	Fts_Function_Handle();   //haoweiwei add

/*	
  //Firmware Upgrade handle Start
#ifdef CONFIG_SUPPORT_FTS_CTP_UPG
  if (  fts_ctpm_get_upg_ver() > fwVer )
  {
      printk("Fts Upgrade Start\n");
      fts_ctpm_fw_upgrade(ts->client,CTPM_FW, sizeof(CTPM_FW));
      //fts_ctpm_fw_upgrade_with_i_file(ts->client);//Update the CTPM firmware if need
      {
          //adjust begin
          msleep(2000);
          Fts_i2c_write(ts->client, 0x00, 0x40);
          msleep(20);
          ret = Fts_i2c_read(ts->client, 0x00, &buf, 1); 
          retry = 3;
          while(retry-- >0) 
          {   
            if(0x40 == buf)
            	break;
            if(0x40 != buf)
            {   
              pr_info("%s: Enter test mode failed, retry = %d!\n", __func__, retry);
              Fts_i2c_write(ts->client, 0x00, 0x40);
              msleep(20);
              ret = Fts_i2c_read(ts->client, 0x00, &buf, 1); 
            }
          }   
          
          msleep(3000);
          Fts_i2c_write(ts->client, 0x4D, 0x4);
          msleep(10000);
          Fts_i2c_read(ts->client, 0x4D, &buf, 1); 
          retry=5;
          while(retry-- >0) 
          {   
            if(0x50 == buf)
              break;
            if(0x50 != buf)
            {   
              pr_info("%s value of register 0x4D is %d.\n", __func__, buf);
              Fts_i2c_read(ts->client, 0x4D, &buf, 1); 
              if(buf == 0x30)
                      pr_info("%s: Calibration failed! Retry!\n", __func__);
              msleep(2000);
            } 
          }   
          
          pr_info("%s value of register 0x4D is %d\n", __func__, buf);
          
          Fts_i2c_write(ts->client, 0x00, 0x0);
          //adjust end
      }
  }
  ret = Fts_i2c_read(ts->client, FT5X0X_REG_FIRMID, &fwVer, 1) ;
  printk("%s: New Fts FW ID read ID = 0x%x, ret = 0x%x\n", __FUNCTION__, fwVer, ret);
#endif
	//Firmware Upgrade handle End
*/
	ts->input_dev = input_allocate_device();
	if (ts->input_dev == NULL) {
		ret = -ENOMEM;
		printk(KERN_ERR "%s: Failed to allocate input device\n", __func__);
		goto err_input_dev_alloc_failed;
	}
	
	ts->input_dev->name = "Fts-touchscreen";
	//ts->input_dev->phys = "Fts-touchscreen/input0";

	get_screeninfo(&xres, &yres);

	set_bit(EV_SYN, ts->input_dev->evbit);
	set_bit(EV_KEY, ts->input_dev->evbit);
	set_bit(EV_ABS, ts->input_dev->evbit);
	//set_bit(BTN_9, ts->input_dev->keybit);
	set_bit(KEY_HOME, ts->input_dev->keybit);
	set_bit(KEY_MENU, ts->input_dev->keybit);
	set_bit(KEY_BACK, ts->input_dev->keybit);
	set_bit(KEY_SEARCH, ts->input_dev->keybit);
	set_bit(ABS_MT_TRACKING_ID, ts->input_dev->absbit);
	//set_bit(ABS_MT_TOUCH_MAJOR, ts->input_dev->absbit);
	set_bit(ABS_MT_POSITION_X, ts->input_dev->absbit);
	set_bit(ABS_MT_POSITION_Y, ts->input_dev->absbit);
	//set_bit(ABS_MT_WIDTH_MAJOR, ts->input_dev->absbit);
	//set_bit(ABS_MT_ORIENTATION, ts->input_dev->absbit);
	set_bit(ABS_MT_PRESSURE, ts->input_dev->absbit);
	
	input_set_abs_params(ts->input_dev, ABS_MT_TRACKING_ID, 0, 10, 0, 0);
	//input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0, 127, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0, xres, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0, yres, 0, 0);
	//input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 16, 208, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_PRESSURE, 0, 0xFF, 0, 0);
	/*input_set_abs_params(ts->input_dev, ABS_SINGLE_TAP, 0, 5, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_TAP_HOLD, 0, 5, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_EARLY_TAP, 0, 5, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_FLICK, 0, 5, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_PRESS, 0, 5, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_DOUBLE_TAP, 0, 5, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_PINCH, -255, 255, 0, 0);*/

	ret = input_register_device(ts->input_dev);
	if (ret)
	{
		printk(KERN_ERR "%s: Unable to register %s input device\n", __func__, ts->input_dev->name);
		goto err_input_register_device_failed;
	}

    if (client->irq)
    {
      ret = request_irq(client->irq, Fts_ts_irq_handler, IRQF_TRIGGER_FALLING, "ft5x0x_ts", ts);
      if (ret == 0)
        ts->use_irq = 1;
      else
      {
        dev_err(&client->dev, "request_irq failed\n");
        goto err_input_request_irq_failed;
      }
    }
#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	ts->early_suspend.suspend = Fts_ts_early_suspend;
	ts->early_suspend.resume = Fts_ts_late_resume;
	register_early_suspend(&ts->early_suspend);
#endif
	//dir = proc_mkdir("touchscreen", NULL);
	refresh = create_proc_entry("driver/tsc_id", 0777, NULL);
	if (refresh) {
		refresh->data		= NULL;
		refresh->read_proc  = proc_read_val;
		refresh->write_proc = proc_write_val;
	}
	printk(KERN_INFO "%s: Start touchscreen %s in %s mode\n", __func__, ts->input_dev->name, ts->use_irq ? "interrupt" : "polling");
	
#if defined(CONFIG_TOUCHSCREEN_VIRTUAL_KEYS)
	ts_key_report_init();
#endif
#if defined(CONFIG_FTS_USB_NOTIFY)
	Ft5x0x_register_ts_notifier(&ts_notifier);
#endif

	return 0;

err_input_request_irq_failed:
err_input_register_device_failed:
	input_free_device(ts->input_dev);

err_input_dev_alloc_failed:
err_detect_failed:
	kfree(ts);
	destroy_workqueue(Fts_wq);
err_create_singlethread: 
err_alloc_data_failed:
err_check_functionality_failed:
	gpio_free(GPIO_TOUCH_EN_OUT);
err_check_gpio2_failed:
	gpio_free(GPIO_TOUCH_RST_OUT);
	
err_check_gpio1_failed:
	return ret;
}

static int Fts_ts_remove(struct i2c_client *client)
{
	struct Fts_ts_data *ts = i2c_get_clientdata(client);
#if defined(CONFIG_TOUCHSCREEN_VIRTUAL_KEYS)
	ts_key_report_deinit();
#endif
	unregister_early_suspend(&ts->early_suspend);
	if (ts->use_irq)
		free_irq(client->irq, ts);
	else
		hrtimer_cancel(&ts->timer);
	input_unregister_device(ts->input_dev);
	kfree(ts);
	gpio_direction_output(GPIO_TOUCH_EN_OUT, 0);
	return 0;
}




static const struct i2c_device_id Fts_ts_id[] = {
	{ "ft5x0x_ts", 0 },
	{ }
};

static struct i2c_driver Fts_ts_driver = {
	.probe		= Fts_ts_probe,
	.remove		= Fts_ts_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend	= Fts_ts_suspend,
	.resume		= Fts_ts_resume,
#endif
	.id_table	= Fts_ts_id,
	.driver 	= {
		.name	= "ft5x0x_ts",
	},
};

static int __devinit Fts_ts_init(void)
{
	
	#if 0
	Fts_wq = create_rt_workqueue("Fts_wq");
	if (!Fts_wq)
		return -ENOMEM;
	#endif
	
	return i2c_add_driver(&Fts_ts_driver);
}

static void __exit Fts_ts_exit(void)
{
	i2c_del_driver(&Fts_ts_driver);
	if (Fts_wq)
		destroy_workqueue(Fts_wq);
}

module_init(Fts_ts_init);
module_exit(Fts_ts_exit);

MODULE_DESCRIPTION("Fts Touchscreen Driver");
MODULE_LICENSE("GPL");
