// BandDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BandEditorDLL.h"
#include "Band.h"
#include "BandCtl.h"
#include "BandDlg.h"
#include "DlgDLS.h"
#include "DLSDesigner.h"
#include "PChannelList.h"
#include "AddRemoveDlg.h"
#include "PChannelPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "resource.cpp"
#include "resourcemaps.h"

CString CBandDlg::m_arrIntervals[12];
CString CBandDlg::m_arrNotes[12];

#define MIDI_CCHANGE    0xB0

/////////////////////////////////////////////////////////////////////////////
// CBandDlg

IMPLEMENT_DYNCREATE(CBandDlg, CFormView)


/////////////////////////////////////////////////////////////////////////////
// CBandDlg::CBandDlg

CBandDlg::CBandDlg()
	: CFormView(CBandDlg::IDD)
{
	//{{AFX_DATA_INIT(CBandDlg)
	//}}AFX_DATA_INIT

	m_pBandCtrl = NULL;
	m_dwRef = 1;
	m_dwCookie = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CBandDlg::~CBandDlg

CBandDlg::~CBandDlg()
{
}


void CBandDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBandDlg)
	DDX_Control(pDX, IDC_GRID, m_MixGrid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBandDlg, CFormView)
	//{{AFX_MSG_MAP(CBandDlg)
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR()
	ON_WM_CONTEXTMENU()
	ON_LBN_SELCHANGE(IDC_PCHANNEL_LIST, OnSelchangePchannelList)
	ON_COMMAND(IDM_PROPERTIES, OnProperties)
	ON_WM_VKEYTOITEM()
	ON_LBN_DBLCLK(IDC_PCHANNEL_LIST, OnDblClkPChannelList)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBandDlg diagnostics

#ifdef _DEBUG
void CBandDlg::AssertValid() const
{
	CFormView::AssertValid();
}

void CBandDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CBandDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CBandDlg::OnInitialUpdate

void CBandDlg::OnInitialUpdate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CFormView::OnInitialUpdate();

	LoadIntervals();
	LoadNotes();

	m_PChannelList.SubclassDlgItem(IDC_PCHANNEL_LIST, this);
	m_PChannelList.SetBandDialog(this);

	RemoveAndAddAllButtonsToGrid();

	RefreshDisplay();

}

/////////////////////////////////////////////////////////////////////////////
// CBandDlg::Create

BOOL CBandDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pBandDo = new CUndoMan;

	RegisterMidi();

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


/////////////////////////////////////////////////////////////////////////////
// CBandDlg::OnDestroy

void CBandDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	delete m_pBandDo;

	UnRegisterMidi();

	CFormView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CBandDlg::OnSetCursor

BOOL CBandDlg::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HCURSOR hCursor;

	// Cursor in border of controls or window in menu mode
	if( message == 0 )
	{
		hCursor = ::LoadCursor( NULL, IDC_ARROW );
		if( hCursor )
		{
			::SetCursor( hCursor );
		}
		return TRUE;
	}
	
	return CFormView::OnSetCursor(pWnd, nHitTest, message);
}


// all popup menu choices get funneled through this function
BOOL CBandDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdConductor* pIC = m_pBandCtrl->m_pComponent->m_pIConductor;

	// Check if not a menu message
	if( lParam )  
		return CFormView::OnCommand(wParam, lParam);

	WORD wCommand = LOWORD(wParam);

	if(wCommand == IDM_ADDRMV_PCHANNEL)
	{
		CPtrList* pInstrumentList = (CPtrList*)m_pBandCtrl->m_pBand->GetInstrumentList();
		ASSERT(pInstrumentList);
		CAddRemoveDlg addRemoveDlg;
		addRemoveDlg.SetBandDialog(this);
		addRemoveDlg.SetInstrumentList(pInstrumentList);
		addRemoveDlg.DoModal();
	}
	if(wCommand == ID_INSERT_PCHANNEL)
	{
		// Limit the PChannels to 999
		CBand* pBand = GetBand();
		ASSERT(pBand);

		int nPChannels = pBand->GetInstrumentCount();
		if(nPChannels < 999)
		{

			SaveStateForUndo("Insert PChannel");

			CInstrumentListItem* pInstrumentItem = m_pBandCtrl->m_pBand->InsertIntoInstrumentList(); // Start from the beginning
			ASSERT(pInstrumentItem);
			CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
			ASSERT(pInstrument);
			pInstrument->Send(m_pBandCtrl->m_pBand, DM_PATCH_CHANGE);
			AddPChannel(-1, pInstrumentItem);
			AddToGrid(pInstrumentItem);
			m_pBandCtrl->m_pBand->SetModifiedFlag( TRUE );

			pInstrument->SendBandUpdate(pBand);

		}
	}
	
	if(wCommand == ID_DELETE_PCHANNEL)
	{
		OnDeleteCommand();
	}

	if(wCommand == ID_EDIT_SELECTALL)
	{
		SelectAllPChannels();
	}

	return CFormView::OnCommand(wParam, lParam);
}


BEGIN_EVENTSINK_MAP(CBandDlg, CFormView)
    //{{AFX_EVENTSINK_MAP(CBandDlg)
	ON_EVENT(CBandDlg, IDC_GRID, 1 /* CursorOver */, OnCursorOverGrid, VTS_I4)
	ON_EVENT(CBandDlg, IDC_GRID, 2 /* Move */, OnMoveGrid, VTS_I4 VTS_I2 VTS_I2)
	ON_EVENT(CBandDlg, IDC_GRID, 3 /* Update */, OnUpdateGrid, VTS_UNKNOWN VTS_BOOL VTS_BOOL)
	ON_EVENT(CBandDlg, IDC_GRID, 4 /* SelectPChannel */, OnSelectPChannelGrid, VTS_I4 VTS_BOOL VTS_BOOL)
	ON_EVENT(CBandDlg, IDC_GRID, 5 /* SaveUndoState */, OnSaveUndoStateGrid, VTS_NONE)
	ON_EVENT(CBandDlg, IDC_GRID, 6 /* DisplayStatus */, OnDisplayStatusGrid, VTS_I2)
	ON_EVENT(CBandDlg, IDC_GRID, 7 /* RightClick */, OnRightClickGrid, VTS_I2 VTS_I2)
	ON_EVENT(CBandDlg, IDC_GRID, 8 /* DeleteChannel */, OnDeleteChannelGrid, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CBandDlg::OnMoveGrid(long nId, short nPan, short nVolume) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString str;
	BSTR bstr;

	str = GetPanVolumeText(nPan, nVolume);

	bstr = str.AllocSysString();
	m_pBandCtrl->m_pComponent->m_pIFramework->SetStatusBarPaneText( m_pBandCtrl->m_hKeyStatusBar, 0, bstr, TRUE );
}

