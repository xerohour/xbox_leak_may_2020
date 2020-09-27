//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctsegst.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctsegst8.cpp
//
// Test harness implementation of IDirectMusicSegmentState8
//
// Functions:
//    IUnknown
//      CtIDirectMusicSegmentState8::QueryInterface()
//      CtIDirectMusicSegmentState8::AddRef()
//      CtIDirectMusicSegmentState8::Release()
//
//    IDirectMusicSegmentState
//      CtIDirectMusicSegmentState8::GetRepeats()
//      CtIDirectMusicSegmentState8::GetSegment()
//      CtIDirectMusicSegmentState8::GetStartTime()
//      CtIDirectMusicSegmentState8::GetSeek()
//      CtIDirectMusicSegmentState8::GetStartPoint()
//
//    IDirectMusicSegmentState8
//      CtIDirectMusicSegmentState8::SetTrackConfig()
//      CtIDirectMusicSegmentState8::GetTrackConfig()
//      CtIDirectMusicSegmentState8::GetObjectInPath()
//      CtIDirectMusicSegmentState8::Transition()
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================

#include "globals.h"
//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicSegmentState8::CtIDirectMusicSegmentState()
//
// Default constructor
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
CtIDirectMusicSegmentState8::CtIDirectMusicSegmentState8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicSegmentState8";

} // *** end CtIDirectMusicSegmentState8::CtIDirectMusicSegmentState8()


//===========================================================================
// CtIDirectMusicSegmentState8::~CtIDirectMusicSegmentState8()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
CtIDirectMusicSegmentState8::~CtIDirectMusicSegmentState8(void)
{
    // nothing to do

} // *** end CtIDirectMusicSegmentState8::~CtIDirectMusicSegmentState8()


//===========================================================================
// CtIDirectMusicSegmentState8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmSegmentState - pointer to real
//                                      IDirectMusic object
//
// Returns:
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::InitTestClass(IDirectMusicSegmentState8 *pdmSegmentState8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmSegmentState8, sizeof(IDirectMusicSegmentState8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmSegmentState8));

} // *** end CtIDirectMusicSegmentState8::InitTestClass()


//===========================================================================
// CtIDirectMusicSegmentState8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicSegmentState8 **ppdmSegmentState - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::GetRealObjPtr(IDirectMusicSegmentState8 **ppdmSegmentState8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmSegmentState8, sizeof(IDirectMusicSegmentState8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmSegmentState8));

} // *** end CtIDirectMusicSegmentState8::GetRealObjPtr()

/*
//===========================================================================
// CtIDirectMusicSegmentState8::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::QueryInterface(REFIID riid,
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

} // *** end CtIDirectMusicSegmentState8::QueryInterface


//===========================================================================
// CtIDirectMusicSegmentState8::GetRepeats()
//
// Encapsulates calls to GetRepeats
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::GetRepeats(DWORD* pdwRepeats)
{
    HRESULT     hRes						= E_NOTIMPL;
    BOOL		fValid_pdwRepeats			= TRUE;

    // validate pGraph
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
            fValid_pdwRepeats ? "" : "BAD");

    // call the real function
    hRes = ((CtIDirectMusicSegmentState8*)m_pUnk)->GetRepeats(pdwRepeats);

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

} // *** end CtIDirectMusicSegmentState8::GetRepeats()

*/
//===========================================================================
// CtIDirectMusicSegmentState::GetSegment()
//
// Encapsulates calls to GetSegment
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::GetSegment(CtIDirectMusicSegment** pptdmSegment)
{
    HRESULT             hRes				= E_NOTIMPL;
    BOOL		        fValid_pptdmSegment	= TRUE;
    IDirectMusicSegment *pdmSeg             = NULL;
    IDirectMusicSegment **ppdmSeg           = NULL;

    // validate pptdmSegment
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegment pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicSegmentState8::GetSegment().  Otherwise, we are to create and
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

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetSegment()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppSegment == %p   %s",
            ppdmSeg,
            fValid_pptdmSegment ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->GetSegment(ppdmSeg);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetSegment()",
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

} // *** end CtIDirectMusicSegmentState8::GetSegment()


