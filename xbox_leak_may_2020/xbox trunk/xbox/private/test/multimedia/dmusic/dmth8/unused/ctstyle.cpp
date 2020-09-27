//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctstyle.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctstyle.cpp
//
// Test harness implementation of IDirectMusicStyle
//
// Functions:
//  CtIDirectMusicStyle::CtIDirectMusicStyle()
//  CtIDirectMusicStyle::~CtIDirectMusicStyle()
//  CtIDirectMusicStyle::InitTestClass()
//  CtIDirectMusicStyle::GetRealObjPtr()
//  CtIDirectMusicStyle::QueryInterface()
//  CtIDirectMusicStyle::GetBand()
//  CtIDirectMusicStyle::GetBandName()
//  CtIDirectMusicStyle::GetDefaultBand()
//  CtIDirectMusicStyle::GetMotifName()
//  CtIDirectMusicStyle::GetMotif()
//  CtIDirectMusicStyle::GetDefaultChordMap()
//  CtIDirectMusicStyle::GetChordMapName()
//  CtIDirectMusicStyle::GetChordMap()
//  CtIDirectMusicStyle::GetTimeSignature()
//  CtIDirectMusicStyle::GetEmbellishmentLength()
//  CtIDirectMusicStyle::GetTempo()
//
// History:
//  01/02/1998 - a-llucar - created
//  01/10/1998 - a-llucar - updated GetBand()
//	01/10/1998 - a-llucar - added GetBandName(), GetDefaultChordMap(),
//							GetChordMapName(), GetChordMap()
//  03/31/1998 - davidkl - inheritance update
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicStyle::CtIDirectMusicStyle()
//
// Default constructor
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicStyle::CtIDirectMusicStyle(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicStyle";

} //*** end CtIDirectMusicStyle::CtIDirectMusicStyle()


//===========================================================================
// CtIDirectMusicStyle::~CtIDirectMusicStyle()
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
CtIDirectMusicStyle::~CtIDirectMusicStyle(void)
{
    // nothing to do

} //*** end CtIDirectMusicStyle::~CtIDirectMusicStyle()


//===========================================================================
// CtIDirectMusicStyle::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmStyle - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicStyle::InitTestClass(IDirectMusicStyle *pdmStyle)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmStyle, sizeof(IDirectMusicStyle), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmStyle));

} //*** end CtIDirectMusicStyle::InitTestClass()


//===========================================================================
// CtIDirectMusicStyle::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicStyle **ppdmStyle - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/02/1998 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicStyle::GetRealObjPtr(IDirectMusicStyle **ppdmStyle)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmStyle, sizeof(IDirectMusicStyle*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmStyle));

} //*** end CtIDirectMusicStyle::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicStyle::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicStyle::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicStyle::QueryInterface


