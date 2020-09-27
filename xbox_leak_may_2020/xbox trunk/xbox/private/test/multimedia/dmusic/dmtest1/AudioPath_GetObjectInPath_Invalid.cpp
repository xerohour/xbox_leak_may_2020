#include "globals.h"
#include "cicmusicx.h"
#include "AudioPath_GetObjectInPath_Invalid.hpp"


//Called functions.
HRESULT AudioPath_GetObjectInPath_NULLppvObject(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwStage);
HRESULT AudioPath_GetObjectInPath_InvalidScenarios(CtIDirectMusicPerformance8 *ptPerf8, LPSTR szAudPath, LPSTR szSegment, DWORD dwStage, GUID *pCLSID, GUID *pIID, DWORD dwBuffer, DWORD dwIndex);


/*******************************************************************************
Main test function for AudioPath::GetObjectInPath
********************************************************************************/
HRESULT AudioPath_GetObjectInPath_Invalid(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{

HRESULT hr = S_OK;
#define TEST AudioPath_GetObjectInPath_InvalidScenarios

    // ************
    //GUID and IIDs
    // ************
        //---------Performance Scenarios-------------
         //null IIDs                                                    
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, DMUS_PATH_PERFORMANCE,   (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL,0, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, DMUS_PATH_PERFORMANCE,   (GUID *)&GUID_NULL,        (GUID *)&GUID_NULL,0, 0));

        //invalid CLSIDs
//        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, DMUS_PATH_PERFORMANCE,   (GUID *)&CLSID_DirectMusicSegment,  (GUID *)&IID_IUnknown, 0, 0));

        //invalid IIDs
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia,  DMUS_PATH_PERFORMANCE,  (GUID *)&GUID_All_Objects,               (GUID *)&IID_IDirectMusicSegment,         0, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia,  DMUS_PATH_PERFORMANCE,  (GUID *)&GUID_All_Objects,               (GUID *)&GUID_Random,                     0, 0));
    
        //---------Buffer Scenarios-------------
        //NULL GUIDs
        
        //BUGBUG: Works anyway.  Should it?
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_NULL,          (GUID *)&GUID_NULL, 0, 0 ));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects,   (GUID *)&GUID_NULL, 0, 0 ));

        //Invalid IIDs
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&GUID_Random,     0, 0    ));



        //DMTEST_EXECUTE(TEST(ptPerf8, "GUIDs_MixinDMOsA.aud", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&IID_IDirectSoundFXChorus, 0, 0    )));


        //---------Buffer DMO Scenarios -------------
