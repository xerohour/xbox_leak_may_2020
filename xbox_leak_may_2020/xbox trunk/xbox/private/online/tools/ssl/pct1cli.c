/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
* This file is part of the Microsoft Private Communication Technology
* reference implementation, version 1.0
*
* The Private Communication Technology reference implementation, version 1.0
* ("PCTRef"), is being provided by Microsoft to encourage the development and
* enhancement of an open standard for secure general-purpose business and
* personal communications on open networks.  Microsoft is distributing PCTRef
* at no charge irrespective of whether you use PCTRef for non-commercial or
* commercial use.
*
* Microsoft expressly disclaims any warranty for PCTRef and all derivatives of
* it.  PCTRef and any related documentation is provided "as is" without
* warranty of any kind, either express or implied, including, without
* limitation, the implied warranties or merchantability, fitness for a
* particular purpose, or noninfringement.  Microsoft shall have no obligation
* to provide maintenance, support, upgrades or new releases to you or to anyone
* receiving from you PCTRef or your modifications.  The entire risk arising out
* of use or performance of PCTRef remains with you.
*
* Please see the file LICENSE.txt,
* or http://pct.microsoft.com/pct/pctlicen.txt
* for more information on licensing.
*
* Please see http://pct.microsoft.com/pct/pct.htm for The Private
* Communication Technology Specification version 1.0 ("PCT Specification")
*
* 1/23/96
*----------------------------------------------------------------------------*/


#include <spbase.h>
#include <pct1msg.h>
#include <pct1prot.h>




