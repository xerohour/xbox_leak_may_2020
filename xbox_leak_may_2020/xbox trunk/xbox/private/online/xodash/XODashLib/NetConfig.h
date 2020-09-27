// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once
#include "SmartObject.h"
#include <winsockp.h>
#include <xonlinep.h>

enum eNetworkState
{
	eCableFail,
	eCablePending,
	eCablePass,
	eIPFail,
	eIPPending,
	eIPPass,
	ePPPoEFail,
	ePPPoEPending,
	eDNSFail,
	eDNSPending,
	eDNSPass,
	eServiceFail,
	eServicePending,
	eDone,
};

// NTS_Status screen, icons' states
enum eNetworkIconStatus{
	eFailIcon,
	eSuccessIcon,
	eDisableIcon
} ;

#ifndef HTONL
#define HTONL(l) \
        ((((l) >> 24) & 0x000000FFL) | \
         (((l) >>  8) & 0x0000FF00L) | \
         (((l) <<  8) & 0x00FF0000L) | \
         (((l) << 24)              ))

#endif

#ifndef NTOHL
#define NTOHL HTONL
#endif

// this class has been copied and changed from the CIpAddr class, names have been changed to 
// prevent possible names collision. The sole purpose of this utility class is to verify that
// set IP address has a valid format

struct CIp                      // IP Address in Network Byte Order
{
    union
    {
        BYTE        _ab[4];
        DWORD       _dw;
    };

    #define IP_BROADCAST            CIp(HTONL(0xFFFFFFFF))
    #define IP_LOOPBACK             CIp(HTONL(0x7F000001))
    #define IP_CLASSA_NETMASK       CIp(HTONL(0xFF000000))
    #define IP_CLASSB_NETMASK       CIp(HTONL(0xFFFF0000))
    #define IP_CLASSC_NETMASK       CIp(HTONL(0xFFFFFF00))
    #define IP_CLASSD_NETMASK       CIp(HTONL(0xF0000000))
    #define IP_LOOPBACK_NETID       CIp(HTONL(0x7F000000))
    #define IP_SECURE_NETID         CIp(HTONL(0x00000000))
    #define IP_SECURE_NETMASK       CIp(HTONL(0xFF000000))


    inline operator DWORD () const { return(_dw); }
    inline DWORD operator = (DWORD & dw) { return(_dw = dw); }
    inline CIp(DWORD dw) { _dw = dw; }
    inline CIp() {}

    inline bool IsBroadcast() const { return(_dw == IP_BROADCAST); }
    inline bool IsMulticast() const { return(IsClassD() && !IsBroadcast()); }
    inline bool IsLoopback() const { return((_dw & IP_CLASSA_NETMASK) == IP_LOOPBACK_NETID); }
    inline bool IsSecure() const { return(_dw && (_dw & IP_SECURE_NETMASK) == IP_SECURE_NETID); }

    inline bool IsClassA() const { return((_dw & HTONL(0x80000000)) == HTONL(0x00000000)); }
    inline bool IsClassB() const { return((_dw & HTONL(0xC0000000)) == HTONL(0x80000000)); }
    inline bool IsClassC() const { return((_dw & HTONL(0xE0000000)) == HTONL(0xC0000000)); }
    inline bool IsClassD() const { return((_dw & HTONL(0xF0000000)) == HTONL(0xE0000000)); }
    inline bool IsValidMask() const { DWORD dw = ~NTOHL(_dw); return((dw & (dw + 1)) == 0); }
    bool IsValidUnicast() const;
    CIp DefaultMask() const;
};

struct CXNetData : public XNetConfigParams
{
	char m_szIp[IP_ADDRESS_SIZE + 1];
	char m_szSubnet[IP_ADDRESS_SIZE + 1];
	char m_szGateway[IP_ADDRESS_SIZE + 1];
	char m_szPrimaryDns[IP_ADDRESS_SIZE + 1];
	char m_szSecondaryDns[IP_ADDRESS_SIZE + 1];
	bool bDynamicIp;
};

class CNetConfig
{
public:
    
	CNetConfig();
	virtual ~CNetConfig();
 
	const TCHAR* GetValue(eButtonId eField);
	HRESULT SetValue(eButtonId, TCHAR*);
	HRESULT Initialize();
	void Cleanup();
	
	bool IsEditable(eButtonId eField);
	eNetworkState GetNetworkStatus() { return m_eCurrentStatus;}
	eNetworkState CheckNetworkStatus();
	eNetworkState CheckServiceStatus();

	void ResetNetworkState();
	void SaveConfig();
	void RestoreConfig();
	

private:
	bool m_bDirty;
	eNetworkState m_eCurrentStatus;
	XONLINETASK_HANDLE   m_hLogon;
	BYTE m_NetConfigFlags;

	CXNetData m_xNetConfig;
	CXNetData m_xNetOrigConfig;
	
	static TCHAR m_szResult[MAX_NTS_SIZE+1];

private:
	void StringFromIP( DWORD dwIPAddr, char* szIp );
	HRESULT IPFromString(TCHAR* szIP, DWORD* dwIPAddr);
	HRESULT ValidateString(TCHAR*);
	void SetIPString(char*, const TCHAR*);

	
	void GetIp();
	
};
