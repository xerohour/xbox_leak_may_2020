/*****************************************************
*** hdoptions.h
***
*** Header file for our Hard Drive Options menu
*** class. This menu class will allow you to perform
*** various operations on the Xbox Hard Drive.
***
*** by James N. Helm
*** April 2nd, 2001
***
*****************************************************/

#ifndef _HDOPTIONS_H_
#define _HDOPTIONS_H_

#include "menuscreen.h"  // Base menu type
#include "harddrive.h"
#include "memconfigs.h"

class CHDOptions : public CMenuScreen
{
public:
    CHDOptions( CXItem* pParent );
    ~CHDOptions();

    // Must be overridden for this to be a Item on our XShell program
    // void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( IDirect3DDevice8* pD3DDevice, char* menuFileName );       // Initialize our font and settings

private:
    BOOL         m_bActionInitiated;            // Used to track when a user is entering data from a virtual keyboard
    WCHAR*       m_pwszValueBuffer;             // Buffer to hold info from our Keypad / Keyboard functions

    DWORD        m_dwNumTitles;                 // Used to store the number of Titles the user wishes to create
    DWORD        m_dwNumSavedGames;             // Used to store the number of Saved Games the user wishes to create
    WCHAR*       m_pwszSavedGameName;           // Used to store our Saved Game Name
    DWORD        m_dwSavedGameSize;             // Used to store the desired size of the Saved Game
    DWORD        m_dwFileSize;                  // Used to create saved game data of a specific size
    HANDLE       m_hThreadHandle;               // Handle to our thread

    // Private Methods
    // void Leave( CXItem* pItem );                // This should be called whenever a user leaves this menu

    static friend DWORD WINAPI DataCreateThreadFunc( LPVOID lpParameter );
};

#endif // _HDOPTIONS_H_
