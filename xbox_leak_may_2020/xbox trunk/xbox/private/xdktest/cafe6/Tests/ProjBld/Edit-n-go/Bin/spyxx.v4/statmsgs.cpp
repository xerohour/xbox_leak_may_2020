//-----------------------------------------------------------------------------
//  StatMsgs.cpp
//
//  Static control message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

NODECODE(STM_GETICON);

DECODERET(STM_GETICON)
{
	PARM(hicon, HICON, RET);

	POUTC(hicon, DWORD);

	return TRUE;
}

NODECODEPARM(STM_GETICON);

//////////////////////////////////////////////////////////////////////////////

DECODE(STM_SETICON)
{
	PARM(hicon, HICON, WP);

	POUTC(hicon, DWORD);

	return TRUE;
}

DECODERET(STM_SETICON)
{
	PARM(hiconPrevious, HICON, RET);

	POUTC(hiconPrevious, DWORD);

	return TRUE;
}

NODECODEPARM(STM_SETICON);

//////////////////////////////////////////////////////////////////////////////
