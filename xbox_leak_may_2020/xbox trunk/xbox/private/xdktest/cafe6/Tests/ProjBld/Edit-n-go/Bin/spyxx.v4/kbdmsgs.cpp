//-----------------------------------------------------------------------------
//  KbdMsgs.cpp
//
//  Keyboard message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

static VALUETABLE tblHotKey[] =
{
	TABLEENTRY(IDHOT_SNAPDESKTOP),
	TABLEENTRY(IDHOT_SNAPWINDOW),
	TABLEEND
};

static VALUETABLE tblVirtKey[] =
{
	TABLEENTRY(VK_LBUTTON),
	TABLEENTRY(VK_RBUTTON),
	TABLEENTRY(VK_CANCEL),
	TABLEENTRY(VK_MBUTTON),
	TABLEENTRY(VK_BACK),
	TABLEENTRY(VK_TAB),
	TABLEENTRY(VK_CLEAR),
	TABLEENTRY(VK_RETURN),
	TABLEENTRY(VK_SHIFT),
	TABLEENTRY(VK_CONTROL),
	TABLEENTRY(VK_MENU),
	TABLEENTRY(VK_PAUSE),
	TABLEENTRY(VK_CAPITAL),
	TABLEENTRY(VK_ESCAPE),
	TABLEENTRY(VK_SPACE),
	TABLEENTRY(VK_PRIOR),
	TABLEENTRY(VK_NEXT),
	TABLEENTRY(VK_END),
	TABLEENTRY(VK_HOME),
	TABLEENTRY(VK_LEFT),
	TABLEENTRY(VK_UP),
	TABLEENTRY(VK_RIGHT),
	TABLEENTRY(VK_DOWN),
	TABLEENTRY(VK_SELECT),
	TABLEENTRY(VK_PRINT),
	TABLEENTRY(VK_EXECUTE),
	TABLEENTRY(VK_SNAPSHOT),
	TABLEENTRY(VK_INSERT),
	TABLEENTRY(VK_DELETE),
	TABLEENTRY(VK_HELP),
	TABLEENTRY(VK_NUMPAD0),
	TABLEENTRY(VK_NUMPAD1),
	TABLEENTRY(VK_NUMPAD2),
	TABLEENTRY(VK_NUMPAD3),
	TABLEENTRY(VK_NUMPAD4),
	TABLEENTRY(VK_NUMPAD5),
	TABLEENTRY(VK_NUMPAD6),
	TABLEENTRY(VK_NUMPAD7),
	TABLEENTRY(VK_NUMPAD8),
	TABLEENTRY(VK_NUMPAD9),
	TABLEENTRY(VK_MULTIPLY),
	TABLEENTRY(VK_ADD),
	TABLEENTRY(VK_SEPARATOR),
	TABLEENTRY(VK_SUBTRACT),
	TABLEENTRY(VK_DECIMAL),
	TABLEENTRY(VK_DIVIDE),
	TABLEENTRY(VK_F1),
	TABLEENTRY(VK_F2),
	TABLEENTRY(VK_F3),
	TABLEENTRY(VK_F4),
	TABLEENTRY(VK_F5),
	TABLEENTRY(VK_F6),
	TABLEENTRY(VK_F7),
	TABLEENTRY(VK_F8),
	TABLEENTRY(VK_F9),
	TABLEENTRY(VK_F10),
	TABLEENTRY(VK_F11),
	TABLEENTRY(VK_F12),
	TABLEENTRY(VK_F13),
	TABLEENTRY(VK_F14),
	TABLEENTRY(VK_F15),
	TABLEENTRY(VK_F16),
	TABLEENTRY(VK_F17),
	TABLEENTRY(VK_F18),
	TABLEENTRY(VK_F19),
	TABLEENTRY(VK_F20),
	TABLEENTRY(VK_F21),
	TABLEENTRY(VK_F22),
	TABLEENTRY(VK_F23),
	TABLEENTRY(VK_F24),
	TABLEENTRY(VK_NUMLOCK),
	TABLEENTRY(VK_SCROLL),
	TABLEEND
};

