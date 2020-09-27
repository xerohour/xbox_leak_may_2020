/*

Copyright (C) 1999 Microsoft Corporation

Module Name:

    SecureSocketLayer.h

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

*/
#ifndef _SECURESOCKETLAYER_H_
#define _SECURESOCKETLAYER_H_

// NOTE: Include files with error codes
//       SSL       <issperr.h> 
//       Sockets   <winsock2.h>
//       HTTP      <wininet.h>
//


// support non NT build environments that do not set this,
// 0x0400 is minimum required for security headers
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#define SECURITY_WIN32
#include <wincrypt.h>
#include <schannel.h>
#include <security.h>
#include <softpub.h>

#define IO_BUFFER_SIZE  0x10000

// This table duplicates the current security table but it has
// the proper types associated with the Reserved3 and 4 functions
typedef struct _MYSECURITY_FUNCTION_TABLE_A {
    unsigned long                       dwVersion;
    ENUMERATE_SECURITY_PACKAGES_FN_A    EnumerateSecurityPackagesA;
    QUERY_CREDENTIALS_ATTRIBUTES_FN_A   QueryCredentialsAttributesA;
    ACQUIRE_CREDENTIALS_HANDLE_FN_A     AcquireCredentialsHandleA;
    FREE_CREDENTIALS_HANDLE_FN          FreeCredentialHandle;
    #ifndef WIN32_CHICAGO
        void SEC_FAR *                  Reserved2;
    #else
        SSPI_LOGON_USER_FN              SspiLogonUserA;
    #endif
    INITIALIZE_SECURITY_CONTEXT_FN_A    InitializeSecurityContextA;
    ACCEPT_SECURITY_CONTEXT_FN          AcceptSecurityContext;
    COMPLETE_AUTH_TOKEN_FN              CompleteAuthToken;
    DELETE_SECURITY_CONTEXT_FN          DeleteSecurityContext;
    APPLY_CONTROL_TOKEN_FN              ApplyControlToken;
    QUERY_CONTEXT_ATTRIBUTES_FN_A       QueryContextAttributesA;
    IMPERSONATE_SECURITY_CONTEXT_FN     ImpersonateSecurityContext;
    REVERT_SECURITY_CONTEXT_FN          RevertSecurityContext;
    MAKE_SIGNATURE_FN                   MakeSignature;
    VERIFY_SIGNATURE_FN                 VerifySignature;
    FREE_CONTEXT_BUFFER_FN              FreeContextBuffer;
    QUERY_SECURITY_PACKAGE_INFO_FN_A    QuerySecurityPackageInfoA;
    ENCRYPT_MESSAGE_FN                  Reserved3;
    DECRYPT_MESSAGE_FN                  Reserved4;
    EXPORT_SECURITY_CONTEXT_FN          ExportSecurityContext;
    IMPORT_SECURITY_CONTEXT_FN_A        ImportSecurityContextA;
    void SEC_FAR *                      Reserved7;
    void SEC_FAR *                      Reserved8;
    QUERY_SECURITY_CONTEXT_TOKEN_FN     QuerySecurityContextToken;
    ENCRYPT_MESSAGE_FN                  EncryptMessage;
    DECRYPT_MESSAGE_FN                  DecryptMessage;
} MySecurityFunctionTable, SEC_FAR* PMySecurityFunctionTable;

class CSecureSocketLayer
    {
    private:
        HMODULE hSecuritydll;
        MySecurityFunctionTable SecurityFunc;

        // vars used by security.dll
        CredHandle hClientCreds;
        CtxtHandle hContext;
        HCERTSTORE      hMyCertStore;
        SCHANNEL_CRED   SchannelCred;
        SecBuffer  ExtraData;
        PCCERT_CONTEXT pRemoteCertContext;
        BOOL bConnected;

    public:
        CSecureSocketLayer(LPSTR pszUserName=NULL, DWORD dwProtocol = SP_PROT_SSL2, ALG_ID aiKeyExch = CALG_RSA_KEYX);
        ~CSecureSocketLayer();

    public:
        SECURITY_STATUS Connect(SOCKET sock, const char *server);
        SECURITY_STATUS Encrypt(char *data, DWORD &dataLen, DWORD bufferLen);
        SECURITY_STATUS Decrypt(char *data, DWORD &dataLen, DWORD bufferLen);
        SECURITY_STATUS Disconnect(SOCKET sock);

        SECURITY_STATUS ClientHandshakeLoop(SOCKET Socket, BOOL fDoRead);

    private: // internal helper functions
        BOOL LoadSecurityLibrary(void);
        void UnloadSecurityLibrary(void);
        SECURITY_STATUS InitializeClient(LPSTR pszUserName, DWORD dwProtocol, ALG_ID aiKeyExch);
        SECURITY_STATUS PerformClientHandshake(SOCKET Socket, const char* pszServerName);
        void GetNewClientCredentials(CredHandle *phCreds, CtxtHandle *phContext);
        long VerifyServerCertificate(PCCERT_CONTEXT  pServerCert, const char* pszServerName, DWORD dwCertFlags);
        long DisconnectFromServer(SOCKET Socket);

    };

#endif // _SECURESOCKETLAYER_H_