void CBandDlg::OnUpdateGrid(LPUNKNOWN pIBandPChannel, BOOL bCTRLDown, BOOL bRefreshUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)pIBandPChannel;
	CDMInstrument* pInstrument = NULL;

	if(pInstrumentItem)
	{
		pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);
		if(pInstrument)
		{
			m_pBandCtrl->m_pBand->SetModifiedFlag( TRUE );
		}
	}
	else
	{
		m_PChannelList.SetSel(-1, FALSE);
		m_pBandCtrl->m_pBand->UnselectAllInstruments();
		m_PChannelList.Invalidate();
		UpdateCommonPropertiesObject();
		RefreshPChannelPropertyPage();
	}

	// Show the selected items.
	if(pInstrument && bRefreshUI)
	{
		m_PChannelList.Invalidate();
		UpdateCommonPropertiesObject();
		RefreshPChannelPropertyPage();
	}
}

void CBandDlg::DisplayStatus(long nChannel)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString str;
	BSTR bstr;
	CString csFormatStr;

	CCommonInstrumentPropertyObject& commonInstrumentObject = CInstrumentListItem::m_CommonPropertyObject;

	if(nChannel >= 0)
	{
		CDMInstrument* pInstrument = m_pBandCtrl->m_pBand->GetInstrumentFromList(nChannel);
		ASSERT(pInstrument);

		// update the pan/vol status pane
		short nPan = pInstrument->bPan;
		short nVolume = pInstrument->bVolume;
		
		str = GetPanVolumeText(nPan, nVolume);
	}
	else
		str = GetPanVolumeText(short(commonInstrumentObject.GetPan()), short(commonInstrumentObject.GetVolume()));

	bstr = str.AllocSysString();
	m_pBandCtrl->m_pComponent->m_pIFramework->SetStatusBarPaneText( m_pBandCtrl->m_hKeyStatusBar, 0, bstr, TRUE );
}


void CBandDlg::PopulatePChannelList()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CBand* pBand = m_pBandCtrl->m_pBand;
	
	CPtrList* pList = (CPtrList*)pBand->GetInstrumentList();
	POSITION position = pList->GetHeadPosition();

	int nCount = 0;
	while(position)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)pList->GetNext(position);
		ASSERT(pInstrumentItem);
		if(pInstrumentItem)
		{
			AddPChannel(nCount, pInstrumentItem);
			nCount++;
		}
	}
}

void CBandDlg::UpdatePChannelChanges(bool bResetChannelList, bool fChangeToBandProperties)
{
	if(!bResetChannelList)
	{
		RefreshSelectedItems();
	}
	else
	{
		RefreshPChannelList();
	}

	UpdateCommonPropertiesObject();

	RefreshPChannelPropertyPage(fChangeToBandProperties);
}


void CBandDlg::UpdateGridControls()
{
	// update controls in the display
	int nNumberOfInstruments = m_pBandCtrl->m_pBand->m_lstInstruments.GetCount();
	for( int i = 0; i < nNumberOfInstruments; i++ )
	{
		CDMInstrument* pInstrument = m_pBandCtrl->m_pBand->GetInstrumentByIndex(i);
		ASSERT(pInstrument);
		
		m_MixGrid.UpdateButton( pInstrument->dwPChannel);
		m_MixGrid.SetEnabled( pInstrument->dwPChannel, (pInstrument->dwFlags & DMUS_IO_INST_PATCH));
	}
}

void CBandDlg::RefreshSelectedItems()
{
	CBand* pBand = GetBand();
	ASSERT(pBand);
	if(pBand == NULL)
	{
		return;
	}

	int nSelectedChannels = pBand->GetNumberOfSelectedInstruments();
	if(nSelectedChannels == 0)
	{
		return;
	}

	CInstrumentListItem** ppInstrumentsArray = new CInstrumentListItem*[nSelectedChannels];
	ASSERT(ppInstrumentsArray);
	if(ppInstrumentsArray == NULL)
	{
		return;
	}
	pBand->GetSelectedInstruments(ppInstrumentsArray);

	for(int nCount = 0; nCount < nSelectedChannels; nCount++)
	{
		CInstrumentListItem* pInstrumentItem = ppInstrumentsArray[nCount];
		ASSERT(pInstrumentItem);
		if(pInstrumentItem)
		{
			CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
			ASSERT(pInstrument);
			if(pInstrument)
			{
				m_MixGrid.UpdateButton( pInstrument->dwPChannel);
				m_MixGrid.SetEnabled(pInstrument->dwPChannel, (pInstrument->dwFlags & DMUS_IO_INST_PATCH));
			}
		}
	}

	delete[] ppInstrumentsArray;

	m_PChannelList.Invalidate();
}


void CBandDlg::RefreshPChannelList()
{
	// Remove everything from the list and repopulate it.
	m_PChannelList.ResetContent();
	PopulatePChannelList();

	// Show the selected items.
	SetSelectedPChannels();
}


void CBandDlg::SetSelectedPChannels()
{
	int nPChannels = m_PChannelList.GetCount();

	for(int nCount = 0; nCount < nPChannels; nCount++)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*) m_PChannelList.GetItemDataPtr(nCount);
		ASSERT(pInstrumentItem);
		if(pInstrumentItem->IsSelected()) 
			m_PChannelList.SetSel(nCount); 
	}
}

CString CBandDlg::FormatForDisplay(CDMInstrument* pInstrument)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString sPChannelNumber;
	sPChannelNumber.Format("%d", pInstrument->dwPChannel + 1);

	CString sPChannelName("");

	CString sInstrumentName;

	sInstrumentName = CBandDlg::GetPatchName(m_pBandCtrl->m_pComponent, pInstrument);

	CString sOctave = GetOctave(pInstrument);
	
	return sPChannelNumber + "\t" + sPChannelName + "\t" + sInstrumentName + "\t" + sOctave;
}

void CBandDlg::UpdatePChannelList()
{
	int nItemCount = m_PChannelList.GetCount();
	
	while(nItemCount)
	{
		nItemCount = m_PChannelList.DeleteString(0);
	}

	PopulatePChannelList();
}



