/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ChildView.cpp

Abstract:

	The primary window for the dsp builder

Author:

	Robert Heitkamp (robheit) 07-May-2001

Revision History:

	07-May-2001 robheit
		Initial Version
	17-Jul-2001 robheit
		Added the toolbar support

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"
#include "ChildView.h"
#include "ConnectDialog.h"
#include "DirsDialog.h"
#include "Module.h"

//------------------------------------------------------------------------------
//	Stuff:
//------------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//	Versions:
//------------------------------------------------------------------------------
static const BYTE cDspImageVersion	= 2;
static const BYTE cDspIndexVersion	= 1;
static const BYTE cFXParamsVersion	= 1;
static const BYTE cI3DL2Version		= 1;

//------------------------------------------------------------------------------
//	CChildView::CChildView
//------------------------------------------------------------------------------
CChildView::CChildView(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	char					name[256];
	DWORD					length = 256;
	DSFX_AMPMOD_MONO_STATE	effect;

	m_pGraph		= NULL;
	m_bConnected	= FALSE;
	m_width			= 0;
	m_height		= 0;
	m_offset.x		= 0;
	m_offset.y		= 0;
	m_dwParamOffset	= (DWORD)&effect.dwInMixbinPtrs - (DWORD)&effect;
	m_socket		= INVALID_SOCKET;

	WSADATA wsaData;
	unsigned short version = MAKEWORD(2, 2);
	WSAStartup(version, &wsaData);

	// Get the Xbox name
	if(DmGetXboxName(name, &length) == XBDM_NOERR)
	{
		m_xboxName = name;
		m_xboxName += ":80";

		m_transmitDestination = m_xboxName;
	}
}

//------------------------------------------------------------------------------
//	CChildView::~CChildView
//------------------------------------------------------------------------------
CChildView::~CChildView(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pGraph)
		delete m_pGraph;

	Disconnect();
}

