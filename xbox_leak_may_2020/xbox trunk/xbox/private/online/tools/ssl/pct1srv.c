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
#include <ssl2msg.h>

SP_STATUS
Pct1SrvHandleUniHello(
    PSPContext          pContext,
    PSPBuffer           pCommInput,
    PSsl2_Client_Hello  pHello,
    PSPBuffer           pCommOutput);



SP_STATUS WINAPI
Pct1ServerProtocolHandler(PSPContext pContext,
                    PSPBuffer  pCommInput,
                    PSPBuffer  pCommOutput,
                    PSPBuffer  pAppOutput)
{
    SP_STATUS      pctRet= 0;
    DWORD          dwStateTransition;

    SP_BEGIN("Pct1ServerProtocolHandler");

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

    dwStateTransition = (pContext->State & 0xffff);

    if(((pContext->State & 0xffff) != SP_STATE_CONNECTED) &&
       ((pContext->State & 0xffff) != PCT1_STATE_RENEGOTIATE))
    {
        if(pCommInput->cbData < 3)
        {
            pctRet = PCT_INT_INCOMPLETE_MSG;
        }
    }
    if(pCommInput->cbData >= 1)
    {
        dwStateTransition |= (((PUCHAR)pCommInput->pvBuffer)[2]<<16);
    }


    if(pctRet == PCT_ERR_OK)
    {
        switch(dwStateTransition)
        {
            case SP_STATE_CONNECTED:
            {
                //We're connected, and we got called, so we must be doing a REDO
                SPBuffer    In;
                DWORD       cbMessage;

                // Calculate size of buffer

                pCommOutput->cbData = 0;

                cbMessage    =  pContext->pCheck->cbCheckSum +
                                pContext->pSystem->BlockSize +
                                sizeof(PCT1_MESSAGE_HEADER_EX);


                    /* are we allocating our own memory? */
                if(pCommOutput->pvBuffer == NULL)
                {
                    pCommOutput->pvBuffer = SPExternalAlloc(cbMessage);
                    if (NULL == pCommOutput->pvBuffer)
                    {
                        SP_RETURN(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));
                    }
                    pCommOutput->cbBuffer = cbMessage;
                }


                if(cbMessage > pCommOutput->cbBuffer)
                {
                    SP_RETURN(PCT_INT_BUFF_TOO_SMALL);
                }

                In.pvBuffer = ((char *)pCommOutput->pvBuffer)+3;
                In.cbBuffer = pCommOutput->cbBuffer-3;
                In.cbData = 1;

                ((char *)In.pvBuffer)[0] = PCT1_ET_REDO_CONN;

                // Build a Redo Request
                pctRet = Pct1EncryptRaw(pContext, &In, pCommOutput, PCT1_ENCRYPT_ESCAPE);
                break;
            }

            /* Server receives client hello */
            case (SSL2_MT_CLIENT_HELLO << 16) | UNI_STATE_RECVD_UNIHELLO:
            {
                PSsl2_Client_Hello pSsl2Hello;

                // Attempt to recognize and handle various versions of client
                // hello, start by trying to unpickle the most recent version, and
                // then next most recent, until one unpickles.  Then run the handle
                // code.  We can also put unpickling and handling code in here for
                // SSL messages.

                pctRet = Ssl2UnpackClientHello(pCommInput, &pSsl2Hello);
                if(PCT_ERR_OK == pctRet)
                {

                    pContext->RipeZombie->fProtocol = SP_PROT_PCT1_SERVER;

                    pctRet = Pct1SrvHandleUniHello(
			                     pContext,
			                     pCommInput,
			                     pSsl2Hello,
			                     pCommOutput);
                    if (PCT_ERR_OK == pctRet)
                    {
                        pContext->State = PCT1_STATE_SERVER_HELLO;
                    }

                    SPExternalFree(pSsl2Hello);
                }

                if (SP_FATAL(pctRet))
                {
	                pContext->State = PCT1_STATE_ERROR;
                }
                break;
            }
            /* Server receives client hello */

            case (PCT1_MSG_CLIENT_HELLO << 16) | PCT1_STATE_RENEGOTIATE:
            {
                PPct1_Client_Hello pPct1Hello;
                UCHAR fRealSessId = 0;
//                SessCacheItem   CachedSession;
//                int i;

                // This is a renegotiate hello, so we do not restart

                pctRet = Pct1UnpackClientHello(
                                pCommInput,
                                &pPct1Hello);

                if(PCT_ERR_OK == pctRet)
                {

                    pContext->RipeZombie->fProtocol = SP_PROT_PCT1_SERVER;
                    pctRet = Pct1SrvHandleClientHello(pContext,
                                                 pCommInput,
                                                 pPct1Hello,
                                                 pCommOutput);
                    if(PCT_ERR_OK == pctRet) {
                        pContext->State = PCT1_STATE_SERVER_HELLO;
                    }

                    SPExternalFree(pPct1Hello);

                }
                else if(pctRet != PCT_INT_INCOMPLETE_MSG)
                {
                    pctRet |= PCT_INT_DROP_CONNECTION;
                }

                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                }
                break;
            }
            case (PCT1_MSG_CLIENT_HELLO << 16) | SP_STATE_NONE:
            {
                PPct1_Client_Hello pPct1Hello;
                UCHAR fRealSessId = 0;
                SessCacheItem   CachedSession;
                int i;

                /* Attempt to recognize and handle various versions
                 * of client hello, start by trying to unpickle the
                 * most recent version, and then next most recent, until
                 * one unpickles.  Then run the handle code.  We can also put
                 * unpickling and handling code in here for SSL messages */
                pctRet = Pct1UnpackClientHello(
                                pCommInput,
                                &pPct1Hello);

                if(PCT_ERR_OK == pctRet)
                {


                    for(i=0;i<(int)pPct1Hello->cbSessionID;i++)
                    {
                        fRealSessId |= pPct1Hello->SessionID[i];
                    }

                    if ((fRealSessId) &&
                        (SPCacheRetrieveBySession(SP_PROT_PCT1_SERVER,
                                                  pPct1Hello->SessionID,
                                                  pPct1Hello->cbSessionID,
                                                  &pContext->RipeZombie)))
                    {
                        // We have a good zombie
                        pctRet = Pct1SrvRestart(pContext,
                                                pPct1Hello,
                                                pCommOutput,
                                                &CachedSession);

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


                        pContext->RipeZombie->fProtocol = SP_PROT_PCT1_SERVER;
                        pctRet = Pct1SrvHandleClientHello(pContext,
                                                     pCommInput,
                                                     pPct1Hello,
                                                     pCommOutput);
                        if(PCT_ERR_OK == pctRet) {
                            pContext->State = PCT1_STATE_SERVER_HELLO;
                        }

                    }
                    SPExternalFree(pPct1Hello);

                }
                else if(pctRet != PCT_INT_INCOMPLETE_MSG)
                {
                    pctRet |= PCT_INT_DROP_CONNECTION;
                }

                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                }
                break;
            }
            case (PCT1_MSG_CLIENT_MASTER_KEY << 16) | PCT1_STATE_SERVER_HELLO:
                pctRet = Pct1SrvHandleCMKey(pContext,
                                            pCommInput,
                                            pCommOutput);
                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                } else {
                    if(PCT_ERR_OK == pctRet) {
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

                    } else {
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
    if(pctRet & PCT_INT_DROP_CONNECTION) {
        pContext->State &= ~SP_STATE_CONNECTED;
    }
    SP_RETURN(pctRet);
}

SP_STATUS
Pct1SrvHandleUniHello(
    PSPContext         pContext,
    PSPBuffer           pCommInput,
    PSsl2_Client_Hello  pHello,
    PSPBuffer           pCommOutput)
{
    SP_STATUS pctRet = PCT_ERR_ILLEGAL_MESSAGE;
    PSPCredential  pCred;
    Pct1_Server_Hello    Reply;
    DWORD           i,/* j,*/ k , fMismatch;
    SPBuffer        ErrData;
    DWORD           ClientCipher;
    DWORD           PctCipher;
    PSessCacheItem  pZombie;


    BOOL            fCipher = FALSE;
    BOOL            fHash = FALSE;
    BOOL            fExch = FALSE;
    BOOL            fCert = FALSE;
    CertSpec        CertSpec = 0;


    SP_BEGIN("Pct1SrvHandlUniHello");

    pCommOutput->cbData = 0;

    /* validate the buffer configuration */

    if(NULL == pContext)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    pCred = pContext->pCred;
    pZombie = pContext->RipeZombie;

    if (!pCred)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }
    /* validate the buffer configuration */
    ErrData.cbData = 0;
    ErrData.pvBuffer = NULL;
    ErrData.cbBuffer = 0;

    do {

        FillMemory( &Reply, sizeof( Reply ), 0 );

        //
        // Calculate common ciphers:
        //

        pContext->pSystem = NULL;
        pContext->pCheck = NULL;
        pContext->pKeyExch = NULL;


        for (ClientCipher = 0;
             ClientCipher < pHello->cCipherSpecs ;
             ClientCipher++ )
        {
            PctCipher = pHello->CipherSpecs[ClientCipher] & 0xffff;

            switch(pHello->CipherSpecs[ClientCipher] >> 16)
            {
                case PCT_SSL_HASH_TYPE:
                {
                    if(!fHash)
                    {
                        // We've not picked a hash yet, so
                        // pick one.
                        for(k=0; k < Pct1NumHash; k++) {
                            if(Pct1HashRank[k] == PctCipher)
                            {
                                if(HashFromSpec(PctCipher, SP_PROT_PCT1_SERVER))
                                {
                                    fHash = TRUE;
                                    pZombie->SessHashSpec = PctCipher;
                                    Reply.SrvHashSpec = PctCipher;
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }

                case PCT_SSL_EXCH_TYPE:
                {
                    if(!fExch)
                    {
                        for(k=0; k < Pct1NumExch; k++) {
                            if(Pct1ExchRank[k] == PctCipher)
                            {
                                if(KeyExchangeFromSpec(PctCipher, SP_PROT_PCT1_SERVER))
                                {
                                    fExch = TRUE;
                                    pZombie->SessExchSpec = PctCipher;
                                    Reply.SrvExchSpec = PctCipher;
                                    break;
                                }
                            }
                        }

                    }
                    break;
                }

                case PCT_SSL_CIPHER_TYPE_1ST_HALF:
                {
                    if(!fCipher)
                    {

                        // Do we have enough room for a 2nd half.
                        if(ClientCipher+1 >= pHello->cCipherSpecs)
                        {
                            break;
                        }
                        if((pHello->CipherSpecs[ClientCipher+1] >> 16) != PCT_SSL_CIPHER_TYPE_2ND_HALF)
                        {
                            break;
                        }

                        PctCipher = (pHello->CipherSpecs[ClientCipher+1] & 0xffff) |
                                     (PctCipher<< 16);

                        for(k=0; k < Pct1NumCipher; k++)
                        {
                            if(Pct1CipherRank[k] == PctCipher)
                            {
                                if(CipherFromSpec(PctCipher, SP_PROT_PCT1_SERVER))
                                {
                                    fCipher = TRUE;
                                    pZombie->SessCiphSpec = PctCipher;
                                    Reply.SrvCipherSpec = PctCipher;
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }

                case PCT_SSL_CERT_TYPE:
                {
                    if(!fCert)
                    {
                         pctRet = SPCredPickCertificate(SP_PROT_PCT1_SERVER,
                                                       pCred,
                                                       Pct1CertRank,
                                                       Pct1NumCert,
                                                       &PctCipher,
                                                       1,
                                                       &pZombie->pServerCert,
                                                       &pContext->pPrivateKey);

                        if(PCT_ERR_OK == pctRet)
                        {
                            fCert = TRUE;
                            Reply.SrvCertSpec = PctCipher;
                            Reply.pCertificate = pZombie->pServerCert->pRawCert;
                            Reply.CertificateLen = pZombie->pServerCert->cbRawCert;
                        }
                    }
                    break;
                }
             }
        }

        fMismatch = 0;
        if (!fCert)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH);
            fMismatch |= PCT_IMIS_CERT;
        }
        if (!fCipher)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH);
            fMismatch |= PCT_IMIS_CIPHER;
        }
        if (!fHash)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH);
            fMismatch |= PCT_IMIS_HASH;
        }
        if (!fExch)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH);
            fMismatch |= PCT_IMIS_EXCH;
        }

        pctRet = ContextInitCiphers(pContext);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }
        /* Generate a conneciton id to use while establishing connection */

        GenerateRandomBits(  Reply.ConnectionID,
                             PCT1_SESSION_ID_SIZE );
		Reply.cbConnectionID = PCT1_SESSION_ID_SIZE;

        CopyMemory( pContext->pConnectionID,
                    Reply.ConnectionID,
                    PCT1_SESSION_ID_SIZE );

	    pContext->cbConnectionID = PCT1_SESSION_ID_SIZE;

        /* Note, there are no restarts allowed in a UNIHELLO  */

        Reply.RestartOk = FALSE;
        Reply.ClientAuthReq = ((pContext->Flags & CONTEXT_FLAG_CLIAUTH) != 0);

         /* sig and cert specs are pre-zeroed when Reply is initialized */

        if(Reply.ClientAuthReq)
        {
            Reply.pClientSigSpecs = Pct1SigRank;
            Reply.cSigSpecs = Pct1NumSig;

            Reply.pClientCertSpecs = Pct1CertRank;
            Reply.cCertSpecs = Pct1NumCert;
        }
        /* Make the SSL2 challenge into a PCT1 challenge as per the
         * compatability doc. */

        CopyMemory( pContext->pChallenge,
                    pHello->Challenge,
                    pHello->cbChallenge);


        for(i=0; i < pHello->cbChallenge; i++)
        {
            pContext->pChallenge[i + pHello->cbChallenge] = ~pContext->pChallenge[i];
        }
        pContext->cbChallenge = 2*pHello->cbChallenge;



        pctRet = Pct1PackServerHello(&Reply, pCommOutput);
        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        /* Regenerate the internal pVerifyPrelude, so we */
        /* can match it against the client when we get the */
        /* client master key */

        pctRet = Pct1BeginVerifyPrelude(pContext,
                               pCommInput->pvBuffer,
                               pCommInput->cbData,
                               pCommOutput->pvBuffer,
                               pCommOutput->cbData);



        if(PCT_ERR_OK != pctRet)
        {
            break;
        }


        SP_RETURN(PCT_ERR_OK);
    } while (TRUE); /* end Polish Loop */
     if(pctRet == PCT_ERR_SPECS_MISMATCH) {
            BYTE            MisData[PCT_NUM_MISMATCHES];

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



/* Otherwise known as Handle Client Hello */
SP_STATUS
Pct1SrvHandleClientHello(
    PSPContext          pContext,
    PSPBuffer           pCommInput,
    PPct1_Client_Hello  pHello,
    PSPBuffer           pCommOutput
    )
{
    SP_STATUS           pctRet = PCT_ERR_ILLEGAL_MESSAGE;
    PSPCredential       pCred;
    Pct1_Server_Hello   Reply;
    DWORD               i, j, /*k ,*/ fMismatch;
    BYTE                MisData[PCT_NUM_MISMATCHES];
    SPBuffer            ErrData;
    PSessCacheItem      pZombie;
    BOOL            fCipher = FALSE;
    BOOL            fHash = FALSE;
    BOOL            fExch = FALSE;
    BOOL            fCert = FALSE;
#if DBG
    DWORD               di;
#endif

    SP_BEGIN("Pct1SrvHandleClientHello");

    pCommOutput->cbData = 0;

    /* validate the buffer configuration */
    ErrData.cbData = 0;
    ErrData.pvBuffer = NULL;
    ErrData.cbBuffer = 0;

    pZombie = pContext->RipeZombie;


    pCred = pContext->pCred;
    if (!pCred)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }


    do {

#if DBG
        DebugLog((DEB_TRACE, "Client Hello at %x\n", pHello));
        DebugLog((DEB_TRACE, "  CipherSpecs  %d\n", pHello->cCipherSpecs));
        for (di = 0 ; di < pHello->cCipherSpecs ; di++ )
        {
            DebugLog((DEB_TRACE, "    Cipher[%d] = %06x (%s)\n", di,
                      pHello->pCipherSpecs[di],
                      DbgGetNameOfCrypto(pHello->pCipherSpecs[di]) ));
        }
#endif


        /* store the challenge in the auth block */
        CopyMemory( pContext->pChallenge,
                    pHello->Challenge,
                    pHello->cbChallenge );
		pContext->cbChallenge = pHello->cbChallenge;


        CopyMemory( pZombie->SessionID,
                    pHello->SessionID,
                    pHello->cbSessionID );
	    pZombie->cbSessionID = pHello->cbSessionID;

        /* Begin to build the server hello */
        FillMemory( &Reply, sizeof( Reply ), 0 );

        /* no matter what, we need to make a new connection id */

        GenerateRandomBits(  Reply.ConnectionID,
                             PCT1_SESSION_ID_SIZE );
		Reply.cbConnectionID = PCT1_SESSION_ID_SIZE;

        CopyMemory( pContext->pConnectionID,
                    Reply.ConnectionID,
                    PCT1_SESSION_ID_SIZE );

	    pContext->cbConnectionID = PCT_SESSION_ID_SIZE;

        /* no restart case */
        /* fill in from properties here... */

        Reply.RestartOk = FALSE;
        Reply.ClientAuthReq = ((pContext->Flags & CONTEXT_FLAG_CLIAUTH) != 0);

        fMismatch = 0;
        /* pick a cert, any cert   */
        /* Server order of preference for certificates:
         * Hardcoded Preference (CertRank)
         *    Client Preference    (Hello)
         *        What server has     (pCred)
         */

        pctRet = SPCredPickCertificate(SP_PROT_PCT1_SERVER,
                                       pCred,
                                       Pct1CertRank,
                                       Pct1NumCert,
                                       pHello->pCertSpecs,
                                       pHello->cCertSpecs,
                                       &pZombie->pServerCert,
                                       &pContext->pPrivateKey);

        if(PCT_ERR_OK != pctRet)
        {
            fMismatch |= PCT_IMIS_CERT;
        }
        else
        {

            Reply.SrvCertSpec =     pZombie->pServerCert->Spec;
            Reply.pCertificate =    pZombie->pServerCert->pRawCert;
            Reply.CertificateLen =  pZombie->pServerCert->cbRawCert;
        }



        /* Determine Cipher to use */
        /* Cipher order of preference:
         * Server Preference
         *    Client Preference
         */
        for(i=0; i < Pct1NumCipher; i++)
        {
            // Do we enable this cipher
            if(NULL == CipherFromSpec(Pct1CipherRank[i], SP_PROT_PCT1_SERVER))
            {
                continue;
            }

            for(j=0; j< pHello->cCipherSpecs; j++)
            {
                // Does the client want this cipher type
                if(Pct1CipherRank[i] == pHello->pCipherSpecs[j])
                {
                    fCipher = TRUE;
                    pZombie->SessCiphSpec = Pct1CipherRank[i];
                    Reply.SrvCipherSpec = Pct1CipherRank[i];
                    break;
                }
            }
            if(fCipher)
            {
                break;
            }

        }

        if (!fCipher)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH);
            fMismatch |= PCT_IMIS_CIPHER;
        }

        /* Determine Hash to use */
        /* Hash order of preference:
         * Server Preference
         *    Client Preference
         */
        for(i=0; i < Pct1NumHash; i++)
        {
            // Do we enable this cipher
            if(NULL == HashFromSpec(Pct1HashRank[i], SP_PROT_PCT1_SERVER))
            {
                continue;
            }

            for(j=0; j< pHello->cHashSpecs; j++)
            {
                // Does the client want this cipher type
                if(Pct1HashRank[i] == pHello->pHashSpecs[j])
                {
                    fHash = TRUE;
                    pZombie->SessHashSpec = Pct1HashRank[i];
                    Reply.SrvHashSpec = Pct1HashRank[i];
                    break;
                }
            }
            if(fHash)
            {
                break;
            }
        }

        if (!fHash)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH);
            fMismatch |= PCT_IMIS_HASH;
        }
        /* Determine Key Exchange to use */
        /* Key Exchange order of preference:
         * Server Preference
         *    Client Preference
         */
        for(i=0; i < Pct1NumExch; i++)
        {
            // Do we enable this cipher
            if(NULL == KeyExchangeFromSpec(Pct1ExchRank[i], SP_PROT_PCT1_SERVER))
            {
                continue;
            }

            for(j=0; j< pHello->cExchSpecs; j++)
            {
                // Does the client want this cipher type
                if(Pct1ExchRank[i] == pHello->pExchSpecs[j])
                {
                    fExch = TRUE;
                    pZombie->SessExchSpec = Pct1ExchRank[i];
                    Reply.SrvExchSpec = Pct1ExchRank[i];
                    break;
                }
            }
            if(fExch)
            {
                break;
            }
        }


        if (!fExch)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH);
            fMismatch |= PCT_IMIS_EXCH;
        }


        if (fMismatch) {
            pctRet = PCT_ERR_SPECS_MISMATCH;
            break;
        }

        pctRet = ContextInitCiphers(pContext);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        /* sig and cert specs are pre-zeroed when Reply is initialized */

        if(Reply.ClientAuthReq)
        {
            Reply.pClientSigSpecs = Pct1SigRank;
            Reply.cSigSpecs = Pct1NumSig;

            Reply.pClientCertSpecs = Pct1CertRank;
            Reply.cCertSpecs = Pct1NumCert;
        }



