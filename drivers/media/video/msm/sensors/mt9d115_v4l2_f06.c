/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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

#include "msm_sensor.h"
#include "msm.h"
#include <linux/proc_fs.h>  //wangjianping add sensor  id
#define SENSOR_NAME "mt9d115"
#define PLATFORM_DRIVER_NAME "msm_camera_mt9d115"
#define mt9d115_obj mt9d115_##obj

#define CONFIG_CAMERA_MT9D115_FLIP_MIRROR_ZTE   //zhangzhao for v72 2013-2-6

DEFINE_MUTEX(mt9d115_mut);
static struct msm_sensor_ctrl_t mt9d115_s_ctrl;

//#define _LEN_CORRECTION_85_
//#define _LEN_CORRECTION_90_
//#define _LEN_CORRECTION_100_
#define LENS_2 1

//24M  -250M mipi clk
//12M  -125M mipi clk
static struct msm_camera_i2c_reg_conf reset_sensor_setting1[] = {
    {0x001A, 0x0051,}, // RESET_AND_MISC_CONTROL
    {0x001A, 0x0050,}, // RESET_AND_MISC_CONTROL
};

static struct msm_camera_i2c_reg_conf reset_sensor_setting2[] = {
    {0x001A, 0x0058,}, // RESET_AND_MISC_CONTROL
};

static struct msm_camera_i2c_reg_conf  mt9d115_pll_settings[] = {
    {0x0014, 0x21F9,}, 	// PLL_CONTROL
    {0x0010, 0x073f,}, 	// PLL_DIVIDERS   0X0115    zhangzhao modified for V72A
    {0x0012, 0x00F5,}, 	// PLL_P_DIVIDERS
    {0x0014, 0x2545,}, 	// PLL_CONTROL
    {0x0014, 0x2547,}, 	// PLL_CONTROL
    {0x0014, 0x2447,}, 	// PLL_CONTROL
};
static struct msm_camera_i2c_reg_conf  mt9d115_settings[] = {
    {0x0014, 0x2047,}, 	// PLL_CONTROL
    {0x0014, 0x2046,}, 	// PLL_CONTROL
    {0x0018, 0x402D,}, 	// STANDBY_CONTROL
    {0x0018, 0x402C,}, 	// STANDBY_CONTROL
};

static struct msm_camera_i2c_reg_conf preview_mode_setting[] = {
    {0x098C, 0xA115,},     // MCU_ADDRESS [SEQ_CAP_MODE]
    {0x0990, 0x0000,},     // MCU_DATA_0
    {0x098C, 0xA103,},     // MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0001,},     // MCU_DATA_0
};

static struct msm_camera_i2c_reg_conf capture_mode_setting[] = {
    {0x098C, 0xA115,},     // MCU_ADDRESS [SEQ_CAP_MODE]
    {0x0990, 0x0002,},     // MCU_DATA_0
    {0x098C, 0xA103,},     // MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0002,},     // MCU_DATA_0
};

static struct msm_camera_i2c_reg_conf software_standby_disable[] = {
   {0x0018, 0x0028,},
};

static struct msm_camera_i2c_reg_conf refresh_setting1[] = {
    {0x098C, 0xA103,},
    {0x0990, 0x0006,},     
};

static struct msm_camera_i2c_reg_conf mt9d115_refresh[] = {
    {0x098C, 0xA103,},          
    {0x0990, 0x0005,},		
};

static struct msm_camera_i2c_reg_conf mt9d115_start_settings[] = {
    //{0x0018, 0x0028,},
};

static struct msm_camera_i2c_reg_conf mt9d115_stop_settings[] = {
    //{0x0018, 0x0029,},
};

static struct msm_camera_i2c_reg_conf mt9d115_init_fine_tune[] = {

//[Lens Correction 90% 06/25/13 16:31:14]
#ifdef LENS_1 
{0x3210, 0x01b8},
{0x364E, 0x0590},
{0x3650, 0x3EAC},
{0x3652, 0x4B11},
{0x3654, 0x976F},
{0x3656, 0xA1D2},
{0x3658, 0x0130},
{0x365A, 0x59ED},
{0x365C, 0x5971},
{0x365E, 0xDAEF},
{0x3660, 0x87B1},
{0x3662, 0x0170},
{0x3664, 0x0A4D},
{0x3666, 0x44D1},
{0x3668, 0x89D0},
{0x366A, 0xABB2},
{0x366C, 0x0110},
{0x366E, 0x108D},
{0x3670, 0x4AF1},
{0x3672, 0xA56F},
{0x3674, 0xA212},
{0x3676, 0x65EC},
{0x3678, 0x8590},
{0x367A, 0x0490},
{0x367C, 0x19D2},
{0x367E, 0xE4D2},
{0x3680, 0x1DAD},
{0x3682, 0x2DEF},
{0x3684, 0x9AEE},
{0x3686, 0xBEB1},
{0x3688, 0x96D2},
{0x368A, 0xC6AC},
{0x368C, 0xBF8F},
{0x368E, 0x7E4E},
{0x3690, 0x1172},
{0x3692, 0x8212},
{0x3694, 0x536C},
{0x3696, 0x5C2F},
{0x3698, 0xC7AD},
{0x369A, 0x83B2},
{0x369C, 0x9DD2},
{0x369E, 0x4672},
{0x36A0, 0x8811},
{0x36A2, 0x98B4},
{0x36A4, 0x1C91},
{0x36A6, 0x00D3},
{0x36A8, 0x4CF2},
{0x36AA, 0x22ED},
{0x36AC, 0x5BAE},
{0x36AE, 0x8A34},
{0x36B0, 0xFB96},
{0x36B2, 0x37D2},
{0x36B4, 0xCDF0},
{0x36B6, 0xA594},
{0x36B8, 0x7DF0},
{0x36BA, 0x4C74},
{0x36BC, 0x47D2},
{0x36BE, 0xCB10},
{0x36C0, 0x9EB4},
{0x36C2, 0x8831},
{0x36C4, 0x1EF3},
{0x36C6, 0xEE2F},
{0x36C8, 0x78D1},
{0x36CA, 0xD933},
{0x36CC, 0xABD1},
{0x36CE, 0x1037},
{0x36D0, 0x284C},
{0x36D2, 0xE630},
{0x36D4, 0x84D1},
{0x36D6, 0xDB33},
{0x36D8, 0x73D6},
{0x36DA, 0x62EF},
{0x36DC, 0x73B1},
{0x36DE, 0xDB52},
{0x36E0, 0xD2B4},
{0x36E2, 0x1976},
{0x36E4, 0xEECF},
{0x36E6, 0x9E12},
{0x36E8, 0xC4D3},
{0x36EA, 0x08F4},
{0x36EC, 0x0CB7},
{0x36EE, 0x96D4},
{0x36F0, 0x4E12},
{0x36F2, 0xC874},
{0x36F4, 0x2714},
{0x36F6, 0x2459},
{0x36F8, 0x9234},
{0x36FA, 0x9354},
{0x36FC, 0xAFF7},
{0x36FE, 0x61D7},
{0x3700, 0x351A},
{0x3702, 0xB7D4},
{0x3704, 0x0F71},
{0x3706, 0x0214},
{0x3708, 0x08F6},
{0x370A, 0x4278},
{0x370C, 0x9314},
{0x370E, 0x5DEF},
{0x3710, 0xBA14},
{0x3712, 0x4675},
{0x3714, 0x12D9},
{0x3644, 0x0344},
{0x3642, 0x0240},
{0x3210, 0x01b8},
#endif

#ifdef LENS_2
//[Lens Correction 95% 06/25/13 16:31:22]
{0x3210, 0x01b0},
{0x364E, 0x0590},
{0x3650, 0x522C},
{0x3652, 0x60B1},
{0x3654, 0x83CF},
{0x3656, 0xF591},
{0x3658, 0x0130},
{0x365A, 0x64CD},
{0x365C, 0x6D91},
{0x365E, 0xC00F},
{0x3660, 0xA68F},
{0x3662, 0x0170},
{0x3664, 0x144D},
{0x3666, 0x5A71},
{0x3668, 0x8210},
{0x366A, 0x8AD2},
{0x366C, 0x0110},
{0x366E, 0x1B2D},
{0x3670, 0x6011},
{0x3672, 0x900F},
{0x3674, 0xF951},
{0x3676, 0x502C},
{0x3678, 0x82F0},
{0x367A, 0x662F},
{0x367C, 0x1032},
{0x367E, 0xC6F2},
{0x3680, 0x12ED},
{0x3682, 0x32CF},
{0x3684, 0xFD8E},
{0x3686, 0xB651},
{0x3688, 0xC7B1},
{0x368A, 0xD3EC},
{0x368C, 0xC3AF},
{0x368E, 0x45AD},
{0x3690, 0x19D2},
{0x3692, 0x8E51},
{0x3694, 0x3B4C},
{0x3696, 0x5FAF},
{0x3698, 0xA7EE},
{0x369A, 0xFCF1},
{0x369C, 0x87D2},
{0x369E, 0x5512},
{0x36A0, 0xEF30},
{0x36A2, 0xF5D3},
{0x36A4, 0x0051},
{0x36A6, 0xD931},
{0x36A8, 0x59F2},
{0x36AA, 0x49CE},
{0x36AC, 0x6611},
{0x36AE, 0x87B4},
{0x36B0, 0x8077},
{0x36B2, 0x4572},
{0x36B4, 0xA630},
{0x36B6, 0x8E14},
{0x36B8, 0x538F},
{0x36BA, 0x1DF4},
{0x36BC, 0x5672},
{0x36BE, 0xB330},
{0x36C0, 0x88B4},
{0x36C2, 0x8991},
{0x36C4, 0x5DB2},
{0x36C6, 0xF2EF},
{0x36C8, 0x3231},
{0x36CA, 0xEB33},
{0x36CC, 0x7092},
{0x36CE, 0x1857},
{0x36D0, 0x022D},
{0x36D2, 0x86D1},
{0x36D4, 0xD6B0},
{0x36D6, 0xD8D3},
{0x36D8, 0x5C96},
{0x36DA, 0x538F},
{0x36DC, 0x5BF1},
{0x36DE, 0xC312},
{0x36E0, 0xD154},
{0x36E2, 0x0416},
{0x36E4, 0xE5EF},
{0x36E6, 0xA4D2},
{0x36E8, 0xEA33},
{0x36EA, 0x03F4},
{0x36EC, 0x1817},
{0x36EE, 0x8B54},
{0x36F0, 0x4F51},
{0x36F2, 0xB275},
{0x36F4, 0x4C94},
{0x36F6, 0x4319},
{0x36F8, 0x84B4},
{0x36FA, 0xA154},
{0x36FC, 0xAFB7},
{0x36FE, 0x5297},
{0x3700, 0x2EDA},
{0x3702, 0xACF4},
{0x3704, 0x8291},
{0x3706, 0xB36E},
{0x3708, 0x34F6},
{0x370A, 0x7018},
{0x370C, 0x8934},
{0x370E, 0xBDAD},
{0x3710, 0xD174},
{0x3712, 0x0DF5},
{0x3714, 0x0C99},
{0x3644, 0x0344},
{0x3642, 0x0240},
{0x3210, 0x01b8},
#endif

#ifdef LENS_3
//[Lens Correction 95% 06/25/13 16:51:57]
{0x3210, 0x01b0},
{0x364E, 0x0590},
{0x3650, 0x83ED},
{0x3652, 0x6551},
{0x3654, 0x27EE},
{0x3656, 0x9C12},
{0x3658, 0x00D0},
{0x365A, 0x6CC6},
{0x365C, 0x7751},
{0x365E, 0xB2CE},
{0x3660, 0x8D11},
{0x3662, 0x00F0},
{0x3664, 0xCC6B},
{0x3666, 0x5971},
{0x3668, 0xBA8E},
{0x366A, 0x8C92},
{0x366C, 0x00D0},
{0x366E, 0xBE6B},
{0x3670, 0x6351},
{0x3672, 0x4EEC},
{0x3674, 0x96F2},
{0x3676, 0x300D},
{0x3678, 0xF9AF},
{0x367A, 0xF74F},
{0x367C, 0x0B32},
{0x367E, 0xF36F},
{0x3680, 0x366D},
{0x3682, 0x530F},
{0x3684, 0xE46F},
{0x3686, 0xE771},
{0x3688, 0xCC31},
{0x368A, 0x894C},
{0x368C, 0xAAAF},
{0x368E, 0xB230},
{0x3690, 0x0772},
{0x3692, 0x1870},
{0x3694, 0x658C},
{0x3696, 0x00F0},
{0x3698, 0xB1D0},
{0x369A, 0xA152},
{0x369C, 0x754F},
{0x369E, 0x53F2},
{0x36A0, 0x982F},
{0x36A2, 0xC593},
{0x36A4, 0x5930},
{0x36A6, 0xED94},
{0x36A8, 0x5A52},
{0x36AA, 0xEC2D},
{0x36AC, 0x14B2},
{0x36AE, 0xF6AF},
{0x36B0, 0xE196},
{0x36B2, 0x43F2},
{0x36B4, 0xACEE},
{0x36B6, 0xA713},
{0x36B8, 0xB26F},
{0x36BA, 0xB834},
{0x36BC, 0x5412},
{0x36BE, 0xF46F},
{0x36C0, 0xBAF3},
{0x36C2, 0x652F},
{0x36C4, 0x8655},
{0x36C6, 0x8EF1},
{0x36C8, 0x0592},
{0x36CA, 0xE453},
{0x36CC, 0xEA91},
{0x36CE, 0x71F6},
{0x36D0, 0xCA2F},
{0x36D2, 0xB031},
{0x36D4, 0xDF93},
{0x36D6, 0xF572},
{0x36D8, 0x1057},
{0x36DA, 0x9E4E},
{0x36DC, 0x43D1},
{0x36DE, 0xD792},
{0x36E0, 0x8BD4},
{0x36E2, 0x4DD5},
{0x36E4, 0x8151},
{0x36E6, 0xA332},
{0x36E8, 0xE973},
{0x36EA, 0x2974},
{0x36EC, 0x62B6},
{0x36EE, 0xE0F3},
{0x36F0, 0x3093},
{0x36F2, 0xB577},
{0x36F4, 0xECF6},
{0x36F6, 0x563A},
{0x36F8, 0xC8D3},
{0x36FA, 0xAE2E},
{0x36FC, 0x8F38},
{0x36FE, 0xF473},
{0x3700, 0x65DA},
{0x3702, 0x8574},
{0x3704, 0x33D2},
{0x3706, 0xA5D7},
{0x3708, 0xE394},
{0x370A, 0x2D7A},
{0x370C, 0xD753},
{0x370E, 0x26B3},
{0x3710, 0xB937},
{0x3712, 0xE116},
{0x3714, 0x4D3A},
{0x3644, 0x0308},
{0x3642, 0x0264},
{0x3210, 0x01b8},
#endif


#ifdef LENS_4
//[Lens Correction 85% 06/25/13 16:52:10]
{0x3210, 0x01b0},
{0x364E, 0x05D0},
{0x3650, 0xCECC},
{0x3652, 0x3A31},
{0x3654, 0x2D4E},
{0x3656, 0xE752},
{0x3658, 0x00D0},
{0x365A, 0x582A},
{0x365C, 0x4D91},
{0x365E, 0xB34E},
{0x3660, 0x9F32},
{0x3662, 0x0130},
{0x3664, 0xC42A},
{0x3666, 0x2F11},
{0x3668, 0xB16E},
{0x366A, 0xD032},
{0x366C, 0x00F0},
{0x366E, 0xABEA},
{0x3670, 0x3A31},
{0x3672, 0x250C},
{0x3674, 0xE132},
{0x3676, 0x2C2D},
{0x3678, 0x8970},
{0x367A, 0xA30F},
{0x367C, 0x2C32},
{0x367E, 0xF751},
{0x3680, 0x3A4D},
{0x3682, 0x2CCF},
{0x3684, 0xD74F},
{0x3686, 0xB971},
{0x3688, 0xE0D1},
{0x368A, 0x9ECC},
{0x368C, 0xC4EF},
{0x368E, 0xB3AF},
{0x3690, 0x2892},
{0x3692, 0xC191},
{0x3694, 0x588C},
{0x3696, 0x592F},
{0x3698, 0x81F0},
{0x369A, 0x84B2},
{0x369C, 0xD210},
{0x369E, 0x4232},
{0x36A0, 0xF2EF},
{0x36A2, 0xD034},
{0x36A4, 0x5B32},
{0x36A6, 0x1CD4},
{0x36A8, 0x4892},
{0x36AA, 0x936F},
{0x36AC, 0xAFD3},
{0x36AE, 0x0C72},
{0x36B0, 0xD515},
{0x36B2, 0x3392},
{0x36B4, 0xC6EF},
{0x36B6, 0xB794},
{0x36B8, 0x1792},
{0x36BA, 0x0CD4},
{0x36BC, 0x42D2},
{0x36BE, 0x9AD0},
{0x36C0, 0xC9D4},
{0x36C2, 0x3852},
{0x36C4, 0x6193},
{0x36C6, 0x96D1},
{0x36C8, 0x5A12},
{0x36CA, 0x9734},
{0x36CC, 0xD754},
{0x36CE, 0x3757},
{0x36D0, 0x8B90},
{0x36D2, 0xE1AF},
{0x36D4, 0xDA53},
{0x36D6, 0x9054},
{0x36D8, 0x1977},
{0x36DA, 0xF88D},
{0x36DC, 0x31D2},
{0x36DE, 0x8AB4},
{0x36E0, 0x9D75},
{0x36E2, 0x0B97},
{0x36E4, 0x82D1},
{0x36E6, 0xBD31},
{0x36E8, 0x9DD4},
{0x36EA, 0x2512},
{0x36EC, 0x2F77},
{0x36EE, 0xA0F4},
{0x36F0, 0x7933},
{0x36F2, 0x8476},
{0x36F4, 0xA7D7},
{0x36F6, 0x117A},
{0x36F8, 0x94D4},
{0x36FA, 0x4291},
{0x36FC, 0x97B7},
{0x36FE, 0xEA75},
{0x3700, 0x0EFA},
{0x3702, 0xB214},
{0x3704, 0x2213},
{0x3706, 0xF4F5},
{0x3708, 0x9696},
{0x370A, 0x6859},
{0x370C, 0x9914},
{0x370E, 0x4A53},
{0x3710, 0xA036},
{0x3712, 0x92D7},
{0x3714, 0x11BA},
{0x3644, 0x0308},
{0x3642, 0x0264},
{0x3210, 0x01b8},
#endif
          
