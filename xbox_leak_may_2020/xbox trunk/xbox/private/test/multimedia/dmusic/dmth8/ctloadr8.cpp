//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctloadr8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctloadr8.cpp
//
// Test harness implementation of IDirectMusicLoader8
//
// Functions:
//	CtIDirectMusicLoader8::EnableGarbageCollector()
//	CtIDirectMusicLoader8::CollectGarbage()
//	CtIDirectMusicLoader8::ReleaseObjectByUnknown()
//	CtIDirectMusicLoader8::GetDynamicallyReferencedObject()
//	CtIDirectMusicLoader8::ReportDynamicallyReferencedObject()
//	CtIDirectMusicLoader8::LoadObjectFromFile()
//
// History:
//  01/24/2000 - kcraven - created
//  02/08/2000 - kcraven - matched changes to dmusic headers
//  02/18/2000 - kcraven - added LoadObjectFromFile - commented out
//===========================================================================
#include "globals.h"

//===========================================================================
// CtIDirectMusicLoader8::CtIDirectMusicLoader8()
//
// Default constructor
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
CtIDirectMusicLoader8::CtIDirectMusicLoader8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicLoader8";

} // ** end CtIDirectMusicLoader8::CtIDirectMusicLoader8()


//===========================================================================
// CtIDirectMusicLoader8::~CtIDirectMusicLoader8()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
CtIDirectMusicLoader8::~CtIDirectMusicLoader8(void)
{
    // nothing to do

} // ** end CtIDirectMusicLoader8::~CtIDirectMusicLoader8()


//===========================================================================
// CtIDirectMusicLoader8::InitTestClass
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
//  01/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicLoader8::InitTestClass(IDirectMusicLoader8 *pdmLoader8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmLoader8, sizeof(IDirectMusicLoader8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmLoader8));

} // ** end CtIDirectMusicLoader8::InitTestClass()


//===========================================================================
// CtIDirectMusicLoader8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicLoader8 **ppdmLoader - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicLoader8::GetRealObjPtr(IDirectMusicLoader8 **ppdmLoader8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmLoader8, sizeof(IDirectMusicLoader8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmLoader8));

} // ** end CtIDirectMusicLoader8::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicLoader8::GetObject()
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
HRESULT CtIDirectMusicLoader8::GetObject(LPDMUS_OBJECTDESC pDesc,
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

	IDirectMusicObject8*		pdmObject		= NULL;
	IDirectMusicObject8**		ppdmObject		= NULL;

//	IDirectMusicBand*			pdmBand			= NULL;
//	IDirectMusicBand**			ppdmBand		= NULL;

//	IDirectMusicCollection*		pdmCollection	= NULL;
//	IDirectMusicCollection**	ppdmCollection	= NULL;

//	IDirectMusicChordMap*		pdmChordMap		= NULL;
//	IDirectMusicChordMap**		ppdmChordMap	= NULL;

//	IDirectMusicStyle*			pdmStyle		= NULL;
//	IDirectMusicStyle**			ppdmStyle		= NULL;

	IDirectMusicSegment8*		pdmSegment8		= NULL;
	IDirectMusicSegment8**		ppdmSegment8	= NULL;

//	IDirectMusicStyle8*			pdmStyle8		= NULL;
//	IDirectMusicStyle8**		ppdmStyle8		= NULL;

	IDirectMusicScript*			pdmScript		= NULL;
	IDirectMusicScript**		ppdmScript		= NULL;

	//IDirectMusicContainer*		pdmContainer	= NULL;
//	IDirectMusicContainer**		ppdmContainer	= NULL;

//	IDirectMusicSong*			pdmSong			= NULL;
//	IDirectMusicSong**			ppdmSong		= NULL;

//	IDirectSoundWave*			pdmWave			= NULL;
//	IDirectSoundWave**			ppdmWave		= NULL;

//	IDirectMusicGraph*			pdmGraph		= NULL;
//	IDirectMusicGraph**			ppdmGraph		= NULL;


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
/*
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
    else if(IsEqualIID(CTIID_IDirectMusicStyle, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicStyle, 
                    sizeof(GUID));
		ppdmStyle = &pdmStyle;
		ppdmv = (void**)ppdmStyle;
		*ppv = NULL;
    }

*/
    else if(IsEqualIID(CTIID_IDirectMusicSegment8, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicSegment8, 
                    sizeof(GUID));
		ppdmSegment8 = &pdmSegment8;
		ppdmv = (void**)ppdmSegment8;
		*ppv = NULL;
    }

/*
    else if(IsEqualIID(CTIID_IDirectMusicStyle8, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicStyle8, 
                    sizeof(GUID));
		ppdmStyle8 = &pdmStyle8;
		ppdmv = (void**)ppdmStyle8;
		*ppv = NULL;
    }
*/
	else if(IsEqualIID(CTIID_IDirectMusicScript, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicScript, 
                    sizeof(GUID));
		ppdmScript = &pdmScript;
		ppdmv = (void**)ppdmScript;
		*ppv = NULL;
    }

/*
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
*/
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
    else if(IsEqualIID(CTIID_IDirectMusicGraph, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicGraph, 
                    sizeof(GUID));
		ppdmGraph = &pdmGraph;
		ppdmv = (void**)ppdmGraph;
		*ppv = NULL;
    }
*/
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
/*
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
			else if(IsEqualIID(CTIID_IDirectMusicStyle8, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmStyle8,(CtIDirectMusicStyle8**)ppv);
				pdmStyle8->Release();
			}
*/
			else if(IsEqualIID(CTIID_IDirectMusicSegment8, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmSegment8,(CtIDirectMusicSegment8**)ppv);
				pdmSegment8->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicScript, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmScript,(CtIDirectMusicScript**)ppv);
				pdmScript->Release();
			}
