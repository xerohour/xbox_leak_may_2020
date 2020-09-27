//-----------------------------------------------------------------------------
//  BtnMsgs.cpp
//
//  Button (BM_*) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

static VALUETABLE tblBtnStyles[] =
{
	TABLEENTRY(BS_3STATE),
	TABLEENTRY(BS_AUTO3STATE),
	TABLEENTRY(BS_AUTOCHECKBOX),
	TABLEENTRY(BS_AUTORADIOBUTTON),
	TABLEENTRY(BS_CHECKBOX),
	TABLEENTRY(BS_DEFPUSHBUTTON),
	TABLEENTRY(BS_GROUPBOX),
	TABLEENTRY(BS_OWNERDRAW),
	TABLEENTRY(BS_PUSHBUTTON),
	TABLEENTRY(BS_RADIOBUTTON),
	TABLEENTRY(BS_USERBUTTON),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////
/*
// TODO: 
NODECODE(BM_CLICK);

NODECODERET(BM_CLICK);

NODECODEPARM(BM_CLICK);
*/
//////////////////////////////////////////////////////////////////////////////

NODECODE(BM_GETCHECK);

DECODERET(BM_GETCHECK)
{
	PARM(fCheck, UINT, RET);

	POUTC(fCheck, INT);
	switch (fCheck)
	{
		case 0:
			MOUT << ids(IDS_UNCHECKED);
			break;

		case 1:
			MOUT << ids(IDS_CHECKED);
			break;

		case 2:
			MOUT << ids(IDS_INDETERMINATE);
			break;
	}

	return TRUE;
}

NODECODEPARM(BM_GETCHECK);

//////////////////////////////////////////////////////////////////////////////

NODECODE(BM_GETSTATE);

DECODERET(BM_GETSTATE)
{
	PARM(lResult, UINT, RET);
	UINT fChecked = lResult & 0x0003;
	BOOL fHilight = (lResult & 0x0004) != 0;
	BOOL fFocus = (lResult & 0x0008) != 0;

	POUTC(fChecked, INT);
	switch (fChecked)
	{
		case 0:
			MOUT << ids(IDS_UNCHECKED);
			break;

		case 1:
			MOUT << ids(IDS_CHECKED);
			break;

		case 2:
			MOUT << ids(IDS_INDETERMINATE);
			break;
	}

	POUTB(fHilight);
	POUTB(fFocus);

	return TRUE;
}

NODECODEPARM(BM_GETSTATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(BM_SETCHECK)
{
	PARM(fCheck, UINT, WP);

	POUTC(fCheck, INT);
	switch (fCheck)
	{
		case 0:
			MOUT << ids(IDS_UNCHECKED);
			break;

		case 1:
			MOUT << ids(IDS_CHECKED);
			break;

		case 2:
			MOUT << ids(IDS_INDETERMINATE);
			break;
	}

	return TRUE;
}

NODECODERET(BM_SETCHECK);

NODECODEPARM(BM_SETCHECK);

//////////////////////////////////////////////////////////////////////////////

DECODE(BM_SETSTATE)
{
	PARM(fState, BOOL, WP);

	POUTB(fState);

	return TRUE;
}

NODECODERET(BM_SETSTATE);

NODECODEPARM(BM_SETSTATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(BM_SETSTYLE)
{
	PARM(dwStyle, DWORD, WP);
	PARM(fRedraw, BOOL, LOLP);

	PLABEL(dwStyle);
	TableOrValue(tblBtnStyles, (UINT)(dwStyle & ~BS_LEFTTEXT));

	if (dwStyle & BS_LEFTTEXT)
	{
		MOUT << " | BS_LEFTTEXT";
	}

	POUTB(fRedraw);

	return TRUE;
}

NODECODERET(BM_SETSTYLE);

NODECODEPARM(BM_SETSTYLE);

//////////////////////////////////////////////////////////////////////////////
