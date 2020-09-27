//
//  Copyright (C) Microsoft Corporation, 1995 - 1997
//
//  File:     frmtfunc.h
//
//  Contents:   The header for frmtfunc.cpp
//
//  History:    Sept. 1st, 1997
//              
//--------------------------------------------------------------------------

#ifndef __FRMTFUNC_H__
#define __FRMTFUNC_H__

#include <wchar.h> 
        
#include "base64.h"
#include "wintrust.h"
#include "mssip.h"
#include "sipbase.h"
#include "pfx.h"


#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
//	 constants
//--------------------------------------------------------------------------
#define	    UPPER_BITS			                        0xF0
#define	    LOWER_BITS			                        0x0F
#define	    CHAR_SIZE			                        2
#define	    HEX_SIZE			                        3    

//for szOID_BASIC_CONSTRAINTS2
#define		SUBJECT_SIZE								256
#define		NONE_SIZE									256
   
//for szOID_CRL_REASON_CODE
#define		CRL_REASON_SIZE								256

//for szOID_ENHANCED_KEY_USAGE
#define     NO_INFO_SIZE                                256

//for szOID_ALT_NAME
#define     UNKNOWN_VALUE_SIZE                          256
#define     ALT_NAME_SIZE                               256

//for SPC_FINANCIAL_CRIERIA
#define     AVAIL_SIZE                                  256
#define     YES_NO_SIZE                                 256

//for Key Usage                                         
#define     KEY_USAGE_SIZE                              256
#define     UNKNOWN_ACCESS_METHOD_SIZE                  256
#define     UNKNOWN_KEY_USAGE_SIZE                      256
#define     DAY_SIZE                                    256
#define     MONTH_SIZE                                  256
#define     CRL_DIST_NAME_SIZE                          256
#define     UNKNOWN_CRL_REASON_SIZE                     256
#define     PRE_FIX_SIZE                                256
//----------------------------------------------------------------------------
//	 WCHAR string contants
//--------------------------------------------------------------------------

//used for formatting
#define	wszPLUS			L" + "
#define	wszCOMMA		L" , "
#define	wszSEMICOLON	L" ; "
#define	wszCRLF			L"\r\n"
#define	wszEQUAL		L"="
#define	strCOMMA			", "
//#define szNEWLN         "\n"
#define wszNEWLN        L"\n"


#define BEGINCERT_W                 L"-----BEGIN CERTIFICATE-----"
#define CBBEGINCERT_W               (sizeof(BEGINCERT_W)/sizeof(WCHAR) - 1)

#define BEGINCERT_A                 "-----BEGIN CERTIFICATE-----"
#define CBBEGINCERT_A               (sizeof(BEGINCERT_A)/sizeof(CHAR) - 1)


//---------------------------------------------------------
//	The following is used by this dll
//
//-----------------------------------------------------------  

const DWORD	g_AllocateSize=128*sizeof(WCHAR);

//---------------------------------------------------------------------------
//
//	Unitlity functions used by the dll
//---------------------------------------------------------------------------								
BOOL	DecodeGenericBLOB(DWORD dwEncodingType, LPCSTR lpszStructType,
			const BYTE *pbEncoded, DWORD cbEncoded,void **ppStructInfo); 
BOOL	FormatMessageUnicode(LPWSTR	*ppwszFormat,UINT ids,...);
HRESULT	SZtoWSZ(LPSTR szStr,LPWSTR *pwsz);
DWORD   FormatToStr(DWORD   dwFormatType);
//BOOL	FormatMessageStr(LPSTR	*ppszFormat,UINT ids,...);

BOOL    GetCertNameMulti(LPWSTR          pwszNameStr,
                         UINT            idsPreFix, 
                         LPWSTR          *ppwsz);

BOOL	FormatFileTime(FILETIME *pFileTime,LPWSTR   *ppwszFormat);

BOOL    FormatCertPolicyID(PCERT_POLICY_ID pCertPolicyID, LPWSTR    *ppwszFormat);

BOOL    FormatCRLReason(DWORD		    dwCertEncodingType,
	                    DWORD		    dwFormatType,
	                    DWORD		    dwFormatStrType,
	                    void		    *pFormatStruct,
	                    LPCSTR		    lpszStructType,
                        PCRYPT_BIT_BLOB pInfo,
                        LPWSTR          *ppwszFormat);



static BOOL
WINAPI
FormatBytesToHex(
	DWORD		dwCertEncodingType,
	DWORD		dwFormatType,
	DWORD		dwFormatStrType,
	void		*pFormatStruct,
	LPCSTR		lpszStructType,
	const BYTE *pbEncoded, 
	DWORD		cbEncoded,
	void	   *pbFormat,
	DWORD	   *pcbFormat);


BOOL FormatAltNameInfo(
    DWORD		            dwCertEncodingType,
	DWORD		            dwFormatType,
    DWORD                   dwFormatStrType,
    void	            	*pFormatStruct,
    UINT                    idsPreFix,
    BOOL                    fNewLine,
    PCERT_ALT_NAME_INFO	    pInfo,
    void	                *pbFormat,
	DWORD	                *pcbFormat);

static BOOL
WINAPI
FormatKeyUsageBLOB(
	DWORD		    dwCertEncodingType,
	DWORD		    dwFormatType,
	DWORD		    dwFormatStrType,
	void		    *pFormatStruct,
	LPCSTR		    lpszStructType,
    PCRYPT_BIT_BLOB	pInfo,
	void	        *pbFormat,
	DWORD	        *pcbFormat);

BOOL    FormatDistPointName(
    DWORD		            dwCertEncodingType,                         
	DWORD		            dwFormatType,                               
	DWORD		            dwFormatStrType,                            
	void		            *pFormatStruct,                             
    PCRL_DIST_POINT_NAME    pInfo,                                      
    LPWSTR                  *ppwszFormat);    

BOOL FormatCertQualifier(
	DWORD		                    dwCertEncodingType,
	DWORD		                    dwFormatType,
	DWORD		                    dwFormatStrType,
	void		                    *pFormatStruct,
    PCERT_POLICY_QUALIFIER_INFO     pInfo,
    LPWSTR                          *ppwszFormat);

BOOL FormatSPCObject(
	DWORD		                dwFormatType,
	DWORD		                dwFormatStrType,
    void		                *pFormatStruct,
    UINT                        idsPrefix,
    PSPC_SERIALIZED_OBJECT      pInfo,
    LPWSTR                      *ppwszFormat);

BOOL FormatSPCLink(
	DWORD		dwFormatType,
	DWORD		dwFormatStrType,
    void		*pFormatStruct,
    UINT        idsPrefix,
    PSPC_LINK   pInfo,
    LPWSTR      *ppwszFormat);

BOOL FormatSPCImage(
	DWORD		dwFormatType,
	DWORD		dwFormatStrType,
    void		*pFormatStruct,
    UINT        idsPrefix,
    PSPC_IMAGE  pInfo,
    LPWSTR      *ppwszImageFormat);


BOOL	CryptDllFormatNameAll(  
				DWORD		dwEncodingType,	
				DWORD		dwFormatType,
				DWORD		dwFormatStrType,
				void		*pStruct,
                UINT        idsPreFix,
                BOOL        fToAllocate,
				const BYTE	*pbEncoded,
				DWORD		cbEncoded,
				void		**ppbBuffer,
				DWORD		*pcbBuffer);

                          


#ifdef __cplusplus
}       // Balance extern "C" above
#endif


#endif // __FRMTFUNC_H__
