/*****************************************************************************\
*
* Module: MsgHook.cpp
*
*   Contains the message hooking code for Spy++.
*
* History:
*
\*****************************************************************************/

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static HANDLE ghHookThread = NULL;		// Hook thread handle
static HANDLE ghWriterMutex;			// Writer mutex
static HANDLE ghAccessMutex;			// Access to data mutex
static HANDLE ghReadEvent;				// Data was just read event
static HANDLE ghWrittenEvent;			// Data was just written event
static HANDLE ghOtherProcessDataEvent;	// Process data was just written event
static BYTE gabPacket[CBMSGPACKETMAX];	// Private buffer area for the packet

static CHAR gszProcessPacketClass[]		// Class for ProcessPacket window
	= "SpyxxProcessPacket";
static HWND ghwndProcessPacket;			// Hwnd that processes hook packets

//
// Array of initializing strings for the class atom array.
// This table must be updated if the CLS_* defines are
// changed!
//
static LPSTR gapszClasses[MAXCLASSATOMS] =
{
	"Button",				// CLS_BUTTON
	"ComboBox",				// CLS_COMBOBOX
	"Edit",					// CLS_EDIT
	"ListBox",				// CLS_LISTBOX
	"MDIClient",			// CLS_MDICLIENT
	"ScrollBar",			// CLS_SCROLLBAR
	"Static",				// CLS_STATIC
#ifndef DISABLE_WIN95_MESSAGES
	"#32770",				// CLS_DIALOG
	"SysAnimate32",			// CLS_ANIMATE		
	"SysHeader32",			// CLS_HEADER		
	"ToolbarWindow32",		// CLS_TOOLBAR		
	"tooltips_class32",		// CLS_TOOLTIP		
	"msctls_statusbar32",	// CLS_STATUSBAR	
	"msctls_trackbar32",	// CLS_TRACKBAR	
	"msctls_updown32",		// CLS_UPDOWN		
	"msctls_progress32",	// CLS_PROGRESS	
	"msctls_hotkey32",		// CLS_HOTKEY		
	"SysListView32",		// CLS_LISTVIEW	
	"SysTreeView32",		// CLS_TREEVIEW	
	"SysTabControl32"		// CLS_TABCTRL		
#else	// DISABLE_WIN95_MESSAGES
	"#32770"				// CLS_DIALOG
#endif	// DISABLE_WIN95_MESSAGES

};


DWORD WINAPI HookMain(LPVOID lpv);
LRESULT CALLBACK ProcessPacketWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



//-----------------------------------------------------------------------------
// LogMsg
//
// Logs a message received from the hook.  It will pass it to each
// open and running message stream view.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

void LogMsg(PMSGSTREAMDATA pmsd)
{
	ASSERT(theApp.m_pMsgDocTemplate);

	//
	// Pass the message to all message stream docs
	//
	POSITION pos = theApp.m_pMsgDocTemplate->GetFirstDocPosition();
	while (pos)
	{
		CMsgDoc* pDoc = (CMsgDoc *)theApp.m_pMsgDocTemplate->GetNextDoc(pos);
		if (pDoc->IsLogging())
		{
			pDoc->LogMsg(pmsd);
		}
	}
}



/*****************************************************************************\
* CreateHookThread
*
*
*
* Arguments:
*
*
*
* Returns:
*
*
\*****************************************************************************/

BOOL CreateHookThread(void)
{
	DWORD id;
	WNDCLASS wc;

	wc.style = 0;
	wc.lpfnWndProc = ProcessPacketWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hIcon = 0;
	wc.hCursor = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = gszProcessPacketClass;
	if (!RegisterClass(&wc))
		return FALSE;

	//
	// Create the window that will receive and process messages
	// from the hook thread.  This window MUST be created by the
	// main thread, not the hook processing thread.  This avoids
	// some nasty multithreaded problems.
	//
	ghwndProcessPacket = CreateWindow(gszProcessPacketClass, NULL, WS_CHILD, 0, 0, 0, 0,
		theApp.m_pMainWnd->m_hWnd, NULL, AfxGetInstanceHandle(), NULL);

	if (!ghwndProcessPacket)
	{
		return FALSE;
	}

	if (!(ghHookThread = CreateThread(NULL, 0, HookMain, 0L, STANDARD_RIGHTS_REQUIRED, &id)))
	{
		return FALSE;
	}

	CloseHandle(ghHookThread);

	return TRUE;
}



