#pragma once 
#include "globals.h"
//#include "Utils.hpp"


/********************************************************************************
templNULLppvObject

HISTORY:
    Created 08/03/00      danhaff
********************************************************************************/
template <class T>
DWORD templNULLppvObject(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwStage, T* pDoNotUseDoNotRemove = NULL)
{
    HRESULT                     hr              = S_OK;
    CICMusic                    Music;
//    CtIDirectMusicTool     *    ptTool = NULL;
    IUnknown *pUnk      = NULL;
    T* ptInterface = NULL;

    //Init
//    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, "AllStages.aud"));  //TODO: Put this back in!!!
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));  
    CHECKRUN(Music.GetInterface(&ptInterface));
    //CHECKRUN(hr = PutToolInGraphIfNeeded(ptInterface,dwStage,&ptTool,NULL,NULL));

    //First try this and verify we actually get the interface.
    CHECKRUN(ptInterface->GetObjectInPath(IgnorePChannel(dwStage),
                                          dwStage,
                                          0,
                                          GUID_All_Objects,
                                          0,
                                          IID_IUnknown,
                                          (void **)&pUnk
                                          ));

    //Release it
    SAFE_RELEASE(pUnk);

    //Should ASSERT on debug.
    CHECKRUN(ptInterface->GetObjectInPath(IgnorePChannel(dwStage),
                                      dwStage,
                                      0,
                                      GUID_All_Objects,
                                      0,
                                      IID_IUnknown,
                                      NULL
                                      ));

    SAFE_RELEASE(ptInterface);
//    SAFE_RELEASE(ptTool);

    return hr;
};







/********************************************************************************
PURPOSE:
    Verify that passing dwBuffer != 0 for any stage where the buffer doesn't matter,
    results in failures.
********************************************************************************/
/*
template <class T>
HRESULT templBufferNotUsed(CtIDirectMusicPerformance8 *ptPerf8,  
                         DWORD dwStage, 
                         GUID *pCLSID,
                         GUID *pIID,
                         T* pDoNotUseDoNotRemove = NULL)
{
    HRESULT                     hr                  = S_OK;
    CICMusic                    Music;
    T*                          ptInterface         = NULL;
    DWORD                       dwStageIndex        = 0;

    IUnknown                   * pUnk               = NULL;
//    CtIDirectMusicTool         * ptTool             = NULL;
    


    //Start the music playing.
    CHECKRUN(Music.Init(L"test.mid", DMUS_APATH_SHARED_STEREOPLUSREVERB));
    //Get the AudioPath or SegmentState interface.
    CHECKRUN(Music.GetInterface(&ptInterface));
    //CHECKRUN(PutToolInGraphIfNeeded(ptInterface, pStage->dwStage, &ptTool, NULL, NULL));


    //Verify that you can get this object from the path with dwBuffer = 0
    CHECKRUN(ptInterface->GetObjectInPath(IgnorePChannel(pStage->dwStage),   
                                      pStage->dwStage,    
                                      0,                 //dwbuffer= 0
                                      *pStage->pCLSID,    //no CLSID
                                      0,                 //dwIndex = 0
                                      *pStage->pIID,
                                      (void **)&pUnk));
    SAFE_RELEASE(pUnk);
        
    CHECKRUN(ptInterface->GetObjectInPath(IgnorePChannel(pStage->dwStage),   
                                      pStage->dwStage,    
                                      dwBuffer,         //dwbuffer=1
                                      *pStage->pCLSID,  //no CLSID
                                      0,                 //dwIndex = 0
                                      *pStage->pIID,
                                      (void **)&pUnk))


    SAFE_RELEASE(pUnk);
    SAFE_RELEASE(ptInterface);
    return hr;
};

*/