static VALUETABLE tblVirtKey2[] =
{
	TABLEENTRY('0'),
	TABLEENTRY('1'),
	TABLEENTRY('2'),
	TABLEENTRY('3'),
	TABLEENTRY('4'),
	TABLEENTRY('5'),
	TABLEENTRY('6'),
	TABLEENTRY('7'),
	TABLEENTRY('8'),
	TABLEENTRY('9'),
	TABLEENTRY('A'),
	TABLEENTRY('B'),
	TABLEENTRY('C'),
	TABLEENTRY('D'),
	TABLEENTRY('E'),
	TABLEENTRY('F'),
	TABLEENTRY('G'),
	TABLEENTRY('H'),
	TABLEENTRY('I'),
	TABLEENTRY('J'),
	TABLEENTRY('K'),
	TABLEENTRY('L'),
	TABLEENTRY('M'),
	TABLEENTRY('N'),
	TABLEENTRY('O'),
	TABLEENTRY('P'),
	TABLEENTRY('Q'),
	TABLEENTRY('R'),
	TABLEENTRY('S'),
	TABLEENTRY('T'),
	TABLEENTRY('U'),
	TABLEENTRY('V'),
	TABLEENTRY('W'),
	TABLEENTRY('X'),
	TABLEENTRY('Y'),
	TABLEENTRY('Z'),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

static VOID PrintVirtualKey(UINT vkey)
{
	if (!TableOrNothing(tblVirtKey, vkey))
	{
		TableOrValueNotHot(tblVirtKey2, vkey);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CHAR)
{
	static TCHAR tchLead = 0;

	PARM(chCharCode, TCHAR, WP);
	PARM(lKeyData, UINT, LP);
	INT cRepeat = (INT)(unsigned short)LOWORD(lKeyData);
	CHAR szScanCode[8];
	WORD wScanCode = HIWORD(lKeyData) & 0xFF;
	BOOL fExtended = (HIWORD(lKeyData) & KF_EXTENDED) ? 1 : 0;
	BOOL fAltDown = (HIWORD(lKeyData) & KF_ALTDOWN) ? 1 : 0;
	BOOL fRepeat = (HIWORD(lKeyData) & KF_REPEAT) ? 1 : 0;
	BOOL fUp = (HIWORD(lKeyData) & KF_UP) ? 1 : 0;

	if (IsDBCSLeadByte((char)chCharCode))
	{
		MOUT << " chCharCode:'.' (" << (INT)(UCHAR)chCharCode << ") (lead)";
		tchLead = chCharCode;
	}
	else if (tchLead)
	{
		MOUT << " chCharCode:'" << tchLead << chCharCode << "' (" << (INT)(UCHAR)chCharCode << ") (trail)";
		tchLead = 0;
	}
	else
	{
		MOUT << " chCharCode:'" << chCharCode << "' (" << (INT)(UCHAR)chCharCode << ')';
	}

	POUT(cRepeat);
	wsprintf(szScanCode, "%2.2X", wScanCode);
	PLABEL(ScanCode);
	MOUT << szScanCode;

	//
	// These are printed out as 1/0, not True/False to save space.
	//
	POUT(fExtended);
	POUT(fAltDown);
	POUT(fRepeat);
	POUT(fUp);

	return TRUE;
}

NODECODERET(WM_CHAR);

NODECODEPARM(WM_CHAR);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CHARTOITEM)
{
	PARM(nKey, CHAR, LOWP);
	PARM(nCaretPos, UINT, HIWP);
	PARM(hwndListBox, HWND, LP);

	PLABEL(nKey);
	MOUT << '\'' << nKey << "' (" << (INT)(UCHAR)nKey << ')';
	POUTC(nCaretPos, INT);
	POUT(hwndListBox);

	return TRUE;
}

DECODERET(WM_CHARTOITEM)
{
	PARM(fuAction, INT, RET);

	POUT(fuAction);
	switch (fuAction)
	{
		case -2:
			MOUT << ids(IDS_HANDLED_BY_APPLICATION);
			break;

		case -1:
			MOUT << ids(IDS_PERFORM_DEFAULT_ACTION);
			break;

		default:
			MOUT << ids(IDS_SELECT_ITEM_AT_INDEX) << fuAction << ')';
			break;
	}

	return TRUE;
}

NODECODEPARM(WM_CHARTOITEM);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DEADCHAR)
{
	DECODELIKE(WM_CHAR);
}

NODECODERET(WM_DEADCHAR);

NODECODEPARM(WM_DEADCHAR);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_GETHOTKEY);

