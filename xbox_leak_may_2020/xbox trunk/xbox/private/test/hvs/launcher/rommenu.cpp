/*****************************************************
*** rommenu.cpp
***
*** CPP file for our ROM Menu class.
*** This menu will allow the user to select a ROM and
*** flash it to their Xbox.
***
*** by James N. Helm
*** January 21st, 2002
***
*****************************************************/

#include "stdafx.h"
#include "rommenu.h"

extern PVOID KernelRomBase;

// Constructors and Destructor
CROMMenu::CROMMenu( void ) :
m_eCurrentAction( ENUM_ROMMENU_NOACTION ),
m_pucROMImage( NULL )
{
}

CROMMenu::~CROMMenu( void )
{
    if( NULL != m_pucROMImage )
    {
        LocalFree( m_pucROMImage );
        m_pucROMImage = NULL;
    }
}

// Initialize the Menu
HRESULT CROMMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CROMMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"ROM Flash Menu" );

    // Add screen items here
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );

    // Status Bar Column 1
    AddTextItem( GetButtonFont(), L"A",             gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_A_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Flash ROM",    gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"B",             gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_B_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Back",         gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

	
	// Add menu items here -- Find all the roms and list them
    WIN32_FIND_DATA wfd;
    ZeroMemory( &wfd, sizeof( wfd ) );

    HANDLE hRomFiles = FindFirstFile( "d:\\roms\\*.bin", &wfd );
    if( INVALID_HANDLE_VALUE != hRomFiles )
    {
        WCHAR pwszFileName[MAX_PATH+1];
        pwszFileName[MAX_PATH] = L'\0';

        do
        {
            // Add the file name to our menu
            swprintf( pwszFileName, L"%hs", wfd.cFileName );

            AddMenuItem( pMenuItemFont, pwszFileName, FALSE, TRUE, 0 );
        } while( FindNextFile( hRomFiles, &wfd ) );

        FindClose( hRomFiles );
        hRomFiles = INVALID_HANDLE_VALUE;
    }

    return hr;
}

// Renders to the texture and calls any per-frame processing
void CROMMenu::Action( CUDTexture* pTexture )
{
    // Make sure we handle any user initiated actions
    ProcessMenuActions();

    if( ( m_eCurrentAction != ENUM_ROMMENU_NOACTION ) && ( m_eCurrentAction != ENUM_ROMMENU_FLASHINGROM_MESSAGE ) )
        return;

    // Call the base class function
    CMenuScreen::Action( pTexture );
}

// Render the selector for our menu
void CROMMenu::RenderSelector( CUDTexture* pTexture )
{
    if( ENUM_ROMMENU_FLASHINGROM_MESSAGE == m_eCurrentAction )
        return;
    else
        CMenuScreen::RenderSelector( pTexture );
}

// Render the menu items properly on our menu
void CROMMenu::RenderMenuItems( CUDTexture* pTexture )
{
    if( 0 == GetNumMenuItems() )
    {
        // Display Message Here
        float fXPos = gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), gc_pwszROMMENU_NO_ROMS_MESSAGE ) / 2 );
        float fYPos = gc_fROMMENU_TEXT_YPOS;

        pTexture->DrawText( GetBodyFont(), fXPos, fYPos, m_dwMenuItemEnabledFGColor, m_dwMenuItemEnabledBGColor, L"%ls", gc_pwszROMMENU_NO_ROMS_MESSAGE );
    }
    else if( ENUM_ROMMENU_FLASHINGROM_MESSAGE == m_eCurrentAction )
    {
        // Display Message Here
        float fXPos = 0.0f;
        float fYPos = gc_fROMMENU_TEXT_YPOS;

        for( unsigned int i = 0; i < ARRAYSIZE(gc_pwszROMMENU_FLASHING_ROM_MESSAGE); i++ )
        {
            fXPos = gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), (WCHAR*)gc_pwszROMMENU_FLASHING_ROM_MESSAGE[i] ) / 2 );
            fYPos = gc_fROMMENU_TEXT_YPOS + ( ( m_fMenuItemVertSpace + m_uiMenuItemFontHeight ) * i );

            pTexture->DrawText( GetBodyFont(), fXPos, fYPos, m_dwMenuItemEnabledFGColor, m_dwMenuItemEnabledBGColor, L"%ls", gc_pwszROMMENU_FLASHING_ROM_MESSAGE[i] );
        }
    }
    else
    {
        CMenuScreen::RenderMenuItems( pTexture );
    }
}

