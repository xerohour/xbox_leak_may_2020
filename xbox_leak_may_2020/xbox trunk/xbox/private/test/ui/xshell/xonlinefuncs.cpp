/////////////////////////////////////////////////////////
// Much of the code in this file was originally written
// by Dan Rosenstein (danrose)
//
// It was copied and modified by James Helm (jimhelm)
// for use in the XDK Launcher on 2/12/2002
/////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xonlinefuncs.h"

static XONLINETASK_HANDLE g_hOnlineTask = NULL;  // Global Online Task handle
static char g_pszOnlineClusterName[MAX_PATH+1];
static char g_pszOnlineRealmName[MAX_PATH+1];

///////////////////////////////////////////////////////////////////////////////
// Name: QuickLoginStart
// Description: does a quick logon to the user account creation server
///////////////////////////////////////////////////////////////////////////////
HRESULT OnlineQuickLoginStart( void )
{
	//
	// handles to communicate with Xbox Online
	//

	HRESULT hr = S_OK;
	HRESULT hrTask = S_OK;
	HANDLE hEvent = NULL;

	DWORD dwWait = 0;

	// create the event handle, make sure it is valid
	hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( NULL == hEvent )
        return( E_FAIL );

	// since no users will be logged in (this is just a ping), we
	// want to only hit the user account server
    DWORD aServices[] = { XONLINE_USER_ACCOUNT_SERVICE };

	// create the user array, zero it out
	XONLINE_USER aUsers[XONLINE_MAX_LOGON_USERS];
	ZeroMemory( aUsers, sizeof( aUsers ) );

	// generate the async logon to the user account service
	hr = XOnlineLogon( aUsers, 
                       aServices,
                       sizeof( aServices ) / sizeof( DWORD ),
                       hEvent,
                       &g_hOnlineTask );

    // pump the task handle until logon completes
	if ( SUCCEEDED( hr ) )
	{
		do
		{   
			// wait for data to be ready for processing
			dwWait = WaitForSingleObject( hEvent, ONLINE_ACCOUNT_WAIT_INTERVAL );

			// see if the wait failed due to memory
			if ( WAIT_FAILED == dwWait )
            {
				hr = E_FAIL;
                break;
            }

			// see if the wait timed out
			if ( WAIT_TIMEOUT == dwWait )
            {
				hr = E_FAIL;
                break;
            }

			// continue pumping the handle
			hrTask = XOnlineTaskContinue( g_hOnlineTask );

		} while( hrTask == XONLINETASK_S_RUNNING ); 
	}

    // close the event
    if ( NULL != hEvent )
    {
        CloseHandle( hEvent );
        hEvent = NULL;
    }

    // Return the proper error code
    if( XONLINETASK_S_RUNNING == hrTask )
        return( hr );
    else
        return XOnlineLogonTaskGetResults( g_hOnlineTask );
}


