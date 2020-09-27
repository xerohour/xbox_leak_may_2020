
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1994-1995 Microsoft Corporation.
*       All rights reserved.
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/


/*++

Module Name:

	plist.cpp

Abstract:

	This module provides APIs for getting task lists and for killing processes
	under Windows NT and Windows 95.

--*/
#include "stdafx.h"

#include <winperf.h>
#include <tlhelp32.h>

#include "plist.h"


//
// manafest constants
//

#define INITIAL_SIZE        51200
#define EXTEND_SIZE         25600
#define REGKEY_PERF         "software\\microsoft\\windows nt\\currentversion\\perflib"
#define REGSUBKEY_COUNTERS  "Counters"
#define PROCESS_COUNTER     "process"
#define PROCESSID_COUNTER   "id process"
#define UNKNOWN_TASK        "unknown"

//
// For dynamically loading TOOLHELP32 -- W95 only
//

#define CreateToolhelp32Snapshot _CreateToolhelp32Snapshot
#define Module32First _Module32First
#define Module32Next _Module32Next
#define Process32First _Process32First
#define Process32Next _Process32Next
#define Thread32First _Thread32First
#define Thread32Next _Thread32Next

typedef BOOL(WINAPI *MODULEWALK)(HANDLE hSnapshot, LPMODULEENTRY32 lpme); 
typedef BOOL (WINAPI *THREADWALK)(HANDLE hSnapshot, LPTHREADENTRY32 lpte); 
typedef BOOL (WINAPI *PROCESSWALK)(HANDLE hSnapshot,LPPROCESSENTRY32 lppe); 
typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD dwFlags,DWORD th32ProcessID); 



CREATESNAPSHOT	CreateToolhelp32Snapshot	= NULL; 
MODULEWALK		Module32First				= NULL; 
MODULEWALK		Module32Next				= NULL; 
PROCESSWALK		Process32First				= NULL; 
PROCESSWALK		Process32Next				= NULL; 
THREADWALK		Thread32First				= NULL; 
THREADWALK		Thread32Next				= NULL; 

//
//	For dynimcally loading psapi.dll -- WinNT only
//

typedef DWORD (WINAPI *GETMODULEFILENAMEEXA) (HANDLE hProcess, HMODULE hModule,
										   LPSTR lpFilename,DWORD nSize);

typedef DWORD (WINAPI *GETMODULEFILENAMEEXW) (HANDLE hProcess, HMODULE hModule,
										   LPWSTR lpFilename,DWORD nSize);

typedef BOOL (WINAPI *ENUMPROCESSES) ( DWORD* rgProcessIds, DWORD  cb, 
		                               DWORD* cbNeeded);


LUID                  SeDebugLuid                 = {0, 0};
GETMODULEFILENAMEEXA	GetModuleFileNameExA = NULL;
GETMODULEFILENAMEEXW	GetModuleFileNameExW = NULL;
HMODULE					hModPsApi			 = NULL;
ENUMPROCESSES           EnumProcesses               = NULL;

//
// prototypes
//

BOOL CALLBACK
EnumWindowsProc(
    HWND    hwnd,
    LPARAM  lParam
    );

DWORD
W95GetTaskList(
	PTASK_LIST	pTask,
	DWORD		dwNumTasks
	);

DWORD
WNtGetTaskList(
    PTASK_LIST  pTask,
    DWORD     dwNumTasks
    );

BOOL
InitPsApi(
	);

void
FreePsApi(
	);

BOOL
InitToolhelp32(
	);

BOOL
W95GetImageInfo(
	PTASK_LIST	Task
	);

BOOL
WNtGetImageInfo(
	PTASK_LIST	Task
	);

BOOL
WINAPI
IsDebugPrivilegeSet(
    );
BOOL
WINAPI
GetImageNameUsingPsapiEx(
    HANDLE               hProcess,		
	HMODULE              hDll,
	DWORD                dwProcessId,
    GETMODULEFILENAMEEXA pfnGetModuleFileNameExA,
	LPTSTR               szImageName,
	DWORD                cbBufferSize
	);

ULONG
GetOsVersion(
	)
{
	BOOL			fSucc;
	OSVERSIONINFO	VersionInfo;

	VersionInfo.dwOSVersionInfoSize = sizeof (VersionInfo);
	
	fSucc = GetVersionEx (&VersionInfo);
	ASSERT (fSucc);
	return VersionInfo.dwPlatformId;
}



BOOL
InitPlistApi(
	)
{
	ULONG	OsVersion = GetOsVersion ();

	if (OsVersion == VER_PLATFORM_WIN32_NT)
	{
		return InitPsApi ();
	}
	else if (OsVersion == VER_PLATFORM_WIN32_WINDOWS)
	{
		return InitToolhelp32 ();
	}
	else
	{
		// neither NT nor W95
		ASSERT (FALSE);
		return FALSE;
	}
}

void
FreePlistApi(
	)
{
	ULONG	OsVersion = GetOsVersion ();

	if (OsVersion == VER_PLATFORM_WIN32_NT)
	{
		FreePsApi ();
	}
	else if (OsVersion == VER_PLATFORM_WIN32_WINDOWS)
	{
		// no freeing necessary under W95
	}
	else
	{
		ASSERT (FALSE);
	}
}


BOOL
InitToolhelp32(
	)
