/*************************************************************************
MODULE: L_Valid.C
AUTHOR: JohnMil
  DATE: 2-5-92

Copyright (c) 1992 Microsoft Corporation

This module contains API Return value Validation routines for API's
beginning with the letter 'L'.
For more information, please refer to BadMan.Doc.
*************************************************************************/
#include <BadMan.h>


#include <Validate.h>

void LoadImageWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
    ValPrintf(hConOut, "LoadImageW: No Validation done - just bad param testing - so always pass!!\r\n");
}

void LoadImageAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
    ValPrintf(hConOut, "LoadImageA: No Validation done - just bad param testing - so always pass!!\r\n");
}

/*
 * Validation for LookupPrivilegeDisplayNameA
 */

VOID
LookupPrivilegeDisplayNameAValidate(
	DefineType		*VarInfo,
	DWORD			*dwParms,
	int			*ParmGoodBad,
	DWORD			dwRetVal,
	HANDLE			hLog,
	HANDLE			hConOut
)
{

}


/*
 * Validation for LookupPrivilegeNameA()
 */

void
LookupPrivilegeNameAValidate(
	DefineType		*VarInfo,
	DWORD			*dwParms,
	int			*ParmGoodBad,
	DWORD			dwRetVal,
	HANDLE			hLog,
	HANDLE			hConOut
)
{

}


/*
 * Validation for LookupPrivilegeValueA
 */

void
LookupPrivilegeValueAValidate(
	DefineType		*VarInfo,
	DWORD			*dwParms,
	int			*ParmGoodBad,
	DWORD			dwRetVal,
	HANDLE			hLog,
	HANDLE			hConOut
)
{

}


/******************************************************/

void LPtoDPValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LeaveCriticalSectionValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LineDDAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LineToValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadAcceleratorsAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadAcceleratorsWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadBitmapAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadBitmapWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadCursorAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadCursorWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadIconAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadIconWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadLibraryAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadLibraryWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadMenuAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadMenuIndirectAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadMenuIndirectWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadMenuWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadModuleValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadResourceValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadStringAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LoadStringWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LocalAllocValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LocalFlagsValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LocalFreeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LocalInitValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LocalLockValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LocalReAllocValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LocalSizeValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LocalUnlockValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LockFileValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LockResourceValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LockWindowUpdateValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void LookupIconIdFromDirectoryValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void lstrcatValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void lstrcmpValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void lstrcmpiValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void lstrcpyValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}


/******************************************************/

void lstrlenValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
	VarInfo;	// -W3 warning elimination
	dwParms;	// -W3 warning elimination
	ParmGoodBad;	// -W3 warning elimination
	dwRetVal;	// -W3 warning elimination
	hLog;		// -W3 warning elimination
	hConOut;	// -W3 warning elimination
}