/*
			else if(IsEqualIID(CTIID_IDirectMusicContainer, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmContainer,(CtIDirectMusicContainer**)ppv);
				pdmContainer->Release();
			}
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
			else if(IsEqualIID(CTIID_IDirectMusicGraph, riid))
			{
				hWrapRes = dmthCreateTestWrappedObject(pdmGraph,(CtIDirectMusicGraph**)ppv);
				pdmGraph->Release();
			}
*/
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

} //*** end CtIDirectMusicLoader8::GetObject()


//===========================================================================
// CtIDirectMusicLoader8::SetObject(LPDMUS_OBJECTDESC pDesc)
//
// Encapsulates calls to SetObject()
//
// History:
//  10/13/1998 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicLoader8::SetObject(LPDMUS_OBJECTDESC pDesc)
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

} //*** end CtIDirectMusicLoader8::SetObject()
 

//===========================================================================
// CtIDirectMusicLoader8::SetSearchDirectory()
//
// Encapsulates calls to SetSearchDirectory()
//
// History:
//  12/29/1997 - a-llucar - created
//  03/26/1998 - davidkl - fixed logging and param validation code
//  03/30/1998 - davidkl - added fClear parameter
//===========================================================================
HRESULT CtIDirectMusicLoader8::SetSearchDirectory(GUID idClass, 
                                                CHAR *pzPath,
                                                BOOL fClear)
{                                           
    HRESULT hRes                = E_NOTIMPL;
	BOOL	fValid_pzPath		= TRUE;
    char    szGuid[MAX_LOGSTRING];  

    // validate pszPath
    if(!helpIsValidReadPtr((void*)pzPath, sizeof(CHAR), FALSE))
    {
        // bogus pointer, use as such
        fValid_pzPath = FALSE;
    }

    fnsIncrementIndent();

    // make the guid readable
    tdmGUIDtoString(idClass, szGuid);

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetSearchDirectory()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "idClass == %s (%s)",
            szGuid, tdmXlatGUID(idClass));
    fnsLog(PARAMLOGLEVEL, "pzPath == %p   %s",
            pzPath,
            fValid_pzPath ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "fClear  == %s",
            fClear ? "TRUE" : "FALSE");
            
    // call the real function
    hRes = ((IDirectMusicLoader*)m_pUnk)->SetSearchDirectory(idClass, 
                                                            pzPath,
                                                            fClear);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetSearchDirectory()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicLoader8::SetSearchDirectory()

















//===========================================================================
// CtIDirectMusicLoader8::EnableGarbageCollector()
//
// Encapsulates calls to EnableGarbageCollector()
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicLoader8::EnableGarbageCollector(BOOL fEnable)
{                                           
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnableGarbageCollector()",
            m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "fEnable == %s",
            fEnable ? "TRUE" : "FALSE");

    // call the real function
    hRes = ((IDirectMusicLoader8*)m_pUnk)->EnableGarbageCollector(fEnable);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnableGarbageCollector()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // ** end CtIDirectMusicLoader8::EnableGarbageCollector()