SP_STATUS WINAPI
Pct1ClientProtocolHandler(PSPContext pContext,
    PSPBuffer  pCommInput,
    PSPBuffer  pCommOutput,
    PSPBuffer  pAppOutput)
{
    SP_STATUS      pctRet= PCT_ERR_OK;
    DWORD           dwStateTransition;

    SP_BEGIN("Pct1ClientProtocolHandler");

    if(pCommOutput) pCommOutput->cbData = 0;
    if(pAppOutput) pAppOutput->cbData = 0;
    /* Protocol handling steps should be listed in most common
     * to least common in order to improve performance
     */

    /* We are not connected, so we're doing
     * protocol negotiation of some sort.  All protocol
     * negotiation messages are sent in the clear */
    /* There are no branches in the connecting protocol
     * state transition diagram, besides connection and error,
     * which means that a simple case statement will do */

    /* Do we have enough data to determine what kind of message we have */
    /* Do we have enough data to determine what kind of message we have, or how much data we need*/

    if(pCommInput->cbData < 3)
    {
        if((pContext->State & 0xffff) == PCT1_STATE_RENEGOTIATE)
        {
            dwStateTransition = (pContext->State & 0xffff);
        }
        else
        {
            pctRet = PCT_INT_INCOMPLETE_MSG;
        }
    }
    else
    {

        dwStateTransition = (((PUCHAR)pCommInput->pvBuffer)[2]<<16) |
                          (pContext->State & 0xffff);
    }

    if(pctRet == PCT_ERR_OK)
    {
        switch(dwStateTransition)
        {
            case PCT1_STATE_RENEGOTIATE:
            {
                SPBuffer In;
                SPBuffer Out;
                DWORD    cbMessage;
                BOOL     fAllocated = FALSE;

                 cbMessage    =  pContext->pCheck->cbCheckSum +
                                pContext->pSystem->BlockSize +
                                sizeof(PCT1_MESSAGE_HEADER_EX) +
                                PCT1_MAX_CLIENT_HELLO;


                    /* are we allocating our own memory? */
                if(pCommOutput->pvBuffer == NULL)
                {
                    pCommOutput->pvBuffer = SPExternalAlloc(cbMessage);
                    if (NULL == pCommOutput->pvBuffer)
                    {
                        SP_RETURN(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));
                    }
                    fAllocated = TRUE;
                    pCommOutput->cbBuffer = cbMessage;
                }


                if(cbMessage > pCommOutput->cbBuffer)
                {
                    if (fAllocated)
                    {
                        SPExternalFree(pCommOutput->pvBuffer);
                        pCommOutput->pvBuffer = NULL;
                        SP_RETURN(PCT_INT_INTERNAL_ERROR);
                    }
                    SP_RETURN(PCT_INT_BUFF_TOO_SMALL);
                }

                In.pvBuffer = ((char *)pCommOutput->pvBuffer)+3;
                In.cbBuffer = pCommOutput->cbBuffer-3;
                In.cbData = 1;

                ((char *)In.pvBuffer)[0] = PCT1_ET_REDO_CONN;

                // Build a Redo Request
                pctRet = Pct1EncryptRaw(pContext, &In, pCommOutput, PCT1_ENCRYPT_ESCAPE);
                if(pctRet != PCT_ERR_OK)
                {
                    if (fAllocated)
                    {
                        SPExternalFree(pCommOutput->pvBuffer);
                        pCommOutput->pvBuffer = NULL;
                    }
                    break;
                }
                Out.pvBuffer = (char *)pCommOutput->pvBuffer + pCommOutput->cbData;
                Out.cbBuffer = pCommOutput->cbBuffer - pCommOutput->cbData;

                if (!SPCacheClone(&pContext->RipeZombie))
                {
                    SPExternalFree(pCommOutput->pvBuffer);
                    pCommOutput->pvBuffer = NULL;
                    SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
                }
                pctRet = GeneratePct1StyleHello(pContext, &Out);
                pCommOutput->cbData += Out.cbData;
                break;
            }
            /* Client receives Server hello */
            case (PCT1_MSG_SERVER_HELLO << 16) | UNI_STATE_CLIENT_HELLO:
            case (PCT1_MSG_SERVER_HELLO << 16) | PCT1_STATE_CLIENT_HELLO:
            {
                PPct1_Server_Hello pHello;
                /* Attempt to recognize and handle various versions
                 * of Server hello, start by trying to unpickle the
                 * oldest, and the next version, until
                 * one unpickles.  Then run the handle code.  We can also put
                 * unpickling and handling code in here for SSL messages */
                if(PCT_ERR_OK == (pctRet = Pct1UnpackServerHello(
                                                    pCommInput,
                                                    &pHello)))
                {
                    /* let's resurrect the zombie session */
                    if (pHello->RestartOk)
                    {
                        pctRet = Pct1CliRestart(pContext, pHello, pCommOutput);
                        if(PCT_ERR_OK == pctRet)
                        {
                            pContext->State = SP_STATE_CONNECTED;
                            pContext->DecryptHandler = Pct1DecryptHandler;
                            pContext->Encrypt = Pct1EncryptMessage;
                            pContext->Decrypt = Pct1DecryptMessage;
                        }
                    }
                    else
                    {

                        pContext->RipeZombie->fProtocol = SP_PROT_PCT1_CLIENT;

                        if (pContext->RipeZombie->cbMasterKey != 0)
                        {
                            // We've attempted to do a reconnect and the server has
                            // blown us off. In this case we must use a new and different
                            // cache entry.
                            if (!SPCacheClone(&pContext->RipeZombie))
                            {
                                pctRet = SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
                            }
                        }
                        
                        if (pctRet == PCT_ERR_OK)
                        {
                            pctRet = Pct1CliHandleServerHello(pContext,
                                                            pCommInput,
                                                            pHello,
                                                            pCommOutput);
                        }
                        if(PCT_ERR_OK == pctRet)
                        {
                            pContext->State = PCT1_STATE_CLIENT_MASTER_KEY;
                            pContext->DecryptHandler = Pct1DecryptHandler;
                            pContext->Encrypt = Pct1EncryptMessage;     /* ?DCB? */
                            pContext->Decrypt = Pct1DecryptMessage;     /* ?DCB? */
                        }

                    }
                    SPExternalFree(pHello);

                }
                else if(pctRet != PCT_INT_INCOMPLETE_MSG)
                {
                    pctRet |= PCT_INT_DROP_CONNECTION;
                }

                if(SP_FATAL(pctRet))
                {
                    pContext->State = PCT1_STATE_ERROR;
                }

                break;
            }

            case (PCT1_MSG_SERVER_VERIFY << 16) | PCT1_STATE_CLIENT_MASTER_KEY:
                pctRet = Pct1CliHandleServerVerify(pContext,
                                                    pCommInput,
                                                    pCommOutput);
                if(SP_FATAL(pctRet))
                {
                    pContext->State = PCT1_STATE_ERROR;
                }
                else
                {
                    if(PCT_ERR_OK == pctRet)
                    {
                        pContext->State = SP_STATE_CONNECTED;
                        pContext->DecryptHandler = Pct1DecryptHandler;
                        pContext->Encrypt = Pct1EncryptMessage;
                        pContext->Decrypt = Pct1DecryptMessage;
                    }
                    /* We received a non-fatal error, so the state doesn't
                     * change, giving the app time to deal with this */
                }
                break;

            default:
                pContext->State = PCT1_STATE_ERROR;
                {
                    pctRet = PCT_INT_ILLEGAL_MSG;
                    if(((PUCHAR)pCommInput->pvBuffer)[2] == PCT1_MSG_ERROR)
                    {
                        /* we received an error message, process it */
                        pctRet = Pct1HandleError(pContext,
                                                 pCommInput,
                                                 pCommOutput);

                    }
                    else
                    {
                        /* we received an unknown error, generate a
                         * PCT_ERR_ILLEGAL_MESSAGE */
                        pctRet = Pct1GenerateError(pContext,
                                                    pCommOutput,
                                                    PCT_ERR_ILLEGAL_MESSAGE,
                                                    NULL);
                    }
                }

        }
    }
    if(pctRet & PCT_INT_DROP_CONNECTION)
    {
        pContext->State &= ~SP_STATE_CONNECTED;
    }
    SP_RETURN(pctRet);
}