/*++

Routine Description:

	Dynamically load the Win95-only TOOLHELP32 functions.  This is necessary
	so we will run under NT.

	Toolhelp32 does not need to be unloaded, hence there is no FreeToolhelp32
	function.

Return Value:

	True if the TOOLHELP32 functions were successfully loaded and all entry
	points were successfully setup.  False otherwise.

Environment:

	W95 only.
	
--*/
{ 
    BOOL   	bRet  = FALSE; 
    HMODULE hKernel = NULL; 

	if (GetOsVersion () != VER_PLATFORM_WIN32_WINDOWS)
	{
		ASSERT (FALSE);
		return FALSE;
	}
	
    hKernel = GetModuleHandle("KERNEL32.DLL"); 
 
    if (hKernel)
	{ 
        CreateToolhelp32Snapshot =
			(CREATESNAPSHOT) GetProcAddress (hKernel, "CreateToolhelp32Snapshot"); 
 
        Module32First  = (MODULEWALK)GetProcAddress(hKernel, "Module32First"); 
		Module32Next   = (MODULEWALK)GetProcAddress(hKernel, "Module32Next"); 
		Process32First = (PROCESSWALK)GetProcAddress(hKernel,"Process32First");
		Process32Next  = (PROCESSWALK)GetProcAddress(hKernel, "Process32Next");
		Thread32First  = (THREADWALK)GetProcAddress(hKernel, "Thread32First"); 
		Thread32Next   = (THREADWALK)GetProcAddress(hKernel,  "Thread32Next"); 
 
		bRet =  Module32First && Module32Next  && Process32First && 
                Process32Next && Thread32First && Thread32Next && 
                CreateToolhelp32Snapshot; 
    } 
    else 
        bRet = FALSE;
 
    return bRet; 
} 

BOOL
InitPsApi(
	)
/*++

Routine Description:

	Dynamically load the WinNT-only PSAPI function(s).  These functions must
	be dynamically loaded so this will run properly under W95.

	If this function is successful, you must call FreePsApi () function when
	you are done using the psapi dll.  If you do not, resources will not be
	properly deallocated.

Return Value:

	True if the PSAPI dll was successfully loaded and all entry points to that
	dll were successfully setup.  False otherwise.

Comments:

	At this time we are only getting entries to GetModuleFileHandleEx.

Environment:

	WinNT only.

--*/
{
	ASSERT (!hModPsApi);
	
	if (GetOsVersion () != VER_PLATFORM_WIN32_NT)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	hModPsApi = LoadLibrary ("psapi.dll");

	if (!hModPsApi)
		return FALSE;

	GetModuleFileNameExA = (GETMODULEFILENAMEEXA)
			GetProcAddress (hModPsApi, "GetModuleFileNameExA");

	GetModuleFileNameExW = (GETMODULEFILENAMEEXW)
			GetProcAddress (hModPsApi, "GetModuleFileNameExW");

	EnumProcesses = (ENUMPROCESSES)
		        GetProcAddress (hModPsApi, "EnumProcesses");
			

	if (!GetModuleFileNameExA || !EnumProcesses || !GetModuleFileNameExW)
	{
		FreePsApi ();
		return FALSE;
	}

	return TRUE;
}


void
FreePsApi(
	)
/*++

Routine Description:

	This frees all PSAPI resources.

--*/
{
	if (hModPsApi)
	{
		FreeLibrary (hModPsApi);
	}

	hModPsApi = NULL;
	GetModuleFileNameExA = NULL;
	GetModuleFileNameExW = NULL;
    EnumProcesses        = NULL;

}


BOOL
GetTaskImageInfo(
	IN OUT	PTASK_LIST	Task
	)
{
	ULONG OSVersion = GetOsVersion ();
	
	if (OSVersion == VER_PLATFORM_WIN32_WINDOWS)	// W95
	{
		return W95GetImageInfo (Task);
	}
	else if (OSVersion == VER_PLATFORM_WIN32_NT)
	{
		return WNtGetImageInfo (Task);
	}
	else
	{
		//	Not NT or W95
		
		return FALSE;
	}
}


BOOL
WNtGetImageInfo(
	IN OUT PTASK_LIST	Task
	)
{
	HANDLE	hProcess;
	BOOL	fStatus = TRUE;
	CHAR*	ImageName;
	CHAR	Buffer [_MAX_PATH];
	CHAR*	foo;
	WCHAR	Buffer2 [_MAX_PATH];

	hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE,
							Task->dwProcessId);

	if (!hProcess)
		return FALSE;

	fStatus = GetModuleFileNameExA (hProcess,
									NULL,
									Buffer,
									sizeof (Buffer)
									);

	GetModuleFileNameExW (hProcess, NULL, Buffer2, sizeof (Buffer2));

	if (!fStatus)
	{
		CloseHandle (hProcess);
		return fStatus;
	}


	if (strncmp (Buffer, "\\??\\", 3) == 00)
	{
		ImageName = &Buffer [4];
	}
	else
	{
		ImageName = Buffer;
	}
	
	//	Sometimes we get funky image names; fix them up a little

	if (GetFullPathName (ImageName, sizeof (Task->ImageName),
					 Task->ImageName, &foo) < 0)
	{
		strcpy (Task->ImageName, ImageName);
	}

	//	If we can't get the proper binary type, we set BinaryType == -1
	//	to signify unknown.
	
	if (!GetImageType (Task->ImageName, &Task->BinaryType))
	{
		Task->BinaryType = IMAGE_BAD_EXECUTABLE;
	}

	return fStatus;
}

	
BOOL
W95GetImageInfo(
	IN OUT	PTASK_LIST	Task
	)
