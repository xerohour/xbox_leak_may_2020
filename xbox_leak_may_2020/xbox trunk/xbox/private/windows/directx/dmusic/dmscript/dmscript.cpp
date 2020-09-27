// Copyright (c) 1999 Microsoft Corporation. All rights reserved.
//
// Implementation of CDirectMusicScript.
//

/*#include "stdinc.h"
#include "dll.h"
#include "dmscript.h"
#include "oleaut.h"
#include "globaldisp.h"
#include "activescript.h"
#include "sourcetext.h"
#include "..\shared\xcreate.h"*/

#include "pchscript.h"

//////////////////////////////////////////////////////////////////////
// Creation

CDirectMusicScript::CDirectMusicScript()
  : m_cRef(0),
	m_fZombie(false),
	m_fCriticalSectionInitialized(false),
	m_pPerformance8(NULL),
	m_pLoader8P(NULL),
	m_pDispPerformance(NULL),
	m_pComposer8(NULL),
	m_fUseOleAut(true),
	m_pScriptManager(NULL),
	m_pContainerDispatch(NULL),
	m_pGlobalDispatch(NULL),
	m_fInitError(false), 
    CMemTrack(DMTRACK_SCRIPT)
{
	LockModule(true);
	INITIALIZE_CRITICAL_SECTION(&m_CriticalSection);
	m_fCriticalSectionInitialized = TRUE;

	m_info.fLoaded = false;
	m_vDirectMusicVersion.dwVersionMS = 0;
	m_vDirectMusicVersion.dwVersionLS = 0;
	Zero(&m_iohead);
	ZeroAndSize(&m_InitErrorInfo);
}

void CDirectMusicScript::ReleaseObjects()
{
	if (m_pScriptManager)
	{
		m_pScriptManager->Close();
		SafeRelease(m_pScriptManager);
	}
	SafeRelease(m_pPerformance8);
	SafeRelease(m_pDispPerformance);
	if (m_pLoader8P)
	{
		m_pLoader8P->ReleaseP();
		m_pLoader8P = NULL;
	}
	SafeRelease(m_pComposer8);
	delete m_pContainerDispatch;
	m_pContainerDispatch = NULL;
	delete m_pGlobalDispatch;
	m_pGlobalDispatch = NULL;
}

HRESULT CDirectMusicScript::CreateInstance(
		IUnknown* pUnknownOuter,
		const IID& iid,
		void** ppv)
{
	*ppv = NULL;
	if (pUnknownOuter)
		 return CLASS_E_NOAGGREGATION;

	CDirectMusicScript *pInst = new CDirectMusicScript;
	if (pInst == NULL)
		return E_OUTOFMEMORY;

	return pInst->QueryInterface(iid, ppv);
}

//////////////////////////////////////////////////////////////////////
// IUnknown

STDMETHODIMP 
CDirectMusicScript::QueryInterface(const IID &iid, void **ppv)
{
	V_INAME(CDirectMusicScript::QueryInterface);
	V_PTRPTR_WRITE(ppv);
	V_REFGUID(iid);

	if (iid == IID_IUnknown || iid == IID_IDirectMusicScript)
	{
		*ppv = static_cast<IDirectMusicScript*>(this);
	}
	else if (iid == IID_IDirectMusicScriptPrivate)
	{
		*ppv = static_cast<IDirectMusicScriptPrivate*>(this);
	}
	else if (iid == IID_IDirectMusicObject)
	{
		*ppv = static_cast<IDirectMusicObject*>(this);
	}
	else if (iid == IID_IDirectMusicObjectP)
	{
		*ppv = static_cast<IDirectMusicObjectP*>(this);
	}
	else if (iid == IID_IPersistStream)
	{
		*ppv = static_cast<IPersistStream*>(this);
	}
	else if (iid == IID_IPersist)
	{
		*ppv = static_cast<IPersist*>(this);
	}
	else if (iid == IID_IDispatch)
	{
		*ppv = static_cast<IDispatch*>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	
	reinterpret_cast<IUnknown*>(this)->AddRef();
	
	return S_OK;
}

STDMETHODIMP_(ULONG)
CDirectMusicScript::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG)
CDirectMusicScript::Release()
{
	if (!InterlockedDecrement(&m_cRef)) 
	{
		this->Zombie();
		DELETE_CRITICAL_SECTION(&m_CriticalSection);
		delete this;
		LockModule(false);
		return 0;
	}

	return m_cRef;
}

//////////////////////////////////////////////////////////////////////
// IPersistStream

