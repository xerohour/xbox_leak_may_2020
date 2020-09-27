 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/******************************* DAC TV Control ****************************\
*                                                                           *
* Module: DACTV.C                                                           *
*   Mode set for TV is done here.                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <dac.h>
#include <os.h>
#include "nvhw.h"
#include "vga.h"
#include "i2c.h"
#include "tv.h"
#include <nvcm.h>
#include <edid.h>

extern char strDevNodeRM[];

U008 dacGetTVmode(PHWINFO, U032);

//
//  I2C  Strings
//
//---------------------------------------------------------------------------------------
// Philips


//-----------------------------------------------------------------------------
U008 Ph_Gen_Init0[] = {  
              0x01,            //subaddress 01
              0x00,            //reg 1 - load macrovision registers with 0's ???  
              0x00,            //reg 2  
              0x00,            //reg 3  
              0x00,            //reg 4  
              0x00,            //reg 5  
              0x00,            //reg 6  
              0x00,            //reg 7  
              0x00,            //reg 8  
              0x00,            //reg 9  
              0x00,            //reg a  
              0x00,            //reg b  
              0x00,            //reg c  
              0x00,            //reg d  
              0x00,            //reg e  
              0x00,            //reg f  
              0x00,            //reg 10 
              0x00,            //reg 11 
              0x00,            //reg 12 
              0x00,            //reg 13  
              0x00,            //reg 14
              0x00,            //reg 15  
              0x00,            //reg 16
              0x1b,            //reg 17
              0x1b,            //reg 18  
              0x1f,            //reg 19
              0x46,            //reg 1a
              0x00,            //reg 1b  
              0x00,            //reg 1c
              0x00,            //reg 1d  
              0x00,            //reg 1e
              0x00,            //reg 1f  
              0x00,            //reg 20
              0x1b,            //reg 21
              0x00,            //reg 22
              0x00,            //reg 23  
              0x00,            //reg 24
              0x00,            //reg 25  
              0xff,            //reg26                  
              0x3f,            //reg27                  
              0x19,            //reg28 Burst start: PAL=21h,NTSC=19h                 
              0x1d,            //reg29 Burst End: PAL=1dh,NTSC=1dh                 
              0xff,            //reg2a copy guard          
              0xff,            //reg2b copy guard          
              0x0f,            //reg2c cg data             
              0xb4,            //reg2d (clock polarity) 
              0x00,            //reg2e null             
              0x00,            //reg2f null             
              0x00,            //reg30 macro-vision     
              0x00,            //reg31 macro data 0        
              0x00,            //reg32                  
              0x00,            //reg33 data                
              0x00,            //reg34                  
              0x00,            //reg35                  
              0x00,            //reg36 data             
              0x00,            //reg37                  
              0x1a,            //reg38 RGB luma gain       
              0x1a,            //reg39 color gain diff.    
              0x00             //reg3a Color burst Enable    3Ah = 58
};
#define Ph_Gen_Init0_Len sizeof(Ph_Gen_Init0)
        
//----------------------------------------------------------------------------
U008 Ph_Gen_Init1[] = {
              0xa2,            //subaddress a2                2
              0x10,            //rega2 border color black     3   
              0x80,            //rega3 border color           4   
              0x80             //rega4 border color           5
};
#define Ph_Gen_Init1_Len sizeof(Ph_Gen_Init1)

//-----------------------------------------------------------------------------
U008 Ph_Gen_Init2[] = {
              0xfc,            
              0x03,            //regfc cursor position, move cursor off screen     
              0x98             //regfd lut off for now...         
};
#define Ph_Gen_Init2_Len sizeof(Ph_Gen_Init2)

// Format dependent init tables
// Init string 0
U008 Ph_NTSC_Init0[] = {
              0x54,            //start at address 54,
              0x01,            //reg 54 Internal PPD1 & PPD2 clock edge
              0x0f,            //reg 55
              0xc3,            //reg 56
              0x00,            //reg 57
              0x02,            //reg 58
              0x30,            //reg 59
              0x00,            //reg 5a Chrominance Phase (was 88)
              0x76,            //reg 5b
              0xa5,            //reg 5c 
              0x3a,            //reg 5d 
              0x2e,            //reg 5e 
              0x2e,            //reg 5f Cross Color Reduction (was 6e)
              0x00,            //reg 60                 
              0x15,            //reg 61 
              0x3f,            //reg 62 
              0x1f,            //reg 63 
              0x7c,            //reg 64 
              0xf0,            //reg 65  
              0x21,            //reg 66  
              0x00,            //reg 67  
              0x00,            //reg 68  
              0x00,            //reg 69  
              0x00,            //reg 6a
              0x00,            //reg 6b reserved
              0x01,            //reg 6c  
              0x20,            //reg 6d  
              0x00,            //reg 6e Blanking Control
              0x42,            //reg 6f CCN b[7:6], SCCLN b[4:0] (was 00)
              0x48,            //reg 70
              0x44,            //reg 71
              0x61,            //reg 72
              0x90,            //reg 73
              0x94,            //reg 74
              0xB0,            //reg 75
              0x78,            //reg 76
              0xf9,            //reg 77
              0x2a,            //reg 78
              0x2a,            //reg 79
              0x29,            //reg 7a >>> was 29, maybe 1d
              0x01,            //reg 7b >>> was 01, maybe f9 or de
              0x00,            //reg 7c
              0x00,            //reg 7d
              0x0c,            //reg 7e
              0x0d,            //reg 7f
              0x00,            //reg 80
              0x48,            //reg 81 >>> was 48, maybe ad
              0x2a,            //reg 82
              0x21             //reg 83
};
#define Ph_NTSC_Init0_Len sizeof(Ph_NTSC_Init0)

//-----------------------------------------------------------------------------
U008 Ph_NTSC_Init1[] = {
              0x90,            //start at address 90,
              0x5c,            //reg 90
              0x40,            //reg 91
              0x58,            //reg 92
              0x58,            //reg 93
              0x04,            //reg 94
              0xa0,            //reg 95
              0x02,            //reg 96
              0x2e,            //reg 97
              0x1f,            //reg 98
              0x13,            //reg 99
              0xf4,            //reg 9a
              0x33,            //reg 9b
              0x7f,            //reg 9c
              0x99,            //reg 9d
              0x99             //reg 9e
};
#define Ph_NTSC_Init1_Len sizeof(Ph_NTSC_Init1)

// Init string 0
U008 Ph_PAL_Init0[] = {
              0x54,            //start at address 54,
              0x01,            //reg 54 Internal PPD1 & PPD2 clock edge
              0x0f,            //reg 55
              0xc3,            //reg 56
              0x00,            //reg 57
              0x02,            //reg 58
              0x30,            //reg 59
              0x00,            //reg 5a Chrominance Phase (was 88)
              0x7d,            //reg 5b
              0xaf,            //reg 5c 
              0x33,            //reg 5d 
              0x35,            //reg 5e 
              0x75,            //reg 5f Cross Color Reduction (was 6e)
              0x00,            //reg 60                 
              0x02,            //reg 61 
              0x2f,            //reg 62 
              0xcb,            //reg 63 
              0x8a,            //reg 64 
              0x09,            //reg 65  
              0x2a,            //reg 66  
              0x00,            //reg 67  
              0x00,            //reg 68  
              0x00,            //reg 69  
              0x00,            //reg 6a
              0x00,            //reg 6b reserved
              0x01,            //reg 6c  
              0x20,            //reg 6d  
              0x00,            //reg 6e Blanking Control
              0x42,            //reg 6f CCN b[7:6], SCCLN b[4:0] (was 00)
              0x56,            //reg 70
              0x52,            //reg 71
              0x61,            //reg 72
              0x90,            //reg 73
              0x94,            //reg 74
              0xB0,            //reg 75
              0x78,            //reg 76
              0xf9,            //reg 77
              0x2a,            //reg 78
              0xfa,            //reg 79
              0x26,            //reg 7a >>> was 29, maybe 1d
              0x27,            //reg 7b >>> was 01, maybe f9 or de
              0x40,            //reg 7c
              0x00,            //reg 7d
              0x0c,            //reg 7e
              0x0d,            //reg 7f
              0x00,            //reg 80
              0x00,            //reg 81 >>> was 48, maybe ad
              0x00,            //reg 82
              0x1c             //reg 83
};
#define Ph_PAL_Init0_Len sizeof(Ph_PAL_Init0)

//-----------------------------------------------------------------------------
U008 Ph_PAL_Init1[] = {
              0x90,            //start at address 90,
              0x68,            //reg 90
              0x40,            //reg 91
              0x45,            //reg 92
              0x45,            //reg 93
              0x04,            //reg 94
              0xb8,            //reg 95
              0x02,            //reg 96
              0x2e,            //reg 97
              0x25,            //reg 98
              0x53,            //reg 99
              0xf4,            //reg 9a
              0x89,            //reg 9b
              0x8f,            //reg 9c
              0x43,            //reg 9d
              0x43,            //reg 9e
              0x4c
};
#define Ph_PAL_Init1_Len sizeof(Ph_PAL_Init1)

// Mode Tables
U008 Ph_NTSC_640[] = {
                0x70,       //start at address 70  
                0x3e,       //reg 70 Active display H Start (horizontal centering)
                0x3e,       //reg 71 Active display H End
                0x61,       //reg 72=0x61
                0x90,       //reg 73=0x90
                0x94,       //reg 74=0x94
                0xb0,       //reg 75=0xb0
                0x78,       //reg 76=0x78
                0xf9,       //reg 77=0xf9
                0x2a,       //reg 78=0x2a
                0xfa,       //reg 79=0xfa
                0x20,       //reg 7a
                0xf6,       //reg 7b
                0x00,       //reg 7c=0x00
                0x00,       //reg 7d=0x00
                0x0c,       //reg 7e=0x0c
                0x0d,       //reg 7f=0x0d
                0x00,       //reg 80=0x00
                0x00,       //reg 81  Pix clk 0
                0x00,       //reg 82  Pix clk 1
                0x20,       //reg 83  Pix Clk 2
                0x00,       //reg 84=0x00
                0x00,       //reg 85=0x00
                0x00,       //reg 86=0x00
                0x00,       //reg 87=0x00
                0x00,       //reg 88=0x00
                0x00,       //reg 89=0x00
                0x00,       //reg 8a=0x00
                0x00,       //reg 8b=0x00
                0x00,       //reg 8c=0x00
                0x00,       //reg 8d=0x00
                0x00,       //reg 8e=0x00
                0x00,       //reg 8f=0x00
                0x64,       //reg 90=0x60
                0x40,       //reg 91=0x40
                0x45,       //reg 92
                0x45,       //reg 93
                0x04,       //reg 94=0x04
                0xa0,       //reg 95=0xa0
                0x02,       //reg 96=0x02
                0x2e,       //reg 97=0x2e
                0xf9,       //reg 98=0x1f
                0x12,       //reg 99=0x13
                0x00,       //reg 9a=0x00
                0x1b,       //reg 9b
                0x70,       //reg 9c
                0x8d,       //reg 9d
                0x8d,       //reg 9e
                0x3b,       //reg 9f=0x3b
                0x00,       //reg a0=0x00
                0x00,       //reg a1=0x00
                0x10,       //reg a2 
                0x80,       //reg a3  
                0x80        //reg a4  
};
#define Ph_NTSC_Res_Len sizeof(Ph_NTSC_640)

U008 Ph_NTSC_800[] = {
                0x70,        //begin at register 70
                0x4b,        //reg 70
                0x23,        //reg 71
                0x61,        //reg 72
                0x90,        //reg 73
                0x94,        //reg 74
                0xb0,        //reg 75
                0x78,        //reg 76
                0xf9,        //reg 77
                0x2a,        //reg 78
                0xfa,        //reg 79
                0x1e,        //reg 7a
                0xf4,        //reg 7b
                0x00,        //reg 7c
                0x00,        //reg 7d
                0x0c,        //reg 7e
                0x0d,        //reg 7f
                0x00,        //reg 80
                0x00,        //reg 81 Pix clk 0
                0x00,        //reg 82 Pix clk 1
                0x40,        //reg 83 Pix clk 2
                0x00,        //reg 84
                0x00,        //reg 85
                0x00,        //reg 86
                0x00,        //reg 87
                0x00,        //reg 88
                0x00,        //reg 89
                0x00,        //reg 8a
                0x00,        //reg 8b
                0x00,        //reg 8c
                0x00,        //reg 8d
                0x00,        //reg 8e
                0x00,        //reg 8f
                0x62,        //reg 90 [Wiped out by fine adjust]
                0x90,        //reg 91
                0x51,        //reg 92
                0x51,        //reg 93
                0x04,        //reg 94
                0xb8,        //reg 95
                0x02,        //reg 96
                0x2e,        //reg 97
                0xc5,        //reg 98 HLen 
                0x64,        //reg 99 IDEL + HLen
                0x67,        //reg 9a
                0xb1,        //reg 9b
                0x5c,        //reg 9c
                0xd8,        //reg 9d
                0xd8,        //reg 9e
                0x2a,        //reg 9f
                0x00,        //reg a0
                0x00,        //reg a1
                0x10,        //reg a2
                0x80,        //reg a3
                0x80         //reg a4
};

U008 Ph_PAL_640[] = {
                0x70,       //start at address 70  
                0x81,       //reg 70=0x44
                0x59,       //reg 71=0x44
                0x61,       //reg 72=0x61
                0x90,       //reg 73=0x90
                0x94,       //reg 74=0x94
                0xb0,       //reg 75=0xb0
                0x78,       //reg 76=0x78
                0xf9,       //reg 77=0xf9
                0x2a,       //reg 78=0x2a
                0xfa,       //reg 79=0xfa
                0x26,       //reg 7a=0x29  >>> was 29, maybe 1d
                0x27,       //reg 7b=0x01  >>> was 01, maybe f9 or de
                0x40,       //reg 7c=0x00
                0x00,       //reg 7d=0x00
                0x0c,       //reg 7e=0x0c
                0x0d,       //reg 7f=0x0d
                0x00,       //reg 80=0x00
                0x00,       //reg 81=0x48  Pix clk 0
                0x00,       //reg 82=0x21  Pix clk 1
                0x1c,       //reg 83=0x21  Pix Clk 2
                0x00,       //reg 84=0x00
                0x00,       //reg 85=0x00
                0x00,       //reg 86=0x00
                0x00,       //reg 87=0x00
                0x00,       //reg 88=0x00
                0x00,       //reg 89=0x00
                0x00,       //reg 8a=0x00
                0x00,       //reg 8b=0x00
                0x00,       //reg 8c=0x00
                0x00,       //reg 8d=0x00
                0x00,       //reg 8e=0x00
                0x00,       //reg 8f=0x00
                0x68,       //reg 90=0x60
                0x40,       //reg 91=0x40
                0x45,       //reg 92=0x58  >>> was 58, maybe 3d
                0x45,       //reg 93=0x58  >>> was 58, maybe 3d
                0x04,       //reg 94=0x04
                0xb8,       //reg 95=0xa0
                0x02,       //reg 96=0x02
                0x2e,       //reg 97=0x2e
                0x25,       //reg 98=0x1f
                0x53,       //reg 99=0x13
                0x81,       //reg 9a=0x00
                0x89,       //reg 9b=0x33  >>> was 33, maybe 4e
                0x8f,       //reg 9c=0x70  >>> was 70, maybe 70
                0x43,       //reg 9d=0x99  >>> was 99, maybe a6
                0x43,       //reg 9e=0x99  >>> was 99, maybe a6
                0x4c,       //reg 9f=0x3b
                0x00,       //reg a0=0x00
                0x00,       //reg a1=0x00
                0x10,       //reg a2 
                0x80,       //reg a3  
                0x80        //reg a4  
};
#define Ph_PAL_Res_Len sizeof(Ph_PAL_640)

U008 Ph_PAL_800[] = {
                0x70,        //begin at register 70
                0x5d,        //reg 70
                0x51,        //reg 71
                0x61,        //reg 72
                0x90,        //reg 73
                0x94,        //reg 74
                0xb0,        //reg 75
                0x78,        //reg 76
                0xf9,        //reg 77
                0x2a,        //reg 78
                0xfa,        //reg 79
                0x24,        //reg 7a
                0x29,        //reg 7b
                0x40,        //reg 7c
                0x00,        //reg 7d
                0x0c,        //reg 7e
                0x0d,        //reg 7f
                0x00,        //reg 80
                0x00,        //reg 81 Pix clk 0
                0x00,        //reg 82 Pix clk 1
                0x2c,        //reg 83 Pix clk 2
                0x00,        //reg 84
                0x00,        //reg 85
                0x00,        //reg 86
                0x00,        //reg 87
                0x00,        //reg 88
                0x00,        //reg 89
                0x00,        //reg 8a
                0x00,        //reg 8b
                0x00,        //reg 8c
                0x00,        //reg 8d
                0x00,        //reg 8e
                0x00,        //reg 8f
                0x66,        //reg 90 [Wiped out by fine adjust]
                0x90,        //reg 91
                0x50,        //reg 92
                0x50,        //reg 93
                0x04,        //reg 94
                0xb8,        //reg 95
                0x02,        //reg 96
                0x2e,        //reg 97
                0x07,        //reg 98 ,LEN was 0xxff, c,anged to non-sentinel value
                0x54,        //reg 99
                0xaf,        //reg 9a
                0xf2,        //reg 9b
                0x6c,        //reg 9c
                0x77,        //reg 9d
                0x77,        //reg 9e
                0x3b,        //reg 9f
                0x00,        //reg a0
                0x00,        //reg a1
                0x10,        //reg a2
                0x80,        //reg a3
                0x80         //reg a4
};


// Autoconfigure hack
U008    Bt_NM800[] = { 0xB8, 0x02, 0xff };
U008    Bt_NM640[] = { 0xB8, 0x00, 0xff };
//
// Brooktree strings
//

//  Init string               C4    C6    C8            CA           CC             CE
U008    BT_Init[] = { 0xC4, 0x01, 0x98, BT_FLICKER_MED, BT_LUMA_DEF, BT_CHROMA_DEF, (BT_OUT_MUX_B_SVID | BT_OUT_MUX_C_ALL), 0xff };
U008    BT871_Init[] = { 0xC4, 0x01, 0x98, BT_FLICKER_MED, BT871_LUMA_DEF, BT871_CHROMA_DEF, (BT_OUT_MUX_B_SVID | BT_OUT_MUX_C_ALL), 0xff };

/************************************************************************************************
    We now support 6 distinct TV standards: NTSC M, NTSC J, PAL NC, PAL M, PAL N, and
    PAL B, D, G, I (which are the same as far as we are concerned).
*/
//---------------------------------------------------------------------------------------
// Brooktree 800x600

