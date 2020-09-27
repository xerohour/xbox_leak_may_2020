/*****************************************************
*** dumpcachemenu.h
***
*** Header file for our XShell Dump Cache partition
*** menu class.  This is a standard menu contains a
*** list of menu items to be selected.
***
*** by James N. Helm
*** May 30th, 2001
***
*****************************************************/

#ifndef _DUMPCACHEMENU_H_
#define _DUMPCACHEMENU_H_

#include "menuscreen.h"

class CDumpCacheMenu : public CMenuScreen
{
public:
    CDumpCacheMenu( CXItem* pParent );
    ~CDumpCacheMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( char* menuFileName );                         // Initialize our font and settings
    void    Leave( CXItem* pItem );                             // Should be called whenever a user deactivates this item
    DWORD   GetSelectedTitleID() { return m_dwTitleID; };       // Return the TitleID of the selected item to dump

private:
    // Private Properties
    DWORD               m_dwTitleID;            // Title ID of the selected item to dump
    unsigned int        m_uiTopItemIndex;       // Index of the top item being displayed
    DWORD               m_dwNumCacheEntries;    // Used to hold the number of cache entries we have
    PX_CACHE_DB_ENTRY   m_pCacheEntries;        // Used to hold our cache entries

    // Private Methods
    void AdjustDisplayArrows();         // Adjust the UP and DOWN arrows on the screen
};

#endif // _CERTMENU_H_
