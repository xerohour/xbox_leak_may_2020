#include "globals.h"
#include "cicmusicx.h"





/********************************************************************************
Valid test function.
********************************************************************************/
/*
HRESULT Performance8_InitAudio_Fatal (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    IDirectMusicPerformance *pPerformance = NULL;
    Log(FYILOGLEVEL, "Calling invalid test function Performance8_InitAudio())");

    hr = DirectMusicCreateInstance(CLSID_DirectMusicPerformance, NULL, IID_IDirectMusicPerformance, (void **)&pPerformance);
    hr = pPerformance->InitAudioX(NULL, NULL);
    
    _asm int 3;

    return hr;
};

*/