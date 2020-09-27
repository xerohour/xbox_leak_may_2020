/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
*	Owner 			:ramas
*	Date			:4/16/96
*	description		: Main Crypto functions for SSL3
*----------------------------------------------------------------------------*/

#include <spbase.h>
#include <ssl2msg.h>
#include <ssl3msg.h>
#include <ssl3key.h>

#include <ssl2prot.h>
#include <md5.h>
#include <sha.h>
#include <rsa.h>

const Ssl2CipherMap Ssl3CipherRank[] =
{
    {SSL3_RSA_WITH_RC4_128_MD5, SP_HASH_MD5 , SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1},
    {SSL3_RSA_WITH_RC4_128_SHA, SP_HASH_SHA , SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1},
    {SSL_RSA_FINANCE64_WITH_RC4_64_MD5, SP_HASH_MD5 , SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1},
    {SSL_RSA_FINANCE64_WITH_RC4_64_SHA, SP_HASH_SHA , SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1},
    {SSL3_RSA_EXPORT_WITH_RC4_40_MD5, SP_HASH_MD5 , SP_CIPHER_RC4 | SP_ENC_BITS_40 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1},
    {SSL3_RSA_WITH_NULL_MD5, SP_HASH_MD5, SP_CIPHER_NONE | SP_ENC_BITS_128 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1},
    {SSL3_RSA_WITH_NULL_SHA, SP_HASH_SHA, SP_CIPHER_NONE | SP_ENC_BITS_128 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1}
};

const DWORD Ssl3NumCipherRanks = sizeof(Ssl3CipherRank)/sizeof(Ssl2CipherMap);

CertSpec Ssl3CertRank[] = {
    SP_CERT_X509
};

const DWORD Ssl3NumCert = sizeof(Ssl3CertRank)/sizeof(CertSpec);


void DoSsl3MAC
(
	PSPContext pContext,
	PSPBuffer pClean,
	PCheckSumBuffer pwriteread,		//Need to pass in WriteMacState, WriteMacState1 for Encrypt
	PCheckSumBuffer pwriteread1,		//AND pas in ReadMacState. ReadMACstate1 for decrypt
	DWORD	dwSeq,
	CHAR cType,
	PUCHAR pbMAC
);



#if VERIFYHASH
BYTE  rgbF[5000];
DWORD ibF = 0;
#endif

#define CB_PAD1  48


//------------------------------------------------------------------------------------------

SP_STATUS WINAPI
Ssl3DecryptHandler(
    PSPContext pContext,
    PSPBuffer pCommInput,
    PSPBuffer pAppOutput)
{
    SP_STATUS pctRet = 0;

    if (pCommInput->cbData > 0)
    {
        // First, we'll handle incoming data packets:

        if ((pContext->State & SP_STATE_CONNECTED) && pContext->Decrypt)
        {
            pctRet = pContext->Decrypt(
		                    pContext,
		                    pCommInput,		// message
		                    pAppOutput);	// Unpacked Message
            if (PCT_ERR_OK != pctRet)
            {
                return pctRet;
            }
            if(*(PBYTE)pCommInput->pvBuffer == SSL3_CT_HANDSHAKE /*Escape*/)
    	    {

    	        if(pAppOutput->cbData == 0)
                {
                    return (PCT_INT_ILLEGAL_MSG);
                }
                switch(*(PBYTE)pAppOutput->pvBuffer)
                {
                    case SSL3_HS_HELLO_REQUEST:
                        // We're fully reading the hello request here
                        pContext->State = SSL3_STATE_HELLO_REQUEST;
                        pAppOutput->cbData = 0;
                        return SP_LOG_RESULT(PCT_INT_RENEGOTIATE);

#ifndef SCHANNEL_CLIENT_ONLY
                    case SSL3_HS_CLIENT_HELLO:
                        {
	                        BOOL 				fRestart = FALSE;
                            PBYTE pbMessage = (PBYTE) pAppOutput->pvBuffer;
                            DWORD dwHandshakeLen = pAppOutput->cbData;
                            // We received a client hello, so process the hello,
                            // Validate handshake type

                            UpdateHandshakeHash(pContext,
				                                (PUCHAR)pAppOutput->pvBuffer,
				                                dwHandshakeLen,
				                                TRUE);

                            pbMessage += sizeof(SHSH);

                            if(!Ssl3ParseClientHello(pContext, pbMessage, dwHandshakeLen - sizeof(SHSH), &fRestart))
                            {
	                            SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
                            }
                           	//WE SHOULDn't be doing Retstart on REDO's. May be we may ask for Client-auth
                            SPCacheDereference(pContext->RipeZombie);
                           	if(!SPCacheRetrieveNew(NULL, &pContext->RipeZombie))
			            	{
	                            SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
                            }

							pContext->RipeZombie->fProtocol = pContext->pCred->Type;

                            pContext->State = SSL3_STATE_REDO;
                            pAppOutput->cbData = 0;
                            return SP_LOG_RESULT(PCT_INT_RENEGOTIATE);
                        }
#endif //!SCHANNEL_CLIENT_ONLY                        
                    default:
                         /* Unknown escape, generate error */
                        pctRet = SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
                        /* Disconnect */
                        break;
               }

    	    }
    	    else  if(*(PBYTE)pCommInput->pvBuffer == SSL3_CT_ALERT /*Escape*/)
            {
                UCHAR bAlertLevel;
                UCHAR bAlertDesc;
                if(pAppOutput->cbData != 2)
                {
                    pctRet =  SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
                }

                bAlertLevel = *(PBYTE)pAppOutput->pvBuffer;
                bAlertDesc = *((PBYTE)pAppOutput->pvBuffer + 1);
                if(bAlertLevel == SSL3_ALERT_WARNING)
                {
                    if (bAlertDesc == SSL3_ALERT_CLOSE_NOTIFY)
                    {
                        // we dont expect any more messages on this connection
                        pContext->State = SP_STATE_NONE;
                    }
                }
                else if (bAlertLevel == SSL3_ALERT_FATAL)
                {
                    pctRet = SP_LOG_RESULT(PCT_INT_MSG_ALTERED);
                }
                else
                {
                    pctRet =  SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
                }
                pAppOutput->cbData = 0; // swallow the decrypted data in all cases

                // DBGCHK(TEXT("SCHANNEL ALERT"),pctRet==0);
            }
            /* look for escapes */
            return pctRet;
        }
        else
        {
            return(PCT_INT_ILLEGAL_MSG);
        }
    }
    return PCT_INT_INCOMPLETE_MSG;
}




