//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctcurhlp.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctcurhlp.cpp
//
// Test harness implementation of IDirectMusicMediaParamsCurveHelper
//
// Functions:
//    CtIDirectMusicMediaParamsCurveHelper::CtIDirectMusicMediaParamsCurveHelper()
//    CtIDirectMusicMediaParamsCurveHelper::~CtIDirectMusicMediaParamsCurveHelper()
//    CtIDirectMusicMediaParamsCurveHelper::InitTestClass()
//    CtIDirectMusicMediaParamsCurveHelper::GetRealObjPtr()
//  IUnknown
//    CtIDirectMusicMediaParamsCurveHelper::QueryInterface()
//  DX8
//    CtIDirectMusicMediaParamsCurveHelper::Init()
//    CtIDirectMusicMediaParamsCurveHelper::GetValue()
//    CtIDirectMusicMediaParamsCurveHelper::Clone()
//
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicMediaParamsCurveHelper::CtIDirectMusicMediaParamsCurveHelper()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
CtIDirectMusicMediaParamsCurveHelper::CtIDirectMusicMediaParamsCurveHelper()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicMediaParamsCurveHelper";

} //*** end CtIDirectMusicMediaParamsCurveHelper::CtIDirectMusicMediaParamsCurveHelper()




//===========================================================================
// CtIDirectMusicMediaParamsCurveHelper::~CtIDirectMusicMediaParamsCurveHelper()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
CtIDirectMusicMediaParamsCurveHelper::~CtIDirectMusicMediaParamsCurveHelper(void)
{
    // nothing to do

} //*** emd CtIDirectMusicMediaParamsCurveHelper::~CtIDirectMusicMediaParamsCurveHelper()




//===========================================================================
// CtIDirectMusicMediaParamsCurveHelper::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicMediaParamsCurveHelper object pointer for future use.
//
// Parameters:
//  CtIDirectMusicMediaParamsCurveHelper    *pdmHelper    - pointer to real IDirectMusicMediaParamsCurveHelper object
//
// Returns: 
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicMediaParamsCurveHelper::InitTestClass(IDirectMusicMediaParamsCurveHelper *pdmHelper)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmHelper, sizeof(IDirectMusicMediaParamsCurveHelper), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmHelper));

} //*** end CtIDirectMusicMediaParamsCurveHelper::InitTestClass()




//===========================================================================
// CtIDirectMusicMediaParamsCurveHelper::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicMediaParamsCurveHelper **ppdmHelper - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicMediaParamsCurveHelper::GetRealObjPtr(IDirectMusicMediaParamsCurveHelper **ppdmHelper)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmHelper, sizeof(IDirectMusicMediaParamsCurveHelper*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmHelper));

} //*** end CtIDirectMusicMediaParamsCurveHelper::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicMediaParamsCurveHelper::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicMediaParamsCurveHelper::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicMediaParamsCurveHelper::QueryInterface()




//===========================================================================
// CtIDirectMusicMediaParamsCurveHelper::Init()
//
// Encapsulates calls to Init
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicMediaParamsCurveHelper::Init
(
	DWORD cParams,
	DMUS_MEDIAPARAM *prgParamInfo
)
{
    HRESULT     hRes						= E_NOTIMPL;
    BOOL	fValid_prgParamInfo    = TRUE;
    
    // validate prgParamInfo
    if(!helpIsValidPtr((void*)prgParamInfo, sizeof(DMUS_MEDIAPARAM), FALSE))
    {
        fValid_prgParamInfo	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::Init()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "cParams      == %08Xh",
            cParams);
    fnsLog(PARAMLOGLEVEL, "prgParamInfo == %p   %s",
            prgParamInfo,
            fValid_prgParamInfo ? "" : "BAD");
    
    // call the real function
    hRes = ((IDirectMusicMediaParamsCurveHelper*)m_pUnk)->Init(cParams,prgParamInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::Init()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicMediaParamsCurveHelper::Init()




//===========================================================================
// CtIDirectMusicMediaParamsCurveHelper::GetValue()
//
// Encapsulates calls to GetValue
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicMediaParamsCurveHelper::GetValue(DWORD dwParamIndex,REFERENCE_TIME rt,float *pval)
{
    HRESULT	hRes		= E_NOTIMPL;
    BOOL	fValid_pval = TRUE;
    
    // validate pval
    if(!helpIsValidPtr((void*)pval, sizeof(float), FALSE))
    {
        fValid_pval	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetValue()",
            m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "dwParamIndex == %08Xh",
            dwParamIndex);
    fnsLog(PARAMLOGLEVEL, "rt           == %016Xh",
            rt);
    fnsLog(PARAMLOGLEVEL, "pval         == %p   %s",
            pval,
            fValid_pval? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicMediaParamsCurveHelper*)m_pUnk)->GetValue(dwParamIndex,rt,pval);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetValue()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicMediaParamsCurveHelper::GetValue()




//===========================================================================
// CtIDirectMusicMediaParamsCurveHelper::Clone()
//
// Encapsulates calls to Clone
//
// History:
//  12/01/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicMediaParamsCurveHelper::Clone(CtIDirectMusicMediaParamsCurveHelper **ppCopy)
{
    HRESULT	hRes		= E_NOTIMPL;
    BOOL	fValid_ppCopy	= TRUE;
    IDirectMusicMediaParamsCurveHelper*		pdwCopy		= NULL;
    IDirectMusicMediaParamsCurveHelper**	ppdwCopy	= NULL;
    
    // validate ppCopy
    if(!helpIsValidPtr((void*)ppCopy, sizeof(CtIDirectMusicMediaParamsCurveHelper*),FALSE))
    {
        // bogus pointer, use as such
        fValid_ppCopy = FALSE;
        ppdwCopy = (IDirectMusicMediaParamsCurveHelper**)ppCopy;

    }
    else
    {
        // valid pointer, create a real object
        ppdwCopy = &pdwCopy;

        // just in case we fail, init test object ptr to NULL
        *ppCopy = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::Clone()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppCopy         == %p   %s",
            ppdwCopy,
            fValid_ppCopy? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicMediaParamsCurveHelper*)m_pUnk)->Clone(ppdwCopy);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::Clone()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppCopy)
    {
        fnsLog(PARAMLOGLEVEL, "*ppCopy == %p",
                *ppdwCopy);
		// create the test object
		if(SUCCEEDED(hRes))
		{
			hRes = dmthCreateTestWrappedObject(*ppdwCopy,ppCopy);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
			}
		}
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicMediaParamsCurveHelper::Clone()

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




