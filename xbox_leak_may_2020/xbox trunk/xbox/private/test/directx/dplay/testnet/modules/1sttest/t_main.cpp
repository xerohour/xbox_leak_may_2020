//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include <objbase.h>
#include <windows.h>
#include <initguid.h>

#include "..\..\tncommon\debugprint.h"
#include "..\..\tncommon\cppobjhelp.h"
#include "..\..\tncommon\linklist.h"
#include "..\..\tncommon\linkedstr.h"
#include "..\..\tncommon\cmdline.h"
#include "..\..\tncommon\excptns.h"

#include "..\..\tncontrl\tncontrl.h"

#include "..\..\tnshell\dll\tnshell.h"


#include "t_version.h"
#include "t_main.h"
#include "1sttest.h"




//==================================================================================
// Globals
//==================================================================================
BOOL		g_fMaster = FALSE;
DWORD		g_dwMode = 0;
char*		g_pszDirectory = NULL;






//==================================================================================
// Statics
//==================================================================================
static TNMODULEID	s_ModuleID =	{	TNMOD_SHORTNAMECAPS,		// short name, must match
										TNMOD_VERSION_MATCH_MAJOR,	// major version, must match
										TNMOD_VERSION_MATCH_MINOR,	// minor version, must match
										TNMOD_VERSION_MINOR,		// minor version, doesn't matter
										TNMOD_VERSION_BUILD			// build number, doesn't matter
									};

static HINSTANCE	s_hInstance = NULL;





//==================================================================================
// Prototypes
//==================================================================================
HRESULT TNM_LoadTestTable(PTNTESTTABLEGROUP pTable);

HRESULT TNM_InitModuleTesting(PTNMACHINEINFO pInfo);

HRESULT TNM_CleanupTesting(PTNMACHINEINFO pInfo);

HRESULT TNM_AddImportantBinaries(PTNTRACKEDBINARIES pBinList);

HRESULT TNM_GetModuleInfo(PTNSHELLINFO pTNsi, PTNMODULEINFO pTNmi);
HRESULT TNM_InitModule(PTNINITMODULEDATA pTNid);
HRESULT TNM_Release(void);



