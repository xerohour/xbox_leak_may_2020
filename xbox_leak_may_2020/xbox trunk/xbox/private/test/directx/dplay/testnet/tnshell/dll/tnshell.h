#ifndef __TNSHELL__
#define __TNSHELL__
//#pragma message("Defining __TNSHELL__")





//==================================================================================
// Debugging help
//==================================================================================
// You can dump this string from a debugger to get some useful information and
// tips for debugging.
extern char		help[];

// This is the version of the DLL
extern char		g_szTNSHELLVersion[];




//==================================================================================
// Defines
//==================================================================================
#define CURRENT_TNSHELL_API_VERSION				9


//----------------------------------------------------------------------------------
// Control layer requirement flags
//----------------------------------------------------------------------------------
// Specify this to have COM automatically initialized for your module in the test
// thread, and cleaned up on test thread exit.
#define TNCLR_INITIALIZE_COM_FOR_TESTTHREAD		0x1


//----------------------------------------------------------------------------------
// Windows messages posted to poke pages
//----------------------------------------------------------------------------------
// The slave is now in a session.  Generally this is used as a signal to
// activate/show the buttons that were previously disabled.
#define WM_TN_JOINEDSESSION						(WM_USER + 128)






//==================================================================================
// External Structures
//==================================================================================
typedef struct tagTNSHELLINFO
{
	DWORD		dwSize; // size of this structure
	DWORD		dwShellAPIVersion; // the highest version of the TestNet shell API we are implementing
	DWORD		dwControlAPIVersion; // the highest version of the TestNet control layer API we are implementing
} TNSHELLINFO, * PTNSHELLINFO;

typedef struct tagTNMODULEINFO
{
	DWORD		dwSize; // size of this structure
	DWORD		dwShellAPIVersion; // the highest version of the TestNet shell API the module knows about
	DWORD		dwControlAPIVersion; // the highest version of the TestNet control layer API the module knows about
	TNMODULEID	moduleid; // ID of the module, used for identifying sessions
	char*		pszName; // pointer to string to fill in with name of module
	DWORD		dwNameSize; // size of buffer for name string, including zero terminator
	char*		pszDescription; // pointer to string to fill in with description of module
	DWORD		dwDescriptionSize; // size of buffer for description string, including zero terminator
	char*		pszAuthors; // pointer to string to fill in with the comma delimited authors of the module
	DWORD		dwAuthorsSize; // size of buffer for authors string, including zero terminator
} TNMODULEINFO, * PTNMODULEINFO;



//==================================================================================
// More function definitions
//==================================================================================
typedef HRESULT   (__stdcall *PTNPOKEPAGEPROC)(PTNSLAVE, HWND, UINT, WPARAM, LPARAM);



//==================================================================================
// More structures
//==================================================================================
typedef struct tagTNPOKEPAGEINFO
{
	int					iTemplate; // ID of dialog resource for this page
	LPCSTR				pszTitle; // name that should appear in the tab for this page.
	PTNPOKEPAGEPROC		pfnPokePageProc; // callback to use for processing window messages
} TNPOKEPAGEINFO, * PTNPOKEPAGEINFO;




//==================================================================================
// More function definitions
//==================================================================================
typedef HRESULT   (__stdcall *PTNGETPOKEPAGESPROC)(PTNPOKEPAGEINFO, DWORD*);




//==================================================================================
// Still more structures
//==================================================================================
typedef struct tagTNMODULEFUNCS
{
	// This function is required.
	PTNLOADTESTTABLEPROC			pfnLoadTestTable; // Module's TNM_LoadTestTable()

	// Optional module functions
	PTNGETSTARTUPDATAPROC			pfnGetStartupData; // Module's TNM_GetStartupData()
	PTNLOADSTARTUPDATAPROC			pfnLoadStartupData; // Module's TNM_LoadStartupData()
	PTNWRITESTARTUPDATAPROC			pfnWriteStartupData; // Module's TNM_WriteStartupData()
	PTNDOSTARTUPPROC				pfnDoStartup; // Module's TNM_DoStartup()
	PTNINITIALIZETESTINGPROC		pfnInitializeTesting; // Module's TNM_InitializeTesting()
	PTNCLEANUPTESTINGPROC			pfnCleanupTesting; // Module's TNM_CleanupTesting()
	PTNADDIMPORTANTBINARIESPROC		pfnAddImportantBinaries; // Module's TNM_AddImportantBinaries()
	PTNCHANGEBINARYSETPROC			pfnChangeBinarySet; // Module's TNM_ChangeBinarySet()
	PTNGETPOKEPAGESPROC				pfnGetPokePages; // Module's TNM_GetPokePages()
} TNMODULEFUNCS, * PTNMODULEFUNCS;

typedef struct tagTNINITMODULEDATA
{
	// These are informational items for the module.
	DWORD							dwSize; // size of this structure
	BOOL							fMaster; // Should the module be initialize as a master?
	DWORD							dwMode; // the mode the module should initialize in (TNMODE_xxx)
	PCOMMANDLINE					pCmdline; // Command line object so module can see what options the user has specified
	TNSHELLINFO						shellinfo; // information on the shell initializing this module
	char*							pszModuleDirPath; // directory the module resides in; pointer is only valid for duration of function call, module should make a copy of it

	// The following items are set/filled out by the module.
	TNMODULEFUNCS					TNModuleFuncs; // functions the module implements
	DWORD							dwControlLayerFlags; // flags describing the module's control layer requirements, see TNCLR_xxx
} TNINITMODULEDATA, * PTNINITMODULEDATA;





//==================================================================================
// Module exported function definitions
//==================================================================================
typedef HRESULT   (__stdcall *PTNGETMODULEINFOPROC)(PTNSHELLINFO, PTNMODULEINFO);
typedef HRESULT   (__stdcall *PTNINITMODULEPROC)(PTNINITMODULEDATA);
typedef HRESULT   (__stdcall *PTNRELEASEPROC)(void);


VOID WINAPI StartTest(IN HANDLE  hLog);
VOID WINAPI EndTest();


#else //__TNSHELL__
//#pragma message("__TNSHELL__ already included!")
#endif //__TNSHELL__
