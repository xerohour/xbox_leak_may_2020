/*****************************************************
*** memconfigs.h
***
*** CPP file for our library of functions that
*** will allows us to configure various memory
*** areas in different ways
***
*** by James N. Helm
*** June 20th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "memconfigs.h"
#include "TCHAR.h"


// A:
// MU Name     : Missing name (No Nickname)
// MU Space    : 0 blocks used
HRESULT CreateMU_A( CMemoryUnit* pMemoryUnit )
{
    return pMemoryUnit->Format( TRUE );
}


// B:
// MU Name     : MAX (31) chars
// Game Config : 0 Game Titles
// MU Space    : 1 block used
HRESULT CreateMU_B( CMemoryUnit* pMemoryUnit )
{
    pMemoryUnit->Format( TRUE );

    pMemoryUnit->Name( L"MAX+1muNAME23456789012345678901" );

    char pszDir[4];
    pszDir[3] = '\0';
    sprintf( pszDir, "%c:\\", pMemoryUnit->GetUDataDrive() );

    pMemoryUnit->FillArea( pszDir, 1 );

    return S_OK;
}


// C:
// MU Name     : 1 char
// Game Config : 1 Game Title, 0 Saved Games
// MU Space    : 1/2 blocks used
HRESULT CreateMU_C( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"1" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 0 );

    //
    // Fill 1/2 of the blocks
    //

    // Get the available and free block sizes
    DWORD dwFree = pMemoryUnit->GetFreeBlocks();
    DWORD dwTotal = pMemoryUnit->GetTotalBlocks();

    DWORD dwNumBlocksToFill = ( dwTotal / 2 ) - ( dwTotal - dwFree );

    char pszDir[4];
    pszDir[3] = '\0';
    sprintf( pszDir, "%c:\\", pMemoryUnit->GetUDataDrive() );

    pMemoryUnit->FillArea( pszDir, dwNumBlocksToFill );

    return S_OK;
}


// D:
// THIS IS IMPOSSIBLE NOW -- MU Name     : MAX + 1 (33) chars
// Game Config : 1 Game Title, 1 Saved Game
// MU Space    : MAX blocks used
HRESULT CreateMU_D( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    // BUGBUG -- Pretty sure we can't set a 33 char name
    pMemoryUnit->Name( L"MAX+1muNAME2345678901234567890123" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 1 );

    //
    // Fill all of the blocks
    //

    // Get the available and free block sizes
    DWORD dwFree = pMemoryUnit->GetFreeBlocks();

    char pszDir[4];
    pszDir[3] = '\0';
    sprintf( pszDir, "%c:\\", pMemoryUnit->GetUDataDrive() );

    pMemoryUnit->FillArea( pszDir, dwFree );

    return S_OK;
}


// E:
// MU Name     : Large (1023 chars)
// Game Config : 2+ Game Titles, 2+ Saved Games
// MU Space    : MAX - 1 blocks used
HRESULT CreateMU_E( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    // BUGBUG -- Pretty sure we can't set a LARGE char name
    // pMemoryUnit->Name( L"Large: 1023 characters - 56789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 2, 2 );
    
    //
    // Fill all of the blocks except 1
    //

    // Get the available and free block sizes
    DWORD dwFree = pMemoryUnit->GetFreeBlocks();

    char pszDir[4];
    pszDir[3] = '\0';
    sprintf( pszDir, "%c:\\", pMemoryUnit->GetUDataDrive() );

    pMemoryUnit->FillArea( pszDir, dwFree - 1 );

    return S_OK;
}


// F:
// MU Name     : Invalid Chars
// Game Config : Game Titles arranged for sorting
HRESULT CreateMU_F( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x000A\x000B\x000C\x000D\x000E\x000F\x0010\x0011\x0012\x0013\x0014\x0015" );

    // Create Game Sorting routine
    CreateSortOrderGames( pMemoryUnit, TRUE );

    return S_OK;
}


// G:
// MU Name     : Empty muname.xbx (0 chars, no Nickname)
// Game Config : MAX (4096) Game Titles, 1 Saved Game, 1 Block Per Game
HRESULT CreateMU_G( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 4096, 1 );

    return S_OK;
}


// H:
// MU Name     : Missing muname.xbx (No Nickname)
// Game Config : MAX (4096) Game Titles, 10 saved Games, 1 Block Per Game
HRESULT CreateMU_H( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 4096, 10 );

    return S_OK;
}


// I:
// MU Name     : Missing muname.xbx (No Nickname)
// Game Config : 1 Game Title, 3 Saved Games ( 1 less than a screen line)
HRESULT CreateMU_I( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 3 );

    return S_OK;
}


// J:
// MU Name     : Missing muname.xbx (No Nickname)
// Game Config : 1 Game Title, 4 Saved Games ( equal to a screen line)
HRESULT CreateMU_J( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 4 );

    return S_OK;
}


// K:
// MU Name     : Missing muname.xbx (No Nickname)
// Game Config : 1 Game Title, 5 Saved Games ( 1 more than a screen line)
HRESULT CreateMU_K( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 5 );

    return S_OK;
}


// L:
// MU Name     : Descriptive
// Game Config : 1 Game Title, MAX (500) Saved Games, 1 Block Per Game
HRESULT CreateMU_L( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"1 Game, 500 Saves" );

    // Create the Game Titles and Saves
    // BUGBUG: 500 won't fit, so we'll use something that will be faster
    CreateGamesAndSaves( pMemoryUnit, 1, 300 );

    return S_OK;
}


// M:
// MU Name     : Descriptive
// Game Config : 1 Game Title, MAX + 1 (501) Saved Games, 1 Block Per Game
HRESULT CreateMU_M( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"1 Game, 501 Saves" );

    // Create the Game Titles and Saves
    // BUGBUG: 500 won't fit, so we'll use something that will be faster
    CreateGamesAndSaves( pMemoryUnit, 1, 300 );

    return S_OK;
}


// N:
// MU Name     : Descriptive
// Game Config : 2 Game Titles, MAX (500) Saved Games, 1 block per game, each titles uses 1/2 space
HRESULT CreateMU_N( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"2 Games, 500 Saves" );

    // Create the Game Titles and Saves
    // BUGBUG: 500 won't fit, so we'll use something that will be faster
    CreateGamesAndSaves( pMemoryUnit, 2, 300 );

    return S_OK;
}


// O:
// MU Name     : Descriptive
// Game Config : 10 Titles, each has 4 or less Saved Games
HRESULT CreateMU_O( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 4 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 10, 4 );

    return S_OK;
}


// P:
// MU Name     : Descriptive
// Game Config : 10 Titles, 1st has 8 Saved Games
HRESULT CreateMU_P( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 1st has 8 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 8 );
    CreateGamesAndSaves( pMemoryUnit, 9, 1 );

    return S_OK;
}


// Q:
// MU Name     : Descriptive
// Game Config : 10 Titles, 1st has 12 Saved Games
HRESULT CreateMU_Q( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 1st has 12 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 12 );
    CreateGamesAndSaves( pMemoryUnit, 9, 1 );

    return S_OK;
}


// R:
// MU Name     : Descriptive
// Game Config : 10 Titles, 1st has 16 Saved Games
HRESULT CreateMU_R( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 1st has 16 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 16 );
    CreateGamesAndSaves( pMemoryUnit, 9, 1 );

    return S_OK;
}

// S:
// MU Name     : Descriptive
// Game Config : 10 Titles, 1st has 25 Saved Games
HRESULT CreateMU_S( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 1st has 25 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 25 );
    CreateGamesAndSaves( pMemoryUnit, 9, 1 );

    return S_OK;
}


// T:
// MU Name     : Descriptive
// Game Config : 10 Titles, 1st has 30 Saved Games
HRESULT CreateMU_T( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 1st has 30 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 30 );
    CreateGamesAndSaves( pMemoryUnit, 9, 1 );

    return S_OK;
}


// U:
// MU Name     : Descriptive
// Game Config : 10 Titles, 1st has 35 Saved Games
HRESULT CreateMU_U( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 1st has 35 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 35 );
    CreateGamesAndSaves( pMemoryUnit, 9, 1 );

    return S_OK;
}


// V:
// MU Name     : Descriptive
// Game Config : 10 Titles, 1st has 40 Saved Games
HRESULT CreateMU_V( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 1st has 40 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 40 );
    CreateGamesAndSaves( pMemoryUnit, 9, 1 );

    return S_OK;
}


// W:
// MU Name     : Descriptive
// Game Config : 10 Titles, 1st has 45 Saved Games
HRESULT CreateMU_W( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 1st has 45 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 45 );
    CreateGamesAndSaves( pMemoryUnit, 9, 1 );

    return S_OK;
}


// X:
// MU Name     : Descriptive
// Game Config : 10 Titles, 1st has 50 Saved Games
HRESULT CreateMU_X( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"10 Games, 1st has 50 Saves" );

    // Create the Game Titles and Saves
    CreateGamesAndSaves( pMemoryUnit, 1, 50 );
    CreateGamesAndSaves( pMemoryUnit, 9, 1 );

    return S_OK;
}


// Y:
// MU Name     : Descriptive
// Save Config : SAVE.XBX is MISSING
// Save Config : SAVE.XBX is EMPTY
// Save Config : SAVE.XBX has SaveName = 0 chars
// Save Config : SAVE.XBX has SaveName = 1 char
// Save Config : SAVE.XBX has SaveName = MAX (63) chars
// Save Config : SAVE.XBX has SaveName = MAX + 1 (64) chars
// Save Config : SAVE.XBX has SaveName = Huge
// Save Config : IMAGE.XBX is MISSING
// Save Config : IMAGE.XBX is SMALL (32x32) in size
// Save Config : IMAGE.XBX is NORMAL (64x64) in size
// Save Config : IMAGE.XBX is LARGER (128x128) in size
// Save Config : IMAGE.XBX is LARGER (256x256) in size
// Save Config : IMAGE.XBX is LARGE (1024x1024) in size
// Save Config : IMAGE.XBX is 1 byte file
// Save Config : IMAGE.XBX is CORRUPT XBX file
// Save Config : Time is set to 12:00am
// Save Config : Time is set to 12:00pm
// Save Config : All META Data Missing (No IMAGE.XBX, or SAVE.XBX)
// Save Config : Parent Game has 0 char title
// Save Config : Parent Game has 1 char title
// Save Config : Parent Game has MAX (63) char title
// Save Config : Parent Game has MAX + 1 (64) char title
// Save Config : Size is 1 block
// Save Config : Save Game cannot be copied (NoCopy=1)
// Save Config : Save Game can be copied (NoCopy=0)
// Save Config : NoCopy=Large
// Save Config : NoCopy=Huge
HRESULT CreateMU_Y( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"Meta Data Vars (1 of 2)" );

    // Create the games
    CreateMetaVarGames( pMemoryUnit );

    return S_OK;
}


// Z:
// MU Name     : Descriptive
// Save Config : Size is MAX (504) blocks
HRESULT CreateMU_Z( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"Meta Data Vars (2 of 2)" );

    // Create a Game Title
    pMemoryUnit->CreateGameTitle( pMemoryUnit->GetUDataDrive(), "00000001" );

    // Mount the MU to our Game Title directory
    pMemoryUnit->Unmount();
    if( FAILED( pMemoryUnit->MapUDataToTitleID( "00000001" ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CreateMU_Z():Failed to MapUDataToTitleID!! ID - '00000001', Error - '0x%0.8X (%d)'", GetLastError(), GetLastError() );
    }
    else
    {
        char pszGameDir[MAX_PATH+1];
        ZeroMemory( pszGameDir, MAX_PATH+1 );
        pMemoryUnit->CreateSavedGame( L"Max Blocks Game",
                                      pszGameDir,
                                      MAX_PATH,
                                      NULL,
                                      "",
                                      FALSE );

        DWORD dwFree = pMemoryUnit->GetFreeBlocks();
        char pszFilePath[MAX_PATH+1];
        ZeroMemory( pszFilePath, MAX_PATH+1 );

        _snprintf( pszFilePath, MAX_PATH, "%hsfill_file.xdk", pszGameDir );
        XDBGWRN( APP_TITLE_NAME_A, "CreateMU_Z():File path - '%hs'", pszFilePath );

        // Fill the Game
        LARGE_INTEGER liNumBlocks;
        liNumBlocks.QuadPart = dwFree * MEMORY_AREA_BLOCK_SIZE_IN_BYTES;
        pMemoryUnit->CreateFileOfSize( pszFilePath, liNumBlocks );
    }

    // Restore the MU to it's orignal Mount location
    pMemoryUnit->Unmount();
    pMemoryUnit->Mount();

    return S_OK;
}


// AA:
// MU Name     : Descriptive
// Save Config : Saved Game contains many smaller (1 block) files
HRESULT CreateMU_AA( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"Saved Game Many Files" );

    // Create a Game Title
    pMemoryUnit->CreateGameTitle( pMemoryUnit->GetUDataDrive(), "00000001" );

    // Mount the MU to our Game Title directory
    pMemoryUnit->Unmount();
    if( FAILED( pMemoryUnit->MapUDataToTitleID( "00000001" ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CreateMU_AA():Failed to MapUDataToTitleID!! ID - '00000001', Error - '0x%0.8X (%d)'", GetLastError(), GetLastError() );
    }
    else
    {
        char pszGameDir[MAX_PATH+1];
        ZeroMemory( pszGameDir, MAX_PATH+1 );
        pMemoryUnit->CreateSavedGame( L"Max Blocks Game",
                                      pszGameDir,
                                      MAX_PATH,
                                      NULL,
                                      "",
                                      FALSE );

        char pszFilePath[MAX_PATH+1];
        ZeroMemory( pszFilePath, MAX_PATH+1 );

        LARGE_INTEGER liNumBlocks;
        liNumBlocks.QuadPart = MEMORY_AREA_BLOCK_SIZE_IN_BYTES;

        DWORD dwFree = pMemoryUnit->GetFreeBlocks();
        for( unsigned int x = 0; x < dwFree; x++ )
        {
            _snprintf( pszFilePath, MAX_PATH, "%hsfile_%d.xdk", pszGameDir, x );

            pMemoryUnit->CreateFileOfSize( pszFilePath, liNumBlocks );
        }
    }

    // Restore the MU to it's orignal Mount location
    pMemoryUnit->Unmount();
    pMemoryUnit->Mount();

    return S_OK;
}



// BB:
// MU Name     : Jap Sort
// Game Config : Game Titles arranged for japanese sorting
HRESULT CreateMU_BB( CMemoryUnit* pMemoryUnit )
{
    // Format the MU
    pMemoryUnit->Format( TRUE );

    // Name the MU
    pMemoryUnit->Name( L"Jap Sort" );

    // Create Game Sorting routine
    CreateSortOrderGames( pMemoryUnit, FALSE );

    return S_OK;
}

void CreateGenGame( CMemoryArea* pMemoryArea, DWORD dwTitleID, WCHAR* pwszGameName, unsigned int uiNumSavedGames, BOOL bPrependLangCodeToName/*=TRUE*/ )
{
    if( NULL == pwszGameName )
    {
        return;
    }

    // Create the Game Titles
    char pszGameTitle[9];
    
    pszGameTitle[8] = '\0';
    sprintf( pszGameTitle, "%0.8X", dwTitleID );
    pMemoryArea->CreateGameTitle( pMemoryArea->GetUDataDrive(), pszGameTitle, pwszGameName, bPrependLangCodeToName );

    if( uiNumSavedGames > 0 )
    {
        if( FAILED( pMemoryArea->MapUDataToTitleID( pszGameTitle ) ) )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CreateGenGame():Failed to MapUDataToTitleID!! ID - '%hs', Error - '0x%0.8X (%d)'", pszGameTitle, GetLastError(), GetLastError() );
        }
        else
        {
            WCHAR pwszSavedGameName[50];
            ZeroMemory( pwszSavedGameName, sizeof( WCHAR ) * 50 );
            for( unsigned int x = 0; x < uiNumSavedGames; x++ )
            {
                _snwprintf( pwszSavedGameName, 49, L"Game #%d", x );
                pMemoryArea->CreateSavedGame( pwszSavedGameName,
                                              NULL,
                                              0,
                                              NULL,
                                              "",
                                              FALSE );
            }
        }

        pMemoryArea->MapUDataToOrigLoc();
    }
}






