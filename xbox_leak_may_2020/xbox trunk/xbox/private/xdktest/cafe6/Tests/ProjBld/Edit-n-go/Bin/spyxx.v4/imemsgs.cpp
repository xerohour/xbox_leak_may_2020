//-----------------------------------------------------------------------------
//  IMEMsgs.cpp
//
//  IME message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

static VALUETABLE tblIMEReportType[] =
{
	TABLEENTRY(IR_STRINGSTART),
	TABLEENTRY(IR_STRINGEND),
	TABLEENTRY(IR_OPENCONVERT),
	TABLEENTRY(IR_CHANGECONVERT),
	TABLEENTRY(IR_CLOSECONVERT),
	TABLEENTRY(IR_FULLCONVERT),
	TABLEENTRY(IR_IMESELECT),
	TABLEENTRY(IR_STRING),
	TABLEENTRY(IR_DBCSCHAR),
	TABLEENTRY(IR_UNDETERMINE),
	TABLEENTRY(IR_STRINGEX),
	TABLEEND
};

#ifndef DISABLE_WIN95_MESSAGES

static VALUETABLE tblIMECompFlags[] =
{
	TABLEENTRY(GCS_COMPREADSTR),
	TABLEENTRY(GCS_COMPREADATTR),
	TABLEENTRY(GCS_COMPREADCLAUSE),
	TABLEENTRY(GCS_COMPSTR),
	TABLEENTRY(GCS_COMPATTR),
	TABLEENTRY(GCS_COMPCLAUSE),
	TABLEENTRY(GCS_CURSORPOS),
	TABLEENTRY(GCS_DELTASTART),
	TABLEENTRY(GCS_RESULTREADSTR),
	TABLEENTRY(GCS_RESULTREADCLAUSE),
	TABLEENTRY(GCS_RESULTSTR),
	TABLEENTRY(GCS_RESULTCLAUSE),
	TABLEENTRY(CS_INSERTCHAR),
	TABLEENTRY(CS_NOMOVECARET),
	TABLEEND
};

static VALUETABLE tblIMENotifyCmds[] =
{
	TABLEENTRY(IMN_PRIVATE),
	TABLEENTRY(IMN_GUIDELINE),
	TABLEENTRY(IMN_SETSTATUSWINDOWPOS),
	TABLEENTRY(IMN_SETCOMPOSITIONWINDOW),
	TABLEENTRY(IMN_SETCOMPOSITIONFONT),
	TABLEENTRY(IMN_SETCANDIDATEPOS),
	TABLEENTRY(IMN_SETOPENSTATUS),
	TABLEENTRY(IMN_SETSENTENCEMODE),
	TABLEENTRY(IMN_SETCONVERSIONMODE),
	TABLEENTRY(IMN_OPENCANDIDATE),
	TABLEENTRY(IMN_CLOSECANDIDATE),
	TABLEENTRY(IMN_CHANGECANDIDATE),
	TABLEENTRY(IMN_OPENSTATUSWINDOW),
	TABLEENTRY(IMN_CLOSESTATUSWINDOW),
	TABLEEND
};

static VALUETABLE tblIMEControls[] =
{
	TABLEENTRY(IMC_SETSTATUSWINDOWPOS),
	TABLEENTRY(IMC_GETSTATUSWINDOWPOS),
	TABLEENTRY(IMC_SETCOMPOSITIONWINDOW),
	TABLEENTRY(IMC_GETCOMPOSITIONWINDOW),
	TABLEENTRY(IMC_SETCOMPOSITIONFONT),
	TABLEENTRY(IMC_GETCOMPOSITIONFONT),
	TABLEENTRY(IMC_SETCANDIDATEPOS),
	TABLEENTRY(IMC_GETCANDIDATEPOS),
	TABLEEND
};

#endif
//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CONVERTREQUESTEX)
{
	PARM(hwnd, HWND, WP);
	PARM(hglbIMEStruct, HGLOBAL, LP);

	POUT(hwnd);
	POUTC(hglbIMEStruct, DWORD);

	return TRUE;
}

DECODERET(WM_CONVERTREQUESTEX)
{
	PARM(lResult, LRESULT, RET);

	POUTC(lResult, DWORD);

	return TRUE;
}

NODECODEPARM(WM_CONVERTREQUESTEX);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IMEKEYDOWN)
{
	DECODELIKE(WM_KEYDOWN);
}

DECODERET(WM_IMEKEYDOWN)
{
	DECODERETLIKE(WM_KEYDOWN);
}

DECODEPARM(WM_IMEKEYDOWN)
{
	DECODEPARMLIKE(WM_KEYDOWN);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IMEKEYUP)
{
	DECODELIKE(WM_KEYUP);
}

DECODERET(WM_IMEKEYUP)
{
	DECODERETLIKE(WM_KEYUP);
}

