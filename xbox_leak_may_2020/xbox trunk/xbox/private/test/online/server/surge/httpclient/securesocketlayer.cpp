/*

Copyright (C) 1999 Microsoft Corporation

Module Name:

    SecureSocketLayer.cpp

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

*/
#include "stdafx.h"
#include "SecureSocketLayer.h"

#define SSL_RECV_TIMEOUT 4 // recv timeout in sec

#ifdef _DEBUG
void _cdecl TraceDbg(LPCTSTR lpszFormat, ...)
    {
    int nBuf;
    char szBuffer[1024];

    va_list args;
    va_start(args, lpszFormat);

    nBuf = vsprintf(szBuffer, lpszFormat, args);
    OutputDebugString(szBuffer);

    va_end(args);
    }
#endif

CSecureSocketLayer::CSecureSocketLayer(LPSTR pszUserName /* =NULL */, DWORD dwProtocol /* =SP_PROT_SSL2 */, ALG_ID  aiKeyExch /* =CALG_RSA_KEYX */ )
    {
    try 
        {
        hSecuritydll = NULL;
        bConnected = FALSE;
        SecInvalidateHandle(&hClientCreds); // set dwUpper & dwLower to -1
        SecInvalidateHandle(&hContext);
        ZeroMemory(&SchannelCred, sizeof(SchannelCred));
        hMyCertStore = NULL;

        LoadSecurityLibrary();

        InitializeClient(pszUserName, dwProtocol, aiKeyExch);
        }
    catch(...)
        {
        SecInvalidateHandle(&hClientCreds); // set dwUpper & dwLower to -1
        SecInvalidateHandle(&hContext);
        #ifdef _DEBUG
        TraceDbg("CSecureSocketLayer::CSecureSocketLayer Execption Caught!");
        #endif
        }
    }


CSecureSocketLayer::~CSecureSocketLayer()
    {
    try 
        {
        // hClientCred and hMyCertStore are created in InitializeClient()
        if(hClientCreds.dwLower != -1 && hClientCreds.dwUpper != -1)
            {
            SecurityFunc.FreeCredentialsHandle(&hClientCreds);
            }
        CertCloseStore(hMyCertStore, 0);
        }
    catch(...)
        {
        #ifdef _DEBUG
        TraceDbg("CSecureSocketLayer::~CSecureSocketLayer Execption Caught!");
        #endif
        }
    UnloadSecurityLibrary();
    }

SECURITY_STATUS CSecureSocketLayer::Connect(SOCKET sock, const char *server)
    {
    if(!server) return SEC_E_INTERNAL_ERROR;

    SECURITY_STATUS scRet = SEC_E_OK;
    try 
        {
        // Client-Server Handshake
        scRet = PerformClientHandshake(sock, server);
        if(scRet == SEC_E_OK)
            {
            bConnected = TRUE;

            // Check Cert
            // TODO
            //scRet = SecurityFunc.QueryContextAttributes(&hContext, SECPKG_ATTR_REMOTE_CERT_CONTEXT, (PVOID)&pRemoteCertContext);
            //if(!FAILED(scRet)) scRet = VerifyServerCertificate(pRemoteCertContext, server, 0);
            }
        else
            {
            bConnected = FALSE;
            }
        }
    catch(...)
        {
        bConnected = FALSE;
        SecInvalidateHandle(&hContext);
        #ifdef _DEBUG
        TraceDbg("CSecureSocketLayer::Connect Execption Caught!");
        #endif
        scRet = SEC_E_INTERNAL_ERROR;
        }

    return scRet;
    }

