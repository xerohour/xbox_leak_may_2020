/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xboxkeys.h

Abstract:

  This module contains the function definitions

Author:

  Steven Kehrli (steveke) 9-Jul-2001

------------------------------------------------------------------------------*/

#pragma once



namespace XboxKeysNamespace {

typedef struct _XBOXKEYS {
    UCHAR  XboxSignatureKey[XBOX_KEY_LENGTH];  // XboxSignatureKey is the xbox sig key
    UCHAR  XboxLANKey[XBOX_KEY_LENGTH];        // XboxLANKey is the xbox lan key
} XBOXKEYS, *PXBOXKEYS;



// Function prototypes

VOID
XboxKeysTest(
    IN HANDLE     hLog,
    IN BOOL       bKeys,
    IN PXBOXKEYS  pXboxKeys
);

VOID
XboxKeysNetworkTest(
    IN HANDLE  hLog,
    IN BOOL    bKeys
);

} // namespace XboxKeysNamespace
