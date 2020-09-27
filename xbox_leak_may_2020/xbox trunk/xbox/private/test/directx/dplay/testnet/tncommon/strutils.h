#ifndef __TNCOMMON_STRUTILS__
#define __TNCOMMON_STRUTILS__





//==================================================================================
// Defines
//==================================================================================
#ifdef LOCAL_TNCOMMON

	// define LOCAL_TNCOMMON when including this code directly into your project
	#ifdef DLLEXPORT
		#undef DLLEXPORT
	#endif // DLLEXPORT defined
	#define DLLEXPORT

#else // ! LOCAL_TNCOMMON

	#ifdef TNCOMMON_EXPORTS

		// define TNCOMMON_EXPORTS only when building the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllexport)

	#else // ! TNCOMMON_EXPORTS

		// default behavior is to import the functions from the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllimport)

	#endif // ! TNCOMMON_EXPORTS
#endif // ! LOCAL_TNCOMMON

#ifndef DEBUG
	#ifdef _DEBUG
		#define DEBUG
	#endif // _DEBUG
#endif // DEBUG not defined

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef ERROR_BUFFER_TOO_SMALL
#define ERROR_BUFFER_TOO_SMALL		603L
#endif // ! ERROR_BUFFER_TOO_SMALL





//==================================================================================
// Array element definition
//==================================================================================
typedef struct tagTNFLAGTABLEITEM
{
	DWORD_PTR	dwFlag; // the flag value
	char*		szString; // textual representation of the flag
} TNFLAGTABLEITEM, * PTNFLAGTABLEITEM;





//==================================================================================
// Prototypes
//==================================================================================
DLLEXPORT BOOL StringStartsWith(char* szString, char* szPrefix, BOOL fMatchCase);

DLLEXPORT BOOL StringEndsWith(char* szString, char* szSuffix, BOOL fMatchCase);


DLLEXPORT int StringContainsChar(char* szString, char cCharToFind, BOOL fMatchCase,
								int iPos);

DLLEXPORT int StringCountNumChars(char* szString, char cCharToFind, BOOL fMatchCase);

DLLEXPORT int StringContainsOneOfChars(char* szString, char* szCharsToFind,
										BOOL fMatchCase, int iPos);

DLLEXPORT int StringContainsNonChar(char* szString, char* szCharsAllowed,
									BOOL fMatchCase, int iPos);


DLLEXPORT BOOL StringCmpNoCase(char* szString1, char* szString2);

DLLEXPORT BOOL StringMatchesWithWildcard(char* szWildcardString, char* szCompareString,
										BOOL fMatchCase);

DLLEXPORT int StringContainsString(char* szString, char* szSubstring,
									BOOL fMatchCase, int iPos);


DLLEXPORT BOOL StringIsTrue(char* szString);

DLLEXPORT void StringGetKey(char* szString, char* szResult);

DLLEXPORT char* StringGetKeyValue(char* szString);

DLLEXPORT BOOL StringIsKey(char* szString, char* szKeyName);

DLLEXPORT BOOL StringIsValueTrue(char* szString);


DLLEXPORT BOOL StringIsCharBackslashEscaped(char* szString, char* pcChar);



DLLEXPORT int StringPopLeadingChars(char* szString, char* szPopChars,
									BOOL fMatchCase);

DLLEXPORT int StringPopTrailingChars(char* szString, char* szPopChars,
									BOOL fMatchCase);


DLLEXPORT void StringPrepend(char* szString, char* szPrefix);

DLLEXPORT BOOL StringSplit(char* szString, unsigned int uiSplitPoint,
							char* szFirst, char* szLast);


DLLEXPORT unsigned int StringSplitIntoFixedFields(char* szString,
												char* szSplitChars,
												char* resultArray,
												unsigned int uiMaxResults,
												unsigned int uiStringLength);

DLLEXPORT DWORD StringSplitIntoFields(char* szString, char* szSplitChars,
									char* pszResultBuffer,
									DWORD* pdwResultBufferSize);

DLLEXPORT char* StringGetFieldPtr(char* szFieldsBuffer, DWORD dwFieldNum);


DLLEXPORT HRESULT StringGetBetween(char* szSource,
								   char* szStartToken,
									char* pszEndToken,
									BOOL fMatchCase,
									BOOL fKeepTokensInOutside,
									BOOL fKeepTokensInInside,
									BOOL fCheckForNonEscapedStart,
									BOOL fCheckForNonEscapedEnd,
									int* piPos,
									char* pszOutside,
									char* pszInside,
									BOOL* pfUnfinished);

DLLEXPORT int StringReplaceAll(char* szString, char* szSearchString,
								char* szReplaceString, BOOL fMatchCase, int iPos,
								unsigned int uiStringLength);


DLLEXPORT int StringToInt(char* szString);

DLLEXPORT DWORD StringNumDWordDigits(DWORD dwValue);

DLLEXPORT DWORD StringToDWord(char* szString);

DLLEXPORT DWORD StringHexToDWord(char* szString);

DLLEXPORT HRESULT StringToGuid(char* szString, GUID* pguidResult);

DLLEXPORT void StringDWordToIPAddress(DWORD dwValue, char* szResult);

DLLEXPORT DWORD StringNumIntDigits(int iValue);

DLLEXPORT void StringIntToZeroPaddedStr(int iNum, int iDigits, char* szString);

DLLEXPORT char StringIntToChar(int iNum);

DLLEXPORT void StringBufferToHexStr(PVOID pvBuffer, DWORD dwBufferSize, char* szResult);


DLLEXPORT HRESULT StringEnclosePathSpacesInQuotes(char* szPath,
												char* pszResultBuffer,
												DWORD* pdwResultBufferSize);


DLLEXPORT HRESULT StringURLEncode(char* szString, char* pszEncodedString,
								DWORD* pdwEncodedStringSize);


DLLEXPORT void StringGetCurrentDateStr(char* szString);

DLLEXPORT void StringGetCurrentDateStrShort(char* szString);

DLLEXPORT void StringGetCurrentTimeStr(char* szString);


DLLEXPORT void StringGetStrFromFiletime(FILETIME* pFiletime, BOOL fAbbreviated,
										char* szString);


DLLEXPORT WCHAR* StringCpyAToU(WCHAR* wszUnicodeString, char* szANSIString);

DLLEXPORT char* StringCpyUToA(char* szANSIString, WCHAR* wszUnicodeString);

DLLEXPORT WCHAR* StringCatAToU(WCHAR* wszUnicodeString, char* szANSIString);

DLLEXPORT BOOL StringCmpAToU(WCHAR* wszUnicodeString, char* szANSIString,
							BOOL fMatchCase);

DLLEXPORT DWORD_PTR StringGetFlags(char* szString, PTNFLAGTABLEITEM paFlagTable,
									DWORD_PTR dwNumFlagsInTable,
									BOOL fCaseSensitive);

DLLEXPORT HRESULT StringAddFlags(DWORD_PTR dwFlags, PTNFLAGTABLEITEM paFlagTable,
								DWORD_PTR dwNumFlagsInTable, char* pszString,
								DWORD_PTR* pdwStringSize);




#endif // __TNCOMMON_STRUTILS__
