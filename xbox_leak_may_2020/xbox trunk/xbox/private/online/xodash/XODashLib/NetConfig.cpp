//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#include "std.h"

#include "NetConfig.h"
#include "globals.h"
#include "Utilities.h"
#include "XOConst.h"


//-------------------------------------------------------------
// Function name	: CIp::DefaultMask
// Description	    : get Default mask for IP address
// Return type		: CIp 
//-------------------------------------------------------------
CIp CIp::DefaultMask() const
{
    if (IsClassA()) return(IP_CLASSA_NETMASK);
    if (IsClassB()) return(IP_CLASSB_NETMASK);
    if (IsClassC()) return(IP_CLASSC_NETMASK);
    if (IsClassD()) return(IP_CLASSD_NETMASK);
    return(0);
}


//-------------------------------------------------------------
// Function name	: CIp::IsValidUnicast
// Description	    : validate the IP address
// Return type		: bool 
//-------------------------------------------------------------
bool CIp::IsValidUnicast() const
{
    return(_dw != 0 && !IsSecure() && !IsBroadcast() && !IsMulticast() && !IsLoopback());
}

// 
TCHAR CNetConfig::m_szResult[];
CNetConfig::CNetConfig(): m_bDirty(true), 
						  m_eCurrentStatus (eCablePending), 
						  m_NetConfigFlags(XNET_STARTUP_BYPASS_SECURITY)
{
}



//----------------------------------------------------------------
// Function name	: CNetConfig::~CNetConfig
// Description	    : 
// Return type		: 
//----------------------------------------------------------------
CNetConfig::~CNetConfig()
{
	Cleanup();
}


//----------------------------------------------------------------
// Function name	: CNetConfig::Cleanup
// Description	    : cleanup all the resources
// Return type		: void 
//----------------------------------------------------------------
void CNetConfig::Cleanup()
{
	if(m_hLogon)
	{
		XOnlineTaskClose(m_hLogon);
	}
	m_hLogon = NULL;
	XOnlineCleanup();
	DbgPrint("XonlineCleanup() called\n");
	XNetCleanup();
}

static const char szStaticIp[] = "STATIC_IP";
static const char szDynamicIp[] = "DYNAMIC_IP";


//-------------------------------------------------------------
// Function name	: CNetConfig::GetValue
// Description	    : 
// Return type		: const TCHAR* 
// Argument         : eButtonId eField - the field a value is required for
//-------------------------------------------------------------
const TCHAR* CNetConfig::GetValue(eButtonId eField)
{
	HRESULT hr = S_OK;
	ZeroMemory(m_szResult, countof(m_szResult));

	do
	{
		if(m_bDirty)
		{
			hr = Initialize();
			BREAKONFAIL(hr, "Fail to retreive network stack state\n");
		}
		switch(eField)
		{
		// NTS_Hostname screen
		case eNTS_HostnameHostname:
			Unicode (m_szResult, m_xNetConfig.achDhcpHostName, countof(m_szResult)-1);
			break;
		case eNTS_HostnameDomainName:
			Unicode (m_szResult, "ASKD", countof(m_szResult)-1);
			break;

		// NTS_PPPoE screen
		case eNTS_PPPoEUserName:
			Unicode (m_szResult, m_xNetConfig.achPppUserName, countof(m_szResult)-1);
			break;
		case eNTS_PPPoEPassword:
			Unicode(m_szResult, m_xNetConfig.achPppPassword, countof(m_szResult)-1);
			break;
		case eNTS_PPPoEServiceName:
			Unicode(m_szResult, m_xNetConfig.achPppServer, countof(m_szResult)-1);
			break;

		// NTS_IP screen
		case eNTS_IPToggle:
		case eNTS_DNSToggle:
			Unicode(m_szResult, m_xNetConfig.bDynamicIp ? szDynamicIp : szStaticIp, countof(m_szResult)-1 );
			break;
		case eNTS_IPIP:
			Unicode(m_szResult, m_xNetConfig.m_szIp, countof(m_szResult)-1);
			break;
		case eNTS_IPSubnet:
			Unicode(m_szResult, m_xNetConfig.m_szSubnet, countof(m_szResult)-1);
			break;
		case eNTS_IPGate:
			Unicode(m_szResult, m_xNetConfig.m_szGateway, countof(m_szResult)-1);
			break;

		//	NTS_DNS	screen
		case eNTS_DNSPrimary:
			Unicode(m_szResult, m_xNetConfig.m_szPrimaryDns, countof(m_szResult)-1);
			break;
		case eNTS_DNSSecondary:
			Unicode(m_szResult, m_xNetConfig.m_szSecondaryDns, countof(m_szResult)-1);
			break;
			// no values to be set for those fields
		case eNTS_HostnameContinue:
		case eNTS_PPPoEContinue:
		case eNTS_DNSContinue:
			break;
		default:
			// requested value is not found
			ASSERT(false);
		}
	}while (0);
	
	//return found value
	return m_szResult;
}


