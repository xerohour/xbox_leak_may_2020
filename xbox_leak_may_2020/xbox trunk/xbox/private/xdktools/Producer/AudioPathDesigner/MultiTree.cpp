// MultiTree.cpp : implementation file

#include "stdafx.h"
#include <windowsx.h>
#include "dsoundp.h"
#undef SubclassWindow
#include "AudioPathDesigner.h"
#include "MultiTree.h"
#include "resource.h"
#include "AudioPathDlg.h"
#include "AudioPath.h"
#include "ItemInfo.h"
#include "EffectInfo.h"
#include "RiffStrm.h"
#include "AudioPathDesignerDLL.h"
#include "MixGroupPPGMgr.h"
#include "BufferPPGMgr.h"
#include "EffectPPGMgr.h"
#include "AudioPathCtl.h"
#include "DllJazzDataObject.h"
#include "DlgAddRemoveBuses.h"
#include "EffectListDlg.h"
#include "DlgInsertEffect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef MST_TIMER_PERIOD
#define MST_TIMER_PERIOD	75		//ms
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditLabel

CEditLabel::CEditLabel( CString strOrigName )
{
	m_strOrigName = strOrigName;
}

CEditLabel::~CEditLabel()
{
}


BEGIN_MESSAGE_MAP(CEditLabel, CEdit)
	//{{AFX_MSG_MAP(CEditLabel)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditLabel message handlers

/////////////////////////////////////////////////////////////////////////////
// CEditLabel::OnKeyDown

void CEditLabel::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	{
		switch( nChar )
		{
			case VK_TAB:
			case VK_RETURN:
				CloseWindow();
				return;

			case 0x43:  // VK_C
				if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
				{
					Copy();
				}
				break;

			case 0x56:	// VK_V
				if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
				{
					Paste();
				}
				break;

			case 0x58:	// VK_X
				if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
				{
					Cut();
				}
				break;

			case 0x5A:	// VK_Z
				if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
				{
					SetSel( 0, -1 );
					ReplaceSel( m_strOrigName );
					SetSel( 0, -1 );
				}
				break;
		}
	}

	CEdit::OnKeyDown( nChar, nRepCnt, nFlags );
}


/////////////////////////////////////////////////////////////////////////////
// CMultiTree


IMPLEMENT_DYNAMIC(CMultiTree, CListCtrl)

CMultiTree::CMultiTree()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pAudioPathDlg = NULL;
	m_pointPopUpMenu.x = -1;
	m_pointPopUpMenu.y = -1;
	m_pEditCtrl = NULL;
}

CMultiTree::~CMultiTree()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Persist MixGroup or Effect to clipboard
	theApp.FlushClipboard( this );
}

BEGIN_MESSAGE_MAP(CMultiTree, CListCtrl)
	//{{AFX_MSG_MAP(CMultiTree)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditPChannelCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditPChannelCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditPChannelDelete)
	ON_COMMAND(ID_EDIT_INSERT, OnEditPChannelInsert)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPChannelPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditPChannelSelectAll)
	ON_COMMAND(ID_EDIT_INSERT_MIXGROUP, OnEditInsertMixGroup)
	ON_COMMAND(ID_EDIT_MIXGROUP_PROPERTIES, OnEditMixGroupProperties)
	ON_COMMAND(ID_EDIT_EFFECT_COPY, OnEditEffectCopy)
	ON_COMMAND(ID_EDIT_EFFECT_CUT, OnEditEffectCut)
	ON_COMMAND(ID_EDIT_EFFECT_DELETE, OnEditEffectDelete)
#ifndef DMP_XBOX
	ON_COMMAND(ID_EDIT_EFFECT_INSERT, OnEditEffectInsert)
	ON_COMMAND(ID_EDIT_SEND_EFFECT_INSERT, OnEditEffectInsertSend)
#endif
	ON_COMMAND(ID_EDIT_EFFECT_PASTE, OnEditEffectPaste)
	ON_COMMAND(ID_EDIT_EFFECT_SELECT_ALL, OnEditEffectSelectAll)
	ON_COMMAND(ID_EDIT_EFFECT_PROPERTIES, OnEditEffectProperties)
	ON_COMMAND(ID_EDIT_BUS_PROPERTIES, OnEditBusProperties)
	ON_COMMAND(ID_EDIT_BUFFER_INSERT, OnEditBufferInsert)
	ON_COMMAND(ID_EDIT_BUFFER_DELETE, OnEditBufferDelete)
	ON_COMMAND(ID_EDIT_BUS_ADDREMOVE, OnEditBusAddRemove)
	ON_COMMAND(ID_EDIT_INSERT_ENV_REVERB, OnEditInsertEnvReverb)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMultiTree message handlers


void CMultiTree::SelectAll( void )
{
	for( int iIndex = 0; iIndex < GetItemCount(); iIndex++ )
	{
		SetItemState( iIndex, LVIS_SELECTED, LVIS_SELECTED );
	}
}

void CMultiTree::SelectItem( ItemInfo *pItemInfo )
{
	ASSERT( pItemInfo );

	if( pItemInfo->lstBuffers.IsEmpty() )
	{
		int iIndex = m_pAudioPathDlg->FindItem( pItemInfo, NULL );
		SetItemState( iIndex, LVIS_SELECTED, LVIS_SELECTED );
	}

	POSITION pos = pItemInfo->lstBuffers.GetHeadPosition();
	while( pos )
	{
		BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( pos );

		int iIndex = m_pAudioPathDlg->FindItem( pItemInfo, pBufferOptions );
		SetItemState( iIndex, LVIS_SELECTED, LVIS_SELECTED );
	}
}


/////////////////////////////////////////////////////////////////////////////
// OnButtonDown

#define _bShift	(nFlags & MK_SHIFT)
#define _bCtrl	(nFlags & MK_CONTROL)


void CMultiTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	OnButtonDown(TRUE, nFlags, point);
}

void CMultiTree::OnRButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	OnButtonDown(FALSE, nFlags, point);
}

void CMultiTree::OnButtonDown(BOOL bLeft, UINT nFlags, CPoint point)
{
	UINT nHF = 0;
	int nItem = HitTest(point, &nHF);
	if (nItem >= 0 && !(nHF & LVHT_NOWHERE))
	{
		RECT rect;
		if( GetItemRect( nItem, &rect, LVIR_LABEL )
		&&	(point.x > rect.right) )
		{
			OnButtonDownOnItemRight( nItem, nFlags, point );
			SetFocus();
			return;
		}

		// Clean up all sub-field selections
		UnselectAllSubFields();
	}

	// Fall through to the base class implementation
	if( bLeft )
	{
		// If the user clicked on a selected item when we don't have focus
		if( (nItem >=0)
		&&	(nHF & LVHT_ONITEMLABEL)
		&&	IsSelected( nItem )
		&&	(::GetFocus() != m_hWnd) )
		{
			// Just set the focus to our control
			SetFocus();
		}
		else
		{
			// Otherwise, do the default behavior
			CListCtrl::OnLButtonDown( nFlags, point );
		}
	}
	else
	{
		CListCtrl::OnRButtonDown( nFlags, point );
	}

	m_pAudioPathDlg->SwitchToCorrectPropertyPage();
}

void CMultiTree::OnButtonDownOnItemRight(int nItem, UINT nFlags, CPoint point)
{
	RECT rectItemRect;
	GetItemRect( nItem, &rectItemRect, LVIR_BOUNDS );

	void *pObject = NULL;
	TreeColumn tcRes = GetThingUnderPointAndItem( point, nItem, &pObject );

	switch( tcRes )
	{
	case PCHANNEL_COL:
		// Should not happen
		ASSERT(FALSE);
		break;
	case BUS_BUFFER_COL:
		if( pObject )
		{
			BufferOptions *pBufferOptions = reinterpret_cast<BufferOptions *>(pObject);
			if( _bCtrl )
			{
				pBufferOptions->fSelected = !pBufferOptions->fSelected;
			}
			else
			{
				UnselectAll();
				pBufferOptions->fSelected = TRUE;
			}
			InvalidateRect( &rectItemRect, TRUE );
		}
		break;
	case EFFECT_COL:
		if( pObject )
		{
			BusEffectInfo *pBusEffectInfo = reinterpret_cast<BusEffectInfo *>(pObject);
			if( _bCtrl )
			{
				if( pBusEffectInfo->m_beiSelection == BEI_NONE )
				{
					pBusEffectInfo->m_beiSelection = BEI_NORMAL;
				}
				else
				{
					pBusEffectInfo->m_beiSelection = BEI_NONE;
				}
			}
			else
			{
				UnselectAll();
				pBusEffectInfo->m_beiSelection = BEI_NORMAL;
			}
			InvalidateRect( &rectItemRect, TRUE );
		}
		break;
	default:
		break;
	}

	m_pAudioPathDlg->SwitchToCorrectPropertyPage();
}


