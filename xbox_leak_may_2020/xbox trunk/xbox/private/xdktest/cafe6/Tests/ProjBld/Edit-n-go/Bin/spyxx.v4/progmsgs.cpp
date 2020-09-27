//-----------------------------------------------------------------------------
//  ProgMsgs.cpp
//
//  Progress gauge (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef DISABLE_WIN95_MESSAGES
//////////////////////////////////////////////////////////////////////////////

DECODE(PBM_SETRANGE)
{
	PARM(nMinRange, INT, LOLP);
	PARM(nMaxRange, INT, HILP);

	POUT(nMinRange);
	POUT(nMaxRange);

	return TRUE;
}

DECODERET(PBM_SETRANGE)
{
	PARM(nPrevMinRange, INT, LORET);
	PARM(nPrevMaxRange, INT, HIRET);

	POUT(nPrevMinRange);
	POUT(nPrevMaxRange);

	return TRUE;
}

NODECODEPARM(PBM_SETRANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(PBM_SETPOS)
{
	PARM(nNewPos, INT, WP);
	POUT(nNewPos);

	return TRUE;
}

DECODERET(PBM_SETPOS)
{
	PARM(nPrevPos, INT, RET);
	POUT(nPrevPos);

	return TRUE;
}

NODECODEPARM(PBM_SETPOS);

//////////////////////////////////////////////////////////////////////////////

DECODE(PBM_DELTAPOS)
{
	PARM(nIncrement, INT, WP);
	POUT(nIncrement);

	return TRUE;
}

DECODERET(PBM_DELTAPOS)
{
	PARM(nPrevPos, INT, RET);
	POUT(nPrevPos);

	return TRUE;
}

NODECODEPARM(PBM_DELTAPOS);

//////////////////////////////////////////////////////////////////////////////

DECODE(PBM_SETSTEP)
{
	PARM(nStepInc, INT, WP);
	POUT(nStepInc);

	return TRUE;
}

DECODERET(PBM_SETSTEP)
{
	PARM(nPrevStepInc, INT, RET);
	POUT(nPrevStepInc);

	return TRUE;
}

NODECODEPARM(PBM_SETSTEP);

//////////////////////////////////////////////////////////////////////////////

NODECODE(PBM_STEPIT);

DECODERET(PBM_STEPIT)
{
	PARM(nPrevPos, INT, RET);
	POUT(nPrevPos);

	return TRUE;
}

NODECODEPARM(PBM_STEPIT);

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
