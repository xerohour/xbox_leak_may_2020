/********************************************************************************
	FILE:
		Script_CallRoutine.cpp

	PURPOSE:
		CallRoutine tests for script

	BY:
		DANROSE
********************************************************************************/

#include "globals.h"
#include "script.h"

HRESULT Script_CallRoutine_PerfMem1_Real( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwScriptName, DWORD dwRoutineIndex, BOOL bFull);

 
HRESULT Script_CallRoutine( LPSTR szRoutine, CtIDirectMusicPerformance8* pPerf, DMUS_SCRIPT_ERRORINFO* pInfo )
{
	HRESULT hr = S_OK;
	
	CtIDirectMusicScript* pScript = NULL;
	CtIDirectMusicLoader8* pLoader = NULL;

	CHECKRUN( dmthCreateLoader( IID_IDirectMusicLoader8, &pLoader ) );
	CHECKALLOC( pLoader );
	CHECKRUN( dmthInitScriptErrorInfo( pInfo ) );
	CHECKRUN( dmthCreateScript( CALLROUTINE_MASTER, pLoader, &pScript ) );
	CHECKALLOC( pScript );
	CHECKRUN( pScript->Init( pPerf, pInfo ) );
	dmthVerifyScriptError( pInfo );

	CHECKRUN( dmthInitScriptErrorInfo( pInfo ) );
	CHECKRUN( pScript->CallRoutine( szRoutine, pInfo ) );
	dmthVerifyScriptError( pInfo );

	RELEASE( pScript );
	RELEASE( pLoader );

	return hr;
}

HRESULT Script_CallRoutine_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

	DMUS_SCRIPT_ERRORINFO dmScriptError;

	DMTEST_EXECUTE( Script_CallRoutine( ROUTINE_GOOD, ptPerf8, NULL ) );
	DMTEST_EXECUTE( Script_CallRoutine( ROUTINE_GOOD, ptPerf8, &dmScriptError ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_CallRoutine( ROUTINE_EMPTY, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_ROUTINE_NOT_FOUND ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_CallRoutine( ROUTINE_NO_EXIST, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_ROUTINE_NOT_FOUND ) );
	DMTEST_EXECUTE( SUCCEED_ON_FAIL( Script_CallRoutine( ROUTINE_RUNTIME_ERROR, ptPerf8, &dmScriptError ), DMUS_E_SCRIPT_ERROR_IN_SCRIPT ) );

	return hr;
}
                                                      

HRESULT Script_CallRoutine_PerfMem1_Partial( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwScriptName, DWORD dwRoutineIndex)
{
    return Script_CallRoutine_PerfMem1_Real(ptPerf8, dwScriptName, dwRoutineIndex, FALSE);
};


HRESULT Script_CallRoutine_PerfMem1_Full( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwScriptName, DWORD dwRoutineIndex)
{
    return Script_CallRoutine_PerfMem1_Real(ptPerf8, dwScriptName, dwRoutineIndex, TRUE);
};


HRESULT Script_CallRoutine_PerfMem1_Real( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwScriptNameServer, DWORD dwRoutineIndex, BOOL bFull)
{
	HRESULT hr = S_OK;	
    DWORD dwLoads = 0;
    CHAR szRoutineName[1000] = {NULL};
    CHAR *szScriptNameServer = (CHAR *)dwScriptNameServer;
    CHAR szScriptName[MAX_PATH] = {0};
	CtIDirectMusicScript* ptScript = NULL;
	CtIDirectMusicLoader8* ptLoader = NULL;
    DWORD dwStartTime = 0, dwTimePassed = 0, dwCounter = 0;
    sprintf(szRoutineName, "Routine%02d", dwRoutineIndex);
    CtIDirectMusicAudioPath *ptPath = NULL;

    CHECKRUN(MediaCopyDirectory(szScriptNameServer));
    strcpy(szScriptName, MediaServerToLocal(szScriptNameServer));

    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, TRUE, &ptPath));
    CHECKRUN(ptPerf8->SetDefaultAudioPath(ptPath));

    if (!bFull)
    {
	    CHECKRUN( dmthCreateLoader( IID_IDirectMusicLoader8, &ptLoader ) );
	    CHECKRUN( dmthCreateScript(szScriptName, ptLoader, &ptScript ) );
	    CHECKALLOC( ptScript );
	    CHECKRUN( ptScript->Init( ptPerf8, NULL) );
    }


    CHECKRUN(Log(FYILOGLEVEL, "Calling %s's %s", PathFindFileName(szScriptName), szRoutineName));
    dwStartTime = timeGetTime();
    do
    {
        dwLoads++;
        if (bFull)
        {
	        CHECKRUN( dmthCreateLoader( IID_IDirectMusicLoader8, &ptLoader ) );
	        CHECKRUN( dmthCreateScript(szScriptName, ptLoader, &ptScript ) );
	        CHECKALLOC( ptScript );
	        CHECKRUN( ptScript->Init( ptPerf8, NULL) );
        }

        CHECKRUN(ptScript->CallRoutine(szRoutineName, NULL));
        if (FAILED(hr))
            Log(ABORTLOGLEVEL, "Attempt #%u to call %s's %s FAILED!!!!", dwLoads, szScriptName, szRoutineName);

        if (bFull)
        {
            SAFE_RELEASE(ptScript);
            SAFE_RELEASE(ptLoader);
        }

        dwTimePassed = timeGetTime() - dwStartTime;
        if (dwTimePassed / 1000 > dwCounter)
        {
            Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
            dwCounter++;
        }

    }
    while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));

    ptPerf8->SetDefaultAudioPath(NULL);
    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptScript);
    SAFE_RELEASE(ptLoader);
    return hr;
};






