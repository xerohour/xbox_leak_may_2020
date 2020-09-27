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


CipherSpec Pct1CipherRank[] = {
    SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128,
    SP_CIPHER_RC4 | SP_ENC_BITS_64  | SP_MAC_BITS_128,
    SP_CIPHER_RC4 | SP_ENC_BITS_40  | SP_MAC_BITS_128
};

const DWORD Pct1NumCipher = sizeof(Pct1CipherRank)/sizeof(CipherSpec);

/* available hashes, in order of preference */
HashSpec Pct1HashRank[] = {
    SP_HASH_MD5,
    SP_HASH_SHA
};
const DWORD Pct1NumHash = sizeof(Pct1HashRank)/sizeof(HashSpec);

CertSpec Pct1CertRank[] = {
    SP_CERT_X509
};
const DWORD Pct1NumCert = sizeof(Pct1CertRank)/sizeof(CertSpec);


SigSpec Pct1SigRank[] = {
    SP_SIG_RSA_MD5,
};
const DWORD Pct1NumSig = sizeof(Pct1SigRank)/sizeof(SigSpec);


ExchSpec Pct1ExchRank[] = {
   SP_EXCH_RSA_PKCS1
};

const DWORD Pct1NumExch = sizeof(Pct1ExchRank)/sizeof(ExchSpec);


SP_STATUS WINAPI
Pct1EncryptRaw( PSPContext          pContext,
                    PSPBuffer       pAppInput,
                    PSPBuffer       pCommOutput,
                    DWORD           dwFlags)
{
    SP_STATUS                  pctRet;
    DWORD                       cPadding;
    SPBuffer                   Clean;
    SPBuffer                   Encrypted;

    PCheckSumBuffer             pSum, pSubSum;
    HashBuf                     SumBuf, SubSumBuf;
    DWORD                       ReverseSequence;
    BOOL                        fEscape;
    DWORD                       cbHeader;

#if DBG
    DWORD           di;
    CHAR            KeyDispBuf[MASTER_KEY_SIZE*2+1];
#endif


    fEscape = (0 != (dwFlags & PCT1_ENCRYPT_ESCAPE));
    /* Generate Padding */
    Clean.cbData = pAppInput->cbData;
    Clean.cbBuffer = pAppInput->cbBuffer;
    Clean.pvBuffer = pAppInput->pvBuffer;
    cPadding = Clean.cbData & (pContext->pSystem->BlockSize - 1);
    if(Clean.cbData + cPadding > Clean.cbBuffer)
    {
        return PCT_INT_BUFF_TOO_SMALL;
    }

    GenerateRandomBits( (PUCHAR)Clean.pvBuffer + Clean.cbData, cPadding );

    Clean.cbData += cPadding;

    if(fEscape || (0 != cPadding)) {
        cbHeader = sizeof(PCT1_MESSAGE_HEADER_EX);
    }
    else
    {
        cbHeader = sizeof(PCT1_MESSAGE_HEADER);
    }
    if(pCommOutput->cbBuffer < (cbHeader + cPadding + Clean.cbData))
    {
        return PCT_INT_BUFF_TOO_SMALL;
    }

    Encrypted.pvBuffer = (PUCHAR)pCommOutput->pvBuffer+cbHeader;
    Encrypted.cbBuffer = pCommOutput->cbBuffer-cbHeader;


    DebugLog((DEB_TRACE, "Sealing message %x\n", pContext->WriteCounter));


    /* Begin Checksum Process */
    CloneHashBuf(SumBuf, pContext->WriteMACState, pContext->pCheck);
    CloneHashBuf(SubSumBuf, pContext->InitMACState, pContext->pCheck);
    pSum = (PCheckSumBuffer)SumBuf;
    pSubSum = (PCheckSumBuffer)SubSumBuf;

    pContext->pCheck->Sum( pSubSum, pAppInput->cbData, (PUCHAR)pAppInput->pvBuffer );

    ReverseSequence = htonl( pContext->WriteCounter );
    pContext->pCheck->Sum( pSubSum, sizeof(DWORD), (PUCHAR) &ReverseSequence );


    pctRet = pContext->pSystem->Encrypt( pContext->pWriteState,
                     pAppInput,
                     &Encrypted);

    /* Perform encryption to fill buffer */
    if(PCT_ERR_OK != pctRet)
    {
        return pctRet;
    }

    if(Encrypted.cbData + pContext->pCheck->cbCheckSum > Encrypted.cbBuffer)
    {
        return PCT_INT_BUFF_TOO_SMALL;
    }


    /* complete checksum, now that we know where to put it */
    pContext->pCheck->Finalize( pSubSum, (PUCHAR)Encrypted.pvBuffer + Encrypted.cbData);

    pContext->pCheck->Sum( pSum, pContext->pCheck->cbCheckSum,
                           (PUCHAR)Encrypted.pvBuffer + Encrypted.cbData);
    pContext->pCheck->Finalize( pSum, (PUCHAR)Encrypted.pvBuffer + Encrypted.cbData );

    Encrypted.cbData += pContext->pCheck->cbCheckSum;

    /* set sizes */
    if(fEscape || (0 != cPadding)) {
        if(Encrypted.cbData > 0x3fff)
        {
            return PCT_INT_DATA_OVERFLOW;
        }

        ((PUCHAR)pCommOutput->pvBuffer)[0]= (UCHAR)(0x3f & (Encrypted.cbData>>8));
        if(fEscape)
        {
            ((PUCHAR)pCommOutput->pvBuffer)[0] |= 0x40;
        }

        ((PUCHAR)pCommOutput->pvBuffer)[1]= (UCHAR)(0xff & Encrypted.cbData);
        ((PUCHAR)pCommOutput->pvBuffer)[2]= (UCHAR)cPadding;

    } else {
        if(Encrypted.cbData > 0x7fff)
        {
            return PCT_INT_DATA_OVERFLOW;
        }
        ((PUCHAR)pCommOutput->pvBuffer)[0]= (UCHAR)(0x7f & (Encrypted.cbData>>8)) | 0x80;
        ((PUCHAR)pCommOutput->pvBuffer)[1]= (UCHAR)(0xff & Encrypted.cbData);
    }

    pCommOutput->cbData = Encrypted.cbData + cbHeader;

#if DBG
    for(di=0;di<MASTER_KEY_SIZE;di++)
        wsprintf(KeyDispBuf+(di*2), "%2.2x",
                ((BYTE *)Encrypted.pvBuffer+Encrypted.cbData-pContext->pCheck->cbCheckSum)[di]);
    DebugLog((DEB_TRACE, "  MAC\t%s\n", KeyDispBuf));
#endif

    pContext->WriteCounter ++ ;

    return( PCT_ERR_OK );

}
SP_STATUS WINAPI
Pct1EncryptMessage( PSPContext      pContext,
                    PSPBuffer       pAppInput,
                    PSPBuffer       pCommOutput)
{
    return Pct1EncryptRaw(pContext, pAppInput, pCommOutput,0);
}