STDMETHODIMP
CDirectMusicScript::Load(IStream* pStream)
{
	V_INAME(CDirectMusicScript::Load);
	V_INTERFACE(pStream);

	CHECK_ZOMBIE(m_fZombie);

	HRESULT hr = S_OK;

	SmartRef::CritSec CS(&m_CriticalSection);

	// Clear any old info
	this->ReleaseObjects();
	m_info.fLoaded = false;
	m_info.oinfo.Clear();
	m_vDirectMusicVersion.dwVersionMS = 0;
	m_vDirectMusicVersion.dwVersionLS = 0;
	m_wstrLanguage = NULL;
	m_fInitError = false;

	// Get the loader from stream
	IDirectMusicGetLoader *pIDMGetLoader = NULL;
	SmartRef::ComPtr<IDirectMusicLoader> scomLoader;
	hr = pStream->QueryInterface(IID_IDirectMusicGetLoader, reinterpret_cast<void **>(&pIDMGetLoader));
	if (FAILED(hr))
	{
		Trace(1, "Error: unable to load script from a stream because it doesn't support the IDirectMusicGetLoader interface.\n");
		return DMUS_E_UNSUPPORTED_STREAM;
	}

	hr = pIDMGetLoader->GetLoader(&scomLoader);
	pIDMGetLoader->Release();
	if (FAILED(hr))
		return hr;

	hr = scomLoader->QueryInterface(IID_IDirectMusicLoader8P, reinterpret_cast<void **>(&m_pLoader8P)); // OK if this fails -- just means the scripts won't be garbage collected
	if (SUCCEEDED(hr))
	{
		// Hold only a private ref on the loader.  See IDirectMusicLoader8P::AddRefP for more info.
		m_pLoader8P->AddRefP();
		m_pLoader8P->Release(); // offset the QI
	}

	// Read the script's header information

	SmartRef::RiffIter riForm(pStream);
	if (!riForm)
	{
#ifdef DBG
		if (SUCCEEDED(riForm.hr()))
		{
			Trace(1, "Error: Unable to load script: Unexpected end of file.\n");
		}
#endif
		return SUCCEEDED(riForm.hr()) ? DMUS_E_SCRIPT_INVALID_FILE : riForm.hr();
	}
	hr = riForm.FindRequired(SmartRef::RiffIter::Riff, DMUS_FOURCC_SCRIPT_FORM, DMUS_E_SCRIPT_INVALID_FILE);
	if (FAILED(hr))
	{
#ifdef DBG
		if (hr == DMUS_E_SCRIPT_INVALID_FILE)
		{
			Trace(1, "Error: Unable to load script: Form 'DMSC' not found.\n");
		}
#endif
		return hr;
	}

	SmartRef::RiffIter ri = riForm.Descend();
	if (!ri)
		return ri.hr();

	hr = ri.FindRequired(SmartRef::RiffIter::Chunk, DMUS_FOURCC_SCRIPT_CHUNK, DMUS_E_SCRIPT_INVALID_FILE);
	if (FAILED(hr))
	{
#ifdef DBG
		if (hr == DMUS_E_SCRIPT_INVALID_FILE)
		{
			Trace(1, "Error: Unable to load script: Chunk 'schd' not found.\n");
		}
#endif
		return hr;
	}

	hr = SmartRef::RiffIterReadChunk(ri, &m_iohead);
	if (FAILED(hr))
		return hr;

	hr = ri.LoadObjectInfo(&m_info.oinfo, SmartRef::RiffIter::Chunk, DMUS_FOURCC_SCRIPTVERSION_CHUNK);
	if (FAILED(hr))
		return hr;

	hr = SmartRef::RiffIterReadChunk(ri, &m_vDirectMusicVersion);
	if (FAILED(hr))
		return hr;

	// Read the script's embedded container
	IDirectMusicContainer *pContainer = NULL;
	hr = ri.FindAndGetEmbeddedObject(
				SmartRef::RiffIter::Riff,
				DMUS_FOURCC_CONTAINER_FORM,
				DMUS_E_SCRIPT_INVALID_FILE,
				scomLoader,
				CLSID_DirectMusicContainer,
				IID_IDirectMusicContainer,
				reinterpret_cast<void**>(&pContainer));
	if (FAILED(hr))
	{
#ifdef DBG
		if (hr == DMUS_E_SCRIPT_INVALID_FILE)
		{
			Trace(1, "Error: Unable to load script: Form 'DMCN' no found.\n");
		}
#endif
		return hr;
	}

	// Build the container object that will represent the items in the container to the script

	m_pContainerDispatch = new CContainerDispatch(pContainer, scomLoader, m_iohead.dwFlags, &hr);
	pContainer->Release();
	if (!m_pContainerDispatch)
		return E_OUTOFMEMORY;
	if (FAILED(hr))
		return hr;

	// Create the global dispatch object

	m_pGlobalDispatch = new CGlobalDispatch(this);
	if (!m_pGlobalDispatch)
		return E_OUTOFMEMORY;

	// Get the script's language

	hr = ri.FindRequired(SmartRef::RiffIter::Chunk, DMUS_FOURCC_SCRIPTLANGUAGE_CHUNK, DMUS_E_SCRIPT_INVALID_FILE);
	if (FAILED(hr))
	{
#ifdef DBG
		if (hr == DMUS_E_SCRIPT_INVALID_FILE)
		{
			Trace(1, "Error: Unable to load script: Chunk 'scla' no found.\n");
		}
#endif
		return hr;
	}

	hr = ri.ReadText(&m_wstrLanguage);
	if (FAILED(hr))
	{
#ifdef DBG
		if (hr == E_FAIL)
		{
			Trace(1, "Error: Unable to load script: Problem reading 'scla' chunk.\n");
		}
#endif
		return hr == E_FAIL ? DMUS_E_SCRIPT_INVALID_FILE : hr;
	}

	// Get the script's source code

	SmartRef::WString wstrSource;
	for (++ri; ;++ri)
	{
		if (!ri)
		{
			Trace(1, "Error: Unable to load script: Expected chunk 'scsr' or list 'DMRF'.\n");
			return DMUS_E_SCRIPT_INVALID_FILE;
		}

		SmartRef::RiffIter::RiffType type = ri.type();
		FOURCC id = ri.id();

		if (type == SmartRef::RiffIter::Chunk)
		{
			if (id == DMUS_FOURCC_SCRIPTSOURCE_CHUNK)
			{
				hr = ri.ReadText(&wstrSource);
				if (FAILED(hr))
				{
#ifdef DBG
					if (hr == E_FAIL)
					{
						Trace(1, "Error: Unable to load script: Problem reading 'scsr' chunk.\n");
					}
#endif
					return hr == E_FAIL ? DMUS_E_SCRIPT_INVALID_FILE : hr;
				}
			}
			break;
		}
		else if (type == SmartRef::RiffIter::List)
		{
			if (id == DMUS_FOURCC_REF_LIST)
			{
				DMUS_OBJECTDESC desc;
				hr = ri.ReadReference(&desc);
				if (FAILED(hr))
					return hr;
				// The resulting desc shouldn't have a name or GUID (the plain text file can't hold name/GUID info)
				// and it should have a clsid should be GUID_NULL, which we'll replace with the clsid of our private
				// source helper object.
				if (desc.dwValidData & (DMUS_OBJ_NAME | DMUS_OBJ_OBJECT) ||
						!(desc.dwValidData & DMUS_OBJ_CLASS) || desc.guidClass != GUID_NULL)
				{
#ifdef DBG
					if (desc.dwValidData & (DMUS_OBJ_NAME | DMUS_OBJ_OBJECT))
					{
						Trace(1, "Error: Unable to load script: 'DMRF' list must have dwValidData with DMUS_OBJ_CLASS and guidClassID of GUID_NULL.\n");
					}
					else
					{
						Trace(1, "Error: Unable to load script: 'DMRF' list cannot have dwValidData with DMUS_OBJ_NAME or DMUS_OBJ_OBJECT.\n");
					}
#endif
					return DMUS_E_SCRIPT_INVALID_FILE;
				}
				desc.guidClass = CLSID_DirectMusicSourceText;
				IDirectMusicSourceText *pISource = NULL;
				hr = scomLoader->EnableCache(CLSID_DirectMusicSourceText, false); // This is a private object we just use temporarily. Don't want these guys hanging around in the cache.
				if (FAILED(hr))
					return hr;
				hr = scomLoader->GetObject(&desc, IID_IDirectMusicSourceText, reinterpret_cast<void**>(&pISource));
				if (FAILED(hr))
					return hr;
				DWORD cwchSourceBufferSize = 0;
				pISource->GetTextLength(&cwchSourceBufferSize);
				WCHAR *pwszSource = new WCHAR[cwchSourceBufferSize];
				if (!pwszSource)
					return E_OUTOFMEMORY;
				pISource->GetText(pwszSource);
				*&wstrSource = pwszSource;
				pISource->Release();
			}
			break;
		}
	}

	m_info.fLoaded = true;

	// Now that we are loaded and initialized, we can start active scripting

#ifdef XBOX
	bool fCustomScriptEngine = TRUE;
#else // XBOX
	// See if we're dealing with a custom DirectMusic scripting engine.  Such engines are marked with the key DMScript.  They can be
	// called on multiple threads and they don't use oleaut32.  Ordinary active scripting engines are marked with the key OLEScript.
	SmartRef::HKey shkeyLanguage;
	SmartRef::HKey shkeyMark;
	SmartRef::AString astrLanguage = m_wstrLanguage;
	if (ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CLASSES_ROOT, astrLanguage, 0, KEY_QUERY_VALUE, &shkeyLanguage) || !shkeyLanguage)
	{
		Trace(1, "Error: Unable to load script: Scripting engine for language %s does not exist or is not registered.\n", astrLanguage);
		return DMUS_E_SCRIPT_LANGUAGE_INCOMPATIBLE;
	}
	bool fCustomScriptEngine = ERROR_SUCCESS == ::RegOpenKeyEx(shkeyLanguage, "DMScript", 0, KEY_QUERY_VALUE, &shkeyMark) && shkeyMark;
	if (!fCustomScriptEngine)
	{
		if (ERROR_SUCCESS != ::RegOpenKeyEx(shkeyLanguage, "OLEScript", 0, KEY_QUERY_VALUE, &shkeyMark) || !shkeyMark)
		{
			Trace(1, "Error: Unable to load script: Language %s refers to a COM object that is not registered as a scripting engine (OLEScript key).\n", astrLanguage);
			return DMUS_E_SCRIPT_LANGUAGE_INCOMPATIBLE;
		}
	}
