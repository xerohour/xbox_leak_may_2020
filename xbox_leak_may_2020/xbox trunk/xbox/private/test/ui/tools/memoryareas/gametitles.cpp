/*****************************************************
*** gametitles.cpp
***
*** CPP file for our game title classes.  These
*** classes will enumerate and store information
*** about all of the Game Titles, and their Saved
*** games on the Xbox
***
*** by James N. Helm
*** June 16th, 2001
***
*****************************************************/

#include "memarpch.h"
#include "memoryarea.h"
#include "gametitles.h"

////////////////
////////////////
// Saved Game //
////////////////
////////////////

// Constructor
CSavedGame::CSavedGame()
: m_pwszName( NULL ),
m_dwBlockSize( 0 ),
m_pszGameID( NULL ),
m_pszGameDirectory( NULL )
{
    ZeroMemory( &m_GameDate, sizeof( m_GameDate ) );
}

// Destructor
CSavedGame::~CSavedGame()
{
    if( NULL != m_pwszName )
    {
        delete[] m_pwszName;
        m_pwszName = NULL;
    }

    if( NULL != m_pszGameID )
    {
        delete[] m_pszGameID;
        m_pszGameID = NULL;
    }

    if( NULL != m_pszGameDirectory )
    {
        delete[] m_pszGameDirectory;
        m_pszGameDirectory = NULL;
    }
}


// Initialize the Saved Game object and populate it's data
HRESULT CSavedGame::Init( XGAME_FIND_DATA* pGameData )
{
    if( NULL == pGameData )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CSavedGame::Init():Invalid parameter passed in!!" );

        return E_INVALIDARG;
    }

    //
    // Store the base Game Information
    //

    //
    // Handle the Saved Game name
    //
    if( NULL != m_pwszName )
    {
        delete[] m_pwszName;
        m_pwszName = NULL;
    }

    m_pwszName = new WCHAR[wcslen(pGameData->szSaveGameName)+1];
    if( NULL == m_pwszName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CSavedGame::Init():Failed to allocate memory!!" );

        return E_OUTOFMEMORY;
    }

    wcscpy( m_pwszName, pGameData->szSaveGameName );

    //
    // Handle the Saved Game Directory
    //
    if( NULL != m_pszGameDirectory )
    {
        delete[] m_pszGameDirectory;
        m_pszGameDirectory = NULL;
    }

    m_pszGameDirectory = new char[strlen(pGameData->szSaveGameDirectory)+1];
    if( NULL == m_pszGameDirectory )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CSavedGame::Init():Failed to allocate memory!!" );

        return E_OUTOFMEMORY;
    }

    strcpy( m_pszGameDirectory, pGameData->szSaveGameDirectory );

    //
    // Handle the Date/Time information
    //
    HRESULT hr = S_OK;
    if( !FileTimeToSystemTime( &pGameData->wfd.ftLastWriteTime, &m_GameDate ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CSavedGame::Init():Failed to convert FileTimeToSystemTime!! Error - '0x%.8X (%d)'", GetLastError(), GetLastError() );

        hr = E_FAIL;
    }

    // Get the Game ID by parsing the Game Directory
    if( NULL != m_pszGameID )
    {
        delete[] m_pszGameID;
        m_pszGameID = NULL;
    }

    char* pszIDPointer = strrchr( m_pszGameDirectory, '\\' );
    if( NULL == pszIDPointer )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CSavedGame::Init():Failed to Get the Title ID!!" );

        return E_FAIL;
    }

    pszIDPointer++;
    m_pszGameID = new char[strlen( pszIDPointer )+1];
    if( NULL == m_pszGameID )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CSavedGame::Init():Failed to Allocate memory!!" );

        return E_OUTOFMEMORY;
    }

    strcpy( m_pszGameID, pszIDPointer );

    // TODO: Get the Game Block Size by navigating the directory and totaling the size of all files

    return hr;
}


// Get a pointer to the name of the saved game
WCHAR* CSavedGame::GetName()
{
    if( NULL == m_pwszName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CSavedGame::GetName():Returning a NULL pointer!!" );
    }

    return m_pwszName;
}


// Get a pointer to the Saved Game directory
char* CSavedGame::GetDirectory()
{
    if( NULL == m_pszGameDirectory )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CSavedGame::GetDirectory():Returning a NULL pointer!!" );
    }

    return m_pszGameDirectory;
}


