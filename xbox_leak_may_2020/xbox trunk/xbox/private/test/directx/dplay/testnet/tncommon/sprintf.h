#ifndef __NEW_SPRINTF__
#define __NEW_SPRINTF__





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




//==================================================================================
// Defines
//==================================================================================
#ifdef _WIN64
#define SPRINTF_PTR		"%p"
#else
#define SPRINTF_PTR		"%08x"
#endif





//==================================================================================
// Prototypes
//==================================================================================
DLLEXPORT void TNsprintfInitialize(void);

DLLEXPORT void TNsprintfCleanup(void);


DLLEXPORT void TNsprintf(char** ppszOutput, char* szFormatString,
						DWORD dwNumParms, ...);

DLLEXPORT void TNsprintf_array(char** ppszOutput, char* szFormatString,
								DWORD dwNumParms, PVOID* apvParms);

DLLEXPORT void TNsprintf_free(char** ppszBuffer);


/*
DLLEXPORT void TNsprintfW(WCHAR** ppwszOutput, WCHAR* wszFormatString,
						DWORD dwNumParms, ...);

DLLEXPORT void TNsprintfW_array(WCHAR** ppwszOutput, WCHAR* wszFormatString,
								DWORD dwNumParms, PVOID* apvParms);

DLLEXPORT void TNsprintfW_free(WCHAR** ppwszBuffer);
*/






#endif // __NEW_SPRINTF__