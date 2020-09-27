=========
 General
=========

- We don't support any protocol other than TCP/IP.
- We're strictly a client-side implementation.
- Overall priorities are:
    code simplicity / compactness
    performance
    resource consumption
- Adjustable configuration parameters are stored
  in global variables with 'cfg' name prefix.

===================
 Buffer management
===================

- There are two kinds of packets:
    regular packets allocated out of pool memory, and
    DMA packets allocated inside contiguous physical page
- We maintain a few lookaside lists for smaller-sized regular packets.
- DMA packets are fixed size, ~half a page and large enough
  to hold an Ethernet frame.
- When packets are passed around, the callee takes over the ownership
  of the packet. When the callee is done with the packet, it calls
  CompletePacket to dispose of it. Exception to this rule will be
  explicitly noted.

============
 Interfaces
============

- Each interface has only one IP address.
- Interfaces supported:
    Ethernet
    PPP over USB serial modem
    PPP over Ethernet
    loopback
- Ethernet:
    receive both Ethernet and IEEE 802.3 frames
    send Ethernet frames only
    assume we hear our own broadcast/multicast transmissions
- Default ARP configuration parameters:
    positive ARP cache entry timeout = 10min
    negative ARP cache entry timeout = 1min
    ARP request retry count = 2

========
IP Layer
========

- We implement "Strong End-System" model, i.e.
    when a datagram arrives on an interface, the destination
    address must match that interface's address (or an
    approriate broadcast / multicast address)
- We only support the all-1's form of broadcast addresses.
- We do NOT support fragmentation on outgoing datagrams.
- The default size limit for reassembled datagrams is 2048 bytes
  and the default reassembly timeout period is 60sec.
- By default, the maximum number of datagrams that can be reassembled
  simultaneously is 4.
- Subnet-directed broadcast are not received locally.

=======
Winsock
=======

- Our socket handles are not file handles, i.e.
  you can NOT call Read/WriteFile APIs on them.
- Only limited validatation is performed on socket handles
  passed into the API calls. If app passes in a random
  handle value, the resulting behavior will be undefined.
- Overlapped completion routines are not supported.
- Since our socket handles are not file handles, apps can NOT
  call CancelIO API to cancel outstanding overlapped I/O requests.
  Apps must call WSACancelOverlappedIO function instead.
- We claim to support the following Winsock versions:
    1.0, 1.1
    2.0, 2.1, 2.2
- But we don't support 1.x blocking hooks, i.e. the following calls are
  not supported:
    WSASetBlockingHook
    WSAUnhookBlockingHook
    WSACancelBlockingCall
    WSAIsBlocking
- Following calls from Winsock 2 are NOT supported:
    WSAAsyncGetHostByAddr
    WSAAsyncGetHostByName
    WSAAsyncGetProtoByName
    WSAAsyncGetProtoByNumber
    WSAAsyncGetServByName
    WSAAsyncGetServByPort
    WSAAsyncSelect
    WSACancelAsyncRequest
    WSADuplicateSocket
    WSAGetQOSByName
    WSAJoinLeaf
    WSAProviderConfigChange
    WSARecvDisconnect
    WSASendDisconnect
    WSAHtonl
    WSAHtons
    WSANtohl
    WSANtohs
    WSAAddressToString
    WSAStringToAddress
    WSASocket
    WSAAccept
    WSAConnect
    WSAIoctl
    WSAEventSelect
    WSAEnumProtocols
    WSAEnumNetworkEvents
    WSALookupServiceBegin
    WSALookupServiceNext
    WSALookupServiceEnd
    WSAInstallServiceClass
    WSARemoveServiceClass
    WSAGetServiceClassInfo
    WSAEnumNameSpaceProviders
    WSAGetServiceClassNameByClassId
    WSASetService
- closesocket always return immediately. If SO_LINGER is on and
  timeout is non-zero, we use the graceful close semantics (i.e.
  as if SO_DONTLINGER is on).
- For send calls, we don't support MSG_OOB and MSG_PARTIAL flags
  and ignore MSG_DONTROUTE.
- For recv calls, we don't support MSG_OOB and MSB_PARTIAL flags.
  MSG_PEEK flag is supported for datagram sockets.
