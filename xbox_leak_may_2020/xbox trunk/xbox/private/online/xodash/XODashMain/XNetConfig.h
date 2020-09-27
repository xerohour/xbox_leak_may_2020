#pragma once


// Those two API are used to pass user input to the network stack.
// First the XNetConfigParams are load, then changed by UI and then saved back to hard disk.
// void    WSAAPI XNetLoadConfigParams(XNetConfigParams * pxncp);
// void    WSAAPI XNetSaveConfigParams(const XNetConfigParams * pxncp);

////////////////////////////////////////////////////////////
// Description: 
// This call triggers network discovery process
// IN Params: None
// OUT Params: None
// Assumptions: XNetSaveConfigParams() is called prior to this function 	
//
////////////////////////////////////////////////////////////
void XNetConnect();


////////////////////////////////////////////////////////////
// Description: 
// XNetCancelConnect terminates network discovery process. 
// IN Params: None
// OUT Params: None
// Assumptions: XNetConnect() could be called prior to this call. 
//		Although it's valid to call this API without XNetConnect()
//		Syncronous call. Caller is not blocked and could call XNetConnect() 
//		immediately after XNetCancelConnect()
////////////////////////////////////////////////////////////
void XNetCancelConnect();

////////////////////////////////////////////////////////////
// Description: 
//  This API is a communication channel between UI and network stack.
//  UI runs in a tight loop, retrieves the status and provide data 
// IN Params: None
// OUT Params: HRESULT with the current status
//		XNET_BUSY - wait for the result, ask later
//		XNET_GET_PPPOE - user should enter PPPoE information, service is optional field
//		XNET_GET_PPPOE_SERVICE - user should enter PPPoE service name
//		XNET_GET_DNS - user should enter DNS server
//		XNET_GET_HOSTNAME - user should enter Hostname to be passed to DHCP server
//		XNET_IDLE - valid while waiting for user input (e.g. after XNET_GET_YYYYYY - see above
//		S_XNET_SUCCESS - we are connected.
//
//		E_XNET_NO_CABLE - cable is not plugged in
//		E_XNET_INVALID_PPPoE - PPPoE concentrator is found, but probably PPPoE data is incorrect
//		E_XNET_INVALID_HOSTNAME - DHCP server was found but rejected to provide IP
//		E_XNET_IP_EXPIRED - Leased time of IP expired and DHCP refuses to renew it. Do we need it?
//		E_XNET_INVALID_DNS - IP obtained, but can't resolve IP addresses for online services
//		E_NO_IDEA - do not know, nothing works, show help pages and ask user to verify configuration
//		Assumptions: 
/////////////////////////////////////////////////////////////
HRESULT XNetConnectionStatus();

/* 
The code will look something like this:

XNetConfigParams xncp;
XNetLoadConfigParams(&xncp);
// get user input ....
XNetSaveConfigParams(&xncp);
XNetConnect();
HRESULT hr = S_OK;
while(1)
{
	
	hr = XNetConnectionStatus();
	// process the status
	if(hr == XNET_GET_YYYY)
	{	XNetLoadConfigParams(&xncp);
		// get user input ....
		XNetSaveConfigParams(&xncp);
		XNetConnect();
	}
	// call XNetCancelConnect() if user requested or time expired
		
}
General questions:
1. Does achDhcpHostName include both Domain name and HostName. If yes what is the expected format?
*/

struct XNetDashConfig: public XNetConfigParams
{
	bool bStaticIP;    // true if current IP is static, false if received from DHCP server
	bool bStaticDNS;   // true if DNS is configured manually, false if received from DHCP server    
};

////////////////////////////////////////////////////////////
// Description: 
// this is a private online dash API for retrieving current network settings
// Dash uses this API to display to user current network settings. 
// The following scenario is a valid example: User gets all network configuration from DHCP server, no user input is required,
// he is connected to the online services and just curious to see his network settings. 
// IN Params: None
// OUT Params: XNetDashConfig - with all the requested data
//		pxncp->achLan[20] - ignored
//		pxncp->ina.s_addr - current IP address, received from DHCP server or Static IP 
//		pxncp->ina.inaMask.s_addr - current subnet mask
//		pxncp->ina.inaDnsPrimary.s_addr - primary DNS 
//		pxncp->ina.inaDnsSecondary.s_addr - secondary DNS
//		pxncp->achDhcpHostName            // Host name for DHCP ("" means no host name)
//		pxncp->achPppUserName             // User name for PPPoE ("" means no PPPoE)
//		pxncp->achPppPassword             // User password for PPPoE
//		pxncp->achPppServer               // Server name for PPPoE ("" ok)
//		pxncp->abReserved - ignored               
//		pxncp->dwSigEnd   - ignored
//		pxncp->bStaticIP  - valid values: true/false
//		pxncp->bStaticDNS  - valid values: true/false
//
// Assumptions: Call should be synchronous  and the result is expected immediately 
//
void    WSAAPI XNetGetConfigParams(XNetDashConfig * pxncp);



