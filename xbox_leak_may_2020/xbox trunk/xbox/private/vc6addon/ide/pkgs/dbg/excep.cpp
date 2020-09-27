#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static BOOL fExceptionEdited;	// any edits in current exception fields?
static int SelCount;
static int FAR *lpiSel;

BOOL IsForCompatiblePlatform(UINT idPlatform)
{
	BOOL fRet = FALSE;
	UINT idCurPlatform = pDebugCurr->GetPlatform( );
	
	if ( idPlatform == idCurPlatform || idPlatform == iPlatformGeneric)
	{
		fRet = TRUE;
	}
	else
	{
		// We assume that all Win32 platforms are compatible.
		// as far as exceptions go.
		switch ( idPlatform )
		{
			case win32x86:
			case win32mips:
			case win32alpha:
			case win32ppc:
				switch( idCurPlatform )
				{
					case win32x86:
					case win32mips:
					case win32alpha:
					case win32ppc:
						fRet = TRUE;
						break;
					default:
						break;
				}
			default:
				break;
		}
	}
	
	return fRet;
}
										
			
/****************************************************************************

	FUNCTION:	MakeExceptionObject

	PURPOSE:	Initialize an Exception_Object Structure

****************************************************************************/
void MakeExceptionObject(EXCEPTION_OBJECT *pObject, DWORD dwExceptionCode,
	LPCSTR Name, int iAction, UINT iPlatformId)
{
	pObject->dwExceptionCode = dwExceptionCode;
	_ftcsncpy(pObject->Name,Name,cchExcepName);
	pObject->Name[cchExcepName-1] = '\0';
	pObject->iAction = iAction;
	pObject->iPlatformId = iPlatformId;
}

/****************************************************************************

	FUNCTION:	UnmakeExceptionObject

	PURPOSE:	Get fields of an exception_object struct

****************************************************************************/
void UnmakeExceptionObject(EXCEPTION_OBJECT *pObject, DWORD *pdwExceptionCode,
	LPSTR Name, int *piAction, UINT *piPlatformId)
{
	*pdwExceptionCode = pObject->dwExceptionCode;
	_ftcsncpy(Name,pObject->Name,cchExcepName);
	Name[cchExcepName-1] = '\0';
	*piAction = pObject->iAction;
	*piPlatformId = pObject->iPlatformId;
}

/****************************************************************************

	FUNCTION:	MakeExceptionString

	PURPOSE:	Create a user string from an EXCEPTION_OBJECT struct

****************************************************************************/
void MakeExceptionString(EXCEPTION_OBJECT *pObject, LPSTR Buffer, int cch)
{
	CString	strAction;
	char	Number[cchExcepNumber];
#ifndef _WIN32
	char	LocalBuffer[cchExcepAll];
#endif

	VERIFY(strAction.LoadString(DLG_Exception_Action_First + pObject->iAction));

	sprintf(Number, "%08lx", pObject->dwExceptionCode);

#ifndef _WIN32
	sprintf(LocalBuffer, "%.*s\t%.*s\t%.*s",
		cchExcepNumber-1,	Number,
		cchExcepName-1,		pObject->Name,
		cchExcepAction-1,	(const char *) strAction);
	_ftcscpy(Buffer, LocalBuffer);
#else
	sprintf(Buffer, "%.*s\t%.*s\t%.*s",
		cchExcepNumber-1,	Number,
		cchExcepName-1,		pObject->Name,
		cchExcepAction-1,	(const char *) strAction);
#endif
}

/****************************************************************************

	FUNCTION:	ReInitSystemExceptions

	PURPOSE:	Re-initialize the system exceptions in the list.

****************************************************************************/
VOID ReInitSystemExceptions(BOOL fInsertOnly)
{
	EXCEPTION_OBJECT	ObjSys, ObjCur;
	DWORD	iobjSys, iobjCur;
	DWORD	cobjSys, cobjCur;
	int		cmp;

	cobjSys = ListGetCount(DLG_SYSTEM_EXCEP_LIST);
	cobjCur = ListGetCount(DLG_EXCEP_LIST);

	iobjCur = 0;

	for (iobjSys=0; iobjSys<cobjSys; ++iobjSys)
	{
		// Get the system exception.
		if (!ListGetObject(iobjSys, DLG_SYSTEM_EXCEP_LIST, (LPSTR)&ObjSys))
		{
			ASSERT(FALSE);
			break;
		}

		// See if it's in the current list of exceptions.  Both lists are
		// sorted, so we know that no matter what the current value of iobjCur,
		// the system object we just found is not before iobjCur in the list.
		cmp = -1;
		while (iobjCur < cobjCur && cmp < 0)
		{
			VERIFY(ListGetObject(iobjCur, DLG_EXCEP_LIST, (LPSTR)&ObjCur));

			if (ObjCur.dwExceptionCode < ObjSys.dwExceptionCode)
				cmp = -1;
			else if (ObjCur.dwExceptionCode == ObjSys.dwExceptionCode)
				cmp = 0;
			else
				cmp = 1;

			// if cmp < 0, then we haven't yet reached the point in the list
			// where this element should go
			if (cmp < 0)
			{
				iobjCur++;
			}
			else
			{
				break;
			}
		}
		// if cmp == 0, we've found a match
		if (cmp == 0)
		{
			if (!fInsertOnly)
			{
				ListUpdateObject(iobjCur, DLG_EXCEP_LIST, (LPSTR)&ObjSys,
					sizeof(ObjSys));
			}
		}
		// if cmp != 0, we have to insert the element into the list here
		else
		{
			ListInsertObject(iobjCur, DLG_EXCEP_LIST, (LPSTR)&ObjSys,
				sizeof(ObjSys));
			cobjCur++;
		}
		iobjCur++;
	}
}