void CMultiTree::OnSetFocus(CWnd* pOldWnd) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListCtrl::OnSetFocus(pOldWnd);

	// Continue through all mix group items
	for( int i=0; i < GetItemCount(); i++ )
	{
		// Get a pointer to the ItemInfo in this item
		ItemInfo *pItemInfo = GetItemInfo( i );

		// Check if anything in the item is selected
		if( pItemInfo
		&&	(pItemInfo->IsAnythingSelected() || IsSelected(i)) )
		{
			// Redraw the item
			InvalidateItem( pItemInfo );
		}
	}

	if( m_pAudioPathDlg )
	{
		if( m_pAudioPathDlg->m_pAudioPathCtrl )
		{
			m_pAudioPathDlg->m_pAudioPathCtrl->SetLastFocus( FOC_MIXGROUP );
		}
	}
}

TreeColumn CMultiTree::GetFirstSelectedThing( ItemInfo **ppItemInfo, BufferOptions **ppBufferOptions, BusEffectInfo **ppBusEffectInfo ) const
{
	if( ppItemInfo )
	{
		*ppItemInfo = NULL;
	}

	if( ppBufferOptions )
	{
		*ppBufferOptions= NULL;
	}

	if( ppBusEffectInfo )
	{
		*ppBusEffectInfo = NULL;
	}

	// Iterate through the items
	for( int i=0; i < GetItemCount(); i++ )
	{
		// Get a pointer to this item
		ItemInfo *pItemInfo = GetItemInfo( i );

		// If the pointer is NULL, go on to the next item
		if( pItemInfo == NULL )
		{
			continue;
		}

		// If the MixGroup is selected, switch to the MixGroup PPG
		UINT nState = GetItemState( i, LVIS_SELECTED );
		if ( nState & LVIS_SELECTED )
		{
			if( ppItemInfo )
			{
				*ppItemInfo = pItemInfo;
			}
			return PCHANNEL_COL;
		}

		// See if a buffer or effect is selected
		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

			// If the buffer is selected, switch to the buffer PPG
			if( pBufferOptions->fSelected )
			{
				if( ppItemInfo )
				{
					*ppItemInfo = pItemInfo;
				}
				if( ppBufferOptions )
				{
					*ppBufferOptions = pBufferOptions;
				}
				return BUS_BUFFER_COL;
			}

			// See if any effect is selected
			POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
			while( posEffect )
			{
				// Get a pointer to each effect
				BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );

				// If the effect is selected, switch to the effect PPG
				if( pBusEffectInfo->m_beiSelection == BEI_NORMAL )
				{
					if( ppItemInfo )
					{
						*ppItemInfo = pItemInfo;
					}
					if( ppBufferOptions )
					{
						*ppBufferOptions = pBufferOptions;
					}
					if( ppBusEffectInfo )
					{
						*ppBusEffectInfo = pBusEffectInfo;
					}
					return EFFECT_COL;
				}
			}
		}
	}

	return NONE_COL;
}

void CMultiTree::OnKillFocus(CWnd* pNewWnd) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListCtrl::OnKillFocus(pNewWnd);
	//'emulated' selected items may not get
	// refreshed to grey

	// Continue through all mix group items
	for( int i=0; i < GetItemCount(); i++ )
	{
		// Get a pointer to the ItemInfo in this item
		ItemInfo *pItemInfo = GetItemInfo( i );

		// Check if anything in the item is selected
		if( pItemInfo
		&&	(pItemInfo->IsAnythingSelected() || IsSelected(i)) )
		{
			// Redraw the item
			InvalidateItem( pItemInfo );
		}
	}
}

void CMultiTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch (nChar) {
		case VK_F2:
			if( m_pEditCtrl == NULL )
			{
				POSITION pos = GetFirstSelectedItemPosition();
				if( pos )
				{
					// Get a pointer to the item being edited
					int nItem = GetNextSelectedItem( pos );
					ItemInfo *pItemInfo = GetItemInfo( nItem );
					if( pItemInfo )
					{
						// Ensure this is not the Env Reverb mix group
						if( pItemInfo->lstBuffers.GetCount() != 1
						||	pItemInfo->lstBuffers.GetHead()->guidBuffer != GUID_Buffer_EnvReverb )
						{
							// Edit the label
							CEdit *pEdit = EditLabel( nItem );
							if( pEdit )
							{
								// Change the text for the item to only contain the Mix Group name
								pEdit->SetWindowText( pItemInfo->strBandName );
								pEdit->SetSel( 0, -1, FALSE );
							}
						}
					}
				}
			}
			break;
	}

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMultiTree::GetSelectedList(CItemList& list) const
{
	list.RemoveAll();

	POSITION pos = GetFirstSelectedItemPosition();
	while( pos )
	{
		list.AddTail( GetItemInfo( GetNextSelectedItem( pos ) ) );
	}
}

void CMultiTree::SetDMAudioPathDlg( CAudioPathDlg *pDMAudioPathDlg )
{
	m_pAudioPathDlg = pDMAudioPathDlg;
}

void CMultiTree::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pWnd);

	ScreenToClient( &point );
	ItemInfo *pItemInfo = NULL;
	BusEffectInfo *pBusEffectInfo;
	BufferOptions *pBufferOptions;
	int iColumn = GetItemBufferEffectUnderPoint( point, &pItemInfo, &pBufferOptions, &pBusEffectInfo );

	// If there is no item under the mouse cursor
	if( pItemInfo == NULL )
	{
		// Always display the PChannel menu
		iColumn = PCHANNEL_COL;
	}

	HMENU hMenu = NULL;
	switch( iColumn )
	{
	case PCHANNEL_COL:
		hMenu = LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_PCHANNELTREE_RMENU));
		break;
	case BUS_BUFFER_COL:
		hMenu = LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_AUDIOBUS_RMENU));
		break;
	case EFFECT_COL:
		hMenu = LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_EFFECTLIST_RMENU));
		break;
	}

	if( hMenu )
	{
		HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );
		if( hMenuPopup )
		{
			// Get the position of the popup menu
			const DWORD dwPos = ::GetMessagePos();

			bool fTemp;
			switch( iColumn )
			{
			case BUS_BUFFER_COL:
				if( pItemInfo
				&&	pItemInfo->nNumPChannels > 0
				&&	ItemSupportsBuffers( pItemInfo ) )
				{
					if( pBufferOptions
					&&	!(pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
					{
						EnableMenuItem( hMenuPopup, ID_EDIT_BUS_ADDREMOVE, MF_ENABLED );
					}
					else
					{
						EnableMenuItem( hMenuPopup, ID_EDIT_BUS_ADDREMOVE, MF_GRAYED );
					}
					EnableMenuItem( hMenuPopup, ID_EDIT_BUFFER_INSERT, MF_ENABLED );
					EnableMenuItem( hMenuPopup, ID_EDIT_BUFFER_DELETE, pItemInfo->lstBuffers.GetCount() > 0 ? MF_ENABLED : MF_GRAYED );
				}
				else
				{
					EnableMenuItem( hMenuPopup, ID_EDIT_BUS_ADDREMOVE, MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_EDIT_BUFFER_INSERT, MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_EDIT_BUFFER_DELETE, MF_GRAYED );
				}
				break;

			case PCHANNEL_COL:
				// Enable/disable items as appropriate
				fTemp = (GetSelectedCount() > 0);

				EnableMenuItem( hMenuPopup, ID_EDIT_CUT, fTemp ?
								MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_COPY, fTemp ?
								MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, fTemp ?
								MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_MIXGROUP_PROPERTIES, fTemp ? MF_ENABLED : MF_GRAYED );
				if( m_pAudioPathDlg->m_pAudioPath )
				{
					fTemp = ::IsClipboardFormatAvailable( m_pAudioPathDlg->m_pAudioPath->m_scfPChannel ) ||
							::IsClipboardFormatAvailable( m_pAudioPathDlg->m_pAudioPath->m_scfEffect );
				}
				else
				{
					fTemp = false;
				}
				EnableMenuItem( hMenuPopup, ID_EDIT_PASTE, fTemp ?
								MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( GetItemCount() > signed(GetSelectedCount()) ) ?
								MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_INSERT_MIXGROUP, MF_ENABLED );
				EnableMenuItem( hMenuPopup, ID_EDIT_INSERT_ENV_REVERB, m_pAudioPathDlg->HasEnvReverb() ? MF_GRAYED : MF_ENABLED );

				// Check if the user clicked on the Env. Reverb mix group
				if( IsItemEnvReverb( pItemInfo ) )
				{
					EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, MF_GRAYED );
				}
				else
				{
					EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, MF_ENABLED );
				}
				break;

			case EFFECT_COL:
				// Enable/disable items as appropriate

				// fTemp is true if we clicked on an effect
				fTemp = ((pBusEffectInfo != NULL) ? true : false);

				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_CUT, fTemp ? MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_COPY, fTemp ? MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_DELETE, fTemp ? MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_PROPERTIES, fTemp ? MF_ENABLED : MF_GRAYED );

				fTemp = ItemAndBufferSupportEffects( pItemInfo, pBufferOptions );
				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_SELECT_ALL, (fTemp && (pBufferOptions->lstEffects.GetCount() > 0)) ? MF_ENABLED : MF_GRAYED );
#ifndef DMP_XBOX
				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_INSERT, fTemp ? MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_SEND_EFFECT_INSERT, fTemp ? MF_ENABLED : MF_GRAYED );
#else
				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_INSERT, MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_SEND_EFFECT_INSERT, MF_GRAYED );
