 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: VID_MODE.C                                                        *
*   This is the VIDEO Mode OEM module.                                      *
*   Additional video modes can be added via this module.                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include "basedef.h"
#include "vmmtypes.h"
#include "vmm.h"
#include "vmmreg.h"
#include "vpicd.h"
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <gr.h>
#include <mc.h>
#include <os.h>
#include <vga.h>
#include <tv.h>
#include "nvrmwin.h"
#include "nvhw.h"
#include "vnvrmd.h"
#include "oswin.h"
#include "vdd.h"
#include "vmm2.h"		// more vmm services
#include "modes.h"
#include "edid.h"
#include "vesadata.h"

extern char strRefreshOverride[];
//JJV-New Registry Process extern char strMaxRefreshRate[];
//JJV-New Registry Process extern char strMaxOptimalRefreshRate[];
//JJV-New Registry Process extern char strOptimalNotFromEDID[];
extern char strMemoryOverride[];
extern char strTilingOverride[];
extern char strRTLOverride[]; 
extern char strPBUSOverride[];
extern char strUnderscanXOverride[];
extern char strUnderscanYOverride[];
extern char strDevNodeRM[];
//JJV-New Registry Process extern char strDevNodeDisplay[];
//JJV-New Registry Process extern char strDMTOverride[];
//JJV-New Registry Process extern char strMonitorTiming[];
//JJV-New Registry Process extern char strCursorCacheOverride[];
extern char strFetchTriggerOverride[];
extern char strFetchSizeOverride[];
extern char strFetchMaxReqsOverride[];
extern char strGraphicsDebug0[];
extern char strGraphicsDebug1[];
extern char strGraphicsDebug2[];
extern char strGraphicsDebug3[];

//JJV-New Registry Process char strNewModeSet[] = "NewStyleModeSet";

RM_STATUS UpdateArbitrationSettings(PHWINFO);

#define NUMBER_OF_MODES 18	// this may change later
#define NUMBER_OF_DEPTHS 4
#define NUMBER_OF_RATES  13 // Double all the 60-120 Hz modes up to 240 Hz
#define DEFAULT_MODE 0x111
#define MODE_TABLE_LENGTH (NUMBER_OF_MODES * NUMBER_OF_DEPTHS * sizeof(U016))

//	this table translates resolutions into their VESA BIOS mode equivalents
U016 ModeSetTable[NUMBER_OF_MODES*NUMBER_OF_DEPTHS] =
{
	// depths: 4 (planar modes), 8, 16, 24/32
	//0:
	// 1600 x 1200
	0xff, 0x145, 0x146, 0x14E,
	//1:
	// 1280 x 1024
	0x106, 0x107, 0x11a, 0x14D,
	//2:
	// 1152 x 864
	0xff, 0x141, 0x142, 0x143,
	//3:
	// 1024 x 768
	0x104, 0x105, 0x117, 0x118,
	//4:
	// 960 x 720
	0xff, 0x14a, 0x14b, 0x14c,
	//5:
	// 800 x 600
	0x102, 0x103, 0x114, 0x115,
	//6:
	// 640 x 480
	0xff, 0x101, 0x111, 0x112,
	//7:
	// 640 x 400
	0xff, 0x100, 0x13d, 0x13e,
	//8:
	// 512 x 384
	0xff, 0x13a, 0x13b, 0x13c,
	//9:
	// 480 x 360
	0xff, 0x147, 0x148, 0x149,
	//10:
	// 400 x 300
	0xff, 0x137, 0x138, 0x139,
	//11:
	// 320 x 400
	0xff, 0x131, 0x132, 0x133,
	//12:
	// 320 x 240					   
	0xff, 0x134, 0x135, 0x136,
	//13:
	// 320 x 200
	0xff, 0x130, 0x10e, 0x10f,
	//14:
	// 1920 x 1200
	0xff, 0x154, 0x155, 0x156, 
	//15:
	// 1920 x 1080
	0xff, 0x151, 0x152, 0x153,
	//16:
	// 1800 x 1440
	0xff, 0x14f, 0x150, 0x157,
	//17:
	// 2048 x 1536
	0xff, 0x158, 0x159, 0x15A
};