SP_STATUS WINAPI
Pct1DecryptMessage( PSPContext          pContext,
                   PSPBuffer          pMessage,
                   PSPBuffer          pAppOutput)
{

    SP_STATUS              pctRet;
    DWORD                   cPadding;
    DWORD                   dwLength;
    PUCHAR                  pbMAC;
    DWORD                   cbData;

    SPBuffer               Encrypted;

    PCheckSumBuffer         pSum, pSubSum;
    HashBuf                 SumBuf, SubSumBuf;
    DWORD                   ReverseSequence;
    DWORD                   cbActualData;
    UCHAR                   Digest[16];
#if DBG
    DWORD           di;
    CHAR            KeyDispBuf[MASTER_KEY_SIZE*2+1];
#endif

    /* First determine the length of data, the length of padding,
     * and the location of data, and the location of MAC */
    cbActualData = pMessage->cbData;
    pMessage->cbData = 2; /* minimum amount of data we need */

    if(pMessage->cbData > cbActualData)
    {
        return PCT_INT_INCOMPLETE_MSG;
    }

    if(  ((PUCHAR)pMessage->pvBuffer)[0]&0x80 ) {
        /* 2 byte header */
        cPadding = 0;
        dwLength = ((((PUCHAR)pMessage->pvBuffer)[0]&0x7f)<< 8) | ((PUCHAR)pMessage->pvBuffer)[1];
        Encrypted.pvBuffer = ((PUCHAR)pMessage->pvBuffer) + 2;
    } else {
        pMessage->cbData++;
        if(pMessage->cbData > cbActualData)
        {
            return PCT_INT_INCOMPLETE_MSG;
        }
        /* 3 byte header */
        cPadding = ((PUCHAR)pMessage->pvBuffer)[2];
        dwLength = ((((PUCHAR)pMessage->pvBuffer)[0]&0x3f)<< 8) | ((PUCHAR)pMessage->pvBuffer)[1];
        Encrypted.pvBuffer = ((PUCHAR)pMessage->pvBuffer) + 3;
    }

    /* Now we know how mutch data we will eat, so set cbData on the Input to be that size */
    pMessage->cbData += dwLength;
    /* do we have enough bytes for the reported data */
    if(pMessage->cbData > cbActualData)
    {
        return PCT_INT_INCOMPLETE_MSG;
    }

    /* do we have engough data for our checksum */
    if(dwLength < pContext->pCheck->cbCheckSum)
    {
        return SP_LOG_RESULT(PCT_INT_MSG_ALTERED);
    }
    Encrypted.cbData = dwLength - pContext->pCheck->cbCheckSum; /* encrypted data size */
    pbMAC = (PUCHAR)Encrypted.pvBuffer + Encrypted.cbData;            /* location of MAC */
    cbData = Encrypted.cbData - cPadding;                       /* size of actual data */

    /* check to see if we have a block size violation */
    if(Encrypted.cbData % pContext->pSystem->BlockSize)
    {
        return SP_LOG_RESULT(PCT_INT_MSG_ALTERED);
    }

    Encrypted.cbBuffer = Encrypted.cbData;

    /* Decrypt */
    pctRet = pContext->pSystem->Decrypt( pContext->pReadState,
                                &Encrypted,
                                pAppOutput);
    if(PCT_ERR_OK != pctRet)
    {
        return pctRet;
    }

    /* Validate MAC: */


    CloneHashBuf(SumBuf, pContext->ReadMACState, pContext->pCheck);
    CloneHashBuf(SubSumBuf, pContext->InitMACState, pContext->pCheck);

    pSum = (PCheckSumBuffer)SumBuf;
    pSubSum = (PCheckSumBuffer)SubSumBuf;

    pContext->pCheck->Sum( pSubSum, pAppOutput->cbData, pAppOutput->pvBuffer);

    ReverseSequence = htonl( pContext->ReadCounter );

    DebugLog((DEB_TRACE, "Unsealing message %x\n", pContext->ReadCounter));

    pContext->pCheck->Sum( pSubSum, sizeof(DWORD), (PUCHAR) &ReverseSequence );
    pContext->pCheck->Finalize( pSubSum, Digest );
    pContext->pCheck->Sum( pSum, pContext->pCheck->cbCheckSum, Digest );
    pContext->pCheck->Finalize( pSum, Digest );

    pContext->ReadCounter++;

#if DBG

    for(di=0;di<MASTER_KEY_SIZE;di++)
        wsprintf(KeyDispBuf+(di*2), "%2.2x",
                pbMAC[di]);
    DebugLog((DEB_TRACE, "  Incoming MAC\t%s\n", KeyDispBuf));

    for(di=0;di<MASTER_KEY_SIZE;di++)
        wsprintf(KeyDispBuf+(di*2), "%2.2x",
                Digest[di]);
    DebugLog((DEB_TRACE, "  Computed MAC\t%s\n", KeyDispBuf));

#endif

    if (memcmp( Digest, pbMAC, pContext->pCheck->cbCheckSum ) )
    {
       return SP_LOG_RESULT(PCT_INT_MSG_ALTERED);
    }

    pAppOutput->cbData = cbData;

    return( PCT_ERR_OK );
}