DECODEPARM(WM_IMEKEYUP)
{
	DECODEPARMLIKE(WM_KEYUP);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IME_REPORT)
{
	PARM(nReportType, UINT, WP);
	PARM(lParam, LPARAM, LP);

	TABLEORVALUE(tblIMEReportType, nReportType);
	POUTC(lParam, DWORD);

	return TRUE;
}

DECODERET(WM_IME_REPORT)
{
	PARM(lResult, LRESULT, RET);

	POUTC(lResult, DWORD);

	return TRUE;
}

NODECODEPARM(WM_IME_REPORT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_IME_STARTCOMPOSITION);

NODECODERET(WM_IME_STARTCOMPOSITION);

NODECODEPARM(WM_IME_STARTCOMPOSITION);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_IME_ENDCOMPOSITION);

NODECODERET(WM_IME_ENDCOMPOSITION);

NODECODEPARM(WM_IME_ENDCOMPOSITION);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IME_COMPOSITION)
{
	PARM(chDBCS, WORD, WP);
	PARM(fFlags, LPARAM, LP);

	POUTC(chDBCS, WORD);
#ifndef DISABLE_WIN95_MESSAGES
	TABLEORVALUE(tblIMECompFlags, fFlags);
#endif
	return TRUE;
}

NODECODERET(WM_IME_COMPOSITION);

NODECODEPARM(WM_IME_COMPOSITION);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IME_SETCONTEXT)
{
	PARM(fSet, BOOL, WP);
	PARM(hContext, HANDLE, LP);

	POUTC(fSet, BOOL);
	POUTC(hContext, DWORD);

	return TRUE;
}

NODECODERET(WM_IME_SETCONTEXT);

NODECODEPARM(WM_IME_SETCONTEXT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IME_NOTIFY)
{
	PARM(dwCommand, LPARAM, WP);
	PARM(dwData, DWORD, LP);

#ifndef DISABLE_WIN95_MESSAGES
	TABLEORVALUE(tblIMENotifyCmds, dwCommand);
#endif
	POUTC(dwData, DWORD);

	return TRUE;
}

NODECODERET(WM_IME_NOTIFY);

NODECODEPARM(WM_IME_NOTIFY);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IME_CONTROL)
{
	PARM(dwCommand, LPARAM, WP);
	PARM(dwData, DWORD, LP);

#ifndef DISABLE_WIN95_MESSAGES
	TABLEORVALUE(tblIMEControls, dwCommand);
#endif
	POUTC(dwData, DWORD);

	return TRUE;
}

NODECODERET(WM_IME_CONTROL);

NODECODEPARM(WM_IME_CONTROL);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_IME_COMPOSITIONFULL);

NODECODERET(WM_IME_COMPOSITIONFULL);

NODECODEPARM(WM_IME_COMPOSITIONFULL);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IME_SELECT)
{
	PARM(fSelect, BOOL, WP);
	PARM(hKL, HANDLE, LP);

	POUTC(fSelect, BOOL);
	POUTC(hKL, DWORD);

	return TRUE;
}

NODECODERET(WM_IME_SELECT);

NODECODEPARM(WM_IME_SELECT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IME_CHAR)
{
	PARM(chCharCode1, TCHAR, WP);
	PARM(chCharCode2, TCHAR, WP>>8);
	PARM(lKeyData, UINT, LP);
	INT cRepeat = (INT)(unsigned short)LOWORD(lKeyData);
	CHAR szScanCode[8];
	WORD wScanCode = HIWORD(lKeyData) & 0xFF;
	static TCHAR achBuffer[2+1];	// WinslowF - For V2 compatibility [MB_LEN_MAX + 1] VC2.0 does not have this defined. (MB_LEN_MAX = 2)

	achBuffer[0] = chCharCode1;
	if (_istlead(chCharCode1))
		achBuffer[1] = chCharCode2;
	else
		achBuffer[1] = (TCHAR)0;

	MOUT << " chCharCode:'" << achBuffer << "' (" << (INT)(UCHAR)WP << ')';

	POUT(cRepeat);
	wsprintf(szScanCode, "%2.2X", wScanCode);
	PLABEL(ScanCode);
	MOUT << szScanCode;

	return TRUE;
}

NODECODERET(WM_IME_CHAR);

NODECODEPARM(WM_IME_CHAR);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IME_KEYDOWN)
{
	DECODELIKE(WM_KEYDOWN);
}

DECODERET(WM_IME_KEYDOWN)
{
	DECODERETLIKE(WM_KEYDOWN);
}

DECODEPARM(WM_IME_KEYDOWN)
{
	DECODEPARMLIKE(WM_KEYDOWN);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_IME_KEYUP)
{
	DECODELIKE(WM_KEYUP);
}

DECODERET(WM_IME_KEYUP)
{
	DECODERETLIKE(WM_KEYUP);
}

DECODEPARM(WM_IME_KEYUP)
{
	DECODEPARMLIKE(WM_KEYUP);
}

//////////////////////////////////////////////////////////////////////////////
