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


/* TBD: add redo capability? */

#include <spbase.h>
#include <pct1msg.h>
#include <pct1prot.h>
#include <md5.h>
#include <sha.h>
#include <ssl2msg.h>
#include <ssl3msg.h>
#include <ssl2prot.h>


DWORD g_ProtEnabled = SP_PROT_ALL;


const UNICipherMap UniAvailableCiphers[] = {
    // Pct ciphers
    {
        UNI_CK_PCT,
            SP_PROT_PCT1,
            SP_HASH_UNKNOWN,
            SP_CIPHER_UNKNOWN,
            SP_EXCH_UNKNOWN
    },

    {
        SSL_MKFAST(PCT_SSL_CERT_TYPE, MSBOF(SP_CERT_X509), LSBOF(SP_CERT_X509)),
            SP_PROT_PCT1,
            SP_HASH_UNKNOWN,
            SP_CIPHER_UNKNOWN,
            SP_EXCH_UNKNOWN
    },

    {
        SSL_MKFAST(PCT_SSL_HASH_TYPE, MSBOF(SP_HASH_MD5), LSBOF(SP_HASH_MD5)),
            SP_PROT_PCT1,
            SP_HASH_MD5,
            SP_CIPHER_UNKNOWN,
            SP_EXCH_UNKNOWN
    },
    {
        SSL_MKFAST(PCT_SSL_HASH_TYPE, MSBOF(SP_HASH_SHA), LSBOF(SP_HASH_SHA)),
            SP_PROT_PCT1,
            SP_HASH_SHA,
            SP_CIPHER_UNKNOWN,
            SP_EXCH_UNKNOWN
    },

    {
        SSL_MKFAST(PCT_SSL_EXCH_TYPE, MSBOF(SP_EXCH_RSA_PKCS1),  LSBOF(SP_EXCH_RSA_PKCS1)),
            SP_PROT_PCT1,
            SP_HASH_UNKNOWN,
            SP_CIPHER_UNKNOWN,
            SP_EXCH_RSA_PKCS1
    },

    // Domestic Ciphers
    // PCT1
    {
        SSL_MKFAST(PCT_SSL_CIPHER_TYPE_1ST_HALF, MSBOF(SP_CIPHER_RC4>>16), LSBOF(SP_CIPHER_RC4>>16)),
            SP_PROT_PCT1,
            SP_HASH_UNKNOWN,
            SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128,
            SP_EXCH_UNKNOWN
    },

    {
        SSL_MKFAST(PCT_SSL_CIPHER_TYPE_2ND_HALF, MSBOF(SP_ENC_BITS_128), LSBOF(SP_MAC_BITS_128)),
            SP_PROT_PCT1,
            SP_HASH_UNKNOWN,
            SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128,
            SP_EXCH_UNKNOWN
    },

    // Ssl3 ciphers (Domestic)
    {
        SSL3_RSA_WITH_RC4_128_MD5,
            SP_PROT_SSL3,
            SP_HASH_MD5 ,
            SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
    },
    {
        SSL3_RSA_WITH_RC4_128_SHA,
            SP_PROT_SSL3,
            SP_HASH_SHA ,
            SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
    },

    // Ssl2 ciphers (Domestic)
    {
        SSL_CK_RC4_128_WITH_MD5,
            SP_PROT_SSL2 ,
            SP_HASH_MD5 ,
            SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
    },

    // FInance Ciphers
    // PCT1
    {
        SSL_MKFAST(PCT_SSL_CIPHER_TYPE_1ST_HALF, MSBOF(SP_CIPHER_RC4>>16), LSBOF(SP_CIPHER_RC4>>16)),
            SP_PROT_PCT1,
            SP_HASH_UNKNOWN,
            SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128,
            SP_EXCH_UNKNOWN
    },

    {
        SSL_MKFAST(PCT_SSL_CIPHER_TYPE_2ND_HALF, MSBOF(SP_ENC_BITS_64), LSBOF(SP_MAC_BITS_128)),
            SP_PROT_PCT1,
            SP_HASH_UNKNOWN,
            SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128,
            SP_EXCH_UNKNOWN
    },
    // SSL3
    {
        SSL_RSA_FINANCE64_WITH_RC4_64_MD5,
            SP_PROT_SSL3,
            SP_HASH_MD5 ,
            SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
    },
    {
        SSL_RSA_FINANCE64_WITH_RC4_64_SHA,
            SP_PROT_SSL3,
            SP_HASH_SHA,
            SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
    },

    // SSL2
    {
        SSL_CK_RC4_128_FINANCE64_WITH_MD5,
            SP_PROT_SSL2 | SP_PROT_SSL3,
            SP_HASH_MD5 ,
            SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
    },

    // Export Ciphers
    // PCT1
    {
        SSL_MKFAST(PCT_SSL_CIPHER_TYPE_1ST_HALF, MSBOF(SP_CIPHER_RC4>>16), LSBOF(SP_CIPHER_RC4>>16)),
            SP_PROT_PCT1,
            SP_HASH_UNKNOWN,
            SP_CIPHER_RC4 | SP_ENC_BITS_40 | SP_MAC_BITS_128,
            SP_EXCH_UNKNOWN
    },

    {
        SSL_MKFAST(PCT_SSL_CIPHER_TYPE_2ND_HALF, MSBOF(SP_ENC_BITS_40), LSBOF(SP_MAC_BITS_128)),
            SP_PROT_PCT1,
            SP_HASH_UNKNOWN,
            SP_CIPHER_RC4 | SP_ENC_BITS_40 | SP_MAC_BITS_128,
            SP_EXCH_UNKNOWN
    },

    //SSL3

    {
        SSL3_RSA_EXPORT_WITH_RC4_40_MD5,
            SP_PROT_SSL3,
            SP_HASH_MD5 ,
            SP_CIPHER_RC4 | SP_ENC_BITS_40 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
    },


    // SSL2

    {
        SSL_CK_RC4_128_EXPORT40_WITH_MD5,
            SP_PROT_SSL2 ,
            SP_HASH_MD5 ,
            SP_CIPHER_RC4 | SP_ENC_BITS_40 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
#if 0
    // SHolden - I was informed that these zero privacy ciphers should be
    //           disabled for both export and domestic.
    },

    // Zero Privacy ciphers
    // SSL3
    {
        SSL3_RSA_WITH_NULL_MD5,
            SP_PROT_SSL3,
            SP_HASH_MD5 ,
            SP_CIPHER_NONE | SP_ENC_BITS_128 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
    },
    {
        SSL3_RSA_WITH_NULL_SHA,
            SP_PROT_SSL3,
            SP_HASH_SHA ,
            SP_CIPHER_NONE | SP_ENC_BITS_128 | SP_MAC_BITS_128,
            SP_EXCH_RSA_PKCS1
#endif // 0
    }
};

