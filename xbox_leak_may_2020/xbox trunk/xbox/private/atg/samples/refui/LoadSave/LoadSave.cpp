//-----------------------------------------------------------------------------
// File: LoadSave.cpp
//
// Desc: Load and save game reference UI
//
// Hist: 02.06.01 - New for March XDK release 
//       03.12.01 - Updated for April XDK release
//       04.13.01 - Updated for May XDK release -- full localization, more
//                  accurate file size calculation
//       05.21.01 - Updated for June XDK release -- new signature calculations,
//                  buttons, text revisions, UI updates
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// This sample is intended to show appropriate functionality only. Please do 
// not lift the graphics for use in your game. A description of the user 
// research that went into the creation of this sample is located in the 
// XDK documentation at Developing for Xbox - Reference User Interface.
// For functionality ideas see the Technical Certification game
//
//-----------------------------------------------------------------------------
#include "LoadSave.h"
#include <XbConfig.h>
#include <cassert>
#include <algorithm>
#include "Text.h"

// The following header file is generated from "LoadSaveResource.rdf" file
// using the Bundler tool. In addition to the header, the tool outputs a binary
// file (LoadSaveResource.xpr) which contains compiled (i.e. bundled) resources
// and is loaded at runtime using the CXBPackedResource class.
#include "LoadSaveResource.h"




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
// Interesting regions of screen UI
const D3DXVECTOR4 g_avUIRects[] = 
{
    // VSafe:    > 40   > 30   < 600  < 450
    //           Left,  Top,   Right, Bottom
    D3DXVECTOR4( 210,   126,   430,   176 ),  // Main: "Save"
    D3DXVECTOR4( 210,   200,   430,   252 ),  // Main: "Load"
    D3DXVECTOR4( 100,   319,   388,   366 ),  // Device: Big description
    D3DXVECTOR4( 225,   305,   516,   378 ),  // Device: Blocks box
    D3DXVECTOR4(  80,    36,   560,    86 ),  // Device: Title box
    D3DXVECTOR4( 229,    43,   458,    93 ),  // Game: Big description
    D3DXVECTOR4( 352,    34,   576,   105 ),  // Game: Blocks box
    D3DXVECTOR4(  64,    34,   220,   103 ),  // Game: Device Image
    D3DXVECTOR4( 340,   111,   576,   366 ),  // Game: Meta desc
    D3DXVECTOR4( 393,   130,   523,   260 ),  // Game: Meta img
    D3DXVECTOR4( 350,   270,   566,   370 ),  // Game: Meta text
    D3DXVECTOR4(  80,   121,   320,   366 ),  // Game: Game listing area
    D3DXVECTOR4( 228,    93,   272,   116 ),  // Game: Up arrow
    D3DXVECTOR4( 228,   371,   272,   394 ),  // Game: Down arrow
    D3DXVECTOR4( 170,   146,   470,   306 ),  // MsgBox: Box
    D3DXVECTOR4( 190,   276,   450,   296 ),  // MsgBox: Progress
    D3DXVECTOR4( 220,   266,   300,   286 ),  // MsgBox: "Yes"
    D3DXVECTOR4( 340,   266,   420,   286 ),  // MsgBox: "No"
};

// Must be this far from center on 0.0 - 1.0 scale
const FLOAT fTHUMB_DEADZONE = 0.35f;

// 2 MUs per port
const DWORD MAX_MEMORY_UNITS = 2 * XGetPortCount();

// Maximum number of storage devices: HD plus all MUs
const DWORD MAX_STORAGE_DEVICES = 1 + MAX_MEMORY_UNITS;

// Maximum number of saved games displayed on screen at any one time
const INT MAX_GAMES_DISPLAYED = 7;

// Maximum number of characters displayed in MU name during error message
// TCR 3-10 Memory Unit Personalization requires we display at least 10 
// characters of the personalized name. The name is formatted 
// "Xbox MU xx (PersonalName)", which includes 12 leading characters.
const DWORD MAX_MU_DISPLAY = 12 + 10;

// Maximum number of saved games per device for cert requirements
const DWORD MAX_SAVED_GAMES = 4096;

// Maximum number of blocks ever displayed for cert requirments
const DWORD MAX_BLOCKS = 50000;

// Name of saved game data file
const CHAR* const strSAVE_FILE = "game.xsv";

// Ellipses
const WCHAR* const strELLIPSES = L"...";

// For maximum read/write efficiency, we choose a large chunk size.
// 128K is the maximum IDE buffer, which should provide the best performance.
const DWORD FILE_CHUNK_BYTES = 128 * 1024;
const DWORD FILE_CHUNK_DWORDS = FILE_CHUNK_BYTES / sizeof(DWORD);

// This sample generates fake saved game data that is some random 
// size between MIN and MAX
const DWORD MIN_SAVE_DATA_CHUNKS = 1;  // 128K
const DWORD MAX_SAVE_DATA_CHUNKS = 16; // 2 MB

// Meta data image sizes
const DWORD IMAGE_META_HDR_SIZE = 2048;             // 2K
const DWORD IMAGE_META_DATA_SIZE = (64 * 64) / 2; // DXT1 is 4 bits per pixel

// Controller repeat values
const FLOAT fINITIAL_REPEAT = 0.333f; // 333 mS
const FLOAT fSTD_REPEAT = 0.04f;      // 40 mS

// Used bar color values
const FLOAT fBAR_FULL = 0.95f;
const FLOAT fBAR_WARN = 0.85f;

// Maximum message box time for status messages
const FLOAT fSTATUS_SECONDS = 0.5f;

// Minimum message box time for device activity messages
const FLOAT fDEVICE_SECONDS = 3.0f;
const FLOAT fHD_DEVICE_SECONDS = 1.0f;

const D3DCOLOR BARCOLOR_NORMAL   = 0xFF00FF00; // Green
const D3DCOLOR BARCOLOR_WARNING  = 0xFFFFFF00; // Yellow
const D3DCOLOR BARCOLOR_FULL     = 0xFFFF0000; // Red
const D3DCOLOR BARCOLOR_PROGRESS = 0xFF00FF00; // Green
const D3DCOLOR BARBORDER         = 0xFF000000; // Black
const D3DCOLOR MB_TEXT_COLOR     = 0xFF000000; // Black

// Space between text and message box border
const FLOAT MB_TEXT_OFFSET = 8.0f;

// Ratio between game and inter-game space
const FLOAT GAME_SPACE_MULTIPLE = 5.0f;

// Selection offset / inter-game space
const FLOAT GAME_SELECTION_RATIO = 3.0f / 4.0f;

// Selection offset for main menu
const FLOAT MAIN_MENU_OFFSET = 20.0f;

const FLOAT BUTTON_Y = 400.0f;          // button text line
const FLOAT BUTTON_OFFSET = 40.0f;      // space between button and text
const D3DCOLOR BUTTON_COLOR = 0xFFFFFFFF;
const D3DCOLOR BUTTON_TEXT  = 0xF0000000;

// Xboxdings font button mappings
const WCHAR BTN_A = L'A';
const WCHAR BTN_B = L'B';
const WCHAR BTN_Y = L'D';

const DWORD FVF_CUSTOMVERTEX = D3DFVF_XYZRHW | D3DFVF_TEX1; // see CUSTOMVERTEX
const DWORD FVF_BARVERTEX = D3DFVF_XYZRHW;




//-----------------------------------------------------------------------------
// Name: struct CUSTOMVERTEX
// Desc: For background vertex buffer
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR4 p;
    D3DXVECTOR2 t;
};




//-----------------------------------------------------------------------------
// Name: struct XValidationHeader
// Desc: Header information from saved game data file
//-----------------------------------------------------------------------------
struct XValidationHeader
{
    // Length of the file, including header, in bytes
    DWORD dwFileLength;

    // File signature (secure hash of file data)
    XCALCSIG_SIGNATURE Signature;
};




//-----------------------------------------------------------------------------
// Name: class File
// Desc: Helper class for common file operations
//-----------------------------------------------------------------------------
class File
{

    HANDLE m_hFile;

public:

    //-------------------------------------------------------------------------
    // Name: File()
    // Desc: Construct file object
    //-------------------------------------------------------------------------
    File() : m_hFile( INVALID_HANDLE_VALUE )
    {
    }


    //-------------------------------------------------------------------------
    // Name: ~File()
    // Desc: Close file object
    //-------------------------------------------------------------------------
    ~File()
    {
        Close();
    }


    //-------------------------------------------------------------------------
    // Name: Open()
    // Desc: Open the given file; fails if file doesn't exist
    //-------------------------------------------------------------------------
    BOOL Open( const CHAR* strFile, DWORD dwAccess, DWORD dwAttributes = 0 )
    {
        Close();
        m_hFile = CreateFile( strFile, dwAccess, 0, NULL, OPEN_EXISTING,
                              dwAttributes, NULL );
        return( m_hFile != INVALID_HANDLE_VALUE );
    }


    //-------------------------------------------------------------------------
    // Name: Create()
    // Desc: Create the given file
    //-------------------------------------------------------------------------
    BOOL Create( const CHAR* strFile, DWORD dwAccess = GENERIC_WRITE | GENERIC_READ,
                 DWORD dwAttributes = 0 )
    {
        Close();
        m_hFile = CreateFile( strFile, dwAccess, 0, NULL, CREATE_ALWAYS,
                              dwAttributes, NULL );
        return( m_hFile != INVALID_HANDLE_VALUE );
    }


    //-------------------------------------------------------------------------
    // Name: Close()
    // Desc: Close the file handle
    //-------------------------------------------------------------------------
    VOID Close()
    {
        if( m_hFile != INVALID_HANDLE_VALUE )
        {
            CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;
        }
    }


    //-------------------------------------------------------------------------
    // Name: IsOpen()
    // Desc: TRUE if file is open
    //-------------------------------------------------------------------------
    BOOL IsOpen() const
    {
        return( m_hFile != INVALID_HANDLE_VALUE );
    }


    //-------------------------------------------------------------------------
    // Name: GetSize()
    // Desc: Returns the file size in bytes
    //-------------------------------------------------------------------------
    DWORD GetSize() const
    {
        assert( IsOpen() );
        return GetFileSize( m_hFile, NULL );
    }


    //-------------------------------------------------------------------------
    // Name: SetPos()
    // Desc: Set the file pointer to the given position (relative to the
    //       beginning of the file)
    //-------------------------------------------------------------------------
    VOID SetPos( DWORD dwFilePos ) const
    {
        assert( IsOpen() );
        assert( LONG(dwFilePos) >= 0 );
        SetFilePointer( m_hFile, LONG(dwFilePos), NULL, FILE_BEGIN );
    }


    //-------------------------------------------------------------------------
    // Name: Read()
    // Desc: Read data from an open file
    //-------------------------------------------------------------------------
    BOOL Read( VOID* pBuffer, DWORD dwBytesToRead, DWORD& dwBytesRead ) const
    {
        assert( IsOpen() );
        return ReadFile( m_hFile, pBuffer, dwBytesToRead, &dwBytesRead, NULL );
    }


    //-------------------------------------------------------------------------
    // Name: Write()
    // Desc: Write data to an open file
    //-------------------------------------------------------------------------
    BOOL Write( const VOID* pBuffer, DWORD dwBytesToWrite ) const
    {
        assert( IsOpen() );
        DWORD dwBytesWritten;
        BOOL bSuccess = WriteFile( m_hFile, pBuffer, dwBytesToWrite,
                                   &dwBytesWritten, NULL );
        return( bSuccess && dwBytesToWrite == dwBytesWritten );
    }

}; // end File class




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBLoadSave xbApp;

    if( FAILED( xbApp.Create() ) )
        return;

    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBLoadSave::StorageDev()
// Desc: Construct private storage device object
//-----------------------------------------------------------------------------
CXBLoadSave::StorageDev::StorageDev( CHAR chDriveLetter, BOOL bIsMu )
:
    CXBStorageDevice( chDriveLetter ),
    fPercentUsed    ( 0.0f ),
    strTotalBlocks  (),
    strFreeBlocks   (),
    dwState         ( 0 ),
    bIsMU           ( bIsMu ),
    pTextureGood    ( NULL ),
    pTextureBad     ( NULL ),
    pWideTexture    ( NULL ),
    pTexSel         ( NULL ),
    rcRegion        (),
    rcImage         (),
    rcUsed          ()
{
    Refresh();
}




