// SignPostDialog.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "SignPostDialog.h"
#include "PersonalityCtl.h"
#include "font.h"
#include "chordio.h"
#pragma warning(default:4201)

#include "DMPPrivate.h"
#include "chorddatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static const int gnHDir = 1;
static const int gnVDir = 0;

int ConvertTwipsToPixels(int nTwips, int dir)
{
	const int nTwipsPerInch = 1440;
	int nPixelsPerInch;
	HDC hDC = ::GetDC(0);
	if(dir == gnHDir) // horiz
	{
		nPixelsPerInch = ::GetDeviceCaps(hDC, LOGPIXELSX);	
	}
	else		// vert
	{
		nPixelsPerInch = ::GetDeviceCaps(hDC, LOGPIXELSY);
	}
	int result =  (nTwips * nPixelsPerInch) / nTwipsPerInch;
	// round up, just like the telcos
	if( (result * nTwipsPerInch) < (nTwips * nPixelsPerInch) )
	{
		result++;
	}
	return result;
}



/////////////////////////////////////////////////////////////////////////////
// CSignPostDialog dialog


CSignPostDialog::CSignPostDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSignPostDialog::IDD, pParent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	EnableAutomation();

	m_bInitialized = false;
	
	m_pChordDialog = NULL;
	m_ptLastMouse.x = m_ptLastMouse.y = 1;

	m_ptXLastMousePixels = 0;
	m_ptYLastMousePixels = 0;

	m_pPropertyPage = NULL;
	m_pPropPageMgr = NULL;
	m_pIFramework = NULL;

	m_pIPersonality = NULL;
	m_pPersonalityCtrl = NULL;


	m_nLastEdit = 0;
	m_fDragging = FALSE;


	m_pITargetDataObject = 0;
	m_pISourceDataObject = 0;
	m_bInternalDrop = false;
	m_pDragImage = 0;

	m_bIgnoreClick = false;
	m_pDragChord = 0;
	
	
	m_pCopyDataObject = 0;

	m_cfChordList = 0;
	m_cfChordMapList = 0;

	m_startDragPosition.x = 0;
	m_startDragPosition.y = 0;
	
	AddRef();

	//{{AFX_DATA_INIT(CSignPostDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSignPostDialog::OnFinalRelease()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.
	
	CDialog::OnFinalRelease();
}

void CSignPostDialog::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSignPostDialog)
	DDX_Control(pDX, IDC_GRID, m_Grid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSignPostDialog, CDialog)
	//{{AFX_MSG_MAP(CSignPostDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSignPostDialog, CDialog)
	//{{AFX_DISPATCH_MAP(CSignPostDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISignPostDialog to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {2B651785-E908-11D0-9EDC-00AA00A21BA9}
static const IID IID_ISignPostDialog =
{ 0x2b651785, 0xe908, 0x11d0, { 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9 } };

BEGIN_INTERFACE_MAP(CSignPostDialog, CDialog)
	INTERFACE_PART(CSignPostDialog, IID_ISignPostDialog, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSignPostDialog message handlers

BOOL CSignPostDialog::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
	m_cfChordMapList = RegisterClipboardFormat (CF_CHORDMAP_LIST);

	m_Grid.SetCols( MAX_COLUMNS );
	m_Grid.SetRow( 0 );
//	m_Grid.SetHighLight(2);	// highlight selected only when grid has focus

	for( int nCol = 0; nCol < MAX_COLUMNS; nCol++ )
	{
		m_Grid.SetColWidth(	nCol,
									( (nCol > COL_SELECT) && (nCol < COL_1) )? 600 : 250 );

		// Set Column headers
		CString	csHeader;
		m_Grid.SetCol( nCol );

		m_Grid.SetCellTextStyle( 1 ); // Raised
		
		if( nCol == COL_SELECT )
		{
			m_Grid.SetCellFontName( "Symbol" );
			csHeader.Format( "®" );
		}
		else if ( nCol == COL_C1 ) csHeader = "C1";
		else if ( nCol == COL_C2 ) csHeader = "C2";
		else if ( nCol == COL_SP ) csHeader = "SP";
		else if ( nCol == COL_1 ) csHeader = "1";
		else if ( nCol == COL_2 ) csHeader = "2";
		else if ( nCol == COL_3 ) csHeader = "3";
		else if ( nCol == COL_4 ) csHeader = "4";
		else if ( nCol == COL_5 ) csHeader = "5";
		else if ( nCol == COL_6 ) csHeader = "6";
		else if ( nCol == COL_7 ) csHeader = "7";
		else if ( nCol == COL_A ) csHeader = "A";
		else if ( nCol == COL_B ) csHeader = "B";
		else if ( nCol == COL_C ) csHeader = "C";
		else if ( nCol == COL_D ) csHeader = "D";
		else if ( nCol == COL_E ) csHeader = "E";
		else if ( nCol == COL_F ) csHeader = "F";

		m_Grid.SetText( csHeader );
		m_Grid.SetCellAlignment( 4 );	// Centered Horiz.
	}

	m_bInitialized = true;

// Insert all the SignPosts from the SignPost list into the Grid Control
	for(	SignPost *pSignPost = m_pSignPostList->GetHead();
			pSignPost != NULL;
			pSignPost = pSignPost->GetNext() )
	{
		AddSignPostToGrid( pSignPost );
	}

	m_Grid.SetCol(COL_SP);
	m_Grid.SetRow(m_Grid.GetRows()-1);
//	m_Grid.SetCellFontBold(TRUE);
	m_Grid.SetCellFontItalic(TRUE);

	m_Grid.SetTextMatrix(m_Grid.GetRows()-1, COL_SP, "<new>");
	m_Grid.SetCol( 1 );
	m_Grid.SetRow( 1 );

	m_Grid.SetAllowBigSelection(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSignPostDialog::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CDialog::OnSize(nType, cx, cy);
	
	ResizeGrid();
}

void CSignPostDialog::SetSignPostListPtr( DWORD pSignPostList )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( pSignPostList != NULL );

	// Save a local reference to the CPersonality SignPostList
	m_pSignPostList = (SignPostList *)pSignPostList;
}

BEGIN_EVENTSINK_MAP(CSignPostDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CSignPostDialog)
	ON_EVENT(CSignPostDialog, IDC_GRID, -604 /* KeyUp */, OnKeyUpGrid, VTS_PI2 VTS_I2)
	ON_EVENT(CSignPostDialog, IDC_GRID, -607 /* MouseUp */, OnMouseUpGrid, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CSignPostDialog, IDC_GRID, -605 /* MouseDown */, OnMouseDownGrid, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CSignPostDialog, IDC_GRID, 1554 /* OLEDragOver */, OnOLEDragOverGrid, VTS_PDISPATCH VTS_PI4 VTS_PI2 VTS_PI2 VTS_PR4 VTS_PR4 VTS_PI2)
	ON_EVENT(CSignPostDialog, IDC_GRID, 1555 /* OLEDragDrop */, OnOLEDragDropGrid, VTS_PDISPATCH VTS_PI4 VTS_PI2 VTS_PI2 VTS_PR4 VTS_PR4)
	ON_EVENT(CSignPostDialog, IDC_GRID, -600 /* Click */, OnClickCMSFlexGrid, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CSignPostDialog::OnClickGrid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CRect rc;
	long lLastRow = m_Grid.GetRows()-1;  // Minus 1 because the first row is the header.
	CString csCellText;
	SignPost *pSignPost=0;

	bool addSignpost = false;

	SetFocus();
	m_pPersonalityCtrl->SetLastFocus(CPersonalityCtrl::SignPost);

	// if user clicks on row 0, deselect and return
//	TRACE("row: %d, col: %d, mouserow: %d, mousecol: %d\n",
//			m_Grid.GetRow(), m_Grid.GetCol(), m_Grid.GetMouseRow(), m_Grid.GetMouseCol());
	if(m_Grid.GetMouseRow() == 0)
	{
		m_Grid.SetRedraw(FALSE);
//		m_Grid.SetCol(COL_SP);
//		m_Grid.SetColSel(COL_SP);
		short	top = static_cast<short>(m_Grid.GetTopRow());
		short	col = static_cast<short>(m_Grid.GetCol());
		m_Grid.SetColSel(col);
		m_Grid.SetRow(top);
		m_Grid.SetRowSel(top);
		m_ptLastMouse.x = col;
		m_ptLastMouse.y = top;
		m_Grid.SetRedraw(TRUE);
		return;
	}

	// if user clicks to right of grid, ignore
	if(m_Grid.GetMouseCol() == m_Grid.GetCols() - 1)
	{
		// verify that this is actually in a col
		int x = ConvertTwipsToPixels(ComputeRowWidth(), gnHDir);
		if(m_ptXLastMousePixels > x)
		{
			m_Grid.SetCol(m_ptLastMouse.x);
			m_Grid.SetColSel(m_ptLastMouse.x);
			m_Grid.SetRow(m_ptLastMouse.y);
			m_Grid.SetRowSel(m_ptLastMouse.y);
			return;
		}
	}

	// if user clicks below the grid, ignore
	if(m_Grid.GetMouseRow() == m_Grid.GetRows() - 1)
	{
		// verify that this is actually in a col
		int y = ConvertTwipsToPixels(ComputeColHeight(), gnVDir);
		if(m_ptYLastMousePixels > y)
		{
			m_Grid.SetCol(m_ptLastMouse.x);
			m_Grid.SetColSel(m_ptLastMouse.x);
			m_Grid.SetRow(m_ptLastMouse.y);
			m_Grid.SetRowSel(m_ptLastMouse.y);
			return;
		}
	}


	// user has selected a multi-cell region,  If first cell is a signpost group box, then toggle
	// its state and set all signpost group boxes in region to toggled state.
	if( (m_Grid.GetRow() != m_Grid.GetRowSel()) || m_Grid.GetCol() != m_Grid.GetColSel())
	{
		if(	ToggleSignpostGroupBoxes() )
			goto SyncWithEngine;
		else
			return;
	}


//	TRACE( "OnClickGrid\n" );

	m_Grid.GetClientRect(rc);
	TRACE("(%d,%d),(%d,%d)\n", rc.left, rc.top, rc.right, rc.bottom);

	//NormalizeRowCol();

	// Add new SignPost to Grid
	if( m_ptLastMouse.y == lLastRow )
	{
		if( FAILED(AddSignPostToList()) )
		{
			MessageBox( "Unable to add new SignPost.", "ChordMap Designer", MB_ICONEXCLAMATION );
			goto ShowProps;
		}
		else
		{
			addSignpost = true;
		}
		// make sure that signpost chord is selected for SetSelected
		m_ptLastMouse.x = COL_SP;
		m_Grid.SetCol(COL_SP);
		m_Grid.SetColSel(COL_SP);
		pSignPost = (SignPost*)(m_Grid.GetRowData( m_ptLastMouse.y ));

		// add a new signpost, do orphan check
		m_pIPersonality->Link2CM(pSignPost);
		m_pIPersonality->Link2SP(pSignPost);
		m_pIPersonality->RefreshChordMap();
		UpdateOrphanStatus(true);

		goto ShowProps;
	}

	// If user selected 1 through F then select/unselect the cell state and update the chord
	if( (m_ptLastMouse.x >= COL_1) && (m_ptLastMouse.x <= COL_F) && (m_ptLastMouse.y != m_Grid.GetRows()-1) )
	{
		csCellText = m_Grid.GetText();

		if( csCellText == "X" )
		{
			SendEditNotification(IDS_UNDO_SignpostRemoveFromGroup);
			m_Grid.SetText( " " );
			SetAcceptChords( m_ptLastMouse.y, m_ptLastMouse.x, FALSE );
		}
		else
		{
			SendEditNotification(IDS_UNDO_SignpostAddToGroup);
			m_Grid.SetText( "X" );
			SetAcceptChords( m_ptLastMouse.y, m_ptLastMouse.x );
		}

		goto SyncWithEngine;
	}


	// If user is selecting a row then return
	if( m_ptLastMouse.x == COL_SELECT ) return;

	pSignPost = (SignPost*)(m_Grid.GetRowData( m_ptLastMouse.y ));

	if( m_ptLastMouse.x == COL_C1 || m_ptLastMouse.x == COL_C2)
	{
		ASSERT(pSignPost);
//		pSignPost->m_dwflags |= m_ptLastMouse.x == COL_C1 ? SPOST_CADENCE1 : SPOST_CADENCE2;
		UpdateCadenceChord(m_ptLastMouse.y, m_ptLastMouse.x, true);
	}
ShowProps:
	// Add this Chord to the template Property Chord.
	ASSERT( pSignPost != NULL );
	SetSelectedChord( pSignPost, m_ptLastMouse.x );
	DMusicSync(CPersonality::syncAudition);
	PreparePropChord( pSignPost );

//	RELEASE(m_pPropPageMgr);
//	m_pPropPageMgr = NULL;

	if(addSignpost || IsPropPageShowing())
	{
		if( SUCCEEDED(OnShowProperties()) )
		{
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->RefreshData();
			}
		}
	}
	m_pIPersonality->Modified() = TRUE;

	SetFocus();
return;

SyncWithEngine:
	if( m_pIPersonality )
	{
		m_pIPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
		m_pIPersonality->Modified() = TRUE;
	}
}