#endif
				fTemp = (::IsClipboardFormatAvailable( m_pAudioPathDlg->m_pAudioPath->m_scfEffect ) != 0);
				EnableMenuItem( hMenuPopup, ID_EDIT_EFFECT_PASTE, fTemp ? MF_ENABLED : MF_GRAYED );
				break;
			}

			// Save the position of the popup menu
			m_pointPopUpMenu.x = GET_X_LPARAM( dwPos );
			m_pointPopUpMenu.y = GET_Y_LPARAM( dwPos );

			// Display the popup menu
			::TrackPopupMenu( hMenuPopup, TPM_LEFTALIGN | TPM_TOPALIGN, m_pointPopUpMenu.x, m_pointPopUpMenu.y, 0, m_hWnd, NULL );
		}

		DestroyMenu( hMenu ); // This will destroy the submenu as well.
	}
}

HRESULT	CMultiTree::CreateDataObject( IDataObject** ppIDataObject )
{
	// Validate our pointer to the data object
	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Initialize the data object
	*ppIDataObject = NULL;

	if( m_pAudioPathDlg && (GetSelectedCount() > 0) )
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

		CItemList lstTreeItems;
		GetSelectedList( lstTreeItems );

		// Create the RIFF stream to save the selected PChannels in
		IDMUSProdRIFFStream *pRiffStream = NULL;
		AllocRIFFStream( pIStream, &pRiffStream );

		// Write out all the selected PChannels
		POSITION pos = lstTreeItems.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to the information for the item
			const ItemInfo *pItemInfo = lstTreeItems.GetNext( pos );

			// Verify we got a pointer to the item
			if( pItemInfo )
			{
				// If this is a sub-item, then it needs to get the synth/effects/buffer information from the parent.
				pItemInfo->Write( pRiffStream, NULL );
			}
		}

		// Release the RIFF stream
		pRiffStream->Release();

		// Place MixGroups into CDllJazzDataObject
		pDataObject->AddClipFormat( CDirectMusicAudioPath::m_scfPChannel, pIStream );

		// Release the stream
		pIStream->Release();

		// Copy the pointer to the IDataObject
		*ppIDataObject = pDataObject; // Already AddRef'd

		// Return a success code
		return S_OK;
	}

	return E_FAIL;
}

void CMultiTree::OnEditPChannelCopy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(GetSelectedCount() > 0);

	// Get a copy of the currently selected MixGroups
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