#if   DBG
BYTE rgb3Mac[2048];
DWORD ibMac = 0;
#endif

void DoSsl3MAC
(
PSPContext pContext,
PSPBuffer pClean,
PCheckSumBuffer pwriteread,		//Need to pass in WriteMacState, WriteMacState1 for Encrypt
PCheckSumBuffer pwriteread1,		//AND pas in ReadMacState. ReadMACstate1 for decrypt
DWORD	dwSeq,
CHAR cType,
PUCHAR pbMAC
)
	{
    PCheckSumBuffer             pSum;
    HashBuf                     SumBuf;
    PCheckSumBuffer             pSumFinal;
    HashBuf                     SumBufFinal;
    DWORD                       dwReverseSeq;
    DWORD                       dw32High = 0;
    SHORT                       wData, wDataReverse;
    UCHAR                       rgbDigest[MAX_DIGEST_LEN];

	CloneHashBuf(SumBuf, pwriteread, pContext->pCheck);
    pSum = (PCheckSumBuffer)SumBuf;
    dwReverseSeq = htonl(dwSeq);

    /* add count */
    pContext->pCheck->Sum( pSum, sizeof(DWORD), (PUCHAR) &dw32High);
    pContext->pCheck->Sum( pSum, sizeof(DWORD), (PUCHAR) &dwReverseSeq );
    if(cType != 0)
	    pContext->pCheck->Sum( pSum, 1 , &cType );

    /* add length */
    wData = (short) (pClean->cbData - pContext->pCheck->cbCheckSum); //length to be MAC'ed
 	wDataReverse = wData >> 8 | wData << 8;
    pContext->pCheck->Sum( pSum, sizeof(short), (PUCHAR) &wDataReverse );

   /* add data */
    pContext->pCheck->Sum( pSum, wData, (PUCHAR)pClean->pvBuffer );
#if VERIFYHASH
	if(ibMac > 1800)
		ibMac = 0;
	CopyMemory(&rgb3Mac[ibMac], (BYTE *)&dw32High, sizeof(DWORD));
	ibMac += sizeof(DWORD);
	CopyMemory(&rgb3Mac[ibMac], (BYTE *)&dwReverseSeq, sizeof(DWORD));
	ibMac += sizeof(DWORD);
	CopyMemory(&rgb3Mac[ibMac], (BYTE *)&wDataReverse, sizeof(WORD));
	ibMac += sizeof(WORD);
	if(wData < 50)
		{
		CopyMemory(&rgb3Mac[ibMac], (PUCHAR)pClean->pvBuffer, wData);
		ibMac += wData;
		}
#endif

    pContext->pCheck->Finalize(pSum, rgbDigest);
#if VERIFYHASH
	CopyMemory(&rgb3Mac[ibMac], rgbDigest, pContext->pCheck->cbCheckSum);
	ibMac += pContext->pCheck->cbCheckSum;
#endif
	CloneHashBuf(SumBufFinal, pwriteread1, pContext->pCheck);

    pSumFinal = (PCheckSumBuffer)SumBufFinal;
    pContext->pCheck->Sum(pSumFinal, pContext->pCheck->cbCheckSum,  rgbDigest);

    pContext->pCheck->Finalize(pSumFinal, rgbDigest);
#if VERIFYHASH
	CopyMemory(&rgb3Mac[ibMac], rgbDigest, pContext->pCheck->cbCheckSum);
	ibMac += pContext->pCheck->cbCheckSum;
#endif
	CopyMemory(pbMAC, rgbDigest, pContext->pCheck->cbCheckSum);
	}



