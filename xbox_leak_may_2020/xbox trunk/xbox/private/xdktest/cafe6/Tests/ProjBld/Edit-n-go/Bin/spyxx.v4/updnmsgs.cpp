//-----------------------------------------------------------------------------
//  UpDnMsgs.cpp
//
//  Up-Down (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef DISABLE_WIN95_MESSAGES
//////////////////////////////////////////////////////////////////////////////

DECODE(UDM_SETRANGE)
{
	PARM(nUpper, INT, LOLP);
	PARM(nLower, INT, HILP);

	POUT(nUpper);
	POUT(nLower);

	return TRUE;
}

NODECODERET(UDM_SETRANGE);

NODECODEPARM(UDM_SETRANGE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(UDM_GETRANGE);

DECODERET(UDM_GETRANGE)
{
	PARM(nUpper, INT, LORET);
	PARM(nLower, INT, HIRET);

	POUT(nUpper);
	POUT(nLower);

	return TRUE;
}

NODECODEPARM(UDM_GETRANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(UDM_SETPOS)
{
	PARM(nPos, INT, LOLP);
	POUT(nPos);

	return TRUE;
}

DECODERET(UDM_SETPOS)
{
	PARM(nPrevPos, INT, RET);
	POUT(nPrevPos);

	return TRUE;
}

NODECODEPARM(UDM_SETPOS);

//////////////////////////////////////////////////////////////////////////////

NODECODE(UDM_GETPOS);

DECODERET(UDM_GETPOS)
{
	PARM(nPos, INT, LORET);
	PARM(fError, BOOL, HIRET);

	POUT(nPos);
	POUTB(fError);

	return TRUE;
}

NODECODEPARM(UDM_GETPOS);

//////////////////////////////////////////////////////////////////////////////

DECODE(UDM_SETBUDDY)
{
	PARM(hwndBuddy, HWND, WP);
	POUTC(hwndBuddy, DWORD);

	return TRUE;
}

DECODERET(UDM_SETBUDDY)
{
	PARM(hwndPrevBuddy, HWND, RET);
	POUTC(hwndPrevBuddy, DWORD);

	return TRUE;
}

NODECODEPARM(UDM_SETBUDDY);

//////////////////////////////////////////////////////////////////////////////

NODECODE(UDM_GETBUDDY);

DECODERET(UDM_GETBUDDY)
{
	PARM(hwndBuddy, HWND, RET);
	POUTC(hwndBuddy, DWORD);

	return TRUE;
}

NODECODEPARM(UDM_GETBUDDY);

//////////////////////////////////////////////////////////////////////////////

DECODE(UDM_SETACCEL)
{
	PARM(cAccels, INT, WP);
	PARM(aAccels, LPUDACCEL, LP);

	POUT(cAccels);
	POUTC(aAccels, DWORD);

	return TRUE;
}

DECODERET(UDM_SETACCEL)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(UDM_SETACCEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(UDM_GETACCEL)
{
	PARM(cAccels, INT, WP);
	PARM(paAccels, LPUDACCEL, LP);

	POUT(cAccels);
	POUTC(paAccels, DWORD);

	return TRUE;
}

DECODERET(UDM_GETACCEL)
{
	PARM(cAccels, INT, RET);
	POUT(cAccels);

	return TRUE;
}

NODECODEPARM(UDM_GETACCEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(UDM_SETBASE)
{
	PARM(nBase, INT, WP);
	POUT(nBase);

	return TRUE;
}

DECODERET(UDM_SETBASE)
{
	PARM(nPrevBase, INT, RET);
	POUT(nPrevBase);

	return TRUE;
}

NODECODEPARM(UDM_SETBASE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(UDM_GETBASE);

DECODERET(UDM_GETBASE)
{
	PARM(nBase, INT, RET);
	POUT(nBase);

	return TRUE;
}

NODECODEPARM(UDM_GETBASE);

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
