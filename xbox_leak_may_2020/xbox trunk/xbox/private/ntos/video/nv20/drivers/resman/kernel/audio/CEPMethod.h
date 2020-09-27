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

/***************************** Object Manager *****************************\
*                                                                           *
* Module: CEPMethod.h                                                     *
*     Object Classes are defined in this module.                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CEPMETHODH_
#define _CEPMETHODH_

#define METHOD_MIN_OFFSET(x)       (x)
#define METHOD_MAX_OFFSET(x)       ((x) + 0x4)

class CHalBase;
class CHalExtendedProc;

class CEPMethod
{
public:
    CEPMethod(CHalBase *pHal, CHalExtendedProc *pProc) 
    { 
        m_pHal = pHal;
        m_pProc = pProc;
        m_uCurrSge = 0;
        m_uBaseAddr = 0;
        m_uMaxOffset = 0;
    }

    ~CEPMethod() { };

public:
    RM_STATUS Execute(U032 uOffset, U032 uData);

private:
    CHalBase            *m_pHal;
    CHalExtendedProc    *m_pProc;

    // SW methods
    RM_STATUS SetCurrentSge(U032 uData);
    RM_STATUS SetSgeBaseAddr(U032, U032);
    RM_STATUS SetSgeLength(U032, U032);
    RM_STATUS SetContextDMA(U032);
    RM_STATUS SetSgeOffset(U032);
    RM_STATUS SetState(U032);
    RM_STATUS SetAC3Fifo(U032);

    // utility
    VOID GetBaseAndEnd(U032 uIndex, U032 *pBase, U032 *pEnd);
    VOID GetBaseCurEnd(U032 uIndex, U032 *pBase, U032 *pCur, U032 *pEnd);
    BOOL CheckRange(U032 uOffset, U032 uMethodOffset)   
    { 
        return ((uOffset >= METHOD_MIN_OFFSET(uMethodOffset)) && 
                (uOffset < METHOD_MAX_OFFSET(uMethodOffset))?TRUE:FALSE);
    }

    // variables equiv of some GP regs
    U032    m_uCurrSge;
    U032    m_uBaseAddr;
    U032    m_uMaxOffset;
};

#endif