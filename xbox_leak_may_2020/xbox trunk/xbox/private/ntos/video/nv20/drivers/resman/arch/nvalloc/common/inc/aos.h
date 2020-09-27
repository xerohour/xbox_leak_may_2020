/***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  NVIDIA, Corp. of Sunnyvale,  California owns     *|
|*     copyrights, patents, and has design patents pending on the design     *|
|*     and  interface  of the NV chips.   Users and  possessors  of this     *|
|*     source code are hereby granted a nonexclusive, royalty-free copy-     *|
|*     right  and design patent license  to use this code  in individual     *|
|*     and commercial software.                                              *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright  1993-1998  NVIDIA,  Corporation.   NVIDIA  has  design     *|
|*     patents and patents pending in the U.S. and foreign countries.        *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF THIS SOURCE CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS" WITH-     *|
|*     OUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPORATION     *|
|*     DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOURCE CODE, INCLUD-     *|
|*     ING ALL IMPLIED WARRANTIES  OF MERCHANTABILITY  AND FITNESS FOR A     *|
|*     PARTICULAR  PURPOSE.  IN NO EVENT  SHALL NVIDIA,  CORPORATION  BE     *|
|*     LIABLE FOR ANY SPECIAL,  INDIRECT,  INCIDENTAL,  OR CONSEQUENTIAL     *|
|*     DAMAGES, OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,     *|
|*     DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR     *|
|*     OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION  WITH THE     *|
|*     USE OR PERFORMANCE OF THIS SOURCE CODE.                               *|
|*                                                                           *|
|*     RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the     *|
|*     Government is subject  to restrictions  as set forth  in subpara-     *|
|*     graph (c) (1) (ii) of the Rights  in Technical Data  and Computer     *|
|*     Software  clause  at DFARS  52.227-7013 and in similar clauses in     *|
|*     the FAR and NASA FAR Supplement.                                      *|
|*                                                                           *|
\***************************************************************************/

/*
* aos.h
*
* OS-independent functions implemented by the RM
*
*/


#if !defined _AUDIOOS_H_
#define _AUDIOOS_H_

#if defined __cplusplus
extern "C" {
#endif

// init calls
RM_STATUS   RmInitAudioDevice(VOID *pResList, VOID *pDevObj, PHWINFO_COMMON_FIELDS pDev);
VOID        RmShutdownAudioDevice(PHWINFO_COMMON_FIELDS pDev);


/* specifies the dont care bits from bit 12 onwards */
#define ALIGN_4K					0x00000000
#define ALIGN_8K					0x00000001		
#define ALIGN_16K					0x00000003
#define ALIGN_32K					0x00000007
#define ALIGN_64K					0x0000000F
#define ALIGN_128K					0x0000001F
#define ALIGN_DONT_CARE				0xFFFFFFFF

// exported services
RM_STATUS	aosAllocateMem(U032 uSize, U032 uAlignMask, VOID **ppLinAddr, VOID **ppPhysAddr);
VOID		aosFreeMem(VOID *pLinAddr);

RM_STATUS	aosMapLinearAddress(VOID *pPhys, U032 uSize, VOID **ppLinear);
VOID		aosUnmapLinearAddress(VOID *pLinear, U032 uSize);

RM_STATUS   aosGetPhysicalAddress(VOID *pLinAddr, U032 uSize, VOID **pPhysAddr);

VOID        aosZeroMemory(VOID *pLinAddr, U032 uSize);
VOID        aosMemCopy(VOID *pDestination, VOID *pSource, U032 uSize);

// debug print stuff
VOID        aosDbgPrintString(int iDbgLevel, char *pStr);
VOID        aosDbgBreakPoint();

// OS-independent functions for doing file i/o - used for debugging drivers without real HW
VOID *      aosCreateDiskFile();
VOID        aosWriteDiskFile(VOID *pHandle, VOID *pBuffer, U032 uSize);
VOID        aosCloseDiskFile(VOID *pHandle);


#define PAGEOFF(p)			        (((U032)(p)) & 0xFFF)
#define MAX_SIMUL_ALLOCATIONS       2048     /* maximum number of simultaneous memory allocations */

#if !defined PAGE_SIZE
#define PAGE_SIZE                   4096
#endif

#if !defined PAGE_SHIFT
#define PAGE_SHIFT			        12L
#endif

#if !defined PAGENUM
#define PAGENUM(p)			        (((U032)(p)) >> PAGE_SHIFT)
#endif

// this is all defined in os.h for Win9x, but not for NT (??)
#if !defined DEBUGLEVEL_TRACEINFO
#define DEBUGLEVEL_TRACEINFO	    0       // For informational debug trace info
#define DEBUGLEVEL_SETUPINFO	    1       // For informational debug setup info
#define DEBUGLEVEL_USERERRORS	    2       // For debug info on app level errors
#define DEBUGLEVEL_WARNINGS	        3       // For RM debug warning info
#define DEBUGLEVEL_ERRORS	        4       // For RM debug error info
#endif

#define MIN(a,b)                    (((a)<(b))?(a):(b))
#define MAX(a,b)                    (((a)>(b))?(a):(b))

#if defined __cplusplus
};
#endif

#if defined __cplusplus
// only include the class definition for c++
class CFpState
{
public:
    CFpState();
    ~CFpState();

private:
#if defined WDM
    KFLOATING_SAVE m_State;
#else
    U032 m_dwFpuSave[32];
    U032 m_dwCr0;
#endif
};

#endif      // C++

#endif