SECURITY_STATUS CSecureSocketLayer::Encrypt(char *data, DWORD &dataLen, DWORD bufferLen)
    {
    // dataLen gets modified to the new length
    SECURITY_STATUS scRet = SEC_E_OK;

    try 
        {
        SecPkgContext_StreamSizes Sizes;
        SecBufferDesc   Message;
        SecBuffer       Buffers[4];

        // Mapping of information in the buffer:
        //   data[0]                         start of the SSL header
        //   data[Sizes.cbHeader]            start of the actual message
        //   data[Sizes.cbHeader + dataLen]  start of the SSL trailer

        scRet = SecurityFunc.QueryContextAttributes(&hContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);
        if(FAILED(scRet))
            {
            dataLen = 0;
            }

        // encryption will be done inplace
        else if(Sizes.cbHeader + dataLen + Sizes.cbTrailer >= bufferLen)
            {
            // this message would make data overflow
            dataLen = 0;
            scRet = SEC_E_BUFFER_TOO_SMALL;
            }
        else
            {
            // NOTE: The plaintext sent to EncryptMessage
            //       should never be more than 'Sizes.cbMaximumMessage'

            // Header
            int dataBuff = 0;
            unsigned long dataSize = 0;
            if(Sizes.cbHeader != 0)
                {
                Buffers[dataBuff].pvBuffer = data;
                Buffers[dataBuff].cbBuffer = Sizes.cbHeader;
                Buffers[dataBuff].BufferType = SECBUFFER_STREAM_HEADER;
                dataSize += Buffers[dataBuff].cbBuffer;
                ++dataBuff;
                }

            // Data
            memmove(data + Sizes.cbHeader, data, dataLen);
            Buffers[dataBuff].pvBuffer = data + Sizes.cbHeader;
            Buffers[dataBuff].cbBuffer = dataLen;
            Buffers[dataBuff].BufferType = SECBUFFER_DATA;
            dataSize += Buffers[dataBuff].cbBuffer;
            ++dataBuff;

            // Trailer
            if(Sizes.cbTrailer != 0)
                {
                Buffers[dataBuff].pvBuffer = data + Sizes.cbHeader + dataLen;
                Buffers[dataBuff].cbBuffer = Sizes.cbTrailer;
                Buffers[dataBuff].BufferType = SECBUFFER_STREAM_TRAILER;
                dataSize += Buffers[dataBuff].cbBuffer;
                ++dataBuff;
                }

            // Ending
            Buffers[dataBuff].BufferType = SECBUFFER_EMPTY;
            ++dataBuff;

            Message.ulVersion = SECBUFFER_VERSION;
            Message.cBuffers = dataBuff;
            Message.pBuffers = Buffers;

            dataLen = Sizes.cbHeader + dataLen + Sizes.cbTrailer;

            // Reserved3 == EncryptMessage
            scRet = SecurityFunc.Reserved3(&hContext, 0, &Message, 0);
            }
        }
    catch(...)
        {
        #ifdef _DEBUG
        TraceDbg("CSecureSocketLayer::Encrypt Execption Caught!");
        #endif
        scRet = SEC_E_INTERNAL_ERROR;
        }

    return scRet;
    }

SECURITY_STATUS CSecureSocketLayer::Decrypt(char *data, DWORD &dataLen, DWORD bufferLen)
    {
    // dataLen gets modified to the new length
    SECURITY_STATUS scRet = SEC_E_OK;

    try 
        {
        SecBufferDesc   Message;
        SecBuffer       Buffers[4];

        Buffers[0].pvBuffer     = data;
        Buffers[0].cbBuffer     = dataLen;
        Buffers[0].BufferType   = SECBUFFER_DATA;

        Buffers[1].BufferType   = SECBUFFER_EMPTY;
        Buffers[2].BufferType   = SECBUFFER_EMPTY;
        Buffers[3].BufferType   = SECBUFFER_EMPTY;

        Message.ulVersion       = SECBUFFER_VERSION;
        Message.cBuffers        = 4;
        Message.pBuffers        = Buffers;

        // Reserved4 == DecryptMessage
        scRet = SecurityFunc.Reserved4(&hContext, &Message, 0, NULL);
        if(!FAILED(scRet))
            {
            // This loop moves the data around and decrypts extra data if needed
            for(unsigned long i=0, offset=0; i<Message.cBuffers; i++)
                {
                if(Buffers[i].BufferType == SECBUFFER_STREAM_HEADER)
                    {
                    offset += Buffers[i].cbBuffer;
                    }

                // Here is the data that actually got decrypted
                else if(Buffers[i].BufferType == SECBUFFER_DATA)
                    {
                    memmove(data, data+offset, Buffers[i].cbBuffer);
                    data[Buffers[i].cbBuffer] = '\0';
                    dataLen = Buffers[i].cbBuffer;
                    offset = Buffers[i].cbBuffer;
                    }

                // Here is the data that still needs to be decrypted
                else if(Buffers[i].BufferType == SECBUFFER_EXTRA)
                    {
                    DWORD len = Buffers[i].cbBuffer;
                    Decrypt((char*)Buffers[i].pvBuffer, len, bufferLen-dataLen);
                    memmove(data+offset, Buffers[i].pvBuffer, len);
                    dataLen += len;
                    }
                }
            }
        else
            {
            dataLen = 0;
            }
        }
    catch(...)
        {
        #ifdef _DEBUG
        TraceDbg("CSecureSocketLayer::Decrypt Execption Caught!");
        #endif
        scRet = SEC_E_INTERNAL_ERROR;
        }

    return scRet;
    }