CString CBandDlg::GetPatchName(CBandComponent* pComponent, CDMInstrument* pInstrument)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	BInstr* pbi = NULL;
	CString str ="";

	ASSERT(pInstrument);
	if(pInstrument == NULL)
		return str;

	ASSERT(pComponent);
	if(pComponent == NULL)
		return str;

	// Check if it's a drum track
	if(pInstrument->IsDrums())
		pbi =  &abiDrums[0];
	else
		pbi = &abiGS[0];

	DWORD dwPatch = pInstrument->dwPatch;
	
	if( pComponent->IsGM(dwPatch >> 8, dwPatch & 0x7F ) )
	{
		// find the patch in the instrument database
		while( pbi->nStringId != IDS_PGMEND )
		{
			if( pbi->bPatch == (dwPatch & 0x7F)  &&
				pbi->bMSB   == MSB(dwPatch)  &&
				pbi->bLSB   == LSB(dwPatch) )
			{
				// update the program button label
				str = *pbi->pstrName;
				if( pbi->bMSB != 0  ||  pbi->bLSB != 0 )
				{
					str += " (GS)";
				}
				
				return str;
			}
			pbi++;
		}	
	}
	BOOL fGotName = FALSE;
	signed char szName[200];

	dwPatch = pInstrument->dwPatch;
	if (pInstrument->m_pDMReference)
	{
		IDMUSProdReferenceNode*  pDLSRef = pInstrument->m_pDMReference;
		IDLSQueryInstruments *pDLSQuery;
		pDLSRef->QueryInterface(IID_IDLSQueryInstruments,(void **)&pDLSQuery);
		if (pDLSQuery)
		{
			fGotName = pDLSQuery->GetInstrumentName((BYTE)MSB(dwPatch), (BYTE)LSB(dwPatch),	
													(BYTE)(dwPatch & 0x7F),	pInstrument->IsDrums(),	
													szName, 200 );
			pDLSQuery->Release();
		}
	}

	if( fGotName )
	{
		str = (char *) szName;
		str += " (DLS)";
	}
	else  
	{
		// Set the Collection reference to NULL since 
		// no collection with this MSB, LSB and Patch were found
		pInstrument->SetDLSCollection(NULL);

		dwPatch = pInstrument->dwPatch;
		str.Format( "DLS %d,%d,%d",
			MSB(dwPatch),
			LSB(dwPatch),
			dwPatch & 0x7F );
	}
	return str;
}



CString CBandDlg::GetOctave(CDMInstrument* pInstrument)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString str;

	if( pInstrument->nTranspose == 0 )  
		str = "--";
	else  
	{
		if( pInstrument->nTranspose > 0 )
			str.Format( "+%1d", pInstrument->nTranspose / 12 );
		else  
			str.Format( "%1d", pInstrument->nTranspose / 12 );
	}	

	return str;
}

void CBandDlg::GetItemRect( UINT id, CRect *prectRel, BOOL bInvalidate )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CWnd  *wnd;
	CRect  rectDlg, rectBtnScreen, rectBtnClient, rectRel;

	// get the dialog window rectangle
	GetWindowRect( &rectDlg );

	// get the item's window handle and rectangles
	wnd = GetDlgItem( id );

	if( !wnd )
	{
		return;
	}

	wnd->GetWindowRect( &rectBtnScreen );
	wnd->GetClientRect( &rectBtnClient );

	// math to get the item coordinates relative to the dialog area
	rectRel.left = rectBtnScreen.left - rectDlg.left;
	rectRel.top = rectBtnScreen.top - rectDlg.top;
	rectRel.right = rectRel.left + rectBtnClient.right;
	rectRel.bottom = rectRel.top + rectBtnClient.bottom;

	// copy it
	if( prectRel )  {
		prectRel->left = rectRel.left;
		prectRel->top = rectRel.top;
		prectRel->right = rectRel.right;
		prectRel->bottom = rectRel.bottom;
	}

	// invalidate it
	if( bInvalidate )  {
		rectRel.right += 2;
		rectRel.bottom += 2;
		InvalidateRect( &rectRel );
}	}


// Called from the CPChannelList class to update the status bar and grid on Mouse move
void CBandDlg::EnableGridButtonAndUpdateStatus(int nChannel, bool bShowSelected)
{
	if(bShowSelected)
	{
		m_MixGrid.BringToFront(nChannel);
		DisplayStatus(nChannel);
	}

	m_MixGrid.SelectButton(nChannel, bShowSelected);
}

// Called on an event fron the CGridCtrl class
void CBandDlg::OnCursorOverGrid(long nId) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_nTrackEdit = nId;
	DisplayStatus(nId - 1); // ID of a button in the grid is already +1
}

void  CBandDlg::RefreshDisplay()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Set the dialog for the common property object
	CInstrumentListItem::m_CommonPropertyObject.SetBandDialog(this);

	UpdateGridControls();

	RefreshPChannelList();

	UpdateCommonPropertiesObject();

	RefreshPChannelPropertyPage();

}


HBRUSH CBandDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	pDC->SetTextColor( RGB(0,0,0) );

	HBRUSH hbr = CFormView::OnCtlColor( pDC, pWnd, nCtlColor );
	
	if( nCtlColor == CTLCOLOR_STATIC )
	{
		int nID = pWnd->GetDlgCtrlID();

		if( nID >= IDC_NAME1
		&&  nID <= IDC_NAME16 )
		{
			CDMInstrument* pInstrument = m_pBandCtrl->m_pBand->GetInstrumentFromList(nID - IDC_NAME1);
			ASSERT(pInstrument);

			if( !(pInstrument->dwFlags & DMUS_IO_INST_PATCH) )
			{
				pDC->SetTextColor( ::GetSysColor(COLOR_BTNSHADOW) );
			}
		}
	}

	return hbr;
}

void CBandDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pWnd != NULL );
	
	if( pWnd->GetDlgCtrlID() == IDC_PCHANNEL_LIST)
	{
		int nSelectedChannels = GetBand()->GetNumberOfSelectedInstruments();
	
		CMenu menu;
		m_PChannelList.SetFocus(); 

		if( menu.LoadMenu(IDR_PCHANNELLIST_MENU))
		{
			CMenu* pPopup = menu.GetSubMenu(0 );
			if(nSelectedChannels == 0)
			{
				menu.EnableMenuItem(ID_DELETE_PCHANNEL, MF_GRAYED);
				menu.EnableMenuItem(IDM_PROPERTIES, MF_GRAYED);
			}
			pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
		}
	}
}

void CBandDlg::OnSelchangePchannelList() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// This notification is received everytime the user navigates with the arrow key,
	// regradless of whether the selection has changed or not.
	// So we go and get the selected items every time and keep the list updated.

	CInstrumentListItem* pInstrumentItem = NULL;
	CDMInstrument* pInstrument = NULL;
	int nChannels = m_PChannelList.GetCount();
	bool bShowStatusForMultipleChannels = false;

	for(int nCount=0; nCount < nChannels; nCount++)
	{

		int nSelected = m_PChannelList.GetSel(nCount);
		
		pInstrumentItem = (CInstrumentListItem*)m_PChannelList.GetItemDataPtr(nCount);
		ASSERT(pInstrumentItem);

		pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);
		
		if( nSelected > 0 && nSelected != LB_ERR)
		{
			pInstrumentItem->SetSelected(true);
			if(m_PChannelList.GetSelCount() > 1)
			{
				m_MixGrid.BringToFront(pInstrument->dwPChannel);
				m_MixGrid.SelectButton(pInstrument->dwPChannel, true);
				bShowStatusForMultipleChannels = true;
			}
			else
			{
				EnableGridButtonAndUpdateStatus(pInstrument->dwPChannel, true);
			}
		}
		else
		{
			pInstrumentItem->SetSelected(false);
			RECT itemRect;
			m_PChannelList.GetItemRect(nCount, &itemRect);
			m_PChannelList.InvalidateRect(&itemRect);
			EnableGridButtonAndUpdateStatus(pInstrument->dwPChannel, false);
		}
	}

	SendAllNotesOffIfNotPlaying();

	UpdateCommonPropertiesObject();

	RefreshPChannelPropertyPage();
	
	if(bShowStatusForMultipleChannels)
		DisplayStatus(-1);
}