*/
	
	
	
//===========================================================================
// CtIDirectMusicLoader8::CollectGarbage()
//
// Encapsulates calls to CollectGarbage()
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
void CtIDirectMusicLoader8::CollectGarbage(void)
{                                           
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::CollectGarbage()",
            m_szInterfaceName);

    // call the real function
    ((IDirectMusicLoader8*)m_pUnk)->CollectGarbage();

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::CollectGarbage()",
            m_szInterfaceName);

    fnsDecrementIndent();

    // done
    return;

} // ** end CtIDirectMusicLoader8::CollectGarbage()




//===========================================================================
// CtIDirectMusicLoader8::ReleaseObjectByUnknown()
//
// Encapsulates calls to ReleaseObjectByUnknown()
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================

HRESULT CtIDirectMusicLoader8::ReleaseObjectByUnknown(CtIUnknown *pObject)
{                                           
    HRESULT		hRes			= E_NOTIMPL;
	BOOL		fValid_pObject	= TRUE;
    IUnknown	*pdmObject		= NULL;
	
    // validate pObject
    if(!helpIsValidPtr((void*)pObject, sizeof(CtIUnknown), FALSE))
    {
        // bogus pointer, use as such
        fValid_pObject = FALSE;
        // we have a bogus pointer, use it as such
        pdmObject = (IUnknown*)pObject;
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ReleaseObjectByUnknown()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pObject== %p   %s",
            pObject, 
            fValid_pObject ? "" : "BAD");
            
    // call the real function
    hRes = ((IDirectMusicLoader8*)m_pUnk)->ReleaseObjectByUnknown(pdmObject);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ReleaseObjectByUnknown()",
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

} // ** end CtIDirectMusicLoader8::ReleaseObjectByUnknown()

