//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctbuffer.cpp
//
//--------------------------------------------------------------------------

//==========================================================================
// ctbuffer.cpp
//
// Test harness implementation of IDirectMusicBuffer
//
// Functions:
//  CtIDirectMusicBuffer::CtIDirectMusicBuffer()
//  CtIDirectMusicBuffer::~CtIDirectMusicBuffer()
//  CtIDirectMusicBuffer::InitTestClass()
//  CtIDirectMusicBuffer::GetRealObjPtr()
//  CtIDirectMusicBuffer::QueryInterface()
//  CtIDirectMusicBuffer::Flush()
//  CtIDirectMusicBuffer::TotalTime()
//  CtIDirectMusicBuffer::PackStructured()
//  CtIDirectMusicBuffer::PackUnstructured()
//  CtIDirectMusicBuffer::ResetReadPtr()
//  CtIDirectMusicBuffer::GetNextEvent()
//  CtIDirectMusicBuffer::GetRawBufferPtr()
//  CtIDirectMusicBuffer::GetStartTime()
//  CtIDirectMusicBuffer::GetUsedBytes()
//  CtIDirectMusicBuffer::GetMaxBytes()
//  CtIDirectMusicBuffer::SetStartTime()
//  CtIDirectMusicBuffer::SetUsedBytes()
//  CtIDirectMusicBuffer::GetBufferFormat()
//
// History:
//  10/17/1997 - davidkl - created
//  11/14/1997 - davidkl - changed in response to new dmusic.h
//	01/19/1998 - a-llucar - changed to latest core changes
//  03/24/1998 - davidkl - brave new world...  now a derived class
//  04/01/1998 - davidkl - minor tweaks
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicBuffer::CtIDirectMusicBuffer
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/17/1997 - davidkl - created
//===========================================================================
CtIDirectMusicBuffer::CtIDirectMusicBuffer(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicBuffer";

} //*** end CtIDirectMusicBuffer::CtIDirectMusicBuffer()


//===========================================================================
// CtIDirectMusicBuffer::~CtIDirectMusicBuffer
//
// Default destructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/17/1997 - davidkl - created
//===========================================================================
CtIDirectMusicBuffer::~CtIDirectMusicBuffer(void)
{
    // nothing to do

} //*** end CtIDirectMusicBuffer::~CtIDirectMusicBuffer()


//===========================================================================
// CtIDirectMusicBuffer::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicBuffer object pointer for future use.
//
// Parameters:
//  IDirectMusicBuffer  *pdmBuf - pointer to real IDirectMusicBuffer object
//
// Returns: 
//
// History:
//  10/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::InitTestClass(IDirectMusicBuffer *pdmBuf)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmBuf, sizeof(IDirectMusicBuffer), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmBuf));

} //*** end CtIDirectMusicBuffer::InitTestClass()


//===========================================================================
// CtIDirectMusicBuffer::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicBuffer **ppdmBuf - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//	10/17/1997 - davidkl - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicBuffer::GetRealObjPtr(IDirectMusicBuffer **ppdmBuf)
{

	// validate ppdmBuf
	if(!helpIsValidPtr(ppdmBuf, sizeof(IDirectMusicBuffer*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmBuf));

} //*** end CtIDirectMusicBuffer::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicBuffer::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//  HRESULT - return code from real QueryInterface
//
// History:
//  10/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    HRESULT hRes        = E_NOTIMPL;
    LPVOID  *ppvTemp    = NULL;
    BOOL    fMatchFound = FALSE;

    // BUGBUG - validate ppvObj

    // BUGBUG - this will need to do some wrapper class creation
    // based on test harness specific IIDs

    // use the base class to perform the actual QI
    hRes = (CtIUnknown::QueryInterface(riid, ppvObj));

    // BUGBUG - iff harness IID, wrap the interface returned

    // BUGBUG - otherwise, copy ppvTemp to ppvObj

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::QueryInterface()


//===========================================================================
// CtIDirectMusicBuffer::Flush
//
// Encapsulates calls to Flush
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real Flush
//
// History:
//  10/20/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::Flush(void)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Flush()",
            m_szInterfaceName);

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->Flush();

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Flush()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    //done
    return hRes;

} //*** end CtIDirectMusicBuffer::Flush()


