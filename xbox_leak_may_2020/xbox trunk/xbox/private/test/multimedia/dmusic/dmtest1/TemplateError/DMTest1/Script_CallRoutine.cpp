/********************************************************************************
	FILE:
		Script_CallRoutine.cpp

	PURPOSE:
		CallRoutine tests for script

	BY:
		DANROSE
********************************************************************************/

#include "script.h"
 
HRESULT Script_CallRoutine( LPSTR szRoutine, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo )
{
	HRESULT hr = S_OK;
	
	CtIDirectMusicScript* pScript = NULL;
	CtIDirectMusicLoader8* pLoader = NULL;

	CHECKRUN( dmthCreateLoader( IID_IDirectMusicLoader8, &pLoader ) );
	CHECKALLOC( pLoader );
	CHECKRUN( dmthInitScriptErrorInfo( pInfo ) );
	CHECKRUN( dmthCreateScript( CALLROUTINE_MASTER, pLoader, &pScript ) );
	CHECKALLOC( pScript );
	CHECKRUN( pScript->Init( pPerf, pInfo ) );
	dmthVerifyScriptError( pInfo );

	CHECKRUN( dmthInitScriptErrorInfo( pInfo ) );
	CHECKRUN( pScript->CallRoutine( szRoutine, pInfo ) );
	dmthVerifyScriptError( pInfo );

	RELEASE( pScript );
	RELEASE( pLoader );

	return hr;
}

HRESULT Script_CallRoutine( CtIDirectMusicPerformance8* ptPerf8 )
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;

//	CHECKEXECUTE( Script_CallRoutine( NULL, ptPerf8, NULL ) );
//	CHECKEXECUTE( Script_CallRoutine( NULL, ptPerf8, &dmScriptError ) );
	CHECKEXECUTE( Script_CallRoutine( ROUTINE_GOOD, ptPerf8, NULL ) );
	CHECKEXECUTE( Script_CallRoutine( ROUTINE_GOOD, ptPerf8, &dmScriptError ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_CallRoutine( ROUTINE_EMPTY, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_ROUTINE_NOT_FOUND ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_CallRoutine( ROUTINE_NO_EXIST, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_ROUTINE_NOT_FOUND ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_CallRoutine( ROUTINE_RUNTIME_ERROR, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_ERROR_IN_SCRIPT ) );

	return hr;
}
                                                      