SECURITY_STATUS CSecureSocketLayer::Disconnect(SOCKET sock)
    {
    SECURITY_STATUS scRet = SEC_E_OK;

    try 
        {
        if(bConnected)
            {
            scRet = DisconnectFromServer(sock);
            }
        if(hContext.dwLower != -1 || hContext.dwUpper != -1)
            {
            // hContext is created in PerformClientHandshake() on success
            SecurityFunc.DeleteSecurityContext(&hContext);
            }
        }
    catch(...)
        {
        #ifdef _DEBUG
        TraceDbg("CSecureSocketLayer::Disconnect Execption Caught!");
        #endif
        scRet = SEC_E_INTERNAL_ERROR;
        }

    return scRet;
    }


/*****************************************************************************/
BOOL CSecureSocketLayer::LoadSecurityLibrary(void)
    {
    PSecurityFunctionTable  pSecurityFunc;
    INIT_SECURITY_INTERFACE pInitSecurityInterface;

    hSecuritydll = LoadLibrary("security.dll");
    if(hSecuritydll == NULL)
        {
        #ifdef _DEBUG
        TraceDbg("Error 0x%x loading security.dll.\n", GetLastError());
        #endif
        return FALSE;
        }

    pInitSecurityInterface = (INIT_SECURITY_INTERFACE)GetProcAddress(hSecuritydll, "InitSecurityInterfaceA");
    
    if(pInitSecurityInterface == NULL)
        {
        #ifdef _DEBUG
        TraceDbg("Error 0x%x reading InitSecurityInterface entry point.\n", GetLastError());
        #endif
        return FALSE;
        }

    pSecurityFunc = pInitSecurityInterface();

    if(pSecurityFunc == NULL)
        {
        #ifdef _DEBUG
        TraceDbg("Error 0x%x reading security interface.\n", GetLastError());
        #endif
        return FALSE;
        }

    CopyMemory(&SecurityFunc, pSecurityFunc, sizeof(SecurityFunc));

    return TRUE;
    }

/*****************************************************************************/
void CSecureSocketLayer::UnloadSecurityLibrary(void)
    {
    FreeLibrary(hSecuritydll);
    hSecuritydll = NULL;
    }

/*****************************************************************************/
SECURITY_STATUS CSecureSocketLayer::InitializeClient(
    LPSTR pszUserName,
    DWORD dwProtocol,
    ALG_ID  aiKeyExch)
    {
    TimeStamp       tsExpiry;
    SECURITY_STATUS status;

    DWORD           cSupportedAlgs = 0;
    ALG_ID          rgbSupportedAlgs[16];

    PCCERT_CONTEXT  pCertContext = NULL;


    // Open the "MY" certificate store, which is where Internet Explorer
    // stores its client certificates.
    if(hMyCertStore == NULL)
        {
        hMyCertStore = CertOpenSystemStore(0, "MY");

        if(!hMyCertStore)
            {
            #ifdef _DEBUG
            TraceDbg("**** Error 0x%x returned by CertOpenSystemStore\n", GetLastError());
            #endif
            return SEC_E_NO_CREDENTIALS;
            }
        }

    // If a user name is specified, then attempt to find a client
    // certificate. Otherwise, just create a NULL credential.
    if(pszUserName)
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
            #ifdef _DEBUG
            TraceDbg("**** Error 0x%x returned by CertFindCertificateInStore\n", GetLastError());
            #endif
            return SEC_E_NO_CREDENTIALS;
            }
        }


    // Build Schannel credential structure. Currently, this sample only
    // specifies the protocol to be used (and optionally the certificate, 
    // of course). Real applications may wish to specify other parameters 
    // as well.
    SchannelCred.dwVersion = SCHANNEL_CRED_VERSION;
    if(pCertContext)
        {
        SchannelCred.cCreds = 1;
        SchannelCred.paCred = &pCertContext;
        }

    SchannelCred.grbitEnabledProtocols = dwProtocol;

    if(aiKeyExch)
        {
        rgbSupportedAlgs[cSupportedAlgs++] = aiKeyExch;
        }

    if(cSupportedAlgs)
        {
        SchannelCred.cSupportedAlgs    = cSupportedAlgs;
        SchannelCred.palgSupportedAlgs = rgbSupportedAlgs;
        }

    SchannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;
    SchannelCred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;

    // Create an SSPI credential.
    status = SecurityFunc.AcquireCredentialsHandleA(
                        NULL,                   // Name of principal    
                        UNISP_NAME_A,           // Name of package
                        SECPKG_CRED_OUTBOUND,   // Flags indicating use
                        NULL,                   // Pointer to logon ID
                        &SchannelCred,          // Package specific data
                        NULL,                   // Pointer to GetKey() func
                        NULL,                   // Value to pass to GetKey()
                        &hClientCreds,          // (out) Cred Handle
                        &tsExpiry);             // (out) Lifetime (optional)

    if(pCertContext) CertFreeCertificateContext(pCertContext);

    if(FAILED(status))
        {
        #ifdef _DEBUG
        TraceDbg("**** Error 0x%x returned by AcquireCredentialsHandle\n", status);
        #endif
        }
    return status;
    }

