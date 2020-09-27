/*++

Copyright (c) 1995 Microsoft Corporation

Module Name:

    webclient.c

Abstract:

    Schannel web client sample application.

Revision History:

    09-18-97 jbanes     Created.

--*/

#include "xapip.h"
#include "winbasep.h"
#include <stdio.h>
#include <winsock2.h>
#include <wincrypt.h>
#include <tchar.h>
#include <xdbg.h>
#include <schnlsp.h>
#include <spdefs.h>
#include <ipmap.h>
#include <xrlp.h>
#include <httpsget.h>

#ifdef UNDER_CE
//
// Flags for identifying the various different protocols.
//

/* flag/identifiers for protocols we support */
#define SP_PROT_PCT1_SERVER             0x00000001
#define SP_PROT_PCT1_CLIENT             0x00000002
#define SP_PROT_PCT1                    (SP_PROT_PCT1_SERVER | SP_PROT_PCT1_CLIENT)

#define SP_PROT_SSL2_SERVER             0x00000004
#define SP_PROT_SSL2_CLIENT             0x00000008
#define SP_PROT_SSL2                    (SP_PROT_SSL2_SERVER | SP_PROT_SSL2_CLIENT)

#define SP_PROT_SSL3_SERVER             0x00000010
#define SP_PROT_SSL3_CLIENT             0x00000020
#define SP_PROT_SSL3                    (SP_PROT_SSL3_SERVER | SP_PROT_SSL3_CLIENT)

#define SP_PROT_TLS1_SERVER             0x00000040
#define SP_PROT_TLS1_CLIENT             0x00000080
#define SP_PROT_TLS1                    (SP_PROT_TLS1_SERVER | SP_PROT_TLS1_CLIENT)

#define SP_PROT_SSL3TLS1_CLIENTS        (SP_PROT_TLS1_CLIENT | SP_PROT_SSL3_CLIENT)
#define SP_PROT_SSL3TLS1_SERVERS        (SP_PROT_TLS1_SERVER | SP_PROT_SSL3_SERVER)
#define SP_PROT_SSL3TLS1                (SP_PROT_SSL3 | SP_PROT_TLS1)

#define SP_PROT_UNI_SERVER              0x40000000
#define SP_PROT_UNI_CLIENT              0x80000000
#define SP_PROT_UNI                     (SP_PROT_UNI_SERVER | SP_PROT_UNI_CLIENT)

#define SP_PROT_ALL                     0xffffffff
#define SP_PROT_ALL_NOT_TLS             (SP_PROT_PCT1 | SP_PROT_SSL2 | SP_PROT_SSL3)
#define SP_PROT_NONE                    0
#define SP_PROT_CLIENTS                 (SP_PROT_PCT1_CLIENT | SP_PROT_SSL2_CLIENT | SP_PROT_SSL3_CLIENT | SP_PROT_UNI_CLIENT | SP_PROT_TLS1_CLIENT)
#define SP_PROT_SERVERS                 (SP_PROT_PCT1_SERVER | SP_PROT_SSL2_SERVER | SP_PROT_SSL3_SERVER | SP_PROT_UNI_SERVER | SP_PROT_TLS1_SERVER)
#endif

#define SECURITY_WIN32
#include <security.h>
#include <sspi.h>

#define IO_BUFFER_SIZE  0x10000

#ifdef UNDER_CE
#define INITSECURITYNAME TEXT("InitSecurityInterfaceW")
#else // NT
#ifdef UNICODE
#define INITSECURITYNAME "InitSecurityInterfaceW"
#else
#define INITSECURITYNAME "InitSecurityInterfaceA"
#endif
#endif

#ifdef UNICODE
#define ATOI(p) wcstol(p,NULL,10)
#else
#define ATOI(p) atoi(p)
#endif

PSecurityFunctionTable g_pSecFuncs;


SECURITY_STATUS SEC_ENTRY
SealMessage(
    PCtxtHandle         phContext,
    DWORD               fQOP,
    PSecBufferDesc      pMessage,
    ULONG               MessageSeqNo
    );


SECURITY_STATUS SEC_ENTRY
UnsealMessage(
    PCtxtHandle         phContext,
    PSecBufferDesc      pMessage,
    ULONG               MessageSeqNo,
    DWORD *             pfQOP
    );

extern
BOOL
WINAPI
InitializeSSL();

typedef SECURITY_STATUS 
(SEC_ENTRY *PFSealMessage)( PCtxtHandle         phContext,
                unsigned long       fQOP,
                PSecBufferDesc      pMessage,
                unsigned long       MessageSeqNo);

typedef SECURITY_STATUS 
(SEC_ENTRY *PFUnsealMessage)( PCtxtHandle         phContext,
                PSecBufferDesc      pMessage,
                unsigned long       MessageSeqNo,
                unsigned long *     pfQOP);


LPSTR   pszProxyServer  = "itgproxy";
INT     iProxyPort      = 80;

// User options.
LPTSTR   pszServerName[10];
DWORD    cServers = 0;
char     strServerName[100];
INT     iPortNumber     = 443;
LPTSTR  pszFileName     = TEXT("default.htm");
BOOL    fVerbose        = FALSE;    
BOOL    fDumpSSL        = TRUE;     // display SSL header (if unencrypted)
BOOL    fUseProxy       = FALSE;    
BOOL    fUseSecSock     = FALSE;    // use winsock (otherwise use SSPI)
BOOL    fMultiThread    = FALSE;    // multi-threaded test
BOOL    fTime           = FALSE;    // display timing info
BOOL    fOnlyTime       = FALSE;    // only prints time info
BOOL    fOnlyTimeSummary= FALSE;    // only prints total time
BOOL    fQuiet          = FALSE;    // suppress all output
DWORD   tTotal          = 0;        // for total elapsed time
DWORD   tTimeConnect,               // for summary statistics 
        tTimeHS, 
        tTimeGetFile;
BOOL    fContinue       = FALSE;    // keep hitting the server
LPTSTR  pszUserName     = NULL;
char    strUserName[100];

BOOL    fClientAuth     = TRUE;
BOOL    fUseFortezza    = FALSE;
BOOL    fDebugBreak     = FALSE;

HCERTSTORE  hMyCertStore = NULL;

PTSTR    pszServerCertFilename = NULL;

static
SECURITY_STATUS
CreateCredentials(
    LPSTR pszUserName,
    DWORD dwProtocol,
    PCredHandle phCreds);

static DWORD
GetFortezzaContext(PCCERT_CONTEXT *ppCertContext);

static INT
ConnectToServer(
    LPSTR pszServerName,
    INT   iPortNumber,
    SOCKET *pSocket);

static
SECURITY_STATUS
PerformClientHandshake(
    SOCKET          Socket,
    PCredHandle     phCreds,
    LPTSTR           pszServerName,
    CtxtHandle *    phContext,
    SecBuffer *     pExtraData);

static
SECURITY_STATUS
ClientHandshakeLoop(
    SOCKET          Socket,
    PCredHandle     phCreds,
    CtxtHandle *    phContext,
    BOOL            fDoInitialRead,
    SecBuffer *     pExtraData);

static
HRESULT
HttpsGet(
    SOCKET          Socket,
    PCredHandle     phCreds,
    CtxtHandle *    phContext,
    const XRL xrl,
    DWORD* pcbBuffer,
    PBYTE pBuffer,
    XRL_KNOWN_HEADERS *pHTTPHeaders,
    PXAPPSERVICEINFO pServiceInfo);

static 
void
DisplayCertChain(
    PCCERT_CONTEXT pServerCert);

static 
DWORD
VerifyServerCertificate(
    PCCERT_CONTEXT  pServerCert,
    PSTR            pszServerName,
    DWORD           dwCertFlags);

void
DisplayConnectionInfo(
    SecPkgContext_ConnectionInfo *pConnectionInfo);

void PrintHexDump(DWORD length, PBYTE buffer);
static void DumpSSL(DWORD length, const BYTE *buffer);

// make a ANSI string from a LPSTR
void
CopyToStr(LPSTR pDest, LPTSTR pSrc, size_t cbDest)
{
#ifdef UNICODE
    wcstombs(pDest, pSrc, cbDest);
#else
    strncpy(pDest, pSrc, cbDest);
#endif
}

