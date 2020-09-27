//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <windowsx.h>
#include <direct.h>
#ifndef CMDLINE_WINDOWS_ONLY
#include <stdio.h>
#endif // ! CMDLINE_WINDOWS_ONLY

#ifndef CMDLINE_CONSOLE_ONLY
#include <stdlib.h>	// NT BUILD needs this for _pgmptr
#endif // ! CMDLINE_CONSOLE_ONLY

#include "resource.h"

#include "main.h"
#include "cppobjhelp.h"
#include "linklist.h"
#include "linkedstr.h"
#include "fileutils.h"
#include "strutils.h"
#ifndef NO_TNCOMMON_DEBUG_SPEW
#include "debugprint.h"
#endif // NO_TNCOMMON_DEBUG_SPEW
#ifdef _XBOX
#include "debugprint.h"
#endif

#include "cmdline.h"




//==================================================================================
// Defines
//==================================================================================
#define WRAP_DESCRIPTION_CHARACTERS		75




//==================================================================================
// Structures
//==================================================================================
typedef struct tagCMDLINEREADFILECONTEXT
{
	BOOL					fInCommandLineSection; // are we currently in the command line section?
	PSPECIFIEDPARAMSLIST	pItemList; // pointer to list to add items to
} CMDLINEREADFILECONTEXT, * PCMDLINEREADFILECONTEXT;




//==================================================================================
// Prototypes
//==================================================================================
HRESULT CmdlineReadFileCB(char* szLine, PFILELINEINFO pInfo, PVOID pvContext,
						BOOL* pfStopReading);





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::CCommandLine()"
//==================================================================================
// CCommandLine constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CCommandLine object.
//
// Arguments: None.
//
// Returns: None (the object).
//==================================================================================
CCommandLine::CCommandLine(void)
{
	this->m_pszPathToApp = NULL;
	this->m_pszOriginalCommandLine = NULL;
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	this->m_fConsole = FALSE;
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
	this->m_dwFlags = 0;

	this->m_fHTML = FALSE;
	this->m_fHelp = FALSE;
	this->m_fDebug = FALSE;
	this->m_pszIniFile = NULL;

	this->m_papHelpItems = NULL;
	this->m_dwNumHelpItems = 0;
	this->m_dwHelpStartItem = 0;
	this->m_lMaxHelpLineWidth = 0;
	this->m_lMaxHelpHeight = 0;
	this->m_lLargestLeftHelpTextWidth = 0;
	this->m_lLargestRightHelpTextWidth = 0;
	this->m_lLargestHelpLineHeight = 0;

#ifndef CMDLINE_CONSOLE_ONLY
	this->m_pszTempParamNameBuffer = NULL;
#endif // not CMDLINE_CONSOLE_ONLY
} // CCommandLine::CCommandLine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::~CCommandLine()"
//==================================================================================
// CCommandLine destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CCommandLine object.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CCommandLine::~CCommandLine(void)
{
	if (this->m_pszOriginalCommandLine != NULL)
	{
		LocalFree(this->m_pszOriginalCommandLine);
		this->m_pszOriginalCommandLine = NULL;
	} // end if (allocated command line)

	if (this->m_pszPathToApp != NULL)
	{
		LocalFree(this->m_pszPathToApp);
		this->m_pszPathToApp = NULL;
	} // end if (we allocated a path to the app)

	if (this->m_papHelpItems != NULL)
	{
		LocalFree(this->m_papHelpItems);
		this->m_papHelpItems = NULL;
	} // end if (we allocated a help item array)

#ifndef CMDLINE_CONSOLE_ONLY
	if (this->m_pszTempParamNameBuffer != NULL)
	{
		LocalFree(this->m_pszTempParamNameBuffer);
		this->m_pszTempParamNameBuffer = NULL;
	} // end if (we allocated a param name buffer)
#endif // not CMDLINE_CONSOLE_ONLY
} // CCommandLine::~CCommandLine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::Initialize()"
//==================================================================================
// CCommandLine::Initialize
//----------------------------------------------------------------------------------
//
// Description: Initializes the CCommandLine object with the specified flags, and
//				prepares to parse the command line searching for the given items.
//
// Arguments:
//	DWORD dwFlags						The flags for the behavior you want.
//	PCMDLINEHANDLEDITEM aHandledItems	Pointer to array of handled items that you
//										expect to parse.
//	DWORD dwNumHandledItems				Number of items in the preceding array.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::Initialize(DWORD dwFlags, PCMDLINEHANDLEDITEM aHandledItems,
								DWORD dwNumHandledItems)
{
	HRESULT				hr;
	CMDLINEHANDLEDITEM	stditem;
	DWORD				dwTemp;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)
#endif // DEBUG

	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(1, "Called with (%x, %x, %u)", 3, dwFlags, lpaHandledItems, dwNumHandledItems);
	#endif // NO_TNCOMMON_DEBUG_SPEW
	*/

	// Set the flags
	this->m_dwFlags = dwFlags;


	// Add built-in "ini" switch with parameter if it's allowed.

	if (! (this->m_dwFlags & CMDLINE_FLAG_DONTHANDLEINIFILES))
	{
		ZeroMemory(&stditem, sizeof (CMDLINEHANDLEDITEM));
		stditem.dwType = CMDLINE_PARAMTYPE_STRING;
		stditem.pszName = "ini";
		stditem.pszDescription = "Reads command line items from the specified file.";
		//stditem.dwOptions = 0;
		stditem.ppvValuePtr = (PVOID*) (&(this->m_pszIniFile));
		//stditem.lpvAdditionalData = NULL;
		//stditem.dwAdditionalDataSize = 0;

		hr = this->AddItemInternal(&stditem, &(this->m_handleditems));
		if (hr != CMDLINE_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Failed adding built-in item \"ini\"!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			return (hr);
		} // end if (adding the item failed)
	} // end if (ini files are allowed)


	// Add built-in "?" and "help" switches if they're allowed

	if (! (this->m_dwFlags & CMDLINE_FLAG_DONTHANDLEHELPSWITCHES))
	{
		ZeroMemory(&stditem, sizeof (CMDLINEHANDLEDITEM));
		stditem.dwType = CMDLINE_PARAMTYPE_BOOL;
		stditem.pszName = "?";
		stditem.pszDescription = "Displays help.";
		//stditem.dwOptions = 0;
		stditem.ppvValuePtr = (PVOID*) (&(this->m_fHelp));
		//stditem.lpvAdditionalData = NULL;
		//stditem.dwAdditionalDataSize = 0;

		hr = this->AddItemInternal(&stditem, &(this->m_handleditems));
		if (hr != CMDLINE_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Failed adding built-in item \"?\"!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			return (hr);
		} // end if (adding the item failed)


		// Add built-in "help" switch

		stditem.pszName = "help";

		hr = this->AddItemInternal(&stditem, &(this->m_handleditems));
		if (hr != CMDLINE_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Failed adding built-in item \"help\"!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			return (hr);
		} // end if (adding the item failed)
	} // end if (help switches are allowed)



	// Add built-in "debug" switch if it's allowed

	if (! (this->m_dwFlags & CMDLINE_FLAG_DONTHANDLEDEBUGSWITCH))
	{
		stditem.pszName = "debug";
		stditem.pszDescription = "Turns on debug mode.";
		stditem.ppvValuePtr = (PVOID*) (&(this->m_fDebug));

		hr = this->AddItemInternal(&stditem, &(this->m_handleditems));
		if (hr != CMDLINE_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Failed adding built-in item \"debug\"!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			return (hr);
		} // end if (adding the item failed)
	} // end if (debug switch is allowed)



	// Add built-in "html" switch if it's allowed

	if (! (this->m_dwFlags & CMDLINE_FLAG_DONTHANDLEHTMLSWITCH))
	{
		stditem.pszName = "html";
		stditem.pszDescription = "Turns on html mode.";
		stditem.ppvValuePtr = (PVOID*) (&(this->m_fHTML));

		hr = this->AddItemInternal(&stditem, &(this->m_handleditems));
		if (hr != CMDLINE_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Failed adding built-in item \"html\"!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			return (hr);
		} // end if (adding the item failed)
	} // end if (HTML switch is allowed)



	for(dwTemp = 0; dwTemp < dwNumHandledItems; dwTemp++)
	{
		hr = this->AddItemInternal(&(aHandledItems[dwTemp]), &(this->m_handleditems));
		if (hr != CMDLINE_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Failed adding item %u!", 1, dwTemp);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			return (hr);
		} // end if (adding the item failed)
	} // end for (each handled command line item)

	return (CMDLINE_OK);
} // CCommandLine::Initialize
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#ifndef CMDLINE_CONSOLE_ONLY

#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::Parse(windows)"
//==================================================================================
// CCommandLine::Parse overloaded
// Windows version.
//----------------------------------------------------------------------------------
//
// Description: Parses the command line arguments passed in.
//				This is the Windows version, so call this with the parameters
//				passed into your application's WinMain() function.
//
// Arguments:
//	LPCSTR lpcszString	The command line string.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::Parse(LPCSTR lpcszString)
{
	HRESULT		hr;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (we've got a bad object)
#endif // DEBUG

	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(1, "Called with (%s)", 1, lpcszString);
	#endif // NO_TNCOMMON_DEBUG_SPEW
	*/

	// Grab the path to the app

	if (this->m_pszPathToApp != NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "WARNING: Overwriting previous path to app (was %s)!",
			1, this->m_pszPathToApp);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		LocalFree(this->m_pszPathToApp);
		this->m_pszPathToApp = NULL;
	} // end if (we already allocated a name)

	//BUGBUG _pgmptr may not contain the correct info.
	this->m_pszPathToApp = (char*) LocalAlloc(LPTR, (strlen(_pgmptr) + 1));
	if (this->m_pszPathToApp == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)

	strcpy(this->m_pszPathToApp, _pgmptr);



	// Copy the original command line

	if (this->m_pszOriginalCommandLine != NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "WARNING: Overwriting original command line (was %s)!?",
			1, this->m_pszOriginalCommandLine);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		LocalFree(this->m_pszOriginalCommandLine);
		this->m_pszOriginalCommandLine = NULL;
	} // end if (we already allocated a name)

	this->m_pszOriginalCommandLine = (char*) LocalAlloc(LPTR, strlen(lpcszString) + 1);
	if (this->m_pszOriginalCommandLine == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)

	strcpy(this->m_pszOriginalCommandLine, lpcszString);


	// Take out any extra spaces
	StringPopLeadingChars(this->m_pszOriginalCommandLine, " ", TRUE);
	StringPopTrailingChars(this->m_pszOriginalCommandLine, " ", TRUE);


	// If there aren't any items, and the user wants to fail in that case, then
	// do it.
	if ((strcmp(this->m_pszOriginalCommandLine, "") == 0) &&
		(this->m_dwFlags & CMDLINE_FLAG_FAILIFNOPARAMS))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "No command line parameters passed, but user required them!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = CMDLINEERR_NOPARAMSPASSED;
		goto ERROR_EXIT;
	} // end if (no command line items and should fail)


	// Actually parse it

	hr = this->ParseInternal(this->m_pszOriginalCommandLine, &(this->m_handleditems),
							&(this->m_specifieditems));
	if (hr != S_OK)
		goto ERROR_EXIT;

	return (S_OK);


ERROR_EXIT:

	if (this->m_pszOriginalCommandLine != NULL)
	{
		LocalFree(this->m_pszOriginalCommandLine);
		this->m_pszOriginalCommandLine = NULL;
	} // end if (allocated command line)

	if (this->m_pszPathToApp != NULL)
	{
		LocalFree(this->m_pszPathToApp);
		this->m_pszPathToApp = NULL;
	} // end if (allocated command line)

	return (hr);
} // CCommandLine::Parse(windows)
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#endif // ! CMDLINE_CONSOLE_ONLY



#ifndef CMDLINE_WINDOWS_ONLY

#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::Parse(console)"
//==================================================================================
// CCommandLine::Parse overloaded
// Console version.
//----------------------------------------------------------------------------------
//
// Description: Parses the command line arguments passed in.
//				This is the console version, so call this with the parameters
//				passed into your application's main() function.
//
// Arguments:
//	int iNumArgs		The number of arguments in the string array.
//	char* aszArgsArray	The array of command line arguments.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::Parse(int iNumArgs, char* aszArgsArray[])
{
	HRESULT		hr;
	int			i;
	DWORD		dwLength = 0;
	
	
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (we've got a bad object)
#endif // DEBUG

	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "Called with (%i,  %x)", 2, iNumArgs, aszArgsArray);
	#endif // NO_TNCOMMON_DEBUG_SPEW
	*/

#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	// Make note of the fact we're in a console app
	this->m_fConsole = TRUE;
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)


	// Grab the application path.

	if (this->m_pszPathToApp != NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "WARNING: Overwriting previous path to app (was %s)!?",
			1, this->m_pszPathToApp);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		LocalFree(this->m_pszPathToApp);
		this->m_pszPathToApp = NULL;
	} // end if (we already allocated a name)

	this->m_pszPathToApp = (char*) LocalAlloc(LPTR, (strlen(aszArgsArray[0]) + 1));
	if (this->m_pszPathToApp == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)

	strcpy(this->m_pszPathToApp, aszArgsArray[0]);


	// Now we want to flatten out the command line into all one string. Start at 1
	// because first parameter passed is the path of the app.
	for(i = 1; i < iNumArgs; i++)
	{
		dwLength += strlen(aszArgsArray[i]);

		// If this isn't the last item, add room for a space character
		if (i < iNumArgs)
			dwLength++;
	} // end for (loop through all the passed arguments)



	// If there aren't any items, and the user wants to fail in that case, then
	// do it.
	if ((dwLength == 0) && (this->m_dwFlags & CMDLINE_FLAG_FAILIFNOPARAMS))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "No command line parameters passed, but user required them!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = CMDLINEERR_NOPARAMSPASSED;
		goto ERROR_EXIT;
	} // end if (no command line items and should fail)


	if (this->m_pszOriginalCommandLine != NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "WARNING: Overwriting original command line (was %s)!?",
			1, this->m_pszOriginalCommandLine);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		LocalFree(this->m_pszOriginalCommandLine);
		this->m_pszOriginalCommandLine = NULL;
	} // end if (we already allocated a name)

	this->m_pszOriginalCommandLine = (char*) LocalAlloc(LPTR, dwLength + 1); // + 1 for NULL termination
	if (this->m_pszOriginalCommandLine == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)


	// Loop through again and put in all the elements
	for(i = 1; i < iNumArgs; i++)
	{
		strcat(this->m_pszOriginalCommandLine, aszArgsArray[i]);

		// If this isn't the last item, add a space character
		if (i < iNumArgs)
			strcat(this->m_pszOriginalCommandLine, " ");
	} // end for (loop through all the passed arguments)


	// Actually parse it

	hr = this->ParseInternal(this->m_pszOriginalCommandLine, &(this->m_handleditems),
							&(this->m_specifieditems));
	if (hr != S_OK)
		goto ERROR_EXIT;

	return (S_OK);


