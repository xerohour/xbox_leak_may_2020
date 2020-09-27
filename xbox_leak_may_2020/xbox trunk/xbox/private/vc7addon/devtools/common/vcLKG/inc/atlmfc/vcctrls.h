//NONSHIP

// This is a part of the Visual C++ Library
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//

#ifndef __VCCTRLS_H__
#define __VCCTRLS_H__

#pragma once

#include <vcc.h>

#pragma pack(push, 8)

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
// General

#ifndef VCCTRLS_STATIC
#ifdef _VCCTRLS_DLL
	#define VCCTRLS_API __declspec(dllexport)
	#else
	#define VCCTRLS_API __declspec(dllimport)
	#pragma comment(lib, "vcctrls.lib")
#endif
#else
	#define VCCTRLS_API
#endif


VCCTRLS_API BOOL InitVcControls(void);

/////////////////////////////////////////////////////////////////////////////
// Docking Windows

// Windows Class Name:
#define DOCKSITEWINDOWCLASSNAMEW	L"VC_DockSiteWindow"
#define DOCKSITEWINDOWCLASSNAMEA	"VC_DockSiteWindow"

#define DOCKWINDOWCLASSNAMEW		L"VC_DockWindow"
#define DOCKWINDOWCLASSNAMEA		"VC_DockWindow"

#ifdef _UNICODE
#define DOCKSITEWINDOWCLASSNAME	DOCKSITEWINDOWCLASSNAMEW
#define DOCKWINDOWCLASSNAME		DOCKWINDOWCLASSNAMEW
#else
#define DOCKSITEWINDOWCLASSNAME	DOCKSITEWINDOWCLASSNAMEA
#define DOCKWINDOWCLASSNAME		DOCKWINDOWCLASSNAMEA
#endif

// Dock Window Style Flags
#define DWS_DOCKLEFT		0x00000001
#define DWS_DOCKTOP			0x00000002
#define DWS_DOCKRIGHT		0x00000004
#define DWS_DOCKBOTTOM		0x00000008
#define DWS_DOCKANY			(DWS_DOCKLEFT | DWS_DOCKTOP | DWS_DOCKRIGHT | DWS_DOCKBOTTOM)
#define DWS_DOCKTABTOP		0x00000010
#define DWS_DOCKTABBOTTOM	0x00000020
#define DWS_DOCKTABLEFT		0x00000040
#define DWS_DOCKTABRIGHT	0x00000080
#define DWS_DOCKTAB			0x00000100
#define DWS_GRIPPER			0x00000200		// draw a gripper instead of a caption bar when docked
#define DWS_FIXEDHEIGHT		0x00000400	// don't allow changing the height of the window
#define DWS_NOEXCLIENTEDGE  0x00000800	// docking frame doesn't have WS_EX_CLIENTEDGE STYLE
#define DWS_TOOLBARBORDER	0x00001000
#define DWS_TOOLBAR3DBORDER	0x00002000

#define DWS_TOOLBAR		(DWS_DOCKTOP | DWS_DOCKBOTTOM | DWS_GRIPPER | DWS_FIXEDHEIGHT)

typedef struct tagDEFAULTDOCKLOCATION
{
	int nDockSide;
	int nDockRow;
	int nDockCol;
} DEFAULTDOCKLOCATION;

// DockSide Values
#define DW_DOCK_TOP		1
#define DW_DOCK_BOTTOM	2
#define DW_DOCK_LEFT	4
#define DW_DOCK_RIGHT	8

typedef struct tagDOCKINFOA
{
	int cbSize;
	int nDockFlags;
	int nID;
	HINSTANCE hInstResource;
	LPCSTR lpstrTitle;
	SIZE sizeDockHorizontal;
	SIZE sizeDockVertical;
	RECT rectFloat;
	DEFAULTDOCKLOCATION dockLoc;
} DOCKINFOA, *LPDOCKINFOA;

typedef struct tagDOCKINFOW
{
	int cbSize;
	int nDockFlags;
	int nID;
	HINSTANCE hInstResource;
	LPCWSTR lpstrTitle;
	SIZE sizeDockHorizontal;
	SIZE sizeDockVertical;
	RECT rectFloat;
	DEFAULTDOCKLOCATION dockLoc;
} DOCKINFOW, *LPDOCKINFOW;

VCCTRLS_API BOOL DockWin_InitDockingSite(HWND hwndFrame, DWORD dwFlags);
VCCTRLS_API HWND DockWin_CreateDockWnd(HWND hwndParent, DWORD dwFlags);

#define DWM_BASE					(WM_USER + 400)

#define DWM_SETVIEWWINDOW			(DWM_BASE + 1)

