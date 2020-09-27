//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:   context.c
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    8-08-95   RichardW   Created
//  8-13-95   TerenceS   PCTized
//
//----------------------------------------------------------------------------

#include <spbase.h>
#include <security.h>
#include <certmap.h>
#include <ssl2msg.h>
#include <ssl3msg.h>
#include <pct1msg.h>
#include <rsa.h>
#include "spsspi.h"

extern BOOLEAN CryptoOk;

#ifndef SECPKG_ATTR_PROTO_INFO

//
// We seem to not have the new sspi.h file yet.  So as to not break any builds,
// we'll just sneak in the missing definitions here.
//

#define SECPKG_ATTR_PROTO_INFO      7
typedef struct _SecPkgContext_ProtoInfoA
{
    SEC_CHAR SEC_FAR * sProtocolName;
    unsigned long majorVersion;
    unsigned long minorVersion;
} SecPkgContext_ProtoInfoA, SEC_FAR * PSecPkgContext_ProtoInfoA;

typedef struct _SecPkgContext_ProtoInfoW
{
    SEC_WCHAR SEC_FAR * sProtocolName;
    unsigned long majorVersion;
    unsigned long minorVersion;
} SecPkgContext_ProtoInfoW, SEC_FAR * PSecPkgContext_ProtoInfoW;

#ifdef UNICODE
#define SecPkgContext_ProtoInfo   SecPkgContext_ProtoInfoW
#define PSecPkgContext_ProtoInfo  PSecPkgContext_ProtoInfoW
#else
#define SecPkgContext_ProtoInfo   SecPkgContext_ProtoInfoA
#define PSecPkgContext_ProtoInfo  PSecPkgContext_ProtoInfoA
#endif

#endif  // !SECPKG_ATTR_PROTO_INFO


typedef struct {
    DWORD dwProtoId;
    LPCSTR szProto;
    DWORD dwMajor;
    DWORD dwMinor;
} PROTO_ID;

const PROTO_ID
rgProts[]
    = {
        { SP_PROT_SSL2_CLIENT, "SSL", 2, 0 },
        { SP_PROT_SSL2_SERVER, "SSL", 2, 0 },
        { SP_PROT_PCT1_CLIENT, "PCT", 1, 0 },
        { SP_PROT_PCT1_SERVER, "PCT", 1, 0 },
        { SP_PROT_SSL3_CLIENT, "SSL", 3, 0 },
        { SP_PROT_SSL3_SERVER, "SSL", 3, 0 }
};

