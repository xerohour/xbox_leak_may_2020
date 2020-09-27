/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    smcmessage.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

*/

#include "stdafx.h"
#include "xnetref.h"
#include "commontest.h"
#include "smcstress.h"

extern "C"
    {
    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN WriteWordValue,
        IN ULONG DataValue
        );
    }

static void WriteSMC(unsigned char addr, unsigned char value)
    {
    HalWriteSMBusValue(0x20, addr, FALSE, value);
    }

static DWORD ReadSMC(unsigned char addr)
    {
    DWORD value = 0xCCCCCCCC;
    HalReadSMBusValue(0x21, addr, FALSE, &value);
    return value;
    }

namespace SMCStress
    {
    extern char serverIP[32];
    extern char machineName[40];
    }

/*

Routine Description:

    Tests the HalWriteSMBusValue API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    DWORD - 

*/
DWORD SMCStress::Message_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );
    */
    xSetFunctionName(hLog, "HalWriteSMBusValue");
    DWORD time = GetTickCount();

    const unsigned buffLen = 2048;
    char post[256];
    char *data = new char[buffLen];
    
    if(!data)
        {
        return 0;
        }

    while(1)
        {
        bool state = false;
        SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
        SOCKADDR_IN dest;
        dest.sin_family = PF_INET;
        dest.sin_port = htons(80);
        dest.sin_addr.s_addr = inet_addr(serverIP);

        if(connect(sock, (SOCKADDR*)&dest, sizeof(SOCKADDR)) == SOCKET_ERROR)
            {
            DebugPrint("SMCSTRESS: connect error: (ec: %u)\n", GetLastError());
            }
        else
            {
            unsigned length, dataLen;

            length = sprintf(post, "machine=%s", machineName);
            dataLen = sprintf(data, 
                            "POST /smc/query.asp HTTP/1.1\r\n"
                            "Accept: */*\r\n"
                            "User-Agent: SMC\r\n"
                            "Connection: Close\r\n"
                            "Host: %s\r\n"
                            "Content-Length: %u\r\n"
                            "Content-Type: application/x-www-form-urlencoded\r\n"
                            "\r\n"
                            "%s",
                            serverIP,
                            length,
                            post);

            DWORD err = send(sock, data, dataLen, 0);

            err = recv(sock, data, buffLen, 0);
            data[err] = '\0';
            //OutputDebugStringA("\r\nHTTPA: ");
            //OutputDebugStringA(data);

            err = recv(sock, data, buffLen, 0);
            data[err] = '\0';
            //OutputDebugStringA("\r\nHTTPB: ");
            //OutputDebugStringA(data);

            if(strstr(data, "state=on"))
                {
                state = true;
                }
            else if(strstr(data, "stopstopstop"))
                {
                closesocket(sock);
                Sleep(INFINITE);
                break;
                }
            shutdown(sock, SD_BOTH);
            }
        closesocket(sock);

        if(state) WriteSMC(0x08, 0xF0);
        else  WriteSMC(0x08, 0x0F);

        WriteSMC(0x07, 0x01);
        Sleep(250);
        } // while(1)

    delete[] data;
    return 0;
    }

