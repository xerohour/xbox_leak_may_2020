/****************************************************
*** testconfigmenu.cpp
***
*** CPP file for our Test Config Menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** November 27th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "testconfigmenu.h"

// Constructors and Destructor
CTestConfigMenu::CTestConfigMenu( void ) :
m_pXBEInfo( NULL ),
m_eCurrentAction( ENUM_TESTCONFMENU_NOACTION ),
m_uiSelectedConfigItem( 0 ),
m_lNum1Val( 0l ),
m_bMultiConfigs( TRUE )
{
}

CTestConfigMenu::~CTestConfigMenu( void )
{
}

// Initialize the Menu
HRESULT CTestConfigMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CTestConfigMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Test Config Menu (Config 1 of 1)" );

    // Adjust our on-screen menu items
    ClearScreenItems();

    /////////////////////////
    // Add screen items here
    /////////////////////////
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );

    // Status Bar Column 1
    AddTextItem( GetButtonFont(), L"A",       gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_A_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Select", gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"B",       gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_B_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Back",   gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

    // Status Bar Column 2
    AddTextItem( GetButtonFont(), L"C",              gc_fSTATUS_TEXT_COL2_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_X_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Delete Config", gc_fSTATUS_TEXT_COL2_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"D",              gc_fSTATUS_TEXT_COL2_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_Y_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Create Config", gc_fSTATUS_TEXT_COL2_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

    // Status Bar Column 3
    AddTextItem( GetButtonFont(), L"E",                gc_fSTATUS_TEXT_COL3_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_L_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Previous Config", gc_fSTATUS_TEXT_COL3_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"F",                gc_fSTATUS_TEXT_COL3_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_R_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Next Config",     gc_fSTATUS_TEXT_COL3_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

    return hr;
}


// Renders to the texture and calls any per-frame processing
void CTestConfigMenu::Action( CUDTexture* pTexture )
{
    // Make sure we handle any user initiated actions
    ProcessMenuActions();

    if( ENUM_TESTCONFMENU_NOACTION != m_eCurrentAction )
        return;

    // Call the base class function
    CMenuScreen::Action( pTexture );
}


// Render the screen items properly on our menu
void CTestConfigMenu::RenderScreenItems( CUDTexture* pTexture )
{
    // Don't render anything if we don't have any screen items
    if( 0 == GetNumScreenItems() )
        return;

    m_ScreenItems.MoveFirst(); // Start at the 1st Item
    for( unsigned int x = 0; x < GetNumScreenItems(); ++x )
    {
        m_ScreenItems.GetCurrentNode()->Render( pTexture );
        m_ScreenItems.MoveNext();


        // If we don't support multiple configs, only render our first two buttons
        if( ( !m_bMultiConfigs ) && ( x == 4 ) )
            break;
    }
}


// Render the menu title on to the menu
void CTestConfigMenu::RenderMenuTitle( CUDTexture* pTexture )
{
    // Render the App Title here
    XFONT_SetTextHeight( GetHeaderFont(), 25 );
    pTexture->DrawText( GetHeaderFont(), gc_fLEFT_MENU_AREA_BORDER, 30, gc_dwCOLOR_TEXT_YELLOW, gc_dwCOLOR_DARK_GREY, L"HVS Launcher" );
    XFONT_SetTextHeight( GetHeaderFont(), gc_uiDEFAULT_FONT_HEIGHT );

	if( m_bMultiConfigs )
		pTexture->DrawText( GetHeaderFont(), gc_fLEFT_MENU_AREA_BORDER, 60, gc_dwCOLOR_TEXT_GREEN, gc_dwCOLOR_DARK_GREY, L"%ls (Config %u of %u)", GetMenuTitle(), m_pXBEInfo->GetCurrentTestConfigIndex() + 1, m_pXBEInfo->GetNumTestConfigs() );
	else
		pTexture->DrawText( GetHeaderFont(), gc_fLEFT_MENU_AREA_BORDER, 60, gc_dwCOLOR_TEXT_GREEN, gc_dwCOLOR_DARK_GREY, L"%ls", GetMenuTitle() );
}


// Render the menu items properly on our menu
void CTestConfigMenu::RenderMenuItems( CUDTexture* pTexture )
{
    // Don't render anything if we don't have any menu items
    if( 0 == GetNumMenuItems() )
        return;

    m_MenuItems.MoveTo( GetTopItemIndex() );    // Start at menu item that needs to be displayed
    for( unsigned int x = GetTopItemIndex(); x <= GetBottomItemIndex(); ++x )
    {
        // Determine where on the screen the menu item should be
        float fXPos = m_fLeftMenuAreaBoundary + m_fLeftMenuAreaBorder;
        float fYPos = m_fTopMenuAreaBoundary + m_fTopMenuAreaBorder + ( ( m_fMenuItemVertSpace + m_uiMenuItemFontHeight ) * ( x - GetTopItemIndex() ) );

        // Determine the color of the item
        DWORD dwItemColorFG, dwItemColorBG;

        // Distinguish between enabled and disabled items
        if( m_MenuItems.GetCurrentNode()->m_bEnabled )
        {
            // Check to see if it's highlighted
            if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
            {
                if( x == GetSelectedItem() )
                {
                    dwItemColorFG = m_dwSelectedItemHLFGColor;
                    dwItemColorBG = m_dwSelectedItemHLBGColor;
                }
                else
                {
                    dwItemColorFG = m_dwMenuItemHighlightedFGColor;
                    dwItemColorBG = m_dwMenuItemHighlightedBGColor;
                }
            }
            else
            {
                if( x == GetSelectedItem() )
                {
                    dwItemColorFG = m_dwSelectedItemFGColor;
                    dwItemColorBG = m_dwSelectedItemBGColor;
                }
                else
                {
                    dwItemColorFG = m_dwMenuItemEnabledFGColor;
                    dwItemColorBG = m_dwMenuItemEnabledBGColor;
                }
            }
        }
        else
        {
            dwItemColorFG = m_dwMenuItemDisabledFGColor;
            dwItemColorBG = m_dwMenuItemDisabledBGColor;
        }

        pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, dwItemColorFG, dwItemColorBG, m_MenuItems.GetCurrentNode()->m_pwszItemName );

        WCHAR pwszMenuValue[MAX_PATH+1];
        pwszMenuValue[MAX_PATH] = L'\0';

        CTestConfig* pTestConfig = m_pXBEInfo->GetCurrentTestConfig();  // Store our current test config in a handy pointer

        if( 0 == _wcsicmp( pTestConfig->GetConfigItem( x )->GetType(), L"number" ) )
        {
            CConfigNumber* pConfigNumber = (CConfigNumber*)pTestConfig->GetConfigItem( x );

            _snwprintf( pwszMenuValue, MAX_PATH, L"%ld", pConfigNumber->GetDefault() );
        }
        else if( 0 == _wcsicmp( pTestConfig->GetConfigItem( x )->GetType(), L"range" ) )
        {
            CConfigRange* pConfigRange = (CConfigRange*)pTestConfig->GetConfigItem( x );

            _snwprintf( pwszMenuValue, MAX_PATH, L"%ld to %ld", pConfigRange->GetDefault1(), pConfigRange->GetDefault2() );
        }
        else if( 0 == _wcsicmp( pTestConfig->GetConfigItem( x )->GetType(), L"string" ) )
        {
            CConfigString* pConfigString = (CConfigString*)pTestConfig->GetConfigItem( x );

            _snwprintf( pwszMenuValue, MAX_PATH, L"%ls", pConfigString->GetCurrentString() );
        }
        else if( 0 == _wcsicmp( pTestConfig->GetConfigItem( x )->GetType(), L"string2" ) )
        {
            CConfigString2* pConfigString2 = (CConfigString2*)pTestConfig->GetConfigItem( x );

            _snwprintf( pwszMenuValue, MAX_PATH, L"%ls", pConfigString2->GetString() );
        }
        else if( 0 == _wcsicmp( pTestConfig->GetConfigItem( x )->GetType(), L"numberstring" ) )
        {
            CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pTestConfig->GetConfigItem( x );

            _snwprintf( pwszMenuValue, MAX_PATH, L"%ld %ls", pConfigNumberString->GetDefault(), pConfigNumberString->GetCurrentString() );
        }

        // Draw the value right justified
        fXPos = m_fRightMenuAreaBoundary - GetStringPixelWidth( m_MenuItems.GetCurrentNode()->m_pFont, pwszMenuValue ) - m_fRightMenuAreaBorder;
        pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, dwItemColorFG, dwItemColorBG, pwszMenuValue );
        
        m_MenuItems.MoveNext();
    }
}


// Handles input from the buttons
void CTestConfigMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Controls being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            CConfigItem* pConfigItem = m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

            // Check to see if our value is a string2.  If so, launch the keyboard
            if( 0 == wcscmp( pConfigItem->GetType(), L"string2" ) )
            {
                CConfigString2* pConfigString2 = (CConfigString2*)pConfigItem;
                m_uiSelectedConfigItem = GetSelectedItem();

                m_eCurrentAction = ENUM_TESTCONFMENU_EDITSTRING2;

                WCHAR pwszText[MAX_PATH+1];
                pwszText[MAX_PATH] = '\0';

                _snwprintf( pwszText, MAX_PATH, L"Enter the value for '%ls'", pConfigString2->GetFieldName() );

                GetKeyboardPtr()->SetBackPtr( this );
                GetKeyboardPtr()->SetInfo( pwszText, 40, pConfigString2->GetString() );

                Leave( GetKeyboardPtr() );
            }
            else if( 0 == wcscmp( pConfigItem->GetType(), L"numberstring" ) )
            {
                CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pConfigItem;
                m_uiSelectedConfigItem = GetSelectedItem();

                m_eCurrentAction = ENUM_TESTCONFMENU_EDIT_NUMBERSTRING;

                WCHAR pwszText[MAX_PATH+1];
                WCHAR pwszValText[MAX_PATH+1];
                pwszText[MAX_PATH] = '\0';
                pwszValText[MAX_PATH] = L'\0';

                _snwprintf( pwszText, MAX_PATH, L"Enter a number between %ld and %ld.", pConfigNumberString->GetMinimum(), pConfigNumberString->GetMaximum() );
                _snwprintf( pwszValText, MAX_PATH, L"%ld", pConfigNumberString->GetDefault() );

                GetKeypadPtr()->SetBackPtr( this );
                GetKeypadPtr()->SetInfo( pwszText, 15, pwszValText );

                Leave( GetKeypadPtr() );
            }
            else if( 0 == wcscmp( pConfigItem->GetType(), L"number" ) )
            {
                CConfigNumber* pConfigNumber = (CConfigNumber*)pConfigItem;
                m_uiSelectedConfigItem = GetSelectedItem();

                m_eCurrentAction = ENUM_TESTCONFMENU_EDIT_NUMBER;

                WCHAR pwszText[MAX_PATH+1];
                WCHAR pwszValText[MAX_PATH+1];
                pwszText[MAX_PATH] = '\0';
                pwszValText[MAX_PATH] = L'\0';

                _snwprintf( pwszText, MAX_PATH, L"Enter a number between %ld and %ld.", pConfigNumber->GetMinimum(), pConfigNumber->GetMaximum() );
                _snwprintf( pwszValText, MAX_PATH, L"%ld", pConfigNumber->GetDefault() );

                GetKeypadPtr()->SetBackPtr( this );
                GetKeypadPtr()->SetInfo( pwszText, 15, pwszValText );

                Leave( GetKeypadPtr() );
            }
            else if( 0 == wcscmp( pConfigItem->GetType(), L"range" ) )
            {
                CConfigRange* pConfigRange = (CConfigRange*)pConfigItem;
                m_uiSelectedConfigItem = GetSelectedItem();

                m_eCurrentAction = ENUM_TESTCONFMENU_EDIT_RANGE1;

                WCHAR pwszText[MAX_PATH+1];
                WCHAR pwszValText[MAX_PATH+1];
                pwszText[MAX_PATH] = L'\0';
                pwszValText[MAX_PATH] = L'\0';

                _snwprintf( pwszText, MAX_PATH, L"Enter #1 (between %ld and %ld)", pConfigRange->GetMinimum1(), pConfigRange->GetMaximum1() );
                _snwprintf( pwszValText, MAX_PATH, L"%ld", pConfigRange->GetDefault1() );

                GetKeypadPtr()->SetBackPtr( this );
                GetKeypadPtr()->SetInfo( pwszText, 15, pwszValText );

                Leave( GetKeypadPtr() );
            }
            break;
        }
    case BUTTON_X:
        {
            if( !m_bMultiConfigs )
                break;

            UpdateTexture();

            // Delete configuration
            m_pXBEInfo->DeleteCurrentTestConfig();

            break;
        }
    case BUTTON_Y:
        {
            if( !m_bMultiConfigs )
                break;

            UpdateTexture();

            // Create New Configuration
            m_pXBEInfo->AddNewTestConfig();

            break;
        }
    case BUTTON_B:
        {
            GoBack();

            break;
        }
    case BUTTON_RIGHT_TRIGGER:
        {
            if( !m_bMultiConfigs )
                break;

            UpdateTexture();
            
            // Move to the next test configuration
            m_pXBEInfo->MoveNextTestConfig();

            break;
        }
    case BUTTON_LEFT_TRIGGER:
        {
            if( !m_bMultiConfigs )
                break;

            UpdateTexture();

            // Move to the previous test configuration
            m_pXBEInfo->MovePrevTestConfig();

            break;
        }
    }
}

// Handle any requests for the joystick (thumb-pad)
void CTestConfigMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
{
    switch( joystick )
    {
    // This is the LEFT Thumb Joystick on the controller
    case JOYSTICK_LEFT:
        {
            CMenuScreen::HandleInput( joystick, nThumbY, nThumbX, bFirstYPress, bFirstXPress );

            break;
        }
    case JOYSTICK_RIGHT:
        {
            // Yxc
            if( nThumbY < 0 )  // Move the right joystick down
            {

                // Process the default button delay
                if( !HandleInputDelay( bFirstYPress ) )
                    return;
                
                CConfigItem* pConfigItem = m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

                // Check to see what type of item we are dealing with
                if( 0 == _wcsicmp( pConfigItem->GetType(), L"range" ) )
                {
                    UpdateTexture();

                    CConfigRange* pConfigRange = (CConfigRange*)pConfigItem;

                    // Process the second part of a range
                    long lScaler = ThumbStickScale( nThumbY,
                                                    gc_lTESTCONFIG_NUMBER_BUFZONE,
                                                    pConfigRange->GetMaximum2() / gc_lTESTCONFIG_NUMBER_MAXSCALE_DIVISOR );

                    if( ( pConfigRange->GetDefault2() - lScaler ) < pConfigRange->GetMinimum2() )
                        pConfigRange->SetDefault2( pConfigRange->GetMaximum2() );
                    else
                        pConfigRange->SetDefault2( pConfigRange->GetDefault2() - lScaler );
                }
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"numberstring" ) )
                {
                    UpdateTexture();

                    CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pConfigItem;

                    // Process the string part of a number string
                    if( !pConfigNumberString->MovePrevString() )
                        pConfigNumberString->MoveLastString();
                }
            }
            else if( nThumbY > 0 ) // Move right joystick up
            {
                // Process the default button delay
                if( !HandleInputDelay( bFirstYPress ) )
                    return;

                CConfigItem* pConfigItem = m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

                // Check to see what type of item we are dealing with
                if( 0 == _wcsicmp( pConfigItem->GetType(), L"range" ) )
                {
                    UpdateTexture();

                    CConfigRange* pConfigRange = (CConfigRange*)pConfigItem;

                    // Process the second part of a range
                    long lScaler = ThumbStickScale( nThumbY,
                                                    gc_lTESTCONFIG_NUMBER_BUFZONE,
                                                    pConfigRange->GetMaximum2() / gc_lTESTCONFIG_NUMBER_MAXSCALE_DIVISOR );

                    if( ( pConfigRange->GetDefault2() + lScaler ) > pConfigRange->GetMaximum2() )
                        pConfigRange->SetDefault2( pConfigRange->GetMinimum2() );
                    else
                        pConfigRange->SetDefault2( pConfigRange->GetDefault2() + lScaler );
                }
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"numberstring" ) )
                {
                    UpdateTexture();

                    CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pConfigItem;

                    // Process the string part of a number string
                    if( !pConfigNumberString->MoveNextString() )
                        pConfigNumberString->MoveFirstString();
                }
            }

            // X
            if( nThumbX < 0 )       // Move the right joystick left
            {
                CConfigItem* pConfigItem = m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

                if( 0 == _wcsicmp( pConfigItem->GetType(), L"range" ) )
                {
                    // Process the default button delay
                    if( !HandleInputDelay( bFirstXPress ) )
                        return;
                
                    UpdateTexture();

                    CConfigRange* pConfigRange = (CConfigRange*)pConfigItem;

                    // Process the first part of a range
                    long lScaler = ThumbStickScale( nThumbX,
                                                    gc_lTESTCONFIG_NUMBER_BUFZONE,
                                                    pConfigRange->GetMaximum1() / gc_lTESTCONFIG_NUMBER_MAXSCALE_DIVISOR );

                    if( ( pConfigRange->GetDefault1() - lScaler ) < pConfigRange->GetMinimum1() )
                        pConfigRange->SetDefault1( pConfigRange->GetMaximum1() );
                    else
                        pConfigRange->SetDefault1( pConfigRange->GetDefault1() - lScaler );

                }
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"number" ) )
                {
                    // Process the default button delay
                    if( !HandleInputDelay( bFirstXPress ) )
                        return;
                
                    UpdateTexture();
                    
                    CConfigNumber* pConfigNumber = (CConfigNumber*)pConfigItem;

                    long lScaler = ThumbStickScale( nThumbX,
                                                    gc_lTESTCONFIG_NUMBER_BUFZONE,
                                                    pConfigNumber->GetMaximum() / gc_lTESTCONFIG_NUMBER_MAXSCALE_DIVISOR ); // gc_lTESTCONFIG_NUMBER_MAXSCALE );

                    if( ( pConfigNumber->GetDefault() - lScaler ) < pConfigNumber->GetMinimum() )
                        pConfigNumber->SetDefault( pConfigNumber->GetMaximum() );
                    else
                        pConfigNumber->SetDefault( pConfigNumber->GetDefault() - lScaler );
                }
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"numberstring" ) )
                {
                    // Process the default button delay
                    if( !HandleInputDelay( bFirstXPress ) )
                        return;
                
                    UpdateTexture();
                    
                    CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pConfigItem;

                    long lScaler = ThumbStickScale( nThumbX,
                                                    gc_lTESTCONFIG_NUMBER_BUFZONE,
                                                    pConfigNumberString->GetMaximum() / gc_lTESTCONFIG_NUMBER_MAXSCALE_DIVISOR ); // gc_lTESTCONFIG_NUMBER_MAXSCALE );

                    // Handle the number part of our number string
                    if( ( pConfigNumberString->GetDefault() - lScaler ) < pConfigNumberString->GetMinimum() )
                        pConfigNumberString->SetDefault( pConfigNumberString->GetMaximum() );
                    else
                      pConfigNumberString->SetDefault( pConfigNumberString->GetDefault() - lScaler );
                }
                else // Hand off this to the "DPad" handler
                {
                    HandleInput( CONTROL_DPAD_LEFT, bFirstXPress );
                }
            }
            else if( nThumbX > 0 )  // Move the right joystick right
            {
                CConfigItem* pConfigItem = m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

                if( 0 == _wcsicmp( pConfigItem->GetType(), L"range" ) )
                {
                    // Process the default button delay
                    if( !HandleInputDelay( bFirstXPress ) )
                        return;

                    UpdateTexture();

                    CConfigRange* pConfigRange = (CConfigRange*)pConfigItem;

                    // Process the first part of a range
                    long lScaler = ThumbStickScale( nThumbX,
                                                    gc_lTESTCONFIG_NUMBER_BUFZONE,
                                                    pConfigRange->GetMaximum1() / gc_lTESTCONFIG_NUMBER_MAXSCALE_DIVISOR );

                    if( ( pConfigRange->GetDefault1() + lScaler ) > pConfigRange->GetMaximum1() )
                        pConfigRange->SetDefault1( pConfigRange->GetMinimum1() );
                    else
                        pConfigRange->SetDefault1( pConfigRange->GetDefault1() + lScaler );

                }
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"number" ) )
                {
                    // Process the default button delay
                    if( !HandleInputDelay( bFirstXPress ) )
                        return;

                    UpdateTexture();

                    CConfigNumber* pConfigNumber = (CConfigNumber*)pConfigItem;

                    // Process the first part of a range
                    long lScaler = ThumbStickScale( nThumbX,
                                                    gc_lTESTCONFIG_NUMBER_BUFZONE,
                                                    pConfigNumber->GetMaximum() / gc_lTESTCONFIG_NUMBER_MAXSCALE_DIVISOR );

                    if( ( pConfigNumber->GetDefault() + lScaler ) > pConfigNumber->GetMaximum() )
                        pConfigNumber->SetDefault( pConfigNumber->GetMinimum() );
                    else
                        pConfigNumber->SetDefault( pConfigNumber->GetDefault() + lScaler );
                }
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"numberstring" ) )
                {
                    // Process the default button delay
                    if( !HandleInputDelay( bFirstXPress ) )
                        return;

                    UpdateTexture();

                    CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pConfigItem;

                    // Process the first part of a range
                    long lScaler = ThumbStickScale( nThumbX,
                                                    gc_lTESTCONFIG_NUMBER_BUFZONE,
                                                    pConfigNumberString->GetMaximum() / gc_lTESTCONFIG_NUMBER_MAXSCALE_DIVISOR );

                    if( ( pConfigNumberString->GetDefault() + lScaler ) > pConfigNumberString->GetMaximum() )
                        pConfigNumberString->SetDefault( pConfigNumberString->GetMinimum() );
                    else
                      pConfigNumberString->SetDefault( pConfigNumberString->GetDefault() + lScaler );
                }
                else // Hand off this to the "DPad" handler
                {
                    HandleInput( CONTROL_DPAD_RIGHT, bFirstXPress );
                }
            }
            break;
        }
    }
}


// Handle control input for a single list menu
void CTestConfigMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    // Pass off UP/DOWN navigation to our base class
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
    case CONTROL_DPAD_DOWN:
        {
            CMenuScreen::HandleInput( controlPressed, bFirstPress );
            break;
        }
    }

    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Controls being pressed
    switch( controlPressed )
    {
    case CONTROL_BACK:
        {
            GoToHelp();

            break;
        }
    case CONTROL_DPAD_LEFT:
        {
            // Only act if we have any items on our screen
            if( GetNumMenuItems() > 0 )
            {
                UpdateTexture();

                CConfigItem* pConfigItem = m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

                // Determine what type of item we are dealing with
                /*
                if( 0 == _wcsicmp( pConfigItem->GetType(), L"number" ) )
                {
                    CConfigNumber* pConfigNumber = (CConfigNumber*)pConfigItem;

                    if( pConfigNumber->GetDefault() == pConfigNumber->GetMinimum() )
                        pConfigNumber->SetDefault( pConfigNumber->GetMaximum() );
                    else
                      pConfigNumber->SetDefault( pConfigNumber->GetDefault() - 1 );
                }
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"range" ) )
                {
                    CConfigRange* pConfigRange = (CConfigRange*)pConfigItem;

                    // Process the first part of a range
                    if( pConfigRange->GetDefault1() == pConfigRange->GetMinimum1() )
                        pConfigRange->SetDefault1( pConfigRange->GetMaximum1() );
                    else
                        pConfigRange->SetDefault1( pConfigRange->GetDefault1() - 1 );
                }
                */
                if( 0 == _wcsicmp( pConfigItem->GetType(), L"string" ) )
                {
                    CConfigString* pConfigString = (CConfigString*)pConfigItem;

                    if( !pConfigString->MovePrevString() )
                        pConfigString->MoveLastString();
                }
                /*
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"numberstring" ) )
                {
                    // Handle the number part of our number string

                    CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pConfigItem;

                    if( pConfigNumberString->GetDefault() == pConfigNumberString->GetMinimum() )
                        pConfigNumberString->SetDefault( pConfigNumberString->GetMaximum() );
                    else
                      pConfigNumberString->SetDefault( pConfigNumberString->GetDefault() - 1 );
                }
                */
            }
            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
            // Only act if we have any items on our screen
            if( GetNumMenuItems() != 0 )
            {
                UpdateTexture();

                CConfigItem* pConfigItem = m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

                // Determine what type of item we are dealing with
                /*
                if( 0 == _wcsicmp( pConfigItem->GetType(), L"number" ) )
                {
                    CConfigNumber* pConfigNumber = (CConfigNumber*)pConfigItem;

                    if( pConfigNumber->GetDefault() == pConfigNumber->GetMaximum() )
                        pConfigNumber->SetDefault( pConfigNumber->GetMinimum() );
                    else
                      pConfigNumber->SetDefault( pConfigNumber->GetDefault() + 1 );
                }
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"range" ) )
                {
                    CConfigRange* pConfigRange = (CConfigRange*)pConfigItem;

                    // Process the first part of a range
                    if( pConfigRange->GetDefault1() == pConfigRange->GetMaximum1() )
                        pConfigRange->SetDefault1( pConfigRange->GetMinimum1() );
                    else
                        pConfigRange->SetDefault1( pConfigRange->GetDefault1() + 1 );
                }
                */
                if( 0 == _wcsicmp( pConfigItem->GetType(), L"string" ) )
                {
                    CConfigString* pConfigString = (CConfigString*)pConfigItem;

                    if( !pConfigString->MoveNextString() )
                        pConfigString->MoveFirstString();
                }
                /*
                else if( 0 == _wcsicmp( pConfigItem->GetType(), L"numberstring" ) )
                {
                    CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pConfigItem;

                    if( pConfigNumberString->GetDefault() == pConfigNumberString->GetMaximum() )
                        pConfigNumberString->SetDefault( pConfigNumberString->GetMinimum() );
                    else
                      pConfigNumberString->SetDefault( pConfigNumberString->GetDefault() + 1 );
                }
                */
            }
            break;
        }
	}
}


