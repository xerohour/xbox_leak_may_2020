/*

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    HttpClient.cpp

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

NOTE: Include files with error codes:
    SSL       <winerror.h> 
    Sockets   <winsock2.h>
    HTTP      <wininet.h>

*/

#include "stdafx.h"
#include <time.h>

#include "SecureSocketLayer.h"
#include "HttpClient.h"
#include <mswsock.h>

// timeout in seconds + msec
#define RECV_TIMEOUT_S  2
#define RECV_TIMEOUT_US 0

#define DEFAULT_PORT 80
#define DEFAULT_PROXY_PORT DEFAULT_PORT
#define DEFAULT_SSL_PORT 443

extern int followDomainRules;

#ifdef _DEBUG
    #define DEBUGLOG "HttpClient.log"
    // NOTE: There is no critical section around the logging.
    //       Keep this in mind when viewing the log from a
    //       multithreaded app.
    // NOTE: You can #define _DEBUG_RAW_DUMP to dump raw information
    //       to the log file. (Hex output is default).

    void Report(LPCTSTR lpszFormat, ...);
    void PrintHexDump(const char * buffer, DWORD length);
#endif

/*/////////////////////////////////////////////////////////////////////////////
Routine Description:


Arguments:


Return value:

*/
CHttpClient::CHttpClient()
    {
    // windows socket initialization
    sock = INVALID_SOCKET;
    nextsock = INVALID_SOCKET;

    wPort = DEFAULT_PORT;
    dataLen = 0;
    dest.sin_family = PF_INET;

    // Proxy
    wProxyPort = DEFAULT_PROXY_PORT;
    bUseProxy = FALSE;
    szProxyServerName[0] = '\0';

    // SSL
    SSL = NULL;

    // timeouts
    timeout.tv_sec = RECV_TIMEOUT_S;
    timeout.tv_usec = RECV_TIMEOUT_US;

    // timers
    sendTime = 0;
    ttfbTime = 0;
    ttlbTime = 0;
    }

CHttpClient::~CHttpClient()
    {
    if(sock != INVALID_SOCKET) Disconnect();
    if(nextsock != INVALID_SOCKET)
        {
        closesocket(nextsock);
        nextsock = INVALID_SOCKET;
        }

    if(SSL) delete SSL;
    }

long CHttpClient::Open(void)
    {
    #ifdef _DEBUG
    Report("Sock Opened\n");
    #endif

    wPort = DEFAULT_PORT;
    dest.sin_family = PF_INET;

    // Proxy
    wProxyPort = DEFAULT_PROXY_PORT;
    bUseProxy = FALSE;

    // SSL
    if(SSL) 
        {
        delete SSL;
        SSL = NULL;
        }

    if(nextsock != INVALID_SOCKET)
        {
        sock = nextsock;
        nextsock = INVALID_SOCKET;
        }
    else
        {
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if(sock == INVALID_SOCKET) return (long)WSAGetLastError();
        }

    return 0;
    }

int CHttpClient::GetHTTPStatus(void)
{
    char *beginning = strstr(data, "HTTP/");
    if(!beginning) return 0;
    beginning += 9;
    int res = atoi(beginning);
    // if first HTTP status found is 100, look if there is another one
    // IIS 5 sends HTTP 100 and then actual data
    if (res == 100) 
    {
        char *nextcode = strstr(beginning, "HTTP/");
        if (nextcode) 
        {
            nextcode += 9;
            res = atoi(nextcode);
        }
    }
    return res;
}

char* CHttpClient::GetBody(void)
    {
    char *body = strstr(data, "\r\n\r\n");
    if(!body) return NULL;
    return body + 4;
    }

inline char* CHttpClient::GetData(void)
    {
    return data;
    }

inline int CHttpClient::GetDataLen(void)
    {
    return dataLen;
    }

long CHttpClient::Connect(IN_ADDR server, const char *serverName)
    {
    // NOTE: if connecting through a proxy, 'server' must be the address of 
    //       the proxy, and 'serverName' the final destination
    if(!serverName) return WSAHOST_NOT_FOUND;

    dest.sin_port = htons( (bUseProxy ? wProxyPort : wPort) );
    dest.sin_addr.s_addr = server.s_addr;

    if(connect(sock, (SOCKADDR*)&dest, sizeof(SOCKADDR)) == SOCKET_ERROR)
        {
        return (long)WSAGetLastError();
        }

    if(bUseProxy)
        {
        // Build message for proxy server
        dataLen = sprintf(data, "CONNECT %s:%u %s", serverName, wPort, "HTTP/1.0\r\nUser-Agent: CHttpClient\r\n\r\n");
        if(send(sock, data, dataLen, 0) == SOCKET_ERROR)
            {
            return (long)WSAGetLastError();
            }
        do
            {
            if((dataLen = recv(sock, data, DATA_SIZE, 0)) == SOCKET_ERROR)
                {
                data[dataLen = 0] = '\0';
                return (long)WSAGetLastError();
                }
            data[dataLen] = '\0';
            } while(strstr(data, "\r\n\r\n") == NULL);
        }

    if(SSL)
        {
        long status = SSL->Connect(sock, serverName);
        return status;
        }

    return 0;
    }

