/*****************************************************
*** passcodemenu.h
***
*** Header file for our Passcode Menu class.
***
*** by James N. Helm
*** February 13th, 2002
***
*****************************************************/

#ifndef _PASSCODEMENU_H_
#define _PASSCODEMENU_H_

#include "menuscreen.h"  // Base menu type

class CPassCodeMenu : public CMenuScreen
{
public:
    // Constructors and Destructors
    CPassCodeMenu( CXItem* pParent );
    virtual ~CPassCodeMenu();

    // Process drawing and input for a menu screen
    virtual void Enter();   // This will be called whenever this menu is entered / activated
    virtual void Action( CUDTexture* pTexture );
    virtual void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    virtual void HandleInput( enum CONTROLS buttonPressed, BOOL bFirstPress );

    virtual HRESULT Init( char* menuFileName );             // Initialize the Menu

    BOOL GetCancelled() { return m_bCancelled; };           // Used to determine if the user cancelled the passcode menu
    BYTE* GetValue() { return m_pbyPassCode; };             // Return a pointer to the user entered passcode

protected:
    // Properties
    BYTE            m_pbyPassCode[XONLINE_PIN_LENGTH];
    unsigned short  m_usPassCodeIndex;                      // Used to track which character of the passcode we are working with
    BOOL            m_bCancelled;                           // Used to determine if the user cancelled the Pass Code Menu
    XFONT*          m_pButtonFont;                          // Font used to display button presses
    BOOL            m_bDisplayInvalidMsg;                   // Used to display the invalid passcode message

    // Methods
    void Leave( CXItem* pItem );                            // This will be called when the user leaves this menu
};

#endif // _PASSCODEMENU_H_