//===========================================================================
// CtIDirectMusicSegmentState8::GetStartTime()
//
// Encapsulates calls to GetStartTime
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::GetStartTime(MUSIC_TIME* pmtOffset)
{
    HRESULT     hRes						= E_NOTIMPL;
    BOOL		fValid_pmtOffset			= TRUE;

    // validate pmtOffset
    if(!helpIsValidPtr((void*)pmtOffset,
                        sizeof(MUSIC_TIME),
                        FALSE))
    {
        fValid_pmtOffset	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetStartTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pmtOffset == %p   %s",
            pmtOffset,
            fValid_pmtOffset ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->GetStartTime(pmtOffset);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetStartTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pmtOffset)
    {
        fnsLog(PARAMLOGLEVEL, "*pmtOffset == %08Xh",
                *pmtOffset);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegmentState8::GetStartTime()

/*


//===========================================================================
// CtIDirectMusicSegmentState::GetSeek()
//
// Encapsulates calls to GetSeek
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::GetSeek(MUSIC_TIME* pmtSeek)
{
    HRESULT     hRes						= E_NOTIMPL;
    BOOL		fValid_pmtSeek				= TRUE;

    // validate pmtOffset
    if(!helpIsValidPtr((void*)pmtSeek, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtSeek	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetSeek()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pmtSeek == %p   %s",
            pmtSeek,
            fValid_pmtSeek ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->GetSeek(pmtSeek);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetSeek()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes     == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pmtSeek)
    {
        fnsLog(PARAMLOGLEVEL, "*pmtSeek == %08Xh",
                *pmtSeek);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegmentState8::GetSeek()


//===========================================================================
// CtIDirectMusicSegmentState8::GetStartPoint()
//
// Encapsulates calls to GetStartPoint
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::GetStartPoint(MUSIC_TIME *pmtStart)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_pmtStart = TRUE;

    // validate pmtStart
    if(!helpIsValidPtr((void*)pmtStart, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtStart = FALSE;
    }

	fnsIncrementIndent();

	// log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetStartPoint()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pmtStart == %p   %s",
            pmtStart,
            fValid_pmtStart ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->GetStartPoint(pmtStart);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetStartPoint()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
            dmthXlatHRESULT(hRes), hRes);
    if(fValid_pmtStart)
    {
        fnsLog(PARAMLOGLEVEL, "*pmtStart == %08Xh",
                *pmtStart);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegmentState8::GetStartPoint()


*/

//===========================================================================
// CtIDirectMusicSegmentState8::SetTrackConfig()
//
// Encapsulates calls to SetTrackConfig
//
// History:
//  10/12/1999 - kcraven - created
//  03/17/2000 - kcraven - fixed to match dmusic change
//===========================================================================
/*
HRESULT CtIDirectMusicSegmentState8::SetTrackConfig(
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
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->SetTrackConfig(
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

} // *** end CtIDirectMusicSegmentState8::SetTrackConfig()




//===========================================================================
// CtIDirectMusicSegmentState8::GetTrackConfig()
//
// Encapsulates calls to GetTrackConfig
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicSegmentState8::GetTrackConfig(
	REFGUID rguidTrackClassID,
	DWORD dwGroup,
	DWORD dwIndex,
	DWORD *pdwFlags)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_rguidTrackClassID  = TRUE;
    BOOL    fValid_pdwFlags  = TRUE;
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
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->GetTrackConfig(
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

} // *** end CtIDirectMusicSegmentState8::GetTrackConfig()
*/



//===========================================================================
// CtIDirectMusicSegmentState8::GetObjectInPath()
//
// Encapsulates calls to GetObjectInPath
//
// History:
//  10/12/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::GetObjectInPath(
    DWORD dwPChannel,
	DWORD dwStage,
	DWORD dwBuffer,
	REFGUID guidObject,
	DWORD dwIndex,
	REFGUID iidInterface,
	void** ppObject)
{
    HRESULT hRes					= E_NOTIMPL;
    BOOL    fValid_guidObject		= TRUE;
    BOOL    fValid_iidInterface		= TRUE;
    BOOL    fValid_ppObject			= TRUE;
    char    szGuidObj[MAX_LOGSTRING];
    char    szGuidIID[MAX_LOGSTRING];

    dmthGUIDtoString(guidObject, szGuidObj);
    dmthGUIDtoString(iidInterface, szGuidIID);

    // validate ppObject
    if(!helpIsValidPtr((void*)ppObject,1, FALSE))
    {
        fValid_ppObject = FALSE;
    }

	fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetObjectInPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwPChannel   == %08Xh%s",
            dwPChannel,
            dwPChannel==DMUS_PCHANNEL_ALL ? " (DMUS_PCHANNEL_ALL)":"");
    fnsLog(PARAMLOGLEVEL, "dwStage      == %08Xh (%s)",
            dwStage, dmthXlatDMUS_STAGE(dwStage));
    fnsLog(PARAMLOGLEVEL, "dwBuffer     == %08Xh",
            dwBuffer);
    fnsLog(PARAMLOGLEVEL, "guidObject   == %s (%s)",
            szGuidObj, dmthXlatGUID(guidObject));
    fnsLog(PARAMLOGLEVEL, "dwIndex      == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "iidInterface == %s (%s)",
            szGuidIID, dmthXlatGUID(iidInterface));
    fnsLog(PARAMLOGLEVEL, "ppObject     == %p   %s",
            ppObject,
            (fValid_ppObject) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->GetObjectInPath(
		dwPChannel,
		dwStage,
		dwBuffer,
		guidObject,
		dwIndex,
		iidInterface,
		ppObject
		);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetObjectInPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
            dmthXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegmentState8::GetObjectInPath()





//===========================================================================
// CtIDirectMusicSegmentState8::SetVolume()
//
// Encapsulates calls to SetVolume
//
// Parameters:
//  
//  lVolume:   Gain, in 100ths of a dB.
//  dwDuration Duration of volume ramp. Note that 0 is more efficient.
//
// History:
//  04/05/2000 - danhaff - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::SetVolume(long lVolume, DWORD dwDuration)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pdwPChannelOut   = TRUE;

    fnsIncrementIndent();


    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetVolume()", m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "lVolume == %d", lVolume);
	fnsLog(PARAMLOGLEVEL, "dwDuration == %u", dwDuration);

    // call the real function
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->SetVolume(lVolume, dwDuration);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetVolume()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)", tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();


    return hRes;
}