HRESULT CSignPostDialog::SetSelectedChord( SignPost* pSignPost, int nCol )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ChordSelection* pSelectedChord = NULL;

	ASSERT( pSignPost != NULL );
	ASSERT( m_pSignPostList != NULL );

	for( SignPost *pSPTemp = m_pSignPostList->GetHead(); pSPTemp != NULL; pSPTemp = pSPTemp->GetNext() )
	{
		pSPTemp->m_cadence[0].KeyDown() = false;
		pSPTemp->m_cadence[1].KeyDown() = false;
		pSPTemp->m_chord.KeyDown() = false;
	}

	if( nCol == COL_C1 )
	{
		pSignPost->m_cadence[0].KeyDown() = true;
		pSelectedChord = &(pSignPost->m_cadence[0]);
	}
	else if( nCol == COL_C2 )
	{
		pSignPost->m_cadence[1].KeyDown() = true;
		pSelectedChord = &(pSignPost->m_cadence[1]);
	}
	else if( nCol == COL_SP )
	{
		pSignPost->m_chord.KeyDown() = true;
		pSelectedChord = &(pSignPost->m_chord);
	}

	if(pSelectedChord)
	{
		m_pIPersonality->SetChordChanges(*dynamic_cast<DMPolyChord*>(pSelectedChord));
	}

	return S_OK;
}

HRESULT CSignPostDialog::AddSignPostToList()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSignPostList != NULL );	// The SignPost list has to have already been initialized by a call to
										// CSignPostDialog::SetSignPostListPtr().

	// Add new SignPost to internal list.
	SignPost *pSignPost = new SignPost;
	pSignPost->m_chord = m_pIPersonality->GetKeyChord();
	if( pSignPost == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Default with Cadence Chords turned off.
	pSignPost->m_dwflags ^= (SPOST_CADENCE1 | SPOST_CADENCE2);

	SendEditNotification(IDS_UNDO_InsertSignpostChord);


	// Insert the new SignPost into the list
	m_pSignPostList->AddTail( pSignPost );

	// Insert the new SignPost into the Grid.
	AddSignPostToGrid( pSignPost );

	// Download new Personality to the Engine.
	if( m_pIPersonality != NULL )
	{
		m_pIPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
		m_pIPersonality->Modified() = TRUE;
	}

	ResizeGrid();

	return S_OK;
}

HRESULT CSignPostDialog::SetAcceptChords( long lRow, long lCol, BOOL bSet )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( lRow != 0 ); // The header row does not contain SignPost information!

	SignPost* pSignPost = (SignPost*)(m_Grid.GetRowData( lRow ));
	ASSERT( pSignPost != NULL );

	DWORD lFlag = 0;

	if( lCol == COL_1 )	lFlag = SP_1;
	else if ( lCol == COL_A ) lFlag = SP_A;
	else if ( lCol == COL_B ) lFlag = SP_B;
	else if ( lCol == COL_C ) lFlag = SP_C;
	else if ( lCol == COL_D ) lFlag = SP_D;
	else if ( lCol == COL_E ) lFlag = SP_E;
	else if ( lCol == COL_F ) lFlag = SP_F;
	else if ( lCol == COL_1 ) lFlag = SP_1;
	else if ( lCol == COL_2 ) lFlag = SP_2;
	else if ( lCol == COL_3) lFlag = SP_3;
	else if ( lCol == COL_4) lFlag = SP_4;
	else if ( lCol == COL_5) lFlag = SP_5;
	else if ( lCol == COL_6) lFlag = SP_6;
	else if ( lCol == COL_7) lFlag = SP_7;


	if( bSet == FALSE )
	{
		pSignPost->m_chords &= ~lFlag;
	}
	else
	{
		pSignPost->m_chords |= lFlag;
	}

	if( m_pIPersonality != NULL )
	{
		m_pIPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
		m_pIPersonality->Modified() = TRUE;
	}

	return S_OK;
}

HRESULT CSignPostDialog::UpdateSignPostChord( int row )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	SignPost *pSignPost = (SignPost*)(m_Grid.GetRowData(row));

	m_Grid.SetRow( row );
	m_Grid.SetCol(COL_SP);

	// Add new fields to the Grid
	CString csBuffer,
			csWrkBuffer;
	char	szRootName[30];

// Store SignPost names.
	csBuffer = "";
	pSignPost->m_chord.RootToString(szRootName);
	csWrkBuffer.Format( "%s %s", szRootName, pSignPost->m_chord.Name() );
	csBuffer += csWrkBuffer;

	//UpdateSignPostChord is currently only called from paste: however if we are overwriting an existing
	// signpost then we may be orphaning chords, so we need to relink EVERYTHING!
	m_pIPersonality->LinkAll(false);
	

	m_Grid.SetTextMatrix(row, COL_SP, csBuffer);
	// set current sp font depending on whether its an orphan
	UpdateOrphanStatus(true, pSignPost);


	return S_OK;
}


HRESULT CSignPostDialog::AddSignPostToGrid( SignPost* pSignPost, int nRow)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( pSignPost != NULL );

	if( pSignPost == NULL )
	{
		return E_INVALIDARG;
	}

	int nIndex = m_Grid.GetRows() - 1;
//	m_Grid.SetRow( 1 );	// Always add to the first Row of the Grid so long lists can be maintained easier.
	m_Grid.SetRow( nIndex );

	// Add new fields to the Grid
	CString csBuffer,
			csWrkBuffer;
	char	szRootName[30];

// Store Cadence and SignPost names.
	csBuffer = "\t";
	if( pSignPost->m_dwflags & SPOST_CADENCE1 )
	{
		pSignPost->m_cadence[0].RootToString(szRootName);
		csWrkBuffer.Format( "%s %s", szRootName, pSignPost->m_cadence[0].Name() );
		csBuffer += csWrkBuffer;
	}
	csBuffer += "\t";
	if( pSignPost->m_dwflags & SPOST_CADENCE2 )
	{
		pSignPost->m_cadence[1].RootToString(szRootName);
		csWrkBuffer.Format( "%s %s", szRootName, pSignPost->m_cadence[1].Name() );
		csBuffer += csWrkBuffer;
	}

	pSignPost->m_chord.RootToString(szRootName);
	csWrkBuffer.Format( "\t%s %s", szRootName, pSignPost->m_chord.Name() );
	csBuffer += csWrkBuffer;

	csBuffer += "\t"; if( pSignPost->m_chords & SP_1 ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_2 ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_3 ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_4 ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_5 ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_6 ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_7 ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_A ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_B ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_C ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_D ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_E ) csBuffer += "X";
	csBuffer += "\t"; if( pSignPost->m_chords & SP_F ) csBuffer += "X";

	// Add a new Row to the Grid.
	VARIANT	vt;
	vt.vt = VT_I4;
	V_I4(&vt) = nIndex;
	m_Grid.AddItem( csBuffer, vt );

	// This is how we retrieve the correct SignPost from the Grid (we store it's pointer).
	m_Grid.SetRowData( nIndex, (long)pSignPost ); 

	// Horiz Center columns 1 - F
	for( int x = COL_1; x <= COL_F; x++ )
	{
		m_Grid.SetCol( x );
		m_Grid.SetCellAlignment( 4 );	// Centered Horiz.
	}
	
	if(nRow != -1)
	{
		m_ptLastMouse.x = COL_SP;
		m_ptLastMouse.y = static_cast<short>(nRow);
	}

	// set font according to orphan state
	m_Grid.SetCol(COL_SP);
	m_Grid.SetRow(nIndex);
	UpdateOrphanStatus(false, pSignPost);

	// added from mouse
	m_Grid.SetCol( m_ptLastMouse.x );
	m_Grid.SetColSel( m_ptLastMouse.x );

	nIndex = m_Grid.GetRows() - 2;
	if( m_ptLastMouse.y < nIndex )
	{
		nIndex = m_ptLastMouse.y;
	}
	m_Grid.SetRow( nIndex );
	m_Grid.SetRowSel( nIndex );

	int top = m_Grid.GetTopRow();
	m_Grid.SetTopRow(top+1);
	return S_OK;
}


void CSignPostDialog::OnKeyUpGrid(short FAR* KeyCode, short /*Shift*/) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

//	TRACE( "OnKeyUpGrid\n" );

	long nRowCount = m_Grid.GetRows();
	long lMouseCol = m_Grid.GetColSel();
	long lMouseRow = m_Grid.GetRowSel();
	bool bMove = false;

	switch( *KeyCode )
	{
		case VK_UP:
			bMove = true;
			if(lMouseRow > 1)
			{
				m_ptLastMouse.y--;
			}
			else
			{
				m_ptLastMouse.y = static_cast<short>(nRowCount-1);
			}
			break;
		case VK_DOWN:
			bMove = true;
			if(lMouseRow < nRowCount - 1)
			{
				m_ptLastMouse.y++;
			}
			else
			{
				m_ptLastMouse.y = 1;
			}
			break;
		case VK_LEFT:
			bMove = true;
			if(lMouseCol > 1)
			{
				m_ptLastMouse.x--;
			}
			else
			{
				m_ptLastMouse.x = MAX_COLUMNS - 1;
			}
			break;
		case VK_RIGHT:
			bMove = true;
			if(lMouseCol < MAX_COLUMNS - 1)
			{
				m_ptLastMouse.x++;
			}
			else
			{
				m_ptLastMouse.x = 1;
			}
			break;
		case VK_INSERT:
			m_ptLastMouse.y = static_cast<short>(lMouseRow);
//			m_ptLastMouse.x = COL_SP;
			OnClickGrid();
			break;
		case VK_DELETE:
			
			SignPost *pSignPost = (SignPost*)(m_Grid.GetRowData(m_ptLastMouse.y));

			// This is the last Entry in the Grid (ie, the Add row)
			if( pSignPost == NULL ) return;
 
			// Delete entire row if the entire row is selected
			if( m_ptLastMouse.x == COL_SELECT || m_ptLastMouse.x == COL_SP )
			{
				SendEditNotification(IDS_UNDO_DeleteSignpostChord);
				// Remove the SignPost from the list.
				m_Grid.RemoveItem( m_ptLastMouse.y );

				// Delete SignPost from the SignPost list.
				m_pSignPostList->Remove( pSignPost );
				delete pSignPost;
				m_pIPersonality->LinkAll(true);
				ResizeGrid();
				ShowPropPage(FALSE);
			}
			// or.. Remove (Inactivate) Cadence Chord 1 if it is selected
			else if( m_ptLastMouse.x == COL_C1 )
			{
				SendEditNotification(IDS_UNDO_DeleteCadenceChord);
				pSignPost->m_dwflags ^= SPOST_CADENCE1;
				m_Grid.SetText( "" );
				ShowPropPage(FALSE);
			}
			// or.. Remove (Inactivate) Cadence Chord 2 if it is selected
			else if( m_ptLastMouse.x == COL_C2 )
			{
				SendEditNotification(IDS_UNDO_DeleteCadenceChord);
				pSignPost->m_dwflags ^= SPOST_CADENCE2;
				m_Grid.SetText( "" );
				ShowPropPage(FALSE);
			}
			else break;

			if( m_pIPersonality != NULL )
			{
				m_pIPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
				m_pIPersonality->Modified() = TRUE;
			}
			break;
	}
	
	m_Grid.SetCol( m_ptLastMouse.x );
	m_Grid.SetColSel( m_ptLastMouse.x );
	m_Grid.SetRow( m_ptLastMouse.y );
	m_Grid.SetRowSel( m_ptLastMouse.y );

	if(bMove)
	{
		SignPost* pSignPost = NULL;
		if(m_ptLastMouse.x >= COL_C1 && m_ptLastMouse.x <= COL_SP)
		{
			pSignPost = (SignPost*)(m_Grid.GetRowData( m_ptLastMouse.y ));
		}
		if(pSignPost)
		{
			bool bShow = false;
			switch(m_ptLastMouse.x)
			{
			case COL_C1:
				if(pSignPost->m_dwflags & SPOST_CADENCE1)
					bShow = true;
				break;
			case COL_C2:
				if(pSignPost->m_dwflags & SPOST_CADENCE2)
					bShow = true;
				break;
			case COL_SP:
				bShow = true;
				break;
			}
			
			if(bShow)
			{
				// Add this Chord to the template Property Chord.
				ASSERT( pSignPost != NULL );
				SetSelectedChord( pSignPost, m_ptLastMouse.x );
				DMusicSync(CPersonality::syncAudition);
				PreparePropChord( pSignPost );

				if( IsPropPageShowing() )
				{
					if(SUCCEEDED(OnShowProperties()) )
					{
						if( m_pPropPageMgr )
						{
							m_pPropPageMgr->RefreshData();
						}
					}
				}
			}
			else
			{
				ShowPropPage(FALSE);
			}
		}
		else
		{
			ShowPropPage(FALSE);
		}
		SetFocus();
	}
	
}



void CSignPostDialog::NormalizeRowCol() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	long lMouseCol = m_Grid.GetMouseCol();
	long lMouseRow = m_Grid.GetMouseRow();
	long lBeginRow = m_Grid.GetRow();
	long lEndRow = m_Grid.GetRowSel();


	// Only allow one row to be selected.
	if( lBeginRow != lEndRow )
	{
		m_Grid.SetRow( lEndRow );
		m_Grid.SetRowSel( lBeginRow );
	}

	// If 'Select' column is selected then highlight entire row
	if( lMouseCol == COL_SELECT )
	{
		m_Grid.SetCol( COL_C1 );
		m_Grid.SetColSel( COL_F );
	}
	// Otherwise force single selection to where the user lifed the mouse button.
	else
	{
		m_Grid.SetCol( lMouseCol );
		m_Grid.SetColSel( lMouseCol );
		m_Grid.SetRow( lMouseRow );
		m_Grid.SetRowSel( lMouseRow );
	}

	// Don't process Row zero (Header) messages.  Instead make it easier for the user
	// to add SignPost Chords by setting the selection to the last row (scrolling as needed).
	if( m_Grid.GetRow() == 0 )
	{
		if( m_Grid.GetRowIsVisible( m_Grid.GetRows()-1 ) == FALSE )
		{
			m_Grid.SetTopRow( m_Grid.GetRows()-1 );
		}
		m_Grid.SetRow( m_Grid.GetRows()-1 );
		m_Grid.SetRowSel( m_Grid.GetRows()-1 );
		m_Grid.SetCol( COL_C1 );
		return;
	}

	//m_ptLastMouse.x = (short)(m_Grid.GetCol());
	//m_ptLastMouse.y = (short)(m_Grid.GetRow());
	m_ptLastMouse.x = (short)lMouseCol;
	m_ptLastMouse.y = (short)lMouseRow;

}