void Ssl3BuildFinishMessage(
PSPContext pContext,
BYTE *pbMd5Digest,
BYTE *pbSHADigest,
BOOL fClient //This is needed to Verify the Finished messages
)
	{
    MD5_CTX Md5Hash;
    A_SHA_CTX ShaHash;
    BYTE rgbPad1[CB_PAD1];
    BYTE rgbPad2[CB_PAD1];
    DWORD ib;
    BYTE szClnt[] = "CLNT";
    BYTE szSrvr[] = "SRVR";
//    DWORD cbMessage;

    for(ib = 0 ; ib < CB_PAD1; ib++)
    {
    	rgbPad1[ib] = CH_SLL3_MD5_PAD_CHAR;
    	rgbPad2[ib] = CH_SSL3_SHA_PAD_CHAR;
    }
	//UNDONE: cbPad = CB_SSL3_SHA_PAD if SHA
    // HandshakeType

    //
    // Compute the two hash values as follows:
    //
    // enum { client(0x434c4e54), server(0x53525652) } Sender;
    // enum { client("CLNT"), server("SRVR") } Sender;
    //
    // struct {
    //	   opaque md5_hash[16];
    //	   opaque sha_hash[20];
    // } Finished;
    //
    // md5_hash  -  MD5(master_secret + pad2 + MD5(handshake_messages +
    //			Sender + master_secret + pad1))
    //
    // sha_hash  -  SHA(master_secret + pad2 + SHA(handshake_messages +
    //			Sender + master_secret + pad1))
    //
    // pad_1 - The character 0x36 repeated 48 times for MD5 or
    //	       40 times for SHA.
    //
    // pad_2 - The character 0x5c repeated the same number of times.
    //

    //
    // Build MD5 Hash
    //

    // Make local copy of the handshake_messages MD5 hash object
    CopyMemory((BYTE *)&Md5Hash,
	   (BYTE *)pContext->MD5Handshake,
	   sizeof(MD5_CTX));

    // Add rest of stuff to local MD5 hash object.
    if (fClient)
		MD5Update(&Md5Hash, szClnt, 4);  // This is Better be 4, NO # define...
	else
		MD5Update(&Md5Hash, szSrvr, 4);
    MD5Update(&Md5Hash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);

    MD5Update(&Md5Hash, rgbPad1, CB_SSL3_MD5_PAD);
    MD5Final(&Md5Hash);
    CopyMemory(pbMd5Digest, Md5Hash.digest, MD5DIGESTLEN);

    // Compute "parent" MD5 hash
    MD5Init(&Md5Hash);
    MD5Update(&Md5Hash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);
    MD5Update(&Md5Hash, rgbPad2, CB_SSL3_MD5_PAD);
    MD5Update(&Md5Hash, pbMd5Digest, MD5DIGESTLEN);
    MD5Final(&Md5Hash);
    CopyMemory(pbMd5Digest, Md5Hash.digest, MD5DIGESTLEN);

    // Build SHA Hash

    // Make local copy of the handshake_messages SHA hash object
    CopyMemory((BYTE *)&ShaHash, (BYTE *)pContext->SHAHandshake, sizeof(A_SHA_CTX));

    // SHA(handshake_messages + Sender + master_secret + pad1)
    if (fClient)
		A_SHAUpdate(&ShaHash, szClnt, 4);
	else
		A_SHAUpdate(&ShaHash, szSrvr, 4);

    A_SHAUpdate(&ShaHash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);
    A_SHAUpdate(&ShaHash, rgbPad1, CB_SSL3_SHA_PAD);
    A_SHAFinal(&ShaHash, pbSHADigest);

    // SHA(master_secret + pad2 + SHA-hash);
    A_SHAInit(&ShaHash);
    A_SHAUpdate(&ShaHash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);
    A_SHAUpdate(&ShaHash, rgbPad2, CB_SSL3_SHA_PAD);
    A_SHAUpdate(&ShaHash, pbSHADigest, A_SHA_DIGEST_LEN);
    A_SHAFinal(&ShaHash, pbSHADigest);

}




SP_STATUS WINAPI
Ssl3EncryptRaw( PSPContext pContext,
                    PSPBuffer   pAppInput,
                    PSPBuffer   pCommOutput,
                    BYTE        bType)
{
    SP_STATUS       pctRet;
    SPBuffer        Clean;
    SPBuffer        Encrypted;
    UCHAR           rgbMAC[MAX_DIGEST_LEN];
    PUCHAR          pbMAC;
//    SWRAP *         pwsrap;


    Clean.cbData = Encrypted.cbData = pAppInput->cbData+pContext->pCheck->cbCheckSum;
    Clean.pvBuffer = Encrypted.pvBuffer = (PUCHAR)pCommOutput->pvBuffer+sizeof(SWRAP);
    Clean.cbBuffer = Encrypted.cbBuffer = pCommOutput->cbBuffer-sizeof(SWRAP);

    /* Move data out of the way if necessary */
    if((PUCHAR)Clean.pvBuffer != (PUCHAR)pAppInput->pvBuffer)
    {
        DebugLog((DEB_WARN, "SSL3EncryptRaw: Unnecessary Move, performance hog\n"));
        MoveMemory((PUCHAR)Clean.pvBuffer,
                    (PUCHAR)pAppInput->pvBuffer,
                    pAppInput->cbData);
    }

    DoSsl3MAC(pContext,
              &Clean,
              pContext->WriteMACState,
              pContext->WriteMACState1,
              pContext->WriteCounter,
              bType,
              rgbMAC);

    pbMAC = (PUCHAR)Clean.pvBuffer + (Clean.cbBuffer - pContext->pCheck->cbCheckSum);

    CopyMemory(pbMAC, rgbMAC, pContext->pCheck->cbCheckSum);
    pContext->WriteCounter ++ ;

    pctRet = pContext->pSystem->Encrypt(pContext->pWriteState, &Clean, &Encrypted);
    pCommOutput->cbData = Encrypted.cbData + sizeof(SWRAP);

    return(pctRet);
}

