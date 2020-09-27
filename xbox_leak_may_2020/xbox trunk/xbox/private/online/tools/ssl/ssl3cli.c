//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       context.c
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    8-08-95   Ramas   Created
//
//----------------------------------------------------------------------------

#include <spbase.h>
#include <md5.h>
#include <sha.h>
#include <ssl2msg.h>
#include <ssl3msg.h>
#include <ssl3key.h>
#include <ssl2prot.h>
#include <rsa.h>
#include <ssl3sign.h>




DWORD
IMacParseSrvHello(
    PUCHAR  pSrvHello,
    DWORD   cbMsg,
    PUCHAR  prgchHandshake,
    SHSH *  prgpshsh[]
);

BOOL FImportServerPublicKeyBlob(
    PBYTE    pbMod,                 // in
    INT      iModLen,               // in
    PBYTE    pbExp,                 // in
    INT      iKeyExpLen,            // in
    PctPublicKey   **ppKey) ;  // out

#define iHandshakeMax 10



BOOL
FCompleteMessageSrvHello(
    PSPContext  pContext,
    PBYTE       pSrvHello,
    DWORD       cbMessage,
    DWORD *     pichHandshakeMac,
    PUCHAR      prgchHandshake,
    SHSH *      prgpshsh[],
    BOOL *      pfRestartServer
);

SP_STATUS
ParseHelloRequest(
    PSPContext  pContext,
    PUCHAR      pSrvHello,
    DWORD       cbMessage,
    PSPBuffer   pCommOutput
);

SP_STATUS
BuildCertVerify(
    PctPrivateKey * pClientAuthKey,
    PctCertificate *pClientCert,
    PSPContext      pContext,
    PBYTE           pb,
    DWORD *         pdwcbCertVerify
);

SP_STATUS
PatchNetscapeBugAndStore
(
    PSPContext  pContext,
    PBYTE       pb
);

DWORD
CbLenOfEncode(DWORD dw, PBYTE pbDst);



static
BOOL
FSsl3ParseServerKeyExchange(
    PSPContext      pContext,
    PBYTE           pbMessage,
    DWORD           iMessageLen,
    LPBSAFE_PUB_KEY lpServerPublicKey,      //in
    PctPublicKey   **ppKey);

#define PbSessionid(pssh)  (((BYTE *)&pssh->cbSessionId) + 1)


SP_STATUS
ParseCertificateRequest
(
    PSPContext          pContext,
    PBYTE               pb,
    DWORD               dwcb,
    PctCertificate **   ppClientCert,
    DWORD *             pdwcbCert,
    PctPrivateKey **    ppClientAuthKey
);




//--------------------------------------------------------------------------------------------
SP_STATUS WINAPI
Ssl3ClientProtocolHandler(
    PSPContext  pContext,
    PSPBuffer pCommInput,
    PSPBuffer pCommOutput,
    PSPBuffer pAppOutput)
{
    SP_STATUS pctRet = 0;
    DWORD cMessageType;
    SWRAP *pswrap;
    DWORD dwStateTransition;
    SPBuffer MsgInput;
    DWORD cbMsg;
//    PUCHAR pb;
//    UCHAR bCT;
    DWORD cbDataT;

    if (NULL != pCommOutput)
    {
        pCommOutput->cbData = 0;
    }
    if (NULL != pAppOutput)
    {
        pAppOutput->cbData = 0;
    }


    MsgInput.pvBuffer = pCommInput->pvBuffer;
    MsgInput.cbBuffer = pCommInput->cbBuffer;
    cbDataT = MsgInput.cbData = pCommInput->cbData;

    // Did we just process a Hello Request?
    if(pCommInput->cbData < 3)
    {
        if((pContext->State & 0xffff) == SSL3_STATE_HELLO_REQUEST)
        {
            dwStateTransition = (pContext->State & 0xffff);
            cbMsg = 0;
            pswrap = NULL;
            cMessageType = 0;
        }
        else
        {
            pctRet = PCT_INT_INCOMPLETE_MSG;
        }
    }
    else
    {

        pswrap = (SWRAP *) MsgInput.pvBuffer;
	    cbMsg = COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize);
	    if (cbMsg > MsgInput.cbData)
	    {
	        return PCT_INT_INCOMPLETE_MSG;
	    }

        pCommInput->cbData = cbMsg + sizeof(SWRAP); //Do One record at one time...

        if(0 != pContext->wS3CipherSuiteServer)
        {
            pctRet = UnwrapSsl3Message(pContext, &MsgInput);

        }
        else
        {
        	pContext->ReadCounter++;
        }

        switch(((PUCHAR) MsgInput.pvBuffer)[0])
        {
            case SSL3_CT_ALERT:
                cMessageType = SSL3_CT_ALERT;
                break;
            case SSL3_CT_CHANGE_CIPHER_SPEC:
                cMessageType = SSL3_CT_CHANGE_CIPHER_SPEC;
                break;
            case SSL3_CT_HANDSHAKE:
                if(pContext->State == SP_STATE_CONNECTED)
                {//We may be  getting a REDO message
                    DebugLog((DEB_WARN, "May be a ReDO"));
                    pContext->State = SSL3_STATE_CLIENT_HELLO;
                }
                cMessageType = ((PUCHAR) MsgInput.pvBuffer)[5];
                break;
            default:
                pctRet = PCT_INT_ILLEGAL_MSG;
                break;
        }



	    if(cMessageType == SSL3_CT_ALERT)
	        dwStateTransition = cMessageType ;
	    else
	        dwStateTransition = (cMessageType << 16) |
	                        (pContext->State & 0xffff);
    }

    if (pctRet != PCT_ERR_OK)
    {
        // to handle incomplete message errors
        return(pctRet);
    }

    switch(dwStateTransition)
    {
        // Client receives Server hello:

        case  (SSL3_CT_ALERT):
            pctRet = ParseAlertMessage(pContext,
                                        MsgInput.pvBuffer,
                                        MsgInput.cbData,
                                        pCommOutput);

            break;

        case  SSL3_STATE_HELLO_REQUEST:
        	//Temp Disabling Reconnect during REDO
        	if(SPCacheRetrieveNew(NULL, &pContext->RipeZombie))
            	{
            	pContext->RipeZombie->fProtocol = pContext->pCred->Type;
	            pctRet =  GenerateSsl3ClientHello(pContext, pCommOutput);
	            pContext->State = SSL3_STATE_CLIENT_HELLO;
            	}
            break;

        case  (SSL3_HS_SERVER_HELLO << 16) | SSL3_STATE_CLIENT_HELLO:
        case  (SSL3_HS_SERVER_HELLO << 16) | UNI_STATE_CLIENT_HELLO:
        {

            pctRet = Ssl3CliHandleServerHello(
                                                pContext,
                                                MsgInput.pvBuffer,
                                                MsgInput.cbData,
                                                pCommOutput);
            if (SP_FATAL(pctRet))
            {
                pContext->State = PCT1_STATE_ERROR;
            }
            else if(pContext->State != SSL3_STATE_RESTART_CCS)
            {
                //for Server hello, we take in one big chunk... if not RESTART...
                pCommInput->cbData = cbDataT;
            }
        }
        break;

        case (SSL3_CT_CHANGE_CIPHER_SPEC << 16) | SSL3_STATE_RESTART_CCS:
            pctRet = Ssl3HandleCCS(
                                    pContext,
		                            MsgInput.pvBuffer,
		                            MsgInput.cbData,
		                            FALSE);

            if (PCT_ERR_OK == pctRet)
            {
                pContext->State = SSL3_STATE_RESTART_SERVER_FINISH;
            }
            break;

        case (SSL3_CT_CHANGE_CIPHER_SPEC << 16) | SSL3_STATE_CHANGE_CIPHER_SPEC:
            pctRet = Ssl3HandleCCS(
                                    pContext,
                                    MsgInput.pvBuffer,
                                    MsgInput.cbData,
                                    FALSE);
            // State is set within the function call.

            break;
        case (SSL3_HS_FINISHED << 16) | SSL3_STATE_RESTART_SERVER_FINISH:
            pctRet = Ssl3HandleServerFinish(
                                            pContext,
                                            MsgInput.pvBuffer,
                                            MsgInput.cbData,
                                            pCommOutput);

            if(pctRet == PCT_ERR_OK)
            {
                DWORD cbOutMessage;
                cbOutMessage =  CB_SLL3_CHANGE_CIPHER_SPEC +   //ChangeCipherSpec.
                                CB_SSL3_FINISHED_MSG +
                                pContext->pCheck->cbCheckSum ; //Finished Message;

                pCommOutput->cbData = cbOutMessage;
                /* are we allocating our own memory? */
                if(pCommOutput->pvBuffer == NULL)
                {
                    pCommOutput->pvBuffer = SPExternalAlloc(pCommOutput->cbData);
                    if (NULL == pCommOutput->pvBuffer)
                    {
                        SP_RETURN(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));
                    }
                    pCommOutput->cbBuffer = pCommOutput->cbData;
                }


                if(pCommOutput->cbData > pCommOutput->cbBuffer)
                {
                    SP_RETURN(PCT_INT_BUFF_TOO_SMALL);
                }
                pctRet = BuildCCSAndFinishMessage(pContext,
                                                    pCommOutput->pvBuffer,
                                                    cbOutMessage,
                                                    TRUE);
                pContext->State = SP_STATE_CONNECTED;
                pContext->DecryptHandler = Ssl3DecryptHandler;
                pContext->Encrypt = Ssl3EncryptMessage;
                pContext->Decrypt = Ssl3DecryptMessage;
            }
            break;

        case (SSL3_HS_FINISHED << 16) | SSL3_STATE_SERVER_FINISH:
            pctRet = Ssl3HandleServerFinish(
                                            pContext,
                                            MsgInput.pvBuffer,
                                            MsgInput.cbData,
                                            pCommOutput);
            if (PCT_ERR_OK == pctRet)
            {
                // pContext->State = SSL3_STATE_CLIENT_HELLO;
                pContext->State = SP_STATE_CONNECTED;
                pContext->DecryptHandler = Ssl3DecryptHandler;
                pContext->Encrypt = Ssl3EncryptMessage;
                pContext->Decrypt = Ssl3DecryptMessage;
            }
            else if (PCT_ERR_RENEGOTIATE == pctRet)
            {
        	    if(SPCacheClone(&pContext->RipeZombie))
            	{
                    pctRet =  GenerateSsl3ClientHello(pContext, pCommOutput);
                    pContext->State = SSL3_STATE_CLIENT_HELLO;
         	
            	}
            }
            break;

        default:
            DebugLog((DEB_WARN, "Error in protocol, dwStateTransition is %lx\n", dwStateTransition));
            pContext->State = PCT1_STATE_ERROR;
            if (cMessageType == SSL2_MT_ERROR)
            {
                // BUGBUG: handle an SSL2 error message
            }
            break;
    }


    if (pctRet & PCT_INT_DROP_CONNECTION)
    {
        pContext->State &= ~ SP_STATE_CONNECTED;
    }
    // To handle incomplete message errors:
    return(pctRet);
}