void CMultiTree::OnEditPChannelCut() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(GetSelectedCount() > 0);
	if( m_pAudioPathDlg && (GetSelectedCount() > 0) )
	{
		// Save an undo state
		m_pAudioPathDlg->m_pAudioPath->SaveUndoState( IDS_UNDO_CUT_PCHANNELS, true );
		OnEditPChannelCopy();
		DoDelete();
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMultiTree::OnEditPChannelDelete() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(GetSelectedCount() > 0);
	if( m_pAudioPathDlg && (GetSelectedCount() > 0) )
	{
		// Save an undo state
		m_pAudioPathDlg->m_pAudioPath->SaveUndoState( IDS_UNDO_DELETE_PCHANNELS, true );
		DoDelete();
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMultiTree::DoDelete( void )
{
	ASSERT(GetSelectedCount() > 0);
	if( m_pAudioPathDlg && (GetSelectedCount() > 0) )
	{
		CItemList lstTreeItems;
		GetSelectedList( lstTreeItems );

		while( !lstTreeItems.IsEmpty() )
		{
			ItemInfo *pItemInfo = lstTreeItems.RemoveHead();

			if( pItemInfo )
			{
				// Removing a first-level item
				const POSITION posToRemove = m_pAudioPathDlg->m_pAudioPath->m_lstItems.Find( pItemInfo );
				ASSERT( posToRemove );
				m_pAudioPathDlg->m_pAudioPath->m_lstItems.RemoveAt( posToRemove );
			}

			// Update any send effects that pointed at this item
			m_pAudioPathDlg->m_pAudioPath->DisconnectSendEffects( pItemInfo );

			// Now, delete the item that was clicked on
			int nItem = m_pAudioPathDlg->FindItem( pItemInfo, NULL );
			while( nItem >= 0 )
			{
				ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(GetItemData( nItem ));
				ASSERT( pItemInfoWrapper );
				DeleteItem( nItem );
				delete pItemInfoWrapper;
				nItem = m_pAudioPathDlg->FindItem( pItemInfo, NULL );
			}
			if( pItemInfo )
			{
				delete pItemInfo;
			}
		}

		// Now, sort the band-level PChannels
		m_pAudioPathDlg->m_pAudioPath->UpdateDisplayIndexes();
		SortItems( ComparePChannelsInTree, 0 );

		m_pAudioPathDlg->m_pAudioPath->SyncAudiopathsInUse( AUDIOPATH_UNLOAD_DOWNLOAD_WAVES );

		m_pAudioPathDlg->m_pAudioPathCtrl->SwitchToCorrectPropertyPage();
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMultiTree::OnEditPChannelInsert() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPathDlg )
	{
		m_pAudioPathDlg->InsertPChannel( m_pointPopUpMenu );
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMultiTree::OnEditInsertMixGroup()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPathDlg )
	{
		POINT pointNowhere = { LONG_MIN, LONG_MIN };
		m_pAudioPathDlg->InsertPChannel( pointNowhere );
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMultiTree::OnEditInsertEnvReverb()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPathDlg )
	{
		m_pAudioPathDlg->InsertEnvReverb();
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMultiTree::OnEditPChannelPaste() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPathDlg && m_pAudioPathDlg->m_pAudioPath )
	{
		if( ::IsClipboardFormatAvailable( m_pAudioPathDlg->m_pAudioPath->m_scfPChannel ) )
		{
			m_pAudioPathDlg->PastePChannelFromClipboard( m_pointPopUpMenu );
		}
		else if( ::IsClipboardFormatAvailable( m_pAudioPathDlg->m_pAudioPath->m_scfEffect ) )
		{
			m_pAudioPathDlg->PasteEffectFromClipboard( m_pointPopUpMenu );
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMultiTree::GetEffectRect( const POINT &point, int nTreeItem, BusEffectInfo **ppBusEffectInfo, RECT *pRect )
{
	ASSERT( nTreeItem >= 0 );
	ASSERT( ppBusEffectInfo || pRect );

	if( ppBusEffectInfo )
	{
		*ppBusEffectInfo = NULL;
	}

	if( pRect )
	{
		ZeroMemory( pRect, sizeof(RECT) );
	}

	ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(GetItemData( nTreeItem ));

	// If the pointer is NULL, return early
	if( pItemInfoWrapper == NULL )
	{
		return;
	}

	ItemInfo *pItemInfo = pItemInfoWrapper->pItemInfo;

	// If the pointer is NULL, return early
	if( pItemInfo == NULL )
	{
		return;
	}

	// If there are no buffers, return early
	if( pItemInfo->lstBuffers.IsEmpty()
	||	!pItemInfoWrapper->pBufferOptions )
	{
		return;
	}

	long lMixGroupWidth, lBusWidth, lBufferWidth;
	if( pItemInfo
	&&	m_pAudioPathDlg->GetColumnWidths( lMixGroupWidth, lBusWidth, lBufferWidth ) )
	{
		RECT rectItem;
		if( 0 != GetItemRect( nTreeItem, &rectItem, LVIR_BOUNDS ) )
		{
			RECT rectWindow;
			{
				rectWindow.left = lMixGroupWidth + lBusWidth + lBufferWidth - GetScrollPos(SB_HORZ);
				rectWindow.top = rectItem.top;
				rectWindow.bottom = rectItem.bottom;

				// Pointer to each buffer
				BufferOptions *pBufferOptions = pItemInfoWrapper->pBufferOptions;

				if( ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
				{
					POSITION pos = pBufferOptions->lstEffects.GetHeadPosition();
					while( pos )
					{
						BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( pos );
						rectWindow.right = rectWindow.left + pBusEffectInfo->m_lPixelWidth;

						if( (rectWindow.left <= point.x)
						&&	(rectWindow.right >= point.x) )
						{
							if( ppBusEffectInfo )
							{
								*ppBusEffectInfo = pBusEffectInfo;
							}
							if( pRect )
							{
								*pRect = rectWindow;
							}
							break;
						}
						rectWindow.left += pBusEffectInfo->m_lPixelWidth;
					}
				}
			}
		}
	}
}

TreeColumn CMultiTree::GetColumn( long lXPos )
{
	lXPos += GetScrollPos(SB_HORZ);

	if( lXPos < 0 )
	{
		return NONE_COL;
	}

	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH;

	// Mix group column
	if( GetColumn( 0, &lvColumn ) )
	{
		lXPos -= lvColumn.cx;

		if( lXPos < 0 )
		{
			return PCHANNEL_COL;
		}

	}

	// Bus column
	if( GetColumn( 1, &lvColumn ) )
	{
		lXPos -= lvColumn.cx;

		if( lXPos < 0 )
		{
			return BUS_BUFFER_COL;
		}
	}

	// Buffer column
	if( GetColumn( 2, &lvColumn ) )
	{
		lXPos -= lvColumn.cx;

		if( lXPos < 0 )
		{
			return BUS_BUFFER_COL;
		}
	}

	return EFFECT_COL;
}

TreeColumn CMultiTree::GetItemBufferEffectUnderPoint( const POINT &point, ItemInfo **ppItem, BufferOptions **ppBufferOptions, BusEffectInfo **ppBusEffectInfo )
{
	TreeColumn tcRes = NONE_COL;

	if( ppItem )
	{
		*ppItem = NULL;
	}
	if( ppBufferOptions )
	{
		*ppBufferOptions = NULL;
	}
	if( ppBusEffectInfo )
	{
		*ppBusEffectInfo = NULL;
	}

	int nItem = HitTest( point, NULL );
	if( nItem >= 0 )
	{
		ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(GetItemData( nItem ));
		if( !pItemInfoWrapper )
		{
			return NONE_COL;
		}

		ItemInfo *pItemInfo = pItemInfoWrapper->pItemInfo;;
		if( !pItemInfo )
		{
			return NONE_COL;
		}

		if( ppItem )
		{
			*ppItem = pItemInfo;
		}

		tcRes = GetColumn( point.x );

		RECT rectItem;
		switch( tcRes )
		{
		case BUS_BUFFER_COL:
		case EFFECT_COL:
			// If we found an item, the item has at least one buffer, and we should return
			// a pointer to the buffer and/or effect
			if( !pItemInfo->lstBuffers.IsEmpty()
			&&	(ppBufferOptions || ppBusEffectInfo)
			&&	( 0 != GetItemRect( nItem, &rectItem, LVIR_BOUNDS )) )
			{
				// Our pointer to the clicked on buffer
				BufferOptions *pBufferOptions = pItemInfoWrapper->pBufferOptions;

				if( pBufferOptions )
				{
					if( ppBufferOptions )
					{
						*ppBufferOptions = pBufferOptions;
					}

					long lMixGroupWidth, lBusWidth, lBufferWidth;
					if( ppBusEffectInfo
					&&	ItemAndBufferSupportEffects( pItemInfo, pBufferOptions )
					&&	m_pAudioPathDlg->GetColumnWidths( lMixGroupWidth, lBusWidth, lBufferWidth ) )
					{
						long lLeft = lMixGroupWidth + lBusWidth + lBufferWidth - GetScrollPos(SB_HORZ);

						POSITION pos = pBufferOptions->lstEffects.GetHeadPosition();
						while( pos )
						{
							BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( pos );
							const long lRight = lLeft + pBusEffectInfo->m_lPixelWidth;

							if( (lLeft <= point.x)
							&&	(lRight >= point.x) )
							{
								*ppBusEffectInfo = pBusEffectInfo;
								break;
							}
							lLeft += pBusEffectInfo->m_lPixelWidth;
						}
					}
				}
			}
			break;
		}
	}

	return tcRes;
}

TreeColumn CMultiTree::GetThingUnderPointAndItem( const POINT &point, int nTreeItem, void **ppThing )
{
	UNREFERENCED_PARAMETER(nTreeItem);

	if( ppThing )
	{
		ItemInfo *pItemInfo;
		BufferOptions *pBufferOptions;
		BusEffectInfo *pBusEffectInfo;
		const TreeColumn tcRes = GetItemBufferEffectUnderPoint( point, &pItemInfo, &pBufferOptions, &pBusEffectInfo );

		switch( tcRes )
		{
		case PCHANNEL_COL:
			*ppThing = pItemInfo;
			break;
		case BUS_BUFFER_COL:
			*ppThing = pBufferOptions;
			break;
		case EFFECT_COL:
			*ppThing = pBusEffectInfo;
			break;
		case NONE_COL:
			*ppThing = NULL;
			break;
		}

		return tcRes;
	}
	else
	{
		return GetColumn( point.x );
	}
}

void CMultiTree::UnselectAll(void )
{
	// Continue until we're through all mix group items
	for( int i=0; i < GetItemCount(); i++ )
	{
		// If the parent item is selected
		if (IsSelected(i) != 0)
		{
			// Unselect it
			SetItemState(i, 0, LVIS_SELECTED);
		}

		// Get a pointer to the ItemInfo in this item
		ItemInfo *pItemInfo = GetItemInfo( i );

		// Unselect it.  If anything was previously selected
		if( pItemInfo && pItemInfo->Select( false ) )
		{
			// Redraw the item
			InvalidateItem( pItemInfo );
		}
	}
}

void CMultiTree::OnEditPChannelSelectAll()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	SelectAll();
}

void CMultiTree::OnEditEffectCopy()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get a pointer to the first selected effect
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	BusEffectInfo *pBusEffectInfo;
	if( GetFirstSelectedThing( &pItemInfo, &pBufferOptions, &pBusEffectInfo ) == EFFECT_COL )
	{
		IDataObject *pIDataObject;
		if( SUCCEEDED( CreateDataObjectEffect( &pIDataObject ) ) )
		{
			// Send the data to the clipboard
			theApp.PutDataInClipboard( pIDataObject, this );

			// Release our reference to the data object
			pIDataObject->Release();
		}
	}
}

void CMultiTree::OnEditEffectCut()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Just do a copy followed by a delete
	OnEditEffectCopy();
	OnEditEffectDelete();
}

void CMultiTree::OnEditEffectDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get a pointer to the first selected effect
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	if( GetFirstSelectedThing( &pItemInfo, &pBufferOptions, NULL ) == EFFECT_COL )
	{
		// Save an UNDO state
		m_pAudioPathDlg->m_pAudioPath->SaveUndoState( IDS_UNDO_DELETE_EFFECT );

		POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
		while( posEffect )
		{
			const POSITION posToRemove = posEffect;
			BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );
			if( pBusEffectInfo->m_beiSelection == BEI_NORMAL )
			{
				pBufferOptions->lstEffects.RemoveAt( posToRemove );

				// Delete the Bus effect
				delete pBusEffectInfo;
			}
		}

		// Update the internal names, so that column resize works correctly
		m_pAudioPathDlg->UpdateTreeItemName( pItemInfo );

		// Redraw the item
		InvalidateItem( pItemInfo );

		// Resort the display
		m_pAudioPathDlg->m_pAudioPath->UpdateDisplayIndexes();
		SortItems( ComparePChannelsInTree, 0 );

		// Resize the effect column, since an effect was removed
		m_pAudioPathDlg->ResizeEffectColumn();

		m_pAudioPathDlg->m_pAudioPath->SyncAudiopathsInUse( 0 );

		m_pAudioPathDlg->m_pAudioPathCtrl->SwitchToCorrectPropertyPage();
	}
}

#ifndef DMP_XBOX
void CMultiTree::OnEditEffectInsert()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Convert from screen to client coordinates
	POINT pointInsert = m_pointPopUpMenu;
	ScreenToClient( &pointInsert );

	UINT flags;
	int nTargetItem = HitTest(pointInsert, &flags);
	if( nTargetItem >= 0 )
	{
		BusEffectInfo *pInsertedOnBusEffectInfo;
		RECT rectDroppedOnEffect;
		GetEffectRect( pointInsert, nTargetItem, &pInsertedOnBusEffectInfo, &rectDroppedOnEffect );

		ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(GetItemData( nTargetItem ));

		// If the pointer is NULL, return early
		if( pItemInfoWrapper == NULL )
		{
			return;
		}

		ItemInfo *pItemInfo = pItemInfoWrapper->pItemInfo;

		if( pItemInfo
		&&	(pInsertedOnBusEffectInfo || !pItemInfo->lstBuffers.IsEmpty()) )
		{
			// Create and initialize the dialog
			CDlgInsertEffect dlgInsertEffect;
			dlgInsertEffect.m_pAudioPath = m_pAudioPathDlg->m_pAudioPath;

			// Display the dialog
			if( (dlgInsertEffect.DoModal() == IDOK)
			&&	(dlgInsertEffect.m_EffectInfoToCopy.m_clsidObject != GUID_NULL) )
			{
				// Save an undo state
				m_pAudioPathDlg->m_pAudioPath->SaveUndoState( IDS_UNDO_INSERT_EFFECT, true );

				if( pItemInfo && pInsertedOnBusEffectInfo )
				{
					POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
					while( posBuffer )
					{
						BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

						POSITION pos = pBufferOptions->lstEffects.Find( pInsertedOnBusEffectInfo );
						if( pos )
						{
							// Create a BusEffectInfo to wrap the dropped EffectInfo
							BusEffectInfo *pBusEffectInfo = new BusEffectInfo( &dlgInsertEffect.m_EffectInfoToCopy );

							if( pBusEffectInfo )
							{
								// Set the instance name for the effect
								pBusEffectInfo->m_EffectInfo.m_strInstanceName = dlgInsertEffect.m_strInstanceName;

								// Make only this effect selected
								UnselectAll();
								pBusEffectInfo->m_beiSelection = BEI_NORMAL;

								if( pointInsert.x + GetScrollPos(SB_HORZ) <= rectDroppedOnEffect.left + pInsertedOnBusEffectInfo->m_lPixelWidth / 2 )
								{
									pBufferOptions->lstEffects.InsertBefore( pos, pBusEffectInfo );
								}
								else
								{
									pBufferOptions->lstEffects.InsertAfter( pos, pBusEffectInfo );
								}
							}
							break;
						}
					}
				}
				else if( pItemInfo && !pItemInfo->lstBuffers.IsEmpty() )
				{
					// Get the rectangle of the item
					RECT rectItem;
					if( 0 != GetItemRect( nTargetItem, &rectItem, LVIR_BOUNDS ) )
					{
						// Pointer to the buffer
						BufferOptions *pBufferOptions = pItemInfoWrapper->pBufferOptions;

						// Add the effect to the buffer, if it supports effects,
						// and we can get the column widths
						long lMixGroupWidth, lBusWidth, lBufferWidth;
						if( ItemAndBufferSupportEffects( pItemInfo, pBufferOptions )
						&&	m_pAudioPathDlg->GetColumnWidths( lMixGroupWidth, lBusWidth, lBufferWidth ) )
						{
							// Create a BusEffectInfo to wrap the dropped EffectInfo
							BusEffectInfo *pBusEffectInfo = new BusEffectInfo( &dlgInsertEffect.m_EffectInfoToCopy );

							if( pBusEffectInfo )
							{
								// Set the instance name for the effect
								pBusEffectInfo->m_EffectInfo.m_strInstanceName = dlgInsertEffect.m_strInstanceName;

								// Make only this effect selected
								UnselectAll();
								pBusEffectInfo->m_beiSelection = BEI_NORMAL;

								if( pointInsert.x + GetScrollPos(SB_HORZ) <= lMixGroupWidth + lBusWidth + lBufferWidth )
								{
									pBufferOptions->lstEffects.AddHead( pBusEffectInfo );
								}
								else
								{
									pBufferOptions->lstEffects.AddTail( pBusEffectInfo );
								}
							}
						}
					}
				}

				if( pItemInfo )
				{
					// Update the internal names, so that column resize works correctly
					m_pAudioPathDlg->UpdateTreeItemName( pItemInfo );

					// Resize the effect column, since an effect may have been added
					m_pAudioPathDlg->ResizeEffectColumn();
				}

				InvalidateItem( nTargetItem );

				m_pAudioPathDlg->m_pAudioPath->SyncAudiopathsInUse( 0 );

				// Set the focus to the mix group
				m_pAudioPathDlg->m_pAudioPathCtrl->SetLastFocus( FOC_MIXGROUP );

				// Switch to the correct property page
				m_pAudioPathDlg->m_pAudioPathCtrl->SwitchToCorrectPropertyPage();
			}
		}
	}
}

