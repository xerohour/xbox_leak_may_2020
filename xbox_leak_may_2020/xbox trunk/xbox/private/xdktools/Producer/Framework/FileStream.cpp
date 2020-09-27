//////////////////////////////////////////////////////////////////////////////
// CFileStream

/*-----------
@doc DMUSPROD
-----------*/

#include "stdafx.h"
#include "JazzApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*======================================================================================
STRUCT:  DMUSPRODSTREAMINFO
========================================================================================
@struct DMUSProdStreamInfo | Contains additional information about a stream created by
	calling either <om IDMUSProdFramework::AllocFileStream> or <om IDMUSProdFramework::AllocMemoryStream>.
	
@field FileType | ftFileType | Type of stream. FT_DESIGN, FT_RUNTIME, or FT_UNKNOWN.
@field GUID | guidDataFormat | GUID identifying data format of stream.
@field IDMUSProdNode | pITargetDirectoryNode | <i IDMUSProdNode> interface pointer to the
		Project Tree Directory <o Node> that will contain this file.
--------------------------------------------------------------------------------------*/


/*======================================================================================
INTERFACE:  IDMUSPRODPERSISTINFO
========================================================================================
@interface IDMUSProdPersistInfo | 
	This interface provides access to additional information for consideration when
	persisting objects through an IStream interface.  
	
	<i IDMUSProdFramework> provides two methods which return pointers to an IStream interface: 
	<om IDMUSProdFramework.AllocFileStream> and <om IDMUSProdFramework.AllocMemoryStream>.
	Streams returned by these methods also implement the <i IDMUSProdPersistInfo>
	interface for the purpose of making available additional information about the stream.
	In particular, these methods associate a file type and data format with the streams they
	create.  In addition, <om IDMUSProdFramework.AllocFileStream> associates a target directory
	node with each stream having a <p dwDesiredAccess> of GENERIC_READ.
  
	An object can obtain a pointer to a stream's <i IDMUSProdPersistInfo> interface via a call to
	<om IStream::QueryInterface>.  <om IDMUSProdPersistInfo.GetStreamInfo> returns the <p ftFileType>,
	<p guidDataFormat>, and <p pITargetDirectoryNode> associated with a stream.  <p pITargetDirectory>
	will always be NULL for streams created via <om IDMUSProdFramework.AllocMemoryStream>.

@meth HRESULT | GetFileName | Returns the path/filename of the file associated with 
	this stream.
@meth HRESULT | GetStreamInfo | Fills a DMUSProdStreamInfo structure with information about
	this stream.
@meth HRESULT | IsInEmbeddedFileList | Determines whether <p pIDocRootNode> is already embedded
	in this stream.
@meth HRESULT | AddToEmbeddedFileList | Adds <p pIDocRootNode> to the list of files embedded
	in this stream.
	
@base public | IUnknown

@xref <om IDMUSProdFramework.AllocFileStream>, <om IDMUSProdFramework.AllocMemoryStream>
-------------------------------------------------------------------------- */


//////////////////////////////////////////////////////////////////////////////
// CFileStream class

