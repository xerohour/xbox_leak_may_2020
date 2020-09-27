//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctsong.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctsong.cpp
//
// Test harness implementation of IDirectMusicSong
//
// Functions:
//  CtIDirectMusicSong::CtIDirectMusicSong()
//  CtIDirectMusicSong::~CtIDirectMusicSong()
//  CtIDirectMusicSong::InitTestClass()
//  CtIDirectMusicSong::GetRealObjPtr()
//  CtIDirectMusicSong::QueryInterface()
//  CtIDirectMusicSong::Compose() 
//  CtIDirectMusicSong::GetParam() 
//  CtIDirectMusicSong::EnumSegment()
//  CtIDirectMusicSong::Clone()
//  CtIDirectMusicSong::GetAudioPathConfig()
//  CtIDirectMusicSong::Download()
//  CtIDirectMusicSong::Unload()
//
// History:
//  10/25/1999 - kcraven - created
//  04/06/2000 - danhaff - removed EnumSegment as per dmusic header changes.
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicSong::CtIDirectMusicSong()
//
// Default constructor
//
// History:
//  10/25/1999 - kcraven - created
//===========================================================================
CtIDirectMusicSong::CtIDirectMusicSong(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicSong";

} //*** end CtIDirectMusicSong::CtIDirectMusicSong()


//===========================================================================
// CtIDirectMusicSong::~CtIDirectMusicSong()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/25/1999 - kcraven - created
//===========================================================================
CtIDirectMusicSong::~CtIDirectMusicSong(void)
{
    // nothing to do

} //*** end CtIDirectMusicSong::~CtIDirectMusicSong()


//===========================================================================
// CtIDirectMusicSong::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicSong *pdmSong - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  10/25/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSong::InitTestClass(IDirectMusicSong *pdmSong)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmSong, sizeof(IDirectMusicSong), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmSong));

} //*** end CtIDirectMusicSong::InitTestClass()


//===========================================================================
// CtIDirectMusicSong::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	CtIDirectMusicSong **ppdmSong - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/25/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSong::GetRealObjPtr(IDirectMusicSong **ppdmSong)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmSong, sizeof(IDirectMusicSong*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmSong));

} //*** end CtIDirectMusicSong::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicSong::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  10/25/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSong::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicSong::QueryInterface()




//===========================================================================
// CtIDirectMusicSong::Compose()
//
// Encapsulates calls to Compose
//
// History:
//  10/25/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSong::Compose(void)
{
    HRESULT		hRes				= E_NOTIMPL;
 
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Compose()",
			m_szInterfaceName);
	
    // call the real function
    hRes = ((IDirectMusicSong*)m_pUnk)->Compose();

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Compose()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicSong::Compose()




//===========================================================================
// CtIDirectMusicSong::GetParam()
//
// Encapsulates calls to GetParam
//
// History:
//  10/25/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSong::GetParam
(
	REFGUID rguidType, 
	DWORD dwGroupBits, 
	DWORD dwIndex, 
	MUSIC_TIME mtTime, 
	MUSIC_TIME* pmtNext, 
	void* pParam
)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL	fValid_pmtNext		= TRUE;
    BOOL	fValid_pParam		= TRUE;
    char    szGuid[MAX_LOGSTRING];


    // validate pmtNext
    if(!helpIsValidPtr((void*)pmtNext, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtNext = FALSE;
    }

    // validate pData
    if(!helpIsValidPtr((void*)pParam, 1, FALSE))
    {
        fValid_pParam = FALSE;
    }

    dmthGUIDtoString(rguidType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType   == %s (%s)",
            szGuid, dmthXlatGUID(rguidType));
	fnsLog(PARAMLOGLEVEL, "dwGroupBits == %08Xh",
			dwGroupBits);
	fnsLog(PARAMLOGLEVEL, "dwIndex     == %08Xh",
			dwIndex);
    fnsLog(PARAMLOGLEVEL, "mtTime      == %08Xh",
            mtTime);
    fnsLog(PARAMLOGLEVEL, "pmtNext     == %p   %s",
            pmtNext,
            fValid_pmtNext ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pParam      == %p   %s",
            pParam,
            fValid_pParam ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSong*)m_pUnk)->GetParam(
											rguidType, 
											dwGroupBits, 
											dwIndex, 
											mtTime, 
											pmtNext, 
											pParam
										);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    // BUGBUG want to log out params

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSong::GetParam()





