//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       cttrack8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// cttrack.cpp
//
// Test harness implementation of IDirectMusicTrack8
//
// Functions:
//  CtIDirectMusicTrack8::CtIDirectMusicTrack()
//  CtIDirectMusicTrack8::~CtIDirectMusicTrack()
//  CtIDirectMusicTrack8::InitTestClass()
//  CtIDirectMusicTrack8::GetRealObjPtr()
//  CtIDirectMusicTrack8::QueryInterface()
//  CtIDirectMusicTrack8::PlayEx()
//  CtIDirectMusicTrack8::GetParamEx()
//  CtIDirectMusicTrack8::SetParamEx()
//  CtIDirectMusicTrack8::Compose()
//  CtIDirectMusicTrack8::Join()
//
//
// History:
//  10/22/1999 - kcraven - created
//  03/03/2000 - kcraven - fixed to match dmusic change
//===========================================================================


#include "dmth.h"
#include "dmthp.h"
#include "dmthcom.h"


//===========================================================================
// CtIDirectMusicTrack8::CtIDirectMusicTrack8()
//
// Default constructor
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
CtIDirectMusicTrack8::CtIDirectMusicTrack8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicTrack8";

} // *** end CtIDirectMusicTrack8::CtIDirectMusicTrack8()


//===========================================================================
// CtIDirectMusicTrack8::~CtIDirectMusicTrack8()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
CtIDirectMusicTrack8::~CtIDirectMusicTrack8(void)
{
    // nothing to do

} // *** end CtIDirectMusicTrack8::~CtIDirectMusicTrack8()


//===========================================================================
// CtIDirectMusicTrack8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicTrack8 *pdmTrack8 - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicTrack8::InitTestClass(IDirectMusicTrack8 *pdmTrack8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmTrack8, sizeof(IDirectMusicTrack8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmTrack8));

} // *** end CtIDirectMusicTrack8::InitTestClass()


