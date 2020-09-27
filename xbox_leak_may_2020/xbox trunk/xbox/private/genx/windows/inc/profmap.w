;begin_internal
//=============================================================================
//  profmapp.h  -   Header file for user remap API.
//
//  Copyright (c) Microsoft Corporation 1995-1999
//  All rights reserved
//
//=============================================================================

//
// BUGBUG: These two APIs are internal only at this point.
//         Remove begin_internal above, then restore commented
//         out lines below, indicated by BUGBUG, and restore
//         the semicolon.
//
// BUGBUG: #if (_WIN32_WINNT >= 0x0500)
// BUGBUG: begin_both

//=============================================================================
//
// RemapUserProfile
//
// Changes the security of a user profile from one user to another.
//
// pComputer    - Specifies the computer to run the API on
// dwFlags      - Specifies zero or more REMAP_PROFILE_* flags
// pCurrentSid  - Specifies the existing user's SID
// pNewSid      - Specifies the new SID for the profile
//
// Returns:  TRUE if successful
//           FALSE if not.
//
//=============================================================================

#define REMAP_PROFILE_NOOVERWRITE           0x0001
#define REMAP_PROFILE_NOUSERNAMECHANGE      0x0002
#define REMAP_PROFILE_KEEPLOCALACCOUNT      0x0004

USERENVAPI
BOOL
WINAPI
RemapUserProfile%(
    LPCTSTR% pComputer,
    DWORD dwFlags,
    PSID pSidCurrent,
    PSID pSidNew
    );

// BUGBUG: end_both
// BUGBUG: #endif
// BUGBUG: begin_internal

BOOL
WINAPI
InitializeProfileMappingApi (
    VOID
    );

// BUGBUG: end_internal
// BUGBUG: #if (_WIN32_WINNT >= 0x0500)
// BUGBUG: begin_both

//=============================================================================
//
// RemapAndMoveUser
//
// Transfers security settings and the user profile for one user to
// another.
//
// pComputer    - Specifies the computer to run the API on
// dwFlags      - Specifies zero or mor REMAP_PROFILE_* flags
// pCurrentUser - Specifies the existing user's SID
// pNewUser     - Specifies the new SID for the profile
//
// Returns:  TRUE if successful
//           FALSE if not.
//
//=============================================================================

USERENVAPI
BOOL
WINAPI
RemapAndMoveUser%(
    LPCTSTR% pComputer,
    DWORD dwFlags,
    LPCTSTR% pCurrentUser,
    LPCTSTR% pNewUser
    );

// BUGBUG: end_both
// BUGBUG: #endif
//
// BUGBUG: Remove this end_internal if the RemapUserProfile and
//         RemapAndMoveUser APIs are made public.
//
;end_internal
