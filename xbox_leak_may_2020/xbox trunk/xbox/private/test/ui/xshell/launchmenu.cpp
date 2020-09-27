/*****************************************************
*** launchmenu.cpp
***
*** CPP file for our XShell Launch menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** December 2nd, 2000
***
*****************************************************/

#include "stdafx.h"
#include "launchmenu.h"

#include "..\..\..\ntos\inc\xlaunch.h"

struct MYTEXVERTEX
{
    D3DXVECTOR3 v;
    float       fRHW;
    D3DCOLOR    diffuse;
    float       tu, tv;    // The texture coordinates
};

#define D3DFVF_MYTEXVERTEX  ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

extern BOOL                 g_bDisplayBButton;      // Used to determine if we should render the B Button
extern BOOL                 g_bUpdateTitleTexture;  // Update the Title Texture
extern CSoundCollection     g_Sounds;               // Used to generate all sounds for the XShell
extern USBManager           Controllers;            // Used to determine what buttons / direction the user is pressing
extern CXSettings           g_XboxSettings;         // Settings that are stored on the Xbox
extern char                 g_pszKeyCombo[5];       // Used to determine the last 4 keys the user has entered
extern unsigned int         g_uiKeyPos;             // Used to track which keypress the user is on


extern IDirect3DDevice8*    g_pD3DDevice;

extern XFONT*               g_pFont;

// Constructors
CLaunchMenu::CLaunchMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_OptionsMenu( NULL ),
m_OnlineMenu( NULL ),
m_SysInfoMenu( NULL ),
m_uiPrevNumItems( 0 ),
m_bCheckedConfig( FALSE )
{
    XDBGTRC( APP_TITLE_NAME_A, "CLaunchMenu::CLaunchMenu( CXItem )" );

    ZeroMemory( m_pszSecretKeyCombo, 5 );
    ComputeSecretKey( m_pszSecretKeyCombo );

    XDBGWRN( APP_TITLE_NAME_A, "CLaunchMenu::CLaunchMenu( CXItem )m_pszSecretKeyCombo - '%s'", m_pszSecretKeyCombo );
}

// Destructor
CLaunchMenu::~CLaunchMenu()
{
}

// Initialize the Menu
HRESULT CLaunchMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CLaunchMenu::Init()" );

    CMenuScreen::Init( menuFileName );

    // Initialize and Start our directory monitor
    m_XBEDirMonitor.Init();
    m_XBEDirMonitor.Run();

    // Obtain a pointer to the correct XBE List object from our thread
    m_pXBEList = m_XBEDirMonitor.GetXBEListPtr();

    XDBGTRC( APP_TITLE_NAME_A, "CLaunchMenu::Init():GetNumItems() - '%d'", GetNumItems() );

    m_uiPrevNumItems = GetNumItems();

    if( 0 == GetNumItems() )
    {
        SetInitialSelectState();
    }
    else
    {
        SetSelectedItem( 0 );
        SetSelectorPos( 0 );
        SetTopIndexOfItem( 0 );

        AdjustDisplayArrows();
    }

    // Get our screen information
    ImportScreenItems( menuFileName );

    // Get the height of the font
    unsigned int decent;
    GetFontSize( &m_uiFontHeightOfItem, &decent );

    // Setup our sub-menus
    m_OptionsMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\options.mnu" );
    m_OptionsMenu.SetParent( this );

    m_OnlineMenu.Init( FILE_DATA_MENU_DIRECTORY_A "\\online.mnu" );
    m_OnlineMenu.SetParent( this );

    m_SysInfoMenu.Init( SYSINFOMENU_FILENAME_A );
    m_SysInfoMenu.SetParent( this );

    return S_OK;
}


