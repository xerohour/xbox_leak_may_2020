//-----------------------------------------------------------------------------
// File: tutorial2.cpp
//
// Desc: DirectMusic tutorial to show how to get an object from
//       an audio path, and set the 3D position of a DirectMusic segment
//
// Copyright (c) 2000 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#define INITGUID
#include <windows.h>
#include <dmusicc.h>
#include <dmusici.h>
#include <cguid.h>




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
INT APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, 
                      INT nCmdShow )
{
    // Initialize COM
    CoInitialize(NULL);
    
    // Create loader object
    CoCreateInstance( CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
                      IID_IDirectMusicLoader8, (void**)&g_pLoader );

    // Create performance object
    CoCreateInstance( CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
                      IID_IDirectMusicPerformance8, (void**)&g_pPerformance );

    // Initialize the performance with the 3D audio path.
    // This initializes both DirectMusic and DirectSound and 
    // sets up the synthesizer. 
    g_pPerformance->InitAudio( NULL, NULL, NULL, 
                               DMUS_APATH_DYNAMIC_3D, 64,
                               DMUS_AUDIOF_ALL, NULL );

    CHAR strPath[MAX_PATH];
    GetWindowsDirectory( strPath, MAX_PATH );
    strcat( strPath, "\\media" );

    // Tell DirectMusic where the default search path is
    WCHAR wstrSearchPath[MAX_PATH];
    MultiByteToWideChar( CP_ACP, 0, strPath, -1, 
                         wstrSearchPath, MAX_PATH );

	g_pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
	                               wstrSearchPath, FALSE );
    
    // Load the segment from the file
    WCHAR wstrFileName[MAX_PATH] = L"The Microsoft Sound.wav";   
    if( FAILED( g_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
                                               IID_IDirectMusicSegment8,
                                               wstrFileName,
                                               (LPVOID*) &g_pSegment ) ) )
    {
        MessageBox( NULL, "Media not found, sample will now quit", 
                          "DirectMusic Tutorial", MB_OK );
        return 0;
    }

    // Download the segment's instruments to the synthesizer
    g_pSegment->Download( g_pPerformance );

    // Tell DirectMusic to repeat this segment forever
    g_pSegment->SetRepeats( DMUS_SEG_REPEAT_INFINITE );

    // Play segment on the default audio path
    g_pPerformance->PlaySegmentEx( g_pSegment, NULL, NULL, 0, 
                                   0, NULL, NULL, NULL );

    // Now DirectMusic will play in the backgroud, 
    // so continue on with our task
    MessageBox( NULL, "The music is now playing in center. " \
                "Click OK to pan music to left.", "DirectMusic Tutorial", MB_OK );

    // Get the default audio path we just created.
    IDirectMusicAudioPath8* pAudioPath = NULL;
    g_pPerformance->GetDefaultAudioPath( &pAudioPath );

    // Get the IDirectSound3DBuffer8 from the 3D audio path
    IDirectSound3DBuffer8* pDSB = NULL;
    pAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, 
                                 GUID_NULL, 0, IID_IDirectSound3DBuffer, 
                                 (LPVOID*) &pDSB );

    // Set the position of sound a little to the left
    pDSB->SetPosition( -0.1f, 0.0f, 0.0f, DS3D_IMMEDIATE );

    // Wait for input
    MessageBox( NULL, "The music is now playing on the left. " \
                "Click OK to pan music to right.", "DirectMusic Tutorial", MB_OK );

    // Set the position of sound a little to the right
    pDSB->SetPosition( 0.1f, 0.0f, 0.0f, DS3D_IMMEDIATE );

    // Wait for input
    MessageBox( NULL, "The music is now playing on the right. " \
                "Click OK to exit.", "DirectMusic Tutorial", MB_OK );

    // Stop the music, and close down 
    g_pPerformance->Stop( NULL, NULL, 0, 0 );
    g_pPerformance->CloseDown();

    // Cleanup all interfaces
    pDSB->Release();
    pAudioPath->Release();
    g_pLoader->Release(); 
    g_pPerformance->Release();
    g_pSegment->Release();

    // Close down COM
    CoUninitialize();
        
    return 0;
}