SP_STATUS Pct1CliHandleServerHello(PSPContext pContext,
                                   PSPBuffer  pCommInput,
                                   PPct1_Server_Hello pHello,
                                   PSPBuffer  pCommOutput)
{
    /* error to return to peer */
    SP_STATUS          pctRet=PCT_ERR_ILLEGAL_MESSAGE;

    PctPrivateKey *pClientAuthKey = NULL;
    PSessCacheItem      pZombie;
    PSPCredential       pCred;
    PPct1_Client_Master_Key   pCMKey = NULL;
    SPBuffer           ErrData;

    DWORD               i, dwKeyLen;
    DWORD               fMismatch = 0;
    DWORD               cbClientCert = 0;
    SignatureSystem *   pSigSys = NULL;
    CertSystem *        pCertSys = NULL;
    BYTE                MisData[PCT_NUM_MISMATCHES];

    SP_BEGIN("Pct1CliHandleServerHello");
    pCommOutput->cbData = 0;

    /* validate the buffer configuration */
    ErrData.cbData = 0;
    ErrData.pvBuffer = NULL;
    ErrData.cbBuffer = 0;


    pCred = pContext->pCred;
    pZombie = pContext->RipeZombie;
    do {

#if DBG
        DebugLog((DEB_TRACE, "Hello = %x\n", pHello));
        DebugLog((DEB_TRACE, "   Restart\t%s\n", pHello->RestartOk ? "Yes":"No"));
        DebugLog((DEB_TRACE, "   ClientAuth\t%s\n",
                  pHello->ClientAuthReq ? "Yes":"No"));
        DebugLog((DEB_TRACE, "   Certificate Type\t%x\n", pHello->SrvCertSpec));
        DebugLog((DEB_TRACE, "   Hash Type\t%x\n", pHello->SrvHashSpec));
        DebugLog((DEB_TRACE, "   Cipher Type\t%x (%s)\n", pHello->SrvCipherSpec,
        DbgGetNameOfCrypto(pHello->SrvCipherSpec)));
        DebugLog((DEB_TRACE, "   Certificate Len\t%ld\n", pHello->CertificateLen));
#endif
        if(pHello->ClientAuthReq)
        {
            // If we're doing client auth, check to see if we have
            // proper credentials.
//            DWORD j;

            // Choose a certificate/private key pair for client auth.
            pctRet = SPCredPickCertificate(SP_PROT_PCT1_CLIENT,
                                           pCred,
                                           pHello->pClientCertSpecs,
                                           pHello->cCertSpecs,
                                           Pct1CertRank,
                                           Pct1NumCert,
                                           &pZombie->pClientCert,
                                           &pClientAuthKey);


            if (pctRet != PCT_ERR_OK)
            {
                // It's okay to return here as we've not done anything
                // yet.  We need to return this error as a warning.
                if(!(pContext->Flags & CONTEXT_FLAG_NO_INCOMPLETE_CRED_MSG))
                {
                    pctRet = PCT_ERR_INCOMPLETE_CREDS;
                    SP_RETURN( pctRet);
                }
            }
            pctRet  = SPSerializeCertificate(SP_PROT_PCT1_CLIENT,
                                             NULL,
                                             &cbClientCert,
                                             pZombie->pClientCert);
            if(pctRet != PCT_ERR_OK)
            {
                SP_RETURN(pctRet);
            }

        }



        CopyMemory(pContext->pConnectionID,
                   pHello->ConnectionID,
                   pHello->cbConnectionID);

        pContext->cbConnectionID = pHello->cbConnectionID;

        /* Set up server selected systems */
        pZombie->SessCiphSpec = pHello->SrvCipherSpec;
        pZombie->SessHashSpec = pHello->SrvHashSpec;
        pZombie->SessExchSpec = pHello->SrvExchSpec;

        // We know what our ciphers are, so init the cipher system
        pctRet = ContextInitCiphers(pContext);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }


        /* we aren't restarting, so let's continue with the protocol. */

        /* Crack the server certificate */

        pctRet = SPLoadCertificate(pZombie->fProtocol,
                                 pHello->SrvCertSpec,
                                 pHello->pCertificate,
                                 pHello->CertificateLen,
                                 &pZombie->pServerCert,
                                 NULL);


        if(pctRet != PCT_ERR_OK)
        {
            break;
        }

        /* Start building Response message */
        GenerateRandomBits(pZombie->pMasterKey, PCT1_MASTER_KEY_SIZE);
        pZombie->cbMasterKey = PCT1_MASTER_KEY_SIZE;

        /* take care of generating the needed clearkey if we are not */
        /* using a full strength cipher. */

        dwKeyLen = ((pZombie->SessCiphSpec & SP_CIPHER_STRENGTH) >> SP_CSTR_POS) / 8;

        if (dwKeyLen < MASTER_KEY_SIZE)
        {
            pZombie->cbClearKey = MASTER_KEY_SIZE - dwKeyLen;
            GenerateRandomBits(pZombie->pClearKey, pZombie->cbClearKey);
        }
        else
        {
            pZombie->cbClearKey = 0;
        }


        pctRet = PCT_ERR_ILLEGAL_MESSAGE;

        /* Create the verify prelude hashes */
        /* Which should look like  */
        /* Hash(CLIENT_MAC_KEY, Hash( "cvp", CLIENT_HELLO, SERVER_HELLO)) */
        /* Here we just do the inner hash */


        if(pContext->pClHello == NULL) {
            pctRet = SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
            break;
        }
        pctRet = Pct1BeginVerifyPrelude(pContext,
                               pContext->pClHello,
                               pContext->cbClHello,
                               pCommInput->pvBuffer,
                               pCommInput->cbData);


        if(PCT_ERR_OK != pctRet)
        {
            break;
        }


        pCMKey = (PPct1_Client_Master_Key)SPExternalAlloc(sizeof(Pct1_Client_Master_Key) + cbClientCert);

        if (NULL == pCMKey)
        {
            pctRet = SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
            break;
        }



        if(PCT_ERR_OK != (pctRet = Pct1MakeSessionKeys(pContext)))
        {
            break;
        }

        pctRet = Pct1EndVerifyPrelude(pContext,
                                      pCMKey->VerifyPrelude,
                                      &pCMKey->VerifyPreludeLen);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }


        pCMKey->ClearKeyLen = pZombie->cbClearKey;

        if (pZombie->cbClearKey)
        {
            CopyMemory(pCMKey->ClearKey, pZombie->pClearKey, pZombie->cbClearKey);
        }

        pCMKey->KeyArgLen = 0;


        /* Choose a client cert */
        /* For each Cert the server understands, check to see if we */
        /* have that type of cert */

        pCMKey->ClientCertLen = 0;
        pCMKey->ClientCertSpec = 0;
        pCMKey->ClientSigSpec = 0;
        pCMKey->ResponseLen = 0;

        if(pHello->ClientAuthReq &&
           pZombie->pClientCert != NULL &&
           pZombie->pClientCert->cbRawCert != 0)
        {
            DWORD j;
            const SignatureSystem *pSigSys = NULL;

            pCMKey->ClientCertSpec = pZombie->pClientCert->Spec;
            pCMKey->pClientCert = (PUCHAR)(pCMKey+1);

            pctRet  = SPSerializeCertificate(SP_PROT_PCT1_CLIENT,
                                            &pCMKey->pClientCert,
                                            &pCMKey->ClientCertLen,
                                            pZombie->pClientCert);
            if(pctRet != PCT_ERR_OK)
            {
                SP_RETURN(pctRet);
            }



            /* Choose the client signature that we want */
            for(j=0;j<pHello->cSigSpecs;j++)
            {
                if(pSigSys = SigFromSpec(pHello->pClientSigSpecs[j], pZombie->fProtocol))
                {
                    pCMKey->ClientSigSpec = pHello->pClientSigSpecs[j];
                    break;
                }
            }

            if (pSigSys == NULL)
            {
                fMismatch |= PCT_IMIS_CL_SIG;
                pctRet = SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH);
            }

            if(fMismatch) break;

            pCMKey->ResponseLen = PCT_SIGNATURE_SIZE;
            if(!pSigSys->Sign(pCMKey->VerifyPrelude,
                              pCMKey->VerifyPreludeLen,
                              pCMKey->Response,
                              &pCMKey->ResponseLen,
                              pClientAuthKey))
            {
                pctRet = SP_LOG_RESULT(PCT_ERR_BAD_CERTIFICATE);
                break;
            }
        }

        /* ok, we have the master key.  Now, encrypt it with the */
        /* public key we got from our friends on the net... */


        pCMKey->EncryptedKeyLen = ENCRYPTED_KEY_SIZE;


        if(!pContext->pKeyExch->Encrypt(
                            (PUCHAR)pZombie->pMasterKey,
                            pZombie->cbMasterKey,
                            pCMKey->EncryptedKey,
                            &pCMKey->EncryptedKeyLen,
                            pZombie->pServerCert->pPublicKey, 0))
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_BAD_CERTIFICATE);
            break;
        }

        pctRet = PCT_ERR_ILLEGAL_MESSAGE;
        if(PCT_ERR_OK != (pctRet = Pct1PackClientMasterKey(pCMKey,
                                                           pCommOutput)))
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
            break;
        }

        pContext->WriteCounter++;

        if(pCMKey)
        {
            SPExternalFree(pCMKey);
        }
        SP_RETURN(PCT_ERR_OK);
        break;
    } while(TRUE);

    if(pCMKey)
    {
        SPExternalFree(pCMKey);
    }
    if(pctRet == PCT_ERR_SPECS_MISMATCH)
    {
        for(i=0;i<PCT_NUM_MISMATCHES;i++)
        {
            MisData[i] = (BYTE)(fMismatch & 1);
            fMismatch = fMismatch >> 1;
        }

        ErrData.cbData = ErrData.cbBuffer = PCT_NUM_MISMATCHES;
        ErrData.pvBuffer = MisData;
    }

    pctRet = Pct1GenerateError(pContext,
                              pCommOutput,
                              pctRet,
                              &ErrData);


    SP_RETURN(pctRet | PCT_INT_DROP_CONNECTION);

}



