
//=============================================================================
//  userenv.h   -   Header file for user environment API.
//                  User Profiles, environment variables, and Group Policy
//
//  Copyright (c) Microsoft Corporation 1995-1999
//  All rights reserved
//
//=============================================================================


#ifndef _INC_USERENV
#define _INC_USERENV

//
// Define API decoration for direct importing of DLL references.
//

#if !defined(_USERENV_)
#define USERENVAPI DECLSPEC_IMPORT
#else
#define USERENVAPI
#endif


#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//
// LoadUserProfile
//
// Loads the specified user's profile.
//
// Most applications should not need to use this function.  It's used
// when a user has logged onto the system or a service starts in a named
// user account.
//
// hToken        - Token for the user, returned from LogonUser()
// lpProfileInfo - Address of a PROFILEINFO structure
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:  The caller of this function must have admin privileges on the machine.
//
//        Upon successful return, the hProfile member of the PROFILEINFO
//        structure is a registry key handle opened to the root
//        of the user's hive.  It has been opened with full access. If
//        you need to read or write to the user's registry file, use
//        this key instead of HKEY_CURRENT_USER.  Do not close this
//        handle.  Instead pass it to UnloadUserProfile to close
//        the handle.
//
//=============================================================================

//
// Flags that can be set in the dwFlags field
//

#define PI_NOUI         0x00000001      // Prevents displaying of messages
#define PI_APPLYPOLICY  0x00000002      // Apply NT4 style policy

typedef struct _PROFILEINFOA {
    DWORD       dwSize;                 // Set to sizeof(PROFILEINFO) before calling
    DWORD       dwFlags;                // See flags above
    LPSTR       lpUserName;             // User name (required)
    LPSTR       lpProfilePath;          // Roaming profile path (optional, can be NULL)
    LPSTR       lpDefaultPath;          // Default user profile path (optional, can be NULL)
    LPSTR       lpServerName;           // Validating domain controller name in netbios format (optional, can be NULL but group NT4 style policy won't be applied)
    LPSTR       lpPolicyPath;           // Path to the NT4 style policy file (optional, can be NULL)
    HANDLE      hProfile;               // Filled in by the function.  Registry key handle open to the root.
} PROFILEINFOA, FAR * LPPROFILEINFOA;
typedef struct _PROFILEINFOW {
    DWORD       dwSize;                 // Set to sizeof(PROFILEINFO) before calling
    DWORD       dwFlags;                // See flags above
    LPWSTR      lpUserName;             // User name (required)
    LPWSTR      lpProfilePath;          // Roaming profile path (optional, can be NULL)
    LPWSTR      lpDefaultPath;          // Default user profile path (optional, can be NULL)
    LPWSTR      lpServerName;           // Validating domain controller name in netbios format (optional, can be NULL but group NT4 style policy won't be applied)
    LPWSTR      lpPolicyPath;           // Path to the NT4 style policy file (optional, can be NULL)
    HANDLE      hProfile;               // Filled in by the function.  Registry key handle open to the root.
} PROFILEINFOW, FAR * LPPROFILEINFOW;
#ifdef UNICODE
typedef PROFILEINFOW PROFILEINFO;
typedef LPPROFILEINFOW LPPROFILEINFO;
#else
typedef PROFILEINFOA PROFILEINFO;
typedef LPPROFILEINFOA LPPROFILEINFO;
#endif // UNICODE



USERENVAPI
BOOL
WINAPI
LoadUserProfileA(
    IN HANDLE hToken,
    IN OUT LPPROFILEINFOA lpProfileInfo);
USERENVAPI
BOOL
WINAPI
LoadUserProfileW(
    IN HANDLE hToken,
    IN OUT LPPROFILEINFOW lpProfileInfo);
#ifdef UNICODE
#define LoadUserProfile  LoadUserProfileW
#else
#define LoadUserProfile  LoadUserProfileA
#endif // !UNICODE


//=============================================================================
//
// UnloadUserProfile
//
// Unloads a user's profile that was loaded by LoadUserProfile()
//
// hToken        -  Token for the user, returned from LogonUser()
// hProfile      -  hProfile member of the PROFILEINFO structure
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     The caller of this function must have admin privileges on the machine.
//
//=============================================================================

USERENVAPI
BOOL
WINAPI
UnloadUserProfile(
    IN HANDLE hToken,
    IN HANDLE hProfile);