// NTSC M 800x600
U008 Bt_NM600[] = {
// 0x6e, 0xe7, 0xc2, 0x00, 0x01,
// 76    78    7A    7C    7E    80    82    84    86    88    8A    8C    8E    90
 0x76, 
 0xA0, 0x20, 0xB6, 0xD8, 0xB9, 0x55, 0x20, 0xD8, 0x39, 0x00, 0x70, 0x42, 0x03, 0xDF,
// 92    94    96    98    9A    9C    9E    A0    A2    A4    A6    A8    AA    AC    AE
 0x56, 0x58, 0x3A, 0xCD, 0x9C, 0x14, 0x3B, 0x11, 0x0A, 0xE5, 0x74, 0x77, 0x43, 0x85, 0xBA,
// B0    B2    B4    B6
 0xE8, 0xA2, 0x17, 0x00, 
 0xff};

// NTSC J 800x600
U008 Bt_NJ600[] = {
// 0x6e, 0xe7, 0xc2, 0x00, 0x01,
 0x76, 
 0xA0, 0x20, 0xB6, 0xD8, 0xB9, 0x55, 0x20, 0xD8, 0x39, 0x00, 0x70, 0x42, 0x03, 0xDF,
 0x56, 0x58, 0x3A, 0xCD, 0x9C, 0x14, 0x3B, 0x11, 0x08, 0xE5, 0x74, 0x77, 0x43, 0x85, 0xBA,
 0xE8, 0xA2, 0x17, 0x00,
 0xff};

// PAL NC 800x600
U008 Bt_NC600[] = {
// 0x6e, 0xe7, 0xc2, 0x00, 0x01,
 0x76, 
 0x00, 0x20, 0xAA, 0xCA, 0xA4, 0x0D, 0x29, 0xFC, 0x39, 0x00, 0xC0, 0x8C, 0x03, 0xEE, 
 0x5F, 0x58, 0x3A, 0x66, 0x96, 0x00, 0x00, 0x10, 0x24, 0xF0, 0x56, 0x7F, 0x47, 0x8C, 0x35,
 0xEF, 0x78, 0x19, 0x00,
 0xff};

// PAL B,D,G,I 800x600
U008 Bt_PA600[] = {
// 0x6e, 0x31, 0x02, 0x00, 0x01,
 0x76, 
 0x00, 0x20, 0xAA, 0xCA, 0x9A, 0x0D, 0x29, 0xFC, 0x39, 0x00, 0xC0, 0x8C, 0x03, 0xEE,
 0x5F, 0x58, 0x3A, 0x66, 0x96, 0x00, 0x00, 0x10, 0x24, 0xF0, 0x57, 0x80, 0x48, 0x8C, 0x18,
 0x28, 0x87, 0x1F, 0x00,
 0xff};

// PAL M 800x600
U008 Bt_PM600[] = {
// 0x6e, 0x31, 0x02, 0x00, 0x01,
 0x76, 
 0xA0, 0x20, 0xB6, 0xE0, 0xC2, 0x55, 0x20, 0xD8, 0x39, 0x00, 0x70, 0x42, 0x03, 0xDF,
 0x56, 0x58, 0x3A, 0xCD, 0x9C, 0x14, 0x3B, 0x11, 0x2A, 0xF0, 0x56, 0x7E, 0x47, 0x8C, 0x7E,
 0x42, 0x9C, 0x17, 0x00,
 0xff};

// PAL N 800x600
U008 Bt_PN600[] = {
// 0x6e, 0x31, 0x02, 0x00, 0x01,
0x76, 
0x00, 0x20, 0xAA, 0xCA, 0x9A, 0xF3, 0x29, 0xFC, 0x39, 0x00, 0xC0, 0x8C, 0x03, 0xEE,
0x5F, 0x58, 0x3A, 0x66, 0x56, 0x00, 0x00, 0x10, 0x2E, 0xF0, 0x57, 0x80, 0x48, 0x8C, 0x18,
0x28, 0x87, 0x1F, 0x00,
0xff};

//---------------------------------------------------------------------------------------
// Native TV modes with no overscan.  For use when playing DVD's

// Brooktree 720x480 NTSC
U008 Bt_NMDVD[] = {
//    76    78    7A    7C    7E    80    82    84    86    88    8A    8C    8E    90    92    94
    0x76, 
    0xE0, 0xD0, 0x82, 0x92, 0x5C, 0x1B, 0x13, 0xF2, 0x26, 0x00, 0x70, 0x8C, 0x03, 0x0D, 0x24, 0xE0, 
    0x36, 0x00, 0x50, 0xC5, 0x4E, 0x0C, 0x0A, 0xE5, 0x76, 0x79, 0x44, 0x85, 0xD1, 0x45, 0x17, 0x21, 
    0x00, 0xff};

U008 Bt_NJDVD[] = {
//    76    78    7A    7C    7E    80    82    84    86    88    8A    8C    8E    90    92    94
    0x76, 
    0x66, 0xD0, 0x82, 0x92, 0x5C, 0x1B, 0x13, 0xF2, 0x27, 0x00, 0x70, 0x8C, 0x03, 0x0D, 0x24, 0xE0, 
    0x36, 0x00, 0x50, 0xC5, 0x4E, 0x0C, 0x08, 0xE5, 0x76, 0x79, 0x44, 0x85, 0xD1, 0x45, 0x17, 0x21, 
    0x00, 0xff};

U008 Bt_NCDVD[] = {
//    76    78    7A    7C    7E    80    82    84    86    88    8A    8C	  8E	90    92    94
    0x76,
    0xF0, 0xD0, 0x82, 0x9C, 0x5A, 0x31, 0x16, 0x22, 0xa6, 0x00, 0x78, 0x93, 0x03, 0x71, 0x2A, 0x40,
    0x3A, 0x00, 0x50, 0x55, 0x55, 0x0C, 0x24, 0xF0, 0x59, 0x82, 0x49, 0x8C, 0x8E, 0xB0, 0xE6, 0x28,
    0x00, 0xff};

U008 Bt_PADVD[] = {
//    76    78    7A    7C    7E    80    82    84    86    88    8A    8C	  8E	90    92    94
    0x76,
    0xF0, 0xD0, 0x82, 0x9C, 0x5A, 0x31, 0x16, 0x22, 0xa6, 0x00, 0x78, 0x93, 0x03, 0x71, 0x2A, 0x40,
    0x3A, 0x00, 0x50, 0x55, 0x55, 0x0C, 0x24, 0xF0, 0x59, 0x82, 0x49, 0x8C, 0x8E, 0xB0, 0xE6, 0x28,
    0x00, 0xff};

U008 Bt_PMDVD[] = {
//    76    78    7A    7C    7E    80    82    84    86    88    8A    8C	  8E	90    92    94
    0x76,
    0xF0, 0xD0, 0x82, 0x9C, 0x5A, 0x31, 0x16, 0x22, 0xa6, 0x00, 0x78, 0x93, 0x03, 0x71, 0x2A, 0x40,
    0x3A, 0x00, 0x50, 0x55, 0x55, 0x0C, 0x24, 0xF0, 0x59, 0x82, 0x49, 0x8C, 0x8E, 0xB0, 0xE6, 0x28,
    0x00, 0xff};

U008 Bt_PNDVD[] = {
//    76    78    7A    7C    7E    80    82    84    86    88    8A    8C	  8E	90    92    94
    0x76,
    0xF0, 0xD0, 0x82, 0x9C, 0x5A, 0x31, 0x16, 0x22, 0xa6, 0x00, 0x78, 0x93, 0x03, 0x71, 0x2A, 0x40,
    0x3A, 0x00, 0x50, 0x55, 0x55, 0x0C, 0x24, 0xF0, 0x59, 0x82, 0x49, 0x8C, 0x8E, 0xB0, 0xE6, 0x28,
    0x00, 0xff};

//---------------------------------------------------------------------------------------

// Two no-overscan modes, which will be the default on X-box:

// Brooktree 640x480 NTSC
U008 Bt_NM480x[] = {
//    76    78    7A    7C    7E    80    82    84    86    88    8A    8C	   8E	 90
    0x76,
    0x08, 0x80, 0x72, 0x80, 0x42, 0xf1, 0x13, 0xf2, 0x26, 0x00, 0x04, 0x73, 0x03, 0x0d, 
    0x24, 0xe0, 0x00, 0x00, 0x10, 0x15, 0xcc, 0x0a, 0x0a, 0xe5, 0x77, 0x7a, 0x45, 0x85, // 92-AC
    0x81, 0x5f, 0xb8, 0x25, 0x00,             // AE-B6 (stop here: B8 is the autoconfig mode register)
    0xff};

// Brooktree 640x480 PAL. This one starts at reg 38.
U008 Bt_PA480x[] = {
// 38,   3A,   3C,   3E,   40,   42,   44,   46,   48,   4A,   4C,   4E,   50,   52,   54,   56,   58,   5A,   5C,   5E 
 0x38,
 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x01, 0x2e, 0x1a, 0x01, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x46, 0x00, 0x01,                                  // 60-74
 0x3c, 0x80, 0x76, 0x8c, 0x44, 0x1b, 0x16, 0x20, 0xA6, 0x00, 0xB6, 0xFB, 0x03, 0x0D,    // 76-90
 0x23, 0xe0, 0x06, 0xe1, 0x4a, 0x55, 0x15, 0x0b, 0x24, 0xf0, 0x59, 0x83, 0x4a, 0x8c,    // 92-AC
 0x33, 0x98, 0x83, 0x2d, 0x00,                                                          // AE-B6
 0xff};

// Normal modes used with 868/869:

 //---------------------------------------------------------------------------------------
// Brooktree 640x480

// NTSC 640x480
U008 Bt_NM480[] = {
// 0x6e, 0x26, 0x02, 0x00, 0x01,
// 76    78    7A    7C    7E    80    82    84    86    88    8A    8C    8E    90
 0x76, 
 0x00, 0x80, 0x84, 0x9E, 0x65, 0x7D, 0x22, 0xD4, 0x27, 0x00, 0x10, 0x7E, 0x03, 0x58,
 0x4B, 0xE0, 0x36, 0x92, 0x54, 0x0E, 0x88, 0x0C, 0x0A, 0xE5, 0x76, 0x79, 0x44, 0x85, 0x00,
 0x00, 0x80, 0x20, 0x00,
 0xff};

// NTSC J 640x480
U008 Bt_NJ480[] = {
// 0x6e, 0x26, 0x02, 0x00, 0x01,
 0x76, 
 0x00, 0x80, 0x84, 0x9E, 0x65, 0x7D, 0x22, 0xD4, 0x27, 0x00, 0x10, 0x7E, 0x03, 0x58, 
 0x4B, 0xE0, 0x36, 0x92, 0x54, 0x0E, 0x88, 0x0C, 0x08, 0xE5, 0x76, 0x79, 0x44, 0x85, 0x00,
 0x00, 0x80, 0x20, 0x00,
 0xff};

// PAL NC 640x480
U008 Bt_NC480[] = {
// 0x6e, 0xb2, 0x02, 0x00, 0x01,
 0x76, 
 0x60, 0x80, 0x8A, 0xA6, 0x70, 0xC1, 0x2E, 0xF2, 0x27, 0x00, 0xB0, 0x0A, 0x0B, 0x71, 
 0x5A, 0xE0, 0x36, 0x00, 0x50, 0x72, 0x1C, 0x0D, 0x24, 0xF0, 0x57, 0x80, 0x48, 0x8C, 0x1E,
 0xC0, 0x15, 0x1F, 0x00,
 0xff};

// PAL B,D,G,I 640x480
U008 Bt_PA480[] = {
// 0x6e, 0xb2, 0x02, 0x00, 0x01,
// 0x76, 78    7a    7c    7e    80    82    84    86    88    8a    8c    8e    90
 0x76,
 0x60, 0x80, 0x8A, 0xA6, 0x68, 0xC1, 0x2E, 0xF2, 0x27, 0x00, 0xB0, 0x0A, 0x0B, 0x71,
// 92    94    96    98    9a    9c    a0    a2    a4    a6    a8    aa    ac    ae    b0
 0x5A, 0xE0, 0x36, 0x00, 0x50, 0x72, 0x1C, 0x0D, 0x24, 0xF0, 0x58, 0x81, 0x49, 0x8C, 0x0C,
 0x8C, 0x79, 0x26, 0x00,
 0xff};

// PAL M 640x480
U008 Bt_PM480[] = {
// 0x6e, 0xb2, 0x02, 0x00, 0x01,
 0x76, 
 0x00, 0x80, 0x84, 0xA4, 0x6A, 0x7D, 0x22, 0xD4, 0x27, 0x00, 0x10, 0x7E, 0x03, 0x58, 
 0x4B, 0xE0, 0x36, 0x92, 0x54, 0x0E, 0x88, 0x0C, 0x2A, 0xF0, 0x57, 0x80, 0x48, 0x8C, 0x6E, 
 0xDB, 0x76, 0x20, 0x00,
 0xff};

// PAL N 640x480
U008 Bt_PN480[] = {
// 0x6e, 0xb2, 0x02, 0x00, 0x01,
 0x76, 
 0x60, 0x80, 0x8A, 0xA6, 0x68, 0xAB, 0x2E, 0xF2, 0x27, 0x00, 0xB0, 0x0A, 0x0B, 0x71, 
 0x5A, 0xE0, 0x36, 0x00, 0x50, 0x72, 0x1C, 0x0D, 0x2E, 0xF0, 0x58, 0x81, 0x49, 0x8C, 0x0C,
 0x8C, 0x79, 0x26, 0x00,
 0xff};

//---------------------------------------------------------------------------------------
// Brooktree 640x400

// NTSC M 640x400
U008 Bt_NM400[] = {
// 0x6e, 0xb2, 0x02, 0x00, 0x01,
 0x76, 
 0x00, 0x80, 0x84, 0x96, 0x60, 0x7D, 0x20, 0xD8, 0x27, 0x00, 0xC0, 0x18, 0x0B, 0xEA, 
 0x39, 0x90, 0x35, 0xDE, 0x4D, 0x0E, 0x88, 0x0C, 0x0A, 0xE5, 0x76, 0x79, 0x44, 0x85, 0x00,
 0x00, 0x80, 0x20, 0x00,
 0xff};

// NTSC J 640x400
U008 Bt_NJ400[] = {
// 0x6e, 0xc0, 0x02, 0x00, 0x01,
 0x76, 
 0x00, 0x80, 0x84, 0x96, 0x60, 0x7D, 0x20, 0xD8, 0x27, 0x00, 0xC0, 0x18, 0x0B, 0xEA, 
 0x39, 0x90, 0x35, 0xDE, 0x4D, 0x0E, 0x88, 0x0C, 0x08, 0xE5, 0x76, 0x79, 0x44, 0x85, 0x00, 
 0x00, 0x80, 0x20, 0x00,
 0xff};

// PAL NC 640x400
U008 Bt_NC400[] = {
// 0x6e, 0xc0, 0x02, 0x00, 0x01,
0x76, 
0x90, 0x80, 0x8E, 0xAA, 0x76, 0xDD, 0x35, 0xE5, 0x27, 0x00, 0x4C, 0x92, 0x0C, 0x26, 
0x5B, 0x90, 0x36, 0x29, 0x4C, 0xC7, 0x71, 0x0D, 0x24, 0xF0, 0x57, 0x80, 0x48, 0x8C, 0x6B, 
0x73, 0x50, 0x1E, 0x00, 
0xff};

// PAL B,D,G,I 640x400
U008 Bt_PA400[] = {
 0x6e, 0x20, 0x42, 0x00, 0x01,
//0x76,  78    7a    7c    7e    80    82    84    86    88    8a    8c    8e    90
  0x90, 0x80, 0x8E, 0xAA, 0x6E, 0xDD, 0x35, 0xE5, 0x27, 0x00, 0x4C, 0x92, 0x0C, 0x26, 
//  92    94    96    98    9a    9c    a0    a2    a4    a6    a8    aa    ac    ae    b0
  0x5B, 0x90, 0x36, 0x29, 0x4C, 0xC7, 0x71, 0x0D, 0x24, 0xF0, 0x58, 0x81, 0x49, 0x8C, 0xEA, 
  0x57, 0x85, 0x25, 0x00,
 0xff};

// PAL M 640x400
U008 Bt_PM400[] = {
 0x6e, 0x20, 0x42, 0x00, 0x01,
// 0x76, 
 0x00, 0x80, 0x84, 0xA4, 0x6A, 0x7D, 0x20, 0xD8, 0x27, 0x00, 0xC0, 0x18, 0x0B, 0xEA, 
 0x39, 0x90, 0x35, 0xDE, 0x4D, 0x0E, 0x88, 0x0C, 0x2A, 0xF0, 0x57, 0x80, 0x48, 0x8C, 0x6E, 
 0xDB, 0x76, 0x20, 0x00,
 0xff};

// PAL N 640x400
U008 Bt_PN400[] = {
 0x6e, 0x20, 0x42, 0x00, 0x01,
//0x76 
0x90, 0x80, 0x8E, 0xAA, 0x6E, 0xC5, 0x35, 0xE5, 0x27, 0x00, 0x4C, 0x92, 0x0C, 0x26, 
0x5B, 0x90, 0x36, 0x29, 0x4C, 0xC7, 0x71, 0x0D, 0x2E, 0xF0, 0x58, 0x81, 0x49, 0x8C, 0xEA, 
0x57, 0x85, 0x25, 0x00, 
0xff};


//---------------------------------------------------------------------------------------
//  Centering
//  H, V Offset. 6E = H Offset Low, 70[7,6] = H Offset High, 70[5:0] = H Sync Width
//               72 = V Offset Low, 74[5:3] = V Offset High, 74[2:0] = V Sync Width
//  NV4 Horizontal Offset should be 8 pixels less than NV5 & up (see programBrooktree()).
//---------------------------------------------------------------------------------------

