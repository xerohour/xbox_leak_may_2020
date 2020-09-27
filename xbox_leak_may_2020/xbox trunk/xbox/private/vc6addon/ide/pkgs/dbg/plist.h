/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*		  Copyright (C) 1994-1995 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

#ifndef _PLIST_H_
#define _PLIST_H_

#define TITLE_SIZE          64
#define PROCESS_SIZE	_MAX_FNAME

#ifdef __cplusplus
extern "C" {
#endif

//
// task list structure
//

//	
//	NOTE:
//
//		The WindowTitle member is filled in by the GetWindowTitles ()
//		function.  It will not be valid before calling that fucntion.  Also,
//		it is only valid when hwnd != NULL.  The ImageName and BinaryType
//		parameters are valid only after calling GetTaskImageInfo ().
//

typedef struct _TASK_LIST {
    DWORD       dwProcessId;
    DWORD       dwInheritedFromProcessId;
    BOOL        flags;
    HWND		hwnd;
    CHAR        ProcessName [PROCESS_SIZE];
    CHAR        WindowTitle [TITLE_SIZE];
	CHAR		ImageName [_MAX_PATH];
	ULONG		BinaryType;
} TASK_LIST, *PTASK_LIST;


typedef struct _TASK_LIST_ENUM {
    PTASK_LIST  tlist;
    DWORD       numtasks;
} TASK_LIST_ENUM, *PTASK_LIST_ENUM;


DWORD
GetTaskList(
    PTASK_LIST  pTask,
    DWORD       dwNumTasks
    );

BOOL
GetTaskImageInfo(
	IN OUT	PTASK_LIST	pTask
	);

BOOL
SetDebugPrivilege(
	BOOL	fEnable
    );

BOOL
KillProcess(
    PTASK_LIST tlist,
    BOOL       fForce
    );

VOID
GetWindowTitles(
    PTASK_LIST_ENUM te
    );

BOOL
MatchPattern(
    PUCHAR String,
    PUCHAR Pattern
    );

BOOL
InitPlistApi(
	);

void
FreePlistApi(
	);

BOOL
GetImageType(
	LPCSTR	ImageName,
	ULONG*	BinaryType
	);

ULONG
GetOsVersion(
	);

#define IMAGE_DOS_STUB			(((ULONG) -1) - 1)
#define IMAGE_WIN16_OR_OS2		(((ULONG) -1) - 2)
#define IMAGE_BAD_EXECUTABLE	(((ULONG) -1) - 3)

#define TASK_NORMAL_PROCESS 0x00000000
#define TASK_SYSTEM_PROCESS 0x00000001

#ifdef __cplusplus
};
#endif

#endif // _PLIST_H_
