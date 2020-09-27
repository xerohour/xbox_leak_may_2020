//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctksprop.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctksprop.cpp
//
// Test harness implementation of IKsPropertySet
//
// Functions:
//  CtIKsPropertySet::CtIKsPropertySet()
//  CtIKsPropertySet::~CtIKsPropertySet()
//  CtIKsPropertySet::InitTestClass()
//  CtIKsPropertySet::GetRealObjPtr()
//  CtIKsPropertySet::QueryInterface()
//  CtIKsPropertySet::Set()
//  CtIKsPropertySet::Get()
//  CtIKsPropertySet::QuerySupported()
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIKsPropertySet::CtIKsPropertySet
//
// Default constructor
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================
CtIKsPropertySet::CtIKsPropertySet(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IKsPropertySet";
    
} //*** end CtIKsPropertySet::CtIKsPropertySet()


//===========================================================================
// CtIKsPropertySet::~CtIKsPropertySet
//
// Default destructor
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================
CtIKsPropertySet::~CtIKsPropertySet(void)
{
    // nothing to do
        
} //*** end CtIKsPropertySet::~CtIKsPropertySet()


//===========================================================================
// CtIKsPropertySet::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IKsPropertySet pointer for future use.
//
// Parameters:
//  IKsPropertySet *pPropSet - pointer to real object
//
// Returns: 
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================
HRESULT CtIKsPropertySet::InitTestClass(IKsPropertySet *pPropSet)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pPropSet, sizeof(IKsPropertySet), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pPropSet));

} //*** end CtIKsPropertySet::InitTestClass()


