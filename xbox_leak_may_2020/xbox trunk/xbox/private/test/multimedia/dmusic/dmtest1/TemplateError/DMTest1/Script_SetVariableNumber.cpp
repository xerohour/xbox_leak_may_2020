/********************************************************************************
	FILE:
		Script_SetVariableNumber.cpp

	PURPOSE:
		SetVariableNumber tests for script

	BY:
		DANROSE
********************************************************************************/

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

HRESULT Script_SetVariableNumber( CtIDirectMusicPerformance8* ptPerf8 )
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;
	
//	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableNumber( NULL, NEW_VALUE, ptPerf8, NULL ), E_POINTER ) );
//	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableNumber( NULL, NEW_VALUE, ptPerf8, &dmScriptError ), E_POINTER ) );
	CHECKEXECUTE( Script_SetVariableNumber( NUMBER_GOOD, NEW_VALUE, ptPerf8, NULL ) );
	CHECKEXECUTE( Script_SetVariableNumber( NUMBER_GOOD, NEW_VALUE, ptPerf8, &dmScriptError ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableNumber( VARIABLE_EMPTY, NEW_VALUE, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableNumber( VARIABLE_NO_EXIST, NEW_VALUE, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );

	return hr;
}

