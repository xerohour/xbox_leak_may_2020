/********************************************************************************
	FILE:
		Script_GetVariableObject_Invalid.cpp

	PURPOSE:
		GetVariableObject invalid/fatal tests

	BY:
		DANHAFF
********************************************************************************/
#include "globals.h"
#include "script.h"

HRESULT Script_GetVariableObject( LPSTR szVariableName, REFIID riid, LPVOID* ppv, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo );


HRESULT Script_GetVariableObject_Invalid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;
    LPVOID pVoid = NULL;

    //Copy down everything in the scripts directory.
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Script/"));

	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableObject( NULL, GUID_NULL, NULL, ptPerf8, NULL ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableObject( NULL, GUID_NULL, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableObject( NULL, IID_IDirectMusicSegment, &pVoid, ptPerf8, NULL ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableObject( NULL, IID_IDirectMusicSegment, &pVoid, ptPerf8, &dmScriptError ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableObject( SEGMENT_GOOD, GUID_NULL, NULL, ptPerf8, NULL ), E_POINTER ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_GetVariableObject( SEGMENT_GOOD, GUID_NULL, NULL, ptPerf8, &dmScriptError ), E_POINTER ) );

    return hr;

};