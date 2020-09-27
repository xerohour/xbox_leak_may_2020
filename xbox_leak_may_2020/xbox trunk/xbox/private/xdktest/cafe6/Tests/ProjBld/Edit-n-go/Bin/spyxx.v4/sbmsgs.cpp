//-----------------------------------------------------------------------------
//  SBMsgs.cpp
//
//  Scroll Bar message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

static VALUETABLE tblESBFlags[] =
{
	TABLEENTRY(ESB_ENABLE_BOTH),
	TABLEENTRY(ESB_DISABLE_LTUP),
	TABLEENTRY(ESB_DISABLE_RTDN),
	TABLEENTRY(ESB_DISABLE_BOTH),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

DECODE(SBM_ENABLE_ARROWS)
{
	PARM(fuArrowFlags, UINT, WP);

	TABLEORVALUE(tblESBFlags, fuArrowFlags);

	return TRUE;
}

DECODERET(SBM_ENABLE_ARROWS)
{
	PARM(fSuccess, BOOL, RET);

	POUTB(fSuccess);

	return TRUE;
}

NODECODEPARM(SBM_ENABLE_ARROWS);

//////////////////////////////////////////////////////////////////////////////
/*
// TODO: do some cracking here!
NODECODE(SBM_GETPAGE);

NODECODERET(SBM_GETPAGE);

NODECODEPARM(SBM_GETPAGE);
*/
//////////////////////////////////////////////////////////////////////////////

NODECODE(SBM_GETPOS);

DECODERET(SBM_GETPOS)
{
	PARM(nPos, INT, RET);

	POUT(nPos);

	return TRUE;
}

NODECODEPARM(SBM_GETPOS);

//////////////////////////////////////////////////////////////////////////////

DECODE(SBM_GETRANGE)
{
	PARM(lpnMinPos, LPINT, WP);
	PARM(lpnMaxPos, LPINT, LP);

	POUTC(lpnMinPos, DWORD);
	POUTC(lpnMaxPos, DWORD);

	return TRUE;
}

DECODERET(SBM_GETRANGE)
{
	PARM(lpnMinPos, LPINT, WP);
	PARM(lpnMaxPos, LPINT, LP);

	PIFSIZEOF(PACK_WPLPDWORDS)
	{
		PARM(ppwld, PPACK_WPLPDWORDS, ED);

		POUTC(lpnMinPos, DWORD);
		if (ppwld->fValidWP)
		{
			MOUT << " (" << (INT)ppwld->nWP << ')';
		}

		POUTC(lpnMaxPos, DWORD);
		if (ppwld->fValidLP)
		{
			MOUT << " (" << (INT)ppwld->nLP << ')';
		}
	}
	else
	{
		POUTC(lpnMinPos, DWORD);
		POUTC(lpnMaxPos, DWORD);
	}

	return TRUE;
}

DECODEPARM(SBM_GETRANGE)
{
	P2IFSIZEOF(PACK_WPLPDWORDS)
	{
		PARM(ppwld, PPACK_WPLPDWORDS, ED2);

		if (ppwld->fValidWP)
		{
			P2WPOUTPTR(int);
			INDENT();
			M2OUT << (INT)ppwld->nWP;
			P2ENDLINE();
			OUTDENT();
		}
		else
		{
			P2WPOUT();
		}

		if (ppwld->fValidLP)
		{
			P2LPOUTPTR(int);
			INDENT();
			M2OUT << (INT)ppwld->nLP;
			P2ENDLINE();
			OUTDENT();
		}
		else
		{
			P2WPOUT();
		}
	}
	else
	{
		P2WPOUT();
		P2LPOUT();
	}
}

//////////////////////////////////////////////////////////////////////////////
/*
// TODO: do some cracking here!
NODECODE(SBM_SETPAGE);

NODECODERET(SBM_SETPAGE);

NODECODEPARM(SBM_SETPAGE);
*/
//////////////////////////////////////////////////////////////////////////////

DECODE(SBM_SETPOS)
{
	PARM(nPos, INT, WP);
	PARM(fRedraw, BOOL, LP);

	POUT(nPos);
	POUTB(fRedraw);

	return TRUE;
}

DECODERET(SBM_SETPOS)
{
	PARM(nPosPrevious, INT, RET);

	POUT(nPosPrevious);

	return TRUE;
}

NODECODEPARM(SBM_SETPOS);

//////////////////////////////////////////////////////////////////////////////

DECODE(SBM_SETRANGE)
{
	PARM(nMinPos, INT, WP);
	PARM(nMaxPos, INT, LP);

	POUT(nMinPos);
	POUT(nMaxPos);

	return TRUE;
}

DECODERET(SBM_SETRANGE)
{
	DECODERETLIKE(SBM_SETPOS);
}

NODECODEPARM(SBM_SETRANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(SBM_SETRANGEREDRAW)
{
	DECODELIKE(SBM_SETRANGE);
}

DECODERET(SBM_SETRANGEREDRAW)
{
	DECODERETLIKE(SBM_SETPOS);
}

NODECODEPARM(SBM_SETRANGEREDRAW);

//////////////////////////////////////////////////////////////////////////////