//===========================================================================
// CtIDirectMusicSegmentState8::SetPitch()
//
// Encapsulates calls to SetPitch
//
// Parameters:
//  
//  lPitch:    Pitch bend, in 100ths of a semitone.
//  dwDuration Duration of Pitch ramp. Note that 0 is more efficient.
//
// History:
//  04/05/2000 - danhaff - created
//===========================================================================
HRESULT CtIDirectMusicSegmentState8::SetPitch(long lPitch, DWORD dwDuration)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pdwPChannelOut   = TRUE;

    fnsIncrementIndent();


    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetPitch()", m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "lPitch == %d", lPitch);
	fnsLog(PARAMLOGLEVEL, "dwDuration == %u", dwDuration);

    // call the real function
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->SetPitch(lPitch, dwDuration);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetPitch()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)", tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();


    return hRes;
}



//===========================================================================
// CtIDirectMusicSegmentState8::Transition()
//
// Encapsulates calls to Transition
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicSegmentState8::Transition(DWORD dwDestinationSegment)
{
    HRESULT hRes            = E_NOTIMPL;

	fnsIncrementIndent();

	// log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Transition()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwDestinationSegment == %08Xh",
            dwDestinationSegment);

    // call the real function
    hRes = ((IDirectMusicSegmentState8*)m_pUnk)->Transition(dwDestinationSegment);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Transition()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
            dmthXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSegmentState8::Transition()
*/












