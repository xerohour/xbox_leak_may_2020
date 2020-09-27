//-----------------------------------------------------------------------------
// File: play.cpp
//
// Desc: DirectMusic tutorial to show how to play a segment
//       on the default audio path
//
// Copyright (c) 2000 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#define NODSOUND
#include <xtl.h>

#include <dsound.h>
#include "dmusicip.h"

// warning C4102: 'xxx' : unreferenced label
#pragma warning(disable:4102)

extern "C" ULONG __cdecl DbgPrint(PCH Format, ...);

HRESULT ReportError(HRESULT hr,  const char *filename, int linenumber ){
	if ( FAILED(hr) ) {
		DbgPrint("%s(%d) : Failure 0x%08x\n", filename, linenumber, hr);
		__asm int 3
	}
	return hr;
}

#define REPORT(X) ReportError(X, __FILE__, __LINE__)

#define CHECK(X) if ( FAILED(hr = REPORT(X)) ) { goto bail; }

#define SAFE_RELEASE(X) if(X) { X->Release(); X = 0; }
void CreateClassLinkFile(char *pszFileName);

HRESULT Test() {
	HRESULT hr = S_OK;
	IDirectMusicLoader8*      pLoader         = NULL;
	IDirectMusicPerformance8* pPerformance    = NULL;
	IDirectMusicSegment8*     pMusic        = NULL;
	IDirectMusicSegment8*     pSound        = NULL;
	IDirectMusicSegment8*     pSound2        = NULL;

    // Create loader object
    DbgPrint("About to create DirectMusicLoader.\n");
    DirectMusicCreateInstance( CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader8,
		(void**)&pLoader );
 
    // Create performance object
    DbgPrint("About to create DirectMusicPerformance.\n");
    DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL,
		IID_IDirectMusicPerformance8, (void**)&pPerformance );

    // Initialize the performance with the standard audio path.
    // This initializes both DirectMusic and DirectSound and 
    // sets up the synthesizer. 
    DbgPrint("About to initialize audio.\n");

    // Also try DMUS_APATH_DYNAMIC_3D

    pPerformance->InitAudioX( DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, 64, DMUS_INITAUDIO_NOTHREADS);


    // Tell DirectMusic where the default search path is
    DbgPrint("SetSearchDirectory.\n");
	pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
	                               "D:\\Media\\dmtest", FALSE );

    // This command will copy the Media files to the proper location
    // on the Xbox hard disk:
    // xbcp -r Media xe:\

    // Set the GM DLS Collection so we can load midi files.
    DbgPrint("Install GM.DLS.\n");
	{
		DMUS_OBJECTDESC DESC;					// Descriptor to use to find it.
		memset( &DESC, 0, sizeof(DMUS_OBJECTDESC) );
		DESC.dwSize = sizeof (DMUS_OBJECTDESC);
		DESC.guidClass = CLSID_DirectMusicCollection;  
		wcscpy(DESC.wszFileName,L"gm.dls");
		DESC.guidObject = GUID_DefaultGMCollection;
		DESC.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_OBJECT;
		pLoader->SetObject(&DESC);
	}
    // Load the segment from the file
    DbgPrint("LoadObjectFromFile.\n");
  
	hr = pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
											   IID_IDirectMusicSegment8,
											   "lfive.sgt",
											   (LPVOID*) &pMusic );
    if(FAILED(hr)){
        DbgPrint("Could not load music lfive.sgt. Error: 0x%08x.\n", hr);
		return hr;
    }
	pSound = NULL;
	hr = pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
											   IID_IDirectMusicSegment8,
											   "tmsound.wav",
											   (LPVOID*) &pSound );
	pSound2 = NULL;
	hr = pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
											   IID_IDirectMusicSegment8,
											   "ricochet.wav",
											   (LPVOID*) &pSound2 );
	DbgPrint("Load sound returned %lx, %lx\n",hr,pSound);
    // Download the segment's instruments to the synthesizer
    DbgPrint("Download instruments.\n");
    pMusic->Download( pPerformance ); // A crash here usually indicates the media files are missing.
                                      // (See instructions on how to copy media files above.)
    pMusic->SetRepeats(100);
    pSound->Download( pPerformance );
    pSound2->Download( pPerformance );
    // Play segment on the default audio path

#define PLAY_SEGMENT
#ifdef PLAY_SEGMENT
    DbgPrint("Play segment.\n");
    pPerformance->PlaySegmentEx( pMusic, NULL, NULL, 0, 
                                   0, NULL, NULL, NULL );
#endif

