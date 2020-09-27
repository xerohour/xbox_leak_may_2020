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
#include <ssl2msg.h>
#include <ssl2prot.h>
#include <rc4.h>

Ssl2CipherMap Ssl2CipherRank[] =
{
    {SSL_CK_RC4_128_WITH_MD5, SP_HASH_MD5 , SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1},
    {SSL_CK_RC4_128_FINANCE64_WITH_MD5, SP_HASH_MD5 , SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1},
    {SSL_CK_RC4_128_EXPORT40_WITH_MD5,SP_HASH_MD5 , SP_CIPHER_RC4 | SP_ENC_BITS_40 | SP_MAC_BITS_128, SP_EXCH_RSA_PKCS1},
};
const DWORD Ssl2NumCipherRanks = sizeof(Ssl2CipherRank)/sizeof(Ssl2CipherMap);

CertSpec Ssl2CertRank[] = {
    SP_CERT_X509
};
const DWORD Ssl2NumCert = sizeof(Ssl2CertRank)/sizeof(CertSpec);

SP_STATUS WINAPI
Ssl2DecryptHandler(
    PSPContext pContext,
    PSPBuffer pCommInput,
    PSPBuffer pAppOutput)
{
    SP_STATUS pctRet = PCT_ERR_OK;

    if (pCommInput->cbData > 0)
    {
        // First, we'll handle incoming data packets:

        if ((pContext->State & SP_STATE_CONNECTED) && pContext->Decrypt)
        {
            pctRet = pContext->Decrypt(
		                    pContext,
		                    pCommInput,		// message
		                    pAppOutput);	// Unpacked Message
            if (PCT_ERR_OK == pctRet)
            {
                /* look for escapes */
            }
            return(pctRet);
        }
        else
        {
            return(PCT_INT_ILLEGAL_MSG);
        }
    }
    return (PCT_INT_INCOMPLETE_MSG);
}


