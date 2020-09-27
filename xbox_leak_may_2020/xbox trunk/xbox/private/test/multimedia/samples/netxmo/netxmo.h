#ifndef __NETXMO_H__
#define __NETXMO_H__

#include <xtl.h>

HRESULT
XnetCreateMediaObject(
    XMediaObject **ppMediaObject,
    SOCKET Socket,
    const char * pAddress,
    DWORD dwPort,
    DWORD dwMaxPacketSize,
    DWORD dwFlags
    );

#define MAX_NETXMO_TRANSFER_SIZE 1500
#define MAX_NETXMO_BUFFER_COUNT 20

#endif
