/********************************************************************************
	FILE:
		Script_SetVariableNumber.cpp

	PURPOSE:
		SetVariableNumber tests for script

	BY:
		DANROSE
********************************************************************************/
#include "globals.h"

#include "script.h"
 
HRESULT Script_SetVariableNumber( LPSTR szVariableName, LONG lValue, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo )
{
	HRESULT hr = S_OK;
	
	CtIDirectMusicScript* pScript = NULL;
	CtIDirectMusicLoader8* pLoader = NULL;

	LONG lVal = 0;

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
	CHECKRUN( pScript->SetVariableNumber( szVariableName, lValue, pInfo ) );
	dmthVerifyScriptError( pInfo );

	CHECKRUN( dmthInitScriptErrorInfo( pInfo ) );
	CHECKRUN( pScript->GetVariableNumber( szVariableName, &lVal, pInfo ) );
	dmthVerifyScriptError( pInfo );

	CHECKRUN( lVal == lValue ? S_OK : E_FAIL );

	RELEASE( pScript );
	RELEASE( pLoader );

	return hr;
}

HRESULT Script_SetVariableNumber_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;

    //Copy down everything in the scripts directory.
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Script/"));
	
//	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableNumber( NULL, NEW_VALUE, ptPerf8, NULL ), E_POINTER ) );
//	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableNumber( NULL, NEW_VALUE, ptPerf8, &dmScriptError ), E_POINTER ) );
	DMTEST_EXECUTE( Script_SetVariableNumber( NUMBER_GOOD, NEW_VALUE, ptPerf8, NULL ) );
	DMTEST_EXECUTE( Script_SetVariableNumber( NUMBER_GOOD, NEW_VALUE, ptPerf8, &dmScriptError ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableNumber( VARIABLE_EMPTY, NEW_VALUE, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableNumber( VARIABLE_NO_EXIST, NEW_VALUE, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );

	return hr;
}

