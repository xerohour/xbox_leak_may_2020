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

//UNDONE: these are copied from ssl2msg.h God knows why this file is not including any files.
// May be for PCTREF, SO for now, I'm copying these defenitions...
//These defines MAY not change as often as you think, these are from SSL3 SPEC...

#define CB_SSL3_RANDOM		 32
#define CB_SSL3_UNIXTIME	  4
#define CB_SSL3_PRE_MASTER_SECRET  48
#define CB_SSL3_MASTER_KEY_BLOCK    80



#define SP_CONTEXT_MAGIC   *(DWORD *)"!Tcp"

#define CONTEXT_KEY_SIZE    16


typedef struct _SPContext
{
    DWORD               Magic;          /* tags structure */

    DWORD               State;          /* the current state the connection is in */

    DWORD               Flags;

    /* data for the context that can be used
     * to start a new session */
    SessCacheItem *     RipeZombie;   /* cacheable context that is being used  */


    /* credential used when creating context */
    PSPCredential      pCred;
    PctPrivateKey       *pPrivateKey;

    /* virtual function tables to various crypto functions */

    const CryptoSystem      * pSystem;
    const CheckSumFunction  * pCheck;
    const KeyExchangeSystem * pKeyExch;

    /* functions pointing to the various handlers for this protocol */
    SPDecryptMessageFn  Decrypt;
	SPEncryptMessageFn  Encrypt;
    SPProtocolHandlerFn ProtocolHandler;
    SPDecryptHandlerFn  DecryptHandler;
    SPInitiateHelloFn   InitiateHello;

    /* session crypto state */

    // encryption key size.
    DWORD               KeySize;

    // Encryption States
    PStateBuffer        pReadState;     /* keying struct for readkey */
    PStateBuffer        pWriteState;    /* keying struct for writekey */

    // Mac secret values
    UCHAR               ReadMACKey[SP_MAX_MAC_KEY];
    UCHAR               WriteMACKey[SP_MAX_MAC_KEY];

    // MAC states
    HashBuf             InHSHBuf;

    HashBuf             RdMACBuf;
    HashBuf             WrMACBuf;

    PCheckSumBuffer     InitMACState;

    PCheckSumBuffer     ReadMACState;
    PCheckSumBuffer     WriteMACState;


    // Packet Sequence counters.

    DWORD               ReadCounter;
    DWORD               WriteCounter;


    DWORD               cbConnectionID;
    UCHAR               pConnectionID[SP_MAX_CONNECTION_ID];

    DWORD               cbChallenge;
    UCHAR               pChallenge[SP_MAX_CHALLENGE];

    DWORD               cbKeyArgs;
    UCHAR               pKeyArgs[SP_MAX_KEY_ARGS];


    // Save copy of client hello to hash for verification.
    DWORD               cbClHello;      /* length of client hello message */
    PUCHAR              pClHello;       /* points to client hello message */

    CipherSpec	    PendingCiphSpec;
    HashSpec	    PendingHashSpec;
    ExchSpec	    PendingExchSpec;

    // PCT1 specific items.

    HashBuf             pVerifyPrelude; /* running hash to calc VP */

    // SSL3 specific items.

    BOOL				fExchKey; // Did we sent a Exchnage key message
    BOOL				fCertReq; //Did we request a certificate ?? this is for ssl3 Serverside..
    BOOL                fSGC;     // Does the server support Server Gated Crypto
    HashBuf             MD5Handshake;
    HashBuf             SHAHandshake;

    HashBuf             RdMACBuf1;		//SSL3 needs 1 more pre hashed buffers
    PCheckSumBuffer     ReadMACState1;	//SSL3 needs 1 more Pre hashed states.
    HashBuf             WrMACBuf1;		
    PCheckSumBuffer     WriteMACState1;

    PUCHAR              pbIssuerList;
    DWORD               cbIssuerList;

    short				wS3CipherSuiteClient;
    short				wS3CipherSuiteServer;
    short				wS3pendingCipherSuite;
    UCHAR               Ssl3MasterKeyBlock[CB_SSL3_MASTER_KEY_BLOCK];

    UCHAR 				rgbS3CRandom[CB_SSL3_RANDOM];
    UCHAR				rgbS3SRandom[CB_SSL3_RANDOM];

} SPContext, * PSPContext;




/* Flags */
#define CONTEXT_FLAG_CLIENT                 0x00000001
#define CONTEXT_FLAG_CLIAUTH                0x00000100
#define CONTEXT_FLAG_EXT_ERR                0x00000200  /* Generate error message on error */
#define CONTEXT_FLAG_NO_INCOMPLETE_CRED_MSG 0x00000400  /* don't generate an INCOMPLETE CREDS message */
#define CONTEXT_FLAG_CONNECTION_MODE        0x00001000  /* as opposed to stream mode */
#define CONTEXT_FLAG_NOCACHE                0x00001000  /* do not look things up in the cache */



#ifdef DBG
PSTR DbgGetNameOfCrypto(DWORD x);
#endif

void InitHashBuf(HashBuf Buf,
                 PSPContext pContext
                 );

PSTR
CopyString(
           PSTR        pszString);



PSPContext SPContextCreate(PUCHAR pszTarget);

BOOL SPContextDelete(PSPContext pContext);

SP_STATUS
SPContextSetCredentials(
    PSPContext pContext,
    PSPCredential  pCred);


SP_STATUS
ContextInitCiphers(SPContext *pContext);

SP_STATUS
SPContextDoMapping(
    PSPContext pContext);

SP_STATUS
SPContextGetIssuers(
    PSPContext pContext,
    PBYTE pbIssuers,
    DWORD *pcbIssuers);

BOOL FGetServerIssuer(
    PBYTE pbIssuer,	
    DWORD *pdwIssuer);