#if DBG
        DebugLog((DEB_TRACE, "Server picks cipher %06x (%s)\n",
                  Reply.SrvCipherSpec,
                  DbgGetNameOfCrypto(Reply.SrvCipherSpec) ));
#endif


        Reply.ResponseLen = 0;

        pctRet = Pct1PackServerHello(&Reply, pCommOutput);
        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        /* Regenerate the internal pVerifyPrelude, so we */
        /* can match it against the client when we get the */
        /* client master key */

        pctRet = Pct1BeginVerifyPrelude(pContext,
                               pCommInput->pvBuffer,
                               pCommInput->cbData,
                               pCommOutput->pvBuffer,
                               pCommOutput->cbData);



        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        SP_RETURN(PCT_ERR_OK);
    } while (TRUE); /* end Polish Loop */



    if(pctRet == PCT_ERR_SPECS_MISMATCH) {
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
Pct1SrvHandleCMKey(
    PSPContext     pContext,
    PSPBuffer       pCommInput,
    PSPBuffer       pCommOutput)
{
    SP_STATUS          pctRet = PCT_ERR_ILLEGAL_MESSAGE;
    PPct1_Client_Master_Key  pMasterKey = NULL;
    DWORD               dwKeyLen;
    DWORD               EncryptedLen;
    PCheckSumBuffer     pSum, pSubSum;
    HashBuf             SumBuf, SubSumBuf;
    Pct1_Server_Verify       Verify;
    UCHAR               VerifyPrelude[RESPONSE_SIZE];
    DWORD               cbVerifyPrelude;
    SPBuffer           ErrData;
    DWORD k;
    PSessCacheItem      pZombie;

    const SignatureSystem     *pSigSys = NULL;

    SP_BEGIN("PctHandleClientMasterKey");

    pCommOutput->cbData = 0;

    ErrData.cbData = 0;
    ErrData.pvBuffer = NULL;
    ErrData.cbBuffer = 0;
    pZombie = pContext->RipeZombie;

    do {


        pctRet = Pct1UnpackClientMasterKey(pCommInput, &pMasterKey);
        if (PCT_ERR_OK != pctRet)
        {
            // If it's an incomplete message or something, just return;
            if(!SP_FATAL(pctRet))
            {
                return pctRet;
            }
            break;
        }




        /* Validate that the client properly authed */

        /* The server requested client auth */
        /* NOTE: this deviates from the first pct 1.0 spec,
         * Now, we continue with the protocol if client
         * auth fails.  By the first spec, we should
         * drop the connection */

        if (pContext->Flags & CONTEXT_FLAG_CLIAUTH)
        {



            /* Client auth polish loop */
            pctRet = PCT_ERR_OK;
            do
            {


                /* check to see if the client sent no cert */
                if(pMasterKey->ClientCertLen == 0)
                {
                    /* No client auth */
                    break;
                }

                pctRet = SPLoadCertificate(SP_PROT_PCT1_SERVER,
                                           pMasterKey->ClientCertSpec,
                                           pMasterKey->pClientCert,
                                           pMasterKey->ClientCertLen,
                                           &pZombie->pClientCert,
                                           NULL);

                if(PCT_ERR_OK != pctRet)
                {
                    break;
                }
                if(pZombie->pClientCert == NULL)
                {
                    break;
                }



                /* verify that we got a sig type that meets PCT spec */
                for(k=0; k < Pct1NumSig; k++) {
                    if(Pct1SigRank[k] == pMasterKey->ClientSigSpec)
                    {
                        break;
                    }
                }

                if(k == Pct1NumSig)
                {
                    break;
                }

                if(NULL == (pSigSys = SigFromSpec(pMasterKey->ClientSigSpec, SP_PROT_PCT1_SERVER)))
                {
                    pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
                    break;
                }


                /* Validate Signature */
                if(!pSigSys->Verify(pMasterKey->VerifyPrelude,
                               pMasterKey->VerifyPreludeLen,
                               pMasterKey->Response,
                               pMasterKey->ResponseLen,
                               pZombie->pClientCert->pPublicKey))
                {
                    /* client auth signature failed to verify, so client auth
                     * does not happen
                     */
                    break;
                }

                pctRet = SPContextDoMapping(pContext);


            } while(FALSE); /* end polish loop */

            if(PCT_ERR_OK != pctRet)
            {
                break;
            }

        }

        /* Client auth was successful */
        pctRet = PCT_ERR_ILLEGAL_MESSAGE;
        EncryptedLen = PCT1_MASTER_KEY_SIZE;

        if(pMasterKey->ClearKeyLen > PCT1_MASTER_KEY_SIZE)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
            break;
        }

        /* Decrypt the client keys */
        if ((!pContext->pKeyExch->Decrypt(pMasterKey->EncryptedKey,
                             pMasterKey->EncryptedKeyLen,
                             pZombie->pMasterKey,
                             &EncryptedLen,
                             pContext->pPrivateKey,
                             SP_PROT_PCT1_SERVER)) ||
            (EncryptedLen != MASTER_KEY_SIZE))
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
            break;
        }


        dwKeyLen = ((pZombie->SessCiphSpec & SP_CIPHER_STRENGTH) >> SP_CSTR_POS) / 8;

        if((dwKeyLen == PCT1_MASTER_KEY_SIZE ) && (pMasterKey->ClearKeyLen))
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
            break;
        }

        pZombie->cbClearKey = pMasterKey->ClearKeyLen;

        CopyMemory(pZombie->pClearKey, pMasterKey->ClearKey, pZombie->cbClearKey);

        pctRet = Pct1MakeSessionKeys( pContext);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        pctRet = PCT_ERR_INTEGRITY_CHECK_FAILED;


        if (pMasterKey->VerifyPreludeLen != pContext->pCheck->cbCheckSum)
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_INTEGRITY_CHECK_FAILED);
            break;
        }

        /* Check the verify prelude hashes */
        /* Hash(CLIENT_MAC_KEY, Hash( "cvp", CLIENT_HELLO, SERVER_HELLO)) */
        /* The internal hash should already be in the verify prelude buffer */
        /* from the handle client master key. */

        pctRet = Pct1EndVerifyPrelude(pContext, VerifyPrelude, &cbVerifyPrelude);
        if(PCT_ERR_OK != pctRet)
        {
            break;
        }


        /* Did the verify prelude hash successfully? */
        if(memcmp(VerifyPrelude, pMasterKey->VerifyPrelude, pContext->pCheck->cbCheckSum))
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_INTEGRITY_CHECK_FAILED);
            break;
        }

        /* don't need master key info anymore */
        SPExternalFree(pMasterKey);
        pMasterKey = NULL;


        pContext->WriteCounter = 2;
        pContext->ReadCounter = 2;

        GenerateRandomBits( pZombie->SessionID, PCT1_SESSION_ID_SIZE );
        pZombie->cbSessionID = PCT1_SESSION_ID_SIZE;

        CopyMemory( Verify.SessionIdData,
                    pZombie->SessionID,
                    pZombie->cbSessionID);

        /* compute the response */

        CloneHashBuf(SumBuf, pContext->WriteMACState, pContext->pCheck);
        InitHashBuf(SubSumBuf, pContext);

        pSum = (PCheckSumBuffer)SumBuf;
        pSubSum = (PCheckSumBuffer)SubSumBuf;

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
                              pZombie->cbSessionID,
                              pZombie->SessionID);

        /* we don't need the challenge anymore, so finish the compare hash */
        /* out into the challenge data memory. */

        pContext->pCheck->Finalize(pSubSum, pContext->pChallenge);

        pContext->pCheck->Sum(pSum,
                              pContext->pCheck->cbCheckSum,
                              pContext->pChallenge);
        pContext->pCheck->Finalize(pSum, pContext->pChallenge);

		pContext->cbChallenge = pContext->pCheck->cbCheckSum;
		
        CopyMemory( Verify.Response, pContext->pChallenge,
                    pContext->pCheck->cbCheckSum );

        Verify.ResponseLen = pContext->pCheck->cbCheckSum;

        pctRet = Pct1PackServerVerify(&Verify, pCommOutput);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        /* set up the session in cache */
        SPCacheAdd(pContext->RipeZombie);

        SP_RETURN(PCT_ERR_OK);
    } while(TRUE); /* End of polish loop */

    if(pMasterKey) SPExternalFree(pMasterKey);

    pctRet = Pct1GenerateError(pContext,
                              pCommOutput,
                              pctRet,
                              NULL);

    SP_RETURN(pctRet | PCT_INT_DROP_CONNECTION);

}


