/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
    Module implementing file and socket I/O primitives

Module Name:

    baseio.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// Do work function for file I/O
//
HRESULT CXo::ReadWriteFileContinue(
            XONLINETASK_HANDLE  hTask
            )
{
    PXONLINETASK_FILEIO pfileio = (PXONLINETASK_FILEIO)hTask;

    Assert(hTask != NULL);

    // Check to see if our overlapped file I/O has completed
    if (!GetOverlappedResult(pfileio->hFile, 
                &(pfileio->overlapped), &(pfileio->cbDone), FALSE))
    {
        DWORD dwError = GetLastError();
        if ((dwError != ERROR_IO_PENDING) &&
            (dwError != ERROR_IO_INCOMPLETE))
        {
            // Hard error, abort the file I/O
            pfileio->hr = HRESULT_FROM_WIN32(dwError);
        }
        else
            pfileio->hr = XONLINETASK_S_RUNNING;
        
        goto Cleanup;
    }
    else
    {
        // Return completion and success
        pfileio->hr = XONLINETASK_S_SUCCESS;
    }

Cleanup:
    return(pfileio->hr);
}

//
// Function to close a file IO
//
VOID CXo::ReadWriteFileClose(
            XONLINETASK_HANDLE  hTask
            )
{
    SysFree(hTask);
}

//
// Function to actually kick off the read or write
//
HRESULT CXo::ReadWriteFileBegin(
            PXONLINETASK_FILEIO pfileio,
            BOOL                fRead
            )
{
    HRESULT     hr = S_OK;
    BOOL        fResult;

    // Kick off the asynchronous op
    if (fRead)
        fResult = ReadFile(pfileio->hFile, pfileio->pbBuffer, 
                    pfileio->cbBuffer, NULL, &(pfileio->overlapped));
    else
        fResult = WriteFile(pfileio->hFile, pfileio->pbBuffer, 
                    pfileio->cbBuffer, NULL, &(pfileio->overlapped));
        
    // We need to set the work event here because overlapped file I/O will call ResetEvent,
    // and that might have just reset the signal from a different subtask.  By setting it
    // now, we might be over-setting but at least we guarantee we are not under-setting,
    // which could cause a stall.

    if (pfileio->xontask.hEventWorkAvailable)
        SetEvent(pfileio->xontask.hEventWorkAvailable);
    
    if (!fResult)
    {
        DWORD dwError = GetLastError();
        
        if (dwError != ERROR_IO_PENDING)
            hr = HRESULT_FROM_WIN32(dwError);
    }

    return(hr);
}

//
// Function to initialize a context for file read or write
//
VOID CXo::ReadWriteFileInitializeContext(
            HANDLE              hFile,
            LPBYTE              pbBuffer,
            DWORD               cbBytes,
            ULARGE_INTEGER      uliStartingOffset,
            HANDLE              hWorkEvent,
            PXONLINETASK_FILEIO pfileio
            )
{
    Assert(pfileio != NULL);
    Assert(hFile != INVALID_HANDLE_VALUE);
    Assert(pbBuffer != NULL);
    Assert(((DWORD)pbBuffer & 0x3) == 0);
    Assert((cbBytes & (XBOX_HD_SECTOR_SIZE - 1)) == 0);
    Assert((uliStartingOffset.u.LowPart & (XBOX_HD_SECTOR_SIZE - 1)) == 0);

    // Fill in the blanks
    ZeroMemory(pfileio, sizeof(XONLINETASK_FILEIO));
    pfileio->hr = S_OK;
    pfileio->hFile = hFile;
    pfileio->pbBuffer = pbBuffer;
    pfileio->cbBuffer = cbBytes;
    pfileio->overlapped.Offset = uliStartingOffset.u.LowPart;
    pfileio->overlapped.OffsetHigh = uliStartingOffset.u.HighPart;
    pfileio->overlapped.hEvent = hWorkEvent;

    TaskInitializeContext(&pfileio->xontask);
    pfileio->xontask.pfnContinue = ReadWriteFileContinue;
    pfileio->xontask.pfnClose = ReadWriteFileClose;
    pfileio->xontask.hEventWorkAvailable = hWorkEvent;
}

