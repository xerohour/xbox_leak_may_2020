#include "stdafx.h"
#include "fbrdbg.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

extern int iCallsCxf ;
extern BOOL CLDispCallInfo( int iCall );

static BOOL LocationDisplay = TRUE;		// True if we're displaying function names, false if address
static BOOL OldLocationDisplay;			// Save the LocationDisplay BOOL if the user clicks CANCEL
static UINT FocusThread = (UINT) -1;	// Index of focus thread
static BOOL fMustRestoreStack;			// We must restore current thread's stack when we're done

/****************************************************************************

	FUNCTION:	MakeThreadObject

	PURPOSE:	Initialize a Thread_Object Structure

****************************************************************************/
static void MakeThreadObject(THREAD_OBJECT *Object, DWORD id,
	DWORD cSuspend, DWORD cSuspendNew, LPSTR priority, BOOL HasFocus,
	DWORD Address, PSTR Location)
{
	Object->id = id;
	Object->cSuspend = cSuspend;
	Object->cSuspendNew = cSuspendNew;
	_ftcsncpy(Object->szPriority,priority,sizeof(Object->szPriority)-1);
	Object->szPriority[sizeof(Object->szPriority)-1] = '\0';
	Object->HasFocus = HasFocus;
	Object->address = Address;
	_ftcsncpy((char *)Object->Location,Location,THREAD_FIELD5-THREAD_FIELD4-1);
}

/****************************************************************************

	FUNCTION:	MakeString

	PURPOSE:	Create a user string from a THREAD_OBJECT struct

****************************************************************************/
static void MakeString(THREAD_OBJECT *Object, char *Buffer, int cch)
{
	char szState[THREAD_FIELD3-THREAD_FIELD2+1];
	char szAddress[THREAD_FIELD5-THREAD_FIELD4+1];

	if (cch < THREAD_FIELD5)
	{
		ASSERT(FALSE);
		Buffer[0] = '\0';
		return;
	}

	// show suspend count
	_snprintf(szState, sizeof(szState), "%lu", Object->cSuspendNew);

	if (LocationDisplay)
	{
		_ftcsncpy(szAddress, Object->Location, sizeof(szAddress));
		szAddress[sizeof(szAddress)-1] = _T('\0');
	}
	else
		sprintf(szAddress, "%08lx", Object->address);

	sprintf(Buffer, "%c%-*.*lx %-*.*s %-*.*s %-*.*s",
		Object->HasFocus?'*':' ',
		THREAD_FIELD2-THREAD_FIELD1-1,8,
		Object->id,
		THREAD_FIELD3-THREAD_FIELD2-1,THREAD_FIELD3-THREAD_FIELD2-1,
		szState,
		THREAD_FIELD4-THREAD_FIELD3-1,THREAD_FIELD4-THREAD_FIELD3-1,
		Object->szPriority,
		THREAD_FIELD5-THREAD_FIELD4-1,THREAD_FIELD5-THREAD_FIELD4-1,
		szAddress);
}

/****************************************************************************

	FUNCTION:	GetSymbolFromAddr

	PURPOSE:	Given an ADDR, search for the nearest symbol name
				Returns TRUE if symbol found

****************************************************************************/
BOOL GetSymbolFromAddr(PADDR pAddr, CString& strSymbolName)
{
	CXT CXTT;
	HMOD hmod = 0;
	HSYM hSym;
	char rgch [100];
	ADDR Addr; // Unfixed up address

	// Setup context struct
	memset(&CXTT, 0, sizeof(CXT));
	SHHMODFrompCXT(&CXTT) = (HMOD) NULL;

	// Unfix up the address
	Addr = *pAddr;
	SYUnFixupAddr(&Addr);

	// load the context from the address
	if(!SHSetCxt(&Addr, &CXTT))
	{
		return (FALSE); // problem
	}

	// Get hmod from context
	hmod = SHHMODFrompCXT(&CXTT);
	if (!hmod)
	{
		return (FALSE); // problem
	}

	// check the symbol table
	if (SHGetNearestHsym(&Addr,hmod,EECODE,&hSym) == CV_MAXOFFSET)
	{
		// check the publics
		if (PHGetNearestHsym(&Addr,SHHexeFromHmod(hmod),&hSym) == CV_MAXOFFSET)
		{
			// Not found..
			return (FALSE);
		}
	}

	// At this point we have a valid HSYM
	memset(rgch, 0, sizeof(rgch));
	if (SHGetSymName (hSym, rgch) != NULL)
	{
		strSymbolName = rgch;
		return TRUE;
	}
	else
	{
		strSymbolName.Empty();
		return FALSE;
	}
}