/*
//===========================================================================
// CtIDirectMusicLoader8::GetDynamicallyReferencedObject()
//
//
// Parameters:
//
// Returns: HRESULT
//
// History:
//  01/24/2000 - kcraven - created
//  02/08/2000 - kcraven - matched changes to dmusic headers
//  04/06/2000 - danhaff - removed as per dmusic header changes.
//===========================================================================
HRESULT CtIDirectMusicLoader8::GetDynamicallyReferencedObject(
	CtIDirectMusicObject *pSourceObject,
	LPDMUS_OBJECTDESC pDesc,
	REFIID riid,
	LPVOID FAR *ppv)
{
    HRESULT hRes					= E_NOTIMPL;
	IDirectMusicObject*	pdmSourceObject	= NULL;
	BOOL	fValid_pSourceObject	= TRUE;
	BOOL	fValid_pDesc			= TRUE;
    IID		riidReal;
	BOOL	fValid_ppv				= TRUE;
    char    szIID[MAX_LOGSTRING];
	BOOL	bWrapObject				= TRUE;

	void**						ppdmv			= NULL;			
	IDirectMusicObject*			pdmObject		= NULL;
	IDirectMusicObject8*		pdmObject8		= NULL;
	IDirectMusicBand*			pdmBand			= NULL;
	IDirectMusicBand8*			pdmBand8		= NULL;
	IDirectMusicCollection*		pdmCollection	= NULL;
	IDirectMusicChordMap*		pdmChordMap		= NULL;
	IDirectMusicSegment*		pdmSegment		= NULL;
	IDirectMusicStyle*			pdmStyle		= NULL;
	IDirectMusicSegment8*		pdmSegment8		= NULL;
	IDirectMusicStyle8*			pdmStyle8		= NULL;

	IDirectMusicObject**		ppdmObject		= NULL;
	IDirectMusicObject8**		ppdmObject8		= NULL;
	IDirectMusicBand**			ppdmBand		= NULL;
	IDirectMusicBand8**			ppdmBand8		= NULL;
	IDirectMusicCollection**	ppdmCollection	= NULL;
	IDirectMusicChordMap**		ppdmChordMap	= NULL;
	IDirectMusicSegment**		ppdmSegment		= NULL;
	IDirectMusicStyle**			ppdmStyle		= NULL;
	IDirectMusicSegment8**		ppdmSegment8	= NULL;
	IDirectMusicStyle8**		ppdmStyle8		= NULL;

	IDirectMusicScript*			pdmScript		= NULL;
	IDirectMusicScript**		ppdmScript		= NULL;

	    // validate pSourceObject
    if(!helpIsValidPtr((void*)pSourceObject, sizeof(CtIDirectMusicObject), FALSE))
    {
        // bogus pointer, use as such
        fValid_pSourceObject = FALSE;
        // we have a bogus pointer, use it as such
        pdmSourceObject = (IDirectMusicObject*)pSourceObject;
    }
    else
    {
        // we have a good pointer, get the real object
        hRes = pSourceObject->GetRealObjPtr(&pdmSourceObject);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

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
    if(IsEqualIID(CTIID_IDirectMusicObject, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicObject, 
                    sizeof(GUID));
		ppdmObject = &pdmObject;
		ppdmv = (void**)ppdmObject;
		*ppv = NULL;
    }
    else if(IsEqualIID(CTIID_IDirectMusicObject8, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicObject8, 
                    sizeof(GUID));
		ppdmObject8 = &pdmObject8;
		ppdmv = (void**)ppdmObject8;
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
    else if(IsEqualIID(CTIID_IDirectMusicBand8, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicBand8, 
                    sizeof(GUID));
		ppdmBand8 = &pdmBand8;
		ppdmv = (void**)ppdmBand8;
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
	fnsLog(CALLLOGLEVEL, "--- Calling %s::GetDynamicallyReferencedObject()", 
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSourceObject == %p   %s",
			pdmSourceObject,
			fValid_pSourceObject ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "pDesc         == %p   %s",
			pDesc, fValid_pDesc ? "" : "BAD");
    if(fValid_pDesc)
    {
        // BUGBUG log contents of pDesc (STRUCTLOGLEVEL)
    }
    fnsLog(PARAMLOGLEVEL, "riid          == %s (%s)",
            szIID, dmthXlatGUID(riidReal));
    fnsLog(PARAMLOGLEVEL, "ppv           == %p   %s",
			ppdmv,
			fValid_ppv ? "" : "BAD");

	// Get a real IDirectMusicObject
	hRes = ((IDirectMusicLoader8*)m_pUnk)->GetDynamicallyReferencedObject(
												pdmSourceObject,
												pDesc,
												riidReal,
												ppdmv);

	// log results
	fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetDynamicallyReferencedObject()",
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
				hRes = dmthCreateTestWrappedObject(pdmObject,(CtIDirectMusicObject**)ppv);
				pdmObject->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicObject8, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmObject8,(CtIDirectMusicObject8**)ppv);
				pdmObject8->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicBand, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmBand,(CtIDirectMusicBand**)ppv);
				pdmBand->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicBand8, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmBand8,(CtIDirectMusicBand8**)ppv);
				pdmBand8->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicCollection, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmCollection,(CtIDirectMusicCollection**)ppv);
				pdmCollection->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicChordMap, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmChordMap,(CtIDirectMusicChordMap**)ppv);
				pdmChordMap->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicSegment, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmSegment,(CtIDirectMusicSegment**)ppv);
				pdmSegment->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicStyle, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmStyle,(CtIDirectMusicStyle**)ppv);
				pdmStyle->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicSegment8, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmSegment8,(CtIDirectMusicSegment8**)ppv);
				pdmSegment8->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicStyle8, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmStyle8,(CtIDirectMusicStyle8**)ppv);
				pdmStyle8->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicScript, riid))
			{
				hRes = dmthCreateTestWrappedObject(pdmScript,(CtIDirectMusicScript**)ppv);
				pdmScript->Release();
			}
			else
			{
				hRes = E_FAIL; //we want to but don't support wrapping this interface
			}

			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object (%08Xh)", hRes);
			}
		}

	}

	fnsDecrementIndent();

END:
    if(fValid_pSourceObject && pdmSourceObject)
    {
        pdmSourceObject->Release();
    }
    // done
    return hRes;

} // ** end CtIDirectMusicLoader8::GetDynamicallyReferencedObject()




//===========================================================================
// CtIDirectMusicLoader8::ReportDynamicallyReferencedObject()
//
// Encapsulates calls to ReportDynamicallyReferencedObject()
//
// History:
//  01/24/2000 - kcraven - created
//  02/08/2000 - kcraven - matched changes to dmusic headers
//  04/06/2000 - danhaff - removed as per dmusic header changes
//===========================================================================
HRESULT CtIDirectMusicLoader8::ReportDynamicallyReferencedObject
(
	CtIDirectMusicObject *pSourceObject,
	CtIUnknown *pReferencedObject
)
{                                           
    HRESULT				hRes						= E_NOTIMPL;
	BOOL				fValid_pSourceObject		= TRUE;
    IDirectMusicObject	*pdmSourceObject			= NULL;
	BOOL				fValid_pReferencedObject	= TRUE;
    IUnknown			*pdmReferencedObject		= NULL;
	
    // validate pSourceObject
    if(!helpIsValidPtr((void*)pSourceObject, sizeof(CtIDirectMusicObject), FALSE))
    {
        // bogus pointer, use as such
        fValid_pSourceObject = FALSE;
        // we have a bogus pointer, use it as such
        pdmSourceObject = (IDirectMusicObject*)pSourceObject;
    }
    else
    {
        // we have a good pointer, get the real object
        hRes = pSourceObject->GetRealObjPtr(&pdmSourceObject);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }
    // validate pReferencedObject
    if(!helpIsValidPtr((void*)pReferencedObject, sizeof(CtIUnknown), FALSE))
    {
        // bogus pointer, use as such
        fValid_pReferencedObject = FALSE;
        // we have a bogus pointer, use it as such
        pdmReferencedObject = (IUnknown*)pReferencedObject;
    }
    else
    {
        // we have a good pointer, get the real object
        hRes = pReferencedObject->GetRealObjPtr(&pdmReferencedObject);
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
    fnsLog(CALLLOGLEVEL, "--- Calling %s::ReportDynamicallyReferencedObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSourceObject     == %p   %s",
            pdmSourceObject, 
            fValid_pSourceObject ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pReferencedObject == %p   %s",
            pdmReferencedObject, 
            fValid_pReferencedObject ? "" : "BAD");
            
    // call the real function
    hRes = ((IDirectMusicLoader8*)m_pUnk)->ReportDynamicallyReferencedObject(pdmSourceObject,pdmReferencedObject);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::ReportDynamicallyReferencedObject()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_pSourceObject && pdmSourceObject)
    {
        pdmSourceObject->Release();
    }
    if(fValid_pReferencedObject && pdmReferencedObject)
    {
        pdmReferencedObject->Release();
    }
    return hRes;

} // ** end CtIDirectMusicLoader8::ReportDynamicallyReferencedObject()


*/

