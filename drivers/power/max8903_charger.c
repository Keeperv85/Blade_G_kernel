/*
 * max8903_charger.c - Maxim 8903 USB/Adapter Charger Driver
 *
 * Copyright (C) 2011 Samsung Electronics
 * MyungJoo Ham <myungjoo.ham@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*==============================================================================

                           EDIT HISTORY

when         who           what
--------     ---------     ---------------------------             ------------
2012/12/17   maxiaoping    add for max8903 charger IC        
2013/01/16   maxiaoping    done with max8903 charger IC bsp driver
2013/01/17   maxiaoping    merge to the 8X25 JB2035 SVN.
2013/01/23   maxiaoping    modify battery capacity 99% report full problem.
2013/03/21   maxiaoping    modify max8903_stop_charging local struct point null problem.
2013/04/01   maxiaoping    modify max8903 occasionally can't charged fully during power off charging.
==============================================================================*/

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/power_supply.h>
#include <linux/platform_device.h>
#include <linux/power/max8903_charger.h>

#include <linux/jiffies.h>
#include <mach/pmic.h>
#include <linux/delay.h>
#include <mach/msm_hsusb.h>
#include <linux/wakelock.h>
#include <linux/i2c.h>
#include <linux/earlysuspend.h>  
#include <mach/msm_battery.h>

extern struct i2c_client *max17040_bak_client;
extern int pmic_get_chg_real_status(uint *status);
extern int max17040_get_soc(struct i2c_client *client);
extern int max17040_get_vcell(struct i2c_client *client);

bool max8903_batt_probed = false;
bool max8903_poll_timer_on = true;
struct max8903_data {
	struct max8903_pdata pdata;
	struct device *dev;
	struct power_supply psy;
	struct delayed_work		battery_work;
	struct delayed_work		charger_work;
	
	int		present;
	
	bool		charging;
	bool		usb_chg_enable;/*We don't enable usb_chg*/
	bool 		fault;
	bool 		usb_in;
	bool 		ta_in;
	
	struct wake_lock wl;
	
};


#define MAX8903_BATT_POLL_PERIOD	(60000) /*1 minute poll*/
#define MAX8903_BATTERY_FULL	100
#define AC_CHARGER_DETECT_DELAY   ((HZ) * 2)
#define INVALID_BATT_HANDLE    -1
#define BATTERY_LOW		3200
#define BATTERY_HIGH		4300

static DEFINE_SPINLOCK(max8903_spinlock);

/* Wall charger or USB charger */
static enum chg_type cur_chg_type = USB_CHG_TYPE__INVALID;
static enum chg_type zte_get_chg_type = USB_CHG_TYPE__INVALID;
static struct max8903_data *zte_max8903_data = NULL;
static int delay_ac_charger_detect = 0;

struct msm_battery_info {
	u32 voltage_max_design;
	u32 voltage_min_design;
	u32 voltage_fail_safe;
	u32 chg_api_version;
	u32 batt_technology;
	u32 batt_api_version;

	u32 avail_chg_sources;
	u32 current_chg_source;

	u32 batt_status;
	u32 batt_health;
	u32 charger_valid;
	u32 batt_valid;
	u32 batt_capacity; /* in percentage */

	u32 charger_status;
	u32 charger_type;
	u32 battery_status;
	u32 battery_level;
	u32 battery_voltage; /* in millie volts */
	u32 battery_temp;  /* in celsius */

	u32(*calculate_capacity) (u32 voltage);

	s32 batt_handle;

	struct power_supply *msm_psy_ac;
	struct power_supply *msm_psy_usb;
	struct power_supply *msm_psy_batt;
	struct power_supply *current_ps;

	wait_queue_head_t wait_q;

	u32 vbatt_modify_reply_avail;

	struct early_suspend early_suspend;
};

static struct msm_battery_info max8903_batt_info = {
	.batt_handle = INVALID_BATT_HANDLE,
	.charger_status = CHARGER_STATUS_BAD,
	.charger_type = CHARGER_TYPE_INVALID,
	.battery_status = BATTERY_STATUS_GOOD,
	.battery_level = BATTERY_LEVEL_FULL,
	.battery_voltage = BATTERY_HIGH,
	.batt_capacity = 100,
	.batt_status = POWER_SUPPLY_STATUS_DISCHARGING,
	.batt_health = POWER_SUPPLY_HEALTH_GOOD,
	.batt_valid  = 1,
	.battery_temp = 23,
	.vbatt_modify_reply_avail = 0,
};

static enum power_supply_property max8903_power_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static char *max8903_power_supplied_to[] = {
	"battery",
};

int zte_hsusb_get_chg_type(void)
{
	return zte_get_chg_type;
}

void zte_hsusb_set_chg_type(int chg_type)
{
	 zte_get_chg_type = chg_type;
}

bool  max8903_batt_driver_registered(void)
{
	return max8903_batt_probed;
}

static int max8903_power_get_property(struct power_supply *psy,
				  enum power_supply_property psp,
				  union power_supply_propval *val)
{
	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		if (psy->type == POWER_SUPPLY_TYPE_MAINS) {
			val->intval = max8903_batt_info.current_chg_source & AC_CHG
			    ? 1 : 0;
		}
		if (psy->type == POWER_SUPPLY_TYPE_USB) {
			val->intval = max8903_batt_info.current_chg_source & USB_CHG
			    ? 1 : 0;
		}
		break;
	default:
		return -EINVAL;
	}
	return 0;
}


