//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctobject8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctobject8.cpp
//
// Test harness implementation of IDirectMusicObject8
//
// Functions:
//  CtIDirectMusicObject8::Zombie();
//
// History:
//  01/28/2000 - kcraven - created
//===========================================================================

#include "globals.h"

//===========================================================================
// CtIDirectMusicObject8::CtIDirectMusicObject8()
//
// Default constructor
//
// History:
//  01/28/2000 - kcraven - created
//===========================================================================
CtIDirectMusicObject8::CtIDirectMusicObject8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicObject8";

//	m_bZombie = FALSE; //special BOOL for testing Zombie

} // ** end CtIDirectMusicObject8::CtIDirectMusicObject8()


//===========================================================================
// CtIDirectMusicObject8::~CtIDirectMusicObject8()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/28/2000 - kcraven - created
//===========================================================================
CtIDirectMusicObject8::~CtIDirectMusicObject8(void)
{
    // nothing to do

} // ** end CtIDirectMusicObject8::~CtIDirectMusicObject8()


//===========================================================================
// CtIDirectMusicObject8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject8 pointer for future use.
//
// Parameters:
//  IDirectMusicObject8 *pdmObject8 - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  01/28/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicObject8::InitTestClass(IDirectMusicObject8 *pdmObject8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmObject8, sizeof(IDirectMusicObject8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmObject8));

} // ** end CtIDirectMusicObject8::InitTestClass()


//===========================================================================
// CtIDirectMusicObject8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicObject8 **ppdmObject8 - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/28/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicObject8::GetRealObjPtr(IDirectMusicObject8 **ppdmObject8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmObject8, sizeof(IDirectMusicObject8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmObject8));

} // ** end CtIDirectMusicObject8::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicObject8::Zombie()
//
// Encapsulates calls to Zombie()
//
// History:
//  01/28/2000 - kcraven - created
//===========================================================================
/*
void CtIDirectMusicObject8::Zombie(void)
{                                           


    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Zombie()",
            m_szInterfaceName);

    // call the real function
    ((IDirectMusicObject8*)m_pUnk)->Zombie();

	m_bZombie = TRUE; //special BOOL for testing Zombie

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Zombie()",
            m_szInterfaceName);

    fnsDecrementIndent();

    // done
    return;

} // ** end CtIDirectMusicObject8::Zombie()
*/


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
HRESULT CtIDirectMusicObject8::GetDescriptor(LPDMUS_OBJECTDESC pDesc)
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
HRESULT CtIDirectMusicObject8::SetDescriptor(LPDMUS_OBJECTDESC pDesc)
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
HRESULT CtIDirectMusicObject8::ParseDescriptor(LPSTREAM pStream, 
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

