/*****************************************************
*** onlinemenu.cpp
***
*** CPP file for our XShell Online menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** February 8th, 2002
***
*****************************************************/

#include "stdafx.h"
#include "onlinemenu.h"

extern CSoundCollection g_Sounds;               // Used to generate all sounds for the XShell
extern CGenKeyboardMenu g_GenKeyboardMenu;      // Generic Virutal Keyboard menu
extern CMADisplayMenu   g_MADisplayMenu;        // Used to display our Memory Areas
extern XFONT*           g_pFont;
extern BOOL             g_bDisplayBButton;      // Used to determine if we should render the B Button
extern BOOL             g_bDisplayAButton;      // Used to determine if we should render the A Button
extern CMemoryUnit      g_MemoryUnits[8];       // Memory Units that can be inserted, or removed

// Constructors
COnlineMenu::COnlineMenu( CXItem* pParent )
: CMenuScreen( pParent ),
m_UserEvent( ENUM_ONLINE_NO_ACTION ),
m_hrOnlineResult( S_OK ),
m_bDisableInput( FALSE ),
m_hrPingResult( S_OK ),
m_bPNetPinged( FALSE ),
m_hrCreateResult( S_OK ),
m_bUserCreated( FALSE ),
m_hThreadHandle( NULL ),
m_OLUserMenu( NULL ),
m_OLUserOptionMenu( NULL ),
m_PassCodeMenu( NULL ),
m_uiSourceMA( 0 )
{
    XDBGTRC( APP_TITLE_NAME_A, "COnlineMenu::COnlineMenu(ex)" );

    m_uiNumItems = ENUM_ONLINE_MAX_MENUITEMS;
    SetSelectedItem( 0 );

    // Zero out our User Array
    ZeroMemory( m_aOnlineUsers, sizeof( m_aOnlineUsers ) );

    SetTitle( MENU_TITLE_ONLINE );
}


// Destructor
COnlineMenu::~COnlineMenu()
{
}

