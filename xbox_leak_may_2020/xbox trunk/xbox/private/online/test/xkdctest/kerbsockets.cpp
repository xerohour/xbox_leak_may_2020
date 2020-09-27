//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1996.
//
//  File:       sockets.cxx
//
//  Contents:   Code for kerberos client sockets
//
//  Classes:
//
//  Functions:
//
//  History:    26-Jul-1996     MikeSw          Created
//
//----------------------------------------------------------------------------

#include "xkdctest.h"
#include "kerberos.h"

#define DS_INET_ADDRESS    1

#define KERB_KDC_PORT       88

//
// These are arbitrary sizes for max request and responses sizes for datagram
// requests.
//

#define KERB_MAX_KDC_RESPONSE_SIZE      4000
#define KERB_MAX_KDC_REQUEST_SIZE       4000
#define KERB_MAX_DATAGRAM_SIZE          1460
#define KERB_MAX_RETRIES                3

//
// timeout values in seconds
//

#define KERB_KDC_CALL_TIMEOUT                   5
#define KERB_KDC_CALL_TIMEOUT_BACKOFF           5
#define KERB_KDC_WAIT_TIME      120


//+-------------------------------------------------------------------------
//
//  Function:   KerbCloseSocket
//
//  Synopsis:   Closes a socket binding handle
//
//  Effects:    calls closesocket on the handle
//
//  Arguments:  SocketHandle - handle to close
//
//  Requires:
//
//  Returns:    none
//
//  Notes:
//
//
//--------------------------------------------------------------------------

VOID
CXoTest::KerbCloseSocket(
    IN SOCKET SocketHandle
    )
{
    int SockError;
    if (SocketHandle != 0)
    {
        SockError = GetXo()->closesocket(SocketHandle);
        if (SockError != 0)
        {
            TraceSz1( AuthWarn, "CloseSocket failed: last error %d", WSAGetLastError());
        }
    }
}


//+-------------------------------------------------------------------------
//
//  Function:   KerbBindSocketByAddress
//
//  Synopsis:   Binds to the KDC socket on the specified address
//
//  Effects:
//
//  Arguments:  Address - Address to bind to
//              AddressType - Address type, as specified by DC locator
//              ContextHandle - Receives bound socket
//
//  Requires:
//
//  Returns:
//
//  Notes:
//
//
//--------------------------------------------------------------------------

NTSTATUS
CXoTest::KerbBindSocketByAddress(
    IN ULONG Address,
    IN ULONG AddressType,
    IN BOOLEAN UseDatagram,
    IN USHORT PortNumber,
    OUT SOCKET * ContextHandle
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    SOCKET ClientSocket = INVALID_SOCKET;
    struct sockaddr_in ServerAddress;
    struct sockaddr_in ClientAddress;
    int serr;
    LINGER  l_linger;

    ClientSocket = GetXo()->socket(
                    PF_INET,
                    (UseDatagram ? SOCK_DGRAM : SOCK_STREAM),
                    0
                    );
    if (ClientSocket == INVALID_SOCKET)
    {
        TraceSz1( AuthWarn, "Failed to create socket: %d", WSAGetLastError());
        Status = STATUS_NO_LOGON_SERVERS;
        goto Cleanup;
    }

    /*// We enable linger, but set a linger timeout of zero. This forces a hard
    // close on closesocket. This is what we have to do to make sure 1) the
    // closesocket call returns synchronously, and 2) sockets don't linger
    // around trying to gracefully close and cause xbox to run out of sockets
    l_linger.l_onoff = 1;
    l_linger.l_linger = 0;
    serr = setsockopt(ClientSocket, SOL_SOCKET, 
                SO_LINGER, (char *)&l_linger, sizeof(l_linger));
    if (serr == SOCKET_ERROR)
    {
        TraceSz1( AuthWarn, "Failed to setsockopt: %d", WSAGetLastError());
        Status = STATUS_NO_LOGON_SERVERS;
        goto Cleanup;
    }*/

    GetXo()->XnSetInsecure(ClientSocket);
    
    if (UseDatagram)
    {
        //
        // Bind client socket to any local interface and port
        //

        ClientAddress.sin_family = AF_INET;
        ClientAddress.sin_addr.s_addr = INADDR_ANY;
        ClientAddress.sin_port = 0;                 // no specific port

        if (GetXo()->bind(
                ClientSocket,
                (LPSOCKADDR) &ClientAddress,
                sizeof(ClientAddress)
                ) == SOCKET_ERROR )
        {
            TraceSz1( AuthWarn, "Failed to bind client socket: %d", WSAGetLastError());
            Status = STATUS_NO_LOGON_SERVERS;
            goto Cleanup;
        }
    }

    if (AddressType == DS_INET_ADDRESS)
    {
        ServerAddress.sin_family = AF_INET;

        RtlCopyMemory(
            &ServerAddress.sin_addr,
            &Address,
            sizeof(ULONG)
            );

    }
    else
    {
        //
        // Get the address of the server
        //
        Assert(!"Must use DS_INET_ADDRESS");
    }

    ServerAddress.sin_port = htons(PortNumber);

    if (GetXo()->connect(
            ClientSocket,
            (LPSOCKADDR) &ServerAddress,
            sizeof(ServerAddress)
            ) == SOCKET_ERROR)
    {
        TraceSz1( AuthWarn, "Failed to connect to server: %d", WSAGetLastError());
        Status = STATUS_NO_LOGON_SERVERS;
        goto Cleanup;
    }
    *ContextHandle = ClientSocket;

Cleanup:
    if (!NT_SUCCESS(Status))
    {
        if (ClientSocket != INVALID_SOCKET)
        {
            GetXo()->closesocket(ClientSocket);
        }
    }
    return(Status);


}