/****************************************************************************

	FUNCTION:	GetExceptionInfo

	PURPOSE:	Ask OSDebug for informations about an exception.
				'pdwExceptionCode' and 'Name' are optional args (can be NULL).
				'fSystemOnly' indicates whether caller is only interested
				in system exceptions.

****************************************************************************/
BOOL GetExceptionInfo(BOOL FirstException, BOOL fSystemOnly,
	DWORD *pdwExceptionCode, LPSTR Name, UINT cchName, UINT *piAction, UINT *piPlatformId)
{
static EXCEPTION_DESCRIPTION ExceptionDesc;
#if !(defined (OSDEBUG4))
static EXR ExceptionCode;
#else
EXCEPTION_CONTROL exf;
#endif
HTID htid;
HPID hpid;

	// Get pid of current process
	hpid = HpidFromHprc(hprcCurr);

	// Get tid of current process
	htid = HtidFromHthd(hthdCurr);

#if !(defined (OSDEBUG4))
	// Get next exception number
	do
	{
		if (OSDGetNextExRecord (hpid, htid, &ExceptionCode, FirstException)
			!= xosdNone)
		{
			return (FALSE);
		}
		FirstException = FALSE;
	// if !fSystemOnly, stop on the first exception;
	// if fSystemOnly, stop on the first known exception
	}
	while (fSystemOnly && !ExceptionCode.fKnown);

	// Store exception number
	if (pdwExceptionCode)
		*pdwExceptionCode = ExceptionCode.dwExceptionCode;

	// Setup EXCEPTION_DESC struct to call GetExceptionDescription
	ExceptionDesc.dwExceptionCode = ExceptionCode.dwExceptionCode;
	OSDGetExDescription(hpid, htid, (LPEXCEPTION_DESCRIPTION)&ExceptionDesc);
#else
	if (FirstException) {
	    exf = exfFirst;
	    FirstException = FALSE;
	}
	else {
	    exf = exfNext;
	}
        if (OSDGetExceptionState( hpid, htid, &ExceptionDesc, exf) != xosdNone) {
		return (FALSE);
	}
	// Store exception number
	if (pdwExceptionCode)
		*pdwExceptionCode = ExceptionDesc.dwExceptionCode;
#endif
	// Store the platform id
	if ( piPlatformId )
	{
		ASSERT( pDebugCurr );
		*piPlatformId = pDebugCurr->GetPlatform();
	}

	// Get exception description
	if (Name)
	{
#if !(defined (OSDEBUG4))
		_ftcsncpy(Name,ExceptionDesc.lszDescription,cchName);
#else
		_ftcsncpy(Name,ExceptionDesc.rgchDescription,cchName);
#endif
		Name[cchName-1] = '\0';
	}

	// Get Exception action
	switch(ExceptionDesc.efd)
   	{
	case efdIgnore:
	case efdNotify:
		*piAction = EX_ACTION_2NDCHANCE;
		break;
	case efdStop:
		*piAction = EX_ACTION_1STCHANCE;
		break;
	default:
		ASSERT(FALSE);
	}

	return (TRUE);
}
/****************************************************************************

	FUNCTION:	InitDefaultExceptionList

	PURPOSE:	Get default exception list

	INPUT:		listId = DLG_EXCEP_LIST or DLG_SYSTEM_EXCEP_LIST

****************************************************************************/
static void InitDefaultExceptionList(int listId)
{
EXCEPTION_OBJECT Object;
DWORD dwExceptionCode;
char Name[cchExcepName];
int iAction;
BOOL IsFirstException = TRUE;
UINT iPlatformId;
	EmptyList(listId);

	while (GetExceptionInfo(IsFirstException, (listId == DLG_SYSTEM_EXCEP_LIST),
		&dwExceptionCode, Name, sizeof(Name), (UINT *)&iAction, &iPlatformId))
	{
		MakeExceptionObject(&Object, dwExceptionCode, Name, iAction, iPlatformId);

		// Add exception in the linked list
		ListAddObject(listId, (LPSTR)&Object, sizeof(EXCEPTION_OBJECT));

		// We want the next exceptions from GetExceptionInfo
		IsFirstException = FALSE;
	}

	if (listId == DLG_EXCEP_LIST)
		fExcepListInitialized = TRUE;
}

/****************************************************************************

	FUNCTION:	InitProcessExceptions

	PURPOSE:	Initialize the exceptions for a process.

****************************************************************************/
VOID InitProcessExceptions(VOID)
{
	UINT				iexception;
	EXCEPTION_OBJECT	Object;

	// save the list of system exceptions
	InitList(DLG_SYSTEM_EXCEP_LIST);
	InitDefaultExceptionList(DLG_SYSTEM_EXCEP_LIST);

	// If list empty, get default
	InitList(DLG_EXCEP_LIST);
	if (!fExcepListInitialized)
		InitDefaultExceptionList(DLG_EXCEP_LIST);

	// Update the exception list with the system (OSDebug) exception
	// list.  This may have changed if the platform did.
	ReInitSystemExceptions( TRUE );

	// set up default actions for all exceptions
	for (iexception=0;
		 ListGetObject(iexception, DLG_EXCEP_LIST, (LPSTR)&Object);
		 iexception++)
	{
		if (IsForCompatiblePlatform(Object.iPlatformId))
		{
#if !(defined (OSDEBUG4))
			OSDHandleEx(hpidCurr, htidCurr, Object.dwExceptionCode,
				(Object.iAction == EX_ACTION_1STCHANCE) ? efdStop : efdNotify);
#else
		    EXCEPTION_DESCRIPTION ExceptionDesc = { 0 };

			ExceptionDesc.dwExceptionCode = Object.dwExceptionCode;
			ExceptionDesc.efd = (Object.iAction == EX_ACTION_1STCHANCE)
				? efdStop : efdNotify;
			VERIFY(OSDSetExceptionState( hpidCurr, htidCurr, &ExceptionDesc)
				== xosdNone);
#endif
		}
	}
}