// Draws a menu on to the screen
void COnlineMenu::Action( CUDTexture* pTexture )
{
    // Don't do anything if we are running our thread (input is already disabled)
    DWORD dwThreadExitCode;
    if( GetExitCodeThread( m_hThreadHandle, &dwThreadExitCode ) )
    {
        if( STILL_ACTIVE == dwThreadExitCode )
            return;
    }

    // Only update the texture if we need to, otherwise return
    if( !m_bUpdateTexture )
        return;
    else
        m_bUpdateTexture = FALSE;

    // Used to hold a generic message for one of our sub-menus
    WCHAR pwszMessage[MAX_PATH+1];
    pwszMessage[MAX_PATH] = L'\0';

    switch( m_UserEvent )
    {
    case ENUM_ONLINE_NO_ACTION:     // Draw the menu as normal
        {
            // Lock our texture and set it as the render target
            pTexture->Lock();
            pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

#ifdef _DEBUG // This removes the picture on the Online Menu
            // Let our base menu class draw all the items pulled from the MNU file
            CMenuScreen::Action( pTexture );
#endif // _DEBUG

            // Update our Font Height
            XFONT_SetTextHeight( g_pFont, ITEM_ONLINEMENU_FONT_HEIGHT );
            m_uiFontHeightOfItem = ITEM_ONLINEMENU_FONT_HEIGHT;

            // Draw a box behind the currently highlighted choice
            float X1Pos = MENUBOX_SELECTBAR_X1;
            float Y1Pos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * GetSelectedItem() ) - ITEM_SELECTOR_BORDER_SPACE;
            float X2Pos = MENUBOX_SELECTBAR_X2;
            float Y2Pos = Y1Pos + m_uiFontHeightOfItem + ( 2 * ITEM_SELECTOR_BORDER_SPACE );
            
            pTexture->DrawBox( X1Pos, Y1Pos, X2Pos, Y2Pos, ITEM_SELECTOR_COLOR );

            //
            // Draw our Menu Items
            //
            for( unsigned int x = 0; x < GetNumItems(); x++ )
            {
                float XPos = MENUBOX_TEXTAREA_X1;
                float YPos = MENUBOX_TEXTAREA_Y1 + MENUBOX_TEXT_TOP_BORDER + ( ( ITEM_VERT_SPACING + m_uiFontHeightOfItem ) * x );

                // Determine which menu item we are dealing with, and draw the text accordingly
                switch( x )
                {
                case ENUM_ONLINE_PING:
                    {
                        if( m_bPNetPinged )
                        {
                            if( SUCCEEDED( m_hrPingResult ) )
                                pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls %hs (%ls)", g_pwszOnlineMenuItems[x], OnlineGetCurrentCluster(), ONLINE_PING_SUCCESS_MSG );
                            else
                                pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls %hs (%ls 0x%08X)", g_pwszOnlineMenuItems[x], OnlineGetCurrentCluster(), ONLINE_PING_FAILED_MSG, m_hrPingResult );
                        }
                        else
                        {
                            pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls %hs", g_pwszOnlineMenuItems[x], OnlineGetCurrentCluster() );
                        }

                        break;
                    }
                case ENUM_ONLINE_CREATEUSER:
                    {
                        if( m_bUserCreated )
                        {
                            if( SUCCEEDED( m_hrCreateResult ) )
                                pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls (%ls)", g_pwszOnlineMenuItems[x], ONLINE_CREATE_USER_SUCCESS_MSG );
                            else
                                pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls (%ls 0x%08X)", g_pwszOnlineMenuItems[x], ONLINE_CREATE_USER_FAILED_MSG, m_hrCreateResult );
                        }
                        else
                        {
                            pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%ls", g_pwszOnlineMenuItems[x] );
                        }

                        break;
                    }
                default:
                    {
                        pTexture->DrawText( XPos, YPos, SCREEN_DEFAULT_TEXT_FG_COLOR, SCREEN_DEFAULT_TEXT_BG_COLOR, L"%s", g_pwszOnlineMenuItems[x] );
                    }
                }
            }

            // Correct our Font Height
            m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
            XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

            // Unlock the texture
            pTexture->Unlock();

            break;
        }
    case ENUM_ONLINE_PING_START:    // Draw our "ping in progress" message
        {
            m_UserEvent = ENUM_ONLINE_PING_IN_PROGRESS;
            UpdateTexture();

            // Lock our texture and set it as the render target
            pTexture->Lock();
            pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

            // Have to do this a second time because the base class action might interfere
            // Update our Font Height
            XFONT_SetTextHeight( g_pFont, ITEM_ONLINEMENU_FONT_HEIGHT );
            m_uiFontHeightOfItem = ITEM_ONLINEMENU_FONT_HEIGHT;

            pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( ONLINE_PING_IN_PROGRESS_MSG ) / 2 ),
                                MENUBOX_TEXTAREA_Y_CENTER - ( m_uiFontHeightOfItem / 2 ),
                                SCREEN_DEFAULT_TEXT_FG_COLOR,
                                SCREEN_DEFAULT_TEXT_BG_COLOR,
                                L"%ls",
                                ONLINE_PING_IN_PROGRESS_MSG );

            // Correct our Font Height
            m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
            XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

            // Unlock the texture
            pTexture->Unlock();

            break;
        }
    case ENUM_ONLINE_PING_COMPLETE:
        {
            m_UserEvent = ENUM_ONLINE_NO_ACTION;
            m_bDisableInput = FALSE;
            g_bDisplayBButton = TRUE;
            g_bDisplayAButton = TRUE;
            UpdateTexture();

            // Close the thread handle
            CloseHandle( m_hThreadHandle );
            m_hThreadHandle = NULL;

            // Display a message with the results on it
            if( SUCCEEDED( m_hrPingResult ) )
                ::DisplayMessage( L"Ping Results", L"\nThe Ping Was Successful" );
            else
                ::DisplayMessage( L"Ping Results", L"The Ping Failed!\n0x%08X (%d)", m_hrPingResult, m_hrPingResult );

            break;
        }
    case ENUM_ONLINE_GET_USERNAME:
        {
            m_UserEvent = ENUM_ONLINE_NO_ACTION;
            UpdateTexture();

            if( !g_GenKeyboardMenu.GetCancelled() )
            {
                // Check to see if it's a valid user name; If not, message the user
                if( !OnlineIsNameValid( g_GenKeyboardMenu.GetValue() ) )
                {
                    m_UserEvent = ENUM_ONLINE_GOT_INVALIDNAME;
                    ::DisplayMessage( L"Invalid User Name", L"The name must be at least 5 characters\nbeginning with a letter.  It may only\ncontain letters, numbers, and\nunderscores (_)." );
                    return;
                }
                else // Collect the passcode
                {
                    m_UserEvent = ENUM_ONLINE_GET_PASSCODE;
                    Leave( &m_PassCodeMenu );
                }
            }

            break;
        }
    case ENUM_ONLINE_GOT_INVALIDNAME:
        {
            m_UserEvent = ENUM_ONLINE_NO_ACTION;
            UpdateTexture();

            CreateUserStart( g_GenKeyboardMenu.GetValue() );
            break;
        }
    case ENUM_ONLINE_GET_PASSCODE:
        {
            m_UserEvent = ENUM_ONLINE_NO_ACTION;
            UpdateTexture();

            if( !m_PassCodeMenu.GetCancelled() )
                m_UserEvent = ENUM_ONLINE_GENERATE_USER_START;

            break;
        }
    case ENUM_ONLINE_GENERATE_USER_START:    // Draw our "Create in progress" message
        {
            m_UserEvent = ENUM_ONLINE_GENERATE_USER_IN_PROGRESS;
            UpdateTexture();

            // Lock our texture and set it as the render target
            pTexture->Lock();
            pTexture->Clear( SCREEN_DEFAULT_BACKGROUND_COLOR );

            // Have to do this a second time because the base class action might interfere
            // Update our Font Height
            XFONT_SetTextHeight( g_pFont, ITEM_ONLINEMENU_FONT_HEIGHT );
            m_uiFontHeightOfItem = ITEM_ONLINEMENU_FONT_HEIGHT;

            pTexture->DrawText( MENUBOX_TEXTAREA_X_CENTER - ( GetStringPixelWidth( ONLINE_CREATE_USER_IN_PROGRESS_MSG ) / 2 ),
                                MENUBOX_TEXTAREA_Y_CENTER - ( m_uiFontHeightOfItem / 2 ),
                                SCREEN_DEFAULT_TEXT_FG_COLOR,
                                SCREEN_DEFAULT_TEXT_BG_COLOR,
                                L"%ls",
                                ONLINE_CREATE_USER_IN_PROGRESS_MSG );

            // Correct our Font Height
            m_uiFontHeightOfItem = FONT_DEFAULT_HEIGHT;
            XFONT_SetTextHeight( g_pFont, FONT_DEFAULT_HEIGHT );

            // Unlock the texture
            pTexture->Unlock();

            break;
        }
    case ENUM_ONLINE_GENERATE_USER_COMPLETE:
        {
            m_UserEvent = ENUM_ONLINE_NO_ACTION;
            m_bDisableInput = FALSE;
            g_bDisplayBButton = TRUE;
            g_bDisplayAButton = TRUE;
            UpdateTexture();

            // Close the thread handle
            CloseHandle( m_hThreadHandle );
            m_hThreadHandle = NULL;

            // Display a message with the results on it
            if( SUCCEEDED( m_hrCreateResult ) )
            {
                ::DisplayMessage( L"Create User Results", L"\nThe User Was Created Successfully" );
                return;
            }
            else
            {
                if( XONLINE_S_ACCOUNTS_NAME_TAKEN == m_hrCreateResult ) // March XDK Hack --> || ( XONLINE_E_SERVER_ERROR == m_hrCreateResult ) )
                    ::DisplayMessage( L"Create User Results", L"That User Name Is Already In Use\nPlease Try Again With a Different Name" );
                else
                    ::DisplayMessage( L"Create User Results", L"Creating The User Failed!\n0x%08X (%d)", m_hrCreateResult, m_hrCreateResult );
                return;
            }

            break;
        }
    case ENUM_ONLINE_MANAGE_NAVTOGETSOURCEMEMAREA:
        {
            m_UserEvent = ENUM_ONLINE_MANAGE_GETMEMAREA;
            UpdateTexture();

            g_MADisplayMenu.SetTitle( L"Select Source" );
            g_MADisplayMenu.SetInfo( FALSE, TRUE );
            Leave( &g_MADisplayMenu );
            break;
        }
    case ENUM_ONLINE_MANAGE_NAVTOSOURCEMEMAREAUSERS:
        {
            //
            // Navigate back to the source memory area user display screen
            //
            m_UserEvent = ENUM_ONLINE_NO_ACTION;
            UpdateTexture();

            if( !NavToSourceMAUsers() )
                m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOGETSOURCEMEMAREA;

            break;
        }
    case ENUM_ONLINE_MANAGE_GETMEMAREA:
        {
            m_UserEvent = ENUM_ONLINE_NO_ACTION;
            UpdateTexture();

            if( !g_MADisplayMenu.GetCancelled() )
            {
                // Store the source memory area and prompt for a destination
                m_uiSourceMA = g_MADisplayMenu.GetSelectedItem();

                // Check to see if we are dealing with the Hard Drive
                if( ENUM_MADISPLAY_HARD_DRIVE == m_uiSourceMA )
                {
                    // Zero out our Online Users
                    ZeroMemory( m_aOnlineUsers, sizeof( m_aOnlineUsers ) );

                    // Get the users from the Hard Drive
                    DWORD dwNumUsers;
                    HRESULT hr = OnlineGetUsersFromHD( (PXONLINE_USER) m_aOnlineUsers, &dwNumUsers );
                    if( FAILED( hr ) )
                    {
                        m_UserEvent = ENUM_ONLINE_NO_ACTION;

                        ::DisplayErrorMessage( L"Obtaining users from the Hard Drive Failed!\n0x%08X (%d)", hr, hr );
                        return;
                    }

                    // Make sure there is at least one user.  If not, message the user
                    if( 0 == dwNumUsers )
                    {
                        m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOGETSOURCEMEMAREA;

                        ::DisplayMessage( L"No Users", L"There are no users to manage\non the Hard Drive" );
                        return;
                    }
                    else if( 1 == dwNumUsers )  // If there is only 1 user, skip the user select menu
                    {
                        // Go to the user options screen
                        m_UserEvent = ENUM_ONLINE_MANAGE_GETUSEROPTION;

                        m_OLUserMenu.SetSelectedItem( 0 );  // We use this menu to store our source user for HD copies
                        swprintf( pwszMessage, L"What would you like to do with\n%hs.%hs?", m_aOnlineUsers[0].name, m_aOnlineUsers[0].kingdom );

                        m_OLUserOptionMenu.SetText( pwszMessage );
                        Leave( &m_OLUserOptionMenu );
                        break;
                    }
                    else
                    {
                        m_UserEvent = ENUM_ONLINE_MANAGE_GETUSERFROMHD;

                        m_OLUserMenu.SetInfo( m_aOnlineUsers, dwNumUsers );
                        Leave( &m_OLUserMenu );
                        break;
                    }                    
                }
                else // Selected area must be memory unit
                {
                    // Prompt the user for a list of options (Copy and Delete)
                    m_UserEvent = ENUM_ONLINE_MANAGE_GETUSEROPTION;

                    // Get the user name off of the MU
                    BOOL bUserExists;
                    HRESULT hr = OnlineGetUserFromMU( g_MemoryUnits[m_uiSourceMA].GetPort(),
                                                      g_MemoryUnits[m_uiSourceMA].GetSlot(),
                                                      (PXONLINE_USER) m_aOnlineUsers,
                                                      &bUserExists );
                    if( FAILED( hr ) )
                    {
                        m_UserEvent = ENUM_ONLINE_NO_ACTION;

                        ::DisplayErrorMessage( L"Failed to get the user from the\nMemory Unit!!\n0x%08X (%d)", hr, hr );
                        return;
                    }

                    // If there is no user on the MU, message the user
                    if( !bUserExists )
                    {
                        m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOGETSOURCEMEMAREA;

                        ::DisplayMessage( L"No Users", L"There are no users to manage\non this Memory Area" );
                        return;
                    }

                    swprintf( pwszMessage,
                              L"What would you like to do with\n%hs.%hs?",
                              m_aOnlineUsers[0].name,
                              m_aOnlineUsers[0].kingdom );
                    
                    m_OLUserOptionMenu.SetText( pwszMessage );
                    Leave( &m_OLUserOptionMenu );
                }
            }

            break;
        }
    case ENUM_ONLINE_MANAGE_GETUSERFROMHD:
        {
            m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOGETSOURCEMEMAREA;
            UpdateTexture();

            if( !m_OLUserMenu.GetCancelled() )
            {
                m_UserEvent = ENUM_ONLINE_MANAGE_GETUSEROPTION;
                
                swprintf( pwszMessage, L"What would you like to do with\n%hs.%hs?", m_aOnlineUsers[m_OLUserMenu.GetValue()].name, m_aOnlineUsers[m_OLUserMenu.GetValue()].kingdom );
                
                m_OLUserOptionMenu.SetText( pwszMessage );
                Leave( &m_OLUserOptionMenu );
            }

            break;
        }
    case ENUM_ONLINE_MANAGE_GETUSEROPTION:
        {
            m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOSOURCEMEMAREAUSERS;
            UpdateTexture();

            // Check to see if the user cancelled the menu
            if( !m_OLUserOptionMenu.GetCancelled() )
            {
                switch( m_OLUserOptionMenu.GetOption() )
                {
                case ENUM_OLUSEROPTION_COPY:
                    {
                        m_UserEvent = ENUM_ONLINE_MANAGE_GETDESTMEMAREA;

                        // Ask the user for the destination memory area
                        // TODO: Check with PM -- Should user be allowed to select same destination as source?
                        // TODO: Probably, because once the user is read from a MA, that MA can be switched with another
                        g_MADisplayMenu.SetTitle( L"Select Destination" );
                        g_MADisplayMenu.SetInfo( FALSE, TRUE );
                        Leave( &g_MADisplayMenu );

                        break;
                    }
                case ENUM_OLUSEROPTION_DELETE:
                    {
                        m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOSOURCEMEMAREAUSERS;
                        UpdateTexture();

                        // Delete the specified user from the specified memory area
                        if( ENUM_MADISPLAY_HARD_DRIVE == m_uiSourceMA )
                        {
                            HRESULT hr = OnlineRemoveUserFromHD( &m_aOnlineUsers[m_OLUserMenu.GetValue()] );
                            if( FAILED( hr ) )
                            {
                                ::DisplayErrorMessage( L"Failed to remove the user\nfrom the hard drive!\n%hs.%hs\n0x%08X (%d)", m_aOnlineUsers[m_OLUserMenu.GetValue()].name, m_aOnlineUsers[m_OLUserMenu.GetValue()].kingdom, hr, hr );
                                return;
                            }
                            else // Successful!
                            {
                                ::DisplayMessage( L"Success", L"The user was successfully removed\nfrom the hard drive!\n%hs.%hs", m_aOnlineUsers[m_OLUserMenu.GetValue()].name, m_aOnlineUsers[m_OLUserMenu.GetValue()].kingdom );
                                return;
                            }
                        }
                        else // Must be a MU
                        {
                            // Set an empty user in to the MU
                            HRESULT hr = OnlineClearUserFromMU( g_MemoryUnits[m_uiSourceMA].GetUDataDrive() );
                            if( FAILED( hr ) )
                            {
                                ::DisplayErrorMessage( L"Failed to remove the user\nfrom the Memory Unit!\n%hs.%hs\n0x%08X (%d)", m_aOnlineUsers[0].name, m_aOnlineUsers[0].kingdom, hr, hr );
                                return;
                            }
                            else // Successful!
                            {
                                // Refresh the MU name (it probably changed)
                                g_MemoryUnits[m_uiSourceMA].RefreshName();

                                ::DisplayMessage( L"Success", L"The user was successfully removed\nfrom the Memory Unit!\n%hs.%hs", m_aOnlineUsers[0].name, m_aOnlineUsers[0].kingdom );
                                return;
                            }
                        }

                        break;
                    }
                }
            }

            break;
        }
    case ENUM_ONLINE_MANAGE_GETDESTMEMAREA:
        {
            m_UserEvent = ENUM_ONLINE_NO_ACTION;
            UpdateTexture();

            if( g_MADisplayMenu.GetCancelled() )
            {
                m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOSOURCEMEMAREAUSERS;
                ::DisplayMessage( L"Copy Cancelled", L"\nThe copy operation has been cancelled." );
                return;
            }
            else  // Did the user cancel the action?  If so, abort
            {
                unsigned int uiDestMemArea = g_MADisplayMenu.GetSelectedItem();
                PXONLINE_USER pSourceUser = NULL;

                if( ENUM_MADISPLAY_HARD_DRIVE == m_uiSourceMA )
                    pSourceUser = &m_aOnlineUsers[m_OLUserMenu.GetValue()];
                else // Source is the Memory Unit
                    pSourceUser = &m_aOnlineUsers[0];

                // Does destionation memory areas exist?
                if( ( ENUM_MADISPLAY_HARD_DRIVE != uiDestMemArea ) && ( !g_MemoryUnits[uiDestMemArea].IsMounted() ) )
                {
                    ::DisplayMessage( L"User Copy Aborted", L"The destination Memory Unit was removed\n\nCopy Aborted" );
                    return;
                }

                // If the destination is the hard drive, check to see if the name already exists
                if( ENUM_MADISPLAY_HARD_DRIVE == uiDestMemArea )
                {
                    BOOL bUserExists;
                    HRESULT hr = OnlineIsUserOnHD( pSourceUser->name, &bUserExists );
                    
                    // TODO: Message the user with an error
                    // if( FAILED( hr ) )                        

                    if( bUserExists )
                    {
                        m_UserEvent = ENUM_ONLINE_MANAGE_OVERWRITE;

                        ::DisplayYesNoMessage( L"Overwrite User", L"Would you like to overwrite the user\non the hard drive?\n%hs.%hs", pSourceUser->name, pSourceUser->kingdom );
                        return;
                    }
                    else // Check to see if the Hard Drive is full.  If so, message the user
                    {
                        DWORD dwNumUsers = 0;
                        XONLINE_USER  aOnlineUsers[XONLINE_MAX_STORED_ONLINE_USERS];

                        HRESULT hr = OnlineGetUsersFromHD( (PXONLINE_USER) aOnlineUsers, &dwNumUsers );
                        if( FAILED( hr ) )
                        {
                            swprintf( pwszMessage, L"Failed to retreive users from the\nHard Drive!!\n0x%08X (%d)", hr, hr );

                            // Message the user with the failure code
                            ::DisplayErrorMessage( L"Failed to retreive users from the\nHard Drive!!\n0x%08X (%d)", hr, hr );
                            return;
                        }

                        if( 8 == dwNumUsers )
                        {
                            ::DisplayMessage( L"Hard Drive Full", L"The hard drive contains the maximum\nnumber of users. (8).\nYou need to delete a user to continue" );
                            return;
                        }
                    }
                }
                else // Memory Unit
                {
                    BOOL bUserExists;
                    XONLINE_USER OnlineUser;

                    HRESULT hr = OnlineGetUserFromMU( g_MemoryUnits[uiDestMemArea].GetPort(),
                                                      g_MemoryUnits[uiDestMemArea].GetSlot(),
                                                      (PXONLINE_USER) &OnlineUser,
                                                      &bUserExists );
                    if( FAILED( hr ) )
                    {
                        ::DisplayErrorMessage( L"Failed to get the users from the\nMemory Unit!!\n0x%08X (%d)", hr, hr );
                        return;
                    }

                    if( bUserExists )
                    {
                        // Prompt for overwrite
                        m_UserEvent = ENUM_ONLINE_MANAGE_OVERWRITE;
                        
                        // TODO: Use the MU name as well as the destination user name
                        ::DisplayYesNoMessage( L"Overwrite User", L"Overwrite %hs.%hs\nwith %hs.%hs\non the Memory Unit?", OnlineUser.name, OnlineUser.kingdom, pSourceUser->name, pSourceUser->kingdom );
                        return;
                    }
                }

                // Copy the user to the destination memory area
                HRESULT hr = CopyUserToMA( pSourceUser, uiDestMemArea );

                // Make sure we return the user to the 'Select User' screen if possible,
                // or the 'Select Memory Area' screen if not possible.
                m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOSOURCEMEMAREAUSERS;

                if( FAILED( hr ) )
                {
                    // Message the user with the failure code
                    ::DisplayErrorMessage( L"Failed to Copy the user to the\nMemory Area!0x%08X (%d)", hr, hr );
                    return;
                }
                else
                {
                    // Report success to the user
                    ::DisplayMessage( L"User Copy Successful", L"Successfully copied the user to the\nmemory area!\n%hs.%hs", pSourceUser->name, pSourceUser->kingdom );
                    return;
                }
            }

            break;
        }
    case ENUM_ONLINE_MANAGE_OVERWRITE:
        {
            // Make sure we return the user to the 'Select User' screen if possible,
            // or the 'Select Memory Area' screen if not possible.
            m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOSOURCEMEMAREAUSERS;
            UpdateTexture();

            if( YesNoMenuCancelled() )
            {
                ::DisplayMessage( L"Copy Cancelled", L"\nThe copy operation has been cancelled." );
                return;
            }
            else
            {
                unsigned int uiDestMemArea = g_MADisplayMenu.GetSelectedItem();
                PXONLINE_USER pSourceUser = NULL;

                if( ENUM_MADISPLAY_HARD_DRIVE == m_uiSourceMA )
                    pSourceUser = &m_aOnlineUsers[m_OLUserMenu.GetValue()];
                else // Source is the Memory Unit
                    pSourceUser = &m_aOnlineUsers[0];

                // Does the destionation memory areas exist?
                if( ( ENUM_MADISPLAY_HARD_DRIVE != uiDestMemArea ) && ( !g_MemoryUnits[uiDestMemArea].IsMounted() ) )
                {
                    ::DisplayMessage( L"User Copy Aborted", L"The destination Memory Unit was removed\n\nCopy Aborted" );
                    return;
                }

                // Copy the user to the destination memory area
                HRESULT hr = CopyUserToMA( pSourceUser, uiDestMemArea );
                if( FAILED( hr ) )
                {
                    // Message the user with the failure code
                    ::DisplayErrorMessage( L"Failed to Copy the user to the\nMemory Area!0x%08X (%d)", hr, hr );
                    return;
                }
                else
                {
                    // Report success to the user
                    ::DisplayMessage( L"User Copy Successful", L"Successfully copied the user to the\nmemory area!\n%hs.%hs", pSourceUser->name, pSourceUser->kingdom );
                    return;
                }
            }

            break;
        }
    case ENUM_ONLINE_PING_IN_PROGRESS:  // These events need to be handled in a thread
    case ENUM_ONLINE_GENERATE_USER_IN_PROGRESS:
        {
            // Make sure no input is enabled before we create our thread
            m_bDisableInput = TRUE;

            // Disable the a and b buttons
            g_bDisplayBButton = FALSE;
            g_bDisplayAButton = FALSE;

            m_hThreadHandle = CreateThread( NULL,
                                            0,
                                            ProcessUserEvents,
                                            (void*) this,
                                            0,
                                            NULL );
            break;
        }
    }
}


