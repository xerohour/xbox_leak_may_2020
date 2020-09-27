//  SpyxxHk.c

#include <windows.h>

#ifndef WM_NOTIFY
// WM_NOTIFY is new in later versions of Win32
#define WM_NOTIFY 0x004e
typedef struct tagNMHDR
{
	HWND hwndFrom;
	UINT idFrom;
	UINT code;
} NMHDR;
#endif //!WM_NOTIFY

#if (WINVER < 0x400)
	#define _REDEF_WINVER
	#undef WINVER
	#define WINVER 0x0400
#endif
#include <commctrl.h>
#ifdef _REDEF_WINVER
	#undef _REDEF_WINVER
	#undef WINVER
	#define WINVER 0x030A
#endif

#include <stdlib.h>

#include "spyxxhk.h"

//
// Define this to NOT do an actual add of packets to the shared
// memory queue.  This is for testing and guarantees that the hook
// will never need to block waiting for the reader to clear room
// in the queue.
//
//#define DONTUSEQUEUE

//
// Maximum number of slots in the subclass queue.  This determines the
// maximum number of windows in a process that have been subclassed
// but have not yet had all messages processed for them.
//
#define MAXQUEUE		64

//
// Size of the message packet buffer area.
//
#define CBMSGBUFFER			 8000

typedef struct
{
	HWND hwnd;
	WNDPROC pfnPrev;
	INT cSub;
} SUBCLASSQUEUE, *PSUBCLASSQUEUE;

#ifndef WH_CALLWNDPROCRET
//
// Message structure used by WH_CALLWNDPROCRET
// defined in winuser.h in Chicago
//
typedef struct tagCWPRETSTRUCT {
	LRESULT	lResult;
    LPARAM  lParam;
    WPARAM  wParam;
    UINT    message;
    HWND    hwnd;
} CWPRETSTRUCT, *PCWPRETSTRUCT, NEAR *NPCWPRETSTRUCT, FAR *LPCWPRETSTRUCT;
#endif	// WH_CALLWNDPROCRET

///////////////////////////////////////////////////////////////////////////////
// Shared memory data segment.
//
// All these variables MUST be initialized for the compiler to place them
// in the .shdata segment instead of the .BSS segment!!!
//
///////////////////////////////////////////////////////////////////////////////

#pragma data_seg(".shdata")

SPYXXHK_SHDATA BOOL gfOnChicago = FALSE;				// TRUE if the application is running on Chicago
SPYXXHK_SHDATA BOOL gfOnDaytona = FALSE;				// TRUE if the application is running on Daytona
SPYXXHK_SHDATA HHOOK ghhkMsgHook = NULL;				// hook from SetWindowsHookEx(WH_GETMESSAGE)
SPYXXHK_SHDATA HHOOK ghhkCallHook = NULL;				// hook from SetWindowsHookEx(WH_CALLWNDPROC)
SPYXXHK_SHDATA HHOOK ghhkRetHook = NULL;				// hook from SetWindowsHookEx(WH_CALLWNDPROCRET) (Chicago only)
SPYXXHK_SHDATA BOOL gfHookEnabled = 0;					// TRUE if the hook is enabled
SPYXXHK_SHDATA BOOL gfEnableSubclass = FALSE;			// TRUE if subclassing is enabled
SPYXXHK_SHDATA BOOL gfDecodeStructs = FALSE;			// TRUE if structs/strings are cracked
SPYXXHK_SHDATA UINT gcSubclass = 0;						// Count of subclassed messages
SPYXXHK_SHDATA DWORD gpidSpyxx = 0;						// Process ID of the Spy++ app
SPYXXHK_SHDATA DWORD gtidSpyxx = 0;						// Thread ID of Spy++'s main thread
SPYXXHK_SHDATA DWORD goffRead = 0;						// Read offset
SPYXXHK_SHDATA DWORD goffWrite = 0;						// Write offset
SPYXXHK_SHDATA UINT gcMsgPackets = 0;					// Count of message packets
SPYXXHK_SHDATA ATOM gaaClasses[MAXCLASSATOMS] = { 0 };	// Class atom array

//
// Message buffer area.  Be sure that there is extra room at the end
// to write a DWORD of zero so that the reader will know when to
// wrap around.  It simplifies the wrapping logic to guarantee that
// at least one more DWORD can always be written after the "end" of
// the buffer.
//
SPYXXHK_SHDATA BYTE gabMsgBuf[CBMSGBUFFER + sizeof(DWORD)] = { 0 };

//
// Variables used for obtaining data from another process
//
SPYXXHK_SHDATA UINT gmsgOtherProcessData = 0;// Registered message
SPYXXHK_SHDATA OTHERPROCESSDATA gopd = { 0 };// Structure for other process data

//
// Bring in the message encoding table.  This is in a separate file
// for readability.
//
#include "met.h"

#pragma data_seg()

///////////////////////////////////////////////////////////////////////////////
// Private (instance) memory segment
///////////////////////////////////////////////////////////////////////////////

PRIVATE HANDLE ghWriterMutex;				// Writer mutex
PRIVATE HANDLE ghAccessMutex;				// Access to data mutex
PRIVATE HANDLE ghReadEvent;					// Data was just read event
PRIVATE HANDLE ghWrittenEvent;				// Data was just written event
PRIVATE HANDLE ghOtherProcessDataEvent;		// Process data was just written event
PRIVATE UINT gcSubclassThisProcess = 0;		// Subclassed msgs for this process
PRIVATE INT gnLevel = 0;					// Message nesting level
PRIVATE CRITICAL_SECTION gcsQueue;			// Critical section for queue access
PRIVATE INT gcQueue = 0;					// High water mark of the queue
PRIVATE SUBCLASSQUEUE gascq[MAXQUEUE];		// Hwnd database queue

///////////////////////////////////////////////////////////////////////////////
// Function declarations.
///////////////////////////////////////////////////////////////////////////////

__declspec(dllexport) BOOL WINAPI DLLEntryPoint(HINSTANCE hinstDLL,	DWORD fdwReason, LPVOID lpvReserved);
LRESULT WINAPI SpyxxSubclassWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

PRIVATE UINT SubclassWnd(HWND hwnd, UINT msg);
PRIVATE BOOL AddSlot(HWND hwnd, WNDPROC pfnPrev);
PRIVATE INT UnSubclassWnd(HWND hwnd, WNDPROC *ppfnPrev);
PRIVATE VOID PostToRadar(UINT fPostType, INT nLevel, LRESULT lResult, HWND hwnd, UINT msg,
						WPARAM wParam, LPARAM lParam, DWORD time, LONG ptX, LONG ptY);
PRIVATE PBYTE GetMsgPacket(DWORD cbSize);
PRIVATE DWORD StringSizeof(LPCSTR psz, BOOL* pfLongString);
PRIVATE void StringCopy(CHAR* pszDest, LPCSTR pszSrc, DWORD cb, BOOL fLongString);
PRIVATE VOID CopyFont(PPACK_FONT ppf, HFONT hfont);

#ifdef _DEBUG
VOID dprintf(LPTSTR fmt, ...);
#endif

//-----------------------------------------------------------------------------
// DLLEntryPoint
//-----------------------------------------------------------------------------

