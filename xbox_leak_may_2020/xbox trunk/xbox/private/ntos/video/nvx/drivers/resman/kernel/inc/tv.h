#ifndef _TV_H_
#define _TV_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/***************************************************************************\
*                                                                           *
* Module: TV.H                                                              *
*       The TV flicker filter constants are defined in this module.         *
*                                                                           *
*****************************************************************************
*/

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

#define	MY_SCALED_IMAGE_CTX	0x97100200
#define	X_RES	(640)		// source x
#define	Y_RES	(480)		// source y
//#define DST_X_RES (640)		// destination x
//#define DST_Y_RES (480)		// destination y
#define DST_X_RES (620)		// destination x
#define DST_Y_RES (420)		// destination y

#define I2C_AUTOINC 0x40    //OR this in to a register to set auto-increment mode.

// TV mode numbers
#define TV_MODE_320x200 0
#define TV_MODE_320x240 1
#define TV_MODE_320x400 2
#define TV_MODE_400x300 3
#define TV_MODE_480x360 4
#define TV_MODE_512x384 5
#define TV_MODE_640x400 6
#define TV_MODE_640x480 7  
#define TV_MODE_800x600 8
#define TV_MODE_DVD     9   // in NTSC this is 720x480, for PAL this is 704x576 or 720x576
#define TV_MODE_1024x768 10


// Brooktree register defs
// Reg CE = output modes. DAC A = bits 0,1, DAC B = 2,3, etc.
// Mode 0: Composite
// Mode 1: Luma delayed composite, Luma, Chroma, Delayed Luma (composite on S-Video connector)
// Mode 2: S-video (component YUV and delayed LUMA)
// Mode 3: RGB
// We use modes 0, 1, and 2
#define BT_OUT_MUX			0xce	// register #
#define BT_OUT_MUX_A_COMP	0		// values
#define BT_OUT_MUX_A_ALL	1
#define BT_OUT_MUX_A_SVID	2
#define BT_OUT_MUX_B_COMP	0
#define BT_OUT_MUX_B_ALL	1<<2
#define BT_OUT_MUX_B_SVID	2<<2
#define BT_OUT_MUX_C_COMP	0
#define BT_OUT_MUX_C_ALL	1<<4
#define BT_OUT_MUX_C_SVID	2<<4

// Brooktree registers
#define BT_HS0   0x6E
#define BT_HS1   0x70
#define BT_CA0   0xCA
#define BT_CA1   0xCC
#define BT_VAL   0x96
#define BT_FF0   0xC8
#define BT_FF1   0x34
#define BT_FF2   0x36

// Monitor Connection Status
#define BT_MONSTAT			0xe0
#define BT_MONSTAT_A		0x80
#define BT_MONSTAT_B		0x40
#define BT_MONSTAT_C		0x20

// DAC enable/check status reg
#define BT_DAC_CTL          0xBA
#define BT_DAC_OFF          0x10    
#define BT_DAC_ON           0x00    // normal setting
#define BT_CHK_STAT         0x40
#define BT_CHK_OFF          0x00
#define BT_CONFIG           0xB8

// Chrontel registers
#define CH_FF	0x29	// 5:4 = chroma, 3:2 = text, 1:0 = luma	
#define CH_BL   0x7f	// 7:0, 90 < x < 208
#define CH_CE	0x03	// 2:0

// Init regs struct
typedef struct
{
	U008	subaddr;	// register address of 1st register in this string
	U008	reg0xC4;
	U008	reg0xC6;
	U008	reg0xC8;
	U008	reg0xCA;
	U008	reg0xCC;
	U008	reg0xCE;
} BT_INIT_REGS;

