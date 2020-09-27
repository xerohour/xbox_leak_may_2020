
/*************************************************************************
MODULE: O_Valid.C
AUTHOR: JohnMil
  DATE: 2-5-92

Copyright (c) 1992 Microsoft Corporation

This module contains API Return value Validation routines for API's
beginning with the letter 'O'.
For more information, please refer to BadMan.Doc.
*************************************************************************/


#include <BadMan.h>
#include <Validate.h>
#include <parsedef.h>


/******************************************************/

void OemToCharAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OemToCharBuffAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OemToCharBuffWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OemToCharWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OffsetClipRgnValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OffsetRectValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OffsetRgnValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OffsetViewportOrgExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OffsetWindowOrgExValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenClipboardValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenDesktopAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenDesktopWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenEventAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenEventLogAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenEventLogWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenEventWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenFileValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenFileMappingAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenFileMappingWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenIconValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenLocaleValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenMutexAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenMutexWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenProcessTokenValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  DWORD ret = 0, cbError;
  BOOL  bRet = FALSE;
  PUCHAR buffer;
  
  if((ParmGoodBad[0] == CLOPT_GOOD) &&
     (ParmGoodBad[1] == CLOPT_GOOD) &&
     (ParmGoodBad[2] == CLOPT_GOOD)) {
     
     // if the call to the API succeeded

     if((BOOL)dwRetVal) {
        buffer = malloc(256);
        if(!buffer) { 
          ValPrintf(hConOut,"Insufficient memory\n");
          return;
        }
        //BUGBUG bRet = GetTokenInformation((HANDLE) * ((PHANDLE)dwParms[2]), 
                                   //TokenOwner,
                                   //buffer,
                                   //256,
                                   //& ret);
        bRet = FALSE;
        if(!bRet) {
          cbError = GetLastError();
          ValPrintf(hConOut,"OpenProcessTokenValidate Failure %d\n", cbError);
        }
        else {
          ValPrintf(hConOut,"OpenProcessTokenValidate success!!\n");
          free(buffer);
        }
     } // end if dwRetVal
     else
        ValPrintf(hConOut,"OpenProcessToken API call failed\n");
  } // end if parm check
}


/******************************************************/

void OpenProcessValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenSemaphoreAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenSemaphoreWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenSoundValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenSystemFileValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenThreadTokenValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  DWORD ret = 0, cbError;
  BOOL  bRet = FALSE;
  PUCHAR buffer;
  
  if((ParmGoodBad[0] == CLOPT_GOOD) &&
     (ParmGoodBad[1] == CLOPT_GOOD) &&
     (ParmGoodBad[2] == CLOPT_GOOD)) {
     
     // if the call to the API succeeded

     if((BOOL)dwRetVal) {
        buffer = malloc(256);
        if(!buffer) { 
          ValPrintf(hConOut,"Insufficient memory\n");
          return;
        }
        bRet = FALSE;//BUGBUG GetTokenInformation((HANDLE) * ((PHANDLE)dwParms[2]), 
                                   //TokenOwner,
                                   //buffer,
                                   //256,
                                   //& ret);
        if(!bRet) {
          cbError = GetLastError();
          ValPrintf(hConOut,"OpenThreadTokenValidate Failure %d\n", cbError);
        }
        else {
          ValPrintf(hConOut,"OpenThreadTokenValidate success!!\n");
          free(buffer);
        }
     } // end if dwRetVal
     else
        ValPrintf(hConOut,"OpenThreadToken API call failed\n");
  } // end if parm check
}

/******************************************************/

void OpenWindowStationAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OpenWindowStationWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OutputDebugStringAValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}


/******************************************************/

void OutputDebugStringWValidate(DefineType VarInfo[],DWORD dwParms[],
			int ParmGoodBad[],DWORD dwRetVal,
			HANDLE hLog,HANDLE hConOut)
{
  ;
}