SP_STATUS
Ssl3HandleServerFinish(
    PSPContext  pContext,
    PUCHAR      pb,
    DWORD       cbMessage,
    PSPBuffer   pCommOutput
    )
{
    SP_STATUS          pctRet;
    pctRet = VerifyFinishMsg(pContext, pb, cbMessage, FALSE);

#ifdef SCHANNEL_EXPORT
    if (pctRet == PCT_ERR_OK && pContext->fSGC && !pContext->RipeZombie->fSGC)
    {   // set the SGC flag and Renegotiate
        pContext->RipeZombie->fSGC = pContext->fSGC;
        return(PCT_ERR_RENEGOTIATE);
    }
#endif
        
    /* Cache Session Here */
    if(pctRet == PCT_ERR_OK)
    {
        /* set up the session in cache */
        SPCacheAdd(pContext->RipeZombie);
    }
    return(pctRet);
}



SP_STATUS Ssl3CliHandleServerHello(
    PSPContext  pContext,
    PUCHAR pSrvHello,
    DWORD cbMessage,
    PSPBuffer  pCommOutput
    )
{
    /* error to return to peer */
    SP_STATUS                   pctRet=PCT_ERR_ILLEGAL_MESSAGE;
    Ssl2_Client_Master_Key      Key;
    UCHAR                       rgbToEncrypt[CB_SSL3_PRE_MASTER_SECRET];
    DWORD                       dwKeyLen, dwSrvHello;
    SWRAP *                     pswrap = (SWRAP *)pSrvHello;
    SSH *                       pssh = (SSH *) (pSrvHello + sizeof(SWRAP));




    pCommOutput->cbData = 0;
    dwSrvHello = COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize);


    // Pad the random structure with zero's if the challenge is
    // smaller than the normal SSL3 size (SSL2 v3 hello, Unihello, PCT1 wierdness if
    // we add it)
    FillMemory(pContext->rgbS3CRandom, CB_SSL3_RANDOM - pContext->cbChallenge, 0);

    CopyMemory(  pContext->rgbS3CRandom + CB_SSL3_RANDOM - pContext->cbChallenge,
                 pContext->pChallenge,
                 pContext->cbChallenge);



    do
    {
        DWORD   PrevHs = 0;
        BOOL    fLoopDone = FALSE;

        SHSH *pshsh;
        CERT *pcert;
        DWORD dwcb;
        DWORD cbCert;
        SHORT wCipher, wCompression;
        DWORD  ichHandshakeMac = 0;
        UCHAR   rgchHandshakeTypes[iHandshakeMax] ;
        SHSH    *prgpshsh [iHandshakeMax];
        WORD    iHandshake = 0;
        WORD    iHS = 0;
        PctPublicKey    *pCreatedKey = NULL;
        BOOL        fServerKeyExchange = FALSE;
        BOOL    fRestartServer = FALSE;
        BOOL 	fClientAuth = FALSE;
        DWORD dwCertClient;
        PctPrivateKey *pClientAuthKey = NULL;
        PctCertificate *pClientCert= NULL;
        PSessCacheItem      pZombie;
        const KeyExchangeSystem *KeyExchSys;
        const CheckSumFunction *Check;
        //UNDONE: Handling seperate Wrappers for Handshake messages....
        //UNDONE: Split into four Cases with ServerHello/Cert/ServerKeyExchange/Server_done

        //Initialize with the individual Message pointers and their types...

        if(!FCompleteMessageSrvHello(pContext,
                                     pSrvHello,
                                     cbMessage,
					                 &ichHandshakeMac,
                                     (PUCHAR)&rgchHandshakeTypes,
                                     prgpshsh,
					                &fRestartServer))
        {
	        return(PCT_INT_INCOMPLETE_MSG);
        }


        pZombie = pContext->RipeZombie;


        if(0 == ichHandshakeMac)
        {
            pctRet=SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
		    break;
        }

        //We need to check whether we have the appropriate CRED before we process
        // other messages....Client Auth..

        pctRet = PCT_ERR_OK;
        for( iHS = 0; iHS < ichHandshakeMac; iHS++)
        {
            if(rgchHandshakeTypes[iHS] == SSL3_HS_CERTIFICATE_REQUEST)
	        {
	            pshsh = prgpshsh[iHS];
	            dwcb = COMBINEBYTES(pshsh->bcbMSB, pshsh->bcbLSB);

	            pctRet = ParseCertificateRequest(pContext,
                                                (PBYTE)pshsh,
                                                dwcb,
                                                &pClientCert,
                                                &dwCertClient,
                                                &pClientAuthKey
                                                );
	            if(PCT_ERR_OK != pctRet)
                {
		            break;
                }
	            fClientAuth = TRUE;
	        }
        }
        if(pctRet != PCT_ERR_OK)
        {
            break;
        }

        iHandshake = 0;
        PrevHs = 0;
        fLoopDone = FALSE;
        pctRet = PCT_ERR_OK;
        do
        {
            if(iHandshake > ichHandshakeMac)
            {
                pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
                break;
            }
            pshsh = prgpshsh[iHandshake];
            dwcb = COMBINEBYTES(pshsh->bcbMSB, pshsh->bcbLSB);

            switch(( PrevHs << 8 ) | rgchHandshakeTypes[iHandshake])
            {
                case SSL3_HS_SERVER_HELLO:
                {
                    // Process Server Hello Message
                    pssh = (SSH *)prgpshsh[iHandshake];
                    if(pssh->cbSessionId < 0 && pssh->cbSessionId > CB_SSL3_SESSION_ID)
                    {
                        pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
		                break;
                    }
                    if(COMBINEBYTES(pssh->bMajor, pssh->bMinor)  < 0x300)
                    {
                        pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
		                break;
                    }
                    CopyMemory(pContext->rgbS3SRandom, pssh->rgbRandom, CB_SSL3_RANDOM);
                    wCipher = (SHORT)COMBINEBYTES(pssh->rgbSessionId[pssh->cbSessionId],
                                           pssh->rgbSessionId[pssh->cbSessionId+1]);
                    wCompression = pssh->rgbSessionId[pssh->cbSessionId+2];



                    if( wCompression != 0)
                    {
                        pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
                        break; //Non - NULL compression method..
                    }
                    if(pssh->cbSessionId && !fRestartServer)
                    {
                        CopyMemory(pZombie->SessionID, PbSessionid(pssh), pssh->cbSessionId);
                        pZombie->cbSessionID = pssh->cbSessionId;
                    }

                    Ssl3SelectCipher(pContext, wCipher);
                    pContext->wS3pendingCipherSuite = wCipher ;
                    if(fRestartServer)
                    {
                        fLoopDone = TRUE;
                    }
                    else
                    {
                        pContext->RipeZombie->fProtocol = SP_PROT_SSL3_CLIENT;
                    }
                    break;

                }
                case SSL3_HS_CERTIFICATE | (SSL3_HS_SERVER_HELLO<<8):
                {
                    DWORD dwSGC = 0;
                    // Process Certificate Message
                    pcert = (CERT *)prgpshsh[iHandshake];
                    if(pcert->bcbCert24 != 0x00)
                    {
                        pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
                        break; //This is NOT YET implemented..
                    }

                    cbCert = COMBINEBYTES(pcert->bcbMSBClist, pcert->bcbLSBClist);

                    pctRet = SPLoadCertificate(SP_PROT_SSL3_CLIENT,
                                     SP_CERT_X509,
                                     (PUCHAR)&pcert->bcbCert24,
                                     cbCert,
                                     &pContext->RipeZombie->pServerCert,
                                     &dwSGC);
                    if (dwSGC)
                    {   // the server supports SGC.
                        //This will trigger a renegotiate on non-US systems
                        pContext->fSGC = TRUE;
                    }

                    break;
                }
                case SSL3_HS_SERVER_KEY_EXCHANGE | (SSL3_HS_CERTIFICATE<<8):
                {
                    LPBSAFE_PUB_KEY pk = (LPBSAFE_PUB_KEY)(pZombie->pServerCert->pPublicKey->pKey);

                    if(!FSsl3ParseServerKeyExchange(pContext, (
                                                    PBYTE)pshsh + sizeof(SHSH),
                                                    dwcb,
								                    pk,
                                                    &pCreatedKey))
                    {
                        pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
                        break;
                    }
                    fServerKeyExchange = TRUE;
                    break;
                }
                case SSL3_HS_CERTIFICATE_REQUEST | (SSL3_HS_SERVER_KEY_EXCHANGE<<8):
                case SSL3_HS_CERTIFICATE_REQUEST | (SSL3_HS_CERTIFICATE<<8):
                {

                    // We already dealt with this message above when we were checking
                    // for client auth.

                    break;
                }

                case SSL3_HS_SERVER_HELLO_DONE | (SSL3_HS_SERVER_KEY_EXCHANGE<<8):
                case SSL3_HS_SERVER_HELLO_DONE | (SSL3_HS_CERTIFICATE<<8):
                case SSL3_HS_SERVER_HELLO_DONE | (SSL3_HS_CERTIFICATE_REQUEST<<8):
                {
                    fLoopDone = TRUE;
                    break;
                }

                default:
                {
                    pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
                    // We received an error
                    break;
                }
            }
            if(PCT_ERR_OK != pctRet)
            {
                break;
            }
            PrevHs = rgchHandshakeTypes[iHandshake];
            UpdateHandshakeHash(pContext, (LPBYTE)pshsh, dwcb + sizeof(SHSH), FALSE);
            // Each message, we increment our read counter
            // pContext->ReadCounter++;
            iHandshake++;
        } while(!fLoopDone);

        if(PCT_ERR_OK != pctRet)
        {
            break;
        }
        if(fRestartServer)
        {
            Ssl3MakeMasterKeyBlock(pContext);
            pContext->State = SSL3_STATE_RESTART_CCS;

            return PCT_ERR_OK;
        }



        //-----------------------PREPARE to return with information
        //Generate Pre_master_secret..
        //Encrypt Pre_master_secret with Server's Public Key
        //Add ChangeCipher message and finish message

        rgbToEncrypt[0] = (UCHAR)SSL3_CLIENT_VERSION_MSB;       //MajorVesion
        rgbToEncrypt[1] = (UCHAR)SSL3_CLIENT_VERSION_LSB;       //Minor version
        //2 : 2 bytes of version #
        GenerateRandomBits((PUCHAR)&(rgbToEncrypt[2]), CB_SSL3_PRE_MASTER_SECRET - 2);
        //Build Master Secret
        Ssl3BuildMasterKeys(pContext, (PUCHAR) rgbToEncrypt);
        Ssl3MakeMasterKeyBlock(pContext);

        dwKeyLen = ((pContext->PendingCiphSpec & SP_CIPHER_STRENGTH) >> SP_CSTR_POS) / 8;
        if (dwKeyLen < SSL3_MASTER_KEY_SIZE)
        {
            pZombie->cbClearKey = SSL3_MASTER_KEY_SIZE - dwKeyLen;
            /* The clear key is just the first portion of the Master Key */
            CopyMemory(pZombie->pClearKey, pZombie->pMasterKey, pZombie->cbClearKey);
        }
        else
        {
            pZombie->cbClearKey = 0;
        }
        if (pZombie->cbClearKey)
        {
            CopyMemory(Key.ClearKey, pZombie->pMasterKey, pZombie->cbClearKey);
        }
        Key.ClearKeyLen = pZombie->cbClearKey;
        Key.EncryptedKeyLen = SSL3_ENCRYPTED_KEY_SIZE;
        //NOTE: we always encrypt the pCreatedKey, so if no serverexchange, put the real one
        if(!fServerKeyExchange)
        {
            pCreatedKey = pZombie->pServerCert->pPublicKey;
        }

        KeyExchSys = KeyExchangeFromSpec(pContext->PendingExchSpec, SP_PROT_SSL3_CLIENT);
        if(KeyExchSys == NULL)
        {
            pctRet = SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
            break;
        }
        if(!KeyExchSys->Encrypt(
            (PUCHAR)rgbToEncrypt,
            CB_SSL3_PRE_MASTER_SECRET,
            Key.EncryptedKey,
            &Key.EncryptedKeyLen,
            pCreatedKey, 0))
        {
            pctRet = SP_LOG_RESULT(PCT_ERR_BAD_CERTIFICATE);
            if(fServerKeyExchange)
            {
                SPExternalFree(pCreatedKey);
                pCreatedKey = NULL; //Do we really need to do this ???
            }
            break;
        }

		if(fServerKeyExchange)
        {
            SPExternalFree(pCreatedKey);
            pCreatedKey = NULL; //Do we really need to do this ???
        }

        Key.KeyArgLen = 0;

        //Now create message to send...
        {
            DWORD                   cbMessage;
            PUCHAR                  pb;
            PUCHAR pbHash;
            DWORD 	dwClientAuth = 0;
            DWORD   dwHash = 0;

            if(fClientAuth)
            {

                if(pClientCert == NULL)
                {
                    dwClientAuth = SSL3_ALERT_NO_CERTIFICATE;
                }
                else
                {
                    pctRet  = SPSerializeCertificate(SP_PROT_SSL3_CLIENT,
                                                     NULL,
                                                     &dwCertClient,
                                                     pClientCert);
                    if(pctRet != PCT_ERR_OK)
                    {
                        return pctRet;
                    }
                    if(dwCertClient > 0x3fff) //Seperate Wrappers after this
                    {   		               // is a BIG UNDONE...
                        pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
                        break;
                    }
                    dwClientAuth = sizeof(SHSH) +              // Certificate Message
                                   CB_SSL3_CERT_VECTOR +
                                   dwCertClient +
                                   CB_SSL3_MAX_CERT_VERIFY;
                }

            }
            Check = HashFromSpec(pContext->PendingHashSpec, SP_PROT_SSL3_CLIENT);
            if (Check == NULL)
            {
                break;
            }
            cbMessage = sizeof(SWRAP) +
                        dwClientAuth +
                        sizeof(SHSH) +                  // Client Key Exchange
                        Key.EncryptedKeyLen +
                        CB_SLL3_CHANGE_CIPHER_SPEC +    //ChangeCipherSpec.
                        CB_SSL3_FINISHED_MSG +          //Finished Message;
                        Check->cbCheckSum ;

            cbMessage += FSsl3Cipher(TRUE) ? ( 2 * pContext->pCheck->cbCheckSum) : 0;
            pCommOutput->cbData = cbMessage;

            /* are we allocating our own memory? */
            if(pCommOutput->pvBuffer == NULL)
            {
                pCommOutput->pvBuffer = SPExternalAlloc(pCommOutput->cbData);
                if (NULL == pCommOutput->pvBuffer)
                {
                    SP_RETURN(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));
                }
                pCommOutput->cbBuffer = pCommOutput->cbData;
            }


            if(pCommOutput->cbData > pCommOutput->cbBuffer)
            {
                SP_RETURN(PCT_INT_BUFF_TOO_SMALL);
            }

            pbHash = pb = (PUCHAR)pCommOutput->pvBuffer + sizeof(SWRAP);

            if(fClientAuth)
            {
                if(pClientCert != NULL)
                {
                    CERT *pcert = (CERT *) pb ;
                    PUCHAR pbCert = &pcert->bcbCert24;
                    //Handle CertChain....

                    pctRet = SPSerializeCertificate(SP_PROT_SSL3_CLIENT,
                                                    &pbCert,
                                                    &dwCertClient,
                                                    pClientCert);
                    if(pctRet != PCT_ERR_OK)
                    {
                        break;
                    }
                    pcert->bcbClist24 = MS24BOF(dwCertClient);
                    pcert->bcbMSBClist = MSBOF(dwCertClient);
                    pcert->bcbLSBClist = LSBOF(dwCertClient);

                    SetHandshake(pb,  SSL3_HS_CERTIFICATE, NULL,  (WORD)(dwCertClient + CB_SSL3_CERT_VECTOR));

                    pb += sizeof(SHSH) + CB_SSL3_CERT_VECTOR + dwCertClient;
                    dwHash += sizeof(SHSH) + CB_SSL3_CERT_VECTOR + dwCertClient;
                }
                else
                {
                    BuildAlertMessage(pb, SSL3_ALERT_WARNING, SSL3_ALERT_NO_CERTIFICATE);
                    pb += SSL3_ALERT_NO_CERTIFICATE;
                    dwHash += SSL3_ALERT_NO_CERTIFICATE;
               }
            }

            // Add the KeyExchange message;

            SetHandshake(pb,
                         SSL3_HS_CLIENT_KEY_EXCHANGE,
                         (PUCHAR)Key.EncryptedKey,
                         (WORD) Key.EncryptedKeyLen);

            pb += sizeof(SHSH) + Key.EncryptedKeyLen;
            dwHash += sizeof(SHSH) + Key.EncryptedKeyLen;

            // Update the handhsake hash for the CertVerify message
            UpdateHandshakeHash(pContext,
                            (PUCHAR)pbHash,
                            (UINT)dwHash, FALSE);
            if(fClientAuth && (pClientCert != NULL))
            {
                DWORD dwCertVerifyMsgLen = 0;

                pctRet = BuildCertVerify(   pClientAuthKey,
                                            pClientCert,
                                            pContext, pb,
                                            &dwCertVerifyMsgLen);
                if(pctRet != PCT_ERR_OK)
                {
                    break;
                }
                UpdateHandshakeHash(pContext,  (PUCHAR)pb, (UINT)dwCertVerifyMsgLen, FALSE);
                pCommOutput->cbData -= CB_SSL3_MAX_CERT_VERIFY - dwCertVerifyMsgLen; //Extra allocation
                pb += dwCertVerifyMsgLen;
                dwHash += dwCertVerifyMsgLen;

          }


            // Encrypt the whole message if necessary.
            SetWrap(pContext, (PUCHAR)pCommOutput->pvBuffer, SSL3_CT_HANDSHAKE, dwHash, TRUE);
            if(FSsl3Cipher(TRUE))
            {
                pb += pContext->pCheck->cbCheckSum;
            }
            pctRet = BuildCCSAndFinishMessage(pContext,
                                    pb,
                                    CB_SLL3_CHANGE_CIPHER_SPEC +
                                    CB_SSL3_FINISHED_MSG +
                                    Check->cbCheckSum,
                                    TRUE);

            if(pctRet != PCT_ERR_OK)
            {
                break;
            }
            pContext->State = SSL3_STATE_CHANGE_CIPHER_SPEC;


		}
    	return(PCT_ERR_OK);
    } while(TRUE);

    if(pctRet == PCT_ERR_INCOMPLETE_CREDS)
    {
        return(pctRet); //DON'T Drop connection, it's a WARNING...
    }
    return pctRet | PCT_INT_DROP_CONNECTION;
}