struct CFileStream : IStream, public IDMUSProdPersistInfo,
							  public IDirectMusicGetLoader
{
///// object state
    ULONG           m_dwRef;				// object reference count
    HANDLE          m_hFile;				// file handle
	IDMUSProdNode*	m_pITargetDirectoryNode;// IDMUSProdNode interface pointer to the Project Tree
											//		Directory node that will contain this file
	CString			m_strFileName;			// file name
	FileType		m_ftFileType;			// Runtime or design-time
	GUID			m_guidDataFormat;		// specific format of data in stream
											// current file format, older file format, strip, etc

	CTypedPtrList<CPtrList, IDMUSProdNode*> m_lstEmbeddedFiles;

// construction and destruction
    CFileStream( HANDLE hFile, LPCTSTR szFileName, FileType ftFileType, GUID guidDataFormat,
				 IDMUSProdNode* pITargetDirectoryNode )
	{
		m_dwRef = 1;
		m_hFile = hFile;
		m_strFileName = szFileName;
		m_ftFileType = ftFileType;
		memcpy( &m_guidDataFormat, &guidDataFormat, sizeof(guidDataFormat) );
		m_pITargetDirectoryNode = pITargetDirectoryNode;
	}
    ~CFileStream()
	{
		// Release all items in m_lstEmbeddedFiles
		while( !m_lstEmbeddedFiles.IsEmpty() )
		{
			IDMUSProdNode* pIDocRootNode = m_lstEmbeddedFiles.RemoveHead();
			pIDocRootNode->Release();
		}

		CloseHandle( m_hFile );
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
		ASSERT( cb != 0 );
		DWORD dw;

		int nReturn = ReadFile( m_hFile, pv, cb, &dw, NULL );
		if( pcbRead )
		{
			*pcbRead = dw;
		}
		if( nReturn
		&&  dw == cb )
		{
			return S_OK;
		}
		return E_FAIL;
    }
    STDMETHODIMP Write( const void* pv, ULONG cb, ULONG* pcbWritten )
    {
		DWORD dw;

		if( WriteFile( m_hFile, pv, cb, &dw, NULL ) &&
			dw == cb )
		{
			if( pcbWritten != NULL )
			{
				*pcbWritten = dw;
			}
			return S_OK;
		}
        return E_FAIL;
	}
    STDMETHODIMP Seek( LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition )
    {
		DWORD dw;

		dw = SetFilePointer( m_hFile, dlibMove.LowPart, &dlibMove.HighPart, dwOrigin );
		if( dw == (DWORD)-1 )
		{
			return E_FAIL;
		}
		if( plibNewPosition != NULL )
		{
			plibNewPosition->LowPart = dw;
	        plibNewPosition->HighPart = dlibMove.HighPart;
		}
        return S_OK;
	}
    STDMETHODIMP SetSize(ULARGE_INTEGER /*libNewSize*/)
      { return E_NOTIMPL; }
	STDMETHODIMP CopyTo(IStream* pstm, ULARGE_INTEGER cb,
		ULARGE_INTEGER* pcbRead,
		ULARGE_INTEGER* pcbWritten)
	{
		// Validate pstm
		if ( pstm == NULL )
		{
			return E_INVALIDARG;
		}

		// Can only read and write an array of maximum size of a DWORD.
		if ( cb.HighPart != 0 )
		{
			return E_INVALIDARG;
		}

		// Pass on the target directory
		IDMUSProdSetPersistInfo* pISetPersistInfo;
		if( SUCCEEDED ( pstm->QueryInterface( IID_IDMUSProdSetPersistInfo, (void **)&pISetPersistInfo ) ) )
		{
			pISetPersistInfo->SetTargetDirectory( m_pITargetDirectoryNode );
			pISetPersistInfo->Release();
		}

		// Check if the user requested to copy zero bytes
		if ( cb.LowPart == 0 )
		{
			if( pcbRead )
			{
				pcbRead->QuadPart = 0;
			}
			if( pcbWritten )
			{
				pcbWritten->QuadPart = 0;
			}

			// Nothing more to do
			return S_OK;
		}

		// Initialize the array to NULL
		BYTE *pbaTemp = NULL;

		TRY
		{
			// Try and allocate a temporary array to store the data in
			pbaTemp = new BYTE[cb.LowPart];
		}
		CATCH(CException, e)
		{
			// Do nothing - handled by the if() statement below
		}
		END_CATCH

		// If allocation failes, return E_OUTOFMEMORY
		if ( pbaTemp == NULL )
		{
			return E_OUTOFMEMORY;
		}

		// Now, actually read in the data
		HRESULT hr;
		ULONG cbRead, cbWritten;
		hr = Read( pbaTemp, cb.LowPart, &cbRead );

		// Pass on how much data was read
		if ( pcbRead != NULL )
		{
			pcbRead->QuadPart = cbRead;
		}

		// Handle any error in the read
		if ( FAILED( hr ) )
		{
			delete []pbaTemp;
			return hr;
		}

		// Now, write out the data
		hr = pstm->Write( pbaTemp, cbRead, &cbWritten );

		// Delete the temporary array
		delete []pbaTemp;

		// Pass on how much data was written
		if ( pcbWritten != NULL )
		{
			pcbWritten->QuadPart = cbWritten;
		}

		// Return the result from the Write() call
		return hr;
	}
    STDMETHODIMP Commit(DWORD /*grfCommitFlags*/)
      { return E_NOTIMPL; }
    STDMETHODIMP Revert()
      { return E_NOTIMPL; }
    STDMETHODIMP LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/,
        DWORD /*dwLockType*/)
      { return E_NOTIMPL; }
    STDMETHODIMP UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/,
        DWORD /*dwLockType*/)
      { return E_NOTIMPL; }
    STDMETHODIMP Stat(STATSTG* /*pstatstg*/, DWORD /*grfStatFlag*/)
      { return E_NOTIMPL; }
    STDMETHODIMP Clone(IStream** /*ppstm*/)
      { return E_NOTIMPL; }


