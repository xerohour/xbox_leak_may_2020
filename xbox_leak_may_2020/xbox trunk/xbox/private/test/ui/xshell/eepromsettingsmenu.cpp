/*****************************************************
*** eepromsettingsmenu.cpp
***
*** CPP file for our XShell EEPROM settings menu 
*** class.  This menu will display all of the
*** EEPROM settings and allow the user to scroll
*** through this list.
***
*** by James N. Helm
*** June 6th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "eepromsettingsmenu.h"

void ArrayToHexString( OUT char* pszBuffer, IN ULONG cbBufferSize, IN BYTE* achArray, IN ULONG cbArraySize );

extern CXSettings           g_XboxSettings;         // Settings that are stored on the Xbox
extern CSoundCollection     g_Sounds;               // Used to generate all sounds for the XShell
extern BOOL                 g_bDisplayUpArrow;      // Used to determine if we should render the Up Arrow
extern BOOL                 g_bDisplayDownArrow;    // Used to determine if we should render the Down Arrow
extern XFONT*               g_pFont;

// Constructors
CEEPROMSettingsMenu::CEEPROMSettingsMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_uiTopItemIndex( 0 )
{
    XDBGTRC( APP_TITLE_NAME_A, "CEEPROMSettingsMenu::CEEPROMSettingsMenu(ex)" );

    m_uiNumItems = EEPROMMENU_NUM_ITEMS;
    SetSelectedItem( 0 );

    for( unsigned int x = 0; x < EEPROMMENU_NUM_ITEMS; x++ )
    {
        m_pawszMenuItems[x] = new WCHAR[MAX_PATH+1];
        ZeroMemory( m_pawszMenuItems[x], sizeof( WCHAR ) * MAX_PATH+1 );
    }

    SetTitle( MENU_TITLE_EEPROM );
}


// Destructor
CEEPROMSettingsMenu::~CEEPROMSettingsMenu()
{
    // Clean up memory
    for( unsigned int x = 0; x < EEPROMMENU_NUM_ITEMS; x++ )
    {
        if( m_pawszMenuItems[x] )
        {
            delete[] m_pawszMenuItems[x];
            m_pawszMenuItems[x] = NULL;
        }
    }    
}


// Draws a menu on to the screen
void CEEPROMSettingsMenu::Action( CUDTexture* pTexture )
{
    g_XboxSettings.LoadSettings();
    if( g_XboxSettings.SettingsHaveChanged() )
    {
        m_bUpdateTexture = TRUE;
    }

    // Only update the texture if we need to, otherwise return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else
    {
        m_bUpdateTexture = FALSE;
    }

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Update our Font Height
    XFONT_SetTextHeight( g_pFont, ITEM_OPTIONMENU_FONT_HEIGHT );
    m_uiFontHeightOfItem = ITEM_OPTIONMENU_FONT_HEIGHT;

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

    //
    // Populate our Menu Item WCHAR array
    //
    unsigned int uiIndex = 0;
    UCHAR pszOutBuffer[MAX_PATH+1];
    UCHAR pszBuffer[MAX_PATH+1];
    WCHAR pwszBuffer[MAX_PATH+1];

    // New Global Checksum
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"XDK EEPROM Checksum:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetNewGlobalChecksum() );

    // New Factory Checksum
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"XDK Factory Checksum:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetNewFactoryChecksum() );

    // New User Checksum
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"XDK User Checksum:");
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetNewUserChecksum() );

    // New Encrypted Checksum
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"XDK Encrypted Checksum:");
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetNewEncryptedChecksum() );

#ifdef _DEBUG   // If you remove this, you MUST also remove it from CONSTANTS.H, or XShell will crash

    // Factory Checksum
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Factory Checksum:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetFactoryChecksum() );

    // Factory SerialNumber
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    ZeroMemory( pszOutBuffer, MAX_PATH+1 );
    g_XboxSettings.GetFactorySerialNumber( pszBuffer, MAX_PATH );
    ArrayToHexString( (char*)pszOutBuffer, MAX_PATH, (BYTE*)pszBuffer, SETTINGS_FACTORY_SERIAL_NUM_SIZE );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Factory Serial Number:");
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %hs", pszOutBuffer );

    // Factory EthernetAddr
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    ZeroMemory( pszOutBuffer, MAX_PATH+1 );
    g_XboxSettings.GetFactoryEthernetAddr( pszBuffer, MAX_PATH );
    ArrayToHexString( (char*)pszOutBuffer, MAX_PATH, (BYTE*)pszBuffer, SETTINGS_FACTORY_ETHERNET_SIZE );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Factory Ethernet Addr:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %hs", pszOutBuffer );

    // Factory Reserved1
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    ZeroMemory( pszOutBuffer, MAX_PATH+1 );
    g_XboxSettings.GetFactoryReserved1( pszBuffer, MAX_PATH );
    ArrayToHexString( (char*)pszOutBuffer, MAX_PATH, (BYTE*)pszBuffer, SETTINGS_FACTORY_RESERVED1_SIZE );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Factory Reserved1:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %hs", pszOutBuffer );

    // Factory OnlineKey
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    ZeroMemory( pszOutBuffer, MAX_PATH+1 );
    g_XboxSettings.GetFactoryOnlineKey( pszBuffer, MAX_PATH );
    ArrayToHexString( (char*)pszOutBuffer, MAX_PATH, (BYTE*)pszBuffer, SETTINGS_FACTORY_ONLINEKEY_SIZE );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Factory Online Key:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %hs", pszOutBuffer );

    // Factory AVRegion
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Factory AV Region:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetFactoryAVRegion() );

    // Factory Reserved2
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Factory Reserved2:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetFactoryReserved2() );

    // Spacers
    //_snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"" );
    //_snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"" );

    // User Checksum
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Checksum:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserChecksum() );

    // User Time Zone Bias
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Time Zone Bias:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserTimeZoneBias() );

    // User Time Zone Std Name
    ZeroMemory( pwszBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );
    g_XboxSettings.GetUserTimeZoneStdName( pwszBuffer, MAX_PATH );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Time Zone Std Name:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %ls", pwszBuffer );

    // User Time Zone Dlt Name
    ZeroMemory( pwszBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );
    g_XboxSettings.GetUserTimeZoneDltName( pwszBuffer, MAX_PATH );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Time Zone Dlt Name:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %ls", pwszBuffer );

    // User Time Zone Std Date
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    XBOX_TIMEZONE_DATE* tzdate = (XBOX_TIMEZONE_DATE*) pszBuffer;
    g_XboxSettings.GetUserTimeZoneStdDate( tzdate, MAX_PATH );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Time Zone Std Date: ");
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %.2d/%.2d/%.2d/%.2d", tzdate->Month, tzdate->Day, tzdate->DayOfWeek, tzdate->Hour);

    // User Time Zone Dlt Date
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    g_XboxSettings.GetUserTimeZoneDltDate( tzdate, MAX_PATH );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Time Zone Dlt Date: ");
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %.2d/%.2d/%.2d/%.2d", tzdate->Month, tzdate->Day, tzdate->DayOfWeek, tzdate->Hour);

    // User Reserved
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    ZeroMemory( pszOutBuffer, MAX_PATH+1 );
    g_XboxSettings.GetUserReserved( (ULONG*)pszBuffer, MAX_PATH );
    ArrayToHexString( (char*)pszOutBuffer, MAX_PATH, (BYTE*)pszBuffer, SETTINGS_USER_RESERVED_SIZE * sizeof( ULONG ) );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Reserved:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %hs", pszOutBuffer );

    // User Time Zone Std Bias
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Time Zone Std Bias:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserTimeZoneStdBias() );

    // User Time Zone Dlt Bias
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Time Zone Dlt Bias:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserTimeZoneDltBias() );

    // User Language
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Language:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserLanguage() );

    // User Video Flags
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Video Flags:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserVideoFlags() );

    // User Audio Flags
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Audio Flags:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserAudioFlags() );

    // User Parental Control Games
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Parental Control Games:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserParentalControlGames() );

    // User Parental Control Password
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Parental Control Password:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserParentalControlPassword() );

    // User Parental Control Movies
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Parental Control Movies:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserParentalControlMovies() );

    // User Online IP Address
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Online IP Address:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserOnlineIpAddress() );

    // User Online DNS Address
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Online DNS Address:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserOnlineDnsAddress() );

    // User Online Default Gateway Address
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Online Default Gateway Address:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserOnlineDefaultGatewayAddress() );

    // User Online Subnet Mask
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Online Subnet Mask:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserOnlineSubnetMask() );

    // User Misc Flags
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User Misc Flags:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserMiscFlags() );

    // User DVD Region
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"User DVD Region:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetUserDvdRegion() );

    // Encrypted Checksum
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    ZeroMemory( pszOutBuffer, MAX_PATH+1 );
    g_XboxSettings.GetEncChecksum( pszBuffer, MAX_PATH );
    ArrayToHexString( (char*)pszOutBuffer, MAX_PATH, (BYTE*)pszBuffer, SETTINGS_ENC_CHECKSUM_SIZE );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Encrypted Checksum:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %hs", pszOutBuffer );

    // Encrypted Confounder
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    ZeroMemory( pszOutBuffer, MAX_PATH+1 );
    g_XboxSettings.GetEncConfounder( pszBuffer, MAX_PATH );
    ArrayToHexString( (char*)pszOutBuffer, MAX_PATH, (BYTE*)pszBuffer, SETTINGS_ENC_CONFOUNDER_SIZE );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Encrypted Confounder:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %hs", pszOutBuffer );

    // Encrypted HD Key
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    ZeroMemory( pszOutBuffer, MAX_PATH+1 );
    g_XboxSettings.GetEncHDKey( pszBuffer, MAX_PATH );
    ArrayToHexString( (char*)pszOutBuffer, MAX_PATH, (BYTE*)pszBuffer, SETTINGS_ENC_HDKEY_SIZE );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Encrypted HD Key:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  %hs", pszOutBuffer );

    // Encrypted Game Region
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"Encrypted Game Region:" );
    _snwprintf( m_pawszMenuItems[uiIndex++], MAX_PATH, L"  0x%.8X", g_XboxSettings.GetEncGameRegion() );
#endif // _DEBUG

    ASSERT( EEPROMMENU_NUM_ITEMS == uiIndex );

    //
    // Draw our Menu Items
    //

    for( unsigned int x = m_uiTopItemIndex; x < ( EEPROMMENU_NUM_ITEMS_TO_DISPLAY + m_uiTopItemIndex ); x++ )
    {
        // Make sure we don't try to draw too many items
        if( x == GetNumItems() )
        {
            break;
        }

        float XPos = MENUBOX_TEXTAREA_X1;
        float YPos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( x - m_uiTopItemIndex ) );

        DWORD dwColor;
        if( ( x % 2 ) == 0 )
        {
            dwColor = SCREEN_DEFAULT_TEXT_FG_COLOR;
        }
        else
        {
            dwColor = COLOR_DARK_GREEN2;
        }

        pTexture->DrawText( XPos, YPos, dwColor, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", m_pawszMenuItems[x] );
    }

    // Correct our Font Height
    m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    // Unlock the texture
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CEEPROMSettingsMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
{
    switch( joystick )
    {
    // This is the LEFT Thumb Joystick on the controller
    case JOYSTICK_LEFT:
        {
            // Y
            if( ( nThumbY < 0 ) && ( abs( nThumbY ) > m_nJoystickDeadZone ) ) // Move the left joystick down
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_DOWN, bFirstYPress );
            }
            else if( ( nThumbY > 0 ) && ( abs( nThumbY ) > m_nJoystickDeadZone ) )// Move left joystick up
            {
                // Hand off this to the "DPad" handler
                HandleInput( CONTROL_DPAD_UP, bFirstYPress );
            }

            // X
            if( ( nThumbX < 0 ) && ( abs( nThumbX ) > m_nJoystickDeadZone ) ) // Move the left joystick left
            {
            }
            else if( ( nThumbX > 0 ) && ( abs( nThumbX ) > m_nJoystickDeadZone ) )// Move the left joystick right
            {
            }
            break;
        }
    }
}


// Handles input (of the CONTROLS) for the current menu
void CEEPROMSettingsMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // If this is a repeat button press, let's delay a bit
    if( bFirstPress )
    {
        m_keyPressDelayTimer = GetTickCount();
        m_keyPressDelayInterval = KEY_PRESS_INITIAL_DELAY;
    }
    else // Check to see if the repeat press is within our timer, otherwise bail
    {
        // If the interval is too small, bail
        if( ( GetTickCount() - m_keyPressDelayTimer ) < m_keyPressDelayInterval )
        {
            return;
        }
        m_keyPressDelayTimer = GetTickCount();
        m_keyPressDelayInterval = KEY_PRESS_REPEAT_DELAY;
    }

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            if( m_uiTopItemIndex != 0 )
            {
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

                m_uiTopItemIndex -= EEPROMMENU_NUM_ITEMS_TO_DISPLAY;

                AdjustDisplayArrows();
            }

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( m_uiTopItemIndex < ( GetNumItems() - EEPROMMENU_NUM_ITEMS_TO_DISPLAY ) )
            {
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

                m_uiTopItemIndex += EEPROMMENU_NUM_ITEMS_TO_DISPLAY;

                AdjustDisplayArrows();
            }

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CEEPROMSettingsMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, or input is disabled, let's bail
    if( !bFirstPress )
    {
        return;
    }

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Go Back one menu
                if( GetParent() )
                {
                    Leave( GetParent() );
                }
            }

            break;
        }
    case BUTTON_B:
        {
            // Go Back one menu
            if( GetParent() )
            {
                // Play the Back Sound
                g_Sounds.PlayBackSound();

                Leave( GetParent() );
            }

            break;
        }
    }
}


// Initialize the Menu
HRESULT CEEPROMSettingsMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CEEPROMSettingsMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( menuFileName );

    return hr;
}


// Should be called whenever a user leaves the menu
void CEEPROMSettingsMenu::Leave( CXItem* pItem )
{
    // Call our base class Leave
    CMenuScreen::Leave( pItem );

    g_bDisplayUpArrow = FALSE;
    g_bDisplayDownArrow = FALSE;
}


// This will be called whenever the user enters this menu
void CEEPROMSettingsMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Set our selected item to the top of the menu
    // SetSelectedItem( 0 );

    // Make sure the proper arrows are displayed
    AdjustDisplayArrows();
}


// Adjust the UP and DOWN arrows on the screen
void CEEPROMSettingsMenu::AdjustDisplayArrows()
{
    // Decide if we need to turn on the Up Arrow
    if( 0 == m_uiTopItemIndex )
    //
    // We can turn off the Up Arrow, because there is nothing above us
    //
    {
        g_bDisplayUpArrow = FALSE;
    }
    else
    //
    // We have to turn on the Up Arrow, because a menu choice is above us
    //
    {
        g_bDisplayUpArrow = TRUE;
    }

    // Decide if we need to turn on the Down Arrow
    if( ( m_uiTopItemIndex + EEPROMMENU_NUM_ITEMS_TO_DISPLAY ) == EEPROMMENU_NUM_ITEMS )
    //
    // We can turn off the Down arrow, because nothing is below us
    //
    {
        g_bDisplayDownArrow = FALSE;
    }
    else
    //
    // We have to turn on the Down Arrow, because there are menu choices below us
    //
    {
        g_bDisplayDownArrow = TRUE;
    }
}


// Takes a arrach of characters and converts them to a char* of hex values
void ArrayToHexString( OUT char* pszBuffer, IN ULONG cbBufferSize, IN BYTE* achArray, IN ULONG cbArraySize )
{
    if( ( NULL == pszBuffer ) || ( NULL == achArray ) )
    {
        return;
    }

    ASSERT( cbBufferSize >= ( ( cbArraySize * 2 ) + 1 ) );

    unsigned int iIndex = 0;
    unsigned int iNumChars = cbArraySize * 2;
    for ( unsigned int i = 0; i < iNumChars; i+=2 )
    {
        sprintf( pszBuffer + i, "%02X", achArray[iIndex++] );
    }

    pszBuffer[iNumChars+1] = '\0';
}