// NTSC M 800x600
U008 Btc_NM600[] = {
 0x6e, 0xe0, 0xc2, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 800x600
U008 Btc_PA600[] = {
 0x6e, 0x2a, 0x02, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// NTSC 720x480
U008 Btc_NMDVD[] = {
 0x6e, 0x28, 0x02, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 720x576
U008 Btc_PADVD[] = {
 0x6e, 0x28, 0x02, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// NTSC 640x480
U008 Btc_NM480[] = {
 0x6e, 0x1e, 0x02, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 640x480
U008 Btc_PA480[] = {
 0x6e, 0xac, 0x02, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// NTSC M 640x400
U008 Btc_NM400[] = {
 0x6e, 0xb6, 0x02, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 640x400
U008 Btc_PA400[] = {
 0x6e, 0x2e, 0x42, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// NTSC M 320x400
// CURRENTLY UNUSED
U008 Btc_NM3x4[] = {
 0x6e, 0x00, 0x42, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 320X400
U008 Btc_PA3x4[] = {
 0x6e, 0x20, 0x42, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// NTSC M 512x384
U008 Btc_NM384[] = {
 0x6e, 0x08, 0x42, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 512x384
U008 Btc_PA384[] = {
 0x6e, 0x80, 0x42, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// NTSC M 480x360
U008 Btc_NM360[] = {
 0x6e, 0x08, 0x42, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 480x360
U008 Btc_PA360[] = {
 0x6e, 0x80, 0x42, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// NTSC M 400x300
U008 Btc_NM300[] = {
 0x6e, 0xac, 0xc2, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 400x300
U008 Btc_PA300[] = {
 0x6e, 0x05, 0x02, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// NTSC M 320x240
U008 Btc_NM240[] = {
 0x6e, 0xd2, 0xc2, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 320x240
U008 Btc_PA240[] = {
 0x6e, 0x60, 0x02, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// Brooktree 320x200

// NTSC M 320x200
U008 Btc_NM200[] = {
 0x6e, 0x6b, 0x02, 0x00, 0x01,
 0xff};

// PAL B,D,G,I 320x200
U008 Btc_PA200[] = {
 0x6e, 0xd8, 0x02, 0x00, 0x01,
 0xff};

//---------------------------------------------------------------------------------------
// Brooktree I2C tables for TV modes 0..9
// Some modes are scanline and/or character doubled (e.g. 320x200 is really 640x400)
// Some modes do not have specific encoder tables; they use the next larger format
// (e.g. 512x384 uses 640x400).
// DVD native mode is the native TV resolution with overscan compensation disabled
// There are 3 sets of tables (400, 480, 600) times 6 TV formats.
//    320x200   320x240   320x400   400x300   480x360   512x384   640x400   640x480   800x600  DVD native
U008 *Bt_NM[] =    // NTSC M
    {Bt_NM400, Bt_NM480, Bt_NM400, Bt_NM600, Bt_NM400, Bt_NM400, Bt_NM400, Bt_NM480, Bt_NM600, Bt_NMDVD};

U008 *Bt_NJ[] =    // NTSC J
    {Bt_NJ400, Bt_NJ480, Bt_NJ400, Bt_NJ600, Bt_NJ400, Bt_NJ400, Bt_NJ400, Bt_NJ480, Bt_NJ600, Bt_NJDVD};

U008 *Bt_PM[] =    // PAL M
    {Bt_PM400, Bt_PM480, Bt_PM400, Bt_PM600, Bt_PM400, Bt_PM400, Bt_PM400, Bt_PM480, Bt_PM600, Bt_PMDVD};

U008 *Bt_PA[] =    // PAL BDHGI
    {Bt_PA400, Bt_PA480, Bt_PA400, Bt_PA600, Bt_PA400, Bt_PA400, Bt_PA400, Bt_PA480, Bt_PA600, Bt_PADVD};

U008 *Bt_PN[] =    // PAL N
    {Bt_PM400, Bt_PN480, Bt_PN400, Bt_PN600, Bt_PN400, Bt_PN400, Bt_PN400, Bt_PN480, Bt_PN600, Bt_PNDVD};

U008 *Bt_NC[] =   // PAL NC
    {Bt_NC400, Bt_NC480, Bt_NC400, Bt_NC600, Bt_NC400, Bt_NC400, Bt_NC400, Bt_NC480, Bt_NC600, Bt_NCDVD};

//---------------------------------------------------------------------------------------
// Brooktree Centering tables
// Centering is NTSC-like (NTSC-M, NTSC-J, PAL-M) or PAL-like (PAL-BDGHI, PAL-N, PAL-NC).
// There are 2 sets of tables for 6 TV formats.
//    320x200   320x240    320x400    400x300    480x360    512x384    640x400    640x480    800x600
U008 *Btc_NM[] =    // NTSC M
    {Btc_NM200, Btc_NM240, Btc_NM3x4, Btc_NM300, Btc_NM360, Btc_NM384, Btc_NM400, Btc_NM480, Btc_NM600, Btc_NMDVD};

U008 *Btc_NJ[] =    // NTSC J
    {Btc_NM400, Btc_NM480, Btc_NM3x4, Btc_NM300, Btc_NM360, Btc_NM384, Btc_NM400, Btc_NM480, Btc_NM600, Btc_NMDVD};

U008 *Btc_PM[] =   // PAL M
    {Btc_NM200, Btc_NM480, Btc_NM3x4, Btc_NM300, Btc_NM360, Btc_NM384, Btc_NM400, Btc_NM480, Btc_NM600, Btc_NMDVD};

U008 *Btc_PA[] =   // PAL BDHGI
    {Btc_PA200, Btc_PA240, Btc_PA3x4, Btc_PA300, Btc_PA360, Btc_PA384, Btc_PA400, Btc_PA480, Btc_PA600, Btc_PADVD};

U008 *Btc_PN[] =   // PAL N
    {Btc_PA200, Btc_PA480, Btc_PA3x4, Btc_PA300, Btc_PA360, Btc_PA384, Btc_PA400, Btc_PA480, Btc_PA600, Btc_PADVD};

U008 *Btc_NC[] =   // PAL NC
    {Btc_PA200, Btc_PA480, Btc_PA3x4, Btc_PA300, Btc_PA360, Btc_PA384, Btc_PA400, Btc_PA480, Btc_PA600, Btc_PADVD};


//---------------------------------------------------------------------------------------
// Brooktree Autoconfig tables
//
// Bit 0 indicates NTSC/PAL
// Bits 5:4 indicate overscan ratio, but not in a consistent way (640x480 standard is 0 for PAL and 1 for NTSC, and lower is the reverse).
#define BTAN_640x400    0x08    //001000b
#define BTAN_640x480    0x10    //010000b   Conexant has taken this out of the data sheet -- but with mode 0 we lose some pixels in horiz
#define BTAN_800x600    0x12    //010010b
#define BTAN_1024x768   0x0a    //001010b
#define BTAP_640x400    0x09    //001001b
#define BTAP_640x480    0x01    //000001b
#define BTAP_800x600    0x13    //010011b
#define BTAP_1024x768   0x0b    //001011b

//              640x400  640x480  800x600  1024x768
U008 BTA[] = {  BTAN_640x400, BTAN_640x480, BTAN_800x600, BTAN_1024x768,
                BTAP_640x400, BTAP_640x480, BTAP_800x600, BTAP_1024x768
};
#define BTA_LEN sizeof(BTA)/2

// H Sync
U008 BTHS0[] = { 0x1B, 0x19, 0xEB, 0x23,
                 0xAC, 0xAA, 0x35, 0xE5};
U008 BTHS1[] = { 0x02, 0x02, 0x02, 0x02,
                 0x02, 0x02, 0x02, 0x02};
// Coring & Attenuation
U008 BTCA0[] = { 0xC3, 0xC3, 0xC3, 0xC3,
                 0xC3, 0xC3, 0xC3, 0xC3 };
U008 BTCA1[] = { 0xC2, 0xC2, 0xC2, 0xC2,
                 0xC2, 0xC2, 0xC2, 0xC2 };
// V Active & Lines
U008 BTVAL[] = { 0x06, 0x06, 0x0A, 0x0F,
                 0x06, 0x06, 0x0B, 0x0F };
// Flicker Filter
U008 BTFF0[] = { 0x1B, 0x1B, 0x1B, 0x00,
                 0x12, 0x12, 0x1B, 0x00 };
U008 BTFF1[] = { 0x9B, 0x9B, 0x80, 0x80,
                 0x9B, 0x9B, 0x80, 0x80 };
U008 BTFF2[] = { 0xC0, 0xC0, 0x92, 0xF6,
                 0xC0, 0xC0, 0x92, 0xF6 };

//
//  Chrontel strings
//

//  The I2C init string
//  Reg 3D and reg 10[7:5] are Macrovision bits, and on the 7008 they must be cleared when the device
//  powers up (standby D3 cold) or else the clock will stop.
U008 CH_Init[] = { 0xea, CH_REG_04, 0x05, CH_REG_06, 0x40, CH_REG_0D, 0x07, CH_REG_0E, 0x0B, CH_REG_10, 
                   0x00, CH_REG_1B, 0x20, CH_REG_1C, 0xC0, CH_REG_3D, 0x00, 0xff };

CH_string  CH_NULL_STRING = {   0xff    };

//  The I2C disable string
U008    CH_Disable[] = { 0xea, CH_REG_0D, 0x03, CH_REG_0E, 0x09, 0xff };

//  Flicker filter, Brightness, and Contrast.
U008    CH_FBC[] = { 0xea, CH_REG_01, CH_FF, CH_REG_09, CH_BL, CH_REG_11, CH_CE, 0xff };

//
// Chrontel I2C timing tables. All resolutions are multiples of these modes.
// Reg A is horizontal position; Reg B is vertical position
// DEVICE REG 0       REG 7       REG A       REG B       REG13, AUTOINC TO REG 15
CH_string  CH_NTSC320X200 =
{   0xea, CH_REG_00, 0x49, CH_REG_07, 0x70, CH_REG_0A, 0x34, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x0b, 0x16, 0xff };  // NTSC 320x200 (640x400)
CH_string  CH_NTSC320X240 =
{   0xea, CH_REG_00, 0x6a, CH_REG_07, 0x5C, CH_REG_0A, 0x2a, CH_REG_0B, 0x04, CH_REG_13 | I2C_AUTOINC, 0x00, 0x3f, 0x7e, 0xff };  // NTSC 320x240 (640x480)
CH_string CH_NTSC400X300 =
{   0xea, CH_REG_00, 0x8d, CH_REG_07, 0x84, CH_REG_0A, 0x38, CH_REG_0B, 0xe8, CH_REG_13 | I2C_AUTOINC, 0x02, 0x59, 0x2E, 0xff };  // NTSC 400x300 (800x600)
CH_string  CH_NTSC480X360 =
{   0xea, CH_REG_00, 0x49, CH_REG_07, 0x70, CH_REG_0A, 0x34, CH_REG_0B, 0x16, CH_REG_13 | I2C_AUTOINC, 0x00, 0x0b, 0x16, 0xff };  // NTSC 480x360 (640x400) 
CH_string  CH_NTSC512X384 = 
//{   0xea, CH_REG_00, 0x09, CH_REG_07, 0xd4, CH_REG_0A, 0x0a, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x63, 0x16, 0xff };  // NTSC 512x384
{   0xea, CH_REG_00, 0x49, CH_REG_07, 0x70, CH_REG_0A, 0x48, CH_REG_0B, 0x10, CH_REG_13 | I2C_AUTOINC, 0x00, 0x0b, 0x16, 0xff };  // use NTSC 640x400 
CH_string  CH_NTSC640X400 =
{   0xea, CH_REG_00, 0x49, CH_REG_07, 0x70, CH_REG_0A, 0x34, CH_REG_0B, 0x10, CH_REG_13 | I2C_AUTOINC, 0x00, 0x0b, 0x16, 0xff };  // NTSC 640x400 mode 11
CH_string  CH_NTSC640X480 = 
{   0xea, CH_REG_00, 0x6A, CH_REG_07, 0x5e, CH_REG_0A, 0x2b, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x3F, 0x7E, 0xff };  // NTSC 640x480 mode 17
CH_string  CH_NTSC720X480 = 
//{   0xea, CH_REG_00, 0x69, CH_REG_07, 0x46, CH_REG_0A, 0x1E, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x3F, 0x6E, 0xff };
//{   0xea, CH_REG_00, 0x8C, CH_REG_07, 0x84, CH_REG_0A, 0x2D, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x13, 0x3E, 0xff };  //mode 23
{   0xea, CH_REG_00, 0x8D, CH_REG_07, 0x8E, CH_REG_0A, 0x3c, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x02, 0x59, 0x2E, 0xff };  // copy of NTSC 800x600 mode 24
CH_string  CH_NTSC720X576 = 
{   0xea, CH_REG_00, 0x8B, CH_REG_07, 0x7C, CH_REG_0A, 0x29, CH_REG_0B, 0xF4, CH_REG_13 | I2C_AUTOINC, 0x00, 0x21, 0x5E, 0xff };
CH_string  CH_NTSC800X600 = 
{   0xea, CH_REG_00, 0x8D, CH_REG_07, 0x8E, CH_REG_0A, 0x3c, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x02, 0x59, 0x2E, 0xff };  // NTSC 800x600 mode 24

// PAL values 
CH_string  CH_PAL320X200 =
{   0xea, CH_REG_00, 0x42, CH_REG_07, 0xf0, CH_REG_0A, 0x30, CH_REG_0B, 0x08, CH_REG_13 | I2C_AUTOINC, 0x00, 0x3d, 0x6c, 0xff };  // PAL 320x200 (640x400)
CH_string  CH_PAL320X240 =
{   0xea, CH_REG_00, 0x61, CH_REG_07, 0xc0, CH_REG_0A, 0x38, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x04, 0x09, 0xff };  // PAL 320x240 (640x480)
CH_string  CH_PAL400X300 = 
{   0xea, CH_REG_00, 0x83, CH_REG_07, 0x8c, CH_REG_0A, 0x40, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x21, 0x56, 0xff };  // PAL  800x600 (800x600)
CH_string  CH_PAL480X360 = 
{   0xea, CH_REG_00, 0x42, CH_REG_07, 0x5c, CH_REG_0A, 0x40, CH_REG_0B, 0x10, CH_REG_13 | I2C_AUTOINC, 0x00, 0x3d, 0x6c, 0xff };  // PAL  480x360 (640x400)
CH_string  CH_PAL512X384 = 
{   0xea, CH_REG_00, 0x42, CH_REG_07, 0xc0, CH_REG_0A, 0x20, CH_REG_0B, 0x10, CH_REG_13 | I2C_AUTOINC, 0x00, 0x3d, 0x6c, 0xff };  // PAL  512x384 (640x400)
CH_string  CH_PAL640X400 = 
{   0xea, CH_REG_00, 0x42, CH_REG_07, 0xee, CH_REG_0A, 0x30, CH_REG_0B, 0x10, CH_REG_13 | I2C_AUTOINC, 0x00, 0x3d, 0x6c, 0xff };  // PAL  640x400 100
CH_string  CH_PAL640X480 = 
{   0xea, CH_REG_00, 0x61, CH_REG_07, 0x6e, CH_REG_0A, 0x34, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x04, 0x09, 0xff };  // PAL  640x480 101
CH_string  CH_PAL720X480 = 
{   0xea, CH_REG_00, 0x83, CH_REG_07, 0x7e, CH_REG_0A, 0x40, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x21, 0x56, 0xff };  // PAL  800x600 103
//{   0xea, CH_REG_00, 0x81, CH_REG_07, 0x84, CH_REG_0A, 0x2D, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x05, 0x39, 0x87, 0xff };  // PAL  720x480
CH_string  CH_PAL720X576 = 
{   0xea, CH_REG_00, 0x81, CH_REG_07, 0x84, CH_REG_0A, 0x2D, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x05, 0x39, 0x87, 0xff };  // PAL  720x576
CH_string  CH_PAL800X600 = 
{   0xea, CH_REG_00, 0x83, CH_REG_07, 0x7e, CH_REG_0A, 0x40, CH_REG_0B, 0x00, CH_REG_13 | I2C_AUTOINC, 0x00, 0x21, 0x56, 0xff };  // PAL  800x600 103

// FSCI data (subcarrier frequency adjustment). If reg 21 bit 0 is set, the subcarrier frequency is automatically selected.
U008 CH_FSCI_640x480[] = {0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 }; // mode 17
//U008 CH_FSCI_720x480[] = {0x01, 0x0D, 0x09, 0x0C, 0x00, 0x04, 0x06, 0x0F }; // mode 23
U008 CH_FSCI_720x480[] = {0x01, 0x09, 0x08, 0x0B, 0x03, 0x0A, 0x06, 0x03 }; // mode 24
//U008 CH_FSCI_720x576[] = {0x02, 0x05, 0x02, 0x04, 0x09, 0x0C, 0x07, 0x0A }; // mode 16
U008 CH_FSCI_720x576[] = {0x01, 0x0F, 0x01, 0x0C, 0x07, 0x0A, 0x05, 0x00 }; // mode 22
U008 CH_FSCI_800x600[] = {0x01, 0x09, 0x08, 0x0B, 0x03, 0x0A, 0x06, 0x03 }; // mode 24
U008 CH_FSCI_640x480NC[] = {0x01, 0x0f, 0x00, 0x0d, 0x05, 0x04, 0x05, 0x0e }; // PAL NC 640x480
//U008 CH_FSCI_720x480NC[] = {0x02, 0x06, 0x07, 0x09, 0x08, 0x0C, 0x00, 0x0C }; // mode 19
U008 CH_FSCI_720x480NC[] = {0x01, 0x09, 0x07, 0x08, 0x0e, 0x0f, 0x03, 0x05 }; // PAL NC 800x600
U008 CH_FSCI_720x576NC[] = {0x02, 0x06, 0x07, 0x39, 0xC8, 0x0C, 0x00, 0x0C }; // ??
U008 CH_FSCI_800x600NC[] = {0x01, 0x09, 0x07, 0x08, 0x0e, 0x0f, 0x03, 0x05 }; // PAL NC 800x600

#define numFSCIbytes    sizeof CH_FSCI_640x480

//
//  NV4 TV adjustments for Chrontel
//
TV_ADJUST CHRONTEL_NTSC[] =
{
//   hRes  vRes  SR01  CR00  CR04  CR06  CR07  CR08  CR10  CR12  CR28  encoderStringPtr,          htotal vtotal hdelay
    {320,  200,  0x09, 0x2b, 0x2b, 0x0c, 0x3e, 0x00, 0xc0, 0x8f, 0x98, (U008 *)(&CH_NTSC320X200), 0x1A4, 0x20D, 0x18},
    {320,  240,  0x09, 0x2b, 0x2b, 0x3c, 0x3e, 0x40, 0xe4, 0xdf, 0x88, (U008 *)(&CH_NTSC320X240), 0x188, 0x258, 0x0d},
    {400,  300,  0x09, 0x3d, 0x3d, 0x72, 0xf0, 0x20, 0x59, 0x57, 0x80, (U008 *)(&CH_NTSC400X300), 0x000, 0x000, 0x00},
    // 480x360 and 512x384 use 640x400 encoder mode 
    {480,  360,  0x01, 0x49, 0x40, 0x20, 0xB3, 0x00, 0x90, 0x67, 0x80, (U008 *)(&CH_NTSC480X360), 0x000, 0x000, 0x00}, 
    {512,  384,  0x01, 0x5b, 0x57, 0x3c, 0x3e, 0x00, 0xd0, 0x7f, 0x80, (U008 *)(&CH_NTSC512X384), 0x000, 0x000, 0x00},
    {640,  400,  0x01, 0x5b, 0x57, 0x3c, 0x3e, 0x00, 0xd0, 0x8f, 0x80, (U008 *)(&CH_NTSC640X400), 0x348, 0x20D, 0x03},
    {640,  480,  0x01, 0x5f, 0x5c, 0x3c, 0x3e, 0x00, 0xe4, 0xdf, 0x80, (U008 *)(&CH_NTSC640X480), 0x310, 0x258, 0x0d},
    {720,  480,  0x01, 0x68, 0x6f, 0x80, 0xf0, 0x00, 0x59, 0x57, 0x80, (U008 *)(&CH_NTSC720X480), 0x428, 0x2EE, 0x00}, //correct?
    {720,  576,  0x01, 0x65, 0x6f, 0x92, 0xf0, 0x00, 0x50, 0x57, 0x80, (U008 *)(&CH_NTSC720X576), 0x410, 0x276, 0x00},
    {800,  600,  0x01, 0x76, 0x6d, 0xb8, 0xf0, 0x00, 0x59, 0x57, 0x80, (U008 *)(&CH_NTSC800X600), 0x428, 0x2EE, 0x0d},
};
//
TV_ADJUST  CHRONTEL_PAL[] =
{
//   hRes  vRes  SR01  CR00  CR04  CR06  CR07  CR08  CR10  CR12  CR28  encoderStringPtr,         htotal vtotal hdelay
    {320,  200,  0x09, 0x2e, 0x29, 0x0c, 0x3e, 0x00, 0xc0, 0x8f, 0x98, (U008 *)(&CH_PAL320X200), 0x1F4, 0x1F4, 0x06},
    {320,  240,  0x09, 0x2c, 0x2a, 0x3c, 0x3e, 0x00, 0xe4, 0xdf, 0x88, (U008 *)(&CH_PAL320X240), 0x1A4, 0x271, 0x08},
    {400,  300,  0x09, 0x3c, 0x3c, 0xb0, 0xf0, 0x00, 0x57, 0x57, 0x80, (U008 *)(&CH_PAL400X300), 0x000, 0x000, 0x00},
    // 480x360 and 512x384 use 640x400 encoder mode
    {480,  360,  0x01, 0x49, 0x48, 0xf0, 0xB2, 0x00, 0x90, 0x67, 0x80, (U008 *)(&CH_PAL480X360), 0x000, 0x000, 0x00},
    {512,  384,  0x01, 0x75, 0x57, 0x0e, 0x3e, 0x00, 0xc0, 0x7f, 0x80, (U008 *)(&CH_PAL512X384), 0x000, 0x000, 0x00},
    {640,  400,  0x01, 0x6a, 0x56, 0x0C, 0x3e, 0x00, 0xc0, 0x8f, 0x80, (U008 *)(&CH_PAL640X400), 0x3E8, 0x1F4, 0x05},
    {640,  480,  0x01, 0x5a, 0x56, 0x3c, 0x3e, 0x00, 0xe4, 0xdf, 0x80, (U008 *)(&CH_PAL640X480), 0x348, 0x271, 0x05},
    {720,  480,  0x01, 0x74, 0x6a, 0xe0, 0xf0, 0x00, 0x59, 0x57, 0x80, (U008 *)(&CH_PAL720X480), 0x3C0, 0x2EE, 0x05},  //copied from 720x576
//    {720,  576,  0x01, 0x6f, 0x6a, 0xb8, 0xf0, 0x00, 0x59, 0x57, 0x80, (U008 *)(&CH_PAL720X576) },
//    {720,  576,  0x01, 0x65, 0x6f, 0x92, 0xf0, 0x00, 0x50, 0x57, 0x80, (U008 *)(&CH_PAL720X576) },
    {720,  576,  0x01, 0x6f, 0x6a, 0x70, 0xf0, 0x00, 0x48, 0x57, 0x80, (U008 *)(&CH_PAL720X576), 0x3B0, 0x271, 0x00},
    {800,  600,  0x01, 0x70, 0x6a, 0xb8, 0xf0, 0x00, 0x59, 0x57, 0x80, (U008 *)(&CH_PAL800X600), 0x3C0, 0x2EE, 0x05},
};
    
//
//  adjustments for Brooktree 
//
// New method eliminates most of the table entries. Only VT needs to be table driven.
// Now we can use normal CRTC values, except HRS = HDE + 1, HT = HRS + 1.
// There are some exceptions to this (see programTV()).
U008    VT_BT_NTSC[] =
   // 320x200,320x240,320x400,400x300,480x360,512x384,640x400,640x480,800x600, DVD
    { 0xd3,   0x32,   0xc8,   0xac,   0xe4,   0x39,   0xca,   0x34,   0xac,    0x0D}; 
U008    VT_BT_PAL[] =
    { 0xf5,   0x42,   0xcf,   0xb6,   0xe8,   0x5a,   0xf4,   0x43,   0xb6,    0x54};


/*************************** Helper functions *******************************/

// Read register, mask off bits, or with new value
// Write Mask is positive mask of data (e.g. use 0x14 to alter bits 4 and 2)
U008 dacTVReadModifyWrite
(
    PHWINFO pDev, 
    U032    Head,
    U008    Reg,
    U008    WriteData,
    U008    WriteMask
)
{
U008 ReadData, ack;

     // Read register
     i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
     ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
     ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) Reg); 
     i2cStart(pDev, Head, pDev->Dac.TVOutPortID); // Start w/o Stop = Restart
     ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
     i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &ReadData, 1);
     i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
     
     // Combine data and write
     WriteData &= WriteMask;   // just to be safe
     WriteData |= (ReadData & ~WriteMask);
     
     i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
     ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
     ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) Reg); 
     ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) WriteData); 
     i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
     
     return ReadData;
}

//  Read encoder register
//  This will work on Chrontel, Philips, and Conexant 870/871, but not 868/869
U008 dacTVReadReg
(
    PHWINFO pDev,
    U032    Head,
    U008    Reg
)
{
    U008 read;

    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID));     
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) Reg);                        // subaddress
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID+1));     
    i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read, 1);
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    return read;
}
 
static U032 dacGetTVAdjustTableArray
(
    PHWINFO pDev,
    U032 Head,
    TV_ADJUST **adjustTablePtr
)
{
    U032  numEntries;

    numEntries = 0;
    if (TV_ENCODER_FAMILY(pDev->Dac.EncoderType) == TV_ENCODER_CHRONTEL)
    {
        // Chrontel encoder.
        switch (pDev->Dac.TVStandard)
        {
            // modes with NTSC CRTC timing
            case NTSC_M:
            case NTSC_J:
            case PAL_M:
                *adjustTablePtr = (TV_ADJUST *)&CHRONTEL_NTSC;
                numEntries = sizeof(CHRONTEL_NTSC) / sizeof(TV_ADJUST);
                break;
            // modes with PAL CRTC timing
            case PAL_A:
            case PAL_N:
            case PAL_NC:
                *adjustTablePtr = (TV_ADJUST *)&CHRONTEL_PAL;
                numEntries = sizeof(CHRONTEL_PAL) / sizeof(TV_ADJUST);
                break;
        }
    }
#if 0   // this style table lookup not used
    else
    {
        // Brooktree encoder.
        if (pDev->Dac.TVStandard == NTSC)
        {
            // NTSC 
            *adjustTablePtr = (TV_ADJUST *)&BROOKTREE_NTSC;
            numEntries = sizeof(BROOKTREE_NTSC) / sizeof(TV_ADJUST);
        }
        else
        {
            // PAL
            *adjustTablePtr = (TV_ADJUST *)&BROOKTREE_PAL;
            numEntries = sizeof(BROOKTREE_PAL) / sizeof(TV_ADJUST);
        }
    }
#endif

    return numEntries;
} // end of dacGetTVAdjustTableArray

static RM_STATUS dacGetI2CModeString
(
    PHWINFO pDev, 
    U032 Head,
    U032 hRes, 
    U032 vRes,
    U008 **encoderStringPtr,
    U032 *hTotalPM, //(optional) pseudo-master mode param
    U032 *vTotalPM, //(optional) pseudo-master mode param
    U008 *hDelayPM  //(optional) pseudo-master mode param
)
{
    U032 i, numEntries;
    TV_ADJUST * adjustTable;

    numEntries = dacGetTVAdjustTableArray(pDev, Head, &adjustTable);
    for (i = 0; i < numEntries; i++)
    {
        if (adjustTable[i].hRes == hRes &&
            adjustTable[i].vRes == vRes)
        {
            // found a match.
            *encoderStringPtr = adjustTable[i].encoderString;
            if (hTotalPM) *hTotalPM = adjustTable[i].hTotalPM;
            if (vTotalPM) *vTotalPM = adjustTable[i].vTotalPM;
            if (hDelayPM) *hDelayPM = adjustTable[i].hDelayPM;

            return RM_OK;
        }
    }

    // did not find a match.
    return RM_ERROR;
} // end of dacGetI2CModeString

static RM_STATUS dacGetTVAdjustTableEntry
(
    PHWINFO pDev,
    U032 Head,
    U032 hRes,
    U032 vRes,
    TV_ADJUST **adjustTableEntry
)
{
    U032 i, numEntries;
    TV_ADJUST * adjustTable;

    numEntries = dacGetTVAdjustTableArray(pDev, Head, &adjustTable);
    for (i = 0; i < numEntries; i++)
    {
        if (adjustTable[i].hRes == hRes &&
            adjustTable[i].vRes == vRes)
        {
            // found a match.
            *adjustTableEntry = &adjustTable[i];
            return RM_OK;
        }
    }

    // did not find a match.
    return RM_ERROR;
} // end of dacGetTVAdjustTableEntry

// Autoconfiguration requires writing a single register.
// The resolution, TV standard, and overscan ration is encapsulated in this mode number.
// Currently, only "standard" overscan is used. 
// Note: in order to get non-overscan modes for 640x480, we can't use autoconfig, so I've added discrete tables.
RM_STATUS dacProgramBrooktreeAuto(PHWINFO pDev, U032 Head)
{
    U032    hRes, vRes;
    U016    status = 0;
    U008    mode, config1, config2, autoConfigMode, *strptr, i, bths0, bths_override=0;
    hRes = pDev->Framebuffer.HalInfo.HorizDisplayWidth;
    vRes = pDev->Framebuffer.HalInfo.VertDisplayWidth;

    // generate an index into our mode table
    mode = 0; // not really needed but some compilers complain about uninitialized use
    
    switch (hRes) 
    {
        case 320:
            switch (vRes) 
            {
                case 200:
                    mode = 0;       // 640x400
                    bths0 = 0x10;   //table override
                    bths_override = 1;
                    break;
                case 240:
                    mode = 1;   // 640x480
                    bths_override = 1;
                    if (pDev->Dac.TVStandard == PAL_M)
                        bths0 = 0x00;
                    else
                        bths0 = 0x02;
                    break;
                default:
                    return RM_ERROR;
            }
            break;
        case 400:
            if (vRes == 300) 
                mode = 2; // 800x600
            else
                return RM_ERROR;
            break;
        case 480:
            if (vRes == 360) 
                mode = 1;   // 640x480
            else
                return RM_ERROR;
            break;
        case 512:
            if (vRes == 384) 
                mode = 3;   // 1024x768
            break;
        case 640:
            switch (vRes)
            {
                case 400:
                    mode = 0;   // 640x400
                    break;
                case 480:
                    mode = 1;
                    break;
                default:
                    return RM_ERROR;
            }
            break;
        case 800:
            if (vRes == 600) 
            {
                mode = 2;
                if (pDev->Dac.TVStandard == PAL_M)
                {
                    bths_override = 1;
                    bths0 = 0x3C;
                }
            }
            else
                return RM_ERROR;
            break;
        case 1024:
            if (vRes == 768) 
                mode = 3;
            else
                return RM_ERROR;
            break;
        default:
            return RM_ERROR;
    }
    // 640x480 defaults to non-overscan, for which there is no autoconfig. Use tables.
    if ((mode == 1) && pDev->Dac.TVOverscanDisabled)
    {
        // Set the auto-config mode 0
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CONFIG);	// subaddress
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0);	// 
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
        
        // send an init string
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        strptr = (U008 *)&BT871_Init;          // send init sequence
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        for (i=0; *strptr != 0xff; i++)
        {
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++);  
            if (status)
                break;
        }     
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         // end state for I2C
        
        switch (pDev->Dac.TVStandard)
        {
            case NTSC_M:
            case NTSC_J:
                strptr = (U008 *)&Bt_NM480x;
                break;
            case PAL_NC:
            case PAL_A:
            case PAL_M:
            case PAL_N:
                strptr = (U008 *)&Bt_PA480x;                
                break;
            default:
                break;
        }
        
        // send a timing string
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        for (i=0; *strptr != 0xff; i++)
        {
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++);
            if (status)
                break;
        }
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         // end state for I2C
        
        // Toggle TIMING_RST
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);  
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x44);  
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         // end state for I2C
        tmrDelay(pDev, 10000000);
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);  
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0xC4);  
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         // end state for I2C

    }
    else // do autoconfig
    {
        // Now adjust index for TV standard
        switch (pDev->Dac.TVStandard)
        {
            case NTSC_M:
            case NTSC_J:
                break;
            case PAL_NC:
            case PAL_A:
            case PAL_M:
            case PAL_N:
                mode += BTA_LEN;  // second row of table
                break;
            default:
                break;
        }
        autoConfigMode = BTA[mode];
        // The HW designer shoved a reserved bit into the middle of the config info.
        // I could account for this in the table, but I want my table to correspond to the values in the manual, and
        // if the reserved bit is ever used, we'll have to preserve it, so it's better to leave the table as is, and insert
        // into the config byte here.
        config1 = autoConfigMode & 0x07;
        config2 = autoConfigMode & 0x38;
        config2 <<= 1;   // put a 0 in bit 3
        autoConfigMode = config2 | config1;

        // Set the auto-config mode
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CONFIG);	// subaddress
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) autoConfigMode);	// 
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        strptr = (U008 *)&BT871_Init;          // send init sequence
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        for (i=0; *strptr != 0xff; i++)
        {
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++);  
            if (status)
                break;
        }     
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         // end state for I2C
    }
    
    
    if (!bths_override)
        bths0 = BTHS0[mode];

    // Some adjustments
    // Hsync offset + width
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_HS0);	    // subaddress
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) bths0);	        // 
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_HS1);	    // subaddress
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BTHS1[mode]);	// 
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    // Coring + attenuation
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CA0);	// subaddress
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BTCA0[mode]);	// 
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CA1);	// subaddress
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BTCA1[mode]);	// 
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    // VActive + VLines
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_VAL);	// subaddress
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BTVAL[mode]);	// 
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    // Flicker Filter
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_FF0);	// subaddress
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BTFF0[mode]);	// 
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_FF1);	// subaddress
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BTFF1[mode]);	// 
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_FF2);	// subaddress
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BTFF2[mode]);	// 
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    // Set proper clock edge (default for autoconfig is wrong)
// This causes worse problems: the encoder is sometimes not readable after setting this.
//    dacTVReadModifyWrite(pDev, Head, 0xA0, 0x80, 0x80);
    
    // Save positioning regs. These will become our defaults
    pDev->Dac.TV871_80 = (U032)dacTVReadReg(pDev, Head, 0x80);
    pDev->Dac.TV871_82 = (U032)dacTVReadReg(pDev, Head, 0x82);
    pDev->Dac.TV871_92 = (U032)dacTVReadReg(pDev, Head, 0x92);
    pDev->Dac.TV871_98 = (U032)dacTVReadReg(pDev, Head, 0x98);
    pDev->Dac.TV871_9A = (U032)dacTVReadReg(pDev, Head, 0x9a);
    pDev->Dac.TV871_C8 = (U032)dacTVReadReg(pDev, Head, 0xC8);
    pDev->Dac.TV871_CA = (U032)dacTVReadReg(pDev, Head, 0xCA);
    pDev->Dac.TV871_CC = (U032)dacTVReadReg(pDev, Head, 0xCC);
    return(RM_OK);
}