DECODERET(WM_GETHOTKEY)
{
	PARM(vkey, UINT, RET);

	PLABEL(vkey);
	if (vkey)
	{
		PrintVirtualKey(vkey);
	}
	else
	{
		MOUT << ids(IDS_NONE);
	}

	return TRUE;
}

NODECODEPARM(WM_GETHOTKEY);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_HOTKEY)
{
	PARM(idHotKey, INT, WP);

	TABLEORVALUE(tblHotKey, idHotKey);
	// TODO: do a GlobalGetAtomName in the hook if the idHotKey is
	// in the proper range to try and get the name of the hotkey.

	return TRUE;
}

NODECODERET(WM_HOTKEY);

NODECODEPARM(WM_HOTKEY);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_KEYDOWN)
{
	PARM(nVirtKey, UINT, WP);
	PARM(lKeyData, UINT, LP);
	INT cRepeat = (INT)(unsigned short)LOWORD(lKeyData);
	CHAR szScanCode[8];
	WORD wScanCode = HIWORD(lKeyData) & 0xFF;
	BOOL fExtended = (HIWORD(lKeyData) & KF_EXTENDED) ? 1 : 0;
	BOOL fAltDown = (HIWORD(lKeyData) & KF_ALTDOWN) ? 1 : 0;
	BOOL fRepeat = (HIWORD(lKeyData) & KF_REPEAT) ? 1 : 0;
	BOOL fUp = (HIWORD(lKeyData) & KF_UP) ? 1 : 0;

	PLABEL(nVirtKey);
	PrintVirtualKey(nVirtKey);
	POUT(cRepeat);
	wsprintf(szScanCode, "%2.2X", wScanCode);
	PLABEL(ScanCode);
	MOUT << szScanCode;

	//
	// These are printed out as 1/0, not True/False to save space.
	//
	POUT(fExtended);
	POUT(fAltDown);
	POUT(fRepeat);
	POUT(fUp);

	return TRUE;
}

NODECODERET(WM_KEYDOWN);

NODECODEPARM(WM_KEYDOWN);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_KEYUP)
{
	DECODELIKE(WM_KEYDOWN);
}

NODECODERET(WM_KEYUP);

NODECODEPARM(WM_KEYUP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SETHOTKEY)
{
	PARM(vkey, UINT, WP);

	PLABEL(vkey);
	PrintVirtualKey(vkey);

	return TRUE;
}

DECODERET(WM_SETHOTKEY)
{
	PARM(lResult, INT, RET);

	POUT(lResult);
	switch (lResult)
	{
		case 2:
			MOUT << ids(IDS_SUCCESSFUL_HOTKEY1);
			break;

		case 1:
			MOUT << ids(IDS_SUCCESSFUL_HOTKEY2);
			break;

		case 0:
			MOUT << ids(IDS_UNSUCCESSFUL_HOTKEY1);
			break;

		case -1:
			MOUT << ids(IDS_UNSUCCESSFUL_HOTKEY2);
			break;
	}

	return TRUE;
}

NODECODEPARM(WM_SETHOTKEY);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SYSCHAR)
{
	DECODELIKE(WM_CHAR);
}

NODECODERET(WM_SYSCHAR);

NODECODEPARM(WM_SYSCHAR);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SYSDEADCHAR)
{
	DECODELIKE(WM_CHAR);
}

NODECODERET(WM_SYSDEADCHAR);

NODECODEPARM(WM_SYSDEADCHAR);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SYSKEYDOWN)
{
	DECODELIKE(WM_KEYDOWN);
}


NODECODERET(WM_SYSKEYDOWN);

NODECODEPARM(WM_SYSKEYDOWN);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SYSKEYUP)
{
	DECODELIKE(WM_KEYDOWN);
}


NODECODERET(WM_SYSKEYUP);

NODECODEPARM(WM_SYSKEYUP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_VKEYTOITEM)
{
	PARM(vkey, UINT, LOWP);
	PARM(nCaretPos, UINT, HIWP);
	PARM(hwndLB, HWND, LP);

	PLABEL(vkey);
	PrintVirtualKey(vkey);
	POUTC(nCaretPos, INT);
	POUT(hwndLB);

	return TRUE;
}

DECODERET(WM_VKEYTOITEM)
{
	DECODERETLIKE(WM_CHARTOITEM);
}

NODECODEPARM(WM_VKEYTOITEM);

//////////////////////////////////////////////////////////////////////////////
