//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctaudpth.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctaudpth.cpp
//
// Test harness implementation of IDirectMusicAudioPath
//
// Functions:
//  CtIDirectMusicAudioPath::CtIDirectMusicAudioPath()
//  CtIDirectMusicAudioPath::~CtIDirectMusicAudioPath()
//  CtIDirectMusicAudioPath::InitTestClass()
//  CtIDirectMusicAudioPath::GetRealObjPtr()
//  CtIDirectMusicAudioPath::QueryInterface()
//  CtIDirectMusicAudioPath::GetObjectInPath()
//  CtIDirectMusicAudioPath::ConvertPChannel()
//
// History:
//  12/17/1999 - kcraven - created
//===========================================================================

#include "globals.h"


//===========================================================================
// CtIDirectMusicAudioPath::CtIDirectMusicAudioPath()
//
// Default constructor
//
// History:
//  12/17/1999 - kcraven - created
//===========================================================================
CtIDirectMusicAudioPath::CtIDirectMusicAudioPath(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicAudioPath";

} //*** end CtIDirectMusicAudioPath::CtIDirectMusicAudioPath()


//===========================================================================
// CtIDirectMusicAudioPath::~CtIDirectMusicAudioPath()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  12/17/1999 - kcraven - created
//===========================================================================
CtIDirectMusicAudioPath::~CtIDirectMusicAudioPath(void)
{
    // nothing to do

} //*** end CtIDirectMusicAudioPath::~CtIDirectMusicAudioPath()


//===========================================================================
// CtIDirectMusicAudioPath::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicAudioPath *pdmAudioPath - pointer to real
//                                      IDirectMusic object
//
// Returns:
//
// History:
//  12/17/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicAudioPath::InitTestClass(IDirectMusicAudioPath *pdmAudioPath)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmAudioPath, sizeof(IDirectMusicAudioPath), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmAudioPath));

} //*** end CtIDirectMusicAudioPath::InitTestClass()


//===========================================================================
// CtIDirectMusicAudioPath::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	CtIDirectMusicAudioPath **ppdmAudioPath - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  12/17/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicAudioPath::GetRealObjPtr(IDirectMusicAudioPath **ppdmAudioPath)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmAudioPath, sizeof(IDirectMusicAudioPath*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmAudioPath));

} //*** end CtIDirectMusicAudioPath::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicAudioPath::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  12/17/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicAudioPath::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicAudioPath::QueryInterface()


//===========================================================================
// CtIDirectMusicAudioPath::GetObjectInPath()
//
// Encapsulates calls to GetObjectInPath
//
// History:
//  12/17/1999 - kcraven - created
//  04/04/200 - kcraven - sync to dmusic changes
//===========================================================================
HRESULT CtIDirectMusicAudioPath::GetObjectInPath(
	DWORD dwPChannel,
	DWORD dwStage,
	DWORD dwBuffer,
	REFGUID guidObject,
	DWORD dwIndex,
	REFGUID iidInterface,
	void ** ppObject)
{
    HRESULT hRes					= E_NOTIMPL;
    BOOL    fValid_ppObject			= TRUE;
    char    szGuidObj[MAX_LOGSTRING];
    char    szGuidIID[MAX_LOGSTRING];

    dmthGUIDtoString(guidObject, szGuidObj);
    dmthGUIDtoString(iidInterface, szGuidIID);

    if(!helpIsValidPtr((void*)ppObject, sizeof(void**), FALSE))
    {
        // bogus pointer, use as such
        fValid_ppObject = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetObjectInPath()",
								m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "dwPChannel      == %08Xh%s",
								dwPChannel,
                                dwPChannel==DMUS_PCHANNEL_ALL ? " (DMUS_PCHANNEL_ALL)":"");
	fnsLog(PARAMLOGLEVEL, "dwStage      == %08Xh (%s)",
								dwStage, dmthXlatDMUS_STAGE(dwStage));
	fnsLog(PARAMLOGLEVEL, "dwBuffer      == %08Xh",
								dwBuffer);
    fnsLog(PARAMLOGLEVEL, "guidObject == %s (%s)",
								szGuidObj, dmthXlatGUID(guidObject));
	fnsLog(PARAMLOGLEVEL, "dwIndex      == %08Xh",
								dwIndex);
    fnsLog(PARAMLOGLEVEL, "iidInterface == %s (%s)",
								szGuidIID, dmthXlatGUID(iidInterface));
    fnsLog(PARAMLOGLEVEL, "ppObject == %p   %s",
								ppObject,
								(fValid_ppObject) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicAudioPath*)m_pUnk)->GetObjectInPath(
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
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
	if(fValid_ppObject)
	{
    	fnsLog(PARAMLOGLEVEL, "*ppObject == %p",
				ppObject);
	}

    fnsDecrementIndent();


    return hRes;

} //*** end CtIDirectMusicAudioPath::GetObjectInPath()


