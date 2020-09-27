#include "stdafx.h"

#define CX_COMBOBOX 175
#define CX_TOOLBAR 300

TBBUTTON TOOLVIEW::tbb[N_TOOLS] = {
	{ 0, IDM_PARENTDIR, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{ 1, IDM_REBOOT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
#ifdef FOURBUTTONS
	{ 2, IDM_TARGETMACHINE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{ 3, IDM_ABOUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
#endif
};

const TBBUTTON TOOLVIEW::tb = {
	0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0
};

TOOLVIEW::TOOLVIEW()
{
	hToolWnd = NULL;
	hWndCB = hWndTB = NULL;
}

TOOLVIEW::~TOOLVIEW()
{
}

HWND TOOLVIEW::GethWnd()
{
	return hToolWnd;
}

HWND TOOLVIEW::CreateToolView( HWND hWnd )
{
	REBARINFO rbi;
	INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx( &icex );
	hToolWnd = CreateWindowEx( WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | RBS_VARHEIGHT | RBS_BANDBORDERS | CCS_NODIVIDER | CCS_NOPARENTALIGN/* | CCS_VERT(êÇíºÉoÅ[)*/, 0, 0, 0, 0, hWnd, (HMENU) ID_REBAR, hInst, NULL );
	if ( ! hToolWnd ) return NULL;
	rbi.cbSize = sizeof(REBARINFO);
	rbi.fMask = 0;
	rbi.himl = (HIMAGELIST) NULL;
	if ( ! SendMessage( hToolWnd, RB_SETBARINFO, 0, (LPARAM) &rbi ) ) return NULL;
	CreateComboBox( hToolWnd );
	CreateToolBar( hWnd );

	return hToolWnd;
}

HWND TOOLVIEW::RedrawToolView( HWND hWndParent )
{
	RECT rcl;

	GetClientRect( hWndParent, &rcl );
	SetWindowPos( hToolWnd, NULL, 0, 0, rcl.right - rcl.left, 100, SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW );
    return hToolWnd;
}

void TOOLVIEW::Refresh()
{
    SendMessage( hWndCB, CB_RESETCONTENT, 0, 0 );
	int iCurSel = InsertDriveNameToComboBox();
    SendMessage( hWndCB, CB_SETCURSEL, iCurSel, 0 );
}

void TOOLVIEW::Clear()
{
    SendMessage( hWndCB, CB_RESETCONTENT, 0, 0 );
}

void TOOLVIEW::CreateToolBar( HWND hWnd )
{
	InitCommonControls();
	hWndTB = CreateToolbarEx( hWnd, WS_CHILD | WS_VISIBLE | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_NODIVIDER | TBSTYLE_FLAT | TBSTYLE_WRAPABLE | TBSTYLE_TOOLTIPS, ID_TOOLBAR, N_TOOLS, hInst, ID_TOOLBAR, tbb, N_TOOLS, 0, 0, 0, 0, sizeof(TBBUTTON) );
	SendMessage( hWndTB, TB_INSERTBUTTON, 1, (LPARAM) &tb );
#ifdef FOURBUTTONS
	SendMessage( hWndTB, TB_INSERTBUTTON, 4, (LPARAM) &tb );
#endif
}

void TOOLVIEW::CreateComboBox( HWND hWnd )
{
	hWndCB = CreateWindow( TEXT( "COMBOBOX" ), NULL, WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST,
		0, 0, 100, 200, hWnd, (HMENU) ID_COMBOBOX, hInst, NULL );

	SendMessage( hWndCB, WM_SETFONT, (WPARAM) GetStockObject( DEFAULT_GUI_FONT ), MAKELPARAM( TRUE, 0 ) );
}

int TOOLVIEW::InsertDriveNameToComboBox()
{
	char szString[64];
    char szDrives[128];
	int i, iCurSel;
    DWORD cch = sizeof szDrives;

    if(!xbfu.GetDriveList(szDrives, &cch))
        szDrives[0] = 0;

	for( i = 0; szDrives[i]; i++ )
	{
        wsprintf(szString, "X%c:\\", toupper(szDrives[i]));
        if (szString[1] == g_CurrentDir[1])
            iCurSel = i;
		SendMessage( hWndCB, CB_ADDSTRING, 0, (LPARAM) szString );
	}

    return iCurSel;
}

void TOOLVIEW::InsertComboBox()
{
	REBARBANDINFO rbBand;
	RECT rc;
	int iCurSel = InsertDriveNameToComboBox();
	SendMessage( hWndCB, CB_SETCURSEL, iCurSel, 0 );

	if ( hWndCB )
	{
		GetWindowRect( hWndCB, &rc );
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.hbmBack = NULL;
		rbBand.fStyle = RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_GRIPPERALWAYS;

		rbBand.fMask = RBBIM_TEXT | RBBIM_BACKGROUND | RBBIM_STYLE | RBBIM_CHILDSIZE | RBBIM_SIZE |RBBIM_CHILD | RBBIM_ID;
		rbBand.wID = ID_COMBOBOX;
		rbBand.lpText = TEXT( "Drive" );
		rbBand.cch = 2;
		rbBand.hwndChild = hWndCB;
		rbBand.cxMinChild = rc.right - rc.left + 10;
		rbBand.cyMinChild = rc.bottom - rc.top;
		rbBand.cx = CX_COMBOBOX;
		SendMessage( hToolWnd, RB_INSERTBAND, (WPARAM) -1, (LPARAM) &rbBand );
	}
}

void TOOLVIEW::InsertToolBar()
{
	REBARBANDINFO rbBand;
	RECT rc;
	DWORD dwBtnSize;

	if ( hWndTB )
	{
		GetWindowRect( hWndTB, &rc );
		dwBtnSize = SendMessage( hWndTB, TB_GETBUTTONSIZE, 0, 0 );
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.hbmBack = NULL;
		rbBand.fStyle = RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_GRIPPERALWAYS;

		rbBand.fMask = RBBIM_BACKGROUND | RBBIM_STYLE | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_CHILD | RBBIM_ID;
		rbBand.wID = ID_TOOLBAR;
		rbBand.cch = 2;
		rbBand.hwndChild = hWndTB;
		rbBand.cxMinChild = rc.right - rc.left + 24;
		rbBand.cyMinChild = HIWORD( dwBtnSize );
		rbBand.cx = CX_TOOLBAR;
		SendMessage( hToolWnd, RB_INSERTBAND, (WPARAM) -1, (LPARAM) &rbBand );
	}
}
