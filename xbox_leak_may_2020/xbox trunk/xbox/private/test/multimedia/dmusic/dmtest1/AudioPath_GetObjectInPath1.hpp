#pragma once
 
#include "globals.h"
#include "Help_Definitions.h"
#include "cicmusicx.h"

/********************************************************************************
********************************************************************************/
template <class T>
HRESULT tmplGetObjectInPath(CtIDirectMusicPerformance8 *ptPerf8,                          
                          DWORD dwPathType,
                          DWORD dwStage,
                          GUID *pguidObject,
                          GUID *pguidInterface,
                          T* pDoNotUseDoNotRemove = NULL)
{
	HRESULT						hr				    = S_OK;
    void                        *pVoid              = NULL;    
    IUnknown                    *pUnknown           = NULL;
    IDirectSoundBuffer          *pDSB               = NULL;
    T                           *ptTestInterface    = NULL;
    CICMusic                    *pMusic             = NULL;   
    DWORD                        dwBuffer           = 0;

    
    ALLOCATEANDCHECK(pMusic, CICMusic);
    CHECKRUN(pMusic->Init(ptPerf8, g_TestParams.szDefaultMedia, dwPathType));
    CHECKRUN(pMusic->GetInterface(&ptTestInterface));
    dwBuffer = 0;
    
    // Get the object out of the audio path.
    CHECKRUN(ptTestInterface->GetObjectInPath(
            IgnorePChannel(dwStage),   
            dwStage,           
            dwBuffer,          
            *pguidObject,    
            0,               //dwIndex
            *pguidInterface, 
            (void **)&pVoid));

    //Wait a sec, to catch any mixer problems.
    CHECKRUN(Wait(1000));

  
    //TODO: Call the last method on each interface.  This test sucks, write another one.


    //Verify it's the correct type by QI'ing
    if (dwStage == DMUS_PATH_BUFFER)
    {
        CHECK((pDSB = (IDirectSoundBuffer *)pVoid));
    }
    else
    {
        CHECK((pUnknown = (IUnknown *)pVoid));
    }

    SAFE_RELEASE(pUnknown);
    SAFE_RELEASE(pDSB);
    SAFE_RELEASE(ptTestInterface);
    SAFEDELETE(pMusic);
	return hr;
}	



/********************************************************************************
********************************************************************************/
template <class T>
HRESULT tmplTraverseStandard(CtIDirectMusicPerformance8 *ptPerf8,
                          DWORD dwStandardPath,
                          PATH_DEFINITION *pDefinition,
                          T* pDoNotUseDoNotRemove = NULL)
{

    CICMusic Music;
    T *ptInterface = NULL;
    HRESULT hr = S_OK;

    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, dwStandardPath));
    CHECKRUN(Music.GetInterface(&ptInterface));
    CHECKRUN(VerifyPathData(pDefinition, ptInterface));

    SAFE_RELEASE(ptInterface);
    return hr;

}



/********************************************************************************
********************************************************************************/
template <class T>
HRESULT tmplGetBuffer(CtIDirectMusicPerformance8 *ptPerf8,
                     DWORD dwStandardPath,
                     T* pDoNotUseDoNotRemove = NULL)
{

    CICMusic Music;
    T *ptInterface = NULL;
    HRESULT hr = S_OK;
    IDirectSoundBuffer *pBuffer = NULL;
    DWORD dwStatus = 0;

    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, dwStandardPath));
    Log(FYILOGLEVEL, "Playing music normal for 3 seconds");
    CHECKRUN(Wait(3000));
    CHECKRUN(Music.GetInterface(&ptInterface));
    CHECKRUN(ptInterface->GetObjectInPath(DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, GUID_All_Objects, 0, GUID_NULL, (void **)&pBuffer));
    Log(FYILOGLEVEL, "Setting volume to -1000 on AudioPath buffer for 3 seconds");
    CHECKRUN(pBuffer->SetVolume(-1000));
    CHECKRUN(Wait(3000));
    Log(FYILOGLEVEL, "Setting frequency to 50000 on audioPath buffer for 3 seconds");
    CHECKRUN(pBuffer->SetVolume(0));
    CHECKRUN(pBuffer->SetFrequency(50000));
    CHECKRUN(Wait(3000));

    Log(FYILOGLEVEL, "Calling other random APIs on buffer");
    CHECKRUN(pBuffer->SetPosition(0, 0, 0, DS3D_IMMEDIATE));
    CHECKRUN(pBuffer->SetVelocity(0, 0, 0, DS3D_IMMEDIATE));


    SAFE_RELEASE(pBuffer);
    SAFE_RELEASE(ptInterface);

    return hr;

}

