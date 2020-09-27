/*****************************************************
*** muunformat.h
***
*** Header file for our XShell MU UnFormat class.
*** This screen will unformat a selected Memory Unit.
***
*** by James N. Helm
*** June 22nd, 2001
***
*****************************************************/

#ifndef _MUUNFORMAT_H_
#define _MUUNFORMAT_H_

#include "keypadmenu.h"
#include "muunformatthread.h"
#include "memoryunit.h"

#define MUUNFORMAT_NUM_ROWS    2
#define MUUNFORMAT_NUM_COLS    1

class CMUUnFormat : public CKeypadMenu
{
public:
    CMUUnFormat( CXItem* pParent );
    ~CMUUnFormat();

    // Must be overridden for this to be a Item on our XShell program
    void Enter();   // Should be called when this menu is entered / activated
    void Action( CUDTexture* pTexture );
    void HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress );
    void HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, BOOL bFirstYPress, BOOL bFirstXPress );

    // Public Methods
    HRESULT SetMemoryUnit( CMemoryUnit* pMemoryUnit );  // Set the address of the Memory Unit to be used

private:
    CMemoryUnit*        m_pMemoryUnit;              // Active Memory Unit

    BOOL                m_bUnFormatMU;              // Used to signal when the MU should be formatted
    BOOL                m_bUnFormatBegun;           // Used to determine if we have kicked off a format
    BOOL                m_bSleepAndExit;            // Used to signal the menu it's time to pause, and leave
    CMUUnFormatThread   m_MUUnFormatThread;         // Thread object used to format an MU
    BOOL                m_bDisableInput;            // Used to disable user input
    DWORD               m_dwTimeSlept;              // Used to determine how long we've paused

    // Private Methods
    void GenerateKeypad(void);
	BOOL renderKeypad( CUDTexture* pScreen );
    void Leave( CXItem* pItem );        // Will be called whenever the user leaves this menu
};

#endif // _MUUNFORMAT_H_
