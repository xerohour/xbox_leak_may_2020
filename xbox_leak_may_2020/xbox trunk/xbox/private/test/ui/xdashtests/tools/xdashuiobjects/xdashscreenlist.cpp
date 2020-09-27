/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashScreenList.cpp

Abstract:
    This object can be used to hold a list of screens.  a screen consists
    of a name, and a list of controls and their destinations.

Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

Notes:

*/

#include "xdashscreenlist.h"


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This is the default constructor.  Initialization of properies is done
  here.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
CXDashScreenList::CXDashScreenList()
{
    // Initial our member variables
    PropertyInit();
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This constructor will accept an INI File name, and will immediately parse the
  file and populate our lists.

Arguments:
  char* iniFileName - The name of the INI file that contains the screen
                       information

Return Value:

------------------------------------------------------------------------------*/
CXDashScreenList::CXDashScreenList( char* iniFileName )
{
    // Initial our member variables
    PropertyInit();

    // Populate our arrays with the data found in the INI File that was passed in
    GetScreensFromINI( iniFileName );
};



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  The destructor.  Any memory this object allocates will be cleaned up here.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
CXDashScreenList::~CXDashScreenList()
{
    // Clean up the memory we allocated for our screen list
    if ( m_Screens )
    {
        delete[] m_Screens;
        m_Screens = NULL;
    }
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will be called by our constructors to initialize our memory
  variables.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
void CXDashScreenList::PropertyInit()
{
    m_Screens = NULL;   // Set our Screens member to NULL
    m_numScreens = 0;   // The number of screens we currently have in our list
};



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will allow a user to add a screen to their list.  It simply
  needs a name for the screen, which the user passes in.

Arguments:
  char* screenName - The name of the screen to be added to our screen list

Return Value:

------------------------------------------------------------------------------*/
void CXDashScreenList::AddScreenToList( char* screenName )
{
    // Make sure we were passed a valid pointer.  If not, error and bail
    if( !screenName )
    {
        DebugPrint( "CXDashScreenList::AddScreenToList( char* ):Invalid parameter passed in\r\n" );
        return;
    }

    // Make sure we have enough room to add a screen.  If not, error and bail
    if( m_numScreens >= MAX_SCREENS )
    {
        DebugPrint( "CXDashScreenList::AddScreenToList():Exceeded screen list maximum size = '%d'!!\r\n", MAX_SCREENS );
        return;
    }

    // Increment the number of screens the list contains
    ++m_numScreens;

    // Set the name of the screen the user wishes to add
    m_Screens[m_numScreens - 1].SetScreenName( screenName );
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will allow a user to add a control to the current screen.  The
  current screen is the last screen that was added to our list.

Arguments:
  char* controlName - The name of the control to be added to our screen
  char* controlDest - The destination of the control to be added to our screen

Return Value:
------------------------------------------------------------------------------*/
void CXDashScreenList::AddControlToScreen( char* controlName, char* controlDest )
{
    // Make sure we were passed a valid pointer.  If not, inform the user and return
    if( !controlName || !controlDest )
    {
        DebugPrint( "CXDashScreenList::AddControlToScreen( char* ):Invalid parameter passed in\r\n" );
        return;
    }

    // Make sure that there has been at least one screen added before we update controls - If not, inform the
    // user and return
    if( m_numScreens <= 0 )
    {
        DebugPrint( "CXDashScreenList::AddControlToScreen( char* ):No Screens have been added to the list yet!!\r\n" );
        return;
    }

    // Make sure we have allocated memory for our screens -- If not, inform the user and return
    if( !m_Screens )
    {
        DebugPrint( "CXDashScreenList::AddControlToScreen( char* ):m_Screens has not be allocated any memory!!\r\n" );
        return;
    }

    // Add the control to our current screen
    m_Screens[m_numScreens - 1].AddControl( controlName, controlDest );
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will allow a user to get the number of controls that are on the
  current screen.  The current screen is the last screen that was added to our
  list.

Arguments:

Return Value:
  size_t - The number of screens that the current (last) screen has defined
           within its control list

  This function will also return 0 if there are no screens defined in our
  screen list
------------------------------------------------------------------------------*/
size_t CXDashScreenList::GetNumControlsOnCurrentScreen()
{
    // Check to see if there are any screen in our list -- If not, inform the user and return 0
    if( GetNumScreens() == 0 )
    {
        DebugPrint( "CXDashScreenList::GetNumControlsOnCurrentScreen():No screens in list!!\r\n" );
        return 0;
    }

    // Return the number of controls from our current screen
    return m_Screens[m_numScreens - 1].GetNumControls();
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will allow a user to get the number of controls that are on the
  screen specified by an index.  The current screen is the last screen that was added to our
  list.

Arguments:
  size_t index - The 0 based index of the screen they wish to have the number
                 of controls for.

Return Value:
  size_t - The number of screens that the current (last) screen has defined
           within its control list

  This function will also return 0 if the user passes in an invalid index
------------------------------------------------------------------------------*/
size_t CXDashScreenList::GetNumControlsOnScreenByIndex( size_t index )
{
    // Check to see if the index is valid -- If not, inform the user and return 0
    if( index < 0 || index >= GetNumScreens() )
    {
        DebugPrint( "CXDashScreenList::GetNumControlsOnScreenByIndex( size_t ):index out of range - '%d'\r\n", index );
        return 0;
    }

    // Return the number of controls for the specified screen
    return m_Screens[index].GetNumControls();
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will allow a user to get the name of a control from a specific
  screen by specifying the screen index, and the control index within that
  screen.

Arguments:
  size_t screenIndex - The 0 based index of the screen they are interested in
  size_t controlIndex - The 0 based index of the control they are interested in

Return Value:
  char* - A pointer to the name of the control the user is intetersted in.

  If the user passes in an invalid parameter, of if there is an error,
  the function will return NULL.
------------------------------------------------------------------------------*/
char* CXDashScreenList::GetControlNameFromScreenByIndex( size_t screenIndex, size_t controlIndex )
{
    // Check the screen index to ensure it's within range
    if( screenIndex < 0 || screenIndex >= GetNumScreens() )
    {
        DebugPrint( "CXDashScreenList::GetControlNameFromScreenByIndex( size_t, size_t ):screenIndex out of range - '%d'\r\n", screenIndex );
        return NULL;
    }

    // Check the control index to ensure it's within range
    if( controlIndex < 0 || controlIndex >= m_Screens[screenIndex].GetNumControls() )
    {
        DebugPrint( "CXDashScreenList::GetControlNameFromScreenByIndex( size_t, size_t ):controlIndex out of range - '%d'\r\n", controlIndex );
        return NULL;
    }

    // Return the name of the control to the user
    return m_Screens[screenIndex].GetControlNameByIndex( controlIndex );
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will allow a user to get the destination of a control from a 
  specific screen by specifying the screen index, and the control index within
  that screen.

Arguments:
  size_t screenIndex - The 0 based index of the screen they are interested in
  size_t controlIndex - The 0 based index of the control they are interested in

Return Value:
  char* - A pointer to the destination of the control the user is intetersted
           in.

  If the user passes in an invalid parameter, of if there is an error,
  the function will return NULL.
------------------------------------------------------------------------------*/
char* CXDashScreenList::GetControlDestFromScreenByIndex( size_t screenIndex, size_t controlIndex )
{
    // Check the screen index to ensure it's within range
    if( screenIndex < 0 || screenIndex >= GetNumScreens() )
    {
        DebugPrint( "CXDashScreenList::GetControlDestFromScreenByIndex( size_t, size_t ):screenIndex out of range - '%d'\r\n", screenIndex );
        return NULL;
    }

    // Check the control index to ensure it's within range
    if( controlIndex < 0 || controlIndex >= m_Screens[screenIndex].GetNumControls() )
    {
        DebugPrint( "CXDashScreenList::GetControlDestFromScreenByIndex( size_t, size_t ):controlIndex out of range - '%d'\r\n", controlIndex );
        return NULL;
    }

    // Return the destination of the control to the user
    return m_Screens[screenIndex].GetControlDestByIndex( controlIndex );
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will allow a user to get the name of a screen at a specific
  index.

Arguments:
  size_t index - The 0 based index of the screen they are interested in

Return Value:
  char* - A pointer to the name of the screen the user is intetersted in.

  If the user passes in an invalid parameter, of if there is an error,
  the function will return NULL.
------------------------------------------------------------------------------*/
char* CXDashScreenList::GetScreenNameByIndex( size_t index )
{
    // Check to make sure our index is valid -- If not, inform the user and return NULL
    if( index < 0 || index >= GetNumScreens() )
    {
        DebugPrint( "CXDashScreenList::GetScreenNameByIndex( size_t ):index out of range - '%d'\r\n", index );
        return NULL;
    }

    // Return the name of the screen the user is interested in
    return m_Screens[index].GetScreenName();
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will return the name of the current screen.  The current screen
  is the screen that was added last to our list.

Arguments:

Return Value:
  char* - A pointer to the name of the current (last) screen

  If there is an error, the function will return NULL.
------------------------------------------------------------------------------*/
char* CXDashScreenList::GetCurrentScreenName()
{
    // Make sure we have a current screen -- If not, inform the user, and return NULL
    if( m_numScreens == 0 )
    {
        DebugPrint( "CXDashScreenList::GetCurrentScreenName():No screens in list!!\r\n" );
        return NULL;
    }

    // Return the name of the current screen
    return m_Screens[m_numScreens - 1].GetScreenName();
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will return the name of a control specified by the user, from
  the current screen.  The current screen is the screen that was added last to
  our list.

Arguments:
  size_t index - The 0 based index of the control from the current screen that
                 the user is interested in.

Return Value:
  char* - A pointer to the name of the control that the user is interested in

  If there is an error, the function will return NULL.
------------------------------------------------------------------------------*/
char* CXDashScreenList::GetControlFromCurrentScreenByIndex( size_t index )
{
    // Check to make sure we have some screens in our list -- If not, inform user and return NULL
    if( 0 == m_numScreens )
    {
        DebugPrint( "CXDashScreenList::GetControlFromCurrentScreen( size_t ):No screen in list!!\r\n" );
        return NULL;
    }

    // Check to make sure the index is valid -- If not, inform user and return NULL
    if( index < 0 || index >= m_Screens[m_numScreens - 1].GetNumControls() )
    {
        DebugPrint( "CXDashScreenList::GetControlFromCurrentScreen( size_t ):index out of range - '%d'\r\n", index );
        return NULL;
    }

    // Return the name of the control to the user
    return m_Screens[m_numScreens - 1].GetControlNameByIndex( index );
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will parse an INI file, and populate our data structures with
  the information from the file.

Arguments:
  char* iniFileName - The name of the INI file that the function should
                       parse

Return Value:
------------------------------------------------------------------------------*/
void CXDashScreenList::GetScreensFromINI( char* iniFileName )
{
    // If the user did not pass a valid file name, inform them and return
    if( !iniFileName )
    {
        DebugPrint("CXDashScreenList::GetScreensFromINI( char* ):iniFileName not set!!\r\n" );
        return;
    }

    // Check to see if we've already allocated memory for our screens.  If so, delete
    // the memory we've allocated, and create new space
    if( m_Screens )
    {
        delete[] m_Screens;
        m_Screens = NULL;
    }

    // Allocate memory to hold our screens
    m_Screens = new CXDashScreen[MAX_SCREENS];

    // If the memory allocation failed, inform user and return
    if( !m_Screens )
    {
        DebugPrint("**ERROR: CXDashScreenList::GetScreensFromINI( char* ):Couldn't allocate memory!!\r\n" );
        return;
    }

    FILE* iniFile = NULL;           // The stream we will get when opening the INI file
    char fileLine[MAX_LINE_SIZE];   // Used to hold each line from the INI file as we read it in
    char* subString = NULL;         // Used several times to parse the 'fileLine' from the INI file
    BOOL bReturnEarly = false;      // Used if we have a problem, and want to return early but STILL CLEAN UP

    // Zero our our file buffer memory (helps for debugging if there is a problem down the road)
    ZeroMemory( fileLine, sizeof(char) * MAX_LINE_SIZE );
    
    // Open the INI file for reading
    iniFile = fopen( iniFileName, "r" );

    // If there was a problem opening the INI file, inform the user and return
    if( !iniFile )
    {
        DebugPrint("CXDashScreenList::GetScreensFromINI( char* ):Could not open INI file - '%S'\r\n", iniFileName );
        return;
    }

    // Read in lines from the INI file until we reach the end of file (fgetws will return NULL)
    while( fgets( fileLine, MAX_LINE_SIZE, iniFile ) )
    {
        // If it's a comment line, skip to the next line of the file before doing ANY processing
        if( fileLine[0] == COMMENT_CHAR_A )
        {
            continue;
        }

        // Remove any newline characters from the line, so they are not added to our arrays (fgetws leaves newlines in)
        NullCharInString( fileLine, '\n' );

        // If the line contains the NAME_TOKEN, add a new screen to our list
        if( subString = strstr( fileLine, NAME_TOKEN_A ) )
        {
            AddScreenToList( subString + strlen( NAME_TOKEN_A ) );
        }
        else if( subString = strstr( fileLine, CONTROL_TOKEN_A ) )  // If the line contains the CONTROL_TOKEN, add a new control with its destination to our current screen
        {
            char* controlName = _strdup( subString + strlen( CONTROL_TOKEN_A ) );    // The name of our control minus the CONTROL_TOKEN from the INI file (stored since we will overwrite fileLine)
            char* lpDestToken = NULL;                                                // Used to determine if the line from the INI file contains the destination of the current control we are dealing with

            subString = NULL;   // Set subString back to NULL now that we are finished with it

            // Make sure we could allocate memory to hold our control name -- If not, inform user and return early
            if( !controlName )
            {
                DebugPrint("**ERROR: CXDashScreenList::GetScreensFromINI( char* ):Couldn't allocate memory!!\r\n" );
                bReturnEarly = true;
            }

            // Make sure there wasn't a problem above, and continue if there was not
            if( !bReturnEarly )
            {
                // Read the next line of the file (which is REQUIRED to be a ControlDestination) -- If not, inform user and return early
                if( !fgets( fileLine, MAX_LINE_SIZE, iniFile ) )
                {
                    DebugPrint("**CXDashScreenList::GetScreensFromINI( char* ):Found control with no destination - 1!!\r\n" );
                    bReturnEarly = true;
                }

                if( !bReturnEarly )
                {
                    // Remove any newline characters from the line, so they are not added to our arrays (fgetws leaves newlines in)
                    NullCharInString( fileLine, '\n' );

                    // search for our CONTROL_DEST_TOKEN from our file line
                    lpDestToken = strstr( fileLine, CONTROL_DEST_TOKEN_A );

                    // Make sure we found the token, if not, error and bail
                    // If so, add the control and the destination to the list of screens
                    if( !lpDestToken )
                    {
                        DebugPrint("CXDashScreenList::GetScreensFromINI( char* ):Found control with no destination - 2!!\r\n" );
                        bReturnEarly = true;
                    }

                    // Make sure there wasn't a problem above, and continue with the processing
                    if( !bReturnEarly )
                    {
                        // Let's get the actual name of the Control destination from the INI file
                        // Make sure our call succeeded, otherwise use what was pulled from the INI
                        char* lpControlDest = NULL;    // Used to hold the destination of our control from the INI

                        // Get the value of the screen destination from the INI file, so that names of screens are only recorded ONCE in an INI file
                        // If there was a problem, or the name wasn't found, we will use what was pulled from the INI file (user override)
                        if( lpControlDest = GetValueFromINI( iniFileName, lpDestToken + strlen( CONTROL_DEST_TOKEN_A ), "Name", MAX_LINE_SIZE ) )
                        {
                            AddControlToScreen( controlName, lpControlDest );

                            // Clean up the memory allocated by the call to GetPrivateProfileString
                            delete[] lpControlDest;
                        }
                        else
                        {
                            // Add the control using the original information pulled from the INI
                            AddControlToScreen( controlName, lpDestToken + strlen( CONTROL_DEST_TOKEN_A ) );
                        }
                    }
                }
            }
            
            // If we allocated memory, let's clean up
            if( controlName )
            {
                delete[] controlName;
                controlName = NULL;
            }

            // If the user wishes to return, we should break out of the loop so that we can clean up properly and exit
            // of the INI file
            if( bReturnEarly )
            {
                break;
            }
        }
    }

    // Close the file we opened
    if( iniFile )
    {
        fclose( iniFile );
    }

};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will get in to INI file specified by the user and return
  value.

Arguments:
  char* iniFileName - INI File the user wishes to parse
  char* iniSection - INI Section the user wishes to search
  char* iniKey - INI Key the user wishes to get the value of
  size_t bufferSize - Size of the buffer that user has to hold the information
                      found in the INI

Return Value:
  char* - pointer to the value that was found in the INI.  The caller is
          responsible for cleaning up memory allocated by this function

  If there is an error, NULL is returned
------------------------------------------------------------------------------*/
char* CXDashScreenList::GetValueFromINI( char* iniFileName, char* iniSection, char* iniKey, size_t bufferSize )
{
    BOOL bReturnEarly = false;

    // Check parameters
    if( !iniFileName || !iniSection || !iniKey )
    {
        DebugPrint("CXDashScreenList::GetValueFromINI():Invalid parameters passed in!!\r\n" );
        return NULL;
    }
    
    // Local Variables
    char* lpControlDest = new char[bufferSize];       // The control destination as pulled from the ini

    // Make sure we could allocate memory for our buffer -- If not, inform the user and exit
    if( !lpControlDest )
    {
        DebugPrint("**ERROR: CXDashScreenList::GetValueFromINI():Couldn't allocate memory - lpControlDest!!\r\n" );
        return NULL;
    }

    // Zero out our buffer memory (helps with debugging)
    ZeroMemory( lpControlDest, sizeof(char) * bufferSize );

    // Make sure our call succeeds, otherwise Error and bail
    if( 0 == GetPrivateProfileStringA( iniSection, iniKey, "Not Found", lpControlDest, bufferSize, iniFileName ) )
    {
        DebugPrint("CXDashScreenList::GetValueFromINI():Didn't find value in INI!  INI File: '%S', Section: '%S', Key: '%S', BufferSize: '%d'\r\n", iniFileName, iniSection, iniKey, bufferSize );
        bReturnEarly = true;
    }

    // Return the pointer to the value that was pulled from the INI file
    return lpControlDest;
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This function will NULL out the first instance of a user passed in character.

Arguments:
  char* targetString - Pointer to the targetString the user wishes to search
  char charToNull  - The character the user wishes to null out in the
                     targetString

Return Value:
  size_t - Will return a 0 if the character wasn't found, and a 1 if the
           character was found and nulled out.

  If there is an error, 0 is returned
------------------------------------------------------------------------------*/
size_t CXDashScreenList::NullCharInString( char* targetString, char charToNull )
{
    char* subString = NULL;     // Used to parse our targetString
    size_t returnVal = 0;       // Return value

    // Ensure we have a valid pointer -- If not, inform user and return 0
    if( !targetString )
    {
        DebugPrint( "CXDashScreenList::NullCharInString():Invalid parameters passed in\r\n" );
        return returnVal;
    }

    // See if there are any instances of the character in our string
    subString = strchr( targetString, charToNull );
    if( subString ) // If one is found, convert it to a NULL character
    {
        *subString = '\0';
        subString = NULL;   // Set our char* back to null for the next use
        returnVal = 1;      // Set our return value to 1 since we found a character
    }

    return returnVal;
}