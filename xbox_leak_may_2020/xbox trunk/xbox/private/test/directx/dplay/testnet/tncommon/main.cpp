//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "main.h"
#include "sprintf.h"
#include "debugprint.h"
#ifdef DEBUG
#include "cppobjhelp.h"
#include "symbols.h"
#endif // DEBUG

#include "version.h"



//==================================================================================
// Debugging help
//==================================================================================
char	g_szTNCOMMONVersion[] = TNCOMMON_VERSION_STRING;	




//==================================================================================
// External Statics
//==================================================================================
HINSTANCE	s_hInstance = NULL;




extern DWORD	g_dwStringsSize;




#undef DEBUG_SECTION
#define DEBUG_SECTION	"DllMain()"
//==================================================================================
// DllMain
//----------------------------------------------------------------------------------
//
// Description: DLL entry point.
//
// Arguments:
//	HINSTANCE hmod		Handle to this DLL module.
//	DWORD dwReason		Reason for calling this function.
//	LPVOID lpvReserved	Reserved.
//
// Returns: TRUE if all goes well.
//==================================================================================
BOOL WINAPI DllMain(HINSTANCE hmod, DWORD dwReason, LPVOID lpvReserved)
{
#ifdef DEBUG
	HRESULT		hr;
#endif // DEBUG


	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
#ifdef DEBUG
			InitCPPObjHelp();
#endif // DEBUG

			TNsprintfInitialize();

#ifdef DEBUG
			TNDebugInitialize();

#ifndef _XBOX // no symbol initialization
			hr = TNSymInitialize();
			if (hr != S_OK)
			{
				DPL(0, "Couldn't initialize symbol functions!  %e", 1, hr);
			} // end if (couldn't initialize symbols)
#endif // ! XBOX

			DPL(1, "v%02u.%02u.%02u.%04u: attaching to process %x, hinstance = %x.",
				6, TNCOMMON_VERSION_MAJOR, TNCOMMON_VERSION_MINOR1,
				TNCOMMON_VERSION_MINOR2, TNCOMMON_VERSION_BUILD,
				GetCurrentProcessId(), hmod);
#endif // DEBUG

			s_hInstance = hmod;
		  break;

		case DLL_PROCESS_DETACH:
#ifndef _XBOX // No current process stuff...
			DPL(1, "Detaching from process %x, hinstance = %x.",
				2, GetCurrentProcessId(), hmod);
#endif // ! XBOX

#ifdef DEBUG
			//CheckForCPPObjLeaks();


			hr = TNSymCleanup();
			if (hr != S_OK)
			{
				DPL(0, "Couldn't cleanup symbol functions!  %e", 1, hr);
			} // end if (couldn't cleanup symbols)

			TNDebugCleanup();
#endif //DEBUG

			TNsprintfCleanup();

#ifdef DEBUG
			CleanupCPPObjHelp();
#endif // ! XBOX
		  break;
	} // end switch (on the reason we're being called here)

	return (TRUE);
} // DllMain
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
