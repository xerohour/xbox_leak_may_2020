//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\cmdline.h"

#include "..\tncontrl\tncontrl.h"


#include "resource.h"

#include "tnshell.h"
#include "main.h"
#include "prefs.h"







#ifndef _XBOX // no INI files supporetd
#undef DEBUG_SECTION
#define DEBUG_SECTION	"PrefsStoreCurrentSettings()"
//==================================================================================
// PrefsStoreCurrentSettings
//----------------------------------------------------------------------------------
//
// Description: Saves the current settings to a text file.
//
// Arguments:
//	PCOMMANDLINE pCmdline	Pointer to command line object used.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT PrefsStoreCurrentSettings(PCOMMANDLINE pCmdline)
{
	HRESULT		hr;
	char*		pszTemp;



	pszTemp = (char*) LocalAlloc(LPTR, strlen(g_pszTestNetRootPath) + strlen(LAST_COMMANDLINE_FILE) + 1);
	if (pszTemp == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	strcpy(pszTemp, g_pszTestNetRootPath);
	strcat(pszTemp, LAST_COMMANDLINE_FILE);


	hr = pCmdline->WriteToINI(pszTemp, CMDLINE_WRITEINIOPTION_PRINTDESCRIPTIONS);
	if (hr != S_OK)
	{
		DPL(0, "Writing command line to \"%s\" failed!", 1, pszTemp);
		goto DONE;
	} // end if (writing command line failed)


DONE:

	if (pszTemp != NULL)
	{
		LocalFree(pszTemp);
		pszTemp = NULL;
	} // end if (allocated a string)

	return (hr);
} // PrefsSaveLastPreference
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX