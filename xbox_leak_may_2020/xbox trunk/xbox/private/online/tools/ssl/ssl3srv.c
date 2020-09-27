//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       ssl3srv.c
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    05-21-96   ramas   Created
//
//----------------------------------------------------------------------------

#include <spbase.h>
#include <ssl2msg.h>
#include <ssl3msg.h>
#include <ssl3key.h>
#include <md5.h>
#include <sha.h>
#include <rsa.h>
#include <ssl2prot.h>
#include <certmap.h>





BOOL
FSSL3(PBYTE pb, BYTE *pchCT);

static DWORD vcbIssuerList = 0x61;

static BYTE  pbPctIssuer[] = {
    0x30, 0x5f, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31,
    0x20, 0x30, 0x1e, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x17, 0x52, 0x53, 0x41, 0x20, 0x44, 0x61,
    0x74, 0x61, 0x20, 0x53, 0x65, 0x63, 0x75, 0x72, 0x69, 0x74, 0x79, 0x2c, 0x20, 0x49, 0x6e, 0x63,
    0x2e, 0x31, 0x2e, 0x30, 0x2c, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x13, 0x25, 0x53, 0x65, 0x63, 0x75,
    0x72, 0x65, 0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66,
    0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74,
    0x79
};


static BYTE  pbClass1Issuer[] =
{
0x30,0x5f,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x17,0x30,
0x15,0x06,0x03,0x55,0x04,0x0a,0x13,0x0e,0x56,0x65,0x72,0x69,0x53,0x69,0x67,0x6e,
0x2c,0x20,0x49,0x6e,0x63,0x2e,0x31,0x37,0x30,0x35,0x06,0x03,0x55,0x04,0x0b,0x13,
0x2e,0x43,0x6c,0x61,0x73,0x73,0x20,0x31,0x20,0x50,0x75,0x62,0x6c,0x69,0x63,0x20,
0x50,0x72,0x69,0x6d,0x61,0x72,0x79,0x20,0x43,0x65,0x72,0x74,0x69,0x66,0x69,0x63,
0x61,0x74,0x69,0x6f,0x6e,0x20,0x41,0x75,0x74,0x68,0x6f,0x72,0x69,0x74,0x79
};

BOOL GetServerIssuer(
    PBYTE    pbIssuer,      // out
    DWORD *  piIssuerLen);          // in, out


SP_STATUS
Ssl3HandleClientFinish(
    PSPContext  pContext,
    PUCHAR      pb,
    DWORD       cbMessage,
    PSPBuffer   pCommOutput
    );
SP_STATUS
Ssl3SrvHandleClientHello(PSPContext  pContext,
			 PSPBuffer  pCommInput,
			 PSPBuffer  pCommOutput);
SP_STATUS
Ssl3SrvHandleUniHello(PSPContext  pContext,
			 PSPBuffer  pCommInput,
			 PSsl2_Client_Hello pHello,
			 PSPBuffer  pCommOutput);

BOOL FGetServerIssuer(
    PBYTE pbIssuer,         // out
    DWORD *pdwIssuer);    // in, out

DWORD CbIssuerLists();

SP_STATUS
Ssl3SrvGenRestart(
    PSPContext         pContext,
    PSPBuffer          pCommOutput);

SP_STATUS
Ssl3SrvGenServerHello(
    PSPContext         pContext,
    PSPBuffer          pCommOutput);


BOOL FBuildSsl3ExchgKeys();
static
BOOL
Ssl3ParseCertificateVerify(PSPContext  pContext, PBYTE pbMessage, INT iMessageLen);

SP_STATUS
BuildHelloRequest
(
PSPContext  pContext,
PSPBuffer  pCommOutput
);

BOOL FInitCertSystem(PSPContext  pContext, BYTE **ppbCert, DWORD *pcbCert);

SP_STATUS EnoughDataCKeyExchg(PBYTE pCommInput, DWORD cbMsg);
SP_STATUS ParseKeyExchgMsg(PSPContext  pContext, PBYTE pCommInput, DWORD cbMsg, DWORD *pcbExchMsg);

static BOOL Ssl3BuildCertificateRequest(
    PSPContext  pContext,   // in
    PBYTE pbMessage,        // out
    DWORD cbIssuerLen,      //in
    DWORD   *pdwMessageLen);// out

void
Ssl3BuildServerHello(PSPContext pContext, PBYTE pb);

void BuildServerHelloDone(PBYTE pb, DWORD cb);

static BOOL Ssl3BuildServerKeyExchange(
    PSPContext  pContext,
    PBYTE pbMessage,                // out
    PINT  piMessageLen) ;           // out

SP_STATUS BuildCCSAndFinishForServer
(
PSPContext  pContext,
PSPBuffer  pCommOutput,
BOOL fDone
);