long CHttpClient::Connect(const char *serverName)
    {
    if(!serverName) return WSAHOST_NOT_FOUND;

    #ifdef _DEBUG
    Report("Connect %s:%u\n", serverName, wPort);
    #endif

    IN_ADDR address;

    // try to treat serverName as a dotted decimal
    if(bUseProxy) address.s_addr = inet_addr(szProxyServerName);
    else address.s_addr = inet_addr(serverName);

    if(address.s_addr == INADDR_NONE)
        {
        // not dotted decimal, so try to get an ip for it
        HOSTENT *hosts;
        if(bUseProxy) hosts = gethostbyname(szProxyServerName);
        else hosts = gethostbyname(serverName);
        if(!hosts) return (long)WSAGetLastError();

        // take the first address found
        address.s_addr = **(LPDWORD*)&hosts->h_addr_list[0];
        }

    return Connect(address, serverName);
    }

long CHttpClient::Disconnect(void)
    {
    #ifdef _DEBUG
    Report("Disconnect\n");
    #endif

    if(sock == INVALID_SOCKET) return 0;

    // SSL clean up
    if(SSL)
        {
        SSL->Disconnect(sock);
        }

    int err;
    shutdown(sock, SD_BOTH);

    /* NOTE: Based on the "polite" way to close a connection, we
             should call recv untill no more data is available
             (timeout), but due to performance reasons, we will 
             not do this (or we leave it up to the application).
             On the above line, set the SD_BOTH to SD_SEND, if 
             you wish to use the below code.

    // clean out the receive buffer
    char tempdata[DATA_SIZE+1];
    for(int i=0; i<100; i++) // just quit after a while
        {
        if(!IsDataAvailable()) break;
        err = recv(sock, tempdata, DATA_SIZE, 0);
        if(err == 0 || err == SOCKET_ERROR) break;
        }
    */

    err = closesocket(sock);
    sock = INVALID_SOCKET;

    if(err) return (long)WSAGetLastError();
    return 0;
    }

long CHttpClient::HardDisconnect(void)
    {
    #ifdef _DEBUG
    Report("HardDisconnect\n");
    #endif

    if(sock == INVALID_SOCKET) return 0;

    // SSL clean up
    if(SSL)
        {
        SSL->Disconnect(sock);
        }

    int err = closesocket(sock);
    sock = INVALID_SOCKET;

    if(err) return (long)WSAGetLastError();
    return 0;
    }

long CHttpClient::Send(const char *senddata, int len)
    {
    int err;

    // Set our timers back to 0
    sendTime = 0;
    ttfbTime = 0;
    ttlbTime = 0;

    #ifdef _DEBUG
    Report("Send\n");
    #ifdef _DEBUG_DUMP
    PrintHexDump(senddata, len);
    #endif
    #endif

    if(len <= 0) return WSAEMSGSIZE;
    if(!senddata) return WSAEFAULT;

    if(SSL)
        {
        long status;
        memcpy(data, senddata, len);
        DWORD dLen = len;
        status = SSL->Encrypt(data, dLen, DATA_SIZE);
        dataLen = (int)dLen;
        if(FAILED(status))
            {
            return status;
            }
        err = send(sock, data, dataLen, 0);
        }
    else
        {
        err = send(sock, senddata, len, 0);
        }

    if(err == SOCKET_ERROR)
        {
        return (long)WSAGetLastError();
        }

    sendTime = time(NULL);
    return 0;
    }