SP_STATUS WINAPI
Ssl3EncryptMessage( PSPContext pContext,
                    PSPBuffer   pAppInput,
                    PSPBuffer   pCommOutput)
{

    SetWrapNoEncrypt(pCommOutput->pvBuffer,
                        SSL3_CT_APPLICATIONDATA,
                        pAppInput->cbData+pContext->pCheck->cbCheckSum);
    return Ssl3EncryptRaw(pContext, pAppInput, pCommOutput, SSL3_CT_APPLICATIONDATA);
}


SP_STATUS WINAPI
Ssl3DecryptMessage( PSPContext         pContext,
                    PSPBuffer          pMessage,
                    PSPBuffer          pAppOutput)
{
    SP_STATUS                  pctRet;
    SPBuffer                   Clean;
    SPBuffer                   Encrypted;
	UCHAR 					   rgbDigest[MAX_DIGEST_LEN];
	PUCHAR 						pbMAC;
	DWORD  					dwLength, cbActualData;
	SWRAP					*pswrap = pMessage->pvBuffer;

    /* First determine the length of data, the length of padding,
     * and the location of data, and the location of MAC */
    cbActualData = pMessage->cbData;
    pMessage->cbData = sizeof(SWRAP); /* minimum amount of data we need */

    if(pMessage->cbData > cbActualData)
    	return PCT_INT_INCOMPLETE_MSG;

    dwLength = COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize);

    Clean.pvBuffer = Encrypted.pvBuffer = ((PUCHAR)pMessage->pvBuffer) + sizeof(SWRAP);
	Clean.cbBuffer = pMessage->cbBuffer - sizeof(SWRAP);

    pMessage->cbData += dwLength ;

    if(pMessage->cbData > cbActualData)
        return PCT_INT_INCOMPLETE_MSG;
    Encrypted.cbData = Clean.cbData = dwLength;	/* encrypted data size */

    pctRet = pContext->pSystem->Decrypt(pContext->pReadState, &Encrypted, &Clean);
    pbMAC = (PUCHAR)Clean.pvBuffer + (Clean.cbData - pContext->pCheck->cbCheckSum);
    DoSsl3MAC(pContext,
              &Clean,
              pContext->ReadMACState,
			  pContext->ReadMACState1,
              pContext->ReadCounter,
              pswrap->bCType,
              rgbDigest);

	pContext->ReadCounter++;
    if (memcmp(rgbDigest, pbMAC, pContext->pCheck->cbCheckSum))
    {
        DebugLog((DEB_WARN, "FINISHED Message: Checksum Invalid\n"));
        pctRet = SP_LOG_RESULT(PCT_INT_MSG_ALTERED);
    }

    if(pAppOutput->pvBuffer != Clean.pvBuffer)
        CopyMemory(pAppOutput->pvBuffer, Clean.pvBuffer, Clean.cbData);
    pAppOutput->cbData = Clean.cbData - pContext->pCheck->cbCheckSum;
    //THIS is mondo KLUDGE: We don't support Esc after the connection is
    //Established. This needs work from IE 3.0 side which we are postponing for POST beta
    //if(*(PBYTE)pMessage->pvBuffer == SSL3_CT_ALERT /*Escape*/)
    //	{
    //	pAppOutput->cbData = 0;
    //	}
    return(pctRet);
}

SP_STATUS
VerifyFinishMsg(
    PSPContext pContext,
    PBYTE       pbMsg,
    DWORD       cbMessage,
    BOOL        fClient
    )
{
    BYTE rgbDigest[MD5DIGESTLEN+A_SHA_DIGEST_LEN];
    SP_STATUS pctRet = PCT_ERR_OK;


    if(cbMessage < sizeof(SWRAP))
    {
        return PCT_INT_INCOMPLETE_MSG;
    }

    do
    {
//        SHSH *pshsh;
        SWRAP *pswrap = (SWRAP *)pbMsg;
        DWORD dwSize = COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize);
        FMWIRE *pfmwire = (FMWIRE *) pbMsg;

        if(cbMessage < dwSize)
        {
          return PCT_INT_INCOMPLETE_MSG;
        }


        if(*pbMsg != SSL3_CT_HANDSHAKE)
        {
            pctRet = SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
	        break;
        }

        if(pfmwire->typHS != SSL3_HS_FINISHED)
        {
            pctRet = SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
	        break;
        }
        dwSize = COMBINEBYTES(pfmwire->bcbMSB, pfmwire->bcbLSB);
        if(dwSize != (MD5DIGESTLEN+A_SHA_DIGEST_LEN))
        {
            pctRet = SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
	        break;
        }
        Ssl3BuildFinishMessage(pContext, rgbDigest, &rgbDigest[MD5DIGESTLEN], fClient);
        if (memcmp(rgbDigest, (BYTE *)pfmwire->rgbMD5,MD5DIGESTLEN+A_SHA_DIGEST_LEN))
        {
            DebugLog((DEB_WARN, "Finished MAC didn't matchChecksum Invalid\n"));
            pctRet = SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
	        break;
        }
        //Server's Finished message contains the finished message of client...
        //This is NOT used by the client, but it doesn't matter, we could put an if...
        UpdateHandshakeHash(pContext, (BYTE *)&pfmwire->typHS, dwSize + sizeof(SHSH), FALSE);
        return (PCT_ERR_OK);
    } while(TRUE);
    return(pctRet);
}