/****************************************************************************

	FUNCTION:	ValidateExceptionStatus

	PURPOSE:	Update the exception actions (ie return, ignore, etc...)

****************************************************************************/
static void ValidateExceptionStatus(void)
{
	int							Count,i;
	DWORD						dwExceptionCode;
	char						Name[cchExcepName];
	int							action;
	EXCEPTION_OBJECT			Object;
	EXCEPTION_FILTER_DEFAULT	efd;
	HTID						htid;
	HPID						hpid;
	BOOL						fFirstException;
	UINT						iPlatformId;

	// Get pid of current process
	hpid = HpidFromHprc(hprcCurr);

	// Get tid of current process
	htid = HtidFromHthd(hthdCurr);

	// If there are any exceptions that have been removed from the list,
	// restore their action to "stop if not handled" (the default).
	fFirstException = TRUE;
	while (GetExceptionInfo(fFirstException, FALSE,
		&dwExceptionCode, Name, sizeof(Name), (UINT *)&action, &iPlatformId))
	{
		BOOL	fFound = FALSE;

		for (i=0; ListGetObject(i, DLG_EXCEP_LIST, (LPSTR)&Object); ++i)
		{
			if (dwExceptionCode == Object.dwExceptionCode)
			{
				fFound = TRUE;
				break;
			}
		}

		if (!fFound) {
#if !(defined (OSDEBUG4))
			OSDHandleEx(hpid, htid, dwExceptionCode, efdNotify);
#else
		    EXCEPTION_DESCRIPTION ExceptionDesc = { 0 };
			ExceptionDesc.dwExceptionCode = dwExceptionCode;
			ExceptionDesc.efd = efdNotify;
			VERIFY(OSDSetExceptionState( hpid, htid, &ExceptionDesc)
				== xosdNone);
#endif
		}

		fFirstException = FALSE;
	}

	// Update exception actions for all exceptions in list
	Count = (int)ListGetCount(DLG_EXCEP_LIST);
	for (i=0; i<Count; i++)
	{
		if (!ListGetObject(i,DLG_EXCEP_LIST,(LPSTR)&Object))
		{
			ASSERT(FALSE);
			break;
		}

		switch(Object.iAction)
		{
		case EX_ACTION_1STCHANCE:
			efd = efdStop;
			break;
		case EX_ACTION_2NDCHANCE:
			efd = efdNotify;
			break;
		default:
			efd = efdStop;
			break;
		}

#if !(defined (OSDEBUG4))
		OSDHandleEx ( hpid, htid, Object.dwExceptionCode, efd );
#else
		{
		    EXCEPTION_DESCRIPTION ExceptionDesc = { 0 };
			ExceptionDesc.dwExceptionCode = Object.dwExceptionCode;
			ExceptionDesc.efd = efd;
			VERIFY(OSDSetExceptionState( hpid, htid, &ExceptionDesc)
				== xosdNone);
		}
#endif
	}
}

/****************************************************************************

	FUNCTION:	GetExceptionDescription

	PURPOSE:	Get the user exception description, for a given exception#
				If the exception is not in our list, get the standard osd
				exception description

****************************************************************************/
void GetExceptionDescription(DWORD dwExceptionCode, CString& strDescription)
{
	int						Count,i;
	EXCEPTION_DESCRIPTION	ExceptionDesc;
	EXCEPTION_OBJECT		Object;
	UINT                    idCurPlatform = pDebugCurr->GetPlatform( );

	// Get # of exception in our list
	Count = (int)ListGetCount(DLG_EXCEP_LIST);

	if (Count != 0)
	{
		// Our list isn't empty, so get our own description
		for (i=0; i<Count; i++)
		{
			if (!ListGetObject(i,DLG_EXCEP_LIST,(LPSTR)&Object))
			{
				ASSERT(FALSE);
				strDescription = "";
				return;
			}
			if ((dwExceptionCode == Object.dwExceptionCode) &&
                (idCurPlatform == Object.iPlatformId))
				break;
		}

		if (i != Count)
		{
			// We've found the exception. Get the name
			strDescription = Object.Name;
			return;
		}
		else
		{
			// Not found in our list
			// So, let osd handle that.
		}
	}

	// If list empty, or our list doesn't know the specified exception code
	// get default osd exception description

	// Setup EXCEPTION_DESCRIPTION struct to call GetExceptionDescription
	ExceptionDesc.dwExceptionCode = dwExceptionCode;
#if !(defined (OSDEBUG4))
	OSDGetExDescription(hpidCurr, htidCurr, (LPEXCEPTION_DESCRIPTION)&ExceptionDesc);
	// Get the name
	strDescription = ExceptionDesc.lszDescription;
#else
	OSDGetExceptionState(hpidCurr, htidCurr, &ExceptionDesc, exfSpecified);
	// Get the name
	strDescription = ExceptionDesc.rgchDescription;
#endif
}

