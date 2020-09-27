#ifndef _INC_WINMISC
#define _INC_WINMISC

#include <windows.h>
#include "..\defs.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern int ParseCommandLine(LPCSTR, LPSTR **);
extern void FreeCommandLine(int, LPSTR **);
extern void InitDriveList(HWND);
extern void BytesToString(DWORDLONG, LPSTR);
extern void SetWaitCursor(HWND);
extern void ReleaseWaitCursor(void);
extern BOOL BrowseForImageFile(HWND, LPSTR, BOOL);
extern void CenterWindow(HWND, HWND);
extern HWND CreateProgressDialog(HWND);
extern HWND CloseProgressDialog(HWND);
extern void SetProgressLabel(HWND, LPSTR, ...);
extern void SetProgressPos(HWND, DWORD, DWORD);
extern BOOL GetFileName(HWND, LPSTR, LPSTR);
extern BOOL GetFragmentName(HWND, UINT, LPSTR);
extern void ProcessMessages(HWND);
extern void GetCheckFlag(HWND, UINT, WORD, LPWORD);
extern void SetCheckFlag(HWND, UINT, WORD, WORD);
extern void EnableDragDrop(HWND, BOOL);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _INC_WINMISC