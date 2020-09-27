/*****************************************************
*** avregionmenu.cpp
***
*** CPP file for our AV Region menu class.  This
*** class will allow a user to select a new AV
*** region
***
*** by James Helm
*** May 11th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "avregionmenu.h"

extern CXSettings g_XboxSettings;   // Xbox Settings Object

// Constructor
CAVRegionMenu::CAVRegionMenu( CXItem* pParent )
: CKeypadMenu( pParent )
{
    XDBGTRC( APP_TITLE_NAME_A, "CAVRegionMenu::CAVRegionMenu( CXItem )" );

    // Set the number of items on the menu
    m_uiNumItems = AVREGIONMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

	GenerateKeypad();

    // Default the key to the TOP Button
	m_xKeypos = 0;
	m_yKeypos = 0;
}


// Destructor
CAVRegionMenu::~CAVRegionMenu()
{
}

// Draws a menu on to the screen
void CAVRegionMenu::Action( CUDTexture* pTexture )
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
	// Render the current AV Region Text
    //
    int iStringPixelWidth = GetStringPixelWidth( MENU_AVREGION_CURRENT_REGION_TEXT );
    float fXPos = SCREEN_X_CENTER - ( iStringPixelWidth / 2.0f );
    float fYPos = MENU_AVREGION_TEXT_Y1;
    pTexture->DrawText( fXPos, fYPos, SCREEN_INACTIVE_OPTION_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", MENU_AVREGION_CURRENT_REGION_TEXT );

    iStringPixelWidth = GetStringPixelWidth( g_wpszAVRegionMenuItems[GetAVRegion()] );
    fXPos = SCREEN_X_CENTER - ( iStringPixelWidth / 2.0f );
    unsigned int uiFontHeight, uiFontWidth;
    GetFontSize( &uiFontHeight, &uiFontWidth );
    fYPos = MENU_AVREGION_TEXT_Y1 + uiFontHeight + MENUBOX_ITEM_VERT_SPACING;
    pTexture->DrawText( fXPos, fYPos, SCREEN_ACTIVE_OPTION_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", g_wpszAVRegionMenuItems[GetAVRegion()] );
    
    //
    // Draw our keypad / AV Region Menu Items
    //
	renderKeypad( pTexture );

    // Unlock our Texture
    pTexture->Unlock();
}

// Handles input (of the BUTTONS) for the current menu
void CAVRegionMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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

                // Set the AV Region in to the Config Sector
                HRESULT hr = SetAVRegion( m_yKeypos ); // m_yKeypos will map to the enum AVRegionMenu items
                if( FAILED( hr ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CAVRegionMenu::HandleInput():Failed to set the AV Region!! Region - '%d', Error - '0x%X.08'", m_yKeypos, hr );
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
                Leave( GetParent() );
            }
            break;
        }
    }
}

// Handles input (of the CONTROLS) for the current menu
void CAVRegionMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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

                m_yKeypos--;
            }
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            if( m_yKeypos < ( AVREGIONMENU_ITEM_NUM_ITEMS - 1 ) )
            {
                m_bUpdateTexture = TRUE;

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


void CAVRegionMenu::GenerateKeypad(void) 
{
	// First create numberpad
	int xgrid  = MENU_AVREGION_NUM_COLS;
	int ygrid  = MENU_AVREGION_NUM_ROWS;
	int xstart = MENU_AVREGION_BUTTON_X1;
	int ystart = MENU_AVREGION_BUTTON_Y1;
	int width  = MENU_AVREGION_BUTTON_WIDTH;
	int height = 32;
	int spacer = 2;

	int currentx = xstart;
	int currenty = ystart;

	for(int y = 0; y < ygrid; y++)
	{
		for(int x = 0; x < xgrid; x++)
		{
			m_keyPad[x][y].define( currentx, currenty, width, height, SCREEN_DEFAULT_FOREGROUND_COLOR );
			currentx += width + spacer;
		}
		currentx = xstart;
		currenty += height + spacer;
	}

    // Set the text of the keys
    for( int z = 0; z < AVREGIONMENU_ITEM_NUM_ITEMS; z++ )
    {
        m_keyPad[0][z].defineText(g_wpszAVRegionMenuItems[z]);
    }
}


BOOL CAVRegionMenu::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < MENU_AVREGION_NUM_COLS; x++)
	{
		for(int y = 0; y < MENU_AVREGION_NUM_ROWS; y++)
		{
			if(m_keyPad[x][y].getRender())
			{
                // Lock our texture
                pTexture->Lock();

				x1 = (float)m_keyPad[x][y].m_iXOrigin;
				x2 = (float)(m_keyPad[x][y].m_iXOrigin + m_keyPad[x][y].m_iWidth);
				
				y1 = (float)m_keyPad[x][y].m_iYOrigin;
				y2 = (float)(m_keyPad[x][y].m_iYOrigin + m_keyPad[x][y].m_iHeight);
	
				// hilight key if currently selected
                DWORD dwCharColor;
				if((m_xKeypos == x) && (m_yKeypos == y))
                {
					pTexture->DrawBox( x1, y1, x2, y2, SCREEN_DEFAULT_SELECTOR_COLOR );
                    
                    dwCharColor = SCREEN_SELECTED_OPTION_COLOR;
                }
                else
                {
                    dwCharColor = SCREEN_DEFAULT_FOREGROUND_COLOR;
                }

                pTexture->DrawOutline( x1, y1, x2, y2, MENU_AVREGION_LINE_WIDTH, m_keyPad[x][y].m_dwSelectColor );
				
				unsigned int decent;
				unsigned int fontHeight;
				GetFontSize( &fontHeight, &decent );
				
				float textX = (float)((m_keyPad[x][y].m_iWidth / 2) + m_keyPad[x][y].m_iXOrigin) - GetStringPixelWidth( m_keyPad[x][y].m_pwszResultChar ) / 2;
				float textY = (float)((m_keyPad[x][y].m_iHeight / 2) + m_keyPad[x][y].m_iYOrigin) - (fontHeight / 2);
				pTexture->DrawText( textX, textY, dwCharColor, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", m_keyPad[x][y].m_pwszResultChar);

                // Unlock our Texture
                pTexture->Unlock();
			}
		}
	}
	
	return TRUE;
}

// This will be called whenever the user enters this menu
void CAVRegionMenu::Enter()
{
    // Call the base-class enter
    CKeypadMenu::Enter();

    // Default the key to the TOP Button
	m_xKeypos = 0;
	m_yKeypos = GetAVRegion();
}


// Get the AV Region in a 'menu-friendly' format
DWORD CAVRegionMenu::GetAVRegion()
{
    switch( g_XboxSettings.GetAVRegion() )
    {
    case XSETTINGS_DISPLAY_SETTING_NTSC_M:
        {
            return AVREGIONMENU_ITEM_REGION1;

            break;
        }
    case XSETTINGS_DISPLAY_SETTING_NTSC_J:
        {
            return AVREGIONMENU_ITEM_REGION2;

            break;
        }
    case XSETTINGS_DISPLAY_SETTING_PAL_I:
        {
            return AVREGIONMENU_ITEM_REGION3;

            break;
        }
    default:
        {
            XDBGWRN( APP_TITLE_NAME_A, "CAVRegionMenu::GetAVRegion():Received unknown AV Region from Config Sector!! Region - '%d'", g_XboxSettings.GetAVRegion() );
            XDBGWRN( APP_TITLE_NAME_A, "CAVRegionMenu::GetAVRegion():Setting AV Region to - '%d'", XSETTINGS_DISPLAY_SETTING_NTSC_M );

            HRESULT hr = g_XboxSettings.SetAVRegion( XSETTINGS_DISPLAY_SETTING_NTSC_M );
            if( FAILED( hr ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CAVRegionMenu::GetAVRegion():Failed to Set the AV Region!! Region - '%d', Error - '0x%0.8X (%d)'", XSETTINGS_DISPLAY_SETTING_NTSC_M, hr, hr );
            }

            break;
        }
    }

    return XSETTINGS_DISPLAY_SETTING_NONE;
}


// Set the AV Region using our 'menu-friendly' format
HRESULT CAVRegionMenu::SetAVRegion( DWORD dwAVRegion )
{
    DWORD dwConfigSectorAVRegion = XSETTINGS_DISPLAY_SETTING_NONE;

    switch( dwAVRegion )
    {
    case AVREGIONMENU_ITEM_REGION1:
        {
            dwConfigSectorAVRegion = XSETTINGS_DISPLAY_SETTING_NTSC_M;

            break;
        }
    case AVREGIONMENU_ITEM_REGION2:
        {
            dwConfigSectorAVRegion = XSETTINGS_DISPLAY_SETTING_NTSC_J;

            break;
        }
    case AVREGIONMENU_ITEM_REGION3:
        {
            dwConfigSectorAVRegion = XSETTINGS_DISPLAY_SETTING_PAL_I;

            break;
        }
    default:
        {
            XDBGWRN( APP_TITLE_NAME_A, "CAVRegionMenu::SetAVRegion():Invalid AV Region Passed In - '%d'", dwAVRegion );

            return E_INVALIDARG;
        }
    }

    HRESULT hr = g_XboxSettings.SetAVRegion( dwConfigSectorAVRegion );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CAVRegionMenu::SetAVRegion():Failed to set the AV Region!! Region - '%d', Error - '0x%0.8X (%d)'", dwConfigSectorAVRegion, hr, hr );
    }

    return hr;
}
