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
* Module: CGpMethod.h                                                       *
*     SW functions for the GP                                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CGPMETHODH_
#define _CGPMETHODH_

class CGPScratchManager;
class CGPDspProgram;
class CHalBase;
class CHalGlobalProc;
class CCommandQ;
class CContextDma;
class CMixerBuffer;
class AUDIOFX;


class CGPMethod
{
public:
    CGPMethod(CHalGlobalProc *pProc, CGPScratchManager *pMgr, CGPDspProgram *pDsp, CHalBase *pHal);
    ~CGPMethod();

public:
    RM_STATUS   Initialize();
    VOID        Execute();
    VOID        *GetCommandQAddress();

private:
    CGPScratchManager   *m_pScratchManager;
    CGPDspProgram       *m_pDspProgram;
    CHalBase            *m_pHal;
    CHalGlobalProc      *m_pGlobalProc;
    CCommandQ           *m_pCommandQ;
    CMixerBuffer        *m_pMixerBuffer;
    AUDIOFX             *m_pAudioFx;
    
    U032    m_uActiveEffect;
    
    enum EFFECT_STATE
    {
        FREE = 0,
        ALLOCATED,
    };

    struct
    {
        EFFECT_STATE eState;
        CContextDma *pParamDma;
        CContextDma *pDspCodeDma;
    } m_EffectInfo[MAX_EFFECTS];
    
    struct
    {
        U032 uCount;
        U032 uEffects[MAX_EFFECTS];
    } m_ExecutionList;

    RM_STATUS GetMixBuffer(U032 uArg, U032 *pId);
    VOID UpdateExecutionList(U032 uEffectID, BOOL bOption);
    VOID ConfigureFifo(U032 uMethod, U032 uData);

    // software methods
    RM_STATUS SetCurrentEffect(U032 uData);
    RM_STATUS SetContextDmaNotifier(U032 uData);
    RM_STATUS SetContextDmaParameter(U032 uData);
    RM_STATUS SetContextDmaDspCode(U032 uData);
};

#endif