//
// CRTC Timings modes for 60Hz, 70Hz, 72Hz, 75Hz, 85Hz, 100Hz, 120Hz
//
// Currently we're using hardcoded table values, but there is no reason why we
// don't just use the standard GTF algorithms and compute any frequency on the
// fly (once we've got a GTF function that doesn't use floating point).
//
// Horizontal Total, Start, End
// Vertical Total, Start, End
// DotClock, Refresh
// HSync polarity 
// VSync polarity 
//
// Max out all the modes above our pixclk (~250).
// 
U016 ModeTimingTable[NUMBER_OF_MODES*NUMBER_OF_RATES][10] =
{
	//0:
    // Settings for 1600x1200:
    2160,1704,1880,1242,1201,1204,16096,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2176,1712,1888,1249,1201,1204,19025,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2176,1712,1888,1251,1201,1204,19600,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1253,1201,1204,20599,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2208,1728,1904,1272,1201,1204,28064,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2224,1736,1912,1286,1201,1204,34321,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //1:
    // Settings for 1280x1024:
    1712,1360,1496,1060,1025,1028,10888,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1728,1368,1504,1066,1025,1028,12894,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1728,1368,1504,1067,1025,1028,13275,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1728,1368,1504,1069,1025,1028,13854,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1744,1376,1512,1075,1025,1028,15936,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1760,1376,1520,1085,1025,1028,19096,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1776,1384,1528,1097,1025,1028,23379,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1776,1384,1528,1111,1025,1028,27624,14000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1776,1384,1528,1113,1025,1028,28464,14400,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1776,1384,1528,1117,1025,1028,29757,15000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1792,1392,1536,1131,1025,1028,34455,17000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1776,1384,1528,1097,1025,1028,23379,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1776,1384,1528,1097,1025,1028,23379,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //2:
    // Settings for 1152x864:
    1520,1216,1336,895,865,868,8162 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1536,1224,1344,900,865,868,9677 ,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1536,1224,1344,901,865,868,9964 ,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1552,1224,1352,902,865,868,10499,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1552,1224,1352,907,865,868,11965,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1568,1232,1360,915,865,868,14347,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1584,1240,1368,926,865,868,17601,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1600,1248,1376,937,865,868,20989,14000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1600,1248,1376,939,865,868,21635,14400,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1600,1248,1376,943,865,868,22632,15000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1600,1248,1376,954,865,868,25949,17000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1616,1256,1384,972,865,868,31415,20000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1600,1248,1376,943,865,868,22632,15000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //3:
    // Settings for 1024x768:
    1344,1080,1184,795,769,772,6411 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1360,1080,1192,800,769,772,7616 ,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1360,1080,1192,801,769,772,7843 ,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1360,1080,1192,802,769,772,8180 ,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1376,1088,1200,807,769,772,9439 ,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //1392,1096,1208,814,769,772,11331,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1392,1096,1208,814,769,772,11370,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1408,1104,1216,823,769,772,13905,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1408,1104,1216,833,769,772,16420,14000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1408,1104,1216,835,769,772,16930,14400,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1408,1104,1216,838,769,772,17699,15000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1424,1112,1224,848,769,772,20528,17000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1424,1112,1224,864,769,772,24607,20000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1440,1120,1232,886,769,772,30620,24000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //4:
    // Settings for 960x720:
    1248,1008,1104,746,721,724,5586 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1264,1008,1112,750,721,724,6636 ,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1264,1008,1112,751,721,724,6835 ,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1280,1016,1120,752,721,724,7219 ,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1280,1016,1120,756,721,724,8225 ,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1296,1024,1128,763,721,724,9888 ,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1312,1032,1136,772,721,724,12154,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1312,1032,1136,781,721,724,14345,14000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1328,1040,1144,783,721,724,14973,14400,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1328,1040,1144,786,721,724,15657,15000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1328,1040,1144,795,721,724,17948,17000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1344,1048,1152,810,721,724,21773,20000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1344,1048,1152,831,721,724,26805,24000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //5:
    // Settings for 800x600:
    1024,832,912,622,601,604,3822,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1040,840,920,625,601,604,4550,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1040,840,920,626,601,604,4687,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1040,840,920,627,601,604,4891,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1056,840,928,630,601,604,5655,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1072,848,936,636,601,604,6818,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1088,856,944,643,601,604,8395,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1088,856,944,651,601,604,9916,14000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1088,856,944,653,601,604,10231,14400,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1088,856,944,655,601,604,10690,15000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1104,864,952,663,601,604,12443,17000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1104,864,952,675,601,604,14904,20000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1120,872,960,692,601,604,18601,24000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //6:
    // Settings for 640x480:
    800,656,720,497,481,484,2386,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    816,664,728,500,481,484,2856,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    816,664,728,501,481,484,2943,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    816,664,728,502,481,484,3072,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    832,672,736,505,481,484,3571,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,509,481,484,4316,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,515,481,484,5241,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    864,680,752,521,481,484,6302,14000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    864,680,752,522,481,484,6495,14400,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    864,680,752,524,481,484,6791,15000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    880,688,760,531,481,484,7944,17000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    880,688,760,540,481,484,9504,20000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    880,688,760,554,481,484,11700,24000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //7:
    // Settings for 640x400:
    //There is no 60Hz timings for 640x400 as defined by IBM VGA
    //784,648,712,415,401,404,1952,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    800,656,720,417,401,404,2335,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    800,656,720,417,401,404,2335,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    800,656,720,418,401,404,2408,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    800,656,720,418,401,404,2508,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    816,664,728,421,401,404,2920,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    832,672,736,424,401,404,3528,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,429,401,404,4366,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,429,401,404,4366,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,429,401,404,4366,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,429,401,404,4366,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,429,401,404,4366,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,429,401,404,4366,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,429,401,404,4366,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //8:
    // Settings for 512x384:
    // NOTE: This is a scan-doubled version of 512x768 timings
    672,536,592,795,769,772,3205,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    672,536,592,800,769,772,3763,7000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    688,544,600,801,769,772,3968,7200,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    688,544,600,802,769,772,4138,7500,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    688,544,600,807,769,772,4719,8500,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    688,544,600,814,769,772,5600,10000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    704,552,608,823,769,772,6953,12000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    704,552,608,823,769,772,6953,12000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    704,552,608,823,769,772,6953,12000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    704,552,608,823,769,772,6953,12000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    704,552,608,823,769,772,6953,12000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    704,552,608,823,769,772,6953,12000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    704,552,608,823,769,772,6953,12000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    //9:
    // Settings for 480x360:
    // NOTE: This is a scan-doubled version of 480x720 timings
    624,504,552,746,721,724,2793,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    640,512,560,750,721,724,3360,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    640,512,560,751,721,724,3461,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    640,512,560,752,721,724,3610,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    640,512,560,756,721,724,4113,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,763,721,724,5005,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,772,721,724,6077,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,772,721,724,6077,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,772,721,724,6077,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,772,721,724,6077,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,772,721,724,6077,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,772,721,724,6077,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,772,721,724,6077,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //10:
    // Settings for 400x300:
    // NOTE: This is a scan-doubled version of 400x600 timings
    512,416,456,622,601,604,1911,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,625,601,604,2310,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,626,601,604,2380,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,627,601,604,2483,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,630,601,604,2827,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,636,601,604,3358,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    544,432,472,643,601,604,4198,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    544,432,472,643,601,604,4198,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    544,432,472,643,601,604,4198,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    544,432,472,643,601,604,4198,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    544,432,472,643,601,604,4198,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    544,432,472,643,601,604,4198,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    544,432,472,643,601,604,4198,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //11:
    // Settings for 320x400:
    //
    // !!! This mode should never be set !!! since DDraw doesn't support it yet
    //
    //384,320,352,415,401,404,956 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,417,401,404,1168,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,417,401,404,1168,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,418,401,404,1204,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,418,401,404,1254,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,421,401,404,1489,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,424,401,404,1764,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //12:
    // Settings for 320x240:
    // NOTE: This is a scan-doubled version of 320x480 timings
    400,328,360,497,481,484,1193,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,500,481,484,1400,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,501,481,484,1501,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,502,481,484,1566,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,505,481,484,1786,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,509,481,484,2117,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    432,344,376,515,481,484,2670,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    432,344,376,515,481,484,2670,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    432,344,376,515,481,484,2670,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    432,344,376,515,481,484,2670,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    432,344,376,515,481,484,2670,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    432,344,376,515,481,484,2670,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    432,344,376,515,481,484,2670,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //13:
    // Settings for 320x200:
    // NOTE: This a scan-doubled version of 320x400 timings
    //There is no 60Hz timings for 320x200 as defined by IBM VGA
    //384,320,352,415,401,404,956 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,417,401,404,1168,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,417,401,404,1168,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,418,401,404,1204,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,418,401,404,1254,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,421,401,404,1489,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,424,401,404,1764,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,


    //14:
    // Settings for 1920x1200:
    2592,2048,2256,1242,1201,1204,19316,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2608,2056,2264,1249,1201,1204,22802,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1251,1201,1204,23635,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1253,1201,1204,24659,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2640,2072,2280,1260,1201,1204,28274,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2656,2072,2288,1271,1201,1204,33758,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //15:
    // Settings for 1920x1080:
    2576,2040,2248,1118,1081,1084,17280,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2608,2056,2264,1124,1081,1084,20520,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2608,2056,2264,1126,1081,1084,21144,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2608,2056,2264,1128,1081,1084,22064,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2640,2072,2280,1144,1081,1084,30202,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2656,2072,2288,1157,1081,1084,36876,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //16:
    // Settings for 1800x1440:
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2472,1936,2136,1499,1441,1444,25939,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2472,1936,2136,1500,1441,1444,26698,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2472,1936,2136,1503,1441,1444,27866,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2488,1944,2144,1512,1441,1444,31976,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    
    //17:
    // Settings for 2048x1536:
    2800,2200,2424,1589,1537,1540,26695,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2816,2208,2432,1599,1537,1540,31519,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2816,2208,2432,1600,1537,1540,32440,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2832,2216,2440,1603,1537,1540,34048,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2832,2216,2440,1612,1537,1540,38804,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2800,2200,2424,1589,1537,1540,26695,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2800,2200,2424,1589,1537,1540,26695,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2800,2200,2424,1589,1537,1540,26695,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2800,2200,2424,1589,1537,1540,26695,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2800,2200,2424,1589,1537,1540,26695,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2800,2200,2424,1589,1537,1540,26695,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2800,2200,2424,1589,1537,1540,26695,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2800,2200,2424,1589,1537,1540,26695,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    
};