SP_STATUS WINAPI
Ssl2EncryptMessage(
    PSPContext     pContext,
    PSPBuffer       pAppInput,
    PSPBuffer       pCommOutput)
{
    SP_STATUS                  pctRet;
    DWORD                      cPadding, cPad2;
    SPBuffer                   Clean;
    SPBuffer                   Encrypted;

    PCheckSumBuffer            pSum;
    HashBuf                    SumBuf;
    DWORD                      ReverseSequence;

#if DBG
    DWORD           di;
    CHAR            KeyDispBuf[SSL2_MASTER_KEY_SIZE*2+1];
#endif

    SP_BEGIN("Ssl2EncryptMessage");

    /* Estimate if we have padding or not */
    Clean.cbData = pAppInput->cbData+pContext->pCheck->cbCheckSum;
    cPadding = Clean.cbData & (pContext->pSystem->BlockSize - 1);

    Clean.cbData += cPadding;

    if(cPadding) {
        if(pCommOutput->cbBuffer + Clean.cbData + cPadding < 3)
        {
            SP_RETURN(PCT_INT_BUFF_TOO_SMALL);
        }
        Clean.pvBuffer = Encrypted.pvBuffer = (PUCHAR)pCommOutput->pvBuffer+3;
        Clean.cbBuffer = Encrypted.cbBuffer = pCommOutput->cbBuffer-3;

    }
    else
    {
        if(pCommOutput->cbBuffer + Clean.cbData + cPadding< 2)
        {
            SP_RETURN(PCT_INT_BUFF_TOO_SMALL);
        }
        Clean.pvBuffer = Encrypted.pvBuffer = (PUCHAR)pCommOutput->pvBuffer+2;
        Clean.cbBuffer = Encrypted.cbBuffer = pCommOutput->cbBuffer-2;
    }

    DebugLog((DEB_TRACE, "Sealing message %x\n", pContext->WriteCounter));


    /* Begin Checksum Process */
	/* Clone Hash(SECRET... */
    CloneHashBuf(SumBuf, pContext->WriteMACState, pContext->pCheck);
    pSum = (PCheckSumBuffer)SumBuf;
    /* add data */
    pContext->pCheck->Sum( pSum, pAppInput->cbData, (PUCHAR)pAppInput->pvBuffer );

    ReverseSequence = htonl( pContext->WriteCounter );
    /* add count */
    pContext->pCheck->Sum( pSum, sizeof(DWORD), (PUCHAR) &ReverseSequence );

   /* Move data out of the way if necessary */
    if((PUCHAR)Clean.pvBuffer+pContext->pCheck->cbCheckSum != (PUCHAR)pAppInput->pvBuffer)
    {
        DebugLog((DEB_WARN, "SSL2EncryptMessage: Unnecessary Move, performance hog\n"));
        /* if caller wasn't being smart, then we must copy memory here */
        MoveMemory((PUCHAR)Clean.pvBuffer+pContext->pCheck->cbCheckSum,
		           (PUCHAR)pAppInput->pvBuffer,
		           pAppInput->cbData);
    }

	/* finalize and store the hash */
    pContext->pCheck->Finalize( pSum, (PUCHAR)Clean.pvBuffer);


    /* Perform encryption to fill buffer */
    pctRet = pContext->pSystem->Encrypt( pContext->pWriteState,
		                                 &Clean,
		                                 &Encrypted);
    if(PCT_ERR_OK != pctRet)
    {
        SP_RETURN(pctRet);
    }

	cPad2 = Encrypted.cbData - Clean.cbData;
	if(cPad2 & !cPadding)
    {
		/* Our first padding calc was wrong, so we must move all data */
        DebugLog((DEB_WARN, "SSL2EncryptMessage-pad1: Unnecessary Move, performance hog\n"));

        MoveMemory(&((PUCHAR)pCommOutput->pvBuffer)[3],
                   Encrypted.pvBuffer,
                   Encrypted.cbData);
	}
    else  if(!cPad2 & cPadding)
    {
        DebugLog((DEB_WARN, "SSL2EncryptMessage-pad2: Unnecessary Move, performance hog\n"));

        MoveMemory(&((PUCHAR)pCommOutput->pvBuffer)[2],
                   Encrypted.pvBuffer,
                   Encrypted.cbData);
	}

	cPadding = cPad2;
    /* set sizes */
    if(cPadding) {
        if(Encrypted.cbData > 0x3fff)
        {
            SP_RETURN(SP_LOG_RESULT(PCT_INT_DATA_OVERFLOW));
        }

        ((PUCHAR)pCommOutput->pvBuffer)[0]= (UCHAR)(0x80 || (0x3f & (Encrypted.cbData>>8)));
        ((PUCHAR)pCommOutput->pvBuffer)[1]= (UCHAR)(0xff & Encrypted.cbData);
        ((PUCHAR)pCommOutput->pvBuffer)[2]= (UCHAR)cPadding;

    }
    else
    {
        if(Encrypted.cbData > 0x7fff)
        {
            SP_RETURN(SP_LOG_RESULT(PCT_INT_DATA_OVERFLOW));
        }
        ((PUCHAR)pCommOutput->pvBuffer)[0]= (UCHAR)(0x7f & (Encrypted.cbData>>8)) | 0x80;
        ((PUCHAR)pCommOutput->pvBuffer)[1]= (UCHAR)(0xff & Encrypted.cbData);
    }

    pCommOutput->cbData = Encrypted.cbData + (cPadding?3:2);

#if DBG
    for(di=0;di<SSL2_MASTER_KEY_SIZE;di++)
	sprintf(KeyDispBuf+(di*2), "%2.2x",
		((BYTE *)Encrypted.pvBuffer+Encrypted.cbData-pContext->pCheck->cbCheckSum)[di]);
    DebugLog((DEB_TRACE, "  MAC\t%s\n", KeyDispBuf));
#endif

    pContext->WriteCounter ++ ;

    SP_RETURN( PCT_ERR_OK );

}