SECURITY_STATUS SEC_ENTRY
InitializeSecurityContextA(
    PCredHandle             phCred,         // Cred to base context
    PCtxtHandle             phContext,      // Existing context (OPT)
    SEC_CHAR SEC_FAR *      pszTargetName,  // Name of target
    unsigned long           fContextReq,    // Context Requirements
    unsigned long           Reserved1,      // Reserved, MBZ
    unsigned long           TargetDataRep,  // Data rep of target
    PSecBufferDesc          pInput,         // Input Buffers
    unsigned long           Reserved2,      // Reserved, MBZ
    PCtxtHandle             phNewContext,   // (out) New Context handle
    PSecBufferDesc          pOutput,        // (inout) Output Buffers
    unsigned long SEC_FAR * pfContextAttr,  // (out) Context attrs
    PTimeStamp              ptsExpiry       // (out) Life span (OPT)
)
{
    PSPContext  pContext = NULL;
    PSPCredential pCred = NULL;
    SPBuffer CommOut;
    SPBuffer CommIn;
    PSecBuffer pInToken = NULL;
    PSecBuffer pOutToken = NULL;
    PSecBuffer pExtra = NULL;
    unsigned long fAttr;
    BOOL fCache;

    DWORD         fContext = CONTEXT_FLAG_CLIENT;
    int i;

    SP_STATUS pctRet = PCT_ERR_OK;

    if (!CryptoOk)
    {
        return (SEC_E_UNSUPPORTED_FUNCTION );
    }

    /* These flags are never allowed */
    if (fContextReq & (ISC_REQ_DELEGATE |
                       ISC_REQ_INTEGRITY |
                       ISC_REQ_PROMPT_FOR_CREDS |
                       ISC_REQ_USE_DCE_STYLE |
                       ISC_REQ_DATAGRAM))
    {
       return SEC_E_UNSUPPORTED_FUNCTION;
    }

    fAttr = ISC_RET_REPLAY_DETECT | ISC_RET_SEQUENCE_DETECT | ISC_RET_CONFIDENTIALITY;

    if(fContextReq & ISC_REQ_MUTUAL_AUTH)
    {
         fAttr |= ISC_RET_MUTUAL_AUTH;
         fContext |= CONTEXT_FLAG_CLIAUTH;
    }
    if (fContextReq & (ISC_REQ_EXTENDED_ERROR))
    {
        fAttr |= ISC_RET_EXTENDED_ERROR;
        fContext |= CONTEXT_FLAG_EXT_ERR;
    }
    if (fContextReq & (ISC_REQ_CONNECTION))
    {
        fAttr |= ISC_RET_CONNECTION;
        fContext |= CONTEXT_FLAG_CONNECTION_MODE;
        if (fContextReq & (ISC_REQ_STREAM))
        {
           return SEC_E_UNSUPPORTED_FUNCTION;
        }
    }
    if (fContextReq & (ISC_REQ_STREAM))
    {
        fAttr |= ISC_RET_STREAM;
    }

    if (fContextReq & (ISC_REQ_USE_SESSION_KEY))
    {
        fAttr |= ISC_RET_USE_SESSION_KEY;
        fContext |= CONTEXT_FLAG_NOCACHE;
    }
    if(fContextReq & (ISC_REQ_USE_SUPPLIED_CREDS)) {
        fAttr |= ISC_REQ_USE_SUPPLIED_CREDS;
        fContext |= CONTEXT_FLAG_NO_INCOMPLETE_CRED_MSG;
    }


    /* Initialize output buffer locations */
    for (i = 0; i < (int)pOutput->cBuffers; i++ )
    {
        switch(pOutput->pBuffers[i].BufferType)
        {
            case SECBUFFER_EMPTY:
                if(!pOutToken)
                    pOutToken = &pOutput->pBuffers[i];
                break;

            case SECBUFFER_TOKEN:
                    pOutToken = &pOutput->pBuffers[i];
                break;
            case SECBUFFER_DATA:
            case SECBUFFER_STREAM_HEADER:
            case SECBUFFER_STREAM_TRAILER:
            default:
                break;
        }
    }
    if (pOutToken == NULL)
    {
        return SEC_E_INVALID_TOKEN;
    }
    pOutToken->BufferType = SECBUFFER_TOKEN;




    if (Reserved2)
    {
        return(SEC_E_UNSUPPORTED_FUNCTION);
    }


    if (fContextReq & ISC_REQ_ALLOCATE_MEMORY)
    {
        fAttr |= ISC_RET_ALLOCATED_MEMORY;
        pOutToken->pvBuffer = NULL;
        pOutToken->cbBuffer = 0;
    }
    else
    {
        if(pOutToken->pvBuffer == NULL)
        {
            return (SEC_E_INVALID_HANDLE);
        }
    }

    CommOut.pvBuffer = pOutToken->pvBuffer;
    CommOut.cbBuffer = pOutToken->cbBuffer;
    CommOut.cbData = 0;


    if(pfContextAttr)
    {
        *pfContextAttr = fAttr;
    }

    if(phContext == NULL)
    {
        pContext = SPContextCreate(pszTargetName);
        if(pContext == NULL)
        {
            return SEC_E_INSUFFICIENT_MEMORY;
        }
        pCred = ValidateCredentialHandle(phCred);

        if(pctRet == PCT_ERR_OK)
        {
            pctRet = SPContextSetCredentials(pContext, pCred);
        }

        if(pctRet != PCT_ERR_OK){
            SPContextDelete(pContext);
           if(phNewContext) phNewContext->dwUpper = 0;
           if(phContext) phContext->dwUpper = 0;
            return PctTranslateError(pctRet);
        }

        //KLUDGE: to make sure that we call with Vanilla SSL2 because of OLD servers
        if(pInput)
        {
            pctRet = (DWORD) SEC_E_UNSUPPORTED_FUNCTION;
            /* Initialize intput buffer locations */
            for (i = 0; i < (int)pInput->cBuffers; i++ )
            {
                switch(pInput->pBuffers[i].BufferType)
                {
                    case SECBUFFER_PKG_PARAMS:
                        pInToken = &pInput->pBuffers[i];
                        break;
                    case SECBUFFER_EMPTY:
                        if(!pExtra)
                        {
                            pExtra = &pInput->pBuffers[i];
                        }
                    break;
                    default:
                        break;
                }
            }

            if (!pInToken)
            {
                return(pctRet);
            }

            if (pInToken->cbBuffer != sizeof(DWORD))
            {
                return(pctRet);
            }
        }

        // use session cache only if target name specified
        fCache = (pszTargetName && pszTargetName[0]); 
        pctRet = pContext->InitiateHello(pContext, &CommOut, fCache);
        if(CommOut.cbBuffer == 0 && pctRet == PCT_INT_BUFF_TOO_SMALL)
            return SEC_E_INSUFFICIENT_MEMORY;
        if(pctRet != PCT_ERR_OK) {
            SPContextDelete(pContext);
            if(phNewContext) phNewContext->dwUpper = 0;
            if(phContext) phContext->dwUpper = 0;
            return PctTranslateError(pctRet);
        }
        phNewContext->dwUpper = (DWORD) pContext;
        /* the following is because there is a requirement with the sspi
         * implementation that the lower word of the context handle be equal
         * to the index of the package in the table returned by EnumerateSecurityPackages
         */

        if(pCred)
        {
            phNewContext->dwLower = phCred->dwLower;
        }
        else
        {
            phNewContext->dwLower = 0;
        }
        pOutToken->pvBuffer = CommOut.pvBuffer;
        pOutToken->cbBuffer = CommOut.cbBuffer;

    }
    else
    {
        /* Initialize intput buffer locations */
        for (i = 0; i < (int)pInput->cBuffers; i++ )
        {
            switch(pInput->pBuffers[i].BufferType)
            {
                case SECBUFFER_TOKEN:
                case SECBUFFER_TOKEN | SECBUFFER_READONLY:
                    pInToken = &pInput->pBuffers[i];
                    break;
                case SECBUFFER_EMPTY:
                    if(!pInToken)
                    {
                        pInToken = &pInput->pBuffers[i];
                    }
                    else if(!pExtra)
                    {
                        pExtra = &pInput->pBuffers[i];
                    }
                break;

                case SECBUFFER_DATA:
                case SECBUFFER_STREAM_HEADER:
                case SECBUFFER_STREAM_TRAILER:
                default:
                    break;
            }
        }
        if(pInToken == NULL)
        {
            return SEC_E_INVALID_TOKEN;
        }
        CommIn.pvBuffer = pInToken->pvBuffer;
        CommIn.cbBuffer = pInToken->cbBuffer;
        CommIn.cbData = pInToken->cbBuffer;

        pContext= ValidateContextHandle(phContext);
        pCred = ValidateCredentialHandle(phCred);

        if(pContext == NULL || pCred == NULL)
        {
            return( SEC_E_INVALID_HANDLE );
        }

        pctRet = SPContextSetCredentials(pContext, pCred);

        pctRet =  pContext->ProtocolHandler(pContext,
                                         &CommIn,
                                         &CommOut,
                                         NULL);
        if(CommOut.cbBuffer == 0 && pctRet == PCT_INT_BUFF_TOO_SMALL) {
            CommOut.pvBuffer = SPExternalAlloc(CommOut.cbData);
            if (NULL == CommOut.pvBuffer)
                return SEC_E_INSUFFICIENT_MEMORY;
            pctRet =  pContext->ProtocolHandler(pContext,
                                         &CommIn,
                                         &CommOut,
                                         NULL);
        }

        if(pctRet == PCT_INT_INCOMPLETE_MSG)
        {
            if(pExtra)
            {
                pExtra->BufferType = SECBUFFER_MISSING;
                pExtra->cbBuffer = CommIn.cbData -  pInToken->cbBuffer;
            }
        } else {
            pOutToken->pvBuffer = CommOut.pvBuffer;
            pOutToken->cbBuffer = CommOut.cbData;
        }

        if(SP_FATAL(pctRet))
        {
           SPContextDelete(pContext);
           if(phNewContext) phNewContext->dwUpper = 0;
           if(phContext) phContext->dwUpper = 0;
        }

        if(PCT_ERR_OK != pctRet)
        {
            return PctTranslateError(pctRet);
        }
        if(CommIn.cbData < pInToken->cbBuffer && pExtra) {
            pExtra->BufferType = SECBUFFER_EXTRA;
            pExtra->cbBuffer = pInToken->cbBuffer - CommIn.cbData;
        }

    }
    return(pContext->State == SP_STATE_CONNECTED?SEC_E_OK:SEC_I_CONTINUE_NEEDED);
}


