//-----------------------------------------------------------------------------
//  DDEMsgs.cpp
//
//  DDE message crackers.

#include "stdafx.h"
#pragma hdrstop

#include <dde.h>		// for UnpackDDElParam

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DDE_ACK)
{
	if (pmsd->fPostType != POSTTYPE_POSTED)
	{
		DECODELIKE(WM_DDE_INITIATE);
	}
	else
	{
		PARM(hwnd, HWND, WP);
		PARM(lPackedVal, DWORD, LP);

		UINT wStatus, aItem;

		POUT(hwnd);
		POUT(lPackedVal);

		if (UnpackDDElParam(WM_DDE_ACK, lPackedVal, &wStatus, &aItem) ||
			UnpackDDElParam(WM_DDE_ADVISE, lPackedVal, &wStatus, &aItem) ||
			UnpackDDElParam(WM_DDE_DATA, lPackedVal, &wStatus, &aItem) ||
			UnpackDDElParam(WM_DDE_EXECUTE, lPackedVal, &wStatus, &aItem) ||
			UnpackDDElParam(WM_DDE_POKE, lPackedVal, &wStatus, &aItem) ||
			UnpackDDElParam(WM_DDE_REQUEST, lPackedVal, &wStatus, &aItem) ||
			UnpackDDElParam(WM_DDE_TERMINATE, lPackedVal, &wStatus, &aItem) ||
			UnpackDDElParam(WM_DDE_UNADVISE, lPackedVal, &wStatus, &aItem))
		{
			CHAR szAtom[256];

			POUTC(wStatus, WORD);

			if (GlobalGetAtomName((ATOM)aItem, szAtom, sizeof(szAtom)))
			{
				POUTC(aItem, WORD);
				POUTS(szAtom);
			}
			else
			{
				UINT hCommands;

				if (UnpackDDElParam(WM_DDE_ACK, lPackedVal, &wStatus, &hCommands) ||
					UnpackDDElParam(WM_DDE_ADVISE, lPackedVal, &wStatus, &hCommands) ||
					UnpackDDElParam(WM_DDE_DATA, lPackedVal, &wStatus, &hCommands) ||
					UnpackDDElParam(WM_DDE_EXECUTE, lPackedVal, &wStatus, &hCommands) ||
					UnpackDDElParam(WM_DDE_POKE, lPackedVal, &wStatus, &hCommands) ||
					UnpackDDElParam(WM_DDE_REQUEST, lPackedVal, &wStatus, &hCommands) ||
					UnpackDDElParam(WM_DDE_TERMINATE, lPackedVal, &wStatus, &hCommands) ||
					UnpackDDElParam(WM_DDE_UNADVISE, lPackedVal, &wStatus, &hCommands))
				{
					POUTC(hCommands, DWORD);

					LPSTR lpszCommands;
	
					if ((lpszCommands = (LPSTR)GlobalLock((HGLOBAL)hCommands)) != NULL)
					{
						POUTS(lpszCommands);
						GlobalUnlock((HGLOBAL)hCommands);
					}
				}
			}
		}
	}

	return TRUE;
}

NODECODERET(WM_DDE_ACK);

NODECODEPARM(WM_DDE_ACK);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DDE_ADVISE)
{
	PARM(hwnd, HWND, WP);
	PARM(lPackedVal, DWORD, LP);

	UINT hOptions, aItem;

	POUT(hwnd);
	POUT(lPackedVal);

	if (UnpackDDElParam(WM_DDE_ACK, lPackedVal, &hOptions, &aItem) ||
		UnpackDDElParam(WM_DDE_ADVISE, lPackedVal, &hOptions, &aItem) ||
		UnpackDDElParam(WM_DDE_DATA, lPackedVal, &hOptions, &aItem) ||
		UnpackDDElParam(WM_DDE_EXECUTE, lPackedVal, &hOptions, &aItem) ||
		UnpackDDElParam(WM_DDE_INITIATE, lPackedVal, &hOptions, &aItem) ||
		UnpackDDElParam(WM_DDE_POKE, lPackedVal, &hOptions, &aItem) ||
		UnpackDDElParam(WM_DDE_REQUEST, lPackedVal, &hOptions, &aItem) ||
		UnpackDDElParam(WM_DDE_TERMINATE, lPackedVal, &hOptions, &aItem) ||
		UnpackDDElParam(WM_DDE_UNADVISE, lPackedVal, &hOptions, &aItem))
	{
		CHAR szAtom[256];

		POUTC(hOptions, WORD);
		POUTC(aItem, WORD);

		if (GlobalGetAtomName((ATOM)aItem, szAtom, sizeof(szAtom)))
		{
			POUTS(szAtom);
		}						 
	}

	return TRUE;
}