SP_STATUS WINAPI
Ssl2DecryptMessage(
    PSPContext         pContext,
    PSPBuffer          pMessage,
    PSPBuffer          pAppOutput)
{

    SP_STATUS              pctRet;
    DWORD                   cPadding;
    DWORD                   dwLength;
    PUCHAR                  pbMAC;
    DWORD                   cbData;

    SPBuffer               Encrypted;
    SPBuffer               Clean;

    PCheckSumBuffer         pSum;
    HashBuf                 SumBuf;
    DWORD                   ReverseSequence;
    DWORD                   cbActualData;

    UCHAR                   Digest[16];
#if DBG
    DWORD           di;
    CHAR            KeyDispBuf[SSL2_MASTER_KEY_SIZE*2+1];
#endif

    SP_BEGIN("Ssl2DecryptMessage");

    /* First determine the length of data, the length of padding,
     * and the location of data, and the location of MAC */
    cbActualData = pMessage->cbData;
    pMessage->cbData = 2; /* minimum amount of data we need */


    if(pMessage->cbData > cbActualData)
    {
        SP_RETURN(PCT_INT_INCOMPLETE_MSG);
    }

    if(  ((PUCHAR)pMessage->pvBuffer)[0]&0x80 )
    {
        /* 2 byte header */
        cPadding = 0;
        dwLength = ((((PUCHAR)pMessage->pvBuffer)[0]&0x7f)<< 8) |
                   ((PUCHAR)pMessage->pvBuffer)[1];

        Clean.pvBuffer = Encrypted.pvBuffer = ((PUCHAR)pMessage->pvBuffer) + 2;
	    Clean.cbBuffer = pMessage->cbBuffer - 2;
    }
    else
    {
        pMessage->cbData++;
        if(pMessage->cbData > cbActualData)
        {
            SP_RETURN(PCT_INT_INCOMPLETE_MSG);
        }

        /* 3 byte header */
        cPadding = ((PUCHAR)pMessage->pvBuffer)[2];
        dwLength = ((((PUCHAR)pMessage->pvBuffer)[0]&0x3f)<< 8) |
                    ((PUCHAR)pMessage->pvBuffer)[1];

        Clean.pvBuffer = Encrypted.pvBuffer = ((PUCHAR)pMessage->pvBuffer) + 3;
        Clean.cbBuffer = pMessage->cbBuffer - 3;
    }
    /* Now we know how mutch data we will eat, so set cbData on the Input to be that size */
    pMessage->cbData += dwLength;
    /* do we have enough bytes for the reported data */
    if(pMessage->cbData > cbActualData)
    {
        SP_RETURN(PCT_INT_INCOMPLETE_MSG);
    }

    /* do we have engough data for our checksum */
    if(dwLength < pContext->pCheck->cbCheckSum)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_MSG_ALTERED));
    }

    Encrypted.cbData = dwLength;    /* encrypted data size */
    pbMAC = (PUCHAR)Clean.pvBuffer; /* location of MAC */

    /* check to see if we have a block size violation */
    if(Encrypted.cbData % pContext->pSystem->BlockSize)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_MSG_ALTERED));
    }

    Encrypted.cbBuffer = Encrypted.cbData;

    /* Decrypt */
    pctRet = pContext->pSystem->Decrypt( pContext->pReadState,
			    &Encrypted,
			    &Clean);

    if(PCT_ERR_OK != pctRet)
    {
        SP_RETURN(pctRet);
    }


    (PUCHAR)Clean.pvBuffer += pContext->pCheck->cbCheckSum; /* move pointer to start of data */
    Clean.cbData -=  pContext->pCheck->cbCheckSum;
    cbData = Clean.cbData - cPadding;
    /* Validate MAC: */


    CloneHashBuf(SumBuf, pContext->ReadMACState, pContext->pCheck);

    pSum = (PCheckSumBuffer)SumBuf;

    pContext->pCheck->Sum( pSum, Clean.cbData, Clean.pvBuffer);

    ReverseSequence = htonl( pContext->ReadCounter );

    DebugLog((DEB_TRACE, "Unsealing message %x\n", pContext->ReadCounter));

    pContext->pCheck->Sum( pSum, sizeof(DWORD), (PUCHAR) &ReverseSequence );
    pContext->pCheck->Finalize( pSum, Digest );

    pContext->ReadCounter++;

#if DBG

    for(di=0;di<SSL2_MASTER_KEY_SIZE;di++)
	sprintf(KeyDispBuf+(di*2), "%2.2x",
		pbMAC[di]);
    DebugLog((DEB_TRACE, "  Incoming MAC\t%s\n", KeyDispBuf));

    for(di=0;di<SSL2_MASTER_KEY_SIZE;di++)
	sprintf(KeyDispBuf+(di*2), "%2.2x",
		Digest[di]);
    DebugLog((DEB_TRACE, "  Computed MAC\t%s\n", KeyDispBuf));