SECURITY_STATUS SEC_ENTRY
InitializeSecurityContextW(
    PCredHandle             phCredential,   // Cred to base context
    PCtxtHandle             phContext,      // Existing context (OPT)
    SEC_WCHAR SEC_FAR *     pszTargetName,  // Name of target
    unsigned long           fContextReq,    // Context Requirements
    unsigned long           Reserved1,      // Reserved, MBZ
    unsigned long           TargetDataRep,  // Data rep of target
    PSecBufferDesc          pInput,     // Input Buffers
    unsigned long           Reserved2,      // Reserved, MBZ
    PCtxtHandle             phNewContext,   // (out) New Context handle
    PSecBufferDesc          pOutput,        // (inout) Output Buffers
    unsigned long SEC_FAR * pfContextAttr,  // (out) Context attrs
    PTimeStamp              ptsExpiry)      // (out) Life span (OPT)
{
    PCHAR   pszAnsiTarget;
    DWORD   cchTarget;
    SECURITY_STATUS scRet;

    if (pszTargetName != NULL)
    {
        cchTarget = lstrlenW(pszTargetName) + 1;
        pszAnsiTarget = SPExternalAlloc(cchTarget * sizeof(WCHAR));

        if (pszAnsiTarget == NULL)
        {
            return(SEC_E_INSUFFICIENT_MEMORY);
        }

        WideCharToMultiByte(
                    CP_ACP,
                    0,
                    pszTargetName,
                    cchTarget,
                    pszAnsiTarget,
                    cchTarget * sizeof(WCHAR),
                    NULL,
                    NULL);
    }
    else
    {
        pszAnsiTarget = NULL;
    }

    scRet = InitializeSecurityContextA(phCredential,
                                       phContext,
                                       pszAnsiTarget,
                                       fContextReq,
                                       Reserved1,
                                       TargetDataRep,
                                       pInput,
                                       Reserved2,
                                       phNewContext,
                                       pOutput,
                                       pfContextAttr,
                                       ptsExpiry );

    if (pszAnsiTarget != NULL)
    {
        SPExternalFree(pszAnsiTarget);
    }
    return(scRet);
}


SECURITY_STATUS SEC_ENTRY
AcceptSecurityContext(
    PCredHandle             phCred,         // Cred to base context
    PCtxtHandle             phContext,      // Existing context (OPT)
    PSecBufferDesc          pInput,         // Input buffer
    unsigned long           fContextReq,    // Context Requirements
    unsigned long           TargetDataRep,  // Target Data Rep
    PCtxtHandle             phNewContext,   // (out) New context handle
    PSecBufferDesc          pOutput,        // (inout) Output buffers
    unsigned long SEC_FAR * pfContextAttr,  // (out) Context attributes
    PTimeStamp              ptsExpiry       // (out) Life span (OPT)
)
{
    PSPContext  pContext = NULL;
    PSPCredential pCred = NULL;
    SPBuffer CommOut;
    SPBuffer CommIn;
    PSecBuffer pInToken = NULL;
    PSecBuffer pOutToken = NULL;
    PSecBuffer pExtra = NULL;

    // SCHANNEL Contexts always have these req's .
    unsigned long fAttr;

    DWORD         fContext = 0;
    int i;
    SP_STATUS pctRet = PCT_ERR_OK;

    if (!CryptoOk)
    {
        return( SEC_E_UNSUPPORTED_FUNCTION );
    }

    /* These flags are never allowed */
    if (fContextReq & (ASC_REQ_DELEGATE |
                       ASC_REQ_INTEGRITY |
                       ASC_REQ_USE_DCE_STYLE |
                       ASC_REQ_DATAGRAM))
    {
       return SEC_E_UNSUPPORTED_FUNCTION;
    }

    fAttr = ASC_RET_REPLAY_DETECT | ASC_RET_SEQUENCE_DETECT | ASC_RET_CONFIDENTIALITY;

    if(fContextReq & ASC_REQ_MUTUAL_AUTH)
    {
         fAttr |= ASC_RET_MUTUAL_AUTH;
         fContext |= CONTEXT_FLAG_CLIAUTH;
    }
    if (fContextReq & (ASC_REQ_EXTENDED_ERROR))
    {
        fAttr |= ASC_RET_EXTENDED_ERROR;
        fContext |= CONTEXT_FLAG_EXT_ERR;
    }
    if (fContextReq & (ASC_REQ_CONNECTION))
    {
        fAttr |= ASC_RET_CONNECTION;
        fContext |= CONTEXT_FLAG_CONNECTION_MODE;
        if (fContextReq & (ASC_REQ_STREAM))
        {
           return SEC_E_UNSUPPORTED_FUNCTION;
        }
    }
    if (fContextReq & (ASC_REQ_STREAM))
    {
        fAttr |= ASC_RET_STREAM;
    }

    if (fContextReq & (ASC_REQ_USE_SESSION_KEY))
    {
        fAttr |= ASC_RET_USE_SESSION_KEY;
        fContext |= CONTEXT_FLAG_NOCACHE;
    }

    /* Initialize intput buffer locations */
    for (i = 0; i < (int)pInput->cBuffers; i++ )
    {
        switch(pInput->pBuffers[i].BufferType)
        {
            case SECBUFFER_TOKEN:
            case SECBUFFER_TOKEN | SECBUFFER_READONLY:
                pInToken = &pInput->pBuffers[i];
                break;
            case SECBUFFER_EMPTY:
                if(!pInToken)
                {
                    pInToken = &pInput->pBuffers[i];
                }
                else if(!pExtra)
                {
                    pExtra = &pInput->pBuffers[i];
                }
                break;

            case SECBUFFER_DATA:
            case SECBUFFER_STREAM_HEADER:
            case SECBUFFER_STREAM_TRAILER:
            default:
                break;
        }
    }

    /* Initialize output buffer locations */
    for (i = 0; i < (int) pOutput->cBuffers; i++ )
    {
        switch(pOutput->pBuffers[i].BufferType)
        {
            case SECBUFFER_EMPTY:
                if(!pOutToken)
                    pOutToken = &pOutput->pBuffers[i];
                break;
            case SECBUFFER_TOKEN:
                    pOutToken = &pOutput->pBuffers[i];
                break;
            case SECBUFFER_DATA:
            case SECBUFFER_STREAM_HEADER:
            case SECBUFFER_STREAM_TRAILER:
            default:
                break;
        }
    }

    if (pInToken == NULL || pOutToken == NULL)
    {
        return SEC_E_INVALID_TOKEN;
    }
    pOutToken->BufferType = SECBUFFER_TOKEN;

    if(pInToken->BufferType == SECBUFFER_TOKEN)
    {
        CommIn.pvBuffer = pInToken->pvBuffer;
        CommIn.cbBuffer = pInToken->cbBuffer;
        CommIn.cbData = pInToken->cbBuffer;
    }
    else
    {
        CommIn.pvBuffer = NULL;
        CommIn.cbBuffer = 0;
        CommIn.cbData = 0;
    }
    if (fContextReq & ASC_REQ_ALLOCATE_MEMORY)
    {
        fAttr |= ASC_RET_ALLOCATED_MEMORY;
        pOutToken->pvBuffer = NULL;
        pOutToken->cbBuffer = 0;
    }
    else
    {
        if(pOutToken->pvBuffer == NULL)
        {
            return (SEC_E_INVALID_HANDLE);
        }
    }

    CommOut.pvBuffer = pOutToken->pvBuffer;
    CommOut.cbBuffer = pOutToken->cbBuffer;
    CommOut.cbData = 0;

    if (pfContextAttr)
    {
        *pfContextAttr = fAttr;
    }

    if (phContext == NULL)
    {
        pContext = SPContextCreate(NULL);
        if (pContext == NULL)
        {
            return SEC_E_INSUFFICIENT_MEMORY;
        }

    }
    else
    {
        pContext= ValidateContextHandle(phContext);
    }

    pCred = ValidateCredentialHandle(phCred);

    if(pContext == NULL || pCred == NULL)
    {
        return( SEC_E_INVALID_HANDLE );
    }

    pctRet = SPContextSetCredentials(pContext, pCred);

    pContext->Flags = fContext;

    if(pctRet == PCT_ERR_OK)
    {
        pctRet = pContext->ProtocolHandler(pContext, &CommIn, &CommOut, NULL);
    }

    if(phContext == NULL)
    {
        if(pctRet != PCT_ERR_OK)
        {
            SPContextDelete(pContext);
        }
        else
        {
            phNewContext->dwUpper = (DWORD) pContext;

            // The following is because there is a requirement with the sspi
            // implementation that the lower word of the context handle be equal
            // to the index of the package in the table returned by
            // EnumerateSecurityPackages

            if(pCred)
            {
                phNewContext->dwLower = phCred->dwLower;
            }
            else
            {
                phNewContext->dwLower = 0;
            }
        }
    }
    else
    {
        if(SP_FATAL(pctRet))
        {
           SPContextDelete(pContext);
           if(phNewContext) phNewContext->dwUpper = 0;
           if(phContext) phContext->dwUpper = 0;
        }
    }

    if (CommOut.cbBuffer == 0 && pctRet == PCT_INT_BUFF_TOO_SMALL)
    {
        return SEC_E_INSUFFICIENT_MEMORY;
    }
    if (pctRet == PCT_INT_INCOMPLETE_MSG) {
        if(pExtra)
        {
            pExtra->BufferType = SECBUFFER_MISSING;
            pExtra->cbBuffer = CommIn.cbData -  pInToken->cbBuffer;
        }
    } else {
        pOutToken->pvBuffer = CommOut.pvBuffer;
        pOutToken->cbBuffer = CommOut.cbData;
    }

    if (PCT_ERR_OK != pctRet)
    {
       return PctTranslateError(pctRet);
    }
    if (CommIn.cbData < pInToken->cbBuffer && pExtra) {
        pExtra->BufferType = SECBUFFER_EXTRA;
        pExtra->cbBuffer = pInToken->cbBuffer - CommIn.cbData;
    }

    return (pContext->State == SP_STATE_CONNECTED?SEC_E_OK : SEC_I_CONTINUE_NEEDED);
}


