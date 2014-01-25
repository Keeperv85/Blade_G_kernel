
/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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
#include <linux/proc_fs.h>  

#define SENSOR_NAME "t4k28"



#define FULLSIZE_OUTPUT

#ifndef FULLSIZE_OUTPUT
typedef enum
{  
      AE_enable,  
	AE_lock,  
	AE_disable,
} AE_status;

uint16_t set_pv_back_es = 0xff;
uint16_t set_pv_back_ag = 0x20;
uint16_t set_pv_back_dg = 0x10;
#endif

DEFINE_MUTEX(t4k28_mut);
static struct msm_sensor_ctrl_t t4k28_s_ctrl;

static struct msm_camera_i2c_reg_conf t4k28_start_settings[] = {
	//{0x3010,0x01},
};

static struct msm_camera_i2c_reg_conf t4k28_stop_settings[] = {
	//{0x3010,0x00},
};


static struct msm_camera_i2c_reg_conf t4k28_recommend_settings[] =
{

	{0x3000,0x08},
	{0x3001,0x40},
	{0x3002,0x00},
	{0x3003,0x00},
	{0x3004,0x00},
	{0x3005,0xB4},
	{0x3010,0x00},
	{0x3012,0x03},
	{0x3011,0x00},
	{0x3014,0x03},
	{0x3015,0x04},
	{0x3016,0x04},
	{0x3017,0x03},
	{0x3018,0x00},
	{0x3019,0x00},
	{0x301A,0x10},
	{0x301B,0x00},
	{0x301C,0x01},
	{0x3020,0x06},
	{0x3021,0x40},
	{0x3022,0x04},
	{0x3023,0xB0},
	{0x3025,0x00},
	{0x3026,0x00},
	{0x3027,0x01},
	{0x302C,0x00},
	{0x302D,0x00},
	{0x302E,0x00},
	{0x302F,0x00},
	{0x3030,0x00},
	{0x3031,0x02},
	{0x3032,0x00},
	{0x3033,0x83},
	{0x3034,0x01},
	{0x3037,0x00},
	{0x303C,0x80},
	{0x303E,0x00},
	{0x303F,0x00},
	{0x3040,0x80},
	{0x3044,0x02},
	{0x3045,0x04},
	{0x3046,0x00},
	{0x3047,0x80},
	{0x3048,0x04},
	{0x3049,0x01},
	{0x304A,0x04},
	{0x304B,0x0A},
	{0x304C,0x00},
	{0x304E,0x01},
	{0x3050,0x60},
	{0x3051,0x82},
	{0x3052,0x10},
	{0x3053,0x00},
	{0x3055,0x84},
	{0x3056,0x02},
	{0x3059,0x18},
	{0x305A,0x00},
	{0x3068,0xF0},
	{0x3069,0xF0},
	{0x306C,0x06},
	{0x306D,0x40},
	{0x306E,0x00},
	{0x306F,0x04},
	{0x3070,0x06},
	{0x3071,0x43},
	{0x3072,0x04},
	{0x3073,0xB0},
	{0x3074,0x00},
	{0x3075,0x04},
	{0x3076,0x04},
	{0x3077,0xB3},
	{0x307F,0x03},
	{0x3080,0x70},
	{0x3081,0x28},
	{0x3082,0x60},
	{0x3083,0x48},
	{0x3084,0x40},
	{0x3085,0x28},
	{0x3086,0xF8},
	{0x3087,0x38},
	{0x3088,0x03},
	{0x3089,0x02},
	{0x308A,0x58},
	{0x3091,0x00},
	{0x3092,0x10},
	{0x3093,0x6B},
	{0x3095,0x78},
	{0x3097,0x00},
	{0x3098,0x40},
	{0x309A,0x00},
	{0x309B,0x00},
	{0x309D,0x00},
	{0x309E,0x00},
	{0x309F,0x00},
	{0x30A0,0x02},
	{0x30A1,0x00},
	{0x30A2,0xA7},
	{0x30A3,0x20},
	{0x30A4,0xFF},
	{0x30A5,0x80},
	{0x30A6,0xFF},
	{0x30A7,0x00},
	{0x30A8,0x01},
	{0x30F1,0x00},
	{0x30F2,0x00},
	{0x30FE,0x80},
	{0x3100,0xD2},
	{0x3101,0xD3},
	{0x3102,0x45},
	{0x3103,0x80},
	{0x3104,0x31},
	{0x3105,0x02},
	{0x3106,0x23},
	{0x3107,0x20},
	{0x3108,0x7B},
	{0x3109,0x80},
	{0x310A,0x00},
	{0x310B,0x00},
	{0x3110,0x11},
	{0x3111,0x11},
	{0x3112,0x00},
	{0x3113,0x00},
	{0x3114,0x10},
	{0x3115,0x22},
	{0x3120,0x08},
	{0x3121,0x13},
	{0x3122,0x33},
	{0x3123,0x0E},
	{0x3124,0x26},
	{0x3125,0x00},
	{0x3126,0x0C},
	{0x3127,0x08},
	{0x3128,0x80},
	{0x3129,0x65},
	{0x312A,0x27},
	{0x312B,0x77},
	{0x312C,0x77},
	{0x312D,0x1A},
	{0x312E,0xB8},
	{0x312F,0x38},
	{0x3130,0x80},
	{0x3131,0x33},
	{0x3132,0x63},
	{0x3133,0x00},
	{0x3134,0xDD},
	{0x3135,0x07},
	{0x3136,0xB7},
	{0x3137,0x11},
	{0x3138,0x0B},
	{0x313B,0x0A},
	{0x313C,0x05},
	{0x313D,0x01},
	{0x313E,0x62},
	{0x313F,0x85},
	{0x3140,0x01},
	{0x3141,0x40},
	{0x3142,0x80},
	{0x3143,0x22},
	{0x3144,0x3E},
	{0x3145,0x32},
	{0x3146,0x2E},
	{0x3147,0x23},
	{0x3148,0x22},
	{0x3149,0x11},
	{0x314A,0x6B},
	{0x314B,0x30},
	{0x314C,0x69},
	{0x314D,0x80},
	{0x314E,0x31},
	{0x314F,0x32},
	{0x3150,0x32},
	{0x3151,0x03},
	{0x3152,0x0C},
	{0x3153,0xB3},
	{0x3154,0x20},
	{0x3155,0x13},
	{0x3156,0x66},
	{0x3157,0x02},
	{0x3158,0x03},
	{0x3159,0x01},
	{0x315A,0x16},
	{0x315B,0x10},
	{0x315C,0x00},
	{0x315D,0x44},
	{0x315E,0x1B},
	{0x315F,0x52},
	{0x3160,0x00},
	{0x3161,0x03},
	{0x3162,0x00},
	{0x3163,0xFF},
	{0x3164,0x00},
	{0x3165,0x01},
	{0x3166,0x00},
	{0x3167,0xFF},
	{0x3168,0x01},
	{0x3169,0x00},
	{0x3180,0x00},
	{0x3181,0x20},
	{0x3182,0x40},
	{0x3183,0x96},
	{0x3184,0x40},
	{0x3185,0x8F},
	{0x3186,0x31},
	{0x3187,0x06},
	{0x3188,0x0C},
	{0x3189,0x44},
	{0x318A,0x42},
	{0x318B,0x0B},
	{0x318C,0x11},
	{0x318D,0xAA},
	{0x318E,0x40},
	{0x318F,0x30},
	{0x3190,0x03},
	{0x3191,0x01},
	{0x3192,0x00},
	{0x3193,0x00},
	{0x3194,0x00},
	{0x3195,0x00},
	{0x3196,0x00},
	{0x3197,0xDE},
	{0x3198,0x00},
	{0x3199,0x00},
	{0x319A,0x00},
	{0x319B,0x00},
	{0x319C,0x16},
	{0x319D,0x0A},
	{0x31A0,0xBF},
	{0x31A1,0xFF},
	{0x31A2,0x11},
	{0x31B0,0x00},
	{0x31B1,0x42},
	{0x31B2,0x09},
	{0x31B3,0x51},
	{0x31B4,0x02},
	{0x31B5,0xEA},
	{0x31B6,0x09},
	{0x31B7,0x42},
	{0x31B8,0x00},
	{0x31B9,0x03},
	{0x31BA,0x3F},
	{0x31BB,0xFF},
	{0x3300,0xFF},
	{0x3301,0x35},
	{0x3303,0x40},
	{0x3304,0x00},
	{0x3305,0x00},
	{0x3306,0x30},
	{0x3307,0x00},
	{0x3308,0x87},
	{0x330A,0x60},
	{0x330B,0x56},
	{0x330D,0x79},
	{0x330E,0xFF},
	{0x330F,0xFF},
	{0x3310,0xFF},
	{0x3311,0x7F},
	{0x3312,0x0F},
	{0x3313,0x0F},
	{0x3314,0x02},
	{0x3315,0xC0},
	{0x3316,0x18},
	{0x3317,0x08},
	{0x3318,0x60},
	{0x3319,0x90},
	{0x331B,0x00},
	{0x331C,0x00},
	{0x331D,0x00},
	{0x331E,0x00},
	{0x3322,0x23},
	{0x3323,0x23},
	{0x3324,0x05},
	{0x3325,0x50},
	{0x3327,0x00},
	{0x3328,0x00},
	{0x3329,0x80},
	{0x332A,0x80},
	{0x332B,0x80},
	{0x332C,0x80},
	{0x332D,0x80},
	{0x332E,0x80},
	{0x332F,0x08},
	{0x3330,0x06},
	{0x3331,0x10},
	{0x3332,0x00},
	{0x3333,0x09},
	{0x3334,0x10},
	{0x3335,0x00},
	{0x3336,0x08},
	{0x3337,0x0d},
	{0x3338,0x03},
	{0x3339,0x03},
	{0x333A,0x02},
	{0x333B,0x74},
	{0x333C,0xa0},
	{0x333D,0x53},
	{0x333E,0x6c},
	{0x333F,0x97},
	{0x3340,0x52},
	{0x3341,0x3c},
	{0x3342,0x5c},
	{0x3343,0x2c},
	{0x3344,0x42},
	{0x3345,0x50},
	{0x3346,0x28},
	{0x3347,0x00},
	{0x3348,0x00},
	{0x3349,0x00},
	{0x334A,0x00},
	{0x334B,0x00},
	{0x334C,0x00},
	{0x334D,0x40},
	{0x334E,0x00},
	{0x334F,0xA0},
	{0x3350,0x03},
	{0x335F,0x00},
	{0x3360,0x00},
	{0x3400,0xA4},
	{0x3401,0x7F},
	{0x3402,0x00},
	{0x3403,0x00},
	{0x3404,0x3A},
	{0x3405,0xE3},
	{0x3406,0x22},
	{0x3407,0x25},
	{0x3408,0x17},
	{0x3409,0x5C},
	{0x340A,0x20},
	{0x340B,0x20},
	{0x340C,0x3B},
	{0x340D,0x2E},
	{0x340E,0x26},
	{0x340F,0x3F},
	{0x3410,0x34},
	{0x3411,0x2D},
	{0x3412,0x28},
	{0x3413,0x47},
	{0x3414,0x3E},
	{0x3415,0x6A},
	{0x3416,0x5A},
	{0x3417,0x50},
	{0x3418,0x48},
	{0x3419,0x42},
	{0x341B,0x10},
	{0x341C,0x40},
	{0x341D,0x70},
	{0x341E,0xc4},
	{0x341F,0x88},
	{0x3420,0x80},
	{0x3421,0xc4},
	{0x3422,0x00},
	{0x3423,0x0F},
	{0x3424,0x0F},
	{0x3425,0x0F},
	{0x3426,0x0F},
	{0x342B,0x10},
	{0x342C,0x20},
	{0x342D,0x80},
	{0x342E,0x50},
	{0x342F,0x60},
	{0x3430,0x30},
	{0x3431,0x1E},
	{0x3432,0x1E},
	{0x3433,0x0A},
	{0x3434,0x0A},
	{0x3435,0x15},
	{0x3436,0x15},
	{0x343F,0x10},
	{0x3440,0xF0},
	{0x3441,0x86},
	{0x3442,0xb0},
	{0x3443,0x60},
	{0x3444,0x08},
	{0x3446,0x03},
	{0x3447,0x00},
	{0x3448,0x00},
	{0x3449,0x00},
	{0x344A,0x00},
	{0x344B,0x00},
	{0x344C,0x20},
	{0x344D,0xFF},
	{0x344E,0x0F},
	{0x344F,0x20},
	{0x3450,0x80},
	{0x3451,0x0F},
	{0x3452,0x55},
	{0x3453,0x49},
	{0x3454,0x6A},
	{0x3455,0x93},
	{0x345C,0x00},
	{0x345D,0x00},
	{0x345E,0x00},
	{0x3500,0xC1},
	{0x3501,0x01},
	{0x3502,0x40},
	{0x3503,0x1A},
	{0x3504,0x00},
	{0x3505,0x9C},
	{0x3506,0x04},
	{0x3507,0xD0},
	{0x3508,0x00},
	{0x3509,0xBD},
	{0x350A,0x00},
	{0x350B,0x20},
	{0x350C,0x00},
	{0x350D,0x15},
	{0x350E,0x15},
	{0x350F,0x51},
	{0x3510,0x50},
	{0x3511,0x90},
	{0x3512,0x10},
	{0x3513,0x00},
	{0x3514,0x00},
	{0x3515,0x10},
	{0x3516,0x10},
	{0x3517,0x00},
	{0x3518,0x00},
	{0x3519,0xFF},
	{0x351A,0xC0},
	{0x351B,0x08},
	{0x351C,0x69},
	{0x351D,0xba},
	{0x351E,0x16},
	{0x351F,0x80},
	{0x3520,0x26},
	{0x3521,0x02},
	{0x3522,0x08},
	{0x3523,0x0C},
	{0x3524,0x01},
	{0x3525,0x5A},
	{0x3526,0x3C},
	{0x3527,0xE0},
	{0x3528,0xf5},
	{0x3529,0x72},
	{0x352A,0xDE},
	{0x352B,0x22},
	{0x352C,0xD0},
	{0x352D,0x1e},
	{0x352E,0x28},
	{0x352F,0xD2},
	{0x3530,0x2f},
	{0x3531,0x40},
	{0x3532,0x3a},
	{0x3533,0x38},
	{0x3534,0x28},
	{0x3535,0xDc},
	{0x3536,0x2f},
	{0x3537,0x40},
	{0x3538,0x3a},
	{0x3539,0x2e},
	{0x353A,0x3C},
	{0x353B,0x35},
	{0x353C,0xDE},
	{0x353D,0x33},
	{0x353E,0xDC},
	{0x353F,0x38},
	{0x3540,0x55},
	{0x3541,0x38},
	{0x3542,0x60},
	{0x3543,0x3F},
	{0x3544,0xBD},
	{0x3545,0x46},
	{0x3546,0xF0},
	{0x3547,0x10},
	{0x3548,0x84},
	{0x3549,0x00},
	{0x354A,0x00},
	{0x354B,0x00},
	{0x354C,0x00},
	{0x354D,0x00},
	{0x354E,0x00},
	{0x354F,0x00},
	{0x3550,0x00},
	{0x3551,0x03},
	{0x3552,0x28},
	{0x3553,0x20},
	{0x3554,0x60},
	{0x3555,0xF0},
	{0x355D,0x80},
	{0x355E,0x27},
	{0x355F,0x0F},
	{0x3560,0x90},
	{0x3561,0x01},
	{0x3562,0x00},
	{0x3563,0x00},
	{0x3564,0x00},
	{0x3565,0x08},
	{0x3566,0x50},
	{0x3567,0x5F},
	{0x3568,0x00},
	{0x3569,0x00},
	{0x356A,0x00},
	{0x356B,0x00},
	{0x356C,0x16},
	{0x356D,0xEE},
	{0x356E,0x01},
	{0x356F,0xAA},
	{0x3570,0x01},
	{0x3571,0x00},
	{0x3572,0x01},
	{0x3573,0x41},
	{0x3574,0x01},
	{0x3575,0x00},
	{0x3576,0x02},
	{0x3577,0x66},
	{0x3578,0xBA},
	{0x3579,0x0C},
	{0x357A,0x88},
	{0x357B,0xFF},
	{0x357C,0xE0},
	{0x357D,0x00},
	{0x3900,0x00},
	{0x3901,0x07},
	{0x3902,0x00},
	{0x3903,0x00},
	{0x3904,0x00},
	{0x3905,0x00},
	{0x3906,0x00},
	{0x3907,0x00},
	{0x3908,0x00},
	{0x3909,0x00},
	{0x390A,0x00},
	{0x390B,0x00},
	{0x390C,0x00},
	{0x30F0,0x00},
   #ifdef FULLSIZE_OUTPUT
	{0x3047,0x80},//PLL_MULTI[7:0]
	{0x3048,0x04},//FLLONGON/FRMSPD[1:0]/FL600S[12:8]
	{0x304a,0x04},//FL600S[7:0]
	{0x351C,0x69},//FLLONGON/FRMSPD[1:0]/FL600S[12:8]
	{0x351D,0xba},//FL600S[7:0]
	{0x351B,0x08},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]
   #endif

	{0x3012,0x02},//-/-/-/-/-/-/VLAT_ON/GROUP_HOLD
	{0x3010,0x01},//-/-/-/-/-/-/-/MODSEL ;
};

