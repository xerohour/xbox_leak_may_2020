// Container.cpp : Implementation of CContainer
//
// Copyright (c) 1999-1999 Microsoft Corporation
//
//

/*#include "dmusicc.h" 
#include "dmusicip.h" 
#include "dmusicf.h" 
#include "validate.h"
#include "container.h"
#include "loader.h"
#include "debug.h"
#include "riff.h"
#include "dmscriptautguids.h"
#include "smartref.h"
#include "miscutil.h"
#ifdef UNDER_CE
#include "dragon.h"
#endif*/

#include "pchloader.h"

extern long g_cComponent;

CContainerItem::CContainerItem(bool fEmbedded)

{
    m_Desc.dwSize = sizeof(m_Desc);
    m_Desc.dwValidData = 0;
    m_dwFlags = 0;
    m_pObject = NULL;
    m_fEmbedded = fEmbedded;
    m_pwszAlias = NULL;
}

CContainerItem::~CContainerItem()

{
    if (m_pObject)
    {
        m_pObject->Release();
    }
    if (m_Desc.dwValidData & DMUS_OBJ_STREAM)
    {
        SafeRelease(m_Desc.pStream);
    }
    delete m_pwszAlias;
}

CContainer::CContainer() : m_MemTrack(DMTRACK_CONTAINER)
{
    m_cRef = 1;
    m_dwFlags = 0;
    m_dwPartialLoad = 0;
    m_pStream = NULL;
    m_fZombie = false;
}

CContainer::~CContainer()
{
    Clear();
    if (m_pStream)
    {
        m_pStream->Release();
    }
}

void CContainer::Clear()
{
    IDirectMusicLoader *pLoader = NULL;
    IDirectMusicGetLoader *pGetLoader = NULL;
    if (m_pStream)
    {
        m_pStream->QueryInterface(IID_IDirectMusicGetLoader,(void **) &pGetLoader);
        if (pGetLoader)
        {
            pGetLoader->GetLoader(&pLoader);
            pGetLoader->Release();
        }
    }
    CContainerItem *pItem = m_ItemList.GetHead();
    CContainerItem *pNext;
    for (;pItem;pItem = pNext)
    {
        pNext = pItem->GetNext();
        if (pItem->m_pObject)
        {
            if (pLoader && !(pItem->m_dwFlags & DMUS_CONTAINED_OBJF_KEEP))
            {
#ifdef DXAPI
                pLoader->ReleaseObject(pItem->m_pObject);
#else
                pLoader->ReleaseObjectByUnknown(pItem->m_pObject);
#endif
            }
            pItem->m_pObject->Release();
            pItem->m_pObject = NULL;
        }
        delete pItem;
    }
    if (pLoader)
    {
        pLoader->Release();
    }
}

STDMETHODIMP_(void) CContainer::Zombie()
{
    Clear();
    if (m_pStream)
    {
        m_pStream->Release();
        m_pStream = NULL;
    }
    m_fZombie = true;
}