    {0x098C, 0xAB22,},    // MCU_ADDRESS
    {0x0990, 0x0003,},    // MCU_DATA_0
    {0x098C, 0xA20C,},     // MCU_ADDRESS [AE_MAX_INDEX]
    {0x0990, 0x000a,},     // MCU_DATA_0
    {0x098C, 0xA215,},     // MCU_ADDRESS [AE_INDEX_TH23]
    {0x0990, 0x0006,},     // MCU_DATA_0
    {0x098C, 0xAB20,},     // MCU_ADDRESS [HG_LL_SAT1]
    {0x0990, 0x006E,}, 	// MCU_DATA_0 // ZHANGZHAO FOR SATURATION
    {0x098C, 0xA34A,}, 	// MCU_ADDRESS [AWB_GAIN_MIN]
    {0x0990, 0x0059,}, 	// MCU_DATA_0
    {0x098C, 0xA34B,}, 	// MCU_ADDRESS [AWB_GAIN_MAX]
    {0x0990, 0x00B6,}, 	// MCU_DATA_0
    {0x098C, 0xA34C,}, 	// MCU_ADDRESS [AWB_GAINMIN_B]
    {0x0990, 0x0059,}, 	// MCU_DATA_0
    {0x098C, 0xA34D,}, 	// MCU_ADDRESS [AWB_GAINMAX_B]
    {0x0990, 0x00B5,}, 	// MCU_DATA_0
    {0x098C, 0xA351,}, 	// MCU_ADDRESS [AWB_CCM_POSITION_MIN]
    {0x0990, 0x0000,}, 	// MCU_DATA_0
    {0x098C, 0xA352,}, 	// MCU_ADDRESS [AWB_CCM_POSITION_MAX]
    {0x0990, 0x007F,}, 	// MCU_DATA_0
    {0x098C, 0xAB2C,},     // MCU_ADDRESS [HG_NR_START_R]
    {0x0990, 0x0001,},     // MCU_DATA_0
    {0x098C, 0xAB2D,},     // MCU_ADDRESS [HG_NR_START_G]
    {0x0990, 0x0001,},     // MCU_DATA_0
    {0x098C, 0xAB2E,},     // MCU_ADDRESS [HG_NR_START_B]
    {0x0990, 0x0001,},     // MCU_DATA_0
    {0x098C, 0xAB2F,},     // MCU_ADDRESS [HG_NR_START_OL]
    {0x0990, 0x0001,},     // MCU_DATA_0
    {0x098C, 0xA366,},  	// MCU_ADDRESS [AWB_KR_L]
    {0x0990, 0x0078,},  	// MCU_DATA_0
    {0x098C, 0x2306,},  	// MCU_ADDRESS [AWB_CCM_L_0]
    {0x0990, 0x01B1,},  	// MCU_DATA_0
    {0x098C, 0x231C,},  	// MCU_ADDRESS [AWB_CCM_RL_0]
    {0x0990, 0x0028,},  	// MCU_DATA_0
    {0x098C, 0x2308,},  	// MCU_ADDRESS [AWB_CCM_L_1]
    {0x0990, 0xFFA2,},  	// MCU_DATA_0
    {0x098C, 0x231E,},  	// MCU_ADDRESS [AWB_CCM_RL_1]
    {0x0990, 0xFFE5,},  	// MCU_DATA_0
    {0x098C, 0x230A,},  	// MCU_ADDRESS [AWB_CCM_L_2]
    {0x0990, 0xFFAD,},  	// MCU_DATA_0
    {0x098C, 0x2320,},  	// MCU_ADDRESS [AWB_CCM_RL_2]
    {0x0990, 0xFFF3,},  	// MCU_DATA_0
    {0x098C, 0x230C,},  	// MCU_ADDRESS [AWB_CCM_L_3]
    {0x0990, 0xFF87,},  	// MCU_DATA_0
    {0x098C, 0x2322,},  	// MCU_ADDRESS [AWB_CCM_RL_3]
    {0x0990, 0xFFF6,},  	// MCU_DATA_0
    {0x098C, 0x230E,},  	// MCU_ADDRESS [AWB_CCM_L_4]
    {0x0990, 0x0184,},  	// MCU_DATA_0
    {0x098C, 0x2324,},  	// MCU_ADDRESS [AWB_CCM_RL_4]
    {0x0990, 0x0026,},  	// MCU_DATA_0
    {0x098C, 0x2310,},  	// MCU_ADDRESS [AWB_CCM_L_5]
    {0x0990, 0xFFF5,},  	// MCU_DATA_0
    {0x098C, 0x2326,},  	// MCU_ADDRESS [AWB_CCM_RL_5]
    {0x0990, 0xFFE4,},  	// MCU_DATA_0
    {0x098C, 0x2312,},  	// MCU_ADDRESS [AWB_CCM_L_6]
    {0x0990, 0xFFBF,},  	// MCU_DATA_0
    {0x098C, 0x2328,},  	// MCU_ADDRESS [AWB_CCM_RL_6]
    {0x0990, 0x001C,},  	// MCU_DATA_0
    {0x098C, 0x2314,},  	// MCU_ADDRESS [AWB_CCM_L_7]
    {0x0990, 0xFF4F,},  	// MCU_DATA_0
    {0x098C, 0x232A,},  	// MCU_ADDRESS [AWB_CCM_RL_7]
    {0x0990, 0x0011,},  	// MCU_DATA_0
    {0x098C, 0x2316,},  	// MCU_ADDRESS [AWB_CCM_L_8]
    {0x0990, 0x01F3,},  	// MCU_DATA_0
    {0x098C, 0x232C,},  	// MCU_ADDRESS [AWB_CCM_RL_8]
    {0x0990, 0xFFD3,},  	// MCU_DATA_0
    {0x098C, 0xAB30,},  // MCU_ADDRESS [HG_NR_STOP_R]
    {0x0990, 0x001E,},    // MCU_DATA_0 //0X001E 20111024
    {0x098C, 0xAB31,},  // MCU_ADDRESS [HG_NR_STOP_G]
    {0x0990, 0x001E,},    // MCU_DATA_0 //0X001E 20111024
    {0x098C, 0xAB32,},  // MCU_ADDRESS [HG_NR_STOP_B]
    {0x0990, 0x001E,},    // MCU_DATA_0 //0X001E 20111024
    {0x098C, 0xAB33,},  // MCU_ADDRESS [HG_NR_STOP_OL]
    {0x0990, 0x001E,},    // MCU_DATA_0 //0X001E 20111024 
    {0x098C, 0xAB34,},  // MCU_ADDRESS [HG_NR_GAINSTART] 
    {0x0990, 0x000E,},    // MCU_DATA_0 
    {0x098C, 0xAB35,},  // MCU_ADDRESS [HG_NR_GAINSTOP]
    {0x0990, 0x0091,},    // MCU_DATA_0
    {0x0018, 0x0028,},  	// STANDBY_CONTROL
    {0x33F4, 0x005B,},     // MCU_DATA_0 //default 4b
    {0x3404, 0x0080,}, 
    {0x098C, 0xA118,}, 	// MCU_ADDRESS [SEQ_PREVIEW_0_FD]        
    {0x0990, 0x0002,}, 	// MCU_DATA_0                                 
    {0x098C, 0xA11E,}, 	// MCU_ADDRESS [SEQ_PREVIEW_1_FD]             
    {0x0990, 0x0002,}, 	// MCU_DATA_0                                 
    {0x098C, 0xA124,}, 	// MCU_ADDRESS [SEQ_PREVIEW_2_FD]             
    {0x0990, 0x0002,}, 	// MCU_DATA_0                                     
    {0x098C, 0xA12A,},  	// MCU_ADDRESS [SEQ_PREVIEW_3_FD]            
    {0x0990, 0x0002,},  	// MCU_DATA_0                                  
    {0x098C, 0xA404,},  	// MCU_ADDRESS [FD_MODE]                       
    {0x0990, 0x00E0,},  	// MCU_DATA_0                               
    {0x098C, 0xA103,},           
    {0x0990, 0x0005,},  
};
static struct msm_camera_i2c_reg_conf mt9d115_init_fine_tune_end[] = {		               
    {0x3404, 0x0080,}, 
    {0x098C, 0xA118,}, 	// MCU_ADDRESS [SEQ_PREVIEW_0_FD]          
    {0x0990, 0x0001,}, 	// MCU_DATA_0                              
    {0x098C, 0xA11E,}, 	// MCU_ADDRESS [SEQ_PREVIEW_1_FD]         
    {0x0990, 0x0001,}, 	// MCU_DATA_0                            
    {0x098C, 0xA124,}, 	// MCU_ADDRESS [SEQ_PREVIEW_2_FD]         
    {0x0990, 0x0000,}, 	// MCU_DATA_0                                  
    {0x098C, 0xA12A,}, 	// MCU_ADDRESS [SEQ_PREVIEW_3_FD]          
    {0x0990, 0x0001,}, 	// MCU_DATA_0  
    {0x098C, 0xA404,}, 	// MCU_ADDRESS [FD_MODE]                      
    {0x0990, 0x0060,}, 	// MCU_DATA_0
    {0x098C, 0xA215,}, 	// MCU_ADDRESS [AE_INDEX_TH23]
    {0x0990, 0x0004,},//c 	// MCU_DATA_0	
    {0x098C, 0xA20C,}, 	// MCU_ADDRESS [AE_MAX_INDEX]
    {0x0990, 0x000B,},//12 	// MCU_DATA_0 
    {0x3404, 0x0080,}, 
    {0x0018, 0x0028,},
                   
};                  

static struct msm_camera_i2c_reg_conf mt9d115_recommend_settings[] = {
    
//800x600
    {0x098C, 0x2703,}, 	// MCU_ADDRESS [MODE_OUTPUT_WIDTH_A]
    {0x0990, 0x0320,}, 	// MCU_DATA_0
    {0x098C, 0x2705,}, 	// MCU_ADDRESS [MODE_OUTPUT_HEIGHT_A]
    {0x0990, 0x0258,}, 	// MCU_DATA_0

//1024x768
//    {0x098C, 0x2703,}, 	// MCU_ADDRESS [MODE_OUTPUT_WIDTH_A]
//    {0x0990, 0x0400,}, 	// MCU_DATA_0
//    {0x098C, 0x2705,}, 	// MCU_ADDRESS [MODE_OUTPUT_HEIGHT_A]
//    {0x0990, 0x0300,}, 	// MCU_DATA_0

//640x480
//    {0x098C, 0x2703,}, 	// MCU_ADDRESS [MODE_OUTPUT_WIDTH_A]
//    {0x0990, 0x0280,}, 	// MCU_DATA_0
//    {0x098C, 0x2705,}, 	// MCU_ADDRESS [MODE_OUTPUT_HEIGHT_A]
//    {0x0990, 0x01E0,}, 	// MCU_DATA_0