/*****************************************************************************/
#if 0
void Usage(void)
{
    printf("\n");
    printf("USAGE: webclient -s<server> [ <options> ]\n");
    printf("\n");
    printf("    -s<server>      DNS name of server.\n");
    printf("    -p<port>        Port that server is listing on (default 443).\n");
    printf("    -f<file>        Name of file to retrieve (default \"%s\")\n", pszFileName);
    printf("    -v              Verbose Mode.\n");
    printf("    -t              display timing info.\n");
    printf("    -x              Connect via the \"%s\" proxy server.\n", pszProxyServer);
    printf("    -w              Connect using secure WINSOCK instead of SSPI\n");
    printf("    -m              Multi-threaded test.\n");
    printf("    -c<filename>    Dump server certificate to file.\n");   
    printf("    -F              Attempt to acquire Fortezza credentials\n");
    printf("    -t              Display timing information\n");
    printf("    -o              Display only timing information\n");
    printf("    -n              Display only the total connection time\n");
    printf("    -a              Keep connecting to the server until you hold shift down\n");
    printf("    -q              Quiet - no output unless errors occur\n");
#ifdef DEBUG    
    printf("    -b              Debug break on entry\n");
#endif
    
    printf("\n");
    printf("  For client auth\n");
    printf("    -u<user>        Name of user (in existing client certificate)\n");
    printf("    -d              Don't send certificate to server unless it asks first.\n");

    DoExit(1);
}

void
DoMain(TCHAR *pCmdLine)
{
    WSADATA WsaData;
    DWORD i;

    INIT_SECURITY_INTERFACE pfnInitSecurity;
    HINSTANCE hLibSchannel;
    INT iOption;
    PTCHAR pszOption, pszEndOption;

    //
    // Parse the command line.
    //
    
    if(*pCmdLine == 0)
    {
        Usage();
    }

    while(*pCmdLine) 
    {
        
        if(*pCmdLine == '/') *pCmdLine = '-';

        if(*pCmdLine != '-') 
        {
            _tprintf(TEXT("**** Invalid argument \"%s\"\n"), pCmdLine);
            Usage();
        }

        iOption = pCmdLine[1];
        pszOption = pCmdLine+2;
        while (*pCmdLine != ' ' && *pCmdLine != 0)
            pCmdLine++;
        pszEndOption = pCmdLine;
        // advance to next option
        while (*pCmdLine == ' ')
            pCmdLine++;

        *pszEndOption = 0;    // null terminate the option

        switch(iOption) 
        {
        case 's':
            if (cServers < sizeof(pszServerName)/sizeof(pszServerName[0]))
                pszServerName[cServers++] = pszOption;         
            break;

        case 'p':
            iPortNumber = ATOI(pszOption);
            break;

        case 'f':
            pszFileName = pszOption;
             CopyToStr(strFileName, pszFileName, sizeof(strFileName));
            break;

        case 'v':
            fVerbose = TRUE;
            break;

        case 't':
            fTime = TRUE;
            break;
        case 'x':
            fUseProxy = TRUE;
            break;

        case 'w':
            fUseSecSock = TRUE;
            break;

        case 'm':
            fMultiThread = TRUE;
            break;
            
        case 'u':
            pszUserName = pszOption;
            CopyToStr(strUserName, pszUserName, sizeof(strUserName));
            break;

        case 'd':
            fClientAuth = FALSE;
            break;

        case 'c':
            pszServerCertFilename = pszOption;
            break;

        case 'F':
            fUseFortezza = TRUE;
            break;
        case 'b':
            fDebugBreak = TRUE;
            break;
        case 'o':
            fOnlyTime = TRUE;
            fTime = TRUE;
            break;
        case 'n':
            fOnlyTimeSummary = TRUE;
            fOnlyTime = TRUE;
            fTime = TRUE;
            break;
        case 'a':
            fContinue = TRUE;
            break;
        case 'q':
            fQuiet = TRUE;
            break;

        default:
            printf("**** Invalid option \"%c\"\n", iOption);
            Usage();
        }
    }

#ifdef DEBUG
    if (fDebugBreak)
        DebugBreak();
#endif
    //
    // Initialize the WinSock subsystem.
    //

    if(WSAStartup(0x0101, &WsaData) == SOCKET_ERROR)
    {
        XDBGTRC("Xonline", "Error %d returned by WSAStartup", GetLastError());
        DoExit(1);
    }

    if (!fUseSecSock)
    {
        // 
        // Load SCHANNEL
        //

        hLibSchannel = LoadLibrary(TEXT("SCHANNEL.DLL"));
        if (!hLibSchannel)
        {
            XDBGTRC("Xonline", "Error %d returned by LoadLibrary(SCHANNEL)", GetLastError());
            DoExit(1);
        }
        pfnInitSecurity = (INIT_SECURITY_INTERFACE)GetProcAddress(hLibSchannel,INITSECURITYNAME);


        if (!pfnInitSecurity || !(g_pSecFuncs = (*pfnInitSecurity)()))
        {
            XDBGTRC("Xonline", "Cant get SCHANNEL security interface pointers");
            DoExit(1);
        }
    }

    for (i = 0; i < cServers || fContinue; i++) {
        Sleep(1000);
        if ((GetAsyncKeyState(VK_LSHIFT) >> 1) || 
            (GetAsyncKeyState(VK_RSHIFT) >> 1))
            break;
        tTotal = GetTickCount();

#ifdef UNDER_CE    
        if (fUseSecSock)
        {
            SSLSockConnect(pszServerName[i]);
        }
        else
#endif
        if (fMultiThread)
        {
            MTSChannelConnect(pszServerName[(fContinue ? 0 : i)]);
        }
        else
        {
            SChannelConnect(pszServerName[(fContinue ? 0 : i)]);
        }

        tTotal = GetTickCount() - tTotal;
        if (fTime)
            XDBGTRC("Xonline", "Connect: %d  HandShake: %d  GetFile: %d  Total: %d", tTimeConnect, tTimeHS, 
                tTimeGetFile, tTotal);
    } 
       
    // Shutdown WinSock subsystem.
    WSACleanup();

#ifndef UNDER_CE
    // Close "MY" certificate store.
    if(hMyCertStore)
    {
        CertCloseStore(hMyCertStore, 0);
    }
#endif


    DoExit(0);
}
#endif

HRESULT XRL_SSLDownloadToMemory( const XRL xrl, DWORD* pcbBuffer, PBYTE pBuffer, XRL_KNOWN_HEADERS *pHTTPHeaders )
{
    static BOOL fFirstTime = TRUE;
    CredHandle hClientCreds;
    CtxtHandle hContext;
    SecBuffer  ExtraData;
    SecPkgContext_ConnectionInfo ConnectionInfo;
    SECURITY_STATUS Status;
    TimeStamp tsExpiry;
    HRESULT     hr;
    int         serr;
    XAPPSERVICEINFO serviceInfo;
    SOCKET      socketGet = INVALID_SOCKET;
    SOCKADDR_IN sockAddress;
    LINGER      l_linger;

    // values for the header parsing
    XRL_KNOWN_HEADERS   parsedHeaders;
    ULARGE_INTEGER      contentLength;

    RIP_ON_NOT_TRUE("XRL_DownloadToMemory()", (NULL != xrl));
    RIP_ON_NOT_TRUE("XRL_DownloadToMemory()", (NULL != pcbBuffer));
    RIP_ON_NOT_TRUE("XRL_DownloadToMemory()", (NULL != pBuffer));

    //
    // Initialize stuff if this is the first time called.
    //
    if (fFirstTime)
    {
        InitializeSSL();
        fFirstTime = FALSE;
    }

    //
    // Create credentials.
    //
    Status = AcquireCredentialsHandle(
                        NULL,                   // Name of principal
                        UNISP_NAME,             // Name of package
                        SECPKG_CRED_OUTBOUND,   // Flags indicating use
                        NULL,                   // Pointer to logon ID
                        NULL,                   // Package specific data
                        NULL,                   // Pointer to GetKey() func
                        NULL,                   // Value to pass to GetKey()
                        &hClientCreds,          // (out) Cred Handle
                        &tsExpiry);             // (out) Lifetime (optional)
    if(Status != SEC_E_OK)
    {
        XDBGTRC("Xonline", "Error 0x%x returned by AcquireCredentialsHandle", Status);
        hr = HRESULT_FROM_WIN32( Status );
        goto abort;
    }
        
    // prep the parsed headers structure
    // prepare to read the headers
    // we only care about content length here
    ZeroMemory( &parsedHeaders, sizeof(parsedHeaders) );
    // if they requested a headers structure, fill in the pointers
    if ( pHTTPHeaders )
    {
        CopyMemory( &parsedHeaders, pHTTPHeaders, sizeof(parsedHeaders) );
    }
    // if they didn't request content length, we still need it
    if ( !parsedHeaders.pContentLength )
    {
        parsedHeaders.pContentLength = &contentLength;
    }

    // reset the amount read to 0
    parsedHeaders.pContentLength->QuadPart = 0;
    parsedHeaders.dwHTTPResponseCode = 0;

    // get the XRL's target IP address
    hr = XRL_LookupXRLIPAddress( xrl, &sockAddress, &serviceInfo );
    // see if we were able to obtain the address - have not tried
    // connect to it. Just got the address
    if ( hr != S_OK )
    {
        goto abort;
    }

    // Open up the socket to the target address. This socket uses TCP over IP
    socketGet = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socketGet == INVALID_SOCKET)
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );
        goto abort;
    }

    // We have to set the linger to 0 or else the xbox socket layer runs out
    // of sockets if you execute the function lots of times really rapidly.
    l_linger.l_onoff=1;
    l_linger.l_linger=0;
    serr = setsockopt(socketGet,SOL_SOCKET,SO_LINGER,(char *)&l_linger,sizeof(l_linger));
    if (serr == SOCKET_ERROR)
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );
        goto abort;
    }

    // Initiate the connection
    serr = connect(socketGet, (PSOCKADDR) &sockAddress, sizeof(sockAddress));
    if (serr == SOCKET_ERROR)
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );
        goto abort;
    }

    //
    // Perform SSL handshake
    //
    if(PerformClientHandshake(socketGet,
                              &hClientCreds,
                              L"xaccount", // TODO: see if this matters
                              &hContext,
                              &ExtraData))
    {
        XDBGTRC("Xonline", "Error performing handshake");
        hr = E_FAIL;
        goto abort;
    }

    //
    // Authenticate server's credentials.
    //