SP_STATUS WINAPI
GenerateSsl2StyleHello(
    PSPContext              pContext,
    PSPBuffer               pOutput,
    WORD                    fProtocol);


SP_STATUS WINAPI
GenerateHello(
    PSPContext              pContext,
    PSPBuffer               pOutput,
    BOOL                    fCache)
{


    /* Check to see if we can find this target in the cache */


    DWORD           fProt;

    if (!pOutput)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }



    if(fCache)
    {
        // Look this id up in the cache
        SPCacheRetrieveByName(g_ProtEnabled, pContext->RipeZombie->szCacheID, &pContext->RipeZombie);
    }

    // We should have a zombie identity here
    if(pContext->RipeZombie == NULL)
    {
        return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }

    fProt = pContext->RipeZombie->fProtocol & g_ProtEnabled;

    if(SP_PROT_UNI_CLIENT & fProt)
    {
        pContext->State             = UNI_STATE_CLIENT_HELLO;
        pContext->ProtocolHandler   = ClientProtocolHandler;

        return GenerateUniHello(pContext, pOutput, g_ProtEnabled);
    }
#ifdef SCHANNEL_PCT    
    else
    if(SP_PROT_PCT1_CLIENT & fProt)
    {
        pContext->State             = PCT1_STATE_CLIENT_HELLO;
        pContext->ProtocolHandler   = Pct1ClientProtocolHandler;

        return GeneratePct1StyleHello(pContext, pOutput);
    }
#endif

    else
    if(SP_PROT_SSL3_CLIENT & fProt)
    {
        pContext->State             = SSL3_STATE_CLIENT_HELLO;
        pContext->ProtocolHandler   = Ssl3ClientProtocolHandler;

        return GenerateSsl3ClientHello(pContext,  pOutput);
    }

    else
    if(SP_PROT_SSL2_CLIENT & fProt)
    {
        pContext->State             = SSL2_STATE_CLIENT_HELLO;
        pContext->ProtocolHandler   = Ssl2ClientProtocolHandler;

        return GenerateUniHello(pContext, pOutput, SP_PROT_SSL2_CLIENT);
    }
    else
    {
        return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
    }
}


