//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       cttrack.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// cttrack.cpp
//
// Test harness implementation of IDirectMusicTrack
//
// Functions:
//  CtIDirectMusicTrack::CtIDirectMusicTrack()
//  CtIDirectMusicTrack::~CtIDirectMusicTrack()
//  CtIDirectMusicTrack::InitTestClass()
//  CtIDirectMusicTrack::GetRealObjPtr()
//  CtIDirectMusicTrack::QueryInterface()
//  CtIDirectMusicTrack::Init()
//  CtIDirectMusicTrack::InitPlay()
//  CtIDirectMusicTrack::EndPlay()
//  CtIDirectMusicTrack::Play()
//  CtIDirectMusicTrack::GetData()
//  CtIDirectMusicTrack::SetData()
//  CtIDirectMusicTrack::TypeSupported()
//  CtIDirectMusicTrack::AddNotify()
//  CtIDirectMusicTrack::RemoveNotify()
//
// History:
//  12/31/1997 - a-llucar - created
//  01/09/1998 - a-llucar - changed Play(), TypeSupported()
//	01/16/1998 - markburt - modified GetData()
//  03/30/1998 - davidkl - implemented inheritance
//===========================================================================

#include "dmth.h"
#include "dmthp.h"
#include "dmthcom.h"


//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicTrack::CtIDirectMusicTrack()
//
// Default constructor
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
CtIDirectMusicTrack::CtIDirectMusicTrack(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicTrack";

} //*** end CtIDirectMusicTrack::CtIDirectMusicTrack()


//===========================================================================
// CtIDirectMusicTrack::~CtIDirectMusicTrack()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
CtIDirectMusicTrack::~CtIDirectMusicTrack(void)
{
    // nothing to do

} //*** end CtIDirectMusicTrack::~CtIDirectMusicTrack()


//===========================================================================
// CtIDirectMusicTrack::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmTrack - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicTrack::InitTestClass(IDirectMusicTrack *pdmTrack)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmTrack, sizeof(IDirectMusicTrack), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmTrack));

} //*** end CtIDirectMusicTrack::InitTestClass()


//===========================================================================
// CtIDirectMusicTrack::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicTrack **ppdmTrack - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  12/29/1997 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicTrack::GetRealObjPtr(IDirectMusicTrack **ppdmTrack)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmTrack, sizeof(IDirectMusicTrack*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmTrack));

} //*** end CtIDirectMusicTrack::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicTrack::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicTrack::QueryInterface(REFIID riid, 
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

} //*** end CtIDirectMusicTrack::QueryInterface


