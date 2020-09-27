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



BOOL
WINAPI
noneInitialize(  PUCHAR          pbKey,
                DWORD           dwOptions,
                PStateBuffer *  psbBuffer);

SP_STATUS
WINAPI
noneEncrypt(     PStateBuffer    psbBuffer,
                PSPBuffer      pInput,
                PSPBuffer      pOutput);

SP_STATUS
WINAPI
noneDecrypt(     PStateBuffer    psbBuffer,
                PSPBuffer      pInput,
                PSPBuffer      pOutput);

BOOL
WINAPI
noneFinish(      PStateBuffer *  psbBuffer);



const CryptoSystem    csNONE = {
    0,
    1,                      /* Blocksize (stream) */
    "None",
    noneInitialize,
    noneEncrypt,
    noneEncrypt,             /* Same operation... */
    noneFinish
    };

/* none implementation */


BOOL
WINAPI
noneInitialize(  PUCHAR          pbKey,
                DWORD           cbKey,
                PStateBuffer *  psbBuffer)
{

    DebugLog((DEB_TRACE, "Initialize NONE\n"));
    *psbBuffer = NULL;
    return(TRUE);
}


SP_STATUS
WINAPI
noneEncrypt(     PStateBuffer    psbBuffer,
                PSPBuffer      pInput,
                PSPBuffer      pOutput)
{

    if (pInput->pvBuffer != pOutput->pvBuffer)
    {
        DebugLog((DEB_WARN, "rc4Encrypt:  Unnecessary MoveMemory, performance hog"));

        if(pInput->cbData > pOutput->cbBuffer)
        {
            return (PCT_INT_BUFF_TOO_SMALL);
        }
        MoveMemory((PUCHAR)pOutput->pvBuffer,
                   (PUCHAR)pInput->pvBuffer,
                   pInput->cbData);
    }
    if(pInput->cbData > pOutput->cbBuffer)
    {
        return (SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    pOutput->cbData = pInput->cbData;

    return(PCT_ERR_OK);
}


BOOL
WINAPI
noneFinish(      PStateBuffer *  psbBuffer)
{

    *psbBuffer = NULL;

    return( TRUE );
}