    {0x098C, 0x2707,}, 	// MCU_ADDRESS [MODE_OUTPUT_WIDTH_B]
    {0x0990, 0x0640,}, 	// MCU_DATA_0
    {0x098C, 0x2709,}, 	// MCU_ADDRESS [MODE_OUTPUT_HEIGHT_B]
    {0x0990, 0x04B0,}, 	// MCU_DATA_0
    {0x098C, 0x270D,}, 	// MCU_ADDRESS [MODE_SENSOR_ROW_START_A]
    {0x0990, 0x0000,}, 	// MCU_DATA_0
    {0x098C, 0x270F,}, 	// MCU_ADDRESS [MODE_SENSOR_COL_START_A]
    {0x0990, 0x0000,}, 	// MCU_DATA_0
    {0x098C, 0x2711,}, 	// MCU_ADDRESS [MODE_SENSOR_ROW_END_A]
    {0x0990, 0x04BD,}, 	// MCU_DATA_0
    {0x098C, 0x2713,}, 	// MCU_ADDRESS [MODE_SENSOR_COL_END_A]
    {0x0990, 0x064D,}, 	// MCU_DATA_0
    {0x098C, 0x2715,}, 	// MCU_ADDRESS [MODE_SENSOR_ROW_SPEED_A]
    {0x0990, 0x0111,}, 	// MCU_DATA_0
    {0x098C, 0x2717,}, 	// MCU_ADDRESS [MODE_SENSOR_READ_MODE_A]
    {0x0990, 0x046C,}, 	// MCU_DATA_0
    {0x098C, 0x2719,}, 	// MCU_ADDRESS [MODE_SENSOR_FINE_CORRECTION_A]
    {0x0990, 0x005A,}, 	// MCU_DATA_0
    {0x098C, 0x271B,}, 	// MCU_ADDRESS [MODE_SENSOR_FINE_IT_MIN_A]
    {0x0990, 0x01BE,}, 	// MCU_DATA_0
    {0x098C, 0x271D,}, 	// MCU_ADDRESS [MODE_SENSOR_FINE_IT_MAX_MARGIN_A]
    {0x0990, 0x0131,}, 	// MCU_DATA_0
    {0x098C, 0x271F,}, 	// MCU_ADDRESS [MODE_SENSOR_FRAME_LENGTH_A]
    {0x0990, 0x02BB,}, 	// MCU_DATA_0
    {0x098C, 0x2721,}, 	// MCU_ADDRESS [MODE_SENSOR_LINE_LENGTH_PCK_A]
    {0x0990, 0x0888,}, 	// MCU_DATA_0
    {0x098C, 0x2723,}, 	// MCU_ADDRESS [MODE_SENSOR_ROW_START_B]
    {0x0990, 0x0004,}, 	// MCU_DATA_0
    {0x098C, 0x2725,}, 	// MCU_ADDRESS [MODE_SENSOR_COL_START_B]
    {0x0990, 0x0004,}, 	// MCU_DATA_0
    {0x098C, 0x2727,}, 	// MCU_ADDRESS [MODE_SENSOR_ROW_END_B]
    {0x0990, 0x04BB,}, 	// MCU_DATA_0
    {0x098C, 0x2729,}, 	// MCU_ADDRESS [MODE_SENSOR_COL_END_B]
    {0x0990, 0x064B,}, 	// MCU_DATA_0
    {0x098C, 0x272B,}, 	// MCU_ADDRESS [MODE_SENSOR_ROW_SPEED_B]
    {0x0990, 0x0111,}, 	// MCU_DATA_0
    {0x098C, 0x272D,}, 	// MCU_ADDRESS [MODE_SENSOR_READ_MODE_B]
    {0x0990, 0x0024,}, 	// MCU_DATA_0
    {0x098C, 0x272F,}, 	// MCU_ADDRESS [MODE_SENSOR_FINE_CORRECTION_B]
    {0x0990, 0x003A,}, 	// MCU_DATA_0
    {0x098C, 0x2731,}, 	// MCU_ADDRESS [MODE_SENSOR_FINE_IT_MIN_B]
    {0x0990, 0x00F6,}, 	// MCU_DATA_0
    {0x098C, 0x2733,}, 	// MCU_ADDRESS [MODE_SENSOR_FINE_IT_MAX_MARGIN_B]
    {0x0990, 0x008B,}, 	// MCU_DATA_0
    {0x098C, 0x2735,}, 	// MCU_ADDRESS [MODE_SENSOR_FRAME_LENGTH_B]
    {0x0990, 0x0521,}, 	// MCU_DATA_0
    {0x098C, 0x2737,}, 	// MCU_ADDRESS [MODE_SENSOR_LINE_LENGTH_PCK_B]
    {0x0990, 0x0888,}, 	// MCU_DATA_0
    {0x098C, 0x2739,}, 	// MCU_ADDRESS [MODE_CROP_X0_A]
    {0x0990, 0x0000,}, 	// MCU_DATA_0
    {0x098C, 0x273B,}, 	// MCU_ADDRESS [MODE_CROP_X1_A]
    {0x0990, 0x031F,}, 	// MCU_DATA_0
    {0x098C, 0x273D,}, 	// MCU_ADDRESS [MODE_CROP_Y0_A]
    {0x0990, 0x0000,}, 	// MCU_DATA_0
    {0x098C, 0x273F,}, 	// MCU_ADDRESS [MODE_CROP_Y1_A]
    {0x0990, 0x0257,}, 	// MCU_DATA_0
    {0x098C, 0x2747,}, 	// MCU_ADDRESS [MODE_CROP_X0_B]
    {0x0990, 0x0000,}, 	// MCU_DATA_0
    {0x098C, 0x2749,}, 	// MCU_ADDRESS [MODE_CROP_X1_B]
    {0x0990, 0x063F,}, 	// MCU_DATA_0
    {0x098C, 0x274B,}, 	// MCU_ADDRESS [MODE_CROP_Y0_B]
    {0x0990, 0x0000,}, 	// MCU_DATA_0
    {0x098C, 0x274D,}, 	// MCU_ADDRESS [MODE_CROP_Y1_B]
    {0x0990, 0x04AF,}, 	// MCU_DATA_0
    {0x098C, 0x2222,}, 	// MCU_ADDRESS [AE_R9]
    {0x0990, 0x00A0,}, 	// MCU_DATA_0
    {0x098C, 0xA408,}, 	// MCU_ADDRESS [FD_SEARCH_F1_50]
    {0x0990, 0x0026,}, 	// MCU_DATA_0
    {0x098C, 0xA409,}, 	// MCU_ADDRESS [FD_SEARCH_F2_50]
    {0x0990, 0x0029,}, 	// MCU_DATA_0
    {0x098C, 0xA40A,}, 	// MCU_ADDRESS [FD_SEARCH_F1_60]
    {0x0990, 0x002E,}, 	// MCU_DATA_0
    {0x098C, 0xA40B,}, 	// MCU_ADDRESS [FD_SEARCH_F2_60]
    {0x0990, 0x0031,}, 	// MCU_DATA_0
    {0x098C, 0x2411,}, 	// MCU_ADDRESS [FD_R9_STEP_F60_A]
    {0x0990, 0x00A0,}, 	// MCU_DATA_0
    {0x098C, 0x2413,}, 	// MCU_ADDRESS [FD_R9_STEP_F50_A]
    {0x0990, 0x00C0,}, 	// MCU_DATA_0
    {0x098C, 0x2415,}, 	// MCU_ADDRESS [FD_R9_STEP_F60_B]
    {0x0990, 0x00A0,}, 	// MCU_DATA_0
    {0x098C, 0x2417,}, 	// MCU_ADDRESS [FD_R9_STEP_F50_B]
    {0x0990, 0x00C0,}, 	// MCU_DATA_0
    {0x098C, 0xA404,}, 	// MCU_ADDRESS [FD_MODE]
    {0x0990, 0x0010,}, 	// MCU_DATA_0
    {0x098C, 0xA40D,}, 	// MCU_ADDRESS [FD_STAT_MIN]
    {0x0990, 0x0002,}, 	// MCU_DATA_0
    {0x098C, 0xA40E,}, 	// MCU_ADDRESS [FD_STAT_MAX]
    {0x0990, 0x0003,}, 	// MCU_DATA_0
    {0x098C, 0xA410,}, 	// MCU_ADDRESS [FD_MIN_AMPLITUDE]
    {0x0990, 0x000A,}, 	// MCU_DATA_0
    {0x098C, 0xA117,}, 	// MCU_ADDRESS [SEQ_PREVIEW_0_AE]
    {0x0990, 0x0002,}, 	// MCU_DATA_0
    {0x098C, 0xA11D,}, 	// MCU_ADDRESS [SEQ_PREVIEW_1_AE]
    {0x0990, 0x0002,}, 	// MCU_DATA_0
    {0x098C, 0xA129,}, 	// MCU_ADDRESS [SEQ_PREVIEW_3_AE]
    {0x0990, 0x0002,}, 	// MCU_DATA_0
    {0x098C, 0xA24F,}, 	// MCU_ADDRESS [AE_BASETARGET]
    {0x0990, 0x0032,}, 	// MCU_DATA_0
    {0x098C, 0xA20C,}, 	// MCU_ADDRESS [AE_MAX_INDEX]
    {0x0990, 0x0010,}, 	// MCU_DATA_0
    {0x098C, 0xA216,}, 	// MCU_ADDRESS
    {0x0990, 0x0091,}, 	// MCU_DATA_0
    {0x098C, 0xA20E,}, 	// MCU_ADDRESS [AE_MAX_VIRTGAIN]
    {0x0990, 0x0091,}, 	// MCU_DATA_0
    {0x098C, 0x2212,}, 	// MCU_ADDRESS [AE_MAX_DGAIN_AE1]
    {0x0990, 0x00A4,}, 	// MCU_DATA_0
    {0x3210, 0x01B8,}, 	// COLOR_PIPELINE_CONTROL
    {0x098C, 0xAB36,}, 	// MCU_ADDRESS [HG_CLUSTERDC_TH]
    {0x0990, 0x0014,}, 	// MCU_DATA_0
    {0x098C, 0x2B66,}, 	// MCU_ADDRESS [HG_CLUSTER_DC_BM]
    {0x0990, 0x2AF8,}, 	// MCU_DATA_0
    {0x098C, 0xAB20,}, 	// MCU_ADDRESS [HG_LL_SAT1]
    {0x0990, 0x0080,}, 	// MCU_DATA_0
    {0x098C, 0xAB24,}, 	// MCU_ADDRESS [HG_LL_SAT2]
    {0x0990, 0x007f,}, 	// MCU_DATA_0 //2013.06.24
    {0x098C, 0xAB21,}, 	// MCU_ADDRESS [HG_LL_INTERPTHRESH1]
    {0x0990, 0x000A,}, 	// MCU_DATA_0
    {0x098C, 0xAB25,}, 	// MCU_ADDRESS [HG_LL_INTERPTHRESH2]
    {0x0990, 0x002A,}, 	// MCU_DATA_0
    {0x098C, 0xAB22,}, 	// MCU_ADDRESS [HG_LL_APCORR1]
    {0x0990, 0x0007,}, 	// MCU_DATA_0
    {0x098C, 0xAB26,}, 	// MCU_ADDRESS [HG_LL_APCORR2]
    {0x0990, 0x0001,}, 	// MCU_DATA_0
    {0x098C, 0xAB23,}, 	// MCU_ADDRESS [HG_LL_APTHRESH1]
    {0x0990, 0x0004,}, 	// MCU_DATA_0
    {0x098C, 0xAB27,}, 	// MCU_ADDRESS [HG_LL_APTHRESH2]
    {0x0990, 0x0009,}, 	// MCU_DATA_0
    {0x098C, 0x2B28,}, 	// MCU_ADDRESS [HG_LL_BRIGHTNESSSTART]
    {0x0990, 0x0BB8,}, 	// MCU_DATA_0
    {0x098C, 0x2B2A,}, 	// MCU_ADDRESS [HG_LL_BRIGHTNESSSTOP]
    {0x0990, 0x2968,}, 	// MCU_DATA_0
    {0x098C, 0xAB2C,}, 	// MCU_ADDRESS [HG_NR_START_R]
    {0x0990, 0x00FF,}, 	// MCU_DATA_0
    {0x098C, 0xAB30,}, 	// MCU_ADDRESS [HG_NR_STOP_R]
    {0x0990, 0x00FF,}, 	// MCU_DATA_0
    {0x098C, 0xAB2D,}, 	// MCU_ADDRESS [HG_NR_START_G]
    {0x0990, 0x00FF,}, 	// MCU_DATA_0
    {0x098C, 0xAB31,}, 	// MCU_ADDRESS [HG_NR_STOP_G]
    {0x0990, 0x00FF,}, 	// MCU_DATA_0
    {0x098C, 0xAB2E,}, 	// MCU_ADDRESS [HG_NR_START_B]
    {0x0990, 0x00FF,}, 	// MCU_DATA_0
    {0x098C, 0xAB32,}, 	// MCU_ADDRESS [HG_NR_STOP_B]
    {0x0990, 0x00FF,}, 	// MCU_DATA_0
    {0x098C, 0xAB2F,}, 	// MCU_ADDRESS [HG_NR_START_OL]
    {0x0990, 0x000A,}, 	// MCU_DATA_0
    {0x098C, 0xAB33,}, 	// MCU_ADDRESS [HG_NR_STOP_OL]
    {0x0990, 0x0006,}, 	// MCU_DATA_0
    {0x098C, 0xAB34,}, 	// MCU_ADDRESS [HG_NR_GAINSTART]
    {0x0990, 0x0020,}, 	// MCU_DATA_0
    {0x098C, 0xAB35,}, 	// MCU_ADDRESS [HG_NR_GAINSTOP]
    {0x0990, 0x0091,}, 	// MCU_DATA_0
    {0x098C, 0xA765,}, 	// MCU_ADDRESS [MODE_COMMONMODESETTINGS_FILTER_MODE]
    {0x0990, 0x0006,}, 	// MCU_DATA_0
    {0x098C, 0xAB37,}, 	// MCU_ADDRESS [HG_GAMMA_MORPH_CTRL]
    {0x0990, 0x0003,}, 	// MCU_DATA_0
    {0x098C, 0x2B38,}, 	// MCU_ADDRESS [HG_GAMMASTARTMORPH]
    {0x0990, 0x2968,}, 	// MCU_DATA_0
    {0x098C, 0x2B3A,}, 	// MCU_ADDRESS [HG_GAMMASTOPMORPH]
    {0x0990, 0x2D50,}, 	// MCU_DATA_0
    {0x098C, 0x2B62,}, 	// MCU_ADDRESS [HG_FTB_START_BM]
    {0x0990, 0xFFFE,}, 	// MCU_DATA_0
    {0x098C, 0x2B64,}, 	// MCU_ADDRESS [HG_FTB_STOP_BM]
    {0x0990, 0xFFFF,}, 	// MCU_DATA_0
    {0x098C, 0xAB4F,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_0]
    {0x0990, 0x0000,}, 	// MCU_DATA_0
    {0x098C, 0xAB50,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_1]
    {0x0990, 0x000C,}, 	// MCU_DATA_0
    {0x098C, 0xAB51,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_2]
    {0x0990, 0x0022,}, 	// MCU_DATA_0
    {0x098C, 0xAB52,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_3]
    {0x0990, 0x003F,}, 	// MCU_DATA_0
    {0x098C, 0xAB53,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_4]
    {0x0990, 0x0062,}, 	// MCU_DATA_0
    {0x098C, 0xAB54,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_5]
    {0x0990, 0x007D,}, 	// MCU_DATA_0
    {0x098C, 0xAB55,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_6]
    {0x0990, 0x0093,}, 	// MCU_DATA_0
    {0x098C, 0xAB56,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_7]
    {0x0990, 0x00A5,}, 	// MCU_DATA_0
    {0x098C, 0xAB57,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_8]
    {0x0990, 0x00B3,}, 	// MCU_DATA_0
    {0x098C, 0xAB58,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_9]
    {0x0990, 0x00BF,}, 	// MCU_DATA_0
    {0x098C, 0xAB59,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_10]
    {0x0990, 0x00C9,}, 	// MCU_DATA_0
    {0x098C, 0xAB5A,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_11]
    {0x0990, 0x00D3,}, 	// MCU_DATA_0
    {0x098C, 0xAB5B,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_12]
    {0x0990, 0x00DB,}, 	// MCU_DATA_0
    {0x098C, 0xAB5C,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_13]
    {0x0990, 0x00E2,}, 	// MCU_DATA_0
    {0x098C, 0xAB5D,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_14]
    {0x0990, 0x00E9,}, 	// MCU_DATA_0
    {0x098C, 0xAB5E,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_15]
    {0x0990, 0x00EF,}, 	// MCU_DATA_0
    {0x098C, 0xAB5F,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_16]
    {0x0990, 0x00F5,}, 	// MCU_DATA_0
    {0x098C, 0xAB60,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_17]
    {0x0990, 0x00FA,}, 	// MCU_DATA_0
    {0x098C, 0xAB61,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_18]
    {0x0990, 0x00FF,}, 	// MCU_DATA_0
    {0x098C, 0xAB3C,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_0]
    {0x0990, 0x0000,}, 	// MCU_DATA_0
    {0x098C, 0xAB3D,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_1]
    {0x0990, 0x000C,}, 	// MCU_DATA_0
    {0x098C, 0xAB3E,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_2]
    {0x0990, 0x0022,}, 	// MCU_DATA_0
    {0x098C, 0xAB3F,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_3]
    {0x0990, 0x003F,}, 	// MCU_DATA_0
    {0x098C, 0xAB40,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_4]
    {0x0990, 0x0062,}, 	// MCU_DATA_0
    {0x098C, 0xAB41,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_5]
    {0x0990, 0x007D,}, 	// MCU_DATA_0
    {0x098C, 0xAB42,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_6]
    {0x0990, 0x0093,}, 	// MCU_DATA_0
    {0x098C, 0xAB43,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_7]
    {0x0990, 0x00A5,}, 	// MCU_DATA_0
    {0x098C, 0xAB44,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_8]
    {0x0990, 0x00B3,}, 	// MCU_DATA_0
    {0x098C, 0xAB45,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_9]
    {0x0990, 0x00BF,}, 	// MCU_DATA_0
    {0x098C, 0xAB46,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_10]
    {0x0990, 0x00C9,}, 	// MCU_DATA_0
    {0x098C, 0xAB47,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_11]
    {0x0990, 0x00D3,}, 	// MCU_DATA_0
    {0x098C, 0xAB48,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_12]
    {0x0990, 0x00DB,}, 	// MCU_DATA_0
    {0x098C, 0xAB49,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_13]
    {0x0990, 0x00E2,}, 	// MCU_DATA_0
    {0x098C, 0xAB4A,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_14]
    {0x0990, 0x00E9,}, 	// MCU_DATA_0
    {0x098C, 0xAB4B,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_15]
    {0x0990, 0x00EF,}, 	// MCU_DATA_0
    {0x098C, 0xAB4C,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_16]
    {0x0990, 0x00F5,}, 	// MCU_DATA_0
    {0x098C, 0xAB4D,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_17]
    {0x0990, 0x00FA,}, 	// MCU_DATA_0
    {0x098C, 0xAB4E,}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_18]--------------------------ZHANGZHAO----------------
    {0x0990, 0x00FF,}, 	// MCU_DATA_0
    {0x098C, 0x2306,}, 	// MCU_ADDRESS [AWB_CCM_L_0]
    {0x0990, 0x01D6,}, 	// MCU_DATA_0
    {0x098C, 0x2308,}, 	// MCU_ADDRESS [AWB_CCM_L_1]
    {0x0990, 0xFF89,}, 	// MCU_DATA_0
    {0x098C, 0x230A,}, 	// MCU_ADDRESS [AWB_CCM_L_2]
    {0x0990, 0xFFA1,}, 	// MCU_DATA_0
    {0x098C, 0x230C,}, 	// MCU_ADDRESS [AWB_CCM_L_3]
    {0x0990, 0xFF73,}, 	// MCU_DATA_0
    {0x098C, 0x230E,}, 	// MCU_ADDRESS [AWB_CCM_L_4]
    {0x0990, 0x019C,}, 	// MCU_DATA_0
    {0x098C, 0x2310,}, 	// MCU_ADDRESS [AWB_CCM_L_5]
    {0x0990, 0xFFF1,}, 	// MCU_DATA_0
    {0x098C, 0x2312,}, 	// MCU_ADDRESS [AWB_CCM_L_6]
    {0x0990, 0xFFB0,}, 	// MCU_DATA_0
    {0x098C, 0x2314,}, 	// MCU_ADDRESS [AWB_CCM_L_7]
    {0x0990, 0xFF2D,}, 	// MCU_DATA_0
    {0x098C, 0x2316,}, 	// MCU_ADDRESS [AWB_CCM_L_8]
    {0x0990, 0x0223,}, 	// MCU_DATA_0
    {0x098C, 0x2318,}, 	// MCU_ADDRESS [AWB_CCM_L_9]
    {0x0990, 0x001C,}, 	// MCU_DATA_0
    {0x098C, 0x231A,}, 	// MCU_ADDRESS [AWB_CCM_L_10]
    {0x0990, 0x0048,}, 	// MCU_DATA_0
    {0x098C, 0x2318,}, 	// MCU_ADDRESS [AWB_CCM_L_9]
    {0x0990, 0x001C,}, 	// MCU_DATA_0
    {0x098C, 0x231A,}, 	// MCU_ADDRESS [AWB_CCM_L_10]
    {0x0990, 0x0038,}, 	// MCU_DATA_0
    {0x098C, 0x2318,}, 	// MCU_ADDRESS [AWB_CCM_L_9]
    {0x0990, 0x001E,}, 	// MCU_DATA_0
    {0x098C, 0x231A,}, 	// MCU_ADDRESS [AWB_CCM_L_10]
    {0x0990, 0x0038,}, 	// MCU_DATA_0
    {0x098C, 0x2318,}, 	// MCU_ADDRESS [AWB_CCM_L_9]
    {0x0990, 0x0022,}, 	// MCU_DATA_0
    {0x098C, 0x231A,}, 	// MCU_ADDRESS [AWB_CCM_L_10]
    {0x0990, 0x0038,}, 	// MCU_DATA_0
    {0x098C, 0x2318,}, 	// MCU_ADDRESS [AWB_CCM_L_9]
    {0x0990, 0x002C,}, 	// MCU_DATA_0
    {0x098C, 0x231A,}, 	// MCU_ADDRESS [AWB_CCM_L_10]
    {0x0990, 0x0038,}, 	// MCU_DATA_0
    {0x098C, 0x2318,}, 	// MCU_ADDRESS [AWB_CCM_L_9]
    {0x0990, 0x0024,}, 	// MCU_DATA_0
    {0x098C, 0x231A,}, 	// MCU_ADDRESS [AWB_CCM_L_10]
    {0x0990, 0x0038,}, 	// MCU_DATA_0
    {0x098C, 0x231C,}, 	// MCU_ADDRESS [AWB_CCM_RL_0]
    {0x0990, 0xFFCD,}, 	// MCU_DATA_0
    {0x098C, 0x231E,}, 	// MCU_ADDRESS [AWB_CCM_RL_1]
    {0x0990, 0x0023,}, 	// MCU_DATA_0
    {0x098C, 0x2320,}, 	// MCU_ADDRESS [AWB_CCM_RL_2]
    {0x0990, 0x0010,}, 	// MCU_DATA_0
    {0x098C, 0x2322,}, 	// MCU_ADDRESS [AWB_CCM_RL_3]
    {0x0990, 0x0026,}, 	// MCU_DATA_0
    {0x098C, 0x2324,}, 	// MCU_ADDRESS [AWB_CCM_RL_4]
    {0x0990, 0xFFE9,}, 	// MCU_DATA_0
    {0x098C, 0x2326,}, 	// MCU_ADDRESS [AWB_CCM_RL_5]
    {0x0990, 0xFFF1,}, 	// MCU_DATA_0
    {0x098C, 0x2328,}, 	// MCU_ADDRESS [AWB_CCM_RL_6]
    {0x0990, 0x003A,}, 	// MCU_DATA_0
    {0x098C, 0x232A,}, 	// MCU_ADDRESS [AWB_CCM_RL_7]
    {0x0990, 0x005D,}, 	// MCU_DATA_0
    {0x098C, 0x232C,}, 	// MCU_ADDRESS [AWB_CCM_RL_8]
    {0x0990, 0xFF69,}, 	// MCU_DATA_0
    {0x098C, 0x232E,}, 	// MCU_ADDRESS [AWB_CCM_RL_9]
    {0x0990, 0x000C,}, 	// MCU_DATA_0
    {0x098C, 0x2330,}, 	// MCU_ADDRESS [AWB_CCM_RL_10]
    {0x0990, 0xFFE4,}, 	// MCU_DATA_0
    {0x098C, 0x232E,}, 	// MCU_ADDRESS [AWB_CCM_RL_9]
    {0x0990, 0x000C,}, 	// MCU_DATA_0
    {0x098C, 0x2330,}, 	// MCU_ADDRESS [AWB_CCM_RL_10]
    {0x0990, 0xFFF4,}, 	// MCU_DATA_0
    {0x098C, 0x232E,}, 	// MCU_ADDRESS [AWB_CCM_RL_9]
    {0x0990, 0x000A,}, 	// MCU_DATA_0
    {0x098C, 0x2330,}, 	// MCU_ADDRESS [AWB_CCM_RL_10]
    {0x0990, 0xFFF4,}, 	// MCU_DATA_0
    {0x098C, 0x232E,}, 	// MCU_ADDRESS [AWB_CCM_RL_9]
    {0x0990, 0x0006,}, 	// MCU_DATA_0
    {0x098C, 0x2330,}, 	// MCU_ADDRESS [AWB_CCM_RL_10]
    {0x0990, 0xFFF4,}, 	// MCU_DATA_0
    {0x098C, 0x232E,}, 	// MCU_ADDRESS [AWB_CCM_RL_9]
    {0x0990, 0xFFFC,}, 	// MCU_DATA_0
    {0x098C, 0x2330,}, 	// MCU_ADDRESS [AWB_CCM_RL_10]
    {0x0990, 0xFFF4,}, 	// MCU_DATA_0
    {0x098C, 0x232E,}, 	// MCU_ADDRESS [AWB_CCM_RL_9]
    {0x0990, 0x0004,}, 	// MCU_DATA_0
    {0x098C, 0x2330,}, 	// MCU_ADDRESS [AWB_CCM_RL_10]
    {0x0990, 0xFFF4,}, 	// MCU_DATA_0
    {0x098C, 0x0415,}, 	// MCU_ADDRESS
    {0x0990, 0xF601,},
    {0x0992, 0x42C1,},
    {0x0994, 0x0326,},
    {0x0996, 0x11F6,},
    {0x0998, 0x0143,},
    {0x099A, 0xC104,},
    {0x099C, 0x260A,},
    {0x099E, 0xCC04,},
    {0x098C, 0x0425,},     // MCU_ADDRESS
    {0x0990, 0x33BD,},
    {0x0992, 0xA362,},
    {0x0994, 0xBD04,},
    {0x0996, 0x3339,},
    {0x0998, 0xC6FF,},
    {0x099A, 0xF701,},
    {0x099C, 0x6439,},
    {0x099E, 0xDE5D,}, 
    {0x098C, 0x0435,},     // MCU_ADDRESS
    {0x0990, 0x18CE,}, 
    {0x0992, 0x0325,}, 
    {0x0994, 0xCC00,}, 
    {0x0996, 0x27BD,}, 
    {0x0998, 0xC2B8,}, 
    {0x099A, 0xCC04,}, 
    {0x099C, 0xC7FD,}, 
    {0x099E, 0x033B,}, 
    {0x098C, 0x0445,},     // MCU_ADDRESS
    {0x0990, 0xCC06,}, 
    {0x0992, 0x75FD,}, 
    {0x0994, 0x032F,}, 
    {0x0996, 0xCC06,}, 
    {0x0998, 0x7FFD,}, 
    {0x099A, 0x032D,}, 
    {0x099C, 0xCC03,}, 
    {0x099E, 0x25DD,}, 
    {0x098C, 0x0455,},     // MCU_ADDRESS
    {0x0990, 0x5DC6,}, 
    {0x0992, 0x1ED7,}, 
    {0x0994, 0x6CD7,}, 
    {0x0996, 0x6D5F,}, 
    {0x0998, 0xD76E,}, 
    {0x099A, 0xD78D,}, 
    {0x099C, 0x8620,}, 
    {0x099E, 0x977A,}, 
    {0x098C, 0x0465,},     // MCU_ADDRESS
    {0x0990, 0xD77B,}, 
    {0x0992, 0x979A,}, 
    {0x0994, 0xC621,}, 
    {0x0996, 0xD79B,}, 
    {0x0998, 0xC610,}, 
    {0x099A, 0xF703,}, 
    {0x099C, 0x73FE,}, 
    {0x099E, 0x0169,}, 
    {0x098C, 0x0475,},     // MCU_ADDRESS
    {0x0990, 0x18CE,}, 
    {0x0992, 0x034D,}, 
    {0x0994, 0xCC00,}, 
    {0x0996, 0x13BD,}, 
    {0x0998, 0xC2B8,}, 
    {0x099A, 0xCC05,}, 
    {0x099C, 0xF3FD,}, 
    {0x099E, 0x034F,}, 
    {0x098C, 0x0485,},     // MCU_ADDRESS
    {0x0990, 0xCC03,}, 
    {0x0992, 0x4DFD,}, 
    {0x0994, 0x0169,}, 
    {0x0996, 0xFE02,}, 
    {0x0998, 0xBD18,}, 
    {0x099A, 0xCE03,}, 
    {0x099C, 0x61CC,}, 
    {0x099E, 0x0011,}, 
    {0x098C, 0x0495,},     // MCU_ADDRESS
    {0x0990, 0xBDC2,}, 
    {0x0992, 0xB8CC,}, 
    {0x0994, 0x0632,}, 
    {0x0996, 0xFD03,}, 
    {0x0998, 0x6FCC,}, 
    {0x099A, 0x0361,}, 
    {0x099C, 0xFD02,}, 
    {0x099E, 0xBDDE,}, 
    {0x098C, 0x04A5,},     // MCU_ADDRESS
    {0x0990, 0x0018,}, 
    {0x0992, 0xCE00,}, 
    {0x0994, 0xC2CC,}, 
    {0x0996, 0x0037,}, 
    {0x0998, 0xBDC2,}, 
    {0x099A, 0xB8CC,}, 
    {0x099C, 0x0659,}, 
    {0x099E, 0xDDE6,}, 
    {0x098C, 0x04B5,},     // MCU_ADDRESS
    {0x0990, 0xCC00,}, 
    {0x0992, 0xC2DD,}, 
    {0x0994, 0x00C6,}, 
    {0x0996, 0x03F7,}, 
    {0x0998, 0x0164,}, 
    {0x099A, 0x5AF7,}, 
    {0x099C, 0x0165,}, 
    {0x099E, 0x7F01,}, 
    {0x098C, 0x04C5,},     // MCU_ADDRESS
    {0x0990, 0x6639,}, 
    {0x0992, 0x373C,}, 
    {0x0994, 0x3C3C,}, 
    {0x0996, 0x3C3C,}, 
    {0x0998, 0x30EC,}, 
    {0x099A, 0x11ED,}, 
    {0x099C, 0x02EC,}, 
    {0x099E, 0x0FED,}, 
    {0x098C, 0x04D5,},     // MCU_ADDRESS
    {0x0990, 0x008F,}, 
    {0x0992, 0x30ED,}, 
    {0x0994, 0x04EC,}, 
    {0x0996, 0x0DEE,}, 
    {0x0998, 0x04BD,}, 
    {0x099A, 0xA406,}, 
    {0x099C, 0x30EC,}, 
    {0x099E, 0x02ED,}, 
    {0x098C, 0x04E5,},     // MCU_ADDRESS
    {0x0990, 0x06FC,}, 
    {0x0992, 0x10C0,}, 
    {0x0994, 0x2705,}, 
    {0x0996, 0xCCFF,}, 
    {0x0998, 0xFFED,}, 
    {0x099A, 0x06F6,}, 
    {0x099C, 0x0256,}, 
    {0x099E, 0x8616,}, 
    {0x098C, 0x04F5,},     // MCU_ADDRESS
    {0x0990, 0x3DC3,}, 
    {0x0992, 0x0261,}, 
    {0x0994, 0x8FE6,}, 
    {0x0996, 0x09C4,}, 
    {0x0998, 0x07C1,}, 
    {0x099A, 0x0226,}, 
    {0x099C, 0x1DFC,}, 
    {0x099E, 0x10C2,}, 
    {0x098C, 0x0505,}, 
    {0x0990, 0x30ED,}, 
    {0x0992, 0x02FC,}, 
    {0x0994, 0x10C0,}, 
    {0x0996, 0xED00,}, 
    {0x0998, 0xC602,}, 
    {0x099A, 0xBDC2,}, 
    {0x099C, 0x5330,}, 
    {0x099E, 0xEC00,}, 
    {0x098C, 0x0515,}, 
    {0x0990, 0xFD10,}, 
    {0x0992, 0xC0EC,}, 
    {0x0994, 0x02FD,}, 
    {0x0996, 0x10C2,}, 
    {0x0998, 0x201B,}, 
    {0x099A, 0xFC10,}, 
    {0x099C, 0xC230,}, 
    {0x099E, 0xED02,}, 
    {0x098C, 0x0525,}, 
    {0x0990, 0xFC10,}, 
    {0x0992, 0xC0ED,}, 
    {0x0994, 0x00C6,}, 
    {0x0996, 0x01BD,}, 
    {0x0998, 0xC253,}, 
    {0x099A, 0x30EC,}, 
    {0x099C, 0x00FD,}, 
    {0x099E, 0x10C0,}, 
    {0x098C, 0x0535,}, 
    {0x0990, 0xEC02,}, 
    {0x0992, 0xFD10,}, 
    {0x0994, 0xC2C6,}, 
    {0x0996, 0x80D7,}, 
    {0x0998, 0x85C6,}, 
    {0x099A, 0x40F7,}, 
    {0x099C, 0x10C4,}, 
    {0x099E, 0xF602,}, 
    {0x098C, 0x0545,}, 
    {0x0990, 0x5686,}, 
    {0x0992, 0x163D,}, 
    {0x0994, 0xC302,}, 
    {0x0996, 0x618F,}, 
    {0x0998, 0xEC14,}, 
    {0x099A, 0xFD10,}, 
    {0x099C, 0xC501,}, 
    {0x099E, 0x0101,}, 
    {0x098C, 0x0555,}, 
    {0x0990, 0x0101,}, 
    {0x0992, 0xFC10,}, 
    {0x0994, 0xC2DD,}, 
    {0x0996, 0x7FFC,}, 
    {0x0998, 0x10C7,}, 
    {0x099A, 0xDD76,}, 
    {0x099C, 0xF602,}, 
    {0x099E, 0x5686,}, 
    {0x098C, 0x0565,}, 
    {0x0990, 0x163D,}, 
    {0x0992, 0xC302,}, 
    {0x0994, 0x618F,}, 
    {0x0996, 0xEC14,}, 
    {0x0998, 0x939F,}, 
    {0x099A, 0x30ED,}, 
    {0x099C, 0x08DC,}, 
    {0x099E, 0x7693,}, 
    {0x098C, 0x0575,}, 
    {0x0990, 0x9D25,}, 
    {0x0992, 0x08F6,}, 
    {0x0994, 0x02BC,}, 
    {0x0996, 0x4F93,}, 
    {0x0998, 0x7F23,}, 
    {0x099A, 0x3DF6,}, 
    {0x099C, 0x02BC,}, 
    {0x099E, 0x4F93,}, 
    {0x098C, 0x0585,}, 
    {0x0990, 0x7F23,}, 
    {0x0992, 0x06F6,}, 
    {0x0994, 0x02BC,}, 
    {0x0996, 0x4FDD,}, 
    {0x0998, 0x7FDC,}, 
    {0x099A, 0x9DDD,}, 
    {0x099C, 0x76F6,}, 
    {0x099E, 0x02BC,}, 
    {0x098C, 0x0595,}, 
    {0x0990, 0x4F93,}, 
    {0x0992, 0x7F26,}, 
    {0x0994, 0x0FE6,}, 
    {0x0996, 0x0AC1,}, 
    {0x0998, 0x0226,}, 
    {0x099A, 0x09D6,}, 
    {0x099C, 0x85C1,}, 
    {0x099E, 0x8026,}, 
    {0x098C, 0x05A5,}, 
    {0x0990, 0x0314,}, 
    {0x0992, 0x7401,}, 
    {0x0994, 0xF602,}, 
    {0x0996, 0xBC4F,}, 
    {0x0998, 0x937F,}, 
    {0x099A, 0x2416,}, 
    {0x099C, 0xDE7F,}, 
    {0x099E, 0x09DF,}, 
    {0x098C, 0x05B5,}, 
    {0x0990, 0x7F30,}, 
    {0x0992, 0xEC08,}, 
    {0x0994, 0xDD76,}, 
    {0x0996, 0x200A,}, 
    {0x0998, 0xDC76,}, 
    {0x099A, 0xA308,}, 
    {0x099C, 0x2304,}, 
    {0x099E, 0xEC08,}, 
    {0x098C, 0x05C5,}, 
    {0x0990, 0xDD76,}, 
    {0x0992, 0x1274,}, 
    {0x0994, 0x0122,}, 
    {0x0996, 0xDE5D,}, 
    {0x0998, 0xEE14,}, 
    {0x099A, 0xAD00,}, 
    {0x099C, 0x30ED,},
    {0x099E, 0x11EC,}, 
    {0x098C, 0x05D5,}, 
    {0x0990, 0x06ED,}, 
    {0x0992, 0x02CC,}, 
    {0x0994, 0x0080,}, 
    {0x0996, 0xED00,}, 
    {0x0998, 0x8F30,}, 
    {0x099A, 0xED04,}, 
    {0x099C, 0xEC11,}, 
    {0x099E, 0xEE04,}, 
    {0x098C, 0x05E5,}, 
    {0x0990, 0xBDA4,}, 
    {0x0992, 0x0630,}, 
    {0x0994, 0xE603,}, 
    {0x0996, 0xD785,}, 
    {0x0998, 0x30C6,}, 
    {0x099A, 0x0B3A,}, 
    {0x099C, 0x3539,}, 
    {0x099E, 0x3C3C,}, 
    {0x098C, 0x05F5,}, 
    {0x0990, 0x3C34,}, 
    {0x0992, 0xCC32,}, 
    {0x0994, 0x3EBD,}, 
    {0x0996, 0xA558,}, 
    {0x0998, 0x30ED,}, 
    {0x099A, 0x04BD,}, 
    {0x099C, 0xB2D7,}, 
    {0x099E, 0x30E7,}, 
    {0x098C, 0x0605,}, 
    {0x0990, 0x06CC,}, 
    {0x0992, 0x323E,}, 
    {0x0994, 0xED00,}, 
    {0x0996, 0xEC04,}, 
    {0x0998, 0xBDA5,}, 
    {0x099A, 0x44CC,}, 
    {0x099C, 0x3244,}, 
    {0x099E, 0xBDA5,}, 
    {0x098C, 0x0615,}, 
    {0x0990, 0x585F,}, 
    {0x0992, 0x30ED,}, 
    {0x0994, 0x02CC,}, 
    {0x0996, 0x3244,}, 
    {0x0998, 0xED00,}, 
    {0x099A, 0xF601,}, 
    {0x099C, 0xD54F,}, 
    {0x099E, 0xEA03,}, 
    {0x098C, 0x0625,}, 
    {0x0990, 0xAA02,}, 
    {0x0992, 0xBDA5,}, 
    {0x0994, 0x4430,}, 
    {0x0996, 0xE606,}, 
    {0x0998, 0x3838,}, 
    {0x099A, 0x3831,}, 
    {0x099C, 0x39BD,}, 
    {0x099E, 0xD661,}, 
    {0x098C, 0x0635,}, 
    {0x0990, 0xF602,}, 
    {0x0992, 0xF4C1,}, 
    {0x0994, 0x0126,}, 
    {0x0996, 0x0BFE,}, 
    {0x0998, 0x02BD,}, 
    {0x099A, 0xEE10,}, 
    {0x099C, 0xFC02,}, 
    {0x099E, 0xF5AD,}, 
    {0x098C, 0x0645,}, 
    {0x0990, 0x0039,}, 
    {0x0992, 0xF602,}, 
    {0x0994, 0xF4C1,}, 
    {0x0996, 0x0226,}, 
    {0x0998, 0x0AFE,}, 
    {0x099A, 0x02BD,}, 
    {0x099C, 0xEE10,}, 
    {0x099E, 0xFC02,}, 
    {0x098C, 0x0655,}, 
    {0x0990, 0xF7AD,}, 
    {0x0992, 0x0039,}, 
    {0x0994, 0x3CBD,}, 
    {0x0996, 0xB059,}, 
    {0x0998, 0xCC00,}, 
    {0x099A, 0x28BD,}, 
    {0x099C, 0xA558,}, 
    {0x099E, 0x8300,}, 
    {0x098C, 0x0665,}, 
    {0x0990, 0x0027,}, 
    {0x0992, 0x0BCC,}, 
    {0x0994, 0x0026,}, 
    {0x0996, 0x30ED,}, 
    {0x0998, 0x00C6,}, 
    {0x099A, 0x03BD,}, 
    {0x099C, 0xA544,}, 
    {0x099E, 0x3839,}, 
    {0x098C, 0x0675,}, 
    {0x0990, 0xBDD9,}, 
    {0x0992, 0x42D6,}, 
    {0x0994, 0x9ACB,}, 
    {0x0996, 0x01D7,}, 
    {0x0998, 0x9B39,}, 
    {0x099A, 0x373C,}, 
    {0x099C, 0x3C3C,}, 
    {0x099E, 0x30E6,}, 
    {0x098C, 0x0685,}, 
    {0x0990, 0x06BD,}, 
    {0x0992, 0xD9D7,}, 
    {0x0994, 0xCC30,}, 
    {0x0996, 0x88BD,}, 
    {0x0998, 0xA558,}, 
    {0x099A, 0x30ED,}, 
    {0x099C, 0x04CC,}, 
    {0x099E, 0x309A,}, 
    {0x098C, 0x0695,}, 
    {0x0990, 0xBDA5,}, 
    {0x0992, 0x5830,}, 
    {0x0994, 0xED02,}, 
    {0x0996, 0xD68C,}, 
    {0x0998, 0xC101,}, 
    {0x099A, 0x2223,}, 
    {0x099C, 0xF603,}, 
    {0x099E, 0x734F,}, 
    {0x098C, 0x06A5,}, 
    {0x0990, 0x937F,}, 
    {0x0992, 0x251B,}, 
    {0x0994, 0xCC30,}, 
    {0x0996, 0x88ED,}, 
    {0x0998, 0x00EC,}, 
    {0x099A, 0x04C4,}, 
    {0x099C, 0xF1BD,}, 
    {0x099E, 0xA544,}, 
    {0x098C, 0x06B5,}, 
    {0x0990, 0xCC30,}, 
    {0x0992, 0x9A30,}, 
    {0x0994, 0xED00,}, 
    {0x0996, 0xEC02,}, 
    {0x0998, 0xCA01,}, 
    {0x099A, 0xBDA5,}, 
    {0x099C, 0x4420,}, 
    {0x099E, 0x1D1F,}, 
    {0x098C, 0x06C5,}, 
    {0x0990, 0x0301,}, 
    {0x0992, 0x19CC,}, 
    {0x0994, 0x3088,}, 
    {0x0996, 0xED00,}, 
    {0x0998, 0xEC04,}, 
    {0x099A, 0xCA0F,}, 
    {0x099C, 0xBDA5,}, 
    {0x099E, 0x44CC,}, 
    {0x098C, 0x06D5,}, 
    {0x0990, 0x309A,}, 
    {0x0992, 0x30ED,}, 
    {0x0994, 0x00EC,}, 
    {0x0996, 0x02C4,}, 
    {0x0998, 0xFEBD,}, 
    {0x099A, 0xA544,}, 
    {0x099C, 0x3838,}, 
    {0x099E, 0x3831,}, 
    {0x098C, 0x86E5,}, 
    {0x0990, 0x0039,}, 
    {0x098C, 0x2006,},     // MCU_ADDRESS [MON_ARG1]
    {0x0990, 0x0415,},     // MCU_DATA_0
    {0x098C, 0xA005,},     // MCU_ADDRESS [MON_CMD]
    {0x0990, 0x0001,},     // MCU_DATA_0    

    {0x098C, 0x2755,},     // MCU_ADDRESS [MODE_OUTPUT_FORMAT_A]
    {0x0990, 0x0002,},     // MCU_DATA_0
    {0x098C, 0x2757,},     // MCU_ADDRESS [MODE_OUTPUT_FORMAT_B]
    {0x0990, 0x0002,},     // MCU_DATA_0

    #ifdef CONFIG_CAMERA_MT9D115_MIRROR_ZTE
    {0x098C, 0x2717,},
    {0x0990, 0x046D,},
    {0x098C, 0x272D,},
    {0x0990, 0x0025,},
    {0x098C, 0xA103,},
    {0x0990, 0x0006,},
    #endif 
    #ifdef CONFIG_CAMERA_MT9D115_FLIP_ZTE 
    {0x098C, 0x2717,},
    {0x0990, 0x046E,},
    {0x098C, 0x272D,},
    {0x0990, 0x0026,},
    {0x098C, 0xA103,},
    {0x0990, 0x0006,},
    #endif
    #ifdef CONFIG_CAMERA_MT9D115_FLIP_MIRROR_ZTE
    {0x098C, 0x2717,},
    {0x0990, 0x046F,},
    {0x098C, 0x272D,},
    {0x0990, 0x0027,},
    {0x098C, 0xA103,},
    {0x0990, 0x0006,},
    #endif    
	
};