SP_STATUS
PctComputeKey(PCheckSumBuffer *ppHash,
              PSPContext  pContext,
              UCHAR *Buffer,
              CONST CHAR * pConst,
              DWORD dwCLen,
              DWORD fFlags)
{
    DWORD               BufferLen;

#if DBG
#if DBG_WATCH_KEYS
    DWORD   di;
    CHAR    BufDispBuf[16 * 3 + 5];
#endif
#endif
    SP_BEGIN("PctComputeKey");


    BufferLen = 0;

    Buffer[BufferLen] = 1;
    BufferLen += 1;

    if (!(fFlags & PCT_MAKE_MAC))
    {
        /* add the first constant */
        CopyMemory(Buffer+BufferLen, pConst, dwCLen);
        BufferLen += dwCLen;
    }

    /* add the master key */
    CopyMemory(Buffer+BufferLen, pContext->RipeZombie->pMasterKey, MASTER_KEY_SIZE );
    BufferLen += MASTER_KEY_SIZE;

    /* repeat the constant */
    CopyMemory(Buffer+BufferLen, pConst, dwCLen);
    BufferLen += dwCLen;

    /* add the connection id */
    CopyMemory(Buffer+BufferLen, pContext->pConnectionID,
               pContext->cbConnectionID);
    BufferLen += pContext->cbConnectionID;

    /* repeat the constant */
    CopyMemory(Buffer+BufferLen, pConst, dwCLen);
    BufferLen += dwCLen;

    if (fFlags & PCT_USE_CERT)
    {
        /* add in the certificate */
        CopyMemory(Buffer+BufferLen, (UCHAR *)pContext->RipeZombie->pServerCert->pRawCert,
                   pContext->RipeZombie->pServerCert->cbRawCert);
        BufferLen += pContext->RipeZombie->pServerCert->cbRawCert;

        /* repeat the constant */
        CopyMemory(Buffer+BufferLen, pConst, dwCLen);
        BufferLen += dwCLen;
    }

    /* add the challenge */
    CopyMemory(Buffer+BufferLen, (UCHAR *)pContext->pChallenge,
               pContext->cbChallenge);
    BufferLen += pContext->cbChallenge;

    /* again, repeat the constant */
    CopyMemory(Buffer+BufferLen, pConst, dwCLen);
    BufferLen += dwCLen;

#if DBG

#if DBG_WATCH_KEYS

    DebugLog((DEB_TRACE, "Buffer:\n"));

    BufDispBuf[0] = 0;
    for(di=0;di<BufferLen;di++)
    {
        wsprintf(BufDispBuf+((di % 16)*3), "%2.2x ", Buffer[di]);

        if ((di & 15) == 15)
        {
            DebugLog((DEB_TRACE, "\t%s\n", BufDispBuf));
            BufDispBuf[0] = 0;
        }
    }

    DebugLog((DEB_TRACE, "\t%s\n", BufDispBuf));

#endif

#endif

    /* hash the buffer */
    pContext->pCheck->Sum( *ppHash, BufferLen, Buffer );
    SP_RETURN(PCT_ERR_OK);
}