void BuildS3FinalFinish(PSPContext pContext, PUCHAR pb, BOOL fClient)
{
    BYTE rgbMd5Digest[MD5DIGESTLEN];
    BYTE rgbSHADigest[A_SHA_DIGEST_LEN];
    FMWIRE *pfmwire = (FMWIRE *) pb ;

    FillMemory((BYTE *)pfmwire, sizeof(FMWIRE), 0);
    Ssl3BuildFinishMessage(pContext, rgbMd5Digest, rgbSHADigest, fClient);
    CopyMemory(pfmwire->rgbMD5, rgbMd5Digest, MD5DIGESTLEN);
    CopyMemory(pfmwire->rgbSHA, rgbSHADigest, A_SHA_DIGEST_LEN);

    SetHandshake((BYTE *)&pfmwire->typHS,  SSL3_HS_FINISHED,  NULL, MD5DIGESTLEN+A_SHA_DIGEST_LEN);
    UpdateHandshakeHash(pContext, (BYTE *)&pfmwire->typHS, sizeof(FMWIRE) - sizeof(SWRAP), FALSE);
    SetWrap(pContext, pb,  SSL3_CT_HANDSHAKE, sizeof(FMWIRE) - sizeof(SWRAP), fClient);
}



void SetWrap(PSPContext pContext, PUCHAR pb, UCHAR bCType, DWORD wT, BOOL fClient)
	{
	SWRAP *pswrap = (SWRAP *)pb;
	DWORD wTT = wT + (FSsl3Cipher(fClient) ? pContext->pCheck->cbCheckSum : 0);

	FillMemory(pswrap, sizeof(SWRAP), 0);
    pswrap->bCType = bCType;
	pswrap->bMajor = 0x03;
	pswrap->bcbMSBSize = MSBOF(wTT);
	pswrap->bcbLSBSize = LSBOF(wTT);
    if(FSsl3Cipher(fClient))
    {
        SPBuffer   Clean;

        Clean.pvBuffer = pb;
        Clean.cbBuffer = wT+sizeof(SWRAP);
        Clean.cbData = wT;
        WrapSsl3Message(pContext, &Clean, bCType);
    }

}


void SetWrapNoEncrypt(PUCHAR pb, UCHAR bCType, DWORD wT)
	{
	SWRAP *pswrap = (SWRAP *)pb;

	FillMemory(pswrap, sizeof(SWRAP), 0);
    pswrap->bCType = bCType;
	pswrap->bMajor = 0x03;
	pswrap->bcbMSBSize = MSBOF(wT);
	pswrap->bcbLSBSize = LSBOF(wT);
    }


void SetHandshake(PUCHAR pb, BYTE bHandshake, PUCHAR pbData, WORD wSize)
{
    SHSH *pshsh = (SHSH *) pb;

    FillMemory(pshsh, sizeof(SHSH), 0);
    pshsh->typHS = bHandshake;
    pshsh->bcbMSB = MSBOF(wSize) ;
    pshsh->bcbLSB = LSBOF(wSize) ;
    if(NULL != pbData)
    {
        CopyMemory( pb + sizeof(SHSH) , pbData, wSize);
    }
}





void   UpdateHandshakeHash(PSPContext pContext, PUCHAR pb, DWORD dwcb, BOOL fInit)
{

    SP_BEGIN("UpdateHandshakeHash");

    DebugLog((DEB_TRACE, "Number of Bytes:%d\n", dwcb));

    if(fInit)
    {
        MD5Init((MD5_CTX *)pContext->MD5Handshake);
        A_SHAInit((A_SHA_CTX *)pContext->SHAHandshake);
    }

    MD5Update((MD5_CTX *)pContext->MD5Handshake, pb,  dwcb);
    A_SHAUpdate((A_SHA_CTX *)pContext->SHAHandshake, pb, dwcb);

#if DBG_HASHING
    DebugLog((DEB_TRACE, "  MD5:"));
    DbgDumpHexString(pContext->MD5Handshake,  sizeof(MD5_CTX));
    DebugLog((DEB_TRACE, "  SHA:"));
    DbgDumpHexString(pContext->SHAHandshake,  sizeof(A_SHA_CTX));

#endif

#if VERIFYHASH
    CopyMemory(&rgbF[ibF], pb, dwcb);
    ibF += dwcb;
#endif
    SP_END();
}