long CHttpClient::Send(void)
    {
    int err;

   // Set our timers back to 0
    sendTime = 0;
    ttfbTime = 0;
    ttlbTime = 0;

    #ifdef _DEBUG
    Report("Send\n");
    #ifdef _DEBUG_DUMP
    PrintHexDump(data, dataLen);
    #endif
    #endif

    if(dataLen <= 0) return WSAEMSGSIZE;

    if(SSL)
        {
        long status;
        DWORD dLen = dataLen;
        status = SSL->Encrypt(data, dLen, DATA_SIZE);
        dataLen = (int)dLen;
        if(FAILED(status))
            {
            return status;
            }
        err = send(sock, data, dataLen, 0);
        }
    else
        {
        err = send(sock, data, dataLen, 0);
        }

    if(err == SOCKET_ERROR)
        {
        return (long)WSAGetLastError();
        }

    sendTime = time(NULL);

    return 0;
    }

long CHttpClient::Receive(BOOL readAll/*=FALSE*/)
    {
    long err=0;
    DWORD dLen;
    dataLen = 0;
    //DWORD contentLength = 0; // length of data according to the received header
    //DWORD headerSize = 0;

    #ifdef _DEBUG
    Report("Receive:\n");
    #endif

    DWORD count = 0;
    const DWORD maxCount = 500;
    
    data[0] = '\0';
    dataLen = 0;

    receive:
    do
        {
        if(++count >= maxCount)
            {
            err = WSAECONNABORTED;
            break;
            }

        // check for available data
	    if(!IsDataAvailable())
            {
            data[dataLen] = '\0';
            err = WSAETIMEDOUT;
            break;
            }

        // retreive the data
        if((dLen = recv(sock, data+dataLen, DATA_SIZE-dataLen, 0)) == SOCKET_ERROR)
            {
            data[dataLen] = '\0';
            err = WSAGetLastError();
            break;
            }
        
        if (!ttfbTime)
            {
            ttfbTime = time(NULL);
            }

        if(dLen == 0) break; // no more data

        /*  This code is commented out because we would want to do this
            but because we un SSL after we get everything it wont work.

        if(contentLength == 0)
            {
            char *t = strstr(data, "Content-Length:");
            if(t)
                {
                contentLength = atol(t + strlen("Content-Length:"));
                //FILE *f = fopen("c:\\test.txt", "a+");
                //fprintf(f, "content length = %d\n", contentLength);
                //fclose(f);

                t = strstr(t, "\r\n\r\n");
                headerSize = (t+4) - data;
                }
            }
            */

        dataLen += dLen;
        data[dataLen] = '\0';

        /*  See above note 

        // reached end of body
        if(contentLength && dataLen-headerSize >= contentLength)
            {
                //FILE *f = fopen("c:\\test.txt", "a+");
                //fprintf(f, "reached end of body\n", contentLength);
                //fclose(f);
            break;
            }
            */

        } while(readAll);

    if(SSL)
    {
        dLen = (DWORD)dataLen;
        // antonpav - add dLen check
        // in case recv() returns zero length
        // Decrypt() with zero len data results in 
        // SEC_E_INCOMPLETE_MESSAGE, and loops to receive label
        if (dLen != 0)
        {
            long status = SSL->Decrypt(data, dLen, DATA_SIZE);
            if(status == SEC_I_RENEGOTIATE)
                {
                status = SSL->ClientHandshakeLoop(sock, FALSE);
                err = status;
                }
            else if(status == SEC_E_INCOMPLETE_MESSAGE)
                {
                // We need to read more from the server before we can decrypt
                if(count < maxCount) goto receive;
                }
            if(FAILED(status))
                {
                err = status;
                }
            dataLen = (int)dLen;
        }
    }

    data[dataLen] = '\0';
    
    if (!ttfbTime)
        {
        sendTime = 0;
        }
    else
        {
        ttlbTime = time(NULL);
        }

    #ifdef _DEBUG_DUMP
    PrintHexDump(data, dataLen);
    #endif

    return err;
    }

BOOL CHttpClient::IsDataAvailable(void)
    {
    FD_SET bucket;
    bucket.fd_count = 1;
    bucket.fd_array[0] = sock;

    // do some work for the next connection before waiting
    if(nextsock == INVALID_SOCKET)
        {
        nextsock = socket(PF_INET, SOCK_STREAM, 0);
        }

    int err = select(0, &bucket, NULL, NULL, &timeout);
    if(err == 0 || err == SOCKET_ERROR)
        {
        return FALSE;
        }

    return TRUE;
    }

BOOL CHttpClient::IsSendAvailable(void)
    {
    TIMEVAL timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 20;

    FD_SET bucket;
    bucket.fd_count = 1;
    bucket.fd_array[0] = sock;

    if(select(0, NULL, &bucket, NULL, &timeout) == 1)
        {
        return TRUE;
        }

    return FALSE;
    }