// Draws a menu on to the screen
void CLaunchMenu::Action( CUDTexture* pTexture )
{
    if( m_pXBEList != m_XBEDirMonitor.GetXBEListPtr() )
    {
        m_bUpdateTexture = TRUE;
    }

    // Get the XBEList pointer to display our items
    m_pXBEList = m_XBEDirMonitor.GetXBEListPtr();

    // If it's not time to update our texture, simply return outta here
    if( !m_bUpdateTexture )
    {
        return;
    }
    else // Reset for next frame
    {
        m_bUpdateTexture = FALSE;
    }

    // Lock our texture and set it as the render target
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );

    // If the user has changed the number of items, update!!
    if( m_uiPrevNumItems != GetNumItems() )
    {
        if( 0 == GetNumItems() )
        {
            SetInitialSelectState();
        }
        else if ( GetNumItems() >= m_uiPrevNumItems ) // If the amount is larger, we don't need to reset to the top
        {
            SetTopIndexOfItem( GetTopIndexOfItem() ); // Do this to update the Bottom Index correctly
            AdjustDisplayArrows();
        }
        else // Reset everything to the top of the menu
        {
            SetSelectedItem( 0 );
            SetTopIndexOfItem( 0 );
            SetSelectorPos( 0 );
            AdjustDisplayArrows();
        }

        m_uiPrevNumItems = GetNumItems();  // Reset our previous counter
    }

    // Draw our XBE items if we need to
    if( GetNumItems() > 0 )
    {
        // Update our Font Height
        XFONT_SetTextHeight( g_pFont, ITEM_XBELIST_FONT_HEIGHT );
        m_uiFontHeightOfItem = ITEM_XBELIST_FONT_HEIGHT;

        // Draw a box behind the currently highlighted choice
        float X1Pos = MENUBOX_SELECTBAR_X1;
        float Y1Pos = MENUBOX_TEXTAREA_Y1 + MENU_LAUNCH_TOP_BORDER_SIZE + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectorPos() ) - ITEM_SELECTOR_BORDER_SPACE;
        float X2Pos = MENUBOX_SELECTBAR_X2;
        float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );

        pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );

        // Draw the XBE files on to our screen
        for( unsigned int x = GetTopIndexOfItem(); x <= GetBottomIndexOfItem(); ++x )
        {
            float XPos = MENUBOX_TEXTAREA_X1;
            float YPos = MENUBOX_TEXTAREA_Y1 + MENU_LAUNCH_TOP_BORDER_SIZE + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * ( x - GetTopIndexOfItem() ) );

            // Create a buffer that can hold the title name, XBE name + extension, a space, 2 parens and a NULL
            int bufLen = XBEIMAGE_TITLE_NAME_LENGTH + MAX_PATH + 8;
            WCHAR* pwszDisplayString = new WCHAR[bufLen];
            if( !pwszDisplayString )
            {
                XDBGERR( APP_TITLE_NAME_A, "CLaunchMenu::Action():Failed to allocate memory!!" );

				break;  // Break out of our loop so that we don't try to mess with the buffer
            }

            // NULL out our new memory
            ZeroMemory( pwszDisplayString, sizeof( WCHAR ) * bufLen );

            // If the friendly title is null, only display the XBE Name
            if( L'\0' == m_pXBEList->GetXBETitle( x )[0] )
            {
                _snwprintf( pwszDisplayString, bufLen - 1, L"%hs", m_pXBEList->GetXBEName( x ) );
            }
            else  // Display the friendly name and the XBE Name
            {
#ifdef COOL_XDASH
                _snwprintf( pwszDisplayString, bufLen - 1, L"%ls", m_pXBEList->GetXBETitle( x ) );
#else
                _snwprintf( pwszDisplayString, bufLen - 1, L"%ls (%hs)", m_pXBEList->GetXBETitle( x ), m_pXBEList->GetXBEName( x ) );
#endif // COOL_XDASH
            }
            // pScreen->TruncateStringToFit( pwszDisplayString, bufLen, XPos, MENUBOX_SELECTBAR_X2 - 7.0f );

            // If we are on the currently selected item, let's set the text color
            DWORD dwTextCol;
            if( GetSelectedItem() == (int)x )
            {
                dwTextCol = ITEM_SELECTED_TEXT_COLOR;
            }
            else
            {
                dwTextCol = SCREEN_DEFAULT_TEXT_FG_COLOR;
            }

            pTexture->DrawText( XPos, YPos, dwTextCol, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%ls", pwszDisplayString );

            delete[] pwszDisplayString;
            pwszDisplayString = NULL;
        }

        m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
        XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );
    }

    // Unlock our texture and restore our render target
    pTexture->Unlock();

    /////////////////////
    // Draw # XBE Message
    /////////////////////

    // Lock the Title Texture so we can render on to it

    // Signal that our title texture is going to change
    g_bUpdateTitleTexture = TRUE;

    #define LM_BUFFSIZE 60

    WCHAR tempBuff[LM_BUFFSIZE+1];
    ZeroMemory( tempBuff, ( LM_BUFFSIZE + 1 ) * sizeof( WCHAR ) );

    if( 1 == GetNumItems() )
    {
        _snwprintf( tempBuff, LM_BUFFSIZE, L"%d Xbox executable", GetNumItems() );
    }
    else
    {
        _snwprintf( tempBuff, LM_BUFFSIZE, L"%d Xbox executables", GetNumItems() );
    }

    SetTitle( tempBuff );
}


