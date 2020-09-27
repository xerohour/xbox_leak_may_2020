//-----------------------------------------------------------------------------
// File: LoadSave.h
//
// Desc: Load and save menus
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_LOAD_SAVE_H
#define TECH_CERT_GAME_LOAD_SAVE_H

#include "Common.h"
#include <XbFont.h>
#include <XbStorageDevice.h>
#include <XbMemUnit.h>
#include <XbResource.h>
#include <XbSavedGame.h>
#include <XbStopWatch.h>
#include "cxfont.h"

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <deque>
#include <vector>
#pragma warning( pop )

// Forward references
struct XBGAMEPAD;




//-----------------------------------------------------------------------------
// Name: class LoadSave
// Desc: Load and save menus
//-----------------------------------------------------------------------------
class LoadSave
{
public:

    enum Mode
    {
        MODE_SAVE,      // Saving a game
        MODE_LOAD,      // Loading a game

        MODE_MAX
    };

private:

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
        DWORD dwState;                            // MU state; see MU_???
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
        VOID Render( BOOL bSelected, const LoadSave* pRContext ) const;
    };

    //-------------------------------------------------------------------------
    // Enums
    //-------------------------------------------------------------------------

    enum State
    {
        MENU_DEVICE,      // Select memory device
        MENU_GAMELIST,    // Game list menu
        GAME_SAVE,        // Saving game to device
        GAME_LOAD,        // Loading game from device
        BOX_OVERWRITE,    // Overwrite question box
        BOX_DELETE,       // Delete game question box
        BOX_DASH_CONFIRM, // Boot to dash question box
        MENU_MAIN,        // Return to main menu
        GAME_LOADED,      // Game successfully loaded
        GAME_SAVED,       // Game successfully saved

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
    mutable CXFont    m_XFontBigJPN;
    mutable CXFont    m_XFontMedJPN;
    mutable CXFont    m_XFontSmallJPN;
    Mode              m_Mode;         // Current mode
    State             m_State;        // Current menu/box

    MemUnitList       m_MemUnitList;  // MUs
    StorageDevList    m_DeviceList;   // User data regions
    SavedGameList     m_GameList;     // Games on the current storage device

    StorageDevIndex   m_iCurrDev;          // Current storage device
    StorageDevIndex   m_iLastMu;           // Last selected MU
    SavedGameIndex    m_iCurrGame;         // Current selected game
    SavedGameIndex    m_iTopGame;          // Topmost game visible on screen
    BOOL              m_bOverwriteMode;    // TRUE if overwriting saved game

    CXBStopWatch      m_RepeatTimer;          // Controller button repeat timer
    FLOAT             m_fRepeatDelay;         // Time between button repeats

    CXBStopWatch      m_MsgBoxTimer;                 // Time msg box displayed
    FLOAT             m_fMsgBoxSeconds;              // Max msg box display time; 0=forever
    WCHAR             m_strMessage[ MAX_MESSAGE ];   // Message box message
    Answer            m_Answer;                      // Yes/no
    State             m_NextState;                   // State after message box exit
    CXBStopWatch      m_LoadSaveTimer;               // Generic timer

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

    LPDIRECT3DTEXTURE8 m_ptGameImage;       // Save game image

    BYTE* m_pGameData;                      // Pointer to information to be saved
    DWORD m_dwGameDataSize;                 // Size of information to be saved
    WCHAR m_strGameName[ MAX_GAMENAME ];    // Readable game name
    DWORD m_dwFileErr;                      // Last file I/O error

public:

    LoadSave();

    VOID    Start( Mode );  // Begin load
    VOID    End();          // End load

    HRESULT FrameMove( const XBGAMEPAD* );  // Frame advance
    HRESULT Render();                       // Draw frame

    VOID    SetGameData( const WCHAR* strName, DWORD dwSize, 
                         LPDIRECT3DTEXTURE8 pGameImage = NULL );
    DWORD   GetGameDataSize() const;
    BYTE*   GetGameDataPtr();

    BOOL    WasCancelled() const;
    BOOL    IsGameLoaded() const;
    BOOL    WasGameSaved() const;
    VOID    FreeGameData();

    static DWORD GetGameSaveMaxSize( DWORD dwGameDataSize );

private:

    VOID ValidateState() const;
    BOOL SaveGame();
    LPDIRECT3DTEXTURE8 GetGameImage( const WCHAR* ) const;
    BOOL SaveGame( const CHAR* );
    BOOL LoadGame();

    static BOOL GetSignature( const BYTE*, DWORD, XCALCSIG_SIGNATURE& );

    BOOL GameExists( const WCHAR* ) const;

    Event GetEvent( const XBGAMEPAD* );
    Event GetControllerEvent( const XBGAMEPAD* );
    Event GetMemoryUnitEvent();
    static DWORD GetMuState( DWORD );
    VOID  BuildMemoryUnitList();

    VOID UpdateState( Event );
    VOID UpdateStateDevice( Event );
    VOID UpdateStateGameList( Event );
    VOID UpdateStateOverwrite( Event );
    VOID UpdateStateDelete( Event );
    VOID UpdateStateDashConfirm( Event );

    VOID StartMsgBox( const WCHAR*, State, FLOAT = 0.0f );
    VOID StartMsgBoxSave();
    VOID StartMsgBoxFree();

    VOID RenderDevice() const;
    VOID RenderGameList() const;
    VOID RenderOverwrite() const;
    VOID RenderDelete() const;
    VOID RenderDashConfirm() const;
    VOID RenderYesNo() const;

    VOID DrawButton( FLOAT, WCHAR ) const;
    VOID DrawMsgBox( const D3DXVECTOR4&, const WCHAR*, CXBFont&, DWORD ) const;
    VOID DrawMsgBox( const D3DXVECTOR4&, const WCHAR*, CXFont&, DWORD ) const;
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

};




#endif // TECH_CERT_GAME_LOAD_SAVE_H
