#ifndef _XBOX // no window logging supported
//--------------------------------------------------------------------------;
//
//  File: Text.h
//
//  Copyright (c) 1994 Microsoft Corporation.  All rights reserved
//
//  Abstract:
//
//
//  History:
//      08/28/94    Fwong       Created.
//      10/20/95    Fwong       Adding color support.
//
//--------------------------------------------------------------------------;

//BEGIN vanceo
//==================================================================================
// Defines
//==================================================================================
#ifdef LOCAL_TNCOMMON

	// define LOCAL_TNCOMMON when including this code directly into your project
	#ifdef DLLEXPORT
		#undef DLLEXPORT
	#endif // DLLEXPORT defined
	#define DLLEXPORT

#else // ! LOCAL_TNCOMMON

	#ifdef TNCOMMON_EXPORTS

		// define TNCOMMON_EXPORTS only when building the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllexport)

	#else // ! TNCOMMON_EXPORTS

		// default behavior is to import the functions from the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllimport)

	#endif // ! TNCOMMON_EXPORTS
#endif // ! LOCAL_TNCOMMON

#ifndef DEBUG
	#ifdef _DEBUG
		#define DEBUG
	#endif // _DEBUG
#endif // DEBUG not defined
//END vanceo


//==========================================================================;
//
//                             Messages...
//
//==========================================================================;

#define TM_DELETETEXT       WM_USER + 1
#define TM_SETSIZELIMIT     WM_USER + 2
#define TM_GETSIZELIMIT     WM_USER + 3
#define TM_SEARCH           WM_USER + 4
#define TM_SETNUMCOLORS     WM_USER + 5
#define TM_GETNUMCOLORS     WM_USER + 6
#define TM_SETCOLOR         WM_USER + 7
#define TM_GETCOLOR         WM_USER + 8


//==========================================================================;
//
//                               Flags...
//
//==========================================================================;

#define SETSIZEFLAG_TRUNCATE    0x0001
#define SEARCHFLAG_MATCHCASE    0x0001
#define TEXTCOLOR_DEFAULT       0x00

//==========================================================================;
//
//                              Macros...
//
//==========================================================================;

#define Text_Copy(hWnd)             SendMessage(hWnd,WM_COPY,0,0L)
#define Text_Delete(hWnd)           SendMessage(hWnd,TM_DELETETEXT,0,0L)
#define Text_GetBufferLimit(hWnd)   SendMessage(hWnd,TM_GETSIZELIMIT,0,0L)

#define Text_SetBufferLimit(hWnd,cbSize,uFlags)  \
            SendMessage(hWnd,TM_SETSIZELIMIT,(WPARAM)(uFlags),(LPARAM)(cbSize))

#define Text_SearchString(hWnd,pszString,uFlags)  \
            (BOOL)(SendMessage(hWnd,TM_SEARCH,(WPARAM)(uFlags), \
            (LPARAM)(LPSTR)(pszString)))

#define Text_SetNumColors(hWnd,uCount)  \
            (BOOL)(SendMessage(hWnd,TM_SETNUMCOLORS,(WPARAM)(uCount),0L))

#define Text_GetNumColors(hWnd)     SendMessage(hWnd,TM_GETNUMCOLORS,0,0L)

#define Text_SetColor(hWnd,id,cr)  \
            SendMessage(hWnd,TM_SETCOLOR,(WPARAM)id,(LPARAM)cr)

#define Text_GetColor(hWnd,id)  \
            (COLORREF)(SendMessage(hWnd,TM_GETCOLOR,(WPARAM)id,0L))

#define TextOutputString(a,b)       TextOutputStringColor(a,0,b)

//==========================================================================;
//
//                            Prototypes...
//
//==========================================================================;

DLLEXPORT BOOL TextInit
(
    HINSTANCE   hInstance
);

DLLEXPORT void TextEnd
(
    HINSTANCE   hInstance
);

DLLEXPORT HWND TextCreateWindow
(
    DWORD       dwStyle,
    int         x,
    int         y,
    int         nWidth,
    int         nHeight,
    HWND        hWndParent,
    HMENU       hMenu,
    HINSTANCE   hInstance
);

DLLEXPORT void TextOutputStringColor
(
    HWND    hWnd,
    UINT    uColor,
    LPSTR   pText
);

DLLEXPORT int _cdecl TextPrintf
(
    HWND    hWnd,
    LPSTR   pszFormat,
    ...
);

DLLEXPORT int _cdecl TextPrintfColor
(
    HWND    hWnd,
    UINT    uColor,
    LPSTR   pszFormat,
    ...
);

DLLEXPORT LRESULT TextFocusProc
(
    HWND    hWnd,
    UINT    uMessage,
    WPARAM  wParam,
    LPARAM  lParam
);
#endif // ! XBOX