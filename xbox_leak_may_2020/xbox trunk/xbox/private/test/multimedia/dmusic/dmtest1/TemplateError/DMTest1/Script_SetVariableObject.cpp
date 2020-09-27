/********************************************************************************
	FILE:
		Script_SetVariableObject.cpp

	PURPOSE:
		SetVariableObject tests for script

	BY:
		DANROSE
********************************************************************************/

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

HRESULT Script_SetVariableObject( CtIDirectMusicPerformance8* ptPerf8 )
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;
	IDirectMusicSegmentState8* pSeg = NULL;
	
//	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( NULL, NULL, ptPerf8, NULL ), E_POINTER ) );
//	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( NULL, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );
//	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( NULL, pSeg, ptPerf8, NULL ), E_POINTER ) );
//	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( NULL, pSeg, ptPerf8, &dmScriptError ), E_POINTER ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( SEGMENT_GOOD, NULL, ptPerf8, NULL ), E_POINTER ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( SEGMENT_GOOD, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );
	CHECKEXECUTE( Script_SetVariableObject( SEGMENT_GOOD, pSeg, ptPerf8, NULL ) );
	CHECKEXECUTE( Script_SetVariableObject( SEGMENT_GOOD, pSeg, ptPerf8, &dmScriptError ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( VARIABLE_EMPTY, pSeg, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( VARIABLE_NO_EXIST, pSeg, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_VARIABLE_NOT_FOUND ) );

	RELEASE( pSeg );

	return hr;
}
