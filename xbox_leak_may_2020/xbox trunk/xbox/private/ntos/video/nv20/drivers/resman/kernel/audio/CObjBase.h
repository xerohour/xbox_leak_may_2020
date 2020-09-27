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
* Module: CObjBase.h                                                        *
*   All functions common to diff HALs are implemented here			        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _COBJBASEH_
#define _COBJBASEH_

class CHalBase;

class CObjBase
{
public:
	CObjBase(CHalBase *pHal)	{ m_pHal = pHal; }

public:
	// each derived object has to implement these functions
	virtual RM_STATUS Allocate(VOID *pParam) = 0;
	virtual RM_STATUS Free(VOID *pParam = NULL) = 0;
    
    // each derived object also has to make a method handler
    // for any software methods associated with the object

protected:
	
	virtual VOID RegWrite(U032 uOffset, U032 uValue) { m_pHal->RegWrite(uOffset, uValue); }
	virtual VOID RegWrite(U032 uOffset, U016 uValue) { m_pHal->RegWrite(uOffset, uValue); }
	virtual VOID RegWrite(U032 uOffset, U008 uValue) { m_pHal->RegWrite(uOffset, uValue); }

	virtual VOID RegRead(U032 uOffset, U032 *pValue) { m_pHal->RegRead(uOffset, pValue); }
	virtual VOID RegRead(U032 uOffset, U016 *pValue) { m_pHal->RegRead(uOffset, pValue); }
	virtual VOID RegRead(U032 uOffset, U008 *pValue) { m_pHal->RegRead(uOffset, pValue); }
	virtual VOID RegRead(U032 uOffset, volatile U032 *pValue) { m_pHal->RegRead(uOffset, pValue); }
	virtual VOID RegRead(U032 uOffset, volatile U016 *pValue) { m_pHal->RegRead(uOffset, pValue); }
	virtual VOID RegRead(U032 uOffset, volatile U008 *pValue) { m_pHal->RegRead(uOffset, pValue); }

    virtual void MemWrite(U032 uAddr, U008 uData)    { 	m_pHal->MemWrite(uAddr, uData);   }
    virtual void MemWrite(U032 uAddr, U016 uData)    { 	m_pHal->MemWrite(uAddr, uData);   }
    virtual void MemWrite(U032 uAddr, U032 uData)    { 	m_pHal->MemWrite(uAddr, uData);   }

    CHalBase *GetDevice()           { return m_pHal; }

private:
	CHalBase *m_pHal;
};

#endif