static struct v4l2_subdev_info mt9d115_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_YUYV8_2X8,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array mt9d115_init_conf[] = {
	{&reset_sensor_setting1[0],
	ARRAY_SIZE(reset_sensor_setting1), 2, MSM_CAMERA_I2C_WORD_DATA},
	{&reset_sensor_setting2[0],
	ARRAY_SIZE(reset_sensor_setting2), 10, MSM_CAMERA_I2C_WORD_DATA},
	{&mt9d115_pll_settings[0],
	ARRAY_SIZE(mt9d115_pll_settings), 10, MSM_CAMERA_I2C_WORD_DATA},
	{&mt9d115_settings[0],
	ARRAY_SIZE(mt9d115_settings), 50, MSM_CAMERA_I2C_WORD_DATA},	//wangminrong
	{&mt9d115_recommend_settings[0],
	ARRAY_SIZE(mt9d115_recommend_settings), 10, MSM_CAMERA_I2C_WORD_DATA},
	{&mt9d115_init_fine_tune[0],
	ARRAY_SIZE(mt9d115_init_fine_tune), 20, MSM_CAMERA_I2C_WORD_DATA},
	{&mt9d115_init_fine_tune_end[0],
	ARRAY_SIZE(mt9d115_init_fine_tune_end), 10, MSM_CAMERA_I2C_WORD_DATA},	
	{&software_standby_disable[0],
	ARRAY_SIZE(software_standby_disable), 0, MSM_CAMERA_I2C_WORD_DATA},	
	{&refresh_setting1[0],
	ARRAY_SIZE(refresh_setting1), 200, MSM_CAMERA_I2C_WORD_DATA},	//wangminrong 
	{&mt9d115_refresh[0],
	ARRAY_SIZE(mt9d115_refresh), 150, MSM_CAMERA_I2C_WORD_DATA},	
};
#if 1
static struct msm_camera_i2c_reg_conf mt9d115_saturation[][2] = {
    {{0x098C, 0xAB20,},{0x0990, 0x004e,}},        // saturation -2
    {{0x098C, 0xAB20,},{0x0990, 0x005e,}},        // saturation -1
    {{0x098C, 0xAB20,},{0x0990, 0x006e,}},        // saturation 0//ZHANGZHAO FOR SATURATION
    {{0x098C, 0xAB20,},{0x0990, 0x007e,}},        // saturation +1
    {{0x098C, 0xAB20,},{0x0990, 0x008e,}}         // saturation +2
};