/****************************************************************************

	FUNCTION:	GetExceptionAction

	PURPOSE:	Get the user exception action, for a given exception #.
				If the exception is not in our list, return
				EX_ACTION_2NDCHANCE (only stop on 2nd-chance)

****************************************************************************/
int GetExceptionAction(DWORD dwExceptionCode)
{
	int	Count,i;
	EXCEPTION_OBJECT		Object;

	// Get # of exception in our list
	Count = (int)ListGetCount(DLG_EXCEP_LIST);

	if (Count != 0)
	{
		// Our list isn't empty, so get our own description
		for (i=0; i<Count; i++)
		{
			if (!ListGetObject(i,DLG_EXCEP_LIST,(LPSTR)&Object))
			{
				ASSERT(FALSE);
				return EX_ACTION_2NDCHANCE;
			}
			if (dwExceptionCode == Object.dwExceptionCode)
				break;
		}

		if (i != Count)
		{
			// We've found the exception. Get the action
			return Object.iAction;
		}
		else
		{
			// Not found in our list
		}
	}

	// If list empty, or our list doesn't know the specified exception code
	return EX_ACTION_2NDCHANCE;
}

/****************************************************************************

	FUNCTION:	UpdateListboxContent

	PURPOSE:	Copy the content of the linked list into the listbox

****************************************************************************/
static void UpdateListboxContent(HWND hDlg)
{
int Count,i,j = 0;
char Buffer[cchExcepAll];
EXCEPTION_OBJECT Object;

	SendDlgItemMessage(hDlg,DLG_EXCEP_LIST,LB_RESETCONTENT,0,0);
	Count = (int)ListGetCount(DLG_EXCEP_LIST);
	for (i=0; i<Count; i++)
	{
		if (!ListGetObject(i,DLG_EXCEP_LIST,(LPSTR)&Object))
		{
			ASSERT(FALSE);
			EmptyList(DLG_EXCEP_LIST);
			SendDlgItemMessage(hDlg,DLG_EXCEP_LIST,LB_RESETCONTENT,0,0);
			break;
		}
		if (IsForCompatiblePlatform(Object.iPlatformId))
		{
			MakeExceptionString(&Object,Buffer,sizeof(Buffer));
			SendDlgItemMessage(hDlg,DLG_EXCEP_LIST,LB_INSERTSTRING,j,(LPARAM)(LPSTR)Buffer);
			SendDlgItemMessage(hDlg,DLG_EXCEP_LIST,LB_SETITEMDATA,j,(LPARAM)Object.iAction);
			j++;
		}
	}
}

static int GetNthEnabledObject(DWORD iSel, int iList, EXCEPTION_OBJECT * pObject)
{
	// State:
	//	-1 = Item not found in specified list
	//  all other values = actual index in list of object
	int		iRet = -1;
	DWORD	iSelCur = 0;

	// Increment iSel so iSel == 0 can be used as a termination condition
	++iSel;

	while( iSel && ListGetObject( iSelCur, iList, (LPSTR)pObject ))
	{
		// Matching platform, save current index
		if ( IsForCompatiblePlatform( pObject->iPlatformId ) )
		{
			--iSel;
			iRet = iSelCur;
		}
		++iSelCur;
	}

	// If iSel != 0, then the end of list was found before finding the nth
	// matching entry, return failure
	return iSel ? -1 : iRet;
}

static int GetAction (HWND hDlg)
{
	// Returns state:
	//	-1 = neither radiobutton selected
	//	 0 = 1st chance selected
	//	 1 = last chance selected
	if (SendDlgItemMessage(hDlg,DLG_EXCEP_1STCHANCE,BM_GETCHECK,0,0))
		return 0;
	else if (SendDlgItemMessage(hDlg,DLG_EXCEP_2NDCHANCE,BM_GETCHECK,0,0))
		return 1;
	else
		return -1;
}

static void SetAction (HWND hDlg, int State)
{
	// State:
	//	-1 = neither radiobutton selected
	//	 0 = 1st chance selected
	//	 1 = last chance selected
	SendDlgItemMessage(hDlg,DLG_EXCEP_1STCHANCE,BM_SETCHECK,(State == 0),0);
	SendDlgItemMessage(hDlg,DLG_EXCEP_2NDCHANCE,BM_SETCHECK,(State == 1),0);
}

/****************************************************************************

	FUNCTION:	EnableControls

	PURPOSE:	Enable or disable some buttons in the exception box

****************************************************************************/
static void EnableControls(HWND hDlg)
{
BOOL EnableAdd,EnableChange;
BOOL EnableNumber,EnableName;
char szNumber[cchExcepNumber];
int Length;
char *pchEnd;
DWORD dwExceptionCode;
UINT idDefault;

	// If EXCEPTION NUMBER field empty, disable ADD and CHANGE
	// Else, enable either ADD or CHANGE if the number exists
	EnableAdd = EnableChange = FALSE;
	EnableNumber = EnableName = TRUE;
	if (SelCount > 1)
	{
		// Enable Change as long as one of "Stop always" and "Stop if not
		// handled" is selected
		EnableChange = (GetAction(hDlg) != -1);
		EnableNumber = EnableName = FALSE;
	}
	else if ((Length = (int)SendDlgItemMessage(hDlg,DLG_EXCEP_NUMBER,WM_GETTEXTLENGTH,0,0)) != 0)
	{
		// If Number is not empty, get its text
		SendDlgItemMessage(hDlg, DLG_EXCEP_NUMBER, WM_GETTEXT,
			sizeof(szNumber), (LPARAM)(LPSTR)szNumber);
		// Convert it to 8 digits
		dwExceptionCode = strtoul(szNumber, &pchEnd, 16);
		if (*pchEnd == '\0')	// don't allow non-hex characters
		{
			sprintf(szNumber, "%08lx", dwExceptionCode);
			// Search the listbox for a same number
			if ((int)SendDlgItemMessage(hDlg,DLG_EXCEP_LIST,
				LB_FINDSTRING,(WPARAM)-1,(LPARAM)(LPSTR)szNumber) != LB_ERR)
			{
				EnableChange = TRUE;
			}
			else
			{
				EnableAdd = TRUE;
			}
		}
	}

	EnableWindow(GetDlgItem(hDlg, DLG_EXCEP_ADD),EnableAdd);
	EnableWindow(GetDlgItem(hDlg, DLG_EXCEP_CHANGE),EnableChange);
	EnableWindow(GetDlgItem(hDlg, DLG_EXCEP_NUMBER_LABEL),EnableNumber);
	EnableWindow(GetDlgItem(hDlg, DLG_EXCEP_NUMBER),EnableNumber);
	EnableWindow(GetDlgItem(hDlg, DLG_EXCEP_NAME_LABEL),EnableName);
	EnableWindow(GetDlgItem(hDlg, DLG_EXCEP_NAME),EnableName);

	// Delete button is enabled if any lines of the listbox are selected
	EnableWindow(GetDlgItem(hDlg, DLG_EXCEP_DELETE), SelCount > 0);

	// Set which button is default: Close, Add, or Change
	idDefault = IDOK;
	if (fExceptionEdited)
	{
		if (EnableAdd)
			idDefault = DLG_EXCEP_ADD;
		else if (EnableChange)
			idDefault = DLG_EXCEP_CHANGE;
	}

	SendMessage(hDlg, DM_SETDEFID, idDefault, 0);
}