//-----------------------------------------------------------------------------
// Name: CXBLoadSave::StorageDev::SetDevice()
// Desc: Update the storage device to the given logical drive
//-----------------------------------------------------------------------------
VOID CXBLoadSave::StorageDev::SetDevice( CHAR chDriveLetter, BOOL bIsMu )
{
    SetDrive( chDriveLetter );
    bIsMU = bIsMu;
    Refresh();
    dwState = 0;
}




//-----------------------------------------------------------------------------
// Name: CXBLoadSave::StorageDev::SetDeviceState()
// Desc: Mark the device as unusable/full
//-----------------------------------------------------------------------------
VOID CXBLoadSave::StorageDev::SetDeviceState( DWORD dwState )
{
    SetDevice( 0, bIsMU );
    CXBLoadSave::StorageDev::dwState = dwState;
    fPercentUsed = 0.0f;
    if( dwState == StorageDev::MU_FULL )
        fPercentUsed = 1.0f;
}




//-----------------------------------------------------------------------------
// Name: CXBLoadSave::StorageDev::Refresh()
// Desc: Cache device data in printable strings
//-----------------------------------------------------------------------------
VOID CXBLoadSave::StorageDev::Refresh()
{
    // Clear the strings
    *strTotalBlocks = 0;
    *strFreeBlocks = 0;

    // Get the total and used bytes
    ULONGLONG qwTotalBytes;
    ULONGLONG qwUsedBytes;
    ULONGLONG qwFreeBytes;
    if( GetSize( qwTotalBytes, qwUsedBytes, qwFreeBytes ) )
    {
        // Convert to blocks
        DWORD dwBlockSize   = CXBStorageDevice::GetBlockSize();
        DWORD dwTotalBlocks = DWORD( qwTotalBytes / dwBlockSize );
        DWORD dwUsedBlocks  = DWORD( qwUsedBytes / dwBlockSize );
        DWORD dwFreeBlocks  = dwTotalBlocks - dwUsedBlocks;

        // TCR 3-65 Computation of Total MU Capacity
        // Must allow one block for file system data
        /*
           This requirement is currently being reconsidered...
           Contact xboxds@xbox.com for additional information.

        if( bIsMU )
        {
            if( dwTotalBlocks > 1 )
                dwTotalBlocks -= 1;
            else
                dwTotalBlocks = 0;
        }
        */

        // Format with thousands separators

        // To meet cert requirements, any amount over 50,000 blocks is
        // formatted "50,000+"
        if( dwTotalBlocks > MAX_BLOCKS )
            lstrcpyA( strTotalBlocks, strMAX_BLOCKS[ CXBConfig::GetLanguage() ] );
        else
            CXBConfig::FormatInt( dwTotalBlocks, strTotalBlocks );

        if( dwFreeBlocks > MAX_BLOCKS )            
            lstrcpyA( strFreeBlocks, strMAX_BLOCKS[ CXBConfig::GetLanguage() ] );
        else
            CXBConfig::FormatInt( dwFreeBlocks,  strFreeBlocks );

        // Graphical representation must only account for the high 50,000
        // block range. If the device has more than 50,000 blocks, adjust
        // total to 50,000, and scale "used" into the 50,000 section.
        if( dwTotalBlocks > MAX_BLOCKS )
        {
            dwTotalBlocks = MAX_BLOCKS;
            dwUsedBlocks = ( dwFreeBlocks > MAX_BLOCKS ) ? 0 : 
                                                    MAX_BLOCKS - dwFreeBlocks;
        }

        fPercentUsed = FLOAT( dwUsedBlocks ) / FLOAT( dwTotalBlocks );
    }
}




//-----------------------------------------------------------------------------
// Name: StorageDev::Render
// Desc: Renders the storage device image
//-----------------------------------------------------------------------------
VOID CXBLoadSave::StorageDev::Render( BOOL bSelected, 
                                      const CXBLoadSave* pContext ) const
{
    // Selection highlight
    if( bSelected )
        pContext->RenderSelection( rcRegion, 0 );

    // MU or Xbox image
    pContext->RenderTile( rcImage, ( dwState != 0 ) ? pTextureBad : pTextureGood );

    // Used bar
    DWORD dwColor = BARCOLOR_NORMAL;
    if( fPercentUsed > fBAR_FULL )
        dwColor = BARCOLOR_FULL;
    else if( fPercentUsed > fBAR_WARN )
        dwColor = BARCOLOR_WARNING;
    pContext->RenderBar( rcUsed, fPercentUsed, dwColor );
}




//-----------------------------------------------------------------------------
// Name: CXBLoadSave()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBLoadSave::CXBLoadSave()
:
    CXBApplication     (),
    m_xprResource      (),
    m_iLang            ( 0 ),
    m_FontBig          (),
    m_FontMed          (),
    m_FontSmall        (),
    m_FontBtn          (),
    m_Mode             ( MODE_SAVE ),
    m_State            ( START_SCREEN ),
    m_MemUnitList      (),
    m_DeviceList       (),
    m_GameList         (),
    m_iCurrDev         ( 0 ),
    m_iLastMu          ( 1 ),
    m_iCurrGame        ( 0 ),
    m_iTopGame         ( 0 ),
    m_dwSavedGameBytes ( 0 ),
    m_bOverwriteMode   ( FALSE ),
    m_RepeatTimer      ( FALSE ),
    m_fRepeatDelay     ( fINITIAL_REPEAT ),
    m_MsgBoxTimer      ( FALSE ),
    m_fMsgBoxSeconds   ( 0.0f ),
    m_strMessage       (),
    m_Answer           ( ANSWER_YES ),
    m_NextState        ( MENU_MAIN ),
    m_pvbBackground    ( NULL ),
    m_ptBackground     ( NULL ),
    m_ptMsgBox         ( NULL ),
    m_ptPlainBack      ( NULL ),
    m_ptXbox           ( NULL ),
    m_ptMU             ( NULL ),
    m_ptWideMU         ( NULL ),
    m_ptXboxSel        ( NULL ),
    m_ptMUSel          ( NULL ),
    m_ptUp             ( NULL ),
    m_ptDown           ( NULL ),
    m_dwFileErr        ( 0 )
{
    *m_strMessage = 0;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Sets up the load save example
//-----------------------------------------------------------------------------
HRESULT CXBLoadSave::Initialize()
{
    // We support a subset of available languages. If the language
    // isn't supported, switch to English in this sample
    m_iLang = CXBConfig::GetLanguage();
    switch( m_iLang )
    {
        // These languages are supported
        case XC_LANGUAGE_ENGLISH:
        case XC_LANGUAGE_GERMAN:
        case XC_LANGUAGE_FRENCH:
        case XC_LANGUAGE_SPANISH:
        case XC_LANGUAGE_ITALIAN : break;

        // These languages are not
        case XC_LANGUAGE_JAPANESE:
        default: m_iLang = XC_LANGUAGE_ENGLISH; break;
    }

    // Set the matrices
    D3DXVECTOR3 vEye(-2.5f, 2.0f, -4.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEye,&vAt, &vUp );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 100.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Create the fonts
    if( FAILED( m_FontBig.Create( m_pd3dDevice, "Font16.xpr" ) ) ||
        FAILED( m_FontMed.Create( m_pd3dDevice, "Font12.xpr" ) ) ||
        FAILED( m_FontSmall.Create( m_pd3dDevice, "Font9.xpr" ) ) ||
        FAILED( m_FontBtn.Create( m_pd3dDevice, "Xboxdings_24.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Reserve memory for the lists. These lists always hold the maximum
    // elements. Valid devices are considered IsMounted() for MUs and
    // IsValid() for storage devices.
    m_MemUnitList.resize( MAX_MEMORY_UNITS );
    m_DeviceList.resize( MAX_STORAGE_DEVICES );

    // Load our textures
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "LoadSaveResource.xpr", 
                                      LoadSaveResource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load our textures from the bundled resource
    m_ptXbox       = m_xprResource.GetTexture( (DWORD)LoadSaveResource_Xbox_OFFSET );
    m_ptXboxSel    = m_xprResource.GetTexture( (DWORD)LoadSaveResource_Xbox_Sel_OFFSET );
    m_ptMU         = m_xprResource.GetTexture( (DWORD)LoadSaveResource_MU_OFFSET );
    m_ptWideMU     = m_xprResource.GetTexture( (DWORD)LoadSaveResource_MUWide_OFFSET );
    m_ptMUSel      = m_xprResource.GetTexture( (DWORD)LoadSaveResource_MU_Sel_OFFSET );
    m_ptBackground = m_xprResource.GetTexture( (DWORD)LoadSaveResource_Background_OFFSET );
    m_ptMsgBox     = m_xprResource.GetTexture( (DWORD)LoadSaveResource_MsgBox_OFFSET );
    m_ptPlainBack  = m_xprResource.GetTexture( (DWORD)LoadSaveResource_PlainBack_OFFSET );
    m_ptUp         = m_xprResource.GetTexture( (DWORD)LoadSaveResource_Up_OFFSET );
    m_ptDown       = m_xprResource.GetTexture( (DWORD)LoadSaveResource_Down_OFFSET );
    m_ptMUBad      = m_xprResource.GetTexture( (DWORD)LoadSaveResource_MU_Bad_OFFSET );

    // The first device is always the hard drive
    m_DeviceList[0].SetDevice( CXBStorageDevice::GetUserRegion().GetDrive(), FALSE );
    m_DeviceList[0].pWideTexture = m_ptXbox;
    m_DeviceList[0].pTextureGood = m_ptXbox;
    m_DeviceList[0].pTextureBad  = NULL;
    m_DeviceList[0].pTexSel      = m_ptXboxSel;

    // NOTE: The following rectangles are based off of locations on the background
    // image, and will need to be updated as that art changes
    m_DeviceList[0].rcRegion = D3DXVECTOR4( 154, 101, 490, 227 );
    m_DeviceList[0].rcImage  = D3DXVECTOR4( 193, 104, 447, 211 );
    m_DeviceList[0].rcUsed   = D3DXVECTOR4( 193, 214, 447, 224 );

    for( DWORD i = 0; i < MAX_MEMORY_UNITS; i++ )
    {
        DWORD j = i + 1;

        // Offset between controller regions is 114.0f
        // Offset within a controller region is 45.0f
        m_DeviceList[ j ].pWideTexture = m_ptWideMU;
        m_DeviceList[ j ].pTextureGood = m_ptMU;
        m_DeviceList[ j ].pTextureBad  = m_ptMUBad;
        m_DeviceList[ j ].pTexSel  = m_ptMUSel;
        m_DeviceList[ j ].rcRegion = D3DXVECTOR4( 151.0f + ( i / 2 ) * 91.0f + ( i % 2 ) * 38.0f,
                                                  249.0f,
                                                  181.0f + ( i / 2 ) * 91.0f + ( i % 2 ) * 38.0f,
                                                  296.0f );
        m_DeviceList[ j ].rcImage  = D3DXVECTOR4( 153.0f + ( i / 2 ) * 91.0f + ( i % 2 ) * 38.0f,
                                                  252.0f,
                                                  178.0f + ( i / 2 ) * 91.0f + ( i % 2 ) * 38.0f,
                                                  287.0f );
        m_DeviceList[ j ].rcUsed   = D3DXVECTOR4( 153.0f + ( i / 2 ) * 91.0f + ( i % 2 ) * 38.0f,
                                                  289.0f,
                                                  178.0f + ( i / 2 ) * 91.0f + ( i % 2 ) * 38.0f,
                                                  294.0f );
    }

    // Create our background vertex buffer
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ), D3DUSAGE_WRITEONLY, FVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_pvbBackground );
    CUSTOMVERTEX* pVertices;
    m_pvbBackground->Lock( 0, 0, (BYTE **)&pVertices, 0L );
    pVertices[0].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 1.0f, 1.0f );  pVertices[0].t = D3DXVECTOR2( 0.0f, 1.0f ); // Lower left
    pVertices[1].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 1.0f, 1.0f );  pVertices[1].t = D3DXVECTOR2( 0.0f, 0.0f ); // Upper left
    pVertices[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 1.0f, 1.0f );  pVertices[2].t = D3DXVECTOR2( 1.0f, 1.0f ); // Lower right
    pVertices[3].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 1.0f, 1.0f );  pVertices[3].t = D3DXVECTOR2( 1.0f, 0.0f ); // Upper right
    m_pvbBackground->Unlock();

    // Initialize RNG for random game size and random game name.
    // Your game does not need to do this
    srand( GetTickCount() );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame; the entry point for animating the scene