#ifndef FULLSIZE_OUTPUT
static struct msm_camera_i2c_reg_conf t4k28_preview_settings[] =
{
     //yuxin copy from P671B50 snapshot params 2013.07.10
    //SVGA 800X600
	{0x3012,0x03},//-/-/-/-/-/-/VLAT_ON/GROUP_HOLD
	{0x3015,0x04},//-/-/-/H_COUNT[12:8]   //2012.12.26
	{0x3016,0x04},//H_COUNT[7:0] //2012.12.26
	{0x3017,0x01},//-/-/-/V_COUNT[12:8]
	{0x3018,0x80},//V_COUNT[7:0]
	{0x3019,0x00},//-/-/-/-/-/-/-/SCALE_M[8]
	{0x301A,0x20},//SCALE_M[7:0]
	{0x301B,0x10},//-/-/-/V_ANABIN/-/-/-/-
	{0x301C,0x01},//-/-/-/-/-/-/-/SCALING_MODE
	{0x3020,0x03},//-/-/-/-/-/HOUTPIX[10:8]
	{0x3021,0x20},//HOUTPIX[7:0]
	{0x3022,0x02},//-/-/-/-/-/VOUTPIX[10:8]
	{0x3023,0x58},//VOUTPIX[7:0]
	{0x334E,0x01},//-/-/-/-/LSVCNT_MPY[11:8]
	{0x334F,0x40},//LSVCNT_MPY[7:0]
	{0x3012,0x02},//-/-/-/-/-/-/VLAT_ON/GROUP_HOLD
};
#endif