// This function assumes you have UDATA Mapped to the root of the partition
// This is for character sorting,  specify TRUE or FALSE for english or japanese sorting
void CreateSortOrderGames( CMemoryArea* pMemoryArea, bool english )
{
    DWORD dwTitleIDStartNo = 0x00007000;

    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"ZZ", 0, FALSE );
    
    // Create Saved Games in special sort order
    if( FAILED( pMemoryArea->MapUDataToTitleID( dwTitleIDStartNo - 1 ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CreateSortOrderGames():Failed to MapUDataToTitleID!! ID - '%d', Error - '0x%0.8X (%d)'", dwTitleIDStartNo - 1, GetLastError(), GetLastError() );

        return;
    }

    SYSTEMTIME sysTime;
    ZeroMemory( &sysTime, sizeof( sysTime ) );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 59; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"5th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 2; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"10th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 59;
    pMemoryArea->CreateSavedGame( L"3rd Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 1; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"6th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2099; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"14th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 31; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"9th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 2; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"8th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 23; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"7th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 1;
    pMemoryArea->CreateSavedGame( L"2nd Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 12; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"11th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2050; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"13th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2001; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"12th Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"1st Game", NULL, 0, &sysTime, "", FALSE );

    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 1; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"4th Game", NULL, 0, &sysTime, "", FALSE );


    pMemoryArea->MapUDataToOrigLoc();

    
	if (english)
	{	
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"z",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"zz",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"A",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"AA",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"a",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"aa",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"A",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"AA",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"a",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"aa",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"B",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"b",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"Ab",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"1",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"01",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"10",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"A1",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"ba",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"!!",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"#A#", 2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L" A",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"A A", 2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"ZZ",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"z",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"zz",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"ZZ",  2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"Z",   2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"zz",  2, FALSE  );
	}
	else
	{
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x30ED\x30ED",	2, FALSE );		//last char of Katakana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x30ED",			2, FALSE  );	//last char of Katakana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x3041",			2, FALSE  );	//first char of Hirigana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x3041\x3041",	2, FALSE  );	//first char of Hirigana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"z",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"ZZ",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"a",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"Aa",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x30FC",			2, FALSE  );	//last char of Hirigana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x30FC\x30FC",	2, FALSE  );	//last char of Hirigana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x30A1",			2, FALSE  );	//first char of Katakana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x30A1\x30A1",	2, FALSE  );	//first char of Katakana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"1",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"01",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"10",				2, FALSE  );
  		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x3041" L"1",		2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"ba",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"!!",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"#A#",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"@"L"\x3088\x30E6",2, FALSE  );	//Tilda then Hirigana then Katakana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x30E8\x3082",	2, FALSE  );	//Katakana then Hirigana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x3082\x30E8",	2, FALSE  );	//Hirigana then Katakana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x3088" L"z",		2, FALSE  );	//Hirigana then English
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"z" L"\x3088",		2, FALSE  );	//English then Hirigana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x30E4" L"X",		2, FALSE  );	//Katakana then English
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"X" L"\x30E4",		2, FALSE  );	//English the Kataka
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"\x3093" L"F",		2, FALSE  );	//Hirigana then English
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"F" L"\x3093",		2, FALSE  );	//English then Hirigana
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"A9",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"B6",				2, FALSE  );
		CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"B8",				2, FALSE  );
	}
}