// Returns the Block size of the Saved Game
DWORD CSavedGame::GetBlockSize()
{
    return m_dwBlockSize;
}


// Get a pointer to the Game ID of the Saved Game
char* CSavedGame::GetGameID()
{
    if( NULL == m_pszGameID )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CSavedGame::GetGameID():Returning a NULL pointer!!" );
    }

    return m_pszGameID;
}


////////////////
////////////////
// Game Title //
////////////////
////////////////

// Constructor
CGameTitle::CGameTitle()
: m_pwszName( NULL ),
m_dwBlockSize( 0 ),
m_pszTitleID( NULL ),
m_pszDirectory( NULL )
{
    // Make sure if we empty our list we clean up properly
    m_SavedGameList.SetDeleteDataItem( TRUE );
}


// Destructor
CGameTitle::~CGameTitle()
{
    if( NULL != m_pszTitleID )
    {
        delete[] m_pszTitleID;
        m_pszTitleID = NULL;
    }

    if( NULL != m_pszDirectory )
    {
        delete[] m_pszDirectory;
        m_pszDirectory = NULL;
    }

    if( NULL != m_pwszName )
    {
        delete[] m_pwszName;
        m_pwszName = NULL;
    }
}


// Initialize our Game Title
HRESULT CGameTitle::Init( char* pszDirectory, CMemoryArea* pMemoryArea )
{
    if( NULL == pszDirectory )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::Init():Invalid argument passed in!!" );

        return E_INVALIDARG;
    }

    //
    // Handle the directory
    //
    if( NULL != m_pszDirectory )
    {
        delete[] m_pszDirectory;
        m_pszDirectory = NULL;
    }

    m_pszDirectory = new char[strlen( pszDirectory )+1];
    if( NULL == m_pszDirectory )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::Init():Failed to allocate memory!!" );

        return E_OUTOFMEMORY;
    }

    strcpy( m_pszDirectory, pszDirectory );

    //
    // Handle the Title ID
    //
    if( NULL != m_pszTitleID )
    {
        delete[] m_pszTitleID;
        m_pszTitleID = NULL;
    }

    char* pszIDPointer = strrchr( m_pszDirectory, '\\' );
    if( NULL == pszIDPointer )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::Init():Failed to Get the Title ID!!" );

        return E_FAIL;
    }

    pszIDPointer++;
    m_pszTitleID = new char[strlen( pszIDPointer )+1];
    if( NULL == m_pszTitleID )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::Init():Failed to Allocate memory!!" );

        return E_OUTOFMEMORY;
    }

    strcpy( m_pszTitleID, pszIDPointer );

    //
    // Handle the Saved Games
    //
    HRESULT hr = ProcessSavedGames( m_pszTitleID, pMemoryArea );
    if( FAILED( hr ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::Init():Failed to ProcessSavedGames!! Error - '0x%.8X (%d)'", hr, hr );
    }

    // Get the Game Name
    if( NULL != m_pwszName )
    {
        delete[] m_pwszName;
        m_pwszName = NULL;
    }

    char pszFileName[MAX_PATH+1];
    ZeroMemory( pszFileName, MAX_PATH+1 );

    _snprintf( pszFileName, MAX_PATH, "%s\\%s", m_pszDirectory, GAME_TITLE_XBX_FILENAME );

    FILE* pfTitleMeta = fopen( pszFileName, "rb" );
    if( NULL == pfTitleMeta )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::Init():Failed to open the Game Name file!! File - '%hs', Error - '0x%.8X (%d)'", pszFileName, GetLastError(), GetLastError() );

        return E_FAIL;
    }

    WCHAR pwszBuffer[MAX_PATH+1];
    ZeroMemory( pwszBuffer, sizeof( WCHAR ) * ( MAX_PATH+1 ) );

    while( fgetws( pwszBuffer, MAX_PATH, pfTitleMeta ) )
    {
        WCHAR* pwszStringLoc = wcsstr( pwszBuffer, GAME_TITLE_NAME_TOKEN_W );
        if( NULL != pwszStringLoc )
        {
            // Adjust our pointer to skip the token
            pwszStringLoc = pwszStringLoc + wcslen( GAME_TITLE_NAME_TOKEN_W );

            // Allocate memory for our buffer
            m_pwszName = new WCHAR[wcslen( pwszStringLoc ) - 1];    // We don't need to allocate space for the null, since this string has a CR at the end, we'll use that
            if( NULL == m_pwszName )
            {
                XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::Init():Failed to Allocate memory!!" );

                hr = E_FAIL;

                break;
            }
            else
            {
                m_pwszName[wcslen( pwszStringLoc ) - 2] = L'\0';
                wcsncpy( m_pwszName, pwszStringLoc, wcslen( pwszStringLoc ) - 2 );

                break;
            }
        }
    }

    XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::Init():Game Name - '%ls'", m_pwszName );

    fclose( pfTitleMeta );
    pfTitleMeta = NULL;

    // TODO: Get the Game Block Size

    return hr;
}