//===========================================================================
// CtIDirectMusicLoader8::LoadObjectFromFile()
//
// Encapsulates calls to LoadObjectFromFile()
//
// Note: Make sure that if you want a wrapped test class, you pass in the
//       test class's IID, not the real one!!!
// History:
//  02/18/2000 - kcraven - created
//  03/04/2000 - danhaff - took life in hands and uncommented it :)
//===========================================================================
HRESULT CtIDirectMusicLoader8::LoadObjectFromFile(
	REFGUID rguidClassID,
	REFIID riid,			//iidInterfaceID
	const CHAR *pzFilePath,
	void ** ppv)			//ppObject
{
	HRESULT hRes				= E_NOTIMPL;
	HRESULT hResWrap            = E_NOTIMPL;
	BOOL	fValid_ppv			= TRUE;
	BOOL	fValid_pzFilePath	= TRUE;
//	GUID	dwrguidClassID;
	IID		riidReal;
	char	szGUID[MAX_LOGSTRING];
	char	szIID[MAX_LOGSTRING];
	BOOL	bWrapObject			= TRUE;

	void**						ppdmv			= NULL;			

	IUnknown*					pdmUnknown		= NULL;
	IUnknown**					ppdmUnknown		= NULL;

	IDirectMusicObject8*		pdmObject8		= NULL;
	IDirectMusicObject8**		ppdmObject8		= NULL;

//	IDirectMusicBand*			pdmBand			= NULL;
//	IDirectMusicBand**			ppdmBand		= NULL;

//	IDirectMusicCollection*		pdmCollection	= NULL;
//	IDirectMusicCollection**	ppdmCollection	= NULL;

//	IDirectMusicChordMap*		pdmChordMap		= NULL;
//	IDirectMusicChordMap**		ppdmChordMap	= NULL;

//	IDirectMusicStyle*			pdmStyle		= NULL;
//	IDirectMusicStyle**			ppdmStyle		= NULL;

	IDirectMusicSegment8*		pdmSegment8		= NULL;
	IDirectMusicSegment8**		ppdmSegment8	= NULL;

//	IDirectMusicStyle8*			pdmStyle8		= NULL;
//	IDirectMusicStyle8**		ppdmStyle8		= NULL;

//	IDirectSoundWave*           pdmWave         = NULL;
//	IDirectSoundWave**          ppdmWave        = NULL;

//	IDirectMusicContainer*      pdmContainer    = NULL;
//	IDirectMusicContainer**     ppdmContainer   = NULL;

//	IDirectMusicSong*			pdmSong			= NULL;
//	IDirectMusicSong**			ppdmSong		= NULL;

//	IDirectMusicGraph*			pdmGraph		= NULL;
//	IDirectMusicGraph**			ppdmGraph		= NULL;

	IDirectMusicScript*         pdmScript       = NULL;
	IDirectMusicScript**        ppdmScript      = NULL;

    // validate ppv
    if(!helpIsValidPtr((void*)ppv, sizeof(void*), FALSE))
    {
        // bogus pointer, use as such
        fValid_ppv = FALSE;
		ppdmv = ppv;
    }

	// validate pzFilePath
    if(!helpIsValidReadPtr((void*)pzFilePath, sizeof(CHAR), FALSE))
    {
		fValid_pzFilePath = FALSE;
    }

    // determine which IID we >really< want to use
    if(IsEqualIID(CTIID_IUnknown, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IUnknown, 
                    sizeof(GUID));
		ppdmUnknown = &pdmUnknown;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmUnknown;
			*ppv = NULL;
		}
    }
