/* Copyright (c) 2008-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


/*We use one wire program method to realize it.*/

/* modified history
 
         when             who   what  where
     12/04/27         maxiaoping  add file and code.
     12/06/08	     maxiaoping  modify lcd_gpio for P865E01 new hardware board.	
     12/07/13	     maxiaoping  disable debug logs for lcd backlight.
     13/03/15	     maxiaoping  modify delay function to avoid cpu freq varied leading udelay() loops_per_jiffy not updated problem.
*/

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_lead.h"
#include <mach/gpio.h>
#ifdef CONFIG_ZTE_PLATFORM
#include <mach/zte_memlog.h>
#endif


static bool onewiremode = false;

extern void __const_udelay_zte(unsigned long xloops);

#ifdef CONFIG_PROJECT_P865E01
extern int zte_get_hw_board_id(void);
#endif


int zte_backlight=0;


#if defined(CONFIG_PROJECT_P825A20)||defined(CONFIG_PROJECT_P865E04)
static int lcd_bkl_ctl=97;
#elif defined(CONFIG_PROJECT_V72A)
static int lcd_bkl_ctl=85;
#else 
static int lcd_bkl_ctl=96;
#endif

//maxiaoping temp modify for p865f03 20121104
#if defined(CONFIG_PROJECT_P865F03) || defined(CONFIG_PROJECT_P865F06)
#define ZTE_LCD_PWM_EN        127
extern void mipi_set_backlight_no_cabc(struct msm_fb_data_type *mfd);
#endif

void myudelay(unsigned int usec);
static void select_1wire_mode(void);
static void send_bkl_address(void);
static void send_bkl_data(int level);
void mipi_zte_set_backlight(struct msm_fb_data_type *mfd);
#ifdef CONFIG_PROJECT_P865E01
static int select_lcd_bl_gpio(void);
#endif

#define TPS61165_DEVICE_ADDRESS 0x72

void myudelay(unsigned int usec)
{
	//udelay(usec);
	__const_udelay_zte(usec * ((2199023UL*HZ)>>11));
}

#ifdef CONFIG_PROJECT_P865E01
static int select_lcd_bl_gpio(void)
{
	static int return_value = 0;
	static int read_once = 0;
	int rtl = 0;
	if(0 ==read_once)
	{
		rtl =zte_get_hw_board_id();	
		if(0 == rtl)
		{
			return_value = 96;
		}
		else
		{
			return_value = 97;
		}
		gpio_request(return_value, "lcd_bl_gpio");
		read_once = 1;
	}
	return return_value;
}
#endif

//Aviod udelay problem cause tps61165 Es mode time window wrong.
static void select_1wire_mode(void)
{
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(300);//myudelay(275);//myudelay(360);//myudelay(150);
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(600);//myudelay(650);//myudelay(840);//myudelay(300);				
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(700);//myudelay(770);//myudelay(1000);//myudelay(700);				
	
}

static void send_bkl_address(void)
{
	unsigned int i,j;
	i = TPS61165_DEVICE_ADDRESS;//0x72
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(10);
	//printk("PM_DEBUG_MXP: send_bkl_address \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(10);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(40);//myudelay(180);
		}
		else
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(40);//myudelay(180);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(10);
	gpio_direction_output(lcd_bkl_ctl, 1);

}

static void send_bkl_data(int level)
{
	unsigned int i,j;
	
	i = level & 0x1F;
	//printk("PM_DEBUG_MXP: tps61165 backlight data = %d \n",i);
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(10);
	//printk("PM_DEBUG_MXP: send_bkl_data \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(10);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(40);//myudelay(180);
		}
		else
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(40);//myudelay(180);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(10);
	gpio_direction_output(lcd_bkl_ctl, 1);

}