/*****************************************************************************/
//It better be allocated and of length 16+20
void ComputeCertVerifyHashes(PSPContext pContext, PBYTE pbHash)
{
    MD5_CTX Md5Hash;
    A_SHA_CTX ShaHash;
    BYTE rgbPad1[CB_PAD1];
    BYTE rgbPad2[CB_PAD1];
    DWORD ib;

    for(ib = 0 ; ib < CB_PAD1; ib++)
    {
    	rgbPad1[ib] = CH_SLL3_MD5_PAD_CHAR;
    	rgbPad2[ib] = CH_SSL3_SHA_PAD_CHAR;
    }

    //
    // CertificateVerify.signature.md5_hash = MD5(master_secret + pad2 +
    //		MD5(handshake_messages + master_secret + pad1));
    //

    // Make copy of handshake hash and add stuff to it
    CopyMemory(&Md5Hash, (BYTE *)pContext->MD5Handshake, sizeof(MD5_CTX));
    MD5Update(&Md5Hash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);
    MD5Update(&Md5Hash, rgbPad1, CB_SSL3_MD5_PAD);
    MD5Final(&Md5Hash);
    CopyMemory(pbHash, Md5Hash.digest, MD5DIGESTLEN);

    MD5Init(&Md5Hash);
    MD5Update(&Md5Hash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);
    MD5Update(&Md5Hash, rgbPad2, CB_SSL3_MD5_PAD);
    MD5Update(&Md5Hash, pbHash, MD5DIGESTLEN);
    MD5Final(&Md5Hash);
    CopyMemory(pbHash, Md5Hash.digest, MD5DIGESTLEN);

    //
    // CertificateVerify.signature.sha_hash = SHA(master_secret + pad2 +
    //		SHA(handshake_messages + master_secret + pad1));
    //

    // Make copy of handshake hash and add stuff to it
    CopyMemory(&ShaHash, (BYTE *)pContext->SHAHandshake, sizeof(A_SHA_CTX));
    A_SHAUpdate(&ShaHash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);
    A_SHAUpdate(&ShaHash, rgbPad1, CB_SSL3_SHA_PAD);
    A_SHAFinal(&ShaHash, pbHash+MD5DIGESTLEN);

    A_SHAInit(&ShaHash);
    A_SHAUpdate(&ShaHash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);
    A_SHAUpdate(&ShaHash, rgbPad2, CB_SSL3_SHA_PAD);
    A_SHAUpdate(&ShaHash, pbHash+MD5DIGESTLEN, A_SHA_DIGEST_LEN);
    A_SHAFinal(&ShaHash, pbHash+MD5DIGESTLEN);
}

//Assumes one cert and NOT cert chanin....
void BuildCertificateMessage(PBYTE pb, PBYTE rgbCert, DWORD dwCert)
    {
    	CERT *pcert = (CERT *) pb ;
        WORD wT = (WORD)dwCert + sizeof(CERT) - sizeof(SHSH);
        WORD wTT = wT - 3; //for the list....

    	FillMemory(pcert, sizeof(CERT), 0);
    	pcert->typHS = SSL3_HS_CERTIFICATE;
    	pcert->bcbMSBClist = MSBOF(wTT);
    	pcert->bcbLSBClist = LSBOF(wTT);
    	pcert->bcbMSB = MSBOF(wT) ;
    	pcert->bcbLSB = LSBOF(wT) ;
    	pcert->bcbMSBCert = MSBOF(dwCert) ;
    	pcert->bcbLSBCert = LSBOF(dwCert) ;
        CopyMemory( pcert->rgbCert, rgbCert, dwCert);
    }



SP_STATUS Ssl3HandleCCS(PSPContext pContext,
				   PUCHAR pb,
				   DWORD cbMessage,
				   BOOL fClient)
{

    SP_STATUS pctRet = PCT_ERR_OK;

    BYTE rgbCCS[CB_SLL3_CHANGE_CIPHER_SPEC] = SSL3_CHANGE_CIPHER_MSG;


    if(memcmp(rgbCCS, pb, CB_SLL3_CHANGE_CIPHER_SPEC))
    {
        pctRet = SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
        return (pctRet);
    }

    // We always zero out the read counter on receipt
    // of a change cipher spec message.
    pContext->ReadCounter = 0;

    pContext->RipeZombie->SessCiphSpec = pContext->PendingCiphSpec;
    pContext->RipeZombie->SessHashSpec = pContext->PendingHashSpec;
    pContext->RipeZombie->SessExchSpec = pContext->PendingExchSpec;

    pctRet = ContextInitCiphers(pContext);

    if(pctRet != PCT_ERR_OK)
    {
        return(pctRet);
    }

    pctRet = Ssl3MakeReadSessionKeys(pContext);

    if(pctRet != PCT_ERR_OK)
    {
        return(pctRet);
    }
    if(fClient)
    {
        pContext->State = SSL3_STATE_CLIENT_FINISH;
        pContext->wS3CipherSuiteClient = pContext->wS3pendingCipherSuite;
    }
    else
    {
        pContext->State = SSL3_STATE_SERVER_FINISH;
        pContext->wS3CipherSuiteServer = pContext->wS3pendingCipherSuite;
    }
    return(PCT_ERR_OK);
}


SP_STATUS
BuildCCSAndFinishMessage
(
PSPContext pContext,
PUCHAR pb,
DWORD cbMessage,
BOOL fClient
)
{
    SP_STATUS pctRet;
    PUCHAR pbT = pb + sizeof(SWRAP);


    *pbT = 0x1;
    SetWrap(pContext, pb,  SSL3_CT_CHANGE_CIPHER_SPEC, 1, fClient);
    pb += CB_SLL3_CHANGE_CIPHER_SPEC + (FSsl3Cipher(fClient) ? pContext->pCheck->cbCheckSum : 0) ;

    pContext->WriteCounter = 0 ;
    pContext->RipeZombie->SessCiphSpec = pContext->PendingCiphSpec;
    pContext->RipeZombie->SessHashSpec = pContext->PendingHashSpec;
    pContext->RipeZombie->SessExchSpec = pContext->PendingExchSpec;

    pctRet = ContextInitCiphers(pContext);

    if(pctRet != PCT_ERR_OK)
    {
        return(pctRet);
    }

    pctRet = Ssl3MakeWriteSessionKeys(pContext);

    if(pctRet != PCT_ERR_OK)
    {
        return(pctRet);
    }
    if(fClient)
    {
        pContext->wS3CipherSuiteClient = pContext->wS3pendingCipherSuite;
    }
    else
    {
        pContext->wS3CipherSuiteServer = pContext->wS3pendingCipherSuite;
    }

    BuildS3FinalFinish(pContext, pb, fClient);
    return(PCT_ERR_OK);
}