#ifndef XBOX
    // Get server's certificate.
    Status = QueryContextAttributes(&hContext,
                                    SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                    (PVOID)&pRemoteCertContext);
    if(Status != SEC_E_OK)
    {
        XDBGTRC("Xonline", "Error 0x%x querying remote certificate", Status);
        hr = HRESULT_FROM_WIN32( Status );
        goto abort;
    }

    // Display server certificate chain.
    DisplayCertChain(pRemoteCertContext);
#endif

#if 0
    // Attempt to validate server certificate.
    Status = VerifyServerCertificate(pRemoteCertContext,
                                     pszServerName,
                                     0);
    if(Status)
    {
        XDBGTRC("Xonline", "**** Error authenticating server credentials!");
    }
#endif

#ifdef  DBG
    //
    // Display connection info. 
    //    
    Status = QueryContextAttributes(&hContext,
                                    SECPKG_ATTR_CONNECTION_INFO,
                                    (PVOID)&ConnectionInfo);
    if(Status != SEC_E_OK)
    {
        XDBGTRC("Xonline", "Error 0x%x querying connection info", Status);
    }
    else
    {
        DisplayConnectionInfo(&ConnectionInfo);
    }
#endif

    //
    // Perform the HTTP Get from server.
    //
    hr = HttpsGet(socketGet, 
                  &hClientCreds,
                  &hContext, 
                  xrl,
                  pcbBuffer,
                  pBuffer,
                  pHTTPHeaders,
                  &serviceInfo);
    if( FAILED(hr) )
    {
        XDBGTRC("Xonline", "Error %x from HttpsGet", hr );
        goto abort;
    }

    //
    // Cleanup.
    //
abort:

    // Free security context.
    DeleteSecurityContext(&hContext);

    // Free SSPI credentials handle.
    FreeCredentialHandle(&hClientCreds);

    // Close socket.
    closesocket(socketGet);

    return hr;
}


#ifdef XBOX

/*****************************************************************************/
static
SECURITY_STATUS
CreateCredentials(
    LPSTR pszUserName,              // in
    DWORD dwProtocol,               // in
    PCredHandle phCreds)            // out
{
    TimeStamp       tsExpiry;
    SECURITY_STATUS Status;


    //
    // Create an SSPI credential.
    //

    Status = AcquireCredentialsHandle(
                        NULL,                   // Name of principal
                        UNISP_NAME,           // Name of package
                        SECPKG_CRED_OUTBOUND,   // Flags indicating use
                        NULL,                   // Pointer to logon ID
                        NULL,          // Package specific data
                        NULL,                   // Pointer to GetKey() func
                        NULL,                   // Value to pass to GetKey()
                        phCreds,                // (out) Cred Handle
                        &tsExpiry);             // (out) Lifetime (optional)
    if(Status != SEC_E_OK)
    {
        XDBGTRC("Xonline", "**** Error 0x%x returned by AcquireCredentialsHandle", Status);
        return Status;
    }



    return SEC_E_OK;
}
#else

/*****************************************************************************/
static
SECURITY_STATUS
CreateCredentials(
    LPSTR pszUserName,              // in
    DWORD dwProtocol,               // in
    PCredHandle phCreds)            // out
{
    SCHANNEL_CRED   SchannelCred;
    TimeStamp       tsExpiry;
    SECURITY_STATUS Status;

    PCCERT_CONTEXT  pCertContext = NULL;

    // Open the "MY" certificate store, which is where Internet Explorer
    // stores its client certificates.
    if(hMyCertStore == NULL)
    {
        hMyCertStore = CertOpenSystemStore(0, TEXT("MY"));

        if(!hMyCertStore)
        {
            XDBGTRC("Xonline", "**** Error 0x%x returned by CertOpenSystemStore", 
                GetLastError());
            return SEC_E_NO_CREDENTIALS;
        }
    }

    //
    // If a user name is specified, then attempt to find a client
    // certificate. Otherwise, just create a NULL credential.
    //

    if(pszUserName && *pszUserName)
    {
        // Find client certificate. Note that this sample just searchs for a 
        // certificate that contains the user name somewhere in the subject name.
        // A real application should be a bit less casual.
        pCertContext = CertFindCertificateInStore(hMyCertStore, 
                                                  X509_ASN_ENCODING, 
                                                  0,
                                                  CERT_FIND_SUBJECT_STR_A,
                                                  pszUserName,
                                                  NULL);
        if(pCertContext == NULL)
        {
            XDBGTRC("Xonline", "**** Error 0x%x returned by CertFindCertificateInStore",
                GetLastError());
            return SEC_E_NO_CREDENTIALS;
        }
    }
    else if(fUseFortezza)
    {
        Status = GetFortezzaContext(&pCertContext);
        if(Status != SEC_E_OK)
        {
            return Status;
        }
    }


    //
    // Build Schannel credential structure. Currently, this sample only
    // specifies the protocol to be used (and optionally the certificate, 
    // of course). Real applications may wish to specify other parameters 
    // as well.
    //

    ZeroMemory(&SchannelCred, sizeof(SchannelCred));

    SchannelCred.dwVersion  = SCHANNEL_CRED_VERSION;
    if(pCertContext)
    {
        SchannelCred.cCreds     = 1;
        SchannelCred.paCred     = &pCertContext;
    }

//    SchannelCred.grbitEnabledProtocols = SP_PROT_SSL3;

    
    //
    // Create an SSPI credential.
    //

    Status = AcquireCredentialsHandle(
                        NULL,                   // Name of principal
                        UNISP_NAME,           // Name of package
                        SECPKG_CRED_OUTBOUND,   // Flags indicating use
                        NULL,                   // Pointer to logon ID
                        &SchannelCred,          // Package specific data
                        NULL,                   // Pointer to GetKey() func
                        NULL,                   // Value to pass to GetKey()
                        phCreds,                // (out) Cred Handle
                        &tsExpiry);             // (out) Lifetime (optional)
    if(Status != SEC_E_OK)
    {
        XDBGTRC("Xonline", "**** Error 0x%x returned by AcquireCredentialsHandle", Status);
        return Status;
    }


    //
    // Free the certificate context. Schannel has already made its own copy.
    //

    if(pCertContext)
    {
        CertFreeCertificateContext(pCertContext);
    }


    return SEC_E_OK;
}


