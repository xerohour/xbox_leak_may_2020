/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    main.cpp

Abstract:

    Entry point and initialization routines for test harness

Environment:

    Xbox

Revision History:

--*/

#include <xtl.h>
#include <dmusici.h>

void CallLoaderAPIs(void)
{
    IDirectMusicLoader     *pLoader     = NULL;
    DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader, (void **)&pLoader);
    pLoader->AddRef();
    pLoader->CollectGarbage();
   

};
/*


void CallSegmentAPIs(void)
{
    IDirectMusicSegment     *pSegment     = NULL;
    DirectMusicCreateInstance(CLSID_DirectMusicSegment, NULL, IID_IDirectMusicSegment, (void **)&pSegment);
};

void CallOtherAPIs(void)
{
    IDirectMusicPerformance *pPerformance = NULL;
    IDirectMusicScript     *pScript     = NULL;
    DirectMusicCreateInstance(CLSID_DirectMusicPerformance, NULL, IID_IDirectMusicPerformance, (void **)&pPerformance);
    DirectMusicCreateInstance(CLSID_DirectMusicScript, NULL, IID_IDirectMusicScript, (void **)&pScript);

    pPerformance->InitAudioX(0, 0, 0, 0);
    pPerformance->StopEx(0, 0, 0);
};
*/

int __cdecl main()
{
    //CallLoaderAPIs();
    //CallSegmentAPIs();
    //CallOtherAPIs();
    return 0;
}
