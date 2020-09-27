// Implementation of CDMOInfoProxy

#include "stdafx.h"
#include "DMOInfoProxy.h"
#include "dmusici.h"
#include <initguid.h>
#define _SYS_GUID_OPERATORS_
#include "dmoreg.h"
#include "uuids.h"


// Constructor/Desctructor
CDMOInfoProxy::CDMOInfoProxy( DWORD dwStandardID )
{
	m_cRef = 0;
	AddRef();

	m_dwType = dwStandardID;
}

CDMOInfoProxy::~CDMOInfoProxy()
{
}

// IUnknown
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::QueryInterface( const IID &iid, void **ppv )
{
    if( ::IsEqualIID( iid, IID_IDMUSProdDMOInfo )  ||
		::IsEqualIID( iid, IID_IUnknown ) )
	{
		*ppv = static_cast<IDMUSProdDMOInfo *>(this);
	}
    else if( ::IsEqualIID( iid, IID_IDMUSProdNode ) )
	{
		*ppv = static_cast<IDMUSProdNode *>(this);
	}
	else 
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	static_cast<IUnknown *>(*ppv)->AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE CDMOInfoProxy::AddRef()
{
	return InterlockedIncrement( &m_cRef );
}

ULONG STDMETHODCALLTYPE CDMOInfoProxy::Release()
{
	if( InterlockedDecrement( &m_cRef ) == 0 )
	{
		delete this;
		return 0;
	}
	return m_cRef;
}

void GetDMOName( CLSID clsidDMO, WCHAR *wcstrName )
{
	IEnumDMO *pEnumDMO;

	DMO_PARTIAL_MEDIATYPE dmoPMT;
	dmoPMT.type = MEDIATYPE_Audio;
	dmoPMT.subtype = MEDIASUBTYPE_PCM;

	if( SUCCEEDED( DMOEnum( DMOCATEGORY_AUDIO_EFFECT, 0, 1, &dmoPMT, 1, &dmoPMT, &pEnumDMO ) ) )
	{
		pEnumDMO->Reset();
		CLSID clsidItem;
		WCHAR *pwcName;
		DWORD dwItemsFetched;
		while( S_OK == pEnumDMO->Next( 1, &clsidItem, &pwcName, &dwItemsFetched ) )
		{
			if( clsidItem == clsidDMO )
			{
				wcsncpy( wcstrName, pwcName, 64 );
				::CoTaskMemFree( pwcName );
				break;
			}

			::CoTaskMemFree( pwcName );
		}
		pEnumDMO->Release();
	}
}

// IDMUSProdDMOInfo method
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::EnumDMOInfo(
	DWORD dwIndex, DMUSProdDMOInfo *pDMOInfo ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Validate arguments
	if( pDMOInfo == NULL
	||	pDMOInfo->dwSize != sizeof( DMUSProdDMOInfo ) )
	{
		return E_POINTER;
	}

	ZeroMemory( pDMOInfo->awchPChannelText, sizeof(WCHAR) * 65 );
	ZeroMemory( pDMOInfo->awchMixGroupName, sizeof(WCHAR) * 65 );
	ZeroMemory( pDMOInfo->awchBufferText, sizeof(WCHAR) * 65 );
	ZeroMemory( pDMOInfo->awchDMOName, sizeof(WCHAR) * 65 );

	switch( m_dwType )
	{
	case DMUS_APATH_SHARED_STEREOPLUSREVERB:
		switch( dwIndex )
		{
		case 0:
			pDMOInfo->clsidDMO = GUID_DSFX_WAVES_REVERB;
			pDMOInfo->dwStage = DMUS_PATH_BUFFER_DMO;
			pDMOInfo->dwPChannel = 0;
			pDMOInfo->dwBufferIndex = 1;
			pDMOInfo->dwEffectIndex = 0;
			//pDMOInfo->awchPChannelText = ;
			//pDMOInfo->awchMixGroupName = ;
			//pDMOInfo->awchBufferText = ;
			GetDMOName( GUID_DSFX_WAVES_REVERB, pDMOInfo->awchDMOName );
			pDMOInfo->guidDesignGUID = GUID_DSFX_WAVES_REVERB;
			return S_OK;
			break;
		}
		break;
		/*
	case DMUS_APATH_DYNAMIC_ENV3D:
		switch( dwIndex )
		{
		case 0:
			pDMOInfo->clsidDMO = GUID_DSFX_STANDARD_I3DL2SOURCE;
			pDMOInfo->dwStage = DMUS_PATH_BUFFER_DMO;
			pDMOInfo->dwPChannel = 0;
			pDMOInfo->dwBufferIndex = 0;
			pDMOInfo->dwEffectIndex = 0;
			//pDMOInfo->awchPChannelText = ;
			//pDMOInfo->awchMixGroupName = ;
			//pDMOInfo->awchBufferText = ;
			GetDMOName( GUID_DSFX_STANDARD_I3DL2SOURCE, pDMOInfo->awchDMOName );
			pDMOInfo->guidDesignGUID = GUID_DSFX_STANDARD_I3DL2SOURCE;
			return S_OK;
		case 1:
			pDMOInfo->clsidDMO = GUID_DSFX_STANDARD_I3DL2REVERB;
			pDMOInfo->dwStage = DMUS_PATH_MIXIN_BUFFER_DMO;
			pDMOInfo->dwPChannel = 0;
			pDMOInfo->dwBufferIndex = 0;
			pDMOInfo->dwEffectIndex = 0;
			//pDMOInfo->awchPChannelText = ;
			//pDMOInfo->awchMixGroupName = ;
			//pDMOInfo->awchBufferText = ;
			GetDMOName( GUID_DSFX_STANDARD_I3DL2REVERB, pDMOInfo->awchDMOName );
			pDMOInfo->guidDesignGUID = GUID_DSFX_STANDARD_I3DL2REVERB;
			return S_OK;
		}
		break;
		*/
	}

	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetNodeImageIndex( short* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetFirstChild( IDMUSProdNode** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetNextChild( IDMUSProdNode*, IDMUSProdNode** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetComponent( IDMUSProdComponent** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetDocRootNode( IDMUSProdNode** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::SetDocRootNode( IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetParentNode( IDMUSProdNode** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::SetParentNode( IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetNodeId( GUID* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetNodeName( BSTR* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetNodeNameMaxLength( short* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::ValidateNodeName( BSTR )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::SetNodeName( BSTR )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetNodeListInfo( DMUSProdListInfo* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetEditorClsId( CLSID* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetEditorTitle( BSTR* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetEditorWindow( HWND* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::SetEditorWindow( HWND )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::UseOpenCloseImages( BOOL* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetRightClickMenuId( HINSTANCE*, UINT* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::OnRightClickMenuInit( HMENU )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::OnRightClickMenuSelect( long )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::DeleteChildNode( IDMUSProdNode*, BOOL )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::InsertChildNode( IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::DeleteNode( BOOL )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::OnNodeSelChanged( BOOL )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::CreateDataObject( IDataObject** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::CanCut()
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::CanCopy()
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::CanDelete()
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::CanDeleteChildNode( IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::CanPasteFromData( IDataObject*, BOOL* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::PasteFromData( IDataObject* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::CanChildPasteFromData( IDataObject*, IDMUSProdNode*, BOOL* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::ChildPasteFromData( IDataObject*, IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CDMOInfoProxy::GetObject( REFCLSID, REFIID, void** ) 
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
