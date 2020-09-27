#ifndef KERBSOCKETS_H
#define KERBSOCKETS_H

class KerbSockets
{
public:
    KerbSockets()
    {
        WSADATA wsaData;
        WORD wVersionRequested = MAKEWORD(2, 2);
        int err = WSAStartup(wVersionRequested, &wsaData);

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
            WSACleanup();
        }
    }

    ~KerbSockets()
    {
        WSACleanup();
    }

    NTSTATUS KerbMakeKdcCall(
        IN IN_ADDR IpAddress,
        IN BOOLEAN UseTcp,
        IN PKERB_MESSAGE_BUFFER RequestMessage,
        IN PKERB_MESSAGE_BUFFER ReplyMessage
        );

private:
    VOID KerbCloseSocket(
        IN SOCKET SocketHandle
        );

    NTSTATUS KerbBindSocketByAddress(
        IN ULONG Address,
        IN ULONG AddressType,
        IN BOOLEAN UseDatagram,
        IN USHORT PortNumber,
        OUT SOCKET * ContextHandle
        );

    NTSTATUS KerbCallKdc(
        IN ULONG KdcAddress,
        IN ULONG AddressType,
        IN ULONG Timeout,
        IN BOOLEAN UseDatagram,
        IN USHORT PortNumber,
        IN PKERB_MESSAGE_BUFFER Input,
        OUT PKERB_MESSAGE_BUFFER Output
        );
};

#endif