//
// VBE3.0A CRTC Table
//
typedef struct crtc_table_structure
{
    U016	horiz_total;
	U016	horiz_start;
	U016	horiz_end;
	U016	vertical_total;
	U016	vertical_start;
	U016	vertical_end;
	U008	horiz_polarity;
	U008	vertical_polarity;
	U008	flags;
	U016	dot_clock;
	U016	refresh;
	U008	reserved[40];
} CRTC_STRUCTURE, *PCRTC_STRUCTURE;

//
// VBE3.0P CRTC Table
//
typedef struct crtc_table_structure2
{
    U016	horiz_total;
	U016	horiz_start;
	U016	horiz_end;
	U016	vertical_total;
	U016	vertical_start;
	U016	vertical_end;
	U008	flags;
	U032	dot_clock;
	U016	refresh;
	U008	reserved[40];
} CRTC_STRUCTURE2, *PCRTC_STRUCTURE2;



//*****************************************************************************
// DMT (Discret Mode Timings) for older monitors
//*****************************************************************************

//*****************************************************************************
//
// Do VESA 1.0 rev 0.7 DMT timings for the following modes:
//
// DMT timing calculations were produced with the VESA DMT 1.0 rev 0.7 
// Monitor Timing Specifications and D.Reed's NV3 CRTC spreadsheet.
//
//       1600 x 1200 @ 60,70,75,85
//       1280 x 1024 @ 60,75,85
//       1152 x  864 @ 75
//       1024 x  768 @ 60,70,75,85
//        800 x  600 @ 60,72,75,85
//        640 x  480 @ 60,72,75,85
//
// The refresh rates in the Mode Table are specified in the following order:
//
//       60,70,72,75,85,100,120
//
//*****************************************************************************

#define POLAR_NEG_NEG        0x0c0
#define POLAR_POS_NEG        0x040
#define POLAR_NEG_POS        0x080
#define POLAR_POS_POS        0x000