/*****************************************************************************/
SECURITY_STATUS CSecureSocketLayer::PerformClientHandshake(
    SOCKET          Socket,
    const char*     pszServerName)
    {
    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    SECURITY_STATUS scRet;
    int           cbData;

    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

    // Initiate a ClientHello message and generate a token.
    OutBuffers[0].pvBuffer   = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer   = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    // build a unique identifier
    char *target;
    unsigned length = strlen(pszServerName);
    target = new char[length + 10];
    strcpy(target, pszServerName);
    for(int i=0; i<9; i++)
        {
        target[length+i] = (rand() % 26) + 'A';
        }
    target[length + 9] = '\0';

    scRet = SecurityFunc.InitializeSecurityContextA(
                    &hClientCreds,
                    NULL,
                    target, // use a unique value to avoid caching of creds (char*)pszServerName,
                    dwSSPIFlags,
                    0,
                    SECURITY_NATIVE_DREP,
                    NULL,
                    0,
                    &hContext,
                    &OutBuffer,
                    &dwSSPIOutFlags,
                    &tsExpiry);
    delete[] target;

    if(scRet != SEC_I_CONTINUE_NEEDED)
        {
        #ifdef _DEBUG
        TraceDbg("**** Error %Lu returned by InitializeSecurityContext (1)\n", scRet);
        #endif
        SecInvalidateHandle(&hClientCreds); // set dwUpper & dwLower to -1
        SecInvalidateHandle(&hContext); // set dwUpper & dwLower to -1
        if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
            {
            SecurityFunc.FreeContextBuffer(OutBuffers[0].pvBuffer);
            }
        return scRet;
        }

    // Send response to server if there is one
    if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
        {
        cbData = send(Socket, (char*)OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer, 0);

        // Free output buffer.
        SecurityFunc.FreeContextBuffer(OutBuffers[0].pvBuffer);
        OutBuffers[0].pvBuffer = NULL;

        if(cbData == SOCKET_ERROR || cbData == 0)
            {
            scRet = WSAGetLastError();
            #ifdef _DEBUG
            TraceDbg("**** Error %d sending data to server (1)\n", (int)scRet);
            #endif
            return scRet;
            }

        }

    scRet = ClientHandshakeLoop(Socket, TRUE);

    return scRet;
    }