//-------------------------------------------------------------
// Function name	: CNetConfig::SetValue
// Description	    : Set value for a field specified by eField parameter
// Return type		: HRESULT  - indication whether set succeeded or the value is invalid for this field
// Argument         : eButtonId eField
// Argument         : TCHAR* pNewValue
//-------------------------------------------------------------
HRESULT CNetConfig::SetValue(eButtonId eField, TCHAR* pNewValue)
{
	DWORD dwIp = 0;
	HRESULT hr = S_OK;
	do
	{
		if(m_bDirty)
		{
			hr = Initialize();
			BREAKONFAIL(hr, "Fail to retreive network stack state\n");
		}

		switch(eField)
		{
		case eNTS_HostnameHostname:
			hr = ValidateString(pNewValue);
			if(FAILED(hr))
			{
				break;
			}
			ZeroMemory(m_xNetConfig.achDhcpHostName, countof(m_xNetConfig.achDhcpHostName));
			Ansi(m_xNetConfig.achDhcpHostName, pNewValue,  min(_tcslen(pNewValue), countof(m_xNetConfig.achDhcpHostName)-1) );		
			break;
		case eNTS_HostnameDomainName:
			hr = ValidateString(pNewValue);
			if(FAILED(hr))
			{
				break;
			}
			ZeroMemory(m_xNetConfig.achDhcpHostName, countof(m_xNetConfig.achDhcpHostName));
			// TBD ???Ansi(m_xNetConfig.achDhcpHostName, pNewValue,  min(_tcslen(pNewValue)-1, countof(m_xNetConfig.achDhcpHostName)-1) );		
			break;
		// NTS_PPPoE 
		case eNTS_PPPoEUserName:
			hr = ValidateString(pNewValue);
			if(FAILED(hr))
			{
				break;
			}
			ZeroMemory(m_xNetConfig.achPppUserName, countof(m_xNetConfig.achPppUserName));
			Ansi(m_xNetConfig.achPppUserName, pNewValue,  min(_tcslen(pNewValue), countof(m_xNetConfig.achPppUserName)-1) );		
			break;
		case eNTS_PPPoEPassword:
			hr = ValidateString(pNewValue);
			if(FAILED(hr))
			{
				break;
			}

			ZeroMemory(m_xNetConfig.achPppPassword, countof(m_xNetConfig.achPppPassword));
			Ansi(m_xNetConfig.achPppPassword, pNewValue, min(_tcslen(pNewValue), countof(m_xNetConfig.achPppPassword)-1) );		
			break;
		case eNTS_PPPoEServiceName:
			hr = ValidateString(pNewValue);
			if(FAILED(hr))
			{
				break;
			}
			ZeroMemory(m_xNetConfig.achPppServer, countof(m_xNetConfig.achPppServer));
			Ansi(m_xNetConfig.achPppServer, pNewValue, min(_tcslen(pNewValue), countof(m_xNetConfig.achPppServer)-1) );		
			break;

		// NTS_IP
		case eNTS_IPToggle:
			{
			m_xNetConfig.bDynamicIp = !m_xNetConfig.bDynamicIp;
			XNetConfigParams* xncp = (XNetConfigParams*) &m_xNetConfig;
			{
				xncp->ina.s_addr = 0L;
				xncp->inaMask.s_addr = 0L;
				xncp->inaGateway.s_addr = 0L;
			}

			StringFromIP(m_xNetConfig.ina.s_addr, m_xNetConfig.m_szIp);
			StringFromIP(m_xNetConfig.inaMask.s_addr, m_xNetConfig.m_szSubnet);
			StringFromIP(m_xNetConfig.inaGateway.s_addr, m_xNetConfig.m_szGateway);
			}
			break;

		case eNTS_DNSToggle:
			{
			XNetConfigParams* xncp = (XNetConfigParams*) &m_xNetConfig;
			m_xNetConfig.bDynamicIp = !m_xNetConfig.bDynamicIp;
			xncp->inaDnsPrimary.s_addr = 0L;
			xncp->inaDnsSecondary.s_addr = 0L;
			StringFromIP(m_xNetConfig.inaDnsPrimary.s_addr, m_xNetConfig.m_szPrimaryDns);
			StringFromIP(m_xNetConfig.inaDnsSecondary.s_addr, m_xNetConfig.m_szSecondaryDns);
			}
			break;
		case eNTS_IPIP:
			hr = IPFromString(pNewValue, &dwIp);
			if(FAILED(hr))
			{
				break;
			}

			m_xNetConfig.ina.s_addr = dwIp;
			SetIPString(m_xNetConfig.m_szIp, pNewValue);
			break;
		case eNTS_IPSubnet:
			hr = IPFromString(pNewValue, &dwIp);
			if(FAILED(hr))
			{
				break;
			}
			m_xNetConfig.inaMask.s_addr = dwIp;			
			SetIPString(m_xNetConfig.m_szSubnet, pNewValue);
			break;
		case eNTS_IPGate:
			hr = IPFromString(pNewValue, &dwIp);
			if(FAILED(hr))
			{
				break;
			}
			m_xNetConfig.inaGateway.s_addr = dwIp;
			SetIPString(m_xNetConfig.m_szGateway, pNewValue);
			break;

		//	"NTS_DNS"
		case eNTS_DNSPrimary:
			hr = IPFromString(pNewValue, &dwIp);
			if(FAILED(hr))
			{
				break;
			}		
			
			m_xNetConfig.inaDnsPrimary.s_addr = dwIp;
			SetIPString(m_xNetConfig.m_szPrimaryDns, pNewValue);
			break;
		case eNTS_DNSSecondary:
			hr = IPFromString(pNewValue, &dwIp);
			if(FAILED(hr))
			{
				break;
			}		
			m_xNetConfig.inaDnsSecondary.s_addr = dwIp;
			SetIPString(m_xNetConfig.m_szSecondaryDns, pNewValue);
			break;
		default:
			;
		}
	 }while (0);
	return hr;
}




