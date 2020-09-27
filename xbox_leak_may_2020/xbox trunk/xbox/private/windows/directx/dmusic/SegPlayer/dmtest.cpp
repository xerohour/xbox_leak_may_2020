//-----------------------------------------------------------------------------
// File: play.cpp
//
// Desc: DirectMusic tutorial to show how to play a segment 
//       on the default audio path
//
// Copyright (c) 2000 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <dmusicc.h>
#include <dmusici.h>

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
    CHECK(DirectMusicCreateInstance( CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader8,
		(void**)&pLoader ));

    // Create performance object
    DbgPrint("About to create DirectMusicPerformance.\n");
    CHECK(DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL,
		IID_IDirectMusicPerformance8, (void**)&pPerformance ));

    // Initialize the performance with the standard audio path.
    // This initializes both DirectMusic and DirectSound and 
    // sets up the synthesizer. 
    DbgPrint("About to initialize audio.\n");

    CHECK(pPerformance->InitAudio( NULL, NULL, NULL, 
                               DMUS_APATH_SHARED_STEREOPLUSREVERB, 64,
                               DMUS_AUDIOF_ALL, NULL ));


    // Tell DirectMusic where the default search path is
    DbgPrint("SetSearchDirectory.\n");
	CHECK(pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
	                               L"T:\\Media\\dmtest", FALSE ));
    
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
  
	pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
											   IID_IDirectMusicSegment8,
											   L"lfive.sgt",
											   (LPVOID*) &pMusic );
	pSound = NULL;
	hr = pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
											   IID_IDirectMusicSegment8,
											   L"tmsound.wav",
											   (LPVOID*) &pSound );
	pSound2 = NULL;
	hr = pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
											   IID_IDirectMusicSegment8,
											   L"ricochet.wav",
											   (LPVOID*) &pSound2 );
	DbgPrint("Load sound returned %lx, %lx\n",hr,pSound);
    // Download the segment's instruments to the synthesizer
    DbgPrint("Download instruments.\n");
    CHECK(pMusic->Download( pPerformance ));
    pMusic->SetRepeats(100);
	pSound->Download( pPerformance );
	pSound2->Download( pPerformance );
    // Play segment on the default audio path
    DbgPrint("Play segment.\n"); 
    CHECK(pPerformance->PlaySegmentEx( pMusic, NULL, NULL, 0, 
                                   0, NULL, NULL, NULL ));

    // Now DirectMusic will play in the background, 
    // so continue on with our task
    for (;;)
    {
        Sleep(5000);
        REFERENCE_TIME rtNow,rtLatency,rtQueue;
        MUSIC_TIME mtNow;
		if (rand() % 3)
		{
			hr = pPerformance->PlaySegmentEx( pSound2, NULL, NULL, DMUS_SEGF_SECONDARY, 
								   0, NULL, NULL, NULL );
			DbgPrint("Play sound effect returned %lx\n",hr);
		}
		else
		{
			hr = pPerformance->PlaySegmentEx( pSound, NULL, NULL, DMUS_SEGF_SECONDARY, 
								   0, NULL, NULL, NULL );
			DbgPrint("Play sound effect returned %lx\n",hr);
		}
        pPerformance->GetTime(&rtNow,&mtNow);
        pPerformance->GetLatencyTime(&rtLatency);
        pPerformance->GetQueueTime(&rtQueue);
        DbgPrint("Timenow: %ld, Latency: %ld, Queue: %ld, or %ld, %ld\n",
            (long) rtNow / 10000, (long) rtLatency / 10000, (long) rtQueue / 10000,
            (long)(rtQueue - rtLatency)/10000,(long)( rtLatency - rtNow)/10000);
    }

    // Stop the music, and close down 
    CHECK(pPerformance->Stop( NULL, NULL, 0, 0 ));
    CHECK(pPerformance->CloseDown());

bail:
    // Cleanup all interfaces
    SAFE_RELEASE(pLoader); 
    SAFE_RELEASE(pPerformance);
    SAFE_RELEASE(pSound);
    SAFE_RELEASE(pMusic);
	return hr;
}
//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Plays a single wave file using DirectMusic on the default audio path.
//-----------------------------------------------------------------------------
void __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow ) 
{
    DbgPrint("\n--------------------------\n\nWelcome to dmtest.\n\n");
//    DbgPrint("Ready to start testing.  Set breakpoints\n");
//    __asm int 3

	while( FAILED( Test() ) ) {
		DbgPrint("Trying test again.\n");
	}
    DbgPrint("dmtest exiting.\n");
}



