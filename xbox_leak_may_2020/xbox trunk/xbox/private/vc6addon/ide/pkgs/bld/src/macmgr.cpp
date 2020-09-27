//////////////////////////////////////////////////////////////////////
// MACMGR.CPP
//
// Implementation of CMacList object
//
// History
// =======
// Date			Who			What
// ----			---			----
// 22-May-93	mattg		Created
// 07-Jul-93	mattg		Added to VSHELL
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Include files

#include "stdafx.h"
#pragma hdrstop
#include "macmgr.h"

//////////////////////////////////////////////////////////////////////
// IMPLEMENT_*
//
// These must be placed BEFORE the #ifdef _DEBUG stuff below

IMPLEMENT_DYNAMIC(CMacList, CObject)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Constructors, destructors

CMacList::~CMacList
(
)
{
}

//////////////////////////////////////////////////////////////////////
// CMacList::IsMacDefined

BOOL CMacList::IsMacDefined
(
	const TCHAR *	szMacName
)
const
{
	CString	strUnused;

	return(m_dict.Lookup(szMacName, strUnused));
}

//////////////////////////////////////////////////////////////////////
// CMacList::GetMacVal

const TCHAR * CMacList::GetMacVal
(
	const TCHAR *	szMacName
)
const
{
	if (!IsMacDefined(szMacName))
	{
		ASSERT(FALSE);
		return(NULL);
	}

	// What follows is a little strange.  We want this method to be 'const'
	// since it doesn't modify this object.  However, CMapStringToString's
	// operator [] is NOT const, since it could conceivably modify the CMap-
	// StringToString object if an association with the specified key doesn't
	// yet exist (it will create one and add it to its map).  In this case,
	// we KNOW the association exists (by virtue of calling IsMacDefined()
	// above), so we don't have to worry about this case.
	//
	// Anyway, the end result is that we have to cast m_dict to a (CMapString-
	// ToString *) to avoid a compiler error message.
	//
	// So why is this method using CMapStringToString::operator []?  Because
	// that's the only method of CMapStringToString that gives us a reference
	// to the map's INTERNAL CString value, which is what we want.

	return(((CMapStringToString *)&m_dict)->operator[](szMacName));
}

//////////////////////////////////////////////////////////////////////
// CMacList::ResolveString

VOID CMacList::ResolveString
(
	CString &	str
)
{
	INT		ichStartMac, ichEndMac, cch = 0;
	CString	strMacName, strT;

	/* Continue looping until all macro references have been resolved.
	*/

	while (((ichStartMac = str.Find(_T('$'))) != -1) && (ichStartMac != cch - 1))
	{
		cch = str.GetLength();

		/* We've found a '$', indicating some sort of macro.  If the
		** next character is '(', then everything between that parenthesis
		** and the closing ')' parenthesis constitutes the macro name.
		** Otherwise, the macro name is the next character.
		*/

		if (str[ichStartMac + 1] == _T('('))
		{
			ichEndMac = ichStartMac + 2;

			while ((ichEndMac < cch) && (str[ichEndMac] != _T(')')))
			{
#ifdef _MBCS
				if (IsDBCSLeadByte(str[ichEndMac]))
					++ichEndMac;
#endif

				++ichEndMac;
			}

			if (ichEndMac >= cch)
			{
				/* Couldn't find the closing ')'.  Leave the string alone
				** and return.
				*/

				return;
			}

			strMacName = str.Mid(ichStartMac + 2, ichEndMac - ichStartMac - 2);
		}
		else
		{
			ichEndMac = ichStartMac + 1;

			strMacName = str[ichStartMac + 1];

#ifdef _MBCS
			if ((IsDBCSLeadByte(str[ichStartMac + 1]) && (ichStartMac + 1 < cch - 1)))
				strMacName += str[ichStartMac + 2];
#endif
		}

		/* Now make a copy of the current string so we can split it apart to form
		** the new string.
		*/

		strT = str;

		str.Empty();

		/* Add the part of the string BEFORE the macro from the copy to the string.
		*/

		str += strT.Left(ichStartMac);

		/* Add the macro's value, if any, to the string.
		*/

		if (IsMacDefined(strMacName))
			str += GetMacVal(strMacName);

		/* Now add the part of the string AFTER the macro from the copy to the string.
		*/

		str += strT.Right(cch - ichEndMac - 1);
	}
}