//------------------------------------------------------------------------------
//	Message Map
//------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_FILE_GENERATE_AND_SAVE_IMAGE, OnFileGenerateAndSaveImage)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_XBOX_TRANSMIT_IMAGE, OnXboxTransmitImage)
	ON_UPDATE_COMMAND_UI(ID_XBOX_TRANSMIT_IMAGE, OnUpdateXboxTransmitImageUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNewUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSaveUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpenUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_GENERATE_AND_SAVE_IMAGE, OnUpdateFileGenSaveImgUI)
	ON_COMMAND(IDC_TOOLS_DIRECTORIES, OnToolsDirectories)
	ON_COMMAND(IDC_TOOLS_BUILD_OPTIONS, OnToolsBuildOptions)
	ON_COMMAND(IDC_TOOLS_TRANSMIT_OPTIONS, OnToolsTransmitOptions)
	ON_COMMAND(ID_GRID_SHOW_GRID, OnGraphShowGrid)
	ON_COMMAND(IDC_TOOLS_SNAP_TO_GRID, OnToolsSnapToGrid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
//	CChildView::PreCreateWindow
//------------------------------------------------------------------------------
BOOL 
CChildView::PreCreateWindow(
							IN OUT CREATESTRUCT&	cs
							) 
/*++

Routine Description:

	Called before the window is created to adjust in initialization params

Arguments:

	IN OUT cs -	CREATESTRUCT (modify this)

Return Value:

	TRUE on success, otherwise FALSE

--*/
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle	|= WS_EX_CLIENTEDGE;
	cs.style		&= ~WS_BORDER;
	cs.style		|= WS_VSCROLL | WS_HSCROLL;
	cs.lpszClass	= AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
										  ::LoadCursor(NULL, IDC_ARROW), 
										  HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

//------------------------------------------------------------------------------
//	CChildView::OnPaint
//------------------------------------------------------------------------------
void 
CChildView::OnPaint(void) 
/*++

Routine Description:

	Handles all expose events

Arguments:

	None

Return Value:

	None

--*/
{
	CRect	rect;

	if(!m_pGraph)
		return;

	// Get the update region before creating the CPaintDC because CPaintDC
	// will clear the region
	GetUpdateRect(rect);

	rect += m_pGraph->GetRect().TopLeft();

	CPaintDC	dc(this);

	m_pGraph->Draw(rect, &dc);
}

//------------------------------------------------------------------------------
//	CChildView::OnCreate
//------------------------------------------------------------------------------
int 
CChildView::OnCreate(
					 IN OUT LPCREATESTRUCT	lpCreateStruct
					 ) 
/*++

Routine Description:

	Create event. Create the grid and setup the scroll bars

Arguments:

	IN OUT lpCreateStruct -	Create parameters

Return Value:

	0 on success, anything else on failure

--*/
{
	SCROLLINFO	scrollInfo;
	CRect		rect;

	// Base class callback
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the graph
	m_pGraph = new CGraph(this);

	GetClientRect(rect);
	m_width		= rect.Width();
	m_height	= rect.Height();
	m_pGraph->SetRect(rect);

	// Vertical scroll bar
	scrollInfo.cbSize		= sizeof(SCROLLINFO);
	scrollInfo.fMask		= SIF_ALL;
	scrollInfo.nMin			= 0;
	scrollInfo.nMax			= MAX(1, m_height - 1);
	scrollInfo.nPage		= (UINT)scrollInfo.nMax;
	scrollInfo.nPos			= 0;
	scrollInfo.nTrackPos	= 0;
	SetScrollInfo(SB_VERT, &scrollInfo);

	// Horizontal scroll bar (hidden for now)
	scrollInfo.cbSize		= sizeof(SCROLLINFO);
	scrollInfo.fMask		= SIF_ALL;
	scrollInfo.nMin			= 0;
	scrollInfo.nMax			= MAX(1, m_width - 1);
	scrollInfo.nPage		= (UINT)scrollInfo.nMax;
	scrollInfo.nPos			= 0;
	scrollInfo.nTrackPos	= 0;
	SetScrollInfo(SB_HORZ, &scrollInfo);

	return 0;
}

//------------------------------------------------------------------------------
//	CChildView::OnLButtonDblClk
//------------------------------------------------------------------------------
void 
CChildView::OnLButtonDblClk(
						    IN UINT		nFlags, 
						    IN CPoint	point
						    ) 
/*++

Routine Description:

	Handles left mouse double click events by passing them to the grid

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	if(m_pGraph)
		m_pGraph->OnLButtonDblClk(nFlags, point);
}

//------------------------------------------------------------------------------
//	CChildView::OnLButtonDown
//------------------------------------------------------------------------------
void 
CChildView::OnLButtonDown(
						  IN UINT	nFlags, 
						  IN CPoint	point
						  ) 
/*++

Routine Description:

	Handles left mouse down events by passing them to the grid

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	// Trap the mouse
	if(m_pGraph)
	{
		SetCapture();
		m_pGraph->OnLButtonDown(nFlags, point);
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnLButtonUp
//------------------------------------------------------------------------------
void 
CChildView::OnLButtonUp(
						IN UINT		nFlags, 
						IN CPoint	point
						) 
/*++

Routine Description:

	Passes left mouse up events to the grid

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was released

Return Value:

	None

--*/
{
	if(m_pGraph)
	{
		ReleaseCapture();
		m_pGraph->OnLButtonUp(nFlags, point);
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnRButtonDown
//------------------------------------------------------------------------------
void 
CChildView::OnRButtonDown(
						  IN UINT	nFlags, 
						  IN CPoint	point
						  ) 
/*++

Routine Description:

	Passes right mouse up events to the grid

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was pressed

Return Value:

	None

--*/
{
	if(m_pGraph)
		m_pGraph->OnRButtonDown(nFlags, point);
}

//------------------------------------------------------------------------------
//	CChildView::OnSize
//------------------------------------------------------------------------------
void 
CChildView::OnSize(
				   IN UINT	nType, 
				   IN int	cx, 
				   IN int	cy
				   ) 
{
	CRect		rect;
	SCROLLINFO	scrollInfo;
	CRect		bounds;

	if(m_pGraph)
		bounds = m_pGraph->GetBounds();

	CWnd ::OnSize(nType, cx, cy);

	if(!m_pGraph)
		return;

	// Adjust the visible area on the graph
	rect		= m_pGraph->GetRect();
	rect.right	= rect.left + cx;
	rect.bottom = rect.top + cy;
	m_pGraph->SetRect(rect);

	// Adjust the scroll bars
	if(cx != m_width)
	{
		m_width				= cx;
		cx					= m_width + m_offset.x;
		cx					= MAX(cx, bounds.right);
		scrollInfo.cbSize	= sizeof(SCROLLINFO);
		scrollInfo.fMask	= SIF_PAGE | SIF_RANGE | SIF_POS;
		scrollInfo.nMin		= 0;
		scrollInfo.nPos		= m_offset.x;
		scrollInfo.nPage	= (UINT)MAX(0, m_width - 1);
		scrollInfo.nMax		= MAX(0, cx - 1);
//		if(scrollInfo.nPage > (UINT)scrollInfo.nMax)
//			scrollInfo.nPage = (UINT)scrollInfo.nMax;
		SetScrollInfo(SB_HORZ, &scrollInfo);
	}

	if(cy != m_height)
	{
		m_height			= cy;
		cy					= m_height + m_offset.y;
		cy					= MAX(cy, bounds.bottom);
		scrollInfo.cbSize	= sizeof(SCROLLINFO);
		scrollInfo.fMask	= SIF_PAGE | SIF_RANGE | SIF_POS;
		scrollInfo.nMin		= 0;
		scrollInfo.nPos		= m_offset.y;
		scrollInfo.nPage	= (UINT)MAX(0, m_height - 1);
		scrollInfo.nMax		= MAX(0, cy - 1);
//		if(scrollInfo.nPage > (UINT)scrollInfo.nMax)
//			scrollInfo.nPage = (UINT)scrollInfo.nMax;
		SetScrollInfo(SB_VERT, &scrollInfo);
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnVScroll
//------------------------------------------------------------------------------
void 
CChildView::OnVScroll(
					  IN UINT			nSBCode, 
					  IN UINT			nPos, 
					  IN CScrollBar*
					  ) 
{
	int			pos;
	SCROLLINFO	scrollInfo;
	int			y;

	if(!m_pGraph || !GetScrollInfo(SB_VERT, &scrollInfo, SIF_PAGE))
		return;

	switch(nSBCode)
	{
	case SB_LINEUP:
		pos	= m_offset.y;
		y	= GetScrollPos(SB_VERT);
		y	-= m_height / 20;
		if(y < 0)
			y = 0;
		m_offset.y = y;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		UpdateScrollBars();
		SetScrollPos(SB_VERT, m_offset.y);
		ScrollWindow(0, pos - m_offset.y);
		break;

	case SB_LINEDOWN:
		pos	= m_offset.y;
		y	= GetScrollPos(SB_VERT);
		y	+= m_height / 20;
		if(y > (MAX_SIZE - m_height))
			y = MAX_SIZE - m_height;
		m_offset.y = y;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		UpdateScrollBars();
		SetScrollPos(SB_VERT, m_offset.y);
		ScrollWindow(0, pos - m_offset.y);
		break;

	case SB_PAGEUP:
		y	= GetScrollPos(SB_VERT);
		y	-= m_height;
		if(y < 0)
			y = 0;
		m_offset.y = y;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		SetScrollPos(SB_VERT, m_offset.y);
		Invalidate();
		break;

	case SB_PAGEDOWN:
		y	= GetScrollPos(SB_VERT);
		y	+= m_height;
		if(y > (MAX_SIZE - m_height))
			y = MAX_SIZE - m_height;
		m_offset.y = y;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		SetScrollPos(SB_VERT, m_offset.y);
		Invalidate();
		break;

	case SB_THUMBTRACK:
		y			= m_offset.y - (int)nPos;
		m_offset.y	= (int)nPos;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		ScrollWindow(0, y);
		SetScrollPos(SB_VERT, (int)nPos);
		break;
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnHScroll
//------------------------------------------------------------------------------
void 
CChildView::OnHScroll(
					  IN UINT			nSBCode, 
					  IN UINT			nPos, 
					  IN CScrollBar*
					  ) 
{
	int			pos;
	SCROLLINFO	scrollInfo;
	int			x;

	if(!m_pGraph || !GetScrollInfo(SB_HORZ, &scrollInfo, SIF_PAGE | SIF_RANGE))
		return;

	switch(nSBCode)
	{
	case SB_LINEUP:
		pos	= m_offset.x;
		x	= GetScrollPos(SB_HORZ);
		x	-= m_width / 20;
		if(x < 0)
			x = 0;
		m_offset.x = x;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		UpdateScrollBars();
		SetScrollPos(SB_HORZ, m_offset.x);
		ScrollWindow(pos - m_offset.x, 0);
		break;

	case SB_LINEDOWN:
		pos	= m_offset.x;
		x	= GetScrollPos(SB_HORZ);
		x	+= m_width / 20;
		if(x > (MAX_SIZE - m_width))
			x = MAX_SIZE - m_width;
		m_offset.x = x;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		UpdateScrollBars();
		SetScrollPos(SB_HORZ, m_offset.x);
		ScrollWindow(pos - m_offset.x, 0);
		break;

	case SB_PAGEUP:
		x	= GetScrollPos(SB_HORZ);
		x	-= m_width;
		if(x < 0)
			x = 0;
		m_offset.x = x;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		SetScrollPos(SB_HORZ, m_offset.x);
		Invalidate();
		break;

	case SB_PAGEDOWN:
		x	= GetScrollPos(SB_HORZ);
		x	+= m_width;
		if(x > (MAX_SIZE - m_width))
			x = MAX_SIZE - m_width;
		m_offset.x 	= x;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		SetScrollPos(SB_HORZ, m_offset.x);
		Invalidate();
		break;

	case SB_THUMBTRACK:
		x			= m_offset.x - (int)nPos;
		m_offset.x	= (int)nPos;
		m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
		ScrollWindow(x, 0);
		SetScrollPos(SB_HORZ, (int)nPos);
		break;
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnMouseWheel
//------------------------------------------------------------------------------
BOOL 
CChildView::OnMouseWheel(
						 IN UINT, 
						 IN short	zDelta, 
						 IN CPoint
						 ) 
/*++

Routine Description:

	Respond to mouse wheel movement (VScroll)

Arguments:

	IN nFlags -	State of mouse buttons and control/shift keys
	IN zDelta -	Mouse wheel movement
	IN pt -		Mouse location

Return Value:

	None

--*/
{
	int			pos;
	SCROLLINFO	scrollInfo;
	int			y;

	if(m_pGraph && GetScrollInfo(SB_VERT, &scrollInfo, SIF_PAGE))
	{
		if(zDelta > 0)
		{
			pos	= m_offset.y;
			y	= GetScrollPos(SB_VERT);
			y	-= m_height / 20;
			if(y < 0)
				y = 0;
			m_offset.y = y;
			m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
			SetScrollPos(SB_VERT, m_offset.y);
			ScrollWindow(0, pos - m_offset.y);
		}
		else if(zDelta < 0)
		{
			pos	= m_offset.y;
			y	= GetScrollPos(SB_VERT);
			y	+= m_height / 20;
			if(y > (MAX_SIZE - (int)scrollInfo.nPage))
				y = MAX_SIZE - (int)scrollInfo.nPage;
			m_offset.y = y;
			m_pGraph->SetRect(CRect(m_offset.x, m_offset.y, m_offset.x + m_width, m_offset.y + m_height));
			SetScrollPos(SB_VERT, m_offset.y);
			ScrollWindow(0, pos - m_offset.y);
		}
	}
	return TRUE;
}

//------------------------------------------------------------------------------
//	CChildView::OnMouseMove
//------------------------------------------------------------------------------
void 
CChildView::OnMouseMove(
						IN UINT		nFlags, 
						IN CPoint	point
						) 
/*++

Routine Description:

	Passes mouse move events to the grid

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was pressed

Return Value:

	None

--*/
{
	if(m_pGraph)
		m_pGraph->OnMouseMove(nFlags, point);
}

//------------------------------------------------------------------------------
//	CChildView::EnableMenuItem
//------------------------------------------------------------------------------
void 
CChildView::EnableMenuItem(
						   IN CMenu*	pPopupMenu,
						   IN UINT		nIndex
						   )
/*++

Routine Description:

	Enables or disables menu items

Arguments:

	IN pPopupMenu -	Menu
	IN nIndex -		Item

Return Value:

	None

--*/
{
	if(!m_pGraph)
		return;

	switch(nIndex)
	{
	case 0:	// File
		pPopupMenu->EnableMenuItem(ID_FILE_NEW, m_pGraph->AreEffectsLoaded() ? MF_ENABLED : MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_FILE_OPEN, m_pGraph->AreEffectsLoaded() ? MF_ENABLED : MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_FILE_SAVE, m_pGraph->AreEffectsLoaded() ? MF_ENABLED : MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_FILE_SAVE_AS, m_pGraph->AreEffectsLoaded() ? MF_ENABLED : MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_FILE_GENERATE_AND_SAVE_IMAGE, 
								   (m_pGraph->AreEffectsLoaded() && 
								   (strlen(m_pGraph->GetIniFilename()) > 4)) ? MF_ENABLED : MF_GRAYED);
		break;

	case 1:	// View
		pPopupMenu->CheckMenuItem(ID_GRID_SHOW_GRID, m_pGraph->GetShowGrid() ? MF_CHECKED : MF_UNCHECKED);
		break;

	case 2:	// Xbox
		pPopupMenu->EnableMenuItem(ID_XBOX_TRANSMIT_IMAGE, 
								   (m_pGraph->AreEffectsLoaded() &&  
								    (strlen(m_pGraph->GetIniFilename()) > 4)) ? MF_ENABLED : MF_GRAYED);
		break;

	case 3: // Tools
		pPopupMenu->CheckMenuItem(IDC_TOOLS_SNAP_TO_GRID, m_pGraph->GetSnapToGrid() ? MF_CHECKED : MF_UNCHECKED);
		break;
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnFileGenerateAndSaveImage
//------------------------------------------------------------------------------
void 
CChildView::OnFileGenerateAndSaveImage(void) 
/*++

Routine Description:

	Saves the data as a dsp image

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pGraph)
		m_pGraph->SaveImage();
}

//------------------------------------------------------------------------------
//	CChildView::OnFileSave
//------------------------------------------------------------------------------
void 
CChildView::OnFileSave(void) 
/*++

Routine Description:

	Saves the data as a grid file

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pGraph)
		m_pGraph->Save(NULL);
}

//------------------------------------------------------------------------------
//	CChildView::OnFileSaveAs
//------------------------------------------------------------------------------
void 
CChildView::OnFileSaveAs(void) 
/*++

Routine Description:

	Saves the grid as a new name

Arguments:

	None

Return Value:

	None

--*/
{
	if(!m_pGraph)
		return;

	CFileDialog	fileDialog(FALSE, _T("fx"), NULL, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
						   _T("DSP Builder Files (.fx)|*.fx|All Files (*.*)|*.*||"));
	if(fileDialog.DoModal() == IDOK)
		m_pGraph->Save(fileDialog.GetPathName());
}

//------------------------------------------------------------------------------
//	CChildView::OnFileNew
//------------------------------------------------------------------------------
void 
CChildView::OnFileNew(void) 
/*++

Routine Description:

	Create a new grid file

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pGraph)
		m_pGraph->New();
}

//------------------------------------------------------------------------------
//	CChildView::OnFileOpen
//------------------------------------------------------------------------------
void 
CChildView::OnFileOpen(void) 
/*++

Routine Description:

	Opens a new grid file

Arguments:

	None

Return Value:

	None

--*/
{
	if(!m_pGraph)
		return;

	CFileDialog	fileDialog(TRUE, _T("fx"), NULL, 
						   OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
						   _T("DSP Builder Files (.fx)|*.fx|All Files (*.*)|*.*||"));
	if(fileDialog.DoModal() == IDOK)
		m_pGraph->Open(fileDialog.GetPathName());
}

//------------------------------------------------------------------------------
//	CChildView::Connect
//------------------------------------------------------------------------------
BOOL
CChildView::Connect(
					IN LPCTSTR	pNameOrIP
					) 
/*++

Routine Description:

	Connects to the xbox. This routine can take a while. It is recommended
	that a wait cursor be displayed before calling.

Arguments:

	IN pNameOrIP -	Name or IP address of Xbox

Return Value:

	TRUE if a connection was made, FALSE otherwise

--*/
{
	LPVOID			lpMsgBuf;
    int				ret;
	sockaddr_in		sa;
	SOCKADDR		bindSockaddr;
	unsigned long	addr;
	int				index;
	CString			portString;
	ULONG			ulTitleAddr;
	CString			string	= pNameOrIP;
	unsigned short	port	= 80;

	// Disconnect
	Disconnect();

	// Cleanup the string and save it
	string.TrimLeft();
	string.TrimRight();
	m_xboxName = string;

	// Is a port listed?
	index = string.Find(':');
	if(index >= 0)
	{
		portString = string.Right(string.GetLength() - index - 1);
		portString.TrimLeft();
		if(!portString.IsEmpty())
			port = (unsigned short)atol((LPCTSTR)portString);
		string.Delete(index, string.GetLength() - index);
	}

	memset(&sa, 0, sizeof(SOCKADDR));
	sa.sin_family	= AF_INET;
	sa.sin_port		= htons(port);

	// Is it an ip address?
	addr = inet_addr((LPCTSTR)string);
	if(addr != INADDR_NONE)
		sa.sin_addr.s_addr = addr;

	// Otherwise treat it as a name
	else
	{
		// Save the old name to restore
		if(FAILED(DmSetXboxNameNoRegister((LPCTSTR)string)) || 
		   FAILED(DmGetAltAddress(&ulTitleAddr)))
		{
			AfxMessageBox(_T("Unable to resolve Xbox name"), MB_OK | MB_ICONERROR);
			return FALSE;
		}
		sa.sin_addr.s_addr = htonl(ulTitleAddr);
	}

	// Create the socket
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_socket == INVALID_SOCKET)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					  FORMAT_MESSAGE_FROM_SYSTEM | 
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, (unsigned long)WSAGetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&lpMsgBuf, 0, NULL);
		AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONERROR);
		LocalFree(lpMsgBuf);
		return FALSE;
	}

	// Bind the socket
	memset(&bindSockaddr, 0, sizeof(SOCKADDR));
	bindSockaddr.sa_family = AF_INET;
	ret = bind(m_socket, (LPSOCKADDR)&bindSockaddr, sizeof(SOCKADDR));
	if(ret == SOCKET_ERROR)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					  FORMAT_MESSAGE_FROM_SYSTEM | 
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, (unsigned long)WSAGetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&lpMsgBuf, 0, NULL);
		AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONERROR);
		LocalFree(lpMsgBuf);
		return FALSE;
	}

	// Connect
	ret = connect(m_socket, (LPSOCKADDR)&sa, sizeof(SOCKADDR));
	if(SOCKET_ERROR == ret)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					  FORMAT_MESSAGE_FROM_SYSTEM | 
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, (unsigned long)WSAGetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&lpMsgBuf, 0, NULL);
		AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONERROR);
		LocalFree(lpMsgBuf);
		return FALSE;
	}
	
	// We are connected
	m_bConnected = TRUE;
	return TRUE;
}

//------------------------------------------------------------------------------
//	CChildView::Disconnect
//------------------------------------------------------------------------------
void
CChildView::Disconnect(void)
/*++

Routine Description:

	Disconnects from an Xbox

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_bConnected)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
		m_bConnected = FALSE;
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnXboxTransmitImage
//------------------------------------------------------------------------------
void 
CChildView::OnXboxTransmitImage(void) 
/*++

Routine Description:

	Transmits the dsp image to the xbox. This method assumes that a valid image
	is stored on disk and that a connection exists.

Arguments:

	None

Return Value:

	None

--*/
{
	DWORD			i;
	int				ii;
	CFile			file;
	CStdioFile		sFile;
	char*			pBuffer;
	DWORD			length;
	LPVOID			lpMsgBuf;
	char			header[36];
	CString			string;
	CString			name;
	CString			index;
	DWORD			e;
	char*			pEffectName[1024];	// No more than 1024 effects per file
	char*			pEffectIndex[1024];	// No more than 1024 effects per file
	CString			pEffectName2[1024];	// No more than 1024 effects per file
	DWORD			numEffects	= 0;
	BOOL			error		= FALSE;
	CConnectDialog	dialog(m_xboxName);

	if(!m_pGraph)
		return;

	// If changes have been made, try to rebuild
	if(m_pGraph->GetChangesMade() || !m_pGraph->IsImageSaved())
	{
		if(AfxMessageBox(_T("Changes have been made since you last generated the image.\n"
			   			    "Hit Ok to rebuild."), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
			return;
		if(!m_pGraph->SaveImage())
			return;
	}

	// Does a potentialy valid destination exist?
	if(m_transmitDestination.GetLength() == 0)
	{
		OnToolsTransmitOptions();
		if(m_transmitDestination.GetLength() == 0)
			return;
	}

	CWaitCursor cursor;
	if(!Connect(m_transmitDestination))
		return;

	// Open the binary file
	file.SetFilePath(m_pGraph->GetBinFilename());
	if(!file.Open(file.GetFilePath(), CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(CString(_T("Unable to open file: ")) + file.GetFilePath(),
					  MB_OK | MB_ICONERROR);
		return;
	}

	// Get the file size and allocate a buffer
	length	= file.GetLength();
	pBuffer	= new char [length+25];

	// Add the other data to the buffer
	memcpy(pBuffer, "DSPIMAGE", 8);
	*(BYTE*)&pBuffer[8]		= cDspImageVersion;
	*(DWORD*)&pBuffer[9]	= m_pGraph->GetI3DL2Index();
	*(DWORD*)&pBuffer[13]	= m_pGraph->GetXTalkIndex();
	memcpy(&pBuffer[length+17], "DSPIMAGE", 8);

	// Read the entire file into a buffer
	if(file.Read(&pBuffer[17], length) != length)
	{
		AfxMessageBox(CString(_T("Error reading dsp file: ")) + file.GetFilePath(),
					  MB_OK | MB_ICONERROR);
		delete [] pBuffer;
		file.Close();
		return;
	}
	file.Close();

	// Adjust the length for additional info
	length += 25;

	// Build the header block
	memcpy(header, "DSPBUILDER", 10);
	strncpy(&header[10], "DSPIMAGE", 8);
	*(DWORD*)&header[18] = length;
	*(DWORD*)&header[22] = GetCRC(pBuffer, length);
	memcpy(&header[26], "DSPBUILDER", 10);

	// Send the header
	if(send(m_socket, header, 36, 0) == SOCKET_ERROR)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					  FORMAT_MESSAGE_FROM_SYSTEM | 
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, (unsigned long)WSAGetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&lpMsgBuf, 0, NULL);
		AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONERROR);
		LocalFree(lpMsgBuf);
		error = TRUE;
	}

	// Send the data block
	else if(send(m_socket, pBuffer, (int)length, 0) == SOCKET_ERROR)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					  FORMAT_MESSAGE_FROM_SYSTEM | 
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, (unsigned long)WSAGetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&lpMsgBuf, 0, NULL);
		AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONERROR);
		LocalFree(lpMsgBuf);
		error = TRUE;
	}
	
	// Free memory
	delete [] pBuffer;

	if(!error)
	{
		// Open the header file
		sFile.SetFilePath(m_pGraph->GetHFilename());
		if(!sFile.Open(sFile.GetFilePath(), CFile::modeRead | CFile::typeText))
		{
			AfxMessageBox(CString(_T("Unable to open file: ")) + sFile.GetFilePath(),
						  MB_OK | MB_ICONERROR);
			return;
		}

		// Parse the file and find the effects indices
		while(sFile.ReadString(string))
		{
			// Look for the index block
			// All lines after it will contain the names and indices
			if((string.Find(_T("typedef enum _DSP_IMAGE_")) == 0) && 
			   (string.Find(_T("_INDICES {")) != -1))
			{
				while(sFile.ReadString(string))
				{
					// Clean up the string
					string.TrimLeft();
					string.TrimRight();

					// Is an effect name/index pair found
					ii = string.Find(_T(" = "));
					if(ii == -1)
						break;

					name	= string.Left(ii);
					index	= string.Right(string.GetLength() - ii - 3);
					ii		= index.Find(',');
					if(ii != -1)
						index.Delete(ii, index.GetLength() - ii);
					
					pEffectName[numEffects] = new char [(unsigned int)name.GetLength() + 1];
					ASSERT(pEffectName[numEffects] != NULL);
					pEffectIndex[numEffects] = new char [(unsigned int)index.GetLength() + 1];
					ASSERT(pEffectIndex[numEffects] != NULL);

					// For now (this is crap) convert to pure ascii
					for(i=0; i<(DWORD)name.GetLength(); ++i)
						pEffectName[numEffects][i] = (char)(name.GetAt((int)i) & 0xff);
					pEffectName[numEffects][i] = 0;
					for(i=0; i<(DWORD)index.GetLength(); ++i)
						pEffectIndex[numEffects][i] = (char)(index.GetAt((int)i) & 0xff);
					pEffectIndex[numEffects][i] = 0;

					pEffectName2[numEffects] = name;
					++numEffects;
				}
				break;
			}
		} 
		sFile.Close();

		// Calculate the buffer size to transmit
		// Intial 25 bytes are for header/trailer 
		// (8x2) version (1) and one size values (4)
		for(length=21, i=0; i<numEffects; ++i)
		{
			// Effect name + null
			length += strlen(pEffectName[i]) + 1;

			// Effect index + null
			length += strlen(pEffectIndex[i]) + 1;

			// Set the indices for each module
			m_pGraph->SetModuleIndex(pEffectName2[i], (DWORD)atoi(pEffectIndex[i]));
		}

		// Get the file size and allocate a buffer
		pBuffer	= new char [length];

		// Add the other data to the buffer
		memcpy(pBuffer, "DSPINDEX", 8);
		*(BYTE*)&pBuffer[8]		= cDspIndexVersion;
		*(DWORD*)&pBuffer[9]	= numEffects;
		memcpy(&pBuffer[length-8], "DSPINDEX", 8);

		// Fill the effects into the buffer
		for(i=13, e=0; e<numEffects; ++e)
		{
			for(ii=0; pEffectName[e][ii] != 0; ++ii, ++i)
				pBuffer[i] = pEffectName[e][ii];
			pBuffer[i++] = pEffectName[e][ii];
	
			for(ii=0; pEffectIndex[e][ii] != 0; ++ii, ++i)
				pBuffer[i] = pEffectIndex[e][ii];
			pBuffer[i++] = pEffectIndex[e][ii];
		}

		// Build the header block
		memcpy(header, "DSPBUILDER", 10);
		memcpy(&header[10], "DSPINDEX", 8);
		*(DWORD*)&header[18] = length;
		*(DWORD*)&header[22] = GetCRC(pBuffer, length);
		memcpy(&header[26], "DSPBUILDER", 10);

		// Send the header
		if(send(m_socket, header, 36, 0) == SOCKET_ERROR)
		{
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						  FORMAT_MESSAGE_FROM_SYSTEM | 
						  FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL, (unsigned long)WSAGetLastError(),
						  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						  (LPTSTR)&lpMsgBuf, 0, NULL);
			AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONERROR);
			LocalFree(lpMsgBuf);
			error = TRUE;
		}

		// Send the data block
		else if(send(m_socket, pBuffer, (int)length, 0) == SOCKET_ERROR)
		{
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						  FORMAT_MESSAGE_FROM_SYSTEM | 
						  FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL, (unsigned long)WSAGetLastError(),
						  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						  (LPTSTR)&lpMsgBuf, 0, NULL);
			AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONERROR);
			LocalFree(lpMsgBuf);
			error = TRUE;
		}
		
		// Free memory
		delete [] pBuffer;
	}

	// Free resources
	for(i=0; i<numEffects; ++i)
	{
		delete [] pEffectName[i];
		delete [] pEffectIndex[i];
	}
	
	// If any transmission errors occured, disconnect
	if(error)
		Disconnect();
}