/*****************************************************************************/
SECURITY_STATUS CSecureSocketLayer::ClientHandshakeLoop(
    SOCKET          Socket,
    BOOL            fDoRead)
    {
    SECURITY_STATUS scRet;
    SecBufferDesc   InBuffer;
    SecBuffer       InBuffers[2];
    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    int             cbData;

    ExtraData.pvBuffer   = NULL;
    ExtraData.cbBuffer   = 0;
    ExtraData.BufferType = SECBUFFER_EMPTY;


    char           IoBuffer[IO_BUFFER_SIZE*2];
    DWORD           cbIoBuffer;

    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

    cbIoBuffer = 0;


    scRet = SEC_I_CONTINUE_NEEDED; // this comes from the initial call in PerformClientHandshake

    // loop control
    UINT count = 0;

    // Loop until the handshake is finished or an error occurs.
    while(scRet == SEC_I_CONTINUE_NEEDED || scRet == SEC_E_INCOMPLETE_MESSAGE || scRet == SEC_I_INCOMPLETE_CREDENTIALS) 
	    {
        if(++count == 100)
            {
            break; // bail after a while (returns the previous scRet error)
            }

        // Read data from server.
        if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE)
            {
            if(fDoRead)
                {
                TIMEVAL timeout;
                timeout.tv_sec = SSL_RECV_TIMEOUT;
                timeout.tv_usec = 0;

                FD_SET bucket;
                bucket.fd_count = 1;
                bucket.fd_array[0] = Socket;

                // wait 5 seconds for data to come
                int err = select(0, &bucket, NULL, NULL, &timeout);
                if(err == 0 || err == SOCKET_ERROR)
                    {
                    scRet = WSAETIMEDOUT;
                    break;
                    }

                cbData = recv(Socket, IoBuffer + cbIoBuffer, IO_BUFFER_SIZE - cbIoBuffer, 0);
                if(cbData == SOCKET_ERROR)
                    {
                    scRet = WSAGetLastError();
                    #ifdef _DEBUG
                    TraceDbg("**** Error %d reading data from server\n", (int)scRet);
                    #endif
                    break;
                    }

                cbIoBuffer += cbData;
                }
            else
                {
                fDoRead = TRUE;
                }
            }


        // Set up the input buffers. Buffer 0 is used to pass in data
        // received from the server. Schannel will consume some or all
        // of this. Leftover data (if any) will be placed in buffer 1 and
        // given a buffer type of SECBUFFER_EXTRA.
        InBuffers[0].pvBuffer   = IoBuffer;
        InBuffers[0].cbBuffer   = cbIoBuffer;
        InBuffers[0].BufferType = SECBUFFER_TOKEN;

        InBuffers[1].pvBuffer   = NULL;
        InBuffers[1].cbBuffer   = 0;
        InBuffers[1].BufferType = SECBUFFER_EMPTY;

        InBuffer.cBuffers       = 2;
        InBuffer.pBuffers       = InBuffers;
        InBuffer.ulVersion      = SECBUFFER_VERSION;

        // Set up the output buffers. These are initialized to NULL
        // so as to make it less likely we'll attempt to free random
        // garbage later.
        OutBuffers[0].pvBuffer  = NULL;
        OutBuffers[0].BufferType= SECBUFFER_TOKEN;
        OutBuffers[0].cbBuffer  = 0;

        OutBuffer.cBuffers      = 1;
        OutBuffer.pBuffers      = OutBuffers;
        OutBuffer.ulVersion     = SECBUFFER_VERSION;



        // Call InitializeSecurityContext.
        scRet = SecurityFunc.InitializeSecurityContextA(&hClientCreds,
                                          &hContext,
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


        // If InitializeSecurityContext was successful (or if the error was 
        // one of the special extended ones), send the contends of the output
        // buffer to the server.
        if(scRet == SEC_E_OK || scRet == SEC_I_CONTINUE_NEEDED || (FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR)))
            {
            if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
                {
                cbData = send(Socket, (char*)OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer, 0);

                // Free output buffer.
                SecurityFunc.FreeContextBuffer(OutBuffers[0].pvBuffer);
                OutBuffers[0].pvBuffer = NULL;

                if(cbData == SOCKET_ERROR || cbData == 0)
                    {
                    scRet = WSAGetLastError();
                    #ifdef _DEBUG
                    TraceDbg("**** Error %d sending data to server (2)\n", (int)scRet);
                    #endif
                    break;
                    }

                }
            }


        // If InitializeSecurityContext returned SEC_E_INCOMPLETE_MESSAGE,
        // then we need to read more data from the server and try again.
        if(scRet == SEC_E_INCOMPLETE_MESSAGE)
            {
            continue;
            }

        // If InitializeSecurityContext returned SEC_E_OK, then the 
        // handshake completed successfully.
        if(scRet == SEC_E_OK)
            {
            // If the "extra" buffer contains data, this is encrypted application
            // protocol layer stuff. It needs to be saved. The application layer
            // will later decrypt it with DecryptMessage.

            ExtraData.pvBuffer   = NULL;
            ExtraData.cbBuffer   = 0;
            ExtraData.BufferType = SECBUFFER_EMPTY;

            // This is commented out since our application layer 
            // (CHttpClient) doesnt care about it
            /*
            if(InBuffers[1].BufferType == SECBUFFER_EXTRA)
                {
                ExtraData.pvBuffer = LocalAlloc(LMEM_FIXED, InBuffers[1].cbBuffer);
                if(ExtraData.pvBuffer == NULL)
                    {
                    return SEC_E_INSUFFICIENT_MEMORY;
                    }
                MoveMemory(ExtraData.pvBuffer, IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer), InBuffers[1].cbBuffer);
                ExtraData.cbBuffer   = InBuffers[1].cbBuffer;
                ExtraData.BufferType = SECBUFFER_TOKEN;
                }
            */

            // we are done with handshake
            break;
            }

        // Check for fatal error.
        if(FAILED(scRet))
            {
            #ifdef _DEBUG
            TraceDbg("**** Error 0x%x returned by InitializeSecurityContext (2)\n", scRet);
            #endif
            break;
            }


        // If InitializeSecurityContext returned SEC_I_INCOMPLETE_CREDENTIALS,
        // then the server just requested client authentication. 
        if(scRet == SEC_I_INCOMPLETE_CREDENTIALS)
            {
            // Display trusted issuers info. 
            GetNewClientCredentials(&hClientCreds, &hContext);

            // Now would be a good time perhaps to prompt the user to select
            // a client certificate and obtain a new credential handle, 
            // but I don't have the energy nor inclination.
            //
            // As this is currently written, Schannel will send a "no 
            // certificate" alert to the server in place of a certificate. 
            // The server might be cool with this, or it might drop the 
            // connection.

            // Go around again.
            fDoRead = FALSE;
            scRet = SEC_I_CONTINUE_NEEDED;
            continue;
            }


        // Copy any leftover data from the "extra" buffer, and go around again.
        if ( InBuffers[1].BufferType == SECBUFFER_EXTRA )
            {
            MoveMemory(IoBuffer,
                       IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
                       InBuffers[1].cbBuffer);

            cbIoBuffer = InBuffers[1].cbBuffer;
            }
        else
            {
            cbIoBuffer = 0;
            }
        }

    // Uncomment this if we actually use the ExtraData.pvBuffer (see above)
    //if(ExtraData.pvBuffer) LocalFree(ExtraData.pvBuffer);

    return scRet;
    }