//--------------------------------------------------------------------------------------------
SP_STATUS WINAPI
Ssl3ServerProtocolHandler(
    PSPContext  pContext,
    PSPBuffer pCommInput,
    PSPBuffer pCommOutput,
    PSPBuffer pAppOutput)
{
    SP_STATUS pctRet = 0;
    DWORD cMessageType;
    SWRAP *pswrap;
    DWORD dwStateTransition;
    BOOL fRaw = TRUE;
    SPBuffer MsgInput;
    DWORD cbMsg;
    DWORD cbInputData = 0;
//    PUCHAR pb;
//    UCHAR bCT;
//    BOOL fSsl3;

    SP_BEGIN("Ssl3ServerProtocolHandler");
    if (NULL != pCommOutput)
    {
	pCommOutput->cbData = 0;
    }
    if (NULL != pAppOutput)
    {
	pAppOutput->cbData = 0;
    }


	do
	{
	MsgInput.pvBuffer = (PUCHAR) pCommInput->pvBuffer + cbInputData;
	MsgInput.cbBuffer = pCommInput->cbBuffer;
	MsgInput.cbData = pCommInput->cbData - cbInputData;


	switch(pContext->State & 0xffff)
	{
	    case UNI_STATE_RECVD_UNIHELLO:
	    {
		if(MsgInput.cbData < 3)
		{
		    pctRet = PCT_INT_INCOMPLETE_MSG;
		}
		dwStateTransition = (((PUCHAR) MsgInput.pvBuffer)[2] << 16) |
							  (pContext->State & 0xffff);
			cbInputData = pCommInput->cbData;
		break;
	    }
	    case SSL3_STATE_REDO:
	    case SSL3_STATE_REDO_RESTART:
	    case SP_STATE_CONNECTED:
		  dwStateTransition =  pContext->State & 0xffff;
	      break;
	
	    default:
	    {

                pswrap = (SWRAP *) MsgInput.pvBuffer;
		        cbMsg = COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize);
		
                if(COMBINEBYTES(pswrap->bMajor, pswrap->bMinor)  < 0x300)
                    {
                        pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
		                break;
                    }
		
		        if (cbMsg > MsgInput.cbData)
	            {
	                return PCT_INT_INCOMPLETE_MSG;
	            }
	            cbInputData += cbMsg + sizeof(SWRAP); //Do One record at one time...
                if(0 != pContext->wS3CipherSuiteClient)
                {
                    pctRet = UnwrapSsl3Message(pContext, &MsgInput);
                }
                else
                {
                	pContext->ReadCounter++;
                }

		switch(((PUCHAR) MsgInput.pvBuffer)[0])
		{
		    case SSL3_CT_CHANGE_CIPHER_SPEC:
			cMessageType = SSL3_CT_CHANGE_CIPHER_SPEC;
			break;
		    case SSL3_CT_HANDSHAKE:
			cMessageType = ((PUCHAR) MsgInput.pvBuffer)[5];
			break;
		    case SSL3_CT_APPLICATIONDATA:
			cMessageType = SSL3_CT_APPLICATIONDATA;
			break;
		  default:
			pctRet = PCT_INT_ILLEGAL_MSG;
			break;
		}

		if (MsgInput.cbData < 5)
		{
		    pctRet = PCT_INT_INCOMPLETE_MSG;
		}

		dwStateTransition = (cMessageType << 16) |
				    (pContext->State & 0xffff);

	    }




	}
	if (pctRet != PCT_ERR_OK)
	{
	    // to handle incomplete message errors
		SP_RETURN(pctRet);
	}

	switch(dwStateTransition)
	{
	
	    case SP_STATE_CONNECTED:
	    // We were called from a connected state, so the app
	    // is requesting a redo.
		    BuildHelloRequest(pContext, pCommOutput);
		    break;

	case SSL3_STATE_REDO:
	    // We processed a client hello from the decrypt handler,
	    // so generate a server hello.
	    pctRet = Ssl3SrvGenServerHello(pContext, pCommOutput);
	    break;

	case SSL3_STATE_REDO_RESTART:
	    // We processed a client hello from the decrypt handler,
	    // so generate a server hello. (restart case)
	    pctRet = Ssl3SrvGenRestart(pContext, pCommOutput);
	    break;

	    case (SSL2_MT_CLIENT_HELLO << 16) | UNI_STATE_RECVD_UNIHELLO:
	    {
		    PSsl2_Client_Hello pSsl2Hello = NULL;

		pctRet = Ssl2UnpackClientHello(pCommInput, &pSsl2Hello);

		if (PCT_ERR_OK == pctRet)
		{
		    pctRet = Ssl3SrvHandleUniHello(
							    pContext,
							    pCommInput,
							    pSsl2Hello,
							    pCommOutput);


		    if (SP_FATAL(pctRet))
		    {
			pContext->State = PCT1_STATE_ERROR;
		    }
		}
		if(NULL != pSsl2Hello)
		{
			    SPExternalFree(pSsl2Hello);
		}
	    }
	    break;

	    case (SSL3_HS_CLIENT_HELLO << 16) :
	    {

		pctRet = Ssl3SrvHandleClientHello(
							pContext,
							pCommInput,
							pCommOutput);


		if (SP_FATAL(pctRet))
		{
		    pContext->State = PCT1_STATE_ERROR;
		}
	    }
	    break;

	    case (SSL3_CT_CHANGE_CIPHER_SPEC << 16) | SSL3_STATE_RESTART_CCS:
		pctRet = Ssl3HandleCCS(
					pContext,
					MsgInput.pvBuffer,
					MsgInput.cbData,
					TRUE);
		// State is set within the function call.
		pContext->State = SSL3_STATE_RESTART_CLI_FINISH;

		break;

	    case (SSL3_CT_CHANGE_CIPHER_SPEC << 16) | SSL3_STATE_CHANGE_CIPHER_SPEC:
		pctRet = Ssl3HandleCCS(
					pContext,
					MsgInput.pvBuffer,
					MsgInput.cbData,
					TRUE);


		break;
	    case (SSL3_HS_FINISHED << 16) | SSL3_STATE_CLIENT_FINISH:
		pctRet = Ssl3HandleClientFinish(
						pContext,
						MsgInput.pvBuffer,
						MsgInput.cbData,
						pCommOutput);
		if (PCT_ERR_OK == pctRet)
		{
		    pContext->State = SP_STATE_CONNECTED;
		    pContext->RipeZombie->fProtocol = SP_PROT_SSL3_SERVER;
		    pContext->Encrypt = Ssl3EncryptMessage;
		    pContext->Decrypt = Ssl3DecryptMessage;
		}
		break;

	    case (SSL3_HS_FINISHED << 16) | SSL3_STATE_RESTART_CLI_FINISH:
		    pctRet = VerifyFinishMsg(pContext, MsgInput.pvBuffer, MsgInput.cbData, TRUE);

		if (PCT_ERR_OK == pctRet)
		{
			pContext->State = SP_STATE_CONNECTED;
		    pContext->RipeZombie->fProtocol = SP_PROT_SSL3_SERVER;
		    pContext->Encrypt = Ssl3EncryptMessage;
		    pContext->Decrypt = Ssl3DecryptMessage;
		}
		break;


	    case (SSL3_HS_CLIENT_KEY_EXCHANGE << 16) | SSL2_STATE_SERVER_HELLO:
	    case ( SSL3_HS_CERTIFICATE << 16) | SSL2_STATE_SERVER_HELLO:
	    pctRet = Ssl3SrvHandleCMKey(pContext,
				(PBYTE)MsgInput.pvBuffer,
				MsgInput.cbData,
				pCommOutput);

            if (SP_FATAL(pctRet))
            {
                pContext->State = PCT1_STATE_ERROR;
            }
           break;
        default:
                DebugLog((DEB_WARN, "Error in protocol, dwStateTransition is %lx\n", dwStateTransition));
                pContext->State = PCT1_STATE_ERROR;
                pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
                if (cMessageType == SSL2_MT_ERROR)
                {
                // BUGBUG: handle an SSL2 error message
                }
                break;
        }
        if(pContext->State == SP_STATE_CONNECTED)
        {
            break;
        }

    } while(pCommInput->cbData - cbInputData) ;

    if (pctRet & PCT_INT_DROP_CONNECTION)
    {
	pContext->State &= ~SP_STATE_CONNECTED;
    }
    // To handle incomplete message errors:
    pCommInput->cbData = cbInputData;
    SP_RETURN(pctRet);
}





