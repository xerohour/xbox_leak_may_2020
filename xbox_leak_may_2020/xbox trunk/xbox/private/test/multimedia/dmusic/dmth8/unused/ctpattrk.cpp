//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctpattrk.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctpattrk.cpp
//
// Test harness implementation of IDirectMusicPatternTrack
//
// Functions:
//  CtIDirectMusicPatternTrack::CtIDirectMusicPatternTrack()
//  CtIDirectMusicPatternTrack::~CtIDirectMusicPatternTrack()
//  CtIDirectMusicPatternTrack::InitTestClass()
//  CtIDirectMusicPatternTrack::GetRealObjPtr()
//  CtIDirectMusicPatternTrack::QueryInterface()
// IDirectMusicPatternTrack methods
//  CtIDirectMusicPatternTrack::CreateSegment()
//  CtIDirectMusicPatternTrack::etVariation()
//  CtIDirectMusicPatternTrack::SetPatternByName()
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicPatternTrack::CtIDirectMusicPatternTrack()
//
// Default constructor
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
CtIDirectMusicPatternTrack::CtIDirectMusicPatternTrack(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicPatternTrack";

} //*** end CtIDirectMusicPatternTrack::CtIDirectMusicPatternTrack()


//===========================================================================
// CtIDirectMusicPatternTrack::~CtIDirectMusicPatternTrack()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
CtIDirectMusicPatternTrack::~CtIDirectMusicPatternTrack(void)
{
    // nothing to do

} //*** end CtIDirectMusicPatternTrack::~CtIDirectMusicPatternTrack()


//===========================================================================
// CtIDirectMusicPatternTrack::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPatternTrack *pdmPatternTrack - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPatternTrack::InitTestClass(IDirectMusicPatternTrack *pdmPatternTrack)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmPatternTrack, sizeof(IDirectMusicPatternTrack), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmPatternTrack));

} //*** end CtIDirectMusicPatternTrack::InitTestClass()


//===========================================================================
// CtIDirectMusicPatternTrack::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	CtIDirectMusicPatternTrack **ppdmPatternTrack - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPatternTrack::GetRealObjPtr(IDirectMusicPatternTrack **ppdmPatternTrack)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmPatternTrack, sizeof(IDirectMusicPatternTrack*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmPatternTrack));

} //*** end CtIDirectMusicPatternTrack::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicPatternTrack::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPatternTrack::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicPatternTrack::QueryInterface()