/*****************************************************************************\
* HookMain
*
*
*
* Arguments:
*
*
*
* Returns:
*
*
\*****************************************************************************/

DWORD WINAPI HookMain(LPVOID)
{
	DWORD cbSize;
	INT i;

	//
	// Initialize the shared memory globals for the message hook.
	//
	goffRead = 0;
	goffWrite = 0;
	gcMsgPackets = 0;
	ghWriterMutex = CreateMutex(NULL, FALSE, WRITERMUTEXNAME);
	ghAccessMutex = CreateMutex(NULL, FALSE, ACCESSMUTEXNAME);
	ghReadEvent = CreateEvent(NULL, TRUE, FALSE, READEVENTNAME);
	ghWrittenEvent = CreateEvent(NULL, TRUE, FALSE, WRITTENEVENTNAME);
	ghOtherProcessDataEvent = CreateEvent(NULL, TRUE, FALSE, OTHERPROCESSDATAEVENTNAME);

	//
	// Get the main thread ID and process ID for Spy++.
	// This code assumes that the main thread created
	// the main window.
	//
	gtidSpyxx = GetWindowThreadProcessId(theApp.m_pMainWnd->m_hWnd, &gpidSpyxx);

#ifndef DISABLE_WIN95_MESSAGES
	if (theApp.IsChicago())
		gapszClasses[CLS_UPDOWN] = "msctls_updown\0";

	//
	// Initialize the class atom array.
	//
	for (i = 0; i < MAXCLASSATOMS; i++)
	{
		if (i < CLS_ANIMATE)
		{
			gaaClasses[i] = (ATOM)RegisterClipboardFormat(gapszClasses[i]);
		}
		else
		{
			HWND hwndTmp = ::CreateWindowEx(0, gapszClasses[i], gapszClasses[i],
							0, 0, 0, 0, 0, NULL, NULL, theApp.m_hInstance, NULL);
			ASSERT(hwndTmp);
			gaaClasses[i] = (ATOM)::GetClassWord(hwndTmp, GCW_ATOM);
			::DestroyWindow(hwndTmp);
		}
		ASSERT(gaaClasses[i]);
	}
#else
	for (i = 0; i < MAXCLASSATOMS; i++)
	{
		gaaClasses[i] = (ATOM)RegisterClipboardFormat(gapszClasses[i]);
		ASSERT(gaaClasses[i]);
	}
#endif

	CMsgDoc::EnableSubclass(FALSE);
	CMsgDoc::EnableHook(FALSE);
	SetMsgHook(TRUE);

	for (;;)
	{
		WaitForSingleObject(ghWrittenEvent, INFINITE);
		WaitForSingleObject(ghAccessMutex, INFINITE);

		cbSize = *((PDWORD)(gabMsgBuf + goffRead));
		if (!cbSize)
		{
			goffRead = 0;
			cbSize = *((PDWORD)(gabMsgBuf + goffRead));
		}

		memcpy(gabPacket, gabMsgBuf + goffRead, cbSize);

		goffRead += cbSize;
		gcMsgPackets--;
		if (gcMsgPackets == 0)
		{
			ResetEvent(ghWrittenEvent);
		}

		ReleaseMutex(ghAccessMutex);
		SetEvent(ghReadEvent);

		SendMessage(ghwndProcessPacket, WM_USER_PROCESSPACKET, 0, (LPARAM)gabPacket);
	}

	ASSERT(FALSE);  // not reached
	return 0;
}