long CHttpClient::SetLinger(BOOL linger, WORD timeout)
    {
    LINGER ling;
    int size = sizeof(LINGER);

    #ifdef _DEBUG
    Report("Linger Enabled\n");
    #endif

    ling.l_onoff = (WORD)linger;
    ling.l_linger = timeout;
    int err;
    if((err = setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&ling, size)) == SOCKET_ERROR)
        {
        return (long)WSAGetLastError();
        }

    return 0;
    }

long CHttpClient::SetKeepAlive(BOOL keepalive)
    {
    int err;
    int val = keepalive;

    #ifdef _DEBUG
    Report("KeepAlive Enabled\n");
    #endif

    if((err = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(int))) == SOCKET_ERROR)
        {
        return (long)WSAGetLastError();
        }

    return 0;
    }

long CHttpClient::SetSSL(LPSTR pszUserName /* =NULL */, DWORD dwProtocol /* =SP_PROT_SSL2 */, ALG_ID  aiKeyExch /* =CALG_RSA_KEYX */)
    {
    #ifdef _DEBUG
    Report("SSL Enabled\n");
    #endif

    wPort = DEFAULT_SSL_PORT;

    // available encryption type values
    //    dwProtocol = SP_PROT_PCT1;
    //    dwProtocol = SP_PROT_SSL2;
    //    dwProtocol = SP_PROT_SSL3;
    //    dwProtocol = SP_PROT_TLS1;
    //    aiKeyExch = CALG_RSA_KEYX;
    //    aiKeyExch = CALG_DH_EPHEM;

    if(!SSL) SSL = new CSecureSocketLayer(pszUserName, dwProtocol, aiKeyExch);
    if(!SSL) return SEC_E_INSUFFICIENT_MEMORY;

    return 0;
    }

long CHttpClient::SetProxy(const char *serverName, WORD port)
    {
    #ifdef _DEBUG
    Report("Proxy Enabled\n");
    #endif

    bUseProxy = TRUE;
    strcpy(szProxyServerName, serverName);
    wProxyPort = port;

    return 0;
    }

void CHttpClient::SetRecvTimeout(UINT sec, UINT usec)
    {
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    }

void CHttpClient::SetDefaultPort(WORD newPort)
    {
    wPort = newPort;
    }

int CHttpClient::ParseURL(const char *url, char *server, char *site, BOOL* ssl)
    {
    // TODO try to recover on malformed urls
    if(!url || !server || !site || !ssl) return 0;

    // Check for SSL
    url += 4;
    if(*url == 's' || *url == 'S')
        {
        *ssl = 1;
        url += 4;
        }
    else if(*url == ':')
        {
        *ssl = 0;
        url += 3;
        }
    else return 0;

    // copy the server portion
    while(*url != '/')
        {
        if(!(*url))
            {
            // no site, so we tack on an ending /
            *server = '\0';
            site[0] = '/';
            site[1] = '\0';
            return 0;
            }
        *server++ = *url++;
        }

    // copy the path/page portion
    *server = '\0';
    strcpy(site, url);

    return 1;
    }

int CHttpClient::GrabCookies(Cookie *jar, char *source, char *domain /*=NULL*/)
    {
    if(!jar || !source) return 0;

    int numFound = 0;
    char *cookiestart;

    cookiestart = (char*)source;

    // antonpav
    // should search through the entire response as 
    // we may have multiple replies in one buffer
//    char *body = strstr(data, "\r\n\r\n");
//    if(body) body[0] = '\0'; // just so we dont search through the entire response

    while((cookiestart = strstr(cookiestart, "Set-Cookie:")) != NULL)
        {
        cookiestart += sizeof("Set-Cookie:");
        jar->Add(cookiestart, domain);

        ++numFound;
        }

//    if(body) body[0] = '\r';

    return numFound;
    }

int CHttpClient::GetCookieRules(void)
    {
    return followDomainRules;
    }

void CHttpClient::SetCookieRules(int i)
    {
    followDomainRules = i;
    }

