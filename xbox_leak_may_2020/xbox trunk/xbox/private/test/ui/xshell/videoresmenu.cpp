/*****************************************************
*** videoresmenu.cpp
***
*** CPP file for our XShell Video Resolution menu 
*** class.  This menu will allow the user to select
*** different screen resolutions and run the
*** XDK Launcher in that resolution
***
*** by James N. Helm
*** June 9th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "videoresmenu.h"

extern IDirect3DDevice8* g_pD3DDevice;                  // Pointer to our Direct3D Device Object
extern CSoundCollection  g_Sounds;                      // Used to generate all sounds for the XShell
extern XFONT* g_pFont;

// Constructors
CVideoResMenu::CVideoResMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_pVideoRes( NULL ),
m_uiNumRes( 0 )
{
    XDBGTRC( APP_TITLE_NAME_A, "CVideoResMenu::CVideoResMenu(ex)" );

    SetTitle( MENU_TITLE_VIDEORES );

    GetVideoResModes();
    m_uiNumItems = m_uiNumRes;
    SetSelectedItem( 0 );
}


// Destructor
CVideoResMenu::~CVideoResMenu()
{
    if( NULL != m_pVideoRes )
    {
        delete[] m_pVideoRes;
        m_pVideoRes = NULL;
    }
}

// Draws a menu on to the screen
void CVideoResMenu::Action( CUDTexture* pTexture )
{
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

    // Let our base menu class draw all the items pulled from the MNU file
    CMenuScreen::Action( pTexture );

    // Update our Font Height
    XFONT_SetTextHeight( g_pFont, ITEM_OPTIONMENU_FONT_HEIGHT );
    m_uiFontHeightOfItem = ITEM_OPTIONMENU_FONT_HEIGHT;

    // Draw a box behind the currently highlighted choice
    float X1Pos = MENUBOX_SELECTBAR_X1;
    float Y1Pos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectedItem() ) - ITEM_SELECTOR_BORDER_SPACE;
    float X2Pos = MENUBOX_SELECTBAR_X2;
    float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );
    
    pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );

    //
    // Draw our Menu Items
    //

    /*
    for( unsigned int x = 0; x < ENUM_VIDEORES_MAX_MENUITEMS; x++ )
    {
        float XPos = MENUBOX_TEXTAREA_X1;
        float YPos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

        pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", g_pwszVideoResMenuItems[x] );
    }
    */
    for( unsigned int x = 0; x < m_uiNumRes; x++ )
    {
        float XPos = MENUBOX_TEXTAREA_X1;
        float YPos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

        pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ux%u", m_pVideoRes[x].uiWidth, m_pVideoRes[x].uiHeight );
    }

    // Correct our Font Height
    m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
    XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

    // Unlock the texture
    pTexture->Unlock();
}


// Handle any requests for the joystick (thumb-pad)
void CVideoResMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
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
void CVideoResMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
            // Only act if we have any items on our screen
            if( GetSelectedItem() > 0 )
            {
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

                SetSelectedItem( GetSelectedItem() - 1 );
            }

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( ( (unsigned int)GetSelectedItem() + 1 ) < GetNumItems() )
            {
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

                SetSelectedItem( GetSelectedItem() + 1 );
            }

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CVideoResMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, let's bail
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
                m_bUpdateTexture = TRUE;

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                /*
                DWORD dwFlags = 0;
                if( 240 == m_pVideoRes[GetSelectedItem()].uiHeight )
                {
                    dwFlags = D3DPRESENTFLAG_FIELD;
                }
                */

                ::InitD3D( m_pVideoRes[GetSelectedItem()].uiWidth, m_pVideoRes[GetSelectedItem()].uiHeight, m_pVideoRes[GetSelectedItem()].uiFlags );
                ::Init3DObjects();
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
HRESULT CVideoResMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "CVideoResMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( menuFileName );

    return hr;
}


// This will be called whenever the user enters this menu
void CVideoResMenu::Enter()
{
    // Call the base-class enter
    CMenuScreen::Enter();

    // Determine which option should be highlighted
    unsigned int uiScreenWidth = ::GetScreenWidth();
    unsigned int uiScreenHeight = ::GetScreenHeight();

    for( unsigned int x = 0; x < m_uiNumRes; x++ )
    {
        if( ( uiScreenWidth == m_pVideoRes[x].uiWidth ) && ( uiScreenHeight == m_pVideoRes[x].uiHeight ) )
        {
            SetSelectedItem( x );
            
            break;
        }
    }
}


