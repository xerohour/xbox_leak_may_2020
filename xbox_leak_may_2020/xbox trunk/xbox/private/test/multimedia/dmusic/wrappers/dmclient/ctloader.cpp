//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctloader.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctloader.cpp
//
// Test harness implementation of IDirectMusicLoader
//
// Functions:
//  CtIDirectMusicLoader::CtIDirectMusicLoader()
//  CtIDirectMusicLoader::~CtIDirectMusicLoader()
//  CtIDirectMusicLoader::InitTestClass()
//  CtIDirectMusicLoader::GetRealObjPtr()
//  CtIDirectMusicLoader::QueryInterface()
//  CtIDirectMusicLoader::GetObject(old parameter set)
//  CtIDirectMusicLoader::GetObject()
//  CtIDirectMusicLoader::SetSearchDirectory()
//  CtIDirectMusicLoader::ScanDirectory()
//  CtIDirectMusicLoader::CacheObject()
//  CtIDirectMusicLoader::ReleaseObject()
//  CtIDirectMusicLoader::ClearCache()
//	CtIDirectMusicLoader::EnableCache()
//	CtIDirectMusicLoader::EnumObject()
//
// History:
//  12/29/1997 - a-llucar - created
//  01/09/1998 - a-llucar - added ScanDirectory, changed EnumObject(s)
//  03/23/1998 - a-kellyc - fixed ReleaseObject and CacheObject functions
//  03/26/1998 - a-kellyc - updated EnableCache and SetSearchDirectory 
//  03/27/1998 - davidkl - Brave new world... now a derrived class
//  04/01/1998 - davidkl - minor house cleaning
//  10/13/1998 - kcraven - added SetObject
//===========================================================================

#include "dmth.h"
#include "dmthp.h"
#include "dmthcom.h"

//===========================================================================
// CtIDirectMusicLoader::CtIDirectMusicLoader()
//
// Default constructor
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
CtIDirectMusicLoader::CtIDirectMusicLoader(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicLoader";

} //*** end CtIDirectMusicLoader::CtIDirectMusicLoader()


//===========================================================================
// CtIDirectMusicLoader::~CtIDirectMusicLoader()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
CtIDirectMusicLoader::~CtIDirectMusicLoader(void)
{
    // nothing to do

} //*** end CtIDirectMusicLoader::~CtIDirectMusicLoader()


//===========================================================================
// CtIDirectMusicLoader::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmLoader - pointer to real object
//
// Returns: 
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicLoader::InitTestClass(IDirectMusicLoader *pdmLoader)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmLoader, sizeof(IDirectMusicLoader), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmLoader));

} //*** end CtIDirectMusicLoader::InitTestClass()


//===========================================================================
// CtIDirectMusicLoader::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicLoader **ppdmLoader - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  12/29/1997 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicLoader::GetRealObjPtr(IDirectMusicLoader **ppdmLoader)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmLoader, sizeof(IDirectMusicLoader*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmLoader));

} //*** end CtIDirectMusicLoader::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicLoader::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicLoader::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicLoader::QueryInterface