//=============================================================================
//
// GetProfilesDirectory
//
// Returns the path to the root of where all user profiles are stored.
//
// lpProfilesDir  -  Receives the path
// lpcchSize      -  Size of lpProfilesDir
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If lpProfilesDir is not large enough, the function will fail,
//           and lpcchSize will contain the necessary buffer size.
//
// Example return value: C:\Documents and Settings
//
//=============================================================================

USERENVAPI
BOOL
WINAPI
GetProfilesDirectoryA(
    OUT LPSTR lpProfilesDir,
    IN OUT LPDWORD lpcchSize);
USERENVAPI
BOOL
WINAPI
GetProfilesDirectoryW(
    OUT LPWSTR lpProfilesDir,
    IN OUT LPDWORD lpcchSize);
#ifdef UNICODE
#define GetProfilesDirectory  GetProfilesDirectoryW
#else
#define GetProfilesDirectory  GetProfilesDirectoryA
#endif // !UNICODE


//=============================================================================
//
//  GetProfileType()
//
//  Returns the type of the profile that is loaded for a user.
//
//  dwFlags   - Returns the profile flags
//
//  Return:     TRUE if successful
//              FALSE if an error occurs. Call GetLastError for more details
//
//  Comments:   if profile is not already loaded the function will return an error.
//              The caller needs to have access to HKLM part of the registry.
//              (exists by default)
//
//=============================================================================

#if(WINVER >= 0x0500)

//
// Flags that can be set in the dwFlags field
//

#define PT_TEMPORARY         0x00000001      // A profile has been allocated that will be deleted at logoff.
#define PT_ROAMING           0x00000002      // The loaded profile is a roaming profile.
#define PT_MANDATORY         0x00000004      // The loaded profile is mandatory.

USERENVAPI
BOOL
WINAPI
GetProfileType(
    OUT DWORD *dwFlags);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
//  DeleteProfile()
//
//  Deletes the profile and all other user related settings from the machine
//
//  lpSidString    - String form of the user sid.
//  lpProfilePath  - ProfilePath (if Null, lookup in the registry)
//  lpComputerName - Computer Name from which profile has to be deleted
//
//  Return:     TRUE if successful
//              FALSE if an error occurs. Call GetLastError for more details
//
//  Comments:   Deletes the profile directory, registry and appmgmt stuff
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
DeleteProfileA (
        IN LPCSTR lpSidString,
        IN LPCSTR lpProfilePath,
	IN LPCSTR lpComputerName);
USERENVAPI
BOOL
WINAPI
DeleteProfileW (
        IN LPCWSTR lpSidString,
        IN LPCWSTR lpProfilePath,
	IN LPCWSTR lpComputerName);
#ifdef UNICODE
#define DeleteProfile  DeleteProfileW
#else
#define DeleteProfile  DeleteProfileA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GetDefaultUserProfilesDirectory
//
// Returns the path to the root of the default user profile
//
// lpProfileDir   -  Receives the path
// lpcchSize      -  Size of lpProfileDir
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If lpProfileDir is not large enough, the function will fail,
//           and lpcchSize will contain the necessary buffer size.
//
// Example return value: C:\Documents and Settings\Default User
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
GetDefaultUserProfileDirectoryA(
    OUT LPSTR lpProfileDir,
    IN OUT LPDWORD lpcchSize);
USERENVAPI
BOOL
WINAPI
GetDefaultUserProfileDirectoryW(
    OUT LPWSTR lpProfileDir,
    IN OUT LPDWORD lpcchSize);
#ifdef UNICODE
#define GetDefaultUserProfileDirectory  GetDefaultUserProfileDirectoryW
#else
#define GetDefaultUserProfileDirectory  GetDefaultUserProfileDirectoryA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GetAllUsersProfilesDirectory
//
// Returns the path to the root of the All Users profile
//
// lpProfileDir   -  Receives the path
// lpcchSize      -  Size of lpProfileDir
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If lpProfileDir is not large enough, the function will fail,
//           and lpcchSize will contain the necessary buffer size.
//
// Example return value: C:\Documents and Settings\All Users
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
GetAllUsersProfileDirectoryA(
    OUT LPSTR lpProfileDir,
    IN OUT LPDWORD lpcchSize);
