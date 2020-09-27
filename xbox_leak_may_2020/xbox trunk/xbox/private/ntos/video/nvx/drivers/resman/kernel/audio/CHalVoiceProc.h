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
* Module: CHalVoiceProc.h                                                   *
*   All functions common to diff HALs are implemented here			        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CHALVOICEPROCH_
#define _CHALVOICEPROCH_

class CHalBase;
class CObjBase;

class CHalVoiceProc : public CObjBase
{
public:
    CHalVoiceProc(CHalBase *pHal) : CObjBase(pHal) { };
	~CHalVoiceProc() { };

public:

	// each derived object has to implement these functions
	RM_STATUS Allocate(VOID *pParam);
	RM_STATUS Free(VOID *pParam = NULL);
	RM_STATUS GetCaps(VOID *pParam);
    
    void *  GetSgeInLinAddr()   { return m_SgeIn.pLinAddr; }
    U032    GetSgeInSize()      { return m_SgeIn.Size; }

    void *  GetSegInLinAddr()   { return m_SegIn.pLinAddr; }
    U032    GetSegInSize()      { return m_SegIn.Size; }

private:
	VOID InitializeHardware(APU_AP_CREATE *);

    RM_STATUS AllocateMemory(APU_AP_CREATE *);
    VOID FreeMemory();

    MEM_DESC    m_Voice;
    MEM_DESC    m_SgeIn;
    MEM_DESC    m_SegIn;
    MEM_DESC    m_HRTFTarg;
    MEM_DESC    m_HRTFCurr;
};

#endif