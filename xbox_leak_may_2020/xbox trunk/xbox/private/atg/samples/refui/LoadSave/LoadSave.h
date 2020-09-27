//-----------------------------------------------------------------------------
// File: LoadSave.h
//
// Desc: Load and save game reference UI
//
// Hist: 02.06.01 - New for March XDK release 
//       03.14.01 - Metadata updates for April XDK release
//       03.27.01 - Certification updates for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBLOADSAVE_H
#define XBLOADSAVE_H

#pragma warning( disable: 4702 4786 )
#include <xtl.h>
#include <XbApp.h>
#include <XbFont.h>
#include <XbStorageDevice.h>
#include <XbMemUnit.h>
#include <XbResource.h>
#include <XbSavedGame.h>
#include <XbStopWatch.h>

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <deque>
#include <vector>
#pragma warning( pop )

// Forward declarations
class File;




//-----------------------------------------------------------------------------
// Name: class CXBLoadSave
// Desc: Application object for load and save game reference UI
//-----------------------------------------------------------------------------
class CXBLoadSave : public CXBApplication
{
    //-------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------

    // We never display a block count greater than "50,000+" to meet
    // cert requirements
    static const int MAX_BLOCKS_STR = 8;

    // Largest message in message box
    static const int MAX_MESSAGE = 128;

    // MU names include default name plus user-defined name
    static const DWORD MAX_DEVNAME = MAX_MUNAME + 32;

    //-------------------------------------------------------------------------
    // Classes
    //-------------------------------------------------------------------------

    // Preformats storage device values
    struct StorageDev : public CXBStorageDevice
    {
        enum MuState
        {
            MU_UNUSABLE = 1 << 0,
            MU_FULL     = 1 << 1,
        };

        FLOAT fPercentUsed;                       // blocks used/blocks free
        CHAR  strTotalBlocks[ MAX_BLOCKS_STR ];   // Formatted total blocks string
        CHAR  strFreeBlocks[ MAX_BLOCKS_STR ];    // Formatted free blocks string
        DWORD dwState;                            // MU unusable/full
        BOOL  bIsMU;                              // TRUE if Memory Unit

        LPDIRECT3DTEXTURE8  pTextureGood;
        LPDIRECT3DTEXTURE8  pTextureBad;
        LPDIRECT3DTEXTURE8  pWideTexture;
        LPDIRECT3DTEXTURE8  pTexSel;
        D3DXVECTOR4         rcRegion;
        D3DXVECTOR4         rcImage;
        D3DXVECTOR4         rcUsed;

        explicit StorageDev( CHAR chDriveLetter = 0, BOOL bIsMU = FALSE );
        VOID SetDevice( CHAR chDriveLetter, BOOL bIsMU );
        VOID SetDeviceState( DWORD );
        VOID Refresh();
        VOID Render( BOOL bSelected, const CXBLoadSave* pRContext ) const;

    };

    //-------------------------------------------------------------------------
    // Enums
    //-------------------------------------------------------------------------

    enum Mode
    {
        MODE_SAVE,      // Saving a game
        MODE_LOAD,      // Loading a game

        MODE_MAX
    };

    enum State
    {
        START_SCREEN,     // Initial screen
        MENU_MAIN,        // Save/Load menu
        MENU_DEVICE,      // Select memory device
        MENU_GAMELIST,    // Game list menu
        GAME_SAVE,        // Saving game to device
        GAME_LOAD,        // Loading game from device
        BOX_OVERWRITE,    // Overwrite question box
        BOX_DELETE,       // Delete game question box
        BOX_DASH_CONFIRM, // Boot to dash question box

        STATE_MAX
    };

    enum Event
    {
        EV_NULL,            // No events
        EV_A_BUTTON,        // A button
        EV_START_BUTTON,    // Start button
        EV_B_BUTTON,        // B button
        EV_BACK_BUTTON,     // Back button
        EV_UP,              // Up Dpad or left joy
        EV_DOWN,            // Down Dpad or left joy
        EV_LEFT,            // Left Dpad or left joy
        EV_RIGHT,           // Right Dpad or left joy
        EV_MU_CHANGE,       // MU removed/inserted
        EV_X_BUTTON,        // X button
        EV_Y_BUTTON,        // Y button

        EVENT_MAX
    };

    enum Answer
    {
        ANSWER_YES,
        ANSWER_NO,

        ANSWER_MAX
    };

    enum DeviceZone
    {
        MM_SAVE,
        MM_LOAD,
        DS_MAINDESC,
        DS_BLOCKBOX,
        DS_TITLE,
        GS_MAINDESC,
        GS_BLOCKBOX,
        GS_DEVICEIMG,
        GS_META,
        GS_METAIMG,
        GS_METATEXT,
        GS_GAMELIST,
        GS_UP,
        GS_DOWN,
        MB_MESSAGE,
        MB_PROGRESS,
        MB_YES,
        MB_NO
    };


    //-------------------------------------------------------------------------
    // Types
    //-------------------------------------------------------------------------

    typedef std::vector< CXBMemUnit >   MemUnitList;
    typedef std::vector< StorageDev >   StorageDevList;
    typedef StorageDevList::size_type   StorageDevIndex;
    typedef std::deque< CXBSavedGame >  SavedGameList;
    typedef SavedGameList::size_type    SavedGameIndex;

