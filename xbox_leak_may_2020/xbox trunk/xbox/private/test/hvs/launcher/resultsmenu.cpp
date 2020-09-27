/*****************************************************
*** resultsmenu.cpp
***
*** CPP file for our Results Menu class.
*** This class will parse the results file and
*** display the data to the user
***
*** by James N. Helm
*** December 7th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "resultsmenu.h"

// Constructors and Destructor
CResultsMenu::CResultsMenu( void ) :
m_bFileProcessed( FALSE )
{
    m_TestResults.SetDeleteDataItem( TRUE );
}

CResultsMenu::~CResultsMenu( void )
{
    m_TestResults.EmptyList();
}

// Initialize the Menu
HRESULT CResultsMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CResultsMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

    SetMenuAreaBoundaries( gc_fLEFT_MENU_AREA_BOUNDARY,
                           gc_fTOP_MENU_AREA_BOUNDARY + m_uiMenuItemFontHeight + m_fMenuItemVertSpace,
                           gc_fRIGHT_MENU_AREA_BOUNDARY,
                           gc_fBOTTOM_MENU_AREA_BOUNDARY );


	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Test Results" );

    /////////////////////////
    // Add screen items here
    /////////////////////////
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );

    // Status bar column 1
    AddTextItem( GetButtonFont(), L"B",             gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_B_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetBodyFont(),   L" Back",         gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetButtonFont(), L"H",             gc_fSTATUS_TEXT_COL1_XPOS - 5,                       gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_BACK_BUTTON,    gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Test Notes",   gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

    return hr;
}


// Updates the screen items depending upon screen dimensions
void CResultsMenu::UpdateScreenItems( void )
{
    // Header text for results columns
    AddTextItem( GetBodyFont(), L"Test Name", gc_fLEFT_MENU_AREA_BOUNDARY + gc_fLEFT_MENU_AREA_BORDER,                                                       gc_fTOP_MENU_AREA_BOUNDARY + m_fTopMenuAreaBorder, gc_dwCOLOR_TEXT_YELLOW, gc_dwCOLOR_DARK_GREY );
    AddTextItem( GetBodyFont(), L"Passed",    ( m_fRightMenuAreaBoundary - 100 ) - GetStringPixelWidth( GetBodyFont(), L"Passed" ) - m_fRightMenuAreaBorder, gc_fTOP_MENU_AREA_BOUNDARY + m_fTopMenuAreaBorder, gc_dwCOLOR_TEXT_YELLOW, gc_dwCOLOR_DARK_GREY );
    AddTextItem( GetBodyFont(), L"Failed",    m_fRightMenuAreaBoundary - GetStringPixelWidth( GetBodyFont(), L"Failed" ) - m_fRightMenuAreaBorder,           gc_fTOP_MENU_AREA_BOUNDARY + m_fTopMenuAreaBorder, gc_dwCOLOR_TEXT_YELLOW, gc_dwCOLOR_DARK_GREY );
}


// Handles input (of the CONTROLS) for the current menu
void CResultsMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    if( CONTROL_BACK != controlPressed )
    {
        CMenuScreen::HandleInput( controlPressed, bFirstPress );

        return;
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
	}
}


// Render the menu items properly on our menu
void CResultsMenu::RenderMenuItems( CUDTexture* pTexture )
{
    // Don't render anything if we don't have any menu items
    if( 0 == GetNumMenuItems() )
    {
        // Display Message Here
        float fXPos = gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), gc_pwszRESULTS_NO_RESULTS_MESSAGE ) / 2 );
        float fYPos = gc_fRESULTS_NO_RESULTS_TEXT_YPOS;

        pTexture->DrawText( GetBodyFont(), fXPos, fYPos, m_dwMenuItemEnabledFGColor, m_dwMenuItemEnabledBGColor, L"%ls", gc_pwszRESULTS_NO_RESULTS_MESSAGE );
        return;
    }

    m_MenuItems.MoveTo( GetTopItemIndex() );    // Start at menu item that needs to be displayed
    m_TestResults.MoveTo( GetTopItemIndex() );
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

        // Draw the PASSED values
        _snwprintf( pwszMenuValue, MAX_PATH, L"%u", m_TestResults.GetCurrentNode()->GetPassed() );

        // Draw the value right justified
        fXPos = ( m_fRightMenuAreaBoundary - 100 ) - GetStringPixelWidth( m_MenuItems.GetCurrentNode()->m_pFont, pwszMenuValue ) - m_fRightMenuAreaBorder;
        pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, dwItemColorFG, dwItemColorBG, pwszMenuValue );

        // Draw the FAILED values
        _snwprintf( pwszMenuValue, MAX_PATH, L"%u", m_TestResults.GetCurrentNode()->GetFailed() );

        // Draw the value right justified
        fXPos = m_fRightMenuAreaBoundary - GetStringPixelWidth( m_MenuItems.GetCurrentNode()->m_pFont, pwszMenuValue ) - m_fRightMenuAreaBorder;
        pTexture->DrawText( m_MenuItems.GetCurrentNode()->m_pFont, fXPos, fYPos, dwItemColorFG, dwItemColorBG, pwszMenuValue );

        m_MenuItems.MoveNext();
        m_TestResults.MoveNext();
    }
}


// This should be called whenever the menu is entered or activated
void CResultsMenu::Enter( void )
{
    CMenuScreen::Enter();

    if( !m_bFileProcessed )
    {
        m_bFileProcessed = TRUE;

        // Empty our menu items
        ClearMenuItems();

        // Process the results file and create our menu items
        ProcessResultsFile();

        // Update our screen items
        UpdateScreenItems();
    }
}


// Parse the results file and create the list of Passed / Fail info
HRESULT CResultsMenu::ProcessResultsFile( void )
{
    // Clear our current Test Results
    m_TestResults.EmptyList();

    // Open the results file
    FILE* pfRes = fopen( gc_pszResultsFile, "rt" );
    if( NULL == pfRes )
    {
        DebugPrint( "CResultsMenu::ProcessResultsFile():Failed to open the results file!! - '%hs'\n", gc_pszResultsFile );
        return E_FAIL;
    }

    HRESULT hr = S_OK;

    unsigned int uiCount = 0;
    unsigned __int64 ui64Pass, ui64Fail;
    WCHAR* pwszTestName = new WCHAR[g_cuiRESULTS_TEST_NAME_SIZE];
    char* pszTestNotes = new char[g_cuiRESULTS_TEST_NOTES_SIZE];
    WCHAR* pwszFileLine = new WCHAR[g_cuiRESULTSFILE_LINE_SIZE];
    
    if( ( NULL == pwszFileLine ) || ( NULL == pszTestNotes ) || ( NULL == pwszTestName ) )
    {
        DebugPrint( "CResultsMenu::ProcessResultsFile():Failed to allocate memory!!\n" );
        hr = E_OUTOFMEMORY;
        goto cleanup;
    }

    while( NULL != fgetws( pwszFileLine, g_cuiRESULTSFILE_LINE_SIZE, pfRes ) )
    {
        // Make sure we have a vaild data line
        if( !( ( pwszFileLine[0] > L'0' && pwszFileLine[0] < L'9' ) ||
               ( pwszFileLine[0] > L'A' && pwszFileLine[0] < L'Z' ) ||
               ( pwszFileLine[0] > L'a' && pwszFileLine[0] < L'z' ) ) )
            continue;

        // Remove any comments that might be in the line
        WCHAR* wc = wcschr( pwszFileLine, ';' );
        if( NULL != wc ) *wc = L'\0';
        wc = wcschr( pwszFileLine, L'[' );
        if( NULL != wc ) *wc = L'\0';

        RightTrim( pwszFileLine );

        // If we fail to parse the line, return an error
        int iResult = swscanf( pwszFileLine, L"%[^,], %I64u, %I64u, \"%h[^\"]", pwszTestName, &ui64Pass, &ui64Fail, pszTestNotes );
        if(  iResult != 4 )
        {
            if( 3 == iResult )
            {
                pszTestNotes[0] = '\0';
            }
            else
            {
                DebugPrint( "CResultsMenu::ProcessResultsFile():Failed to properly parse line!! - '%ls'\n", pwszFileLine );

                hr = E_FAIL;
                break;
            }
        }

        // Create a new Result object
        CTestResult* pTestResult = new CTestResult;
        if( NULL == pTestResult )
        {
            DebugPrint( "CResultsMenu::ProcessResultsFile():Unable to allocate memory!!\n" );

            hr = E_OUTOFMEMORY;
            break;
        }

        // Store the values in our Test Result
        pTestResult->Update( pwszTestName, ui64Pass, ui64Fail, pszTestNotes );

        // Store the Test Result in our list
        m_TestResults.AddNode( pTestResult );

        // Add a menu item with the test name
        AddMenuItem( GetBodyFont(), pwszTestName, FALSE, TRUE, uiCount++ );
    }

cleanup:
    // Clean up our allocated memory and close our files
    if( NULL != pwszFileLine ) delete[] pwszFileLine;
    if( NULL != pszTestNotes ) delete[] pszTestNotes;
    if( NULL != pwszTestName ) delete[] pwszTestName;
    if( NULL != pfRes ) fclose( pfRes );

    // Set our pointers to NULL
    pwszFileLine = NULL;
    pszTestNotes = NULL;
    pwszTestName = NULL;
    pfRes = NULL;

    return hr;
}


// Sets up and goes to the help menu
void CResultsMenu::GoToHelp( void )
{
    // Check to see if there are any test notes.  If not, simply bail
    if( '\0' == m_TestResults.GetNode( GetSelectedItem() )->GetTestNotes()[0] )
        return;

    // Set the menu title
    WCHAR pwszTitle[50];
    pwszTitle[49] = L'\0';

    _snwprintf( pwszTitle, 49, L"%ls Test Notes", m_TestResults.GetNode( GetSelectedItem() )->GetTestName() );
    GetHelpMenuPtr()->SetMenuTitle( GetHeaderFont(), pwszTitle );

    // Set the back pointer for the help menu
    GetHelpMenuPtr()->SetBackPtr( this );

    // Clear the current help
    GetHelpMenuPtr()->ClearHelpItems();

    // Populate the help menu with info in the test notes
    WCHAR pwszTempStr[MAX_PATH+1];
    char pszLine[MAX_PATH+1];

    pwszTempStr[MAX_PATH] = L'\0';
    pszLine[MAX_PATH] = '\0';

    char* pszTestNotes = m_TestResults.GetNode( GetSelectedItem() )->GetTestNotes();
    char* pszTestNotesPtr = pszTestNotes;
    char* pszNewLinePtr;

    // Determine if there are any "\n's" in the string
    while( NULL != ( pszNewLinePtr = strstr( pszTestNotesPtr, "\\n" ) ) )
    {
        // Copy the string segment to a temp char string
        strncpy( pszLine, pszTestNotesPtr, pszNewLinePtr - pszTestNotesPtr );
        pszLine[pszNewLinePtr - pszTestNotesPtr] = '\0';

        // Copy the line of text to a WCHAR string and add it to our help
        _snwprintf( pwszTempStr, MAX_PATH, L"%hs", pszLine );

        GetHelpMenuPtr()->AddHelpItem( GetBodyFont(),
                                       pwszTempStr,
                                       GetBodyFont(),
                                       L"" );

        // Skip the remaining portion of the \n
        pszTestNotesPtr = pszNewLinePtr + 2;
    }

    // Copy the last string (that won't have a \n)
    _snwprintf( pwszTempStr, MAX_PATH, L"%hs", pszTestNotesPtr );
    
    GetHelpMenuPtr()->AddHelpItem( GetBodyFont(),
                                   pwszTempStr,
                                   GetBodyFont(),
                                   L"" );

    // navigate to the menu
    Leave( GetHelpMenuPtr() );
}


////////////////////////////////////////
////////////////////////////////////////
//// CTestResult Class
////////////////////////////////////////
////////////////////////////////////////

// Constructor
CTestResult::CTestResult( void ) :
m_pwszTestName( NULL ),
m_ui64Passed( 0 ),
m_ui64Failed( 0 ),
m_pszTestNotes( NULL )
{
}

// Destructor
CTestResult::~CTestResult( void )
{
    if( NULL != m_pszTestNotes )
    {
        delete[] m_pszTestNotes;
        m_pszTestNotes = NULL;
    }

    if( NULL != m_pwszTestName )
    {
        delete[] m_pwszTestName;
        m_pwszTestName = NULL;
    }
}


// Operators
BOOL CTestResult::operator>( const CTestResult& item )
{
    if( _wcsicmp( m_pwszTestName, item.m_pwszTestName ) > 0 )
        return TRUE;

    return FALSE;
}

BOOL CTestResult::operator<( const CTestResult& item )
{
    if( _wcsicmp( m_pwszTestName, item.m_pwszTestName ) < 0 )
        return TRUE;

    return FALSE;
}

BOOL CTestResult::operator==( const CTestResult& item )
{
    if( _wcsicmp( m_pwszTestName, item.m_pwszTestName ) == 0 )
        return TRUE;

    return FALSE;
}


// Set the name of the test
void CTestResult::SetTestName( WCHAR* pwszTestName )
{
    if( NULL == pwszTestName )
        return;

    if( NULL != m_pwszTestName )
    {
        delete[] m_pwszTestName;
        m_pwszTestName = NULL;
    }

    m_pwszTestName = new WCHAR[wcslen( pwszTestName ) + 1];
    if( NULL == m_pwszTestName )
    {
        DebugPrint( "CTestResult::SetTestName():Failed to allocate memory!!\n" );
        return;
    }

    // Copy the test name in to our newly allocated memory
    wcscpy( m_pwszTestName, pwszTestName );
}


// Store the test notes
void CTestResult::SetTestNotes( char* pszTestNotes )
{
    if( NULL == pszTestNotes )
        return;

    if( NULL != m_pszTestNotes )
    {
        delete[] m_pszTestNotes;
        m_pszTestNotes = NULL;
    }

    m_pszTestNotes = new char[strlen( pszTestNotes )+1];
    if( NULL == m_pszTestNotes )
    {
        DebugPrint( "CTestResult::SetTestNotes():Unable to allocate memory!!\n" );

        return;
    }

    strcpy( m_pszTestNotes, pszTestNotes );
}


// Update all fields at once
void CTestResult::Update( WCHAR* pwszTestName, unsigned __int64 ui64Passed, unsigned __int64 ui64Failed, char* pszTestNotes )
{
    if( NULL == pwszTestName )
        return;

    SetTestName( pwszTestName );
    SetPassed( ui64Passed );
    SetFailed( ui64Failed );
    SetTestNotes( pszTestNotes );
}