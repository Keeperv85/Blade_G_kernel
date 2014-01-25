/* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
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

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_4p5_video_qhd.h"


static struct msm_panel_info pinfo;

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db ={

		/* DSI Bit Clock at 500 MHz, 2 lane, RGB888 */
#if (defined CONFIG_PROJECT_P825F01 ) || ( defined CONFIG_PROJECT_P825A31 ) || ( defined CONFIG_PROJECT_P865F01 )|| ( defined CONFIG_PROJECT_P865V30 )//liyeqing change 20121025
	/* regulator */
	{0x03, 0x01, 0x01, 0x00},
	/* timing   */			/*447M*/
	{0x7e, 0x30, 0x12, 0x00, 0x40, 0x4b, 0x16, 0x34,
	0x15, 0x03, 0x04},
	/* phy ctrl */
	{0x7f, 0x00, 0x00, 0x00},
	/* strength */
	{0xbb, 0x02, 0x06, 0x00},
	/* pll control */
	{0x01, 0xb9, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
#elif ( defined CONFIG_PROJECT_P865F03 ) || ( defined CONFIG_PROJECT_P865F06 )
	/* regulator */
//	{0x03, 0x01, 0x01, 0x00},
	/* timing   */			/*455M*/
//	{0x81, 0x30, 0x12, 0x00, 0x41, 0x4a, 0x16, 0x34,
//	0x16, 0x03, 0x04},
	/* phy ctrl */
//	{0x7f, 0x00, 0x00, 0x00},
	/* strength */
//	{0xbb, 0x02, 0x06, 0x00},
	/* pll control */
//	{0x01, 0xc1, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
//	0x01, 0x0f, 0x07,
//	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},

	/* regulator */
	{0x03, 0x01, 0x01, 0x00},
	/* timing   */			/*493M*/
	{0x89, 0x33, 0x14, 0x00, 0x44, 0x4e, 0x18, 0x36,
	0x18, 0x03, 0x04},
	/* phy ctrl */
	{0x7f, 0x00, 0x00, 0x00},
	/* strength */
	{0xbb, 0x02, 0x06, 0x00},
	/* pll control */
	{0x01, 0xe6, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
#else  //liyeqing change 20121025, old 
	/* regulator */
	{0x03, 0x01, 0x01, 0x00},
	/* timing   */			/*420M*/
	{0x79, 0x2e, 0x10, 0x00, 0x3e, 0x45, 0x15, 0x32,
	0x14, 0x03, 0x04},
	/* phy ctrl */
	{0x7f, 0x00, 0x00, 0x00},
	/* strength */
	{0xbb, 0x02, 0x06, 0x00},
	/* pll control */
	{0x01, 0x9e, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
#endif
};


static int __init mipi_4p5_video_qhd_pt_init(void)
{
	int ret;


	if (msm_fb_detect_client("video_qhd_pt"))
		return 0;

printk("wangminrong the lcd is video_qhd_pt\r\n");
	pinfo.xres = 540;
	pinfo.yres = 960;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 100;//100
	pinfo.lcdc.h_front_porch = 100;//100

	pinfo.lcdc.h_pulse_width = 10;
	pinfo.lcdc.v_back_porch = 26;//26;//lead panel must use 26
	pinfo.lcdc.v_front_porch = 10;
	pinfo.lcdc.v_pulse_width = 10;
	pinfo.clk_rate = 420000000;//499000000;

	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 32;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;

	pinfo.mipi.mode = DSI_VIDEO_MODE;
	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.hfp_power_stop = TRUE;
	pinfo.mipi.hbp_power_stop = TRUE;
	pinfo.mipi.hsa_power_stop = TRUE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;

	pinfo.mipi.traffic_mode = DSI_BURST_MODE;
	pinfo.mipi.dst_format =  DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.t_clk_post = 0x20;
	pinfo.mipi.t_clk_pre = 0x2F;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo.mipi.dlane_swap = 0x01;
	pinfo.mipi.tx_eot_append = 0x01;


	ret = mipi_4p5_video_qhd_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_QHD_PT);
	if (ret)
		printk(KERN_ERR "%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_4p5_video_qhd_pt_init);