//===========================================================================
// CtIDirectMusicPatternTrack::CreateSegment()
//
// Encapsulates calls to CreateSegment
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPatternTrack::CreateSegment
(
	CtIDirectMusicStyle* pStyle,
	CtIDirectMusicSegment** ppSegment
)
{        
    HRESULT					hRes				= E_NOTIMPL;
	BOOL					fValid_pStyle		= TRUE;
	BOOL					fValid_ppSegment	= TRUE;
    IDirectMusicStyle*		pdmStyle			= NULL; 
    IDirectMusicSegment*	pdmSegment			= NULL;
    IDirectMusicSegment**	ppdmSegment			= NULL;

    // validate pStyle
    if(!helpIsValidPtr((void*)pStyle, sizeof(CtIDirectMusicStyle), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pStyle = FALSE;
        pdmStyle = (IDirectMusicStyle*)pStyle;

    }
    else
    {
        // good pointer, get the real object
        hRes = pStyle->GetRealObjPtr(&pdmStyle);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ppSegment
    if(!helpIsValidPtr((void*)ppSegment, sizeof(CtIDirectMusicSegment*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ppSegment = FALSE;
        ppdmSegment = (IDirectMusicSegment**)ppSegment;

    }
    else
    {
        // valid pointer, create a real object
        ppdmSegment = &pdmSegment;

        // just in case we fail, init test object ptr to NULL
        *ppSegment = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::CreateSegment()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStyle       == %p   %s",
            pdmStyle,
            fValid_pStyle ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppSegment    == %p   %s",
            ppdmSegment,
            fValid_ppSegment ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPatternTrack*)m_pUnk)->CreateSegment(
				pdmStyle,
				ppdmSegment
				);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::CreateSegment()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppSegment)
    {
        fnsLog(PARAMLOGLEVEL, "*ppSegment == %p",
                pdmSegment);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmSegment, ppSegment);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();
END:
    // done
    if(fValid_pStyle && pdmStyle)
    {
        pdmStyle->Release();
    }
    if(fValid_ppSegment && pdmSegment)
    {
        pdmSegment->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPatternTrack::CreateSegment()





//===========================================================================
// CtIDirectMusicPatternTrack::SetVariation()
//
// Encapsulates calls to SetVariation
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPatternTrack::SetVariation
(
	CtIDirectMusicSegmentState* pSegState,
	DWORD dwVariationFlags,
	DWORD dwPart
)
{        
    HRESULT						hRes				= E_NOTIMPL;
	BOOL						fValid_pSegState	= TRUE;
    IDirectMusicSegmentState*	pdmSegState			= NULL; 

    // validate pSegState
    if(!helpIsValidPtr((void*)pSegState, sizeof(CtIDirectMusicSegmentState), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pSegState = FALSE;
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetVariation()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSegState        == %p   %s",
            pdmSegState,
            fValid_pSegState ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwVariationFlags == %08Xh",
            dwVariationFlags);
    fnsLog(PARAMLOGLEVEL, "dwPart         == %08Xh",
            dwPart);

    // call the real function
    hRes = ((IDirectMusicPatternTrack*)m_pUnk)->SetVariation(pdmSegState,dwVariationFlags,dwPart);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetVariation()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();
END:
    // done
    if(fValid_pSegState && pdmSegState)
    {
        pdmSegState->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPatternTrack::SetVariation()




//===========================================================================
// CtIDirectMusicPatternTrack::SetPatternByName()
//
// Encapsulates calls to SetPatternByName
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPatternTrack::SetPatternByName
(
	CtIDirectMusicSegmentState* pSegState,
	WCHAR* wszName,
	CtIDirectMusicStyle* pStyle,
	DWORD dwPatternType,
	DWORD* pdwLength
)
{        
    HRESULT						hRes				= E_NOTIMPL;
	BOOL						fValid_pSegState	= TRUE;
    BOOL						fValid_wszName		= TRUE;
	BOOL						fValid_pStyle		= TRUE;
    BOOL						fValid_pdwLength	= TRUE;
    IDirectMusicSegmentState*	pdmSegState			= NULL; 
    IDirectMusicStyle*			pdmStyle			= NULL; 

    // validate pSegState
    if(!helpIsValidPtr((void*)pSegState, sizeof(CtIDirectMusicSegmentState), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pSegState = FALSE;
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

    // validate wszName
    if(!helpIsValidPtr((void*)wszName, sizeof(WCHAR), FALSE))
    {
        fValid_wszName	= FALSE;
    }

    // validate pStyle
    if(!helpIsValidPtr((void*)pStyle, sizeof(CtIDirectMusicStyle), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pStyle = FALSE;
        pdmStyle = (IDirectMusicStyle*)pStyle;

    }
    else
    {
        // good pointer, get the real object
        hRes = pStyle->GetRealObjPtr(&pdmStyle);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate pdwLength
    if(!helpIsValidPtr((void*)pdwLength, sizeof(DWORD), FALSE))
    {
        fValid_pdwLength	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetPatternByName()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSegState     == %p   %s",
            pdmSegState,
            fValid_pSegState ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "wszName       == %p   %s",
            wszName,
            fValid_wszName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pStyle        == %p   %s",
            pdmStyle,
            fValid_pStyle ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwPatternType == %08Xh",
            dwPatternType);
    fnsLog(PARAMLOGLEVEL, "pdwLength     == %p   %s",
            pdwLength,
            fValid_pdwLength ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPatternTrack*)m_pUnk)->SetPatternByName(
													pdmSegState,
													wszName,
													pdmStyle,
													dwPatternType,
													pdwLength
													);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetPatternByName()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwLength)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwLength == %p",
                pdwLength);
    }

    fnsDecrementIndent();
END:
    // done
    if(fValid_pSegState && pdmSegState)
    {
        pdmSegState->Release();
    }
    if(fValid_pStyle && pdmStyle)
    {
        pdmStyle->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPatternTrack::SetPatternByName()



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