SECURITY_STATUS SEC_ENTRY
CompleteAuthToken(
    PCtxtHandle phContext,              // Context to complete
    PSecBufferDesc pToken               // Token to complete
    )
{
    PSPContext  pContext;

    pContext = ValidateContextHandle(phContext);

    if (!pContext)
    {
        return( SEC_E_INVALID_HANDLE );
    }
    return (SEC_E_OK);
}



SECURITY_STATUS SEC_ENTRY
DeleteSecurityContext(
        PCtxtHandle         phContext       // Context to delete
)
{
    PSPContext  pContext;

    pContext = ValidateContextHandle(phContext);

    if (!pContext)
    {
        return( SEC_E_INVALID_HANDLE );
    }

    SPContextDelete( pContext );
    return (SEC_E_OK);
}


SECURITY_STATUS SEC_ENTRY
QueryContextAttributesA(
    PCtxtHandle         phContext,      // Context to query
    unsigned long           ulAttribute,    // Attribute to query
    void SEC_FAR *          pBuffer     // Buffer for attributes
)
{
    PSPContext                  pContext;
    PSecPkgContext_StreamSizes  pStrSizes;
    PSecPkgContext_Lifespan     pLifeSpan;
    PSecPkgContext_NamesA       pNames;
    PSecPkgContext_AuthorityA   pAuthority;
    PSecPkgContext_KeyInfoA     pKeyInfo;
    PPctCertificate             pCert;

    pContext = ValidateContextHandle(phContext);

    if ( !pContext )
    {
        return( SEC_E_INVALID_HANDLE );
    }


    if (0 == (pContext->Flags & CONTEXT_FLAG_CLIENT))
    {
        pCert = pContext->RipeZombie->pClientCert;
    }
    else
    {
        pCert = pContext->RipeZombie->pServerCert;
    }

    switch ( ulAttribute )
    {
        case SECPKG_ATTR_SIZES:
        {
            PSecPkgContext_Sizes        pSizes;
            if (NULL == pContext->pSystem)
            {
                return(SEC_E_INVALID_HANDLE);
            }
            pSizes = (PSecPkgContext_Sizes) pBuffer;
            switch(pContext->RipeZombie->fProtocol)
            {
                case SP_PROT_SSL2_CLIENT:
                case SP_PROT_SSL2_SERVER:
                    pSizes->cbMaxToken = SSL2_MAX_MESSAGE_LENGTH;
                    break;

                case SP_PROT_PCT1_CLIENT:
                case SP_PROT_PCT1_SERVER:
                    pSizes->cbMaxToken = PCT1_MAX_MESSAGE_LENGTH;
                    break;

                case SP_PROT_SSL3_CLIENT:
                case SP_PROT_SSL3_SERVER:
                    pSizes->cbMaxToken = SSL3_MAX_MESSAGE_LENGTH;
                    break;

                default:
                    pSizes->cbMaxToken = 32768;
            }

            pSizes->cbMaxSignature = pCert? pCert->cbSig : 0;
            pSizes->cbBlockSize = pContext->pSystem->BlockSize;
            pSizes->cbSecurityTrailer = pSizes->cbMaxSignature;

            return( SEC_E_OK );
        }
        case SECPKG_ATTR_STREAM_SIZES:
            if (NULL == pContext->pCheck)
            {
                return SEC_E_INVALID_HANDLE;
            }

            pStrSizes = (PSecPkgContext_StreamSizes)pBuffer;
            switch(pContext->RipeZombie->fProtocol)
            {
                case SP_PROT_SSL2_CLIENT:
                case SP_PROT_SSL2_SERVER:
                    pStrSizes->cbHeader = 2 + pContext->pCheck->cbCheckSum;
                    pStrSizes->cbTrailer = 0;
                    break;

                case SP_PROT_PCT1_CLIENT:
                case SP_PROT_PCT1_SERVER:
                    pStrSizes->cbHeader = 2;
                    pStrSizes->cbTrailer = pContext->pCheck->cbCheckSum;
                    break;

                case SP_PROT_SSL3_CLIENT:
                case SP_PROT_SSL3_SERVER:
                    pStrSizes->cbHeader = 5;
                    pStrSizes->cbTrailer = pContext->pCheck->cbCheckSum;
                    break;

                default:
                    pStrSizes->cbHeader = 0;
                    pStrSizes->cbTrailer = 0;
            }

            pStrSizes->cbMaximumMessage = 32768;
            if(pContext->pSystem)
            {
                pStrSizes->cbBlockSize = pContext->pSystem->BlockSize;
            }
            else
            {
                pStrSizes->cbBlockSize = 0;
            }
            pStrSizes->cBuffers = 4;

            return( SEC_E_OK );

        case SECPKG_ATTR_NAMES:
            pNames = (PSecPkgContext_NamesA) pBuffer;

#ifdef FOO
            /* First try to return the NT Auth Name */
            if(pContext->pszAuthName)
            {
                pNames->sUserName = SPExternalAlloc(strlen(pCert->pszSubject) + 1);
                if (NULL == pNames->sUserName)
                {
                    return(SEC_E_INSUFFICIENT_MEMORY);
                }
                strcpy(pNames->sUserName, pContext->pszAuthName);
                return(SEC_E_OK);
            }

            if (NULL == pCert || NULL == pCert->pszSubject)
            {
                return( SEC_E_UNSUPPORTED_FUNCTION );
            }
#endif
            pNames->sUserName = SPExternalAlloc(strlen(pCert->pszSubject) + 1);
            if (NULL == pNames->sUserName)
            {
                return(SEC_E_INSUFFICIENT_MEMORY);
            }
            strcpy(pNames->sUserName, pCert->pszSubject);
            return(SEC_E_OK);

        case SECPKG_ATTR_LIFESPAN:
            if (NULL == pCert)
            {
                return(SEC_E_UNSUPPORTED_FUNCTION);
            }
            pLifeSpan = (PSecPkgContext_Lifespan) pBuffer;

            pLifeSpan->tsStart.QuadPart = *((LONGLONG *) &pCert->ValidFrom);
            pLifeSpan->tsExpiry.QuadPart = *((LONGLONG*) &pCert->ValidUntil);
            return( SEC_E_OK );

        case SECPKG_ATTR_DCE_INFO:
            return( SEC_E_UNSUPPORTED_FUNCTION );

        case SECPKG_ATTR_KEY_INFO:
            if (NULL == pContext->pSystem ||
                NULL == pContext->pCheck ||
                NULL == pContext->pKeyExch)
            {
                return( SEC_E_UNSUPPORTED_FUNCTION );
            }
            if (NULL == pContext->pSystem->pszName ||
                NULL == pContext->pCheck->pszName ||
                NULL == pContext->pKeyExch->pszName)
            {
                return SEC_E_INTERNAL_ERROR;
            }

            pKeyInfo = (PSecPkgContext_KeyInfoA) pBuffer;
            pKeyInfo->KeySize = (pContext->RipeZombie->SessCiphSpec & SP_CIPHER_STRENGTH) >>
                                    SP_CSTR_POS;
            pKeyInfo->EncryptAlgorithm = pContext->RipeZombie->SessCiphSpec;
            pKeyInfo->SignatureAlgorithm = pContext->RipeZombie->SessHashSpec;

            pKeyInfo->sSignatureAlgorithmName = SPExternalAlloc(
                            strlen(pContext->pKeyExch->pszName) +
                            1 +
                            strlen(pContext->pCheck->pszName) +
                            1);

            if (NULL == pKeyInfo->sSignatureAlgorithmName)
            {
                return( SEC_E_INSUFFICIENT_MEMORY );
            }
            strcpy(
                pKeyInfo->sSignatureAlgorithmName,
                pContext->pKeyExch->pszName);
            strcat(pKeyInfo->sSignatureAlgorithmName, "/");
            strcat(
                pKeyInfo->sSignatureAlgorithmName,
                pContext->pCheck->pszName);

            pKeyInfo->sEncryptAlgorithmName = SPExternalAlloc(
                            strlen( pContext->pSystem->pszName ) + 1 );

            if ( pKeyInfo->sEncryptAlgorithmName )
            {
                strcpy( pKeyInfo->sEncryptAlgorithmName,
                        pContext->pSystem->pszName );
            }
            else
            {
                SPExternalFree( pKeyInfo->sSignatureAlgorithmName );
                return( SEC_E_INSUFFICIENT_MEMORY );
            }

            return( SEC_E_OK );

        case SECPKG_ATTR_AUTHORITY:
            if (NULL == pCert)
            {
                return( SEC_E_UNSUPPORTED_FUNCTION );
            }

            pAuthority = (PSecPkgContext_AuthorityA) pBuffer;

            if ( pCert->pszSubject && pCert->szSigningAuthority)
            {
                pAuthority->sAuthorityName = SPExternalAlloc(strlen(pCert->szSigningAuthority) + 1);
                if (pAuthority->sAuthorityName )
                {
                    strcpy(
            pAuthority->sAuthorityName,
            pCert->szSigningAuthority);
                    return( SEC_E_OK );
                }

                return( SEC_E_INSUFFICIENT_MEMORY );
            }

            return( SEC_E_UNSUPPORTED_FUNCTION );

        case SECPKG_ATTR_PROTO_INFO:
            {
                DWORD index;
                PSecPkgContext_ProtoInfoA pProtoInfo
                    = (PSecPkgContext_ProtoInfoA)pBuffer;
                for (index = 0;
                     index < sizeof(rgProts) / sizeof(PROTO_ID);
                     index += 1)
                {
                    if (pContext->RipeZombie->fProtocol == rgProts[index].dwProtoId)
                        break;
                }
                if (index == sizeof(rgProts) / sizeof(PROTO_ID))
                    return( SEC_E_UNSUPPORTED_FUNCTION );

                pProtoInfo->sProtocolName =
                    SPExternalAlloc(strlen(rgProts[index].szProto) + 1);
                if (NULL == pProtoInfo->sProtocolName)
                    return SEC_E_INSUFFICIENT_MEMORY;
                strcpy(pProtoInfo->sProtocolName, rgProts[index].szProto);
                pProtoInfo->majorVersion = rgProts[index].dwMajor;
                pProtoInfo->minorVersion = rgProts[index].dwMinor;
                switch(pContext->RipeZombie->fProtocol)
                {
                    case SP_PROT_PCT1_CLIENT:
                    case SP_PROT_PCT1_SERVER:
                        pProtoInfo->majorVersion = 1;
                        break;
                    case SP_PROT_SSL2_CLIENT:
                    case SP_PROT_SSL2_SERVER:
                        pProtoInfo->majorVersion = 2;
                        break;
                    case SP_PROT_SSL3_CLIENT:
                    case SP_PROT_SSL3_SERVER:
                        pProtoInfo->majorVersion = 3;
                        break;
                }
            }
            return SEC_E_OK;

        case SECPKG_ATTR_ISSUER_LIST:
            {
                PSecPkgContext_IssuerListInfo pIssuerList
                    = (PSecPkgContext_IssuerListInfo)pBuffer;

                if(pBuffer == NULL)
                {
                    return(SEC_E_INVALID_HANDLE);
                }

                pIssuerList->cbIssuerList = pContext->cbIssuerList;
                pIssuerList->pIssuerList = pContext->pbIssuerList;
//                pIssuerList->cbIssuerList = 0;
//                pIssuerList->pIssuerList = NULL;

                return SEC_E_OK;
            }

        case SECPKG_ATTR_REMOTE_CRED:
            {
                PSecPkgContext_RemoteCredenitalInfo pRemoteCred
                    = (PSecPkgContext_RemoteCredenitalInfo)pBuffer;
                PPctCertificate pRemoteCert, pCurCert;
//                SP_STATUS pctRet;

                if(pBuffer == NULL)
                {
                    return(SEC_E_INVALID_HANDLE);
                }


                if (0 == (pContext->Flags & CONTEXT_FLAG_CLIENT))
                {
                    pRemoteCert = pContext->RipeZombie->pClientCert;
                }
                else
                {
                    pRemoteCert = pContext->RipeZombie->pServerCert;
                }

                pRemoteCred->pbCertificateChain = NULL;
                pRemoteCred->cbCertificateChain  = 0;
                pRemoteCred->cCertificates = 0;
                pRemoteCred->fFlags = RCRED_STATUS_NOCRED;

                if(pRemoteCert == NULL)
                {
                    return (SEC_E_OK);
                }

                if(pRemoteCert->pRawCert == NULL ||
                    pRemoteCert->cbRawCert == 0)
                {
                    return(SEC_E_OK);
                }
                pRemoteCred->fFlags |= RCRED_CRED_EXISTS;

                pRemoteCred->pbCertificateChain = pRemoteCert->pRawCert;
                pRemoteCred->cbCertificateChain = pRemoteCert->cbRawCert;
                pRemoteCred->cCertificates = 1;

                pCurCert = pRemoteCert;

                // We perform basically the same loop we do in SPSerializeCertificate
                // to get to the root cert, and then we check to see if it is known.
                while(pCurCert != NULL)
                {
                    if(pCurCert->pIssuerCertificate == pCurCert)
                    {
                        break;
                    }
                    pCurCert = pCurCert->pIssuerCertificate;

                }

                if((pCurCert == NULL) || (pCurCert->fTrusted == FALSE))
                {
                    pRemoteCred->fFlags |= RCRED_STATUS_UNKNOWN_ISSUER;
                }
                pRemoteCred->dwBits = 0;
                switch(pRemoteCert->Spec)
                {
                    case SP_CERT_X509:
                    {
                        BSAFE_PRV_KEY *pk;
                        PctPublicKey  *pKey;
                        pKey = pRemoteCert->pPublicKey;
                        pk = (BSAFE_PRV_KEY *)pKey->pKey;
                        pRemoteCred->dwBits = pk->bitlen;

                    }
                    break;

                }

                return SEC_E_OK;
            }

        case SECPKG_ATTR_LOCAL_CRED:
            {
                PSecPkgContext_LocalCredenitalInfo pLocalCred
                    = (PSecPkgContext_LocalCredenitalInfo)pBuffer;
                PPctCertificate pLocalCert, pCurCert;
//                SP_STATUS pctRet;


                if(pBuffer == NULL)
                {
                    return(SEC_E_INVALID_HANDLE);
                }


                if (0 == (pContext->Flags & CONTEXT_FLAG_CLIENT))
                {
                    pLocalCert = pContext->RipeZombie->pServerCert;
                }
                else
                {
                    pLocalCert = pContext->RipeZombie->pClientCert;
                }

                pLocalCred->pbCertificateChain = NULL;
                pLocalCred->cbCertificateChain  = 0;
                pLocalCred->cCertificates = 0;
                pLocalCred->fFlags = LCRED_STATUS_NOCRED;

                if(pLocalCert == NULL)
                {
                    return (SEC_E_OK);
                }

                if(pLocalCert->pRawCert == NULL ||
                    pLocalCert->cbRawCert == 0)
                {
                    return(SEC_E_OK);
                }
                pLocalCred->fFlags |= LCRED_CRED_EXISTS;


                pLocalCred->pbCertificateChain = pLocalCert->pRawCert;
                pLocalCred->cbCertificateChain = pLocalCert->cbRawCert;
                pLocalCred->cCertificates = 1;
                pCurCert = pLocalCert;
                // We perform basically the same loop we do in SPSerializeCertificate
                // to get to the root cert, and then we check to see if it is known.

                while(pCurCert != NULL)
                {
                    if(pCurCert->pIssuerCertificate == pCurCert)
                    {
                        break;
                    }
                    pCurCert = pCurCert->pIssuerCertificate;

                }

                if((pCurCert == NULL) || (pCurCert->fTrusted == FALSE))
                {
                    pLocalCred->fFlags |= LCRED_STATUS_UNKNOWN_ISSUER;
                }


                pLocalCred->dwBits = 0;
                switch(pLocalCert->Spec)
                {
                    case SP_CERT_X509:
                    {
                        BSAFE_PRV_KEY *pk;
                        PctPublicKey  *pKey;
                        pKey = pLocalCert->pPublicKey;
                        pk = (BSAFE_PRV_KEY *)pKey->pKey;
                        pLocalCred->dwBits = pk->bitlen;

                    }
                    break;

                }

                return SEC_E_OK;
            }

        case SECPKG_ATTR_CONNECTION_INFO:
        {
            PSecPkgContext_ConnectionInfo pResult = pBuffer;

            if (pBuffer == NULL)
            {
                return (SEC_E_INVALID_HANDLE);
            }

            pResult->dwProtocol = pContext->RipeZombie->fProtocol;

            // This is in the correct ALG_ID CALG_XXX format.
            pResult->aiCipher = pContext->pSystem->Type;
            // Need to take this from the CipherInfo struct...
            pResult->dwCipherStrength =
                (pContext->RipeZombie->SessCiphSpec & SP_CIPHER_STRENGTH)
                >> SP_CSTR_POS;

            // This is in the correct ALG_ID CALG_XXX format.
            pResult->aiHash = pContext->pCheck->Type;
            pResult->dwHashStrength = pContext->pCheck->cbCheckSum * 8;

            // How we store it is in the wrong format, but we always know that
            // we use the RSA PKCS1 key exch.
            ASSERT(pContext->pKeyExch->Type == SP_EXCH_RSA_PKCS1);
            pResult->aiExch = CALG_RSA_KEYX;

            // BUGBUG: How do I find the real key exch strength???????
            pResult->dwExchStrength = 512;

            return (SEC_E_OK);
        }
            break;

        default:
            return( SEC_E_INVALID_TOKEN );


    }

    return( SEC_E_INVALID_TOKEN );
}