// This should be called whenever the menu is entered or activated
void CTestConfigMenu::Enter( void )
{
    CMenuScreen::Enter();

    if( NULL == m_pXBEInfo )
    {
        DebugPrint( "Enter():Invalid XBE Pointer set!!\n" );

        GoBack();
    }

    CTestConfig* pTestConfig = m_pXBEInfo->GetCurrentTestConfig();  // Store our config item in a handy pointer
    // CTestConfig* pTestConfig = m_pXBEInfo->GetDefaultTestConfig();  // Store our config item in a handy pointer

    pTestConfig->MoveFirstConfigItem();
    unsigned int uiCount = 0;
    
    do
    {
        AddMenuItem( GetBodyFont(), pTestConfig->GetCurrentConfigItem()->GetFieldName(), FALSE, TRUE, uiCount );
        uiCount++;
    } while( pTestConfig->MoveNextConfigItem() );

    // Adjust our selected item if necessary
    // Set our Selected Item back to the proper config setting
    SetSelectedItem( m_uiSelectedConfigItem );
    m_uiSelectedConfigItem = 0;

    if( GetSelectedItem() < GetNumMenuItemsToDisplay() )
        SetTopItemIndex( 0 );
    else if( ( GetSelectedItem() + GetNumMenuItemsToDisplay() ) >= GetNumMenuItems() )
        SetTopItemIndex( GetNumMenuItems() - GetNumMenuItemsToDisplay() );
    else // It can be our top item
        SetTopItemIndex( GetSelectedItem() );

    /*
    DebugPrint( "TopItemIndex    - '%d'\n", GetTopItemIndex() );
    DebugPrint( "BottomItemIndex - '%d'\n", GetBottomItemIndex() );
    DebugPrint( "SelectedItem    - '%u'\n", GetSelectedItem() );
    DebugPrint( "SelectorPos     - '%u'\n", GetSelectorPosition() );
    */
}


