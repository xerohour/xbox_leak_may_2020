///////////////////////////////////////////////////////////////////////////////
//
// Name: OnlineAccounts.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the functions to muck with account status
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _XBOX
#define NTLEANANDMEAN
#define _WIN32_WINNT 0x0500
 
#include <nt.h>
#include <ntrtl.h>
#endif

#include "OnlineAccounts2.h"

#ifdef _XBOX
extern "C" { 
#include <init.h> 
}
#include <xboxp.h>
#endif

//#include <cryptkeys.h>
#ifdef _XBOX
#include <xconfig.h>
#endif
//#include <memory.h>
#include <time.h>
/*
BOOL
KerbPasswordToKey(
    IN LPSTR pszPassword,
    OUT BYTE* Key
    );
*/

///////////////////////////////////////////////////////////////////////////////
//
// Name: Constructor
//
// Input: szName - the name of the online object
//
// Output: an instance of the class
//
// Description: creates the OnlineAccounts class
//
///////////////////////////////////////////////////////////////////////////////

COnlineAccounts::COnlineAccounts( LPSTR szName )
#ifndef _XBOX
	: CXOnline( szName )
#endif

{
    m_hTask = NULL;
}

/*
///////////////////////////////////////////////////////////////////////////////
//
// Name: ModifyMachineAccount
//
// Input: ullId - the new id
//
//        szName - the new name of the machine account
//		           If it is NULL, the machine name is cleared out
//
//		  szPassword - the new password of the machine account
//                     If it is NULL, the machine password is cleared out
//
// Output: S_OK if removing machine data from the Config Sector was successful 
//
// Description: Modifies the machine name and machine password
//
///////////////////////////////////////////////////////////////////////////////

HRESULT 
ModifyMachineAccount( ULONGLONG ullId, LPSTR szName, LPSTR szPassword )
{
#if 1
    RIP("should not call this anymore");
    return E_FAIL;
#else
	HRESULT hr = S_OK;
    XC_ONLINE_MACHINE_ACCOUNT_STRUCT machineAccount;

	//
	// Zero out the machine account
	//

	ZeroMemory( &machineAccount, sizeof( XC_ONLINE_MACHINE_ACCOUNT_STRUCT ) );

	//
	// setup the id for the machine account
	//

	machineAccount.xuid.qwUserID = ullId;

	//
	// Only fill in the name if it was passed in
	//
	
	if ( szName )
	{
		strcpy( machineAccount.name, szName );

		//
		// make the machine account noevmeber 11/01 compatible
		//

		machineAccount.reserved[XONLINE_ACCOUNT_RESERVED_SIZE - 4] = '1';
		machineAccount.reserved[XONLINE_ACCOUNT_RESERVED_SIZE - 3] = '1';
		machineAccount.reserved[XONLINE_ACCOUNT_RESERVED_SIZE - 2] = '0';
		machineAccount.reserved[XONLINE_ACCOUNT_RESERVED_SIZE - 1] = '1';
	}

	//
	// Only fill in the password if it was passed in, then encrypt it
	//

	if ( szPassword )
	{
		hr = KerbPasswordToKey( szPassword, machineAccount.key ) ? S_OK : E_FAIL;
		ASSERT( SUCCEEDED( hr ) || !"Could not convert password to key" );
		EncryptKeyWithHardDriveKey( machineAccount.key, 
			                        sizeof( machineAccount.key ) );
	}

	//
	// Update the config sector
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = XSetConfigValue( &g_OnlineMainConfigSectorInfo,
							  XC_ONLINE_MACHINE_ACCOUNT, 
							  REG_BINARY,
							  &machineAccount, 
							  sizeof( XC_ONLINE_MACHINE_ACCOUNT_STRUCT ) );

		hr = SUCCEEDED( hr ) ? S_OK : E_FAIL;
		ASSERT( SUCCEEDED( hr ) || !"Could not update config sector" );
	}

	//
	// spit back the final hresult
	//

	return hr;
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: RemoveMachineAccount
//
// Input: None
//
// Output: S_OK if removing machine data from the Config Sector was successful 
//
// Description: Removes the machine name and machine password
//
///////////////////////////////////////////////////////////////////////////////

HRESULT 
RemoveMachineAccount( void )
{
	return ModifyMachineAccount( 0, NULL, NULL );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: CreateMachineAccount
//
// Input: None
//
// Output: S_OK if populating the Config Sector was successful 
//
// Description: creates the machine name and machine password
//
///////////////////////////////////////////////////////////////////////////////

HRESULT 
CreateMachineAccount( void )
{
	return ModifyMachineAccount( 1, 
		                         "SN999999999999@machines.xbox.com", 
								 "PASSWORD" );	
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: WriteEEPROMDevkitData
//
// Input: None
//
// Output: S_OK if populating the EEPROM was successful 
//
// Description: propogates the DEVKIT online key and serial number
//
///////////////////////////////////////////////////////////////////////////////

HRESULT WriteEEPROMDevkitData( void )
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    XBOX_FACTORY_SETTINGS* factorySettings;
    BYTE OnlineKey[16];
    NTSTATUS status;
    DWORD type, size;
    BOOL bResult;
    HRESULT hr;

    eeprom = (EEPROM_LAYOUT*) buf;
    factorySettings = (XBOX_FACTORY_SETTINGS*) &eeprom->FactorySection;

    //
    // read EEPROM Data
    //
    status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);
    if (!NT_SUCCESS(status))
    {
        return E_FAIL;
    }

    ASSERT( type == REG_BINARY );
    ASSERT( size == EEPROM_TOTAL_MEMORY_SIZE );

    //
    // Put in the all 9 serial number for dev kits
    //
    if (!RtlEqualMemory( factorySettings->SerialNumber, "999999999999", sizeof(factorySettings->SerialNumber) ))
    {
        //
        // Save Serial Number into the EEPROM
        //
        status = ExSaveNonVolatileSetting(XC_FACTORY_SERIAL_NUMBER, REG_BINARY, "999999999999", sizeof(factorySettings->SerialNumber));
        if (!NT_SUCCESS(status))
        {
            return E_FAIL;
        }
    }


    //
    // Figure out the devkit online key and morph it
    //
    ASSERT( sizeof(OnlineKey) == sizeof(factorySettings->OnlineKey) );
    ASSERT( sizeof(OnlineKey) == DEVKIT_ONLINE_RAND_KEY_LEN );
    RtlCopyMemory( OnlineKey, DEVKIT_ONLINE_RAND_KEY, DEVKIT_ONLINE_RAND_KEY_LEN );
    MorphKeyByHardDriveKey( OnlineKey, sizeof(OnlineKey) );

    if (!RtlEqualMemory( factorySettings->OnlineKey, OnlineKey, sizeof(factorySettings->OnlineKey) ))
    {
        //
        // Save Online Key into the EEPROM
        //
        status = ExSaveNonVolatileSetting(XC_FACTORY_ONLINE_KEY, REG_BINARY, OnlineKey, sizeof(OnlineKey));
        if (!NT_SUCCESS(status))
        {
            return E_FAIL;
        }
    }
    
    //
    // Zero out the memory
    //
    RtlZeroMemory( factorySettings, sizeof(*factorySettings) );
    RtlZeroMemory( OnlineKey, sizeof(OnlineKey) );

    return S_OK;
}


HRESULT 
OldWriteEEPROMDevkitData( void )
{
    HRESULT hr = S_OK;
    NTSTATUS status = ERROR_SUCCESS;
    XBOX_FACTORY_SETTINGS factorySettings;

	if ( SUCCEEDED( hr ) )
	{

		//
		// Put in the all 9 serial number for dev kits
		//

		CopyMemory( factorySettings.SerialNumber, 
			        "999999999999", 
					sizeof( factorySettings.SerialNumber ) );

		//
		// Save Serial Number into the EEPROM
		//
		status = XSetValue( XC_FACTORY_SERIAL_NUMBER, 
							REG_BINARY, 
							factorySettings.SerialNumber, 
							sizeof( factorySettings.SerialNumber ) );

		hr = ERROR_SUCCESS == status ? S_OK : E_FAIL;
		ASSERT( SUCCEEDED( hr ) || !"Could not save serial number to EEPROM" );

	}

	if ( SUCCEEDED( hr ) )
	{

		//
		// copy in the DEVKIT Online Key
		//

		CopyMemory( factorySettings.OnlineKey, 
			        "\x7b\x59\x2e\x4f\x81\x78\xb4\xc7\x57\x88\x53\x1b\x2e\x74\x76\x87", 
					sizeof( factorySettings.OnlineKey ) );
		
		//
		// Morph with the hard-drive key
		//

		MorphKeyByHardDriveKey( factorySettings.OnlineKey, sizeof(factorySettings.OnlineKey) );

		//
		// Save Online Key into the EEPROM
		//

		status = XSetValue( XC_FACTORY_ONLINE_KEY, 
							REG_BINARY, 
							factorySettings.OnlineKey, 
							sizeof( factorySettings.OnlineKey ) );

		hr = ERROR_SUCCESS == status ? S_OK : E_FAIL;
		ASSERT( SUCCEEDED( hr ) || !"could not save online key to EEPROM" );
	}

    //
    // Zero out the memory
    //

    ZeroMemory( &factorySettings, sizeof( factorySettings ) );

	//
	// Convert the status to an HR
	//

    return ERROR_SUCCESS == status ? S_OK : E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ValidateMachineAccount
//
// Input: None
//
// Output: S_OK if either the machine account exists OR creation of a machine
//         account was successful
//
// Description: checks for a valid machine account on an xbox, and if need be
//              populates the xbox with the xdk serial number and 
//              XDK online key, and then creates the machine account
//
///////////////////////////////////////////////////////////////////////////////


HRESULT 
ValidateMachineAccount( void )
{
#if 1
    RIP("should not call this anymore");
    return E_FAIL;
#else
	//
	// Setup error codes
	//

	HRESULT	hr = S_OK;
	DWORD dwRet = ERROR_SUCCESS;

	//
	// Check to see if the xbox has a machine account
	//

	if ( ! XOnlineHasMachineAccount() )
	{
		//
		// If it does not, populate the EEPROM
		// (Faking the manufacturing process)
		//

		hr = WriteEEPROMDevkitData();
		hr = SUCCEEDED( hr ) ? S_OK : E_FAIL;
		ASSERT( SUCCEEDED( hr ) || !"could not populate EEPROM" );

		//
		// Create the machine account
		//

		if ( SUCCEEDED( hr ) )
		{
		//	hr = CreateMachineAccount();

            // hr = XOnlineCreateMachineAccount();
            hr = E_FAIL;    //@@@ drm: XOnlineCreateMachineAccount doesn't exist anymore
			hr = SUCCEEDED( hr ) ? S_OK : E_FAIL;
			ASSERT( SUCCEEDED( hr ) || !"Could not create machine account" );
		}

	}

	//
	// return the final hr
	//

	return hr;
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: HasOldMachineAccount
//
// Input: None
//
// Output: TRUE if an old account exists
//         FALSE
//
// Description: checks to see if an old machine account exists
//
///////////////////////////////////////////////////////////////////////////////

BOOL HasOldMachineAccount( void )
{
#if 1
    RIP("should not call this anymore");
    return FALSE;
#else
	DWORD dwRet = 0;
	DWORD cbResultLength = 0;
	DWORD dwType = 0;
	INT iVal = 0;

	XC_ONLINE_MACHINE_ACCOUNT_STRUCT machineAccount;

	//
	// Zero out the machine account
	//

	ZeroMemory( &machineAccount, sizeof( XC_ONLINE_MACHINE_ACCOUNT_STRUCT ) );

	//
	// check to see if a machine account exists
	//

	if ( ! XOnlineHasMachineAccount() )
	{
		return FALSE;
	}

	//
	// pull out the machine account information
	//


	dwRet = XQueryConfigValue( &g_OnlineMainConfigSectorInfo,
                               XC_ONLINE_MACHINE_ACCOUNT, 
							   &dwType,
							   &machineAccount, 
							   sizeof( XC_ONLINE_MACHINE_ACCOUNT_STRUCT ), 
							   &cbResultLength );

	//
	// if htis call failed, something must be wrong
	// we return true to signify that this section should be wiped
	//

	ASSERT( ERROR_SUCCESS == dwRet || !"Could not read config sector" );

	if ( ERROR_SUCCESS != dwRet )
	{
		return TRUE;
	}

	//
	// check the last four bytes for 1101 november time stamp
	//

	iVal = memcmp( machineAccount.reserved + XONLINE_ACCOUNT_RESERVED_SIZE - 4,
			       "1101",
				   4 );
	
	//
	// if iVal is 0, that means the account is November. We want hte negation
	// of that
	//

	return 0 != iVal;
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: HasOldUserAccounts
//
// Input: None
//
// Output: TRUE if an old user account exists
//         FALSE otherwise
//
// Description: decides if old user accounts exist. If even one of the
//              users is old, all are wiped out
//
///////////////////////////////////////////////////////////////////////////////

BOOL HasOldUserAccounts( void )
{
	HRESULT hr = S_OK;

	INT iVal = 0;
	DWORD cUsers = 0;
	DWORD cFinalUsers = 0;
	XONLINE_USER aUsers[XONLINE_MAX_STORED_ONLINE_USERS];

	//
	// zero out the user array
	//

	ZeroMemory( aUsers, sizeof( aUsers ) );

	//
	// query the users
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = XOnlineGetUsers( aUsers, &cUsers );
		ASSERT( SUCCEEDED( hr ) || !"Could not get users" );
	}

	//
	// copy the number of final users
	//

	cFinalUsers = cUsers;

	//
	// cycle through the users, removing the ones on MU
	//

	for ( DWORD i = 0; i < cUsers && SUCCEEDED( hr ); i++ )
	{
		if ( XONLINE_USER_OPTION_CAME_FROM_MU & aUsers[i].dwUserOptions )
		{
			cFinalUsers--;
			ZeroMemory( aUsers + i, sizeof( XONLINE_USER ) );
		}
	}

	//
	// cycle through the users
	//

	for ( DWORD i = 0; i < cFinalUsers && SUCCEEDED( hr ); i++ )
	{
		//
		// look for the november 1101 tag
		//

		iVal = memcmp( aUsers[i].reserved + XONLINE_USER_RESERVED_SIZE - 4,
			           "1101",
					   4 );

		//
		// return if the user tag is not 1101
		//

		if ( 0 != iVal )
		{
			return TRUE;
		}
	}

	//
	// if we got here, there were no old users
	//

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: VerifyMachineAcount
//
// Input: ullId - the id
//        szName - the name of the machine
//        szPassword - the password for the machine
//
// Output: TRUE if the machine accounts match
//         FALSE otherwise
//
// Description: decides if the machine account on the box is the same
//              as the one passed in
//
///////////////////////////////////////////////////////////////////////////////

BOOL
VerifyMachineAccount( ULONGLONG ullId, LPSTR szName, LPSTR szPassword )
{
#if 1
    RIP("should not call this anymore");
    return FALSE;
#else
	DWORD dwRet = 0;
	DWORD cbResultLength = 0;
	DWORD dwType = 0;
	INT iVal = 0;
	BYTE byKey[XONLINE_KEY_LENGTH];

	XC_ONLINE_MACHINE_ACCOUNT_STRUCT machineAccount;

	//
	// Zero out the machine account and the key
	//

	ZeroMemory( &machineAccount, sizeof( XC_ONLINE_MACHINE_ACCOUNT_STRUCT ) );
	ZeroMemory( byKey, sizeof( byKey ) );

	//
	// get the config sector info
	//

	dwRet = XQueryConfigValue( &g_OnlineMainConfigSectorInfo,
                               XC_ONLINE_MACHINE_ACCOUNT, 
							   &dwType,
							   &machineAccount, 
							   sizeof( XC_ONLINE_MACHINE_ACCOUNT_STRUCT ), 
							   &cbResultLength );

	//
	// if there was a failure reading the config sector, fail
	//

	if ( ERROR_SUCCESS != dwRet )
	{
		return FALSE;
	}

	//
	// decrypt the key
	//

	DecryptKeyWithHardDriveKey( machineAccount.key, sizeof( machineAccount.key ) );
	KerbPasswordToKey( szPassword, byKey );

	//
	// decide on the result
	//
	
	return ullId == machineAccount.id && 
		   0 == strcmp( machineAccount.name, szName ) &&
		   0 == memcmp( machineAccount.key, byKey, XONLINE_KEY_LENGTH );
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ReadUserData
//
// Input: pIniFile - an open file pointer to the ini file to read from
//        pNewUser - where the user data should be placed
//
// Output: S_OK on success, E_OUTOFMEMORY when no more data to read E_XXXX otherwise
//
// Description: reads 1 line of user data, and populate the user
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
ReadUserData( FILE* pIniFile, PXONLINEP_USER pNewUser )
{
#if 1
    RIP("should not call this anymore");
	return E_FAIL;
#else

	HRESULT hr = S_OK;
	INT iVal = 0;

	CHAR szString[256];
	BYTE byPin[XONLINE_PIN_LENGTH];

	union un {
		ULONGLONG ullVals[2];
		BYTE byVals[16];
	} unn;

	//
	// make sure the ini file is not NULL
	//

	if ( NULL == pIniFile )
	{
		hr = E_POINTER;
		ASSERT( SUCCEEDED( hr ) || !"Ini file pointer was null" );
	}

	//
	// make sure the user is not NULL
	//

	if ( NULL == pNewUser )
	{
		hr = E_POINTER;
		ASSERT( SUCCEEDED( hr ) || !"User pointer was NULL" );
	}

	if ( SUCCEEDED( hr ) )
	{

		ZeroMemory( szString, sizeof( szString ) );

		//
		// the format string is:
		//
		// 16 hex charecters put into an 8 byte value
		// , delimieter
		// 63 maximum valid email string charecters
		// , delimeter
		// 32 hex charecters (16 + 16) put into 2-8 byte values
		// , delimiter
		// 255 maximum valid string charecters (optional)

		iVal = fscanf( pIniFile, 
					   "%16I64x,%16[a-zA-Z0-9!#$%&'*+./=?^_`{|}~@-],%12[a-zA-Z0-9!#$%&'*+./=?^_`{|}~@-],%20[a-zA-Z0-9!#$%&'*+./=?^_`{|}~@-],%16I64x%16I64x,%255[a-zA-Z0-9,]", 
					   &pNewUser->xuid.qwUserID,
					   pNewUser->name,
					   pNewUser->kingdom,
					   pNewUser->domain,
					   unn.ullVals + 1,
					   unn.ullVals,
					   szString );

		hr = 0 != iVal ? S_OK : E_FAIL;

		ASSERT( SUCCEEDED( hr ) || !"Bad ini data" );

		if ( EOF == iVal )
		{
			hr = E_OUTOFMEMORY;
		}

	}

	
	//
	// copy the realm value
	// AND
	// copy the values from the union into the key
	//

	if ( SUCCEEDED( hr ) )
	{
		strcpy( pNewUser->realm, "passport.net" );

		for ( DWORD i = 0; i < sizeof( ULONGLONG ) * 2 && SUCCEEDED( hr ); i++ )
		{
			pNewUser->key[i] = unn.byVals[sizeof( ULONGLONG ) * 2 - i - 1];
		}
	}
	
	//
	// Parse the optional params. These are (all optional)
	// 8 bytes flags
	// , delimieter
	// 8 bytes pin
	// , delimiter
	// 237 chars
	//

	if ( SUCCEEDED( hr ) )
	{
		iVal = sscanf( szString, "%8x,%8x", &pNewUser->dwUserOptions, byPin );

		hr = 0 != iVal ? S_OK : E_FAIL;
		ASSERT( SUCCEEDED( hr ) || !"Bad ini data" );
	}

	//
	// rearange the pin value
	//
	
	if ( SUCCEEDED( hr ) )
	{
		for ( DWORD i = 0; i < XONLINE_PIN_LENGTH && SUCCEEDED( hr ); i++ )
		{
			pNewUser->pin[i] = byPin[XONLINE_PIN_LENGTH - i - 1];
		}
	}
	
	//
	// spit back the hr
	//

	return hr;
#endif

}
*/

