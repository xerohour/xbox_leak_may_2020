/*****************************************************
*** muoptions.h
***
*** Header file for our Memory Unit Options menu
*** class. This menu class will allow you to perform
*** various operations on an MU.  (Format, etc)
***
*** by James N. Helm
*** March 29th, 2001
***
*****************************************************/

#ifndef _MUOPTIONS_H_
#define _MUOPTIONS_H_

#include "menuscreen.h"  // Base menu type
#include "memoryunit.h"
#include "memconfigs.h"
#include "gametitlemenu.h"
#include "savegamemenu.h"

class CMUOptions : public CMenuScreen
{
public:
    CMUOptions( CXItem* pParent );
    ~CMUOptions();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();                                       // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed,
                      BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed,
                      BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick,
                      int nThumbY,
                      int nThumbX,
                      BOOL bFirstYPress,
                      BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( IDirect3DDevice8* pD3DDevice, char* menuFileName );       // Initialize our font and settings
    HRESULT SetMemoryUnit( CMemoryUnit* pMemoryUnit );                      // Sets the local member to point at the proper memory unit

private:
    BOOL            m_bActionInitiated;     // Used to track when a user is entering data from a virtual keyboard
    unsigned int    m_uiActionStage;        // Used for multi-stage actions (Actions that require two menus, etc)
    CMemoryUnit*    m_pMemoryUnit;          // Memory Unit that this class should act upon
    WCHAR*          m_pwszValueBuffer;      // Buffer to hold info from our Keypad / Keyboard functions
    unsigned int    m_uiTopItemIndex;       // Index of the top item being displayed

                    
    DWORD           m_dwNumTitles;          // Used to store the number of Titles the user wishes to create
    DWORD           m_dwNumSavedGames;      // Used to store the number of Saved Games the user wishes to create
    WCHAR*          m_pwszSavedGameName;    // Used to store our Saved Game Name
    DWORD           m_dwSavedGameSize;      // Used to store the desired size of the Saved Game
    DWORD           m_dwFileSize;           // Used to create saved game data of a specific size
    HANDLE          m_hThreadHandle;        // Handle to our thread

    CGameTitleMenu  m_MenuGameTitles;       // List out all the Game Titles and allow the user to select one
    CSaveGameMenu   m_MenuSaveGames;        // List out all the Saved Games for a specific title and allow the user to select one

    // Private Methods
    void Leave( CXItem* pItem );            // This should be called whenever a user leaves this menu

    static friend DWORD WINAPI DataCreateThreadFunc( LPVOID lpParameter );
};

#define MUOPTIONS_MENU_NUM_ITEMS_TO_DISPLAY    9

#endif // _MUOPTIONS_H_
