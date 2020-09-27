/***************************************************************************
 *
 *  Copyright (C) 1/18/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ifileio.cpp
 *  Content:    IRemoteFileIo implementation.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/18/2002   dereks  Created.
 *
 ****************************************************************************/

#include "dsremi.h"

using namespace DSREMOTE;


/****************************************************************************
 *
 *  CreateRemoteFileIo
 *
 *  Description:
 *      Creates a remote file I/O object.
 *
 *  Arguments:
 *      LPREMOTECONNECTION [in]: connection object.
 *      LPREMOTEFILEIO * [out]: remote file I/O object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CreateRemoteFileIo"

HRESULT
CreateRemoteFileIo
(
    LPREMOTECONNECTION      pConnection, 
    LPREMOTEFILEIO *        ppFileIo
)
{
    CRemoteFileIo *         pFileIo;
    HRESULT                 hr;

    hr = HRFROMP(pFileIo = NEW(CRemoteFileIo(pConnection)));

    if(SUCCEEDED(hr))
    {
        *ppFileIo = pFileIo;
    }

    return hr;
}


/****************************************************************************
 *
 *  CRemoteFileIo
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      LPREMOTECONNECTION [in]: connection object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::CRemoteFileIo"

CRemoteFileIo::CRemoteFileIo
(
    LPREMOTECONNECTION      pConnection
)
{
    m_pConnection = ADDREF(pConnection);
}


/****************************************************************************
 *
 *  ~CRemoteFileIo
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::~CRemoteFileIo"

CRemoteFileIo::~CRemoteFileIo
(
    void
)
{
    RELEASE(m_pConnection);
}


/****************************************************************************
 *
 *  CreateFile
 *
 *  Description:
 *      Creates or opens a file on the remote machine.
 *
 *  Arguments:
 *      LPCSTR [in]: remote file path.
 *      DWORD [in]: access flags.
 *      DWORD [in]: share mode.
 *      DWORD [in]: creation flags.
 *      DWORD [in]: general flags and file attributes.
 *      LPHANDLE [out]: remote file handle.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::CreateFile"

HRESULT
CRemoteFileIo::CreateFile
(
    LPCSTR                          pszRemoteFile, 
    DWORD                           dwDesiredAccess, 
    DWORD                           dwShareMode, 
    DWORD                           dwCreationDistribution, 
    DWORD                           dwFlagsAndAttributes, 
    LPHANDLE                        phFile
)
{
    DSRPACKET                       Packet;
    DSRBUFFER                       Buffers[1];
    DSRBUFFER                       Response;
    DSRFILEIO_CREATEFILE_MSGDATA    PacketData;
    DSRFILEIO_CREATEFILE_RESPONSE   ResponseData;
    HRESULT                         hr;

    Packet.dwMajorType = DSRTYPE_FILEIO;
    Packet.dwMinorType = DSRTYPE_FILEIO_CREATEFILE;
    Packet.dwFlags = 0;

    Buffers[0].dwSize = sizeof(PacketData);
    Buffers[0].pvData = &PacketData;

    strcpy(PacketData.szFileName, pszRemoteFile);

    PacketData.dwDesiredAccess = dwDesiredAccess;
    PacketData.dwShareMode = dwShareMode;
    PacketData.dwCreationDistribution = dwCreationDistribution;
    PacketData.dwFlagsAndAttributes = dwFlagsAndAttributes;

    Response.dwSize = sizeof(ResponseData);
    Response.pvData = &ResponseData;

    hr = m_pConnection->Send(&Packet, NUMELMS(Buffers), Buffers, &Response);

    if(SUCCEEDED(hr))
    {
        ASSERT(sizeof(ResponseData) == Response.dwSize);
        
        if(ERROR_SUCCESS == ResponseData.dwError)
        {
            ASSERT(IS_VALID_HANDLE_VALUE(ResponseData.hFile));

            *phFile = ResponseData.hFile;
        }
        else
        {
            ASSERT(!IS_VALID_HANDLE_VALUE(ResponseData.hFile));

            hr = HRESULT_FROM_WIN32(ResponseData.dwError);
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  ReadFile
 *
 *  Description:
 *      Reads data from a file opened on the remote machine.
 *
 *  Arguments:
 *      HANDLE [in]: remote file handle.
 *      LPVOID [out]: data buffer.
 *      DWORD [in]: data buffer size, in bytes.
 *      LPDWORD [out]: amount read, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::ReadFile"

HRESULT
CRemoteFileIo::ReadFile
(
    HANDLE                          hFile, 
    LPVOID                          pvBuffer, 
    DWORD                           dwBufferSize, 
    LPDWORD                         pdwBytesRead
)                                   
{                                   
    DSRPACKET                       Packet;
    DSRBUFFER                       Buffers[1];
    DSRBUFFER                       Response;
    DSRFILEIO_READFILE_MSGDATA      PacketData;
    LPDSRFILEIO_READFILE_RESPONSE   pResponseData;
    DWORD                           dwResponseSize;
    HRESULT                         hr;

    //
    // Read requests are sent as a simple message containing just the file
    // handle.  The response packet dictates the amount of data to read and
    // the amount actually read.
    //

    dwResponseSize = sizeof(*pResponseData) + dwBufferSize;

    hr = HRFROMP(pResponseData = (LPDSRFILEIO_READFILE_RESPONSE)MEMALLOC_NOINIT(BYTE, dwResponseSize));

    if(SUCCEEDED(hr))
    {
        Packet.dwMajorType = DSRTYPE_FILEIO;
        Packet.dwMinorType = DSRTYPE_FILEIO_READFILE;
        Packet.dwFlags = 0;
        
        Buffers[0].dwSize = sizeof(PacketData);
        Buffers[0].pvData = &PacketData;
    
        PacketData.hFile = hFile;

        Response.dwSize = dwResponseSize;
        Response.pvData = pResponseData;

        hr = m_pConnection->Send(&Packet, NUMELMS(Buffers), Buffers, &Response);
    }

    if(SUCCEEDED(hr))
    {
        if(ERROR_SUCCESS == pResponseData->dwError)
        {
            CopyMemory(pvBuffer, pResponseData + 1, Response.dwSize - sizeof(*pResponseData));

            if(pdwBytesRead)
            {
                *pdwBytesRead = Response.dwSize - sizeof(*pResponseData);
            }
        }
        else
        {
            hr = HRESULT_FROM_WIN32(pResponseData->dwError);
        }
    }

    MEMFREE(pResponseData);

    return hr;
}


/****************************************************************************
 *
 *  WriteFile
 *
 *  Description:
 *      Writes data to a file opened on the remote machine.
 *
 *  Arguments:
 *      HANDLE [in]: remote file handle.
 *      LPCVOID [in]: data buffer.
 *      DWORD [in]: data buffer size, in bytes.
 *      LPDWORD [out]: amount written, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::WriteFile"

HRESULT
CRemoteFileIo::WriteFile
(
    HANDLE                          hFile, 
    LPCVOID                         pvBuffer, 
    DWORD                           dwBufferSize, 
    LPDWORD                         pdwBytesWritten
)                                   
{                                   
    DSRPACKET                       Packet;
    DSRBUFFER                       Buffers[2];
    DSRBUFFER                       Response;
    DSRFILEIO_WRITEFILE_MSGDATA     PacketData;
    DSRFILEIO_WRITEFILE_RESPONSE    ResponseData;
    DWORD                           dwPacketSize;
    HRESULT                         hr;

    //
    // Write requests are sent with the buffer data appended to the outgoing
    // packet.
    //

    Packet.dwMajorType = DSRTYPE_FILEIO;
    Packet.dwMinorType = DSRTYPE_FILEIO_WRITEFILE;
    Packet.dwFlags = 0;

    Buffers[0].dwSize = sizeof(PacketData);
    Buffers[0].pvData = &PacketData;

    PacketData.hFile = hFile;

    Buffers[1].dwSize = dwBufferSize;
    Buffers[1].pvData = (LPVOID)pvBuffer;
    
    Response.dwSize = sizeof(ResponseData);
    Response.pvData = &ResponseData;

    hr = m_pConnection->Send(&Packet, NUMELMS(Buffers), Buffers, &Response);

    if(SUCCEEDED(hr))
    {
        if(ERROR_SUCCESS == ResponseData.dwError)
        {
            if(pdwBytesWritten)
            {
                *pdwBytesWritten = ResponseData.dwBytesWritten;
            }
        }
        else
        {
            hr = HRESULT_FROM_WIN32(ResponseData.dwError);
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  SetFilePointer
 *
 *  Description:
 *      Sets the current file pointer on a file opened on the remote machine.
 *
 *  Arguments:
 *      HANDLE [in]: remote file handle.
 *      LONG [in]: distance to move.
 *      DWORD [in]: move method.
 *      LPDWORD [out]: absolute file position after the move.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::SetFilePointer"

HRESULT
CRemoteFileIo::SetFilePointer
(
    HANDLE                              hFile, 
    LONG                                lDistanceToMove, 
    DWORD                               dwMoveMethod, 
    LPDWORD                             pdwAbsPosition
)                                   
{                                   
    DSRPACKET                           Packet;
    DSRBUFFER                           Buffers[1];
    DSRBUFFER                           Response;
    DSRFILEIO_SETFILEPOINTER_MSGDATA    PacketData;
    DSRFILEIO_SETFILEPOINTER_RESPONSE   ResponseData;
    HRESULT                             hr;

    Packet.dwMajorType = DSRTYPE_FILEIO;
    Packet.dwMinorType = DSRTYPE_FILEIO_SETFILEPOINTER;
    Packet.dwFlags = 0;
    
    Buffers[0].dwSize = sizeof(PacketData);
    Buffers[0].pvData = &PacketData;

    PacketData.hFile = hFile;
    PacketData.lDistanceToMove = lDistanceToMove;
    PacketData.dwMoveMethod = dwMoveMethod;

    Response.dwSize = sizeof(ResponseData);
    Response.pvData = &ResponseData;

    hr = m_pConnection->Send(&Packet, NUMELMS(Buffers), Buffers, &Response);

    if(SUCCEEDED(hr))
    {
        if(ERROR_SUCCESS == ResponseData.dwError)
        {
            if(pdwAbsPosition)
            {
                *pdwAbsPosition = ResponseData.dwAbsPosition;
            }
        }
        else
        {
            hr = HRESULT_FROM_WIN32(ResponseData.dwError);
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  SetEndOfFile
 *
 *  Description:
 *      Sets the current file position as the end of the file.
 *
 *  Arguments:
 *      HANDLE [in]: remote file handle.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::SetEndOfFile"

HRESULT
CRemoteFileIo::SetEndOfFile
(
    HANDLE                          hFile
)                                   
{                                   
    DSRPACKET                       Packet;
    DSRBUFFER                       Buffers[1];
    DSRBUFFER                       Response;
    DSRFILEIO_SETENDOFFILE_MSGDATA  PacketData;
    DSRFILEIO_SETENDOFFILE_RESPONSE ResponseData;
    HRESULT                         hr;

    Packet.dwMajorType = DSRTYPE_FILEIO;
    Packet.dwMinorType = DSRTYPE_FILEIO_SETENDOFFILE;
    Packet.dwFlags = 0;
    
    Buffers[0].dwSize = sizeof(PacketData);
    Buffers[0].pvData = &PacketData;

    PacketData.hFile = hFile;

    Response.dwSize = sizeof(ResponseData);
    Response.pvData = &ResponseData;

    hr = m_pConnection->Send(&Packet, NUMELMS(Buffers), Buffers, &Response);

    if(SUCCEEDED(hr))
    {
        if(ERROR_SUCCESS != ResponseData.dwError)
        {
            hr = HRESULT_FROM_WIN32(ResponseData.dwError);
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  CloseHandle
 *
 *  Description:
 *      Closes a remotely open file.
 *
 *  Arguments:
 *      HANDLE [in]: remote file handle.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::CloseHandle"

HRESULT
CRemoteFileIo::CloseHandle
(
    HANDLE                          hFile
)                                   
{                                   
    DSRPACKET                       Packet;
    DSRBUFFER                       Buffers[1];
    DSRBUFFER                       Response;
    DSRFILEIO_CLOSEHANDLE_MSGDATA   PacketData;
    DSRFILEIO_CLOSEHANDLE_RESPONSE  ResponseData;
    HRESULT                         hr;

    Packet.dwMajorType = DSRTYPE_FILEIO;
    Packet.dwMinorType = DSRTYPE_FILEIO_CLOSEHANDLE;
    Packet.dwFlags = 0;

    Buffers[0].dwSize = sizeof(PacketData);
    Buffers[0].pvData = &PacketData;

    PacketData.hFile = hFile;

    Response.dwSize = sizeof(ResponseData);
    Response.pvData = &ResponseData;

    hr = m_pConnection->Send(&Packet, NUMELMS(Buffers), Buffers, &Response);

    if(SUCCEEDED(hr))
    {
        if(ERROR_SUCCESS != ResponseData.dwError)
        {
            hr = HRESULT_FROM_WIN32(ResponseData.dwError);
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  DeleteFile
 *
 *  Description:
 *      Deletes a file from the remote machine.
 *
 *  Arguments:
 *      LPCSTR [in]: remote file name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::DeleteFile"

HRESULT
CRemoteFileIo::DeleteFile
(
    LPCSTR                          pszRemoteFile
)                                   
{                                   
    DSRPACKET                       Packet;
    DSRBUFFER                       Buffers[1];
    DSRBUFFER                       Response;
    DSRFILEIO_DELETEFILE_MSGDATA    PacketData;
    DSRFILEIO_DELETEFILE_RESPONSE   ResponseData;
    HRESULT                         hr;

    Packet.dwMajorType = DSRTYPE_FILEIO;
    Packet.dwMinorType = DSRTYPE_FILEIO_DELETEFILE;
    Packet.dwFlags = 0;
    
    Buffers[0].dwSize = sizeof(PacketData);
    Buffers[0].pvData = &PacketData;

    strcpy(PacketData.szFileName, pszRemoteFile);

    Response.dwSize = sizeof(ResponseData);
    Response.pvData = &ResponseData;

    hr = m_pConnection->Send(&Packet, NUMELMS(Buffers), Buffers, &Response);

    if(SUCCEEDED(hr))
    {
        if(ERROR_SUCCESS != ResponseData.dwError)
        {
            hr = HRESULT_FROM_WIN32(ResponseData.dwError);
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  CopyFile
 *
 *  Description:
 *      Copies a file from this machine to the remote one.
 *
 *  Arguments:
 *      LPCSTR [in]: source file name.
 *      LPCSTR [in]: remote file name.
 *      BOOL [in]: FALSE to allow overwriting the file.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIo::CopyFile"

HRESULT
CRemoteFileIo::CopyFile
(
    LPCSTR                  pszSourceFile,
    LPCSTR                  pszRemoteFile,
    BOOL                    fFailIfFileExists
)                                   
{                                   
    static const DWORD      dwBufferSize    = DSREMOTE_OPTIMAL_PACKET_SIZE - sizeof(DSRFILEIO_WRITEFILE_MSGDATA);
    HANDLE                  hSourceFile     = INVALID_HANDLE_VALUE;
    HANDLE                  hRemoteFile     = INVALID_HANDLE_VALUE;
    LPVOID                  pvBuffer;
    DWORD                   dwRead;
    HRESULT                 hr;

    hr = HRFROMP(pvBuffer = MEMALLOC_NOINIT(BYTE, dwBufferSize));

    if(SUCCEEDED(hr))
    {
        if(!IS_VALID_HANDLE_VALUE(hSourceFile = ::CreateFile(pszSourceFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL)))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = this->CreateFile(pszRemoteFile, GENERIC_READ | GENERIC_WRITE, 0, fFailIfFileExists ? CREATE_NEW : CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, &hRemoteFile);
    }

    while(SUCCEEDED(hr))
    {
        if(!::ReadFile(hSourceFile, pvBuffer, dwBufferSize, &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        if(SUCCEEDED(hr) && !dwRead)
        {
            break;
        }

        if(SUCCEEDED(hr))
        {
            hr = this->WriteFile(hRemoteFile, pvBuffer, dwRead, NULL);
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = this->SetEndOfFile(hRemoteFile);
    }

    if(IS_VALID_HANDLE_VALUE(hSourceFile))
    {
        ::CloseHandle(hSourceFile);
    }
    
    if(IS_VALID_HANDLE_VALUE(hRemoteFile))
    {
        this->CloseHandle(hRemoteFile);
    }
    
    MEMFREE(pvBuffer);

    return hr;
}


/****************************************************************************
 *
 *  Receive
 *
 *  Description:
 *      Called when data is received.
 *
 *  Arguments:
 *      LPCDSRPACKET [in]: packet data.
 *      LPDSRBUFFER   [in/out]: response data.
 *      LPVOID [in]: callback context.
 *
 *  Returns:  
 *      BOOL: TRUE if the message was handled.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIoHandler;:Receive"

BOOL
CRemoteFileIoHandler::Receive
(
    LPCDSRPACKET            pPacketHeader, 
    LPCDSRBUFFER            pPacketBuffer, 
    LPDSRBUFFER             pResponseBuffer, 
    LPVOID                  pvContext
)
{
    CRemoteFileIoHandler *  pHandler    = (CRemoteFileIoHandler *)pvContext;
    BOOL                    fHandled    = TRUE;
    
    ASSERT(pPacketHeader);
    ASSERT(pPacketBuffer);
    ASSERT(pResponseBuffer);

    ASSERT(DSRTYPE == pPacketHeader->dwMajorType);
    
    switch(pPacketHeader->dwMinorType)
    {
        case DSRTYPE_FILEIO_CREATEFILE:
            pHandler->CreateFile(pPacketBuffer, pResponseBuffer);
            break;

        case DSRTYPE_FILEIO_READFILE:
            pHandler->ReadFile(pPacketBuffer, pResponseBuffer);
            break;

        case DSRTYPE_FILEIO_WRITEFILE:
            pHandler->WriteFile(pPacketBuffer, pResponseBuffer);
            break;

        case DSRTYPE_FILEIO_SETFILEPOINTER:
            pHandler->SetFilePointer(pPacketBuffer, pResponseBuffer);
            break;

        case DSRTYPE_FILEIO_SETENDOFFILE:
            pHandler->SetEndOfFile(pPacketBuffer, pResponseBuffer);
            break;

        case DSRTYPE_FILEIO_CLOSEHANDLE:
            pHandler->CloseHandle(pPacketBuffer, pResponseBuffer);
            break;

        case DSRTYPE_FILEIO_DELETEFILE:
            pHandler->DeleteFile(pPacketBuffer, pResponseBuffer);
            break;

        default:
            fHandled = FALSE;
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  CreateFile
 *
 *  Description:
 *      Creates or opens a file on the remote machine.
 *
 *  Arguments:
 *      LPCDSRBUFFER [in]: packet data.
 *      LPDSRBUFFER   [in/out]: response data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIoHandler::CreateFile"

void
CRemoteFileIoHandler::CreateFile
(
    LPCDSRBUFFER                    pPacket, 
    LPDSRBUFFER                     pResponse
)
{
    LPCDSRFILEIO_CREATEFILE_MSGDATA pPacketData     = (LPCDSRFILEIO_CREATEFILE_MSGDATA)pPacket->pvData;
    LPDSRFILEIO_CREATEFILE_RESPONSE pResponseData   = (LPDSRFILEIO_CREATEFILE_RESPONSE)pResponse->pvData;

    ASSERT(pPacket->dwSize >= sizeof(*pPacketData));
    ASSERT(pResponse->dwSize >= sizeof(*pResponseData));

    pResponseData->hFile = ::CreateFile(pPacketData->szFileName, pPacketData->dwDesiredAccess, pPacketData->dwShareMode, NULL, pPacketData->dwCreationDistribution, pPacketData->dwFlagsAndAttributes, NULL);

    if(IS_VALID_HANDLE_VALUE(pResponseData->hFile))
    {
        pResponseData->dwError = ERROR_SUCCESS;
    }
    else
    {
        pResponseData->dwError = GetLastError();

        DPF_ERROR("CreateFile failed with error %lu", pResponseData->dwError);
    }

    pResponse->dwSize = sizeof(*pResponseData);
}


/****************************************************************************
 *
 *  ReadFile
 *
 *  Description:
 *      Reads data from a file opened on the remote machine.
 *
 *  Arguments:
 *      LPCDSRBUFFER [in]: packet data.
 *      LPDSRBUFFER   [in/out]: response data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIoHandler::ReadFile"

void
CRemoteFileIoHandler::ReadFile
(
    LPCDSRBUFFER                    pPacket, 
    LPDSRBUFFER                     pResponse
)
{
    LPCDSRFILEIO_READFILE_MSGDATA   pPacketData     = (LPCDSRFILEIO_READFILE_MSGDATA)pPacket->pvData;
    LPDSRFILEIO_READFILE_RESPONSE   pResponseData   = (LPDSRFILEIO_READFILE_RESPONSE)pResponse->pvData;
    DWORD                           dwBytesRead;

    ASSERT(pPacket->dwSize >= sizeof(*pPacketData));
    ASSERT(pResponse->dwSize >= sizeof(*pResponseData));

    if(::ReadFile(pPacketData->hFile, pResponseData + 1, pResponse->dwSize - sizeof(*pResponseData), &dwBytesRead, NULL))
    {
        pResponseData->dwError = ERROR_SUCCESS;
        pResponse->dwSize = dwBytesRead + sizeof(*pResponseData);
    }
    else
    {
        pResponseData->dwError = GetLastError();
        pResponse->dwSize = sizeof(*pResponseData);

        DPF_ERROR("ReadFile failed with error %lu", pResponseData->dwError);
    }
}


/****************************************************************************
 *
 *  WriteFile
 *
 *  Description:
 *      Writes data to a file opened on the remote machine.
 *
 *  Arguments:
 *      LPCDSRBUFFER [in]: packet data.
 *      LPDSRBUFFER   [in/out]: response data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIoHandler::WriteFile"

void
CRemoteFileIoHandler::WriteFile
(
    LPCDSRBUFFER                    pPacket, 
    LPDSRBUFFER                     pResponse
)
{
    LPCDSRFILEIO_WRITEFILE_MSGDATA  pPacketData     = (LPCDSRFILEIO_WRITEFILE_MSGDATA)pPacket->pvData;
    LPDSRFILEIO_WRITEFILE_RESPONSE  pResponseData   = (LPDSRFILEIO_WRITEFILE_RESPONSE)pResponse->pvData;

    ASSERT(pPacket->dwSize >= sizeof(*pPacketData));
    ASSERT(pResponse->dwSize >= sizeof(*pResponseData));

    if(::WriteFile(pPacketData->hFile, pPacketData + 1, pPacket->dwSize - sizeof(*pPacketData), &pResponseData->dwBytesWritten, NULL))
    {
        pResponseData->dwError = ERROR_SUCCESS;
    }
    else
    {
        pResponseData->dwError = GetLastError();

        DPF_ERROR("WriteFile failed with error %lu", pResponseData->dwError);
    }

    pResponse->dwSize = sizeof(*pResponseData);
}


/****************************************************************************
 *
 *  SetFilePointer
 *
 *  Description:
 *      Writes data to a file opened on the remote machine.
 *
 *  Arguments:
 *      LPCDSRBUFFER [in]: packet data.
 *      LPDSRBUFFER   [in/out]: response data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIoHandler::SetFilePointer"

void
CRemoteFileIoHandler::SetFilePointer
(
    LPCDSRBUFFER                        pPacket, 
    LPDSRBUFFER                         pResponse
)
{
    LPCDSRFILEIO_SETFILEPOINTER_MSGDATA pPacketData     = (LPCDSRFILEIO_SETFILEPOINTER_MSGDATA)pPacket->pvData;
    LPDSRFILEIO_SETFILEPOINTER_RESPONSE pResponseData   = (LPDSRFILEIO_SETFILEPOINTER_RESPONSE)pResponse->pvData;

    ASSERT(pPacket->dwSize >= sizeof(*pPacketData));
    ASSERT(pResponse->dwSize >= sizeof(*pResponseData));

    pResponseData->dwAbsPosition = ::SetFilePointer(pPacketData->hFile, pPacketData->lDistanceToMove, NULL, pPacketData->dwMoveMethod);
    
    if(INVALID_SET_FILE_POINTER != pResponseData->dwAbsPosition)
    {
        pResponseData->dwError = ERROR_SUCCESS;
    }
    else
    {
        pResponseData->dwError = GetLastError();

        DPF_ERROR("SetFilePointer failed with error %lu", pResponseData->dwError);
    }

    pResponse->dwSize = sizeof(*pResponseData);
}


/****************************************************************************
 *
 *  SetEndOfFile
 *
 *  Description:
 *      Writes data to a file opened on the remote machine.
 *
 *  Arguments:
 *      LPCDSRBUFFER [in]: packet data.
 *      LPDSRBUFFER   [in/out]: response data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIoHandler::SetEndOfFile"

void
CRemoteFileIoHandler::SetEndOfFile
(
    LPCDSRBUFFER                        pPacket, 
    LPDSRBUFFER                         pResponse
)
{
    LPCDSRFILEIO_SETENDOFFILE_MSGDATA   pPacketData     = (LPCDSRFILEIO_SETENDOFFILE_MSGDATA)pPacket->pvData;
    LPDSRFILEIO_SETENDOFFILE_RESPONSE   pResponseData   = (LPDSRFILEIO_SETENDOFFILE_RESPONSE)pResponse->pvData;

    ASSERT(pPacket->dwSize >= sizeof(*pPacketData));
    ASSERT(pResponse->dwSize >= sizeof(*pResponseData));

    if(::SetEndOfFile(pPacketData->hFile))
    {
        pResponseData->dwError = ERROR_SUCCESS;
    }
    else
    {
        pResponseData->dwError = GetLastError();

        DPF_ERROR("SetEndOfFile failed with error %lu", pResponseData->dwError);
    }

    pResponse->dwSize = sizeof(*pResponseData);
}


/****************************************************************************
 *
 *  CloseHandle
 *
 *  Description:
 *      Writes data to a file opened on the remote machine.
 *
 *  Arguments:
 *      LPCDSRBUFFER [in]: packet data.
 *      LPDSRBUFFER   [in/out]: response data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIoHandler::CloseHandle"

void
CRemoteFileIoHandler::CloseHandle
(
    LPCDSRBUFFER                        pPacket, 
    LPDSRBUFFER                         pResponse
)
{
    LPCDSRFILEIO_CLOSEHANDLE_MSGDATA    pPacketData     = (LPCDSRFILEIO_CLOSEHANDLE_MSGDATA)pPacket->pvData;
    LPDSRFILEIO_CLOSEHANDLE_RESPONSE    pResponseData   = (LPDSRFILEIO_CLOSEHANDLE_RESPONSE)pResponse->pvData;

    ASSERT(pPacket->dwSize >= sizeof(*pPacketData));
    ASSERT(pResponse->dwSize >= sizeof(*pResponseData));

    if(::CloseHandle(pPacketData->hFile))
    {
        pResponseData->dwError = ERROR_SUCCESS;
    }
    else
    {
        pResponseData->dwError = GetLastError();

        DPF_ERROR("CloseHandle failed with error %lu", pResponseData->dwError);
    }

    pResponse->dwSize = sizeof(*pResponseData);
}


/****************************************************************************
 *
 *  DeleteFile
 *
 *  Description:
 *      Writes data to a file opened on the remote machine.
 *
 *  Arguments:
 *      LPCDSRBUFFER [in]: packet data.
 *      LPDSRBUFFER   [in/out]: response data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteFileIoHandler::DeleteFile"

void
CRemoteFileIoHandler::DeleteFile
(
    LPCDSRBUFFER                    pPacket, 
    LPDSRBUFFER                     pResponse
)
{
    LPCDSRFILEIO_DELETEFILE_MSGDATA pPacketData     = (LPCDSRFILEIO_DELETEFILE_MSGDATA)pPacket->pvData;
    LPDSRFILEIO_DELETEFILE_RESPONSE pResponseData   = (LPDSRFILEIO_DELETEFILE_RESPONSE)pResponse->pvData;

    ASSERT(pPacket->dwSize >= sizeof(*pPacketData));
    ASSERT(pResponse->dwSize >= sizeof(*pResponseData));

    if(::DeleteFile(pPacketData->szFileName))
    {
        pResponseData->dwError = ERROR_SUCCESS;
    }
    else
    {
        pResponseData->dwError = GetLastError();

        DPF_ERROR("DeleteFile failed with error %lu", pResponseData->dwError);
    }

    pResponse->dwSize = sizeof(*pResponseData);
}


