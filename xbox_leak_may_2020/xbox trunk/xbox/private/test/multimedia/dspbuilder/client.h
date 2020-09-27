/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	client.h

Abstract:

	Client code for remote connection to Xbox for dsp builder

Author:

	Matt Bronder

Revision History:

	mattbron
		Initial Version
	16-May-2001	robheit
		Customized for dsp builder

--*/

#ifndef __CLIENT_H__
#define __CLIENT_H__

//------------------------------------------------------------------------------
//	Defines:
//------------------------------------------------------------------------------
#define PORT_SERVICE        2202
#define PORT_ENUMERATE      2203
#define ARG_BUFFER_SIZE     512

//------------------------------------------------------------------------------
//	CClient:
//------------------------------------------------------------------------------
class CClient 
{
public:

	CClient(void);
    virtual ~CClient(void);

    BOOL Create(void);

    void Disconnect(void);
    BOOL IsConnected(void);

    void EnableMethodCalls(BOOL bEnable);
    BOOL MethodCallsEnabled(void);

    BOOL CallMethod(METHODID methodID, LPVOID pvObject, ...);

    int SendPacket(PPACKET ppk, SOCKET s = INVALID_SOCKET);
    BOOL RecvPacket(PPACKET ppk = NULL, SOCKET s = INVALID_SOCKET);

    int Send(LPVOID pv, int nSize, SOCKET s = INVALID_SOCKET);
    int Recv(LPVOID pv, int nSize, SOCKET s = INVALID_SOCKET);

private:

    SOCKET			m_sEnumerate;
    SOCKET			m_sListen;
    SOCKET			m_sServer;
    BOOL			m_bWSAStartup;
    BOOL			m_bMCallsEnabled;
    PPACKET			m_pPacket;
    PACKET_ENUM		m_pkEnum;
	static LPCSTR	m_szSigs[];

};

//------------------------------------------------------------------------------
//	Structures
//------------------------------------------------------------------------------
struct SLOCKDESC 
{
    D3DLOCKED_RECT	d3dlr;
    RECT            rect;
    DWORD           dwFlags;
    D3DLOCKED_RECT  d3dlrSrv;
};

struct VLOCKDESC 
{
    D3DLOCKED_BOX	d3dlb;
    D3DBOX          box;
    DWORD           dwFlags;
    D3DLOCKED_BOX   d3dlbSrv;
};

struct RLOCKDESC 
{
    LPBYTE	pData;
    UINT    uSize;
    LPBYTE  pDataSrv;
};

struct ILOCKDESC 
{
    LPBYTE	pData;
    UINT    uSize;
    LPBYTE  pDataSrv;
};

int gethostaddr(IN_ADDR* psin);
int gethostaddr(char* addr, int addrlen);

#endif