SP_STATUS WINAPI
GenerateUniHelloMessage(
    PSPContext              pContext,
    Ssl2_Client_Hello *     pHelloMessage,
    DWORD                   fProtocol
    )
{
    DWORD                i;
    SP_STATUS pctRet = PCT_INT_INTERNAL_ERROR;
    UCHAR bOffset = 2;
    DWORD                fClientProt = fProtocol & SP_PROT_CLIENTS;

    SP_BEGIN("GenerateUniHello");


    if (!pHelloMessage)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }


    pContext->Flags |= CONTEXT_FLAG_CLIENT;


    pHelloMessage->cCipherSpecs = 0;
    // Generate a cipher list
    //
    for(i=0; i < sizeof(UniAvailableCiphers)/sizeof(UNICipherMap); i++)
    {
        DWORD  fAllowedProt = UniAvailableCiphers[i].fProt & fClientProt;
        if(0 == fAllowedProt)
        {
            continue;
        }
        if((UniAvailableCiphers[i].Hash != SP_HASH_UNKNOWN) &&
           (NULL == HashFromSpec(UniAvailableCiphers[i].Hash, fAllowedProt)))
        {
            continue;
        }

        if((UniAvailableCiphers[i].Cipher != SP_CIPHER_UNKNOWN) &&
           (NULL == CipherFromSpec(UniAvailableCiphers[i].Cipher,
                        fAllowedProt | (pContext->RipeZombie->fSGC ? SP_PROT_SGC : 0))))
        {
            continue;
        }

        if((UniAvailableCiphers[i].KeyExch != SP_EXCH_UNKNOWN) &&
           (NULL == KeyExchangeFromSpec(UniAvailableCiphers[i].KeyExch, fAllowedProt)))
        {
            continue;
        }
        // this cipher is good to request
        pHelloMessage->CipherSpecs[pHelloMessage->cCipherSpecs++] = UniAvailableCiphers[i].CipherKind;

    }


    // We're minimally version 2
    pHelloMessage->dwVer = SSL2_CLIENT_VERSION;



    if(fClientProt & SP_PROT_SSL3_CLIENT)
    {
        pHelloMessage->dwVer = SSL3_CLIENT_VERSION;
    }

   /* Build the hello message. */
    pHelloMessage->cbSessionID = 0;


    if (pContext->RipeZombie && pContext->RipeZombie->cbSessionID)
    {
        CopyMemory(pHelloMessage->SessionID,
                   pContext->RipeZombie->SessionID,
                   pContext->RipeZombie->cbSessionID);

        pHelloMessage->cbSessionID =  pContext->RipeZombie->cbSessionID;
    }

    CopyMemory(  pHelloMessage->Challenge,
                pContext->pChallenge,
                pContext->cbChallenge);
    pHelloMessage->cbChallenge = pContext->cbChallenge;



    SP_RETURN(PCT_ERR_OK);

}


SP_STATUS WINAPI
GenerateUniHello(
    PSPContext             pContext,
    PSPBuffer               pOutput,
    DWORD                   fProtocol
    )

{
    SP_STATUS pctRet;
    Ssl2_Client_Hello    HelloMessage;

    SP_BEGIN("GenerateUniHello");

    GenerateRandomBits( pContext->pChallenge, SSL2_CHALLENGE_SIZE );
    pContext->cbChallenge = SSL2_CHALLENGE_SIZE;

    pctRet = GenerateUniHelloMessage(pContext, &HelloMessage, fProtocol);

    pContext->ReadCounter = 0;

    if(PCT_ERR_OK != pctRet)
    {
        SP_RETURN(pctRet);
    }
    if(PCT_ERR_OK != (pctRet = Ssl2PackClientHello(&HelloMessage,  pOutput)))
    {
        SP_RETURN(pctRet);
    }

    // In case this is a PCT session, save the hello
    // so we can hash it later.

    pContext->pClHello = SPExternalAlloc(pOutput->cbData);
    if(pContext->pClHello == NULL)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));
    }
    CopyMemory(pContext->pClHello, pOutput->pvBuffer, pOutput->cbData);
    pContext->cbClHello = pOutput->cbData;



    // In case of SSL3, do the handshare hash.
	UpdateHandshakeHash(pContext,
                        (PUCHAR)pOutput->pvBuffer + 2,
                        pOutput->cbData - 2,
                        TRUE);
    /* We set this here to tell the protocol engine that we just send a client
     * hello, and we're expecing a pct server hello */

    pContext->WriteCounter = 1;
    pContext->ReadCounter = 0;

    SP_RETURN(PCT_ERR_OK);
}