// Handles input (of the BUTTONS) for the current menu
void CROMMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, let's bail
    if( !bFirstPress )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            WCHAR pwszMessage[MAX_PATH+1];  // Used to display various messages to the user
            pwszMessage[MAX_PATH] = L'\0';

            // Load the ROM image in to memory
            char pszROMFileName[MAX_PATH+1];
            pszROMFileName[MAX_PATH] = '\0';

            _snprintf( pszROMFileName, MAX_PATH, "d:\\roms\\%ls", m_MenuItems.GetNode( GetSelectedItem() )->m_pwszItemName );

            if( FAILED( LoadROMImage( pszROMFileName ) ) )
            {
                DebugPrint( "CROMMenu::HandleInput():Unable to load the ROM image!! - '%hs'\n", pszROMFileName );

                // Display a failure Message to the user
                _snwprintf( pwszMessage, MAX_PATH, L"ROM load was unsuccessful!!\n\nThe ROM that was attempted:\n%ls", m_MenuItems.GetNode( GetSelectedItem() )->m_pwszItemName );

				GetMessagePtr()->SetBackPtr( this );
                GetMessagePtr()->SetText( pwszMessage );
                Leave( GetMessagePtr() );
            }
            else
            {
                // Check the ROM Version so that we can display the proper message
				BYTE byImageVer, byXboxROMVer;
                if( !CheckROMVersion( &byImageVer, &byXboxROMVer ) )
                    _snwprintf( pwszMessage, MAX_PATH, L"Image ROM version (0x%02X)  Xbox ROM version (0x%02X)\n/c%ld %ld VERSIONS DO NOT MATCH!!\n\nAre you sure you wish to flash the ROM with\n/c%ld %ld %ls?", byImageVer, byXboxROMVer, gc_dwCOLOR_TVSAFE_RED, m_dwMenuItemEnabledBGColor, gc_dwCOLOR_TEXT_YELLOW, m_dwMenuItemEnabledBGColor, m_MenuItems.GetNode( GetSelectedItem() )->m_pwszItemName );
                else
                    _snwprintf( pwszMessage, MAX_PATH, L"Image ROM version (0x%02X)  Xbox ROM version (0x%02X)\nVersions Match\n\nAre you sure you wish to flash the ROM with\n/c%ld %ld %ls?", byImageVer, byXboxROMVer, gc_dwCOLOR_TEXT_YELLOW, m_dwMenuItemEnabledBGColor, m_MenuItems.GetNode( GetSelectedItem() )->m_pwszItemName );

                // Go get the users permission (YES / NO menu)
                m_eCurrentAction = ENUM_ROMMENU_FLASHCONFIG_YESNO;

                GetYesNoPtr()->SetBackPtr( this );
                GetYesNoPtr()->SetMenuTitle( GetBodyFont(), L"ROM Flash" );

                GetYesNoPtr()->SetText( pwszMessage );

                Leave( GetYesNoPtr() );
            }

            break;
        }
    case BUTTON_B:
        {
            GoBack();

            break;
        }
    }
}


// If the user is performing an action, this will handle it
void CROMMenu::ProcessMenuActions( void )
{
    switch( m_eCurrentAction )
    {
    case ENUM_ROMMENU_FLASHCONFIG_YESNO:
        {
            if( !GetYesNoPtr()->GetCancelled() )
            {
                // Get the Configuration Name
                m_eCurrentAction = ENUM_ROMMENU_FLASHINGROM_MESSAGE;

            }
            else
            {
                m_eCurrentAction = ENUM_ROMMENU_NOACTION;  // Make sure we set this to no action

                // Unload the loaded ROM Image
                if( NULL != m_pucROMImage )
                {
                    LocalFree( m_pucROMImage );
                    m_pucROMImage = NULL;
                }
            }

            break;
        }
    case ENUM_ROMMENU_FLASHINGROM_MESSAGE:
        {
            // Make sure we set this to our program action
            m_eCurrentAction = ENUM_ROMMENU_FLASHINGROM_PROGRAM;
            UpdateTexture();
            
            break;
        }
    case ENUM_ROMMENU_FLASHINGROM_PROGRAM:
        {
            // Make sure we set this to no action
            m_eCurrentAction = ENUM_ROMMENU_NOACTION;

            // Flash the ROM
            HRESULT hFlashResult = FlashROMImage();

            // Unload the loaded ROM Image
            if( NULL != m_pucROMImage )
            {
                LocalFree( m_pucROMImage );
                m_pucROMImage = NULL;
            }

            GetMessagePtr()->SetBackPtr( this );

            WCHAR pwszMessage[MAX_PATH+1];
            pwszMessage[MAX_PATH] = L'\0';
            
            if( FAILED( hFlashResult ) )
                _snwprintf( pwszMessage, MAX_PATH, L"ROM flash was unsuccessful!!\nThe ROM that was attempted:\n%ls", m_MenuItems.GetNode( GetSelectedItem() )->m_pwszItemName );
            else
                _snwprintf( pwszMessage, MAX_PATH, L"ROM flashed successfully!!\nThe ROM that was flashed:\n%ls\nYou should cold reboot for the ROM to take affect.", m_MenuItems.GetNode( GetSelectedItem() )->m_pwszItemName );

            GetMessagePtr()->SetText( pwszMessage );
            Leave( GetMessagePtr() );

            break;
        }
    }
}


