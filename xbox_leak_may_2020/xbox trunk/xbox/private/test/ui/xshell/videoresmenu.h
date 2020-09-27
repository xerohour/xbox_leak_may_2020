/*****************************************************
*** videoresmenu.h
***
*** Header file for our XShell Video Resolution menu 
*** class.  This menu will allow the user to select
*** different screen resolutions and run the
*** XDK Launcher in that resolution
***
*** by James N. Helm
*** June 9th, 2001
***
*****************************************************/

#ifndef _VIDEORESMENU_H_
#define _VIDEORESMENU_H_

#include "menuscreen.h"

// Sub-Menus

class CVideoResMenu : public CMenuScreen
{
public:
    CVideoResMenu( CXItem* pParent );
    ~CVideoResMenu();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Called whenever this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT Init( char* menuFileName );                 // Initialize our font and settings

private:
    typedef struct _VideoRes
    {
        unsigned int uiHeight;
        unsigned int uiWidth;
        unsigned int uiFlags;
    } VideoRes;

    VideoRes* m_pVideoRes;      // Holds a list of our supported video modes
    unsigned int m_uiNumRes;    // Number of Video Resolution modes we have

    // Private Methods

    void GetVideoResModes();    // This function will enumerate all of the valid video resolution modes
};

#endif // _VIDEORESMENU_H_
