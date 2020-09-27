/********************************************************************************
	FILE:
		Script_GetVariableObject.cpp

	PURPOSE:
		GetVariableObject tests for script

	BY:
		DANROSE
********************************************************************************/
#include "globals.h"
#include "script.h"


 
HRESULT Script_GetVariableObject( LPSTR szVariableName, REFIID riid, LPVOID* ppv, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo )
{
	HRESULT hr = S_OK;
	
	CtIDirectMusicScript* pScript = NULL;
	CtIDirectMusicLoader8* pLoader = NULL;

	CHECKRUN( dmthCreateLoader( IID_IDirectMusicLoader8, &pLoader ) );
	CHECKALLOC( pLoader );
	CHECKRUN( dmthInitScriptErrorInfo( pInfo ) );
	CHECKRUN( dmthCreateScript( VARIABLE_MASTER, pLoader, &pScript ) );
	CHECKALLOC( pScript );

	CHECKRUN( pScript->Init( pPerf, pInfo ) );
	dmthVerifyScriptError( pInfo );

	CHECKRUN( dmthInitScriptErrorInfo( pInfo ) );
	CHECKRUN( pScript->CallRoutine( VARIABLE_INIT, pInfo ) );
	dmthVerifyScriptError( pInfo );

	CHECKRUN( dmthInitScriptErrorInfo( pInfo ) );
	CHECKRUN( pScript->GetVariableObject( szVariableName, riid, ppv, pInfo ) );
	dmthVerifyScriptError( pInfo );

	CHECKRUN( NULL != *((IDirectMusicSegment**)ppv) ? S_OK : E_FAIL );
	RELEASE( pScript );
	RELEASE( pLoader );

	return hr;
}

HRESULT Script_GetVariableObject_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;
	IDirectMusicSegmentState8* pSeg = NULL;

    //Copy down everything in the scripts directory.
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Script/"));	

	DMTEST_EXECUTE( Script_GetVariableObject( SEGMENT_GOOD, IID_IDirectMusicSegment, (LPVOID*) &pSeg, ptPerf8, NULL ) );
	SAFE_RELEASE( pSeg );
	DMTEST_EXECUTE( Script_GetVariableObject( SEGMENT_GOOD, IID_IDirectMusicSegment, (LPVOID*) &pSeg, ptPerf8, &dmScriptError ) );
	SAFE_RELEASE( pSeg );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableObject( VARIABLE_EMPTY, IID_IDirectMusicSegment, (LPVOID*) &pSeg, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );
	SAFE_RELEASE( pSeg );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableObject( VARIABLE_NO_EXIST, IID_IDirectMusicSegment, (LPVOID*) &pSeg, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );
	SAFE_RELEASE( pSeg );

	return hr;
}



