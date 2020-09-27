/********************************************************************************
	FILE:
		Script_GetVariableNumber.cpp

	PURPOSE:
		GetVariableNumber tests for script

	BY:
		DANROSE
********************************************************************************/
#include "globals.h" 
#include "script.h"
 
HRESULT Script_GetVariableNumber( LPSTR szVariableName, LONG* plValue, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo )
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
	CHECKRUN( pScript->GetVariableNumber( szVariableName, plValue, pInfo ) );
	dmthVerifyScriptError( pInfo );

	CHECKRUN( NUMBER_VALUE == *plValue ? S_OK : E_FAIL );

	RELEASE( pScript );
	RELEASE( pLoader );

	return hr;
}

HRESULT Script_GetVariableNumber_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;
	LONG lValue = 0;

    //Copy down everything in the scripts directory.
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Script/"));
	
	DMTEST_EXECUTE( Script_GetVariableNumber( NUMBER_GOOD, &lValue, ptPerf8, NULL ) );
	DMTEST_EXECUTE( Script_GetVariableNumber( NUMBER_GOOD, &lValue, ptPerf8, &dmScriptError ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( VARIABLE_EMPTY, &lValue, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( VARIABLE_NO_EXIST, &lValue, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );

	return hr;
}

