//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <windowsx.h>

#ifdef _XBOX
#include <stdio.h>
#include <stdlib.h>
#endif

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\cmdline.h"
#include "..\tncommon\strutils.h"


#include "..\tncontrl\tncontrl.h"


#include "resource.h"

#include "tnshell.h"
#include "main.h"
#include "prefs.h"
#include "select.h"





//==================================================================================
// Prototypes
//==================================================================================
HRESULT LoadModuleInfos(PMODULEDATA paModules, DWORD* pdwNumModules);
HRESULT InitializeModuleSelectWindow(HINSTANCE hInstance);
HRESULT InitializeMasterOptionsWindow(HINSTANCE hInstance);
HRESULT InitializeSlaveOptionsWindow(HINSTANCE hInstance);

INT_PTR CALLBACK SelectModuleDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MasterOptionsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SlaveOptionsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);




//==================================================================================
// Globals
//==================================================================================
PMODULEDATA		g_paModules = NULL;
DWORD			g_dwNumModules = 0;
HRESULT			g_hrExitCode = S_OK;







#undef DEBUG_SECTION
#define DEBUG_SECTION	"SelectLoadModuleAndInfo()"
//==================================================================================
// SelectLoadModuleAndInfo
//----------------------------------------------------------------------------------
//
// Description: Loads a module and its information.
//
// Arguments:
//	char* szTestNetRootPath		Path to TestNet root directory.
//	PMODULEDATA pModuleData		Data containing module name to load and place to
//								store module and information loaded.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT SelectLoadModuleAndInfo(char* szTestNetRootPath, PMODULEDATA pModuleData)
{
	HRESULT			hr;
	DWORD			dwSize;
	char*			pszTemp = NULL;
	TNSHELLINFO		shellinfo;
#ifdef _XBOX
	WCHAR			szWideTemp[512];
#endif


	if ((pModuleData->pszModuleName == NULL) ||
		(strlen(pModuleData->pszModuleName) == 0))
	{
		DPL(0, "Got passed an invalid module name!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (invalid module name)


	// Build the actual module DLL path so we can load it.

#ifndef _XBOX
	dwSize = strlen(szTestNetRootPath)				// TestNet path
			+ strlen(pModuleData->pszModuleName)	// module directory
			+ 1										// backslash
			+ strlen(pModuleData->pszModuleName)	// module root name
			+ strlen(".dll")						// module extension
			+ 1;									// NULL terminator
#else // ! XBOX
	dwSize = strlen(pModuleData->pszModuleName)		// module root name
			+ strlen(".dll")						// module extension
			+ 1;									// NULL terminator
#endif // XBOX

	pszTemp = (char*) LocalAlloc(LPTR, (dwSize * sizeof (char)));
	if (pszTemp == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)

#ifndef _XBOX
	strcpy(pszTemp, szTestNetRootPath);
	strcat(pszTemp, pModuleData->pszModuleName);
	strcat(pszTemp, "\\");
	strcat(pszTemp, pModuleData->pszModuleName);
	strcat(pszTemp, ".dll");
	pModuleData->hModule = LoadLibrary(pszTemp);
#else // ! XBOX
	strcpy(pszTemp, pModuleData->pszModuleName);
	strcat(pszTemp, ".dll");
	mbstowcs(szWideTemp, pszTemp, strlen(pszTemp));
	szWideTemp[strlen(pszTemp)] = 0;
	pModuleData->hModule = LoadLibrary(szWideTemp);
#endif // XBOX

	if (pModuleData->hModule == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't load test module library \"%s\"!", 1, pszTemp);
		goto ERROR_EXIT;
	} // end if (couldn't load test module DLL)

	LocalFree(pszTemp);
	pszTemp = NULL;



	// Retrieve the standard callbacks the module needs to implement.

	pModuleData->pfnGetModuleInfo = (PTNGETMODULEINFOPROC) GetProcAddress(pModuleData->hModule,
																		"TNM_GetModuleInfo");
	if (pModuleData->pfnGetModuleInfo == NULL)
	{
		DPL(0, "Couldn't find required procedure \"TNM_GetModuleInfo\" in DLL!", 0);
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (couldn't find required function in DLL)

	pModuleData->pfnInitModule = (PTNINITMODULEPROC) GetProcAddress(pModuleData->hModule,
																	"TNM_InitModule");
	if (pModuleData->pfnInitModule == NULL)
	{
		DPL(0, "Couldn't find required procedure \"TNM_InitModule\" in DLL!", 0);
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (couldn't find required function in DLL)

	pModuleData->pfnRelease = (PTNRELEASEPROC) GetProcAddress(pModuleData->hModule,
																"TNM_Release");
	if (pModuleData->pfnRelease == NULL)
	{
		DPL(0, "Couldn't find required procedure \"TNM_Release\" in DLL!", 0);
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (couldn't find required function in DLL)


	ZeroMemory(&shellinfo, sizeof (TNSHELLINFO));
	//BUGBUG the control API version is not necessarily what the DLL has,
	//		 it's actually what we (the shell) think it is.
	shellinfo.dwSize = sizeof (TNSHELLINFO);
	shellinfo.dwShellAPIVersion = CURRENT_TNSHELL_API_VERSION;
	shellinfo.dwControlAPIVersion = CURRENT_TNCONTROL_API_VERSION;


	ZeroMemory(&(pModuleData->info), sizeof (TNMODULEINFO));
	pModuleData->info.dwSize = sizeof (TNMODULEINFO);


	// Grab the size of the main module's info.

	hr = pModuleData->pfnGetModuleInfo(&shellinfo, &(pModuleData->info));
	if ((hr != S_OK) && (hr != ERROR_BUFFER_TOO_SMALL))
	{
		DPL(0, "Module's GetModuleInfo() function failed!", 0);
		goto ERROR_EXIT;
	} // end if (module's GetModuleInfo function failed)


	if (pModuleData->info.dwNameSize > 0)
	{
		pModuleData->info.pszName = (char*) LocalAlloc(LPTR, pModuleData->info.dwNameSize);
		if (pModuleData->info.pszName == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)
	} // end if (there's a module name)
	else
	{
		// Hmm, maybe it should be a requirement?
	} // end else (there's no module name)

	if (pModuleData->info.dwDescriptionSize > 0)
	{
		pModuleData->info.pszDescription = (char*) LocalAlloc(LPTR, pModuleData->info.dwDescriptionSize);
		if (pModuleData->info.pszDescription == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)
	} // end if (there's a module description)
	else
	{
		// Hmm, maybe it should be a requirement?
	} // end else (there's no module description)

	if (pModuleData->info.dwAuthorsSize > 0)
	{
		pModuleData->info.pszAuthors = (char*) LocalAlloc(LPTR, pModuleData->info.dwAuthorsSize);
		if (pModuleData->info.pszAuthors == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)
	} // end if (there's module authors)
	else
	{
		// Hmm, maybe it should be a requirement?
	} // end else (there's no module authors)


	// Actually grab the main module's info.

	hr = pModuleData->pfnGetModuleInfo(&shellinfo, &(pModuleData->info));
	if (hr != S_OK)
	{
		DPL(0, "Module's GetModuleInfo() function failed!", 0);
		goto ERROR_EXIT;
	} // end if (module's GetModuleInfo function failed)


	return (S_OK);


ERROR_EXIT:

	if (pszTemp != NULL)
	{
		LocalFree(pszTemp);
		pszTemp = NULL;
	} // end if (allocated string)

	if (pModuleData->hModule != NULL)
	{
		FreeLibrary(pModuleData->hModule);
		pModuleData->hModule = NULL;
	} // end if (have module)

	pModuleData->pfnGetModuleInfo = NULL;
	pModuleData->pfnInitModule = NULL;
	pModuleData->pfnRelease = NULL;

	pModuleData->info.dwNameSize = 0;
	if (pModuleData->info.pszName != NULL)
	{
		LocalFree(pModuleData->info.pszName);
		pModuleData->info.pszName = NULL;
	} // end if (allocated string)

	pModuleData->info.dwDescriptionSize = 0;
	if (pModuleData->info.pszDescription != NULL)
	{
		LocalFree(pModuleData->info.pszDescription);
		pModuleData->info.pszDescription = NULL;
	} // end if (allocated string)

	pModuleData->info.dwAuthorsSize = 0;
	if (pModuleData->info.pszAuthors != NULL)
	{
		LocalFree(pModuleData->info.pszAuthors);
		pModuleData->info.pszAuthors = NULL;
	} // end if (allocated string)

	return (hr);
} // SelectLoadModuleAndInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no window logging supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"SelectDoSelectModule()"
//==================================================================================
// SelectDoSelectModule
//----------------------------------------------------------------------------------
//
// Description: Displays the select module screen.
//
// Arguments:
//	HINSTANCE hInstance		Handle to application instance.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT SelectDoSelectModule(HINSTANCE hInstance)
{
	HRESULT		hr;
	MSG			msg;
	DWORD		dwTemp;



	// Ignore error, assume BUFFER_TOO_SMALL
	LoadModuleInfos(NULL, &g_dwNumModules);

	if (g_dwNumModules > 0)
	{
		g_paModules = (PMODULEDATA) LocalAlloc(LPTR, g_dwNumModules * sizeof (MODULEDATA));
		if (g_paModules == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (not enough memory)

		hr = LoadModuleInfos(g_paModules, &g_dwNumModules);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't load module infos!", 0);
			goto DONE;
		} // end if (couldn't load modules)
	} // end if (there were some modules)


	// Display the module selection window

	hr = InitializeModuleSelectWindow(hInstance);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't initialize module selection window!", 0);
		goto DONE;
	} // end if (couldn't initialize window)

	// Enter the module selection window message loop
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end while (there's a non-quit message)

	// Check for cancellation or errors
	if (g_hrExitCode != S_OK)
	{
		hr = g_hrExitCode;

		if (hr != SHELLERR_USERCANCEL)
		{
			DPL(0, "Window proc reports an error!", 0);
		} // end if (user didn't cancel)

		goto DONE;
	} // end if (window failed)

	if (g_pModuleData == NULL)
	{
		DPL(0, "Module data wasn't created!?", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (module wasn't created)


	DPL(0, "User selected module \"%s\".", 1, g_pModuleData->pszModuleName);



	// Display the appropriate options window.
	hr = InitializeSlaveOptionsWindow(hInstance);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't initialize slave options window!", 0);
		goto DONE;
	} // end if (couldn't initialize window)

	// Enter the options window message loop
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end while (there's a non-quit message)

	// Check for cancellation or errors
	if (g_hrExitCode != S_OK)
	{
		hr = g_hrExitCode;

		if (hr != SHELLERR_USERCANCEL)
		{
			DPL(0, "Window proc reports an error!", 0);
		} // end if (user didn't cancel)

		goto DONE;
	} // end if (window failed)



DONE:

	g_dwNumModules = 0;
	if (g_paModules != NULL)
	{
		for(dwTemp = 0; dwTemp < g_dwNumModules; dwTemp++)
		{
			if (g_paModules[dwTemp].pszModuleName != NULL)
			{
				LocalFree(g_paModules[dwTemp].pszModuleName);
				g_paModules[dwTemp].pszModuleName = NULL;
			} // end if (allocated string)

			if (g_paModules[dwTemp].hModule != NULL)
			{
				FreeLibrary(g_paModules[dwTemp].hModule);
				g_paModules[dwTemp].hModule = NULL;
			} // end if (have module)

			g_paModules[dwTemp].pfnGetModuleInfo = NULL;
			g_paModules[dwTemp].pfnInitModule = NULL;
			g_paModules[dwTemp].pfnRelease = NULL;

			g_paModules[dwTemp].info.dwNameSize = 0;
			if (g_paModules[dwTemp].info.pszName != NULL)
			{
				LocalFree(g_paModules[dwTemp].info.pszName);
				g_paModules[dwTemp].info.pszName = NULL;
			} // end if (allocated string)

			g_paModules[dwTemp].info.dwDescriptionSize = 0;
			if (g_paModules[dwTemp].info.pszDescription != NULL)
			{
				LocalFree(g_paModules[dwTemp].info.pszDescription);
				g_paModules[dwTemp].info.pszDescription = NULL;
			} // end if (allocated string)

			g_paModules[dwTemp].info.dwAuthorsSize = 0;
			if (g_paModules[dwTemp].info.pszAuthors != NULL)
			{
				LocalFree(g_paModules[dwTemp].info.pszAuthors);
				g_paModules[dwTemp].info.pszAuthors = NULL;
			} // end if (allocated string)
		} // end for (each module slot)

		LocalFree(g_paModules);
		g_paModules = NULL;
	} // end if (allocated memory)


	return (hr);
} // SelectDoSelectModule
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // !	XBOX



#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"LoadModuleInfos()"
//==================================================================================
// LoadModuleInfos
//----------------------------------------------------------------------------------
//
// Description: Loads the names of all the found valid modules into the array of
//				MODULEDATA structures.
//				If paModules is NULL, the number of MODULEDATA structures required
//				is placed in piNumModules.  If it is not NULL, the number of modules
//				that could actually be loaded is stored here.
//
// Arguments:
//	PMODULEDATA paModules	Pointer to array of module info structures to fill in,
//							or NULL to retrieve number of entries required.
//	DWORD* pdwNumModules	Place to store number of MODULEDATA structures required,
//							or actually found and loaded.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT LoadModuleInfos(PMODULEDATA paModules, DWORD* pdwNumModules)
{
	HRESULT					hr;
	DWORD					dwTemp = 0;
	DWORD					dwSearchPatternSize = 0;
	char*					pszSearchPattern = NULL;
	HANDLE					hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA			founddata;
	DWORD					dwSubItemPathSize = 0;
	char*					pszSubItemPath = NULL;
	HANDLE					hFile;



	dwSearchPatternSize = strlen(g_pszTestNetRootPath) + strlen("*.*") + 1;

	pszSearchPattern = (char*) LocalAlloc(LPTR, dwSearchPatternSize);
	if (pszSearchPattern == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)

	strcpy(pszSearchPattern, g_pszTestNetRootPath);
	strcat(pszSearchPattern, "*.*");

	hFind = FindFirstFile(pszSearchPattern, &founddata);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();
		DPL(0, "Couldn't start search!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't find first file)

	do
	{
		if ((strcmp(founddata.cFileName, ".") != 0) &&
			(strcmp(founddata.cFileName, "..") != 0) &&
			(founddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			dwSubItemPathSize = strlen(g_pszTestNetRootPath)
								+ strlen(founddata.cFileName)
								+ strlen("\\")
								+ strlen(founddata.cFileName)
								+ strlen(".dll")
								+ 1;

			pszSubItemPath = (char*) LocalAlloc(LPTR, dwSubItemPathSize);
			if (pszSubItemPath == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto ERROR_EXIT;
			} // end if (couldn't allocate memory)

			strcpy(pszSubItemPath, g_pszTestNetRootPath);
			strcat(pszSubItemPath, founddata.cFileName);
			strcat(pszSubItemPath, "\\");
			strcat(pszSubItemPath, founddata.cFileName);
			strcat(pszSubItemPath, ".dll");


			hFile = CreateFile(pszSubItemPath, 0, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				hr = GetLastError();

				// Assume the error is okay, it might just be a directory for the
				// user's personal use. Ignore it and move on.

				DPL(0, "Subdir \"%s\" found, but \"%s\" doesn't appear to exist.  %e",
					3, founddata.cFileName, pszSubItemPath, hr);
				hr = S_OK;
			} // end if (couldn't find DLL)
			else
			{
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;


				if (paModules != NULL)
				{
					paModules[dwTemp].pszModuleName = (char*) LocalAlloc(LPTR, strlen(founddata.cFileName) + 1);
					if (paModules[dwTemp].pszModuleName == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto ERROR_EXIT;
					} // end if (couldn't allocate memory)
					strcpy(paModules[dwTemp].pszModuleName, founddata.cFileName);

					hr = SelectLoadModuleAndInfo(g_pszTestNetRootPath, &(paModules[dwTemp]));
					if (hr != S_OK)
					{
						DPL(0, "Couldn't load module \"%s\"'s info!  %e",
							2, pszSubItemPath, hr);

						LocalFree(paModules[dwTemp].pszModuleName);
						paModules[dwTemp].pszModuleName = NULL;

						hr = S_OK;
					} // end if (couldn't load information)
					else
					{
						// We successfully loaded another module.
						dwTemp++;
					} // end else (could load information)
				} // end if (there's an array)
				else
				{
					// We found another module.
					dwTemp++;
				} // end else (there isn't an array)

			} // end else (found DLL)

			LocalFree(pszSubItemPath);
			pszSubItemPath = NULL;
		} // end if (the found item is a directory that's not special)

		if (! FindNextFile(hFind, &founddata))
		{
			hr = GetLastError();

			// If there aren't any more files, get out of the loop.
			if (hr == ERROR_NO_MORE_FILES)
			{
				hr = S_OK;
				break;
			} // end if (that was the last file)

			DPL(0, "Couldn't find next file!", 0);
			goto ERROR_EXIT;
		} // end if (finding next file failed)
	} // end do
	while (TRUE);


	if (! FindClose(hFind))
	{
		hr = GetLastError();
		DPL(0, "Couldn't close search!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't close search)

	hFind = INVALID_HANDLE_VALUE; // forget about it so we don't close it below)


	// If no buffer, this tell them how many DLLs we found.  If there is a buffer,
	// this shows how many we actually loaded.
	(*pdwNumModules) = dwTemp;


	return (S_OK);


ERROR_EXIT:

	if (paModules != NULL)
	{
		for(dwTemp = 0; dwTemp < (*pdwNumModules); dwTemp++)
		{
			if (paModules[dwTemp].pszModuleName != NULL)
			{
				LocalFree(paModules[dwTemp].pszModuleName);
				paModules[dwTemp].pszModuleName = NULL;
			} // end if (allocated string)

			if (paModules[dwTemp].hModule != NULL)
			{
				FreeLibrary(paModules[dwTemp].hModule);
				paModules[dwTemp].hModule = NULL;
			} // end if (have module)

			paModules[dwTemp].pfnGetModuleInfo = NULL;
			paModules[dwTemp].pfnInitModule = NULL;
			paModules[dwTemp].pfnRelease = NULL;

			paModules[dwTemp].info.dwNameSize = 0;
			if (paModules[dwTemp].info.pszName != NULL)
			{
				LocalFree(paModules[dwTemp].info.pszName);
				paModules[dwTemp].info.pszName = NULL;
			} // end if (allocated string)

			paModules[dwTemp].info.dwDescriptionSize = 0;
			if (paModules[dwTemp].info.pszDescription != NULL)
			{
				LocalFree(paModules[dwTemp].info.pszDescription);
				paModules[dwTemp].info.pszDescription = NULL;
			} // end if (allocated string)

			paModules[dwTemp].info.dwAuthorsSize = 0;
			if (paModules[dwTemp].info.pszAuthors != NULL)
			{
				LocalFree(paModules[dwTemp].info.pszAuthors);
				paModules[dwTemp].info.pszAuthors = NULL;
			} // end if (allocated string)
		} // end for (each module slot)
	} // end if (there's an array)

	if (pszSubItemPath != NULL)
	{
		LocalFree(pszSubItemPath);
		pszSubItemPath = NULL;
	} // end if (allocated string)

	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	} // end if (have find)

	return (hr);
} // LoadModuleInfos
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"InitializeModuleSelectWindow()"
//==================================================================================
// InitializeModuleSelectWindow
//----------------------------------------------------------------------------------
//
// Description: Initializes the module selection window.
//
// Arguments:
//	HINSTANCE hInstance		Application instance handle.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT InitializeModuleSelectWindow(HINSTANCE hInstance)
{
	HRESULT			hr;
	WNDCLASSEX		wndclsx;
	HWND			hWindow = NULL;



	// Register the window class
	wndclsx.cbSize = sizeof(wndclsx);
	GetClassInfoEx(NULL, WC_DIALOG, &wndclsx);
	wndclsx.lpfnWndProc = (WNDPROC)SelectModuleDlgProc;
	wndclsx.hInstance = hInstance;
	wndclsx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclsx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclsx.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wndclsx.lpszMenuName = NULL;
	wndclsx.lpszClassName = WINDOWCLASS_NAME_SELECTMODULE;
	wndclsx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (! RegisterClassEx(&wndclsx))
	{
		DPL(0, "Failed to register the window class!", 0);
		return (GetLastError());
	} // end if (failed to register the window class)


	// Load and create the window from a resource
	hWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_SELECTMODULE), NULL,
						   SelectModuleDlgProc);
	if (hWindow == NULL)
	{
		hr = GetLastError();

		if (hr == S_OK)
			hr = E_FAIL;

		DPL(0, "Couldn't create the module selection dialog window!", 0);
		return (hr);
	} // end if (couldn't make the window)

	ShowWindow(hWindow, SW_SHOWDEFAULT);

	return (S_OK);
} // InitializeModuleSelectWindow
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"InitializeMasterOptionsWindow()"
//==================================================================================
// InitializeMasterOptionsWindow
//----------------------------------------------------------------------------------
//
// Description: Initializes the master options window.
//
// Arguments:
//	HINSTANCE hInstance		Application instance handle.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT InitializeMasterOptionsWindow(HINSTANCE hInstance)
{
	HRESULT			hr;
	WNDCLASSEX		wndclsx;
	HWND			hWindow = NULL;



	// Register the window class
	wndclsx.cbSize = sizeof(wndclsx);
	GetClassInfoEx(NULL, WC_DIALOG, &wndclsx);
	wndclsx.lpfnWndProc = (WNDPROC)MasterOptionsDlgProc;
	wndclsx.hInstance = hInstance;
	wndclsx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclsx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclsx.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wndclsx.lpszMenuName = NULL;
	wndclsx.lpszClassName = WINDOWCLASS_NAME_MASTEROPTIONS;
	wndclsx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (! RegisterClassEx(&wndclsx))
	{
		DPL(0, "Failed to register the window class!", 0);
		return (GetLastError());
	} // end if (failed to register the window class)


	// Load and create the window from a resource
	hWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MASTEROPTIONS), NULL,
						   MasterOptionsDlgProc);
	if (hWindow == NULL)
	{
		hr = GetLastError();

		if (hr == S_OK)
			hr = E_FAIL;

		DPL(0, "Couldn't create the master options dialog window!", 0);
		return (hr);
	} // end if (couldn't make the window)

	ShowWindow(hWindow, SW_SHOWDEFAULT);

	return (S_OK);
} // InitializeMasterOptionsWindow
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"InitializeSlaveOptionsWindow()"
//==================================================================================
// InitializeSlaveOptionsWindow
//----------------------------------------------------------------------------------
//
// Description: Initializes the slave options window.
//
// Arguments:
//	HINSTANCE hInstance		Application instance handle.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT InitializeSlaveOptionsWindow(HINSTANCE hInstance)
{
	HRESULT			hr;
	WNDCLASSEX		wndclsx;
	HWND			hWindow = NULL;



	// Register the window class
	wndclsx.cbSize = sizeof(wndclsx);
	GetClassInfoEx(NULL, WC_DIALOG, &wndclsx);
	wndclsx.lpfnWndProc = (WNDPROC)SlaveOptionsDlgProc;
	wndclsx.hInstance = hInstance;
	wndclsx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclsx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclsx.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wndclsx.lpszMenuName = NULL;
	wndclsx.lpszClassName = WINDOWCLASS_NAME_SLAVEOPTIONS;
	wndclsx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (! RegisterClassEx(&wndclsx))
	{
		DPL(0, "Failed to register the window class!", 0);
		return (GetLastError());
	} // end if (failed to register the window class)


	// Load and create the window from a resource
	hWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_SLAVEOPTIONS), NULL,
						   SlaveOptionsDlgProc);
	if (hWindow == NULL)
	{
		hr = GetLastError();

		if (hr == S_OK)
			hr = E_FAIL;

		DPL(0, "Couldn't create the slave options dialog window!", 0);
		return (hr);
	} // end if (couldn't make the window)

	ShowWindow(hWindow, SW_SHOWDEFAULT);

	return (S_OK);
} // InitializeSlaveOptionsWindow
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no window logging supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"SelectModuleDlgProc()"
//==================================================================================
// SelectModuleDlgProc
//----------------------------------------------------------------------------------
//
// Description: SelectModule dialog window procedure.
//
// Arguments:
//	HWND hWnd		Window handle.
//	UINT uMsg		Message identifier.
//	WPARAM wParam	Depends on message.
//	LPARAM lParam	Depends on message.
//
// Returns: Depends on message.
//==================================================================================
INT_PTR CALLBACK SelectModuleDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD	dwTemp;
	int		i;
	int		iSize;
	char*	pszTemp;


	switch (uMsg)
	{
		case WM_INITDIALOG:
			// Set default value
			Button_SetCheck(GetDlgItem(hWnd, IDCHK_MASTER), FALSE);


			// Turn off OK by default
			EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);


			for(dwTemp = 0; dwTemp < g_dwNumModules; dwTemp++)
			{
				ListBox_AddString(GetDlgItem(hWnd, IDLB_MODULES),
									g_paModules[dwTemp].pszModuleName);

				/*
				// If the user had a last selection and this was it, we should
				// choose it again by default.
				if ((pszTemp != NULL) &&
					(StringCmpNoCase(pszString, pszTemp) == 0))
				{
					ListBox_SetCurSel(GetDlgItem(hWnd, IDL_MODULES), i);

					SetWindowText(GetDlgItem(hWnd, IDT_MODULENAME),
								g_modnames.GetIndexedString(i));
					SetWindowText(GetDlgItem(hWnd, IDT_MODULEDESCRIPTION),
								g_moddescs.GetIndexedString(i));

					// There's a selection, so we can enable the OK button
					EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
				} // end if (this was the user's last choice)
				*/
			} // end for (each module found)
		  break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					// Get current module selection.
					i = ListBox_GetCurSel(GetDlgItem(hWnd, IDLB_MODULES));
					if (i >= 0)
					{
						iSize = ListBox_GetTextLen(GetDlgItem(hWnd, IDLB_MODULES), i) + 1;
						pszTemp = (char*) LocalAlloc(LPTR, iSize);
						if (pszTemp != NULL)
						{
							ListBox_GetText(GetDlgItem(hWnd, IDLB_MODULES), i, pszTemp);

							for(dwTemp = 0; dwTemp < g_dwNumModules; dwTemp++)
							{
								if (strcmp(g_paModules[dwTemp].pszModuleName, pszTemp) == 0)
								{
									g_pModuleData = (PMODULEDATA) LocalAlloc(LPTR, sizeof (MODULEDATA));
									if (g_pModuleData != NULL)
									{
										// It's okay to copy pointers like this since its
										// all local.  And as long as we zero out the
										// memory, it won't get freed twice.
										CopyMemory(g_pModuleData, &(g_paModules[dwTemp]), sizeof (MODULEDATA));
										ZeroMemory( &(g_paModules[dwTemp]), sizeof (MODULEDATA));

									} // end if (could allocate memory)

									break; // stop looping
								} // end if (found module)
							} // end for (each module)

							LocalFree(pszTemp);
							pszTemp = NULL;
						} // end if (could allocate memory)
					} // end if (something is selected)

					PostMessage(hWnd, WM_CLOSE, S_OK, 0);
				  break;

				case IDCANCEL:
					DPL(0, "User pressed 'Cancel'.", 0);

					PostMessage(hWnd, WM_CLOSE, SHELLERR_USERCANCEL, 0);
				  break;

				case IDLB_MODULES:
					// Find out what this is!
					if (HIWORD(wParam) == 1)
					{
						// Get current module selection.
						i = ListBox_GetCurSel(GetDlgItem(hWnd, IDLB_MODULES));
						if (i >= 0)
						{
							iSize = ListBox_GetTextLen(GetDlgItem(hWnd, IDLB_MODULES), i) + 1;
							pszTemp = (char*) LocalAlloc(LPTR, iSize);
							if (pszTemp != NULL)
							{
								ListBox_GetText(GetDlgItem(hWnd, IDLB_MODULES), i, pszTemp);

								for(dwTemp = 0; dwTemp < g_dwNumModules; dwTemp++)
								{
									if (strcmp(g_paModules[dwTemp].pszModuleName, pszTemp) == 0)
									{
										char	szTemp[MAX_STRING];


										wsprintf(szTemp,
												"v%02u.%02u.%02u.%04u",
												g_paModules[dwTemp].info.moduleid.dwMajorVersion,
												g_paModules[dwTemp].info.moduleid.dwMinorVersion1,
												g_paModules[dwTemp].info.moduleid.dwMinorVersion2,
												g_paModules[dwTemp].info.moduleid.dwBuildVersion);

										SetWindowText(GetDlgItem(hWnd, IDT_MODULENAME),
													g_paModules[dwTemp].info.pszName);
										SetWindowText(GetDlgItem(hWnd, IDT_MODULEVERSION), szTemp);
										SetWindowText(GetDlgItem(hWnd, IDT_MODULEDESCRIPTION),
													g_paModules[dwTemp].info.pszDescription);
										SetWindowText(GetDlgItem(hWnd, IDT_MODULEAUTHORS),
													g_paModules[dwTemp].info.pszAuthors);

										EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);

										break; // stop looping
									} // end if (found module)
								} // end for (each module)

								LocalFree(pszTemp);
								pszTemp = NULL;
							} // end if (allocated string)
						} // end if (something is selected)
						else
						{
							SetWindowText(GetDlgItem(hWnd, IDT_MODULENAME), "");
							SetWindowText(GetDlgItem(hWnd, IDT_MODULEVERSION), "");
							SetWindowText(GetDlgItem(hWnd, IDT_MODULEDESCRIPTION), "");
							SetWindowText(GetDlgItem(hWnd, IDT_MODULEAUTHORS), "");

							EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);
						} // end else (did not select an item)
					} // end if (?)
				  break;
			} // end switch (on the button pressed/control changed)
		  break;

		case WM_CLOSE:
			g_hrExitCode = (HRESULT) wParam;

			PostQuitMessage((HRESULT) wParam);
		  break;
	} // end switch (on the type of window message)

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
} // SelectModuleDlgProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX


#ifndef _XBOX // no window logging supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"MasterOptionsDlgProc()"
//==================================================================================
// MasterOptionsDlgProc
//----------------------------------------------------------------------------------
//
// Description: MasterOptions dialog window procedure.
//
// Arguments:
//	HWND hWnd		Window handle.
//	UINT uMsg		Message identifier.
//	WPARAM wParam	Depends on message.
//	LPARAM lParam	Depends on message.
//
// Returns: Depends on message.
//==================================================================================
INT_PTR CALLBACK MasterOptionsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			// Set defaults

			Button_SetCheck(GetDlgItem(hWnd, IDR_MODE_API),
							((g_dwMode == TNMODE_API) ? TRUE : FALSE));

			Button_SetCheck(GetDlgItem(hWnd, IDR_MODE_STRESS),
							((g_dwMode == TNMODE_STRESS) ? TRUE : FALSE));

			Button_SetCheck(GetDlgItem(hWnd, IDR_MODE_POKE),
							((g_dwMode == TNMODE_POKE) ? TRUE : FALSE));


			if (g_pszMetaMastersAddress == NULL)
			{
				Button_SetCheck(GetDlgItem(hWnd, IDCHK_USEMETAMASTER), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDE_USEMETAMASTER), FALSE);
				SetWindowText(GetDlgItem(hWnd, IDE_USEMETAMASTER), "");
			} // end if (there's a meta master to use)
			else
			{
				Button_SetCheck(GetDlgItem(hWnd, IDCHK_USEMETAMASTER), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDE_USEMETAMASTER), TRUE);
				SetWindowText(GetDlgItem(hWnd, IDE_USEMETAMASTER),
							g_pszMetaMastersAddress);
			} // end else (there isn't a meta master to use)
		  break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					PostMessage(hWnd, WM_CLOSE, S_OK, 0);
				  break;

				case IDCANCEL:
					DPL(0, "User pressed 'Cancel'.", 0);

					PostMessage(hWnd, WM_CLOSE, SHELLERR_USERCANCEL, 0);
				  break;
			} // end switch (on the button pressed/control changed)
		  break;

		case WM_CLOSE:
			g_hrExitCode = (HRESULT) wParam;

			PostQuitMessage((HRESULT) wParam);
		  break;
	} // end switch (on the type of window message)

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
} // MasterOptionsDlgProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX


#ifndef _XBOX // no window logging supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"SlaveOptionsDlgProc()"
//==================================================================================
// SlaveOptionsDlgProc
//----------------------------------------------------------------------------------
//
// Description: SlaveOptions dialog window procedure.
//
// Arguments:
//	HWND hWnd		Window handle.
//	UINT uMsg		Message identifier.
//	WPARAM wParam	Depends on message.
//	LPARAM lParam	Depends on message.
//
// Returns: Depends on message.
//==================================================================================
INT_PTR CALLBACK SlaveOptionsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		  break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					PostMessage(hWnd, WM_CLOSE, S_OK, 0);
				  break;

				case IDCANCEL:
					DPL(0, "User pressed 'Cancel'.", 0);

					PostMessage(hWnd, WM_CLOSE, SHELLERR_USERCANCEL, 0);
				  break;
			} // end switch (on the button pressed/control changed)
		  break;

		case WM_CLOSE:
			g_hrExitCode = (HRESULT) wParam;

			PostQuitMessage((HRESULT) wParam);
		  break;
	} // end switch (on the type of window message)

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
} // SlaveOptionsDlgProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX