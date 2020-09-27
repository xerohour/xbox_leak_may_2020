//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctport8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctport8.cpp
//
// Test harness implementation of IDirectMusicPort8
//
// Functions:
//    CtIDirectMusicPort8::DownloadWave()
//    CtIDirectMusicPort8::UnloadWave()
//    CtIDirectMusicPort8::AllocVoice()
//    CtIDirectMusicPort8::AssignChannelToBuses()
//
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"
#include "dmthcom.h"


//===========================================================================
// CtIDirectMusicPort8::CtIDirectMusicPort8()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
CtIDirectMusicPort8::CtIDirectMusicPort8()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicPort8";

} // *** end CtIDirectMusicPort8::CtIDirectMusicPort8()


//===========================================================================
// CtIDirectMusicPort8::~CtIDirectMusicPort8()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
CtIDirectMusicPort8::~CtIDirectMusicPort8(void)
{
    // nothing to do

} // *** emd CtIDirectMusicPort8::~CtIDirectMusicPort8()


//===========================================================================
// CtIDirectMusicPort8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicPort8 object pointer for future use.
//
// Parameters:
//  IDirectMusicPort8    *pdmPort8    - pointer to real IDirectMusicPort8 object
//
// Returns:
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPort8::InitTestClass(IDirectMusicPort8 *pdmPort8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmPort8, sizeof(IDirectMusicPort8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmPort8));

} // *** end CtIDirectMusicPort8::InitTestClass()


//===========================================================================
// CtIDirectMusicPort8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicPort8 **ppdmPort8 - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPort8::GetRealObjPtr(IDirectMusicPort8 **ppdmPort8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmPort8, sizeof(IDirectMusicPort8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmPort8));

} // *** end CtIDirectMusicPort8::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicPort8::DownloadWave
//
// Encapsulates calls to DownloadWave
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to DownloadWave
//
// History:
//  10/14/1999 - kcraven - created
//  02/25/2000 - danhaff - filled out.
//===========================================================================
HRESULT CtIDirectMusicPort8::DownloadWave
(
	IN  CtIDirectSoundWave *ptWave,
	OUT CtIDirectSoundDownloadedWave **pptDLWave
)
{
    HRESULT hRes = E_FAIL;
    BOOL	fValid_ptWave	= TRUE;
    BOOL	fValid_pptDLWave= TRUE;

    IDirectSoundWave *pWave               = NULL;
    IDirectSoundDownloadedWave **ppDLWave = NULL;
    IDirectSoundDownloadedWave * pDLWave  = NULL;
    

  // validate ptWave
    if(!helpIsValidPtr((void*)ptWave, sizeof(CtIDirectSoundWave), FALSE))
    {
        // bogus pointer, use as such
        fValid_ptWave = FALSE;
        pWave = (IDirectSoundWave*)ptWave;

    }
    else
    {
        // valid pointer, create a real object
        hRes = ptWave->GetRealObjPtr(&pWave);
        if(FAILED(hRes))
        {
        fnsLog(PARAMLOGLEVEL, "Couldn't GetRealObjPtr from CtIDirectSoundWave hr=%s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
            goto TEST_END;
        }
    }


    // validate pptDLWave
    if(!helpIsValidPtr((void*)pptDLWave, sizeof(CtIDirectSoundDownloadedWave *), FALSE))
    {
        // bogus pointer, use as such
        fValid_pptDLWave = FALSE;
        ppDLWave = (IDirectSoundDownloadedWave**)pptDLWave;
    }
    else
    {
        // valid pointer, create a real object (later)
        ppDLWave = &pDLWave;

    }

	fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::DownloadWave()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pWave == %p   %s",
            ptWave,
            (fValid_ptWave) ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL,
            "ppDLWave == %p   %s",
            pptDLWave,
            (fValid_pptDLWave) ? "" : "BAD");

    hRes = ((IDirectMusicPort8*)m_pUnk)->DownloadWave(pWave,ppDLWave);

   // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::DownloadWave()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);    
    if (fValid_pptDLWave)
        fnsLog(PARAMLOGLEVEL, "*pDLWave == %p", *ppDLWave);

	fnsDecrementIndent();


    //Wrap the returned downloaded wave.
    if (SUCCEEDED(hRes))
    {
        HRESULT hResWrap;
        hResWrap = dmthCreateTestWrappedObject(pDLWave, pptDLWave);
        if(FAILED(hResWrap))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hResWrap);
            hRes = hResWrap;
        }
    }


TEST_END:
    //Release the real objects we've created, if they exist.
    SAFE_RELEASE(pDLWave);
    SAFE_RELEASE(pWave);
    return hRes;
} // *** end CtIDirectMusicPort8::DownloadWave()




