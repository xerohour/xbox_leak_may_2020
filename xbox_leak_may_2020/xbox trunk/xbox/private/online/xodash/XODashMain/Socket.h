//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#pragma once
#include <xtl.h>
#include "AutomationMsg.h"
#include "XBStopWatch.h"


const int ciTCPServerPort = 10000;      // port open by the TCP server to listen to
const float cfAutomationTimeout = 60.0f; // (in seconds)- if there is no activity in ciAutomationTimeout, client will be disconnected


///////////////////////////////////////////////////////////////
// Class:		CSocket
// Description: CSocket wraps winsock. CSocket implements standard 
// TCP server object. CSocket opnes one non-blocking socket and waits 
// for client to connnect. CSocket allows only one simultaneous connection
// CSocket will shutdown the accepted socket if no traffic discovered
// during predefined period of time
// Usage:		Used by automation tests to simulate input of pereferral device
///////////////////////////////////////////////////////////////
class CSocket
{
public:
	CSocket();
	~CSocket();
	HRESULT InitServer(bool bTCP = true);
	bool ReceiveMessage(CAutomationMsg* pMsg);
	void Close();
	bool IsAutomationActive ();

private:
	HRESULT ProcessMessage();
	HRESULT BindSocket(bool bNonBlocking = true);
	HRESULT InitNetworkStack();
	
private:
	SOCKET m_ListenSocket;
	SOCKET m_AcceptedSocket;
	bool m_bClientConnected;
	CXBStopWatch m_AutomationTimer;

};