///////////////////////////////////////////////////////////////////////////////
// Name: QuickLoginFinish
// Description: does a quick logoff to the user account creation server
///////////////////////////////////////////////////////////////////////////////
HRESULT OnlineQuickLoginFinish( void )
{
	HRESULT hr = S_OK;

	// close the handle, wether we have falied or not
	if ( NULL != g_hOnlineTask )
    {
		hr = XOnlineTaskClose( g_hOnlineTask );
        g_hOnlineTask = NULL;
    }

	// spit back the hr
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GenerateUserName
// Input: wszUserName - a WCHAR array of XONLINE_NAME_SIZE bytes to get 
//        the new user name                      
// Description: create a random user name based on the Ethernet MAC address
//              and 7 bytes of charecter values
///////////////////////////////////////////////////////////////////////////////
HRESULT OnlineGenerateUserName( WCHAR wszName[XONLINE_NAME_SIZE] )
{
    if( NULL == wszName )
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    char pszUserName[XONLINE_NAME_SIZE];
    pszUserName[XONLINE_NAME_SIZE - 1] = '\0';

    hr = OnlineGenerateUserName( pszUserName );
    if( FAILED( hr ) )
        return hr;

    // Convert the user name to a WCHAR string and copy it to the users string
    swprintf( wszName, L"%hs", pszUserName );

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GenerateUserName
// Input: szUserName - an array of XONLINE_NAME_SIZE bytes to get 
//        the new user name                      
// Description: create a random user name based on the Ethernet MAC address
//              and 7 bytes of charecter values
///////////////////////////////////////////////////////////////////////////////
HRESULT OnlineGenerateUserName( CHAR szName[XONLINE_NAME_SIZE] )
{
    if( NULL == szName )
        return E_INVALIDARG;

	HRESULT hr = S_OK;

	DWORD dwStatus = 0;
	DWORD dwType = 0;
	DWORD cbResultLength = 0;

	CHAR abMacAddress[6];
    ZeroMemory( abMacAddress, 6 );

    // Clear out the memory that was passed to us
    ZeroMemory( szName, XONLINE_NAME_SIZE );

	// get the Ethernet MAC address
	dwStatus = ExQueryNonVolatileSetting( XC_FACTORY_ETHERNET_ADDR, &dwType, abMacAddress, 6, &cbResultLength );
	hr = NT_SUCCESS( dwStatus ) ? S_OK : E_FAIL;

    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "Could not get ethernet MAC address!! Error - '0x%08X', '%d'", hr, hr );
        return hr;
    }

	// populate user name with the last 4 bytes of the ethernet mac address
	for ( DWORD i = 0; i < 4; i++ )
        sprintf( szName + i * 2, "%02x", (BYTE) abMacAddress[i + 2] );  // print the current byte

	// seed the random number generator
	srand( time( NULL ) );

	// stuff the last 7 bytes with a random char
	sprintf( szName + 8,
             "%c%c%c%c%c%c%c", 
             (BYTE) rand() % ( 'Z' - 'A' ) + 'A',
             (BYTE) rand() % ( 'Z' - 'A' ) + 'A',
             (BYTE) rand() % ( 'Z' - 'A' ) + 'A',
             (BYTE) rand() % ( 'Z' - 'A' ) + 'A',
             (BYTE) rand() % ( 'Z' - 'A' ) + 'A', 
             (BYTE) rand() % ( 'Z' - 'A' ) + 'A', 
             (BYTE) rand() % ( 'Z' - 'A' ) + 'A' );

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
// Name: GenerateUserAccount
// Input: pOnlineUser - the user to fill in                     
// Description: fill in and create a user
///////////////////////////////////////////////////////////////////////////////
HRESULT OnlineGenerateUserAccount( PXONLINE_USER pOnlineUser )
{
	HRESULT hr = S_OK;

    HRESULT hrCreateResult = S_OK;      // This will be used to return the actual return code of the account creation
	HRESULT hrTask = S_OK;
	HANDLE hEvent = NULL;

	XONLINETASK_HANDLE hTask = NULL;

	DWORD dwWait = 0;

	HRESULT hrGenName = S_OK;

    // Make sure we have room on the hard drive to store a new user
    DWORD dwNumUsers;
    XONLINE_USER aExistingUsers[XONLINE_MAX_STORED_ONLINE_USERS];

    hr = OnlineGetUsersFromHD( (PXONLINE_USER) aExistingUsers, &dwNumUsers );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():OnlineGetUsersFromHD Failed!! - '0x%08X', '%d'", hr, hr );
        return E_FAIL;
    }

    if( XONLINE_MAX_HD_ONLINE_USERS == dwNumUsers )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():Max users (%u) already exist on the hard drive!!", dwNumUsers );
        // BUGBUG: TODO: Return an error code so that the app can determine the hard drive is full!!
        return E_FAIL;
    }

    // Logon
    hr = OnlineQuickLoginStart();
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():Unable to logon!! - '0x%08X', '%d'", hr, hr );
        if( FAILED( OnlineQuickLoginFinish() ) )
            XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():OnlineQuickLoginFinish failed!!" );
        return hr;
    }

	// create the event handle, make sure it is valid
	hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( NULL == hEvent )
        return E_FAIL;

	// create the account
	hr = _XOnlineAccountTempCreate( (PXONLINE_USER) pOnlineUser, hEvent, &hTask );
    if( FAILED( hr ) )
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():_XOnlineAccountTempCreate Failed!! Error - '0x%08X', '%d'", hr, hr );

    if( SUCCEEDED( hr ) )
    {
	    // pump the task handle until user creation completes
	    do
	    {   
		    // wait for data to be ready for processing
		    dwWait = WaitForSingleObject( hEvent, ONLINE_ACCOUNT_WAIT_INTERVAL );

            if ( WAIT_FAILED == dwWait )
		    {
                hr = E_FAIL;
                break;
		    }
		    else if ( WAIT_TIMEOUT == dwWait )  // make sure the wait did not time out
		    {
			    hr = E_FAIL;
                break;
		    }

		    hrTask = XOnlineTaskContinue( hTask );  // continue pumping the handle
	    } while( XONLINETASK_S_RUNNING == hrTask );

        hrCreateResult = hrTask;
    }

    // if the name was taken, zero it out
    // TODO: Use this in an error message
	// if( XUAC_E_NAME_TAKEN == hrTask )

	// Now retrieve the created account
    if ( SUCCEEDED( hr ) && SUCCEEDED( hrCreateResult ) )
    {
		hr = _XOnlineAccountTempCreateGetResults( hTask, (PXONLINE_USER) pOnlineUser );
        if( FAILED( hr ) )
            XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():Creation of temp account failed!! Error - '0x%08X', '%d'", hr, hr );

        /*
        // close the handle, even if we have failed
	    if ( NULL != hTask )
	    {
		    hrTask = XOnlineTaskClose( hTask );
            if( FAILED( hrTask ) )
                XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():Could not close handle!! Error - '0x%08X', '%d'", hrTask, hrTask );
            else
		        hTask = NULL;
	    }
        */

        // Add the account to the hard drive
        if( SUCCEEDED( hr ) )
        {
            hr = _XOnlineAddUserToHD( (PXONLINE_USER) pOnlineUser );
            if( FAILED( hr ) )
                XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():Creation of temp account failed!! Error - '0x%08X', '%d'", hr, hr );

        }
    }

    // close down the handle
	if ( NULL != hTask )
	{
		hrTask = XOnlineTaskClose( hTask );
        if( FAILED( hrTask ) )
            XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():Could not close handle!! Error - '0x%08X', '%d'", hrTask, hrTask );
        else
		    hTask = NULL;
	}

    // Logoff
    hr = OnlineQuickLoginFinish();
    if( FAILED( hr ) )
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGenerateUserAccount():Failed to logoff!! - '0x%08X', '%d'", hr, hr );

    // close the event
    if ( NULL != hEvent )
    {
        CloseHandle( hEvent );
        hEvent = NULL;
    }

    // Return the proper error code
    if( XONLINETASK_S_RUNNING == hrCreateResult )
        return( hr );
    else
        return( hrCreateResult );
}