///////////////////////////////////////////////////////////////////////////////
//
// Name: QuickLogonStart
//
// Input: none
//
// Output: S_OK on success,  E_XXXX otherwise
//
// Description: does a quick logon to the user account creation server
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
COnlineAccounts::QuickLogonStart( void )
{
	//
	// handles to communicate with Xbox Online
	//

	HRESULT hr = S_OK;
	HRESULT hrTask = S_OK;
	HANDLE hEvent = NULL;

	DWORD dwWait = 0;

	//
	// create the event handle, make sure it is valid
	//

	hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	if ( NULL == hEvent )
	{
		hr = E_OUTOFMEMORY;
	}

	//
	// since no users will be logged in (this is just a ping), we
	// want to only hit the user account server
	//

    DWORD aServices[] = { XONLINE_USER_ACCOUNT_SERVICE };

	//
	// create the user array, zero it out
	//

	XONLINE_USER aUsers[XONLINE_MAX_LOGON_USERS];
	ZeroMemory( aUsers, sizeof( aUsers ) );

	//
	// generate the async logon to the user account service
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = XOnlineLogon( aUsers, 
						   aServices,
			               sizeof( aServices ) / sizeof( DWORD ),
						   hEvent, 
						   &m_hTask );

	}

	//
	// pump the task handle until logon completes
	//

	if ( SUCCEEDED( hr ) )
	{

		do
		{   
			//
			// wait for data to be ready for processing
			//

			dwWait = WaitForSingleObject( hEvent, ONLINE_ACCOUNT_WAIT_INTERVAL );

			//
			// see if the wait failed due to memory
			//

			if ( WAIT_FAILED == dwWait )
			{
				hr = E_OUTOFMEMORY;
			}

			//
			// see if the wait timed out
			//
        
			if ( WAIT_TIMEOUT == dwWait )
			{
				hr = E_FAIL;
			}

			//
			// continue pumping the handle
			//

			if ( SUCCEEDED( hr ) )
			{
				hrTask = XOnlineTaskContinue( m_hTask );
			}
                        Sleep(1);
		} while ( hrTask == XONLINETASK_S_RUNNING && SUCCEEDED( hr ) );
	}

    //
    // close the event
    //

    if ( NULL != hEvent )
    {
        CloseHandle( hEvent );
        hEvent = NULL;
    }

	return SUCCEEDED( hrTask ) ? hr : hrTask;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: QuickLogonFinish