void CBandDlg::OnDblClkPChannelList() 
{
	OnProperties();
}


void CBandDlg::OnProperties() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CInstrumentListItem::m_CommonPropertyObject.OnShowProperties();
}

void CBandDlg::UpdateCommonPropertiesObject()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nSelectedChannels = m_pBandCtrl->m_pBand->GetNumberOfSelectedInstruments();

	CInstrumentListItem::m_CommonPropertyObject.SetSelectedChannels(nSelectedChannels);
	
	// Do nothing if nothing is selected
	if(nSelectedChannels == 0)
		return;
	
	CInstrumentListItem** ppInstrumentsArray = new CInstrumentListItem* [nSelectedChannels];
	ASSERT(ppInstrumentsArray);

	m_pBandCtrl->m_pBand->GetSelectedInstruments(ppInstrumentsArray);

	// Reset the common properties values so we can diff for the common values again
	CInstrumentListItem::m_CommonPropertyObject.SetReset(true);
	for(int nCount=0; nCount < nSelectedChannels; nCount++)
	{
			ppInstrumentsArray[nCount]->m_CommonPropertyObject.SetProperties(this, ppInstrumentsArray[nCount]->GetInstrument());
			ppInstrumentsArray[nCount]->m_CommonPropertyObject.SetReset(false);
	}
	
	delete[] ppInstrumentsArray;	
}


/////////////////////////////////////////////////////////////////////////////
// CBandDlg::RefreshPChannelPropertyPage	

void CBandDlg::RefreshPChannelPropertyPage( bool fChangeToBandProperties )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdFramework* pFramework = m_pBandCtrl->m_pComponent->m_pIFramework;
	ASSERT(pFramework);


	if( pFramework )
	{
		IDMUSProdPropSheet* pIPropSheet;

		// Get IDMUSProdPropSheet interface
		if( SUCCEEDED ( pFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(&(CInstrumentListItem::m_CommonPropertyObject)) != S_OK)
			{
				if( fChangeToBandProperties )
				{
					CInstrumentListItem::m_CommonPropertyObject.OnShowProperties();
				}
			}
			else if( pIPropSheet->IsShowing() == S_OK)
			{
				CWnd* pWnd = GetFocus();
				pIPropSheet->RefreshActivePageByObject(&(CInstrumentListItem::m_CommonPropertyObject));
				if(pWnd)
					pWnd->SetFocus();
			}

			RELEASE(pIPropSheet);
		}
	}
}

CBand* CBandDlg::GetBand()
{
	return m_pBandCtrl->m_pBand;
}


int CBandDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch(nKey)
	{
		/*case VK_INSERT :
		{
			InsertPChannel();
			break;
		}*/
		case VK_DELETE :
		{
			OnDeleteCommand();
			
			break;
		}
		
		case 'a':
		case 'A':
		{
			if(GetKeyState(VK_CONTROL))
			{
				SelectAllPChannels();
			}
			break;
		}

		default:
		{
			break;
		}
	}
	
	return CFormView::OnVKeyToItem(nKey, pListBox, nIndex);
}

void CBandDlg::InsertPChannel()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Limit the PChannels to 999
	CBand* pBand = GetBand();
	ASSERT(pBand);

	int nPChannels = pBand->GetInstrumentCount();
	if(nPChannels < 999)
	{

		SaveStateForUndo("Insert PChannel");

		CInstrumentListItem* pInstrumentItem = m_pBandCtrl->m_pBand->InsertIntoInstrumentList(); // Start from the beginning
		ASSERT(pInstrumentItem);
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);
		pInstrument->Send(m_pBandCtrl->m_pBand, DM_PATCH_CHANGE);
		AddPChannel(-1, pInstrumentItem);
		AddToGrid(pInstrumentItem);
		m_pBandCtrl->m_pBand->SetModifiedFlag( TRUE );
		pInstrument->SendBandUpdate(pBand);
	}
}

void CBandDlg::AddPChannel(int nIndex, CInstrumentListItem* pInstrumentItem)
{
	CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
	ASSERT(pInstrument);

	CString sInstrumentDetails = FormatForDisplay(pInstrument);

	// Find a good place for this instrument
	int nChannels = m_PChannelList.GetCount();
	for(int nCount=0; nCount<nChannels && nIndex < 0; nCount++)
	{
		CInstrumentListItem* pItemInList = (CInstrumentListItem*)m_PChannelList.GetItemDataPtr(nCount);
		ASSERT(pItemInList);
		CDMInstrument* pInstInList = pItemInList->GetInstrument();
		ASSERT(pInstInList);

		if(pInstInList->dwPChannel > pInstrument->dwPChannel)
		{
			if(nCount > 0)
				nIndex = nCount;
			else
				nIndex = 0;

			break;
		}
	}

	int nInsertionIndex = m_PChannelList.InsertString(nIndex, sInstrumentDetails);
	ASSERT( nInsertionIndex != LB_ERR);

	int nErrorCode = m_PChannelList.SetItemDataPtr(nInsertionIndex, pInstrumentItem);
	
	ASSERT( nErrorCode != LB_ERR);
}

void CBandDlg::AddToGrid(CInstrumentListItem* pInstrumentItem)
{
	ASSERT(pInstrumentItem);
	CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
	ASSERT(pInstrument);

	// Every tenth channel in the group of 16 instruments is the drumkit
	if(pInstrument->dwPChannel != 9 && (pInstrument->dwPChannel)%16 != 9)
	{
		m_MixGrid.AddButton((LPUNKNOWN)pInstrumentItem);
		m_MixGrid.SetEnabled(pInstrument->dwPChannel, (pInstrument->dwFlags & DMUS_IO_INST_PATCH));
	}
	else
	{
		// Drums
		m_MixGrid.AddButton((LPUNKNOWN)pInstrumentItem);
		m_MixGrid.SetEnabled(pInstrument->dwPChannel, (pInstrument->dwFlags & DMUS_IO_INST_PATCH));
	}
}