SP_STATUS
Ssl3SelectCipher    (
    PSPContext pContext,
    DWORD   dwCipher
)
{
    DWORD               i;

    for(i=0; i<Ssl3NumCipherRanks; i++) {
        if(NULL == CipherFromSpec(Ssl3CipherRank[i].Cipher,
            pContext->RipeZombie->fProtocol | (pContext->RipeZombie->fSGC ? SP_PROT_SGC : 0)))
        {
            continue;
        }
        if(NULL == HashFromSpec(Ssl3CipherRank[i].Hash, pContext->RipeZombie->fProtocol))
        {
            continue;
        }
        if(NULL == KeyExchangeFromSpec(Ssl3CipherRank[i].KeyExch, pContext->RipeZombie->fProtocol))
        {
            continue;
        }

        if (Ssl3CipherRank[i].Kind == dwCipher) {

            pContext->wS3pendingCipherSuite = (WORD)Ssl3CipherRank[i].Kind;
            pContext->PendingCiphSpec  = Ssl3CipherRank[i].Cipher;
            pContext->PendingHashSpec  = Ssl3CipherRank[i].Hash;
            pContext->PendingExchSpec  = Ssl3CipherRank[i].KeyExch;
            return PCT_ERR_OK;
        }
    }



    return(PCT_ERR_ILLEGAL_MESSAGE);
}


VOID
ReverseMemCopy(
    PUCHAR      Dest,
    PUCHAR      Source,
    ULONG       Size)
{
    PUCHAR  p;

    p = Dest + Size - 1;
    do
    {
	*p-- = *Source++;
    } while (p >= Dest);
}


#ifdef LATER
SP_STATUSBuildAlertMessage()
{
    UCHAR rgbCertAlert[] = { 21, 3, 0, 0, 2, 2, 40 };
    pCommOutput->cbData = 7;
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
    CopyMemory(pCommOutput->pvBuffer, rgbCertAlert, 7);
    return PCT_ERR_OK;
}

 #endif

PctPrivateKey *g_pSsl3Private; // = NULL;
PctPublicKey *g_pSsl3Public;   // = NULL;

BOOL FBuildSsl3ExchgKeys()
{
    BOOL fSucc = FALSE;
    DWORD dwPrivateSize;
    DWORD dwPublicSize;
    DWORD BitsCopy;

  	if(g_pSsl3Private != NULL && g_pSsl3Public != NULL)
  		return(TRUE);

    DebugLog((DEB_WARN, "Doing BuildSSl3ExchgKeys ...\n"));
    BitsCopy = 512; //We always generate 512 bits keys
    BSafeComputeKeySizes(&dwPublicSize, &dwPrivateSize, &BitsCopy);
    BitsCopy = 512;

    g_pSsl3Private = SPExternalAlloc(dwPrivateSize + sizeof(PctPrivateKey));
    if (NULL == g_pSsl3Private)
        goto Ret;

    g_pSsl3Public = SPExternalAlloc(dwPublicSize + sizeof(PctPublicKey));
    if (NULL == g_pSsl3Public)
        goto Ret;

    g_pSsl3Public->cbKey = dwPublicSize;
    g_pSsl3Private->cbKey = dwPrivateSize;

    if (BSafeMakeKeyPair(
		    (LPBSAFE_PUB_KEY) g_pSsl3Public->pKey,
		    (LPBSAFE_PRV_KEY) g_pSsl3Private->pKey,
		    BitsCopy))
	   return(TRUE);
Ret:
    	SPExternalFree(g_pSsl3Public);
    	SPExternalFree(g_pSsl3Private);
        return(fSucc);

}



/*****************************************************************************/
VOID ComputeServerExchangeHashes(
	PSPContext pContext,
    PBYTE pbServerParams,      // in
    INT   iServerParamsLen,    // in
    PBYTE pbMd5HashVal,        // out
    PBYTE pbShaHashVal)        // out
{
    MD5_CTX Md5Hash;
    A_SHA_CTX ShaHash;

    //
    // md5_hash = MD5(ClientHello.random + ServerHello.random + ServerParams);
    //
    // sha_hash = SHA(ClientHello.random + ServerHello.random + ServerParams);
    //

	//UNDONE: pbRandom should be another parameter rathe taking from pContext
    MD5Init(&Md5Hash);
    MD5Update(&Md5Hash, pContext->rgbS3CRandom, 32);
    MD5Update(&Md5Hash, pContext->rgbS3SRandom, 32);
    MD5Update(&Md5Hash, pbServerParams, iServerParamsLen);
    MD5Final(&Md5Hash);
    CopyMemory(pbMd5HashVal, Md5Hash.digest, 16);

    A_SHAInit(&ShaHash);
    A_SHAUpdate(&ShaHash, pContext->rgbS3CRandom, 32);
    A_SHAUpdate(&ShaHash, pContext->rgbS3SRandom, 32);
    A_SHAUpdate(&ShaHash, pbServerParams, iServerParamsLen);
    A_SHAFinal(&ShaHash, pbShaHashVal);
}