void CMultiTree::OnEditEffectInsertSend()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Convert from screen to client coordinates
	POINT pointInsert = m_pointPopUpMenu;
	ScreenToClient( &pointInsert );

	UINT flags;
	int nTargetItem = HitTest(pointInsert, &flags);
	if( nTargetItem >= 0 )
	{
		// Save an undo state
		m_pAudioPathDlg->m_pAudioPath->SaveUndoState( IDS_UNDO_INSERT_EFFECT, true );

		BusEffectInfo *pInsertedOnBusEffectInfo;
		RECT rectDroppedOnEffect;
		GetEffectRect( pointInsert, nTargetItem, &pInsertedOnBusEffectInfo, &rectDroppedOnEffect );

		ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(GetItemData( nTargetItem ));

		// If the pointer is NULL, return early
		if( pItemInfoWrapper == NULL )
		{
			return;
		}

		ItemInfo *pItemInfo = pItemInfoWrapper->pItemInfo;

		if( pItemInfo && pInsertedOnBusEffectInfo )
		{
			POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
			while( posBuffer )
			{
				BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

				POSITION pos = pBufferOptions->lstEffects.Find( pInsertedOnBusEffectInfo );
				if( pos )
				{
					// Initialize to send to the EnvReverb buffer
					GUID guidToSendTo = GUID_Buffer_EnvReverb;

					// Find the first mixgroup that doesn't have any PChannels
					for( int i=0; i < GetItemCount(); i++ )
					{
						// Get a pointer to the ItemInfo in this item
						ItemInfo *pDestItemInfo = GetItemInfo( i );

						// Check if the item has zero PChannels and at least one buffer
						if( pDestItemInfo
						&&	pDestItemInfo != pItemInfo
						&&	pDestItemInfo->nNumPChannels == 0
						&&	!pDestItemInfo->lstBuffers.IsEmpty() )
						{
							guidToSendTo = pDestItemInfo->lstBuffers.GetHead()->guidBuffer;
							break;
						}
					}

					CString strText;
					strText.LoadString( IDS_SEND_TEXT );
					EffectInfo *pNewEffectInfo = new EffectInfo( strText,strText, GUID_DSFX_SEND, guidToSendTo);

					if( pNewEffectInfo )
					{
						// Ensure the effect's name is unique
						m_pAudioPathDlg->m_pAudioPath->GetUniqueEffectInstanceName( pNewEffectInfo );

						// Create a BusEffectInfo to wrap the dropped EffectInfo
						BusEffectInfo *pBusEffectInfo = new BusEffectInfo(pNewEffectInfo);

						if( pBusEffectInfo )
						{
							if( pointInsert.x + GetScrollPos(SB_HORZ) <= rectDroppedOnEffect.left + pInsertedOnBusEffectInfo->m_lPixelWidth / 2 )
							{
								pBufferOptions->lstEffects.InsertBefore( pos, pBusEffectInfo );
							}
							else
							{
								pBufferOptions->lstEffects.InsertAfter( pos, pBusEffectInfo );
							}
						}
						delete pNewEffectInfo;
					}
					break;
				}
			}
		}
		else if( pItemInfo && !pItemInfo->lstBuffers.IsEmpty() )
		{
			// Get the rectangle of the item
			RECT rectItem;
			if( 0 != GetItemRect( nTargetItem, &rectItem, LVIR_BOUNDS ) )
			{
				// Pointer to each buffer
				BufferOptions *pBufferOptions = pItemInfoWrapper->pBufferOptions;

				// Add the effect to the buffer, if it supports effects,
				// and we can get the column widths
				long lMixGroupWidth, lBusWidth, lBufferWidth;
				if( ItemAndBufferSupportEffects( pItemInfo, pBufferOptions )
				&&	m_pAudioPathDlg->GetColumnWidths( lMixGroupWidth, lBusWidth, lBufferWidth ) )
				{
					// Initialize to send to the EnvReverb buffer
					GUID guidToSendTo = GUID_Buffer_EnvReverb;

					// Find the first mixgroup that doesn't have any PChannels
					for( int i=0; i < GetItemCount(); i++ )
					{
						// Get a pointer to the ItemInfo in this item
						ItemInfo *pDestItemInfo = GetItemInfo( i );

						// Check if the item has zero PChannels and at least one buffer
						if( pDestItemInfo
						&&	pDestItemInfo != pItemInfo
						&&	pDestItemInfo->nNumPChannels == 0
						&&	!pDestItemInfo->lstBuffers.IsEmpty() )
						{
							guidToSendTo = pDestItemInfo->lstBuffers.GetHead()->guidBuffer;
							break;
						}
					}

					CString strText;
					strText.LoadString( IDS_SEND_TEXT );
					EffectInfo *pNewEffectInfo = new EffectInfo( strText, strText, GUID_DSFX_SEND, guidToSendTo);

					if( pNewEffectInfo )
					{
						// Ensure the effect's name is unique
						m_pAudioPathDlg->m_pAudioPath->GetUniqueEffectInstanceName( pNewEffectInfo );

						// Create a BusEffectInfo to wrap the dropped EffectInfo
						BusEffectInfo *pBusEffectInfo = new BusEffectInfo(pNewEffectInfo);

						if( pBusEffectInfo )
						{
							// Make only this effect selected
							UnselectAll();
							pBusEffectInfo->m_beiSelection = BEI_NORMAL;

							if( pointInsert.x + GetScrollPos(SB_HORZ)<= lMixGroupWidth + lBusWidth + lBufferWidth )
							{
								pBufferOptions->lstEffects.AddHead( pBusEffectInfo );
							}
							else
							{
								pBufferOptions->lstEffects.AddTail( pBusEffectInfo );
							}
						}
						
						delete pNewEffectInfo;
					}
				}
			}
		}

		if( pItemInfo )
		{
			// Update the internal names, so that column resize works correctly
			m_pAudioPathDlg->UpdateTreeItemName( pItemInfo );

			// Resize the effect column, since an effect may have been added
			m_pAudioPathDlg->ResizeEffectColumn();
		}

		InvalidateItem( nTargetItem );

		// Resort the display
		m_pAudioPathDlg->m_pAudioPath->UpdateDisplayIndexes();
		SortItems( ComparePChannelsInTree, 0 );

		m_pAudioPathDlg->m_pAudioPath->SyncAudiopathsInUse( 0 );

		// Set the focus to the mix group
		m_pAudioPathDlg->m_pAudioPathCtrl->SetLastFocus( FOC_MIXGROUP );

		// Switch to the correct property page
		m_pAudioPathDlg->m_pAudioPathCtrl->SwitchToCorrectPropertyPage();
	}
}
#endif // DMP_XBOX

void CMultiTree::OnEditEffectPaste()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pAudioPathDlg->PasteEffectFromClipboard( m_pointPopUpMenu );
}

