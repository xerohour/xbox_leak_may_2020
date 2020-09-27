/*****************************************************
*** choosesavemenu.h
***
*** Header file for our Choose Save Menu class.
*** This menu will enumerate the saved games for a
*** memory area and allow the user to choose which
*** Saved Game they wish to interact with
***
*** by James N. Helm
*** November 30th, 2001
***
*****************************************************/

#ifndef _CHOOSESAVEMENU_H_
#define _CHOOSESAVEMENU_H_

#include "menuscreen.h"

///////////////////////////////////
// CSaveGameInfo
///////////////////////////////////
class CSaveGameInfo
{
public:
    CSaveGameInfo( void );
    ~CSaveGameInfo( void );

    // Properties
    char* m_pszSaveDirectory;                           // Directory of the saved game
    WCHAR* m_pwszSaveName;                              // Name of the saved game

    // Methods
    char* GetSaveDirectory( void )                      // Set the name of the Saved Game
        { return m_pszSaveDirectory; };
    WCHAR* GetSaveName( void )                          // Set the name of the Saved Game
        { return m_pwszSaveName; };

    HRESULT SetSaveDirectory( char* pszSaveDirectory ); // Set the name of the Saved Game
    HRESULT SetSaveName( WCHAR* pwszSaveName );         // Set the name of the Saved Game

    // Operators
    virtual BOOL operator>( const CSaveGameInfo& item );
    virtual BOOL operator<( const CSaveGameInfo& item );
    virtual BOOL operator==( const CSaveGameInfo& item );
};


const WCHAR gc_pwszNO_CONFIGURATIONS_ON_MA[] =  L"There are no configurations on this Memory Area";
const float gc_fNO_CONFIGURATIONS_TEXT_YPOS =   125.0f;

///////////////////////////////////
// CChooseSaveMenu
///////////////////////////////////
class CChooseSaveMenu : public CMenuScreen
{
public:
    // Constructors and Destructor
    CChooseSaveMenu( void );
    ~CChooseSaveMenu( void );

    // Public Methods
    HRESULT Init( XFONT* pMenuItemFont,             // Initialize the Menu
                  XFONT* pMenuTitleFont );
    void HandleInput( enum BUTTONS buttonPressed,   // Handles input (of the BUTTONS) for the current menu
                      BOOL bFirstPress );
    void Enter( void );                             // This should be called whenever the menu is entered or activated

    BOOL GetCancelled( void )                       // Returns the status of the menu (was it cancelled or not)
        { return m_bCancelled; };
    HRESULT SetInfo( char* pszDrivePath );          // Set the drive path of the Memory Area the menu will work with

    WCHAR* GetCurrentSaveName( void );              // Get the name of the currently selected Saved Game (NULL if none)
    char* GetCurrentSaveDirectory( void );          // Get the directory of the currently selected Saved Game (NULL if none)

private:
    // Properties
    char*                           m_pszDrivePath;         // Used to determine which Memory Area we should look at
    BOOL                            m_bCancelled;           // Used to track if the user cancelled the save game selection
    CLinkedList< CSaveGameInfo* >   m_SaveGameInfoList;     // Information for the Saved Games that have been found

    // Methods
    void RenderMenuItems( CUDTexture* pTexture );   // Render the menu items properly on our menu
    void Leave( CXItem* pItem );                    // This should be called whenever you leave the menu
    void PopulateMenuItems( void );                 // This function will look for Saved Games and populate our menu
};

#endif // _CHOOSESAVEMENU_H_