DWORD IMacParseSrvHello
(
PUCHAR pbSrvHello,
DWORD cbMsg,
PUCHAR prgchHandshake,
SHSH *prgpshsh[]
)
	{
	DWORD dwcb;
	SHSH *pshsh = (SHSH *)pbSrvHello;
	DWORD iHandshake = 0;
	PUCHAR pbMax = pbSrvHello + cbMsg;

	do
		{
		if((PUCHAR)pshsh > pbMax)
			break;
		prgchHandshake[iHandshake] = pshsh->typHS;
		prgpshsh[iHandshake++] = pshsh;
	    dwcb = COMBINEBYTES(pshsh->bcbMSB, pshsh->bcbLSB);
	    cbMsg -= dwcb + sizeof(SHSH);
	    pshsh = (SHSH *) ((PUCHAR) pshsh + dwcb + sizeof(SHSH));
	    } while(cbMsg > 0);
	return(iHandshake);
	}





/*****************************************************************************/
static BOOL FSsl3ParseServerKeyExchange
(
PSPContext  pContext,
PBYTE pbMessage,                                //in
DWORD iMessageLen,                              //in
LPBSAFE_PUB_KEY lpServerPublicKey,      //in
PctPublicKey   **ppctKey                //out
)
{
    PBYTE pbMessageStart = pbMessage;
    PBYTE pbModulus = NULL;
    INT   iModulusLen;
    PBYTE pbExponent = NULL;
    INT   iExponentLen;
    BYTE  pbInputBlock[SSL3_ENCRYPTED_KEY_SIZE];
    BYTE  pbOutputBlock[SSL3_ENCRYPTED_KEY_SIZE];
    INT   iBlockLen;
    PBYTE pbServerParams = NULL;
    INT   iServerParamsLen;
    BYTE  pbMd5HashValue[16];
    BYTE  pbShaHashValue[20];
    INT   iPadLen;
    INT   i;
    //
    // struct {
    //   select (KeyExchangeAlgorithm) {
    //       case diffie_hellman:
    //             ServerDHParams params;
    //             Signature signed_params;
    //       case rsa:
    //             ServerRSAParams params;
    //             Signature signed_params;
    //       case fortezza_dms:
    //             ServerFortezzaParams params;
    //   };
    // } ServerKeyExchange;
    //


    //
    // struct {
    //   opaque rsa_modulus<1..2^16-1>;
    //   opaque rsa_exponent<1..2^16-1>;
    // } ServerRSAParams;
    //

    // Mark start of ServerRSAParams structure.
    // This is used to build hash values.
    pbServerParams = pbMessage;

    // Modulus length
    iModulusLen = ((INT)pbMessage[0] << 8) + pbMessage[1];
    if(iModulusLen < 512/8 || iModulusLen > 2048/8)
		return FALSE;
    pbMessage += 2;

    // Since the modulus is encoded as an INTEGER, it is padded with a leading
    // zero if its most significant bit is one. Remove this padding, if
    // present.
    if(pbMessage[0] == 0)
	{
		iModulusLen -= 1;
		pbMessage   += 1;
	}

    // Modulus
    pbModulus = pbMessage;
    pbMessage += iModulusLen;


    // Exponent length
    iExponentLen = ((INT)pbMessage[0] << 8) + pbMessage[1];
    if(iExponentLen < 1 || iExponentLen > 4)
		return FALSE;
    pbMessage += 2;

    // Exponent
    pbExponent = pbMessage;
    pbMessage += iExponentLen;


    // Compute length of ServerRSAParams structure.
    iServerParamsLen = pbMessage - pbServerParams;

    // build BSAFE_PUB_KEY structure from modulus and exponent
    if(!FImportServerPublicKeyBlob(
	    pbModulus,              // in
	    iModulusLen,            // in
	    pbExponent,             // in
	    iExponentLen,           // in
	    ppctKey))       // out
			return FALSE;

    //
    // digitally-signed struct {
    //   select(SignatureAlgorithm) {
    //        case anonymous: struct { };
    //        case rsa:
    //             opaque md5_hash[16];
    //             opaque sha_hash[20];
    //        case dsa:
    //             opaque sha_hash[20];
    //   };
    // } Signature;
    //

    // Signature block length
    iBlockLen = ((INT)pbMessage[0] << 8) + pbMessage[1];
    pbMessage += 2;

    // Signature block. Convert to little endian with at least 8 bytes of
    // zero padding at the end.
    FillMemory(pbInputBlock, SSL3_ENCRYPTED_KEY_SIZE, 0);
    ReverseMemCopy(pbInputBlock, pbMessage, iBlockLen);
    pbMessage += iBlockLen;
    // Zero destination buffer
    FillMemory(pbOutputBlock, SSL3_ENCRYPTED_KEY_SIZE, 0);

    // Decrypt signature block, using public key contained in certificate
    if(BSafeEncPublic(lpServerPublicKey, pbInputBlock, pbOutputBlock) == FALSE)
		return FALSE;

    // Convert decrypted signature block to little endian.
	ReverseMemCopy(pbInputBlock, pbOutputBlock, iBlockLen);

    // Verify signature block format
    iPadLen = iBlockLen - MD5DIGESTLEN - A_SHA_DIGEST_LEN - 3;
    if(pbInputBlock[0] != 0 || pbInputBlock[1] != 1 ||
       pbInputBlock[2 + iPadLen] != 0)
		return FALSE;
    for(i=0 ; i<iPadLen ; i++)
	{
		if(pbInputBlock[i + 2] != 0xff)
		    return FALSE;
	    }

    // Generate hash values
    ComputeServerExchangeHashes(pContext, pbServerParams,
				iServerParamsLen,
				pbMd5HashValue,
				pbShaHashValue);

    // Verify hash values
    if(memcmp(pbMd5HashValue, pbInputBlock + iPadLen + 3, MD5DIGESTLEN) ||
       memcmp(pbShaHashValue, pbInputBlock + iPadLen + 3 + MD5DIGESTLEN, A_SHA_DIGEST_LEN))
		return FALSE;

    // Validate message length
    if(iMessageLen != (DWORD) (pbMessage - pbMessageStart))
		return FALSE;

    return TRUE;
}