SP_STATUS
Pct1CliRestart(PSPContext  pContext,
              PPct1_Server_Hello pHello,
              PSPBuffer pCommOutput)
{
    SP_STATUS           pctRet = PCT_ERR_ILLEGAL_MESSAGE;
//    HashBuf             SumBuf,
//                        SubSumBuf;
//    PCheckSumBuffer     pSum,
//                        pSubSum;
    UCHAR               Response[RESPONSE_SIZE];
    DWORD               cbResponse;
    PPct1_Server_Hello  pLocalHello = pHello;
    PSessCacheItem      pZombie;

    SP_BEGIN("Pct1CliRestart");
    pZombie = pContext->RipeZombie;

    do {
        /* if there's no zombie, the message is wrong.  We can't restart. */

        if(pZombie == NULL  ||
           pZombie->ZombieJuju == FALSE)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
            break;
        }


		CopyMemory(pContext->pConnectionID,
		           pHello->ConnectionID,
		           pHello->cbConnectionID);

        pContext->cbConnectionID = pHello->cbConnectionID;



        // We know what our ciphers are, so init the cipher system
        pctRet = ContextInitCiphers(pContext);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        pctRet = Pct1MakeSessionKeys(pContext);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        pctRet = PCT_ERR_ILLEGAL_MESSAGE;

        DebugLog((DEB_TRACE, "Session Keys Made\n"));
        /* let's check the response in the message */

        /* check the length */
        if (pLocalHello->ResponseLen != pContext->pCheck->cbCheckSum)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
            break;
        }

        /* calculate the correct response */
        /* calculate the response */
        pctRet = Pct1BuildRestartResponse(pContext, Response, &cbResponse);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        /* check it against the response in the message */

        if (memcmp(Response, pLocalHello->Response, pLocalHello->ResponseLen))
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_SERVER_AUTH_FAILED);
            break;
        }
        /* ok, we're done, so let's jettison the auth data */
        pContext->ReadCounter = 1;
        pContext->WriteCounter = 1;

        /* fini. */
        SP_RETURN(PCT_ERR_OK);
    } while (TRUE);

    pctRet = Pct1GenerateError(pContext,
                              pCommOutput,
                              pctRet,
                              NULL);

    SP_RETURN(pctRet | PCT_INT_DROP_CONNECTION);
}