/****************************************************************************

	FUNCTION:	GetThreadInfo

	PURPOSE:	Ask OSDebug for information about a thread

****************************************************************************/
static BOOL GetThreadInfo(BOOL FirstThread, UINT *id,
					DWORD *cSuspend, LPSTR *Priority, UINT *HasFocus,
					DWORD *Address, LPSTR Location, UINT cch)
{
	static HTHD hthd;
	static TST ThreadStatus; // static so we can pass part of it back to caller
	HLLI ListId;
	LPTHD lpthd;
	HTID htid;
	HPID hpid;
	ADDR Addr = {0};
	BOOL fAlive;
	CString strLocation;

	if (FirstThread)
		hthd = NULL;	// We want the first thread of the list

	// Get pid of current process
	hpid = HpidFromHprc(hprcCurr);

	// Get the ID of the list of threads
	ListId = LlthdFromHprc (hprcCurr);

	// Get the handle of a live thread
	do
	{
		hthd = (HTHD)LLHlleFindNext(ListId,hthd);
		if (hthd)
		{
			lpthd = (LPTHD)LLLpvFromHlle(hthd);
			fAlive = !(lpthd->tdf & tdfDead);
			UnlockHlle(hthd);
		}
	}
	while (hthd && !fAlive);

	// Is there any thread ?
	if (hthd == NULL)
		return (FALSE);

	// Get a pointer to the thread structure
	lpthd = (LPTHD)LLLpvFromHlle ( hthd );
	if (lpthd == NULL)
		return (FALSE);

	// Unlock the thread structure
	UnlockHlle(hthd);

	// Get NT thread ID
	*id = lpthd->tid;

	// Get OSD thread ID
	htid = lpthd->htid;

	// Is it the current thread ?
	if (hthd == hthdCurr)
	{
		*HasFocus = TRUE;

		// Print current tid in status bar
// this has never been enabled, but if the feature gets added, here's the place.
//		StatusTid(*id);
	}
	else
		*HasFocus = FALSE;

	// Get the thread PC address
	OSDGetAddr(hpidCurr,htid,adrPC,(PADDR)&Addr);

	*Address = GetAddrOff(Addr); // only get the 32-bit offset

	// Get the thread location.  First, see if we can determine a function
	// name for the current CS:EIP.  If we can't, then look up the stack
	// for the first address for which we DO have symbols.
	Location[0] = '\0';
	if (!GetSymbolFromAddr(&Addr,strLocation))
	{
		// If the address that the top of stack doesn't have a symbol,
		// then look up the stack, and if we find something we recognize,
		// put its function name in brackets
		int i;
		HFME hfme;
		LPFME lpfme;

		// We need to get the callstack for a different thread, but when
		// we're done, we are going to need to restore whatever callstack
		// info is currently available.
		if ( fMustRestoreStack = (hthdCurr != hthd) )
		{
			CLFreeWalkbackStack( TRUE );
		}

		// Initialize the call stack chain with the appropriate thread
		CLGetWalkbackStack(hpidCurr, htid, 0);

		// start at i==1, to skip first element, which we already checked
		for (i = 1; (hfme = CLHfmeGetNth(i)) != NULL; ++i)
		{
			lpfme = (LPFME)LLLpvFromHlle(hfme);
			if (lpfme->clt == cltProc)
			{
				// found a function: format as "[func_name]"
				if (GetSymbolFromAddr(&lpfme->addrProc, strLocation))
				{
					sprintf(Location, "[%.*s]", cch-3,
						(const char*)strLocation);
					break;
				}
			}
			UnlockHlle(hfme);
		}
		if (hfme) {
			UnlockHlle(hfme);
		}
	}
	else
	{
		_tcsncpy(Location, (LPCTSTR)strLocation, cch);
		Location[cch-1] = '\0';
	}

	// Get thread status in tst structure
	VERIFY(OSDGetThreadStatus(hpidCurr,htid, &ThreadStatus) == xosdNone);

	*cSuspend = ThreadStatus.dwSuspendCount;
	*Priority = ThreadStatus.rgchPriority;

	if (ThreadStatus.rgchThreadID[0])
	{
		// thread has a name, insert on the front of Location
		CString oldLoc = Location;
		_snprintf( Location, cch-1, "%s,%s", ThreadStatus.rgchThreadID, (LPCTSTR)oldLoc );
		Location[cch-1] = 0;
	}

	return (TRUE);
}

