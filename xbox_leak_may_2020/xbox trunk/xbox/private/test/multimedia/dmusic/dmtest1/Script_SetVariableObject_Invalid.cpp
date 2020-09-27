/********************************************************************************
	FILE:
		Script_SetVariableObject_Invalid.cpp

	PURPOSE:
		SetVariableObject tests for script

	BY:
		DANROSE
********************************************************************************/

#include "globals.h"
#include "script.h"
 
HRESULT Script_SetVariableObject( LPSTR szVariableName, IUnknown* ppv, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo );

HRESULT Script_SetVariableObject_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

    //Copy down everything in the scripts directory.
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Script/"));

	DMUS_SCRIPT_ERRORINFO dmScriptError;
	IDirectMusicSegmentState8* pSeg = NULL;	
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( NULL, NULL, ptPerf8, NULL ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( NULL, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( NULL, pSeg, ptPerf8, NULL ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( NULL, pSeg, ptPerf8, &dmScriptError ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( SEGMENT_GOOD, NULL, ptPerf8, NULL ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_SetVariableObject( SEGMENT_GOOD, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );
	RELEASE( pSeg );

	return hr;
}