// Returns the number of saved games for the game title
DWORD CGameTitle::GetNumSavedGames()
{
    return m_SavedGameList.GetNumItems();
}


// Get the total block size of the Game Title
DWORD CGameTitle::GetBlockSize()
{
    return m_dwBlockSize;
}


// Get a pointer to the name of the Game Title
WCHAR* CGameTitle::GetName()
{
    if( NULL == m_pwszName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::GetName():Returning a NULL pointer!!" );
    }

    return m_pwszName;
}


// Get a pointer to the directory of the Game Title
char* CGameTitle::GetDirectory()
{
    if( NULL == m_pszDirectory )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::GetDirectory():Returning a NULL pointer!!" );
    }

    return m_pszDirectory;
}


// Get a pointer to the title ID of the Game Title
char* CGameTitle::GetTitleID()
{
    if( NULL == m_pszTitleID )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::GetTitleID():Returning a NULL pointer!!" );
    }

    return m_pszTitleID;
}


// Get the name of a Saved Game
WCHAR* CGameTitle::GetSavedGameName( unsigned int uiIndex )
{
    if( uiIndex >= m_SavedGameList.GetNumItems() )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::GetSavedGameName():Invalid Index passed in! Arg - '%u', MAX - '%u'", uiIndex, m_SavedGameList.GetNumItems() - 1 );

        return NULL;
    }

    return m_SavedGameList.GetNode( uiIndex )->GetName();
}


// Get the ID of a Saved Game
char* CGameTitle::GetSavedGameID( unsigned int uiIndex )
{
    if( uiIndex >= m_SavedGameList.GetNumItems() )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::GetSavedGameID():Invalid Index passed in! Arg - '%u', MAX - '%u'", uiIndex, m_SavedGameList.GetNumItems() - 1 );

        return NULL;
    }

    return m_SavedGameList.GetNode( uiIndex )->GetGameID();
}


// Get the block size of a Saved Game
DWORD CGameTitle::GetSavedGameBlockSize( unsigned int uiIndex )
{
    if( uiIndex >= m_SavedGameList.GetNumItems() )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::GetSavedGameBlockSize():Invalid Index passed in! Arg - '%u', MAX - '%u'", uiIndex, m_SavedGameList.GetNumItems() - 1 );

        return NULL;
    }

    return m_SavedGameList.GetNode( uiIndex )->GetBlockSize();
}


// Process the Saved Games for the specified Game Title
HRESULT CGameTitle::ProcessSavedGames( char* pszTitleID, CMemoryArea* pMemoryArea )
{
    if( NULL == pszTitleID )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::ProcessSavedGames():Invalid Argument Passed In!!" );

        return E_INVALIDARG;
    }

    m_SavedGameList.EmptyList();

    // Map the proper UData Drive
    pMemoryArea->MapUDataToTitleID( pszTitleID );

    char pszGameDrive[4];
    pszGameDrive[3] = '\0';

    sprintf( pszGameDrive, "%c:\\", pMemoryArea->GetUDataDrive() );

    // Use FindFirst / Next Saved Game to fill our Saved Game Data
    XGAME_FIND_DATA gameFindData;
    ZeroMemory( &gameFindData, sizeof( gameFindData ) );

    HANDLE hGame = XFindFirstSaveGame( pszGameDrive,
                                       &gameFindData );
    if( INVALID_HANDLE_VALUE == hGame )
    {
        if( ERROR_FILE_NOT_FOUND == GetLastError() )
        {
            // There are no Saved Games, so this is OK
            return S_OK;
        }

        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::ProcessSavedGames():Failed to find a Saved Game!! Error - '0x%.8X (%d)'", GetLastError(), GetLastError() );

        return E_FAIL;
    }

    do
    {
        CSavedGame* pSavedGame = new CSavedGame;
        if( NULL == pSavedGame )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CGameTitle::ProcessSavedGames():Failed to allocate memory!!" );

            // Try the next one, but it will fail as well
            continue;
        }

        pSavedGame->Init( &gameFindData );

        m_SavedGameList.AddNode( pSavedGame );
    } while( XFindNextSaveGame( hGame, &gameFindData ) );

    XFindClose( hGame );

    // Remap the U Drive to the proper location
    pMemoryArea->MapUDataToOrigLoc();

    return S_OK;
}