SP_STATUS
Pct1SrvRestart(
    PSPContext         pContext,
    PPct1_Client_Hello  pHello,
    PSPBuffer           pCommOutput,
    SessCacheItem *     CachedSession)
{
//    PCheckSumBuffer pSum, pSubSum;
//    HashBuf         SumBuf, SubSumBuf;
    Pct1_Server_Hello    Reply;
    SPBuffer       ErrData;
    SP_STATUS	 pctRet = PCT_INT_ILLEGAL_MSG;
    PSessCacheItem      pZombie;

    SP_BEGIN("Pct1SrvRestart");

    pCommOutput->cbData = 0;

    /* validate the buffer configuration */
    ErrData.cbData = 0;
    ErrData.pvBuffer = NULL;
    ErrData.cbBuffer = 0;

    pZombie = pContext->RipeZombie;



     do {

        /* store the challenge in the auth block */
        CopyMemory( pContext->pChallenge,
                    pHello->Challenge,
                    pHello->cbChallenge );
		pContext->cbChallenge = pHello->cbChallenge;


        /* Begin to build the server hello */
        FillMemory( &Reply, sizeof( Reply ), 0 );


        /* Generate new connection id */
        GenerateRandomBits(  Reply.ConnectionID,
                             PCT1_SESSION_ID_SIZE );
		Reply.cbConnectionID = PCT1_SESSION_ID_SIZE;

        CopyMemory( pContext->pConnectionID,
                    Reply.ConnectionID,
                    Reply.cbConnectionID );
		pContext->cbConnectionID = Reply.cbConnectionID;

        Reply.RestartOk = TRUE;


        /* We don't pass a server cert back during a restart */
        Reply.pCertificate = NULL;
        Reply.CertificateLen = 0;
        /* setup the context */


        Reply.SrvCipherSpec = pZombie->SessCiphSpec;
        Reply.SrvHashSpec =   pZombie->SessHashSpec;
        Reply.SrvCertSpec =   pZombie->pServerCert->Spec;
        Reply.SrvExchSpec =   pZombie->SessExchSpec;

        // We know what our ciphers are, so init the cipher system
        pctRet = ContextInitCiphers(pContext);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        if ((pZombie->SessCiphSpec & SP_CIPHER_STRENGTH) <
            SP_ENC_BITS_128)
        {
            Pct1MakeSessionKeys(pContext);
        }
        else
        {
            Pct1MakeSessionKeys(pContext);
        }

        /* calculate the response */
        pctRet = Pct1BuildRestartResponse(pContext, Reply.Response, &Reply.ResponseLen);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        pctRet = Pct1PackServerHello(&Reply, pCommOutput);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }

        pContext->ReadCounter = 1;
        pContext->WriteCounter = 1;

        SP_RETURN(PCT_ERR_OK);
    } while (TRUE);
    pctRet = Pct1GenerateError(pContext,
                              pCommOutput,
                              pctRet,
                              &ErrData);


    SP_RETURN(pctRet);
}