__declspec(dllexport) BOOL WINAPI DLLEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		ghWriterMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, WRITERMUTEXNAME);
		ghAccessMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, ACCESSMUTEXNAME);
		ghReadEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, READEVENTNAME);
		ghWrittenEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, WRITTENEVENTNAME);
		ghOtherProcessDataEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, OTHERPROCESSDATAEVENTNAME);
		InitializeCriticalSection(&gcsQueue);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		//
		// Remove the count of pending subclassed messages for this
		// process from the total count.  The local count will normally
		// be zero at this point, but if this process is abnormally
		// terminating, we MUST remove any local count from the total
		// count here or the total count will never reach zero again.
		//
		WaitForSingleObject(ghAccessMutex, INFINITE);
		gcSubclass -= gcSubclassThisProcess;
		ReleaseMutex(ghAccessMutex);

		CloseHandle(ghWriterMutex);
		CloseHandle(ghAccessMutex);
		CloseHandle(ghReadEvent);
		CloseHandle(ghWrittenEvent);
		CloseHandle(ghOtherProcessDataEvent);
		DeleteCriticalSection(&gcsQueue);
	}

	return TRUE;
}

/*****************************************************************************\
* SpyxxGetMsgProc
*
* The Get Message hook function.
*
\*****************************************************************************/

SPYXXHK_API LRESULT CALLBACK SpyxxGetMsgProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG pmsg;
	DWORD pidMsgHwnd;

	pmsg = (PMSG)lParam;

	if (nCode == HC_ACTION && wParam & PM_REMOVE && pmsg && pmsg->hwnd)
	{
		if (pmsg->message == gmsgOtherProcessData)
		{
			//
			// Double-check to be sure this isn't an old message that
			// finally got processed after we timed out.
			//
			if (pmsg->hwnd == gopd.hwnd)
			{
				CHAR szClass[128];

				GetClassName(pmsg->hwnd, szClass, sizeof(szClass) / sizeof(CHAR));

				WaitForSingleObject(ghAccessMutex, INFINITE);

				//
				// Call the Unicode or ANSI GetWindowLong depending on
				// whether the window is currently Unicode or ANSI.
				// This ensures that we do not get back a thunk.
				//
				if (IsWindowUnicode(pmsg->hwnd))
				{
					gopd.pfnWndProc = (WNDPROC)GetWindowLongW(pmsg->hwnd, GWL_WNDPROC);
				}
				else
				{
					gopd.pfnWndProc = (WNDPROC)GetWindowLongA(pmsg->hwnd, GWL_WNDPROC);
				}

				gopd.fValidWC = GetClassInfo(NULL, szClass, &gopd.wc);
				*gopd.szMenuName = 0;
				if (gopd.fValidWC)
				{
					//
					// Is there a menu name?
					//
					if (gopd.wc.lpszMenuName)
					{
						//
						// Is the menu name a string or an ordinal?
						//
						if (HIWORD(gopd.wc.lpszMenuName))
						{
							//
							// Copy the string
							//
							strcpy(gopd.szMenuName, gopd.wc.lpszMenuName);
						}
						else
						{
							//
							// Convert the ordinal into a string
							//
							*gopd.szMenuName = '#';
							_itoa((INT)LOWORD(gopd.wc.lpszMenuName), &gopd.szMenuName[1], 10);
						}
					}

					//
					// If this window is unicode, we must use the
					// unicode api to get the wndproc value, or
					// we will get back a thunk instead of the real
					// proc.
					//
					if (IsWindowUnicode(pmsg->hwnd))
					{
						gopd.wc.lpfnWndProc = (WNDPROC)GetClassLongW(pmsg->hwnd, GCL_WNDPROC);
					}
				}

				ReleaseMutex(ghAccessMutex);

				SetEvent(ghOtherProcessDataEvent);
			}

			//
			// Turn the message into a benign WM_NULL message before
			// the app gets it.
			//
			pmsg->message = WM_NULL;
			pmsg->wParam = 0;
			pmsg->lParam = 0;
		}
		else if (gfHookEnabled)
		{
			//
			// Do not log the message if it is for a Spy++ window.
			//
			GetWindowThreadProcessId(pmsg->hwnd, &pidMsgHwnd);
			if (pidMsgHwnd != gpidSpyxx)
			{
				PostToRadar(POSTTYPE_POSTED, 0, 0, pmsg->hwnd, pmsg->message,
					pmsg->wParam, pmsg->lParam, pmsg->time, pmsg->pt.x, pmsg->pt.y);
			}
		}
	}

	return CallNextHookEx(ghhkMsgHook, nCode, wParam, lParam);
}

/*****************************************************************************\
* SpyxxCallWndProc
*
* The Call Window Proc (Send Message) hook function.
*
\*****************************************************************************/

SPYXXHK_API LRESULT CALLBACK SpyxxCallWndProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPSTRUCT pcwps;
	DWORD pidMsgHwnd;
	LRESULT lRes;

	lRes = CallNextHookEx(ghhkCallHook, nCode, wParam, lParam);

	pcwps = (PCWPSTRUCT)lParam;

	if (gfHookEnabled && nCode == HC_ACTION && pcwps && pcwps->hwnd)
	{
		//
		// Do not log the message if it is for a Spy++ window,
		// or if this is a Spy++ thread sending the message.
		// If messages to Spy++ windows were logged, this would
		// immediately cause infinite recursion when Spy++ tried
		// to log a message (which could cause another message
		// to be sent, etc).
		//
		// We must also strip out messages that are being sent
		// from the Spy++ first (main) thread to a window in another
		// process.  This is necessary because trying to log
		// the message could cause a deadlock case, where the
		// second thread in Spy++ is blocked trying to send
		// a message to a window created by the first thread,
		// but the first thread is blocked here waiting for
		// the second thread to clear out some packets from
		// the message buffer queue.
		//
		GetWindowThreadProcessId(pcwps->hwnd, &pidMsgHwnd);
		if (pidMsgHwnd != gpidSpyxx && GetCurrentThreadId() != gtidSpyxx)
		{
			UINT fPostType = POSTTYPE_SENT;

			if (!gfOnChicago && gfEnableSubclass)
			{
				fPostType = SubclassWnd(pcwps->hwnd, pcwps->message);
			}

			//
			// Post the sent message information.  The return results
			// will be posted later, in the SpyxxSubclassWndProc function.
			//
			PostToRadar(fPostType, gnLevel, 0, pcwps->hwnd,
				pcwps->message, pcwps->wParam, pcwps->lParam, 0, 0, 0);
		}
	}

	return lRes;
}

/*****************************************************************************\
* SpyxxCallWndRetProc
*
* The Call Window Proc Return (Send Message return) hook function.
*
\*****************************************************************************/

SPYXXHK_API LRESULT CALLBACK SpyxxCallWndRetProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPRETSTRUCT pcwprs;
	DWORD pidMsgHwnd;
	LRESULT lRes;

	lRes = CallNextHookEx(ghhkRetHook, nCode, wParam, lParam);

	pcwprs = (PCWPRETSTRUCT)lParam;

	if (gfHookEnabled && nCode == HC_ACTION && pcwprs && pcwprs->hwnd)
	{
		//
		// Do not log the message if it is for a Spy++ window,
		// or if this is a Spy++ thread sending the message.
		// If messages to Spy++ windows were logged, this would
		// immediately cause infinite recursion when Spy++ tried
		// to log a message (which could cause another message
		// to be sent, etc).
		//
		// We must also strip out messages that are being sent
		// from the Spy++ first (main) thread to a window in another
		// process.  This is necessary because trying to log
		// the message could cause a deadlock case, where the
		// second thread in Spy++ is blocked trying to send
		// a message to a window created by the first thread,
		// but the first thread is blocked here waiting for
		// the second thread to clear out some packets from
		// the message buffer queue.
		//
		GetWindowThreadProcessId(pcwprs->hwnd, &pidMsgHwnd);
		if (pidMsgHwnd != gpidSpyxx && GetCurrentThreadId() != gtidSpyxx)
		{
			UINT fPostType = POSTTYPE_RETURN;

			//
			// Post the sent message information.  The return results
			// will be posted later, in the SpyxxSubclassWndProc function.
			//
			PostToRadar(fPostType, gnLevel, pcwprs->lResult, pcwprs->hwnd,
				pcwprs->message, pcwprs->wParam, pcwprs->lParam, 0, 0, 0);
		}
	}

	return lRes;
}

