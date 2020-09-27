/////////////////////////////////////////////////////////////////////////////
// guiv1.h
//
// email	date		change
// briancr	12/05/94	created
//
// copyright 1994 Microsoft

// Backward compatibility

#ifndef __GUIV1_H__
#define __GUIV1_H__

#include "settings.h"
#include "log.h"

#include "guixprt.h"

// REVIEW(briancr): these objects are declared as globals for backward compatibility

// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
extern GUI_DATA CLog* gpLog;
#define LOG gpLog
extern GUI_DATA CSettings* gpsettingsCmdLine;
#define CMDLINE gpsettingsCmdLine

//REVIEW(michma): this is a work-around to allow suite dlls to call
// GetUserTargetPlatforms (in shl.dll) while the test lists are being
// processed at load time.  GetUserTargetPlatforms() used to use a
// COConnection object for its work, but now COConnection is initialized
// by an IDE object which doesn't exist while suite dlls are being loaded.

extern GUI_DATA CString gplatformStr;

#define CAFE_LANG_ENGLISH	0
#define CAFE_LANG_GERMAN	1
#define CAFE_LANG_JAPANESE	2

#define MAX_ATOM_LENGTH 256

#define INITIALIZE_STRING "Initializing..."

GUI_API int GetLang(void);
GUI_API void SetLang(int lang);

GUI_API HINSTANCE GetLangDllHandle(void);
GUI_API void SetLangDllHandle(HINSTANCE hLangDll);

GUI_API int GetSysLang(void);

// hotkey stuff
enum EHotKey { HOTKEY_RUN, HOTKEY_SINGLE, HOTKEY_STEPOVER, HOTKEY_BREAK, HOTKEY_STEPBREAK};

extern GUI_DATA HANDLE g_hSingleStep;
extern GUI_DATA HANDLE g_hUserAbort;
extern GUI_DATA HWND g_hwndHotKey;
extern GUI_DATA EHotKey g_hkToDo;
extern GUI_DATA int g_stopStep;

GUI_API void WaitStepInstructions( LPCSTR szFmt, ... );

#endif // __CAFEV1_H__