static struct msm_camera_i2c_reg_conf mt9d115_contrast[][76] = {
{//-2
    {0x098C, 0xAB3C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_0]
    {0x0990, 0x0000}, 	// MCU_DATA_0
    {0x098C, 0xAB3D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_1]
    {0x0990, 0x0018}, 	// MCU_DATA_0
    {0x098C, 0xAB3E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_2]
    {0x0990, 0x0041}, 	// MCU_DATA_0
    {0x098C, 0xAB3F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_3]
    {0x0990, 0x0064}, 	// MCU_DATA_0
    {0x098C, 0xAB40}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_4]
    {0x0990, 0x0083}, 	// MCU_DATA_0
    {0x098C, 0xAB41}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_5]
    {0x0990, 0x0096}, 	// MCU_DATA_0
    {0x098C, 0xAB42}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_6]
    {0x0990, 0x00A5}, 	// MCU_DATA_0
    {0x098C, 0xAB43}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_7]
    {0x0990, 0x00B1}, 	// MCU_DATA_0
    {0x098C, 0xAB44}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_8]
    {0x0990, 0x00BC}, 	// MCU_DATA_0
    {0x098C, 0xAB45}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_9]
    {0x0990, 0x00C5}, 	// MCU_DATA_0
    {0x098C, 0xAB46}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_10]
    {0x0990, 0x00CE}, 	// MCU_DATA_0
    {0x098C, 0xAB47}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_11]
    {0x0990, 0x00D6}, 	// MCU_DATA_0
    {0x098C, 0xAB48}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_12]
    {0x0990, 0x00DD}, 	// MCU_DATA_0
    {0x098C, 0xAB49}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_13]
    {0x0990, 0x00E3}, 	// MCU_DATA_0
    {0x098C, 0xAB4A}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_14]
    {0x0990, 0x00E9}, 	// MCU_DATA_0
    {0x098C, 0xAB4B}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_15]
    {0x0990, 0x00EF}, 	// MCU_DATA_0
    {0x098C, 0xAB4C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_16]
    {0x0990, 0x00F5}, 	// MCU_DATA_0
    {0x098C, 0xAB4D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_17]
    {0x0990, 0x00FA}, 	// MCU_DATA_0
    {0x098C, 0xAB4E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_18]
    {0x0990, 0x00FF}, 	// MCU_DATA_0
    {0x098C, 0xAB4F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_0]
    {0x0990, 0x0000}, 	// MCU_DATA_0
    {0x098C, 0xAB50}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_1]
    {0x0990, 0x0018}, 	// MCU_DATA_0
    {0x098C, 0xAB51}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_2]
    {0x0990, 0x0041}, 	// MCU_DATA_0
    {0x098C, 0xAB52}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_3]
    {0x0990, 0x0064}, 	// MCU_DATA_0
    {0x098C, 0xAB53}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_4]
    {0x0990, 0x0083}, 	// MCU_DATA_0
    {0x098C, 0xAB54}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_5]
    {0x0990, 0x0096}, 	// MCU_DATA_0
    {0x098C, 0xAB55}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_6]
    {0x0990, 0x00A5}, 	// MCU_DATA_0
    {0x098C, 0xAB56}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_7]
    {0x0990, 0x00B1}, 	// MCU_DATA_0
    {0x098C, 0xAB57}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_8]
    {0x0990, 0x00BC}, 	// MCU_DATA_0
    {0x098C, 0xAB58}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_9]
    {0x0990, 0x00C5}, 	// MCU_DATA_0
    {0x098C, 0xAB59}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_10]
    {0x0990, 0x00CE}, 	// MCU_DATA_0
    {0x098C, 0xAB5A}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_11]
    {0x0990, 0x00D6}, 	// MCU_DATA_0
    {0x098C, 0xAB5B}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_12]
    {0x0990, 0x00DD}, 	// MCU_DATA_0
    {0x098C, 0xAB5C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_13]
    {0x0990, 0x00E3}, 	// MCU_DATA_0
    {0x098C, 0xAB5D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_14]
    {0x0990, 0x00E9}, 	// MCU_DATA_0
    {0x098C, 0xAB5E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_15]
    {0x0990, 0x00EF}, 	// MCU_DATA_0
    {0x098C, 0xAB5F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_16]
    {0x0990, 0x00F5}, 	// MCU_DATA_0
    {0x098C, 0xAB60}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_17]
    {0x0990, 0x00FA}, 	// MCU_DATA_0
    {0x098C, 0xAB61}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_18]
    {0x0990, 0x00FF}, 	// MCU_DATA_0
},        // saturation -2
    {
    {0x098C, 0xAB3C},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_0]
    {0x0990, 0x0000},	// MCU_DATA_0
    {0x098C, 0xAB3D},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_1]
    {0x0990, 0x0010},	// MCU_DATA_0
    {0x098C, 0xAB3E},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_2]
    {0x0990, 0x002E},	// MCU_DATA_0
    {0x098C, 0xAB3F},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_3]
    {0x0990, 0x004F},	// MCU_DATA_0
    {0x098C, 0xAB40},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_4]
    {0x0990, 0x0072},	// MCU_DATA_0
    {0x098C, 0xAB41},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_5]
    {0x0990, 0x008C},	// MCU_DATA_0
    {0x098C, 0xAB42},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_6]
    {0x0990, 0x009F},	// MCU_DATA_0
    {0x098C, 0xAB43},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_7]
    {0x0990, 0x00AD},	// MCU_DATA_0
    {0x098C, 0xAB44},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_8]
    {0x0990, 0x00BA},	// MCU_DATA_0
    {0x098C, 0xAB45},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_9]
    {0x0990, 0x00C4},	// MCU_DATA_0
    {0x098C, 0xAB46},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_10]
    {0x0990, 0x00CE},	// MCU_DATA_0
    {0x098C, 0xAB47},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_11]
    {0x0990, 0x00D6},	// MCU_DATA_0
    {0x098C, 0xAB48},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_12]
    {0x0990, 0x00DD},	// MCU_DATA_0
    {0x098C, 0xAB49},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_13]
    {0x0990, 0x00E4},	// MCU_DATA_0
    {0x098C, 0xAB4A},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_14]
    {0x0990, 0x00EA},	// MCU_DATA_0
    {0x098C, 0xAB4B},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_15]
    {0x0990, 0x00F0},	// MCU_DATA_0
    {0x098C, 0xAB4C},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_16]
    {0x0990, 0x00F5},	// MCU_DATA_0
    {0x098C, 0xAB4D},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_17]
    {0x0990, 0x00FA},	// MCU_DATA_0
    {0x098C, 0xAB4E},	// MCU_ADDRESS [HG_GAMMA_TABLE_A_18]
    {0x0990, 0x00FF},	// MCU_DATA_0
    {0x098C, 0xAB4F},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_0]
    {0x0990, 0x0000},	// MCU_DATA_0
    {0x098C, 0xAB50},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_1]
    {0x0990, 0x0010},	// MCU_DATA_0
    {0x098C, 0xAB51},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_2]
    {0x0990, 0x002E},	// MCU_DATA_0
    {0x098C, 0xAB52},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_3]
    {0x0990, 0x004F},	// MCU_DATA_0
    {0x098C, 0xAB53},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_4]
    {0x0990, 0x0072},	// MCU_DATA_0
    {0x098C, 0xAB54},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_5]
    {0x0990, 0x008C},	// MCU_DATA_0
    {0x098C, 0xAB55},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_6]
    {0x0990, 0x009F},	// MCU_DATA_0
    {0x098C, 0xAB56},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_7]
    {0x0990, 0x00AD},	// MCU_DATA_0
    {0x098C, 0xAB57},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_8]
    {0x0990, 0x00BA},	// MCU_DATA_0
    {0x098C, 0xAB58},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_9]
    {0x0990, 0x00C4},	// MCU_DATA_0
    {0x098C, 0xAB59},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_10]
    {0x0990, 0x00CE},	// MCU_DATA_0
    {0x098C, 0xAB5A},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_11]
    {0x0990, 0x00D6},	// MCU_DATA_0
    {0x098C, 0xAB5B},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_12]
    {0x0990, 0x00DD},	// MCU_DATA_0
    {0x098C, 0xAB5C},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_13]
    {0x0990, 0x00E4},	// MCU_DATA_0
    {0x098C, 0xAB5D},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_14]
    {0x0990, 0x00EA},	// MCU_DATA_0
    {0x098C, 0xAB5E},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_15]
    {0x0990, 0x00F0},	// MCU_DATA_0
    {0x098C, 0xAB5F},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_16]
    {0x0990, 0x00F5},	// MCU_DATA_0
    {0x098C, 0xAB60},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_17]
    {0x0990, 0x00FA},	// MCU_DATA_0
    {0x098C, 0xAB61},	// MCU_ADDRESS [HG_GAMMA_TABLE_B_18]
    {0x0990, 0x00FF},	// MCU_DATA_0
},        // saturation -1
    {
     {0x098C, 0xAB4F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_0]
     {0x0990, 0x0000}, 	// MCU_DATA_0
     {0x098C, 0xAB50}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_1]
     {0x0990, 0x000C}, 	// MCU_DATA_0
     {0x098C, 0xAB51}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_2]
     {0x0990, 0x0022}, 	// MCU_DATA_0
     {0x098C, 0xAB52}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_3]
     {0x0990, 0x003F}, 	// MCU_DATA_0
     {0x098C, 0xAB53}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_4]
     {0x0990, 0x0062}, 	// MCU_DATA_0
     {0x098C, 0xAB54}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_5]
     {0x0990, 0x007D}, 	// MCU_DATA_0
     {0x098C, 0xAB55}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_6]
     {0x0990, 0x0093}, 	// MCU_DATA_0
     {0x098C, 0xAB56}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_7]
     {0x0990, 0x00A5}, 	// MCU_DATA_0
     {0x098C, 0xAB57}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_8]
     {0x0990, 0x00B3}, 	// MCU_DATA_0
     {0x098C, 0xAB58}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_9]
     {0x0990, 0x00BF}, 	// MCU_DATA_0
     {0x098C, 0xAB59}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_10]
     {0x0990, 0x00C9}, 	// MCU_DATA_0
     {0x098C, 0xAB5A}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_11]
     {0x0990, 0x00D3}, 	// MCU_DATA_0
     {0x098C, 0xAB5B}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_12]
     {0x0990, 0x00DB}, 	// MCU_DATA_0
     {0x098C, 0xAB5C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_13]
     {0x0990, 0x00E2}, 	// MCU_DATA_0
     {0x098C, 0xAB5D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_14]
     {0x0990, 0x00E9}, 	// MCU_DATA_0
     {0x098C, 0xAB5E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_15]
     {0x0990, 0x00EF}, 	// MCU_DATA_0
     {0x098C, 0xAB5F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_16]
     {0x0990, 0x00F5}, 	// MCU_DATA_0
     {0x098C, 0xAB60}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_17]
     {0x0990, 0x00FA}, 	// MCU_DATA_0
     {0x098C, 0xAB61}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_18]
     {0x0990, 0x00FF}, 	// MCU_DATA_0
     {0x098C, 0xAB3C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_0]
     {0x0990, 0x0000}, 	// MCU_DATA_0
     {0x098C, 0xAB3D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_1]
     {0x0990, 0x000C}, 	// MCU_DATA_0
     {0x098C, 0xAB3E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_2]
     {0x0990, 0x0022}, 	// MCU_DATA_0
     {0x098C, 0xAB3F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_3]
     {0x0990, 0x003F}, 	// MCU_DATA_0
     {0x098C, 0xAB40}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_4]
     {0x0990, 0x0062}, 	// MCU_DATA_0
     {0x098C, 0xAB41}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_5]
     {0x0990, 0x007D}, 	// MCU_DATA_0
     {0x098C, 0xAB42}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_6]
     {0x0990, 0x0093}, 	// MCU_DATA_0
     {0x098C, 0xAB43}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_7]
     {0x0990, 0x00A5}, 	// MCU_DATA_0
     {0x098C, 0xAB44}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_8]
     {0x0990, 0x00B3}, 	// MCU_DATA_0
     {0x098C, 0xAB45}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_9]
     {0x0990, 0x00BF}, 	// MCU_DATA_0
     {0x098C, 0xAB46}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_10]
     {0x0990, 0x00C9}, 	// MCU_DATA_0
     {0x098C, 0xAB47}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_11]
     {0x0990, 0x00D3}, 	// MCU_DATA_0
     {0x098C, 0xAB48}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_12]
     {0x0990, 0x00DB}, 	// MCU_DATA_0
     {0x098C, 0xAB49}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_13]
     {0x0990, 0x00E2}, 	// MCU_DATA_0
     {0x098C, 0xAB4A}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_14]
     {0x0990, 0x00E9}, 	// MCU_DATA_0
     {0x098C, 0xAB4B}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_15]
     {0x0990, 0x00EF}, 	// MCU_DATA_0
     {0x098C, 0xAB4C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_16]
     {0x0990, 0x00F5}, 	// MCU_DATA_0
     {0x098C, 0xAB4D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_17]
     {0x0990, 0x00FA}, 	// MCU_DATA_0
     {0x098C, 0xAB4E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_18]
     {0x0990, 0x00FF}, 	// MCU_DATA_0
},        // saturation 0
    {
    {0x098C, 0xAB3C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_0]
    {0x0990, 0x0000}, 	// MCU_DATA_0
    {0x098C, 0xAB3D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_1]
    {0x0990, 0x0005}, 	// MCU_DATA_0
    {0x098C, 0xAB3E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_2]
    {0x0990, 0x0010}, 	// MCU_DATA_0
    {0x098C, 0xAB3F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_3]
    {0x0990, 0x0029}, 	// MCU_DATA_0
    {0x098C, 0xAB40}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_4]
    {0x0990, 0x0049}, 	// MCU_DATA_0
    {0x098C, 0xAB41}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_5]
    {0x0990, 0x0062}, 	// MCU_DATA_0
    {0x098C, 0xAB42}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_6]
    {0x0990, 0x0078}, 	// MCU_DATA_0
    {0x098C, 0xAB43}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_7]
    {0x0990, 0x008D}, 	// MCU_DATA_0
    {0x098C, 0xAB44}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_8]
    {0x0990, 0x009E}, 	// MCU_DATA_0
    {0x098C, 0xAB45}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_9]
    {0x0990, 0x00AD}, 	// MCU_DATA_0
    {0x098C, 0xAB46}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_10]
    {0x0990, 0x00BA}, 	// MCU_DATA_0
    {0x098C, 0xAB47}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_11]
    {0x0990, 0x00C6}, 	// MCU_DATA_0
    {0x098C, 0xAB48}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_12]
    {0x0990, 0x00D0}, 	// MCU_DATA_0
    {0x098C, 0xAB49}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_13]
    {0x0990, 0x00DA}, 	// MCU_DATA_0
    {0x098C, 0xAB4A}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_14]
    {0x0990, 0x00E2}, 	// MCU_DATA_0
    {0x098C, 0xAB4B}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_15]
    {0x0990, 0x00EA}, 	// MCU_DATA_0
    {0x098C, 0xAB4C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_16]
    {0x0990, 0x00F2}, 	// MCU_DATA_0
    {0x098C, 0xAB4D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_17]
    {0x0990, 0x00F9}, 	// MCU_DATA_0
    {0x098C, 0xAB4E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_18]
    {0x0990, 0x00FF}, 	// MCU_DATA_0
    {0x098C, 0xAB4F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_0]
    {0x0990, 0x0000}, 	// MCU_DATA_0
    {0x098C, 0xAB50}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_1]
    {0x0990, 0x0005}, 	// MCU_DATA_0
    {0x098C, 0xAB51}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_2]
    {0x0990, 0x0010}, 	// MCU_DATA_0
    {0x098C, 0xAB52}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_3]
    {0x0990, 0x0029}, 	// MCU_DATA_0
    {0x098C, 0xAB53}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_4]
    {0x0990, 0x0049}, 	// MCU_DATA_0
    {0x098C, 0xAB54}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_5]
    {0x0990, 0x0062}, 	// MCU_DATA_0
    {0x098C, 0xAB55}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_6]
    {0x0990, 0x0078}, 	// MCU_DATA_0
    {0x098C, 0xAB56}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_7]
    {0x0990, 0x008D}, 	// MCU_DATA_0
    {0x098C, 0xAB57}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_8]
    {0x0990, 0x009E}, 	// MCU_DATA_0
    {0x098C, 0xAB58}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_9]
    {0x0990, 0x00AD}, 	// MCU_DATA_0
    {0x098C, 0xAB59}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_10]
    {0x0990, 0x00BA}, 	// MCU_DATA_0
    {0x098C, 0xAB5A}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_11]
    {0x0990, 0x00C6}, 	// MCU_DATA_0
    {0x098C, 0xAB5B}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_12]
    {0x0990, 0x00D0}, 	// MCU_DATA_0
    {0x098C, 0xAB5C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_13]
    {0x0990, 0x00DA}, 	// MCU_DATA_0
    {0x098C, 0xAB5D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_14]
    {0x0990, 0x00E2}, 	// MCU_DATA_0
    {0x098C, 0xAB5E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_15]
    {0x0990, 0x00EA}, 	// MCU_DATA_0
    {0x098C, 0xAB5F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_16]
    {0x0990, 0x00F2}, 	// MCU_DATA_0
    {0x098C, 0xAB60}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_17]
    {0x0990, 0x00F9}, 	// MCU_DATA_0
    {0x098C, 0xAB61}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_18]
    {0x0990, 0x00FF}, 	// MCU_DATA_0
},        // saturation +1
    {
     {0x098C, 0xAB3C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_0]
     {0x0990, 0x0000}, 	// MCU_DATA_0
     {0x098C, 0xAB3D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_1]
     {0x0990, 0x0003}, 	// MCU_DATA_0
     {0x098C, 0xAB3E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_2]
     {0x0990, 0x000A}, 	// MCU_DATA_0
     {0x098C, 0xAB3F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_3]
     {0x0990, 0x001C}, 	// MCU_DATA_0
     {0x098C, 0xAB40}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_4]
     {0x0990, 0x0036}, 	// MCU_DATA_0
     {0x098C, 0xAB41}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_5]
     {0x0990, 0x004D}, 	// MCU_DATA_0
     {0x098C, 0xAB42}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_6]
     {0x0990, 0x0063}, 	// MCU_DATA_0
     {0x098C, 0xAB43}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_7]
     {0x0990, 0x0078}, 	// MCU_DATA_0
     {0x098C, 0xAB44}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_8]
     {0x0990, 0x008D}, 	// MCU_DATA_0
     {0x098C, 0xAB45}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_9]
     {0x0990, 0x009E}, 	// MCU_DATA_0
     {0x098C, 0xAB46}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_10]
     {0x0990, 0x00AE}, 	// MCU_DATA_0
     {0x098C, 0xAB47}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_11]
     {0x0990, 0x00BC}, 	// MCU_DATA_0
     {0x098C, 0xAB48}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_12]
     {0x0990, 0x00C8}, 	// MCU_DATA_0
     {0x098C, 0xAB49}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_13]
     {0x0990, 0x00D3}, 	// MCU_DATA_0
     {0x098C, 0xAB4A}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_14]
     {0x0990, 0x00DD}, 	// MCU_DATA_0
     {0x098C, 0xAB4B}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_15]
     {0x0990, 0x00E7}, 	// MCU_DATA_0
     {0x098C, 0xAB4C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_16]
     {0x0990, 0x00EF}, 	// MCU_DATA_0
     {0x098C, 0xAB4D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_17]
     {0x0990, 0x00F7}, 	// MCU_DATA_0
     {0x098C, 0xAB4E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_A_18]
     {0x0990, 0x00FF}, 	// MCU_DATA_0
     {0x098C, 0xAB4F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_0]
     {0x0990, 0x0000}, 	// MCU_DATA_0
     {0x098C, 0xAB50}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_1]
     {0x0990, 0x0003}, 	// MCU_DATA_0
     {0x098C, 0xAB51}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_2]
     {0x0990, 0x000A}, 	// MCU_DATA_0
     {0x098C, 0xAB52}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_3]
     {0x0990, 0x001C}, 	// MCU_DATA_0
     {0x098C, 0xAB53}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_4]
     {0x0990, 0x0036}, 	// MCU_DATA_0
     {0x098C, 0xAB54}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_5]
     {0x0990, 0x004D}, 	// MCU_DATA_0
     {0x098C, 0xAB55}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_6]
     {0x0990, 0x0063}, 	// MCU_DATA_0
     {0x098C, 0xAB56}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_7]
     {0x0990, 0x0078}, 	// MCU_DATA_0
     {0x098C, 0xAB57}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_8]
     {0x0990, 0x008D}, 	// MCU_DATA_0
     {0x098C, 0xAB58}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_9]
     {0x0990, 0x009E}, 	// MCU_DATA_0
     {0x098C, 0xAB59}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_10]
     {0x0990, 0x00AE}, 	// MCU_DATA_0
     {0x098C, 0xAB5A}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_11]
     {0x0990, 0x00BC}, 	// MCU_DATA_0
     {0x098C, 0xAB5B}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_12]
     {0x0990, 0x00C8}, 	// MCU_DATA_0
     {0x098C, 0xAB5C}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_13]
     {0x0990, 0x00D3}, 	// MCU_DATA_0
     {0x098C, 0xAB5D}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_14]
     {0x0990, 0x00DD}, 	// MCU_DATA_0
     {0x098C, 0xAB5E}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_15]
     {0x0990, 0x00E7}, 	// MCU_DATA_0
     {0x098C, 0xAB5F}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_16]
     {0x0990, 0x00EF}, 	// MCU_DATA_0
     {0x098C, 0xAB60}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_17]
     {0x0990, 0x00F7}, 	// MCU_DATA_0
     {0x098C, 0xAB61}, 	// MCU_ADDRESS [HG_GAMMA_TABLE_B_18]
     {0x0990, 0x00FF}, 	// MCU_DATA_0
	}         // saturation +2
};
static struct msm_camera_i2c_reg_conf mt9d115_sharpness[][2] = {
    {{0x098C, 0xAB22,},{0x0990, 0x0000,}},        // saturation -2
    {{0x098C, 0xAB22,},{0x0990, 0x0002,}},        // saturation -1
    {{0x098C, 0xAB22,},{0x0990, 0x0003,}},        // saturation 0
    {{0x098C, 0xAB22,},{0x0990, 0x0004,}},        // saturation +1
    {{0x098C, 0xAB22,},{0x0990, 0x0007,}}         // saturation +2
};
static struct msm_camera_i2c_reg_conf mt9d115_brightness[][6] = {
    {{0x098C, 0xA75D},{0x0990, 0x0000},{0x098C, 0xA75E},{0x0990, 0x0000},{0x098C, 0xA103},{0x0990, 0x0005}},        // brightness -2
    {{0x098C, 0xA75D},{0x0990, 0x0010},{0x098C, 0xA75E},{0x0990, 0x0010},{0x098C, 0xA103},{0x0990, 0x0005}},        // brightness -1
    {{0x098C, 0xA75D},{0x0990, 0x0020},{0x098C, 0xA75E},{0x0990, 0x0020},{0x098C, 0xA103},{0x0990, 0x0005}},        // brightness 0
    {{0x098C, 0xA75D},{0x0990, 0x0030},{0x098C, 0xA75E},{0x0990, 0x0030},{0x098C, 0xA103},{0x0990, 0x0005}},        // brightness +1
    {{0x098C, 0xA75D},{0x0990, 0x0040},{0x098C, 0xA75E},{0x0990, 0x0040},{0x098C, 0xA103},{0x0990, 0x0005}}         // brightness +2
};
static struct msm_camera_i2c_reg_conf mt9d115_exposure[][2] = {
    {{0x098C, 0xA24F,},{0x0990, 0x0022,}},        
    {{0x098C, 0xA24F,},{0x0990, 0x0036,}},        
    {{0x098C, 0xA24F,},{0x0990, 0x0040,}},  //2013.06.26 46-40      
    {{0x098C, 0xA24F,},{0x0990, 0x0056,}},       
    {{0x098C, 0xA24F,},{0x0990, 0x0070,}}         
};
static struct msm_camera_i2c_reg_conf mt9d115_awb[][12] = {
    {//AUTO
	{0x098C, 0xA34A,},
	{0x0990, 0x0059,},
	{0x098C, 0xA34B,},
	{0x0990, 0x00B6,},
	{0x098C, 0xA34C,},
	{0x0990, 0x0059,},
	{0x098C, 0xA34D,},
	{0x0990, 0x00B5,},
	{0x098C, 0xA351,},
	{0x0990, 0x0000,},
	{0x098C, 0xA352,},
	{0x0990, 0x007F,},
    },        // 
{//A LIGHT  ÎÙË¿µÆ
	{0x098C, 0xA34A,},
	{0x0990, 0x006E,},
	{0x098C, 0xA34B,},
	{0x0990, 0x006E,},
	{0x098C, 0xA34C,},
	{0x0990, 0x00B3,},
	{0x098C, 0xA34D,},
	{0x0990, 0x00B5,},
	{0x098C, 0xA351,},
	{0x0990, 0x0000,},
	{0x098C, 0xA352,},
	{0x0990, 0x0000,},
    },
	{//U30
      { 0x098C, 0xA34A,}, 	// MCU_ADDRESS [AWB_GAIN_MIN]
      { 0x0990, 0x0085 ,},	// MCU_DATA_0
      { 0x098C, 0xA34B,},	// MCU_ADDRESS [AWB_GAIN_MAX]
      { 0x0990, 0x0085,}, 	// MCU_DATA_0
      { 0x098C, 0xA34C ,},	// MCU_ADDRESS [AWB_GAINMIN_B]
      { 0x0990, 0x0097 ,},	// MCU_DATA_0
      { 0x098C, 0xA34D ,},	// MCU_ADDRESS [AWB_GAINMAX_B]
      { 0x0990, 0x0097 ,},	// MCU_DATA_0
      { 0x098C, 0xA351 ,},	// MCU_ADDRESS [AWB_CCM_POSITION_MIN]
      { 0x0990, 0x0000 ,},	// MCU_DATA_0
      { 0x098C, 0xA352 ,},	// MCU_ADDRESS [AWB_CCM_POSITION_MAX]
      { 0x0990, 0x0000 ,},	// MCU_DATA_0
    },
	{//CWF
         { 0x098C, 0xA34A ,},	// MCU_ADDRESS [AWB_GAIN_MIN]
         { 0x0990, 0x0097,}, 	// MCU_DATA_0
         { 0x098C, 0xA34B ,},	// MCU_ADDRESS [AWB_GAIN_MAX]
         { 0x0990, 0x0097 ,},	// MCU_DATA_0
         { 0x098C, 0xA34C,}, 	// MCU_ADDRESS [AWB_GAINMIN_B]
         { 0x0990, 0x00a0,}, 	// MCU_DATA_0
         { 0x098C, 0xA34D ,},	// MCU_ADDRESS [AWB_GAINMAX_B]
         { 0x0990, 0x00a0 ,},	// MCU_DATA_0
         { 0x098C, 0xA351,}, 	// MCU_ADDRESS [AWB_CCM_POSITION_MIN]
         { 0x0990, 0x0000 ,},	// MCU_DATA_0
         { 0x098C, 0xA352 ,},	// MCU_ADDRESS [AWB_CCM_POSITION_MAX]
         { 0x0990, 0x0000 ,},	// MCU_DATA_0
    },
      {//CLOUDY
      	{0x098C, 0xA34A,},
      	{0x0990, 0x00A2,},
      	{0x098C, 0xA34B,},
      	{0x0990, 0x00A6,},
      	{0x098C, 0xA34C,},
      	{0x0990, 0x0072,},
      	{0x098C, 0xA34D,},
      	{0x0990, 0x0074,},
      	{0x098C, 0xA351,},
      	{0x0990, 0x007F,},
      	{0x098C, 0xA352,},
      	{0x0990, 0x007F,},
          },
     {//SUNNY
     	{0x098C, 0xA34A,},
     	{0x0990, 0x0099,},
     	{0x098C, 0xA34B,},
     	{0x0990, 0x009B,},
     	{0x098C, 0xA34C,},
     	{0x0990, 0x007E,},
     	{0x098C, 0xA34D,},
     	{0x0990, 0x0080,},
     	{0x098C, 0xA351,},
     	{0x0990, 0x007F,},
     	{0x098C, 0xA352,},
     	{0x0990, 0x007F,},
         },
 };