// Get the number of items that should be displayed on the screen
unsigned int CLaunchMenu::GetNumItemsToDisplay()
{
    unsigned int uiReturnVal = 0;
    float panelHeight = MENUBOX_TEXTAREA_Y2 - MENUBOX_TEXTAREA_Y1;

    uiReturnVal = (unsigned int)( ( panelHeight - MENU_LAUNCH_TOP_BORDER_SIZE ) / ( ITEM_XBELIST_FONT_HEIGHT + ITEM_VERT_SPACING ) );
    return uiReturnVal;
}


// Handle the Joystick Input
void CLaunchMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CLaunchMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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

    // Check the secret key to see if we should display the special menu
    if( ProcessSecretKey() )
    {
        return;
    }

    // Font height of our XBE Items
    XFONT_SetTextHeight( g_pFont, ITEM_XBELIST_FONT_HEIGHT );
    m_uiFontHeightOfItem = ITEM_XBELIST_FONT_HEIGHT;

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() > 0 )
            {
                if( GetSelectedItem() > 0 )
                {
                    // Flag our texture to be updated, since the selection has changed
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() - 1 );

                    // Adjust the screen appropriately
                    if( GetSelectorPos() > 0 )
                    {
                        SetSelectorPos( GetSelectorPos() - 1 );
                    }
                    else
                    {
                        SetTopIndexOfItem( GetTopIndexOfItem() - 1 );
                    }

                    AdjustDisplayArrows();
                }
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() > 0 )
            {
                if( (unsigned int)GetSelectedItem() < ( GetNumItems() - 1 ) )
                {
                    // Flag our texture to be updated, since the selection has changed
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() + 1 );

                    // Adjust the screen appropriately
                    if( GetSelectorPos() < ( GetNumItemsToDisplay() - 1 ) )
                    {
                        SetSelectorPos( GetSelectorPos() + 1 );
                    }
                    else
                    {
                        SetTopIndexOfItem( GetTopIndexOfItem() + 1 );
                    }

                    AdjustDisplayArrows();
                }
            }
            break;
        }
	}

    // Restoring Font Height
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );
    m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
}