#endif // XBOX

	m_fUseOleAut = !fCustomScriptEngine;
	if (fCustomScriptEngine)
	{
		m_pScriptManager = new CActiveScriptManager(
										m_fUseOleAut,
										m_wstrLanguage,
										wstrSource,
										this,
										&hr,
										&m_InitErrorInfo);
	}
#ifndef XBOX
	else
	{
		m_pScriptManager = new CSingleThreadedScriptManager(
										m_fUseOleAut,
										m_wstrLanguage,
										wstrSource,
										this,
										&hr,
										&m_InitErrorInfo);
	}
#endif
	if (!m_pScriptManager)
		return E_OUTOFMEMORY;

	if (FAILED(hr))
	{
		SafeRelease(m_pScriptManager);
	}

	if (hr == DMUS_E_SCRIPT_ERROR_IN_SCRIPT)
	{
		// If we fail here, load would fail and client would never be able to get the
		// error information.  Instead, return S_OK and save the error to return from Init.
		m_fInitError = true;
		hr = S_OK;
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////
// IDirectMusicObject

STDMETHODIMP 
CDirectMusicScript::GetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
	V_INAME(CDirectMusicScript::GetDescriptor);
	V_PTR_WRITE(pDesc, DMUS_OBJECTDESC);
	
	ZeroMemory(pDesc, sizeof(DMUS_OBJECTDESC));
	pDesc->dwSize = sizeof(DMUS_OBJECTDESC);

	CHECK_ZOMBIE(m_fZombie);
	
	if (wcslen(m_info.oinfo.wszName) > 0)
	{
		pDesc->dwValidData |= DMUS_OBJ_NAME;
		wcscpy(pDesc->wszName, m_info.oinfo.wszName);
		pDesc->wszName[DMUS_MAX_NAME-1] = L'\0';
	}

	if (GUID_NULL != m_info.oinfo.guid)
	{
		pDesc->guidObject = m_info.oinfo.guid;
		pDesc->dwValidData |= DMUS_OBJ_OBJECT;
	}

	pDesc->vVersion = m_info.oinfo.vVersion;
	pDesc->dwValidData |= DMUS_OBJ_VERSION;

	pDesc->guidClass = CLSID_DirectMusicScript;
	pDesc->dwValidData |= DMUS_OBJ_CLASS;

	if (m_info.wstrFilename)
	{
		wcscpy(pDesc->wszFileName, m_info.wstrFilename);
		pDesc->dwValidData |= DMUS_OBJ_FILENAME;
	}

	if (m_info.fLoaded)
	{
		pDesc->dwValidData |= DMUS_OBJ_LOADED;
	}

	return S_OK;
}

