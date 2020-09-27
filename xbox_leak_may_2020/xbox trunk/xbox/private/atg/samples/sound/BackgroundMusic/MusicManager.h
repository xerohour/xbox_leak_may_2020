//-----------------------------------------------------------------------------
// File: MusicManager.h
//
// Desc: Class definition for the CMusicManager class.  This is the real
//       playback engine.
//
// Hist: 8.20.01 - New for October XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _MUSICMANAGER_H_
#define _MUSICMANAGER_H_

#include <xtl.h>
#include <dsound.h>

// Packet size is set up to be 2048 samples
// Samples are 16-bit and stereo
#define PACKET_SIZE ( 2048 * 2 * 2 )
#define PACKET_TIME ( 2048.0f / 44100.0f )

#define PACKET_COUNT 43     // Base number of packets
#define EXTRA_PACKETS 10    // Extra packets to get through the transition
#define FADE_TIME ( PACKET_COUNT * PACKET_TIME )


typedef enum {
    MM_STOPPED,
    MM_PAUSED,
    MM_PLAYING,
} MM_STATE;



//-----------------------------------------------------------------------------
// Name: class CSoundtrack
// Desc: Abstraction layer for soundtracks that help merge together game
//       soundtracks and user soundtracks stored on the Xbox hard drive
//-----------------------------------------------------------------------------
class CSoundtrack
{
public:
    CSoundtrack() {}

    VOID    GetSoundtrackName( WCHAR * szName )
        { wcscpy( szName, m_szName ); }
    UINT    GetSongCount()
        { return m_uSongCount; }

    VOID    GetSongInfo( UINT uSongIndex, DWORD * pdwID, DWORD * pdwLength, WCHAR * szName );
    HANDLE  OpenSong( DWORD dwSongID );
    
    WCHAR       m_szName[MAX_SOUNDTRACK_NAME];
    UINT        m_uSongCount;
    BOOL        m_fGameSoundtrack;
    union {
        UINT    m_uSoundtrackID;
        UINT    m_uSoundtrackIndex;
    };
};


//-----------------------------------------------------------------------------
// Name: class CMusicManager
// Desc: Background music engine class.  Spawns its own worker thread when
//       initialized.  Cost of calls from main rendering loop is negligible.
//       All public methods are safe to be called from main rendering loop, 
//       however, they do not all take effect immediately.
//-----------------------------------------------------------------------------
class CMusicManager
{
public:
    CMusicManager();
    ~CMusicManager();
    HRESULT Initialize( BOOL bSpinThread = TRUE );  // Initialize MusicManager

    HRESULT Play();                                 // Start playing
    HRESULT Stop();                                 // Stop playback
    HRESULT Pause();                                // Pause playback
    MM_STATE GetStatus() { return m_mmOldState; }   // Returns current playback status
    HRESULT SetRandom( BOOL fRandom );              // Change random mode
    BOOL    GetRandom() { return m_fRandom; }       // Get random mode
    HRESULT SetGlobal( BOOL fGlobal );              // Toggle global mode
    BOOL    GetGlobal() { return m_fGlobal; }       // Get global mode
    HRESULT SetVolume( LONG lVolume );              // Set volume level
    LONG    GetVolume() { return m_lVolume; }       // Get volume level

    // Returns info on currently playing song
    HRESULT GetCurrentInfo( WCHAR * szSoundtrack, WCHAR * szSong, DWORD * pdwLength );
    FLOAT   GetPlaybackPosition();                  // Returns position in current song
    HRESULT NextSoundtrack();                       // Switch to next soundtrack
    HRESULT NextSong();                             // Switch to next song
    HRESULT RandomSong( BOOL fGlobal = TRUE );      // Switch to a random song

    HRESULT MusicManagerDoWork();                   // Performs processing

private:
    HRESULT LoadSoundtracks();                      // Fill our soundtrack cache
    HRESULT SelectSoundtrack( DWORD dwSoundtrack ); // Switch to a soundtrack
    HRESULT SelectSong( DWORD dwSong );             // Switch to a song
    HRESULT FindNextSong();                         // Determine next song to play

    HRESULT Prepare();          // Get ready to decode and playback current song
    HRESULT Cleanup();          // Cleanup from playing current song
    HRESULT Process();          // Workhorse function - update state and manage packets

    BOOL    FindFreePacket( DWORD *pdwPacket, DWORD dwStream );     // Looks for a free packet
    HRESULT ProcessSource( DWORD dwPacket, XMEDIAPACKET * pxmp );   // Fills packet from source
    HRESULT ProcessStream( DWORD dwPacket, XMEDIAPACKET * pxmp );   // Submist packet to stream

    // Give the threadproc access to our Process() method
    friend DWORD WINAPI MusicManagerThreadProc(LPVOID lpParameter );

    // Stream callback routine for updating playback position
    friend static void CALLBACK StreamCallback( LPVOID pStreamContext, LPVOID pPacketContext, DWORD dwStatus );

    // State variables
    BOOL                    m_fGlobal;                          // TRUE to loop/randomize globally
    BOOL                    m_fRandom;                          // TRUE to move randomly
    LONG                    m_lVolume;                          // Volume level
    MM_STATE                m_mmOldState;                       // Old/current state
    MM_STATE                m_mmState;                          // New state set from game thread
    DWORD                   m_dwPacketsCompleted;               // # of packets completed

    // Music information
    CSoundtrack *           m_aSoundtracks;                     // List of soundtracks
    UINT                    m_uSoundtrackCount;                 // Total number of soundtracks
    UINT                    m_uCurrentSoundtrack;               // Currently selected soundtrack
    WCHAR                   m_szSong[MAX_SONG_NAME];            // Current song name
    DWORD                   m_dwLength;                         // Length of current song
    DWORD                   m_dwSong;                           // Current song index
    DWORD                   m_dwSongID;                         // Current song ID
    HANDLE                  m_hDecodingFile;                    // Song file handle

    // Decode/playback members
    XWmaFileMediaObject *   m_pDecoder;                         // WMA decoder
    IDirectSoundStream *    m_pStream[2];                       // Output streams
    DWORD                   m_dwStream;                         // Current stream
    BYTE *                  m_pbSampleData;                     // Audio sample data

    // Packet status values
    DWORD                   m_adwPacketStatus[PACKET_COUNT+EXTRA_PACKETS];

    // Thread/Synchronization
    HANDLE                  m_hThread;                          // Worker thread
    HANDLE                  m_hShutdownEvent;                   // Shutdown event

#if _DEBUG
    HRESULT DebugVerify();  // Verify all game soundtracks are present
#endif
};

#endif // _MUSICMANAGER_H_
