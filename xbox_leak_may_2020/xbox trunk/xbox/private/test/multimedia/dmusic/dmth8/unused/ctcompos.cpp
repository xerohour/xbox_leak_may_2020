//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctcompos.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// tdmobj49.cpp
//
// Test harness implementation of IDirectMusicComposer IUnknown
//
// Functions:
//  CtIDirectMusicComposer::CtIDirectMusicComposer()
//  CtIDirectMusicComposer::~CtIDirectMusicComposer()
//  CtIDirectMusicComposer::InitTestClass()
//  CtIDirectMusicComposer::GetRealObjPtr()
//  CtIDirectMusicComposer::QueryInterface()
//  CtIDirectMusicComposer::ComposeSegmentFromTemplate()
//  CtIDirectMusicComposer::ComposeSegmentFromShape()
//  CtIDirectMusicComposer::AutoTransition()
//  CtIDirectMusicComposer::ComposeTransition()
//  CtIDirectMusicComposer::ComposeTemplateFromShape()
//  CtIDirectMusicComposer::ChangeChordMap()
//
// History:
//  01/02/1998 - a-llucar - created
//  01/15/1998 - a-llucar - added AutoTransition()
//  03/31/1998 - davidkl - inheritance update
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicComposer::CtIDirectMusicComposer()
//
// Default constructor
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicComposer::CtIDirectMusicComposer(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicComposer";

} //*** end CtIDirectMusicComposer::CtIDirectMusicComposer()


//===========================================================================
// CtIDirectMusicComposer::~CtIDirectMusicComposer()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicComposer::~CtIDirectMusicComposer(void)
{
    // nothing to do

} //*** end CtIDirectMusicComposer::~CtIDirectMusicComposer()


//===========================================================================
// CtIDirectMusicComposer::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmComposer - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicComposer::InitTestClass(IDirectMusicComposer *pdmComposer)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmComposer, sizeof(IDirectMusicComposer), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmComposer));

} //*** end CtIDirectMusicComposer::InitTestClass()


//===========================================================================
// CtIDirectMusicComposer::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicComposer **ppdmComposer - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/02/1998 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicComposer::GetRealObjPtr(IDirectMusicComposer **ppdmComposer)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmComposer, sizeof(IDirectMusicComposer*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmComposer));

} //*** end CtIDirectMusicComposer::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicComposer::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicComposer::QueryInterface(REFIID riid, 
												LPVOID *ppvObj)
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

} //*** end CtIDirectMusicComposer::QueryInterface


//===========================================================================
// CtIDirectMusicComposer::ComposeSegmentFromTemplate()
//
// Encapsulates calls to ComposeSegmentFromTemplate
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicComposer::ComposeSegmentFromTemplate
                                            (CtIDirectMusicStyle* ptdmStyle,
											CtIDirectMusicSegment* ptdmTempSeg,
											WORD wActivity, 
											CtIDirectMusicChordMap* ptdmChordMap,
											CtIDirectMusicSegment** pptdmSectionSeg)
{
    HRESULT                 hRes					= E_NOTIMPL;
	BOOL				    fValid_ptdmStyle		= TRUE;
    BOOL				    fValid_ptdmTempSeg		= TRUE;
	BOOL				    fValid_ptdmChordMap	= TRUE;
	BOOL				    fValid_pptdmSectionSeg	= TRUE;
    IDirectMusicStyle       *pdmStyle               = NULL;
    IDirectMusicSegment     *pdmTempSeg             = NULL;
    IDirectMusicChordMap *pdmPers                = NULL;
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ComposeSegmentFromTemplate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStyle       == %p   %s",
            pdmStyle,
            fValid_ptdmStyle ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pTempSeg     == %p   %s",
            pdmTempSeg,
            fValid_ptdmTempSeg ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "wActivity    == %04Xh",
            wActivity);
    fnsLog(PARAMLOGLEVEL, "pChordMap == %p   %s",
            pdmPers,
            fValid_ptdmChordMap ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppSectionSeg == %p   %s",
            ppdmSectionSeg,
            fValid_pptdmSectionSeg ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicComposer*)m_pUnk)->ComposeSegmentFromTemplate(pdmStyle, pdmTempSeg, 
                                        wActivity, pdmPers, ppdmSectionSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ComposeSegmentFromTemplate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes          == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmSectionSeg)
    {
        fnsLog(PARAMLOGLEVEL, "*ppSectionSeg == %p",
                pdmSectionSeg);
    }

    // create the test object
    if(SUCCEEDED(hRes))
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

} //*** end CtIDirectMusicComposer::ComposeSegmentFromTemplate()