//===========================================================================
// CtIDirectMusicBuffer::TotalTime
//
// Encapsulates calls to TotalTime
//
// Parameters:
//
// Returns:
//  HRESULT - return code from real TotalTime
//
// History:
//  10/20/1997 - davidkl - created
//  11/14/1997 - davidkl - changed in response to new dmusic.h
//===========================================================================
HRESULT CtIDirectMusicBuffer::TotalTime(LPREFERENCE_TIME prtTime)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_prtTime  = TRUE;

    // validate pdwTime
    if(!helpIsValidPtr((void*)prtTime, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prtTime = FALSE;
    }
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::TotalTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "prtTime == %p   %s",
            prtTime,
            fValid_prtTime ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->TotalTime(prtTime);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::TotalTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes     == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_prtTime)
    {
        fnsLog(PARAMLOGLEVEL, "*prtTime == %016Xh",
                *prtTime);
    }

    fnsDecrementIndent();

    //done
    return hRes;

} //*** end CtIDirectMusicBuffer::TotalTime()


//===========================================================================
// CtIDirectMusicBuffer::PackStructured
//
// Encapsulates calls to PackStructured
//
// Parameters:
//
// Returns:
//  HRESULT - return code from real PackStructured
//
// History:
//  10/24/1997 - davidkl - created
//  11/14/1997 - davidkl - changed in response to new dmusic.h
//===========================================================================
HRESULT CtIDirectMusicBuffer::PackStructured(REFERENCE_TIME rt, 
                                            DWORD dwChannelGroup,
                                            DWORD dwMsg)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::PackStructured()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rt             == %016Xh",
            rt);
    fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh",
            dwChannelGroup);
    fnsLog(PARAMLOGLEVEL, "dwMsg          == %08Xh",
            dwMsg);

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->PackStructured(rt, dwChannelGroup, dwMsg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::PackStructured()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::PackStructured()


//===========================================================================
// CtIDirectMusicBuffer::PackUnstructured
//
// Encapsulates calls to PackUnstructured
//
// Parameters:
//
// Returns:
//  HRESULT - return code from real PackUnstructured
//
// History:
//  10/24/1997 - davidkl - created
//  11/14/1997 - davidkl - changed in response to new dmusic.h
//===========================================================================
HRESULT CtIDirectMusicBuffer::PackUnstructured(REFERENCE_TIME rt, 
                                        DWORD dwChannelGroup,
                                        DWORD cb, 
                                        LPBYTE lpb)
{
    HRESULT hRes        = E_NOTIMPL;
    BOOL    fValid_lpb  = TRUE;

    // validate lpb
    if(!helpIsValidPtr((void*)lpb, sizeof(BYTE) * cb, FALSE))
    {
        fValid_lpb = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::PackUnstructured()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rt             == %016Xh",
            rt);
    fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh",
            dwChannelGroup);
    fnsLog(PARAMLOGLEVEL, "cb             == %08Xh",
            cb);
    fnsLog(PARAMLOGLEVEL, "lpb            == %p   %s",
            lpb,
            fValid_lpb ? "" : "BAD");
    // BUGBUG log contents of lpb?

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->PackUnstructured(rt, dwChannelGroup, cb, lpb);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::PackUnstructured)",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::PackUnstructured()


//===========================================================================
// CtIDirectMusicBuffer::ResetReadPtr
//
// Encapsulates calls to ResetReadPtr
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real ResetReadPtr
//
// History:
//  10/20/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::ResetReadPtr(void)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ResetReadPtr()",
            m_szInterfaceName);

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->ResetReadPtr();

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ResetReadPtr()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    //done
    return hRes;

} //*** end CtIDirectMusicBuffer::ResetReadPtr()


