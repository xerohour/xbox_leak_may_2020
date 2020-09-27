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
SP_STATUS WINAPI SslProtocolHandler(PPctContext pContext,
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
        case SSL2_STATE_SERVER_VERIFY:
        case SSL2_STATE_CLIENT_MASTER_KEY:
        case SSL2_STATE_CLIENT_FINISH:
        case SSL2_STATE_SERVER_RESTART:
        case SSL2_STATE_CLIENT_RESTART:
            pctRet = Ssl2DecryptMessage(pContext, pCommInput, &MsgInput);
            fRaw = FALSE;
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
#ifdef BUGBUG
    if(MsgInput.cbData < 3) {
        pctRet = Pct1GenerateError(pContext, 
                                  pCommOutput, 
                                  PCT_ERR_ILLEGAL_MESSAGE, 
                                  NULL);

    }
#endif
    if((PCT_ERR_OK == pctRet) && (MsgInput.cbData > (DWORD)(fRaw?3:1))) {        

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

#ifndef SCHANNEL_CLIENT_ONLY
            /* Server receives client hello */
            case (SSL2_MT_CLIENT_HELLO << 16) | SP_STATE_NONE:
                {
                    PSsl2_Client_Hello pSsl2Hello;
                    UCHAR fRealSessId = 0;
                    SessCacheItem   CachedSession;

                    /* Attempt to recognize and handle various versions
                     * of client hello, start by trying to unpickle the
                     * most recent version, and then next most recent, until
                     * one unpickles.  Then run the handle code.  We can also put
                     * unpickling and handling code in here for SSL messages */
                    if(PCT_ERR_OK == (pctRet = Ssl2UnpackClientHello(
                                                    &MsgInput,
                                                    &pSsl2Hello))) {
                        pContext->Protocol = CACHE_PROTOCOL_SSL2;
                        pSsl2Hello->cbSessionID = 0;
                        if ((pSsl2Hello->cbSessionID) &&
                            (PctFindSessIdInCache(CACHE_PROTOCOL_SSL2, 
                                                  pSsl2Hello->SessionID, 
                                                  pSsl2Hello->cbSessionID, 
                                                  &CachedSession))) {
                              pctRet = Ssl2SrvGenRestart(pContext,
                                                     pSsl2Hello,
                                                     pCommOutput,
                                                     &CachedSession);
                            if(PCT_ERR_OK == pctRet) {
                                pContext->State = SSL2_STATE_SERVER_RESTART;
                            }

                        } else {
                            pctRet = Ssl2SrvHandleClientHello(pContext,
                                                     &MsgInput,
                                                     pSsl2Hello,
                                                     pCommOutput);
                            if(PCT_ERR_OK == pctRet) {
                                pContext->State = SSL2_STATE_SERVER_HELLO;
                            }
                        }     
                        SPExternalFree(pSsl2Hello);
                    }
                    if(SP_FATAL(pctRet)) {
                        pContext->State = PCT1_STATE_ERROR;
                    } 
                    break;
                }
            case (SSL2_MT_CLIENT_MASTER_KEY << 16) | SSL2_STATE_SERVER_HELLO:
                pctRet = Ssl2SrvHandleCMKey(pContext,
                                            &MsgInput,
                                            pCommOutput);
                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                } else {
                    if(PCT_ERR_OK == pctRet) {
                        pContext->State = SSL2_STATE_SERVER_VERIFY;
                    } 
                    /* We received a non-fatal error, so the state doesn't
                     * change, giving the app time to deal with this */
                }
                break;
            case (SSL2_MT_CLIENT_FINISHED_V2 << 16) | SSL2_STATE_SERVER_VERIFY:
                pctRet = Ssl2SrvHandleClientFinish(pContext,
                                            &MsgInput,
                                            pCommOutput);
                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                } else {
                    if(PCT_ERR_OK == pctRet) {
                        pContext->State = SP_STATE_CONNECTED;
                        pContext->Encrypt = Ssl2EncryptMessage;
                        pContext->Decrypt = Ssl2DecryptMessage;
                    } 
                    /* We received a non-fatal error, so the state doesn't
                     * change, giving the app time to deal with this */
                }
                break;
            case (SSL2_MT_CLIENT_FINISHED_V2 << 16) | SSL2_STATE_SERVER_RESTART:
                pctRet = Ssl2SrvFinishClientRestart(pContext,
                                            &MsgInput,
                                            pCommOutput);
                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                } else {
                    if(PCT_ERR_OK == pctRet) {
                        pContext->State = SP_STATE_CONNECTED;
                        pContext->Encrypt = Ssl2EncryptMessage;
                        pContext->Decrypt = Ssl2DecryptMessage;
                    } 
                    /* We received a non-fatal error, so the state doesn't
                     * change, giving the app time to deal with this */
                }
                break;