//****************************************************************************************
//  Program the Brooktree device for a display mode
static RM_STATUS dacProgramBrooktree(PHWINFO pDev, U032 Head)
{
U016    i, status;
U008    *strptr, *strptr2;
U008    Data, TVmode;

    status = RM_OK;

    // to send a string to the I2C: send the start sequence, send the string, send the stop sequence

    TVmode = dacGetTVmode(pDev, Head);
    // Turn DAC on
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
    status |= i2cSendByte(pDev, Head,  pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL);   // subaddress
    status |= i2cSendByte(pDev, Head,  pDev->Dac.TVOutPortID, (U008) BT_DAC_ON);    // DAC ON
    i2cStop(pDev, Head,  pDev->Dac.TVOutPortID);
    tmrDelay(pDev, 20000000);   // delay 20 ms to allow DAC to power up. If we don't wait, check status could give wrong input.
    
    // Use Autoconfiguration for 871
    if (pDev->Dac.EncoderType == NV_ENCODER_BROOKTREE_871)
    {
        dacProgramBrooktreeAuto(pDev, Head);

        // Set Check Status bit to tell encoder to check for TV connected.
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL);    // subaddress
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CHK_STAT);   // set check stat
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

        // Using 871 non-legacy readback, Read Monitor Status bits into Data.
        Data = dacTVReadReg(pDev, Head, 0x6);

        // Reset Check Status -- not documented, but very important (can't program encoder if not done).
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL);    // subaddress
        if (Data & BT_MONSTAT_A)   // see if A is loaded
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CHK_OFF); // set check stat off, turn on all DAC's
        else
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CHK_OFF_A_OFF); // set check stat, turn off DAC A
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    }
    else
    {
        // send an init string
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        strptr = (U008 *)&BT_Init;          // send init sequence
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        for (i=0; *strptr != 0xff; i++)
        {
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++);  
            if (status)
                break;
        }     
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         // end state for I2C

        // Get pointers to the strings to write to the I2C controller.
        // One string sets the timing for a particular resolution.
        // The other string sets the position (centering).
        switch (pDev->Dac.TVStandard)
        {
            case NTSC_M:
                strptr = (U008 *)Bt_NM[TVmode];
                strptr2 = (U008 *)Btc_NM[TVmode];
                break;
            case NTSC_J:
                strptr = (U008 *)Bt_NJ[TVmode];
                strptr2 = (U008 *)Btc_NJ[TVmode];
                break;
            case PAL_NC:
                strptr = (U008 *)Bt_NC[TVmode];
                strptr2 = (U008 *)Btc_NC[TVmode];
                break;
            case PAL_A:
                strptr = (U008 *)Bt_PA[TVmode];
                strptr2 = (U008 *)Btc_PA[TVmode];
                break;
            case PAL_M:
                strptr = (U008 *)Bt_PM[TVmode];
                strptr2 = (U008 *)Btc_PM[TVmode];
                break;
            case PAL_N:
                strptr = (U008 *)Bt_PN[TVmode];
                strptr2 = (U008 *)Btc_PN[TVmode];
                break;
            default:
                strptr = (U008 *)Bt_NM[TVmode];
                strptr2 = (U008 *)Btc_NM[TVmode];
                break;
        }
        // send a timing string
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        for (i=0; *strptr != 0xff; i++)
        {
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++);
            if (status)
                break;
        }
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         // end state for I2C

        // send a centering string
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        for (i=0; *strptr2 != 0xff; i++)
        {
            Data = *strptr2++;
            if (i == 1) // second byte is H Offset, which differs between NV4 and NV5
            {
                if(IsNV4(pDev))
                {
                    // NV4 timing 8-16 pixels different, generally
                    if (TVmode <= TV_MODE_400x300)
                        Data -= 16;
                    else
                        Data -= 8; 
                    switch (TVmode)
                    {
                        case TV_MODE_400x300:
                            if ((pDev->Dac.TVStandard != NTSC_M) && (pDev->Dac.TVStandard != NTSC_J))
                                Data = 0x31;
                            break;
                        case TV_MODE_640x480:                                           
                            if ((pDev->Dac.TVStandard != NTSC_M) && (pDev->Dac.TVStandard != NTSC_J))
                                Data = 0xa2;
                            break;
                        case TV_MODE_800x600:
                            if ((pDev->Dac.TVStandard != NTSC_M) && (pDev->Dac.TVStandard != NTSC_J))
                                Data = 0x22;
                            else
                                Data = 0xd8;
                            break;
                        case TV_MODE_320x200:
                            if ((pDev->Dac.TVStandard != NTSC_M) && (pDev->Dac.TVStandard != NTSC_J))
                                Data = 0xca;
                            break;
                    }
                }
            }
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, Data);
            if (status)
                break;
        }
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         // end state for I2C

        // Set Check Status bit to tell encoder to check for TV connected.
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL);    // subaddress
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CHK_STAT);   // set check stat
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

        // Set Estat to select status register 1
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0xC4);  // subaddress
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x41);  // set estat (estat 01 + en_out)
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

        // Read Monitor Status bits into Data
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID+1));
        i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &Data, 1);
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

        // Reset Check Status -- not documented, but very important (can't program encoder if not done).
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
        status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL);    // subaddress
        if (Data & BT_MONSTAT_A)   // see if A is loaded
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CHK_OFF); // set check stat off, turn on all DAC's
        else
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CHK_OFF_A_OFF); // set check stat, turn off DAC A
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    }

    // Read registry to decide how to output on the S-video connector--do we use straight
    // S-video, or do we output composite?
    // DAC A is always composite out. DAC B and C are S-video (chroma and luma).
    // If the user has an S-video cable attached to a TV with composite input, we want
    // to output composite on the S-video (DAC C). We try to sense whether both 
    // B and C are loaded (S-video) or only C is loaded (composite).
    // Unfortunately, we may falsely detect composite even with an S-video monitor,
    // so the registry setting is necessary to override our detection.
    // Brooktree reg CE: D7  D6  D5  D4  D3  D2  D1  D0
    //                   MUX D   MUX C   MUX  B  MUX  A 
    // MUX n:   00 = output Video 0 on DAC n
    //          01 =              1
    //          10 =              2
    //          11 =              3
    // Our default init is 18 -- DAC A = mode 0, DAC B = mode 2, DAC C = mode 1
    // DAC A is always set to composite out (mode 0), which has a value of 0.
    switch (pDev->Dac.TVoutput)
    {
        case NV_CFG_VIDEO_OUTPUT_FORMAT_AUTOSELECT:
            if ((Data & BT_MONSTAT) == BT_MONSTAT_C)    // Stat A, B, C
            {
                // Set composite out on B if DAC C only is loaded.
                // Set DAC C to output composite.
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_OUT_MUX);    // subaddress
                // mode 2 on DAC B,  mode 0 on DAC C
                status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (BT_OUT_MUX_B_COMP | BT_OUT_MUX_C_COMP));
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            }
            else
            {
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_OUT_MUX);    // subaddress
                // mode 2 on DAC B, mode 1 on DAC C
                status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (BT_OUT_MUX_B_SVID | BT_OUT_MUX_C_ALL));    
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            }
            break;
        case NV_CFG_VIDEO_OUTPUT_FORMAT_FORCE_COMPOSITE:
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_OUT_MUX);    // subaddress
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (BT_OUT_MUX_B_COMP | BT_OUT_MUX_C_COMP));   
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            break;
        case NV_CFG_VIDEO_OUTPUT_FORMAT_FORCE_SVIDEO:
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_OUT_MUX);    // subaddress
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (BT_OUT_MUX_B_SVID | BT_OUT_MUX_C_ALL));    
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            break;
        default:
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_OUT_MUX);    // subaddress
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_OUT_MUX_B_SVID | BT_OUT_MUX_C_ALL);  
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            break;
    }

    // If no head is using TV, blank image.
    for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
        if (GETDISPLAYTYPE(pDev, i) == DISPLAY_TYPE_TV)
            break;
    if (i == pDev->Dac.HalInfo.NumCrtcs)
        dacDisableTV(pDev, Head);

    return (status);
} // end of dacProgramBrooktree