/****************************************************************************

	FUNCTION:	DrawExceptionListItem

	PURPOSE:	Callback routine to HandleStandardOwnerDrawListBox to
				actually output the text

****************************************************************************/
VOID FAR PASCAL DrawExceptionListItem(
	HWND hDlg,
	LPDRAWITEMSTRUCT lpDIS)
{
	char Buffer[cchExcepAll];
	LPSTR CurStartPos, CurEndPos;
	POINT pt1, pt2;
	RECT rcDraw;

	// Get the exception text.  The fields are separated by the
	// tab character
	VERIFY(SendMessage(lpDIS->hwndItem, LB_GETTEXT, lpDIS->itemID, (LONG)(LPSTR)Buffer) != LB_ERR);

	// So, we draw the exception fields flush with entry fields
	// that appear above the list box.

	// Exception number
	rcDraw = lpDIS->rcItem;
	CurStartPos = Buffer;
	CurEndPos = _ftcschr(CurStartPos, _T('\t'));
	ASSERT(CurEndPos != NULL);
	*CurEndPos = '\0';
	pt1.x = 0; pt1.y = 0;
	ClientToScreen(GetDlgItem(hDlg, DLG_EXCEP_LIST), &pt1);
	pt2.x = 0; pt2.y = 0;
	ClientToScreen(GetDlgItem(hDlg, DLG_EXCEP_NAME), &pt2);
	rcDraw.right = rcDraw.left + pt2.x - pt1.x;
	ExtTextOut(lpDIS->hDC, rcDraw.left+2, rcDraw.top, ETO_CLIPPED, &rcDraw,
		CurStartPos, _ftcslen(CurStartPos), NULL);

	// Exception number
	CurStartPos = CurEndPos+1;
	CurEndPos = _ftcschr(CurStartPos, _T('\t'));
	ASSERT(CurEndPos != NULL);
	*CurEndPos = '\0';
	pt1.x = 0; pt1.y = 0;
	ClientToScreen(GetDlgItem(hDlg, DLG_EXCEP_ACTION), &pt1);
	rcDraw.left = rcDraw.right + 1;
	rcDraw.right = rcDraw.left + pt1.x - pt2.x;
	ExtTextOut(lpDIS->hDC, rcDraw.left+2, rcDraw.top, ETO_CLIPPED, &rcDraw,
		CurStartPos, _ftcslen(CurStartPos), NULL);

	// Exception action
	CurStartPos = CurEndPos+1;
	rcDraw.left = rcDraw.right + 1;
	rcDraw.right = lpDIS->rcItem.right;
	ExtTextOut(lpDIS->hDC, rcDraw.left+2, rcDraw.top, ETO_CLIPPED, &rcDraw,
		CurStartPos, _ftcslen(CurStartPos), NULL);
}