void CBandDlg::DeleteSelectedPChannels()
{
	ASSERT(m_pBandCtrl);
	if(m_pBandCtrl == NULL)
	{
		return;
	}

	CBand* pBand = m_pBandCtrl->m_pBand;
	ASSERT(pBand);
	if(pBand == NULL)
	{
		return;
	}

	SendAllNotesOffIfNotPlaying();

	int nSelectedChannels = pBand->GetNumberOfSelectedInstruments();
		
	CInstrumentListItem** ppInstrumentsArray = new CInstrumentListItem* [nSelectedChannels];
	ASSERT(ppInstrumentsArray);
	pBand->GetSelectedInstruments(ppInstrumentsArray);

	for(int nCount=0; nCount < nSelectedChannels; nCount++)
	{
		CInstrumentListItem* pInstrumentItem = ppInstrumentsArray[nCount];
		ASSERT(pInstrumentItem);

		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);

		m_MixGrid.RemoveButton((long)pInstrument->dwPChannel);
	}
	m_PChannelList.ResetContent();
	for(nCount=0; nCount < nSelectedChannels; nCount++)
	{
		CInstrumentListItem* pInstrumentItem = ppInstrumentsArray[nCount];
		ASSERT(pInstrumentItem);

		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);
		
		CBand* pBand = GetBand();
		ASSERT(pBand);
		pInstrument->Send(pBand, DM_PATCH_CHANGE);
		pBand->DeleteFromInstrumentList(pInstrumentItem);
	}
	
	RefreshDisplay();
	int nChannelCount = m_PChannelList.GetCount();
	if(nChannelCount > 0)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)m_PChannelList.GetItemDataPtr(0);
		ASSERT(pInstrumentItem);
		if(pInstrumentItem)
		{
			pInstrumentItem->SetSelected(true);
			m_PChannelList.SetCurSel(0);
		}
	}

	delete[] ppInstrumentsArray;	

	// sync the band
	pBand->NotifyEveryone(GUID_BAND_ChangeNotifyMsg);
}

void CBandDlg::ChangePChannelNumber(CDMInstrument* pInstrument, DWORD newPChannel)
{
	if(pInstrument->dwPChannel == newPChannel)
		return;
	
	m_PChannelList.ResetContent();
	m_MixGrid.RemoveButton((long)pInstrument->dwPChannel);
	CInstrumentListItem* pInstrumentItem = GetBand()->ChangePChannelNumber(pInstrument->dwPChannel, newPChannel);
	ASSERT(pInstrumentItem);
	AddToGrid(pInstrumentItem);
	
	CString sDisplayString = FormatForDisplay(pInstrument);
	m_PChannelList.SelectString(0, sDisplayString);
}


void CBandDlg::LoadIntervals()
{
	for(int nCount = 0; nCount < 12; nCount++)
	{
		m_arrIntervals[nCount].LoadString(IDS_NO_TRANSPOSE + nCount);
	}
}

CString CBandDlg::GetInterval(int nTranspose)
{
	if(nTranspose >= 11)
	{
		nTranspose = 11;
	}
	
	if(nTranspose <= -11)
	{
		nTranspose = -11;
	}

	CString sTranspose = "";
	int nIndex = nTranspose;
	if( nIndex < 0)
	{
		sTranspose = "-";
			nIndex = -nIndex;
	}
	else if(nIndex > 0)
	{
		sTranspose = "+";
	}

	sTranspose += m_arrIntervals[nIndex];

	return sTranspose;
}


bool CBandDlg::GetTransposeValue(CString sTransposeString, LPINT pnTranspose)
{
	ASSERT(pnTranspose);
	if(pnTranspose == NULL)
	{
		return false;
	}

	bool bNegative = false;

	if(sTransposeString.GetAt(0) == '-')
	{
		bNegative = true;
		sTransposeString = sTransposeString.Right(sTransposeString.GetLength() - 1);
	}
	else if(sTransposeString.GetAt(0) == '+')
	{
		sTransposeString = sTransposeString.Right(sTransposeString.GetLength() - 1);
	}


	if(sTransposeString.GetLength() < 3 && _ttol(sTransposeString) > 0 && _ttol(sTransposeString) <= 12)
	{
		if(bNegative)
			*pnTranspose = 0 - _ttol(sTransposeString);
		else
			*pnTranspose = _ttol(sTransposeString);

		return true;
	}

	int nCount;
	for(nCount = 0; nCount < 12; nCount++)
	{
		if(m_arrIntervals[nCount].CompareNoCase(sTransposeString) == 0)
			break;
	}

	if(nCount == 12)
		return false;

	
	if(bNegative)
		*pnTranspose = 0 - nCount;
	else
		*pnTranspose = nCount;

	return true;
}

CString CBandDlg::GetPanVolumeText(short nPan, short nVolume)
{
	CString sPan;
	if(nPan >= 0)
	{
		if(nPan > 63)
			sPan.Format("Pan R%d", nPan - 63);
		else if(nPan < 63)
			sPan.Format("Pan L%d", 63 - nPan);
		else if(nPan == 63)
			sPan.Format("Pan Mid");
	}
	else
		sPan = "-- ";
	
	// update the status bar
	CString sVolume;
	if(nVolume >= 0)
		sVolume.Format(" Vol %d", nVolume);
	else
		sVolume = " --";
	
	return sPan + sVolume;
}

void CBandDlg::LoadNotes()
{
	for(int nNote = 0; nNote < 12; nNote++)
	{
		m_arrNotes[nNote].LoadString(IDS_NOTE_C + nNote);
	}
}

HRESULT CBandDlg::GetNoteValue(CString sNoteText, LPINT pnNote)
{
	ASSERT(pnNote);
	if(pnNote == NULL)
	{
		return E_POINTER;
	}

	// Take out the white spaces
	sNoteText.TrimLeft();
	sNoteText.TrimRight();

	bool bSyntaxOK = false;
	UINT nNoteIndex = 0;

	CString sRemaining = sNoteText;
	
	// If it's the note in numbers we just return what we have
	bool bAllDigits = true;
	for(int nChar = 0; nChar < sRemaining.GetLength(); nChar++)
	{
		if(_istdigit(sRemaining.GetAt(nChar)) == 0)
		{
			bAllDigits = false;
			break;
		}
	}

	if(bAllDigits)
	{
		*pnNote = atoi(sRemaining);
		return S_OK;
	}

	// Has to be a number or alphabet
	if(isalnum(sNoteText.GetAt(0)) == false)
	{
		return E_FAIL;
	}

	if(isdigit(sNoteText.GetAt(0)))
	{
		bSyntaxOK = true;
	}

	CString sFirstChar = sNoteText.GetAt(0);
	for(int nNote = 0; (nNote < 12 && bSyntaxOK == false); nNote++)
	{
		if(sFirstChar.CompareNoCase(m_arrNotes[nNote]) == 0)
		{
			nNoteIndex = nNote;
			bSyntaxOK = true;
			sRemaining = sNoteText.Right(sNoteText.GetLength() - 1);
			break;
		}
	}

	if(!bSyntaxOK)
	{
		return E_FAIL;
	}

	if(sRemaining.IsEmpty())
	{
		*pnNote = nNoteIndex;
	}

	// Sharp or a flat?
	TCHAR tchSecond = sRemaining.GetAt(0);
	if(tchSecond != '#' && (tchSecond != 'b' || tchSecond != 'B') && _istalpha(tchSecond))
	{
		return E_FAIL;
	}

	if(tchSecond == '#')
	{
		nNoteIndex++;
		CString sTemp = sRemaining.Right(sRemaining.GetLength() - 1);
		sRemaining = sTemp;
	}
	else if(tchSecond == 'b' || tchSecond == 'B')
	{
		nNoteIndex--;
		CString sTemp = sRemaining.Right(sRemaining.GetLength() - 1);
		sRemaining = sTemp;
	}

	// Do we have an octave?
	if(!sRemaining.IsEmpty())
	{
		// Check if all characters are digits...
		for(int nChar = 0; nChar < sRemaining.GetLength(); nChar++)
		{
			if(_istdigit(sRemaining.GetAt(nChar)) == 0)
			{
				return E_FAIL;
			}
		}

		int nOctave = atoi(sRemaining) * 12;
		nNoteIndex += nOctave;
	}

	*pnNote = nNoteIndex;
	return S_OK;
}