//===========================================================================
// CtIKsPropertySet::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IKsPropertySet **ppPropSet - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================
HRESULT CtIKsPropertySet::GetRealObjPtr(IKsPropertySet **ppPropSet)
{
	// validate ppdm
	if(!helpIsValidPtr(ppPropSet, sizeof(IKsPropertySet*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppPropSet));

} //*** end CtIKsPropertySet::GetRealObjPtr()


//===========================================================================
// CtIKsPropertySet::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================
HRESULT CtIKsPropertySet::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIKsPropertySet::QueryInterface


//===========================================================================
// CtIKsPropertySet::Set
//
// Encapsulates calls to Set
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================
HRESULT CtIKsPropertySet::Set(REFGUID  rguidPropSet,
                                ULONG    ulId,
                                LPVOID   pInstanceData,
                                ULONG    ulInstanceLength,
                                LPVOID   pPropertyData,
                                ULONG    ulDataLength)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pInstanceData    = TRUE;
    BOOL    fValid_pPropertyData    = TRUE;
    char    szGuid[MAX_LOGSTRING];

    // validate pInstanceData
    if(!helpIsValidPtr(pInstanceData, ulInstanceLength, FALSE))
    {
        fValid_pInstanceData = FALSE;
    }

    // validate pPropertyData
    if(!helpIsValidPtr(pPropertyData, ulDataLength, FALSE))
    {
        fValid_pInstanceData = FALSE;
    }

    fnsIncrementIndent();

    tdmGUIDtoString(rguidPropSet, szGuid);

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::Set()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidPropSet     == %s (%s)",
            szGuid, tdmXlatGUID(rguidPropSet));
    fnsLog(PARAMLOGLEVEL, "ulId             == %08Xh",
            ulId);
    fnsLog(PARAMLOGLEVEL, "pInstanceData    == %p   %s",
            pInstanceData,
            fValid_pInstanceData ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ulInstanceLength == %08Xh",
            ulInstanceLength);
    fnsLog(PARAMLOGLEVEL, "pPropertyData    == %p   %s",
            pPropertyData,
            fValid_pPropertyData ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ulDataLength     == %08Xh",
            ulDataLength);
    if(fValid_pInstanceData)
    {
        // BUGBUG log instance data as a string of bytes
        // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    }
    if(fValid_pPropertyData)
    {
        // BUGBUG log property data as a string of bytes
        // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    }

    // call the real function
    hRes = ((IKsPropertySet*)m_pUnk)->Set(rguidPropSet,
                                        ulId,
                                        pInstanceData,
                                        ulInstanceLength,
                                        pPropertyData,
                                        ulDataLength);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::Set()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIKsPropertySet::Set()


//===========================================================================
// CtIKsPropertySet::Get
//
// Encapsulates calls to Get
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================
HRESULT CtIKsPropertySet::Get(REFGUID  rguidPropSet,
                        ULONG    ulId,
                        LPVOID   pInstanceData,
                        ULONG    ulInstanceLength,
                        LPVOID   pPropertyData,
                        ULONG    ulDataLength,
                        ULONG*   pulBytesReturned)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pInstanceData    = TRUE;
    BOOL    fValid_pPropertyData    = TRUE;
    BOOL    fValid_pulBytesReturned = TRUE;
    char    szGuid[MAX_LOGSTRING];

    // validate pInstanceData
    if(!helpIsValidPtr(pInstanceData, ulInstanceLength, FALSE))
    {
        fValid_pInstanceData = FALSE;
    }

    // validate pPropertyData
    if(!helpIsValidPtr(pPropertyData, ulDataLength, FALSE))
    {
        fValid_pInstanceData = FALSE;
    }

    // validate pPropertyData
    if(!helpIsValidPtr(pulBytesReturned, sizeof(ULONG), FALSE))
    {
        fValid_pulBytesReturned = FALSE;
    }

    fnsIncrementIndent();

    tdmGUIDtoString(rguidPropSet, szGuid);

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::Get()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidPropSet     == %s (%s)",
            szGuid, tdmXlatGUID(rguidPropSet));
    fnsLog(PARAMLOGLEVEL, "ulId             == %08Xh",
            ulId);
    fnsLog(PARAMLOGLEVEL, "pInstanceData    == %p   %s",
            pInstanceData,
            fValid_pInstanceData ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ulInstanceLength == %08Xh",
            ulInstanceLength);
    fnsLog(PARAMLOGLEVEL, "pPropertyData    == %p   %s",
            pPropertyData,
            fValid_pPropertyData ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ulDataLength     == %08Xh",
            ulDataLength);
    fnsLog(PARAMLOGLEVEL, "pulBytesReturned == %p   %s",
            pulBytesReturned,
            fValid_pulBytesReturned ? "" : "BAD");
    if(fValid_pInstanceData)
    {
        // BUGBUG log instance data as a string of bytes
        // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    }

    // call the real function
    hRes = ((IKsPropertySet*)m_pUnk)->Get(rguidPropSet,
                                        ulId,
                                        pInstanceData,
                                        ulInstanceLength,
                                        pPropertyData,
                                        ulDataLength,
                                        pulBytesReturned);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::Get()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes              == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pPropertyData)
    {
        // BUGBUG log property data as a string of bytes
        // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    }
    if(fValid_pulBytesReturned)
    {
        fnsLog(PARAMLOGLEVEL, "*pulBytesReturned == %08Xh   %s",
                *pulBytesReturned);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIKsPropertySet::Get()


//===========================================================================
// CtIKsPropertySet::QuerySupported
//
// Encapsulates calls to QuerySupported
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================
HRESULT CtIKsPropertySet::QuerySupported(REFGUID  rguidPropSet,
                                          ULONG    ulId,
                                          ULONG*   pulTypeSupport)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pulTypeSupport   = TRUE;
    char    szGuid[MAX_LOGSTRING];

    // validate pulTypeSupport
    if(!helpIsValidPtr((void*)pulTypeSupport, sizeof(ULONG), FALSE))
    {
        fValid_pulTypeSupport = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::QuerySupported()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidPropSet   == %s (%s)",
            szGuid, tdmXlatGUID(rguidPropSet));
    fnsLog(PARAMLOGLEVEL, "ulId           == %08Xh",
            ulId);
    fnsLog(PARAMLOGLEVEL, "pulTypeSupport == %p   %s",
            pulTypeSupport,
            fValid_pulTypeSupport ? "" : "BAD");

    // call the real function
    hRes = ((IKsPropertySet*)m_pUnk)->QuerySupported(rguidPropSet, 
                                                    ulId,
                                                    pulTypeSupport);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::QuerySupported()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes            == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pulTypeSupport)
    {
        fnsLog(PARAMLOGLEVEL, "*pulTypeSupport == %08Xh",
                *pulTypeSupport);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIKsPropertySet::QuerySupported()


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