/*++

Routine Description:

	On W95 the W95GetTaskList () function has already filled in the
	ImageName.  We just need to fill in the binary type.

--*/
{

	if (!GetImageType (Task->ImageName, &Task->BinaryType))
		Task->BinaryType = IMAGE_BAD_EXECUTABLE;

	return TRUE;
}


DWORD
GetTaskList(
	PTASK_LIST	pTask,
	DWORD		dwNumTasks
	)
{
	ULONG	OsVersion = GetOsVersion ();
	
	if (OsVersion == VER_PLATFORM_WIN32_WINDOWS)
	{
		return W95GetTaskList (pTask, dwNumTasks);
	}
	else if (OsVersion == VER_PLATFORM_WIN32_NT)
	{
		return WNtGetTaskList (pTask, dwNumTasks);
	}
	else
	{
		ASSERT (FALSE);
		return 0;
	}
}


DWORD
W95GetTaskList(
	PTASK_LIST	pTask,
	DWORD		dwNumTasks
	)
/*++

Routine Description:

	Provides an API for getting a list of running tasks using TOOLHELP32 API.

Environment:

	Win95 only.

s--*/
{
	DWORD			i;
	PROCESSENTRY32	ProcessInfo;
	BOOL			Success;
	HANDLE			hSnapshot;
	
	if (!InitToolhelp32 ())
		return 0;

	hSnapshot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);

	ProcessInfo.dwSize = sizeof (ProcessInfo);

	i = 0;

	for (i = 0, Success = Process32First (hSnapshot, &ProcessInfo);
		 Success;
		 Success = Process32Next (hSnapshot, &ProcessInfo)
		)
	{
		pTask [i].dwProcessId = ProcessInfo.th32ProcessID ;
		pTask [i].dwInheritedFromProcessId = ProcessInfo.th32ParentProcessID ;
		pTask [i].flags = 0;
		pTask [i].hwnd = 0;
		pTask [i].WindowTitle [0] = '\000';

		_splitpath (ProcessInfo.szExeFile, NULL, NULL, pTask [i].ProcessName,
					NULL);

		if (_tcsicmp (pTask [i].ProcessName, "KERNEL32") == 00)
			continue;
			
		strcpy (pTask [i].ImageName, ProcessInfo.szExeFile);

		i++;
	}

	CloseHandle (hSnapshot);

	return i;
}


BOOL
GetImageNameUsingPsapi(
	DWORD dwProcessId,
	LPTSTR szImageName,
	DWORD cbBufferSize
	)
{
	HANDLE	hProcess = NULL;
	DWORD	dwStatus = 0;
	BOOL	fSucc = FALSE;
	TCHAR*	pchImageName = NULL;
	BOOL	fDebugPrivilege;

	fDebugPrivilege = IsDebugPrivilegeSet ();
	SetDebugPrivilege (TRUE);

	hProcess = OpenProcess (PROCESS_ALL_ACCESS,
							FALSE,
							dwProcessId
							);

	if (!hProcess) {
		fSucc = FALSE;
		goto Cleanup;
	}

	fSucc = GetImageNameUsingPsapiEx( hProcess, 
									  NULL,
			                          dwProcessId,
			                          GetModuleFileNameExA,
			                          szImageName,
									  cbBufferSize
									 );


Cleanup:

	if (hProcess) {
		CloseHandle (hProcess);
		hProcess = FALSE;
	}

	SetDebugPrivilege (fDebugPrivilege);
	
	return fSucc;
}


BOOL
WINAPI
IsDebugPrivilegeSet(
	)

/*++

Routine Description:

	Check if the SE_DEBUG_PRIVILEGE is set.

Return Values:

	TRUE - if SE_DEBUG_PRIVILEGE is set.

	FALSE - if not or error.

--*/

{
	HANDLE hToken;
	BOOL fSucc;
	BOOL Result;
	PRIVILEGE_SET Privileges;

	
	fSucc = OpenProcessToken (GetCurrentProcess (),
							  TOKEN_QUERY,
							  &hToken
							  );

	if (!fSucc) {
		return FALSE;
	}

	Privileges.PrivilegeCount = 1;
	Privileges.Control = 0;
	Privileges.Privilege [0].Luid = SeDebugLuid;
	Privileges.Privilege [0].Attributes = 0;

	fSucc = PrivilegeCheck (hToken,
							&Privileges,
							&Result);

	if (!fSucc) {
		Result = FALSE;
	}

	CloseHandle(hToken);

	return Result;
}

LOCAL
BOOL
GetLongFileNameAlt(
    LPCTSTR	szInFilename,
    LPTSTR	szOutLongFileName
    )
