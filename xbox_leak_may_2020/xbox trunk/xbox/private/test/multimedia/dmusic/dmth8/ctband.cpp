//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctband.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctband.cpp
//
// Test harness implementation of IDirectMusicBand
//
// Functions:
//  CtIDirectMusicBand::QueryInterface()
//  CtIDirectMusicBand::CreateSegment()
//  CtIDirectMusicBand::Download()
//  CtIDirectMusicBand::Unlaod()
//  CtIDirectMusicBand::GetFlags()
//
// History:
//  01/06/1997 - a-llucar - created
//  03/30/1998 - davidkl - brave new world... now inherits from CtIUnknown
//===========================================================================
 
#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicBand::CtIDirectMusicBand()
//
// Default constructor
//
// History:
//  01/05/1997 - a-llucar - created
//===========================================================================
CtIDirectMusicBand::CtIDirectMusicBand(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicBand";

} //*** end CtIDirectMusicBand::CtIDirectMusicBand()


//===========================================================================
// CtIDirectMusicBand::~CtIDirectMusicBand()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/05/1997 - a-llucar - created
//===========================================================================
CtIDirectMusicBand::~CtIDirectMusicBand(void)
{
    // nothing to do

} //*** end CtIDirectMusicBand::~CtIDirectMusicBand()


//===========================================================================
// CtIDirectMusicBand::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicBand *pdmBand - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  01/05/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicBand::InitTestClass(IDirectMusicBand *pdmBand)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmBand, sizeof(IDirectMusicBand), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmBand));
		
} //*** end CtIDirectMusicBand::InitTestClass()


//===========================================================================
// CtIDirectMusicBand::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicBand **ppdmBand - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/05/1997 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicBand::GetRealObjPtr(IDirectMusicBand **ppdmBand)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmBand, sizeof(IDirectMusicBand*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmBand));

} //*** end CtIDirectMusicBand::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicBand::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  01/05/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicBand::QueryInterface(REFIID riid, 
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

} //*** end CtIDirectMusicBand::QueryInterface


//===========================================================================
// CtIDirectMusicBand::CreateSegment()
//
// Encapsulates calls to CreateSegment()
//
// History:
//  01/05/1997 - a-llucar - created
//  03/06/1998 - davidkl - now uses wrapper classes, enhanced logging
//===========================================================================
HRESULT CtIDirectMusicBand::CreateSegment(CtIDirectMusicSegment** pptdmSegment)
{                                           
    HRESULT             hRes                = E_NOTIMPL;
    BOOL                fValid_pptdmSegment = TRUE;
    IDirectMusicSegment *pdmSeg             = NULL;
    IDirectMusicSegment **ppdmSeg           = NULL;

    // validate pptdmSegment
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegment pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicBand::CreateSegment().  
    //  Otherwise, we are to create and return a CtIDirectMusicSegment object
    if(!helpIsValidPtr((void*)pptdmSegment, sizeof(CtIDirectMusicSegment*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmSegment = FALSE;
        ppdmSeg = (IDirectMusicSegment**)pptdmSegment;

    }
    else
    {
        // valid pointer, create a real object
        ppdmSeg = &pdmSeg;

        // just in case we fail, init test object ptr to NULL
        *pptdmSegment = NULL;
    }
	
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::CreateSegment()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppSegment == %p   %s",
            ppdmSeg,
            fValid_pptdmSegment ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBand*)m_pUnk)->CreateSegment(ppdmSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::CreateSegment()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmSegment)
    {
        fnsLog(PARAMLOGLEVEL, "*ppSegment == %p",
                pdmSeg);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmSeg, pptdmSegment);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh",
                    hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmSegment && pdmSeg)
    {
        pdmSeg->Release();
    }
    return hRes;

} //*** end CtIDirectMusicBand::CreateSegment()
 

//===========================================================================
// CtIDirectMusicBand::Download()
//
// Encapsulates calls to Download()
//
// History:
//  01/05/1997 - a-llucar - created
//  03/06/1998 - davidkl - now uses wrapper classes
//  04/02/1998 - davidkl - fixed ref. leak
//===========================================================================
HRESULT CtIDirectMusicBand::Download(CtIDirectMusicPerformance* ptdmPerformance)
{                                           
    HRESULT                 hRes                    = E_NOTIMPL;
    BOOL                    fValid_ptdmPerformance  = TRUE;
    IDirectMusicPerformance *pdmPerf                = NULL;

    // validate ptdmPerformance
    if(!helpIsValidPtr((void*)ptdmPerformance, 
                    sizeof(CtIDirectMusicPerformance), FALSE))
    {
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
	
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Download()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPerformance == %p   %s",
            pdmPerf,
            fValid_ptdmPerformance ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBand*)m_pUnk)->Download(pdmPerf);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Download()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmPerformance && pdmPerf)
    {
        pdmPerf->Release();
    }
    return hRes;

} //*** end CtIDirectMusicBand::Download()


//===========================================================================
// CtIDirectMusicBand::Unload()
//
// Encapsulates calls to Unload()
//
// History:
//  01/05/1997 - a-llucar - created
//  03/06/1998 - davidkl - now uses wrapper classes
//  04/02/1998 - davidkl - fixed ref. leak
//===========================================================================
HRESULT CtIDirectMusicBand::Unload(CtIDirectMusicPerformance* ptdmPerformance)
{                                           
    HRESULT                 hRes                    = E_NOTIMPL;
    BOOL                    fValid_ptdmPerformance  = TRUE;
    IDirectMusicPerformance *pdmPerf                = NULL;

    // validate ptdmPerformance
    if(!helpIsValidPtr((void*)ptdmPerformance, 
                    sizeof(CtIDirectMusicPerformance), FALSE))
    {
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
	
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Unload()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPerformance == %p   %s",
            pdmPerf,
            fValid_ptdmPerformance ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBand*)m_pUnk)->Unload(pdmPerf);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Unload()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmPerformance && pdmPerf)
    {
        pdmPerf->Release();
    }
    return hRes;

} //*** end CtIDirectMusicBand::Unload()


