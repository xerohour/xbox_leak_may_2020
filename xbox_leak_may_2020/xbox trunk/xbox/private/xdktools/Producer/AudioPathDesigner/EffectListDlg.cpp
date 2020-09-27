// EffectListDlg.cpp : implementation file
//

#include "stdafx.h"
#include <windowsx.h>
#include "dsoundp.h"
#include "EffectListDlg.h"
#include "RiffStrm.h"
#include "AudioPath.h"
#include "AudioPathDesignerDLL.h"
#include "EffectPPGMgr.h"
#include "AudioPathCtl.h"
#include "AudioPathDlg.h"
#include "ItemInfo.h"
#define _SYS_GUID_OPERATORS_
#include "dmoreg.h"
#include "uuids.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectListDlg dialog


CEffectListDlg::CEffectListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEffectListDlg::IDD, pParent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//{{AFX_DATA_INIT(CEffectListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pAudioPathCtrl = NULL;
	m_pAudioPath = NULL;
}

CEffectListDlg::~CEffectListDlg()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Remove Effect from clipboard
	theApp.FlushClipboard( this );
}

void CEffectListDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffectListDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffectListDlg, CDialog)
	//{{AFX_MSG_MAP(CEffectListDlg)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_EFFECTS, OnBegindragListEffects)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_EFFECT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_EFFECT_SELECT_ALL, OnEditSelectAll)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_EFFECTS, OnSetfocusListEffects)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectListDlg message handlers

void CEffectListDlg::OnBegindragListEffects(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NMLISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	COleDataSource *poleSourceObj = new COleDataSource;
	if( poleSourceObj )
	{
		IStream *pIStream;
		::CreateStreamOnHGlobal( NULL, TRUE, &pIStream );
		IDMUSProdRIFFStream *pRiffStream = NULL;
		AllocRIFFStream( pIStream, &pRiffStream );

		bool fWrote = false;
		if( !m_listEffects.GetFirstSelectedItemPosition() )
		{
			// Write out the effect that the drag is started on
			EffectInfo *pEffectInfo = (EffectInfo *)m_listEffects.GetItemData( pNMListView->iItem );
			if( pEffectInfo )
			{
				pEffectInfo->Write( pRiffStream );
				fWrote = true;
			}
		}
		else
		{
			// Write out all the selected effects
			POSITION pos = m_listEffects.GetFirstSelectedItemPosition();
			while( pos )
			{
				// Get a pointer to each effect
				int nIndex = m_listEffects.GetNextSelectedItem( pos );
				EffectInfo *pEffectInfo = reinterpret_cast<EffectInfo *>(m_listEffects.GetItemData( nIndex ));
				if( pEffectInfo )
				{
					pEffectInfo->Write( pRiffStream );

					fWrote = true;
				}
			}
		}

		pRiffStream->Release();

		if( fWrote )
		{
			// Seek back to the start
			StreamSeek( pIStream, 0, STREAM_SEEK_SET );

			STGMEDIUM stgMedium;
			stgMedium.tymed = TYMED_ISTREAM;
			stgMedium.pstm = pIStream;
			stgMedium.pstm->AddRef();
			stgMedium.pUnkForRelease = NULL;
			poleSourceObj->CacheData( CDirectMusicAudioPath::m_scfEffect, &stgMedium );

			// Begin Drag operation
			poleSourceObj->DoDragDrop( DROPEFFECT_COPY );
		}

		delete poleSourceObj;
		pIStream->Release();

		*pResult = 1;
		return;
	}

	*pResult = 0;
}

void CEffectListDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pWnd);

	RECT rectWindow;
	m_listEffects.GetWindowRect( &rectWindow );
	if( ::PtInRect( &rectWindow, point ) )
	{
		HMENU hMenu = LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_EFFECTPALETTE_RMENU));
		if( hMenu )
		{
			HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );
			if( hMenuPopup )
			{
				// Enable/disable items as appropriate
				bool fTemp = AnyEffectsSelected();

				// TODO:
				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_COPY, fTemp ?
								MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_SELECT_ALL, ( m_listEffects.GetItemCount() > (fTemp ? 1 : 0) ) ?
								MF_ENABLED : MF_GRAYED );

				// Get the position of the popup menu
				DWORD dwPos = ::GetMessagePos();

				// Display the popup menu
				::TrackPopupMenu( hMenuPopup, TPM_LEFTALIGN | TPM_TOPALIGN, GET_X_LPARAM( dwPos ), GET_Y_LPARAM( dwPos ), 0, m_hWnd, NULL );
			}

			DestroyMenu( hMenu ); // This will destroy the submenu as well.
		}
	}
	else
	{
		//TRACE("Context out of list.\n");
	}
}

BOOL CEffectListDlg::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	m_listEffects.SubclassDlgItem(IDC_LIST_EFFECTS, this);

	CDC *pDC = GetDC();

	CString strTitle;
	CSize size;
	size.cx = 80;

	strTitle.LoadString( IDS_EFFECTLIST_EFFECT_TYPE );
	if( pDC )
	{
		size = pDC->GetTextExtent( strTitle );
	}
	m_listEffects.InsertColumn( 0, strTitle, LVCFMT_LEFT, size.cx, -1 );

	ReleaseDC( pDC );

	RefreshControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEffectListDlg::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnSize(nType, cx, cy);
    if( nType == SIZE_MINIMIZED )
	{
        return;
	}

	// Exit if we are not fully created yet
	if( !::IsWindow( m_listEffects.GetSafeHwnd() ) )
	{
		return;
	}

	RECT rect;
	GetClientRect(&rect);
	m_listEffects.MoveWindow(&rect);
}

BOOL CEffectListDlg::OnEraseBkgnd( CDC* pDC )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

void CEffectListDlg::OnEditCopy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(AnyEffectsSelected());

	// Get a copy of the currently selected effects
	IDataObject *pIDataObject;
	if( SUCCEEDED( CreateDataObject( &pIDataObject ) ) )
	{
		// Send the data to the clipboard
		theApp.PutDataInClipboard( pIDataObject, this );

		// Release our reference to the data object
		pIDataObject->Release();
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CEffectListDlg::OnEditCut() 
{
	ASSERT( 0 );
}

void CEffectListDlg::OnEditDelete() 
{
	ASSERT( 0 );
}

void CEffectListDlg::OnEditInsert() 
{
	ASSERT( 0 );
}

void CEffectListDlg::OnEditPaste() 
{
	ASSERT( 0 );
}

void CEffectListDlg::OnEditSelectAll() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	for( int iIndex = 0; iIndex < m_listEffects.GetItemCount(); iIndex++ )
	{
		m_listEffects.SetItemState( iIndex, LVIS_SELECTED, LVIS_SELECTED );
	}
}

void CEffectListDlg::OnSetfocusListEffects(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pNMHDR);

	if( m_pAudioPathCtrl )
	{
		m_pAudioPathCtrl->SetLastFocus( FOC_EFFECT );
	}
	
	*pResult = 0;
}

int CEffectListDlg::FindEffectInfoIndex( const EffectInfo *pEffectInfo )
{
	const int nNumItems = m_listEffects.GetItemCount();
	for( int i=0; i < nNumItems; i++ )
	{
		// Compare the EffectInfo pointer
		if( pEffectInfo == reinterpret_cast<EffectInfo *>(m_listEffects.GetItemData( i )) )
		{
			return i;
		}
	}

	// Not found - return -1
	return -1;
}

void CEffectListDlg::AddEffectToList( EffectInfo *pEffectInfo )
{
	// Don't add send effects to the effect palette
	if( pEffectInfo->m_clsidObject == GUID_DSFX_SEND )
	{
		return;
	}

	// Don't add non-default effects to the effect palette
	if( pEffectInfo->m_strInstanceName != pEffectInfo->m_strName )
	{
		return;
	}

	// Fill in the first column
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM | LVIF_TEXT;
	lvItem.iItem = 0;
	lvItem.iSubItem = 0;
	lvItem.pszText = pEffectInfo->m_strName.GetBuffer( 0 );
	lvItem.lParam = DWORD(pEffectInfo);

	m_listEffects.InsertItem( &lvItem );
	pEffectInfo->m_strName.ReleaseBuffer( -1 );
}