//
// Function to kick off a generic overlapped file read or write
//
HRESULT CXo::ReadWriteFile(
            HANDLE              hFile,
            LPBYTE              pbBuffer,
            DWORD               cbBytes,
            ULARGE_INTEGER      uliStartingOffset,
            HANDLE              hWorkEvent,
            XONLINETASK_HANDLE  *phTask,
            BOOL                fRead
            )
{
    HRESULT             hr = S_OK;
    PXONLINETASK_FILEIO pfileio = NULL;
    DWORD               dwError;
    BOOL                fResult;

    Assert(phTask != NULL);
    Assert(hFile != INVALID_HANDLE_VALUE);
    Assert(pbBuffer != NULL);
    Assert(((DWORD)pbBuffer & 0x3) == 0);
    Assert((cbBytes & (XBOX_HD_SECTOR_SIZE - 1)) == 0);
    Assert((uliStartingOffset.u.LowPart & (XBOX_HD_SECTOR_SIZE - 1)) == 0);

    // Allocate the task context
    pfileio = (PXONLINETASK_FILEIO)SysAlloc(sizeof(XONLINETASK_FILEIO), PTAG_XONLINETASK_FILEIO);
    if (!pfileio)
        return(E_OUTOFMEMORY);

    // Initialize the context
    ReadWriteFileInitializeContext(hFile, pbBuffer, cbBytes, 
                uliStartingOffset, hWorkEvent, pfileio);

    // Kick off the asynchronous op
    hr = ReadWriteFileBegin(pfileio, fRead);
    if (FAILED(hr))
        goto Error;

    // Return the opaque handle
    *phTask = (XONLINETASK_HANDLE)pfileio;
    
Cleanup:
    return(hr);

Error:
    if (pfileio)
    {
        SysFree(pfileio);
    }
    goto Cleanup;
}

//
// Function to retrieve the results of a file I/O
//
HRESULT CXo::GetReadWriteFileResults(
            XONLINETASK_HANDLE  hTask,
            DWORD               *pdwBytesTransferred,
            LPBYTE              *ppbBuffer
            )
{
    PXONLINETASK_FILEIO pfileio = (PXONLINETASK_FILEIO)hTask;

    Assert(hTask != NULL);

    if (pdwBytesTransferred)
        *pdwBytesTransferred = pfileio->cbDone;
    if (ppbBuffer)
        *ppbBuffer = pfileio->pbBuffer;
    return(pfileio->hr);
}

//
// Do work function for socket stream connect
//
HRESULT CXo::StreamConnectContinue(
            XONLINETASK_HANDLE  hTask
            )
{
    int                 iResult, iError;
    fd_set              fdsetWrite, fdsetExcept;
    struct timeval      tvTimeout = { 0, 0 };
    PXONLINETASK_SOCKIO psockio = (PXONLINETASK_SOCKIO)hTask;

    Assert(hTask != NULL);

    // Make sure the event is set for polling
    if (psockio->xontask.hEventWorkAvailable)
        SetEvent(psockio->xontask.hEventWorkAvailable);

    // Check to see if our async stream connect had completed
    fdsetWrite.fd_count = 1;
    fdsetWrite.fd_array[0] = psockio->socket;
    fdsetExcept.fd_count = 1;
    fdsetExcept.fd_array[0] = psockio->socket;

    psockio->hr = XONLINETASK_S_RUNNING;
    iResult = select(1, NULL, &fdsetWrite, &fdsetExcept, &tvTimeout);
    if (iResult == SOCKET_ERROR)
    {
        psockio->hr = HRESULT_FROM_WIN32(WSAGetLastError());
        goto Cleanup;
    }
    else if (!iResult)
    {
        // The results are not available yet, continue
        goto Cleanup;
    }
    else
    {
        psockio->hr = XONLINETASK_S_SUCCESS;
    
        // Something happened to the socket, figure out what happened
        if (fdsetExcept.fd_count != 0)
        {
            Assert(fdsetExcept.fd_array[0] == psockio->socket);

            // We failed, return E_FAIL. In original winsock, we should
            // be able to retrieve the failure code using getsockopt on
            // SO_ERROR. But Xbox does not support this option. 
            psockio->hr = HRESULT_FROM_WIN32(WSAEADDRNOTAVAIL);
        }
        else if (fdsetWrite.fd_count != 0)
        {
            Assert(fdsetWrite.fd_count == 1);
            Assert(fdsetWrite.fd_array[0] == psockio->socket);
        }
        else
        {
            AssertSz(FALSE, "StreamConnectContinue: bad select results");
        }

        // Either case, we are done!
    }

Cleanup:
    return(psockio->hr);
}

//
// Close function for socket stream connect
//
VOID CXo::StreamConnectClose(
            XONLINETASK_HANDLE  hTask
            )
{
    SysFree(hTask);
}