// Handles input for the current menu
void CLaunchMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If this is a repeat button press, let's bail
    if( !bFirstPress )
    {
        return;
    }

    // Check the secret key to see if we should display the special menu
    if( ProcessSecretKey() )
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

                char* pszDirname = new char[MAX_PATH+1];
                char* pszFilename = new char[MAX_PATH+1];
                if( ( NULL == pszDirname ) || ( NULL == pszFilename ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CLaunchMenu::HandleInput():Failed to allocate memory!!  Can't reboot to the title!!" );
                }
                else
                {
                    ZeroMemory( pszDirname, MAX_PATH + 1 );
                    ZeroMemory( pszFilename, MAX_PATH + 1 );

                    _snprintf( pszFilename, MAX_PATH, "%s%s", m_pXBEList->GetXBEName( GetSelectedItem() ), FILE_EXECUTABLE_EXENSION_A );

                    if( ( strcmp( "", m_pXBEList->GetXBEDir( GetSelectedItem() ) ) == 0 ) &&
                        ( ( strcmp( FILE_DATA_DASHBOARD_FILENAME_A, pszFilename ) == 0 ) ||
                        ( ( strcmp( FILE_DATA_ONLINE_DASHBOARD_FILENAME_A, pszFilename ) == 0 ) ) ) )
                    //
                    // Check to see if we are dealing with the Dashboard or Online Dashboard (special case #1)
                    //
                    {
                        _snprintf( pszDirname, MAX_PATH, "%s%s", FILE_DASH_PARTITION_PATH_A, m_pXBEList->GetXBEDir( GetSelectedItem() ) );
                    }
                    else if( ( strcmp( "", m_pXBEList->GetXBEDir( GetSelectedItem() ) ) == 0 ) &&
                           ( strcmp( FILE_DATA_DEFAULT_FILENAME_A, pszFilename ) == 0 ) )
                    //
                    // Check to see if we are dealing with the DEFAULT.XBE from the DVD Drive (special case #2)
                    //
                    {
                        _snprintf( pszDirname, MAX_PATH, "%s%s", FILE_DVDDRIVE_PARTITION_PATH_A, m_pXBEList->GetXBEDir( GetSelectedItem() ) );
                    }
                    else
                    //
                    // No special case.  Must be dealing with an XBE from our \devkit directory
                    //
                    {
                        _snprintf( pszDirname, MAX_PATH, "%s%s", FILE_EXECUTABLE_DEVICE_PATH_A, m_pXBEList->GetXBEDir( GetSelectedItem() ) );
                    }

                    DWORD dwSleepTime = 0;
                    // Let the Select Sound finish before we launch
                    while( g_Sounds.IsSoundPlaying( ENUM_SOUND_SELECT ) && ( dwSleepTime < 5000 ) )
                    {
                        dwSleepTime += 10;
                        Sleep( 10 );
                    }

                    //////////////////////////////
                    // Display our logo screen
                    //////////////////////////////
                    CUDTexture SplashTexture;

                    if( g_XboxSettings.GetWidescreen() )
                        SplashTexture.Initialize(g_pD3DDevice, "y:\\images\\logow.bmp", 512, 512 );
                    else
                        SplashTexture.Initialize(g_pD3DDevice, "y:\\images\\logo.bmp", 512, 512 );

                    IDirect3DVertexBuffer8* pVB = NULL;
                    MYTEXVERTEX* pb = NULL;

                    g_pD3DDevice->CreateVertexBuffer( sizeof( MYTEXVERTEX ) * 4, 0, 0, 0, &pVB );

                    pVB->Lock( 0, 0, (BYTE**)&pb, 0 );

                    pb[0].v.x = 0.0f;         pb[0].v.y = 0.0f;          pb[0].v.z = 0.1f; pb[0].fRHW = 0.9f; pb[0].diffuse = COLOR_WHITE; pb[0].tu = 0.0f; pb[0].tv = 0.0f;
                    pb[1].v.x = SCREEN_WIDTH; pb[1].v.y = 0.0f;          pb[1].v.z = 0.1f; pb[1].fRHW = 0.9f; pb[1].diffuse = COLOR_WHITE; pb[1].tu = 1.0f; pb[1].tv = 0.0f;
                    pb[2].v.x = SCREEN_WIDTH; pb[2].v.y = SCREEN_HEIGHT; pb[2].v.z = 0.1f; pb[2].fRHW = 0.9f; pb[2].diffuse = COLOR_WHITE; pb[2].tu = 1.0f; pb[2].tv = 1.0f;
                    pb[3].v.x = 0.0f;         pb[3].v.y = SCREEN_HEIGHT; pb[3].v.z = 0.1f; pb[3].fRHW = 0.9f; pb[3].diffuse = COLOR_WHITE; pb[3].tu = 0.0f; pb[3].tv = 1.0f;

                    pVB->Unlock();

                    // Make sure that edge antialiasing is off for the following objects
                    DWORD dwEdgeCurrentState = 0;

                    g_pD3DDevice->GetRenderState( D3DRS_EDGEANTIALIAS, &dwEdgeCurrentState );
                    g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, FALSE );

                    g_pD3DDevice->SetVertexShader( D3DFVF_MYTEXVERTEX );
                    g_pD3DDevice->SetStreamSource( 0, pVB, sizeof( MYTEXVERTEX ) );
                    g_pD3DDevice->SetTexture( 0, SplashTexture.GetTexture() );
                    g_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
                    g_pD3DDevice->SetTexture( 0, NULL );

                    g_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, dwEdgeCurrentState );

                    pVB->Release();
                    pVB = NULL;

                    g_pD3DDevice->Present( NULL, NULL, NULL, NULL );

                    g_pD3DDevice->PersistDisplay();

                    // Make sure our Background Thread is gone before we call the reboot API
                    if( !m_XBEDirMonitor.SoftBreak( 60000 ) )   // If there are a lot of XBE's, this could take a long time
                        XDBGWRN( APP_TITLE_NAME_A, "CLaunchMenu::HandleInput():Failed to Shutdown our background XBE Search Thread!!" );

                    // Launch the title that the user selected
                    XDBGWRN( APP_TITLE_NAME_A, "CLaunchMenu::HandleInput():Filename  - '%s'", pszFilename );
                    XDBGWRN( APP_TITLE_NAME_A, "CLaunchMenu::HandleInput():Directory - '%s'", pszDirname );
                    NTSTATUS status = WriteTitleInfoAndReboot( pszFilename, pszDirname, LDT_NONE, 0, NULL );
                    if( FAILED( status ) ) // !NT_SUCCESS( status ) ) // API says this returns NTSTATUS, but I think it's an HRESULT
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CLaunchMenu::HandleInput():Failed to launch title!! NTSTATUS - '0x%.8X (%d)'", status, status );
                    }
                }

                if( pszDirname )
                {
                    delete[] pszDirname;
                    pszDirname = NULL;
                }

                if( pszFilename )
                {
                    delete[] pszFilename;
                    pszFilename = NULL;
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

                // SetSelectedItem( 0 );
                Leave( GetParent() );
            }

            break;
        }
    case BUTTON_X:
        {
            // Play the Select Sound
            g_Sounds.PlaySelectSound();

            Leave( &m_OptionsMenu );

            break;
        }
    case BUTTON_Y:
        {
            // Play the Select Sound
            g_Sounds.PlaySelectSound();

            // Go to the Online Menu
            Leave( &m_OnlineMenu );

            break;
        }
    case BUTTON_WHITE:
        {
            break;
        }
    }
}