// Load the ROM image in to memory
HRESULT CROMMenu::LoadROMImage( char* pszROMFileName )
{
    //
    // Load the ROM image
    //

    if( NULL != m_pucROMImage )
    {
        LocalFree( m_pucROMImage );
        m_pucROMImage = NULL;
    }

    m_pucROMImage = (PUCHAR) LocalAlloc( LMEM_FIXED, ROM_SIZE );
    if( NULL == m_pucROMImage )
    {
        DebugPrint( "CROMMenu::LoadROMImage():Could not allocate ROM image memory!!\n" );

        return E_OUTOFMEMORY;
    }

    //
    // Open the ROM Image file
    //
    HANDLE hROMFileHandle = CreateFile( pszROMFileName,
                                        GENERIC_READ,
                                        0,
                                        NULL,
                                        OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL );

    if( INVALID_HANDLE_VALUE == hROMFileHandle )
    {
        DebugPrint( "CROMMenu::LoadROMImage():Could not open the ROM file!! - '%hs'\n", pszROMFileName );

        LocalFree( m_pucROMImage );
        m_pucROMImage = NULL;

        return E_FAIL;
    }

    //
    // Read in the ROM
    //

    ULONG ulSizeOfDataReturned = 0;
    PUCHAR pucROMData = m_pucROMImage;
    while( ReadFile( hROMFileHandle, pucROMData, ROM_SIZE - ( pucROMData - m_pucROMImage ), &ulSizeOfDataReturned, NULL)
           && ( 0 != ulSizeOfDataReturned ) )
    {
        pucROMData += ulSizeOfDataReturned;
    }
    
    CloseHandle( hROMFileHandle );
    hROMFileHandle = INVALID_HANDLE_VALUE;

    if( ROM_SIZE != ( pucROMData - m_pucROMImage ) )
    {
        DebugPrint( "CROMMenu::LoadROMImage():Could not read ROM image file, read %d out of %d bytes!!\n", ( pucROMData - m_pucROMImage ), ROM_SIZE );

        LocalFree( m_pucROMImage );
        m_pucROMImage = NULL;

        return E_FAIL;
    }

    return S_OK;
}


// Flash the ROM image to the Xbox
HRESULT CROMMenu::FlashROMImage( void )
{
    // Check to make sure we have loaded a ROM image before attempting the flash
    if( NULL == m_pucROMImage )
    {
        DebugPrint( "CROMMenu::FlashROMImage():You need to load a ROM image first!!\n" );
        
        return E_FAIL;
    }

    //
    // Flash the ROM
    //
    DebugPrint( "CROMMenu::FlashROMImage():Starting ROM Flash...\n" );

    HRESULT hrFlash = S_OK;

    for( unsigned int i = 0; i < ROM_FLASH_RETRIES; i++ )
    {
        CHAR szResp[128];
        hrFlash = FlashKernelImage( m_pucROMImage,
                                    ROM_SIZE,
                                    szResp,
                                    ARRAYSIZE( szResp ) );

        DebugPrint( "CROMMenu::FlashROMImage():Flash Status (%d): hr=0x%08x (%hs)\n", i, hrFlash, szResp );

        if( SUCCEEDED( hrFlash ) )
            break;
    }

    if( ROM_FLASH_RETRIES == i )
    {
        DebugPrint( "CROMMenu::FlashROMImage():Could not flash ROM, hr=0x%08x\n", hrFlash );

        return hrFlash;
    }


    return S_OK;
}


// Check to make sure the version of the ROM matches the Xbox
BOOL CROMMenu::CheckROMVersion( BYTE* pbyImageVer/*=NULL*/, BYTE* pbyXboxROMVer/*=NULL*/ )
{
    // Check to make sure we have loaded a ROM image before attempting to check the version
    if( NULL == m_pucROMImage )
    {
        DebugPrint( "CROMMenu::CheckROMVersion():You need to load a ROM image first!!\n" );

        return FALSE;
    }

    //
    // Map top 1MB of physical memory of ROM region (FFF00000-FFFFFFFF)
    //

    KernelRomBase = MmMapIoSpace( FLASH_BASE_ADDRESS, FLASH_REGION_SIZE,
                                  PAGE_READWRITE | PAGE_NOCACHE );

    if ( NULL == KernelRomBase )
    {
        DebugPrint( "CROMMenu::CheckROMVersion():Unable to map i/o space!!\n" );

        return FALSE;
    }

    BYTE byXboxROMVersion = FlashReadByte( FLASH_BASE_ADDRESS + ROM_VERSION_BYTE_OFFSET );
    BYTE byImageVersion = ((PBYTE)m_pucROMImage)[ROM_VERSION_BYTE_OFFSET];

    DebugPrint( "CheckROMVersion():XBOX ROM Ver: 0x%02X, Image ROM Ver 0x%02X\n", byXboxROMVersion, byImageVersion );

    // Unmap I/O Space
    MmUnmapIoSpace( KernelRomBase, FLASH_REGION_SIZE );
    KernelRomBase = NULL;

	//
	// Pass back the proper values if the user wants them
	//
	if( NULL != pbyImageVer )
		*pbyImageVer = byImageVersion;

	if( NULL != pbyXboxROMVer )
		*pbyXboxROMVer = byXboxROMVersion;

	//
	// Return whether the versions match (TRUE), or do not match (FALSE)
	//
    if( byXboxROMVersion != byImageVersion )
        return FALSE;

    return TRUE;
}
