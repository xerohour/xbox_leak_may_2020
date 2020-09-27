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

/************************Object to manage scratch Qs*************************\
*                                                                           *
* Module: CCommandQ.cpp														*
*   Object to command Q reads												*
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
#include <nv_papu.h>
#include <AudioHw.h>
};

#include "CHalBase.h"
#include "CCommandQ.h"

CCommandQ::~CCommandQ()
{
    if (m_CommandQ.pLinAddr)
        delete m_CommandQ.pLinAddr;
}

RM_STATUS
CCommandQ::Initialize(Q_TYPE eType)
{
    if (Q_TYPE_EP)
    {
        m_uPut = NV_PAPU_EPPUT;
        m_uGet = NV_PAPU_EPGET;
        m_uQ = NV_PAPU_EPCADDR;
    }
    else if (Q_TYPE_GP)
    {
        m_uPut = NV_PAPU_GPPUT;
        m_uGet = NV_PAPU_GPGET;
        m_uQ = NV_PAPU_GPCADDR;
    }
    else
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CCommandQ::CCommandQ - bad argument!");
        aosDbgBreakPoint();
        return RM_ERROR;
    }

    // allocate memory for the command Q
    RM_STATUS rmStatus =  aosAllocateMem(m_CommandQ.Size,
						                ALIGN_16K,
						                &m_CommandQ.pLinAddr,
						                &m_CommandQ.pPhysAddr);
    if (rmStatus == RM_OK)
    {
        m_pHal->RegWrite(m_uQ, (U032)m_CommandQ.pPhysAddr);
        m_pHal->RegWrite(m_uPut, (U032)0);
        m_pHal->RegWrite(m_uGet, (U032)0);
    }

    return rmStatus;
}


BOOL
CCommandQ::Read(U032 *puData)
{
    U032 uOffset;
    U032 uPut;

    // read the offset to be read from
    m_pHal->RegRead(m_uGet, &uOffset);
    *puData = *(U032*)((U008*)m_CommandQ.pLinAddr + uOffset);

    // update the GET pointer
    COMMAND_Q_INCREMENT(uOffset);
    m_pHal->RegWrite(m_uGet, uOffset);
    m_pHal->RegRead(m_uPut, &uPut);
    return (uPut == uOffset) ? TRUE:FALSE;
}
