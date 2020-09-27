//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctseg8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctseg8.cpp
//
// Test harness implementation of IDirectMusicSegment
//
// Functions:
//    CtIDirectMusicSegment8::SetTrackConfig()
//    CtIDirectMusicSegment8::GetTrackConfig()
//    CtIDirectMusicSegment8::SetClockTimeDuration()
//    CtIDirectMusicSegment8::GetClockTimeDuration()
//    CtIDirectMusicSegment8::SetFlags();
//    CtIDirectMusicSegment8::GetFlags();
//    CtIDirectMusicSegment8::GetObjectInPath()
//    CtIDirectMusicSegment8::GetAudioPathConfig()
//    CtIDirectMusicSegment8::Compose()
//    CtIDirectMusicSegment8::Download()
//    CtIDirectMusicSegment8::Unload()
//
//
// History:
//  10/13/1999 - kcraven - created
//  03/03/2000 - kcraven - fixed to match dmusic change
//===========================================================================

#include "globals.h"

//===========================================================================
// CtIDirectMusicSegment8::CtIDirectMusicSegment8()
//
// Default constructor
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
CtIDirectMusicSegment8::CtIDirectMusicSegment8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicSegment8";

} // *** end CtIDirectMusicSegment8::CtIDirectMusicSegment8()




//===========================================================================
// CtIDirectMusicSegment8::~CtIDirectMusicSegment8()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
CtIDirectMusicSegment8::~CtIDirectMusicSegment8(void)
{
    // nothing to do

} // *** end CtIDirectMusicSegment8::~CtIDirectMusicSegment8()




//===========================================================================
// CtIDirectMusicSegment8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicSegment8 *pdmSegment8 - pointer to real 
//                                      IDirectMusicSegment8 object
//
// Returns: 
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegment8::InitTestClass(IDirectMusicSegment8 *pdmSegment8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmSegment8, sizeof(IDirectMusicSegment8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmSegment8));

} // *** end CtIDirectMusicSegment8::InitTestClass()