//-----------------------------------------------------------------------------
// SpyxxSubclassWndProc
//-----------------------------------------------------------------------------

LRESULT WINAPI SpyxxSubclassWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	WNDPROC pfnPrev;
	INT cSub;

	cSub = UnSubclassWnd(hwnd, &pfnPrev);

	//
	// Increment the nesting level, then call the original window proc
	//
	gnLevel++;
	lResult = CallWindowProc(pfnPrev, hwnd, msg, wParam, lParam);
	gnLevel--;

	PostToRadar(POSTTYPE_RETURN, gnLevel, lResult, hwnd, msg, wParam, lParam, 0, 0, 0);

	//
	// If there are still pending messages to process, we must
	// resubclass the window now so that they are not lost.
	//
	if (cSub)
	{
		SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)SpyxxSubclassWndProc);
	}

	//
	// Decrement the subclass count.
	//
	WaitForSingleObject(ghAccessMutex, INFINITE);
	gcSubclass--;
	gcSubclassThisProcess--;
	ReleaseMutex(ghAccessMutex);

	return lResult;
}

//-----------------------------------------------------------------------------
// SubclassWnd
//-----------------------------------------------------------------------------

PRIVATE UINT SubclassWnd(HWND hwnd, UINT msg)
{
	UINT fPostType = POSTTYPE_SENT;
	WNDPROC pfnPrev;

	EnterCriticalSection(&gcsQueue);

	//
	// Subclass the window.
	//
	pfnPrev = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)SpyxxSubclassWndProc);

	if (pfnPrev)
	{
		//
		// Store away the previous proc address.
		//
		if (AddSlot(hwnd, pfnPrev))
		{
			//
			// Increment the subclass counts.
			//
			WaitForSingleObject(ghAccessMutex, INFINITE);
			gcSubclass++;
			gcSubclassThisProcess++;
			ReleaseMutex(ghAccessMutex);
		}
		else
		{
			//
			// No room in the queue for the hwnd.  Unsubclass
			// the window.  Return value for this message will
			// not be available.
			//
			SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)pfnPrev);
			fPostType = POSTTYPE_SENTNORETURN;
		}
	}
	else
	{
		//
		// The subclass failed for some reason.  This is
		// probably because this is a send to a window in
		// another process.  The return value will not be
		// available.
		//
		fPostType = POSTTYPE_SENTNORETURN;
	}

	LeaveCriticalSection(&gcsQueue);
	return fPostType;
}

//-----------------------------------------------------------------------------
// AddSlot
//
// Adds an entry to the subclass queue for the specified window.  If the
// window is already found in the queue, increments the count for that
// entry instead of making a new one.
//
// This routine assumes that it is called while owning the subclass queue
// critical section.
//-----------------------------------------------------------------------------

PRIVATE BOOL AddSlot(HWND hwnd, WNDPROC pfnPrev)
{
	INT i;
	PSUBCLASSQUEUE pscq;

	for (pscq = gascq, i = 0; i < gcQueue; i++, pscq++)
	{
		if (pscq->hwnd == hwnd)
		{
			//
			// The hwnd is already in the queue.  Is this the same
			// previous proc (or already subclassed)?
			//
			if (pfnPrev == SpyxxSubclassWndProc	|| pfnPrev == pscq->pfnPrev)
			{
				//
				// Update the reference count and return success.
				//
				pscq->cSub++;
				return TRUE;
			}
			else
			{
				//
				// This is a different proc from the original one.
				// Something funny is going on, so we should not allow
				// the subclass.
				//
#ifdef _DEBUG
				{
					CHAR szClass[128] = "(unknown)";

					GetClassName(hwnd, szClass, sizeof(szClass));
					dprintf("SPYXXHK:Hwnd %8.8X, class \"%s\", pfnNow %8.8X, pfnPrev %8.8X proc was changed!", hwnd, szClass, pfnPrev, pscq->pfnPrev);
					//DebugBreak();
				}
#endif
				return FALSE;
			}
		}
	}

	//
	// Did we run out of slots in the queue?
	//
	if (i == MAXQUEUE)
	{
		return FALSE;
	}

	//
	// Add a new slot at the top of the queue.
	//
	pscq->hwnd = hwnd;
	pscq->pfnPrev = pfnPrev;
	pscq->cSub = 1;
	gcQueue++;

	return TRUE;
}

//-----------------------------------------------------------------------------
// UnSubclassWnd
//-----------------------------------------------------------------------------

PRIVATE INT UnSubclassWnd(HWND hwnd, WNDPROC *ppfnPrev)
{
	INT i;
	INT cSubRet;
	PSUBCLASSQUEUE pscq;

	EnterCriticalSection(&gcsQueue);

	for (pscq = gascq, i = 0; i < gcQueue; i++, pscq++)
	{
		if (pscq->hwnd == hwnd)
		{
			pscq->cSub--;
			cSubRet = pscq->cSub;
			*ppfnPrev = pscq->pfnPrev;

			//
			// Unsubclass the window right away!
			//
			SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)pscq->pfnPrev);

			//
			// Did the reference count go down to zero?
			//
			if (!cSubRet)
			{
				//
				// Are we the slot at the top of the queue?
				//
				if (i == gcQueue - 1)
				{
					//
					// Compact the queue.  The gcQueue counter is a
					// "high-water mark" that needs to be dropped
					// down to the next highest slot that is in use.
					//
					while (--gcQueue)
					{
						pscq--;

						if (pscq->hwnd)
							break;
					}
				}
				else
				{
					//
					// Just mark this slot as free.  Don't bother
					// compacting the queue right now.
					//
					pscq->hwnd = (HWND)0;
				}
			}

			LeaveCriticalSection(&gcsQueue);
			return cSubRet;
		}
	}

	//
	// We should never get to here...
	//

	LeaveCriticalSection(&gcsQueue);

#ifdef _DEBUG
	{
		CHAR szClass[128] = "(unknown)";

		GetClassName(hwnd, szClass, sizeof(szClass));
		dprintf("SPYXXHK:Hwnd %8.8X, class \"%s\" was not found in queue!", hwnd, szClass);
		DebugBreak();
	}
#endif

	*ppfnPrev = 0;
	return 0;
}

//-----------------------------------------------------------------------------
// PostToRadar
//-----------------------------------------------------------------------------