//===========================================================================
// CtIDirectMusicStyle::GetBand()
//
// Encapsulates calls to GetBand
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::GetBand(WCHAR *pwszName, 
                                    CtIDirectMusicBand** pptdmBand)
{
    HRESULT             hRes			    = E_NOTIMPL;
    BOOL				fValid_pptdmBand	= TRUE;
    BOOL                fValid_pwszName     = TRUE;
    IDirectMusicBand    *pdmBand            = NULL;
    IDirectMusicBand    **ppdmBand          = NULL;
    
    // validate pptdmBand
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicBand pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicStyle::GetBand().  Otherwise, we are to create and
    //  return a CtIDirectMusicBand object
    if(!helpIsValidPtr((void*)pptdmBand, sizeof(CtIDirectMusicBand*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmBand = FALSE;
        ppdmBand = (IDirectMusicBand**)pptdmBand;

    }
    else
    {
        // valid pointer, create a real object
        ppdmBand = &pdmBand;

        // just in case we fail, init test object ptr to NULL
        *pptdmBand = NULL;
    }

    // validate wszName
    if(!helpIsValidPtr((void*)pwszName, sizeof(WCHAR), FALSE))
    {
        fValid_pwszName = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetBand()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pwszName == %p   %s",
            pwszName,
            fValid_pwszName ? "" : "BAD");
    if(fValid_pwszName)
    {
        // BUGBUG - convert and log string
    }
    fnsLog(PARAMLOGLEVEL, "ppBand   == %p   %s",
            ppdmBand,
            fValid_pptdmBand ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->GetBand(pwszName, ppdmBand);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetBand()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes    == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmBand)
    {
        fnsLog(PARAMLOGLEVEL, "*ppBand == %p",
            pdmBand);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmBand, pptdmBand);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmBand && pdmBand)
    {
        pdmBand->Release();
    }
    return hRes;

} //*** end CtIDirectMusicStyle::GetBand()


//===========================================================================
// CtIDirectMusicStyle::EnumBand()
//
// Encapsulates calls to EnumBand
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - dvaidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::EnumBand(DWORD dwIndex, WCHAR *pwszName)
{
    HRESULT hRes			= E_NOTIMPL;
    BOOL    fValid_pwszName = TRUE;
    
    // validate bstrName
    if(!helpIsValidPtr((void*)pwszName, sizeof(WCHAR), FALSE))
    {
        fValid_pwszName	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumBand()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIndex  == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "pwszName == %p   %s",
            pwszName,
            fValid_pwszName ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->EnumBand(dwIndex, pwszName);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumBand()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pwszName)
    {
        // BUGBUG - convert and log string
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicStyle::EnumBand()


//===========================================================================
// CtIDirectMusicStyle::GetDefaultBand()
//
// Encapsulates calls to GetDefaultBand
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::GetDefaultBand(CtIDirectMusicBand** pptdmBand)
{
    HRESULT             hRes						= E_NOTIMPL;
    BOOL				fValid_pptdmBand	= TRUE;
    IDirectMusicBand    *pdmBand            = NULL;
    IDirectMusicBand    **ppdmBand          = NULL;
    
    // validate pptdmBand
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicBand pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicStyle::GetBand().  Otherwise, we are to create and
    //  return a CtIDirectMusicBand object
    if(!helpIsValidPtr((void*)pptdmBand, sizeof(CtIDirectMusicBand*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmBand = FALSE;
        ppdmBand = (IDirectMusicBand**)pptdmBand;

    }
    else
    {
        // valid pointer, create a real object
        ppdmBand = &pdmBand;

        // just in case we fail, init test object ptr to NULL
        *pptdmBand = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetDefaultBand()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppBand == %p   %s",
            ppdmBand,
            fValid_pptdmBand ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->GetDefaultBand(ppdmBand);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetDefaultBand()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes    == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmBand)
    {
        fnsLog(PARAMLOGLEVEL, "*ppBand == %p",
                pdmBand);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmBand, pptdmBand);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmBand && pdmBand)
    {
        pdmBand->Release();
    }
    return hRes;

} //*** end CtIDirectMusicStyle::GetDefaultBand()


//===========================================================================
// CtIDirectMusicStyle::EnumMotif()
//
// Encapsulates calls to EnumMotif
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - dvaidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::EnumMotif(DWORD dwIndex, WCHAR *pwszName)
{
    HRESULT hRes			= E_NOTIMPL;
    BOOL    fValid_pwszName	= TRUE;
    
    // validate pwszName
    if(!helpIsValidPtr((void*)pwszName, sizeof(WCHAR), FALSE))
    {
        fValid_pwszName	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumMotif()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIndex  == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "pwszName == %p   %s",
            pwszName,
            fValid_pwszName ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->EnumMotif(dwIndex, pwszName);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumMotif()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pwszName)
    {
        // BUGBUG - convert and log string
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicStyle::EnumMotif()


//===========================================================================
// CtIDirectMusicStyle::GetMotif()
//
// Encapsulates calls to GetMotif
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::GetMotif(WCHAR *pwszName, 
							CtIDirectMusicSegment** pptdmSegment)
{
    HRESULT             hRes				= E_NOTIMPL;
    BOOL				fValid_pptdmSegment	= TRUE;
    BOOL                fValid_pwszName	    = TRUE;
    IDirectMusicSegment *pdmSeg             = NULL;
    IDirectMusicSegment **ppdmSeg           = NULL;
    
    // validate pptdmSegment
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegment pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicStyle::GetMotif().  Otherwise, we are to create and
    //  return a CtIDirectMusicSegment object
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
   
    // validate pwszName
    if(!helpIsValidPtr((void*)pwszName, sizeof(WCHAR), FALSE))
    {
        fValid_pwszName	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetMotif()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pwszName  == %p   %s",
            pwszName,
            fValid_pwszName ? "" : "BAD");
    if(fValid_pwszName)
    {
        // BUGBUG convert and log string
    }
    fnsLog(PARAMLOGLEVEL, "ppSegment == %p   %s",
            ppdmSeg,
            fValid_pptdmSegment ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->GetMotif(pwszName, ppdmSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetMotif()",
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
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmSegment && pdmSeg)
    {
        pdmSeg->Release();
    }
    return hRes;

} //*** end CtIDirectMusicStyle::GetMotif()


//===========================================================================
// CtIDirectMusicStyle::GetDefaultChordMap()
//
// Encapsulates calls to GetDefaultChordMap
//
// History:
//  01/11/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::GetDefaultChordMap
                                (CtIDirectMusicChordMap** pptdmChordMap)
{
    HRESULT                 hRes					= E_NOTIMPL;
    BOOL				    fValid_pptdmChordMap	= TRUE;
    IDirectMusicChordMap *pdmPers                = NULL;
    IDirectMusicChordMap **ppdmPers              = NULL;
    
    // validate pptdmChordMap
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicChordMap pointer, 
    //  we assume that we are to perform an invalid parameter test on
    //  IDirectMusicStyle::GetDefaultChordMap().  Otherwise, we are to 
    //  create and return a CtIDirectMusicSegment object
    if(!helpIsValidPtr((void*)pptdmChordMap, 
                        sizeof(CtIDirectMusicChordMap*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmChordMap = FALSE;
        ppdmPers = (IDirectMusicChordMap**)pptdmChordMap;

    }
    else
    {
        // valid pointer, create a real object
        ppdmPers = &pdmPers;

        // just in case we fail, init test object ptr to NULL
        *pptdmChordMap = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetDefaultChordMap()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppChordMap == %p   %s",
            ppdmPers,
            fValid_pptdmChordMap ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->GetDefaultChordMap(ppdmPers);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetDefaultChordMap()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes           == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmChordMap)
    {
        fnsLog(PARAMLOGLEVEL, "*ppChordMap == %p",
                pdmPers);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmPers, pptdmChordMap);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }


    fnsDecrementIndent();

    // done
    if(fValid_pptdmChordMap && pdmPers)
    {
        pdmPers->Release();
    }
    return hRes;

} //*** end CtIDirectMusicStyle::GetDefaultChordMap()


//===========================================================================
// CtIDirectMusicStyle::EnumChordMap()
//
// Encapsulates calls to EnumChordMap
//
// History:
//  01/11/1998 - a-llucar - created
//  03/05/1998 - davidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::EnumChordMap(DWORD dwIndex, WCHAR *pwszName)
{
    HRESULT hRes			= E_NOTIMPL;
    BOOL    fValid_pwszName	= TRUE;
    
    // validate pwszName
    if(!helpIsValidPtr((void*)pwszName, sizeof(WCHAR), FALSE))
    {
        fValid_pwszName	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumChordMap()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIndex  == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "pwszName == %p   %s",
            pwszName,
            fValid_pwszName ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->EnumChordMap(dwIndex, pwszName);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumChordMap()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pwszName)
    {
        // BUGBUG convert and log string
    }

    fnsDecrementIndent();

    // done
    return hRes;


} //*** end CtIDirectMusicStyle::EnumChordMap()


//===========================================================================
// CtIDirectMusicStyle::GetChordMap()
//
// Encapsulates calls to GetChordMap
//
// History:
//  01/11/1998 - a-llucar - created
//  03/05/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::GetChordMap(WCHAR *pwszName, 
								    CtIDirectMusicChordMap** pptdmChordMap)
{
    HRESULT                 hRes					= E_NOTIMPL;
    BOOL				    fValid_pptdmChordMap	= TRUE;
    BOOL                    fValid_pwszName	        = TRUE;
    IDirectMusicChordMap *pdmPers                = NULL;
    IDirectMusicChordMap **ppdmPers              = NULL;
    
    // validate pwszName
    if(!helpIsValidPtr((void*)pwszName, sizeof(WCHAR), FALSE))
    {
        fValid_pwszName	= FALSE;
    }
    
    // validate pptdmChordMap
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicChordMap pointer, 
    //  we assume that we are to perform an invalid parameter test on
    //  IDirectMusicStyle::GetDefaultChordMap().  Otherwise, we are to 
    //  create and return a CtIDirectMusicSegment object
    if(!helpIsValidPtr((void*)pptdmChordMap, 
                        sizeof(CtIDirectMusicChordMap*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmChordMap = FALSE;
        ppdmPers = (IDirectMusicChordMap**)pptdmChordMap;

    }
    else
    {
        // valid pointer, create a real object
        ppdmPers = &pdmPers;

        // just in case we fail, init test object ptr to NULL
        *pptdmChordMap = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetChordMap()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pwszName      == %p   %s",
            pwszName,
            fValid_pwszName ? "" : "BAD");
    if(fValid_pwszName)
    {
        // BUGBUG convert and log string
    }
    fnsLog(PARAMLOGLEVEL, "ppChordMap == %p   %s",
            ppdmPers,
            fValid_pptdmChordMap ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->GetChordMap(pwszName, ppdmPers);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetChordMap()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes           == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmChordMap)
    {
        fnsLog(PARAMLOGLEVEL, "*ppChordMap == %p",
                pdmPers);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmPers, pptdmChordMap);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmChordMap && pdmPers)
    {
        pdmPers->Release();
    }
    return hRes;


} //*** end CtIDirectMusicStyle::GetChordMap()


//===========================================================================
// CtIDirectMusicStyle::GetTimeSignature()
//
// Encapsulates calls to GetTimeSignature
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - updated to new dm, fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::GetTimeSignature(DMUS_TIMESIGNATURE* pTimeSig)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_pTimeSig = TRUE;

    // validate pTimeSig
    if(!helpIsValidPtr((void*)pTimeSig, sizeof(DMUS_TIMESIGNATURE), FALSE))
    {
        fValid_pTimeSig = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetTimeSignature()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pTimeSig == %p   %s",
            pTimeSig,
            fValid_pTimeSig ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->GetTimeSignature(pTimeSig);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetTimeSignature()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pTimeSig)
    {
        // BUGBUG log contents of pTimeSig @ STRUCTLOGLEVEL
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicStyle::GetTimeSignature()


//===========================================================================
// CtIDirectMusicStyle::GetEmbellishmentLength()
//
// Encapsulates calls to GetEmbellishmentLength
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::GetEmbellishmentLength(BYTE bType, BYTE bLevel,
                                                DWORD* pdwMin, DWORD* pdwMax)
{
    HRESULT hRes			= E_NOTIMPL;
    BOOL	fValid_pdwMin   = TRUE;
	BOOL	fValid_pdwMax	= TRUE;
    
    // validate pdwMin
    if(!helpIsValidPtr((void*)pdwMin, sizeof(DWORD), FALSE))
    {
        fValid_pdwMin = FALSE;
    }

    // validate pwMax
    if(!helpIsValidPtr((void*)pdwMax, sizeof(DWORD), FALSE))
    {
        fValid_pdwMax = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetEmbellishmentLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "bType  == %08Xh",
            bType);
    fnsLog(PARAMLOGLEVEL, "bLevel == %08Xh",
            bLevel);
    fnsLog(PARAMLOGLEVEL, "pdwMin == %p   %s",
            pdwMin,
            fValid_pdwMin ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdwMax == %p   %s",
            pdwMax,
            fValid_pdwMax ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->GetEmbellishmentLength(bType, bLevel, pdwMin, pdwMax);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetEmbellishmentLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes   == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwMin)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwMin == %08Xh",
                *pdwMin);
    }
    if(fValid_pdwMax)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwMax == %08Xh",
                *pdwMax);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicStyle::GetEmbellishmentLength()


//===========================================================================
// CtIDirectMusicStyle::GetTempo()
//
// Encapsulates calls to GetTempo
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicStyle::GetTempo(double* pTempo)
{
    HRESULT hRes			= E_NOTIMPL;
	BOOL	fValid_pTempo	= TRUE;
    
    // validate pTempo
    if(!helpIsValidPtr((void*)pTempo, sizeof(double), FALSE))
    {
        fValid_pTempo = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetTempo()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pTempo == %p   %s",
            pTempo,
            fValid_pTempo ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicStyle*)m_pUnk)->GetTempo(pTempo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetTempo()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pTempo)
    {
        // BUGBUG log *pTempo
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicStyle::GetTempo()
