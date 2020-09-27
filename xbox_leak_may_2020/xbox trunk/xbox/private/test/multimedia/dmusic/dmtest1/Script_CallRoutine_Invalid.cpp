/********************************************************************************
	FILE:
		Script_CallRoutine_Invalid.cpp

	PURPOSE:
		CallRoutine invalid/fatal tests for script

	BY:
		DANHAFF
********************************************************************************/

#include "globals.h"

HRESULT Script_CallRoutine( LPSTR szRoutine, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo );


HRESULT Script_CallRoutine_Fatal( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;
	DMUS_SCRIPT_ERRORINFO dmScriptError;

    DMTEST_EXECUTE( Script_CallRoutine( NULL, ptPerf8, NULL ) );
    DMTEST_EXECUTE( Script_CallRoutine( NULL, ptPerf8, &dmScriptError ) );
    
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Script_CallRoutine_Invalid_NoAudioPath(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD dwUnused)
{
	HRESULT hr = S_OK;	
    DMUS_SCRIPT_ERRORINFO dmScriptError = {0};
    CHAR *szBasePathServer = "DMusic/DMTest1/Script/Play01/";
    CHAR  szBasePath[MAX_PATH] = {0};
    CHAR  szScriptName[MAX_PATH] = {0};
    CHAR  szRoutineName[MAX_PATH] = {0};
    DWORD dwRangeBottom = 0;
    LONG lRoutineCall = 0;
    CtIDirectMusicScript *ptScript          = NULL;
    CtIDirectMusicScript *ptScriptGlobals   = NULL;
    CtIDirectMusicLoader *ptLoader          = NULL;

    //Get all the media in that directory.
    CHECKRUN(MediaCopyDirectory(szBasePathServer));
    strcpy(szBasePath, MediaServerToLocal(szBasePathServer));

    //Get MAIN1.dls
    {
    CHAR  szBaseDir [MAX_PATH] = {0};
    CHECKRUN(ChopPath(szBasePath, szBaseDir, NULL));
    CHECKRUN(MediaCopyFile("DLS/Main1.DLS", szBaseDir));
    }

    dwRangeBottom = (dwRoutineNumber / 10) * 10;
    CHECK(sprintf(szScriptName, "Routines%03d-%03d.spt", dwRangeBottom, dwRangeBottom + 9));
    CHECK(Log(FYILOGLEVEL, "Script: %s", szScriptName));
    CHECKRUN( dmthCreateLoader( CTIID_IDirectMusicLoader8, &ptLoader ) );
    CHECKRUN( ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szBasePath, TRUE));
    CHECKRUN( ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, "Globals.spt", (void **)&ptScriptGlobals));
    CHECKRUN( ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, szScriptName, (void **)&ptScript));
    CHECKALLOC(ptScript);
    CHECKRUN( ptScript->Init( ptPerf8, &dmScriptError));
    CHECK(dmthVerifyScriptError( &dmScriptError ));

    //Concatenate the RoutineName together.
    CHECK(sprintf(szRoutineName, "Routine%03dc%1d", dwRoutineNumber, lRoutineCall));  //i.e. for dwRoutineNumber = 100, dwRoutineCall=3, makes "Routine100c3"
    CHECK(Log(FYILOGLEVEL, "RoutineName: %s", szRoutineName));

    //Call the routine we're supposed to.  If it doesn't exist, the script is messed up.
    CHECKRUN(ptScript->CallRoutine(szRoutineName, &dmScriptError));
    if (SUCCEEDED(hr))
    {
        if (hr != DMUS_E_SCRIPT_ERROR_IN_SCRIPT)
        {
            Log(ABORTLOGLEVEL, "Error: CallRoutine returned %s (%08X) instead of DMUS_E_SCRIPT_ERROR_IN_SCRIPT", tdmXlatHRESULT(hr), hr);
            hr = E_FAIL;
        }
        else
        {
            Log(ABORTLOGLEVEL, "Error: CallRoutine returned DMUS_E_SCRIPT_ERROR_IN_SCRIPT as expected");
            hr = S_OK;
        }
    }

    SAFE_RELEASE(ptScriptGlobals);
    SAFE_RELEASE(ptScript);
    SAFE_RELEASE(ptLoader);
    return hr;
};