/********************************************************************************
This separates out the init code so it can be easily called from either inside
or outside the main loop based on a flag.
********************************************************************************/
HRESULT Script_CallRoutine_PerfMem_TestScript_Init(
CtIDirectMusicPerformance*(&ptPerf8),
CtIDirectMusicScript*(&ptScript),
CtIDirectMusicScript*(&ptScriptGlobals),
CtIDirectMusicLoader*(&ptLoader),
CtIDirectMusicAudioPath*(&ptPath),
LPSTR szBasePath,
LPSTR szScriptName
)
{
HRESULT hr = S_OK;


//Init performance
CHECKRUN( ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &ptPath));
CHECKRUN( ptPerf8->SetDefaultAudioPath(ptPath));
CHECKRUN( InsertToolTrace(ptPerf8));

//Init loader
CHECKRUN( dmthCreateLoader( CTIID_IDirectMusicLoader8, &ptLoader ) );
CHECKRUN( ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szBasePath, TRUE));

//Load/Init ScriptGlobals
CHECKRUN( ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, "Globals.spt", (void **)&ptScriptGlobals));
CHECKRUN(ptScriptGlobals->Init( ptPerf8, NULL));

//Load/Init our script
CHECKRUN( ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, szScriptName, (void **)&ptScript));
CHECKRUN(ptScript->Init( ptPerf8, NULL));

return hr;

}

/********************************************************************************
This separates out the cleanup code so it can be easily called from either inside
or outside the main loop based on a flag.
********************************************************************************/
HRESULT Script_CallRoutine_PerfMem_TestScript_Cleanup(
CtIDirectMusicPerformance*(&ptPerf8),
CtIDirectMusicScript*(&ptScript),
CtIDirectMusicScript*(&ptScriptGlobals),
CtIDirectMusicLoader*(&ptLoader),
CtIDirectMusicAudioPath*(&ptPath)
)
{
    //Clean up.
    RemoveToolTrace(ptPerf8);
    ptPerf8->SetDefaultAudioPath(NULL);
    SAFE_RELEASE(ptScriptGlobals);
    SAFE_RELEASE(ptScript);
    SAFE_RELEASE(ptLoader);
    SAFE_RELEASE(ptPath);
    return S_OK;
}