//===========================================================================
// CtIDirectMusicBuffer::GetNextEvent
//
// Encapsulates calls to GetNextEvent
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real GetNextEvent
//
// History:
//  11/14/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::GetNextEvent(LPREFERENCE_TIME prt, 
                                            LPDWORD pdwChannelGroup,
                                            LPDWORD pdwLength, 
                                            LPBYTE *ppData)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_prt              = TRUE;
    BOOL    fValid_pdwChannelGroup  = TRUE;
    BOOL    fValid_pdwLength        = TRUE;
    BOOL    fValid_ppData           = TRUE;

    // validate prt
    if(!helpIsValidPtr((void*)prt, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prt = FALSE;
    }
    // validate pdwChannelGroup
    if(!helpIsValidPtr((void*)pdwChannelGroup, sizeof(DWORD), FALSE))
    {
        fValid_pdwChannelGroup = FALSE;
    }
    // validate pdwLength
    if(!helpIsValidPtr((void*)pdwLength, sizeof(DWORD), FALSE))
    {
        fValid_pdwLength = FALSE;
    }
    // validate ppData
    if(!helpIsValidPtr((void*)ppData, sizeof(LPBYTE), FALSE))
    {
        fValid_ppData = FALSE;       
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetNextEvent()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "prt             == %p   %s",
            prt,
            fValid_prt ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdwChannelGroup == %p   %s",
            pdwChannelGroup,
            fValid_pdwChannelGroup ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdwLength       == %p   %s",
            pdwLength,
            fValid_pdwLength ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppData          == %p   %s",
            ppData,
            fValid_ppData ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->GetNextEvent(prt, pdwChannelGroup, pdwLength, ppData);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetNextEvent()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes             == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_prt)
    {
        fnsLog(PARAMLOGLEVEL, "*prt             == %016Xh",
                *prt);
    }
    if(fValid_pdwChannelGroup)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwChannelGroup == %08Xh",
                *pdwChannelGroup);
    }
    if(fValid_pdwLength)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwLength       == %08Xh",
                *pdwLength);
    }
//    if(fValid_ppData)
//    {
// BUGBUG - need to use length to log all data correctly
//        fnsLog(PARAMLOGLEVEL, "*ppData          == %08Xh",
//                *ppData);
//    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::GetNextEvent()


