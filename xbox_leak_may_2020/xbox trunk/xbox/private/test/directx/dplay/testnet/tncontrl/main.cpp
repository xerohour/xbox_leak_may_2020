//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include <objbase.h>
#include <cguid.h>
#include <windows.h>
#include <initguid.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"

#include "tncontrl.h"
#include "main.h"

#include "version.h"




//==================================================================================
// Debugging help
//==================================================================================
char	g_szTNCONTRLVersion[] = TNCONTRL_VERSION_STRING;	



//==================================================================================
// External Statics
//==================================================================================
HINSTANCE			s_hInstance = NULL;






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
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			TNDebugInitialize();

#ifndef _XBOX // No GetCurrentProcessId
			DPL(1, "v%02u.%02u.%02u.%04u: attaching to process %x, hinstance = %x.",
				6, TNCONTRL_VERSION_MAJOR, TNCONTRL_VERSION_MINOR1,
				TNCONTRL_VERSION_MINOR2, TNCONTRL_VERSION_BUILD,
				GetCurrentProcessId(), hmod);
#endif // ! XBOX

			s_hInstance = hmod;
		  break;

		case DLL_PROCESS_DETACH:
#ifndef _XBOX // No get current process ID
			DPL(1, "Detaching from process %x, hinstance = %x.",
				2, GetCurrentProcessId(), hmod);
#endif // ! XBOX

			TNDebugCleanup();
		  break;
	} // end switch (on the reason we're being called here)

	return (TRUE);
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
