/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#ifndef __CONFIG_H_
#define __CONFIG_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif

//Default optional configuration values
const DWORD default_channels = 1;
const DWORD default_users = 10000;
const DWORD default_destinations = 10;
const DWORD default_runtime = 5;
const BOOL default_ticklecontrol = TRUE;
const DWORD default_maxttl = 1800;

/////////////////////////////////////////////////////////////////////////////
// Stores application configuration information
/////////////////////////////////////////////////////////////////////////////

class Configuration{

public:
	Configuration():
		dwChannels(default_channels),
		dwUsers(default_users),
		dwDestinations(default_destinations),
		dwPort(0L),
		dwQueues(0L),
		rgdwMaxSize(NULL),
		rgdwMinSize(NULL),
		rgfDuplicates(NULL),
		dwRuntime(default_runtime),
		fTickleControl(default_ticklecontrol),
		dwMaxLifetime(default_maxttl){

			//Clear server name
			ZeroMemory(pszServerName,sizeof(pszServerName));

			//Clear config file name
			ZeroMemory(pszConfigFile,sizeof(pszConfigFile));
		}//endmethod

	~Configuration(){

		//Deallocate max size array
		if (NULL!=rgdwMaxSize){
			delete [] rgdwMaxSize;
			rgdwMaxSize=NULL;
		}//endif

		//Deallocate min size array
		if (NULL!=rgdwMinSize){
			delete [] rgdwMinSize;
			rgdwMinSize=NULL;
		}//endif

		//Deallocate array of duplicate tags
		if (NULL!=rgfDuplicates){
			delete [] rgfDuplicates;
			rgfDuplicates=NULL;
		}//endif

	}//endmethod

public:
	char pszServerName[256];  //Server name
	char pszConfigFile[256];  //Server config file
	DWORD dwChannels;  //Number of channels
	DWORD dwUsers;  //Number of unique users
	DWORD dwDestinations;  //Max number of destinations a user can add to
	DWORD dwQueues;  //Number of server queues
	DWORD dwPort;  //Server port
	DWORD* rgdwMaxSize;  //Array of max size for a queue
	DWORD* rgdwMinSize;  //Array of min size for a queue
	BOOL* rgfDuplicates;  //Queue allows duplicates?
	DWORD dwRuntime;  //Minutes to run
	DWORD dwMaxLifetime;  //Max seconds a client can live
	BOOL fTickleControl;  //Do tickles control list/del? (vs. internal state)
};

#endif