SP_STATUS
Pct1MakeSessionKeys(
    PSPContext  pContext)
{
    SP_STATUS           pctRet;
    PCheckSumBuffer     CWriteHash, SWriteHash, CMacHash, SMacHash;
    PCheckSumBuffer     CExportKey, SExportKey;
    UCHAR               pWriteKey[MASTER_KEY_SIZE], pReadKey[MASTER_KEY_SIZE];
    UCHAR               Buffer[DERIVATION_BUFFER_SIZE];
    DWORD               BufferLen, MaxBufferLen, dwKeyLen;
    HashBuf             CWriteHB, SWriteHB, CMACHB, SMACHB, CExpHB, SExpHB;
#if DBG
    DWORD       i;
    CHAR        KeyDispBuf[MASTER_KEY_SIZE*2+1];
#endif

    SP_BEGIN("PctMakeSessionKeys");

    if (!pContext->InitMACState)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    InitHashBuf(CWriteHB, pContext);
    InitHashBuf(SWriteHB, pContext);
    InitHashBuf(CMACHB, pContext);
    InitHashBuf(SMACHB, pContext);
    InitHashBuf(CExpHB, pContext);
    InitHashBuf(SExpHB, pContext);

    CWriteHash = (PCheckSumBuffer)CWriteHB;
    SWriteHash = (PCheckSumBuffer)SWriteHB;
    CMacHash = (PCheckSumBuffer)CMACHB;
    SMacHash = (PCheckSumBuffer)SMACHB;
    CExportKey = (PCheckSumBuffer)CExpHB;
    SExportKey = (PCheckSumBuffer)SExpHB;


#if DBG
    DebugLog((DEB_TRACE, "Making session keys\n", KeyDispBuf));

    for(i=0;i<MASTER_KEY_SIZE;i++)
        wsprintf(KeyDispBuf+(i*2), "%2.2x",
                pContext->pConnectionID[i]);
    DebugLog((DEB_TRACE, "  ConnId\t%s\n", KeyDispBuf));

    for(i=0;i<MASTER_KEY_SIZE;i++)
        wsprintf(KeyDispBuf+(i*2), "%2.2x",
                pContext->RipeZombie->pServerCert->pRawCert[i]);
    DebugLog((DEB_TRACE, "  Cert\t%s\n", KeyDispBuf));

    for(i=0;i<MASTER_KEY_SIZE;i++)
        wsprintf(KeyDispBuf+(i*2), "%2.2x", (UCHAR)pContext->pChallenge[i]);
    DebugLog((DEB_TRACE, "  Challenge \t%s\n", KeyDispBuf));

    for(i=0;i<pContext->RipeZombie->cbClearKey;i++)
        wsprintf(KeyDispBuf+(i*2), "%2.2x", (UCHAR)pContext->RipeZombie->pClearKey[i]);
    DebugLog((DEB_TRACE, "  ClearKey \t%s\n", KeyDispBuf));

#endif


    MaxBufferLen = 1 +      /* initial number */
                   PCT_MAX_NUM_SEP * PCT_MAX_SEP_LEN +
                   MASTER_KEY_SIZE +
                   PCT_SESSION_ID_SIZE +
                   PCT_SESSION_ID_SIZE +
                   pContext->RipeZombie->pServerCert->cbRawCert;

    if (MaxBufferLen > DERIVATION_BUFFER_SIZE)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_DATA_OVERFLOW));
    }

    /* compute the ClientWriteKey */

    pctRet = PctComputeKey( &CWriteHash, pContext, Buffer, PCT_CONST_CWK,
                   PCT_CONST_CWK_LEN, PCT_USE_CERT);

    if(PCT_ERR_OK != pctRet)
    {
        SP_RETURN(pctRet);
    }


    /* compute the ServerWriteKey */

    pctRet = PctComputeKey( &SWriteHash, pContext, Buffer, PCT_CONST_SWK,
                   PCT_CONST_SWK_LEN, 0);
    if(PCT_ERR_OK != pctRet)
    {
        SP_RETURN(pctRet);
    }

    /* compute the ClientMacKey */

    pctRet = PctComputeKey( &CMacHash, pContext, Buffer, PCT_CONST_CMK,
                   PCT_CONST_CMK_LEN, PCT_USE_CERT | PCT_MAKE_MAC);

    if(PCT_ERR_OK != pctRet)
    {
        SP_RETURN(pctRet);
    }

    /* compute the ServerMacKey */

    pctRet = PctComputeKey( &SMacHash, pContext, Buffer, PCT_CONST_SMK,
                   PCT_CONST_SMK_LEN, PCT_MAKE_MAC);

    if(PCT_ERR_OK != pctRet)
    {
        SP_RETURN(pctRet);
    }

    /* find bit strength of cipher */
    dwKeyLen = (pContext->RipeZombie->SessCiphSpec & SP_CIPHER_STRENGTH) >> SP_CSTR_POS;

    /* convert to bytes */
    dwKeyLen = dwKeyLen / 8;

    if (dwKeyLen < (pContext->pCheck->cbCheckSum))
    {
        /* chop the encryption keys down to selected length */

#if DBG
        DebugLog((DEB_TRACE, "Chopping down client write keys\n", KeyDispBuf));
#endif
        /* compute the standard length clientwritekey */

        BufferLen = 0;

        Buffer[BufferLen] = 1;
        BufferLen += 1;

        CopyMemory(Buffer+BufferLen, PCT_CONST_SLK, PCT_CONST_SLK_LEN);
        BufferLen += PCT_CONST_SLK_LEN;

        pContext->pCheck->Finalize( CWriteHash, Buffer+BufferLen );

        BufferLen += dwKeyLen;

        CopyMemory(Buffer+BufferLen, PCT_CONST_SLK, PCT_CONST_SLK_LEN);
        BufferLen += PCT_CONST_SLK_LEN;

        CopyMemory(Buffer+BufferLen, pContext->RipeZombie->pClearKey, pContext->RipeZombie->cbClearKey);
        BufferLen += pContext->RipeZombie->cbClearKey;

        InitHashBuf(CWriteHB, pContext);
        pContext->pCheck->Sum( CWriteHash, BufferLen, Buffer );

        /* compute the standard length serverwritekey */

#if DBG
        DebugLog((DEB_TRACE, "Chopping down server write keys\n", KeyDispBuf));
#endif

        BufferLen = 0;

        Buffer[BufferLen] = 1;
        BufferLen += 1;

        CopyMemory(Buffer+BufferLen, PCT_CONST_SLK, PCT_CONST_SLK_LEN);
        BufferLen += PCT_CONST_SLK_LEN;

        pContext->pCheck->Finalize( SWriteHash, Buffer+BufferLen );

        BufferLen += dwKeyLen;

        CopyMemory(Buffer+BufferLen, PCT_CONST_SLK, PCT_CONST_SLK_LEN);
        BufferLen += PCT_CONST_SLK_LEN;

        CopyMemory(Buffer+BufferLen, pContext->RipeZombie->pClearKey, pContext->RipeZombie->cbClearKey);
        BufferLen += pContext->RipeZombie->cbClearKey;

        InitHashBuf(SWriteHB, pContext);
        pContext->pCheck->Sum( SWriteHash, BufferLen, Buffer );
    }

    pContext->KeySize = MASTER_KEY_SIZE;

    if (pContext->Flags & CONTEXT_FLAG_CLIENT)
    {
        pContext->pCheck->Finalize( SWriteHash, pReadKey );
        pContext->pCheck->Finalize( CWriteHash, pWriteKey );
        pContext->pCheck->Finalize( SMacHash, pContext->ReadMACKey );
        pContext->pCheck->Finalize( CMacHash, pContext->WriteMACKey );
    }
    else
    {
        pContext->pCheck->Finalize( SWriteHash, pWriteKey );
        pContext->pCheck->Finalize( CWriteHash, pReadKey );
        pContext->pCheck->Finalize( SMacHash, pContext->WriteMACKey );
        pContext->pCheck->Finalize( CMacHash, pContext->ReadMACKey );
    }

    InitHashBuf(pContext->RdMACBuf, pContext);
    InitHashBuf(pContext->WrMACBuf, pContext);

    // Note, we truncuate the MACing keys down to the negotiated key size
    pContext->ReadMACState = (PCheckSumBuffer)pContext->RdMACBuf;

    pContext->pCheck->Sum( pContext->ReadMACState, pContext->KeySize,
               pContext->ReadMACKey);

    pContext->WriteMACState = (PCheckSumBuffer)pContext->WrMACBuf;

    pContext->pCheck->Sum( pContext->WriteMACState, pContext->KeySize,
               pContext->WriteMACKey);