static struct msm_camera_i2c_reg_conf mt9d115_effect[][4] = {
    {{0x098C, 0x2759,},{0x0990, 0x6440,},{0x098C, 0x275B,},{0x0990, 0x6440,}},        // NORMAL -2
    {{0x098C, 0x2759,},{0x0990, 0x6441,},{0x098C, 0x275B,},{0x0990, 0x6441,}},        // Mono -2
    {{0x098C, 0x2759,},{0x0990, 0x6443,},{0x098C, 0x275B,},{0x0990, 0x6443,}},        // Negative -2
    {{0x098C, 0x2759,},{0x0990, 0x6442,},{0x098C, 0x275B,},{0x0990, 0x6442,}},        // Sepia -2
    /*{{0x098C, 0x2759,},{0x0990, 0x6444,},{0x098C, 0x275B,},{0x0990, 0x6444,}},        // Solarize -2
    {{0x098C, 0x2759,},{0x0990, 0x6445,},{0x098C, 0x275B,},{0x0990, 0x6445,}},        // Solarize 2 -2
    {{0x098C, 0x2759,},{0x0990, 0x6440,},{0x098C, 0x275B,},{0x0990, 0x6440,}},        // NORMAL -2
    {{0x098C, 0x2759,},{0x0990, 0x6440,},{0x098C, 0x275B,},{0x0990, 0x6440,}},        // NORMAL -2
    */
};
static struct msm_camera_i2c_reg_conf mt9d115_antibanding[][10] = {
     {//auto
       { 0x098C, 0xA118},  	// MCU_ADDRESS [SEQ_PREVIEW_0_FD]
       { 0x0990, 0x0001},  	// MCU_DATA_0
       { 0x098C, 0xA11E},  	// MCU_ADDRESS [SEQ_PREVIEW_1_FD]
       { 0x0990, 0x0001},  	// MCU_DATA_0
       { 0x098C, 0xA124},  	// MCU_ADDRESS [SEQ_PREVIEW_2_FD]
       { 0x0990, 0x0000},  	// MCU_DATA_0
       { 0x098C, 0xA12A},  	// MCU_ADDRESS [SEQ_PREVIEW_3_FD]
       { 0x0990, 0x0001},  	// MCU_DATA_0
    },
      {//60-hz
      {0x098C, 0xA118}, 	// MCU_ADDRESS [SEQ_PREVIEW_0_FD]
      {0x0990, 0x0002}, 	// MCU_DATA_0
      {0x098C, 0xA11E}, 	// MCU_ADDRESS [SEQ_PREVIEW_1_FD]
      {0x0990, 0x0002}, 	// MCU_DATA_0
      {0x098C, 0xA124}, 	// MCU_ADDRESS [SEQ_PREVIEW_2_FD]
      {0x0990, 0x0002}, 	// MCU_DATA_0
      {0x098C, 0xA12A}, 	// MCU_ADDRESS [SEQ_PREVIEW_3_FD]
      {0x0990, 0x0002}, 	// MCU_DATA_0
      {0x098C, 0xA404}, 	// MCU_ADDRESS [FD_MODE]
      {0x0990, 0x00A0}, 	// MCU_DATA_0
    },
    {//50hz
      {0x098C, 0xA118},	// MCU_ADDRESS [SEQ_PREVIEW_0_FD]
      {0x0990, 0x0002},	// MCU_DATA_0
      {0x098C, 0xA11E},	// MCU_ADDRESS [SEQ_PREVIEW_1_FD]
      {0x0990, 0x0002},	// MCU_DATA_0
      {0x098C, 0xA124},	// MCU_ADDRESS [SEQ_PREVIEW_2_FD]
      {0x0990, 0x0002},	// MCU_DATA_0
      {0x098C, 0xA12A},	// MCU_ADDRESS [SEQ_PREVIEW_3_FD]
      {0x0990, 0x0002},	// MCU_DATA_0
      {0x098C, 0xA404},	// MCU_ADDRESS [FD_MODE]
      {0x0990, 0x00E0},	// MCU_DATA_0
    },        // 
};
static struct msm_camera_i2c_reg_conf mt9d115_iso[][4] = {
    {{0x098C, 0xa20d,},{0x0990, 0x0020,},{0x098C, 0xa20e,},{0x0990, 0x0090,}},        // auto -2
    {{0x098C, 0xa20d,},{0x0990, 0x0020,},{0x098C, 0xa20e,},{0x0990, 0x0028,}},        // 100 -2
    {{0x098C, 0xa20d,},{0x0990, 0x0040,},{0x098C, 0xa20e,},{0x0990, 0x0048,}},        // 200 -2
    {{0x098C, 0xa20d,},{0x0990, 0x0050,},{0x098C, 0xa20e,},{0x0990, 0x0080,}},        // 100 -2
};
#endif
static struct msm_camera_i2c_conf_array mt9d115_confs[] = {
	{&capture_mode_setting[0],
	ARRAY_SIZE(capture_mode_setting), 0, MSM_CAMERA_I2C_WORD_DATA},	   
	{&preview_mode_setting[0],
	ARRAY_SIZE(preview_mode_setting), 0, MSM_CAMERA_I2C_WORD_DATA},
};