STDMETHODIMP 
CDirectMusicScript::SetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
	V_INAME(CDirectMusicScript::SetDescriptor);
	V_STRUCTPTR_READ(pDesc, DMUS_OBJECTDESC);

	CHECK_ZOMBIE(m_fZombie);
	
	DWORD dwTemp = pDesc->dwValidData;

	if (pDesc->dwValidData & DMUS_OBJ_OBJECT)
	{
		m_info.oinfo.guid = pDesc->guidObject;
	}

	if (pDesc->dwValidData & DMUS_OBJ_CLASS)
	{
		pDesc->dwValidData &= ~DMUS_OBJ_CLASS;
	}

	if (pDesc->dwValidData & DMUS_OBJ_NAME)
	{
		wcscpy(m_info.oinfo.wszName, pDesc->wszName);
	}

	if (pDesc->dwValidData & DMUS_OBJ_CATEGORY)
	{
		pDesc->dwValidData &= ~DMUS_OBJ_CATEGORY;
	}

	if (pDesc->dwValidData & DMUS_OBJ_FILENAME)
	{
		m_info.wstrFilename = pDesc->wszFileName;
	}

	if (pDesc->dwValidData & DMUS_OBJ_FULLPATH)
	{
		pDesc->dwValidData &= ~DMUS_OBJ_FULLPATH;
	}

	if (pDesc->dwValidData & DMUS_OBJ_URL)
	{
		pDesc->dwValidData &= ~DMUS_OBJ_URL;
	}

	if (pDesc->dwValidData & DMUS_OBJ_VERSION)
	{
		m_info.oinfo.vVersion = pDesc->vVersion;
	}
	
	if (pDesc->dwValidData & DMUS_OBJ_DATE)
	{
		pDesc->dwValidData &= ~DMUS_OBJ_DATE;
	}

	if (pDesc->dwValidData & DMUS_OBJ_LOADED)
	{
		pDesc->dwValidData &= ~DMUS_OBJ_LOADED;
	}
	
	return dwTemp == pDesc->dwValidData ? S_OK : S_FALSE;
}

STDMETHODIMP 
CDirectMusicScript::ParseDescriptor(LPSTREAM pStream, LPDMUS_OBJECTDESC pDesc)
{
	V_INAME(CDirectMusicScript::ParseDescriptor);
	V_INTERFACE(pStream);
	V_PTR_WRITE(pDesc, DMUS_OBJECTDESC);
	
	ZeroMemory(pDesc, sizeof(DMUS_OBJECTDESC));
	pDesc->dwSize = sizeof(DMUS_OBJECTDESC);

	CHECK_ZOMBIE(m_fZombie);
	
	SmartRef::CritSec CS(&m_CriticalSection);

	// Read the script's header information

	SmartRef::RiffIter riForm(pStream);
	if (!riForm)
	{
#ifdef DBG
		if (SUCCEEDED(riForm.hr()))
		{
			Trace(2, "Error: ParseDescriptor on a script failed: Unexpected end of file. "
						"(Note that this may be OK, such as when ScanDirectory is used to parse a set of unknown files, some of which are not scripts.)\n");
		}
#endif
		return SUCCEEDED(riForm.hr()) ? DMUS_E_SCRIPT_INVALID_FILE : riForm.hr();
	}
	HRESULT hr = riForm.FindRequired(SmartRef::RiffIter::Riff, DMUS_FOURCC_SCRIPT_FORM, DMUS_E_SCRIPT_INVALID_FILE);
	if (FAILED(hr))
	{
#ifdef DBG
		if (hr == DMUS_E_SCRIPT_INVALID_FILE)
		{
			Trace(1, "Error: ParseDescriptor on a script failed: Form 'DMSC' not found. "
						"(Note that this may be OK, such as when ScanDirectory is used to parse a set of unknown files, some of which are not scripts.)\n");
		}
#endif
		return hr;
	}

	SmartRef::RiffIter ri = riForm.Descend();
	if (!ri)
		return ri.hr();

	hr = ri.LoadObjectInfo(&m_info.oinfo, SmartRef::RiffIter::Chunk, DMUS_FOURCC_SCRIPTVERSION_CHUNK);
	if (FAILED(hr))
		return hr;

	hr = this->GetDescriptor(pDesc);
	return hr;
}

STDMETHODIMP_(void)
CDirectMusicScript::Zombie()
{
	m_fZombie = true;
	this->ReleaseObjects();
}

//////////////////////////////////////////////////////////////////////
// IDirectMusicScript