//        //NULL GUIDs
//        {L"GUIDs_SinkinDMOsA.aud", L"Test.mid", (GUID *)&GUID_NULL,         (GUID *)&IID_IDirectSoundFXChorus,       0, 0},
//        {L"GUIDs_SinkinDMOsA.aud", L"Test.mid", (GUID *)&GUID_NULL,         (GUID *)&GUID_NULL,                      0, 0},
//        {L"GUIDs_SinkinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_CHORUS,         (GUID *)&GUID_NULL,      0, 0},
//
//        //Invalid CLSIDs
//        //IID (Chorus) matches index (0) but CLSID is wrong (Compressor).
//        {L"GUIDs_SinkinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_COMPRESSOR,  (GUID *)&IID_IDirectSoundFXChorus,       0, 0},
//        //IID (Chorus) matches index (0) but CLSID is wrong (Random).
//        {L"GUIDs_SinkinDMOsA.aud", L"Test.mid", (GUID *)&GUID_Random,                    (GUID *)&IID_IDirectSoundFXChorus,       0, 0},
//
//
//        //Invalid IIDs
//        //CLSID (Distortion) matches index (2) but IID is wrong (Chorus).
//        {L"GUIDs_SinkinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_DISTORTION,  (GUID *)&IID_IDirectSoundFXChorus,       0, 0},
//
//        //IID (echo) matches index (2) but GUID is wrong (Random).
//        {L"GUIDs_SinkinDMOsA.aud", L"Test.mid", (GUID *)&GUID_Random,                    (GUID *)&IID_IDirectSoundFXDistortion,   0, 0},
//
//        //IID is soundbuffer
//        {L"GUIDs_SinkinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_DISTORTION,  (GUID *)&IID_IDirectSoundBuffer,         0, 0},
//
//        //Invalid index... (doesn't really belong here but oh well).
//        //IID matches CLSID but neither of the match the index.
//        {L"GUIDs_SinkinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_CHORUS,      (GUID *)&IID_IDirectSoundFXChorus,       0, 1},
//        
//
//        //---------Mixin Buffer Scenarios -------------
//        
//        //NULL GUIDs
//        {L"GUIDs_MixinDMOsB.aud", L"Test.mid", (GUID *)&GUID_NULL,                 (GUID *)&GUID_NULL,                  0, 0},
//        {L"GUIDs_MixinDMOsB.aud", L"Test.mid", (GUID *)&GUID_All_Objects,          (GUID *)&GUID_NULL,                  0, 0},
//
//        //Invalid IIDs
//        {L"GUIDs_MixinDMOsB.aud", L"Test.mid", (GUID *)&GUID_All_Objects,          (GUID *)&IID_IDirectSoundNotify,     0, 0},
//        {L"GUIDs_MixinDMOsB.aud", L"Test.mid", (GUID *)&GUID_All_Objects,          (GUID *)&IID_IPersist,               0, 0},
//
//        //wrong stage for this, otherwise this would work.
//        {L"GUIDs_MixinDMOsB.aud", L"Test.mid", (GUID *)&GUID_All_Objects,          (GUID *)&IID_IDirectSoundFXFlanger,  0, 0},
//
//        //Invalid combos (invalid cuz we're not asking for DMOs)
//        {L"GUIDs_MixinDMOsB.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_FLANGER, (GUID *)&IID_IDirectSoundFXFlanger, 0, 0},
//        {L"GUIDs_MixinDMOsB.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_GARGLE,  (GUID *)&IID_IDirectSoundFXGargle,  0, 0},
//
//        //Shouldn't be able to get 3D interface from a MIXIN buffer.
//        {L"GUIDs_MixinDMOsB.aud", L"Test.mid", (GUID *)&GUID_All_Objects,          (GUID *)&IID_IDirectSound3DBuffer,   0, 0},
//        
//
//    
//        //---------Mixin Buffer DMO Scenarios -------------
//
//        //NULL GUIDs
//        {L"GUIDs_MixinDMOsA.aud", L"Test.mid", (GUID *)&GUID_NULL,         (GUID *)&IID_IDirectSoundFXChorus,       0, 0},
//        {L"GUIDs_MixinDMOsA.aud", L"Test.mid", (GUID *)&GUID_NULL,         (GUID *)&GUID_NULL,                      0, 0},
//        {L"GUIDs_MixinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_CHORUS,         (GUID *)&GUID_NULL,      0, 0},
//
//        //Invalid CLSIDs
//        //IID (Chorus) matches index (0) but CLSID is wrong (Distortion).
//        {L"GUIDs_MixinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_DISTORTION,  (GUID *)&IID_IDirectSoundFXChorus,       0, 0},
//        //IID (Chorus) matches index (0) but CLSID is wrong (Random).
//        {L"GUIDs_MixinDMOsA.aud", L"Test.mid", (GUID *)&GUID_Random,                    (GUID *)&IID_IDirectSoundFXChorus,       0, 0},
//
//
//        //Invalid IIDs
//        //CLSID (echo) matches index (0) but IID is wrong (Chorus).
//        {L"GUIDs_MixinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_DISTORTION,  (GUID *)&IID_IDirectSoundFXChorus,       0, 0},
//
//        //IID (echo) matches index (1) but CLSID is wrong (Random).
//        {L"GUIDs_MixinDMOsA.aud", L"Test.mid", (GUID *)&GUID_Random,                    (GUID *)&IID_IDirectSoundFXCompressor,   0, 0},
//
//        //IID is soundbuffer
//        {L"GUIDs_MixinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_COMPRESSOR,  (GUID *)&IID_IDirectSoundBuffer,         0, 0},
//
//        //Invalid index... (doesn't really belong here but oh well).
//        //IID matches CLSID but neither of the match the index.
//        {L"GUIDs_MixinDMOsA.aud", L"Test.mid", (GUID *)&GUID_DSFX_STANDARD_CHORUS,      (GUID *)&IID_IDirectSoundFXChorus,       0, 1},
//    
//
//
    // ***************
    // Invalid dwStage 
    // ***************
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, 0, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown,0, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, 1, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown,0, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, 2, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown,0, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, 4, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown,0, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, 5, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown,0, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, 8, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown,0, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, 0xFFFFFFFF, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown,0, 0));

    // ****************
    //  TODO: Add more tests when we get DMOs.
    // Invalid dwBuffer    
    // ****************

        //Buffers 0 and 1 should work on this path; two shouldn't.
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown, 2, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREOPLUSREVERB", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown, 0xFFFFFFFF, 0));

        //Buffer1 shouldn't work on these paths.
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREO", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown, 1, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_DYNAMIC_MONO", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown, 1, 0));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_DYNAMIC_3D", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown, 1, 0));


    // ****************
    // Invalid dwIndex
    // ****************
        //Index 1 shouldn't work on anything.
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_SHARED_STEREO", g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown, 0, 1));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_DYNAMIC_MONO",   g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown, 0, 0xFFFFFFFF));
        DMTEST_EXECUTE(TEST(ptPerf8, "DMUS_APATH_DYNAMIC_3D",     g_TestParams.szDefaultMedia, DMUS_PATH_BUFFER, (GUID *)&GUID_All_Objects, (GUID *)&IID_IUnknown, 0, 1));


    // ************
    //ppvObject 
    // ************
        //---------NULL ppvObject Scenarios -------------
        //DMTEST_EXECUTE(AudioPath_GetObjectInPath_NULLppvObject(ptPerf8, DMUS_PATH_SEGMENT)); //Not for AudioPath
        //DMTEST_EXECUTE(AudioPath_GetObjectInPath_NULLppvObject(ptPerf8, DMUS_PATH_AUDIOPATH));
        DMTEST_EXECUTE(AudioPath_GetObjectInPath_NULLppvObject(ptPerf8, DMUS_PATH_PERFORMANCE));
        DMTEST_EXECUTE(AudioPath_GetObjectInPath_NULLppvObject(ptPerf8, DMUS_PATH_BUFFER));
        //DMTEST_EXECUTE(AudioPath_GetObjectInPath_NULLppvObject(ptPerf8, DMUS_PATH_BUFFER_DMO));
        //DMTEST_EXECUTE(AudioPath_GetObjectInPath_NULLppvObject(ptPerf8, DMUS_PATH_MIXIN_BUFFER));
        //DMTEST_EXECUTE(AudioPath_GetObjectInPath_NULLppvObject(ptPerf8, DMUS_PATH_MIXIN_BUFFER_DMO));

    return hr;

};


/********************************************************************************
IDirectMusicAudioPath::GetObjectInPath()

HISTORY:
    Created 07/31/00      danhaff
********************************************************************************/
HRESULT AudioPath_GetObjectInPath_InvalidScenarios(CtIDirectMusicPerformance8 *ptPerf8, LPSTR szAudPath, LPSTR szSegment, DWORD dwStage, GUID *pCLSID, GUID *pIID, DWORD dwBuffer, DWORD dwIndex)
{
    return templInvalidScenarios<CtIDirectMusicAudioPath>(ptPerf8, szAudPath, szSegment, dwStage, pCLSID, pIID, dwBuffer, dwIndex);
};


/********************************************************************************
tdmapthiNULLppvObject

HISTORY:
    Created 08/03/00      danhaff
********************************************************************************/
HRESULT AudioPath_GetObjectInPath_NULLppvObject(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwStage)
{
    return templNULLppvObject<CtIDirectMusicAudioPath>(ptPerf8, dwStage);
};








