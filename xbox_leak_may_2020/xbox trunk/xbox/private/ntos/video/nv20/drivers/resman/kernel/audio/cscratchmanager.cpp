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

/************************Object to manage scratch***************************\
*                                                                           *
* Module: CScratchManager.cpp                                              *
*   Object to handle all RM-GP conversations via the scracth memory         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

extern "C"
{
#include <nvarm.h>
#include <aos.h>
#include <AudioHw.h>
#include <nv_papu.h>
};

#include "CHalBase.h"
#include "CScratchDma.h"
#include "CScratchManager.h"


RM_STATUS   
CScratchManager::Initialize(VOID_PTR pLoader, 
                            U032 uLoaderSize)
{
    
    if (!m_pScratchDma)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "CScratchManager::Initialize - one of the objects could not be constructed\n");
        return RM_ERROR;
    }
    
    // WARNING!  WARNING!  WARNING!
    // vik todo = this line is to test out the EP dolby digital stuff without
    // GP effects actually working... remove in final version!!!!

    m_uSize = 512 * PAGE_SIZE;
    
    /*
    just need enough memory to copy the loader program and the command node the GP polls
    m_uSize = uLoaderSize + whatever the command node size is...
    */

    RM_STATUS rmStatus = m_pScratchDma->Allocate(m_uSize, &m_uOffset);

    if (rmStatus != RM_OK)
        return rmStatus;

    // sanity check!
    if (m_uOffset)
    {
        // internal error!  it's all screwed up!
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "CScratchManager::Initialize - Internal Error!!!\n");
        aosDbgBreakPoint();
    }

    // make sure the loader size is good
    if (uLoaderSize < MAX_SIZE_LOADER)
        m_pScratchDma->Write(m_uOffset, (VOID *)pLoader, uLoaderSize*sizeof(U032));
    else
        return RM_ERROR;

    return RM_OK;
}

VOID
CScratchManager::Shutdown()
{
    if (m_pScratchDma && m_uSize)
        m_pScratchDma->Free(m_uOffset, m_uSize);

    m_uSize = 0;
}

// debug onlY!
VOID *
CScratchManager::GetScratchPage(U032 uPageNumber) 
{ 
    return m_pScratchDma->GetPage(uPageNumber); 
}