//****************************************************************************************
//
//  Program the Chrontel device for a display mode
//  args:       internal TV mode
//  returns:    status word
static RM_STATUS dacProgramChrontel(PHWINFO pDev, U032 Head)
{    
    RM_STATUS status;
    U016    i;
    U008    *strptr, FsciReg, read, hDelayPM;
    U032    hRes, vRes, reg32, hTotalPM, vTotalPM;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
   
    if (pDev->Dac.EncoderType == NV_ENCODER_NONE) 
    {
        // Used to check for known Chrontel types. Checking in dacinit now, this is probably useless.
        return ERR_I2C_UNKNOWN;
    }
    status = RM_OK;
    // to send a string to the I2C: send the start sequence, send the string, send the stop sequence
    // Chrontel 7007 wants bit 7 of the register address set (on others it's a don't care).
        
    // Get resolution
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    hRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
    vRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;

    // power up
    dacEnableTV(pDev, Head);
    
    // send an init string
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                   // start state for I2C
    strptr = (U008 *)&CH_Init;     // send init sequence
    status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
    strptr++;                                  // string includes ID, skip it
    for (i=0; *strptr != 0xff; i++)
    {
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++);  
        if (status)
            break;
    }     
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                     // end state for I2C

    // get a pointer to the string to write to the I2C controller
    status = dacGetI2CModeString(pDev, Head, hRes, vRes, &strptr, &hTotalPM, &vTotalPM, &hDelayPM);
    if (status)
    {
        // could not get the encoder string for this mode.
        return ERR_I2C_MODE;
    }
            
    //the init string may have put put the 7007 into master mode.
    //but we may want pseudo-master mode (where we provide syncs, 7007 provides clock)
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_0D);
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                                        //restart
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));    //read
    i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read, 1);
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

    read |= 0x04;
    switch (pDev->Dac.EncoderType)
    {
        case NV_ENCODER_CHRONTEL_7007:
        case NV_ENCODER_CHRONTEL_7008:
            if (IsNV11(pDev) || IsNV20(pDev))
            {
                if (!pDev->Dac.TVMasterMode)
                {
                    //these sync widths apply to every mode
                    DAC_REG_WR32(NV_PRAMDAC_TV_VSYNC_START,  0x00000001, Head);
                    DAC_REG_WR32(NV_PRAMDAC_TV_VSYNC_END,    0x00000010, Head);
                    DAC_REG_WR32(NV_PRAMDAC_TV_HSYNC_START,  0x00000001, Head);
                    DAC_REG_WR32(NV_PRAMDAC_TV_HSYNC_END,    0x00000040, Head);

                    //some modes need a horizontal sync delay
                    reg32  =  DAC_REG_RD32(NV_PRAMDAC_TV_SYNC_DELAY,   Head);
                    reg32 &= ~DRF_NUM(_PRAMDAC, _TV_SYNC_DELAY, _HSYNC,  0xFFFFFFFF);
                    reg32 |=  DRF_NUM(_PRAMDAC, _TV_SYNC_DELAY, _HSYNC,  hDelayPM);
                    DAC_REG_WR32(NV_PRAMDAC_TV_SYNC_DELAY, reg32,  Head);

                    //htotal and vtotal need to match what
                    //the encoder will expect to receive.
                    reg32  =  DAC_REG_RD32(NV_PRAMDAC_TV_HTOTAL,   Head);
                    reg32 &= ~DRF_NUM(_PRAMDAC, _TV_HTOTAL, _VAL,  0xFFFFFFFF);
                    reg32 |=  DRF_NUM(_PRAMDAC, _TV_HTOTAL, _VAL,  hTotalPM);
                    DAC_REG_WR32(NV_PRAMDAC_TV_HTOTAL, reg32,  Head);

                    reg32  =  DAC_REG_RD32(NV_PRAMDAC_TV_VTOTAL,   Head);
                    reg32 &= ~DRF_NUM(_PRAMDAC, _TV_VTOTAL, _VAL,  0xFFFFFFFF);
                    reg32 |=  DRF_NUM(_PRAMDAC, _TV_VTOTAL, _VAL,  vTotalPM);
                    DAC_REG_WR32(NV_PRAMDAC_TV_VTOTAL, reg32,  Head);
                    
                    //tell Chrontel to expect syncs from the slave
                    read &= ~0x04;
                }
            }
            break;
    }

    //write modified reg 0x0D back out to hardware
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_0D);
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, read);
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

    // send a timing string
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                   // start state for I2C
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
    strptr++;                                  // string includes ID, skip it
    for (i=0; *strptr != 0xff; i++)
    {
//        DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, " ", *strptr);
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++);
        if (status)
            break;
    }
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                     // end state for I2C

    // send default flicker filter, brightness, and contrast
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                   // start state for I2C
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
    strptr = (U008 *)&CH_FBC;
    strptr++;                                  // string includes ID, skip it
    for (i=0; *strptr != 0xff; i++)
    {
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++);
        if (status)
            break;
    }
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                     // end state for I2C
    
    // Set register 21 for PAL/NTSC, and reg 3 for mode 24 (800x600 NTSC)
    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                   // start state for I2C
    i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
    
    // Reg 3 bit 7 set for mode 24 only. (Flicker filter used in 7/10 scaling modes)
    status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_03); // reg 3 
    status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x00); // 
    
    switch (pDev->Dac.TVStandard) // not supporting scaling on PAL. What happens on NTSC unscaled modes?
    {
        case NTSC_J:
        case NTSC_M:
            if ((hRes == 800) || (hRes == 720) || ((hRes == 640) && (vRes == 480)))
            {
                // 800x600 & 640x480 modes use programmed FSCI values (subcarrier frequency), others use subcarrier free-run
                // set carrier free-run bit in reg 6 and clear ACIV bit in reg 21

                // set carrier free-run bit
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_06); // reg 6
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
                i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // 
                read |= CH_CFR;                                             // set carrier freerun
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_06); // reg 6
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, read);  
                
                // Clear ACIV bit            
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_21); // reg 21
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x00); 
                
                switch (hRes)
                {
                case 800:
                    status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_03); // reg 3 
                    status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x80); // 
                    strptr = (U008 *)&CH_FSCI_800x600;
                    break;
                case 720:
                    if (vRes == 576) {
                        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_03); // reg 3 
                        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x80); // 
                
                        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_08); // reg 8
                        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x30); // 

                        strptr = (U008 *)&CH_FSCI_720x576;
                    } else {
                        strptr = (U008 *)&CH_FSCI_720x480;
                    }
                    break;
                case 640:
                default:
                    strptr = (U008 *)&CH_FSCI_640x480;
                    break;
                }

                FsciReg = CH_REG_18; // starting at reg 0x18
                for (i=0; i<numFSCIbytes; i++) 
                {
                    status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, FsciReg++); // 
                    status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++); // 
                }
            }
            else    // Other NTSC modes use subcarrier free run
            {
                // set ACIV 
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_21); // reg 21
                status = i2cSendByte(pDev, Head,  pDev->Dac.TVOutPortID, 0x01); // reg 21 bit 0 = 1 -> automatically increment subcarrier
                i2cStop(pDev, Head,  pDev->Dac.TVOutPortID);                       // end state for I2C

                // clear carrier free-run bit when in ACIV mode
                i2cStart(pDev, Head,  pDev->Dac.TVOutPortID);                      // start state for I2C
                status = i2cSendByte(pDev, Head,  pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
                status = i2cSendByte(pDev, Head,  pDev->Dac.TVOutPortID, CH_REG_06); // reg 6
                i2cStart(pDev, Head,  pDev->Dac.TVOutPortID);   // RESTART = start without previous end
                status = i2cSendByte(pDev, Head,  pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
                i2cReceiveByte(pDev, Head,  pDev->Dac.TVOutPortID, &read,1);
                i2cStop(pDev, Head,  pDev->Dac.TVOutPortID);                       // 
                read &= ~CH_CFR;                                            // clear carrier freerun
                i2cStart(pDev, Head,  pDev->Dac.TVOutPortID);                      // start state for I2C
                status = i2cSendByte(pDev, Head,  pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
                status = i2cSendByte(pDev, Head,  pDev->Dac.TVOutPortID, CH_REG_06); // reg 6
                status = i2cSendByte(pDev, Head,  pDev->Dac.TVOutPortID, read);  
            }
            break;
        case PAL_NC:
            // set carrier free-run bit
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_06); // reg 6
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
            i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // 
            read |= CH_CFR;                                             // set carrier freerun
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_06); // reg 6
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, read);  

            // Clear ACIV bit            
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_21); // reg 21
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x00); 
            switch (hRes) 
            {
                case 640:
                case 320:
                    strptr = (U008 *)&CH_FSCI_640x480NC;
                break;
                case 720:
                    if (vRes == 576) {
                        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_08); // reg 8
                        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x01); // 
                        strptr = (U008 *)&CH_FSCI_720x576NC;
                    } else {
                        strptr = (U008 *)&CH_FSCI_720x480NC;
                    }
                break;
                case 800:
                case 400:
                    strptr = (U008 *)&CH_FSCI_800x600NC;
                break;
                default:
                    strptr = (U008 *)&CH_FSCI_640x480NC;    // 512x384 will fit in 640x480
            }
            FsciReg = CH_REG_18; // starting at reg 0x18
            for (i=0; i<numFSCIbytes; i++) 
            {
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, FsciReg++); // 
                status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++); // 
            }
            break;
        default:
        {
            // set ACIV 
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_21); // reg 21
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x01); // reg 21 bit 0 = 1 -> automatically increment subcarrier
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C

            // clear carrier free-run bit when in ACIV mode
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_06); // reg 6
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
            i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // 
            read &= ~CH_CFR;                                            // clear carrier freerun
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_06); // reg 6
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, read);  
        }
    }
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                     // end state for I2C
    
    // Set gain of DAC. Read/modify/write reg 4. NTSC-M and PAL-M use one level, all others use another level.
    switch (pDev->Dac.TVStandard) 
    {
        case NTSC_M:
        case PAL_M:
        {
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_04); // reg 4
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
            i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
            read &= CH_GAIN;                                            // set GAIN bits
            read |= CH_GAIN_NTSC;
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_04); // reg 4
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, read);  
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
            break;
        }
        default:
        {
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_04); // reg 4
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
            i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
            read &= CH_GAIN;                                            // set GAIN bits
            read |= CH_GAIN_PAL;
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_04); // reg 4
            status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, read);  
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
        }
    }
    // Default TV standards (in Chrontel reg 0) is 00 (PAL) or 01 (NTSC). There are 2 bits, so 4 combinations.
    // PAL B, D, G, H, I, N, NC = 00
    // NTSC-M                   = 01
    // PAL-M                    = 10
    // NTSC-J                   = 11
    
    // Set special TV standards
    if (pDev->Dac.TVStandard == NTSC_J) 
    {
        // NTSC-J should have reg 0[4:3] = 11
        // Read/Modify/Write reg 0
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_00); // reg 0
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
        i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
        read |= CH_NTSC_J;                                          // set NTSC-J bits
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_00); // reg 0
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, read);  
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
        
        // Brightness range for NTSC-J is wider
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_09); // reg 9
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_BL_J);  
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
    
    }
    if (pDev->Dac.TVStandard == PAL_M) 
    {
        // PAL-M should have reg 0[4:3] = 10. 
        // Read/Modify/Write reg 0
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_00); // reg 0
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
        i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
        read &= 0xe7;                                               // mask bits 4:3
        read |= CH_PAL_M;                                           // set PAL_M bits
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      // start state for I2C
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_00); // reg 0
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, read);  
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
    }
    // Fix for NV15/NV20: invert encoder clock out, to give NV a little more time to sample HSYNC.
    if (IsNV15orBetter(pDev)) 
    {
        // check for registry key to override this default behavior
        if (!pDev->Dac.TVClockInversionDisabled)
            dacTVReadModifyWrite(pDev, Head, CH_REG_1B, 0x10, 0x10);
    }
    return (status);
} // end of dacProgramChrontel

//****************************************************************************************
RM_STATUS dacSendEncoderString(PHWINFO pDev, U032 Head, U008 *strptr, U016 len)
{
RM_STATUS status;
U016 i;

    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                   // start state for I2C
    status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
    for (i=0; i<len; i++)
    {
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, *strptr++);  
        if (status)
            break;
    }     
    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                     // end state for I2C
    return (status);
}