//+-------------------------------------------------------------------------
//
//  Function:   KerbCallKdc
//
//  Synopsis:   Socket client stub for calling the KDC.
//
//  Effects:
//
//  Arguments:
//
//  Requires:
//
//  Returns:
//
//  Notes:
//
//
//--------------------------------------------------------------------------


NTSTATUS
CXoTest::KerbCallKdc(
    IN ULONG KdcAddress,
    IN ULONG AddressType,
    IN ULONG Timeout,
    IN BOOLEAN UseDatagram,
    IN USHORT PortNumber,
    IN PKERB_MESSAGE_BUFFER Input,
    OUT PKERB_MESSAGE_BUFFER Output
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    ULONG Bytes;
    int NumberReady;
    SOCKET Socket = 0;
    PUCHAR RemainingBuffer;
    ULONG RemainingSize;
    ULONG SendSize ;
    fd_set ReadHandles;
    struct timeval TimeoutTime;
    ULONG NetworkSize;
    BOOLEAN RetriedOnce = FALSE;

    WSABUF Buffers[2] = {0};
    LPWSABUF SendBuffers = NULL;
    ULONG BufferCount = 0;
    int SendStatus;

    Output->Buffer = NULL;
    
    //
    // Start out by binding to the KDC
    //
  
    Status = KerbBindSocketByAddress(
                KdcAddress,
                AddressType,
                UseDatagram,
                PortNumber,
                &Socket
                );
    if (!NT_SUCCESS(Status))
    {
        goto Cleanup;
    }

    RemainingBuffer = Input->Buffer;
    RemainingSize = Input->BufferSize;

    //
    // Use winsock2
    //

    Buffers[0].len = sizeof(ULONG);
    NetworkSize = htonl(RemainingSize);
    Buffers[0].buf = (PCHAR) &NetworkSize;
    Buffers[1].len = Input->BufferSize;
    Buffers[1].buf = (PCHAR) Input->Buffer;

    if (UseDatagram)
    {
        BufferCount = 1;
        SendBuffers = &Buffers[1];
        RemainingSize = Buffers[1].len;
    }
    else
    {
        BufferCount = 2;
        SendBuffers = &Buffers[0];
        RemainingSize = Buffers[0].len + Buffers[1].len;
    }

RetrySend:

    SendStatus = GetXo()->WSASend(
                    Socket,
                    SendBuffers,
                    BufferCount,
                    &Bytes,
                    0,          // no flags
                    NULL,               // no overlapped
                    NULL                // no completion routine
                    );

    TraceSz5( AuthVerbose, "KerbCallKdc sent %d bytes to %d.%d.%d.%d",Bytes,
        (*((BYTE*)&KdcAddress)),(*(((BYTE*)&KdcAddress)+1)),(*(((BYTE*)&KdcAddress)+2)),(*(((BYTE*)&KdcAddress)+3))
        );

    if ((SendStatus != 0) || (Bytes == 0))
    {
        Assert(SendStatus == SOCKET_ERROR);
        TraceSz1( AuthWarn, "Failed to send data: %d", WSAGetLastError());
        Status = SEC_E_NO_AUTHENTICATING_AUTHORITY;
        goto Cleanup;
    }
    if (Bytes < RemainingSize)
    {
        RemainingSize -= Bytes;
        if (Bytes > SendBuffers->len)
        {
            //
            // We sent the whole of a buffer, so move on to the next
            //

            Bytes -= SendBuffers->len;

            Assert(BufferCount > 1);
            BufferCount--;
            SendBuffers++;
            SendBuffers->len -= Bytes;
            SendBuffers->buf += Bytes;
        }
        else
        {
            SendBuffers->len -= Bytes;
            SendBuffers->buf += Bytes;
        }
        goto RetrySend;
    }


    //
    // Now select on the socket and wait for a response
    // ReadHandles and TimeoutTime must be reset each time, cause winsock
    // zeroes them out in case of error

    ReadHandles.fd_count = 1;
    ReadHandles.fd_array[0] = Socket;
    TimeoutTime.tv_sec = Timeout;
    TimeoutTime.tv_usec = 0;

    NumberReady = GetXo()->select(
                    1,
                    &ReadHandles,
                    NULL,
                    NULL,
                    &TimeoutTime
                    );
    if ((NumberReady == SOCKET_ERROR) ||
        (NumberReady == 0))
    {

        TraceSz2( AuthWarn, "Failed to select from kdc %x%X, NumberReady: %d", WSAGetLastError(), NumberReady );

        //
        // Retry again and wait.
        //

        if ((NumberReady == 0) && (!RetriedOnce))
        {
            RetriedOnce = TRUE;
            goto RetrySend;
        }
        Status = STATUS_NO_LOGON_SERVERS;
        goto Cleanup;
    }

    //
    // Now receive the data
    //

    if (UseDatagram)
    {
        Output->BufferSize = KERB_MAX_KDC_RESPONSE_SIZE;
        Output->Buffer = (PUCHAR) LocalAlloc(LMEM_ZEROINIT,KERB_MAX_KDC_RESPONSE_SIZE);
        if (Output->Buffer == NULL)
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }
        Bytes = GetXo()->recv(
                    Socket,
                    (char *) Output->Buffer,
                    Output->BufferSize,
                    0
                    );
        if ((Bytes == SOCKET_ERROR) || (Bytes == 0))
        {
            DWORD err=WSAGetLastError();
            TraceSz1( AuthWarn, "Failed to receive socket data: %d", WSAGetLastError());
            Status = SEC_E_NO_AUTHENTICATING_AUTHORITY;
            goto Cleanup;
        }
        Output->BufferSize = Bytes;
    }
    else
    {
        Bytes = GetXo()->recv(
                    Socket,
                    (char *) &NetworkSize,
                    sizeof(ULONG),
                    0
                    );
        if (Bytes != sizeof(ULONG) )
        {
            TraceSz1( AuthWarn, "Failed to receive socket data: %d", WSAGetLastError());
            Status = SEC_E_NO_AUTHENTICATING_AUTHORITY;
            goto Cleanup;
        }
        RemainingSize = ntohl(NetworkSize);
        Output->BufferSize = RemainingSize;
        Output->Buffer = (PUCHAR) LocalAlloc(LMEM_ZEROINIT,RemainingSize);
        if (Output->Buffer == NULL)
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }
        while (RemainingSize != 0)
        {
            //
            // Make sure there is data ready
            //

            NumberReady = GetXo()->select(
                            1,
                            &ReadHandles,
                            NULL,
                            NULL,
                            &TimeoutTime
                            );
            if ((NumberReady == SOCKET_ERROR) ||
                (NumberReady == 0))
            {
                TraceSz2( AuthWarn, "Failed to select from kdc %x%X, NumberReady: %d", WSAGetLastError(), NumberReady );

                Status = STATUS_NO_LOGON_SERVERS;
                goto Cleanup;
            }

            //
            // Receive the data
            //

            Bytes = GetXo()->recv(
                        Socket,
                        (char *) Output->Buffer + Output->BufferSize - RemainingSize,
                        RemainingSize,
                        0
                        );
            if ((Bytes == SOCKET_ERROR) || (Bytes == 0))
            {
                TraceSz1( AuthWarn, "Failed to receive socket data: %d", WSAGetLastError());
                Status = SEC_E_NO_AUTHENTICATING_AUTHORITY;
                goto Cleanup;
            }
            RemainingSize -= Bytes;
        }
    }

    TraceSz5( AuthVerbose, "KerbCallKdc received %d bytes from %d.%d.%d.%d",Output->BufferSize,
        (*((BYTE*)&KdcAddress)),(*(((BYTE*)&KdcAddress)+1)),(*(((BYTE*)&KdcAddress)+2)),(*(((BYTE*)&KdcAddress)+3))
        );

