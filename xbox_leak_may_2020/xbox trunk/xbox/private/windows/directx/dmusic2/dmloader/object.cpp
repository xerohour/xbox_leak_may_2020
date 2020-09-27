// Copyright (c) 1998-1999 Microsoft Corporation
// Object.cpp : Implementations of CObject and CClass

/*#ifdef XBOX
#include <xtl.h>
#endif // XBOX
#include "dmusicip.h"
#include "loader.h"
#include "debug.h"
#include "miscutil.h"
#ifndef XBOX
#ifdef UNDER_CE
#include "dragon.h"
#else
extern BOOL g_fIsUnicode;
#endif
#endif // !XBOX

#ifdef XBOX
//#include "..\shared\nocom.h"
//#include "..\shared\xsoundp.h" // For IDirectSoundWave
//#include "dmusics.h" // for IID_IDirectMusicSynth8
//#include "..\shared\dmscriptautguids.h"
#endif // XBOX

#ifndef XBOX
const GUID GUID_NULL = { 0 };
#endif
*/
#include "pchloader.h"

#ifdef XBOX
#define USE_ANSI 
#define ONLY_ANSI
#else // !XBOX
#ifdef UNDER_CE
#define USE_UNICODE
#define ONLY_UNICODE
#else // !CE
#define USE_ANSI
#define USE_UNICODE
#define USE_BOTH
#endif // !CE
#endif // !XBOX

CDescriptor::CDescriptor()

{
    m_fCSInitialized = FALSE;

    INITIALIZE_CRITICAL_SECTION(&m_CriticalSection);
    m_fCSInitialized = TRUE;

    m_llMemLength = 0;
    m_pbMemData = NULL;         // Null pointer to memory.
    m_dwValidData = 0;          // Flags indicating which of above is valid.
    m_guidObject = GUID_NULL;           // Unique ID for this object.
    m_guidClass = GUID_NULL;            // GUID for the class of object.
    ZeroMemory( &m_ftDate, sizeof(FILETIME) );              // File date of object.
    ZeroMemory( &m_vVersion, sizeof(DMUS_VERSION) );                // Version, as set by authoring tool.
    m_pwzName = NULL;               // Name of object.  
    m_pwzCategory = NULL;           // Category for object (optional).
    m_pwzFileName = NULL;           // File path.
    m_dwFileSize = 0;           // Size of file.
    m_pIStream = NULL;
    m_liStartPosition.QuadPart = 0;
}

CDescriptor::~CDescriptor()

{
    if (m_fCSInitialized)
    {
        // If critical section never initialized, never got a chance
        // to do any other initializations
        //
        if (m_pwzFileName) delete[] m_pwzFileName;
        if (m_pwzCategory) delete[] m_pwzCategory;
        if (m_pwzName) delete[] m_pwzName;
        if (m_pIStream) m_pIStream->Release();
        DELETE_CRITICAL_SECTION(&m_CriticalSection);
    }
}

void CDescriptor::ClearName()

{
    if (m_pwzName) delete[] m_pwzName;
    m_pwzName = NULL;
    m_dwValidData &= ~DMUS_OBJ_NAME;
}

void CDescriptor::SetName(WCHAR *pwzName)

{
    ClearName();
    m_pwzName = new WCHAR[wcslen(pwzName)+1];
    if (m_pwzName)
    {
        wcscpy( m_pwzName,pwzName ); 
        m_dwValidData |= DMUS_OBJ_NAME;
    }
    else
    {
        m_dwValidData &= ~DMUS_OBJ_NAME;
    }
}

void CDescriptor::ClearCategory()

{
    if (m_pwzCategory) delete[] m_pwzCategory;
    m_pwzCategory = NULL;
    m_dwValidData &= ~DMUS_OBJ_CATEGORY;
}

void CDescriptor::SetCategory(WCHAR *pwzCategory)

{
    ClearCategory();
    m_pwzCategory = new WCHAR[wcslen(pwzCategory)+1];
    if (m_pwzCategory)
    {
        wcscpy( m_pwzCategory,pwzCategory ); 
        m_dwValidData |= DMUS_OBJ_CATEGORY;
    }
    else
    {
        m_dwValidData &= ~DMUS_OBJ_CATEGORY;
    }
}

void CDescriptor::ClearFileName()

{
    if (m_pwzFileName) delete[] m_pwzFileName;
    m_pwzFileName = NULL;
    m_dwValidData &= ~DMUS_OBJ_FILENAME;
}

// return S_FALSE if the filename is already set to this
HRESULT CDescriptor::SetFileName(WCHAR *pwzFileName)