int CHttpClient::POSTEncode(char *dest, const char *source)
    {
    if(dest == source) return 0;

    // NOTE: the restricted characters are defined in the URI RFC 
    //       (current ver: 2396) in section 2.

    char *restricted = ";/?:@&=+$,\"#%%<>\\~";
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

int ctox(char c)
    {
    if(c >= '0' && c <= '9') return c - '0';
    else if(c >= 'A' && c <= 'F') return c - 'A' + 10;
    else if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    else return 0;
    }

int CHttpClient::URLDecode(char *dest, const char *source)
    {
    if(dest == source) return 0;

    // NOTE: the restricted characters are defined in the URI RFC 
    //       (current ver: 2396) in section 2.

    int i;

    for(i=0; *source; source++, i++)
        {
        if(*source == '%')
            {
            ++source;
            dest[i] = (char)ctox(*source)*16;
            ++source;
            dest[i] += (char)ctox(*source);
            }
        else
            {
            dest[i] = *source;
            }
        }

    dest[i] = '\0';
    return i;
    }

int CHttpClient::URLEncode(char *dest, const char *source)
    {
    if(dest == source) return 0;

    // NOTE: the restricted characters are defined in the URI RFC 
    //       (current ver: 2396) in section 2.

    char *restricted = ";/?:@&=+$,\"#%%<>\\~ ";
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
        else
            {
            dest[i] = *source;
            }
        }

    dest[i] = '\0';
    return i;
    }

BOOL CHttpClient::DNSLookup(char *address, char *output, size_t bufferlen)
    {
    if(!address || !output || bufferlen==0) return FALSE;

    HOSTENT *hosts;
    hosts = gethostbyname(address);
    if(!hosts) return FALSE;

    size_t length = 0;

    // get the entries
    for(unsigned i=0; hosts->h_addr_list[i]; i++)
        {
        if(bufferlen <= length+16) break;
        length =+ sprintf(output+length, "%u.%u.%u.%u", (unsigned char)hosts->h_addr_list[i][0], (unsigned char)hosts->h_addr_list[i][1], (unsigned char)hosts->h_addr_list[i][2], (unsigned char)hosts->h_addr_list[i][3]);
        if(hosts->h_addr_list[i+1]) length += sprintf(output+length, ", ");
        }

    return TRUE;
    }

// 0 = TTFB
// 1 = TTLB
time_t CHttpClient::GetTime(unsigned char timeToReturn)
    {
    switch (timeToReturn)
        {
        case 0:
            return ttfbTime - sendTime;
        case 1:
            return ttlbTime - sendTime;
        }

    return 0;
    }

#ifdef _DEBUG    // enabled only in debug mode
static void Report(LPCTSTR lpszFormat, ...)
    {
    va_list args;
    va_start(args, lpszFormat);

    char szBuffer[512];
    char szFormat[512];

    char dbuffer[9];
    char tbuffer[9];
    _strdate(dbuffer);
    _strtime(tbuffer);

    sprintf(szFormat, "%s, %s, %s", tbuffer, dbuffer, lpszFormat);
    vsprintf(szBuffer, szFormat, args);

    FILE *f = fopen(DEBUGLOG, "a+");
    if(f)
        {
        fprintf(f, "%s", szBuffer);
        fclose(f);
        }
    else
        printf("%s", szBuffer);

    va_end(args);
    }

static void PrintHexDump(const char * buffer, DWORD length)
    {
    FILE *f = fopen(DEBUGLOG, "a+");

#ifdef _DEBUG_RAW_DUMP
    if(f)
        {
        fprintf(f, "%s", buffer);
        fprintf(f, "\n");
        fclose(f);
        }
    else
        {
        printf("%s", buffer);
        printf("\n");
        }
    return;
#endif

    DWORD i,count,index;
    CHAR rgbDigits[]="0123456789ABCDEF";
    CHAR rgbLine[100];
    char cbLine;


    for(index = 0; length; length -= count, buffer += count, index += count)
        {
        count = (length > 16) ? 16:length;

        sprintf(rgbLine, "%4.4x  ",index);
        cbLine = 6;

        for(i=0;i<count;i++)
            {
            rgbLine[cbLine++] = rgbDigits[buffer[i] >> 4];
            rgbLine[cbLine++] = rgbDigits[buffer[i] & 0x0f];
            if(i == 7)
                {
                rgbLine[cbLine++] = ' ';
                }
            else
                {
                rgbLine[cbLine++] = ' ';
                }
            }
        for(; i < 16; i++)
            {
            rgbLine[cbLine++] = ' ';
            rgbLine[cbLine++] = ' ';
            rgbLine[cbLine++] = ' ';
            }

        rgbLine[cbLine++] = ' ';

        for(i = 0; i < count; i++)
            {
            if(buffer[i] < 32 || buffer[i] > 126)
                {
                rgbLine[cbLine++] = '.';
                }
            else
                {
                rgbLine[cbLine++] = buffer[i];
                }
            }

        rgbLine[cbLine++] = 0;
        if(f) fprintf(f, "%s\n", rgbLine);
        else printf("%s\n", rgbLine);
        }

    if(f) fclose(f);
    }

#endif

