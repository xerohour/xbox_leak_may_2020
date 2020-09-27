#ifndef _FIFO_H_
#define _FIFO_H_

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

/****************************** FIFO Manager *******************************\
*                                                                           *
* Module: FIFO.H                                                            *
*       FIFO assignment and hardware management.                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

// Required include files
#include "dmapush.h"

//---------------------------------------------------------------------------
//
//  Macros.
//
//---------------------------------------------------------------------------

#define NUM_SUBCHANNELS     8
#define FIFO_STACK_DEPTH    16

//
// Hash table function.
//
//KJK #define FIFO_HASH(h,c)  ((((h)^((h)>>8)^((h)>>16)^((h)>>24))&0xFF)^((c)&0x7F))
#define FIFO_HASH(h,c)  (((h>>0)&0x7FF)^((h>>11)&0x7FF)^((h>>22)&0x7FF)^((c<<7)&0x7FF))
#define HASH_DEPTH      pDev->Pram.HalInfo.HashDepth
//KJK #define HASH_ENTRY(h,d) ((h)*HASH_DEPTH+(d))
#define HASH_ENTRY(h,d) ((h)+(d))
//
// Gray code translation macros.  Gray code is used by the FIFO runout
// GET and PUT pointers because of the asynchronous boundary.
//
extern U032 fifoGrayToBinTable[];
extern U032 fifoBinToGrayTable[];
#define BIN_TO_GRAY(bb)     fifoIntToGrayTable[(bb)>>2]
#define GRAY_TO_BIN(gg)     fifoGrayToIntTable[(gg)>>2]

//---------------------------------------------------------------------------
//
//  Chip Engines known to FIFO
//
//---------------------------------------------------------------------------

#define ENGINE_SW           0
#define ENGINE_GRAPHICS     1

//---------------------------------------------------------------------------
//
//  Structures and types.
//
//---------------------------------------------------------------------------

//
// FIFO structure.
//
typedef struct _def_fifo
{
    BOOL    InUse;
    U032    ChID;
    U032    CacheDepth;
//    U032    ObjectStackTop[NUM_SUBCHANNELS];
//    POBJECT ObjectStack[NUM_SUBCHANNELS][FIFO_STACK_DEPTH];
    POBJECT SubchannelContext[NUM_SUBCHANNELS];
    U032    ObjectCount;
    PNODE   ObjectTree;
    PDMAPUSHER DmaPushObject;  // Object representing the state of the dma
						       // pusher for this channel
    PDMAOBJECT StallNotifier;
    BOOL       IsStalledPendingVblank;
    U032       Put;
    POBJECT    LastIllegalMthdObject;
    U032       ClassAllocLocks;
    U032       Instance;       // Additional channel context instmem
    U032       InstanceSize;   // Amount of instmem
} FIFO, *PFIFO;


//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

RM_STATUS initFifo(PHWINFO);
RM_STATUS fifoAlloc(PHWINFO, U032, U032 *);
RM_STATUS fifoAllocDma(PHWINFO, U032, U032 *, PDMAOBJECT);
RM_STATUS fifoFree(PHWINFO, U032);
RM_STATUS fifoResetChannelState(PHWINFO, U032);
RM_STATUS fifoSoftwareMethod(PHWINFO, PFIFOEXCEPTIONDATA);
RM_STATUS fifoHashEntry(PHWINFO, POBJECT, U032, U032 *);
RM_STATUS fifoAddHashEntry(PHWINFO, POBJECT, U032, U032);
RM_STATUS fifoDelHashEntry(PHWINFO, POBJECT, U032);
RM_STATUS fifoAddObject(PHWINFO, POBJECT);
RM_STATUS fifoDelObject(PHWINFO, POBJECT);
RM_STATUS fifoSearchObject(PHWINFO, U032, U032, POBJECT *);
RM_STATUS fifoUpdateContext(POBJECT, U032, U032);
RM_STATUS fifoUpdateObjectInstance(PHWINFO, POBJECT, U032, U032);
RM_STATUS fifoDeleteObjectInstance(PHWINFO, POBJECT, U032);
RM_STATUS fifoDeleteSubchannelContextEntry(PHWINFO, POBJECT);
V032      fifoService(PHWINFO);

#ifdef DEBUG
#define MAX_FIFOLOG_LENGTH	1024
extern U032 findex;
extern U032 fifolog[];

// fifolog format looks like:
//	31:28	= unique file number
//	27:4	= file line number
//	1:0	= fifo state bits (bit1 = puller, bit0 = reassign)
#define FIFOLOG(fn, fa, fb)					\
	fifolog[findex] = ((fn << 28) | (__LINE__ << 4) |	\
				((fa & 0x1) ? 1 : 0) << 1 |	\
				((fb & 0x1) ? 1 : 0));		\
	if (++findex == MAX_FIFOLOG_LENGTH)			\
		findex = 0
#else
#define FIFOLOG(fn, fa, fb)
#endif // DEBUG

#endif // _FIFO_H_