void CSignPostDialog::OnMouseUpGrid(short Button, short /*Shift*/, long x, long y) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_bIgnoreClick)
	{
		m_ptLastMouse.x = static_cast<short>(m_Grid.GetCol());
		m_ptLastMouse.y = static_cast<short>(m_Grid.GetRow());
		m_Grid.SetRowSel(m_ptLastMouse.y);
		m_Grid.SetColSel(m_ptLastMouse.x);
		return;
	}

	long row, col, rowsel, colsel;
	rowsel = m_Grid.GetRowSel();
	colsel = m_Grid.GetColSel();
	row = m_Grid.GetRow();
	col = m_Grid.GetCol();

//	TRACE( "*** OnMouseUpGrid\n" );
	RECT rtGridRect;
	GetClientRect( &rtGridRect );

	int width = rtGridRect.right - rtGridRect.left;
	int height = rtGridRect.bottom - rtGridRect.top;

	TRACE("width = %d, height = %d, x = %d, y = %d\n\n", width, height, x, y);

//	NormalizeRowCol();

	// Dispatch left mouse clicks to OnClickGrid.
	if( Button != 2 )
	{
		OnClickGrid();
		return;
	}
/*
	// Right mouse clicks on the last row are illegal Except for signpost (can paste in a signpost to create a
	// new entry in grid)
	if( m_ptLastMouse.y == m_Grid.GetRows()-1 && 
		!(m_ptLastMouse.x == COL_SP || m_ptLastMouse.x == COL_C1 || m_ptLastMouse.x == COL_C2))
	{
		return;
	}
*/


	SignPost* pSignPost = (SignPost*)(m_Grid.GetRowData( m_ptLastMouse.y ));

	if(pSignPost)
	{
		SetSelectedChord(pSignPost,  m_ptLastMouse.x);
		PreparePropChord( pSignPost);
	}

	m_Grid.SetCol(m_ptLastMouse.x);
	m_Grid.SetColSel(m_ptLastMouse.x);
	m_Grid.SetRow(m_ptLastMouse.y);
	m_Grid.SetRowSel(m_ptLastMouse.y);



//	TRACE( "Before: x:%ld,y:%ld\n", m_ptLastMouse.x, m_ptLastMouse.y );

	CMenu cMenu;
	col = m_Grid.GetCol();
	row = m_Grid.GetRow();
	// Load the SignPost RMB menu.
	if( cMenu.LoadMenu( MAKEINTRESOURCE(IDR_SIGNPOST_RMENU) ) != 0 )
	{
		CMenu *pSubMenu = cMenu.GetSubMenu(0);
		if(!pSignPost || col != COL_SP)
		{
			pSubMenu->EnableMenuItem(IDM_SIGNPOST_INSERT_CHORD, MF_GRAYED);
		}
		if(pSignPost)
		{
			switch(col)
			{
			case COL_SP:
				break;
			case COL_C1:
				if(!(pSignPost->m_dwflags & SPOST_CADENCE1))
				{
					pSubMenu->EnableMenuItem(IDM_SIGNPOST_DELETE, MF_GRAYED);
					pSubMenu->EnableMenuItem (ID_EDIT_PROPERTIES, (MF_GRAYED | MF_BYCOMMAND));
				}
				break;
			case COL_C2:
				if(!(pSignPost->m_dwflags & SPOST_CADENCE2))
				{
					pSubMenu->EnableMenuItem(IDM_SIGNPOST_DELETE, MF_GRAYED);
					pSubMenu->EnableMenuItem (ID_EDIT_PROPERTIES, (MF_GRAYED | MF_BYCOMMAND));
				}
				break;
			default:
					pSubMenu->EnableMenuItem(IDM_SIGNPOST_DELETE, MF_GRAYED);
					pSubMenu->EnableMenuItem (ID_EDIT_PROPERTIES, (MF_GRAYED | MF_BYCOMMAND));
				break;
			}
		}
		else
		{
			if(col != COL_SP)
			{
				pSubMenu->EnableMenuItem( ID_EDIT_PASTE, (MF_GRAYED | MF_BYCOMMAND) );
			}
			// no signpost
			pSubMenu->EnableMenuItem(IDM_SIGNPOST_DELETE, MF_GRAYED);
			pSubMenu->EnableMenuItem (ID_EDIT_PROPERTIES, (MF_GRAYED | MF_BYCOMMAND));
		}
		if( CanPaste() != S_OK || col > COL_SP)
		{
			pSubMenu->EnableMenuItem( ID_EDIT_PASTE, (MF_GRAYED | MF_BYCOMMAND) );
		}
		if( CanCopy() != S_OK || col > COL_SP)
		{
			pSubMenu->EnableMenuItem (ID_EDIT_COPY, (MF_GRAYED | MF_BYCOMMAND));
		}
		if( CanInsert() != S_OK || col > COL_SP)
		{
			pSubMenu->EnableMenuItem (IDM_SIGNPOST_INSERT_CHORD, (MF_GRAYED | MF_BYCOMMAND));
		}

		if( m_ptLastMouse.y == m_Grid.GetRows()-1 || col > COL_SP)
		{
			pSubMenu->EnableMenuItem (ID_EDIT_PROPERTIES, (MF_GRAYED | MF_BYCOMMAND));
		}
		if( pSubMenu != NULL )
		{
			POINT ptMouse = {x, y};
			ClientToScreen( &ptMouse );

			MSG	msg;
			if( pSubMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, (int)(ptMouse.x), (int)(ptMouse.y),
										  this ) != 0 )
			{
				m_bIgnoreClick = true;
				::PeekMessage( &msg, m_Grid.GetSafeHwnd(), 0, 0, PM_REMOVE ); // Remove the last mouse click msg from the queue.
			}
		}
	}
}

#include "ChordDialog.h"
BOOL CSignPostDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( (WORD)wParam )
	{
		case IDM_SIGNPOST_INSERT_CHORD:
			TRACE( "Sending WM_INSERT_CHORD\n" );
			if( (m_pChordDialog != NULL) && (m_ptLastMouse.y != m_Grid.GetRows()-1) )
			{				
				SignPost		*pSignPost = NULL;
				ChordSelection	*pChord = NULL;
				DWORD			dwFlags = 0;
				
				ASSERT( m_pChordDialog->m_pChordMapStrip != NULL );

				pSignPost = (SignPost*)(m_Grid.GetRowData( m_ptLastMouse.y ));
				ASSERT( pSignPost != NULL );

				switch( m_ptLastMouse.x )
				{
					// todo: replace these direct calls into chordmapstrip with window messages
					case COL_C1:
						if( pSignPost->m_dwflags & SPOST_CADENCE1 )
						{
							pChord = &(pSignPost->m_cadence[0]);
							m_pChordDialog->m_pChordMapStrip->OnWMMessage(	WM_INSERT_CHORD, CE_END,
																			(LPARAM)pChord, 0, 0 );
						}
						break;
					case COL_C2:
						if( pSignPost->m_dwflags & SPOST_CADENCE2 )
						{
							pChord = &(pSignPost->m_cadence[1]);
							m_pChordDialog->m_pChordMapStrip->OnWMMessage(	WM_INSERT_CHORD, CE_END,
																			(LPARAM)pChord, 0, 0 );
						}
						break;
					case COL_SP:
						pChord = &(pSignPost->m_chord);
						m_pChordDialog->m_pChordMapStrip->OnWMMessage(	WM_INSERT_CHORD, CE_START,
																		(LPARAM)pChord, 0, 0 );

						m_pChordDialog->m_pChordMapStrip->OnWMMessage(	WM_INSERT_CHORD, CE_END,
																		(LPARAM)pChord, 0, 0 );
						dwFlags = CE_START | CE_END;
						break;
					default:
						TRACE( "Invalid attempt to insert Chord into ChordMap.\n" );
						return FALSE;
				}


				// since we just inserted a signpost chord in chordmap, we know they're matched!
				// but use Link functions so that other matching chords are set
				m_pIPersonality->Link2CM(pSignPost);
				m_pIPersonality->Link2SP(pSignPost);
				m_pIPersonality->RefreshChordMap();
				UpdateOrphanStatus(true, pSignPost);
//				m_Grid.Refresh();	// don't need: done by UpdateOrphanStatus

				if( m_pIPersonality != NULL )
				{
					DMusicSync(CPersonality::syncPersonality + CPersonality::syncAudition);
					m_pIPersonality->Modified() = TRUE;
					SetSelectedChord( pSignPost, m_ptLastMouse.x );
					HRESULT hr = PreparePropChord( pSignPost );
					
					if(hr == S_OK && IsPropPageShowing() )
					{
						if(SUCCEEDED(OnShowProperties()) )
						{
							if( m_pPropPageMgr )
							{
								m_pPropPageMgr->RefreshData();
							}
						}
					}
					else
					{
						ShowPropPage(FALSE);
					}
					SetFocus();
				}
				return TRUE;
			}
			return FALSE;
		case IDM_SIGNPOST_ADD_CHORD:
			AddSignPostToList();
			return TRUE;
		case IDM_SIGNPOST_DELETE:
			{
				short x = VK_DELETE;
				OnKeyUpGrid(&x, 0);
				return TRUE;
			}
		case ID_EDIT_COPY:
			{
				HRESULT hr = Copy();
				return SUCCEEDED(hr) ? TRUE : FALSE;
			}
		case ID_EDIT_PASTE:
			{
				return SUCCEEDED(Paste()) ? TRUE : FALSE;
			}
		case ID_EDIT_PROPERTIES:
			if(m_ptLastMouse.y < static_cast<short>(m_Grid.GetRows() - 1)
				&& m_ptLastMouse.y > 0)
			{
				long row = m_ptLastMouse.y;
				long col = m_ptLastMouse.x;
				bool bShow = false;
				SignPost* pSignPost = 0;
				if(col > COL_SP)
					col = COL_SP;	// make sure that we select a chord rather than a group checkbox
				// make sure that chord is selected
				for(	pSignPost = m_pSignPostList->GetHead();
						pSignPost != NULL && !bShow;
						pSignPost = pSignPost->GetNext() )
				{
					long testrow = GetRowFromRowData(reinterpret_cast<DWORD>(pSignPost));
					if(testrow == row)
					{
						ASSERT( pSignPost != NULL );
						SetSelectedChord( pSignPost, col );
						PreparePropChord( pSignPost );
						bShow = true;
						break;
					}
				}

				if(bShow)
				{
					if( SUCCEEDED(OnShowProperties()) )
					{
						if( m_pPropPageMgr )
						{
							m_pPropPageMgr->RefreshData();
						}
					}
				}

				SetFocus();
				return TRUE;
			}
			return FALSE;
			break;
		default:
			ASSERT( FALSE ); // A new menu item has been added to the popup menu but no handler defined.
	}

	return CDialog::OnCommand(wParam, lParam);
}

 
void CSignPostDialog::InitializeDialog( CChordDialog* pChordDialog, CPersonality* pPersonality, CPersonalityCtrl* const pCtl )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pChordDialog = pChordDialog;
	m_pPersonalityCtrl = pCtl;

// set m_pJazzFramework to the jazz framework pointer for Property Page
	if( (m_pIFramework = m_pChordDialog->m_pPersonality->m_pComponent->m_pIFramework) != NULL )
	{
		m_pIFramework->AddRef();
	}

	m_pIPersonality = pPersonality;
	m_pIPersonality->m_pSignPostDlg = this;
	m_pIPersonality->AddRef();
}

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject functions

