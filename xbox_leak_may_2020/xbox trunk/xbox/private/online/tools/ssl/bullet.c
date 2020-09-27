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

#include <stdlib.h>
#include <spbase.h>
#include <wincrypt.h>

#ifdef ENABLE_SELECTIVE_CRYPTO
#include <scrypt.h>

//  Isser:
//      2.5.4.6=US
//      2.5.4.10=Bolt Beranek & Newman, Inc.
//      2.5.4.3=Serial No. 950140 283
//  Signature Algorithm: 1.2.840.113549.1.1.2
//  Subject:
//      2.5.4.3=Evil


static BYTE g_SCVerifyKey[] = {
        0x01, 0x00, 0x00, 0x00,
        0x9c, 0x00, 0x00, 0x00,
        'R', 'S', 'A', '1',			// key data....
        0x88, 0x00, 0x00, 0x00,     // key length
        0x00, 0x04, 0x00, 0x00,     // bit length
        0x7f, 0x00, 0x00, 0x00,     // data length
        0x01, 0x00, 0x01, 0x00,     // public exponent.
        0x51, 0x01, 0x98, 0x56, 0x37, 0xc6, 0xa2, 0x0c,  // modulus
        0x21, 0x91, 0xfb, 0x99, 0xcd, 0xbb, 0x2d, 0x4f,
        0x96, 0x0f, 0xf2, 0x02, 0xaa, 0x5a, 0x6e, 0xe0,
        0x4b, 0x2e, 0x5a, 0x0a, 0xf3, 0x6f, 0x29, 0xce,
        0x5b, 0xa1, 0x8e, 0xc6, 0xcf, 0x21, 0xe4, 0xe1,
        0x1e, 0x6a, 0xc5, 0xed, 0x86, 0x7b, 0xd1, 0x72,
        0x25, 0x4e, 0x20, 0x3b, 0x6f, 0xca, 0x38, 0x8d,
        0x8d, 0x29, 0x63, 0xb9, 0x17, 0x80, 0x64, 0xeb,
        0xde, 0x40, 0x29, 0xca, 0x40, 0x48, 0x40, 0x7a,
        0xc0, 0xce, 0xcb, 0x7a, 0x69, 0x42, 0x24, 0xc9,
        0xff, 0xe4, 0x3d, 0x79, 0x68, 0x7d, 0x69, 0x65,
        0xde, 0x81, 0x54, 0x94, 0x38, 0x4e, 0x84, 0xd4,
        0x75, 0xa5, 0x94, 0x41, 0xbe, 0xc8, 0xce, 0x49,
        0xe4, 0x8e, 0x3e, 0x8b, 0xdc, 0xf8, 0x8d, 0x66,
        0xf2, 0xcb, 0x80, 0x1c, 0x85, 0xb4, 0x0c, 0xe3,
        0x15, 0xb8, 0xec, 0xdf, 0xf6, 0x60, 0xcb, 0xc2,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};





BOOL VerifyFinCert(LPCTSTR Filename, BYTE *Sig, DWORD SigSize)
{
    HANDLE              hFile, hMap;
    LPBYTE              lpContents;
    DWORD               dwSize;
    BYTE                FileHash[16];
    BOOL                bSigWorked;
    
    bSigWorked = FALSE;
    hFile = INVALID_HANDLE_VALUE;
    hMap = NULL;
    lpContents = NULL;
    
    // check the file.  
    do
    {


        hFile = CreateFile(Filename,
                           GENERIC_READ,
                           FILE_SHARE_READ,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
			   0);

        if (hFile == INVALID_HANDLE_VALUE)
            break;

        dwSize = GetFileSize(hFile, NULL);
        
        hMap = CreateFileMapping(hFile,
                                 NULL,
                                 PAGE_WRITECOPY,
                                 0,
                                 0,
                                 NULL);

        if (hMap == NULL)
            break;
        
        lpContents = (LPBYTE)MapViewOfFile(hMap, FILE_MAP_COPY, 0, 0, 0);

        if (lpContents == NULL)
            break;
        
        bSigWorked = sigRSAMD5.Verify(lpContents,
                                      dwSize,
                                      Sig,
                                      SigSize,
                                      (PctPublicKey *)g_SCVerifyKey);
	
	    break;
	
    } while(1);

    if (lpContents)
        UnmapViewOfFile(lpContents);

    if (hMap != NULL)
        CloseHandle(hMap);

    if (hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);

    return bSigWorked;
}

#endif

SP_STATUS
SPEnableFinanceCipher(VOID)
{
    
#ifdef ENABLE_SELECTIVE_CRYPTO
    HMODULE     hCaller;
    TCHAR       pszModFile[MAX_PATH_LEN], *pszPureFile;
    DWORD       dwPathLen, i, dwType, dwLen, err, disp;
    PBYTE       SCSig;

    
    // we need to look for a financial certificate for the application.

    // find the application
    hCaller = GetModuleHandle(NULL);
    dwPathLen = GetModuleFileName(hCaller, pszModFile, MAX_PATH_LEN);
        
    pszPureFile = pszModFile+dwPathLen;

    // grab the filename from the path.
    while(pszPureFile > pszModFile)
    {
        if(*(pszPureFile-1) == '\\')
        {
            break;
        }
        pszPureFile--;
        
    }

    SCSig = NULL;
        
    while(TRUE)
    {
        dwLen = 0;
        err = RegQueryValueEx(g_hkBase,
                              pszPureFile,
                              NULL,
                              &dwType,
                              NULL,
                              &dwLen);

        if(err || (dwLen == 0))
        {
            break;
        }
        SCSig = SPExternalAlloc(dwLen);
        if(SCSig == NULL)
        {
            break;
        }

        err = RegQueryValueEx(g_hkBase,
                              pszPureFile,
                              NULL,
                              &dwType,
                              SCSig,
                              &dwLen);

        if(err || (dwLen == 0))
        {
            break;
        }

        // We have a signature for this file.

        if (VerifyFinCert(pszModFile, SCSig, dwLen))
        {
            PCipherInfo pInfo;
            pInfo = GetCipherInfo(SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128);
            pInfo->fProtocol |= SP_PROT_CLIENTS;
        }
        break;
    }

    if(SCSig != NULL)
    {
        SPExternalFree(SCSig);
    }
    
#endif
    return (PCT_ERR_OK);
}