//-------------------------------------------------------------
// Function name	: CNetConfig::Initialize
// Description	    : initialize all internals structures
// Return type		: HRESULT 
//-------------------------------------------------------------
HRESULT CNetConfig::Initialize()
{
	HRESULT hr = S_OK;
	XNetStartupParams xnsp;
	ZeroMemory( &xnsp, sizeof(xnsp) );
	xnsp.cfgSizeOfStruct = sizeof(xnsp);
	xnsp.cfgFlags = m_NetConfigFlags;
 
    XNetStartup( &xnsp );
	g_ResetAutomation = true; // net stack is reset, simulator needs to reset the connection

	XNetLoadConfigParams(&m_xNetConfig);
	StringFromIP(m_xNetConfig.ina.s_addr, m_xNetConfig.m_szIp);
	StringFromIP(m_xNetConfig.inaMask.s_addr, m_xNetConfig.m_szSubnet);
	StringFromIP(m_xNetConfig.inaGateway.s_addr, m_xNetConfig.m_szGateway);
	StringFromIP(m_xNetConfig.inaDnsPrimary.s_addr, m_xNetConfig.m_szPrimaryDns);
	StringFromIP(m_xNetConfig.inaDnsSecondary.s_addr, m_xNetConfig.m_szSecondaryDns);
	
	(0L == m_xNetConfig.ina.s_addr)?	m_xNetConfig.bDynamicIp = true : m_xNetConfig.bDynamicIp = false;
	m_bDirty = false;

	m_eCurrentStatus = eCablePending;
	if(m_hLogon)
	{
		XOnlineTaskClose(m_hLogon);
		m_hLogon = NULL;
	}

	hr = XOnlineStartup(NULL);
	DbgPrint("XOnlineStartup called\n");
	if(FAILED(hr))
	{
		DbgPrint("Failed XOnlineStartup with 0x%x\n", hr);
	} 

	m_NetConfigFlags = XNET_STARTUP_BYPASS_SECURITY;
	m_xNetOrigConfig = m_xNetConfig;
	return hr;
}