//===========================================================================
// CtIDirectMusicLoader::GetObject(old parameter set)
//
// Encapsulates calls to GetObject()
//
// RObjects are real, and WObjects are wrapped.
//
// History:
//  12/29/1997 - a-llucar - created
//  1/8/98: BThomas - modified flow
//  1/15/98: bthomas - made it more COM-like
//  06/24/1998 - davidkl - updated to use the new parameter list internally
//===========================================================================
HRESULT CtIDirectMusicLoader::GetObject(CtIDirectMusicObject **ppWObject, 
	                                    LPDMUS_OBJECTDESC pDesc)
{
	HRESULT	hRes		= E_NOTIMPL;
	HRESULT	hWrapRes	= E_NOTIMPL;
	IDirectMusicObject*		pRObject	= NULL;
	IDirectMusicObject**	ppRObject	= NULL;
	BOOL fValid_ppWObject	= TRUE;
	BOOL fValid_pDesc		= TRUE;

	// validate ppWObject
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicObject pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicLoader::GetObject().  Otherwise, we are to create and
    //  return a CtIDirectObject object
    if(!helpIsValidPtr((void*)ppWObject, sizeof(CtIDirectMusicObject*), FALSE))
    {
        // bogus pointer, use as such
        fValid_ppWObject = FALSE;
        ppRObject = (IDirectMusicObject**)ppWObject;
    }
    else
    {
        // valid pointer, create a real object
        ppRObject = &pRObject;

        // just in case we fail, init test object ptr to NULL
        *ppWObject = NULL;
    }

	// validate pDesc
	if(!helpIsValidPtr(pDesc, sizeof(DMUS_OBJECTDESC), FALSE))
    {
		fValid_pDesc = FALSE;
    }

	fnsIncrementIndent();
	// log inputs
	fnsLog(CALLLOGLEVEL, "--- Calling %s::GetObject()", 
            m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "pDesc    == %p   %s",
			pDesc, fValid_pDesc ? "" : "BAD");
    if(fValid_pDesc)
    {
		dmthLogDMUS_OBJECTDESCStruct(*pDesc);
    }
    fnsLog(PARAMLOGLEVEL, "riid     == IID_IDirectMusicObject");
    fnsLog(PARAMLOGLEVEL, "ppObject == %p   %s",
			ppRObject,
			fValid_ppWObject ? "" : "BAD");

	// Get a real IDirectMusicObject
	hRes = ((IDirectMusicLoader*)m_pUnk)->GetObject(pDesc, 
                                                    IID_IDirectMusicObject, 
                                                    (void**)ppRObject);

	// log results
	fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetObject()",
			m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
			tdmXlatHRESULT(hRes), hRes);
	if(fValid_ppWObject)
	{
    	fnsLog(PARAMLOGLEVEL, "*ppObject == %p",
				pRObject);
	}

    
    if(SUCCEEDED(hRes) && (*ppRObject) && fValid_ppWObject)
    {
        hWrapRes = dmthCreateTestWrappedObject(pRObject, ppWObject);
        if(FAILED(hWrapRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hWrapRes);
			hRes = E_FAIL;
        }
    }

	fnsDecrementIndent();

	// done
    if(fValid_ppWObject && pRObject)
    {
        pRObject->Release();
    }
    return hRes;

} //*** end CtIDirectMusicLoader::GetObject(old parameter set)

