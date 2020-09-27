
#ifndef _VESADATA_H_
#define _VESADATA_H_
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


//*****************************************************************************
//
// Definition of display timing terms:
//
//
//  <------------------><--------><-------><------><-------><--------><--------
//
//     Active Display     Border    Porch    Sync    Porch    Border    Active
//
//
//  <---------------------------------------------------------------->
//  Total
//
//  <------------------>
//  Visible
//
//  <---------------------------->
//  Blank Start
//
//  <------------------------------------->
//  Sync Start
//
//  <--------------------------------------------->
//  Sync End
//
//  <------------------------------------------------------>
//  Blank End
//
//*****************************************************************************

#ifndef BUFFER_HSYNC_NEGATIVE
// nvrm.h also defines these. So check if it is already defined.
#define BUFFER_HSYNC_NEGATIVE           1
#define BUFFER_HSYNC_POSITIVE           0
#define BUFFER_VSYNC_NEGATIVE           1
#define BUFFER_VSYNC_POSITIVE           0

#endif

typedef struct _mode_timing_values
{  
    unsigned int HorizontalVisible; 
    unsigned int VerticalVisible;
    unsigned int Refresh;
    unsigned int HorizontalTotal; 
    unsigned int HorizontalBlankStart; 
    unsigned int HorizontalRetraceStart; 
    unsigned int HorizontalRetraceEnd; 
    unsigned int HorizontalBlankEnd;
    unsigned int VerticalTotal; 
    unsigned int VerticalBlankStart; 
    unsigned int VerticalRetraceStart; 
    unsigned int VerticalRetraceEnd; 
    unsigned int VerticalBlankEnd;
    unsigned int PixelClock;
    unsigned int HSyncpolarity; 
    unsigned int VSyncpolarity; 
} MODE_TIMING_VALUES;

unsigned int vesaGetGTFTimings (unsigned int hRes, 
                                unsigned int vRes, 
                                unsigned int refreshRate, 
                                MODE_TIMING_VALUES * timingInfo);
unsigned int vesaGetDMTTimings (unsigned int hRes, 
                                unsigned int vRes, 
                                unsigned int refreshRate, 
                                MODE_TIMING_VALUES * timingInfo);

unsigned int vesaGetNumGTFEntries(void);


#endif // _VESADATA_H_