//
// Function to actually kick off the stream connect
//
HRESULT CXo::StreamConnectBegin(SOCKADDR_IN * psockaddr, PXONLINETASK_SOCKIO psockio)
{
    HRESULT hr = S_OK;
    ULONG   lnbio = 1;
    int     iResult;

    // Make sure the socket is set to nonblocking mode
    iResult = ioctlsocket(psockio->socket, FIONBIO, &lnbio);
    if (iResult == SOCKET_ERROR)
        return(HRESULT_FROM_WIN32(WSAGetLastError()));

    // Kick off the asynchronous connect
    iResult = connect(psockio->socket, (PSOCKADDR)psockaddr, sizeof(SOCKADDR_IN));
    if (iResult == SOCKET_ERROR)
    {
        DWORD dwError = WSAGetLastError();

        // Treat WSAEWOULDBLOCK as an error (too many IO's)
        if (dwError != WSAEWOULDBLOCK)
            hr = HRESULT_FROM_WIN32(dwError);

        // WSAEWOULDBLOCK is the expected result ...
    }
    // connect returning success immedaitely is also a success
    // scenario, although not expected. This means that the connection
    // was established synchronously. Since this should be so rare,
    // we treat it as if it was asynchronous, and the first Continue
    // call will return success.

    // This is actually a polling operation, set the event
    if (psockio->xontask.hEventWorkAvailable)
        SetEvent(psockio->xontask.hEventWorkAvailable);

    return(hr);
}

//
// Function to initialize the context for async stream connect
//
VOID CXo::StreamConnectInitializeContext(
            SOCKET              socketIn,
            HANDLE              hWorkEvent,
            PXONLINETASK_SOCKIO psockio
            )
{
    Assert(psockio != NULL);
    Assert(socketIn != INVALID_SOCKET);

    // Fill in the blanks
    ZeroMemory(psockio, sizeof(XONLINETASK_SOCKIO));
    psockio->hr = S_OK;
    psockio->socket = socketIn;
    psockio->wsaoverlapped.hEvent = hWorkEvent;

    TaskInitializeContext(&psockio->xontask);
    psockio->xontask.pfnContinue = StreamConnectContinue;
    psockio->xontask.pfnClose = StreamConnectClose;
    psockio->xontask.hEventWorkAvailable = hWorkEvent;
}

//
// Function to kick off an asynchronous stream connect
//
HRESULT CXo::StreamConnect(
            SOCKET              socketIn,
            SOCKADDR_IN         *psockaddr,
            HANDLE              hWorkEvent,
            XONLINETASK_HANDLE  *phTask
            )
{
    HRESULT             hr = S_OK;
    PXONLINETASK_SOCKIO psockio = NULL;
    DWORD               dwError;
    int                 iResult;

    Assert(phTask != NULL);
    Assert(socketIn != INVALID_SOCKET);

    // Allocate the task context
    psockio = (PXONLINETASK_SOCKIO)SysAlloc(sizeof(XONLINETASK_SOCKIO), PTAG_XONLINETASK_SOCKIO);
    if (!psockio)
        return(E_OUTOFMEMORY);

    // Initialize the context
    StreamConnectInitializeContext(socketIn, hWorkEvent, psockio);

    // Kick off the asynchronous op
    hr = StreamConnectBegin(psockaddr, psockio);
    if (FAILED(hr))
        goto Error;

    // Return the opaque handle
    *phTask = (XONLINETASK_HANDLE)psockio;

Cleanup:
    return(hr);

Error:
    if (psockio)
    {
        SysFree(psockio);
    }
    goto Cleanup;
}

//
// Function to retrieve the results of a stream connect
//
HRESULT CXo::GetStreamConnectResults(XONLINETASK_HANDLE hTask)
{
    return(((PXONLINETASK_SOCKIO)hTask)->hr);
}

//
// Do work function for socket I/O
//
HRESULT CXo::SendRecvContinue(XONLINETASK_HANDLE hTask)
{
    PXONLINETASK_SOCKIO psockio = (PXONLINETASK_SOCKIO)hTask;

    Assert(hTask != NULL);

    // Check to see if our overlapped file I/O has completed
    if (!WSAGetOverlappedResult(psockio->socket, 
                &(psockio->wsaoverlapped), &(psockio->cbDone), FALSE,
                &(psockio->dwFlags)))
    {
        DWORD dwError = WSAGetLastError();
        if ((dwError != ERROR_IO_PENDING) &&
            (dwError != ERROR_IO_INCOMPLETE))
        {
            psockio->hr = HRESULT_FROM_WIN32(dwError);
        }
        else
            psockio->hr = XONLINETASK_S_RUNNING;
        goto Cleanup;
    }
    else
    {
        // Return signalled state and success
        psockio->hr = XONLINETASK_S_SUCCESS;
    }

Cleanup:
    return(psockio->hr);
}