/****************************************************************************

	FUNCTION:	AddChangeException(hDlg)

	PURPOSE:	Adds or changes an exception.

****************************************************************************/
static VOID AddChangeException(HWND hDlg)
{
	HWND				hwndList = GetDlgItem(hDlg, DLG_EXCEP_LIST);
	EXCEPTION_OBJECT	Object;
	char				Buffer[cchExcepAll];
	char				Number[cchExcepNumber];
	char				Name[cchExcepName];
	int					Line,NewLine,iAction;
	DWORD				dwExceptionCode;
	char *pchEnd;
	int					i;
	BOOL				fSelected;
	UINT				iPlatformId;

	// After add/change, there are no changes pending
	fExceptionEdited = FALSE;

	iAction = GetAction(hDlg);
	ASSERT(iAction != -1);

	// Are multiple lines selected?
	if (SelCount <= 1)
	{
		// Get text from controls
		GetWindowText(GetDlgItem(hDlg,DLG_EXCEP_NUMBER),(LPSTR)Number,sizeof(Number));
		GetWindowText(GetDlgItem(hDlg,DLG_EXCEP_NAME),(LPSTR)Name,sizeof(Name));

		// Convert number to 8 digits
		dwExceptionCode = strtoul(Number, &pchEnd, 16);
		if (pchEnd == Number || *pchEnd != '\0')
			return;
		sprintf(Number, "%08lx", dwExceptionCode);

		// Search the listbox for same number
		Line = (int)SendMessage(hwndList, LB_FINDSTRING, (WPARAM)-1,
			(LPARAM)(LPSTR)Number);
		// If same number already exists, delete the current line
		// else insert it sorted into the list
		if (Line != LB_ERR)
		{
			SendMessage(hwndList, LB_DELETESTRING, Line, 0);
			iPlatformId = pDebugCurr->GetPlatform();
		}
		else {
			iPlatformId = iPlatformGeneric;
		}

		MakeExceptionObject(&Object, dwExceptionCode, Name, iAction, iPlatformId);
		MakeExceptionString(&Object,Buffer,sizeof(Buffer));

		// Create the string, and add it in the sorted listbox
		NewLine = SendMessage(hwndList, LB_ADDSTRING, 0,
			(LPARAM)(LPSTR)Buffer);

		// Put the iAction combo index to the data field of the listbox line
		SendMessage(hwndList, LB_SETITEMDATA, NewLine, iAction);

		// Add or update exception in the linked list
		if (Line == LB_ERR)
		{
			DWORD				iItem = 0;
			EXCEPTION_OBJECT	ObjectT;

			// Find the real place in the list for this exception to go
			while(ListGetObject(iItem,DLG_EXCEP_LIST,(LPSTR)&ObjectT))
			{
				if ( ObjectT.dwExceptionCode > dwExceptionCode )
				{
					break;
				}
				++iItem;
			}

			ListInsertObject(iItem,DLG_EXCEP_LIST,(LPSTR)&Object,sizeof(EXCEPTION_OBJECT));
		}
		else
		{
			EXCEPTION_OBJECT	ObjectT;
			DWORD				iObject;

			// Get the nth enabled object and its REAL index
			iObject = GetNthEnabledObject(Line,DLG_EXCEP_LIST,&ObjectT);
			ASSERT( iObject != -1 );

			ListUpdateObject(iObject,DLG_EXCEP_LIST,(LPSTR)&Object,sizeof(EXCEPTION_OBJECT));
		}
	}
	else /* multiple lines selected */
	{
		for (i=0; i<SelCount; ++i)
		{
			DWORD	iObject;

			// Get the nth enabled object and its REAL index
			iObject = GetNthEnabledObject(lpiSel[i],DLG_EXCEP_LIST,&Object);
			ASSERT( iObject != -1 );

			Object.iAction = iAction;
			ListUpdateObject(iObject,DLG_EXCEP_LIST,(LPSTR)&Object,
				sizeof(EXCEPTION_OBJECT));

			MakeExceptionString(&Object, Buffer, sizeof(Buffer));
			fSelected = SendMessage(hwndList, LB_GETSEL, lpiSel[i], 0);
			SendMessage(hwndList, LB_DELETESTRING, lpiSel[i], 0);
			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)(LPSTR)Buffer);
			if (fSelected)
				SendMessage(hwndList, LB_SETSEL, TRUE, lpiSel[i]);
		}
	}

	// Set buttons
	EnableControls(hDlg);
}