// Should be called whenever user is leaving the menu
void CTestConfigMenu::Leave( CXItem* pItem )
{
    ClearMenuItems();
    
    CMenuScreen::Leave( pItem );
};


// If the user is performing an action, this will handle it
void CTestConfigMenu::ProcessMenuActions( void )
{
    switch( m_eCurrentAction )
    {
    case ENUM_TESTCONFMENU_EDITSTRING2:
        {
            // Make sure we set this to no action
            m_eCurrentAction = ENUM_TESTCONFMENU_NOACTION;

            // Check to see if the user cancelled the operation.  If not, store the new value
            if( !GetKeyboardPtr()->GetCancelled() )
            {
                CConfigString2* pConfigString2 = (CConfigString2*)m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );

                pConfigString2->SetString( GetKeyboardPtr()->GetValue() );
            }

            break;
        }
    case ENUM_TESTCONFMENU_EDIT_RANGE1:
        {
            if( !GetKeypadPtr()->GetCancelled() )
            {
                CConfigRange* pConfigRange = (CConfigRange*)m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer
                m_uiSelectedConfigItem = GetSelectedItem();

                m_eCurrentAction = ENUM_TESTCONFMENU_EDIT_RANGE2;

                // Store the first number the user entered
                WCHAR* pwszStopString = NULL;
                m_lNum1Val = wcstol( GetKeypadPtr()->GetValue(), &pwszStopString, 10 );

                WCHAR pwszText[MAX_PATH+1];
                WCHAR pwszValText[MAX_PATH+1];
                pwszText[MAX_PATH] = L'\0';
                pwszValText[MAX_PATH] = L'\0';

                _snwprintf( pwszText, MAX_PATH, L"Enter #2 (between %ld and %ld)", pConfigRange->GetMinimum2(), pConfigRange->GetMaximum2() );
                _snwprintf( pwszValText, MAX_PATH, L"%ld", pConfigRange->GetDefault2() );

                DebugPrint( "pwszValText - '%ls'\n", pwszValText );

                GetKeypadPtr()->SetBackPtr( this );
                GetKeypadPtr()->SetInfo( pwszText, 15, pwszValText );

                Leave( GetKeypadPtr() );
            }
            else    // User cancelled
                m_eCurrentAction = ENUM_TESTCONFMENU_NOACTION;  // Make sure we set this to no action

            break;
        }
    case ENUM_TESTCONFMENU_EDIT_RANGE2:
        {
            m_eCurrentAction = ENUM_TESTCONFMENU_NOACTION;  // Make sure we set this to no action

            if( !GetKeypadPtr()->GetCancelled() )
            {
                CConfigRange* pConfigRange = (CConfigRange*)m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

                // Get the second number the user entered
                WCHAR* pwszStopString = NULL;
                long lVal2 = wcstol( GetKeypadPtr()->GetValue(), &pwszStopString, 10 );

                // Set our range values
                pConfigRange->SetDefault1( m_lNum1Val );
                pConfigRange->SetDefault2( lVal2 );

                m_lNum1Val = 0l;                
            }

            break;
        }
    case ENUM_TESTCONFMENU_EDIT_NUMBERSTRING:
        {
            m_eCurrentAction = ENUM_TESTCONFMENU_NOACTION;  // Make sure we set this to no action

            if( !GetKeypadPtr()->GetCancelled() )
            {
                CConfigNumberString* pConfigNumberString = (CConfigNumberString*)m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

                // Get the second number the user entered
                WCHAR* pwszStopString = NULL;
                long lVal1 = wcstol( GetKeypadPtr()->GetValue(), &pwszStopString, 10 );

                // Set our range values
                pConfigNumberString->SetDefault( lVal1 );
            }

            break;
        }
    case ENUM_TESTCONFMENU_EDIT_NUMBER:
        {
            m_eCurrentAction = ENUM_TESTCONFMENU_NOACTION;  // Make sure we set this to no action

            if( !GetKeypadPtr()->GetCancelled() )
            {
                CConfigNumber* pConfigNumber = (CConfigNumber*)m_pXBEInfo->GetCurrentTestConfig()->GetConfigItem( GetSelectedItem() );  // Store our config item in a handy pointer

                // Get the second number the user entered
                WCHAR* pwszStopString = NULL;
                long lVal1 = wcstol( GetKeypadPtr()->GetValue(), &pwszStopString, 10 );

                // Set our range values
                pConfigNumber->SetDefault( lVal1 );
            }

            break;
        }
    }
}