#define DWM_SETOFFSETS				(DWM_BASE + 2)

#define DWM_GETOFFSETS				(DWM_BASE + 3)

#define DWM_UPDATELAYOUT			(DWM_BASE + 4)

#define DWM_REGISTERDOCKWINDOWA			(DWM_BASE + 21)
#define DWM_REGISTERDOCKWINDOWW			(DWM_BASE + 22)

#define DWM_UNREGISTERDOCKWINDOW		(DWM_BASE + 23)

#define DWM_GETDOCKWINDOWINFOA			(DWM_BASE + 24)
#define DWM_GETDOCKWINDOWINFOW			(DWM_BASE + 25)

#define DWM_SETDOCKWINDOWINFOA			(DWM_BASE + 26)
#define DWM_SETDOCKWINDOWINFOW			(DWM_BASE + 27)


#define DWM_DOCKWINDOW				(DWM_BASE + 28)

#define DWM_FLOATWINDOW				(DWM_BASE + 29)

#define DWM_QUERYDOCKSTATUS			(DWM_BASE + 30)

#define DWM_SHOWDOCKEDCHILDREN		(DWM_BASE + 31)

#define DWM_SETSPLITTERWIDTH			(DWM_BASE + 32)

#ifdef _UNICODE
typedef DOCKINFOW DOCKINFO;
typedef LPDOCKINFOW LPDOCKINFO;
#define DWM_REGISTERDOCKWINDOW DWM_REGISTERDOCKWINDOWW
#define DWM_GETDOCKWINDOWINFO DWM_GETDOCKWINDOWINFOW
#define DWM_SETDOCKWINDOWINFO DWM_SETDOCKWINDOWINFOW
#else
typedef DOCKINFOA DOCKINFO;
typedef LPDOCKINFOA LPDOCKINFO;
#define DWM_REGISTERDOCKWINDOW DWM_REGISTERDOCKWINDOWA
#define DWM_GETDOCKWINDOWINFO DWM_GETDOCKWINDOWINFOA
#define DWM_SETDOCKWINDOWINFO DWM_SETDOCKWINDOWINFOA
#endif


/////////////////////////////////////////////////////////////////////////////
// TAB Windows

VCCTRLS_API HWND CreateTabWindow(HWND hWndParent, DWORD dwStyle);

#define TBWM_BASE					(WM_USER + 450)

#define TBWM_ADDCHILD				(TBWM_BASE + 1)

#define TBWM_REMOVECHILD			(TBWM_BASE + 2)

#define TBWM_CANADDCHILD			(TBWM_BASE + 3)

#define TBWM_GETCHILDRECT			(TBWM_BASE + 4)


/////////////////////////////////////////////////////////////////////////////
// Command Bars

// Windows Class Name:
#define COMMANDBARCLASSNAMEW	L"VC_CommandBar"
#define COMMANDBARCLASSNAMEA	"VC_CommandBar"

#ifdef _UNICODE
#define COMMANDBARCLASSNAME	COMMANDBARCLASSNAMEW
#else
#define COMMANDBARCLASSNAME	COMMANDBARCLASSNAMEA
#endif

// Window Styles:
#define CBRWS_TOP		0x00000001L
#define CBRWS_BOTTOM		0x00000003L
#define CBRWS_NORESIZE		0x00000004L
#define CBRWS_NOPARENTALIGN	0x00000008L
#define CBRWS_NODIVIDER		0x00000040L
#define CBRWS_TRANSPARENT	0x00008000L

// Functions:
VCCTRLS_API HWND CommandBar_Create(HWND hWndParent, DWORD dwStyle, int nID);	// creates a command bar window
VCCTRLS_API BOOL CommandBar_AttachToWindow(HWND hWnd);				// creates a command bar and attaches it to a window.

// Messages:
#define CBRM_FIRST			(WM_USER + 301)
#define CBRM_LAST			(WM_USER + 320)

#define CBRM_LOADMENUA			(WM_USER + 301)	// loads a menu from a resource
#define CBRM_LOADMENUW			(WM_USER + 302)
#define CBRM_ATTACHMENU			(WM_USER + 303)	// attaches a menu handle
#define CBRM_LOADIMAGESA		(WM_USER + 304)	// loads toolbar resource
#define CBRM_LOADIMAGESW		(WM_USER + 305)
#define CBRM_ADDIMAGEA			(WM_USER + 306)	// adds an image form a resource or handle
#define CBRM_ADDIMAGEW			(WM_USER + 307)
#define CBRM_GETMENU			(WM_USER + 308)	// returns loaded or attached menu