// Handle any requests for the joystick (thumb-pad)
void COnlineMenu::HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress )
{
    if( m_bDisableInput )
        return;

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
void COnlineMenu::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    if( m_bDisableInput )
        return;

    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Buttons being pressed
    switch( controlPressed )
    {
    case CONTROL_DPAD_UP:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( GetSelectedItem() > 0 )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() - 1 );
                }
            }

            break;
        }
    case CONTROL_DPAD_DOWN:
        {
            // Only act if we have any items on our screen
            if( GetNumItems() != 0 )
            {
                if( ( (unsigned int)GetSelectedItem() + 1 ) < GetNumItems() )
                {
                    m_bUpdateTexture = TRUE;

                    // Play the Menu Item Select Sound
                    g_Sounds.PlayMenuItemSelectSound();

                    SetSelectedItem( GetSelectedItem() + 1 );
                }
            }

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void COnlineMenu::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    if( m_bDisableInput )
        return;

    // If it's a repeat press, let's bail
    if( !bFirstPress )
        return;

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

                switch( GetSelectedItem() )
                {
                case ENUM_ONLINE_PING:
                    {
                        // Ping the currently selected domain
                        m_UserEvent = ENUM_ONLINE_PING_START;
                        UpdateTexture();

                        break;
                    }
                case ENUM_ONLINE_CREATEUSER:
                    {
                        WCHAR pwszUserName[XONLINE_NAME_SIZE];
                        pwszUserName[XONLINE_NAME_SIZE - 1] = L'\0';

                        OnlineGenerateUserName( pwszUserName );
                        CreateUserStart( pwszUserName );

                        break;
                    }
                case ENUM_ONLINE_MANAGEUSERS:
                    {
                        // Manage Users
                        m_UserEvent = ENUM_ONLINE_MANAGE_NAVTOGETSOURCEMEMAREA;
                        UpdateTexture();

                        break;
                    }
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
HRESULT COnlineMenu::Init( char* menuFileName )
{
    XDBGTRC( APP_TITLE_NAME_A, "COnlineMenu::Init()" );

    HRESULT hr = CMenuScreen::Init( menuFileName );

    //
    // Initialize Sub-Menus
    //

    m_PassCodeMenu.Init( MENU_PASSCODE_MENU_FILENAME );
    m_PassCodeMenu.SetParent( this );

    m_OLUserMenu.Init( MENU_OLUSER_MENU_FILENAME_A );
    m_OLUserMenu.SetParent( this );

    m_OLUserOptionMenu.Init( MENU_OLUSEROPTION_MENU_FILENAME_A );
    m_OLUserOptionMenu.SetParent( this );

    return hr;
}

DWORD WINAPI COnlineMenu::ProcessUserEvents( LPVOID lpParam )
{
    COnlineMenu* pOnlineMenu = (COnlineMenu*)lpParam;
    switch( pOnlineMenu->m_UserEvent )
    {
    case ENUM_ONLINE_PING_IN_PROGRESS:     // Draw the menu as normal
        {
            // Does a quick logon to the user account creation server
            pOnlineMenu->m_hrOnlineResult = OnlineQuickLoginStart();
            if( FAILED( pOnlineMenu->m_hrOnlineResult ) )
                XDBGWRN( APP_TITLE_NAME_A, "COnlineMenu::ProcessUserEvents():Failed to perform OnlineQuickLoginStart!! (0x%08X)", pOnlineMenu->m_hrOnlineResult );

            // Does a quick logoff to the user account creation server
            HRESULT hr = OnlineQuickLoginFinish(); 
            if( FAILED( hr ) )
                XDBGWRN( APP_TITLE_NAME_A, "COnlineMenu::ProcessUserEvents():Failed to perform OnlineQuickLoginFinish!! (0x%08X)", hr );

            // Store the Ping Result for later
            pOnlineMenu->m_hrPingResult = pOnlineMenu->m_hrOnlineResult;
            pOnlineMenu->m_UserEvent = ENUM_ONLINE_PING_COMPLETE;
            pOnlineMenu->m_bPNetPinged = TRUE;
            pOnlineMenu->UpdateTexture();

            break;
        }
    case ENUM_ONLINE_GENERATE_USER_IN_PROGRESS:
        {
            pOnlineMenu->m_UserEvent = ENUM_ONLINE_GENERATE_USER_COMPLETE;
            pOnlineMenu->m_bUserCreated = TRUE;

            XONLINE_USER OnlineUser;
            ZeroMemory( &OnlineUser, sizeof( OnlineUser ) );

            sprintf( OnlineUser.name, "%ls", g_GenKeyboardMenu.GetValue() );
            memcpy( OnlineUser.pin, pOnlineMenu->m_PassCodeMenu.GetValue(), XONLINE_PIN_LENGTH );
            strncpy( OnlineUser.kingdom, OnlineGetCurrentRealm(), XONLINE_KINGDOM_SIZE );    // TODO: Get this from an INI file somewhere

            // Set the require pin flag if necessary
            if( '\0' != OnlineUser.pin[0] )
                OnlineUser.dwUserOptions |= XONLINE_USER_OPTION_REQUIRE_PIN;

            pOnlineMenu->m_hrOnlineResult = OnlineGenerateUserAccount( &OnlineUser );
            if( FAILED( pOnlineMenu->m_hrOnlineResult ) )
                XDBGWRN( APP_TITLE_NAME_A, "COnlineMenu::ProcessUserEvents():Failed to Generate User Account!! Error - '0x%08X', '%d'", pOnlineMenu->m_hrOnlineResult, pOnlineMenu->m_hrOnlineResult );

            // Store this result to user later on our menu
            pOnlineMenu->m_hrCreateResult = pOnlineMenu->m_hrOnlineResult;

            pOnlineMenu->UpdateTexture();
            
            break;
        }
    }

    return 0;
}


// Will be called whenever the user enters this menu
void COnlineMenu::Enter()
{
    // Call the base class enter
    CMenuScreen::Enter();

    // Set our "global" menus to point to this menu
    g_GenKeyboardMenu.SetParent( this );
    g_MADisplayMenu.SetParent( this );
}


// Copies a user to the specified memory area
HRESULT COnlineMenu::CopyUserToMA( PXONLINE_USER pOnlineUser, unsigned int uiMemArea )
{
    HRESULT hr = S_OK;

    // Copy the user
    if( ENUM_MADISPLAY_HARD_DRIVE == uiMemArea )
    {
        hr = OnlineAddUserToHD( (PXONLINE_USER) pOnlineUser );
        if( FAILED( hr ) )
            XDBGWRN( APP_TITLE_NAME_A, "COnlineMenu::CopyUserToMA()OnlineAddUserToHD Failed!! - '0x%08X', '%d'", hr, hr );
    }
    else // Destination is an MU
    {
        hr = OnlineSetUserInMU( g_MemoryUnits[uiMemArea].GetUDataDrive(), (PXONLINE_USER) pOnlineUser );
        if( FAILED( hr ) )
            XDBGWRN( APP_TITLE_NAME_A, "COnlineMenu::CopyUserToMA()OnlineSetUserInMU Failed!! - '0x%08X', '%d'", hr, hr );
        else // Refresh the MU name (it probably changed)
            g_MemoryUnits[uiMemArea].RefreshName();
    }

    return hr;
}


// Used to start the Create User process
void COnlineMenu::CreateUserStart( WCHAR* pwszUserName )
{
    if( NULL == pwszUserName )
        return;

    // Check to see if the Hard Drive is full.  If so, message the user
    DWORD dwNumUsers = 0;
    HRESULT hr = OnlineGetUsersFromHD( (PXONLINE_USER) m_aOnlineUsers, &dwNumUsers );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "COnlineMenu::HandleInput():Failed to get the users from the hard drive!!" );
        ::DisplayErrorMessage( L"Obtaining users from the Hard Drive Failed!\n0x%08X (%d)", hr, hr );
        return;
    }

    // If the hard drive is full we need to message the user
    if( 8 == dwNumUsers )
    {
        ::DisplayMessage( L"Hard Drive Full", L"The hard drive contains the maximum\nnumber of users. (8).\nYou need to delete a user to continue" );
        return;
    }

    //Create a user on the currently selected domain
    m_UserEvent = ENUM_ONLINE_GET_USERNAME;

    g_GenKeyboardMenu.SetInfo( L"Enter a user name:", XONLINE_NAME_SIZE - 1, pwszUserName );
    g_GenKeyboardMenu.SetTitle( L"Online User Name" );

    Leave( &g_GenKeyboardMenu );
}