SECURITY_STATUS SEC_ENTRY
QueryContextAttributesW(
    PCtxtHandle         phContext,      // Context to query
    unsigned long           ulAttribute,    // Attribute to query
    void SEC_FAR *          pBuffer     // Buffer for attributes
)
{
    PSPContext                  pContext;
    PSecPkgContext_NamesW       pNames;
    PSecPkgContext_AuthorityW   pAuthority;
    PSecPkgContext_KeyInfoW     pKeyInfo;
    PPctCertificate             pCert;

    pContext = ValidateContextHandle(phContext);

    if ( !pContext )
    {
        return( SEC_E_INVALID_HANDLE );
    }


    if (0 == (pContext->Flags & CONTEXT_FLAG_CLIENT))
    {
        pCert = pContext->RipeZombie->pClientCert;
    }
    else
    {
        pCert = pContext->RipeZombie->pServerCert;
    }

    switch ( ulAttribute )
    {
        // Non-unicode specific types.
        case SECPKG_ATTR_SIZES:
        case SECPKG_ATTR_STREAM_SIZES:
        case SECPKG_ATTR_LIFESPAN:
        case SECPKG_ATTR_ISSUER_LIST:
        case SECPKG_ATTR_REMOTE_CRED:
        case SECPKG_ATTR_LOCAL_CRED:
        case SECPKG_ATTR_CONNECTION_INFO:
        case SECPKG_ATTR_DCE_INFO:

            return( QueryContextAttributesA( phContext, ulAttribute, pBuffer ) );

        case SECPKG_ATTR_NAMES:
            pNames = (PSecPkgContext_NamesW) pBuffer;

#ifdef FOO
            /* First try to return the NT Auth Name */
            if(pContext->pszAuthName)
            {
                pNames->sUserName = SPExternalAlloc(strlen(pCert->pszSubject) + 1);
                if (NULL == pNames->sUserName)
                {
                    return(SEC_E_INSUFFICIENT_MEMORY);
                }
                strcpy(pNames->sUserName, pContext->pszAuthName);
                return(SEC_E_OK);
            }

            if (NULL == pCert || NULL == pCert->pszSubject)
            {
                return( SEC_E_UNSUPPORTED_FUNCTION );
            }
#endif
            pNames->sUserName = SPExternalAlloc((strlen(pCert->pszSubject) + 1) * sizeof(WCHAR) );
            if (NULL == pNames->sUserName)
            {
                return(SEC_E_INSUFFICIENT_MEMORY);
            }
            mbstowcs(pNames->sUserName, pCert->pszSubject, 
                strlen(pCert->pszSubject) + 1);
            return(SEC_E_OK);


        case SECPKG_ATTR_KEY_INFO:
            if (NULL == pContext->pSystem ||
                NULL == pContext->pCheck ||
                NULL == pContext->pKeyExch)
            {
                return( SEC_E_UNSUPPORTED_FUNCTION );
            }
            if (NULL == pContext->pSystem->pszName ||
                NULL == pContext->pCheck->pszName ||
                NULL == pContext->pKeyExch->pszName)
            {
                return SEC_E_INTERNAL_ERROR;
            }

            pKeyInfo = (PSecPkgContext_KeyInfoW) pBuffer;
            pKeyInfo->KeySize = (pContext->RipeZombie->SessCiphSpec & SP_CIPHER_STRENGTH) >>
                                    SP_CSTR_POS;
            pKeyInfo->EncryptAlgorithm = pContext->RipeZombie->SessCiphSpec;
            pKeyInfo->SignatureAlgorithm = pContext->RipeZombie->SessHashSpec;

            pKeyInfo->sSignatureAlgorithmName = SPExternalAlloc(
                (strlen(pContext->pKeyExch->pszName) + 1 +
                 strlen(pContext->pCheck->pszName) +1) * sizeof(WCHAR));

            if (NULL == pKeyInfo->sSignatureAlgorithmName)
            {
                return( SEC_E_INSUFFICIENT_MEMORY );
            }
            mbstowcs(
                pKeyInfo->sSignatureAlgorithmName,
                pContext->pKeyExch->pszName,
                strlen(pContext->pKeyExch->pszName) + 1);
            // strcat(pKeyInfo->sSignatureAlgorithmName, "/");
            pKeyInfo->sSignatureAlgorithmName[strlen(pContext->pKeyExch->pszName)] = L'/';
            mbstowcs(
                &pKeyInfo->sSignatureAlgorithmName[strlen(pContext->pKeyExch->pszName) + 1],
                pContext->pCheck->pszName,
                strlen(pContext->pCheck->pszName) + 1);

            pKeyInfo->sEncryptAlgorithmName = SPExternalAlloc(
                            (strlen( pContext->pSystem->pszName ) + 1) * sizeof(WCHAR) );

            if ( pKeyInfo->sEncryptAlgorithmName )
            {
                mbstowcs( pKeyInfo->sEncryptAlgorithmName,
                        pContext->pSystem->pszName,
                        strlen(pContext->pSystem->pszName) + 1 );
            }
            else
            {
                SPExternalFree( pKeyInfo->sSignatureAlgorithmName );
                return( SEC_E_INSUFFICIENT_MEMORY );
            }

            return( SEC_E_OK );

        case SECPKG_ATTR_AUTHORITY:
            if (NULL == pCert)
            {
                return( SEC_E_UNSUPPORTED_FUNCTION );
            }

            pAuthority = (PSecPkgContext_Authority) pBuffer;

            if ( pCert->pszSubject && pCert->szSigningAuthority)
            {
                pAuthority->sAuthorityName = SPExternalAlloc(
                    (strlen(pCert->szSigningAuthority) + 1) * sizeof(WCHAR));
                if (pAuthority->sAuthorityName )
                {
                    mbstowcs(
                        pAuthority->sAuthorityName,
                        pCert->szSigningAuthority,
                        strlen(pCert->szSigningAuthority) + 1);
                    return( SEC_E_OK );
                }

                return( SEC_E_INSUFFICIENT_MEMORY );
            }

            return( SEC_E_UNSUPPORTED_FUNCTION );

        case SECPKG_ATTR_PROTO_INFO:
        {
            DWORD index;
            PSecPkgContext_ProtoInfoW pProtoInfo
                = (PSecPkgContext_ProtoInfoW)pBuffer;
            for (index = 0;
                 index < sizeof(rgProts) / sizeof(PROTO_ID);
                 index += 1)
            {
                if (pContext->RipeZombie->fProtocol == rgProts[index].dwProtoId)
                    break;
            }
            if (index == sizeof(rgProts) / sizeof(PROTO_ID))
                return( SEC_E_UNSUPPORTED_FUNCTION );

            pProtoInfo->sProtocolName = SPExternalAlloc(
                (strlen(rgProts[index].szProto) + 1) * sizeof(WCHAR));
            if (NULL == pProtoInfo->sProtocolName)
                return SEC_E_INSUFFICIENT_MEMORY;
            mbstowcs(pProtoInfo->sProtocolName, 
                rgProts[index].szProto,
                strlen(rgProts[index].szProto) + 1);
            pProtoInfo->majorVersion = rgProts[index].dwMajor;
            pProtoInfo->minorVersion = rgProts[index].dwMinor;
            switch(pContext->RipeZombie->fProtocol)
            {
                case SP_PROT_PCT1_CLIENT:
                case SP_PROT_PCT1_SERVER:
                    pProtoInfo->majorVersion = 1;
                    break;
                case SP_PROT_SSL2_CLIENT:
                case SP_PROT_SSL2_SERVER:
                    pProtoInfo->majorVersion = 2;
                    break;
                case SP_PROT_SSL3_CLIENT:
                case SP_PROT_SSL3_SERVER:
                    pProtoInfo->majorVersion = 3;
                    break;
            }
        }
            return SEC_E_OK;

        default:
            return( SEC_E_INVALID_TOKEN );


    }

    return( SEC_E_INVALID_TOKEN );
}