//------------------------------------------------------------------------------
//	CChildView::OnFileExit
//------------------------------------------------------------------------------
BOOL
CChildView::Quit(void)
/*++

Routine Description:

	Prompts user to save data and then exits

Arguments:

	None

Return Value:

	TRUE to quit, FALSE otherwise

--*/
{
	if(!m_pGraph)
		return TRUE;

	if(!m_pGraph->IsEmpty() && m_pGraph->GetChangesMade())
	{
		switch(AfxMessageBox(_T("Save changes?"), MB_YESNOCANCEL | MB_ICONEXCLAMATION))
		{
		case IDYES:
			if(!m_pGraph->Save(NULL))
				return FALSE;
			break;
		case IDCANCEL:
			return FALSE;
		default:
			break;
		}
	}
	return TRUE;
}

//------------------------------------------------------------------------------
//	CChildView::OnUpdateXboxTransmitImageUI
//------------------------------------------------------------------------------
void
CChildView::OnUpdateXboxTransmitImageUI(
									    IN CCmdUI*	pCmdUI
									    )
{
	if(!m_pGraph)
		return;

	pCmdUI->Enable((m_pGraph->AreEffectsLoaded() &&  
					(strlen(m_pGraph->GetIniFilename()) > 4)) ? TRUE : FALSE);
}