/********************************************************************************
The only difference between these two functions is that they reference content
in different directories.  They both call Script_CallRoutine_Valid_RunScripts
********************************************************************************/
HRESULT Script_CallRoutine_PerfMem_TestScript(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD bFull)
{
	HRESULT hr = S_OK;	
    CHAR  szScriptName[MAX_PATH] = {0};
    CHAR  szRoutineName[MAX_PATH] = {0};
    DWORD dwRangeBottom = 0;
    LONG lRoutineCall = 0;
    LONG lNumRoutines = 0;
    LONG lNextRoutineDelay = 0;
    CtIDirectMusicScript *ptScript          = NULL;
    CtIDirectMusicScript *ptScriptGlobals   = NULL;
    CtIDirectMusicLoader *ptLoader          = NULL;
    CtIDirectMusicAudioPath *ptPath         = NULL;
    DWORD dwStartTime = 0;
    DWORD dwTimePassed = 0;
    DWORD dwCounter = 0;
    LPSTR szBasePath = "T:\\DMTest1\\Script\\Play01\\";

    //We need everything from this directory, AND main1.dls
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Script/Play01/"));
    //Load media.
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"DLS/Main1.DLS", szBasePath, COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));


    //Determine the name of the script (stays the same for the whole function)
    memset((void *)szScriptName, 0, MAX_PATH);
    dwRangeBottom = 0;
    dwRangeBottom = (dwRoutineNumber / 10) * 10;
    CHECK(sprintf(szScriptName, "Routines%03d-%03d.spt", dwRangeBottom, dwRangeBottom + 9));
    CHECK(Log(FYILOGLEVEL, "Script: %s", szScriptName));

    //If we're testing the full load/free scenario, then do this inside the loop.  Otherwise do it here.
    if (!bFull)
        CHECKRUN(Script_CallRoutine_PerfMem_TestScript_Init(ptPerf8, ptScript, ptScriptGlobals, ptLoader, ptPath, szBasePath, szScriptName));

    dwStartTime = timeGetTime();
    do
    {
        //Rethink initialization of variables here. *routinecall, etc)
        lRoutineCall = 0;
        lNumRoutines = 0;

        //If we're fully creating everything every time, then do this inside the loop.
        if (bFull)
            CHECKRUN(Script_CallRoutine_PerfMem_TestScript_Init(ptPerf8, ptScript, ptScriptGlobals, ptLoader, ptPath, szBasePath, szScriptName));

        //Call all the routines.
        do
        {        
            //Reset this since it's set every loop.
            lNextRoutineDelay = 0;

            //Concatenate the RoutineName together.
            memset((void *)szRoutineName, 0, MAX_PATH);
            CHECK(sprintf(szRoutineName, "Routine%03dc%1d", dwRoutineNumber, lRoutineCall));  //i.e. for dwRoutineNumber = 100, dwRoutineCall=3, makes "Routine100c3"
            CHECK(Log(FYILOGLEVEL, "RoutineName: %s", szRoutineName));

            //Call the routine we're supposed to.  If it doesn't exist, the script is messed up.
            CHECK(hr = ptScript->CallRoutine(szRoutineName, NULL));
        
            //Grab the number of routines.
            if (0 == lRoutineCall)
            {
                CHECKRUN(ptScriptGlobals->GetVariableNumber("NumRoutines", &lNumRoutines, NULL));
            }

            CHECKRUN(ptScriptGlobals->GetVariableNumber("NextRoutineDelay", &lNextRoutineDelay, NULL));
            //Don't wait!!
            CHECK(lRoutineCall++);
        }
        while (lRoutineCall < lNumRoutines && SUCCEEDED(hr));

        if (bFull)
            Script_CallRoutine_PerfMem_TestScript_Cleanup(ptPerf8, ptScript, ptScriptGlobals, ptLoader, ptPath);

        //Record the time.
        dwTimePassed = timeGetTime() - dwStartTime;
        if (dwTimePassed / 1000 > dwCounter)
        {
            Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
            dwCounter++;
        }
    }
    while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));


    //Cleanup time!!!
    if (!bFull)
        Script_CallRoutine_PerfMem_TestScript_Cleanup(ptPerf8, ptScript, ptScriptGlobals, ptLoader, ptPath);

    return hr;
};



//these would normally be local, but we're having problems with stack size.
static CHAR  szScriptName[MAX_PATH] = {0};
static CHAR  szRoutineName[MAX_PATH] = {0};