// This function will enumerate all of the valid video resolution modes
void CVideoResMenu::GetVideoResModes()
{
	// Query the number of display modes
    IDirect3D8* pD3D = NULL;
    
    // Create our Direct3D Object
    pD3D = Direct3DCreate8( D3D_SDK_VERSION );
    if( NULL == pD3D )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CVideoResMenu::GetVideoResModes():Failed to create our D3D pointer!!" );

        return;
    }

	DWORD dwNumModes = pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT );
    XDBGTRC( APP_TITLE_NAME_A, "CVideoResMenu::Enter():dwNumModes1 - '%d'", dwNumModes );

    // Enumerate the mode lists
    D3DDISPLAYMODE* pModeList = new D3DDISPLAYMODE[dwNumModes];
    if( NULL == pModeList )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CVideoResMenu::GetVideoResModes():Failed to allocate memory!!" );
    }
    else
    {
        D3DFORMAT formats[]	= 
	    { 
		    D3DFMT_D24S8, 
		    D3DFMT_D16, 
		    D3DFMT_F24S8, 
		    D3DFMT_F16, 
		    D3DFMT_D16_LOCKABLE 
	    };
        unsigned int uiNumFormats = sizeof( formats ) / sizeof( formats[0] );

        for( unsigned int uiIndex = 0, i = 0; i < dwNumModes; ++i )
	    {
		    if( pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, i, &pModeList[uiIndex] ) != D3D_OK )
		    {
                XDBGWRN( APP_TITLE_NAME_A, "CVideoResMenu::GetVideoResModes():Failed to enumerate mode - '%u'", i );
                /*
			    // Find the first valid depth format for the mode
			    for( unsigned int ii = 0; ii < uiNumFormats; ++ii )
			    {
				    // Is it a good match?
				    if( pD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
												             pModeList[index].Format,
												             pModeList[index].Format,
												             formats[ii]) == D3D_OK )
				    {
					    ++index;
					    break;
				    }
			    }
                */
		    }
            else
            {
                ++uiIndex;
            }
	    }

	    dwNumModes = uiIndex;
        XDBGTRC( APP_TITLE_NAME_A, "CVideoResMenu::Enter():dwNumModes2 - '%d'", dwNumModes );

        m_pVideoRes = new VideoRes[dwNumModes];   // Holds a list of our supported video modes
        if( NULL == m_pVideoRes )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CVideoResMenu::GetVideoResModes():Failed to allocate memory!!" );
        }
        else
        {
            ZeroMemory( m_pVideoRes, sizeof( VideoRes ) * dwNumModes );
            BOOL bAddItem;

            for( unsigned int x = 0; x < dwNumModes; x++ )
            {
                bAddItem = TRUE;

                // Add the modes to our array, if it doesn't already exist in our matrix
                for( unsigned int xx = 0; xx < m_uiNumRes; xx++ )
                {
                    if( ( m_pVideoRes[xx].uiWidth == pModeList[x].Width ) && ( m_pVideoRes[xx].uiHeight == pModeList[x].Height ) )
                    {
                        bAddItem = FALSE;
                        
                        break;
                    }
                }

                if( bAddItem )
                {
                    m_pVideoRes[m_uiNumRes].uiWidth = pModeList[x].Width;
                    m_pVideoRes[m_uiNumRes].uiHeight = pModeList[x].Height;
                    m_pVideoRes[m_uiNumRes].uiFlags = pModeList[x].Flags;
                    
                    XDBGTRC( APP_TITLE_NAME_A, "CVideoResMenu::GetVideoResModes():Added #%u - '%ux%u', Flags = '%u'", m_uiNumRes, m_pVideoRes[m_uiNumRes].uiWidth, m_pVideoRes[m_uiNumRes].uiHeight, m_pVideoRes[m_uiNumRes].uiFlags );
                    
                    m_uiNumRes++;
                }
            }
        }
    }

    // If this wasn't created, we would have returned, so we can safely release without checking
    pD3D->Release();
    pD3D = NULL;

    if( NULL != pModeList )
    {
        delete[] pModeList;
        pModeList = NULL;
    }
}