ERROR_EXIT:

	if (this->m_pszOriginalCommandLine != NULL)
	{
		LocalFree(this->m_pszOriginalCommandLine);
		this->m_pszOriginalCommandLine = NULL;
	} // end if (allocated command line)

	if (this->m_pszPathToApp != NULL)
	{
		LocalFree(this->m_pszPathToApp);
		this->m_pszPathToApp = NULL;
	} // end if (allocated command line)

	return (hr);
} // CCommandLine::Parse(console)
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#endif // ! CMDLINE_WINDOWS_ONLY




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::InHelpMode()"
//==================================================================================
// CCommandLine::InHelpMode
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if a help switch was specified (which can't happen if
//				the help switch is not allowed) or a required parameter wasn't
//				found and CMDLINE_FLAG_MISSINGPARAMMEANSHELP was specifed.
//
// Arguments: None.
//
// Returns: TRUE if help switch was specified or param was missing, FALSE otherwise.
//==================================================================================
BOOL CCommandLine::InHelpMode(void)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG


	return (this->m_fHelp);
} // CCommandLine::InHelpMode
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::InDebugMode()"
//==================================================================================
// CCommandLine::InDebugMode
//----------------------------------------------------------------------------------
//
// Description: Returns whether the debug switch was specified (this will always
//				return FALSE if the debug switch is not allowed).
//
// Arguments: None.
//
// Returns: TRUE if debug switch was allowed and specified, FALSE otherwise.
//==================================================================================
BOOL CCommandLine::InDebugMode(void)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG


	return (this->m_fDebug);
} // CCommandLine::InDebugMode
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::InHTMLMode()"
//==================================================================================
// CCommandLine::InHTMLMode
//----------------------------------------------------------------------------------
//
// Description: Returns whether the HTML switch was specified (this will always
//				return FALSE if the HTML switch is not allowed).
//
// Arguments: None.
//
// Returns: TRUE if HTML switch was allowed and specified, FALSE otherwise.
//==================================================================================
BOOL CCommandLine::InHTMLMode(void)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG


	return (this->m_fHTML);
} // CCommandLine::InHTMLMode
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::DisplayHelp()"
//==================================================================================
// CCommandLine::DisplayHelp
//----------------------------------------------------------------------------------
//
// Description: Displays a window with (or prints to the console if in a console
//				app) all the commmand line parameters along with their descriptions
//				given to Initialize.
//				IMPORTANT: Parse must have been called prior to this function!
//
// Arguments:
//	BOOL fAlphabetize	Whether to alphabetize the items when displaying them.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::DisplayHelp(BOOL fAlphabetize)
{
	HRESULT				hr = CMDLINE_OK;
	PHANDLEDPARAM		pParam = NULL;
	HWND				hHelpWnd = NULL;
#ifndef CMDLINE_CONSOLE_ONLY
	WNDCLASSEX			wcex;
	HDC					hdc = NULL;
	MSG					msg;
	SIZE				size;
	DWORD				dwLongestNameLength = 0;
	DWORD				dwNameLength;
	DWORD				dwTemp;
#endif // ! CMDLINE_CONSOLE_ONLY
	LONG				lLeftWidth;
	LONG				lRightWidth;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)
