/***************************************************************************
 *
 *  Copyright (C) 1/8/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       win32.cpp
 *  Content:    insert content description here.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/8/2002   dereks  Created.
 *
 ****************************************************************************/

#include "dsremote.h"
#include <xboxdbg.h>
#include <stdio.h>

EXTERN_C DWORD g_dwDirectSoundDebugLevel;

#define ASSERT(exp) \
    { \
        if(!(exp)) \
        { \
            __asm int 3 \
        } \
    }

//
// Major packet types
//

enum
{
    DSRTYPE_TEST = 'tset',
};

//
// Minor packet types
//

enum
{
    DSRTYPE_TEST_STRING = 0,
    DSRTYPE_TEST_COUNT
};

//
// Message handler callback
//

class CTestMsgHandler
{
public:
    enum
    {
        DSRTYPE = DSRTYPE_TEST
    };

public:
    // Handler registration
    HRESULT Register(LPREMOTECONNECTION pConnection);
   
protected:
    // Specific handler functions
    void ReceiveString(LPCSTR pszString, LPSTR pszResponseString, LPDWORD pdwResponseSize);

private:
    // Handler function
    static BOOL STDMETHODCALLTYPE Receive(LPCDSRPACKET pPacket, LPCDSRBUFFER pPacketBuffer, LPDSRBUFFER pResponseBuffer, LPVOID pvContext);
};