/********************************************************************************
********************************************************************************/
HRESULT Script_CallRoutine_Valid_RunScripts(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected, LPSTR szBasePathServer)
{
	HRESULT hr = S_OK;	
    HRESULT hrCallRoutine = S_OK;
    DWORD dwRangeBottom = 0;
    LONG lRoutineCall = 0;
    LONG lNumRoutines = 0;
    LONG lTestResult = FALSE; //FALSE = FAIL, TRUE = SUCCESS.
    CtIDirectMusicScript *ptScript          = NULL;
    CtIDirectMusicScript *ptScriptGlobals   = NULL;
    CtIDirectMusicLoader *ptLoader          = NULL;
    CtIDirectMusicAudioPath *ptPath         = NULL;
    CHAR szBasePath[MAX_PATH]  = {0};

    strcpy(szBasePath, MediaServerToLocal(szBasePathServer));

    //Copy everything from the base path.
    CHECKRUN(MediaCopyDirectory(szBasePathServer));
    //Load media.
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"DLS/Main1.DLS",                           szBasePath, COPY_IF_NEWER},
    {"DLS/SonicImplants Live Band.dls",         szBasePath, COPY_IF_NEWER},
    {"DLS/SonicImplants Orchestral.dls",        szBasePath, COPY_IF_NEWER},
    {"DLS/SonicImplants SFX.dls",        szBasePath, COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));


    //For processing
    LONG  lNextRoutineDelay = 0;
 
    dwRangeBottom = (dwRoutineNumber / 10) * 10;
    CHECK(sprintf(szScriptName, "Routines%03d-%03d.spt", dwRangeBottom, dwRangeBottom + 9));
    CHECK(Log(FYILOGLEVEL, "Script: %s", szScriptName));
  
    //Init performance
    CHECKRUN( ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &ptPath));
    CHECKRUN( ptPerf8->SetDefaultAudioPath(ptPath));
    CHECKRUN( InsertToolTrace(ptPerf8));
    
    //Init loader
    CHECKRUN( dmthCreateLoader( CTIID_IDirectMusicLoader8, &ptLoader ) );
    CHECKRUN( ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szBasePath, TRUE));

    //Load/Init ScriptGlobals
    CHECKRUN( ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, "Globals.spt", (void **)&ptScriptGlobals));
    CHECKRUN(ptScriptGlobals->Init( ptPerf8, NULL));

    //Load/Init our script
    CHECKRUN( ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, szScriptName, (void **)&ptScript));
    CHECKRUN(ptScript->Init( ptPerf8, NULL));
    do
    {        
        //Concatenate the RoutineName together.
        CHECK(sprintf(szRoutineName, "Routine%03dc%1d", dwRoutineNumber, lRoutineCall));  //i.e. for dwRoutineNumber = 100, dwRoutineCall=3, makes "Routine100c3"
        CHECK(Log(FYILOGLEVEL, "RoutineName: %s", szRoutineName));

        //Call the routine we're supposed to.  If it doesn't exist, the script is messed up.
        CHECK(hrCallRoutine = ptScript->CallRoutine(szRoutineName, NULL));
        
        //If we didn't get the expected HRESULT then set the test's hr correctly.
        if (SUCCEEDED(hr))
        {
            if (hrCallRoutine != hrExpected)
            {
                Log(ABORTLOGLEVEL, "CallRoutine(%s) returned %s (%08X) instead of %s (%08X)", szRoutineName, tdmXlatHRESULT(hrCallRoutine), hrCallRoutine, tdmXlatHRESULT(hrExpected), hrExpected);
                if (S_OK == hrExpected)
                    hr = hrCallRoutine;
                else
                    hr = E_FAIL;
            }
        }

        if (0 == lRoutineCall)
        {
            CHECKRUN(ptScriptGlobals->GetVariableNumber("NumRoutines", &lNumRoutines, NULL));
        }

        CHECKRUN(ptScriptGlobals->GetVariableNumber("NextRoutineDelay", &lNextRoutineDelay, NULL));
        CHECK(lRoutineCall++);
        
        //Skip this if we're just finishing up the last segment.
        if (lRoutineCall != lNumRoutines)
            CHECKRUN(Wait(lNextRoutineDelay));        
    }
    while (lRoutineCall < lNumRoutines && SUCCEEDED(hr) && SUCCEEDED(hrExpected));

    CHECKRUN(ptScriptGlobals->GetVariableNumber("TestResult", &lTestResult, NULL));
    if (lTestResult != 1 && SUCCEEDED(hr) && SUCCEEDED(hrCallRoutine))
    {
        CHECK(Log(ABORTLOGLEVEL, "According to %s's last test routine %s, the test failed.", szScriptName, szRoutineName));
        CHECK(hr = E_FAIL);
    }


    //Stop everything.
    ptPerf8->StopEx(0, 0, 0);

    RemoveToolTrace(ptPerf8);
    ptPerf8->SetDefaultAudioPath(NULL);
    SAFE_RELEASE(ptScriptGlobals);
    SAFE_RELEASE(ptScript);
    SAFE_RELEASE(ptLoader);
    SAFE_RELEASE(ptPath);
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Script_CallRoutine_Valid_TestScript(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected)
{
    return Script_CallRoutine_Valid_RunScripts(ptPerf8, dwRoutineNumber, hrExpected, "DMusic/DMTest1/Script/Play01/");
}