STDMETHODIMP
CDirectMusicScript::Init(IDirectMusicPerformance *pPerformance, DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
	V_INAME(CDirectMusicScript::Init);
	V_INTERFACE(pPerformance);
	V_PTR_WRITE_OPT(pErrorInfo, DMUS_SCRIPT_ERRORINFO);

	CHECK_ZOMBIE(m_fZombie);

	SmartRef::ComPtr<IDirectMusicPerformance8> scomPerformance8;
	HRESULT hr = pPerformance->QueryInterface(IID_IDirectMusicPerformance8, reinterpret_cast<void **>(&scomPerformance8));
	if (FAILED(hr))
		return hr;
	
	// Don't take the critical section if the script is already initialized.
	// For example, this is necessary in the following situation:
	//  - The critical section has already been taken by CallRoutine.
	//  - The routine played a segment with a script track referencing this script.
	//  - The script track calls Init (from a different thread) to make sure the script
	//    is initialized.
	if (m_pPerformance8)
	{
		// Additional calls to Init are ignored.
		// First call wins.  Return S_FALSE if performance doesn't match.
		if (m_pPerformance8 == scomPerformance8)
			return S_OK;
		else
			return S_FALSE;
	}

	SmartRef::CritSec CS(&m_CriticalSection);
	if (m_fInitError)
	{
//#ifndef XBOX    // XBOX doesn't care about returning errors.
		if (pErrorInfo)
		{
			// Syntax errors in a script occur as it is loaded, before SetDescriptor gives a script
			// its filename.  We'll have it after the load (before init is called) so can add it
			// back in here.
#ifdef XBOX
			if (m_InitErrorInfo.wszSourceFile[0] == L'\0' && m_info.wstrFilename)
				wcstombs(m_InitErrorInfo.wszSourceFile, m_info.wstrFilename, DMUS_MAX_FILENAME);
#else
			if (m_InitErrorInfo.wszSourceFile[0] == L'\0' && m_info.wstrFilename)
				wcsTruncatedCopy(m_InitErrorInfo.wszSourceFile, m_info.wstrFilename, DMUS_MAX_FILENAME);
#endif
			CopySizedStruct(pErrorInfo, &m_InitErrorInfo);
		}
//#endif
		return DMUS_E_SCRIPT_ERROR_IN_SCRIPT;
	}

	if (!m_info.fLoaded)
	{
		Trace(1, "Error: IDirectMusicScript::Init called before the script has been loaded.\n");
		return DMUS_E_NOT_LOADED;
	}
    ENTER_API_CRITICAL_SECTION;
	// Get the dispatch interface for the performance
	SmartRef::ComPtr<IDispatch> scomDispPerformance = NULL;
	hr = pPerformance->QueryInterface(IID_IDispatch, reinterpret_cast<void **>(&scomDispPerformance));
	if (SUCCEEDED(hr))
    {
	    // Get a composer object
	    hr = DirectMusicCreateInstance(CLSID_DirectMusicComposer, NULL, IID_IDirectMusicComposer8, reinterpret_cast<void **>(&m_pComposer8));
    }
    if (SUCCEEDED(hr))
    {
	    m_pDispPerformance = scomDispPerformance.disown();
	    m_pPerformance8 = scomPerformance8.disown();

    	hr = m_pScriptManager->Start(pErrorInfo);
    }
	if (SUCCEEDED(hr))
    {
    	hr = m_pContainerDispatch->OnScriptInit(m_pPerformance8);
    }
    LEAVE_API_CRITICAL_SECTION;
	return hr;
}

// Returns DMUS_E_SCRIPT_ROUTINE_NOT_FOUND if routine doesn't exist in the script.
STDMETHODIMP
CDirectMusicScript::CallRoutine(
#ifdef XBOX
                                const char *pszRoutineName,
#else
                                WCHAR *pwszRoutineName, 
#endif
                                DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
	V_INAME(CDirectMusicScript::CallRoutine);
	V_PTR_WRITE_OPT(pErrorInfo, DMUS_SCRIPT_ERRORINFO);
	CHECK_ZOMBIE(m_fZombie);
#ifdef XBOX
    V_BUFPTR_READ(pszRoutineName, 1);
    WCHAR *pwszRoutineName = new WCHAR[strlen(pszRoutineName)+1];
    assert(pwszRoutineName);
    mbstowcs( pwszRoutineName, pszRoutineName, MAX_PATH );
#else
	V_BUFPTR_READ(pwszRoutineName, 2);
#endif
    HRESULT hr;
	SmartRef::CritSec CS(&m_CriticalSection);
	if (!m_pScriptManager || !m_pPerformance8)
	{
		Trace(1, "Error: IDirectMusicScript::Init must be called before IDirectMusicScript::CallRoutine.\n");
		hr = DMUS_E_NOT_INIT;
	}
    else
    {
        ENTER_API_CRITICAL_SECTION;
	    hr = m_pScriptManager->CallRoutine(pwszRoutineName, pErrorInfo);
        LEAVE_API_CRITICAL_SECTION;
    }
#ifdef XBOX
    delete [] pwszRoutineName;
#endif
	return hr;
}

// Returns DMUS_E_SCRIPT_VARIABLE_NOT_FOUND if variable doesn't exist in the script.
STDMETHODIMP
CDirectMusicScript::SetVariableVariant(
		WCHAR *pwszVariableName,
		VARIANT varValue,
		BOOL fSetRef,
		DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
	V_INAME(CDirectMusicScript::SetVariableVariant);
	V_BUFPTR_READ(pwszVariableName, 2);
	V_PTR_WRITE_OPT(pErrorInfo, DMUS_SCRIPT_ERRORINFO);

	switch (varValue.vt)
	{
	case VT_BSTR:
		V_BUFPTR_READ_OPT(varValue.bstrVal, sizeof(OLECHAR));
		// We could be more thorough and verify each character until we hit the terminator but
		// that would be inefficient.  We could also use the length preceding a BSTR pointer,
		// but that would be cheating COM's functions that encapsulate BSTRs and could lead to
		// problems in future versions of windows such as 64 bit if the BSTR format changes.
		break;
	case VT_UNKNOWN:
		V_INTERFACE_OPT(varValue.punkVal);
		break;
	case VT_DISPATCH:
		V_INTERFACE_OPT(varValue.pdispVal);
		break;
	}

	CHECK_ZOMBIE(m_fZombie);

	SmartRef::CritSec CS(&m_CriticalSection);

	if (!m_pScriptManager || !m_pPerformance8)
	{
		Trace(1, "Error: IDirectMusicScript::Init must be called before IDirectMusicScript::SetVariableVariant.\n");
		return DMUS_E_NOT_INIT;
	}

	HRESULT hr = m_pScriptManager->SetVariable(pwszVariableName, varValue, !!fSetRef, pErrorInfo);
	if (hr == DMUS_E_SCRIPT_VARIABLE_NOT_FOUND)
	{
		// There are also items in the script's container that the m_pScriptManager object isn't available.
		// If that's the case, we should return a more specific error message.
		IUnknown *punk = NULL;
		hr = m_pContainerDispatch->GetVariableObject(pwszVariableName, &punk);
		if (SUCCEEDED(hr))
		{
			// We don't actually need the object--it can't be set.  Just needed to find out if it's there
			// in order to return a more specific error message.
			punk->Release();
			return DMUS_E_SCRIPT_CONTENT_READONLY;
		}
	}
	return hr;
}

