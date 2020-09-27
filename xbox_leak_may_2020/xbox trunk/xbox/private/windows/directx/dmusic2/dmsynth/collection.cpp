#include <xtl.h>
#include <xboxutil.h>
#define COM_NO_WINDOWS_H
#define RPC_NO_WINDOWS_H
#define _WINGDI_
//#include <objbase.h>
//#include <mmsystem.h>
//#include <dsoundp.h>
//#include <ks.h>

#include "debug.h"
#include "dmusicc.h"
#include "dmusics.h"

STDAPI DirectMusicCollectionCreate(REFIID iid, void ** ppDMC )
{
    CCollection *pCollection = new CCollection;
    if (pCollection)
    {
        return pCollection->QueryInterface(iid,ppDMC);
    }
    return E_OUTOFMEMORY;
}

CCollection::CCollection() : m_MemTrack(DMTRACK_COLLECTION)
{
	m_cRef = 1;
	INITIALIZE_CRITICAL_SECTION(&m_CrSec);
}

CCollection::~CCollection()
{
	DELETE_CRITICAL_SECTION(&m_CrSec);
}

STDMETHODIMP CCollection::QueryInterface(const IID &iid, void **ppv)
{
    V_INAME(IDirectMusicCollection::QueryInterface);
    V_REFGUID(iid);
    V_PTRPTR_WRITE(ppv);


	if(iid == IID_IUnknown || iid == IID_IDirectMusicCollection)
	{
		*ppv = static_cast<IDirectMusicCollection*>(this);
	} 
	else if(iid == IID_IDirectMusicObject)
	{
		*ppv = static_cast<IDirectMusicObject*>(this);
	}
	else if(iid == IID_IPersistStream)
	{
		*ppv = static_cast<IPersistStream*>(this);
	}
	else if(iid == IID_IPersist)
	{
		*ppv = static_cast<IPersist*>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

    reinterpret_cast<IUnknown*>(this)->AddRef();
    
	return S_OK;
}


STDMETHODIMP_(ULONG) CCollection::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CCollection::Release()
{
    if (!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

/////////////////////////////////////////////////////////////////////////////
// IPersist

HRESULT CCollection::GetClassID( CLSID* pClassID )
{
	V_INAME(CCollection::GetClassID);
	V_PTR_WRITE(pClassID, CLSID); 
	*pClassID = CLSID_DirectMusicCollection;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistStream functions

HRESULT CCollection::IsDirty()
{
	return S_FALSE;
}

HRESULT CCollection::Load( IStream* pIStream )
{
	V_INAME(IPersistStream::Load);
	V_INTERFACE(pIStream);

    CRiffParser Parser(pIStream);
    RIFFIO ckMain;
    HRESULT hr = S_OK;

    Parser.EnterList(&ckMain);  
    if (Parser.NextChunk(&hr) && (ckMain.fccType == FOURCC_DLS))
    {
        hr = Load(&Parser);
    }
    else
    {
        Trace(1,"Error: Unknown file format when parsing DLS Collection\n");
        hr = DMUS_E_NOTADLSCOL;
    }
    return hr;
}

HRESULT CCollection::Load(CRiffParser *pParser)

{
	RIFFIO ckNext;
    RIFFIO ckChild;
    HRESULT hr = S_OK;
    pParser->EnterList(&ckNext);
	while(pParser->NextChunk(&hr))
	{
		switch(ckNext.ckid)
		{
        case FOURCC_DLID:
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
				case DMUS_FOURCC_TOOL_LIST:
                    pParser->EnterList(&ckChild);
	                while(pParser->NextChunk(&hr))
	                {
		                if ((ckChild.ckid == FOURCC_RIFF) && 
                            (ckChild.fccType == DMUS_FOURCC_TOOL_FORM))
                        {
			                hr = LoadTool(pParser);
		                } 
	                }
                    pParser->LeaveList();
					break;
			}
			break;
		}
	}
    pParser->LeaveList();

	return hr;
}

HRESULT CCollection::LoadTool(CRiffParser *pParser)
{
	RIFFIO ckNext;
	DWORD cbSize;
	
	DMUS_IO_TOOL_HEADER ioDMToolHdr;
	DWORD *pdwPChannels = NULL;

	HRESULT hr = S_OK; 

    pParser->EnterList(&ckNext);

    if (pParser->NextChunk(&hr))
    {
		if(ckNext.ckid != DMUS_FOURCC_TOOL_CHUNK)
		{
            pParser->LeaveList();
            Trace(1,"Error: Tool header chunk not first in tool list.\n");
			return DMUS_E_TOOL_HDR_NOT_FIRST_CK;
		}	
		
		hr = pParser->Read(&ioDMToolHdr, sizeof(DMUS_IO_TOOL_HEADER));
		
		if(ioDMToolHdr.ckid == 0 && ioDMToolHdr.fccType == NULL)
		{
			pParser->LeaveList();
            Trace(1,"Error: Invalid Tool header.\n");
			return DMUS_E_INVALID_TOOL_HDR;
		}
		
		if(ioDMToolHdr.cPChannels)
		{
			pdwPChannels = new DWORD[ioDMToolHdr.cPChannels];
			// subtract 1 from cPChannels, because 1 element is actually stored
			// in the ioDMToolHdr array.
			cbSize = (ioDMToolHdr.cPChannels - 1) * sizeof(DWORD);
			if(pdwPChannels)
			{
				pdwPChannels[0] = ioDMToolHdr.dwPChannels[0];
				if( cbSize )
				{
					hr = pParser->Read(&pdwPChannels[1], cbSize);
					if(FAILED(hr))
					{
						delete [] pdwPChannels;
						pdwPChannels = NULL;
                        pParser->LeaveList();
                        Trace(1,"Error: File read error loading Tool.\n");
						return DMUS_E_CANNOTREAD;
					}
				}
			}
			else
			{
				hr = E_OUTOFMEMORY;
			}
		}
	}
	else
	{
        pParser->LeaveList();
        Trace(1,"Error reading Tool chunk - not RIFF format.\n");
		hr = DMUS_E_DESCEND_CHUNK_FAIL;
	}
    while (pParser->NextChunk(&hr))
    {
        if((((ckNext.ckid == FOURCC_LIST) || (ckNext.ckid == FOURCC_RIFF))
		    && ckNext.fccType == ioDMToolHdr.fccType) ||
            (ckNext.ckid == ioDMToolHdr.ckid))
		{
            pParser->SeekBack();
			hr = CreateTool(ioDMToolHdr, pParser->GetStream(), pdwPChannels);
            pParser->SeekForward();
		} 
	}

    pParser->LeaveList();

    if( pdwPChannels )
    {
        delete [] pdwPChannels;
        pdwPChannels = NULL;
    }

	return hr;
}

HRESULT CCollection::CreateTool(DMUS_IO_TOOL_HEADER ioDMToolHdr, IStream *pStream, DWORD *pdwPChannels)
{
	assert(pStream);

	IDirectMusicTool* pDMTool = NULL;
	HRESULT hr = DirectMusicCreateInstance(ioDMToolHdr.guidClassID,
								  NULL,
								  IID_IDirectMusicTool,
								  (void**)&pDMTool);
	IPersistStream *pIPersistStream = NULL;
	
	if(SUCCEEDED(hr))
	{
		hr = pDMTool->QueryInterface(IID_IPersistStream, (void **)&pIPersistStream);
	}
    else
    {
        Trace(1,"Error creating tool for loading\n");
    }

	if(SUCCEEDED(hr))
	{
		hr = pIPersistStream->Load(pStream);
#ifdef DBG
        if (FAILED(hr))
        {
            Trace(1,"Error loading data into tool\n");
        }
#endif
	}

	if(SUCCEEDED(hr))
	{
		hr = InsertTool(pDMTool, pdwPChannels, ioDMToolHdr.cPChannels, ioDMToolHdr.lIndex, &ioDMToolHdr.guidClassID);  
	}

	if(pIPersistStream)
	{
		pIPersistStream->Release();
	}
	
	if(pDMTool)
	{
		pDMTool->Release();
	}

	return hr;
}

HRESULT CCollection::Save( IStream* pIStream, BOOL fClearDirty )
{
	return E_NOTIMPL;
}

HRESULT CCollection::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// IDirectMusicObject

STDMETHODIMP CCollection::GetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
	// Argument validation
	V_INAME(CCollection::GetDescriptor);
	V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC);
    return m_Info.GetDescriptor(pDesc,CLSID_DirectMusiCCollection);
}

STDMETHODIMP CCollection::SetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
	// Argument validation
	V_INAME(CCollection::SetDescriptor);
	V_STRUCTPTR_READ(pDesc, DMUS_OBJECTDESC);
	return m_Info.SetDescriptor(pDesc);
}

STDMETHODIMP CCollection::ParseDescriptor(LPSTREAM pIStream, LPDMUS_OBJECTDESC pDesc) 
{
    V_INAME(CCollection::ParseDescriptor);
    V_INTERFACE(pIStream);
    V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC);
    return m_Info.ParseDescriptor(pIStream,pDesc,DMUS_FOURCC_TOOLGRAPH_FORM,CLSID_DirectMusiCCollection);
}