NODECODERET(WM_DDE_ADVISE);

NODECODEPARM(WM_DDE_ADVISE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DDE_DATA)
{
	DECODELIKE(WM_DDE_ADVISE);
}

NODECODERET(WM_DDE_DATA);

NODECODEPARM(WM_DDE_DATA);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DDE_EXECUTE)
{
	PARM(hwnd, HWND, WP);
	PARM(lPackedVal, DWORD, LP);

	UINT hOptions, hCommands;

	POUT(hwnd);
	POUT(lPackedVal);

	if (UnpackDDElParam(WM_DDE_ACK, lPackedVal, &hOptions, &hCommands) ||
		UnpackDDElParam(WM_DDE_ADVISE, lPackedVal, &hOptions, &hCommands) ||
		UnpackDDElParam(WM_DDE_DATA, lPackedVal, &hOptions, &hCommands) ||
		UnpackDDElParam(WM_DDE_EXECUTE, lPackedVal, &hOptions, &hCommands) ||
		UnpackDDElParam(WM_DDE_INITIATE, lPackedVal, &hOptions, &hCommands) ||
		UnpackDDElParam(WM_DDE_POKE, lPackedVal, &hOptions, &hCommands) ||
		UnpackDDElParam(WM_DDE_REQUEST, lPackedVal, &hOptions, &hCommands) ||
		UnpackDDElParam(WM_DDE_TERMINATE, lPackedVal, &hOptions, &hCommands) ||
		UnpackDDElParam(WM_DDE_UNADVISE, lPackedVal, &hOptions, &hCommands))
	{
		LPSTR lpszCommands;

		POUTC(hCommands, DWORD);

		if ((lpszCommands = (LPSTR)GlobalLock((HGLOBAL)hCommands)) != NULL)
		{
			POUTS(lpszCommands);
			GlobalUnlock((HGLOBAL)hCommands);
		}						 
	}

	return TRUE;
}

NODECODERET(WM_DDE_EXECUTE);

NODECODEPARM(WM_DDE_EXECUTE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DDE_INITIATE)
{
	PARM(hwnd, HWND, WP);
	PARM(aApp, WORD, LOLP);
	PARM(aTopic, WORD, HILP);

	CHAR szAtom[256];

	POUT(hwnd);

	POUT(aApp);
	if (GlobalGetAtomName(aApp, szAtom, sizeof(szAtom)))
	{
		POUTS(szAtom);
	}

	POUT(aTopic);
	if (GlobalGetAtomName(aTopic, szAtom, sizeof(szAtom)))
	{
		POUTS(szAtom);
	}

	return TRUE;
}

NODECODERET(WM_DDE_INITIATE);

NODECODEPARM(WM_DDE_INITIATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DDE_POKE)
{
	DECODELIKE(WM_DDE_ADVISE);
}

NODECODERET(WM_DDE_POKE);

NODECODEPARM(WM_DDE_POKE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DDE_REQUEST)
{
	PARM(hwnd, HWND, WP);
	PARM(cfFormat, UINT, LOLP);
	PARM(aItem, WORD, HILP);

	CHAR szAtom[256];

	POUT(hwnd);
	TABLEORVALUE(tblCBFormat, cfFormat);

	POUT(aItem);
	if (GlobalGetAtomName(aItem, szAtom, sizeof(szAtom)))
	{
		POUTS(szAtom);
	}

	return TRUE;
}

NODECODERET(WM_DDE_REQUEST);

NODECODEPARM(WM_DDE_REQUEST);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DDE_TERMINATE)
{
	PARM(hwnd, HWND, WP);

	POUT(hwnd);

	return TRUE;
}

NODECODERET(WM_DDE_TERMINATE);

NODECODEPARM(WM_DDE_TERMINATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DDE_UNADVISE)
{
	DECODELIKE(WM_DDE_REQUEST);
}

NODECODERET(WM_DDE_UNADVISE);

NODECODEPARM(WM_DDE_UNADVISE);

//////////////////////////////////////////////////////////////////////////////