SP_STATUS
Ssl3SrvHandleClientHello(PSPContext  pContext,
			 PSPBuffer  pCommInput,
			 PSPBuffer  pCommOutput)
{
    SP_STATUS pctRet = PCT_ERR_ILLEGAL_MESSAGE;
	BOOL                            fRestart = FALSE;
    PBYTE pbMessage = (PBYTE) pCommInput->pvBuffer+sizeof(SWRAP);
    SWRAP *pswrap = (SWRAP *) pCommInput->pvBuffer;
    DWORD dwHandshakeLen;
    DWORD dwMessageLen = (INT) COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize);




    // Validate handshake type
    if(pbMessage[0] != SSL3_HS_CLIENT_HELLO)
    {
	SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    dwHandshakeLen = ((INT)pbMessage[1] << 16) +
			     ((INT)pbMessage[2] << 8) +
			     (INT)pbMessage[3];
    if( (dwHandshakeLen + sizeof(SHSH)) > dwMessageLen)
    {
	SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    UpdateHandshakeHash(pContext,
					(PUCHAR)pCommInput->pvBuffer+sizeof(SWRAP),
					pCommInput->cbData-sizeof(SWRAP),
					TRUE);

    pbMessage += sizeof(SHSH);

    if(!Ssl3ParseClientHello(pContext, pbMessage, dwHandshakeLen, &fRestart))
    {
	    SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    if(fRestart)
    {
	return Ssl3SrvGenRestart(pContext, pCommOutput);
    }
    else
    {
	return Ssl3SrvGenServerHello(pContext, pCommOutput);
    }
}


SP_STATUS
Ssl3SrvHandleUniHello(PSPContext  pContext,
		      PSPBuffer  pCommInput,
		      PSsl2_Client_Hello pHello,
		      PSPBuffer  pCommOutput)
{
    SP_STATUS   pctRet = PCT_ERR_ILLEGAL_MESSAGE;
//	DWORD       ServerCipher, ClientCipher;
    DWORD ClientCipher;


	UpdateHandshakeHash(pContext,
			(PUCHAR)pCommInput->pvBuffer+sizeof(SSL2_MESSAGE_HEADER),
			pCommInput->cbData-sizeof(SSL2_MESSAGE_HEADER),
			TRUE);


    /* keep challenge around for later */
    CopyMemory( pContext->pChallenge,
		pHello->Challenge,
		pHello->cbChallenge);
    pContext->cbChallenge = pHello->cbChallenge;

    /* Initialize the "Client.random" from the challenge */

    FillMemory(pContext->rgbS3CRandom, CB_SSL3_RANDOM - pContext->cbChallenge, 0);

    CopyMemory(  pContext->rgbS3CRandom + CB_SSL3_RANDOM - pContext->cbChallenge,
		 pContext->pChallenge,
		 pContext->cbChallenge);

	//
	// Calculate common ciphers:
	//
    pContext->wS3pendingCipherSuite = 0;
    // Does the client want this cipher type?
    for (ClientCipher = 0;
	 ClientCipher < pHello->cCipherSpecs ;
	 ClientCipher++ )
    {
	pctRet = Ssl3SelectCipher(pContext, pHello->CipherSpecs[ClientCipher]);
	if(pctRet == PCT_ERR_OK)
	{
	    break;
	}
    }
	//
    if (pctRet != PCT_ERR_OK)
    {
	return( pctRet );
    }
	// Generate the response
    return Ssl3SrvGenServerHello(pContext, pCommOutput);
}


SP_STATUS
Ssl3SrvGenRestart(
    PSPContext          pContext,
    PSPBuffer           pCommOutput)
{
    SP_STATUS   pctRet;
//  BOOL        fExchKey;
    DWORD       cbMessage;
    PUCHAR      pb;
    BOOL        fClientAuth = ((pContext->Flags & CONTEXT_FLAG_CLIAUTH) != 0);
    const CheckSumFunction * pHash = NULL;

    if(pCommOutput == NULL)
    {
	return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }
    pCommOutput->cbData = 0;
    pHash = HashFromSpec(pContext->PendingHashSpec, SP_PROT_SSL3_SERVER);
    if(pHash == NULL)
    {
	return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }

    // Calculate size of server hello
     cbMessage = sizeof(SWRAP) + // Record Wrapper
	 sizeof(SSH) +        // Server Hello message
	 CB_SLL3_CHANGE_CIPHER_SPEC +
	 CB_SSL3_FINISHED_MSG +
	 pHash->cbCheckSum;


    DebugLog((DEB_TRACE, "Server hello message %x\n", cbMessage));

    pCommOutput->cbData = cbMessage ;
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
    // Generate internal values to make server hello
    GenerateRandomBits(pContext->rgbS3SRandom, CB_SSL3_RANDOM);
    Ssl3MakeMasterKeyBlock(pContext);

    // Build Server Hello
    pb = pCommOutput->pvBuffer;
    pb  +=  sizeof(SWRAP);
    cbMessage -= sizeof(SWRAP);
    Ssl3BuildServerHello(pContext, pb);
    SetWrap(pContext, pCommOutput->pvBuffer, SSL3_CT_HANDSHAKE, sizeof(SSH), FALSE);
    UpdateHandshakeHash(pContext, pb, sizeof(SSH), FALSE);

    pb += sizeof(SSH);
    cbMessage -= sizeof(SSH);

    pContext->WriteCounter = 0;

    pctRet = BuildCCSAndFinishMessage(pContext,
				      pb,
					  cbMessage,
				      FALSE);

    if(pctRet != PCT_ERR_OK)
    {
	return pctRet;
    }
    pContext->State =  SSL3_STATE_RESTART_CCS;
    return(PCT_ERR_OK);

}

SP_STATUS
Ssl3SrvGenServerHello(
    PSPContext         pContext,
    PSPBuffer          pCommOutput)
{
    SP_STATUS   pctRet;
    BOOL        fExchKey = FALSE;
    DWORD       cbMessage;
    DWORD       cbCert, cbCertMsg;
	PUCHAR      pbCert;
    DWORD       dwKeyLen;
    DWORD       dwModulusLen;
    PUCHAR      pb;
    CERT *pcert ;
    DWORD               cbIssuerLists = 0;

    BOOL        fClientAuth = ((pContext->Flags & CONTEXT_FLAG_CLIAUTH) != 0);
    const CheckSumFunction * pHash = NULL;

    if(pCommOutput == NULL)
    {
	return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }
    pCommOutput->cbData = 0;

    pctRet = SPCredPickCertificate(SP_PROT_SSL3_SERVER,
				   pContext->pCred,
				   Ssl3CertRank,
				   Ssl3NumCert,
				   Ssl3CertRank,
				   Ssl3NumCert,
				   &pContext->RipeZombie->pServerCert,
				   &pContext->pPrivateKey);

    if(pctRet != PCT_ERR_OK)
    {
	return pctRet;
    }
    pctRet  = SPSerializeCertificate(SP_PROT_SSL3_SERVER,
				   NULL,
				   &cbCert,
				   pContext->RipeZombie->pServerCert);
    if(pctRet != PCT_ERR_OK)
    {
	return pctRet;
    }

    pHash = HashFromSpec(pContext->PendingHashSpec, SP_PROT_SSL3_SERVER);
    if(pHash == NULL)
    {
	return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }

    cbMessage = sizeof(SWRAP) + // Record Wrapper
	 sizeof(SSH) +        // Server Hello message
	 sizeof(SHSH) +         // Certificate Message
	 CB_SSL3_CERT_VECTOR +
	 cbCert +
	 sizeof(SHSH);          // Server Hello Done message

    dwKeyLen = (pContext->PendingCiphSpec & SP_CIPHER_STRENGTH) >> SP_CSTR_POS;
    // BUGBUG: Only works for LPBSAFE_PRV_KEY
    dwModulusLen = ((LPBSAFE_PRV_KEY)pContext->pPrivateKey->pKey)->bitlen;

    if(dwKeyLen <= 40 && dwModulusLen > 512)
    {
	    fExchKey = FBuildSsl3ExchgKeys();
	cbMessage += CB_SSL3_MAX_SRV_KEY_EXCHANGE;
    }

    if(fClientAuth)
    {
	pctRet = SPContextGetIssuers(pContext, NULL, &cbIssuerLists);

	if(PCT_ERR_OK != pctRet)
	{
	    return(pctRet);
	}
	cbMessage += cbIssuerLists  + sizeof(CERTREQ);
    }
    pContext->fCertReq = fClientAuth;

    DebugLog((DEB_TRACE, "Server hello message %x\n", cbMessage));

    pCommOutput->cbData = cbMessage ;
    pCommOutput->cbData += (FSsl3Cipher(FALSE)) ? pContext->pCheck->cbCheckSum : 0;
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

    // Generate the session ID (actually previously generated)
    pContext->RipeZombie->cbSessionID = CB_SSL3_SESSION_ID;

    // Generate internal values to make server hello
    GenerateRandomBits(pContext->rgbS3SRandom, CB_SSL3_RANDOM);

    // Build Server Hello
    pb = pCommOutput->pvBuffer;
    pb  +=  sizeof(SWRAP);
    Ssl3BuildServerHello(pContext, pb);
    pb +=sizeof(SSH);

    pcert = (CERT *) pb ;
    pbCert = &pcert->bcbCert24;
    //Handle CertChain....

    pctRet = SPSerializeCertificate(SP_PROT_SSL3_CLIENT,
				    &pbCert,
				    &cbCert,
				    pContext->RipeZombie->pServerCert);
    if(pctRet != PCT_ERR_OK)
    {
	return pctRet;
    }
    cbCertMsg = cbCert + CB_SSL3_CERT_VECTOR;
    SetHandshake(pb,  SSL3_HS_CERTIFICATE, NULL,  (WORD)(cbCertMsg));

    pcert->bcbClist24 = MS24BOF(cbCert);
    pcert->bcbMSBClist = MSBOF(cbCert);
    pcert->bcbLSBClist = LSBOF(cbCert);

    pb += sizeof(SHSH) + cbCertMsg;

    if(fExchKey)
    {
	int wLen;
		Ssl3BuildServerKeyExchange(pContext, pb, &wLen);
		pb += wLen;
		cbMessage -= (CB_SSL3_MAX_SRV_KEY_EXCHANGE - wLen);
	pCommOutput->cbBuffer = pCommOutput->cbData -= (CB_SSL3_MAX_SRV_KEY_EXCHANGE - wLen);
	}
	if(fClientAuth)
	{
	DWORD dwT;
	    Ssl3BuildCertificateRequest(pContext, pb, cbIssuerLists, &dwT);
	pb += dwT;
	}
    BuildServerHelloDone(pb, sizeof(SHSH));
    //Initialize the wrapper..
    UpdateHandshakeHash(pContext, (PUCHAR)pCommOutput->pvBuffer+sizeof(SWRAP), cbMessage-sizeof(SWRAP), FALSE);
    SetWrap(pContext, pCommOutput->pvBuffer, SSL3_CT_HANDSHAKE, cbMessage - sizeof(SWRAP), FALSE);

    pContext->State = SSL2_STATE_SERVER_HELLO;
    return(PCT_ERR_OK);

}

SP_STATUS Ssl3SrvHandleCMKey(PSPContext  pContext,
			      PUCHAR  pbInput,
			      DWORD cbMsg,
			      PSPBuffer  pCommOutput)
{
    SP_STATUS               pctRet = PCT_ERR_ILLEGAL_MESSAGE;
    PUCHAR                                  pb;
//    Ssl2_Client_Master_Key  Key;  //DON"T CHEKIN..
    INT iHandshakeLen;

    pCommOutput->cbData = 0;
    DebugLog((DEB_TRACE, "%3d:Input parameter from SSPI\n", cbMsg));

    do {
	DWORD cbExchgMsg, cbCert;
	CERT *pcert;
//	DWORD cbMessage;
	SWRAP *pswrap = (SWRAP *)pbInput;
	SHSH *pshsh;
	DWORD cbCertMsg;


	pb = pbInput;
	//Check for the Certificate message and send alert that YOU NEED CERTIFICATE...
	pshsh = (SHSH *) ((PBYTE)pbInput + sizeof(SWRAP));
	//IF we expect a Certificate we should error out...
	if(pContext->fCertReq && pshsh->typHS != SSL3_HS_CERTIFICATE)
	{ //Send out the client auth alert require certificate
	    break;
	}


	if(pContext->fCertReq)
	{
	    pcert = (CERT *) (pbInput + sizeof(SWRAP));
	    if(pcert->bcbCert24 != 0x00)
	    {
		    break; //This is NOT YET implemented..
	    }
	    cbCertMsg = COMBINEBYTES(pcert->bcbMSB, pcert->bcbLSB) + sizeof(SHSH);
	    cbCert = COMBINEBYTES(pcert->bcbMSBClist, pcert->bcbLSBClist) | ( pcert->bcbClist24 << 16);
	    UpdateHandshakeHash(pContext, (PUCHAR)pcert, cbCertMsg, FALSE);


	    pctRet = SPLoadCertificate(SP_PROT_SSL3_CLIENT,
					 SP_CERT_X509,
					 (PUCHAR)&pcert->bcbCert24,
					 cbCert,
					 &pContext->RipeZombie->pClientCert,
					 NULL);


	    if(pctRet != PCT_ERR_OK)
		    break;
	    pb += sizeof(SWRAP) + cbCertMsg ;
	}
	pctRet = ParseKeyExchgMsg(pContext, pb, cbMsg, &cbExchgMsg);
	if(pctRet != PCT_ERR_OK)
	{
	    return(pctRet);
	}
	pb += cbExchgMsg;

	if(*pb == SSL3_CT_HANDSHAKE)
	{
	    pb += sizeof(SWRAP);
	}
		// Validate handshake type
	if(pContext->fCertReq)
	{
	    if(pb[0] != SSL3_HS_CERTIFICATE_VERIFY)
	    {
		//Need a better error

		pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
		break;
	    }
	    // Validate handshake length
	    iHandshakeLen = ((INT)pb[1] << 16) +
			    ((INT)pb[2] << 8) +
			    (INT)pb[3];

	    if(!Ssl3ParseCertificateVerify(pContext, pb+sizeof(SHSH), iHandshakeLen))
	    {
		pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
		break;
	    }

	    pctRet = SPContextDoMapping(pContext);

	    if(pctRet != PCT_ERR_OK)
	    {
		break;
	    }

	    UpdateHandshakeHash(pContext, pb, iHandshakeLen + sizeof(SHSH), FALSE);
	    pb += iHandshakeLen + sizeof(SHSH);
	}



		    pContext->State = SSL3_STATE_CHANGE_CIPHER_SPEC;
	return( PCT_ERR_OK );
    } while(TRUE);

    return pctRet;
}



//Build Server hello onto pb... we need to check the boundary condition with cb
void
Ssl3BuildServerHello(PSPContext pContext, PBYTE pb)
{
    SSH *pssh = (SSH *) pb;
    WORD wT = sizeof(SSH) - sizeof(SHSH);

    FillMemory(pssh, sizeof(SSH), 0);
    pssh->typHS = SSL3_HS_SERVER_HELLO;
    pssh->bcbMSB = MSBOF(wT) ;
    pssh->bcbLSB = LSBOF(wT) ;
    pssh->bMajor = SSL3_CLIENT_VERSION_MSB;
    pssh->wCipherSelectedMSB = MSBOF(pContext->wS3pendingCipherSuite);
    pssh->wCipherSelectedLSB = LSBOF(pContext->wS3pendingCipherSuite);
    pssh->cbSessionId = (char)pContext->RipeZombie->cbSessionID;
    CopyMemory(pssh->rgbSessionId, pContext->RipeZombie->SessionID, pContext->RipeZombie->cbSessionID) ;
    //UNDONE: do t_time and convert to unix
    //  t_time - time(NULL);
    //  htonl(&t_time, pssh->rgbTime);
    CopyMemory(pssh->rgbRandom, pContext->rgbS3SRandom, CB_SSL3_RANDOM);
}


	// Server Hello Done
void BuildServerHelloDone(PBYTE pb, DWORD cb)
{
SHSH *pshsh = (SHSH *) pb ;

    SP_BEGIN("BuildServerHelloDone");
    FillMemory(pshsh, sizeof(SHSH), 0);
    pshsh->typHS = SSL3_HS_SERVER_HELLO_DONE;
    SP_END();
}

SP_STATUS
BuildCCSAndFinishForServer
(
PSPContext  pContext,
PSPBuffer  pCommOutput,
BOOL fDone
)
	//Build Change_cipher_spec message and finished message...
{
    // BUILD Change Cipher messages.
    DWORD cbOutMessage =    CB_SLL3_CHANGE_CIPHER_SPEC +  //ChangeCipherSpec.
				CB_SSL3_FINISHED_MSG +
			    pContext->pCheck->cbCheckSum ; //Finished Message;
    cbOutMessage += FSsl3Cipher(FALSE) ? pContext->pCheck->cbCheckSum : 0;
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

    return(BuildCCSAndFinishMessage(pContext,
				    pCommOutput->pvBuffer,
							    cbOutMessage,
				    FALSE));
}


SP_STATUS
EnoughDataCKeyExchg(PBYTE pCommInput, DWORD cbMsg)
    {
    DWORD dwTmp;
    DWORD dwKeyMsg;
	SHSH *pshsh = (SHSH *) ((PBYTE)pCommInput + sizeof(SWRAP));
    DWORD dwEncryptedKey;
    SWRAP *pswrap = (SWRAP *)pCommInput;
    DWORD dwAll;

    dwAll = COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize) + sizeof(SWRAP);

	dwEncryptedKey = COMBINEBYTES(pshsh->bcbMSB, pshsh->bcbLSB);
	dwKeyMsg = dwEncryptedKey + sizeof(SHSH) + sizeof(SWRAP);
    dwTmp = dwAll + CB_SLL3_CHANGE_CIPHER_SPEC
			    + CB_SSL3_FINISHED_MSG   ; //752 ? client auth cert+certverify
							    //Please don't checkin this.
	if(cbMsg <  dwTmp)
	{ // There could be ChangeCipher spec also
	    DebugLog((DEB_TRACE, "%3d:SIZE reported by SSPI\n", cbMsg));
	    DebugLog((DEB_TRACE, "%3d:SIZE Comparing\n", dwTmp));
		return(PCT_INT_INCOMPLETE_MSG);
	}
    else
	{
	    DebugLog((DEB_TRACE, "%3d:SIZE reported by SSPI\n", dwTmp));
	    return(PCT_ERR_OK);
	}
   }

SP_STATUS
ParseKeyExchgMsg(PSPContext  pContext, PBYTE pCommInput, DWORD cbMsg, DWORD *pcbExchgMsg)
{
    SP_STATUS pctRet = PCT_ERR_ILLEGAL_MESSAGE;
    SHSH *pshsh;
    UCHAR  rgbPreMasterSecret[CB_SSL3_PRE_MASTER_SECRET];
    DWORD dwEncryptedKey;
    DWORD dwDecrypted;
    PBYTE pbEncryptedKey;
    DWORD dwcbWrap = 0;
    PPctPrivateKey pPvtKey;
    const KeyExchangeSystem *KeyExchSys = NULL;
    CheckSumFunction *Check = NULL;

    if(*pCommInput == SSL3_CT_HANDSHAKE)
    {
	pshsh = (SHSH *) ((PBYTE)pCommInput + sizeof(SWRAP));
	dwcbWrap = sizeof(SWRAP);
    }
    else
    {
	pshsh = (SHSH *) pCommInput;
    }
    do
    {
	if(pshsh->typHS != SSL3_HS_CLIENT_KEY_EXCHANGE)
	{
	    break;
	}

	dwEncryptedKey = COMBINEBYTES(pshsh->bcbMSB, pshsh->bcbLSB);
	pbEncryptedKey = pCommInput + (sizeof(SHSH)) + dwcbWrap;


	if(pContext->fExchKey)
	{
		pPvtKey = g_pSsl3Private;
	}
	else
	{
		pPvtKey = pContext->pPrivateKey;
	}
	KeyExchSys = KeyExchangeFromSpec(pContext->PendingExchSpec, SP_PROT_SSL3_CLIENT);
	if(KeyExchSys == NULL)
	{
	    break;
	}

	/* Decrypt the encrypted portion of the master key */
	if (!KeyExchSys->Decrypt(pbEncryptedKey,
				dwEncryptedKey,
				rgbPreMasterSecret,
				&dwDecrypted,
				pPvtKey,
				SP_PROT_SSL3_SERVER))
	{
	    break;
	}
        if(COMBINEBYTES(rgbPreMasterSecret[0], rgbPreMasterSecret[1]) != SSL3_CLIENT_VERSION)
        	break;
	pctRet = PCT_ERR_OK;
	*pcbExchgMsg = dwEncryptedKey + sizeof(SHSH) + dwcbWrap;
	UpdateHandshakeHash(pContext,
			    pCommInput+dwcbWrap,
				    *pcbExchgMsg - dwcbWrap,
			    FALSE);

	Ssl3BuildMasterKeys(pContext, rgbPreMasterSecret);
	Ssl3MakeMasterKeyBlock(pContext);
	    break;
    } while(TRUE);
    return(pctRet);
}




/******************************************************************************
* Ssl3BuildCertificateRequest
*
* Build the CERTIFICATE_REQUEST handshake message.
*/
static BOOL Ssl3BuildCertificateRequest(
    PSPContext pContext,
    PBYTE pbMessage,        // out
    DWORD cbIssuerLen,  //in
    DWORD   *pdwMessageLen)     // out
{
    SP_STATUS       pctRet;
    PBYTE           pbMessageStart = pbMessage;
    PBYTE           pbIssuerLenLoc;
    DWORD           dwBodyLength;
    DWORD           cbIssuerLenLocal = cbIssuerLen;
    PSPCredential   pCred;
//    DWORD           iMapper;
    BOOL            fLocalMap = TRUE;

    //
    // Get server's certificate issuer. For now, this is the only issuer
    // we will accept from the client.
    //

    pCred = pContext->pCred;

    // HandshakeType
    pbMessage[0] = SSL3_HS_CERTIFICATE_REQUEST;
    pbMessage += 1;

    // Skip message body length field (3 bytes)
    pbMessage += 3;

    //
    // enum {
    //     rsa_sign(1), dss_sign(2), rsa_fixed_dh(3), dss_fixed_dh(4),
    //     rsa_ephemeral_dh(5), dss_ephemeral_dh(6), fortezza_dms(20), (255)
    // } ClientCertificateType;
    //
    // opaque DistinguishedName<1..2^16-1>;
    //
    // struct {
    //     ClientCertificateType certificate_types<1..2^8-1>;
    //     DistinguishedName certificate_authorities<3..2^16-1>;
    // } CertificateRequest;
    //

    // Certificate type
    pbMessage[0] = 1;       // certificate type vector length
    pbMessage[1] = 1;       // rsa_sign
    pbMessage += 2;

    pbIssuerLenLoc = pbMessage;

    // Issuer name list length (just one entry in list for now)
    pbMessage += 2;

    pctRet = SPContextGetIssuers(pContext, pbMessage, &cbIssuerLenLocal);
    if(PCT_ERR_OK != pctRet)
    {
	return FALSE;
    }
    pbIssuerLenLoc[0] = MSBOF(cbIssuerLenLocal);
    pbIssuerLenLoc[1] = LSBOF(cbIssuerLenLocal);

    pbMessage += cbIssuerLenLocal;

    // Compute message body length (subtract 4 byte header)
    dwBodyLength = pbMessage - pbMessageStart - 4;

    // Fill in message body length field (3 bytes)
    pbMessageStart[1] = (UCHAR) ((dwBodyLength & 0x00ff0000) >> 16);
    pbMessageStart[2] = MSBOF(dwBodyLength);
    pbMessageStart[3] = LSBOF(dwBodyLength);

    *pdwMessageLen = dwBodyLength + 4;

    return TRUE;
}






/*****************************************************************************/
static BOOL Ssl3ParseCertificateVerify(PSPContext  pContext, PBYTE pbMessage, INT iMessageLen)
{
    PBYTE pbMessageStart = pbMessage;
    BYTE  pbInputBlock[SSL3_ENCRYPTED_KEY_SIZE];
    BYTE  pbOutputBlock[SSL3_ENCRYPTED_KEY_SIZE];
    INT   iBlockLen;
    INT   iPadLen;
    INT   i;
    BYTE  rgbHashValue[MD5DIGESTLEN+A_SHA_DIGEST_LEN];
	LPBSAFE_PUB_KEY pk;

    pk  = (LPBSAFE_PUB_KEY)(pContext->RipeZombie->pClientCert->pPublicKey->pKey);

    //
    // digitally-signed struct {
    //     opaque md5_hash[16];
    //     opaque sha_hash[20];
    // } Signature;
    //
    // struct {
    //     Signature signature;
    // } CertificateVerify;
    //
    // CertificateVerify.signature.md5_hash = MD5(master_secret + pad2 +
    //          MD5(handshake_messages + master_secret + pad1));
    //
    // CertificateVerify.signature.sha_hash = SHA(master_secret + pad2 +
    //          SHA(handshake_messages + master_secret + pad1));
    //

    // Signature block length
    iBlockLen = ((INT)pbMessage[0] << 8) + pbMessage[1];
//    if(iBlockLen != (INT)pSslContext->lpClientPublicKey->keylen)
//       {
//          iBlockLen, pSslContext->lpClientPublicKey->keylen);
//          return FALSE;
//        }
    pbMessage += 2;

    // Signature block. Convert to little endian with at least 8 bytes of
    // zero padding at the end.
    FillMemory(pbInputBlock, SSL3_ENCRYPTED_KEY_SIZE, 0);
    ReverseMemCopy(pbInputBlock, pbMessage, iBlockLen);
    pbMessage += iBlockLen;


    // Zero destination buffer
    FillMemory(pbOutputBlock, SSL3_ENCRYPTED_KEY_SIZE, 0);


    // Decrypt signature block, using public key contained in certificate
    if(BSafeEncPublic(pk, pbInputBlock, pbOutputBlock) == FALSE)
    {
	return FALSE;
    }

    // Convert decrypted signature block to little endian.
    ReverseMemCopy(pbInputBlock, pbOutputBlock, iBlockLen);


    // Verify signature block format
    iPadLen = iBlockLen - MD5DIGESTLEN - A_SHA_DIGEST_LEN - 3;
    if(pbInputBlock[0] != 0 || pbInputBlock[1] != 1 ||
       pbInputBlock[2 + iPadLen] != 0)
    {
	return FALSE;
    }
    for(i=0 ; i<iPadLen ; i++)
    {
	    if(pbInputBlock[i + 2] != 0xff)
	    return FALSE;
    }

    // Generate hash values
    ComputeCertVerifyHashes(pContext, rgbHashValue);


    // Verify hash values
    if(memcmp(rgbHashValue, pbInputBlock + iPadLen + 3, MD5DIGESTLEN + A_SHA_DIGEST_LEN))
    {
	return FALSE;
    }

    // Validate message length
//    if(iMessageLen != pbMessage - pbMessageStart)
//    {
//      return FALSE;
//   }

    return TRUE;
}







BOOL
FSSL3(PBYTE pb, BYTE *pchCT)
{
    UCHAR bCT = pb[0];

    *pchCT = bCT;

    // if (major == 3 && minor == 0)
    if (pb[1] == 0x03 && pb[2] == 0x00)
    {
	if (bCT >= 20 && bCT <= 23)             // VOILA! Version 3
	{
	    return(TRUE);
	}
    }
    return(FALSE);
}





/******************************************************************************
* Ssl3BuildServerKeyExchange
*
* Determine if it is necessary to send a SERVER_KEY_EXCHANGE handshake
* message. If so, build the message.
*/
static BOOL Ssl3BuildServerKeyExchange(
    PSPContext  pContext,
    PBYTE pbMessage,                // out
    PINT  piMessageLen)             // out
{
    PBYTE pbMessageStart = pbMessage;
    INT   iBodyLength;
    BYTE  pbModulus[SSL3_ENCRYPTED_KEY_SIZE];
    DWORD   iModulusLen;
    BYTE  pbExponent[4];
    DWORD   iExponentLen;
    PBYTE pbServerParams = NULL;
    INT   iServerParamsLen;
    BYTE  pbInputBlock[SSL3_ENCRYPTED_KEY_SIZE];
    BYTE  pbOutputBlock[SSL3_ENCRYPTED_KEY_SIZE];
    DWORD   iBlockLen;
    INT   iPadLen;
    BYTE  pbMd5HashValue[16];
    BYTE  pbShaHashValue[20];
    LPBSAFE_PUB_KEY pPublicKey = NULL;

    //
    // If we are using an export cipher suite and the certificate
    // has a modulas of more than 512 bits, then we need to send a
    // SERVER_KEY_EXCHANGE message (containing a 512-bit public key)
    // so that the client has something with which to encrypt the
    // PRE_MASTER_KEY.
    //


    // HandshakeType
    pbMessage[0] = SSL3_HS_SERVER_KEY_EXCHANGE;
    pbMessage += 1;

    // Skip message body length field (3 bytes)
    pbMessage += 3;

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

    // Extract modulus from BSAFE public key structure. Add padding byte
    // if necessary.
    pPublicKey = (LPBSAFE_PUB_KEY)g_pSsl3Public->pKey;
    iModulusLen = pPublicKey->bitlen / 8;
    ReverseMemCopy(pbModulus, (PBYTE)(pPublicKey + 1), iModulusLen);
    if(pbModulus[0] & 0x80)
    {
	// Add padding byte
	MoveMemory(pbModulus + 1, pbModulus, iModulusLen);
	pbModulus[0] = 0;
	iModulusLen += 1;
    }

    // Extract exponent from BSAFE public key structure.
    ReverseMemCopy(pbExponent, (PBYTE)&pPublicKey->pubexp, sizeof(DWORD));
    iExponentLen = sizeof(DWORD);
    while(pbExponent[0] == 0)
    {
	iExponentLen--;
	MoveMemory(pbExponent, pbExponent + 1, iExponentLen);
    }

    // Mark start of ServerRSAParams structure.
    // This is used to build hash values.
    pbServerParams = pbMessage;

    // Modulus length
    pbMessage[0] = MSBOF(iModulusLen);
    pbMessage[1] = LSBOF(iModulusLen);
    pbMessage += 2;

	CopyMemory(pbMessage, pbModulus, iModulusLen);
    pbMessage += iModulusLen;

    // Exponent length
    pbMessage[0] = MSBOF(iExponentLen);
    pbMessage[1] = LSBOF(iExponentLen);
    pbMessage += 2;

    // Exponent data
    CopyMemory(pbMessage, pbExponent, iExponentLen);
    pbMessage += iExponentLen;

    // Compute length of ServerRSAParams structure.
    iServerParamsLen = pbMessage - pbServerParams;

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

    // Generate hash values
    ComputeServerExchangeHashes(pContext, pbServerParams,
				iServerParamsLen,
				pbMd5HashValue,
				pbShaHashValue);



    // Build signature block (in network order)
    iBlockLen = ((LPBSAFE_PRV_KEY)pContext->pPrivateKey->pKey)->bitlen / 8;
    iPadLen = iBlockLen - MD5DIGESTLEN - A_SHA_DIGEST_LEN - 3;
    pbOutputBlock[0] = 0;
    pbOutputBlock[1] = 1;
    FillMemory(pbOutputBlock + 2, iPadLen, 0xff);
    pbOutputBlock[iPadLen + 2] = 0;
    CopyMemory(pbOutputBlock + iPadLen + 3, pbMd5HashValue, MD5DIGESTLEN);
    CopyMemory(pbOutputBlock + iPadLen + 3 + MD5DIGESTLEN, pbShaHashValue, A_SHA_DIGEST_LEN);


    // Convert signature block to little endian (so BSAFE is happy)
    FillMemory(pbInputBlock, SSL3_ENCRYPTED_KEY_SIZE, 0);
    ReverseMemCopy(pbInputBlock, pbOutputBlock, iBlockLen);

    // Encrypt (aka "sign") signature block with (large) private key
    if(!BSafeDecPrivate((LPBSAFE_PRV_KEY)pContext->pPrivateKey->pKey,
			pbInputBlock,
			pbOutputBlock))
	{
	return FALSE;
    }

    // Store block length to message
    pbMessage[0] = MSBOF(iBlockLen);
    pbMessage[1] = LSBOF(iBlockLen);
    pbMessage += 2;

    // Store signature block to message, converting it back to network order
    ReverseMemCopy(pbMessage, pbOutputBlock, iBlockLen);
    pbMessage += iBlockLen;

    // Compute message body length (subtract 4 byte header)
    iBodyLength = pbMessage - pbMessageStart - 4;


	SetHandshake(pbMessageStart,  SSL3_HS_SERVER_KEY_EXCHANGE, NULL,  (WORD)iBodyLength);


    *piMessageLen = iBodyLength + 4;
	pContext->fExchKey = TRUE;
    return TRUE;
}


SP_STATUS
Ssl3HandleClientFinish(
    PSPContext  pContext,
    PUCHAR      pb,
    DWORD       cbMessage,
    PSPBuffer   pCommOutput
    )
{
    SP_STATUS          pctRet;
	SP_BEGIN("Ssl3HandleClientFinish");


    pctRet = VerifyFinishMsg(pContext, pb, cbMessage, TRUE);
	if(pctRet != PCT_ERR_OK)
		return(pctRet);
	pctRet = BuildCCSAndFinishForServer(pContext, pCommOutput, FALSE);
	/* Cache Session Here */
    if(pctRet == PCT_ERR_OK)
    {
	SPCacheAdd(pContext->RipeZombie);
    }
    SP_END();
    return(pctRet);
}


/******************************************************************************
* Ssl3ParseClientHello
*
* This routine parses just the CLIENT_HELLO message itself. The
* handshake crud has already been stripped off.
*/
BOOL Ssl3ParseClientHello
(
PSPContext  pContext,
PBYTE pbMessage,
INT iMessageLen,
BOOL *pfRestart
)
{
    PBYTE pbMessageStart = pbMessage;
    PBYTE pbCliRandom = NULL;
    INT iVersion;
    INT iSessionIdLen;
    INT iCipherSpecLen;
//    INT iCipherSpec;
    INT iCompMethodLen;
    INT iCompMethod;
    INT i;
//	SessCacheItem CachedSession;
    SP_STATUS pctRet = PCT_INT_INTERNAL_ERROR;

    //
    // struct {
    //     ProtocolVersion client_version;
    //     Random random;
    //     SessinoID session_id;
    //     CipherSuite cipher_suites<2..2^16-1>
    //     CompressionMethod compression_methods<1..2^8-1>;
    // } ClientHello;
    //

	*pfRestart = FALSE;


    // ProtocolVersion = client_version;
    iVersion = ((INT)pbMessage[0] << 8) + pbMessage[1];
    if(iVersion < SSL3_CLIENT_VERSION)
    {
		return FALSE;
    }
    pbMessage += 2;

	pbCliRandom = pbMessage;
    // Random random
    CopyMemory(pContext->rgbS3CRandom, pbMessage, CB_SSL3_RANDOM);
	pContext->cbChallenge = CB_SSL3_RANDOM;
    pbMessage += CB_SSL3_RANDOM;

    // SessionID session_id; (length)
    iSessionIdLen = pbMessage[0];
    if(iSessionIdLen > CB_SSL3_SESSION_ID)
    {
		return FALSE;
    }
    pbMessage += 1;
    if ((iSessionIdLen) &&
	(SPCacheRetrieveBySession(SP_PROT_SSL3_SERVER,
				  pbMessage,
				  iSessionIdLen,
				  &pContext->RipeZombie)))
    {
	    CopyMemory(pContext->rgbS3CRandom, pbCliRandom, CB_SSL3_RANDOM);
		DebugLog((DEB_TRACE, "------------TRYING TO DO RESTART---------"));
		*pfRestart = TRUE;
    }
    pbMessage += iSessionIdLen;
    // CipherSuite cipher_suites<2..2^16-1>; (length)
    iCipherSpecLen = ((INT)pbMessage[0] << 8) + pbMessage[1];
    if(iCipherSpecLen % 2)
	{
	    return FALSE;
	}
    pbMessage += 2;

    // CipherSuite cipher_suites<2..2^16-1>;
    pContext->wS3pendingCipherSuite = SSL3_NULL_WITH_NULL_NULL;
    // Does the client want this cipher type?
    for(i = 0 ; i < iCipherSpecLen / 2 ; i++)
    {
	pctRet = Ssl3SelectCipher(pContext, COMBINEBYTES(pbMessage[i*2], pbMessage[(2*i)+1]));
	if(pctRet == PCT_ERR_OK)
	{
	    break;
	}
    }
	//
    if (pctRet != PCT_ERR_OK)
    {
	return( FALSE );
    }


    pbMessage += iCipherSpecLen;
    // CompressionMethod compression_methods<1..2^8-1>; (length)
    iCompMethodLen = pbMessage[0];
    if(iCompMethodLen < 1)
    {
		return FALSE;
    }
    pbMessage += 1;

	for(i = 0 ; i <iCompMethodLen; i++)
	{
		if(pbMessage[i] == 0)
		{
			iCompMethod = 0;
			break;
		}
		
	}
    pbMessage += iCompMethodLen;
    // CompressionMethod compression_methods<1..2^8-1>;
    if(iCompMethod != 0)
    {
		return FALSE;
    }

    // Validate message length
    if(iMessageLen != pbMessage - pbMessageStart)
    {
		return TRUE;
    }

    return TRUE;
}


SP_STATUS
BuildHelloRequest
(
PSPContext  pContext,
PSPBuffer  pCommOutput
)
{
	DWORD cbMessage = sizeof(SHSH) + sizeof(SWRAP) + pContext->pCheck->cbCheckSum;
	PBYTE pb;

	pCommOutput->cbData = cbMessage ;

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

	pb = pCommOutput->pvBuffer;
	pb  +=  sizeof(SWRAP);

	FillMemory(pb, sizeof(SHSH), 0);
	pb[0] = SSL3_HS_HELLO_REQUEST;

	SetWrap(pContext, pCommOutput->pvBuffer, SSL3_CT_HANDSHAKE, sizeof(SHSH), FALSE);
    return PCT_ERR_OK;

}


