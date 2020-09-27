//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctDSWave.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctsegst8.cpp
//
// Test harness implementation of IDirectSoundWave
//
// Functions:
//    IUnknown
//      CtIDirectSoundWave::QueryInterface()
//      CtIDirectSoundWave::AddRef()
//      CtIDirectSoundWave::Release()
//
//    IDirectSoundWave
//      CtIDirectSoundWave::CreateSource
//      CtIDirectSoundWave::GetFormat
//      CtIDirectSoundWave::GetWaveArticulation
//
// History:
//  02/23/00 - danhaff - created
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectSoundWave::CtIDirectSoundWave()
//
// Default constructor
//
// History:
//  02/23/00 - danhaff - created
//===========================================================================
CtIDirectSoundWave::CtIDirectSoundWave(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectSoundWave";

} // *** end CtIDirectSoundWave::CtIDirectSoundWave()


//===========================================================================
// CtIDirectSoundWave::~CtIDirectSoundWave()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  02/23/00 - danhaff - created
//===========================================================================
CtIDirectSoundWave::~CtIDirectSoundWave(void)
{
    // nothing to do

} // *** end CtIDirectSoundWave::~CtIDirectSoundWave()


//===========================================================================
// CtIDirectSoundWave::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectSoundWave *pDirectSoundWave- pointer to real IDirectSoundWave object
//
// Returns:
//
// History:
//  02/23/00 - danhaff - created
//===========================================================================
HRESULT CtIDirectSoundWave::InitTestClass(IDirectSoundWave *pDirectSoundWave)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pDirectSoundWave, sizeof(IDirectSoundWave), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pDirectSoundWave));

} // *** end CtIDirectSoundWave::InitTestClass()



//===========================================================================
// CtIDirectSoundWave::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectSoundWave **ppDirectSoundWave- ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  02/23/00 - danhaff - created
//===========================================================================
HRESULT CtIDirectSoundWave::GetRealObjPtr(IDirectSoundWave **ppDirectSoundWave)
{

	// validate ppdm
	if(!helpIsValidPtr(ppDirectSoundWave, sizeof(IDirectSoundWave*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppDirectSoundWave));

} // *** end CtIDirectSoundWave::GetRealObjPtr()


//===========================================================================
// CtIDirectSoundWave::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  02/23/00 - danhaff - created
//===========================================================================
HRESULT CtIDirectSoundWave::QueryInterface(REFIID riid,
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
// CtIDirectSoundWave::GetFormat
//
// Encapsulates calls to GetFormat
//
// Parameters:
//    pWfx:
//          holds the returned waveformat.
//    pcbSize:
//          if pWfx NULL, pcbSize will contain the # of bytes needed
//    to hold the wave format on return.  If not NULL, the value in
//    pcbSize should be set to the size of the pWfx buffer before
//    calling this function.
//
// Returns:
//  HRESULT - return code from actual call to GetFormat
//
// History:
//  02/23/00 - danhaff - created
//  03/29/00 - kcraven - mangled to be in sync
//===========================================================================
HRESULT CtIDirectSoundWave::GetFormat
(
	LPWAVEFORMATEX pwfxFormat, 
	DWORD dwSizeAllocated, 
	LPDWORD pdwSizeWritten
)
//BUGBUG
//PHOOPHOO
//HRESULT CtIDirectSoundWave::GetFormat
//(
//    OUT LPWAVEFORMATEX  pWfx,
//    OUT LPDWORD         pcbSize
//)
{
    HRESULT	hRes			= E_FAIL;
    BOOL fValid_pWfx        = TRUE;
//    BOOL fValid_dwSizeAlloc     = TRUE;
    BOOL fValid_pcbSizeWritt     = TRUE;


    fnsIncrementIndent();

    // validate pWfx - we can only guarantee that it is the size of
    //  a PCMWAVEFORMAT (16) w/o checking wFormatTag.
    if(!helpIsValidPtr((void*)pwfxFormat, sizeof(PCMWAVEFORMAT), FALSE))
        fValid_pWfx = FALSE;
//    if(!helpIsValidPtr((void*)dwSizeAllocated, sizeof(DWORD), FALSE))
//        fValid_dwSizeAlloc = FALSE;
    if(!helpIsValidPtr((void*)pdwSizeWritten, sizeof(DWORD), FALSE))
        fValid_pcbSizeWritt = FALSE;

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetFormat()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL,     "pwfxFormat       == %08Xh %s", pwfxFormat,    fValid_pWfx    ? "" : "(BAD)");
//    fnsLog(PARAMLOGLEVEL, "dwSizeAllocated  == %08Xh %s", dwSizeAllocated, fValid_dwSizeAlloc ? "" : "(BAD)");
    fnsLog(PARAMLOGLEVEL,     "dwSizeAllocated  == %08Xh", dwSizeAllocated);
    fnsLog(PARAMLOGLEVEL,     "pdwSizeWritten   == %08Xh %s", pdwSizeWritten, fValid_pcbSizeWritt ? "" : "(BAD)");
  //  if (fValid_dwSizeAlloc)
  //      fnsLog(PARAMLOGLEVEL, "*dwSizeAllocated  == %08Xh", *dwSizeAllocated);
    if (fValid_pcbSizeWritt)
        fnsLog(PARAMLOGLEVEL, "*pdwSizeWritten  == %08Xh", *pdwSizeWritten);
        

    // call the real function
    hRes = ((IDirectSoundWave*)m_pUnk)->GetFormat(pwfxFormat, dwSizeAllocated, pdwSizeWritten);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetFormat()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);    
    if (fValid_pWfx)
        dmthLogWAVEFORMATEX(PARAMLOGLEVEL, pwfxFormat);
//    if (fValid_dwSizeAlloc)
//        fnsLog(PARAMLOGLEVEL, "*dwSizeAllocated  == %08Xh", *dwSizeAllocated);
    if (fValid_pcbSizeWritt)
        fnsLog(PARAMLOGLEVEL, "*pdwSizeWritten  == %08Xh", *pdwSizeWritten);

    fnsDecrementIndent();
    return hRes;

} // *** end CtIDirectSoundWave::GetFormat()