SP_STATUS
Pct1CliHandleServerVerify(
                          PSPContext pContext,
                          PSPBuffer  pCommInput,
                          PSPBuffer  pCommOutput)
{
    SP_STATUS     pctRet = PCT_ERR_ILLEGAL_MESSAGE;

    PCheckSumBuffer         pSum,
                            pSubSum;
    PPct1_Server_Verify     pVerify = NULL;
    HashBuf                 SumBuf,
                            SubSumBuf;
    SPBuffer                ErrData;
    PSessCacheItem      pZombie;


    SP_BEGIN("Pct1CliHandleServerVerify");

    pZombie = pContext->RipeZombie;
    pContext->ReadCounter = 2;
    pContext->WriteCounter = 2;

    pCommOutput->cbData = 0;

    ErrData.cbData = 0;
    ErrData.pvBuffer = NULL;
    ErrData.cbBuffer = 0;

    do
    {

        /* unpack the message */
        pctRet = Pct1UnpackServerVerify(pCommInput, &pVerify);
        if (PCT_ERR_OK != pctRet)
        {
            // If it's an incomplete message or something, just return;
            if(!SP_FATAL(pctRet))
            {
                return pctRet;
            }
            break;
        }



        CloneHashBuf(SumBuf, pContext->ReadMACState, pContext->pCheck);
        InitHashBuf(SubSumBuf, pContext);

        pSum = (PCheckSumBuffer)SumBuf;
        pSubSum = (PCheckSumBuffer)SubSumBuf;

        /* Build this hash : Hash ("cvp", CH_CHALLENGE_DATE, SH_CONNECTION_ID_DATA, SV_SESSION_ID_DATA)
         * Place the result in pAuth->Challenge temporarily */
        pContext->pCheck->Sum(pSubSum,
                              PCT_CONST_RESP_LEN,
                              (PUCHAR)PCT_CONST_RESP);

        pContext->pCheck->Sum(pSubSum,
                              pContext->cbChallenge,
                              pContext->pChallenge);

        pContext->pCheck->Sum(pSubSum,
                              pContext->cbConnectionID,
                              pContext->pConnectionID);

        pContext->pCheck->Sum(pSubSum,
                              PCT_SESSION_ID_SIZE,
                              pVerify->SessionIdData);

        pContext->pCheck->Finalize(pSubSum, pContext->pChallenge);
        pContext->cbChallenge = pContext->pCheck->cbCheckSum;

        /*  Finish the hash, by hashing the result in pAuth->Challenge with
         * SERVER_MAC_KEY.  Note, SERVER_MAC_KEY has was pre-initialized
         * so there is no sum. */

        pContext->pCheck->Sum(pSum,
                              pContext->cbChallenge,
                              pContext->pChallenge);

        pContext->pCheck->Finalize(pSum, pContext->pChallenge);

        /* Result stored */
		pContext->cbChallenge = pContext->pCheck->cbCheckSum;

        if ((pVerify->ResponseLen != pContext->pCheck->cbCheckSum) ||
            (memcmp(pVerify->Response, pContext->pChallenge, pVerify->ResponseLen)))
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
            break;
        }

        CopyMemory(pZombie->SessionID,
                   pVerify->SessionIdData,
                   PCT_SESSION_ID_SIZE);

		pZombie->cbSessionID = PCT_SESSION_ID_SIZE;

        /* done with the verify data */
        SPExternalFree(pVerify);
        pVerify = NULL;

        /* set up the session in cache */
        SPCacheAdd(pContext->RipeZombie);

        SP_RETURN( PCT_ERR_OK );
    } while(TRUE); /* End of polish loop */

    if(pVerify) SPExternalFree(pVerify);

    pctRet = Pct1GenerateError(pContext,
                              pCommOutput,
                              pctRet,
                              NULL);

    SP_RETURN(pctRet | PCT_INT_DROP_CONNECTION);

}