USERENVAPI
BOOL
WINAPI
GetAllUsersProfileDirectoryW(
    OUT LPWSTR lpProfileDir,
    IN OUT LPDWORD lpcchSize);
#ifdef UNICODE
#define GetAllUsersProfileDirectory  GetAllUsersProfileDirectoryW
#else
#define GetAllUsersProfileDirectory  GetAllUsersProfileDirectoryA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GetUserProfileDirectory
//
// Returns the path to the root of the requested user's profile
//
// hToken         -  User's token returned from LogonUser()
// lpProfileDir   -  Receives the path
// lpcchSize      -  Size of lpProfileDir
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If lpProfileDir is not large enough, the function will fail,
//           and lpcchSize will contain the necessary buffer size.
//
// Example return value: C:\Documents and Settings\Joe
//
//=============================================================================

USERENVAPI
BOOL
WINAPI
GetUserProfileDirectoryA(
    IN HANDLE  hToken,
    OUT LPSTR lpProfileDir,
    IN OUT LPDWORD lpcchSize);
USERENVAPI
BOOL
WINAPI
GetUserProfileDirectoryW(
    IN HANDLE  hToken,
    OUT LPWSTR lpProfileDir,
    IN OUT LPDWORD lpcchSize);
#ifdef UNICODE
#define GetUserProfileDirectory  GetUserProfileDirectoryW
#else
#define GetUserProfileDirectory  GetUserProfileDirectoryA
#endif // !UNICODE


//=============================================================================
//
// CreateEnvironmentBlock
//
// Returns the environment variables for the specified user.  This block
// can then be passed to CreateProcessAsUser().
//
// lpEnvironment  -  Receives a pointer to the new environment block
// hToken         -  User's token returned from LogonUser() (optional, can be NULL)
// bInherit       -  Inherit from the current process's environment block
//                   or start from a clean state.
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If hToken is NULL, the returned environment block will contain
//           system variables only.
//
//           Call DestroyEnvironmentBlock to free the buffer when finished.
//
//           If this block is passed to CreateProcessAsUser, the
//           CREATE_UNICODE_ENVIRONMENT flag must also be set.
//
//=============================================================================

USERENVAPI
BOOL
WINAPI
CreateEnvironmentBlock(
    OUT LPVOID *lpEnvironment,
    IN HANDLE  hToken,
    IN BOOL    bInherit);


//=============================================================================
//
// DestroyEnvironmentBlock
//
// Frees environment variables created by CreateEnvironmentBlock
//
// lpEnvironment  -  A pointer to the environment block
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
//=============================================================================

USERENVAPI
BOOL
WINAPI
DestroyEnvironmentBlock(
    IN LPVOID  lpEnvironment);


//=============================================================================
//
// ExpandEnvironmentStringsForUser
//
// Expands the source string using the environment block for the
// specified user.  If hToken is null, the system environment block
// will be used (no user environment variables).
//
// hToken         -  User's token returned from LogonUser() (optional, can be NULL)
// lpSrc          -  Pointer to the string with environment variables
// lpDest         -  Buffer that receives the expanded string
// dwSize         -  Size of lpDest in characters (max chars)
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If the user profile for hToken is not loaded, this api will fail.
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
ExpandEnvironmentStringsForUserA(
    IN HANDLE hToken,
    IN LPCSTR lpSrc,
    OUT LPSTR lpDest,
    IN DWORD dwSize);
USERENVAPI
BOOL
WINAPI
ExpandEnvironmentStringsForUserW(
    IN HANDLE hToken,
    IN LPCWSTR lpSrc,
    OUT LPWSTR lpDest,
    IN DWORD dwSize);
#ifdef UNICODE
#define ExpandEnvironmentStringsForUser  ExpandEnvironmentStringsForUserW
#else
#define ExpandEnvironmentStringsForUser  ExpandEnvironmentStringsForUserA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// RefreshPolicy()
//
// Causes group policy to be applied immediately on the client machine
//
// bMachine  -  Refresh machine or user policy
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
RefreshPolicy(
    IN BOOL bMachine);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// EnterCriticalPolicySection