//===========================================================================
// CtIDirectMusicTrack::Init()
//
// Encapsulates calls to Init
//
// History:
//  12/29/1997 - a-llucar - created
//  03/04/1998 - davidkl - modified to use wrapped classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicTrack::Init(CtIDirectMusicSegment* ptdmSegment)
{
    HRESULT             hRes				= E_NOTIMPL;
    BOOL		        fValid_ptdmSegment	= TRUE;
    IDirectMusicSegment *pdmSeg             = NULL;
    
    // validate pSegment
    if(!helpIsValidPtr((void*)ptdmSegment, sizeof(CtIDirectMusicSegment), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmSegment = FALSE;
        pdmSeg = (IDirectMusicSegment*)ptdmSegment;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmSegment->GetRealObjPtr(&pdmSeg);
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Init()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSegment == %p   %s",
            pdmSeg,
            (fValid_ptdmSegment) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->Init(pdmSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Init()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmSegment && pdmSeg)
    {
        pdmSeg->Release();
    }
    return hRes;

} //*** end CtIDirectMusicTrack::Init()


//===========================================================================
// CtIDirectMusicTrack::InitPlay()
//
// Encapsulates calls to InitPlay
//
// History:
//  12/29/1997 - a-llucar - created
//  03/04/1998 - davidkl - modified to use wrapped classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicTrack::InitPlay(CtIDirectMusicSegmentState* ptdmSegmentState,
							        CtIDirectMusicPerformance* ptdmPerformance,
									void** ppStateData, DWORD dwVirtualTrackID, 
									DWORD dwFlags)
{
    HRESULT                     hRes					= E_NOTIMPL;
    BOOL		                fValid_ptdmSegmentState	= TRUE;
	BOOL		                fValid_ptdmPerformance	= TRUE;
	BOOL		                fValid_ppStateData  	= TRUE;
    IDirectMusicSegmentState    *pdmSegSt               = NULL;
    IDirectMusicPerformance     *pdmPerf                = NULL;
    
    // validate pSegmentState
    if(!helpIsValidPtr((void*)ptdmSegmentState, sizeof(CtIDirectMusicSegmentState), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmSegmentState = FALSE;
        pdmSegSt = (IDirectMusicSegmentState*)ptdmSegmentState;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmSegmentState->GetRealObjPtr(&pdmSegSt);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate pPerformance
    if(!helpIsValidPtr((void*)ptdmPerformance, sizeof(CtIDirectMusicPerformance), 
                    FALSE))
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

    // validate ppStateData
    if(!helpIsValidPtr((void*)ppStateData, 1, FALSE))
    {
        fValid_ppStateData = FALSE;

    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::InitPlay()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSegmentState == %p   %s",
            pdmSegSt,
            fValid_ptdmSegmentState ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pPerformance == %p   %s",
            pdmPerf,
            fValid_ptdmPerformance ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppStateData == %p   %s",
            ppStateData,
            fValid_ppStateData ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwVirtualTrackID == %08Xh",
            dwVirtualTrackID);
    fnsLog(PARAMLOGLEVEL, "dwFlags == %08Xh",
            dwFlags);

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->InitPlay(pdmSegSt, pdmPerf, 
                                ppStateData, dwVirtualTrackID, dwFlags);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::InitPlay()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    // BUGBUG - log contents of ppStateData?

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmSegmentState && pdmSegSt)
    {
        pdmSegSt->Release();
    }
    if(fValid_ptdmPerformance && pdmPerf)
    {
        pdmPerf->Release();
    }
    return hRes;

} //*** end CtIDirectMusicTrack::InitPlay()


//===========================================================================
// CtIDirectMusicTrack::EndPlay()
//
// Encapsulates calls to EndPlay
//
// History:
//  12/29/1997 - a-llucar - created
//  03/04/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicTrack::EndPlay(void* pStateData)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_pStateData   = TRUE;

    // validate pStateData
    if(!helpIsValidPtr((void*)pStateData, 1, FALSE))
    {
        fValid_pStateData = FALSE;

    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EndPlay()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStateData == %p   %s",
            pStateData,
            fValid_pStateData ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->EndPlay(pStateData);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EndPlay()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicTrack::EndPlay()


//===========================================================================
// CtIDirectMusicTrack::Play()
//
// Encapsulates calls to Play
//
// History:
//  12/29/1997 - a-llucar - created
//  03/04/1998 - davidkl - modified to use wrapped classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicTrack::Play(void* pStateData, MUSIC_TIME mtStart, 
                                MUSIC_TIME mtEnd, MUSIC_TIME mtOffset, 
                                DWORD dwFlags, 
                                CtIDirectMusicPerformance* ptdmPerf, 
                                CtIDirectMusicSegmentState* ptdmSegSt, 
                                DWORD dwVirtualID)
{
    HRESULT                     hRes				= E_NOTIMPL;
	BOOL		                fValid_ptdmPerf		= TRUE;
    BOOL		                fValid_ptdmSegSt    = TRUE;
    BOOL                        fValid_pStateData   = TRUE;
    IDirectMusicPerformance     *pdmPerf            = NULL;
    IDirectMusicSegmentState    *pdmSegSt           = NULL;

    // validate ptdmSegSt
    if(!helpIsValidPtr((void*)ptdmSegSt, sizeof(CtIDirectMusicSegmentState), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmSegSt = FALSE;
        pdmSegSt = (IDirectMusicSegmentState*)ptdmSegSt;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmSegSt->GetRealObjPtr(&pdmSegSt);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ptdmPerf
    if(!helpIsValidPtr((void*)ptdmPerf, sizeof(CtIDirectMusicPerformance), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_ptdmPerf = FALSE;
        pdmPerf = (IDirectMusicPerformance*)ptdmPerf;

    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmPerf->GetRealObjPtr(&pdmPerf);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate pStateData
    if(!helpIsValidPtr((void*)pStateData, 1, FALSE))
    {
        fValid_pStateData = FALSE;

    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Play()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStateData  == %p   %s",
            pStateData,
            fValid_pStateData ? "" : "BAD");
    // BUGBUG log contents of pStateData?
	fnsLog(PARAMLOGLEVEL, "mtStart     == %08Xh",
            mtStart);
	fnsLog(PARAMLOGLEVEL, "mtEnd       == %08Xh",
            mtEnd);
	fnsLog(PARAMLOGLEVEL, "mtOffset    == %08Xh",
            mtOffset);
	fnsLog(PARAMLOGLEVEL, "dwFlags     == %08Xh",
            dwFlags);
	fnsLog(PARAMLOGLEVEL, "pPerf       == %p   %s",
			pdmPerf,
            fValid_ptdmPerf ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "pSegSt      == %p   %s",
            pdmSegSt,
            fValid_ptdmSegSt ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "dwVirtualID == %08Xh",
            dwVirtualID);

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->Play(pStateData,
                                            mtStart, 
                                            mtEnd, 
                                            mtOffset,
                                            dwFlags, 
                                            pdmPerf, 
                                            pdmSegSt, 
                                            dwVirtualID);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Play()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmSegSt && pdmSegSt)
    {
        pdmSegSt->Release();
    }
    if(fValid_ptdmPerf && pdmPerf)
    {
        pdmPerf->Release();
    }
    return hRes;

} //*** end CtIDirectMusicTrack::Play()


//===========================================================================
// CtIDirectMusicTrack::GetData()
//
// Encapsulates calls to GetData
//
// History:
//  12/29/1997 - a-llucar - created
//	01/16/1998 - markburt - modified
//  03/04/1998 - davidkl - renamed to GetParam, fixed logging
//  03/30/1998 - davidkl - fixed logging (for real this time)
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicTrack::GetParam(REFGUID rguidType, MUSIC_TIME mtTime, 
									 MUSIC_TIME* pmtNext, void* pData)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL	fValid_pmtNext		= TRUE;
    BOOL	fValid_pData		= TRUE;
    char    szGuid[MAX_LOGSTRING];


    // validate pmtNext
    if(!helpIsValidPtr((void*)pmtNext, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtNext = FALSE;
    }

    // validate pData
    if(!helpIsValidPtr((void*)pData, 1, FALSE))
    {
        fValid_pData = FALSE;
    }

    dmthGUIDtoString(rguidType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType  == %s (%s)",
            szGuid, dmthXlatGUID(rguidType));
    fnsLog(PARAMLOGLEVEL, "mtTime     == %08Xh",
            mtTime);
    fnsLog(PARAMLOGLEVEL, "pmtNext    == %p   %s",
            pmtNext,
            fValid_pmtNext ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pData      == %p   %s",
            pData,
            fValid_pData ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->GetParam(rguidType, mtTime, pmtNext, pData);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetData()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    // BUGBUG want to log out params

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicTrack::GetParam()


//===========================================================================
// CtIDirectMusicTrack::SetParam()
//
// Encapsulates calls to SetParam
//
// History:
//  12/29/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed to SetParam, fixed logging
//  03/30/1998 - davidkl - fixed logging (for real this time :)
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicTrack::SetParam(REFGUID rguidType, MUSIC_TIME mtTime, 
										void* pData)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL	fValid_pData		= TRUE;
    char    szGuid[MAX_LOGSTRING];

    // validate pData
    if(!helpIsValidPtr((void*)pData, 1, FALSE))
    {
        fValid_pData = FALSE;
    }

    dmthGUIDtoString(rguidType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType  == %s (%s)",
            szGuid, dmthXlatGUID(rguidType));
    fnsLog(PARAMLOGLEVEL, "mtTime     == %08Xh",
            mtTime);
    fnsLog(PARAMLOGLEVEL, "pData      == %p   %s",
            pData,
            fValid_pData ? "" : "BAD");
    // BUGBUG log contents of pData?

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->SetParam(rguidType, mtTime, pData);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicTrack::SetParam()


//===========================================================================
// CtIDirectMusicTrack::IsParamSupported()
//
// Encapsulates calls to IsParamSupported
//
// History:
//  12/29/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed to IsParamSupported, fixed logging
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicTrack::IsParamSupported(REFGUID rguidType)
{
    HRESULT hRes				= E_NOTIMPL;
    char    szGuid[MAX_LOGSTRING];
    
    dmthGUIDtoString(rguidType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::IsParamSupported()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType == %s (%s)",
            szGuid, dmthXlatGUID(rguidType));

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->IsParamSupported(rguidType);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::IsParamSupported()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicTrack::IsParamSupported()


//===========================================================================
// CtIDirectMusicTrack::AddNotificationType()
//
// Encapsulates calls to AddNotificationType
//
// History:
//  12/29/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed, fixed logging
//  03/30/1998 - davidkl - fixed logging bug
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicTrack::AddNotificationType(REFGUID rguidNotificationType)
{
    HRESULT hRes						    = E_NOTIMPL;
    char    szGuid[MAX_LOGSTRING];
    
    dmthGUIDtoString(rguidNotificationType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::AddNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidNotificationType == %s (%s)",
            szGuid, dmthXlatGUID(rguidNotificationType));

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->AddNotificationType(rguidNotificationType);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::AddNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicTrack::AddNotificationType()


//===========================================================================
// CtIDirectMusicTrack::RemoveNotificationType()
//
// Encapsulates calls to RemoveNotificationType
//
// History:
//  12/29/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed, fixed logging
//  03/30/1998 - davidkl - fixed GUID logging bug
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicTrack::RemoveNotificationType(REFGUID rguidNotificationType)
{
    HRESULT hRes						    = E_NOTIMPL;
    char    szGuid[MAX_LOGSTRING];
    
    dmthGUIDtoString(rguidNotificationType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::RemoveNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidNotificationType == %s (%s)",
            szGuid, dmthXlatGUID(rguidNotificationType));

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->RemoveNotificationType(rguidNotificationType);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::RemoveNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicTrack::RemoveNotify()


//===========================================================================
// CtIDirectMusicTrack::Clone()
//
// Encapsulates calls to Clone
//
// History:
//  03/04/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicTrack::Clone(MUSIC_TIME mtStart, MUSIC_TIME mtEnd, 
                                CtIDirectMusicTrack** pptdmTrack)
{
    HRESULT             hRes				= E_NOTIMPL;
    BOOL	            fValid_pptdmTrack	= TRUE;
    IDirectMusicTrack   *pdmTrack           = NULL;
    IDirectMusicTrack   **ppdmTrack         = NULL;
    
    // validate pptdmTrack
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicTrack pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicTrack::Clone().  Otherwise, we are to create and
    //  return a CtIDirectMusicTrack object
    if(!helpIsValidPtr((void*)pptdmTrack, sizeof(CtIDirectMusicTrack*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmTrack = FALSE;
        ppdmTrack = (IDirectMusicTrack**)pptdmTrack;

    }
    else
    {
        // valid pointer, create a real object
        ppdmTrack = &pdmTrack;

        // just in case we fail, init test object ptr to NULL
        *pptdmTrack = NULL;
    }
    
	fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Clone()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "mtStart   == %08Xh",
            mtStart);
    fnsLog(PARAMLOGLEVEL, "mtEnd     == %08Xh",
            mtEnd);
    fnsLog(PARAMLOGLEVEL, "ppTrack   == %p   %s",
            ppdmTrack,
            (fValid_pptdmTrack) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicTrack*)m_pUnk)->Clone(mtStart, mtEnd, ppdmTrack);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Clone()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmTrack)
    {
        fnsLog(PARAMLOGLEVEL, "*ppTrack == %p",
                pdmTrack);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmTrack, pptdmTrack);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmTrack && pdmTrack)
    {
        pdmTrack->Release();
    }
    return hRes;

} //*** end CtIDirectMusicTrack::Clone()



//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================