////////////////
////////////////
// Xbox Games //
////////////////
////////////////

// Constructors
CXboxGames::CXboxGames()
{
    // Make sure we clean up after ourselves
    m_GameList.SetDeleteDataItem( TRUE );
}


// Destructor
CXboxGames::~CXboxGames()
{
}


// Initialize the list starting at the specified directory
HRESULT CXboxGames::Init( char* pszDirectory, CMemoryArea* pMemoryArea )
{
    if( NULL == pszDirectory )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CXboxGames::Init():Invalid arg passed in!!" );

        return E_INVALIDARG;
    }

    // Make sure our Game list is empty
    m_GameList.EmptyList();

    char pszBuf[MAX_PATH+1];
    ZeroMemory( pszBuf, MAX_PATH+1 );

    // BUGBUG: What if there is a trailing slash?
    _snprintf( pszBuf, MAX_PATH, "%hs\\*.*", pszDirectory );

    char* apszDirectories[4096];
    for( unsigned int x = 0; x < 4096; x++ )
    {
        apszDirectories[x] = NULL;
    }

    unsigned int uiNumDirs = 0;
    WIN32_FIND_DATA wfd;
    ZeroMemory( &wfd, sizeof( wfd ) );

    HANDLE hFile = FindFirstFile( pszBuf, &wfd );
    if( INVALID_HANDLE_VALUE == hFile )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CXboxGames::Init():Failed to find first file!! Pattern - '%hs', Error - '0x%.8X (%d)'", pszBuf, GetLastError(), GetLastError() );

        // BUGBUG: This might not be a failure.  There could be no titles!
        return E_FAIL;
    }

    do
    {
        // Skip the . and .. directories
        if ( wfd.cFileName[0] == '.' )
        {
            continue;
        }

        ZeroMemory( pszBuf, MAX_PATH+1 );
        _snprintf( pszBuf, MAX_PATH, "%s\\%s", pszDirectory, wfd.cFileName );

        // Check to see if we are dealing with a directory
        if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            apszDirectories[uiNumDirs] = new char[strlen( pszBuf ) + 1];
            if( NULL == apszDirectories[uiNumDirs] )
            {
                XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CXboxGames::Init():Failed to allocate memory!!" );

                continue;
            }

            strcpy( apszDirectories[uiNumDirs], pszBuf );

            uiNumDirs++;
        }
    } while( FindNextFile( hFile, &wfd ) );

    FindClose( hFile );

    // Process the directories we found
    for( unsigned int x = 0; x < uiNumDirs; x++ )
    {
        // Create a Game Title Object
        CGameTitle* pGameTitle = new CGameTitle;
        if( NULL == pGameTitle )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CXboxGames::Init():Failed to allocate memory!!" );

            // We'll try, but the next one will fail too
            continue;
        }
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CXboxGames::Init():apszDirectories[x] - '%hs'", apszDirectories[x] );
        pGameTitle->Init( apszDirectories[x], pMemoryArea );

        m_GameList.AddNode( pGameTitle );

        // Clean up any memory we allocated
        delete[] apszDirectories[x];
        apszDirectories[x] = NULL;
    }

    return S_OK;
}


// Get a pointer to a game at the specified index
CGameTitle* CXboxGames::GetGame( unsigned int uiIndex )
{
    if( uiIndex >= m_GameList.GetNumItems() )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CXboxGames::GetGame():Invalid Index passed in! Arg - '%u', MAX - '%u'", uiIndex, m_GameList.GetNumItems() - 1 );

        return NULL;
    }

    return m_GameList.GetNode( uiIndex );
}


// Returns the number of Games on the Xbox
DWORD CXboxGames::GetNumGames()
{
    return m_GameList.GetNumItems();
}