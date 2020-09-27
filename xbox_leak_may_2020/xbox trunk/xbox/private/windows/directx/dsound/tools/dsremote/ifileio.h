/***************************************************************************
 *
 *  Copyright (C) 1/18/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ifileio.h
 *  Content:    IRemoteFileIo implementation.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/18/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __IFILEIO_H__
#define __IFILEIO_H__

//
// Remote file I/O message types
//

enum
{
    DSRTYPE_FILEIO_CREATEFILE = 0,
    DSRTYPE_FILEIO_READFILE,
    DSRTYPE_FILEIO_WRITEFILE,
    DSRTYPE_FILEIO_SETFILEPOINTER,
    DSRTYPE_FILEIO_SETENDOFFILE,
    DSRTYPE_FILEIO_CLOSEHANDLE,
    DSRTYPE_FILEIO_DELETEFILE,
};

BEGIN_DEFINE_STRUCT()
    CHAR        szFileName[MAX_PATH];
    DWORD       dwDesiredAccess;
    DWORD       dwShareMode;
    DWORD       dwCreationDistribution;
    DWORD       dwFlagsAndAttributes;
END_DEFINE_STRUCT(DSRFILEIO_CREATEFILE_MSGDATA);

BEGIN_DEFINE_STRUCT()
    DWORD       dwError;
    HANDLE      hFile;
END_DEFINE_STRUCT(DSRFILEIO_CREATEFILE_RESPONSE);

BEGIN_DEFINE_STRUCT()
    HANDLE      hFile;
END_DEFINE_STRUCT(DSRFILEIO_READFILE_MSGDATA);

BEGIN_DEFINE_STRUCT()
    DWORD       dwError;
END_DEFINE_STRUCT(DSRFILEIO_READFILE_RESPONSE);

BEGIN_DEFINE_STRUCT()
    HANDLE      hFile;
END_DEFINE_STRUCT(DSRFILEIO_WRITEFILE_MSGDATA);

BEGIN_DEFINE_STRUCT()
    DWORD       dwError;
    DWORD       dwBytesWritten;
END_DEFINE_STRUCT(DSRFILEIO_WRITEFILE_RESPONSE);

BEGIN_DEFINE_STRUCT()
    HANDLE      hFile;
    LONG        lDistanceToMove;
    DWORD       dwMoveMethod;
END_DEFINE_STRUCT(DSRFILEIO_SETFILEPOINTER_MSGDATA);

BEGIN_DEFINE_STRUCT()
    DWORD       dwError;
    DWORD       dwAbsPosition;
END_DEFINE_STRUCT(DSRFILEIO_SETFILEPOINTER_RESPONSE);

BEGIN_DEFINE_STRUCT()
    HANDLE      hFile;
END_DEFINE_STRUCT(DSRFILEIO_SETENDOFFILE_MSGDATA);

BEGIN_DEFINE_STRUCT()
    DWORD       dwError;
END_DEFINE_STRUCT(DSRFILEIO_SETENDOFFILE_RESPONSE);

BEGIN_DEFINE_STRUCT()
    HANDLE      hFile;
END_DEFINE_STRUCT(DSRFILEIO_CLOSEHANDLE_MSGDATA);

BEGIN_DEFINE_STRUCT()
    DWORD       dwError;
END_DEFINE_STRUCT(DSRFILEIO_CLOSEHANDLE_RESPONSE);

BEGIN_DEFINE_STRUCT()
    CHAR        szFileName[MAX_PATH];
END_DEFINE_STRUCT(DSRFILEIO_DELETEFILE_MSGDATA);

BEGIN_DEFINE_STRUCT()
    DWORD       dwError;
END_DEFINE_STRUCT(DSRFILEIO_DELETEFILE_RESPONSE);

#ifdef __cplusplus

//
// IRemoteFileIo implementation
//

namespace DSREMOTE
{
    class CRemoteFileIo
        : public IRemoteFileIo, public CRefCount
    {
    protected:
        LPREMOTECONNECTION      m_pConnection;      // Remote connection object

    public:
        CRemoteFileIo(LPREMOTECONNECTION pConnection);
        virtual ~CRemoteFileIo(void);

    public:
        // IUnknown methods
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE Release(void);

        // IRemoteFileIo methods
        virtual HRESULT STDMETHODCALLTYPE CreateFile(LPCSTR pszRemoteFile, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDistribution, DWORD dwFlagsAndAttributes, LPHANDLE phFile);
        virtual HRESULT STDMETHODCALLTYPE ReadFile(HANDLE hFile, LPVOID pvBuffer, DWORD dwBufferSize, LPDWORD pdwBytesRead);
        virtual HRESULT STDMETHODCALLTYPE WriteFile(HANDLE hFile, LPCVOID pvBuffer, DWORD dwBufferSize, LPDWORD pdwBytesWritten);
        virtual HRESULT STDMETHODCALLTYPE SetFilePointer(HANDLE hFile, LONG lDistanceToMove, DWORD dwMoveMethod, LPDWORD pdwAbsPosition);
        virtual HRESULT STDMETHODCALLTYPE SetEndOfFile(HANDLE hFile);
        virtual HRESULT STDMETHODCALLTYPE CloseHandle(HANDLE hFile);
        virtual HRESULT STDMETHODCALLTYPE CopyFile(LPCSTR pszLocalFile, LPCSTR pszRemoteFile, BOOL fFailIfFileExists);
        virtual HRESULT STDMETHODCALLTYPE DeleteFile(LPCSTR pszRemoteFile);
    };

    __inline ULONG CRemoteFileIo::AddRef(void)
    {
        return CRefCount::AddRef();
    }

    __inline ULONG CRemoteFileIo::Release(void)
    {
        return CRefCount::Release();
    }
}

//
// Remote file I/O message handler
//

namespace DSREMOTE
{
    class CRemoteFileIoHandler
    {
    public:
        enum
        {
            DSRTYPE = DSRTYPE_FILEIO
        };

    public:
        // Handler registration
        HRESULT Register(LPREMOTECONNECTION pConnection);

    protected:
        // Specific handler functions
        void CreateFile(LPCDSRBUFFER pPacket, LPDSRBUFFER pResponse);
        void ReadFile(LPCDSRBUFFER pPacket, LPDSRBUFFER pResponse);
        void WriteFile(LPCDSRBUFFER pPacket, LPDSRBUFFER pResponse);
        void SetFilePointer(LPCDSRBUFFER pPacket, LPDSRBUFFER pResponse);
        void SetEndOfFile(LPCDSRBUFFER pPacket, LPDSRBUFFER pResponse);
        void CloseHandle(LPCDSRBUFFER pPacket, LPDSRBUFFER pResponse);
        void DeleteFile(LPCDSRBUFFER pPacket, LPDSRBUFFER pResponse);

    protected:
        // Generic handler function
        static BOOL STDMETHODCALLTYPE Receive(LPCDSRPACKET pPacketHeader, LPCDSRBUFFER pPacketBuffer, LPDSRBUFFER pResponseBuffer, LPVOID pvContext);
    };    

    __inline HRESULT CRemoteFileIoHandler::Register(LPREMOTECONNECTION pConnection)
    {
        DSREMOTEMSGHANDLER      VTable;

        ZeroMemory(&VTable, sizeof(VTable));

        VTable.Receive = Receive;

        return pConnection->RegisterMsgHandler(DSRTYPE, &VTable, this);
    }
}

#endif // __cplusplus

#endif // __IFILEIO_H__
