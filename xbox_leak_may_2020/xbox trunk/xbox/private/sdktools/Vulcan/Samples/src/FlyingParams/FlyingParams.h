/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: FlyingParams.h
*
* File Comments:
*
*
***********************************************************************/

struct VALUE_DATA
{
    __int64 value;
    int count;
};

const int cValueMax = 128*1024/sizeof(VALUE_DATA);
const int cbValueMax = cValueMax * sizeof(VALUE_DATA);

//#include "psapi.h"
//#include "vulcanapi.h"
class VProg;
class VComp;
class VProc;
class VSymbol;

// Callbacks into UI side
void ClearProgramList();
void ClearComponentList();
void ClearProcedureList();
void ClearParamsList();
void AddProgramToList( const char *sz, VProg *pProg );
void AddComponentToList( const char *sz, VComp *pComp );
void AddProcedureToList( VProc *pProc, int iProc ); 
void AddParamToTree( const char *szSym, VSymbol *pSym, bool fHashChildren );
void Error( const char *szError );

// Calls from UI to Vulcan local routines
void RefreshProcesses( const char *szMachine );
void OnSelChangeProcesses( VProg * pProgSel );
void OnSelChangeComponent( VComp *pComp );
void OnSelChangeProcedures( VProc *pProc );
void OnSelChangeParams( VSymbol **rgpSyms, size_t cPSym );
void OnExpandParam( VSymbol *pSymParent );
void UpdateProcedureHitData();
int  CValuesUpdateValueData();
const char *SzNameAndHitFromProcedure( VProc * pProc, int iProc, bool &fHit );
void GetValueStringFromValueIndex( int iVI, char *szName );
int  GetValueCountFromValueIndex( int iVI );
void Cleanup();