HRESULT STDMETHODCALLTYPE CSignPostDialog::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CChordScalePropPageData* pChordScalePropPageData = (CChordScalePropPageData *)*ppData;

	pChordScalePropPageData->m_pPropChord = &m_SelectedChord;

	if( m_pChordDialog
	&&  m_pChordDialog->m_pPersonality )
	{
		pChordScalePropPageData->m_fLockAllScales = m_pChordDialog->m_pPersonality->m_fLockAllScales;
		pChordScalePropPageData->m_fSyncLevelOneToAll = m_pChordDialog->m_pPersonality->m_fSyncLevelOneToAll;
	}
	else
	{
		pChordScalePropPageData->m_fLockAllScales = true;
		pChordScalePropPageData->m_fSyncLevelOneToAll = true;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSignPostDialog::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CChordScalePropPageData* pChordScalePropPageData = (CChordScalePropPageData *)pData;
	CPropChord* pNewChord = pChordScalePropPageData->m_pPropChord;

	BOOL fRefresh = FALSE;

	if( m_pChordDialog
	&&  m_pChordDialog->m_pPersonality )
	{
		if( pChordScalePropPageData->m_fLockAllScales != m_pChordDialog->m_pPersonality->m_fLockAllScales )
		{
			fRefresh = TRUE;
			SendEditNotification( IDS_UNDO_LOCK_SCALES );	
			m_pChordDialog->m_pPersonality->m_fLockAllScales = pChordScalePropPageData->m_fLockAllScales;
		}
		else if( pChordScalePropPageData->m_fSyncLevelOneToAll != m_pChordDialog->m_pPersonality->m_fSyncLevelOneToAll )
		{
			fRefresh = TRUE;
			SendEditNotification( IDS_UNDO_SYNC_LEVELS );	
			m_pChordDialog->m_pPersonality->m_fSyncLevelOneToAll = pChordScalePropPageData->m_fSyncLevelOneToAll;
		}
		else
		{
			SendEditNotification(IDS_UNDO_CHORDEDIT);
			ChordSelection *pSelectedChord = NULL; // the found chord.
			int	nCol = COL_SELECT;

			for(	SignPost *pSignPost = m_pSignPostList->GetHead();
					pSignPost != NULL;
					pSignPost = pSignPost->GetNext() )
			{
				// Check to see if we found the SignPost, or one of it's cadence
				// chords, that is selected.
				if( pSignPost->m_chord.KeyDown() == 1 )
				{
					nCol = COL_SP;
					pSelectedChord = &(pSignPost->m_chord);
					break;
				}
				
				if( pSignPost->m_cadence[0].KeyDown() == 1 )
				{
					if( !(pSignPost->m_dwflags & SPOST_CADENCE1) ) return E_FAIL;
					nCol = COL_C1;
					pSelectedChord = &(pSignPost->m_cadence[0]);
					break;
				}
				
				if( pSignPost->m_cadence[1].KeyDown() == 1 )
				{
					if( !(pSignPost->m_dwflags & SPOST_CADENCE2) ) return E_FAIL;
					nCol = COL_C2;
					pSelectedChord = &(pSignPost->m_cadence[1]);
					break;
				}
			}

			// Didn't find the Selected chord in the list.  This is an internal selection
			// or list management error.
			if( pSignPost == NULL )
			{
				ASSERT( 0 );
				return E_FAIL;
			}


			// Only refresh if UI elements have changed.
			// this will need to be updated when chord props can edit polychords
			int iRoot = pSelectedChord->RootIndex();


			// Only refresh if UI elements have changed.
			fRefresh = (pSelectedChord->SubChord(iRoot)->ChordRoot() != pNewChord->Base()->ChordRoot() 
									|| strcmp(pSelectedChord->Name(), pNewChord->Name() )
									|| pSelectedChord->SubChord(iRoot)->UseFlat() != pNewChord->Base()->UseFlat());

			DWORD dwChangedFlags[DMPolyChord::MAX_POLY];
			DWORD dwEditFlags = 0;	// used to determine what notifications (for undo) to send
			bool bStructureChange = false;

			for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
			{
				DMChord* pDMFrom = pNewChord->SubChord(i);
				DMChord* pDMTo = pSelectedChord->SubChord(i);

				if(i == iRoot)
				{
					bStructureChange = (pDMFrom->ChordRoot() != pDMTo->ChordRoot());
				}

				bStructureChange = bStructureChange || (pDMFrom->ChordPattern() != pDMTo->ChordPattern());

				dwChangedFlags[i] = ~pNewChord->GetUndetermined(i);
				if (dwChangedFlags[i] & UD_NAME)
				{
					dwEditFlags |= UD_NAME;
					strcpy(pSelectedChord->Name(), pNewChord->Name());
				}
				if (dwChangedFlags[i] & UD_CHORDPATTERN)
				{
					dwEditFlags |= UD_CHORDPATTERN;
					pDMTo->ChordPattern() = pDMFrom->ChordPattern();
				}
				if (dwChangedFlags[i] & UD_SCALEPATTERN)
				{
					dwEditFlags |= UD_SCALEPATTERN;
					pDMTo->ScalePattern() = pDMFrom->ScalePattern();
				}
				if(dwChangedFlags[i] & UD_INVERTPATTERN)
				{
					dwEditFlags |= UD_INVERTPATTERN;
					pDMTo->InvertPattern() = pDMFrom->InvertPattern();
				}
				if (dwChangedFlags[i] & UD_CHORDROOT)
				{
					dwEditFlags |= UD_CHORDROOT;
					pDMTo->ChordRoot() = pDMFrom->ChordRoot();
				}
				if(dwChangedFlags[i] & UD_SCALEROOT)
				{
					dwEditFlags |= UD_SCALEROOT;
					pDMTo->ScaleRoot() = pDMFrom->ScaleRoot();
				}
				if (dwChangedFlags[i] & UD_FLAT)
				{
					dwEditFlags |= UD_FLAT;
					pDMTo->UseFlat() = pDMFrom->UseFlat();
				}
				if (dwChangedFlags[i] & UD_LEVELS)
				{
					dwEditFlags |= UD_LEVELS;
					pDMTo->Levels() = pDMFrom->Levels();
				}
				if (dwChangedFlags[i] & UD_INVERT)
				{
					dwEditFlags |= UD_INVERT;
					pDMTo->Bits() &= ~CHORD_INVERT;
					pDMTo->Bits() |= (pDMFrom->Bits() & CHORD_INVERT);
				}
				if (dwChangedFlags[i] & UD_FOUR)
				{
					dwEditFlags |= UD_FOUR;
					pDMTo->Bits() &= ~CHORD_FOUR;
					pDMTo->Bits() |= (pDMFrom->Bits() & CHORD_FOUR);
				}
				if (dwChangedFlags[i] & UD_UPPER)
				{
					dwEditFlags |= UD_UPPER;
					pDMTo->Bits() &= ~CHORD_UPPER;
					pDMTo->Bits() |= (pDMFrom->Bits() & CHORD_UPPER);
				}
			}

			/* TBD: catches too many spurious edits, need to tighten undeterimate flags stuff
			if(dwEditFlags & UD_NAME)
				SendEditNotification(IDS_UNDO_ChordNameChange);
			if(dwEditFlags & UD_CHORDPATTERN)
				SendEditNotification(IDS_UNDO_ChordPatternChange);
			if(dwEditFlags & UD_SCALEPATTERN)
				SendEditNotification(IDS_UNDO_ScalePatternChange);
			if(dwEditFlags & UD_INVERTPATTERN)
				SendEditNotification(IDS_UNDO_InvertPatternChange);
			if(dwEditFlags & UD_CHORDROOT)
				SendEditNotification(IDS_UNDO_ChordRootChange);
			if(dwEditFlags & UD_SCALEROOT)
				SendEditNotification(IDS_UNDO_ScaleRootChange);
			if(dwEditFlags & UD_FLAT)
				SendEditNotification(IDS_UNDO_UseFlat);
				*/

			// if edit of structual components, then relink
			if(nCol == COL_SP && bStructureChange)
			{
				m_pIPersonality->LinkAll(false);
				fRefresh = TRUE;
			}

			m_SelectedChord = *pNewChord;


			m_pIPersonality->SetChordChanges(*dynamic_cast<DMPolyChord*>(pSelectedChord));

			if( fRefresh == TRUE )
			{
				long lRow;
				
				if( (lRow = GetRowFromRowData( (DWORD)pSignPost )) != 0 )
				{
					char szRootName[30];
					CString csBuffer;
					pSelectedChord->RootToString(szRootName);
					csBuffer.Format( "%s %s", szRootName, pSelectedChord->Name() );
					m_Grid.SetCol(nCol);
					m_Grid.SetRow(lRow);
					m_Grid.SetText( csBuffer );
					// set drawing state
					if(nCol == COL_SP)
					{
						if(pSignPost->m_dwflags & SPOST_MATCHED)
						{
							m_Grid.SetCellFontBold(TRUE);
							m_Grid.SetCellFontItalic(FALSE);
						}
						else
						{
							m_Grid.SetCellFontBold(FALSE);
							m_Grid.SetCellFontItalic(TRUE);
						}
					}
				}
			}

			// Refresh the engine with the Personality.
			ASSERT( m_pIPersonality != NULL );
			if( FAILED(m_pIPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality 
																	| CPersonality::syncAudition)) )
			{
				return E_FAIL;
			}
		}
	}

	m_pIPersonality->Modified() = TRUE;

	return S_OK;
}

///////////////////////////////////////////
// GetRowFromRowData( DWORD dwRowData )
//
// Retrieves the row that represents the SignPost given a ptr to the SignPost.
//
// Return:
// lRow == 0 on error.  Row 0 is the header and is not a valid return value.
//
#pragma warning( disable : 4018 )
long CSignPostDialog::GetRowFromRowData( DWORD dwRowData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	long lMaxRows = m_Grid.GetRows();

	for( long lRow = 1; lRow < lMaxRows; lRow++ )
	{
		if( m_Grid.GetRowData( lRow ) == dwRowData ) break;
	}

	// Error, user probably deleted the selected row while in the PropertyPage.
	if( lRow == lMaxRows )
	{
		return 0;
	}

	return lRow;
}
#pragma warning( default : 4018 )
HRESULT STDMETHODCALLTYPE CSignPostDialog::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = S_OK;
	ASSERT(m_pIFramework);
	if(m_pIFramework == NULL)
	{
		return E_FAIL;
	}
	if( m_pPropPageMgr == NULL )
	{
		CChordPropPageMgr* pPPM = new CChordPropPageMgr(m_pIFramework);
		if( NULL == pPPM ) return E_OUTOFMEMORY;
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		m_pPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
		if( FAILED(hr) )
			return hr;
	}
	//hr = m_pITimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	if( FAILED(hr) )
		return hr;
	hr = E_FAIL;

	// SetPropertyPage
	if( m_pIFramework )
	{
		IDMUSProdPropSheet* pJPS;
		if( SUCCEEDED(hr = m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet,
			(void**)&pJPS )))
		{
			if( SUCCEEDED(pJPS->SetPageManager( m_pPropPageMgr )))
			{
				m_pPropPageMgr->SetObject((IDMUSProdPropPageObject*)this);
			}
			pJPS->Show(TRUE);
			pJPS->Release();
		}
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE CSignPostDialog::OnRemoveFromPageManager( void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSignPostDialog::PreparePropChord( SignPost *pSignPost )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pSignPost != NULL );
	if( pSignPost == NULL ) return E_INVALIDARG;

	ChordSelection *pChord;

	ZeroMemory( &m_SelectedChord, sizeof(m_SelectedChord) );

	if( pSignPost->m_chord.KeyDown() == 1 ) pChord = &(pSignPost->m_chord);
	else if( pSignPost->m_cadence[0].KeyDown() == 1 )
	{
		if( !(pSignPost->m_dwflags & SPOST_CADENCE1) ) return E_FAIL;
		pChord = &(pSignPost->m_cadence[0]);
	}
	else if( pSignPost->m_cadence[1].KeyDown() == 1 )
	{
		if( !(pSignPost->m_dwflags & SPOST_CADENCE2) ) return E_FAIL;
		pChord = &(pSignPost->m_cadence[1]);
	}
	else return E_FAIL; // One Chord from this SignPost must have been selected.

//		ChordToPropChord( &tempChord, pChord );
	dynamic_cast<DMPolyChord&>(m_SelectedChord) = *pChord;


	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostDialog IUnknown implementation for IDMUSProdPropPageObject support

HRESULT CSignPostDialog::QueryInterface( REFIID riid, void** ppv)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        AddRef();
        *ppv = (IDMUSProdPropPageObject *)this;
        return S_OK;
    }
	else if (IsEqualIID(riid, IID_IDropSource))
	{
		AddRef();
		*ppv = (IDropSource *) this;
		return S_OK;
	}
/*
	else if (IsEqualIID(riid, IID_IDropTarget))
	{
		AddRef();
		*ppv = (IDropTarget *) this;
		return S_OK;
	}
*/
    *ppv = NULL;
    return E_NOINTERFACE;
}

ULONG CSignPostDialog::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CSignPostDialog::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

	TRACE( "CSignPostDialog: %ld\n", m_dwRef );

    if( m_dwRef == 0 )
    {
		TRACE( "CSignPostDialog: CSignPostDialog destroyed!\n" );
        delete this;
        return 0;
    }

    return m_dwRef;
}

void CSignPostDialog::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIFramework )
	{
		IDMUSProdPropSheet* pJPS;
		if( SUCCEEDED(m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet,
			(void**)&pJPS )))
		{
			pJPS->RemovePageManagerByObject( (IDMUSProdPropPageObject*)this );
			pJPS->Release();
		}
	}

	m_pIPersonality->m_pSignPostDlg = 0;	// so personality doesn't call me

	RELEASE( m_pIPersonality );
	m_pIPersonality = NULL;
	RELEASE( m_pPropPageMgr );
	m_pPropPageMgr = NULL;

	RELEASE( m_pIFramework );
	m_pIFramework = NULL;

	if( m_pCopyDataObject )
	{
		if( S_OK == OleIsCurrentClipboard( m_pCopyDataObject ))
		{
			OleFlushClipboard();
		}
		m_pCopyDataObject->Release();
		m_pCopyDataObject = NULL;
	}

	CDialog::OnDestroy();
}



void CSignPostDialog::ResizeGrid()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CWnd* pGrid = GetDlgItem( IDC_GRID );
	if(pGrid)
	{
		RECT sprect;
		GetClientRect(&sprect);
		MapWindowPoints(pGrid, &sprect);
		pGrid->MoveWindow(&sprect);
	}
}


void CSignPostDialog::TwipsToPixels(POINT& pt)
{
	CDC* pdc = m_Grid.GetDC();
//	pdc->CreateCompatibleDC(NULL);
	pdc->SetMapMode( MM_TWIPS );
	pdc->LPtoDP(&pt);
	m_Grid.ReleaseDC(pdc);

}

void CSignPostDialog::PixelsToTwips(POINT& pt)
{
	CDC* pdc = m_Grid.GetDC();
	pdc->SetMapMode( MM_TWIPS );
	pdc->DPtoLP(&pt);
	m_Grid.ReleaseDC(pdc);
}