// Set the PTR to our active XBEInfo object
void CTestConfigMenu::SetXBEInfoPtr( CXBEInfo* pXBEInfo )
{
    m_pXBEInfo = pXBEInfo;
};


// Sets up and goes to the help menu
void CTestConfigMenu::GoToHelp( void )
{
    // Set the menu title
    WCHAR pwszTitle[50];
    pwszTitle[49] = L'\0';

    _snwprintf( pwszTitle, 49, L"%ls Help", m_pXBEInfo->GetTitleName() );
    GetHelpMenuPtr()->SetMenuTitle( GetHeaderFont(), pwszTitle );

    // Set the back pointer for the help menu
    GetHelpMenuPtr()->SetBackPtr( this );

    // Clear the current help
    GetHelpMenuPtr()->ClearHelpItems();

    // Populate the help menu with our items
    m_pXBEInfo->GetCurrentTestConfig()->MoveFirstConfigItem();
    do
    {
        GetHelpMenuPtr()->AddHelpItem( GetBodyFont(),
                                       m_pXBEInfo->GetCurrentTestConfig()->GetCurrentConfigItem()->GetFieldName(),
                                       GetBodyFont(),
                                       m_pXBEInfo->GetCurrentTestConfig()->GetCurrentConfigItem()->GetHelpText() );
    } while( m_pXBEInfo->GetCurrentTestConfig()->MoveNextConfigItem() );
    
    // navigate to the menu
    Leave( GetHelpMenuPtr() );
}