/****************************************************************************

	FUNCTION:	DlgException(HWND, unsigned, UINT, LONG)

	PURPOSE:	Processes messages for "EXCEPTION" dialog box

****************************************************************************/
BOOL FAR PASCAL EXPORT DlgException(
	HWND       hDlg,
	unsigned   message,
	UINT       wParam,
	LONG       lParam)
{
	static BOOL	fSelChange = FALSE;
	UINT		id;
	UINT		cmd;
	EXCEPTION_OBJECT	Object;

	switch (message)
	{
	case WM_CONTEXTMENU:
	{
		DoHelpContextMenu(CWnd::FromHandle(hDlg), IDD_EXCEPTIONS, CPoint(LOWORD(lParam),HIWORD(lParam)));
	}
	return TRUE;

	case WM_HELP:
	{
		DoHelpInfo(CWnd::FromHandle(hDlg), IDD_EXCEPTIONS, (HELPINFO *)lParam);
	}
	return TRUE;

	case WM_INITDIALOG:
		{
			// No edits have been made to the current exception
			fExceptionEdited = FALSE;

			lpiSel = NULL;
			SelCount = 0;

			SetAction(hDlg,1);

			// Enable valid controls
			EnableControls(hDlg);

			// Set field length
			SendDlgItemMessage(hDlg,DLG_EXCEP_NUMBER,EM_LIMITTEXT,cchExcepNumber-1,0);
			SendDlgItemMessage(hDlg,DLG_EXCEP_NAME,EM_LIMITTEXT,cchExcepName-1,0);

			// Initialize the linked list, and add the strings
			// in the listbox if the list is not empty
			InitList(DLG_EXCEP_LIST);
			InitList(DLG_EXCEP_LIST_BACKUP);
			ListCopy(DLG_EXCEP_LIST_BACKUP, DLG_EXCEP_LIST, (LPSTR)&Object, sizeof(Object));
			if (!fExcepListInitialized)
				InitDefaultExceptionList(DLG_EXCEP_LIST);
			UpdateListboxContent(hDlg);

            return( TRUE );
		}
		break;
	case WM_COMMAND:
		id = GET_WM_COMMAND_ID(wParam, lParam);
		cmd = GET_WM_COMMAND_CMD(wParam, lParam);
		switch (id)
		{
		case DLG_EXCEP_NUMBER:
			if (cmd == EN_CHANGE)
				EnableControls(hDlg);
			break;
		case DLG_EXCEP_NAME:
			if (cmd == EN_CHANGE && !fExceptionEdited && !fSelChange)
			{
				fExceptionEdited = TRUE;
				EnableControls(hDlg);
			}
			break;
		case DLG_EXCEP_1STCHANCE:
		case DLG_EXCEP_2NDCHANCE:
			if ((cmd == BN_CLICKED || cmd == BN_DOUBLECLICKED)
				&& !fExceptionEdited)
			{
				fExceptionEdited = TRUE;
				EnableControls(hDlg);
			}
			break;
		case DLG_EXCEP_ADD:
		case DLG_EXCEP_CHANGE:
			AddChangeException(hDlg);
			if (id == DLG_EXCEP_ADD)
			{
				SetFocus(GetDlgItem(hDlg, DLG_EXCEP_NUMBER));
				SendDlgItemMessage(hDlg, DLG_EXCEP_NUMBER, EM_SETSEL,
					GET_EM_SETSEL_MPS(0, -1));
			}
			break;
		case DLG_EXCEP_DELETE:
			{
				int Caret;
				int i;
				int Count;

				// Delete strings if any are highlighted
				if (SelCount == 0)
				{
					MessageBeep(0);
					break;
				}
				// Get current caret
				Caret = SendDlgItemMessage(hDlg,DLG_EXCEP_LIST,
										   LB_GETCARETINDEX,0,0);
				// Delete current items
				for (i=SelCount-1; i>=0; --i)
				{
					DWORD	iObject;

					SendDlgItemMessage(hDlg,DLG_EXCEP_LIST,LB_DELETESTRING,
									   lpiSel[i],0);

					iObject = GetNthEnabledObject(lpiSel[i], DLG_EXCEP_LIST, &Object);
					ASSERT( iObject != -1 );

					ListDeleteObject(iObject, DLG_EXCEP_LIST);
					if (lpiSel[i] < Caret)
						Caret--;
				}

				SelCount = 0;
				_ffree(lpiSel);
				lpiSel = NULL;

				// Set listbox selection

				Count = SendDlgItemMessage(hDlg, DLG_EXCEP_LIST, LB_GETCOUNT,
					0, 0);

				if (Caret >= Count)
					Caret = Count-1;
				if (Caret < 0)
					Caret = 0;

				SendDlgItemMessage(hDlg, DLG_EXCEP_LIST,
					LB_SETCARETINDEX, Caret, FALSE);
				SendDlgItemMessage(hDlg, DLG_EXCEP_LIST,
					LB_SETSEL, TRUE, Caret);
				SetFocus(GetDlgItem(hDlg, DLG_EXCEP_LIST));

				fExceptionEdited = FALSE;

				// update edit fields & action buttons
				SendMessage(hDlg, WM_COMMAND,
					GET_WM_COMMAND_MPS(DLG_EXCEP_LIST, 0, LBN_SELCHANGE));
			}
			break;
		case DLG_EXCEP_LIST:
			if (cmd == LBN_SELCHANGE)
			{
				int iAction;
				DWORD dwExceptionCode;
				UINT iPlatformId;
				char Number[cchExcepNumber];
				char Name[cchExcepName];
				EXCEPTION_OBJECT Object;
				int i;
				int FAR *lpiSelNew;
				int selCountNew;

				// Get new list of selected items
				selCountNew = SendDlgItemMessage(hDlg, DLG_EXCEP_LIST,
					LB_GETSELCOUNT, 0, 0);
				lpiSelNew = (int *)malloc(selCountNew * sizeof(int));

				if (!lpiSelNew)
					break;
				// [dolphin #12252 5/18/94 mikemo]  Chicago barfs if we
				// call LB_GETSELITEMS with a count of zero items, so
				// if selCountNew is zero, don't call it
				if (selCountNew > 0)
				{
					SendDlgItemMessage(hDlg, DLG_EXCEP_LIST, LB_GETSELITEMS,
						selCountNew, (LPARAM)(LPSTR)lpiSelNew);
				}

				if (fExceptionEdited)
				{
					// If Change is enabled, it will happen automatically
					// when the user selects another line in the listbox.
					// But if Add is enabled, we don't add, we just beep.
					if (IsWindowEnabled(GetDlgItem(hDlg, DLG_EXCEP_ADD)))
						MessageBeep(0);
					else
					{
						AddChangeException(hDlg);

						// The process of changing exceptions may have caused
						// some of them to be de-selected.  We want to restore
						// the selection to what it was when we began this
						// function.  We don't need to clear the list, because
						// changing exceptions won't have caused any unwanted
						// exceptions to be added.
						for (i=0; i<selCountNew; ++i)
						{
							SendDlgItemMessage(hDlg, DLG_EXCEP_LIST, LB_SETSEL,
								TRUE, lpiSelNew[i]);
						}
					}

					fExceptionEdited = FALSE;
				}

				// Get new list of selected items
				SelCount = selCountNew;
				if (lpiSel)
					_ffree(lpiSel);
				lpiSel = lpiSelNew;

				// Only one item selected?
				if (SelCount == 1)
				{
					// Get object and decode it
					if (GetNthEnabledObject(lpiSel[0],DLG_EXCEP_LIST,&Object) == -1 )
					{
						MessageBeep(0);
						break;
					}

					UnmakeExceptionObject(&Object, &dwExceptionCode, Name,
						&iAction, &iPlatformId);
					sprintf(Number, "%08lx", dwExceptionCode);
				}
				else
				{
					Number[0] = '\0';
					Name[0] = '\0';

					if (SelCount == 0)
						iAction = 1;	// default to "stop if not handled"
					else
					{
						// Set 'iAction' to -1, 0, or 1, depending on the actions of
						// all selected lines
						iAction = -1;
						for (i=0; i<SelCount; ++i)
						{
							GetNthEnabledObject(lpiSel[i],DLG_EXCEP_LIST,&Object);
							if (iAction == -1)
							{
								// first time through loop: get action of first item
								iAction = Object.iAction;
							}
							else if (iAction != Object.iAction)
							{
								// item has different action from previous item;
								// break out of loop
								iAction = -1;
								break;
							}
						}
					}
				}

				// Put the text in the edit fields
				fSelChange = TRUE;
				SendDlgItemMessage(hDlg,DLG_EXCEP_NUMBER,WM_SETTEXT,0,(LPARAM)(LPSTR)Number);
				SendDlgItemMessage(hDlg,DLG_EXCEP_NAME,WM_SETTEXT,0,(LPARAM)(LPSTR)Name);
				fSelChange = FALSE;

				// Set action state
				SetAction(hDlg,iAction);

				EnableControls(hDlg);
			}
			break;
		case DLG_EXCEP_DEFAULT:
			ReInitSystemExceptions( FALSE );
			UpdateListboxContent(hDlg);

			// now no exceptions are selected in listbox
			if (lpiSel)
				_ffree(lpiSel);
			lpiSel = NULL;
			SelCount = 0;

			fSelChange = TRUE;
			SendDlgItemMessage(hDlg, DLG_EXCEP_NAME, WM_SETTEXT, 0,
				(LPARAM)(LPSTR)szNull);
			SendDlgItemMessage(hDlg, DLG_EXCEP_NUMBER, WM_SETTEXT, 0,
				(LPARAM)(LPSTR)szNull);
			SetAction(hDlg, 1);
			fSelChange = FALSE;

			EnableControls(hDlg);
			break;
		case IDCANCEL:
			ListCopy(DLG_EXCEP_LIST, DLG_EXCEP_LIST_BACKUP, (LPSTR)&Object, sizeof(Object));
			EndDialog(hDlg, TRUE);
			if (lpiSel)
			{
				_ffree(lpiSel);
				lpiSel = NULL;
			}
			break;
		case IDOK:
			if (fExceptionEdited)
				AddChangeException(hDlg);
			// Validate exception actions
			ValidateExceptionStatus();
			EndDialog(hDlg, TRUE);
			if (lpiSel)
			{
				_ffree(lpiSel);
				lpiSel = NULL;
			}
			return (TRUE);
		}
		break;

	// Owner-Draw list-box stuff:
	case WM_MEASUREITEM:
	case WM_DRAWITEM:
		if ( HandleStandardOwnerDrawListBox(hDlg, message, (WPARAM)wParam, (LPARAM)lParam,
                                			DLG_EXCEP_LIST, DrawExceptionListItem) )
			return( TRUE );

		return FALSE;

	}

	return DefaultDialogHandler(hDlg, message, wParam, lParam);
}

