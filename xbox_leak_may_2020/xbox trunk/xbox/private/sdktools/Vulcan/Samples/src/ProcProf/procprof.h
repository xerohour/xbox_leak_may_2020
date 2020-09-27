/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: ProcProf.cpp
*
* File Comments:
*
*
***********************************************************************/

struct FUNDATA
{
    DWORD addr;         // The address of a routine in the instrumented binary
    const char *szFun;  // A pointer to the null terminated name of the routine
};