CString	CBandDlg::GetNoteText(UINT nNote)
{
	CString sOctave = "";
	int nOctave = nNote / 12;
	sOctave.Format("%d", nOctave);
	nNote -= nOctave * 12;

	CString sNote = m_arrNotes[nNote];
	CString sNoteText = sNote + sOctave;
	return sNoteText;
}



void CBandDlg::OnDeleteCommand()
{
	ASSERT(m_pBandCtrl);
	if(m_pBandCtrl == NULL)
	{
		return;
	}

	CBand* pBand = m_pBandCtrl->m_pBand;
	ASSERT(pBand);
	if(pBand == NULL)
	{
		return;
	}

	int nSelectedChannels = pBand->GetNumberOfSelectedInstruments();
	if(nSelectedChannels == 0)
	{
		return;
	}

	CWnd* pLastFocusWnd = GetFocus();
	if( AfxMessageBox( IDS_REMOVEWARNING, MB_YESNO ) == IDYES )
	{
		SaveStateForUndo("Delete PChannel");

		int nChannelCount = m_PChannelList.GetCount();
				
		if(nChannelCount > 0)
		{
			DeleteSelectedPChannels();
			pBand->SetModifiedFlag( TRUE );
		}
	}
	if(pLastFocusWnd)
	{
		pLastFocusWnd->SetFocus();
	}
}


void CBandDlg::RemoveAndAddAllButtonsToGrid()
{
	m_MixGrid.RemoveAllButtons();
	
	CPtrList* plstInstruments = &(m_pBandCtrl->m_pBand->m_lstInstruments);
	POSITION position  = plstInstruments->GetHeadPosition();
	while(position)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)plstInstruments->GetNext(position);
		ASSERT(pInstrumentItem);
		AddToGrid(pInstrumentItem);
	}
}


void CBandDlg::SaveStateForUndo(char* szStateName)
{
	// save the state for undo
	IPersistStream *pIPStream;
	m_pBandCtrl->m_pBand->QueryInterface( IID_IPersistStream, (void **) &pIPStream );
	if( pIPStream )
	{
		m_pBandCtrl->m_pBand->m_bInUndoRedo = true;
		m_pBandDo->SaveState( pIPStream, szStateName);
		m_pBandCtrl->m_pBand->m_bInUndoRedo = false;
		pIPStream->Release();
	}	
}

void CBandDlg::OnSelectPChannelGrid(long nId, BOOL bSelection, BOOL bCtrlDown) 
{

}

void CBandDlg::OnSaveUndoStateGrid() 
{
	SaveStateForUndo("Pan/Volume Changes");
}

void CBandDlg::OnDisplayStatusGrid(short nId) 
{
	DisplayStatus(nId);
}

void CBandDlg::SelectAllPChannels()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pBandCtrl);
	if(m_pBandCtrl == NULL)
	{
		return;
	}

	CBand* pBand = m_pBandCtrl->m_pBand;
	ASSERT(pBand);
	if(pBand == NULL)
	{
		return;
	}

	pBand->SelectAllInstruments();
	int nItemCount = m_PChannelList.GetCount();
	for(int nIndex = 0; nIndex < nItemCount; nIndex++)
	{
		m_PChannelList.SetSel(nIndex);
	}

	OnSelchangePchannelList();
}

void CBandDlg::RegisterMidi() 
{
	ASSERT(m_pBandCtrl->m_pComponent);
	if(m_pBandCtrl->m_pComponent == NULL)
	{
		return;
	}

	if( m_dwCookie == 0 )
	{
		REGISTER_MIDI_IN(m_pBandCtrl->m_pComponent->m_pIConductor, m_dwCookie)
	}
}

void CBandDlg::UnRegisterMidi()
{
	ASSERT(m_pBandCtrl->m_pComponent);
	if(m_pBandCtrl->m_pComponent == NULL)
	{
		return;
	}

	SendAllNotesOffIfNotPlaying();

	if( m_dwCookie != 0 )
	{
		UNREGISTER_MIDI_IN(m_pBandCtrl->m_pComponent->m_pIConductor, m_dwCookie)
		m_dwCookie = 0;
	}
}

bool CBandDlg::IsMidiRegistered()
{
	if(m_dwCookie == 0)
		return false;
	else
		return true;
}


// =====================================================================
// OnMidiMsg
//  This is received when user hits a key in the midi keybd.
// RegisterMidi() should have been called for this to work.
// =====================================================================
HRESULT CBandDlg::OnMidiMsg(REFERENCE_TIME dwTime, BYTE bStatus, BYTE bData1, BYTE bData2)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(m_pBandCtrl->m_pComponent);
	if(m_pBandCtrl->m_pComponent == NULL)
	{
		return E_FAIL;
	}

	ASSERT(m_pBandCtrl->m_pComponent->m_pIConductor);
	if(m_pBandCtrl->m_pComponent->m_pIConductor == NULL)
	{
		return E_FAIL;
	}

	PlayMIDIEvent(bStatus & 0xF0, bData1, bData2);
	return S_OK;
}