#if DBG

    for(i=0;i<MASTER_KEY_SIZE;i++)
        wsprintf(KeyDispBuf+(i*2), "%2.2x", pContext->RipeZombie->pMasterKey[i]);
    DebugLog((DEB_TRACE, "  MasterKey \t%s\n", KeyDispBuf));

    for(i=0;i<CONTEXT_KEY_SIZE;i++)
        wsprintf(KeyDispBuf+(i*2), "%2.2x", pReadKey[i]);
    DebugLog((DEB_TRACE, "    ReadKey\t%s\n", KeyDispBuf));

    for(i=0;i<CONTEXT_KEY_SIZE;i++)
        wsprintf(KeyDispBuf+(i*2), "%2.2x", pContext->ReadMACKey[i]);
    DebugLog((DEB_TRACE, "     MACKey\t%s\n", KeyDispBuf));

    for(i=0;i<CONTEXT_KEY_SIZE;i++)
        wsprintf(KeyDispBuf+(i*2), "%2.2x", pWriteKey[i]);
    DebugLog((DEB_TRACE, "    WriteKey\t%s\n", KeyDispBuf));

    for(i=0;i<CONTEXT_KEY_SIZE;i++)
        wsprintf(KeyDispBuf+(i*2), "%2.2x", pContext->WriteMACKey[i]);
    DebugLog((DEB_TRACE, "     MACKey\t%s\n", KeyDispBuf));