//****************************************************************************************
//
//  Program the Philips device for a display mode
//  args:       internal TV mode
//  returns:    status word
static RM_STATUS dacProgramPhilips(PHWINFO pDev, U032 Head)
{    
    RM_STATUS status;
    U008    read, tvdacs;
    U032    hRes, vRes;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    // to send a string to the I2C: send the start sequence, send the string, send the stop sequence

    // Get resolution
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    hRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
    vRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;

    // Send the init strings
    // Each string is a block of consecutive registers, some blocks are unique to the TV standard.
    status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_Gen_Init0, Ph_Gen_Init0_Len);
    status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_Gen_Init1, Ph_Gen_Init1_Len);
    status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_Gen_Init2, Ph_Gen_Init2_Len);

    switch (pDev->Dac.TVStandard)
    {
        case NTSC_M:
        case NTSC_J:
            // Do NTSC init
            status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_NTSC_Init0, Ph_NTSC_Init0_Len);
            status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_NTSC_Init1, Ph_NTSC_Init1_Len);
            // Program the resolution    
            switch (hRes)
            {
                case 320:
                case 640:
                    status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_NTSC_640, Ph_NTSC_Res_Len);
                    break;        
                case 400:
                case 800:
                    status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_NTSC_800, Ph_NTSC_Res_Len);
                    break;
                default:
                    status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_NTSC_640, Ph_NTSC_Res_Len);
            }            
            break;        
        break;

        case PAL_M:
        case PAL_A:
        case PAL_N:
        case PAL_NC:
            // Do PAL init
            status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_PAL_Init0, Ph_PAL_Init0_Len);
            status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_PAL_Init1, Ph_PAL_Init1_Len);
            // Program the resolution    
            switch (hRes)
            {
                case 320:
                case 640:
                    status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_PAL_640, Ph_PAL_Res_Len);
                    break;        
                case 400:
                case 800:
                    status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_PAL_800, Ph_PAL_Res_Len);
                    break;
                default:
                    status = dacSendEncoderString(pDev, Head, (U008 *)&Ph_PAL_640, Ph_PAL_Res_Len);
            }            
            break;
        break;
    }

    if (pDev->Dac.EncoderType == NV_ENCODER_PHILIPS_7108B)
    {
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                      
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);  
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x54); // reg 54[1:0] = clocking mode (inverted on B)
        status = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 2);    // 1 on later devices, 2 on 7108B
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                       // end state for I2C
    }

    switch (pDev->Dac.EncoderType)
    {
        case NV_ENCODER_PHILIPS_7102:
        case NV_ENCODER_PHILIPS_7108:
        case NV_ENCODER_PHILIPS_7108B:
            //read output port control
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
            status  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x2d);
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
            i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &tvdacs,1);
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

            switch (pDev->Dac.TVoutput)
            {
                case NV_CFG_VIDEO_OUTPUT_FORMAT_FORCE_COMPOSITE:
                    read = ~0x03;  //GREEN and BLUE connected
                    break;

                case NV_CFG_VIDEO_OUTPUT_FORMAT_FORCE_SVIDEO:
                    read = ~0x06;  //RED and GREEN connected
                    break;

                default:
                case NV_CFG_VIDEO_OUTPUT_FORMAT_AUTOSELECT:
                    //set monitor sense threshold
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                    status  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x1a);   // subaddress
                    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x46);   // MSM threshold
                    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    
                    //set monitor sense mode
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                    status  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x1b);   // subaddress
                    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x80);   // MSM
                    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

                    //read sense bits
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                    status  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x1b);
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART
                    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
                    i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
                    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

                    //reset monitor sense mode
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                    status  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x1b);   // subaddress
                    status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x00);   // MSM
                    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
                    break;
            }

            if (~read & 0x01)  //BLUE connected
                tvdacs |=  0x20;  //CVBSEN0=1  (CVBS out the blue DAC)

            switch (~read & 0x06)
            {
                case 0x02: //GREEN only
                    tvdacs |=  0x40;    //CVBSEN1=1
                    tvdacs &= ~0x80;    //VBSEN=0 (CVBS out the green DAC.)
                    break;

                case 0x06: //RED and GREEN
                    tvdacs &= ~0x40;    //CVBSEN1=0 (luminance out the green DAC.)
                    tvdacs |=  0x90;    //CEN=1, VBSEN=1 (chroma out the red DAC, luma out the green DAC.)
                    break;
            }

            //set output port control
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
            status  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x2d);
            status |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, tvdacs);
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            break;
    }

    return (status);
} // end of dacProgramPhilips

//*************************** Exported functions *****************************************
//
//  Program the I2C device for a display mode
//  returns:    status word
RM_STATUS dacSetupTVEncoder(PHWINFO pDev, U032 Head)   
{
    RM_STATUS status;

    pDev->Dac.DevicesEnabled |= DAC_TV_ENABLED;
    // Identify which part: Chrontel or Brooktree?
    // ID was done at dacInit.
    switch (TV_ENCODER_FAMILY(pDev->Dac.EncoderType))
    {
        case TV_ENCODER_NONE:
            status = ERR_I2C_UNKNOWN;
            break;                        
        case TV_ENCODER_BROOKTREE:
            i2cInit(pDev, Head, pDev->Dac.TVOutPortID);
            status = dacProgramBrooktree(pDev, Head);
            break;
        case TV_ENCODER_CHRONTEL:
            i2cInit(pDev, Head, pDev->Dac.TVOutPortID);
            status = dacProgramChrontel(pDev, Head);
            break;                        
        case TV_ENCODER_PHILIPS:
            i2cInit(pDev, Head, pDev->Dac.TVOutPortID);
            status = dacProgramPhilips(pDev, Head);
            break;                        
        default:
            status = ERR_I2C_UNKNOWN;
            break;                        
    }
    return (status);
}

RM_STATUS dacAdjustCRTC_CH(PHWINFO pDev, U032 Head)
{
    RM_STATUS status;
    U008    data, hbeAdjust;
    U016    data16;
    TV_ADJUST * adjustEntry;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032    hRes, vRes;
    
    // Get resolution
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    hRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
    vRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;

    // Look up the array of CRTC adjustments for Chrontel
    status = dacGetTVAdjustTableEntry(pDev,Head,
                                      hRes,
                                      vRes,
                                      &adjustEntry);
    if (status)
    {
        // did not find a match.
        return status;
    }
    switch (pDev->Dac.TVStandard)
    {
        case NTSC_M:
        case NTSC_J:
            if ((hRes == 720) && (vRes == 576)) {
                CRTC_WR(NV_CIO_CR_HDT_INDEX, adjustEntry->CR00, Head);
                CRTC_WR(NV_CIO_CR_VRS_INDEX, adjustEntry->CR10, Head);
                break;
            }
            //else, fall through to next section
        case PAL_M:
            if ((hRes == 720) && (vRes == 480)) {
                CRTC_WR(NV_CIO_CR_HDT_INDEX, adjustEntry->CR00, Head);
                CRTC_WR(NV_CIO_CR_VDT_INDEX, adjustEntry->CR06, Head);
            }
            else
            {
                CRTC_WR(NV_CIO_CR_HDT_INDEX, adjustEntry->CR00, Head);
                CRTC_WR(NV_CIO_CR_HRS_INDEX, adjustEntry->CR04, Head);
                CRTC_WR(NV_CIO_CR_VDT_INDEX, adjustEntry->CR06, Head);
                CRTC_WR(NV_CIO_CR_OVL_INDEX, adjustEntry->CR07, Head);
                CRTC_WR(NV_CIO_CR_RSAL_INDEX, adjustEntry->CR08, Head);
                CRTC_WR(NV_CIO_CR_VRS_INDEX, adjustEntry->CR10, Head);
                CRTC_WR(NV_CIO_CR_VDE_INDEX, adjustEntry->CR12, Head);
                if (IsNV4(pDev))    // NV4 timing is slightly different
                {
                    if (vRes == 240) 
                        CRTC_WR(NV_CIO_CR_RSAL_INDEX, 0x60, Head);
                    if  (vRes == 300)
                        CRTC_WR(NV_CIO_CR_RSAL_INDEX, 0x40, Head);
                }
                CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data, Head);      // read pixel format
                data |= adjustEntry->CR28;        // OR with horizontal pixel adjust and slave TV to encoder
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data, Head);
                // write Seq 01
                WriteIndexed(NV_PRMVIO_SRX, (U016) ((adjustEntry->SR01 << 8) | NV_PRMVIO_SR_CLOCK_INDEX));         
            }
            break;

        case PAL_A:
        case PAL_N:
        case PAL_NC:
            CRTC_WR(NV_CIO_CR_HDT_INDEX, adjustEntry->CR00, Head);
            CRTC_WR(NV_CIO_CR_HRS_INDEX, adjustEntry->CR04, Head);
            CRTC_WR(NV_CIO_CR_VDT_INDEX, adjustEntry->CR06, Head);
            CRTC_WR(NV_CIO_CR_OVL_INDEX, adjustEntry->CR07, Head);
            CRTC_WR(NV_CIO_CR_RSAL_INDEX, adjustEntry->CR08, Head);
            CRTC_WR(NV_CIO_CR_VRS_INDEX, adjustEntry->CR10, Head);
            CRTC_WR(NV_CIO_CR_VDE_INDEX, adjustEntry->CR12, Head);
            if (IsNV4(pDev))    // NV4 timing is slightly different
            {
                if ((vRes == 200) ||
                    (vRes == 240) ||
                    (vRes == 300) ||
                    (vRes == 384))
                    CRTC_WR(NV_CIO_CR_RSAL_INDEX, 0x20, Head);
                if (vRes == 480)
                    CRTC_WR(NV_CIO_CR_HRS_INDEX, adjustEntry->CR04 + 1, Head);
            }
            CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data, Head);      // read pixel format
            data |= adjustEntry->CR28;           // OR with horizontal pixel adjust and slave TV to encoder
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data, Head);

            // write Seq 01
            WriteIndexed(NV_PRMVIO_SRX, (U016) ((adjustEntry->SR01 << 8) | NV_PRMVIO_SR_CLOCK_INDEX));         
            break;
    }
    if (hRes == 320)
    {
        // adjust HBE = HT. This lets the cursor move across the whole screen.
        CRTC_RD(NV_CIO_CR_HDT_INDEX, data16, Head);           // get HT (CR0)
        CRTC_RD(NV_CIO_CR_HBE_INDEX, data, Head);             // CR03
        data &= ~0x1f;                                  // lose the HBE bits
        data |= (U008) data16 & 0x1F;
        CRTC_WR(NV_CIO_CR_HBE_INDEX, data, Head);             // HBE 4:0
        pDev->Framebuffer.CursorAdjust = 8;
    }
    else
    {
        if (hRes == 400)
        {
            // 400x300
            CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data, Head);          // get CR28
            data &= ~0x38;
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data | (5<<3), Head); // bits[5:3] shift 5 pixels right        
            pDev->Framebuffer.CursorAdjust = 0;
            if (pDev->Dac.TVStandard == NTSC)   // PAL 400x300 requires special case for HBE for cursor position
                hbeAdjust = 4;
            else                
                hbeAdjust = 2;
        }
        else
        {
            pDev->Framebuffer.CursorAdjust = 5;
            hbeAdjust = 4;
        }
        // adjust HBE = HT + 4. This lets the cursor move across the whole screen.
        CRTC_RD(NV_CIO_CR_HDT_INDEX, data16, Head);           // get HT (CR0)
        CRTC_RD(NV_CIO_CRE_HEB__INDEX, data, Head);           // MS bit HT (CR2D)            
        if (data & 1)
            data16 |= 0x100;
        data16 += hbeAdjust;                            // HT effective
    
        CRTC_RD(NV_CIO_CR_HBE_INDEX, data, Head);             // CR03
        data &= ~0x1f;                                  // lose the HBE bits
        data |= (U008) data16 & 0x1F;
        CRTC_WR(NV_CIO_CR_HBE_INDEX, data, Head);             // HBE 4:0

        CRTC_RD(NV_CIO_CR_HRE_INDEX, data, Head);             // CR05
        data &= ~0x80;                                  // lose the HBE bit
        if (data16 & 0x20)
            data |= 0x80;
        CRTC_WR(NV_CIO_CR_HRE_INDEX, data, Head);             // HBE 5
                            
        CRTC_RD(NV_CIO_CRE_LSR_INDEX, data, Head);            // CR25
        data &= ~0x10;
        if (data16 & 0x40)
            data |= 0x10;
        CRTC_WR(NV_CIO_CRE_LSR_INDEX, data, Head);            // HBE 6
    }
    // adjust normally scanline doubled modes which are not doubled for TV
    // (512x384 and 480x360)
    if (hRes == 512 ||   // 512x384
        hRes == 480)     // 480x360
    {
        CRTC_WR(NV_CIO_CR_CELL_HT_INDEX, 0x40, Head);
    }
    if (!IsNV4(pDev))
        pDev->Framebuffer.CursorAdjust = 0; // cursor adjust not needed 

    return status;
}

RM_STATUS dacAdjustCRTC_BT(PHWINFO pDev, U032 Head)
{
RM_STATUS status = RM_OK;
U008    tvmode, data;
U016    data16;

        // Use the "TV mode number" lookup method
        tvmode = dacGetTVmode(pDev, Head);

        // adjust normally scanline doubled modes which are not doubled for TV
        if ((tvmode == TV_MODE_512x384) | (tvmode == TV_MODE_480x360))
            CRTC_WR(NV_CIO_CR_CELL_HT_INDEX, 0x40, Head);

        if (tvmode <= TV_MODE_400x300)
            WriteIndexed(NV_PRMVIO_SRX, 0x901); // dot clock /2 for low res modes

        // Horizontal adjustment is very simple: HT = HDE + 2, HRS = HDE + 1
        CRTC_RD(NV_CIO_CR_HDE_INDEX, data, Head);           // get HDE
        CRTC_WR(NV_CIO_CR_HRS_INDEX, data + 1, Head);       // write HRS
        if (tvmode == TV_MODE_400x300)  // 400x300 is a special case
            data += 1;
        else
            data += 2;
        CRTC_WR(NV_CIO_CR_HDT_INDEX, data, Head);   // write HT=DE+2
        
        // NV10 makes us do some more special casing. The reason is that the video overlay
        // syncs to the CRTC differntly. Instead of using display end, it syncs off the blank
        // signal, so we have to make an adjustment to HBE.
        // We'll also add offsets via PVIDEO_POINT_OUT (see class63 for NV10).
        if (IsNV10orBetter(pDev)) 
        {
            if (tvmode == TV_MODE_640x480)
            {
                CRTC_WR(NV_CIO_CR_HBE_INDEX, 0x95, Head);       // write HBE
                CRTC_WR(NV_CIO_CR_HRE_INDEX, 0x1F, Head);       // write HBE msb
            }
            if (tvmode == TV_MODE_800x600)
            {
                CRTC_WR(NV_CIO_CR_HBE_INDEX, 0x89, Head);       // write HBE
                CRTC_WR(NV_CIO_CR_HRE_INDEX, 0x9A, Head);       // write HBE msb
            }
        }
        // Vertical total must be adjusted per resolution
        switch (pDev->Dac.TVStandard)
        {
            case NTSC_M:
            case NTSC_J:
            case PAL_M:
                CRTC_WR(NV_CIO_CR_VDT_INDEX, VT_BT_NTSC[ tvmode ], Head);
                break;              
            case PAL_A:
            case PAL_N:
            case PAL_NC:
                CRTC_WR(NV_CIO_CR_VDT_INDEX, VT_BT_PAL[ tvmode ], Head);
                break;
            default:
                CRTC_WR(NV_CIO_CR_VDT_INDEX, VT_BT_NTSC[ 0 ], Head);    // default to NTSC_M
        }
        if (pDev->Dac.EncoderType == NV_ENCODER_BROOKTREE_871)
        {
            // Adjust CRTC vertical -- adjustment is dependent on mode (Conexant mode, i.e. overscan ration, as well as resolution)
            if (tvmode == TV_MODE_640x480)
            {
//                CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x18, Head);   
                CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x41, Head);     // normal 
//                CRTC_WR(NV_CIO_CR_VRS_INDEX, 0xF6, Head);     
            }
            if (tvmode == TV_MODE_800x600)
            {
                switch (pDev->Dac.TVStandard)
                {
                    case PAL_NC:
                    case PAL_A:
                    case PAL_N:
                        CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x64, Head);
                        CRTC_WR(NV_CIO_CR_HDE_INDEX, 0x64, Head);
                        CRTC_WR(NV_CIO_CR_HBS_INDEX, 0x65, Head);
                        CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x65, Head);
                        CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xC5, Head);
                        break;

                    case PAL_M:     //525,60Hz PAL
                        CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x62, Head);
                        CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xC4, Head);
                        break;

                    case NTSC_M:
                    case NTSC_J:
                        // Adjust CRTC vertical
                        CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xB5, Head);
                        break;              
                }
            }
            if (tvmode == TV_MODE_1024x768)
            {
                CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x83, Head);   
                // Adjust horizontal skew
                CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data, Head);          // get CR28
                data &= ~0x38;
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data | (4<<3), Head); // bits[5:3] shift 4 pixels right        
            }
            if (IsNV15orBetter(pDev))   //RYAN@20010420, this might apply to older, too... but I don't know.
            {
                if (tvmode == TV_MODE_640x400)
                {
                    switch (pDev->Dac.TVStandard)
                    {
                        case PAL_M:     //525,60Hz PAL
                        case NTSC_M:
                        case NTSC_J:
                            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x62, Head);
                            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xE6, Head);
                            break;              
                    }
                }
                if (tvmode == TV_MODE_320x200)
                {
                    switch (pDev->Dac.TVStandard)
                    {
                        case PAL_M:     //525,60Hz PAL
                        case NTSC_M:
                        case NTSC_J:
                            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x2C, Head);
                            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xE5, Head);
                            break;              
                    }
                }
                if (tvmode == TV_MODE_320x240)
                {
                    //RYAN@TVOUT, it doesn't seem possible to dial these modes in perfectly.
                    //perhaps if we switch to pseudo-master mode...
                    switch (pDev->Dac.TVStandard)
                    {
                        case PAL_A:
                        case PAL_NC:
                        case PAL_N:
                            //add more front porch to the total horizontal time
                            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x3E, Head);   //increase HTOTAL
                            CRTC_WR(NV_CIO_CR_HDE_INDEX, 0x27, Head);   //start of HBLANK
                            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x38, Head);   //put sync near end of line
                            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x41, Head);
                            break;

                        case PAL_M:     //525,60Hz PAL
                            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x2E, Head);
                            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x42, Head);
                            break;

                        case NTSC_M:
                        case NTSC_J:
                            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x25, Head);
                            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x41, Head);
                            break;
                    }
                }
            }
        }
        if (tvmode == TV_MODE_480x360)  // a special case
        {
            WriteIndexed(NV_PRMCIO_CRX__COLOR, 0xe806);     // VT
            WriteIndexed(NV_PRMCIO_CRX__COLOR, 0xb207);     // extra bits
            WriteIndexed(NV_PRMCIO_CRX__COLOR, 0x9010);     // VRS
            WriteIndexed(NV_PRMCIO_CRX__COLOR, 0x6712);     // VDE
        }
        if (tvmode == TV_MODE_512x384)  // another special case
        {
            // On a monitor, the vertical is 768, scan doubled.
            // On the TV, we want 384.
            WriteIndexed(NV_PRMCIO_CRX__COLOR, 0x7f12);     // VDE
            WriteIndexed(NV_PRMCIO_CRX__COLOR, 0xb707);     // extra bits
        }
        // NV4 has a hw bug which requires adjustment to the cursor position
        if (IsNV4(pDev))
        {
            // adjust HBE = HT + 4. This lets the cursor move across the whole screen.
            CRTC_RD(NV_CIO_CR_HDT_INDEX, data16, Head);           // get HT (CR0)
            CRTC_RD(NV_CIO_CRE_HEB__INDEX, data, Head);           // MS bit HT (CR2D)            
            if (data & 1)
                data16 |= 0x100;
            data16 += 4;                                    // HT effective

            CRTC_RD(NV_CIO_CR_HBE_INDEX, data, Head);             // CR03
            data &= ~0x1f;                                  // lose the HBE bits
            data |= (U008) data16 & 0x1F;
            CRTC_WR(NV_CIO_CR_HBE_INDEX, data, Head);             // HBE 4:0

            CRTC_RD(NV_CIO_CR_HRE_INDEX, data, Head);             // CR05
            data &= ~0x80;                                  // lose the HBE bit
            if (data16 & 0x20)
                data |= 0x80;
            CRTC_WR(NV_CIO_CR_HRE_INDEX, data, Head);             // HBE 5
                            
            CRTC_RD(NV_CIO_CRE_LSR_INDEX, data, Head);            // CR25
            data &= ~0x10;
            if (data16 & 0x40)
                data |= 0x10;
            CRTC_WR(NV_CIO_CRE_LSR_INDEX, data, Head);            // HBE 6
            switch (tvmode)
            {
                // modes which are shoehorned in to 640x400 need a horiz cursor adjustment
                case TV_MODE_512x384:
                case TV_MODE_480x360:
                // likewise 640 modes
                case TV_MODE_640x400:
                case TV_MODE_640x480:
                case TV_MODE_800x600:
                    pDev->Framebuffer.CursorAdjust = 5;
                    break;          
                default:
                    pDev->Framebuffer.CursorAdjust = 0;
            }
        }
        return status;
} 


