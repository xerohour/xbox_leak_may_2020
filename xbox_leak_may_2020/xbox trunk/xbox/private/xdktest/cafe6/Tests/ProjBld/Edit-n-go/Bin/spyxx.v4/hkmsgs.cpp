//-----------------------------------------------------------------------------
//  HKMsgs.cpp
//
//  Hotkey (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef DISABLE_WIN95_MESSAGES

static VALUETABLE tblCombInv[] =
{
	TABLEENTRY(HKCOMB_SCA),
	TABLEENTRY(HKCOMB_SC),
	TABLEENTRY(HKCOMB_SA),
	TABLEENTRY(HKCOMB_CA),
	TABLEENTRY(HKCOMB_S),
	TABLEENTRY(HKCOMB_C),
	TABLEENTRY(HKCOMB_A),
	TABLEENTRY(HKCOMB_NONE),
	TABLEEND
};

static VALUETABLE tblModInv[] =
{
	TABLEENTRY(HOTKEYF_ALT),
	TABLEENTRY(HOTKEYF_CONTROL),
	TABLEENTRY(HOTKEYF_SHIFT),
	TABLEENTRY(HOTKEYF_EXT),
	TABLEEND
};


//////////////////////////////////////////////////////////////////////////////

DECODE(HKM_SETHOTKEY)
{
	PARM(bVKHotKey, BYTE, LOWP);
	PARM(bfMods, BYTE, HIWP);

	// REVIEW: PrintVirtualKey()?
	POUTC(bVKHotKey, BYTE);
	PLABEL(bfMods);
	TABLEORVALUE(tblModInv, bfMods);

	return TRUE;
}

NODECODERET(HKM_SETHOTKEY);

NODECODEPARM(HKM_SETHOTKEY);

//////////////////////////////////////////////////////////////////////////////

NODECODE(HKM_GETHOTKEY);

DECODERET(HKM_GETHOTKEY)
{
	PARM(bVKHotKey, BYTE, LOBYTE(LORET));
	PARM(bfMods, BYTE, HIBYTE(LORET));

	// REVIEW: PrintVirtualKey()?
	POUTC(bVKHotKey, BYTE);
	PLABEL(bfMods);
	TABLEORVALUE(tblModInv, bfMods);

	return TRUE;
}

NODECODEPARM(HKM_GETHOTKEY);

//////////////////////////////////////////////////////////////////////////////

DECODE(HKM_SETRULES)
{
	PARM(fwCombInv, WORD, WP);
	PARM(fwModInv, WORD, LOLP);

	PLABEL(fwCombInv);
	TABLEORVALUE(tblCombInv, fwCombInv);
	PLABEL(fwModInv);
	TABLEORVALUE(tblModInv, fwModInv);

	return TRUE;
}

NODECODERET(HKM_SETRULES);

NODECODEPARM(HKM_SETRULES);

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