static struct msm_camera_i2c_reg_conf t4k28_snapshot_settings[] =
{
     //yuxin copy from P671B50 snapshot params 2013.07.09
	//FULL SIZE 1600x1200
	{0x3012,0x03},//-/-/-/-/-/-/VLAT_ON/GROUP_HOLD
	{0x3015,0x04},//-/-/-/H_COUNT[12:8]
	{0x3016,0x04},//H_COUNT[7:0]
	{0x3017,0x03},//-/-/-/V_COUNT[12:8]
	{0x3018,0x00},//V_COUNT[7:0]
	{0x3019,0x00},//-/-/-/-/-/-/-/SCALE_M[8]
	{0x301A,0x10},//SCALE_M[7:0]
	{0x301B,0x00},//-/-/-/V_ANABIN/-/-/-/-
	{0x301C,0x01},//-/-/-/-/-/-/-/SCALING_MODE
	{0x3020,0x06},//-/-/-/-/-/HOUTPIX[10:8]
	{0x3021,0x40},//HOUTPIX[7:0]
	{0x3022,0x04},//-/-/-/-/-/VOUTPIX[10:8]
	{0x3023,0xB0},//VOUTPIX[7:0]
	{0x334E,0x00},//-/-/-/-/LSVCNT_MPY[11:8]
	{0x334F,0xA0},//LSVCNT_MPY[7:0]
	{0x3012,0x02},//-/-/-/-/-/-/VLAT_ON/GROUP_HOLD
};



static struct v4l2_subdev_info t4k28_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};


static struct msm_camera_i2c_conf_array t4k28_init_conf[] = {
	{&t4k28_recommend_settings[0],
	ARRAY_SIZE(t4k28_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}

};

static struct msm_camera_i2c_conf_array t4k28_confs[] = {
#ifdef FULLSIZE_OUTPUT
	{&t4k28_snapshot_settings[0],
	ARRAY_SIZE(t4k28_snapshot_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
#else
	{&t4k28_snapshot_settings[0],
	ARRAY_SIZE(t4k28_snapshot_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&t4k28_preview_settings[0],
	ARRAY_SIZE(t4k28_preview_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
#endif
};


static struct msm_sensor_output_info_t t4k28_dimensions[] =
{
    
    { /* For SNAPSHOT */
		.x_output = 1600,
		.y_output = 1200,
		.line_length_pclk = 2056,
		.frame_length_lines = 1536,
		.vt_pixel_clk = 78000000,
		.op_pixel_clk = 15800000,
		.binning_factor = 0x0,
     },
  #ifndef FULLSIZE_OUTPUT
	{ /* For PREVIEW */
		.x_output = 800,//640
		.y_output = 600,//480
		.line_length_pclk = 2056,//640
		.frame_length_lines = 768,//480
		.vt_pixel_clk = 78000000,
		.op_pixel_clk = 15800000,
		.binning_factor = 0x0,
	},
  #endif
};


#ifndef FULLSIZE_OUTPUT
void T4K28YUV_write_shutter_and_gain(struct msm_sensor_ctrl_t *s_ctrl,
                                         uint16_t shutter, uint16_t again, uint16_t dgain)
{

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		                             0x3506, ((shutter >> 8) & 0xFF),MSM_CAMERA_I2C_BYTE_DATA);//MES[15:8]
       msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		                             0x3507, (shutter & 0xFF),MSM_CAMERA_I2C_BYTE_DATA);//MES[7:0]
       msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		                             0x350a, ((again >> 8) & 0xFF),MSM_CAMERA_I2C_BYTE_DATA);//ESLIMMODE/ROOMDET/-/-/MAG[11:8]
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		                             0x350b, (again & 0xFF),MSM_CAMERA_I2C_BYTE_DATA);//MAG[7:0]
       msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		                             0x350c, (dgain >> 2),MSM_CAMERA_I2C_BYTE_DATA);//MDG[7:0]
	return;
}   /* write_T4K28_shutter */

/*************************************************************************
* FUNCTION
*    T4K28YUV_set_AE_status
*
* DESCRIPTION
*    AE enable, manual AE or lock AE
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/

static void T4K28YUV_set_AE_status(struct msm_sensor_ctrl_t *s_ctrl,
                            uint16_t AE_status)
{
    uint16_t temp_AE_reg = 0;

    if(AE_status == AE_enable) {
        //turn on AEC/AGC
       msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3500, &temp_AE_reg,MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x3500, temp_AE_reg | 0x80,MSM_CAMERA_I2C_BYTE_DATA);
    } else if(AE_status == AE_lock) {
        //Lock AEC/AGC
       msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3500, &temp_AE_reg,MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x3500, temp_AE_reg | 0x20,MSM_CAMERA_I2C_BYTE_DATA);
    } else {
	//turn off AEC/AGC
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3500, &temp_AE_reg,MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x3500, temp_AE_reg & ~0xa0,MSM_CAMERA_I2C_BYTE_DATA);
    }
}


/*************************************************************************
* FUNCTION
*    T4K28YUV_set_AWB_mode
*
* DESCRIPTION
*    awb enable or manual awb
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
/*
static void T4K28YUV_set_AWB_mode(struct msm_sensor_ctrl_t *s_ctrl,
                                                           uint16_t AWB_enable)
{
	uint16_t temp_AWB_reg = 0;

    if(AWB_enable == TRUE) {
		//enable Auto WB
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3500, &temp_AWB_reg,MSM_CAMERA_I2C_BYTE_DATA);
	       msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x3500, (temp_AWB_reg | 0x40),MSM_CAMERA_I2C_BYTE_DATA);
    } else {
		//turn off AWB
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3500, &temp_AWB_reg,MSM_CAMERA_I2C_BYTE_DATA);
	       msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x3500, (temp_AWB_reg & ~0x40),MSM_CAMERA_I2C_BYTE_DATA);
    }
}
*/

#endif

static int is_first_preview = 0;
static unsigned int csi_config = 0;

int32_t t4k28_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;
	/*if (update_type != MSM_SENSOR_REG_INIT)
	{
		if (csi_config == 0 || res == 0)
		msleep(66);
		else
		msleep(266);
	}*/
	s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
       msleep(50);
	if (update_type == MSM_SENSOR_REG_INIT) {
		CDBG("Register INIT\n");
		s_ctrl->curr_csi_params = NULL;
		msm_sensor_enable_debugfs(s_ctrl);
		csi_config = 0;
		is_first_preview = 1;
		msm_sensor_write_init_settings(s_ctrl);
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		CDBG("PERIODIC : %d\n", res);
		if (!csi_config) {
		msleep(30);
		s_ctrl->curr_csic_params = s_ctrl->csic_params[res];
		CDBG("CSI config in progress\n");
			v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
				NOTIFY_CSIC_CFG,s_ctrl->curr_csic_params);
		CDBG("CSI config is done\n");
		mb();
		msleep(30);
		csi_config = 1;
	}

