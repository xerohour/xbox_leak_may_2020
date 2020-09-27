//-----------------------------------------------------------------------------
//  DlgMsgs.cpp
//
//  Dialog message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

NODECODE(DM_GETDEFID);

DECODERET(DM_GETDEFID)
{
	PARM(wHasDef, WORD, HIRET);
	PARM(wDefID, WORD, LORET);

	if (wHasDef == DC_HASDEFID)
	{
		PLABEL(wHasDef);
		MOUT << "DC_HASDEFID";
		POUTC(wDefID, WORD);
		return(TRUE);
	}

	return(FALSE);
}

NODECODEPARM(DM_GETDEFID);

//////////////////////////////////////////////////////////////////////////////

DECODE(DM_SETDEFID)
{
	PARM(wDefID, WORD, LOWP);

	POUTC(wDefID, WORD);

	return(TRUE);
}

DECODERET(DM_SETDEFID)
{
	PARM(fSetDefID, DWORD, RET);
	TABLEORINT(tblTrueFalse, fSetDefID);
	return(TRUE);
}

NODECODEPARM(DM_SETDEFID);

//////////////////////////////////////////////////////////////////////////////
