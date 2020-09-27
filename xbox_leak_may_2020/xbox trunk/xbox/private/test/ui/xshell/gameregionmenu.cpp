/*****************************************************
*** gameregionmenu.cpp
***
*** CPP file for our Game Region menu class.  This
*** class will allow a user to select a new game
*** region
***
*** by James Helm
*** May 11th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "gameregionmenu.h"

extern CXSettings       g_XboxSettings;     // Xbox Settings Object
extern CSoundCollection g_Sounds;           // Used to generate all sounds for the XShell

// Constructor
CGameRegionMenu::CGameRegionMenu( CXItem* pParent )
: CKeypadMenu( pParent )
{
    XDBGTRC( APP_TITLE_NAME_A, "CGameRegionMenu::CGameRegionMenu( CXItem )" );

    // Set the number of items on the menu
    m_uiNumItems = GAMEREGIONMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

	GenerateKeypad();

    // Default the key to the TOP Button
	m_xKeypos = 0;
	m_yKeypos = 0;

    SetTitle( MENU_TITLE_GAMEREGION );
}


// Destructor
CGameRegionMenu::~CGameRegionMenu()
{
}

// Draws a menu on to the screen
void CGameRegionMenu::Action( CUDTexture* pTexture )
{
    // Check to see if we should update our texture, if not, return
    if( !m_bUpdateTexture )
    {
        return;
    }
    else // Reset the Update flag
    {
        m_bUpdateTexture = FALSE;
    }

    // Lock our texture so we can render to it
    pTexture->Lock();
    pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );
    
    // Let our base menu draw the items from the data file
    CMenuScreen::Action( pTexture );

    //
	// Render the current Game Region Text
    //
    int iStringPixelWidth = GetStringPixelWidth( MENU_GAMEREGION_CURRENT_REGION_TEXT );
    float fXPos = MENUBOX_TEXTAREA_X_CENTER - ( iStringPixelWidth / 2.0f );
    float fYPos = MENU_GAMEREGION_TEXT_Y1;
    pTexture->DrawText( fXPos, fYPos, ITEM_HEADER_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", MENU_GAMEREGION_CURRENT_REGION_TEXT );

    iStringPixelWidth = GetStringPixelWidth( g_pwszGameRegionMenuItems[GetGameRegion()] );
    fXPos = MENUBOX_TEXTAREA_X_CENTER - ( iStringPixelWidth / 2.0f );
    unsigned int uiFontHeight, uiFontWidth;
    GetFontSize( &uiFontHeight, &uiFontWidth );
    fYPos = MENU_GAMEREGION_TEXT_Y1 + uiFontHeight + ITEM_VERT_SPACING;
    pTexture->DrawText( fXPos, fYPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", g_pwszGameRegionMenuItems[GetGameRegion()] );
    
    //
    // Draw our keypad / Game Region Menu Items
    //
	renderKeypad( pTexture );

    // Unlock our Texture
    pTexture->Unlock();
}

// Handles input (of the BUTTONS) for the current menu
void CGameRegionMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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
            // Don't update the setting unless the user is on one of the buttons
            if( 0 == m_xKeypos )
            {
                m_bUpdateTexture = TRUE;

                // Play the Select Sound
                g_Sounds.PlaySelectSound();

                // Set the Game Region in to the Config Sector
                HRESULT hr = SetGameRegion( m_yKeypos ); // m_yKeypos will map to the enum GameRegionMenu items
                if( FAILED( hr ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CGameRegionMenu::HandleInput():Failed to set the Game Region!! Region - '%d', Error - '0x%X.08'", m_yKeypos, hr );
                }
                else
                {
                    XDBGTRC( APP_TITLE_NAME_A, "CGameRegionMenu::HandleInput():Successfully set the Game Region!!" );
                }

                Leave( GetParent() );
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

// Handles input (of the CONTROLS) for the current menu
void CGameRegionMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
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
            if( m_yKeypos != 0 )
            {
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

                m_yKeypos--;
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( m_yKeypos < ( GAMEREGIONMENU_ITEM_NUM_ITEMS - 1 ) )
            {
                m_bUpdateTexture = TRUE;

                // Play the Menu Item Select Sound
                g_Sounds.PlayMenuItemSelectSound();

				m_yKeypos++;
            }
            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            break;
        }
	}
}


void CGameRegionMenu::GenerateKeypad(void) 
{
	// First create numberpad
	int xgrid  = MENU_GAMEREGION_NUM_COLS;
	int ygrid  = MENU_GAMEREGION_NUM_ROWS;
	int xstart = MENU_GAMEREGION_BUTTON_X1;
	int ystart = MENU_GAMEREGION_BUTTON_Y1;
	int width  = MENU_GAMEREGION_BUTTON_WIDTH;
	int height = 32;
	int spacer = 2;

	int currentx = xstart;
	int currenty = ystart;

	for(int y = 0; y < ygrid; y++)
	{
		for(int x = 0; x < xgrid; x++)
		{
			m_keyPad[x][y].define( currentx, currenty, width, height, SCREEN_DEFAULT_TEXT_FG_COLOR );
			currentx += width + spacer;
		}
		currentx = xstart;
		currenty += height + spacer;
	}

    // Set the text of the keys
    for( int z = 0; z < GAMEREGIONMENU_ITEM_NUM_ITEMS; z++ )
    {
        m_keyPad[0][z].defineText(g_pwszGameRegionMenuItems[z]);
    }
}


BOOL CGameRegionMenu::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < MENU_GAMEREGION_NUM_COLS; x++)
	{
		for(int y = 0; y < MENU_GAMEREGION_NUM_ROWS; y++)
		{
			if(m_keyPad[x][y].getRender())
			{
                // Lock our texture
                pTexture->Lock();

				x1 = (float)m_keyPad[x][y].xorigin;
				x2 = (float)(m_keyPad[x][y].xorigin + m_keyPad[x][y].width);
				
				y1 = (float)m_keyPad[x][y].yorigin;
				y2 = (float)(m_keyPad[x][y].yorigin + m_keyPad[x][y].height);
	
				// hilight key if currently selected
                DWORD dwCharColor;
				if((m_xKeypos == x) && (m_yKeypos == y))
                {
					pTexture->DrawBox( x1, y1, x2, y2, ITEM_SELECTOR_COLOR );
                    
                    dwCharColor = ITEM_SELECTED_VALUE_COLOR;
                }
                else
                {
                    dwCharColor = SCREEN_DEFAULT_TEXT_FG_COLOR;
                }

                pTexture->DrawOutline( x1, y1, x2, y2, MENU_GAMEREGION_LINE_WIDTH, m_keyPad[x][y].selectColor );
				
				unsigned int decent;
				unsigned int fontHeight;
				GetFontSize( &fontHeight, &decent );
				
				float textX = (float)((m_keyPad[x][y].width / 2) + m_keyPad[x][y].xorigin) - GetStringPixelWidth( m_keyPad[x][y].resultChar ) / 2;
				float textY = (float)((m_keyPad[x][y].height / 2) + m_keyPad[x][y].yorigin) - (fontHeight / 2);
				pTexture->DrawText( textX, textY, dwCharColor, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", m_keyPad[x][y].resultChar );

                // Unlock our Texture
                pTexture->Unlock();
			}
		}
	}
	
	return TRUE;
}

// This will be called whenever the user enters this menu
void CGameRegionMenu::Enter()
{
    // Call the base-class enter
    CKeypadMenu::Enter();

    // Default the key to the currently selection option
	m_xKeypos = 0;
	m_yKeypos = GetGameRegion(); // 0;

    // Make sure our settings are up to date
    g_XboxSettings.LoadSettings();
}


// Get the Game Region in a 'menu-friendly' format
DWORD CGameRegionMenu::GetGameRegion()
{
    switch( g_XboxSettings.GetGameRegion() )
    {
    case XC_GAME_REGION_NA:
        {
            return GAMEREGIONMENU_ITEM_REGION1;

            break;
        }
    case XC_GAME_REGION_JAPAN:
        {
            return GAMEREGIONMENU_ITEM_REGION2;

            break;
        }
    case XC_GAME_REGION_RESTOFWORLD:
        {
            return GAMEREGIONMENU_ITEM_REGION3;

            break;
        }
    default:
        {
            XDBGWRN( APP_TITLE_NAME_A, "CGameRegionMenu::GetGameRegion():Received unknown Game Region from Config Sector!! Region - '%d'", g_XboxSettings.GetGameRegion() );
            XDBGWRN( APP_TITLE_NAME_A, "CGameRegionMenu::GetGameRegion():Setting Game Region to - '%d'", XC_GAME_REGION_NA );

            HRESULT hr = g_XboxSettings.SetGameRegion( XC_GAME_REGION_NA );
            if( FAILED( hr ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CGameRegionMenu::GetGameRegion():Failed to Set the Game Region!! Region - '%d', Error - '0x%0.8X (%d)'", XC_GAME_REGION_NA, hr, hr );
            }

            break;
        }
    }

    return GAMEREGION_NONE;
}


// Set the Game Region using our 'menu-friendly' format
HRESULT CGameRegionMenu::SetGameRegion( DWORD dwGameRegion )
{
    DWORD dwConfigSectorGameRegion = 0;

    switch( dwGameRegion )
    {
    case GAMEREGIONMENU_ITEM_REGION1:
        {
            dwConfigSectorGameRegion = XC_GAME_REGION_NA;

            break;
        }
    case GAMEREGIONMENU_ITEM_REGION2:
        {
            dwConfigSectorGameRegion = XC_GAME_REGION_JAPAN;

            break;
        }
    case GAMEREGIONMENU_ITEM_REGION3:
        {
            dwConfigSectorGameRegion = XC_GAME_REGION_RESTOFWORLD;

            break;
        }
    default:
        {
            XDBGWRN( APP_TITLE_NAME_A, "CGameRegionMenu::SetGameRegion():Invalid Game Region Passed In - '%d'", dwGameRegion );

            return E_INVALIDARG;
        }
    }

    HRESULT hr = g_XboxSettings.SetGameRegion( dwConfigSectorGameRegion );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CGameRegionMenu::SetGameRegion():Failed to set the Game Region!! Region - '%d', Error - '0x%0.8X (%d)'", dwConfigSectorGameRegion, hr, hr );
    }

    return hr;
}
