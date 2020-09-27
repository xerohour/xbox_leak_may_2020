//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       cttool8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// cttool8.cpp
//
// Test harness implementation of IDirectMusicTool8
//
// Functions:
//  CtIDirectMusicTool8::Clone()
//
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicTool8::CtIDirectMusicTool8()
//
// Default constructor
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================
CtIDirectMusicTool8::CtIDirectMusicTool8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicTool8";

} // *** end CtIDirectMusicTool8::CtIDirectMusicTool8()


//===========================================================================
// CtIDirectMusicTool8::~CtIDirectMusicTool8()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================
CtIDirectMusicTool8::~CtIDirectMusicTool8(void)
{
    // nothing to do

} // *** end CtIDirectMusicTool8::~CtIDirectMusicTool8()


//===========================================================================
// CtIDirectMusicTool8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicTool8 *pdmTool8 - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicTool8::InitTestClass(IDirectMusicTool8 *pdmTool8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmTool8, sizeof(IDirectMusicTool8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmTool8));

} // *** end CtIDirectMusicTool8::InitTestClass()


//===========================================================================
// CtIDirectMusicTool8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicTool8 **ppdmTool8 - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicTool8::GetRealObjPtr(IDirectMusicTool8 **ppdmTool8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmTool8, sizeof(IDirectMusicTool8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmTool8));

} // *** end CtIDirectMusicTool8::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicTool8::Clone()
//
// Encapsulates calls to Clone
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicTool8::Clone(CtIDirectMusicTool** ppTool)
{
    HRESULT				hRes			= E_NOTIMPL;
	BOOL				fValid_ppTool	= TRUE;
    IDirectMusicTool*	pdmTool			= NULL;
	IDirectMusicTool**	ppdmTool		= NULL;

    // validate padwMediaTypes
    if(!helpIsValidPtr((void*)ppTool, sizeof(DWORD*), FALSE))
    {
        fValid_ppTool = FALSE;
		ppdmTool = (IDirectMusicTool**)ppTool;
    }
	else
	{
		//use real object pointer
		ppdmTool = &pdmTool;
		*ppTool = NULL;
	}

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Clone()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppTool == %p   %s",
            ppdmTool,
            fValid_ppTool);

    // call the real function
    hRes = ((IDirectMusicTool8*)m_pUnk)->Clone(ppdmTool);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Clone()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmTool, ppTool);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    if(fValid_ppTool && pdmTool)
    {
        pdmTool->Release();
    }

    return hRes;

} // *** end CtIDirectMusicTool8::Clone()