//
// Import public key from modulus and exponent
//
/******************************************************************************
* ImportServerPublicKeyBlob
*
* Build BSAFE public key structure from modulus and exponent. Caller is
* responsible for freeing this structure using LocalFree.
*/
BOOL FImportServerPublicKeyBlob(
    PBYTE    pbMod,                 // in
    INT      iModLen,               // in
    PBYTE    pbExp,                 // in
    INT      iKeyExpLen,            // in
    PctPublicKey   **ppKey)  //out
{
    DWORD Aligned, dwKeySize;
    INT iCnt;
    PctPublicKey    *pCreatedKey = NULL;
    BSAFE_PUB_KEY * pPublicKey;


    // Allocate DWORD aligned memory for public key
    // Correction: Allocate QUADWORD aligned memory for public key    
    Aligned = ((iModLen + sizeof(DWORD)*2 -1) & ~(sizeof(DWORD)*2-1));
    
    /* rsa code requires two extra null DWORDS padded on the end of the aligned key */
    Aligned += sizeof(DWORD)*2;
    dwKeySize = (DWORD)((PctPublicKey *)NULL)->pKey + sizeof(BSAFE_PUB_KEY) + Aligned;
    pCreatedKey = (PctPublicKey *)SPExternalAlloc(dwKeySize);
    pPublicKey = (BSAFE_PUB_KEY *)pCreatedKey->pKey;

    pPublicKey->magic   = RSA1;
    pPublicKey->keylen  = Aligned;
    pPublicKey->bitlen  = iModLen * 8;

    // max number of bytes to be encoded
    pPublicKey->datalen = (pPublicKey->bitlen / 8) - 1;

    // copy modulus; convert to big endian format
    ReverseMemCopy((BYTE *)(pPublicKey + 1), pbMod, iModLen);

    // form a (little endian) DWORD from *pbExp data
    pPublicKey->pubexp =  0;
    for(iCnt=0; iCnt<iKeyExpLen; iCnt++)
	{
		pPublicKey->pubexp <<= 8;
		pPublicKey->pubexp |=  pbExp[iCnt];
	    }
    pCreatedKey->cbKey = ( dwKeySize );
    *ppKey = pCreatedKey;
    return TRUE;
}







