#include "globals.h"
#include "Help_Buffer3D.h"
#include "AudioPath_GetObjectInPath_DSBTestFunc.h"

#define INTERFACE_NAME CtIDirectMusicAudioPath


/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT HelpInitBUFFER3D(CtIDirectMusicPerformance *ptPerf8, CICMusic &Music, DWORD dwPath, DWORD dwBuffer, BUFFER3D *pSound, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
T *ptInterface  = NULL;
IDirectSoundBuffer *pBuffer = NULL;
LPSTR szAudioPathConfig = NULL;

    if (!pSound)
    {
        Log(ABORTLOGLEVEL, "Test app error: pSoundis NULL!!!");
        hr = E_FAIL;
    }


    //Create the audiopath.
    if (!IsValidAudioPath(dwPath))
    {
        CHECK(szAudioPathConfig = (LPSTR)dwPath);
        CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, szAudioPathConfig));
    }
    else
    {
        CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, dwPath));
    }

    CHECKRUN(Music.GetInterface(&ptInterface));
    CHECKRUN(ptInterface->GetObjectInPath(DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, dwBuffer, GUID_NULL, 0, GUID_NULL, (void **)&pBuffer));
    CHECKRUN(pSound->Init(vDefault, pBuffer));
    SAFE_RELEASE(ptInterface);
    return hr;

};



/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetAllParameters(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetAllParameters(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetConeAngles(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetConeAngles(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetConeOrientation_BoundaryCases(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetConeOrientation_BoundaryCases(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetConeOrientation_Listening(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetConeOrientation_Listening(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetConeOutsideVolume(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetConeOutsideVolume(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetMaxDistance_BoundaryCases(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetMaxDistance_BoundaryCases(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetMaxDistance_InsideSource(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetMaxDistance_InsideSource(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetMinDistance_BoundaryCases(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetMinDistance_BoundaryCases(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetMinDistance_Listening(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetMinDistance_Listening(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetMode(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetMode(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetPosition_BoundaryCases(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetPosition_BoundaryCases(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetPosition_Listening(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetPosition_Listening(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetVelocity(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetVelocity(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
/*
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_INFINITY_Bug(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_INFINITY_Bug(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}
*/
/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetFrequency_Listening(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetFrequency_Listening(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetFrequency_Range(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetFrequency_Range(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetVolume_Listening(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetVolume_Listening(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetVolume_Range(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetVolume_Range(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}

/****************************************************************************************************
****************************************************************************************************/
/*
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetChannelVolume_Channel_Listening(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetChannelVolume_Channel_Listening(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}
*/
/****************************************************************************************************
****************************************************************************************************/
/*
template <class T>
HRESULT Template_GetObjectInPath_3DBuf_SetChannelVolume_Mask_Listening(CtIDirectMusicPerformance* ptPerf8, DWORD dwPath, DWORD dwBuffer, T* pDoNotUseDoNotRemove = NULL)
{
HRESULT hr = S_OK;
CICMusic Music;
BUFFER3D Sound;
CHECKRUN(HelpInitBUFFER3D(ptPerf8, Music, dwPath, dwBuffer, &Sound, pDoNotUseDoNotRemove));
CHECKRUN(DMTest_Buf_SetChannelVolume_Mask_Listening(&Sound, TESTAPPLY_IMMEDIATE, vDefault));
return hr;
}
*/

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetAllParameters(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetAllParameters(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetConeAngles(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetConeAngles(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetConeOrientation_BoundaryCases(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetConeOrientation_BoundaryCases(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetConeOrientation_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetConeOrientation_Listening(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetConeOutsideVolume(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetConeOutsideVolume(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetMaxDistance_BoundaryCases(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetMaxDistance_BoundaryCases(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetMaxDistance_InsideSource(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetMaxDistance_InsideSource(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetMinDistance_BoundaryCases(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetMinDistance_BoundaryCases(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetMinDistance_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetMinDistance_Listening(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetMode(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetMode(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetPosition_BoundaryCases(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetPosition_BoundaryCases(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetPosition_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetPosition_Listening(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetVelocity(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetVelocity(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
/*
HRESULT AudioPath_GetObjectInPath_3DBuf_INFINITY_Bug(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_INFINITY_Bug(ptPerf8, dwPath, dwBuffer, ptUseless);
}
*/

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetFrequency_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetFrequency_Listening(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetFrequency_Range(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetFrequency_Range(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetVolume_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetVolume_Listening(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
HRESULT AudioPath_GetObjectInPath_3DBuf_SetVolume_Range(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetVolume_Range(ptPerf8, dwPath, dwBuffer, ptUseless);
}

/****************************************************************************************************
****************************************************************************************************/
/*
HRESULT AudioPath_GetObjectInPath_3DBuf_SetChannelVolume_Channel_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetChannelVolume_Channel_Listening(ptPerf8, dwPath, dwBuffer, ptUseless);
}
*/

/****************************************************************************************************
****************************************************************************************************/
/*
HRESULT AudioPath_GetObjectInPath_3DBuf_SetChannelVolume_Mask_Listening(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPath, DWORD dwBuffer)
{
    INTERFACE_NAME *ptUseless = NULL;
    return Template_GetObjectInPath_3DBuf_SetChannelVolume_Mask_Listening(ptPerf8, dwPath, dwBuffer, ptUseless);
}
*/




