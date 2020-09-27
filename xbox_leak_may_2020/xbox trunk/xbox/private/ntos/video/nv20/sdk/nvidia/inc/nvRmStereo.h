#ifndef _NVRM_STEREO_H_
#define _NVRM_STEREO_H_

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

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: NvRmStereo.h                                                      *
*		Resource Manager stereo interface structure.						*
*                                                                           *
*****************************************************************************
*                                                                           *
* History:																	*
*		Andrei Osnovich  03/17/2000 Created. 								*
*       Martin Schwarzer 10/11/2000 added flags to use interface for NT4/W2K*
*                                   opengl api stereo                       *
*                                                                           *
\***************************************************************************/




//Flag values that can be ORed in the NV_CFG_STEREO_PARAMS.Flags below
#define STEREOCFG_STEREOACTIVATED   0x80000000
#define STEREOCFG_CONNECTIONTYPE    0x00000077
#define STEREOCFG_ELSADDC           0x00000001
#define STEREOCFG_INTERLACED        0x00000002
#define STEREOCFG_GPIO_0            0x00000004
#define STEREOCFG_GPIO_1            0x00000008
#define STEREOCFG_LINECODE          0x00000010
// syncronisation and other special stuff should be added below here
#define STEREOCFG_WS_OGL_API_STEREO 0x00001000 // is used by workstation OpenGL API stereo 
#define STEREOCFG_FREE_MEMORY       0x00002000 // free memory in interrupt routine to avoid race condition
// common settings
#define STEREOCFG_STEREO_INITIALIZE 0x00004000 // used by NT4/W2K displaydriver to switch on  glasses-toggling
#define STEREOCFG_STEREO_DISABLE    0x00008000 // used by NT4/W2K displaydriver to switch oFF glasses-toggling
#define STEREOCFG_ENTER_ADJACENT    0x00010000 // tells resman that from this moment all allocated stereo surfaces have be double pitched.
#define STEREOCFG_EXIT_ADJACENT     0x00020000 // cancelled the state above.

// Param Structure
typedef struct {
	unsigned long	Flags;
	unsigned long	EyeDisplayed;
	unsigned long	FlipOffsets[4][2];
} NV_CFG_STEREO_PARAMS, *LPNV_CFG_STEREO_PARAMS;

#endif	//_NVRM_STEREO_H_