void CMultiTree::OnEditEffectSelectAll()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Convert from screen to client coordinates
	POINT pointInsert = m_pointPopUpMenu;
	ScreenToClient( &pointInsert );

	// Find out which buffer was clicked on
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	GetItemBufferEffectUnderPoint( pointInsert, &pItemInfo, &pBufferOptions, NULL );
	if( ItemAndBufferSupportEffects( pItemInfo, pBufferOptions )
	&&	!pBufferOptions->lstEffects.IsEmpty() )
	{
		// Make only these effects selected
		UnselectAll();

		POSITION pos = pBufferOptions->lstEffects.GetHeadPosition();
		while( pos )
		{
			pBufferOptions->lstEffects.GetNext( pos )->m_beiSelection = BEI_NORMAL;
		}

		InvalidateItem( pItemInfo );
	}
}

void CMultiTree::OnEditMixGroupProperties()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	// Get a pointer to the first selected item
	const ItemInfo *pItemInfo = GetFirstSelectedMixGroupItem();
	if( pItemInfo )
	{
		m_pAudioPathDlg->UpdateMixGroupForPPG();

		// Verify that the property sheet is showing
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() != S_OK )
			{
				short nActiveTab = CMixGroupPPGMgr::sm_nActiveTab;

				if( SUCCEEDED ( pIPropSheet->SetPageManager(theApp.m_pAudioPathComponent->m_pIMixGroupPageManager) ) )
				{
					theApp.m_pAudioPathComponent->m_pIMixGroupPageManager->SetObject( m_pAudioPathDlg );
					pIPropSheet->SetActivePage( nActiveTab ); 
				}

				// Show the property sheet
				pIPropSheet->Show( TRUE );

				// Refresh the property sheet title
				pIPropSheet->RefreshTitleByObject( m_pAudioPathDlg );

				// Refresh the property sheet page
				pIPropSheet->RefreshActivePageByObject( m_pAudioPathDlg );
			}
			pIPropSheet->Release();
		}
	}
}

void CMultiTree::OnEditBusAddRemove()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Convert from screen to client coordinates
	POINT pointInsert = m_pointPopUpMenu;
	ScreenToClient( &pointInsert );

	// Get the item inserted on
	UINT uFlags;
	int nTreeItem = HitTest( pointInsert, &uFlags );
	if( nTreeItem >= 0 )
	{
		// Get a pointer to the mix group item
		//ItemInfo *pItemInfo = GetItemInfo( nTreeItem );

		// Get the buffer to change
		BufferOptions *pBufferOptions;
		if( (GetThingUnderPointAndItem( pointInsert, nTreeItem, (void **)&pBufferOptions ) == BUS_BUFFER_COL)
		&&	pBufferOptions )
		{
			CDlgAddRemoveBuses dlgAddRemoveBuses;

			dlgAddRemoveBuses.m_adwBusIDs.Copy( pBufferOptions->lstBusIDs );

			if( (dlgAddRemoveBuses.DoModal() == IDOK)
			&&	((pBufferOptions->lstBusIDs.GetSize() != dlgAddRemoveBuses.m_adwBusIDs.GetSize()) ||
				 (0 != memcmp( pBufferOptions->lstBusIDs.GetData(), dlgAddRemoveBuses.m_adwBusIDs.GetData(), sizeof(DWORD) * pBufferOptions->lstBusIDs.GetSize() ))) )
			{
				// Save an UNDO state
				m_pAudioPathDlg->m_pAudioPath->SaveUndoState( IDS_UNDO_ADDREMOVE_BUSES );

				// Save the offset between the # of buses and the # of channels
				const int nChannelOffset = pBufferOptions->wChannels - pBufferOptions->lstBusIDs.GetSize();
				ASSERT( nChannelOffset >= 0 );

				// Copy the new list of buses
				pBufferOptions->lstBusIDs.Copy( dlgAddRemoveBuses.m_adwBusIDs );

				// Update the # of channels
				pBufferOptions->wChannels = WORD( min( MAX_AUDIO_CHANNELS, pBufferOptions->lstBusIDs.GetSize() + nChannelOffset ) );

				// Redraw the item's bus list
				InvalidateItem( nTreeItem );

				// Update the buffer property page
				m_pAudioPathDlg->UpdateBufferForPPG();
				theApp.m_pAudioPathComponent->m_pIBufferPageManager->RefreshData();

				m_pAudioPathDlg->m_pAudioPath->SyncAudiopathsInUse( 0 );
			}
		}
	}
}

ItemInfo *CMultiTree::GetFirstSelectedMixGroupItem() const
{
	POSITION pos = GetFirstSelectedItemPosition();
	if( pos )
	{
		// Return a pointer to the selected item
		return GetItemInfo( GetNextSelectedItem( pos ) );
	}

	return NULL;
}

void CMultiTree::UnselectAllSubFields(void )
{
	// Continue until we're through all mix group items
	for( int iIndex = 0; iIndex < GetItemCount(); iIndex++ )
	{
		// Get a pointer to the ItemInfo in this item
		ItemInfo *pItemInfo = GetItemInfo( iIndex );

		// Unselect it.  If anything was previously selected
		if( pItemInfo && pItemInfo->Select( false ) )
		{
			// Redraw the item
			InvalidateItem( pItemInfo );
		}
	}
}

void CMultiTree::OnEditEffectProperties()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	// Get a pointer to the first selected item
	if( GetFirstSelectedThing( NULL, NULL, NULL ) == EFFECT_COL )
	{
		m_pAudioPathDlg->UpdateEffectForPPG();

		// Verify that the property sheet is showing
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() != S_OK )
			{
				short nActiveTab = CEffectPPGMgr::sm_nActiveTab;

				if( SUCCEEDED ( pIPropSheet->SetPageManager(theApp.m_pAudioPathComponent->m_pIEffectPageManager) ) )
				{
					theApp.m_pAudioPathComponent->m_pIEffectPageManager->SetObject( m_pAudioPathDlg );
					pIPropSheet->SetActivePage( nActiveTab ); 
				}

				// Show the property sheet
				pIPropSheet->Show( TRUE );

				// Refresh the property sheet title
				pIPropSheet->RefreshTitleByObject( m_pAudioPathDlg );

				// Refresh the property sheet page
				pIPropSheet->RefreshActivePageByObject( m_pAudioPathDlg );
			}
			pIPropSheet->Release();
		}
	}
}

void CMultiTree::OnEditBusProperties()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	// Get a pointer to the first selected item
	if( GetFirstSelectedThing( NULL, NULL, NULL ) == BUS_BUFFER_COL )
	{
		m_pAudioPathDlg->UpdateBufferForPPG();

		// Verify that the property sheet is showing
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() != S_OK )
			{
				short nActiveTab = CBufferPPGMgr::sm_nActiveTab;

				if( SUCCEEDED ( pIPropSheet->SetPageManager(theApp.m_pAudioPathComponent->m_pIBufferPageManager ) ) )
				{
					theApp.m_pAudioPathComponent->m_pIBufferPageManager->SetObject( m_pAudioPathDlg );
					pIPropSheet->SetActivePage( nActiveTab ); 
				}

				// Show the property sheet
				pIPropSheet->Show( TRUE );

				// Refresh the property sheet title
				pIPropSheet->RefreshTitleByObject( m_pAudioPathDlg );

				// Refresh the property sheet page
				pIPropSheet->RefreshActivePageByObject( m_pAudioPathDlg );
			}
			pIPropSheet->Release();
		}
	}
}

void CMultiTree::OnEditBufferInsert()
{
	// Convert from screen to client coordinates
	POINT pointInsert = m_pointPopUpMenu;
	ScreenToClient( &pointInsert );

	// Get the item inserted on
	UINT uFlags;
	int nTreeItem = HitTest( pointInsert, &uFlags );
	if( nTreeItem >= 0 )
	{
		// Get a pointer to the mix group item
		ItemInfo *pItemInfo = GetItemInfo( nTreeItem );

		if( pItemInfo )
		{
			// Save an UNDO state
			m_pAudioPathDlg->m_pAudioPath->SaveUndoState( IDS_UNDO_INSERT_BUFFER );

			// Create a buffer
			BufferOptions *pBufferOptions = new BufferOptions;

			// Add the buffer to the item
			pItemInfo->lstBuffers.AddTail( pBufferOptions );

			// If there was an existing buffer
			if( pItemInfo->lstBuffers.GetCount() > 1 )
			{
				// Insert the buffer into the list
				LVITEM lvItem;
				lvItem.mask = LVIF_PARAM | LVIF_TEXT;
				lvItem.iItem = 0;
				lvItem.iSubItem = 0;
				lvItem.pszText = pItemInfo->strBandName.GetBuffer( 0 );
				lvItem.lParam = DWORD(new ItemInfoWrapper(pItemInfo, pBufferOptions));
				InsertItem( &lvItem );
				pItemInfo->strBandName.ReleaseBuffer( -1 );
			}
			else
			{
				// No existing buffer - set the click-on item's buffer pointer
				ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(GetItemData( nTreeItem ));
				if( pItemInfoWrapper )
				{
					pItemInfoWrapper->pBufferOptions = pBufferOptions;
				}
			}

			m_pAudioPathDlg->UpdateTreeItemName( pItemInfo );
			m_pAudioPathDlg->m_pAudioPath->UpdateDisplayIndexes();
			SortItems( ComparePChannelsInTree, 0 );

			// Redraw the parent item's synth and effect setting, in case
			// it should become unselected
			// TODO: Redraw less
			InvalidateRect( NULL );

			m_pAudioPathDlg->m_pAudioPath->SyncAudiopathsInUse( 0 );
		}
	}
}