/*****************************************************************************/
void CSecureSocketLayer::GetNewClientCredentials(
    CredHandle *phCreds,
    CtxtHandle *phContext)
    {
    CredHandle hCreds;
    SecPkgContext_IssuerListInfoEx IssuerListInfo;
    PCCERT_CHAIN_CONTEXT pChainContext;
    CERT_CHAIN_FIND_BY_ISSUER_PARA FindByIssuerPara;
    PCCERT_CONTEXT  pCertContext;
    TimeStamp       tsExpiry;
    SECURITY_STATUS Status;

    // Read list of trusted issuers from schannel.
    Status = SecurityFunc.QueryContextAttributes(phContext,
                                    SECPKG_ATTR_ISSUER_LIST_EX,
                                    (PVOID)&IssuerListInfo);
    if(Status != SEC_E_OK)
        {
        #ifdef _DEBUG
        TraceDbg("Error 0x%x querying issuer list info\n", Status);
        #endif
        return;
        }

    // Enumerate the client certificates.
    ZeroMemory(&FindByIssuerPara, sizeof(FindByIssuerPara));

    FindByIssuerPara.cbSize = sizeof(FindByIssuerPara);
    FindByIssuerPara.pszUsageIdentifier = szOID_PKIX_KP_CLIENT_AUTH;
    FindByIssuerPara.dwKeySpec = 0;
    FindByIssuerPara.cIssuer   = IssuerListInfo.cIssuers;
    FindByIssuerPara.rgIssuer  = IssuerListInfo.aIssuers;

    pChainContext = NULL;

    UINT count = 0;

    while(TRUE)
        {
        if(++count >= 100)
            {
            break;
            }

        // Find a certificate chain.
        pChainContext = CertFindChainInStore(hMyCertStore,
                                             X509_ASN_ENCODING,
                                             0,
                                             CERT_CHAIN_FIND_BY_ISSUER,
                                             &FindByIssuerPara,
                                             pChainContext);
        if(pChainContext == NULL)
            {
            #ifdef _DEBUG
            TraceDbg("Error 0x%x finding cert chain\n", GetLastError());
            #endif
            break;
            }

        // Get pointer to leaf certificate context.
        pCertContext = pChainContext->rgpChain[0]->rgpElement[0]->pCertContext;

        // Create schannel credential.
        SchannelCred.cCreds = 1;
        SchannelCred.paCred = &pCertContext;

        Status = SecurityFunc.AcquireCredentialsHandleA(
                            NULL,                   // Name of principal
                            UNISP_NAME_A,           // Name of package
                            SECPKG_CRED_OUTBOUND,   // Flags indicating use
                            NULL,                   // Pointer to logon ID
                            &SchannelCred,          // Package specific data
                            NULL,                   // Pointer to GetKey() func
                            NULL,                   // Value to pass to GetKey()
                            &hCreds,                // (out) Cred Handle
                            &tsExpiry);             // (out) Lifetime (optional)
        if(Status != SEC_E_OK)
            {
            #ifdef _DEBUG
            TraceDbg("**** Error 0x%x returned by AcquireCredentialsHandle\n", Status);
            #endif
            continue;
            }

        // Destroy the old credentials.
        SecurityFunc.FreeCredentialsHandle(phCreds);
        SecInvalidateHandle(&hClientCreds);

        *phCreds = hCreds;

        break;
        }
    }