#endif

    if (pContext->pSystem->Initialize(  pReadKey,
                                        CONTEXT_KEY_SIZE,
                                        &pContext->pReadState ) )
    {
        if (pContext->pSystem->Initialize(  pWriteKey,
                                            CONTEXT_KEY_SIZE,
                                            &pContext->pWriteState) )
        {

            SP_RETURN(PCT_ERR_OK);
        }
        pctRet = SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
        pContext->pSystem->Discard( &pContext->pReadState );
    }

    SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
}

SP_STATUS WINAPI Pct1DecryptHandler(PSPContext  pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pAppOutput)
{
    SP_STATUS      pctRet= 0;
    BOOL           fEscape;
    PPCT1_CLIENT_HELLO pHello;
    if(pCommInput->cbData > 0) {
        /* first, we'll handle incoming data packets */
        if((pContext->State == SP_STATE_CONNECTED) && (pContext->Decrypt))
        {
            fEscape = (((*(PUCHAR)pCommInput->pvBuffer) & 0xc0) == 0x40);
            /* BUGFIX:  IE 3.0 and 3.0a incorrectly respond to a REDO request
             * by just sending a PCT1 client hello, instead of another REDO.
             * We therefore look at the incomming message and see if it
             * looks like a PCT1 client hello.
             */
            pHello = (PPCT1_CLIENT_HELLO)pCommInput->pvBuffer;

            if((pCommInput->cbData >= 5) &&
               (pHello->MessageId == PCT1_MSG_CLIENT_HELLO) &&
               (pHello->VersionMsb == MSBOF(PCT_VERSION_1)) &&
               (pHello->VersionLsb == LSBOF(PCT_VERSION_1)) &&
               (pHello->OffsetMsb  == MSBOF(PCT_CH_OFFSET_V1)) &&
               (pHello->OffsetLsb  == LSBOF(PCT_CH_OFFSET_V1)))
            {
                // This looks a lot like a client hello
                 /* InitiateRedo */
                pAppOutput->cbData = 0;
                pCommInput->cbData = 0;

                pContext->State = PCT1_STATE_RENEGOTIATE;
;
                return SP_LOG_RESULT(PCT_INT_RENEGOTIATE);
           }

            if(PCT_ERR_OK ==
               (pctRet = pContext->Decrypt(pContext,
                                           pCommInput,   /* message */
                                           pAppOutput /* Unpacked Message */
                                )))
            {
                /* look for escapes */
                if(fEscape)
                {
                    if(pAppOutput->cbData < 1)
                    {
                        return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
                    }
                    /* The first byte of the decrypt buffer is the escape code */
                    switch(*(PUCHAR)pAppOutput->pvBuffer)
                    {
                        case PCT1_ET_REDO_CONN:
                        {
                            /* InitiateRedo */
                            if(pAppOutput->cbData != 1)
                            {
                                return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
                            }
                            pContext->State = PCT1_STATE_RENEGOTIATE;
                            pAppOutput->cbData = 0;
                            return SP_LOG_RESULT(PCT_INT_RENEGOTIATE);
                        }
                        case PCT1_ET_OOB_DATA:
                            /* HandleOOB */
                        default:
                            /* Unknown escape, generate error */
                            pctRet = SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
                            /* Disconnect */
                            break;
                    }

                }
            }
            return (pctRet);

        }
        else
        {
			return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
		}
	}
    return PCT_INT_INCOMPLETE_MSG;
}

