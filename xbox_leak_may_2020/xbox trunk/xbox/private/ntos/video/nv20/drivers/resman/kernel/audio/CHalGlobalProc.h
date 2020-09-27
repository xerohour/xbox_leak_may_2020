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
* Module: CHalGlobalProc.h                                                   *
*   All functions common to diff HALs are implemented here			        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CHALGLOBALPROCH_
#define _CHALGLOBALPROCH_

class CHalBase;
class CObjBase;
class CGPScratchManager;
class CGPDspProgram;
class CGPMethod;
class CScratchManager;

class CHalGlobalProc : public CObjBase
{
public:
    CHalGlobalProc(CHalBase *pHal);
	~CHalGlobalProc() { };

public:

	// each derived object has to implement these functions
	RM_STATUS Allocate(VOID *pParam);
	RM_STATUS Free(VOID *pParam = NULL);

    VOID    *GetSgeOutLinAddr() { return m_SgeOut.pLinAddr; }
    VOID    WriteCommand(U032, DSP_CONTROL *, WORD4 * = NULL, WORD5 * = NULL, WORD6 * = NULL);
    
    VOID    SetNotifierMemory(VOID *pLinAddr, U032 uSize)   { m_pNotifier = (U008 *)pLinAddr; m_uNotifierSize = uSize; }
    VOID    WriteNotifier(U032 uMethod, U032 uData, U008 uStatus);
    
    VOID    ExecuteMethod();
    
    // debug only
    CScratchManager *GetScratchManager() { return (CScratchManager *)m_pScratchManager; }
    
    RM_STATUS   AC3SetOutputBuffer(PMEM_DESC pMem, U032 uNumOfPages);
    VOID        AC3RemoveOutputBuffer(PMEM_DESC pMem, U032 uNumOfPages);
    
    U032        AC3GetBufferOffset();

private:
    MEM_DESC    m_SgeOut;

    // notifier memory
    U008        *m_pNotifier;
    U032        m_uNotifierSize;

    // objects
    CGPScratchManager   *m_pScratchManager;
    CGPDspProgram       *m_pDspProgram;
    CGPMethod           *m_pMethod;
};

#endif