//-----------------------------------------------------------------------------
// ProcessPacketWndProc
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

LRESULT CALLBACK ProcessPacketWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_USER_PROCESSPACKET:
			LogMsg((PMSGSTREAMDATA)lParam);
			return 0;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}



/*****************************************************************************\
* SetMsgHook
*
* Sets the windows hooks used to trap the messages.  After this
* is called with a TRUE for fSet, the messages will start flowing
* through the hook DLL.
*
* Arguments:
*   BOOL fSet - TRUE to hook, FALSE to unhook.
*
* Returns:
*   TRUE if successful.
*
\*****************************************************************************/
#ifndef WH_CALLWNDPROCRET
//
// Hook number for WH_CALLWNDPROCRET
// defined in winuser.h in Chicago
//
#undef  WH_MAX
#define WH_CALLWNDPROCRET 12
#define WH_MAX 12
#endif	// WH_CALLWNDPROCRET

BOOL SetMsgHook(BOOL fSet)
{
	static HHOOK hhkGetMessage = 0;
	static HHOOK hhkCallWndProc = 0;
	static HHOOK hhkCallWndRetProc = 0;
	static HINSTANCE hmodHook = 0;

	if (fSet)
	{
		if (!hmodHook)
		{
			if (!(hmodHook = GetModuleHandle("spyhk30")))
			{
				return FALSE;
			}
		}

		if (!hhkGetMessage)
		{
			if (!(hhkGetMessage = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)SpyxxGetMsgProc, hmodHook, 0)))
			{
				return FALSE;
			}
			ghhkMsgHook = hhkGetMessage;
		}

		if (!hhkCallWndProc)
		{
			if (!(hhkCallWndProc = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)SpyxxCallWndProc, hmodHook, 0)))
			{
				UnhookWindowsHookEx(hhkGetMessage);
				return FALSE;
			}
			ghhkCallHook = hhkCallWndProc;
		}

		if (theApp.IsChicago() && !hhkCallWndRetProc)
		{
			if (!(hhkCallWndRetProc = SetWindowsHookEx(WH_CALLWNDPROCRET, (HOOKPROC)SpyxxCallWndRetProc, hmodHook, 0)))
			{
				UnhookWindowsHookEx(hhkGetMessage);
				UnhookWindowsHookEx(hhkCallWndProc);
				return FALSE;
			}
			ghhkRetHook = hhkCallWndRetProc;
		}
	}
	else
	{
		//
		// Be sure that there are no messages that have caused a
		// window to be subclassed but have not finished going
		// through the hooks subclass procedure.  This must be
		// checked for, or there is a possibility of a thread
		// being ready to be dispatched to the subclass proc
		// or even still having the subclass proc on its stack
		// as a return address when the unhook is done, which
		// will remove the spy hook dll from the processes
		// address space!
		//
		while (gcSubclass)
		{
			//
			// Allow a little time for the hook to clear out.
			//
			Sleep(50);
		}

		if (hhkGetMessage)
		{
			UnhookWindowsHookEx(hhkGetMessage);
			hhkGetMessage = NULL;
		}

		if (hhkCallWndProc)
		{
			UnhookWindowsHookEx(hhkCallWndProc);
			hhkCallWndProc = NULL;
		}

		if (theApp.IsChicago() && hhkCallWndProc)
		{
			UnhookWindowsHookEx(hhkCallWndRetProc);
			hhkCallWndRetProc = NULL;
		}
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
// GetOtherProcessData
//
// Gets data, possibly from another process.  The window may belong
// to another process, so the method that is used is to post a special
// message to the window which is detected by the get message hook,
// which then does the actual api calls in the context of the
// other process, and trips an event that the caller of this function
// will wait on.
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

BOOL GetOtherProcessData(HWND hwnd, POTHERPROCESSDATA popd)
{
	INT i;
	DWORD pidHwnd;

	//
	// If the window cannot be spied upon, don't
	// bother posting the message.  Some windows do NOT get their
	// messages sent to hooks, and we will timeout for sure.
	//
	if (!IsSpyableWindow(hwnd))
	{
		return FALSE;
	}

	//
	// Special case requests for the Spy++ app windows.  This is
	// necessary because the Spy++ app would wait for the posted
	// message (to itself) to be processed, but it would never
	// get back to the GetMessage call to process it and would
	// thus timeout on the wait.
	//
	GetWindowThreadProcessId(hwnd, &pidHwnd);
	if (pidHwnd == gpidSpyxx)
	{
		CHAR szClass[64];

		GetClassName(hwnd, szClass, sizeof(szClass) / sizeof(CHAR));
		popd->pfnWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
		popd->fValidWC = GetClassInfo(NULL, szClass, &popd->wc);
		*popd->szMenuName = 0;
		if (popd->fValidWC && popd->wc.lpszMenuName)
		{
			//
			// Is the menu name a string or an ordinal?
			//
			if (HIWORD(popd->wc.lpszMenuName))
			{
				//
				// Copy the string
				//
				_tcscpy(popd->szMenuName, popd->wc.lpszMenuName);
			}
			else
			{
				//
				// Convert the ordinal into a string
				//
				*popd->szMenuName = '#';
				_itoa((INT)LOWORD((DWORD)popd->wc.lpszMenuName), &(popd->szMenuName[1]), 10);
			}
		}

		return TRUE;
	}

	ResetEvent(ghOtherProcessDataEvent);

	//
	// Post the message.  Try up to 25 times (in case the other apps
	// queue is full) with a 1/5 second between tries.
	//
	gopd.hwnd = hwnd;
	for (i = 0; i < 25; i++)
	{
		if (PostMessage(hwnd, gmsgOtherProcessData, 0, 0))
		{
			break;
		}

		Sleep(200);
	}

	if (i == 25)
	{
		return FALSE;
	}

	//
	// Wait for the hook to get the message and respond to it by
	// filling in the other process data struct.  Return TRUE if the
	// wait did not timeout.
	//
	if (WaitForSingleObject(ghOtherProcessDataEvent, TIMEOUT_OTHERPROCESSDATA) == WAIT_OBJECT_0)
	{
		WaitForSingleObject(ghAccessMutex, INFINITE);
		memcpy(popd, &gopd, sizeof(OTHERPROCESSDATA));
		ReleaseMutex(ghAccessMutex);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



//-----------------------------------------------------------------------------
// IsSpyableWindow
//
// Returns TRUE if this window can be spied upon by Spy++.
//
// The system does not send messages for certain windows through
// the hook chain.  This includes the desktop window, popup menu
// and console windows.
//
// Arguments:
//  HWND hwnd - Window to check
//
// Returns:
//  TRUE if the window can be spied upon, FALSE if not.
//
//-----------------------------------------------------------------------------

BOOL IsSpyableWindow(HWND hwnd)
{
	CHAR szClass[256];
	DWORD dwProcess;

	if (!IsWindow(hwnd))
	{
		return FALSE;
	}

	//
	// Cannot spy on a window if we are unable to get its thread
	// or process id. This includes Icon Title windows for consoles.
	//
	if (GetWindowThreadProcessId(hwnd, &dwProcess) == 0)
	{
		return FALSE;
	}

	//
	// Cannot spy on the popup menu window, desktop window, or consoles.
	//
	GetClassName(hwnd, szClass, sizeof(szClass) / sizeof(CHAR));
	if (_tcscmp(szClass, "#32768") == 0	||				// PopupMenu
		_tcscmp(szClass, "#32769") == 0 ||				// Desktop
		_tcscmp(szClass, "tty") == 0 ||					// Consoles (Chicago)
		_tcscmp(szClass, "ConsoleWindowClass") == 0)	// Consoles (NT)
	{
		return FALSE;
	}

	return TRUE;
}