/****************************************************************************

	FUNCTION:	InitDefaultThreadList

	PURPOSE:	Get default thread list

****************************************************************************/
static void InitDefaultThreadList(void)
{
	THREAD_OBJECT Object;
	char Location[THREAD_FIELD5-THREAD_FIELD4+1];
	UINT id, HasFocus;
	LPSTR Priority;
	UINT ListboxIndex = 0;
	DWORD Address;
	DWORD cSuspend;
	BOOL IsFirstThread = TRUE;
	int chfme;
	int iCallsPrev;

	// GetThreadInfo may call ClGetWalkbackStack on some thread other than
	// the current thread, in which case we'll need to restore the stack
	// to its previous state after we're done looping
	chfme = CLChfmeOnStack();
	fMustRestoreStack = FALSE ;
	iCallsPrev = iCallsCxf;

	FocusThread = (UINT) -1;

	// Get the thread informations
	while (GetThreadInfo(IsFirstThread, &id, &cSuspend, &Priority,
		&HasFocus, &Address, Location, sizeof(Location)))
	{
		// Create an object
		MakeThreadObject(&Object, id, cSuspend, cSuspend, Priority,
			HasFocus, Address, Location);

		// Add the thread object in the linked list
		VERIFY(ListAddObject(DLG_THREAD_LIST,(LPSTR)&Object,sizeof(THREAD_OBJECT)));

		// Remember if this is the thread with focus
		if (HasFocus)
			FocusThread = ListboxIndex;

		// We want the next threads from GetThreadInfo
		IsFirstThread = FALSE;

		// Move on to the next listbox entry
		ListboxIndex++;
	}

	// Now restore whatever stack info existed before we started
	if (fMustRestoreStack)
	{
		if (chfme == 0)
			CLFreeWalkbackStack(FALSE);
		else
		{
			CLGetWalkbackStack(hpidCurr, htidCurr, chfme);
			iCallsCxf = iCallsPrev;
		}
	}
}

/****************************************************************************

	FUNCTION:	UpdateListboxContent

	PURPOSE:	Copy the content of the linked list into the listbox

 ****************************************************************************/
static void UpdateListboxContent(HWND hDlg)
{
	int		Count,i;
	char	Buffer[THREAD_FIELD5+1];
	THREAD_OBJECT Object;

	SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_RESETCONTENT,0,0);
	Count = (int)ListGetCount(DLG_THREAD_LIST);
	for (i=0; i<Count; i++)
	{
		if (!ListGetObject(i,DLG_THREAD_LIST,(LPSTR)&Object))
		{
			ASSERT(FALSE);
			EmptyList(DLG_THREAD_LIST);
			SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_RESETCONTENT,0,0);
			break;
		}
		MakeString(&Object,Buffer,sizeof(Buffer));
		SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_INSERTSTRING,i,(LPARAM)(LPSTR)Buffer);
	}
}