//
// Pauses the background application of group policy to allow safe
// reading of the registry.  Applications that need to read multiple
// policy entries and ensure that the values are not changed while reading
// them should use this function.
//
// The maximum amount of time an application can hold a critical section
// is 10 minutes.  After 10 minutes, policy can be applied again.
//
// bMachine -  Pause machine or user policy
//
// Returns:  Handle if successful
//           NULL if not.  Call GetLastError() for more details
//
// Note 1:  The handle returned should be passed to LeaveCriticalPolicySection
// when finished.  Do not close this handle, LeaveCriticalPolicySection
// will do that.
//
// Note 2:  If both user and machine critical sections need to be acquired then
// they should be done in this order: first acquire user critical section and
// then acquire machine critical section.
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
HANDLE
WINAPI
EnterCriticalPolicySection(
    IN BOOL bMachine);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// LeaveCriticalPolicySection
//
// Resumes the background application of group policy.  See
// EnterCriticalPolicySection for more details.
//
// hSection - Handle returned from EnterCriticalPolicySection
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:  This function will close the handle.
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
LeaveCriticalPolicySection(
    IN HANDLE hSection);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// RegisterGPNotification
//
// Entry point for registering for Group Policy change notification.
//
// Parameters: hEvent     -   Event to be notified, by calling SetEvent(hEvent)
//             bMachine   -   If true, then register machine policy notification
//                                     else register user policy notification
//
// Returns:    True if successful
//             False if error occurs
//
// Notes:      Group Policy Notifications.  There are 2 ways an application can
//             be notify when Group Policy is finished being applied.
//
//             1) Using the RegisterGPNotifcation function and waiting for the
//                event to be signalled.
//
//             2) A WM_SETTINGCHANGE message is broadcast to all desktops.
//                wParam - 1 if machine policy was applied, 0 if user policy was applied.
//                lParam - Points to the string "Policy"
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
RegisterGPNotification(
    IN HANDLE hEvent,
    IN BOOL bMachine );

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// UnregisterGPNotification
//
// Removes registration for a Group Policy change notification.
//
// Parameters: hEvent    -   Event to be removed
//
// Returns:    True if successful
//             False if error occurs
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
UnregisterGPNotification(
    IN HANDLE hEvent );

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GPOptions flags
//
// These are the flags found in the GPOptions property of a DS object
//
// For a given DS object (Site, Domain, OU), the GPOptions property
// contains options that effect all the GPOs link to this SDOU.
//
// This is a DWORD type
//
//=============================================================================

#if(WINVER >= 0x0500)

#define GPC_BLOCK_POLICY        0x00000001  // Block all non-forced policy from above

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GPLink flags
//
// These are the flags found on the GPLink property of a DS object after
// the GPO path.
//
// For a given DS object (Site, Domain, OU), the GPLink property will
// be in this text format
//
// [LDAP://CN={E615A0E3-C4F1-11D1-A3A7-00AA00615092},CN=Policies,CN=System,DC=ntdev,DC=Microsoft,DC=Com;1]
//
// The GUID is the GPO name, and the number following the LDAP path are the options
// for that link from this DS object.  Note, there can be multiple GPOs
// each in their own square brackets in a prioritized list.
//
//=============================================================================

#if(WINVER >= 0x0500)

//
// Options for a GPO link
//

#define GPO_FLAG_DISABLE        0x00000001  // This GPO is disabled
#define GPO_FLAG_FORCE          0x00000002  // Don't override the settings in
                                            // this GPO with settings from
                                            // a GPO below it.