// Modeset regs 
typedef struct
{
	U008	subaddr;	// register address of 1st register in this string
	U008	reg0x76;
	U008	reg0x78;
	U008	reg0x7A;
	U008	reg0x7C;
	U008	reg0x7E;
	U008	reg0x80;	// HBLANKO
	U008	reg0x82;	// VBLANKO
	U008	reg0x84;
	U008	reg0x86;
	U008	reg0x88;
	U008	reg0x8A;
	U008	reg0x8C;
	U008	reg0x8E;
	U008	reg0x90;
	U008	reg0x92;	// VBLANKI
	U008	reg0x94;
	U008	reg0x96;
	U008	reg0x98;	// VSCALE
	U008	reg0x9A;	// HBLANKO
	U008	reg0x9C;
	U008	reg0x9E;
	U008	reg0xA0;
	U008	reg0xA2;
	U008	reg0xA4;
	U008	reg0xA6;
	U008	reg0xA8;
	U008	reg0xAA;
	U008	reg0xAC;
	U008	reg0xAE;
	U008	reg0xB0;
	U008	reg0xB2;
	U008	reg0xB4;
	U008	reg0xB6;
} BT_MODE_REGS;

//	Position regs
typedef struct
{
	U008	reg0x6E;
	U008	reg0x70;
	U008	reg0x72;
	U008	reg0x74;
} BT_POS_REGS;

//	Chrontel encoder mode set struct
typedef struct
{
	U008	reg0x00;
	U008	reg0x07;
	U008	reg0x0a;
	U008	reg0x0b;
	U008	reg0x13;
	U008	reg0x14;
	U008	reg0x15;
} CH_MODE_REGS;

// Chrontel mode registers - bit 7 should always be set
#define CH_REG_00   0x00 | 0x80
#define CH_REG_01   0x01 | 0x80
#define CH_REG_03   0x03 | 0x80
#define CH_REG_04   0x04 | 0x80
#define CH_REG_06   0x06 | 0x80
#define CH_REG_07   0x07 | 0x80
#define CH_REG_08   0x08 | 0x80
#define CH_REG_09   0x09 | 0x80
#define CH_REG_0A   0x0A | 0x80
#define CH_REG_0B   0x0B | 0x80
#define CH_REG_0D   0x0D | 0x80
#define CH_REG_0E   0x0E | 0x80
#define CH_REG_10   0x10 | 0x80
#define CH_REG_11   0x11 | 0x80
#define CH_REG_13   0x13 | 0x80
#define CH_REG_18   0x18 | 0x80
#define CH_REG_19   0x19 | 0x80
#define CH_REG_1A   0x1A | 0x80
#define CH_REG_1B   0x1B | 0x80
#define CH_REG_1C   0x1C | 0x80
#define CH_REG_1D   0x1D | 0x80
#define CH_REG_1E   0x1E | 0x80
#define CH_REG_1F   0x1F | 0x80
#define CH_REG_21   0x21 | 0x80
#define CH_REG_3D   0x3D | 0x80

// DAC PM control
#define CH_ON       0x0B
#define CH_OFF      0x09

// TV standard specified in reg 0 bits 4:3
#define CH_NTSC_M   0x00
#define CH_NTSC_J   0x18
#define CH_PAL_M    0x10
#define CH_PAL_OTHER 0x08
// NTSC-J wants different blackness level
// Range is 0x5A - 0xD0. Default is 7F.
// Recommended:  NTSC-M & PAL-M 7F, NTSC-J 0x64, PAL 0x69
#define CH_BL_J     0x64
#define CH_BL_M     0x7f
#define CH_REG_06_DEF 0x10  // MCP set: latch data on positive edge
#define CH_CFR      0x80
#define CH_GAIN     0xBF    // mask gain bit
#define CH_GAIN_NTSC 0x00   // NTSC-M & PAL-M
#define CH_GAIN_PAL 0x40    // PAL & NTSC-J

#define PH_CHIP_ID  0x1C
#define PH_STDCTL   0x61
#define PH_DAC_POWER 0x40
#define PH_DAC_OFF  0x40
#define PH_DAC_ON   0x00

#endif // _TV_H#