SP_STATUS
Ssl3PackClientHello(
    PSPContext              pContext,
    PSsl2_Client_Hello       pCanonical,
    PSPBuffer          pCommOutput)
{
//    DWORD               cbMessage;
    CLH                                 *pclh;
    DWORD               dwSize;
    PUCHAR              pbBuffer;
    DWORD               i, dwFixed;
    DWORD                               dwCipherSize = pCanonical->cCipherSpecs * sizeof(short);
//    short                               *pw;



    if(pCanonical == NULL || pCommOutput == NULL)
	return PCT_INT_INTERNAL_ERROR;

	//Calculate the size...
    dwSize = pCommOutput->cbData = ( dwFixed = sizeof(SWRAP) +
							 sizeof(SHSH) + CB_SSL3_RANDOM + 3) +
					 pCanonical->cbSessionID + sizeof(short) +
					pCanonical->cCipherSpecs * sizeof(short) +
					  2 ; //Size of Compression Alogorithm 1 + null (0)

    pCommOutput->cbData += FSsl3Cipher(TRUE) ? pContext->pCheck->cbCheckSum : 0;
    /* are we allocating our own memory? */
    if(pCommOutput->pvBuffer == NULL)
    {
        pCommOutput->pvBuffer = SPExternalAlloc(pCommOutput->cbData);
        if (NULL == pCommOutput->pvBuffer)
        {
            SP_RETURN(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));
        }
        pCommOutput->cbBuffer = pCommOutput->cbData;
    }


    if(pCommOutput->cbData > pCommOutput->cbBuffer)
    {
        SP_RETURN(PCT_INT_BUFF_TOO_SMALL);
    }

	//Initialize the member varaibles
    pclh = (CLH *)  ((BYTE *)pCommOutput->pvBuffer + sizeof(SWRAP));
	pclh->bMajor = SSL3_CLIENT_VERSION_MSB;
	pclh->bMinor = SSL3_CLIENT_VERSION_LSB;
	CopyMemory(pclh->rgbRandom, pCanonical->Challenge, CB_SSL3_RANDOM);
	pclh->cbSessionId = (UCHAR) pCanonical->cbSessionID;
	CopyMemory(pclh->rgbSessionId, pCanonical->SessionID, pCanonical->cbSessionID);

	pbBuffer = (BYTE *)     (BYTE *)pCommOutput->pvBuffer + dwFixed + pCanonical->cbSessionID;
	*pbBuffer++ =   MSBOF(dwCipherSize);
	*pbBuffer++ =   LSBOF(dwCipherSize);

	for(i = 0 ; i < pCanonical->cCipherSpecs ; i++)
		{
			*pbBuffer++ =   MSBOF(pCanonical->CipherSpecs[i]);
			*pbBuffer++ =   LSBOF(pCanonical->CipherSpecs[i]);
		}
	*pbBuffer++ = 0x1 ; //One compression Method;
	*pbBuffer = 0x00 ; //NULL Compression.
	SetHandshake((BYTE *)&pclh->typHS,  SSL3_HS_CLIENT_HELLO,  NULL,
				(WORD) (dwSize - sizeof(SWRAP) - sizeof(SHSH)));
	    // In case of SSL3, do the handshare hash.
	UpdateHandshakeHash(pContext, (PUCHAR)pCommOutput->pvBuffer + sizeof(SWRAP),
											dwSize - sizeof(SWRAP), TRUE);

	SetWrap(pContext, pCommOutput->pvBuffer,  SSL3_CT_HANDSHAKE, dwSize - sizeof(SWRAP), TRUE);

    return(PCT_ERR_OK);
}

