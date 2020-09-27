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
* Module: CHalBase.h                                                        *
*   All functions common to diff HALs are implemented here			        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CHALBASEH_
#define _CHALBASEH_

class CHalBase
{
public:
    CHalBase::CHalBase(U008 uRevID, HWINFO_COMMON_FIELDS *pDev);
    virtual CHalBase::~CHalBase();

	virtual RM_STATUS Allocate() = 0;
	virtual RM_STATUS Free() = 0;

    virtual RM_STATUS AddClient(VOID *pParam, U008 *pClientRef) = 0;
    virtual RM_STATUS DeleteClient(U008 uClientRef) = 0;

    // context dma
    virtual RM_STATUS AllocateContextDma(VOID *pParam) { return ErrorFn(); }
    virtual RM_STATUS FreeContextDma(VOID *pParam) { return ErrorFn(); }
    
    virtual RM_STATUS AllocateObject(U032 uObjectType, VOID *pParam) { return ErrorFn(); }
    virtual RM_STATUS FreeObject(U032 uObjectType, VOID *pParam) { return ErrorFn(); }

	// the ISR function called at interrupt time
	virtual RM_STATUS InterruptService() = 0;


	virtual VOID RegWrite(U032 uOffset, U032 uValue) { *(U032*)(m_uBase + uOffset) = uValue; }
	virtual VOID RegWrite(U032 uOffset, U016 uValue) { *(U016*)(m_uBase + uOffset) = uValue; }
	virtual VOID RegWrite(U032 uOffset, U008 uValue) { *(U008*)(m_uBase + uOffset) = uValue; }
	
	virtual VOID RegRead(U032 uOffset, U032 *pValue) { *pValue = *(U032*)(m_uBase + uOffset); }
	virtual VOID RegRead(U032 uOffset, U016 *pValue) { *pValue = *(U016*)(m_uBase + uOffset); }
	virtual VOID RegRead(U032 uOffset, U008 *pValue) { *pValue = *(U008*)(m_uBase + uOffset); }

	virtual VOID RegRead(U032 uOffset, volatile U032 *pValue) { *pValue = *(U032*)(m_uBase + uOffset); }
	virtual VOID RegRead(U032 uOffset, volatile U016 *pValue) { *pValue = *(U016*)(m_uBase + uOffset); }
	virtual VOID RegRead(U032 uOffset, volatile U008 *pValue) { *pValue = *(U008*)(m_uBase + uOffset); }
    

    virtual VOID MemWrite(U032 uAddr, U008 uData)    { 	*((U008 *)uAddr) = uData;   }
    virtual VOID MemWrite(U032 uAddr, U016 uData)    { 	*((U016 *)uAddr) = uData;   }
    virtual VOID MemWrite(U032 uAddr, U032 uData)    { 	*((U032 *)uAddr) = uData;   }

	U008                    GetRevID()	{ return m_uRevID; }
	HWINFO_COMMON_FIELDS *  GetpDev()   { return m_pDev; }
    U032                    GetDevBase() { return m_uBase; }

protected:
    VOID DumpChipToFile();

private:

    RM_STATUS   ErrorFn()
    {
        // the called function HAS to be
        // either defined in the dervied class,
        // or the call is on the wrong object
#if defined DEBUG
        aosDbgBreakPoint();
#endif
        return RM_ERROR;
    }


	U008 m_uRevID;
	U032 m_uBase;
	HWINFO_COMMON_FIELDS *m_pDev;
};


#endif