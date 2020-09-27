//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       message.c
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    9-22-95   RichardW   Created
//              10-8-95   TerenceS   Created PctSealMessage
//              10-8-95   TerenceS   Created PctUnSealMessage
//
//----------------------------------------------------------------------------

#include <spbase.h>
#include <security.h>
#include "spsspi.h"

SECURITY_STATUS
SEC_ENTRY
MakeSignature(PCtxtHandle         phContext,
              DWORD               fQOP,
              PSecBufferDesc      pMessage,
              ULONG               MessageSeqNo)
{
    return(SEC_E_UNSUPPORTED_FUNCTION);
}

SECURITY_STATUS SEC_ENTRY
VerifySignature(PCtxtHandle     phContext,
                PSecBufferDesc  pMessage,
                ULONG           MessageSeqNo,
                DWORD *         pfQOP)
{
    return(SEC_E_UNSUPPORTED_FUNCTION);
}

SECURITY_STATUS
SEC_ENTRY
SealMessage( PCtxtHandle         phContext,
             DWORD               fQOP,
             PSecBufferDesc      pMessage,
             ULONG               MessageSeqNo)
{

    PSecBuffer                  pHdrBuffer;
    PSecBuffer                  pDataBuffer;
    PSecBuffer                  pTlrBuffer;
    PSecBuffer                  pTokenBuffer;

    PSPContext                  pContext;
    SP_STATUS                  pctRet = PCT_ERR_OK;
    SPBuffer                   CommOut;
    SPBuffer                   AppIn;
    BOOL                       fAlloced=FALSE;
    BOOL                       fConnectionMode = FALSE;
    int i;
    SecPkgContext_StreamSizes  Sizes;

    pContext = ValidateContextHandle(phContext);

    if (!pContext)
    {
        return( SEC_E_INVALID_HANDLE );
    }
    if ((0 == (pContext->State & SP_STATE_CONNECTED)) || (NULL ==  pContext->Encrypt))
    {
        return( SEC_E_CONTEXT_EXPIRED );
    }


    QueryContextAttributesA(phContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);

    //
    // Find the buffer with the data:
    //

    pHdrBuffer = NULL;
    pDataBuffer = NULL;
    pTlrBuffer = NULL;
    pTokenBuffer = NULL;

    /* Gibraltar passes in the following, in stream mode,
     * a TOKEN buffer (or SECBUFFER_STREAM_HEADER)
     * a DATA buffer
     * a TOKEN buffer (or SECBUFFER_STREAM_TRAILER)
     * or we can get a connection mode as in
     * DATA buffer
     * Token buffer
     */




    if(0 == (pContext->Flags & CONTEXT_FLAG_CONNECTION_MODE))
    {
        // Stream Mode
        // The output buffer should be a concatenation of
        // the header buffer, Data buffer, and Trailer buffers.
        for (i = 0 ; i < (int)pMessage->cBuffers ; i++ )
        {
            switch(pMessage->pBuffers[i].BufferType)
            {
                case SECBUFFER_STREAM_HEADER:
                     pHdrBuffer = &pMessage->pBuffers[i];
                     break;

                case SECBUFFER_DATA :
                     pDataBuffer = &pMessage->pBuffers[i];
                     break;

                case SECBUFFER_STREAM_TRAILER:
                     pTlrBuffer = &pMessage->pBuffers[i];
                     break;

                case SECBUFFER_TOKEN:
                     if(pHdrBuffer == NULL)
                     {
                         pHdrBuffer = &pMessage->pBuffers[i];
                     }
                     else if(pTlrBuffer == NULL)
                     {
                          pTlrBuffer = &pMessage->pBuffers[i];
                     }
                     break;
                default:
                    break;
            }
        }
        if((pHdrBuffer == NULL) || (pDataBuffer == NULL) )
        {
            return( SEC_E_INVALID_TOKEN );
        }
        if((PUCHAR)pDataBuffer->pvBuffer ==
           ((PUCHAR)pHdrBuffer->pvBuffer + pHdrBuffer->cbBuffer))
        {
            pTokenBuffer = pHdrBuffer;
            CommOut.pvBuffer = pHdrBuffer->pvBuffer;
            CommOut.cbData   = 0;
            CommOut.cbBuffer = pHdrBuffer->cbBuffer + pDataBuffer->cbBuffer;
            if(pTlrBuffer)
            {
                CommOut.cbBuffer += pTlrBuffer->cbBuffer;
            }
        }
        else
        {
            // We have to realloc the buffer
            fAlloced = TRUE;
            CommOut.cbBuffer = pDataBuffer->cbBuffer + pTokenBuffer->cbBuffer;
            if(pTlrBuffer)
            {
                CommOut.cbBuffer += pTlrBuffer->cbBuffer;
            }
            CommOut.pvBuffer = SPExternalAlloc(CommOut.cbBuffer);
            if(CommOut.pvBuffer == NULL)
            {
                return (SEC_E_INSUFFICIENT_MEMORY);
            }
            CommOut.cbData = 0;
        }
        // The data buffer always goes to AppIn
        AppIn.pvBuffer = pDataBuffer->pvBuffer;
        AppIn.cbData   = pDataBuffer->cbBuffer;
        AppIn.cbBuffer = pDataBuffer->cbBuffer;
        pctRet = pContext->Encrypt(pContext,
                                   &AppIn,
                                   &CommOut);
        if(fAlloced)
        {
            if(pctRet == PCT_ERR_OK)
            {

                CopyMemory(pHdrBuffer->pvBuffer,
                       CommOut.pvBuffer,
                       pHdrBuffer->cbBuffer);

                pDataBuffer->cbBuffer = min(pDataBuffer->cbBuffer, CommOut.cbBuffer - pHdrBuffer->cbBuffer);

                CopyMemory(pDataBuffer->pvBuffer,
                       (PUCHAR)CommOut.pvBuffer + pHdrBuffer->cbBuffer,
                       pDataBuffer->cbBuffer);

                if(pTlrBuffer)
                {
                    pTlrBuffer->cbBuffer = min(pTlrBuffer->cbBuffer,
                                               CommOut.cbBuffer - (pHdrBuffer->cbBuffer + pDataBuffer->cbBuffer));

                    CopyMemory(pTlrBuffer->pvBuffer,
                               (PUCHAR)CommOut.pvBuffer + pHdrBuffer->cbBuffer + pDataBuffer->cbBuffer,
                               pTlrBuffer->cbBuffer);
                }
            }
            SPExternalFree(CommOut.pvBuffer);
        }

    }
    else
    {
        // We're doing connection mode, so unpack buffers as a
        // Data and then Token buffer
        fConnectionMode = TRUE;
        for (i = 0 ; i < (int)pMessage->cBuffers ; i++ )
        {
            switch(pMessage->pBuffers[i].BufferType)
            {
                case SECBUFFER_DATA :
                     pDataBuffer = &pMessage->pBuffers[i];
                     break;


                case SECBUFFER_TOKEN:
                     if(pTokenBuffer == NULL)
                     {
                         pTokenBuffer = &pMessage->pBuffers[i];
                     }
                     break;
                default:
                    break;
            }
        }
        if((pTokenBuffer == NULL) || (pDataBuffer == NULL))
        {
            return( SEC_E_INVALID_TOKEN );
        }


        if((pDataBuffer->pvBuffer == NULL) || (pTokenBuffer->pvBuffer == NULL))
        {
            return( SEC_E_INVALID_TOKEN );
        }
        // Connection Mode
        // The output should get written to a concatenation of the
        // data buffer and the token buffer.  If no token buffer is
        // given, then we should allocate one.

        if((PUCHAR)pTokenBuffer->pvBuffer ==
           ((PUCHAR)pDataBuffer->pvBuffer + pDataBuffer->cbBuffer))
        {
            // If the buffers are contiguous, we can optimize!
            CommOut.pvBuffer = pDataBuffer->pvBuffer;
            CommOut.cbData   = 0;
            CommOut.cbBuffer = pDataBuffer->cbBuffer + pTokenBuffer->cbBuffer;
        }
        else
        {
            // We have to realloc the buffer
            fAlloced = TRUE;
            CommOut.pvBuffer = SPExternalAlloc(pDataBuffer->cbBuffer + pTokenBuffer->cbBuffer);
            if(CommOut.pvBuffer == NULL)
            {
                return (SEC_E_INSUFFICIENT_MEMORY);
            }
            CommOut.cbBuffer = pDataBuffer->cbBuffer + pTokenBuffer->cbBuffer;
            CommOut.cbData = 0;
        }
        // The data buffer always goes to AppIn
        AppIn.pvBuffer = pDataBuffer->pvBuffer;
        AppIn.cbData   = pDataBuffer->cbBuffer;
        AppIn.cbBuffer = pDataBuffer->cbBuffer;
        pctRet = pContext->Encrypt(pContext,
                                   &AppIn,
                                   &CommOut);

        if(fAlloced)
        {
            if(pctRet == PCT_ERR_OK)
            {
                // We encrypted into our temporary buffer, so we must
                // copy.
                CopyMemory(pDataBuffer->pvBuffer,
                           CommOut.pvBuffer,
                           pDataBuffer->cbBuffer);
                pTokenBuffer->cbBuffer = min(pTokenBuffer->cbBuffer, CommOut.cbData - pDataBuffer->cbBuffer);

                CopyMemory(pTokenBuffer->pvBuffer,
                           (PUCHAR)CommOut.pvBuffer + pDataBuffer->cbBuffer,
                           pTokenBuffer->cbBuffer);
            }
            SPExternalFree(CommOut.pvBuffer);
        }
    }
    return PctTranslateError(pctRet);
}


