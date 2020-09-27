//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctunk.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctunk.cpp
//
// Test harness implementation of IUnknown
//
// Functions:
//  CtIUnknown::CtIUnknown()
//  CtIUnknown::~CtIUnknown()
//  CtIUnknown::InitTestClass()
//  CtIUnknown::GetRealObjPtr()
//  CtIUnknown::AddRef()
//  CtIUnknown::Release()
//  CtIUnknown::QueryInterface()
//
// History:
//  10/15/1997 - davidkl - created
//  04/01/1998 - davidkl - a little maintenance
//  04/26/2000 - danhaff - fixed Release bug.
//  07/20/2000 - danhaff - Added DMSTRESS functionality to this.
//===========================================================================

#include "globals.h"
#include "assert.h"

//===========================================================================
// CtIUnknown::CtIUnknown
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  03/16/1998 - davidkl - created
//===========================================================================
CtIUnknown::CtIUnknown(void)
{
HRESULT hr = S_OK;
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IUnknown";
//    m_pDMObjectStressor = NULL;
} //*** end CtIUnknown::CtIUnknown()


//===========================================================================
// CtIUnknown::CtIUnknown
//
// Default destructor
//
// Parameters: none
//
// Returns:
//
// History:
//  03/16/1998 - davidkl - created
//===========================================================================
CtIUnknown::~CtIUnknown(void)
{
/*
    if (m_pDMObjectStressor)
    {
        delete m_pDMObjectStressor;
        m_pDMObjectStressor = NULL;
    }
*/
} //*** end CtIUnknown::~CtIUnknown()


//===========================================================================
// CtIUnknown::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IUnknown object pointer for future use.
//
// Parameters:
//  IUnknown    *pUnk   - pointer to real IUnknown object
//
// Returns:
//
// History:
//  03/16/1998 - davidkl - created
//  08/01/2000 - danhaff - Updated with DMObjectStressor object.
//===========================================================================
HRESULT CtIUnknown::InitTestClass(IUnknown *pUnk)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pUnk, sizeof(IUnknown), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

	// if we get here, we succeeded
	m_pUnk = pUnk;
    m_pUnk->AddRef();

	//TODO: Need to allocate array corresponding to possible stressable interfaces on
	//		App initialization, allow the app to tweak it, then pass it in here to tell
	//		the DMObjectStressor which interfaces to stress.

	//Maybe we should ALWAYS initialize an objectstressor, and let it decide which
	//	interfaces to stress.

    
	//Initialize dmStress with this object, if that's enabled.
/*
    if (g_bDMStressOn && !m_pDMObjectStressor)
    {
        m_pDMObjectStressor = new DMOBJECTSTRESSOR();
        if (m_pDMObjectStressor)
        {
            HRESULT hr = S_OK;
            hr = m_pDMObjectStressor->Initialize(m_pUnk);
            if (FAILED(hr))
            {
                delete m_pDMObjectStressor;
                m_pDMObjectStressor = NULL;
            }

        }

    }
*/
	return  S_OK;

} //*** end CtIUnknown::InitTestClass()


//===========================================================================
// CtIUnknown::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IUnknown **ppUnk - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//	03/16/1998 - davidkl - created
//===========================================================================
HRESULT CtIUnknown::GetRealObjPtr(IUnknown **ppUnk)
{

	// validate ppdm
	if(!helpIsValidPtr(ppUnk, sizeof(IUnknown*), FALSE))
	{
		return E_POINTER;
	}

	// if we get here, return m_pUnk
    *ppUnk = m_pUnk;
    m_pUnk->AddRef();
    return S_OK;

} //*** end CtIUnknown::GetRealObjPtr()


