/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashScreen.cpp

Abstract:
    This object can be used to set information about a screen that
    lives within the XDash.  The information includes the Name of the
    screen, and the controls that live on that screen

Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

Notes:

*/

#include "xdashscreen.h"


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This is the default constructor.  Initialization of properies is done
  here.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
CXDashScreen::CXDashScreen()
{
    // Initialize our memory variables and allocate space
    PropertyInit();
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This constructor will allow the user to pass a screen name in, so that it can
  be set upon object creation.

Arguments:
  char* screenName - The name of the screen

Return Value:

------------------------------------------------------------------------------*/
CXDashScreen::CXDashScreen( char* screenName )
{
    // Initialize our memory variables and allocate space
    PropertyInit();

    // Call the method that will set the screen name
    SetScreenName( screenName );
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  The destructor.  Any memory this object allocates will be cleaned up here.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
CXDashScreen::~CXDashScreen()
{
    // If the user allocated space to hold a screen name,
    // let's clean it up
    if( m_Name )
    {
        delete[] m_Name;
        m_Name = NULL;
    }

    // Clean up the memory we allocated to hold our array of controls
    if( m_Controls )
    {
        delete[] m_Controls;
        m_Controls = NULL;
    }
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will be called by our constructors to initialize our memory
  variables.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
void CXDashScreen::PropertyInit()
{
    m_Name = NULL;
    m_Controls = NULL;
    m_numControls = 0;

    // Allocate memory to hold our array of controls
    m_Controls = new CXDashControl[NUM_CONTROLS];

    // If we failed to allocate memory, let's inform the user
    if( !m_Controls )
    {
        DebugPrint( "**ERROR: CXDashScreen::CXDashScreen(): failed to allocate memory!!" );
    }
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will return the name of the control that is specified by
  the user via an index.

Arguments:
  size_t index - The 0 based index number of the control the user wishes to
                 have the name of.

Return Value:

------------------------------------------------------------------------------*/
char* CXDashScreen::GetControlNameByIndex( size_t index )
{
    // Check to see if the user passed in a valid index.
    // If not, return NULL
    if( index < 0 || index >= GetNumControls() )
    {
        DebugPrint( "CXDashScreen::GetControlNameByIndex( size_t ): index out of range - '%d'\r\n", index );
        return NULL;
    }

    // Return the control at the index the user requested.
    return m_Controls[index].GetControlName();
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will return the destination of the control that is specified by
  the user via an index.

Arguments:
  size_t index - The 0 based index number of the control the user wishes to
                 have the description of.

Return Value:

------------------------------------------------------------------------------*/
char* CXDashScreen::GetControlDestByIndex( size_t index )
{
    // Check to see if the user passed in a valid index.
    // If not, return NULL
    if( index < 0 || index >= GetNumControls() )
    {
        DebugPrint( "CXDashScreen::GetControlDestByIndex( size_t ): index out of range - '%d'\r\n", index );
        return NULL;
    }

    // Return the control at the index the user requested.
    return m_Controls[index].GetControlDest();
};



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will set the name of the screen.  It will allocate and free
  memory as necessary to do this.

Arguments:
  char* screenName - The name of the screen

Return Value:

------------------------------------------------------------------------------*/
void CXDashScreen::SetScreenName( char* screenName )
{
    // Make sure the user passed us a legitimate pointer
    if( !screenName )
    {
        DebugPrint( "CXDashScreen::SetScreenName( char* ):Invalid parameter passed in\r\n" );
        return;
    }

    // If there is already a screen name set, let's clean it up
    if( m_Name )
    {
        delete[] m_Name;
        m_Name = NULL;
    }
    
    // Create memory to hold our new screen name, and copy the data in to it
    m_Name = _strdup( screenName );
}



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will add a control to the control array for this screen.

Arguments:
  char* controlName - The name of the control to be added
  char* controlDestination - The destination of the control to be added.  The
                              destination is the screen name of the screen the
                              user would end up on if they selected this
                              control.

Return Value:

------------------------------------------------------------------------------*/
// This function will let the user add a control to the current screen
void CXDashScreen::AddControl( char* controlName, char* controlDestination )
{
    // Make sure we were passed valid pointers, otherwise let the user know and return
    if( !controlName || !controlDestination )
    {
        DebugPrint( "  CXDashScreen::AddControl( char*, char* ):Invalid parameter passed in\r\n" );
        return;
    }

    // Make sure we haven't exceeded our maximum number of controls.  If not, update the control at the current
    // location with the data the user passed in
    if( m_numControls < NUM_CONTROLS )
    {
        m_Controls[m_numControls].UpdateControl( controlName, controlDestination );

        // Make sure we increase the number of controls
        ++m_numControls;
    }
    else // Too many controls, notify user
    {
        DebugPrint( "  CXDashScreen::AddControl( char*, char* ):Exceeded NUM_CONTROLS - '%d'\r\n", NUM_CONTROLS );
    }
}
