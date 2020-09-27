//==================================================================================
// Includes
//==================================================================================
#include <time.h>
#include <windows.h>

#include <stdlib.h>	// NT BUILD needs this for div_t
#ifdef _XBOX
#include "convhelp.h"
#endif

#include "main.h"
#ifndef NO_TNCOMMON_DEBUG_SPEW
#include "debugprint.h"
#endif // ! NO_TNCOMMON_DEBUG_SPEW

#include "strutils.h"






//==================================================================================
// URL encoding hash table.
//==================================================================================

#define URLHASH_SAFE	0x00 // character is safe in the normal sequence
#define URLHASH_ESCAPE	0x01 // character is valid in an escape sequence
#define URLHASH_UNSAFE	0x80 // character is unsafe all places


// This was taken from DNet which in turn was from some IE source code.
//
// s_SafetyHash - the list of characters above 0x20 and below 0x7f that are
// classified as safe, escape, or unsafe. Safe characters do not need to be
// escaped.  Escape characters are the only ones allowed in an escape sequence.
// Unsafe characters must be escaped to be used.

static BYTE		s_SafetyHash[] =
{
	// UNSAFE: 0x00..0x20

	URLHASH_SAFE,					// 0x21 (!)
	URLHASH_UNSAFE,					// 0x22 (")
	URLHASH_SAFE,					// 0x23 (#)
	URLHASH_SAFE,					// 0x24 ($)
	URLHASH_UNSAFE,					// 0x25 (%)
	URLHASH_SAFE,					// 0x26 (&)
	URLHASH_SAFE,					// 0x27 (')
	URLHASH_SAFE,					// 0x28 (()
	URLHASH_SAFE,					// 0x29 ())
	URLHASH_SAFE,					// 0x2A (*)
	URLHASH_SAFE,					// 0x2B (+)
	URLHASH_SAFE,					// 0x2C (,)
	URLHASH_SAFE,					// 0x2D (-)
	URLHASH_SAFE,					// 0x2E (.)
	URLHASH_SAFE,					// 0x2F (/)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x30 (0)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x31 (1)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x32 (2)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x33 (3)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x34 (4)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x35 (5)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x36 (6)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x37 (7)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x38 (8)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x39 (9)
	URLHASH_SAFE,					// 0x3A (:)
	URLHASH_SAFE,					// 0x3B (;)
	URLHASH_UNSAFE,					// 0x3C (<)
	URLHASH_SAFE,					// 0x3D (=)
	URLHASH_UNSAFE,					// 0x3E (>)
	URLHASH_UNSAFE,					// 0x3F (?)
	URLHASH_SAFE,					// 0x40 (@)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x41 (A)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x42 (B)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x43 (C)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x44 (D)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x45 (E)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x46 (F)
	URLHASH_SAFE,					// 0x47 (G)
	URLHASH_SAFE,					// 0x48 (H)
	URLHASH_SAFE,					// 0x49 (I)
	URLHASH_SAFE,					// 0x4A (J)
	URLHASH_SAFE,					// 0x4B (K)
	URLHASH_SAFE,					// 0x4C (L)
	URLHASH_SAFE,					// 0x4D (M)
	URLHASH_SAFE,					// 0x4E (N)
	URLHASH_SAFE,					// 0x4F (O)
	URLHASH_SAFE,					// 0x50 (P)
	URLHASH_SAFE,					// 0x51 (Q)
	URLHASH_SAFE,					// 0x42 (R)
	URLHASH_SAFE,					// 0x43 (S)
	URLHASH_SAFE,					// 0x44 (T)
	URLHASH_SAFE,					// 0x45 (U)
	URLHASH_SAFE,					// 0x46 (V)
	URLHASH_SAFE,					// 0x47 (W)
	URLHASH_SAFE,					// 0x48 (X)
	URLHASH_SAFE,					// 0x49 (Y)
	URLHASH_SAFE,					// 0x5A (Z)
	URLHASH_UNSAFE,					// 0x5B ([)
	URLHASH_UNSAFE,					// 0x5C (\)
	URLHASH_UNSAFE,					// 0x5D (])
	URLHASH_UNSAFE,					// 0x5E (^)
	URLHASH_SAFE,					// 0x5F (_)
	URLHASH_UNSAFE,					// 0x60 (`)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x61 (a)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x62 (b)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x63 (c)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x64 (d)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x65 (e)
	URLHASH_SAFE | URLHASH_ESCAPE,	// 0x66 (f)
	URLHASH_SAFE,					// 0x67 (g)
	URLHASH_SAFE,					// 0x68 (h)
	URLHASH_SAFE,					// 0x69 (i)
	URLHASH_SAFE,					// 0x6A (j)
	URLHASH_SAFE,					// 0x6B (k)
	URLHASH_SAFE,					// 0x6C (l)
	URLHASH_SAFE,					// 0x6D (m)
	URLHASH_SAFE,					// 0x6E (n)
	URLHASH_SAFE,					// 0x6F (o)
	URLHASH_SAFE,					// 0x70 (p)
	URLHASH_SAFE,					// 0x71 (q)
	URLHASH_SAFE,					// 0x72 (r)
	URLHASH_SAFE,					// 0x73 (s)
	URLHASH_SAFE,					// 0x74 (t)
	URLHASH_SAFE,					// 0x75 (u)
	URLHASH_SAFE,					// 0x76 (v)
	URLHASH_SAFE,					// 0x77 (w)
	URLHASH_SAFE,					// 0x78 (x)
	URLHASH_SAFE,					// 0x79 (y)
	URLHASH_SAFE,					// 0x7A (z)
	URLHASH_UNSAFE,					// 0x7B ({)
	URLHASH_UNSAFE,					// 0x7C (|)
	URLHASH_UNSAFE,					// 0x7D (})
	URLHASH_UNSAFE					// 0x7E (~)

	// UNSAFE: 0x7F..0xFF
};