/********************************************************************************
PURPOSE:
    Goes through all the scenarios specified in the Scenarios array and verifies
    they assert
********************************************************************************/
template <class T>
DWORD templInvalidScenarios(CtIDirectMusicPerformance8 *ptPerf8,
                            LPSTR szAudPath,
                            LPSTR szSegment,
                            DWORD dwStage,
                            GUID *pCLSID,
                            GUID *pIID,
                            DWORD dwBuffer,
                            DWORD dwIndex,
                            T* pDoNotUseDoNotRemove = NULL)
{

    HRESULT                         hr                  = S_OK;
    DWORD                           i                   = 0;
//    BOOL                            bSameFilesAsLastTime = FALSE;
//    DWORD                           dwTotalScenarios = 0;
//    WCHAR *                         wszPreviousPath     = L"NULL String";
//    WCHAR *                         wszPreviousSeg      = L"NULL String";
    DWORD                           dwStandardPathType = NULL;


    

    CICMusic                        Music;
    T *                             ptInterface         = NULL;
    IUnknown *                      pUnk                = NULL;
//    CEchoTool               *       pCEchoTool          = NULL;
//    IDirectMusicTool        *       pTool[3]            = {NULL};

    BOOL bFound = FALSE;

    
    //Go check to see if we're creating a standard path type.  If not, dwStandardPathType==FALSE.
    struct PAIR
    {
        LPSTR szString;
        DWORD dwStandardPathType;       
    };
    PAIR Pair[] = { "DMUS_APATH_SHARED_STEREOPLUSREVERB", DMUS_APATH_SHARED_STEREOPLUSREVERB,
                    "DMUS_APATH_DYNAMIC_MONO", DMUS_APATH_DYNAMIC_MONO,
//                    "DMUS_APATH_DYNAMIC_STEREO", DMUS_APATH_DYNAMIC_STEREO,
                    "DMUS_APATH_DYNAMIC_3D", DMUS_APATH_DYNAMIC_3D};
    dwStandardPathType = 0;
    for (i=0; i<AMOUNT(Pair); i++)
    {
        if (strcmp(Pair[i].szString, szAudPath)==0)
        {
            dwStandardPathType = Pair[i].dwStandardPathType;
            break;
        }
    }


    //Create 3 tools and have them just sitting around for our pleasure if we need them.    
    //  We'll delete them at the end of the function.  Other than that, we don't mess
    //  with creating destroying them during the test.

//     for (i=0; i<3; i++)
//     {
//         pCEchoTool = new CEchoTool;
//         if (NULL == pCEchoTool)
//         {
//             fnsLog(ABORTLOGLEVEL,"**** ABORT:  new CEchoTool returned NULL");
//             goto TEST_END;
//         }
// 
//         pCEchoTool->SetDelay(i);
// 
//         //Put this into the correct place.
//         pTool[i] = (IDirectMusicTool *)pCEchoTool;
//         pCEchoTool = NULL;
//     }
//     
// 
// 

    //Determine whether we must Initialize the music class again (this takes time).
//     if (i==0)
//         bSameFilesAsLastTime = FALSE;
//     else if (wcscmp(Scenarios[i].wszPathName, wszPreviousPath) || wcscmp(Scenarios[i].wszSegName, wszPreviousSeg))
//         bSameFilesAsLastTime = FALSE;
//     else
//         bSameFilesAsLastTime = TRUE;    
    //If this scenario needs a different audiopath, then load up the new one.  Otherwise we'll stick with the
//     //  same one.
// 
//     if (!bSameFilesAsLastTime)
//     {
//         //Release the stuff we allocated before.  Music.Shutdown isn't necessary but good form
//         //  to call it.
//         SAFE_RELEASE(ptInterface);
//         Music.Shutdown();
 
 
 
     if (dwStandardPathType)
     {
        CHECKRUN(Music.Init(ptPerf8, szSegment, dwStandardPathType));
    }
    else
    {
        CHECKRUN(Music.Init(ptPerf8, szSegment, szAudPath));
    }
        


    CHECKRUN(Music.GetInterface(&ptInterface));

    //Add EchoTools to all three ToolGraphs (two if we're the audiopath).  Note - this happens
    //  to every instance of CICMusic, even if the test isn't stressing toolgraphs.
//    if (!bAudioPath)
//    {
//        CHECKRUN(AddToolToGraph(ptInterface, DMUS_PATH_SEGMENT_TOOL, pTool[0], NULL, NULL));
//    }
//    CHECKRUN(AddToolToGraph(ptInterface, DMUS_PATH_AUDIOPATH_TOOL, pTool[1], NULL, NULL));
//    CHECKRUN(AddToolToGraph(ptInterface, DMUS_PATH_PERFORMANCE_TOOL, pTool[2], NULL, NULL));


    //Save the file's name.
//    szPreviousPath = szAudPath;
//    szPreviousSeg  = szSegment;

    CHECKRUN(ptInterface->GetObjectInPath(
                                      IgnorePChannel(dwStage),
                                      dwStage,
                                      dwBuffer,
                                      *pCLSID,
                                      dwIndex,
                                      *pIID,
                                      (void **)&pUnk));

    //Should assert here!!!
    // };

     
    //Release this pointer we picked up.
    SAFE_RELEASE(pUnk);    
    SAFE_RELEASE(ptInterface);
    SAFE_RELEASE(pUnk);
// 
//   for (i=0; i<AMOUNT(pTool); i++)
//   {
//       SAFE_RELEASE(pTool[i]);
//   }
// 
    return hr;
};