    //-------------------------------------------------------------------------
    // Data
    //-------------------------------------------------------------------------

	CXBPackedResource m_xprResource;  // Packed resource (textures) for the LoadSave UI

    DWORD             m_iLang;        // Current language
    mutable CXBFont   m_FontBig;      // Big font
    mutable CXBFont   m_FontMed;      // Medium font
    mutable CXBFont   m_FontSmall;    // Smaller font
    mutable CXBFont   m_FontBtn;      // Xboxdings font buttons
    Mode              m_Mode;         // Current mode
    State             m_State;        // Current menu/box

    MemUnitList       m_MemUnitList;  // MUs
    StorageDevList    m_DeviceList;   // User data regions
    SavedGameList     m_GameList;     // Games on the current storage device

    StorageDevIndex   m_iCurrDev;          // Current storage device
    StorageDevIndex   m_iLastMu;           // Last selected MU
    SavedGameIndex    m_iCurrGame;         // Current selected game
    SavedGameIndex    m_iTopGame;          // Topmost game visible on screen
    DWORD             m_dwSavedGameBytes;  // Size of game save data
    BOOL              m_bOverwriteMode;    // TRUE if overwriting saved game

    CXBStopWatch      m_RepeatTimer;          // Controller button repeat timer
    FLOAT             m_fRepeatDelay;         // Time between button repeats

    CXBStopWatch      m_MsgBoxTimer;                 // Time msg box displayed
    FLOAT             m_fMsgBoxSeconds;              // Max msg box display time; 0=forever
    WCHAR             m_strMessage[ MAX_MESSAGE ];   // Message box message
    Answer            m_Answer;                      // Yes/no
    State             m_NextState;                   // State after message box exit

    LPDIRECT3DVERTEXBUFFER8     m_pvbBackground;    // Quad for background
    LPDIRECT3DTEXTURE8          m_ptBackground;     // Texture for background
    LPDIRECT3DTEXTURE8          m_ptMsgBox;         // Message Box texture
    LPDIRECT3DTEXTURE8          m_ptPlainBack;      // Plain background

    LPDIRECT3DTEXTURE8 m_ptXbox;    // Xbox graphic
    LPDIRECT3DTEXTURE8 m_ptMU;      // MU graphic
    LPDIRECT3DTEXTURE8 m_ptWideMU;  // Wide MU Graphic
    LPDIRECT3DTEXTURE8 m_ptXboxSel; // Xbox selection background
    LPDIRECT3DTEXTURE8 m_ptMUSel;   // MU selection background
    LPDIRECT3DTEXTURE8 m_ptUp;      // Up arrow
    LPDIRECT3DTEXTURE8 m_ptDown;    // Down arrow
    LPDIRECT3DTEXTURE8 m_ptMUBad;   // Corrupt MU graphic

    DWORD m_dwFileErr;              // Last read/write error

public:

    CXBLoadSave();

    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();

private:

    VOID ValidateState() const;
    BOOL SaveGame();
    LPDIRECT3DTEXTURE8 GetGameImage( const WCHAR* ) const;
    BOOL SaveGame( const CHAR* );
    BOOL LoadGame();
    BOOL GameExists( const WCHAR* ) const;

    Event GetEvent();
    Event GetControllerEvent();
    Event GetMemoryUnitEvent();
    static DWORD GetMuState( DWORD );

    VOID UpdateState( Event );
    VOID UpdateStateStartScreen( Event );
    VOID UpdateStateMainMenu( Event );
    VOID UpdateStateDevice( Event );
    VOID UpdateStateGameList( Event );
    VOID UpdateStateOverwrite( Event );
    VOID UpdateStateDelete( Event );
    VOID UpdateStateDashConfirm( Event );

    VOID StartMsgBox( const WCHAR*, State, FLOAT = 0.0f );
    VOID StartMsgBoxSave();
    VOID StartMsgBoxFree();

    VOID RenderStartScreen() const;
    VOID RenderMainMenu() const;
    VOID RenderDevice() const;
    VOID RenderGameList() const;
    VOID RenderOverwrite() const;
    VOID RenderDelete() const;
    VOID RenderDashConfirm() const;
    VOID RenderYesNo() const;
    VOID DrawButton( FLOAT fX, WCHAR ) const;
    VOID DrawMsgBox( const D3DXVECTOR4&, const WCHAR*, CXBFont&, DWORD ) const;
    VOID RenderTile( const D3DXVECTOR4&, const LPDIRECT3DTEXTURE8,
                     BOOL = FALSE ) const;
    VOID RenderBar( const D3DXVECTOR4&, FLOAT, DWORD ) const;
    VOID RenderSelection( const D3DXVECTOR4&, FLOAT ) const;

    BOOL IsDeviceInserted( DWORD ) const;
    BOOL AnyMemoryUnitsInserted() const;
    BOOL IsSpaceAvail( DWORD* = NULL ) const;
    VOID DeleteGame();
    VOID DeleteGameFromList();

    static bool SortByLastWriteTime( const CXBSavedGame&, const CXBSavedGame& );

    VOID BuildGameList();
    VOID GetSavedGameFileName( CHAR* ) const;
    DWORD GetSavedGameSize() const;

    static VOID GenerateSavedGameName( WCHAR* );

};

#endif // XBLOADSAVE_H