void CSignPostDialog::RowColFromPoint(POINT& pt, int& row, int& col)
{
	// since positive y is up (normal coordinates), but top is origin
	// make sure y is negative (all offsets into control should be negative)
	pt.y = -abs(pt.y);
	PixelsToTwips(pt);
//	long linewidth = m_Grid.GetGridLineWidth();
//	POINT q;
//	q.x = linewidth;
//	PixelsToTwips(q);

//	long xfudge = 12;
//	long yfudge = 30;
	long xfudge = 0;
	long yfudge = 0;

	long x,y;
	// find col
	int lb,ub;
	for(col = 0; col < m_Grid.GetCols(); col++)
	{
		x = pt.x + xfudge*col;
		lb = m_Grid.GetColPos(col);
		ub = lb + m_Grid.GetColWidth(col);
		if(lb <= x && x < ub)
			break;
	}
	if( col == m_Grid.GetCols())
		col--;
//	pos = 0;
	for(row = 0; row < m_Grid.GetRows(); row++)
	{
		y = pt.y + yfudge*row;
		lb = m_Grid.GetRowPos(row);
		ub = lb + m_Grid.GetRowHeight(row);
		if(lb <= y && y < ub)
			break;
	}
	if( row == m_Grid.GetRows())
		row--;
}

void CSignPostDialog::RowColFromTwips(POINT& pt, int& row, int& col)
{
	pt.y = -pt.y;
	long x,y;
	// find col
	int lb,ub;
	for(col = 0; col < m_Grid.GetCols(); col++)
	{
		x = pt.x;
		lb = m_Grid.GetColPos(col);
		ub = lb + m_Grid.GetColWidth(col);
		if(lb <= x && x < ub)
			break;
	}
	if( col == m_Grid.GetCols())
		col--;
//	pos = 0;
	for(row = 0; row < m_Grid.GetRows(); row++)
	{
		y = pt.y;
		lb = m_Grid.GetRowPos(row);
		ub = lb + m_Grid.GetRowHeight(row);
		if(lb <= y && y < ub)
			break;
	}
	if( row == m_Grid.GetRows())
		row--;
}

void CSignPostDialog::OnMouseDownGrid(short Button, short Shift, long x, long y) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(Shift);
//	UNREFERENCED_PARAMETER(x);
	UNREFERENCED_PARAMETER(y);

	m_ptXLastMousePixels = x;
	m_ptYLastMousePixels = y;

	m_bIgnoreClick = false;

	ASSERT(m_pChordDialog);
	m_pChordDialog->m_pPersonalityCtrl->SetLastFocus(CPersonalityCtrl::SignPost);
	long row, col;
//	row = m_Grid.GetRow();
//	col = m_Grid.GetCol();
	row = m_Grid.GetMouseRow();
	col = m_Grid.GetMouseCol();
	if(row == 0 || col == 0)
	{
		m_bIgnoreClick = true;
		return;
	}
	m_ptLastMouse.x = static_cast<short>(col);
	m_ptLastMouse.y = static_cast<short>(row);
	if(row < m_Grid.GetRows() - 1)
	{
		bool bDrag = false;
		bool bDrop = false;

		// determine if chord mouse is on is selected
		SignPost *pSignPost = (SignPost*)(m_Grid.GetRowData(row));
		switch(col)
		{
		case COL_C1:
			if(pSignPost->m_dwflags & SPOST_CADENCE1)
			{
				bDrag |= pSignPost->m_cadence[0].KeyDown();
			}
			break;
		case COL_C2:
			if(pSignPost->m_dwflags & SPOST_CADENCE2)
			{
				bDrag |= pSignPost->m_cadence[1].KeyDown();
			}
			break;
		case COL_SP:
			bDrag |= pSignPost->m_chord.KeyDown();
			break;
		}

		// anything selected, yes->start drag
		if((Button & MK_LBUTTON) && bDrag)
		{
			m_fDragging = TRUE;
			bDrop = true;
			IDropSource* pIDropSource;
			DWORD dwEffect = DROPEFFECT_NONE;
			if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
			{
				// create data object and image
				POINTS pt;
				pt.x = static_cast<short>(col);
				pt.y = static_cast<short>(row);
				HRESULT hr = CreateDataObject(&m_pISourceDataObject, pt);
				if(SUCCEEDED(hr))
				{
					m_pDragImage = CreateDragImage();
					if(m_pDragImage)
					{
						m_pDragImage->BeginDrag(0, CPoint(8,12));
					}
					// start drag, drop. NB: Only DragCopy allowed
					DWORD dwOKDragEffects = DROPEFFECT_COPY;
					m_dwStartDragButton = Button;
					m_startDragPosition.x = col;
					m_startDragPosition.y = row;
					m_bInternalDrop = false;
					hr = ::DoDragDrop(m_pISourceDataObject,
										pIDropSource,
										dwOKDragEffects,
										&dwEffect);
					// drag drop completed -- cleanup
					m_startDragPosition.x = 0;
					m_startDragPosition.y = 0;
					m_dwStartDragButton = 0;
					if(m_pDragImage)
					{
						m_pDragImage->EndDrag();
						delete m_pDragImage;
						m_pDragImage = NULL;
					}
					switch(hr)
					{
					case DRAGDROP_S_DROP:
						if(m_bInternalDrop)
						{
							m_bInternalDrop = false;
							m_nLastEdit = IDS_COPYSIGNPOST;
							InternalCopy(m_ptDrop);
						}
						else if(dwEffect == DROPEFFECT_NONE)
						{
							bDrop = false;
						}
						break;
					default:
						bDrop = false;
						break;
					}
					hr = S_OK;
					m_pISourceDataObject->Release();
					m_pISourceDataObject = NULL;
				}
				pIDropSource->Release();
			}
		}
		if( !bDrop && IsPropPageShowing())
		{
			SignPost* pSignPost = (SignPost*)(m_Grid.GetRowData( m_ptLastMouse.y ));
			int col = m_ptLastMouse.x;
			bool bShow = false;
			if(m_ptLastMouse.x != COL_SP)
			{
				// check to see if mouse on a valid prop page object
				if(m_ptLastMouse.x == COL_C1)
				{
					if(pSignPost->m_dwflags & SPOST_CADENCE1)
					{
						bShow = true;
					}
				}
				else if (m_ptLastMouse.x == COL_C2)
				{
					if(pSignPost->m_dwflags & SPOST_CADENCE2)
					{
						bShow = true;
					}
				}
			}
			else
			{
				bShow = true;
			}
			if(bShow)
			{

				SetSelectedChord(pSignPost,  col);
				PreparePropChord( pSignPost);
				
				if(SUCCEEDED(OnShowProperties()) )
				{
					if( m_pPropPageMgr )
					{
						m_pPropPageMgr->RefreshData();
					}
				}
			}
			else
			{
				ShowPropPage(FALSE);
			}

		}
//		if(m_ptLastMouse.x == COL_SP ||m_ptLastMouse.x == COL_C1 || m_ptLastMouse.x == COL_C2)
//		{
//			OnClickGrid();
//		}
	}
}

HRESULT CSignPostDialog::UpdateCadenceChord(int nRow, int nCol, bool bVal)
{
	// Add new fields to the Grid
	CString csBuffer;
	char	szRootName[30];

	csBuffer = "";
	SignPost *pSignPost = (SignPost*)(m_Grid.GetRowData(nRow));

	if(nCol == COL_C1)
	{
		if(bVal)
		{
			if(!(pSignPost->m_dwflags & SPOST_CADENCE1))
			{
				SendEditNotification(IDS_UNDO_SignpostAddCadenceChord);
				pSignPost->m_dwflags |= SPOST_CADENCE1;
				pSignPost->m_cadence[0] = m_pIPersonality->GetKeyChord();
				pSignPost->m_cadence[0].RootToString(szRootName);
				csBuffer.Format( "%s %s", szRootName, pSignPost->m_cadence[0].Name() );
			}
			else
			{
				SendEditNotification(IDS_UNDO_SignpostAddCadenceChord);
				pSignPost->m_cadence[0].RootToString(szRootName);
				csBuffer.Format( "%s %s", szRootName, pSignPost->m_cadence[0].Name() );
			}
		}
		else
		{
			SendEditNotification(IDS_UNDO_DeleteCadenceChord);
			pSignPost->m_dwflags &= ~SPOST_CADENCE1;
		}
	}
	else if(nCol == COL_C2)
	{
		if(bVal)
		{
			if(!(pSignPost->m_dwflags & SPOST_CADENCE2))
			{
				SendEditNotification(IDS_UNDO_SignpostAddCadenceChord);
				pSignPost->m_cadence[1] = m_pIPersonality->GetKeyChord();
				pSignPost->m_cadence[1].RootToString(szRootName);
				csBuffer.Format( "%s %s", szRootName, pSignPost->m_cadence[1].Name() );
				pSignPost->m_dwflags |= SPOST_CADENCE2;
			}
			else
			{
				SendEditNotification(IDS_UNDO_SignpostAddCadenceChord);
				pSignPost->m_cadence[1].RootToString(szRootName);
				csBuffer.Format( "%s %s", szRootName, pSignPost->m_cadence[1].Name() );
			}
		}
		else
		{
			SendEditNotification(IDS_UNDO_DeleteCadenceChord);
			pSignPost->m_dwflags &= ~SPOST_CADENCE2;
		}
	}
	else
	{
		E_FAIL;
	}

	m_Grid.SetTextMatrix(nRow, nCol, csBuffer);

	return S_OK;
}