/*****************************************************************************/
long CSecureSocketLayer::VerifyServerCertificate(
    PCCERT_CONTEXT  pServerCert,
    const char*     pszServerName,
    DWORD           dwCertFlags)
    {
    HTTPSPolicyCallbackData  polHttps;
    CERT_CHAIN_POLICY_PARA   PolicyPara;
    CERT_CHAIN_POLICY_STATUS PolicyStatus;
    CERT_CHAIN_PARA          ChainPara;
    PCCERT_CHAIN_CONTEXT     pChainContext = NULL;

    long   Status;
    PWSTR   pwszServerName;
    DWORD   cchServerName;

    if(pServerCert == NULL)
        {
        return SEC_E_WRONG_PRINCIPAL;
        }


    // Convert server name to unicode.
    if(pszServerName == NULL || strlen(pszServerName) == 0)
        {
        return SEC_E_WRONG_PRINCIPAL;
        }

    cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, NULL, 0);
    pwszServerName = (unsigned short*)LocalAlloc(LMEM_FIXED, cchServerName * sizeof(WCHAR));
    if(pwszServerName == NULL)
        {
        return SEC_E_INSUFFICIENT_MEMORY;
        }
    cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, pwszServerName, cchServerName);
    if(cchServerName == 0)
        {
        LocalFree(pwszServerName);
        return SEC_E_WRONG_PRINCIPAL;
        }


    // Build certificate chain.
    ZeroMemory(&ChainPara, sizeof(ChainPara));
    ChainPara.cbSize = sizeof(ChainPara);

    if(!CertGetCertificateChain(
                            NULL,
                            pServerCert,
                            NULL,
                            pServerCert->hCertStore,
                            &ChainPara,
                            0,
                            NULL,
                            &pChainContext))
        {
        Status = GetLastError();
        #ifdef _DEBUG
        TraceDbg("Error 0x%x returned by CertGetCertificateChain!\n", Status);
        #endif
        goto cleanup;
        }


    // Validate certificate chain.
    ZeroMemory(&polHttps, sizeof(HTTPSPolicyCallbackData));
    polHttps.cbStruct           = sizeof(HTTPSPolicyCallbackData);
    polHttps.dwAuthType         = AUTHTYPE_SERVER;
    polHttps.fdwChecks          = dwCertFlags;
    polHttps.pwszServerName     = pwszServerName;

    memset(&PolicyPara, 0, sizeof(PolicyPara));
    PolicyPara.cbSize            = sizeof(PolicyPara);
    PolicyPara.pvExtraPolicyPara = &polHttps;

    memset(&PolicyStatus, 0, sizeof(PolicyStatus));
    PolicyStatus.cbSize = sizeof(PolicyStatus);

    CertVerifyCertificateChainPolicy(
                            CERT_CHAIN_POLICY_SSL,
                            pChainContext,
                            &PolicyPara,
                            &PolicyStatus);

    /* Error Values for PolicyStatus.dwError
    0
        Ok
    TRUST_E_CERT_SIGNATURE 
        The signature of the certificate cannot be verified. 
    CERT_E_UNTRUSTEDROOT 
        A certification chain was processed correctly but terminated in a root certificate that is not trusted by the trust provider. 
    CERT_E_UNTRUSTEDTESTROOT 
        The root certificate is a testing certificate and the policy settings disallow test certificates. 
    CERT_E_CHAINING 
        A chain of certificates was not correctly created. 
    CERT_E_WRONG_USAGE 
        The certificate is not valid for the requested usage. 
    CERT_E_EXPIRED 
        A required certificate is not within its validity period. 
    CERT_E_VALIDITYPERIODNESTING 
        The validity periods of the certification chain do not nest correctly. 
    CERT_E_PURPOSE 
        A certificate is being used for a purpose other than that for which it is permitted. 
    TRUST_E_BASIC_CONSTRAINTS 
        The certificate's basic constraints are invalid or missing. 
    CERT_E_ROLE 
        A certificate that can only be used as an end-entity is being used as a CA or visa versa. 
    CERT_E_CN_NO_MATCH 
        The certificate's CN name does not match the passed value. 
    CRYPT_E_REVOKED 
        The certificate or signature has been revoked. 
    CRYPT_E_REVOCATION_OFFLINE 
        The revocation server was offline because the called function wasn't able to complete the revocation check. 
    CERT_E_REVOKED 
        A certificate in the chain was explicitly revoked by its issuer. 
    CERT_E_REVOCATION_FAILURE 
        The revocation process could not continue. The certificates could not be checked. 
    */

    if(PolicyStatus.dwError)
        {
        Status = SEC_E_UNTRUSTED_ROOT;
        goto cleanup;
        }

    Status = SEC_E_OK;