//===========================================================================
// CtIDirectMusicSegment8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicSegment8 **ppdmSegment8 - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegment8::GetRealObjPtr(IDirectMusicSegment8 **ppdmSegment8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmSegment8, sizeof(IDirectMusicSegment8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmSegment8));

} // *** end CtIDirectMusicSegment8::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicSegment8::SetTrackConfig()
//
// Encapsulates calls to SetTrackConfig
//
// History:
//  10/12/1999 - kcraven - created
//  03/17/2000 - kcraven - fixedto match dmusic change
//===========================================================================
/*
HRESULT CtIDirectMusicSegment8::SetTrackConfig(
	REFGUID rguidTrackClassID,
    DWORD dwGroup,
    DWORD dwIndex,
    DWORD dwFlagsOn,
    DWORD dwFlagsOff)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_rguidTrackClassID  = TRUE;
    char    szGuid[MAX_LOGSTRING];

    dmthGUIDtoString(rguidTrackClassID, szGuid);

	fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetTrackConfig()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidTrackClassID == %s (%s)",
            szGuid, dmthXlatGUID(rguidTrackClassID));
    fnsLog(PARAMLOGLEVEL, "dwGroup    == %08Xh",
            dwGroup);
    fnsLog(PARAMLOGLEVEL, "dwIndex    == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "dwFlagsOn  == %08Xh",
            dwFlagsOn);
    fnsLog(PARAMLOGLEVEL, "dwFlagsOff == %08Xh",
            dwFlagsOff);

    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->SetTrackConfig(
				rguidTrackClassID,
    			dwGroup,
    			dwIndex,
    			dwFlagsOn,
    			dwFlagsOff
				);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetTrackConfig()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
            dmthXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegment8::SetTrackConfig()




//===========================================================================
// CtIDirectMusicSegment8::GetTrackConfig()
//
// Encapsulates calls to GetTrackConfig
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegment8::GetTrackConfig(
	REFGUID rguidTrackClassID,
	DWORD dwGroup,
	DWORD dwIndex,
	DWORD *pdwFlags)
{
    HRESULT hRes						= E_NOTIMPL;
    BOOL    fValid_rguidTrackClassID	= TRUE;
    BOOL    fValid_pdwFlags				= TRUE;
    char    szGuid[MAX_LOGSTRING];

    dmthGUIDtoString(rguidTrackClassID, szGuid);

    // validate pdwFlags
    if(!helpIsValidPtr((void*)pdwFlags, sizeof(DWORD), FALSE))
    {
        fValid_pdwFlags = FALSE;
    }

	fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetTrackConfig()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidTrackClassID == %s (%s)",
            szGuid, dmthXlatGUID(rguidTrackClassID));
    fnsLog(PARAMLOGLEVEL, "dwGroup == %08Xh",
            dwGroup);
    fnsLog(PARAMLOGLEVEL, "dwIndex == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "pdwFlags == %p",
            pdwFlags);

    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->GetTrackConfig(
				rguidTrackClassID,
    			dwGroup,
    			dwIndex,
    			pdwFlags
				);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetTrackConfig()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
            dmthXlatHRESULT(hRes), hRes);
    if(fValid_pdwFlags)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwFlags == %08Xh",
                *pdwFlags);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegment8::GetTrackConfig()



//===========================================================================
// CtIDirectMusicSegment8::GetClockTimeDuration()
//
// Encapsulates calls to GetClockTimeDuration
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegment8::GetClockTimeDuration(REFERENCE_TIME *prtDuration)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_prtDuration    = TRUE;
    
    // validate pmtLength
    if(!helpIsValidPtr((void*)prtDuration, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prtDuration	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetClockTimeDuration()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "prtDuration == %016Xh   %s",
            prtDuration,
            fValid_prtDuration ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->GetClockTimeDuration(prtDuration);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetClockTimeDuration()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_prtDuration)
    {
        fnsLog(PARAMLOGLEVEL, "*prtDuration == %016Xh",
                *prtDuration);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegment8::GetClockTimeDuration()



//===========================================================================
// CtIDirectMusicSegment8::SetClockTimeDuration()
//
// Encapsulates calls to SetClockTimeDuration
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicSegment8::SetClockTimeDuration(REFERENCE_TIME rtDuration)
{
    HRESULT hRes    = E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetClockTimeDuration()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rtDuration == %016Xh",
            rtDuration);

    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->SetClockTimeDuration(rtDuration);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetClockTimeDuration()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegment8::SetClockTimeDuration()
*/



//===========================================================================
// CtIDirectMusicSegment8::GetFlags()
//
// Encapsulates calls to GetFlags
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicSegment8::GetFlags(DWORD *pdwFlags)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_pdwFlags    = TRUE;
    
    // validate pmtLength
    if(!helpIsValidPtr((void*)pdwFlags, sizeof(DWORD), FALSE))
    {
        fValid_pdwFlags	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetFlags()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwFlags == %p   %s",
            pdwFlags,
            fValid_pdwFlags ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->GetFlags(pdwFlags);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetFlags()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwFlags)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwFlags == %08Xh",
                *pdwFlags);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegment8::GetFlags()
*/



//===========================================================================
// CtIDirectMusicSegment8::SetFlags()
//
// Encapsulates calls to SetFlags
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicSegment8::SetFlags(DWORD dwFlags)
{
    HRESULT hRes    = E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetFlags()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwFlags == %08Xh",
            dwFlags);

    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->SetFlags(dwFlags);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetFlags()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegment8::SetFlags()
*/