static struct power_supply max8903_psy_ac = {
	.name = "ac",
	.type = POWER_SUPPLY_TYPE_MAINS,
	.supplied_to = max8903_power_supplied_to,
	.num_supplicants = ARRAY_SIZE(max8903_power_supplied_to),
	.properties = max8903_power_props,
	.num_properties = ARRAY_SIZE(max8903_power_props),
	.get_property = max8903_power_get_property,
};

static struct power_supply max8903_psy_usb = {
	.name = "usb",
	.type = POWER_SUPPLY_TYPE_USB,
	.supplied_to = max8903_power_supplied_to,
	.num_supplicants = ARRAY_SIZE(max8903_power_supplied_to),
	.properties = max8903_power_props,
	.num_properties = ARRAY_SIZE(max8903_power_props),
	.get_property = max8903_power_get_property,
};

static enum power_supply_property max8903_batt_power_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP		 
};

static int max8903_batt_power_get_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       union power_supply_propval *val)
{
	
	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = max8903_batt_info.batt_status;
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = max8903_batt_info.batt_health;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = max8903_batt_info.batt_valid;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = max8903_batt_info.batt_technology;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
		val->intval = max8903_batt_info.voltage_max_design;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
		val->intval = max8903_batt_info.voltage_min_design;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = max8903_batt_info.battery_voltage*1000;   
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = max8903_batt_info.batt_capacity;
		break;
	case POWER_SUPPLY_PROP_TEMP:
		val->intval = max8903_batt_info.battery_temp*10;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}


static struct power_supply max8903_psy_batt = {
	.name = "battery",
	.type = POWER_SUPPLY_TYPE_BATTERY,
	.properties = max8903_batt_power_props,
	.num_properties = ARRAY_SIZE(max8903_batt_power_props),
	.get_property = max8903_batt_power_get_property,
};
static u32 max8903_batt_capacity(u32 current_voltage)
{
	u32 low_voltage	 = 3200;
	u32 high_voltage = 4200;
	u8 batt_cap = 0;
	
	if (current_voltage <= low_voltage)
		return 0;
	else if (current_voltage >= high_voltage)
		return 100;
	else
	{
		batt_cap = max17040_get_soc(max17040_bak_client);
	}
		
	 return batt_cap;
}

static void max8903_charger_work(struct work_struct *max8903_work)
{
	struct max8903_data *max_chg;
	struct	power_supply	*supp=NULL;
	int chg_status;
	int chg_type;
	int err_flag;
	max_chg = container_of(max8903_work, struct max8903_data,
			charger_work.work);
	
	printk( "PM_DEBUG_MXP:Enter max8903_charger_work.\n");

	err_flag = pmic_get_chg_real_status(&chg_status);
	pr_debug("PM_DEBUG_MXP:REAL_STATUS:err_flag =%d.\n",err_flag);
	printk("PM_DEBUG_MXP:REAL_STATUS:chg_status =%d.\n",chg_status);
	if(chg_status)
	{
		delay_ac_charger_detect = 1;
		//printk("PM_DEBUG_MXP:delay_ac_charger_detect=%d.\n",delay_ac_charger_detect);
		chg_type = zte_hsusb_get_chg_type();
		printk("PM_DEBUG_MXP:Here chg_type=%d.\n",chg_type);
		/* change charger type */
		if(USB_CHG_TYPE__WALLCHARGER == chg_type)//wall charger
		{
			max8903_batt_info.current_chg_source = AC_CHG;
			max8903_batt_info.charger_type = CHARGER_TYPE_USB_WALL;
			supp=&max8903_psy_ac;
		}
		else if(USB_CHG_TYPE__SDP == chg_type)//usb host
		{
			max8903_batt_info.current_chg_source = USB_CHG;
			max8903_batt_info.charger_type = CHARGER_TYPE_USB_PC;
			supp=&max8903_psy_usb;
		}
		else//other unknown charger 
		{
			/* if it's not a wall charger or usb host,we all think it as ac charger.*/
			max8903_batt_info.current_chg_source = AC_CHG;
			max8903_batt_info.charger_type = CHARGER_TYPE_USB_WALL;
			supp=&max8903_psy_ac;
		}
	}
	else//No charger detect.
	{
		delay_ac_charger_detect = 0;
		max8903_batt_info.current_chg_source = 0;// update the online stauts.
		supp=&max8903_psy_batt;
		max8903_batt_info.charger_type = CHARGER_TYPE_NONE;
	}

	if (supp) 
	{	
			printk("PM_DEBUG_MXP:Handset Supply = %s\n", supp->name);
			if(max8903_batt_info.current_ps != supp)
			{
				if(max8903_batt_info.current_ps)
				power_supply_changed(supp);
				power_supply_changed(max8903_batt_info.current_ps);
				max8903_batt_info.current_ps = supp;
			}
	}
	
	printk( "PM_DEBUG_MXP:Exit max8903_charger_work.\n");
}

/*
 * max8903_chg_connected() - notify the charger connected event
 * @chg_type:	charger type 
 */
void max8903_chg_connected(enum chg_type chg_type)
{
	cur_chg_type = chg_type;
	pr_info("%s:chg type =%d\n", __func__, cur_chg_type);

	if (delay_ac_charger_detect){
		delay_ac_charger_detect = 0;
		cancel_delayed_work(&zte_max8903_data->charger_work);
		
		pr_info("%s: queue a insert event\n", __func__);
		/*msm_charger_notify_event(&saved_msm_chg->adapter_hw_chg,
				CHG_INSERTED_EVENT);*/
	}	
}

