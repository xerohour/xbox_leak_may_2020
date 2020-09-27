/*

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    HttpClient.h

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

*/
#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

// NOTE: Be sure to call WSAStartup() and WSACleanup() in the parent 
//       application.
//
//       Example:
//      
//       WSADATA wsaData;
//       WSAStartup(0x0101, &wsaData);
//        ... // your program code here
//       WSACleanup();
//
// NOTE: If you plan on using SSL you may wish to load the dlls in your main 
//       program, this will keep CHttpClient from loading and unloading all
//       the security libraries.
//
//       Example:
//
//       HINSTANCE hSecuritydll = LoadLibrary("security.dll");
//        ... // your program code here
//       FreeLibrary(hSecuritydll);
//
// NOTE: The debug version of this dll (#define _DEBUG) will dump log 
//       information to the file specified by DEBUGLOG. An extensive
//       amount of data is written to this file.
//
// NOTE: Include files with error codes
//       SSL       <issperr.h> 
//       Sockets   <winsock2.h>
//       HTTP      <wininet.h>
//

#include <winsock2.h>
#include <time.h>

#include "SecureSocketLayer.h"

#define DATA_SIZE 65536

class __declspec( dllexport ) Cookie
    {
    public:
        char *name;
        char *value;
        char *domain;
        char *path;

        Cookie *next;

        char *localname;
        char *localvalue;
        size_t localnameLen;
        size_t localvalueLen;

    public:
        Cookie();
        Cookie(char *n, char *v=NULL, char *d=NULL);
        ~Cookie();

    public:
        Cookie* Find(char *n, char *d=NULL);
        Cookie* Add(char *n, char *v, char *d);
        Cookie* Add(char *setcookie, char *d);
        BOOL Remove(char *n, char *d=NULL);

        void Eval(char *dest, char *d=NULL);
    };

class CHttpClient
    {
    private:
        SOCKET sock;
        SOCKET nextsock;
        SOCKADDR_IN dest;

        WORD wPort;

        // Proxy Settings
        BOOL bUseProxy;
        WORD wProxyPort;
        char szProxyServerName[1024];

        // SSL stuffs
        CSecureSocketLayer *SSL;

        // timeouts
        TIMEVAL timeout;

		// timers
        time_t sendTime;
        time_t ttfbTime;        // Time to First Byte
        time_t ttlbTime;        // Time to Last Byte

    public: // send and receive buffer
        char data[DATA_SIZE+1];
        int dataLen;

    public:
        __declspec( dllexport ) int GetHTTPStatus(void);
        __declspec( dllexport ) char* GetBody(void);
        __declspec( dllexport ) char* GetData(void);
        __declspec( dllexport ) int GetDataLen(void);

    public:
        __declspec( dllexport ) int ParseURL(const char *url, char *server, char *site, BOOL* ssl);
        __declspec( dllexport ) int GrabCookies(Cookie *jar, char *source, char *domain=NULL);
        __declspec( dllexport ) int GetCookieRules(void);
        __declspec( dllexport ) void SetCookieRules(int i);
        __declspec( dllexport ) int URLEncode(char *dest, const char *source);
        __declspec( dllexport ) int URLDecode(char *dest, const char *source);
        __declspec( dllexport ) int POSTEncode(char *dest, const char *source);

    public:
        __declspec( dllexport ) long Open(void);
        __declspec( dllexport ) long Connect(IN_ADDR server, const char *serverName);
        __declspec( dllexport ) long Connect(const char *serverName);
        __declspec( dllexport ) long Disconnect(void);
        __declspec( dllexport ) long HardDisconnect(void);

        __declspec( dllexport ) long Send(const char *senddata, int len);
        __declspec( dllexport ) long Send(void); // send what is already in the 'data' buffer
        __declspec( dllexport ) long Receive(BOOL readAll=FALSE);

        __declspec( dllexport ) BOOL IsDataAvailable(void);
        __declspec( dllexport ) BOOL IsSendAvailable(void);

    public: // socket / connection options
        __declspec( dllexport ) long SetLinger(BOOL linger, WORD timeout);
        __declspec( dllexport ) long SetKeepAlive(BOOL keepalive);
        __declspec( dllexport ) long SetSSL(LPSTR pszUserName=NULL, DWORD dwProtocol=SP_PROT_SSL2, ALG_ID aiKeyExch=CALG_RSA_KEYX);
        __declspec( dllexport ) long SetProxy(const char *serverName, WORD port);
        __declspec( dllexport ) void SetRecvTimeout(UINT sec, UINT msec);
        __declspec( dllexport ) void SetDefaultPort(WORD newPort);

    public: // information
        __declspec( dllexport ) BOOL DNSLookup(char *address, char *output, size_t bufferlen);
        __declspec( dllexport ) time_t GetTime(unsigned char timeToReturn);

    public: // constructor / destructor
        __declspec( dllexport ) CHttpClient();
        __declspec( dllexport ) ~CHttpClient();
    };

#endif // _HTTPCLIENT_H_