/****************************************************************************

	FUNCTION:   HandleStandardOwnerDrawListBox

	PURPOSE:    Provide standard handling of "owner draw" list boxes.
				Can be used for all list box controls that are
				owner draw fixed where we write text.

	RETURNS:    TRUE if message handled for the passed item,
				FALSE otherwise

****************************************************************************/
BOOL FAR PASCAL HandleStandardOwnerDrawListBox(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam,UINT CtlID,OWNERDRAWCALLBACK ODCallBack)
{
	switch (message)
	{
		case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpMIS;
			HWND hwndLB;
			HDC hdcLB;
			TEXTMETRIC tm;
			HFONT hFont, hOldFont;

			lpMIS = (LPMEASUREITEMSTRUCT)lParam;

			// Firstly, is this message for us
			if (lpMIS->CtlID != CtlID) return FALSE;

			hwndLB = GetDlgItem(hDlg, lpMIS->CtlID);
			hdcLB = GetDC(hwndLB);
			hFont = (HFONT)SendMessage(hwndLB, WM_GETFONT, 0, 0);
			if (hFont == NULL)
				hFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
			if (hFont != NULL)
				hOldFont = (HFONT)SelectObject(hdcLB, hFont);
			GetTextMetrics(hdcLB, &tm);
			if (hFont != NULL)
				SelectObject(hdcLB, hOldFont);
			ReleaseDC(hwndLB, hdcLB);
			lpMIS->itemHeight = tm.tmHeight;

			return TRUE;
		}

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDIS;

			lpDIS = (LPDRAWITEMSTRUCT)lParam;

			// Firstly, is this message for us
			if (lpDIS->CtlID != CtlID) return FALSE;

			if (lpDIS->itemID == -1)
			{
				// We have focus but no items so allow
				// default handling
				return FALSE;
			}

			switch (lpDIS->itemAction)
			{
				case ODA_DRAWENTIRE:
				case ODA_SELECT:
				{
               HBRUSH hBrushBack;
               COLORREF rgbTextColor, rgbBkColor;

					if (lpDIS->itemState & ODS_SELECTED)
					{
						hBrushBack = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
      				FillRect(lpDIS->hDC, &lpDIS->rcItem, hBrushBack);
      				DeleteObject((HGDIOBJ)hBrushBack);
      				rgbTextColor = SetTextColor(lpDIS->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
      				rgbBkColor = SetBkColor(lpDIS->hDC, GetSysColor(COLOR_HIGHLIGHT));
    				}
    				else
    				{
						hBrushBack = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
      				FillRect(lpDIS->hDC, &lpDIS->rcItem, hBrushBack);
      				DeleteObject((HGDIOBJ)hBrushBack);
    				}

					// Call the callback to draw the text
					(*ODCallBack)(hDlg, lpDIS);

					if (lpDIS->itemState & ODS_SELECTED)
					{
      				SetTextColor(lpDIS->hDC, rgbTextColor);
      				SetBkColor(lpDIS->hDC, rgbBkColor);
    				}
					break;
				}

				case ODA_FOCUS:
					DrawFocusRect(lpDIS->hDC, &lpDIS->rcItem);
					break;
			}

			// We've done the processing
			return TRUE;
		}

		default:
			return FALSE;
	}
}

