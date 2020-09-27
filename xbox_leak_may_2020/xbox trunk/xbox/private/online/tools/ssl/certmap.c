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

#include "spbase.h"

INIT_CERT_MAPPER_FN         g_InitCertMapper = NULL;
DELETE_ISSUER_LIST_FN       g_DeleteIssuerList = NULL;
TERMINATE_CERT_MAPPER_FN    g_TerminateCertMapper = NULL;
CERT_MAPPER_FN              g_CertMapper = NULL;
REFRESH_TOKEN_FN            g_RefreshToken = NULL;
CLOSE_TOKEN_HANDLE_FN       g_CloseTokenHandle = NULL;

HINSTANCE g_hCertMapper = NULL;

BOOL 
CERT_API
InitCertMapper(
    IssuerAccepted ** ppIssuer,
    DWORD           * pdwIssuer
    )
{
    DWORD dwSize;
    DWORD err;
    TCHAR pszCertMapperPath[512];
    DWORD dwType;

    dwSize = sizeof(pszCertMapperPath);
    err = RegQueryValueEx(g_hkBase, SP_REG_VAL_CERTMAPPER, NULL, &dwType, (PUCHAR)pszCertMapperPath, &dwSize);
    if(err) {
        return FALSE;
    }


    g_hCertMapper = LoadLibrary(pszCertMapperPath);

    if(g_hCertMapper == NULL)
    {
        return FALSE;
    }

    g_InitCertMapper =      (INIT_CERT_MAPPER_FN)GetProcAddress(g_hCertMapper, INIT_CERT_MAPPER_FN_NAME);
    g_DeleteIssuerList =    (DELETE_ISSUER_LIST_FN)GetProcAddress(g_hCertMapper, DELETE_ISSUER_LIST_FN_NAME);
    g_TerminateCertMapper = (TERMINATE_CERT_MAPPER_FN)GetProcAddress(g_hCertMapper, TERMINATE_CERT_MAPPER_FN_NAME);
    g_CertMapper =          (CERT_MAPPER_FN)GetProcAddress(g_hCertMapper, CERT_MAPPER_FN_NAME);
    g_RefreshToken =        (REFRESH_TOKEN_FN)GetProcAddress(g_hCertMapper, REFRESH_TOKEN_FN_NAME);
    g_CloseTokenHandle =    (CLOSE_TOKEN_HANDLE_FN)GetProcAddress(g_hCertMapper, CLOSE_TOKEN_HANDLE_FN_NAME);


    if(!g_InitCertMapper)
    {
        return FALSE;
    }
    return (*g_InitCertMapper)(ppIssuer, pdwIssuer);
}

BOOL 
CERT_API
DeleteIssuerList(
    IssuerAccepted * pIssuer,
    DWORD dwIssuer
    )
{
    if(g_DeleteIssuerList)
    {
        return (*g_DeleteIssuerList)(pIssuer, dwIssuer);
    }
    return FALSE;
}

BOOL 
CERT_API
TerminateCertMapper(
    VOID
    )
{
    BOOL fResult = FALSE;
    if(g_TerminateCertMapper)
    {
        fResult = (*g_TerminateCertMapper)();

    }
    if(g_hCertMapper)
    {
        FreeLibrary(g_hCertMapper);
    }
    g_InitCertMapper = NULL;
    g_DeleteIssuerList = NULL;
    g_TerminateCertMapper = NULL;
    g_CertMapper = NULL;
    g_RefreshToken = NULL;
    g_CloseTokenHandle = NULL;


    return fResult;
}

BOOL 
CERT_API
CertMapper(
    PCert_Map  pCert,
    DWORD    * pdwUserHandle,
    HANDLE   * phToken,
    LPSTR      pszUserName,
    LPDWORD    pdwNameLen
    )
{
    if(g_CertMapper)
    {
        return (*g_CertMapper)(pCert, pdwUserHandle, phToken, pszUserName, pdwNameLen);
    }
    return FALSE;
}


BOOL 
CERT_API
RefreshToken( 
    DWORD    dwUserHandle, 
    HANDLE * phToken
    )
{
    if(g_RefreshToken)
    {
        return (*g_RefreshToken)(dwUserHandle, phToken);
    }
    return FALSE;
}

BOOL 
CERT_API
CloseTokenHandle( 
    DWORD dwUserHandle
    )
{
    if(g_CloseTokenHandle)
    {
        return (*g_CloseTokenHandle)(dwUserHandle);
    }
    return FALSE;
}

