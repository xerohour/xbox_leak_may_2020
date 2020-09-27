/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  keysnet.c

Abstract:

  This modules tests the xbox lan key during broadcast and lan key exchange

Author:

  Steven Kehrli (steveke) 9-Jul-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef XBOXKEYS_CLIENT

using namespace XboxKeysNamespace;

namespace XboxKeysNamespace {

NETSYNC_TYPE_THREAD  XboxKeysTestSessionXbox =
{
    1,
    1,
    L"xboxkeys_xbox.dll",
    "XboxKeysTestServer"
};



VOID
XboxKeysNetworkTest(
    IN HANDLE  hLog,
    IN BOOL    bKeys
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests xbox lan key during broadcast and lan key exchange

Arguments:

  hLog - handle to the xLog log object
  bKeys - specifies if the xbox keys structure is valid

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    // hNetsyncObject is a handle to the netsync object
    HANDLE   hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncAddr is the address of the netsync server
    u_long   NetsyncAddr = 0;



    // Set the component and subcomponent
    xSetComponent(hLog, "Network(S)", "Others(S-n)");

    // Set the function name
    xSetFunctionName(hLog, "XboxLANKey");

    // Start the variation
    xStartVariation(hLog, "XboxLANKey");

    // Connect to the session
    hNetsyncObject = NetsyncCreateClient(0, NETSYNC_SESSION_THREAD, &XboxKeysTestSessionXbox, &NetsyncAddr, NULL, NULL, NULL);

    // Check the xbox sig key
    if (TRUE == bKeys) {
        if (INVALID_HANDLE_VALUE != hNetsyncObject) {
            xLog(hLog, XLL_FAIL, "Netsync response: XboxLANKey failed");
        }
        else {
            xLog(hLog, XLL_PASS, "XboxLANKey passed");
        }
    }
    else {
        if (INVALID_HANDLE_VALUE == hNetsyncObject) {
            xLog(hLog, XLL_FAIL, "Netsync no response: XboxLANKey failed");
        }
        else {
            xLog(hLog, XLL_PASS, "XboxLANKey passed");
        }
    }

    if (INVALID_HANDLE_VALUE != hNetsyncObject) {
        // Close the netsync client object
        NetsyncCloseClient(hNetsyncObject);
    }

    // End the variation
    xEndVariation(hLog);
}

} // namespace XboxKeysNamespace



#else



VOID
WINAPI
XboxKeysTestServer(
    IN HANDLE   hNetsyncObject,
    IN BYTE     byClientCount,
    IN u_long   *ClientAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests xboxkeys - Server side

Arguments:

  hNetsyncObject - Handle to the netsync object
  byClientCount - Specifies the number of clients in the session
  ClientAddrs - Pointer to an array of client addresses
  LowPort - Specifies the low bound of the netsync port range
  HighPort - Specifies the high bound of the netsync port range

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // FromAddr is the address of the netsync sender
    u_long                    FromAddr;
    // dwMessageType is the type of received message
    DWORD                     dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD                     dwMessageSize;
    // pMessage is a pointer to the received message
    char                      *pMessage;



    while (TRUE) {
        // Receive a message
        NetsyncReceiveMessage(hNetsyncObject, INFINITE, &dwMessageType, &FromAddr, NULL, &dwMessageSize, &pMessage);
        if ((NETSYNC_MSGTYPE_SERVER == dwMessageType) && ((NETSYNC_MSG_STOPSESSION == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) || (NETSYNC_MSG_DELETECLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId))) {
            NetsyncFreeMessage(pMessage);
            break;
        }

        NetsyncFreeMessage(pMessage);
    }
}

#endif // XBOXKEYS_CLIENT