//-------------------------------------------------------------
// Function name	: CNetConfig::StringFromIP
// Description	    : Converts an IP Address from a DWORD to a string
// Return type		: void 
// Argument         :  DWORD dwIPAddr
// Argument         : char* szIp
//-------------------------------------------------------------
void CNetConfig::StringFromIP( DWORD dwIPAddr, char* szIp )
{
	ASSERT(szIp);
	if(!szIp)
	{
		return;
	}
	if ( dwIPAddr == 0l )
    {
        strcpy(szIp,"0.0.0.0");
        return ;
    }

	BYTE b4 = (BYTE)( dwIPAddr >> 24 );
	BYTE b3 = (BYTE)( dwIPAddr >> 16 );
	BYTE b2 = (BYTE)( dwIPAddr >> 8 );
	BYTE b1 = (BYTE)dwIPAddr;

	ZeroMemory(szIp, IP_ADDRESS_SIZE + 1);
	_snprintf( szIp, IP_ADDRESS_SIZE , "%d.%d.%d.%d", b1, b2, b3, b4 );
}


//-------------------------------------------------------------
// Function name	: CNetConfig::IPFromString
// Description	    : Converts an Ip address from string to DWORD
// Return type		: HRESULT 
// Argument         : TCHAR* szIP
// Argument         : DWORD* dwIPAddr
//-------------------------------------------------------------
HRESULT CNetConfig::IPFromString(TCHAR* szIP, DWORD* dwIPAddr)
{
	HRESULT hr = S_OK;
	ASSERT(szIP);
	*dwIPAddr = 0L;	

	if(!szIP || (szIP && _tcslen(szIP) > IP_ADDRESS_SIZE))
	{
		return XBAPPERR_INVALIDIP;
	}

   // make a private copy because tsctok modifies the buffer
   static TCHAR szBuf[IP_ADDRESS_SIZE + 1];
   ZeroMemory(szBuf, (IP_ADDRESS_SIZE + 1)*sizeof(TCHAR));
   _tcscpy(szBuf, szIP);
   
   int nDotCount = 0;
   for (TCHAR* pCur = szIP; *pCur!='\0';pCur++)
   {
	   if(*pCur == _T('.'))
	   {
		   nDotCount++;
	   }
   }
   
   if(nDotCount != 3)
   {
		return XBAPPERR_INVALIDIP;	
   }

   TCHAR* token = _tcstok( szBuf, _T(".") );
   int nOctetNum = 0;
   while( token != NULL )
   {
	  if(_tcslen(token) > 3 || !token[0] )
	  {
		return XBAPPERR_INVALIDIP;
	  }
	  unsigned int uNextOctet = _ttoi(token);
	  if (uNextOctet > 255)
	  {
		  return XBAPPERR_INVALIDIP;
	  }
	  *dwIPAddr = uNextOctet<<nOctetNum*8 | *dwIPAddr;

	  /* Get next token: */
      token = _tcstok( NULL, _T("."));
	  nOctetNum++;
   }
   if(nOctetNum != 4)
   {
		*dwIPAddr = 0L;	
		return XBAPPERR_INVALIDIP;
   }

   CIp cTestAddr = *dwIPAddr;
   if(cTestAddr.IsValidUnicast() )
   {
	    return hr;
   }

   return XBAPPERR_INVALIDIP;
}