// Returns DMUS_E_SCRIPT_VARIABLE_NOT_FOUND and empty value if variable doesn't exist in the script.
// Certain varient types such as BSTRs and interface pointers must be freed/released according to the standards for VARIANTS.
// If unsure, use VariantClear (requires oleaut32).
STDMETHODIMP
CDirectMusicScript::GetVariableVariant(WCHAR *pwszVariableName, VARIANT *pvarValue, DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
	V_INAME(CDirectMusicScript::GetVariableVariant);
	V_BUFPTR_READ(pwszVariableName, 2);
	V_PTR_WRITE(pvarValue, VARIANT);
	V_PTR_WRITE_OPT(pErrorInfo, DMUS_SCRIPT_ERRORINFO);
	
	DMS_VariantInit(m_fUseOleAut, pvarValue);

	CHECK_ZOMBIE(m_fZombie);

	SmartRef::CritSec CS(&m_CriticalSection);

	if (!m_pScriptManager || !m_pPerformance8)
	{
		Trace(1, "Error: IDirectMusicScript::Init must be called before IDirectMusicScript::GetVariableVariant.\n");
		return DMUS_E_NOT_INIT;
	}

	HRESULT hr = m_pScriptManager->GetVariable(pwszVariableName, pvarValue, pErrorInfo);

	if (hr == DMUS_E_SCRIPT_VARIABLE_NOT_FOUND)
	{
		// There are also items in the script's container that we need to return.
		// This is implemented by the container, which returns the IUnknown pointer directly rather than through a variant.
		IUnknown *punk = NULL;
		hr = m_pContainerDispatch->GetVariableObject(pwszVariableName, &punk);
		if (SUCCEEDED(hr))
		{
			pvarValue->vt = VT_UNKNOWN;
			pvarValue->punkVal = punk;
		}
	}

#ifdef DBG
	if (hr == DMUS_E_SCRIPT_VARIABLE_NOT_FOUND)
	{
		Trace(1, "Error: Attempt to get variable '%S' that is not defined in the script.\n", pwszVariableName);
	}
#endif

#ifndef DMS_NEVER_USE_OLEAUT
	if (!m_fUseOleAut && pvarValue->vt == VT_BSTR)
	{
		// m_fUseOleAut is false when we're using our own custom scripting engine that avoids
		// depending on oleaut32.dll.  But in this case we're returning a BSTR variant to the
		// caller.  We have to allocate this string with SysAllocString (from oleaut32)
		// because the caller is going to free it with SysFreeString--the standard thing to
		// do with a variant BSTR.
		BSTR bstrOle = DMS_SysAllocString(true, pvarValue->bstrVal); // allocate a copy with oleaut
		DMS_SysFreeString(false, pvarValue->bstrVal); // free the previous value (allocated without oleaut)
		pvarValue->bstrVal = bstrOle; // return the oleaut string to the user
		if (!bstrOle)
			hr = E_OUTOFMEMORY;
	}
#endif

	return hr;
}

// Returns DMUS_E_SCRIPT_VARIABLE_NOT_FOUND if variable doesn't exist in the script.
STDMETHODIMP
CDirectMusicScript::SetVariableNumber(
#ifdef XBOX
                                      const char *pszVariableName,
#else
                                      WCHAR *pwszVariableName, 
#endif
                                      LONG lValue, DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
	V_INAME(CDirectMusicScript::SetVariableNumber);
#ifdef XBOX
    V_BUFPTR_READ(pszVariableName, 1);
    WCHAR *pwszVariableName = new WCHAR[strlen(pszVariableName)+1];
    assert(pwszVariableName);
    mbstowcs( pwszVariableName, pszVariableName, MAX_PATH );
#endif
	VARIANT var;
	var.vt = VT_I4;
	var.lVal = lValue;

    ENTER_API_CRITICAL_SECTION;
	HRESULT hr = this->SetVariableVariant(pwszVariableName, var, false, pErrorInfo);
    LEAVE_API_CRITICAL_SECTION;
#ifdef XBOX
    delete [] pwszVariableName;
#endif
	return hr;
}

// Returns DMUS_E_SCRIPT_VARIABLE_NOT_FOUND and 0 if variable doesn't exist in the script.
// Returns DISP_E_TYPEMISMATCH if variable's datatype cannot be converted to LONG.
STDMETHODIMP
CDirectMusicScript::GetVariableNumber(
#ifdef XBOX
                                      const char *pszVariableName,
#else
                                      WCHAR *pwszVariableName, 
#endif
                                      LONG *plValue, DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
	V_INAME(CDirectMusicScript::GetVariableNumber);
	V_PTR_WRITE(plValue, LONG);
	*plValue = 0;
#ifdef XBOX
    V_BUFPTR_READ(pszVariableName, 1);
    WCHAR *pwszVariableName = new WCHAR[strlen(pszVariableName)+1];
    assert(pwszVariableName);
    mbstowcs( pwszVariableName, pszVariableName, MAX_PATH );
#endif
	VARIANT var;
    ENTER_API_CRITICAL_SECTION;
	HRESULT hr = this->GetVariableVariant(pwszVariableName, &var, pErrorInfo);
	if (!(FAILED(hr) && (hr != S_FALSE) && (hr != DMUS_S_GARBAGE_COLLECTED)))
    {
    	hr = DMS_VariantChangeType(m_fUseOleAut, &var, &var, 0, VT_I4);
    	if (SUCCEEDED(hr))
        {
    	    *plValue = var.lVal;
        }
        // GetVariableVariant forces a BSTR to be allocated with SysAllocString;
        // so if we allocated a BSTR there, we need to free it with SysAllocString here.
        bool fUseOleAut = m_fUseOleAut;
#ifndef DMS_NEVER_USE_OLEAUT
        if (!m_fUseOleAut && var.vt == VT_BSTR)
	    {
            fUseOleAut = true;
        }
#endif
	    DMS_VariantClear(fUseOleAut, &var);
    }
    LEAVE_API_CRITICAL_SECTION;
#ifdef XBOX
    delete [] pwszVariableName;
#endif
    return hr;
}