/*****************************************************************************/
static DWORD
GetFortezzaContext(PCCERT_CONTEXT *ppCertContext)
{
    DWORD           Status;
    HCRYPTPROV      hProv = 0;
    CRYPT_HASH_BLOB HashBlob;
    CRYPT_DATA_BLOB DataBlob;
    BYTE            rgbHash[20];
    DWORD           cbHash;
    PBYTE           pbChain = NULL;
    DWORD           cbChain;
    PBYTE           pbCert;
    DWORD           cbCert;
    PCCERT_CONTEXT  pCertContext = NULL;


    //
    // Attempt to log on to Fortezza card. This call will typically 
    // display a dialog box.
    //
    // Note that within the CryptAcquireContext function, the Fortezza
    // CSP populated the MY store with the Fortezza certificate chain.
    // At least, it will once it's finished.
    //

    if(!CryptAcquireContext(&hProv, NULL, NULL, PROV_FORTEZZA, 0))
    {
        Status = GetLastError();
        XDBGTRC("Xonline", "Error 0x%x logging onto Fortezza card", Status);
        goto done; 
    }


    //
    // Read the appropriate leaf certificate from the card, and
    // obtain its MD5 thumbprint.
    //

    // Get length of certificate chain.
    if(!CryptGetProvParam(hProv, PP_CERTCHAIN, NULL, &cbChain, 0))
    {
        Status = GetLastError();
        XDBGTRC("Xonline", "**** Error 0x%x reading certificate from CSP", Status);
        goto done; 
    }

    // Allocate memory for certificate chain.
    pbChain = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, cbChain);
    if(pbChain == NULL)
    {
        Status = ERROR_NOT_ENOUGH_MEMORY;
        XDBGTRC("Xonline", "**** Out of memory");
        goto done;
    }

    // Download certificate chain from CSP.
    if(!CryptGetProvParam(hProv, PP_CERTCHAIN, pbChain, &cbChain, 0))
    {
        Status = GetLastError();
        XDBGTRC("Xonline", "**** Error 0x%x reading certificate from CSP", Status);
        goto done;
    }

    // Parse out the leaf certificate.
    cbCert = *(PDWORD)pbChain;
    pbCert = pbChain + sizeof(DWORD);

    // Decode the leaf certificate.
    pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING,
                                                pbCert,
                                                cbCert);
    if(pCertContext == NULL)
    {
        Status = GetLastError();
        XDBGTRC("Xonline", "**** Error 0x%x parsing certificate", Status);
        goto done;
    }

    // Get thumbprint of certificate.
    cbHash = sizeof(rgbHash);
    if(!CertGetCertificateContextProperty(pCertContext,
                                          CERT_MD5_HASH_PROP_ID,
                                          rgbHash,
                                          &cbHash))
    {
        Status = GetLastError();
        XDBGTRC("Xonline", "**** Error 0x%x reading MD5 property", Status);
        goto done;
    }

    // Free certificate chain.
    LocalFree(pbChain);
    pbChain = NULL;

    // Free certificate context.
    CertFreeCertificateContext(pCertContext);
    pCertContext = NULL;

    
    //
    // Search the "MY" certificate store for the certificate with
    // the matching thumbprint.
    //

    HashBlob.cbData = cbHash;
    HashBlob.pbData = rgbHash;
    pCertContext = CertFindCertificateInStore(hMyCertStore, 
                                              X509_ASN_ENCODING, 
                                              0,
                                              CERT_FIND_MD5_HASH,
                                              &HashBlob,
                                              NULL);
    if(pCertContext == NULL)
    {
        XDBGTRC("Xonline", "**** Leaf certificate not found in MY store");
        Status = SEC_E_NO_CREDENTIALS;
        goto done;
    }


    //
    // Attach the Fortezza hProv to the certificate context.
    //

    DataBlob.pbData = (PBYTE)&hProv;
    DataBlob.cbData = sizeof(hProv);

    if(!CertSetCertificateContextProperty(
            pCertContext,
            CERT_KEY_PROV_HANDLE_PROP_ID,
            0,
            &DataBlob))
    {
        Status = GetLastError();
        XDBGTRC("Xonline", "**** Error 0x%x setting KEY_PROV_HANDLE property", GetLastError());
        goto done;
    }
    hProv = 0;


    *ppCertContext = pCertContext;
    pCertContext = NULL;
    
    Status = SEC_E_OK;

done:

    if(pbChain) LocalFree(pbChain);
    if(hProv) CryptReleaseContext(hProv, 0);
    if(pCertContext) CertFreeCertificateContext(pCertContext);

    return Status;
}
#endif

