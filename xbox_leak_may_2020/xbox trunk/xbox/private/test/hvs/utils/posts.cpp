/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    posts.cpp

Abstract:

    HVS Test Helper functions

Notes:

*****************************************************************************/

#include <stdio.h>
#include <xtl.h>
#include <xbeimage.h>
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

#include "hvsUtils.h"

static int POSTEncode(char *dest, const char *source);
static DWORD POST(const char *server, char *post, DWORD &dataLen, DWORD maxRecvSize);



/*****************************************************************************

Routine Description:

    PostResults

    Sends the DBDATA information as an HTTP POST to a web server where it
    will get entered into a database.

Arguments:

    char *url
    const DBDATA *results

Return Value:

    DWORD - error code

*****************************************************************************/
DWORD PostResults(const char *url, const DBDATA *results)
    {
    if(!url || !results) return 0;

    char *postData = new char[128*KILOBYTE];
    if(!postData) return 0;

    char fullURL[256];
    sprintf(fullURL, "%sxbox/postResults.asp", url);

    char *post = postData;

    XNADDR xnaddr;
    UINT iTimeout = 500;    // Five seconds maximum just in case
    memset(&xnaddr, 0, sizeof(xnaddr));
    while (XNetGetTitleXnAddr(&xnaddr) == 0 && iTimeout-- > 0) Sleep(10);

    post += sprintf(post, "macAddr=%02X-%02X-%02X-%02X-%02X-%02X", xnaddr.abEnet[0], xnaddr.abEnet[1], xnaddr.abEnet[2], xnaddr.abEnet[3], xnaddr.abEnet[4], xnaddr.abEnet[5]);

    post += sprintf(post, "&devType=");
    post += POSTEncode(post, results->deviceType);

    post += sprintf(post, "&mfg=");
    post += POSTEncode(post, results->manufacturer);

    post += sprintf(post, "&fw=");
    post += POSTEncode(post, results->firmwareVersion);

    post += sprintf(post, "&hw=");
    post += POSTEncode(post, results->hardwareVersion);

    post += sprintf(post, "&sn=");
    post += POSTEncode(post, results->serialNumber);

    post += sprintf(post, "&test=");
    post += POSTEncode(post, results->testName);

    post += sprintf(post, "&variation=");
    post += POSTEncode(post, results->variation);

    post += sprintf(post, "&status=%u", results->status);
    post += sprintf(post, "&numOps=%I64u", results->numOperations);

    post += sprintf(post, "&boxInfo=");
    post += POSTEncode(post, results->boxInformation);

    post += sprintf(post, "&testConfig=");
    post += POSTEncode(post, results->configSettings);

    post += sprintf(post, "&notes=");
    post += POSTEncode(post, results->notes);

    DWORD postLen = post-postData;

    DWORD err = POST(fullURL, postData, postLen, 128*KILOBYTE);
    //LogPrint("%s\n", postData);
    //LogPrint("****************************************************************************\n");
    //LogPrint("****************************************************************************\n");
    //LogPrint("****************************************************************************\n");
    //LogPrint("****************************************************************************\n");

    delete[] postData;
    return 0;
    }



/*****************************************************************************

Routine Description:

    GetAcceptanceCriteria

    Retrieves the percentage of acceptable failres for the given test

Arguments:

    char* testName

Return Value:

    double

*****************************************************************************/
double GetAcceptanceCriteria(const char *url, const char* testName)
    {
    if(!url || !testName) return 0.0;

    double result = 0.0;
    char *postData = new char[4*KILOBYTE];
    char *post = postData;

    char fullURL[256];
    sprintf(fullURL, "%sxbox/getCriteria.asp", url);

    post += sprintf(post, "test=");
    post += POSTEncode(post, testName);

    DWORD postLen = post-postData;
    DWORD err = POST(url, postData, postLen, 4*KILOBYTE);

    char *criteria = strstr(postData, "Criteria:");
    if(criteria)
        {
        criteria += 9;
        result = atof(criteria);
        }

    delete[] postData;
    return result;
    }


