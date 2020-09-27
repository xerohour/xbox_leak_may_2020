/*****************************************************
*** stmenu.h
***
*** Header file for our Soundtracks menu
*** class. This menu class will allow you to perform
*** various soundtrack operations.
***
*** by James N. Helm
*** April 4th, 2001
***
*****************************************************/

#ifndef _STMENU_H_
#define _STMENU_H_

#include "menuscreen.h"  // Base menu type

class CSTMenu : public CMenuScreen
{
public:
    CSTMenu( CXItem* pParent );
    ~CSTMenu();

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
	WCHAR        m_pwszTrackName[MENU_ST_TRACKNAME_SIZE];	// Used to hold our Named Track

    DWORD        m_dwNumSoundtracks;            // Used to store the number of Soundtracks the user wishes to create
    DWORD        m_dwNumTracks;                 // Used to store the number of Tracks the user wishes to create
	DWORD        m_dwTrackLength;				// Used to hold how long a track should be
    DWORD        m_dwCurrentST;                 // Used to store the current Soundtrack ID the user is working with
    HANDLE       m_hThreadHandle;               // Handle to our thread

    CRITICAL_SECTION m_CritSec;                 // Critical Section used to protect Shared Memory
    BOOL         m_bUsingDashSTFuncs;           // Used to determine if we are using the Dash ST functions

    // Private Methods
    // void Leave( CXItem* pItem );                                        // This should be called whenever a user leaves this menu
    BOOL BeginUseDashSTFuncs();
    BOOL EndUseDashSTFuncs();

    void EnterCritSection();
    void LeaveCritSection();

    static friend DWORD WINAPI DataCreateThreadFunc( LPVOID lpParameter );
};

#endif // _MUOPTIONS_H_
