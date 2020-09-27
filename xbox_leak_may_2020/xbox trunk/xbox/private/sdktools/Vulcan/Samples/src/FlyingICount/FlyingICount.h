/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: FlyingICount.h
*
* File Comments:
*
*
***********************************************************************/


void RefreshProcesses();
void RefreshModules(VProg *pProg);
void RefreshFunctions(VComp *pComp);
void SetInstrumentation(VProc *pProc);
int GetCounter();

void ClearProgramList();
void ClearComponentList();    
void ClearFunctionList();

void AddProgramToList( const char *sz, VProg *pProg );
void AddComponentToList( const char *sz, VComp *pComp );
void AddFunctionToList( const char *sz, VProc *pProc );

void Error( const char *sz );

