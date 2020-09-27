// prodinfo.cpp - Product ID 
//
// Modified from ACME sources for use in Microsoft Developer 4.0
// July 6, 1995 [PaulDe]
//
// - Add HMODULE parameter to support PIDs in multiple DLLs.
// - Correct resource load code according to Win32 docs/standards
// - Cleanup resource
// - Add debug TRACE (Check macro, etc.)
// - Hide ACME behind CProductInfo class
//
/////////////////////////////////////////////////////////////

// original header >>>>
///***************************************************************************
//**
//**	File:		SETUPINI.C
//**	Purpose:	Tool to validate and generate ACME Copy Disincentive files
//**	Notes:      Intended for use outside of ACME, therefore this file
//**	            duplicates some size defines in CPYDIS.H, must keep in sync!
//**
//****************************************************************************/
//
//#pragma warning(disable:4514)/* unreferenced inline function has been removed */
//#pragma warning(disable:4201)/* nonstandard extension: nameless struct/union */
//#pragma warning(disable:4214)/* nonstandard extension: bit field types other than int */
//#pragma warning(disable:4209)/* nonstandard extension: benign typdef redefinition */
//#pragma warning(disable:4115)/* named type definition in parentheses */
//#include <windows.h>         /* produces warnings at warning level /W4 */
//#pragma warning(default:4201)
//#pragma warning(default:4214)
//#pragma warning(default:4209)
//#pragma warning(default:4115)
//<<<<

#include "stdafx.h"

typedef char *	        SZ;
//typedef UINT	CB;

//#include <stdio.h>  /* printf() */
//#include <stdlib.h> /* exit() */
//#include <string.h> /* strchr() */
//#include <time.h>		/* time_t, time */

#define cbCDDate        10  /* YYYY-MM-DD */
#define cbCDUserNameMax 52
#define cbCDOrgNameMax  52
#define cbRawPID        20  /* ASCII DIGITS (and "OEM") */
#define cbFormattedPID  23  /* RPCNO-LOC-SERIALX-SEQNC */
#define cchpSetupIni   149  /* exact size of SETUP.INI */
#define cbCDHeader      15  /* size of SETUP.INI header bytes */
#define cbCDEncrypt    128  /* encrypted byte count in SETUP.INI */

SZ szGarbage  = "LtRrBceHabCT AhlenN";

#define Check( fcond, szMsg ) \
if( !(fcond) )\
	{\
		TRACE0(szMsg);\
		FreeResource((HGLOBAL)rgchBuf);\
		return FALSE;\
	} else