cleanup:


    if(pwszServerName) LocalFree(pwszServerName);

    if(pChainContext)
        {
        CertFreeCertificateChain(pChainContext);
        }

    return Status;
    }

/*****************************************************************************/
long CSecureSocketLayer::DisconnectFromServer(
    SOCKET          Socket)
    {
    DWORD           dwType;
    char*           pbMessage;
    DWORD           cbMessage;
    int             cbData;

    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    long            status;

    //
    // Notify schannel that we are about to close the connection.
    //
    dwType = SCHANNEL_SHUTDOWN;

    OutBuffers[0].pvBuffer   = &dwType;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer   = sizeof(dwType);

    OutBuffer.cBuffers  = 1;
    OutBuffer.pBuffers  = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    status = SecurityFunc.ApplyControlToken(&hContext, &OutBuffer);

    if(FAILED(status)) 
        {
        #ifdef _DEBUG
        TraceDbg("**** Error 0x%x returned by ApplyControlToken\n", status);
        #endif
        return status;
        }

    // Build an SSL close notify message.
    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

    OutBuffers[0].pvBuffer   = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer   = 0;

    OutBuffer.cBuffers  = 1;
    OutBuffer.pBuffers  = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    status = SecurityFunc.InitializeSecurityContextA(
                    &hClientCreds,
                    &hContext,
                    NULL,
                    dwSSPIFlags,
                    0,
                    SECURITY_NATIVE_DREP,
                    NULL,
                    0,
                    &hContext,
                    &OutBuffer,
                    &dwSSPIOutFlags,
                    &tsExpiry);

    if(FAILED(status))
        {
        #ifdef _DEBUG
        TraceDbg("**** Error 0x%x returned by InitializeSecurityContext\n", status);
        #endif
        return status;
        }

    pbMessage = (char*)OutBuffers[0].pvBuffer;
    cbMessage = OutBuffers[0].cbBuffer;


    // Send the close notify message to the server.
    if(pbMessage != NULL && cbMessage != 0)
        {
        cbData = send(Socket, pbMessage, cbMessage, 0);
        if(cbData == SOCKET_ERROR || cbData == 0)
            {
            status = WSAGetLastError();
            #ifdef _DEBUG
            TraceDbg("**** Error %d sending close notify\n", status);
            #endif
            }

        // Free output buffer.
        SecurityFunc.FreeContextBuffer(pbMessage);
        }

    return status;
    }