// Will retrieve users from the hard drive
HRESULT OnlineGetUsersFromHD( PXONLINE_USER paOnlineUsers, DWORD* pdwNumUsers )
{
    if( ( NULL == paOnlineUsers ) || ( NULL == pdwNumUsers ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGetNumHDUsers():Invalid argument(s) passed in!!" );
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    // Store 0 in the number of users (Not sure what the XOnline API does in a failure case)
    *pdwNumUsers = 0;

    // Zero out our array of users
    ZeroMemory( paOnlineUsers, sizeof( XONLINE_USER ) * XONLINE_MAX_STORED_ONLINE_USERS );
    
    hr = _XOnlineGetUsersFromHD( (PXONLINE_USER) paOnlineUsers, pdwNumUsers );
    if( FAILED( hr ) )
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGetUsersFromHD():Failed to get the users from the hard drive!!" );

    return hr;
}


// Will remove the specified user from the hard drive
HRESULT OnlineRemoveUserFromHD( PXONLINE_USER pOnlineUser )
{
    if( NULL == pOnlineUser )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineRemoveUserFromHD():Invalid argument(s) passed in!!" );
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    hr = _XOnlineRemoveUserFromHD( pOnlineUser );
    if( FAILED( hr ) )
        XDBGWRN( APP_TITLE_NAME_A, "OnlineRemoveUserFromHD():_XOnlineRemoveUserFromHD Failed! - '0x%08X', '%d'", hr, hr );

    return hr;
}


// Gets the user information from an MU and returns it
// We're wrapping the call to the XOnline API in case that API changes in the future
HRESULT OnlineGetUserFromMU( DWORD dwPort, DWORD dwSlot, PXONLINE_USER pOnlineUser, BOOL* pbUserExists/*=NULL*/ )
{
    if( NULL == pOnlineUser )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGetUserFromMU():Invalid argument(s) passed in!!" );
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    // Zero out the online user memory passed in
    ZeroMemory( pOnlineUser, sizeof( XONLINE_USER ) );

    hr = _XOnlineGetUserFromMU( dwPort, dwSlot, (PXONLINE_USER) pOnlineUser );
    if( FAILED( hr ) && ( XONLINE_E_NO_USER != hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGetUserFromMU():_XOnlineGetUserFromMU Failed!! - '0x%08X', '%d'", hr, hr );
    }
    else // Success or No User
    {
        if( XONLINE_E_NO_USER == hr )  // Check to see if the Online API returned "No User"
        {
            hr = S_OK;  // Don't return an error in this case
            if( NULL != pbUserExists)
                *pbUserExists = FALSE;
        }
        else // A user does exist on the MU
        {
            if( NULL != pbUserExists)
                *pbUserExists = TRUE;
        }
    }

    return hr;
}


// Will remove the user from the specified MU
HRESULT OnlineClearUserFromMU( char cDriveLetter )
{
    XONLINE_USER OnlineUser;
    ZeroMemory( &OnlineUser, sizeof( OnlineUser ) );

    return OnlineSetUserInMU( cDriveLetter, &OnlineUser );
}


// Will store the specified user on the specified MU
HRESULT OnlineSetUserInMU( char cDriveLetter, PXONLINE_USER pOnlineUser )
{
    if( NULL == pOnlineUser )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineSetUserInMU():Invalid argument(s) passed in!!" );
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    hr = _XOnlineSetUserInMU( cDriveLetter, (PXONLINE_USER) pOnlineUser );
    if( FAILED( hr ) )
        XDBGWRN( APP_TITLE_NAME_A, "OnlineSetUserInMU():_XOnlineSetUserInMU Failed!! - '0x%08X', '%d'", hr, hr );

    return hr;
}


// Will add a user to the hard drive
HRESULT OnlineAddUserToHD( PXONLINE_USER pOnlineUser )
{
    if( NULL == pOnlineUser )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineAddUserToHD():Invalid argument(s) passed in!!" );
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    hr = _XOnlineAddUserToHD( pOnlineUser );
    if( FAILED( hr ) )
        XDBGWRN( APP_TITLE_NAME_A, "OnlineAddUserToHD():_XOnlineAddUserToHD Failed!! - '0x%08X', '%d'", hr, hr );

    return hr;
}


// Will return true if the user already exists on the Hard Drive
HRESULT OnlineIsUserOnHD( char* pszUserName, BOOL* pbUserExists )
{
    if( ( NULL == pszUserName ) || ( NULL == pbUserExists ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineIsUserOnHD():Invalid argument(s) passed in!!" );
        return E_INVALIDARG;
    }

    *pbUserExists = FALSE;
    DWORD dwNumUsers;
    XONLINE_USER  aOnlineUsers[XONLINE_MAX_STORED_ONLINE_USERS];

    HRESULT hr = OnlineGetUsersFromHD( (PXONLINE_USER) aOnlineUsers, &dwNumUsers );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineIsUserOnHD():OnlineGetUsersFromHD Failed!! - '0x%08X', '%d'", hr, hr );
    }
    else
    {
        if( 0 == dwNumUsers )
        {
            XDBGWRN( APP_TITLE_NAME_A, "OnlineIsUserOnHD():No Users Found" );
        }
        else
        {
            for( unsigned int x = 0; x < dwNumUsers; x++ )
            {
                if( 0 == _stricmp( aOnlineUsers[x].name, pszUserName ) )
                {
                    *pbUserExists = TRUE;
                    break;
                }
            }
        }
    }

    return hr;
}


// Used to determine if a user name is valid before creating
BOOL OnlineIsNameValid( WCHAR* pwszUserName )
{
    if( NULL == pwszUserName ) 
        return FALSE;

    // Check the length
    unsigned int uiStrLength = wcslen( pwszUserName );
    if(  uiStrLength < ONLINE_USERNAME_MINIMUM_SIZE )
        return FALSE;

    // Make sure the name begins with a letter
    if( ( pwszUserName[0] < 'A' || pwszUserName[0] > 'z' ) ||
        ( pwszUserName[0] > 'Z' && pwszUserName[0] < 'a' ) )
        return FALSE;

    // Check for invalid characters
    for( unsigned int x = 0; x < uiStrLength; x++ )
    {
        // If invalid character, return FALSE
        if( NULL != wcschr( ONLINE_USERNAME_INVALID_CHARACTERS, pwszUserName[x] ) )
            return FALSE;
    }

    return TRUE;
}

// Get the current Realm users should be in
char* OnlineGetCurrentRealm()
{
    static bParsedINIForRealm = FALSE;

    if( !bParsedINIForRealm )
    {
        bParsedINIForRealm = TRUE;

        if( !OnlineGetValueFromINI( ONLINE_INI_REALM_KEYNAME, g_pszOnlineRealmName, MAX_PATH ) )
        {
            strcpy( g_pszOnlineRealmName, "Mar2002" );
        }
    }

    return g_pszOnlineRealmName;
}


// Get the current Cluster we are pointed to
char* OnlineGetCurrentCluster()
{
    static bParsedINIForCluster = FALSE;

    if( !bParsedINIForCluster )
    {
        bParsedINIForCluster = TRUE;
        if( !OnlineGetValueFromINI( ONLINE_INI_CLUSTER_KEYNAME, g_pszOnlineClusterName, MAX_PATH ) )
        {
            strcpy( g_pszOnlineClusterName, "PartnerNet" );
        }
    }

    return g_pszOnlineClusterName;
}


// Retreive a value from the XOnline INI
BOOL OnlineGetValueFromINI( char* pszKeyName, char* pszDestBuffer, unsigned int uiBufSize )
{
    BOOL bReturn = FALSE;
    FILE* pfInFile = fopen( ONLINE_INI_FILE_PATH, "rt" );
    if( NULL == pfInFile )
    {
        XDBGWRN( APP_TITLE_NAME_A, "OnlineGetValueFromINI():Unable to open the file!! - '%hs'", ONLINE_INI_FILE_PATH );
        return bReturn;
    }
    else
    {
        char pszFileLine[MAX_PATH+1];
        pszFileLine[MAX_PATH] = '\0';

        while( fgets( pszFileLine, MAX_PATH, pfInFile ) )
        {
            pszFileLine[strlen(pszFileLine)-1] = '\0';  // Remove the /n

            if( strstr( pszFileLine, pszKeyName )  )
            {
                char* pszEQLoc = strstr( pszFileLine, "=" );
                if( NULL == pszEQLoc )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "OnlineGetValueFromINI():Did not find '=' in file line!! - '%hs'", pszFileLine );
                    bReturn = FALSE;
                    break;
                }
                else
                {
                    pszEQLoc++; // Move the pointer over the equal sign
                    strcpy( pszDestBuffer, pszEQLoc );
                    bReturn = TRUE;
                    break;
                }
            }
        }

        fclose( pfInFile );
        pfInFile = NULL;
    }

    return bReturn;
}