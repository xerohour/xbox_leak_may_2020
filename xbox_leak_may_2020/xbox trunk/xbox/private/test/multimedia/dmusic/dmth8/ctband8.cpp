//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctband8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctband8.cpp
//
// Test harness implementation of IDirectMusicBand8
//
// Functions:
//  CtIDirectMusicBand8::DownloadEx();     
//  CtIDirectMusicBand8::UnloadEx();  
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
 
#include "globals.h"

//===========================================================================
// CtIDirectMusicBand8::CtIDirectMusicBand8()
//
// Default constructor
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
CtIDirectMusicBand8::CtIDirectMusicBand8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicBand8";

} // *** end CtIDirectMusicBand8::CtIDirectMusicBand8()


//===========================================================================
// CtIDirectMusicBand8::~CtIDirectMusicBand8()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
CtIDirectMusicBand8::~CtIDirectMusicBand8(void)
{
    // nothing to do

} // *** end CtIDirectMusicBand8::~CtIDirectMusicBand8()


//===========================================================================
// CtIDirectMusicBand8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicBand8 *pdmBand - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicBand8::InitTestClass(IDirectMusicBand8 *pdmBand8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmBand8, sizeof(IDirectMusicBand8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmBand8));
		
} // *** end CtIDirectMusicBand8::InitTestClass()


//===========================================================================
// CtIDirectMusicBand8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicBand8 **ppdmBand - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicBand8::GetRealObjPtr(IDirectMusicBand8 **ppdmBand8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmBand8, sizeof(IDirectMusicBand8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmBand8));

} // *** end CtIDirectMusicBand8::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicBand8::DownloadEx()
//
// Encapsulates calls to DownloadEx()
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicBand8::DownloadEx(CtIUnknown *pAudioPath)
{                                           
    HRESULT		hRes				= E_NOTIMPL;
    BOOL		fValid_pAudioPath	= TRUE;
    IUnknown*	pdmAudioPath		= NULL;

    // validate pAudioPath
    if(!helpIsValidPtr((void*)pAudioPath,sizeof(CtIUnknown), FALSE))
    {
        fValid_pAudioPath = FALSE;
        pdmAudioPath = (IUnknown*)pAudioPath;
    }
    else
    {
        // good pointer, get the real object
        hRes = pAudioPath->GetRealObjPtr(&pdmAudioPath);
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::DownloadEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pAudioPath == %p   %s",
            pdmAudioPath,
            fValid_pAudioPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBand8*)m_pUnk)->DownloadEx(pdmAudioPath);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::DownloadEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_pAudioPath && pdmAudioPath)
    {
        pdmAudioPath->Release();
    }
    return hRes;

} // *** end CtIDirectMusicBand8::DownloadEx()


//===========================================================================
// CtIDirectMusicBand8::UnloadEx()
//
// Encapsulates calls to UnloadEx()
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicBand8::UnloadEx(CtIUnknown *pAudioPath)
{                                           
    HRESULT		hRes				= E_NOTIMPL;
    BOOL		fValid_pAudioPath	= TRUE;
    IUnknown*	pdmAudioPath		= NULL;

    // validate pAudioPath
    if(!helpIsValidPtr((void*)pAudioPath, 
                    sizeof(CtIUnknown), FALSE))
    {
        fValid_pAudioPath = FALSE;
        pdmAudioPath = (IUnknown*)pAudioPath;
    }
    else
    {
        // good pointer, get the real object
        hRes = pAudioPath->GetRealObjPtr(&pdmAudioPath);
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::UnloadEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pAudioPath == %p   %s",
            pdmAudioPath,
            fValid_pAudioPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicBand8*)m_pUnk)->UnloadEx(pdmAudioPath);

    // log results
    fnsLog(CALLLOGLEVEL,  "--- Returned from %s::UnloadEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_pAudioPath && pdmAudioPath)
    {
        pdmAudioPath->Release();
    }
    return hRes;

} // *** end CtIDirectMusicBand8::UnloadEx()