//===========================================================================
// CtIDirectMusicLoader::GetObject()
//
// Encapsulates calls to GetObject()  This version of GetObject is quite a
//  bit smarter than previous incarnations -- it uses internal TDMUSIC IIDs
//  to determine when and which wrapped object to return (if we pass in an
//  actual DirectMusic IID, it returns an UNWRAPPED object).
//
// Parameters:
//
// Returns: HRESULT
//
// History:
//  06/24/1998 - davidkl - created
//  11/24/1999 - kcraven - added object wrapping
//  03/17/2000 - kcraven - added more object wrapping
//===========================================================================
HRESULT CtIDirectMusicLoader::GetObject(LPDMUS_OBJECTDESC pDesc,
                                        REFIID riid,
                                        void** ppv)
{
    HRESULT hRes            = E_NOTIMPL;
    HRESULT hWrapRes        = E_NOTIMPL;
	BOOL    fValid_ppv      = TRUE;
	BOOL    fValid_pDesc    = TRUE;
    IID     riidReal;
    char    szIID[MAX_LOGSTRING];
	BOOL	bWrapObject		= TRUE;

	void**						ppdmv			= NULL;			

	IUnknown*					pdmUnknown		= NULL;
	IUnknown**					ppdmUnknown		= NULL;

	IDirectMusicObject*			pdmObject		= NULL;
	IDirectMusicObject**		ppdmObject		= NULL;

	IDirectMusicBand*			pdmBand			= NULL;
	IDirectMusicBand**			ppdmBand		= NULL;

	IDirectMusicCollection*		pdmCollection	= NULL;
	IDirectMusicCollection**	ppdmCollection	= NULL;

	IDirectMusicChordMap*		pdmChordMap		= NULL;
	IDirectMusicChordMap**		ppdmChordMap	= NULL;

	IDirectMusicSegment*		pdmSegment		= NULL;
	IDirectMusicSegment**		ppdmSegment		= NULL;

	IDirectMusicStyle*			pdmStyle		= NULL;
	IDirectMusicStyle**			ppdmStyle		= NULL;

	IDirectMusicSegment8*		pdmSegment8		= NULL;
	IDirectMusicSegment8**		ppdmSegment8	= NULL;

	IDirectMusicStyle8*			pdmStyle8		= NULL;
	IDirectMusicStyle8**		ppdmStyle8		= NULL;

	IDirectMusicScript*			pdmScript		= NULL;
	IDirectMusicScript**		ppdmScript		= NULL;

	IDirectMusicContainer*		pdmContainer	= NULL;
	IDirectMusicContainer**		ppdmContainer	= NULL;

	IDirectMusicSong*			pdmSong			= NULL;
	IDirectMusicSong**			ppdmSong		= NULL;

//	IDirectSoundWave*			pdmWave			= NULL;
//	IDirectSoundWave**			ppdmWave		= NULL;

	IDirectMusicGraph*			pdmGraph		= NULL;
	IDirectMusicGraph**			ppdmGraph		= NULL;


    // validate ppv
    if(!helpIsValidPtr((void*)ppv, sizeof(void*), FALSE))
    {
        // bogus pointer, use as such
        fValid_ppv = FALSE;
    }

	// validate pDesc
	if(!helpIsValidPtr(pDesc, sizeof(DMUS_OBJECTDESC), FALSE))
    {
		fValid_pDesc = FALSE;
    }

    // determine which IID we >really< want to use
    if(IsEqualIID(CTIID_IUnknown, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IUnknown, 
                    sizeof(GUID));
		ppdmUnknown = &pdmUnknown;
		ppdmv = (void**)ppdmUnknown;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicObject, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicObject, 
                    sizeof(GUID));
		ppdmObject = &pdmObject;
		ppdmv = (void**)ppdmObject;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicBand, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicBand, 
                    sizeof(GUID));
		ppdmBand = &pdmBand;
		ppdmv = (void**)ppdmBand;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicCollection, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicCollection, 
                    sizeof(GUID));
		ppdmCollection = &pdmCollection;
		ppdmv = (void**)ppdmCollection;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicChordMap, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicChordMap, 
                    sizeof(GUID));
		ppdmChordMap = &pdmChordMap;
		ppdmv = (void**)ppdmChordMap;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicSegment, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicSegment, 
                    sizeof(GUID));
		ppdmSegment = &pdmSegment;
		ppdmv = (void**)ppdmSegment;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicStyle, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicStyle, 
                    sizeof(GUID));
		ppdmStyle = &pdmStyle;
		ppdmv = (void**)ppdmStyle;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicSegment8, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicSegment8, 
                    sizeof(GUID));
		ppdmSegment8 = &pdmSegment8;
		ppdmv = (void**)ppdmSegment8;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicStyle8, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicStyle8, 
                    sizeof(GUID));
		ppdmStyle8 = &pdmStyle8;
		ppdmv = (void**)ppdmStyle8;
		*ppv = NULL;
    }
	else if(IsEqualIID(CTIID_IDirectMusicScript, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicScript, 
                    sizeof(GUID));
		ppdmScript = &pdmScript;
		ppdmv = (void**)ppdmScript;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicContainer, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicContainer, 
                    sizeof(GUID));
		ppdmContainer = &pdmContainer;
		ppdmv = (void**)ppdmContainer;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicSong, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicSong, 
                    sizeof(GUID));
		ppdmSong = &pdmSong;
		ppdmv = (void**)ppdmSong;
		*ppv = NULL;
    }