//===========================================================================
// CtIDirectMusicTrack8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicTrack8 **ppdmTrack8 - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicTrack8::GetRealObjPtr(IDirectMusicTrack8 **ppdmTrack8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmTrack8, sizeof(IDirectMusicTrack8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmTrack8));

} // *** end CtIDirectMusicTrack8::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicTrack8::PlayEx()
//
// Encapsulates calls to PlayEx
//
// History:
//  10/13/1999 - kcraven - created
//  03/03/2000 - kcraven - fixed to match dmusic change
//===========================================================================
HRESULT CtIDirectMusicTrack8::PlayEx
(
	void* pStateData, 
	REFERENCE_TIME rtStart, 
	REFERENCE_TIME rtEnd, 
	REFERENCE_TIME rtOffset, 
	DWORD dwFlags, 
	CtIDirectMusicPerformance* pPerf, 
	CtIDirectMusicSegmentState* pSegSt, 
	DWORD dwVirtualID
)
{
    HRESULT                     hRes					= E_NOTIMPL;
    BOOL		                fValid_pStateData		= TRUE;
    BOOL		                fValid_pPerf			= TRUE;
    BOOL		                fValid_pSegSt			= TRUE;
    IDirectMusicPerformance*	pdmPerf					= NULL;
    IDirectMusicSegmentState*	pdmSegSt                = NULL;

    // validate pStateData
    if(!helpIsValidPtr((void*)pStateData,1, FALSE))
    {
        fValid_pStateData = FALSE;
    }

    // validate pPerf
    if(!helpIsValidPtr((void*)pPerf,sizeof(CtIDirectMusicPerformance*), TRUE))
    {
        // bogus pointer, use as such
        fValid_pPerf = FALSE;
        pdmPerf = (IDirectMusicPerformance*)pPerf;
    }
    else
    {
        // good pointer, get the real object
        hRes = pPerf->GetRealObjPtr(&pdmPerf);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate pSegSt
    if(!helpIsValidPtr((void*)pSegSt, sizeof(CtIDirectMusicSegmentState*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pSegSt = FALSE;
        pdmSegSt = (IDirectMusicSegmentState*)pSegSt;

    }
    else
    {
        // good pointer, get the real object
        hRes = pSegSt->GetRealObjPtr(&pdmSegSt);
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
    fnsLog(CALLLOGLEVEL,  "---- Calling %s::PlayEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStateData == %p   %s",
            pStateData,
            fValid_pStateData ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "rtStart     == %08X%08Xh",
            (DWORD)(rtStart >> 32), (DWORD)rtStart);
    fnsLog(PARAMLOGLEVEL, "rtEnd       == %08X%08Xh", 
            (DWORD)(rtEnd >> 32), (DWORD)rtEnd);
    fnsLog(PARAMLOGLEVEL, "rtOffset    == %08X%08Xh", 
            (DWORD)(rtOffset >> 32), (DWORD)rtOffset);
    fnsLog(PARAMLOGLEVEL, "dwFlags     == %08Xh", 
            dwFlags);
    fnsLog(PARAMLOGLEVEL, "pPerf       == %p   %s",
            pdmPerf,
            fValid_pPerf ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pSegSt      == %p   %s",
            pdmSegSt,
            fValid_pSegSt ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwVirtualID == %08Xh",
            dwVirtualID);

    // call the real function
    hRes = ((IDirectMusicTrack8*)m_pUnk)->PlayEx(
				pStateData, 
				rtStart, 
				rtEnd, 
				rtOffset, 
				dwFlags, 
				pdmPerf, 
				pdmSegSt, 
				dwVirtualID
				);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::PlayEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes            == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_pPerf && pdmPerf)
    {
        pdmPerf->Release();
    }
    if(fValid_pSegSt && pdmSegSt)
    {
        pdmSegSt->Release();
    }
    return hRes;

} // *** end CtIDirectMusicTrack8::PlayEx()




//===========================================================================
// CtIDirectMusicTrack8::GetParamEx()
//
// Encapsulates calls to GetParamEx
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicTrack8::GetParamEx
(
	REFGUID rguidType,
	REFERENCE_TIME rtTime,
	REFERENCE_TIME* prtNext,
	void* pParam,
	void* pStateData,
	DWORD dwFlags
)
{
    HRESULT                     hRes					= E_NOTIMPL;
    BOOL		                fValid_prtNext			= TRUE;
    BOOL		                fValid_pParam			= TRUE;
    BOOL		                fValid_pStateData		= TRUE;
     char    szGuid[MAX_LOGSTRING];

    dmthGUIDtoString(rguidType, szGuid);

    // validate pmtNext
    if(!helpIsValidPtr((void*)prtNext, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prtNext = FALSE;
    }
    // validate pStateData
    if(!helpIsValidPtr((void*)pParam,1, FALSE))
    {
        fValid_pParam = FALSE;
    }
    // validate pStateData
    if(!helpIsValidPtr((void*)pStateData,1, FALSE))
    {
        fValid_pStateData = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL,  "---- Calling %s::GetParamEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType  == %s (%s)",
            szGuid, dmthXlatGUID(rguidType));
    fnsLog(PARAMLOGLEVEL, "rtTime     == %08X%08Xh",
            (DWORD)(rtTime >> 32), (DWORD)rtTime);
    fnsLog(PARAMLOGLEVEL, "prtNext    == %p   %s",
            prtNext,
            fValid_prtNext ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pParam     == %p   %s",
            pParam,
            fValid_pParam ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pStateData == %p   %s",
            pStateData,
            fValid_pStateData ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwFlags    == %08Xh",
            dwFlags);

    // call the real function
    hRes = ((IDirectMusicTrack8*)m_pUnk)->GetParamEx(
				rguidType,
				rtTime,
				prtNext,
				pParam,
				pStateData,
				dwFlags
				);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetParamEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes            == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done

    return hRes;

} // *** end CtIDirectMusicTrack8::GetParamEx()




//===========================================================================
// CtIDirectMusicTrack8::SetParamEx()
//
// Encapsulates calls to SetParamEx
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicTrack8::SetParamEx
(
	REFGUID rguidType,
	REFERENCE_TIME rtTime,
	void* pParam,
	void* pStateData,
	DWORD dwFlags
)
{
    HRESULT	hRes					= E_NOTIMPL;
    BOOL	fValid_pParam			= TRUE;
    BOOL	fValid_pStateData		= TRUE;
	char		szGuid[MAX_LOGSTRING];

    dmthGUIDtoString(rguidType, szGuid);

    // validate pStateData
    if(!helpIsValidPtr((void*)pParam,1, FALSE))
    {
        fValid_pParam = FALSE;
    }
    // validate pStateData
    if(!helpIsValidPtr((void*)pStateData,1, FALSE))
    {
        fValid_pStateData = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL,  "---- Calling %s::SetParamEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType  == %s (%s)",
            szGuid, dmthXlatGUID(rguidType));
    fnsLog(PARAMLOGLEVEL, "rtTime     == %08X%08Xh",
            (DWORD)(rtTime >> 32), (DWORD)rtTime);
    fnsLog(PARAMLOGLEVEL, "pParam     == %p   %s",
            pParam,
            fValid_pParam ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pStateData == %p   %s",
            pStateData,
            fValid_pStateData ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwFlags    == %08Xh",
            dwFlags);

    // call the real function
    hRes = ((IDirectMusicTrack8*)m_pUnk)->SetParamEx(
				rguidType,
				rtTime,
				pParam,
				pStateData,
				dwFlags
				);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SetParamEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes            == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done

    return hRes;

} // *** end CtIDirectMusicTrack8::SetParamEx()




//===========================================================================
// CtIDirectMusicTrack8::Compose()
//
// Encapsulates calls to Compose
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicTrack8::Compose
(
	CtIUnknown* pContext,
	DWORD dwTrackGroup,
	CtIDirectMusicTrack** ppResultTrack
)
{
    HRESULT					hRes					= E_NOTIMPL;
    BOOL				    fValid_pContext			= TRUE;
    BOOL				    fValid_ppResultTrack	= TRUE;
    IUnknown*				pdmContext				= NULL;
    IDirectMusicTrack*		pdmResultTrack			= NULL;
    IDirectMusicTrack**		ppdmResultTrack			= NULL;

    // validate pSong
    if(!helpIsValidPtr((void*)pContext,sizeof(CtIUnknown*),TRUE))
    {
        // bogus pointer, use as such
        fValid_pContext = FALSE;
        pdmContext = (IUnknown*)pContext;
    }
    else
    {
        // good pointer, get the real object
        hRes = pContext->GetRealObjPtr(&pdmContext);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ppResultTrack
    if(!helpIsValidPtr((void*)ppResultTrack, sizeof(CtIDirectMusicTrack*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ppResultTrack = FALSE;
        ppdmResultTrack = (IDirectMusicTrack**)ppResultTrack;

    }
    else
    {
        // valid pointer, create a real object
        ppdmResultTrack = &pdmResultTrack;

        // just in case we fail, init test object ptr to NULL
        *ppResultTrack = NULL;
    }
    
	fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Compose()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pContext      == %p   %s",
            pdmContext,
            (fValid_pContext) ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "dwTrackGroup  == %08Xh",
            dwTrackGroup);
    fnsLog(PARAMLOGLEVEL, "ppResultTrack == %p   %s",
            ppdmResultTrack,
            (fValid_ppResultTrack) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicTrack8*)m_pUnk)->Compose(pdmContext,dwTrackGroup,ppdmResultTrack);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Compose()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppResultTrack)
    {
        fnsLog(PARAMLOGLEVEL, "*ppResultTrack == %p",
                pdmResultTrack);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmResultTrack, ppResultTrack);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
END:
    if(fValid_pContext && pdmContext)
    {
        pdmContext->Release();
    }
    if(fValid_ppResultTrack && pdmResultTrack)
    {
        pdmResultTrack->Release();
    }
    return hRes;

} // *** end CtIDirectMusicTrack8::Compose()




//===========================================================================
// CtIDirectMusicTrack8::Join()
//
// Encapsulates calls to Join
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicTrack8::Join
(
	CtIDirectMusicTrack* pNewTrack,
	MUSIC_TIME mtJoin,
    CtIUnknown* pContext,
    DWORD dwTrackGroup,
	CtIDirectMusicTrack** ppResultTrack
)
{
    HRESULT             hRes					= E_NOTIMPL;
    BOOL		        fValid_pNewTrack		= TRUE;
    BOOL				fValid_pContext			= TRUE;
    BOOL	            fValid_ppResultTrack	= TRUE;
    IDirectMusicTrack*	pdmNewTrack				= NULL;
    IUnknown*			pdmContext				= NULL;
    IDirectMusicTrack*	pdmResultTrack			= NULL;
    IDirectMusicTrack**	ppdmResultTrack			= NULL;

    // validate pNewTrack
    if(!helpIsValidPtr((void*)pNewTrack,sizeof(CtIDirectMusicTrack*),TRUE))
    {
        // bogus pointer, use as such
        fValid_pNewTrack = FALSE;
        pdmNewTrack = (IDirectMusicTrack*)pNewTrack;
    }
    else
    {
        // good pointer, get the real object
        hRes = pNewTrack->GetRealObjPtr(&pdmNewTrack);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }
    // validate pContext
    if(!helpIsValidPtr((void*)pContext,sizeof(CtIUnknown*),TRUE))
    {
        // bogus pointer, use as such
        fValid_pContext = FALSE;
        pdmContext = (IUnknown*)pContext;
    }
    else
    {
        // good pointer, get the real object
        hRes = pContext->GetRealObjPtr(&pdmContext);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ppResultTrack
    if(!helpIsValidPtr((void*)ppResultTrack, sizeof(CtIDirectMusicTrack*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ppResultTrack = FALSE;
        ppdmResultTrack = (IDirectMusicTrack**)ppResultTrack;

    }
    else
    {
        // valid pointer, create a real object
        ppdmResultTrack = &pdmResultTrack;

        // just in case we fail, init test object ptr to NULL
        *ppResultTrack = NULL;
    }
    
	fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Join()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pNewTrack     == %p   %s",
            pdmNewTrack,
            (fValid_pNewTrack) ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "mtJoin        == %08Xh",
            mtJoin);
    fnsLog(PARAMLOGLEVEL, "pContext      == %p   %s",
            pdmContext,
            (fValid_pContext) ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "dwTrackGroup  == %08Xh",
            dwTrackGroup);
    fnsLog(PARAMLOGLEVEL, "ppResultTrack == %p   %s",
            ppdmResultTrack,
            (fValid_ppResultTrack) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicTrack8*)m_pUnk)->Join(pdmNewTrack,mtJoin,pdmContext,dwTrackGroup,ppdmResultTrack);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Join()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes          == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    // create the test object
    if(SUCCEEDED(hRes) && fValid_ppResultTrack)
    {
        fnsLog(PARAMLOGLEVEL, "*ppResultTrack == %p",
                pdmResultTrack);
        hRes = dmthCreateTestWrappedObject(pdmResultTrack, ppResultTrack);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
END:
    if(fValid_pNewTrack && pdmNewTrack)
    {
        pdmNewTrack->Release();
    }
    if(fValid_pContext && pdmContext)
    {
        pdmContext->Release();
    }
    if(fValid_ppResultTrack && pdmResultTrack)
    {
        pdmResultTrack->Release();
    }
    return hRes;

} // *** end CtIDirectMusicTrack8::Join()
