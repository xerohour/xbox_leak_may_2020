/********************************************************************************
	FILE:
		Script_GetVariableNumber_Invalid.cpp

	PURPOSE:
		GetVariableNumber invalid/fatal tests for script

	BY:
		DANHAFF
********************************************************************************/

#include "globals.h"
#include "script.h"
 
HRESULT Script_GetVariableNumber( LPSTR szVariableName, LONG* plValue, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo );

HRESULT Script_GetVariableNumber_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;
	DMUS_SCRIPT_ERRORINFO dmScriptError;
	LONG lValue = 0;

    //Copy down everything in the scripts directory.
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Script/"));

	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NULL, NULL, ptPerf8, NULL ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NULL, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NULL, &lValue, ptPerf8, NULL ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NULL, &lValue, ptPerf8, &dmScriptError ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NUMBER_GOOD, NULL, ptPerf8, NULL ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableNumber( NUMBER_GOOD, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );
    
    return hr;
};