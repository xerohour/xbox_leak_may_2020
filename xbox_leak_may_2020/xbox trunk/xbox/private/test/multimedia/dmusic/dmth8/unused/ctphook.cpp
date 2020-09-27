//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctphook.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctphook.cpp
//
// Test harness implementation of IDirectMusicParamHook
//
// Functions:
//  CtIDirectMusicParamHook::CtIDirectMusicParamHook()
//  CtIDirectMusicParamHook::~CtIDirectMusicParamHook()
//  CtIDirectMusicParamHook::InitTestClass()
//  CtIDirectMusicParamHook::GetRealObjPtr()
//  CtIDirectMusicParamHook::QueryInterface()
//  CtIDirectMusicParamHook::GetParam()
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicParamHook::CtIDirectMusicParamHook()
//
// Default constructor
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
CtIDirectMusicParamHook::CtIDirectMusicParamHook(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicParamHook";

} //*** end CtIDirectMusicParamHook::CtIDirectMusicParamHook()


//===========================================================================
// CtIDirectMusicParamHook::~CtIDirectMusicParamHook()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
CtIDirectMusicParamHook::~CtIDirectMusicParamHook(void)
{
    // nothing to do

} //*** end CtIDirectMusicParamHook::~CtIDirectMusicParamHook()


//===========================================================================
// CtIDirectMusicParamHook::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicParamHook *pdmParamHook - pointer to real 
//                                      IDirectMusicParamHook object
//
// Returns: 
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicParamHook::InitTestClass(IDirectMusicParamHook *pdmParamHook)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmParamHook, sizeof(IDirectMusicParamHook), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmParamHook));

} //*** end CtIDirectMusicParamHook::InitTestClass()


//===========================================================================
// CtIDirectMusicParamHook::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	CtIDirectMusicParamHook **ppdmParamHook - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicParamHook::GetRealObjPtr(IDirectMusicParamHook **ppdmParamHook)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmParamHook, sizeof(IDirectMusicParamHook*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmParamHook));

} //*** end CtIDirectMusicParamHook::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicParamHook::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicParamHook::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicParamHook::QueryInterface()


//===========================================================================
// CtIDirectMusicParamHook::GetParam()
//
// Encapsulates calls to GetParam
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicParamHook::GetParam
(
	REFGUID rguidType, 
	DWORD dwGroupBits, 
	DWORD dwIndex, 
	MUSIC_TIME mtTime, 
	MUSIC_TIME* pmtNext, 
	void* pData,
	CtIDirectMusicSegmentState *pSegState,
	DWORD dwTrackFlags,
	HRESULT hr
)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL	fValid_pmtNext		= TRUE;
    BOOL	fValid_pData		= TRUE;
    BOOL	fValid_pSegState    = TRUE;
	IDirectMusicSegmentState*	pdmSegState = NULL;
    
    // validate pmtNext
    if(!helpIsValidPtr((void*)pmtNext, sizeof(MUSIC_TIME*), FALSE))
    {
        fValid_pmtNext	= FALSE;
    }

    // validate pData
    if(!helpIsValidPtr((void*)pData, sizeof(void*), FALSE))
    {
        fValid_pData	= FALSE;
    }

    // validate pSegState
    if(!helpIsValidPtr((void*)pSegState, sizeof(CtIDirectMusicSegmentState*), FALSE))
    {
        fValid_pSegState	= FALSE;
		pdmSegState = (IDirectMusicSegmentState*)pSegState;
    }
	else
	{
        // good pointer, get the real object
        hRes = pSegState->GetRealObjPtr(&pdmSegState);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
	}

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetParam()",
		m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "dwGroupBits  == %08Xh",
		dwGroupBits);
	fnsLog(PARAMLOGLEVEL, "dwIndex      == %08Xh",
		dwIndex);
	fnsLog(PARAMLOGLEVEL, "mtTime       == %016Xh",
		mtTime);
	fnsLog(PARAMLOGLEVEL, "pmtNext      == %p   %s",
		pmtNext,fValid_pmtNext ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "pSegState  == %p   %s",
		pdmSegState,fValid_pSegState ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "dwTrackFlags == %08Xh",
		dwTrackFlags);
	fnsLog(PARAMLOGLEVEL, "hr           == %08Xh (%s)",
		hr,tdmXlatHRESULT(hr));
	
    // call the real function
    hRes = ((IDirectMusicParamHook*)m_pUnk)->GetParam(
				rguidType, 
				dwGroupBits, 
				dwIndex, 
				mtTime, 
				pmtNext, 
				pData,
				pdmSegState,
				dwTrackFlags,
				hr);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

END:
    if(fValid_pSegState && pdmSegState)
    {
        pdmSegState->Release();
    }
    // done
    return hRes;

} //*** end CtIDirectMusicParamHook::GetParam()

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






