
///////////////////////////////////////////////////////////////////////////////
//
// Name: myError.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: error and assertion functions
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <stdio.h>

//
// nt error stuff
//

typedef LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;

//
// the error codes
//

enum ERROR_CODES {
	E_BASE = 0xF0000000,
	E_BAD_ARG,
	E_PARSE_INI_FILE,
	E_OPEN_INI_FILE,
    E_CLOSE_INI_FILE,
    E_INI_BAD_DATA,
	E_MU_UNMOUNT,
	E_MU_MOUNT,
	E_MEMORY_ALLOCATION,
	E_INVALID_USER_COUNT,
	E_AUDIO_PLAYBACK,
	E_PASSWORD_TO_KEY,
	E_UPDATE_CONFIG_SECTOR,
	E_UPDATE_EEPROM_SN,
	E_UPDATE_EEPROM_KEY,
	E_WRITE_EEPROM_DATA,
	E_CREATE_MACHINE_ACCOUNT,
	E_LOGON_FAIL
};

//
// the error strings
//

#define S_GENERAL_ERROR "General error" 
#define S_BAD_ARG "Bad Argument" 
#define S_PARSE_INI_FILE "Please copy users.ini to xe:\\Tools\\SetupOnline" 
#define S_OPEN_INI_FILE "Could not open users.ini file" 
#define S_CLOSE_INI_FILE "Could not close users.ini file" 
#define S_INI_BAD_DATA "Malformed data in users.ini file" 
#define S_MU_UNMOUNT "Could not unmount mu" 
#define S_MU_MOUNT "Could not mount mu" 
#define S_MEMORY_ALLOCATION "Could not allocate memory" 
#define S_INVALID_USER_COUNT "An invalid user count was returned" 
#define S_AUDIO_PLAYBACK "Audio file could not be played" 
#define S_PASSWORD_TO_KEY "Could not convert password to key" 
#define S_UPDATE_CONFIG_SECTOR "Could not update config sector" 
#define S_UPDATE_EEPROM_SN "Could not update EEPROM with SN" 
#define S_UPDATE_EEPROM_KEY "Could not update EEPROM with Key" 
#define S_WRITE_EEPROM_DATA "Could not write EEPROM data" 
#define S_CREATE_MACHINE_ACCOUNT "Could not create machine account" 
#define S_LOGON_FAIL "Logon to PartnerNet failed" 

#define S_REMOVE_MACHINE_ACCOUNT "Could not remove machine account" 
#define S_GET_USERS_FAILED "Could not get users" 
#define S_POPULATE_DRIVE_LIST "Could not populate the drive menu" 
#define S_INIT_DEVICES "Could not init devices" 
#define S_UPDATE_LISTS "Could not update menus" 
#define S_ADD_CACHED_USER "Could not add user" 
#define S_REMOVE_CACHED_USER "Could not remove user" 

//#define MY_ASSERT( hr, str )
#define MY_ASSERT( hr, str ) ASSERT( SUCCEEDED( hr ) || ! str )
	
