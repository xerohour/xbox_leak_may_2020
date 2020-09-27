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
* Module: CDspProgram.h                                                     *
*	interface for accessing DSP program information                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CDSPPROGRAMH_
#define _CDSPPROGRAMH_

class CDspProgram
{
public:
    CDspProgram()
    {
        m_pLoader = NULL;
        m_uLoaderSize = 0;

        for (U032 uCnt = 0; uCnt < MAX_EFFECTS; uCnt++)
        {
            m_Effect[uCnt].pCode = NULL;
            m_Effect[uCnt].uCodeSize = 0;
            m_Effect[uCnt].uDelaySize = 0;
        }
    }
    
    virtual VOID Initialize() = 0;

    VOID_PTR    GetLoader() { return m_pLoader; }
    U032        GetLoaderSize() { return m_uLoaderSize; }
    
    VOID_PTR    GetCode(U032 uIndex) { return ((uIndex < MAX_EFFECTS)?(m_Effect[uIndex].pCode):NULL); }
    U032        GetDelayLineSize(U032 uIndex) { return ((uIndex < MAX_EFFECTS)?(m_Effect[uIndex].uDelaySize):0); }
    U032        GetCodeSize(U032 uIndex) { return ((uIndex < MAX_EFFECTS)?(m_Effect[uIndex].uCodeSize):0); }

protected:
    struct
    {
        VOID_PTR pCode;
        U032 uCodeSize;
        U032 uDelaySize;
    } m_Effect[MAX_EFFECTS];

    VOID_PTR    m_pLoader;
    U032        m_uLoaderSize;
};

#endif