STDMETHODIMP_(ULONG) CContainer::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CContainer::Release()
{
    if (!InterlockedDecrement(&m_cRef)) 
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

STDMETHODIMP CContainer::QueryInterface( const IID &riid, void **ppvObj )
{
    if (riid == IID_IUnknown || riid == IID_IDirectMusicContainer) {
        *ppvObj = static_cast<IDirectMusicContainer*>(this);
        AddRef();
        return S_OK;
    }
    else if (riid == IID_IDirectMusicObject)
    {
        *ppvObj = static_cast<IDirectMusicObject*>(this);
        AddRef();
        return S_OK;
    }
    else if (riid == IID_IDirectMusicObjectP)
    {
        *ppvObj = static_cast<IDirectMusicObjectP*>(this);
    }
    else if (riid == IID_IPersistStream) 
    {
        *ppvObj = static_cast<IPersistStream*>(this);
        AddRef();
        return S_OK;
    }
    *ppvObj = NULL;
    return E_NOINTERFACE;
}

HRESULT CContainer::EnumObject(REFGUID rguidClass,
                               DWORD dwIndex,
                               LPDMUS_OBJECTDESC pDesc,
                               WCHAR *pwszAlias)
{
    V_INAME(CContainer::EnumObject);
    V_PTR_WRITE_OPT(pDesc, LPDMUS_OBJECTDESC);
    V_BUFPTR_WRITE_OPT(pwszAlias, MAX_PATH);
    V_REFGUID(rguidClass);

    CHECK_ZOMBIE(m_fZombie)
    
    CContainerItem *pItem = m_ItemList.GetHead();
    DWORD dwCounter = 0;
    HRESULT hr = S_FALSE;
    for (;pItem;pItem = pItem->GetNext())
    {
        if ((rguidClass == GUID_DirectMusicAllTypes) || 
            (rguidClass == pItem->m_Desc.guidClass))
        {
            if (dwCounter == dwIndex)
            {
                hr = S_OK;
                if (pDesc)
                {
                    DWORD dwCopySize = min(pDesc->dwSize,pItem->m_Desc.dwSize);
                    memcpy(pDesc,&pItem->m_Desc,dwCopySize);
                    if (pDesc->dwValidData & DMUS_OBJ_STREAM && pDesc->pStream)
                        pDesc->pStream->AddRef();
                }
                if (pwszAlias)
                {
                    hr = wcsTruncatedCopy(pwszAlias, pItem->m_pwszAlias ? pItem->m_pwszAlias : L"", MAX_PATH);
                }
                break;
            }
            dwCounter++;
        }
    }
    return hr;
}

HRESULT CContainer::Load(CRiffParser *pParser, IDirectMusicLoader *pLoader)

{
    RIFFIO ckNext;
    RIFFIO ckChild;
    HRESULT hr = S_OK;
    DMUS_IO_CONTAINER_HEADER ioHeader;
    pParser->EnterList(&ckNext);
    while(pParser->NextChunk(&hr))
    {
        switch(ckNext.ckid)
        {
        case DMUS_FOURCC_CONTAINER_CHUNK :
            hr = pParser->Read(&ioHeader, sizeof(DMUS_IO_CONTAINER_HEADER));
            m_dwFlags = ioHeader.dwFlags;
            break;
        case DMUS_FOURCC_GUID_CHUNK:
        case DMUS_FOURCC_VERSION_CHUNK:
        case DMUS_FOURCC_CATEGORY_CHUNK:
        case DMUS_FOURCC_DATE_CHUNK:
            hr = m_Info.ReadChunk(pParser,ckNext.ckid);
			break;
		case FOURCC_LIST:
			switch(ckNext.fccType)
			{
            case DMUS_FOURCC_UNFO_LIST:
                hr = m_Info.ReadChunk(pParser,ckNext.fccType);
				break;
            case DMUS_FOURCC_CONTAINED_OBJECTS_LIST :
                pParser->EnterList(&ckChild);
                while(pParser->NextChunk(&hr))
                {
                    switch(ckChild.ckid)
                    {
                    case FOURCC_RIFF:
                    case FOURCC_LIST:
                        switch(ckChild.fccType)
                        {
                        case DMUS_FOURCC_CONTAINED_OBJECT_LIST :
                            hr = LoadObject(pParser, pLoader);
                            break;
                        }
                        break;
                    }
                }
                pParser->LeaveList();
            }
        }
    }
    pParser->LeaveList();
    if (SUCCEEDED(hr))
    {
        pParser->LeaveList(); // This is to ascend the caller completely outside the container.
        if (!(m_dwFlags & DMUS_CONTAINER_NOLOADS))
        {
            for (CContainerItem *pItem = m_ItemList.GetHead();pItem && SUCCEEDED(hr);pItem = pItem->GetNext())
            {
#ifdef DXAPI
                if (FAILED(pLoader->GetObject(&pItem->m_Desc,
                    IID_IDirectMusicObject,
                    (void **)&pItem->m_pObject)))
                {
                    hr = DMUS_S_PARTIALLOAD;
                }
#else
                hr = pLoader->GetObject(&pItem->m_Desc,IID_IDirectMusicObject,
                    (void **)&pItem->m_pObject);
#endif
            }
        }

        if (m_pStream)
        {
            m_pStream->Release();
        }
        m_pStream = pParser->GetStream();
        m_pStream->AddRef();
    }
    return hr;
}


HRESULT CContainer::LoadObject(CRiffParser *pParser,
                              IDirectMusicLoader *pLoader)
{
    RIFFIO ckNext;
    DMUS_IO_CONTAINED_OBJECT_HEADER ioHeader;
    HRESULT hr = S_OK;
    DWORD dwStep = 0;
    SmartRef::Buffer<WCHAR> wbufAlias;
    pParser->EnterList(&ckNext);  
    while (pParser->NextChunk(&hr))
    {
        switch (ckNext.ckid)
        {
        case DMUS_FOURCC_CONTAINED_ALIAS_CHUNK:
            if (dwStep > 1) hr = DMUS_E_INVALID_CONTAINER_OBJECT;
            else
            {
                wbufAlias.Alloc(ckNext.cksize / 2);
                if (wbufAlias) 
                {
                    hr = pParser->Read(wbufAlias,ckNext.cksize);
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                }
            }
            break;
        case DMUS_FOURCC_CONTAINED_OBJECT_CHUNK:
            if (dwStep > 1) hr = DMUS_E_INVALID_CONTAINER_OBJECT;
            else
            {
                hr = pParser->Read(&ioHeader, sizeof(DMUS_IO_CONTAINED_OBJECT_HEADER));
                if(ioHeader.ckid == 0 && ioHeader.fccType == NULL)
                {
                    Trace(1,"Invalid object header in Container.\n");
                    hr = DMUS_E_INVALID_CONTAINER_OBJECT;
                }
            }
            dwStep = 1;
            break;
        case FOURCC_LIST:
        case FOURCC_RIFF:
            if (dwStep < 1) hr = DMUS_E_INVALID_CONTAINER_OBJECT;
            else
            {
                dwStep = 2;
                if ((ckNext.fccType == ioHeader.fccType) || (ckNext.ckid == ioHeader.ckid))
                {
                    // Okay, this is the chunk we are looking for.
                    // Seek back to start of chunk.
                    bool fEmbedded = !(ckNext.ckid == FOURCC_LIST && ckNext.fccType == DMUS_FOURCC_REF_LIST);
                    CContainerItem *pItem = new CContainerItem(fEmbedded);
                    if (!pItem)
                        hr = E_OUTOFMEMORY;
                    else
                    {
                        if (fEmbedded)
                        {
                            // This is an embedded object.  Return to the position where from which it will be loaded.
                            pParser->SeekBack();
                            pItem->m_Desc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_STREAM;
                            pItem->m_Desc.guidClass = ioHeader.guidClassID;
                            pItem->m_Desc.pStream = pParser->GetStream();
                            pItem->m_Desc.pStream->AddRef();
                        }
                        else
                        {
                            // This is a reference chunk.  Read the object descriptor.
                            CLinkParser LinkParser;
                            hr = LinkParser.ReadReference(pParser, &pItem->m_Desc);
                        }

                        if (SUCCEEDED(hr))
                        {
                            // We will call SetObject on items in the container here.  The items are loaded later.
                            // This ensures that out-of-order references between objects can be retrieved as the objects
                            // load themselves.
#ifdef DXAPI
                            pLoader->SetObject(&pItem->m_Desc);
#else
                            hr = pLoader->SetObject(&pItem->m_Desc);
#endif
                        }
                        if (SUCCEEDED(hr))
                        {
                            if (pItem->m_Desc.dwValidData & DMUS_OBJ_STREAM)
                            {
                                // The loader has the stream now so we don't need it any more.
                                pItem->m_Desc.dwValidData &= ~DMUS_OBJ_STREAM;
                                SafeRelease(pItem->m_Desc.pStream);
                                pParser->SeekForward();
                            }
                            pItem->m_pwszAlias = wbufAlias.disown();
                            m_ItemList.AddTail(pItem);
                        }
                        else
                        {
                            delete pItem;
                        }
                    }
                }
            }
        }
    }
    pParser->LeaveList();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// IPersist

HRESULT CContainer::GetClassID( CLSID* pClassID )
{
    if (pClassID)
    {
        *pClassID = CLSID_DirectMusicContainer;
        return S_OK;
    }
    return E_POINTER;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistStream functions

HRESULT CContainer::IsDirty()
{
    return S_FALSE;
}

HRESULT CContainer::Load( IStream* pStream )
{
    V_INAME(IPersistStream::Load);
    V_INTERFACE(pStream);

    IDirectMusicLoader *pLoader = NULL;
    IDirectMusicGetLoader *pGetLoader = NULL;
    if (pStream)
    {
        pStream->QueryInterface(IID_IDirectMusicGetLoader,(void **) &pGetLoader);
        if (pGetLoader)
        {
            pGetLoader->GetLoader(&pLoader);
            pGetLoader->Release();
        }
    }
    if (pLoader)
    {
        HRESULT hr = S_OK;
        CRiffParser Parser(pStream);
        RIFFIO ckMain;
        Parser.EnterList(&ckMain);  
        if (Parser.NextChunk(&hr) && (ckMain.fccType == DMUS_FOURCC_CONTAINER_FORM))
        {
            hr = Load(&Parser, pLoader);
        }
        else
        {
            Trace(1,"Error: Failed parsing - file is not in Container format.\n");
            hr = DMUS_E_DESCEND_CHUNK_FAIL;
        }
        pLoader->Release();
        return hr;
    }
    Trace(1, "Error: unable to load container from a stream because it doesn't support the IDirectMusicGetLoader interface.\n");
    return DMUS_E_UNSUPPORTED_STREAM;
}


HRESULT CContainer::Save( IStream* pIStream, BOOL fClearDirty )
{
    return E_NOTIMPL;
}

HRESULT CContainer::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// IDirectMusicObject

STDMETHODIMP CContainer::GetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
    // Argument validation
    V_INAME(CContainer::GetDescriptor);
    V_PTR_WRITE(pDesc, DMUS_OBJECTDESC); 

    CHECK_ZOMBIE(m_fZombie)
    
    return (m_Info.GetDescriptor(pDesc,CLSID_DirectMusicContainer));
}

STDMETHODIMP CContainer::SetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
    // Argument validation
    V_INAME(CContainer::SetDescriptor);
    V_PTR_READ(pDesc, DMUS_OBJECTDESC); 
    
    CHECK_ZOMBIE(m_fZombie)

    return (m_Info.SetDescriptor(pDesc));
}

STDMETHODIMP CContainer::ParseDescriptor(LPSTREAM pStream, LPDMUS_OBJECTDESC pDesc) 
{
    V_INAME(CContainer::ParseDescriptor);
    V_INTERFACE(pStream);
    V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC);

    return (m_Info.ParseDescriptor(pStream,pDesc,DMUS_FOURCC_CONTAINER_FORM,CLSID_DirectMusicContainer));
}
