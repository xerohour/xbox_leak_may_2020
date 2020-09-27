#ifndef __TNSHELL_MAIN__
#define __TNSHELL_MAIN__
//#pragma message("Defining __TNSHELL_MAIN__")






//==================================================================================
// Defines
//==================================================================================
#define MAX_STRING		256

#undef DEBUG_MODULE
#define DEBUG_MODULE	"TNSHELL"


/*
//BUGBUG make it so UI doesn't go away until threads are actually gone
#define TIMEOUT_DIE_WORKERTHREAD		25000 // in milliseconds
*/
// This means that everything the worker thread is waiting on has to have timeouts/
// DEBUGBREAKs of their own...
#define TIMEOUT_DIE_WORKERTHREAD		INFINITE



#define SHELLERR_USERCANCEL				0x88770118 // DPERR_USERCANCEL

#define LAST_COMMANDLINE_FILE			"last.ini"




//==================================================================================
// Structures
//==================================================================================
typedef struct tagMODULEDATA
{
	char*					pszModuleName; // pointer to string with module's name (i.e. name of directory and DLL minus extension)
	HMODULE					hModule; // handle to module's instance
	BOOL					fInitialized; // has the module been initialized yet

	TNMODULEINFO			info; // module's self-description

	// Exported functions
	PTNGETMODULEINFOPROC	pfnGetModuleInfo; // Module's TNM_GetModuleInfo()
	PTNINITMODULEPROC		pfnInitModule; // Module's TNM_InitModule()
	PTNRELEASEPROC			pfnRelease; // Module's TNM_Release()

	// Other implemented functions
	TNMODULEFUNCS			funcs; // struct with functions the module implements
} MODULEDATA, * PMODULEDATA;





//==================================================================================
// External Globals
//==================================================================================
extern char*		g_pszTestNetRootPath;
extern PMODULEDATA	g_pModuleData;
extern DWORD		g_dwMode;
extern char*		g_pszSessionFilter;
extern char*		g_pszMastersAddress;
extern char*		g_pszMetaMastersAddress;
extern BOOL			g_fUseReps;
extern DWORD		g_dwReps;
extern BOOL			g_fUseTimelimit;
extern DWORD		g_dwTimelimit;
extern char*		g_pszDocFilepath;





#else //__TNSHELL_MAIN__
//#pragma message("__TNSHELL_MAIN__ already included!")
#endif //__TNSHELL_MAIN__