		if(is_first_preview)
		{
			msleep(50);
		}
		else
		{
			msleep(10);
			is_first_preview = 0;
		}
#ifndef FULLSIZE_OUTPUT

    switch(res)
	{
	   //snapshot
	   case 0:
	   	{
			//uint16_t exp_h,exp_l,Again_h,Again_l,dg_h,dg_l;
                   // uint16_t dg;
				uint16_t MESH = 0;
				uint16_t MESL = 0;
				uint16_t MES = 0;
				uint16_t MAGH = 0;
				uint16_t MAGL = 0;
				uint16_t MAG = 0;
				uint16_t MDG = 0;
				uint16_t MDGH = 0;
				uint16_t MDGL = 0;
				uint16_t AG_1X = 0x1a;
			//lock AEC
                 T4K28YUV_set_AE_status(s_ctrl,AE_lock);
			//FPS-Stable
			//msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			//                    0x351b, 0x08, MSM_CAMERA_I2C_BYTE_DATA);
			
		      msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x355f, &MESH,MSM_CAMERA_I2C_BYTE_DATA);
		      msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3560, &MESL,MSM_CAMERA_I2C_BYTE_DATA);
		      msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3561, &MAGH,MSM_CAMERA_I2C_BYTE_DATA);
		      msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3562, &MAGL,MSM_CAMERA_I2C_BYTE_DATA);
		      msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3563, &MDGH,MSM_CAMERA_I2C_BYTE_DATA);
		      msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3564, &MDGL,MSM_CAMERA_I2C_BYTE_DATA);
			msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			                    0x3503, &AG_1X,MSM_CAMERA_I2C_BYTE_DATA);
			//calculate and record the preview shutter gain
			MES = (MESH << 8) | (MESL);
			MAG = (MAGH << 8) | (MAGL);
			MDG = ((MDGH & 0x03) << 8) | (MDGL & 0xFF);
			set_pv_back_es = MES * MAG * 0x100/AG_1X;
			set_pv_back_es = set_pv_back_es /0x100;
			set_pv_back_ag = AG_1X;
			set_pv_back_dg = MDG;
			//down capture window setting
			msm_sensor_write_conf_array(
			s_ctrl->sensor_i2c_client,
			s_ctrl->msm_sensor_reg->mode_settings, res);

		    //turn off AEC
                  T4K28YUV_set_AE_status(s_ctrl,AE_disable);
				//set shutter and gain
		     msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x3506, MESH,MSM_CAMERA_I2C_BYTE_DATA);
		     msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x3507, MESL,MSM_CAMERA_I2C_BYTE_DATA);
		     msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x350a, MAGH,MSM_CAMERA_I2C_BYTE_DATA);
		     msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x350b, MAGL,MSM_CAMERA_I2C_BYTE_DATA);
		     msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x350c, (MDG>>2),MSM_CAMERA_I2C_BYTE_DATA);
			csi_config = 1;

			break;
		}

			

	   //preview
	   case 1:
	   	{
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x3012, 0x03, MSM_CAMERA_I2C_BYTE_DATA);
			msm_sensor_write_conf_array(
			s_ctrl->sensor_i2c_client,
			s_ctrl->msm_sensor_reg->mode_settings, res);
			//set back preview shutter gain
			//T4K28YUV_set_AE_status(s_ctrl, AE_disable);
			T4K28YUV_write_shutter_and_gain(s_ctrl,set_pv_back_es, set_pv_back_ag, set_pv_back_ag);
			
			//fps-auto
			//msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			//                    0x351b, 0x98, MSM_CAMERA_I2C_BYTE_DATA);
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			                    0x3012, 0x02,MSM_CAMERA_I2C_BYTE_DATA);
			msleep(100);
			//turn on AEC
			T4K28YUV_set_AE_status(s_ctrl,AE_enable);
			 
			csi_config = 1;
			break;
		     }
			
			
	}
#endif
	 s_ctrl->func_tbl->sensor_start_stream(s_ctrl);
        msleep(50);
	/*rc = msm_sensor_write_conf_array(
			s_ctrl->sensor_i2c_client,
			s_ctrl->msm_sensor_reg->mode_settings, res);*/

	}
	return rc;
}




// saturation
static struct msm_camera_i2c_reg_conf t4k28_saturation[][2] =
{
   	{//level 0
		{0x341E,0x80},
		{0x3421,0x80},
	},

	{//level 1
		//{0x3422,0xa0},
             {0x341E,0xa0},
		{0x3421,0xa0},
	},

	{//level 2 -default level
		{0x341E,0xc4},
		{0x3421,0xc4},
	},

	{//level 3
		{0x341E,0xe0},
		{0x3421,0xe0},
	},

	{//level 4
		{0x341E,0xff},
		{0x3421,0xff},
	},
};