#endif // DEBUG

	//BUGBUG make sure we're not already in help mode

	// Allocate room for the maximum number of items
	this->m_papHelpItems = (PHANDLEDPARAM*) LocalAlloc(LPTR, (this->m_handleditems.Count() * sizeof (PHANDLEDPARAM)));
	if (this->m_papHelpItems == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	// This boolean is only set by calling the Console version of Parse.
	if (this->m_fConsole)
	{
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
#ifndef CMDLINE_WINDOWS_ONLY
		this->m_lMaxHelpLineWidth = 77;

		printf("Command line parameters:\n");
#endif // ! CMDLINE_WINDOWS_ONLY
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	} // end if (console app)
	else
	{
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
#ifndef CMDLINE_CONSOLE_ONLY
		this->m_lMaxHelpLineWidth = 595;
		this->m_lMaxHelpHeight = 350;

		ZeroMemory(&wcex, sizeof (WNDCLASSEX));
		wcex.cbSize = sizeof (WNDCLASSEX);
		GetClassInfoEx(NULL, WC_DIALOG, &wcex);
		wcex.lpfnWndProc = (WNDPROC)HelpBoxWndProc;
#ifdef LOCAL_TNCOMMON
		wcex.hInstance = NULL;
#else
		wcex.hInstance = s_hInstance;
#endif // ! LOCAL_TNCOMMON
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = WINDOWCLASS_CMDLINE_HELPBOX;
		wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (! RegisterClassEx(&wcex))
		{
			hr = GetLastError();
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't register window class!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (couldn't register class)

#ifdef LOCAL_TNCOMMON
		hHelpWnd = CreateDialog(NULL, MAKEINTRESOURCE(IDD_HELP), NULL,
								HelpBoxWndProc);
#else
		hHelpWnd = CreateDialog(s_hInstance, MAKEINTRESOURCE(IDD_HELP), NULL,
								HelpBoxWndProc);
#endif // ! LOCAL_TNCOMMON
		if (hHelpWnd == NULL)
		{
			hr = GetLastError();

			if (hr == S_OK)
				hr = E_FAIL;

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't create help dialog!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (couldn't create dialog)

		// Store the object pointer with the window
		SetWindowLongPtr(hHelpWnd, 0, (LONG_PTR) this);


		hdc = GetDC(hHelpWnd);
		if (hdc == NULL)
		{
			hr = GetLastError();
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get window's device context!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (couldn't get device context)
#endif // ! CMDLINE_CONSOLE_ONLY
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	} // end else (Windows app)
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)


	do
	{
		pParam = (PHANDLEDPARAM) this->m_handleditems.GetNextItem(pParam);
		if (pParam == NULL)
			break;


		// If it's a hidden option, don't display it
		if (pParam->m_dwOptions & CMDLINE_PARAMOPTION_HIDDEN)
			continue;

#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
		if (this->m_fConsole)
		{
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
#ifndef CMDLINE_WINDOWS_ONLY
			lLeftWidth = strlen(pParam->GetString());
			lRightWidth = 0;
			if (pParam->m_pszDescription != NULL)
			{
				lRightWidth = strlen(pParam->m_pszDescription);
			} // end if (there's a description)
#endif // ! CMDLINE_WINDOWS_ONLY
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
		} // end if (console app)
		else
		{
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
#ifndef CMDLINE_CONSOLE_ONLY

			// Make sure the temporary buffer is big enough to hold this parameter
			// name plus at least three periods.
			dwNameLength = strlen(pParam->GetString());
			if (dwLongestNameLength < (dwNameLength + 4))
			{
				if (this->m_pszTempParamNameBuffer != NULL)
				{
					LocalFree(this->m_pszTempParamNameBuffer);
					this->m_pszTempParamNameBuffer = NULL;
				} // end if (already had buffer)

				dwLongestNameLength = dwNameLength + 4;

				this->m_pszTempParamNameBuffer = (char*) LocalAlloc(LPTR, dwLongestNameLength);
				if (this->m_pszTempParamNameBuffer == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)
			} // end if (we allocated a param name buffer)


			// Copy the param name (without the NULL termination).
			memcpy(this->m_pszTempParamNameBuffer, pParam->GetString(), dwNameLength);

			// Make sure remaining buffer is filled with periods.
			for(dwTemp = dwNameLength; dwTemp < (dwLongestNameLength - 1); dwTemp++)
			{
				this->m_pszTempParamNameBuffer[dwTemp] = '.';
			} // end for (each remaining character)

			// Make sure buffer is NULL terminated.
			//this->m_pszTempParamNameBuffer[dwTemp] = '\0';


			// Prepare to calculate using a fixed font.
			SelectFont(hdc, GetStockObject(ANSI_FIXED_FONT));

			if (! GetTextExtentPoint32(hdc, this->m_pszTempParamNameBuffer, dwLongestNameLength, &size))
			{
				hr = GetLastError();
				if (hr == S_OK)
					hr = E_FAIL;
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't get text extents for \"%s\"!",
					1, this->m_pszTempParamNameBuffer);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW

				goto DONE;
			} // end if (couldn't get text size)

			lLeftWidth = size.cx;

			if (size.cy > this->m_lLargestHelpLineHeight)
				this->m_lLargestHelpLineHeight = size.cy;

			lRightWidth = 0;
			if (pParam->m_pszDescription != NULL)
			{
				// Prepare to calculate using a more pleasing font.
				SelectFont(hdc, GetStockObject(ANSI_VAR_FONT));

				if (! GetTextExtentPoint32(hdc, pParam->m_pszDescription, strlen(pParam->m_pszDescription), &size))
				{
					hr = GetLastError();
					if (hr == S_OK)
						hr = E_FAIL;
					#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(0, "Couldn't get text extents for \"%s\"!", 1, pParam->m_pszDescription);
					#endif // ! NO_TNCOMMON_DEBUG_SPEW

					goto DONE;
				} // end if (couldn't get text size)

				lRightWidth = size.cx;

				if (size.cy > this->m_lLargestHelpLineHeight)
					this->m_lLargestHelpLineHeight = size.cy;
			} // end if (there's a description)
#endif // ! CMDLINE_CONSOLE_ONLY
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
		} // end else (not a console app)
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)

		if (lLeftWidth > this->m_lLargestLeftHelpTextWidth)
		{
			// Clip the description section line length if this puts us over
			// the limit
			this->m_lLargestLeftHelpTextWidth = lLeftWidth;
			if (lLeftWidth + this->m_lLargestRightHelpTextWidth > this->m_lMaxHelpLineWidth)
				this->m_lLargestRightHelpTextWidth = this->m_lMaxHelpLineWidth - lLeftWidth;
		} // end if (this is longer than the previous longest name)

		if (lRightWidth > this->m_lLargestRightHelpTextWidth)
		{
			// Wrap to the next line if greater than the max allowable
			if (lRightWidth + this->m_lLargestLeftHelpTextWidth > this->m_lMaxHelpLineWidth)
				this->m_lLargestRightHelpTextWidth = this->m_lMaxHelpLineWidth - this->m_lLargestLeftHelpTextWidth;
			else
				this->m_lLargestRightHelpTextWidth = lRightWidth;
		} // end if (this is longer than the previous longest description)

		if (! fAlphabetize)
			this->m_papHelpItems[this->m_dwNumHelpItems++] = pParam;
		else
		{
			DWORD	dwTemp;
			DWORD	dwTemp2;


			// Loop through all the params we've put in the array so far.  If we
			// should be higher alphabetically than an item, insert it there and
			// push all the remaining ones down.  Otherwise, we'll just put it
			// at the last entry.
			for(dwTemp = 0; dwTemp < this->m_dwNumHelpItems; dwTemp++)
			{
				if (strcmp(pParam->GetString(), this->m_papHelpItems[dwTemp]->GetString()) < 0)
				{
					// Bump all current items in the table down one
					for(dwTemp2 = this->m_dwNumHelpItems; dwTemp2 > dwTemp; dwTemp2--)
					{
						this->m_papHelpItems[dwTemp2] = this->m_papHelpItems[dwTemp2 - 1];
					} // end for (each item after this in the list)

					// We found our spot, so stop
					break;
				} // end if (we should place the new item before the current item)
			} // end for (each existing item)

			// Store this item in the current spot and increment the count
			this->m_papHelpItems[dwTemp] = pParam;
			this->m_dwNumHelpItems++;
		} // end else (we should alphabetize the items)
	} // end do (while not done with list of parameters)
	while (pParam != NULL);


	this->m_dwHelpStartItem = 0;


	// Display the first page of help items.  hHelpWnd is ignored if in
	// console mode (it's NULL then anyway).
	hr = this->DisplayHelpPageInternal(&(this->m_dwHelpStartItem), hHelpWnd);
	if (hr != S_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't display first help page!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (displaying first help page failed)


	// If we're displaying a Window, wait for it to be closed.
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	if (! this->m_fConsole)
	{
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
#ifndef CMDLINE_CONSOLE_ONLY
		ShowWindow(hHelpWnd, SW_SHOW); // ignoring error

		while(GetMessage(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} // end while (there's a non-quit message)
#endif // ! CMDLINE_CONSOLE_ONLY
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	} // end if (Windows app)
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)


DONE:

#ifndef CMDLINE_CONSOLE_ONLY
	if (hdc != NULL)
	{
		ReleaseDC(hHelpWnd, hdc); // ignore error;
		hdc = NULL;
	} // end if (got device context)

	if (this->m_pszTempParamNameBuffer != NULL)
	{
		LocalFree(this->m_pszTempParamNameBuffer);
		this->m_pszTempParamNameBuffer = NULL;
	} // end if (we allocated a param name buffer)
#endif // ! CMDLINE_CONSOLE_ONLY

	if (this->m_papHelpItems != NULL)
	{
		LocalFree(this->m_papHelpItems);
		this->m_papHelpItems = NULL;
	} // end if (allocated an arrary for param items)

	this->m_dwNumHelpItems = 0;

	return (hr);
} // CCommandLine::DisplayHelp
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#else // ! XBOX
//#pragma TODO(tristanj, "No help displayed")
#endif // XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::CheckForItem()"
//==================================================================================
// CCommandLine::CheckForItem
//----------------------------------------------------------------------------------
//
// Description: Adds an item to parse on the command line.  This must be called
//				after Parse.
//
// Arguments:
//	PCMDLINEHANDLEDITEM pHandledItem	Pointer to items that you want to parse.
//	BOOL* pfWasSpecified				Optional pointer to boolean to set to TRUE
//										if it was actually found on the command
//										line, FALSE otherwise.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::CheckForItem(PCMDLINEHANDLEDITEM pHandledItem,
									BOOL* pfWasSpecified)
{
	HRESULT			hr;
	PHANDLEDPARAM	pParam;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we've got a bad object)
#endif // DEBUG

	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(1, "Called with (%x, %x)", 2, pHandledItem, pfWasSpecified);
	#endif // NO_TNCOMMON_DEBUG_SPEW
	*/

	hr = this->AddItemInternal(pHandledItem, &(this->m_handleditems));
	if (hr != CMDLINE_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Failed adding item!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end if (adding the item failed)


	// Get the last item
	pParam = (PHANDLEDPARAM) this->m_handleditems.GetPrevItem(NULL);
	if (pParam == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't get item just added!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		hr = E_FAIL;
		goto DONE;
	} // end if (adding the item failed)


	// Now try to read it from the remaining command line values
	hr = pParam->GetSelfAndValues(((this->m_dwFlags & CMDLINE_FLAG_CASESENSITIVE) ? TRUE : FALSE),
									&(this->m_specifieditems));
	if (hr != CMDLINE_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Failed getting parameter and value!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end if (failed getting param from command line)


	if (pfWasSpecified != NULL)
		(*pfWasSpecified) = pParam->m_fFound;

DONE:

	return (hr);
} // CCommandLine::CheckForItem
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::CheckForItems()"
//==================================================================================
// CCommandLine::CheckForItems
//----------------------------------------------------------------------------------
//
// Description: Adds items to parse on the command line.  This must be called after
//				Parse.
//
// Arguments:
//	PCMDLINEHANDLEDITEM aHandledItems	Array of items that you want to parse.
//	DWORD dwNumHandledItems				Number of items in previous array.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::CheckForItems(PCMDLINEHANDLEDITEM aHandledItems,
									DWORD dwNumHandledItems)
{
	HRESULT			hr;
	DWORD			dwTemp;
	PHANDLEDPARAM	pParam;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we've got a bad object)
#endif // DEBUG

	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(1, "Called with (%x, %u)", 2, aHandledItems, dwNumHandledItems);
	#endif // NO_TNCOMMON_DEBUG_SPEW
	*/

	for(dwTemp = 0; dwTemp < dwNumHandledItems; dwTemp++)
	{
		hr = this->AddItemInternal(&(aHandledItems[dwTemp]), &(this->m_handleditems));
		if (hr != CMDLINE_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Failed adding item!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (adding the item failed)


		// Get the last item
		pParam = (PHANDLEDPARAM) this->m_handleditems.GetPrevItem(NULL);
		if (pParam == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get item just added!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			hr = E_FAIL;
			goto DONE;
		} // end if (adding the item failed)


		// Now try to read it from the remaining command line values
		hr = pParam->GetSelfAndValues(((this->m_dwFlags & CMDLINE_FLAG_CASESENSITIVE) ? TRUE : FALSE),
										&(this->m_specifieditems));
		if (hr != CMDLINE_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Failed getting parameter and value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (failed getting param from command line)
	} // end for (each handled item)


DONE:

	return (hr);
} // CCommandLine::CheckForItems
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::GetPathToApp()"
//==================================================================================
// CCommandLine::GetPathToApp
//----------------------------------------------------------------------------------
//
// Description: Copies the path to this application into the string.  If the buffer
//				is NULL, the size required (including NULL termination) for the
//				string is placed in pdwBufferSize, and ERROR_BUFFER_TOO_SMALL is
//				returned.
//
// Arguments:
//	char* pszBuffer			String to store results in, or NULL to retrieve size.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to store size
//							required.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::GetPathToApp(char* pszBuffer, DWORD* pdwBufferSize)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)
#endif // DEBUG


	if (pszBuffer == NULL)
	{
		(*pdwBufferSize) = strlen(this->m_pszPathToApp) + 1;
		return (ERROR_BUFFER_TOO_SMALL);
	} // end if (just retrieving size)
	
	strcpy(pszBuffer, this->m_pszPathToApp);

	return (CMDLINE_OK);
} // CCommandLine::GetPathToApp
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // not supported on Xbox
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::GetPathToAppDir()"
//==================================================================================
// CCommandLine::GetPathToAppDir
//----------------------------------------------------------------------------------
//
// Description: Copies the path to this application's directory into the string.  If
//				the buffer is NULL, the size required (including NULL termination)
//				for the string is placed in pdwBufferSize, and
//				ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	char* pszBuffer			String to store results in, or NULL to retrieve size.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to store size
//							required.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::GetPathToAppDir(char* pszBuffer, DWORD* pdwBufferSize)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)
#endif // DEBUG


	// The size returned is actually too large (it's for the entire string, not just
	// the parent dir path).  Oh well.
	if (pszBuffer == NULL)
	{
		(*pdwBufferSize) = strlen(this->m_pszPathToApp) + 1;
		return (ERROR_BUFFER_TOO_SMALL);
	} // end if (just retrieving size)
	
	FileGetParentDirFromPath(this->m_pszPathToApp, pszBuffer);
	
	return (CMDLINE_OK);
} // CCommandLine::GetPathToAppDir
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX // not supported on Xbox
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::GetNameOfApp()"
//==================================================================================
// CCommandLine::GetNameOfApp
//----------------------------------------------------------------------------------
//
// Description: Copies the name of this application into the string.  If the buffer
//				is NULL, the size required (including NULL termination) for the
//				string is placed in pdwBufferSize, and ERROR_BUFFER_TOO_SMALL is
//				returned.
//
// Arguments:
//	char* pszBuffer			String to store results in, or NULL to retrieve size.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to store size
//							required.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::GetNameOfApp(char* pszBuffer, DWORD* pdwBufferSize)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)
#endif // DEBUG



	// The size returned is actually too large (it's for the entire string, not just
	// the app name).  Oh well.
	if (pszBuffer == NULL)
	{
		(*pdwBufferSize) = strlen(this->m_pszPathToApp) + 1;
		return (ERROR_BUFFER_TOO_SMALL);
	} // end if (just retrieving size)

	FileGetLastItemInPath(this->m_pszPathToApp, pszBuffer);

	return (CMDLINE_OK);
} // CCommandLine::GetNameOfApp
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::WasSpecified()"
//==================================================================================
// Overloaded CCommandLine::WasSpecified
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the item passed in was overridden with a value from
//				the command line, FALSE otherwise.  Note this must match a value
//				specified as a ppvValuePtr in the array passed to Initialize.
//
// Arguments:
//	PVOID* ppvItem		Pointer to item that was given to Initialize.
//
// Returns: TRUE if item was overridden, FALSE if still default.
//==================================================================================
BOOL CCommandLine::WasSpecified(PVOID* ppvItem)
{
	PHANDLEDPARAM	pParam = NULL;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG


	do
	{
		pParam = (PHANDLEDPARAM) this->m_handleditems.GetNextItem(pParam);
		if (pParam == NULL)
			break;

		if (pParam->m_ppvValuePtr == ppvItem)
			return (pParam->m_fFound);
	} // end do
	while (pParam != NULL);

	return (FALSE);
} // CCommandLine::WasSpecified()
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::GetSwitchValueString()"
//==================================================================================
// CCommandLine::GetSwitchValueString
//----------------------------------------------------------------------------------
//
// Description: Returns pointer to the string value of the switch with the given
//				name.  This will return NULL if an error occurred, or the default
//				value if it was not specified.  If you want to know whether this
//				item was specified or not, you will have to use NULL for the default
//				value, and check whether the return value is NULL.
//				If you did not pass an item with the same name into Initialize, then
//				it is added here and its value returned.  If it was not found on the
//				command line, NULL is returned.
//
// Arguments:
//	char* szName	Name of an item passed to Initialize, or new item to add.
//
// Returns: Pointer to string value of switch given, or NULL.
//==================================================================================
char* CCommandLine::GetSwitchValueString(char* szName)
{
	HRESULT				hr;
	PHANDLEDPARAM		pParam = NULL;
	BOOL				fMatch = FALSE;
	CMDLINEHANDLEDITEM	newitem;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG


	do
	{
		pParam = (PHANDLEDPARAM) this->m_handleditems.GetNextItem(pParam);
		if (pParam == NULL)
			break;

		// If this item isn't a string, skip it
		if (pParam->m_dwType != CMDLINE_PARAMTYPE_STRING)
			continue;

		if (this->m_dwFlags & CMDLINE_FLAG_CASESENSITIVE)
			fMatch = (strcmp(szName, pParam->GetString()) == 0) ? TRUE : FALSE;
		else
			fMatch = StringCmpNoCase(szName, pParam->GetString());

		if (fMatch)
			return (((PHANDLEDPARAMSTRING) pParam)->m_pszValue);
	} // end do
	while (pParam != NULL);


	// We didn't find the item specified, so it appears it wasn't passed to
	// Initialize.  Let's build it and add it now.
	ZeroMemory(&newitem, sizeof (CMDLINEHANDLEDITEM));
	newitem.dwType = CMDLINE_PARAMTYPE_STRING;
	newitem.pszName = szName;
	//newitem.pszDescription = NULL;
	newitem.dwOptions = CMDLINE_PARAMOPTION_HIDDEN;
	//newitem.ppvValuePtr = NULL;
	//newitem.lpvAdditionalData = NULL;
	//newitem.dwAdditionalDataSize = 0;

	//BUGBUG do we want to add it to the main list or just some temp one?
	hr = this->AddItemInternal(&newitem, &(this->m_handleditems));
	if (hr != CMDLINE_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Failed adding built-in item \"ini\"!  %e", 1, hr);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (NULL);
	} // end if (adding the item failed)

	// Get the last item
	pParam = (PHANDLEDPARAM) this->m_handleditems.GetPrevItem(NULL);
	if (pParam == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't get item just added!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (NULL);
	} // end if (adding the item failed)

	//BUGBUG check to make sure it's the right one


	// Now try to read it from the remaining command line values
	hr = pParam->GetSelfAndValues(((this->m_dwFlags & CMDLINE_FLAG_CASESENSITIVE) ? TRUE : FALSE),
									&(this->m_specifieditems));
	if (hr != CMDLINE_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Failed getting parameter and value!  %e", 1, hr);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (NULL);
	} // end if (failed getting param from command line)

	// If getting the value found it, this will be the value, otherwise it will be
	// NULL.  That's what we want, so just return it now.
	return (((PHANDLEDPARAMSTRING) pParam)->m_pszValue);
} // CCommandLine::GetSwitchValueString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no INI files supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::WriteToINI()"
//==================================================================================
// CCommandLine::WriteToINI
//----------------------------------------------------------------------------------
//
// Description: Writes all parsable command line items with the actual values to the
//				specified .ini file.
//
// Arguments:
//	char* szFilepath	Path to .ini file to write command line to.
//	DWORD dwOptions		Options to use when writing file.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::WriteToINI(char* szFilepath, DWORD dwOptions)
{
	HRESULT			hr;
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	int				i;
	//int				j;
	PHANDLEDPARAM	pParam = NULL;
	char*			pszTemp;
	char*			pcTemp;
	char*			pcTemp2;



	hr = FileCreateAndOpenFile(szFilepath,
							((dwOptions & CMDLINE_WRITEINIOPTION_FAILIFEXISTS) ? TRUE : FALSE),
							((dwOptions & CMDLINE_WRITEINIOPTION_APPEND) ? TRUE : FALSE),
							FALSE,
							&hFile);
	if (hr != S_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't create and open file \"%s\"!", 1, szFilepath);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end if (couldn't open file)

	FileWriteLine(hFile, "[CommandLine]");

	for(i = 0; i < this->m_handleditems.Count(); i++)
	{
		pParam = (PHANDLEDPARAM) this->m_handleditems.GetItem(i);
		if (pParam == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get command line item %i!", 1, i);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		if ((dwOptions & CMDLINE_WRITEINIOPTION_PRINTDESCRIPTIONS) &&
			(pParam->m_pszDescription != NULL))
		{
			/*
			FileWriteString(hFile, "//");
			// - 2 for "//"
			for(j = WRAP_DESCRIPTION_CHARACTERS - 2; j > 0; j--)
				FileWriteString(hFile, "-");
			FileWriteLine(hFile, "");
			*/


			pszTemp = pParam->m_pszDescription;

			do
			{
				// + 3 for "// "
				if (strlen(pszTemp) > (WRAP_DESCRIPTION_CHARACTERS + 3))
				{
					pcTemp = pszTemp + (WRAP_DESCRIPTION_CHARACTERS - 4);

					// Walk back until we find an appropriate place to word-wrap.
					while ((*pcTemp) != ' ')
					{
						pcTemp--;

						// If we're about to hit the beginning, we have to stop and
						// just break the line in the middle of the text.
						if (pcTemp == pszTemp + 1)
						{
							pcTemp = pszTemp + WRAP_DESCRIPTION_CHARACTERS - 4;
							break;
						} // end if (about to hit beginning)
					} // end while (haven't found space)

					// Skip all preceding spaces
					pcTemp2 = pcTemp;
					while (((*pcTemp2) == ' ') && (pcTemp2 > pszTemp))
						pcTemp2--;

					// Write this segment of the string, ignoring error.
					FileSprintfWriteLine(hFile, "// %z", 2, pszTemp,
										((pcTemp2 + 1) - pszTemp));

					// Skip all trailing spaces
					while (((*pcTemp) == ' ') && ((*pcTemp) != '\0'))
						pcTemp++;


					pszTemp += pcTemp - pszTemp;
				} // end if (longer than the line we want)
				else
				{
					// Ignore error
					FileSprintfWriteLine(hFile, "// %s", 1, pszTemp);

					pszTemp += strlen(pszTemp);
				} // end else (fits in one line)
			} // end do (while there's more string left)
			while (pszTemp[0] != '\0');


			/*
			FileWriteString(hFile, "//");
			// - 2 for "//"
			for(j = WRAP_DESCRIPTION_CHARACTERS - 2; j > 0; j--)
				FileWriteString(hFile, "-");
			FileWriteLine(hFile, "");
			*/
		} // end if (we can print description and there is one)


		// Ignore error
		pParam->PrintToFile(hFile,
							((dwOptions & CMDLINE_WRITEINIOPTION_PRINTDESCRIPTIONS) ? TRUE : FALSE));


		if ((dwOptions & CMDLINE_WRITEINIOPTION_PRINTDESCRIPTIONS))
		{
			// Space out the items, ignoring error
			FileWriteLine(hFile, "");
		} // end if (printing descriptions)
	} // end for (each item)


DONE:

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	} // end if (file is still open)

	return (hr);
} // CCommandLine::WriteToINI
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::AddItemInternal()"
//==================================================================================
// CCommandLine::AddItemInternal
//----------------------------------------------------------------------------------
//
// Description: Adds the specified item to the list.
//
// Arguments:
//	PCMDLINEHANDLEDITEM pItem			Pointer to the item to add.
//	PHANDLEDPARAMSLIST pHandledItems	Pointer to list to add item to.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::AddItemInternal(PCMDLINEHANDLEDITEM pItem,
										PHANDLEDPARAMSLIST pHandledItems)
{
	HRESULT			hr = CMDLINE_OK;
	PHANDLEDPARAM	pParam = NULL;


	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "Adding cmdline item \"%s\".", 1, lpItem->pszName);
	#endif // NO_TNCOMMON_DEBUG_SPEW
	*/

	pParam = (PHANDLEDPARAM) pHandledItems->GetStringObject(pItem->pszName, 0,
															((this->m_dwFlags & CMDLINE_FLAG_CASESENSITIVE) ? TRUE : FALSE));
	if (pParam != NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Already specified an item with name \"%s\"!",
			1, pItem->pszName);
		#endif // NO_TNCOMMON_DEBUG_SPEW
		return (CMDLINEERR_DUPLICATEHANDLEDITEM);
	} // end if (user already specified that name)


	switch (pItem->dwType)
	{
		case CMDLINE_PARAMTYPE_BOOL:
			pParam = (PHANDLEDPARAM) new (CHandledParamBool)(pItem->pszName,
															pItem->pszDescription,
															pItem->dwOptions,
															pItem->ppvValuePtr);
			if (pParam == NULL)
			{
				hr = E_OUTOFMEMORY;
				break;
			} // end if (couldn't allocate object)

			hr = pHandledItems->Add(pParam);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't add boolean \"%s\" to list!",
					1, pItem->pszName);
				#endif // NO_TNCOMMON_DEBUG_SPEW

				delete (pParam);
				pParam = NULL;

				//break;
			} // end if (couldn't allocate object)
		  break;

		case CMDLINE_PARAMTYPE_INT:
			pParam = (PHANDLEDPARAM) new (CHandledParamInt)(pItem->pszName,
															pItem->pszDescription,
															pItem->dwOptions,
															pItem->ppvValuePtr);
			if (pParam == NULL)
			{
				hr = E_OUTOFMEMORY;
				break;
			} // end if (couldn't allocate object)

			hr = pHandledItems->Add(pParam);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't add int \"%s\" to list!",
					1, pItem->pszName);
				#endif // NO_TNCOMMON_DEBUG_SPEW

				delete (pParam);
				pParam = NULL;

				//break;
			} // end if (couldn't allocate object)
		  break;

		case CMDLINE_PARAMTYPE_DWORD:
			pParam = (PHANDLEDPARAM) new (CHandledParamDWord)(pItem->pszName,
															pItem->pszDescription,
															pItem->dwOptions,
															pItem->ppvValuePtr);
			if (pParam == NULL)
			{
				hr = E_OUTOFMEMORY;
				break;
			} // end if (couldn't allocate object)

			hr = pHandledItems->Add(pParam);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't add DWORD \"%s\" to list!",
					1, pItem->pszName);
				#endif // NO_TNCOMMON_DEBUG_SPEW

				delete (pParam);
				pParam = NULL;

				//break;
			} // end if (couldn't allocate object)
		  break;

		case CMDLINE_PARAMTYPE_DWORD_HEX:
			pParam = (PHANDLEDPARAM) new (CHandledParamDWordHex)(pItem->pszName,
																pItem->pszDescription,
																pItem->dwOptions,
																pItem->ppvValuePtr);
			if (pParam == NULL)
			{
				hr = E_OUTOFMEMORY;
				break;
			} // end if (couldn't allocate object)

			hr = pHandledItems->Add(pParam);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't add DWORD_HEX \"%s\" to list!",
					1, pItem->pszName);
				#endif // NO_TNCOMMON_DEBUG_SPEW

				delete (pParam);
				pParam = NULL;

				//break;
			} // end if (couldn't allocate object)
		  break;

		case CMDLINE_PARAMTYPE_STRING:
			pParam = (PHANDLEDPARAM) new (CHandledParamString)(pItem->pszName,
																pItem->pszDescription,
																pItem->dwOptions,
																pItem->ppvValuePtr);
			if (pParam == NULL)
			{
				hr = E_OUTOFMEMORY;
				break;
			} // end if (couldn't allocate object)

			hr = pHandledItems->Add(pParam);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't add string \"%s\" to list!",
					1, pItem->pszName);
				#endif // NO_TNCOMMON_DEBUG_SPEW

				delete (pParam);
				pParam = NULL;

				//break;
			} // end if (couldn't allocate object)
		  break;

		case CMDLINE_PARAMTYPE_ID:
			if ((pItem->pvAdditionalData == NULL) ||
				(pItem->dwAdditionalDataSize < (sizeof (CMDLINEIDITEM))))
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "ID Item table is invalid!",
					1, pItem->pszName);
				#endif // NO_TNCOMMON_DEBUG_SPEW
				hr = CMDLINEERR_INVALIDINITDATA;
				break;
			} // end if (user didn't give a valid table)

			pParam = (PHANDLEDPARAM) new (CHandledParamID)(pItem->pszName,
															pItem->pszDescription,
															pItem->dwOptions,
															pItem->ppvValuePtr,
															(PCMDLINEIDITEM) (pItem->pvAdditionalData),
															(pItem->dwAdditionalDataSize / sizeof (CMDLINEIDITEM)));
			if (pParam == NULL)
			{
				hr = E_OUTOFMEMORY;
				break;
			} // end if (couldn't allocate object)

			hr = pHandledItems->Add(pParam);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't add ID \"%s\" to list!",
					1, pItem->pszName);
				#endif // NO_TNCOMMON_DEBUG_SPEW

				delete (pParam);
				pParam = NULL;

				//break;
			} // end if (couldn't allocate object)
		  break;

		case CMDLINE_PARAMTYPE_MULTISTRING:
			pParam = (PHANDLEDPARAM) new (CHandledParamMultiString)(pItem->pszName,
																	pItem->pszDescription,
																	pItem->dwOptions,
																	pItem->ppvValuePtr);
			if (pParam == NULL)
			{
				hr = E_OUTOFMEMORY;
				break;
			} // end if (couldn't allocate object)

			hr = pHandledItems->Add(pParam);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't add multistring \"%s\" to list!",
					1, pItem->pszName);
				#endif // NO_TNCOMMON_DEBUG_SPEW

				delete (pParam);
				pParam = NULL;

				//break;
			} // end if (couldn't allocate object)
		  break;

		default:
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Unknown item type %u!", 1, pItem->dwType);
			#endif // NO_TNCOMMON_DEBUG_SPEW
			hr = CMDLINEERR_UNKNOWNITEMTYPE;
		  break;
	} // end switch (on item type)

	return (hr);
} // CCommandLine::AddItemInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::ParseInternal()"
//==================================================================================
// CCommandLine::ParseInternal
//----------------------------------------------------------------------------------
//
// Description: Parses the command line arguments passed in.
//
// Arguments:
//	char* szCommandLine						The command line string to parse.
//	PHANDLEDPARAMSLIST pHandledItems		Pointer to list containing handled
//											items.
//	PSPECIFIEDPARAMSLIST pSpecifiedItems	Pointer to list to hold remaining
//											specified items.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::ParseInternal(char* szCommandLine,
									PHANDLEDPARAMSLIST pHandledItems,
									PSPECIFIEDPARAMSLIST pSpecifiedItems)
{
	HRESULT			hr;
	BOOL			fDone = FALSE;
	DWORD			dwIndex = 0;
	DWORD			dwItemStart = 0xFFFFFFFF;
	DWORD			dwQuoteStart = 0xFFFFFFFF;
	BOOL			fSwitch;
	PHANDLEDPARAM	pParam = NULL;


	// Split line into spaced components but spaces inside non-escaped quote
	// characters don't count.
	do
	{
		switch (szCommandLine[dwIndex])
		{
			case '\0':
				// End of the string.  If we're in an item, add it.
				if (dwItemStart != 0xFFFFFFFF)
				{
					// If the item starts with a switch character, it's a switch
					if (StringContainsChar(CMDLINE_SWITCH_TOKENS, szCommandLine[dwItemStart], TRUE, 0) >= 0)
						fSwitch = TRUE;
					else
						fSwitch = FALSE;

					// Skip past the switch char if it's there, and allow it to be
					// split.
					hr = pSpecifiedItems->AddItem(((fSwitch) ? szCommandLine + dwItemStart + 1 : szCommandLine + dwItemStart),
													fSwitch, TRUE, FALSE);
					if (hr != S_OK)
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't add string!", 0);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
						return (hr);
					} // end if (couldn't add string)

					dwItemStart = 0xFFFFFFFF;
				} // end if (in the middle of an item)

				fDone = TRUE;
			  break;

			case ' ':
				// Space.  If we're in a non-quoted item, add it.
				if ((dwItemStart != 0xFFFFFFFF) && (dwQuoteStart == 0xFFFFFFFF))
				{
					// If the item starts with a switch character, it's a switch
					if (StringContainsChar(CMDLINE_SWITCH_TOKENS, szCommandLine[dwItemStart], TRUE, 0) >= 0)
						fSwitch = TRUE;
					else
						fSwitch = FALSE;

					szCommandLine[dwIndex] = '\0'; // temporarily end the string here

					// Skip past the switch char if it's there, and allow it to be
					// split.
					hr = pSpecifiedItems->AddItem(((fSwitch) ? szCommandLine + dwItemStart + 1 : szCommandLine + dwItemStart),
													fSwitch, TRUE, FALSE);

					szCommandLine[dwIndex] = ' '; // go back to it being a space

					if (hr != S_OK)
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't add string!", 0);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
						return (hr);
					} // end if (couldn't add string)

					dwItemStart = 0xFFFFFFFF;
				} // end if (in the middle of an item)
			  break;

			case '\"':
				// Quote character.  Start or end the quotes, unless this quote
				// character is escaped.

				if ((dwIndex <= 0) || (szCommandLine[dwIndex - 1] != '\\'))
				{
					if (dwQuoteStart == 0xFFFFFFFF)
					{
						if (dwItemStart == 0xFFFFFFFF)
							dwItemStart = dwIndex;

						dwQuoteStart = dwIndex;
					} // end if (we're starting a quote)
					else
					{
						dwQuoteStart = 0xFFFFFFFF;
					} // end else (we're ending a quote)
				} // end if (non-escaped character)
			  break;

			default:
				// Any other characters.  Just treat it as an item start.
				if (dwItemStart == 0xFFFFFFFF)
					dwItemStart = dwIndex;
			  break;
		} // end switch (on the current character)

		dwIndex++;
	} // end do (while not at the end of the string)
	while (! fDone);


	// We now have a list of individual strings.  We want to parse them as
	// appropriate.

