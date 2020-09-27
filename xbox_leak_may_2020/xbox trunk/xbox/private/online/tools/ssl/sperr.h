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





/* BUGBUG,TBDTBD.  We really have to straighten out
 * the error message situation */
/* basic error codes, for transmission over the 
 * communications channel */
#define PCT_ERR_OK                      0x0000
#define PCT_ERR_BAD_CERTIFICATE		    0x0001
#define PCT_ERR_CLIENT_AUTH_FAILED	    0x0002
#define PCT_ERR_ILLEGAL_MESSAGE		    0x0003
#define PCT_ERR_INTEGRITY_CHECK_FAILED	0x0004
#define PCT_ERR_SERVER_AUTH_FAILED	    0x0005
#define PCT_ERR_SPECS_MISMATCH		    0x0006
#define PCT_ERR_INCOMPLETE_CREDS        0x0007
#define PCT_ERR_SSL_STYLE_MSG		    0x00ff
#define PCT_ERR_RENEGOTIATE             0x0008
#define PCT_ERR_UNKNOWN_CREDENTIAL      0x0009

/* internal error codes, for communications with
 * the application */
#define PCT_INT_BUFF_TOO_SMALL          0x40000000
#define PCT_INT_INCOMPLETE_MSG          0x40000001  /* this specifies to the comm layer to pass
                                                     * more data */

#define PCT_INT_DROP_CONNECTION    0x80000000
#define PCT_INT_BAD_CERT           (PCT_INT_DROP_CONNECTION | PCT_ERR_BAD_CERTIFICATE)
#define PCT_INT_CLI_AUTH           (PCT_INT_DROP_CONNECTION | PCT_ERR_CLIENT_AUTH_FAILED)
#define PCT_INT_ILLEGAL_MSG        (PCT_INT_DROP_CONNECTION | PCT_ERR_ILLEGAL_MESSAGE)
#define PCT_INT_MSG_ALTERED        0x80000101
#define PCT_INT_INTERNAL_ERROR     0xffffffff
#define PCT_INT_OUT_OF_MEMORY      0xfffffffe
#define PCT_INT_DATA_OVERFLOW      0x80000102
#define PCT_INT_SPECS_MISMATCH     (PCT_INT_DROP_CONNECTION | PCT_ERR_SPECS_MISMATCH)
#define PCT_INT_INCOMPLETE_CREDS   (PCT_INT_DROP_CONNECTION | PCT_ERR_INCOMPLETE_CREDS)
#define PCT_INT_RENEGOTIATE        (PCT_INT_DROP_CONNECTION | PCT_ERR_RENEGOTIATE)
#define PCT_INT_UNKNOWN_CREDENTIAL (PCT_INT_DROP_CONNECTION | PCT_ERR_UNKNOWN_CREDENTIAL)

#define SP_FATAL(s) (PCT_INT_DROP_CONNECTION & (s))