U016 DMTOverrideTable[NUMBER_OF_MODES*NUMBER_OF_RATES][13] =
{
    //0:
    // Settings for 1600x1200:
    16200, POLAR_POS_POS, 0x009, 0x0c7, 0x08d,0x0d1,0x009,0x0e0, 0x010, (0x0b1-1),(0x024-1) | 0x080, 0x0af,0x0e1,
    18900, POLAR_POS_POS, 0x009, 0x0c7, 0x08d,0x0d1,0x009,0x0e0, 0x010, (0x0b1-1),(0x024-1) | 0x080, 0x0af,0x0e1,
    0    ,             0,     0,     0,     0,    0,    0,    0,     0,         0,                 0,     0,    0,
    20250, POLAR_POS_POS, 0x009, 0x0c7, 0x08d,0x0d1,0x009,0x0e0, 0x010, (0x0b1-1),(0x024-1) | 0x080, 0x0af,0x0e1,
    22950, POLAR_POS_POS, 0x009, 0x0c7, 0x08d,0x0d1,0x009,0x0e0, 0x010, (0x0b1-1),(0x024-1) | 0x080, 0x0af,0x0e1,
    0    ,             0,     0,     0,     0,    0,    0,    0,     0,         0,                 0,     0,    0,
    0    ,             0,     0,     0,     0,    0,    0,    0,     0,         0,                 0,     0,    0,
    0    ,             0,     0,     0,     0,    0,    0,    0,     0,         0,                 0,     0,    0,
    0    ,             0,     0,     0,     0,    0,    0,    0,     0,         0,                 0,     0,    0,
    0    ,             0,     0,     0,     0,    0,    0,    0,     0,         0,                 0,     0,    0,
    0    ,             0,     0,     0,     0,    0,    0,    0,     0,         0,                 0,     0,    0,
    0    ,             0,     0,     0,     0,    0,    0,    0,     0,         0,                 0,     0,    0,
    0    ,             0,     0,     0,     0,    0,    0,    0,     0,         0,                 0,     0,    0,
    //1:
    // Settings for 1280x1024:
    10800, POLAR_POS_POS, 0x0ce, 0x09f, 0x092, 0x0a7,   0x015, 0x028, 0x05a, (0x001-1), (0x024-1) | 0x080, 0x0ff, 0x029,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    13500, POLAR_POS_POS, 0x0ce, 0x09f, 0x092, 0x0a3,   0x015, 0x028, 0x05a, (0x001-1), (0x024-1) | 0x080, 0x0ff, 0x029,
    15750, POLAR_POS_POS, 0x0d3, 0x09f, 0x097, 0x0a9,   0x01d, 0x02e, 0x05a, (0x001-1), (0x024-1) | 0x080, 0x0ff, 0x02f,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,       0,     0,     0,         0,                  0,     0,     0,
    //2:
    // Settings for 1152x864:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    10800, POLAR_POS_POS, 0x0c3, 0x08f, 0x087, 0x099, 0x009, 0x082, 0x0ff, (0x061-1), (0x024-1) | 0x080, 0x05f, 0x083,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //3:
    // Settings for 1024x768:
    6500 , POLAR_NEG_NEG, 0x0a3, 0x07f, 0x087, 0x084, 0x095, 0x024, 0x0f5, (0x003-1), (0x029-1) | 0x080, 0x0ff, 0x025,
    7500 , POLAR_NEG_NEG, 0x0a1, 0x07f, 0x085, 0x084, 0x095, 0x024, 0x0f5, (0x003-1), (0x029-1) | 0x080, 0x0ff, 0x025,
// 72Hz does not appear in DMT, so this is a fudge of the GTF's    
    7843 , POLAR_NEG_NEG, 0x0a1, 0x07f, 0x085, 0x085, 0x095, 0x024, 0x0f5, (0x003-1), (0x029-1) | 0x080, 0x0ff, 0x025,
    7875 , POLAR_POS_POS, 0x09f, 0x07f, 0x083, 0x083, 0x08f, 0x01e, 0x0f5, (0x001-1), (0x024-1) | 0x080, 0x0ff, 0x01f,
    9450 , POLAR_POS_POS, 0x0a7, 0x07f, 0x08b, 0x087, 0x093, 0x026, 0x0f5, (0x001-1), (0x024-1) | 0x080, 0x0ff, 0x027,
// 100Hz does not appear in DMT, so this is a fudge of the GTF's    
    11370, POLAR_POS_POS, 0x0a9, 0x07f, 0x08D, 0x086, 0x098, 0x02C, 0x0f5, (0x002-1), (0x024-1) | 0x080, 0x0ff, 0x02D,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //4:
    // Settings for 960x720:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //5:
    // Settings for 800x600:
    4000 , POLAR_POS_POS, 0x07f, 0x063, 0x083, 0x06a, 0x01a, 0x072, 0x0f0, (0x059-1), (0x02d-1) | 0x080, 0x057, 0x073,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    5000 , POLAR_POS_POS, 0x07d, 0x063, 0x081, 0x06c, 0x01b, 0x098, 0x0f0, (0x07d-1), (0x023-1) | 0x080, 0x057, 0x099,
    4950 , POLAR_POS_POS, 0x07f, 0x063, 0x083, 0x067, 0x011, 0x06f, 0x0f0, (0x059-1), (0x02c-1) | 0x080, 0x057, 0x070,
    5625 , POLAR_POS_POS, 0x07e, 0x063, 0x082, 0x069, 0x011, 0x075, 0x0f0, (0x059-1), (0x02c-1) | 0x080, 0x057, 0x076,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //6:
    // Settings for 640x480:
    2517 , POLAR_NEG_NEG, 0x05f, 0x04f, 0x083, 0x053, 0x09f, 0x00b, 0x03e, (0x0ea-1), (0x02c-1) | 0x080, 0x0df, 0x00c,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    3150 , POLAR_NEG_NEG, 0x063, 0x04f, 0x087, 0x054, 0x099, 0x006, 0x03e, (0x0e9-1), (0x02c-1) | 0x080, 0x0df, 0x007,
    3150 , POLAR_NEG_NEG, 0x064, 0x04f, 0x088, 0x053, 0x09b, 0x0f2, 0x01f, (0x0e1-1), (0x024-1) | 0x080, 0x0df, 0x0f3,
    3600 , POLAR_NEG_NEG, 0x063, 0x04f, 0x087, 0x058, 0x09f, 0x0fb, 0x01f, (0x0e1-1), (0x024-1) | 0x080, 0x0df, 0x0fc,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //7:
    // Settings for 640x400:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //8:
    // Settings for 512x384:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //9:
    // Settings for 480x360:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //10:
    // Settings for 400x300:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //11:
    // Settings for 320x400:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //12:
    // Settings for 320x240:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    //13:
    // Settings for 320x200:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,

    //14:
    // Settings for 1920x1200:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,

    //15:
    // Settings for 1920x1080:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,

    //16:
    // Settings for 1800x1440:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    
    //17:
    // Settings for 2048x1536:
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    0    ,             0,     0,     0,     0,     0,     0,     0,     0,         0,                  0,     0,     0,
    
};

//---------------------------------------------------------------------------
//
//  Set the requested video mode
//
//---------------------------------------------------------------------------