/*
 * max8903_usb_charger_state() - determine the usb charger state
 * @mA:	the charge current from USB system
 *
 * if current is USB charger and USB charger is disabled,
 * modify the *mA to 0. That cause not to start charging
 */
 /*
static void max8903_usb_charger_state(int *mA)
{
	if ((USB_CHG_TYPE__SDP == cur_chg_type) && 
				(!zte_max8903_data->usb_chg_enable)){
		pr_info("%s:mA=%d, change to 0\n", __func__, *mA);
		*mA = 0;
	}
}
*/

static void max8903_battery_work(struct work_struct *max8903_work)
{
	struct max8903_data *max_chg;
	int current_capacity;
	int current_voltage;
	static int old_capacity = 0;
	static int old_voltage = 0;

	max_chg = container_of(max8903_work, struct max8903_data,
			battery_work.work);
	
	pr_debug("PM_DEBUG_MXP:Enter max8903_battery_work.\n");
	current_capacity = max17040_get_soc(max17040_bak_client);
	current_voltage  = max17040_get_vcell(max17040_bak_client);
	printk("PM_DEBUG_MXP:battery capacity=%d,battery voltage=%d.\n",current_capacity, current_voltage);
	max8903_batt_info.battery_status = BATTERY_STATUS_GOOD;
	pr_debug("PM_DEBUG_MXP:max_chg->charging=%d.\n",max_chg->charging);
	if (max_chg->charging) 
	{
		max8903_batt_info.charger_status =CHARGER_STATUS_GOOD;
		if((current_capacity < old_capacity) && (current_voltage > (old_voltage-100)))
		{
			max8903_batt_info.battery_voltage = old_voltage;
			max8903_batt_info.batt_capacity = old_capacity;
		}
		else
		{
			max8903_batt_info.battery_voltage = current_voltage;
			max8903_batt_info.batt_capacity = current_capacity;
		}
		
		if(max8903_batt_info.batt_capacity >= MAX8903_BATTERY_FULL)
		{
			max8903_batt_info.batt_status = POWER_SUPPLY_STATUS_FULL;
			spin_lock(&max8903_spinlock);
			if(max_chg->charging == true) 
			{
				if(wake_lock_active(&max_chg->wl))
				{
					printk("PM_DEBUG_MXP:Battery full,release wakelock.\n");
					wake_unlock(&max_chg->wl);
				}
			}
			spin_unlock(&max8903_spinlock);
		}
		else
		{
			max8903_batt_info.batt_status = POWER_SUPPLY_STATUS_CHARGING;
		}

	}
	else
	{
		max8903_batt_info.charger_status =CHARGER_STATUS_GOOD;
		max8903_batt_info.batt_status = POWER_SUPPLY_STATUS_DISCHARGING;
		max8903_batt_info.battery_voltage = current_voltage;
		max8903_batt_info.batt_capacity = current_capacity;
	}

	power_supply_changed(&max8903_psy_batt);

	old_voltage = max8903_batt_info.battery_voltage;
	old_capacity = max8903_batt_info.batt_capacity;
	schedule_delayed_work(&max_chg->battery_work,msecs_to_jiffies(MAX8903_BATT_POLL_PERIOD));
	pr_debug("PM_DEBUG_MXP:Exit max8903_battery_work.\n");
}

#if 0
static int max8903_get_property(struct power_supply *psy,
		enum power_supply_property psp,
		union power_supply_propval *val)
{
	struct max8903_data *data = container_of(psy,
			struct max8903_data, psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = POWER_SUPPLY_STATUS_UNKNOWN;
		if (data->pdata.chg) {
			if (gpio_get_value(data->pdata.chg) == 0)
				val->intval = POWER_SUPPLY_STATUS_CHARGING;
			else if (data->usb_in || data->ta_in)
				val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
			else
				val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		}
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = 0;
		if (data->usb_in || data->ta_in)
			val->intval = 1;
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = POWER_SUPPLY_HEALTH_GOOD;
		if (data->fault)
			val->intval = POWER_SUPPLY_HEALTH_UNSPEC_FAILURE;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}
#endif