#ifndef _XBOX // no INI files just yet...
	// If the user specified INI files as allowable, check for that first.
	if (! (this->m_dwFlags & CMDLINE_FLAG_DONTHANDLEINIFILES))
	{
		// The first item should be "ini".  And if it was specified, we want to
		// read the file in now, so we only have to parse once.  Things still on
		// the command line override what was in the .ini file, since ini items are
		// added at the end of the list.

		pParam = (PHANDLEDPARAM) pHandledItems->GetNextItem(pParam);
		if ((pParam == NULL) || (strcmp(pParam->GetString(), "ini") != 0))
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "First parameter wasn't \"%ini\"!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			return (ERROR_BAD_ENVIRONMENT);
		} // end if (first item wasn't ini)
		

		// "ini" is always case insensitive, hence FALSE
		hr = pParam->GetSelfAndValues(FALSE, &(this->m_specifieditems));
		if (hr != CMDLINE_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get \"%ini\" from command line!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			return (hr);
		} // end if (failed getting ini from command line)
		

		// If we found the ini file, we want to read it in right now before
		// continuing.
		if (pParam->m_fFound)
		{
			DWORD	dwSize;
			char*	pszSpecifiedPath;
			char*	pszRealPath;


			// Add a NULL string.  Since the GetSelfAndValues parsers may try to
			// grab "the next item", meaning it may find a switch with no value in
			// the same string so it has to retrieve the next string, tacking on all
			// the ini file items at the end may cause it to walk off the command
			// line and into the ini file entries, which is obviously not good.
			// Adding a NULL separator forces it to stop in case it was going to.
			hr = pSpecifiedItems->AddItem(NULL, FALSE, FALSE, FALSE);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't add NULL string to list!", 0);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				return (hr);
			} // end if (failed getting ini from command line)


			pszSpecifiedPath = ((PHANDLEDPARAMSTRING) pParam)->m_pszValue;

			dwSize = strlen(pszSpecifiedPath) + 1; // + NULL termination

			// Check for surrounding quotes.
			if (StringStartsWith(pszSpecifiedPath, "\"", TRUE))
			{
				if ((! StringEndsWith(pszSpecifiedPath, "\"", TRUE)) ||
					(StringIsCharBackslashEscaped(pszSpecifiedPath, pszSpecifiedPath + strlen(pszSpecifiedPath))))
				{
					#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(0, "Ini path starts but doesn't end with a non-escaped quote character!", 0);
					#endif // ! NO_TNCOMMON_DEBUG_SPEW
					return (E_FAIL);
				} // end if (string doesn't end with a quote)

				dwSize -= 2;
			} // end if (item is surrounded by quotes)

			pszRealPath = (char*) LocalAlloc(LPTR, dwSize);
			if (pszRealPath == NULL)
			{
				return (E_OUTOFMEMORY);
			} // end if (couldn't allocate memory)

			if (StringStartsWith(pszSpecifiedPath, "\"", TRUE))
			{
				CopyMemory(pszRealPath, pszSpecifiedPath + 1,
							strlen(pszSpecifiedPath) - 2);
			} // end if (item is surrounded by quotes)
			else
			{
				strcpy(pszRealPath, pszSpecifiedPath);
			} // end else (item is not surrounded by quotes)


			hr = this->ReadIniFileInternal(pszRealPath, pSpecifiedItems);
			if (hr != CMDLINE_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't read ini file \"%s\"!", 1, pszRealPath);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW

				LocalFree(pszRealPath);
				pszRealPath = NULL;

				return (hr);
			} // end if (failed getting ini from command line)

			LocalFree(pszRealPath);
			pszRealPath = NULL;
		} // end if (we found ini on the command line)
	} // end if (ini files are allowable)
