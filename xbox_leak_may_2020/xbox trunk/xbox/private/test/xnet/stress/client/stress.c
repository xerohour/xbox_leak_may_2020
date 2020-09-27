/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright Microsoft Corporation

Module Name:

  stress.c

Abstract:

  This module tests XNet stress - library

Author:

  Steven Kehrli (steveke) 17-Apr-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetStressNamespace;

namespace XNetStressNamespace {

#define LARGE_TCP_BUFFER            5672
#define LARGE_UDP_BUFFER            1430
#define LARGE_BUFFER                __max(LARGE_TCP_BUFFER, LARGE_UDP_BUFFER)

int     nTCPBufferSize[3]           = {10, LARGE_TCP_BUFFER, 0};
int     nUDPBufferSize[3]           = {10, LARGE_UDP_BUFFER, 0};
#define nTCPBufferSizes             sizeof(nTCPBufferSize) / sizeof(nTCPBufferSize[0])
#define nUDPBufferSizes             sizeof(nUDPBufferSize) / sizeof(nUDPBufferSize[0])

#define CONNECT_FUNC                1
#define ACCEPT_FUNC                 2
#define SEND_RECV_FUNC              3
#define WSASEND_WSARECV_FUNC        4
#define SENDTO_RECVFROM_FUNC        5
#define WSASENDTO_WSARECVFROM_FUNC  6

#define OVERLAPPED_NONE             0
#define OVERLAPPED_WAIT             1
#define OVERLAPPED_RESULT           2
#define OVERLAPPED_CANCEL           3

#define STATE_ANY_DATA              0
#define STATE_SEND_DATA             1
#define STATE_RECV_DATA             2



typedef struct _STRESS_THREAD {
    u_short  uIndex;
    HANDLE   hExitEvent;
    int      socket_type;
    BOOL     bNonblocking;
    in_addr  sin_addr;
    u_short  sin_port;
    BOOL     bListen;
    DWORD    dwFunction;
    BOOL     bOverlapped;
    DWORD    dwOverlapped;
} STRESS_THREAD, *PSTRESS_THREAD;

static STRESS_THREAD StressThreads1[] =
{
    {   1, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  ACCEPT_FUNC,                FALSE, OVERLAPPED_NONE   },
    {   2, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    {   3, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    {   4, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    {   5, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    {   6, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    {   7, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    {   8, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    {   9, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    {  10, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    {  11, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL },
    {  12, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  ACCEPT_FUNC,                FALSE, OVERLAPPED_NONE   },
    {  13, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    {  14, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    {  15, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    {  16, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    {  17, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    {  18, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    {  19, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    {  20, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    {  21, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    {  22, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL },
    {  23, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, CONNECT_FUNC,               FALSE, OVERLAPPED_NONE   },
    {  24, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    {  25, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    {  26, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    {  27, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    {  28, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    {  29, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    {  30, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    {  31, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    {  32, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    {  33, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL },
    {  34, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, CONNECT_FUNC,               FALSE, OVERLAPPED_NONE   },
    {  35, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    {  36, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    {  37, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    {  38, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    {  39, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    {  40, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    {  41, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    {  42, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    {  43, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    {  44, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL },
    {  45, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    {  46, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    {  47, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    {  48, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    {  49, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    {  50, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    {  51, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    {  52, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    {  53, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    {  54, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL }
};

static STRESS_THREAD StressThreads2[] =
{
    { 101, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, CONNECT_FUNC,               FALSE, OVERLAPPED_NONE   },
    { 102, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    { 103, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 104, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    { 105, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    { 106, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    { 107, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 108, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    { 109, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    { 110, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    { 111, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL },
    { 112, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, CONNECT_FUNC,               FALSE, OVERLAPPED_NONE   },
    { 113, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    { 114, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 115, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    { 116, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    { 117, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    { 118, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 119, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    { 120, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    { 121, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    { 122, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL },
    { 123, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  ACCEPT_FUNC,                FALSE, OVERLAPPED_NONE   },
    { 124, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    { 125, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 126, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    { 127, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    { 128, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    { 129, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 130, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    { 131, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    { 132, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    { 133, NULL, SOCK_STREAM, FALSE, { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL },
    { 134, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  ACCEPT_FUNC,                FALSE, OVERLAPPED_NONE   },
    { 135, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    { 136, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 137, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    { 138, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    { 139, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    { 140, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 141, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    { 142, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    { 143, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    { 144, NULL, SOCK_STREAM, TRUE,  { 0, 0, 0, 0}, 0, TRUE,  WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL },
    { 145, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, SEND_RECV_FUNC,             FALSE, OVERLAPPED_NONE   },
    { 146, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 147, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_WAIT   },
    { 148, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_RESULT },
    { 149, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASEND_WSARECV_FUNC,       TRUE,  OVERLAPPED_CANCEL },
    { 150, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, SENDTO_RECVFROM_FUNC,       FALSE, OVERLAPPED_NONE   },
    { 151, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, FALSE, OVERLAPPED_NONE   },
    { 152, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_WAIT   },
    { 153, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_RESULT },
    { 154, NULL, SOCK_DGRAM,  FALSE, { 0, 0, 0, 0}, 0, FALSE, WSASENDTO_WSARECVFROM_FUNC, TRUE,  OVERLAPPED_CANCEL }
};

#define StressThreadCount1      (sizeof(StressThreads1) / sizeof(STRESS_THREAD))
#define StressThreadCount2      (sizeof(StressThreads2) / sizeof(STRESS_THREAD))
#define StressThreadCountEqual  (StressThreadCount1 == StressThreadCount2)
#define StressThreadCount       __max(StressThreadCount1, StressThreadCount2)

NETSYNC_TYPE_CALLBACK  XNetStressSessionNt =
{
    1,
    NUM_STRESS_CLIENTS,
    StressThreadCount,
    L"xnetstress_nt.dll",
    "XNetStressMain",
    "XNetStressStart",
    "XNetStressStop"
};

NETSYNC_TYPE_CALLBACK  XNetStressSessionXbox =
{
    1,
    NUM_STRESS_CLIENTS,
    StressThreadCount,
    L"xnetstress_xbox.dll",
    "XNetStressMain",
    "XNetStressStart",
    "XNetStressStop"
};



SOCKET
CreateSocket(
    IN PSTRESS_THREAD  pStressThread
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates and binds a socket

Arguments:

  pStressThread - Pointer to the stress thread object

Return Value:

  SOCKET:
    If the function succeeds, the return value is a socket descriptor
    If the function fails, the return value is INVALID_SOCKET.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // sSocket is the socket descriptor
    SOCKET       sSocket;

    // dwSendBufferSize and dwRecvBufferSize are the size of the send and receive buffers
    DWORD        dwSendBufferSize;
    DWORD        dwRecvBufferSize;

    // dwSendTimeout and dwRecvTimeout are the send and receive timeouts
    DWORD        dwSendTimeout;
    DWORD        dwRecvTimeout;

    // bNagle indicates if Nagle is on (TRUE) or off (FALSE)
    BOOL         bNagle;

    // localname is the local host name
    SOCKADDR_IN  localname;



    // Seed the pseudorandom-number generator
    srand(GetTickCount());

    // Create the socket
    sSocket = INVALID_SOCKET;
    sSocket = socket(AF_INET, pStressThread->socket_type, 0);

    // Set the blocking mode of the socket
    ioctlsocket(sSocket, FIONBIO, (u_long *) &pStressThread->bNonblocking);

    // Set the size of the send and receive buffers
    dwSendBufferSize = (rand() % 0x2000) + 1;  // Range is 1 to 8192 (bytes)
    dwRecvBufferSize = (rand() % 0x2000) + 1;  // Range is 1 to 8192 (bytes)
    setsockopt(sSocket, SOL_SOCKET, SO_SNDBUF, (char *) &dwSendBufferSize, sizeof(dwSendBufferSize));
    setsockopt(sSocket, SOL_SOCKET, SO_RCVBUF, (char *) &dwRecvBufferSize, sizeof(dwRecvBufferSize));

    // Set the send and receive timeout
    dwSendTimeout = (rand() % 59) + 1;         // Range is 1 to 60 (seconds)
    dwRecvTimeout = (rand() % 59) + 1;         // Range is 1 to 60 (seconds)
    setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &dwSendTimeout, sizeof(dwSendTimeout));
    setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &dwRecvTimeout, sizeof(dwRecvTimeout));

    // Enable or disable Nagle
    bNagle = rand() % 1;                       // Range is 0 or 1
    setsockopt(sSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &bNagle, sizeof(bNagle));

    // Bind the socket
    ZeroMemory(&localname, sizeof(localname));
    localname.sin_family = AF_INET;
    localname.sin_port = htons(pStressThread->sin_port);
    bind(sSocket, (SOCKADDR *) &localname, sizeof(localname));

    return sSocket;
}



DWORD
ConnectThread(
    IN LPVOID  lpv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to handle the connect logic

Arguments:

  lpv - Pointer to the STRESS_THREAD

------------------------------------------------------------------------------*/
{
    // pStressThread is a pointer to the STRESS_THREAD object
    PSTRESS_THREAD  pStressThread = (PSTRESS_THREAD) lpv;

    // sSocket is the socket descriptor
    SOCKET          sSocket;

    // remotename is the remote host name
    SOCKADDR_IN     remotename;

    // writefds is the set of sockets for write
    fd_set          writefds;
    // exceptfds is the set of sockets for write
    fd_set          exceptfds;
    // select_timeout is the timeout for select
    timeval         select_timeout;
    // iResult is the result of the operation
    int             iResult;



    do {
        // Create the socket
        sSocket = CreateSocket(pStressThread);;

        // Initialize the remote name
        ZeroMemory(&remotename, sizeof(remotename));
        remotename.sin_family = AF_INET;
        remotename.sin_addr = pStressThread->sin_addr;
        remotename.sin_port = htons(pStressThread->sin_port);

        // Connect the socket
        iResult = connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

        if (TRUE == pStressThread->bNonblocking) {
            do {
                // Wait for the connection
                FD_ZERO(&writefds);
                FD_ZERO(&exceptfds);
                FD_SET(sSocket, &writefds);
                FD_SET(sSocket, &exceptfds);

                select_timeout.tv_sec = 60;
                select_timeout.tv_usec = 0;

                select(0, NULL, &writefds, &exceptfds, &select_timeout);
            } while ((0 == FD_ISSET(sSocket, &exceptfds)) && (0 == FD_ISSET(sSocket, &writefds)) && (WAIT_OBJECT_0 != WaitForSingleObject(pStressThread->hExitEvent, 0)));
        }

        // Close the socket
        if (INVALID_SOCKET != sSocket) {
            shutdown(sSocket, SD_BOTH);
            closesocket(sSocket);
        }
    } while (WAIT_OBJECT_0 != WaitForSingleObject(pStressThread->hExitEvent, 0));

    return 0;
}



DWORD
AcceptThread(
    IN LPVOID  lpv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to handle the accept logic

Arguments:

  lpv - Pointer to the STRESS_THREAD

------------------------------------------------------------------------------*/
{
    // pStressThread is a pointer to the STRESS_THREAD object
    PSTRESS_THREAD  pStressThread = (PSTRESS_THREAD) lpv;

    // sSocket and nsSocket are the socket descriptors
    SOCKET          sSocket;
    SOCKET          nsSocket;

    // readfds is the set of sockets for read
    fd_set          readfds;
    // select_timeout is the timeout for select
    timeval         select_timeout;
    // iResult is the result of the operation
    int             iResult;



    // Create the socket
    sSocket = CreateSocket(pStressThread);

    // Place the socket in listening mode
    listen(sSocket, SOMAXCONN);

    do {
        do {
            // Wait for a connection
            FD_ZERO(&readfds);
            FD_SET(sSocket, &readfds);

            select_timeout.tv_sec = 60;
            select_timeout.tv_usec = 0;

            select(0, &readfds, NULL, NULL, &select_timeout);
        } while ((0 == FD_ISSET(sSocket, &readfds)) && (WAIT_OBJECT_0 != WaitForSingleObject(pStressThread->hExitEvent, 0)));

        if (WAIT_OBJECT_0 == WaitForSingleObject(pStressThread->hExitEvent, 0)) {
            goto AcceptLoopEnd;
        }

        // Accept the connection
        nsSocket = INVALID_SOCKET;
        nsSocket = accept(sSocket, NULL, NULL);

AcceptLoopEnd:
        // Close the socket
        if (INVALID_SOCKET != nsSocket) {
            shutdown(nsSocket, SD_BOTH);
            closesocket(nsSocket);
        }
    } while (WAIT_OBJECT_0 != WaitForSingleObject(pStressThread->hExitEvent, 0));

    // Close the socket
    closesocket(sSocket);

    return 0;
}



DWORD
SendReceiveThread(
    IN LPVOID  lpv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to handle the send/receive logic

Arguments:

  lpv - Pointer to the STRESS_THREAD

------------------------------------------------------------------------------*/
{
    // pStressThread is a pointer to the STRESS_THREAD object
    PSTRESS_THREAD  pStressThread = (PSTRESS_THREAD) lpv;

    // sSocket and nsSocket are the socket descriptors
    SOCKET          sSocket = INVALID_SOCKET;
    SOCKET          nsSocket = INVALID_SOCKET;

    // StressSendBuffer is the send buffer
    char            StressSendBuffer[LARGE_BUFFER];
    // StressReceiveBuffer is the receive buffer
    char            StressReceiveBuffer[LARGE_BUFFER];

    // remotename is the remote host name
    SOCKADDR_IN     remotename;
    // fromname is the remote sender name
    SOCKADDR_IN     fromname;
    // fromnamelen is the size of the remote send name
    int             fromnamelen;

    // readfds is the set of sockets for read
    fd_set          readfds;
    // writefds is the set of sockets for write
    fd_set          writefds;
    // exceptfds is the set of sockets for write
    fd_set          exceptfds;
    // select_timeout is the timeout for select
    timeval         select_timeout;
    // iResult is the result of the operation
    int             iResult;

    // dwStartTime is the start time of the send/receive loop
    DWORD           dwStartTime;
    // dwRunTime is the run time of the send/receive loop
    DWORD           dwRunTime;

    // nStressBufferSize is the stress buffer size
    int             *nStressBufferSize;
    // nStressBufferSizes is the number of stress buffer sizes
    int             nStressBufferSizes;
    // dwBufferIndex is a counter to enumerate the stress buffer sizes
    DWORD           dwBufferIndex;
    // dwIndex is a counter
    DWORD           dwIndex;

    // WSASendBuf is the send buffer
    WSABUF          WSASendBuf;
    // WSARecvBuf is the receive buffer
    WSABUF          WSARecvBuf;
    // dwBytesSent is the number of bytes sent
    DWORD           dwBytesSent;
    // dwBytesReceived is the number of bytes received
    DWORD           dwBytesReceived;
    // WSASendOverlapped is the send overlapped i/o structure
    WSAOVERLAPPED   WSASendOverlapped;
    // WSARecvOverlapped is the receive overlapped i/o structure
    WSAOVERLAPPED   WSARecvOverlapped;
    // hSendEvent is a handle to the send overlapped i/o event
    HANDLE          hSendEvent;
    // hRecvEvent is a handle to the receive overlapped i/o event
    HANDLE          hRecvEvent;
    // dwFlags is the flags of the send/receive operation
    DWORD           dwFlags;
    // dwBytes is the number of bytes in the buffer
    DWORD           dwBytes;



    // Seed the pseudorandom-number generator
    srand(GetTickCount());

    // Initialize the stress buffer
    for (dwIndex = 0; dwIndex < sizeof(StressSendBuffer); dwIndex++) {
        StressSendBuffer[dwIndex] = (char) (dwIndex % 256);
    }

    // Initialize the stress buffer sizes
    if (SOCK_STREAM == pStressThread->socket_type) {
        nStressBufferSize = nTCPBufferSize;
        nStressBufferSizes = nTCPBufferSizes;
    }
    else {
        nStressBufferSize = nUDPBufferSize;
        nStressBufferSizes = nUDPBufferSizes;
    }

    // Initialize the overlapped i/o structures
    if (TRUE == pStressThread->bOverlapped) {
        hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        hRecvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        ZeroMemory(&WSASendOverlapped, sizeof(WSASendOverlapped));
        ZeroMemory(&WSARecvOverlapped, sizeof(WSARecvOverlapped));

        WSASendOverlapped.hEvent = hSendEvent;
        WSARecvOverlapped.hEvent = hRecvEvent;
    }
    else {
        hSendEvent = NULL;
        hRecvEvent = NULL;
    }

    do {
        if (INVALID_SOCKET == sSocket) {
            // Create the socket
            sSocket = CreateSocket(pStressThread);

            // Place the socket in listening mode
            if (TRUE == pStressThread->bListen) {
                listen(sSocket, SOMAXCONN);
            }
        }

        // Initialize the remote name
        ZeroMemory(&remotename, sizeof(remotename));
        remotename.sin_family = AF_INET;
        remotename.sin_addr = pStressThread->sin_addr;
        remotename.sin_port = htons(pStressThread->sin_port);

        if ((SOCK_STREAM == pStressThread->socket_type) && (TRUE == pStressThread->bListen)) {
            do {
                // Wait for a connection
                FD_ZERO(&readfds);
                FD_SET(sSocket, &readfds);

                select_timeout.tv_sec = 60;
                select_timeout.tv_usec = 0;

                select(0, &readfds, NULL, NULL, &select_timeout);
            } while ((0 == FD_ISSET(sSocket, &readfds)) && (WAIT_OBJECT_0 != WaitForSingleObject(pStressThread->hExitEvent, 0)));

            if (WAIT_OBJECT_0 == WaitForSingleObject(pStressThread->hExitEvent, 0)) {
                goto SendReceiveLoopEnd;
            }

            // Accept the connection
            nsSocket = accept(sSocket, NULL, NULL);
        }
        else if ((SOCK_STREAM == pStressThread->socket_type) || (SEND_RECV_FUNC == pStressThread->dwFunction) || (WSASEND_WSARECV_FUNC == pStressThread->dwFunction)) {
            // Connect the socket
            iResult = connect(sSocket, (SOCKADDR *) &remotename, sizeof(remotename));

            if ((SOCK_STREAM == pStressThread->socket_type) && (TRUE == pStressThread->bNonblocking)) {
                do {
                    // Wait for the connection
                    FD_ZERO(&writefds);
                    FD_ZERO(&exceptfds);
                    FD_SET(sSocket, &writefds);
                    FD_SET(sSocket, &exceptfds);

                    select_timeout.tv_sec = 60;
                    select_timeout.tv_usec = 0;

                    select(0, NULL, &writefds, &exceptfds, &select_timeout);
                } while ((0 == FD_ISSET(sSocket, &exceptfds)) && (0 == FD_ISSET(sSocket, &writefds)) && (WAIT_OBJECT_0 != WaitForSingleObject(pStressThread->hExitEvent, 0)));

                if (1 == FD_ISSET(sSocket, &exceptfds)) {
                    iResult = SOCKET_ERROR;
                }
                else {
                    iResult = 0;
                }
            }

            if ((WAIT_OBJECT_0 == WaitForSingleObject(pStressThread->hExitEvent, 0)) || (SOCKET_ERROR == iResult)) {
                goto SendReceiveLoopEnd;
            }
        }

        // Initialize the buffer index
        dwBufferIndex = (rand() % nStressBufferSizes);

        // Get the run time
        dwRunTime = (rand() % 6) + 1;          // Range is 1 to 6 (5 minute intervals)
        dwRunTime = dwRunTime * 5;             // Range is 5, 10, 15, 20, 25, or 30 (minutes)
        dwRunTime = dwRunTime * 60 * 1000;     // Range is 300000, 600000, 900000, 1200000, 1500000, 1800000 (milli-seconds)

        // Get the start time
        dwStartTime = GetTickCount();

        do {
            if (WAIT_OBJECT_0 == WaitForSingleObject(pStressThread->hExitEvent, 0)) {
                goto SendReceiveLoopEnd;
            }

            // Check for the write
            FD_ZERO(&writefds);
            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &writefds);

            select_timeout.tv_sec = 0;
            select_timeout.tv_usec = 0;

            select(0, NULL, &writefds, NULL, &select_timeout);

            // Send
            if (SEND_RECV_FUNC == pStressThread->dwFunction) {
                iResult = send((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, StressSendBuffer, nStressBufferSize[dwBufferIndex], 0);
            }
            else if (WSASEND_WSARECV_FUNC == pStressThread->dwFunction) {
                WSASendBuf.buf = StressSendBuffer;
                WSASendBuf.len = nStressBufferSize[dwBufferIndex];
                iResult = WSASend((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSASendBuf, 1, &dwBytesSent, 0, (TRUE == pStressThread->bOverlapped) ? &WSASendOverlapped : NULL, NULL);
            }
            else if (SENDTO_RECVFROM_FUNC == pStressThread->dwFunction) {
                iResult = sendto((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, StressSendBuffer, nStressBufferSize[dwBufferIndex], 0, (SOCKADDR *) &remotename, sizeof(remotename));
            }
            else {
                WSASendBuf.buf = StressSendBuffer;
                WSASendBuf.len = nStressBufferSize[dwBufferIndex];
                iResult = WSASendTo((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSASendBuf, 1, &dwBytesSent, 0, (SOCKADDR *) &remotename, sizeof(remotename), (TRUE == pStressThread->bOverlapped) ? &WSASendOverlapped : NULL, NULL);
            }

            if ((SOCKET_ERROR == iResult) && (WSAECONNRESET == WSAGetLastError())) {
                goto SendReceiveLoopEnd;
            }

            // Overlapped I/O
            if (TRUE == pStressThread->bOverlapped) {
                if (OVERLAPPED_WAIT == pStressThread->dwOverlapped) {
                    // Wait for event
                    WaitForSingleObject(WSASendOverlapped.hEvent, 60 * 1000);
                }
                else if (OVERLAPPED_RESULT == pStressThread->dwOverlapped) {
                    for (dwIndex = 0; dwIndex < 60; dwIndex++) {
                        dwFlags = 0;
                        if (TRUE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSASendOverlapped, &dwBytesSent, FALSE, &dwFlags)) {
                            break;
                        }

                        if (WAIT_OBJECT_0 == WaitForSingleObject(pStressThread->hExitEvent, 0)) {
                            break;
                        }

                        Sleep(1000);
                    }
                }
                else {
                    // Cancel overlapped i/o
                    WSACancelOverlappedIO((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket);
                }
            }

            if (WAIT_OBJECT_0 == WaitForSingleObject(pStressThread->hExitEvent, 0)) {
                goto SendReceiveLoopEnd;
            }

            // Check for the read
            FD_ZERO(&readfds);
            FD_SET((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &readfds);

            select_timeout.tv_sec = 0;
            select_timeout.tv_usec = 0;

            select(0, &readfds, NULL, NULL, &select_timeout);

            // Receive
            if (SEND_RECV_FUNC == pStressThread->dwFunction) {
                iResult = recv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, StressReceiveBuffer, nStressBufferSize[dwBufferIndex], 0);
            }
            else if (WSASEND_WSARECV_FUNC == pStressThread->dwFunction) {
                WSARecvBuf.buf = StressReceiveBuffer;
                WSARecvBuf.len = nStressBufferSize[dwBufferIndex];
                dwFlags = 0;
                iResult = WSARecv((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSARecvBuf, 1, &dwBytesReceived, &dwFlags, (TRUE == pStressThread->bOverlapped) ? &WSARecvOverlapped : NULL, NULL);
            }
            else if (SENDTO_RECVFROM_FUNC == pStressThread->dwFunction) {
                fromnamelen = sizeof(fromname);
                iResult = recvfrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, StressReceiveBuffer, nStressBufferSize[dwBufferIndex], 0, (SOCKADDR *) &fromname, &fromnamelen);
            }
            else {
                WSARecvBuf.buf = StressReceiveBuffer;
                WSARecvBuf.len = nStressBufferSize[dwBufferIndex];
                dwFlags = 0;
                fromnamelen = sizeof(fromname);
                iResult = WSARecvFrom((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSARecvBuf, 1, &dwBytesReceived, &dwFlags, (SOCKADDR *) &fromname, &fromnamelen, (TRUE == pStressThread->bOverlapped) ? &WSARecvOverlapped : NULL, NULL);
            }

            if ((SOCKET_ERROR == iResult) && (WSAECONNRESET == WSAGetLastError())) {
                goto SendReceiveLoopEnd;
            }

            // Overlapped I/O
            if (TRUE == pStressThread->bOverlapped) {
                if (OVERLAPPED_WAIT == pStressThread->dwOverlapped) {
                    // Wait for event
                    WaitForSingleObject(WSARecvOverlapped.hEvent, 60 * 1000);
                }
                else if (OVERLAPPED_RESULT == pStressThread->dwOverlapped) {
                    for (dwIndex = 0; dwIndex < 60; dwIndex++) {
                        dwFlags = 0;
                        if (TRUE == WSAGetOverlappedResult((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, &WSARecvOverlapped, &dwBytesReceived, FALSE, &dwFlags)) {
                            break;
                        }
                        
                        if (WAIT_OBJECT_0 == WaitForSingleObject(pStressThread->hExitEvent, 0)) {
                            break;
                        }

                        Sleep(1000);
                    }
                }
                else {
                    // Cancel overlapped i/o
                    WSACancelOverlappedIO((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket);
                }
            }

            // Check the amount of read data
            ioctlsocket((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, FIONREAD, &dwBytes);

            // Increment the buffer index
            dwBufferIndex = (dwBufferIndex + 1) % nStressBufferSizes;

            // Surrender quantum
            Sleep(0);
        } while ((dwRunTime > (GetTickCount() - dwStartTime)) && (WAIT_OBJECT_0 != WaitForSingleObject(pStressThread->hExitEvent, 0)));

SendReceiveLoopEnd:
        // Shutdown the socket
        if (SOCK_STREAM == pStressThread->socket_type) {
            shutdown((INVALID_SOCKET == nsSocket) ? sSocket : nsSocket, SD_BOTH);
        }

        // Close the sockets
        if (INVALID_SOCKET != nsSocket) {
            closesocket(nsSocket);
            nsSocket = INVALID_SOCKET;
        }

        if (FALSE == pStressThread->bListen) {
            closesocket(sSocket);
            sSocket = INVALID_SOCKET;
        }
    } while (WAIT_OBJECT_0 != WaitForSingleObject(pStressThread->hExitEvent, 0));

    // Close the socket
    if (INVALID_SOCKET != sSocket) {
        closesocket(sSocket);
    }

    // Close send/receive overlapped i/o events
    if (NULL != hRecvEvent) {
        CloseHandle(hRecvEvent);
    }

    if (NULL != hSendEvent) {
        CloseHandle(hSendEvent);
    }

    return 0;
}



VOID
StressTest(
    IN HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Test entry point

Arguments:

  hLog - handle to the xLog log object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is the mem object
    HANDLE                  hMemObject = INVALID_HANDLE_VALUE;

    // bXnetInitialized indicates if Xnet is initialized
    BOOL                    bXnetInitialized = FALSE;
    // WSAData is the Winsock init data
    WSADATA                 WSAData;
    // bWinsockInitialized indicated is Winsock is initialized
    BOOL                    bWinsockInitialized = FALSE;

    // hNetsyncObject is a handle to the netsync object
    HANDLE                  hNetsyncObject = INVALID_HANDLE_VALUE;
    // NetsyncTypeSession is the session type descriptor
    NETSYNC_TYPE_CALLBACK   NetsyncTypeSession;
    // bSecure indicates if remote netsync server is secure or insecure
    BOOL                    bSecure = FALSE;
    // lpszNetsyncRemote is a pointer to the remote netsync server
    LPSTR                   lpszNetsyncRemote = NULL;
    // lpszRemote is a pointer to the remote netsync server type
    LPSTR                   lpszRemote = NULL;
    // NetsyncInAddr is the netsync server address
    u_long                  NetsyncInAddr = 0;
    // LowPort is the low bound of the netsync port range
    u_short                 LowPort = 0;
    // HighPort is the high bound of the netsync port range
    u_short                 HighPort = 0;
    // FromInAddr is the address of the netsync sender
    u_long                  FromInAddr;
    // dwMessageType is the type of received message
    DWORD                   dwMessageType;
    // dwMessageSize is the size of the received message
    DWORD                   dwMessageSize;
    // pMessage is a pointer to the received message
    char                    *pMessage;
    // StressSessionMessage is a message sent to/received from the stress session
    STRESS_SESSION_MESSAGE  StressSessionMessage;

    // sin_addr is the in_addr structure
    IN_ADDR                 sin_addr;
    // RemoteInAddr is the address of the remote host
    u_long                  RemoteInAddr;
    // RemoteXnAddr is the xnet address of the remote host
    XNADDR                  RemoteXnAddr;
    // XnKid is the xnet key id
    XNKID                   XnKid;
    // XnKid is the xnet key
    XNKEY                   XnKey;
    // bKeyPairOwner indicates this client owns the key-pair
    BOOL                    bKeyPairOwner;

    // hExitEvent is a handle to the exit event
    HANDLE                  hExitEvent = NULL;
    // hThreads is an array of the stress thread handles
    HANDLE                  hThreads[StressThreadCount];
    // uIndex is a counter to enumerate the stress threads
    u_short                 uIndex;



    // Set the xLog info
    xSetComponent(hLog, "Network", "XNet");
    xSetFunctionName(hLog, "Stress");
    xStartVariation(hLog, "Stress Test");

    if (FALSE == StressThreadCountEqual) {
        xLog(hLog, XLL_FAIL, "Stress thread count is not equal");

        goto FunctionExit;
    }

    // Create the private heap
    hMemObject = xMemCreate();
    if (INVALID_HANDLE_VALUE == hMemObject) {
        xLog(hLog, XLL_FAIL, "xMemCreate failed - ec: %u", GetLastError());

        goto FunctionExit;
    }

    // Get the remote info
    bSecure = (BOOL) GetProfileIntA("xnetstress", "bSecure", 0);
    if (FALSE == bSecure) {
        lpszNetsyncRemote = GetIniString(hMemObject, "xnetstress", "Secure");
    }

    lpszRemote = GetIniString(hMemObject, "xnetstress", "Remote");
    if ((NULL != lpszRemote) && (0 == strcmp("xbox", lpszRemote))) {
        NetsyncTypeSession = XNetStressSessionXbox;
    }
    else {
        NetsyncTypeSession = XNetStressSessionNt;
    }

    // Create the exit event
    hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == hExitEvent) {
        xLog(hLog, XLL_FAIL, "CreateEvent failed - ec: %u", GetLastError());

        goto FunctionExit;
    }

    while (TRUE) {
        // Initialize Xnet
        if (0 == XNetAddRef()) {
            break;
        }
        bXnetInitialized = TRUE;

        // Initialize Winsock
        if (0 != WSAStartup(MAKEWORD(2, 2), &WSAData)) {
            break;
        }
        bWinsockInitialized = TRUE;

        // Initialize the key-pair
        ZeroMemory(&XnKid, sizeof(XnKid));
        ZeroMemory(&XnKey, sizeof(XnKey));
        bKeyPairOwner = FALSE;

        // Connect to the stress session
        hNetsyncObject = NetsyncCreateClient((NULL != lpszNetsyncRemote) ? inet_addr(lpszNetsyncRemote) : 0, NETSYNC_SESSION_CALLBACK, &XNetStressSessionNt, &NetsyncInAddr, NULL, &LowPort, &HighPort);
        if (INVALID_HANDLE_VALUE == hNetsyncObject) {
            xLog(hLog, XLL_FAIL, "Connect Netsync session failed - ec: 0x%08x", GetLastError());

            break;
        }

        while (WAIT_OBJECT_0 != WaitForSingleObject(hExitEvent, 0)) {
            // Send a keep-alive message
            ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
            StressSessionMessage.dwMessageId = CLIENT_KEEPALIVE_MSG;
            if (WAIT_OBJECT_0 != NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(StressSessionMessage), (char *) &StressSessionMessage)) {
                xLog(hLog, XLL_FAIL, "Send Keep-Alive failed - ec: 0x%08x", GetLastError());

                break;
            }

            // Check for messages
            if (WAIT_OBJECT_0 == NetsyncReceiveMessage(hNetsyncObject, 60000, &dwMessageType, &FromInAddr, NULL, &dwMessageSize, &pMessage)) {
                if (SERVER_KEYPAIR_MSG == ((PSTRESS_SESSION_MESSAGE) pMessage)->dwMessageId) {
                    if (FALSE == bKeyPairOwner) {
                        // Create and register the key pair
                        XNetCreateKey(&XnKid, &XnKey);
                        XNetRegisterKey(&XnKid, &XnKey);
                        bKeyPairOwner = TRUE;
                    }

                    // Send the key-pair message
                    ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                    StressSessionMessage.dwMessageId = CLIENT_KEYPAIR_MSG;
                    CopyMemory(&StressSessionMessage.XnKid, &XnKid, sizeof(StressSessionMessage.XnKid));
                    CopyMemory(&StressSessionMessage.XnKey, &XnKey, sizeof(StressSessionMessage.XnKey));

                    if (WAIT_OBJECT_0 != NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(StressSessionMessage), (char *) &StressSessionMessage)) {
                        xLog(hLog, XLL_FAIL, "Send Key-Pair failed - ec: 0x%08x", GetLastError());

                        break;
                    }
                }
                else if (SERVER_REGISTER_MSG == ((PSTRESS_SESSION_MESSAGE) pMessage)->dwMessageId) {
                    // Copy the key
                    CopyMemory(&XnKid, &((PSTRESS_SESSION_MESSAGE) pMessage)->XnKid, sizeof(XnKid));
                    CopyMemory(&XnKey, &((PSTRESS_SESSION_MESSAGE) pMessage)->XnKey, sizeof(XnKey));

                    // Register the key pair
                    XNetRegisterKey(&XnKid, &XnKey);

                    // Send the ready message
                    ZeroMemory(&StressSessionMessage, sizeof(StressSessionMessage));
                    StressSessionMessage.dwMessageId = CLIENT_REGISTER_MSG;
                    if (WAIT_OBJECT_0 != NetsyncSendClientMessage(hNetsyncObject, 0, sizeof(StressSessionMessage), (char *) &StressSessionMessage)) {
                        xLog(hLog, XLL_FAIL, "Send Ready failed - ec: 0x%08x", GetLastError());

                        break;
                    }
                }
                else if (SERVER_START_MSG == ((PSTRESS_SESSION_MESSAGE) pMessage)->dwMessageId) {
                    // Get the remote address
                    CopyMemory(&RemoteXnAddr, &((PSTRESS_SESSION_MESSAGE) pMessage)->XnAddr, sizeof(RemoteXnAddr));
                    XNetXnAddrToInAddr(&RemoteXnAddr, &XnKid, &sin_addr);
                    RemoteInAddr = sin_addr.s_addr;

                    // Initialize the stress threads
                    ZeroMemory(hThreads, sizeof(hThreads));
                    for (uIndex = 0; uIndex < StressThreadCount; uIndex++) {
                        if (STRESS_TYPE_1 == ((PSTRESS_SESSION_MESSAGE) pMessage)->dwStressType) {
                            StressThreads1[uIndex].hExitEvent = hExitEvent;
                            StressThreads1[uIndex].sin_addr.s_addr = RemoteInAddr;
                            StressThreads1[uIndex].sin_port = LowPort + uIndex;

                            if (CONNECT_FUNC == StressThreads1[uIndex].dwFunction) {
                                hThreads[uIndex] = CreateThread(NULL, 0, ConnectThread, &StressThreads1[uIndex], 0, NULL);
                            }
                            else if (ACCEPT_FUNC == StressThreads1[uIndex].dwFunction) {
                                hThreads[uIndex] = CreateThread(NULL, 0, AcceptThread, &StressThreads1[uIndex], 0, NULL);
                            }
                            else {
                                hThreads[uIndex] = CreateThread(NULL, 0, SendReceiveThread, &StressThreads1[uIndex], 0, NULL);
                            }
                        }
                        else {
                            StressThreads2[uIndex].hExitEvent = hExitEvent;
                            StressThreads2[uIndex].sin_addr.s_addr = RemoteInAddr;
                            StressThreads2[uIndex].sin_port = LowPort + uIndex;

                            if (CONNECT_FUNC == StressThreads2[uIndex].dwFunction) {
                                hThreads[uIndex] = CreateThread(NULL, 0, ConnectThread, &StressThreads2[uIndex], 0, NULL);
                            }
                            else if (ACCEPT_FUNC == StressThreads2[uIndex].dwFunction) {
                                hThreads[uIndex] = CreateThread(NULL, 0, AcceptThread, &StressThreads2[uIndex], 0, NULL);
                            }
                            else {
                                hThreads[uIndex] = CreateThread(NULL, 0, SendReceiveThread, &StressThreads2[uIndex], 0, NULL);
                            }
                        }
                    }
                }
                else if (SERVER_STOP_MSG == ((PSTRESS_SESSION_MESSAGE) pMessage)->dwMessageId) {
                    // Free the message
                    NetsyncFreeMessage(pMessage);

                    break;
                }

                // Free the message
                NetsyncFreeMessage(pMessage);
            }
        }

        // Signal the exit event
        SetEvent(hExitEvent);

        // Wait for the stress threads to exit
        WaitForMultipleObjects(StressThreadCount, hThreads, TRUE, INFINITE);

        // Reset the exit event
        ResetEvent(hExitEvent);

        // Unregister the key pair
        XNetUnregisterKey(&XnKid);

        // Close the netsync object
        NetsyncCloseClient(hNetsyncObject);
        hNetsyncObject = INVALID_HANDLE_VALUE;

        // Terminate Winsock
        WSACleanup();
        bWinsockInitialized = FALSE;

        // Terminate Xnet
        XNetRelease();
        bXnetInitialized = FALSE;
    };

FunctionExit:
    if (INVALID_HANDLE_VALUE != hNetsyncObject) {
        // Close the netsync object
        NetsyncCloseClient(hNetsyncObject);
        hNetsyncObject = INVALID_HANDLE_VALUE;
    }

    if (TRUE == bWinsockInitialized) {
        // Terminate Winsock
        WSACleanup();
        bWinsockInitialized = FALSE;
    }

    if (TRUE == bXnetInitialized) {
        // Terminate Xnet
        XNetRelease();
        bXnetInitialized = FALSE;
    }

    if (NULL != hExitEvent) {
        // Close the exit event
        CloseHandle(hExitEvent);
    }

    if (NULL != lpszRemote) {
        xMemFree(hMemObject, lpszRemote);
    }

    if (NULL != lpszNetsyncRemote) {
        xMemFree(hMemObject, lpszNetsyncRemote);
    }

    // Free the private heap
    xMemClose(hMemObject);

    xEndVariation(hLog);
}

} // namespace XNetStressNamespace
