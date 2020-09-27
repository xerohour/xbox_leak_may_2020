#ifndef __ERRORS__
#define __ERRORS__





//==================================================================================
// Common Defines
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




//=========================================================================================
// Defines
//=========================================================================================
// Prefix the name of the error with the hexdecimal value.
#define ERRORSFLAG_PRINTHEXVALUE		0x01
// Prefix the name of the error with the decimal value.
#define ERRORSFLAG_PRINTDECIMALVALUE	0x02
// Print a description of the error, if any.
#define ERRORSFLAG_PRINTDESCRIPTION		0x04

#define ERRORSSHORTCUT_NOTSEARCHED		0xFFFFFFFF
#define ERRORSSHORTCUT_NOTFOUND			0xFFFFFFFE




//=========================================================================================
// Structures
//=========================================================================================
typedef struct tagTNERRORITEM
{
	HRESULT		hresult;
	char*		pszName;
	char*		pszDescription;
} TNERRORITEM, * PTNERRORITEM;




//=========================================================================================
// Prototypes
//=========================================================================================
DLLEXPORT HRESULT GetErrorDescription(LONG_PTR hresult, DWORD_PTR dwFlags, char* pszBuffer,
									DWORD* pdwBufferSize, DWORD* pdwShortcut);

DLLEXPORT void DoSprintfErrorBox(HWND hWndParent, LPTSTR lptszTitle, LPTSTR lptszFormatString,
								DWORD dwNumParms, ...);

DLLEXPORT void GetTNErrorTable(PTNERRORITEM* ppaTNErrorTable, DWORD* pdwNumEntries);





#endif //__ERRORS__