SP_STATUS Pct1GenerateError(PSPContext  pContext,
                              PSPBuffer  pCommOutput,
                              SP_STATUS  pError,
                              PSPBuffer  pErrData)
{
    Pct1Error            XmitError;

    /* Only pack up an error if we are allowed to return errors */
    if(!(pContext->Flags & CONTEXT_FLAG_EXT_ERR)) return pError;

    XmitError.Error = pError;
    XmitError.ErrInfoLen = 0;
    XmitError.ErrInfo = NULL;

    if(pErrData) {
        XmitError.ErrInfoLen = pErrData->cbData;
        XmitError.ErrInfo = pErrData->pvBuffer;
    }
    Pct1PackError(&XmitError,
                 pCommOutput);
    return pError;
}

/* session key computation */


SP_STATUS Pct1HandleError(PSPContext  pContext,
                          PSPBuffer  pCommInput,
                          PSPBuffer  pCommOutput)
{
    pCommOutput->cbData = 0;
    return(((PPCT1_ERROR)pCommInput->pvBuffer)->ErrorMsb << 8 )|  ((PPCT1_ERROR)pCommInput->pvBuffer)->ErrorLsb;
}

SP_STATUS Pct1BeginVerifyPrelude(PSPContext pContext,
                                 PUCHAR     pClientHello,
                                 DWORD      cbClientHello,
                                 PUCHAR     pServerHello,
                                 DWORD      cbServerHello)
{

    // Hash( "cvp", CLIENT_HELLO, SERVER_HELLO);

    InitHashBuf(pContext->pVerifyPrelude, pContext);

    pContext->pCheck->Sum((PCheckSumBuffer)(pContext->pVerifyPrelude),
                                            PCT_CONST_VP_LEN,
                                            (PUCHAR)PCT_CONST_VP);

    pContext->pCheck->Sum((PCheckSumBuffer)(pContext->pVerifyPrelude),
                                            cbClientHello,
                                            (PUCHAR)pClientHello);

    pContext->pCheck->Sum((PCheckSumBuffer)(pContext->pVerifyPrelude),
                                            cbServerHello,
                                            pServerHello);

    return PCT_ERR_OK;

}