/*
//PHOOPHOO
    else if(IsEqualIID(CTIID_IDirectSoundWave, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectSoundWave, 
                    sizeof(GUID));
		ppdmWave = &pdmWave;
		ppdmv = (void**)ppdmWave;
		*ppv = NULL;
    }
*/
    else if(IsEqualIID(CTIID_IDirectMusicGraph, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicGraph, 
                    sizeof(GUID));
		ppdmGraph = &pdmGraph;
		ppdmv = (void**)ppdmGraph;
		*ppv = NULL;
    }
	else
    {
        // trust that the loader is going to recognize this and we don't want it wrapped
		bWrapObject = FALSE;
        CopyMemory((void*)&riidReal, 
                    (void*)&riid, 
                    sizeof(GUID));
		ppdmv = ppv;
    }

    dmthGUIDtoString(riidReal, szIID);

	fnsIncrementIndent();

	// log inputs
	fnsLog(CALLLOGLEVEL, "--- Calling %s::GetObject()", 
            m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "pDesc == %p   %s",
			pDesc, fValid_pDesc ? "" : "BAD");
    if(fValid_pDesc)
    {
		dmthLogDMUS_OBJECTDESCStruct(*pDesc);
    }
    fnsLog(PARAMLOGLEVEL, "riid  == %s (%s)",
            szIID, dmthXlatGUID(riidReal));
    fnsLog(PARAMLOGLEVEL, "ppv   == %p   %s",
			ppdmv,
			fValid_ppv ? "" : "BAD");

	// Get a real IDirectMusicObject
	hRes = ((IDirectMusicLoader*)m_pUnk)->GetObject(pDesc, 
                                                    riidReal, 
                                                    ppdmv);

	// log results
	fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetObject()",
			m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
			tdmXlatHRESULT(hRes), hRes);
	if(fValid_ppv)
	{
    	fnsLog(PARAMLOGLEVEL, "*ppv == %p",
				*ppdmv);

		//we succeeded and we want to wrap the object and we got an object
		if(SUCCEEDED(hRes) && bWrapObject && *ppdmv)
		{
			// determine which IID we >really< want to use
			if(IsEqualIID(CTIID_IDirectMusicObject, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmObject,(CtIDirectMusicObject**)ppv);
				pdmObject->Release();
			}
			else if(IsEqualIID(CTIID_IUnknown, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmUnknown,(CtIUnknown**)ppv);
				pdmUnknown->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicBand, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmBand,(CtIDirectMusicBand**)ppv);
				pdmBand->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicCollection, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmCollection,(CtIDirectMusicCollection**)ppv);
				pdmCollection->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicChordMap, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmChordMap,(CtIDirectMusicChordMap**)ppv);
				pdmChordMap->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicSegment, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmSegment,(CtIDirectMusicSegment**)ppv);
				pdmSegment->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicStyle, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmStyle,(CtIDirectMusicStyle**)ppv);
				pdmStyle->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicSegment8, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmSegment8,(CtIDirectMusicSegment8**)ppv);
				pdmSegment8->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicStyle8, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmStyle8,(CtIDirectMusicStyle8**)ppv);
				pdmStyle8->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicScript, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmScript,(CtIDirectMusicScript**)ppv);
				pdmScript->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicContainer, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmContainer,(CtIDirectMusicContainer**)ppv);
				pdmContainer->Release();
			}
/*
			else if(IsEqualIID(CTIID_IDirectMusicSong, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmSong,(CtIDirectMusicSong**)ppv);
				pdmSong->Release();
			}
*/
/*
//PHOOPHOO
			else if(IsEqualIID(CTIID_IDirectSoundWave, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmWave,(CtIDirectSoundWave**)ppv);
				pdmWave->Release();
			}
*/
			else if(IsEqualIID(CTIID_IDirectMusicGraph, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmGraph,(CtIDirectMusicGraph**)ppv);
				pdmGraph->Release();
			}
			else
			{
				hWrapRes = E_FAIL; //don't support wrapping this interface
			}

			if(FAILED(hWrapRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hWrapRes);
				hRes = E_FAIL; //don't support wrapping this interface
			}
		}

	}

	fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicLoader::GetObject()