Cleanup:
    if (Socket != 0)
    {
        KerbCloseSocket(Socket);
    }
    if (!NT_SUCCESS(Status))
    {
        if (Output->Buffer != NULL)
        {
            LocalFree(Output->Buffer);
            Output->Buffer = NULL;
        }
    }
    return(Status);

}

#ifdef TONYCHEN_PRIVATE
ULONG KerbGlobalKdcCallTimeout = 1000000;
ULONG KerbGlobalKdcCallBackoff = 1000000;
ULONG KerbGlobalKdcSendRetries = 0;
#else
ULONG KerbGlobalKdcCallTimeout = KERB_KDC_CALL_TIMEOUT;
ULONG KerbGlobalKdcCallBackoff = KERB_KDC_CALL_TIMEOUT_BACKOFF;
ULONG KerbGlobalKdcSendRetries = KERB_MAX_RETRIES;
#endif

ULONG KerbGlobalMaxDatagramSize = KERB_MAX_DATAGRAM_SIZE;

//--------------------------------------------------------------------------

NTSTATUS
CXoTest::KerbMakeKdcCall(
    IN DWORD dwIP, 
    IN USHORT PortNumber,
    IN BOOLEAN UseTcp,
    IN PKERB_MESSAGE_BUFFER RequestMessage,
    IN PKERB_MESSAGE_BUFFER ReplyMessage
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    ULONG Retries;
    ULONG Timeout = KerbGlobalKdcCallTimeout;

    Retries = 0;
    do
    {
        //
        // don't force retry the first time
        //

        if (Retries > 0)
        {
            Timeout += KerbGlobalKdcCallBackoff;
        }

        if  (RequestMessage->BufferSize > KerbGlobalMaxDatagramSize)
        {
            UseTcp = TRUE;
        }

        Status =  KerbCallKdc(
                    dwIP,
                    DS_INET_ADDRESS,
                    Timeout,
                    !UseTcp,
                    PortNumber, 
                    RequestMessage,
                    ReplyMessage
                    );

        if (!NT_SUCCESS(Status) )
        {
            //
            // If the request used UDP and we got an invalid buffer size error,
            // try again with TCP.
            //

            if ((Status == STATUS_INVALID_BUFFER_SIZE) && (!UseTcp))
            {

                UseTcp = TRUE;
                Status =  KerbCallKdc(
                            dwIP,
                            DS_INET_ADDRESS,
                            Timeout,
                            !UseTcp,
                            KERB_KDC_PORT,
                            RequestMessage,
                            ReplyMessage
                            );
            }

        }

        Retries++;
    } while ( !NT_SUCCESS(Status) && (Retries < KerbGlobalKdcSendRetries) );

    if (Status != STATUS_SUCCESS)
    {
        TraceSz1( AuthWarn, "KerbMakeKdcCall failure status %X", Status );
    }
    return(Status);
}