// Set the selector position on the screen
HRESULT CLaunchMenu::SetSelectorPos( unsigned int index )
{
    if( index >= GetNumItemsToDisplay() )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CLaunchMenu::SetSelectorPos():Invalid index passed!! - '%d', MAX - '%d'", index, GetNumItemsToDisplay() - 1 );

        return E_INVALIDARG;
    }

    m_nSelectorPos = index;

    return S_OK;
}


// Set the top index of item that should be displayed on our screen
HRESULT CLaunchMenu::SetTopIndexOfItem( unsigned int index )
{
    // Make sure the passed in index is either 0, or within the realm of display
    if( ( ( index + GetNumItemsToDisplay() - 1 ) < GetNumItems() ) || ( 0 == index) )
    {
        m_nTopIndexOfItem = index;
    }
    else
    {
        XDBGWRN( APP_TITLE_NAME_A, "CLaunchMenu::SetTopIndexOfItem():Invalid index passed!! - '%d'", index );

        return E_INVALIDARG;
    }

    if( 0 == GetNumItems() )
    {
        SetBottomIndexOfItem( 0 );
    }
    else if( GetNumItems() < GetNumItemsToDisplay() )
    {
        SetBottomIndexOfItem( GetNumItems() - 1 );
    }
    else
    {
        SetBottomIndexOfItem( m_nTopIndexOfItem + GetNumItemsToDisplay() - 1 );
    }

    return S_OK;
}