//===========================================================================
// CtIDirectMusicBuffer::GetRawBufferPtr
//
// Encapsulates calls to GetRawBufferPtr
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real GetRawBufferPtr
//
// History:
//  11/14/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::GetRawBufferPtr(LPBYTE *ppData)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_ppData   = TRUE;

    // validate ppData
    if(!helpIsValidPtr((void*)ppData, sizeof(LPBYTE), FALSE))
    {
        fValid_ppData = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetRawBufferPtr()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppData == %p   %s",
            ppData,
            fValid_ppData ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->GetRawBufferPtr(ppData);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetRawBufferPtr()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes    == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
//    if(fValid_ppData)
//    {
//        fnsLog(PARAMLOGLEVEL, "*ppData == %08Xh",
//                *ppData);
        // BUGBUG log contents of buffer pointed to by *ppData??
//    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::GetRawBufferPtr()


//===========================================================================
// CtIDirectMusicBuffer::GetStartTime
//
// Encapsulates calls to GetStartTime
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real GetStartTime
//
// History:
//  11/14/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::GetStartTime(LPREFERENCE_TIME prt)
{
    HRESULT hRes        = E_NOTIMPL;
    BOOL    fValid_prt  = TRUE;

    // validate prt
    if(!helpIsValidPtr((void*)prt, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prt = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetStartTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "prt == %p   %s",
            prt,
            fValid_prt ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->GetStartTime(prt);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetStartTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_prt)
    {
        fnsLog(PARAMLOGLEVEL, "*prt == %016Xh",
                *prt);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::GetStartTime()


//===========================================================================
// CtIDirectMusicBuffer::GetUsedBytes
//
// Encapsulates calls to GetUsedBytes
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real GetUsedBytes
//
// History:
//  11/14/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::GetUsedBytes(LPDWORD pcb)
{
    HRESULT hRes        = E_NOTIMPL;
    BOOL    fValid_pcb  = TRUE;

    // validate pcb
    if(!helpIsValidPtr((void*)pcb, sizeof(DWORD), FALSE))
    {
        fValid_pcb = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetUsedBytes()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pcb == %p   %s",
            pcb,
            fValid_pcb ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->GetUsedBytes(pcb);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetStartTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pcb)
    {
        fnsLog(PARAMLOGLEVEL, "*pcb == %08Xh",
                *pcb);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::GetUsedBytes()


//===========================================================================
// CtIDirectMusicBuffer::GetMaxBytes
//
// Encapsulates calls to GetMaxBytes
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real GetMaxBytes
//
// History:
//  11/14/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::GetMaxBytes(LPDWORD pcb)
{
    HRESULT hRes    = E_NOTIMPL;
    BOOL    fValid_pcb  = TRUE;

    // validate pcb
    if(!helpIsValidPtr((void*)pcb, sizeof(DWORD), FALSE))
    {
        fValid_pcb = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetMaxBytes()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pcb == %08Xh   %s",
            pcb,
            fValid_pcb ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->GetMaxBytes(pcb);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetMaxBytes()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pcb)
    {
        fnsLog(PARAMLOGLEVEL, "*pcb == %08Xh",
                *pcb);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::GetMaxBytes()


//===========================================================================
// CtIDirectMusicBuffer::SetStartTime
//
// Encapsulates calls to SetStartTime
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real SetStartTime
//
// History:
//  11/14/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::SetStartTime(REFERENCE_TIME rt)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetStartTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rt == %016Xh",
            rt);

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->SetStartTime(rt);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetStartTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::SetStartTime()


//===========================================================================
// CtIDirectMusicBuffer::SetUsedBytes
//
// Encapsulates calls to SetUsedBytes
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real SetUsedBytes
//
// History:
//  11/14/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::SetUsedBytes(DWORD cb)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetUsedBytes()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "cb == %08Xh",
            cb);

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->SetUsedBytes(cb);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetUsedBytes()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicBuffer::SetUsedBytes()


//===========================================================================
// CtIDirectMusicBuffer::GetBufferFormat
//
// Encapsulates calls to GetBufferFormat
//
// Parameters: nothing
//
// Returns:
//  HRESULT - return code from real GetBufferFormat
//
// History:
//  03/02/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicBuffer::GetBufferFormat(LPGUID pGuidFormat)
{
    HRESULT hRes                = E_NOTIMPL;
    BOOL    fValid_pGuidFormat  = TRUE;
    char    szGuid[MAX_LOGSTRING];

    // validate pGuidFormat
    if(!helpIsValidPtr((void*)pGuidFormat, sizeof(GUID), FALSE))
    {
        fValid_pGuidFormat = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetBufferFormat()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pGuidFormat == %p   %s",
            pGuidFormat,
            (fValid_pGuidFormat) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBuffer*)m_pUnk)->GetBufferFormat(pGuidFormat);
    
    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetBufferFormat()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes         == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pGuidFormat)
    {
        tdmGUIDtoString(*pGuidFormat, szGuid);
        fnsLog(PARAMLOGLEVEL, "*pGuidFormat == %s (%s)",
                szGuid, tdmXlatGUID(*pGuidFormat));
    }

    fnsDecrementIndent();

    // done
    return hRes;
   
} //*** end CtIDirectMusicBuffer::GetBufferFormat()


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================















