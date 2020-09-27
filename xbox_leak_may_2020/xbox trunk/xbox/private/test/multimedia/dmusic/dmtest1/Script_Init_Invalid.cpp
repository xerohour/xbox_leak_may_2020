/********************************************************************************
	FILE:
		Script_Init_Invalid.cpp

	PURPOSE:
		Init invalid/fatal tests for script

	BY:
		DANHAFF
********************************************************************************/
#include "globals.h"
#include "script.h"

HRESULT Script_Init( LPSTR szScript, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo );

HRESULT Script_Init_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;
    //Copy down everything in the scripts directory.
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Script/"));

	DMUS_SCRIPT_ERRORINFO dmScriptError;


	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_Init( INIT_SCRIPT, NULL, NULL ), E_POINTER ) );
    DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_Init( INIT_SCRIPT, NULL, &dmScriptError ), E_POINTER ) );

	return hr;
}