PRIVATE VOID PostToRadar(UINT fPostType, INT nLevel, LRESULT lResult, HWND hwnd, UINT msg,
						WPARAM wParam, LPARAM lParam, DWORD time, LONG ptX, LONG ptY)
{
#ifndef DONTUSEQUEUE
	PMSGSTREAMDATA pmsd;
	DWORD cbSize;
	DWORD cbExtraData = 0;
	WORD fEncoding;
	BOOL fLongString;
	BOOL fOrdinal;
	BOOL fHasExtra = FALSE;

	//
	// Only try to crack the structures and strings pointed to
	// by wParam/lParam if this is a known message, the message
	// encoding table says there is something to do and the
	// Spy++ app has enabled this feature.
	//
	if (gfDecodeStructs)
	{
		if (msg < MAX_MESSAGES)
		{
			if ((fEncoding = gmet[msg].fEncoding) != MET_NOEXTRADATA)
				fHasExtra = TRUE;
		}
		else
		{
			switch (GetWindowClass(hwnd))
			{
				case CLS_DIALOG:
					if (msg >= DM_GETDEFID && msg <= DM_SETDEFID)
						if ((fEncoding = gmetDlg[msg-WM_USER].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;
#ifndef DISABLE_WIN95_MESSAGES
				case CLS_ANIMATE:
					if (msg >= ACM_OPEN && msg <= ACM_STOP)
						if ((fEncoding = gmetAni[msg-(WM_USER+100)].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_HEADER:
					if (msg >= HDM_GETITEMCOUNT && msg <= HDM_HITTEST)
						if ((fEncoding = gmetHdr[msg-HDM_FIRST].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_TOOLBAR:
					if (msg >= TB_ENABLEBUTTON && msg <= TB_GETBUTTONTEXT)
						if ((fEncoding = gmetTB[msg-(WM_USER+1)].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_TOOLTIP:
					if (msg >= TTM_ACTIVATE && msg <= TTM_GETCURRENTTOOL)
						if ((fEncoding = gmetTT[msg-(WM_USER+1)].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_STATUSBAR:
					if (msg >= SB_SETTEXT && msg <= SB_GETRECT)
						if ((fEncoding = gmetStat[msg-(WM_USER+1)].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_TRACKBAR:
					if (msg >= TBM_GETPOS && msg <= TBM_GETTHUMBLENGTH)
						if ((fEncoding = gmetTrk[msg-WM_USER].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_UPDOWN:
					if (msg >= UDM_SETRANGE && msg <= UDM_GETBASE)
						if ((fEncoding = gmetUpDn[msg-(WM_USER+101)].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_PROGRESS:
					if (msg >= PBM_SETRANGE && msg <= PBM_STEPIT)
						if ((fEncoding = gmetProg[msg-(WM_USER+1)].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_HOTKEY:
					if (msg >= HKM_SETHOTKEY && msg <= HKM_SETRULES)
						if ((fEncoding = gmetHK[msg-(WM_USER+1)].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_LISTVIEW:
					if (msg >= LVM_GETBKCOLOR && msg <= LVM_GETISEARCHSTRING)
						if ((fEncoding = gmetLV[msg-LVM_FIRST].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_TREEVIEW:
					if (msg >= TVM_INSERTITEM && msg <= TVM_GETISEARCHSTRING)
						if ((fEncoding = gmetTV[msg-TV_FIRST].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;

				case CLS_TABCTRL:
					if (msg >= TCM_GETBKCOLOR && msg <= TCM_SETCURFOCUS)
						if ((fEncoding = gmetTab[msg-TCM_FIRST].fEncoding) != MET_NOEXTRADATA)
							fHasExtra = TRUE;
					break;
#endif	// DISABLE_WIN95_MESSAGES
				default:
					fHasExtra = FALSE;
					break;
			}
		}		
	}

	if (fHasExtra)
	{
		switch (fEncoding)
		{
			case MET_STRUCTRET:
				if (msg < MAX_MESSAGES)
				{
					if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmet[msg].cbExtraData))
						cbExtraData = gmet[msg].cbExtraData;
				}
				else
				{
					switch (GetWindowClass(hwnd))
					{
						case CLS_DIALOG:
							if (msg >= DM_GETDEFID && msg <= DM_SETDEFID)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetDlg[msg-WM_USER].cbExtraData))
									cbExtraData = gmetDlg[msg-WM_USER].cbExtraData;
							break;
#ifndef DISABLE_WIN95_MESSAGES
						case CLS_ANIMATE:
							if (msg >= ACM_OPEN && msg <= ACM_STOP)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetAni[msg-(WM_USER+100)].cbExtraData))
									cbExtraData = gmetAni[msg-(WM_USER+100)].cbExtraData;
							break;

						case CLS_HEADER:
							if (msg >= HDM_GETITEMCOUNT && msg <= HDM_HITTEST)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetHdr[msg-HDM_FIRST].cbExtraData))
									cbExtraData = gmetHdr[msg-HDM_FIRST].cbExtraData;
							break;

						case CLS_TOOLBAR:
							if (msg >= TB_ENABLEBUTTON && msg <= TB_GETBUTTONTEXT)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetTB[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetTB[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_TOOLTIP:
							if (msg >= TTM_ACTIVATE && msg <= TTM_GETCURRENTTOOL)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetTT[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetTT[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_STATUSBAR:
							if (msg >= SB_SETTEXT && msg <= SB_GETRECT)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetStat[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetStat[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_TRACKBAR:
							if (msg >= TBM_GETPOS && msg <= TBM_GETTHUMBLENGTH)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetTrk[msg-WM_USER].cbExtraData))
									cbExtraData = gmetTrk[msg-WM_USER].cbExtraData;
							break;

						case CLS_UPDOWN:
							if (msg >= UDM_SETRANGE && msg <= UDM_GETBASE)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetUpDn[msg-(WM_USER+101)].cbExtraData))
									cbExtraData = gmetUpDn[msg-(WM_USER+101)].cbExtraData;
							break;

						case CLS_PROGRESS:
							if (msg >= PBM_SETRANGE && msg <= PBM_STEPIT)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetProg[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetProg[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_HOTKEY:
							if (msg >= HKM_SETHOTKEY && msg <= HKM_SETRULES)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetHK[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetHK[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_LISTVIEW:
							if (msg >= LVM_GETBKCOLOR && msg <= LVM_GETISEARCHSTRING)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetLV[msg-LVM_FIRST].cbExtraData))
									cbExtraData = gmetLV[msg-LVM_FIRST].cbExtraData;
							break;

						case CLS_TREEVIEW:
							if (msg >= TVM_INSERTITEM && msg <= TVM_GETISEARCHSTRING)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetTV[msg-TV_FIRST].cbExtraData))
									cbExtraData = gmetTV[msg-TV_FIRST].cbExtraData;
							break;

						case CLS_TABCTRL:
							if (msg >= TCM_GETBKCOLOR && msg <= TCM_SETCURFOCUS)
								if (fPostType == POSTTYPE_RETURN && !IsBadReadPtr((LPVOID)lParam, gmetTab[msg-TCM_FIRST].cbExtraData))
									cbExtraData = gmetTab[msg-TCM_FIRST].cbExtraData;
							break;
#endif	// DISABLE_WIN95_MESSAGES
						default:
							break;
					}
				}		
				break;

			case MET_STRUCTBOTH:
			case MET_CREATESTRUCTBOTH:
			case MET_MDICREATESTRUCTBOTH:
				if (msg < MAX_MESSAGES)
				{
					if (!IsBadReadPtr((LPVOID)lParam, gmet[msg].cbExtraData))
						cbExtraData = gmet[msg].cbExtraData;
				}
				else
				{
					switch (GetWindowClass(hwnd))
					{
						case CLS_DIALOG:
							if (msg >= DM_GETDEFID && msg <= DM_SETDEFID)
								if (!IsBadReadPtr((LPVOID)lParam, gmetDlg[msg-WM_USER].cbExtraData))
									cbExtraData = gmetDlg[msg-WM_USER].cbExtraData;
							break;
#ifndef DISABLE_WIN95_MESSAGES
						case CLS_ANIMATE:
							if (msg >= ACM_OPEN && msg <= ACM_STOP)
								if (!IsBadReadPtr((LPVOID)lParam, gmetAni[msg-(WM_USER+100)].cbExtraData))
									cbExtraData = gmetAni[msg-(WM_USER+100)].cbExtraData;
							break;

						case CLS_HEADER:
							if (msg >= HDM_GETITEMCOUNT && msg <= HDM_HITTEST)
								if (!IsBadReadPtr((LPVOID)lParam, gmetHdr[msg-HDM_FIRST].cbExtraData))
									cbExtraData = gmetHdr[msg-HDM_FIRST].cbExtraData;
							break;

						case CLS_TOOLBAR:
							if (msg >= TB_ENABLEBUTTON && msg <= TB_GETBUTTONTEXT)
								if (!IsBadReadPtr((LPVOID)lParam, gmetTB[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetTB[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_TOOLTIP:
							if (msg >= TTM_ACTIVATE && msg <= TTM_GETCURRENTTOOL)
								if (!IsBadReadPtr((LPVOID)lParam, gmetTT[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetTT[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_STATUSBAR:
							if (msg >= SB_SETTEXT && msg <= SB_GETRECT)
								if (!IsBadReadPtr((LPVOID)lParam, gmetStat[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetStat[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_TRACKBAR:
							if (msg >= TBM_GETPOS && msg <= TBM_GETTHUMBLENGTH)
								if (!IsBadReadPtr((LPVOID)lParam, gmetTrk[msg-WM_USER].cbExtraData))
									cbExtraData = gmetTrk[msg-WM_USER].cbExtraData;
							break;

						case CLS_UPDOWN:
							if (msg >= UDM_SETRANGE && msg <= UDM_GETBASE)
								if (!IsBadReadPtr((LPVOID)lParam, gmetUpDn[msg-(WM_USER+101)].cbExtraData))
									cbExtraData = gmetUpDn[msg-(WM_USER+101)].cbExtraData;
							break;

						case CLS_PROGRESS:
							if (msg >= PBM_SETRANGE && msg <= PBM_STEPIT)
								if (!IsBadReadPtr((LPVOID)lParam, gmetProg[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetProg[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_HOTKEY:
							if (msg >= HKM_SETHOTKEY && msg <= HKM_SETRULES)
								if (!IsBadReadPtr((LPVOID)lParam, gmetHK[msg-(WM_USER+1)].cbExtraData))
									cbExtraData = gmetHK[msg-(WM_USER+1)].cbExtraData;
							break;

						case CLS_LISTVIEW:
							if (msg >= LVM_GETBKCOLOR && msg <= LVM_GETISEARCHSTRING)
								if (!IsBadReadPtr((LPVOID)lParam, gmetLV[msg-LVM_FIRST].cbExtraData))
									cbExtraData = gmetLV[msg-LVM_FIRST].cbExtraData;
							break;

						case CLS_TREEVIEW:
							if (msg >= TVM_INSERTITEM && msg <= TVM_GETISEARCHSTRING)
								if (!IsBadReadPtr((LPVOID)lParam, gmetTV[msg-TV_FIRST].cbExtraData))
									cbExtraData = gmetTV[msg-TV_FIRST].cbExtraData;
							break;

						case CLS_TABCTRL:
							if (msg >= TCM_GETBKCOLOR && msg <= TCM_SETCURFOCUS)
								if (!IsBadReadPtr((LPVOID)lParam, gmetTab[msg-TCM_FIRST].cbExtraData))
									cbExtraData = gmetTab[msg-TCM_FIRST].cbExtraData;
							break;
#endif	// DISABLE_WIN95_MESSAGES
						default:
							break;
					}
				}		
				break;

			case MET_STRINGRET:
				if (msg < MAX_MESSAGES)
				{
					if (fPostType == POSTTYPE_RETURN && lParam && !IsBadStringPtr((LPVOID)lParam, (UINT)-1))
						cbExtraData = StringSizeof((LPCSTR)lParam, &fLongString);
				}
				else
				{
					switch (GetWindowClass(hwnd))
					{
						case CLS_DIALOG:
#ifndef DISABLE_WIN95_MESSAGES
						case CLS_ANIMATE:
						case CLS_HEADER:
						case CLS_TOOLBAR:
						case CLS_TOOLTIP:
						case CLS_STATUSBAR:
						case CLS_TRACKBAR:
						case CLS_UPDOWN:
						case CLS_PROGRESS:
						case CLS_HOTKEY:
						case CLS_LISTVIEW:
						case CLS_TREEVIEW:
						case CLS_TABCTRL:
#endif	// DISABLE_WIN95_MESSAGES
							if (fPostType == POSTTYPE_RETURN && lParam && !IsBadStringPtr((LPVOID)lParam, (UINT)-1))
								cbExtraData = StringSizeof((LPCSTR)lParam, &fLongString);
							break;
						default:
							break;
					}
				}		
				break;

			case MET_STRINGBOTH:
				if (msg < MAX_MESSAGES)
				{
					if (lParam && !IsBadStringPtr((LPVOID)lParam, (UINT)-1))
						cbExtraData = StringSizeof((LPCSTR)lParam, &fLongString);
				}
				else
				{
					switch (GetWindowClass(hwnd))
					{
						case CLS_DIALOG:
#ifndef DISABLE_WIN95_MESSAGES
						case CLS_ANIMATE:
						case CLS_HEADER:
						case CLS_TOOLBAR:
						case CLS_TOOLTIP:
						case CLS_STATUSBAR:
						case CLS_TRACKBAR:
						case CLS_UPDOWN:
						case CLS_PROGRESS:
						case CLS_HOTKEY:
						case CLS_LISTVIEW:
						case CLS_TREEVIEW:
						case CLS_TABCTRL:
#endif	// DISABLE_WIN95_MESSAGES
							if (lParam && !IsBadStringPtr((LPVOID)lParam, (UINT)-1))
								cbExtraData = StringSizeof((LPCSTR)lParam, &fLongString);
							break;
						default:
							break;
					}
				}		
				break;

			case MET_LBODSTRINGBOTH:
				//
				// Prepare to copy the string, but first check to be sure
				// that this is not an ownerdrawn listbox that does not
				// have strings.  This is necessary because the psz value
				// specified for these messages to this type of a listbox
				// is NOT a valid string pointer.
				//
				if (lParam && !IsBadStringPtr((LPVOID)lParam, (UINT)-1))
				{
					DWORD flStyle = GetWindowLong(hwnd, GWL_STYLE);

					if (!(flStyle & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) || (flStyle & LBS_HASSTRINGS))
					{
						cbExtraData = StringSizeof((LPCSTR)lParam, &fLongString);
					}
				}

				break;

			case MET_CBODSTRINGBOTH:
				//
				// Prepare to copy the string, but first check to be sure
				// that this is not an ownerdrawn combobox that does not
				// have strings.  This is necessary because the psz value
				// specified for these messages to this type of a combobox
				// is NOT a valid string pointer.
				//
				if (lParam && !IsBadStringPtr((LPVOID)lParam, (UINT)-1))
				{
					DWORD flStyle = GetWindowLong(hwnd, GWL_STYLE);

					if (!(flStyle & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE)) || (flStyle & CBS_HASSTRINGS))
					{
						cbExtraData = StringSizeof((LPCSTR)lParam, &fLongString);
					}
				}

				break;

			case MET_LBSTRINGORDRET:
				if (fPostType == POSTTYPE_RETURN && lParam)
				{
					DWORD flStyle = GetWindowLong(hwnd, GWL_STYLE);

					if (!(flStyle & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) || (flStyle & LBS_HASSTRINGS))
					{
						cbExtraData = StringSizeof((LPCSTR)lParam, &fLongString);
						fOrdinal = FALSE;
					}
					else
					{
						cbExtraData = sizeof(PACK_STRINGORD);
						fOrdinal = TRUE;
					}
				}

				break;

			case MET_CBSTRINGORDRET:
				if (fPostType == POSTTYPE_RETURN && lParam)
				{
					DWORD flStyle = GetWindowLong(hwnd, GWL_STYLE);

					if (!(flStyle & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE)) || (flStyle & CBS_HASSTRINGS))
					{
						cbExtraData = StringSizeof((LPCSTR)lParam, &fLongString);
						fOrdinal = FALSE;
					}
					else
					{
						cbExtraData = sizeof(PACK_STRINGORD);
						fOrdinal = TRUE;
					}
				}

				break;

			case MET_WM_NCCALCSIZEBOTH:
				//
				// Only dereference the pointer in lParam if
				// wParam (fCalcValidRects) is TRUE!
				//
				if (wParam)
				{
					LPNCCALCSIZE_PARAMS p2 = (LPNCCALCSIZE_PARAMS)lParam;

					if (!IsBadReadPtr(p2, sizeof(NCCALCSIZE_PARAMS)) && !IsBadReadPtr(p2->lppos, sizeof(WINDOWPOS)))
					{
						cbExtraData = sizeof(PACK_NCCALCSIZE);
					}
				}

				break;

			case MET_GLOBALSTRUCTBOTH:
				if (lParam)
				{
					LPBYTE pb = GlobalLock((HGLOBAL)lParam);

					if (pb)
					{
						if (msg < MAX_MESSAGES)
						{
							if (!IsBadReadPtr(pb, gmet[msg].cbExtraData))
								cbExtraData = gmet[msg].cbExtraData;
						}
						else
						{
							switch (GetWindowClass(hwnd))
							{
								case CLS_DIALOG:
									if (msg >= DM_GETDEFID && msg <= DM_SETDEFID)
										if (!IsBadReadPtr(pb, gmetDlg[msg-WM_USER].cbExtraData))
											cbExtraData = gmetDlg[msg-WM_USER].cbExtraData;
									break;
#ifndef DISABLE_WIN95_MESSAGES
								case CLS_ANIMATE:
									if (msg >= ACM_OPEN && msg <= ACM_STOP)
										if (!IsBadReadPtr(pb, gmetAni[msg-(WM_USER+100)].cbExtraData))
											cbExtraData = gmetAni[msg-(WM_USER+100)].cbExtraData;
									break;

								case CLS_HEADER:
									if (msg >= HDM_GETITEMCOUNT && msg <= HDM_HITTEST)
										if (!IsBadReadPtr(pb, gmetHdr[msg-HDM_FIRST].cbExtraData))
											cbExtraData = gmetHdr[msg-HDM_FIRST].cbExtraData;
									break;

								case CLS_TOOLBAR:
									if (msg >= TB_ENABLEBUTTON && msg <= TB_GETBUTTONTEXT)
										if (!IsBadReadPtr(pb, gmetTB[msg-(WM_USER+1)].cbExtraData))
											cbExtraData = gmetTB[msg-(WM_USER+1)].cbExtraData;
									break;

								case CLS_TOOLTIP:
									if (msg >= TTM_ACTIVATE && msg <= TTM_GETCURRENTTOOL)
										if (!IsBadReadPtr(pb, gmetTT[msg-(WM_USER+1)].cbExtraData))
											cbExtraData = gmetTT[msg-(WM_USER+1)].cbExtraData;
									break;

								case CLS_STATUSBAR:
									if (msg >= SB_SETTEXT && msg <= SB_GETRECT)
										if (!IsBadReadPtr(pb, gmetStat[msg-(WM_USER+1)].cbExtraData))
											cbExtraData = gmetStat[msg-(WM_USER+1)].cbExtraData;
									break;

								case CLS_TRACKBAR:
									if (msg >= TBM_GETPOS && msg <= TBM_GETTHUMBLENGTH)
										if (!IsBadReadPtr(pb, gmetTrk[msg-WM_USER].cbExtraData))
											cbExtraData = gmetTrk[msg-WM_USER].cbExtraData;
									break;

								case CLS_UPDOWN:
									if (msg >= UDM_SETRANGE && msg <= UDM_GETBASE)
										if (!IsBadReadPtr(pb, gmetUpDn[msg-(WM_USER+101)].cbExtraData))
											cbExtraData = gmetUpDn[msg-(WM_USER+101)].cbExtraData;
									break;

								case CLS_PROGRESS:
									if (msg >= PBM_SETRANGE && msg <= PBM_STEPIT)
										if (!IsBadReadPtr(pb, gmetProg[msg-(WM_USER+1)].cbExtraData))
											cbExtraData = gmetProg[msg-(WM_USER+1)].cbExtraData;
									break;

								case CLS_HOTKEY:
									if (msg >= HKM_SETHOTKEY && msg <= HKM_SETRULES)
										if (!IsBadReadPtr(pb, gmetHK[msg-(WM_USER+1)].cbExtraData))
											cbExtraData = gmetHK[msg-(WM_USER+1)].cbExtraData;
									break;

								case CLS_LISTVIEW:
									if (msg >= LVM_GETBKCOLOR && msg <= LVM_GETISEARCHSTRING)
										if (!IsBadReadPtr(pb, gmetLV[msg-LVM_FIRST].cbExtraData))
											cbExtraData = gmetLV[msg-LVM_FIRST].cbExtraData;
									break;

								case CLS_TREEVIEW:
									if (msg >= TVM_INSERTITEM && msg <= TVM_GETISEARCHSTRING)
										if (!IsBadReadPtr(pb, gmetTV[msg-TV_FIRST].cbExtraData))
											cbExtraData = gmetTV[msg-TV_FIRST].cbExtraData;
									break;

								case CLS_TABCTRL:
									if (msg >= TCM_GETBKCOLOR && msg <= TCM_SETCURFOCUS)
										if (!IsBadReadPtr(pb, gmetTab[msg-TCM_FIRST].cbExtraData))
											cbExtraData = gmetTab[msg-TCM_FIRST].cbExtraData;
									break;
#endif	// DISABLE_WIN95_MESSAGES
								default:
									break;
							}
						}
					}

					GlobalUnlock((HGLOBAL)lParam);
				}

				break;

			case MET_CLASSBOTH:
				//
				// Is there an hwndCtl in lParam?
				//
				if (lParam)
				{
					cbExtraData = sizeof(PACK_CLASS);
				}

				break;

			case MET_WPLPDWORDSRET:
				if (fPostType == POSTTYPE_RETURN)
				{
					cbExtraData = sizeof(PACK_WPLPDWORDS);
				}

				break;

			case MET_EM_GETLINE:
				if (fPostType == POSTTYPE_RETURN)
				{
					//
					// On return, the lResult will be the number of
					// characters copied.
					//
					int cch = (INT)lResult;

					if (cch > CCHMAXPARMSTRING)
					{
						cch = CCHMAXPARMSTRING;
					}

					if (cch > 0 && !IsBadReadPtr((LPVOID)lParam, (UINT)cch))
					{
						//
						// Allow size for the characters.
						// Don't forget room for the null.
						//
						cbExtraData = (DWORD)(cch + 1);
					}
				}
				else
				{
					//
					// The string buffer pointed to by lParam should
					// contain a WORD that is the max size of the
					// buffer.
					//
					if (!IsBadReadPtr((LPVOID)lParam, sizeof(WORD)))
					{
						cbExtraData = sizeof(WORD);
					}
				}

				break;

			case MET_INTARRAYBOTH:
				{
					//
					// The wParam has the number of ints in the buffer.
					//
					int cItems = (INT)wParam;

					if (cItems > CMAXINTARRAYITEMS)
					{
						cItems = CMAXINTARRAYITEMS;
					}

					if (cItems > 0 && !IsBadReadPtr((LPVOID)lParam, cItems * sizeof(INT)))
					{
						cbExtraData = (DWORD)(cItems * sizeof(INT));
					}
				}

				break;

			case MET_INTARRAYRET:
				if (fPostType == POSTTYPE_RETURN)
				{
					//
					// On return, the lResult will be the number of
					// items (ints) placed in the buffer, or -1 for
					// an error.
					//
					int cItems = (INT)lResult;

					if (cItems > CMAXINTARRAYITEMS)
					{
						cItems = CMAXINTARRAYITEMS;
					}

					if (cItems > 0 && !IsBadReadPtr((LPVOID)lParam, cItems * sizeof(INT)))
					{
						cbExtraData = (DWORD)(cItems * sizeof(INT));
					}
				}

				break;

			case MET_FONTBOTH:
				//
				// Be sure that the font (in wParam) is not null (which
				// indicates the system font).
				//
				if (wParam)
				{
					//
					// We will be returning a PACK_FONT structure.
					//
					cbExtraData = sizeof(PACK_FONT);
				}

				break;

			case MET_FONTRET:
				//
				// Only try and return the font on a return message
				// and be sure that the return is not null (which
				// indicates the system font).
				//
				if (fPostType == POSTTYPE_RETURN && lResult)
				{
					//
					// We will be returning a PACK_FONT structure.
					//
					cbExtraData = sizeof(PACK_FONT);
				}

				break;
		}
	}

	cbSize = sizeof(MSGSTREAMDATA) + cbExtraData;
	cbSize += (4 - (cbSize & 3)) % 4;
	pmsd = (PMSGSTREAMDATA)GetMsgPacket(cbSize);

	pmsd->cb = cbSize;
	pmsd->fPostType = fPostType;
	pmsd->nLevel = nLevel;
	pmsd->hwnd = hwnd;
	pmsd->msg = msg;
	pmsd->wParam = wParam;
	pmsd->lParam = lParam;
	pmsd->lResult = lResult;
	pmsd->time = time;
	pmsd->ptX = ptX;
	pmsd->ptY = ptY;

	if (cbExtraData)
	{
		switch (fEncoding)
		{
			case MET_STRUCTRET:
			case MET_STRUCTBOTH:
			case MET_INTARRAYBOTH:
			case MET_INTARRAYRET:
				memcpy(pmsd + 1, (BYTE*)lParam, cbExtraData);
				break;

			case MET_STRINGRET:
			case MET_STRINGBOTH:
			case MET_LBODSTRINGBOTH:
			case MET_CBODSTRINGBOTH:
				StringCopy((CHAR*)(pmsd + 1), (LPCSTR)lParam, cbExtraData, fLongString);
				break;

			case MET_LBSTRINGORDRET:
			case MET_CBSTRINGORDRET:
				if (fOrdinal)
				{
					PPACK_STRINGORD pso = (PPACK_STRINGORD)(pmsd + 1);
					pso->dwMarker = 0xFFFFFFFF;
					pso->dwOrdinal = *(PDWORD)lParam;
				}
				else
				{
					StringCopy((CHAR*)(pmsd + 1), (LPCSTR)lParam, cbExtraData, fLongString);
				}

				break;

			case MET_WM_NCCALCSIZEBOTH:
				{
					PPACK_NCCALCSIZE p = (PPACK_NCCALCSIZE)(pmsd + 1);
					LPNCCALCSIZE_PARAMS p2 = (LPNCCALCSIZE_PARAMS)lParam;
					memcpy(&p->nccp, p2, sizeof(NCCALCSIZE_PARAMS));
					memcpy(&p->wp, p2->lppos, sizeof(WINDOWPOS));
				}

				break;

			case MET_GLOBALSTRUCTBOTH:
				{
					LPBYTE pb = GlobalLock((HGLOBAL)lParam);

					if (pb)
					{
						memcpy(pmsd + 1, pb, cbExtraData);
						GlobalUnlock((HGLOBAL)lParam);
					}
				}

				break;

			case MET_CREATESTRUCTBOTH:
				{
					PPACK_CREATESTRUCT p = (PPACK_CREATESTRUCT)(pmsd + 1);
					LPCREATESTRUCT p2 = (LPCREATESTRUCT)lParam;
					DWORD cbString;
					BOOL fLS;

					memcpy(&p->cs, p2, sizeof(CREATESTRUCT));

					if (HIWORD(p2->lpszName) && !IsBadStringPtr(p2->lpszName, (UINT)-1))
					{
						cbString = StringSizeof(p2->lpszName, &fLS);
						StringCopy(p->szName, p2->lpszName, cbString, fLS);
						p->fValidName = TRUE;
					}
					else
					{
						p->fValidName = FALSE;
					}

					if (HIWORD(p2->lpszClass) && !IsBadStringPtr(p2->lpszClass, (UINT)-1))
					{
						cbString = StringSizeof(p2->lpszClass, &fLS);
						StringCopy(p->szClass, p2->lpszClass, cbString, fLS);
						p->fValidClass = TRUE;
					}
					else
					{
						p->fValidClass = FALSE;
					}
				}

				break;

			case MET_MDICREATESTRUCTBOTH:
				{
					PPACK_MDICREATESTRUCT p = (PPACK_MDICREATESTRUCT)(pmsd + 1);
					LPMDICREATESTRUCT p2 = (LPMDICREATESTRUCT)lParam;
					DWORD cbString;
					BOOL fLS;

					memcpy(&p->mdic, p2, sizeof(MDICREATESTRUCT));

					if (HIWORD(p2->szTitle) && !IsBadStringPtr(p2->szTitle, (UINT)-1))
					{
						cbString = StringSizeof(p2->szTitle, &fLS);
						StringCopy(p->szTitle, p2->szTitle, cbString, fLS);
						p->fValidTitle = TRUE;
					}
					else
					{
						p->fValidTitle = FALSE;
					}

					if (HIWORD(p2->szClass) && !IsBadStringPtr(p2->szClass, (UINT)-1))
					{
						cbString = StringSizeof(p2->szClass, &fLS);
						StringCopy(p->szClass, p2->szClass, cbString, fLS);
						p->fValidClass = TRUE;
					}
					else
					{
						p->fValidClass = FALSE;
					}
				}

				break;

			case MET_CLASSBOTH:
				{
					PPACK_CLASS p = (PPACK_CLASS)(pmsd + 1);
					p->nClass = GetWindowClass((HWND)lParam);
				}

				break;

			case MET_WPLPDWORDSRET:
				{
					PPACK_WPLPDWORDS p = (PPACK_WPLPDWORDS)(pmsd + 1);

					if (!IsBadReadPtr((PVOID)wParam, sizeof(DWORD)))
					{
						p->fValidWP = TRUE;
						p->nWP = *(PDWORD)wParam;
					}
					else
					{
						p->fValidWP = FALSE;
					}

					if (!IsBadReadPtr((PVOID)lParam, sizeof(DWORD)))
					{
						p->fValidLP = TRUE;
						p->nLP = *(PDWORD)lParam;
					}
					else
					{
						p->fValidLP = FALSE;
					}
				}

				break;

			case MET_EM_GETLINE:
				if (fPostType == POSTTYPE_RETURN)
				{
					//
					// On return, the lResult will be the number of
					// characters copied.
					//
					int cch = (INT)lResult;

					if (cch > CCHMAXPARMSTRING)
					{
						cch = CCHMAXPARMSTRING;
						fLongString = TRUE;
					}
					else
					{
						fLongString = FALSE;
					}

					//
					// Copy the characters over.  Because the data is
					// not already null terminated, we must explicitly
					// do that here as well.
					//
					StringCopy((CHAR*)(pmsd + 1), (CHAR*)lParam, cch, fLongString);
					*((CHAR*)(pmsd + 1) + cch) = 0;
				}
				else
				{
					*(WORD*)(pmsd + 1) = *(WORD*)lParam;
				}

				break;

			case MET_FONTBOTH:
				CopyFont((PPACK_FONT)(pmsd + 1), (HFONT)wParam);
				break;

			case MET_FONTRET:
				CopyFont((PPACK_FONT)(pmsd + 1), (HFONT)lResult);
				break;
		}
	}

	gcMsgPackets++;
	goffWrite = ((PBYTE)pmsd - (PBYTE)gabMsgBuf) + cbSize;

	SetEvent(ghWrittenEvent);
	ReleaseMutex(ghAccessMutex);
	ReleaseMutex(ghWriterMutex);
#endif
}

//-----------------------------------------------------------------------------
// GetMsgPacket
//
// This routine will grab the writer and access semaphores and find a
// block in the message buffer area large enough for a message packet
// of the specified size.  It will block and allow the reader to read
// some packets to free up space, if necessary.  When it returns, it
// will own both the writer and access mutexes.
//
// Note that the cbSize MUST be rounded up to a DWORD boundary for
// the packet writing mechanism to work properly!
//
// If the buffer write pointer needs to be looped back to the beginning
// of the circular buffer, a DWORD of zero will be written out before
// the loop is done.  This allows the reader to notice a packet that
// starts with a zero size, meaning that it should jump to the
// beginning of the buffer for the real next packet.
//
// Arguments:
//  DWORD cbSize - Size in bytes required for the message packet
//				 (rounded up to a DWORD boundary).
//
// Returns:
//  A pointer to an area to write the message packet.
//
//-----------------------------------------------------------------------------

PRIVATE PBYTE GetMsgPacket(DWORD cbSize)
{
	PBYTE pb;

	WaitForSingleObject(ghWriterMutex, INFINITE);
	WaitForSingleObject(ghAccessMutex, INFINITE);

	pb = gabMsgBuf + goffWrite;

	//
	// Is the packet too big to write without wrapping around?
	//
	if (pb + cbSize >= gabMsgBuf + CBMSGBUFFER)
	{
		//
		// Check if the reader is ahead of the writer
		//
		while (gcMsgPackets && pb <= gabMsgBuf + goffRead)
		{
			//
			// Be sure to allow the reader to wrap around first!
			//
			ResetEvent(ghReadEvent);
			ReleaseMutex(ghAccessMutex);
			WaitForSingleObject(ghReadEvent, INFINITE);
			WaitForSingleObject(ghAccessMutex, INFINITE);
		}

		//
		// Wrap around in the buffer
		//
		*((PDWORD)pb) = 0;
		pb = gabMsgBuf;
	}

	//
	// Loop allowing the reader to read until either there are
	// no more packets left or the reader wraps around (we already
	// guaranteed above that there is enough room up to the end
	// of the buffer, we just need to be sure the reader is out
	// of the way) or until the reader reads enough to allow the
	// new packet to be added.
	//
	while (gcMsgPackets && pb <= gabMsgBuf + goffRead && pb + cbSize > gabMsgBuf + goffRead)
	{
		//
		// Wait until enough is read by the reader
		//
		ResetEvent(ghReadEvent);
		ReleaseMutex(ghAccessMutex);
		WaitForSingleObject(ghReadEvent, INFINITE);
		WaitForSingleObject(ghAccessMutex, INFINITE);
	}

	return pb;
}

//-----------------------------------------------------------------------------
// GetWindowClass
//
//  Returns the window class that a given window has.
//
// Arguments:
//  HWND hwnd - Window to get the class of.
//
// Returns:
//  One of the CLS_* defines.  The value CLS_UNKNOWN will be returned
//  if the window is not one of the known types in the class atom table.
//
//-----------------------------------------------------------------------------

UINT GetWindowClass(HWND hwnd)
{
	UINT Class = CLS_UNKNOWN;
	ATOM aClass;
	INT i;

	aClass = (ATOM)GetClassWord(hwnd, GCW_ATOM);
	if (aClass)
	{
		//
		// Search the array for this atom.
		//
		for (i = 0; i < MAXCLASSATOMS; i++)
		{
			if (aClass == gaaClasses[i])
			{
				Class = i;
				break;
			}
		}
	}

	return Class;
}

//-----------------------------------------------------------------------------
// StringSizeof
//
//  Returns the length of the specified string.
//
// Arguments:
//  LPCSTR psz		  - The string to get the size of.
//  BOOL* pfLongString  - Flag to set to TRUE or FALSE depending on the length.
//
// Returns:
//  The size of the string in bytes, including room for the null
//  terminator.  If the string is longer than CCHMAXPARMSTRING, it
//  will set pfLongString to TRUE and return a size as if the string
//  were truncated at CCHMAXPARMSTRING.  If not, it will set pfLongString
//  to FALSE.
//
//-----------------------------------------------------------------------------

PRIVATE DWORD StringSizeof(LPCSTR psz, BOOL* pfLongString)
{
	DWORD cch = strlen(psz);

	if (cch > CCHMAXPARMSTRING)
	{
		cch = CCHMAXPARMSTRING;
		*pfLongString = TRUE;
	}
	else
	{
		*pfLongString = FALSE;
	}

	return (cch + 1) * sizeof(CHAR);
}

//-----------------------------------------------------------------------------
// StringCopy
//
//  Copies a string.  If the string is too long (specified by the
//  fLongString flag) then the end of the string is set to "..."
//  and it is null terminated.  If fLongString is FALSE, it is
//  assumed that cb bytes should be copied and that this is enough
//  to copy the null.
//
// Arguments:
//  CHAR* pszDest	- The destination buffer.
//  LPCSTR pszSrc	- The source string.
//  DWORD cb		 - The length of the string in bytes.
//  BOOL fLongString - TRUE if the string was too long, FALSE otherwise.
//
//-----------------------------------------------------------------------------

PRIVATE void StringCopy(CHAR* pszDest, LPCSTR pszSrc, DWORD cb, BOOL fLongString)
{
	memcpy(pszDest, pszSrc, cb);

	if (fLongString)
	{
		pszDest[cb - 4] = '.';
		pszDest[cb - 3] = '.';
		pszDest[cb - 2] = '.';
		pszDest[cb - 1] = 0;
	}
}

//-----------------------------------------------------------------------------
// CopyFont
//
// Given a font, gets the facename and pointsize for it and fills
// the specified PACK_FONT structure with the information.  If an
// error occurs, the nHeight field of the PACK_FONT structure
// will be set to -1.
//
// Arguments:
//  PPACK_FONT ppf - Pointer to the PACK_FONT structure to fill.
//  HFONT hfont	- The font handle.
//
//-----------------------------------------------------------------------------

PRIVATE VOID CopyFont(PPACK_FONT ppf, HFONT hfont)
{
	TEXTMETRIC tm;
	HDC hdc;
	HFONT hfontOld;

	//
	// Assume the worst...
	//
	ppf->nHeight = -1;

	//
	// We need a DC for the GetTextxxx calls below.
	//
	if (hdc = GetDC(NULL))
	{
		hfontOld = SelectObject(hdc, hfont);

		//
		// Grab the text face name and put it in the specified buffer.
		//
		if (GetTextFace(hdc, LF_FACESIZE, ppf->szFaceName))
		{
			if (GetTextMetrics(hdc, &tm))
			{
				//
				// Finally, grab the height and set the nHeight field.
				// We are done except for cleanup at this point.
				//
				ppf->nHeight = tm.tmHeight - tm.tmInternalLeading;
			}
		}

		SelectObject(hdc, hfontOld);
		ReleaseDC(NULL, hdc);
	}
}

#ifdef _DEBUG
/****************************************************************************
* dprintf
*
* This debugging function prints out a string to the debug output.
* An optional set of substitutional parameters can be specified,
* and the final output will be the processed result of these combined
* with the format string, just like printf.  A newline is always
* output after every call to this function.
*
* Arguments:
*   LPTSTR fmt - Format string (printf style).
*   ...		- Variable number of arguments.
*
* History:
*  28-Aug-1990  Byron Dazey - Created
****************************************************************************/

#undef dprintf

VOID dprintf(LPTSTR fmt, ...)
{
	va_list marker;
	TCHAR szBuf[256];
	INT len;

	va_start(marker, fmt);
	len = wvsprintf(szBuf, fmt, marker);
	va_end(marker);

	szBuf[len++] = TEXT('\r');
	szBuf[len++] = TEXT('\n');
	szBuf[len] = 0;

	OutputDebugString(szBuf);
}
#endif