#endif // ! XBOX

	// Now loop through the rest of the handled items and find them in the command
	// line.
	do
	{
		pParam = (PHANDLEDPARAM) pHandledItems->GetNextItem(pParam);
		if (pParam == NULL)
			break;

		hr = pParam->GetSelfAndValues(((this->m_dwFlags & CMDLINE_FLAG_CASESENSITIVE) ? TRUE : FALSE),
										&(this->m_specifieditems));
		if (hr != CMDLINE_OK)
			return (hr);

		// If this item was supposed to be on the command line, but we didn't find
		// it, then return an error.
		if ((! pParam->m_fFound) &&
			(pParam->m_dwOptions & CMDLINE_PARAMOPTION_FAILIFNOTEXIST))
		{
			if (this->m_dwFlags & CMDLINE_FLAG_MISSINGPARAMMEANSHELP)
			{
				// Put us into help mode
				this->m_fHelp = TRUE;
			} // end if (should go into help mode)
			else
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't find required parameter \"%s\"!",
					1, pParam->GetString());
				#endif // ! NO_TNCOMMON_DEBUG_SPEW

				return (CMDLINEERR_REQUIREDPARAMNOTPASSED);
			} // end else (should fail directly)
		} // end if (param had to be there but it wasn't)
	} // end do (while haven't looked at all the handled items)
	while (TRUE);

	return (CMDLINE_OK);
} // CCommandLine::ParseInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no INI files just yet...
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::ReadIniFileInternal()"
//==================================================================================
// CCommandLine::ReadIniFileInternal
//----------------------------------------------------------------------------------
//
// Description: Reads the INI file specified and loads all the items into the string
//				list passed in.  If the given item doesn't end in an extension, .ini
//				is assumed.
//
// Arguments:
//	char* szFilepath				The path to the INI file to read.
//	PSPECIFIEDPARAMSLIST pItemList	Pointer to the list to add the items to.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CCommandLine::ReadIniFileInternal(char* szFilepath,
										PSPECIFIEDPARAMSLIST pItemList)
{
	HRESULT					hr = CMDLINE_OK;
	char					szExtension[32];
	DWORD					dwTempSize;
	char*					pszTemp = NULL;
	CMDLINEREADFILECONTEXT	clrfc;


	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(1, "Called with (%s, %x).", 2, szFilepath, lpItemList);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/

	// Build the file path.  It's just a straight copy of the one passed in unless
	// the path doesn't end in an extension, in which case ".ini" is tacked on.

	dwTempSize = strlen(szFilepath) + 1;
	if (dwTempSize <= 1)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Bad file path specified!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (couldn't open file)

	ZeroMemory(szExtension, 32);
	FileGetExtension(szFilepath, szExtension, FALSE);
	if (strcmp(szExtension, "") == 0)
		dwTempSize += 4;

	pszTemp = (char*) LocalAlloc(LPTR, dwTempSize);
	if (pszTemp == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	strcpy(pszTemp, szFilepath);
	if (strcmp(szExtension, "") == 0)
		strcat(pszTemp, ".ini");

	ZeroMemory(&clrfc, sizeof (CMDLINEREADFILECONTEXT));
	//clrfc.fInCommandLineSection = FALSE;
	clrfc.pItemList = pItemList;

	hr = FileReadTextFile(pszTemp, FILERTFO_ALLOPTIONS, CmdlineReadFileCB, &clrfc);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't read text file \"%s\"!", 1, pszTemp);
		goto DONE;
	} // end if (couldn't read text file)


DONE:

	if (pszTemp != NULL)
	{
		LocalFree(pszTemp);
		pszTemp = NULL;
	} // end if (we allocated the filepath)

	return (hr);
} // CCommandLine::ReadIniFileInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CCommandLine::DisplayHelpPageInternal()"
//==================================================================================
// CCommandLine::DisplayHelpPageInternal
//----------------------------------------------------------------------------------
//
// Description: Copies the path to this application into the string.
//
// Arguments:
//	DWORD* pdwStartItem		Pointer to first help item on this page, and place to
//							update start of next page if there are more pages to
//							display.
//	HWND hWnd				Window to use, if in Windows app.
//
// Returns: None.
//==================================================================================
HRESULT CCommandLine::DisplayHelpPageInternal(DWORD* pdwStartItem, HWND hWnd)
{
	HRESULT		hr = S_OK;
	DWORD		dwTemp;
#ifndef CMDLINE_CONSOLE_ONLY
	HDC			hdc = NULL;
	//HFONT		hFont = NULL;
	SIZE		size;
	LONG		lRightWidth;
	LONG		lCurrentHeight = 0;
	BOOL		fMorePages = FALSE;
	char*		pszRemainingLine = NULL;
	HWND		hSubWnd = NULL;
	char*		pcCurrent;
#endif // ! CMDLINE_CONSOLE_ONLY


	// If we're a windows app, get the device context
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	if (! this->m_fConsole)
	{
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
#ifndef CMDLINE_CONSOLE_ONLY
		// Remove all the current text items, if any
		while (GetDlgItem(hWnd, IDS_ALL) != NULL)
			DestroyWindow(GetDlgItem(hWnd, IDS_ALL));


		hdc = GetDC(hWnd);
		if (hdc == NULL)
		{
			hr = GetLastError();
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get window's device context!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (couldn't get device context)
#endif // ! CMDLINE_CONSOLE_ONLY
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	} // end if (a Windows app)
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)

	for(dwTemp = (*pdwStartItem); dwTemp < this->m_dwNumHelpItems; dwTemp++)
	{
		if (this->m_papHelpItems[dwTemp] == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Help item %u is non-existent!", 1, dwTemp);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			hr = E_FAIL;
			goto DONE;
		} // end if (there isn't an item there)

#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
		if (this->m_fConsole)
		{
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
#ifndef CMDLINE_WINDOWS_ONLY
			printf(this->m_papHelpItems[dwTemp]->GetString());

			// Print the description if there is one
			if (this->m_papHelpItems[dwTemp]->m_pszDescription != NULL)
			{
				DWORD	dwSpaces;


				// Pad the left field with enough spaces
				dwSpaces = this->m_lLargestLeftHelpTextWidth - strlen(this->m_papHelpItems[dwTemp]->GetString()) + 1;
				while (dwSpaces > 0)
				{
					printf(" ");
					dwSpaces--;
				} // end while (need more spacing)

				printf(this->m_papHelpItems[dwTemp]->m_pszDescription);
			} // end if (there's a description)

			printf("\n");

			//BUGBUG pause if exceed a page height?

#endif // ! CMDLINE_WINDOWS_ONLY
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
		} // end if (console app)
		else
		{
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
#ifndef CMDLINE_CONSOLE_ONLY
			// Find out where end of param name will be.
			pcCurrent = this->m_pszTempParamNameBuffer
						+ strlen(this->m_papHelpItems[dwTemp]->GetString());

			// Copy the param name (without the NULL termination).
			memcpy(this->m_pszTempParamNameBuffer,
					this->m_papHelpItems[dwTemp]->GetString(),
					((DWORD_PTR) pcCurrent - (DWORD_PTR) this->m_pszTempParamNameBuffer));

			// Make sure remaining buffer is filled with periods.
			while ((*pcCurrent) != '\0')
			{
				(*pcCurrent) = '.';
				pcCurrent++;
			} // end while (still more buffer)

			
			// Create the param name static text window
#ifdef LOCAL_TNCOMMON
			hSubWnd = CreateWindow("STATIC",
									this->m_pszTempParamNameBuffer,
									WS_CHILD | WS_VISIBLE | SS_SIMPLE,
									0,
									lCurrentHeight,
									this->m_lLargestLeftHelpTextWidth,
									this->m_lLargestHelpLineHeight,
									hWnd,
									(HMENU) IDS_ALL,
									NULL,
									0);
#else // ! LOCAL_TNCOMMON
			hSubWnd = CreateWindow("STATIC",
									this->m_pszTempParamNameBuffer,
									WS_CHILD | WS_VISIBLE | SS_SIMPLE,
									0,
									lCurrentHeight,
									this->m_lLargestLeftHelpTextWidth,
									this->m_lLargestHelpLineHeight,
									hWnd,
									(HMENU) IDS_ALL,
									s_hInstance,
									0);
#endif // ! LOCAL_TNCOMMON
			if (hSubWnd == NULL)
			{
				hr = GetLastError();

				if (hr == S_OK)
					hr = E_FAIL;

				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't create text \"%s\"!",
					1, this->m_pszTempParamNameBuffer);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (couldn't create window)

			// Change the font to a fixed font.
			SetWindowFont(hSubWnd, GetStockObject(ANSI_FIXED_FONT), TRUE);


			// Word wrap the description if there is one
			if ((this->m_papHelpItems[dwTemp]->m_pszDescription != NULL) &&
				(strlen(this->m_papHelpItems[dwTemp]->m_pszDescription) > 0))
			{
				char*	lpcCurrent;


				pszRemainingLine = (char*) LocalAlloc(LPTR, strlen(this->m_papHelpItems[dwTemp]->m_pszDescription) + 1);
				if (pszRemainingLine == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)
				strcpy(pszRemainingLine, this->m_papHelpItems[dwTemp]->m_pszDescription);

				lpcCurrent = pszRemainingLine;
				lRightWidth = 0;

				while ((*lpcCurrent) != '\0')
				{
					// Prepare to calculate using a more pleasing font.
					SelectFont(hdc, GetStockObject(ANSI_VAR_FONT));

					if (! GetTextExtentPoint32(hdc, lpcCurrent, 1, &size))
					{
						hr = GetLastError();
						if (hr == S_OK)
							hr = E_FAIL;

						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't get text extents for '%c'!", 1, lpcCurrent);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW

						goto DONE;
					} // end if (couldn't get text size)

					lRightWidth += size.cx;

					// If we went over the limit for text width, find the
					// most recent word break and wrap there
					if (lRightWidth > this->m_lLargestRightHelpTextWidth)
					{
						while (((*lpcCurrent) != ' ') && (lpcCurrent > pszRemainingLine))
							lpcCurrent--;

						// If we didn't find a word break earlier in the
						// line, we're just going to have to clip it, and
						// start with the next word.
						if (lpcCurrent == pszRemainingLine)
						{
							while (((*lpcCurrent) != ' ') && ((*lpcCurrent) != '\0'))
								lpcCurrent++;

							// Stop if we hit the end of the string
							if ((*lpcCurrent) != '\0')
								break;
						} // end if (we didn't find a word break)

						// Display the line up to this break
						(*lpcCurrent) = '\0';

#ifdef LOCAL_TNCOMMON
						hSubWnd = CreateWindow("STATIC",
												pszRemainingLine,
												WS_CHILD | WS_VISIBLE | SS_SIMPLE,
												this->m_lLargestLeftHelpTextWidth + 5,
												lCurrentHeight,
												this->m_lLargestRightHelpTextWidth,
												this->m_lLargestHelpLineHeight,
												hWnd,
												(HMENU) IDS_ALL,
												NULL,
												0);
#else // ! LOCAL_TNCOMMON
						hSubWnd = CreateWindow("STATIC",
												pszRemainingLine,
												WS_CHILD | WS_VISIBLE | SS_SIMPLE,
												this->m_lLargestLeftHelpTextWidth + 5,
												lCurrentHeight,
												this->m_lLargestRightHelpTextWidth,
												this->m_lLargestHelpLineHeight,
												hWnd,
												(HMENU) IDS_ALL,
												s_hInstance,
												0);
#endif // ! LOCAL_TNCOMMON
						if (hSubWnd == NULL)
						{
							hr = GetLastError();

							if (hr == S_OK)
								hr = E_FAIL;

							#ifndef NO_TNCOMMON_DEBUG_SPEW
							DPL(0, "Couldn't create text \"%s\"!", 1, pszRemainingLine);
							#endif // ! NO_TNCOMMON_DEBUG_SPEW
							goto DONE;
						} // end if (couldn't create window)

						// Change the font to a more pleasing font.
						SetWindowFont(hSubWnd, GetStockObject(ANSI_VAR_FONT), TRUE);

						// Slide all the remaining text down to the beginning
						// of the string, and dump any extra spaces.
						memcpy(pszRemainingLine, lpcCurrent + 1, strlen(lpcCurrent + 1) + 1);
						StringPopLeadingChars(pszRemainingLine, " ", TRUE);

						lpcCurrent = pszRemainingLine;

						lCurrentHeight += this->m_lLargestHelpLineHeight;

						// Reset the line width counter
						lRightWidth = 0;
					} // end if (this puts us over the limit)
					else
					{
						// Move to next character
						lpcCurrent++;
					} // end else (we're still in the limit)
				} // end while (not at the end of the string)

				// If there's any string left, dump it.
				if (strlen(pszRemainingLine) > 0)
				{
#ifdef LOCAL_TNCOMMON
					hSubWnd = CreateWindow("STATIC",
											pszRemainingLine,
											WS_CHILD | WS_VISIBLE | SS_SIMPLE,
											this->m_lLargestLeftHelpTextWidth + 5,
											lCurrentHeight,
											this->m_lLargestRightHelpTextWidth,
											this->m_lLargestHelpLineHeight,
											hWnd,
											(HMENU) IDS_ALL,
											NULL,
											0);
#else // ! LOCAL_TNCOMMON
					hSubWnd = CreateWindow("STATIC",
											pszRemainingLine,
											WS_CHILD | WS_VISIBLE | SS_SIMPLE,
											this->m_lLargestLeftHelpTextWidth + 5,
											lCurrentHeight,
											this->m_lLargestRightHelpTextWidth,
											this->m_lLargestHelpLineHeight,
											hWnd,
											(HMENU) IDS_ALL,
											s_hInstance,
											0);
#endif // ! LOCAL_TNCOMMON
					if (hSubWnd == NULL)
					{
						hr = GetLastError();

						if (hr == S_OK)
							hr = E_FAIL;

						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't create text \"%s\"!", 1, pszRemainingLine);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
						goto DONE;
					} // end if (couldn't create window)

					// Change the font to a more pleasing font.
					SetWindowFont(hSubWnd, GetStockObject(ANSI_VAR_FONT), TRUE);
				} // end if (there's more to dump)
				else
				{
					// BUGBUG Subtract it?
					//lCurrentHeight -= this->m_lLargestHelpLineHeight + 7;
				} // end else (there's nothing to dump)

				LocalFree(pszRemainingLine);
				pszRemainingLine = NULL;
			} // end if (there's a description)

			// Include space for each item + a little buffer
			lCurrentHeight += this->m_lLargestHelpLineHeight + 7;

			// Note this only ends the page on an item, so if you have
			// a really long description the window could go beyond
			// this limit.
			if ((lCurrentHeight >= this->m_lMaxHelpHeight) &&
				(dwTemp < this->m_dwNumHelpItems - 1))
			{
				fMorePages = TRUE;
				(*pdwStartItem) = dwTemp + 1;
				break;
			} // end if (we're going past the height)
#endif // ! CMDLINE_CONSOLE_ONLY
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
		} // end else (in Windows mode)
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
	} // end for (each item)


	// If we're a Windows app, resize the window, show buttons, cleanup, etc.
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	if (! this->m_fConsole)
	{
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
#ifndef CMDLINE_CONSOLE_ONLY
		if (hdc != NULL)
		{
			ReleaseDC(hWnd, hdc); // ignore error;
			hdc = NULL;
		} // end if (got device context)

		// Space out the text from the button
		lCurrentHeight += 10;

		if (fMorePages)
		{
			// Center the OK button on the left
			if (! MoveWindow(GetDlgItem(hWnd, IDOK),
							(((this->m_lLargestLeftHelpTextWidth + this->m_lLargestRightHelpTextWidth + 20) / 2) - 110),
							lCurrentHeight,
							100,
							25,
							TRUE))
			{
				hr = GetLastError();
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't move OK button!", 0);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (couldn't move OK button)

			// Center the Next button on the right
			if (! MoveWindow(GetDlgItem(hWnd, IDB_NEXT),
							(((this->m_lLargestLeftHelpTextWidth + this->m_lLargestRightHelpTextWidth + 20) / 2) + 10),
							lCurrentHeight,
							100,
							25,
							TRUE))
			{
				hr = GetLastError();
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't move Next button!", 0);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (couldn't move Next button)

			// Show the Next button
			ShowWindow(GetDlgItem(hWnd, IDB_NEXT), SW_SHOW);

			// Hide the Back button
			ShowWindow(GetDlgItem(hWnd, IDB_BACKTOFIRST), SW_HIDE);
		} // end if (there are more pages)
		else if (this->m_dwHelpStartItem > 0)
		{
			// Center the OK button on the left
			if (! MoveWindow(GetDlgItem(hWnd, IDOK),
							(((this->m_lLargestLeftHelpTextWidth + this->m_lLargestRightHelpTextWidth + 20) / 2) - 110),
							lCurrentHeight,
							100,
							25,
							TRUE))
			{
				hr = GetLastError();
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't move OK button!", 0);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (couldn't move OK button)

			// Hide the Next button
			ShowWindow(GetDlgItem(hWnd, IDB_NEXT), SW_HIDE);

			// Center the Back button on the right
			if (! MoveWindow(GetDlgItem(hWnd, IDB_BACKTOFIRST),
							(((this->m_lLargestLeftHelpTextWidth + this->m_lLargestRightHelpTextWidth + 20) / 2) + 10),
							lCurrentHeight,
							100,
							25,
							TRUE))
			{
				hr = GetLastError();
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't move Back button!", 0);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (couldn't move Back button)

			// Show the Back button
			ShowWindow(GetDlgItem(hWnd, IDB_BACKTOFIRST), SW_SHOW);
		} // end else if (there is a first page)
		else
		{
			// Center the OK button
			if (! MoveWindow(GetDlgItem(hWnd, IDOK),
							(((this->m_lLargestLeftHelpTextWidth + this->m_lLargestRightHelpTextWidth + 20) / 2) - 50),
							lCurrentHeight,
							100,
							25,
							TRUE))
			{
				hr = GetLastError();
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't move OK button!", 0);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (couldn't move OK button)

			// Get rid of the Next and Back buttons
			DestroyWindow(GetDlgItem(hWnd, IDB_NEXT));
			DestroyWindow(GetDlgItem(hWnd, IDB_BACKTOFIRST));
		} // end else (there's only one page)

		// Button height + some extra (don't know why)
		lCurrentHeight += 25 + 30;


		// Resize the main window
		if (! MoveWindow(hWnd,
						0,
						0,
						(this->m_lLargestLeftHelpTextWidth + this->m_lLargestRightHelpTextWidth + 20),
						lCurrentHeight,
						TRUE))
		{
			hr = GetLastError();
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't resize window!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (couldn't resize window)
#endif // ! CMDLINE_CONSOLE_ONLY
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
	} // end if (a Windows app)
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)


DONE:

#ifndef CMDLINE_CONSOLE_ONLY
	if (pszRemainingLine != NULL)
	{
		LocalFree(pszRemainingLine);
		pszRemainingLine = NULL;
	} // end if (allocated a remaining line string)

	if (hdc != NULL)
	{
		ReleaseDC(hWnd, hdc); // ignore error;
		hdc = NULL;
	} // end if (got device context)
#endif // ! CMDLINE_CONSOLE_ONLY

	return (hr);
} // CCommandLine::DisplayHelpPageInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParam::CHandledParam()"
//==================================================================================
// CHandledParam constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CHandledParam object.  Initializes the string and
//				options and value pointer to the ones passed in.
//
// Arguments:
//	char* szName			Name of this item.
//	char* pszDescription	Pointer to optional description of this item
//	DWORD dwType			Type of this item (child class ID)
//	DWORD dwOptions			Options for this item.
//	PVOID* ppvValuePtr		Pointer to default value and place to update with value.
//
// Returns: None (the object).
//==================================================================================
CHandledParam::CHandledParam(char* szName, char* pszDescription, DWORD dwType,
							DWORD dwOptions, PVOID* ppvValuePtr):
	CLString(szName),
	m_pszDescription(NULL),
	m_dwType(dwType),
	m_dwOptions(dwOptions),
	m_fFound(FALSE),
	m_ppvValuePtr(ppvValuePtr)
{
	if (pszDescription != NULL)
	{
		this->m_pszDescription = (char*) LocalAlloc(LPTR, (strlen(pszDescription) + 1));
		if (this->m_pszDescription != NULL)
			strcpy(this->m_pszDescription, pszDescription);
	} // end if (there's a description)
} // CHandledParam::CHandledParam
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParam::~CHandledParam()"
//==================================================================================
// CHandledParam destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CHandledParam object and any resources it may have
//				allocated.
//
// Arguments: None.
//
// Returns: None
//==================================================================================
CHandledParam::~CHandledParam(void)
{
	if (this->m_pszDescription != NULL)
	{
		LocalFree(this->m_pszDescription);
		this->m_pszDescription = NULL;
	} // end if (we allocated a description)
} // CHandledParam::CHandledParam 
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamBool::CHandledParamBool()"
//==================================================================================
// CHandledParamBool constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CHandledParamBool object.  Initializes the name and
//				value to the ones passed in.
//
// Arguments:
//	char* szName			Name of this item.
//	char* pszDescription	Pointer to optional description of this item
//	DWORD dwOptions			Options for this item.
//	PVOID* ppvValuePtr		Pointer to default value and place to update with value.
//
// Returns: None (the object).
//==================================================================================
CHandledParamBool::CHandledParamBool(char* szName, char* pszDescription,
									DWORD dwOptions, PVOID* ppvValuePtr):
	CHandledParam(szName, pszDescription, CMDLINE_PARAMTYPE_BOOL, dwOptions,
				ppvValuePtr),
	m_fValue(FALSE)
{
	if (ppvValuePtr != NULL)
	{
		//  6/29/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		this->m_fValue = (DWORD_PTR) (*((UNALIGNED PVOID*) ppvValuePtr));
	} // end if (there's a value pointer)
} // CHandledParamBool::CHandledParamBool 
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamBool::GetSelfAndValues()"
//==================================================================================
// CHandledParamBool::GetSelfAndValues
//----------------------------------------------------------------------------------
//
// Description: Searches the list of items for this object and pertinent values.  If
//				found, this object's values are updated and the items used are
//				pulled from the passed in list.
//				CMDLINE_OK is still returned even if this object wasn't found in
//				the list.
//
// Arguments:
//	BOOL fCaseSensitive				Whether the searches should be case sensitive.
//	PSPECIFIEDPARAMSLIST pItemList	List of remaining command line items.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamBool::GetSelfAndValues(BOOL fCaseSensitive,
											PSPECIFIEDPARAMSLIST pItemList)
{
	HRESULT				hr;
	PSPECIFIEDPARAM		pString = NULL;
	PSPECIFIEDPARAM		pNextString = NULL;
	BOOL				fMatch;


	do
	{
		pString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pString == NULL)
			break;


		// If this item was already used, skip it.
		if (pString->GetString() == NULL)
			continue;

		// If this item isn't a switch, skip it.
		if (! pString->m_fSwitch)
			continue;


		// Check to see if we've matched the name
		if (fCaseSensitive)
			fMatch = (strcmp(pString->GetString(), this->GetString()) == 0) ? TRUE : FALSE;
		else
			fMatch = StringCmpNoCase(pString->GetString(), this->GetString());

		// If we didn't match the name, just move on
		if (! fMatch)
			continue;

		pNextString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);

		if ((pNextString == NULL) || (! pNextString->m_fSeparated))
		{
			// If the value wasn't specified, the new value is just opposite of
			// the current value.
			this->m_fValue = (! this->m_fValue);
		} // end if (value wasn't specified
		else
		{
			// If the next item was already used it can't be a value for us, and we
			// can't use this switch.
			if (pNextString->GetString() == NULL)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "WARNING: Next item (%x) already used but it's marked separated!?",
					1, pNextString);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				continue;
			} // end if (the item was already used)

			// Set the value to the one specified.
			this->m_fValue = StringIsTrue(pNextString->GetString());
		} // end else (the value was specified)


		// Mark this as found and update the user's value pointer
		this->m_fFound = TRUE;
		if (this->m_ppvValuePtr != NULL)
		{
			//  6/29/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
			*((UNALIGNED DWORD_PTR*) this->m_ppvValuePtr) = this->m_fValue;
		} // end if (there's a value pointer)


		// Mark the command line item as used.
		hr = pString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Stop searching.
		break;
	} // end do (while haven't looked at all the remaining command line items)
	while (TRUE);

	return (CMDLINE_OK);
} // CHandledParamBool::GetSelfAndValues
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no file printing
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamBool::PrintToFile()"
//==================================================================================
// CHandledParamBool::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the textual representation of this item to the specified open
//				file.
//
// Arguments:
//	HANDLE hFile	Handle of open file to write to.
//	BOOL fVerbose	Whether more description can be printed, if necessary.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamBool::PrintToFile(HANDLE hFile, BOOL fVerbose)
{
	HRESULT		hr;


	hr = FileSprintfWriteLine(hFile, "%s%s=%B",
							3, ((this->m_fFound) ? "": "//"),
							this->GetString(),
							this->m_fValue);

	return (hr);
} // CHandledParamBool::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#else // ! XBOX
#pragma TODO(tristanj, "Print error when unhandled command line parameter encountered?")
#endif // XBOX




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamInt::CHandledParamInt()"
//==================================================================================
// CHandledParamInt constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CHandledParamInt object.  Initializes the name and value
//				to the ones passed in.
//
// Arguments:
//	char* szName			Name of this item.
//	char* pszDescription	Pointer to optional description of this item
//	DWORD dwOptions			Options for this item.
//	PVOID* ppvValuePtr		Pointer to default value and place to update with value.
//
// Returns: None (the object).
//==================================================================================
CHandledParamInt::CHandledParamInt(char* szName, char* pszDescription,
									DWORD dwOptions, PVOID* ppvValuePtr):
	CHandledParam(szName, pszDescription, CMDLINE_PARAMTYPE_INT, dwOptions,
					ppvValuePtr),
	m_iValue(0)
{
	if (ppvValuePtr != NULL)
	{
		//  6/29/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		this->m_iValue = (INT_PTR) (*((UNALIGNED PVOID*) ppvValuePtr));
	} // end if (there's a value pointer)
} // CHandledParamInt::CHandledParamInt
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamInt::GetSelfAndValues()"
//==================================================================================
// CHandledParamInt::GetSelfAndValues
//----------------------------------------------------------------------------------
//
// Description: Searches the list of items for this object and pertinent values.  If
//				found, this object's values are updated and the items used are
//				pulled from the passed in list.
//				CMDLINE_OK is still returned even if this object wasn't found in
//				the list.
//
// Arguments:
//	BOOL fCaseSensitive			Whether the searches should be case sensitive.
//	PSPECIFIEDPARAM pItemList	List of remaining command line items.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamInt::GetSelfAndValues(BOOL fCaseSensitive,
											PSPECIFIEDPARAMSLIST pItemList)
{
	HRESULT				hr;
	PSPECIFIEDPARAM		pString = NULL;
	BOOL				fMatch;
	PSPECIFIEDPARAM		pNextString;


	do
	{
		pString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pString == NULL)
			break;


		// If this item was already used, skip it.
		if (pString->GetString() == NULL)
			continue;

		// If this item isn't a switch, skip it.
		if (! pString->m_fSwitch)
			continue;


		// Check to see if we've matched the name
		if (fCaseSensitive)
			fMatch = (strcmp(pString->GetString(), this->GetString()) == 0) ? TRUE : FALSE;
		else
			fMatch = StringCmpNoCase(pString->GetString(), this->GetString());


		// If we didn't match the name, just move on
		if (! fMatch)
			continue;


		pNextString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pNextString == NULL)
			break;


		// If the next item was already used it can't be a value for us, and we
		// can't use this switch.
		if (pNextString->GetString() == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			if (pNextString->m_fSeparated)
			{
				DPL(0, "WARNING: Next item (%x) already used but it's marked separated!?",
					1, pNextString);
			} // end if (was marked as separated)
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			continue;
		} // end if (the item was already used)


		// We want to handle values separated by only a space, but that can lead
		// to issues.  Alert the user, in case he's trying to figure out why his
		// app is crashing because of wacky input.
		if (! pNextString->m_fSeparated)
		{
			DPL(0, "NOTE: Using next item after \"%s\" that was separated by a space (\"%s\").",
				2, pString->GetString(), pNextString->GetString());
		} // end if (didn't separate)



		// Otherwise, assume it's a valid integer.
		this->m_iValue = StringToInt(pNextString->GetString());

		// Mark this value command line item as used.
		hr = pNextString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear value string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Mark this as found and update the user's value pointer
		this->m_fFound = TRUE;
		if (this->m_ppvValuePtr != NULL)
			*((INT_PTR*) this->m_ppvValuePtr) = this->m_iValue;


		// Mark the command line item as used.
		hr = pString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Stop searching.
		break;
	} // end do (while haven't looked at all the remaining command line items)
	while (TRUE);

	return (CMDLINE_OK);
} // CHandledParamInt::GetSelfAndValues
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no file printing
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamInt::PrintToFile()"
//==================================================================================
// CHandledParamInt::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the textual representation of this item to the specified open
//				file.
//
// Arguments:
//	HANDLE hFile	Handle of open file to write to.
//	BOOL fVerbose	Whether more description can be printed, if necessary.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamInt::PrintToFile(HANDLE hFile, BOOL fVerbose)
{
	HRESULT		hr;


	hr = FileSprintfWriteLine(hFile, "%s%s=%i",
							3, ((this->m_fFound) ? "": "//"),
							this->GetString(),
							this->m_iValue);

	return (hr);
} // CHandledParamInt::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#else // ! XBOX
#pragma TODO(tristanj, "Print error when unhandled command line parameter encountered?")
#endif // XBOX




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamDWord::CHandledParamDWord()"
//==================================================================================
// CHandledParamDWord constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CHandledParamDWord object.  Initializes the name and
//				value to the ones passed in.
//
// Arguments:
//	char* szName			Name of this item.
//	char* pszDescription	Pointer to optional description of this item
//	DWORD dwOptions			Options for this item.
//	PVOID* ppvValuePtr		Pointer to default value and place to update with value.
//
// Returns: None (the object).
//==================================================================================
CHandledParamDWord::CHandledParamDWord(char* szName, char* pszDescription,
										DWORD dwOptions, PVOID* ppvValuePtr):
	CHandledParam(szName, pszDescription, CMDLINE_PARAMTYPE_DWORD, dwOptions,
				ppvValuePtr),
	m_dwValue(0)
{
	if (ppvValuePtr != NULL)
	{
		//  6/29/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		this->m_dwValue = (DWORD_PTR) (*((UNALIGNED PVOID*) ppvValuePtr));
	} // end if (there's a value pointer)
} // CHandledParamDWord::CHandledParamDWord
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamDWord::GetSelfAndValues()"
//==================================================================================
// CHandledParamDWord::GetSelfAndValues
//----------------------------------------------------------------------------------
//
// Description: Searches the list of items for this object and pertinent values.  If
//				found, this object's values are updated and the items used are
//				pulled from the passed in list.
//				CMDLINE_OK is still returned even if this object wasn't found in
//				the list.
//
// Arguments:
//	BOOL fCaseSensitive				Whether the searches should be case sensitive.
//	PSPECIFIEDPARAMSLIST pItemList	List of remaining command line items.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamDWord::GetSelfAndValues(BOOL fCaseSensitive,
											PSPECIFIEDPARAMSLIST pItemList)
{
	HRESULT				hr;
	PSPECIFIEDPARAM		pString = NULL;
	BOOL				fMatch;
	PSPECIFIEDPARAM		pNextString;


	do
	{
		pString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pString == NULL)
			break;


		// If this item was already used, skip it.
		if (pString->GetString() == NULL)
			continue;

		// If this item isn't a switch, skip it.
		if (! pString->m_fSwitch)
			continue;



		// Check to see if we've matched the name
		if (fCaseSensitive)
			fMatch = (strcmp(pString->GetString(), this->GetString()) == 0) ? TRUE : FALSE;
		else
			fMatch = StringCmpNoCase(pString->GetString(), this->GetString());

		// If we didn't match the name, just move on
		if (! fMatch)
			continue;


		pNextString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pNextString == NULL)
			break;


		// If the next item was already used it can't be a value for us, and we
		// can't use this switch.
		if (pNextString->GetString() == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			if (pNextString->m_fSeparated)
			{
				DPL(0, "WARNING: Next item (%x) already used but it's marked separated!?",
					1, pNextString);
			} // end if (was marked as separated)
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			continue;
		} // end if (the item was already used)


		// We want to handle values separated by only a space, but that can lead
		// to issues.  Alert the user, in case he's trying to figure out why his
		// app is crashing because of wacky input.
		if (! pNextString->m_fSeparated)
		{
			DPL(0, "NOTE: Using next item after \"%s\" that was separated by a space (\"%s\").",
				2, pString->GetString(), pNextString->GetString());
		} // end if (didn't separate)


		// Otherwise, assume it's a valid DWORD.
		this->m_dwValue = StringToDWord(pNextString->GetString());

		// Mark this value command line item as used.
		hr = pNextString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear value string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Mark this as found and update the user's value pointer
		this->m_fFound = TRUE;
		if (this->m_ppvValuePtr != NULL)
			*((DWORD_PTR*) this->m_ppvValuePtr) = this->m_dwValue;


		// Mark the command line item as used.
		hr = pString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Stop searching.
		break;
	} // end do (while haven't looked at all the remaining command line items)
	while (TRUE);

	return (CMDLINE_OK);
} // CHandledParamDWord::GetSelfAndValues
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no file printing
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamDWord::PrintToFile()"
//==================================================================================
// CHandledParamDWord::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the textual representation of this item to the specified open
//				file.
//
// Arguments:
//	HANDLE hFile	Handle of open file to write to.
//	BOOL fVerbose	Whether more description can be printed, if necessary.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamDWord::PrintToFile(HANDLE hFile, BOOL fVerbose)
{
	HRESULT		hr;


	hr = FileSprintfWriteLine(hFile, "%s%s=%u",
							3, ((this->m_fFound) ? "": "//"),
							this->GetString(),
							this->m_dwValue);

	return (hr);
} // CHandledParamDWord::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#else // ! XBOX
#pragma TODO(tristanj, "Print error when unhandled command line parameter encountered?")
#endif // XBOX




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamDWordHex::CHandledParamDWordHex()"
//==================================================================================
// CHandledParamDWordHex constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CHandledParamDWordHex object.  Initializes the name and
//				value to the ones passed in.
//
// Arguments:
//	char* szName			Name of this item.
//	char* pszDescription	Pointer to optional description of this item
//	DWORD dwOptions			Options for this item.
//	PVOID* ppvValuePtr		Pointer to default value and place to update with value.
//
// Returns: None (the object).
//==================================================================================
CHandledParamDWordHex::CHandledParamDWordHex(char* szName, char* pszDescription,
										DWORD dwOptions, PVOID* ppvValuePtr):
	CHandledParam(szName, pszDescription, CMDLINE_PARAMTYPE_DWORD, dwOptions,
				ppvValuePtr),
	m_dwValue(0)
{
	if (ppvValuePtr != NULL)
	{
		//  6/29/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		this->m_dwValue = (DWORD_PTR) (*((UNALIGNED PVOID*) ppvValuePtr));
	} // end if (there's a value pointer)
} // CHandledParamDWordHex::CHandledParamDWordHex
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamDWordHex::GetSelfAndValues()"
//==================================================================================
// CHandledParamDWordHex::GetSelfAndValues
//----------------------------------------------------------------------------------
//
// Description: Searches the list of items for this object and pertinent values.  If
//				found, this object's values are updated and the items used are
//				pulled from the passed in list.
//				CMDLINE_OK is still returned even if this object wasn't found in
//				the list.
//
// Arguments:
//	BOOL fCaseSensitive				Whether the searches should be case sensitive.
//	PSPECIFIEDPARAMSLIST pItemList	List of remaining command line items.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamDWordHex::GetSelfAndValues(BOOL fCaseSensitive,
												PSPECIFIEDPARAMSLIST pItemList)
{
	HRESULT				hr;
	PSPECIFIEDPARAM		pString = NULL;
	BOOL				fMatch;
	PSPECIFIEDPARAM		pNextString;


	do
	{
		pString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pString == NULL)
			break;


		// If this item was already used, skip it.
		if (pString->GetString() == NULL)
			continue;

		// If this item isn't a switch, skip it.
		if (! pString->m_fSwitch)
			continue;



		// Check to see if we've matched the name
		if (fCaseSensitive)
			fMatch = (strcmp(pString->GetString(), this->GetString()) == 0) ? TRUE : FALSE;
		else
			fMatch = StringCmpNoCase(pString->GetString(), this->GetString());

		// If we didn't match the name, just move on
		if (! fMatch)
			continue;


		pNextString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pNextString == NULL)
			break;


		// If the next item was already used it can't be a value for us, and we
		// can't use this switch.
		if (pNextString->GetString() == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			if (pNextString->m_fSeparated)
			{
				DPL(0, "WARNING: Next item (%x) already used but it's marked separated!?",
					1, pNextString);
			} // end if (was marked as separated)
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			continue;
		} // end if (the item was already used)


		// We want to handle values separated by only a space, but that can lead
		// to issues.  Alert the user, in case he's trying to figure out why his
		// app is crashing because of wacky input.
		if (! pNextString->m_fSeparated)
		{
			DPL(0, "NOTE: Using next item after \"%s\" that was separated by a space (\"%s\").",
				2, pString->GetString(), pNextString->GetString());
		} // end if (didn't separate)


		// Otherwise, assume it's a valid hex DWORD.
		this->m_dwValue = StringHexToDWord(pNextString->GetString());

		// Mark this value command line item as used.
		hr = pNextString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear value string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Mark this as found and update the user's value pointer
		this->m_fFound = TRUE;
		if (this->m_ppvValuePtr != NULL)
			*((DWORD_PTR*) this->m_ppvValuePtr) = this->m_dwValue;


		// Mark the command line item as used.
		hr = pString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Stop searching.
		break;
	} // end do (while haven't looked at all the remaining command line items)
	while (TRUE);

	return (CMDLINE_OK);
} // CHandledParamDWordHex::GetSelfAndValues
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // No file printing
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamDWordHex::PrintToFile()"
//==================================================================================
// CHandledParamDWordHex::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the textual representation of this item to the specified open
//				file.
//
// Arguments:
//	HANDLE hFile	Handle of open file to write to.
//	BOOL fVerbose	Whether more description can be printed, if necessary.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamDWordHex::PrintToFile(HANDLE hFile, BOOL fVerbose)
{
	HRESULT		hr;


	hr = FileSprintfWriteLine(hFile, "%s%s=%x",
							3, ((this->m_fFound) ? "": "//"),
							this->GetString(),
							this->m_dwValue);

	return (hr);
} // CHandledParamDWordHex::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#else // ! XBOX
#pragma TODO(tristanj, "Print error when unhandled command line parameter encountered?")
#endif // XBOX






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamString::CHandledParamString()"
//==================================================================================
// CHandledParamString constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CHandledParamString object.  Initializes the name and
//				value to the ones passed in.
//
// Arguments:
//	char* szName			Name of this item.
//	char* pszDescription	Pointer to optional description of this item
//	DWORD dwOptions			Options for this item.
//	PVOID* ppvValuePtr		Pointer to default value and place to update with value.
//
// Returns: None (the object).
//==================================================================================
CHandledParamString::CHandledParamString(char* szName, char* pszDescription,
										DWORD dwOptions, PVOID* ppvValuePtr):
	CHandledParam(szName, pszDescription, CMDLINE_PARAMTYPE_STRING, dwOptions,
					ppvValuePtr),
	m_pszValue(NULL)
{
	//  6/29/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
	if ((ppvValuePtr != NULL) && ((*((UNALIGNED PVOID*) ppvValuePtr)) != NULL))
	{
		this->m_pszValue = (char*) LocalAlloc(LPTR, (strlen((char*) (*((UNALIGNED PVOID*) ppvValuePtr))) + 1));
		if (this->m_pszValue != NULL)
		{
			strcpy(this->m_pszValue, ((char*) (*((UNALIGNED PVOID*) ppvValuePtr))));
		} // end if (allocated memory)
	} // end if (there's a default value)
} // CHandledParamString::CHandledParamString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamString::~CHandledParamString()"
//==================================================================================
// CHandledParamString destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CHandledParamString object and any resources it may have
//				allocated.
//
// Arguments: None.
//
// Returns: None
//==================================================================================
CHandledParamString::~CHandledParamString(void)
{
	if (this->m_pszValue != NULL)
	{
		LocalFree(this->m_pszValue);
		this->m_pszValue = NULL;
	} // end if (we allocated a description)
} // CHandledParamString::CHandledParamString 
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamString::GetSelfAndValues()"
//==================================================================================
// CHandledParamString::GetSelfAndValues
//----------------------------------------------------------------------------------
//
// Description: Searches the list of items for this object and pertinent values.  If
//				found, this object's values are updated and the items used are
//				pulled from the passed in list.
//				CMDLINE_OK is still returned even if this object wasn't found in
//				the list.
//
// Arguments:
//	BOOL fCaseSensitive				Whether the searches should be case sensitive.
//	PSPECIFIEDPARAMSLIST pItemList	List of remaining command line items.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamString::GetSelfAndValues(BOOL fCaseSensitive,
											PSPECIFIEDPARAMSLIST pItemList)
{
	HRESULT				hr;
	PSPECIFIEDPARAM		pString = NULL;
	BOOL				fMatch;
	PSPECIFIEDPARAM		pNextString;


	do
	{
		pString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pString == NULL)
			break;


		// If this item was already used, skip it.
		if (pString->GetString() == NULL)
			continue;

		// If this item isn't a switch, skip it.
		if (! pString->m_fSwitch)
			continue;


		// Check to see if we've matched the name
		if (fCaseSensitive)
			fMatch = (strcmp(pString->GetString(), this->GetString()) == 0) ? TRUE : FALSE;
		else
			fMatch = StringCmpNoCase(pString->GetString(), this->GetString());


		// If we didn't match the name, just move on
		if (! fMatch)
			continue;


		pNextString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pNextString == NULL)
			break;


		// If the next item was already used it can't be a value for us, and we
		// can't use this switch.
		if (pNextString->GetString() == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			if (pNextString->m_fSeparated)
			{
				DPL(0, "WARNING: Next item (%x) already used but it's marked separated!?",
					1, pNextString);
			} // end if (was marked as separated)
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			continue;
		} // end if (the item was already used)


		// We want to handle values separated by only a space, but that can lead
		// to issues.  Alert the user, in case he's trying to figure out why his
		// app is crashing because of wacky input.
		if (! pNextString->m_fSeparated)
		{
			DPL(0, "NOTE: Using next item after \"%s\" that was separated by a space (\"%s\").",
				2, pString->GetString(), pNextString->GetString());
		} // end if (didn't separate)



		// If we previously had a value, get rid of it
		if (this->m_pszValue != NULL)
		{
			LocalFree(this->m_pszValue);
			this->m_pszValue = NULL;
		} // end if (we had a value previously)

		this->m_pszValue = (char*) LocalAlloc(LPTR, (strlen(pNextString->GetString()) + 1));
		if (this->m_pszValue == NULL)
			return (E_OUTOFMEMORY);

		strcpy(this->m_pszValue, pNextString->GetString());


		// Mark this value command line item as used.
		hr = pNextString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear value string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Mark this as found and update the user's value pointer
		this->m_fFound = TRUE;
		if (this->m_ppvValuePtr != NULL)
			*((char**) this->m_ppvValuePtr) = this->m_pszValue;


		// Mark the command line item as used.
		hr = pString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Stop searching.
		break;
	} // end do (while haven't looked at all the remaining command line items)
	while (TRUE);

	return (CMDLINE_OK);
} // CHandledParamString::GetSelfAndValues
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no file printing
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamString::PrintToFile()"
//==================================================================================
// CHandledParamString::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the textual representation of this item to the specified open
//				file.
//
// Arguments:
//	HANDLE hFile	Handle of open file to write to.
//	BOOL fVerbose	Whether more description can be printed, if necessary.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamString::PrintToFile(HANDLE hFile, BOOL fVerbose)
{
	HRESULT		hr;


	if (this->m_pszValue)
	{
		hr = FileSprintfWriteLine(hFile, "%s%s=%s",
								3, ((this->m_fFound) ? "": "//"),
								this->GetString(),
								this->m_pszValue);
	} // end if (there's an item)
	else
	{
		hr = FileSprintfWriteLine(hFile, "%s%s=",
								2, ((this->m_fFound) ? "": "//"),
								this->GetString());
	} // end else (there isn't an item)


	return (hr);
} // CHandledParamString::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#else // ! XBOX
#pragma TODO(tristanj, "Print error when unhandled command line parameter encountered?")
#endif // XBOX


