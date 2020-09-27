//-----------------------------------------------------------------------------
// File: play.cpp
//
// Desc: DirectMusic tutorial to show how to play a segment 
//       on the default audio path
//
// Copyright (c) 2000 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <initguid.h>
#include <dmusicc.h>
#include <dmusici.h>


extern "C" ULONG __cdecl DbgPrint(PCH Format, ...);

extern "C" {
HRESULT STDAPICALLTYPE
DirectMusicLoaderCreate(IDirectMusicLoader8 **ppLoader, IUnknown *pUnknownOuter);

HRESULT STDAPICALLTYPE
DirectMusicPerformanceCreate(IDirectMusicPerformance8 **ppLoader, IUnknown *pUnknownOuter);
};


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
IDirectMusicLoader8*      g_pLoader         = NULL;
IDirectMusicPerformance8* g_pPerformance    = NULL;
IDirectMusicSegment8*     g_pSegment        = NULL;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Plays a single wave file using DirectMusic on the default audio path.
//-----------------------------------------------------------------------------
void __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow ) 
{
    DbgPrint("Ready to start testing.  Set breakpoints\n");
    __asm int 3

	// Create loader object
    DirectMusicLoaderCreate( &g_pLoader, NULL );

    // Create performance object
	DirectMusicPerformanceCreate( &g_pPerformance, NULL);

    // Initialize the performance with the standard audio path.
    // This initializes both DirectMusic and DirectSound and 
    // sets up the synthesizer. 
    g_pPerformance->InitAudio( NULL, NULL, NULL, 
                               DMUS_APATH_SHARED_STEREOPLUSREVERB, 64,
                               DMUS_AUDIOF_ALL, NULL );


    // Tell DirectMusic where the default search path is

	g_pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
	                               L"T:\\Media\\dmtest", FALSE );
    
    // Load the segment from the file
    WCHAR wstrFileName[MAX_PATH] = L"tmsound.wav";   
    if( FAILED( g_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
                                               IID_IDirectMusicSegment8,
                                               wstrFileName,
                                               (LPVOID*) &g_pSegment ) ) )
    {
		DbgPrint("Media not found, sample will now quit. File was: %ls\n", wstrFileName);
        return;
    }

    // Download the segment's instruments to the synthesizer
    g_pSegment->Download( g_pPerformance );

    // Play segment on the default audio path
    g_pPerformance->PlaySegmentEx( g_pSegment, NULL, NULL, 0, 
                                   0, NULL, NULL, NULL );

    // Now DirectMusic will play in the backgroud, 
    // so continue on with our task
    Sleep(5*1000);

    // Stop the music, and close down 
    g_pPerformance->Stop( NULL, NULL, 0, 0 );
    g_pPerformance->CloseDown();

    // Cleanup all interfaces
    g_pLoader->Release(); 
    g_pPerformance->Release();
    g_pSegment->Release();
}

