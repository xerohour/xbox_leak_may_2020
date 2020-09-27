/***************************************************************************
 *
 *  Copyright (C) 1/8/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsremote.h
 *  Content:    Remote DirectSound API main header.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/8/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __DSREMOTE_H__
#define __DSREMOTE_H__

#ifdef _XBOX
#include <xtl.h>
#else // _XBOX
#include <windows.h>
#include <objbase.h>
#endif // _XBOX

//
// Forward declarations
//

typedef struct IRemoteConnection IRemoteConnection;
typedef IRemoteConnection *LPREMOTECONNECTION;

typedef struct IRemoteFileIo IRemoteFileIo;
typedef IRemoteFileIo *LPREMOTEFILEIO;

//
// Packet data buffer
//

typedef struct _DSRBUFFER
{
    DWORD       dwSize;             // Buffer size, in bytes
    LPVOID      pvData;             // Buffer data
} DSRBUFFER, *LPDSRBUFFER;

typedef const DSRBUFFER *LPCDSRBUFFER;

//
// Packet header
//

typedef struct _DSRPACKET
{
    DWORD       dwMajorType;        // Packet major type
    DWORD       dwMinorType;        // Packet minor type
    DWORD       dwFlags;            // Packet flags
} DSRPACKET, *LPDSRPACKET;

typedef const DSRPACKET *LPCDSRPACKET;

//
// Reserved major/minor type identifiers
//

#define DSRTYPE_SYSTEM              'sysr'
#define DSRTYPE_FILEIO              'elif'

//
// Default port assignments
//

#define DSREMOTE_DEFAULT_PORT       1818

//
// IRemoteConnection::Connect flags
//

#define DSREMOTE_CONNECT_DNSLOOKUP  0x00000001      // The supplied address is an Xbox name, not an IP address

//
// IRemoteConnection::Disconnect flags
//

#define DSREMOTE_DISCONNECT_LISTEN  0x00000001      // The connection object will listen for a new connection

//
// Optimal packet size
//

#define DSREMOTE_OPTIMAL_PACKET_SIZE 1024

//
// IRemoteConnection::GetState connection states
//

#define DSREMOTE_STATE_IDLE         0
#define DSREMOTE_STATE_LISTENING    1
#define DSREMOTE_STATE_CONNECTED    2

//
// DSREMOTECALLBACK (pseudo-interface vector table)
//

typedef struct _DSREMOTECALLBACK
{
    void (STDMETHODCALLTYPE *Connect)(LPCSTR pszRemoteAddress, LPVOID pvContext);
    void (STDMETHODCALLTYPE *Disconnect)(LPVOID pvContext);
} DSREMOTECALLBACK, *LPDSREMOTECALLBACK;

typedef const DSREMOTECALLBACK *LPCDSREMOTECALLBACK;

//
// DSREMOTEMSGHANDLER (pseudo-interface vector table)
//

typedef struct _DSREMOTEMSGHANDLER
{
    BOOL (STDMETHODCALLTYPE *Receive)(LPCDSRPACKET pPacketHeader, LPCDSRBUFFER pPacketBuffer, LPDSRBUFFER pResponseBuffer, LPVOID pvContext);
} DSREMOTEMSGHANDLER, *LPDSREMOTEMSGHANDLER;

typedef const DSREMOTEMSGHANDLER *LPCDSREMOTEMSGHANDLER;

//
// IRemoteConnection
//

#undef INTERFACE
#define INTERFACE IRemoteConnection

DECLARE_INTERFACE(IRemoteConnection)
{
    // IUnknown methods
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // IRemoteConnection methods
    STDMETHOD(Connect)(THIS_ LPCSTR pszHost, DWORD dwFlags) PURE;
    STDMETHOD(Disconnect)(THIS_ DWORD dwFlags) PURE;
    STDMETHOD_(DWORD, GetState)(THIS) PURE;
    STDMETHOD(Send)(THIS_ LPCDSRPACKET pPacketHeader, DWORD dwPacketBufferCount, LPCDSRBUFFER paPacketBuffers, LPDSRBUFFER pResponseBuffer) PURE;
    STDMETHOD(RegisterMsgHandler)(THIS_ DWORD dwMajorType, LPCDSREMOTEMSGHANDLER pHandler, LPVOID pvContext) PURE;
    STDMETHOD(UnregisterMsgHandler)(THIS_ DWORD dwMajorType) PURE;
    STDMETHOD_(VOID, DoWork)(THIS) PURE;
};

//
// IRemoteFileIo
//

#undef INTERFACE
#define INTERFACE IRemoteFileIo

DECLARE_INTERFACE(IRemoteFileIo)
{
    // IUnknown methods
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // IRemoteFileIo methods
    STDMETHOD(CreateFile)(THIS_ LPCSTR pszRemoteFile, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDistribution, DWORD dwFlagsAndAttributes, LPHANDLE phRemoteFile) PURE;
    STDMETHOD(ReadFile)(THIS_ HANDLE hRemoteFile, LPVOID pvBuffer, DWORD dwBufferSize, LPDWORD pdwBytesRead) PURE;
    STDMETHOD(WriteFile)(THIS_ HANDLE hRemoteFile, LPCVOID pvBuffer, DWORD dwBufferSize, LPDWORD pdwBytesWritten) PURE;
    STDMETHOD(SetFilePointer)(THIS_ HANDLE hRemoteFile, LONG lDistanceToMove, DWORD dwMoveMethod, LPDWORD pdwAbsPosition) PURE;
    STDMETHOD(SetEndOfFile)(THIS_ HANDLE hRemoteFile) PURE;
    STDMETHOD(CloseHandle)(THIS_ HANDLE hRemoteFile) PURE;
    STDMETHOD(CopyFile)(THIS_ LPCSTR pszLocalFile, LPCSTR pszRemoteFile, BOOL fFailIfFileExists) PURE;
    STDMETHOD(DeleteFile)(THIS_ LPCSTR pszRemoteFile) PURE;
};

//
// API
//

STDAPI CreateRemoteConnection(DWORD dwPort, LPCDSREMOTECALLBACK pCallback, LPVOID pvContext, LPREMOTECONNECTION *ppConnection);
STDAPI CreateRemoteFileIo(LPREMOTECONNECTION pConnection, LPREMOTEFILEIO *ppFileIo);

#endif // __DSREMOTE_H__
