//-----------------------------------------------------------------------------
// File: MusicManager.cpp
//
// Desc: Implementation file for CMusicManager class.
//
// Hist: 8.20.01 - New for October XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "musicmanager.h"
#include "XbUtil.h"
#include <assert.h>
#include <stdio.h>
#include <tchar.h>



//
// Here is our game soundtrack including WMA files we ship
// with our game.  If needed, this concept could be extended
// to include several different game soundtracks
//

// Structure representing game soundtrack song
typedef struct {
    WCHAR * szName;
    CHAR *  szFilename;
    DWORD   dwLength;
} MM_SONG;

MM_SONG g_aGameSoundtrack[] =
{
    { L"Becky", "D:\\Media\\Sounds\\Becky.wma", 165000 },
    { L"Becky remix", "D:\\Media\\Sounds\\Becky.wma", 165000 },
};

#define NUM_GAME_SONGS ( sizeof( g_aGameSoundtrack ) / sizeof( g_aGameSoundtrack[0] ) )

#define WORKER_THREAD_STACK_SIZE 12 * 1024
#define WMA_LOOKAHEAD 64 * 1024
#define YIELD_RATE 3

//
// The following methods are safe to be called from the main rendering thread.
// In order to avoid CPU-intensive thread synchronization, we assume that only
// one thread will ever be calling into these functions.  That way, we know
// that if there is no state change already pending, then the state cannot 
// possibly change during a function call.  Threading issues are noted for
// each of these functions
//

//-----------------------------------------------------------------------------
// Name: CMusicManager (ctor)
// Desc: Initializes member variables
// Thread issues: None - worker thread is not created yet.
//-----------------------------------------------------------------------------
CMusicManager::CMusicManager()
{
    m_mmOldState    = MM_STOPPED;
    m_mmState       = MM_STOPPED;
    m_fRandom       = FALSE;
    m_fGlobal       = TRUE;
    m_lVolume       = DSBVOLUME_MAX;
    m_dwStream      = 0;
    m_dwSong        = 0;
    m_szSong[0]     = 0;
    m_dwLength      = 0;
    m_dwSongID      = 0;

    m_aSoundtracks          = NULL;
    m_uSoundtrackCount      = 0;
    m_uCurrentSoundtrack    = 0;
    m_dwPacketsCompleted    = 0;

    m_pbSampleData  = NULL;
    m_pDecoder      = NULL;
    m_pStream[0]    = NULL;
    m_pStream[1]    = NULL;
    m_hDecodingFile = INVALID_HANDLE_VALUE;

    m_hThread           = INVALID_HANDLE_VALUE;
    m_hShutdownEvent    = INVALID_HANDLE_VALUE;

    for( INT i = 0; i < PACKET_COUNT + EXTRA_PACKETS; i++ )
        m_adwPacketStatus[i] = XMEDIAPACKET_STATUS_SUCCESS;
}


//-----------------------------------------------------------------------------
// Name: ~CMusicManager (dtor)
// Desc: Releases any resources allocated by the object
// Thread issues: This function waits for the worker thread to shut down
//                before continuing processing.  
//-----------------------------------------------------------------------------
CMusicManager::~CMusicManager()
{
    // Signal our worker thread to shutdown, then wait for it
    // before releasing objects
    if( m_hShutdownEvent != INVALID_HANDLE_VALUE )
        SetEvent( m_hShutdownEvent );
    
    if( m_hThread != INVALID_HANDLE_VALUE )
    {
        WaitForSingleObject( m_hThread, INFINITE );
        CloseHandle( m_hThread );
    }

    // We can't close the handle until we're sure the worker
    // thread has terminated.
    if( m_hShutdownEvent != INVALID_HANDLE_VALUE )
        CloseHandle( m_hShutdownEvent );

    // Now that we've shut down the worker thread, it's safe
    // to clean these guys up.
    if( m_pStream[0] )
    {
        m_pStream[0]->Pause( DSSTREAMPAUSE_RESUME );
        m_pStream[0]->Release();
    }

    if( m_pStream[1] )
    {
        m_pStream[1]->Pause( DSSTREAMPAUSE_RESUME );
        m_pStream[1]->Release();
    }

    if( m_pbSampleData )
        delete[] m_pbSampleData;

    free( m_aSoundtracks );

}


