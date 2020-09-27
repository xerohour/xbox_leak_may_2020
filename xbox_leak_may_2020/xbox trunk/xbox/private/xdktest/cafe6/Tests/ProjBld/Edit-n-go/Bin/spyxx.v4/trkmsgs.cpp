//-----------------------------------------------------------------------------
//  TrkMsgs.cpp
//
//  Trackbar (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef DISABLE_WIN95_MESSAGES
//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETPOS);

DECODERET(TBM_GETPOS)
{
	PARM(lPosition, INT, RET);
	POUT(lPosition);

	return TRUE;
}

NODECODEPARM(TBM_GETPOS);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETRANGEMIN);

DECODERET(TBM_GETRANGEMIN)
{
	PARM(lMinimum, INT, RET);
	POUT(lMinimum);

	return TRUE;
}

NODECODEPARM(TBM_GETRANGEMIN);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETRANGEMAX);

DECODERET(TBM_GETRANGEMAX)
{
	PARM(lMaximum, INT, RET);
	POUT(lMaximum);

	return TRUE;
}

NODECODEPARM(TBM_GETRANGEMAX);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_GETTIC)
{
	PARM(iTic, INT, WP);
	POUT(iTic);

	return TRUE;
}

DECODERET(TBM_GETTIC)
{
	PARM(lPosition, INT, RET);
	POUT(lPosition);

	return TRUE;
}

NODECODEPARM(TBM_GETTIC);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETTIC)
{
	PARM(lPosition, INT, LP);
	POUT(lPosition);

	return TRUE;
}

DECODERET(TBM_SETTIC)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TBM_SETTIC);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETPOS)
{
	PARM(fPosition, BOOL, WP);
	PARM(lPosition, INT, LP);

	POUTB(fPosition);
	POUT(lPosition);

	return TRUE;
}

NODECODERET(TBM_SETPOS);

NODECODEPARM(TBM_SETPOS);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETRANGE)
{
	PARM(fRedraw, BOOL, WP);
	PARM(lMinimum, INT, LOLP);
	PARM(lMaximum, INT, HILP);

	POUTB(fRedraw);
	POUT(lMinimum);
	POUT(lMaximum);

	return TRUE;
}

NODECODERET(TBM_SETRANGE);

NODECODEPARM(TBM_SETRANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETRANGEMIN)
{
	PARM(fRedraw, BOOL, WP);
	PARM(lMinimum, INT, LP);

	POUTB(fRedraw);
	POUT(lMinimum);

	return TRUE;
}

NODECODERET(TBM_SETRANGEMIN);

NODECODEPARM(TBM_SETRANGEMIN);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETRANGEMAX)
{
	PARM(fRedraw, BOOL, WP);
	PARM(lMaximum, INT, LP);

	POUTB(fRedraw);
	POUT(lMaximum);

	return TRUE;
}

NODECODERET(TBM_SETRANGEMAX);

NODECODEPARM(TBM_SETRANGEMAX);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_CLEARTICS)
{
	PARM(fRedraw, BOOL, WP);
	TABLEORINT(tblTrueFalse, fRedraw);

	return TRUE;
}

NODECODERET(TBM_CLEARTICS);

NODECODEPARM(TBM_CLEARTICS);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETSEL)
{
	PARM(fRedraw, BOOL, WP);
	PARM(lMinimum, INT, LOLP);
	PARM(lMaximum, INT, HILP);

	POUTB(fRedraw);
	POUT(lMinimum);
	POUT(lMaximum);

	return TRUE;
}

NODECODERET(TBM_SETSEL);

NODECODEPARM(TBM_SETSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETSELSTART)
{
	PARM(fRedraw, BOOL, WP);
	PARM(lStart, INT, LP);

	POUTB(fRedraw);
	POUT(lStart);

	return TRUE;
}

NODECODERET(TBM_SETSELSTART);

NODECODEPARM(TBM_SETSELSTART);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETSELEND)
{
	PARM(fRedraw, BOOL, WP);
	PARM(lEnd, INT, LP);

	POUTB(fRedraw);
	POUT(lEnd);

	return TRUE;
}

NODECODERET(TBM_SETSELEND);

NODECODEPARM(TBM_SETSELEND);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETPTICS);