// Set the bottom index of item that should be displayed on our screen
HRESULT CLaunchMenu::SetBottomIndexOfItem( unsigned int index )
{
    if( index > GetNumItems() )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CLaunchMenu::SetBottomIndexOfItem():Invalid index passed!! - '%d', MAX - '%d'", index, GetNumItems() - 1 );

        return E_INVALIDARG;
    }

    m_nBottomIndexOfItem = index;

    return S_OK;
}


// Adjust whether we display the up and down arrows
void CLaunchMenu::AdjustDisplayArrows()
{
    // Set the arrows

    if( 0 == GetNumItems() )
    {
        SetDisplayUpArrow( FALSE );
        SetDisplayDownArrow( FALSE );

        return;
    }

    // Top arrow
    if( GetTopIndexOfItem() > 0 )
    {
        SetDisplayUpArrow( TRUE );
    }
    else
    {
        SetDisplayUpArrow( FALSE );
    }


    // Bottom arrow
    if( GetBottomIndexOfItem() < ( GetNumItems() - 1 ) )
    {
        SetDisplayDownArrow( TRUE );
    }
    else
    {
        SetDisplayDownArrow( FALSE );
    }
}


// Set the initial select state
void CLaunchMenu::SetInitialSelectState()
{
    SetSelectedItem( 0 );
    SetSelectorPos( 0 );
    SetTopIndexOfItem( 0 );
    SetBottomIndexOfItem( 0 );
    SetDisplayUpArrow( FALSE );
    SetDisplayDownArrow( FALSE );
}


// Get a pointer to a sub-menu
CXItem* CLaunchMenu::GetItemPtr( enum XShellMenuIds menuId )
{
    CXItem* pReturn = NULL;

    if( ENUM_SHELLMENUID_OPTIONS == menuId )
    {
        pReturn = &m_OptionsMenu;
    }

    return pReturn;
}


// Should be called whenever a user leaves the menu
void CLaunchMenu::Leave( CXItem* pItem )
{
    CMenuScreen::Leave( pItem );

    // Set the B Button to be displayed
    g_bDisplayBButton = TRUE;

    // Make sure we turn off the Up and Down arrows
    SetDisplayUpArrow( FALSE );
    SetDisplayDownArrow( FALSE );
}

// This will be called whenever the user enters this menu
void CLaunchMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Set the B Button to be hidden
    g_bDisplayBButton = FALSE;

    // Make sure our display arrows are acting correctly
    AdjustDisplayArrows();

    // Reset our secret key combo
    ZeroMemory( g_pszKeyCombo, 5 );
    g_uiKeyPos = 0;
}


// Checks the secret key to see if we should display the 'secret' menu
BOOL CLaunchMenu::ProcessSecretKey()
{
	XDBGTRC( APP_TITLE_NAME_A, "g_pszKeyCombo - '%c'", g_pszKeyCombo[g_uiKeyPos] );

    // Check to see if the user entered the special key combo for the Secret Hardware menu
    if( g_pszKeyCombo[g_uiKeyPos] != m_pszSecretKeyCombo[g_uiKeyPos] )
    {
        g_uiKeyPos = 0; // Reset our Key Sequence
    }
    else
    {
        // Check to see if we found our key sequence.  If so, take us to the secret menu
        if( 4 == ++g_uiKeyPos )
        {
            g_uiKeyPos = 0;

            // Play the Select Sound
            g_Sounds.PlaySelectSound();

            Leave( &m_SysInfoMenu );

            return TRUE;
        }
    }

    return FALSE;
}

// These should never change
#define SECRET_RAND_KEY "\xAC\x21\x07\x1B\xB5\x94\xE8\x23\x50\x33\x4A\x71\x72\x7A\xD5\x91"
#define SECRET_RAND_KEY_LEN SYMMETRIC_KEY_LEN