//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Sets up the object to run
// Thread issues: None - worker thread isn't created until end of function
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::Initialize( BOOL bSpinThread /* TRUE */ )
{
    HRESULT hr;

    // Load up soundtrack information
    hr = LoadSoundtracks();
    if( FAILED( hr ) )
        return hr;
    SelectSong( 0 );

    srand( GetTickCount() );

    // Soundtrack songs are ripped to this format.  Any individual
    // WMA tracks shipped with the game will need to be encoded
    // similarly.
    // Alternately, streams could be released and re-created when
    // switching tracks, but this would incur additional CPU costs.  
    // In the future, a SetFormat() routine on DirectSound streams 
    // and buffers will make this simpler.
    WAVEFORMATEX wfx = {0};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    // Set up a stream desc to create our streams
    DSSTREAMDESC dssd = {0};
    dssd.dwMaxAttachedPackets = PACKET_COUNT;
    dssd.lpwfxFormat = &wfx;
    dssd.lpfnCallback = StreamCallback;
    dssd.lpvContext = this;

    // Create the streams
    hr = DirectSoundCreateStream( &dssd, &m_pStream[0] );
    if( FAILED( hr ) )
        return hr;
    hr = DirectSoundCreateStream( &dssd, &m_pStream[1] );
    if( FAILED( hr ) )
        return hr;

    // Set up attack envelopes to fade in for us
    DSENVELOPEDESC dsed = {0};
    dsed.dwEG = DSEG_AMPLITUDE;
    dsed.dwMode = DSEG_MODE_ATTACK;
    dsed.dwAttack = DWORD(48000 * FADE_TIME / 512);
    dsed.dwRelease = DWORD(48000 * FADE_TIME / 512);
    dsed.dwSustain = 255;
    m_pStream[0]->SetEG( &dsed );
    m_pStream[1]->SetEG( &dsed );

    // Allocate sample data buffer
    m_pbSampleData = new BYTE[ ( PACKET_COUNT + EXTRA_PACKETS ) * PACKET_SIZE ];
    if( !m_pbSampleData )
        return E_OUTOFMEMORY;

#if _DEBUG
    if( FAILED( DebugVerify() ) )
        return E_FAIL;
#endif // _DEBUG

    if( bSpinThread )
    {
        // Create an event to signal the worker thread to shutdown
        m_hShutdownEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
        if( NULL == m_hShutdownEvent )
            return E_FAIL;

        // Create the worker thread
        m_hThread = CreateThread( NULL, WORKER_THREAD_STACK_SIZE, MusicManagerThreadProc, this, 0, NULL );
        if( NULL == m_hThread )
        {
            return E_FAIL;
        }
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Play
// Desc: Starts playing background music
// Thread issues: Depends on worker thread state:
//                MM_PLAYING: No effective change.  Worker thread could be
//                            processing a state change to MM_PAUSED or
//                            MM_STOPPED, in which case we will switch back
//                            to MM_PLAYING on the next iteration of Process()
//                MM_PAUSED:  Next iteration of Process() will pick up the 
//                            state change and resume playback
//                MM_STOPPED: Next iteration of Process() will pick up the
//                            state change and stop playback
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::Play()
{
    // Can always transition to playing
    m_mmState = MM_PLAYING;
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Stop
// Desc: Stops background music playback
// Thread issues: Depends on worker thread state:
//                MM_PLAYING: Next iteration of Process() will pick up the
//                            state change and stop playback
//                MM_PAUSED:  Next iteration of Process() will pick up the 
//                            state change and resume playback
//                MM_STOPPED: No effective change.  Worker thread could be
//                            processing a state change to MM_PLAYING, in
//                            which case we will switch back to MM_STOPPED
//                            on the next iteration of Process()
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::Stop()
{
    // Can always transition to stopped
    if( m_mmOldState != MM_STOPPED &&
        m_mmState    != MM_STOPPED )
    {
        m_mmState = MM_STOPPED;

        // Pause the streams right now so that we
        // stop playback.  They'll get shut down
        // fully on the worker thread
        m_pStream[0]->Pause( DSSTREAMPAUSE_PAUSE );
        m_pStream[1]->Pause( DSSTREAMPAUSE_PAUSE );
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Pause
// Desc: Pauses background music playback
// Thread issues: Since we can only transition to MM_PAUSED from MM_PLAYING,
//                make sure that we're already at MM_PLAYING, and don't have
//                a pending state change.  Next iteration of Process() will
//                pick up the change to MM_PAUSED and pause playback
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::Pause()
{
    // Can only transition to paused from playing.
    if( m_mmOldState == MM_PLAYING &&
        m_mmState == MM_PLAYING )
    {
        m_pStream[0]->Pause( DSSTREAMPAUSE_PAUSE );
        m_pStream[1]->Pause( DSSTREAMPAUSE_PAUSE );
        m_mmState = MM_PAUSED;
        return S_OK;
    }

    return S_FALSE;
}



//-----------------------------------------------------------------------------
// Name: SetRandom
// Desc: Sets the playback mode for how to pick the next song.  If fRandom is
//       true, the next track is picked randomly, otherwise it's sequential.
//       If fGlobal is true, we'll move between soundtracks, otherwise we stay
//       within the current soundtrack
// Thread issues: The worker thread could be in the middle of picking which
//                track to play next.  The new setting may or may not be 
//                used, depending on exactly when the calls occurs.
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::SetRandom( BOOL fRandom )
{
    m_fRandom = fRandom;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SetGlobal
// Desc: Sets the playback mode for how to pick the next song.  If fGlobal is 
//       true, we'll move between soundtracks, otherwise we stay  within the 
//       current soundtrack
// Thread issues: The worker thread could be in the middle of picking which
//                track to play next.  The new setting may or may not be 
//                used, depending on exactly when the calls occurs.
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::SetGlobal( BOOL fGlobal )
{
    m_fGlobal = fGlobal;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SetVolume
// Desc: Sets the overall volume level for music playback.  Note that this
//       won't really work properly until we start using envelope release
// Thread issues: None - the worker thread doesn't set volume levels.
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::SetVolume( LONG lVolume )
{
    assert( m_pStream[0] && m_pStream[1] );

    m_lVolume = lVolume;
    m_pStream[0]->SetVolume( lVolume );
    m_pStream[1]->SetVolume( lVolume );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: GetCurrentInfo
// Desc: Returns pointers to info.  Buffers should be appropriately sized, ie
//       MAX_SOUNDTRACK_NAME and MAX_SONG_NAME, respectively
// Thread issues: The worker thread could be in the middle of switching
//                between songs and/or soundtracks.  In this case, we could
//                return mixed information.  This is OK, since this is meant
//                to be called periodically to update state, so the results
//                will eventually be consistent.
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::GetCurrentInfo( WCHAR * szSoundtrack, WCHAR * szSong, DWORD * pdwLength )
{
    if( szSoundtrack )
        m_aSoundtracks[ m_uCurrentSoundtrack ].GetSoundtrackName( szSoundtrack );
    if( szSong )
        wcscpy( szSong, m_szSong );
    if( pdwLength )
        *pdwLength = m_dwLength;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: GetPlaybackPosition
// Desc: Returns the current playback position, in seconds
//-----------------------------------------------------------------------------
FLOAT
CMusicManager::GetPlaybackPosition()
{
    return PACKET_TIME * m_dwPacketsCompleted;
}



//-----------------------------------------------------------------------------
// Name: NextSoundtrack
// Desc: Switches to the next soundtrack.  This is only safe to do if playback
//       is stopped, and we don't have a state change pending.  
// Thread issues: The worker thread could be trying to switch songs or 
//                soundtracks, so we can't make a call unless we know that
//                the worker thread is in MM_STOPPED and will stay there for
//                the duration of this call
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::NextSoundtrack()
{
    if( m_mmOldState == MM_STOPPED &&
        m_mmState    == MM_STOPPED )
    {
        return SelectSoundtrack( ( m_uCurrentSoundtrack + 1 ) % m_uSoundtrackCount );
    }

    return S_FALSE;
}



//-----------------------------------------------------------------------------
// Name: NextSong
// Desc: Switches to the next song in the current soundtrack.  This is only 
//       safe to do if playback is stopped, and we don't have a state change 
//       pending.
// Thread issues: The worker thread could be trying to switch songs or 
//                soundtracks, so we can't make a call unless we know that
//                the worker thread is in MM_STOPPED and will stay there for
//                the duration of this call
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::NextSong()
{
    if( m_mmOldState == MM_STOPPED &&
        m_mmState    == MM_STOPPED )
    {
        return SelectSong( ( m_dwSong + 1 ) % m_aSoundtracks[ m_uCurrentSoundtrack ].GetSongCount() );
    }

    return S_FALSE;
}




//-----------------------------------------------------------------------------
// Name: RandomSong
// Desc: Switches to a random song, either in this soundtrack if fGlobal is
//       FALSE or globally random if fGlobal is TRUE.  This is only safe
//       to do if playback is stopped, and we don't have a state change 
//       pending.
// Thread issues: The worker thread could be trying to switch songs or 
//                soundtracks, so we can't make a call unless we know that
//                the worker thread is in MM_STOPPED and will stay there for
//                the duration of this call
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::RandomSong( BOOL fGlobal )
{
    if( m_mmOldState == MM_STOPPED &&
        m_mmState    == MM_STOPPED )
    {
        if( fGlobal )
        {
            SelectSoundtrack( rand() % m_uSoundtrackCount );
        }
        return SelectSong( rand() % m_aSoundtracks[ m_uCurrentSoundtrack ].GetSongCount() );
    }
    else
        return S_FALSE;
}




//
// The following methods are not public and are meant only to be called 
// internally by the CMusicManager class.  Any tricky threading issues are
// pointed out as needed.
//



//-----------------------------------------------------------------------------
// Name: LoadSoundtracks
// Desc: Loads soundtrack info for user soundtracks stored on HD
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::LoadSoundtracks()
{
    HANDLE hSoundtrack;
    XSOUNDTRACK_DATA stData;
    UINT uAllocatedSoundtracks;

    m_aSoundtracks = (CSoundtrack *)malloc( sizeof( CSoundtrack ) );
    if( !m_aSoundtracks )
        return E_OUTOFMEMORY;
    uAllocatedSoundtracks = 1;

    // Set up our game soundtrack as soundtrack 0.
    // If we had more than 1 game soundtrack, we could
    // set them all up here
    m_aSoundtracks[0].m_fGameSoundtrack     = TRUE;
    m_aSoundtracks[0].m_uSoundtrackIndex    = 0;
    m_aSoundtracks[0].m_uSongCount          = NUM_GAME_SONGS;
    wcscpy( m_aSoundtracks[0].m_szName, L"Game Soundtrack" );
    m_uSoundtrackCount = 1;

    // Start scanning the soundtrack DB
    hSoundtrack = XFindFirstSoundtrack( &stData );
    if( INVALID_HANDLE_VALUE != hSoundtrack )
    {
        do
        {
            // Double our buffer if we need more space
            if( m_uSoundtrackCount + 1 > uAllocatedSoundtracks )
            {
                void * pNewAlloc = realloc( m_aSoundtracks, ( uAllocatedSoundtracks * 2 ) * sizeof( CSoundtrack ) );
                if( !pNewAlloc )
                {
                    // We couldn't expand our buffer, so clean up
                    // and bail out
                    free( m_aSoundtracks );
                    m_aSoundtracks = NULL;
                    XFindClose( hSoundtrack );
                    return E_OUTOFMEMORY;
                }
                m_aSoundtracks = (CSoundtrack *)pNewAlloc;
                uAllocatedSoundtracks *= 2;
            }

            // Ignore empty soundtracks
            if( stData.uSongCount > 0 )
            {
                // Copy the data over
                m_aSoundtracks[ m_uSoundtrackCount ].m_fGameSoundtrack = FALSE;
                m_aSoundtracks[ m_uSoundtrackCount ].m_uSoundtrackID   = stData.uSoundtrackId;
                m_aSoundtracks[ m_uSoundtrackCount ].m_uSongCount      = stData.uSongCount;
                wcscpy( m_aSoundtracks[ m_uSoundtrackCount ].m_szName, stData.szName );

                m_uSoundtrackCount++;
            }

        } while( XFindNextSoundtrack( hSoundtrack, &stData ) );

        XFindClose( hSoundtrack );
    }

    // Shrink our allocation down to what's actually needed, since
    // we can't add soundtracks on the fly
    m_aSoundtracks = (CSoundtrack *)realloc( m_aSoundtracks, m_uSoundtrackCount * sizeof( CSoundtrack ) );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SelectSoundtrack
// Desc: Changes to the specified soundtrack
// NOTE: This should only be called by the worker thread OR
//       after making sure that playback is stopped with no state change
//       pending.
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::SelectSoundtrack( DWORD dwSoundtrack )
{
    m_uCurrentSoundtrack = dwSoundtrack;
    SelectSong( 0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SelectSong
// Desc: Switches to the specified song and caches song info
// NOTE: This should only be called by the worker thread OR
//       after making sure that playback is stopped with no state change
//       pending.
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::SelectSong( DWORD dwSong )
{
    m_dwSong = dwSong;
    m_aSoundtracks[ m_uCurrentSoundtrack ].GetSongInfo( m_dwSong,
                                                        &m_dwSongID,
                                                        &m_dwLength,
                                                        m_szSong );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FindNextSong
// Desc: Simple helper function to switch to the next song, based on the
//       global repeat flag.
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::FindNextSong()
{
    if( m_fRandom )
    {
        if( m_fGlobal )
        {
            SelectSoundtrack( rand() % m_uSoundtrackCount );
        }
        SelectSong( rand() % m_aSoundtracks[ m_uCurrentSoundtrack ].GetSongCount() );
    }
    else
    {
        if( m_fGlobal && 
            m_dwSong == m_aSoundtracks[ m_uCurrentSoundtrack ].GetSongCount() - 1 )
        {
            SelectSoundtrack( ( m_uCurrentSoundtrack + 1 ) % m_uSoundtrackCount );
        }
        else
            SelectSong( ( m_dwSong + 1 ) % m_aSoundtracks[ m_uCurrentSoundtrack ].GetSongCount() );
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Prepare
// Desc: Prepares to begin playback of the currently set track:
//       1) Clean up the old decoder, if we already had one
//       2) Create a new decoder on the soundtrack handle or our WMA file
//       3) Lower the volume on the output stream for the crossfade
//       4) Mark the start time of the track for rough timing
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::Prepare()
{
    WAVEFORMATEX wfx;

    // Kill the old decoder
    Cleanup();

    // Open soundtrack file
    m_hDecodingFile = m_aSoundtracks[ m_uCurrentSoundtrack ].OpenSong( m_dwSongID );
    if( INVALID_HANDLE_VALUE == m_hDecodingFile )
        return E_FAIL;

    // Create the decoder
    if( FAILED( WmaCreateDecoderEx( NULL, 
                                    m_hDecodingFile, 
                                    FALSE,              // Synchronous
                                    WMA_LOOKAHEAD,      // 64k lookahead
                                    PACKET_COUNT,       // # of packets
                                    YIELD_RATE,         // Yield rate
                                    &wfx,               // We know the format already
                                    &m_pDecoder ) ) )   // New decoder
        return E_FAIL;

    // Remember when we started
    m_dwPacketsCompleted = 0;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Cleanup
// Desc: Cleans up decoding resources
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::Cleanup()
{
    // Close soundtrack file, if that's what we had
    if( INVALID_HANDLE_VALUE != m_hDecodingFile )
    {
        CloseHandle( m_hDecodingFile );
        m_hDecodingFile = INVALID_HANDLE_VALUE;
    }

    // Free up the decoder
    if( m_pDecoder )
    {
        m_pDecoder->Release();
        m_pDecoder = NULL;
    }

    m_dwPacketsCompleted = 0;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Process
// Desc: Manages audio packets, filling them from the source XMO and 
//       dispatching them to the appropriate stream.
// Thread issues: This function manages state changes requested from the
//                main game thread.  The main game thread will INT a new value
//                for m_mmState, and then the worker thread will transition
//                to that state as appropriate
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::Process()
{
    HRESULT hr;
    DWORD   dwPacket;
    MM_STATE mmCurrentState = m_mmState;

    // Check to see if our state has changed from a call
    // from the main rendering loop, and if it has,
    // update accordingly.
    if( m_mmOldState != mmCurrentState )
    {
        // We can't transition from MM_STOPPED to
        // anything but MM_PLAYING
        assert( !( m_mmOldState == MM_STOPPED &&
                   mmCurrentState != MM_PLAYING ) );

        switch( m_mmOldState )
        {
        //
        // Valid transitions from MM_STOPPED:
        // -> MM_PLAYING = Prepare to play
        //
        case MM_STOPPED:
            switch( mmCurrentState )
            {
            case MM_PLAYING:
                // Make sure the streams are unpaused
                m_pStream[0]->Pause( DSSTREAMPAUSE_RESUME );
                m_pStream[1]->Pause( DSSTREAMPAUSE_RESUME );
                Prepare();
                break;
            }
            break;

        //
        // Valid transitions from MM_PAUSED:
        // -> MM_PLAYING = Resume playback
        // -> MM_STOPPED = Stop playback and clean up
        //
        case MM_PAUSED:
            switch( mmCurrentState )
            {
            case MM_PLAYING:
                m_pStream[0]->Pause( DSSTREAMPAUSE_RESUME );
                m_pStream[1]->Pause( DSSTREAMPAUSE_RESUME );
                break;
            case MM_STOPPED:
                m_pStream[0]->Flush();
                m_pStream[1]->Flush();
                Cleanup();
                break;
            }
            break;

        //
        // Valid transitions from MM_PLAYING:
        // -> MM_PAUSED = Pause streams (already done)
        // -> MM_STOPPED = Stop playback and clean up
        case MM_PLAYING:
            switch( mmCurrentState )
            {
            case MM_STOPPED:
                m_pStream[0]->Flush();
                m_pStream[1]->Flush();
                Cleanup();
                break;
            }
            break;
        }

        // We're done with the transition.  At this point,
        // we're really in the appropriate state, and the
        // other thread can push a new state change through
        m_mmOldState = mmCurrentState;
    }


    // If we're currently playing, then process packets
    // and fades
    if( mmCurrentState == MM_PLAYING )
    {
        // Process packets
        while( FindFreePacket( &dwPacket, m_dwStream ) )
        {
            XMEDIAPACKET xmp;

            hr = ProcessSource( dwPacket, &xmp );
            if( FAILED( hr ) )
                return hr;

            // Make sure we got data from the decoder.  When we
            // hit the end of a track, we may end up with an
            // empty packet.  We'll just loop again and fill it
            // up from the next track.
            if( xmp.dwMaxSize )
            {
                hr = ProcessStream( dwPacket, &xmp );
                if( FAILED( hr ) )
                    return hr;
            }
        }
    }


    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FindFreePacket
// Desc: Looks for a free audio packet.  Returns TRUE if one was found and
//       returns the index
//-----------------------------------------------------------------------------
BOOL CMusicManager::FindFreePacket( DWORD * pdwPacket, DWORD dwStream )
{
    for( DWORD dwIndex = dwStream; dwIndex < PACKET_COUNT; dwIndex += 2 )
    {
        // The first EXTRA_PACKETS * 2 packets are reserved - odd packets
        // for stream 1, even packets for stream 2.  This is to ensure
        // that there are packets available during the crossfade
        if( XMEDIAPACKET_STATUS_PENDING != m_adwPacketStatus[ dwIndex ] &&
            ( dwIndex > EXTRA_PACKETS * 2 || dwIndex % 2 == dwStream ) )
        {
            if( pdwPacket )
                *pdwPacket = dwIndex;
            return TRUE;
        }
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
// Name: ProcessSource
// Desc: Fills audio packets from the decoder XMO
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::ProcessSource( DWORD dwPacket, XMEDIAPACKET * pxmp )
{
    HRESULT      hr;
    DWORD        dwBytesDecoded;

    // Set up the XMEDIAPACKET structure
    ZeroMemory( pxmp, sizeof( XMEDIAPACKET ) );
    pxmp->pvBuffer          = m_pbSampleData + dwPacket * PACKET_SIZE;
    pxmp->dwMaxSize         = PACKET_SIZE;
    pxmp->pdwCompletedSize  = &dwBytesDecoded;

    hr = m_pDecoder->Process( NULL, pxmp );
    if( FAILED( hr ) )
        return hr;

    if( 0 == dwBytesDecoded )
    {
        // We hit the end of the current track.  Key the fade-out
        m_pStream[ m_dwStream ]->Discontinuity();
        m_pStream[ m_dwStream ]->FlushEx( 0, DSSTREAMFLUSHEX_ASYNC | DSSTREAMFLUSHEX_ENVELOPE );

        // Clean up resources from this song, and get
        // ready for the next one:
        Cleanup();
        FindNextSong();
        m_dwStream = ( m_dwStream + 1 ) % 2;
        Prepare();

    }
    pxmp->dwMaxSize = dwBytesDecoded;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ProcessStream
// Desc: Submits audio packets to the appropriate stream
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::ProcessStream( DWORD dwPacket, XMEDIAPACKET * pxmp )
{
    HRESULT      hr;

    // The XMEDIAPACKET should already have been filled out
    // by the call to ProcessSource().  In addition, ProcessSource()
    // should have properly set the dwMaxSize member to reflect
    // how much data was decoded.
    pxmp->pdwStatus = &m_adwPacketStatus[ dwPacket ];
    pxmp->pdwCompletedSize = NULL;
    pxmp->pContext = (LPVOID)m_dwStream;

    hr = m_pStream[ m_dwStream ]->Process( pxmp, NULL );
    if( FAILED( hr ) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MusicManagerDoWork
// Desc: Workhorse function for the CMusicManager class.  Either called by
//       the MusicManager's threadproc, or manually by a game worker thread
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::MusicManagerDoWork()
{
    Process();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MusicManagerThreadProc
// Desc: Thread procedure on which the music manager decodes
//-----------------------------------------------------------------------------
DWORD WINAPI MusicManagerThreadProc( LPVOID lpParameter )
{
    CMusicManager * pThis = (CMusicManager *)lpParameter;
    DWORD dwResult;

    //
    // Our thread procedure basically just loops indefinitely,
    // processing packets and switching between songs as 
    // appropriate, until we're signalled to shutdown.
    //
    do
    {
        pThis->MusicManagerDoWork();
        dwResult = WaitForSingleObject( pThis->m_hShutdownEvent, 1000 / 60 );
    } while( dwResult == WAIT_TIMEOUT );

    // Since we created the decoder on this thread, we'll
    // release it on this thread.
    pThis->Cleanup();

    return 0;
}



//-----------------------------------------------------------------------------
// Name: StreamCallback
// Desc: Called back on completion of stream packets.  The stream context
//       contains a pointer to the CMusicManager object, and the packet
//       context contains the stream number
//-----------------------------------------------------------------------------
void CALLBACK 
StreamCallback( LPVOID pStreamContext, LPVOID pPacketContext, DWORD dwStatus )
{
    CMusicManager * pThis = (CMusicManager *)pStreamContext;

    if( DWORD(pPacketContext) == pThis->m_dwStream &&
        dwStatus == XMEDIAPACKET_STATUS_SUCCESS )
        ++pThis->m_dwPacketsCompleted;
}




#if _DEBUG
//-----------------------------------------------------------------------------
// Name: DebugVerify
// Desc: Debug routine to verify that everything is properly set up:
//       * Must have at least 1 game soundtrack, since a game can't depend
//         on there being user soundtracks on the Xbox hard drive
//       * Check that we can open all soundtrack songs
//-----------------------------------------------------------------------------
HRESULT
CMusicManager::DebugVerify()
{
    // Make sure we have at least 1 game soundtrack
    if( !m_aSoundtracks[ 0 ].m_fGameSoundtrack )
    {
        OUTPUT_DEBUG_STRING( "Must have at least 1 game soundtrack.\n" );
        return E_FAIL;
    }

    // Verify we can open all soundtrack files.  This could take a while.
    for( UINT uSoundtrack = 0; uSoundtrack < m_uSoundtrackCount; uSoundtrack++ )
    {
        for( UINT uSong = 0; uSong < m_aSoundtracks[ uSoundtrack ].GetSongCount(); uSong++ )
        {
            DWORD dwSongID;
            DWORD dwSongLength;
            WCHAR szSongName[ MAX_SONG_NAME ];

            m_aSoundtracks[ uSoundtrack ].GetSongInfo( uSong, &dwSongID, &dwSongLength, szSongName );
            HANDLE h = m_aSoundtracks[ uSoundtrack ].OpenSong( dwSongID );
            if( INVALID_HANDLE_VALUE == h )
            {
                OUTPUT_DEBUG_STRING( "Failed to open a soundtrack file.\n" );
                return E_FAIL;
            }
            CloseHandle( h );
        }
    }

    return S_OK;
}
#endif // _DEBUG




//
// CSoundtrack functions
//



//-----------------------------------------------------------------------------
// Name: GetSongInfo
// Desc: Returns information about the given song
//-----------------------------------------------------------------------------
void
CSoundtrack::GetSongInfo( UINT uSongIndex, DWORD * pdwID, DWORD * pdwLength, WCHAR szName[MAX_SONG_NAME] )
{
    if( m_fGameSoundtrack )
    {
        *pdwID = uSongIndex;
        *pdwLength = g_aGameSoundtrack[ uSongIndex ].dwLength;
        wcscpy( szName, g_aGameSoundtrack[ uSongIndex ].szName );
    }
    else
    {
        XGetSoundtrackSongInfo( m_uSoundtrackID, uSongIndex, pdwID, pdwLength, szName, MAX_SONG_NAME );
    }
}



//-----------------------------------------------------------------------------
// Name: OpenSong
// Desc: Opens the song with the given ID and returns a handle to the file
//-----------------------------------------------------------------------------
HANDLE
CSoundtrack::OpenSong( DWORD dwSongID )
{
    if( m_fGameSoundtrack )
        return CreateFile( g_aGameSoundtrack[ dwSongID ].szFilename, 
                           GENERIC_READ, 
                           FILE_SHARE_READ, 
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL );
    else
        return XOpenSoundtrackSong( dwSongID, FALSE );
}