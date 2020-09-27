//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctcont.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctcont.cpp
//
// Test harness implementation of IDirectMusicContainer
//
// Functions:
//  CtIDirectMusicContainer::CtIDirectMusicContainer()
//  CtIDirectMusicContainer::~CtIDirectMusicContainer()
//  CtIDirectMusicContainer::InitTestClass()
//  CtIDirectMusicContainer::GetRealObjPtr()
//  CtIDirectMusicContainer::QueryInterface()
//  CtIDirectMusicContainer::GetParam()
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicContainer::CtIDirectMusicContainer()
//
// Default constructor
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
CtIDirectMusicContainer::CtIDirectMusicContainer(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicContainer";

} //*** end CtIDirectMusicContainer::CtIDirectMusicContainer()


//===========================================================================
// CtIDirectMusicContainer::~CtIDirectMusicContainer()
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
CtIDirectMusicContainer::~CtIDirectMusicContainer(void)
{
    // nothing to do

} //*** end CtIDirectMusicContainer::~CtIDirectMusicContainer()


//===========================================================================
// CtIDirectMusicContainer::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicContainer *pdmContainer - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicContainer::InitTestClass(IDirectMusicContainer *pdmContainer)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmContainer, sizeof(IDirectMusicContainer), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmContainer));

} //*** end CtIDirectMusicContainer::InitTestClass()


//===========================================================================
// CtIDirectMusicContainer::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	CtIDirectMusicContainer **ppdmContainer - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicContainer::GetRealObjPtr(IDirectMusicContainer **ppdmContainer)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmContainer, sizeof(IDirectMusicContainer*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmContainer));

} //*** end CtIDirectMusicContainer::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicContainer::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  10/22/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicContainer::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicContainer::QueryInterface()


//===========================================================================
// CtIDirectMusicContainer::EnumObject()
//
// Encapsulates calls to EnumObject
//
// History:
//  10/22/1999 - kcraven - created
//  01/21/2000 - kcraven - modified to use new parameters
//===========================================================================
HRESULT CtIDirectMusicContainer::EnumObject
(
	REFGUID rguidClass,
	DWORD dwIndex, 
	LPDMUS_OBJECTDESC pDesc,
	WCHAR *pwszAlias
)
{
    HRESULT hRes					= E_NOTIMPL;
	BOOL	fValid_pDesc			= TRUE;
    BOOL    fValid_pwszAlias		= TRUE;
    char    szGuid[MAX_LOGSTRING];

    dmthGUIDtoString(rguidClass, szGuid);
    
	// validate pDesc
	if(!helpIsValidPtr(pDesc, sizeof(DMUS_OBJECTDESC), FALSE))
    {
		fValid_pDesc = FALSE;
    }
	// validate pwszAlias
	if(!helpIsValidPtr(pwszAlias, sizeof(WCHAR), FALSE))
    {
		fValid_pwszAlias = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumObject()",
			m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidClass == %s (%s)",
            szGuid, dmthXlatGUID(rguidClass));
	fnsLog(PARAMLOGLEVEL, "dwIndex    == %08Xh",
			dwIndex);
	fnsLog(PARAMLOGLEVEL, "pDesc      == %p   %s",
			pDesc, fValid_pDesc ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "pwszAlias  == %p   %s",
			pwszAlias, fValid_pwszAlias ? "" : "BAD");
	
  // call the real function
    hRes = ((IDirectMusicContainer*)m_pUnk)->EnumObject(
				rguidClass,
				dwIndex, 
				pDesc,
				pwszAlias
				);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicContainer::EnumObject()

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