#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamID::CHandledParamID()"
//==================================================================================
// CHandledParamID constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CHandledParamID object.  Initializes the name and value
//				to the ones passed in.
//
// Arguments:
//	char* szName					Name of this item.
//	char* pszDescription			Pointer to optional description of this item
//	DWORD dwOptions					Options for this item.
//	PVOID* ppvValuePtr				Pointer to default value and place to update
//									with value.
//	PCMDLINEIDITEM paLookupTable	Array of items to use when looking up a value.
//	DWORD dwNumTableEntries			Number of entries in the above array.
//
// Returns: None (the object).
//==================================================================================
CHandledParamID::CHandledParamID(char* szName, char* pszDescription, DWORD dwOptions,
								PVOID* ppvValuePtr, PCMDLINEIDITEM paLookupTable,
								DWORD dwNumTableEntries):
	CHandledParam(szName, pszDescription, CMDLINE_PARAMTYPE_ID, dwOptions,
					ppvValuePtr),
	m_paLookupTable(paLookupTable),
	m_dwNumTableEntries(dwNumTableEntries),
	m_dwValue(0)
{
	if (ppvValuePtr != NULL)
	{
		//  6/29/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		this->m_dwValue = (DWORD_PTR) (*((UNALIGNED PVOID*) ppvValuePtr));
	} // end if (there's a value pointer)
} // CHandledParamID::CHandledParamID
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamID::GetSelfAndValues()"
//==================================================================================
// CHandledParamID::GetSelfAndValues
//----------------------------------------------------------------------------------
//
// Description: Searches the list of items for this object and pertinent values.  If
//				found, this object's values are updated and the items used are
//				pulled from the passed in list.
//				CMDLINE_OK is still returned even if this object wasn't found in
//				the list.
//
// Arguments:
//	BOOL fCaseSensitive				Whether the searches should be case sensitive.
//	PSPECIFIEDPARAMSLIST pItemList	List of remaining command line items.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamID::GetSelfAndValues(BOOL fCaseSensitive,
										PSPECIFIEDPARAMSLIST pItemList)
{
	HRESULT				hr;
	PSPECIFIEDPARAM		pString = NULL;
	BOOL				fMatch;
	DWORD				dwTemp;
	PSPECIFIEDPARAM		pNextString;


	do
	{
		pString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pString == NULL)
			break;


		// If this item was already used, skip it.
		if (pString->GetString() == NULL)
			continue;

		// If this item isn't a switch, skip it.
		if (! pString->m_fSwitch)
			continue;


		// Check to see if we've matched the name
		if (fCaseSensitive)
			fMatch = (strcmp(pString->GetString(), this->GetString()) == 0) ? TRUE : FALSE;
		else
			fMatch = StringCmpNoCase(pString->GetString(), this->GetString());

		// If we didn't match the name, just move on
		if (! fMatch)
			continue;


		pNextString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pNextString == NULL)
			break;


		// If the next item was already used it can't be a value for us, and we
		// can't use this switch.
		if (pNextString->GetString() == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			if (pNextString->m_fSeparated)
			{
				DPL(0, "WARNING: Next item (%x) already used but it's marked separated!?",
					1, pNextString);
			} // end if (was marked as separated)
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			continue;
		} // end if (the item was already used)


		// We want to handle values separated by only a space, but that can lead
		// to issues.  Alert the user, in case he's trying to figure out why his
		// app is crashing because of wacky input.
		if (! pNextString->m_fSeparated)
		{
			DPL(0, "NOTE: Using next item after \"%s\" that was separated by a space (\"%s\").",
				2, pString->GetString(), pNextString->GetString());
		} // end if (didn't separate)


		// If we don't find a match, we will return an error, unless the
		// user said we shouldn't, in which case it will just stay the
		// default value.
		fMatch = FALSE;
		for(dwTemp = 0; dwTemp < this->m_dwNumTableEntries; dwTemp++)
		{
			if (fCaseSensitive)
				fMatch = (strcmp(this->m_paLookupTable[dwTemp].pszName, pNextString->GetString()) == 0) ? TRUE : FALSE;
			else
				fMatch = StringCmpNoCase(this->m_paLookupTable[dwTemp].pszName, pNextString->GetString());

			if (fMatch)
			{
				this->m_dwValue = this->m_paLookupTable[dwTemp].dwValue;
				break;
			} // end if (this item matches)
		} // end for (each item in the ID table)

		if ((! fMatch) &&
			(! (this->m_dwOptions & CMDLINE_PARAMOPTION_DONTFAILIFUNKNOWNID)))
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "\"%s\" is not a recognized value for the switch \"%s\"!",
				2, pNextString->GetString(), this->GetString());
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (CMDLINERR_INVALIDPARAMETERVALUE);
		} // end if (item wasn't valid and we can fail)


		// Mark this value command line item as used.
		hr = pNextString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear value string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Mark this as found and update the user's value pointer
		this->m_fFound = TRUE;
		if (this->m_ppvValuePtr != NULL)
			*((DWORD_PTR*) this->m_ppvValuePtr) = this->m_dwValue;



		// Mark the command line item as used.
		hr = pString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Stop searching.
		break;
	} // end do (while haven't looked at all the remaining command line items)
	while (TRUE);

	return (CMDLINE_OK);
} // CHandledParamID::GetSelfAndValues
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no file printing
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamID::PrintToFile()"
//==================================================================================
// CHandledParamID::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the textual representation of this item to the specified open
//				file.
//
// Arguments:
//	HANDLE hFile	Handle of open file to write to.
//	BOOL fVerbose	Whether more description can be printed, if necessary.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamID::PrintToFile(HANDLE hFile, BOOL fVerbose)
{
	HRESULT		hr;
	char*		pszName = NULL;
	DWORD		dwTemp;


	// If we can print extended information, we'll print all the accepted values.
	if (fVerbose)
	{
		FileWriteLine(hFile, "// Accepted values:");
	} // end if (can print descriptions)


	// Look for the current value, and if printing more information, print each
	// value.
	for(dwTemp = 0; dwTemp < this->m_dwNumTableEntries; dwTemp++)
	{
		if ((pszName == NULL) &&
			(this->m_paLookupTable[dwTemp].dwValue == this->m_dwValue))
		{
			pszName = this->m_paLookupTable[dwTemp].pszName;

			// If not printing verbosely, stop.
			if (! fVerbose)
				break;
		} // end if (this item matches)

		if (fVerbose)
		{
			hr = FileSprintfWriteLine(hFile, "//     \"%s\" (%u)",
									2, this->m_paLookupTable[dwTemp].pszName,
									this->m_paLookupTable[dwTemp].dwValue);
		} // end if (can print descriptions)
	} // end for (each item in the ID table)


	/*
	FileWriteString(hFile, "//");
	// - 2 for "//" and -1 for the second character each iteration
	for(dwTemp = 0; dwTemp < WRAP_DESCRIPTION_CHARACTERS - 3; dwTemp += 2)
		FileWriteString(hFile, " -");
	FileWriteLine(hFile, "");
	*/


	// If we found a name for our value, print it.  Otherwise print the actual
	// value but commented out.
	if (pszName != NULL)
	{
		hr = FileSprintfWriteLine(hFile, "%s%s=%s",
								3, ((this->m_fFound) ? "": "//"),
								this->GetString(),
								pszName);
	} // end if (found a match)
	else
	{
		hr = FileSprintfWriteLine(hFile, "//%s=%u? %s",
									3, this->GetString(),
									this->m_dwValue,
									((this->m_fFound) ? "": "(was not specified)"));
	} // end else (didn't find a matching ID)

	return (hr);
} // CHandledParamID::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#else // ! XBOX
#pragma TODO(tristanj, "Print error when unhandled command line parameter encountered?")
#endif // XBOX