DECODERET(TBM_GETPTICS)
{
	PARM(apTicks, DWORD, RET);
	POUTC(apTicks, DWORD);

	return TRUE;
}

NODECODEPARM(TBM_GETPTICS);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_GETTICPOS)
{
	DECODELIKE(TBM_GETTIC);
}

DECODERET(TBM_GETTICPOS)
{
	DECODERETLIKE(TBM_GETTIC);
}

NODECODEPARM(TBM_GETTICPOS);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETNUMTICS);

DECODERET(TBM_GETNUMTICS)
{
	PARM(nTickMarks, INT, RET);
	POUT(nTickMarks);

	return TRUE;
}

NODECODEPARM(TBM_GETNUMTICS);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETSELSTART);

DECODERET(TBM_GETSELSTART)
{
	PARM(lStart, INT, RET);
	POUT(lStart);

	return TRUE;
}

NODECODEPARM(TBM_GETSELSTART);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETSELEND);

DECODERET(TBM_GETSELEND)
{
	PARM(lEnd, INT, RET);
	POUT(lEnd);

	return TRUE;
}

NODECODEPARM(TBM_GETSELEND);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_CLEARSEL)
{
	PARM(fRedraw, BOOL, WP);
	TABLEORINT(tblTrueFalse, fRedraw);

	return TRUE;
}

NODECODERET(TBM_CLEARSEL);

NODECODEPARM(TBM_CLEARSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETTICFREQ)
{
	PARM(wFreq, INT, WP);
	PARM(lPosition, INT, LP);

	POUT(wFreq);
	POUT(lPosition);

	return TRUE;
}

NODECODERET(TBM_SETTICFREQ);

NODECODEPARM(TBM_SETTICFREQ);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETPAGESIZE)
{
	PARM(lPageSize, INT, LP);
	POUT(lPageSize);

	return TRUE;
}

DECODERET(TBM_SETPAGESIZE)
{
	PARM(lPrevPageSize, INT, RET);
	POUT(lPrevPageSize);

	return TRUE;
}

NODECODEPARM(TBM_SETPAGESIZE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETPAGESIZE);

DECODERET(TBM_GETPAGESIZE)
{
	PARM(lPageSize, INT, RET);
	POUT(lPageSize);

	return TRUE;
}

NODECODEPARM(TBM_GETPAGESIZE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETLINESIZE)
{
	PARM(lLineSize, INT, LP);
	POUT(lLineSize);

	return TRUE;
}

DECODERET(TBM_SETLINESIZE)
{
	PARM(lPrevLineSize, INT, RET);
	POUT(lPrevLineSize);

	return TRUE;
}

NODECODEPARM(TBM_SETLINESIZE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETLINESIZE);

DECODERET(TBM_GETLINESIZE)
{
	PARM(lLineSize, INT, RET);
	POUT(lLineSize);

	return TRUE;
}

NODECODEPARM(TBM_GETLINESIZE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_GETTHUMBRECT)
{
	DECODELIKE(TBM_GETCHANNELRECT);
}

NODECODERET(TBM_GETTHUMBRECT);

DECODEPARM(TBM_GETTHUMBRECT)
{
	DECODEPARMLIKE(TBM_GETCHANNELRECT);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_GETCHANNELRECT)
{
	PARM(lprc, LPRECT, LP);
	POUTC(lprc, DWORD);

	return TRUE;
}

NODECODERET(TBM_GETCHANNELRECT);

DECODEPARM(TBM_GETCHANNELRECT)
{
	P2WPOUT();

	P2LPOUTPTR(RECT);
	P2IFSIZEOF(RECT)
	{
		PARM(lprc, LPRECT, ED2);
		INDENT();
		P2OUT(lprc, left);
		P2OUT(lprc, top);
		P2OUT(lprc, right);
		P2OUT(lprc, bottom);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TBM_SETTHUMBLENGTH)
{
	PARM(iLength, INT, WP);
	POUT(iLength);

	return TRUE;
}

NODECODERET(TBM_SETTHUMBLENGTH);

NODECODEPARM(TBM_SETTHUMBLENGTH);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TBM_GETTHUMBLENGTH);

DECODERET(TBM_GETTHUMBLENGTH)
{
	PARM(iLength, INT, RET);
	POUT(iLength);

	return TRUE;
}

NODECODEPARM(TBM_GETTHUMBLENGTH);

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