/***************************************************************************/
BOOLEAN ACMEDecrypt ( HMODULE hMod, UINT ResType, UINT ResNum,
		      char *szName, char *szOrg, char *szPid )
{
	BYTE   *rgchBuf;
	CHAR   pchTmp[cchpSetupIni];
	USHORT wYear;
	USHORT wMonth;
	USHORT wDay;
	BYTE   ch;
	CHAR*  pchCur;
	SZ     szGarbageCur;
	INT    cchName, cchOrg, i, j;
	INT    chksumName, chksumOrg, chksumNameNew, chksumOrgNew;
	HRSRC hrsc;

	/** FILE READ **/
	hrsc = FindResource( hMod, MAKEINTRESOURCE(ResNum), MAKEINTRESOURCE(ResType) );
	if (hrsc == NULL) goto Cant;

	rgchBuf = (BYTE *)LoadResource( hMod, hrsc );
	if( rgchBuf == NULL )
	{
Cant:	TRACE("PID:Can't load resource (Module=0x%Lx Type=0x%Lx Id=0x%Lx)\n", (ULONG)hMod, (ULONG)ResType, (ULONG)ResNum );
		return FALSE;
	}

	/** PID **/
	Check(rgchBuf[cchpSetupIni-1] == '\0' && lstrlen((SZ)rgchBuf + cbCDEncrypt) == cbRawPID, 
		"PID:Invalid PID length\n");
	lstrcpy(szPid, (SZ)rgchBuf + cbCDEncrypt);
	if (szPid[5] != '-' && szPid[17] != ' ')
		{
		szPid[5]  = '-';
   	lstrcpy(szPid+6,  (SZ)rgchBuf + cbCDEncrypt + 5);
		szPid[9]  = '-';
   	lstrcpy(szPid+10, (SZ)rgchBuf + cbCDEncrypt + 8);
		szPid[17] = '-';
   	lstrcpy(szPid+18, (SZ)rgchBuf + cbCDEncrypt + 15);
		}

	for (pchCur = szPid + 8 + 2, i=0, j=7; j-- != 0; pchCur++)
		i += (*pchCur == 'x' ? 0 : *pchCur - '0');
	Check(i % 7 == 0, "Invalid serial number check digit");

	/** DECRYPTION **/
	for (i = j = cbCDEncrypt-1; i >= 0; j = (j + 17) & 0x7F)
		pchTmp[i--] = (CHAR)rgchBuf[j];
	for (i = cbCDEncrypt-2; i-- > 0;)
		pchTmp[i + 1] = (CHAR)(pchTmp[i] ^ pchTmp[i + 1]);

	/** USERNAME **/
	pchCur = pchTmp + cbCDHeader;
	cchName = ((*(pchTmp + 2) - 'e') << 4) + (*(pchTmp + 1) - 'e');
	Check(cchName != 0 && cchName <= cbCDUserNameMax, "PID:Invalid user name length\n");
	for (i = 0, chksumNameNew = 0; i < cchName; i++)
		if ((ch = szName[i] = *pchCur++) < ' ')
			Check(1, "PID:Invalid character in user name\n");
		else
			chksumNameNew += ch;
	szName[i] = '\0';

	chksumName = ((*(pchTmp + 6) - 'e') << 4) + (*(pchTmp + 5) - 'e');
	Check(chksumName == (chksumNameNew & 0x0FF), "PID:Invalid user name checksum\n");

	/** COMPANYNAME **/
	cchOrg = ((*(pchTmp + 4) - 'e') << 4) + (*(pchTmp + 3) - 'e');
	Check(cchOrg != 0 && cchOrg <= cbCDOrgNameMax, "PID:Invalid company name length\n");
	for (i = 0, chksumOrgNew = 0; i < cchOrg; i++)
		if ((ch = szOrg[i] = *pchCur++) < ' ')
			Check(1, "PID:Invalid character in company name\n");
		else
			chksumOrgNew += ch;
	szOrg[i] = '\0';

	chksumOrg = ((*(pchTmp + 8) - 'e') << 4) + (*(pchTmp + 7) - 'e');
	Check(chksumOrg == (chksumOrgNew & 0x0FF), "PID:Invalid company name checksum\n");

	/** VALIDATE FILLER **/
	szGarbageCur = szGarbage;
	while (pchCur < pchTmp + cbCDEncrypt-1)
		{
		if (*szGarbageCur == '\0')
			szGarbageCur = szGarbage;
		Check(*pchCur++ == *szGarbageCur++, "PID:Invalid filler character\n");
		}
	Check(*pchCur == '\0', "PID:Encryption not null terminated\n");

	/** DATE **/
	if (pchTmp[9] == (CHAR)0xFF && pchTmp[10] == (CHAR)0xFF)
		;
	else
		{
		wDay = (USHORT)(((*(pchTmp + 10) - 'e') << 4) + (*(pchTmp + 9) - 'e'));
		Check(wDay >= 1 && wDay <= 31, "PID:Invalid day in date\n");
		wMonth = (USHORT)(*(pchTmp + 11) - 'e');
		Check(wMonth >= 1 && wMonth <= 12, "PID:Invalid month in date\n");
		wYear = (USHORT)((((*(pchTmp + 14) - 'e') & 0x0F) << 8) +
				  (((*(pchTmp + 13) - 'e') & 0x0F) << 4) +
				  (*(pchTmp + 12) - 'e'));
		Check(wYear >= 1900 && wYear <= 4096, "PID:Invalid year in date\n");
		}

	FreeResource((HGLOBAL)rgchBuf);
	return TRUE;
}


/////////////////////////////////////////////////////////////
// class CProductInfo

BOOL CProductInfo::Create(HMODULE hModule, UINT nId, UINT nType)
{
	if (!m_bValid)
		m_bValid =  ACMEDecrypt( hModule, nType, nId, m_szUser, m_szOrg, m_szPID );
	return m_bValid;
}

BOOL CProductInfo::CopyInfo(LPTSTR szPID, LPTSTR szUserName, LPTSTR szOrganization) const
{
	if (m_bValid)
	{
		if (szPID) _tcscpy(szPID,m_szPID);
		if (szUserName) _tcscpy(szUserName,m_szUser);
		if (szOrganization) _tcscpy(szOrganization,m_szOrg);
	}
	return m_bValid;
}