HRESULT CBandDlg::PlayMIDIEvent(BYTE bStatus, BYTE bData1, BYTE bData2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_PChannelList.GetSelCount() == 0)
	{
		return E_FAIL;
	}

	CBandComponent* pComponent = m_pBandCtrl->m_pComponent;

	IDirectMusicPerformance* pIDMPerformance = pComponent->m_pIDMPerformance;
	ASSERT(pIDMPerformance != NULL);
	if (pIDMPerformance == NULL )
	{
		return E_UNEXPECTED;
	}

	HRESULT hr = E_FAIL;
	
	int nIndex = 0;
	int nFirstInstrument = m_PChannelList.GetSelItems(1, &nIndex);
	CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)m_PChannelList.GetItemDataPtr(nIndex);
	ASSERT(pInstrumentItem);
	if(pInstrumentItem == NULL)
	{
		return E_FAIL;
	}

	CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
	ASSERT(pInstrument);
	if(pInstrument == NULL)
	{
		return E_FAIL;
	}

	REFERENCE_TIME rtNow;
	pIDMPerformance->GetLatencyTime(&rtNow);
	MUSIC_TIME mtLatency;
	pIDMPerformance->ReferenceToMusicTime( rtNow, &mtLatency );

	// If there currently is no primary segment playing, send down the selected patch
	IDirectMusicSegmentState *pSegState;
	if( FAILED( pIDMPerformance->GetSegmentState( &pSegState, mtLatency ) ) )
	{
		if( pInstrument->dwFlags & DMUS_IO_INST_PATCH )
		{
			DMUS_PATCH_PMSG *pPatchEvent;
			hr = pIDMPerformance->AllocPMsg( sizeof(DMUS_PATCH_PMSG), (DMUS_PMSG **)&pPatchEvent );
			if( FAILED( hr ) )
			{
				return hr;
			}

			ZeroMemory( pPatchEvent, sizeof(DMUS_PATCH_PMSG) );
			// PMSG fields
			pPatchEvent->dwSize = sizeof(DMUS_PATCH_PMSG);
			pIDMPerformance->GetLatencyTime( &pPatchEvent->rtTime );
		//	pPatchEvent->mtTime = 0;
			pPatchEvent->dwFlags = DMUS_PMSGF_REFTIME;
			pPatchEvent->dwPChannel = pInstrument->dwPChannel;
			pPatchEvent->dwVirtualTrackID = 1;
		//	pPatchEvent->pTool = NULL;
		//	pPatchEvent->pGraph = NULL;
			pPatchEvent->dwType = DMUS_PMSGT_PATCH;
		//	pPatchEvent->punkUser = 0;

			// PATCH fields
			const DWORD dwPatch = pInstrument->dwPatch;
			pPatchEvent->byInstrument = BYTE(dwPatch & 0x7f);
			pPatchEvent->byMSB = BYTE((dwPatch >> 16) & 0x7f);
			pPatchEvent->byLSB = BYTE((dwPatch >> 8) & 0x7f);
		//	pPatchEvent->byPad[0] = 0;

			pComponent->SendPMsg( (DMUS_PMSG *)pPatchEvent );
		}

		if( pInstrument->dwFlags & DMUS_IO_INST_PAN )
		{
			DMUS_MIDI_PMSG* pDMMidiEvent = NULL;
			hr = pIDMPerformance->AllocPMsg(sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pDMMidiEvent);
			if(FAILED(hr))
			{
				return hr;
			}

			ZeroMemory( pDMMidiEvent, sizeof(DMUS_MIDI_PMSG) );

			pDMMidiEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
			pDMMidiEvent->bStatus = MIDI_CONTROL_CHANGE;
			pDMMidiEvent->bByte1 = MIDI_CC_PAN;
			pDMMidiEvent->bByte2 = pInstrument->bPan;
			pIDMPerformance->GetLatencyTime(&pDMMidiEvent->rtTime);
			pDMMidiEvent->dwFlags = DMUS_PMSGF_REFTIME;
			pDMMidiEvent->dwPChannel = pInstrument->dwPChannel;
			pDMMidiEvent->dwVirtualTrackID = 1;
			pDMMidiEvent->dwType = DMUS_PMSGT_MIDI;

			// This converts the PChannel, using the performance's default audiopath
			pComponent->SendPMsg( (DMUS_PMSG *)pDMMidiEvent);
		}

		if( pInstrument->dwFlags & DMUS_IO_INST_VOLUME )
		{
			DMUS_MIDI_PMSG* pDMMidiEvent = NULL;
			hr = pIDMPerformance->AllocPMsg(sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pDMMidiEvent);
			if(FAILED(hr))
			{
				return hr;
			}

			ZeroMemory( pDMMidiEvent, sizeof(DMUS_MIDI_PMSG) );

			pDMMidiEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
			pDMMidiEvent->bStatus = MIDI_CONTROL_CHANGE;
			pDMMidiEvent->bByte1 = MIDI_CC_VOLUME;
			pDMMidiEvent->bByte2 = pInstrument->bVolume;
			pIDMPerformance->GetLatencyTime(&pDMMidiEvent->rtTime);
			pDMMidiEvent->dwFlags = DMUS_PMSGF_REFTIME;
			pDMMidiEvent->dwPChannel = pInstrument->dwPChannel;
			pDMMidiEvent->dwVirtualTrackID = 1;
			pDMMidiEvent->dwType = DMUS_PMSGT_MIDI;

			// This converts the PChannel, using the performance's default audiopath
			pComponent->SendPMsg( (DMUS_PMSG *)pDMMidiEvent);
		}

		if( pInstrument->dwFlags & DMUS_IO_INST_TRANSPOSE )
		{
			DMUS_TRANSPOSE_PMSG* pTransposeEvent = NULL;
			hr = pIDMPerformance->AllocPMsg(sizeof(DMUS_TRANSPOSE_PMSG), (DMUS_PMSG**)&pTransposeEvent);
			if(FAILED(hr))
			{
				return hr;
			}

			ZeroMemory( pTransposeEvent, sizeof(DMUS_TRANSPOSE_PMSG) );

			pTransposeEvent->dwSize = sizeof(DMUS_TRANSPOSE_PMSG);
			pTransposeEvent->nTranspose = pInstrument->nTranspose;
			pIDMPerformance->GetLatencyTime(&pTransposeEvent->rtTime);
			pTransposeEvent->dwFlags = DMUS_PMSGF_REFTIME;
			pTransposeEvent->dwPChannel = pInstrument->dwPChannel;
			pTransposeEvent->dwVirtualTrackID = 1;
			pTransposeEvent->dwType = DMUS_PMSGT_TRANSPOSE;

			// This converts the PChannel, using the performance's default audiopath
			pComponent->SendPMsg( (DMUS_PMSG *)pTransposeEvent);
		}

		if( pInstrument->dwFlags & DMUS_IO_INST_PITCHBENDRANGE )
		{
			//pInstrument->nPitchBendRange
			DMUS_CURVE_PMSG* pCurveEvent = NULL;
			hr = pIDMPerformance->AllocPMsg(sizeof(DMUS_CURVE_PMSG), (DMUS_PMSG**)&pCurveEvent);
			if(FAILED(hr))
			{
				return hr;
			}

			ZeroMemory( pCurveEvent, sizeof(DMUS_CURVE_PMSG) );

			pCurveEvent->dwSize = sizeof(DMUS_CURVE_PMSG);
			pIDMPerformance->GetLatencyTime(&pCurveEvent->rtTime);
			pCurveEvent->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_DX8;
			pCurveEvent->dwPChannel = pInstrument->dwPChannel;
			pCurveEvent->dwVirtualTrackID = 1;
			pCurveEvent->dwType = DMUS_PMSGT_CURVE;

			pCurveEvent->mtDuration = 1;
			pCurveEvent->nStartValue = pInstrument->nPitchBendRange << 7;
			pCurveEvent->nEndValue = pInstrument->nPitchBendRange << 7;
			pCurveEvent->bType = DMUS_CURVET_RPNCURVE;
			pCurveEvent->bCurveShape = DMUS_CURVES_INSTANT;
			pCurveEvent->wParamType = 0; // Pitchbend range

			IDirectMusicGraph *pDMGraph = NULL;
			
			hr = pIDMPerformance->QueryInterface( IID_IDirectMusicGraph, (void**) &pDMGraph );
			if ( FAILED( hr ) )
			{
				pIDMPerformance->FreePMsg( (DMUS_PMSG*)pCurveEvent );
				return hr;
			}

			// Initializes a few other DMUS_PMSG members
			pDMGraph->StampPMsg( (DMUS_PMSG*)pCurveEvent );
			pDMGraph->Release();

			// This converts the PChannel, using the performance's default audiopath
			pComponent->SendPMsg( (DMUS_PMSG *)pCurveEvent);
		}
	}
	else
	{
		pSegState->Release();
		//pSegState = NULL;
	}
	
	DMUS_MIDI_PMSG* pDMMidiEvent = NULL;
	hr = pIDMPerformance->AllocPMsg(sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pDMMidiEvent);
	if(FAILED(hr))
	{
		return hr;
	}

	ZeroMemory( pDMMidiEvent, sizeof(DMUS_MIDI_PMSG) );

	pDMMidiEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
	pDMMidiEvent->bStatus = bStatus;
	pDMMidiEvent->bByte1 = bData1;
	pDMMidiEvent->bByte2 = bData2;
	pIDMPerformance->GetLatencyTime(&rtNow);
	pDMMidiEvent->rtTime = 5000 + rtNow; // Add 0.5ms
	pDMMidiEvent->dwFlags = DMUS_PMSGF_REFTIME;
	pDMMidiEvent->dwPChannel = pInstrument->dwPChannel;
	pDMMidiEvent->dwVirtualTrackID = 1;
	pDMMidiEvent->dwType = DMUS_PMSGT_MIDI;

	// This converts the PChannel, using the performance's default audiopath
	return pComponent->SendPMsg( (DMUS_PMSG *)pDMMidiEvent);
}


