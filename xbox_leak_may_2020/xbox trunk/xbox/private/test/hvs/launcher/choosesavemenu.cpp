/*****************************************************
*** choosesavemenu.cpp
***
*** CPP file for our Choose Save Menu class.
*** This menu will enumerate the saved games for a
*** memory area and allow the user to choose which
*** Saved Game they wish to interact with
***
*** by James N. Helm
*** November 30th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "choosesavemenu.h"

///////////////////////////////////
// CSaveGameInfo
///////////////////////////////////

CSaveGameInfo::CSaveGameInfo( void ) :
m_pszSaveDirectory( NULL ),
m_pwszSaveName( NULL )
{
}

CSaveGameInfo::~CSaveGameInfo( void )
{
    if( NULL != m_pszSaveDirectory )
    {
        delete[] m_pszSaveDirectory;
        m_pszSaveDirectory = NULL;
    }

    if( NULL != m_pwszSaveName )
    {
        delete[] m_pwszSaveName;
        m_pwszSaveName = NULL;
    }
}

BOOL CSaveGameInfo::operator>( const CSaveGameInfo& item )
{
    if( _wcsicmp( m_pwszSaveName, item.m_pwszSaveName ) > 0 )
        return TRUE;

    return FALSE;
}

BOOL CSaveGameInfo::operator<( const CSaveGameInfo& item )
{
    if( _wcsicmp( m_pwszSaveName, item.m_pwszSaveName ) < 0 )
        return TRUE;

    return FALSE;
}

BOOL CSaveGameInfo::operator==( const CSaveGameInfo& item )
{
    if( _wcsicmp( m_pwszSaveName, item.m_pwszSaveName ) == 0 )
        return TRUE;

    return FALSE;
}

// Set the name of the Saved Game
HRESULT CSaveGameInfo::SetSaveDirectory( char* pszSaveDirectory )
{
    if( NULL == pszSaveDirectory )
    {
        DebugPrint( "CSaveGameInfo::SetSaveDirectory():Invalid arguments passed in!!\n" );

        return E_INVALIDARG;
    }

    if( NULL != m_pszSaveDirectory )
    {
        delete[] m_pszSaveDirectory;
        m_pszSaveDirectory = NULL;
    }

    m_pszSaveDirectory = new char[strlen(pszSaveDirectory)+1];
    if( NULL == m_pszSaveDirectory )
    {
        DebugPrint( "CSaveGameInfo::SetSaveDirectory():Unable to allocate memory!!\n" );
        
        return E_OUTOFMEMORY;
    }

    strcpy( m_pszSaveDirectory, pszSaveDirectory );

    return S_OK;
}


// Set the name of the Saved Game
HRESULT CSaveGameInfo::SetSaveName( WCHAR* pwszSaveName )
{
    if( NULL == pwszSaveName )
    {
        DebugPrint( "CSaveGameInfo::SetSaveName():Invalid arguments passed in!!\n" );

        return E_INVALIDARG;
    }

    if( NULL != m_pwszSaveName )
    {
        delete[] m_pwszSaveName;
        m_pwszSaveName = NULL;
    }

    m_pwszSaveName = new WCHAR[wcslen(pwszSaveName)+1];
    if( NULL == m_pwszSaveName )
    {
        DebugPrint( "CSaveGameInfo::SetSaveName():Unable to allocate memory!!\n" );
        
        return E_OUTOFMEMORY;
    }

    wcscpy( m_pwszSaveName, pwszSaveName );

    return S_OK;
}


///////////////////////////////////
// CChooseSaveMenu
///////////////////////////////////

// Constructors and Destructor
CChooseSaveMenu::CChooseSaveMenu( void ) :
m_pszDrivePath( NULL ),
m_bCancelled( FALSE )
{
    // Make sure we don't have a memory leak when deleting the data items
    m_SaveGameInfoList.SetSortedList( TRUE );
    m_SaveGameInfoList.SetDeleteDataItem( TRUE );
}

CChooseSaveMenu::~CChooseSaveMenu( void )
{
    if( NULL != m_pszDrivePath )
    {
        delete[] m_pszDrivePath;
        m_pszDrivePath = NULL;
    }
}

// Initialize the Menu
HRESULT CChooseSaveMenu::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CChooseSaveMenu::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Choose a Configuration" );

    // Add screen items here
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );

    // Status bar column 1
    AddTextItem( GetButtonFont(), L"A",              gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_A_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Select",        gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"B",              gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_B_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Back / Cancel", gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

    // Status Bar Column 2
    AddTextItem( GetButtonFont(), L"C",              gc_fSTATUS_TEXT_COL2_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_X_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( pMenuItemFont,   L" Delete Config", gc_fSTATUS_TEXT_COL2_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

    return hr;
}

// Render the menu items properly on our menu
void CChooseSaveMenu::RenderMenuItems( CUDTexture* pTexture )
{
    if( 0 == GetNumMenuItems() )
    {
        // Show Message;
        float fXPos = gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), gc_pwszNO_CONFIGURATIONS_ON_MA ) / 2 );
        float fYPos = gc_fNO_CONFIGURATIONS_TEXT_YPOS;

        pTexture->DrawText( GetBodyFont(), fXPos, fYPos, m_dwMenuItemEnabledFGColor, m_dwMenuItemEnabledBGColor, L"%ls", gc_pwszNO_CONFIGURATIONS_ON_MA );
    }
    else
    {
        CMenuScreen::RenderMenuItems( pTexture );
    }
}


// Set the drive path of the Memory Area the menu will work with
HRESULT CChooseSaveMenu::SetInfo( char* pszDrivePath )
{
    if( NULL == pszDrivePath )
    {
        DebugPrint( "CChooseSaveMenu::SetInfo():Invalid argument passed in!!\n" );

        return E_INVALIDARG;
    }

    if( NULL != m_pszDrivePath )
    {
        delete[] m_pszDrivePath;
        m_pszDrivePath = NULL;
    }

    m_pszDrivePath = new char[strlen( pszDrivePath ) + 1];
    strcpy( m_pszDrivePath, pszDrivePath );

    return S_OK;
}


// Handles input (of the BUTTONS) for the current menu
void CChooseSaveMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // If it's a repeat press, let's bail
    if( !bFirstPress )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            GoBack();

            break;
        }
    case BUTTON_B:
        {
            m_bCancelled = TRUE;

            GoBack();

            break;
        }
    case BUTTON_X:
        {
            ////////////////////////////
            // Delete the Configuration
            ////////////////////////////
            
            // Check to see if there are any saves
            if( 0 == GetNumMenuItems() )
                break;

            UpdateTexture();

            DWORD dwResult = XDeleteSaveGame( m_pszDrivePath, GetCurrentSaveName() );
            if( ERROR_SUCCESS != dwResult )
                DebugPrint( "CChooseSaveMenu::HandleInput():Failed to delete the Saved Game!!\n" );
            else
                PopulateMenuItems();

            break;
        }
    }
}


// Get the name of the currently selected Saved Game (NULL if none)
WCHAR* CChooseSaveMenu::GetCurrentSaveName( void )
{
    if( 0 == GetNumMenuItems() )
        return NULL;

    return m_SaveGameInfoList[GetSelectedItem()]->GetSaveName();
}


// Get the directory of the currently selected Saved Game (NULL if none)
char* CChooseSaveMenu::GetCurrentSaveDirectory( void )
{
    if( 0 == GetNumMenuItems() )
        return NULL;

    return m_SaveGameInfoList[GetSelectedItem()]->GetSaveDirectory();
}


// This should be called whenever the menu is entered or activated
void CChooseSaveMenu::Enter( void )
{
    // Call the base class Enter
    CMenuScreen::Enter();

    m_bCancelled = FALSE;

    // Check to see if the user set the Drive Path
    if( NULL == m_pszDrivePath )
    {
        DebugPrint( "CChooseSaveMenu::Enter():Drive Path not set!! Call SetInfo()\n" );
        GoBack();
        
        return;
    }

    // Add menu items here
    PopulateMenuItems();
}

// This function will look for Saved Games and populate our menu
void CChooseSaveMenu::PopulateMenuItems( void )
{
    // Clear out our old menu items
    ClearMenuItems();
    m_SaveGameInfoList.EmptyList();

    // Add menu items here
    XGAME_FIND_DATA xgFindData;
    ZeroMemory( &xgFindData, sizeof( xgFindData ) );

    HANDLE hTestSaves = XFindFirstSaveGame( m_pszDrivePath, &xgFindData );
    if( INVALID_HANDLE_VALUE == hTestSaves )
        DebugPrint( "CChooseSaveMenu::PopulateMenuItems():Failed to find a saved game!!\n" );
    else
    {
        do
        {
            // Update our Save Game Info List
            CSaveGameInfo* pSaveGameInfo = new CSaveGameInfo;
            if( NULL == pSaveGameInfo )
            {
                DebugPrint( "CChooseSaveMenu::PopulateMenuItems():Failed to allocate memory!!\n" );

                GoBack();

                // Close our handle
                XFindClose( hTestSaves );
                hTestSaves = INVALID_HANDLE_VALUE;

                return;
            }
            pSaveGameInfo->SetSaveName( xgFindData.szSaveGameName );
            pSaveGameInfo->SetSaveDirectory( xgFindData.szSaveGameDirectory );
            m_SaveGameInfoList.AddNode( pSaveGameInfo );
        } while( XFindNextSaveGame( hTestSaves, &xgFindData ) );

        // Close our handle
        XFindClose( hTestSaves );
        hTestSaves = INVALID_HANDLE_VALUE;

        // Add our menu items (since our list is sorted, we have to do this second
        m_SaveGameInfoList.MoveFirst();
        for( unsigned int x = 0; x < m_SaveGameInfoList.GetNumItems(); x++ )
        {
            AddMenuItem( GetBodyFont(), m_SaveGameInfoList.GetCurrentNode()->GetSaveName(), FALSE, TRUE, x );
            m_SaveGameInfoList.MoveNext();
        }
    }
}

// This should be called whenever you leave the menu
void CChooseSaveMenu::Leave( CXItem* pItem )
{
    // Clear out our Drive Path
    if( NULL != m_pszDrivePath )
    {
        delete[] m_pszDrivePath;
        m_pszDrivePath = NULL;
    }

    CMenuScreen::Leave( pItem );
}