/*++

Routine Description:

    Converts 8.3 fileNames to LongFileNames and fixes the upper/lower case 
	of a filename and so that it matches what is on disk.  


Arguments:

	szInFilename - Name of file.

	szOutLongFileName - Upper/lower case changed, long filename variant.
					 Assumes size is big enough to hold long filename.

Comments:

    NT5 and Win'98 have an api called GetLongFileName which could be used 

	This is a rip off of FixFileName in debugger\dm - any changes made
	there needs to be propogated here

--*/
{
	CHAR            rgchDrive[MAX_PATH];   /* "X:\ or \\machine_name" */
    WIN32_FIND_DATA wfd;
    CHAR *          pch;
    CHAR *          pchTemp;
    CHAR *          pchStart;
    CHAR            ch;
    HANDLE          hSearch;
    CHAR            szFilename[512];

	//
	// Make Local copy
	//

    _tcscpy( szFilename, szInFilename );                

	if (szFilename[0] && szFilename[1] == ':') {
		sprintf(rgchDrive, "%c:\\", szFilename[0]);
	} else {

		//
		// we don't have the absolute location since there is no drive
		// name - Don't ask me ask GetModuleFileNameEx -that's what
		// returned the garbage in the first place
		//

		return false;
	}

    // 
    // For each filename component, check what case it has on disk.
    // 
		
    pch = szFilename;
    pchTemp = szOutLongFileName;


	//
	// skip past  drive letter and upper case it
	//


    _tcsncpy(pchTemp, pch, 2);
    *pchTemp = (char) CharUpper ((LPTSTR)(*pchTemp));      
    pch += 2;
    pchTemp += 2;

    if (*pch == '/' || *pch == '\\') {
        *pch = *pchTemp = '\\';
        ++pchTemp;
        ++pch;
    }

    while (*pch) {
       size_t iLen;
       pchStart = pch;

       while (*pch && *pch != '\\' && *pch != '/')
       pch = _tcsinc( pch );

       ch = *pch;
       *pch = '\0';

       //
       // Find this filename component
       // 
       hSearch = FindFirstFile(szFilename, &wfd);

       //
       // If the search failed, we'll give
       // up and convert the rest of the name to lower case

       if (hSearch == INVALID_HANDLE_VALUE) {
            return false;
	   }
       //
       // Copy the correct name into the temp filename,
       // 
       iLen = _tcslen(wfd.cFileName);
       _tcsncpy ( pchTemp, wfd.cFileName, iLen );
       pchTemp += iLen;

       //
       // Close the search
       // 
#ifdef _DEBUG
       if  (FindNextFile(hSearch, &wfd) ) {
           DebugBreak();
       }
#endif
       FindClose ( hSearch );

       //
       // Restore the slash or NULL
       // 
       *pch = ch;
       //
       // If we're on a separator, move to next filename component
       // 
       if (*pch) {
           *pchTemp = *pch = '\\';
           pch++; pchTemp++;
       }
    }
    *pchTemp = '\0';
	
	return true;
}
BOOL
WINAPI
GetImageNameUsingPsapiEx(
    HANDLE               hProcess,		
	HMODULE              hDll,
	DWORD                dwProcessId,
    GETMODULEFILENAMEEXA pfnGetModuleFileNameExA,
	LPTSTR               szImageName,
	DWORD                cbBufferSize
	)
{
	DWORD	dwStatus = 0;
	BOOL	fSucc = FALSE;
	TCHAR	szBuffer [_MAX_PATH + 10];
	TCHAR	szBufferLongName [_MAX_PATH + 10];
	TCHAR*	pchImageName = NULL;

	ASSERT (pfnGetModuleFileNameExA || !"Bogus Input");

	dwStatus =(*pfnGetModuleFileNameExA)(hProcess,
			                             hDll,
									     szBuffer,
									     sizeof (szBuffer)
									     );

	if (dwStatus == 00) {
		fSucc = FALSE;
		goto Cleanup;
	}

	//
	// Fixup funky image names.
	//

	if (_tcsncmp (szBuffer, "\\??\\", 3) == 00) {
		pchImageName = &szBuffer [4];
	} else {
		pchImageName = szBuffer;

	}

	//
	// Compenstate for the occasional 8.3 fileName that comes
	// out of GetModuleFileNameExA
	//
	if (GetLongFileNameAlt(pchImageName, szBufferLongName)) {
	    _tcsncpy (szImageName, szBufferLongName, cbBufferSize);
	} else {
	    _tcsncpy (szImageName, pchImageName, cbBufferSize);
	}

	fSucc = TRUE;

Cleanup:
	/*
	if (!fSucc) {
		if (dwProcessId == 0) {
			_tcsncpy (szImageName, "System Idle Process", cbBufferSize);
			fSucc = TRUE;
		} else if (dwProcessId == 2) {
			_tcsncpy (szImageName, "System", cbBufferSize);
			fSucc = TRUE;
		}
	}
	*/
	return fSucc;
}
	

BOOL
WINAPI
IsSystemProcess(
	DWORD dwProcessId
	)

/*++

Routine Description:

	We define a system process as a process whose handle cannot be opened
	without the SE_DEBUG privledge set. This is probably only meaningful
	on NT.

--*/

{
	HANDLE hProcess;
	BOOL fDebugPrivilege;
	BOOL fSystemProcess;

	if (GetOsVersion () != VER_PLATFORM_WIN32_NT) {
		return FALSE;
	}
	
	fDebugPrivilege = IsDebugPrivilegeSet ();
	SetDebugPrivilege (FALSE);

	hProcess = OpenProcess (PROCESS_ALL_ACCESS,
							FALSE,
							dwProcessId
							);

	
	if (hProcess) {
		fSystemProcess = FALSE;
		if (GetOsVersion () == VER_PLATFORM_WIN32_NT) {
			if (dwProcessId == 0 || dwProcessId == 2) {
				fSystemProcess = TRUE;
			}
		}
		goto Cleanup;
	}

	SetDebugPrivilege (TRUE);
	hProcess = OpenProcess (PROCESS_ALL_ACCESS,
							FALSE,
							dwProcessId
							);

	if (hProcess) {
		fSystemProcess = TRUE;
		goto Cleanup;
	}

	fSystemProcess = FALSE;


Cleanup:

	if (hProcess) {
		CloseHandle (hProcess);
		hProcess = NULL;
	}

	SetDebugPrivilege (fDebugPrivilege);
	return fSystemProcess;
}
DWORD
WNtGetTaskList(
    PTASK_LIST  rgTasks,
	DWORD       maxCount
    )

