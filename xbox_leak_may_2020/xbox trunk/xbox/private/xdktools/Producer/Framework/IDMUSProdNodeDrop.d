
---------------------------------------------------
IDMUSProdNodeDrop AutoDocs for DirectMusic Producer
---------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODNODEDROP
========================================================================================
@interface IDMUSProdNodeDrop | 
	The <i IDMUSProdNodeDrop> interface allows files dragged from Explorer to be dropped
	on a <o Node> in the Project Tree.

@base public | IUnknown

@meth HRESULT | OnDropFiles | Node's handler for WM_DROPFILES message.

@xref <o Node> Object, <o Project> Object
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODNODEDROP::ONDROPFILES
========================================================================================
@method HRESULT | IDMUSProdNodeDrop | OnDropFiles | Allows files dragged from Explorer
	to be dropped on a <o Node> in the Project Tree.
 
@comm
	<i IDMUSProdNodeDrop> should be implemented by Node objects that want to provide a
	handler for the WM_DROPFILES message.

@parm HANDLE | hDropInfo | [in] Handle to an internal structure describing the dropped
								files.

@rvalue S_OK | The files were dropped successfully.
@rvalue E_FAIL | An error occurred and one or more files could not be dropped.

@ex The following example allows *.wav files to be dragged from Explorer and
	dropped on a Wave folder in a DLS Collection: |

HRESULT CCollectionWaves::OnDropFiles( HANDLE handleDropInfo )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

	HDROP hDropInfo = (HDROP)handleDropInfo;

	UINT nFiles = ::DragQueryFile( hDropInfo, (UINT)-1, NULL, 0 );

	for( UINT iFile = 0 ;  iFile < nFiles ;  iFile++ )
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile( hDropInfo, iFile, szFileName, _MAX_PATH );

		if( FAILED ( InsertWaveFile( szFileName ) ) )
		{
			CString strErrMsg;
			strErrMsg.Format( IDS_ERR_FAILED_WAVE_INSERT, szFileName );
			AfxMessageBox(strErrMsg);
		}
	}

	::DragFinish( hDropInfo );
	return S_OK;
}
	
@xref <i IDMUSProdNodeDrop>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/