void CMultiTree::OnEditBufferDelete()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get a pointer to the first selected buffer
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	if( GetFirstSelectedThing( &pItemInfo, &pBufferOptions, NULL ) == BUS_BUFFER_COL )
	{
		// Save an UNDO state
		m_pAudioPathDlg->m_pAudioPath->SaveUndoState( IDS_UNDO_DELETE_BUFFER );

		// Now, delete all the selected buffers from this item
		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Save the current position
			const POSITION posCurrent = posBuffer;

			// Get a pointer to each buffer
			pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );
			if( pBufferOptions->fSelected )
			{
				// Remove the buffer from the list
				pItemInfo->lstBuffers.RemoveAt( posCurrent );

				// Find the tree item corresponding to this buffer
				int nTreeItem = m_pAudioPathDlg->FindItem( pItemInfo, pBufferOptions );
				if( nTreeItem >= 0 )
				{
					ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(GetItemData( nTreeItem ));
					ASSERT( pItemInfoWrapper );
					if( pItemInfo->lstBuffers.IsEmpty() )
					{
						// If we deleted the last buffer
						pItemInfoWrapper->pBufferOptions = NULL;
					}
					else
					{
						// Delete the item
						DeleteItem( nTreeItem );
						delete pItemInfoWrapper;
					}
				}

				// Delete the buffer
				delete pBufferOptions;
			}
		}

		InvalidateItem( pItemInfo );

		m_pAudioPathDlg->m_pAudioPath->SyncAudiopathsInUse( 0 );

		m_pAudioPathDlg->m_pAudioPathCtrl->SwitchToCorrectPropertyPage();
	}
}

void CMultiTree::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// For some reason we periodically miss LVN_ENDLABELEDIT notifications
	// when drag is halfway started on double-click.
	if( m_pEditCtrl )
	{
		delete m_pEditCtrl;
		m_pEditCtrl = NULL;
	}

	NMLVDISPINFO* pLVDispInfo = (NMLVDISPINFO*)pNMHDR;

	ItemInfo *pItemInfo = GetItemInfo( pLVDispInfo->item.iItem );

	CEdit* pEdit = GetEditControl();

	if( pEdit && pItemInfo )
	{
		// Subclass the edit control
		m_pEditCtrl = new CEditLabel( pItemInfo->strBandName );
		if( m_pEditCtrl )
		{
			m_pEditCtrl->SubclassWindow( pEdit->GetSafeHwnd() );
			m_pEditCtrl->SetLimitText( DMUS_MAX_NAME );
			m_pEditCtrl->SetWindowText( pItemInfo->strBandName );
		}

		// Limit length of text
		pEdit->SetLimitText( DMUS_MAX_NAME );
		*pResult = 0;
		return;
	}
	
	*pResult = 1;
}

void CMultiTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pNMHDR);

	if( m_pEditCtrl )
	{
		m_pEditCtrl->UnsubclassWindow();
		delete m_pEditCtrl;
		m_pEditCtrl = NULL;
	}

	NMLVDISPINFO* pLVDispInfo = (NMLVDISPINFO*)pNMHDR;
	ItemInfo *pItemInfo = GetItemInfo( pLVDispInfo->item.iItem );

	if( pItemInfo && m_pAudioPathDlg && m_pAudioPathDlg->m_pAudioPath
	&&	(pLVDispInfo->item.mask & LVIF_TEXT) )
	{
		// Check if the name is unique and at least one character long
		if( (_tcslen( pLVDispInfo->item.pszText ) > 0)
		&&	m_pAudioPathDlg->m_pAudioPath->IsMixgroupNameUnique( NULL, pLVDispInfo->item.pszText ) )
		{
			// Set to true once an undo state has been saved
			bool fChanged = false;

			// Name is unique - save undo state
			m_pAudioPathDlg->m_pAudioPath->SaveUndoState( fChanged, IDS_UNDO_MIXGROUP_NAME, true );

			// Update the name
			pItemInfo->strBandName = pLVDispInfo->item.pszText;
			m_pAudioPathDlg->UpdateTreeItemName( pItemInfo );
			m_pAudioPathDlg->UpdateMixGroupForPPG();
			theApp.m_pAudioPathComponent->m_pIMixGroupPageManager->RefreshData();
		}
		// If name is not unique, ignore the edit
	}

	SetFocus();

	*pResult = 0;
}

void CMultiTree::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pEditCtrl )
	{
		delete m_pEditCtrl;
		m_pEditCtrl = NULL;
	}

	CListCtrl::OnDestroy();
}