// This function assumes you have UDATA Mapped to the root of the partition
void CreateMetaVarGames( CMemoryArea* pMemoryArea )
{
    DWORD dwTitleIDStartNo = 0x00008000;

    LARGE_INTEGER liNumber;
    char pszImageDestName[50];
    char pszTitlePath[50];
    char pszSavedGamePath[50];
    
    ZeroMemory( pszImageDestName, 50 );
    ZeroMemory( pszTitlePath, 50 );
    ZeroMemory( pszSavedGamePath, 50 );


    // Create a Title without a localized TITLEMETA.XBX
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"TITLEMETA.XBX not localized", 0, FALSE );

    _snprintf( pszTitlePath, 49, "%c:\\%0.8X", pMemoryArea->GetUDataDrive(), dwTitleIDStartNo - 1 );
    
    pMemoryArea->CreateTitleMetaXbx( pszTitlePath, L"TITLEMETA.XBX not localized", L"", NULL );

    // Create a Title with MAX - 1 (62) char name (no spaces)
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"01234567890123456789012345678901234567890123456789012345678901", 0, FALSE );

    // Create a Title with MAX (63) char name (no spaces)
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"012345678901234567890123456789012345678901234567890123456789012", 0, FALSE );

    // Create a Title with MAX+1 (64) char name (no spaces)
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"0123456789012345678901234567890123456789012345678901234567890123", 0, FALSE );

    // Create a Title with MAX (63) char name (with spaces)
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2", 0, FALSE );

    // Create a Title with MAX (63) char name (big chuncks)
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"012345678901234 678901234567890123456789012345 7890123456789012", 0, FALSE );

    // Create a Title with (0) char name
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"", 0, FALSE );

    // Create a Title with (1) char name
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"1", 0, FALSE );

    // Saved Game saveimage is at a higher level (saved game is missing saveimage.xbx)
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"Def Save Image / No Save Image", 0 );

    /*
    _snprintf( pszTitlePath, 49, "%c:\\%0.8X", pMemoryArea->GetUDataDrive(), dwTitleIDStartNo++ );

    // Create the directory for the Game
    if( !CreateDirectory( pszTitlePath, NULL ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the directory - '%s', Error - '0x%.8X (%d)'", pszTitlePath, GetLastError(), GetLastError() );

    if( FAILED( pMemoryArea->CreateTitleMetaXbx( pszTitlePath, L"Save Image Higher", NULL, NULL ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the titlemeta.xbx file!! TitlePath - '%hs'", pszTitlePath );
    
    if( FAILED( pMemoryArea->CopyGameImages( pszTitlePath, TRUE, TRUE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the copy the saved game images!! TitlePath - '%hs'" );
    */

    if( FAILED( pMemoryArea->MapUDataToTitleID( dwTitleIDStartNo - 1 ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to MapUDataToTitleID!! ID - '%d', Error - '0x%0.8X (%d)'", dwTitleIDStartNo - 1, GetLastError(), GetLastError() );
    }
    else
    {
        if( FAILED( pMemoryArea->CreateSavedGame( L"Missing Save Image", NULL, 0, NULL, NULL, FALSE ) ) )
            XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );
    }

    pMemoryArea->MapUDataToOrigLoc();

    // Saved Game saveimage is at a higher level (saved game also has saveimage.xbx)
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"1", 1 );

    //
    // Saved Game 'Missing' All Images
    //
    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"All Images Missing", 0 );
    pMemoryArea->MapUDataToTitleID( dwTitleIDStartNo - 1 );

    char pszGameImage[MAX_PATH+1];
    char pszDefSaveImage[MAX_PATH+1];

    pszGameImage[MAX_PATH] = '\0';
    pszDefSaveImage[MAX_PATH] = '\0';

    _snprintf( pszGameImage,    MAX_PATH, "%c:\\%hs", pMemoryArea->GetUDataDrive(), MEMORY_AREA_TITLE_IMAGE_FILENAME );
    _snprintf( pszDefSaveImage, MAX_PATH, "%c:\\%hs", pMemoryArea->GetUDataDrive(), MEMORY_AREA_SAVEGAME_IMAGE_FILENAME );

    if( !DeleteFile( pszGameImage ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to delete the file!! File - '%hs', Error - '0x%0.8X (%d)'", pszGameImage, GetLastError(), GetLastError() );
    }

    if( !DeleteFile( pszDefSaveImage ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to delete the file!! File - '%hs', Error - '0x%0.8X (%d)'", pszDefSaveImage, GetLastError(), GetLastError() );
    }

    // saveimage.xbx is missing
    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Image: Missing", NULL, 0, NULL, NULL, FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    pMemoryArea->MapUDataToOrigLoc();

    //
    // Saved Game Varitions
    //
    SYSTEMTIME sysTime;
    ZeroMemory( &sysTime, sizeof( sysTime ) );

    CreateGenGame( pMemoryArea, dwTitleIDStartNo++, L"Saved Game Variations", 0 );
    pMemoryArea->MapUDataToTitleID( dwTitleIDStartNo - 1 );

    // TODO: All meta-data missing

    // Saved Game Time is 12:00am
    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 0; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"Game Time: 12:00am", NULL, 0, &sysTime, "", FALSE );

    // Saved Game Time is 12:00pm
    sysTime.wMonth = 1; sysTime.wDay = 1; sysTime.wYear = 2000; /**/ sysTime.wHour = 12; sysTime.wMinute = 0; sysTime.wSecond = 0;
    pMemoryArea->CreateSavedGame( L"Game Time: 12:00pm", NULL, 0, &sysTime, "", FALSE );

    // saveimage.xbx is missing
    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Image: Missing", NULL, 0, NULL, NULL, FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // saveimage.xbx is normal (64x64)
    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Image: 64x64 (Normal)", NULL, 0, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // saveimage.xbx is 32x32
    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Image: 32x32 (Small)", NULL, 0, NULL, MEMCONFIGS_32X32_IMAGE_PATHINFO, FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // saveimage.xbx is 128x128
    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Image: 128x128 (Larger)", NULL, 0, NULL, MEMCONFIGS_128X128_IMAGE_PATHINFO, FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // saveimage.xbx is 256x256
    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Image: 256x256 (Larger)", NULL, 0, NULL, MEMCONFIGS_256X256_IMAGE_PATHINFO, FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // saveimage.xbx is 1024x1024
    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Image: 1024x1024 (Larger)", NULL, 0, NULL, MEMCONFIGS_1024X1024_IMAGE_PATHINFO, FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // saveimage.xbx is 1 byte
    ZeroMemory( pszSavedGamePath, 50 );
    liNumber.QuadPart = 1;

    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Image: 1 byte", pszSavedGamePath, 49, NULL, NULL, FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // Create a 1 byte image file
    _snprintf( pszImageDestName, 49, "%hs%hs", pszSavedGamePath, MEMORY_AREA_SAVEGAME_IMAGE_FILENAME );
    if( FAILED( pMemoryArea->CreateFileOfSize( pszImageDestName, liNumber ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create a 1 byte image file!!" );
    
    // saveimage.xbx is corrupt
    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Image: Corrupt", NULL, 0, NULL, MEMCONFIGS_CORRUPT_IMAGE_PATHINFO, FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );
    
    // savemeta.xbx is missing
    ZeroMemory( pszSavedGamePath, 50 );

    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Meta: Missing", pszSavedGamePath, 49, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    _snprintf( pszImageDestName, 49, "%hs%hs", pszSavedGamePath, META_FILE_SAVEXBX_FILENAME );
    if( !DeleteFile( pszImageDestName ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to delete the savemeta.xbx file!!" );

    // savemeta.xbx is empty
    ZeroMemory( pszSavedGamePath, 50 );
    liNumber.QuadPart = 0;

    if( FAILED( pMemoryArea->CreateSavedGame( L"Save Meta: Missing", pszSavedGamePath, 49, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    _snprintf( pszImageDestName, 49, "%hs%hs", pszSavedGamePath, META_FILE_SAVEXBX_FILENAME );
    if( FAILED( pMemoryArea->CreateFileOfSize( pszImageDestName, liNumber ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create an empty savemeta.xbx file!!" );

    // Save Name is 0 chars
    if( FAILED( pMemoryArea->CreateSavedGame( L"", NULL, 0, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // Save Name is 1 char
    if( FAILED( pMemoryArea->CreateSavedGame( L"1", NULL, 0, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // Save Name is MAX - 1 (62) chars
    if( FAILED( pMemoryArea->CreateSavedGame( L"01234567890123456789012345678901234567890123456789012345678901", NULL, 0, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // Save Name is MAX (63) chars
    if( FAILED( pMemoryArea->CreateSavedGame( L"012345678901234567890123456789012345678901234567890123456789012", NULL, 0, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // Save Name is MAX + 1 (64) chars
    if( FAILED( pMemoryArea->CreateSavedGame( L"0123456789012345678901234567890123456789012345678901234567890123", NULL, 0, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // BUGBUG: These will RIP
    /*
    // Save Name is LARGE (1023 chars)
    if( FAILED( pMemoryArea->CreateSavedGame( L"1023-chars0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012", NULL, 0, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // Save Name is HUGE (1024+ chars)
    if( FAILED( pMemoryArea->CreateSavedGame( L"1024-chars0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                            L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", 
                                            NULL, 0, NULL, "", FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );
    */

    // 1 block Saved Game
    if( FAILED( pMemoryArea->CreateSavedGame( L"Saved Game: 1 block", NULL, 0, NULL, NULL, FALSE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // NOCOPY=1 Saved Game
    if( FAILED( pMemoryArea->CreateSavedGame( L"Saved Game: NoCopy=1", NULL, 0, NULL, "", TRUE ) ) )
        XDBGWRN( APP_TITLE_NAME_A, "CreateMetaVarGames():Failed to create the saved game!!" );

    // TODO: NOCOPY=0 Saved Game
    // TODO: NOCOPY=2 Saved Game
    // TODO: NOCOPY=LARGE (1023 chars)
    // TODO: NOCOPY=HUGE (1024+ chars)

    pMemoryArea->MapUDataToOrigLoc();
}


void CreateGamesAndSaves( CMemoryUnit* pMemoryUnit, unsigned int uiNumGames, unsigned int uiNumSaved )
{
    // Create the Game Titles
    WCHAR pwszSaveGameName[20];
    char pszGameTitle[9];
    
    ZeroMemory( pwszSaveGameName, sizeof( WCHAR ) * 20 );
    pszGameTitle[8] = '\0';
    DWORD dwXModifier = 0;
    DWORD dwNumTries = 0;
    for( unsigned int x = 0; x < uiNumGames; x++ )
    {
        sprintf( pszGameTitle, "%0.8X", x + dwXModifier );
        
        while( FAILED( pMemoryUnit->CreateGameTitle( pMemoryUnit->GetUDataDrive(), pszGameTitle ) ) )
        {
            ++dwXModifier;
            sprintf( pszGameTitle, "%0.8X", x + dwXModifier );

            if( ( ERROR_DISK_FULL == GetLastError() ) || ( dwNumTries++ == 100 ) )
            {
                // Restore the MU to it's orignal Mount location
                pMemoryUnit->Unmount();
                pMemoryUnit->Mount();

                return;
            }
        }

        // Mount the MU to our Game Title directory
        pMemoryUnit->Unmount();
        if( SUCCEEDED( pMemoryUnit->MapUDataToTitleID( pszGameTitle ) ) )
        {
            // Create the Saved Games for the Title
            for( unsigned int y = 0; y < uiNumSaved; y++ )
            {
                _snwprintf( pwszSaveGameName, 19, L"Save Game #%03d", y );
                if( FAILED( pMemoryUnit->CreateSavedGame( pwszSaveGameName, NULL, 0, NULL, "", FALSE ) ) )
                {
                    // Full

                    // Restore the MU to it's orignal Mount location
                    pMemoryUnit->Unmount();
                    pMemoryUnit->Mount();

                    return;
                }
            }
        }

        // Restore the MU to it's orignal Mount location
        pMemoryUnit->Unmount();
        pMemoryUnit->Mount();
    }
}