#ifdef XBOX
// Returns DMUS_E_SCRIPT_VARIABLE_NOT_FOUND if variable doesn't exist in the script.
STDMETHODIMP
CDirectMusicScript::SetVariableString(const char *pszVariableName,
                                      const char *pszValue, 
                                      DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
	V_INAME(CDirectMusicScript::SetVariableString);
    V_BUFPTR_READ(pszVariableName, 1);
    V_BUFPTR_READ(pszValue, 1);
    LONG lName = strlen(pszVariableName);
    WCHAR *pwszVariableName = new WCHAR[lName+1];
    assert(pwszVariableName);
    mbstowcs( pwszVariableName, pszVariableName, lName + 1 );

    // Make a wide-char copy of the value
    LONG lValue = strlen(pszValue);
    WCHAR *pwszValue = new WCHAR[lValue+1];
    assert( pwszValue );
    mbstowcs( pwszValue, pszValue, lValue + 1 );

	VARIANT var;
	var.vt = VT_BSTR;
	var.bstrVal = (BSTR)pwszValue;

    ENTER_API_CRITICAL_SECTION;
	HRESULT hr = this->SetVariableVariant(pwszVariableName, var, false, pErrorInfo);
    LEAVE_API_CRITICAL_SECTION;
    delete [] pwszVariableName;
    delete [] pwszValue;
	return hr;
}

// Returns DMUS_E_SCRIPT_VARIABLE_NOT_FOUND and empty if variable doesn't exist in the script.
// Returns DISP_E_TYPEMISMATCH if variable's datatype cannot be converted to BSTR/char *.
STDMETHODIMP
CDirectMusicScript::GetVariableString( const char *pszVariableName,
                                       char *pszValue,
                                       LONG lBufferSize,
                                       LONG *plConverted,
                                       DMUS_SCRIPT_ERRORINFO *pErrorInfo )
{
    V_INAME(CDirectMusicScript::GetVariableString);
    if( pszValue )
        *pszValue = '\0';

    V_BUFPTR_READ(pszVariableName, 1);
    LONG lName = strlen(pszVariableName);
    WCHAR *pwszVariableName = new WCHAR[lName+1];
    assert(pwszVariableName);
    mbstowcs( pwszVariableName, pszVariableName, lName + 1 );

    VARIANT var;
    ENTER_API_CRITICAL_SECTION;
    HRESULT hr = this->GetVariableVariant( pwszVariableName, &var, pErrorInfo );
    if( !(FAILED(hr) && (hr != S_FALSE) && (hr != DMUS_S_GARBAGE_COLLECTED)) )
    {
        hr = DMS_VariantChangeType(m_fUseOleAut, &var, &var, 0, VT_BSTR);
        if( SUCCEEDED(hr))
        {
            LONG lConverted;
            lConverted = wcstombs( pszValue, var.bstrVal, lBufferSize );

            // If it converted exactly lBufferSize chars, then there's
            // no terminator
            if( pszValue && lConverted == lBufferSize )
            {
                pszValue[ lConverted - 1 ] = '\0';
            }
            else
            {
                // Otherwise, either we didn't convert anything, or
                // we did convert and it did add a terminator.
                // Either way, add the terminator in to the count
                assert( !pszValue || pszValue[ lConverted ] == '\0' );
                lConverted += 1;
            }

            // Is caller asking for either # converted or size needed?
            if( plConverted != NULL )
                *plConverted = lConverted;

        }
        // GetVariableVariant forces a BSTR to be allocated with SysAllocString;
        // so if we allocated a BSTR there, we need to free it with SysAllocString here.
        bool fUseOleAut = m_fUseOleAut;
#ifndef DMS_NEVER_USE_OLEAUT
        if (!m_fUseOleAut && var.vt == VT_BSTR)
	    {
            fUseOleAut = true;
        }
#endif
	    DMS_VariantClear(fUseOleAut, &var);
    }
    LEAVE_API_CRITICAL_SECTION;

    delete [] pwszVariableName;
    return hr;
}
#endif // XBOX

// Returns DMUS_E_SCRIPT_VARIABLE_NOT_FOUND if variable doesn't exist in the script.
STDMETHODIMP
CDirectMusicScript::SetVariableObject(
#ifdef XBOX
                                      const char *pszVariableName,
#else
                                      WCHAR *pwszVariableName, 
#endif 
                                      IUnknown *punkValue, DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
	V_INAME(CDirectMusicScript::SetVariableObject);
#ifdef XBOX
    V_BUFPTR_READ(pszVariableName, 1);
    WCHAR *pwszVariableName = new WCHAR[strlen(pszVariableName)+1];
    assert(pwszVariableName);
    mbstowcs( pwszVariableName, pszVariableName, MAX_PATH );
#endif
	VARIANT var;
	var.vt = VT_UNKNOWN;
	var.punkVal = punkValue;
    ENTER_API_CRITICAL_SECTION;
    HRESULT hr = this->SetVariableVariant(pwszVariableName, var, true, pErrorInfo);
    LEAVE_API_CRITICAL_SECTION;
#ifdef XBOX
    delete [] pwszVariableName;
#endif
    return hr;
}

// Returns DMUS_E_SCRIPT_VARIABLE_NOT_FOUND and NULL if variable doesn't exist in the script.
// Returns DISP_E_TYPEMISMATCH if variable's datatype cannot be converted to IUnknown.
STDMETHODIMP
CDirectMusicScript::GetVariableObject(
#ifdef XBOX
                                      const char *pszVariableName,
#else
                                      WCHAR *pwszVariableName, 
#endif
                                      REFIID riid, LPVOID FAR *ppv, DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
	V_INAME(CDirectMusicScript::GetVariableObject);
	V_PTR_WRITE(ppv, IUnknown *);
	*ppv = NULL;
#ifdef XBOX
    V_BUFPTR_READ(pszVariableName, 1);
    WCHAR *pwszVariableName = new WCHAR[strlen(pszVariableName)+1];
    assert(pwszVariableName);
    mbstowcs( pwszVariableName, pszVariableName, MAX_PATH );
#endif
	VARIANT var;
    ENTER_API_CRITICAL_SECTION;
	HRESULT hr = this->GetVariableVariant(pwszVariableName, &var, pErrorInfo);
	if (SUCCEEDED(hr) && (hr != DMUS_S_GARBAGE_COLLECTED))
    {
	    hr = DMS_VariantChangeType(m_fUseOleAut, &var, &var, 0, VT_UNKNOWN);
	    if (SUCCEEDED(hr))
		    hr = var.punkVal->QueryInterface(riid, ppv);
	    DMS_VariantClear(m_fUseOleAut, &var);
    }
	LEAVE_API_CRITICAL_SECTION;
#ifdef XBOX
    delete [] pwszVariableName;
#endif
    return hr;
}