void CEffectListDlg::RefreshControls()
{
	EmptyEffectList();

#ifndef DMP_XBOX
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
			const CString strName = pwcName;
			EffectInfo *pEffectInfo = new EffectInfo( strName, strName, clsidItem, GUID_NULL );

			// Ensure the effect's name is unique
			//GetUniqueEffectInstanceName( pEffectInfo );

			AddEffectToList( pEffectInfo );

			::CoTaskMemFree( pwcName );
		}
		pEnumDMO->Release();
	}
#endif
}

void CEffectListDlg::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	pCmdUI->Enable( AnyEffectsSelected() ? TRUE : FALSE );
}

void CEffectListDlg::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	pCmdUI->Enable( FALSE );
}

void CEffectListDlg::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	pCmdUI->Enable( FALSE );
}

void CEffectListDlg::OnUpdateEditInsert(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	pCmdUI->Enable( FALSE );
}

void CEffectListDlg::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	pCmdUI->Enable( FALSE );
}

void CEffectListDlg::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	bool fTemp = AnyEffectsSelected();
	pCmdUI->Enable( ( m_listEffects.GetItemCount() > (fTemp ? 1 : 0) ) ? TRUE : FALSE );
}

void CEffectListDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	EmptyEffectList();

	CDialog::OnDestroy();
}

bool CEffectListDlg::AnyEffectsSelected( void ) 
{
	return m_listEffects.GetFirstSelectedItemPosition() ? true : false;
}

void CEffectListDlg::EmptyEffectList( void )
{
	while( m_listEffects.GetItemCount() > 0 )
	{
		EffectInfo *pEffectInfo = reinterpret_cast<EffectInfo *>(m_listEffects.GetItemData( 0 ));
		if( pEffectInfo )
		{
			delete pEffectInfo;
		}
		m_listEffects.DeleteItem( 0 );
	}
}

HRESULT	CEffectListDlg::CreateDataObject( IDataObject** ppIDataObject )
{
	// Validate our pointer to the data object
	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Initialize the data object
	*ppIDataObject = NULL;

	if( AnyEffectsSelected() )
	{
		// Create the CDllJazzDataObject 
		CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
		if( pDataObject == NULL )
		{
			return E_OUTOFMEMORY;
		}

		// Create the stream to save the selected PChannels in
		IStream *pIStream;
		if( FAILED( theApp.m_pAudioPathComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIStream ) ) )
		{
			pDataObject->Release();
			return E_OUTOFMEMORY;
		}

		// Create the RIFF stream to save the selected effect in
		IDMUSProdRIFFStream *pRiffStream = NULL;
		AllocRIFFStream( pIStream, &pRiffStream );

		// Write out all the selected effects
		POSITION pos = m_listEffects.GetFirstSelectedItemPosition();
		while( pos )
		{
			// Get a pointer to each effect
			int nIndex = m_listEffects.GetNextSelectedItem( pos );
			EffectInfo *pEffectInfo = reinterpret_cast<EffectInfo *>(m_listEffects.GetItemData( nIndex ));
			if( pEffectInfo )
			{
				pEffectInfo->Write( pRiffStream );
			}
		}

		// Release the RIFF stream
		pRiffStream->Release();

		// Place effects into CDllJazzDataObject
		pDataObject->AddClipFormat( CDirectMusicAudioPath::m_scfEffect, pIStream );

		// Release the stream
		pIStream->Release();

		// Copy the pointer to the IDataObject
		*ppIDataObject = pDataObject; // Already AddRef'd

		// Return a success code
		return S_OK;
	}

	return E_FAIL;
}