RM_STATUS osSetVideoMode
(
    PHWINFO pDev
)
{
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT VidLutCurDac;
	PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

    U008    mode, bitdepth, doubled;
    CLIENT_STRUCT     crs;
    CRTC_STRUCTURE timings;
    CRTC_STRUCTURE2 timings2;
    U032    i, data32, temp32;
    U032    bios_revision;
    U032    UseDMTFlag;
    BOOL    monitorIsGTF;
    RM_STATUS status;
    DISPLAYINFO tmpDisplayInfo;
    BOOL    optimalRefreshSelected;
    U032    monitorTimingMode;
    GTF_TIMINGS vbe_timings;
    PDMT_OVERRIDES pDMTOverrides;
    U008 base_mode;
    U032 rate_table[] = {60,70,72,75,85,100,120,140,144,150,170,200,240};
    U032 validArb;

	mode = 0;
    doubled = 0;


    OEMEnableExtensions();          // Unlock the registers before DDC
    EDIDRead(pDev);
    OEMDisableExtensions();         // Lock the registers

	// get the index for the first dimension (resolution)
	switch (pDev->Framebuffer.VertDisplayWidth)
	{
		case 1440:
			mode = NUMBER_OF_DEPTHS*16; 
		    break;
		case 1080:
			mode = NUMBER_OF_DEPTHS*15; 
		    break;
		case 1200:
			// Just use a nested case for redundant vertical widths.
			switch (pDev->Framebuffer.HorizDisplayWidth)
			{
			  case 1920:
			    mode = NUMBER_OF_DEPTHS*14; 
			    break;
			  case 1600:
			    mode = NUMBER_OF_DEPTHS*0; 
			    break;
			  default:
			    break;
			}
	    	break;
		case 1024:
			mode = NUMBER_OF_DEPTHS*1; 
		    break;
		case 864:
			mode = NUMBER_OF_DEPTHS*2;
		    break;
		case 768:
			mode = NUMBER_OF_DEPTHS*3;
		    break;
		case 720:
			mode = NUMBER_OF_DEPTHS*4; 
		    break;
		case 600:
			mode = NUMBER_OF_DEPTHS*5; 
		    break;
		case 480:
			mode = NUMBER_OF_DEPTHS*6; 
			break;
		case 400:
			switch (pDev->Framebuffer.HorizDisplayWidth)
			{
			  case 640:
			    mode = NUMBER_OF_DEPTHS*7; 
			    break;
			  case 320:
                mode = NUMBER_OF_DEPTHS*11; 
			    break;
			  default:
			    break;
			}
			break;
		case 384:
			mode = NUMBER_OF_DEPTHS*8; 
            doubled = 1;
			break;
		case 360:
			mode = NUMBER_OF_DEPTHS*9; 
            doubled = 1;
			break;
		case 300:
			mode = NUMBER_OF_DEPTHS*10; 
            doubled = 1;
			break;
		case 240:
			mode = NUMBER_OF_DEPTHS*12; 
            doubled = 1;
			break;
		case 200:
			mode = NUMBER_OF_DEPTHS*13; 
            doubled = 1;
			break;
		default:
		    mode = NUMBER_OF_DEPTHS*6;
	}

	switch (pDev->Framebuffer.Depth)
	// the depth is 4,8,16,24, or 32. Add the second dimension.
	{
		case 4:
			break;
		case 8:
			bitdepth = mode + 1;
			break;
		case 16:
			bitdepth = mode + 2;
			break;
		case 24:
			bitdepth = mode + 3;
			break;
		case 32:
			bitdepth = mode + 3;
			break;
	    default:
			bitdepth = mode + 2;
    }
    
    // VBE SetMode
    crs.CWRS.Client_AX = 0x4F02;
	// get mode from the 2 dimensional array
	crs.CWRS.Client_BX = ModeSetTable[bitdepth];
	if ((crs.CWRS.Client_BX == 0xff) || (bitdepth > (MODE_TABLE_LENGTH)))
		crs.CWRS.Client_BX = DEFAULT_MODE;
    // linear/flat frame buffer, clear memory, use default CRTC/refresh values
    crs.CWRS.Client_BX |= 0x4000;

    //
    // Make sure we're not going into a mode that's not possible for boards with
    // less than 8meg of framebuffer
    //
    if ((pDev->Framebuffer.VertDisplayWidth >= 1024) && (pDev->Framebuffer.Depth >= 24)
        && (pDev->Framebuffer.RamSizeMb < 8))
		    crs.CWRS.Client_BX = DEFAULT_MODE;

    // Figure out what monitor timing mode we want. It is controlled by a registry flag "MonitorTiming".
    monitorTimingMode = 0; // By default, read the EDID of the monitor.
    if (osReadRegistryDword(pDev->Registry.DBstrDevNodeDisplay, strMonitorTiming, &data32) == RM_OK)
    {
        monitorTimingMode = data32;
    }
    switch (monitorTimingMode)
    {
        case 1:
            // Use DMT.
            UseDMTFlag = TRUE;
            break;
        case 2:
            // Use GTF.
            UseDMTFlag = FALSE;
            break;
        case 3:
            // Legacy mode.
            // Read the legacy DMT Override flag to see if we should use DMT. Otherwise use GTF.
            UseDMTFlag = FALSE;
            if (osReadRegistryDword(pDev->Registry.DBstrDevNodeDisplay, strDMTOverride, &data32) == RM_OK)
            {
                if (data32 == 1)
                    UseDMTFlag = TRUE;
            }
            break;
        default:
            // Automatically set it by reading the EDID.
            UseDMTFlag = TRUE; // default to DMT
            if (EDIDIsMonitorGTF(pDev, &monitorIsGTF) == RM_OK)
            {
                UseDMTFlag = (monitorIsGTF) ? FALSE : TRUE;
            }
            break;
            // 
    }

    //
    // If there's an override setting for the refresh rate, use it instead
    //
    if (osReadRegistryDword(strDevNodeRM, strRefreshOverride, &data32) == RM_OK)
        pDev->Framebuffer.HalInfo.RefreshRate = data32;


    //
    // The GTF timings for 640x400 at 70Hz and 72Hz are drastically different than
    // the original VGA timings (which officially only list 85Hz as a support rate
    // at this resolution).  Therefore, when we attempt to set the GTF rates the
    // monitors just aren't happy.
    //
    // We have two choices.  Tweak our 70Hz and 72Hz GTF numbers to mimic VGA, or
    // just use IBM VGA (70Hz) for these two rates.  Since we're doing it right, and
    // the monitors just aren't happy, I vote for the latter.
    //
    // Remember that IBM VGA 640x400 starts at 70Hz, not 60Hz.
    //
    if ((pDev->Framebuffer.HorizDisplayWidth == 640) && (pDev->Framebuffer.VertDisplayWidth == 400)
        && (pDev->Framebuffer.HalInfo.RefreshRate < 75))
            pDev->Framebuffer.HalInfo.RefreshRate = 60; // this will kick in BIOS default values (70Hz)

    //
    // NV4 limits for hires modes at 32bpp.  Windows will still send down optimal values larger
    // than the limits we placed in our INF.  Don't let them through.
    //
    if ((pDev->Framebuffer.HorizDisplayWidth == 1280) && (pDev->Framebuffer.VertDisplayWidth == 1024)
        && (pDev->Framebuffer.Depth >= 24)
        && (pDev->Framebuffer.HalInfo.RefreshRate > 100))
            pDev->Framebuffer.HalInfo.RefreshRate = 100; 
    
    if ((pDev->Framebuffer.HorizDisplayWidth == 1600) && (pDev->Framebuffer.VertDisplayWidth == 1200)
        && (pDev->Framebuffer.Depth >= 24)
        && (pDev->Framebuffer.HalInfo.RefreshRate > 75))
            pDev->Framebuffer.HalInfo.RefreshRate = 75; 
    
    if ((pDev->Framebuffer.HorizDisplayWidth == 1920) && (pDev->Framebuffer.VertDisplayWidth == 1080)
        && (pDev->Framebuffer.Depth >= 24)
        && (pDev->Framebuffer.HalInfo.RefreshRate > 72))
            pDev->Framebuffer.HalInfo.RefreshRate = 72; 
    
    if ((pDev->Framebuffer.HorizDisplayWidth == 1920) && (pDev->Framebuffer.VertDisplayWidth == 1200)
        && (pDev->Framebuffer.Depth >= 24)
        && (pDev->Framebuffer.HalInfo.RefreshRate > 60))
            pDev->Framebuffer.HalInfo.RefreshRate = 60; 
            
    //
    // There is a hardware bug in NV4's arbitration hardware having to do with memory refresh and
    // page misses.  This shows its ugly head during 64bit operation (8meg SDRAM).  If we see
    // a pixelclock setting above ~160Hz in 32bpp, drop it down to 60Hz.
    //
    // The user may have selected a different refresh rate (since the INF allows it), but they're
    // going to get 60Hz.  Sorry.
    //            
    // The highest resolution in 8meg is 1600x1200x32bpp, so just watch for that
    //
    if ((pDev->Chip.MaskRevision == NV_PMC_BOOT_0_MASK_REVISION_A)
        && (pDev->Framebuffer.RamSizeMb == 8) && (pDev->Dac.InputWidth == 64)
        && (pDev->Framebuffer.HorizDisplayWidth == 1600) 
        && (pDev->Framebuffer.VertDisplayWidth == 1200)
        && (pDev->Framebuffer.Depth >= 24)
        && (pDev->Framebuffer.HalInfo.RefreshRate > 60))
            pDev->Framebuffer.HalInfo.RefreshRate = 60; 

    // Check if "optimal" refresh rate was chosen. 
    // When "optimal" is chosen, both min and max refresh rates in DISPLAYINFO will have non-zero values. 
    // When a specific refresh rate is chosen, max will have that value and the min will have zero. 
    // When "default" is chosen, both min and max will be zero.
    tmpDisplayInfo.HdrSize = sizeof(DISPLAYINFO);
    vddGetDisplayInfo(pOsHwInfo->osDeviceHandle, &tmpDisplayInfo, sizeof(DISPLAYINFO));
    if ((tmpDisplayInfo.RefreshRateMax != 0) && (tmpDisplayInfo.RefreshRateMin != 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "OPTIMAL REFRESH!!\n\r");

        // Try to get the max frequency for this mode from the EDID. 
        // If we get a refresh rate that is less than what we have now,
        // we will use the lower value.
        if (osReadRegistryDword(pDev->Registry.DBstrDevNodeDisplay, strOptimalNotFromEDID, &data32) != RM_OK)
        {
            // No registry entry that prohibits us from looking in the EDID.
            status = EDIDGetMaxRefreshRate(pDev, pDev->Framebuffer.HorizDisplayWidth, 
                                           pDev->Framebuffer.VertDisplayWidth,
                                           &data32);
            if (status == RM_OK     && 
                data32 != 0 &&
                data32 < pDev->Framebuffer.HalInfo.RefreshRate)
            {
                pDev->Framebuffer.HalInfo.RefreshRate = data32;
            }
        } // Get optimal from EDID

        // Read the registry to see if there is a limit for the "optimal" refresh rate.
        if (osReadRegistryDword(pDev->Registry.DBstrDevNodeDisplay, strMaxOptimalRefreshRate, &data32) == RM_OK)
        {
            // Ignore the value read if it is less than 60Hz.
            if (data32 >= 60 &&
                data32 < pDev->Framebuffer.HalInfo.RefreshRate)
            {
                pDev->Framebuffer.HalInfo.RefreshRate = data32;
            }
        }
    } // "optimal" refresh rate chosen
    
    //
    // If there's max refresh rate, make sure we don't exceed that.
    //
    if (osReadRegistryDword(pDev->Registry.DBstrDevNodeDisplay, strMaxRefreshRate, &data32) == RM_OK)
    {
        // Ignore the value read if it is less than 60Hz.
        if (data32 >= 60 &&
            pDev->Framebuffer.HalInfo.RefreshRate > data32)
        {
            pDev->Framebuffer.HalInfo.RefreshRate = data32;
        }
    }

    //
    // Plug in the refresh timings.  Until we have the real algorithm, just
    // use the predefined table.  Adjust all tweener rates to the real rates.
    //
    mode /= NUMBER_OF_DEPTHS;
    mode *= NUMBER_OF_RATES;
    base_mode = mode;
    crs.CWRS.Client_BX |= 0x0800;

    if (pDev->Framebuffer.HalInfo.RefreshRate >= 240)
    {        
        mode += 12;
        pDev->Framebuffer.HalInfo.RefreshRate = 240;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 200)
    {        
        mode += 11;
        pDev->Framebuffer.HalInfo.RefreshRate = 200;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 170)
    {        
        mode += 10;
        pDev->Framebuffer.HalInfo.RefreshRate = 170;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 150)
    {      
        //
        // Most monitors can support the vert range of 800x600x150Hz but cannot support the
        // horiz range.  We removed 150Hz from the INF, but in case Windows tries it anyway,
        // override with one lower rate.
        //
        if ((pDev->Framebuffer.HorizDisplayWidth == 800) && (pDev->Framebuffer.VertDisplayWidth == 600))
        {
            mode += 8;
            pDev->Framebuffer.HalInfo.RefreshRate = 144;
        }                
        else
        {        
            mode += 9;
            pDev->Framebuffer.HalInfo.RefreshRate = 150;
        }                
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 144)
    {        
        mode += 8;
        pDev->Framebuffer.HalInfo.RefreshRate = 144;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 140)
    {        
        mode += 7;
        pDev->Framebuffer.HalInfo.RefreshRate = 140;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 120)
    {        
        mode += 6;
        pDev->Framebuffer.HalInfo.RefreshRate = 120;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 100)
    {        
        mode += 5;
        pDev->Framebuffer.HalInfo.RefreshRate = 100;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 85)
    {        
        mode += 4;
        pDev->Framebuffer.HalInfo.RefreshRate = 85;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 75)
    {        
        mode += 3;
        pDev->Framebuffer.HalInfo.RefreshRate = 75;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 72)
    {        
        mode += 2;
        pDev->Framebuffer.HalInfo.RefreshRate = 72;
    }            
    else if (pDev->Framebuffer.HalInfo.RefreshRate >= 70)
    {        
        mode += 1;
        pDev->Framebuffer.HalInfo.RefreshRate = 70;
    }            
    else
    {        
        mode += 0;
        pDev->Framebuffer.HalInfo.RefreshRate = 60;
    }            

    //
    // The documented method for Windows to select refresh rates for a display/monitor
    // combination is to take the intersection of the monitor INF entries (EDID) and
    // the display device's INF values.  This will then dictate the rates that are
    // valid for both devices.
    //
    // Unfortunately, it seems Microsoft didn't follow their own documentation in all
    // cases.  If a user selects OPTIMAL from the refresh rate menu, Windows will ask
    // us to set a refresh rate that matches the monitor information.  It doesn't matter
    // if the rate is beyond the display device's INF entries.
    //
    // Due to clocking differences between all of our shipping chips, we need to
    // validate all attempted refresh rates.  If a rate is beyond our device's limits
    // based on memory and clock variables, we need to attempt one refresh rate lower.
    // Luckily, the user has selected OPTIMAL and doesn't know exactly what rate she
    // is choosing.  If we drop it by one or two values, it doesn't really matter.
    //

    nvHalDacValidateArbSettings(pDev, 
                                pDev->Framebuffer.Depth, 0, 0, 
                                (U032)(ModeTimingTable[mode][6] * 10000),
                                &validArb);

    while ((mode > base_mode) && (!validArb))
    {
        // drop a refresh rate
        mode--;
        pDev->Framebuffer.HalInfo.RefreshRate = rate_table[mode];
         
        nvHalDacValidateArbSettings(pDev, 
                                    pDev->Framebuffer.Depth, 0, 0, 
                                    (U032)(ModeTimingTable[mode][6] * 10000),
                                    &validArb);
    }                                                            

    if (osReadRegistryDword(pDev->Registry.DBstrDevNodeDisplay, strNewModeSet, &data32) == RM_OK)
    {
        // New style modeset.
        MODE_TIMING_VALUES timingValues;
        unsigned int vesaStatus;

        if ((pDev->Framebuffer.HorizDisplayWidth == 640) && 
            (pDev->Framebuffer.VertDisplayWidth == 480)  &&
            (pDev->Framebuffer.HalInfo.RefreshRate == 60))
        {
            // force 640x480 @60Hz into DMT mode.
            UseDMTFlag = TRUE;
        }

        if (UseDMTFlag)
        {
            // Get DMT timings for this mode. 
            vesaStatus = vesaGetDMTTimings(pDev->Framebuffer.HorizDisplayWidth, 
                                           pDev->Framebuffer.VertDisplayWidth, 
                                           pDev->Framebuffer.HalInfo.RefreshRate,
                                           &timingValues);
            // If we failed to get DMT timings for this mode, force GTF
            if (vesaStatus != 0)
            {
                UseDMTFlag = FALSE;
            }
        }

        if (!UseDMTFlag)
        {
            // Get the GTF timings
            vesaStatus = vesaGetGTFTimings(pDev->Framebuffer.HorizDisplayWidth, 
                                           pDev->Framebuffer.VertDisplayWidth, 
                                           pDev->Framebuffer.HalInfo.RefreshRate,
                                           &timingValues);
        }

        // copy all the relevant data to the pDev and call dacProgramCRTC.
        if (vesaStatus == 0)
        {
            pDev->Dac.HorizontalVisible       = timingValues.HorizontalVisible;
            pDev->Dac.HorizontalBlankStart    = timingValues.HorizontalBlankStart;
            pDev->Dac.HorizontalRetraceStart  = timingValues.HorizontalRetraceStart;
            pDev->Dac.HorizontalRetraceEnd    = timingValues.HorizontalRetraceEnd;
            pDev->Dac.HorizontalBlankEnd      = timingValues.HorizontalBlankEnd;
            pDev->Dac.HorizontalTotal         = timingValues.HorizontalTotal;
            pDev->Dac.VerticalVisible         = timingValues.VerticalVisible;
            pDev->Dac.VerticalBlankStart      = timingValues.VerticalBlankStart;
            pDev->Dac.VerticalRetraceStart    = timingValues.VerticalRetraceStart;
            pDev->Dac.VerticalRetraceEnd      = timingValues.VerticalRetraceEnd;
            pDev->Dac.VerticalBlankEnd        = timingValues.VerticalBlankEnd;
            pDev->Dac.VerticalTotal           = timingValues.VerticalTotal;
            pDev->Dac.PixelDepth              = pDev->Framebuffer.Depth;
            pDev->Dac.HalInfo.PixelClock      = timingValues.PixelClock;
            pDev->Dac.HorizontalSyncPolarity  = timingValues.HSyncpolarity;
            pDev->Dac.VerticalSyncPolarity    = timingValues.VSyncpolarity;
            pDev->Dac.DoubleScannedMode       = (pDev->Framebuffer.VertDisplayWidth < 400) ? TRUE : FALSE;

            dacProgramCRTC(pDev);
        } // successfully obtained timing values.
        else
        {
            // What the heck are we going to do now! There is neither a valid DMT or GTF 
            // timing description for this mode.
            DBG_BREAKPOINT();
        }
    } // do new style modeset
    else
    {
        // old style modeset.

        //
        // Load up the VBE information table
        //
        vbe_timings.horiz_total     = ModeTimingTable[mode][0];
        vbe_timings.horiz_start     = ModeTimingTable[mode][1];
        vbe_timings.horiz_end       = ModeTimingTable[mode][2];
        vbe_timings.vertical_total  = ModeTimingTable[mode][3];
        vbe_timings.vertical_start  = ModeTimingTable[mode][4];
        vbe_timings.vertical_end    = ModeTimingTable[mode][5];
        vbe_timings.dot_clock       = (U032)(ModeTimingTable[mode][6] * 10000);
        vbe_timings.refresh         = ModeTimingTable[mode][7];
        vbe_timings.flags           = (doubled) ? 0x01 : 0x00;
        vbe_timings.flags |= (ModeTimingTable[mode][8] == BUFFER_HSYNC_POSITIVE) ? 0 : 0x04;
        vbe_timings.flags |= (ModeTimingTable[mode][9] == BUFFER_VSYNC_POSITIVE) ? 0 : 0x08;

        if (UseDMTFlag)
        {        
            pDMTOverrides = (PDMT_OVERRIDES)(&(DMTOverrideTable[mode][0]));
            //
            // If there is no DMT entry for this mode/refresh, fall back to default GTF
            //
            if (pDMTOverrides->DotClock == 0)
            {
                UseDMTFlag = FALSE;
            }
        }    

        if (!UseDMTFlag &&
            pDev->Framebuffer.HalInfo.RefreshRate == 60)
        {        
            // The GTF timings for 60Hz are not right. So just fall back to the VGA defaults.
            crs.CWRS.Client_AX = VBESetMode(crs.CWRS.Client_BX, NULL, NULL, FALSE);
        }        
        else
        {
            crs.CWRS.Client_AX = VBESetMode(crs.CWRS.Client_BX, &vbe_timings, pDMTOverrides, (U016)UseDMTFlag);
        }
    }

    //
    // Determine if it's possible to support a video overlay in this resolution
    //
    nvHalDacValidateArbSettings(pDev, 
                                pDev->Framebuffer.Depth, 1, 0, 
                                (U032)(ModeTimingTable[mode][6] * 10000),
                                &(pDev->Video.OverlayAllowed));
    
    // Check/Set any PFB overrides
    if (osReadRegistryDword(strDevNodeRM, strMemoryOverride, &data32) == RM_OK)
    {
        pDev->Framebuffer.HalInfo.Overrides.LoadMask |= FB_OVERRIDES_MEMORY;
        pDev->Framebuffer.HalInfo.Overrides.MemoryOverride = data32;
    }
    if (osReadRegistryDword(pDev, strDevNodeRM, strRTLOverride, &data32) == RM_OK)
    {
        pDev->Framebuffer.HalInfo.Overrides.LoadMask |= FB_OVERRIDES_RTL;
        pDev->Framebuffer.HalInfo.Overrides.RTLOverride = data32;
    }
    nvHalFbControl(pDev, FB_CONTROL_LOAD_OVERRIDES);

    //
    // If there's a dma fifo fetch trigger override, use it now
    //
    if (osReadRegistryDword(strDevNodeRM, strFetchTriggerOverride, &data32) == RM_OK)
    {
        pDev->Fifo.DmaFetchTrigger = data32;
    }
    
    //
    // If there's a dma fifo fetch size override, use it now
    //
    if (osReadRegistryDword(strDevNodeRM, strFetchSizeOverride, &data32) == RM_OK)
    {
        pDev->Fifo.DmaFetchSize = data32;
    }

    //
    // If there's a dma fifo fetch max reqs override, use it now
    //
    if (osReadRegistryDword(strDevNodeRM, strFetchMaxReqsOverride, &data32) == RM_OK)
    {
        pDev->Fifo.DmaFetchMaxReqs = data32;
    }

    //
    // If there are graphics debug overrides, use them now
    //
    if (osReadRegistryDword(strDevNodeRM, strGraphicsDebug0, &data32) == RM_OK)
    {
        pDev->Graphics.Debug0 = data32;
    }
    if (osReadRegistryDword(strDevNodeRM, strGraphicsDebug1, &data32) == RM_OK)
    {
        pDev->Graphics.Debug1 = data32;
    }
    if (osReadRegistryDword(strDevNodeRM, strGraphicsDebug2, &data32) == RM_OK)
    {
        pDev->Graphics.Debug2 = data32;
    }
    if (osReadRegistryDword(strDevNodeRM, strGraphicsDebug3, &data32) == RM_OK)
    {
        pDev->Graphics.Debug3 = data32;
    }
    
    //
    // Use the display cursor cache override if available
    //
    if (osReadRegistryDword(pDev->Registry.DBstrDevNodeDisplay, strCursorCacheOverride, &data32) == RM_OK)
    {
        if (data32 == 0)
            pDev->Dac.CursorCacheEnable = 0;
        else            
            pDev->Dac.CursorCacheEnable = 1;
    }
    
    //
    // If the modeset was successful, perform any more overrides required
    //
	if (crs.CWRS.Client_AX & 0xff00)
    {
        // let the miniVDD save new register states
        if (pOsHwInfo->miniVDDPostModeChange)  // if primary display, we got callback at init
            (*pOsHwInfo->miniVDDPostModeChange)();
    	return (crs.CRS.Client_EAX);
    }
    else
	{
        //
		// If 16 bit mode, turn on 565
        //
		if (pDev->Framebuffer.Depth == 16)
        {
			FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _ALT_MODE, _SEL);
        }            
        
        //
        // If 16 or 32 bit mode, enable default gamma (no pixmix for these modes)
        //
		if ((pDev->Framebuffer.Depth == 16) || (pDev->Framebuffer.Depth == 32))
        {        
			FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PIXMIX, _ON);
        }
            
		// Make sure we're using an 8bit palette
	    FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _BPC, _8BITS);
         
        //
        // Update arb settings for each mode set
        //
        VidLutCurDac.Head = 0;
        nvHalDacUpdateArbSettings(pDev, (VOID*) &VidLutCurDac));

        // let the miniVDD save new register states
        if (pOsHwInfo->miniVDDPostModeChange)  // if primary display, we got callback at init
            (*pOsHwInfo->miniVDDPostModeChange)();

        //
        // For NV4, change the default flip functionality to end of vsync, not start
        //
        // BUG BUG BUG:  There is a hardware bug in NV4-A relating to this particular
        // register.  You must access another register in this general area before
        // accessing PCRTC_CONFIG
        //
        i = REG_RD32(NV_PCRTC_START);
        FLD_WR_DRF_DEF(_PCRTC, _CONFIG, _START_ADDRESS, _NON_VGA);

    	return (RM_OK);
	}
}


BOOL nvIsResolutionValid(HWINFO info1)
{
  return FALSE;
}