/****************************************************************************

	FUNCTION:	EnableControls

	PURPOSE:	Enable or disable some buttons in the thread box

****************************************************************************/
static void EnableControls(HWND hDlg)
{
	UINT	Selection;
	BOOL	CanSuspend,CanResume,CanSetFocus;
	THREAD_OBJECT Object;
	ULONG	cSuspendMax;

	// If no selection, disable all buttons
	CanSuspend = CanResume = CanSetFocus = FALSE;
	Selection = (UINT)SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_GETCURSEL,0,0);
	if (Selection != LB_ERR)
	{
		// Thread selected, what can we do with it ?
		if (!ListGetObject(Selection,DLG_THREAD_LIST,(LPSTR)&Object))
		{
			ASSERT(FALSE);
			EmptyList(DLG_THREAD_LIST);
		}
		else
		{
		TST tst;

		if (OSDGetThreadStatus(hpidCurr, htidCurr, &tst) != xosdNone)
			cSuspendMax = 0;
		else
			cSuspendMax = tst.dwSuspendCountMax;
		CanSetFocus = TRUE;
		CanSuspend = (Object.cSuspendNew < cSuspendMax);
		CanResume = (Object.cSuspendNew > 0);
	}
	}

	// Dis/Enable the buttons
	EnableWindow(GetDlgItem(hDlg, DLG_THREAD_FOCUS),CanSetFocus);
	EnableWindow(GetDlgItem(hDlg, DLG_THREAD_SUSPEND),CanSuspend);
	EnableWindow(GetDlgItem(hDlg, DLG_THREAD_RESUME),CanResume);
}

/****************************************************************************

	FUNCTION:	SetThreadState(HWND hDlg, UINT id, UINT NewState)

	PURPOSE:	Modify the state of a thread

****************************************************************************/
void SetThreadState(
	HWND hDlg,
	UINT SelectedId,		// SelectedId = index of thread in list
	UINT ModifiedState)
{
	THREAD_OBJECT Object;
	char Buffer[THREAD_FIELD5+1];

	// Get object from list
	if (ListGetObject(SelectedId,DLG_THREAD_LIST,(LPSTR)&Object))
	{
		// Modify state
		if (ModifiedState == THREAD_FOCUS)
		{
			// If thread is given focus, remove focus from other thread
			if (FocusThread != (UINT) -1)
				SetThreadState(hDlg, FocusThread, THREAD_KILLFOCUS);
			Object.HasFocus = TRUE;
			FocusThread = SelectedId;
		}
		else if (ModifiedState == THREAD_KILLFOCUS)
			Object.HasFocus = FALSE;
		else if (ModifiedState == DLG_THREAD_SUSPEND)
			Object.cSuspendNew++;
		else if (ModifiedState == DLG_THREAD_RESUME)
		{
			ASSERT(Object.cSuspendNew > 0);
			Object.cSuspendNew--;
		}
		else
			ASSERT(FALSE);	// unrecognized command

		// Update the list
		ListUpdateObject(SelectedId,DLG_THREAD_LIST,(LPSTR)&Object,sizeof(THREAD_OBJECT));

		// Update the listbox
		SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_DELETESTRING,SelectedId,0);
		// Create the string, and add it in the listbox
		MakeString(&Object,Buffer,sizeof(Buffer));
		SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_INSERTSTRING,SelectedId,(LPARAM)(LPSTR)Buffer);
		SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_SETCURSEL,SelectedId,0);
	}
	// Enable valid controls
	EnableControls(hDlg);
}