/*======================================================================================
METHOD:  IDMUSPRODPERSISTINFO::GETFILENAME
========================================================================================
@method HRESULT | IDMUSProdPersistInfo | GetFileName | Returns the path/filename of the file
		associated with this stream. 

@rvalue S_OK | The filename was returned in <p pbstrFileName>.
@rvalue E_POINTER | The address in <p pbstrFileName> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the filename was not returned.

@xref <i IDMUSProdPersistInfo>
--------------------------------------------------------------------------------------*/
    STDMETHOD(GetFileName)(
		BSTR* pbstrFileName		// @parm [out,retval]  Pointer to the caller-allocated variable
								// that receives the copy of the filename.  The caller must
								// free <p pbstrFileName> with SysFreeString when it is no longer
								// needed.
	)
	{
		if( pbstrFileName == NULL )
		{
			return E_POINTER;
		}

		*pbstrFileName = m_strFileName.AllocSysString();
		return S_OK;
	}

/*======================================================================================
METHOD:  IDMUSPRODPERSISTINFO::GETSTREAMINFO
========================================================================================
@method HRESULT | IDMUSProdPersistInfo | GetStreamInfo | Fills a <t DMUSProdStreamInfo>
		structure with information about this stream.

@comm
	Streams created through either <om IDMUSProdFramework.AllocFileStream> or
	<om IDMUSProdFramework.AllocMemoryStream> carry information communicating their file type
	and data format.  <om IDMUSProdPersistInfo.GetStreamInfo> provides the means for an object
	to obtain this information so that it can determine how to persist itself.

	The following file types may be returned in DMUSProdStreamInfo.ftFileType:

	FT_DESIGN: <tab>Design-time saves may include chunks of UI related information only used
	during editing.
	
	FT_RUNTIME: <tab>Runtime saves are invoked to create files for distribution.  Data should
	be saved in its most compact form.    

	FT_UNKNOWN: <tab>FT_UNKNOWN is only valid when <om IDMUSProdFramework::AllocFileStream>
	is called to open a file.
	
	DMUSProdStreamInfo.guidDataFormat indicates the specific format to be used when writing data
	into the stream.  Objects may create additional GUIDs to communicate specific data formats they
	need when persisting data.  The <o Framework> supplies the following GUIDs for general use:
	
	GUID_CurrentVersion: <tab><tab>Current version of the file format.

	GUID_CurrentVersion_OnlyUI: <tab>Current version of the file format (UI state information only).

	GUID_DirectMusicObject: <tab><tab>Stream being prepared to persist into a DirectMusic object.

	GUID_Bookmark: <tab><tab><tab>Framework uses this GUID when creating streams to include in bookmarks.

	GUID_AllZeros: <tab><tab><tab>GUID_AllZeros is only valid when <om IDMUSProdFramework::AllocFileStream>
	is called to open a file.

@rvalue S_OK | The stream information was returned successfully.
@rvalue E_POINTER | The address in <p pStreamInfo> is not valid. For example, it may be NULL.	

@xref <i IDMUSProdPersistInfo>, <om IDMUSProdFramework.AllocFileStream>, <om IDMUSProdFramework.AllocMemoryStream>, <i IDMUSProdDocType>, <om IDMUSProdDocType.IsFileTypeExtension>
--------------------------------------------------------------------------------------*/
    STDMETHOD(GetStreamInfo)(
		DMUSProdStreamInfo* pStreamInfo	// @parm [in] Pointer to a <t DMUSProdStreamInfo> structure.
	)
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