/********************************************************************************
********************************************************************************/
HRESULT Script_CallRoutine_Valid_TestScript_Transition(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected)
{
    return Script_CallRoutine_Valid_RunScripts(ptPerf8, dwRoutineNumber, hrExpected, "DMusic/DMTest1/Script/Play02Trans/");
}


/********************************************************************************
********************************************************************************/
HRESULT Script_CallRoutine_Valid_TestScript_TransSimple(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected)
{
    return Script_CallRoutine_Valid_RunScripts(ptPerf8, dwRoutineNumber, hrExpected, "DMusic/DMTest1/Script/Play02TransSimple/");
}

/********************************************************************************
********************************************************************************/
HRESULT Script_CallRoutine_Valid_TestScript_SegBoundaries(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected)
{
    return Script_CallRoutine_Valid_RunScripts(ptPerf8, dwRoutineNumber, hrExpected, "DMusic/DMTest1/Script/Play03SegBoundaries/");
}

/********************************************************************************
********************************************************************************/
HRESULT Script_CallRoutine_Invalid_TestScript(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRoutineNumber, DWORD hrExpected)
{
    return Script_CallRoutine_Valid_RunScripts(ptPerf8, dwRoutineNumber, hrExpected, "DMusic/DMTest1/Script/Inv_EmbeddedContentMissing/");
}





/********************************************************************************
********************************************************************************/
HRESULT Script_CallRoutine_Valid_PlayOneScript(CtIDirectMusicPerformance8* ptPerf8, DWORD dwScriptServer, DWORD dwRoutine)
{
	HRESULT hr = S_OK;	
    HRESULT a = S_OK;
    DWORD dwRangeBottom = 0;
    LONG lRoutineCall = 0;
    LONG lNumRoutines = 0;
    LONG lTestResult = FALSE; //FALSE = FAIL, TRUE = SUCCESS.
    CtIDirectMusicScript *ptScript          = NULL;
    CtIDirectMusicLoader *ptLoader          = NULL;
    CtIDirectMusicAudioPath *ptPath         = NULL;
    LPSTR szScriptServer = (LPSTR)dwScriptServer;
    CHAR szScript[MAX_PATH] = {0};
    CHAR szDirectory[MAX_PATH] = {0};
    LPSTR szRoutine = (LPSTR)dwRoutine;

    strcpy(szScript, MediaServerToLocal(szScriptServer));
    CHECKRUN(ChopPath(szScript, szDirectory, NULL));

    //Copy everything from the base path.
    CHECKRUN(MediaCopyDirectory(szScriptServer));
    //Load media.
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"DLS/Main1.DLS", szDirectory, COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));

    //For processing
    LONG  lNextRoutineDelay = 0;
 
    //Init performance
    CHECKRUN( ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &ptPath));
    CHECKRUN( ptPerf8->SetDefaultAudioPath(ptPath));
    CHECKRUN( InsertToolTrace(ptPerf8));
    
    //Init loader
    CHECKRUN( dmthCreateLoader( CTIID_IDirectMusicLoader8, &ptLoader ) );
//    CHECKRUN( ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szBasePath, TRUE));

    //Load/Init our script
    CHECKRUN( ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, szScript, (void **)&ptScript));
    CHECKRUN(ptScript->Init( ptPerf8, NULL));

    //Call the routine we're supposed to.  If it doesn't exist, the script is messed up.
    //CHECK(hrCallRoutine = ptScript->CallRoutine(szRoutine, NULL));
    CHECKRUN(ptScript->CallRoutine(szRoutine, NULL));

    Wait(5000);
        

    //Stop everything.
    ptPerf8->StopEx(0, 0, 0);

    RemoveToolTrace(ptPerf8);
    ptPerf8->SetDefaultAudioPath(NULL);
    SAFE_RELEASE(ptScript);
    SAFE_RELEASE(ptLoader);
    SAFE_RELEASE(ptPath);
    return hr;
};