/*
    else if(IsEqualIID(CTIID_IDirectMusicObject, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicObject, 
                    sizeof(GUID));
		ppdmObject = &pdmObject;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmObject;
			*ppv = NULL;
		}
    }
    else if(IsEqualIID(CTIID_IDirectMusicBand, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicBand, 
                    sizeof(GUID));
		ppdmBand = &pdmBand;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmBand;
			*ppv = NULL;
		}
    }
    else if(IsEqualIID(CTIID_IDirectMusicCollection, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicCollection, 
                    sizeof(GUID));
		ppdmCollection = &pdmCollection;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmCollection;
			*ppv = NULL;
		}
    }
    else if(IsEqualIID(CTIID_IDirectMusicChordMap, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicChordMap, 
                    sizeof(GUID));
		ppdmChordMap = &pdmChordMap;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmChordMap;
			*ppv = NULL;
		}
    }
    else if(IsEqualIID(CTIID_IDirectMusicStyle, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicStyle, 
                    sizeof(GUID));
		ppdmStyle = &pdmStyle;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmStyle;
			*ppv = NULL;
		}
    }

*/
    else if(IsEqualIID(CTIID_IDirectMusicSegment8, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicSegment8, 
                    sizeof(GUID));
		ppdmSegment8 = &pdmSegment8;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmSegment8;
			*ppv = NULL;
		}
    }

	else if ( IsEqualIID( CTIID_IDirectMusicScript, riid ) )
	{
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicScript, 
                    sizeof(GUID));
		ppdmScript = &pdmScript;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmScript;
			*ppv = NULL;
		}
    }
/*
    else if(IsEqualIID(CTIID_IDirectMusicStyle8, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicStyle8, 
                    sizeof(GUID));
		ppdmStyle8 = &pdmStyle8;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmStyle8;
			*ppv = NULL;
		}
    }
*/
/*
//PHOOPHOO
    else if(IsEqualIID(CTIID_IDirectSoundWave, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectSoundWave, 
                    sizeof(GUID));
		ppdmWave = &pdmWave;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmWave;
			*ppv = NULL;
		}
    }
*/