static struct msm_camera_i2c_conf_array t4k28_saturation_confs[][1] =
{
   {{t4k28_saturation[0],		ARRAY_SIZE(t4k28_saturation[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_saturation[1],		ARRAY_SIZE(t4k28_saturation[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_saturation[2],		ARRAY_SIZE(t4k28_saturation[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_saturation[3],		ARRAY_SIZE(t4k28_saturation[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_saturation[4],		ARRAY_SIZE(t4k28_saturation[4]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},

};

static int t4k28_saturation_enum_map[] =
{
   MSM_V4L2_SATURATION_L0,
   MSM_V4L2_SATURATION_L1,
   MSM_V4L2_SATURATION_L2,
   MSM_V4L2_SATURATION_L3,
   MSM_V4L2_SATURATION_L4,

};

static struct msm_camera_i2c_enum_conf_array t4k28_saturation_enum_confs = {
	.conf = &t4k28_saturation_confs[0][0],
       .conf_enum = t4k28_saturation_enum_map,
       .num_enum = ARRAY_SIZE(t4k28_saturation_enum_map),
       .num_index = ARRAY_SIZE(t4k28_saturation_confs),
       .num_conf = ARRAY_SIZE(t4k28_saturation_confs[0]),
       .data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

// contrast
static struct msm_camera_i2c_reg_conf t4k28_contrast[][1] =
{
   	{
		//Contrast -2
		{0x3441,0x49},

	},

	{
		//Contrast -1
		{0x3441,0x59},

	},

	{
		//Contrast (Default)
		{0x3441,0x86},
	},

	{
		//Contrast +1
		{0x3441,0x99},
	},

	{
		//Contrast +2
		{0x3441,0xa9},
	},
};

static struct msm_camera_i2c_conf_array t4k28_contrast_confs[][1] =
{
   {{t4k28_contrast[0],		ARRAY_SIZE(t4k28_contrast[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_contrast[1],		ARRAY_SIZE(t4k28_contrast[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_contrast[2],		ARRAY_SIZE(t4k28_contrast[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_contrast[3],		ARRAY_SIZE(t4k28_contrast[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_contrast[4],		ARRAY_SIZE(t4k28_contrast[4]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},

};

static int t4k28_contrast_enum_map[] =
{
   MSM_V4L2_CONTRAST_L0,
   MSM_V4L2_CONTRAST_L1,
   MSM_V4L2_CONTRAST_L2,
   MSM_V4L2_CONTRAST_L3,
   MSM_V4L2_CONTRAST_L4,

};

static struct msm_camera_i2c_enum_conf_array t4k28_contrast_enum_confs = {
	.conf = &t4k28_contrast_confs[0][0],
       .conf_enum = t4k28_contrast_enum_map,
       .num_enum = ARRAY_SIZE(t4k28_contrast_enum_map),
       .num_index = ARRAY_SIZE(t4k28_contrast_confs),
       .num_conf = ARRAY_SIZE(t4k28_contrast_confs[0]),
       .data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

// sharpness
static struct msm_camera_i2c_reg_conf t4k28_sharpness[][5] =
{
   	{
		//Sharpness 0
		{0x342B,0x20},
		{0x342D,0x60},
		{0x342e,0x30},
		{0x342f,0x40},
		{0x3430,0x10},
	},

	{
		//Sharpness 1
		{0x342B,0x18},
		{0x342D,0x70},
		{0x342e,0x40},
		{0x342f,0x50},
		{0x3430,0x20},
	},

	{
		//Sharpness_Auto (Default)
		{0x342B,0x10},
		{0x342D,0x80},
		{0x342e,0x50},
		{0x342f,0x60},
		{0x3430,0x30},
	},

	{
		//Sharpness 3
		{0x342B,0x08},
		{0x342D,0x90},
		{0x342e,0x60},
		{0x342f,0x70},
		{0x3430,0x40},
	},

	{
		//Sharpness 4
		{0x342B,0x00},
		{0x342D,0xa0},
		{0x342e,0x70},
		{0x342f,0x80},
		{0x3430,0x50},
	},
};

static struct msm_camera_i2c_conf_array t4k28_sharpness_confs[][1] =
{
   {{t4k28_sharpness[0],		ARRAY_SIZE(t4k28_sharpness[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_sharpness[1],		ARRAY_SIZE(t4k28_sharpness[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_sharpness[2],		ARRAY_SIZE(t4k28_sharpness[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_sharpness[3],		ARRAY_SIZE(t4k28_sharpness[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_sharpness[4],		ARRAY_SIZE(t4k28_sharpness[4]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},

};

static int t4k28_sharpness_enum_map[] =
{
   MSM_V4L2_SHARPNESS_L0,
   MSM_V4L2_SHARPNESS_L1,
   MSM_V4L2_SHARPNESS_L2,
   MSM_V4L2_SHARPNESS_L3,
   MSM_V4L2_SHARPNESS_L4,
};

static struct msm_camera_i2c_enum_conf_array t4k28_sharpness_enum_confs = {
	.conf = &t4k28_sharpness_confs[0][0],
       .conf_enum = t4k28_sharpness_enum_map,
       .num_enum = ARRAY_SIZE(t4k28_sharpness_enum_map),
       .num_index = ARRAY_SIZE(t4k28_sharpness_confs),
       .num_conf = ARRAY_SIZE(t4k28_sharpness_confs[0]),
       .data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

// brightness
static struct msm_camera_i2c_reg_conf t4k28_brightness[][2] =
{
   	{
		//Brightness -2
		{0x343F,0xc0},//-/-/-/ bright agmax
		{0x3440,0xc0},//-/-/-/ bright agmin
	},

	{
		//Brightness -1
		{0x343F,0xe0},//-/-/-/ bright agmax
		{0x3440,0xe0},//-/-/-/ bright agmin
	},

	{
		//Brightness 0 (Default)
		{0x343F,0x10},//-/-/-/ bright agmax
		{0x3440,0xf0},//-/-/-/ bright agmin
	},

	{
		//Brightness +1
		{0x343F,0x20},//-/-/-/ bright agmax
		{0x3440,0x20},//-/-/-/ bright agmin
	},

	{
		//Brightness +2
		{0x343F,0x40},//-/-/-/ bright agmax
		{0x3440,0x40},//-/-/-/ bright agmin
	},
};

static struct msm_camera_i2c_conf_array t4k28_brightness_confs[][1] =
{
   {{t4k28_brightness[0],		ARRAY_SIZE(t4k28_brightness[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_brightness[1],		ARRAY_SIZE(t4k28_brightness[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_brightness[2],		ARRAY_SIZE(t4k28_brightness[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_brightness[3],		ARRAY_SIZE(t4k28_brightness[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_brightness[4],		ARRAY_SIZE(t4k28_brightness[4]), 0, MSM_CAMERA_I2C_BYTE_DATA},	},

};

static int t4k28_brightness_enum_map[] =
{
   MSM_V4L2_BRIGHTNESS_L0,
   MSM_V4L2_BRIGHTNESS_L1,
   MSM_V4L2_BRIGHTNESS_L2,
   MSM_V4L2_BRIGHTNESS_L3,
   MSM_V4L2_BRIGHTNESS_L4,
};

static struct msm_camera_i2c_enum_conf_array t4k28_brightness_enum_confs = {
	.conf = &t4k28_brightness_confs[0][0],
       .conf_enum = t4k28_brightness_enum_map,
       .num_enum = ARRAY_SIZE(t4k28_brightness_enum_map),
       .num_index = ARRAY_SIZE(t4k28_brightness_confs),
       .num_conf = ARRAY_SIZE(t4k28_brightness_confs[0]),
       .data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

// Exposure Compensation
static struct msm_camera_i2c_reg_conf t4k28_exposure_compensation[][2] =
{
   	{
		//-1.7EV
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x00},//ALCAIM[7:0]
	},

	{
		//-1.0EV
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x20},//ALCAIM[7:0]
	},

	{
		//default
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x40},//ALCAIM[7:0]
	},

	{
		//+1.0EV
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x60},//ALCAIM[7:0]
	},

	{
		//+1.7EV
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x80},//ALCAIM[7:0]
	},
};

static struct msm_camera_i2c_conf_array t4k28_exposure_compensation_confs[][1] =
{
   {{t4k28_exposure_compensation[0],ARRAY_SIZE(t4k28_exposure_compensation[0]),
								0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_exposure_compensation[1],ARRAY_SIZE(t4k28_exposure_compensation[1]),
   								0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_exposure_compensation[2],ARRAY_SIZE(t4k28_exposure_compensation[2]),
   								0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_exposure_compensation[3],ARRAY_SIZE(t4k28_exposure_compensation[3]),
   								0, MSM_CAMERA_I2C_BYTE_DATA},	},
   {{t4k28_exposure_compensation[4],ARRAY_SIZE(t4k28_exposure_compensation[4]),
   								0, MSM_CAMERA_I2C_BYTE_DATA},	},

};

static int t4k28_exposure_compensation_enum_map[] =
{
	MSM_V4L2_EXPOSURE_N2,
	MSM_V4L2_EXPOSURE_N1,
	MSM_V4L2_EXPOSURE_D,
	MSM_V4L2_EXPOSURE_P1,
	MSM_V4L2_EXPOSURE_P2,
};

static struct msm_camera_i2c_enum_conf_array t4k28_exposure_compensation_enum_confs = {
	.conf = &t4k28_exposure_compensation_confs[0][0],
       .conf_enum = t4k28_exposure_compensation_enum_map,
       .num_enum = ARRAY_SIZE(t4k28_exposure_compensation_enum_map),
       .num_index = ARRAY_SIZE(t4k28_exposure_compensation_confs),
       .num_conf = ARRAY_SIZE(t4k28_exposure_compensation_confs[0]),
       .data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

// special  effect
static struct msm_camera_i2c_reg_conf t4k28_effect[][3] = {
    {
		//effect_off
		{0x3402,0x00},
		{0x3402,0x00},
		{0x3402,0x00},
    },

    {
		//effect_mono
		{0x3402,0x06},
		{0x3402,0x06},
		{0x3402,0x06},
    },

    {
		//effect_negative
		{0x3402,0x01},
		{0x3402,0x01},//0x01
		{0x3402,0x01},//0x01
    },

    {
		//effect_solative,has no params,can't use
		{0x3402,0x00},
		{0x3402,0x00},
		{0x3402,0x00},
    },

    {
		//sepia
		{0x3402,0x05},
		{0x3454,0x6A},//SEPIAOFSU[7:0]
		{0x3455,0x93},//SEPIAOFSV[7:0]
	},

    {
		//redish
		{0x3402,0x05},//sepia red
		{0x3454,0x6d},//SEPIAOFSU[7:0]
		{0x3455,0xc0},//SEPIAOFSV[7:0]
	},

   {
		//blueish
		{0x3402,0x05},//sepia blue
		{0x3454,0xa8},//SEPIAOFSU[7:0]
		{0x3455,0x77},//SEPIAOFSV[7:0]
	},

    {
		//greenish
		{0x3402, 0x04},
		{0x3402, 0x04},
		{0x3402, 0x04},
    },



};

static struct msm_camera_i2c_conf_array t4k28_effect_confs[][1] = {
	{
		{t4k28_effect[0],
		ARRAY_SIZE(t4k28_effect[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_effect[1],
		ARRAY_SIZE(t4k28_effect[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_effect[2],
		ARRAY_SIZE(t4k28_effect[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_effect[3],
		ARRAY_SIZE(t4k28_effect[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_effect[4],
		ARRAY_SIZE(t4k28_effect[4]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_effect[5],
		ARRAY_SIZE(t4k28_effect[5]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_effect[6],
		ARRAY_SIZE(t4k28_effect[6]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_effect[7],
		ARRAY_SIZE(t4k28_effect[7]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

};

static int t4k28_effect_enum_map[] = {
	MSM_V4L2_EFFECT_OFF,
	MSM_V4L2_EFFECT_MONO,
	MSM_V4L2_EFFECT_NEGATIVE,
	MSM_V4L2_EFFECT_SOLARIZE,
	MSM_V4L2_EFFECT_SEPIA,
	MSM_V4L2_EFFECT_REDISH,
	MSM_V4L2_EFFECT_BLUEISH,
	MSM_V4L2_EFFECT_GREENISH,
};

static struct msm_camera_i2c_enum_conf_array t4k28_effect_enum_confs = {
	.conf = &t4k28_effect_confs[0][0],
	.conf_enum = t4k28_effect_enum_map,
	.num_enum = ARRAY_SIZE(t4k28_effect_enum_map),
	.num_index = ARRAY_SIZE(t4k28_effect_confs),
	.num_conf = ARRAY_SIZE(t4k28_effect_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};
//

// awb
static struct msm_camera_i2c_reg_conf t4k28_awb[][5] = {
	{ //MSM_V4L2_WB_MIN_MINUS_1,not used
		//CAMERA_WB_AUTO                //1
		{0x3500,0xC1},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x23},//PWBGAINGR[7:0]
		{0x3323,0x23},//PWBGAINGB[7:0]
		{0x3324,0x05},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
	},

	{ // wb_auto
		//CAMERA_WB_AUTO                //1
		{0x3500,0xC1},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x23},//PWBGAINGR[7:0]
		{0x3323,0x23},//PWBGAINGB[7:0]
		{0x3324,0x05},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
	},
      {//MSM_V4L2_WB_CUSTOM, not used
		//CAMERA_WB_AUTO  
		{0x3500,0xC1},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x23},//PWBGAINGR[7:0]
		{0x3323,0x23},//PWBGAINGB[7:0]
		{0x3324,0x05},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
	},

	{//INCANDESCENT,  //白炽
		{0x3500,0x81},//ALCSW/AWBSW/ALCLOCK/-
		{0x3322,0x50},//PWBGAINGR[7:0]
		{0x3323,0x50},//PWBGAINGB[7:0]
		{0x3324,0x00},//PWBGAINR[7:0]
		{0x3325,0xf0},//PWBGAINB[7:0]
	},

       {//FLUORESCENT,    //荧光
       	{0x3500,0x81},//ALCSW/AWBSW/ALCLOCK/-
		{0x3322,0x50},//PWBGAINGR[7:0]
		{0x3323,0x50},//PWBGAINGB[7:0]
		{0x3324,0x10},//PWBGAINR[7:0]
		{0x3325,0xc0},//PWBGAINB[7:0]
       },

       { //daylight
		{0x3500,0x81},//ALCSW/AWBSW/ALCLOCK/-
		{0x3322,0x20},//PWBGAINGR[7:0]
		{0x3323,0x20},//PWBGAINGB[7:0]
		{0x3324,0x10},//PWBGAINR[7:0]
		{0x3325,0x40},//PWBGAINB[7:0]
       },

     { //cloudy
		{0x3500,0x81},//ALCSW/AWBSW/ALCLOCK/-
		{0x3322,0x30},//PWBGAINGR[7:0]
		{0x3323,0x30},//PWBGAINGB[7:0]
		{0x3324,0x30},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
       },
};

static struct msm_camera_i2c_conf_array t4k28_awb_confs[][1] = {
	{
		{t4k28_awb[0],
		ARRAY_SIZE(t4k28_awb[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_awb[1],
		ARRAY_SIZE(t4k28_awb[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_awb[2],
		ARRAY_SIZE(t4k28_awb[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_awb[3],
		ARRAY_SIZE(t4k28_awb[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_awb[4],
	       ARRAY_SIZE(t4k28_awb[4]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_awb[5],
		ARRAY_SIZE(t4k28_awb[5]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_awb[6],
	       ARRAY_SIZE(t4k28_awb[6]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

};

static int t4k28_awb_enum_map[] = {
	MSM_V4L2_WB_OFF,//not used
	MSM_V4L2_WB_AUTO ,//= 1
	MSM_V4L2_WB_CUSTOM,  //not used
	MSM_V4L2_WB_INCANDESCENT, //白炽
	MSM_V4L2_WB_FLUORESCENT,   //荧光
	MSM_V4L2_WB_DAYLIGHT,
	MSM_V4L2_WB_CLOUDY_DAYLIGHT,
};

static struct msm_camera_i2c_enum_conf_array t4k28_awb_enum_confs = {
	.conf = &t4k28_awb_confs[0][0],
	.conf_enum = t4k28_awb_enum_map,
	.num_enum = ARRAY_SIZE(t4k28_awb_enum_map),
	.num_index = ARRAY_SIZE(t4k28_awb_confs),
	.num_conf = ARRAY_SIZE(t4k28_awb_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};


//anti-banding
static struct msm_camera_i2c_reg_conf t4k28_antibanding[][1] = {
       {//off.not used
            {0x351E, 0x16},
       },

       {//60Hz
            {0x351E, 0x56},
       },

	   {//50Hz
              {0x351E, 0x16},
	   },

       { //auto
		   {0x351E,0x86},//ACFDET/AC60M/FLMANU/ACDETDLY/MSKLINE[1:0]/ACDPWAIT[1:0]
	   },
};

static struct msm_camera_i2c_conf_array t4k28_antibanding_confs[][1] = {
	{
		{t4k28_antibanding[0],
		ARRAY_SIZE(t4k28_antibanding[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_antibanding[1],
		ARRAY_SIZE(t4k28_antibanding[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_antibanding[2],
		ARRAY_SIZE(t4k28_antibanding[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_antibanding[3],
		ARRAY_SIZE(t4k28_antibanding[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

};

static int t4k28_antibanding_enum_map[] = {
	MSM_V4L2_POWER_LINE_OFF,  //not used
	MSM_V4L2_POWER_LINE_60HZ,
	MSM_V4L2_POWER_LINE_50HZ,
	MSM_V4L2_POWER_LINE_AUTO,

};

static struct msm_camera_i2c_enum_conf_array t4k28_antibanding_enum_confs = {
	.conf = &t4k28_antibanding_confs[0][0],
	.conf_enum = t4k28_antibanding_enum_map,
	.num_enum = ARRAY_SIZE(t4k28_antibanding_enum_map),
	.num_index = ARRAY_SIZE(t4k28_antibanding_confs),
	.num_conf = ARRAY_SIZE(t4k28_antibanding_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};


//ISO
static struct msm_camera_i2c_reg_conf t4k28_iso[][2] = {

	{ //auto
		//{0x3503,0x1A},//AGMIN[7:0]
		{0x3504,0x00},//-/-/-/-/AGMAX[11:8]
		{0x3505,0x9C},//AGMAX[7:0]
	},

	{ //MSM_V4L2_ISO_DEBLUR ,not used
		/*不支持，参数设置为auto*/
		//{0x3503,0x1A},//AGMIN[7:0]
		{0x3504,0x00},//-/-/-/-/AGMAX[11:8]
		{0x3505,0x9C},//AGMAX[7:0]
	},

	{//iso_100
		//{0x3503,0x1A},//AGMIN[7:0]
		{0x3504,0x00},//-/-/-/-/AGMAX[11:8]
		{0x3505,0x1A},//AGMAX[7:0]
	},

	{//iso200
		//{0x3503,0x1A},//AGMIN[7:0]
		{0x3504,0x00},//-/-/-/-/AGMAX[11:8]
		{0x3505,0x34},//AGMAX[7:0]
	},

	{ //iso400
		//{0x3503,0x1A},//AGMIN[7:0]
		{0x3504,0x00},//-/-/-/-/AGMAX[11:8]
		{0x3505,0x68},//AGMAX[7:0]
	},

	{ //iso800
		//{0x3503,0x1A},//AGMIN[7:0]
		{0x3504,0x00},//-/-/-/-/AGMAX[11:8]
		{0x3505,0xD0},//AGMAX[7:0]
	},

	{ //iso1600
		/*不支持，参数设置为ISO800*/
		//{0x3503,0x1A},//AGMIN[7:0]
		{0x3504,0x00},//-/-/-/-/AGMAX[11:8]
		{0x3505,0xD0},//AGMAX[7:0]

       },
};

static struct msm_camera_i2c_conf_array t4k28_iso_confs[][1] = {
	{
		{t4k28_iso[0],
		ARRAY_SIZE(t4k28_iso[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_iso[1],
		ARRAY_SIZE(t4k28_iso[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_iso[2],
		ARRAY_SIZE(t4k28_iso[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_iso[3],
		ARRAY_SIZE(t4k28_iso[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_iso[4],
	  ARRAY_SIZE(t4k28_iso[4]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_iso[5],
	  ARRAY_SIZE(t4k28_iso[5]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

	{
		{t4k28_iso[6],
	  ARRAY_SIZE(t4k28_iso[6]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},

};

static int t4k28_iso_enum_map[] = {
	MSM_V4L2_ISO_AUTO,
	MSM_V4L2_ISO_DEBLUR,//not used
	MSM_V4L2_ISO_100,
	MSM_V4L2_ISO_200,
	MSM_V4L2_ISO_400,
	MSM_V4L2_ISO_800,
	MSM_V4L2_ISO_1600,
};

static struct msm_camera_i2c_enum_conf_array t4k28_iso_enum_confs = {
	.conf = &t4k28_iso_confs[0][0],
	.conf_enum = t4k28_iso_enum_map,
	.num_enum = ARRAY_SIZE(t4k28_iso_enum_map),
	.num_index = ARRAY_SIZE(t4k28_iso_confs),
	.num_conf = ARRAY_SIZE(t4k28_iso_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

//scene
static struct msm_camera_i2c_reg_conf t4k28_scene[][16]={
      {//SCENE_MODE_AUTO  自动
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x50},//ALCAIM[7:0]
		{0x351B,0x08},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]   
		{0x3441,0x86},//LCONT_LEV[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xc4}, //saturation setting
		{0x3421,0xc4},
		{0x3500,0xc1},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x23},//PWBGAINGR[7:0]
		{0x3323,0x23},//PWBGAINGB[7:0]
		{0x3324,0x05},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
		{0x350D,0x15},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x15},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x51},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x50},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
	},
	
      {   //SCENE_MODE_LANDSCAPE，风景
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x50},//ALCAIM[7:0]
		{0x351B,0x98},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]   
		{0x3441,0x8c},//LCONT_LEV[7:0]
		//{0x3422,0xc8},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xc8}, //saturation setting
		{0x3421,0xc8},
		{0x3500,0xc1},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x23},//PWBGAINGR[7:0]
		{0x3323,0x23},//PWBGAINGB[7:0]
		{0x3324,0x05},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
		{0x350D,0x55},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x55},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x55},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x54},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
      },
      
      {//SCENE_MODE_FIREWORKS 篝火
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x30},//ALCAIM[7:0]
		{0x351B,0x48},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]   
		{0x3441,0x80},//LCONT_LEV[7:0]
		//{0x3422,0xd0},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xd0}, //saturation setting
		{0x3421,0xd0},
		{0x3500,0x81},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x20},//PWBGAINGR[7:0]
		{0x3323,0x20},//PWBGAINGB[7:0]
		{0x3324,0x10},//PWBGAINR[7:0]
		{0x3325,0x40},//PWBGAINB[7:0]
		{0x350D,0x15},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x15},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x51},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x50},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
      	},
      	
       {//SCENE_MODE_BEACH   海滩
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x80},//ALCAIM[7:0]
		{0x351B,0x98},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]   
		{0x3441,0x80},//LCONT_LEV[7:0]
		//{0x3422,0xc0},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xc0}, //saturation setting
		{0x3421,0xc0},
		{0x3500,0x81},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x20},//PWBGAINGR[7:0]
		{0x3323,0x20},//PWBGAINGB[7:0]
		{0x3324,0x10},//PWBGAINR[7:0]
		{0x3325,0x40},//PWBGAINB[7:0] 
		{0x350D,0x55},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x55},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x55},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x54},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
       },

       {//SCENE_MODE_PARTY - Take indoor low-light shot.派对
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x50},//ALCAIM[7:0]
		{0x351B,0x98},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]   
		{0x3441,0x80},//LCONT_LEV[7:0]
		//{0x3422,0xb0},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xb0}, //saturation setting
		{0x3421,0xb0},
		{0x3500,0xc1},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x23},//PWBGAINGR[7:0]
		{0x3323,0x23},//PWBGAINGB[7:0]
		{0x3324,0x05},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
		{0x350D,0x15},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x15},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x51},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x50},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
       },

       {//SCENE_MODE_PORTRAIT - Take people pictures.人物
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x50},//ALCAIM[7:0]
		{0x351B,0x98},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]   
		{0x3441,0x89},//LCONT_LEV[7:0]
		//{0x3422,0xa0},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xa0}, //saturation setting
		{0x3421,0xa0},
		{0x3500,0xc1},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x23},//PWBGAINGR[7:0]
		{0x3323,0x23},//PWBGAINGB[7:0]
		{0x3324,0x05},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
		{0x350D,0x19},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x0A},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x81},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x90},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
       },
       
       {//SCENE_MODE_SUNSET 日落
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x50},//ALCAIM[7:0]
		{0x351B,0x98},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]
		{0x3441,0xa0},//LCONT_LEV[7:0]
		//{0x3422,0xe0},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xe0}, //saturation setting
		{0x3421,0xe0},
		{0x3500,0x81},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x20},//PWBGAINGR[7:0]
		{0x3323,0x20},//PWBGAINGB[7:0]
		{0x3324,0x10},//PWBGAINR[7:0]
		{0x3325,0x40},//PWBGAINB[7:0]
		{0x350D,0xFB},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0xF9},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0xBF},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0xBC},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
       },

      { //SCENE_MODE_SNOW - Take pictures on the snow. 下雪
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0xc0},//ALCAIM[7:0]
		{0x351B,0x98},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]   
		{0x3441,0x89},//LCONT_LEV[7:0]
		//{0x3422,0xc0},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xc0}, //saturation setting
		{0x3421,0xc0},
		{0x3500,0x81},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x30},//PWBGAINGR[7:0]
		{0x3323,0x30},//PWBGAINGB[7:0]
		{0x3324,0x30},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
		{0x350D,0x55},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x55},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x55},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x54},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
      },

      {//SCENE_MODE_NIGHT    夜景
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x50},//ALCAIM[7:0]
		{0x351B,0xA8},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]
		{0x3441,0x89},//LCONT_LEV[7:0]
		//{0x3422,0xc0},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xc0}, //saturation setting
		{0x3421,0xc0},
		{0x3500,0xc1},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x23},//PWBGAINGR[7:0]
		{0x3323,0x23},//PWBGAINGB[7:0]
		{0x3324,0x05},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
		{0x350D,0x55},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x55},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x95},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x54},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
      	},

      { //SCENE_MODE_SPORTS  运动
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x50},//ALCAIM[7:0]
		{0x351B,0x08},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]
		{0x3441,0x89},//LCONT_LEV[7:0]
		//{0x3422,0xc0},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xc0}, //saturation setting
		{0x3421,0xc0},
		{0x3500,0xc1},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x23},//PWBGAINGR[7:0]
		{0x3323,0x23},//PWBGAINGB[7:0]
		{0x3324,0x05},//PWBGAINR[7:0]
		{0x3325,0x50},//PWBGAINB[7:0]
		{0x350D,0x55},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x55},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x95},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x54},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
	},

       {//SCENE_MODE_CANDLELIGHT 烛光
		{0x3501,0x01},//-/-/-/-/-/-/ALCAIM[9:8]
		{0x3502,0x30},//ALCAIM[7:0]
		{0x351B,0x98},//FAUTO/FCOUNT[2:0]/FCLSBON/EXPLIM[2:0]   
		{0x3441,0x94},//LCONT_LEV[7:0]
		//{0x3422,0xd0},//Cbr_MGAIN[7:0]
		{0x3422,0x00},//Cbr_MGAIN[7:0]
		{0x341E,0xd0}, //saturation setting
		{0x3421,0xd0},
		{0x3500,0x81},//ALCSW/AWBSW/ALCLOCK/-/ESLIMMODE/ROOMDET/-/ALCLIMMODE
		{0x3322,0x50},//PWBGAINGR
		{0x3323,0x50},//PWBGAINGB
		{0x3324,0x00},//PWBGAINR
		{0x3325,0xf0},//PWBGAINB
		{0x350D,0x19},//A1WEIGHT[1:0]/A2WEIGHT[1:0]/A3WEIGHT[1:0]/A4WEIGHT[1:0]
		{0x350E,0x0A},//A5WEIGHT[1:0]/B1WEIGHT[1:0]/B2WEIGHT[1:0]/B3WEIGHT[1:0]
		{0x350F,0x81},//B4WEIGHT[1:0]/B5WEIGHT[1:0]/C1WEIGHT[1:0]/C2WEIGHT[1:0]
		{0x3510,0x90},//C3WEIGHT[1:0]/C4WEIGHT[1:0]/C5WEIGHT[1:0]/-/-
       },
        
};

static struct msm_camera_i2c_conf_array t4k28_scene_confs[][1] = {
	{
		{t4k28_scene[0],
		ARRAY_SIZE(t4k28_scene[0]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	
	{
		{t4k28_scene[1],
		ARRAY_SIZE(t4k28_scene[1]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	
	{
		{t4k28_scene[2],
		ARRAY_SIZE(t4k28_scene[2]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{
		{t4k28_scene[3],
		ARRAY_SIZE(t4k28_scene[3]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	
	{
		{t4k28_scene[4],
		ARRAY_SIZE(t4k28_scene[4]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	
	{
		{t4k28_scene[5],
		ARRAY_SIZE(t4k28_scene[5]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{
		{t4k28_scene[6],
		ARRAY_SIZE(t4k28_scene[6]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	
	{
		{t4k28_scene[7],
		ARRAY_SIZE(t4k28_scene[7]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	{
		{t4k28_scene[8],
		ARRAY_SIZE(t4k28_scene[8]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	
	{
		{t4k28_scene[9],
		ARRAY_SIZE(t4k28_scene[9]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
	
	{
		{t4k28_scene[10],
		ARRAY_SIZE(t4k28_scene[10]), 0, MSM_CAMERA_I2C_BYTE_DATA},
	},
};

static int t4k28_scene_enum_map[] = {	
	MSM_V4L2_SCENE_AUTO,
	MSM_V4L2_SCENE_LANDSCAPE,
	MSM_V4L2_SCENE_FIREWORK,
	MSM_V4L2_SCENE_BEACH,
	MSM_V4L2_SCENE_PARTY,
	MSM_V4L2_SCENE_PORTRAIT,
	MSM_V4L2_SCENE_SUNSET,
	MSM_V4L2_SCENE_SNOW,
	MSM_V4L2_SCENE_NIGHT,
	MSM_V4L2_SCENE_SPORTS,
	MSM_V4L2_SCENE_CANDLELIGHT,

};

static struct msm_camera_i2c_enum_conf_array t4k28_scene_enum_confs = {
	.conf = &t4k28_scene_confs[0][0],
	.conf_enum = t4k28_scene_enum_map,
	.num_enum = ARRAY_SIZE(t4k28_scene_enum_map),
	.num_index = ARRAY_SIZE(t4k28_scene_confs),
	.num_conf = ARRAY_SIZE(t4k28_scene_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};


struct msm_sensor_v4l2_ctrl_info_t t4k28_v4l2_ctrl_info[] = {

	{
		.ctrl_id = V4L2_CID_SATURATION,
		.min = MSM_V4L2_SATURATION_L0,
		.max = MSM_V4L2_SATURATION_L4,
		.step = 1,
		.enum_cfg_settings = &t4k28_saturation_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},

	{
		.ctrl_id = V4L2_CID_CONTRAST,
		.min = MSM_V4L2_CONTRAST_L0,
		.max = MSM_V4L2_CONTRAST_L4,
		.step = 1,
		.enum_cfg_settings = &t4k28_contrast_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},

	{
		.ctrl_id = V4L2_CID_SHARPNESS,
		.min = MSM_V4L2_SHARPNESS_L0,
		.max = MSM_V4L2_SHARPNESS_L4,
		.step = 1,
		.enum_cfg_settings = &t4k28_sharpness_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},

	{
		.ctrl_id = V4L2_CID_BRIGHTNESS,
		.min = MSM_V4L2_BRIGHTNESS_L0,
		.max = MSM_V4L2_BRIGHTNESS_L4,
		.step = 1,
		.enum_cfg_settings = &t4k28_brightness_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},

    {
		.ctrl_id = V4L2_CID_EXPOSURE,
		.min = MSM_V4L2_EXPOSURE_N2,
		.max = MSM_V4L2_EXPOSURE_P2,
		.step = 1,
		.enum_cfg_settings = &t4k28_exposure_compensation_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},

	{
		.ctrl_id = V4L2_CID_COLORFX,
		.min = MSM_V4L2_EFFECT_OFF,
		.max = MSM_V4L2_EFFECT_GREENISH,
		.step = 1,
		.enum_cfg_settings = &t4k28_effect_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,

	},

	{
		.ctrl_id = V4L2_CID_AUTO_WHITE_BALANCE,
		.min = MSM_V4L2_WB_AUTO,
		.max = MSM_V4L2_WB_CLOUDY_DAYLIGHT,
		.step = 1,
		.enum_cfg_settings = &t4k28_awb_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},
   
	{
		.ctrl_id = V4L2_CID_POWER_LINE_FREQUENCY,//antibanding
		.min = MSM_V4L2_POWER_LINE_60HZ,
		.max = MSM_V4L2_POWER_LINE_AUTO,
		.step = 1,
		.enum_cfg_settings = &t4k28_antibanding_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},

	{
		.ctrl_id = V4L2_CID_ISO,//ISO
		.min = MSM_V4L2_ISO_AUTO,
		.max = MSM_V4L2_ISO_1600,
		.step = 1,
		.enum_cfg_settings = &t4k28_iso_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},

	{
		.ctrl_id = V4L2_CID_SCENE,//scene
		.min = MSM_V4L2_SCENE_AUTO,
		.max = MSM_V4L2_SCENE_CANDLELIGHT,
		.step = 1,
		.enum_cfg_settings = &t4k28_scene_enum_confs,
		.s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	},
};


static struct msm_camera_csi_params t4k28_csi_params = {
	.data_format = CSI_8BIT,
	.lane_cnt    = 1,
	.lane_assign = 0xe4,
	.dpcm_scheme = 0,
	.settle_cnt  = 20,
};

static struct msm_camera_csi_params *t4k28_csi_params_array[] = {
	&t4k28_csi_params,
	//&t4k28_csi_params,
};

static struct msm_sensor_output_reg_addr_t t4k28_reg_addr = {
	.x_output = 0x3020,
	.y_output = 0x3022,
	.line_length_pclk = 0x3015 , ///?????
	.frame_length_lines = 0x3017 ,//??????

};

static struct msm_sensor_id_info_t t4k28_id_info = {
	.sensor_id_reg_addr = 0x3000,
	.sensor_id = 0x08,
};

static const struct i2c_device_id t4k28_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&t4k28_s_ctrl},
	{ }
};


static ssize_t camera_id_read_proc(char *page,char **start,off_t off,int count,int *eof,void* data)
{
    int ret;

    unsigned char *camera_status = "BACK Camera ID:Toshiba,T4K28, 2M YUV sensor";
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

static int32_t t4k28_sensor_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	rc = msm_sensor_i2c_probe(client,id);

    if(rc == 0)
    {
        camera_proc_file();
    }
	return rc;
}


static struct i2c_driver t4k28_i2c_driver = {
	.id_table = t4k28_i2c_id,
	.probe  = t4k28_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client t4k28_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static int __init msm_sensor_init_module(void)
{
	int rc = 0;

	rc = i2c_add_driver(&t4k28_i2c_driver);

	return rc;
}

static struct v4l2_subdev_core_ops t4k28_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops t4k28_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops t4k28_subdev_ops = {
	.core = &t4k28_subdev_core_ops,
	.video  = &t4k28_subdev_video_ops,
};

static struct msm_sensor_fn_t t4k28_func_tbl = {
	.sensor_start_stream =msm_sensor_start_stream,
	.sensor_stop_stream =msm_sensor_stop_stream,
	.sensor_csi_setting = t4k28_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
};

static struct msm_sensor_reg_t t4k28_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = t4k28_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(t4k28_start_settings),
	.stop_stream_conf = t4k28_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(t4k28_stop_settings),
	.init_settings = &t4k28_init_conf[0],
	.init_size = ARRAY_SIZE(t4k28_init_conf),
	.mode_settings = &t4k28_confs[0],
	.output_settings = &t4k28_dimensions[0],
	.num_conf = ARRAY_SIZE(t4k28_confs),
};

static struct msm_sensor_ctrl_t t4k28_s_ctrl = {
	.msm_sensor_reg = &t4k28_regs,
	.msm_sensor_v4l2_ctrl_info = t4k28_v4l2_ctrl_info,
	.num_v4l2_ctrl = ARRAY_SIZE(t4k28_v4l2_ctrl_info),
	.sensor_i2c_client = &t4k28_sensor_i2c_client,
	.sensor_i2c_addr = 0x78,
	.sensor_output_reg_addr = &t4k28_reg_addr,
	.sensor_id_info = &t4k28_id_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.csic_params = &t4k28_csi_params_array[0],
	.msm_sensor_mutex = &t4k28_mut,
	.sensor_i2c_driver = &t4k28_i2c_driver,
	.sensor_v4l2_subdev_info = t4k28_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(t4k28_subdev_info),
	.sensor_v4l2_subdev_ops = &t4k28_subdev_ops,
	.func_tbl = &t4k28_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Toshiba 2M YUV sensor driver");
MODULE_LICENSE("GPL v2");