SP_STATUS WINAPI
GenerateSsl3ClientHello(
    PSPContext              pContext,
    PSPBuffer               pOutput)
{
    Ssl2_Client_Hello    HelloMessage;
    SP_STATUS pctRet = PCT_INT_INTERNAL_ERROR;

    SP_BEGIN("GenerateSsl3ClientHello");


    GenerateRandomBits( pContext->pChallenge, CB_SSL3_RANDOM );
    pContext->cbChallenge = CB_SSL3_RANDOM;



    pctRet = GenerateUniHelloMessage(pContext, &HelloMessage, SP_PROT_SSL3_CLIENT);

    if(PCT_ERR_OK != pctRet)
    {
        SP_RETURN(pctRet);
    }


    if(PCT_ERR_OK != (pctRet = Ssl3PackClientHello(pContext, &HelloMessage,  pOutput)))
    {
        SP_RETURN(pctRet);
    }



    SP_RETURN(PCT_ERR_OK);
}


// Do we have all the information to process Server Hello message.
// WE could get 2 types of messages
//   1. Restart enabled.
//       2. No restart, serverhello done.
//
//   case 1: In the first case we get ServerHello with matching sessionid followed by
//                       Change_cipher_spec message and Finished message
//
//   case 2: SeverHello Message, Certificate*, CertificateRequest*, ServerKeyExchange
//                       followed by Server_hello_done.
//
//
//      So this routine checks whether  we expect Restart and if yes, looks for Finished message
//       if it doesn't expect Restart message, then it looks for server hello done.
//
//      RETURNS TRUE if Finished message or server hello done message is present in the packet
//                      FALSE otherwise.
//
//
//
//              In case of REDO, we need to DECRYPT and look inside....( probably we should use
//                              temp key state to peek for completeness.