static struct msm_camera_i2c_conf_array mt9d115_saturation_confs[][2] = {
	{{mt9d115_saturation[0],
		ARRAY_SIZE(mt9d115_saturation[0]), 0, MSM_CAMERA_I2C_WORD_DATA},
	{mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_saturation[1],
		ARRAY_SIZE(mt9d115_saturation[1]), 0, MSM_CAMERA_I2C_WORD_DATA},
	{mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_saturation[2],
		ARRAY_SIZE(mt9d115_saturation[2]), 0, MSM_CAMERA_I2C_WORD_DATA},
	{mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_saturation[3],
		ARRAY_SIZE(mt9d115_saturation[3]), 0, MSM_CAMERA_I2C_WORD_DATA},
	{mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_saturation[4],
		ARRAY_SIZE(mt9d115_saturation[4]), 0, MSM_CAMERA_I2C_WORD_DATA},
	{mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
};

static int mt9d115_saturation_enum_map[] = {
	MSM_V4L2_SATURATION_L0,
	MSM_V4L2_SATURATION_L1,
	MSM_V4L2_SATURATION_L2,
	MSM_V4L2_SATURATION_L3,
	MSM_V4L2_SATURATION_L4,

};

static struct msm_camera_i2c_enum_conf_array mt9d115_saturation_enum_confs = {
	.conf = &mt9d115_saturation_confs[0][0],
	.conf_enum = mt9d115_saturation_enum_map,
	.num_enum = ARRAY_SIZE(mt9d115_saturation_enum_map),
	.num_index = ARRAY_SIZE(mt9d115_saturation_confs),
	.num_conf = ARRAY_SIZE(mt9d115_saturation_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};

static struct msm_camera_i2c_conf_array mt9d115_contrast_confs[][1] = {
	{{mt9d115_contrast[0],
		ARRAY_SIZE(mt9d115_contrast[0]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_contrast[1],
		ARRAY_SIZE(mt9d115_contrast[1]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_contrast[2],
		ARRAY_SIZE(mt9d115_contrast[2]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_contrast[3],
		ARRAY_SIZE(mt9d115_contrast[3]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_contrast[4],
		ARRAY_SIZE(mt9d115_contrast[4]), 0, MSM_CAMERA_I2C_WORD_DATA},},
};
static int mt9d115_contrast_enum_map[] = {
	CAMERA_CONTRAST_LV0,
	CAMERA_CONTRAST_LV1,
	CAMERA_CONTRAST_LV2,
	CAMERA_CONTRAST_LV3,
	CAMERA_CONTRAST_LV4,	
};
static struct msm_camera_i2c_enum_conf_array mt9d115_contrast_enum_confs = {
	.conf = &mt9d115_contrast_confs[0][0],
	.conf_enum = mt9d115_contrast_enum_map,
	.num_enum = ARRAY_SIZE(mt9d115_contrast_enum_map),
	.num_index = ARRAY_SIZE(mt9d115_contrast_confs),
	.num_conf = ARRAY_SIZE(mt9d115_contrast_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};
static struct msm_camera_i2c_conf_array mt9d115_sharpness_confs[][1] = {
	{{mt9d115_sharpness[0],
		ARRAY_SIZE(mt9d115_sharpness[0]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_sharpness[1],
		ARRAY_SIZE(mt9d115_sharpness[1]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_sharpness[2],
		ARRAY_SIZE(mt9d115_sharpness[2]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_sharpness[3],
		ARRAY_SIZE(mt9d115_sharpness[3]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_sharpness[4],
		ARRAY_SIZE(mt9d115_sharpness[4]), 0, MSM_CAMERA_I2C_WORD_DATA},},
};
static int mt9d115_sharpness_enum_map[] = {
	MSM_V4L2_SHARPNESS_L0,
	MSM_V4L2_SHARPNESS_L1,
	MSM_V4L2_SHARPNESS_L2,
	MSM_V4L2_SHARPNESS_L3,
	MSM_V4L2_SHARPNESS_L4,	
};
static struct msm_camera_i2c_enum_conf_array mt9d115_sharpness_enum_confs = {
	.conf = &mt9d115_sharpness_confs[0][0],
	.conf_enum = mt9d115_sharpness_enum_map,
	.num_enum = ARRAY_SIZE(mt9d115_sharpness_enum_map),
	.num_index = ARRAY_SIZE(mt9d115_sharpness_confs),
	.num_conf = ARRAY_SIZE(mt9d115_sharpness_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};
static struct msm_camera_i2c_conf_array mt9d115_brightness_confs[][1] = {
	{{mt9d115_brightness[0],
		ARRAY_SIZE(mt9d115_brightness[0]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_brightness[1],
		ARRAY_SIZE(mt9d115_brightness[1]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_brightness[2],
		ARRAY_SIZE(mt9d115_brightness[2]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_brightness[3],
		ARRAY_SIZE(mt9d115_brightness[3]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_brightness[4],
		ARRAY_SIZE(mt9d115_brightness[4]), 0, MSM_CAMERA_I2C_WORD_DATA},},
};
static int mt9d115_brightness_enum_map[] = {
	MSM_V4L2_BRIGHTNESS_L0,
	MSM_V4L2_BRIGHTNESS_L1,
	MSM_V4L2_BRIGHTNESS_L2,
	MSM_V4L2_BRIGHTNESS_L3,
	MSM_V4L2_BRIGHTNESS_L4,	
};
static struct msm_camera_i2c_enum_conf_array mt9d115_brightness_enum_confs = {
	.conf = &mt9d115_brightness_confs[0][0],
	.conf_enum = mt9d115_brightness_enum_map,
	.num_enum = ARRAY_SIZE(mt9d115_brightness_enum_map),
	.num_index = ARRAY_SIZE(mt9d115_brightness_confs),
	.num_conf = ARRAY_SIZE(mt9d115_brightness_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};
static struct msm_camera_i2c_conf_array mt9d115_exposure_confs[][1] = {
	{{mt9d115_exposure[0],
		ARRAY_SIZE(mt9d115_exposure[0]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_exposure[1],
		ARRAY_SIZE(mt9d115_exposure[1]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_exposure[2],
		ARRAY_SIZE(mt9d115_exposure[2]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_exposure[3],
		ARRAY_SIZE(mt9d115_exposure[3]), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_exposure[4],
		ARRAY_SIZE(mt9d115_exposure[4]), 0, MSM_CAMERA_I2C_WORD_DATA},},
};
static int mt9d115_exposure_enum_map[] = {
	MSM_V4L2_EXPOSURE_N2,
	MSM_V4L2_EXPOSURE_N1,
	MSM_V4L2_EXPOSURE_D,
	MSM_V4L2_EXPOSURE_P1,
	MSM_V4L2_EXPOSURE_P2,
};
static struct msm_camera_i2c_enum_conf_array mt9d115_exposure_enum_confs = {
	.conf = &mt9d115_exposure_confs[0][0],
	.conf_enum = mt9d115_exposure_enum_map,
	.num_enum = ARRAY_SIZE(mt9d115_exposure_enum_map),
	.num_index = ARRAY_SIZE(mt9d115_exposure_confs),
	.num_conf = ARRAY_SIZE(mt9d115_exposure_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};
static struct msm_camera_i2c_conf_array mt9d115_effect_confs[][2] = {
	{{mt9d115_effect[0],
		ARRAY_SIZE(mt9d115_effect[0]), 0, MSM_CAMERA_I2C_WORD_DATA},	
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_effect[1],
		ARRAY_SIZE(mt9d115_effect[1]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_effect[2],
		ARRAY_SIZE(mt9d115_effect[2]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_effect[3],
		ARRAY_SIZE(mt9d115_effect[3]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	/*{{mt9d115_effect[4],
		ARRAY_SIZE(mt9d115_effect[4]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_effect[0],
		ARRAY_SIZE(mt9d115_effect[0]), 0, MSM_CAMERA_I2C_WORD_DATA},	
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_effect[0],
		ARRAY_SIZE(mt9d115_effect[0]), 0, MSM_CAMERA_I2C_WORD_DATA},	
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_effect[0],
		ARRAY_SIZE(mt9d115_effect[0]), 0, MSM_CAMERA_I2C_WORD_DATA},	
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	*/	
};
static int mt9d115_effect_enum_map[] = {
	CAMERA_EFFECT_OFF,
	CAMERA_EFFECT_MONO,
	CAMERA_EFFECT_NEGATIVE,
	CAMERA_EFFECT_SEPIA,
};
static struct msm_camera_i2c_enum_conf_array mt9d115_effect_enum_confs = {
	.conf = &mt9d115_effect_confs[0][0],
	.conf_enum = mt9d115_effect_enum_map,
	.num_enum = ARRAY_SIZE(mt9d115_effect_enum_map),
	.num_index = ARRAY_SIZE(mt9d115_effect_confs),
	.num_conf = ARRAY_SIZE(mt9d115_effect_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};
static struct msm_camera_i2c_conf_array mt9d115_awb_confs[][2] = {
	{{mt9d115_awb[0],
		ARRAY_SIZE(mt9d115_awb[0]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_awb[0],
		ARRAY_SIZE(mt9d115_awb[0]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_awb[0],
		ARRAY_SIZE(mt9d115_awb[0]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_awb[1],
		ARRAY_SIZE(mt9d115_awb[1]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_awb[3],
		ARRAY_SIZE(mt9d115_awb[3]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_awb[4],
		ARRAY_SIZE(mt9d115_awb[4]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_awb[5],
		ARRAY_SIZE(mt9d115_awb[5]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
};
static int mt9d115_awb_enum_map[] = {
	MSM_V4L2_WB_OFF,//not used
	MSM_V4L2_WB_AUTO ,//= 1
	MSM_V4L2_WB_CUSTOM,  //not used
	MSM_V4L2_WB_INCANDESCENT, //°×³ã
	MSM_V4L2_WB_FLUORESCENT,   //Ó«¹â
	MSM_V4L2_WB_DAYLIGHT,
	MSM_V4L2_WB_CLOUDY_DAYLIGHT,
};
static struct msm_camera_i2c_enum_conf_array mt9d115_awb_enum_confs = {
	.conf = &mt9d115_awb_confs[0][0],
	.conf_enum = mt9d115_awb_enum_map,
	.num_enum = ARRAY_SIZE(mt9d115_awb_enum_map),
	.num_index = ARRAY_SIZE(mt9d115_awb_confs),
	.num_conf = ARRAY_SIZE(mt9d115_awb_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};
static struct msm_camera_i2c_conf_array mt9d115_antibanding_confs[][2] = {
	{{mt9d115_antibanding[0],
		ARRAY_SIZE(mt9d115_antibanding[0]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_antibanding[1],
		ARRAY_SIZE(mt9d115_antibanding[1]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_antibanding[2],
		ARRAY_SIZE(mt9d115_antibanding[2]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_antibanding[0],
		ARRAY_SIZE(mt9d115_antibanding[0]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
};
static int mt9d115_antibanding_enum_map[] = {
	MSM_V4L2_POWER_LINE_OFF,  //not used
	MSM_V4L2_POWER_LINE_60HZ,
	MSM_V4L2_POWER_LINE_50HZ,
	MSM_V4L2_POWER_LINE_AUTO,
};
static struct msm_camera_i2c_enum_conf_array mt9d115_antibanding_enum_confs = {
	.conf = &mt9d115_antibanding_confs[0][0],
	.conf_enum = mt9d115_antibanding_enum_map,
	.num_enum = ARRAY_SIZE(mt9d115_antibanding_enum_map),
	.num_index = ARRAY_SIZE(mt9d115_antibanding_confs),
	.num_conf = ARRAY_SIZE(mt9d115_antibanding_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};
static struct msm_camera_i2c_conf_array mt9d115_iso_confs[][2] = {
	{{mt9d115_iso[0],
		ARRAY_SIZE(mt9d115_iso[0]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_iso[1],
		ARRAY_SIZE(mt9d115_iso[1]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_iso[2],
		ARRAY_SIZE(mt9d115_iso[2]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9d115_iso[3],
		ARRAY_SIZE(mt9d115_iso[3]), 0, MSM_CAMERA_I2C_WORD_DATA},
	   {mt9d115_refresh,
		ARRAY_SIZE(mt9d115_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
};
static int mt9d115_iso_enum_map[] = {
	MSM_V4L2_ISO_AUTO,
	MSM_V4L2_ISO_100,
	MSM_V4L2_ISO_200,
	MSM_V4L2_ISO_400,
};
static struct msm_camera_i2c_enum_conf_array mt9d115_iso_enum_confs = {
	.conf = &mt9d115_iso_confs[0][0],
	.conf_enum = mt9d115_iso_enum_map,
	.num_enum = ARRAY_SIZE(mt9d115_iso_enum_map),
	.num_index = ARRAY_SIZE(mt9d115_iso_confs),
	.num_conf = ARRAY_SIZE(mt9d115_iso_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};

struct msm_sensor_v4l2_ctrl_info_t mt9d115_v4l2_ctrl_info[] = {
	{
		.ctrl_id = V4L2_CID_SATURATION,
		.min = MSM_V4L2_SATURATION_L0,
		//.max = MSM_V4L2_SATURATION_L10,
		.max = MSM_V4L2_SATURATION_L4,
		.step = 1,
		.enum_cfg_settings = &mt9d115_saturation_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_CONTRAST,
		.min = CAMERA_CONTRAST_LV0,
		.max = CAMERA_CONTRAST_LV4,
		.step = 1,
		.enum_cfg_settings = &mt9d115_contrast_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_SHARPNESS,
		.min = MSM_V4L2_SHARPNESS_L0,
		.max = MSM_V4L2_SHARPNESS_L4,
		.step = 1,
		.enum_cfg_settings = &mt9d115_sharpness_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},

	{
		.ctrl_id = V4L2_CID_BRIGHTNESS,
		.min = CAMERA_BRIGHTNESS_LV0,
		.max = CAMERA_BRIGHTNESS_LV4,
		.step = 1,
		.enum_cfg_settings = &mt9d115_brightness_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},
      { 
		.ctrl_id = V4L2_CID_EXPOSURE,
		.min = MSM_V4L2_EXPOSURE_N2,
		.max = MSM_V4L2_EXPOSURE_P2,
		.step = 1,
		.enum_cfg_settings = &mt9d115_exposure_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},  
	{
		.ctrl_id = V4L2_CID_COLORFX,
		.min = CAMERA_EFFECT_OFF,
		.max = CAMERA_EFFECT_SEPIA,
		.step = 1,
		.enum_cfg_settings = &mt9d115_effect_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_AUTO_WHITE_BALANCE,
		.min = MSM_V4L2_WB_AUTO,
		.max = MSM_V4L2_WB_CLOUDY_DAYLIGHT,
		.step = 1,
		.enum_cfg_settings = &mt9d115_awb_enum_confs,
        .s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum, 
	},	
	{
		.ctrl_id = V4L2_CID_POWER_LINE_FREQUENCY,//antibanding
		.min = MSM_V4L2_POWER_LINE_60HZ,
		.max = MSM_V4L2_POWER_LINE_AUTO,
		.step = 1,
		.enum_cfg_settings = &mt9d115_antibanding_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},  
       {
		.ctrl_id = V4L2_CID_ISO,//antibanding
		.min = MSM_V4L2_ISO_AUTO,
		.max = MSM_V4L2_ISO_400,
		.step = 1,
		.enum_cfg_settings = &mt9d115_iso_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,//zhangzhao 2012-8-24  solve iso cant save problem
	},  
};

static struct msm_sensor_output_info_t mt9d115_dimensions[] = {
	{
		.x_output = 0x640,  //1600
		.y_output = 0x4b0,  //1200
		.line_length_pclk = 0x640,
		.frame_length_lines = 0x4b0,
		.vt_pixel_clk = 48000000,//96000000,
		.op_pixel_clk = 128000000,//96000000,
		.binning_factor = 1,
	},	
	{
		.x_output = 0x320,  //800
		.y_output = 0x258,  //600
		.line_length_pclk = 0x320,
		.frame_length_lines = 0x258,
		.vt_pixel_clk = 48000000,//96000000,
		.op_pixel_clk = 128000000,//96000000,
		.binning_factor = 1,
	},
};

/*static struct msm_camera_csid_vc_cfg mt9d115_cid_cfg[] = {	
	{0, CSI_YUV422_8, CSI_DECODE_8BIT},
	{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params mt9d115_csi_params = {
	.csid_params = {
		.lane_assign = 0xe4,
		.lane_cnt = 1,
		.lut_params = {
			.num_cid = 2,
			.vc_cfg = mt9d115_cid_cfg,
		},
	},
	.csiphy_params = {
		.lane_cnt = 1,
		.settle_cnt = 0x14,
	},
};*/
static struct msm_camera_csi_params mt9d115_csi_params = {
	.data_format = CSI_8BIT,
	.lane_cnt    = 1,
	.lane_assign = 0xe4,
	.dpcm_scheme = 0,
	.settle_cnt  = 18,//0X5,0X24,
};

static struct msm_camera_csi_params *mt9d115_csi_params_array[] = {
	&mt9d115_csi_params,
	&mt9d115_csi_params,
};

static struct msm_sensor_output_reg_addr_t mt9d115_reg_addr = {
	.x_output = 0x2703,
	.y_output = 0x2705,
	.line_length_pclk = 0x2703,
	.frame_length_lines = 0x2705,
};

static struct msm_sensor_id_info_t mt9d115_id_info = {
	.sensor_id_reg_addr = 0x0000,
	.sensor_id = 0x2580,
};


static const struct i2c_device_id mt9d115_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&mt9d115_s_ctrl},
	{ }
};

static ssize_t camera_id_read_proc(char *page,char **start,off_t off,int count,int *eof,void* data)
{		 	
    int ret;
    unsigned char *camera_status = "BACK Camera ID:Aptina,MT9D115 2.0M";	
    ret = strlen(camera_status);	 	 
    sprintf(page,"%s\n",camera_status);	 	 
    return (ret + 1);	
}
static void camera_proc_file(void)
{	
    struct proc_dir_entry *proc_file  = create_proc_entry("driver/camera_id_back",0644,NULL);	
    if(proc_file)	
     {		
  	     proc_file->read_proc = camera_id_read_proc;			
     }	
    else	
     {		
        printk(KERN_INFO "camera_proc_file error!\r\n");	
     }
}
int32_t mt9d115_sensor_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int32_t rc = 0;
	pr_err("%s IN\r\n", __func__);
	rc = msm_sensor_i2c_probe(client, id);
    if(rc == 0)
    {
        camera_proc_file();
    }
	if (client->dev.platform_data == NULL) {
		pr_err("%s: NULL sensor data\n", __func__);
		return -EFAULT;
	}
	usleep_range(5000, 5100);
	return rc;
}
static struct i2c_driver mt9d115_i2c_driver = {
	.id_table = mt9d115_i2c_id,
	.probe  = mt9d115_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client mt9d115_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};
static int __init msm_sensor_init_module(void)
{
	//return platform_driver_register(&mt9d115_driver);  
	return i2c_add_driver(&mt9d115_i2c_driver);
}

static struct v4l2_subdev_core_ops mt9d115_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops mt9d115_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops mt9d115_subdev_ops = {
	.core = &mt9d115_subdev_core_ops,
	.video  = &mt9d115_subdev_video_ops,
};

int32_t mt9d115_sensor_setting1(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;
	static int csi_config;
	//static int flag_init;
	s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
	msleep(30); //wangminrong
	if (update_type == MSM_SENSOR_REG_INIT) {
		printk("Register INIT\n");
		s_ctrl->curr_csi_params = NULL;
		msm_sensor_enable_debugfs(s_ctrl);

		//flag_init =0;//msm_sensor_write_init_settings(s_ctrl);		
		csi_config = 0;
		if (!csi_config) {
			s_ctrl->curr_csic_params = s_ctrl->csic_params[res];
			printk("CSI config in progress\n");
			v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
				NOTIFY_CSIC_CFG,
				s_ctrl->curr_csic_params);
			printk("CSI config is done\n");
			mb();
			msleep(30);
			printk("init----- config is done\n");
			msm_sensor_write_init_settings(s_ctrl);
			msleep(30); //wangminrong
			csi_config = 1;
			printk("wangminrong add\r\n");
		}
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		printk("PERIODIC : %d\n", res);
		 printk("%s:sensor_name = %s, res = %d\n",__func__, s_ctrl->sensordata->sensor_name, res);
		msm_sensor_write_conf_array(
			s_ctrl->sensor_i2c_client,
			s_ctrl->msm_sensor_reg->mode_settings, res);
		msleep(30);
		if (!csi_config) {
			s_ctrl->curr_csic_params = s_ctrl->csic_params[res];
			printk("CSI config in 222 progress\n");
			v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
				NOTIFY_CSIC_CFG,
				s_ctrl->curr_csic_params);
			printk("CSI config is 111 done\n");
			mb();
			msleep(30);
			csi_config = 1;
		}
		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE,
			&s_ctrl->sensordata->pdata->ioclk.vfe_clk_rate);
		/*if (!flag_init) {
			msleep(30);
			printk("init----- config is done\n");
			msm_sensor_write_init_settings(s_ctrl);
			msleep(30);
			flag_init = 1;
		}*/
		s_ctrl->func_tbl->sensor_start_stream(s_ctrl);
		msleep(50); //wangminrong
	}
	return rc;
}


//#if defined (CONFIG_PROJECT_P865F06)
#if 0
static int32_t mt9d115_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *info = NULL;
	
	info = s_ctrl->sensordata;

	printk("%s: %d sensor_pwd:%d\n", __func__, __LINE__, info->sensor_pwd);

    if(info->sensor_pwd)
    {
        rc = gpio_request(info->sensor_pwd, "mt9d115_pwdn");
        if (rc < 0)
        {
            pr_err("%s: %d request sensor_pwd:%d failed\n", __func__, __LINE__, info->sensor_pwd);
        }
        
        rc = gpio_tlmm_config(GPIO_CFG(info->sensor_pwd, 0,
        GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
        GPIO_CFG_2MA), GPIO_CFG_ENABLE);
        if (rc < 0) {
            pr_err("%s: %d config sensor_pwd:%d failed\n", __func__, __LINE__, info->sensor_pwd);
            
            gpio_free(info->sensor_pwd);
            return rc;	
        }
        
		gpio_set_value_cansleep(info->sensor_pwd, 1);

		gpio_free(info->sensor_pwd);
    }	

    rc = msm_sensor_power_down(s_ctrl);
	if (rc < 0) {
		CDBG("%s: msm_sensor_power_down failed\n", __func__);
		return rc;
	}

    return rc;
}
#else
/* wangjianping added for power issue in sleep mode 20121126, start */
static int32_t mt9d115_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *info = NULL;

    rc = msm_sensor_power_down(s_ctrl);
	if (rc < 0) {
		CDBG("%s: msm_sensor_power_down failed\n", __func__);
		return rc;
	}
	
	info = s_ctrl->sensordata;

    if(info->vcm_pwd)
    {
        rc = gpio_request(info->vcm_pwd, "mt9d115_2v85");
        if (rc < 0)
        {
            pr_err("%s: %d request sensor_pwd:%d failed\n", __func__, __LINE__, info->sensor_pwd);
        }
        
        rc = gpio_tlmm_config(GPIO_CFG(info->vcm_pwd, 0,
        GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
        GPIO_CFG_2MA), GPIO_CFG_ENABLE);
        if (rc < 0) {
            pr_err("%s: %d config sensor_pwd:%d failed\n", __func__, __LINE__, info->sensor_pwd);
            
            gpio_free(info->vcm_pwd);
            return rc;	
        }
        
		gpio_set_value_cansleep(info->vcm_pwd, 1);

		gpio_free(info->vcm_pwd);
    }

    if(info->sensor_reset)
    {
        rc = gpio_request(info->sensor_reset, "mt9d115_reset");
        if (rc < 0)
        {
            pr_err("%s: %d request sensor_pwd:%d failed\n", __func__, __LINE__, info->sensor_pwd);
        }
        
        rc = gpio_tlmm_config(GPIO_CFG(info->sensor_reset, 0,
        GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
        GPIO_CFG_2MA), GPIO_CFG_ENABLE);
        if (rc < 0) {
            pr_err("%s: %d config sensor_pwd:%d failed\n", __func__, __LINE__, info->sensor_pwd);
            
            gpio_free(info->sensor_reset);
            return rc;	
        }
        
		gpio_set_value_cansleep(info->sensor_reset, 1);

		gpio_free(info->sensor_reset);
    }	

	printk("%s: %d sensor_pwd:%d\n", __func__, __LINE__, info->sensor_pwd);

    if(info->sensor_pwd)
    {
        rc = gpio_request(info->sensor_pwd, "mt9d115_pwdn");
        if (rc < 0)
        {
            pr_err("%s: %d request sensor_pwd:%d failed\n", __func__, __LINE__, info->sensor_pwd);
        }
        
        rc = gpio_tlmm_config(GPIO_CFG(info->sensor_pwd, 0,
        GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
        GPIO_CFG_2MA), GPIO_CFG_ENABLE);
        if (rc < 0) {
            pr_err("%s: %d config sensor_pwd:%d failed\n", __func__, __LINE__, info->sensor_pwd);
            
            gpio_free(info->sensor_pwd);
            return rc;	
        }
        
		gpio_set_value_cansleep(info->sensor_pwd, 1);

		gpio_free(info->sensor_pwd);
    }	

    return rc;
}
/* wangjianping added for power issue in sleep mode 20121126, end */
#endif

static struct msm_sensor_fn_t mt9d115_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream, 
	.sensor_csi_setting = mt9d115_sensor_setting1,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	//.sensor_open_init = mt9d115_sensor_open_init,
	//.sensor_release = mt9d115_sensor_release,
	.sensor_power_up = msm_sensor_power_up,
#if defined (CONFIG_PROJECT_P865F06)
	.sensor_power_down = mt9d115_sensor_power_down, 
#else
	.sensor_power_down = msm_sensor_power_down, 
#endif
	.sensor_get_csi_params = msm_sensor_get_csi_params,
};

static struct msm_sensor_reg_t mt9d115_regs = {
	.default_data_type = MSM_CAMERA_I2C_WORD_DATA,
	.start_stream_conf = mt9d115_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(mt9d115_start_settings),
	.stop_stream_conf = mt9d115_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(mt9d115_stop_settings),	
	.init_settings = &mt9d115_init_conf[0],
	.init_size = ARRAY_SIZE(mt9d115_init_conf),
	.mode_settings = &mt9d115_confs[0],
	.output_settings = &mt9d115_dimensions[0],
	.num_conf = ARRAY_SIZE(mt9d115_confs),
};

static struct msm_sensor_ctrl_t mt9d115_s_ctrl = {
	.msm_sensor_reg = &mt9d115_regs,
	.msm_sensor_v4l2_ctrl_info = mt9d115_v4l2_ctrl_info,
	.num_v4l2_ctrl = ARRAY_SIZE(mt9d115_v4l2_ctrl_info),
	.sensor_i2c_client = &mt9d115_sensor_i2c_client,
	.sensor_i2c_addr = 0x78,
	.sensor_output_reg_addr = &mt9d115_reg_addr,
	.sensor_id_info = &mt9d115_id_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.csic_params = &mt9d115_csi_params_array[0],
	.msm_sensor_mutex = &mt9d115_mut,
	.sensor_i2c_driver = &mt9d115_i2c_driver,
	.sensor_v4l2_subdev_info = mt9d115_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(mt9d115_subdev_info),
	.sensor_v4l2_subdev_ops = &mt9d115_subdev_ops,
	.func_tbl = &mt9d115_func_tbl,
	.clk_rate = 32000000,  //zhangzhao modified for V72 
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Aptina 2MP YUV sensor driver");
MODULE_LICENSE("GPL v2");