#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNM_GetModuleInfo()"
//==================================================================================
// TNM_GetModuleInfo
//----------------------------------------------------------------------------------
//
// Description: Mandatory entry point for module that the shell uses to retrieve
//				information on this DLL so the user can choose which to load.
//				This is called with NULL pointers to strings, which requires us to
//				fill in the size required for the strings so the shell can call us
//				again with appropriate buffers.  We return ERROR_BUFFER_TOO_SMALL
//				if that's the case.
//
// Arguments:
//	PTNSHELLINFO pTNsi		Pointer to structure describing shell that is calling
//							us.
//	pTNmi pTNmi		Pointer to structure to return info in.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNM_GetModuleInfo(PTNSHELLINFO pTNsi, PTNMODULEINFO pTNmi)
{
	HRESULT		hr = S_OK;


	// Don't bother printing warnings if the shell and control layers are different
	// versions from what we expected, because the shell may not end up loading us.
	// The information is only marginally useful to us in this call, and is here
	// in case in the future we need to alter how we respond to GetModuleInfo which
	// is not very likely.
	pTNmi->dwShellAPIVersion = CURRENT_TNSHELL_API_VERSION;
	pTNmi->dwControlAPIVersion = CURRENT_TNCONTROL_API_VERSION;


	// Copy the module ID

	memcpy(&(pTNmi->moduleid), &(s_ModuleID), sizeof (TNMODULEID));


	// Return the name string

	if ((pTNmi->pszName == NULL) ||
		(pTNmi->dwNameSize < (strlen(TNMOD_NAME) + 1)))
	{
		hr = ERROR_BUFFER_TOO_SMALL;
		pTNmi->dwNameSize = strlen(TNMOD_NAME) + 1;
	} // end if (the buffer to copy the name in isn't big enough)
	else
		strcpy(pTNmi->pszName, TNMOD_NAME);


	// Return the description string

	if ((pTNmi->pszDescription == NULL) ||
		(pTNmi->dwDescriptionSize < (strlen(TNMOD_DESCRIPTION) + 1)))
	{
		hr = ERROR_BUFFER_TOO_SMALL;
		pTNmi->dwDescriptionSize = strlen(TNMOD_DESCRIPTION) + 1;
	} // end if (the buffer to copy the name in isn't big enough)
	else
		strcpy(pTNmi->pszDescription, TNMOD_DESCRIPTION);


	// Return the authors string

	if ((pTNmi->pszAuthors == NULL) ||
		(pTNmi->dwAuthorsSize < (strlen(TNMOD_AUTHORS) + 1)))
	{
		hr = ERROR_BUFFER_TOO_SMALL;
		pTNmi->dwAuthorsSize = strlen(TNMOD_AUTHORS) + 1;
	} // end if (the buffer to copy the name in isn't big enough)
	else
		strcpy(pTNmi->pszAuthors, TNMOD_AUTHORS);


	return (hr);
} // TNM_GetModuleInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNM_InitModule()"
//==================================================================================
// TNM_InitModule
//----------------------------------------------------------------------------------
//
// Description: Mandatory entry point for module that allows it to initialize any
//				data it may need and to extract information from the command line.
//				The module's other functions, such as GetStartupData() are returned
//				in the TNMODFUNCS structure of the init data, and control layer
//				options required are returned in the given DWORD.
//
// Arguments:
//	PTNINITMODULEDATA pTNid		Pointer to data to use when initializing. Also has
//								items for module to fill out.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNM_InitModule(PTNINITMODULEDATA pTNid)
{
	HRESULT		hr;


	// Don't print warnings for older API versions, since we can assume we know
	// how to be backward compatible.

	if (pTNid->shellinfo.dwShellAPIVersion != CURRENT_TNSHELL_API_VERSION)
	{
		DPL(0, "WARNING: Shell using different API version (%i != %i), problems may exist.",
			2, pTNid->shellinfo.dwShellAPIVersion,  CURRENT_TNSHELL_API_VERSION);
	} // end if (we're working with a shell that's using a higher API version)
	if (pTNid->shellinfo.dwControlAPIVersion != CURRENT_TNCONTROL_API_VERSION)
	{
		DPL(0, "WARNING: Control layer using different API version (%i != %i), problems may exist.",
			2, pTNid->shellinfo.dwControlAPIVersion,  CURRENT_TNCONTROL_API_VERSION);
	} // end if (we're working with a control layer that's using a higher API version)

	// Handle the command line
	g_fMaster = pTNid->fMaster;
	g_dwMode = pTNid->dwMode;


	// Copy the directory we reside in
	g_pszDirectory = (char*) LocalAlloc(LPTR, (strlen(pTNid->pszModuleDirPath) + 1));
	if (g_pszDirectory == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)

	strcpy(g_pszDirectory, pTNid->pszModuleDirPath);



	// Fill in the functions we implement
	pTNid->TNModuleFuncs.pfnLoadTestTable = TNM_LoadTestTable;
	pTNid->TNModuleFuncs.pfnInitializeTesting = TNM_InitModuleTesting;
	pTNid->TNModuleFuncs.pfnCleanupTesting = TNM_CleanupTesting;
	pTNid->TNModuleFuncs.pfnAddImportantBinaries = TNM_AddImportantBinaries;
//	pTNid->TNModuleFuncs.pfnGetPokePages = TNM_GetPokePages;
	pTNid->TNModuleFuncs.pfnGetPokePages = NULL;

	// Return the control layer options we need.
	pTNid->dwControlLayerFlags = TNCLR_INITIALIZE_COM_FOR_TESTTHREAD; 

	return (S_OK);


ERROR_EXIT:

	// If we fail to initialize, Shutdown() won't be called, so we have to clean
	// up any partially allocated things here.

	if (g_pszDirectory != NULL)
	{
		LocalFree(g_pszDirectory);
		g_pszDirectory = NULL;
	} // end if (allocated a directory)

	return (hr);
} // TNM_InitModule
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNM_Release()"
//==================================================================================
// TNM_Release
//----------------------------------------------------------------------------------
//
// Description: Mandatory entry point for module that allows it to clean up any data
//				it used during its lifetime.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNM_Release(void)
{
	// If this function is being called, it means that Initialize completed
	// successfully.

	if (g_pszDirectory != NULL)
	{
		LocalFree(g_pszDirectory);
		g_pszDirectory = NULL;
	} // end if (we allocated a directory string)

	return (S_OK);
} // TNM_Release
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNM_LoadTestTable()"
//==================================================================================
// TNM_LoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Mandatory entry point for module that loads all the possible tests
//				into the table passed in.
//
// Arguments:
//	PTNTESTTABLEGROUP pTable	Table to fill with tests in this file.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNM_LoadTestTable(PTNTESTTABLEGROUP pTable)
{
	HRESULT		hr;


	hr = FirstTestLoadTestTable(pTable);
	if (hr != S_OK)
	{
		DPL(0, "Loading First tests failed!", 0);
		return (hr);
	} // end if (loading tests failed)

	return (S_OK);
} // TNM_LoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNM_InitModuleTesting()"
//==================================================================================
// TNM_InitModuleTesting
//----------------------------------------------------------------------------------
//
// Description: Called before testing begins.  This allows us to allocate any
//				resources we need and prep for testing.
//
// Arguments:
//	PTNMACHINEINFO pInfo	Pointer to current info for this machine.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNM_InitModuleTesting(PTNMACHINEINFO pInfo)
{
	// Do any initialization here.

	return (S_OK);
} // TNM_InitModuleTesting
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNM_CleanupTesting()"
//==================================================================================
// TNM_CleanupTesting
//----------------------------------------------------------------------------------
//
// Description: Called after testing completes.  This allows us to free any
//				resources we allocated during testing.
//
// Arguments:
//	PTNMACHINEINFO pInfo	Pointer to current info for this machine.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNM_CleanupTesting(PTNMACHINEINFO pInfo)
{
	// Do any cleanup here.

	return (S_OK);
} // TNM_CleanupTesting
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNM_AddImportantBinaries()"
//==================================================================================
// TNM_AddImportantBinaries
//----------------------------------------------------------------------------------
//
// Description: Callback that allows the module to select the binaries that are
//				relevant/important or whose versions impact the testing that will be
//				done.
//
// Arguments:
//	PTNTRACKEDBINARIES pBinList		List to attach important binaries to.
//
// Returns: S_OK if successful, the error code otherwise.
//==================================================================================
HRESULT TNM_AddImportantBinaries(PTNTRACKEDBINARIES pBinList)
{
	HRESULT			hr;


	// Specify whatever binaries you think are important instead of these.  The
	// file information for these will show up in the UI and is also easily
	// available during the test.  You can key off of it for version specific tests,
	// for example.

	hr = pBinList->LocateAndAddBinary("kernel32.dll");
	if (hr != S_OK)
	{
		DPL(0, "Couldn't identify kernel32.dll binary!", 0);
		return (hr);
	} // end if (couldn't identify binary)

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	hr = pBinList->LocateAndAddBinary("user32.dll");
	if (hr != S_OK)
	{
		DPL(0, "Couldn't identify user32.dll binary!", 0);
		return (hr);
	} // end if (couldn't identify binary)

	return (S_OK);
} // TNM_AddImportantBinaries
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






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
// Returns: 0 if all goes well.
//==================================================================================
BOOL WINAPI DllMain(HINSTANCE hmod, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			TNDebugInitialize();

//			DPL(0, "Attaching to process %x.", 1, GetCurrentProcessId());

			s_hInstance = hmod;
		  break;

		case DLL_PROCESS_DETACH:
//			DPL(0, "Detaching from process %x.", 1, GetCurrentProcessId());

			TNDebugCleanup();
		  break;
	} // end switch (on the reason we're being called here)

	return (TRUE);
} // DllMain
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
