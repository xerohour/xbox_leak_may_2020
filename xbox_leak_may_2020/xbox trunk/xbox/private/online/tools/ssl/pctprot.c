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
#include <ssl2msg.h>
#include <ssl2prot.h>


CipherSpec PctCipherRank[] = {
#ifndef SCHANNEL_EXPORT
    PCT_CIPHER_RC4 | PCT_ENC_BITS_128 | PCT_MAC_BITS_128,
#endif 
    PCT_CIPHER_RC4 | PCT_ENC_BITS_40  | PCT_MAC_BITS_128
};

DWORD CipherRanks = sizeof(PctCipherRank)/sizeof(CipherSpec);

/* available hashes, in order of preference */
HashSpec PctHashRank[] = {
    PCT_HASH_MD5
};
DWORD HashRanks = sizeof(PctHashRank)/sizeof(HashSpec);

CertSpec PctCertRank[] = {
    PCT_CERT_X509
};
DWORD CertRanks = sizeof(PctCertRank)/sizeof(CertSpec);


SigSpec PctSigRank[] = {
    PCT_SIG_RSA_MD5,
};
DWORD SigRanks = sizeof(PctSigRank)/sizeof(SigSpec);


ExchSpec PctExchRank[] = {
   PCT_EXCH_RSA_PKCS1
};

DWORD ExchRanks = sizeof(PctExchRank)/sizeof(ExchSpec);



/*****************************************************************************
* 
* Main Protocol Handler
*   This procedure handles the state transitions of the protocol.
*
*
* Inputs -
*         PPctContext pContext   - Current Pct Context
*         PSPBuffer  pCommInput - Input Record from Communication Channel
*         PSPBuffer  pAppInput  - Input from the Application 
*
*
* Outputs -
*         pCommOutput - Pending Output to be sent over the communications channel.
*         pAppOutput  - Data for the app, unpacked. 
*
* Returns - status code
*
* NOTE:  The data streams to and from the protocol handler should work like 
*        this.
*                           _________________
*  Communications -------> | Protocol Handler| ----> Cleartext data to
*  Channel        <------- |_________________| <---- and from the Application
*
*  The surrounding environment should always transmit data in the CommOutput
*  if the count of bytes is greater than zero.  
*  CommInput should contain an input buffer with a count of bytes.  If the
*  data is successfully packaged, then the count of bytes in CommInput will
*  be set to zero.  If the data was not packaged, either due to protocol 
*  messages taking precedence, or due to an error, then the count of bytes
*  will not be changed.  The buffer should then be re-presented to the 
*  handler during the next pass.
*
*****************************************************************************/
SP_STATUS WINAPI PctProtocolHandler(PPctContext pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pCommOutput,
                              PSPBuffer  pAppOutput)
{
    SP_STATUS      pctRet= 0;
    DWORD           dwStateTransition;
	BOOL           fRaw = TRUE;
    SPBuffer       MsgInput;

    if(pCommOutput) pCommOutput->cbData = 0;
    if(pAppOutput) pAppOutput->cbData = 0;
    /* Protocol handling steps should be listed in most common
     * to least common in order to improve performance 
     */
    MsgInput.pvBuffer = pCommInput->pvBuffer;
    MsgInput.cbBuffer = pCommInput->cbBuffer;
    MsgInput.cbData = pCommInput->cbData;

	/* in the following states, we should decrypt the message */
	switch(pContext->State) {  
        /* pct has no encrypted protocol messages */
    }
    /* first deal with incoming messages over the comm port, these
     * will protocol related, and take precedence over App messages
     */


    /* We are not connected, so we're doing
     * protocol negotiation of some sort.  All protocol
     * negotiation messages are sent in the clear */
    /* There are no branches in the connecting protocol
     * state transition diagram, besides connection and error,
     * which means that a simple case statement will do */

    /* Do we have enough data to determine what kind of message we have */
    if(MsgInput.cbData < 3) {
        pctRet = Pct1GenerateError(pContext, 
                                  pCommOutput, 
                                  PCT_ERR_ILLEGAL_MESSAGE, 
                                  NULL);

    }

    if((PCT_ERR_OK == pctRet) && (MsgInput.cbData > 3)) {        

	    /* build a single dword out of the last state and the incoming message
	     * to do a case off of */
        if(fRaw) {
            dwStateTransition = (((PUCHAR)MsgInput.pvBuffer)[2]<<16) |
                                  (pContext->State & 0xffff);
        } else {
            dwStateTransition = (((PUCHAR)MsgInput.pvBuffer)[0]<<16) |
                                  (pContext->State & 0xffff);
        }
        switch(dwStateTransition)
        {

#ifndef PCT_CLIENT_ONLY
            /* Server receives client hello */
            case (PCT_MSG_CLIENT_HELLO << 16) | SP_STATE_NONE:
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
                    if(PCT_ERR_OK == (pctRet = Pct1UnpackClientHello(
                                                &MsgInput,
                                                &pPct1Hello))) {

                        pContext->Protocol = CACHE_PROTOCOL_PCT1;
                        for(i=0;i<(int)pPct1Hello->cbSessionID;i++) 
                            fRealSessId |= pPct1Hello->SessionID[i];


                        if ((fRealSessId) &&
                            (PctFindSessIdInCache(CACHE_PROTOCOL_PCT1, 
                                                  pPct1Hello->SessionID, 
                                                  pPct1Hello->cbSessionID, 
                                                  &CachedSession))) {
                            pctRet = Pct1SrvRestart(pContext, 
                                                   pPct1Hello, 
                                                   pCommOutput, 
                                                   &CachedSession);
                            if(PCT_ERR_OK == pctRet) {
                                pContext->State = SP_STATE_CONNECTED;
								pContext->Encrypt = Pct1EncryptMessage;
								pContext->Decrypt = Pct1DecryptMessage;
                            } 
                       	}  else {

                            pctRet = Pct1SrvHandleClientHello(pContext,
                                                     &MsgInput,
                                                     pPct1Hello,
                                                     pCommOutput);
                            if(PCT_ERR_OK == pctRet) {
                                pContext->State = PCT1_STATE_SERVER_HELLO;
                            } 

                        }
                        SPExternalFree(pPct1Hello);

                    } 
                    if(SP_FATAL(pctRet)) {
                        pContext->State = PCT1_STATE_ERROR;
                    } 
                    break;
                }
            case (PCT_MSG_CLIENT_MASTER_KEY << 16) | PCT1_STATE_SERVER_HELLO:
                pctRet = Pct1SrvHandleCMKey(pContext,
                                                 &MsgInput,
                                                 pCommOutput);
                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                } else {
                    if(PCT_ERR_OK == pctRet) {
                        pContext->State = SP_STATE_CONNECTED;
						pContext->Encrypt = Pct1EncryptMessage;
						pContext->Decrypt = Pct1DecryptMessage;
                    } 
                    /* We received a non-fatal error, so the state doesn't
                     * change, giving the app time to deal with this */
                }
                break;

#endif /* PCT_CLIENT_ONLY */
#ifndef PCT_SERVER_ONLY
            /* Client receives Server hello */
            case (PCT_MSG_SERVER_HELLO << 16) | PCT1_STATE_CLIENT_HELLO:
                {
                    PPct1_Server_Hello pHello;
                    /* Attempt to recognize and handle various versions
                     * of Server hello, start by trying to unpickle the
                     * oldest, and the next version, until
                     * one unpickles.  Then run the handle code.  We can also put
                     * unpickling and handling code in here for SSL messages */
                    if(PCT_ERR_OK == (pctRet = Pct1UnpackServerHello(
                                                &MsgInput,
                                                &pHello))) {
                        pContext->Protocol = CACHE_PROTOCOL_PCT1;
                        /* let's resurrect the zombie session */
                        if (pHello->RestartOk) {
                            pctRet = Pct1CliRestart(pContext, pHello, pCommOutput);
                            if(PCT_ERR_OK == pctRet) {
                                pContext->State = SP_STATE_CONNECTED;
								pContext->Encrypt = Pct1EncryptMessage;
								pContext->Decrypt = Pct1DecryptMessage;
                            } 
                        } else {   
                            pctRet = Pct1CliHandleServerHello(pContext,
                                                         &MsgInput,
                                                         pHello,
                                                         pCommOutput);
                            if(PCT_ERR_OK == pctRet) {
                                pContext->State = PCT1_STATE_CLIENT_MASTER_KEY;
                            } 

                        }
                        SPExternalFree(pHello);

                    }
                    if(SP_FATAL(pctRet)) {
                        pContext->State = PCT1_STATE_ERROR;
                    }
                  
                    break;
                }

            case (PCT_MSG_SERVER_VERIFY << 16) | PCT1_STATE_CLIENT_MASTER_KEY:
                pctRet = Pct1CliHandleServerVerify(pContext,
                                                 &MsgInput,
                                                 pCommOutput);
                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                } else {
                    if(PCT_ERR_OK == pctRet) {
                        pContext->State = SP_STATE_CONNECTED;
						pContext->Encrypt = Pct1EncryptMessage;
						pContext->Decrypt = Pct1DecryptMessage;
                    } 
                    /* We received a non-fatal error, so the state doesn't
                     * change, giving the app time to deal with this */
                }
                break;
#endif /* PCT_SERVER_ONLY */

            default:
                pContext->State = PCT1_STATE_ERROR;
                {
                    if(((PUCHAR)MsgInput.pvBuffer)[2] == PCT_MSG_ERROR) {
                        /* we received an error message, process it */
                        pctRet = Pct1HandleError(pContext,
                                                 &MsgInput,
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
    return pctRet;
}

SP_STATUS WINAPI PctDecryptHandler(PPctContext pContext,
                              PSPBuffer  pCommInput,
                              PSPBuffer  pAppOutput)
{
    SP_STATUS      pctRet= 0;
    if(pCommInput->cbData > 0) {        
        /* first, we'll handle incoming data packets */
        if((pContext->State & SP_STATE_CONNECTED) && (pContext->Decrypt)) {
            if(PCT_ERR_OK == 
               (pctRet = pContext->Decrypt(pContext, 
                                           pCommInput,   /* message */ 
                                           pAppOutput /* Unpacked Message */
                                ))) {  
                /* look for escapes */
                if((*(PUCHAR)pCommInput->pvBuffer) & 0x40 == 0xc0) {
                    switch(*(PUCHAR)pAppOutput->pvBuffer) {
                        case PCT_ET_REDO_CONN:
                            /* InitiateRedo */
                        case PCT_ET_OOB_DATA:
                            /* HandleOOB */
                        default:
                            /* Unknown escape, generate error */
                            pctRet = PCT_INT_ILLEGAL_MSG;
                            /* Disconnect */
                            break;
                    }

                }
            }

        } else {
			return PCT_INT_ILLEGAL_MSG;
		}
	}
}




SP_STATUS Pct1GenerateError(PPctContext pContext,
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


SP_STATUS Pct1HandleError(PPctContext pContext,
                          PSPBuffer  pCommInput,
                          PSPBuffer  pCommOutput)
{
    pCommOutput->cbData = 0;
    return(((PPCT1_ERROR)pCommInput->pvBuffer)->ErrorMsb << 8 )|  ((PPCT1_ERROR)pCommInput->pvBuffer)->ErrorLsb;
}