//===========================================================================
// CtIDirectMusicAudioPath::Activate()
//
// Encapsulates calls to Activate
//
// Parameters:
//  fActivate: True to activate, False to deactivate.
//
// History:
//  04/05/2000 - danhaff - created
//===========================================================================
HRESULT CtIDirectMusicAudioPath::Activate(BOOL fActivate)
{
    HRESULT hRes					= E_NOTIMPL;
    BOOL    fValid_pdwPChannelOut			= TRUE;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Activate()", m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "fActivate == %s", fActivate ? "TRUE" : "FALSE");

    // call the real function
    hRes = ((IDirectMusicAudioPath*)m_pUnk)->Activate(fActivate);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Activate()",m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)", tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    return hRes;


}

//===========================================================================
// CtIDirectMusicAudioPath::SetVolume()
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
HRESULT CtIDirectMusicAudioPath::SetVolume(long lVolume, DWORD dwDuration)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pdwPChannelOut   = TRUE;

    fnsIncrementIndent();


    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetVolume()", m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "lVolume == %d", lVolume);
	fnsLog(PARAMLOGLEVEL, "dwDuration == %u", dwDuration);

    // call the real function
    hRes = ((IDirectMusicAudioPath*)m_pUnk)->SetVolume(lVolume, dwDuration);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetVolume()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)", tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();


    return hRes;
}



//===========================================================================
// CtIDirectMusicAudioPath::SetPitch()
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
HRESULT CtIDirectMusicAudioPath::SetPitch(long lPitch, DWORD dwDuration)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pdwPChannelOut   = TRUE;

    fnsIncrementIndent();


    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetPitch()", m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "lPitch == %d", lPitch);
	fnsLog(PARAMLOGLEVEL, "dwDuration == %u", dwDuration);

    // call the real function
    hRes = ((IDirectMusicAudioPath*)m_pUnk)->SetPitch(lPitch, dwDuration);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetPitch()", m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)", tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();


    return hRes;
}



                                           




//===========================================================================
// CtIDirectMusicAudioPath::ConvertPChannel()
//
// Encapsulates calls to ConvertPChannel
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicAudioPath::ConvertPChannel(DWORD dwPChannelIn,DWORD *pdwPChannelOut)
{
    HRESULT hRes					= E_NOTIMPL;
    BOOL    fValid_pdwPChannelOut			= TRUE;

    if(!helpIsValidPtr((void*)pdwPChannelOut, sizeof(DWORD), FALSE))
    {
        // bogus pointer, use as such
        fValid_pdwPChannelOut = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ConvertPChannel()",
								m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "dwPChannelIn   == %08Xh",
								dwPChannelIn);
    fnsLog(PARAMLOGLEVEL, "pdwPChannelOut == %p   %s",
								pdwPChannelOut,
								(fValid_pdwPChannelOut) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicAudioPath*)m_pUnk)->ConvertPChannel(dwPChannelIn,pdwPChannelOut);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ConvertPChannel()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
	if(fValid_pdwPChannelOut)
	{
    	fnsLog(PARAMLOGLEVEL, "*pdwPChannelOut == %p",
				*pdwPChannelOut);
	}

    fnsDecrementIndent();


    return hRes;

} //*** end CtIDirectMusicAudioPath::ConvertPChannel()

*/