//    hr = DirectSoundCreateStream(0, NULL, NULL, NULL);
/*    hr = DirectSoundCreate(0, NULL, NULL);
    hr = DirectSoundCreateBuffer(0, 0, NULL, NULL);
    hr = DirectSoundCreateStream(0, NULL, NULL, NULL);

    hr = DirectSoundCreateMixerSource(NULL, NULL, NULL, NULL);
    hr = DirectSoundCreateMixerDestination(NULL, NULL);

    hr = AdpcmCreateEncoder(NULL, 0, NULL, NULL);
    hr = AdpcmCreateDecoder(NULL, NULL, NULL);

//    hr = WmaCreateDecoder(NULL, NULL,NULL);

    hr = VoxwareCreateConverter(0, 0, NULL, NULL, NULL);

    hr = XHawkCreateMediaObject(NULL, 0, 0, NULL, NULL);

    hr = XFileCreateMediaObject(NULL, 0, 0, 0, 0, NULL);
    hr = XWaveFileCreateMediaObject(NULL, NULL, NULL);

    DirectSoundLinkAdpcm();*/


    // Now DirectMusic will play in the background, 
    // so continue on with our task
    bool fFadeIn = false;

// #define QUICK_TEST // Define this to make the test last just a few seconds. Good for debugging.
#ifdef QUICK_TEST
#define OUTER_LOOP 1
#define INNER_LOOP 500
#define WORK_UNIT 10
#else
#define OUTER_LOOP 250
#define INNER_LOOP 500
#define WORK_UNIT 10
#endif

    for (DWORD dwJ = 0;dwJ < OUTER_LOOP;dwJ++)
    {
        for (DWORD dwI = 0; dwI < INNER_LOOP; dwI++)
        {
            DirectMusicDoWork(WORK_UNIT);
            Sleep(WORK_UNIT);
        }
        DbgPrint("\n");
        REFERENCE_TIME rtNow,rtLatency,rtQueue;
        MUSIC_TIME mtNow;

#define PLAY_WAVES

// Pick one of the following
// #define RANDOMIZE_WAVE_TYPES
#define ALTERNATE_WAVES
// #define JUST_LONG_WAVE
// #define JUST_SHORT_WAVE

#ifdef PLAY_WAVES

        {
            IDirectMusicSegment8* pSegment = NULL;

#ifdef RANDOMIZE_WAVE_TYPES
            if (rand() % 3)
		    {
                pSegment = pSound2;
            }
            else
            {
                pSegment = pSound;
            }
#endif
#ifdef ALTERNATE_WAVES
            static counter;
            if(counter++ % 2)
            {
                pSegment = pSound2;
            }
            else
            {
                pSegment = pSound;
            }
#endif
#ifdef JUST_LONG_WAVE
            pSegment = pSound;
#endif
#ifdef JUST_SHORT_WAVE
            pSegment = pSound2;
#endif

            hr = pPerformance->PlaySegmentEx( pSegment, NULL, NULL, DMUS_SEGF_SECONDARY, 
								       0, NULL, NULL, NULL );
		    DbgPrint("Play sound effect returned %lx\n",hr);
        }

#endif // PLAY_WAVES

        pPerformance->GetTime(&rtNow,&mtNow);
        pPerformance->GetLatencyTime(&rtLatency);
        pPerformance->GetQueueTime(&rtQueue);
		rtNow /= 10000;
		rtLatency /= 10000;
		rtQueue /= 10000;
        DbgPrint("Timenow: %ld, Latency: %ld, Queue: %ld, or %ld, %ld\n",
            (long) rtNow , (long) rtLatency , (long) rtQueue ,
            (long)(rtQueue - rtLatency),(long)( rtLatency - rtNow));

        // DirectMusicMemDump();
    }

    // Stop the music, and close down 
    pPerformance->StopEx( NULL, 0, 0 );
    pMusic->Unload( pPerformance );
	pSound->Unload( pPerformance );
	pSound2->Unload( pPerformance );
    pPerformance->CloseDown();


bail:
    // Cleanup all interfaces
    SAFE_RELEASE(pLoader); 
    SAFE_RELEASE(pPerformance);
    SAFE_RELEASE(pSound);
    SAFE_RELEASE(pMusic);
	return hr;
}
//-----------------------------------------------------------------------------
// Name: main()
// Desc: Plays a single wave file using DirectMusic on the default audio path.
//-----------------------------------------------------------------------------
void __cdecl main()
{
    DbgPrint("\n--------------------------\n\nWelcome to dmtest.\n\n");
//    DbgPrint("Ready to start testing.  Set breakpoints\n");
//    __asm int 3

	DirectMusicInitializeFixedSizeHeaps(2*1024*1024, 0, &DirectMusicDefaultFactory);

	while( 1 )
    {
        Test();
		DbgPrint("Trying test again.\n");
	}
    DbgPrint("dmtest exiting.\n");
}