/*****************************************************************************

Routine Description:

    POSTEncode

    PRIVATE API

    Removes special characters and replaces them with HTTP escape codes

Arguments:

    char* dest
    char* source

Return Value:

    int

*****************************************************************************/
static int POSTEncode(char *dest, const char *source)
    {
    if(dest == source) return 0;
    if(!dest || !source) return 0;

    // NOTE: the restricted characters are defined in the URI RFC 
    //       (current ver: 2396) in section 2.

    char *restricted = ";/?:@&=+$,\"#%%<>\\~\r\n\t";
    char buff[10];

    int i;

    for(i=0; *source; source++, i++)
        {
        if(strchr(restricted, *source) != NULL)
            {
            sprintf(buff, "%02X", (unsigned)(*source));
            dest[i] = (char)'%%';    ++i;
            dest[i] = buff[0]; ++i;
            dest[i] = buff[1]; 
            }
        else if(*source == ' ')
            {
            dest[i] = '+';
            }
        else
            {
            dest[i] = *source;
            }
        }

    dest[i] = '\0';
    return i;
    }


/*****************************************************************************

Routine Description:

    POST

    PRIVATE API

    Sends an HTTP POST to a web server

Arguments:

    char* server - "http://ipaddr/url" (ipaddr must be an IP address)
    char* post - post data
    DWORD dataLen - length of the postData, on return will contain the length 
        of any data received.
    DWORD maxRecvSize maximum size of the post buffer, if 0 no data will
        be returned from the server.

Return Value:

    DWORD

*****************************************************************************/
static DWORD POST(const char *server, char *post, DWORD &dataLen, DWORD maxRecvSize)
    {
    //DebugPrint("POST(%s, data, %u, %u)\n", server, dataLen, maxRecvSize);

    if(strncmp(server, "http://", 7) != 0)
        return 0;

    char *data = new char[dataLen+512];
    if(!data)
        return 0;

    // parse out the ip address and the path/filename
    char serverIP[25];
    const char *url = strchr(server+8, '/');
    strncpy(serverIP, server+7, url - (server+7));
    serverIP[url - (server+7)] = '\0';

    // intiailize the socket
    SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN dest;
    dest.sin_family = PF_INET;
    dest.sin_port = htons(80);
    dest.sin_addr.s_addr = inet_addr(serverIP);

    if(connect(sock, (SOCKADDR*)&dest, sizeof(SOCKADDR)) == SOCKET_ERROR)
        {
        DebugPrint("HVS: connect error: (ec: %u)\n", GetLastError());
        dataLen = 0;
        }
    else
        {
        unsigned postLen;

        // turn off the time wait delay after closing the socket
        linger noTimeWait = { true, 0 };
        setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&noTimeWait, sizeof(noTimeWait));

        postLen = sprintf(data,
                        "POST %s HTTP/1.1\r\n"
                        "Host: %s\r\n"
                        "Accept: */*\r\n"
                        "User-Agent: HVS Test\r\n"
                        "TitleID: 0x%08X\r\n"
                        "Connection: Close\r\n"
                        "Content-Length: %u\r\n"
                        "Content-Type: application/x-www-form-urlencoded\r\n"
                        "\r\n"
                        "%s",
                        url,
                        serverIP,
                        XeImageHeader()->Certificate->TitleID,
                        dataLen,
                        post);

        //
        // send the post
        //
        //LogPrint("****************************************************************************\n");
        //LogPrint("****************************************************************************\n");
        //LogPrint("****************************************************************************\n");
        //LogPrint("****************************************************************************\n");
        //LogPrint("%s", data);
        DWORD err = send(sock, data, postLen, 0);

        //
        // try to receive data (if necessary)
        //
        if(maxRecvSize)
            {
            dataLen = recv(sock, post, maxRecvSize, 0);
            post[dataLen] = '\0';
            if(strstr(post, "100 Continue"))
                {
                dataLen = recv(sock, post, maxRecvSize, 0);
                post[dataLen] = '\0';
                }
            }
        else
            {
            // caller doesnt want any data from the server
            dataLen = 0;
            }

        shutdown(sock, SD_BOTH);
        }

    delete[] data;
    closesocket(sock);

    return 0;
    }