 int max8903_start_usb_charging(void)
{
	struct max8903_data *max_chg;
	int ret = 0;
	max_chg = zte_max8903_data;
	
	printk( "PM_DEBUG_MXP:Enter max8903_start_usb_charging.\n");
	if(NULL == max_chg)
	{
		printk( "PM_DEBUG_MXP:max8903 charger not prepared.\n");
		return -1;
	}

	#ifdef ZTE_MAX8903_DEBUG
	printk( "PM_DEBUG_MXP:(max_chg->pdata).cen = %d.\n",(max_chg->pdata).cen);
	printk( "PM_DEBUG_MXP:(max_chg->pdata).dcm = %d.\n",(max_chg->pdata).dcm);
	#endif
	gpio_direction_output((max_chg->pdata).cen, 0); //enable charger
    
	/*
	 * if usb charger is enabled, pull the USUS down
	 * else pull the USUS up
	 */
	#if 0
	if (max_chg->usb_chg_enable)
		//gpio_direction_output((max_chg->pdata).usus, 0);
		pmic_gpio_set_value((max_chg->pdata).usus, 0)
	else
		gpio_direction_output((max_chg->pdata).usus, 1);
    	#endif
	
	gpio_direction_output((max_chg->pdata).dcm, 0);	//dcm=low ->500ma.		
	
	//Enable wake_lock,Mark charging flag.
	spin_lock(&max8903_spinlock);
	if(max_chg->charging == false) 
	{
		max_chg->charging = true;
		wake_lock(&max_chg->wl);
	}
	spin_unlock(&max8903_spinlock);
	printk( "PM_DEBUG_MXP:Exit max8903_start_usb_charging.\n");
	return ret;
}
int max8903_start_wall_charging(void)
{
	struct max8903_data *max_chg;
	int ret = 0;
	int value = 0;
	max_chg = zte_max8903_data;
	
	printk( "PM_DEBUG_MXP:Enter max8903_start_wall_charging.\n");
	if(NULL == max_chg)
	{
		printk( "PM_DEBUG_MXP:max8903 charger not prepared.\n");
		return -1;
	}
	
	#ifdef ZTE_MAX8903_DEBUG
	printk( "PM_DEBUG_MXP:(max_chg->pdata).cen = %d.\n",(max_chg->pdata).cen);
	printk( "PM_DEBUG_MXP:(max_chg->pdata).dcm = %d.\n",(max_chg->pdata).dcm);
	#endif
	gpio_direction_output((max_chg->pdata).cen, 0); //enable charger
    
	/*
	 * if usb charger is enabled, pull the USUS down
	 * else pull the USUS up
	 */
	 #if 0
	if (max_chg->usb_chg_enable)
		gpio_direction_output((max_chg->pdata).usus, 0);
	else
		gpio_direction_output((max_chg->pdata).usus, 1);
	#endif
	
    	//Enable AC charging.
	ret = gpio_direction_output((max_chg->pdata).dcm, 1);	//dcm=high ->1500ma.	
	pr_debug("PM_DEBUG_MXP:set gpio29 ret=%d.\n",ret);
	value = __gpio_get_value((max_chg->pdata).dcm);
	pr_debug("PM_DEBUG_MXP:get gpio29 value=%d.\n",value);
	
	//Enable wake_lock,Mark charging flag.
	spin_lock(&max8903_spinlock);
	if(max_chg->charging == false) 
	{
		max_chg->charging = true;
		wake_lock(&max_chg->wl);
	}
	spin_unlock(&max8903_spinlock);
	printk( "PM_DEBUG_MXP:Exit max8903_start_wall_charging.\n");
	
	return ret;
}

int max8903_stop_charging(void)
{
	struct max8903_data *max_chg;
	int ret = 0;
	max_chg = zte_max8903_data;
	
	printk( "PM_DEBUG_MXP:Enter max8903_stop_charging.\n");
	if(NULL == max_chg)
	{
		printk( "PM_DEBUG_MXP:max8903 charger not prepared.\n");
		return -1;
	}
	
	gpio_direction_output((max_chg->pdata).cen, 1); //always enable charger.
	
	//Enable usb charging.
	gpio_direction_output((max_chg->pdata).dcm, 0);	//restore its default settings.
	/*
	 * if usb charger is enabled, pull the USUS down
	 * else pull the USUS up
	 */
	#if 0
	if (max_chg->usb_chg_enable)
		gpio_direction_output((max_chg->pdata).usus, 0);
	else
		gpio_direction_output((max_chg->pdata).usus, 1);
	#endif

	//disable wake_lock,Mark charging flag.
	spin_lock(&max8903_spinlock);
	if(max_chg->charging == true) 
	{
		max_chg->charging = false;
		if(wake_lock_active(&max_chg->wl))
			wake_unlock(&max_chg->wl);
		//wake_lock_timeout(&max_chg->wl, 1000);
		wake_lock_timeout(&max_chg->wl, 3 * HZ);
	}
	spin_unlock(&max8903_spinlock);
	printk( "PM_DEBUG_MXP:Exit max8903_stop_charging.\n");
	return ret;
}

#if 0
static irqreturn_t max8903_dcin(int irq, void *_data)
{
	struct max8903_data *data = _data;
	struct max8903_pdata *pdata = &data->pdata;
	bool ta_in;
	enum power_supply_type old_type;

	ta_in = gpio_get_value(pdata->dok) ? false : true;

	if (ta_in == data->ta_in)
		return IRQ_HANDLED;

	data->ta_in = ta_in;

	/* Set Current-Limit-Mode 1:DC 0:USB */
	if (pdata->dcm)
		gpio_set_value(pdata->dcm, ta_in ? 1 : 0);

	/* Charger Enable / Disable (cen is negated) */
	if (pdata->cen)
		gpio_set_value(pdata->cen, ta_in ? 0 :
				(data->usb_in ? 0 : 1));

	dev_dbg(data->dev, "TA(DC-IN) Charger %s.\n", ta_in ?
			"Connected" : "Disconnected");

	old_type = data->psy.type;

	if (data->ta_in)
		data->psy.type = POWER_SUPPLY_TYPE_MAINS;
	else if (data->usb_in)
		data->psy.type = POWER_SUPPLY_TYPE_USB;
	else
		data->psy.type = POWER_SUPPLY_TYPE_BATTERY;

	if (old_type != data->psy.type)
		power_supply_changed(&data->psy);

	return IRQ_HANDLED;
}

static irqreturn_t max8903_usbin(int irq, void *_data)
{
	struct max8903_data *data = _data;
	struct max8903_pdata *pdata = &data->pdata;
	bool usb_in;
	enum power_supply_type old_type;

	usb_in = gpio_get_value(pdata->uok) ? false : true;

	if (usb_in == data->usb_in)
		return IRQ_HANDLED;

	data->usb_in = usb_in;

	/* Do not touch Current-Limit-Mode */

	/* Charger Enable / Disable (cen is negated) */
	if (pdata->cen)
		gpio_set_value(pdata->cen, usb_in ? 0 :
				(data->ta_in ? 0 : 1));

	dev_dbg(data->dev, "USB Charger %s.\n", usb_in ?
			"Connected" : "Disconnected");

	old_type = data->psy.type;

	if (data->ta_in)
		data->psy.type = POWER_SUPPLY_TYPE_MAINS;
	else if (data->usb_in)
		data->psy.type = POWER_SUPPLY_TYPE_USB;
	else
		data->psy.type = POWER_SUPPLY_TYPE_BATTERY;

	if (old_type != data->psy.type)
		power_supply_changed(&data->psy);

	return IRQ_HANDLED;
}

static int max8903_charging_switched(struct msm_hardware_charger *hw_chg)
{
	struct max8903_struct *max_chg;

	max_chg = container_of(hw_chg, struct max8903_struct, adapter_hw_chg);
	dev_info(max_chg->dev, "%s\n", __func__);
	return 0;
}


static irqreturn_t max8903_fault(int irq, void *_data)
{
	struct max8903_data *data = _data;
	struct max8903_pdata *pdata = &data->pdata;
	bool fault;

	fault = gpio_get_value(pdata->flt) ? false : true;

	if (fault == data->fault)
		return IRQ_HANDLED;

	data->fault = fault;

	if (fault)
		dev_err(data->dev, "Charger suffers a fault and stops.\n");
	else
		dev_err(data->dev, "Charger recovered from a fault.\n");

	return IRQ_HANDLED;
}

static irqreturn_t max_valid_handler(int irq, void *dev_id)
{
	struct max8903_struct *max_chg;
    struct platform_device *pdev;
    struct pm8058_chip *chip;
    int state;

    pdev = (struct platform_device *)dev_id;
    max_chg = platform_get_drvdata(pdev);
    chip = get_irq_data(irq);
	
#if 0
	/* Dock insert, think it as AC charger */
	if (gpio_get_value_cansleep(max_chg->dock_det) && (BOARD_NUM(hw_ver) != BOARD_NUM_V11))
		max_chg->adapter_hw_chg.type = CHG_TYPE_USB;
	else
		max_chg->adapter_hw_chg.type = CHG_TYPE_AC;
#endif
	/* reinitialize charger type */
	if ((BOARD_NUM(hw_ver) == BOARD_NUM_V11))
		max_chg->adapter_hw_chg.type = CHG_TYPE_AC;
	else
		max_chg->adapter_hw_chg.type = CHG_TYPE_USB;

    state = pm8058_irq_get_rt_status(chip, irq);
    pr_info("%s:charge state=%d, hw_chg_type=%d\n", __func__, state, max_chg->adapter_hw_chg.type);
    if(state){
		/* delay to queue charge insert envent when charger inserted,
		 * need to detect if it is an ac charger
		 */
        //msm_charger_notify_event(&max_chg->adapter_hw_chg,
        //			 CHG_INSERTED_EVENT);
		delay_ac_charger_detect = 1;
		pr_info("%s:delay_ac_charger_detect=%d start ac charger delay work\n", __func__, delay_ac_charger_detect);
		schedule_delayed_work(&max_chg->ac_charger,
					AC_CHARGER_DETECT_DELAY);
        max_chg->present = 1;
		wake_lock(&max_chg->wl);
    }else{
    	delay_ac_charger_detect = 0;
		pr_info("%s:delay_ac_charger_detect=%d cancel ac charger delay work\n", __func__, delay_ac_charger_detect);
		cancel_delayed_work(&saved_msm_chg->ac_charger);
		
        msm_charger_notify_event(&max_chg->adapter_hw_chg,
        			 CHG_REMOVED_EVENT);
        max_chg->present = 0;
		wake_unlock(&max_chg->wl);
    }
	return IRQ_HANDLED;
}
#endif

void manage_max8903_charging(int work_type)
{
	zte_hsusb_set_chg_type(work_type);
	
	if(USB_CHG_TYPE__WALLCHARGER == work_type)
	{
		max8903_start_wall_charging();
	}
	else if(USB_CHG_TYPE__INVALID == work_type)
	{
		max8903_stop_charging();
	}
	else
	{
		max8903_start_usb_charging();
	}

	if(NULL != zte_max8903_data)
	{
	 	cancel_delayed_work_sync(&zte_max8903_data->charger_work);
		cancel_delayed_work_sync(&zte_max8903_data->battery_work);
	 	schedule_delayed_work(&zte_max8903_data->charger_work,0);
		schedule_delayed_work(&zte_max8903_data->battery_work,0);
	}
	
}
/*
 * show_usb_chg_enable() - Show max8903 usb chg enable status
 */
static ssize_t show_usb_chg_enable(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct max8903_data *data;
	
	data = dev_get_drvdata(dev);

	return sprintf(buf, "CHG ENABLE:%u\nCHG TYPE:%u\n", 
				data->usb_chg_enable, cur_chg_type);
}

/*
 * store_usb_chg_enable() - Enable/Disable max8903 usb charge
 */
static ssize_t store_usb_chg_enable(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct max8903_data *data;	
	char *after;
	unsigned long num;
	
	data = dev_get_drvdata(dev);
	
	
	num = simple_strtoul(buf, &after, 10);
	if (num == 0)
	{
		/* disable max8903 usb charge */
		data->usb_chg_enable = 0;
	
		/*
		 * if current is USB charger, need to stop it
		 * set the source current to 0 mA
		 */
		if (USB_CHG_TYPE__SDP == cur_chg_type){
			cancel_delayed_work(&data->charger_work);	/* cancel the charger work */
			//msm_set_source_current(&data->adapter_hw_chg, 0);
			/*msm_charger_notify_event(&data->adapter_hw_chg,
							CHG_ENUMERATED_EVENT);*/
		}
		
		/* pull the USUS up */
		//gpio_direction_output((data->pdata).usus, 1);
	}
	else
	{
		/* enable max8903 usb charge */
		data->usb_chg_enable = 1;

		/* pull the USUS down */
		//gpio_direction_output((data->pdata).usus, 0);
		
		/*
		 * if current is USB charger, need to start it
		 * set the source current to 500 mA
		 */
		if (USB_CHG_TYPE__SDP == cur_chg_type){
			//msm_set_source_current(&data->adapter_hw_chg, 500);
			/*msm_charger_notify_event(&data->adapter_hw_chg,
							CHG_ENUMERATED_EVENT);*/
		}
	}
	
	return count;
}

static struct device_attribute usb_chg_enable_attr =
	__ATTR(usb_chg_enable, S_IRUGO | S_IWUSR | S_IWGRP, show_usb_chg_enable, store_usb_chg_enable);

void max8903_batt_early_suspend(struct early_suspend *h)	
{
	pr_debug("%s: enter\n", __func__);

	#if 0
	if(NULL != zte_max8903_data)
	cancel_delayed_work_sync(&zte_max8903_data->battery_work);
	#endif
	max8903_poll_timer_on = false;
	pr_debug("%s: exit\n", __func__);
}

void max8903_batt_late_resume(struct early_suspend *h)	
{
	
	pr_debug("%s: enter\n", __func__);
	if(max8903_poll_timer_on == false)
	{
		#if 0
		if(NULL != zte_max8903_data)
		schedule_delayed_work(&zte_max8903_data->battery_work,msecs_to_jiffies(5));
		#endif
		max8903_poll_timer_on = true;
	}
	pr_debug("%s: exit\n", __func__);
}

static int max8903_batt_cleanup(void)
{
	int rc = 0;

	if (max8903_batt_info.msm_psy_ac)
		power_supply_unregister(max8903_batt_info.msm_psy_ac);

	if (max8903_batt_info.msm_psy_usb)
		power_supply_unregister(max8903_batt_info.msm_psy_usb);
	if (max8903_batt_info.msm_psy_batt)
		power_supply_unregister(max8903_batt_info.msm_psy_batt);

#ifdef CONFIG_HAS_EARLYSUSPEND
	if (max8903_batt_info.early_suspend.suspend == max8903_batt_early_suspend)
		unregister_early_suspend(&max8903_batt_info.early_suspend);
#endif

	return rc;
}

static __devinit int max8903_probe(struct platform_device *pdev)
{
	struct max8903_data *data;
	struct device *dev = &pdev->dev;
	struct max8903_pdata *pdata = pdev->dev.platform_data;
	
	int ret = 0;
	int gpio;
	int ta_in = 0;
	uint usb_in = 0;
	int err_flag = 0;

	printk( "PM_DEBUG_MXP:Enter max8903_probe.\n");
	data = kzalloc(sizeof(struct max8903_data), GFP_KERNEL);
	if (data == NULL) {
		printk("PM_DEBUG_MXP:Cannot allocate memory.\n");
		return -ENOMEM;
	}
	
	if (pdata == NULL) {
		printk("PM_DEBUG_MXP:%s no platform data.\n",__func__);
		ret = -EINVAL;
	}
	INIT_DELAYED_WORK(&data->battery_work, max8903_battery_work);//poll for battery.
	INIT_DELAYED_WORK(&data->charger_work, max8903_charger_work); //poll for charging
	wake_lock_init(&data->wl, WAKE_LOCK_SUSPEND, "max8903");
	
	memcpy(&data->pdata, pdata, sizeof(struct max8903_pdata));
	data->dev = dev;
	data->usb_chg_enable = false;
	zte_max8903_data = data;
	platform_set_drvdata(pdev, data);
	
	#ifdef ZTE_MAX8903_DEBUG
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).cen = %d.\n",(zte_max8903_data->pdata).cen);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).dcm = %d.\n",(zte_max8903_data->pdata).dcm);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).chg = %d.\n",(zte_max8903_data->pdata).chg);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).flt = %d.\n",(zte_max8903_data->pdata).flt);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).usb_valid = %d.\n",(zte_max8903_data->pdata).usb_valid);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).dc_valid = %d.\n",(zte_max8903_data->pdata).dc_valid);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).voltage_fail_safe = %d.\n",(zte_max8903_data->pdata).voltage_fail_safe);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).voltage_max_design = %d.\n",(zte_max8903_data->pdata).voltage_max_design);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).voltage_min_design = %d.\n",(zte_max8903_data->pdata).voltage_min_design);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).avail_chg_sources = %d.\n",(zte_max8903_data->pdata).avail_chg_sources);
	printk("PM_DEBUG_MXP:(zte_max8903_data->pdata).batt_technology = %d.\n",(zte_max8903_data->pdata).batt_technology);
	#endif
	
	
	if (pdata->dc_valid == false && pdata->usb_valid == false) {
		printk("PM_DEBUG_MXP:No valid power sources.\n");
		ret = -EINVAL;
		goto err;
	}

	
	err_flag = pmic_get_chg_real_status(&ta_in);
	pr_debug("PM_DEBUG_MXP:err_flag=%d\n",err_flag);
	pr_debug("PM_DEBUG_MXP:ta_in=%d\n",ta_in);

	#if 0 // charger cen pin has been requested before.
	ret = gpio_request(pdata->cen, "MAX8903_CHARGER_CEN");
	if (ret) {
	printk("PM_DEBUG_MXP:CEN gpio_request failed for %d ret=%d.\n", pdata->cen, ret);
	}
	#endif
	ret = gpio_tlmm_config(GPIO_CFG(pdata->cen, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	if (ret) {
	printk("PM_DEBUG_MXP: CEN gpio_tlmm_config failed for %d ret=%d.\n", pdata->cen, ret);
	return -EIO;
	}
	
	ret = gpio_request(pdata->dcm, "MAX8903_CHARGER_DCM");
	if (ret) {
	printk("PM_DEBUG_MXP:DCM gpio_request failed for %d ret=%d.\n",pdata->dcm, ret);
	}
	ret = gpio_tlmm_config(GPIO_CFG(pdata->dcm, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	if (ret) {
	printk("PM_DEBUG_MXP:DCM gpio_tlmm_config failed for %d ret=%d.\n", pdata->dcm, ret);
	return -EIO;
	}
	
	if (pdata->dc_valid) {
		#if 0
		if (pdata->dok && gpio_is_valid(pdata->dok) &&
				pdata->dcm && gpio_is_valid(pdata->dcm)) {
			gpio = pdata->dok; /* PULL_UPed Interrupt */
			ta_in = gpio_get_value(gpio) ? 0 : 1;

			gpio = pdata->dcm; /* Output */
			gpio_set_value(gpio, ta_in);	
		}
		#endif
		if (pdata->dcm && gpio_is_valid(pdata->dcm)) 
		{
			//gpio = pdata->dok; /* PULL_UPed Interrupt */
			//ta_in = gpio_get_value(gpio) ? 0 : 1;

			gpio = pdata->dcm; /* Output */	
			//gpio_direction_output(gpio,~ta_in);
			gpio_direction_output(gpio,0);//default usb charger charging current.
		}
		else {
			printk("PM_DEBUG_MXP:When DC is wired, DOK and DCM should"
					" be wired as well.\n");
			ret = -EINVAL;
			goto err;
		}
	}
	else 
	{
		if (pdata->dcm) {
			if (gpio_is_valid(pdata->dcm))
				gpio_set_value(pdata->dcm, 0);
			else {
				printk("PM_DEBUG_MXP:Invalid pin: dcm.\n");
				ret = -EINVAL;
				goto err;
			}
		}
	}

	if (pdata->usb_valid) {
		if (pdata->uok && gpio_is_valid(pdata->uok)) {
			gpio = pdata->uok;
			usb_in = gpio_get_value(gpio) ? 0 : 1;
		} else {
			dev_err(dev, "When USB is wired, UOK should be wired."
					"as well.\n");
			ret = -EINVAL;
			goto err;
		}
	}

	if (pdata->cen) {
		if (gpio_is_valid(pdata->cen)) 
		{
			//gpio_direction_output(pdata->cen,(ta_in || usb_in) ? 0 : 1);
			gpio_direction_output(pdata->cen,(ta_in) ? 1 : 0);
			//gpio_set_value(pdata->cen, (ta_in || usb_in) ? 0 : 1);
			//gpio_set_value(pdata->cen, 0);
		} 
		else 
		{
			printk("PM_DEBUG_MXP:Invalid pin: cen.\n");
			ret = -EINVAL;
			goto err;
		}
	}

	if (pdata->chg) {
		//As chg pin use PMIC gpio,its api is different.
		ret = pmic_gpio_direction_input(pdata->chg);
		if(ret < 0)
		{
			printk("PM_DEBUG_MXP:Invalid pin: chg.\n");
			ret = -EINVAL;
			goto err;
		}
	}

	if (pdata->flt) {
		//if (!gpio_is_valid(pdata->flt))//As chg pin use PMIC gpio,its api is different.
		ret = pmic_gpio_direction_input(pdata->flt);
		if(ret < 0)
		{
			printk("PM_DEBUG_MXP:Invalid pin: flt.\n");
			ret = -EINVAL;
			goto err;
		}
	}
	
	#if 0//usus is pull down to ground on new pcb.
	if (pdata->usus) {
		//if (!gpio_is_valid(pdata->usus))//As chg pin use PMIC gpio,its api is different.
		ret = pmic_gpio_direction_input(pdata->usus);
		if(ret < 0)	
		{
			printk("PM_DEBUG_MXP:Invalid pin: usus.\n");
			ret = -EINVAL;
			goto err;
		}
	}
	#endif
	
	ret = device_create_file(data->dev, &usb_chg_enable_attr);
	if (ret) {
		printk("PM_DEBUG_MXP:failed: create usb_chg_enable file.\n");
	}
	
	{
	//register ac charger power source.	
	ret = power_supply_register(&pdev->dev, &max8903_psy_ac);
	if (ret < 0) 
	{
		printk("PM_DEBUG_MXP:max8903_psy_ac power_supply_register failed,"
			" ret = %d.\n", ret);
		max8903_batt_cleanup();
		return ret;
	}
	max8903_batt_info.msm_psy_ac = &max8903_psy_ac;
	max8903_batt_info.avail_chg_sources |= AC_CHG;

	//register usb charger power source.	
	ret = power_supply_register(&pdev->dev, &max8903_psy_usb);
	if (ret < 0) {
		printk("PM_DEBUG_MXP:max8903_psy_usb power_supply_register failed,"
			" ret = %d.\n", ret);
		max8903_batt_cleanup();
		return ret;
	}
	max8903_batt_info.msm_psy_usb = &max8903_psy_usb;
	max8903_batt_info.avail_chg_sources |= USB_CHG;

	if (!max8903_batt_info.msm_psy_ac && !max8903_batt_info.msm_psy_usb) 
	{
		printk("PM_DEBUG_MXP:No external Power supply(AC or USB)"
			"is avilable.\n");
		max8903_batt_cleanup();
		return -ENODEV;
	}

	//register battery power source.	
	max8903_batt_info.voltage_max_design = pdata->voltage_max_design;
	max8903_batt_info.voltage_min_design = pdata->voltage_min_design;
	max8903_batt_info.voltage_fail_safe  = pdata->voltage_fail_safe;
	max8903_batt_info.batt_technology = pdata->batt_technology;
	max8903_batt_info.calculate_capacity = max8903_batt_capacity;
	
	ret = power_supply_register(&pdev->dev, &max8903_psy_batt);
	if (ret < 0) 
	{
		printk("PM_DEBUG_MXP:max8903_psy_batt power_supply_register failed,"
			" ret = %d.\n", ret);
		max8903_batt_cleanup();
		return ret;
	}
	max8903_batt_info.msm_psy_batt = &max8903_psy_batt;
	max8903_batt_info.current_ps = &max8903_psy_batt;//
	if(ta_in)
	{
		max8903_start_usb_charging();
		schedule_delayed_work(&data->charger_work,0);
	}
	schedule_delayed_work(&data->battery_work, msecs_to_jiffies(1500));
	
	//register early_suspend&late_resume callback.
	#ifdef CONFIG_HAS_EARLYSUSPEND
	max8903_batt_info.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	max8903_batt_info.early_suspend.suspend = max8903_batt_early_suspend;
	max8903_batt_info.early_suspend.resume = max8903_batt_late_resume;
	register_early_suspend(&max8903_batt_info.early_suspend);
	#endif
	
	}
	
	#if 0
	data->fault = false;
	data->ta_in = ta_in;
	data->usb_in = usb_in;

	data->psy.name = "max8903_charger";
	data->psy.type = (ta_in) ? POWER_SUPPLY_TYPE_MAINS :
			((usb_in) ? POWER_SUPPLY_TYPE_USB :
			 POWER_SUPPLY_TYPE_BATTERY);
	data->psy.get_property = max8903_get_property;
	data->psy.properties = max8903_charger_props;
	data->psy.num_properties = ARRAY_SIZE(max8903_charger_props);

	ret = power_supply_register(dev, &data->psy);
	if (ret) {
		dev_err(dev, "failed: power supply register.\n");
		goto err;
	}
	#endif
	
	#if 0
	if (pdata->dc_valid) {
		ret = request_threaded_irq(gpio_to_irq(pdata->dok),
				NULL, max8903_dcin,
				IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				"MAX8903 DC IN", data);
		if (ret) {
			dev_err(dev, "Cannot request irq %d for DC (%d)\n",
					gpio_to_irq(pdata->dok), ret);
			goto err_psy;
		}
	}

	if (pdata->usb_valid) {
		ret = request_threaded_irq(gpio_to_irq(pdata->uok),
				NULL, max8903_usbin,
				IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				"MAX8903 USB IN", data);
		if (ret) {
			dev_err(dev, "Cannot request irq %d for USB (%d)\n",
					gpio_to_irq(pdata->uok), ret);
			goto err_dc_irq;
		}
	}

	if (pdata->flt) {
		ret = request_threaded_irq(gpio_to_irq(pdata->flt),
				NULL, max8903_fault,
				IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				"MAX8903 Fault", data);
		if (ret) {
			dev_err(dev, "Cannot request irq %d for Fault (%d)\n",
					gpio_to_irq(pdata->flt), ret);
			goto err_usb_irq;
		}
	}
	#endif
	max8903_batt_probed = true;
	printk( "PM_DEBUG_MXP:Exit max8903_probe.\n");
	return 0;

/*err_usb_irq:
	if (pdata->usb_valid)
		free_irq(gpio_to_irq(pdata->uok), data);
err_dc_irq:
	if (pdata->dc_valid)
		free_irq(gpio_to_irq(pdata->dok), data);
err_psy:
	power_supply_unregister(&data->psy);*/
err:
	kfree(data);
	return ret;
}

static __devexit int max8903_remove(struct platform_device *pdev)
{
	struct max8903_data *data = platform_get_drvdata(pdev);

	if (data) {
		//struct max8903_pdata *pdata = &data->pdata;

		device_remove_file(data->dev, &usb_chg_enable_attr);//
		#if 0
		if (pdata->flt)
			free_irq(gpio_to_irq(pdata->flt), data);
		if (pdata->usb_valid)
			free_irq(gpio_to_irq(pdata->uok), data);
		if (pdata->dc_valid)
			free_irq(gpio_to_irq(pdata->dok), data);
		#endif
		cancel_delayed_work_sync(&data->charger_work);//
		cancel_delayed_work_sync(&data->battery_work);//
		//msm_charger_notify_event(&data->adapter_hw_chg, CHG_REMOVED_EVENT);//
		//msm_charger_unregister(&data->adapter_hw_chg);//
		//power_supply_unregister(&data->psy);
		kfree(data);
	}
	max8903_batt_probed = false;
	return 0;
}

static struct platform_driver max8903_driver = {
	.probe	= max8903_probe,
	.remove	= __devexit_p(max8903_remove),
	.driver = {
		.name	= "max8903-charger",
		.owner	= THIS_MODULE,
	},
};

module_platform_driver(max8903_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MAX8903 Charger Driver");
MODULE_AUTHOR("MyungJoo Ham <myungjoo.ham@samsung.com>");
MODULE_ALIAS("platform:max8903-charger");