/****************************************************************************

	FUNCTION:	ValidateNewThreadStates(void)

	PURPOSE:	Make all the changes made in the dialog box come true

****************************************************************************/
static void ValidateNewThreadStates(void)
{
	THREAD_OBJECT Object;
	DWORD cSuspend;
	HTID htid;
	HPID hpid;
	UINT ThreadNumber,index;

	// for each thread in the list, check if state has changed
	ThreadNumber = ListGetCount(DLG_THREAD_LIST);

	for (index = 0; index <ThreadNumber; index++)
	{
		// Get object from list
		if (ListGetObject(index,DLG_THREAD_LIST,(LPSTR)&Object))
		{
		cSuspend = Object.cSuspend;

		// Does the state change ?
		if (cSuspend != Object.cSuspendNew)
		{
			hpid = HpidFromHprc(hprcCurr);
			htid = HtidFromTid(hprcCurr, Object.id);

			// one of these two loops will execute, the other won't
			while (cSuspend < Object.cSuspendNew)
			{
				if (OSDFreezeThread(hpid, htid, TRUE) != xosdNone)
				{
					ErrorBox(DBG_Cannot_Suspend_Thread, Object.id);
					break;
				}
				cSuspend++;
			}
			while (cSuspend > Object.cSuspendNew)
			{
				if (OSDFreezeThread(hpid, htid, FALSE) != xosdNone)
				{
					ASSERT(FALSE);
					break;
				}
				cSuspend--;
			}
		}

		// Is this thread the current thread ?
		if (Object.HasFocus)
		{
			// Get the htid from the tid
			htid = HtidFromTid (hprcCurr, Object.id);

			// Don't do anything if the current thread hasn't changed
			if (htid != htidCurr)
			{
				// Print current tid in status bar
// this has never been enabled, but if the feature gets added, here's the place.
//				StatusTid(Object.id);

				// Set the hthdcurr value
				SYSetContext(hprcCurr, HthdFromHtid(hprcCurr, htid), FALSE);
				// Refresh all debug windows
				UpdateDebuggerState(UPDATE_DEBUGGEE|UPDATE_ALLDBGWIN|UPDATE_SOURCE);
			}
		}
	}
	}
}

/****************************************************************************

	FUNCTION:	AlignColumnHeaders(HWND, HFONT)

	PURPOSE:	Repositions the column headers above the list box based
				on the size of of the fixed font, so that they line up
				nicely with the text in the listbox.

****************************************************************************/
static void
AlignColumnHeaders(HWND hDlg, HFONT hFont)
{
	HDC hdc;
	TEXTMETRIC tm;
	RECT rect;
	LONG xLeft;
	LONG cx, cy;
	HWND hwndItem;

	hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
	ASSERT(hdc != NULL);
	SelectObject(hdc, (HGDIOBJ) hFont);
	VERIFY(GetTextMetrics(hdc, &tm));
	VERIFY(DeleteDC(hdc));

	VERIFY(GetWindowRect(GetDlgItem(hDlg, DLG_THREAD_LIST), &rect));
	VERIFY(ScreenToClient(hDlg, (LPPOINT) &rect.left));
	// FUTURE: HACK: I can't figure out how to get the left edge of the
	// actual text in the listbox, rather than the left edge of the listbox
	// itself.  The "+3*GetSystemMetrics(SM_CXBORDER)" below is a hack to
	// work around this: on my high-res screen the border is one pixel and
	// this is followed by two pixels of whitespace, so this calculation
	// is good enough.
	xLeft = rect.left + 3*GetSystemMetrics(SM_CXBORDER);

	hwndItem = GetDlgItem(hDlg, DLG_THREAD_STATE);
	VERIFY(GetWindowRect(hwndItem, &rect));
	cx = rect.right - rect.left;
	cy = rect.bottom - rect.top;
	VERIFY(ScreenToClient(hDlg, (LPPOINT) &rect.left));
	VERIFY(MoveWindow(hwndItem,
		xLeft + tm.tmAveCharWidth * THREAD_FIELD2,
		rect.top, cx, cy, TRUE));

	hwndItem = GetDlgItem(hDlg, DLG_THREAD_PRIORITY);
	VERIFY(GetWindowRect(hwndItem, &rect));
	cx = rect.right - rect.left;
	cy = rect.bottom - rect.top;
	VERIFY(ScreenToClient(hDlg, (LPPOINT) &rect.left));
	VERIFY(MoveWindow(hwndItem,
		xLeft + tm.tmAveCharWidth * THREAD_FIELD3,
		rect.top, cx, cy, TRUE));

	hwndItem = GetDlgItem(hDlg, DLG_THREAD_LOCATION);
	VERIFY(GetWindowRect(hwndItem, &rect));
	cx = rect.right - rect.left;
	cy = rect.bottom - rect.top;
	VERIFY(ScreenToClient(hDlg, (LPPOINT) &rect.left));
	VERIFY(MoveWindow(hwndItem,
		xLeft + tm.tmAveCharWidth * THREAD_FIELD4,
		rect.top, cx, cy, TRUE));
}