void dacSetTvoutLatency(PHWINFO pDev, U032 Head, U008 latency)
{
    U008 data;

    latency &= 0x07;

    CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data, Head);
    data &= ~0x38;
    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data | (latency<<3), Head); // bits[5:3]
}


RM_STATUS dacAdjustCRTC_PH(PHWINFO pDev, U032 Head)
{
    RM_STATUS status = RM_OK;

    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032    hRes, vRes, std;

    // Get resolution
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    hRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
    vRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;

#define NV_ARCH(pDev) ((pDev->halHwInfo.pMcHalInfo->Architecture & 0x30)   >> 4) //2 bits
#define NV_IMPL(pDev) ((pDev->halHwInfo.pMcHalInfo->Implementation & 0x07) >> 0) //3 bits
#define NV_CHIP(pDev) ((NV_ARCH(pDev) << 3) | NV_IMPL(pDev))  //5 bits

#define TV_CHIPMASK 0xFE000000  /* 7  bits */
#define TV_HRESMASK 0x01FFC000  /* 11 bits */
#define TV_VRESMASK 0x00003FF8  /* 11 bits */
#define TV_STDMASK  0x00000007  /* 3  bits */

#define NV11 (((MC_ARCHITECTURE_NV10 & 0x30)>>1) | (MC_IMPLEMENTATION_NV11 & 0x07))
#define NV20 (((MC_ARCHITECTURE_NV20 & 0x30)>>1) | (MC_IMPLEMENTATION_NV20 & 0x07))

#define PDEV_TVMODE(pDev, h, v, std) ((NV_CHIP(pDev)<<27) | (((h-1)&0x7FF)<<14) | (((v-1)&0x7FF)<<3) | (std&0x07))
#define TV_MODE(nv, h, v, std)       ((nv<<27)            | (((h-1)&0x7FF)<<14) | (((v-1)&0x7FF)<<3) | (std&0x07))

    //special handling for certain low-res modes.
    switch (hRes)
    {
        case 320:
        case 400:
            WriteIndexed(NV_PRMVIO_SRX, 0x901); // dot clock /2 for low res modes
            break;
    }

    //PAL_Nc only differs from PAL_BDGHI by its color subcarrier.
    std = pDev->Dac.TVStandard;
    if (std == PAL_NC) { std = PAL_A; }
    if (std == PAL_M)  { std = NTSC_M; }

    //RYAN@TV: TODO: PAL_N tweakage (need Uruguay TV)
    //RYAN@TV: TODO: Try to compact this code a little more

    switch (PDEV_TVMODE(pDev, hRes, vRes, std))
    {
        case TV_MODE(NV11, 320, 200, PAL_N):
        case TV_MODE(NV11, 320, 200, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x30, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xD4, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0x90, Head);   // CR10
            //vertical scale is too narrow
            break;

        case TV_MODE(NV20, 320, 200, PAL_N):
        case TV_MODE(NV20, 320, 200, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x30, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xD4, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0x90, Head);   // CR10
            //vertical scale is too narrow
            //image needs moved to the left a little, but this seems to be impossible with CRTCs
            break;

        case TV_MODE(NV11, 320, 200, NTSC_M):
        case TV_MODE(NV11, 320, 200, NTSC_J):
        case TV_MODE(NV20, 320, 200, NTSC_M):
        case TV_MODE(NV20, 320, 200, NTSC_J):
            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x27, Head);   // CR0
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xDE, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0x9A, Head);   // CR10
            //image needs moved to the left a little, but this seems to be impossible with CRTCs
            break;

        case TV_MODE(NV11, 320, 240, PAL_N):
        case TV_MODE(NV11, 320, 240, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x30, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x24, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0xE0, Head);   // CR10
            break;

        case TV_MODE(NV20, 320, 240, PAL_N):
        case TV_MODE(NV20, 320, 240, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x30, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x24, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0xE0, Head);   // CR10
            //image needs moved to the left a little, but this seems to be impossible with CRTCs
            break;

        case TV_MODE(NV11, 320, 240, NTSC_M):
        case TV_MODE(NV11, 320, 240, NTSC_J):
        case TV_MODE(NV20, 320, 240, NTSC_M):
        case TV_MODE(NV20, 320, 240, NTSC_J):
            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x27, Head);   // CR0
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x2C, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0xE8, Head);   // CR10
            //image needs moved to the left a little, but this seems to be impossible with CRTCs
            break;

        case TV_MODE(NV11, 640, 400, PAL_N):
        case TV_MODE(NV11, 640, 400, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x5D, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xEF, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0x8F, Head);   // CR10
            dacSetTvoutLatency(pDev, Head, 1);
            //vertical scale is too narrow
            break;
        
        case TV_MODE(NV20, 640, 400, PAL_N):
        case TV_MODE(NV20, 640, 400, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x5E, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xEF, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0x8F, Head);   // CR10
            dacSetTvoutLatency(pDev, Head, 6);
            //vertical scale is too narrow
            break;

        case TV_MODE(NV11, 640, 400, NTSC_M):
        case TV_MODE(NV11, 640, 400, NTSC_J):
            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x54, Head);   // CR0
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x53, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xDC, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0x90, Head);   // CR10
            dacSetTvoutLatency(pDev, Head, 7);
            //vertical scale is narrow
            //horizontal scale is too wide
            break;

        case TV_MODE(NV20, 640, 400, NTSC_M):
        case TV_MODE(NV20, 640, 400, NTSC_J):
            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x54, Head);   // CR0
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x53, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xDC, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0x90, Head);   // CR10
            dacSetTvoutLatency(pDev, Head, 4);
            //vertical scale is narrow
            //horizontal scale is too wide
            break;

        case TV_MODE(NV11, 640, 480, PAL_N):
        case TV_MODE(NV11, 640, 480, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x5D, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x25, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0xDF, Head);   // CR10
            dacSetTvoutLatency(pDev, Head, 1);
            break;

        case TV_MODE(NV20, 640, 480, PAL_N):
        case TV_MODE(NV20, 640, 480, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x5E, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x25, Head);   // CR6
            CRTC_WR(NV_CIO_CR_VRS_INDEX, 0xDF, Head);   // CR10
            dacSetTvoutLatency(pDev, Head, 6);
            break;

        case TV_MODE(NV11, 640, 480, NTSC_M):
        case TV_MODE(NV11, 640, 480, NTSC_J):
            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x54, Head);   // CR0
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x53, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x2F, Head);   // CR6
            dacSetTvoutLatency(pDev, Head, 7);
            //horizontal scale is too wide
            break;

        case TV_MODE(NV20, 640, 480, NTSC_M):
        case TV_MODE(NV20, 640, 480, NTSC_J):
            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x54, Head);   // CR0
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x53, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x2F, Head);   // CR6
            dacSetTvoutLatency(pDev, Head, 4);
            //horizontal scale is too wide
            break;

        case TV_MODE(NV11, 800, 600, PAL_N):
        case TV_MODE(NV11, 800, 600, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x7D, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xA8, Head);   // CR6
            dacSetTvoutLatency(pDev, Head, 5);
            break;

        case TV_MODE(NV20, 800, 600, PAL_N):
        case TV_MODE(NV20, 800, 600, PAL_A):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x7D, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xA8, Head);   // CR6
            dacSetTvoutLatency(pDev, Head, 2);
            break;

        case TV_MODE(NV11, 800, 600, NTSC_M):
        case TV_MODE(NV11, 800, 600, NTSC_J):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x7E, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xA9, Head);   // CR6
            dacSetTvoutLatency(pDev, Head, 4);
            break;

        case TV_MODE(NV20, 800, 600, NTSC_M):
        case TV_MODE(NV20, 800, 600, NTSC_J):
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x7E, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0xA9, Head);   // CR6
            dacSetTvoutLatency(pDev, Head, 1);
            break;

        default:  //RYAN@TV, what is this for?!?
            CRTC_WR(NV_CIO_CR_HDT_INDEX, 0x54, Head);   // CR0
            CRTC_WR(NV_CIO_CR_HRS_INDEX, 0x52, Head);   // CR4
            CRTC_WR(NV_CIO_CR_VDT_INDEX, 0x30, Head);   // CR6
            break;
    }
    return status;

#undef NV11
#undef NV20
}

RM_STATUS dacAdjustCRTCForTV(PHWINFO pDev, U032 Head)
{
    U008    data, lock;
    U032    hRes, vRes;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    // Get resolution
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    hRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
    vRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;
    
    CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 0x20, Head);     // if TV, we can fix low water mark at 20.
    
    // slave the CRTC clock to the encoder
    CRTC_RD(NV_CIO_CRE_LCD__INDEX, data, Head);
    data &= ~(DRF_MASK(NV_CIO_CRE_LCD_LCD_SELECT) << DRF_SHIFT(NV_CIO_CRE_LCD_LCD_SELECT)); // don't slave to LCD
    data &= ~(DRF_MASK(NV_CIO_CRE_LCD_EXT_SELECT) << DRF_SHIFT(NV_CIO_CRE_LCD_EXT_SELECT)); // don't drive LCD ext data pins
    CRTC_WR(NV_CIO_CRE_LCD__INDEX, data, Head); // clear LCD bits
    
    // Toshiba SBIOS behaves badly: it may lock ext CRTC's on us during SMI.
    lock = UnlockCRTC(pDev, 0);
    if (lock != NV_CIO_SR_UNLOCK_RW_VALUE) 
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ext CRTC's are locked in dacAdjustCRTCForTV\r\n");
    }
    CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data, Head);
    data |= 0x80;       // slave mode
    data &= ~0x38;      // clear the pixel skew
    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data, Head);

    //  fix up CRTC timing per encoder
    //  NOTE: Brooktree and Chrontel have incompatible CRTC and encoder tables.
    //  Until this is resolved, this code is going to be ugly.
    switch (TV_ENCODER_FAMILY(pDev->Dac.EncoderType))
    {
        case TV_ENCODER_CHRONTEL:
            dacAdjustCRTC_CH(pDev, Head);
            break;
        case TV_ENCODER_BROOKTREE: 
            dacAdjustCRTC_BT(pDev, Head);
            break;
        case TV_ENCODER_PHILIPS:
            dacAdjustCRTC_PH(pDev, Head);
            break;
        default:
            return RM_ERROR;
    }        
    // Fix the video scalar vertical on NV4. Also allows full range of cursor movement.
    if(IsNV4(pDev))
    {
        // adjust VBE = VT. This adjusts the position of the video scalar window.
        CRTC_RD(NV_CIO_CR_VDT_INDEX, data, Head);             // CR6
        // 7 bits of CR16 are used
        CRTC_WR(NV_CIO_CR_VBE_INDEX, data, Head);             // CR16
    }

    if (hRes <= 400)    // if clock doubling
    {
        // Clock doubled mode
//        DAC_REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, 0x01030700, Head);   
        if (Head == 0) 
        {
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _TVCLK_RATIO, _DB2);
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VCLK_RATIO, _DB1);
        }
        else
        {
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _TVCLK_RATIO, _DB2);
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VCLK2_RATIO, _DB1);
        }
    }
    else
    {
        // Normal clock mode
//        DAC_REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, 0x10030700, Head);
        if (Head == 0) 
        {
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _TVCLK_RATIO, _DB1);
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VCLK_RATIO, _DB1);
        }
        else
        {
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _TVCLK_RATIO, _DB1);
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VCLK2_RATIO, _DB1);
        }
    }
    // Set source of PCLK and VSCLK to be TV encoder
    if (Head == 0) 
    {
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VS_PCLK_TV, _BOTH);
    }
    else
    {
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VS_PCLK2_TV, _BOTH);
    }

    switch (pDev->Dac.EncoderType)
    {
        case NV_ENCODER_CHRONTEL_7007:
        case NV_ENCODER_CHRONTEL_7008:
            if (IsNV11(pDev) || IsNV20(pDev))
            {
                if (!pDev->Dac.TVMasterMode)
                {
                    //setup the head as the pseudo-slave.  Because of a hardware bug,
                    //we have to do this to  both heads.  dacProgramChrontel()
                    //already set up the timing parameters required for this.
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TV_SETUP, _DEV_TYPE, _SLAVE, 0);
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TV_SETUP, _DEV_TYPE, _SLAVE, 1);
                    break;
                }
            }
            //intentional fall-through to default

        default:
            // setup the appropriate head as the master.
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TV_SETUP, _DEV_TYPE, _MASTER, Head);
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TV_SETUP, _DEV_TYPE, _MASTER, Head^1);
    }

    return RM_OK;
} // end of dacAdjustCRTCForTV

// Chrontel and Brooktree tables are incompatible
// Until we have a uniform way to deal with this, we'll use the "mode number"
// method for Brooktree (ala release 2) and the table lookup for Chrontel.
// This function is for Brooktree only.
U008 dacGetTVmode(PHWINFO pDev, U032 Head)
{
U032    hRes, vRes;
U008    TVmode;
PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

        // Get resolution
        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
        hRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
        vRes = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;
        
        // Assign a mode number based on the resolution
        switch (hRes)
        {
            case 320:
                if (vRes == 200)
                    TVmode = TV_MODE_320x200;
                else
                    TVmode = TV_MODE_320x240;
                break;
            case 400:
                TVmode = TV_MODE_400x300;
                break;
            case 480:
                TVmode = TV_MODE_480x360;
                break;
            case 512:
                TVmode = TV_MODE_512x384;
                break;
            case 640:
                if (vRes == 400)
                    TVmode = TV_MODE_640x400;
                else
                    TVmode = TV_MODE_640x480;
                break;
            case 800:
                TVmode = TV_MODE_800x600;
                break;
            case 720:
            case 704:
                TVmode = TV_MODE_DVD;
                break;
            case 1024:
                TVmode = TV_MODE_1024x768;
                break;
            default:
                TVmode = TV_MODE_640x480;
                break;
        }
        return TVmode;
}

//  Return the default register values for position, brightness, contrast and flicker filter.
VOID dacGetDefaultTVPosition
(
    PHWINFO pDev,
    U032    Head,
    U032    EncoderId,
    U008    Default,
    U032    *encoderParms
)
{
    U008    Cr07, Cr25;
    U032    Hrs, Hre, Vrs, Vre;
    U008    TVmode;
    U032    hRes, vRes;
    U008   *strptr;
    RM_STATUS status;
        
    if (EncoderId & TV_ENCODER_BROOKTREE)
    {
        PARAMS_BROOKTREE *regsBt;
        BT_MODE_REGS *pReg;
        BT_INIT_REGS *pRegI;

        // Get default parameters from tables
        TVmode = dacGetTVmode(pDev, Head);
        regsBt = (PARAMS_BROOKTREE *)encoderParms;

        if (pDev->Dac.EncoderType == NV_ENCODER_BROOKTREE_871)
        {
            pRegI = (BT_INIT_REGS *) &BT871_Init;
            regsBt->reg0xC8 = (U008)pDev->Dac.TV871_C8;
            regsBt->reg0xCA = (U008)pDev->Dac.TV871_CA;
            regsBt->reg0xCC = (U008)pDev->Dac.TV871_CC;
            
            regsBt->reg0x80 = (U008)pDev->Dac.TV871_80;
            regsBt->reg0x82 = (U008)pDev->Dac.TV871_82;
            regsBt->reg0x92 = (U008)pDev->Dac.TV871_92;
            regsBt->reg0x98 = (U008)pDev->Dac.TV871_98;
            regsBt->reg0x9A = (U008)pDev->Dac.TV871_9A;
        }
        else
        {
            pRegI = (BT_INIT_REGS *) &BT_Init;
            regsBt->reg0xC8 = pRegI->reg0xC8;
            regsBt->reg0xCA = pRegI->reg0xCA;
            regsBt->reg0xCC = pRegI->reg0xCC;
            switch (pDev->Dac.TVStandard)
            {
                case NTSC_M:
                    strptr = (U008 *)Bt_NM[TVmode];
                    break;
                case NTSC_J:
                    strptr = (U008 *)Bt_NJ[TVmode];
                    break;
                case PAL_NC:
                    strptr = (U008 *)Bt_NC[TVmode];
                    break;
                case PAL_A:
                    strptr = (U008 *)Bt_PA[TVmode];
                    break;
                case PAL_M:
                    strptr = (U008 *)Bt_PM[TVmode];
                    break;
                case PAL_N:
                    strptr = (U008 *)Bt_PN[TVmode];
                    break;
                default:
                    strptr = (U008 *)Bt_NM[TVmode];
                break;
            }
            pReg = (BT_MODE_REGS *) strptr;
            regsBt->reg0x80 = pReg->reg0x80;
            regsBt->reg0x82 = pReg->reg0x82;
            regsBt->reg0x92 = pReg->reg0x92;
            regsBt->reg0x98 = pReg->reg0x98;
            regsBt->reg0x9A = pReg->reg0x9A;
        }

        // Construct Horizontal Retrace Start, End, and Vertical Retrace Start, End
        CRTC_RD(NV_CIO_CR_OVL_INDEX, Cr07, Head);
        CRTC_RD(NV_CIO_CRE_LSR_INDEX, Cr25, Head);

        // Construct Hrs
        CRTC_RD(NV_CIO_CR_HRS_INDEX, Hrs, Head);

        // Construct Hre
        CRTC_RD(NV_CIO_CR_HRE_INDEX, Hre, Head);
        Hre = Hrs + ( ((short)Hre - (short)Hrs) & BITS0_4 );

        // Construct Vrs
        CRTC_RD(NV_CIO_CR_VRS_INDEX, Vrs, Head);
        if (Cr25 & BIT2)
            Vrs |= BIT10;
        if (Cr07 & BIT7)
            Vrs |= BIT9;
        if (Cr07 & BIT2)
            Vrs |= BIT8;

        // Construct Vre
        CRTC_RD(NV_CIO_CR_VRE_INDEX, Vre, Head);
        Vre = Vrs + ( ((short)Vre - (short)Vrs) & BITS0_3 );

        regsBt->HRetraceStart = Hrs;        
        regsBt->HRetraceEnd = Hre;      
        regsBt->VRetraceStart = Vrs;        
        regsBt->VRetraceEnd = Vre;      
    }
    if (EncoderId & TV_ENCODER_CHRONTEL)
    {
        PARAMS_CHRONTEL *regsCh;
        PCH_string  pReg;

        regsCh = (PARAMS_CHRONTEL *)encoderParms;

        hRes = pDev->Framebuffer.HalInfo.HorizDisplayWidth;
        vRes = pDev->Framebuffer.HalInfo.VertDisplayWidth;
        status = dacGetI2CModeString(pDev, Head, hRes, vRes, &strptr, NULL, NULL, NULL);
        pReg = (PCH_string) strptr;
        if (status)
        {
            regsCh->reg0x0A = 0;
            regsCh->reg0x0B = 0;
        }
        else
        {
            regsCh->reg0x0A = pReg->data0x0A;
            regsCh->reg0x0B = pReg->data0x0B;
        }
        regsCh->reg0x08 = 0x00;
        regsCh->reg0x09 = 0x7f;
        regsCh->reg0x11 = 0x03;
        if (EncoderId == NV_ENCODER_CHRONTEL_7003)
            regsCh->reg0x01 = 0x03;
        else
            regsCh->reg0x01 = 0x29;
    }
}
//  Write TV encoder positioning registers, as well as contrast, brightness, flicker filter
//  For Brooktree positioning, we must change the CRTC's also.
VOID dacSetTVPosition
(
    PHWINFO pDev,
    U032    Head,
    U032    *encoderParms
)
{
    U008 Adr, Data, Device, lock;
    NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS *Parms = (NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS *) encoderParms;

    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);
    if (TV_ENCODER_FAMILY(Parms->Encoder_ID) == TV_ENCODER_BROOKTREE)
    {
        // Write CRTC values (we can use the monitor position function)
        dacSetMonitorPosition(pDev, Head, Parms->u.regsBt.HRetraceStart, Parms->u.regsBt.HRetraceEnd,
                                Parms->u.regsBt.VRetraceStart, Parms->u.regsBt.VRetraceEnd);

        Data = Parms->u.regsBt.reg0x80;
        Adr = 0x80;
        Device = (U008)(pDev->Dac.EncoderID >> 1);
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsBt.reg0x82;
        Adr = 0x82;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsBt.reg0x92;
        Adr = 0x92;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsBt.reg0x98;
        Adr = 0x98;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsBt.reg0x9A;
        Adr = 0x9a;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsBt.reg0xC8;
        Adr = 0xc8;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsBt.reg0xCA;
        Adr = 0xca;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsBt.reg0xCC;
        Adr = 0xcc;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
    } else if (TV_ENCODER_FAMILY(Parms->Encoder_ID) == TV_ENCODER_CHRONTEL)
    {
        // Chrontel is positioned completely thru the encoder
        Data = Parms->u.regsCh.reg0x01;
        Adr = 0x01;
        Device = (U008)(pDev->Dac.EncoderID >> 1);
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsCh.reg0x08;
        Adr = 0x08;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsCh.reg0x09;
        Adr = 0x09;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsCh.reg0x0A;
        Adr = 0x0A;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsCh.reg0x0B;
        Adr = 0x0B;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
        Data = Parms->u.regsCh.reg0x11;
        Adr = 0x11;
        i2cWrite(pDev, Head, pDev->Dac.TVOutPortID, Device, (U016) 1, &Adr, 1, &Data);
    }
    RestoreLock(pDev, Head, lock);
}