#endif /* PCT_CLIENT_ONLY */
            /* Client receives Server hello */
           /* Client receives Server hello */
            case (SSL2_MT_SERVER_HELLO << 16) | SSL2_STATE_CLIENT_HELLO:
                {
                    PSsl2_Server_Hello pHello;
                    /* Attempt to recognize and handle various versions
                     * of Server hello, start by trying to unpickle the
                     * oldest, and the next version, until
                     * one unpickles.  Then run the handle code.  We can also put
                     * unpickling and handling code in here for SSL messages */
                    if(PCT_ERR_OK == (pctRet = Ssl2UnpackServerHello(
                                                &MsgInput,
                                                &pHello))) {

                       pContext->Protocol = CACHE_PROTOCOL_SSL2;
                       if(pHello->SessionIdHit) {
                             pctRet = Ssl2CliHandleServerRestart(pContext,
                                                         &MsgInput,
                                                         pHello,
                                                         pCommOutput);
                            if(PCT_ERR_OK == pctRet) {
                                pContext->State = SSL2_STATE_CLIENT_RESTART;
                            }
                           
                        } else {
                            pctRet = Ssl2CliHandleServerHello(pContext,
                                                         &MsgInput,
                                                         pHello,
                                                         pCommOutput);
                            if(PCT_ERR_OK == pctRet) {
                                pContext->State = SSL2_STATE_CLIENT_MASTER_KEY;
                            }
                        } 
                        SPExternalFree(pHello);

                    }
                    if(SP_FATAL(pctRet)) {
                        pContext->State = PCT1_STATE_ERROR;
                    }
                  
                    break;
                }
            case (SSL2_MT_SERVER_VERIFY << 16) | SSL2_STATE_CLIENT_MASTER_KEY:
                pctRet = Ssl2CliHandleServerVerify(pContext,
                                                 &MsgInput,
                                                 pCommOutput);
                if(PCT_ERR_OK == pctRet) {
                    pContext->State =SSL2_STATE_CLIENT_FINISH;
                } 
                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                } 
                break;
            case (SSL2_MT_SERVER_VERIFY << 16) | SSL2_STATE_CLIENT_RESTART:
                pctRet = Ssl2CliFinishRestart(pContext,
                                              &MsgInput,
                                              pCommOutput);
                if(PCT_ERR_OK == pctRet) {
                    pContext->State =SSL2_STATE_CLIENT_FINISH;
                } 
                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                } 
                break;

            case (SSL2_MT_SERVER_FINISHED_V2 << 16) | SSL2_STATE_CLIENT_FINISH:
                pctRet = Ssl2CliHandleServerFinish(pContext,
                                            &MsgInput,
                                            pCommOutput);
                if(SP_FATAL(pctRet)) {
                    pContext->State = PCT1_STATE_ERROR;
                } else {
                    if(PCT_ERR_OK == pctRet) {
                        pContext->State = SP_STATE_CONNECTED;
                        pContext->Encrypt = Ssl2EncryptMessage;
                        pContext->Decrypt = Ssl2DecryptMessage;
                    } 
                    /* We received a non-fatal error, so the state doesn't
                     * change, giving the app time to deal with this */
                }
                break;

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
     /* to handle incomplete message errors */
     pCommInput->cbData = MsgInput.cbData;

    return pctRet;
}

SP_STATUS WINAPI SslDecryptHandler(PPctContext pContext,
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
            }

        } else {
			return PCT_INT_ILLEGAL_MSG;
		}
	}
}
