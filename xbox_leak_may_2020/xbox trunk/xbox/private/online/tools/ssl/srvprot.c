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


/* The main purpose of this handler is to determine what kind of protocol
 * the client hello is
 */

SP_STATUS WINAPI
ServerProtocolHandler(
    PSPContext pContext,
    PSPBuffer pCommInput,
    PSPBuffer pCommOutput,
    PSPBuffer pAppOutput)
{
    SP_STATUS pctRet = PCT_INT_ILLEGAL_MSG;
#ifndef SCHANNEL_CLIENT_ONLY    
    PUCHAR pb;
    DWORD dwVersion;

    /* PCTv1.0 Hello starts with
     * RECORD_LENGTH_MSB  (ignore)
     * RECORD_LENGTH_LSB  (ignore)
     * PCT1_CLIENT_HELLO  (must be equal)
     * PCT1_CLIENT_VERSION_MSB (if version greater than PCTv1)
     * PCT1_CLIENT_VERSION_LSB (if version greater than PCTv1) 
     *
     * ... PCT hello ...
     */


    /* Microsft Unihello starts with
     * RECORD_LENGTH_MSB  (ignore)
     * RECORD_LENGTH_LSB  (ignore)
     * SSL2_CLIENT_HELLO  (must be equal)
     * SSL2_CLIENT_VERSION_MSB (if version greater than SSLv2) ( or v3)
     * SSL2_CLIENT_VERSION_LSB (if version greater than SSLv2) ( or v3)
     *
     * ... SSLv2 Compatable Hello ...
     */

    /* SSLv2 Hello starts with
     * RECORD_LENGTH_MSB  (ignore)
     * RECORD_LENGTH_LSB  (ignore)
     * SSL2_CLIENT_HELLO  (must be equal)
     * SSL2_CLIENT_VERSION_MSB (if version greater than SSLv2) ( or v3)
     * SSL2_CLIENT_VERSION_LSB (if version greater than SSLv2) ( or v3)
     *
     * ... SSLv2 Hello ...
     */

    /* SSLv3 Type 2 Hello starts with
     * RECORD_LENGTH_MSB  (ignore)
     * RECORD_LENGTH_LSB  (ignore)
     * SSL2_CLIENT_HELLO  (must be equal)
     * SSL2_CLIENT_VERSION_MSB (if version greater than SSLv3)
     * SSL2_CLIENT_VERSION_LSB (if version greater than SSLv3)
     *
     * ... SSLv2 Compatable Hello ...
     */

    /* SSLv3 Type 3 Hello starts with
     * 0x15 Hex (HANDSHAKE MESSAGE)
     * VERSION MSB
     * VERSION LSB
     * RECORD_LENGTH_MSB  (ignore)
     * RECORD_LENGTH_LSB  (ignore)
     * HS TYPE (CLIENT_HELLO)
     * 3 bytes HS record length
     * HS Version
     * HS Version
     */

    // We need at least 5 bytes to determine what we have.
    if (pCommInput->cbData < 5)
    {
        return(PCT_INT_INCOMPLETE_MSG);
    }

    pb = pCommInput->pvBuffer;
    // If the first byte is 0x15, then check if we have a
    // SSLv3 Type3 client hello
    
    if(pb[0] == SSL3_CT_HANDSHAKE)
    {
        if(!(SP_PROT_SSL3_SERVER & g_ProtEnabled))
        {
            return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
        }
        // SSLv3 message
        dwVersion = COMBINEBYTES(pb[1], pb[2]);
        pContext->ProtocolHandler = Ssl3ServerProtocolHandler;
        pContext->DecryptHandler = Ssl3DecryptHandler;
        return(Ssl3ServerProtocolHandler(pContext, pCommInput, pCommOutput, pAppOutput));
   }
 	else
 	{
        // SSL type header, so get the version
        dwVersion = COMBINEBYTES(pb[3], pb[4]);
 	}
    if(dwVersion >= PCT_VERSION_1)
    {
#ifdef SCHANNEL_PCT    
        if(!(SP_PROT_PCT1_SERVER & g_ProtEnabled))
        {
            return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
        }
        // We're running PCT, so set up the PCT handlers
        pContext->ProtocolHandler = Pct1ServerProtocolHandler;
        pContext->DecryptHandler = Pct1DecryptHandler;
        return(Pct1ServerProtocolHandler(pContext, pCommInput, pCommOutput, pAppOutput));
#else
        return SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG);
#endif        
    }
    if(dwVersion >= SSL2_CLIENT_VERSION)
    {
        // we're either receiving ssl2, ssl3, or pct1 compat

        PSSL2_CLIENT_HELLO pRawHello = pCommInput->pvBuffer;

        // Do we have one client hello message with at least one
        // cipher spec.
        if (pCommInput->cbData < (sizeof(SSL2_CLIENT_HELLO)+2))
        {
            return(PCT_INT_INCOMPLETE_MSG);
        }

        // We must have at least one cipher spec
        if(COMBINEBYTES(pRawHello->CipherSpecsLenMsb, pRawHello->CipherSpecsLenLsb)< 1)
        {
            return(SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG));
        }

        // Is the PCT compatability flag set.
        if(pRawHello->VariantData[0] == PCT_SSL_COMPAT)
        {
            // Get the pct version.
            dwVersion = COMBINEBYTES(pRawHello->VariantData[1], pRawHello->VariantData[2]);
        }
        if((dwVersion >= PCT_VERSION_1) &&
           (0 != (SP_PROT_PCT1_SERVER & g_ProtEnabled)))
        {
#ifdef SCHANNEL_PCT        
            pContext->State = UNI_STATE_RECVD_UNIHELLO;
            pContext->ProtocolHandler = Pct1ServerProtocolHandler;
            pContext->DecryptHandler = Pct1DecryptHandler;
            return(Pct1ServerProtocolHandler(pContext, pCommInput, pCommOutput, pAppOutput));
#else
            return (SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG));
#endif            
        }

        else 

        if((dwVersion >= SSL3_CLIENT_VERSION) &&
           (0 != (SP_PROT_SSL3_SERVER & g_ProtEnabled)))
        {
            pContext->State = UNI_STATE_RECVD_UNIHELLO;
            pContext->ProtocolHandler = Ssl3ServerProtocolHandler;
            pContext->DecryptHandler = Ssl3DecryptHandler;
            return(Ssl3ServerProtocolHandler(pContext, pCommInput, pCommOutput, pAppOutput));
        }
        if((dwVersion >= SSL2_CLIENT_VERSION) &&
           (0 != (SP_PROT_SSL2_SERVER & g_ProtEnabled)))
        {
            pContext->ProtocolHandler = Ssl2ServerProtocolHandler;
            pContext->DecryptHandler = Ssl2DecryptHandler;
            return(Ssl2ServerProtocolHandler(pContext, pCommInput, pCommOutput, pAppOutput));
       }
        return(SP_LOG_RESULT(PCT_INT_ILLEGAL_MSG));

    }
#endif // !SCHANNEL_CLIENT_ONLY    
    return (pctRet);
}
