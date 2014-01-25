/* Copyright (c) 2012, ZTE. All rights reserved.
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
#ifndef __ASM_ARCH_ZTE_GPIO_CONFIG_H
#define __ASM_ARCH_ZTE_GPIO_CONFIG_H


#define ZTE_GPIO_BT_RESET 16
#define ZTE_GPIO_SDCARD_DET 42

#define ZTE_GPIO_LIGHT_SENSOR_IRQ 17
#define ZTE_GPIO_COMPASS_SENSOR_IRQ 41
#define ZTE_GPIO_GYRO_SENSOR_IRQ 27
#define ZTE_GPIO_GYRO_RDY_IRQ 29
#define ZTE_GPIO_ACCEL_SENSOR_IRQ1 28

#ifdef CONFIG_PN544_NFC
#define ZTE_GPIO_PN544_VEN_EN  12
#define ZTE_GPIO_PN544_IRQ  114
#define ZTE_GPIO_PN544_FIRM  4
#endif


#endif