//===========================================================================
// CtIDirectMusicSegment8::GetObjectInPath()
//
// Encapsulates calls to GetObjectInPath
//
// History:
//  01/24/2000 - kcraven - created
//  04/06/2000 - danhaff - removed as per dmusic header changes.
//===========================================================================
/*
HRESULT CtIDirectMusicSegment8::GetObjectInPath
(
	DWORD dwPChannel,
	DWORD dwStage,
	DWORD dwBuffer,
	REFGUID guidObject,
	DWORD dwIndex,
	REFGUID iidInterface,
	void ** ppObject
)
{
    HRESULT             hRes					= E_NOTIMPL;
    BOOL	            fValid_ppObject		= TRUE;
    char                szguidObject[MAX_LOGSTRING];
    char                sziidInterface[MAX_LOGSTRING];


    // validate ppObject
    if(!helpIsValidPtr((void*)ppObject, sizeof(void*), FALSE))
    {
        fValid_ppObject = FALSE;
    }

    dmthGUIDtoString(guidObject, szguidObject);
    dmthGUIDtoString(iidInterface, sziidInterface);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetObjectInPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwPChannel   == %08Xh",
            dwPChannel);
    fnsLog(PARAMLOGLEVEL, "dwStage      == %08Xh (%s)",
            dwStage, dmthXlatDMUS_PATH(dwStage));
    fnsLog(PARAMLOGLEVEL, "dwBuffer     == %08Xh",
            dwBuffer);
    fnsLog(PARAMLOGLEVEL, "guidObject   == %s (%s)",
            szguidObject, dmthXlatGUID(guidObject));
    fnsLog(PARAMLOGLEVEL, "dwIndex      == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "iidInterface == %s (%s)",
            sziidInterface, dmthXlatGUID(iidInterface));
	fnsLog(PARAMLOGLEVEL, "ppObject     == %p   %s",
            ppObject,
            fValid_ppObject ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->GetObjectInPath(
														dwPChannel,
														dwStage,
														dwBuffer,
														guidObject,
														dwIndex,
														iidInterface,
														ppObject);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetObjectInPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes     == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppObject && *ppObject)
    {
        fnsLog(PARAMLOGLEVEL, "*ppObject == %p",
                *ppObject);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegment8::GetObjectInPath()

*/


//===========================================================================
// CtIDirectMusicSegment8::GetAudioPathConfig()
//
// Encapsulates calls to GetAudioPathConfig
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicSegment8::GetAudioPathConfig(CtIUnknown ** ppIAudioPathConfig)
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
    hRes = ((IDirectMusicSegment8*)m_pUnk)->GetAudioPathConfig(ppdmIAudioPathConfig);

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

} // *** end CtIDirectMusicSegment8::GetAudioPathConfig()


*/