HRESULT CBandDlg::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if(::IsEqualIID(riid, IID_IDMUSProdMidiInCPt)
	|| ::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = (IDMUSProdMidiInCPt *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CBandDlg::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CBandDlg::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(m_dwRef != 0);

	AfxOleUnlockApp();
    --m_dwRef;

    if(m_dwRef == 0)
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

void CBandDlg::OnRightClickGrid(short nX, short nY) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CBand* pBand = GetBand();
	ASSERT(pBand);
	if(pBand == NULL)
	{
		return;
	}

	int nSelectedChannels = pBand->GetNumberOfSelectedInstruments();

	CMenu menu;
	if( menu.LoadMenu(IDR_PCHANNELLIST_MENU))
	{
		CMenu* pPopup = menu.GetSubMenu(0 );
		if(nSelectedChannels == 0)
		{
			menu.EnableMenuItem(ID_DELETE_PCHANNEL, MF_GRAYED);
			menu.EnableMenuItem(IDM_PROPERTIES, MF_GRAYED);
		}
		pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, nX, nY, this );
	}
}

void CBandDlg::OnDeleteChannelGrid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	OnDeleteCommand();
}

void CBandDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void SendAllNotesOff( DWORD dwPChannel, IDirectMusicPerformance *pIDirectMusicPerformance )
{
	ASSERT( pIDirectMusicPerformance );

	IDirectMusicPerformance8 *pDMPerf8;
	if( SUCCEEDED( pIDirectMusicPerformance->QueryInterface( IID_IDirectMusicPerformance8, (void **)&pDMPerf8 ) ) )
	{
		IDirectMusicAudioPath *pDMAudioPath;
		if( SUCCEEDED( pDMPerf8->GetDefaultAudioPath( &pDMAudioPath ) ) )
		{
			pDMAudioPath->ConvertPChannel( dwPChannel, &dwPChannel );
			pDMAudioPath->Release();
		}
		pDMPerf8->Release();
	}

	DMUS_MIDI_PMSG *pDMMIDIEvent = NULL;
	if( SUCCEEDED( pIDirectMusicPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pDMMIDIEvent ) ) )
	{
		ZeroMemory( pDMMIDIEvent, sizeof(DMUS_MIDI_PMSG) );
		// PMSG fields
		pDMMIDIEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
		pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME;
		pDMMIDIEvent->dwPChannel = dwPChannel;
		pDMMIDIEvent->dwVirtualTrackID = 1;
		pDMMIDIEvent->dwType = DMUS_PMSGT_MIDI;

		// DMMIDIEvent fields
		pDMMIDIEvent->bStatus = MIDI_CCHANGE;
		// Send Reset All Controllers (121)
		pDMMIDIEvent->bByte1 = 121;

		pIDirectMusicPerformance->SendPMsg( (DMUS_PMSG *)pDMMIDIEvent );
		// The playback engine will release the event

		if( SUCCEEDED( pIDirectMusicPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pDMMIDIEvent ) ) )
		{
			ZeroMemory( pDMMIDIEvent, sizeof(DMUS_MIDI_PMSG) );
			// PMSG fields
			pDMMIDIEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
			pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME;
			pDMMIDIEvent->dwPChannel = dwPChannel;
			pDMMIDIEvent->dwVirtualTrackID = 1;
			pDMMIDIEvent->dwType = DMUS_PMSGT_MIDI;

			// DMMIDIEvent fields
			pDMMIDIEvent->bStatus = MIDI_CCHANGE;
			// Send All Notes Off (123)
			pDMMIDIEvent->bByte1 = 123;

			pIDirectMusicPerformance->SendPMsg( (DMUS_PMSG *)pDMMIDIEvent );
			// The playback engine will release the event
		}
	}
}

void CBandDlg::SendAllNotesOffIfNotPlaying( void )
{
	if( m_pBandCtrl
	&&	m_pBandCtrl->m_pBand
	&&	m_pBandCtrl->m_pComponent
	&&	m_pBandCtrl->m_pComponent->m_pIDMPerformance )
	{
		// If we're not playing, send all notes off to the performance engine
		IDirectMusicSegmentState *pIDirectMusicSegmentState = NULL;
		MUSIC_TIME mtNow;
		if( SUCCEEDED( m_pBandCtrl->m_pComponent->m_pIDMPerformance->GetTime( NULL, &mtNow ) )
		&&	FAILED( m_pBandCtrl->m_pComponent->m_pIDMPerformance->GetSegmentState( &pIDirectMusicSegmentState, mtNow ) ) )
		{
			int iPChannel;
			CPtrList* pList = (CPtrList*)m_pBandCtrl->m_pBand->GetInstrumentList();
			POSITION position = pList->GetHeadPosition();
			while(position)
			{
				CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)pList->GetNext(position);
				if( SUCCEEDED( pInstrumentItem->GetPChannelNumber( &iPChannel ) ) )
				{
					SendAllNotesOff( iPChannel, m_pBandCtrl->m_pComponent->m_pIDMPerformance );
				}
			}
		}
		if( pIDirectMusicSegmentState )
		{
			pIDirectMusicSegmentState->Release();
		}
	}
}