//===========================================================================
// CtIDirectMusicLoader::SetObject(LPDMUS_OBJECTDESC pDesc)
//
// Encapsulates calls to SetObject()
//
// History:
//  10/13/1998 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicLoader::SetObject(LPDMUS_OBJECTDESC pDesc)
{
	HRESULT hRes = E_NOTIMPL;
	BOOL fValid_pDesc = TRUE;

	// validate pDesc
	if(!helpIsValidPtr(pDesc, sizeof(DMUS_OBJECTDESC), FALSE))
    {
		fValid_pDesc = FALSE;
    }

	fnsIncrementIndent();
	// log inputs
	fnsLog(CALLLOGLEVEL, "--- Calling %s::SetObject()", 
            m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "pDesc    == %p   %s",
			pDesc, fValid_pDesc ? "" : "BAD");
    if(fValid_pDesc)
    {
		dmthLogDMUS_OBJECTDESCStruct(*pDesc);
    }

	// Get a real IDirectMusicObject
	hRes = ((IDirectMusicLoader*)m_pUnk)->SetObject(pDesc);

	// log results
	fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetObject()",
			m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
			tdmXlatHRESULT(hRes), hRes);

	fnsDecrementIndent();

    return hRes;

} //*** end CtIDirectMusicLoader::SetObject()
 

//===========================================================================
// CtIDirectMusicLoader::SetSearchDirectory()
//
// Encapsulates calls to SetSearchDirectory()
//
// History:
//  12/29/1997 - a-llucar - created
//  03/26/1998 - davidkl - fixed logging and param validation code
//  03/30/1998 - davidkl - added fClear parameter
//===========================================================================
HRESULT CtIDirectMusicLoader::SetSearchDirectory(GUID idClass, 
                                                CHAR *pzPath,
                                                BOOL fClear)
{                                           
    HRESULT hRes                = E_NOTIMPL;
	BOOL	fValid_pwzPath		= TRUE;
    char    szGuid[MAX_LOGSTRING];  

    // validate pszPath
    if(!helpIsValidPtr((void*)pzPath, sizeof(CHAR), FALSE))
    {
        // bogus pointer, use as such
        fValid_pwzPath = FALSE;
    }

    fnsIncrementIndent();

    // make the guid readable
    tdmGUIDtoString(idClass, szGuid);

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetSearchDirectory()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "idClass == %s (%s)",
            szGuid, tdmXlatGUID(idClass));
    fnsLog(PARAMLOGLEVEL, "pwzPath == %p   %s",
            pwzPath,
            fValid_pwzPath ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "fClear  == %s",
            fClear ? "TRUE" : "FALSE");
            
    // call the real function
    hRes = ((IDirectMusicLoader*)m_pUnk)->SetSearchDirectory(idClass, 
                                                            pwzPath,
                                                            fClear);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetSearchDirectory()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicLoader::SetSearchDirectory()


