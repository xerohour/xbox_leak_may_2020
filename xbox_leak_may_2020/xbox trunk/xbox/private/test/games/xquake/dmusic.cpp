// Copyright (c) Microsoft Corporation. All rights reserved.

#ifdef XQUAKE_DMUSIC

#include <xtl.h>

#include "dmusicip.h"

extern "C" ULONG __cdecl DbgPrint(PCH Format, ...);

extern "C" void SND_DMusicInit();
extern "C" void SND_DMusicDoWork();

void SND_DMusicInit(){
	HRESULT hr = S_OK;
	IDirectMusicLoader8*      pLoader         = NULL;
	IDirectMusicPerformance8* pPerformance    = NULL;
	IDirectMusicSegment8*     pMusic        = NULL;
	IDirectMusicSegment8*     pSound        = NULL;
	IDirectMusicSegment8*     pSound2        = NULL;

    // Initialize DMusic
    DbgPrint("About to initialize DirectMusic.\n");
    DirectMusicInitialize();

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

    pPerformance->InitAudioX( DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, 64, DMUS_INITAUDIO_NOTHREADS);


    // Tell DirectMusic where the default search path is
    DbgPrint("SetSearchDirectory.\n");
	pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
	                               "D:\\Media\\dmtest", FALSE );

    // These commands will copy the Media files to the proper location
    // on the Xbox hard disk:
    //   dmusic test
    //   xbcp -r Media xc:\tdata\00000000

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
    DbgPrint("Play segment.\n"); 
    pPerformance->PlaySegmentEx( pMusic, NULL, NULL, 0, 
                                   0, NULL, NULL, NULL );

}

void SND_DMusicDoWork(){
            DirectMusicDoWork(10);
}

#endif // XQUAKE_DMUSIC

