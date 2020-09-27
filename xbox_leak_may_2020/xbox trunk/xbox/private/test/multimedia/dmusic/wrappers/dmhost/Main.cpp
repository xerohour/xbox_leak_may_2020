#include <globals.h>
#include "trans.h"

//Why the fuck isn't this getting picked up?  We need to consolidate our utils and globals.h files.
void DbgPrintGUID (REFIID riid);
//void _stdcall LogDMUS_OBJECTDESCStruct(DMUS_OBJECTDESC Desc);

HRESULT Process_Create_IDirectMusicPerformance8(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_Create_IDirectMusicLoader8(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_Create_IDirectMusicSegment8(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_IDirectMusicPerformance8_InitAudio(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_IDirectMusicLoader8_LoadObjectFromFile(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_IDirectMusicLoader8_SetObject(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_IDirectMusicSegment8_Download(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_IDirectMusicSegment8_Unload(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_IDirectMusicPerformance8_PlaySegmentEx(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_IDirectMusicPerformance8_CloseDown(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_IUnknown_Release(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);
HRESULT Process_IUnknown_AddRef(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks);


/******************************************************************************************
******************************************************************************************/
HRESULT CheckParamCount(char *szFunctionName, DWORD dwParamsExpected, DWORD dwParamsActual)
{
    //Verify number of params
    if (dwParamsExpected != dwParamsActual)
    {
        DbgPrint("%s Error - %d parameters instead of %d\n", szFunctionName, dwParamsActual, dwParamsExpected);
        return E_FAIL;
    }
    else
        return S_OK;
   
};



//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Plays a single wave file using DirectMusic on the default audio path.
//-----------------------------------------------------------------------------
//void __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow ) 
void __cdecl main(void)
{
    TRANS *pTrans = NULL;
    HRESULT hr = S_OK;
    char szMethodName[SIZE_METHODNAME];
    DWORD dwParamCount = 0;
    LPDATABLOCK *ppDataBlocks = NULL;
    LPVOID pvThis = NULL;
    
    pTrans = new TRANS;
    hr = pTrans->XBox_Init();
    DbgPrint("\n--------------------------\n\n DMHost!!\n\n");

    
    while (SUCCEEDED(hr))
    {
        
        //Wait for the next set of parameters and then open the file for reading.
        hr = pTrans->XBox_ReadParameterDataFile();

        //Read the file header to check out the method name and param count.
        hr = pTrans->ReadMainBlockHeader(&pvThis, szMethodName, &dwParamCount);

        //Get the parameters, if any.
        hr = pTrans->ReadParameters(dwParamCount, &ppDataBlocks);

        //Close the data file.
        hr = pTrans->CloseDataFile();



        //TODO: Make an array of functions.
        //Depending on the methodname, we process the in and out parameters with a different function.
        if (strcmp(szMethodName, "Create_IDirectMusicPerformance8")==0)
        {
            hr = Process_Create_IDirectMusicPerformance8(pTrans, dwParamCount, ppDataBlocks);
        }
        if (strcmp(szMethodName, "IDirectMusicPerformance8::CloseDown")==0)
        {
            hr = Process_IDirectMusicPerformance8_CloseDown(pTrans, dwParamCount, ppDataBlocks);
        }
        else if (strcmp(szMethodName, "Create_IDirectMusicLoader8")==0)
        {
            hr = Process_Create_IDirectMusicLoader8(pTrans, dwParamCount, ppDataBlocks);
        }
        else if (strcmp(szMethodName, "Create_IDirectMusicSegment8")==0)
        {
            hr = Process_Create_IDirectMusicSegment8(pTrans, dwParamCount, ppDataBlocks);
        }
        else if (strcmp(szMethodName, "IDirectMusicPerformance8::InitAudio")==0)
        {
            hr = Process_IDirectMusicPerformance8_InitAudio(pTrans, dwParamCount, ppDataBlocks);
        }
        else if (strcmp(szMethodName, "IDirectMusicPerformance8::PlaySegmentEx")==0)
        {
            hr = Process_IDirectMusicPerformance8_PlaySegmentEx(pTrans, dwParamCount, ppDataBlocks);
        }

        else if (strcmp(szMethodName, "IDirectMusicLoader8::LoadObjectFromFile")==0)
        {
            hr = Process_IDirectMusicLoader8_LoadObjectFromFile(pTrans, dwParamCount, ppDataBlocks);
        }
        else if (strcmp(szMethodName, "IDirectMusicLoader8::SetObject")==0)
        {
            hr = Process_IDirectMusicLoader8_SetObject(pTrans, dwParamCount, ppDataBlocks);
        }
        else if (strcmp(szMethodName, "IDirectMusicSegment8::Download")==0)
        {
            hr = Process_IDirectMusicSegment8_Download(pTrans, dwParamCount, ppDataBlocks);
        }
        else if (strcmp(szMethodName, "IDirectMusicSegment8::Unload")==0)
        {
            hr = Process_IDirectMusicSegment8_Unload(pTrans, dwParamCount, ppDataBlocks);
        }
        else if (strcmp(szMethodName, "IUnknown::Release")==0)
        {
            hr = Process_IUnknown_Release(pTrans, dwParamCount, ppDataBlocks);
        }
        
        
        //Default
        else
        {
            DbgPrint("Error - transaction type %s not found", szMethodName);
        }

    }

    DbgPrint("DMHost exiting.\n");
}



/******************************************************************************************
******************************************************************************************/
HRESULT Process_Create_IDirectMusicPerformance8(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IDirectMusicPerformance8 *pPerf8 = NULL;

    //Verify there are no parameters.
    CheckParamCount("Process_Create_IDirectMusicPerformance8", 0, dwParamCount);

    // Create Performance object
    DbgPrint("About to create DirectMusicPerformance.\n");
    hrCall = DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL, IID_IDirectMusicPerformance8, (void**)&pPerf8);

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //Two parameters - The return value (always first) and the new pointer!
    hr = pTrans->WriteMainBlockHeader(NULL,"Create_IDirectMusicPerformance8", 2);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);

    //----- PARAM BLOCK 2: The pointer out-param
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(IDirectMusicPerformance8 *), (void *)&pPerf8);

    DbgPrint("Create_IDirectMusicPerformance8() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("pPerformance8 = %08X\n", pPerf8);
    DbgPrint("hr = %08X\n", hrCall);
    DbgPrint("====================\n");


    //Close the data file.
    hr = pTrans->CloseDataFile();

    return hr;
};



/******************************************************************************************
******************************************************************************************/
HRESULT Process_Create_IDirectMusicLoader8(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IDirectMusicLoader8 *pLoader8 = NULL;

    //Verify there are no parameters.
    CheckParamCount("Process_Create_IDirectMusicLoader8", 0, dwParamCount);

    // Create Performance object
    DbgPrint("About to create DirectMusicLoader8");
    hrCall = DirectMusicCreateInstance( CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader8, (void**)&pLoader8);

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //Two parameters - The return value (always first) and the new pointer!
    hr = pTrans->WriteMainBlockHeader(NULL,"Create_IDirectMusicLoader8", 2);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);

    //----- PARAM BLOCK 2: The pointer out-param
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(IDirectMusicLoader8 *), (void *)&pLoader8);

    DbgPrint("Create_IDirectMusicLoader8() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("pLoader8 = %08X\n", pLoader8);
    DbgPrint("hr = %08X\n", hrCall);
    DbgPrint("====================\n");

    //Close the data file.
    hr = pTrans->CloseDataFile();


    //BUGBUG: There's got to be a better way to do this
    DbgPrint("Note: Calling pLoader8->SetSearchDirectory t:\\trans automatically. \n");
    hr = pLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, "T:\\Trans", FALSE);
    DbgPrint("Create_IDirectMusicLoader8::SetSearchDirectory() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("hr = %08X\n", hr);
    DbgPrint("====================\n");

    return hr;
};




/******************************************************************************************
******************************************************************************************/
HRESULT Process_Create_IDirectMusicSegment8(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IDirectMusicSegment8 *pSegment8 = NULL;

    //Verify there are no parameters.
    CheckParamCount("Process_Create_IDirectMusicSegment8", 0, dwParamCount);

    // Create Performance object
    DbgPrint("About to create DirectMusicSegment8.\n");
    hrCall = DirectMusicCreateInstance( CLSID_DirectMusicSegment, NULL, IID_IDirectMusicSegment8, (void**)&pSegment8);

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //Two parameters - The return value (always first) and the new pointer!
    hr = pTrans->WriteMainBlockHeader(NULL,"Create_IDirectMusicSegment8", 2);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);

    //----- PARAM BLOCK 2: The pointer out-param
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(IDirectMusicSegment8 *), (void *)&pSegment8);

    DbgPrint("Create_IDirectMusicSegment8() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("pSegment8 = %08X\n", pSegment8);
    DbgPrint("hr = %08X\n", hrCall);
    DbgPrint("====================\n");


    //Close the data file.
    hr = pTrans->CloseDataFile();

    return hr;
};





/******************************************************************************************
******************************************************************************************/
HRESULT Process_IDirectMusicLoader8_LoadObjectFromFile(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IDirectMusicLoader8 *pLoader8 = NULL;
GUID guidClassID = {0};
GUID iidInterfaceID = {0};
CHAR *szFilePath = NULL;
LPVOID pObject = NULL;
CHAR szRealFilePath[MAX_PATH] = {0};


    hr = CheckParamCount("Process_IDirectMusicLoader8_LoadObjectFromFile", 4, dwParamCount);


    //Get our paramters.  The "this" pointer is always the first one.

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    pLoader8            = *(IDirectMusicLoader8 **)      ppDataBlocks[0][0].pvData;
    guidClassID         = *(GUID *)                      ppDataBlocks[1][0].pvData;
    iidInterfaceID      = *(GUID *)                      ppDataBlocks[2][0].pvData;
    szFilePath         =  *(CHAR **)                     ppDataBlocks[3][0].pvData;


    // Make the call.
    DbgPrint("IDirectMusicLoader8->LoadObjectFromFile() InParams\n");
    DbgPrint("--------------------------------------------------\n");
    DbgPrint("this = %08X\n", pLoader8);
    DbgPrint("guidClassID = ");
    DbgPrintGUID(guidClassID);
    DbgPrint("\niidInterfaceID = ");
    DbgPrintGUID(iidInterfaceID);
    DbgPrint("\nszFilePath = %s\n", szFilePath ? szFilePath : "NULL");

    if (szFilePath)
    {
        sprintf(szRealFilePath, "T:\\Trans\\%s", szFilePath);
        DbgPrint("Changing szFilePath (%s) to %ls\n", szFilePath, szRealFilePath);
    }

    //Make file call.
    hrCall = pLoader8->LoadObjectFromFile(guidClassID, iidInterfaceID, szFilePath ? szRealFilePath : NULL, &pObject);

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //Two parameters - the return value.
    hr = pTrans->WriteMainBlockHeader(NULL,"Create_IDirectMusicLoader8", 2);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);

    //----- PARAM BLOCK 2: The created object.
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(LPVOID), (void *)&pObject);

    DbgPrint("IDirectMusicLoader8->LoadObjectFromFile() OutParams\n");
    DbgPrint("--------------------------------------------------\n");
    DbgPrint("hr = %08X\n", hrCall);
    DbgPrint("pObject = %08X\n", pObject);
    DbgPrint("===================================================\n");

    //Close the data file.
    hr = pTrans->CloseDataFile();
    return hr;

};




/******************************************************************************************
******************************************************************************************/
HRESULT Process_IDirectMusicPerformance8_InitAudio(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IDirectMusicPerformance8 *pPerf8 = NULL;
LPVOID *ppv = NULL;
DWORD dwDefaultPathType;
DWORD dwPChannelCount;


    //Verify number of params
    hr = CheckParamCount("Process_IDirectMusicPerformance8_InitAudio", 5, dwParamCount);

    //Get our paramters.  The "this" pointer is always the first one.

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    pPerf8            = *(IDirectMusicPerformance8 **)   ppDataBlocks[0][0].pvData;
    dwDefaultPathType = *(DWORD *)                       ppDataBlocks[1][0].pvData;
    dwPChannelCount   = *(DWORD *)                       ppDataBlocks[2][0].pvData;

    // Make the call.
    DbgPrint("InitAudio() InParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("dwDefaultPathType = %d\n", dwDefaultPathType);
    DbgPrint("dwPChannelCount = %d\n", dwPChannelCount);

    hrCall = pPerf8->InitAudioX(dwDefaultPathType, dwPChannelCount);

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //One parameter - the return value.
    hr = pTrans->WriteMainBlockHeader(NULL,"Create_IDirectMusicPerformance8", 1);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);
    DbgPrint("InitAudio() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("hr = %08X\n", hrCall);
    DbgPrint("====================\n");


    //Close the data file.
    hr = pTrans->CloseDataFile();

    return hr;


};




/******************************************************************************************
******************************************************************************************/
HRESULT Process_IDirectMusicSegment8_Download(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IDirectMusicSegment8 *pSegment8 = NULL;
IUnknown *pUnk;
LPVOID *ppv = NULL;


    //Verify number of params
    hr = CheckParamCount("Process_IDirectMusicSegment8_Download", 2, dwParamCount);

    //Get our paramters.  The "this" pointer is always the first one.

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    pSegment8            = *(IDirectMusicSegment8 **)   ppDataBlocks[0][0].pvData;
    pUnk                 = *(IUnknown **)               ppDataBlocks[1][0].pvData;

    // Make the call.
    DbgPrint("IDirectMusicSegment::Download() InParams\n");
    DbgPrint("----------------------------------------\n");
    DbgPrint("this = %08X\n", pSegment8);
    DbgPrint("pAudioPath = %08X\n", pUnk);

    hrCall = pSegment8->Download(pUnk);

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //One parameter - the return value.
    hr = pTrans->WriteMainBlockHeader(NULL,"Create_IDirectMusicSegment8", 1);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);
    DbgPrint("IDirectMusicSegment::Download() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("hr = %08X\n", hrCall);
    DbgPrint("====================\n");


    //Close the data file.
    hr = pTrans->CloseDataFile();

    return hr;


};


/******************************************************************************************
******************************************************************************************/
HRESULT Process_IDirectMusicSegment8_Unload(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IDirectMusicSegment8 *pSegment8 = NULL;
IUnknown *pUnk;
LPVOID *ppv = NULL;


    //Verify number of params
    hr = CheckParamCount("Process_IDirectMusicSegment8_Unload", 2, dwParamCount);

    //Get our paramters.  The "this" pointer is always the first one.

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    pSegment8            = *(IDirectMusicSegment8 **)   ppDataBlocks[0][0].pvData;
    pUnk                 = *(IUnknown **)               ppDataBlocks[1][0].pvData;

    // Make the call.
    DbgPrint("IDirectMusicSegment::Unload() InParams\n");
    DbgPrint("----------------------------------------\n");
    DbgPrint("this = %08X\n", pSegment8);
    DbgPrint("pAudioPath = %08X\n", pUnk);

    hrCall = pSegment8->Unload(pUnk);

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //One parameter - the return value.
    hr = pTrans->WriteMainBlockHeader(NULL,"Create_IDirectMusicSegment8", 1);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);
    DbgPrint("IDirectMusicSegment::Unload() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("hr = %08X\n", hrCall);
    DbgPrint("====================\n");


    //Close the data file.
    hr = pTrans->CloseDataFile();

    return hr;


};




/******************************************************************************************
******************************************************************************************/
HRESULT Process_IDirectMusicPerformance8_PlaySegmentEx(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
LPVOID *ppv = NULL;

//OutParams
IDirectMusicSegmentState* pSegmentState = NULL; 

//InParams
IDirectMusicPerformance8 *pPerf8 = NULL;
IUnknown* pSource = NULL;
CHAR *pzSegmentName = NULL;
IUnknown* pTransition = NULL;
DWORD dwFlags = NULL;
__int64 i64StartTime = NULL;
IDirectMusicSegmentState **ppSegmentState;  //Only for input purposes, not gonna receive any data.
IUnknown *pFrom = NULL;
IUnknown *pAudioPath = NULL;


    //Verify number of params
    hr = CheckParamCount("Process_IDirectMusicPerformance8_PlaySegmentEx", 9, dwParamCount);


    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    pPerf8            = *(IDirectMusicPerformance8 **)   ppDataBlocks[0][0].pvData;
    pSource           = *(IUnknown **)                   ppDataBlocks[1][0].pvData;
    pzSegmentName    =  *(CHAR **)                       ppDataBlocks[2][0].pvData;
    pTransition       = *(IUnknown **)                   ppDataBlocks[3][0].pvData;
    dwFlags           = *(DWORD *)                       ppDataBlocks[4][0].pvData;
    i64StartTime      = *(__int64 *)                     ppDataBlocks[5][0].pvData;
    ppSegmentState    = *(IDirectMusicSegmentState ***)  ppDataBlocks[6][0].pvData;
    pFrom             = *(IUnknown **)                   ppDataBlocks[7][0].pvData;
    pAudioPath        = *(IUnknown **)                   ppDataBlocks[8][0].pvData;


    // Make the call.
    DbgPrint("PlaySegmentEx() InParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("pPerf8            = %08X\n", pPerf8);
    DbgPrint("pSource           = %08X\n", pSource);
    DbgPrint("pzSegmentName     = %s\n",   pzSegmentName);
    DbgPrint("pTransition       = %08X\n", pTransition);
    DbgPrint("dwFlags           = %08X\n", dwFlags);
    DbgPrint("i64StartTime      = %08X\n", i64StartTime);
    DbgPrint("ppSegmentState             = %08X\n", ppSegmentState);
    DbgPrint("pFrom             = %08X\n", pFrom);
    DbgPrint("pAudioPath        = %08X\n", pAudioPath);


    
    hrCall = pPerf8->PlaySegmentEx(pSource,                  // Segment to play. Alternately, could be an IDirectMusicSong.
                                   pzSegmentName,            // If song, which segment in the song.
                                   pTransition,              // Optional template segment to compose transition with.
                                   dwFlags,                  // DMUS_SEGF_ flags.
                                   i64StartTime,             // Time to start playback.
                                   ppSegmentState ? &pSegmentState : NULL,           // Returned Segment State.
                                   pFrom,                    // Optional segmentstate or audiopath to replace.
                                   pAudioPath)               ;

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //Two out-parameters - the return value and the segment state.
    hr = pTrans->WriteMainBlockHeader(NULL,"Create_IDirectMusicPerformance8", 2);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);
    //----- PARAM BLOCK 2: The segment state.
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(pSegmentState), (void *)&pSegmentState);

    //Debug output.
    DbgPrint("PlaySegmentEx() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("hr = %08X\n", hrCall);
    DbgPrint("*ppSegmentState = %08X\n", pSegmentState);
    DbgPrint("====================\n");

    //Close the data file.
    hr = pTrans->CloseDataFile();

    return hr;


};



/******************************************************************************************
******************************************************************************************/
HRESULT Process_IDirectMusicLoader8_SetObject(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IDirectMusicLoader8 *pLoader8 = NULL;
LPDMUS_OBJECTDESC pDesc = NULL;
//CHAR szRealFilePath[MAX_PATH] = {0};


    hr = CheckParamCount("Process_IDirectMusicLoader8_SetObject", 2, dwParamCount);


    //Get our paramters.  The "this" pointer is always the first one.

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    pLoader8            = *(IDirectMusicLoader8 **)      ppDataBlocks[0][0].pvData;
    pDesc               = *(LPDMUS_OBJECTDESC    *)      ppDataBlocks[1][0].pvData;

    // Make the call.
    DbgPrint("IDirectMusicLoader8->SetObject() InParams\n");
    DbgPrint("----------------------------------------------\n");
    DbgPrint("this = %08X\n", pLoader8);
    DbgPrint("pDesc = %08X\n", pDesc);
    if (pDesc)
    {
//        LogDMUS_OBJECTDESCStruct(*pDesc);
    }
/*
    if (pDesc->wszFileName)
    {
        sprintf(wszRealFilePath, "T:\\Trans\\%s", pDesc->wszFileName);
        DbgPrint("Changing wszFilePath (%s) to %s\n", pDesc->wszFileName, wszRealFilePath);
        wcscpy(pDesc->wszFileName, wszRealFilePath);
    }
*/

    //Make file call.
    hrCall = pLoader8->SetObject(pDesc);

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //Two parameters - the return value.
    hr = pTrans->WriteMainBlockHeader(NULL,"Create_IDirectMusicLoader8", 1);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);


    DbgPrint("IDirectMusicLoader8->SetObject() OutParams\n");
    DbgPrint("--------------------------------------------------\n");
    DbgPrint("hr = %08X\n", hrCall);
    DbgPrint("===================================================\n");

    //Close the data file.
    hr = pTrans->CloseDataFile();
    return hr;

};




/******************************************************************************************
******************************************************************************************/
HRESULT Process_IUnknown_AddRef(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IUnknown *pUnk = NULL;
DWORD dwRet = 0;

    //Verify there is a single parameter.
    CheckParamCount("Process_IUnknown_AddRef", 1, dwParamCount);

    //Get the pointer.
    pUnk = *(IUnknown **) ppDataBlocks[0][0].pvData;

    // Make the call.
    DbgPrint("About to AddRef an IUnknown.\n");
    dwRet = pUnk->AddRef();

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //One parameter - The return value 
    hr = pTrans->WriteMainBlockHeader(NULL,"IUnknown::AddRef", 1);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(DWORD), (void *)&dwRet);


    DbgPrint("IUnknown::AddRef() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("return = %08X\n", dwRet);
    DbgPrint("====================\n");


    //Close the data file.
    hr = pTrans->CloseDataFile();

    return hr;
};


/******************************************************************************************
******************************************************************************************/
HRESULT Process_IUnknown_Release(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IUnknown *pUnk = NULL;
DWORD dwRet = 0;

    //Verify there is a single parameter.
    CheckParamCount("Process_IUnknown_Release", 1, dwParamCount);

    //Get the pointer.
    pUnk = *(IUnknown **) ppDataBlocks[0][0].pvData;

    // Make the call.
    DbgPrint("About to Release an IUnknown.\n");
    dwRet = pUnk->Release();

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //One parameter - The return value 
    hr = pTrans->WriteMainBlockHeader(NULL,"IUnknown::Release", 1);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(DWORD), (void *)&dwRet);


    DbgPrint("IUnknown::Release() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("return = %08X\n", dwRet);
    DbgPrint("====================\n");


    //Close the data file.
    hr = pTrans->CloseDataFile();

    return hr;
};



/******************************************************************************************
******************************************************************************************/
HRESULT Process_IDirectMusicPerformance8_CloseDown(TRANS *pTrans, DWORD dwParamCount, DATABLOCK **ppDataBlocks)
{
HRESULT hr = S_OK;
HRESULT hrCall = S_OK;
IDirectMusicPerformance8 *pPerf8 = NULL;
DWORD dwRet = 0;

    //Verify there are no parameters.
    CheckParamCount("Process_IDirectMusicPerformance8_Release", 0, dwParamCount);

    //Get the pointer.
    pPerf8 = *(IDirectMusicPerformance8 **) ppDataBlocks[0][0].pvData;

    // Make the call.
    DbgPrint("About to call IDirectMusicPerformance8::CloseDown()\n");
//    hrCall = pPerf8->CloseDown();

    //Create the out-params.
    hr = pTrans->XBox_CreateReturnedDataFile();

    //One parameter - The return value 
    hr = pTrans->WriteMainBlockHeader(NULL,"IDirectMusicPerformance8::Release", 1);

    //----- PARAM BLOCK 1: The return value
    hr = pTrans->WriteParamBlockHeader(1);
    hr = pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(HRESULT), (void *)&hrCall);


    DbgPrint("IDirectMusicPerformance8::Release() OutParams\n");
    DbgPrint("--------------------\n");
    DbgPrint("hr = %08X\n", hrCall );
    DbgPrint("====================\n");


    //Close the data file.
    hr = pTrans->CloseDataFile();

    return hr;
};