//===========================================================================
// CtIDirectMusicSegment8::Compose()
//
// Encapsulates calls to Compose
//
// History:
//  01/24/2000 - kcraven - created
//  09/16/2000 - danhaff - fixed leak by not releasing real composed segment.
//===========================================================================
HRESULT CtIDirectMusicSegment8::Compose
(
	MUSIC_TIME mtTime,
	CtIDirectMusicSegment* pFromSegment,
	CtIDirectMusicSegment* pToSegment,
	CtIDirectMusicSegment** ppComposedSegment
)
{
    HRESULT	hRes								= E_NOTIMPL;
    BOOL	fValid_pFromSegment					= TRUE;
	BOOL	fValid_pToSegment					= TRUE;
	BOOL	fValid_ppComposedSegment			= TRUE;
    IDirectMusicSegment*	pdmFromSegment		= NULL;
    IDirectMusicSegment*	pdmToSegment		= NULL;
    IDirectMusicSegment*	pdmComposedSegment	= NULL;
    IDirectMusicSegment**	ppdmComposedSegment	= NULL;

    // validate pFromSegment
    if(!helpIsValidPtr((void*)pFromSegment, sizeof(CtIDirectMusicSegment),
                        TRUE))
    {
        fValid_pFromSegment = FALSE;

        // we have a bogus pointer, use it as such
        pdmFromSegment = (IDirectMusicSegment*)pFromSegment;
    }
    else
    {
        // we have a good pointer, get the real object
		if(pFromSegment) // bug NULL is OK
		{
			hRes = pFromSegment->GetRealObjPtr(&pdmFromSegment);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
						"object pointer (%s == %08Xh)",
						tdmXlatHRESULT(hRes), hRes);
				goto END;
			}
		}
    }
    // validate pToSegment
    if(!helpIsValidPtr((void*)pToSegment, sizeof(CtIDirectMusicSegment),
                        TRUE))
    {
        fValid_pToSegment = FALSE;

        // we have a bogus pointer, use it as such
        pdmToSegment = (IDirectMusicSegment*)pToSegment;
    }
    else
    {
		if(pToSegment)
		{
			// we have a good pointer, get the real object
			hRes = pToSegment->GetRealObjPtr(&pdmToSegment);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
						"object pointer (%s == %08Xh)",
						tdmXlatHRESULT(hRes), hRes);
				goto END;
			}
		}
    }
    // validate ppComposedSegment
    if(!helpIsValidPtr((void*)ppComposedSegment, sizeof(CtIDirectMusicSegment*), 
                    TRUE))
    {
        // bogus pointer, use as such
        fValid_ppComposedSegment = FALSE;
        ppdmComposedSegment = (IDirectMusicSegment**)ppComposedSegment;

    }
    else
    {
		if(ppComposedSegment)
		{
			// valid pointer, create a real object
			ppdmComposedSegment = &pdmComposedSegment;

			// just in case we fail, init test object ptr to NULL
			*ppComposedSegment = NULL;
		}
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Compose()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "mtTime            == %08Xh",
            mtTime);
    fnsLog(PARAMLOGLEVEL, "pFromSegment      == %p   %s",
            pdmFromSegment,
            (fValid_pFromSegment) ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pToSegment        == %p   %s",
            pdmToSegment,
            (fValid_pToSegment) ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppComposedSegment == %p   %s",
            ppdmComposedSegment,
            (fValid_ppComposedSegment) ? "" : "BAD");
    
    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->Compose(
												mtTime,
												pdmFromSegment,
												pdmToSegment,
												ppdmComposedSegment);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Compose()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes        == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppComposedSegment)
    {
        fnsLog(PARAMLOGLEVEL, "*ppComposedSegment == %p",
                pdmComposedSegment);
		// create the test object
		if(SUCCEEDED(hRes) && pdmComposedSegment)
		{
			hRes = dmthCreateTestWrappedObject(pdmComposedSegment, ppComposedSegment);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", 
						hRes);
			}
		}
	}
    fnsDecrementIndent();

    // done
END:
    if(fValid_pFromSegment && pdmFromSegment)
    {
        pdmFromSegment->Release();
    }
    if(fValid_pToSegment && pdmToSegment)
    {
        pdmToSegment->Release();
    }
    
    //danhaff - added 9/16/00
    if (pdmComposedSegment)
    {
        pdmComposedSegment->Release();
    }
    return hRes;

} // *** end CtIDirectMusicSegment8::Compose()



//===========================================================================
// CtIDirectMusicSegment8::Download()
//
// Encapsulates calls to Download
//
// History:
//  01/24/2000 - kcraven - created
//  03/03/2000 - kcraven - fixed to match dmusic change
//===========================================================================
/*
HRESULT CtIDirectMusicSegment8::Download(CtIUnknown *pAudioPath)
{
    HRESULT					hRes				= E_NOTIMPL;
    BOOL					fValid_pAudioPath	= TRUE;
    IUnknown	*pdmAudioPath		= NULL;

    fnsLog(10, "Returning from Download() wrapper; invalid call");
    return S_OK;

    // validate pAudioPath
    if(!helpIsValidPtr((void*)pAudioPath, sizeof(CtIUnknown),
                        FALSE))
    {
        fValid_pAudioPath = FALSE;

        // we have a bogus pointer, use it as such
        pdmAudioPath = (IUnknown*)pAudioPath;
    }
    else
    {
        // we have a good pointer, get the real object
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
            (fValid_pAudioPath) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->Download(pdmAudioPath);

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

} // *** end CtIDirectMusicSegment8::Download()




//===========================================================================
// CtIDirectMusicSegment8::Unload()
//
// Encapsulates calls to Unload
//
// History:
//  01/24/2000 - kcraven - created
//  03/03/2000 - kcraven - fixed to match dmusic change
//===========================================================================
HRESULT CtIDirectMusicSegment8::Unload(CtIUnknown *pAudioPath)
{
    HRESULT					hRes				= E_NOTIMPL;
    BOOL					fValid_pAudioPath	= TRUE;
    IUnknown				*pdmAudioPath		= NULL;

    fnsLog(10, "Returning from Unload() wrapper; invalid call");
    return S_OK;

    // validate pAudioPath
    if(!helpIsValidPtr((void*)pAudioPath, sizeof(CtIUnknown),
                        FALSE))
    {
        fValid_pAudioPath = FALSE;

        // we have a bogus pointer, use it as such
        pdmAudioPath = (IUnknown*)pAudioPath;
    }
    else
    {
        // we have a good pointer, get the real object
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
            (fValid_pAudioPath) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment8*)m_pUnk)->Unload(pdmAudioPath);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Unload()",
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

} // *** end CtIDirectMusicSegment8::Unload()

*/