/*++

Routine Description:

    Provides an API for getting a list of tasks running at the time of the
    API call.  This function uses the registry performance data to get the
    task list and is therefor straight WIN32 calls that anyone can call.

Arguments:

    dwNumTasks       - maximum number of tasks that the pTask array can hold

Return Value:

    Number of tasks placed into the pTask array.

Environment:

	WinNT only.

--*/

{

	DWORD i;
	DWORD cb;
	BOOL succ;
	PTASK_LIST pTask = NULL;
	IMAGE_INFO ImageInfo;
	DWORD* rgProcessIds = NULL;
	DWORD AllocatedCount = 0;
	DWORD Count = 0;
	int   returningTaskCount  = 0;

	//
	// EnumProcesses has a very strange protocol for returning the number
	// of processes in the list. If the number used is the same as the
	// max number, assume EnumProcesses did not have enough space, realloc
	// more space and try again.
	//
	do {

		if (rgProcessIds) {
			free (rgProcessIds);
			rgProcessIds = NULL;
		} else {
			AllocatedCount = 200;
		}

		AllocatedCount *= 2;

		rgProcessIds = (DWORD*) malloc (sizeof (DWORD) * AllocatedCount);
		succ = EnumProcesses (rgProcessIds, AllocatedCount, &cb);

		if (!succ) {
			free( rgProcessIds );
			return 0;
		}

	} while (cb == AllocatedCount);


	Count = cb / sizeof (DWORD);

	if (maxCount< Count) {
		free( rgProcessIds );
		return Count;
	}


	for (i = 0; i < Count; i++) {

		pTask = &rgTasks [returningTaskCount];

		ZeroMemory (pTask, sizeof (*pTask));

		pTask->dwProcessId = rgProcessIds [i];

		if (IsSystemProcess (pTask->dwProcessId)) {
		    pTask->flags |= TASK_SYSTEM_PROCESS;
		} else{ 
		    pTask->flags |= TASK_NORMAL_PROCESS;
		}

		succ = GetImageNameUsingPsapi ( pTask->dwProcessId,
		                    			pTask->ImageName,
					                    sizeof (pTask->ImageName)
					                  );
	 	if (succ) {
		    _splitpath (pTask->ImageName,
		    			NULL,
		    			NULL,
		    			pTask->ProcessName,
		    			NULL	
		    			);
		    ImageInfo.Size = sizeof (ImageInfo);
		    succ = GetImageInfo (pTask->ImageName, &ImageInfo);

		   	if (!succ) {
		        pTask->BinaryType= IMAGE_BAD_EXECUTABLE;
		    } else {
		        pTask->BinaryType= ImageInfo.ImageType;
		    }
		    returningTaskCount++;
		} else {
			//
			// BUG BUG - We can't get to the process (lack of permissions) - 
			// we're going to assume system processes - SUCKS!!!!!!!!!!
			//
		    //pTask->flags |= TASK_SYSTEM_PROCESS;
		

		}

	}

	free (rgProcessIds);
	rgProcessIds = NULL;

	return returningTaskCount;
}