#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GetGPOList
//
//
// Queries for the list of Group Policy Objects for the specified
// user or machine.  This function will return a link list
// of Group Policy Objects.  Call FreeGPOList to free the list.
//
// Note, most applications will not need to call this function.
// This will primarily be used by services acting on behalf of
// another user or machine.  The caller of this function will
// need to look in each GPO for their specific policy
//
// This function can be called two different ways.  Either the hToken for
// a user or machine can be supplied and the correct name and domain
// controller name will be generated, or hToken is NULL and the caller
// must supply the name and the domain controller name.
//
// Calling this function with an hToken ensures the list of Group Policy
// Objects is correct for the user or machine since security access checking
// can be perfomed.  If hToken is not supplied, the security of the caller
// is used instead which means that list may or may not be 100% correct
// for the intended user / machine.  However, this is the fastest way
// to call this function.
//
// hToken           - User or machine token, if NULL, lpName and lpHostName must be supplied
// lpName           - User or machine name in DN format, if hToken is supplied, this must be NULL
// lpHostName       - Domain DN name or domain controller name. If hToken is supplied, this must be NULL
// lpComputerName   - Computer name to use to determine site location.  If NULL,
//                    the local computer is used as the reference. Format:  \\machinename
// dwFlags          - Flags field.  See flags definition below
// pGPOList         - Address of a pointer which receives the link list of GPOs
//
//
// Returns:  TRUE if successful
//           FALSE if not.  Use GetLastError() for more details.
//
// Examples:
//
// Here's how this function will typically be called for
// looking up the list of GPOs for a user:
//
//      LPGROUP_POLICY_OBJECT  pGPOList
//
//      if (GetGPOList (hToken, NULL, NULL, NULL, 0, &pGPOList))
//      {
//          // do processing here...
//          FreeGPOList (pGPOList)
//      }
//
//
// Here's how this function will typically be called for
// looking up the list of GPOs for a machine:
//
//      LPGROUP_POLICY_OBJECT  pGPOList
//
//      if (GetGPOList (NULL, lpMachineName, lpHostName, lpMachineName,
//                      GPO_LIST_FLAG_MACHINE, &pGPOList))
//      {
//          // do processing here...
//          FreeGPOList (pGPOList)
//      }
//
//=============================================================================

#if(WINVER >= 0x0500)

//
// Each Group Policy Object is associated (linked) with a site, domain,
// organizational unit, or machine.
//

typedef enum _GPO_LINK {
    GPLinkUnknown = 0,                     // No link information available
    GPLinkMachine,                         // GPO linked to a machine (local or remote)
    GPLinkSite,                            // GPO linked to a site
    GPLinkDomain,                          // GPO linked to a domain
    GPLinkOrganizationalUnit               // GPO linked to a organizational unit
} GPO_LINK, *PGPO_LINK;

typedef struct _GROUP_POLICY_OBJECTA {
    DWORD       dwOptions;                  // See GPLink option flags above
    DWORD       dwVersion;                  // Revision number of the GPO
    LPSTR       lpDSPath;                   // Path to the Active Directory portion of the GPO
    LPSTR       lpFileSysPath;              // Path to the file system portion of the GPO
    LPSTR       lpDisplayName;              // Friendly display name
    CHAR        szGPOName[50];              // Unique name
    GPO_LINK    GPOLink;                    // Link information
    LPARAM      lParam;                     // Free space for the caller to store GPO specific information
    struct _GROUP_POLICY_OBJECTA * pNext;   // Next GPO in the list
    struct _GROUP_POLICY_OBJECTA * pPrev;   // Previous GPO in the list
    LPSTR       lpExtensions;               // Extensions that are relevant for this GPO
    LPARAM      lParam2;                    // Free space for the caller to store GPO specific information
    LPSTR       lpLink;                     // Path to the Active Directory site, domain, or organizational unit this GPO is linked to
                                            // If this is the local GPO, this points to the word "Local"
} GROUP_POLICY_OBJECTA, *PGROUP_POLICY_OBJECTA;
typedef struct _GROUP_POLICY_OBJECTW {
    DWORD       dwOptions;                  // See GPLink option flags above
    DWORD       dwVersion;                  // Revision number of the GPO
    LPWSTR      lpDSPath;                   // Path to the Active Directory portion of the GPO
    LPWSTR      lpFileSysPath;              // Path to the file system portion of the GPO
    LPWSTR      lpDisplayName;              // Friendly display name
    WCHAR       szGPOName[50];              // Unique name
    GPO_LINK    GPOLink;                    // Link information
    LPARAM      lParam;                     // Free space for the caller to store GPO specific information
    struct _GROUP_POLICY_OBJECTW * pNext;   // Next GPO in the list
    struct _GROUP_POLICY_OBJECTW * pPrev;   // Previous GPO in the list
    LPWSTR      lpExtensions;               // Extensions that are relevant for this GPO
    LPARAM      lParam2;                    // Free space for the caller to store GPO specific information
    LPWSTR      lpLink;                     // Path to the Active Directory site, domain, or organizational unit this GPO is linked to
                                            // If this is the local GPO, this points to the word "Local"
} GROUP_POLICY_OBJECTW, *PGROUP_POLICY_OBJECTW;
#ifdef UNICODE
typedef GROUP_POLICY_OBJECTW GROUP_POLICY_OBJECT;
typedef PGROUP_POLICY_OBJECTW PGROUP_POLICY_OBJECT;
#else
typedef GROUP_POLICY_OBJECTA GROUP_POLICY_OBJECT;
typedef PGROUP_POLICY_OBJECTA PGROUP_POLICY_OBJECT;
#endif // UNICODE


