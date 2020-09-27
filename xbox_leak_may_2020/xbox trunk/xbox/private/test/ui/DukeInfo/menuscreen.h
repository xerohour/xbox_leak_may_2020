/*****************************************************
*** menuscreen.h
***
*** Header file for our XShell Generic menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** December 1st , 2000
***
*****************************************************/

#ifndef _MENUSCREEN_H_
#define _MENUSCREEN_H_

#include "imagefile.h"

class CMenuScreen : public CXItem
{
public:
    // Constructors and Destructors
    CMenuScreen();
    CMenuScreen( CXItem* pParent );
    ~CMenuScreen();

    // Must be overridden for this to be a Item on our XShell program
    virtual void Action( CXBoxVideo* Screen );

    // Public Methods
    virtual HRESULT      Init( CXBoxVideo* pScreen, char* menuFileName );               // Initialize the Menu

protected:
    struct _TEXTITEM
    {
        WCHAR text[SCREEN_ITEM_TEXT_LENGTH];
        float XPos;
        float YPos;
        DWORD fgcolor;
        DWORD bgcolor;
    };

    struct _PANELITEM
    {
        float X1Pos;
        float Y1Pos;
        float X2Pos;
        float Y2Pos;
        unsigned int width;
        DWORD color;
    };

    CLinkedList< struct _TEXTITEM* >  m_textItems;      // Text items that appear on our screen (can be selected)
    CLinkedList< struct _TEXTITEM* >  m_headerItems;    // Header items that appear on our screen (cannot be selected)
    CLinkedList< struct _PANELITEM* > m_panelItems;     // Panel items that appear on our screen
    CLinkedList< struct _PANELITEM* > m_lineItems;      // Lines that appear on our screen
    CLinkedList< struct _PANELITEM* > m_outlineItems;   // Out-line boxes that appear on our screen
    CLinkedList< BitmapFile* >        m_bitmapItems;    // Bitmap image items that appear on our screen

    int            m_nJoystickDeadZone;                 // The dead zone for the joysticks
    int            m_port;                              // The current port the user is viewing
    DWORD          m_keyPressDelayTimer;                // Used to control rapid port changes

    // Methods
    virtual HRESULT ImportScreenItems( char* menuFileName );    // Add item info to the current screen

    virtual HRESULT AddTextItemFromFile( FILE* menuFile );      // Add a Text Item to our Screen from a file
    virtual HRESULT AddPanelItemFromFile( FILE* menuFile );     // Add a Panel Item to our Screen from a file
    virtual HRESULT AddHeaderItemFromFile( FILE* menuFile );    // Add a Header Item to our Screen from a file
    virtual HRESULT AddLineItemFromFile( FILE* menuFile );      // Add a Line Item to our Screen from a file
    virtual HRESULT AddOutlineItemFromFile( FILE* menuFile );   // Add a Outline Item to our Screen from a file
    virtual HRESULT AddBitmapItemFromFile( FILE* menuFile );    // Add a Bitmap Item to our Screen from a file
};

#endif // _MENUSCREEN_H_