//===========================================================================
// CtIDirectMusicComposer::ComposeSegmentFromShape()
//
// Encapsulates calls to ComposeSegmentFromShape
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicComposer::ComposeSegmentFromShape
                                    (CtIDirectMusicStyle* ptdmStyle,
									WORD wNumMeasures,
									WORD wShape, WORD wActivity,
									BOOL fIntro, BOOL fEnd,
									CtIDirectMusicChordMap* ptdmChordMap,
									CtIDirectMusicSegment** pptdmSectionSeg)
{
    HRESULT                 hRes					= E_NOTIMPL;
	BOOL				    fValid_ptdmStyle		= TRUE;
	BOOL				    fValid_ptdmChordMap	= TRUE;
	BOOL				    fValid_pptdmSectionSeg	= TRUE;
    IDirectMusicStyle       *pdmStyle               = NULL;
    IDirectMusicChordMap *pdmPers                = NULL;
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
    //  IDirectMusicComposer:ComposeSegmentFromShape().  
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ComposeSegmentFromShape()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStyle == %p   %s",
            pdmStyle,
            fValid_ptdmStyle ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "wNumMeasures == %04Xh",
            wNumMeasures);
    fnsLog(PARAMLOGLEVEL, "wShape       == %04Xh",
            wShape);
    fnsLog(PARAMLOGLEVEL, "wActivity    == %04Xh",
            wActivity);
    fnsLog(PARAMLOGLEVEL, "fIntro       == %d",
            fIntro);
    fnsLog(PARAMLOGLEVEL, "fEnd         == %d",
            fEnd);
    fnsLog(PARAMLOGLEVEL, "pChordMap == %p   %s",
            pdmPers,
            fValid_ptdmChordMap ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppSectionSeg == %p   %s",
            ppdmSectionSeg,
            fValid_pptdmSectionSeg ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicComposer*)m_pUnk)->ComposeSegmentFromShape(pdmStyle, wNumMeasures, 
                                                wShape, wActivity,
												fIntro, fEnd, 
                                                pdmPers, ppdmSectionSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ComposeSegmentFromShape()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes          == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmSectionSeg)
    {
        fnsLog(PARAMLOGLEVEL, "*ppSectionSeg == %p",
                pdmSectionSeg);
    }

    // create the test object
    if(SUCCEEDED(hRes))
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
    if(fValid_ptdmChordMap && pdmPers)
    {
        pdmPers->Release();
    }
    if(fValid_pptdmSectionSeg && pdmSectionSeg)
    {
        pdmSectionSeg->Release();
    }
    return hRes;

} //*** end CtIDirectMusicComposer::ComposeSegmentFromShape()


