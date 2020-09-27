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

/***************************** Functions for MCP1***************************\
*                                                                           *
* Module: CVPMethod.h                                                       *
*     SW functions for Voice Processor class (PIO based)                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CVPETHODH_
#define _CVPETHODH_

#define METHOD_MIN_OFFSET(x)       (x)
#define METHOD_MAX_OFFSET(x)       ((x) + 0x4)

class CHalBase;
class CHalVoiceProc;
class CHalGlobalProc;

class CVPMethod
{
public:
    CVPMethod(CHalVoiceProc *pProc, CHalGlobalProc *pGlProc, CHalBase *pHal) 
    { 
        m_pHal = pHal;
        m_pVoiceProc = pProc;
        m_pGlobalProc = pGlProc;
    };

    ~CVPMethod() { };

public:
    VOID Execute(U032 uOffset, U032 uData);

private:
    CHalBase        *m_pHal;
    CHalVoiceProc   *m_pVoiceProc;
    CHalGlobalProc  *m_pGlobalProc;

    // utility
    BOOL CheckRange(U032 uOffset, U032 uMethodOffset)   
    { 
        return ((uOffset >= METHOD_MIN_OFFSET(uMethodOffset)) && 
                (uOffset < METHOD_MAX_OFFSET(uMethodOffset))?TRUE:FALSE);
    }

    // software methods
    RM_STATUS SetContextDMANotify(U032);
    RM_STATUS SetCurrentSSLContextDMA(U032);
    RM_STATUS SetCurrentInBufSGEContextDMA(U032);
    RM_STATUS SetCurrentOutBufSGEContextDMA(U032);

    // damn hacks!
    RM_STATUS SetGPState(U032);
    RM_STATUS SetXCNTMode(U032 uData);
    RM_STATUS SetCurrentOutBufSGEOffset(U032);
    RM_STATUS SetCurrentInBufSGEOffset(U032);
    RM_STATUS SetSSLSegmentOffset(U032);
    RM_STATUS SetSSLSegmentOffsetLength(U032, U032);
    RM_STATUS SetOutBufBA(U032, U032);
    RM_STATUS SetOutBufLen(U032, U032);
};

#endif