SECURITY_STATUS SEC_ENTRY
ImpersonateSecurityContext(
    PCtxtHandle         phContext       // Context to impersonate
)
{
#ifndef XBOX
    PSPContext  pContext;
    HANDLE hToken;

    pContext = ValidateContextHandle( phContext );
    if ( !pContext )
    {
         return( SEC_E_INVALID_HANDLE );
    }

    if((NULL == pContext->RipeZombie->phMapper) ||
       (NULL == pContext->RipeZombie->phMapper->m_vtable))
    {
        return( SEC_E_NO_IMPERSONATION );
    }

    if(!pContext->RipeZombie->phMapper->m_vtable->GetAccessToken(pContext->RipeZombie->phMapper,
                                                       pContext->RipeZombie->hLocator,
                                                       &hToken))
    {
        return (SEC_E_NO_IMPERSONATION);
    }
    if(hToken == NULL)
    {
        return( SEC_E_NO_IMPERSONATION );
    }

    if(!ImpersonateLoggedOnUser(hToken))
    {
        return(GetLastError());
    }

    return(SEC_E_OK);
#else // !UNDER_CE
    return (SEC_E_NO_IMPERSONATION);
#endif // UNDER_CE
}




SECURITY_STATUS SEC_ENTRY
RevertSecurityContext(
    PCtxtHandle         phContext       // Context from which to re
)
{
#ifndef XBOX
    PSPContext  pContext;

    pContext = ValidateContextHandle( phContext );

    if ( pContext )
    {
        if((NULL == pContext->RipeZombie->phMapper) ||
           (NULL == pContext->RipeZombie->phMapper->m_vtable))
        {
            return( SEC_E_NO_IMPERSONATION );
        }
        if(!RevertToSelf())
        {
            return(GetLastError());
        }
        return(SEC_E_OK);
    }

    return( SEC_E_INVALID_HANDLE );
#else // !UNDER_CE
    return (SEC_E_NO_IMPERSONATION);
#endif // UNDER_CE
}

SECURITY_STATUS SEC_ENTRY
ApplyControlToken(
    PCtxtHandle                 phContext,          // Context to modify
    PSecBufferDesc              pInput              // Input token to apply
    )
{
    PSPContext  pContext;

    pContext = ValidateContextHandle(phContext);

    if (!pContext)
    {
        return( SEC_E_INVALID_HANDLE );
    }

    return(SEC_E_UNSUPPORTED_FUNCTION);
}

#if 0

SECURITY_STATUS SEC_ENTRY
QuerySecurityContextToken(
    PCtxtHandle phContext,
    void SEC_FAR * Token
    )
{
    PSPContext  pContext;

    pContext = ValidateContextHandle( phContext );

    if (!pContext)
    {
        return( SEC_E_INVALID_HANDLE );
    }

    if((NULL == pContext->RipeZombie->phMapper) ||
       (NULL == pContext->RipeZombie->phMapper->m_vtable))
    {
        *((HANDLE *)Token) = NULL;
        return SEC_E_OK;
    }

    pContext->RipeZombie->phMapper->m_vtable->GetAccessToken(pContext->RipeZombie->phMapper,
                                                       pContext->RipeZombie->hLocator,
                                                       Token);

    return SEC_E_OK;
}

#else // !UNDER_CE

SECURITY_STATUS SEC_ENTRY
QuerySecurityContextToken(
    PCtxtHandle phContext,
    void SEC_FAR * SEC_FAR * Token
    )
{
    PSPContext  pContext;

    pContext = ValidateContextHandle( phContext );

    if (!pContext)
    {
        return( SEC_E_INVALID_HANDLE );
    }

    if((NULL == pContext->RipeZombie->phMapper) ||
       (NULL == pContext->RipeZombie->phMapper->m_vtable))
    {
        **((HANDLE **)Token) = NULL;
        return SEC_E_OK;
    }

    pContext->RipeZombie->phMapper->m_vtable->GetAccessToken(pContext->RipeZombie->phMapper,
                                                       pContext->RipeZombie->hLocator,
                                                       Token);

    return SEC_E_OK;
}

#endif // !UNDER_CE


/************************************************************************
* PSPContext  ValidateContextHandle(PCtxtHandle phContext)
*
* Derive a valid context pointer from a context handle
*
\***********************************************************************/
PSPContext
ValidateContextHandle(PCtxtHandle phContext)
{
    BOOL        fReturn;
    PSPContext  pContext;


    fReturn = FALSE;
    if (phContext)
    {

#ifdef USE_EXCEPTIONS
        try
#endif
        {
            pContext = (PSPContext )phContext->dwUpper;
            if(pContext == NULL)
            {
                return (NULL);
            }


            if (pContext->Magic == SP_CONTEXT_MAGIC)
            {
                fReturn = TRUE;
            }
        }
#ifdef USE_EXCEPTIONS
#ifdef SECURITY_LINUX
        catch(...)
#else
        except(EXCEPTION_EXECUTE_HANDLER);
#endif
        {
            pContext = NULL;
        }
#endif

    }

    if (fReturn)
    {
        return (pContext);
    }

    return (NULL);
}