//
// Input: none
//
// Output: S_OK on success,  E_XXXX otherwise
//
// Description: does a quick logoff to the user account creation server
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
COnlineAccounts::QuickLogonFinish( void )
{
	HRESULT hr = S_OK;

	//
	// close the handle, wether we have falied or not
	//

	if ( NULL != m_hTask )
	{
		XOnlineTaskClose( m_hTask );
	}

	//
	// reset the task handle
	//

	m_hTask = NULL;

	//
	// spit back the hr
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: GenerateUserName
//
// Input: szUserName - an array of XONLINE_NAME_SIZE bytes ot get 
//        the new user name                      
//        
// Output: S_OK on success, E_XXXX otherwise
//
// Description: create a random user name based on the Ethernet MAC address
//              and 3 bytes of charecter values
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
COnlineAccounts::GenerateUserName( CHAR szName[XONLINE_NAME_SIZE] )
{
	HRESULT hr = S_OK;

	DWORD dwStatus = 0;
	DWORD dwType = 0;
	DWORD cbResultLength = 0;

	CHAR abMacAddress[6];

	//
	// zero out the char array
	//

	ZeroMemory( abMacAddress, 6 );

	//
	// get the Ethernet MAC address
	//

	if ( SUCCEEDED( hr ) )
	{
		//
		// get the address
		//
#ifdef _XBOX
		dwStatus = ExQueryNonVolatileSetting( XC_FACTORY_ETHERNET_ADDR, &dwType, abMacAddress, 6, &cbResultLength );
		hr = NT_SUCCESS( dwStatus ) ? S_OK : E_FAIL;
		
		ASSERT( SUCCEEDED( hr ) || !"Could not get ethernet MAC address" );

#else

		sprintf( abMacAddress, "%6.6s", getenv("COMPUTERNAME") );

#endif
	}

	//
	// now populate the user name
	//

	if ( SUCCEEDED( hr ) )
	{
		//
		// populate user name with the last 4 bytes of the ethernet mac address
		//

		for ( DWORD i = 0; i < 4; i++ )
		{
			//
			// print the current byte
			//

			sprintf( szName + i * 2, "%02x", (BYTE) abMacAddress[i + 2] );
		}

		//
		// stuff the last 7 bytes with a random char
		//

		sprintf( szName + 8, 
				 "%c%c%c%c%c%c%c", 
				 (BYTE) rand() % ( 'Z' - 'A' ) + 'A',
				 (BYTE) rand() % ( 'Z' - 'A' ) + 'A',
				 (BYTE) rand() % ( 'Z' - 'A' ) + 'A',
				 (BYTE) rand() % ( 'Z' - 'A' ) + 'A',
				 (BYTE) rand() % ( 'Z' - 'A' ) + 'A', 
				 (BYTE) rand() % ( 'Z' - 'A' ) + 'A', 
				 (BYTE) rand() % ( 'Z' - 'A' ) + 'A' );
	}

	//
	// spit back the hr
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: GenerateUserAccount
//
// Input: pUser - the user to fill in                     
//        
// Output: S_OK on success, E_XXXX otherwise
//
// Description: fill in and create a user
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
COnlineAccounts::GenerateUserAccount( PXONLINEP_USER pUser )
{
	HRESULT hr = S_OK;

	HRESULT hrTask = S_OK;
	HANDLE hEvent = NULL;

	XONLINETASK_HANDLE hTask = NULL;

	DWORD dwWait = 0;

	HRESULT hrGenName = S_OK;

	//
	// create the event handle, make sure it is valid
	//

	hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	if ( NULL == hEvent )
	{
		hr = E_OUTOFMEMORY;
		ASSERT( SUCCEEDED( hr ) || !"Could not create event handle for account creation" );
	}

	if ( SUCCEEDED( hr ) )
	{
		do
		{
			//
			// see if name needs to be populated
			//

			if ( SUCCEEDED( hr ) && 0 == strcmp( pUser->name, "" ) )
			{
				//
				// go ahead and generate the user name
				//

				hr = GenerateUserName( pUser->name );
				ASSERT( SUCCEEDED( hr ) || !"Could not generate user name" );

				DbgPrint( pUser->name );
				DbgPrint( "\n" );
			}

			//
			// fill in the kingdom
			//

			if ( SUCCEEDED( hr ) && 0 == strcmp( pUser->kingdom, "" ) )
			{
				strcpy( pUser->kingdom, "Feb2002" );
			}

			//
			// create the account
			//

			if ( SUCCEEDED( hr ) )
			{
				hr = _XOnlineAccountTempCreate( (PXONLINE_USER) pUser, hEvent, &hTask );
		//		ASSERT( SUCCEEDED( hr ) || !"Creation of temp account failed" );
			}

			//
			// pump the task handle until user creation completes
			//

			if ( SUCCEEDED( hr ) )
			{
				do
				{   
					//
					// wait for data to be ready for processing
					//

					dwWait = WaitForSingleObject( hEvent, ONLINE_ACCOUNT_WAIT_INTERVAL );

					if ( WAIT_FAILED == dwWait )
					{
						hr = E_OUTOFMEMORY;
					}
    
					//
					// make sure the wait did not time out
					//

					else if ( WAIT_TIMEOUT == dwWait )
					{
						hr = E_FAIL;
					}

					//
					// continue pumping the handle
					//

					if ( SUCCEEDED( hr ) )
					{
						hrTask = XOnlineTaskContinue( hTask );
					}
                                        Sleep(1);
				} while ( XONLINETASK_S_RUNNING == hrTask && SUCCEEDED( hr ) );
			}

			//
			// if the name was taken, zero it out
			//

			if ( XONLINE_S_ACCOUNTS_NAME_TAKEN == hrTask && SUCCEEDED( hr ) )
			{
				hrGenName = S_FALSE;

				hrTask = S_OK;
				ZeroMemory( pUser->name, XONLINE_NAME_SIZE );

				//
				// close down the handle
				//

				if ( NULL != hTask )
				{
					hrTask = XOnlineTaskClose( hTask );
					ASSERT( SUCCEEDED( hrTask ) || !"Could not close handle" );
					hTask = NULL;
				}

				hr = SUCCEEDED( hr ) ? hrTask : hr;
			}

			else if ( FAILED( hrTask ) )
			{
				hr = hrTask;
			}

			//
			// keep going until the name is filled
			//

		} while ( 0 == strcmp( pUser->name, "" ) && SUCCEEDED( hr ) );
	}

	//
	// Now retrieve the created account
	//

    if ( SUCCEEDED( hr ) )
    {
		hr = _XOnlineAccountTempCreateGetResults( hTask, (PXONLINE_USER) pUser );
		ASSERT( SUCCEEDED( hr ) || !"Creation of temp account failed" );
    }

	//
	// close the handle, wether we have falied or not
	//

	if ( NULL != hTask )
	{
		hrTask = XOnlineTaskClose( hTask );
		ASSERT( SUCCEEDED( hrTask ) || !"Could not close handle" );
		hTask = NULL;
	}

	hr = SUCCEEDED( hr ) ? hrTask : hr;

    //
    // close the event
    //

    if ( NULL != hEvent )
    {
        CloseHandle( hEvent );
        hEvent = NULL;
    }

	return SUCCEEDED( hr ) ? hrGenName : hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: GetUsers
//
// Input: aUsers - an output array of users
//        dwNumAccounts - the number of accounts to fill                       
//        
// Output: S_OK on success, E_XXXX otherwise
//
// Description: creates N users using UAPS
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
COnlineAccounts::GetUsers( PXONLINEP_USER aUsers, DWORD dwNumAccounts )
{
	//
	// handles to communicate with xonline
	//

	HRESULT hr = S_OK;
	HRESULT hrTask = S_OK;
	HRESULT hrGenName = S_OK;

	DWORD dwNumCreated = 0;

	//
	// seed the random number generator
	// 

	srand( time( NULL ) );

	//
	// we need to specify the user array, this pointer can't be NULL
	//

	if ( NULL == aUsers )
	{
		hr = E_INVALIDARG;
		ASSERT( SUCCEEDED( hr ) || !"Must pass an array of users. aUsers can not be NULL" );
	}

	//
	// initiate a logon
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = QuickLogonStart();
//		ASSERT( SUCCEEDED( hr ) || !"Could not logon" );
	}

	//
	// create accounts for each item
	//

	for( dwNumCreated = 0; dwNumCreated < dwNumAccounts && SUCCEEDED( hr ); dwNumCreated++ )
	{
		//
		// generate the user
		//

		hr = GenerateUserAccount( aUsers + dwNumCreated );
		hrGenName = hr;
	//	ASSERT( SUCCEEDED( hr ) || !"GenerateUserAccounts failed" );

		if ( SUCCEEDED( hr ) )
		{
			hr = _XOnlineAddUserToHD( (PXONLINE_USER) aUsers + dwNumCreated );
		}
	}

	//
	// verify the number of accounts requested was the same as the number created
	//

	if ( SUCCEEDED( hr ) && dwNumCreated != dwNumAccounts )
	{
		hr = E_FAIL;
		ASSERT( SUCCEEDED( hr ) || !"Number of accounts created is not correct" );
	}

	//
	// verify the number of accounts created is in the right range
	//

	if ( SUCCEEDED( hr ) && dwNumCreated > XONLINE_MAX_STORED_ONLINE_USERS )
	{
		hr = E_FAIL;
		ASSERT( SUCCEEDED( hr ) || !"Can not create more than max users!" );
	}

	//
	// end the logon
	//

	hrTask = QuickLogonFinish();

	hr = SUCCEEDED( hr ) ? hrTask : hr;

	//
	// spit back the hr
	//

	return SUCCEEDED( hr ) ? hrGenName : hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: PopulateUserAccountsMU
//
// Input: chDrive - the drive letter of the MU to populate
//
//        pUser - an XONLINEP_USER pointer. if the pointer is NULL, then the
//                user is erased off the MU. 
//                if the name is NULL, a random name will generated
//        
// Output: S_OK on success, E_XXXX otherwise
//
// Description: creates 1 user on the MU using XUACS
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
COnlineAccounts::PopulateUserAccountsMU( CHAR chDrive, PXONLINEP_USER pUser )
{
	HRESULT hr = S_OK;
	HRESULT hrGenName = S_OK;

	XONLINEP_USER user;

	//
	// zero out the internal user
	//

	ZeroMemory( &user, sizeof( XONLINEP_USER ) );

	//
	// if no user was passed in, then we want to erase the user
	// on the MU
	//

	if ( NULL == pUser )
	{
		pUser = &user;
	}

	//
	// otherwise, see if we need to populate the name and kingdom
	//

	else
	{
		hr = GenerateUserAccount( pUser );
		hrGenName = hr;

		if ( SUCCEEDED( hr ) )
		{
			hr = _XOnlineSetUserInMU( chDrive, (PXONLINE_USER) pUser );
		}
	}

	//
	// spit back the hr
	//
	
	return SUCCEEDED( hr ) ? hrGenName : hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: PopulateUserAccountsHD
//
// Input: aUsers - the array of the xonline users to populate
//
//                 dwNumAccounts elements in this array will be filled. if
//                 the inputed name and kingdom are NULL, then a random 
//                 name and kingdom are generated and used. if the name and
//                 kingdom are both not null, then the specified name/kingdom
//                 are used to generate the user in UAPS. if this pointer is NULL
//                 then the function will internally allocate random users
//
//        dwNumAccounts -  the number of accounts to populate the
//                         drive with
//
//                         dwNumAccounts == 0, then the drive will have all 
//                         accounts removed and no new accounts will be added
//
//        bRemoveUsers - if set, then users are removed from the HD before creating
//                       new ones. Set by default
//
// Output: S_OK on success
//         E_XXXX on failutre
//         S_FALSE if less users were populated then expected.
//
// Description: Populates the hard disk with the specified number of users.
//              this function will clear off any users on the drive before 
//              populating the drive
//
///////////////////////////////////////////////////////////////////////////////

HRESULT 
COnlineAccounts::PopulateUserAccountsHD( PXONLINEP_USER aUsers, DWORD dwNumAccounts, BOOL bRemoveUsers )
{
	HRESULT hr = S_OK;
	HRESULT hrGenName = S_OK;
	DWORD dwNumUsers = 0;
	XONLINEP_USER aExistingUsers[XONLINE_MAX_STORED_ONLINE_USERS];

	//
	// clear out the user array
	//

	ZeroMemory( aExistingUsers, sizeof( aExistingUsers ) );

	//
	// make sure no more than 8 users are asked for
	//

	if ( dwNumAccounts > XONLINE_MAX_HD_ONLINE_USERS )
	{
		hr = E_INVALIDARG;
		ASSERT( SUCCEEDED( hr ) || !"can not ask for more than 8 users!" );
	}

	//
	// get the users on the drive currently
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = _XOnlineGetUsersFromHD( (PXONLINE_USER) aExistingUsers, &dwNumUsers );
		ASSERT( SUCCEEDED( hr ) || !"Could not get users on drive" );
	}

	//
	// if we have a FULL drive, then let's error out, only if removal was not asked for
	//

	if ( SUCCEEDED( hr ) && XONLINE_MAX_HD_ONLINE_USERS == dwNumUsers && FALSE == bRemoveUsers )
	{
		hr = E_OUTOFMEMORY;
	}

	//
	// remove the users on the drive
	//

	for ( DWORD i = 0; i < dwNumUsers && SUCCEEDED( hr ) && TRUE == bRemoveUsers; i++ )
	{ 
		hr = _XOnlineRemoveUserFromHD( (PXONLINE_USER) ( aExistingUsers + i ) );
		ASSERT( SUCCEEDED( hr ) || !"Could not remove user" );
	}

	//
	// allow for NULL user list
	//

	if ( NULL == aUsers )
	{
		aUsers = aExistingUsers;
		ZeroMemory( aUsers, sizeof( aExistingUsers ) );
	}

	//
	// create the users
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = GetUsers( aUsers, dwNumAccounts );
		hrGenName = hr;
	//	ASSERT( SUCCEEDED( hr ) || !"Could not get users" );
	}

	//
	// return the hr
	//

	return SUCCEEDED( hr ) ? hrGenName : hr;
}