/////////////// IDropSource Methods
// IDropSource Methods
HRESULT CSignPostDialog::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
 
	if( fEscapePressed )
	{
        return DRAGDROP_S_CANCEL;
	}

	if( m_dwStartDragButton & MK_LBUTTON )
	{
		if( grfKeyState & MK_RBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}

		if( !(grfKeyState & MK_LBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	if( m_dwStartDragButton & MK_RBUTTON )
	{
		if( grfKeyState & MK_LBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}
		
		if( !(grfKeyState & MK_RBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	return S_OK;
}

HRESULT CSignPostDialog::GiveFeedback( DWORD dwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	UNREFERENCED_PARAMETER(dwEffect);

	if( m_pDragImage )
	{
		CPoint pt;

		GetCursorPos( &pt );

		// Move the drag image
		m_pDragImage->DragMove( pt );
	}

	return DRAGDROP_S_USEDEFAULTCURSORS;
}

CImageList* CSignPostDialog::CreateDragImage()
{
	CImageList* pimage = new CImageList;
	ASSERT(pimage);
	if(!pimage)
	{
		return 0;
	}
	pimage->Create(::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON),
					ILC_COLOR4 | ILC_MASK, 1,1);
	pimage->SetBkColor(CLR_NONE);
	HICON hIcon = ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CHORDTRUCK));
	pimage->Add(hIcon);
	return pimage;
}

HRESULT	CSignPostDialog::CreateDataObject(IDataObject** ppIDataObject, POINTS where)
{
	// where.x = col.  where.y = row
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Selected Chords into stream
	HRESULT hr = S_OK;
	if( SUCCEEDED (m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		ChordSelection* pChordAtDragPoint;
		SignPost* pSignPost = (SignPost*)(m_Grid.GetRowData( where.y ));
		switch(where.x)
		{
		case COL_C1:
			ASSERT(pSignPost->m_dwflags & SPOST_CADENCE1);
			pChordAtDragPoint = &(pSignPost->m_cadence[0]);
			break;
		case COL_C2:
			pChordAtDragPoint = &(pSignPost->m_cadence[1]);
			break;
		case COL_SP:
			pChordAtDragPoint = &(pSignPost->m_chord);
			break;
		default:
			ASSERT(FALSE);	// drag must start from one of above cols
			pChordAtDragPoint = 0;
			hr = E_FAIL;
			break;
		}
		m_pDragChord = pChordAtDragPoint;
		TRACE("drag chord created, addr = %p\n");
		if(!SUCCEEDED(hr))
		{
			ASSERT(FALSE);
			pIStream->Release();
			return hr;
		}
		if( SUCCEEDED ( SaveSelectedChords( pIStream, pChordAtDragPoint, TRUE ) ) )
		{
			// Place CF_CHORDLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfChordList, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		pIStream->Release();
	}



	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		pDataObject->Release();
	}

	return hr;

}

/*
void CSignPostDialog::MarkSelectedChords(DWORD flags)
{
	for(int i = 0; i < m_Grid.GetRows() - 2; i++)
	{
		SignPost* pSignPost = (SignPost*)(m_Grid.GetRowData( i ));
		ASSERT( pSignPost != NULL );
		ChordSelection* pChord;
		for(int j = 0; j < SignPost::MAX_CADENCE; j++)
		{
			pChord = &(pSignPost->m_cadence[j]);
			if(pChord->KeyDown())
			{
				pChord->m_dwflags |= flags;
			}
		}
		pChord = pSignPost->m_chord;
		if(pChord->m_chordsel.KeyDown())
		{
			pChord->m_dwflags |= flags;
		}		
	}
}
*/

HRESULT CSignPostDialog::SaveSelectedChords(LPSTREAM pStream, ChordSelection* pChordAtDragPoint, BOOL bNormalize)
{
	// commmented out lines implement mechanism for drop/pasting signpost chords at beginning and
	// end of chord map
	ASSERT(pStream);
	ASSERT(pChordAtDragPoint);
	CChordList list;
	ChordSelection* pChord;
	CChordItem* pItem;
	// copy selected chords to list
	for(int i = 1; i < m_Grid.GetRows() - 1; i++)
	{
		SignPost* pSignPost = (SignPost*)(m_Grid.GetRowData(i));
		ASSERT( pSignPost != NULL);
		for(int j = 0; j < SignPost::MAX_CADENCE; j++)
		{

			DWORD mask = 0;
			switch(j)
			{
			case 0:
				 mask = SPOST_CADENCE1;
				 break;
			case 1:
				 mask = SPOST_CADENCE2;	// is there really a signpost there
				 break;
			default:
				ASSERT( 0 );
				break;
			}
			pChord = &(pSignPost->m_cadence[j]);
			if( (pSignPost->m_dwflags & mask)  && pChord->KeyDown())
			{
				// make sure that only the chord at the mouse drag point is selected
				pChord->KeyDown() = (pChord == pChordAtDragPoint) ? TRUE : FALSE;
				pItem = new CChordItem(*pChord);
//				pItem->m_dwUndetermined != CE_END;
				list.AddTail(pItem);
				// re-select chord
				pChord->KeyDown() = TRUE;
				// mark as ending chord
			}
		}
		pChord = &(pSignPost->m_chord);
		if(pChord->KeyDown())
		{
			// make sure that only the chord at the mouse drag point is selected
			pChord->KeyDown() = (pChord == pChordAtDragPoint) ? TRUE : FALSE;
			// add beginning signpost
			pItem = new CChordItem(*pChord);
//			pItem->m_dwUndetermined |= CE_START;
			list.AddTail(pItem);
/*			
			// add end signpost
			pItem = new CChordItem(*pChord);
			pItem->m_dwUndetermined |= CE_END;
			list.AddTail(pItem);
*/
			// reselect chord
			pChord->KeyDown() = TRUE;
		}
	}
	// check that anything is selected
	if(!list.GetHead())
		return E_FAIL;
	// normalize chords so that first chord is at offset 0
	if(bNormalize)
	{
		// here normalize means all CE_START at measure 0 beat 0
		// all CE_END at last measure of chord map
		for(pItem = list.GetHead(); pItem; pItem = pItem->GetNext())
		{
//			pItem->SetSignPost(TRUE);
			pItem->SetSignPost(FALSE);
			pItem->Beat() = 0;
			pItem->Measure() = 0;
//			if(pItem->m_dwUndetermined & CE_START)
//			{
//				pItem->m_nMeasure = 0;
//			}
//			else
//			{
//				pItem->m_nMeasure = lastmeasure;
//			}
		}

	}
	HRESULT hr = list.Save(pStream, FALSE);
	list.ClearList();
	return hr;
}

ChordSelection* CSignPostDialog::InternalCopy(POINTS ptDrop)
{
	ChordSelection *pChord = 0;
	UNREFERENCED_PARAMETER(ptDrop);
/*
	if( FAILED(m_pChordMapMgr->CreateBlankChord( &pChord )) )
	{
		return 0;
	}

	// copy contents of dragged chord
	OldChordEntry oce;
	m_pDragChord->SaveSmall(oce);
	pChord->LoadSmall(oce);

	// Set new Chord (UI Box) where the user clicked the mouse.
	if( FAILED(SetChordXY( pChord, ptDrop )) ) // If user clicks end of timeline then method should fail (it currently does not).
	{
		delete pChord;
		pChord = 0;
	}

	m_pChordMapMgr->m_pChordMapList->AddTail( pChord );
	ClearSelectedChords();
	pChord->m_chordsel.KeyDown() = TRUE;	// new inserted chord is selected
*/
	return pChord;
}


DWORD VBtoWIN32MouseState(short button, short shift)
{
	DWORD state = 0;
	if(button & 1)
		state |= MK_LBUTTON;
	if(button & 2)
		state |= MK_RBUTTON;
	if(button & 4)
		state |= MK_MBUTTON;
	if(shift & 1)
		state |= MK_SHIFT;
	if(shift & 2)
		state |= MK_CONTROL;
	if(shift & 4)
		state |= MK_ALT;

	return state;
}

/////////////////////////////////////////////////// IDropTarget Methods

/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSignPostDialog::DragEnter

HRESULT CSignPostDialog::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	TRACE("CSignPostDialog Drag Enter!");
	ASSERT( pIDataObject != NULL );
	ASSERT( m_pITargetDataObject == NULL );

	// Store IDataObject associated with current drag-drop operation
	m_pITargetDataObject = pIDataObject;
	m_pITargetDataObject->AddRef();

	if( m_pDragImage )
	{
		CPoint point( pt.x, pt.y );

		// Show the feedback image
		m_pDragImage->DragEnter( GetDesktopWindow (), point );
	}
/*
	// Make sure editor is on top
	if( m_pStyle->m_hWndEditor )
	{
		::BringWindowToTop( m_pStyle->m_hWndEditor );
	}
*/
	// Determine effect of drop
	return DragOver( grfKeyState, pt, pdwEffect );
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSignPostDialog::DragOver

HRESULT CSignPostDialog::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int row, col;


	POINT q;
	q.x = pt.x;
	q.y = pt.y;

	TRACE("DragOver pt = (%d, %d)\n", q.x, q.y);
//	ScreenToClient(&q);

	RowColFromPoint(q, row, col);
	TRACE("DragOver pt = (%d, %d), row, col = (%d, %d)\n", q.x,q.y, row, col);

	int lastrow = m_Grid.GetRows() - 1;
	bool chordcol = (col == COL_C1) || (col == COL_C2) || (col == COL_SP);
	bool chordrow = row < lastrow && row > 0;

	if(row == lastrow && col == COL_SP)
	{
		chordrow = true;
	}


	TRACE("CSignPostDialog Drag OVER!");
	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragShowNolock( FALSE );
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	
	if(chordcol && chordrow && CanPasteFromData( m_pITargetDataObject ) == S_OK )
	{
		if( grfKeyState & MK_RBUTTON )
		{
			dwEffect = *pdwEffect;
		}
		else
		{
			if( grfKeyState & MK_CONTROL )
			{
				dwEffect = DROPEFFECT_COPY;
			}
			else
			{
				if( *pdwEffect & DROPEFFECT_COPY
				&&  *pdwEffect & DROPEFFECT_MOVE )
				{
					dwEffect = DROPEFFECT_COPY;
				}
				else
				{
					dwEffect = *pdwEffect;
				}
			}
		}
	}

	if( m_pDragImage )
	{
		// Show the feedback image
		m_pDragImage->DragShowNolock( TRUE );
	}

	// Set temp drag over fields
	if( grfKeyState & (MK_RBUTTON | MK_LBUTTON) )
	{
		m_dwOverDragButton = grfKeyState & (MK_RBUTTON | MK_LBUTTON);
		m_dwOverDragEffect = dwEffect;
//		TRACE("EFFECT = %d\n", dwEffect);
	}

	*pdwEffect = dwEffect;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSignPostDialog::DragLeave

HRESULT CSignPostDialog::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	TRACE("CSignPostDialog Drag Leave!");

	// Release IDataObject
	if( m_pITargetDataObject )
	{
		m_pITargetDataObject->Release();
		m_pITargetDataObject = NULL;
	}

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetDesktopWindow () );
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSignPostDialog::Drop

HRESULT CSignPostDialog::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	UNREFERENCED_PARAMETER(grfKeyState);

	ASSERT( m_pITargetDataObject != NULL );
	ASSERT( m_pITargetDataObject == pIDataObject );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetDesktopWindow () );
	}

	// Set default values
	HRESULT hr = S_OK;
	*pdwEffect = DROPEFFECT_NONE;

	// Choose effect when right mouse drag - Move, Copy, or Cancel?
	if( m_dwOverDragButton & MK_RBUTTON )
	{
		HMENU hMenu;
		HMENU hMenuPopup;
		
		// Display arrow cursor
		::LoadCursor( AfxGetInstanceHandle(), IDC_ARROW );
	
		// Prepare context menu
		hMenu = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_DRAG_RMENU) );
		if( hMenu )
		{
			m_dwDragRMenuEffect = DROPEFFECT_NONE;

			// Track right context menu for drag-drop via TrackPopupMenu
			hMenuPopup = ::GetSubMenu( hMenu, 0 );

			// Init state of menu items
			if( !(m_dwOverDragEffect & DROPEFFECT_MOVE) )
			{
				::EnableMenuItem( hMenuPopup, IDM_DRAG_MOVE, (MF_GRAYED | MF_BYCOMMAND) );
			}

			// Get a window to attach menu to
			CWnd* pWnd = dynamic_cast<CWnd*>(this);
			if(pWnd)
			{
				// Display and track menu
				CPoint point( pt.x, pt.y );
				::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, pWnd->GetSafeHwnd(), NULL );
				DestroyMenu( hMenu );

				// Need to process WM_COMMAND from TrackPopupMenu
				MSG msg;
				while( ::PeekMessage( &msg, pWnd->GetSafeHwnd(), NULL, NULL, PM_REMOVE) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}

				// WM_COMMAND from TrackPopupMenu will have set m_dwDragRMenuEffect
				m_dwOverDragEffect = m_dwDragRMenuEffect;
				m_dwDragRMenuEffect = DROPEFFECT_NONE;
			}
		}
	}

	// Paste data
	if( m_dwOverDragEffect != DROPEFFECT_NONE )
	{
		hr = PasteAt( pIDataObject, pt, true );
		if( SUCCEEDED ( hr ) )
		{
			*pdwEffect = m_dwOverDragEffect;
		}
	}
/*
	else
	{
		hr = S_FALSE;
	}
	if(hr != S_OK)
	{
		POINT p;
		p.x = pt.x;
		p.y = pt.y;
		int row;
		int col;
		RowColFromPoint(p, row, col);
		m_Grid.SetCol( row );
		m_Grid.SetRow( col );
	
		short Button = static_cast<short>(m_dwOverDragButton & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON));
		short Shift = static_cast<short>(m_dwOverDragButton & (MK_CONTROL | MK_SHIFT));

		OnMouseUpGrid(Button, Shift, pt.x, pt.y);

	}
*/

	// Cleanup
	DragLeave();


	return hr;
}





HRESULT CSignPostDialog::CanPasteFromData(IDataObject* pIDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
/*	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfChordList ) ))
	{
		// check to see that its a single chord
		hr = S_OK;
	}
*/
	hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_cfChordList );
	if(hr != S_OK)
	{
		hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_cfChordMapList );
	}

	pDataObject->Release();
	return hr;
}


HRESULT CSignPostDialog::PasteAt( IDataObject* pIDataObject, POINTL pt, bool bDropNotEditPaste)
{
	UNREFERENCED_PARAMETER(bDropNotEditPaste);

	if( pIDataObject == NULL )
	{
		return E_NOTIMPL;
	}
/*
	// check that we're not dragging and dropping to same place
	if( m_pCopyDataObject && S_OK == OleIsCurrentClipboard( m_pCopyDataObject ))
	{
		// paste via copy op, use grid coords
		if(pt.y == m_startDragPosition.y && pt.x == m_startDragPosition.x)
		{
			return E_FAIL;
		}
	}
	else
	{
		int row, col;
		POINT q;
		q.x = pt.x;
		q.y = pt.y;
		RowColFromPoint(q, row, col);
		if(col == m_startDragPosition.x && row == m_startDragPosition.y)
		{
			return E_FAIL;
		}
	}
*/
	if(bDropNotEditPaste)
	{
		int row, col;
		POINT q;
		q.x = pt.x;
		q.y = pt.y;
		RowColFromPoint(q, row, col);
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	bool bChordNotChordMap = true;
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_cfChordList );
	if(hr != S_OK)
	{
		bChordNotChordMap = false;
		hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_cfChordMapList );
	}

