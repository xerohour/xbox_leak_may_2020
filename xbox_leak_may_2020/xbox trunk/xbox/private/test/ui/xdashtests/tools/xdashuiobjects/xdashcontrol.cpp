/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashControl.cpp

Abstract:
    This object can be used to set information about a control that
    lives on a screen within the XDash

Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

Notes:

*/

#include "xdashcontrol.h"


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This is the default constructor.  Initialization of properies is done
  here.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
CXDashControl::CXDashControl()
{
    m_Name = NULL;
    m_Destination = NULL;
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This constructor allows the user to pass in the control name and the
  control destination, so that they can set them at object creation time.

Arguments:
  char* controlName - The name of the control.
  char* controlDestination - The destination the user would end up on if they
                              selected this control.

Return Value:

------------------------------------------------------------------------------*/
CXDashControl::CXDashControl( char* controlName, char* controlDestination )
{
    m_Name = NULL;
    m_Destination = NULL;

    UpdateControl( controlName, controlDestination );
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  The destructor.  Any memory allocation the object creates will be cleaned
  up here.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
CXDashControl::~CXDashControl()
{
    // If we allocated space for a control name,
    // let's clean up
    if( m_Name )
    {
        delete[] m_Name;
        m_Name = NULL;
    }

    // If we allocated space for a control destination,
    // let's clean up
    if( m_Destination )
    {
        delete[] m_Destination;
        m_Destination = NULL;
    }
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will set the control name and control destination.  This is
  called by the user, or the contructor.

Arguments:
  WCHAR* controlName - The name of the control.
  WCHAR* controlDestination - The destination the user would end up on if they
                              selected this control.

Return Value:

------------------------------------------------------------------------------*/
void CXDashControl::UpdateControl( char* controlName, char* controlDestination )
{
    // Check to make sure we were passed in valid parameters, otherwise error and
    // return
    if( !controlName || !controlDestination )
    {
        DebugPrint( "CXDashControl::UpdateControl():Invalid Parameter passed in\r\n" );
        return;
    }

    // If we already have allocated space for a name, let's clean it up so that
    // we can replace it with our new name
    if( m_Name )
    {
        delete[] m_Name;
        m_Name = NULL;
    }

    // Allocate memory for our new name, and copy the information in to it
    m_Name = _strdup( controlName );

    // If we already have allocated space for a destination, let's clean it up so that
    // we can replace it with our new destination
    if( m_Destination )
    {
        delete[] m_Destination;
        m_Destination = NULL;
    }

    // Allocate memory for our new destination, and copy the information in to it
    m_Destination = _strdup( controlDestination );

    // Alert our developer if there was a failure to allocate memory
    if( !m_Name || !m_Destination )
    {
        DebugPrint( "CXDashControl::UpdateControl( char*, char* ): Failed to allocate memory!!\r\n" );
    }
};
