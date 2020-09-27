/********************************************************************************
	FILE:
		Script_Init.cpp

	PURPOSE:
		Init tests for script

	BY:
		DANROSE
********************************************************************************/

#include "script.h"

HRESULT Script_Init( LPSTR szScript, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo )
{
	HRESULT hr = S_OK;
	
	CtIDirectMusicScript* pScript = NULL;
	CtIDirectMusicLoader8* pLoader = NULL;

	CHECKRUN( dmthCreateLoader( IID_IDirectMusicLoader8, &pLoader ) );
	CHECKALLOC( pLoader );
	CHECKRUN( dmthInitScriptErrorInfo( pInfo ) );
	CHECKRUN( dmthCreateScript( szScript, pLoader, &pScript ) );
	CHECKALLOC( pScript );
	CHECKRUN( pScript->Init( pPerf, pInfo ) );
	dmthVerifyScriptError( pInfo );

	RELEASE( pScript );
	RELEASE( pLoader );

	return hr;
}

HRESULT Script_Init( CtIDirectMusicPerformance8* ptPerf8 )
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;

	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_Init( INIT_SCRIPT, NULL, NULL ), E_POINTER ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_Init( INIT_SCRIPT, NULL, &dmScriptError ), E_POINTER ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_Init( INIT_SCRIPT_NOEXIST, ptPerf8, &dmScriptError ), E_OUTOFMEMORY ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_Init( INIT_SCRIPT_BAD, ptPerf8, &dmScriptError ), E_OUTOFMEMORY ) );
	CHECKEXECUTE( SUCCEED_ON_FAIL( Script_Init( INIT_SCRIPT_SYNTAX, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_ERROR_IN_SCRIPT ) );

	CHECKEXECUTE( Script_Init( INIT_SCRIPT, ptPerf8, NULL ) );
	CHECKEXECUTE( Script_Init( INIT_SCRIPT, ptPerf8, &dmScriptError ) );

	return hr;
}