SP_STATUS
UnwrapSsl3Message
(
PSPContext pContext,
PSPBuffer pMsgInput
)
    {
    SPBuffer   Encrypted;
    SPBuffer   Clean;
    DWORD cbData;
    SP_STATUS pctRet;
    SWRAP *pswrap = (SWRAP *)pMsgInput->pvBuffer;


    cbData = COMBINEBYTES(pswrap->bcbMSBSize, pswrap->bcbLSBSize) -
                    pContext->pCheck->cbCheckSum;
    Encrypted.pvBuffer = pMsgInput->pvBuffer;
    Encrypted.cbBuffer = pMsgInput->cbBuffer;
    Encrypted.cbData = pMsgInput->cbData;
    Clean.pvBuffer = (PUCHAR)pMsgInput->pvBuffer + sizeof(SWRAP);
    pctRet = Ssl3DecryptMessage(pContext, &Encrypted, &Clean);
   	pswrap->bcbMSBSize = MSBOF(cbData);
   	pswrap->bcbLSBSize = LSBOF(cbData);
    return(pctRet);

    }


SP_STATUS
WrapSsl3Message
(
PSPContext pContext,
PSPBuffer pMsgInput,
UCHAR bContentType
)
    {
    SPBuffer   Encrypted;
    SPBuffer   Clean;
//    SP_STATUS pctRet;

    Clean.pvBuffer = (PUCHAR)pMsgInput->pvBuffer + sizeof(SWRAP);
    Clean.cbData = pMsgInput->cbData;
    Encrypted.pvBuffer = pMsgInput->pvBuffer;
    Encrypted.cbData = pMsgInput->cbData;
    Encrypted.cbBuffer = pMsgInput->cbData + pContext->pCheck->cbCheckSum + sizeof(SWRAP);
    return(Ssl3EncryptRaw(pContext, &Clean, &Encrypted, bContentType));
    }



SP_STATUS
ParseAlertMessage(
    PSPContext pContext,
    PUCHAR pbAlertMsg,
    DWORD cbMessage,
    PSPBuffer  pCommOutput
    )
{
    SP_STATUS   pctRet=PCT_ERR_OK;
    ALRT *      palrt = (ALRT *) pbAlertMsg;
    DWORD       cbMsg = COMBINEBYTES(palrt->bcbMSBSize, palrt->bcbLSBSize);
    if(cbMsg != 2)
    {
        return SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
    }

    if(palrt->bCType != SSL3_CT_ALERT)
    {
        return SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
    }

    if(palrt->bAlertLevel != SSL3_ALERT_WARNING  &&  palrt->bAlertLevel != SSL3_ALERT_FATAL)
    {
        return SP_LOG_RESULT(PCT_ERR_ILLEGAL_MESSAGE);
    }

    DebugLog((DEB_WARN, "AlertMessage, Alert Level -  %lx\n", (DWORD)palrt->bAlertLevel));
    DebugLog((DEB_WARN, "AlertMessage, Alert Description -  %lx\n", (DWORD)palrt->bAlertDesc));

    switch(palrt->bAlertDesc)
    {
        case SSL3_ALERT_CLOSE_NOTIFY:
             pctRet = PCT_INT_ILLEGAL_MSG;
           break;

        case SSL3_ALERT_UNEXPECTED_MESSAGE:
        case SSL3_ALERT_DECOMPRESSION_FAIL:
        case SSL3_ALERT_HANDSHAKE_FAILURE:
        case SSL3_ALERT_ILLEGAL_PARAMETER:
            pctRet = PCT_INT_ILLEGAL_MSG;
            break;


        case SSL3_ALERT_BAD_CERTIFICATE:
        case SSL3_ALERT_CERTIFICATE_REVOKED:
        case SSL3_ALERT_CERTIFICATE_EXPIRED:
        case SSL3_ALERT_CERTIFICATE_UNKNOWN:
            pctRet = PCT_INT_ILLEGAL_MSG;
            break;

        case SSL3_ALERT_NO_CERTIFICATE:
        case SSL3_ALERT_UNSUPPORTED_CERT:
            pctRet = PCT_INT_ILLEGAL_MSG;
            break;
    }

    return pctRet;
}


void BuildAlertMessage(PBYTE pbAlertMsg, UCHAR bAlertLevel, UCHAR bAlertDesc)
	{
    ALRT *palrt = (ALRT *) pbAlertMsg;

    FillMemory(palrt, sizeof(ALRT), 0);

	palrt->bCType = SSL3_CT_ALERT;
	palrt->bMajor = SSL3_CLIENT_VERSION_MSB;
//	palrt->bMinor = SSL3_CLIENT_VERSION_LSB; DONE by FillMemory
//	palrt->bcbMSBSize = 0; Done by FillMemory
	palrt->bcbLSBSize = 2;
	palrt->bAlertLevel = bAlertLevel;
	palrt->bAlertDesc  = bAlertDesc ;
	}