//===========================================================================
// CtIDirectMusicComposer::ComposeTransition()
//
// Encapsulates calls to ComposeTransition
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicComposer::ComposeTransition
                                    (CtIDirectMusicSegment* ptdmFromSeg,
									CtIDirectMusicSegment* ptdmToSeg,
									WORD wMeasureNum, WORD wCommand,
									DWORD dwFlags,
									CtIDirectMusicChordMap* ptdmChordMap,
									CtIDirectMusicSegment** pptdmSectionSeg)
{
    HRESULT                 hRes					= E_NOTIMPL;
	BOOL				    fValid_ptdmFromSeg		= TRUE;
	BOOL				    fValid_ptdmToSeg		= TRUE;
	BOOL				    fValid_ptdmChordMap	= TRUE;
	BOOL				    fValid_pptdmSectionSeg	= TRUE;
    IDirectMusicChordMap *pdmPers                = NULL;
    IDirectMusicSegment     *pdmFromSeg             = NULL;
    IDirectMusicSegment     *pdmToSeg               = NULL;
    IDirectMusicSegment     *pdmSectionSeg          = NULL;
    IDirectMusicSegment     **ppdmSectionSeg        = NULL;

    // validate ptdmFromSeg
    if(!helpIsValidPtr((void*)ptdmFromSeg, sizeof(CtIDirectMusicSegment), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmFromSeg = FALSE;
        pdmFromSeg = (IDirectMusicSegment*)ptdmFromSeg;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmFromSeg->GetRealObjPtr(&pdmFromSeg);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ptdmToSeg
    if(!helpIsValidPtr((void*)ptdmToSeg, sizeof(CtIDirectMusicSegment), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmToSeg = FALSE;
        pdmToSeg = (IDirectMusicSegment*)ptdmToSeg;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmToSeg->GetRealObjPtr(&pdmToSeg);
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
    //  IDirectMusicComposer:ComposeTransition().  
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ComposeTransition()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pFromSeg     == %p   %s",
            pdmFromSeg,
            fValid_ptdmFromSeg ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pToSeg       == %p   %s",
            pdmToSeg,
            fValid_ptdmToSeg ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "wMeasureNum  == %04Xh",
            wMeasureNum);
    fnsLog(PARAMLOGLEVEL, "wCommand     == %04Xh",
            wCommand);
    fnsLog(PARAMLOGLEVEL, "dwFlags      == %08Xh",
            dwFlags);
    fnsLog(PARAMLOGLEVEL, "pChordMap == %p   %s",
            pdmPers,
            fValid_ptdmChordMap ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppSectionSeg == %p   %s",
            ppdmSectionSeg,
            fValid_pptdmSectionSeg ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicComposer*)m_pUnk)->ComposeTransition(pdmFromSeg, pdmToSeg, 
                                            wMeasureNum, wCommand,
											dwFlags, pdmPers, 
                                            ppdmSectionSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ComposeTransition()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes          == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmSectionSeg)
    {
        fnsLog(PARAMLOGLEVEL, "*ppSectionSeg == %p",
                pdmSectionSeg);
    }

    // create the test object
    if(SUCCEEDED(hRes))
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
    if(fValid_ptdmFromSeg && pdmFromSeg)
    {
        pdmFromSeg->Release();
    }
    if(fValid_ptdmToSeg && pdmToSeg)
    {
        pdmToSeg->Release();
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

} //*** end CtIDirectMusicComposer::ComposeTransition()


//===========================================================================
// CtIDirectMusicComposer::AutoTransition()
//
// Encapsulates calls to AutoTransition
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicComposer::AutoTransition
                                    (CtIDirectMusicPerformance*	ptdmPerformance,
									CtIDirectMusicSegment* ptdmToSeg, 
									WORD wCommand, DWORD dwFlags, 
									CtIDirectMusicChordMap* ptdmChordMap, 
									CtIDirectMusicSegment** pptdmTransSeg, 
									CtIDirectMusicSegmentState** pptdmToSegState, 
									CtIDirectMusicSegmentState** pptdmTransSegState)
{
    HRESULT                     hRes					    = E_NOTIMPL;
	BOOL				        fValid_ptdmPerformance	    = TRUE;
	BOOL				        fValid_ptdmToSeg		    = TRUE;
	BOOL				        fValid_ptdmChordMap	    = TRUE;
	BOOL                        fValid_pptdmTransSeg        = TRUE;
    BOOL				        fValid_pptdmTransSegState	= TRUE;
	BOOL				        fValid_pptdmToSegState	    = TRUE;
    IDirectMusicPerformance     *pdmPerf                    = NULL;
    IDirectMusicChordMap     *pdmPers                    = NULL;
    IDirectMusicSegment         *pdmToSeg                   = NULL;
    IDirectMusicSegment         *pdmTransSeg                = NULL; 
    IDirectMusicSegment         **ppdmTransSeg              = NULL;
    IDirectMusicSegmentState    *pdmToSegSt                 = NULL;
    IDirectMusicSegmentState    **ppdmToSegSt               = NULL;
    IDirectMusicSegmentState    *pdmTransSegSt              = NULL;
    IDirectMusicSegmentState    **ppdmTransSegSt            = NULL;

    // validate ptdmPerformance
    if(!helpIsValidPtr((void*)ptdmPerformance, 
                    sizeof(CtIDirectMusicPerformance), FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmPerformance = FALSE;
        pdmPerf = (IDirectMusicPerformance*)ptdmPerformance;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmPerformance->GetRealObjPtr(&pdmPerf);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ptdmToSeg
    if(!helpIsValidPtr((void*)ptdmToSeg, sizeof(CtIDirectMusicSegment), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmToSeg = FALSE;
        pdmToSeg = (IDirectMusicSegment*)ptdmToSeg;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmToSeg->GetRealObjPtr(&pdmToSeg);
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

    // validate pptdmTransSeg
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegment pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicComposer:AutoTransition().  
    //  Otherwise, we are to create and return a CtIDirectMusicSegment object
    if(!helpIsValidPtr((void*)pptdmTransSeg, sizeof(CtIDirectMusicSegment*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmTransSeg = FALSE;
        ppdmTransSeg = (IDirectMusicSegment**)pptdmTransSeg;

    }
    else
    {
        // valid pointer, create a real object
        ppdmTransSeg = &pdmTransSeg;

        // just in case we fail, init test object ptr to NULL
        *pptdmTransSeg = NULL;
    }

    // validate pptdmToSegState
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegmentState pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicComposer:AutoTransition().  
    //  Otherwise, we are to create and return a CtIDirectMusicSegmentState object
    if(!helpIsValidPtr((void*)pptdmToSegState, 
                    sizeof(CtIDirectMusicSegmentState*), TRUE))
    {
        // bogus pointer, use as such
        fValid_pptdmToSegState = FALSE;
        ppdmToSegSt = (IDirectMusicSegmentState**)pptdmToSegState;

    }
    else
    {
        if(pptdmToSegState)
		{
			// valid pointer, create a real object
			ppdmToSegSt = &pdmToSegSt;
		}
		else
		{
	        ppdmToSegSt = (IDirectMusicSegmentState**)pptdmToSegState;
		}

        // just in case we fail, init test object ptr to NULL
        if (pptdmToSegState) *pptdmToSegState = NULL;
    }

    // validate pptdmTransSegState
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegmentState pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicComposer:AutoTransition().  
    //  Otherwise, we are to create and return a CtIDirectMusicSegmentState object
    if(!helpIsValidPtr((void*)pptdmTransSegState, 
                    sizeof(CtIDirectMusicSegmentState*), TRUE))
    {
        // bogus pointer, use as such
        fValid_pptdmTransSegState = FALSE;
        ppdmTransSegSt = (IDirectMusicSegmentState**)pptdmTransSegState;

    }
    else
    {
		if(pptdmTransSegState)
		{
			// valid pointer, create a real object
			ppdmTransSegSt = &pdmTransSegSt;
		}
		else
		{
	        ppdmTransSegSt = (IDirectMusicSegmentState**)pptdmTransSegState;
		}
        // just in case we fail, init test object ptr to NULL
        if (pptdmTransSegState) *pptdmTransSegState = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::AutoTransition()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPerformance    == %p   %s",
            pdmPerf,
            fValid_ptdmPerformance ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pToSeg          == %p   %s",
            pdmToSeg,
            fValid_ptdmToSeg ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "wCommand        == %04Xh",
            wCommand);
    fnsLog(PARAMLOGLEVEL, "dwFlags         == %08Xh",
            dwFlags);
    fnsLog(PARAMLOGLEVEL, "pChordMap    == %p   %s",
            pdmPers,
            fValid_ptdmChordMap ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppTransSeg      == %p   %s",
            ppdmTransSeg,
            fValid_pptdmTransSeg ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppToSegState    == %p   %s",
            ppdmToSegSt,
            fValid_pptdmToSegState ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppTransSegState == %p   %s",
            ppdmTransSegSt,
            fValid_pptdmTransSegState ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicComposer*)m_pUnk)->AutoTransition(pdmPerf, pdmToSeg, wCommand,
                                    dwFlags, pdmPers, ppdmTransSeg, 
                                    ppdmToSegSt, ppdmTransSegSt);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::AutoTransition()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes             == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmTransSeg)
    {
        fnsLog(PARAMLOGLEVEL, "*ppTransSeg      == %p",
            pdmTransSeg);
    }
    if(fValid_pptdmToSegState)
    {
        fnsLog(PARAMLOGLEVEL, "*ppToSegState    == %p",
                pdmToSegSt);
    }
    if(fValid_pptdmTransSegState)
    {
        fnsLog(PARAMLOGLEVEL, "*ppTransSegState == %p",
                pdmTransSegSt);
    }

    // create test objects
    // NOTE: if we fail to create any object, we will not attempt 
    //  any further object creation
    // UGH - do this better soon....
    if(SUCCEEDED(hRes) && pptdmTransSeg && pdmTransSeg)
    {
        hRes = dmthCreateTestWrappedObject(pdmTransSeg, pptdmTransSeg);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    if(SUCCEEDED(hRes) && pptdmToSegState && pdmToSegSt)
    {
        hRes = dmthCreateTestWrappedObject(pdmToSegSt, pptdmToSegState);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
  //      else
 //       {
            // cleanup previously created objects
//            if(pptdmTransSeg)
//            {
//                (*pptdmTransSeg)->Release();
//                *pptdmTransSeg = NULL;
//            }
 //       }
    }

    if(SUCCEEDED(hRes) && pptdmTransSegState && pdmTransSegSt)
    {
        hRes = dmthCreateTestWrappedObject(pdmTransSegSt, pptdmTransSegState);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
 //       else
 //       {
            // cleanup previously created objects
			//BUGBUG - removed by Lou because we have an invalid test that's incorrectly returning
			//S_OK, and this is causing a GPF.  It seems it should be released in the actual test, not 
			// the wrapper.
//            if(pptdmTransSeg)
//            {
//              (*pptdmTransSeg)->Release();
//              *pptdmTransSeg = NULL;
//            }
//            if(pptdmToSegState)
//            {
//              (*pptdmToSegState)->Release();
//              *pptdmToSegState = NULL;
//            }
 //       }
    }
    
    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmPerformance && pdmPerf)
    {
        pdmPerf->Release();
    }
    if(fValid_ptdmToSeg && pdmToSeg)
    {
        pdmToSeg->Release();
    }
    if(fValid_ptdmChordMap && pdmPers)
    {
        pdmPers->Release();
    }
    if(fValid_pptdmTransSeg && pdmTransSeg)
    {
        pdmTransSeg->Release();
    }
    if(fValid_pptdmToSegState && pdmToSegSt)
    {
        pdmToSegSt->Release();
    }
    if(fValid_pptdmTransSegState && pdmTransSegSt)
    {
        pdmTransSegSt->Release();
    }
    return hRes;

} //*** end CtIDirectMusicComposer::AutoTransition()


//===========================================================================
// CtIDirectMusicComposer::ComposeTemplateFromShape()
//
// Encapsulates calls to ComposeTemplateFromShape
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicComposer::ComposeTemplateFromShape
                                        (WORD wNumMeasures,
										WORD wShape,
										BOOL fIntro, BOOL fEnd,
										WORD wEndLength,
										CtIDirectMusicSegment** pptdmTempSeg)
{        
    HRESULT             hRes				= E_NOTIMPL;
	BOOL				fValid_pptdmTempSeg	= TRUE;
    IDirectMusicSegment *pdmTempSeg         = NULL;
    IDirectMusicSegment **ppdmTempSeg       = NULL;

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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ComposeTemplateFromShape()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "wNumMeasures == %04Xh",
            wNumMeasures);
    fnsLog(PARAMLOGLEVEL, "wShape       == %04Xh",
            wShape);
    fnsLog(PARAMLOGLEVEL, "fIntro       == %d",
            fIntro);
    fnsLog(PARAMLOGLEVEL, "fEnd         == %d",
            fEnd);
    fnsLog(PARAMLOGLEVEL, "wEndLength   == %04Xh",
            wEndLength);
    fnsLog(PARAMLOGLEVEL, "ppTempSeg    == %p   %s",
            ppdmTempSeg,
            fValid_pptdmTempSeg ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicComposer*)m_pUnk)->ComposeTemplateFromShape(wNumMeasures, wShape, 
                                                    fIntro, fEnd,
													wEndLength, ppdmTempSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ComposeTemplateFromShape()",
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

    // done
    if(fValid_pptdmTempSeg && pdmTempSeg)
    {
        pdmTempSeg->Release();
    }
    return hRes;

} //*** end CtIDirectMusicComposer::ComposeTemplateFromShape()


//===========================================================================
// CtIDirectMusicComposer::ChangeChordMap()
//
// Encapsulates calls to ChangeChordMap
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicComposer::ChangeChordMap
                                        (CtIDirectMusicSegment* ptdmSectionSeg,
										BOOL fTrackScale, 
										CtIDirectMusicChordMap* ptdmChordMap)
{
    HRESULT                 hRes					= E_NOTIMPL;
	BOOL				    fValid_ptdmSectionSeg	= TRUE;
	BOOL				    fValid_ptdmChordMap	= TRUE;
    IDirectMusicSegment     *pdmSectionSeg          = NULL;
    IDirectMusicChordMap *pdmPers                = NULL;

    // validate ptdmSectionSeg
    if(!helpIsValidPtr((void*)ptdmSectionSeg, sizeof(CtIDirectMusicSegment), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmSectionSeg = FALSE;
        pdmSectionSeg = (IDirectMusicSegment*)ptdmSectionSeg;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmSectionSeg->GetRealObjPtr(&pdmSectionSeg);
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

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ChangeChordMap()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSectionSeg  == %p   %s",
            pdmSectionSeg,
            fValid_ptdmSectionSeg ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "fTrackScale  == %d",
            fTrackScale);
    fnsLog(PARAMLOGLEVEL, "pChordMap == %p   %s",
            pdmPers,
            fValid_ptdmChordMap ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicComposer*)m_pUnk)->ChangeChordMap(pdmSectionSeg, 
                                            fTrackScale, 
                                            pdmPers);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ChangeChordMap()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmSectionSeg && pdmSectionSeg)
    {
        pdmSectionSeg->Release();
    }
    if(fValid_ptdmChordMap && pdmPers)
    {
        pdmPers->Release();
    }
    return hRes;

} //*** end CtIDirectMusicComposer::ChangeChordMap()

