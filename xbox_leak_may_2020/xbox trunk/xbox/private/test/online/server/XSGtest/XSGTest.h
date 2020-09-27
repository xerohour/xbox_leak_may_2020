//------------------------------------------------------------------------------
// XBox Online
//
// File:		XSGTest.h
//
// Copyright:	Copyright (c) 2001 Microsoft Corporation
//
// Contents:	Declarations for XSGTest.cpp.
//
// History:	01/02/2002	johnblac created
//
//------------------------------------------------------------------------------

#ifndef XSGTEST_H
#define XSGTEST_H

#include "StdAfx.h"

struct IP_HEADER
{
	BYTE bVersion;
	BYTE bTypeOfService;
	WORD wLength;			// IPheader + UDP header + data
	WORD wIdentification;
	WORD wFlagsAndOffset;	// first 3 bits are flags, 13-bits for frag offset
	BYTE bTTL;				// Time to Live
	BYTE bProtocol;
	WORD wChecksum;
	DWORD dwSrcAddress;
	DWORD dwDestAddress;	
};

struct ENET_HEADER
{
	BYTE rgbDestMAC[6];
	BYTE rgbSrcMAC[6];
	WORD wEnetType;
};

struct UDP_HEADER
{
	WORD wSrcPort;
	WORD wDestPort;
	WORD wLength;		// UDP header + data
	WORD wChecksum;
};

BOOL atoh(BYTE* pDestHex, WCHAR* pSrcChar, UINT uiLen);
void CopyHeaders(BYTE* prgbFrameBuffer, ENET_HEADER* pEnetHdr, UINT uiEnetSize, IP_HEADER* pIPHdr, UINT uiIPSize, UDP_HEADER* pUDPHdr, UINT uiUDPSize);void E(HRESULT hr);
bool GetLocalMachineAddress();
void InitGlobalArray();
HRESULT InitializeNetworkUDP(SOCKET* ps1, SOCKADDR_IN* psXCSAddress);
HRESULT InitializeNetworkTCP(SOCKET* ps1, SOCKADDR_IN* psXCSAddress, u_short uPort);
HRESULT ReceiveFromCS(SOCKET sSocket, BYTE* pBuffer, DWORD dwLength, DWORD* pdwBytesRecvd);
void SetNetworkHeaders(ENET_HEADER* pEnetHdr, IP_HEADER* pIPHdr, UDP_HEADER* pUDPHdr, WORD wDataLen);
HRESULT RunTestCases(SOCKET* ps1, SOCKADDR_IN* psXCSAddress);
HRESULT SendToCS(SOCKET	sSocket,	SOCKADDR_IN sAddr, const BYTE* pBuffer, DWORD cchBuffer);
void SetKeysToValidState(CKeyExHdr* pFirstKeyExHdr, CKeyExHdr* pSecondKeyExHdr, CKeyExHdr* pThirdKeyExHdr);
HRESULT SGEncryption(SOCKET* pSocket, SOCKADDR_IN* psXSGAddress, int nTestNum);
HRESULT SGKeyOrder(SOCKET* pSocket, SOCKADDR_IN* psXSGAddress, int nTestNum);
HRESULT SGRouting(SOCKET* pSocket, SOCKADDR_IN* psXSGAddress, int nTestNum);
HRESULT SGTicketValidity(SOCKET* pSocket, SOCKADDR_IN* psXSGAddress, int nTestNum);
HRESULT SGValidity(SOCKET* pSocket, SOCKADDR_IN* psXSGAddress, int nTestNum);
HRESULT WriteBadFrame(BYTE* pBuff, char* pbHexString, DWORD wHexStringSize, WORD wBufferLength);

const UINT k_uiNumHeartBeats = 3;
const DWORD k_dw2048const = 2048;
const DWORD k_dwDHKeySize = 96;
const WORD k_w2048const = 2048;
const WORD k_w1522const = 1522;
const DWORD k_dw5secTimeout = 5000;
const DWORD k_dw60secTimeout = 60000;
const DWORD k_dwXBoxHeartbeat = 10000;
const DWORD k_dwCSHeartbeat = 20000;
const DWORD k_dwClientTimeout = 600000;
const long k_lMSTimeout = 65;
const UINT k_uiSocketTimeout = 5000;
const UINT k_uiMSRetryMax = 5;

HRESULT E_NOTHING_RECEIVED =9600;

#endif // XSGTEST_H
