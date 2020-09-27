/********************************************************************************
	FILE:
		Script_GetVariableNumber.cpp

	PURPOSE:
		GetVariableNumber tests for script

	BY:
		DANROSE
********************************************************************************/
 
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

HRESULT Script_GetVariableNumber( CtIDirectMusicPerformance8* ptPerf8 )
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;
	LONG lValue = 0;
	
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NULL, NULL, ptPerf8, NULL ), E_POINTER ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NULL, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );
//	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NULL, &lValue, ptPerf8, NULL ), E_POINTER ) );
//	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NULL, &lValue, ptPerf8, &dmScriptError ), E_POINTER ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NUMBER_GOOD, NULL, ptPerf8, NULL ), E_POINTER ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NUMBER_GOOD, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );
	CHECKEXECUTE( Script_GetVariableNumber( NUMBER_GOOD, &lValue, ptPerf8, NULL ) );
	CHECKEXECUTE( Script_GetVariableNumber( NUMBER_GOOD, &lValue, ptPerf8, &dmScriptError ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( VARIABLE_EMPTY, &lValue, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( VARIABLE_NO_EXIST, &lValue, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );

	return hr;
}
                                           
