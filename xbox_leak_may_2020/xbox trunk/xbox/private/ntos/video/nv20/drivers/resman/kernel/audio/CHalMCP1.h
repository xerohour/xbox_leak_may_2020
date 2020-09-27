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
* Module: CHalMCP1.h                                                        *
*   All HAL functions specific to MCP1 are implemented here			        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CHALMCP1H_
#define _CHALMCP1H_

class CHalVoiceProc;
class CHalGlobalProc;
class CHalExtendedProc;
class CContextDma;

#define MAX_CLIENTS		        8

class CHalMCP1 : public CHalBase
{
public:
	CHalMCP1(U008, HWINFO_COMMON_FIELDS *);
	~CHalMCP1();

public:
	RM_STATUS Allocate();
	RM_STATUS Free();

    RM_STATUS AddClient(VOID *pParam, U008 *pClientRef);
    RM_STATUS DeleteClient(U008 uClientRef);

    RM_STATUS AllocateContextDma(VOID *pParam);
    RM_STATUS FreeContextDma(VOID *pParam);

    RM_STATUS AllocateObject(U032 uObjectType, VOID *pParam);
    RM_STATUS FreeObject(U032 uObjectType, VOID *pParam);

	RM_STATUS InterruptService();
    
    enum E_STATE
    {
        // mode states
        FREE_RUN = 0,
        // SE modes
        OFF,
        AC_SYNC,
        SW,
        // interrupts enables for SE
        INT_PANIC,
        INT_RETRIGGER,
        INT_WARN,
        // FE modes
        HALT,
        // priv locks
        LOCK,
        UNLOCK
    };

    enum INTERRUPT_STATE
    {
        ENABLE,
        DISABLE
    };

    VOID SetFrontEnd(E_STATE);
    VOID SetSetupEngine(E_STATE);
   	VOID SetInterrupts(INTERRUPT_STATE);
    VOID SetPrivLock(E_STATE);

    CHalVoiceProc   *GetVoiceProc()     { return m_pVoiceProc; }
    CHalGlobalProc  *GetGlobalProc()    { return m_pGlobalProc; }
    CHalExtendedProc *GetExtendedProc() { return m_pExtendedProc; }

protected:
	// public objects
	CHalVoiceProc	    *m_pVoiceProc;
    CHalGlobalProc      *m_pGlobalProc;
    CHalExtendedProc    *m_pExtendedProc;

private:
	VOID OnIntrFETrap();
    VOID UpdateUserMethodQ(U032 uMethod, U032 uData);
	BOOL GetUserMethodInfo(U032 *pMethod, U032 *pData);
	VOID WriteError(U032 uIsCurrentVoice, U008	uErrorCode);
    
    VOID SetISO(APU_AP_FLAGS *pFlags);

    VOID SetSetupEngineGlobalCounts(U032 uExt, U032 uInt, U032 uDelta);
    VOID SetSetupEngineExtendedCounts(U032 uInt, U032 uDelta, U032 uStep);

    VOID CallbackClients(U032 uIntrMask);
    RM_STATUS StoreClientInfo(VOID *pFn, U032 uMask, U032 uContext);
    
    VOID GetDebugInfo();

	R_INTR	m_rInterruptMask;

    CContextDma *m_pCDma;

    MCP1_CLIENT_INFO m_ClientInfo[MAX_CLIENTS];

    MEM_DESC    m_MagicWrite;
    U032        m_uMagicWriteValue;
};

#endif