/*
    else if(IsEqualIID(CTIID_IDirectMusicContainer, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicContainer, 
                    sizeof(GUID));
		ppdmContainer = &pdmContainer;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmContainer;
			*ppv = NULL;
		}
    }
    else if(IsEqualIID(CTIID_IDirectMusicSong, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicSong, 
                    sizeof(GUID));
		ppdmSong = &pdmSong;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmSong;
			*ppv = NULL;
		}
    }
    else if(IsEqualIID(CTIID_IDirectMusicGraph, riid))
    {
        CopyMemory((void*)&riidReal, 
                    (void*)&IID_IDirectMusicGraph, 
                    sizeof(GUID));
		ppdmGraph = &pdmGraph;
		if(fValid_ppv)
		{
			ppdmv = (void**)ppdmGraph;
			*ppv = NULL;
		}
    }
*/
    else
    {
        // trust that the loader is going to recognize this and we don't want it wrapped
		bWrapObject = FALSE;
        CopyMemory((void*)&riidReal, 
                    (void*)&riid, 
                    sizeof(GUID));
		ppdmv = ppv;
    }

    dmthGUIDtoString(rguidClassID, szGUID);
    dmthGUIDtoString(riidReal, szIID);

	fnsIncrementIndent();

	// log inputs
	fnsLog(CALLLOGLEVEL, "--- Calling %s::LoadObjectFromFile()", 
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidClassID    == %s (%s)",
            szGUID, dmthXlatGUID(rguidClassID));
    fnsLog(PARAMLOGLEVEL, "iidInterfaceID  == %s (%s)",
            szIID, dmthXlatGUID(riidReal));
	fnsLog(PARAMLOGLEVEL, "pzFilePath     == %p   %s",
			pzFilePath, fValid_pzFilePath ? pzFilePath : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppv             == %p   %s",
			ppdmv,
			fValid_ppv ? "" : "BAD");

	// Get a real IDirectMusicObject
	hRes = ((IDirectMusicLoader8*)m_pUnk)->LoadObjectFromFile(
											rguidClassID,
											riidReal,		//iidInterfaceID
											pzFilePath,
											ppdmv);			//ppObject

	// log results
	fnsLog(CALLLOGLEVEL, "--- Returned from %s::LoadObjectFromFile()",
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
/*
			if(IsEqualIID(CTIID_IDirectMusicObject, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmObject,(CtIDirectMusicObject**)ppv);
				pdmObject->Release();
			}
			else 
            */
            if(IsEqualIID(CTIID_IUnknown, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmUnknown,(CtIUnknown**)ppv);
				pdmUnknown->Release();
			}
/*
			else if(IsEqualIID(CTIID_IDirectMusicBand, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmBand,(CtIDirectMusicBand**)ppv);
				pdmBand->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicCollection, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmCollection,(CtIDirectMusicCollection**)ppv);
				pdmCollection->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicChordMap, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmChordMap,(CtIDirectMusicChordMap**)ppv);
				pdmChordMap->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicSegment, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmSegment,(CtIDirectMusicSegment**)ppv);
				pdmSegment->Release();
			}
*/

			else if(IsEqualIID(CTIID_IDirectMusicScript, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmScript,(CtIDirectMusicScript**)ppv);
				pdmScript->Release();
			}
/*
			else if(IsEqualIID(CTIID_IDirectMusicStyle, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmStyle,(CtIDirectMusicStyle**)ppv);
				pdmStyle->Release();
			}
*/
			else if(IsEqualIID(CTIID_IDirectMusicSegment8, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmSegment8,(CtIDirectMusicSegment8**)ppv);
				pdmSegment8->Release();
			}
/*
			else if(IsEqualIID(CTIID_IDirectMusicStyle8, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmStyle8,(CtIDirectMusicStyle8**)ppv);
				pdmStyle8->Release();
			}
*/
/*
			else if(IsEqualIID(CTIID_IDirectSoundWave, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmWave,(CtIDirectSoundWave **)ppv);
				pdmWave->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicContainer, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmContainer,(CtIDirectMusicContainer **)ppv);
				pdmContainer->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicSong, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmSong,(CtIDirectMusicSong **)ppv);
				pdmSong->Release();
			}
			else if(IsEqualIID(CTIID_IDirectMusicGraph, riid))
			{
				hResWrap = dmthCreateTestWrappedObject(pdmGraph,(CtIDirectMusicGraph **)ppv);
				pdmGraph->Release();
			}
*/
			else
			{
				hResWrap = E_FAIL; //don't support wrapping this interface
			}

			if(FAILED(hResWrap))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hResWrap);
                hRes = E_FAIL;
			}
		}
	}

	fnsDecrementIndent();

    // done
    return hRes;

} // ** end CtIDirectMusicLoader8::LoadObjectFromFile()



//===========================================================================
// CtIDirectMusicLoader::ClearCache()
//
// Encapsulates calls to ClearCache()
//
// History:
//  12/29/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicLoader8::ClearCache(GUID idClass)
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


