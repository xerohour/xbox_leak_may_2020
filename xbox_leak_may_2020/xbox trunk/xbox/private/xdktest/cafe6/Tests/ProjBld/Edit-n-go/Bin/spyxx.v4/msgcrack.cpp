//-----------------------------------------------------------------------------
//  MsgCrack.cpp
//
//  Copyright (C) 1993, Microsoft Corporation
//
//  Purpose:
//
//  Support routines for cracking messages.
//
//  Revision History:
//
//  04/01/93 byrond	 Created.
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CMsgStream * g_pmout;



//-----------------------------------------------------------------------------
// TableOrValue(UINT uValue)
//
// Prints (as a help topic) the value of uValue if found in the specified
// table, otherwise prints it out as either a DWORD or WORD value, depending
// on its size.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableOrValue(VALUETABLE *ptbl, UINT uValue)
{
	if (!(TableOrNothing(ptbl, uValue)))
	{
		if (uValue > 0xFFFF)
		{
			MOUT << (DWORD)uValue;
		}
		else
		{
			MOUT << (WORD)uValue;
		}
	}
}



//-----------------------------------------------------------------------------
// TableOrValue(WORD wValue)
//
// Prints (as a help topic) the value of wValue if found in the specified
// table, otherwise prints it out as a WORD value.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableOrValue(VALUETABLE *ptbl, WORD wValue)
{
	if (!(TableOrNothing(ptbl, wValue)))
	{
		MOUT << wValue;
	}
}



//-----------------------------------------------------------------------------
// TableOrValueNotHot(UINT uValue)
//
// Prints (but NOT as a help topic) the value of uValue if found in the
// specified table, otherwise prints it out as either a DWORD or WORD
// value, depending on its size.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableOrValueNotHot(VALUETABLE *ptbl, UINT uValue)
{
	if (!(TableOrNothingNotHot(ptbl, uValue)))
	{
		if (uValue > 0xFFFF)
		{
			MOUT << (DWORD)uValue;
		}
		else
		{
			MOUT << (WORD)uValue;
		}
	}
}



//-----------------------------------------------------------------------------
// TableOrDWord
//
// Prints (as a help topic) the value of uValue if found in the specified
// table, otherwise prints it out as a DWORD value.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableOrDWord(VALUETABLE *ptbl, UINT uValue)
{
	if (!(TableOrNothing(ptbl, uValue)))
	{
		MOUT << (DWORD)uValue;
	}
}



//-----------------------------------------------------------------------------
// TableOrInt
//
// Prints (as a help topic) the value of uValue if found in the specified
// table, otherwise prints it out as an INT value.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableOrInt(VALUETABLE *ptbl, UINT uValue)
{
	if (!(TableOrNothing(ptbl, uValue)))
	{
		MOUT << (INT)uValue;
	}
}



//-----------------------------------------------------------------------------
// TableOrNothing
//
// Prints (as a help topic) the value of uValue if found in the specified
// table, otherwise does not print anything out and returns FALSE.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