//===========================================================================
// CtIDirectMusicLoader::ScanDirectory()
//
// Encapsulates calls to ScanDirectory()
//
// History:
//  01/09/1998 - a-llucar - created
//  03/26/1998 - davidkl - fixed param validation code
//===========================================================================
HRESULT CtIDirectMusicLoader::ScanDirectory(GUID idClass, 
	                                        CHAR *pzFileExtension, 
	                                        CHAR *pzCacheFileName)
{                                           
    HRESULT hRes						= E_NOTIMPL;
	BOOL	fValid_pwzFileExtension		= TRUE;
	BOOL	fValid_pwzCacheFileName		= TRUE;
    char    szGuid[MAX_LOGSTRING];  

    // validate pwzFileExtention
    if(!helpIsValidReadPtr((void*)pzFileExtension, sizeof(CHAR), FALSE))
    {
        // bogus pointer, use as such
        fValid_pwzFileExtension = FALSE;
    }

    // validate pwzCacheFileName
    // NOTE:
    //  NULL is ok here... be careful later
    if(!helpIsValidReadPtr((void*)pzCacheFileName, sizeof(CHAR), TRUE))
    {
        fValid_pwzCacheFileName = FALSE;
    }

    fnsIncrementIndent();

    // make the guid readable
    tdmGUIDtoString(idClass, szGuid);

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ScanDirectory()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "idClass          == %s (%s)",
            szGuid, tdmXlatGUID(idClass));
    fnsLog(PARAMLOGLEVEL, "pzFileExtension == %p   %s",
			pwzFileExtension, 
            fValid_pwzFileExtension ? "" : "BAD");
    if(fValid_pwzFileExtension && pzFileExtension)
    {
        fnsLog(PARAMLOGLEVEL, "pwzFileExtension == %s",
                pwzFileExtension);
    }
    fnsLog(PARAMLOGLEVEL, "pzCacheFileName == %p   %s",
            pwzCacheFileName, 
            fValid_pwzCacheFileName ? "" : "BAD");
    if(fValid_pwzCacheFileName && pzCacheFileName)
    {
        fnsLog(PARAMLOGLEVEL, "pzCacheFileName == %s",
                pzCacheFileName);
    }
            
    // call the real function
    hRes = ((IDirectMusicLoader*)m_pUnk)->ScanDirectory(idClass, 
		                            pzFileExtension,
		                            pzCacheFileName);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ScanDirectory()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicLoader::ScanDirectory()


//===========================================================================
// CtIDirectMusicLoader::CacheObject()
//
// Encapsulates calls to CacheObject()
//
// History:
//  12/29/1997 - a-llucar - created
//  03/23/1998 - a-kellyc - fixed so it uses wrapped object as parameter
//===========================================================================
HRESULT CtIDirectMusicLoader::CacheObject(CtIDirectMusicObject *pObject)
{                                           
    HRESULT				hRes                = E_NOTIMPL;
	BOOL				fValid_pObject		= TRUE;
    IDirectMusicObject	*pdmObject          = NULL;
	
    // validate pszPath
    if(!helpIsValidPtr((void*)pObject, sizeof(CtIDirectMusicObject), FALSE))
    {
        // bogus pointer, use as such
        fValid_pObject = FALSE;
        // we have a bogus pointer, use it as such
        pdmObject = (IDirectMusicObject*)pObject;
    }
    else
    {
        // we have a good pointer, get the real object
        hRes = pObject->GetRealObjPtr(&pdmObject);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::CacheObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pObject== %p   %s",
            pdmObject, 
            fValid_pObject ? "" : "BAD");
            
    // call the real function
    hRes = ((IDirectMusicLoader*)m_pUnk)->CacheObject(pdmObject);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::CacheObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_pObject && pdmObject)
    {
        pdmObject->Release();
    }
    return hRes;

} //*** end CtIDirectMusicLoader::CacheObject()