//	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfChordList ) ) )

	if(hr == S_OK)
	{
		IStream* pIStream;

		if(bChordNotChordMap)
			hr = pDataObject->AttemptRead( pIDataObject, m_cfChordList, &pIStream);
		else
			hr = pDataObject->AttemptRead( pIDataObject, m_cfChordMapList, &pIStream);

		if(SUCCEEDED (hr))
		{
			hr = S_OK;
			LARGE_INTEGER liTemp;
			// seek to beginning of stream
			liTemp.QuadPart = 0;
			pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
			// declare a temp list and write chords into it
			CChordList list;
			if(bChordNotChordMap)
			{
				list.Load(pIStream);
			}
			else
			{
				ChordEntryList chordEntryList;
				ChordEntry* pEntry = 0;
				hr = DMLoadChordData(chordEntryList, pIStream);
				if(FAILED(hr))
				{
					for(pEntry = chordEntryList.RemoveHead(); pEntry ; pEntry = chordEntryList.RemoveHead())
					{
						delete pEntry;
					}
					goto Leave;
				}
				// convert to CChordList
				for(pEntry = chordEntryList.RemoveHead(); pEntry; )
				{
					CChordItem* pItem = new CChordItem(*pEntry);
					delete pEntry;
					list.AddTail(pItem);
					pEntry = chordEntryList.RemoveHead();
				}
			}
			pIStream->Release();
			if(list.GetHead() == NULL)
			{
				// don't drop an empty list
				hr = E_FAIL;
				goto Leave;
			}
			int row, col;
			SendEditNotification(IDS_UNDO_PasteSignpostChord);
//			if( m_pCopyDataObject && S_OK == OleIsCurrentClipboard( m_pCopyDataObject ))
			if(!bDropNotEditPaste)
			{
				// paste via copy, don't convert coords
				row = pt.y;
				col = pt.x;
			}
			else
			{
				POINT q;
				q.x = pt.x;
				q.y = pt.y;
				RowColFromPoint(q, row, col);
			}
/*
			row = pt.y;
			col = pt.x;
*/
			int lastrow = m_Grid.GetRows() - 1;
			bool chordcol = (col == COL_C1) || (col == COL_C2) || (col == COL_SP);
			bool chordrow = row <= lastrow && row > 0;
			if(chordcol && chordrow)
			{
				m_ptLastMouse.x = static_cast<short>(col);
				m_ptLastMouse.y = static_cast<short>(row);
				// use only the first chord
				CChordItem* pItem = list.GetHead();
				ChordEntry chord;
				chord.LoadFromChordItem(*pItem);
				SignPost* pSignPost;
				if(row == lastrow)
				{
					// insert new signpost equal to passed in chord
					pSignPost = new SignPost;
					DMPolyChord* pPoly = dynamic_cast<DMPolyChord*>(&(pSignPost->m_chord));
					*pPoly = chord.m_chordsel;
					if( pSignPost == NULL )
					{
						hr =  E_OUTOFMEMORY;
						goto Leave;
					}
					// Default with Cadence Chords turned off.
					pSignPost->m_dwflags ^= (SPOST_CADENCE1 | SPOST_CADENCE2);
					// Insert the new SignPost into the list
					m_pSignPostList->AddTail( pSignPost );
					AddSignPostToGrid(pSignPost);
					// add a new signpost, do orphan check
					m_pIPersonality->Link2CM(pSignPost);
					m_pIPersonality->Link2SP(pSignPost);
					m_pIPersonality->RefreshChordMap();
					UpdateOrphanStatus(true);
				}
				else
				{
					pSignPost = (SignPost*)(m_Grid.GetRowData( row ));
					DMPolyChord* pPoly;
					ASSERT(pSignPost);
					switch(col)
					{
					case COL_C1:
						pPoly = dynamic_cast<DMPolyChord*>(&(pSignPost->m_cadence[0]));
						*pPoly = chord.m_chordsel;
						pSignPost->m_dwflags |= SPOST_CADENCE1;
						UpdateCadenceChord(row, col, TRUE);
						break;
					case COL_C2:
						pPoly = dynamic_cast<DMPolyChord*>(&(pSignPost->m_cadence[1]));
						*pPoly = chord.m_chordsel;
						pSignPost->m_dwflags |= SPOST_CADENCE2;
						UpdateCadenceChord(row, col, TRUE);
						break;
					case COL_SP:
						pPoly = dynamic_cast<DMPolyChord*>(&(pSignPost->m_chord));
						*pPoly = chord.m_chordsel;
						hr = UpdateSignPostChord(row);
						break;
					default:
						hr = E_FAIL;
						break;
					}
				}
				if(hr == S_OK)
				{
					m_Grid.SetCol( m_ptLastMouse.x );
					m_Grid.SetColSel( m_ptLastMouse.x );
					m_Grid.SetRow( m_ptLastMouse.y );
					m_Grid.SetRowSel( m_ptLastMouse.y );
					// Add this Chord to the template Property Chord.
					ASSERT( pSignPost != NULL );
					SetSelectedChord( pSignPost, m_ptLastMouse.x );
					PreparePropChord( pSignPost );


					if( IsPropPageShowing())
					{
						if(SUCCEEDED(OnShowProperties()) )
						{
							if( m_pPropPageMgr )
							{
								m_pPropPageMgr->RefreshData();
							}
						}
					}

					SetFocus();
					if( m_pIPersonality != NULL )
					{
						DMusicSync(CPersonality::syncPersonality + CPersonality::syncAudition);
						m_pIPersonality->Modified() = TRUE;
					}
				}
			}
			else
			{
				hr = E_FAIL;
			}
			list.ClearList();
		}
	}
Leave:	
	pDataObject->Release();
	return hr;
}

namespace SignPostDragDrop
{
	const short Enter = 0;
	const short Leave = 1;
	const short Over = 2;
}

void CSignPostDialog::OnOLEDragOverGrid(LPDISPATCH FAR* Data, long FAR* Effect, short FAR* Button, short FAR* Shift, float FAR* x, float FAR* y, short FAR* State) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	POINTL pt;
	pt.x = static_cast<long>(*x);
	pt.y = static_cast<long>(*y);

	DWORD grfState = VBtoWIN32MouseState(*Button, *Shift);

	IDataObject* pIDataObject;

	if(SUCCEEDED((*Data)->QueryInterface(IID_IDataObject, (void**)&pIDataObject)))
	{
		switch(*State)
		{
		case SignPostDragDrop::Enter:
			DragEnter(pIDataObject, 
					  grfState, 
					  pt, 
					  reinterpret_cast<DWORD*>(Effect));
			break;
		case SignPostDragDrop::Leave:
			DragLeave();
			break;
		case SignPostDragDrop::Over:
			DragOver(grfState, pt, reinterpret_cast<DWORD*>(Effect));
			break;
		}
		pIDataObject->Release();
	}
}

void CSignPostDialog::OnOLEDragDropGrid(LPDISPATCH FAR* Data, long FAR* Effect, short FAR* Button, short FAR* Shift, float FAR* x, float FAR* y) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	POINTL pt;
	pt.x = static_cast<long>(*x);
	pt.y = static_cast<long>(*y);

	DWORD grfState = VBtoWIN32MouseState(*Button, *Shift);

	IDataObject* pIDataObject;

	if(SUCCEEDED((*Data)->QueryInterface(IID_IDataObject, (void**)&pIDataObject)))
	{
		Drop(pIDataObject, 
			 grfState, 
			 pt, 
			 reinterpret_cast<DWORD*>(Effect));
		pIDataObject->Release();
	}

}

///////////////////////////// Edit Methods
//
HRESULT CSignPostDialog::Cut (IDataObject** )
{
	return E_NOTIMPL;
}

HRESULT CSignPostDialog::Copy( IDataObject **ppIDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;
	IStream*			pStreamCopy;
	CDllJazzDataObject*	pDataObject;
	IDataObject*		pIDataObjectOrig;

	hr = CanCopy();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}


	if(ppIDataObject == NULL)
	{
		pIDataObjectOrig = NULL;
	}
	else
	{
		pIDataObjectOrig = *ppIDataObject;
	}

	if(pIDataObjectOrig != NULL)
	{
		// If there is an existing data object, copy it to our new one
		IStream		*pIStream;
		IEnumFORMATETC	*pIEnumFORMATETC;
		FORMATETC formatEtc;
		ULONG ulElem;
		pDataObject = new CDllJazzDataObject;

		ASSERT( pDataObject != NULL );
		if (pDataObject == NULL )
		{
			return E_OUTOFMEMORY;
		}

		hr = pIDataObjectOrig->EnumFormatEtc( DATADIR_GET, &pIEnumFORMATETC );
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			pDataObject->Release();
			return E_FAIL;
		}

		while ( pIEnumFORMATETC->Next( 1, &formatEtc, &ulElem ) == S_OK)
		{
			hr = pDataObject->AttemptRead( pIDataObjectOrig, formatEtc.cfFormat, &pIStream);
			ASSERT( hr == S_OK );
			if ( hr != S_OK )
			{
				pIEnumFORMATETC->Release();
				pDataObject->Release();
				return E_FAIL;
			}
			hr = pDataObject->AddClipFormat( formatEtc.cfFormat, pIStream );
			pIStream->Release();
			ASSERT( hr == S_OK );
			if ( hr != S_OK )
			{
				pIEnumFORMATETC->Release();
				pDataObject->Release();
				return E_FAIL;
			}
		}
		pIEnumFORMATETC->Release();
	}
	else // pIDataObjectOrig == NULL
	{
		// There is no existing data object, so just create a new one
		pDataObject = new CDllJazzDataObject;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfChordList == 0 )
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0 )
		{
			pDataObject->Release();
			return E_FAIL;
		}
	}


	if( SUCCEEDED (m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pStreamCopy) ) )
	{
		ChordSelection* pChordAtDragPoint;
		SignPost* pSignPost = (SignPost*)(m_Grid.GetRowData( m_ptLastMouse.y ));
		switch(m_ptLastMouse.x)
		{
		case COL_C1:
			ASSERT(pSignPost->m_dwflags & SPOST_CADENCE1);
			pChordAtDragPoint = &(pSignPost->m_cadence[0]);
			break;
		case COL_C2:
			pChordAtDragPoint = &(pSignPost->m_cadence[1]);
			break;
		case COL_SP:
			pChordAtDragPoint = &(pSignPost->m_chord);
			break;
		default:
			ASSERT(FALSE);	// drag must start from one of above cols
			pChordAtDragPoint = 0;
			hr = E_FAIL;
			break;
		}

		if(!SUCCEEDED(hr))
		{
			ASSERT(FALSE);
			pStreamCopy->Release();
			return hr;
		}

		if( SUCCEEDED ( SaveSelectedChords( pStreamCopy, pChordAtDragPoint, TRUE ) ) )
		{
			// Place CF_CHORDLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfChordList, pStreamCopy ) ) )
			{
				hr = S_OK;
			}
		}
	}



	// add the stream to the passed IDataObject
	hr = pDataObject->AddClipFormat( m_cfChordList, pStreamCopy );
	pStreamCopy->Release();
	ASSERT( hr == S_OK );
	if ( hr != S_OK )
	{
		pDataObject->Release();
		return E_FAIL;
	}

	if (pIDataObjectOrig != NULL)
	{
		pIDataObjectOrig->Release();
	}
	// get the new IDataObject
	hr = pDataObject->QueryInterface(IID_IDataObject, (void**) &pIDataObjectOrig);
	if(FAILED(hr))
	{
		pDataObject->Release();
		return E_UNEXPECTED;
	}


	// Release the old object, and set the pointer to the new one.
	if( m_pCopyDataObject != NULL )
	{
		m_pCopyDataObject->Release();
	}
	m_pCopyDataObject = pDataObject;
	pDataObject->Release();
	if (ppIDataObject != NULL)
	{
		*ppIDataObject = pIDataObjectOrig;
		pIDataObjectOrig->AddRef();
	}
	else
	{
		if( S_OK != OleSetClipboard( pIDataObjectOrig ))
		{
			return E_FAIL;
		}
	}
	
	
	return hr;
}



HRESULT CSignPostDialog::Paste( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr = S_FALSE;
	
	hr = CanPaste();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}


	// If the format hasn't been registered yet, do it now.
	if( m_cfChordList == 0 )
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0 )
		{
			return E_FAIL;
		}
	}
	if( m_cfChordMapList == 0 )
	{
		m_cfChordMapList = RegisterClipboardFormat( CF_CHORDMAP_LIST );
		if( m_cfChordMapList == 0 )
		{
			return E_FAIL;
		}
	}
	
	if(pIDataObject == NULL)
	{
		// Get the IDataObject
		hr = OleGetClipboard(&pIDataObject);
		if(FAILED(hr) || (pIDataObject == NULL))
		{
			return E_FAIL;
		}
	}
	else
	{
		pIDataObject->AddRef();
	}



	// Paste
	POINTL pt;
	pt.x = m_ptLastMouse.x;
	pt.y = m_ptLastMouse.y;
	hr = PasteAt(pIDataObject, pt, false);
	pIDataObject->Release();
	return hr;
}

HRESULT CSignPostDialog::Insert( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pIDataObject);
	OnCommand(IDM_SIGNPOST_ADD_CHORD, 0);
	return S_OK;
}

HRESULT CSignPostDialog::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = CanDelete();
	if(hr == S_OK)
	{
		hr = S_FALSE;
		SignPost *pSignPost = (SignPost*)(m_Grid.GetRowData(m_ptLastMouse.y));
		// Delete entire row if the entire row is selected
		if( m_ptLastMouse.x == COL_SELECT || m_ptLastMouse.x == COL_SP )
		{
			SendEditNotification(IDS_UNDO_DeleteSignpostChord);
			// Remove the SignPost from the list.
			m_Grid.RemoveItem( m_ptLastMouse.y );

			// Delete SignPost from the SignPost list.
			m_pSignPostList->Remove( pSignPost );
			delete pSignPost;
			m_pIPersonality->LinkAll(true);

			ResizeGrid();
			hr = S_OK;
		}
		// or.. Remove (Inactivate) Cadence Chord 1 if it is selected
		else if( m_ptLastMouse.x == COL_C1 )
		{
			SendEditNotification(IDS_UNDO_DeleteCadenceChord);
			pSignPost->m_dwflags ^= SPOST_CADENCE1;
			m_Grid.SetText( "" );
			hr = S_OK;
		}
		// or.. Remove (Inactivate) Cadence Chord 2 if it is selected
		else if( m_ptLastMouse.x == COL_C2 )
		{
			SendEditNotification(IDS_UNDO_DeleteCadenceChord);
			pSignPost->m_dwflags ^= SPOST_CADENCE2;
			m_Grid.SetText( "" );
			hr = S_OK;
		}
		if(hr == S_OK)
		{
			m_Grid.SetCol( m_ptLastMouse.x );
			m_Grid.SetColSel( m_ptLastMouse.x );
			m_Grid.SetRow( m_ptLastMouse.y );
			m_Grid.SetRowSel( m_ptLastMouse.y );
		}
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

HRESULT CSignPostDialog::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_Grid.SetColSel(0);
	m_Grid.SetRowSel(0);
	m_ptLastMouse.x = 0;
	m_ptLastMouse.y = 0;
	return S_OK;
}

HRESULT CSignPostDialog::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return S_FALSE;
}

HRESULT CSignPostDialog::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_FALSE;

	if(m_ptLastMouse.y > 0 && m_ptLastMouse.y < m_Grid.GetRows() - 1)
	{
		SignPost* pSignPost = (SignPost*)(m_Grid.GetRowData( m_ptLastMouse.y ));
		if(pSignPost == 0)
		{
			hr =  S_FALSE;
		}
		else
		{
			switch(m_ptLastMouse.x)
			{
				case COL_C1:
					if( pSignPost->m_dwflags & SPOST_CADENCE1 )
					{
						hr = S_OK;
					}
					break;
				case COL_C2:
					if( pSignPost->m_dwflags & SPOST_CADENCE2 )
					{
						hr = S_OK;
					}
					break;
				case COL_SP:
					hr = S_OK;
					break;
				default:
					hr = S_FALSE;
					break;
			}
		}
	}

	return hr;
}

