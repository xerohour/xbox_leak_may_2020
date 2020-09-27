/*****************************************************
*** launchmenu.h
***
*** Header file for our XShell Launch menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** December 2nd, 2000
***
*****************************************************/

#ifndef _LAUNCHMENU_H_
#define _LAUNCHMENU_H_

extern BOOL g_bDisplayUpArrow;      // Used to determine if we should render the Up Arrow
extern BOOL g_bDisplayDownArrow;    // Used to determine if we should render the Down Arrow

#include "menuscreen.h"  // Base menu type

// Submenus
#include "optionsmenu.h"
#include "onlinemenu.h"
#include "sysinfomenu.h"

// Thread to enumerate XBEs
#include "xbedirmonitor.h"

class CLaunchMenu : public CMenuScreen
{
public:
    // Constructors and Destructors
    CLaunchMenu( CXItem* pParent );
    ~CLaunchMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT      Init( char* menuFileName );                                    // Initialize the Menu
    unsigned int GetSelectorPos() const { return m_nSelectorPos; };             // Get the index of the selector bar on our screen
    unsigned int GetTopIndexOfItem() const { return m_nTopIndexOfItem; };       // Get the index in our list of the top item that is being displayed on the screen
    unsigned int GetBottomIndexOfItem() const { return m_nBottomIndexOfItem; }; // Get the index in our list of the top item that is being displayed on the screen
    unsigned int GetNumItems() { return m_pXBEList->GetNumItems(); };           // Return the number of menu items on our current screen
    unsigned int GetNumItemsToDisplay();                                        // Get the number of items that should be displayed on the screen
    CXItem* GetItemPtr( enum XShellMenuIds menuID );                            // Returns a pointer to a sub-menu

    HRESULT SetSelectorPos( unsigned int index );                               // Set the Selector position on our screen (index)
    HRESULT SetTopIndexOfItem( unsigned int index );                            // Set the top index of item that should be displayed on our screen
    HRESULT SetBottomIndexOfItem( unsigned int index );                         // Set the bottom index of item that should be displayed on our screen

    void SetDisplayDownArrow( BOOL bChoice ) { g_bDisplayDownArrow = bChoice; };
    void SetDisplayUpArrow( BOOL bChoice )   { g_bDisplayUpArrow = bChoice; };

    static NTSTATUS WriteTitleInfoAndReboot(LPCSTR pszLaunchPath, LPCSTR pszDDrivePath,
        DWORD dwLaunchDataType, DWORD dwTitleId, PLAUNCH_DATA pLaunchData);

private:
    // Vertext structure
    struct TEXVERTEX
    {
	    float       x, y, z, rhw; // position
	    D3DCOLOR    cDiffuse;     // color
        float       tu, tv;       // Texture coordinates
    };

    // The custom FVF, which describes the custom vertex structure.
    #define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

    CXBEList*      m_pXBEList;              // Pointer to an XBEList object used to display information on our menu
    CXBEDirMonitor m_XBEDirMonitor;         // Monitors our XBE directory so that we can deal with changes
    unsigned int   m_nSelectorPos;          // The position of the selector on our screen (index)
    unsigned int   m_nTopIndexOfItem;       // Index in our list of the first item to be displayed on our screen
    unsigned int   m_nBottomIndexOfItem;    // Index in our list of the first item to be displayed on our screen
    BOOL           m_bCheckedConfig;        // Checks if the machine name has been set, if not, on FIRST load, will send the user to config
    unsigned int   m_uiPrevNumItems;        // The number of XBE Items had during our last scan
    IDirect3DVertexBuffer8* m_pVB;          // Vertex Buffer for our texture
    TEXVERTEX      m_TVB[4];                // Texture Vertext Buffer
    char           m_pszSecretKeyCombo[5];  // Used to determine when to display the 'Secret' info screen

    // Menus
    COptionsMenu   m_OptionsMenu;           // Options Menu off of the root
    COnlineMenu    m_OnlineMenu;            // Online Menu off of the root
    CSysInfoMenu   m_SysInfoMenu;           // System Information Menu

    // Methods
    void AdjustDisplayArrows();             // Adjust the helpful scrolling arrows
    void SetInitialSelectState();           // Adjust our menu to it's initial (empty) state
    void Leave( CXItem* pItem );            // This should be called whenever a user leaves this menu
    BOOL ProcessSecretKey();                // Checks the secret key to see if we should display the 'secret' menu
    void ComputeSecretKey( char* Buffer );  // Computes the secret key based upon the MAC Address
};

#endif // _LAUNCHMENU_H_