//===========================================================================
// CtIDirectMusicLoader::ReleaseObject()
//
// Encapsulates calls to ReleaseObject()
//
// History:
//  12/29/1997 - a-llucar - created
//  03/23/1998 - a-kellyc - fixed so it uses wrapped object as parameter
//===========================================================================
HRESULT CtIDirectMusicLoader::ReleaseObject(CtIDirectMusicObject *pObject)
{                                           
    HRESULT             hRes                = E_NOTIMPL;
	BOOL				fValid_pObject		= TRUE;
    IDirectMusicObject	*pdmObject          = NULL;
	
    // validate pszPath
    if(!helpIsValidPtr((void*)pObject, sizeof(CtIDirectMusicObject), FALSE))
    {
        // bogus pointer, use as such
        fValid_pObject = FALSE;
        // we have a bogus pointer, use it as such
        pdmObject = (IDirectMusicObject*)pObject;
    }
    else
    {
        // we have a good pointer, get the real object
        hRes = pObject->GetRealObjPtr(&pdmObject);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ReleaseObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pObject== %p   %s",
            pObject, 
            fValid_pObject ? "" : "BAD");
            
    // call the real function
    hRes = ((IDirectMusicLoader*)m_pUnk)->ReleaseObject(pdmObject);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ReleaseObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_pObject && pdmObject)
    {
        pdmObject->Release();
    }
    return hRes;

} //*** end CtIDirectMusicLoader::ReleaseObject()


//===========================================================================
// CtIDirectMusicLoader::ClearCache()
//
// Encapsulates calls to ClearCache()
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicLoader::ClearCache(GUID idClass)
{                                           
    HRESULT hRes                = E_NOTIMPL;
    char    szGuid[MAX_LOGSTRING];  

    fnsIncrementIndent();

    // make the guid readable
    tdmGUIDtoString(idClass, szGuid);

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ClearCache()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "idClass == %s (%s)",
            szGuid, tdmXlatGUID(idClass));
            
    // call the real function
    hRes = ((IDirectMusicLoader*)m_pUnk)->ClearCache(idClass);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ClearCache()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicLoader::ClearCache()


//===========================================================================
// CtIDirectMusicLoader::EnableCache()
//
// Encapsulates calls to EnableCache()
//
// History:
//  12/29/1997 - a-llucar - created
//  03/26/1998 - davidkl - tweaked logging
//  03/27/1998 - davidkl - fixed logging bug
//===========================================================================
HRESULT CtIDirectMusicLoader::EnableCache(GUID idClass, BOOL fEnable)
{                                           
    HRESULT hRes    = E_NOTIMPL;
    char    szGuid[MAX_LOGSTRING];  

    fnsIncrementIndent();

    // make the guid readable
    tdmGUIDtoString(idClass, szGuid);

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnableCache()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "idClass == %s (%s)",
            szGuid, tdmXlatGUID(idClass));
	fnsLog(PARAMLOGLEVEL, "fEnable == %s",
            fEnable ? "TRUE" : "FALSE");

    // call the real function
    hRes = ((IDirectMusicLoader*)m_pUnk)->EnableCache(idClass, fEnable);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnableCache()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicLoader::EnableCache()


//===========================================================================
// CtIDirectMusicLoader::EnumObject()
//
// Encapsulates calls to EnumObject()
//
// History:
//  12/29/1997 - a-llucar - created
//  01/09/1998 - a-llucar - changed name from EnumObjects() to EnumObject()
//===========================================================================
HRESULT CtIDirectMusicLoader::EnumObject(GUID idClass, 
	                                    DWORD dwIndex, 
	                                    LPDMUS_OBJECTDESC pDesc)
{                                           
    HRESULT hRes                = E_NOTIMPL;
	BOOL	fValid_pDesc		= TRUE;
    char    szGuid[MAX_LOGSTRING];  

    // validate pszPath
    if(!helpIsValidPtr((void*)pDesc, sizeof(DMUS_OBJECTDESC), FALSE))
    {
        // bogus pointer, use as such
        fValid_pDesc = FALSE;
    }
	
	fnsIncrementIndent();

    // make the guid readable
    tdmGUIDtoString(idClass, szGuid);

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "idClass == %s (%s)",
            szGuid, tdmXlatGUID(idClass));
	fnsLog(PARAMLOGLEVEL, "dwIndex == %lu",
            dwIndex);
	fnsLog(PARAMLOGLEVEL, "pDesc   == %p   %s",
            pDesc, 
            fValid_pDesc ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicLoader*)m_pUnk)->EnumObject(idClass, dwIndex, pDesc);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicLoader::EnumObject()