#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringStartsWith()"
//==================================================================================
// StringStartsWith
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the first string equals or begins with the second
//				string, FALSE otherwise.
//
// Arguments:
//	char* szString		String to check.
//	char* szPrefix		Prefix to check for.
//	BOOL fMatchCase		Whether case is important or not.
//
// Returns: TRUE if the string starts with the prefix, FALSE otherwise.
//==================================================================================
BOOL StringStartsWith(char* szString, char* szPrefix, BOOL fMatchCase)
{
	BOOL	fResult = FALSE;
	DWORD	dwPartialStringLen = 0;
	char*	pszFullStringStart = NULL;
	char*	pszPartialString = NULL;
	

	dwPartialStringLen = strlen(szPrefix);

	// If the string we're testing is shorter than the prefix, it can't
	// possibly begin with the substring.
	if (strlen(szString) < dwPartialStringLen)
		goto DONE;
	
	pszFullStringStart = (char*) LocalAlloc(LPTR, (dwPartialStringLen + 1));
	if (pszFullStringStart == NULL) // out of memory
		goto DONE;

	CopyMemory(pszFullStringStart, szString, dwPartialStringLen);
	
	// Terminate the temp string copy
	pszFullStringStart[dwPartialStringLen] = '\0';

	if (fMatchCase)
	{
		fResult = (strcmp(pszFullStringStart, szPrefix) == 0) ? TRUE : FALSE;
	} // end if (case is important)
	else
	{
		pszPartialString = (char*) LocalAlloc(LPTR, (dwPartialStringLen + 1));
		if (pszPartialString == NULL) // out of memory
			goto DONE;

		CopyMemory(pszPartialString, szPrefix, dwPartialStringLen + 1);

		_strlwr(pszFullStringStart);
		_strlwr(pszPartialString);

		fResult = (strcmp(pszFullStringStart, pszPartialString) == 0);
	} // end else (case is not important)
	

DONE:

	if (pszFullStringStart != NULL)
	{
		LocalFree(pszFullStringStart);
		pszFullStringStart = NULL;
	} // end if (allocated memory)

	if (pszPartialString != NULL)
	{
		LocalFree(pszPartialString);
		pszPartialString = NULL;
	} // end if (allocated memory)

	return (fResult);
} // StringStartsWith
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringEndsWith()"
//==================================================================================
// StringEndsWith
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the first string equals or ends with the second
//				string, FALSE otherwise.
//
// Arguments:
//	char* szString		String to check.
//	char* szSuffix		Suffix to check for.
//	BOOL fMatchCase		Whether case is important or not.
//
// Returns: TRUE if the string ends with the suffix, FALSE otherwise.
//==================================================================================
BOOL StringEndsWith(char* szString, char* szSuffix, BOOL fMatchCase)
{
	BOOL	fResult = FALSE;
	DWORD	dwPartialStringLen = 0;
	char*	pszFullStringEnd = NULL;
	char*	pszPartialString = NULL;
	
	
	dwPartialStringLen = strlen(szSuffix);

	// If the string we're testing is shorter than the suffix, it can't
	// possibly end with the substring.
	if (strlen(szString) < dwPartialStringLen)
		goto DONE;
	

	if (fMatchCase)
	{
		pszFullStringEnd = (szString + strlen(szString) - dwPartialStringLen);
		fResult = (strcmp(pszFullStringEnd, szSuffix) == 0);
		pszFullStringEnd = NULL; // don't try to free this memory, it was just a temp pointer
	} // end if (case is important)
	else
	{
		pszFullStringEnd = (char*) LocalAlloc(LPTR, (dwPartialStringLen + 1));
		if (pszFullStringEnd == NULL) // out of memory
			goto DONE;

		CopyMemory(pszFullStringEnd, (szString + strlen(szString) - dwPartialStringLen),
				dwPartialStringLen + 1);

		pszPartialString = (char*) LocalAlloc(LPTR, (dwPartialStringLen + 1));
		if (pszPartialString == NULL) // out of memory
			goto DONE;

		CopyMemory(pszPartialString, szSuffix, dwPartialStringLen + 1);

		_strlwr(pszFullStringEnd);
		_strlwr(pszPartialString);

		fResult = (strcmp(pszFullStringEnd, pszPartialString) == 0) ? TRUE : FALSE;
	} // end else (case is not important)
	

DONE:

	if (pszFullStringEnd != NULL)
	{
		LocalFree(pszFullStringEnd);
		pszFullStringEnd = NULL;
	} // end if (allocated memory)

	if (pszPartialString != NULL)
	{
		LocalFree(pszPartialString);
		pszPartialString = NULL;
	} // end if (allocated memory)

	return (fResult);
} // StringEndsWith
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringContainsChar()"
//==================================================================================
// StringContainsChar
//----------------------------------------------------------------------------------
//
// Description: Returns -1 if the passed string doesn't contain the char, otherwise
//				it returns the zero based position the first instance of the char
//				was found.  This checking starts at the value passed for iPos, so
//				pass zero if you want it to start at the beginning of the string. 
//
// Arguments:
//	char* szString		String to search.
//	char cCharToFind	Character to search for.
//	BOOL fMatchCase		Whether the character's case is important or not.
//	int iPos			Offset in string to begin looking
//
// Returns: The index of the char, or -1 if not found.
//==================================================================================
int StringContainsChar(char* szString, char cCharToFind, BOOL fMatchCase, int iPos)
{
	if (szString == NULL)
		return (-1);

	// (int) is to get rid of compiler warning
	if (iPos >= (int) strlen(szString))
		return (-1);

	if (iPos < 0)
		iPos = 0;

	if (cCharToFind == '\0')
		return (-1);

	if (fMatchCase)
	{
		while((szString[iPos] != '\0') && (szString[iPos] != cCharToFind))
			iPos++;
	} // end if (we have to match the case)
	else
	{
		char*	pszLowerString;
		char	szTemp[2];


		pszLowerString = (char*) LocalAlloc(LPTR, strlen(szString) + 1);
		if (pszLowerString == NULL)
			return (-1);

		strcpy(pszLowerString, szString);
		_strlwr(pszLowerString);


		szTemp[0] = cCharToFind;
		szTemp[1] = '\0';
		_strlwr(szTemp);

		while((pszLowerString[iPos] != '\0') &&
				(pszLowerString[iPos] != szTemp[0]))
		{
			iPos++;
		} // end while (not at end of string and not case-insensitive character)

		LocalFree(pszLowerString);
		pszLowerString = NULL;
	} // end else (we don't have to match the case)

	if (szString[iPos] == '\0')	// if we didn't find it
		return (-1);	// failure
	
	return (iPos);
} // StringContainsChar
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringCountNumChars()"
//==================================================================================
// StringCountNumChars
//----------------------------------------------------------------------------------
//
// Description: Returns the number of times the string contains the passed in
//				character, or 0 if none. 
//
// Arguments:
//	char* szString		String to search.
//	char cCharToFind	Character to search for.
//	BOOL fMatchCase		Whether the character's case is important or not.
//
// Returns: The index of the char, or -1 if not found.
//==================================================================================
int StringCountNumChars(char* szString, char cCharToFind, BOOL fMatchCase)
{
	int				iCount = 0;
	unsigned int	ui;


	if (szString == NULL)
		return (-1);

	if (fMatchCase)
	{
		for(ui = 0; ui < strlen(szString); ui++)
		{
			if (szString[ui] == cCharToFind)
				iCount++;
		} // end for (each character in the string)
	} // end if (we have to match the case)
	else
	{
		char*	pszLowerString;
		char	szTemp[2];


		pszLowerString = (char*) LocalAlloc(LPTR, strlen(szString) + 1);
		if (pszLowerString == NULL)
			return (-1);

		strcpy(pszLowerString, szString);
		_strlwr(pszLowerString);


		szTemp[0] = cCharToFind;
		szTemp[1] = '\0';
		_strlwr(szTemp);


		for(ui = 0; ui < strlen(szString); ui++)
		{
			if (pszLowerString[ui] == szTemp[0])
				iCount++;
		} // end for (each character in the string)
	} // end else (we don't have to match the case)
	
	return (iCount);
} // StringCountNumChars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringContainsOneOfChars()"
//==================================================================================
// StringContainsOneOfChars
//----------------------------------------------------------------------------------
//
// Description: Returns -1 if the passed string doesn't contain any of the
//				characters in the szCharsToFind string, otherwise it returns the
//				zero based position of the first char found.  This checking starts
//				at the value passed for iPos, so pass zero if you want it to start
//				at the beginning of the string. 
//
// Arguments:
//	char* szString			String to search.
//	char* szCharsToFind		Characters to search for.
//	BOOL fMatchCase			Whether the characters' cases are important or not.
//	int iPos				Offset in string to begin looking
//
// Returns: The index of the char, or -1 if not found.
//==================================================================================
int StringContainsOneOfChars(char* szString, char* szCharsToFind, BOOL fMatchCase,
							int iPos)
{
	int		iFindCharPos;


	if (szString == NULL)
		return (-1);

	// (int) is to get rid of compiler warning
	if (iPos >= (int) strlen(szString))
		return (-1);

	if (iPos < 0)
		iPos = 0;

	if ((szCharsToFind == NULL) || (szCharsToFind[0] == '\0'))
		return (-1);

	if (fMatchCase)
	{
		while (szString[iPos] != '\0')
		{
			iFindCharPos = 0;

			while (szCharsToFind[iFindCharPos] != '\0')
			{
				if (szString[iPos] != szCharsToFind[iFindCharPos])
					return (iPos);

				iFindCharPos++;
			} // end while (still more chars to search for)
			iPos++;
		} // end while (not at the end of the string)
	} // end if (we have to match the case)
	else
	{
		char*	pszLowerString;
		char*	pszLowerCharsToFind;


		pszLowerString = (char*) LocalAlloc(LPTR, strlen(szString) + 1);
		if (pszLowerString == NULL)
			return (-1);

		strcpy(pszLowerString, szString);
		_strlwr(pszLowerString);


		pszLowerCharsToFind = (char*) LocalAlloc(LPTR, strlen(szCharsToFind) + 1);
		if (pszLowerCharsToFind == NULL)
		{
			LocalFree(pszLowerString);
			pszLowerString = NULL;
			return (-1);
		} // end if (couldn't allocate memory)

		strcpy(pszLowerCharsToFind, szCharsToFind);
		_strlwr(pszLowerCharsToFind);


		while (pszLowerString[iPos] != '\0')
		{
			iFindCharPos = 0;

			while (pszLowerCharsToFind[iFindCharPos] != '\0')
			{
				if (pszLowerString[iPos] != pszLowerCharsToFind[iFindCharPos])
				{
					LocalFree(pszLowerString);
					pszLowerString = NULL;
					LocalFree(pszLowerCharsToFind);
					pszLowerCharsToFind = NULL;
					return (iPos);
				} // end if (couldn't allocate memory)

				iFindCharPos++;
			} // end while (still more chars to search for)
			iPos++;
		} // end while (not at the end of the string)

		LocalFree(pszLowerString);
		pszLowerString = NULL;
		LocalFree(pszLowerCharsToFind);
		pszLowerCharsToFind = NULL;
	} // end else (we don't have to match the case)


	// If we got here, we failed.

	return (-1);
} // StringContainsOneOfChars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringContainsNonChar()"
//==================================================================================
// StringContainsNonChar
//----------------------------------------------------------------------------------
//
// Description: Returns -1 if the passed string contains only characters in the
//				szCharsToFind string, otherwise it returns the zero based position
//				of the first char found that is not.  This checking starts at the
//				value passed for iPos, so pass zero if you want it to start at the
//				beginning of the string. 
//
// Arguments:
//	char* szString			String to search.
//	char* szCharsAllowed	Characters that are accepted.
//	BOOL fMatchCase			Whether the characters' cases are important or not.
//	int iPos				Offset in string to begin looking
//
// Returns: The index of the char, or -1 if not found.
//==================================================================================
int StringContainsNonChar(char* szString, char* szCharsAllowed, BOOL fMatchCase,
						int iPos)
{
	int		iAllowedCharPos;


	if (szString == NULL)
		return (-1);

	// (int) is to get rid of compiler warning
	if (iPos >= (int) strlen(szString))
		return (-1);

	if (iPos < 0)
		iPos = 0;

	if ((szCharsAllowed == NULL) || (szCharsAllowed[0] == '\0'))
		return (-1);

	if (fMatchCase)
	{
		while (szString[iPos] != '\0')
		{
			iAllowedCharPos = 0;

			while (szCharsAllowed[iAllowedCharPos] != '\0')
			{
				if (szString[iPos] == szCharsAllowed[iAllowedCharPos])
					break;

				iAllowedCharPos++;
			} // end while (still more chars to search for)

			if (szCharsAllowed[iAllowedCharPos] == '\0')
				return (iPos);

			iPos++;
		} // end while (not at the end of the string)
	} // end if (we have to match the case)
	else
	{
		char*	pszLowerString;
		char*	pszLowerAllowedChars;


		pszLowerString = (char*) LocalAlloc(LPTR, strlen(szString) + 1);
		if (pszLowerString == NULL)
			return (-1);

		strcpy(pszLowerString, szString);
		_strlwr(pszLowerString);


		pszLowerAllowedChars = (char*) LocalAlloc(LPTR, strlen(szCharsAllowed) + 1);
		if (pszLowerAllowedChars == NULL)
		{
			LocalFree(pszLowerString);
			pszLowerString = NULL;
			return (-1);
		} // end if (couldn't allocate memory)

		strcpy(pszLowerAllowedChars, szCharsAllowed);
		_strlwr(pszLowerAllowedChars);


		while (pszLowerString[iPos] != '\0')
		{
			iAllowedCharPos = 0;

			while (pszLowerAllowedChars[iAllowedCharPos] != '\0')
			{
				if (pszLowerString[iPos] == pszLowerAllowedChars[iAllowedCharPos])
					break;

				iAllowedCharPos++;
			} // end while (still more chars to search for)

			if (szCharsAllowed[iAllowedCharPos] == '\0')
			{
				LocalFree(pszLowerString);
				pszLowerString = NULL;
				LocalFree(pszLowerAllowedChars);
				pszLowerAllowedChars = NULL;
				return (iPos);
			} // end if (got to end of allowed characters string)

			iPos++;
		} // end while (not at the end of the string)

		LocalFree(pszLowerString);
		pszLowerString = NULL;
		LocalFree(pszLowerAllowedChars);
		pszLowerAllowedChars = NULL;
	} // end else (we don't have to match the case)


	// If we got here, we didn't find any non-allowed characters.

	return (-1);
} // StringContainsNonChar
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringCmpNoCase()"
//==================================================================================
// StringCmpNoCase
//----------------------------------------------------------------------------------
//
// Description: Performs similar to strcmp, except the case does not need to match.
//
// Arguments:
//	char* szString1		First string to use.
//	char* szString2		Second string to use.
//
// Returns: Returns TRUE if they match (case insensitive), FALSE otherwise.
//==================================================================================
BOOL StringCmpNoCase(char* szString1, char* szString2)
{
	BOOL	fResult = FALSE;
	DWORD	dwString1Len;
	DWORD	dwString2Len;
	char*	pszTemp1 = NULL;
	char*	pszTemp2 = NULL;


	dwString1Len = strlen(szString1);
	dwString2Len = strlen(szString2);

	// The sizes are wrong, then we don't even need to bother checking more
	if (dwString1Len != dwString2Len)
		goto DONE;

	pszTemp1 = (char*) LocalAlloc(LPTR, (strlen(szString1) + 1));
	if (pszTemp1 == NULL)
	{
		goto DONE;
	} // end if (couldn't allocate memory)

	CopyMemory(pszTemp1, szString1, dwString1Len);
	_strlwr(pszTemp1);


	pszTemp2 = (char*) LocalAlloc(LPTR, (strlen(szString2) + 1));
	if (pszTemp2 == NULL)
	{
		goto DONE;
	} // end if (couldn't allocate memory)
	CopyMemory(pszTemp2, szString2, dwString2Len);
	_strlwr(pszTemp2);


	// Compare lower case strings
	fResult = (strcmp(pszTemp1, pszTemp2) == 0) ? TRUE : FALSE;


DONE:

	if (pszTemp1 != NULL)
	{
		LocalFree(pszTemp1);
		pszTemp1 = NULL;
	} // end if (allocated first temp item)

	if (pszTemp2 != NULL)
	{
		LocalFree(pszTemp2);
		pszTemp2 = NULL;
	} // end if (allocated second temp item)

	return (fResult);
} // StringCmpNoCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringMatchesWithWildcard()"
//==================================================================================
// StringMatchesWithWildcard
//----------------------------------------------------------------------------------
//
// Description: Compares a string to one with wildcard characters (optionally case
//				sensitive), and returns TRUE if they match.
//				Wildcard characters are:
//					'*' - Any number of characters or none matches.
//					'?' - A single character matches.
//					'~' - Only allowed as first character of string.  Inverts any
//							results, so that if the string matches the rest of the
//							wildcard string, FALSE is returned, and if it does not
//							match, TRUE is returned.
//
// Arguments:
//	char* szWildcardString		String possibly containing wildcard characters.
//	char* szCompareString		Plain string to compare.
//	BOOL fMatchCase				Whether case is important or not.
//
// Returns: Returns TRUE if they match, FALSE otherwise.
//==================================================================================
BOOL StringMatchesWithWildcard(char* szWildcardString, char* szCompareString,
								BOOL fMatchCase)
{
	char*	pszTempCompare = NULL;
	char*	pszTempWildcard = NULL;
	BOOL	fResult = FALSE;
	BOOL	fInvert = FALSE;
	int		iStarPos = 0;
	int		iCurrentComparePos = 0;
	int		iNextStar;
	int		iQuestionPos;


	//DPL(8, "\"%s\" \"%s\"", 2, szWildcardString, szCompareString);



	if (szWildcardString[0] == '~')
	{
		fInvert = TRUE;
		// It's okay to bump this up, since we're working with a local copy of the
		// pointer.
		szWildcardString++;
	} // end if (should invert match)


	// We need a temporary buffer for comparisons, since our algorithm for ?
	// matching is destructive.
	pszTempCompare = (char*) LocalAlloc(LPTR, strlen(szCompareString) + 1);
	if (pszTempCompare == NULL)
	{
		fInvert = FALSE; // this ensures we return FALSE
		goto DONE;
	} // end if (couldn't allocate memory)


	// We'll never need anything bigger than the wildcard string, so just allocate
	// a buffer big enough for that.
	pszTempWildcard = (char*) LocalAlloc(LPTR, strlen(szWildcardString) + 1);
	if (pszTempWildcard == NULL)
	{
		fInvert = FALSE; // this ensures we return FALSE
		goto DONE;
	} // end if (couldn't allocate memory)


	/// Loop until we completely match or fail to match at some point.
	do
	{
		iNextStar = StringContainsChar(szWildcardString, '*', TRUE, iStarPos);

		if (iNextStar < 0)
		{
			if (strlen(szCompareString) >= strlen(szWildcardString + iStarPos))
			{
				strcpy(pszTempWildcard, szWildcardString + iStarPos);
				strcpy(pszTempCompare, szCompareString + strlen(szCompareString) - strlen(pszTempWildcard));

				// Loop through the section of the string to compare and convert all
				// characters in the temporary comparison buffer where there was a
				// question mark in the wildcard string into question marks too.  This
				// will cause the string comparison routine to match.
				iQuestionPos = 0;
				do
				{
					iQuestionPos = StringContainsChar(pszTempWildcard, '?', TRUE, iQuestionPos);

					// No more question marks?  Okay, we're done.
					if (iQuestionPos < 0)
						break;

					// This is destructive, but that's what we have the temp buffer
					// for.
					pszTempCompare[iQuestionPos] = '?';

					iQuestionPos++; // move past this question mark
				} // end do (while there are question marks)
				while (TRUE);


				// If the string doesn't end with this part, it's a failure.
				if (fMatchCase)
				{
					fResult = (strcmp(pszTempCompare, pszTempWildcard) == 0) ? TRUE : FALSE;
				} // end if (should match case)
				else
				{
					fResult = StringCmpNoCase(pszTempCompare, pszTempWildcard);
				} // end else (don't match case)
			} // end if (string is long enough)

			break;
		} // end if (didn't find another star)

		if (szWildcardString[iNextStar + 1] == '*')
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Two *'s in a row is illegal (\"%s\"), failing!",
				1, szWildcardString);
			#endif // NO_TNCOMMON_DEBUG_SPEW

			fInvert = FALSE; // this ensures we return FALSE

			break;
		} // end if (user had to stars in a row)

		// Check if the first character is a star.
		if (iNextStar == 0)
		{
			// Special case: having only one star, and it being at the beginning
			if (StringContainsChar(szWildcardString, '*', TRUE, 1) < 0)
			{
				// The string is just "*", so match everything
				if (szWildcardString[1] == '\0')
				{
					fResult = TRUE;
					break;
				} // end if (star is entire string)

				strcpy(pszTempWildcard, szWildcardString + 1);
				strcpy(pszTempCompare, szCompareString + strlen(szCompareString) - strlen(pszTempWildcard));

				// Loop through the section of the string to compare and convert all
				// characters in the temporary comparison buffer where there was a
				// question mark in the wildcard string into question marks too.  This
				// will cause the string comparison routine to match.
				iQuestionPos = 0;
				do
				{
					iQuestionPos = StringContainsChar(pszTempWildcard, '?', TRUE, iQuestionPos);

					// No more question marks?  Okay, we're done.
					if (iQuestionPos < 0)
						break;

					// This is destructive, but that's what we have the temp buffer
					// for.
					pszTempCompare[iQuestionPos] = '?';

					iQuestionPos++; // move past this question mark
				} // end do (while there are question marks)
				while (TRUE);


				// If the string doesn't end with this part, it's a failure.
				if (fMatchCase)
				{
					fResult = (strcmp(pszTempCompare, pszTempWildcard) == 0) ? TRUE : FALSE;
				} // end if (should match case)
				else
				{
					fResult = StringCmpNoCase(pszTempCompare, pszTempWildcard);
				} // end else (don't match case)

				break;
			} // end if (no more stars)

			// Otherwise, just move on

		} // end if (started in star, and this is it)
		else
		{
			// If the compare string isn't even long enough to start with this
			// segment, it can't match.  The (int) is just to avoid the naive
			// compiler warning.
			if ((int) strlen(szCompareString) < iNextStar - iStarPos)
			{
				break;
			} // end if (there's not enough room)

			CopyMemory(pszTempWildcard, szWildcardString + iStarPos,
						iNextStar - iStarPos);
			pszTempWildcard[iNextStar - iStarPos] = '\0'; // make sure the string ends


			// Handle not starting with a star.
			if (iStarPos == 0)
			{
				CopyMemory(pszTempCompare, szCompareString,
							iNextStar - iStarPos);
				pszTempCompare[iNextStar - iStarPos] = '\0'; // make sure the string ends


				// Loop through the section of the string to compare and convert
				// all characters in the temporary comparison buffer where there
				// was a question mark in the wildcard string into question marks
				// too.  This will cause the string comparison routine to match.

				iQuestionPos = 0;
				do
				{
					iQuestionPos = StringContainsChar(pszTempWildcard, '?', TRUE, iQuestionPos);

					// No more question marks?  Okay, we're done.
					if (iQuestionPos < 0)
						break;

					// This is destructive, but that's what we have the temp buffer
					// for.
					pszTempCompare[iQuestionPos] = '?';

					iQuestionPos++; // skip to the next character
				} // end do (while there are question marks)
				while (TRUE);

				// If these first segments don't match, the whole thing doesn't.
				if (fMatchCase)
				{
					if (strcmp(pszTempCompare, pszTempWildcard) != 0)
						break;
				} // end if (should match case)
				else
				{
					if (! StringCmpNoCase(pszTempCompare, pszTempWildcard))
						break;
				} // end else (don't match case)
			} // end if (this is the first star)
			else
			{
				//BUGBUG This probably could still be improved.  I haven't thought
				//		 it through, but I have this nagging feeling like the
				//		 logic here could be tricked.
				BOOL	fMatch;
				int		iStringLen;
				int		iSubStringLen;
				int		i;
				int		iDiffOffset;
				char*	pcCurrentString;
				char*	pcCurrentSubstring;
				

				strcpy(pszTempCompare, szCompareString + iCurrentComparePos);

				iStringLen = strlen(pszTempCompare);
				iSubStringLen = strlen(pszTempWildcard);


				if (! fMatchCase)
				{
					_strlwr(pszTempCompare);
					_strlwr(pszTempWildcard);
				} // end else (case isn't important)

				// Initialize the offset pointers.
				pcCurrentString = pszTempCompare;
				pcCurrentSubstring = pszTempWildcard;



				i = 0;
				fMatch = FALSE;
				// Try starting at every character in the string up to the length of
				// the string minus the length of the substring.  We can stop at
				// that point because if we haven't gotten into the matching
				// characters (inner) while loop, we'll never be able to succeed;
				// we'll hit the end of the main string before hitting the end of
				// the sub string.
				while (i <= iStringLen - iSubStringLen)
				{
					// Reset our current offset so if we travel anywhere in the
					// inner while loop, we can go back that many characters if it
					// ends up failing to match.
					iDiffOffset = 0;

					// Question marks in the substring (wildcard) count as a
					// matched character...
					while (((*pcCurrentString) == (*pcCurrentSubstring)) ||
							((*pcCurrentSubstring) == '?'))
					{
						pcCurrentString++;
						pcCurrentSubstring++;
						iDiffOffset++;

						// If that put us at the end of the substring, we're done.
						if ((*pcCurrentSubstring) == '\0')
						{
							// Move the compare spot after this found substring
							iCurrentComparePos += i + strlen(pszTempWildcard);
							fMatch = TRUE;
							break;
						} // end if (they match completely)
					} // end while (the current characters still match)

					if (fMatch)
						break;

					// If we got here, we didn't match the rest of the string, so reset it
					pcCurrentString -= iDiffOffset;
					pcCurrentSubstring -= iDiffOffset; // should put us back to 0 offset

					i++;
					pcCurrentString++; // move to next character to start
				} // end while (we aren't at the end of the usable string)

				// If we didn't find a match, we're done.
				if (! fMatch)
					break;
			} // end else (this is not the first star)

			// If there's nothing after this star, then we match.
			if (szWildcardString[iNextStar + 1] == '\0')
			{
				fResult = TRUE;
				break;
			} // end if (no more string left)
		} // end else (not looking at starting star)

		iStarPos = iNextStar + 1;
	} // end do (while haven't finished match or failed at some point)
	while (TRUE);