//
// dwFlags for GetGPOList()
//

#define GPO_LIST_FLAG_MACHINE   0x00000001  // Return machine policy information
#define GPO_LIST_FLAG_SITEONLY  0x00000002  // Return site policy information only


USERENVAPI
BOOL
WINAPI
GetGPOListA (
    IN HANDLE hToken,
    IN LPCSTR lpName,
    IN LPCSTR lpHostName,
    IN LPCSTR lpComputerName,
    IN DWORD dwFlags,
    OUT PGROUP_POLICY_OBJECTA *pGPOList);
USERENVAPI
BOOL
WINAPI
GetGPOListW (
    IN HANDLE hToken,
    IN LPCWSTR lpName,
    IN LPCWSTR lpHostName,
    IN LPCWSTR lpComputerName,
    IN DWORD dwFlags,
    OUT PGROUP_POLICY_OBJECTW *pGPOList);
#ifdef UNICODE
#define GetGPOList  GetGPOListW
#else
#define GetGPOList  GetGPOListA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// FreeGPOList
//
//
// Frees the link list returned from GetGPOList
//
// pGPOList - Pointer to the link list of GPOs
//
//
// Returns:  TRUE if successful
//           FALSE if not
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
FreeGPOListA (
    IN PGROUP_POLICY_OBJECTA pGPOList);
USERENVAPI
BOOL
WINAPI
FreeGPOListW (
    IN PGROUP_POLICY_OBJECTW pGPOList);
#ifdef UNICODE
#define FreeGPOList  FreeGPOListW
#else
#define FreeGPOList  FreeGPOListA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GetAppliedGPOList
//
// Queries for the list of applied Group Policy Objects for the specified
// user or machine and specified client side extension. This function will return
// a linked listof Group Policy Objects.  Call FreeGPOList to free the list.
//
// dwFlags          - User or machine policy, if it is GPO_LIST_FLAG_MACHINE then
//                    return machine policy information
// pMachineName     - Name of remote computer in the form \\computername. If null
//                    then local computer is used.
// pSidUser         - Security id of user (relevant for user policy). If pMachineName is
//                    null and pSidUser is null then it means current logged on user.
//                    If pMachine is null and pSidUser is non-null then it means user
//                    represented by pSidUser on local machine. If pMachineName is non-null
//                    then and if dwFlags specifies user policy, then pSidUser must be
//                    non-null.
// pGuidExtension   - Guid of the specified extension
// ppGPOList        - Address of a pointer which receives the link list of GPOs
//
// The return value is a Win32 error code. ERROR_SUCCESS means the GetAppliedGPOList
// function completed successfully. Otherwise it indicates that the function failed.
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
DWORD
WINAPI
GetAppliedGPOListA (
    IN DWORD dwFlags,
    IN LPCSTR pMachineName,
    IN PSID pSidUser,
    IN GUID *pGuidExtension,
    OUT PGROUP_POLICY_OBJECTA *ppGPOList);
USERENVAPI
DWORD
WINAPI
GetAppliedGPOListW (
    IN DWORD dwFlags,
    IN LPCWSTR pMachineName,
    IN PSID pSidUser,
    IN GUID *pGuidExtension,
    OUT PGROUP_POLICY_OBJECTW *ppGPOList);