/****************************************************************************

	FUNCTION:	DlgThread(HWND, unsigned, UINT, LONG)

	PURPOSE:	Processes messages for "THREAD" dialog box

****************************************************************************/
BOOL FAR PASCAL EXPORT DlgThread(
	HWND       hDlg,
	unsigned   message,
	UINT       wParam,
	LONG       lParam)
{
	OFBRS ofbr;
	HTID htid;
			
	switch (message)
	{
	case WM_CONTEXTMENU:
	{
		// "What's this?" help
		DoHelpContextMenu(CWnd::FromHandle(hDlg), IDD_THREADS, CPoint(LOWORD(lParam),HIWORD(lParam)));
	}
	return TRUE;

	case WM_HELP:
	{
		// handler for click after arrow button pressed
		DoHelpInfo(CWnd::FromHandle(hDlg), IDD_THREADS, (HELPINFO *)lParam);
	}
	return TRUE;

	case WM_INITDIALOG:
		{
		int Count;
			HFONT hFont;
			// Radio button state
			OldLocationDisplay = LocationDisplay;
			SendDlgItemMessage(hDlg,DLG_THREAD_NAME,BM_SETCHECK,LocationDisplay,0);
			SendDlgItemMessage(hDlg,DLG_THREAD_ADDRESS,BM_SETCHECK,!LocationDisplay,0);

			// Fixed font for the listbox
            hFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	
			//Turn off fiber support in the dm
			//renable in CANCEL or OK
			if(runDebugParams.fFiberDebugging){
				ofbr.op = OFBR_DISABLE_FBRS;
				OSDSystemService (hpidCurr,
						  htidCurr,
						  ssvcFiberDebug,
						(LPVOID) &ofbr,
						sizeof(ofbr),
						NULL
						);
				OSDGetFrame(hpidCurr,htidCurr,1,&htid);		
			}
			if (hFont != NULL)
			{
				SendDlgItemMessage(hDlg,DLG_THREAD_LIST,WM_SETFONT,(WPARAM)hFont,MAKELONG(FALSE,0));

				// Reposition column headers based on size of fixed font,
				// so they line up nicely with the text in the listbox
				AlignColumnHeaders(hDlg, hFont);
			}

			// Initialize the linked list, and add the strings
			// in the listbox if the list is not empty
			InitList(DLG_THREAD_LIST);
			Count = (int)ListGetCount(DLG_THREAD_LIST);
			if (Count != 0)
			{
				EmptyList(DLG_THREAD_LIST);
			}
			// list empty, get thread list
			InitDefaultThreadList();
			Count = (int)ListGetCount(DLG_THREAD_LIST);
			UpdateListboxContent(hDlg);
			if (FocusThread != -1)
			{
				SendDlgItemMessage(hDlg, DLG_THREAD_LIST, LB_SETCURSEL,
					FocusThread, 0);
			}

			// Enable valid controls
			EnableControls(hDlg);
		}
		break;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case DLG_THREAD_NAME:
		case DLG_THREAD_ADDRESS:
			{
				UINT Selection;
				// Get current listbox item
				Selection = SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_GETCURSEL,0,0);
				// Get button state
				LocationDisplay = (BOOL)SendDlgItemMessage(hDlg,DLG_THREAD_NAME,BM_GETCHECK,0,0);
				// Refresh listbox screen
				UpdateListboxContent(hDlg);
				// Set current item
				if (Selection != LB_ERR)
					SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_SETCURSEL,Selection,0);
			}
			break;
		case DLG_THREAD_LIST:
			switch (GET_WM_COMMAND_CMD(wParam,lParam))
			{
				case LBN_SELCHANGE:
					// Enable valid controls
					EnableControls(hDlg);
					break;
				case LBN_DBLCLK:
					// Set focus to specified thread
					if (IsWindowEnabled(GetDlgItem(hDlg, DLG_THREAD_FOCUS)))
					{
						// Disable repainting of the listbox, since we're
						// about to zap it anyway
						SendDlgItemMessage(hDlg, DLG_THREAD_LIST,
							WM_SETREDRAW, FALSE, 0);

						// Set which thread has focus
						SendMessage(
							hDlg,
							WM_COMMAND,
							GET_WM_COMMAND_MPS(
								DLG_THREAD_FOCUS,
								GET_WM_COMMAND_HWND(wParam,lParam),
								0
							)
						);
						// exit
						return SendMessage(
							hDlg,
							WM_COMMAND,
							GET_WM_COMMAND_MPS(
								IDOK,
								GET_WM_COMMAND_HWND(wParam,lParam),
								0
							)
						);
					}
					break;
			}
			break;
		case DLG_THREAD_FOCUS:
			SetThreadState(hDlg,(UINT)SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_GETCURSEL,0,0),THREAD_FOCUS);
			break;
		case DLG_THREAD_SUSPEND:
			SetThreadState(hDlg,(UINT)SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_GETCURSEL,0,0),DLG_THREAD_SUSPEND);
			break;
		case DLG_THREAD_RESUME:
			SetThreadState(hDlg,(UINT)SendDlgItemMessage(hDlg,DLG_THREAD_LIST,LB_GETCURSEL,0,0),DLG_THREAD_RESUME);
			break;
		case IDCANCEL:
			//REENABLE Fiber support in the dm
			if(runDebugParams.fFiberDebugging){
				ofbr.op = OFBR_ENABLE_FBRS;
				OSDSystemService (hpidCurr,
						  htidCurr,
						  ssvcFiberDebug,
						(LPVOID) &ofbr,
						sizeof(ofbr),
						NULL
						);
				OSDGetFrame(hpidCurr,htidCurr,1,&htid);		
			}
			// Restore BOOL flag	
			LocationDisplay = OldLocationDisplay;
			// Don't save anything
			EmptyList(DLG_THREAD_LIST);
			EndDialog(hDlg, TRUE);
			return (TRUE);
		case IDOK:
			//REENABLE Fiber support in the dm
			if(runDebugParams.fFiberDebugging){
				ofbr.op = OFBR_ENABLE_FBRS;
				OSDSystemService (hpidCurr,
						  htidCurr,
						  ssvcFiberDebug,
						(LPVOID) &ofbr,
						sizeof(ofbr),
						NULL
						);
			//Unset the fiber context
				ofbr.op = OFBR_SET_FBRCNTX;
				ofbr.FbrCntx = NULL;
				OSDSystemService (hpidCurr,
						  htidCurr,
						  ssvcFiberDebug,
						  (LPVOID) &ofbr,
						sizeof(ofbr),
						NULL
						);
				OSDGetFrame(hpidCurr,htidCurr,1,&htid);		
			}
			// Make all the changes valid
			ValidateNewThreadStates();
			// HACK: There seems to be an NT bug where it sometimes does
			// not send a WM_SETFOCUS to the child MDI window.  I think
			// this is related to all the SetFocus junk we're doing in
			// UpdateDebuggerState.  Anyway, we can work around the problem
			// by setting focus to the dialog right before it goes away.

			SetFocus(hDlg);
			EmptyList(DLG_THREAD_LIST);
			EndDialog(hDlg, TRUE);
			
			//Some windows don't update on switch to same thread from fiber
			//So force everything
			UpdateDebuggerState(UPDATE_DEBUGGEE|UPDATE_ALLDBGWIN|UPDATE_SOURCE);
			return (TRUE);
		}
		break;
	}

	return( DefaultDialogHandler(hDlg, message, wParam, lParam) );
}