#endif

    if (memcmp( Digest, pbMAC, pContext->pCheck->cbCheckSum ) )
    {
       SP_RETURN(SP_LOG_RESULT(PCT_INT_MSG_ALTERED));
    }

	if(pAppOutput->pvBuffer != Clean.pvBuffer)
    {
        DebugLog((DEB_WARN, "SSL2DecryptMessage: Unnecessary Move, performance hog\n"));
		MoveMemory(pAppOutput->pvBuffer,
                   Clean.pvBuffer,
                   cbData);
	}
    pAppOutput->cbData = cbData;

    SP_RETURN( PCT_ERR_OK );
}

//+---------------------------------------------------------------------------
//
//  Function:   Ssl2MakeSessionKeys
//
//  Synopsis:   Initializes the session keys (mac, crypt) from the context.
//
//  Arguments:  [pContext] --
//              [Bits]     --
//
//  Algorithm:
//
//  History:    10-10-95   RichardW   Created
//
//  Notes:
//
//----------------------------------------------------------------------------
SP_STATUS
Ssl2MakeSessionKeys(
    PSPContext         pContext)
{

    PCheckSumBuffer     CReadHash, CWriteHash;
    HashBuf             CReadHB, CWriteHB;
    UCHAR               pWriteKey[SSL2_MASTER_KEY_SIZE], pReadKey[SSL2_MASTER_KEY_SIZE];

    UCHAR       Buffer[SSL2_MASTER_KEY_SIZE +
                       1 +
			           SSL2_MAX_CHALLENGE_LEN +
                       SSL2_MAX_CONNECTION_ID_LEN
                      ];

    DWORD       BufferLen;


    // Validate that the context can be used for
    // generating an SSL2 style keyset.

    if((pContext == NULL) ||
        (pContext->RipeZombie == NULL))
    {
        return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }

    if(pContext->RipeZombie->cbMasterKey != SSL2_MASTER_KEY_SIZE)
    {
        return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }

    InitHashBuf(CWriteHB, pContext);
    InitHashBuf(CReadHB, pContext);
    CWriteHash = (PCheckSumBuffer)CWriteHB;
    CReadHash = (PCheckSumBuffer)CReadHB;



    BufferLen = pContext->RipeZombie->cbMasterKey;

    CopyMemory(Buffer, pContext->RipeZombie->pMasterKey, BufferLen);

    Buffer[BufferLen++] = '0';

    CopyMemory(&Buffer[BufferLen],
               pContext->pChallenge,
               pContext->cbChallenge );

    BufferLen += pContext->cbChallenge ;


    CopyMemory( &Buffer[BufferLen],
		pContext->pConnectionID,
		pContext->cbConnectionID );

    BufferLen += pContext->cbConnectionID ;

    /* hash the buffer */
    pContext->pCheck->Sum( CReadHash, BufferLen, Buffer );
    Buffer[16] = '1';
    pContext->pCheck->Sum( CWriteHash, BufferLen, Buffer );

    if (pContext->Flags & CONTEXT_FLAG_CLIENT)
    {
	    pContext->pCheck->Finalize( CReadHash, pReadKey );
	    pContext->pCheck->Finalize( CWriteHash, pWriteKey );
    }
    else
    {
	    pContext->pCheck->Finalize( CWriteHash, pReadKey );
	    pContext->pCheck->Finalize( CReadHash, pWriteKey );
    }

    pContext->KeySize = 16;
    InitHashBuf(pContext->RdMACBuf, pContext);
    InitHashBuf(pContext->WrMACBuf, pContext);

    pContext->ReadMACState = (PCheckSumBuffer)pContext->RdMACBuf;
    pContext->WriteMACState = (PCheckSumBuffer)pContext->WrMACBuf;

    if (pContext->pSystem->Initialize(  pReadKey,
			    CONTEXT_KEY_SIZE,
			    &pContext->pReadState ) )
    {
        if (pContext->pSystem->Initialize(  pWriteKey,
				        CONTEXT_KEY_SIZE,
				        &pContext->pWriteState) )
        {
	            /* initialize the Hash SECRET */

            pContext->pCheck->Sum( pContext->ReadMACState, pContext->KeySize,
					               pReadKey);

            pContext->pCheck->Sum( pContext->WriteMACState, pContext->KeySize,
					               pWriteKey);

            return (PCT_ERR_OK);
        }

        pContext->pSystem->Discard( &pContext->pReadState );
    }

    return( PCT_INT_INTERNAL_ERROR );
}

