///////////////////////////////////////////////////////////////////////////////
//
// Name: OnlineAccounts.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the functions to muck with account status
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#include <xbox.h>

#include <winsockx.h>
#include <winsockp.h>
#endif

#include <xdbg.h>
#include <xonlinep.h>
#include <stdio.h>
#include <stdlib.h>

//
// how long should wait's wait
//

static const DWORD ONLINE_ACCOUNT_WAIT_INTERVAL = 30000;

//
// set up for cross compilation with windows and xbox
//

#ifndef _XBOX
class COnlineAccounts : public CXOnline
{
#else
class COnlineAccounts
{
#endif

protected:
	COnlineAccounts( LPSTR szName = NULL );

private:
	static COnlineAccounts* m_pAccounts;

public:

	//
	// destrcutor
	//

	~COnlineAccounts( void ) { delete m_pAccounts; }

	//
	// singleton pattern
	//

	static COnlineAccounts* Instance( LPSTR szName = NULL );

	//
	// Get N random user accounts, put them on the hard drive
	//

	HRESULT 
	PopulateUserAccountsHD( PXONLINEP_USER aUsers, DWORD dwNumAccounts, BOOL bRemoveUsers = TRUE );

	//
	// Populate an MU with an account
	//

	HRESULT
	PopulateUserAccountsMU( CHAR chDrive, PXONLINEP_USER pUser );

	//
	// get the users and put them into an array
	//

	HRESULT
	GetUsers( PXONLINEP_USER aUsers, DWORD dwNumAccounts );

	//
	// quickly logon to the service
	//

	HRESULT
	QuickLogonStart( void );

	//
	// quickly logoff the service
	//

	HRESULT
	QuickLogonFinish( void );

	//
	// generate a user name
	//

	HRESULT
	GenerateUserName( CHAR szName[XONLINE_NAME_SIZE] );

	//
	// generate a user account
	//

	HRESULT
	GenerateUserAccount( PXONLINEP_USER pUser );
};


//
// modify a machine account
//

//HRESULT 
//ModifyMachineAccount( ULONGLONG ullId, LPSTR szName, LPSTR szPassword );

//
// verify the data in a machine account is what the user thinks...
//

//BOOL
//VerifyMachineAccount( ULONGLONG ullId, LPSTR szName, LPSTR szPassword );

//
// create an account
//

//HRESULT CreateMachineAccount( void );

//
// remove a machine account
//

//HRESULT RemoveMachineAccount( void );

//
// validate a machine account
//

//HRESULT ValidateMachineAccount( void );

//
// update the eeprom with devkit data
//

//HRESULT WriteEEPROMDevkitData( void );

//
// See if old Machine Account exists
//

//BOOL HasOldMachineAccount( void );

//
// see if old user accounts exist
//

//BOOL HasOldUserAccounts( void );

//
// read a line of user data from an ini file
//

//HRESULT 
//ReadUserData( FILE* pIniFile, PXONLINEP_USER pUser );