SP_STATUS WINAPI
ClientProtocolHandler(
    PSPContext pContext,
    PSPBuffer pCommInput,
    PSPBuffer pCommOutput,
    PSPBuffer pAppOutput)
{
    SP_STATUS pctRet = 0;
    PUCHAR pb;
    DWORD dwVersion;

    /* PCTv1.0 Server Hello starts with
     * RECORD_LENGTH_MSB  (ignore)
     * RECORD_LENGTH_LSB  (ignore)
     * PCT1_SERVER_HELLO  (must be equal)
     * SH_PAD
     * PCT1_CLIENT_VERSION_MSB (must be pct1)
     * PCT1_CLIENT_VERSION_LSB (must be pct1)
     *
     * ... PCT hello ...
     */


    /* SSLv2 Hello starts with
     * RECORD_LENGTH_MSB  (ignore)
     * RECORD_LENGTH_LSB  (ignore)
     * SSL2_SERVER_HELLO  (must be equal)
     * SESSION_ID_HIT
     * CERTIFICATE_TYPE
     * SSL2_CLIENT_VERSION_MSB (Must be ssl2)
     * SSL2_CLIENT_VERSION_LSB (Must be ssl2)
     *
     * ... SSLv2 Hello ...
     */


    /* SSLv3 Type 3 Server Hello starts with
     * 0x15 Hex (HANDSHAKE MESSAGE)
     * VERSION MSB
     * VERSION LSB
     * RECORD_LENGTH_MSB  (ignore)
     * RECORD_LENGTH_LSB  (ignore)
     * HS TYPE (SERVER_HELLO)
     * 3 bytes HS record length
     * HS Version
     * HS Version
     */

    // We need at least 5 bytes to determine what we have.
    if (pCommInput->cbData < 7)
    {
        return(PCT_INT_INCOMPLETE_MSG);
    }

    pb = pCommInput->pvBuffer;
    // If the first byte is 0x15, then check if we have a
    // SSLv3 Type3 client hello

    if(pb[0] == SSL3_CT_HANDSHAKE)
    {
        if(!(SP_PROT_SSL3_CLIENT & g_ProtEnabled))
        {
            return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
        }

        // SSLv3 message
        // This appears to be a ssl3 server hello
        pContext->ProtocolHandler = Ssl3ClientProtocolHandler;
        pContext->DecryptHandler = Ssl3DecryptHandler;
        return(Ssl3ClientProtocolHandler(pContext, pCommInput, pCommOutput, pAppOutput));
   }

    if(pb[2] == SSL2_MT_SERVER_HELLO)
    {
        dwVersion = COMBINEBYTES(pb[5], pb[6]);
        if(dwVersion ==SSL2_CLIENT_VERSION)
        {
            if(!(SP_PROT_SSL2_CLIENT & g_ProtEnabled))
            {
                return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
            }
            // This appears to be a ssl2 server hello
            pContext->ProtocolHandler = Ssl2ClientProtocolHandler;
            pContext->DecryptHandler = Ssl2DecryptHandler;
            return(Ssl2ClientProtocolHandler(pContext, pCommInput, pCommOutput, pAppOutput));
        }
    }
#ifdef SCHANNEL_PCT    
    if(pb[2] == PCT1_MSG_SERVER_HELLO)
    {
        DWORD i;
        dwVersion = COMBINEBYTES(pb[4], pb[5]);
        if(dwVersion ==PCT_VERSION_1)
        {
            // Convert challenge from 16 byte to 32 byte
            if(!(SP_PROT_PCT1_CLIENT & g_ProtEnabled))
            {
                return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
            }

            for(i=0; i < pContext->cbChallenge; i++)
            {
                pContext->pChallenge[i + pContext->cbChallenge] = ~pContext->pChallenge[i];
            }
            pContext->cbChallenge = 2*pContext->cbChallenge;
            // This appears to be a ssl2 server hello
            pContext->ProtocolHandler = Pct1ClientProtocolHandler;
            pContext->DecryptHandler = Pct1DecryptHandler;
            return(Pct1ClientProtocolHandler(pContext, pCommInput, pCommOutput, pAppOutput));
        }
    }
#endif    

    return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
}