#define CBRM_GETIMAGEMASKCOLOR		(WM_USER + 309)	// return image mask color
#define CBRM_SETIMAGEMASKCOLOR		(WM_USER + 310)	// sets image mask color

#define CBRM_REPLACEIMAGEA		(WM_USER + 311)	// replaces an image by command ID
#define CBRM_REPLACEIMAGEW		(WM_USER + 312)
#define CBRM_REMOVEIMAGE		(WM_USER + 313)	// removes an image by command ID
#define CBRM_REMOVEALLIMAGES		(WM_USER + 314)	// removes all images
#define CBRM_GETIMAGESVISIBLE		(WM_USER + 315)	// returns if images will be displayed
#define CBRM_SETIMAGESVISIBLE		(WM_USER + 316)	// sets if images will be displayed
#define CBRM_GETIMAGESIZE		(WM_USER + 317)	// returns image size
#define CBRM_SETIMAGESIZE		(WM_USER + 318)	// sets image size

#define CBRM_TRACKPOPUPMENU		(WM_USER + 319)	// displays a popup menu
#define CBRM_SETMDICLIENT		(WM_USER + 320)	// sets an MDI client window

#ifdef UNICODE
#define CBRM_LOADMENU		CBRM_LOADMENUW
#define CBRM_LOADIMAGES		CBRM_LOADIMAGESW
#define CBRM_ADDIMAGE		CBRM_ADDIMAGEW
#define CBRM_REPLACEIMAGE	CBRM_REPLACEIMAGEW
#else
#define CBRM_LOADMENU		CBRM_LOADMENUA
#define CBRM_LOADIMAGES		CBRM_LOADIMAGESA
#define CBRM_ADDIMAGE		CBRM_ADDIMAGEA
#define CBRM_REPLACEIMAGE	CBRM_REPLACEIMAGEA
#endif // UNICODE

// flags for CBRADDIMAGE structure
#define CBRAB_HANDLE	0x0001
#define CBRAB_RESOURCE	0x0002
#define CBRAB_BITMAP	0x0010
#define CBRAB_ICON	0x0020

// Stuctures:
typedef struct tagCBRADDIMAGEA
{
	int cbSize;
	UINT fMask;	// CBRAB_HANDLE, CBRAB_RESOURCE, CBRAB_BITMAP, CBRAB_ICON
	HINSTANCE hInstance;
	union
	{
		LPCSTR lpstrResource;
		HBITMAP hBitmap;
		HICON hIcon;
	};
	int nCmdCount;
	int* arrCommands;
} CBRADDIMAGEA, *LPCBRADDIMAGEA;

typedef struct tagCBRADDIMAGEW
{
	int cbSize;
	UINT fMask;	// CBRAB_HANDLE, CBRAB_RESOURCE, CBRAB_BITMAP, CBRAB_ICON
	HINSTANCE hInstance;
	union
	{
		LPCWSTR lpstrResource;
		HBITMAP hBitmap;
		HICON hIcon;
	};
	int nCmdCount;
	int* arrCommands;
} CBRADDIMAGEW, *LPCBRADDIMAGEW;

#ifdef UNICODE
typedef CBRADDIMAGEW CBRADDIMAGE;
typedef LPCBRADDIMAGEW LPCBRADDIMAGE;
#else
typedef CBRADDIMAGEA CBRADDIMAGE;
typedef LPCBRADDIMAGEA LPCBRADDIMAGE;
#endif // UNICODE

typedef struct tagCBRPOPUPMENU
{
	int cbSize;
	HMENU hMenu;		// popup menu do display
	UINT uFlags;		// TMP_* flags for TrackPopupMenu[Ex]
	int x;
	int y;
	LPTPMPARAMS lptpm;	// ptr to TPMPARAMS for TrackPopupMenuEx
} CBRPOPUPMENU, *LPCBRPOPUPMENU;

/////////////////////////////////////////////////////////////////////////////
// Property Browser

#define PROPERTYBROWSERCLASSNAMEW	L"VC_PropertyBrowser"
#define PROPERTYBROWSERCLASSNAMEA	"VC_PropertyBrowser"

#ifdef _UNICODE
#define PROPERTYBROWSERCLASSNAME	PROPERTYBROWSERCLASSNAMEW
#else
#define PROPERTYBROWSERCLASSNAME	PROPERTYBROWSERCLASSNAMEA
#endif


#define PBM_FIRST			(WM_USER + 501)
#define PBM_LAST			(WM_USER + 502)

#define PBM_GETDISPATCH		PBM_FIRST
#define PBM_SETDISPATCH		(PBM_FIRST + 1)

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif //__VCCTRLS_H__