//-----------------------------------------------------------------------------
HRESULT CXBLoadSave::FrameMove()
{
    ValidateState();

    // Process the current state
    switch( m_State )
    {
        case GAME_SAVE:
            if( !SaveGame() )
            {
                m_State = MENU_GAMELIST;

                // See if an MU was pulled...
                if( m_iCurrDev > 0 &&
                    m_dwFileErr == ERROR_DEVICE_NOT_CONNECTED )
                {
                    // TCR 6-1 Surprise MU Removal
                    WCHAR strErr[256];
                    WCHAR strName[ MAX_DEVNAME ];
                    m_MemUnitList[ m_iCurrDev-1 ].GetName( strName );

                    // Truncate long names to fit in message box
                    if( lstrlenW( strName ) > MAX_MU_DISPLAY )
                    {
                        strName[MAX_MU_DISPLAY] = 0;
                        lstrcatW( strName, strELLIPSES );
                    }

                    wsprintfW( strErr, strMU_REMOVED[ m_iLang ], strName,
                               strACTION_SAVE[ m_iLang ] );
                    StartMsgBox( strErr, MENU_GAMELIST );
                }
                else
                {
                    // Generic error
                    StartMsgBox( strSAVE_FAILED[ m_iLang ], MENU_GAMELIST );
                }
            }
            else
            {
                // Leave the save message up for a while to meet 
                // cert requirements.
                // Technically, this box doesn't have to be displayed at all,
                // since these save games can be written in less than 500 mS,
                // but the code is included to show a method of making
                // the message linger for titles that have large save games.
                if( m_MsgBoxTimer.IsRunning() )
                {
                    while( m_MsgBoxTimer.GetElapsedSeconds() < m_fMsgBoxSeconds )
                        ;
                }

                m_State = MENU_GAMELIST;
                StartMsgBox( strGAME_SAVED[ m_iLang ], MENU_GAMELIST, 
                             fSTATUS_SECONDS );

                // Put the cursor where the new game was saved. Your game
                // would probably return to the game proper at this point.
                m_iCurrGame = 1;
                m_iTopGame = 0;
            }
            m_bOverwriteMode = FALSE;
            break;
        case GAME_LOAD:
            if( !LoadGame() )
            {
                // See if an MU was pulled...
                if( m_iCurrDev > 0 &&
                    m_dwFileErr == ERROR_DEVICE_NOT_CONNECTED )
                {
                    WCHAR strErr[256];
                    WCHAR strName[ MAX_DEVNAME ];
                    m_MemUnitList[ m_iCurrDev-1 ].GetName( strName );

                    // Truncate long names to fit in message box
                    if( lstrlenW( strName ) > MAX_MU_DISPLAY )
                    {
                        strName[MAX_MU_DISPLAY] = 0;
                        lstrcatW( strName, strELLIPSES );
                    }

                    wsprintfW( strErr, strMU_REMOVED[ m_iLang ], strName,
                               strACTION_LOAD[ m_iLang ] );
                    m_State = MENU_GAMELIST;
                    StartMsgBox( strErr, MENU_GAMELIST );
                }
                else
                {
                    m_State = MENU_GAMELIST;
                    StartMsgBox( strLOAD_FAILED[ m_iLang ], MENU_GAMELIST );
                }
            }
            else
            {
                // Leave the load message up for a while to meet 
                // cert requirements.
                // Technically, this box doesn't have to be displayed at all,
                // since these save games can be written in less than 500 mS,
                // but the code is included to show a method of making
                // the message linger for titles that have large save games.
                if( m_MsgBoxTimer.IsRunning() )
                {
                    while( m_MsgBoxTimer.GetElapsedSeconds() < m_fMsgBoxSeconds )
                        ;
                }

                m_State = MENU_GAMELIST;
                StartMsgBox( strGAME_LOADED[ m_iLang ], MENU_GAMELIST, 
                             fSTATUS_SECONDS );
            }
            break;
    }

    // Poll the system for events
    Event ev = GetEvent();

    // Update the current state
    UpdateState( ev );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d rendering.
//       This function sets up render states, clears the viewport, and renders
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBLoadSave::Render()
{
    // Clear the viewport, zbuffer, and stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x000000ff, 1.0f, 0L );

    switch( m_State )
    {
        case START_SCREEN:     RenderStartScreen(); break;
        case MENU_GAMELIST:    RenderGameList();    break;
        case MENU_DEVICE:      RenderDevice();      break;
        case MENU_MAIN:        RenderMainMenu();    break;
        case BOX_OVERWRITE:    RenderOverwrite();   break;
        case BOX_DELETE:       RenderDelete();      break;
        case GAME_SAVE:        RenderGameList();    break;
        case GAME_LOAD:        RenderGameList();    break;
        default:               assert( FALSE );     break;
    }
    
    // If we have a message box to display on top of everything, do that now
    if( *m_strMessage != 0 )
    {
        DrawMsgBox( g_avUIRects[MB_MESSAGE], m_strMessage, m_FontBig, 
                    XBFONT_CENTER_X | XBFONT_CENTER_Y );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ValidateState()
// Desc: Check object invariants
//-----------------------------------------------------------------------------
VOID CXBLoadSave::ValidateState() const
{
    assert( m_Mode < MODE_MAX );
    assert( m_State < STATE_MAX );
    assert( m_NextState < STATE_MAX );
    assert( m_iCurrDev < m_DeviceList.size() );
    assert( IsDeviceInserted( m_iCurrDev ) );
    if( m_iCurrDev > 0 )
    {
        // If we're currently sitting on an MU
        assert( m_MemUnitList[ m_iCurrDev - 1 ].IsValid() ||
                m_DeviceList[ m_iCurrDev ].dwState != 0 );
    }
    assert( m_GameList.empty() || m_iCurrGame < m_GameList.size() );
    assert( m_iTopGame <= m_iCurrGame );
    assert( m_iCurrGame < m_iTopGame + MAX_GAMES_DISPLAYED );
    assert( m_Answer < ANSWER_MAX );
}




//-----------------------------------------------------------------------------
// Name: SaveGame()
// Desc: Writes the save game image, the saved game data, and handles 
//       deleting in the overwrite scenario.
//-----------------------------------------------------------------------------
BOOL CXBLoadSave::SaveGame()
{
    // Generate a save game name. Your game would generate its own name based 
    // on the current level, character, or other context information. We 
    // guarantee a unique name by appending a "version"
    WCHAR strGameName[ MAX_GAMENAME ];
    GenerateSavedGameName( strGameName );

    while( GameExists( strGameName ) )
        lstrcatW( strGameName, L" I" );

    // Get the current device drive letter
    StorageDev& CurrDev = m_DeviceList[ m_iCurrDev ];
    CHAR chDestDrive = CurrDev.GetDrive();

    // If we're overwriting and we need the room, delete the container we're
    // overwriting
    BOOL bCurrGameGone = FALSE;
    DWORD dwBytesNeeded;
    if( m_bOverwriteMode && !IsSpaceAvail( &dwBytesNeeded ) )
        bCurrGameGone = m_GameList[ m_iCurrGame ].DeleteGame();

    // Create the saved game container (directory)
    CXBSavedGame SavedGame;
    if( !SavedGame.CreateGame( chDestDrive, strGameName ) )
    {
        // It's possible that there was enough room on the device, but
        // we hit the directory limit. If we're in overwrite mode, then
        // we can delete the old container and try again.
        BOOL bCreate = FALSE;
        if( m_bOverwriteMode && GetLastError() == ERROR_DISK_FULL &&
            !bCurrGameGone )
        {
            bCurrGameGone = m_GameList[ m_iCurrGame ].DeleteGame();
            bCreate = SavedGame.CreateGame( chDestDrive, strGameName );
        }

        if( !bCreate )
        {
            m_dwFileErr = GetLastError();

            // If we were overwriting and the original game is gone, need
            // to update the list
            if( bCurrGameGone )
                DeleteGameFromList();

            // Update the device to account for deleted games
            CurrDev.Refresh();
            return FALSE;
        }
    }

    // Write the game image to disk
    LPDIRECT3DTEXTURE8 pGameImage = GetGameImage( strGameName );
    if( pGameImage != NULL )
    {
        SavedGame.SaveImage( pGameImage );
        pGameImage->Release();
    }

    // Build the saved game data file name
    CHAR strSaveFile[ MAX_PATH ];
    lstrcpynA( strSaveFile, SavedGame.GetDirectory(), MAX_PATH );
    lstrcatA( strSaveFile, strSAVE_FILE );

    // Save the data
    if( !SaveGame( strSaveFile ) )
    {
        m_dwFileErr = GetLastError();

        // If there's a failure writing the data, attempt to
        // remove the entire container
        SavedGame.DeleteGame();

        // If we were overwriting and the original game is gone, need
        // to update the list
        if( bCurrGameGone )
            DeleteGameFromList();

        // Update the device to account for deleted games
        CurrDev.Refresh();

        return FALSE;
    }

    if( m_bOverwriteMode )
    {
        // If we're overwriting and we haven't deleted the container
        // already, do it now. Otherwise the container has been deleted
        // and we just need to adjust the list.
        if( !bCurrGameGone )
            DeleteGame();
        else
            DeleteGameFromList();
    }

    // Update the device to account for the newly saved game
    CurrDev.Refresh();

    // Update the list
    m_GameList.pop_front(); // remove "empty slot"
    m_GameList.push_front( SavedGame ); // add new game
    m_GameList.push_front( CXBSavedGame() ); // replace "empty slot"

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: GetGameImage()
// Desc: Create a texture that represents the game. Your game would get a game-
//       appropriate image, like the current screen shot. This sample simply
//       generates a texture with the name of the save in random colors.
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE8 CXBLoadSave::GetGameImage( const WCHAR* strGameName ) const
{
    // Generate random colors for meta data image
    BYTE rb = BYTE( rand() % 255 );
    BYTE gb = BYTE( rand() % 255 );
    BYTE bb = BYTE( rand() % 255 );
    BYTE rt = BYTE( rb - 128 );
    BYTE gt = BYTE( gb - 128 );
    BYTE bt = BYTE( bb - 128 );
    D3DCOLOR dwBackgroundColor = D3DCOLOR_RGBA( rb, gb, bb, 0xFF );
    D3DCOLOR dwTextColor = D3DCOLOR_RGBA( rt, gt, bt, 0xFF );

    // Generate a colorful texture with the game name
    return m_FontBig.CreateTexture( strGameName, dwBackgroundColor, dwTextColor );
}




//-----------------------------------------------------------------------------
// Name: SaveGame()
// Desc: Write the saved game data to the current device in file strFile
//-----------------------------------------------------------------------------
BOOL CXBLoadSave::SaveGame( const CHAR* strFile )
{
    // Create the file
    File SavedGameFile;
    if( !SavedGameFile.Create( strFile ) )
        return FALSE;

    // Write the header (we write the official data later)
    XValidationHeader ValHeader;
    ZeroMemory( &ValHeader, sizeof(ValHeader) );
    if( !SavedGameFile.Write( &ValHeader, sizeof(ValHeader) ) )
        return FALSE;

    // Allocate chunk memory
    std::auto_ptr< DWORD > Chunk( new DWORD [ FILE_CHUNK_DWORDS ] );
    DWORD* pChunk = Chunk.get();

    // Generate some bogus save data. Your game would have less than bogus data.
    for( DWORD i = 0; i < FILE_CHUNK_DWORDS; ++i )
        pChunk[ i ] = DWORD( rand() );

    // Start the signature hash
    HANDLE hSignature = XCalculateSignatureBegin( 0 );
    if( hSignature == INVALID_HANDLE_VALUE )
        return FALSE;

    // Save the data a chunk at a time
    BOOL bShowProgress = FALSE;
    CXBStopWatch stopWatch( TRUE );
    DWORD dwSavedGameChunks = m_dwSavedGameBytes / FILE_CHUNK_BYTES;
    for( DWORD i = 0; i < dwSavedGameChunks; ++i )
    {
        // If an MU is removed during the save, this is the most
        // common location for failure
        if( !SavedGameFile.Write( pChunk, FILE_CHUNK_BYTES ) )
        {
            // Release signature resources
            XCalculateSignatureEnd( hSignature, NULL );
            return FALSE;
        }

        // Keep a running file signature (secure hash)
        DWORD dwSuccess = XCalculateSignatureUpdate( hSignature, (BYTE*)(pChunk), 
                                                     FILE_CHUNK_BYTES );
        assert( dwSuccess == ERROR_SUCCESS );
        (VOID)dwSuccess; // avoid compiler warning

        // Avoid showing the progress bar if we're writing to a fast device
        // (e.g. hard drive)
        if( stopWatch.GetElapsedSeconds() > 0.5f )
            bShowProgress = TRUE;

        // Only update twice per second to avoid Present() bottleneck
        if( bShowProgress && stopWatch.GetElapsedSeconds() > 0.5f )
        {
            stopWatch.Reset();

            // Render a progress bar by first rendering the normal screen
            CXBLoadSave::Render();

            // Then draw the bar on top
            FLOAT fProgress = FLOAT(i) / FLOAT( dwSavedGameChunks );
            RenderBar( g_avUIRects[MB_PROGRESS], fProgress, BARCOLOR_PROGRESS );

            // We're in a tight loop here, so we have to call present ourselves
            m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
        }
    }

    // Update the validation header
    ValHeader.dwFileLength = SavedGameFile.GetSize();
    DWORD dwSuccess = XCalculateSignatureEnd( hSignature, 
                                              &ValHeader.Signature );
    assert( dwSuccess == ERROR_SUCCESS );
    (VOID)dwSuccess; // avoid compiler warning
    SavedGameFile.SetPos( 0 );
    if( !SavedGameFile.Write( &ValHeader, sizeof(ValHeader) ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: LoadGame()
// Desc: Load the selected game data into memory
//-----------------------------------------------------------------------------
BOOL CXBLoadSave::LoadGame()
{
    m_dwFileErr = 0;

    // Get the name of the file
    CHAR strFile[ MAX_PATH ];
    GetSavedGameFileName( strFile );

    // Open the data file
    File SavedGameFile;
    if( !SavedGameFile.Open( strFile, GENERIC_READ ) )
    {
        m_dwFileErr = GetLastError();
        return FALSE;
    }

    // Read the file header
    XValidationHeader ValHeader;
    DWORD dwBytesRead;
    if( !SavedGameFile.Read( &ValHeader, sizeof(ValHeader), dwBytesRead ) )
    {
        m_dwFileErr = GetLastError();
        return FALSE;
    }
    if( dwBytesRead != sizeof(ValHeader) )
        return FALSE;

    // If the MU was removed while the file was being written, dwFileLength
    // will be zero and will not match the actual size
    DWORD dwTotalBytes = SavedGameFile.GetSize();
    if( ValHeader.dwFileLength != dwTotalBytes )
        return FALSE;

    // Allocate chunk memory
    std::auto_ptr< DWORD > Chunk( new DWORD [ FILE_CHUNK_DWORDS ] );
    DWORD* pChunk = Chunk.get();

    // Start the signature hash
    HANDLE hSignature = XCalculateSignatureBegin( 0 );
    if( hSignature == INVALID_HANDLE_VALUE )
        return FALSE;

    // Read the file a chunk at a time until we've read the whole thing.
    // This sample doesn't do anything with the data. Your game would load 
    // the data into game state memory.
    BOOL bShowProgress = FALSE;
    DWORD dwTotalBytesRead = 0;
    CXBStopWatch stopWatch( TRUE );
    for( ;; )
    {
        if( !SavedGameFile.Read( pChunk, FILE_CHUNK_BYTES, dwBytesRead ) )
        {
            m_dwFileErr = GetLastError();

            // Release signature resources
            XCalculateSignatureEnd( hSignature, NULL );
            return FALSE;
        }

        // See if we reached the end
        if( dwBytesRead == 0 )
            break;

        // Keep a running file signature (secure hash)
        DWORD dwSuccess = XCalculateSignatureUpdate( hSignature, (BYTE*)(pChunk),
                                                     dwBytesRead );
        assert( dwSuccess == ERROR_SUCCESS );
        (VOID)dwSuccess; // avoid compiler warning

        // Keep a running total for the progress bar
        dwTotalBytesRead += dwBytesRead;

        // Avoid showing the progress bar if we're reading from a fast device
        // (e.g. hard drive)
        if( stopWatch.GetElapsedSeconds() > 0.5f )
            bShowProgress = TRUE;

        // Only update twice per second to avoid Present() bottleneck
        if( bShowProgress && stopWatch.GetElapsedSeconds() > 0.5f )
        {
            stopWatch.Reset();

            // Render a progress bar by first rendering the normal screen
            CXBLoadSave::Render();

            // Then draw the bar on top
            FLOAT fProgress = FLOAT( dwTotalBytesRead ) / FLOAT( dwTotalBytes );
            RenderBar( g_avUIRects[MB_PROGRESS], fProgress, BARCOLOR_PROGRESS );

            // We're in a tight loop here, so we have to call present ourselves
            m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
        }
    }

    // Validate the signature
    XCALCSIG_SIGNATURE Signature;
    DWORD dwSuccess = XCalculateSignatureEnd( hSignature, &Signature );
    assert( dwSuccess == ERROR_SUCCESS );
    (VOID)dwSuccess; // avoid compiler warning
    if( memcmp( &Signature, &ValHeader.Signature, sizeof(XCALCSIG_SIGNATURE) ) != 0 )
        return FALSE;

    // File is OK
    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: GameExists()
// Desc: TRUE if the saved game already exists
//-----------------------------------------------------------------------------
BOOL CXBLoadSave::GameExists( const WCHAR* strGameName ) const
{
    for( SavedGameIndex i = 0; i < m_GameList.size(); ++i )
    {
        const CXBSavedGame& SavedGame = m_GameList[ i ];
        if( lstrcmpW( SavedGame.GetName(), strGameName ) == 0 )
            return TRUE;
    }
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: GetEvent()
// Desc: Polls the controller and MU for events. Returns EV_NULL if no event
//-----------------------------------------------------------------------------
CXBLoadSave::Event CXBLoadSave::GetEvent()
{
    // Query the primary controller
    Event evControllerClick = GetControllerEvent();

    // If the controller isn't doing anything, check MU status
    if( evControllerClick == EV_NULL )
        return GetMemoryUnitEvent();

    return evControllerClick;
}




//-----------------------------------------------------------------------------
// Name: IsAnyButtonActive()
// Desc: TRUE if any button depressed or any thumbstick offset on the given
//       controller.
//-----------------------------------------------------------------------------
BOOL IsAnyButtonActive( const XBGAMEPAD* pGamePad )
{
    // Check digital buttons
    if( pGamePad->wButtons )
        return TRUE;

    // Check analog buttons
    for( DWORD i = 0; i < 8; ++i )
    {
        if( pGamePad->bAnalogButtons[ i ] > XINPUT_GAMEPAD_MAX_CROSSTALK )
            return TRUE;
    }

    // Check thumbsticks
    // Check thumbsticks
    if( pGamePad->fX1 >  fTHUMB_DEADZONE ||
        pGamePad->fX1 < -fTHUMB_DEADZONE ||
        pGamePad->fY1 >  fTHUMB_DEADZONE ||
        pGamePad->fY1 < -fTHUMB_DEADZONE )
    {
        return TRUE;
    }

    if( pGamePad->fX2 >  fTHUMB_DEADZONE ||
        pGamePad->fX2 < -fTHUMB_DEADZONE ||
        pGamePad->fY2 >  fTHUMB_DEADZONE ||
        pGamePad->fY2 < -fTHUMB_DEADZONE )
    {
        return TRUE;
    }

    // Nothing active
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: GetPrimaryController()
// Desc: The primary controller is the first controller used by a player.
//       If no controller has been used or the controller has been removed,
//       the primary controller is the controller inserted at the lowest 
//       port number. Function returns NULL if no controller is inserted.
//-----------------------------------------------------------------------------
const XBGAMEPAD* GetPrimaryController()
{
    static INT nPrimaryController = -1;

    // If primary controller has been set and hasn't been removed, use it
    const XBGAMEPAD* pGamePad = NULL;
    if( nPrimaryController != -1 )
    {
        pGamePad = &g_Gamepads[ nPrimaryController ];
        if( pGamePad->hDevice != NULL )
            return pGamePad;
    }

    // Primary controller hasn't been set or has been removed...

    // Examine each inserted controller to see if any is being used
    INT nFirst = -1;
    for( DWORD i=0; i < XGetPortCount(); ++i )
    {
        pGamePad = &g_Gamepads[i];
        if( pGamePad->hDevice != NULL )
        {
            // Remember the lowest inserted controller ID
            if( nFirst == -1 )
                nFirst = i;

            // If any button is active, we found the primary controller
            if( IsAnyButtonActive( pGamePad ) )
            {
                nPrimaryController = i;
                return pGamePad;
            }
        }
    }

    // No controllers are inserted
    if( nFirst == -1 )
        return NULL;

    // The primary controller hasn't been set and no controller has been
    // used yet, so return the controller on the lowest port number
    pGamePad = &g_Gamepads[ nFirst ];
    return pGamePad;
}




//-----------------------------------------------------------------------------
// Name: GetControllerEvent()
// Desc: Polls the controller for events. Handles button repeats.
//-----------------------------------------------------------------------------
CXBLoadSave::Event CXBLoadSave::GetControllerEvent()
{
    const XBGAMEPAD* pGamePad = GetPrimaryController();
    if( pGamePad != NULL )
    {
        // Handle button press and joystick hold repeats
        if( IsAnyButtonActive( pGamePad ) )
        {
            // If the timer is running, the button is being held. If it's
            // held long enough, it triggers a repeat. If the timer isn't
            // running, we start it.
            if( m_RepeatTimer.IsRunning() )
            {
                // If the timer is running but hasn't expired, bail out
                if( m_RepeatTimer.GetElapsedSeconds() < m_fRepeatDelay )
                    return EV_NULL;

                m_fRepeatDelay = fSTD_REPEAT;
                m_RepeatTimer.StartZero();
            }
            else
            {
                m_fRepeatDelay = fINITIAL_REPEAT;
                m_RepeatTimer.StartZero();
            }
        }
        else
        {
            // No buttons or joysticks active; kill the repeat timer
            m_fRepeatDelay = fINITIAL_REPEAT;
            m_RepeatTimer.Stop();
        }

        // Movement
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_UP ||
            pGamePad->fY1 > fTHUMB_DEADZONE )
            return EV_UP;
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
            pGamePad->fY1 < -fTHUMB_DEADZONE )
            return EV_DOWN;
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
            pGamePad->fX1 < -fTHUMB_DEADZONE )
            return EV_LEFT;
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
            pGamePad->fX1 > fTHUMB_DEADZONE )
            return EV_RIGHT;

        // Only "cursor control" buttons allow repeats, so if
        // we get this far, we can kill the repeat timer
        m_fRepeatDelay = fINITIAL_REPEAT;
        m_RepeatTimer.Stop();

        // Primary buttons
        if( pGamePad->wPressedButtons & XINPUT_GAMEPAD_START )
            return EV_START_BUTTON;
        if( pGamePad->wPressedButtons & XINPUT_GAMEPAD_BACK )
            return EV_BACK_BUTTON;
        if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
            return EV_A_BUTTON;
        if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
            return EV_B_BUTTON;
        if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
            return EV_X_BUTTON;
        if( pGamePad->bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
            return EV_Y_BUTTON;
    }

    // No controllers inserted or no button presses
    return EV_NULL;
}




//-----------------------------------------------------------------------------
// Name: GetMemoryUnitEvent()
// Desc: Polls the controllers for MU insertions and removals.
//-----------------------------------------------------------------------------
CXBLoadSave::Event CXBLoadSave::GetMemoryUnitEvent()
{
    // If no MU updates, no event registered
    DWORD dwInsertions;
    DWORD dwRemovals;
    if( !CXBMemUnit::GetMemUnitChanges( dwInsertions, dwRemovals ) )
        return EV_NULL;

    // Handle removals first
    for( DWORD i = 0; i < MAX_MEMORY_UNITS; ++i )
    {
        if( dwRemovals & CXBMemUnit::GetMemUnitMask( i ) )
        {
            // Invalidate the device and unmount the MU. Must be in this
            // order so that any XFindFirstSavedGame() search handle is closed
            // before the MU is unmounted.
            m_DeviceList[ i+1 ].SetDevice( 0, TRUE );
            m_MemUnitList[ i ].Remove();
        }
    }

    // Insertions
    for( DWORD i = 0; i < MAX_MEMORY_UNITS; ++i )
    {
        if( dwInsertions & CXBMemUnit::GetMemUnitMask( i ) )
        {
            // Mark the MU as inserted/valid
            DWORD dwPort = CXBMemUnit::GetMemUnitPort( i );
            DWORD dwSlot = CXBMemUnit::GetMemUnitSlot( i );
            m_MemUnitList[ i ].Insert( dwPort, dwSlot );

            // Attempt to mount the MU. If mount fails, get error condition
            // and set state appropriately
            DWORD dwError;
            if( m_MemUnitList[ i ].Mount( dwError ) )
                m_DeviceList[ i+1 ].SetDevice( m_MemUnitList[ i ].GetDrive(), TRUE );
            else
                m_DeviceList[ i+1 ].SetDeviceState( GetMuState( dwError ) );
        }
    }

    return EV_MU_CHANGE;
}




//-----------------------------------------------------------------------------
// Name: GetMuState()
// Desc: Translates the GetLastError() code into a state bitmask
//-----------------------------------------------------------------------------
DWORD CXBLoadSave::GetMuState( DWORD dwError ) // static
{
    // The MU failed to mount; check for error conditions.
    switch( dwError )
    {
        // MU is full (of other games)
        case ERROR_DISK_FULL:
            return StorageDev::MU_FULL;

        // MU is already mounted (should never happen in our code)
        case ERROR_ALREADY_ASSIGNED:
            assert( FALSE );
            return 0;

        // MU was removed during the XMountMU call. We ignore this
        // situation, because it will be detected the next time we
        // call XGetDeviceChanges
        case ERROR_DEVICE_NOT_CONNECTED:
            return 0;

        // Allocation failure (mark MU as "unusable").
        // The game is in big trouble at this point, because memory
        // is way too low. Note that ERROR_OUTOFMEMORY can also 
        // indicate mounting more devices than requested in XInitDevices().
        case ERROR_OUTOFMEMORY:
        case ERROR_NO_SYSTEM_RESOURCES:
            return StorageDev::MU_UNUSABLE;

        // Any other error indicates the the MU is probably unusable
        // and should be reformatted
        default:
            return StorageDev::MU_UNUSABLE;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateState()
// Desc: State machine updates the current context based on the incoming event
//-----------------------------------------------------------------------------
VOID CXBLoadSave::UpdateState( Event ev )
{
    if( *m_strMessage != 0 )
    {
        // Check the message box timer
        if( m_MsgBoxTimer.IsRunning() && 
            m_MsgBoxTimer.GetElapsedSeconds() > m_fMsgBoxSeconds )
        {
            // Timer expired; message box goes away
            m_MsgBoxTimer.Stop();
            m_State = m_NextState;
            *m_strMessage = 0;
        }

        // Check for events
        switch( ev )
        {
        case EV_A_BUTTON:
        case EV_START_BUTTON:
        case EV_B_BUTTON:
        case EV_BACK_BUTTON:
            m_State = m_NextState;
            *m_strMessage = 0;
            break;
        case EV_MU_CHANGE:
            // If the current device was an MU and it was removed, we
            // set the current device to the HD and back out to the device
            // list for safety.
            if( !IsDeviceInserted( m_iCurrDev ) )
            {
                m_iCurrDev = 0;
                m_State = MENU_DEVICE;
                m_NextState = MENU_DEVICE;
            }
            break;
        }
    }
    else switch( m_State )
    {
        case START_SCREEN:     UpdateStateStartScreen( ev ); break;
        case MENU_MAIN:        UpdateStateMainMenu( ev );    break;
        case MENU_DEVICE:      UpdateStateDevice( ev );      break;
        case MENU_GAMELIST:    UpdateStateGameList( ev );    break;
        case BOX_OVERWRITE:    UpdateStateOverwrite( ev );   break;
        case BOX_DELETE:       UpdateStateDelete( ev );      break;
        default:               assert( FALSE );              break;
    }

}




//-----------------------------------------------------------------------------
// Name: UpdateStateStartScreen()
// Desc: Update start screen state
//-----------------------------------------------------------------------------
VOID CXBLoadSave::UpdateStateStartScreen( Event ev )
{
    switch( ev )
    {
        case EV_A_BUTTON:
        case EV_START_BUTTON:
            m_State = MENU_MAIN;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateMainMenu()
// Desc: Update main menu state
//-----------------------------------------------------------------------------
VOID CXBLoadSave::UpdateStateMainMenu( Event ev )
{
    switch( ev )
    {
        case EV_A_BUTTON:
        case EV_START_BUTTON:
            // If any MUs inserted, allow device selection, otherwise
            // go directly to the hard drive game list
            if( AnyMemoryUnitsInserted() )
                m_State = MENU_DEVICE;
            else
            {
                // If loading from hard drive and no games, error message
                if( m_Mode == MODE_LOAD && 
                    m_DeviceList[ 0 ].GetSavedGameCount() == 0 )
                {
                    StartMsgBox( strNO_SAVES[ m_iLang ], MENU_MAIN );
                }
                else
                {
                    BuildGameList();
                    m_State = MENU_GAMELIST;
                }
            }

            // Generate a random size for the save game data. It's always evenly
            // divisible by the FILE_CHUNK_BYTES to avoid read/write special cases.
            if( m_Mode == MODE_SAVE )
            {
                DWORD dwChunkRange = MAX_SAVE_DATA_CHUNKS - MIN_SAVE_DATA_CHUNKS;
                DWORD dwChunks = DWORD( rand() ) % dwChunkRange;
                m_dwSavedGameBytes = ( MIN_SAVE_DATA_CHUNKS + dwChunks ) 
                                     * FILE_CHUNK_BYTES;
            }
            break;
        case EV_UP:
        case EV_DOWN:
            // Toggle between save/load
            m_Mode = ( m_Mode == MODE_SAVE ) ? MODE_LOAD : MODE_SAVE;
            break;
        case EV_MU_CHANGE:
            // If the current device was an MU and it was removed, we
            // set the current device to the HD
            if( !IsDeviceInserted( m_iCurrDev ) )
                m_iCurrDev = 0;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateDevice()
// Desc: Update device menu state
//-----------------------------------------------------------------------------
VOID CXBLoadSave::UpdateStateDevice( Event ev )
{
    switch( ev )
    {
        case EV_A_BUTTON:
        case EV_START_BUTTON:
        {
            // If we selected a memory unit, attempt to mount it
            StorageDev& CurrDev = m_DeviceList[ m_iCurrDev ];
            if( m_iCurrDev > 0 )
            {
                CXBMemUnit& CurrMu = m_MemUnitList[ m_iCurrDev-1 ];
                DWORD dwError;

                // This call won't do anything but return TRUE if the MU is
                // already mounted.
                if( CurrMu.Mount( dwError ) )
                    CurrDev.SetDevice( CurrMu.GetDrive(), TRUE );
                else
                    CurrDev.SetDeviceState( GetMuState( dwError ) );
            }


            // TCR 2-22 Unusable MUs
            if( CurrDev.dwState & StorageDev::MU_UNUSABLE )
            {
                StartMsgBox( strMU_UNUSABLE[ m_iLang ], MENU_DEVICE );
                break;
            }
            if( CurrDev.dwState & StorageDev::MU_FULL )
            {
                StartMsgBox( strMU_FULL[ m_iLang ], MENU_DEVICE );
                break;
            }

            BuildGameList();
            if( m_Mode == MODE_SAVE )
            {
                // Check for space. If there's any possibility that games
                // could be stored on the device (even if other games have
                // to be deleted), we go to the game list
                if( !IsSpaceAvail() )
                {
                    StartMsgBox( m_iCurrDev == 0 ? strNO_ROOM_HD[ m_iLang ] : 
                                                   strNO_ROOM_MU[ m_iLang ], 
                                 MENU_DEVICE );
                    break;
                }
            }
            else // MODE_LOAD
            {
                // Check for our games
                if( CurrDev.GetSavedGameCount() == 0 )
                {
                    StartMsgBox( strNO_SAVES[ m_iLang ], MENU_DEVICE );
                    break;
                }
            }

            // Good to go
            m_State = MENU_GAMELIST;
            break;
        }
        case EV_B_BUTTON:
        case EV_BACK_BUTTON:
            m_State = MENU_MAIN;
            break;
        case EV_MU_CHANGE:
            // Do nothing if the current device wasn't removed,
            // otherwise move to the hard drive
            if( IsDeviceInserted( m_iCurrDev ) )
                break;
            // fall thru
        case EV_UP:
            // If on HD, do nothing, else move to HD
            if( m_iCurrDev > 0 )
                m_iCurrDev = 0;
            break;
        case EV_DOWN:
            // If on MU, do nothing, else move to the last MU we were on.
            // If last MU invalid, move to first valid MU
            if( m_iCurrDev == 0 )
            {
                if( IsDeviceInserted( m_iLastMu ) )
                    m_iCurrDev = m_iLastMu;
                else
                {
                    for( StorageDevIndex i = 1; i < MAX_STORAGE_DEVICES; ++i )
                    {
                        if( IsDeviceInserted( i ) )
                        {
                            m_iLastMu = m_iCurrDev = i;
                            break;
                        }
                    }
                }
            }
            break;
        case EV_LEFT:
            // If on HD, do nothing, else move to previous valid MU
            if( m_iCurrDev > 0 )
            {
                for( StorageDevIndex i = m_iCurrDev-1; i > 0; --i )
                {
                    if( IsDeviceInserted( i ) )
                    {
                        m_iLastMu = m_iCurrDev = i;
                        break;
                    }
                }
            }
            break;
        case EV_RIGHT:
            // If on HD, do nothing, else move to next valid MU
            if( m_iCurrDev > 0 )
            {
                for( StorageDevIndex i = m_iCurrDev+1; i < MAX_STORAGE_DEVICES; ++i )
                {
                    if( IsDeviceInserted( i ) )
                    {
                        m_iCurrDev = i;
                        break;
                    }
                }
                // Remember the last MU we were on
                m_iLastMu = m_iCurrDev;
            }
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateGameList()
// Desc: Update game list state
//-----------------------------------------------------------------------------
VOID CXBLoadSave::UpdateStateGameList( Event ev )
{
    switch( ev )
    {
        case EV_A_BUTTON:
        case EV_START_BUTTON:
            if( m_Mode == MODE_LOAD )
            {
                WCHAR strLoad[256];
                m_State = GAME_LOAD;

                lstrcpyW( strLoad, strLOADING[ m_iLang ] );
                if( m_iCurrDev == 0 )
                    lstrcatW( strLoad, strDO_NOT_POWEROFF[ m_iLang ] );
                else
                {
                    lstrcatW( strLoad, L"\n" );
                    lstrcatW( strLoad, strDO_NOT_REMOVE_MU[ m_iLang ] );
                }

                StartMsgBox( strLoad, GAME_LOAD, fDEVICE_SECONDS );
            }
            else // MODE_SAVE
            {
                // If saving in the empty slot
                if( m_iCurrGame == 0 )
                {
                    // Is there room in the slot?
                    DWORD dwBytesNeeded;
                    if( !IsSpaceAvail( &dwBytesNeeded ) )
                    {
                        StartMsgBoxFree();
                    }
                    
                    // Have we reached the limit of saves?
                    // Note that m_GameList includes the empty slot, so
                    // we use > instead of >=.
                    else if( m_GameList.size() > MAX_SAVED_GAMES )
                    {
                        StartMsgBox( strMAX_SAVED_GAMES[ m_iLang ], 
                                     MENU_GAMELIST );
                    }

                    else
                    {
                        // Begin the save
                        m_State = GAME_SAVE;
                        StartMsgBoxSave();
                    }
                }
                else // overwrite
                {
                    // Is there room if existing game is deleted?
                    DWORD dwBytesNeeded;
                    if( !IsSpaceAvail( &dwBytesNeeded ) )
                    {
                        if( m_GameList[ m_iCurrGame ].GetSize() < dwBytesNeeded )
                        {
                            StartMsgBoxFree();
                            break;
                        }
                    }

                    // There's enough room
                    m_State = BOX_OVERWRITE;
                    m_Answer = ANSWER_NO;
                    m_NextState = GAME_SAVE;
                }
            }
            break;
        case EV_B_BUTTON:
        case EV_BACK_BUTTON:
            // If any MUs inserted, return to the device list,
            // otherwise go back to the main menu
            m_State = ( AnyMemoryUnitsInserted() ) ? MENU_DEVICE : MENU_MAIN;
            break;
        case EV_UP:
            // If we're at the top of the displayed list, shift the display
            if( m_iCurrGame == m_iTopGame )
            {
                if( m_iTopGame > 0 )
                    --m_iTopGame;
            }
            // Move to previous game
            if( m_iCurrGame > 0 )
                --m_iCurrGame;
            break;
        case EV_DOWN:
            // If we're at the bottom of the displayed list, shift the display
            if( m_iCurrGame == m_iTopGame + MAX_GAMES_DISPLAYED - 1 )
            {
                if( m_iTopGame + MAX_GAMES_DISPLAYED < m_GameList.size() )
                    ++m_iTopGame;
            }
            // Move to next game
            if( m_iCurrGame < m_GameList.size() - 1 )
                ++m_iCurrGame;
            break;
        case EV_MU_CHANGE:
            // If the current device was removed, back out
            if( !IsDeviceInserted( m_iCurrDev ) )
            {
                m_iCurrDev = 0;
                m_State = MENU_DEVICE;
            }
            break;
        case EV_Y_BUTTON:
            // If we're not on the empty slot, allow delete to proceed
            if( !m_GameList[ m_iCurrGame ].IsEmpty() )
            {
                m_State = BOX_DELETE;
                m_Answer = ANSWER_NO;
            }
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateOverwrite()
// Desc: Update overwrite box state
//-----------------------------------------------------------------------------
VOID CXBLoadSave::UpdateStateOverwrite( Event ev )
{
    switch( ev )
    {
        case EV_A_BUTTON:
        case EV_START_BUTTON:
            if( m_Answer == ANSWER_YES )
            {
                // Begin the save
                m_bOverwriteMode = TRUE;
                m_State = GAME_SAVE;
                StartMsgBoxSave();
            }
            else // ANSWER_NO
            {
                m_State = MENU_GAMELIST;
            }
            break;
            
        case EV_B_BUTTON:
        case EV_BACK_BUTTON:
            m_State = MENU_GAMELIST;
            break;

        case EV_UP:
        case EV_DOWN:
        case EV_LEFT:
        case EV_RIGHT:
            m_Answer = ( m_Answer == ANSWER_YES ) ? ANSWER_NO : ANSWER_YES;
            break;

        case EV_MU_CHANGE:
            // If the current device was removed, back out
            if( !IsDeviceInserted( m_iCurrDev ) )
            {
                m_iCurrDev = 0;
                m_State = MENU_DEVICE;
            }
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateDelete()
// Desc: Update delete box state
//-----------------------------------------------------------------------------
VOID CXBLoadSave::UpdateStateDelete( Event ev )
{
    switch( ev )
    {
        case EV_A_BUTTON:
        case EV_START_BUTTON:
            if( m_Answer == ANSWER_YES )
            {
                DeleteGame();

                // Stay in the list unless we deleted the last game
                m_State = m_GameList.empty() ? MENU_MAIN : MENU_GAMELIST;

                // Refresh the device
                m_DeviceList[ m_iCurrDev ].Refresh();

            }
            else // ANSWER_NO
            {
                m_State = MENU_GAMELIST;
            }
            break;
            
        case EV_B_BUTTON:
        case EV_BACK_BUTTON:
            m_State = MENU_GAMELIST;
            break;

        case EV_UP:
        case EV_DOWN:
        case EV_LEFT:
        case EV_RIGHT:
            m_Answer = ( m_Answer == ANSWER_YES ) ? ANSWER_NO : ANSWER_YES;
            break;

        case EV_MU_CHANGE:
            // If the current device was removed, back out
            if( !IsDeviceInserted( m_iCurrDev ) )
            {
                m_iCurrDev = 0;
                m_State = MENU_DEVICE;
            }
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: StartMsgBox()
// Desc: Change the state so that a message box is displayed with the given
//       message. When the message box is dismissed, the next state will
//       be stNext.
//-----------------------------------------------------------------------------
VOID CXBLoadSave::StartMsgBox( const WCHAR* strMessage, State stNext,
                               FLOAT fDisplaySeconds )
{
    lstrcpynW( m_strMessage, strMessage, MAX_MESSAGE );
    m_NextState = stNext;
    m_fMsgBoxSeconds = fDisplaySeconds;
    m_MsgBoxTimer.Stop();
    if( fDisplaySeconds > 0.0f )
        m_MsgBoxTimer.StartZero();
}




//-----------------------------------------------------------------------------
// Name: StartMsgBoxSave()
// Desc: Display the "saving game" message
//-----------------------------------------------------------------------------
VOID CXBLoadSave::StartMsgBoxSave()
{
    WCHAR strSave[256];
    FLOAT fDeviceSeconds = 0.0f;

    // Hard disk
    if( m_iCurrDev == 0 )
    {
        // TCR 3-9 Hard Disk Warning
        lstrcpynW( strSave, strSAVING[ m_iLang ], 256 );

        // TCR 6-4 Hard Disk Write Warning would apply if save lasted
        // longer than 3 seconds.
        // lstrcatW( strSave, strDO_NOT_POWEROFF[ m_iLang ] );

        fDeviceSeconds = fHD_DEVICE_SECONDS;
    }
    else // MU
    {
        WCHAR strName[ MAX_DEVNAME ];
        m_MemUnitList[ m_iCurrDev-1 ].GetName( strName );

        // Truncate long names to fit in message box
        if( lstrlenW( strName ) > MAX_MU_DISPLAY )
        {
            strName[MAX_MU_DISPLAY] = 0;
            lstrcatW( strName, strELLIPSES );
        }

        // TCR 3-8 Memory Unit Warning
        // TCR 6-2 MU Write Warning
        wsprintfW( strSave, strSAVING_MU[ m_iLang ], strName );
        lstrcatW( strSave, strDO_NOT_REMOVE_MU[ m_iLang ] );
        fDeviceSeconds = fDEVICE_SECONDS;
    }

    StartMsgBox( strSave, GAME_SAVE, fDeviceSeconds );
}




//-----------------------------------------------------------------------------
// Name: StartMsgBoxFree()
// Desc: Display the number of blocks that must be free before the current
//       game can be saved.
//-----------------------------------------------------------------------------
VOID CXBLoadSave::StartMsgBoxFree()
{
    // Convert to blocks
    DWORD dwBlocksToFree = GetSavedGameSize() / CXBStorageDevice::GetBlockSize();
    WCHAR strFree[256];
    wsprintfW( strFree, m_iCurrDev == 0 ? strNO_ROOM_HD_PLZ_FREE[ m_iLang ] : 
               strNO_ROOM_MU_PLZ_FREE[ m_iLang ], dwBlocksToFree );
    StartMsgBox( strFree, MENU_GAMELIST );
}




//-----------------------------------------------------------------------------
// Name: RenderStartScreen()
// Desc: Display start screen
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderStartScreen() const
{
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetStreamSource( 0, m_pvbBackground, sizeof( CUSTOMVERTEX ) );
    m_pd3dDevice->SetTexture( 0, m_ptPlainBack );
    m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    m_pd3dDevice->SetTexture( 0, NULL );

    m_FontMed.DrawText( 320.0f, 80.0f, 0xF0000000,
                        L"This sample is intended to show appropriate\n"
                        L"functionality only. Please do not lift the\n"
                        L"graphics for use in your game.\n\n"
                        L"Source code for this sample is located at\n"
                        L"Samples\\Xbox\\ReferenceUI\\LoadSave.\n\n"
                        L"A description of the user research that\n"
                        L"went into the creation of this sample is\n"
                        L"located in the XDK documentation at\n"
                        L"Developing for Xbox - Reference User Interface.\n\n"
                        L"For functionality ideas see the\n"
                        L"Technical Certification game.",
                        XBFONT_CENTER_X );

    if( m_iLang != XC_LANGUAGE_ENGLISH )
    {
        m_FontMed.DrawText( 320.0f, 350.0f, 0xF0000000,
                            L"Localized terminology not final.\n"
                            L"See Xbox Guide for final terms.",
                            XBFONT_CENTER_X );
    }

    WCHAR strButton[2] = { BTN_A, 0 };
    m_FontBtn.DrawText( 280.0f, BUTTON_Y, BUTTON_COLOR, strButton );
    m_FontBig.DrawText( 280.0f + BUTTON_OFFSET, BUTTON_Y, BUTTON_TEXT, L"continue" );
}




//-----------------------------------------------------------------------------
// Name: RenderMainMenu()
// Desc: Display main menu
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderMainMenu() const
{
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetStreamSource( 0, m_pvbBackground, sizeof( CUSTOMVERTEX ) );
    m_pd3dDevice->SetTexture( 0, m_ptPlainBack );
    m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    m_pd3dDevice->SetTexture( 0, NULL );

    RenderSelection( g_avUIRects[ m_Mode ], MAIN_MENU_OFFSET );
    DrawMsgBox( g_avUIRects[MM_SAVE], strSAVE_GAME[ m_iLang ], m_FontBig,
                XBFONT_CENTER_X | XBFONT_CENTER_Y );
    DrawMsgBox( g_avUIRects[MM_LOAD], strLOAD_GAME[ m_iLang ], m_FontBig,
                XBFONT_CENTER_X | XBFONT_CENTER_Y );

    m_FontSmall.DrawText( 320.0f, 320.0f, 0xF0000000, strILLUS_GRAPHICS[ m_iLang ],
                          XBFONT_CENTER_X );

    DrawButton( 280.0f, BTN_A );
}




//-----------------------------------------------------------------------------
// Name: RenderDevice()
// Desc: Display device menu
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderDevice() const
{
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetStreamSource( 0, m_pvbBackground, sizeof( CUSTOMVERTEX ) );
    m_pd3dDevice->SetTexture( 0, m_ptBackground );
    m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    m_pd3dDevice->SetTexture( 0, NULL );

    // Show statistics for selected device
    const StorageDev& CurrDev = m_DeviceList[m_iCurrDev];
    WCHAR strStats[256];
    wsprintfW( strStats, strFORMAT_DEVICE[ m_iLang ], CurrDev.strTotalBlocks, 
               CurrDev.strFreeBlocks );
    DrawMsgBox( g_avUIRects[DS_BLOCKBOX], strStats, m_FontSmall, XBFONT_RIGHT );

    // Show device full name
    WCHAR strName[ MAX_DEVNAME ];
    if( m_iCurrDev == 0 )
        lstrcpynW( strName, strXHD[ m_iLang ], MAX_DEVNAME );
    else
        m_MemUnitList[ m_iCurrDev-1 ].GetName( strName );

    // Handle MU error conditions
    if( CurrDev.dwState & StorageDev::MU_UNUSABLE )
        lstrcpynW( strName, strUNUSABLE_MU_NAME[ m_iLang ], MAX_DEVNAME );
    else if( CurrDev.dwState & StorageDev::MU_FULL )
        lstrcpynW( strName, strFULL_MU_NAME[ m_iLang ], MAX_DEVNAME );

    DrawMsgBox( g_avUIRects[DS_MAINDESC], strName, m_FontBig, XBFONT_RIGHT );

    // Show header
    WCHAR strHeader[256];
    wsprintfW( strHeader, m_Mode == MODE_LOAD ? strCHOOSE_LOAD[ m_iLang ] : 
                                                strCHOOSE_SAVE[ m_iLang ] );
    DrawMsgBox( g_avUIRects[DS_TITLE], strHeader, m_FontBig,
                XBFONT_CENTER_X | XBFONT_CENTER_Y );

    // Render each inserted device image
    for( DWORD i = 0; i < MAX_STORAGE_DEVICES; ++i )
    {
        if( IsDeviceInserted( i ) )
            m_DeviceList[ i ].Render( m_iCurrDev == i, this );
    }

    DrawButton( 80.0f, BTN_A );
    DrawButton( 460.0f, BTN_B );
}




//-----------------------------------------------------------------------------
// Name: RenderGameList()
// Desc: Display game list for the current device
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderGameList() const
{
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetStreamSource( 0, m_pvbBackground, sizeof( CUSTOMVERTEX ) );
    m_pd3dDevice->SetTexture( 0, m_ptPlainBack );
    m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    m_pd3dDevice->SetTexture( 0, NULL );

    // Device image
    const StorageDev& CurrDev = m_DeviceList[ m_iCurrDev ];
    RenderTile( g_avUIRects[GS_DEVICEIMG], CurrDev.pWideTexture );

    // Show current device statistics
    WCHAR strStats[256];
    wsprintfW( strStats, strFORMAT_DEVICE[ m_iLang ], CurrDev.strTotalBlocks, 
               CurrDev.strFreeBlocks );
    DrawMsgBox( g_avUIRects[GS_BLOCKBOX], strStats, m_FontSmall, XBFONT_RIGHT );

    // Show device full name
    WCHAR strName[ MAX_DEVNAME ];
    if( m_iCurrDev == 0 )
        lstrcpynW( strName, strXHD[ m_iLang ], MAX_DEVNAME );
    else
        m_MemUnitList[ m_iCurrDev-1 ].GetName( strName );

    // Show selected game image
    const CXBSavedGame& CurrGame = m_GameList[ m_iCurrGame ];
    DrawMsgBox( g_avUIRects[GS_META], L"", m_FontBig, 0 );
    LPDIRECT3DTEXTURE8 pGameImage;
    if( CurrGame.GetImage( &pGameImage ) )
    {
        RenderTile( g_avUIRects[GS_METAIMG], pGameImage );
        pGameImage->Release();
    }

    // Show selected game name/size/date/time. "Empty slot" gets today/now
    WCHAR strDate[32];
    WCHAR strTime[32];
    CHAR strBlocks[32];
    DWORD dwBlocks = 0;
    if( CurrGame.IsEmpty() )
    {
        lstrcpynW( strDate, strTODAY[ m_iLang ], 32 );
        lstrcpynW( strTime, strNOW[ m_iLang ], 32 );

        // Show the size of the game to be saved
        dwBlocks = GetSavedGameSize() / CXBStorageDevice::GetBlockSize();
    }
    else
    {
        FILETIME ftLastWriteTime = CurrGame.GetLastWriteTime();
        CXBConfig::FormatDateTime( ftLastWriteTime, strDate, strTime );

        DWORD dwSaveBytes = CurrGame.GetSize();
        dwBlocks = dwSaveBytes / CXBStorageDevice::GetBlockSize();
    }

    CXBConfig::FormatInt( dwBlocks, strBlocks );
    wsprintfW( strStats, strFORMAT_GAME[ m_iLang ], CurrGame.GetName(), strBlocks, 
               strDate, strTime );
    m_FontMed.DrawText( g_avUIRects[GS_METATEXT].x, g_avUIRects[GS_METATEXT].y,
                        0xFF000000, strStats, 0 );

    // Display list of games
    DWORD j = 0;
    FLOAT fUnit = ( g_avUIRects[GS_GAMELIST].w - g_avUIRects[GS_GAMELIST].y ) / 
                  ( MAX_GAMES_DISPLAYED * GAME_SPACE_MULTIPLE + 
                  ( MAX_GAMES_DISPLAYED - 1 ) );
    FLOAT fHeight = GAME_SPACE_MULTIPLE * fUnit;
    FLOAT fSpacing = ( GAME_SPACE_MULTIPLE + 1 ) * fUnit;
    for( DWORD i = m_iTopGame; i < m_GameList.size() && 
                               j < MAX_GAMES_DISPLAYED; ++i, ++j )
    {
        const CXBSavedGame& SavedGame = m_GameList[ i ];

        // The top slot is the "empty space" for new saves
        WCHAR strGame[ MAX_GAMENAME ];
        lstrcpynW( strGame, SavedGame.IsEmpty() ? strEMPTY_SPACE[ m_iLang ] : 
                                                  SavedGame.GetName(), MAX_GAMENAME );

        D3DXVECTOR4 vec = g_avUIRects[GS_GAMELIST];
        vec.y = g_avUIRects[GS_GAMELIST].y + j * fSpacing;
        vec.w = vec.y + fHeight;
        if( i == m_iCurrGame )
            RenderSelection( vec, GAME_SELECTION_RATIO * fUnit );

        // Each game is its own "box"
        DrawMsgBox( vec, strGame, m_FontMed, XBFONT_CENTER_Y );
    }

    // Show scroll arrows
    BOOL bShowTopArrow = m_iTopGame > 0;
    BOOL bShowBtmArrow = m_iTopGame + MAX_GAMES_DISPLAYED < m_GameList.size();
    if( bShowTopArrow )
        RenderTile( g_avUIRects[GS_UP], m_ptUp );
    if( bShowBtmArrow )
        RenderTile( g_avUIRects[GS_DOWN], m_ptDown );

    // Buttons
    DrawButton( 80.0f, BTN_A );
    DrawButton( 270.0f, BTN_Y );
    DrawButton( 460.0f, BTN_B );
}




//-----------------------------------------------------------------------------
// Name: RenderOverwrite()
// Desc: Display "overwrite" question
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderOverwrite() const
{
    RenderGameList();
    DrawMsgBox( g_avUIRects[MB_MESSAGE], strOVERWRITE[ m_iLang ], m_FontBig, 
                XBFONT_CENTER_X | XBFONT_CENTER_Y );
    RenderYesNo();
}




//-----------------------------------------------------------------------------
// Name: RenderDelete()
// Desc: Display "delete" question
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderDelete() const
{
    RenderGameList();
    DrawMsgBox( g_avUIRects[MB_MESSAGE], strDELETE[ m_iLang ], m_FontBig, 
                XBFONT_CENTER_X | XBFONT_CENTER_Y );
    RenderYesNo();
}




//-----------------------------------------------------------------------------
// Name: RenderYesNo()
// Desc: Display Yes/No boxes
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderYesNo() const
{
    RenderSelection( m_Answer == ANSWER_YES ? g_avUIRects[MB_YES] : 
                                              g_avUIRects[MB_NO], 10.0f );
    m_FontBig.DrawText( ( g_avUIRects[MB_YES].x + g_avUIRects[MB_YES].z ) / 2,
                        ( g_avUIRects[MB_YES].y + g_avUIRects[MB_YES].w ) / 2,
                        0xFF000000, strYES[ m_iLang ], 
                        XBFONT_CENTER_X | XBFONT_CENTER_Y );

    m_FontBig.DrawText( ( g_avUIRects[MB_NO].x + g_avUIRects[MB_NO].z ) / 2,
                        ( g_avUIRects[MB_NO].y + g_avUIRects[MB_NO].w ) / 2,
                        0xFF000000, strNO[ m_iLang ], 
                        XBFONT_CENTER_X | XBFONT_CENTER_Y );
}




//-----------------------------------------------------------------------------
// Name: DrawButton()
// Desc: Display button image and descriptive text
//-----------------------------------------------------------------------------
VOID CXBLoadSave::DrawButton( FLOAT fX, WCHAR chButton ) const
{
    const WCHAR* strText = NULL;
    switch( chButton )
    {
        case BTN_A : strText = strA_SELECT[ m_iLang ]; break;
        case BTN_B : strText = strB_BACK  [ m_iLang ]; break;
        case BTN_Y : strText = strY_DELETE[ m_iLang ]; break;
        default : assert( FALSE ); return;
    }
    WCHAR strButton[2] = { chButton, 0 };

    m_FontBtn.DrawText( fX, BUTTON_Y, BUTTON_COLOR, strButton );
    m_FontBig.DrawText( fX + BUTTON_OFFSET, BUTTON_Y, BUTTON_TEXT, strText );
}




//-----------------------------------------------------------------------------
// Name: DrawMsgBox
// Desc: Draws some text inside of a message box tile
//-----------------------------------------------------------------------------
VOID CXBLoadSave::DrawMsgBox( const D3DXVECTOR4& rc, const WCHAR* strMessage,
                              CXBFont& Font, DWORD dwFlags ) const
{
    CXBFont* pFont = &Font;

    // Render the message box tile
    RenderTile( rc, m_ptMsgBox );

    // Figure out our text position, offset into the box
    FLOAT x;
    if( dwFlags & XBFONT_RIGHT )
        x = rc.z - MB_TEXT_OFFSET;
    else if ( dwFlags & XBFONT_CENTER_X )
        x = rc.x + ( rc.z - rc.x ) / 2;
    else
        x = rc.x + MB_TEXT_OFFSET;

    FLOAT y;
    if( dwFlags & XBFONT_CENTER_Y )
        y = rc.y + ( rc.w - rc.y ) / 2;
    else
        y = rc.y + MB_TEXT_OFFSET;

    // TCR 3-26 Unsupported Characters
    WCHAR strMessageX[1024];
    lstrcpynW( strMessageX, strMessage, 1024 );
    Font.ReplaceInvalidChars( strMessageX, L'\x7F' );

    // If the text is not preformatted (no linefeeds), check the text width
    const WCHAR* strDraw = strMessageX;
    WCHAR strText[1024];
    if( wcschr( strMessageX, L'\n' ) == NULL )
    {
        FLOAT fWidth;
        FLOAT fHeight;
        pFont->GetTextExtent( strMessageX, &fWidth, &fHeight );
        FLOAT fMaxWidth = rc.z - rc.x - MB_TEXT_OFFSET - MB_TEXT_OFFSET;

        // If the text is too large
        if( fWidth > fMaxWidth )
        {
            // First try a smaller font
            if( pFont == &m_FontBig )
                pFont = &m_FontMed;

            pFont->GetTextExtent( strMessageX, &fWidth, &fHeight );

            // If the text is still too large, shrink to fit
            if( fWidth > fMaxWidth )
            {
                // Account for ellipses
                FLOAT fDotWidth;
                FLOAT fDotHeight;
                pFont->GetTextExtent( strELLIPSES, &fDotWidth, &fDotHeight );

                lstrcpyW( strText, strMessageX );
                strDraw = strText;

                // Remove characters from the end of the text
                // until the text and ellipses fit
                WCHAR* pEnd = strText + lstrlenW( strText ) - 1;
                do
                {
                    *pEnd = 0;
                    --pEnd;
                    pFont->GetTextExtent( strText, &fWidth, &fHeight );
                } while( pEnd != strText && ( fWidth + fDotWidth ) > fMaxWidth );

                // Append ellipses
                lstrcatW( strText, strELLIPSES );
            }
        }
    }

    // Draw the text
    pFont->DrawText( x, y, MB_TEXT_COLOR, strDraw, dwFlags );
}




//-----------------------------------------------------------------------------
// Name: RenderTile
// Desc: Renders the texture at the given rect.  If bPulse is TRUE, also 
//       does an alpha-blend pulse based on the application time (for
//       things like selections textures)
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderTile( const D3DXVECTOR4& rc, 
                              const LPDIRECT3DTEXTURE8 pTile,
                              BOOL bPulse ) const
{
    if( pTile == NULL )
        return;

    struct TILEVERTEX
    {
        D3DXVECTOR4 p;
        D3DXVECTOR2 t;
    };
    TILEVERTEX* pVertices;

    LPDIRECT3DVERTEXBUFFER8 pvbTemp;
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( TILEVERTEX ), 
                                      D3DUSAGE_WRITEONLY, 
                                      D3DFVF_XYZRHW|D3DFVF_TEX1, 
                                      D3DPOOL_MANAGED, &pvbTemp );

    // Create a quad for us to render our texture on
    pvbTemp->Lock( 0, 0, (BYTE **)&pVertices, 0L );
    pVertices[0].p = D3DXVECTOR4( rc.x - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[0].t = D3DXVECTOR2( 0.0f, 1.0f ); // Bottom Left
    pVertices[1].p = D3DXVECTOR4( rc.x - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[1].t = D3DXVECTOR2( 0.0f, 0.0f ); // Top    Left
    pVertices[2].p = D3DXVECTOR4( rc.z - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[2].t = D3DXVECTOR2( 1.0f, 1.0f ); // Bottom Right
    pVertices[3].p = D3DXVECTOR4( rc.z - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[3].t = D3DXVECTOR2( 1.0f, 0.0f ); // Top    Right
    pvbTemp->Unlock();

    if( bPulse )
    {
        // If we're pulsing, calculate the alpha, and set up the texture stage
        FLOAT fPulse = (FLOAT)( cos( 4.0f * m_fTime ) + 1.0f ) / 3.0f + 1.0f / 3.0f;
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, DWORD( fPulse * 255 ) << 24 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    }

    // Set up our state
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetStreamSource( 0, pvbTemp, sizeof( TILEVERTEX ) );

    // Render the quad with our texture
    m_pd3dDevice->SetTexture( 0, pTile );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    m_pd3dDevice->SetTexture( 0, NULL );

    if( bPulse )
    {
        // Clean up our alpha state
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    }

    pvbTemp->Release();
}




//-----------------------------------------------------------------------------
// Name: RenderBar
// Desc: Renders a progress bar at the given rect, fPercent full
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderBar( const D3DXVECTOR4& vecBar, FLOAT fPercent,
                             DWORD dwColor ) const
{
    struct BARVERTEX
    { 
        D3DXVECTOR4 p;
    };

    LPDIRECT3DVERTEXBUFFER8 pvbTemp;
    m_pd3dDevice->CreateVertexBuffer( 9 * sizeof( BARVERTEX ), D3DUSAGE_WRITEONLY,
                                      FVF_BARVERTEX, D3DPOOL_MANAGED, &pvbTemp );

    // Create vertices for the filled-in bar and for the outer border
    BARVERTEX* pVertices;
    pvbTemp->Lock( 0, 0, (BYTE **)&pVertices, 0L );

    // Quad for filled-in section
    pVertices[0].p = D3DXVECTOR4( vecBar.x - 0.5f, vecBar.w - 0.5f, 1.0f, 1.0f );
    pVertices[1].p = D3DXVECTOR4( vecBar.x - 0.5f, vecBar.y - 0.5f, 1.0f, 1.0f );
    pVertices[2].p = D3DXVECTOR4( vecBar.x + fPercent*(vecBar.z - vecBar.x) - 0.5f,
                                  vecBar.w - 0.5f, 1.0f, 1.0f );
    pVertices[3].p = D3DXVECTOR4( vecBar.x + fPercent*(vecBar.z - vecBar.x) - 0.5f,
                                  vecBar.y - 0.5f, 1.0f, 1.0f );
    
    // Line-strip rectangle for border
    pVertices[4].p = D3DXVECTOR4( vecBar.x, vecBar.w, 1.0f, 1.0f );
    pVertices[5].p = D3DXVECTOR4( vecBar.x, vecBar.y, 1.0f, 1.0f );
    pVertices[6].p = D3DXVECTOR4( vecBar.z, vecBar.y, 1.0f, 1.0f );
    pVertices[7].p = D3DXVECTOR4( vecBar.z, vecBar.w, 1.0f, 1.0f );
    pVertices[8].p = D3DXVECTOR4( vecBar.x, vecBar.w, 1.0f, 1.0f );
    pvbTemp->Unlock();

    m_pd3dDevice->SetVertexShader( FVF_BARVERTEX );
    m_pd3dDevice->SetStreamSource( 0, pvbTemp, sizeof( BARVERTEX ) );
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );

    // First render the filled-in-section in BARCOLOR
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Then render the linestrip border in BARBORDER
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, BARBORDER );
    m_pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, 4, 4 );

    pvbTemp->Release();
}




//-----------------------------------------------------------------------------
// Name: RenderSelection
// Desc: Renders the selection texture, with a given border around the 
//       rectangle
//-----------------------------------------------------------------------------
VOID CXBLoadSave::RenderSelection( const D3DXVECTOR4& rc, FLOAT fOffset ) const
{
    D3DXVECTOR4 vec = rc;

    vec.x -= fOffset;
    vec.y -= fOffset;
    vec.z += fOffset;
    vec.w += fOffset;

    RenderTile( vec, m_ptXboxSel, TRUE );
}




//-----------------------------------------------------------------------------
// Name: IsDeviceInserted
// Desc: TRUE if given device is available (even if unusable/full). The hard
//       disk is always inserted.
//-----------------------------------------------------------------------------
BOOL CXBLoadSave::IsDeviceInserted( DWORD i ) const
{
    // Hard disk
    if( i == 0 )
        return m_DeviceList[ i ].IsValid();

    return m_MemUnitList[ i-1 ].IsValid();
}




//-----------------------------------------------------------------------------
// Name: AnyMemoryUnitsInserts()
// Desc: TRUE if any MUs are currently available for load/save
//-----------------------------------------------------------------------------
BOOL CXBLoadSave::AnyMemoryUnitsInserted() const
{
    for( DWORD i = 1; i < MAX_STORAGE_DEVICES; ++i )
    {
        if( IsDeviceInserted( i ) )
            return TRUE;
    }
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: IsSpaceAvail()
// Desc: TRUE if space is available for the save game data on the current
//       device, even if other saves have to be deleted. If other saves
//       have to be deleted, returns the bytes that must be deleted
//       in the optional pBytesNeeded param.
//       Bytes needed is cluster based, and is always evenly divisible by
//       the cluster size of the device.
//-----------------------------------------------------------------------------
BOOL CXBLoadSave::IsSpaceAvail( DWORD* pBytesNeeded ) const
{
    if( pBytesNeeded != NULL )
        *pBytesNeeded = 0;

    // Examine the device storage statistics
    const StorageDev& CurrDev = m_DeviceList[ m_iCurrDev ];
    ULONGLONG qwTotalBytes;
    ULONGLONG qwUsedBytes;
    ULONGLONG qwFreeBytes;
    CurrDev.GetSize( qwTotalBytes, qwUsedBytes, qwFreeBytes );

    // Scads o' space
    if( qwFreeBytes > ULONGLONG( ULONG_MAX ) )
        return TRUE;

    // Convert to DWORD
    DWORD dwFreeBytes = DWORD( qwFreeBytes );

    // Plenty o' space
    if( GetSavedGameSize() < dwFreeBytes )
        return TRUE;

    // Determine the number of bytes that would need to be deleted
    if( pBytesNeeded != NULL )
    {
        *pBytesNeeded = GetSavedGameSize() - dwFreeBytes;
        return FALSE;
    }

    // Scan through the size of games on the device to see if there might
    // be room
    DWORD dwGameBytes = 0;
    for( SavedGameIndex i = 0; i < m_GameList.size(); ++i )
    {
        const CXBSavedGame& SavedGame = m_GameList[ i ];
        dwGameBytes += SavedGame.GetSize();

        if( GetSavedGameSize() < dwFreeBytes + dwGameBytes )
            return TRUE;
    }

    // Not enough room, even if games were deleted
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: DeleteGame()
// Desc: Remove the selected saved game container and update the game list
//-----------------------------------------------------------------------------
VOID CXBLoadSave::DeleteGame()
{
    // Free up the space and adjust the list
    if( m_GameList[ m_iCurrGame ].DeleteGame() )
        DeleteGameFromList();
}




//-----------------------------------------------------------------------------
// Name: DeleteGame( SavedGameIndex )
// Desc: Remove the given game from the game list
//-----------------------------------------------------------------------------
VOID CXBLoadSave::DeleteGameFromList()
{
    // Remove the element from the list
    SavedGameIndex j = 0;
    SavedGameList::iterator i;
    for( i = m_GameList.begin(); i != m_GameList.end() && j < m_iCurrGame; ++i, ++j )
        ;
    assert( i != m_GameList.end() );
    m_GameList.erase( i );

    // Adjust the current game index if we were on the last game
    if( m_iCurrGame == m_GameList.size() )
    {
        if( m_iCurrGame > 0 )
            --m_iCurrGame;
    }

    // Adjust the top element if the last pane of games displayed
    if( m_iTopGame + MAX_GAMES_DISPLAYED - 1 == m_GameList.size() )
    {
        if( m_iTopGame > 0 )
            --m_iTopGame;
    }
}




//-----------------------------------------------------------------------------
// Name: SortByLastWriteTime()
// Desc: Sorting predicate for std::sort function in BuildGameList(). The
//       game list is sorted by last write time (most recent first).
//-----------------------------------------------------------------------------
bool CXBLoadSave::SortByLastWriteTime( const CXBSavedGame& lhs, 
                                       const CXBSavedGame& rhs ) // static
{
    return( lhs.GetLastWriteQword() > rhs.GetLastWriteQword() );
}




//-----------------------------------------------------------------------------
// Name: BuildGameList()
// Desc: Constructs the list of games on the current device. If we're in save
//       mode, includes an "empty space" game in the top slot. Games are sorted
//       by last write time, most recent first.
//-----------------------------------------------------------------------------
VOID CXBLoadSave::BuildGameList()
{
    const StorageDev& CurrDev = m_DeviceList[ m_iCurrDev ];

    // Nuke the previous list, if any
    m_GameList.clear();

    // Begin timing the load
    CXBStopWatch stopWatch( TRUE );

    // Build the list of all games on the device
    XGAME_FIND_DATA XgameFindData;
    if( CurrDev.FindFirstSaveGame( XgameFindData ) )
    {
        m_GameList.push_back( CXBSavedGame( XgameFindData ) );
        while( CurrDev.FindNextSaveGame( XgameFindData ) )
        {
            m_GameList.push_back( CXBSavedGame( XgameFindData ) );

            // If there are lots of games to load, display a message
            if( stopWatch.GetElapsedSeconds() > 0.5f )
            {
                // Render a progress bar by first rendering the normal screen
                CXBLoadSave::Render();

                // Then draw the message on top
                DrawMsgBox( g_avUIRects[MB_MESSAGE], strLOADING_GAME_LIST[ m_iLang ],
                            m_FontBig, XBFONT_CENTER_X | XBFONT_CENTER_Y );

                // We're in a tight loop here, so we have to call present ourselves
                m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
            }
        }
    }

    // Sort the list by last write time (most recent first)
    std::sort( m_GameList.begin(), m_GameList.end(), SortByLastWriteTime );

    // If we're saving, then add an "empty slot" at the top of the list
    if( m_Mode == MODE_SAVE )
        m_GameList.push_front( CXBSavedGame() );

    // Always begin at the top of the list
    m_iTopGame = m_iCurrGame = 0;
}




//-----------------------------------------------------------------------------
// Name: GetSavedGameFileName()
// Desc: Returns the full path to the saved game data file for the current
//       save game
//-----------------------------------------------------------------------------
VOID CXBLoadSave::GetSavedGameFileName( CHAR* strFile ) const
{
    assert( strFile != NULL );
    const CXBSavedGame& SavedGame = m_GameList[ m_iCurrGame ];

    // Get the game directory
    lstrcpynA( strFile, SavedGame.GetDirectory(), MAX_PATH );

    // Append the game file name
    lstrcatA( strFile, strSAVE_FILE );
}




//-----------------------------------------------------------------------------
// Name: GetSavedGameSize()
// Desc: Returns the total number of bytes required to save the game
//-----------------------------------------------------------------------------
DWORD CXBLoadSave::GetSavedGameSize() const
{
    const StorageDev& CurrDev = m_DeviceList[ m_iCurrDev ];

    DWORD dwSaveGameSize = CurrDev.GetFileBytes( m_dwSavedGameBytes + 
                                                 sizeof( XValidationHeader ) );
    DWORD dwImageSize = CurrDev.GetFileBytes( IMAGE_META_HDR_SIZE + 
                                             IMAGE_META_DATA_SIZE );
    DWORD dwOverhead = CurrDev.GetSaveGameOverhead();

    return( dwSaveGameSize + dwImageSize + dwOverhead );
}




//-----------------------------------------------------------------------------
// Name: GenerateSavedGameName()
// Desc: Uses the patented "Never Fails Save Game Naming Kit" to derive a fake
//       name for the saved game. Your game would typically generate a name
//       based on the current level, character, or other game context
//       information. Using the virtual keyboard to enter a game name should
//       only be done as a last resort.
//-----------------------------------------------------------------------------
VOID CXBLoadSave::GenerateSavedGameName( WCHAR* strName ) // static
{
    struct GameName
    {
        WCHAR* strAdj;
        WCHAR* strNoun;
    };
    const INT MAX_NAME = 16;
    GameName NameList[ MAX_NAME ] =
    {
        { L"Hydro",    L"Fortress" },
        { L"Roller",   L"Castle"   },
        { L"Opposing", L"Level"    },
        { L"Final",    L"Ruins"    },
        { L"Monkey",   L"Island"   },
        { L"Wicked",   L"Hamster"  },
        { L"Battle",   L"Zone"     },
        { L"Dark",     L"Corridor" },   
        { L"Delta",    L"Tundra"   },
        { L"Evil",     L"Core"     },
        { L"Galactic", L"Patrol"   },
        { L"Mech",     L"Fields"   },
        { L"Red",      L"Vista"    },
        { L"Shadow",   L"Company"  },
        { L"Hamster",  L"Sewers"   },
        { L"Power",    L"Outpost"  }   
    };

    // Choose one word from each column
    INT nColumnOne = rand() % MAX_NAME;
    INT nColumnTwo = rand() % MAX_NAME;

    // Build the name
    lstrcpyW( strName, NameList[ nColumnOne ].strAdj );
    lstrcatW( strName, L" " );
    lstrcatW( strName, NameList[ nColumnTwo ].strNoun );
}
