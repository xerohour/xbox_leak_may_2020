#define STRICT

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <process.h>
#include "resource.h"
#include "mapistuff.h"
#include <trisupp.h>

#define HEADER "This mail has been sent by WinTriage.  Please reply to the failure, or reassign as appropriate.\n\n"
#define ERROR_STRING_LENGTH 255
#define DEFAULT_REMOTE_LINES (DWORD)1000

HINSTANCE ghInstance;
HWND ghwndMain;

INT_PTR CALLBACK TriageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID ErrorHandler(TCHAR *szCall, DWORD dwErrorCode);
DWORD WINAPI DoTriage(LPVOID lpParameter);
BOOL SetBoxesFromTextBuffer(HWND hWnd, PTCHAR TextBuffer);
BOOL DoRemote(HWND hWnd);