STDMETHODIMP
CDirectMusicScript::EnumRoutine(DWORD dwIndex, WCHAR *pwszName)
{
	V_INAME(CDirectMusicScript::EnumRoutine);
	V_BUFPTR_WRITE(pwszName, MAX_PATH);

	*pwszName = L'\0';

	CHECK_ZOMBIE(m_fZombie);

	if (!m_pScriptManager || !m_pPerformance8)
	{
		Trace(1, "Error: IDirectMusicScript::Init must be called before IDirectMusicScript::EnumRoutine.\n");
		return DMUS_E_NOT_INIT;
	}

	return m_pScriptManager->EnumItem(true, dwIndex, pwszName, NULL);
}

STDMETHODIMP
CDirectMusicScript::EnumVariable(DWORD dwIndex, WCHAR *pwszName)
{
	V_INAME(CDirectMusicScript::EnumRoutine);
	V_BUFPTR_WRITE(pwszName, MAX_PATH);

	*pwszName = L'\0';

	CHECK_ZOMBIE(m_fZombie);

	if (!m_pScriptManager || !m_pPerformance8)
	{
		Trace(1, "Error: IDirectMusicScript::Init must be called before IDirectMusicScript::EnumVariable.\n");
		return DMUS_E_NOT_INIT;
	}

	int cScriptItems = 0;
	HRESULT hr = m_pScriptManager->EnumItem(false, dwIndex, pwszName, &cScriptItems);
	if (FAILED(hr))
		return hr;

	if (hr == S_FALSE)
	{
		// There are also items in the script's container that we need to report.
		assert(dwIndex >= (DWORD) cScriptItems);
		hr = m_pContainerDispatch->EnumItem(dwIndex - cScriptItems, pwszName);
	}

	return hr;
}

STDMETHODIMP
CDirectMusicScript::ScriptTrackCallRoutine(
		WCHAR *pwszRoutineName,
		IDirectMusicSegmentState *pSegSt,
		DWORD dwVirtualTrackID,
		bool fErrorPMsgsEnabled,
		__int64 i64IntendedStartTime,
		DWORD dwIntendedStartTimeFlags)
{
	V_INAME(CDirectMusicScript::CallRoutine);
	V_BUFPTR_READ(pwszRoutineName, 2);
	V_INTERFACE(pSegSt);

	CHECK_ZOMBIE(m_fZombie);

	SmartRef::CritSec CS(&m_CriticalSection);

	if (!m_pScriptManager || !m_pPerformance8)
	{
        Trace(1, "Error: Unitialized Script elements in an attempt to call a Script Routine.\n");
		return DMUS_E_NOT_INIT;
	}

	return m_pScriptManager->ScriptTrackCallRoutine(
								pwszRoutineName,
								pSegSt,
								dwVirtualTrackID,
								fErrorPMsgsEnabled,
								i64IntendedStartTime,
								dwIntendedStartTimeFlags);
}

STDMETHODIMP
CDirectMusicScript::GetTypeInfoCount(UINT *pctinfo)
{
	V_INAME(CDirectMusicScript::GetTypeInfoCount);
	V_PTR_WRITE(pctinfo, UINT);
	*pctinfo = 0;

	CHECK_ZOMBIE(m_fZombie);

	return S_OK;
}

STDMETHODIMP
CDirectMusicScript::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo)
{
	*ppTInfo = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP
CDirectMusicScript::GetIDsOfNames(
		REFIID riid,
		LPOLESTR __RPC_FAR *rgszNames,
		UINT cNames,
		LCID lcid,
		DISPID __RPC_FAR *rgDispId)
{
	if (m_fZombie)
	{
		if (rgDispId)
		{
			for (UINT i = 0; i < cNames; ++i)
			{
				rgDispId[i] = DISPID_UNKNOWN;
			}
		}
    }
    CHECK_ZOMBIE(m_fZombie);
	
	if (!m_pScriptManager || !m_pPerformance8)
	{
		Trace(1, "Error: IDirectMusicScript::Init must be called before GetIDsOfNames.\n");
		return DMUS_E_NOT_INIT;
	}

	return m_pScriptManager->DispGetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
}

STDMETHODIMP
CDirectMusicScript::Invoke(
		DISPID dispIdMember,
		REFIID riid,
		LCID lcid,
		WORD wFlags,
		DISPPARAMS __RPC_FAR *pDispParams,
		VARIANT __RPC_FAR *pVarResult,
		EXCEPINFO __RPC_FAR *pExcepInfo,
		UINT __RPC_FAR *puArgErr)
{
	if (m_fZombie)
	{
		if (pVarResult)
			DMS_VariantInit(m_fUseOleAut, pVarResult);
    }
    CHECK_ZOMBIE(m_fZombie);

	if (!m_pScriptManager || !m_pPerformance8)
	{
		Trace(1, "Error: IDirectMusicScript::Init must be called before Invoke.\n");
		return DMUS_E_NOT_INIT;
	}

	return m_pScriptManager->DispInvoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

//////////////////////////////////////////////////////////////////////
// Methods that allow CActiveScriptManager access to private script interfaces

IDispatch *CDirectMusicScript::GetGlobalDispatch()
{
	assert(m_pGlobalDispatch);
	return m_pGlobalDispatch;
}