/*****************************************************************************/
static INT
ConnectToServer(
    LPSTR    strServerName, // in
    INT      iPortNumber,   // in
    SOCKET * pSocket)       // out
{
    SOCKET Socket;
    struct sockaddr_in sin;
    struct hostent *hp;
    DWORD hostip;
    
    Socket = socket(PF_INET, SOCK_STREAM, 0);
    if(Socket == INVALID_SOCKET)
    {
        XDBGTRC("Xonline", "**** Error %d creating socket", WSAGetLastError());
        return WSAGetLastError();
    }

    if(fUseProxy)
    {
        sin.sin_family = AF_INET;
        sin.sin_port = ntohs((u_short)iProxyPort);

        if((hp = gethostbyname(pszProxyServer)) == NULL)
        {
            XDBGTRC("Xonline", "**** Error %d returned by gethostbyname", WSAGetLastError());
            return WSAGetLastError();
        }
        else
        {
            memcpy(&sin.sin_addr, hp->h_addr, 4);
        }
    }
    else
    {
        sin.sin_family = AF_INET;
        sin.sin_port = htons((u_short)iPortNumber);

        if((hp = gethostbyname(strServerName)) == NULL)
        {
            XDBGTRC("Xonline", "**** Error %d returned by gethostbyname", WSAGetLastError());
            return WSAGetLastError();
        }
        else
        {
            memcpy(&sin.sin_addr, hp->h_addr, 4);
        }
    }

    if(connect(Socket, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        XDBGTRC("Xonline", "**** Error %d connecting to \"%s\" (%s)", 
            WSAGetLastError(),
            strServerName, 
            inet_ntoa(sin.sin_addr));
        closesocket(Socket);
        return WSAGetLastError();
    }

    if(fUseProxy)
    {
        BYTE  pbMessage[200]; 
        DWORD cbMessage;

        // Build message for proxy server
        strcpy(pbMessage, "CONNECT ");
        strcat(pbMessage, strServerName);
        strcat(pbMessage, ":");
        _itoa(iPortNumber, pbMessage + strlen(pbMessage), 10);
        strcat(pbMessage, " HTTP/1.0\r\nUser-Agent: webclient\r\n\r\n");
        cbMessage = strlen(pbMessage);

        // Send message to proxy server
        if(send(Socket, pbMessage, cbMessage, 0) == SOCKET_ERROR)
        {
            XDBGTRC("Xonline", "**** Error %d sending message to proxy!", WSAGetLastError());
            return WSAGetLastError();
        }

        // Receive message from proxy server
        cbMessage = recv(Socket, pbMessage, 200, 0);
        if(cbMessage == SOCKET_ERROR)
        {
            XDBGTRC("Xonline", "**** Error %d receiving message from proxy", WSAGetLastError());
            return WSAGetLastError();
        }

        // BUGBUG - should continue to receive until CR LF CR LF is received
    }

    *pSocket = Socket;

    return SEC_E_OK;
}


/*****************************************************************************/
static
SECURITY_STATUS
PerformClientHandshake(
    SOCKET          Socket,         // in
    PCredHandle     phCreds,        // in
    LPTSTR          pszServerName,  // in
    CtxtHandle *    phContext,      // out
    SecBuffer *     pExtraData)     // out
{
    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    SECURITY_STATUS scRet;
    DWORD           cbData;
    DWORD   tElapsed;

    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

//    dwSSPIFlags |= ISC_REQ_MANUAL_CRED_VALIDATION;

    //
    //  Initiate a ClientHello message and generate a token.
    //

    OutBuffers[0].pvBuffer   = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer   = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    tElapsed = GetTickCount();

    scRet = InitializeSecurityContext(
                    phCreds,
                    NULL,
                    pszServerName,
                    dwSSPIFlags,
                    0,
                    SECURITY_NATIVE_DREP,
                    NULL,
                    0,
                    phContext,
                    &OutBuffer,
                    &dwSSPIOutFlags,
                    &tsExpiry);

    if (fTime)
    {
        tElapsed = GetTickCount() - tElapsed;
        XDBGTRC("Xonline", "ISC - %d ms",tElapsed);
    }
    if(scRet != SEC_I_CONTINUE_NEEDED)
    {
        XDBGTRC("Xonline", "**** Error %d returned by InitializeSecurityContext (1)", scRet);
        return scRet;
    }

    // Send response to server if there is one.
    if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
    {
        cbData = send(Socket,
                      OutBuffers[0].pvBuffer,
                      OutBuffers[0].cbBuffer,
                      0);
        if(cbData == SOCKET_ERROR || cbData == 0)
        {
            XDBGTRC("Xonline", "**** Error %d sending data to server (1)", WSAGetLastError());
            FreeContextBuffer(OutBuffers[0].pvBuffer);
            DeleteSecurityContext(phContext);
            return SEC_E_INTERNAL_ERROR;
        }

        XDBGTRC("Xonline", "%d bytes of handshake data sent", cbData);

        if(fVerbose)
        {
            PrintHexDump(cbData, OutBuffers[0].pvBuffer);
            XDBGTRC("Xonline", "");
        }

        // Free output buffer.
        FreeContextBuffer(OutBuffers[0].pvBuffer);
        OutBuffers[0].pvBuffer = NULL;
    }


    return ClientHandshakeLoop(Socket, phCreds, phContext, TRUE, pExtraData);
}

/*****************************************************************************/
static
SECURITY_STATUS
ClientHandshakeLoop(
    SOCKET          Socket,         // in
    PCredHandle     phCreds,        // in
    CtxtHandle *    phContext,      // in, out
    BOOL            fDoInitialRead, // in
    SecBuffer *     pExtraData)     // out
{
    SecBufferDesc   InBuffer;
    SecBuffer       InBuffers[2];
    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    SECURITY_STATUS scRet;
    DWORD           cbData;

    PUCHAR          IoBuffer;
    DWORD           cbIoBuffer;
    BOOL            fDoRead;
    DWORD           tElapsed;


    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

//    dwSSPIFlags |= ISC_REQ_MANUAL_CRED_VALIDATION;

    //
    // Allocate data buffer.
    //

    IoBuffer = LocalAlloc(LMEM_FIXED, IO_BUFFER_SIZE);
    if(IoBuffer == NULL)
    {
        XDBGTRC("Xonline", "**** Out of memory (1)");
        return SEC_E_INTERNAL_ERROR;
    }
    cbIoBuffer = 0;

    fDoRead = fDoInitialRead;


    // 
    // Loop until the handshake is finished or an error occurs.
    //

    scRet = SEC_I_CONTINUE_NEEDED;

    while(scRet == SEC_I_CONTINUE_NEEDED        ||
          scRet == SEC_E_INCOMPLETE_MESSAGE     ||
          scRet == SEC_I_INCOMPLETE_CREDENTIALS) 
    {

        //
        // Read data from server.
        //

        if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            if(fDoRead)
            {
                tElapsed = GetTickCount();
                cbData = recv(Socket, 
                              IoBuffer + cbIoBuffer, 
                              IO_BUFFER_SIZE - cbIoBuffer, 
                              0);
                if(cbData == SOCKET_ERROR)
                {
                    XDBGTRC("Xonline", "**** Error %d reading data from server", WSAGetLastError());
                    scRet = SEC_E_INTERNAL_ERROR;
                    break;
                }
                else if(cbData == 0)
                {
                    XDBGTRC("Xonline", "**** Server unexpectedly disconnected");
                    scRet = SEC_E_INTERNAL_ERROR;
                    break;
                }


                if (fTime)
                {
                    tElapsed = GetTickCount() - tElapsed;
                    XDBGTRC("Xonline", "Response time - %d ms",tElapsed);
                }
                XDBGTRC("Xonline", "%d bytes of handshake data received", cbData);

                if (fDumpSSL)
                {
                    DumpSSL(cbIoBuffer + cbData, IoBuffer);
                }

                if(fVerbose)
                {
                    PrintHexDump(cbData, IoBuffer + cbIoBuffer);
                    XDBGTRC("Xonline", "");
                }

                cbIoBuffer += cbData;
            }
            else
            {
                fDoRead = TRUE;
            }
        }


        //
        // Set up the input buffers. Buffer 0 is used to pass in data
        // received from the server. Schannel will consume some or all
        // of this. Leftover data (if any) will be placed in buffer 1 and
        // given a buffer type of SECBUFFER_EXTRA.
        //

        InBuffers[0].pvBuffer   = IoBuffer;
        InBuffers[0].cbBuffer   = cbIoBuffer;
        InBuffers[0].BufferType = SECBUFFER_TOKEN;

        InBuffers[1].pvBuffer   = NULL;
        InBuffers[1].cbBuffer   = 0;
        InBuffers[1].BufferType = SECBUFFER_EMPTY;

        InBuffer.cBuffers       = 2;
        InBuffer.pBuffers       = InBuffers;
        InBuffer.ulVersion      = SECBUFFER_VERSION;

        //
        // Set up the output buffers. These are initialized to NULL
        // so as to make it less likely we'll attempt to free random
        // garbage later.
        //

        OutBuffers[0].pvBuffer  = NULL;
        OutBuffers[0].BufferType= SECBUFFER_TOKEN;
        OutBuffers[0].cbBuffer  = 0;

        OutBuffer.cBuffers      = 1;
        OutBuffer.pBuffers      = OutBuffers;
        OutBuffer.ulVersion     = SECBUFFER_VERSION;

        //
        // Call InitializeSecurityContext.
        //
        tElapsed = GetTickCount();
        scRet = InitializeSecurityContext(phCreds,
                                          phContext,
                                          NULL,
                                          dwSSPIFlags,
                                          0,
                                          SECURITY_NATIVE_DREP,
                                          &InBuffer,
                                          0,
                                          NULL,
                                          &OutBuffer,
                                          &dwSSPIOutFlags,
                                          &tsExpiry);


        if (fTime)
        {
            tElapsed = GetTickCount() - tElapsed;
            XDBGTRC("Xonline", "ISC - %d ms",tElapsed);
        }
        //
        // If InitializeSecurityContext was successful (or if the error was 
        // one of the special extended ones), send the contends of the output
        // buffer to the server.
        //

        if(scRet == SEC_E_OK                ||
           scRet == SEC_I_CONTINUE_NEEDED   ||
           FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR))
        {
            if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
            {
                cbData = send(Socket,
                              OutBuffers[0].pvBuffer,
                              OutBuffers[0].cbBuffer,
                              0);
                if(cbData == SOCKET_ERROR || cbData == 0)
                {
                    XDBGTRC("Xonline", "**** Error %d sending data to server (2)", 
                        WSAGetLastError());
                    FreeContextBuffer(OutBuffers[0].pvBuffer);
                    DeleteSecurityContext(phContext);
                    return SEC_E_INTERNAL_ERROR;
                }

                XDBGTRC("Xonline", "%d bytes of handshake data sent", cbData);

                   if (fDumpSSL)
                {
                    DumpSSL(cbData, OutBuffers[0].pvBuffer);
                }

                if(fVerbose)
                {
                    PrintHexDump(cbData, OutBuffers[0].pvBuffer);
                    XDBGTRC("Xonline", "");
                }

                // Free output buffer.
                FreeContextBuffer(OutBuffers[0].pvBuffer);
                OutBuffers[0].pvBuffer = NULL;
            }
        }


        //
        // If InitializeSecurityContext returned SEC_E_INCOMPLETE_MESSAGE,
        // then we need to read more data from the server and try again.
        //

        if(scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            continue;
        }


        //
        // If InitializeSecurityContext returned SEC_E_OK, then the 
        // handshake completed successfully.
        //

        if(scRet == SEC_E_OK)
        {
            //
            // If the "extra" buffer contains data, this is encrypted application
            // protocol layer stuff. It needs to be saved. The application layer
            // will later decrypt it with DecryptMessage.
            //

            XDBGTRC("Xonline", "Handshake was successful");

            if(InBuffers[1].BufferType == SECBUFFER_EXTRA)
            {
                pExtraData->pvBuffer = LocalAlloc(LMEM_FIXED, 
                                                  InBuffers[1].cbBuffer);
                if(pExtraData->pvBuffer == NULL)
                {
                    XDBGTRC("Xonline", "**** Out of memory (2)");
                    return SEC_E_INTERNAL_ERROR;
                }

                memmove(pExtraData->pvBuffer,
                           IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
                           InBuffers[1].cbBuffer);

                pExtraData->cbBuffer   = InBuffers[1].cbBuffer;
                pExtraData->BufferType = SECBUFFER_TOKEN;

                XDBGTRC("Xonline", "%d bytes of app data was bundled with handshake data",
                    pExtraData->cbBuffer);
            }
            else
            {
                pExtraData->pvBuffer   = NULL;
                pExtraData->cbBuffer   = 0;
                pExtraData->BufferType = SECBUFFER_EMPTY;
            }

            //
            // Bail out to quit
            //

            break;
        }


        //
        // Check for fatal error.
        //

        if(FAILED(scRet))
        {
            XDBGTRC("Xonline", "**** Error 0x%x returned by InitializeSecurityContext (2)", scRet);
            break;
        }


        //
        // If InitializeSecurityContext returned SEC_I_INCOMPLETE_CREDENTIALS,
        // then the server just requested client authentication. 
        //

        if(scRet == SEC_I_INCOMPLETE_CREDENTIALS)
        {
            //
            // Now would be a good time perhaps to prompt the user to select
            // a client certificate and obtain a new credential handle, 
            // but I don't have the energy nor inclination.
            //
            // As this is currently written, Schannel will send a "no 
            // certificate" alert to the server in place of a certificate. 
            // The server might be cool with this, or it might drop the 
            // connection.
            // 

            // Go around again.
            fDoRead = FALSE;
            scRet = SEC_I_CONTINUE_NEEDED;
        }


        //
        // Copy any leftover data from the "extra" buffer, and go around
        // again.
        //

        if ( InBuffers[1].BufferType == SECBUFFER_EXTRA )
        {
            memmove(IoBuffer,
                       IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
                       InBuffers[1].cbBuffer);

            cbIoBuffer = InBuffers[1].cbBuffer;
        }
        else
        {
            cbIoBuffer = 0;
        }
    }

    // Delete the security context in the case of a fatal error.
    if(FAILED(scRet))
    {
        DeleteSecurityContext(phContext);
    }

    LocalFree(IoBuffer);

    return scRet;
}


