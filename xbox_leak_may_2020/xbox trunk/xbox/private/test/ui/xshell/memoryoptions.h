/*****************************************************
*** memoryoptions.h
***
*** Header file for our XShell Memory Options class.
*** This screen will allow the user to select an
*** action that they would like to perform on a
*** memory area
***
*** by James N. Helm
*** February 15th, 2001
***
*****************************************************/

#ifndef _MEMORYOPTIONS_H_
#define _MEMORYOPTIONS_H_

#include "keypadmenu.h"

#include "memoryunit.h"

// Sub-Menus
#include "hexkeypadmenu.h"
#include "muformat.h"
#include "muunformat.h"
#include "muname.h"

#define MEMORYOPTIONS_NUM_ROWS    2
#define MEMORYOPTIONS_NUM_COLS    2

class CMemoryOptions : public CKeypadMenu
{
public:
    CMemoryOptions( CXItem* pParent );
    ~CMemoryOptions();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();                                       // Should be called when this menu is entered / activated
    
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
    HRESULT SetMemoryUnit( CMemoryUnit* pMemoryUnit );  // Set the address of the Memory Unit to be used
    HRESULT Init( char* menuFileName );                 // Initialize the Menu

private:
    CMemoryUnit*    m_pMemoryUnit;          // Active Memory Unit
    WCHAR           m_pwszBlockCount[4];    // Used to hold our block count for MUs
    BOOL            m_bCollectingBlocks;    // Used to determine the block size of the file to create

    // Menus
    CMUFormat       m_MUFormatDlg;          // The dialog-like menu to Format an MU
    CMUUnFormat     m_MUUnFormatDlg;        // The dialog-like menu to Format an MU
    CMUName         m_MUNameDlg;            // The dialog-like menu to Name an MU
    CHexKeypadMenu  m_HexKeypadMenu;        // Used to get Title ID's

    // Private Methods
    void GenerateKeypad(void);
	BOOL renderKeypad( CUDTexture* pScreen );
    void Leave( CXItem* pItem );    // This will be called whenever this menu is exited
};

#endif // _MEMORYOPTIONS_H_
