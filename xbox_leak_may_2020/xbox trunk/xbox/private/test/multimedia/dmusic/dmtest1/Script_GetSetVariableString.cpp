/********************************************************************************
	FILE:
		Script_GetSetVariableString.cpp

	PURPOSE:
		GetVariableString and SetVariableString tests for scripts

	BY:
		DANHAFF
********************************************************************************/

#include "globals.h"
#include "script.h"


/********************************************************************************
********************************************************************************/
HRESULT Script_SetVariableString_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;	
    HRESULT hrCallRoutine = S_OK;
    CtIDirectMusicScript *ptScript          = NULL;
    CtIDirectMusicLoader *ptLoader          = NULL;
    CtIDirectMusicAudioPath *ptPath         = NULL;
    CHAR szString[255] = {0};
    LPSTR szScriptServer = "DMusic/DMTest1/Script/GetSetVariableString/GetSetVariableString.spt";
    CHECKRUN(MediaCopyFile(szScriptServer));
    

    CHECKRUN( ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &ptPath));
    CHECKRUN( ptPerf8->SetDefaultAudioPath(ptPath));
    CHECKRUN( InsertToolTrace(ptPerf8));
    
    //Init loader
    CHECKRUN( dmthCreateLoader( CTIID_IDirectMusicLoader8, &ptLoader ) );

    //Load/Init our script
    CHECKRUN( ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, MediaServerToLocal(szScriptServer), (void **)&ptScript));
    CHECKRUN(ptScript->Init( ptPerf8, NULL));
    CHECKRUN(ptScript->CallRoutine("InitializeBeforeTest", NULL));

    //Call the routine we're supposed to.
    CHECKRUN(ptScript->CallRoutine("InitializeBeforeTest", NULL));
    CHECKRUN(ptScript->SetVariableString("ScriptString", "", NULL));
    CHECKRUN(ptScript->CallRoutine("Receive_01_Nothing", NULL));

    CHECKRUN(ptScript->CallRoutine("InitializeBeforeTest", NULL));
    CHECKRUN(ptScript->SetVariableString("ScriptString", "a", NULL));
    CHECKRUN(ptScript->CallRoutine("Receive_02_OneLetter", NULL));
 
    CHECKRUN(ptScript->CallRoutine("InitializeBeforeTest", NULL));
    CHECKRUN(ptScript->SetVariableString("ScriptString", "LongString0010_6789_0020_6789_0030_6789_0040_6789_0050_6789_0060_6789_0070_6789_0080_6789_0090_6789_0100_6789_0110_6789_0120_6789_0130_6789_0140_6789_0150_6789_0160_6789_0170_6789_0180_6789_0190_6789_0200_6789_0210_6789_0220_6789_0230_6789_0240_6789_0250_", NULL));
    CHECKRUN(ptScript->CallRoutine("Receive_03_LongString", NULL));

    for (int i=0; i<255; i++)
        szString[i] = i+1;
    CHECKRUN(ptScript->SetVariableString("ScriptString", szString, NULL));
    CHECKRUN(ptScript->CallRoutine("Receive_04_AllCharacters", NULL));
  

    Log(ABORTLOGLEVEL, "Check the output to make sure everything was printed okay");
    Wait(4000);  
  
        
    RemoveToolTrace(ptPerf8);
    ptPerf8->SetDefaultAudioPath(NULL);
    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptScript);
    SAFE_RELEASE(ptLoader);
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Script_GetVariableString_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;	
    HRESULT hrCallRoutine = S_OK;
    CtIDirectMusicScript *ptScript          = NULL;
    CtIDirectMusicLoader *ptLoader          = NULL;
    CtIDirectMusicAudioPath *ptPath         = NULL;
    CHAR szString[256] = {0};
    CHAR *szScriptServer = "DMusic/DMTest1/Script/GetSetVariableString/GetSetVariableString2.spt";
    CHAR *szStrings     [4] = {"", "a", "0000_6789^0010_6789^0020_6789^0030_6789^0040_6789^0050_6789^0060_6789^0070_6789^0080_6789^0090_6789^0100_6789^0110_6789^0120_6789^0130_6789^0140_6789^0150_6789^0160_6789^0170_6789^0180_6789^0190_6789^0200_6789^0210_6789^0220_6789^0230_6789^0240_6789^0250^", "Do you like movies about gladiators?"};
    CHAR *szRoutineNames[4] = {"Send_01_Nothing", "Send_02_OneLetter", "Send_03_LongString", "Send_04_LongStringName"};
    CHAR *szVarNames    [4] = {"ScriptString", "ScriptString", "ScriptString", "LongStringName_6789_0020_6789_0030_6789_0040_6789_0050_6789_0060_6789_0070_6789_0080_6789_0090_6789_0100_6789_0110_6789_0120_6789_0130_6789_0140_6789_0150_6789_0160_6789_0170_6789_0180_6789_0190_6789_0200_6789_0210_6789_0220_6789_0230_6789_0240_6789_0250_"};
    DWORD i=0;



    LONG lConverted = 0;
    BOOL bPassing = TRUE;

    CHECKRUN(MediaCopyDirectory(szScriptServer));
    CHECKRUN( ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &ptPath));
    CHECKRUN( ptPerf8->SetDefaultAudioPath(ptPath));
    CHECKRUN( InsertToolTrace(ptPerf8));
    
    //Init loader
    CHECKRUN( dmthCreateLoader( CTIID_IDirectMusicLoader8, &ptLoader ) );

    //Load/Init our script
    CHECKRUN( ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, (CHAR *)MediaServerToLocal(szScriptServer), (void **)&ptScript));
    CHECKRUN(ptScript->Init( ptPerf8, NULL));

    //Go through these three tests.    
    for (i=1; i<4 && SUCCEEDED(hr); i++)
    {
        CHECKRUN(ptScript->CallRoutine("InitializeBeforeTest", NULL));
    
        //Call the routine we're supposed to.
        CHECKRUN(ptScript->CallRoutine(szRoutineNames[i], NULL));
        CHECKRUN(ptScript->GetVariableString(szVarNames[i], szString, 256, &lConverted, NULL));
        if (strcmp(szString, szStrings[i]))
        {
            bPassing = FALSE;
            Log(ABORTLOGLEVEL, "Error (iteration %d): GetVariableString returned [%s] instead of [%s]", i, szString, szStrings[i]);
        }
        if (lConverted != strlen(szStrings[i]) + 1)
        {
            bPassing = FALSE;
            Log(ABORTLOGLEVEL, "Error (iteration %d): GetVariableString returned length of %d instead of %d", i, strlen(szString), strlen(szStrings[i]));
        }
    }

    RemoveToolTrace(ptPerf8);
    ptPerf8->SetDefaultAudioPath(NULL);
    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptScript);
    SAFE_RELEASE(ptLoader);
    return hr;
};