/*****************************************************************************/
static
HRESULT
HttpsGet(
    SOCKET Socket,
    PCredHandle phCreds,
    CtxtHandle *phContext,
    const XRL xrl,
    DWORD* pcbBuffer,
    PBYTE pBuffer,
    XRL_KNOWN_HEADERS *pHTTPHeaders,
    PXAPPSERVICEINFO pServiceInfo)
{
    SecPkgContext_StreamSizes Sizes;
    SECURITY_STATUS scRet;
    SecBufferDesc   Message;
    SecBuffer       Buffers[4];
    SecBuffer *     pDataBuffer;
    SecBuffer *     pExtraBuffer;
    SecBuffer       ExtraBuffer;
    //PFSealMessage    pfSealMessage;
    //PFUnsealMessage    pfUnsealMessage;

    PBYTE pbIoBuffer;
    DWORD cbIoBuffer;
    DWORD cbIoBufferLength;
    PBYTE pbMessage;
    DWORD cbMessage;

    HRESULT hr;
    DWORD cbData;
    INT   i;
    BOOL  fHeaderParsed = FALSE;
    DWORD cbBuffer = 0;
    PBYTE pHeaderEnd;
    PBYTE pHeaderStart = pBuffer;
    
    //
    // Read stream encryption properties.
    //

    scRet = QueryContextAttributes(phContext,
                                   SECPKG_ATTR_STREAM_SIZES,
                                   &Sizes);
    if(scRet != SEC_E_OK)
    {
        XDBGTRC("Xonline", "**** Error 0x%x reading SECPKG_ATTR_STREAM_SIZES", scRet);
        return scRet;
    }

    XDBGTRC("Xonline", "Header: %d, Trailer: %d, MaxMessage: %d",
        Sizes.cbHeader,
        Sizes.cbTrailer,
        Sizes.cbMaximumMessage);

    //
    // Allocate a working buffer. The plaintext sent to EncryptMessage
    // should never be more than 'Sizes.cbMaximumMessage', so a buffer 
    // size of this plus the header and trailer sizes should be safe enough.
    // 

    cbIoBufferLength = Sizes.cbHeader + 
                       Sizes.cbMaximumMessage +
                       Sizes.cbTrailer;

    pbIoBuffer = LocalAlloc(LMEM_FIXED, cbIoBufferLength);
    if(pbIoBuffer == NULL)
    {
        XDBGTRC("Xonline", "Out of memory (2)");
        return SEC_E_INTERNAL_ERROR;
    }

    // Build the HTTP request offset into the data buffer by "header size"
    // bytes. This enables Schannel to perform the encryption in place,
    // which is a significant performance win.
    pbMessage = pbIoBuffer + Sizes.cbHeader;

    //
    // Build an HTTP request to send to the server.
    //
    cbMessage = Sizes.cbMaximumMessage;
    hr = XRL_BuildGetRequest( pbMessage, &cbMessage, xrl, NULL, pServiceInfo );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }
    XDBGTRC("Xonline", "Sending plaintext: %d bytes", cbMessage);

    if(fVerbose)
    {
        PrintHexDump(cbMessage, pbMessage);
        XDBGTRC("Xonline", "");
    }

    //
    // Encrypt the HTTP request.
    //

    Buffers[0].pvBuffer     = pbIoBuffer;
    Buffers[0].cbBuffer     = Sizes.cbHeader;
    Buffers[0].BufferType   = SECBUFFER_STREAM_HEADER;

    Buffers[1].pvBuffer     = pbMessage;
    Buffers[1].cbBuffer     = cbMessage;
    Buffers[1].BufferType   = SECBUFFER_DATA;

    Buffers[2].pvBuffer     = pbMessage + cbMessage;
    Buffers[2].cbBuffer     = Sizes.cbTrailer;
    Buffers[2].BufferType   = SECBUFFER_STREAM_TRAILER;

    Buffers[3].BufferType   = SECBUFFER_EMPTY;

    Message.ulVersion       = SECBUFFER_VERSION;
    Message.cBuffers        = 4;
    Message.pBuffers        = Buffers;

#ifdef XBOX
    scRet = SealMessage(phContext, 0, &Message, 0);
#else
    pfSealMessage = (PFSealMessage) Reserved3;
    scRet = (*pfSealMessage)(phContext, 0, &Message, 0);
#endif

    if(FAILED(scRet))
    {
        XDBGTRC("Xonline", "Error 0x%x returned by EncryptMessage", scRet);
        hr = HRESULT_FROM_WIN32( scRet );
        goto Cleanup;
    }


    // 
    // Send the encrypted data to the server.
    //

    cbData = send(Socket,
                  pbIoBuffer,
                  Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer,
                  0);
    if(cbData == SOCKET_ERROR || cbData == 0)
    {
        XDBGTRC("Xonline", "**** Error %d sending data to server (3)", 
            WSAGetLastError());
        DeleteSecurityContext(phContext);
        hr = HRESULT_FROM_WIN32( SEC_E_INTERNAL_ERROR );
        goto Cleanup;
    }

    XDBGTRC("Xonline", "%d bytes of application data sent", cbData);

    if(fVerbose)
    {
        PrintHexDump(cbData, pbIoBuffer);
        XDBGTRC("Xonline", "");
    }

    //
    // Read data from server until done.
    //

    cbIoBuffer = 0;

    while(TRUE)
    {
        //
        // Read some data.
        //

        if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            cbData = recv(Socket, 
                          pbIoBuffer + cbIoBuffer, 
                          cbIoBufferLength - cbIoBuffer, 
                          0);
            if(cbData == SOCKET_ERROR)
            {
                hr = HRESULT_FROM_WIN32( WSAGetLastError() );
                XDBGTRC("Xonline", "Error %x reading data from server", hr);
                break;
            }
            else if(cbData == 0)
            {
                // Server disconnected.
                if(cbIoBuffer)
                {
                    XDBGTRC("Xonline", "Server unexpectedly disconnected");
                    hr = E_FAIL;
                    goto Cleanup;
                }
                else
                {
                    // This is the normal way to terminate the while loop
                    break;
                }
            }
            else
            {
                XDBGTRC("Xonline", "%d bytes of application data received", cbData);

                if(fVerbose)
                {
                    PrintHexDump(cbData, pbIoBuffer + cbIoBuffer);
                    XDBGTRC("Xonline", "");
                }

                cbIoBuffer += cbData;
            }
        }

        // 
        // Attempt to decrypt the received data.
        //

        Buffers[0].pvBuffer     = pbIoBuffer;
        Buffers[0].cbBuffer     = cbIoBuffer;
        Buffers[0].BufferType   = SECBUFFER_DATA;

        Buffers[1].BufferType   = SECBUFFER_EMPTY;
        Buffers[2].BufferType   = SECBUFFER_EMPTY;
        Buffers[3].BufferType   = SECBUFFER_EMPTY;

        Message.ulVersion       = SECBUFFER_VERSION;
        Message.cBuffers        = 4;
        Message.pBuffers        = Buffers;

#ifdef XBOX
        scRet = UnsealMessage(phContext, &Message, 0, NULL);
#else
        pfUnsealMessage = (PFUnsealMessage) Reserved4;
        scRet = (*pfUnsealMessage)(phContext, &Message, 0, NULL);