BOOL TableOrNothing(VALUETABLE *ptbl, UINT uValue)
{
	while (ptbl->pszValue)
	{
		if (uValue == ptbl->nValue)
		{
			MOUT << ptbl->pszValue;
			return TRUE;
		}

		ptbl++;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// TableOrNothing
//
// Prints (as a help topic) the value of wValue if found in the specified
// table, otherwise does not print anything out and returns FALSE.
// Only the LOWORD of the value in the table is compared to wValue.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

BOOL TableOrNothing(VALUETABLE *ptbl, WORD wValue)
{
	while (ptbl->pszValue)
	{
		if (wValue == (WORD)ptbl->nValue)
		{
			MOUT << ptbl->pszValue;
			return TRUE;
		}

		ptbl++;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// TableOrNothingNotHot
//
// Prints (but NOT as a help topic) the value of uValue if found in the
// specified table, otherwise does not print anything out and returns FALSE.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

BOOL TableOrNothingNotHot(VALUETABLE *ptbl, UINT uValue)
{
	while (ptbl->pszValue)
	{
		if (uValue == ptbl->nValue)
		{
			MOUT << ptbl->pszValue;
			return TRUE;
		}

		ptbl++;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// TableFlags
//
// Prints (as a help topic) the flag values found in uValue that match
// the flags found in the specified table, separated by " | ".  If there
// are any bits remaining in the uValue parameter that are not accounted
// for after checking all the flags in the table, they will be printed
// out at the end as a WORD or DWORD.
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableFlags(VALUETABLE *ptbl, UINT uValue)
{
	BOOL fPrevious = FALSE;

	while (ptbl->pszValue)
	{
		if (ptbl->nValue & uValue)
		{
			if (fPrevious)
			{
				MOUT << " | ";
			}
			else
			{
				fPrevious = TRUE;
			}

			MOUT << ptbl->pszValue;
			uValue &= ~ptbl->nValue;
		}

		ptbl++;
	}

	//
	// Was there nothing written or are there any bits that are left?
	//
	if (!fPrevious || uValue)
	{
		if (fPrevious)
		{
			MOUT << " | ";
		}

		if (uValue > 0xFFFF)
		{
			MOUT << (DWORD)uValue;
		}
		else
		{
			MOUT << (WORD)uValue;
		}
	}
}



//-----------------------------------------------------------------------------
// PrintIntArray
//
// Prints out a list of integers in an array separated by commas.
// If there was originally more integers than are available for
// display, it will add trailing ellipses on the end.
//
// Arguments:
//  PINT pInts	  - Pointer to an array of ints.
//  INT cInts	   - Ints in the pInts array.
//  INT cMaxInts	- The total original ints.  If this is larger than
//					cInts then the ellipses will be shown.
//
//-----------------------------------------------------------------------------

VOID PrintIntArray(PINT pInts, INT cInts, INT cMaxInts)
{
	int i;
	BOOL fPrevWritten = FALSE;

	for (i = 0; i < cInts; i++)
	{
		if (fPrevWritten)
		{
			MOUT << ',';
		}
		else
		{
			MOUT << ' ';
			fPrevWritten = TRUE;
		}

		MOUT << *pInts;
		pInts++;
	}

	if (cMaxInts > cInts)
	{
		MOUT << "...";
	}
}



//-----------------------------------------------------------------------------
// PrintFont
//
// Prints out the font name and point size given a pointer to
// a PACK_FONT structure.
//
// Arguments:
//  PPACK_FONT ppf - Pointer to a PACK_FONT structure.  If the nHeight field
//				   is -1, then the PACK_FONT structure is considered invalid
//				   and nothing is printed.
//
//-----------------------------------------------------------------------------

VOID PrintFont(PPACK_FONT ppf)
{
	if (ppf->nHeight != -1)
	{
		int nPointSize;
		int cyPixelsPerInch;
		HDC hdc;

		//
		// Calculate the point size given the pixels.
		//
		hdc = ::GetDC(theApp.m_pMainWnd->m_hWnd);
		cyPixelsPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(theApp.m_pMainWnd->m_hWnd, hdc);
		nPointSize = MulDiv(ppf->nHeight, 72, cyPixelsPerInch);

		MOUT << " (\"" << ppf->szFaceName << "\" " << nPointSize << ids(IDS_POINT_ABBREV) << ')';
	}
}



//-----------------------------------------------------------------------------
// ParmDecode
//
// The generic parameter decoder function.  This function is called to
// do the parameter decoding of all messages that do not have specific
// decoding that needs to be done (neither wParam nor lParam points to
// a string or a structure).
//
// Arguments:
//  Same as all parameter decoder functions.
//
//-----------------------------------------------------------------------------

DECODEPARM(Generic)
{
	P2WPOUT();
	P2LPOUT();
}



//-----------------------------------------------------------------------------
// TableOrValue2(...UINT uValue)
//
// Prints the value of uValue if found in the specified
// table, otherwise prints it out as either a DWORD or
// WORD value, depending on its size.
//
// Used for parameter decoding crackers only, NOT for message or
// return value crackers!
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableOrValue2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue)
{
	if (!(TableOrNothing2(pmps, ptbl, uValue)))
	{
		if (uValue > 0xFFFF)
		{
			*pmps << (DWORD)uValue;
		}
		else
		{
			*pmps << (WORD)uValue;
		}
	}
}



//-----------------------------------------------------------------------------
// TableOrDWord2
//
// Prints the value of uValue if found in the specified
// table, otherwise prints it out as a DWORD value.
//
// Used for parameter decoding crackers only, NOT for message or
// return value crackers!
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableOrDWord2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue)
{
	if (!(TableOrNothing2(pmps, ptbl, uValue)))
	{
		*pmps << (DWORD)uValue;
	}
}



//-----------------------------------------------------------------------------
// TableOrNothing2
//
// Prints the value of uValue if found in the specified table,
// otherwise does not print anything out and returns FALSE.
//
// Used for parameter decoding crackers only, NOT for message or
// return value crackers!
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

BOOL TableOrNothing2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue)
{
	while (ptbl->pszValue)
	{
		if (uValue == ptbl->nValue)
		{
			*pmps << ptbl->pszValue;
			return TRUE;
		}

		ptbl++;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// TableOrInt2
//
// Prints the value of uValue if found in the specified
// table, otherwise prints it out as an INT value.
//
// Used for parameter decoding crackers only, NOT for message or
// return value crackers!
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableOrInt2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue)
{
	if (!(TableOrNothing2(pmps, ptbl, uValue)))
	{
		*pmps << (INT)uValue;
	}
}



//-----------------------------------------------------------------------------
// TableFlags2
//
// Prints the flag values found in uValue that match the flags
// found in the specified table, separated by " | ".  If there
// are any bits remaining in the uValue parameter that are not
// accounted for after checking all the flags in the table, they
// will be printed out at the end as a WORD or DWORD.
//
// Used for parameter decoding crackers only, NOT for message or
// return value crackers!
//
// Arguments:
//
//
//
//-----------------------------------------------------------------------------

VOID TableFlags2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue)
{
	BOOL fPrevious = FALSE;

	while (ptbl->pszValue)
	{
		if (ptbl->nValue & uValue)
		{
			if (fPrevious)
			{
				*pmps << " | ";
			}
			else
			{
				fPrevious = TRUE;
			}

			*pmps << ptbl->pszValue;
			uValue &= ~ptbl->nValue;
		}

		ptbl++;
	}

	//
	// Was there nothing written or are there any bits that are left?
	//
	if (!fPrevious || uValue)
	{
		if (fPrevious)
		{
			*pmps << " | ";
		}

		if (uValue > 0xFFFF)
		{
			*pmps << (DWORD)uValue;
		}
		else
		{
			*pmps << (WORD)uValue;
		}
	}
}



//-----------------------------------------------------------------------------
// PrintIntArray2
//
// Prints out a list of integers in an array separated by commas.
// If there was originally more integers than are available for
// display, it will add trailing ellipses on the end.
//
// Used for parameter decoding crackers only, NOT for message or
// return value crackers!
//
// Arguments:
//  CMsgParmStream* pmps	- The stream to write to.
//  PINT pInts			  - Pointer to an array of ints.
//  INT cInts			   - Ints in the pInts array.
//  INT cMaxInts			- The total original ints.  If this is larger than
//							cInts then the ellipses will be shown.
//
//-----------------------------------------------------------------------------

VOID PrintIntArray2(CMsgParmStream* pmps, PINT pInts, INT cInts, INT cMaxInts)
{
	int i;
	BOOL fPrevWritten = FALSE;

	for (i = 0; i < cInts; i++)
	{
		if (fPrevWritten)
		{
			M2OUT << ", ";
		}
		else
		{
			fPrevWritten = TRUE;
		}

		M2OUT << *pInts;
		pInts++;
	}

	if (cMaxInts > cInts)
	{
		M2OUT << " ...";
	}
}