#ifdef UNICODE
#define GetAppliedGPOList  GetAppliedGPOListW
#else
#define GetAppliedGPOList  GetAppliedGPOListA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// Group Policy Object client side extension support
//
// Flags, data structures and function prototype
//
// To register your extension, create a subkey under this key
//
// Software\Microsoft\Windows NT\CurrentVersion\Winlogon\GPExtensions
//
// The subkey needs to be a guid so that it is unique. The noname value of the subkey
// can be the friendly name of the extension. Then add these values:
//
//     DllName                      REG_EXPAND_SZ  Path to your DLL
//     ProcessGroupPolicy           REG_SZ       Function name (see PFNPROCESSGROUPPOLICY prototype)
//     NoMachinePolicy              REG_DWORD    True, if extension does not have to be called when
//                                                 machine policies are being processed.
//     NoUserPolicy                 REG_DWORD    True, if extension does not have to be called when
//                                                 user policies are being processed.
//     NoSlowLink                   REG_DWORD    True, if extension does not have to be called on a slow link
//     NoBackgroundPolicy           REG_DWORD    True, if extension does not have to be called on.
//                                                 policies applied in background.
//     NoGPOListChanges             REG_DWORD    True, if extension does not have to be called when
//                                                 there are no changes between cached can current GPO lists.
//     PerUserLocalSettings         REG_DWORD    True, if user policies have to be cached on a per user and
//                                                 per machine basis.
//     RequiresSuccessfulRegistry   REG_DWORD    True, if extension should be called only if registry extension
//                                                 was successfully processed.
//     EnableAsynchronousProcessing REG_DWORD    True, if registry extension will complete its processing
//                                                 asynchronously.
//     NotifyLinkTransition         REG_DWORD    True, if extension should be called when a change in link
//                                                 speed is detected between previous policy application and
//                                                 current policy application.
//
// The return value is a Win32 error code. ERROR_SUCCESS means the ProcessGroupPolicy
// function completed successfully. If return value is ERROR_OVERRIDE_NOCHANGES then it
// means that the extension will be called the next time even if NoGPOListChanges is set
// and there are no changes to the GPO list. Any other return value indicates that the
// ProcessGroupPolicy function failed.
//
//=============================================================================

#if(WINVER >= 0x0500)

#define GPO_INFO_FLAG_MACHINE          0x00000001  // Apply machine policy rather than user policy
#define GPO_INFO_FLAG_BACKGROUND       0x00000010  // Background refresh of policy (ok to do slow stuff)
#define GPO_INFO_FLAG_SLOWLINK         0x00000020  // Policy is being applied across a slow link
#define GPO_INFO_FLAG_VERBOSE          0x00000040  // Verbose output to the eventlog
#define GPO_INFO_FLAG_NOCHANGES        0x00000080  // No changes were detected to the Group Policy Objects
#define GPO_INFO_FLAG_LINKTRANSITION   0x00000100  // A change in link speed was detected between previous policy
                                                   // application and current policy application

typedef UINT_PTR ASYNCCOMPLETIONHANDLE;
typedef DWORD (*PFNSTATUSMESSAGECALLBACK)(BOOL bVerbose, LPWSTR lpMessage);

typedef DWORD(*PFNPROCESSGROUPPOLICY)(
    IN DWORD dwFlags,                              // GPO_INFO_FLAGS
    IN HANDLE hToken,                              // User or machine token
    IN HKEY hKeyRoot,                              // Root of registry
    IN PGROUP_POLICY_OBJECT  pDeletedGPOList,      // Linked list of deleted GPOs
    IN PGROUP_POLICY_OBJECT  pChangedGPOList,      // Linked list of changed GPOs
    IN ASYNCCOMPLETIONHANDLE pHandle,              // For asynchronous completion
    IN BOOL *pbAbort,                              // If true, then abort GPO processing
    IN PFNSTATUSMESSAGECALLBACK pStatusCallback);  // Callback function for displaying status messages
                                                   // Note, this can be NULL

//
// GUID that identifies the registry extension
//

#define REGISTRY_EXTENSION_GUID  { 0x35378EAC, 0x683F, 0x11D2, 0xA8, 0x9A, 0x00, 0xC0, 0x4F, 0xBB, 0xCF, 0xA2 }

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// Group Policy Object client side asynchronous extension processing
//
// extensionId    - Unique guid identifying the extension
// pAsyncHandle   - Asynchronous completion handle that was passed to extension in
//                  ProcessGroupPolicy call
// dwStatus       - Completion status of asynchronous processing
//
// The return value is a Win32 error code. ERROR_SUCCESS means the ProcessGroupPolicyCompleted
// function completed successfully. Otherwise it indicates that the function failed.
//
//=============================================================================

#if(WINVER >= 0x0500)

typedef GUID *REFGPEXTENSIONID;

USERENVAPI
DWORD
WINAPI
ProcessGroupPolicyCompleted(
    IN REFGPEXTENSIONID extensionId,
    IN ASYNCCOMPLETIONHANDLE pAsyncHandle,
    IN DWORD dwStatus);

#endif /* WINVER >= 0x0500 */


#ifdef __cplusplus
}
#endif


#endif // _INC_USERENV