//===========================================================================
// CtIDirectMusicSong::EnumSegment()
//
// Encapsulates calls to EnumSegment
//
// History:
//  10/25/1999 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicSong::EnumSegment
(
	DWORD dwIndex, 
	CtIDirectMusicSegment **ppSegment
)
{
    HRESULT hRes					= E_NOTIMPL;
    BOOL    fValid_ppSegment		= TRUE;
	IDirectMusicSegment*	pdmSegment	= NULL;
	IDirectMusicSegment**	ppdmSegment = NULL;
    char    szGuid[MAX_LOGSTRING];

    if(!helpIsValidPtr((void*)ppSegment, sizeof(CtIDirectMusicSegment*), FALSE))
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumSegment()",
			m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "dwIndex      == %08Xh",
			dwIndex);
    fnsLog(PARAMLOGLEVEL, "ppSegment    == %p   %s",
            ppdmSegment,
            (fValid_ppSegment) ? "" : "BAD");
	
    // call the real function
    hRes = ((IDirectMusicSong*)m_pUnk)->EnumSegment(
				dwIndex, 
				ppdmSegment
				);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumSegment()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
	if(fValid_ppSegment)
	{
    	fnsLog(PARAMLOGLEVEL, "*ppSegment == %p",
				pdmSegment);
	}
  
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
    if(fValid_ppSegment && pdmSegment)
    {
        pdmSegment->Release();
    }

    return hRes;

} //*** end CtIDirectMusicSong::EnumSegment()
*/

//===========================================================================
// CtIDirectMusicSong::Clone()
//
// Encapsulates calls to Clone
//
// History:
//  11/15/1999 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicSong::Clone(CtIDirectMusicSong **ppSong)
{
    HRESULT		hRes				= E_NOTIMPL;
    BOOL		fValid_ppSong		= TRUE;
	IDirectMusicSong*	pdmSong		= NULL;
	IDirectMusicSong**	ppdmSong	= NULL;
 
    if(!helpIsValidPtr((void*)ppSong, sizeof(CtIDirectMusicSong*), FALSE))
    {
        // bogus pointer, use as such
        fValid_ppSong = FALSE;
        ppdmSong = (IDirectMusicSong**)ppSong;
    }
    else
    {
        // valid pointer, create a real object
        ppdmSong = &pdmSong;

        // just in case we fail, init test object ptr to NULL
        *ppSong = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Clone()",
			m_szInterfaceName);
   fnsLog(PARAMLOGLEVEL, "ppSong == %p   %s",
            ppdmSong,
            (fValid_ppSong) ? "" : "BAD");
	
    // call the real function
    hRes = ((IDirectMusicSong*)m_pUnk)->Clone(ppdmSong);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Clone()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
	if(fValid_ppSong)
	{
    	fnsLog(PARAMLOGLEVEL, "*ppdmSong == %p",
				pdmSong);
	}
  
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmSong, ppSong);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_ppSong && pdmSong)
    {
        pdmSong->Release();
    }

    return hRes;

} //*** end CtIDirectMusicSong::Clone()
*/