#endif

        if(scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            // The input buffer contains only a fragment of an
            // encrypted record. Loop around and read some more
            // data.
            continue;
        }

        if(scRet != SEC_E_OK && scRet != SEC_I_RENEGOTIATE)
        {
            XDBGTRC("Xonline", "Error 0x%x returned by UnsealMessage", scRet);
            hr = HRESULT_FROM_WIN32( scRet );
            goto Cleanup;
        }

        // Locate data and (optional) extra buffers.
        pDataBuffer  = NULL;
        pExtraBuffer = NULL;
        for(i = 1; i < 4; i++)
        {
            if(pDataBuffer == NULL && Buffers[i].BufferType == SECBUFFER_DATA)
            {
                pDataBuffer = &Buffers[i];
            }
            if(pExtraBuffer == NULL && Buffers[i].BufferType == SECBUFFER_EXTRA)
            {
                pExtraBuffer = &Buffers[i];
            }
        }

        // Display or otherwise process the decrypted data.
        if(pDataBuffer)
        {
            // Check if enough space in buffer
            // Make sure we always have one extra byte for a termination character.
            if (*pcbBuffer - cbBuffer > pDataBuffer->cbBuffer)
            {
                memmove(pBuffer, pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
                pBuffer += pDataBuffer->cbBuffer;
                cbBuffer += pDataBuffer->cbBuffer;
            }
            else
            {
                XDBGTRC("Xonline", "Error HttpsGet buffer too small");
                hr = HRESULT_FROM_WIN32( ERROR_INSUFFICIENT_BUFFER );
                goto Cleanup;
            }
            
            if (!fHeaderParsed)
            {
                // Null terminate to make sure strstr works
                pHeaderStart[ cbBuffer ] = '\0';
                pHeaderEnd = strstr( pHeaderStart, "\r\n\r\n" );                

                if (pHeaderEnd)
                {
                    // Calculate length of header
                    cbData = pHeaderEnd - pHeaderStart + 4;

                    // Parse the header
                    hr = XRL_ParseHeaders( pHeaderStart, cbData, pHTTPHeaders, pServiceInfo );
                    if ( FAILED(hr) )
                    {
                        XDBGTRC("Xonline", "Error 0x%x returned by XRL_ParseHeaders", hr );
                        goto Cleanup;
                    }
                    
                    fHeaderParsed = TRUE;

                    // Header is parsed, shift buffer to reuse header memory
                    memmove( pHeaderStart, pBuffer, cbBuffer - cbData );
                    cbBuffer -= cbData;
                    pBuffer = pHeaderStart + cbBuffer;
                }                
            }

            XDBGTRC("Xonline", "Received plaintext: %d bytes", pDataBuffer->cbBuffer);
            PrintHexDump(pDataBuffer->cbBuffer, pDataBuffer->pvBuffer);
        }

        // Move any "extra" data to the input buffer.
        if(pExtraBuffer)
        {
            memmove(pbIoBuffer, pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);
            cbIoBuffer = pExtraBuffer->cbBuffer;
        }
        else
        {
            cbIoBuffer = 0;
        }
       
        if(scRet == SEC_I_RENEGOTIATE)
        {
            // The server wants to perform another handshake
            // sequence.

            XDBGTRC("Xonline", "Server requested renegotiate!");

            scRet = ClientHandshakeLoop(Socket, 
                                        phCreds, 
                                        phContext, 
                                        FALSE, 
                                        &ExtraBuffer);
            if(scRet != SEC_E_OK)
            {
                hr = HRESULT_FROM_WIN32( scRet );
                goto Cleanup;
            }

            // Move any "extra" data to the input buffer.
            if(ExtraBuffer.pvBuffer)
            {
                memmove(pbIoBuffer, ExtraBuffer.pvBuffer, ExtraBuffer.cbBuffer);
                cbIoBuffer = ExtraBuffer.cbBuffer;
            }
        }
    }

    if (fHeaderParsed)
    {
        *pcbBuffer = cbBuffer;
    }
    else
    {
        XDBGTRC("Xonline", "No header");
        hr = E_FAIL;
        goto Cleanup;        
    }

    hr = S_OK;
    
Cleanup:
    LocalFree( pbIoBuffer );
    
    return hr;
}

#ifndef UNDER_CE
/*****************************************************************************/
static 
void
DisplayCertChain(
    PCCERT_CONTEXT pServerCert)
{
    TCHAR szName[1000];
    PCCERT_CONTEXT pCurrentCert;
    PCCERT_CONTEXT pIssuerCert;
    DWORD dwVerificationFlags;
    PCERT_ENHKEY_USAGE pEnhKeyUsage = NULL;
    UINT cbUsage, i;

    XDBGTRC("Xonline", "");

    // display leaf name
    if(!CertNameToStr(pServerCert->dwCertEncodingType,
                      &pServerCert->pCertInfo->Subject,
                      CERT_X500_NAME_STR | CERT_NAME_STR_NO_PLUS_FLAG,
                      szName, sizeof(szName)))
    {
        XDBGTRC("Xonline", "**** Error 0x%x building subject name", GetLastError());
    }
    XDBGTRC("Xonline", "Server subject: %s", szName);
    if(!CertNameToStr(pServerCert->dwCertEncodingType,
                      &pServerCert->pCertInfo->Issuer,
                      CERT_X500_NAME_STR | CERT_NAME_STR_NO_PLUS_FLAG,
                      szName, sizeof(szName)))
    {
        XDBGTRC("Xonline", "**** Error 0x%x building issuer name", GetLastError());
    }
    XDBGTRC("Xonline", "Server issuer: %s\n", szName);

    if (CertGetEnhancedKeyUsage(
            pServerCert,  // in - Pointer to the certificate
            0,       // in - Extension only flag
            NULL,        // out- Pointer to enhanced key usage
            &cbUsage))     // in/out - Size of enhanced key usage
    {
        pEnhKeyUsage = LocalAlloc(0,cbUsage);
        if (CertGetEnhancedKeyUsage(
                pServerCert,  // in - Pointer to the certificate
                0,       // in - Extension only flag
                pEnhKeyUsage,        // out- Pointer to enhanced key usage
                &cbUsage))     // in/out - Size of enhanced key usage
        {
            for (i=0;i<pEnhKeyUsage->cUsageIdentifier;i++)
            {
                XDBGTRC("Xonline", "Enhanced Key Usage : %s",pEnhKeyUsage->rgpszUsageIdentifier[i]);
            }
        }
    }

    // display certificate chain
    pCurrentCert = pServerCert;
    while(pCurrentCert != NULL)
    {
        dwVerificationFlags = 0;
        pIssuerCert = CertGetIssuerCertificateFromStore(pServerCert->hCertStore,
                                                        pCurrentCert,
                                                        NULL,
                                                        &dwVerificationFlags);
        if(pIssuerCert == NULL)
        {
            if(pCurrentCert != pServerCert)
            {
                CertFreeCertificateContext(pCurrentCert);
            }
            break;
        }

        if(!CertNameToStr(pIssuerCert->dwCertEncodingType,
                          &pIssuerCert->pCertInfo->Subject,
                          CERT_X500_NAME_STR | CERT_NAME_STR_NO_PLUS_FLAG,
                          szName, sizeof(szName)))
        {
            XDBGTRC("Xonline", "**** Error 0x%x building subject name", GetLastError());
        }
        XDBGTRC("Xonline", "CA subject: %s", szName);
        if(!CertNameToStr(pIssuerCert->dwCertEncodingType,
                          &pIssuerCert->pCertInfo->Issuer,
                          CERT_X500_NAME_STR | CERT_NAME_STR_NO_PLUS_FLAG,
                          szName, sizeof(szName)))
        {
            XDBGTRC("Xonline", "**** Error 0x%x building issuer name", GetLastError());
        }
        XDBGTRC("Xonline", "CA issuer: %s\n", szName);

        if(pCurrentCert != pServerCert)
        {
            CertFreeCertificateContext(pCurrentCert);
        }
        pCurrentCert = pIssuerCert;
        pIssuerCert = NULL;
    }
}

#endif

#if 0
/*****************************************************************************/
static 
DWORD
VerifyServerCertificate(
    PCCERT_CONTEXT  pServerCert,
    PSTR            pszServerName,
    DWORD           dwCertFlags)
{
    GUID                    gHTTPS = HTTPSPROV_ACTION;
    WINTRUST_DATA           sWTD;
    WINTRUST_CERT_INFO      sWTCI;
    HTTPSPolicyCallbackData polHttps;
    DWORD                   Status;
    SECURITY_STATUS         scRet;
    PWSTR                   pwszServerName;
    DWORD                   cchServerName;

    if(pServerCert == NULL)
    {
        return SEC_E_WRONG_PRINCIPAL;
    }
    if(pszServerName == NULL || strlen(pszServerName) == 0)
    {
        return SEC_E_WRONG_PRINCIPAL;
    }

    // Convert server name to unicode.
    cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, NULL, 0);
    pwszServerName = LocalAlloc(LMEM_FIXED, cchServerName * sizeof(WCHAR));
    if(pwszServerName == NULL)
    {
        return SEC_E_INSUFFICIENT_MEMORY;
    }
    cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, pwszServerName, cchServerName);
    if(cchServerName == 0)
    {
        return SEC_E_WRONG_PRINCIPAL;
    }

    ZeroMemory(&sWTD, sizeof(WINTRUST_DATA));
    sWTD.cbStruct               = sizeof(WINTRUST_DATA);
    sWTD.dwUIChoice             = WTD_UI_NONE;
    sWTD.pPolicyCallbackData    = (LPVOID)&polHttps;
    sWTD.dwUnionChoice          = WTD_CHOICE_CERT;
    sWTD.pCert                  = &sWTCI;
    sWTD.pwszURLReference       = NULL;

    ZeroMemory(&sWTCI, sizeof(WINTRUST_CERT_INFO));
    sWTCI.cbStruct              = sizeof(WINTRUST_CERT_INFO);
    sWTCI.psCertContext         = (CERT_CONTEXT *)pServerCert;
    sWTCI.chStores              = 1;
    sWTCI.pahStores             = (HCERTSTORE *)&pServerCert->hCertStore;
    sWTCI.pcwszDisplayName      = pwszServerName;

    ZeroMemory(&polHttps, sizeof(HTTPSPolicyCallbackData));
    polHttps.cbStruct           = sizeof(HTTPSPolicyCallbackData);
    polHttps.dwAuthType         = AUTHTYPE_SERVER;
    polHttps.fdwChecks          = dwCertFlags;
    polHttps.pwszServerName     = pwszServerName;


    Status = WinVerifyTrust(NULL, &gHTTPS, &sWTD);

    LocalFree(pwszServerName);
    pwszServerName = NULL;

    if(Status != ERROR_SUCCESS)
    {
        XDBGTRC("Xonline", "**** Error 0x%x returned by WinVerifyTrust", Status);
    }

    return Status;
}
#endif