//------------------------------------------------------------------------------
//	CChildView::OnUpdateFileNewUI
//------------------------------------------------------------------------------
void
CChildView::OnUpdateFileNewUI(
							  IN CCmdUI*	pCmdUI
							  )
{
	if(!m_pGraph)
		return;

	pCmdUI->Enable(m_pGraph->AreEffectsLoaded());
}

//------------------------------------------------------------------------------
//	CChildView::OnUpdateFileSaveUI
//------------------------------------------------------------------------------
void
CChildView::OnUpdateFileSaveUI(
							   IN CCmdUI*	pCmdUI
							   )
{
	if(!m_pGraph)
		return;

	pCmdUI->Enable(m_pGraph->AreEffectsLoaded());
}

//------------------------------------------------------------------------------
//	CChildView::OnUpdateFileOpenUI
//------------------------------------------------------------------------------
void
CChildView::OnUpdateFileOpenUI(
							   IN CCmdUI*	pCmdUI
							   )
{
	if(!m_pGraph)
		return;

	pCmdUI->Enable(m_pGraph->AreEffectsLoaded());
}

//------------------------------------------------------------------------------
//	CChildView::OnUpdateFileGenSaveImgUI
//------------------------------------------------------------------------------
void 
CChildView::OnUpdateFileGenSaveImgUI(
									 IN CCmdUI* pCmdUI
									 )
{
	if(!m_pGraph)
		return;

	pCmdUI->Enable((m_pGraph->AreEffectsLoaded() && 
				    strlen(m_pGraph->GetIniFilename())) ? TRUE : FALSE);
}