//===========================================================================
// CtIDirectMusicSong::GetAudioPathConfig()
//
// Encapsulates calls to GetAudioPathConfig
//
// History:
//  01/25/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSong::GetAudioPathConfig(CtIUnknown ** ppIAudioPathConfig)
{
    HRESULT		hRes						= E_NOTIMPL;
    BOOL		fValid_ppIAudioPathConfig	= TRUE;
    IUnknown	*pdmIAudioPathConfig		= NULL;
    IUnknown	**ppdmIAudioPathConfig		= NULL;

    // validate ppIAudioPathConfig
    if(!helpIsValidPtr((void*)ppIAudioPathConfig, sizeof(CtIUnknown*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ppIAudioPathConfig = FALSE;
        ppdmIAudioPathConfig = (IUnknown**)ppIAudioPathConfig;

    }
    else
    {
        // valid pointer, create a real object
        ppdmIAudioPathConfig = &pdmIAudioPathConfig;

        // just in case we fail, init test object ptr to NULL
        *ppIAudioPathConfig = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetAudioPathConfig()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppIAudioPathConfig == %p   %s",
            ppdmIAudioPathConfig,
            (fValid_ppIAudioPathConfig) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSong*)m_pUnk)->GetAudioPathConfig(ppdmIAudioPathConfig);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetAudioPathConfig()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppIAudioPathConfig)
    {
        fnsLog(PARAMLOGLEVEL, "*ppIAudioPathConfig == %p",
                pdmIAudioPathConfig);
		// create the test object
		if(SUCCEEDED(hRes) && pdmIAudioPathConfig)
		{
			hRes = dmthCreateTestWrappedObject(pdmIAudioPathConfig, ppIAudioPathConfig);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
			}
		}
    }

    fnsDecrementIndent();

    // done
    if(fValid_ppIAudioPathConfig && pdmIAudioPathConfig)
    {
        pdmIAudioPathConfig->Release();
    }
    return hRes;

} // *** end CtIDirectMusicSong::GetAudioPathConfig()




//===========================================================================
// CtIDirectMusicSong::Download()
//
// Encapsulates calls to Download()
//
// History:
//  01/31/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSong::Download(CtIUnknown *pAudioPath)
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Download()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pAudioPath == %p   %s",
            pdmAudioPath,
            fValid_pAudioPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSong*)m_pUnk)->Download(pdmAudioPath);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Download()",
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

} // *** end CtIDirectMusicSong::Download()




//===========================================================================
// CtIDirectMusicSong::Unload()
//
// Encapsulates calls to Unload()
//
// History:
//  01/31/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSong::Unload(CtIUnknown *pAudioPath)
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Unload()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pAudioPath == %p   %s",
            pdmAudioPath,
            fValid_pAudioPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSong*)m_pUnk)->Unload(pdmAudioPath);

    // log results
    fnsLog(CALLLOGLEVEL,  "--- Returned from %s::Unload()",
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

} // *** end CtIDirectMusicSong::Unload()


/*-----------------------------------------------------------------------------
|	Function:	
|	Purpose:	
|	Arguments:	
|	Returns:	
|	Notes:		
|	History:
|	04/28/2000 - ruswoods - created
\----------------------------------------------------------------------------*/
HRESULT CtIDirectMusicSong::GetSegment (WCHAR* pwzName, CtIDirectMusicSegment **ppSegment)
{
    HRESULT					hRes				= E_NOTIMPL;
    BOOL					fValid_ppSegment	= TRUE;
    IDirectMusicSegment*	pdmSegment			= NULL;

    // validate ppSegment
    if(!helpIsValidPtr((void*)ppSegment,sizeof(CtIDirectMusicSegment *), FALSE))
    {
        fValid_ppSegment = FALSE;
    }
	
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetSegment()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppSegment == %p   %s",
            ppSegment,
            fValid_ppSegment ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSong*)m_pUnk)->GetSegment(pwzName, ppSegment ? &pdmSegment : NULL);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetSegment()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

	if (pdmSegment)
	{
		hRes = dmthCreateTestWrappedObject (pdmSegment, ppSegment);
		if (FAILED (hRes))
		{
			fnsLog(CALLLOGLEVEL, TEXT("**** dmthCreateTestWrappedObject (pdmSegmentfailed! (%s == %08Xh)"),
				tdmXlatHRESULT(hRes), hRes);
			goto END;
		}

		pdmSegment->Release ();
	}

	hRes = S_OK;

    // done
END:
    return hRes;

}