DONE:

	if (pszTempWildcard != NULL)
	{
		LocalFree(pszTempWildcard);
		pszTempWildcard = NULL;
	} // end if (allocated buffer)

	if (pszTempCompare != NULL)
	{
		LocalFree(pszTempCompare);
		pszTempCompare = NULL;
	} // end if (allocated buffer)


	// If we have to invert the result, do it,
	if (fInvert)
		fResult = (fResult) ? FALSE : TRUE;

	return (fResult);
} // StringMatchesWithWildcard
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringContainsString()"
//==================================================================================
// StringContainsString
//----------------------------------------------------------------------------------
//
// Description: Returns the index of the place where szSubstring begins (starting
//				the search at index iPos) inside szString, -1 otherwise. 
//
// Arguments:
//	char* szString		The string to search.
//	char* szSubstring	Character sequence to look for.
//	BOOL fMatchCase		Whether case is important or not.
//	int iPos			Location to start looking (0 for beginning).
//
// Returns: The starting location of the string or -1 if not found.
//==================================================================================
int StringContainsString(char* szString, char* szSubstring, BOOL fMatchCase, int iPos)
{
	int		iResult = -1;
	int		iStringLen;
	int		iSubStringLen;
	char*	pszLowerString = NULL;
	char*	pszLowerSubstring = NULL;
	int		i;
	int		iDiffOffset;
	char*	pcCurrentString;
	char*	pcCurrentSubstring;
	
	
	iStringLen = strlen(szString);
	iSubStringLen = strlen(szSubstring);

	// If the remainder of string we're testing is shorter than the substring, it
	// can't possibly contain the substring.
	if ((iStringLen - iPos) < iSubStringLen)
		goto DONE;
	

	// None of this empty substring stuff.
	if (iSubStringLen < 1)
		goto DONE;


	if (fMatchCase)
	{
		// Initialize the offset pointers.
		pcCurrentString = szString + iPos;
		pcCurrentSubstring = szSubstring;
	} // end if (case is important)
	else
	{
		// Copy the lower case main string
		pszLowerString = (char*) LocalAlloc(LPTR, (iStringLen - iPos + 1));
		if (pszLowerString == NULL)
			goto DONE;

		CopyMemory(pszLowerString, szString + iPos, iStringLen - iPos + 1);

		_strlwr(pszLowerString);


		// Copy the lower case sub string
		pszLowerSubstring = (char*) LocalAlloc(LPTR, (iSubStringLen + 1));
		if (pszLowerSubstring == NULL)
			goto DONE;

		CopyMemory(pszLowerSubstring, szSubstring, iSubStringLen + 1);

		_strlwr(pszLowerSubstring);


		// Initialize the offset pointers.
		pcCurrentString = pszLowerString;
		pcCurrentSubstring = pszLowerSubstring;
	} // end else (case isn't important)



	i = 0;
	// Try starting at every character in the string up to the length of the string
	// minus the length of the substring.  We can stop at that point because if we
	// haven't gotten into the matching characters (inner) while loop, we'll never
	// be able to succeed; we'll hit the end of the main string before hitting the
	// end of the sub string.
	while (i <= iStringLen - iPos - iSubStringLen)
	{
		// Reset our current offset so if we travel anywhere in the inner while
		// loop, we can go back that many characters if it ends up failing to match.
		iDiffOffset = 0;

		while ((*pcCurrentString) == (*pcCurrentSubstring))
		{
			pcCurrentString++;
			pcCurrentSubstring++;
			iDiffOffset++;

			// If that put us at the end of the substring, we're done.
			if ((*pcCurrentSubstring) == '\0')
			{
				iResult = i + iPos;
				goto DONE;
			} // end if (they match completely)
		} // end while (the current characters still match)

		// If we got here, we didn't match the rest of the string, so reset it
		pcCurrentString -= iDiffOffset;
		pcCurrentSubstring -= iDiffOffset; // should put us back to 0 offset

		i++;
		pcCurrentString++; // move to next character to start
	} // end while (we aren't at the end of the usable string)

DONE:

	if (pszLowerString != NULL)
	{
		LocalFree(pszLowerString);
		pszLowerString = NULL;
	} // end if (there's a lower case string)

	if (pszLowerSubstring != NULL)
	{
		LocalFree(pszLowerSubstring);
		pszLowerSubstring = NULL;
	} // end if (there's a lower case substring)
	
	return (iResult);
} // StringContainsString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringIsTrue()"
//==================================================================================
// StringIsTrue
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the string is not all "0"'s, "false" or "off",
//				case-insensitive.  Otherwise this returns FALSE.
//
// Arguments:
//	char* szString	String to check.
//
// Returns: TRUE if the string is anything but the textual representations of FALSE.
//==================================================================================
BOOL StringIsTrue(char* szString)
{
	BOOL	fResult = FALSE;
	char*	pszTemp = NULL;
	char*	pcCurrent = NULL;



	pszTemp = (char*) LocalAlloc(LPTR, (strlen(szString) + 1));
	if (pszTemp == NULL)
		goto DONE;

	strcpy(pszTemp, szString);

	// Make the string lower case
	_strlwr(pszTemp);

	if (strcmp(pszTemp, "false") == 0)
		goto DONE;

	if (strcmp(pszTemp, "off") == 0)
		goto DONE;

	// Search it to see if it's all zeros
	pcCurrent = pszTemp;
	while ((*pcCurrent) == '0')
	{
		pcCurrent++;

		// If moving put us at the end of the string, it contains all zeros,
		// so we return false
		if ((*pcCurrent) == '\0')
			goto DONE;
	} // end while (the current character is a zero)


	// If we got here, we didn't contain any of the textual representations of
	// FALSE, and there were no errors.  So let's go.
	fResult = TRUE;


DONE:

	if (pszTemp != NULL)
	{
		LocalFree(pszTemp);
		pszTemp = NULL;
	} // end if (allocated string)

	return (fResult);
} // StringIsTrue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringGetKey()"
//==================================================================================
// StringGetKey
//----------------------------------------------------------------------------------
//
// Description: Copies the key into the passed in result string, or sets it to the
//				empty string ("") if could not get the key.
//
// Arguments:
//	char* szString		String to retrieve key from.
//	char* szResult		String to place key name into.
//
// Returns: None.
//==================================================================================
void StringGetKey(char* szString, char* szResult)
{
	int		iEqualLoc = -1;
	char*	pszKeyStart = NULL;


	strcpy(szResult, "");

	pszKeyStart = szString;

	// Loop past any white space until we hit something.  If we hit the end of the
	// string, then bail.
	while ((pszKeyStart[0] == ' ') ||
			(pszKeyStart[0] == '\t'))
	{
		if (pszKeyStart[0] == '\0')
			return;

		pszKeyStart++;
	} // end while (there's white space)

	iEqualLoc = StringContainsChar(pszKeyStart, '=', FALSE, 0);

	if (iEqualLoc < 1)
		return;

	CopyMemory(szResult, pszKeyStart, iEqualLoc);
	szResult[iEqualLoc] = '\0'; // terminate the string there
	
	// Clean up any trailing whitespace
	StringPopTrailingChars(szResult, " \t", false);

	return;
} // StringGetKey
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringGetKeyValue()"
//==================================================================================
// StringGetKeyValue
//----------------------------------------------------------------------------------
//
// Description: Returns the a pointer to the first non-whitespace character after
//				the first equal sign in the string.  If no equal sign was found or
//				there are no non-whitespace characters after it, then NULL is
//				returned.
//				Note that the pointer returned refers to the string passed in, so
//				freeing the source string also invalidates the key value offset
//				pointer returned.
//
// Arguments:
//	char* szString		String to retrieve key value from.
//
// Returns: Pointer to the value of key in the string, NULL if none.
//==================================================================================
char* StringGetKeyValue(char* szString)
{
	int		iEqualLoc = -1;
	char*	pszValue = NULL;


	iEqualLoc = StringContainsChar(szString, '=', false, 0);

	if (iEqualLoc < 0)
		return (NULL);

	// Okay, we have a starting point (move past the equal sign character).
	pszValue = szString + iEqualLoc + 1;
	
	// Loop past any white space until we hit something.  If we hit the end of the
	// string, then bail.
	while ((pszValue[0] == ' ') ||
			(pszValue[0] == '\t'))
	{
		if (pszValue[0] == '\0')
			return (NULL);

		pszValue++;
	} // end while (there's white space)

	return (pszValue);
} // StringGetKeyValue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringIsKey()"
//==================================================================================
// StringIsKey
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the given string begins with the key name specified,
//				case insensitive and ignoring whitespace, FALSE otherwise.
//
// Arguments:
//	char* szString		String to check.
//	char* szKeyName		Key to check if we match.
//
// Returns: TRUE if we matched the key, FALSE otherwise.
//==================================================================================
BOOL StringIsKey(char* szString, char* szKeyName)
{
	BOOL	fResult = FALSE;
	char*	pszTemp = NULL;
	char*	pszMyKeyName = NULL;
	char*	pszMyValue = NULL;



	// Make the passed in key name case insensitive
	pszTemp = (char*) LocalAlloc(LPTR, (strlen(szKeyName) + 1));
	if (pszTemp == NULL)
		goto DONE;
	strcpy(pszTemp, szKeyName);
	_strlwr(pszTemp);

	// Remove any leading white space
	StringPopLeadingChars(pszTemp, " \t", FALSE);


	// Make our entire string case insensitive
	pszMyValue = (char*) LocalAlloc(LPTR, (strlen(szString) + 1));
	if (pszMyValue == NULL)
		goto DONE;
	strcpy(pszMyValue, szString);
	_strlwr(pszMyValue);

	// Remove any leading white space
	StringPopLeadingChars(pszMyValue, " \t", FALSE);

	pszMyKeyName = (char*) LocalAlloc(LPTR, (strlen(pszMyValue) + 1));
	if (pszMyKeyName == NULL)
		goto DONE;
	ZeroMemory(pszMyKeyName, strlen(pszMyValue) + 1);
	
	StringGetKey(pszMyValue, pszMyKeyName);

	// Now try to compare key names
	if (strcmp(pszMyKeyName, pszTemp) == 0)
		fResult = TRUE;



DONE:

	if (pszTemp != NULL)
	{
		LocalFree(pszTemp);
		pszTemp = NULL;
	} // end if (allocated a temp string)

	if (pszMyKeyName != NULL)
	{
		LocalFree(pszMyKeyName);
		pszMyKeyName = NULL;
	} // end if (allocated a name string)

	if (pszMyValue != NULL)
	{
		LocalFree(pszMyValue);
		pszMyValue = NULL;
	} // end if (allocated a value string)

	return (fResult);
} // StringIsKey
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringIsValueTrue()"
//==================================================================================
// StringIsValueTrue
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the string's key value exists and is not all "0"'s,
//				"false" or "off", case-insensitive.  Otherwise this returns ;.
//
// Arguments:
//	char* szString	String with value to check.
//
// Returns: TRUE if the string value is anything but the textual representations of
//			false.
//==================================================================================
BOOL StringIsValueTrue(char* szString)
{
	return (StringIsTrue(StringGetKeyValue(szString)));
} // StringIsValueTrue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringIsCharBackslashEscaped()"
//==================================================================================
// StringIsCharBackslashEscaped
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the character pointed to is preceded by a backslash
//				(or an odd multiple of backslashes), indicating it is escaped.
//
// Arguments:
//	char* szString	String with character to check.
//	char* pcChar	Pointer to character within string to check.
//
// Returns: TRUE if the character is escaped, FALSE otherwise.
//==================================================================================
BOOL StringIsCharBackslashEscaped(char* szString, char* pcChar)
{
	char*	pcCurrent = pcChar - 1;


	if ((szString == NULL) || (pcChar == NULL))
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Either the string or character pointer is NULL!", 0);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (FALSE);
	} // end if (got bad pointers)

	// Keep going back until we hit the start of the string or a non-backslash
	// character.
	while ((pcCurrent >= szString) && ((*pcCurrent) == '\\'))
	{
		pcCurrent--;
	} // end while (we haven't hit the start of the string)

	// If there's an odd number of backslashes, it means the character is escaped,
	// but we were 1 off already, we compare with 0 instead of 1.
	if (((pcChar - pcCurrent) % 2) == 0)
		return (TRUE);
	
	return (FALSE);
} // StringIsCharBackslashEscaped
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringPopLeadingChars()"
//==================================================================================
// StringPopLeadingChars
//----------------------------------------------------------------------------------
//
// Description: Takes the string passed to it and removes all the chars in the
//				szPopChars string (if any) from the front of the string.  Returns
//				the number of chars popped. 
//
// Arguments:
//	char* szString		String to modify.
//	char* szPopChars	Characters at beginning of string to remove.
//	BOOL fMatchCase		Whether the case of the characters is important or not.
//
// Returns: Number of characters removed.
//==================================================================================
int StringPopLeadingChars(char* szString, char* szPopChars, BOOL fMatchCase)
{
	char*	pszString;
	int		iNumPopped = 0;


	pszString = szString;
	while (StringContainsChar(szPopChars, szString[iNumPopped], fMatchCase, 0) != -1)
	{
		pszString += sizeof (char);
		iNumPopped++;
	} // end while (there are still chars to pop)
	if (iNumPopped == 0)	// if we did nothing
		return (0);			// get out of here now
	
	strcpy(szString, pszString);

	return (iNumPopped);
} // StringPopLeadingChars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringPopTrailingChars()"
//==================================================================================
// StringPopTrailingChars
//----------------------------------------------------------------------------------
//
// Description: Takes the string passed to it and removes all the chars in the
//				szPopChars string (if any) from the end of the string.  Returns
//				the number of chars popped. 
//
// Arguments:
//	char* szString		String to modify.
//	char* szPopChars	Characters at end of string to remove.
//	BOOL fMatchCase		Whether the case of the characters is important or not.
//
// Returns: Number of characters removed.
//==================================================================================
int StringPopTrailingChars(char* szString, char* szPopChars, BOOL fMatchCase)
{
	int		i;
	int		iLen;


	iLen = strlen(szString) - 1;

	// If it's an empty string, we're done.
	if (iLen < 0)
		return (0);

	i = iLen;
	while (StringContainsChar(szPopChars, szString[i], fMatchCase, 0) != -1)
	{
		i--;
	} // end while (there are still chars to pop)

	szString[i + 1] = '\0'; // terminate the string here

	return (iLen - i);
} // StringPopTrailingChars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringPrepend()"
//==================================================================================
// StringPrepend
//----------------------------------------------------------------------------------
//
// Description: Inserts the second parameter at the front of the first parameter. 
//
// Arguments:
//	char* szString		String to start with.
//	char* szPrefix		String to place in front.
//
// Returns: None.
//==================================================================================
void StringPrepend(char* szString, char* szPrefix)
{
	// This method seems more optimal, but some compilers don't generate the code
	// for this correctly.
	/*
	// Move everything in the current string down (working backwards to avoid
	// overwriting).  +1 is to include NULL termination
	CopyMemory(szString + strlen(szPrefix), szString, (strlen(szString) + 1));
	*/


	char*	pcSource = szString + strlen(szString);
	char*	pcDest = szString + strlen(szString) + strlen(szPrefix);


	// Work backwards to avoid overwriting.
	do
	{
		(*pcDest) = (*pcSource);
		pcDest--;
		pcSource--;
	} // end do (while still more string left)
	while (pcSource >= szString);

	
	// Add in prefix
	CopyMemory(szString, szPrefix, strlen(szPrefix));
} // StringPrepend
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringSplit()"
//==================================================================================
// StringSplit
//----------------------------------------------------------------------------------
//
// Description: Splits the past string into two strings, with the first string
//				getting the first iSplitPoint characters, and the second string
//				getting the remainder.  If the split point is out of bounds, this
//				returns FALSE.
//				If szFirst is NULL, that portion of the string is not copied
//				(obviously).  szLast cannot be NULL.
//				The last string can safely be the same as the source string.  Thus
//				the call StringSplit(szStringA, 5, szStringB, szStringA) essentially
//				pops off the first 5 characters of szStringA and saves them in
//				szStringB. 
//
// Arguments:
//	char* szString				String to split
//	unsigned int uiSplitPoint	Position in string to split at (zero based)
//	char* szFirst				String to store characters before split point.
//	char* szLast				String to store characters after split point.
//
// Returns: TRUE if successful, FALSE otherwise.
//==================================================================================
BOOL StringSplit(char* szString, unsigned int uiSplitPoint, char* szFirst,
				char* szLast)
{
	// Don't want to initialize these strings because they could be the same
	// as the source string.
	//strcpy(szFirst, "");
	//strcpy(szLast, "");

	if (uiSplitPoint > strlen(szString)) // invalid point
		return (FALSE); // get out of here

	if (szLast == NULL)
		return (FALSE); // get out of here

	if (szFirst != NULL)
	{
		CopyMemory(szFirst, szString, uiSplitPoint * sizeof (char));
		szFirst[uiSplitPoint] = '\0'; // null terminate.
	} // end if (the caller cares about the first part of the string)

	//if (fIncludeSplitChar)
		strcpy(szLast, szString + (sizeof (char) * uiSplitPoint));
	//else
	//	strcpy(szLast, szString + (sizeof (char) * (uiSplitPoint + 1)));

	return (TRUE);
} // StringSplit
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringSplitIntoFixedFields()"
//==================================================================================
// StringSplitIntoFixedFields
//----------------------------------------------------------------------------------
//
// Description: Splits the passed in string into fields separated by one or more of
//				any of the characters in szSplitChars.  The caller must allocate
//				the results string array (i.e. char  resultArray[5][256]) and
//				pass in the dimensions of this array in the last two parameters,
//				(i.e. 5, 256).  Note that this function may not actually find
//				iMaxResults fields, but if there are at least that many, then
//				elements 0 through (iMaxResults - 2) are the first
//				(iMaxResults - 1) fields found, and the last element contains the
//				rest of the string with leading and trailing split chars removed,
//				but truncated to fit in the array if necessary. 
//
// Arguments:
//	char* szString					The string to split.
//	char* szSplitChars				String containing all the characters that
//									delimit fields.
//	char* resultArray				An array of strings to store results in.
//	unsigned int uiMaxResults		How many strings the result array can hold.
//	unsigned int uiStringLength		How long the strings in the result array are.
//
// Returns: The number of fields the string was split into, 0 if no non-split
//			characters were found.
//==================================================================================
unsigned int StringSplitIntoFixedFields(char* szString, char* szSplitChars,
										char* resultArray, unsigned int uiMaxResults,
										unsigned int uiStringLength)
{
	char*			pszCurrent = szString;
	char*			pszFieldStart = szString;
	char*			pszDest = NULL;
	unsigned int	uiFieldSize = 0;
	unsigned int	uiNumFields = 0;
	BOOL			fInField = FALSE;


	while (uiNumFields < uiMaxResults)
	{
		// If we're now on a delimiter or we've reached the maximum field size, or
		// we've hit the end of the string.
		if ((StringContainsChar(szSplitChars, (*pszCurrent), FALSE, 0) != -1) ||
			(uiFieldSize >= uiStringLength - 1) ||
			((*pszCurrent) == '\0'))
		{
			if (fInField)
			{
				pszDest = resultArray + ((uiNumFields - 1) * uiStringLength);
				CopyMemory(pszDest, pszFieldStart, uiFieldSize * sizeof (char));

				// Now make sure the string is terminated
				pszDest[uiFieldSize] = '\0';

				fInField = FALSE;
				uiFieldSize = 0;
			} // end if (we were in a field)
		} // end if (this is a field delimiter)
		else
		{
			if (! fInField)
			{
				fInField = TRUE;
				uiNumFields++;
				uiFieldSize = 1;
				pszFieldStart = pszCurrent;
			} // end if (we weren't in a field)
			else
				uiFieldSize++;
		} // end else (this is part of a field)

		// If we're at the end of the string, we're done.
		if ((*pszCurrent) == '\0')
			break;

		pszCurrent++;
	} // end while (there's room for more fields)

	// If we stopped looping because we ran out of room for fields
	if (uiNumFields == uiMaxResults)
	{
		pszDest = resultArray + ((uiNumFields - 1) * uiStringLength);
		if (strlen(pszFieldStart) < uiStringLength)
			uiFieldSize = strlen(pszFieldStart);
		else
			uiFieldSize = uiStringLength - 1;

		CopyMemory(pszDest, pszFieldStart, (uiFieldSize * sizeof (char)));

		// Now make sure the string is terminated
		pszDest[uiFieldSize] = '\0';
	} // end if (ran out of room)

	return (uiNumFields);
} // StringSplitIntoFixedFields
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringSplitIntoFields()"
//==================================================================================
// StringSplitIntoFields
//----------------------------------------------------------------------------------
//
// Description: Splits the passed in string into fields separated by one or more of
//				any of the characters in szSplitChars.  The result buffer is filled
//				with the each of the fields' strings (including NULL termination)
//				concatenated.
//				If pszResultBuffer is NULL, then the size required is placed in
//				pdwResultBufferSize.  Note that the buffer size required will never
//				be larger than the original string.
//				Also note that the size is in number-of-characters, including NULL
//				terminators.
//
// Arguments:
//	char* szString					The string to split.
//	char* szSplitChars				String containing all the characters that
//									delimit fields.
//	char* pszResultBuffer			Pointer to buffer that holds concatentated
//									fields, or NULL to retrieve size.
//	DWORD* pdwResultBufferSize		Pointer to size of preceding buffer, or place to
//									store size required.  If this is NULL, it is
//									assumed the buffer exists and is the right size.
//
// Returns: The number of fields found, zero if an error occurred.
//==================================================================================
DWORD StringSplitIntoFields(char* szString, char* szSplitChars,
							char* pszResultBuffer, DWORD* pdwResultBufferSize)
{
	char*	pcCurrent = szString;
	char*	pcDest = pszResultBuffer;
	BOOL	fInField = FALSE;
	DWORD	dwNumFields = 0;
	DWORD	dwSizeRequired = 0;


	if ((pszResultBuffer == NULL) && (pdwResultBufferSize))
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Both parameters are NULL!", 0);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (0);
	} // end if (both parameters are NULL)

	while ((*pcCurrent) != '\0')
	{
		if (StringContainsChar(szSplitChars, (*pcCurrent), FALSE, 0) == -1)
		{
			// If we weren't in a field, we are now.
			if (! fInField)
			{
				dwNumFields++;
				fInField = TRUE;
			} // end if (we weren't in a field before)

			// Copy the character or at least include it in our size requirement.
			if (pszResultBuffer != NULL)
			{
				(*pcDest) = (*pcCurrent);
				pcDest++; // move the destination character pointer
			} // end if (there's a buffer to copy to)
			else
				dwSizeRequired++;
		} // end if (it's not a split char)
		else
		{
			// If we were in a field, we need to end it.
			if (fInField)
			{
				if (pszResultBuffer != NULL)
				{
					(*pcDest) = '\0';
					pcDest++; // move the destination character pointer
				} // end if (there's a buffer to copy to)
				else
					dwSizeRequired++;

				fInField = FALSE;
			} // end else (we were in a field)
		} // end else (it's a split char)

		pcCurrent++; // move the current character pointer
	} // end while (we're not at the end of the string)

	if (pszResultBuffer != NULL)
	{
		if (fInField)
			(*pcDest) = '\0'; // make sure the last string is terminated here
	} // end if (there's a buffer to copy to)
	else
	{
		if (fInField)
			dwSizeRequired++; // include final string's termination

		if (pdwResultBufferSize != NULL)
			(*pdwResultBufferSize) = dwSizeRequired;
	} // end else (the caller just wants the size)

	return (dwNumFields);
} // StringSplitIntoFields
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringGetFieldPtr()"
//==================================================================================
// StringGetFieldPtr
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the field at the given zero-based index in a
//				concatenated field buffer (like that generated from
//				StringSplitIntoFields).
//				It is up to the caller to make sure we are not walking off the end
//				of the buffer.
//
// Arguments:
//	char* szFieldsBuffer	A buffer previously generated by StringSplitIntoFields.
//	DWORD dwFieldNum		The number of fields in the previous buffer.
//
// Returns: The number of fields found.
//==================================================================================
char* StringGetFieldPtr(char* szFieldsBuffer, DWORD dwFieldNum)
{
	char*	pszCurrent = szFieldsBuffer;
	DWORD	dwCurrentNum = 0;


	while (dwCurrentNum < dwFieldNum)
	{
		pszCurrent += strlen(pszCurrent) + 1; // move past the current field
		dwCurrentNum++;
	} // end while (we haven't reached the field yet)

	return (pszCurrent);
} // StringGetFieldPtr
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringGetBetween()"
//==================================================================================
// StringGetBetween
//----------------------------------------------------------------------------------
//
// Description: This retrieves all characters between the first occurrences of
//				szStartToken and pszEndToken.
//
//				If pszEndToken is NULL, then no ending token is searched for.
//
//				The search is started at the index pointed to by piPos.  Upon
//				completion of this function, that value will be set to -1 if the
//				start token was not found, or else the location at which the first
//				character was removed.
//
//				If pszOutside is not NULL, the remaining string after the
//				characters between the tokens are removed is copied there.  If
//				pszInside is not NULL, then the characters between the tokens are
//				copied to that string.  One or the other of these can safely be the
//				source string.
//
//				If pbUnfinished is not NULL, the boolean it points to will be set
//				to TRUE if the string ended before the end token was found.  Note
//				that this will always be TRUE if pszEndToken is NULL.
//
// Arguments:
//	char* szSource					Place to store the characters removed.
//	char* szStartToken				String that designates the start of characters
//									to be removed.
//	char* pszEndToken				String that designates the end of characters to
//									be removed (optional).
//	BOOL fMatchCase					Whether the cases of the tokens are important
//									when searching for them.
//	BOOL fKeepTokensInOutside		Whether to include the tokens in the remaining
//									outside string or not.
//	BOOL fKeepTokensInInside		Whether to include the tokens in the inside
//									string or not.
//	BOOL fCheckForNonEscapedStart	Should be TRUE if the first character in the
//									start token doesn't count as a match if it is
//									preceded by a backslash character ('\').
//	BOOL fCheckForNonEscapedEnd		Should be TRUE if the first character in the
//									end token doesn't count as a match if it is
//									preceded by a backslash character ('\').
//	int* piPos						Pointer to position to start looking and return
//									starting index of result in.
//	char* pszOutside				Optional place to store the remaining string
//									after the characters between the tokens have
//									been removed.
//	char* pszInside					Optional place to store the characters in
//									between the tokens.
//	BOOL* pfUnfinished				Optional pointer to boolean that will be set to
//									TRUE if the string ended before the end token
//									was found.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT StringGetBetween(char* szSource, char* szStartToken, char* pszEndToken,
						BOOL fMatchCase, BOOL fKeepTokensInOutside,
						BOOL fKeepTokensInInside, BOOL fCheckForNonEscapedStart,
						BOOL fCheckForNonEscapedEnd, int* piPos, char* pszOutside,
						char* pszInside, BOOL* pfUnfinished)
{
	HRESULT		hr = S_OK;
	BOOL		fSearchAgain = FALSE;
	int			iEndLoc = -1;
	char*		pszTempOutside = NULL;
	char*		pszTempInside = NULL;


	do
	{
		(*piPos) = StringContainsString(szSource, szStartToken, fMatchCase, (*piPos));
		if ((*piPos) < 0) // if we didn't find the start token
		{
			goto DONE;
		} // end if (didn't find start token)

		if (fCheckForNonEscapedStart)
		{
			if (StringIsCharBackslashEscaped(szSource, szSource + (*piPos)))
			{
				fSearchAgain = TRUE;
				(*piPos) += 1; // move on to the next character after this false match
			} // end if (we need to search again)
			else
				fSearchAgain = FALSE;
		} // end if (we should make sure the first character is not escaped)
	} // end do (while we should search again for the character)
	while (fSearchAgain);


	// If we should search for an end token, do it
	if (pszEndToken != NULL)
	{
		// Start looking after the start token
		iEndLoc = (*piPos) + strlen(szStartToken);

		do
		{
			iEndLoc = StringContainsString(szSource, pszEndToken, fMatchCase,
											iEndLoc);
			// If we hit the end of the string, stop checking
			if (iEndLoc < 0)
				break;

			if (fCheckForNonEscapedEnd)
			{
				if (StringIsCharBackslashEscaped(szSource, szSource + iEndLoc))
				{
					fSearchAgain = TRUE;
					iEndLoc += 1; // move on to the next character after this false match
				} // end if (we need to search again)
				else
					fSearchAgain = FALSE;
			} // end if (we should make sure the first character is not escaped)
		} // end do (while we should search again for the character)
		while (fSearchAgain);
	} // end if (we should search for an end token)


	// If we hit the end of the string before finding the end token, mark it as
	// an unfinished item.
	if ((iEndLoc < 0) && (pfUnfinished != NULL))
		(*pfUnfinished) = TRUE;


	if (pszOutside != NULL)
	{
		pszTempOutside = (char*) LocalAlloc(LPTR, ((strlen(szSource) + 1) * sizeof (char))); // include NULL character
		if (pszTempOutside == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		if (fKeepTokensInOutside)
			CopyMemory(pszTempOutside, szSource, ((*piPos) + strlen(szStartToken)) * sizeof (char));
		else
			CopyMemory(pszTempOutside, szSource, (*piPos) * sizeof (char));

		if (iEndLoc >= 0)
		{
			if (fKeepTokensInOutside)
				strcat(pszTempOutside, szSource + iEndLoc);
			else
				strcat(pszTempOutside, szSource + iEndLoc + strlen(pszEndToken));
		} // end if (we found the end token)
	} // end if (we should do a destination string)

	if (pszInside != NULL)
	{
		pszTempInside = (char*) LocalAlloc(LPTR, ((strlen(szSource) + 1) * sizeof (char))); // include NULL character
		if (pszTempInside == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		if (iEndLoc >= 0)
		{
			if (fKeepTokensInInside)
				CopyMemory(pszTempInside, szSource + (*piPos),
						(iEndLoc - (*piPos) + strlen(pszEndToken)) * sizeof (char));
			else
				CopyMemory(pszTempInside, szSource + (*piPos) + strlen(szStartToken),
						(iEndLoc - (*piPos) - 1) * sizeof (char));
		} // end if (we found the end token)
		else
		{
			if (fKeepTokensInInside)
				strcpy(pszTempInside, szSource + (*piPos));
			else
				strcpy(pszTempInside, szSource + (*piPos) + strlen(szStartToken));
		} // end else (we didn't find the end token)
	} // end if (we should do a removed string)


DONE:

	if (pszTempOutside != NULL)
	{
		strcpy(pszOutside, pszTempOutside);
		LocalFree(pszTempOutside);
		pszTempOutside = NULL;
	} // end if (we did a destination string)

	if (pszTempInside != NULL)
	{
		strcpy(pszInside, pszTempInside);
		LocalFree(pszTempInside);
		pszTempInside = NULL;
	} // end if (we did a remove string)

	return (hr);
} // StringGetBetween
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringReplaceAll()"
//==================================================================================
// StringReplaceAll
//----------------------------------------------------------------------------------
//
// Description: Replaces all occurrences of the search string with the replace
//				string, starting at the location given by iPos. 
//
// Arguments:
//	char* szString				The string to search & replace in.
//	char* szSearchString		Character sequence that should be replaced if found.
//	char* szReplaceString		Character sequence to replace found items with.
//	BOOL fMatchCase				Whether case is important when searching.
//	int iPos					Location to start looking.
//	unsigned int uiStringSize	Size of szString's buffer, including zero
//								terminator.
//
// Returns: The number of occurrences found and replaced.
//==================================================================================
int StringReplaceAll(char* szString, char* szSearchString, char* szReplaceString,
					 BOOL fMatchCase, int iPos, unsigned int uiStringSize)
{
	int		iNum = 0;
	int		iLoc = 0;
	char*	szFirst;
	char*	szLast;


	szFirst = (char*) LocalAlloc(LPTR, (sizeof (char) * uiStringSize));
	if (szFirst == NULL)
		goto DONE;

	szLast = (char*) LocalAlloc(LPTR, (sizeof (char) * uiStringSize));
	if (szLast == NULL)
		goto DONE;

	// While there's another occurence of the search string and we have
	// enough room in the string to replace it.
	while (((iLoc = StringContainsString(szString, szSearchString, fMatchCase, iPos)) >= 0) &&
		(strlen(szString) - strlen(szSearchString) + strlen(szReplaceString) < uiStringSize))
	{
		ZeroMemory(szFirst, sizeof (char) * uiStringSize);
		CopyMemory(szFirst, szString, sizeof (char) * iLoc);

		ZeroMemory(szLast, sizeof (char) * uiStringSize);
		CopyMemory(szLast, szString + sizeof (char) * (iLoc + strlen(szSearchString)),
				sizeof (char) * (strlen(szString) - strlen(szSearchString) - iLoc));

		strcpy(szString, szFirst);
		strcat(szString, szReplaceString);
		strcat(szString, szLast);

		iNum++;

		iPos = iLoc + strlen(szReplaceString);
	} // end while (there are still instances of the search string)

DONE:

	if (szFirst != NULL)
		LocalFree(szFirst);
	szFirst = NULL;

	if (szLast != NULL)
		LocalFree(szLast);
	szLast = NULL;

	return (iNum);
} // StringReplaceAll
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringToInt()"
//==================================================================================
// StringToInt
//----------------------------------------------------------------------------------
//
// Description: Takes the passed string which holds an integer in base ten and
//				converts it into an integer.  It ignores any non-English number
//				characters.
//
// Arguments:
//	char* szString		Textual representation of value.
//
// Returns: The value of the string converted into an integer.
//==================================================================================
int StringToInt(char* szString)
{
	int		iValue;
	DWORD	dwTemp;
	BOOL	fNegative = FALSE;


	iValue = 0;
	for(dwTemp = 0; dwTemp < strlen(szString); dwTemp++)
	{
		// If the first character is a - sign, note that.
		if ((dwTemp == 0) && (szString[dwTemp] == '-'))
			fNegative = TRUE;

		iValue = iValue * 10; // shift the decimal place
		switch (szString[dwTemp])
		{
			case '0':
			  break; // don't add anything

			case '1':
				iValue += 1;
			  break;

			case '2':
				iValue += 2;
			  break;

			case '3':
				iValue += 3;
			  break;

			case '4':
				iValue += 4;
			  break;

			case '5':
				iValue += 5;
			  break;

			case '6':
				iValue += 6;
			  break;

			case '7':
				iValue += 7;
			  break;

			case '8':
				iValue += 8;
			  break;

			case '9':
				iValue += 9;
			  break;

			default:
				iValue = iValue / 10; // shift the decimal place back
			  break; // we got a wacky character
		} // end switch (on the letter)
	} // end for (loop through each letter in the string)

	if (fNegative)
		return (-1 * iValue);

	return (iValue);
} // StringToInt
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringNumDWordDigits()"
//==================================================================================
// StringNumDWordDigits
//----------------------------------------------------------------------------------
//
// Description: Returns the number of decimal digits a given DWORD has.
//
// Arguments:
//	DWORD dwValue	DWORD to count digits.
//
// Returns: The number of decimal digits the DWORD has.
//==================================================================================
DWORD StringNumDWordDigits(DWORD dwValue)
{
	DWORD	dwDigits = 1;

	
	while (dwValue >= 10)
	{
		dwValue = dwValue / 10;
		dwDigits++;
	} // end while (there's still a tens place)
	
	return (dwDigits);
} // StringNumDWordDigits
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringToDWord()"
//==================================================================================
// StringToDWord
//----------------------------------------------------------------------------------
//
// Description: Takes the passed string which holds a positive integer in base ten
//				and converts it into a DWORD.  It ignores any non-English number
//				characters.
//
// Arguments:
//	char* szString		Textual representation of value.
//
// Returns: The value of the string converted into a DWORD.
//==================================================================================
DWORD StringToDWord(char* szString)
{
	DWORD	dwValue;
	DWORD	dwTemp;


	dwValue = 0;
	for(dwTemp = 0; dwTemp < strlen(szString); dwTemp++)
	{
		dwValue = dwValue * 10; // shift the decimal place
		switch (szString[dwTemp])
		{
			case '0':
			  break; // don't add anything
			case '1':
				dwValue += 1;
			  break;
			case '2':
				dwValue += 2;
			  break;
			case '3':
				dwValue += 3;
			  break;
			case '4':
				dwValue += 4;
			  break;
			case '5':
				dwValue += 5;
			  break;
			case '6':
				dwValue += 6;
			  break;
			case '7':
				dwValue += 7;
			  break;
			case '8':
				dwValue += 8;
			  break;
			case '9':
				dwValue += 9;
			  break;
			default:
				dwValue = dwValue / 10; // shift the decimal place back
			  break; // we got a wacky character
		} // end switch (on the letter)
	} // end for (loop through each letter in the string)

	return (dwValue);
} // StringToDWord
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringHexToDWord()"
//==================================================================================
// StringHexToDWord
//----------------------------------------------------------------------------------
//
// Description: Converts the passed in string representation of a hexadecimal DWORD
//				to the actual DWORD value.  It ignores any non-English number
//				characters.
//
// Arguments:
//	char* szString		Textual representation of value.
//
// Returns: The value of the string converted into a DWORD.
//==================================================================================
DWORD StringHexToDWord(char* szString)
{
	DWORD	dwValue = 0;
	DWORD	dwTemp;


	// If the string starts with that "0x" prefix, we should move past that.
	if (StringStartsWith(szString, "0x", false))
		szString += 2;


	if ((strlen(szString) < 1) || (strlen(szString) > 8))
		return (0);


	dwValue = 0;
	for(dwTemp = 0; dwTemp < strlen(szString); dwTemp++)
	{
		dwValue *= 16; // shift the hexadecimal place
		switch (szString[dwTemp])
		{
			case '0':
			  break; // don't add anything
			case '1':
				dwValue += 1;
			  break;
			case '2':
				dwValue += 2;
			  break;
			case '3':
				dwValue += 3;
			  break;
			case '4':
				dwValue += 4;
			  break;
			case '5':
				dwValue += 5;
			  break;
			case '6':
				dwValue += 6;
			  break;
			case '7':
				dwValue += 7;
			  break;
			case '8':
				dwValue += 8;
			  break;
			case '9':
				dwValue += 9;
			  break;
			case 'a':
			case 'A':
				dwValue += 10;
			  break;
			case 'b':
			case 'B':
				dwValue += 11;
			  break;
			case 'c':
			case 'C':
				dwValue += 12;
			  break;
			case 'd':
			case 'D':
				dwValue += 13;
			  break;
			case 'e':
			case 'E':
				dwValue += 14;
			  break;
			case 'f':
			case 'F':
				dwValue += 15;
			  break;
			default:
				dwValue = dwValue / 16; // shift the hexadecimal place back
			  break; // we got a wacky character
		} // end switch (on the letter)
	} // end for (loop through each letter in the string)

	return (dwValue);
} // StringHexToDWord
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringToGuid()"
//==================================================================================
// StringToGuid
//----------------------------------------------------------------------------------
//
// Description: Converts the passed in string representation of a GUID to the actual
//				GUID. 
//
// Arguments:
//	char* szString			Textual representation of guid.
//	LPGUID pguidResult		Pointer to guid to store result in.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT StringToGuid(char* szString, LPGUID pguidResult)
{
	int		iOldPos = 0;
	int		iNewPos = 0;
	char	szSegment[9];
	DWORD	dwValue = 0;
	int		i;



	// Skip the beginning
	while ((szString[iOldPos] == ' ') ||
			(szString[iOldPos] == '\t') ||
			(szString[iOldPos] == '{'))
	{
		iOldPos++;
	} // end while (it starts with an ignore character)


	// Data1 = 1 DWORD

	iNewPos = StringContainsChar(szString, '-', false, iOldPos);

	// Make sure we've got 8 valid characters of data (assume high order zeroes if
	// it's fewer than 8 characters).
	if ((iNewPos < 0) || (iNewPos > (iOldPos + 9)))
		return (ERROR_INVALID_PARAMETER);


	ZeroMemory(szSegment, 9);
	CopyMemory(szSegment, szString + iOldPos, (iNewPos - iOldPos));

	pguidResult->Data1 = StringHexToDWord(szSegment);

	iOldPos = iNewPos + 1;



	// Data2 = 1 WORD

	iNewPos = StringContainsChar(szString, '-', false, iOldPos);

	// 4 characters.
	if ((iNewPos < 0) || (iNewPos > (iOldPos + 5)))
		return (ERROR_INVALID_PARAMETER);


	ZeroMemory(szSegment, 9);
	CopyMemory(szSegment, szString + iOldPos, (iNewPos - iOldPos));

	pguidResult->Data2 = (WORD) StringHexToDWord(szSegment);

	iOldPos = iNewPos + 1;



	// Data3 = 1 WORD

	iNewPos = StringContainsChar(szString, '-', false, iOldPos);

	// 4 characters.
	if ((iNewPos < 0) || (iNewPos > (iOldPos + 5)))
		return (ERROR_INVALID_PARAMETER);


	ZeroMemory(szSegment, 9);
	CopyMemory(szSegment, szString + iOldPos, (iNewPos - iOldPos));

	pguidResult->Data3 = (WORD) StringHexToDWord(szSegment);


	iNewPos = StringContainsChar(szString, '-', false, iOldPos);

	iOldPos = strlen(szString); // temp variable

	// Make sure there are enough single bytes at the end here
	if (iNewPos + 16 > iOldPos)
		return (ERROR_INVALID_PARAMETER);


	// Move past dash
	iNewPos++;

	// Data4 = 8 single bytes

	for(i = 0; i < 8; i++)
	{
		if (i == 2)
		{
			if (szString[iNewPos] != '-')
				return (ERROR_INVALID_PARAMETER);

			// If it is the dash, then we want to skip it
			iNewPos++;
		} // end if (we should be at a dash)

		ZeroMemory(szSegment, 9);
		CopyMemory(szSegment, szString + iNewPos, 2);

		pguidResult->Data4[i] = (BYTE) StringHexToDWord(szSegment);

		iNewPos += 2;
	} // end for (each of the 8 individual bytes)


	return (S_OK);
} // StringToGuid
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringDWordToIPAddress()"
//==================================================================================
// StringDWordToIPAddress
//----------------------------------------------------------------------------------
//
// Description: Converts a DWORD IP address into its textual representation.
//
// Arguments:
//	DWORD dwValue		DWORD containing IP address.
//	char* szResult		Buffer to store result.
//
// Returns: ?
//==================================================================================
void StringDWordToIPAddress(DWORD dwValue, char* szResult)
{
	BYTE	segment;
	int		i;
	int		offset = 0;

	strcpy(szResult, "");

	for (i = 0; i < 4; i++)
	{
		segment = (BYTE) (dwValue % 256);
		dwValue = (dwValue - segment) / 256;
		offset += wsprintf(szResult + offset, "%i.", segment); 
	} // end for (each segment)

	szResult[offset - 1] = '\0'; // get rid of last period and end the string.
} // StringDWordToIPAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringNumIntDigits()"
//==================================================================================
// StringNumIntDigits
//----------------------------------------------------------------------------------
//
// Description: Returns the number of decimal digits a given integer has.
//
// Arguments:
//	int iValue		int for which to count digits.
//
// Returns: The number of decimal digits the int has.
//==================================================================================
DWORD StringNumIntDigits(int iValue)
{
	DWORD	dwDigits = 1;


	if (iValue < 0)
		iValue = iValue * -1; // make sure it's a positive number
	
	while (iValue >= 10)
	{
		iValue = iValue / 10;
		dwDigits++;
	} // end while (there's still a tens place)
	
	return (dwDigits);
} // StringNumIntDigits
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringIntToZeroPaddedStr()"
//==================================================================================
// StringIntToZeroPaddedStr
//----------------------------------------------------------------------------------
//
// Description: Takes a number and pads the front with zeros if it is fewer than the
//				number of digits passed and returns the result in the passed string. 
//
// Arguments:
//	int iNum			Number to pad, if necessary.
//	int iDigits			Minimum number of digits needed.
//	char* szString		Buffer in which to store results.
//
// Returns: None.
//==================================================================================
void StringIntToZeroPaddedStr(int iNum, int iDigits, char* szString)
{
	char*	szTemp = NULL;
	int		i;
	strcpy(szString, "");

	szTemp = (char*) LocalAlloc(LPTR, ((iDigits + 1 + 1) * sizeof (char))); // + 1 for possible negative
	if (szTemp == NULL) // out of memory
		return;

	if (iNum < 0)
	{
		strcpy(szString, "-");
		iNum = iNum * -1;
		iDigits--; // negative sign takes away a digit
	} // end if (the number is negative)

	wsprintf(szTemp, "%i", iNum);

	for (i = strlen(szTemp); i < iDigits; i++)
		strcat(szString, "0");
	strcat(szString, szTemp);

	LocalFree(szTemp);
	szTemp = NULL;
} // StringIntToZeroPaddedStr
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringIntToChar()"
//==================================================================================
// StringIntToChar
//----------------------------------------------------------------------------------
//
// Description: Takes the integer number passed in (between 0 and 15) and returns
//				the ASCII character corresponding to the hexidecimal value.  If the
//				value is not between 0 and 15, an 'X' character is returned. 
//
// Arguments:
//	int iNum	Integer to convert.
//
// Returns: Character representing value.
//==================================================================================
char StringIntToChar(int iNum)
{
	switch (iNum)
	{
		case 0:
			return('0');
		  break;
		case 1:
			return('1');
		  break;
		case 2:
			return('2');
		  break;
		case 3:
			return('3');
		  break;
		case 4:
			return('4');
		  break;
		case 5:
			return('5');
		  break;
		case 6:
			return('6');
		  break;
		case 7:
			return('7');
		  break;
		case 8:
			return('8');
		  break;
		case 9:
			return('9');
		  break;
		case 10:
			return('A');
		  break;
		case 11:
			return('B');
		  break;
		case 12:
			return('C');
		  break;
		case 13:
			return('D');
		  break;
		case 14:
			return('E');
		  break;
		case 15:
			return('F');
		  break;
		default:
			return('X');
		  break;
	} // end switch
} // StringIntToChar
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringBufferToHexStr()"
//==================================================================================
// StringBufferToHexStr
//----------------------------------------------------------------------------------
//
// Description: Converts a block of memory into a string representation of the bytes
//				of data, in hex.
//
// Arguments:
//	PVOID pvBuffer		Pointer to buffer to use.
//	DWORD dwBufferSize	Size of buffer.
//	char* szResult		Pointer to string large enough to hold result (3 characters
//						per byte of data).
//
// Returns: None.
//==================================================================================
void StringBufferToHexStr(PVOID pvBuffer, DWORD dwBufferSize, char* szResult)
{
	BYTE*	pbCurrent = (BYTE*) pvBuffer;
	int		x;
	int		y;
	DWORD	dwStringPos = 0;


	while (dwBufferSize > 0)
	{
		x = (*pbCurrent);
		y = x % 16;
		x = (x - y) / 16;
		szResult[dwStringPos++] = StringIntToChar(x);
		szResult[dwStringPos++] = StringIntToChar(y);
		szResult[dwStringPos++] = ' ';

		pbCurrent++;
		dwBufferSize--;
	} // end for (each byte in the passed in buffer)

	if (dwStringPos <= 0) // if we didn't do anything
		dwStringPos = 1; // have the termination be at the start

	szResult[dwStringPos - 1] = '\0'; // terminate the string on the last space
} // StringBufferToHexStr
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringEnclosePathSpacesInQuotes()"
//==================================================================================
// StringEnclosePathSpacesInQuotes
//----------------------------------------------------------------------------------
//
// Description: Copies the path string into the result string buffer, enclosing any
//				path items containing spaces in quotes.  If NULL is passed for a
//				result buffer, then the DWORD pointed to by the size argument will
//				be filled with the size of the buffer required, including NULL
//				termination, and ERROR_BUFFER_TOO_SMALL will be returned.
//
// Arguments:
//	char* szPath				String containing environment variables to expand.
//	char* pszResultBuffer		Pointer to buffer to place results in, or NULL to
//								retrieve size required.
//	DWORD* pdwResultBufferSize	Pointer to size of destination buffer, or place to
//								store size required, including NULL terminator.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT StringEnclosePathSpacesInQuotes(char* szPath, char* pszResultBuffer,
										DWORD* pdwResultBufferSize)
{
	(*pdwResultBufferSize) = strlen(szPath) + 1;


	if (StringContainsChar(szPath, ' ', FALSE, 0) >= 0)
		(*pdwResultBufferSize) += 2;


	if (pszResultBuffer == NULL)
		return (ERROR_BUFFER_TOO_SMALL);


	if (StringContainsChar(szPath, ' ', FALSE, 0) < 0)
	{
		strcpy(pszResultBuffer, szPath);
		return (S_OK);
	} // end if (there's no spaces in the path)


	strcpy(pszResultBuffer, "\"");
	strcat(pszResultBuffer, szPath);
	strcat(pszResultBuffer, "\"");

	return (S_OK);
} // StringEnclosePathSpacesInQuotes
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringURLEncode()"
//==================================================================================
// StringURLEncode
//----------------------------------------------------------------------------------
//
// Description: Takes the passed string and escapes the illegal characters via the
//				standard URL %xx format.
//				Pass in NULL for the destination buffer to have the size required
//				(including NULL termination) placed in the DWORD at
//				pdwEncodedStringSize), and have ERROR_BUFFER_TOO_SMALL returned.
//
// Arguments:
//	char* szString					String to convert.
//	char* pszEncodedString			Pointer to buffer to store results, or NULL to
//									retrieve size.
//	DWORD* pdwEncodedStringSize		Place to store buffer size (including NULL
//									terminator) required.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT StringURLEncode(char* szString, char* pszEncodedString,
						DWORD* pdwEncodedStringSize)
{
	BYTE*	pbCurrentSrc = (BYTE*) szString;
	BYTE*	pbCurrentDest = (BYTE*) pszEncodedString;
	DWORD	dwSizeRequired = 0;


	do
	{
		// All characters require at least one space in the encoded string.  We
		// may add more later.
		dwSizeRequired++;

		// If it's the NULL terminator, end the encoded string and we're done.
		if ((*pbCurrentSrc) == 0)
		{
			if (pbCurrentDest == NULL)
			{
				(*pdwEncodedStringSize) = dwSizeRequired;
				return (ERROR_BUFFER_TOO_SMALL);
			} // end if (no destination string)

			// Otherwise, there's a buffer to write to, so do it.
			(*pbCurrentDest) = 0;

			// Get out of the while loop.
			break;
		} // end if (NULL terminator)
		else if (((*pbCurrentSrc) < 0x21) ||
				((*pbCurrentSrc) > 0x7E) ||
				((s_SafetyHash[(*pbCurrentSrc) - 0x21]) & URLHASH_UNSAFE))
		{
			// It's an unsafe character, and must be converted.  That means
			// we need two extra bytes (for a total of 3, string is "%nn")
			dwSizeRequired += 2;
			
			if (pbCurrentDest != NULL)
			{
				// We're going to use (*pbCurrentDest) as a temporary
				// variable to hold the remainder (second letter in byte).
				(*pbCurrentDest) = (*pbCurrentSrc) % 16;

				*(pbCurrentDest + 2) = StringIntToChar(*pbCurrentDest);
				*(pbCurrentDest + 1) = StringIntToChar(((*pbCurrentSrc) - (*pbCurrentDest)) / 16);

				// Overwrite that temp spot with the actual character we want.
				(*pbCurrentDest) = '%'; // escape character;

				// Move 3 characters down.
				pbCurrentDest += 3;
			} // end if (there's a destination string)
		} // end else if (less than 0x21 or greater than 0x7E or unsafe)
		else
		{
			// It's a safe character, so just copy if over if there's a
			// buffer.
			if (pbCurrentDest != NULL)
			{
				(*pbCurrentDest) = (*pbCurrentSrc);
				pbCurrentDest++;
			} // end if (there's a destination string)
		} // end else (it's a safe character)

		pbCurrentSrc++;
	} // end do (while not at end of string)
	while (TRUE);


	return (S_OK);
} // StringURLEncode
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringGetCurrentDateStr()"
//==================================================================================
// StringGetCurrentDateStr
//----------------------------------------------------------------------------------
//
// Description: Gets the current time and date, formats it and puts it in the passed
//				string. 
//
// Arguments:
//	char* szString		Buffer to store result.
//
// Returns: None.
//==================================================================================
void StringGetCurrentDateStr(char* szString)
{
	time_t		tTime;
	struct tm*	ptmLocalTime;


	time(&tTime);
	ptmLocalTime = localtime(&tTime);
	strftime(szString, 128, "%I:%M:%S%p %A, %B %d, %Y", ptmLocalTime);
} // StringGetCurrentDateStr
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringGetCurrentDateStrShort()"
//==================================================================================
// StringGetCurrentDateStrShort
//----------------------------------------------------------------------------------
//
// Description: Gets the current time and date, formats it and puts it in the passed
//				string. 
//
// Arguments:
//	char* szString		Buffer to store result.
//
// Returns: None.
//==================================================================================
void StringGetCurrentDateStrShort(char* szString)
{
	time_t		tTime;
	struct tm*	ptmLocalTime;


	time(&tTime);
	ptmLocalTime = localtime(&tTime);
	strftime(szString, 128, "%I:%M:%S%p %a. %b. %d, %Y", ptmLocalTime);
} // StringGetCurrentDateStrShort
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringGetCurrentTimeStr()"
//==================================================================================
// StringGetCurrentTimeStr
//----------------------------------------------------------------------------------
//
// Description: Gets the current time, formats it and puts it in the passed string. 
//
// Arguments:
//	char* szString		Buffer to store result.
//
// Returns: None.
//==================================================================================
void StringGetCurrentTimeStr(char* szString)
{
	time_t		tTime;
	struct tm*	ptmLocalTime;


	time(&tTime);
	ptmLocalTime = localtime(&tTime);
	strftime(szString, 128, "%I:%M:%S%p", ptmLocalTime);
} // StringGetCurrentTimeStr
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringGetStrFromFiletime()"
//==================================================================================
// StringGetStrFromFiletime
//----------------------------------------------------------------------------------
//
// Description: Converts the passed in file time into a string.  If the abbreviated
//				version is requested, day and month names are shortened.
//
// Arguments:
//	FILETIME* pFiletime		File time to convert.
//	BOOL fAbbreviated		Should the string be more compact?
//	char* szString			The string to store the results in.
//
// Returns: None
//==================================================================================
void StringGetStrFromFiletime(FILETIME* pFiletime, BOOL fAbbreviated,
								char* szString)
{
	BOOL			fZero = TRUE;
	LPBYTE			pCurrent = (LPBYTE) pFiletime;
	FILETIME		ftLocal;
	SYSTEMTIME		systime;
	char			szTemp[1024];

	if (pFiletime == NULL)
		return;

	while (pCurrent < (LPBYTE) (pFiletime + 1))
	{
		if ((*pCurrent) != 0)
		{
			fZero = FALSE;
			break;
		} // end if (non-zero byte)

		pCurrent++;
	} // end while (haven't hit end of buffer)

	if (fZero)
	{
		strcpy(szString, "unknown");
		return;
	} // end if (no time)


	// Convert the file time from UTC (Coordinated Universal Time) to local time
	ZeroMemory(&ftLocal, sizeof (FILETIME));
	if (! FileTimeToLocalFileTime(pFiletime, &ftLocal))
		return;

	// Convert the local time into an easily parsable format.
	if (! FileTimeToSystemTime(&ftLocal, &systime))
		return;


	wsprintf(szString, "%i:%02i:%02i",
			((systime.wHour > 12) ? (systime.wHour - 12) : ((systime.wHour == 0) ? 12 : systime.wHour)),
			systime.wMinute,
			systime.wSecond);

	if (! fAbbreviated)
	{
		// Tack on an extra space
		strcat(szString, " ");
	} // end if (not abbreviated)

	if (systime.wHour > 12)
	{
		strcat(szString, "PM ");
	} // end if (PM)
	else
	{
		strcat(szString, "AM ");
	} // end else (AM)

	if (! fAbbreviated)
	{
		// Tack on an extra space
		strcat(szString, " ");
	} // end if (not abbreviated)

	switch (systime.wDayOfWeek)
	{
		case 0:
			if (fAbbreviated)
				strcat(szString, "Sun. ");
			else
				strcat(szString, "Sunday, ");
		  break;

		case 1:
			if (fAbbreviated)
				strcat(szString, "Mon. ");
			else
				strcat(szString, "Monday, ");
		  break;

		case 2:
			if (fAbbreviated)
				strcat(szString, "Tue. ");
			else
				strcat(szString, "Tuesday, ");
		  break;

		case 3:
			if (fAbbreviated)
				strcat(szString, "Wed. ");
			else
				strcat(szString, "Wednesday, ");
		  break;

		case 4:
			if (fAbbreviated)
				strcat(szString, "Thu. ");
			else
				strcat(szString, "Thursday, ");
		  break;

		case 5:
			if (fAbbreviated)
				strcat(szString, "Fri. ");
			else
				strcat(szString, "Friday, ");
		  break;

		case 6:
			if (fAbbreviated)
				strcat(szString, "Sat. ");
			else
				strcat(szString, "Saturday, ");
		  break;
	} // end switch (on day of week)

	switch (systime.wMonth)
	{
		case 1:
			if (fAbbreviated)
				strcat(szString, "Jan. ");
			else
				strcat(szString, "January ");
		  break;

		case 2:
			if (fAbbreviated)
				strcat(szString, "Feb. ");
			else
				strcat(szString, "February ");
		  break;

		case 3:
			if (fAbbreviated)
				strcat(szString, "Mar. ");
			else
				strcat(szString, "March ");
		  break;

		case 4:
			if (fAbbreviated)
				strcat(szString, "Apr. ");
			else
				strcat(szString, "April ");
		  break;

		case 5:
			strcat(szString, "May ");
		  break;

		case 6:
			if (fAbbreviated)
				strcat(szString, "Jun. ");
			else
				strcat(szString, "June ");
		  break;

		case 7:
			if (fAbbreviated)
				strcat(szString, "Jul. ");
			else
				strcat(szString, "July ");
		  break;

		case 8:
			if (fAbbreviated)
				strcat(szString, "Aug. ");
			else
				strcat(szString, "August ");
		  break;

		case 9:
			if (fAbbreviated)
				strcat(szString, "Sep. ");
			else
				strcat(szString, "September ");
		  break;

		case 10:
			if (fAbbreviated)
				strcat(szString, "Oct. ");
			else
				strcat(szString, "October ");
		  break;

		case 11:
			if (fAbbreviated)
				strcat(szString, "Nov. ");
			else
				strcat(szString, "November ");
		  break;

		case 12:
			if (fAbbreviated)
				strcat(szString, "Dec. ");
			else
				strcat(szString, "December ");
		  break;
	} // end switch (on month)

	wsprintf(szTemp, "%i, %i", systime.wDay, systime.wYear);

	strcat(szString, szTemp);
} // StringGetStrFromFiletime
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringCpyAToU()"
//==================================================================================
// StringCpyAToU
//----------------------------------------------------------------------------------
//
// Description: Performs like strcpy, except the destination string is a Unicode
//				string.  Just like strcpy, the destination buffer is assumed to be
//				large enough to hold it.
//
// Arguments:
//	WCHAR* wszUnicodeString		Unicode destination string to write to.
//	char* szANSIString			ANSI source string to copy from.
//
// Returns: Pointer to the Unicode string passed in.
//==================================================================================
WCHAR* StringCpyAToU(WCHAR* wszUnicodeString, char* szANSIString)
{
	int		i;


	i = MultiByteToWideChar(CP_ACP, 0, szANSIString, -1, wszUnicodeString,
							(strlen(szANSIString) + 1));

	if (i == 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't convert string to Unicode!  %e", 1, GetLastError());
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		ZeroMemory(wszUnicodeString, (strlen(szANSIString) + 1) * sizeof (WCHAR));
	} // end if (the function failed)
	
	return (wszUnicodeString);
} // StringCpyAToU
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringCpyUToA()"
//==================================================================================
// StringCpyUToA
//----------------------------------------------------------------------------------
//
// Description: Performs like strcpy, except the source string is a Unicode
//				string.  Just like strcpy, the destination buffer is assumed to be
//				large enough to hold it.
//
// Arguments:
//	char* szANSIString			ANSI source string to write to.
//	WCHAR* wszUnicodeString		Unicode destination string to copy from.
//
// Returns: Pointer to the Unicode string passed in.
//==================================================================================
char* StringCpyUToA(char* szANSIString, WCHAR* wszUnicodeString)
{
	int		i;


	i = WideCharToMultiByte(CP_ACP, 0, wszUnicodeString, -1, szANSIString,
							(wcslen(wszUnicodeString) + 1), NULL, NULL);
	if (i == 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't convert string to ANSI!  %e", 1, GetLastError());
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		ZeroMemory(szANSIString, (wcslen(wszUnicodeString) + 1));
	} // end if (the function failed)
	
	return (szANSIString);
} // StringCpyUToA
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringCatAToU()"
//==================================================================================
// StringCatAToU
//----------------------------------------------------------------------------------
//
// Description: Performs like strcat, except the destination string is a Unicode
//				string.  Just like strcat, the destination buffer is assumed to be
//				large enough to hold the concatenated string.
//
// Arguments:
//	WCHAR* wszUnicodeString		Unicode destination string to write to.
//	char* szANSIString			ANSI source string to copy from.
//
// Returns: Pointer to the Unicode string passed in.
//==================================================================================
WCHAR* StringCatAToU(WCHAR* wszUnicodeString, char* szANSIString)
{
	int		i;


	i = MultiByteToWideChar(CP_ACP, 0, szANSIString, -1,
							wszUnicodeString + lstrlenW(wszUnicodeString),
							(strlen(szANSIString) + 1));

	if (i == 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't convert string to Unicode!  %e", 1, GetLastError());
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		ZeroMemory(wszUnicodeString + lstrlenW(wszUnicodeString),
					(strlen(szANSIString) + 1) * sizeof (WCHAR));
	} // end if (the function failed)
	
	return (wszUnicodeString);
} // StringCatAToU
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringCmpAToU()"
//==================================================================================
// StringCmpAToU
//----------------------------------------------------------------------------------
//
// Description: Performs similar to strcmp, except the first string is Unicode, and
//				it returns TRUE if they match (case insensitive, if specified),
//				FALSE otherwise.
//
// Arguments:
//	WCHAR* wszUnicodeString		Unicode string to compare.
//	char* szANSIString			ANSI string to compare.
//	BOOL fMatchCase				Whether case matters or not.
//
// Returns: Returns TRUE if they match, FALSE otherwise.
//==================================================================================
BOOL StringCmpAToU(WCHAR* wszUnicodeString, char* szANSIString, BOOL fMatchCase)
{
	BOOL	fResult = FALSE;
	int		i;
	WCHAR*	pwszANSIAsUnicode = NULL;
	WCHAR*	pwszUnicodeLwr = NULL;
	WCHAR*	pwcCurrent1 = NULL;
	WCHAR*	pwcCurrent2 = NULL;



	i = MultiByteToWideChar(CP_ACP, 0, szANSIString, -1, NULL, 0);
	if (i <= 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't get size of ANSI string in Unicode!  %e", 1, GetLastError());
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (couldn't convert string)


	pwszANSIAsUnicode = (WCHAR*) LocalAlloc(LPTR, (i * sizeof (WCHAR)));
	if (pwszANSIAsUnicode == NULL)
	{
		goto DONE;
	} // end if (couldn't allocate memory)
	ZeroMemory(pwszANSIAsUnicode, i * sizeof (WCHAR));

	i = MultiByteToWideChar(CP_ACP, 0, szANSIString, -1, pwszANSIAsUnicode, i);
	if (i <= 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't convert string to Unicode!  %e", 1, GetLastError());
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (the function failed)


	// Make strings lower case for case-insensitive matching, if requested
	if (! fMatchCase)
	{
		_wcslwr(pwszANSIAsUnicode);

		pwszUnicodeLwr = (WCHAR*) LocalAlloc(LPTR, ((wcslen(wszUnicodeString) + 1) * sizeof (WCHAR)));
		if (pwszUnicodeLwr == NULL)
		{
			goto DONE;
		} // end if (couldn't allocate memory)
		wcscpy(pwszUnicodeLwr, wszUnicodeString);
		_wcslwr(pwszUnicodeLwr);
	} // end if (case insensitive)

	// Compare the strings
	fResult = ((wcscmp((fMatchCase ? wszUnicodeString : pwszUnicodeLwr),
				pwszANSIAsUnicode)) == 0) ? TRUE : FALSE;


DONE:

	if (pwszANSIAsUnicode != NULL)
	{
		LocalFree(pwszANSIAsUnicode);
		pwszANSIAsUnicode = NULL;
	} // end if (allocated string)

	if (pwszUnicodeLwr != NULL)
	{
		LocalFree(pwszUnicodeLwr);
		pwszUnicodeLwr = NULL;
	} // end if (allocated string)

	return (fResult);
} // StringCmpAToU
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringGetFlags()"
//==================================================================================
// StringGetFlags
//----------------------------------------------------------------------------------
//
// Description: Converts the passed in textual representation of flags to their
//				actual values.  The flags must be separated by the pipe ('|') and/or
//				space (' ') characters.
//
// Arguments:
//	char* szString					The string to parse.
//	PTNFLAGTABLEITEM paFlagTable	Pointer to table to use for flag lookup.
//	DWORD_PTR dwNumFlagsInTable		Number of elements in the table being used.
//	BOOL fCaseSensitive				Whether case matters when matching flag names or
//									not.
//
// Returns: The valid session flags found.
//==================================================================================
DWORD_PTR StringGetFlags(char* szString, PTNFLAGTABLEITEM paFlagTable,
						DWORD_PTR dwNumFlagsInTable, BOOL fCaseSensitive)
{
	DWORD_PTR	dwResultFlags = 0;
	char*		pszFields = NULL;
	DWORD_PTR	dwSize = 0;
	DWORD_PTR	dwNumFields = 0;
	DWORD_PTR	dwTemp;
	DWORD_PTR	dwTemp2;
	char*		pszCurrentField = NULL;


	dwSize = strlen(szString) + 1;
	pszFields = (char*) LocalAlloc(LPTR, dwSize);
	if (pszFields == NULL)
		return (0);

	dwNumFields = StringSplitIntoFields(szString, " |", pszFields, &dwSize);
	for(dwTemp = 0; dwTemp < dwNumFields; dwTemp++)
	{
		pszCurrentField = StringGetFieldPtr(pszFields, dwTemp);

		for(dwTemp2 = 0; dwTemp2 < dwNumFlagsInTable; dwTemp2++)
		{
			if (fCaseSensitive)
			{
				if (strcmp(pszCurrentField, paFlagTable[dwTemp2].szString) == 0)
					dwResultFlags |= paFlagTable[dwTemp2].dwFlag;
			} // end if (case sensitive)
			else
			{
				if (StringCmpNoCase(pszCurrentField, paFlagTable[dwTemp2].szString))
					dwResultFlags |= paFlagTable[dwTemp2].dwFlag;
			} // end else (not case sensitive)
		} // end for (each known flag)
	} // end for (each field)

	LocalFree(pszFields);
	pszFields = NULL;

	return (dwResultFlags);
} // StringGetFlags
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringAddFlags()"
//==================================================================================
// StringAddFlags
//----------------------------------------------------------------------------------
//
// Description: Converts the flags contained in the passed in DWORD into their
//				textual representation separated by pipe ('|') characters and
//				concatenates them onto the end of the string passed in.
//				If the passed in string pointer is NULL, the DWORD passed in is set
//				to the size required by the string (including NULL terminator), and
//				ERROR_BUFFER_TOO_SMALL is returned.
//				If no valid flags were found, the string size required will be 0.
//				Note: space for 3 extra characters is required during the function's
//				execution, but this is included in the string size returned.
//
// Arguments:
//	DWORD_PTR dwFlags				Flags to convert.
//	PTNFLAGTABLEITEM paFlagTable	Pointer to table to use for flag lookup.
//	DWORD_PTR dwNumFlagsInTable		Number of elements in the table being used.
//	char* pszString					Pointer to string to which text will be added.
//	DWORD_PTR* pdwStringSize		Pointer to size of string buffer, or place to
//									store size required.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT StringAddFlags(DWORD_PTR dwFlags, PTNFLAGTABLEITEM paFlagTable,
						DWORD_PTR dwNumFlagsInTable, char* pszString,
						DWORD_PTR* pdwStringSize)
{
	DWORD_PTR	dwTemp;


	if (pszString == NULL)
		(*pdwStringSize) = 0;


	if (dwFlags != 0)
	{
		for(dwTemp = 0; dwTemp < dwNumFlagsInTable; dwTemp++)
		{
			if (dwFlags & (paFlagTable[dwTemp].dwFlag))
			{
				if (pszString == NULL)
					(*pdwStringSize) += strlen(paFlagTable[dwTemp].szString) + 3;
				else
				{
					strcat(pszString, paFlagTable[dwTemp].szString);
					strcat(pszString, " | ");
				} // end else (there's a buffer to copy to)

				dwFlags -= paFlagTable[dwTemp].dwFlag;

				if (dwFlags == 0)
					break;
			} // end if (we found a match)
		} // end for (each known flag)
	} // end if (there are any flags)

	// If there was no buffer to copy to, return the size required and BUFFER_TOO_SMALL
	if (pszString == NULL)
	{
		// If there are any items to display, also include room for a NULL terminator
		if ((*pdwStringSize) != 0)
			(*pdwStringSize)++;
		
		return (ERROR_BUFFER_TOO_SMALL);
	} // end if (there was no buffer)

	// If we actually added some flags, the string will end with " | ", which looks
	// dumb, so lets chop that off.
	if (strcmp(pszString, "") != 0)
	{
		pszString[strlen(pszString) - 3] = '\0'; // chop off the last 3 characters
	} // end if (we actually copied flags)
	
	return (S_OK);
} // StringAddFlags
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