//===========================================================================
// CtIDirectMusicSegment::GetRepeats()
//
// Encapsulates calls to GetRepeats
//
// History:
//  12/31/1997 - a-llucar - created
//  03/16/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicSegment8::GetRepeats(DWORD* pdwRepeats)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_pdwRepeats   = TRUE;
    
    // validate pdwRepeats
    if(!helpIsValidPtr((void*)pdwRepeats, sizeof(DWORD), FALSE))
    {
        fValid_pdwRepeats	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetRepeats()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwRepeats == %p   %s",
            pdwRepeats,
            fValid_pdwRepeats ?  "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->GetRepeats(pdwRepeats);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetRepeats()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes        == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwRepeats)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwRepeats == %08Xh",
                *pdwRepeats);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::GetRepeats()



//===========================================================================
// CtIDirectMusicSegment::SetRepeats()
//
// Encapsulates calls to SetRepeats
//
// History:
//  12/31/1997 - a-llucar - created
//  03/16/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicSegment8::SetRepeats(DWORD dwRepeats)
{
    HRESULT hRes    = E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetRepeats()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwRepeats == %08Xh",
            dwRepeats);

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->SetRepeats(dwRepeats);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetRepeats()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::SetRepeats()



//===========================================================================
// CtIDirectMusicSegment::GetLength()
//
// Encapsulates calls to GetLength
//
// History:
//  12/31/1997 - a-llucar - created
//  03/12/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicSegment8::GetLength(MUSIC_TIME* pmtLength)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_pmtLength    = TRUE;
    
    // validate pmtLength
    if(!helpIsValidPtr((void*)pmtLength, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtLength	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pmtLength == %p   %s",
            pmtLength,
            fValid_pmtLength ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->GetLength(pmtLength);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pmtLength)
    {
        fnsLog(PARAMLOGLEVEL, "*pmtLength == %08Xh",
                *pmtLength);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::GetLength()

//===========================================================================
// CtIDirectMusicSegment::GetClockTimeLength()
//
// History:
//  04/23/2001- danhaff - created.
//===========================================================================
HRESULT CtIDirectMusicSegment8::GetClockTimeLength(REFERENCE_TIME * prtLength, BOOL *pfClockTime)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_prtLength    = TRUE;
    BOOL    fValid_pfClockTime  = TRUE;
    
    // validate prtLength
    if(!helpIsValidPtr((void*)prtLength, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prtLength	= FALSE;
    }
    // validate prtLength
    if(!helpIsValidPtr((void*)pfClockTime, sizeof(BOOL), FALSE))
    {
        fValid_prtLength	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "prtLength == %p   %s",
            prtLength,
            fValid_prtLength ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pfClockTime == %p   %s",
            pfClockTime,
            fValid_prtLength ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->GetClockTimeLength(prtLength, pfClockTime);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_prtLength)
    {
        fnsLog(PARAMLOGLEVEL, "*prtLength == %08Xh",
                *prtLength);
    }

    if (fValid_pfClockTime)
    {
        fnsLog(PARAMLOGLEVEL, "*pfClockTime == %08Xh",
                *pfClockTime);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::GetLength()


//===========================================================================
// CtIDirectMusicSegment::SetLength()
//
// Encapsulates calls to SetLength
//
// History:
//  12/31/1997 - a-llucar - created
//  03/12/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicSegment8::SetLength(MUSIC_TIME mtLength)
{
    HRESULT hRes    = E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "mtLength == %08Xh",
            mtLength);

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->SetLength(mtLength);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::SetLength()

//===========================================================================
// CtIDirectMusicSegment::SetClockTimeLength()
//
// History:
//  04/23/2001- danhaff - created.
//===========================================================================
HRESULT CtIDirectMusicSegment8::SetClockTimeLength(REFERENCE_TIME rtLength, BOOL fClockTime)
{
    HRESULT hRes    = E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rtLength == %I64Xh",
            rtLength);
    fnsLog(PARAMLOGLEVEL, "fClockTime == %08Xh",
            fClockTime);

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->SetClockTimeLength(rtLength, fClockTime);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::SetLength()



//===========================================================================
// CtIDirectMusicSegment::SetClockTimeLoopPoints()
//
// Encapsulates calls to SetClockTimeLoopPoints
//
// History:
//  10/05/01    danhaff     created
//  
//===========================================================================
/*
HRESULT CtIDirectMusicSegment8::SetClockTimeLoopPoints(REFERENCE_TIME rtStart, REFERENCE_TIME rtEnd)
{
    HRESULT hRes    = E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetClockTimeLoopPoints()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rtStart == %I64X",
            rtStart);
    fnsLog(PARAMLOGLEVEL, "rtEnd == %I64X",
            rtEnd);

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->SetClockTimeLoopPoints(rtStart, rtEnd);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetClockTimeLoopPoints()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::SetClockTimeLoopPoints()



//===========================================================================
// CtIDirectMusicSegment::GetClockTimeLoopPoints()
//
// History:
//  10/05/2001- danhaff - created.
//===========================================================================
HRESULT CtIDirectMusicSegment8::GetClockTimeLoopPoints(REFERENCE_TIME * prtStart, REFERENCE_TIME * prtEnd)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_prtStart    = TRUE;
    BOOL    fValid_prtEnd  = TRUE;
    
    // validate prtStart
    if(!helpIsValidPtr((void*)prtStart, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prtStart	= FALSE;
    }

    // validate prtStart
    if(!helpIsValidPtr((void*)prtEnd, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prtEnd	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "prtStart == %I64u   %s",
            prtStart,
            fValid_prtStart ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "prtEnd == %I64u   %s",
            prtEnd,
            fValid_prtEnd ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->GetClockTimeLoopPoints(prtStart, prtEnd);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_prtStart)
    {
        fnsLog(PARAMLOGLEVEL, "*prtStart == %I64d",
                *prtStart);
    }

    if(fValid_prtEnd)
    {
        fnsLog(PARAMLOGLEVEL, "*prtEnd == %I64d",
                *prtEnd);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::GetClockTimeLoopPoints()

*/


//===========================================================================
// CtIDirectMusicSegment::SetLoopPoints()
//
// Encapsulates calls to SetLoopPoints
//
//===========================================================================
HRESULT CtIDirectMusicSegment8::SetLoopPoints(MUSIC_TIME mtStart, MUSIC_TIME mtEnd)
{
    HRESULT hRes    = E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetLoopPoints()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "mtStart == %08X",
            mtStart);
    fnsLog(PARAMLOGLEVEL, "mtEnd == %08X",
            mtEnd);

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->SetLoopPoints(mtStart, mtEnd);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetLoopPoints()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::SetLoopPoints()


//===========================================================================
// CtIDirectMusicSegment::GetLoopPoints()
//
//===========================================================================
HRESULT CtIDirectMusicSegment8::GetLoopPoints(MUSIC_TIME * pmtStart, MUSIC_TIME * pmtEnd)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_pmtStart    = TRUE;
    BOOL    fValid_pmtEnd  = TRUE;
    
    // validate pmtStart
    if(!helpIsValidPtr((void*)pmtStart, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtStart	= FALSE;
    }

    // validate pmtStart
    if(!helpIsValidPtr((void*)pmtEnd, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtEnd	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pmtStart == %u   %s",
            pmtStart,
            fValid_pmtStart ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pmtEnd == %u   %s",
            pmtEnd,
            fValid_pmtEnd ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->GetLoopPoints(pmtStart, pmtEnd);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetLength()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pmtStart)
    {
        fnsLog(PARAMLOGLEVEL, "*pmtStart == %d",
                *pmtStart);
    }

    if(fValid_pmtEnd)
    {
        fnsLog(PARAMLOGLEVEL, "*pmtEnd == %d",
                *pmtEnd);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::GetLoopPoints()







//===========================================================================
// CtIDirectMusicSegment::SetStartPoint()
//
// Encapsulates calls to SetStartPoint
//
// History:
//  10/05/2001 - danhaff - created
//===========================================================================
HRESULT CtIDirectMusicSegment8::SetStartPoint(MUSIC_TIME mtStart)
{
    HRESULT hRes    = E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetStartPoint()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "mtStart == %08Xh",
            mtStart);

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->SetStartPoint(mtStart);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetStartPoint()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::SetStartPoint()


//===========================================================================
// CtIDirectMusicSegment::GetStartPoint()
//
// Encapsulates calls to GetStartPoint
//
// History:
//  10/05/01    danhaff     created
//===========================================================================
HRESULT CtIDirectMusicSegment8::GetStartPoint(MUSIC_TIME* pmtStart)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_pmtStart    = TRUE;
    
    // validate pmtStart
    if(!helpIsValidPtr((void*)pmtStart, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtStart	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetStartPoint()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pmtStart == %p   %s",
            pmtStart,
            fValid_pmtStart ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->GetStartPoint(pmtStart);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetStartPoint()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pmtStart)
    {
        fnsLog(PARAMLOGLEVEL, "*pmtStart == %08Xh",
                *pmtStart);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::GetStartPoint()



//===========================================================================
// CtIDirectMusicSegment::SetWavePlaybackParams()
//
// Encapsulates calls to SetWavePlaybackParams
//
// History:
//  10/05/2001 - danhaff  - created
//===========================================================================
HRESULT CtIDirectMusicSegment8::SetWavePlaybackParams(DWORD dwFlags, DWORD dwReadAhead)
{
    HRESULT hRes    = E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetWavePlaybackParams()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwFlags == %08Xh",
            dwFlags);
    fnsLog(PARAMLOGLEVEL, "dwReadAhead == %08Xh",
            dwReadAhead);

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->SetWavePlaybackParams(dwFlags, dwReadAhead);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetWavePlaybackParams()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment::SetWavePlaybackParams()


//===========================================================================
// CtIDirectMusicSegment8::AddNotificationType()
//
// Encapsulates calls to AddNotificationType
//
// History:
//  12/31/1997 - a-llucar - created
//  03/03/1998 - davidkl - renamed to AddNotificationType, fixed logging bug
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicSegment8::AddNotificationType(REFGUID rguidNotificationType)
{
    HRESULT     hRes						    = E_NOTIMPL;
    char        szGuid[MAX_LOGSTRING];
    

    dmthGUIDtoString(rguidNotificationType, szGuid);
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::AddNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidNotificationType == %s (%s)",
        szGuid, dmthXlatGUID(rguidNotificationType));

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->AddNotificationType(rguidNotificationType);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::AddNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment8::AddNotificationType()


//===========================================================================
// CtIDirectMusicSegment8::RemoveNotificationType()
//
// Encapsulates calls to RemoveNotificationType
//
// History:
//  12/31/1997 - a-llucar - created
//  03/03/1988 - davidkl - renamed to RemoveNotificationType, fixed logging
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicSegment8::RemoveNotificationType(REFGUID rguidNotificationType)
{
    HRESULT     hRes						    = E_NOTIMPL;
    char        szGuid[MAX_LOGSTRING];
    
    dmthGUIDtoString(rguidNotificationType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::RemoveNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidNotificationType == %s (%s)",
        szGuid, dmthXlatGUID(rguidNotificationType));

    // call the real function
    hRes = ((IDirectMusicSegment*)m_pUnk)->RemoveNotificationType(rguidNotificationType);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::RemoveNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSegment8::RemoveNotificationType()