//===========================================================================
// CtIDirectSoundWave::CreateSource
//
// Encapsulates calls to CreateSource
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to CreateSource
//
// History:
//  02/23/00 - danhaff - created
//===========================================================================
HRESULT CtIDirectSoundWave::CreateSource
(
    OUT IDirectSoundSource **ppSource,
    IN  LPWAVEFORMATEX pWfx,
    DWORD dwFlags
)
{
    HRESULT	hRes			= E_FAIL;
    BOOL fValid_pWfx        = TRUE;
    BOOL fValid_ppSource    = TRUE;

    fnsIncrementIndent();

    // validate pWfx - we can only guarantee that it is the size of
    //  a PCMWAVEFORMAT (16) w/o checking wFormatTag.
    if(!helpIsValidPtr((void*)pWfx, sizeof(PCMWAVEFORMAT), FALSE))
        fValid_pWfx = FALSE;

    //validate ppSource.
    if(!helpIsValidPtr((void*)ppSource, sizeof(IDirectSoundSource *), FALSE))
        fValid_ppSource= FALSE;


    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::CreateSource()",
            m_szInterfaceName);

    fnsLog(PARAMLOGLEVEL, "ppSource == %08Xh %s", ppSource, fValid_ppSource ? "" : "(BAD)");
    fnsLog(PARAMLOGLEVEL, "pWfx     == %08Xh %s", pWfx,     fValid_pWfx     ? "" : "(BAD)");
    if (fValid_pWfx)
        dmthLogWAVEFORMATEX(PARAMLOGLEVEL, pWfx);
    fnsLog(PARAMLOGLEVEL, "dwFlags  == %08Xh", dwFlags);
        

    // call the real function
    hRes = ((IDirectSoundWave*)m_pUnk)->CreateSource(ppSource, pWfx, dwFlags);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::CreateSource()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);    
    if (fValid_ppSource)
        fnsLog(PARAMLOGLEVEL, "*ppSource  == %08Xh", *ppSource);
    fnsDecrementIndent();


    return hRes;

} // *** end CtIDirectSoundWave::GetFormat()



//===========================================================================
// CtIDirectSoundWave::GetWaveArticulation
//
// Encapsulates calls to GetWaveArticulation
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetWaveArticulation
//
// History:
//  02/23/00 - danhaff - created
//  03/29/00 - danhaff - commented out - it's removed from API.
//  03/29/00 - kcraven - removed --- mee too -- mee merged
//===========================================================================
/*
HRESULT CtIDirectSoundWave::GetWaveArticulation
(
    OUT LPDMUS_WAVEART pArticulation
)
{
    HRESULT	hRes			= E_FAIL;
    BOOL fValid_pArticulation = TRUE;


    fnsIncrementIndent();

    // validate params.
    if(!helpIsValidPtr((void*)pArticulation, sizeof(DMUS_WAVEART), FALSE))
        fValid_pArticulation= FALSE;


    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::CreateSource()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pArticulation == %08Xh %s", pArticulation, fValid_pArticulation ? "" : "(BAD)");
        

    // call the real function
    hRes = ((IDirectSoundWave*)m_pUnk)->GetWaveArticulation(pArticulation);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::CreateSource()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);    
    
    if (fValid_pArticulation)
        dmthLogDMUS_WAVEART(PARAMLOGLEVEL, pArticulation);

    fnsDecrementIndent();
    return hRes;

} // *** end CtIDirectSoundWave::GetFormat()
*/