//#if 0
//DWORD
//WNtGetTaskList(
//    PTASK_LIST  pTask,
//    DWORD       dwNumTasks
//    )
//
///*++
//
//Routine Description:
//
//    Provides an API for getting a list of tasks running at the time of the
//    API call.  This function uses the registry performance data to get the
//    task list and is therefor straight WIN32 calls that anyone can call.
//
//Arguments:
//
//    dwNumTasks       - maximum number of tasks that the pTask array can hold
//
//Return Value:
//
//    Number of tasks placed into the pTask array.
//
//Environment:
//
//	WinNT only.
//
//--*/
//
//{
//    DWORD                        rc;
//    HKEY                         hKeyNames;
//    DWORD                        dwType;
//    DWORD                        dwSize;
//    LPBYTE                       buf = NULL;
//    CHAR                         szSubKey[1024];
//    LANGID                       lid;
//    LPSTR                        p;
//    LPSTR                        p2;
//    PPERF_DATA_BLOCK             pPerf;
//    PPERF_OBJECT_TYPE            pObj;
//    PPERF_INSTANCE_DEFINITION    pInst;
//    PPERF_COUNTER_BLOCK          pCounter;
//    PPERF_COUNTER_DEFINITION     pCounterDef;
//    DWORD                        i;
//    DWORD                        dwProcessIdTitle;
//    DWORD                        dwProcessIdCounter;
//    CHAR                         szProcessName[MAX_PATH];
//    DWORD                        dwLimit = dwNumTasks - 1;
//	DWORD						 dwT = 0;
//
//
//
//    //
//    // Look for the list of counters.  Always use the neutral
//    // English version, regardless of the local language.  We
//    // are looking for some particular keys, and we are always
//    // going to do our looking in English.  We are not going
//    // to show the user the counter names, so there is no need
//    // to go find the corresponding name in the local language.
//    //
//    lid = MAKELANGID( LANG_ENGLISH, SUBLANG_NEUTRAL );
//    sprintf( szSubKey, "%s\\%03x", REGKEY_PERF, lid );
//    rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
//                       szSubKey,
//                       0,
//                       KEY_READ,
//                       &hKeyNames
//                     );
//    if (rc != ERROR_SUCCESS) {
//        goto exit;
//    }
//
//    //
//    // get the buffer size for the counter names
//    //
//    rc = RegQueryValueEx( hKeyNames,
//                          REGSUBKEY_COUNTERS,
//                          NULL,
//                          &dwType,
//                          NULL,
//                          &dwSize
//                        );
//
//    if (rc != ERROR_SUCCESS) {
//        goto exit;
//    }
//
//    //
//    // allocate the counter names buffer
//    //
//    buf = (LPBYTE) malloc( dwSize );
//    if (buf == NULL) {
//        goto exit;
//    }
//    memset( buf, 0, dwSize );
//
//    //
//    // read the counter names from the registry
//    //
//    rc = RegQueryValueEx( hKeyNames,
//                          REGSUBKEY_COUNTERS,
//                          NULL,
//                          &dwType,
//                          buf,
//                          &dwSize
//                        );
//
//    if (rc != ERROR_SUCCESS) {
//        goto exit;
//    }
//
//    //
//    // now loop thru the counter names looking for the following counters:
//    //
//    //      1.  "Process"           process name
//    //      2.  "ID Process"        process id
//    //
//    // the buffer contains multiple null terminated strings and then
//    // finally null terminated at the end.  the strings are in pairs of
//    // counter number and counter name.
//    //
//
//    p = (LPSTR) buf;
//    while (*p) {
//        if (p > (LPSTR) buf) {
//            for( p2=p-2; isdigit(*p2); p2--) ;
//            }
//        if (lstrcmpi(p, PROCESS_COUNTER) == 0) {
//            //
//            // look backwards for the counter number
//            //
//            for( p2=p-2; isdigit(*p2); p2--) ;
//            strcpy( szSubKey, p2+1 );
//        }
//        else
//        if (lstrcmpi(p, PROCESSID_COUNTER) == 0) {
//            //
//            // look backwards for the counter number
//            //
//            for( p2=p-2; isdigit(*p2); p2--) ;
//            dwProcessIdTitle = atol( p2+1 );
//        }
//        //
//        // next string
//        //
//        p += (strlen(p) + 1);
//    }
//
//    //
//    // free the counter names buffer
//    //
//    free( buf );
//
//
//    //
//    // allocate the initial buffer for the performance data
//    //
//    dwSize = INITIAL_SIZE;
//    buf = (LPBYTE) malloc( dwSize );
//    if (buf == NULL) {
//        goto exit;
//    }
//    memset( buf, 0, dwSize );
//
//
//    while (TRUE) {
//		dwT = dwSize;	
//        rc = RegQueryValueEx( HKEY_PERFORMANCE_DATA,
//                              szSubKey,
//                              NULL,
//                              &dwType,
//                              buf,
//                              &dwSize
//                            );
//
//        pPerf = (PPERF_DATA_BLOCK) buf;
//
//        //
//        // check for success and valid perf data block signature
//        //
//        if ((rc == ERROR_SUCCESS) &&
//            (dwSize > 0) &&
//            (pPerf)->Signature[0] == (WCHAR)'P' &&
//            (pPerf)->Signature[1] == (WCHAR)'E' &&
//            (pPerf)->Signature[2] == (WCHAR)'R' &&
//            (pPerf)->Signature[3] == (WCHAR)'F' ) {
//            break;
//        }
//
//		//
//		// This is a workaround for a bug either in the API call, or
//		// in one of the performance DLLs.  The return value is
//		// ERROR_MORE_DATA, but the API sometimes tells us either the
//		// same size or a smaller size!!!  This may be related to having
//		// SQL server installed and running.  It only seems to repro in
//		// that case.
//		//
//#if 0		
//		if (rc == ERROR_MORE_DATA && dwT <= dwSize) {
//
//			if (pPerf->Signature [0] == (WCHAR) 'P' &&
//				pPerf->Signature [1] == (WCHAR) 'E' &&
//				pPerf->Signature [2] == (WCHAR) 'R' &&
//				pPerf->Signature [3] == (WCHAR) 'F')
//			{
//				break;
//			} else {
//				goto exit;
//			}
//		}
//#endif			
//
//        //
//        // if buffer is not big enough, reallocate and try again
//        //
//		
//        if (rc == ERROR_MORE_DATA) {
//            dwSize += EXTEND_SIZE;
//            buf = (LPBYTE) realloc( buf, dwSize );
//            memset( buf, 0, dwSize );
//        }
//        else {
//            goto exit;
//        }
//    }
//
//    //
//    // set the perf_object_type pointer
//    //
//    pObj = (PPERF_OBJECT_TYPE) ((DWORD)pPerf + pPerf->HeaderLength);
//
//    //
//    // loop thru the performance counter definition records looking
//    // for the process id counter and then save its offset
//    //
//    pCounterDef = (PPERF_COUNTER_DEFINITION) ((DWORD)pObj + pObj->HeaderLength);
//    for (i=0; i<(DWORD)pObj->NumCounters; i++) {
//        if (pCounterDef->CounterNameTitleIndex == dwProcessIdTitle) {
//            dwProcessIdCounter = pCounterDef->CounterOffset;
//            break;
//        }
//        pCounterDef++;
//    }
//
//    dwNumTasks = min( dwLimit, (DWORD)pObj->NumInstances );
//
//    pInst = (PPERF_INSTANCE_DEFINITION) ((DWORD)pObj + pObj->DefinitionLength);
//
//    //
//    // loop thru the performance instance data extracting each process name
//    // and process id
//    //
//    for (i=0; i<dwNumTasks; i++) {
//        //
//        // pointer to the process name
//        //
//        p = (LPSTR) ((DWORD)pInst + pInst->NameOffset);
//
//        //
//        // convert it to ascii
//        //
//        rc = WideCharToMultiByte( CP_ACP,
//                                  0,
//                                  (LPCWSTR)p,
//                                  -1,
//                                  szProcessName,
//                                  sizeof(szProcessName),
//                                  NULL,
//                                  NULL
//                                );
//
//        if (!rc) {
//
//			//
//			// if we cant convert the string then use a default value
//			//
//			
//            strcpy( pTask->ProcessName, UNKNOWN_TASK );
//        }
//
//        if (strlen(szProcessName)+4 <= sizeof(pTask->ProcessName)) {
//            strcpy( pTask->ProcessName, szProcessName );
////            strcat( pTask->ProcessName, ".exe" );
//        }
//
//        //
//        // get the process id
//        //
//		
//        pCounter = (PPERF_COUNTER_BLOCK) ((DWORD)pInst + pInst->ByteLength);
//		pTask->hwnd = NULL;
//        pTask->flags = 0;
//        pTask->dwProcessId = *((LPDWORD) ((DWORD)pCounter + dwProcessIdCounter));
//		pTask->WindowTitle [0] = '\000';
//		
//        if (pTask->dwProcessId == 0) {
//            pTask->dwProcessId = (DWORD)-2;
//        }
//
//        //
//        // next process
//        //
//        pTask++;
//        pInst = (PPERF_INSTANCE_DEFINITION) ((DWORD)pCounter + pCounter->ByteLength);
//    }
//
//exit:
//    if (buf) {
//        free( buf );
//    }
//
//    RegCloseKey( hKeyNames );
//    RegCloseKey( HKEY_PERFORMANCE_DATA );
//
//    return dwNumTasks;
//}
//
//#endif