HRESULT CSignPostDialog::CanPaste( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;
	IDataObject*		pIDataObject;
	CDllJazzDataObject*	pDataObject;

	// If the format hasn't been registered yet, do it now.
	if( m_cfChordList == 0 )
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0 )
		{
			return E_FAIL;
		}
	}
	if(m_cfChordMapList == 0)
	{
		m_cfChordMapList = RegisterClipboardFormat( CF_CHORDMAP_LIST );
		if( m_cfChordMapList == 0 )
		{
			return E_FAIL;
		}
	}

	// Get the IDataObject
	hr = OleGetClipboard( &pIDataObject );
	if( FAILED( hr ))
	{
		return E_FAIL;
	}

	// Create a new Jazz data object and see if it can read the data object's format.
	pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		pIDataObject->Release();
		return E_OUTOFMEMORY;
	}

	hr = pDataObject->IsClipFormatAvailable(pIDataObject, m_cfChordList);
	if(hr != S_OK)
	{
		hr = pDataObject->IsClipFormatAvailable(pIDataObject, m_cfChordMapList);
	}
	pDataObject->Release();
	pIDataObject->Release();
	if (hr == S_OK)
	{
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CSignPostDialog::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
//	HRESULT hr = GetClickedChord( m_ptLastMouse, NULL );
//	return hr == S_OK ? S_OK : S_FALSE;
	return S_OK;
}

HRESULT CSignPostDialog::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	long rowsel = m_Grid.GetRowSel();
	long colsel = m_Grid.GetColSel();
	HRESULT hr = S_FALSE;
	if( rowsel > -1 && rowsel < m_Grid.GetRows() - 1)
	{
		if(colsel == COL_SELECT
			|| colsel == COL_C1
			|| colsel == COL_SP)
		{
			hr = S_OK;
		}
	}
	return hr;
}



void CSignPostDialog::Refresh(bool isStructualChange)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if(!m_bInitialized)
		return;
	int maxrows = m_Grid.GetRows();
	int i = 1;

	m_Grid.SetRedraw(FALSE);

	if(isStructualChange)
	{
		for(i = 1; i < maxrows -1; i++)
		{
			m_Grid.RemoveItem(1);
		}
//		SetRowText(0, 0);
//		m_Grid.SetRowData(0, 0);
	}

	bool bNotEmpty = false;
	i = 1;
	for(	SignPost *pSignPost = m_pSignPostList->GetHead();
			pSignPost != NULL;
			pSignPost = pSignPost->GetNext() )
	{
		bNotEmpty = true;
		if(isStructualChange)
			AddSignPostToGrid( pSignPost , i++);
		else
		{
			SetRowText(pSignPost, i);
			m_Grid.SetRowData( i++, (long)pSignPost ); 
		}
	}


//	if(bNotEmpty  && IsMyPropPageShowing())
	if(bNotEmpty)
	{
//		m_ptLastMouse.x = COL_SP;
//		m_ptLastMouse.y = 1;
		if(m_ptLastMouse.x > COL_SP)
		{
			// don't want focus on non-chord cell if proppage is up
			m_ptLastMouse.x = COL_SP;
		}
		m_Grid.SetCol( m_ptLastMouse.x );
		m_Grid.SetColSel( m_ptLastMouse.x );
		m_Grid.SetRow( m_ptLastMouse.y );
		m_Grid.SetRowSel( m_ptLastMouse.y );
		if(m_ptLastMouse.x == COL_C1 || m_ptLastMouse.x == COL_C2)
		{
			// check to see if anythings here
			CString str = m_Grid.GetText();
			if(str.GetLength() == 0 || str == " ")
			{
				ShowPropPage(FALSE);
				goto End;
			}
		}
		SignPost* pSignPost = (SignPost*)(m_Grid.GetRowData( m_ptLastMouse.y ));

		SetSelectedChord(pSignPost,  m_ptLastMouse.x);
		DMusicSync(CPersonality::syncAudition);
		PreparePropChord( pSignPost);

		if( IsMyPropPageShowing() )
		{
			if( SUCCEEDED(OnShowProperties()) )
			{
				if( m_pPropPageMgr )
				{
					m_pPropPageMgr->RefreshData();
				}
			}
		}
	}
End:
	m_Grid.SetRedraw(TRUE);
	m_Grid.Refresh();
}

void CSignPostDialog::SendEditNotification(UINT type)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HINSTANCE hInst = AfxGetInstanceHandle( );

	if(m_pIPersonality->m_pUndoMgr)
		m_pIPersonality->m_pUndoMgr->SaveState(m_pIPersonality, hInst, type);

}

bool CSignPostDialog::IsPropPageShowing()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pIFramework);
	if(m_pIFramework == NULL)
	{
		return false;
	}
	/*
	if( m_pPropPageMgr == NULL )
	{
		return false;
	}
	*/

	// check PropertyPage
	IDMUSProdPropSheet* pJPS;
	bool rc = false;
	if( SUCCEEDED(m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet,
		(void**)&pJPS )))
	{
		if(pJPS->IsShowing() == S_OK)
			rc = true;
		else
			rc = false;
		pJPS->Release();
	}

	return rc;
}

bool CSignPostDialog::IsMyPropPageShowing()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pIFramework);
	if(m_pIFramework == NULL)
	{
		return false;
	}
	/*
	if( m_pPropPageMgr == NULL )
	{
		return false;
	}
	*/

	// check PropertyPage
	IDMUSProdPropSheet* pJPS;
	bool rc = false;
	if( SUCCEEDED(m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet,
		(void**)&pJPS )))
	{
//		if(pJPS->IsShowing() == S_OK && pJPS->IsEqualPageManagerObject(this) == S_OK)
		if(pJPS->IsShowing() == S_OK && pJPS->IsEqualPageManagerObject((IDMUSProdPropPageObject*)this) == S_OK)
			rc = true;
		else
			rc = false;
		pJPS->Release();
	}

	return rc;
}

HRESULT CSignPostDialog::ShowPropPage(BOOL fShow)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = S_OK;
	ASSERT(m_pIFramework);
	if(m_pIFramework == NULL)
	{
		return E_FAIL;
	}
	if( m_pPropPageMgr == NULL )
	{
			return S_FALSE;
	}
	hr = E_FAIL;

	// SetPropertyPage
	if( m_pIFramework )
	{
		IDMUSProdPropSheet* pJPS;
		if( SUCCEEDED(hr = m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet,
			(void**)&pJPS )))
		{
			if(fShow == TRUE)
			{
				pJPS->Show(fShow);
			}
			else
			{
				pJPS->RemovePageManager(m_pPropPageMgr);
			}
			pJPS->Release();
		}
	}

	return hr;

}

static int Index(int row, int col)
{
	return row*MAX_COLUMNS + col;
}

void CSignPostDialog::SetRowText(SignPost * pSignPost, int row)
{
	if(pSignPost == 0)
	{
		// blank out grid
		for(int i = 0; i < MAX_COLUMNS; i++)
		{
			m_Grid.SetTextArray(Index(row, i), "");
		}
		return;
	}

	// Add new fields to the Grid
	CString csWrkBuffer;
	char	szRootName[30];
	// save row,col, cause we'll need to change it to set orphan status
	int irow = m_Grid.GetRow();
	int icol = m_Grid.GetCol();

// Store Cadence and SignPost names.
	if( pSignPost->m_dwflags & SPOST_CADENCE1 )
	{
		pSignPost->m_cadence[0].RootToString(szRootName);
		csWrkBuffer.Format( "%s %s", szRootName, pSignPost->m_cadence[0].Name() );
		m_Grid.SetTextArray(Index(row, COL_C1), csWrkBuffer);
	}
	else
	{
		m_Grid.SetTextArray(Index(row, COL_C1), "");
	}
	if( pSignPost->m_dwflags & SPOST_CADENCE2 )
	{
		pSignPost->m_cadence[1].RootToString(szRootName);
		csWrkBuffer.Format( "%s %s", szRootName, pSignPost->m_cadence[1].Name() );
		m_Grid.SetTextArray(Index(row, COL_C2), csWrkBuffer);
	}
	else
	{
		m_Grid.SetTextArray(Index(row, COL_C2), "");
	}

	pSignPost->m_chord.RootToString(szRootName);
	csWrkBuffer.Format( "%s %s", szRootName, pSignPost->m_chord.Name() );
	//set up
	m_Grid.SetRow(row);
	m_Grid.SetCol(COL_SP);
	if(pSignPost->m_dwflags & SPOST_MATCHED)
	{
		m_Grid.SetCellFontBold(TRUE);
		m_Grid.SetCellFontItalic(FALSE);
	}
	else
	{
		m_Grid.SetCellFontBold(FALSE);
		m_Grid.SetCellFontItalic(TRUE);
	}

	m_Grid.SetTextArray(Index(row, COL_SP), csWrkBuffer);

	int col = COL_1;

	if( pSignPost->m_chords & SP_1 )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_2 )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_3 )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_4 )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_5 )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_6 )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_7 )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_A )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_B )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_C)
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_D )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_E )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}
	if( pSignPost->m_chords & SP_F )
	{
		m_Grid.SetTextArray(Index(row, col++), "X");
	}
	else
	{
		m_Grid.SetTextArray(Index(row, col++), " ");
	}


	m_Grid.SetRow(irow);
	m_Grid.SetCol(icol);
}

void CSignPostDialog::DMusicSync(int type)
{
	if(m_pIPersonality)
		m_pIPersonality->SyncPersonalityWithEngine(type);
}

int CSignPostDialog::ComputeRowWidth()
{
	int ncols = m_Grid.GetCols();
	int sum = 0;
	for(int i = 0; i < ncols; i++)
	{
		sum += m_Grid.GetColWidth(i);
	}
	return sum;
}

int CSignPostDialog::ComputeColHeight()
{
	int nRows = m_Grid.GetRows();
	int sum = 0;
	for(int i = 0; i < nRows; i++)
	{
		sum += m_Grid.GetRowHeight(i);
	}
	return sum;
}


void CSignPostDialog::OnClickCMSFlexGrid() 
{
	// TODO: Add your control notification handler code here
//	OnClickGrid();	
}

bool CSignPostDialog::ToggleSignpostGroupBoxes()
{
	// set sel to anchor cell
	int rowsel = m_Grid.GetRowSel();
	int colsel = m_Grid.GetColSel();
	int col = m_Grid.GetCol();
	int row = m_Grid.GetRow();
	int nrows = m_Grid.GetRows() - 1;
	m_Grid.SetRowSel(row);
	m_Grid.SetColSel(col);

	// If user selected 1 through F then select/unselect the cell state and update the chord
	if( (col >= COL_1) && (col <= COL_F) )
	{

		CString csCellText;
		csCellText = m_Grid.GetText();

		m_Grid.SetRedraw(FALSE);	// too slow for this to be effective

		if(csCellText == "X")
		{
			SendEditNotification(IDS_UNDO_SignpostRemoveFromGroup);
			for(int i = row; (i <= rowsel) && (i < nrows); i++)
			{
				for(int j = col; j <= colsel; j++)
				{
//					m_Grid.SetTextMatrix(i,j, " ");
					m_Grid.SetCol(j);
					m_Grid.SetRow(i);
					m_Grid.SetText(" ");
					SetAcceptChords( i, j, FALSE );
				}
			}
		}
		else
		{
			SendEditNotification(IDS_UNDO_SignpostAddToGroup);
			for(int i = row; (i <= rowsel) && (i < nrows); i++)
			{
				for(int j = col; j <= colsel; j++)
				{
					m_Grid.SetCol(j);
					m_Grid.SetRow(i);
					m_Grid.SetText("X");
					SetAcceptChords( i, j );
				}
			}
		}
		m_Grid.SetRowSel(row);
		m_Grid.SetColSel(col);
		m_Grid.SetCol(col);
		m_Grid.SetRow(row);
		m_Grid.SetRedraw(TRUE);
		return true;
	}
	return false;	// nothing done
}



void CSignPostDialog::SetDrawingState(SignPost *pSignPost)
{
	if(pSignPost)
	{
		int row = -1;

		// find row
		for(int i = 0; i < (m_Grid.GetRows() - 1); i++)
		{
			if(pSignPost == (SignPost*)(m_Grid.GetRowData( i )))
			{
				row = i;
				break;
			}
		}
		if(row == -1)
		{
			// signpost doesn't exist in grid so nothing to do
			return;
		}
		m_Grid.SetCol(COL_SP);
		m_Grid.SetColSel(COL_SP);
		m_Grid.SetRowSel(i);
		m_Grid.SetRow(i);

		// set drawing state
		if(pSignPost->m_dwflags & SPOST_MATCHED)
		{
			m_Grid.SetCellFontBold(TRUE);
			m_Grid.SetCellFontItalic(FALSE);
		}
		else
		{
			m_Grid.SetCellFontBold(FALSE);
			m_Grid.SetCellFontItalic(TRUE);
		}
	}
	else
	{
		m_Grid.SetCol(COL_SP);
		m_Grid.SetColSel(COL_SP);
		for(int i= 1; i < (m_Grid.GetRows() - 1); i++)
		{
			m_Grid.SetRowSel(i);
			m_Grid.SetRow(i);
			pSignPost = (SignPost*)(m_Grid.GetRowData( i ));
			// set drawing state
			if(pSignPost->m_dwflags & SPOST_MATCHED)
			{
				m_Grid.SetCellFontBold(TRUE);
				m_Grid.SetCellFontItalic(FALSE);
			}
			else
			{
				m_Grid.SetCellFontBold(FALSE);
				m_Grid.SetCellFontItalic(TRUE);
			}			
		}
	}
}

void CSignPostDialog::UpdateOrphanStatus(bool bRefreshDisplay, SignPost *pSignPost)
{
	if(bRefreshDisplay)
	{
		m_Grid.SetRedraw(FALSE);	// turn off redraw until done
		SetDrawingState(pSignPost);
		m_Grid.SetRedraw(TRUE);
	}
	else
	{
		SetDrawingState(pSignPost);
	}
}

