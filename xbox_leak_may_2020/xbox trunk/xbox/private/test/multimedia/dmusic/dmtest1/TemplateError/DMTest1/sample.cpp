/********************************************************************************
FILE:
    SAMPLE.CPP

PURPOSE:
    Contains an example test function for DMusic.

BY:
    DANROSE
********************************************************************************/
#include "globals.h"


extern "C" ULONG __cdecl DbgPrint(PCH Format, ...);


/********************************************************************************
********************************************************************************/
HRESULT SampleTest() {
	HRESULT hr = S_OK;
	IDirectMusicLoader8*      pLoader         = NULL;
	IDirectMusicPerformance8* pPerformance    = NULL;
	IDirectMusicSegment8*     pMusic        = NULL;
	IDirectMusicSegment8*     pSound        = NULL;
	IDirectMusicSegment8*     pSound2        = NULL;

    // Create loader object
    DbgPrint("About to create DirectMusicLoader.\n");
    CHECKRUN(DirectMusicCreateInstance( CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader8,
		(void**)&pLoader ));

    // Create performance object
    DbgPrint("About to create DirectMusicPerformance.\n");
    CHECKRUN(DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL,
		IID_IDirectMusicPerformance8, (void**)&pPerformance ));

    // Initialize the performance with the standard audio path.
    // This initializes both DirectMusic and DirectSound and 
    // sets up the synthesizer. 
    DbgPrint("About to initialize audio.\n");
    CHECKRUN(pPerformance->InitAudioX(DMUS_APATH_SHARED_STEREOPLUSREVERB, 64));


    // Tell DirectMusic where the default search path is
    DbgPrint("SetSearchDirectory.\n");
	CHECKRUN(pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
	                               "T:\\Media\\Sample", FALSE ));
    
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
		hr = pLoader->SetObject(&DESC);
	}
    // Load the segment from the file
    DbgPrint("LoadObjectFromFile.\n");
  
	CHECKRUN(pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
											   IID_IDirectMusicSegment8,
											   "lfive.sgt",
											   (LPVOID*) &pMusic ));
	pSound = NULL;
	CHECKRUN(pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
											   IID_IDirectMusicSegment8,
											   "tmsound.wav",
											   (LPVOID*) &pSound ));
	pSound2 = NULL;
	CHECKRUN(pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
											   IID_IDirectMusicSegment8,
											   "ricochet.wav",
											   (LPVOID*) &pSound2 ));
	DbgPrint("Load sound returned %lx, %lx\n",hr,pSound);
    // Download the segment's instruments to the synthesizer
    DbgPrint("Download instruments.\n");
    CHECKRUN(pMusic->Download( pPerformance ));
    CHECKRUN(pMusic->SetRepeats(100));
	CHECKRUN(pSound->Download( pPerformance ));
	CHECKRUN(pSound2->Download( pPerformance ));
    // Play segment on the default audio path
    DbgPrint("Play segment.\n"); 
    CHECKRUN(pPerformance->PlaySegmentEx( pMusic, NULL, NULL, 0, 
                                   0, NULL, NULL, NULL ));

    // Now DirectMusic will play in the background, 
    // so continue on with our task

    for (;SUCCEEDED(hr);)
    {
        Sleep(5000);
        REFERENCE_TIME rtNow,rtLatency,rtQueue;
        MUSIC_TIME mtNow;
		if (rand() % 3)
		{
			CHECKRUN(pPerformance->PlaySegmentEx( pSound2, NULL, NULL, DMUS_SEGF_SECONDARY, 
								   0, NULL, NULL, NULL ));
			DbgPrint("Play sound effect returned %lx\n",hr);
		}
		else
		{
			CHECKRUN(pPerformance->PlaySegmentEx( pSound, NULL, NULL, DMUS_SEGF_SECONDARY, 
								   0, NULL, NULL, NULL ));
			DbgPrint("Play sound effect returned %lx\n",hr);
		}
        CHECKRUN(pPerformance->GetTime(&rtNow,&mtNow));
//        CHECKRUN(pPerformance->GetLatencyTime(&rtLatency));
//        CHECKRUN(pPerformance->GetQueueTime(&rtQueue));
//        DbgPrint("Timenow: %ld, Latency: %ld, Queue: %ld, or %ld, %ld\n",
//            (long) rtNow / 10000, (long) rtLatency / 10000, (long) rtQueue / 10000,
//            (long)(rtQueue - rtLatency)/10000,(long)( rtLatency - rtNow)/10000);
        DbgPrint("Timenow: %ld\n",
            (long) rtNow / 10000);
    }

    // Stop the music, and close down 
    CHECKRUN(pPerformance->StopEx( NULL, NULL, 0));
//    CHECKRUN(pPerformance->CloseDown());

    // Cleanup all interfaces
    RELEASE(pLoader); 
//    RELEASE(pPerformance);
    RELEASE(pSound);
    RELEASE(pMusic);
	return hr;
}