#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamMultiString::CHandledParamMultiString()"
//==================================================================================
// CHandledParamMultiString constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CHandledParamMultiString object.  Initializes the name
//				and value to the ones passed in.
//
// Arguments:
//	char* szName			Name of this item.
//	char* pszDescription	Pointer to optional description of this item
//	DWORD dwOptions			Options for this item.
//	PVOID* ppvValuePtr		Pointer to default value and place to update with value.
//
// Returns: None (the object).
//==================================================================================
CHandledParamMultiString::CHandledParamMultiString(char* szName, char* pszDescription,
													DWORD dwOptions,
													PVOID* ppvValuePtr):
	CHandledParam(szName, pszDescription, CMDLINE_PARAMTYPE_MULTISTRING, dwOptions,
					ppvValuePtr),
	m_pValues(NULL),
	m_fCreatedValuesList(FALSE)
{
	// If there was an existing item.
	//  6/29/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
	if ((ppvValuePtr != NULL) && ((*((UNALIGNED PVOID*) ppvValuePtr)) != NULL))
	{
		this->m_pValues = (PLSTRINGLIST) (*((UNALIGNED PVOID*) ppvValuePtr));
	} // end if (there's a default value)
	else
	{
		this->m_pValues = new (CLStringList);
		this->m_fCreatedValuesList = TRUE;
	} // end if (there's a default value)
} // CHandledParamMultiString::CHandledParamMultiString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamMultiString::~CHandledParamMultiString()"
//==================================================================================
// CHandledParamMultiString destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CHandledParamMultiString object and any resources it
//				may have allocated.
//
// Arguments: None.
//
// Returns: None
//==================================================================================
CHandledParamMultiString::~CHandledParamMultiString(void)
{
	if ((this->m_pValues != NULL) && (this->m_fCreatedValuesList))
	{
		delete (this->m_pValues);
		this->m_pValues = NULL;
	} // end if (we allocated a description)
} // CHandledParamMultiString::CHandledParamMultiString 
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamMultiString::GetSelfAndValues()"
//==================================================================================
// CHandledParamMultiString::GetSelfAndValues
//----------------------------------------------------------------------------------
//
// Description: Searches the list of items for this object and pertinent values.  If
//				found, this object's values are updated and the items used are
//				pulled from the passed in list.
//				CMDLINE_OK is still returned even if this object wasn't found in
//				the list.
//
// Arguments:
//	BOOL fCaseSensitive				Whether the searches should be case sensitive.
//	PSPECIFIEDPARAMSLIST pItemList	List of remaining command line items.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamMultiString::GetSelfAndValues(BOOL fCaseSensitive,
													PSPECIFIEDPARAMSLIST pItemList)
{
	HRESULT				hr;
	PSPECIFIEDPARAM		pString = NULL;
	BOOL				fMatch;
	PSPECIFIEDPARAM		pNextString;


	do
	{
		pString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pString == NULL)
			break;


		// If this item was already used, skip it.
		if (pString->GetString() == NULL)
			continue;

		// If this item isn't a switch, skip it.
		if (! pString->m_fSwitch)
			continue;


		// Check to see if we've matched the name
		if (fCaseSensitive)
			fMatch = (strcmp(pString->GetString(), this->GetString()) == 0) ? TRUE : FALSE;
		else
			fMatch = StringCmpNoCase(pString->GetString(), this->GetString());


		// If we didn't match the name, just move on
		if (! fMatch)
			continue;


		pNextString = (PSPECIFIEDPARAM) pItemList->GetNextItem(pString);
		if (pNextString == NULL)
			break;


		// If the next item was already used it can't be a value for us, and we
		// can't use this switch.
		if (pNextString->GetString() == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			if (pNextString->m_fSeparated)
			{
				DPL(0, "WARNING: Next item (%x) already used but it's marked separated!?",
					1, pNextString);
			} // end if (was marked as separated)
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			continue;
		} // end if (the item was already used)


		// We want to handle values separated by only a space, but that can lead
		// to issues.  Alert the user, in case he's trying to figure out why his
		// app is crashing because of wacky input.
		if (! pNextString->m_fSeparated)
		{
			DPL(0, "NOTE: Using next item after \"%s\" that was separated by a space (\"%s\").",
				2, pString->GetString(), pNextString->GetString());
		} // end if (didn't separate)


		hr = this->m_pValues->AddString(pNextString->GetString());
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't add value string \"%s\"!", 1, pNextString->GetString());
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't add string)


		// Mark this value command line item as used.
		hr = pNextString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear value string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Mark this as found and update the user's value pointer
		this->m_fFound = TRUE;
		if (this->m_ppvValuePtr != NULL)
			*((PLSTRINGLIST*) this->m_ppvValuePtr) = this->m_pValues;


		// Mark the command line item as used.
		hr = pString->SetString(NULL);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't clear string value!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (hr);
		} // end if (couldn't set string)


		// Note that we continue searching...
	} // end do (while haven't looked at all the remaining command line items)
	while (TRUE);

	return (CMDLINE_OK);
} // CHandledParamMultiString::GetSelfAndValues
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no file printing
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CHandledParamMultiString::PrintToFile()"
//==================================================================================
// CHandledParamMultiString::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the textual representation of this item to the specified open
//				file.
//
// Arguments:
//	HANDLE hFile	Handle of open file to write to.
//	BOOL fVerbose	Whether more description can be printed, if necessary.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CHandledParamMultiString::PrintToFile(HANDLE hFile, BOOL fVerbose)
{
	HRESULT		hr = CMDLINE_OK;
	BOOL		fPrinted = FALSE;
	int			i;
	PLSTRING	pString = NULL;



	if (this->m_pValues != NULL)
	{
		for(i = 0; i < this->m_pValues->Count(); i++)
		{
			pString = (PLSTRING) this->m_pValues->GetItem(i);
			if (pString == NULL)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't get string value %i!", 1, i);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				hr = E_FAIL;
				break;
			} // end if (couldn't get item)

			hr = FileSprintfWriteLine(hFile, "%s%s=%s",
									3, ((this->m_fFound) ? "": "//"),
									this->GetString(),
									pString->GetString());
			fPrinted = TRUE;
		} // end for (each value)
	} // end if (there are values)

	if (! fPrinted)
	{
		hr = FileSprintfWriteLine(hFile, "//%s has no value%s",
								2, this->GetString(),
								((this->m_fFound) ? "s but was specified": "s"));
	} // end if (haven't printed anything yet)

	return (hr);
} // CHandledParamMultiString::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#else // ! XBOX
#pragma TODO(tristanj, "Print error when unhandled command line parameter encountered?")
#endif // XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CSpecifiedParam::CSpecifiedParam()"
//==================================================================================
// CSpecifiedParam constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CSpecifiedParam object.  Initializes the string and
//				boolean to the ones passed in.
//
// Arguments:
//	char* szName		Name of this item.
//	BOOL fSwitch		Is this item a switch?
//	BOOL fSeparated		Was this item separated from its switch name?
//
// Returns: None (the object).
//==================================================================================
CSpecifiedParam::CSpecifiedParam(char* szName, BOOL fSwitch, BOOL fSeparated):
	CLString(szName),
	m_fSwitch(fSwitch),
	m_fSeparated(fSeparated)
{
} // CSpecifiedParam::CSpecifiedParam
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CSpecifiedParamsList::AddItem()"
//==================================================================================
// CSpecifiedParamsList::AddItem
//----------------------------------------------------------------------------------
//
// Description: Adds an item to the list.  If it's a switch that can be split, the
//				switch and value are added separately.
//
// Arguments:
//	char* szString		String to add.
//	BOOL fSwitch		Whether this item is a switch.
//	BOOL fTryToSplit	If it's a switch, whether this item may contain a value as
//						well.
//	BOOL fWasSplit		If we shouldn't try to split the item, whether this item was
//						split already.
//
// Returns: CMDLINE_OK if successful, error code otherwise.
//==================================================================================
HRESULT CSpecifiedParamsList::AddItem(char* szString, BOOL fSwitch, BOOL fTryToSplit,
									BOOL fWasSplit)
{
	HRESULT				hr;
	unsigned int		ui;
	int					iPos = -1;
	PSPECIFIEDPARAM		pNewItem = NULL;
	char*				pszTemp = NULL;


	if (fSwitch && fTryToSplit)
	{
		// Loop through all the value tokens looking for the first instance of one
		// in the string.
		for(ui = 0; ui < strlen(CMDLINE_VALUE_TOKENS); ui++)
		{
			iPos = StringContainsChar(szString, CMDLINE_VALUE_TOKENS[ui], TRUE, 0);
			if (iPos >= 0)
				break;
		} // end for (each cmdline value token)
	} // end if (we should try to split the item)


	if (iPos >= 0)
	{
		// Grab the first part of the string (the switch)
		pszTemp = (char*) LocalAlloc(LPTR, iPos + 1);
		if (pszTemp == NULL)
			return (E_OUTOFMEMORY);

		CopyMemory(pszTemp, szString, iPos);


		// Make a new item

		pNewItem = new (CSpecifiedParam)(pszTemp, TRUE, FALSE);

		LocalFree(pszTemp);
		pszTemp = NULL;

		if (pNewItem == NULL)
		{
			return (E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		
		// Add the item to the list

		hr = this->Add(pNewItem);
		if (hr != S_OK)
		{
			delete (pNewItem);
			pNewItem = NULL;
			return (hr);
		} // end if (couldn't add object)


		// Grab the second part of the string (the value)
		pszTemp = (char*) LocalAlloc(LPTR, strlen(szString) - iPos);
		if (pszTemp == NULL)
			return (E_OUTOFMEMORY);

		CopyMemory(pszTemp, szString + iPos + 1, strlen(szString) - iPos - 1);


		// Make another new item

		pNewItem = new (CSpecifiedParam)(pszTemp, FALSE, TRUE);

		LocalFree(pszTemp);
		pszTemp = NULL;

		if (pNewItem == NULL)
		{
			return (E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		
		// Add that item to the list

		hr = this->Add(pNewItem);
		if (hr != S_OK)
		{
			delete (pNewItem);
			pNewItem = NULL;
			//return (hr);
		} // end if (couldn't add object)
	} // end if (we should and can split the item)
	else
	{
		pNewItem = new (CSpecifiedParam)(szString, fSwitch, fWasSplit);
		if (pNewItem == NULL)
		{
			return (E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = this->Add(pNewItem);
		if (hr != S_OK)
		{
			delete (pNewItem);
			pNewItem = NULL;
			//return (hr);
		} // end if (couldn't add object)
	} // end else (we shouldn't or couldn't split the item)

	return (hr);
} // CSpecifiedParamsList::AddItem 
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CmdlineReadFileCB()"
//==================================================================================
// CmdlineReadFileCB
//----------------------------------------------------------------------------------
//
// Description: Callback that handles a line read from a text file.
//
// Arguments:
//	char* szLine			Line that was read.
//	PFILELINEINFO pInfo		Information on line that was read.
//	PVOID pvContext			Pointer to context.  Cast as a CMDLINEREADFILECONTEXT
//							pointer.
//	BOOL* pfStopReading		Pointer to boolean to set to TRUE to discontinue reading
//							from the file.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CmdlineReadFileCB(char* szLine, PFILELINEINFO pInfo, PVOID pvContext,
						BOOL* pfStopReading)
{
	HRESULT						hr = S_OK;
	PCMDLINEREADFILECONTEXT		pContext = (PCMDLINEREADFILECONTEXT) pvContext;
	char*						pszKey = NULL;
	char*						pszValue;


	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "Reading line %u:\"%s\".", 2, lpInfo->dwLineNumber, szLine);
	#endif // NO_TNCOMMON_DEBUG_SPEW
	*/

	switch (pInfo->dwType)
	{
		case FILERTFT_SECTION:
			if (StringCmpNoCase(szLine, "CommandLine"))
			{
				pContext->fInCommandLineSection = TRUE;
			} // end if (command line section)
			else
			{
				//pContext->fInCommandLineSection = FALSE;

				// If we were in the command line section, stop parsing
				if (pContext->fInCommandLineSection)
					(*pfStopReading) = TRUE;
			} // end else (not in command line section)
		  break;

		default:
			// If we're not in the command line section, just skip it.
			if (! pContext->fInCommandLineSection)
				break;

			pszKey = (char*) LocalAlloc(LPTR, strlen(szLine) + 1);
			if (pszKey == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			StringGetKey(szLine, pszKey);

			// If we didn't find a key (no '=' character in the line), just add
			// the line directly and we're done.  Otherwise, add the key and
			// value separately.
			if (strcmp(pszKey, "") == 0)
			{
				hr = pContext->pItemList->AddItem(szLine, TRUE, FALSE, FALSE);
				if (hr != S_OK)
				{
					#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(0, "Couldn't add string \"%s\" to list!", 1, szLine);
					#endif // ! NO_TNCOMMON_DEBUG_SPEW
					goto DONE;
				} // end if (couldn't allocate memory)
			} // end if (we didn't find a key)
			else
			{
				// Specifying an ini file to read inside an ini file being read
				// doesn't make a whole lotta sense, so we'll skip it.
				if (StringCmpNoCase(pszKey, "ini"))
				{
					#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(1, "Ignoring \"ini\" being specified inside an ini file, line %u:%s",
						2, pInfo->dwLineNumber, szLine);
					#endif // ! NO_TNCOMMON_DEBUG_SPEW
				} // end if (ini key)
				else
				{
					hr = pContext->pItemList->AddItem(pszKey, TRUE, FALSE, FALSE);
					if (hr != S_OK)
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't add key \"%s\" to list!", 1, pszKey);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
						goto DONE;
					} // end if (couldn't allocate memory)

					pszValue = StringGetKeyValue(szLine);
					hr = pContext->pItemList->AddItem(pszValue, FALSE, FALSE, TRUE);
					if (hr != NULL)
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't add value \"%s\" to list!", 1, pszValue);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
						goto DONE;
					} // end if (couldn't allocate memory)
				} // end else (not ini key)
			} // end else (there's a key)
		  break;
	} // end switch (on line type)


DONE:

	if (pszKey != NULL)
	{
		LocalFree(pszKey);
		pszKey = NULL;
	} // end if (allocated a key)

	return (hr);
} // CmdlineReadFileCB 
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no window logging
#ifndef CMDLINE_CONSOLE_ONLY

#undef DEBUG_SECTION
#define DEBUG_SECTION	"HelpBoxWndProc()"
//==================================================================================
// HelpBoxWndProc
//----------------------------------------------------------------------------------
//
// Description: Message pump that handles the command line help window.
//
// Arguments:
//	HWND hWnd		Window handle.
//	UINT uMsg		Message identifier.
//	WPARAM wParam	Depends on message.
//	LPARAM lParam	Depends on message.
//
// Returns: Depends on message.
//==================================================================================
INT_PTR CALLBACK HelpBoxWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HRESULT			hr;
	PCOMMANDLINE	pThisObj;


	switch (uMsg)
	{
		/*
		case WM_INITDIALOG:
		  break;
		*/

		case WM_CLOSE:
			PostQuitMessage(0);
		  break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(0, "User pressed 'OK'.", 0);
					#endif // ! NO_TNCOMMON_DEBUG_SPEW

					PostMessage(hWnd, WM_CLOSE, 0, 0);
					return (0);
				  break;

				case IDB_NEXT:
					// Retrieve the CCommandLine object pointer
					//  6/29/2000(RichGr) - IA64: Change GetWindowLong() to GetWindowLongPtr() as we are retrieving a pointer.
					pThisObj = (PCOMMANDLINE) GetWindowLongPtr(hWnd, 0);

					// Display the next page of help items.
					hr = pThisObj->DisplayHelpPageInternal(&(pThisObj->m_dwHelpStartItem),
															hWnd);
					if (hr != S_OK)
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't display first help page!", 0);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
					} // end if (displaying first help page failed)
				  break;

				case IDB_BACKTOFIRST:
					// Retrieve the CCommandLine object pointer
					pThisObj = (PCOMMANDLINE) GetWindowLongPtr(hWnd, 0);

					// Display the first page of help items.
					pThisObj->m_dwHelpStartItem = 0;
					hr = pThisObj->DisplayHelpPageInternal(&(pThisObj->m_dwHelpStartItem),
															hWnd);
					if (hr != S_OK)
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't display first help page!", 0);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
					} // end if (displaying first help page failed)
				  break;
			} // end switch (on the button pressed/control changed)
		  break;
	} // end switch (on the type of window message)

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#endif // ! CMDLINE_CONSOLE_ONLY
#endif // ! XBOX