SECURITY_STATUS
SEC_ENTRY
UnsealMessage(  PCtxtHandle         phContext,
                PSecBufferDesc      pMessage,
                ULONG               MessageSeqNo,
                DWORD *             pfQOP)
{
    // Output Buffer Types
    PSecBuffer                  pHdrBuffer;
    PSecBuffer                  pDataBuffer;
    PSecBuffer                  pTokenBuffer;
    PSecBuffer                  pTlrBuffer;
    PSecBuffer                  pExtraBuffer;

    PSPContext                  pContext;
    SP_STATUS                  pctRet = PCT_ERR_OK;
    SPBuffer                   CommIn;
    SPBuffer                   AppOut;
//    DWORD                      cbBytes;
    SecPkgContext_StreamSizes  Sizes;
    BOOL                       fAlloced = FALSE;
    int i;

    pContext = ValidateContextHandle(phContext);

    if (!pContext)
    {
        return( SEC_E_INVALID_HANDLE );
    }
    QueryContextAttributesA(phContext, SECPKG_ATTR_STREAM_SIZES, &Sizes);

    if ((0 == (pContext->State & SP_STATE_CONNECTED)) || (NULL ==  pContext->Decrypt))
    {
        return( SEC_E_CONTEXT_EXPIRED );
    }

    //
    // Set up output buffers:
    //
    pHdrBuffer = NULL;
    pDataBuffer = NULL;
    pTokenBuffer = NULL;
    pTlrBuffer = NULL;
    pExtraBuffer = NULL;

    // On input, the buffers can either be
    // DataBuffer
    // TokenBuffer
    //
    // or
    //
    // Data Buffer
    // Empty
    // Empty
    // Empty
    //

    // on Output, the buffers are
    // DataBuffer
    // TokenBuffer
    //
    // or
    // HdrBuffer
    // DataBuffer
    // Tlrbuffer
    // Extrabuffer or Empty
    if(0 == (pContext->Flags & CONTEXT_FLAG_CONNECTION_MODE))
    {
        // Stream Mode
        // The output buffer should be a concatenation of
        // the header buffer, Data buffer, and Trailer buffers.

        for (i = 0 ; i < (int)pMessage->cBuffers ; i++ )
        {
            switch(pMessage->pBuffers[i].BufferType)
            {

                case SECBUFFER_DATA :
                     pHdrBuffer = &pMessage->pBuffers[i];
                     break;


                case SECBUFFER_EMPTY:
                    if(pHdrBuffer == NULL)
                    {
                        break;
                    }
                    if(pDataBuffer == NULL)
                    {
                        pDataBuffer = &pMessage->pBuffers[i];

                    } else if(pTlrBuffer == NULL)
                    {
                        pTlrBuffer = &pMessage->pBuffers[i];
                    } else if (pExtraBuffer == NULL)
                    {
                         pExtraBuffer = &pMessage->pBuffers[i];
                    }
                    break;
                default:
                    break;
            }
        }
        if((pHdrBuffer == NULL) ||
           (pDataBuffer == NULL) ||
           (pTlrBuffer == NULL) ||
           (pExtraBuffer == NULL))
        {
            return( SEC_E_INVALID_TOKEN );
        }
        if(pHdrBuffer->pvBuffer == NULL)
        {
            return( SEC_E_INVALID_TOKEN );
        }

        CommIn.pvBuffer = pHdrBuffer->pvBuffer;
        CommIn.cbData   = pHdrBuffer->cbBuffer;
        CommIn.cbBuffer = CommIn.cbData;

        AppOut.pvBuffer = (PUCHAR)CommIn.pvBuffer + Sizes.cbHeader;
        AppOut.cbData   = 0;
        AppOut.cbBuffer = CommIn.cbData-Sizes.cbHeader - Sizes.cbTrailer;

        pctRet = pContext->DecryptHandler(pContext,
                                   &CommIn,
                                   &AppOut);

        if((pctRet == PCT_ERR_OK) ||
            (pctRet == PCT_INT_RENEGOTIATE))
        {
            if(CommIn.cbData < pHdrBuffer->cbBuffer)
            {
                pExtraBuffer->BufferType = SECBUFFER_EXTRA;
                pExtraBuffer->cbBuffer = pHdrBuffer->cbBuffer-CommIn.cbData;
                pExtraBuffer->pvBuffer = (PUCHAR)pHdrBuffer->pvBuffer+CommIn.cbData;
            }
            pHdrBuffer->BufferType = SECBUFFER_STREAM_HEADER;
            pHdrBuffer->cbBuffer = Sizes.cbHeader;

            pDataBuffer->BufferType = SECBUFFER_DATA;
            pDataBuffer->pvBuffer = AppOut.pvBuffer;
            pDataBuffer->cbBuffer = AppOut.cbData;

            pTlrBuffer->BufferType = SECBUFFER_STREAM_TRAILER;
            pTlrBuffer->pvBuffer = (PUCHAR)pDataBuffer->pvBuffer + AppOut.cbData;
            pTlrBuffer->cbBuffer = Sizes.cbTrailer;

        }
        else if(pctRet == PCT_INT_INCOMPLETE_MSG)
        {
            pDataBuffer->BufferType = SECBUFFER_MISSING;
            pDataBuffer->cbBuffer = CommIn.cbData - pHdrBuffer->cbBuffer;
        }

    }
    else
    {
        // Connection Mode
        for (i = 0 ; i < (int)pMessage->cBuffers ; i++ )
        {
            switch(pMessage->pBuffers[i].BufferType)
            {
                case SECBUFFER_DATA :
                     pDataBuffer = &pMessage->pBuffers[i];
                     break;


                case SECBUFFER_TOKEN:
                     if(pTokenBuffer == NULL)
                     {
                         pTokenBuffer = &pMessage->pBuffers[i];
                     }
                     break;
                default:
                    break;
            }
        }
        if((pTokenBuffer == NULL) || (pDataBuffer == NULL))
        {
            return( SEC_E_INVALID_TOKEN );
        }


        if((pDataBuffer->pvBuffer == NULL) || (pTokenBuffer->pvBuffer == NULL))
        {
            return( SEC_E_INVALID_TOKEN );
        }
                // The Data and Token buffers are concatenated together to
        // form a single input buffer.
        if((PUCHAR)pDataBuffer->pvBuffer + pDataBuffer->cbBuffer ==
           (PUCHAR)pTokenBuffer->pvBuffer)
        {
            // Speed Opt,  If the buffers really are just one big buffer
            // then we can party on them directly.
            CommIn.pvBuffer = pDataBuffer->pvBuffer;
            CommIn.cbData   = pDataBuffer->cbBuffer + pTokenBuffer->cbBuffer;
            CommIn.cbBuffer = CommIn.cbData;
        }
        else
        {
            // We have to allocate a uniform input buffer
            CommIn.cbData   = pDataBuffer->cbBuffer + pTokenBuffer->cbBuffer;
            CommIn.pvBuffer = SPExternalAlloc(CommIn.cbData);
            CommIn.cbBuffer = CommIn.cbData;
            CopyMemory(CommIn.pvBuffer,  pDataBuffer->pvBuffer,  pDataBuffer->cbBuffer);

            CopyMemory((PUCHAR)CommIn.pvBuffer + pDataBuffer->cbBuffer,
                       pTokenBuffer->pvBuffer,
                       pTokenBuffer->cbBuffer);
            fAlloced = TRUE;

        }
        AppOut.pvBuffer = pDataBuffer->pvBuffer;
        AppOut.cbData   = 0;
        AppOut.cbBuffer = pDataBuffer->cbBuffer;
        pctRet = pContext->DecryptHandler(pContext,
                           &CommIn,
                           &AppOut);


        if((pctRet == PCT_ERR_OK) ||
           (pctRet == PCT_INT_RENEGOTIATE))
        {
            // We don't really care abou the contents of the
            // Token buffer, so just copy data

             pDataBuffer->cbBuffer = AppOut.cbData;

        }

        if(fAlloced)
        {
            SPExternalFree(CommIn.pvBuffer);
        }

    }


    return PctTranslateError(pctRet);

}