BOOL FCompleteMessageSrvHello(
    PSPContext  pContext,
    PBYTE pSrvHello,
    DWORD cbMessage,
    DWORD *pichHandshakeMac,
    PUCHAR prgchHandshake,
    SHSH *prgpshsh[],
    BOOL *pfRestartServer
    )
{
    BOOL fExpectRestart = pContext->RipeZombie->ZombieJuju;
    SWRAP *pswrap = (SWRAP *)pSrvHello;
    DWORD cbMsg;
    DWORD cbRecord;
    DWORD iRecord = 0;


    if(cbMessage <= sizeof(SWRAP))
    {
        return(FALSE);
    }
    cbMsg = COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize);
    cbRecord = cbMsg + sizeof(SWRAP);

    //if total bytes in the packet is less than what the header says,return false
    if( cbMessage < cbRecord)
    {
        return(FALSE);
    }

    *pichHandshakeMac = 0;

    if(fExpectRestart)
    {       //Look for finished message in the packet...
        SSH *pssh;

        pssh = (SSH *) (pSrvHello + sizeof(SWRAP));
        if (!memcmp(pContext->RipeZombie->SessionID, PbSessionid(pssh),
					        pssh->cbSessionId))
        {
            DWORD dwExpect = CB_SLL3_CHANGE_CIPHER_SPEC + CB_SSL3_FINISHED_MSG +
                            MD5DIGESTLEN ; //we need to change this to SHA iff
            *pfRestartServer = TRUE;
            if( (cbMessage - cbRecord) < dwExpect)
            {
	            return(FALSE);
            }
            *pichHandshakeMac = IMacParseSrvHello((PBYTE)pswrap + sizeof(SWRAP),
                                                    cbMsg,
                                                    prgchHandshake + iRecord,
                                                    prgpshsh + iRecord);
	        return(TRUE);
	    }
	    else
	    {
	        // the server refused a restart
	        // release the SessCacheItme and get a new one
	        SPCacheClone(&pContext->RipeZombie);
	    }
    }
    //              UNDONE: this wont work for Combined hasndshake....
    do
    {               //Look for Server hello done message
        DWORD ich;

        //Initialize with the individual Message pointers and their types...
        cbMsg = COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize);
        cbRecord = cbMsg + sizeof(SWRAP);
        //if total bytes in the packet is less than what the header says,return false
        if( cbMessage < cbRecord)
        {
	        return(FALSE);
        }

        *pichHandshakeMac += IMacParseSrvHello((PBYTE)pswrap + sizeof(SWRAP),
                                                cbMsg,
                                                prgchHandshake + iRecord,
                                                prgpshsh + iRecord);
        for( ich = 0 ; ich < *pichHandshakeMac ; ich++)
        {
            if(*(prgchHandshake + ich) == SSL3_HS_SERVER_HELLO_DONE)
            {
	            return(TRUE);
            }
        }
        pswrap = (SWRAP *) ((PBYTE)pswrap + cbRecord);
        cbMessage -= cbRecord;
        iRecord ++;
    } while(cbMessage);
    return(FALSE);

}






SP_STATUS
ParseCertificateRequest
(
PSPContext  pContext,
PBYTE pb,
DWORD dwcb,
PctCertificate **ppClientCert,
DWORD *pdwcbCert,
PctPrivateKey **ppClientAuthKey
)
	{
    SP_STATUS          pctRet=PCT_ERR_ILLEGAL_MESSAGE;
    UCHAR cbCertType;
    INT wCertType;
    DWORD dwcbIssuerName;
    DWORD dwcbIssuerList;
    PSPCredential      pCred;

    //
    // enum {
    //	   rsa_sign(1), dss_sign(2), rsa_fixed_dh(3), dss_fixed_dh(4),
    //	   rsa_ephemeral_dh(5), dss_ephemeral_dh(6), fortezza_dms(20), (255)
    // } ClientCertificateType;
    //
    // opaque DistinguishedName<1..2^16-1>;
    //
    // struct {
    //	   ClientCertificateType certificate_types<1..2^8-1>;
    //	   DistinguishedName certificate_authorities<3..2^16-1>;
    // } CertificateRequest;
    //

	pb += sizeof(SHSH);

	do
	{
	    // certificate type list length
	    cbCertType = pb[0];

	    pb += 1;

	    // certificate type list (only one entry allowed by test program)
	    wCertType = pb[0];
	    if(wCertType != SSL3_CERTTYPE_RSA_SIGN)   //we suppport only rsa-sign for now
	    {
            pctRet=SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
		    break;
	    }
	    pb += cbCertType;

	    // issuer list length
	    dwcbIssuerList = COMBINEBYTES(pb[0], pb[1]);
	    if(dwcbIssuerList > dwcb)
	    {
            pctRet=SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
		    break;
	    }

		pctRet = PatchNetscapeBugAndStore(pContext, pb);
		if(pctRet != PCT_ERR_OK)
        {
            pctRet=SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
		    break;
        }
	    pb = pContext->pbIssuerList + 2;

        *ppClientAuthKey = NULL;
        *ppClientCert = NULL;
        pCred = pContext->pCred;
        dwcbIssuerName = dwcbIssuerList;
        pctRet = PCT_ERR_OK;
        {
            // If we're doing client auth, check to see if we have
            // proper credentials.
            DWORD i, dwcbIss = dwcbIssuerList;
            BOOL fGot = FALSE;
            // Choose a certificate/private key pair for client auth.
            for(; dwcbIss;)
            {
                for(i=0; i<pCred->cCerts; i++)
                {
          	    	PctCertificate *pCertT = pCred->paCerts[i];
					PctCertificate *pCertTT;
					
            	    dwcbIssuerName = COMBINEBYTES(pb[0], pb[1]) + 2 ;
            	    if(dwcbIssuerName > dwcb)
            	    {
		                break;
            	    }
            	    do
            	    {

	                    if(memcmp(pCertT->pbIssuer, pb+2, pCertT->cbIssuer))
	                    {
	                        /* Set up the certificate */
	                        if(ReferenceCert(pCred->paCerts[i]) <= 0)
	                        {
	                            continue;
	                        }
	                        *ppClientCert = pCred->paCerts[i];
	                        *ppClientAuthKey = pCred->paPrivateKeys[i];
	                        fGot = TRUE;
	                        break;
	                    }
	                    pCertTT = pCertT;
	                    pCertT = pCertT->pIssuerCertificate;
                    } while(pCertT != NULL && pCertT != pCertTT);
                }
                if(dwcbIss < dwcbIssuerName) //Prevent Wrap around and Hang...
                    break;
                if(fGot)
                	break;
                dwcbIss -= dwcbIssuerName;
                pb += dwcbIssuerName;
            }
            if ((*ppClientAuthKey == NULL) ||
                (*ppClientCert == NULL))
            {
                if(!(pContext->Flags & CONTEXT_FLAG_NO_INCOMPLETE_CRED_MSG))
                {
	                pctRet = SP_LOG_RESULT(PCT_ERR_INCOMPLETE_CREDS);
                    SP_RETURN( pctRet);
                }
            }
        }

	    break;
	} while(TRUE);

	return (pctRet);
#ifdef VERISIGN
	*ppClientAuthKey = g_pbClientKey;
	*ppClientCert = g_pbClientCert;
	return(PCT_ERR_OK);
#endif
	}