//-------------------------------------------------------------
// Function name	: CNetConfig::IsEditable
// Description	    : return if the field is allowed to edit. E.g. IP address is not allowed to be changed 
//					  if Dynamic IP is chosen
// Return type		: bool 
// Argument         : eButtonId eField
//-------------------------------------------------------------
bool CNetConfig::IsEditable(eButtonId eField)
{
	switch(eField)
	{
	case eNTS_HostnameHostname:
	case eNTS_HostnameDomainName:
	case eNTS_PPPoEUserName:
	case eNTS_PPPoEPassword:
	case eNTS_PPPoEServiceName:
	case eNTS_IPToggle:
	case eNTS_DNSToggle:
		return true;

	case eNTS_IPIP:
	case eNTS_IPSubnet:
	case eNTS_IPGate:
	case eNTS_DNSPrimary:
	case eNTS_DNSSecondary:
		return !m_xNetConfig.bDynamicIp;
	default:
		ASSERT(false);
		return false;
	}	
}


//-------------------------------------------------------------
// Function name	: CNetConfig::ValidateString
// Description	    : All strings should include ASCII characters in the range of '' and '~'
//					  invalidate everything else
// Return type		: HRESULT 
// Argument         : TCHAR* szValue
//-------------------------------------------------------------
HRESULT CNetConfig::ValidateString(TCHAR* szValue)
{
	HRESULT hr = S_OK;
	if(!szValue)
	{
		return S_OK;
	}
	
	for(size_t i =0; i < _tcslen(szValue); i++)
	{
		if( *(szValue +i) >= _T(' ') && *(szValue +i) < _T('~') ) 
		{
			continue;
		}
		hr = XBAPPERR_INVALIDSTR;
		break;	
	}
	return hr;
}




//-------------------------------------------------------------
// Function name	: CNetConfig::CheckNetworkState
// Description	    : This is the main function that calls all appropriate APIs and sets the status 
//					  according to the return from APIs result
// Return type		: eNetworkState 
//-------------------------------------------------------------
eNetworkState CNetConfig::CheckNetworkStatus()
{
	HRESULT hr = S_OK;
	if(m_bDirty)
	{
		hr = Initialize();
		if(FAILED(hr))
		{
			DbgPrint("CNetConfig::GetNetworkState - failed to initialize the stack - hr=0x%x\n", hr);
			m_eCurrentStatus = eIPFail;
			return m_eCurrentStatus;
		}

	}

	switch (m_eCurrentStatus)
	{
	case eCablePending:
	case eCableFail:
		{
			DWORD dwStatus = XNetGetEthernetLinkStatus();
			(dwStatus & XNET_ETHERNET_LINK_ACTIVE ) != 0 ? m_eCurrentStatus = eCablePass : m_eCurrentStatus = eCableFail;	
		}
	break;
	case eCablePass:
		GetIp();
		break;
// do nothing 
	case eIPFail:
	case eDNSFail:
	case eServiceFail:
	case eDone:
		break;
	case eIPPass:
		CheckServiceStatus();
		break;
	default:
		// why I'm here ? 
		ASSERT(false);
	}
 	return m_eCurrentStatus;
}



//-------------------------------------------------------------
// Function name	: CNetConfig::CheckServiceStatus
// Description	    : verify connectivity with the DataCenter
// Return type		: void 
//-------------------------------------------------------------
eNetworkState CNetConfig::CheckServiceStatus()
{
	HRESULT hr = S_OK;
	if(m_bDirty)
	{
		hr = Initialize();
		if(FAILED(hr))
		{
			DbgPrint("CNetConfig::CheckServiceStatus - failed to initialize the stack - hr=0x%x\n", hr);
			m_eCurrentStatus = eIPFail;
			return m_eCurrentStatus;
		}
	}
	if(!m_hLogon)
	{
    // need to log onto all of our services here
        DWORD serviceIDs[] = {XONLINE_USER_ACCOUNT_SERVICE,
							  XONLINE_BILLING_OFFERING_SERVICE};
		
		XONLINE_USER Users[XONLINE_MAX_LOGON_USERS];
		ZeroMemory( Users, sizeof( Users ) );

		hr = XOnlineLogon( Users, serviceIDs, countof(serviceIDs), NULL,  &m_hLogon );
	}
	else
	{
		ASSERT(m_hLogon);
		if(!m_hLogon)
		{
			m_eCurrentStatus = eServiceFail;
			return m_eCurrentStatus;
		}
		hr =XOnlineTaskContinue( m_hLogon );
		switch(hr)
		{
		case XONLINE_E_LOGON_DNS_LOOKUP_FAILED :
		case XONLINE_E_LOGON_NO_IP_ADDRESS:
		case XONLINE_E_LOGON_NO_DNS_SERVICE:
		case XONLINE_E_LOGON_DNS_LOOKUP_TIMEDOUT:
			m_eCurrentStatus = eDNSFail;
			break;

		case XONLINETASK_S_RUNNING:
//			DbgPrint("CNetConfig::GetNetworkState() - pumping\n");
			break;


		case XONLINE_S_LOGON_CONNECTION_ESTABLISHED:
			m_eCurrentStatus = eDone;
 			break;
		
		default:
			m_eCurrentStatus = eServiceFail;
			DbgPrint("CNetConfig::CheckConnectivity, XOnlineLogon fail hr = 0x%x\n", hr);
			break;
			
		}
	}
	return m_eCurrentStatus;
}