/****************************************************************************
 *
 *  Register
 *
 *  Description:
 *      Registers the handler with a connection object.
 *
 *  Arguments:
 *      LPREMOTECONNECTION [in]: connection object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTestMsgHandler;:Register"

HRESULT
CTestMsgHandler::Register
(
    LPREMOTECONNECTION      pConnection
)
{
    DSREMOTEMSGHANDLER      VTable;

    ZeroMemory(&VTable, sizeof(VTable));

    VTable.Receive = Receive;

    return pConnection->RegisterMsgHandler(DSRTYPE, &VTable, this);
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
 *      LPDSRBUFFER [in/out]: response data.
 *      LPVOID [in]: callback context.
 *
 *  Returns:  
 *      BOOL: TRUE if the message was handled.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTestMsgHandler;:Receive"

BOOL
CTestMsgHandler::Receive
(
    LPCDSRPACKET            pPacket, 
    LPCDSRBUFFER            pPacketBuffer,
    LPDSRBUFFER             pResponseBuffer,
    LPVOID                  pvContext
)
{
    CTestMsgHandler *       pHandler    = (CTestMsgHandler *)pvContext;
    BOOL                    fHandled    = TRUE;
    
    switch(pPacket->dwMinorType)
    {
        case DSRTYPE_TEST_STRING:
            pHandler->ReceiveString((LPCSTR)pPacketBuffer->pvData, (LPSTR)pResponseBuffer->pvData, &pResponseBuffer->dwSize);
            break;

        default:
            fHandled = FALSE;
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  ReceiveString
 *
 *  Description:
 *      Called when data is received.
 *
 *  Arguments:
 *      LPCSTR [in]: input string.
 *      LPSTR [out]: response string.
 *      LPDWORD [out]: response string length, in bytes.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTestMsgHandler;:ReceiveString"

void
CTestMsgHandler::ReceiveString
(
    LPCSTR                  pszString, 
    LPSTR                   pszResponseString, 
    LPDWORD                 pdwResponseSize
)
{
    static DWORD            dwResponseIndex     = 0;
    CHAR                    szResponse[0x100];
    DWORD                   dwLength;

    ASSERT(pszString);
    
    OutputDebugStringA("Received string:  ");
    OutputDebugStringA(pszString);
    OutputDebugStringA("\n");

    if(pszResponseString && pdwResponseSize)
    {
        dwLength = sprintf(szResponse, "Response %lu", dwResponseIndex++) + 1;

        if(dwLength <= *pdwResponseSize)
        {
            strcpy(pszResponseString, szResponse);
            *pdwResponseSize = dwLength;
        }
        else
        {
            *pdwResponseSize = 0;
        }
    }
}


/****************************************************************************
 *
 *  TestSend
 *
 *  Description:
 *      Tests basic transfer of data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestSend"

void
TestSend
(
    void
)
{

#ifdef _XBOX

    static const DWORD      dwFlags             = 0;

#else // _XBOX

    static const DWORD      dwFlags             = DSREMOTE_CONNECT_DNSLOOKUP;

#endif // _XBOX

    static const DWORD      dwSendCount         = 10;
    static const DWORD      dwStringRepeat      = 0x100;
    static const LPCSTR     pszStringTemplate   = "%8.8lX\t";
    static const DWORD      dwStringSize        = 9;
    static const DWORD      dwSendSize          = (dwStringSize * dwStringRepeat) + 1;
    IRemoteConnection *     pConnection         = NULL;
    LPVOID                  pvBuffer            = NULL;
    CTestMsgHandler         MsgHandler;
    DSRPACKET               Packet;
    DSRBUFFER               PacketBuffer;
    DSRBUFFER               ResponseBuffer;
    HRESULT                 hr;
    DWORD                   a, b;

    hr = CreateRemoteConnection(DSREMOTE_DEFAULT_PORT, NULL, NULL, &pConnection);

    if(SUCCEEDED(hr))
    {
        hr = pConnection->Connect(NULL, dwFlags);
    }

    if(SUCCEEDED(hr))
    {
        hr = MsgHandler.Register(pConnection);
    }

    if(SUCCEEDED(hr))
    {
        if(!(pvBuffer = LocalAlloc(LPTR, dwSendSize)))
        {
            hr = E_OUTOFMEMORY;
        }
    }

// #ifndef _XBOX

    if(SUCCEEDED(hr))
    {
        for(a = 0; (a < dwSendCount) && SUCCEEDED(hr); a++)
        {
            for(b = 0; b < dwStringRepeat; b++)
            {
                sprintf((LPSTR)pvBuffer + (dwStringSize * b), pszStringTemplate, (dwStringRepeat * a) + b);
            }
        
            Packet.dwMajorType = DSRTYPE_TEST;
            Packet.dwMinorType = DSRTYPE_TEST_STRING;
            Packet.dwFlags = 0;

            PacketBuffer.dwSize = dwSendSize;
            PacketBuffer.pvData = pvBuffer;

            ResponseBuffer.dwSize = dwSendSize;
            ResponseBuffer.pvData = pvBuffer;

            hr = pConnection->Send(&Packet, 1, &PacketBuffer, &ResponseBuffer);

            if(SUCCEEDED(hr))
            {
                OutputDebugStringA("Received response:  ");
                
                if(ResponseBuffer.dwSize)
                {
                    OutputDebugStringA((LPCSTR)pvBuffer);
                }
                else
                {
                    OutputDebugStringA("(null)");
                }

                OutputDebugStringA("\n");
            }        
        }
    }

// #endif // _XBOX

    while(SUCCEEDED(hr))
    {
        pConnection->DoWork();
    }

    if(pConnection)
    {
        pConnection->Release();
    }

    if(pvBuffer)
    {
        LocalFree(pvBuffer);
    }
}


/****************************************************************************
 *
 *  TestFileIo
 *
 *  Description:
 *      Tests remote file i/o.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestFileIo"

void
TestFileIo
(
    void
)
{

#ifdef _XBOX

    static const DWORD      dwFlags         = 0;

#else // _XBOX

    static const DWORD      dwFlags         = DSREMOTE_CONNECT_DNSLOOKUP;
    static const LPCSTR     pszSourceFile   = "c:\\winnt\\regedit.exe";
    static const LPCSTR     pszRemoteFile   = "t:\\regedit.exe";

#endif // _XBOX

    IRemoteConnection *     pConnection     = NULL;
    IRemoteFileIo *         pFileIo         = NULL;
    HRESULT                 hr;

    hr = CreateRemoteConnection(DSREMOTE_DEFAULT_PORT, NULL, NULL, &pConnection);

    if(SUCCEEDED(hr))
    {
        hr = pConnection->Connect(NULL, dwFlags);
    }

#ifndef _XBOX

    if(SUCCEEDED(hr))
    {
        hr = CreateRemoteFileIo(pConnection, &pFileIo);
    }

    if(SUCCEEDED(hr))
    {
        hr = pFileIo->CopyFile(pszSourceFile, pszRemoteFile, FALSE);
    }

    if(pFileIo)
    {
        pFileIo->Release();
    }

#endif // _XBOX

    while(SUCCEEDED(hr))
    {
        pConnection->DoWork();
    }

    if(pConnection)
    {
        pConnection->Release();
    }
}


/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      Application entry point.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "main"

void __cdecl
main
(
    void
)
{
    g_dwDirectSoundDebugLevel = 4;

    // TestSend();
    TestFileIo();
}


