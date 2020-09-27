//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctobject.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctobject.cpp
//
// Test harness implementation of IDirectMusicObject
//
// Functions:
//  CtIDirectMusicObject::CtIDirectMusicObject()
//  CtIDirectMusicObject::~CtIDirectMusicObject()
//  CtIDirectMusicObject::InitTestClass()
//  CtIDirectMusicObject::GetRealObjPtr()
//  CtIDirectMusicObject::QueryInterface()
//  CtIDirectMusicObject::GetDescriptor()
//  CtIDirectMusicObject::SetDescriptor()
//  CtIDirectMusicObject::ParseDescriptor()
//
// History:
//  12/28/1997 - a-llucar - created
//  03/26/1997 - a-kellyc - removed references to dwType in DMUS_OBJECTDESC
//  03/29/1998 - davidkl - brave new world... now a derrived class
//===========================================================================

#include "globals.h"



//===========================================================================
// CtIDirectMusicObject::CtIDirectMusicObject()
//
// Default constructor
//
// History:
//  12/28/1997 - a-llucar - created
//===========================================================================
CtIDirectMusicObject::CtIDirectMusicObject(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicObject";

} //*** end CtIDirectMusicObject::CtIDirectMusicObject()


//===========================================================================
// CtIDirectMusicObject::~CtIDirectMusicObject()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  12/28/1997 - a-llucar - created
//===========================================================================
CtIDirectMusicObject::~CtIDirectMusicObject(void)
{
    // nothing to do

} //*** end CtIDirectMusicObject::~CtIDirectMusicObject()


//===========================================================================
// CtIDirectMusicObject::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmObject - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  12/28/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicObject::InitTestClass(IDirectMusicObject *pdmObject)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmObject, sizeof(IDirectMusicObject), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmObject));

} //*** end CtIDirectMusicObject::InitTestClass()


//===========================================================================
// CtIDirectMusicObject::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicObject **ppdmObject - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  12/28/1997 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicObject::GetRealObjPtr(IDirectMusicObject **ppdmObject)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmObject, sizeof(IDirectMusicObject*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmObject));

} //*** end CtIDirectMusicObject::GetRealObjPtr()



//===========================================================================
// CtIDirectMusicObject::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  12/28/1997 - a-llucar - created
//  1/8/98: bthomas - modified to return properly wrapped object based on riid
//  1/15/98: bthomas - made it more COM-like
//	1/19/98 - bthomas - check to see if we support wrapping the riid before doing it.
//===========================================================================
HRESULT CtIDirectMusicObject::QueryInterface(REFIID riid, 
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

} //*** end CtIDirectMusicObject::QueryInterface


//===========================================================================
// CtIDirectMusicObject::GetDescriptor()
//
// Encapsulates calls to GetDescriptor()
//
// History:
//  12/28/1997 - a-llucar - created
//  03/03/1998 - davidkl - fixed logging bug
//  03/29/1998 - davidkl - fixed ptr validation bug
//	07/21/1998 - t-jwex  - fValid_pDesc now set to false when desc==NULL
//===========================================================================
HRESULT CtIDirectMusicObject::GetDescriptor(LPDMUS_OBJECTDESC pDesc)
{                                           
    HRESULT             hRes                = E_NOTIMPL;
    BOOL                fValid_pDesc	    = TRUE;

	// validate pDesc
    if(!helpIsValidPtr(pDesc, sizeof(DMUS_OBJECTDESC), FALSE))
    {
        fValid_pDesc = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetDescriptor()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pDesc == %p   %s",
            pDesc,
            (fValid_pDesc) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicObject*)m_pUnk)->GetDescriptor(pDesc);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetDescriptor()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pDesc)
    {
		dmthLogDMUS_OBJECTDESCStruct(*pDesc);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicObject::GetDescriptor()



//===========================================================================
// CtIDirectMusicObject::SetDescriptor()
//
// Encapsulates calls to SetDescriptor()
//
// History:
//  12/29/1997 - a-llucar - created
//  03/03/1998 - davidkl - fixed logging bug
//  03/29/1998 - davidkl - fixed ptr validation bug
//===========================================================================
HRESULT CtIDirectMusicObject::SetDescriptor(LPDMUS_OBJECTDESC pDesc)
{                                           
    HRESULT             hRes                = E_NOTIMPL;
    BOOL                fValid_pDesc	    = TRUE;

	// validate pDesc
    if(!helpIsValidPtr(pDesc, sizeof(DMUS_OBJECTDESC), FALSE))
    {
        fValid_pDesc = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetDescriptor()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pDesc == %p   %s",
            pDesc,
            (fValid_pDesc) ? "" : "BAD");
    if(fValid_pDesc)
    {
		dmthLogDMUS_OBJECTDESCStruct(*pDesc);
    }

    // call the real function
    hRes = ((IDirectMusicObject*)m_pUnk)->SetDescriptor(pDesc);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetDescriptor()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicObject::SetDescriptor()



//===========================================================================
// CtIDirectMusicObject::ParseDescriptor()
//
// Encapsulates calls to ParseDescriptor()
//
// History:
//  12/29/1997 - a-llucar - created
//  03/03/1998 - davidkl - fixed logging bug
//  03/29/1998 - davidkl - fixed ptr validation bug
//	07/20/1998 - t-jwex  - fValid_pDesc now set to false when desc==NULL
//===========================================================================
HRESULT CtIDirectMusicObject::ParseDescriptor(LPSTREAM pStream, 
                                              LPDMUS_OBJECTDESC pDesc)
{                                           
    HRESULT             hRes                = E_NOTIMPL;
    BOOL                fValid_pDesc	    = TRUE;
	BOOL				fValid_pStream		= TRUE;

	// validate pDesc
    if(!helpIsValidPtr(pDesc, sizeof(DMUS_OBJECTDESC), FALSE))
    {
        fValid_pDesc = FALSE;
    }

	// validate pStream
    if(!helpIsValidPtr(pStream, sizeof(BYTE), TRUE))
    {
        fValid_pStream = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ParseDescriptor()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStream == %p   %s",
            pStream,
            (fValid_pStream) ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pDesc == %p   %s",
            pDesc,
            (fValid_pDesc) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicObject*)m_pUnk)->ParseDescriptor(pStream, pDesc);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ParseDescriptor()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pDesc)
    {
		dmthLogDMUS_OBJECTDESCStruct(*pDesc);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicObject::ParseDescriptor()

