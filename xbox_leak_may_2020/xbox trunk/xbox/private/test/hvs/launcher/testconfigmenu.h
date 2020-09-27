/*****************************************************
*** testconfigmenu.h
***
*** Header file for our Test Config Menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** November 27th, 2001
***
*****************************************************/

#ifndef _TESTCONFIGMENU_H_
#define _TESTCONFIGMENU_H_

#include "menuscreen.h"
#include "xbeinfo.h"

const long gc_lTESTCONFIG_NUMBER_BUFZONE =              3500;
const long gc_lTESTCONFIG_NUMBER_MAXSCALE =             100000;
const long gc_lTESTCONFIG_NUMBER_MAXSCALE_DIVISOR =     15;

enum TESTCONFMENU_ACTIONS
{
    ENUM_TESTCONFMENU_NOACTION,
    ENUM_TESTCONFMENU_EDITSTRING2,
    ENUM_TESTCONFMENU_EDIT_NUMBERSTRING,
    ENUM_TESTCONFMENU_EDIT_NUMBER,
    ENUM_TESTCONFMENU_EDIT_RANGE1,
    ENUM_TESTCONFMENU_EDIT_RANGE2
};

class CTestConfigMenu : public CMenuScreen
{
public:
    // Constructors and Destructor
    CTestConfigMenu( void );
    ~CTestConfigMenu( void );

    // Public Methods
    HRESULT Init( XFONT* pMenuItemFont,             // Initialize the Menu
                  XFONT* pMenuTitleFont );
    // Draws a menu on to the screen
    void Action( CUDTexture* pTexture );            // Renders to the texture and calls any per-frame processing
    void RenderScreenItems( CUDTexture* pTexture ); // Render the screen items properly on our menu
	void RenderMenuTitle( CUDTexture* pTexture );	// Render the menu title on to the menu
    void RenderMenuItems( CUDTexture* pTexture );   // Used to render the menu items on the screen
    void HandleInput( enum BUTTONS buttonPressed,   // Handles input from the buttons
                      BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, // Handle input (of the CONTROLS) for the current menu
                      BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick,       // Handle any requests for the joystick (thumb-pad)
                      int nThumbY,
                      int nThumbX,
                      BOOL bFirstYPress,
                      BOOL bFirstXPress );

    void Enter( void );                             // This should be called whenever the menu is entered or activated

    void SetXBEInfoPtr( CXBEInfo* pXBEInfo );       // Set the PTR to our active XBEInfo object
    void SetMultiConfigs( BOOL bConfigs )           // Set whether or not the XBE supports multiple configs
        { m_bMultiConfigs = bConfigs; };

private:
    CXBEInfo* m_pXBEInfo;
    enum TESTCONFMENU_ACTIONS m_eCurrentAction;     // Used to track the current action of the user
    unsigned int m_uiSelectedConfigItem;            // Used to track which config item is selected when using multiple menus
    long m_lNum1Val;                                // Used to store numbers if user is in manual edit mode
    BOOL m_bMultiConfigs;                           // Used to determine if the XBE supports multiple configs

    void Leave( CXItem* pItem );                    // Should be called whenever user is leaving the menu
    void ProcessMenuActions( void );                // If the user is performing an action, this will handle it
    void GoToHelp( void );                          // Sets up and goes to the help menu
};

#endif // _TESTCONFIGMENU_H_