//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctcmpos8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctcmpos8.cpp
//
// Test harness implementation of IDirectMusicComposer8 IUnknown
//
// Functions:
//    CtIDirectMusicComposer8::ComposeSegmentFromTemplateEx() 
//    CtIDirectMusicComposer8::ComposeTemplateFromShapeEx() 
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicComposer8::CtIDirectMusicComposer8()
//
// Default constructor
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
CtIDirectMusicComposer8::CtIDirectMusicComposer8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicComposer8";

} // *** end CtIDirectMusicComposer8::CtIDirectMusicComposer8()


//===========================================================================
// CtIDirectMusicComposer8::~CtIDirectMusicComposer8()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
CtIDirectMusicComposer8::~CtIDirectMusicComposer8(void)
{
    // nothing to do

} // *** end CtIDirectMusicComposer8::~CtIDirectMusicComposer8()


//===========================================================================
// CtIDirectMusicComposer8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicComposer8 *pdmComposer8 - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicComposer8::InitTestClass(IDirectMusicComposer8 *pdmComposer8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmComposer8, sizeof(IDirectMusicComposer8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmComposer8));

} // *** end CtIDirectMusicComposer8::InitTestClass()


//===========================================================================
// CtIDirectMusicComposer8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicComposer **ppdmComposer8 - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicComposer8::GetRealObjPtr(IDirectMusicComposer8 **ppdmComposer8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmComposer8, sizeof(IDirectMusicComposer8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmComposer8));

} // *** end CtIDirectMusicComposer8::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicComposer8::ComposeSegmentFromTemplateEx()
//
// Encapsulates calls to ComposeSegmentFromTemplateEx
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicComposer8::ComposeSegmentFromTemplateEx
(
	CtIDirectMusicStyle* ptdmStyle,
	CtIDirectMusicSegment* ptdmTempSeg,
	DWORD dwFlags,
	DWORD dwActivity,
	CtIDirectMusicChordMap* ptdmChordMap,
	CtIDirectMusicSegment** pptdmSectionSeg
)
{
    HRESULT                 hRes					= E_NOTIMPL;
	BOOL				    fValid_ptdmStyle		= TRUE;
    BOOL				    fValid_ptdmTempSeg		= TRUE;
	BOOL				    fValid_ptdmChordMap	= TRUE;
	BOOL				    fValid_pptdmSectionSeg	= TRUE;
    IDirectMusicStyle       *pdmStyle               = NULL;
    IDirectMusicSegment     *pdmTempSeg             = NULL;
    IDirectMusicChordMap	*pdmPers                = NULL;
    IDirectMusicSegment     *pdmSectionSeg          = NULL;
    IDirectMusicSegment     **ppdmSectionSeg        = NULL;

    // validate ptdmStyle
    if(!helpIsValidPtr((void*)ptdmStyle, sizeof(CtIDirectMusicStyle), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmStyle = FALSE;
        pdmStyle = (IDirectMusicStyle*)ptdmStyle;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmStyle->GetRealObjPtr(&pdmStyle);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ptdmTempSeg
    if(!helpIsValidPtr((void*)ptdmTempSeg, sizeof(CtIDirectMusicSegment), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmTempSeg = FALSE;
        pdmTempSeg = (IDirectMusicSegment*)ptdmTempSeg;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmTempSeg->GetRealObjPtr(&pdmTempSeg);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ptdmChordMap
    if(!helpIsValidPtr((void*)ptdmChordMap, sizeof(CtIDirectMusicChordMap), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmChordMap = FALSE;
        pdmPers = (IDirectMusicChordMap*)ptdmChordMap;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmChordMap->GetRealObjPtr(&pdmPers);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate pptdmSectionSeg
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegment pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicComposer:ComposeSegmentFromTemplate().  
    //  Otherwise, we are to create and return a CtIDirectMusicSegment object
    if(!helpIsValidPtr((void*)pptdmSectionSeg, sizeof(CtIDirectMusicSegment*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmSectionSeg = FALSE;
        ppdmSectionSeg = (IDirectMusicSegment**)pptdmSectionSeg;

    }
    else
    {
        // valid pointer, create a real object
        ppdmSectionSeg = &pdmSectionSeg;

        // just in case we fail, init test object ptr to NULL
        *pptdmSectionSeg = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ComposeSegmentFromTemplateEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStyle       == %p   %s",
            pdmStyle,
            fValid_ptdmStyle ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pTempSeg     == %p   %s",
            pdmTempSeg,
            fValid_ptdmTempSeg ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwFlags    == %08Xh",
            dwFlags);
    fnsLog(PARAMLOGLEVEL, "dwActivity    == %08Xh",
            dwActivity);
    fnsLog(PARAMLOGLEVEL, "pChordMap == %p   %s",
            pdmPers,
            fValid_ptdmChordMap ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppSectionSeg == %p   %s",
            ppdmSectionSeg,
            fValid_pptdmSectionSeg ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicComposer8*)m_pUnk)->ComposeSegmentFromTemplateEx(pdmStyle, pdmTempSeg, 
                                        dwFlags,dwActivity, pdmPers, ppdmSectionSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ComposeSegmentFromTemplateEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes          == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmSectionSeg)
    {
        fnsLog(PARAMLOGLEVEL, "*ppSectionSeg == %p",
                pdmSectionSeg);
    }

    // create the test object
    if(SUCCEEDED(hRes) && fValid_pptdmSectionSeg && pdmSectionSeg)
    {
        hRes = dmthCreateTestWrappedObject(pdmSectionSeg, pptdmSectionSeg);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmStyle && pdmStyle)
    {
        pdmStyle->Release();
    }
    if(fValid_ptdmTempSeg && pdmTempSeg)
    {
        pdmTempSeg->Release();
    }
    if(fValid_ptdmChordMap && pdmPers)
    {
        pdmPers->Release();
    }
    if(fValid_pptdmSectionSeg && pdmSectionSeg)
    {
        pdmSectionSeg->Release();
    }
    return hRes;

} // *** end CtIDirectMusicComposer8::ComposeSegmentFromTemplateEx()




//===========================================================================
// CtIDirectMusicComposer8::ComposeTemplateFromShapeEx()
//
// Encapsulates calls to ComposeTemplateFromShapeEx
//
// History:
//  10/14/1999 - kcraven - created
//  04/06/2000 - danhaff - removed as per API change.
//===========================================================================
/*
HRESULT CtIDirectMusicComposer8::ComposeTemplateFromShapeEx
(
	WORD wNumMeasures,
	WORD wShape,
	BOOL fIntro, 
	BOOL fEnd,
    CtIDirectMusicStyle* pStyle, 
	CtIDirectMusicSegment** pptdmTempSeg
)
{        
    HRESULT					hRes				= E_NOTIMPL;
	BOOL					fValid_pStyle		= TRUE;
	BOOL					fValid_pptdmTempSeg	= TRUE;
    IDirectMusicStyle*		pdmStyle			= NULL; 
    IDirectMusicSegment*	pdmTempSeg			= NULL;
    IDirectMusicSegment**	ppdmTempSeg			= NULL;

    // validate ptdmStyle
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

    // validate pptdmTempSeg
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegment pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicComposer::ComposeTemplateFromShape().  
    //  Otherwise, we are to create and return a CtIDirectMusicSegment object
    if(!helpIsValidPtr((void*)pptdmTempSeg, sizeof(CtIDirectMusicSegment*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmTempSeg = FALSE;
        ppdmTempSeg = (IDirectMusicSegment**)pptdmTempSeg;

    }
    else
    {
        // valid pointer, create a real object
        ppdmTempSeg = &pdmTempSeg;

        // just in case we fail, init test object ptr to NULL
        *pptdmTempSeg = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ComposeTemplateFromShapeEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "wNumMeasures == %04Xh",
            wNumMeasures);
    fnsLog(PARAMLOGLEVEL, "wShape       == %04Xh",
            wShape);
    fnsLog(PARAMLOGLEVEL, "fIntro       == %d",
            fIntro);
    fnsLog(PARAMLOGLEVEL, "fEnd         == %d",
            fEnd);
    fnsLog(PARAMLOGLEVEL, "pStyle       == %p   %s",
            pdmStyle,
            fValid_pStyle ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppTempSeg    == %p   %s",
            ppdmTempSeg,
            fValid_pptdmTempSeg ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicComposer8*)m_pUnk)->ComposeTemplateFromShapeEx(
					wNumMeasures,
					wShape,
					fIntro, 
					fEnd,
					pdmStyle,
					ppdmTempSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ComposeTemplateFromShapeEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmTempSeg)
    {
        fnsLog(PARAMLOGLEVEL, "*ppTempSeg == %p",
                pdmTempSeg);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmTempSeg, pptdmTempSeg);
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
    if(fValid_pptdmTempSeg && pdmTempSeg)
    {
        pdmTempSeg->Release();
    }
    return hRes;

} // *** end CtIDirectMusicComposer8::ComposeTemplateFromShapeEx()
*/