// A   - A Button
// B   - B Button
// Y   - Y Button
// X   - X Button
// L   - bLack Button
// W   - White Button
// U   - DPAD Up
// D   - DPAD Down
// F   - DPAD leFt
// R   - DPAD Right
// T   - Left-Thumb Button
// I   - rIght-Thumb Position
// E   - lEft trigger button
// O   - right trigger buttOn
void CLaunchMenu::ComputeSecretKey( char* Buffer )
{
    if( NULL == Buffer )
    {
        return;
    }

    BYTE Digest[XC_SERVICE_DIGEST_SIZE];
    WORD* pwDigestWORD = (WORD*)(&Digest[0]);
    int i = 4;

    BYTE pbMacAddress[SETTINGS_FACTORY_ETHERNET_SIZE + 1];
    ZeroMemory( pbMacAddress, SETTINGS_FACTORY_ETHERNET_SIZE + 1 );

    g_XboxSettings.GetFactoryEthernetAddr( pbMacAddress, SETTINGS_FACTORY_ETHERNET_SIZE );

    for( int x = 0; x < SETTINGS_FACTORY_ETHERNET_SIZE; x++ )
        XDBGTRC( APP_TITLE_NAME_A, "pbMacAddress - '%X'", pbMacAddress[x] );

    // Don't use "A" or "X" button in the combination
    char* secretKeyMap = "BYLWUDFRTIEO";

    XcHMAC( (LPBYTE)SECRET_RAND_KEY, SECRET_RAND_KEY_LEN,
            pbMacAddress, SETTINGS_FACTORY_ETHERNET_SIZE,
            NULL, 0,
            Digest );

    RtlZeroMemory( Buffer, 4 );
    while ( --i >= 0 )
    {
        Buffer[i] = secretKeyMap[ (*pwDigestWORD++) % 12 ];
    }
}

NTSTATUS CLaunchMenu::WriteTitleInfoAndReboot(LPCSTR pszLaunchPath, LPCSTR pszDDrivePath,
    DWORD dwLaunchDataType, DWORD dwTitleId, PLAUNCH_DATA pLaunchData)
{
    extern PLAUNCH_DATA_PAGE *LaunchDataPage;
    NTSTATUS Status;
    LARGE_INTEGER ByteOffset;
    PSTR pszOutput;
    int cch;

    if (NULL == *LaunchDataPage)
    {
        *LaunchDataPage = (PLAUNCH_DATA_PAGE)MmAllocateContiguousMemory(PAGE_SIZE);
    }

    if (NULL == *LaunchDataPage)
    {
        return STATUS_NO_MEMORY;
    }

    MmPersistContiguousMemory(*LaunchDataPage, PAGE_SIZE, TRUE);

    if (!pszDDrivePath)
    {
        pszDDrivePath = "\\Device\\Cdrom0";
    }

    pszOutput = ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath;

    ASSERT(PAGE_SIZE == sizeof(LAUNCH_DATA_PAGE));

    RtlZeroMemory(*LaunchDataPage, PAGE_SIZE);

    ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.dwLaunchDataType = dwLaunchDataType;
    ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.dwTitleId = dwTitleId;

    if (dwLaunchDataType != LDT_NONE)
    {
        memcpy(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->LaunchData,
               pLaunchData,
               sizeof(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->LaunchData));
    }

    if (NULL != pszLaunchPath)
    {
        if (lstrcmpiA(pszDDrivePath, "\\Device\\Harddisk0\\Partition2") == 0)
        {
            lstrcpynA(pszOutput, "\\Device\\Harddisk0\\Partition2\\",
                      ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - 1);
            cch = strlen(pszOutput);
            lstrcpynA(&(pszOutput[cch]),
                      pszLaunchPath,
                      ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - (cch));
        }
        else
        {
            lstrcpynA(pszOutput,
                      pszDDrivePath,
                      ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - 1);

            cch = strlen(pszOutput);
            pszOutput[cch++] = TITLE_PATH_DELIMITER;

            lstrcpynA(&(pszOutput[cch]),
                      pszLaunchPath,
                      ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - (cch));
        }
    }

    // Notify the debugger that we're about to reboot and then reboot
    DmTell(DMTELL_REBOOT, NULL);

    HalReturnToFirmware(HalQuickRebootRoutine);

    return Status;
}
