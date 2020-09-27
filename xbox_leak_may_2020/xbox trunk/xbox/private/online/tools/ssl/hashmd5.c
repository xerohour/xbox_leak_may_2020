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
/*-----------------------------------------------------------------------------
* RSA Public Key Cryptosystem, RC4, MD2, MD5 and RSA are trademarks
* of RSA Data Security, Inc.
*----------------------------------------------------------------------------*/

#include <spbase.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <md5.h>

#ifdef __cplusplus
}
#endif


BOOL
WINAPI
md5Initialize(  PCheckSumBuffer pBuffer,
                ULONG               dwSeed);

BOOL
WINAPI
md5Sum( PCheckSumBuffer     pcsBuffer,
        ULONG               cbData,
        PUCHAR               pbData);

BOOL
WINAPI
md5Finalize(    PCheckSumBuffer pcsBuffer,
                PUCHAR           pbSum);


const CheckSumFunction    ckMD5 = {
    CALG_MD5,
    16,
    sizeof(MD5_CTX),
    "MD5",
    md5Initialize,
    md5Sum,
    md5Finalize,
    };

BOOL
WINAPI
md5Initialize(PCheckSumBuffer     pBuffer,
              DWORD               Flags)

{
    if (!pBuffer)
    {
        return( FALSE );
    }

    MD5Init((MD5_CTX *)pBuffer);
    return( TRUE );
}

BOOL
WINAPI
md5Sum( PCheckSumBuffer     pcsBuffer,
        ULONG               cbData,
        PUCHAR               pbData)
{

    MD5Update((MD5_CTX *) pcsBuffer, pbData, cbData);
    return( TRUE );
}


BOOL
WINAPI
md5Finalize(    PCheckSumBuffer pcsBuffer,
                PUCHAR           pbSum)
{
    MD5Final((MD5_CTX *) pcsBuffer);
	CopyMemory(pbSum, ((MD5_CTX *)pcsBuffer)->digest, 16);
    return( TRUE );
}
