#ifndef __TNCOMMON_VERUTILS__
#define __TNCOMMON_VERUTILS__





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



//==================================================================================
// Defines
//==================================================================================
#define VEROSID_UNKNOWN			0
#define VEROSID_WINDOWS95GOLD	1
#define VEROSID_WINDOWS95OSR2	2
#define VEROSID_WINDOWSNT4		3
#define VEROSID_WINDOWS98GOLD	4
#define VEROSID_WINDOWS98SE		5
#define VEROSID_WINDOWS2000		6
#define VEROSID_MILLENNIUM		7




//==================================================================================
// Prototypes
//==================================================================================
DLLEXPORT DWORD VerGetOSType(OSVERSIONINFO* lpOSVersion);
DLLEXPORT DWORD VerGetCurrentOSType(void);
#ifndef _XBOX
DLLEXPORT void VerGetStrFromOSVersion(OSVERSIONINFO* pOSVersion, char* szString);
#else
DLLEXPORT void VerGetStrFromOSVersion(OSVERSIONINFOA* pOSVersion, char* szString);
#endif

DLLEXPORT void VerGetStrFromCurrentOSVersion(char* szString);

#ifndef _XBOX // GetFileVersionInfo not supported
DLLEXPORT BOOL VerGetBinaryVersionInfo(char* szFile, DWORD* pdwVersionMS,
										DWORD* pdwVersionLS, BOOL* pfDebug);
#endif // ! XBOX






#endif // __TNCOMMON_STRUTILS__
