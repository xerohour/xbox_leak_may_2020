/*****************************************************
*** certmenu.h
***
*** Header file for our XShell Certification menu 
*** class.  This is a standard menu contains a list
*** of menu items to be selected.
***
*** by James N. Helm
*** May 26th, 2001
***
*****************************************************/

#ifndef _CERTMENU_H_
#define _CERTMENU_H_

#include "menuscreen.h"
#include "hexkeypadmenu.h"      // Used to get Title ID's
#include "dumpcachemenu.h"      // Used to dump the cache partition for a specified Title
#include "languagemenu.h"       // Used to set the Xbox Language
#include "timezonemenu.h"       // Used to set the Xbox TimeZone
#include "eepromsettingsmenu.h" // Used to display the EEPROM Settings

#include "certreqs.h"       // Certifcation Requirements Configuration Object

class CCertMenu : public CMenuScreen
{
public:
    CCertMenu( CXItem* pParent );
    ~CCertMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( char* menuFileName );     // Initialize our font and settings
    void    Leave( CXItem* pItem );         // Should be called whenever a user deactivates this item
private:
    // Private Properties
    unsigned int    m_uiTopItemIndex;   // Index of the top item being displayed
    CCertReqs       m_CertReqs;         // Used to configure the Xbox for certain Cert Requirments
    BOOL            m_bProcessing;      // Used to determine if the menu is processing a user command
    BOOL            m_bStartThread;     // Used to determine when we should kick off our thread
    BOOL            m_bDisableInput;    // Used to determine if the menu should continue processing input
    HANDLE          m_hThread;          // Handle for our processing thread
    BOOL            m_bAborting;        // Used to determine if we are aborting a process

    WCHAR           m_pwszTitleID[MENU_CERT_TITLE_ID_STR_LEN+1];   // Used to hold our Title ID
    BOOL            m_bDispHDTVSafe;    // Used to determine if we should display the HDTV Safe Area
    BOOL            m_bDispNonHDTVSafe; // Used to display the Non-HDTV Safe Area

    CHexKeypadMenu      m_HexKeypadMenu;    // Used to get Title ID's
    CDumpCacheMenu      m_DumpCacheMenu;    // Used to prompt the user to delete a particular cache partition
    CLanguageMenu       m_LanguageMenu;     // Used to set the Xbox Language
    CTimeZoneMenu       m_TimeZoneMenu;     // Used to set the Xbox Time Zone
    CEEPROMSettingsMenu m_EEPROMMenu;       // Used to display the EEPROM Settings

    // Private Methods
    void AdjustDisplayArrows();                     // Adjust the UP and DOWN arrows on the screen
    void DrawBox( float x1,                         // Draw a box on the screen of the specified color
                  float y1,
                  float x2,
                  float y2,
                  DWORD dwColor );
    void DrawBoxPercentBased( float fPercentage,    // Draw a box on the screen based on the percentage passed in
                              DWORD dwColor );

    static friend DWORD WINAPI ProcessDataThreadFunc( LPVOID lpParameter );  // Thread Function
};

#endif // _CERTMENU_H_
