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

/************************VP class for the HAL******************************\
*                                                                           *
* Module: CHalExtendedProc.h                                                   *
*   All functions common to diff HALs are implemented here			        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CHALEXTENDEDPROCH_
#define _CHALEXTENDEDPROCH_

class CHalBase;
class CObjBase;
class CEPScratchManager;
class CEPDspProgram;
class CScratchManager;
class CHalGlobalProc;
class CEPMethod;

class CHalExtendedProc : public CObjBase
{
public:
    CHalExtendedProc(CHalBase *pHal, CHalGlobalProc *pGlProc);
	~CHalExtendedProc() { };

public:

	// each derived object has to implement these functions
	RM_STATUS Allocate(VOID *pParam);
	RM_STATUS Free(VOID *pParam = NULL);
    
    VOID ExecuteMethod(U032 uMethod, U032 uParam);
    
    VOID *GetSgeOutLinAddr() { return m_SgeOut.pLinAddr; }
    
    CScratchManager *GetScratchManager() { return (CScratchManager *)m_pScratchManager; }
    CEPScratchManager *GetEPScratchManager() { return m_pScratchManager; }

private:
    RM_STATUS AllocateMemory(APU_EP_CREATE *);
    VOID FreeMemory();

    RM_STATUS GetCaps(APU_EP_CREATE *);
    VOID InitializeHardware();

    MEM_DESC    m_SgeOut;
    MEM_DESC    m_CommandQ;

    CEPScratchManager   *m_pScratchManager;
    CEPDspProgram       *m_pDspProgram;
    CHalGlobalProc      *m_pGlobalProc;
    CEPMethod           *m_pMethod;
};

#endif