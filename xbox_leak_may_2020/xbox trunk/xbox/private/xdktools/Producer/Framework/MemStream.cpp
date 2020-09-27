//////////////////////////////////////////////////////////////////////////////
// CMemStream

#include "stdafx.h"
#include "JazzApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////////
// CMemStream class

struct CMemStream : IStream, public IDMUSProdPersistInfo,
							 public IDMUSProdSetPersistInfo,
							 public IDirectMusicGetLoader
{
///// object state
    ULONG           m_dwRef;				// object reference count
	IStream*		m_pIStream;				// IStream* from CreateStreamOnHGlobal
	FileType		m_ftFileType;			// runtime or design-time
	GUID			m_guidDataFormat;		// specific format of stream
											//		current file format, older file format, strip, etc
	IDMUSProdNode*	m_pITargetDirectoryNode;// IDMUSProdNode interface pointer to the Project Tree
											//		Directory node that will contain this file

	CTypedPtrList<CPtrList, IDMUSProdNode*> m_lstEmbeddedFiles;

// construction and destruction
    CMemStream( IStream* pIStream, FileType ftFileType, GUID guidDataFormat,
				IDMUSProdNode* pITargetDirectoryNode )
	{
		ASSERT( pIStream != NULL );

		m_dwRef = 1;
		m_pIStream = pIStream;
		m_ftFileType = ftFileType;
		memcpy( &m_guidDataFormat, &guidDataFormat, sizeof(guidDataFormat) );
		m_pITargetDirectoryNode = pITargetDirectoryNode;
	}
    ~CMemStream()
	{
		// Release all items in m_lstEmbeddedFiles
		while( !m_lstEmbeddedFiles.IsEmpty() )
		{
			IDMUSProdNode* pIDocRootNode = m_lstEmbeddedFiles.RemoveHead();
			pIDocRootNode->Release();
		}

		m_pIStream->Release();
	}

/////  IUnknown methods
    STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
    {
        if( IsEqualIID( riid, IID_IUnknown )
        ||  IsEqualIID( riid, IID_IStream ) )
        {
            *ppvObj = (IStream *)this;
            AddRef();
            return NOERROR;
        }
        if( IsEqualIID( riid, IID_IDMUSProdPersistInfo ) )
        {
            *ppvObj = (IDMUSProdPersistInfo *)this;
            AddRef();
            return NOERROR;
        }
        if( IsEqualIID( riid, IID_IDMUSProdSetPersistInfo ) )
        {
            *ppvObj = (IDMUSProdSetPersistInfo *)this;
            AddRef();
            return NOERROR;
        }
        if( IsEqualIID( riid, IID_IDirectMusicGetLoader ) )
        {
            *ppvObj = (IDirectMusicGetLoader *)this;
            AddRef();
            return NOERROR;
        }
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef()
    {
        return ++m_dwRef;
    }
    STDMETHODIMP_(ULONG) Release()
    {
        if( --m_dwRef == 0L )
        {
            delete this;
            return 0;
        }
        return m_dwRef;
    }

/////  IStream methods
    STDMETHODIMP Read( void* pv, ULONG cb, ULONG* pcbRead )
	{
		HRESULT hr = m_pIStream->Read( pv, cb, pcbRead );
		if( pcbRead
		&& *pcbRead != cb )
		{
			hr = E_FAIL;
		}
		return hr;
	}
    STDMETHODIMP Write( const void* pv, ULONG cb, ULONG* pcbWritten )
	{
		return m_pIStream->Write( pv, cb, pcbWritten );
	}
    STDMETHODIMP Seek( LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition )
	{
		return m_pIStream->Seek( dlibMove, dwOrigin, plibNewPosition );
	}
    STDMETHODIMP SetSize(ULARGE_INTEGER libNewSize)
	{
		return m_pIStream->SetSize( libNewSize );
	}
    STDMETHODIMP CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
	{
		IDMUSProdSetPersistInfo* pISetPersistInfo;

		if( SUCCEEDED ( pstm->QueryInterface( IID_IDMUSProdSetPersistInfo, (void **)&pISetPersistInfo ) ) )
		{
			pISetPersistInfo->SetTargetDirectory( m_pITargetDirectoryNode );
			pISetPersistInfo->Release();
		}
		return m_pIStream->CopyTo( pstm, cb, pcbRead, pcbWritten );
	}
    STDMETHODIMP Commit(DWORD grfCommitFlags)
	{
		return m_pIStream->Commit( grfCommitFlags );
	}
    STDMETHODIMP Revert()
	{
		return m_pIStream->Revert();
	}
    STDMETHODIMP LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return m_pIStream->LockRegion( libOffset, cb, dwLockType );
	}
    STDMETHODIMP UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return m_pIStream->UnlockRegion( libOffset, cb, dwLockType );
	}
    STDMETHODIMP Stat(STATSTG* pstatstg, DWORD grfStatFlag)
	{
		return m_pIStream->Stat( pstatstg, grfStatFlag );
	}
    STDMETHODIMP Clone(IStream** ppstm)
	{
		if( ppstm == NULL )
		{
			return E_INVALIDARG;
		}

	   // clone the stream
		IStream* pIStreamClone;
		if( SUCCEEDED ( m_pIStream->Clone( &pIStreamClone ) ) )
		{
			CMemStream *pMemStream;
			pMemStream = new CMemStream( pIStreamClone, m_ftFileType,
										 m_guidDataFormat, m_pITargetDirectoryNode );
			if( pMemStream == NULL )
			{
				pIStreamClone->Release();
				return E_OUTOFMEMORY;
			}

			// return an IStream pointer
			*ppstm = (IStream *)pMemStream;
			return S_OK;
		}
		return E_FAIL;
	}