//------------------------------------------------------------------------------
//	CChildView::GetStatusBarText
//------------------------------------------------------------------------------
LPCTSTR 
CChildView::GetStatusBarText(void)
{
	if(!m_pGraph)
		return NULL;

	m_statusBarText.Format(_T("DSP Cycles: %d (%d%%) Y Mem Size: %d (%d%%) "
						   "Scratch Length: %d"), 
						   m_pGraph->GetDSPCycles(), 
						   (int)(m_pGraph->GetDSPCyclesPercent() * 100.0f),
						   m_pGraph->GetYMemSize(), 
						   (int)(m_pGraph->GetYMemSizePercent() * 100.0f),
						   m_pGraph->GetScratchLength());
	return (LPCTSTR)m_statusBarText; 
}

//------------------------------------------------------------------------------
//	CChildView::Reset
//------------------------------------------------------------------------------
void
CChildView::Reset(void)
/*++

Routine Description:

	Resets all offsets and scrollbars

Arguments:

	None

Return Value:

	None

--*/
{
	SCROLLINFO	scrollInfo;
	CRect		rect;

	m_offset.x	= 0;
	m_offset.y	= 0;

	// Reset the graph
	if(m_pGraph)
	{
		GetClientRect(rect);
		m_pGraph->SetRect(rect);
	}

	// Vertical scroll bar
	scrollInfo.cbSize		= sizeof(SCROLLINFO);
	scrollInfo.fMask		= SIF_ALL;
	scrollInfo.nMin			= 0;
	scrollInfo.nMax			= MAX(1, m_height - 1);
	scrollInfo.nPage		= (UINT)scrollInfo.nMax;
	scrollInfo.nPos			= 0;
	scrollInfo.nTrackPos	= 0;
	SetScrollInfo(SB_VERT, &scrollInfo);

	// Horizontal scroll bar (hidden for now)
	scrollInfo.cbSize		= sizeof(SCROLLINFO);
	scrollInfo.fMask		= SIF_ALL;
	scrollInfo.nMin			= 0;
	scrollInfo.nMax			= MAX(1, m_width - 1);
	scrollInfo.nPage		= (UINT)scrollInfo.nMax;
	scrollInfo.nPos			= 0;
	scrollInfo.nTrackPos	= 0;
	SetScrollInfo(SB_HORZ, &scrollInfo);
}