SP_STATUS Pct1EndVerifyPrelude(PSPContext pContext,
                               PUCHAR     VerifyPrelude,
                               DWORD *    pcbVerifyPrelude
                               )
{

    HashBuf             SumBuf;
    PCheckSumBuffer     pSum;

    pContext->pCheck->Finalize(pContext->pVerifyPrelude,
                               VerifyPrelude);


    InitHashBuf(SumBuf, pContext);
    pSum = (PCheckSumBuffer)SumBuf;
    /* Finish up hashing the verify prelude */
    if(pContext->RipeZombie->fProtocol & SP_PROT_PCT1_SERVER)
    {
        pContext->pCheck->Sum(pSum, PCT1_MASTER_KEY_SIZE, pContext->ReadMACKey);
    }
    else
    {
        pContext->pCheck->Sum(pSum, PCT1_MASTER_KEY_SIZE, pContext->WriteMACKey);
    }
    pContext->pCheck->Sum(pSum,
                          pContext->pCheck->cbCheckSum,
                          VerifyPrelude);

    pContext->pCheck->Finalize(pSum, VerifyPrelude);

    *pcbVerifyPrelude = pContext->pCheck->cbCheckSum;

    return PCT_ERR_OK;

}


SP_STATUS Pct1BuildRestartResponse(PSPContext pContext,
                                   PUCHAR     Response,
                                   DWORD *    pcbResponse
                               )
{

    HashBuf             SumBuf, SubSumBuf;
    PCheckSumBuffer     pSum, pSubSum;


    /* calculate the response */
    // Don't suck in C runtimes in retail build:
	// printf("Calculating Response\n");

    if(pContext->RipeZombie->fProtocol & SP_PROT_PCT1_SERVER)
    {
        CloneHashBuf(SumBuf, pContext->WriteMACState, pContext->pCheck);
    }
    else
    {
         CloneHashBuf(SumBuf, pContext->ReadMACState, pContext->pCheck);
    }
    InitHashBuf(SubSumBuf, pContext);

    pSum = (PCheckSumBuffer)SumBuf;
    pSubSum = (PCheckSumBuffer)SubSumBuf;

    pContext->pCheck->Sum(pSubSum, PCT_CONST_RESP_LEN, (PUCHAR)PCT_CONST_RESP);
    pContext->pCheck->Sum(pSubSum,
                          pContext->cbChallenge,
                          pContext->pChallenge);

    pContext->pCheck->Sum(pSubSum,
                          pContext->cbConnectionID,
                          pContext->pConnectionID);

    pContext->pCheck->Sum(pSubSum,
                          pContext->RipeZombie->cbSessionID,
                          pContext->RipeZombie->SessionID);

    pContext->pCheck->Finalize(pSubSum, Response);

    pContext->pCheck->Sum(pSum,
                          pContext->pCheck->cbCheckSum,
                          Response);

    pContext->pCheck->Finalize(pSum, Response);

    *pcbResponse = pContext->pCheck->cbCheckSum;

    return PCT_ERR_OK;

}