//===========================================================================
// CtIDirectMusicPort8::UnloadWave
//
// Encapsulates calls to UnloadWave
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to UnloadWave
//
// History:
//  10/14/1999 - kcraven - created
//  02/25/2000 - danhaff - filled out.
//===========================================================================
HRESULT CtIDirectMusicPort8::UnloadWave
(
	IN CtIDirectSoundDownloadedWave *ptWave
)
{
IDirectSoundDownloadedWave *pWave = NULL;
BOOL fValid_ptWave                = TRUE;
HRESULT hRes                      = E_FAIL;

  // validate ptWave
    if(!helpIsValidPtr((void*)ptWave, sizeof(CtIDirectSoundWave), FALSE))
    {
        // bogus pointer, use as such
        fValid_ptWave = FALSE;
        pWave = (IDirectSoundDownloadedWave*)ptWave;

    }
    else
    {
        // valid pointer, create a real object
        hRes = ptWave->GetRealObjPtr(&pWave);
        if(FAILED(hRes))
        {
        fnsLog(PARAMLOGLEVEL, "Couldn't GetRealObjPtr from CtIDirectSoundWave hr=%s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // log inputs
	fnsIncrementIndent();
    fnsLog(CALLLOGLEVEL, "--- Calling %s::UnloadWave()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pWave == %p   %s",
            ptWave,
            (fValid_ptWave) ? "" : "BAD");	

    //Call function.
    hRes = ((IDirectMusicPort8*)m_pUnk)->UnloadWave(pWave);

   // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::UnloadWave()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);    

	fnsDecrementIndent();


END:
    //Release real object obtained from wrapper.
    SAFE_RELEASE(pWave);
    return hRes;
} // *** end CtIDirectMusicPort8::UnloadWave()




//===========================================================================
// CtIDirectMusicPort8::AllocVoice
//
// Encapsulates calls to AllocVoice
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to AllocVoice
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPort8::AllocVoice
        (
         IN CtIDirectSoundDownloadedWave *pWave,    // Wave to play on this voice
         IN DWORD dwChannel,                        // Channel and channel group
         IN DWORD dwChannelGroup,                   //  this voice will play on
         IN REFERENCE_TIME rtStart,                 // Start position (stream only)
         IN SAMPLE_TIME stLoopStart,                // Loop start (one-shot only)
         IN SAMPLE_TIME stLoopEnd,                  // Loop end (one-shot only)
         OUT CtIDirectMusicVoice **ppVoice          // Returned voice
        )        
//HRESULT CtIDirectMusicPort8::AllocVoice
//    (
//     IN CtIDirectSoundDownloadedWave *pWave,      /* Wave to play on this voice */
//     IN DWORD dwChannel,                        /* Channel and channel group */
//     IN DWORD dwChannelGroup,                   /*  this voice will play on */
//     IN REFERENCE_TIME rtStart,                 /* Start position (stream only) */
//     IN REFERENCE_TIME rtReadahead,             /* How much to read ahead (stream only) */
//     IN SAMPLE_TIME stLoopStart,                /* Loop start (one-shot only) */
//     IN SAMPLE_TIME stLoopEnd,                  /* Loop end (one-shot only) */
//     OUT CtIDirectMusicVoice **ppVoice            /* Returned voice */
//    )
//HRESULT CtIDirectMusicPort8::AllocVoice
//(
//	IN CtIDirectSoundDownloadedWave *pWave,
//	IN DWORD dwChannel,
//	IN DWORD dwChannelGroup,
//	IN REFERENCE_TIME rtStart,
//	IN REFERENCE_TIME rtReadahead,
//	OUT CtIDirectMusicVoice **ppVoice
//)
{
    HRESULT	hRes			= E_FAIL;
    BOOL	fValid_pWave	= TRUE;
    BOOL	fValid_ppVoice	= TRUE;
	IDirectSoundDownloadedWave*	pdmWave		= NULL;
	IDirectMusicVoice*			pdmVoice	= NULL;
	IDirectMusicVoice**			ppdmVoice	= NULL;


    // validate pWave
    if(!helpIsValidPtr((void*)pWave,sizeof(CtIDirectSoundDownloadedWave),FALSE))
    {
        // bogus pointer, use as such
        fValid_pWave = FALSE;
        pdmWave = (IDirectSoundDownloadedWave*)pWave;
    }
    else
    {
        // valid pointer, create a real object
        hRes = pWave->GetRealObjPtr(&pdmWave);
        if(FAILED(hRes))
        {
            fnsLog(PARAMLOGLEVEL, "Couldn't GetRealObjPtr from CtIDirectSoundDownloadedWave hr=%s (%08Xh)",
                tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ppVoice
    if(!helpIsValidPtr((void*)ppVoice,sizeof(CtIDirectMusicVoice*),FALSE))
    {
        fValid_ppVoice = FALSE;

        // we have a bogus pointer, use as the real thing
        ppdmVoice = (IDirectMusicVoice**)ppVoice;

    }
    else
    {
        // valid pointer, use a real object
        ppdmVoice = &pdmVoice;

        // just in case we fail, init test object ptr to NULL
        *ppVoice = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::AllocVoice()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pWave          == %p   %s",
            pdmWave,fValid_pWave ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwChannel      == %08Xh",
            dwChannel);
    fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh",
            dwChannelGroup);
    fnsLog(PARAMLOGLEVEL, "rtStart        == %016Xh",
            rtStart);
//    fnsLog(PARAMLOGLEVEL, "rtReadahead      == %016Xh",
//            rtReadahead);
    fnsLog(PARAMLOGLEVEL, "stLoopStart    == %016Xh",
            stLoopStart);
    fnsLog(PARAMLOGLEVEL, "stLoopEnd      == %016Xh",
            stLoopEnd);
    fnsLog(PARAMLOGLEVEL, "ppVoice        == %p   %s",
            ppdmVoice,
            fValid_ppVoice ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort8*)m_pUnk)->AllocVoice(pdmWave,dwChannel,dwChannelGroup,rtStart,stLoopStart,stLoopEnd,ppdmVoice);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::AllocVoice()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_ppVoice)
    {
        fnsLog(PARAMLOGLEVEL, "*ppVoice == %p",ppdmVoice);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmVoice,ppVoice);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();
END:
    if(fValid_pWave && pdmWave)
    {
        pdmWave->Release();
    }
    if(fValid_ppVoice && pdmVoice)
    {
        pdmVoice->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPort8::AllocVoice()




//===========================================================================
// CtIDirectMusicPort8::AssignChannelToBuses
//
// Encapsulates calls to AssignChannelToBuses
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to AssignChannelToBuses
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPort8::AssignChannelToBuses
(
	IN DWORD dwChannelGroup,
	IN DWORD dwChannel,
	IN LPDWORD pdwBusses,
	IN DWORD cBussCount
)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_pdwBusses	= TRUE;


    // validate pdwBusses
    if(!helpIsValidPtr((void*)pdwBusses, sizeof(DWORD*), FALSE))
    {
        // bogus pointer
        fValid_pdwBusses = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::AssignChannelToBuses()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh",
            dwChannelGroup);
    fnsLog(PARAMLOGLEVEL, "dwChannel      == %08Xh",
            dwChannel);
    fnsLog(PARAMLOGLEVEL, "pdwBusses      == %p   %s",
            pdwBusses,
            fValid_pdwBusses ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "cBussCount     == %08Xh",
            cBussCount);

    // call the real function
    hRes = ((IDirectMusicPort8*)m_pUnk)->AssignChannelToBuses(dwChannelGroup,dwChannel,pdwBusses,cBussCount);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::AssignChannelToBuses()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicPort8::AssignChannelToBuses()




//===========================================================================
// CtIDirectMusicPort8::SetSink
//
// Encapsulates calls to SetSink
//
// Parameters:
//  pSink: IDirectSoundSink to attach port to.
//      
//
// Returns:
//  HRESULT - return code from actual call to SetSink
//
// History:
//  02/26/2000 - created
//===========================================================================
HRESULT CtIDirectMusicPort8::SetSink
(
	IN IDirectSoundSink *pSink
)
{
	HRESULT hRes			= E_FAIL;
    BOOL    fValid_pSink	= TRUE;

    // validate pSink
    if(!helpIsValidPtr((void*)pSink, sizeof(IDirectSoundSink*), FALSE))
    {
        // bogus pointer
        fValid_pSink = FALSE;
    }

    // log inputs
	fnsIncrementIndent();
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetSink()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSink == %p   %s",
            pSink,
            fValid_pSink ? "" : "BAD");

    //Call function.
    hRes = ((IDirectMusicPort8*)m_pUnk)->SetSink(pSink);

   // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetSink()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);    

	fnsDecrementIndent();

    return hRes;

} // *** end CtIDirectMusicPort8::SetSink()






//===========================================================================
// CtIDirectMusicPort8::GetSink
//
// Encapsulates calls to GetSink
//
// Parameters:
//  pSink: IDirectSoundSink to attach port to.
//      
//
// Returns:
//  HRESULT - return code from actual call to GetSink
//
// History:
//  02/26/2000 - created
//===========================================================================
HRESULT CtIDirectMusicPort8::GetSink
(
	OUT IDirectSoundSink **ppSink
)
{
HRESULT hRes                      = E_FAIL;
BOOL fValid_ppSink                = TRUE;


    //Validate ptr.
    if(!helpIsValidPtr((void*)ppSink, sizeof(IDirectSoundSink *), FALSE))
    {
        fValid_ppSink = FALSE;
    }

    // log inputs
	fnsIncrementIndent();
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetSink()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppSink == %p %s", ppSink, fValid_ppSink ? "" : "(BAD)");	

    //Call function.
    hRes = ((IDirectMusicPort8*)m_pUnk)->GetSink(ppSink);

   // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetSink()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);    
    if (fValid_ppSink)
        fnsLog(PARAMLOGLEVEL, "*ppSink == (%08Xh)", *ppSink);    

	fnsDecrementIndent();

    return hRes;

} // *** end CtIDirectMusicPort8::GetSink()