//------------------------------------------------------------------------------
//	CChildView::OnToolsDirectories
//------------------------------------------------------------------------------
void 
CChildView::OnToolsDirectories(void)
/*++

Routine Description:

	Displays a dialog that allows the user the ability to select the 
	directory paths for the application

Arguments:

	None

Return Value:

	None

--*/
{
	CDirsDialog	dirsDialog;
	
	if(m_pGraph)
	{
		dirsDialog.SetIniPath(m_pGraph->GetIniFilePath());
		dirsDialog.SetDspCodePath(m_pGraph->GetDspCodePath());

		if(dirsDialog.DoModal() == IDOK)
			m_pGraph->SetPaths(dirsDialog.GetIniPath(), dirsDialog.GetDspCodePath());
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnToolsBuildOptions
//------------------------------------------------------------------------------
void 
CChildView::OnToolsBuildOptions(void)
/*++

Routine Description:

	Displays a dialog that allows the user the ability to select the 
	build options for the image

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pGraph)
		m_pGraph->SelectBuildOptions();
}

//------------------------------------------------------------------------------
//	CChildView::OnToolsTransmitOptions
//------------------------------------------------------------------------------
void 
CChildView::OnToolsTransmitOptions(void)
/*++

Routine Description:

	Displays a dialog that allows the user the ability to select the 
	destination of the image

Arguments:

	None

Return Value:

	None

--*/
{
	CConnectDialog	dialog(m_xboxName);

	if(dialog.DoModal() == IDOK)
	{
		// If the name is different, and we are connected to an Xbox, disconnect
		if(m_transmitDestination != dialog.GetName())
			Disconnect();
		m_transmitDestination = dialog.GetName();
	}
}

//------------------------------------------------------------------------------
//	CChildView::OnGraphShowGrid
//------------------------------------------------------------------------------
void
CChildView::OnGraphShowGrid(void)
/*++

Routine Description:

	Toggles the grid display

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pGraph)
		m_pGraph->ToggleGrid();
}

//------------------------------------------------------------------------------
//	CChildView::OnToolsSnapToGrid
//------------------------------------------------------------------------------
void
CChildView::OnToolsSnapToGrid(void)
/*++

Routine Description:

	Toggles the snap to grid

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pGraph)
		m_pGraph->ToggleSnapToGrid();
}

//------------------------------------------------------------------------------
//	CChildView::UpdateScrollBars
//------------------------------------------------------------------------------
void
CChildView::UpdateScrollBars(void)
/*++

Routine Description:

	Updates the scrollbars

Arguments:

	None

Return Value:

	None

--*/
{
	int			x;
	int			y;
	SCROLLINFO	scrollInfo;
	CRect		bounds;
	
	if(!m_pGraph)
		return;

	bounds = m_pGraph->GetBounds();

	x	= m_width + m_offset.x;
	y	= m_height + m_offset.y;
	x	= MAX(x, bounds.right);
	y	= MAX(y, bounds.bottom);

	// Adjust the scroll bars
	scrollInfo.cbSize	= sizeof(SCROLLINFO);
	scrollInfo.fMask	= SIF_PAGE | SIF_RANGE;
	scrollInfo.nMin		= 0;
	scrollInfo.nPage	= (UINT)MAX(0, m_height - 1);
	scrollInfo.nMax		= MAX(0, y - 1);
	SetScrollInfo(SB_VERT, &scrollInfo);

	scrollInfo.nPage	= (UINT)MAX(0, m_width - 1);
	scrollInfo.nMax		= MAX(0, x - 1);
	SetScrollInfo(SB_HORZ, &scrollInfo);
}

//------------------------------------------------------------------------------
//	CChildView::TransmitModuleParameters
//------------------------------------------------------------------------------
void
CChildView::TransmitModuleParameters(
									 IN const CModule*	pModule,
									 IN int				index	// = -1
									 )
/*++

Routine Description:

	Transmits the module parameters to a connected Xbox

Arguments:

	IN pModule -	Module whose parameters should be transmitted
	IN index -		Parameter index (-1 for all)

Return Value:

	None

--*/
{
	char	header[36];
	char*	pBuffer;
	DWORD	length;
	DWORD	i;
	DWORD	p;
	LPVOID	lpMsgBuf;
	DWORD	dwFirst;
	DWORD	dwLast;

	// If not connected to the Xbox, just return
	if(!m_bConnected)
		return;
	
	// Special Case: I3DL2
	if(pModule->IsI3DL2())
	{
		length	= 17 + (4 * sizeof(LONG)) + (8 * sizeof(FLOAT));
		pBuffer = new char [length];

		// Head
		memcpy(pBuffer, "I3DL2LIS", 8);

		// Version
		*(BYTE*)&pBuffer[8] = cI3DL2Version;

		// I3DL2 Parameters
		i = 9;
		*(LONG*)&pBuffer[i]  = pModule->GetI3DL2Listener().lRoom; i += sizeof(LONG);
		*(LONG*)&pBuffer[i]  = pModule->GetI3DL2Listener().lRoomHF; i += sizeof(LONG);
		*(FLOAT*)&pBuffer[i] = pModule->GetI3DL2Listener().flRoomRolloffFactor; i += sizeof(FLOAT);
		*(FLOAT*)&pBuffer[i] = pModule->GetI3DL2Listener().flDecayTime; i += sizeof(FLOAT);
		*(FLOAT*)&pBuffer[i] = pModule->GetI3DL2Listener().flDecayHFRatio; i += sizeof(FLOAT);
		*(LONG*)&pBuffer[i]  = pModule->GetI3DL2Listener().lReflections; i += sizeof(LONG);
		*(FLOAT*)&pBuffer[i] = pModule->GetI3DL2Listener().flReflectionsDelay; i += sizeof(FLOAT);
		*(LONG*)&pBuffer[i]  = pModule->GetI3DL2Listener().lReverb; i += sizeof(LONG);
		*(FLOAT*)&pBuffer[i] = pModule->GetI3DL2Listener().flReverbDelay; i += sizeof(FLOAT);
		*(FLOAT*)&pBuffer[i] = pModule->GetI3DL2Listener().flDiffusion; i += sizeof(FLOAT);
		*(FLOAT*)&pBuffer[i] = pModule->GetI3DL2Listener().flDensity; i += sizeof(FLOAT);
		*(FLOAT*)&pBuffer[i] = pModule->GetI3DL2Listener().flHFReference; i += sizeof(FLOAT);

		// Tail
		memcpy(&pBuffer[length-8], "I3DL2LIS", 8);

		// Build the header block
		memcpy(header, "DSPBUILDER", 10);
		memcpy(&header[10], "I3DL2LIS", 8);
		*(DWORD*)&header[18] = length;
		*(DWORD*)&header[22] = GetCRC(pBuffer,length);
		memcpy(&header[26], "DSPBUILDER", 10);
	}

	// All other effects (including IIR2)
	else 
	{
		if((pModule->GetParameters().GetSize() == 0) || (index >= pModule->GetParameters().GetSize()))
			return;
		
		// All parameters?
		if(index == -1)
		{
			dwFirst	= 0;
			dwLast	= pModule->GetParameters().GetSize()-1;
		}
		else
		{
			dwFirst	= (DWORD)index;
			dwLast	= (DWORD)index;
		}

		length	= 21 + (16 * (dwLast - dwFirst + 1));
		pBuffer = new char [length];

		// Head
		memcpy(pBuffer, "FXPARAMS", 8);

		// Version
		*(BYTE*)&pBuffer[8] = cFXParamsVersion;

		// Number of params
		i = 9;
		*(DWORD*)&pBuffer[i] = dwLast - dwFirst + 1; 
		i += sizeof(DWORD);

		// Step through all the params
		for(p=dwFirst; p<=dwLast; ++p)
		{
			// Effect index
			*(DWORD*)&pBuffer[i] = pModule->GetIndex();
			i += sizeof(DWORD);

			// Data offset
			*(DWORD*)&pBuffer[i] = pModule->GetParameters()[p].GetOffset() * sizeof(DWORD) + 
								   (DWORD)pModule->GetInputs().GetSize() * sizeof(DWORD) +
								   (DWORD)pModule->GetOutputs().GetSize() * sizeof(DWORD) +
								   m_dwParamOffset;
			i += sizeof(DWORD);

			// Size of effect data
			*(DWORD*)&pBuffer[i] = sizeof(DWORD);
			i += sizeof(DWORD);

			// Effect data
			*(DWORD*)&pBuffer[i] = pModule->GetParameters()[p].GetValue();
			i += sizeof(DWORD);
		}

		// Tail
		memcpy(&pBuffer[length-8], "FXPARAMS", 8);

		// Build the header block
		memcpy(header, "DSPBUILDER", 10);
		memcpy(&header[10], "FXPARAMS", 8);
		*(DWORD*)&header[18] = length;
		*(DWORD*)&header[22] = GetCRC(pBuffer,length);
		memcpy(&header[26], "DSPBUILDER", 10);
	}
	
	// Send the header
	if(send(m_socket, header, 36, 0) == SOCKET_ERROR)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					  FORMAT_MESSAGE_FROM_SYSTEM | 
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, (unsigned long)WSAGetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&lpMsgBuf, 0, NULL);
		AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONERROR);
		LocalFree(lpMsgBuf);
		Disconnect();
	}

	// Send the data block
	else if(send(m_socket, pBuffer, (int)length, 0) == SOCKET_ERROR)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					  FORMAT_MESSAGE_FROM_SYSTEM | 
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, (unsigned long)WSAGetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&lpMsgBuf, 0, NULL);
		AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONERROR);
		LocalFree(lpMsgBuf);
		Disconnect();
	}
	
	// Free memory
	delete [] pBuffer;
}

#pragma warning (push)
#pragma warning (disable:4035)
//------------------------------------------------------------------------------
//	GetCRC
//------------------------------------------------------------------------------
DWORD
CChildView::GetCRC(
				   IN const VOID*	pBuffer,
				   IN ULONG			bufferSize
				   ) const
/*++

Routine Description:

	Generates a simple CRC

Arguments:

	IN pBuffer -	Buffer to generate crc for
	IN bufferSize -	Size of buffer in bytes

Return Value:

	None

--*/
{
    __asm {
        mov     ecx, pBuffer
		mov		edx, bufferSize
		xor		eax, eax
        xor     ebx, ebx
        shr     edx, 2      // count /= sizeof(ULONG)
        test    edx, edx
        jz      L2
    L1: add     eax, [ecx]  // eax += *data++
        adc     ebx, 0      // ebx += carry
        add     ecx, 4
        dec     edx
        jnz     L1          // while (--count)
    L2: add     eax, ebx    // take care of accumulated carries
        adc     eax, 0
    }
}
#pragma warning (pop)