SP_STATUS
BuildCertVerify
(
PctPrivateKey *pClientAuthKey,
PctCertificate *pClientCert,
PSPContext  pContext,
PBYTE pb,
DWORD *pdwcbCertVerify
)
	{
	SP_STATUS pctRet;
    BYTE  rgbSigned[SSL3_ENCRYPTED_KEY_SIZE];
    DWORD dwcbSigned = SSL3_ENCRYPTED_KEY_SIZE;
    PBYTE pbT = pb + sizeof(SHSH);
    BYTE  rgbHashValue[MD5DIGESTLEN+A_SHA_DIGEST_LEN];
    const SignatureSystem *pSigSys = NULL;


    //
    // digitally-signed struct {
    //	   opaque md5_hash[16];
    //	   opaque sha_hash[20];
    // } Signature;
    //
    // struct {
    //	   Signature signature;
    // } CertificateVerify;
    //
    // CertificateVerify.signature.md5_hash = MD5(master_secret + pad2 +
    //		MD5(handshake_messages + master_secret + pad1));
    //
    // CertificateVerify.signature.sha_hash = SHA(master_secret + pad2 +
    //		SHA(handshake_messages + master_secret + pad1));
    //

    // Generate hash values

    ComputeCertVerifyHashes(pContext, rgbHashValue);


    pSigSys = SigFromSpec(SP_SIG_RSA_SHAMD5, SP_PROT_SSL3_CLIENT);
    if (pSigSys == NULL)
    {
        pctRet = SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH);
        return pctRet;
   }

    if(!pSigSys->Sign(rgbHashValue,
                      MD5DIGESTLEN+A_SHA_DIGEST_LEN,
                      rgbSigned,
                      &dwcbSigned,
                      pClientAuthKey))
    {
        pctRet = SP_LOG_RESULT(PCT_ERR_BAD_CERTIFICATE);
        return pctRet;
    }

    pbT[0] = MSBOF(dwcbSigned);
    pbT[1] = LSBOF(dwcbSigned);
    *pdwcbCertVerify =  dwcbSigned + 2 + sizeof(SHSH);
    pbT += 2;
	SetHandshake(pb,  SSL3_HS_CERTIFICATE_VERIFY, NULL,  (WORD)(dwcbSigned + 2));
    CopyMemory(pbT, rgbSigned, dwcbSigned);
    return PCT_ERR_OK;
}

SP_STATUS
ParseHelloRequest
(
PSPContext  pContext,
PUCHAR pSrvHello,
DWORD cbMessage,
PSPBuffer  pCommOutput
)
    {
    return (GenerateSsl3ClientHello(pContext, pCommOutput));

    }





SP_STATUS
PatchNetscapeBugAndStore
(
PSPContext  pContext,
PBYTE pb
)
	{
	PBYTE pbAlloc, pbAllocT;
	DWORD dwcbToAllocate ;
	DWORD dwcb ;
	PBYTE pbTemp = pb;

	dwcb = dwcbToAllocate = COMBINEBYTES(pbTemp[0], pbTemp[1]); //IssuerListLength

	pbTemp += 2;
	while(dwcb)
	{
		DWORD dwT;

		dwT =  COMBINEBYTES(pbTemp[0], pbTemp[1]); //IssuerListLength
		if(pbTemp[2] != SEQUENCE_TAG) //if there is NO Sequence TAG
	 		dwcbToAllocate +=  CbLenOfEncode(dwT, NULL);
 		dwcb -= (2 + dwT);
 		pbTemp += (2 + dwT);
	}

	dwcbToAllocate += 2; //for the list header...
	pbAlloc = SPExternalAlloc(dwcbToAllocate);
	if(!pbAlloc)
		return(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));

	pbAllocT = pbAlloc;
	pbTemp = pb;
	dwcb = COMBINEBYTES(pbTemp[0], pbTemp[1]);
	pbAlloc[0] = MSBOF((dwcbToAllocate-2));
	pbAlloc[1] = LSBOF((dwcbToAllocate-2));
	pbTemp += 2;
	pbAlloc += 2;
	while(dwcb)
	{
		DWORD dwT;
		DWORD dwPad;

		dwT =  COMBINEBYTES(pbTemp[0], pbTemp[1]); //IssuerListLength

		dwPad = 0;
		if(pbTemp[2] != 0x30) //if there is NO Sequence TAG
	 		dwPad =  CbLenOfEncode(dwT, pbAlloc+2);
	 	pbAlloc[0] = MSBOF((dwT+dwPad));
	 	pbAlloc[1] = LSBOF((dwT+dwPad));
	 	pbAlloc += (dwPad + 2);
	 	CopyMemory(pbAlloc, pbTemp+2, dwT);
	 	pbAlloc += dwT;
 		dwcb -= (dwT+2);
 		pbTemp += (dwT+2);
	}

	if(pContext->pbIssuerList != NULL)
		{
	    SPExternalFree(pContext->pbIssuerList);
	    pContext->pbIssuerList = NULL;
		}
	pContext->cbIssuerList = dwcbToAllocate;
	pContext->pbIssuerList = SPExternalAlloc(dwcbToAllocate);
    if(!pContext->pbIssuerList)
    {
	    SPExternalFree(pbAllocT);
    	return SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
    }

    CopyMemory(pContext->pbIssuerList, pbAllocT, dwcbToAllocate);
    //FREE the extra allocation...
    SPExternalFree(pbAllocT);
	return(PCT_ERR_OK);
	}



DWORD  CbLenOfEncode(DWORD dw, PBYTE pbDst)
	{
	BYTE   lenbuf[8];
    DWORD   length = sizeof(lenbuf) - 1;
    LONG    lth;

	if (0x80 > dw)
    {
    	lenbuf[length] = (BYTE)dw;
        lth = 1;
    }
    else
    {
    	while (0 < dw)
	    {
		    lenbuf[length] = (BYTE)(dw & 0xff);
		    length -= 1;
		    dw = (dw >> 8) & 0x00ffffff;
	    }
	    lth = sizeof(lenbuf) - length;
	    lenbuf[length] = (BYTE)(0x80 | (lth - 1));
    }

	if(NULL != pbDst)
	{
		pbDst[0] = 0x30;
		memcpy(pbDst+1, &lenbuf[length], lth);

	}
 	return ++lth; //for 0x30
  	}