/*======================================================================================
METHOD:  IDMUSPRODPERSISTINFO::ISINEMBEDDEDFILELIST
========================================================================================
@method HRESULT | IDMUSProdPersistInfo | IsInEmbeddedFileList | Determines whether
	<p pIDocRootNode> is already embedded in this stream. 

@rvalue S_OK | <p pIDocRootNode> is embedded in this stream.
@rvalue S_FALSE | <p pIDocRootNode> is not embedded in this stream.
@rvalue E_INVALIDARG | The address in <p pIDocRootNode> is not valid.  For example, it may be NULL.

@xref <i IDMUSProdPersistInfo>
--------------------------------------------------------------------------------------*/
    STDMETHOD(IsInEmbeddedFileList)(
		IDMUSProdNode* pIDocRootNode	// @parm [out,retval]  [in] Pointer to the DocRoot's
									//			<i IDMUSProdNode> interface.
	)
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

/*======================================================================================
METHOD:  IDMUSPRODPERSISTINFO::ADDTOEMBEDDEDFILELIST
========================================================================================
@method HRESULT | IDMUSProdPersistInfo | AddToEmbeddedFileList | Adds <p pIDocRootNode>
	to the list of files embedded in this stream. 

@rvalue S_OK | <p pIDocRootNode> was added to the list of files embedded in this stream.
@rvalue E_INVALIDARG | The address in <p pIDocRootNode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and <p pIDocRootNode> was not added to the list of files
		embedded in this stream.

@xref <i IDMUSProdPersistInfo>
--------------------------------------------------------------------------------------*/
    STDMETHOD(AddToEmbeddedFileList)(
		IDMUSProdNode* pIDocRootNode	// @parm [out,retval]  [in] Pointer to the DocRoot's
									//			<i IDMUSProdNode> interface.
	)
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
};


//////////////////////////////////////////////////////////////////////////////
// AllocFileStream

HRESULT AllocFileStream( LPCTSTR szFileName, DWORD dwDesiredAccess,
						 FileType ftFileType, GUID guidDataFormat,
						 IDMUSProdNode* pITargetDirectoryNode,
						 IStream **ppIStream )
{
    HANDLE       hFile;          // handle to open file
    CFileStream* pFileStream;    // IStream implementation

    // in case of error...
    *ppIStream = NULL;

    // open the file
    if( dwDesiredAccess == GENERIC_READ )
    {
        hFile = CreateFile( szFileName, dwDesiredAccess, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    }
    else if( dwDesiredAccess == GENERIC_WRITE )
    {
		// Must specify type of stream
		ASSERT( ftFileType != FT_UNKNOWN );
		if( ftFileType == FT_UNKNOWN )
		{
			return E_INVALIDARG;
		}

		// Target Directory node must be NULL
		pITargetDirectoryNode = NULL;

        hFile = CreateFile( szFileName, dwDesiredAccess, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    }
    else
    {
        return E_INVALIDARG;
    }

    if( hFile == INVALID_HANDLE_VALUE )
	{
		CString strFileName = szFileName;

		int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strFileName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
		}

		if( dwDesiredAccess == GENERIC_READ )
		{
			theApp.DisplaySystemError( IDS_SYSERR_OPEN, strFileName  );
		}
		else if( dwDesiredAccess == GENERIC_WRITE )
		{
			theApp.DisplaySystemError( IDS_SYSERR_SAVE, strFileName  );
		}

        return E_FAIL;
	}

    // create the Windows object
    pFileStream = new CFileStream( hFile, szFileName, ftFileType, guidDataFormat, pITargetDirectoryNode );
    if( pFileStream == NULL )
	{
		CloseHandle( hFile ); 
        return E_OUTOFMEMORY;
	}

    // return an IStream pointer
    *ppIStream = (IStream *)pFileStream;

    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// AllocFileReadWriteStream

HRESULT AllocFileReadWriteStream( LPCTSTR szFileName, IStream **ppIStream )
{
    HANDLE       hFile;          // handle to open file
    CFileStream* pFileStream;    // IStream implementation

    // in case of error...
    *ppIStream = NULL;

    hFile = CreateFile( szFileName, (GENERIC_READ|GENERIC_WRITE), 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if( hFile == INVALID_HANDLE_VALUE )
	{
        return E_FAIL;
	}

    // create the Windows object
    pFileStream = new CFileStream( hFile, szFileName, FT_UNKNOWN, GUID_AllZeros, NULL );
    if( pFileStream == NULL )
	{
		CloseHandle( hFile ); 
        return E_OUTOFMEMORY;
	}

    // return an IStream pointer
    *ppIStream = (IStream *)pFileStream;

    return S_OK;
}
