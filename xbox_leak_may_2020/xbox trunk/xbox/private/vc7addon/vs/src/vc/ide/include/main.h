/////////////////////////////////////////////////////////////////////////////
//      MAIN.H
//              Developer Studio main application header file.
//

#ifndef __MAIN_H__
#define __MAIN_H__

#ifndef __AFXWIN_H__
        #error include 'afxwin.h' before including this file for PCH
#endif

// The types of options dialogs...
enum OPTDLG_TYPE { options, customize };

// ANSI code page identifier (values for mbcodepage in CTheApp)
#define OEM_JAPAN                       932
#define OEM_PRC                         936
#define OEM_TAIWAN                      950
#define OEM_KOR_WANGSUNG        949
#define OEM_KOR_JOHAB           1361
#define OEM_US                          437

#include <afxtempl.h>
#include <profile.h>

#ifndef __IDS_H__
#include "vcids.h"
#endif

#ifndef __PATH_H__
#include "path.h"
#endif

#ifdef _DEBUG
#define _EXPORT_LAYOUT
#endif

#ifdef _SHIP
#pragma message("Using _SHIP switch")
#endif



#ifndef _SHIP
// Enable/Disable this if you want to build with the MTTF Dialog.
#define _MTTF
#pragma message("MTTF is turned ON.")

#pragma message("NOT Using _SHIP switch")
#endif

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

class CPartFrame;
class CPartTemplate;
class CPartDoc;
class CIPCompDoc;
class CTypedRecentFileList;

enum FORMAT_TYPE {TIME_ALL, TIME_NO_SECONDS, DATE_ALL};

////////////////////////////////////////////////////////////////////////////
// IME utility functions

void imeMoveConvertWin (HWND, int, int);
BOOL imeEnableIME (HWND, BOOL);
BOOL imeIsOpen();
BOOL imeIsEnabled();
BOOL imeIsActive();

///////////////////////////////////////////////////////////////////////////
// DBCS utility functions

BOOL IsJapaneseSystem();
BOOL IsSBKatakana( unsigned char c );
BOOL IsDBCSTrailByte( LPCSTR string, LPCSTR pch );

/////////////////////////////////////////////////////////////////////////////
// Interface to CTL3D32.DLL

#define WM_DLGSUBCLASS (WM_USER+3568)
/* WM_DLGSUBCLASS *(int FAR *)lParam return codes */
#define CTL3D_NOSUBCLASS        0
#define CTL3D_SUBCLASS          1

/////////////////////////////////////////////////////////////////////////////
// Random MFC helpers

inline void SetDlgCtrlID(HWND hWnd, int nID)
{
#ifdef _WIN32
        ::SetWindowLong(hWnd, GWL_ID, nID);
#else
        ::SetWindowWord(hWnd, GWW_ID, nID);
#endif
}

extern HINSTANCE GetResourceHandle();

/////////////////////////////////////////////////////////////////////////////
// Platform alignment

#if defined ( _MIPS_ ) || defined ( _X86_ ) || defined (_ALPHA_) || defined (_PPC_) || defined (_IA64_)
#define PACKSIZE        8
#define ALIGNMASK       (PACKSIZE - 1)
#else
    /* Deal with others as we get them */
#error "Must define PACKSIZE for this platform"
#endif

#define ALIGN(x)  ( ( (x) + ALIGNMASK ) & ( ~ALIGNMASK ) )

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#define AfxMessageBox DsMessageBox
extern int DsMessageBox(LPCTSTR lpszText, UINT nType = MB_OK, UINT nIDHelp = 0);
extern int DsMessageBox(UINT nIDPrompt, UINT nType = MB_OK);
extern int DsMessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp);


// We used to pick these warnings up via various places from the SC++L headers. MFC (or something) no longer includes the
// SC++L headers, so we need these ourselves. In the next version, we should go back and remove these and fix the issues they relate to.
// martynl 12/06/96
// Copied from yvals.h
#pragma warning(4: 4018 4114 4146 4244 4245)
#pragma warning(4: 4663 4664 4665)
#pragma warning(disable: 4237 4284 4290 4514 4242)

#endif // __MAIN_H__