//-------------------------------------------------------------
// Function name	: CNetConfig::GetIp
// Description	    : acquire IP, save settings to disk if IP has been obtained successfully
// Return type		: void 
//-------------------------------------------------------------
void CNetConfig::GetIp()
{
	XNetConfigParams* xncp = (XNetConfigParams*) &m_xNetConfig;
	INT err = XNetConfig(xncp, XNET_CONFIG_NORMAL);
	err? m_eCurrentStatus = eIPFail : m_eCurrentStatus = eIPPass;	
	if(m_eCurrentStatus != eIPPass)
	{
		DbgPrint("CNetConfig::GetIp - failed to obtain IP, res = %d\n",err);
	}
}


//-------------------------------------------------------------
// Function name	: CNetConfig::SetIPString
// Description	    : Set IP string for the pDest field
// Return type		: void 
// Argument         : char* pDest
// Argument         : const TCHAR* pSource
//-------------------------------------------------------------
void CNetConfig::SetIPString(char* pDest, const TCHAR* pSource)
{
	ASSERT(pDest && pSource && _tcslen(pSource) <= IP_ADDRESS_SIZE + 1 );
	if(!pDest || !pSource || (pSource && _tcslen(pSource) > IP_ADDRESS_SIZE + 1))
	{
		return;
	}

	ZeroMemory(pDest, IP_ADDRESS_SIZE + 1);
	Ansi(pDest, pSource,  _tcslen(pSource) );		
}


//-------------------------------------------------------------
// Function name	: CNetConfig::SaveConfig
// Description	    : save new configuration to the hard drive
// Return type		: void 
//-------------------------------------------------------------
void CNetConfig::SaveConfig()
{
	if(m_bDirty)
	{
		HRESULT hr = Initialize();
		if (FAILED(hr))
		{
			DbgPrint("CNetConfig::SaveConfig - fail to init the net stack\n");
		}

	}
	XNetConfigParams* xncp = (XNetConfigParams*) &m_xNetConfig;
	XNetSaveConfigParams(xncp);	
}


//-------------------------------------------------------------
// Function name	: CNetConfig::RestoreConfig
// Description	    : revert configuration to the presious one. 
//					  When user hit cancel from the Settings screen, 
//                    the previous settings are restored
// Return type		: void 
//-------------------------------------------------------------
void CNetConfig::RestoreConfig()
{
	m_xNetConfig = m_xNetOrigConfig;
}


//-------------------------------------------------------------
// Function name	: CNetConfig::ResetNetworkState
// Description	    : reset network state, network discovery process
//                    will be restarted next time GetNetworkStatus is called  
// Return type		: void 
//-------------------------------------------------------------
void CNetConfig::ResetNetworkState() 
{
	m_eCurrentStatus = eCablePending;
	DbgPrint("CNetConfig::ResetNetworkState() - network state is set to eCablePending\n");
	Cleanup();
	m_bDirty = true;
	// restart the stack and troubleshoot the network problems
	m_NetConfigFlags = XNET_STARTUP_BYPASS_SECURITY | XNET_STARTUP_MANUAL_CONFIG;
}

