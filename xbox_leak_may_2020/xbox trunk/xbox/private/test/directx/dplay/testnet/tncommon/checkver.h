#ifndef _INC_CHECKVER
#define _INC_CHECKVER

#include <windows.h>

/* 
   File versions are made up of a most-significant and least-significant
   version.  Both versions are divided into major and minor versions.  The
   versions dealt with in this library can be thought of as follows:

   HIWORD(dwVerMS).LOWORD(dwVerMS).HIWORD(dwVerLS).LOWORD(dwVerLS)

   where the standard major and minor versions are stored in dwVerMS and the
   build number is stored in the LOWORD of dwVerLS.  To check for a version
   of at least 1.2.3.4, call CheckLibraryVersion() like this:

   CheckLibraryVersion(hWnd, NULL, "foo.dll", MAKELONG(2, 1), MAKELONG(4, 3), TRUE)
*/



//BEGIN vanceo
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
//END vanceo


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// ---------------------------------------------------------------------------
// CheckLibraryVersion
// ---------------------------------------------------------------------------
// Description:         Checks a given dll or exe version against a minimum.
// Arguments:
//  HWND [in]           Parent window handle.
//  LPCSTR [in]         Library path or NULL for the system directory.
//  LPCSTR [in]         Library file name.
//  DWORD [in]          Minimum most-significant version number.
//  DWORD [in]          Minimum least-significant version number.
//  BOOL [in]           TRUE to display an error dialog if the version does
//                      not meet the minimums.
// Returns:
//  BOOL                TRUE if the library meets the minimum version.
#ifndef _XBOX // GetFileVersionInfo not supported
	DLLEXPORT extern BOOL WINAPI CheckLibraryVersion(HWND, LPCSTR, LPCSTR, DWORD, DWORD, BOOL);
#endif // ! XBOX

// ---------------------------------------------------------------------------
// GetLibraryVersion
// ---------------------------------------------------------------------------
// Description:         Retreives the file version for a given dll or exe.
// Arguments:
//  LPCSTR [in]         Library path or NULL for the system directory.
//  LPCSTR [in]         Library file name.
//  LPDWORD [out]       Filled with the most-significant version number.
//  LPDWORD [out]       Filled with the least-significant version number.
//  BOOL [out]			TRUE if these are debug bits, FALSE otherwise.
// Returns:
//  BOOL                TRUE if all goes well.
#ifndef _XBOX // GetFileVersionInfo not supported
DLLEXPORT extern BOOL WINAPI GetLibraryVersion(LPCSTR, LPCSTR, LPDWORD, LPDWORD, LPBOOL);
#endif // ! XBOX




#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _INC_CHECKVER