HRESULT	CMultiTree::CreateDataObjectEffect( IDataObject** ppIDataObject )
{
	// Validate our pointer to the data object
	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Initialize the data object
	*ppIDataObject = NULL;

	// Get a pointer to the first selected effect
	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	if( GetFirstSelectedThing( &pItemInfo, &pBufferOptions, NULL ) == EFFECT_COL )
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
		POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
		while( posEffect )
		{
			// Get a pointer to each effect
			BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );
			if( pBusEffectInfo->m_beiSelection == BEI_NORMAL )
			{
				pBusEffectInfo->m_EffectInfo.Write( pRiffStream );
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

void CMultiTree::InvalidateItem( ItemInfo *pItemInfo )
{
	// Redraw the item
	if( pItemInfo->lstBuffers.IsEmpty() )
	{
		InvalidateItem( m_pAudioPathDlg->FindItem( pItemInfo, NULL ) );
	}
	else
	{
		POSITION pos = pItemInfo->lstBuffers.GetHeadPosition();
		while( pos )
		{
			InvalidateItem( m_pAudioPathDlg->FindItem( pItemInfo, pItemInfo->lstBuffers.GetNext( pos ) ) );
		}
	}
}

void CMultiTree::InvalidateItem( int nItem )
{
	// Redraw the item
	if( nItem >= 0 )
	{
		RECT rectDraw;
		if( GetItemRect( nItem, &rectDraw, LVIR_BOUNDS ) )
		{
			InvalidateRect( &rectDraw, TRUE );
		}
	}
}

void BufferToString( const ItemInfo *pItem, const BufferOptions *pBufferOptions, CString &strText );

void CMultiTree::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the Effect
	ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(lpDrawItemStruct->itemData);
	if( !pItemInfoWrapper )
	{
		return;
	}

	ItemInfo *pItemInfo = pItemInfoWrapper->pItemInfo;
	if( !pItemInfo )
	{
		return;
	}

	// if we're drawing a real item and the item is to be completely
	// draw or drawn as if it is selected, then ...

	long lMixGroupWidth, lBusWidth, lBufferWidth;
	if (((LONG)(lpDrawItemStruct->itemID) >= 0)
		&&	m_pAudioPathDlg->GetColumnWidths( lMixGroupWidth, lBusWidth, lBufferWidth )
		&&	(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		bool fItemSelected = IsSelected( m_pAudioPathDlg->FindItem( pItemInfo, NULL ) );
		const bool fHasFocus = (::GetFocus() == GetSafeHwnd());

		const COLORREF crSelectedText = ::GetSysColor( COLOR_HIGHLIGHTTEXT );
		const COLORREF crSelectedBackground = ::GetSysColor( fHasFocus ? COLOR_HIGHLIGHT : COLOR_INACTIVECAPTION );
		const COLORREF crGreySelectedBackground = ::GetNearestColor( lpDrawItemStruct->hDC, RGB( 100, 100, 100 ) );
		const COLORREF crNormalText = ::GetSysColor(COLOR_WINDOWTEXT);
		const COLORREF crNormalBackground = ::GetSysColor(COLOR_WINDOW);

		const COLORREF crNewBackGround = fItemSelected ? crSelectedBackground : crNormalBackground;
		const COLORREF crOldBackGround = ::SetBkColor( lpDrawItemStruct->hDC, crNewBackGround );
		const COLORREF crNewTextColor = fItemSelected ? crSelectedText : crNormalText;
		const COLORREF crOldTextColor = ::SetTextColor( lpDrawItemStruct->hDC, crNewTextColor );

		if( fItemSelected )
		{
			// Draw the (un)selected background
			::ExtTextOut( lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &lpDrawItemStruct->rcItem, NULL, 0, NULL);
		}

		// Get horizontal scroll position
		long lScrollPos = -GetScrollPos( SB_HORZ );

		// Initialize the text rectangle
		RECT rectText = lpDrawItemStruct->rcItem;

		// Only draw the main item name and dashed line for the first buffer
		if( pItemInfo->lstBuffers.IsEmpty()
		||	pItemInfoWrapper->pBufferOptions == pItemInfo->lstBuffers.GetHead() )
		{
			// Draw a line from the Mix Group to the bus column
			HPEN hPenLine = ::CreatePen( PS_DOT, 0, ::GetSysColor( COLOR_GRAYTEXT ) );
			if( hPenLine )
			{
				// Get the extent of the main item text
				SIZE sizeText;
				::GetTextExtentPoint32( lpDrawItemStruct->hDC, GetItemText( lpDrawItemStruct->itemID, 0 ), GetItemText( lpDrawItemStruct->itemID, 0 ).GetLength(), &sizeText );

				const long lYPos = (lpDrawItemStruct->rcItem.top + lpDrawItemStruct->rcItem.bottom) / 2;
				const HPEN hPenOld = static_cast<HPEN>(::SelectObject( lpDrawItemStruct->hDC, hPenLine ));
				::MoveToEx( lpDrawItemStruct->hDC, sizeText.cx + lScrollPos, lYPos, NULL );
				::LineTo( lpDrawItemStruct->hDC, lMixGroupWidth + lScrollPos, lYPos );
				::SelectObject( lpDrawItemStruct->hDC, hPenOld );
				::DeleteObject( hPenLine );
			}

			// Draw the main item text
			rectText.left = lScrollPos;
			rectText.right = lMixGroupWidth + lScrollPos;
			::DrawText( lpDrawItemStruct->hDC, GetItemText( lpDrawItemStruct->itemID, 0 ), -1, &rectText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS );

			// if the item is focused, draw the focus rectangle
			//if ( lpCustomDraw->uItemState & CDIS_FOCUS )
			//{
			//	::DrawFocusRect( lpDrawItemStruct->hDC, &rectText );
			//}
		}

		if( pItemInfoWrapper->pBufferOptions )
		{
			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pItemInfoWrapper->pBufferOptions;

			// Set up the top and bottom of the rectangle to draw in
			rectText.top = lpDrawItemStruct->rcItem.top;
			rectText.bottom = lpDrawItemStruct->rcItem.bottom;
			rectText.right = lpDrawItemStruct->rcItem.right;

			// Only draw effect list when the item and buffer support effects
			if( ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
			{
				// Draw the effects list
				SIZE sizeText;
				CString strEffects;
				long lTextXPos = lMixGroupWidth + lBusWidth + lBufferWidth;
				POSITION pos = pBufferOptions->lstEffects.GetHeadPosition();
				while( pos )
				{
					// Get a pointer to each effect
					BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( pos );

					// Check if this is a sending effect
					if( (GUID_DSFX_SEND == pBusEffectInfo->m_EffectInfo.m_clsidObject)
					/*||	(GUID_DSFX_STANDARD_I3DL2SOURCE == pBusEffectInfo->m_EffectInfo.m_clsidObject)*/ )
					{
						if( GUID_DSFX_SEND == pBusEffectInfo->m_EffectInfo.m_clsidObject )
						{
							// Load the "Send" text
							strEffects.LoadString( IDS_SEND_TEXT );
						}
						else
						{
							// Use the instance name
							strEffects = pBusEffectInfo->m_EffectInfo.m_strInstanceName;
						}

						if( NULL != pBusEffectInfo->m_EffectInfo.m_pSendDestinationMixGroup )
						{
							strEffects += _T(" (") + pBusEffectInfo->m_EffectInfo.m_pSendDestinationMixGroup->strBandName + _T(")");
						}
						else if( GUID_Buffer_EnvReverb == pBusEffectInfo->m_EffectInfo.m_clsidSendBuffer )
						{
							CString strReverb;
							strReverb.LoadString( IDS_BUFFER_ENVREVERB );
							strEffects += _T(" (") + strReverb + _T(")");
						}
						else
						{
							CString strExt;
							strExt.LoadString( IDS_SEND_EXTENAL );
							strEffects += strExt;
						}
					}
					else
					{
						strEffects = pBusEffectInfo->m_EffectInfo.m_strInstanceName;
					}
					::GetTextExtentPoint32( lpDrawItemStruct->hDC, strEffects, strEffects.GetLength(), &sizeText );
					pBusEffectInfo->m_lPixelWidth = sizeText.cx;

					if( !fItemSelected )
					{
						switch( pBusEffectInfo->m_beiSelection )
						{
						case BEI_NORMAL:
							::SetBkColor( lpDrawItemStruct->hDC, crSelectedBackground );
							::SetTextColor( lpDrawItemStruct->hDC, crSelectedText );
							break;
						case BEI_GREY:
							::SetBkColor( lpDrawItemStruct->hDC, crGreySelectedBackground );
							::SetTextColor( lpDrawItemStruct->hDC, crSelectedText );
							break;
						}
					}
					// Set the left edge of the text
					rectText.left = lTextXPos + lScrollPos;

					if( !fItemSelected )
					{
						const long lOldRight = rectText.right;
						if( lTextXPos + lScrollPos + sizeText.cx < rectText.right )
						{
							rectText.right = lTextXPos + lScrollPos + sizeText.cx;
						}
						// Draw the (un)selected background
						::ExtTextOut( lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &rectText, NULL, 0, NULL);
						rectText.right = lOldRight;
					}

					// Draw the text
					::DrawText( lpDrawItemStruct->hDC, strEffects, -1, &rectText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT );
					//::TextOut( lpDrawItemStruct->hDC, lTextXPos, lpDrawItemStruct->rcItem.top + 4 + lBufferVerticalOffset, strEffects, strEffects.GetLength() );

					if( !fItemSelected )
					{
						switch( pBusEffectInfo->m_beiSelection )
						{
						case BEI_NORMAL:
						case BEI_GREY:
							::SetBkColor( lpDrawItemStruct->hDC, crNewBackGround );
							::SetTextColor( lpDrawItemStruct->hDC, crNewTextColor );
							break;
						}
					}
					lTextXPos += sizeText.cx;

					// If there is another effect after this one
					if( pos )
					{
						// Draw the " | " spacer between them
						strEffects = _T(" | ");

						// Get the extent of the spacer text
						::GetTextExtentPoint32( lpDrawItemStruct->hDC, strEffects, strEffects.GetLength(), &sizeText );

						// Set the left edge to just after the text we just drew
						rectText.left = lTextXPos + lScrollPos;

						// Draw the text
						::DrawText( lpDrawItemStruct->hDC, strEffects, -1, &rectText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT );
						//::TextOut( lpDrawItemStruct->hDC, lTextXPos, lpDrawItemStruct->rcItem.top + 4 + lBufferVerticalOffset, strEffects, strEffects.GetLength() );

						// Update how far along we've drawn
						lTextXPos += sizeText.cx;

						// Update the width of the effect to include the " | " spacer
						pBusEffectInfo->m_lPixelWidth += sizeText.cx;
					}
				}
			}

			// Draw the bus list
			CString strTemp;
			BusListToString( pItemInfo, pBufferOptions, strTemp );

			if( !fItemSelected
			&&	pBufferOptions->fSelected )
			{
				::SetBkColor( lpDrawItemStruct->hDC, crSelectedBackground );
				::SetTextColor( lpDrawItemStruct->hDC, crSelectedText );
			}
			rectText.top = lpDrawItemStruct->rcItem.top;
			rectText.bottom = lpDrawItemStruct->rcItem.bottom;
			rectText.left = lMixGroupWidth + lScrollPos;
			rectText.right = lMixGroupWidth + lBusWidth + lScrollPos;
			if( !fItemSelected )
			{
				// Draw the (un)selected background
				::ExtTextOut( lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &rectText, NULL, 0, NULL);
			}

			// Draw the text
			::DrawText( lpDrawItemStruct->hDC, strTemp, -1, &rectText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS );

			// Draw the buffer text
			BufferToString( pItemInfo, pBufferOptions, strTemp );
			rectText.left = lMixGroupWidth + lBusWidth + lScrollPos;
			rectText.right = lMixGroupWidth + lBusWidth + lBufferWidth + lScrollPos;
			if( !fItemSelected )
			{
				// Draw the (un)selected background
				::ExtTextOut( lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &rectText, NULL, 0, NULL);
			}

			// Draw the txt
			::DrawText( lpDrawItemStruct->hDC, strTemp, -1, &rectText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS );

			if( !fItemSelected
			&&	pBufferOptions->fSelected )
			{
				::SetBkColor( lpDrawItemStruct->hDC, crNewBackGround );
				::SetTextColor( lpDrawItemStruct->hDC, crNewTextColor );
			}
		}
		// If the item doesn't have a buffer
		else
		{
			// Draw the bus text
			CString strTemp;
			BusListToString( pItemInfo, NULL, strTemp );

			RECT rectText;
			rectText.top = lpDrawItemStruct->rcItem.top;
			rectText.bottom = lpDrawItemStruct->rcItem.bottom;
			rectText.left = lMixGroupWidth + lScrollPos;
			rectText.right = lMixGroupWidth + lBusWidth + lScrollPos;
			::DrawText( lpDrawItemStruct->hDC, strTemp, -1, &rectText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS );

			// Draw the buffer text
			BufferToString( pItemInfo, NULL, strTemp );
			rectText.left = lMixGroupWidth + lBusWidth + lScrollPos;
			rectText.right = lMixGroupWidth + lBusWidth + lBufferWidth + lScrollPos;
			::DrawText( lpDrawItemStruct->hDC, strTemp, -1, &rectText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS );
		}

		::SetBkColor( lpDrawItemStruct->hDC, crOldBackGround );
		::SetTextColor( lpDrawItemStruct->hDC, crOldTextColor );
	}

	// if the item is focused, draw the focus rectangle
	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
	{
		::DrawFocusRect( lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem) );
	}
}

ItemInfo *CMultiTree::GetItemInfo( int nItem ) const
{
	ItemInfoWrapper *pItemInfoWrapper = reinterpret_cast<ItemInfoWrapper *>(GetItemData( nItem ));
	if( pItemInfoWrapper )
	{
		return pItemInfoWrapper->pItemInfo;
	}
	return NULL;
}