// Returns TRUE if it navigates to the Memory Area screen, FALSE if otherwise
BOOL COnlineMenu::NavToSourceMAUsers()
{
    return FALSE;

    /* Sean changed his mind on the flow, so for now, comment this out
    // Check to see if we are dealing with the Hard Drive
    if( ENUM_MADISPLAY_HARD_DRIVE == m_uiSourceMA )
    {
        // Zero out our Online Users
        ZeroMemory( m_aOnlineUsers, sizeof( m_aOnlineUsers ) );

        // Get the users from the Hard Drive
        DWORD dwNumUsers;
        HRESULT hr = OnlineGetUsersFromHD( (PXONLINE_USER) m_aOnlineUsers, &dwNumUsers );
        if( FAILED( hr ) )
        {
            m_UserEvent = ENUM_ONLINE_NO_ACTION;

            ::DisplayErrorMessage( L"Obtaining users from the Hard Drive Failed!\n0x%08X (%d)", hr, hr );
            return FALSE;
        }

        // Make sure there is at least one user.  If not, return FALSE
        if( ( 0 == dwNumUsers ) || ( 1 == dwNumUsers ) )
        {
            return FALSE;
        }
        else
        {
            m_UserEvent = ENUM_ONLINE_MANAGE_GETUSERFROMHD;

            m_OLUserMenu.SetInfo( m_aOnlineUsers, dwNumUsers );
            Leave( &m_OLUserMenu );
            return TRUE;
        }                    
    }
    else // Selected area must be memory unit
    {
        return FALSE;
    }
    */
}
