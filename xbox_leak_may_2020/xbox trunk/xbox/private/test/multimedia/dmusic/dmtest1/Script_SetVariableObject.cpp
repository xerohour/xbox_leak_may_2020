/********************************************************************************
	FILE:
		Script_SetVariableObject.cpp

	PURPOSE:
		SetVariableObject tests for script

	BY:
		DANROSE
********************************************************************************/

#include "globals.h"
#include "script.h"
 
HRESULT Script_SetVariableObject( LPSTR szVariableName, IUnknown* ppv, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo )
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
	CHECKRUN( pScript->SetVariableObject( szVariableName, ppv, pInfo ) );
	dmthVerifyScriptError( pInfo );

	RELEASE( pScript );
	RELEASE( pLoader );

	return hr;
}

HRESULT Script_SetVariableObject_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;
	IDirectMusicSegmentState8* pSeg = NULL;

    //Copy down everything in the scripts directory.
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Script/"));
	
	DMTEST_EXECUTE( Script_SetVariableObject( SEGMENT_GOOD, pSeg, ptPerf8, NULL ) );
	DMTEST_EXECUTE( Script_SetVariableObject( SEGMENT_GOOD, pSeg, ptPerf8, &dmScriptError ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( VARIABLE_EMPTY, pSeg, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( VARIABLE_NO_EXIST, pSeg, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );

	RELEASE( pSeg );

	return hr;
}
