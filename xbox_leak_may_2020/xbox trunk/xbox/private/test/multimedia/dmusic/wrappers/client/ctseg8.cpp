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

#include "dmth.h"
#include "dmthp.h"
#include "dmthcom.h"

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
/*
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
*/



//===========================================================================
// CtIDirectMusicSegment8::GetClockTimeDuration()
//
// Encapsulates calls to GetClockTimeDuration
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
/*
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
*/



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
HRESULT CtIDirectMusicSegment8::Download(CtIUnknown *pAudioPath)
{
    HRESULT					hRes				= E_NOTIMPL;
    BOOL					fValid_pAudioPath	= TRUE;
    IUnknown	*pdmAudioPath		= NULL;

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