/*****************************************************************************/

void
DisplayConnectionInfo(
    SecPkgContext_ConnectionInfo *pConnectionInfo)
{
    

    XDBGTRC("Xonline", "");

    switch(pConnectionInfo->dwProtocol)
    {
        case SP_PROT_SSL3_CLIENT:
            XDBGTRC("Xonline", "Protocol: SSL3");
            break;

        case SP_PROT_PCT1_CLIENT:
            XDBGTRC("Xonline", "Protocol: PCT");
            break;

        case SP_PROT_SSL2_CLIENT:
            XDBGTRC("Xonline", "Protocol: SSL2");
            break;

        default:
            XDBGTRC("Xonline", "Protocol: 0x%x", pConnectionInfo->dwProtocol);
    }

    switch(pConnectionInfo->aiCipher)
    {
        case CALG_RC4: 
            XDBGTRC("Xonline", "Cipher: RC4");
            break;

        case CALG_3DES: 
            XDBGTRC("Xonline", "Cipher: Triple DES");
            break;

        case CALG_RC2: 
            XDBGTRC("Xonline", "Cipher: RC2");
            break;

        case CALG_DES: 
            XDBGTRC("Xonline", "Cipher: DES");
            break;

        case CALG_SKIPJACK: 
            XDBGTRC("Xonline", "Cipher: Skipjack");
            break;

        default: 
            XDBGTRC("Xonline", "Cipher: 0x%x", pConnectionInfo->aiCipher);
    }

    XDBGTRC("Xonline", "Cipher strength: %d", pConnectionInfo->dwCipherStrength);

    switch(pConnectionInfo->aiHash)
    {
        case CALG_MD5: 
            XDBGTRC("Xonline", "Hash: MD5");
            break;

        case CALG_SHA: 
            XDBGTRC("Xonline", "Hash: SHA");
            break;

        default: 
            XDBGTRC("Xonline", "Hash: 0x%x", pConnectionInfo->aiHash);
    }

    XDBGTRC("Xonline", "Hash strength: %d", pConnectionInfo->dwHashStrength);

    switch(pConnectionInfo->aiExch)
    {
        case CALG_RSA_KEYX: 
        case CALG_RSA_SIGN: 
            XDBGTRC("Xonline", "Key exchange: RSA");
            break;

        case CALG_KEA_KEYX: 
            XDBGTRC("Xonline", "Key exchange: KEA");
            break;

        default: 
            XDBGTRC("Xonline", "Key exchange: 0x%x", pConnectionInfo->aiExch);
    }

    XDBGTRC("Xonline", "Key exchange strength: %d", pConnectionInfo->dwExchStrength);
}

typedef struct {
    BYTE contentType;
    BYTE majVer;
    BYTE minVer;
    BYTE lenHi;
    BYTE lenLo;
} SSL3HDR;

LPTSTR rszAlertLevel[] = {TEXT("Warning"),TEXT("Fatal")};
LPTSTR AlertType(UINT alertType)
{
    switch (alertType)
    {
    case 0:        return TEXT("close_notify");
    case 10:    return TEXT("unexpected_message");
    case 20:    return TEXT("bad_record_mac");
    case 30:    return TEXT("decompression_failure");
    case 40:    return TEXT("handshake_failure");
    case 41:    return TEXT("no_certificate");
    case 42:    return TEXT("bad_certificate");
    case 43:    return TEXT("unsupported_certificate");
    case 44:    return TEXT("certificate_revoked");
    case 45:    return TEXT("certificate_expired");
    case 46:    return TEXT("certificate_unknown");
    case 47:    return TEXT("illegal_parameter");
    default:
        break;
    }
    return TEXT("unknown alert type!");
}
static int
DumpSSL3Handshake(int length, const BYTE *buffer, int *pUsed, TCHAR szDump[])
{
    int cszDump = wsprintf(szDump,TEXT("Handshake: "));
    int cbMsg =(((int)buffer[1]) << 16)
                + (((int)buffer[2])<<8)
                + buffer[3];
    
    if (cbMsg > length-4)
    {
        cszDump += wsprintf(szDump+cszDump,TEXT("?\n"));
        *pUsed = length;
        return cszDump;
    }
    switch(buffer[0])
    {
    case 0:
        cszDump += wsprintf(szDump+cszDump,TEXT("hello_request"));
        break;
    case 1:
        cszDump += wsprintf(szDump+cszDump,TEXT("client_hello"));
        break;
    case 2:
        cszDump += wsprintf(szDump+cszDump,TEXT("server_hello"));
        break;
    case 11:
        cszDump += wsprintf(szDump+cszDump,TEXT("certificate"));
        break;
    case 12:
        cszDump += wsprintf(szDump+cszDump,TEXT("server_key_exchange"));
        break;
    case 13:
        cszDump += wsprintf(szDump+cszDump,TEXT("certificate_request"));
        break;
    case 14:
        cszDump += wsprintf(szDump+cszDump,TEXT("server_hello_done"));
        break;
    case 15:
        cszDump += wsprintf(szDump+cszDump,TEXT("certificate_verify"));
        break;
    case 16:
        cszDump += wsprintf(szDump+cszDump,TEXT("client_key_exchange"));
        break;
    case 20:
        cszDump += wsprintf(szDump+cszDump,TEXT("finished"));
        break;
    default:
        cszDump += wsprintf(szDump+cszDump,TEXT("Unknown HS type!"));
        break;
    }
    cszDump += wsprintf(szDump+cszDump,TEXT("\n"));
    *pUsed = cbMsg+4; 
    return cszDump;
}

typedef enum {
    CCS_TYPE = 20,
    ALERT_TYPE = 21,
    HANDSHAKE_TYPE = 22,
    APPDATA_TYPE = 23
} SSL3MsgTypes;

static void
DumpSSL(DWORD length, const BYTE *buffer)
{
    int cbLeft = (int) length;
    int cbContent;
    TCHAR szDump[2048];
    while (cbLeft > sizeof(SSL3HDR)) {
        int cszDump = 0;
        SSL3HDR *pSSL3Hdr = (SSL3HDR *) buffer;
        cbLeft -= sizeof(SSL3HDR);
        buffer += sizeof(SSL3HDR);
        cbContent = (((int)pSSL3Hdr->lenHi) << 8) + pSSL3Hdr->lenLo;
        if (cbLeft < cbContent)
            break;    // dont have the full message

        cbLeft -= cbContent;

        if (pSSL3Hdr->majVer != 3 && pSSL3Hdr->minVer != 0)
            break;    // only know about SSL3

        while (cbContent > 0)
        {
            cszDump = wsprintf(szDump, TEXT("SSL3 (%d bytes): "),cbContent);
            switch (pSSL3Hdr->contentType)
            {
            case CCS_TYPE:
                cszDump += wsprintf(szDump+cszDump,TEXT("Change Cipher Spec\n"));
                if (buffer[0] == 1)    // plaintext CCS msg
                {
                    cbContent -= 1;        // sizeof(CCS)
                    buffer += 1;
                } else // probably encrypted
                {
                    buffer += cbContent;
                    cbContent = 0;
                }
                break;
            case ALERT_TYPE:
                {
                UINT alertLevel = buffer[0];
                UINT alertType = buffer[1];
                if (alertLevel - 1 <= 1 )
                    cszDump += wsprintf(szDump+cszDump, TEXT("Alert : %s : %s\n"), rszAlertLevel[alertLevel-1], AlertType(alertType));
                cbContent -= 2;        // sizeof(ALERT)
                buffer += 2;
                break;
                }
            case HANDSHAKE_TYPE:
                {
                int cbContentUsed = 0;
                cszDump += DumpSSL3Handshake(cbContent, buffer, &cbContentUsed,szDump+cszDump);
                cbContent -= cbContentUsed;
                buffer += cbContentUsed;
                break;
                }

            case APPDATA_TYPE:
                cszDump += wsprintf(szDump+cszDump, TEXT("App Data\n"));
                buffer += cbContent;
                break;
            default:
                cszDump += wsprintf(szDump+cszDump,TEXT("Unknown Record!\n"));
                cbLeft = -1;
                cbContent = 0;
                break;
            }
            // XDBGTRC("Xonline", "%s", szDump);
        }
    }
}

/*****************************************************************************/
void 
PrintHexDump(DWORD length, PBYTE buffer)
{
    DWORD i,count,index;
    CHAR rgbDigits[]="0123456789abcdef";
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
                rgbLine[cbLine++] = ':';
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
        XDBGTRC("Xonline", "%s", rgbLine);
    }
}