{
    if( m_pwzFileName )
    {
        if( !_wcsicmp( m_pwzFileName, pwzFileName ))
        {
            return S_FALSE;
        }
    }
    ClearFileName();
    m_pwzFileName = new WCHAR[wcslen(pwzFileName)+1];
    if (m_pwzFileName)
    {
        wcscpy( m_pwzFileName,pwzFileName ); 
        m_dwValidData |= DMUS_OBJ_FILENAME;
    }
    else
    {
        m_dwValidData &= ~DMUS_OBJ_FILENAME;
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

BOOL CDescriptor::IsEmpty()

{
    return !(m_dwValidData & (DMUS_OBJ_STREAM | DMUS_OBJ_FILENAME | DMUS_OBJ_NAME | DMUS_OBJ_MEMORY));
}

void CDescriptor::ClearIStream()

{
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    if (m_pIStream)
    {
        m_pIStream->Release();
    }
    m_pIStream      = NULL;
    m_liStartPosition.QuadPart = 0;
    m_dwValidData  &= ~DMUS_OBJ_STREAM;
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
}

void CDescriptor::SetIStream(IStream *pIStream)

{
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    ClearIStream();

    m_pIStream = pIStream;

    if (m_pIStream)
    {
        ULARGE_INTEGER  libNewPosition;
        m_liStartPosition.QuadPart = 0;
        m_pIStream->Seek( m_liStartPosition, STREAM_SEEK_CUR, &libNewPosition );
        m_liStartPosition.QuadPart = libNewPosition.QuadPart;
        m_pIStream->AddRef();
        m_dwValidData |= DMUS_OBJ_STREAM;
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
}

BOOL CDescriptor::IsExtension(WCHAR *pwzExtension)

{
    if (pwzExtension && m_pwzFileName)
    {
        DWORD dwX;
        DWORD dwLen = wcslen(m_pwzFileName);
        for (dwX = 0; dwX < dwLen; dwX++)
        {
            if (m_pwzFileName[dwX] == '.') break;
        }
        dwX++;
        if (dwX < dwLen)
        {
            return !_wcsicmp(pwzExtension,&m_pwzFileName[dwX]);
        }
    }
    return FALSE;
}

void CDescriptor::Get(LPDMUS_OBJECTDESC pDesc)

{
    // Don't return the IStream insterface. Once set, this becomes private to the loader.
    pDesc->dwValidData = m_dwValidData & ~DMUS_OBJ_STREAM;

    pDesc->guidObject = m_guidObject;
    pDesc->guidClass = m_guidClass;
    pDesc->ftDate = m_ftDate;
    pDesc->vVersion = m_vVersion;
    pDesc->llMemLength = m_llMemLength;
    pDesc->pbMemData = m_pbMemData;
    if (m_pwzName) 
    {
        wcsncpy( pDesc->wszName,m_pwzName, DMUS_MAX_NAME ); 
    }
    if (m_pwzCategory)
    {
        wcsncpy( pDesc->wszCategory,m_pwzCategory, DMUS_MAX_CATEGORY ); 
    }
    if (m_pwzFileName)
    {
        wcsncpy( pDesc->wszFileName, m_pwzFileName, DMUS_MAX_FILENAME);
    }
}

void CDescriptor::Set(LPDMUS_OBJECTDESC pDesc)

{
    m_dwValidData = pDesc->dwValidData;
    m_guidObject = pDesc->guidObject;
    m_guidClass = pDesc->guidClass;
    m_ftDate = pDesc->ftDate;
    m_vVersion = pDesc->vVersion;
    m_llMemLength = pDesc->llMemLength;
    m_pbMemData = pDesc->pbMemData;
    ClearName();
    if (pDesc->dwValidData & DMUS_OBJ_NAME)
    {
        pDesc->wszName[DMUS_MAX_NAME - 1] = 0;  // Force string length, in case of error.
        SetName(pDesc->wszName);
    }
    ClearCategory();
    if (pDesc->dwValidData & DMUS_OBJ_CATEGORY)
    {
        pDesc->wszCategory[DMUS_MAX_CATEGORY - 1] = 0;  // Force string length, in case of error.
        SetCategory(pDesc->wszCategory);
    }
    ClearFileName();
    if (pDesc->dwValidData & DMUS_OBJ_FILENAME)
    {
        pDesc->wszFileName[DMUS_MAX_FILENAME - 1] = 0;  // Force string length, in case of error.
        SetFileName(pDesc->wszFileName);
    }
    ClearIStream();
    if (pDesc->dwValidData & DMUS_OBJ_STREAM)
    {
        SetIStream(pDesc->pStream);
    }
}

void CDescriptor::Copy(CDescriptor *pDesc)

{
    m_dwValidData = pDesc->m_dwValidData;
    m_guidObject = pDesc->m_guidObject;
    m_guidClass = pDesc->m_guidClass;
    m_ftDate = pDesc->m_ftDate;
    m_vVersion = pDesc->m_vVersion;
    m_llMemLength = pDesc->m_llMemLength;
    m_pbMemData = pDesc->m_pbMemData;
    ClearName();
    if (pDesc->m_dwValidData & DMUS_OBJ_NAME)
    {
        SetName(pDesc->m_pwzName);
    }
    ClearCategory();
    if (pDesc->m_dwValidData & DMUS_OBJ_CATEGORY)
    {
        SetCategory(pDesc->m_pwzCategory);
    }
    ClearFileName();
    if (pDesc->m_dwValidData & DMUS_OBJ_FILENAME)
    {
        SetFileName(pDesc->m_pwzFileName);
    }
    ClearIStream();
    if (pDesc->m_dwValidData & DMUS_OBJ_STREAM)
    {
        SetIStream(pDesc->m_pIStream);
    }
}

void CDescriptor::Merge(CDescriptor *pSource)

{
    if (pSource->m_dwValidData & DMUS_OBJ_OBJECT)
    {
        m_dwValidData |= DMUS_OBJ_OBJECT;
        m_guidObject = pSource->m_guidObject;
    }
    if (pSource->m_dwValidData & DMUS_OBJ_CLASS)
    {
        m_dwValidData |= DMUS_OBJ_CLASS;
        m_guidClass = pSource->m_guidClass;
    }
    if (pSource->m_dwValidData & DMUS_OBJ_NAME)
    {
        m_dwValidData |= DMUS_OBJ_NAME;
        SetName(pSource->m_pwzName);
    }
    if (pSource->m_dwValidData & DMUS_OBJ_CATEGORY)
    {
        m_dwValidData |= DMUS_OBJ_CATEGORY;
        SetCategory(pSource->m_pwzCategory);
    }
    if (pSource->m_dwValidData & DMUS_OBJ_VERSION)
    {
        m_dwValidData |= DMUS_OBJ_VERSION;
        m_vVersion = pSource->m_vVersion;
    }
    if (pSource->m_dwValidData & DMUS_OBJ_DATE)
    {
        m_dwValidData |= DMUS_OBJ_DATE;
        m_ftDate = pSource->m_ftDate; 
    }
    if (pSource->m_dwValidData & DMUS_OBJ_FILENAME)
    {
        if (!(m_dwValidData & DMUS_OBJ_FILENAME))
        {
            if (SUCCEEDED(SetFileName(pSource->m_pwzFileName)))
            {
                m_dwValidData |= (pSource->m_dwValidData & 
                    (DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH | DMUS_OBJ_URL));
            }
        }
    }
    if (pSource->m_dwValidData & DMUS_OBJ_MEMORY)
    {
        m_pbMemData = pSource->m_pbMemData;
        m_llMemLength = pSource->m_llMemLength;
        if (m_llMemLength && m_pbMemData)
        {
            m_dwValidData |= DMUS_OBJ_MEMORY;
        }
        else
        {
            m_dwValidData &= ~DMUS_OBJ_MEMORY;
        }
    }
    if (pSource->m_dwValidData & DMUS_OBJ_STREAM)
    {
        SetIStream(pSource->m_pIStream);
    }
}

CObject::CObject(CClass *pClass)

{
    m_dwScanBits = 0;
    m_pClass = pClass;
    m_pIDMObject = NULL;
    m_pvecReferences = NULL;
}

CObject::CObject(CClass *pClass, CDescriptor *pDesc)

{
    m_dwScanBits = 0;
    m_pClass = pClass;
    m_pIDMObject = NULL;
    m_ObjectDesc.Copy(pDesc);
    m_ObjectDesc.m_dwValidData &= ~DMUS_OBJ_LOADED;
    if (!(m_ObjectDesc.m_dwValidData & DMUS_OBJ_CLASS))
    {
        m_ObjectDesc.m_guidClass = pClass->m_ClassDesc.m_guidClass;
        m_ObjectDesc.m_dwValidData |= 
            (pClass->m_ClassDesc.m_dwValidData & DMUS_OBJ_CLASS);
    }
    m_pvecReferences = NULL;
}


CObject::~CObject()

{
    if (m_pIDMObject)
    {
        m_pIDMObject->Release();
        m_pIDMObject = NULL;
    }
    delete m_pvecReferences;
}

HRESULT CObject::Parse()

{
    if (m_ObjectDesc.m_dwValidData & DMUS_OBJ_FILENAME)
    {
        return ParseFromFile();
    }
    else if (m_ObjectDesc.m_dwValidData & DMUS_OBJ_MEMORY)
    {
        return ParseFromMemory();
    }
    else if (m_ObjectDesc.m_dwValidData & DMUS_OBJ_STREAM)
    {
        return ParseFromStream();
    }
    assert(false);
    return E_FAIL;
}

HRESULT CObject::ParseFromFile()

{
    HRESULT hr;
    IDirectMusicObject *pIObject = NULL;
    hr = DirectMusicCreateInstance(m_ObjectDesc.m_guidClass,
        NULL,IID_IDirectMusicObject,
        (void **) &pIObject);
    if (SUCCEEDED(hr))
    {
        WCHAR wzFullPath[DMUS_MAX_FILENAME];
        ZeroMemory( wzFullPath, sizeof(WCHAR) * DMUS_MAX_FILENAME );
        CFileStream *pStream = NULL; 
        DirectMusicCreateInstance(CLSID_DirectMusicFileStream,
            NULL, IID_CFileStream, (void **) &pStream );
        if (pStream)
        {
            pStream->Init(m_pClass->m_pLoader);
            if (m_ObjectDesc.m_dwValidData & DMUS_OBJ_FULLPATH)
            {
                wcscpy(wzFullPath,m_ObjectDesc.m_pwzFileName);
            }
            else
            {
                m_pClass->GetPath(wzFullPath);
                wcscat(wzFullPath,m_ObjectDesc.m_pwzFileName);
            }
            hr = pStream->Open(wzFullPath,GENERIC_READ);
            if (SUCCEEDED(hr))
            {
                DMUS_OBJECTDESC *pDESC = new DMUS_OBJECTDESC;
				assert(pDESC);
                memset((void *)pDESC,0,sizeof(DMUS_OBJECTDESC));
                pDESC->dwSize = sizeof (DMUS_OBJECTDESC);
                hr = pIObject->ParseDescriptor(pStream,pDESC);
                if (SUCCEEDED(hr))
                {
                    CDescriptor ParseDesc;
                    ParseDesc.Set(pDESC);
                    m_ObjectDesc.Merge(&ParseDesc);
                }
				delete pDESC;
            }
#ifdef XBOX
#ifdef DEBUG
            else {
                Trace(-1,"CObject::ParseFromFile: Could not open file: \"%S\"\n", wzFullPath);
            }
#endif
#endif
            pStream->Release();
        }
        pIObject->Release();
    }
    return hr;
}


HRESULT CObject::ParseFromMemory()

{
    HRESULT hr;
    IDirectMusicObject *pIObject = NULL;
    hr = DirectMusicCreateInstance(m_ObjectDesc.m_guidClass,
        NULL,IID_IDirectMusicObject,
        (void **) &pIObject);
    if (SUCCEEDED(hr))
    {
        CMemStream *pStream = NULL; 
        DirectMusicCreateInstance(CLSID_DirectMusicMemStream,
            NULL, IID_CMemStream, (void **) &pStream );
        if (pStream)
        {
            pStream->Init(m_pClass->m_pLoader);
            hr = pStream->Open(m_ObjectDesc.m_pbMemData,m_ObjectDesc.m_llMemLength);
            if (SUCCEEDED(hr))
            {
                DMUS_OBJECTDESC *pDESC = new DMUS_OBJECTDESC;
				assert(pDESC);
                memset((void *)pDESC,0,sizeof(DMUS_OBJECTDESC));
                pDESC->dwSize = sizeof (DMUS_OBJECTDESC);
                hr = pIObject->ParseDescriptor(pStream,pDESC);
                if (SUCCEEDED(hr))
                {
                    CDescriptor ParseDesc;
                    ParseDesc.Set(pDESC);
                    m_ObjectDesc.Merge(&ParseDesc);
                }
				delete pDESC;
            }
            pStream->Release();
        }
        pIObject->Release();
    }
    return hr;
}


HRESULT CObject::ParseFromStream()

{
    HRESULT hr;
    IDirectMusicObject *pIObject = NULL;
    hr = DirectMusicCreateInstance(m_ObjectDesc.m_guidClass,
        NULL,IID_IDirectMusicObject,
        (void **) &pIObject);
    if (SUCCEEDED(hr))
    {
        CStream *pStream = NULL; 
        DirectMusicCreateInstance(CLSID_DirectMusicStreamStream,
            NULL, IID_CStream, (void **) &pStream );
        if (pStream)
        {
            pStream->Init(m_pClass->m_pLoader);
            hr = pStream->Open(m_ObjectDesc.m_pIStream,
                m_ObjectDesc.m_liStartPosition);
            if (SUCCEEDED(hr))
            {
				// Allocate dynamically to avoid using stack space on XBox!
                DMUS_OBJECTDESC *pDESC = new DMUS_OBJECTDESC;
				assert(pDESC);
                memset((void *)pDESC,0,sizeof(DMUS_OBJECTDESC));
                pDESC->dwSize = sizeof (DMUS_OBJECTDESC);
                hr = pIObject->ParseDescriptor(pStream,pDESC);
                if (SUCCEEDED(hr))
                {
                    CDescriptor ParseDesc;
                    ParseDesc.Set(pDESC);
                    m_ObjectDesc.Merge(&ParseDesc);
                }
				delete pDESC;
            }
            pStream->Release();
        }
        pIObject->Release();
    }
    return hr;
}


// Record that this object can be garbage collected and prepare to store its references.
// Must be called before any of CObject's other routines.
HRESULT CObject::GC_Collectable()

{
    m_dwScanBits |= SCAN_GC;
    assert(!m_pvecReferences);

    m_pvecReferences = new SmartRef::Vector<CObject*>;
    if (!m_pvecReferences)
        return E_OUTOFMEMORY;
    return S_OK;
}

HRESULT CObject::GC_AddReference(CObject *pObject)

{
    assert(m_dwScanBits & SCAN_GC && m_pvecReferences);

    // don't track references to objects that aren't garbage collected
    if (!(pObject->m_dwScanBits & SCAN_GC))
        return S_OK;

    UINT uiPosNext = m_pvecReferences->size();
    for (UINT i = 0; i < uiPosNext; ++i)
    {
        if ((*m_pvecReferences)[i] == pObject)
            return S_OK;
    }

    if (!m_pvecReferences->AccessTo(uiPosNext))
        return E_OUTOFMEMORY;
    (*m_pvecReferences)[uiPosNext] = pObject;
    return S_OK;
}

HRESULT CObject::GC_RemoveReference(CObject *pObject)

{
    assert(m_dwScanBits & SCAN_GC && m_pvecReferences);

    SmartRef::Vector<CObject*> &vecRefs = *m_pvecReferences;
    UINT iEnd = vecRefs.size();
    for (UINT i = 0; i < iEnd; ++i)
    {
        if (vecRefs[i] == pObject)
        {
            // Remove by clearing the pointer.
            // The open slot will be compacted during garbage collection (GC_Mark).
            vecRefs[i] = NULL;
            return S_OK;
        }
    }
    return S_FALSE;
}

// Helper method used to implement ReleaseObject.
HRESULT CObject::GC_RemoveAndDuplicateInParentList()
{
    CObject* pObjectToFind = NULL;
    HRESULT hr = m_pClass->FindObject(&m_ObjectDesc, &pObjectToFind, this, m_pIDMObject);
    if (SUCCEEDED(hr) && pObjectToFind)
    {
        m_pClass->GC_Replace(this, NULL);
    }
    else
    {
        CObject *pObjectUnloaded = new CObject(m_pClass, &m_ObjectDesc);
        if (!pObjectUnloaded)
        {
            return E_OUTOFMEMORY;
        }

        m_pClass->GC_Replace(this, pObjectUnloaded);
    }
    return S_OK;
}

HRESULT CObject::Load()

{
    // See if we have one of the fields we need to load
    if (!(m_ObjectDesc.m_dwValidData & (DMUS_OBJ_FILENAME | DMUS_OBJ_MEMORY | DMUS_OBJ_STREAM)))
    {
        Trace(1, "Error: GetObject failed because the requested object was not already cached and the supplied desciptor did not specify a source to load the object from (DMUS_OBJ_FILENAME, DMUS_OBJ_MEMORY, or DMUS_OBJ_STREAM).\n");
        return DMUS_E_LOADER_NOFILENAME;
    }

    // Create the object
    SmartRef::ComPtr<IDirectMusicObject> scomIObject = NULL;
    HRESULT hr = DirectMusicCreateInstance(m_ObjectDesc.m_guidClass, NULL, IID_IDirectMusicObject, reinterpret_cast<void**>(&scomIObject));
    if (FAILED(hr))
        return hr;

    // Create the stream the object will load from
    SmartRef::ComPtr<IStream> scomIStream;
    if (m_ObjectDesc.m_dwValidData & DMUS_OBJ_FILENAME)
    {
        WCHAR wzFullPath[DMUS_MAX_FILENAME];
        ZeroMemory( wzFullPath, sizeof(WCHAR) * DMUS_MAX_FILENAME );
        CFileStream *pStream = NULL; 
        DirectMusicCreateInstance(CLSID_DirectMusicFileStream,
            NULL, IID_CFileStream, (void **) &pStream );
        if (!pStream)
            return E_OUTOFMEMORY;
        pStream->Init( m_pClass->m_pLoader );
        scomIStream = pStream;

        if (m_ObjectDesc.m_dwValidData & DMUS_OBJ_FULLPATH)
        {
            wcscpy(wzFullPath,m_ObjectDesc.m_pwzFileName);
        }
        else
        {
            m_pClass->GetPath(wzFullPath);
            wcscat(wzFullPath,m_ObjectDesc.m_pwzFileName);
        }
        hr = pStream->Open(wzFullPath,GENERIC_READ);
        if (FAILED(hr))
            return hr;
    }
    else if (m_ObjectDesc.m_dwValidData & DMUS_OBJ_MEMORY)
    {
        CMemStream *pStream = NULL; 
        DirectMusicCreateInstance(CLSID_DirectMusicMemStream,
            NULL, IID_CMemStream, (void **) &pStream );
        if (!pStream)
            return E_OUTOFMEMORY;
        pStream->Init(m_pClass->m_pLoader);
        scomIStream = pStream;
        hr = pStream->Open(m_ObjectDesc.m_pbMemData, m_ObjectDesc.m_llMemLength);
        if (FAILED(hr))
            return hr;
    }
    else if (m_ObjectDesc.m_dwValidData & DMUS_OBJ_STREAM)
    {
        CStream *pStream = NULL; 
        DirectMusicCreateInstance(CLSID_DirectMusicStreamStream,
            NULL, IID_CStream, (void **) &pStream );
        if (!pStream)
            return E_OUTOFMEMORY;
        pStream->Init(m_pClass->m_pLoader);
        scomIStream = pStream;
        hr = pStream->Open(m_ObjectDesc.m_pIStream, m_ObjectDesc.m_liStartPosition);
        if (FAILED(hr))
            return hr;
    }

    // Load the object
    IPersistStream* pIPS = NULL;
    hr = scomIObject->QueryInterface( IID_IPersistStream, (void**)&pIPS );
    if (FAILED(hr))
        return hr;
    // Save the new object.  Needs to be done before loading because of circular references.  While this object
    // loads it could get other objects and those other objects could need to get this object.
    SafeRelease(m_pIDMObject);
    m_pIDMObject = scomIObject.disown();
    hr = pIPS->Load( scomIStream );
    pIPS->Release();
    if (FAILED(hr))
    {
        // Clear the object we set above.
        SafeRelease(m_pIDMObject);
        return hr;
    }

    // Merge in descriptor information from the object
    CDescriptor Desc;
    DMUS_OBJECTDESC *pDESC = new DMUS_OBJECTDESC;
	assert(pDESC);
    memset((void *)pDESC,0,sizeof(DMUS_OBJECTDESC));
    pDESC->dwSize = sizeof (DMUS_OBJECTDESC);
    m_pIDMObject->GetDescriptor(pDESC);
    Desc.Set(pDESC);
    m_ObjectDesc.Merge(&Desc);
    m_ObjectDesc.m_dwValidData |= DMUS_OBJ_LOADED;
    m_ObjectDesc.Get(pDESC);
    m_pIDMObject->SetDescriptor(pDESC);
	delete pDESC;
    return hr;
}

// Collect everything that is unmarked.
void CObjectList::GC_Sweep(BOOL bOnlyScripts)

{
    // sweep through looking for unmarked GC objects
    CObject *pObjectPrev = NULL;
    CObject *pObjectNext = NULL;
    for (CObject *pObject = this->GetHead(); pObject; pObject = pObjectNext)
    {
        // get the next item now since we could be messing with the list
        pObjectNext = pObject->GetNext();

        bool fRemoved = false;
        if(bOnlyScripts && pObject->m_ObjectDesc.m_guidClass != CLSID_DirectMusicScript)
        {
            pObjectPrev = pObject;
            continue;
        }


        if (pObject->m_dwScanBits & SCAN_GC)
        {
            if (!(pObject->m_dwScanBits & SCAN_GC_MARK))
            {
                // the object is unused

                // Zombie it to break any cyclic references
                IDirectMusicObject *pIDMO = pObject->m_pIDMObject;
                if (pIDMO)
                {
                    IDirectMusicObjectP *pIDMO8 = NULL;
                    HRESULT hr = pIDMO->QueryInterface(IID_IDirectMusicObjectP, reinterpret_cast<void**>(&pIDMO8));
                    if (SUCCEEDED(hr))
                    {
                        pIDMO8->Zombie();
                        pIDMO8->Release();
                    }

#ifdef DBG
                    DebugTrace(4, SUCCEEDED(hr) ? "   *%08X Zombied\n" : "   *%08X no IDirectMusicObjectP interface\n", pObject);
#endif
                }

                // remove it from the list
                if (pObjectPrev)
                    pObjectPrev->Remove(pObject);
                else
                    this->RemoveHead();
                delete pObject;
                fRemoved = true;
            }
            else
            {
                // clear mark for next time
                pObject->m_dwScanBits &= ~SCAN_GC_MARK;
            }
        }

        if (!fRemoved)
            pObjectPrev = pObject;
    }
}

CClass::CClass(CLoader *pLoader)

{
    assert(pLoader);
    m_fDirSearched = FALSE;
    m_pLoader = pLoader;
    m_fKeepObjects = pLoader->m_fKeepObjects;
    m_dwLastIndex = NULL;
    m_pLastObject = NULL;
}

CClass::CClass(CLoader *pLoader, CDescriptor *pDesc)

{
    assert(pLoader);
    m_fDirSearched = FALSE;
    m_pLoader = pLoader;
    m_fKeepObjects = pLoader->m_fKeepObjects;
    m_dwLastIndex = NULL;
    m_pLastObject = NULL;

    // Set up this class's descritor with just the class id.
    m_ClassDesc.m_guidClass = pDesc->m_guidClass;
    m_ClassDesc.m_dwValidData = DMUS_OBJ_CLASS;
}


CClass::~CClass()

{
    ClearObjects(FALSE,NULL);
}

void CClass::ClearObjects(BOOL fKeepCache, WCHAR *pwzExtension)

//  Clear objects from the class list, optionally keep 
//  cached objects or objects that are not of the requested extension.

{
    m_fDirSearched = FALSE;
    CObjectList KeepList;   // Use to store objects to keep.
    while (!m_ObjectList.IsEmpty())
    {
        CObject *pObject = m_ObjectList.RemoveHead();
        DMUS_OBJECTDESC DESC;
        pObject->m_ObjectDesc.Get(&DESC);
        // If the keepCache flag is set, we want to hang on to the object
        // if it is GM.dls, an object that's currently cached, or
        // an object with a different extension from what we are looking for.
#ifdef XBOX

        if (fKeepCache && (pObject->m_pIDMObject || !pObject->m_ObjectDesc.IsExtension(pwzExtension)))

#else // XBOX

        if (fKeepCache && 
            ((DESC.guidObject == GUID_DefaultGMCollection)
#ifdef DRAGON
            || (DESC.guidObject == GUID_DefaultGMDrums)
#endif
            || pObject->m_pIDMObject 
            || !pObject->m_ObjectDesc.IsExtension(pwzExtension)))

#endif // XBOX

        {
            KeepList.AddHead(pObject);
        }
        else
        {
            delete pObject;
        }
    }
    //  Now put cached objects back in list.
    while (!KeepList.IsEmpty())
    {
        CObject *pObject = KeepList.RemoveHead();
        m_ObjectList.AddHead(pObject);
    }
    m_pLastObject = NULL;
}


HRESULT CClass::FindObject(CDescriptor *pDesc,CObject ** ppObject, CObject *pNotThis, IDirectMusicObject *pIDMObject)

{
    DWORD dwSearchBy = pDesc->m_dwValidData;
    CObject *pObject = NULL;

    if (dwSearchBy & DMUS_OBJ_OBJECT)
    {
        pObject = m_ObjectList.GetHead();
        for (;pObject != NULL; pObject = pObject->GetNext())
        {
            if (pObject == pNotThis) continue;
            if (pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_OBJECT)
            {
                if (pObject->m_ObjectDesc.m_guidObject == pDesc->m_guidObject)
                {
                    *ppObject = pObject;
                    return S_OK;
                }
            }
        }
    }
    if (dwSearchBy & DMUS_OBJ_MEMORY)
    {
        pObject = m_ObjectList.GetHead();
        for (;pObject != NULL; pObject = pObject->GetNext())
        {
            if (pObject == pNotThis) continue;
            if (pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_MEMORY)
            {
                if (pObject->m_ObjectDesc.m_pbMemData == pDesc->m_pbMemData)
                {
                    *ppObject = pObject;
                    return S_OK;
                }
            }
        }
    }
    if (dwSearchBy & DMUS_OBJ_STREAM)
    {
        pObject = m_ObjectList.GetHead();
        for (;pObject != NULL; pObject = pObject->GetNext())
        {
            if (pObject == pNotThis) continue;
            if (pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_STREAM)
            {
                if (pObject->m_ObjectDesc.m_pIStream == pDesc->m_pIStream)
                {
                    *ppObject = pObject;
                    return S_OK;
                }
            }
        }
    }
    if ((dwSearchBy & DMUS_OBJ_FILENAME) && (dwSearchBy & DMUS_OBJ_FULLPATH))
    {
        pObject = m_ObjectList.GetHead();
        for (;pObject != NULL; pObject = pObject->GetNext())
        {
            if (pObject == pNotThis) continue;
            if ((pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_FILENAME) &&
                (pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_FULLPATH))
            {
                if (!_wcsicmp(pObject->m_ObjectDesc.m_pwzFileName,pDesc->m_pwzFileName))
                {
                    *ppObject = pObject;
                    return S_OK;
                }
            }
        }
    }
    if ((dwSearchBy & DMUS_OBJ_NAME) && (dwSearchBy & DMUS_OBJ_CATEGORY))
    {
        pObject = m_ObjectList.GetHead();
        for (;pObject != NULL; pObject = pObject->GetNext())
        {
            if (pObject == pNotThis) continue;
            if ((pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_NAME) &&
                (pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_CATEGORY))
            {
                if (!_wcsicmp(pObject->m_ObjectDesc.m_pwzCategory,pDesc->m_pwzCategory))
                {
                    if (!_wcsicmp(pObject->m_ObjectDesc.m_pwzName,pDesc->m_pwzName))
                    {
                        *ppObject = pObject;
                        return S_OK;
                    }
                }
            }
        }
    }
    if (dwSearchBy & DMUS_OBJ_NAME)
    {
        pObject = m_ObjectList.GetHead();
        for (;pObject != NULL; pObject = pObject->GetNext())
        {
            if (pObject == pNotThis) continue;
            if (pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_NAME)
            {
                if (!_wcsicmp(pObject->m_ObjectDesc.m_pwzName,pDesc->m_pwzName))
                {
                    *ppObject = pObject;
                    return S_OK;
                }
            }
        }
    }
    if (dwSearchBy & DMUS_OBJ_FILENAME)
    {
        pObject = m_ObjectList.GetHead();
        for (;pObject != NULL; pObject = pObject->GetNext())
        {
            if (pObject == pNotThis) continue;
            if (pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_FILENAME)
            {
                if ((dwSearchBy & DMUS_OBJ_FULLPATH) == (pObject->m_ObjectDesc.m_dwValidData & DMUS_OBJ_FULLPATH))
                {
                    if (!_wcsicmp(pObject->m_ObjectDesc.m_pwzFileName,pDesc->m_pwzFileName))
                    {
                        *ppObject = pObject;
                        return S_OK;
                    }
                }
                else
                {
                    WCHAR *pC1 = pObject->m_ObjectDesc.m_pwzFileName;
                    WCHAR *pC2 = pDesc->m_pwzFileName;
                    if (dwSearchBy & DMUS_OBJ_FULLPATH)
                    {
                        pC1 = wcsrchr(pObject->m_ObjectDesc.m_pwzFileName, L'\\');
                    }
                    else
                    {
                        pC2 = wcsrchr(pDesc->m_pwzFileName, '\\');
                    }
                    if (pC1 && pC2)
                    {
                        if (!_wcsicmp(pC1,pC2))
                        {
                            *ppObject = pObject;
                            return S_OK;
                        }
                    }
                }
            }
        }
    }
    if (pIDMObject)
    {
        pObject = m_ObjectList.GetHead();
        for (;pObject != NULL; pObject = pObject->GetNext())
        {
            if (pObject == pNotThis) continue;
            if (pObject->m_pIDMObject == pIDMObject)
            {
                *ppObject = pObject;
                return S_OK;
            }
        }
    }

    *ppObject = NULL;
    return DMUS_E_LOADER_OBJECTNOTFOUND;
}

HRESULT CClass::EnumerateObjects(DWORD dwIndex,CDescriptor *pDesc)

{
    if (m_fDirSearched == FALSE)
    {
//      SearchDirectory();
    }
    if ((dwIndex < m_dwLastIndex) || (m_pLastObject == NULL))
    {
        m_dwLastIndex = 0;
        m_pLastObject = m_ObjectList.GetHead();
    }
    while (m_dwLastIndex < dwIndex)
    {
        if (!m_pLastObject) break;
        m_dwLastIndex++;
        m_pLastObject = m_pLastObject->GetNext();
    }
    if (m_pLastObject)
    {
        pDesc->Copy(&m_pLastObject->m_ObjectDesc);
        return S_OK;
    }
    return S_FALSE;
}

HRESULT CClass::GetPath(WCHAR * pwzPath)

{
    if (m_ClassDesc.m_dwValidData & DMUS_OBJ_FILENAME)
    {
        wcsncpy(pwzPath, m_ClassDesc.m_pwzFileName, DMUS_MAX_FILENAME);
        return S_OK;
    }
    else 
    {
        return m_pLoader->GetPath(pwzPath);
    }
}

// returns S_FALSE if the search directory is already set to this.
HRESULT CClass::SetSearchDirectory(WCHAR * pwzPath,BOOL fClear)

{
    HRESULT hr;

    hr = m_ClassDesc.SetFileName(pwzPath);
    if (SUCCEEDED(hr))
    {
        m_ClassDesc.m_dwValidData |= DMUS_OBJ_FULLPATH;
    }
    if (fClear)
    {
        CObjectList KeepList;   // Use to store objects to keep.
        while (!m_ObjectList.IsEmpty())
        {
            CObject *pObject = m_ObjectList.RemoveHead();
            if (pObject->m_pIDMObject)
            {
                KeepList.AddHead(pObject);
            }
#ifndef XBOX
            else
            {
                // check for the special case of the default gm collection.
                // don't clear that one out.
                DMUS_OBJECTDESC DESC;
                pObject->m_ObjectDesc.Get(&DESC);
                if( DESC.guidObject == GUID_DefaultGMCollection )
                {
                    KeepList.AddHead(pObject);
                }
                else
                {
                    delete pObject;
                }
            }
#endif
        }
        //  Now put cached objects back in list.
        while (!KeepList.IsEmpty())
        {
            CObject *pObject = KeepList.RemoveHead();
            m_ObjectList.AddHead(pObject);
        }
        m_pLastObject = NULL;
    }
    return hr;
}

HRESULT CClass::GetObject(CDescriptor *pDesc, CObject ** ppObject)

{
    
    HRESULT hr = FindObject(pDesc,ppObject);
    if (SUCCEEDED(hr)) // Okay, found object in list.
    {
        return hr;
    }
    *ppObject = new CObject (this, pDesc);
    if (*ppObject)
    {
        m_ObjectList.AddHead(*ppObject);
        return S_OK;
    }
    return E_OUTOFMEMORY;
}

void CClass::RemoveObject(CObject* pRemoveObject)
//  Remove an object from the class list
{
    CObjectList KeepList;   // Use to store objects to keep.
    while (!m_ObjectList.IsEmpty())
    {
        CObject *pObject = m_ObjectList.RemoveHead();
        if(( pObject == pRemoveObject ) || (!pRemoveObject && pObject->m_ObjectDesc.IsEmpty()))
        {
            delete pObject;
            // we can assume no duplicates, and we should avoid comparing the deleted
            // object to the remainder of the list
            if (pRemoveObject)
                break;
        }
        else
        {
            KeepList.AddHead(pObject);
        }
    }
    //  Now put cached objects back in list.
    while (!KeepList.IsEmpty())
    {
        CObject *pObject = KeepList.RemoveHead();
        m_ObjectList.AddHead(pObject);
    }
    m_pLastObject = NULL;
}

HRESULT CClass::ClearCache(bool fClearStreams)

{
    CObject *pObject = m_ObjectList.GetHead();
    CObject *pObjectPrev = NULL; // remember the previous object -- needed to quickly remove the current object from the list
    CObject *pObjectNext = NULL; // remember the next object -- needed because the current object may be removed from the list
    for (;pObject;pObject = pObjectNext)
    {
        // Should always clear the IStreams.
//        if (fClearStreams)
        pObject->m_ObjectDesc.ClearIStream();
        pObjectNext = pObject->GetNext();
        if (pObject->m_pIDMObject)
        {
            if (pObject->m_dwScanBits & SCAN_GC)
            {
                // Other objects may have references to this one so we need to keep this object around
                // and track its references.  We'll hold onto the DMObject pointer too because we may
                // later need to Zombie the object in order to break a cyclic reference.

                // We'll place an unloaded object with a duplicate descriptor in the cache to match the
                // non-GC behavior and then move the original object into a list of released objects that
                // will eventually be reclaimed by CollectGarbage.

                CObject *pObjectUnloaded = new CObject(this, &pObject->m_ObjectDesc);
                if (!pObjectUnloaded)
                {
                    return E_OUTOFMEMORY;
                }

                if (!pObjectPrev)
                    m_ObjectList.Remove(pObject);
                else
                    pObjectPrev->Remove(pObject);
                m_ObjectList.AddHead(pObjectUnloaded);
                m_pLoader->GC_UpdateForReleasedObject(pObject);
            }
            else
            {
                pObject->m_pIDMObject->Release();
                pObject->m_pIDMObject = NULL;
                pObject->m_ObjectDesc.m_dwValidData &= ~DMUS_OBJ_LOADED;

                pObjectPrev = pObject;
            }
        }
    }

    return S_OK;
}

// return S_FALSE if the cache is already enabled according to fEnable,
// indicating it's already been done.
HRESULT CClass::EnableCache(BOOL fEnable)

{
    HRESULT hr = S_FALSE;
    if (!fEnable)
    {
        ClearCache(false);
    }
    if( m_fKeepObjects != fEnable )
    {
        hr = S_OK;
        m_fKeepObjects = fEnable;
    }
    return hr;
}

typedef struct ioClass
{
    GUID    guidClass;
} ioClass;


HRESULT CClass::SaveToCache(IRIFFStream *pRiff)

{
    HRESULT hr = S_OK;
    IStream* pIStream;
    MMCKINFO ck;
    WORD wStructSize;
    DWORD dwBytesWritten;
//  DWORD dwBufferSize;
    ioClass oClass;

    ZeroMemory(&ck, sizeof(MMCKINFO));

    pIStream = pRiff->GetStream();
    if( pIStream == NULL )
    {
        // I don't think anybody should actually be calling this function
        // if they don't have a stream.  Currently, this is only called by
        // SaveToCache file.  It definitely has a stream when it calls
        // AllocRIFFStream and the stream should still be there when
        // we arrive here.
        assert(false);

        return DMUS_E_LOADER_NOFILENAME;
    }

    // Write class chunk header
    ck.ckid = FOURCC_CLASSHEADER;
    if( pRiff->CreateChunk( &ck, 0 ) == 0 )
    {
        wStructSize = sizeof(ioClass);
        hr = pIStream->Write( &wStructSize, sizeof(wStructSize), &dwBytesWritten );
        if( FAILED( hr ) ||  dwBytesWritten != sizeof(wStructSize) )
        {
            pIStream->Release();
            return DMUS_E_CANNOTWRITE;
        }
        // Prepare ioClass structure
    //  memset( &oClass, 0, sizeof(ioClass) );
        memcpy( &oClass.guidClass, &m_ClassDesc.m_guidClass, sizeof(GUID) );

        // Write Class header data
        hr = pIStream->Write( &oClass, sizeof(oClass), &dwBytesWritten);
        if( FAILED( hr ) ||  dwBytesWritten != sizeof(oClass) )
        {
            hr = DMUS_E_CANNOTWRITE;
        }
        else
        {
            if( pRiff->Ascend( &ck, 0 ) != 0 )
            {
                hr = DMUS_E_CANNOTSEEK;
            }
        }

    }
    else
    {
        hr = DMUS_E_CANNOTSEEK;
    }
    pIStream->Release();
    return hr;
}

void CClass::PreScan()

/*  Prior to scanning a directory, mark all currently loaded objects
    so they won't be confused with objects loaded in the scan or
    referenced by the cache file.
*/

{
    CObject *pObject = m_ObjectList.GetHead();
    for (;pObject != NULL; pObject = pObject->GetNext())
    {
        // clear the lower fields and set SCAN_PRIOR
        pObject->m_dwScanBits &= ~(SCAN_CACHE | SCAN_PARSED | SCAN_SEARCH);
        pObject->m_dwScanBits |= SCAN_PRIOR;
    }
}

// Helper method used to implement RemoveAndDuplicateInParentList.
void CClass::GC_Replace(CObject *pObject, CObject *pObjectReplacement)

{
    m_ObjectList.Remove(pObject);
    if (pObjectReplacement)
    {
        m_ObjectList.AddHead(pObjectReplacement);
    }
}

HRESULT CClass::SearchDirectory(WCHAR *pwzExtension)

{
    HRESULT hr;
    IDirectMusicObject *pIObject = NULL;
    hr = DirectMusicCreateInstance(m_ClassDesc.m_guidClass,
        NULL,IID_IDirectMusicObject,
        (void **) &pIObject);
    if (SUCCEEDED(hr))
    {
        CFileStream *pStream = new CFileStream;
        if (pStream)
        {
            pStream->Init(m_pLoader);
            WCHAR wzPath[MAX_PATH];
            hr = GetPath(wzPath);
            if (SUCCEEDED(hr))
            {
                hr = S_FALSE;
                CObjectList TempList;
#ifdef USE_ANSI
                char szPath[MAX_PATH];
                WIN32_FIND_DATAA fileinfoA;
#endif      
#ifdef USE_UNICODE
                WIN32_FIND_DATAW fileinfoW;
#endif
                HANDLE  hFindFile;
                CObject * pObject;
                wcscat(wzPath, L"*." );
                wcscat(wzPath, pwzExtension);
#ifdef USE_BOTH
                if (g_fIsUnicode)
                {
                    hFindFile = FindFirstFileW( wzPath, &fileinfoW );
                }
                else
                {
                    wcstombs( szPath, wzPath, MAX_PATH );
                    hFindFile = FindFirstFileA( szPath, &fileinfoA );
                }
#endif
#ifdef ONLY_UNICODE
				hFindFile = FindFirstFileW( wzPath, &fileinfoW );
#endif
#ifdef ONLY_ANSI
				wcstombs( szPath, wzPath, MAX_PATH );
                hFindFile = FindFirstFileA( szPath, &fileinfoA );
#endif // ONLY_ANSI

                if( hFindFile == INVALID_HANDLE_VALUE )
                {
                    pStream->Release();
                    pIObject->Release();
                    return S_FALSE;
                }
                ClearObjects(TRUE, pwzExtension); // Clear everything but the objects currently loaded.
                for (;;)
                {
                    BOOL fGoParse = FALSE;
                    CDescriptor Desc;
                    GetPath(wzPath);
#ifdef USE_BOTH
                    if (g_fIsUnicode)
                    {
                        Desc.m_ftDate = fileinfoW.ftLastWriteTime;
                        wcsncat(wzPath,fileinfoW.cFileName,DMUS_MAX_FILENAME);
                    }
                    else
                    {
                        Desc.m_ftDate = fileinfoA.ftLastWriteTime;
                        WCHAR wzFileName[MAX_PATH];
                        mbstowcs( wzFileName, fileinfoA.cFileName, MAX_PATH );
                        wcsncat(wzPath,wzFileName,DMUS_MAX_FILENAME);
                    }
#endif
#ifdef ONLY_UNICODE
					Desc.m_ftDate = fileinfoW.ftLastWriteTime;
                    wcsncat(wzPath,fileinfoW.cFileName,DMUS_MAX_FILENAME);
#endif
#ifdef ONLY_ANSI
					Desc.m_ftDate = fileinfoA.ftLastWriteTime;
                    WCHAR wzFileName[MAX_PATH];
                    mbstowcs( wzFileName, fileinfoA.cFileName, MAX_PATH );
                    wcsncat(wzPath,wzFileName,DMUS_MAX_FILENAME);
#endif
                    if (SUCCEEDED(Desc.SetFileName(wzPath)))
                    {
                        Desc.m_dwValidData = (DMUS_OBJ_DATE | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH);
                    }
                    if (SUCCEEDED(FindObject(&Desc,&pObject))) // Make sure we don't already have it.
                    {
#ifdef USE_BOTH
                        if (g_fIsUnicode)
                        {
                            fGoParse = (fileinfoW.nFileSizeLow != pObject->m_ObjectDesc.m_dwFileSize);
                            if (!fGoParse)
                            {
                                fGoParse = !memcmp(&fileinfoW.ftLastWriteTime,&pObject->m_ObjectDesc.m_ftDate,sizeof(FILETIME));
                            }
                        }
                        else
                        {
                            fGoParse = (fileinfoA.nFileSizeLow != pObject->m_ObjectDesc.m_dwFileSize);
                            if (!fGoParse)
                            {
                                fGoParse = !memcmp(&fileinfoA.ftLastWriteTime,&pObject->m_ObjectDesc.m_ftDate,sizeof(FILETIME));
                            }
                        }
#endif
#ifdef ONLY_UNICODE
                        fGoParse = (fileinfoW.nFileSizeLow != pObject->m_ObjectDesc.m_dwFileSize);
                        if (!fGoParse)
                        {
                            fGoParse = !memcmp(&fileinfoW.ftLastWriteTime,&pObject->m_ObjectDesc.m_ftDate,sizeof(FILETIME));
                        }
#endif
#ifdef ONLY_ANSI
                        fGoParse = (fileinfoA.nFileSizeLow != pObject->m_ObjectDesc.m_dwFileSize);
                        if (!fGoParse)
                        {
                            fGoParse = !memcmp(&fileinfoA.ftLastWriteTime,&pObject->m_ObjectDesc.m_ftDate,sizeof(FILETIME));
                        }
#endif
							// Yet, disregard if it is already loaded.
                        if (pObject->m_pIDMObject) fGoParse = FALSE;
                    }
                    else fGoParse = TRUE;
                    if (fGoParse)
                    {
                        HRESULT hrTemp = pStream->Open(Desc.m_pwzFileName,GENERIC_READ);
                        if (SUCCEEDED(hrTemp))
                        {
                            DMUS_OBJECTDESC DESC;
                            memset((void *)&DESC,0,sizeof(DESC));
                            DESC.dwSize = sizeof (DMUS_OBJECTDESC);
                            hrTemp = pIObject->ParseDescriptor(pStream,&DESC);
                            if (SUCCEEDED(hrTemp))
                            {
                                hr = S_OK;
                                CDescriptor ParseDesc;
                                ParseDesc.Set(&DESC);
                                Desc.Merge(&ParseDesc);
#ifdef USE_BOTH
                                if (g_fIsUnicode)
                                {
                                    Desc.m_dwFileSize = fileinfoW.nFileSizeLow;
                                    Desc.m_ftDate = fileinfoW.ftLastWriteTime;
                                }
                                else
                                {
                                    Desc.m_dwFileSize = fileinfoA.nFileSizeLow;
                                    Desc.m_ftDate = fileinfoA.ftLastWriteTime;
                                }
#endif                          
#ifdef ONLY_UNICODE
                                Desc.m_dwFileSize = fileinfoW.nFileSizeLow;
                                Desc.m_ftDate = fileinfoW.ftLastWriteTime;
#endif
#ifdef ONLY_ANSI
                                Desc.m_dwFileSize = fileinfoA.nFileSizeLow;
                                Desc.m_ftDate = fileinfoA.ftLastWriteTime;
#endif
                                if (pObject)
                                {
                                    pObject->m_ObjectDesc.Copy(&Desc);
                                    pObject->m_dwScanBits |= SCAN_PARSED | SCAN_SEARCH;
                                }
                                else
                                {
                                    pObject = new CObject(this, &Desc);
                                    if (pObject)
                                    {
                                        TempList.AddHead(pObject);
                                        pObject->m_dwScanBits |= SCAN_PARSED | SCAN_SEARCH;
                                    }
                                }
                            }
                            pStream->Close();
                        }
                    }
#ifdef USE_BOTH
                    if (g_fIsUnicode)
                    {
                        if ( !FindNextFileW( hFindFile, &fileinfoW ) ) break;
                    }
                    else
                    {
                        if ( !FindNextFileA( hFindFile, &fileinfoA ) ) break;
                    }
#endif
#ifdef ONLY_UNICODE
					if ( !FindNextFileW( hFindFile, &fileinfoW ) ) break;
#endif
#ifdef ONLY_ANSI
					if ( !FindNextFileA( hFindFile, &fileinfoA ) ) break;
#endif
                }
                FindClose(hFindFile );
                while (!TempList.IsEmpty())
                {
                    pObject = TempList.RemoveHead();
                    m_ObjectList.AddHead(pObject);
                }
                m_fDirSearched = TRUE;
            }
            pStream->Release();
        }
        pIObject->Release();
    }
    return hr;
}