//  Return the default TV mode stored in the bios.
RM_STATUS dacGetBiosDefaultTVType
(
    PHWINFO pDev,
    U032    *biostvdefault
)
{
 
    U008  data, lock;
    U032  Head = 0;  // BIOS knows only about head 0

    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);
    
    CRTC_RD(NV_CIO_CRE_SCRATCH0__INDEX, data, Head); 
    
    RestoreLock(pDev, Head, lock);
    
    *biostvdefault = data & 0x07;
    
    return RM_OK;
}

// Set the default TV mode stored in the bios.
// Normally, this will only persist till shutdown.  But in the
// mobile case we call through the VGA BIOS to the system BIOS
// to commit the new setting to CMOS.
// This register is referred to (through a call to dacGetBiosDefaultTVType)
// by the control panel and QuickTweak to determine the preferred TV setting
// when referring to the registry is not appropriate.
RM_STATUS dacSetBiosDefaultTVType
(
    PHWINFO pDev,
    U032    biosTVSetting
)
{
 
    U008  data, lock;
    U032  Head = 0;  // BIOS knows only about head 0

    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);
    
    CRTC_RD(NV_CIO_CRE_SCRATCH0__INDEX, data, Head);
    data &= ~0x07;
    data |= (biosTVSetting & 0x07);
    CRTC_WR(NV_CIO_CRE_SCRATCH0__INDEX, data, Head); 
    
    RestoreLock(pDev, Head, lock);
    
#if !defined(MACOS) && !defined(LINUX)
    //
    // If this is a mobile BIOS that supports storing the tv state in the system
    // cmos, we need to pass the information down.
    //
    if (pDev->Power.MobileOperation == 2)
    {
        // Call through VGA BIOS to system BIOS to set tv mode CMOS bit.
        RM_STATUS status;
        U032 eax, ebx, ecx, edx;
        eax = 0x4F14;       // NV VESA function
        ebx = 0x8105;       // set TV format and commit to CMOS
        ecx = biosTVSetting;
        edx = 0;
        status = osCallVideoBIOS(pDev, &eax, &ebx, &ecx, &edx, NULL);
    }
#endif
    return RM_OK;
}
        
//  Detect TV encoder 
//  Find the encoder ID (i2c address) and the type (maker and version) and save in the pDev dac structure.
VOID    dacDetectEncoder
(
    PHWINFO pDev,
    U032    Head
)
{
    U008    lock, ack, read, retry;

    // see if we have a TV encoder, and what type it is.
    // Identify which part: Chrontel or Brooktree? There are 2 possible ID's for each part
    // Unlock CRTC extended regs
    if (IsNV11(pDev))
    {
        AssocDDC(pDev, Head);
        AssocTV(pDev, Head);
    }

    lock = UnlockCRTC(pDev, Head);

    if (IsNV4(pDev) && Head == 0)   // TV and CRT share the i2c port
        EDIDRead(pDev, Head, DISPLAY_TYPE_MONITOR);   // this will stop the CRT from driving DDC1 data on the bus

    i2cInit(pDev, Head, pDev->Dac.TVOutPortID);

    for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x8A);     // is it Brooktree?
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
    }
    if (ack == 0) { // We got a response, so this must be our device
        pDev->Dac.EncoderID = 0x8A;
        pDev->Dac.EncoderType = TV_ENCODER_BROOKTREE;
    } else {    // Not address 8A, try alternate
        for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
            ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x88); // Brooktree alt address
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
        }
        if (ack == 0) { // We got a response, so this must be our device
            pDev->Dac.EncoderID = 0x88;
            // Differentiate between Brooktree and Philips
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
            ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x1C); // Philips ID reg
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID); // Start w/o Stop = Restart
            ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
            i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            if (ack) 
                pDev->Dac.EncoderType = TV_ENCODER_BROOKTREE;
            else
            {
                pDev->Dac.EncoderType = TV_ENCODER_PHILIPS; // generic ID
                switch (read) 
                {
                    case 0x02:
                        pDev->Dac.EncoderType = NV_ENCODER_PHILIPS_7102;
                        break;    
                    case 0x03:
                        pDev->Dac.EncoderType = NV_ENCODER_PHILIPS_7103;
                        break;    
                }
                // If there is a decoder on the chip, then it is considered to be a 7108 or 7109
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x40); // 7114 decoder address    
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
                if (ack == 0) 
                {
                    if (pDev->Dac.EncoderType == NV_ENCODER_PHILIPS_7102) 
                    {
                        pDev->Dac.EncoderType = NV_ENCODER_PHILIPS_7108;
                    }
                    if (pDev->Dac.EncoderType == NV_ENCODER_PHILIPS_7103) 
                    {
                        pDev->Dac.EncoderType = NV_ENCODER_PHILIPS_7109;
                    }
                }
            }
        } else {    // Not Brooktree
            for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0xEA); // is it Chrontel?
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            }
            if (ack == 0) { // We got a response, so this must be our device
                pDev->Dac.EncoderID = 0xEA;
                pDev->Dac.EncoderType = TV_ENCODER_CHRONTEL;
            } else {    // Try alternate Chrontel address
                for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                    ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0xEC); // try Chrontel alt address
                    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
                }
                if (ack == 0) { // We got a response, so this must be our device
                    pDev->Dac.EncoderID = 0xEC;
                    pDev->Dac.EncoderType = TV_ENCODER_CHRONTEL;
                } else {
                    pDev->Dac.EncoderType = NV_ENCODER_NONE;
                }
            }
        }
    }

    // if the part responded to the Chrontel address, check subaddress
    // we may have another Chrontel part (7002?)
    if (pDev->Dac.EncoderType == TV_ENCODER_CHRONTEL)
    {
        // See what BIOS booted to.
        if (dacGetBiosDisplayType(pDev, Head) == DISPLAY_TYPE_TV)  // H
            // Temporarily set DisplayType to TV. This prevents the BIOS from hanging when
            // it gets called to do a modeset, because TVConnectStatus will turn off the encoder
            // if its not the current display. The correct display type will be set later.
            // If the VCLK is coming from the encoder, we must not turn it off.

            SETDISPLAYTYPE(pDev, Head, DISPLAY_TYPE_TV);

        for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
            ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
        }
        if ((ack != 0) || (i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x25))) // 25 = version register
        {
            pDev->Dac.EncoderType = NV_ENCODER_NONE;                    
        }
        else
        {
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
            ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
            i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
            if (ack)    // if read failed, the reg doesn't exist, probably 7002
            {
                pDev->Dac.EncoderType = NV_ENCODER_NONE; 
            }
            else
            {
                switch (read & 0xFF)
                {
                    case 0x00:  // 7003 according to spec
                    case 0x01:  // 7003 observed
                    case 0x02:  // 7003B
                        pDev->Dac.EncoderType = NV_ENCODER_CHRONTEL_7003;
                        break;
                    case 0x30:  // 7004A  From Chrontel, their data sheets are incorrect.
                    case 0x31:  // 7004B
                    case 0x32:  // 7004C
                        pDev->Dac.EncoderType = NV_ENCODER_CHRONTEL_7004;
                        break;
                    case 0x38:  // 7005A
                    case 0x39:  // 7005B
                    case 0x3a:  // 7005C
                        pDev->Dac.EncoderType = NV_ENCODER_CHRONTEL_7005;
                        break;
                    case 0x28:  // 7006A
                    case 0x29:  // 7006B
                    case 0x2A:  // 7006C
                        pDev->Dac.EncoderType = NV_ENCODER_CHRONTEL_7006;
                        break;
                    case 0x50:  // 7007
                        pDev->Dac.EncoderType = NV_ENCODER_CHRONTEL_7007;
                        break;
                    case 0x40:  // 7008
                        pDev->Dac.EncoderType = NV_ENCODER_CHRONTEL_7008;
                        break;
                    default:    // 7002 should return FF
                        pDev->Dac.EncoderType = NV_ENCODER_NONE;
                        break;
                }
            }
        }
    }

    // if Brooktree, identify which variety
    if (pDev->Dac.EncoderType == TV_ENCODER_BROOKTREE)
    {
        // enable non-legacy readback mode for 870/871 by doing a reset - harmless for older parts
        // (this enables read of any register on 87x -- 868/869 has only one read subaddress)
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);     // subaddress
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x44);     // TIMING_RST low, set readback mode, EACTIVE
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         
        tmrDelay(pDev, 10000000);
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);  
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0xC4);      // TIMING_RST high
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         

        // Read register 0 for the device ID.
        // If this is an 86x part, we need to set Estat so that the reg read from subaddress 0 is reg 0.
        // (There are four registers which can be read at subaddress 0; Estat selects which one.)
        // If this is an 87x part and we are in non-legacy mode, setting Estat doesn't matter.
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                   
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);  
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0xC4);   // subaddress
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x01);   // set estat (estat 0 + en_out)
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         
        // Now read register 0, the ID register.
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID));     
        ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0);              // subaddress
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
        ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID+1));     
        ack |= i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read, 1);
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
        
        // Check upper 3 bits: 000 = 868, 001 = 869, 010 = 870, 011 = 871
        switch (read & 0xe0) 
        {
            case 0:
                pDev->Dac.EncoderType = NV_ENCODER_BROOKTREE_868;
                break;
            case 0x20:
                pDev->Dac.EncoderType = NV_ENCODER_BROOKTREE_869;
                break;
            case 0x60:
                pDev->Dac.EncoderType = NV_ENCODER_BROOKTREE_871;
                // disable Macrovision
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                   
                i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);  
                i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0xFC);   // subaddress
                i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x00);   //
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         
                
                break;
            case 0x40:
                pDev->Dac.EncoderType = NV_ENCODER_BROOKTREE_870;
                break;
            default:
                pDev->Dac.EncoderType = NV_ENCODER_BROOKTREE_868;
        }
    }                
      
    //
    // Relock if necessary
    //
    RestoreLock(pDev, Head, lock);            
}

VOID dacTV871Readback
(
    PHWINFO  pDev,
    U032     Head,
    U008     Mode
)
{
    U008 lock;

    if (IsNV11(pDev))
    {
        AssocDDC(pDev, Head);
        AssocTV(pDev, Head);
    }

    lock = UnlockCRTC(pDev, Head);
    if (Mode == 0) 
    {
        // enable legacy readback mode for 870/871
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);     // subaddress
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x04);     // TIMING_RST low, set legacy readback mode, !EACTIVE
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         
        tmrDelay(pDev, 10000000);
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);  
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x84);      // TIMING_RST high
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         
    }
    else
    {
        // enable non-legacy readback mode for 870/871
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);     // subaddress
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x44);     // TIMING_RST low, set legacy readback mode, !EACTIVE
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         
        tmrDelay(pDev, 10000000);
        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);  
        i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0xC4);      // TIMING_RST high
        i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         
    }
    RestoreLock(pDev, Head, lock);
    
    // restore TV connection if TV is connected on the other head
    if (GETDISPLAYTYPE(pDev, Head ^ 0x1) == DISPLAY_TYPE_TV) 
    {
        AssocDDC(pDev, Head ^ 0x1);
        AssocTV(pDev, Head ^ 0x1);
    }
}

VOID
dacDisableTV
(
    PHWINFO pDev,
    U032 Head
)
{
    U032 data32;
    U008 lock;
    U008 data08;

	// Another check to see if we're already off.
	if(!(pDev->Dac.DevicesEnabled & DAC_TV_ENABLED))
		return;

    if (pDev->Power.MobileOperation) Head = 0;  //Set to proper head to unlock and
                                                //modify CR28.

    // On dual-headed devices, enable I2C interface
    if (IsNV11(pDev)) 
    {
        AssocDDC(pDev, Head);
    }
    lock = UnlockCRTC(pDev, Head);
    //
    // Sequence per KevinM:
    //  - clear bits 7,2 in CR28
    //  - clear bits 24,20,17:16 in PLL_COEFF_SELECT
    //  - delay 200ms
    //  - turn off encoder
    //
    if ((GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
        || (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_NONE)) {    //This fn is called during dacSetModeMulti
        // with a Head parameter that need not be the one with the TV.  In that case, it would be
        // dangerous to go overwriting these registers...
        dacDisableCRTCSlave(pDev, Head); 
    }
    //
    // We're about to turn off the encoder, so be sure to
    // clear any pixel clock dependencies on tv.
    //
    data32 = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
    data32 &= ~((DRF_MASK(NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK_TV) <<
                 DRF_SHIFT(NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK_TV)) |
                (DRF_MASK(NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK2_TV) <<
                 DRF_SHIFT(NV_PRAMDAC_PLL_COEFF_SELECT_VS_PCLK2_TV)) |
                (DRF_MASK(NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_SOURCE) <<
                 DRF_SHIFT(NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_SOURCE)) |
                (DRF_MASK(NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_RATIO) <<
                 DRF_SHIFT(NV_PRAMDAC_PLL_COEFF_SELECT_TVCLK_RATIO)));
    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, data32);

    tmrDelay(pDev, 200000000);   // delay 200 ms

    
    // Toshiba can do an SMI, and they may leave the ext CRTC's locked!!!
    // So before doing I2C, make sure they are still unlocked.
    CRTC_RD(NV_CIO_SR_LOCK_INDEX, data08, Head);
    if (!data08) 
    {
        lock = UnlockCRTC(pDev, Head);  // do it again.
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ext CRTC's are locked in dacDisableTV\r\n");
    }
    switch (TV_ENCODER_FAMILY(pDev->Dac.EncoderType))
    {
        case TV_ENCODER_CHRONTEL:
            // power down to blank screen
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                    // start state for I2C
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_0E);      // PM register 
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_OFF);         // power down (+ reset high)  
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                     // end state for I2C
            break;
        case TV_ENCODER_BROOKTREE:
            // Turn DAC off
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL);    // subaddress
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_OFF);    // DAC OFF
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            dacTVReadModifyWrite(pDev, Head, BT_EN_OUT, BT_EN_OUT_OFF, BT_EN_OUT_ON);
            break;
        case TV_ENCODER_PHILIPS:
            dacTVReadModifyWrite(pDev, Head, PH_STDCTL, PH_DAC_OFF, PH_DAC_POWER);
            break;
        case TV_ENCODER_NONE:
        default:
            // what can we do?
            break;
    }
    RestoreLock(pDev, Head, lock);
    
	pDev->Dac.DevicesEnabled &= ~(DAC_TV_ENABLED);
}

VOID
dacEnableTV
(
    PHWINFO pDev,
    U032 Head
)
{
    U008 lock;
    // On dual-headed devices, enable I2C interface
    if (IsNV11(pDev)) 
    {
        AssocDDC(pDev, Head);
    }
    lock = UnlockCRTC(pDev, Head);
    
    switch (TV_ENCODER_FAMILY(pDev->Dac.EncoderType))
    {
        case TV_ENCODER_CHRONTEL:
            // power up DAC
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                // start state for I2C
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_0E);  // PM register 
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_ON);      // power on (+ reset high)  
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                 // end state for I2C
            break;
        case TV_ENCODER_BROOKTREE:
            // Turn DAC on
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL);    // subaddress
            i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_ON);    // DAC ON
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            dacTVReadModifyWrite(pDev, Head, BT_EN_OUT, BT_EN_OUT_ON, BT_EN_OUT_ON);
            break;
        case TV_ENCODER_PHILIPS:
            dacTVReadModifyWrite(pDev, Head, PH_STDCTL, PH_DAC_ON, PH_DAC_POWER);
            break;
        case TV_ENCODER_NONE:
        default:
            // what can we do?
            break;
    }
    RestoreLock(pDev, Head, lock);

	pDev->Dac.DevicesEnabled |= DAC_TV_ENABLED;
}