//===========================================================================
// CtIUnknown::AddRef
//
// Increments the reference count for our object
//
// Parameters: none
//
// Returns:
//  DWORD - new ref count
//
// History:
//	03/16/1998 - davidkl - created
//  03/26/1998 - davidkl - fixed potential faulting bug
//===========================================================================
DWORD CtIUnknown::AddRef(void)
{
    DWORD dwRealObjRefCount    = 0;

    // increment our object's refcount
    InterlockedIncrement((long*)&m_dwRefCount);

    fnsIncrementIndent();

    if(m_pUnk)
    {
        // log inputs to AddRef()
        fnsLog(CALLLOGLEVEL, "--- Calling %s::AddRef()", 
                m_szInterfaceName);

        // call the real AddRef()
        dwRealObjRefCount = m_pUnk->AddRef();

        // log results from AddRef()
        fnsLog(CALLLOGLEVEL, "--- Returned from %s::AddRef()", 
                m_szInterfaceName);
        fnsLog(PARAMLOGLEVEL, "dwRefCount == %08Xh", 
                dwRealObjRefCount);
    }

    fnsDecrementIndent();

    // done
//    return m_dwRefCount;

	//LOULOU - MODIFIED FOR TEST PURPOSES
	return dwRealObjRefCount;
    
} //*** end CtIUnknown::AddRef()


//===========================================================================
// CtIUnknown::Release
//
// Decrements the reference count for our object
//
// Parameters: none
//
// Returns:
//  DWORD - new ref count
//
// History:
//	03/16/1998 - davidkl - created
//  03/26/1998 - davidkl - fixed potential faulting bug
//  04/26/2000 - danhaff - fixed bug where we weren't decrementing internal refcount.
//===========================================================================
DWORD CtIUnknown::Release(void)
{
///    DWORD   dwOurRefCount       = m_dwRefCount;
    DWORD   dwRealObjRefCount   = 0;

    assert(m_dwRefCount);

    // decrement our object's refcount
    //InterlockedDecrement((long*)&dwOurRefCount);
    InterlockedDecrement((long*)&m_dwRefCount);

    fnsIncrementIndent();

    if(m_pUnk)
    {
        // log inputs to Release()
        fnsLog(CALLLOGLEVEL, "--- Calling %s::Release()", 
                m_szInterfaceName);

        // call the real Release()
        dwRealObjRefCount = m_pUnk->Release();

        // log results from Release()
        fnsLog(CALLLOGLEVEL, "--- Returned from %s::Release()", 
                m_szInterfaceName);
        fnsLog(PARAMLOGLEVEL, "dwRefCount == %08Xh", 
                dwRealObjRefCount);
    }

    fnsDecrementIndent();
  
    // is this the last release? if so, clean ourselves up
//    if(0 == dwOurRefCount)
    if(0 == m_dwRefCount)
    {
        delete this;
    }

    // done
//    return dwOurRefCount;
	//LOULOU - MODIFIED FOR TEST PURPOSES
	return dwRealObjRefCount;
} //*** end CtIUnknown::Release()


//===========================================================================
// CtIUnknown::QueryInterface
//
// Decrements the reference count for our object
//
// Parameters: none
//
// Returns:
//  DWORD - new ref count
//
// History:
//	03/16/1998 - davidkl - created
//===========================================================================
HRESULT CtIUnknown::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    HRESULT hRes                = E_NOTIMPL;
    BOOL    fValid_ppvObj       = TRUE;
    char    szIID[MAX_LOGSTRING];

    // BUGBUG - need to figure out how to support returning test wrapped objects

	// validate ppvObj
    if(!helpIsValidPtr(ppvObj, sizeof(IUnknown*), FALSE))
    {
        fValid_ppvObj = FALSE;
    }

    // convert the IID into a string
    tdmGUIDtoString((REFGUID)riid, szIID);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::QueryInterface()", 
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "riid   == %s (%s)",
            szIID, tdmXlatGUID((REFGUID)riid));
    fnsLog(PARAMLOGLEVEL, "ppvObj == %p   %s",
            ppvObj,
            fValid_ppvObj ? "" : "BAD");

    // call the real function
    hRes = m_pUnk->QueryInterface(riid, ppvObj);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::QueryInterface()", 
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes    == %s (%08Xh)", 
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppvObj)
    {
        fnsLog(PARAMLOGLEVEL, "*ppvObj == %p",
                *ppvObj);
    }

    fnsDecrementIndent();

    // done
    return hRes;
    
} //*** end CtIUnknown::QueryInterface()


