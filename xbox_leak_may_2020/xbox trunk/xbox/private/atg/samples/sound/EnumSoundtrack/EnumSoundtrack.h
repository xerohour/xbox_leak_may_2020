//-----------------------------------------------------------------------------
// File: EnumSoundtrack.h
//
// Desc: Enum WMA Soundtrack sample
//
// Hist: 02.16.01 - New for March XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBENUM_SOUNDTRACK_H
#define XBENUM_SOUNDTRACK_H

#pragma warning( disable: 4702 )
#include <xtl.h>
#include <XbApp.h>
#include <XbFont.h>
#include <XbStopWatch.h>
#include <Xbhelp.h>
#include <XbSound.h>
#include "WmaFileStream.h"
#include "dsstdfx.h"

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <vector>
#pragma warning( pop )


//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Change song" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Change song" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Play song" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Stop playback" },
};

#define NUM_HELP_CALLOUTS 5



//-----------------------------------------------------------------------------
// Name: class CXBEnumSoundtrack
// Desc: Application object for enumerating soundtracks
//-----------------------------------------------------------------------------
class CXBEnumSoundtrack : public CXBApplication
{
    //-------------------------------------------------------------------------
    // Song class stores information about each song
    //-------------------------------------------------------------------------

    class Song
    {

        DWORD m_dwId;
        WCHAR m_strAlbum[ MAX_SOUNDTRACK_NAME ];
        WCHAR m_strSong[ MAX_SONG_NAME ];
        DWORD m_dwLength; // mS

    public:

        Song( DWORD dwId, const WCHAR* strAlbum, const WCHAR* strSong,
              DWORD dwLength );

        const WCHAR* GetAlbum() const;
        const WCHAR* GetSong() const;
        DWORD        GetLength() const;
        VOID         GetLength( CHAR* strMMSS ) const;
        HANDLE       Open( BOOL bAsync ) const;

    private:

        Song(); // disabled

    };

    //-------------------------------------------------------------------------
    // Enums
    //-------------------------------------------------------------------------

    enum Event
    {
        EV_NULL,            // No events
        EV_A_BUTTON,        // A button
        EV_START_BUTTON,    // Start button
        EV_B_BUTTON,        // B button
        EV_BACK_BUTTON,     // Back button
        EV_UP,              // Up Dpad or left joy
        EV_DOWN,            // Down Dpad or left joy

        EVENT_MAX
    };

    //-------------------------------------------------------------------------
    // Types
    //-------------------------------------------------------------------------

    typedef std::vector< Song > SongList;
    typedef SongList::size_type SongListIndex;

    //-------------------------------------------------------------------------
    // Data
    //-------------------------------------------------------------------------

    mutable CXBFont m_Font;             // Font renderer
    mutable CXBHelp m_Help;             // Help object
    BOOL            m_bDrawHelp;        // Should we draw help?

    CXBStopWatch    m_RepeatTimer;      // Controller button repeat timer
    FLOAT           m_fRepeatDelay;     // Time between button repeats
    SongList        m_SongList;         // List of songs
    SongListIndex   m_iCurrSong;        // Selected song
    SongListIndex   m_iTopSong;         // Topmost song displayed
    HANDLE          m_hSongFile;        // Song being played
    CWMAFileStream  m_Stream;           // WMA playback object
    LPDIRECTSOUND8  m_pDSound;          // DirectSound object
    BOOL            m_bPlaying;         // TRUE if track is playing

public:

    CXBEnumSoundtrack();

    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();

private:

    VOID InitSoundtrackList();
    VOID AddSoundtrackSongs( const XSOUNDTRACK_DATA& );
    VOID ValidateState() const;
    Event GetControllerEvent();
    VOID UpdateState( Event );
    VOID Start();
    VOID Stop();

    HRESULT DownloadEffectsImage(PCHAR pszScratchFile);  // downloads a default DSP image to the GP
};

#endif // XBENUM_SOUNDTRACK_H