BOOL
SetDebugPrivilege(
	BOOL	fEnable
    )

/*++

Routine Description:

    Changes the process's privilege so that we can open process on all
	processes.  In partucluar, on services.

Arguments:

	fEnable - TRUE means enable the debug privledge; FALSE means
			  disable the privledge.

Return Value:

    TRUE             - success
    FALSE            - failure

--*/

{
    HANDLE 				hToken;
    LUID 				DebugValue;
    TOKEN_PRIVILEGES 	tkp;
	BOOL				fSuccess;

    //
    // Retrieve a handle of the access token
    //

	fSuccess = OpenProcessToken (GetCurrentProcess(),
					             TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
						         &hToken);

	if (!fSuccess)
		return FALSE;

    //
    // Enable the SE_DEBUG_NAME privilege
    //


	fSuccess = LookupPrivilegeValue ((LPSTR) NULL,
									 SE_DEBUG_NAME,
									 &DebugValue);

	if (!fSuccess)
        return FALSE;

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = DebugValue;
    tkp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;

    AdjustTokenPrivileges (hToken,
		 				   FALSE,
						   &tkp,
						   sizeof (TOKEN_PRIVILEGES),
						   (PTOKEN_PRIVILEGES) NULL,
						   (PDWORD) NULL);

    //
    // The return value of AdjustTokenPrivileges can't be tested
    //
	
    if (GetLastError () != ERROR_SUCCESS) {
        return FALSE;
    }

    return TRUE;
}



Protect <HANDLE>::~Protect(
	)
/*++

Routine Description:

	Destructor for HANDLE objects.  NOTE: must be initialized to NULL.

--*/
{
	if (m_h)
	{
		CloseHandle (m_h);
		m_h = NULL;
	}
}


BOOL
GetImageType(
	LPCSTR	ImageName,
	ULONG*	BinaryType
	)
/*++

Routine Description:

	This is basically the same function as GetBinaryType () with slightly
	different semantics.  But this works under both WinNT and W95
	(GetBinaryType () is WinNT only).

Return Value:

	Returns False when it cannot find the file asked for.  Otherwise returns
	True and places the binary type in the BinaryType parameter.  This may
	be IMAGE_BAD_EXECUTABLE.  Note that unlike GetBinaryType () this
	function will not recognize PIF files; nor will it recognize a COM file
	(edit.com) as an executable.

    The Parameter BinaryType will always be valid.

--*/
{
    HANDLE				hImage = NULL;
	Protect <HANDLE>	protect (hImage);
	
    DWORD		bytes;
    DWORD		iSection;
    DWORD		SectionOffset;
    DWORD		CoffHeaderOffset;
    DWORD		MoreDosHeader[16];
    ULONG		ntSignature;
	BOOL		fSuccess;
	ULONG		cbRead;

	IMAGE_DOS_HEADER      image_dos_header;
    IMAGE_FILE_HEADER     image_file_header;
    IMAGE_OPTIONAL_HEADER image_optional_header;
    IMAGE_SECTION_HEADER  image_section_header;

	*BinaryType = IMAGE_BAD_EXECUTABLE;

    hImage = CreateFile (ImageName,
                         GENERIC_READ,
                         FILE_SHARE_READ,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL
						 );

    if (INVALID_HANDLE_VALUE == hImage)
		return FALSE;


    fSuccess = ReadFile (hImage,
						 &image_dos_header,
						 sizeof(IMAGE_DOS_HEADER),
						 &cbRead,
						 NULL
						 );

	if (!fSuccess || IMAGE_DOS_SIGNATURE != image_dos_header.e_magic)
		return TRUE;

	*BinaryType = IMAGE_DOS_STUB;

    fSuccess = ReadFile (hImage,
						 MoreDosHeader,
						 sizeof(MoreDosHeader),
						 &cbRead,
						 NULL
						 );

	if (!fSuccess)
		return TRUE;

#if 1

	CoffHeaderOffset = SetFilePointer (hImage,
									  image_dos_header.e_lfanew,
									  NULL,
									  FILE_BEGIN
									  );

	if (CoffHeaderOffset == (ULONG) -1)
		return FALSE;

	CoffHeaderOffset += sizeof (ULONG);


#else

    CoffHeaderOffset = AbsoluteSeek (hImage, image_dos_header.e_lfanew) +
									 sizeof(ULONG);

#endif

    fSuccess = ReadFile (hImage, &ntSignature, sizeof(ULONG), &cbRead, NULL);

	if (IMAGE_OS2_SIGNATURE == ntSignature)
		*BinaryType = IMAGE_WIN16_OR_OS2;

    if (IMAGE_NT_SIGNATURE != ntSignature)
		return TRUE;

    SectionOffset = CoffHeaderOffset + IMAGE_SIZEOF_FILE_HEADER +
                    IMAGE_SIZEOF_NT_OPTIONAL_HEADER;

	fSuccess = ReadFile (hImage,
						 &image_file_header,
						 IMAGE_SIZEOF_FILE_HEADER,
						 &cbRead,
						 NULL
						 );

	if (!fSuccess)
		return TRUE;
		
    fSuccess = ReadFile (hImage,
						 &image_optional_header,
						 IMAGE_SIZEOF_NT_OPTIONAL_HEADER,
						 &cbRead,
						 NULL
						 );

	if (!fSuccess)
		return TRUE;

	*BinaryType = image_optional_header.Subsystem;

	return TRUE;
}