int zte_get_backlight(void)
{
	return zte_backlight;
}
EXPORT_SYMBOL(zte_get_backlight);


 void mipi_zte_set_backlight(struct msm_fb_data_type *mfd)
{
       /*value range is 1--32*/
	 int current_lel = mfd->bl_level;
  	 unsigned long flags;
	 
	 //maxiaoping temp modify for p865f03 20121104
	 #if defined(CONFIG_PROJECT_P865F03) || defined(CONFIG_PROJECT_P865F06)
	 unsigned lcd_pwm_en = GPIO_CFG(ZTE_LCD_PWM_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA);
	 static int read_once = 0;
	 #endif
	#if defined(CONFIG_PROJECT_V72A)
	 static int read_once = 0;
	#endif
    	//printk("PM_DEBUG_MXP: tps lcd_set_bl level=%d, %d\n", 
		   //current_lel , mfd->panel_power_on);

	#ifdef CONFIG_PROJECT_P865E01
	lcd_bkl_ctl = select_lcd_bl_gpio();
	#endif
	
	//maxiaoping temp modify for p865f03 20121104
	#if defined(CONFIG_PROJECT_P865F03) || defined(CONFIG_PROJECT_P865F06)
	if(0 == read_once)
	{
	    int rc =0;
		rc = gpio_tlmm_config(lcd_pwm_en,GPIO_CFG_ENABLE);
		if(rc < 0 )
		{
			printk("%s: gpio_tlmm_config GPIO_CFG_ENABLE config failed!",
			 __func__);
		}
		printk("%s: gpio_tlmm_config GPIO_CFG_ENABLE config sucess!",
			 __func__);
		rc = gpio_request(ZTE_LCD_PWM_EN, "lcd_pwm_en");
		if(rc < 0 )
		{
			printk("%s: gpio_request GPIO_CFG_ENABLE request failed!",
			 __func__);
		}	
		printk("%s: gpio_request GPIO_CFG_ENABLE request sucess!",
			 __func__);	
		
		gpio_direction_output(ZTE_LCD_PWM_EN, 1);
		read_once = 1;
	}
	#ifndef CONFIG_BACKLIGHT_CABC
	mipi_set_backlight_no_cabc(mfd);
	#endif
	#endif
	
	#if defined(CONFIG_PROJECT_V72A)
	if(0 == read_once)
	{
          int rc = 0;
		rc = gpio_request(lcd_bkl_ctl, "lcd_bl_gpio");
		if(rc < 0 )
		{
			printk("%s: gpio_request GPIO_CFG_ENABLE request failed!",
			 __func__);
		}	
		printk("%s: gpio_request GPIO_CFG_ENABLE request sucess!",
			 __func__);	
		read_once = 1;		
	}
	#endif
	
	if(!mfd->panel_power_on)
	{
    		gpio_direction_output(lcd_bkl_ctl, 0);
	    	return;
	}

    	if(current_lel < 1)
	{
        		current_lel = 0;
	}
		
	if(current_lel > 32)
	{
		current_lel = 32;
	}
	#if defined(CONFIG_PROJECT_V72A)
	if(current_lel >= 30)
	{
		current_lel = 27;
	}
	else
	{
		current_lel =(current_lel *27)/30;
	}		
	#else
	if(current_lel >= 30)
	{
		current_lel = 26;
	}
	else
	{
		current_lel = (current_lel *3)/4;
	}
	#endif
	//printk("PM_DEBUG_MXP: The new bl  level=%d.\n",current_lel);
	
	zte_backlight=current_lel;
	

	local_irq_save(flags);
	/*NOTE: Between local_irq_save() and local_irq_restore(),we can't add any print to prevent disturb the tps61165 timing.*/	
   	if(current_lel==0)
	{
		gpio_direction_output(lcd_bkl_ctl, 0);
		mdelay(3);
		onewiremode = FALSE;
			
	}
	else 
	{
		if(!onewiremode)	
		{
			select_1wire_mode();
			onewiremode = TRUE;
		}
		send_bkl_address();
		send_bkl_data(current_lel);
	}
		
	local_irq_restore(flags);
}

EXPORT_SYMBOL_GPL(mipi_zte_set_backlight);
