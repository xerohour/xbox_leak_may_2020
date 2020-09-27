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

/************************Base class for the HAL******************************\
*                                                                           *
* Module: CHalBase.h                                                        *
*   All functions common to diff HALs are implemented here			        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

extern "C"
{
#include <nvarm.h>
#include <AudioHw.h>
#include <nv_papu.h>
};

#include <aos.h>
#include "CHalBase.h"

CHalBase::CHalBase(U008 uRevID, HWINFO_COMMON_FIELDS *pDev, U032 uSafeReadOffset) 
{ 
	m_uRevID = uRevID; 
	m_pDev = pDev;
	m_uBase = m_pDev->pDeviceBase;
    m_uSafeReadOffset = uSafeReadOffset;

#if defined VIKDBG
    // allocate some memory that will be the device PCI space
    if (RM_OK != aosAllocateMem(1?NV_PAPU, ALIGN_DONT_CARE, (VOID **)&m_uBase, NULL) )
    {
        // well.. too bad .. i tried
        aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: CHalBase::CHalBase - no memory for the chip");
        return;
    }

    // zero the memory
    for (U032 uCnt = 0; uCnt < (1?NV_PAPU)/4; uCnt++)
        RegWrite(uCnt*4, (U032)0);
#endif
}

CHalBase::~CHalBase() 
{
    m_uRevID = 0; 

#if defined VIKDBG
    DumpChipToFile();
    aosFreeMem((VOID *)m_uBase);
#endif
}

VOID
CHalBase::DumpChipToFile()
{
    VOID *pHandle = aosCreateDiskFile();
    if (pHandle)
    {
        aosWriteDiskFile(pHandle, (VOID *)m_uBase, 1?NV_PAPU);
        aosCloseDiskFile(pHandle);
    }
}
