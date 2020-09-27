/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  keys.c

Abstract:

  This modules tests the xbox lan key and xbox sig key

Author:

  Steven Kehrli (steveke) 9-Jul-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XboxKeysNamespace;

namespace XboxKeysNamespace {

VOID
XboxKeysTest(
    IN HANDLE     hLog,
    IN BOOL       bKeys,
    IN PXBOXKEYS  pXboxKeys
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests xbox lan key and xbox sig key

Arguments:

  hLog - handle to the xLog log object
  bKeys - specifies if the xbox keys structure is valid
  pXboxKeys - pointer to the xbox keys structure

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // szXboxKey is the string representation of the xbox key
    char   szXboxKey[XBOX_KEY_LENGTH * 2 + 1];
    // dwByteIndex is a counter to enumerate each byte of the key
    DWORD  dwByteIndex = 0;



    // Set the component and subcomponent
    xSetComponent(hLog, "Kernel(S)", "Certificate-SignatureValidation(S-k)");

    // Set the function name
    xSetFunctionName(hLog, "XboxSignatureKey & XboxLANKey");

    // Start the variation
    xStartVariation(hLog, "XboxSignatureKey Cert");

    // Format the xbox sig key
    for (dwByteIndex = 0; dwByteIndex < XBOX_KEY_LENGTH; dwByteIndex++) {
        sprintf(&szXboxKey[dwByteIndex * 2], "%02x", (*XboxSignatureKey)[dwByteIndex]);
    }
    xLog(hLog, XLL_INFO, "XboxSignatureKey: %s", szXboxKey);

    // Compare the xbox sig key
    if (FALSE == bKeys) {
        CopyMemory(pXboxKeys->XboxSignatureKey, XboxSignatureKey, XBOX_KEY_LENGTH * sizeof(UCHAR));

        if (0 != memcmp(pXboxKeys->XboxSignatureKey, XboxSignatureKey, XBOX_KEY_LENGTH * sizeof(UCHAR))) {
            xLog(hLog, XLL_FAIL, "XboxSignatureKey was changed");
        }
        else {
            xLog(hLog, XLL_PASS, "XboxSignatureKey passed");
        }
    }
    else {
        if (0 == memcmp(pXboxKeys->XboxSignatureKey, XboxSignatureKey, XBOX_KEY_LENGTH * sizeof(UCHAR))) {
            xLog(hLog, XLL_FAIL, "XboxSignatureKey was not changed");
        }
        else {
            xLog(hLog, XLL_PASS, "XboxSignatureKey passed");
        }
    }

    // End the variation
    xEndVariation(hLog);

    // Start the variation
    xStartVariation(hLog, "XboxLANKey Cert");

    // Format the xbox lan key
    for (dwByteIndex = 0; dwByteIndex < XBOX_KEY_LENGTH; dwByteIndex++) {
        sprintf(&szXboxKey[dwByteIndex * 2], "%02x", (*XboxLANKey)[dwByteIndex]);
    }
    xLog(hLog, XLL_INFO, "XboxLANKey: %s", szXboxKey);

    // Compare the xbox lan key
    if (FALSE == bKeys) {
        CopyMemory(pXboxKeys->XboxLANKey, XboxLANKey, XBOX_KEY_LENGTH * sizeof(UCHAR));

        if (0 != memcmp(pXboxKeys->XboxLANKey, XboxLANKey, XBOX_KEY_LENGTH * sizeof(UCHAR))) {
            xLog(hLog, XLL_FAIL, "XboxLANKey was changed");
        }
        else {
            xLog(hLog, XLL_PASS, "XboxLANKey passed");
        }
    }
    else {
        if (0 == memcmp(pXboxKeys->XboxLANKey, XboxLANKey, XBOX_KEY_LENGTH * sizeof(UCHAR))) {
            xLog(hLog, XLL_FAIL, "XboxLANKey was not changed");
        }
        else {
            xLog(hLog, XLL_PASS, "XboxLANKey passed");
        }
    }

    // End the variation
    xEndVariation(hLog);
}

} // namespace XboxKeysNamespace
