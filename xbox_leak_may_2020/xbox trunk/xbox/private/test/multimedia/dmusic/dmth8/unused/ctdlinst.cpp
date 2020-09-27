//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctdlinst.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctdlinst.cpp
//
// Test harness implementation of IDirectMusicDownloadedInstrument
//
// Functions:
//  CtIDirectMusicDownloadedInstrument::CtIDirectMusicDownloadedInstrument()
//  CtIDirectMusicDownloadedInstrument::~CtIDirectMusicDownloadedInstrument()
//  CtIDirectMusicDownloadedInstrument::InitTestClass()
//  CtIDirectMusicDownloadedInstrument::GetRealObjPtr()
//  CtIDirectMusicDownloadedInstrument::QueryInterface()
//
// History:
//  10/24/1997 - davidkl - created
//  11/17/1997 - davidkl - changed to downloadedinstrument
//  03/30/1998 - davidkl - brave new world of inheritance
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicDownloadedInstrument::CtIDirectMusicDownloadedInstrument()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
CtIDirectMusicDownloadedInstrument::CtIDirectMusicDownloadedInstrument()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicDownloadedInstrument";

} //*** end CtIDirectMusicDownloadedInstrument::CtIDirectMusicDownloadedInstrument()


//===========================================================================
// CtIDirectMusicDownloadedInstrument::~CtIDirectMusicDownloadedInstrument
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
CtIDirectMusicDownloadedInstrument::~CtIDirectMusicDownloadedInstrument()
{
    // nothing to do

} //*** end CtIDirectMusicDownloadedInstrument::~CtIDirectMusicDownloadedInstrument()



//===========================================================================
// CtIDirectMusicDownloadedInstrument::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicDownloadedInstrument object pointer for future use.
//
// Parameters:
//  CtIDirectMusicDownloadedInstrument *pdmDlInst - pointer to real object
//
// Returns: 
//
// History:
//  10/24/1997 - davidkl - created
//  11/17/1997 - davidkl - changed to DownloadedInstrument class
//===========================================================================
HRESULT CtIDirectMusicDownloadedInstrument::InitTestClass(IDirectMusicDownloadedInstrument *pdmDlInst)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmDlInst, 
                        sizeof(IDirectMusicDownloadedInstrument), 
                        FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmDlInst));

} //*** end CtIDirectMusicDownloadedInstrument::InitTestClass()


//===========================================================================
// CtIDirectMusicDownloadedInstrument::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicDownloadedInstrument **ppdmDlInst - ptr used to return 
//                                                  real object ptr
//
// Returns: HRESULT
//
// History:
//	10/24/1997 - davidkl - created
//  11/17/1997 - davidkl - changed to DownloadedInstrument class
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicDownloadedInstrument::GetRealObjPtr(IDirectMusicDownloadedInstrument **ppdmDlInst)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmDlInst, 
                        sizeof(IDirectMusicDownloadedInstrument*), 
                        FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmDlInst));

} //*** end CtIDirectMusicDownloadedInstrument::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicDownloadedInstrument::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//  HRESULT - return value from actual call to QueryInterface
//
// History:
//  10/24/1997 - davidkl - created
//  11/17/1997 - davidkl - changed to DownloadedInstrument class
//===========================================================================
HRESULT CtIDirectMusicDownloadedInstrument::QueryInterface(REFIID riid, 
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

} //*** end CtIDirectMusicDownloadedInstrument::QueryInterface()


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


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================















