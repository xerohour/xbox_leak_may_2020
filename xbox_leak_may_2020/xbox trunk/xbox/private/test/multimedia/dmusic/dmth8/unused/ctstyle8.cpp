//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctstyle8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctstyle8.cpp
//
// Test harness implementation of IDirectMusicStyle8
//
// Functions:
//  CtIDirectMusicStyle8::EnumPattern()
//	CtIDirectMusicStyle8::ComposeMelodyFromTemplate()
//
//
// History:
//  10/15/1999 - kcraven - created
//  04/06/2000 - danhaff - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicStyle8::CtIDirectMusicStyle8()
//
// Default constructor
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================
CtIDirectMusicStyle8::CtIDirectMusicStyle8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicStyle8";

} // *** end CtIDirectMusicStyle8::CtIDirectMusicStyle8()




//===========================================================================
// CtIDirectMusicStyle8::~CtIDirectMusicStyle8()
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
CtIDirectMusicStyle8::~CtIDirectMusicStyle8(void)
{
    // nothing to do

} // *** end CtIDirectMusicStyle8::~CtIDirectMusicStyle8()




//===========================================================================
// CtIDirectMusicStyle8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicStyle8 pointer for future use.
//
// Parameters:
//  IDirectMusicStyle8 *pdmStyle8 - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicStyle8::InitTestClass(IDirectMusicStyle8 *pdmStyle8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmStyle8, sizeof(IDirectMusicStyle8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmStyle8));

} // *** end CtIDirectMusicStyle8::InitTestClass()




//===========================================================================
// CtIDirectMusicStyle8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicStyle8 **ppdmStyle8 - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicStyle8::GetRealObjPtr(IDirectMusicStyle8 **ppdmStyle8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmStyle8, sizeof(IDirectMusicStyle8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmStyle8));

} // *** end CtIDirectMusicStyle8::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicStyle8::EnumPattern()
//
// Encapsulates calls to EnumPattern
//
// History:
//  10/15/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicStyle8::EnumPattern(DWORD dwIndex,DWORD dwPatternType,WCHAR* pwszName)
{
    HRESULT hRes			= E_NOTIMPL;
	BOOL	fValid_pwszName	= TRUE;
    
    // validate pTempo
    if(!helpIsValidPtr((void*)pwszName, sizeof(double), FALSE))
    {
        fValid_pwszName = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumPattern()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIndex        == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "dwPatternType  == %08Xh",
            dwPatternType);
    fnsLog(PARAMLOGLEVEL, "pwszName       == %p   %s",
            pwszName,
            fValid_pwszName ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle8*)m_pUnk)->EnumPattern(dwIndex,dwPatternType,pwszName);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumPattern()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes     == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
	if(SUCCEEDED(hRes) && fValid_pwszName && (hRes != S_FALSE) && (wcslen(pwszName) > 0))
	{
	    fnsLog(PARAMLOGLEVEL, "pwszName == %ls",pwszName);
	}

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicStyle8::EnumPattern()




//===========================================================================
// CtIDirectMusicStyle8::ComposeMelodyFromTemplate()
//
// Encapsulates calls to ComposeMelodyFromTemplate
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
/*
HRESULT CtIDirectMusicStyle8::ComposeMelodyFromTemplate
(
	CtIDirectMusicStyle* pStyle, 
	CtIDirectMusicSegment* pTemplate, 
	CtIDirectMusicSegment** ppSegment
)
{
    HRESULT                 hRes				= E_NOTIMPL;
	BOOL				    fValid_pStyle		= TRUE;
    BOOL				    fValid_pTemplate	= TRUE;
	BOOL				    fValid_ppSegment	= TRUE;
    IDirectMusicStyle*		pdmStyle			= NULL;
    IDirectMusicSegment*	pdmTemplate			= NULL;
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

    // validate pTemplate
    if(!helpIsValidPtr((void*)pTemplate, sizeof(CtIDirectMusicSegment), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pTemplate = FALSE;
        pdmTemplate = (IDirectMusicSegment*)pTemplate;

    }
    else
    {
        // good pointer, get the real object
        hRes = pTemplate->GetRealObjPtr(&pdmTemplate);
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ComposeMelodyFromTemplate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStyle    == %p   %s",
            pdmStyle,
            fValid_pStyle ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pTemplate == %p   %s",
            pdmTemplate,
            fValid_pTemplate ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppSegment == %p   %s",
            ppdmSegment,
            fValid_ppSegment ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle8*)m_pUnk)->ComposeMelodyFromTemplate(pdmStyle,pdmTemplate,ppdmSegment);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ComposeMelodyFromTemplate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
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

    // done
END:
    if(fValid_pStyle && pdmStyle)
    {
        pdmStyle->Release();
    }
    if(fValid_pTemplate && pdmTemplate)
    {
        pdmTemplate->Release();
    }
    if(fValid_ppSegment && pdmSegment)
    {
        pdmSegment->Release();
    }
    return hRes;

} // *** end CtIDirectMusicStyle8::ComposeMelodyFromTemplate()
*/