SP_STATUS
WINAPI
GeneratePct1StyleHello(
    PSPContext             pContext,
    PSPBuffer              pOutput)
{
    Pct1_Client_Hello    HelloMessage;
    PSessCacheItem       pZombie;
    CipherSpec          aCipherSpecs[10];
    HashSpec            aHashSpecs[10];
    CertSpec            aCertSpecs[10];
    ExchSpec            aExchSpecs[10];
    DWORD i;

    SP_STATUS pctRet = PCT_INT_INTERNAL_ERROR;

    SP_BEGIN("Pct1CliInstigateHello");

    HelloMessage.pCipherSpecs = aCipherSpecs;
    HelloMessage.pHashSpecs = aHashSpecs;
    HelloMessage.pCertSpecs = aCertSpecs;
    HelloMessage.pExchSpecs = aExchSpecs;

    if(pContext == NULL)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    if (!pOutput)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    pZombie = pContext->RipeZombie;


    pContext->Flags |= CONTEXT_FLAG_CLIENT;

    GenerateRandomBits( pContext->pChallenge, PCT1_CHALLENGE_SIZE );
    pContext->cbChallenge = PCT1_CHALLENGE_SIZE;
    /* Build the hello message. */

    HelloMessage.cbChallenge = PCT1_CHALLENGE_SIZE;
    HelloMessage.pKeyArg = NULL;
    HelloMessage.cbKeyArgSize = 0;


    HelloMessage.cCipherSpecs = 0;
    for(i=0; i < Pct1NumCipher; i++)
    {
        if(NULL != CipherFromSpec(Pct1CipherRank[i], SP_PROT_PCT1_CLIENT))
        {
            HelloMessage.pCipherSpecs[HelloMessage.cCipherSpecs++] = Pct1CipherRank[i];
        }
    }

    HelloMessage.cHashSpecs = 0;
    for(i=0; i < Pct1NumHash; i++)
    {
        if(NULL != HashFromSpec(Pct1HashRank[i], SP_PROT_PCT1_CLIENT))
        {
            HelloMessage.pHashSpecs[HelloMessage.cHashSpecs++] = Pct1HashRank[i];
        }
    }

    HelloMessage.cCertSpecs = 0;
    for(i=0; i < Pct1NumCert; i++)
    {
        if(NULL != CertFromSpec(Pct1CertRank[i], SP_PROT_PCT1_CLIENT))
        {
            HelloMessage.pCertSpecs[HelloMessage.cCertSpecs++] = Pct1CertRank[i];
        }
    }

    HelloMessage.cExchSpecs = 0;
    for(i=0; i < Pct1NumExch; i++)
    {
        if(NULL != KeyExchangeFromSpec(Pct1ExchRank[i], SP_PROT_PCT1_CLIENT))
        {
            HelloMessage.pExchSpecs[HelloMessage.cExchSpecs++] = Pct1ExchRank[i];
        }
    }


    if (pZombie->cbSessionID)
    {
        CopyMemory(HelloMessage.SessionID, pZombie->SessionID, pZombie->cbSessionID);
        HelloMessage.cbSessionID = pZombie->cbSessionID;
    }
    else
    {
	    FillMemory(HelloMessage.SessionID, PCT_SESSION_ID_SIZE, 0);
	    HelloMessage.cbSessionID = PCT_SESSION_ID_SIZE;
    }

    CopyMemory(  HelloMessage.Challenge,
                pContext->pChallenge,
                HelloMessage.cbChallenge );
    HelloMessage.cbChallenge = pContext->cbChallenge;

    pctRet = Pct1PackClientHello(&HelloMessage,  pOutput);

    if(PCT_ERR_OK != pctRet)
    {
        SP_RETURN(pctRet);
    }


    /* we need to keep the actual message around, since we need to */
    /* hash it later, and we don't know the hash algorithm to use */
    /* yet. */

    pContext->pClHello = SPExternalAlloc(pOutput->cbData);
    if(pContext->pClHello == NULL)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));
    }
    CopyMemory(pContext->pClHello, pOutput->pvBuffer, pOutput->cbData);
    pContext->cbClHello = pOutput->cbData;

    /* We set this here to tell the protocol engine that we just send a client
     * hello, and we're expecing a pct server hello */
    pContext->State = PCT1_STATE_CLIENT_HELLO;
    SP_RETURN(PCT_ERR_OK);
}