- For WSARecv calls, we do not support more than one buffers.
- We do not support more than one outstanding overlapped I/O requests.
- We don't allow multiple threads to use the same socket handle simultanenously.
  If two threads try to use the same socket handle at the same time, the first
  one succeeds and the second one fails with a busy error.

=====================
 Directory Structure
=====================

inc     common include files
phy     PHY (Ethernet transceiver) code in ROM
lib     common library functions
enet    Ethernet interface driver
modem   modem driver
ppp     PPP interface driver
ip      IP/ICMP/IGMP code
tcp     TCP/UDP code
dhcp    DHCP client code
dns     DNS client code
winsock Winsock code, and everytying is built into XNET.DLL 
http    lightweight HTTP client library
test    test programs

======
Issues
======

- need to pass TCP related ICMP messages from IP layer to TCP layer
- path MTU discovery
- dead gateway detection
- implement TCP urgent data?
- support source route IP options for TCP connections?
- do receive work in a separate system thread
  instead of at DISPATCH_LEVEL?


=======================================
 Winsock API Parameter Validation RIPs
=======================================

bind:
    name != NULL
    namelen >= sizeof(struct sockaddr)
    name->sin_family == AF_INET

connect:
    same as bind

listen:
    socket must be a TCP socket

accept:
    socket must be a TCP socket
    either addr == NULL, or
      addrlen != NULL and addrlen >= sizeof(struct sockaddr)

getsockname:
    name != NULL
    namelen != NULL
    *namelen >= sizeof(struct sockaddr)

getpeername:
    same as getsockname

gethostbyaddr:
    addr != NULL
    len >= sizeof(struct in_addr)
    type == AF_INET

gethostbyname:
    name != NULL


getservbyname:
    name != NULL

getprotobyname:
    name != NULL

GetBestIpAddress
    addr != NULL

recv:
    either len > 0 and buf != NULL, or
      len == 0

WSARecv:
    dwBufferCount == 1
    lpBuffers != NULL
    ether lpBuffers->len > 0 and lpBuffers->buf != NULL, or
      lpBuffers->len == 0
    lpNumberOfBytesRecvd != NULL
    lpFlags != NULL
    lpCompletionRoutine == NULL

recvfrom:
    either len > 0 and buf != NULL, or
      len == 0
    either from == NULL, or
      fromlen != NULL and *fromlen >= sizeof(struct sockaddr)

WSARecvFrom:
    same as WSARecv, and
    either from == NULL, or
      fromlen != NULL and *fromlen >= sizeof(struct sockaddr)

WSAGetOverlappedResult:
    lpOverlapped != NULL
    lpcbTransfer != NULL
    lpdwFlags != NULL

send:
    either len > 0 and buf != NULL, or
      len == 0
    flags == 0

WSASend:
    dwBufferCount > 0
    lpBuffers != NULL
    for each WSABUF
      either len > 0 and buf != NULL, or
        len == 0
    lpNumberOfBytesSent != NULL
    dwFlags == 0
    lpCompletionRoutine == NULL

sendto:
    either len > 0 and buf != NULL, or
      len == 0
    flags == 0
    either to == NULL, or
      tolen >= sizeof(struct sockaddr)

WSASendTo:
    same as WSASend, and
    either lpTo == NULL, or
      iToLen >= sizeof(struct sockaddr)

shutdown:
    how must be one of the following values:
    SD_RECEIVE
    SD_SEND
    SD_BOTH

WSAStartup:
    lpWSAData != NULL

inet_addr:
    cp != NULL

setsockopt:
    optval != NULL
    optlen > 0
    additional requirements for specific options:
    SO_LINGER:
      optlen >= sizeof(LINGER)
    SO_ERROR:
    SO_RCVTIMEO:
    SO_SNDTIMEO:
    SO_RCVBUF:
    SO_SNDBUF:
      optlen >= sizeof(INT)
    SO_MULTICASTIF:
      optlen >= sizeof(struct in_addr)
    SO_ADD_MEMBERSHIP:
    SO_DROP_MEMBERSHIP:
      optlen >= sizeof(struct ip_mreq)

getsockopt:
    optval != NULL
    optlen != NULL
    *optlen > 0
    additional requirements for specific options:
      same as setsockopt

ioctlsocket:
    argp != NULL