BOOL
KillProcess(
    PTASK_LIST tlist,
    BOOL       fForce
    )
{
    HANDLE            hProcess;


    if (fForce || !tlist->hwnd)
	{
        hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE,
								tlist->dwProcessId );

		if (hProcess)
		{
            hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE,
									tlist->dwProcessId );

			if (hProcess == NULL) {
                return FALSE;
            }

            if (!TerminateProcess( hProcess, 1 )) {
                CloseHandle( hProcess );
                return FALSE;
            }

            CloseHandle( hProcess );
            return TRUE;
        }
    }

    //
    // kill the process
    //
	
    PostMessage( tlist->hwnd, WM_CLOSE, 0, 0 );

    return TRUE;
}


VOID
GetWindowTitles(
    PTASK_LIST_ENUM te
    )
{
    //
    // enumerate all windows
    //
	
    EnumWindows( EnumWindowsProc, (LPARAM) te );
}



BOOL CALLBACK
EnumWindowsProc(
    HWND    hwnd,
    LPARAM 	lParam
    )

/*++

Routine Description:

    Callback function for window enumeration.

Arguments:

    hwnd             - window handle
    lParam           - ** not used **

Return Value:

    TRUE  - continues the enumeration

--*/

{
    DWORD             pid = 0;
    DWORD             i;
    CHAR              buf[TITLE_SIZE];
    PTASK_LIST_ENUM   te = (PTASK_LIST_ENUM)lParam;
    PTASK_LIST        tlist = te->tlist;
    DWORD             numTasks = te->numtasks;


    //
    // get the processid for this window
    //
	
    if (!GetWindowThreadProcessId( hwnd, &pid )) {
        return TRUE;
    }

    //
    // look for the task in the task list for this window
    //
	
    for (i=0; i<numTasks; i++)
	{
		//
		//	we walk the window tree in Z Order (highest in Z order first);
		//	so if we already have a window associated with this process
		//	dont replace it
		//
		
        if (tlist[i].dwProcessId == pid && IsWindowVisible (hwnd) &&
			tlist [i].hwnd == NULL)
		{
            tlist[i].hwnd = hwnd;

			//
		    // we found the task so lets try to get the
            // window text
            //
			
            if (GetWindowText( tlist[i].hwnd, buf, sizeof(buf) )) {

			//
			//	got it, so lets save it
    	    //
            	strcpy( tlist[i].WindowTitle, buf );
            }
            break;
        }
    }

    //
    // continue the enumeration
    //
    return TRUE;
}


BOOL
MatchPattern(
    PUCHAR String,
    PUCHAR Pattern
    )
{
    UCHAR   c, p, l;

    for (; ;) {
        switch (p = *Pattern++) {
            case 0:                             // end of pattern
                return *String ? FALSE : TRUE;  // if end of string TRUE

            case '*':
                while (*String) {               // match zero or more char
                    if (MatchPattern (String++, Pattern))
                        return TRUE;
                }
                return MatchPattern (String, Pattern);

            case '?':
                if (*String++ == 0)             // match any one char
                    return FALSE;                   // not end of string
                break;

            case '[':
                if ( (c = *String++) == 0)      // match char set
                    return FALSE;                   // syntax

                c = toupper(c);
                l = 0;
                while (p = *Pattern++) {
                    if (p == ']')               // if end of char set, then
                        return FALSE;           // no match found

                    if (p == '-') {             // check a range of chars?
                        p = *Pattern;           // get high limit of range
                        if (p == 0  ||  p == ']')
                            return FALSE;           // syntax

                        if (c >= l  &&  c <= p)
                            break;              // if in range, move on
                    }

                    l = p;
                    if (c == p)                 // if char matches this element
                        break;                  // move on
                }

                while (p  &&  p != ']')         // got a match in char set
                    p = *Pattern++;             // skip to end of set

                break;

            default:
                c = *String++;
                if (toupper(c) != p)            // check for exact char
                    return FALSE;                   // not a match

                break;
        }
    }
}
