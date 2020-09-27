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

/****************************** DMA Manager ********************************\
*                                                                           *
* Module: CContextDma.cpp                                                   *
*   The MCPX HAL DMA engine implementation is provided in this module.      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
extern "C"
{
#include <nv32.h>
#include <nvarm.h>
#include <aos.h>
};

#include <ClinkList.h>
#include "CContextDma.h"


RM_STATUS
CContextDma::Initialize(VOID *pAddress, U032 uSize, U032 uType)
{
    RM_STATUS rmStatus = RM_ERROR;

    m_uType = uType;
    m_uSize = uSize;
    m_pLinAddr = NULL;
    m_pAddress = NULL;

    switch(m_uType)
    {
    case AUDIO_CONTEXT_DMA_GPFXPARAM:
    case AUDIO_CONTEXT_DMA_GPFXDSPCODE:
        // no check for MCP1.. just store the virtual address
        m_pLinAddr = pAddress;
        m_pAddress = NULL;
        rmStatus = RM_OK;
        break;

    case AUDIO_CONTEXT_DMA_SESGE:
    case AUDIO_CONTEXT_DMA_SESSL:
    case AUDIO_CONTEXT_DMA_GPSGE:
    case AUDIO_CONTEXT_DMA_EPSGE:
        {
            // make sure the stuff is properly aligned
            if (!PAGEOFF(pAddress))
            {
                if (!PAGEOFF(uSize))
                {
                    m_pAddress = pAddress;
                    rmStatus = RM_OK;
                }
            }
        }
        break;
    
    case AUDIO_CONTEXT_DMA_GPNOTIFIER:
    case AUDIO_CONTEXT_DMA_EPNOTIFIER:
        {
            // just make sure the address is 4K aligned and the size
            // is max 4K
            if (m_uSize <= 4096)
            {
                rmStatus = aosGetPhysicalAddress(pAddress, m_uSize, &m_pAddress);
                if (rmStatus == RM_OK)
                {
                    m_pLinAddr = pAddress;
                    if (PAGEOFF(m_pAddress))
                        rmStatus = RM_ERROR;
                }
            }
        }
        break;

    case AUDIO_CONTEXT_DMA_FENOTIFIER:
        {
            // convert the address to physical
            rmStatus = aosGetPhysicalAddress(pAddress, m_uSize, &m_pAddress);
            if (rmStatus == RM_OK)
            {
                m_pLinAddr = pAddress;

                // has to be 256 byte aligned..
                if (((U032)m_pAddress) & 0xFF)
                    rmStatus = RM_ERROR;        // alignment fails
            }
        }
        break;

    default:
        break;
    }

    return rmStatus;
}
