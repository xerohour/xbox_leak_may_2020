/*****************************************************
*** dvdregionmenu.cpp
***
*** CPP file for our DVD Region menu class.  This
*** class will allow a user to select a new DVD
*** region
***
*** by James Helm
*** May 12th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "dvdregionmenu.h"

extern CXSettings g_XboxSettings;   // Xbox Settings Object

// Constructor
CDVDRegionMenu::CDVDRegionMenu( CXItem* pParent )
: CKeypadMenu( pParent )
{
    XDBGTRC( APP_TITLE_NAME_A, "CDVDRegionMenu::CDVDRegionMenu( CXItem )" );

    // Set the number of items on the menu
    m_uiNumItems = DVDREGIONMENU_ITEM_NUM_ITEMS;
    SetSelectedItem( 0 );

	GenerateKeypad();

    // Default the key to the TOP Button
	m_xKeypos = 0;
	m_yKeypos = 0;
}


// Destructor
CDVDRegionMenu::~CDVDRegionMenu()
{
}

// Draws a menu on to the screen
void CDVDRegionMenu::Action( CUDTexture* pTexture )
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
	// Render the current DVD Region Text
    //
    WCHAR pwszDVDRegion[100];
    ZeroMemory( pwszDVDRegion, sizeof( WCHAR ) * 100 );

    _snwprintf( pwszDVDRegion, 99, L"%s%d", MENU_DVDREGION_CURRENT_REGION_TEXT, GetDVDRegion() );
    int iStringPixelWidth = GetStringPixelWidth( pwszDVDRegion );
    float fXPos = SCREEN_X_CENTER - ( iStringPixelWidth / 2.0f );
    float fYPos = MENU_DVDREGION_TEXT_Y1;
    pTexture->DrawText( fXPos, fYPos, SCREEN_INACTIVE_OPTION_COLOR, SCREEN_DEFAULT_BACKGROUND_COLOR, L"%s", pwszDVDRegion );

    //
    // Draw our keypad / DVD Region Menu Items
    //
	renderKeypad( pTexture );

    // Unlock our Texture
    pTexture->Unlock();
}

// Handles input (of the BUTTONS) for the current menu
void CDVDRegionMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
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

                // Set the DVD Region in to the Config Sector
                HRESULT hr = SetDVDRegion( m_yKeypos ); // m_yKeypos will map to the enum DVDRegionMenu items
                if( FAILED( hr ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CDVDRegionMenu::HandleInput():Failed to set the DVD Region!! Region - '%d', Error - '0x%.8X'", m_yKeypos, hr );
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
void CDVDRegionMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
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
            if( m_yKeypos < ( DVDREGIONMENU_ITEM_NUM_ITEMS - 1 ) )
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


void CDVDRegionMenu::GenerateKeypad(void) 
{
	// First create numberpad
	int xgrid  = MENU_DVDREGION_NUM_COLS;
	int ygrid  = MENU_DVDREGION_NUM_ROWS;
	int xstart = MENU_DVDREGION_BUTTON_X1;
	int ystart = MENU_DVDREGION_BUTTON_Y1;
	int width  = MENU_DVDREGION_BUTTON_WIDTH;
	int height = MENU_DVDREGION_BUTTON_HEIGHT;
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
    for( int z = 0; z < DVDREGIONMENU_ITEM_NUM_ITEMS; z++ )
    {
        m_keyPad[0][z].defineText(g_wpszDVDRegionMenuItems[z]);
    }
}


BOOL CDVDRegionMenu::renderKeypad( CUDTexture* pTexture )
{
	
	// Render outlines of keys	
	float x1, x2;
	float y1, y2;
	for(int x = 0; x < MENU_DVDREGION_NUM_COLS; x++)
	{
		for(int y = 0; y < MENU_DVDREGION_NUM_ROWS; y++)
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

                pTexture->DrawOutline( x1, y1, x2, y2, MENU_DVDREGION_LINE_WIDTH, m_keyPad[x][y].m_dwSelectColor );
				
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
void CDVDRegionMenu::Enter()
{
    // Call the base-class enter
    CKeypadMenu::Enter();

    // Default the key to the TOP Button
	m_xKeypos = 0;
	m_yKeypos = GetDVDRegion();
}


// Get the DVD Region in a 'menu-friendly' format
DWORD CDVDRegionMenu::GetDVDRegion()
{
    switch( g_XboxSettings.GetDVDRegion() )
    {
    case XSETTINGS_DVD_REGION0:
        {
            return DVDREGIONMENU_ITEM_REGION0;
            
            break;
        }
    case XSETTINGS_DVD_REGION1:
        {
            return DVDREGIONMENU_ITEM_REGION1;

            break;
        }
    case XSETTINGS_DVD_REGION2:
        {
            return DVDREGIONMENU_ITEM_REGION2;

            break;
        }
    case XSETTINGS_DVD_REGION3:
        {
            return DVDREGIONMENU_ITEM_REGION3;

            break;
        }
    case XSETTINGS_DVD_REGION4:
        {
            return DVDREGIONMENU_ITEM_REGION4;

            break;
        }
    case XSETTINGS_DVD_REGION5:
        {
            return DVDREGIONMENU_ITEM_REGION5;

            break;
        }
    case XSETTINGS_DVD_REGION6:
        {
            return DVDREGIONMENU_ITEM_REGION6;

            break;
        }
    default:
        {
            XDBGWRN( APP_TITLE_NAME_A, "CDVDRegionMenu::GetDVDRegion():Received unknown DVD Region from Config Sector!! Region - '%d'", g_XboxSettings.GetDVDRegion() );
            XDBGWRN( APP_TITLE_NAME_A, "CDVDRegionMenu::GetDVDRegion():Setting DVD Region to - '%d'", DVDREGIONMENU_ITEM_REGION0 );

            HRESULT hr = g_XboxSettings.SetDVDRegion( DVDREGIONMENU_ITEM_REGION0 );
            if( FAILED( hr ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CDVDRegionMenu::GetDVDRegion():Failed to Set the DVD Region!! Region - '%d', Error - '0x%0.8X (%d)'", DVDREGIONMENU_ITEM_REGION0, hr, hr );
            }

            break;
        }
    }

    return DVDREGIONMENU_ITEM_REGION0;
}


// Set the DVD Region using our 'menu-friendly' format
HRESULT CDVDRegionMenu::SetDVDRegion( DWORD dwDVDRegion )
{
    DWORD dwConfigSectorDVDRegion = 0;

    switch( dwDVDRegion )
    {
    case DVDREGIONMENU_ITEM_REGION0:
        {
            dwConfigSectorDVDRegion = XSETTINGS_DVD_REGION0;

            break;
        }
    case DVDREGIONMENU_ITEM_REGION1:
        {
            dwConfigSectorDVDRegion = XSETTINGS_DVD_REGION1;

            break;
        }
    case DVDREGIONMENU_ITEM_REGION2:
        {
            dwConfigSectorDVDRegion = XSETTINGS_DVD_REGION2;

            break;
        }
    case DVDREGIONMENU_ITEM_REGION3:
        {
            dwConfigSectorDVDRegion = XSETTINGS_DVD_REGION3;

            break;
        }
    case DVDREGIONMENU_ITEM_REGION4:
        {
            dwConfigSectorDVDRegion = XSETTINGS_DVD_REGION4;

            break;
        }
    case DVDREGIONMENU_ITEM_REGION5:
        {
            dwConfigSectorDVDRegion = XSETTINGS_DVD_REGION5;

            break;
        }
    case DVDREGIONMENU_ITEM_REGION6:
        {
            dwConfigSectorDVDRegion = XSETTINGS_DVD_REGION6;

            break;
        }
    default:
        {
            XDBGWRN( XSETTINGS_APP_TITLE_NAME_A, "CDVDRegionMenu::SetDVDRegion():Invalid DVD Region Passed In - '%d'", dwDVDRegion );

            return E_INVALIDARG;
        }
    }

    HRESULT hr = g_XboxSettings.SetDVDRegion( dwConfigSectorDVDRegion );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CDVDRegionMenu::SetDVDRegion():Failed to set the DVD Region!! Region - '%d', Error - '0x%0.8X (%d)'", dwConfigSectorDVDRegion, hr, hr );
    }

    return hr;
}