/////  IDMUSProdPersistInfo methods
    STDMETHOD(GetFileName)( BSTR* pbstrFileName	)
	{
		return E_NOTIMPL;
	}
    STDMETHOD(GetStreamInfo)( DMUSProdStreamInfo* pStreamInfo )
	{
		if( pStreamInfo == NULL )
		{
			return E_POINTER;
		}

		pStreamInfo->pITargetDirectoryNode = m_pITargetDirectoryNode;
		pStreamInfo->ftFileType = m_ftFileType;
		memcpy( &pStreamInfo->guidDataFormat, &m_guidDataFormat, sizeof(m_guidDataFormat) );
		return S_OK;
	}
    STDMETHOD(IsInEmbeddedFileList)( IDMUSProdNode* pIDocRootNode )
	{
		if( pIDocRootNode == NULL )
		{
			return E_INVALIDARG;
		}

		POSITION pos = m_lstEmbeddedFiles.GetHeadPosition();
		while( pos )
		{
			IDMUSProdNode* pIDocRootNodeList = m_lstEmbeddedFiles.GetNext( pos );

			if( pIDocRootNodeList == pIDocRootNode )
			{
				return S_OK;
			}
		}

		return S_FALSE;
	}
    STDMETHOD(AddToEmbeddedFileList)( IDMUSProdNode* pIDocRootNode )
	{
		if( pIDocRootNode == NULL )
		{
			return E_INVALIDARG;
		}

		if( IsInEmbeddedFileList(pIDocRootNode) == S_FALSE )
		{
			pIDocRootNode->AddRef();
			m_lstEmbeddedFiles.AddHead( pIDocRootNode );
		}

		return S_OK;
	}

/////  IDMUSProdSetPersistInfo methods
    STDMETHOD(SetTargetDirectory)( IDMUSProdNode* pITargetDirectoryNode )
	{
		m_pITargetDirectoryNode = pITargetDirectoryNode;
		return S_OK;
	}

/////  IDirectMusicGetLoader methods
	STDMETHOD( GetLoader )( IDirectMusicLoader ** ppLoader )
	{
		if( ppLoader == NULL )
		{
			return E_INVALIDARG;
		}

		if( theApp.m_pLoader == NULL )
		{
			theApp.m_pLoader = new CLoader;
		}

		if( theApp.m_pLoader )
		{
			*ppLoader = (IDirectMusicLoader*)theApp.m_pLoader;
			(*ppLoader)->AddRef();

			return S_OK;
		}

		return E_FAIL;
	}
};


//////////////////////////////////////////////////////////////////////////////
// AllocMemoryStream

HRESULT AllocMemoryStream( FileType ftFileType, GUID guidDataFormat, IStream **ppIStream )
{
    IStream*    pIStream;      // IStream* from CreateStreamOnHGlobal
    CMemStream* pMemStream;    // IStream wrapper

    // in case of error...
    *ppIStream = NULL;

	// Must specify type of stream
	ASSERT( ftFileType != FT_UNKNOWN );
	if( ftFileType == FT_UNKNOWN )
	{
		return E_INVALIDARG;
	}

    // create the stream
	if( SUCCEEDED ( CreateStreamOnHGlobal( NULL, TRUE, &pIStream ) ) )
	{
		pMemStream = new CMemStream( pIStream, ftFileType, guidDataFormat, NULL );
		if( pMemStream == NULL )
		{
			pIStream->Release();
			return E_OUTOFMEMORY;
		}

		// return an IStream pointer
		*ppIStream = (IStream *)pMemStream;
		return S_OK;
	}

	return E_FAIL;
}