//
// Close function for socket stream I/O
//
VOID CXo::SendRecvClose(XONLINETASK_HANDLE hTask)
{
    SysFree(hTask);
}

//
// Function to actually kick off the socket operation
//
HRESULT CXo::SendRecvBegin(PXONLINETASK_SOCKIO psockio, BOOL fSend)
{
    HRESULT hr = S_OK;
    int     iResult;

    // Kick off the asynchronous op
    if (fSend)
        iResult = WSASend(psockio->socket, psockio->pBuffers, 
                    psockio->dwBuffers, &psockio->cbDone, 
                    psockio->dwFlags, &psockio->wsaoverlapped, NULL);
    else                
        iResult = WSARecv(psockio->socket, psockio->pBuffers, 
                    psockio->dwBuffers, &psockio->cbDone, 
                    &psockio->dwFlags, &psockio->wsaoverlapped, NULL);

    if (iResult == SOCKET_ERROR)
    {
        DWORD dwError = WSAGetLastError();

        // Treat WSAEWOULDBLOCK as an error (too many IO's)
        if (dwError != ERROR_IO_PENDING)
            hr = HRESULT_FROM_WIN32(dwError);

        // ERROR_IO_PENDING is the expected result ...
    }
    // WSASend returning success is not generally expected, but 
    // it's OK because this is functionally equivalent to an 
    // async completion.

    return(hr);
}

//
// Function to initialize the context for socket send or receive
//
VOID CXo::SendRecvInitializeContext(
            SOCKET              socket,
            LPWSABUF            pBuffers,
            DWORD               dwBufferCount,
            DWORD               dwFlags,
            HANDLE              hWorkEvent,
            PXONLINETASK_SOCKIO psockio
            )
{
    Assert(psockio != NULL);
    Assert(socket != INVALID_SOCKET);
    Assert(pBuffers != NULL);

    // Fill in the blanks
    ZeroMemory(psockio, sizeof(XONLINETASK_SOCKIO));
    psockio->hr = S_OK;
    psockio->socket = socket;
    psockio->pBuffers = pBuffers;
    psockio->dwBuffers = dwBufferCount;
    psockio->dwFlags = dwFlags;
    psockio->wsaoverlapped.hEvent = hWorkEvent;

    TaskInitializeContext(&psockio->xontask);
    psockio->xontask.pfnContinue = SendRecvContinue;
    psockio->xontask.pfnClose = SendRecvClose;
    psockio->xontask.hEventWorkAvailable = hWorkEvent;
}

//
// Function to kick off a generic overlapped socket send or receive
//
HRESULT CXo::SendRecv(
            SOCKET              socket,
            LPWSABUF            pBuffers,
            DWORD               dwBufferCount,
            DWORD               dwFlags,
            HANDLE              hWorkEvent,
            XONLINETASK_HANDLE  *phTask,
            BOOL                fSend
            )
{
    HRESULT             hr = S_OK;
    PXONLINETASK_SOCKIO psockio = NULL;
    DWORD               dwError;
    int                 iResult;

    Assert(phTask != NULL);
    Assert(socket != INVALID_SOCKET);
    Assert(pBuffers != NULL);

    // Allocate the task context
    psockio = (PXONLINETASK_SOCKIO)SysAlloc(sizeof(XONLINETASK_SOCKIO), PTAG_XONLINETASK_SOCKIO);
    if (!psockio)
        return(E_OUTOFMEMORY);

    // Initialize the context
    SendRecvInitializeContext(socket, pBuffers, dwBufferCount,
                dwFlags, hWorkEvent, psockio);

    // Kick off the asynchronous op
    hr = SendRecvBegin(psockio, fSend);
    if (FAILED(hr))
        goto Error;

    // Return the opaque handle
    *phTask = (XONLINETASK_HANDLE)psockio;

Cleanup:
    return(hr);

Error:
    if (psockio)
    {
        SysFree(psockio);
    }
    goto Cleanup;
}

//
// Function to retrieve the results of a socket I/O
//
HRESULT CXo::GetSendRecvResults(
            XONLINETASK_HANDLE  hTask,
            DWORD               *pdwBytesTransferred,
            DWORD               *pdwFlags,
            LPWSABUF            *ppBuffers
            )
{
    PXONLINETASK_SOCKIO psockio = (PXONLINETASK_SOCKIO)hTask;

    Assert(hTask != NULL);

    if (pdwBytesTransferred)
        *pdwBytesTransferred = psockio->cbDone;
    if (pdwFlags)
        *pdwFlags = psockio->dwFlags;
    if (ppBuffers)
        *ppBuffers = psockio->pBuffers;
    return(psockio->hr);
}


