#ifndef _XONLINEFUNCS_H_
#define _XONLINEFUNCS_H_

#include <time.h>

#define ONLINE_ACCOUNT_WAIT_INTERVAL        30000                               // milliseconds
#define ONLINE_USERNAME_MINIMUM_SIZE        5                                   // characters
#define ONLINE_USERNAME_INVALID_CHARACTERS  L":; <=>?@!\"#$%&\'()*+,-./[\\]"    // TODO: What should these be?
#define ONLINE_INI_FILE_PATH                "C:\\DEVKIT\\XONLINE.INI"
#define ONLINE_INI_SECTION_NAME             "TestSettings"
#define ONLINE_INI_REALM_KEYNAME            "REALM"
#define ONLINE_INI_CLUSTER_KEYNAME          "CLUSTER"

HRESULT OnlineQuickLoginStart( void );                              // Does a quick login to the user account creation server
HRESULT OnlineQuickLoginFinish( void );                             // Does a quick logoff to the user account creation server
HRESULT OnlineGenerateUserName( CHAR szName[XONLINE_NAME_SIZE] );   // Create a random user name based on the Ethernet MAC address and 7 bytes of charecter values
HRESULT OnlineGenerateUserName( WCHAR wszName[XONLINE_NAME_SIZE] ); // Create a random user name based on the Ethernet MAC address and 7 bytes of charecter values
HRESULT OnlineGenerateUserAccount( PXONLINE_USER pOnlineUser );     // Create a user

HRESULT OnlineRemoveUserFromHD( PXONLINE_USER pOnlineUser );        // Will remove the specified user from the hard drive
HRESULT OnlineClearUserFromMU( char cDriveLetter );                 // Will remove the user from the specified MU
HRESULT OnlineSetUserInMU( char cDriveLetter,                       // Will store the specified user on the specified MU
                           PXONLINE_USER pOnlineUser );
HRESULT OnlineGetUserFromMU( DWORD dwPort,                          // Gets the user information from an MU and returns it
                             DWORD dwSlot,
                             PXONLINE_USER pOnlineUser,
                             BOOL* pbUserExists = NULL );
HRESULT OnlineGetUsersFromHD( PXONLINE_USER paOnlineUsers,          // Will retrieve users from the hard drive
                              DWORD* pdwNumUsers );
HRESULT OnlineIsUserOnHD( char* pszUserName,                        // Will return true if the user already exists on the Hard Drive
                          BOOL* pbUserExists );
HRESULT OnlineAddUserToHD( PXONLINE_USER pOnlineUser );             // Will add a user to the hard drive
BOOL OnlineIsNameValid( WCHAR* pwszUserName );                      // Used to determine if a user name is valid before creating

char* OnlineGetCurrentRealm();                                      // Get the current Realm users should be in
char* OnlineGetCurrentCluster();                                    // Get the current Cluster we are pointed to
BOOL OnlineGetValueFromINI( char* pszKeyName,                       // Retreive a value from the XOnline INI
                            char* pszDestBuffer,
                            unsigned int uiBufSize );

#endif // _XONLINEFUNCS_H_