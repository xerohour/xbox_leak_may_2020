//-----------------------------------------------------------------------------
// File: xdpvoice.cpp
//
// Desc: DVoice common routines
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <string.h>

#include "xdppeer.h"
#include "xdpvoice.h"


//-----------------------------------------------------------------------------
// client and server objects
//-----------------------------------------------------------------------------
IDirectPlayVoiceServer *g_pDVServer;				// server object
IDirectPlayVoiceClient *g_pDVClient;				// client object


//-----------------------------------------------------------------------------
// message handlers
//-----------------------------------------------------------------------------
HRESULT (WINAPI *g_AppDVMsgHandler)(void *, DWORD, void *); // app message handler
HRESULT __stdcall XDVMsgHandler(LPVOID *, DWORD, LPVOID *);	// our message handler


//-----------------------------------------------------------------------------
// voice peripheral stuff
//-----------------------------------------------------------------------------
DWORD XDVFindHawk();								// searches for hawk peripherals
DWORD g_dwHawkPort;									// port hawk is in


//-----------------------------------------------------------------------------
// voice targets
//-----------------------------------------------------------------------------
DVID g_dvidTargets[MAX_TARGETS];					// transmit target array
DWORD g_dwNumTargets;								// # of targets in array


//-----------------------------------------------------------------------------
// Name: XDVInit
// Desc: Initialize Voice subsystem and other relevant stuff.
//-----------------------------------------------------------------------------
HRESULT XDVInit(LONG (WINAPI *DVMsgHandler)(void *, DWORD, void *))
{
	HRESULT hr;

	// Save message handler pointer
	g_AppDVMsgHandler = DVMsgHandler;

	// create server object
	hr = XDirectPlayVoiceCreate(IID_IDirectPlayVoiceServer, (LPVOID *)&g_pDVServer);
	assert(SUCCEEDED(hr));

	// create client object
	hr = XDirectPlayVoiceCreate(IID_IDirectPlayVoiceClient, (LPVOID *)&g_pDVClient);
	assert(SUCCEEDED(hr));

	// find hawk port
	g_dwHawkPort = XDVFindHawk();

	// init target array
	g_dwNumTargets = 0;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XDVDoWork
// Desc: Does DVoice work.
//-----------------------------------------------------------------------------
HRESULT XDVDoWork()
{
	if(g_pDVClient)
		return g_pDVClient->DoWork();

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XDVMsgHandler
// Desc: Voice message callback
//-----------------------------------------------------------------------------
HRESULT __stdcall XDVMsgHandler(LPVOID pvUserContext, DWORD dwMsgType, LPVOID pMsg)
{
	switch(dwMsgType)
	{
		// once we are connected, set targets to all players
		// we can override this in our app handler if we want to
        case DVMSGID_CONNECTRESULT:
			XDVAddTransmitTarget(DVID_ALLPLAYERS);
            break;
	}

	// call app DV message handler
	return g_AppDVMsgHandler(pvUserContext, dwMsgType, pMsg);
}

//-----------------------------------------------------------------------------
// Name: XDVCreate
// Desc: Create a DVoice session on the host player
// Params: dwSessionType - DVSESSIONTYPE_PEER, _FORWARDING
//         guidCT - GUID specifying compression for this session
//-----------------------------------------------------------------------------
HRESULT XDVCreate(DWORD dwSessionType, GUID guidCT)
{
	HRESULT hr;
	DVSESSIONDESC dvSession;

	// initialize DVoice server
	hr = g_pDVServer->Initialize(g_pDP, NULL, NULL, NULL, 0);
	assert(SUCCEEDED(hr));

	// initialize the session description
	ZeroMemory(&dvSession, sizeof(DVSESSIONDESC));
	dvSession.dwSize = sizeof(DVSESSIONDESC);
	dvSession.dwFlags = 0;
	dvSession.dwSessionType = dwSessionType;
	dvSession.guidCT = guidCT;

	// start the session
	hr = g_pDVServer->StartSession(&dvSession, 0);
	assert(SUCCEEDED(hr));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XDVConnect
// Desc: Connect to an existing DVoice session
//-----------------------------------------------------------------------------
HRESULT XDVConnect(DWORD dwFlags)
{
	HRESULT hr;
	DVCLIENTCONFIG dvConfig;

	// initialize dvoice
	hr = g_pDVClient->Initialize(g_pDP, XDVMsgHandler, NULL, NULL, 0);
	assert(SUCCEEDED(hr));

	// initialize client configuration
	ZeroMemory(&dvConfig, sizeof(DVCLIENTCONFIG));
	dvConfig.dwSize = sizeof(DVCLIENTCONFIG);

	// the flags dword specifies the type of voice activation
	// it can have DVCLIENTCONFIG_MANUALVOICEACTIVATED set or
	// DVCLIENTCONFIG_AUTOVOICEACTIVATED set, or neither of them set.
	dvConfig.dwFlags = dwFlags;

	// if MANUALVOICEACTIVATED is set, we set the threshold manually.
	// if AUTOVOICEACTIVATED is set, we set the threshold to UNUSED.
	// if neither is set, we set to UNUSED, and voice will 
	// be active whenever there is a valid transmit target.
	if(dwFlags&DVCLIENTCONFIG_MANUALVOICEACTIVATED)
		dvConfig.dwThreshold = DVTHRESHOLD_DEFAULT;
	else
		dvConfig.dwThreshold = DVTHRESHOLD_UNUSED;

	// if hawk was not enumerated, pass a valid value to dvoice anyway
    if(g_dwHawkPort==-1) 
        g_dwHawkPort = 0;
	dvConfig.dwPort = g_dwHawkPort;

	// do the connect
	hr = g_pDVClient->Connect(&dvConfig, 0);
	assert(SUCCEEDED(hr));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XDVDisconnect
// Desc: Disconnect from a DVoice session
//-----------------------------------------------------------------------------
HRESULT XDVDisconnect()
{
	HRESULT hr;
	
	hr = g_pDVClient->Disconnect(0);
	assert(SUCCEEDED(hr));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XDVStopSession
// Desc: Terminates the DVoice session. Does not allow host migration.
//-----------------------------------------------------------------------------
HRESULT XDVStopSession()
{
	HRESULT hr;
	
	hr = g_pDVServer->StopSession(DVFLAGS_NOHOSTMIGRATE);
	assert(SUCCEEDED(hr));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XDVFindHawk
// Desc: Finds a usable voice peripheral.
//-----------------------------------------------------------------------------
DWORD XDVFindHawk()
{
	DWORD dwInsertions, i;
	DWORD dwPort, dwSlot;
	DWORD ticks;

	ticks = GetTickCount();

	OutputDebugStringA("xdpvoice: Searching for Hawk voice peripheral\n");
	
	dwInsertions = 0;
	dwPort = -1;
	while(!dwInsertions)
	{
		dwInsertions = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
		if(dwInsertions)
			for(i=0; i<32; i++)
				if(dwInsertions&(1<<i))
				{
					if(i<16)
					{
						dwPort = i;
						dwSlot = XDEVICE_TOP_SLOT;
					}
					else
					{
						dwPort = i-16;
						dwSlot = XDEVICE_BOTTOM_SLOT;
					}

					if(dwPort>=4)
						OutputDebugStringA("\nxdpvoice: Hawk in illegal port\n\n");
					
					break;
				}

		if((GetTickCount()-ticks)>2000)
		{
			OutputDebugStringA("Hawk search timed out.\n");
			break;
		}
	}

	if(dwPort!=-1)
		OutputDebugStringA("Hawk found...\n");

	return dwPort;
}

//-----------------------------------------------------------------------------
// Name: XDVAddTransmitTarget
// Desc: Adds a transmit target to the current list.
//       If DVID_ALLPLAYERS is added, it sets the array to hold only
//       this single element. If anything besides ALLPLAYERS is added,
//       ALLPLAYERS is removed from the list.
//-----------------------------------------------------------------------------
HRESULT XDVAddTransmitTarget(DVID dvidTarget)
{
	DWORD i;

	if(dvidTarget==DVID_ALLPLAYERS)
	{
		g_dvidTargets[0] = DVID_ALLPLAYERS;
		g_dwNumTargets = 1;
	}
	else if(g_dvidTargets[0]==DVID_ALLPLAYERS || g_dwNumTargets==0)
	{
		g_dvidTargets[0] = dvidTarget;
		g_dwNumTargets = 1;
	}
	else
	{
		for(i=0; i<g_dwNumTargets; i++)
			if(g_dvidTargets[i]==dvidTarget)
				return S_FALSE;

		g_dvidTargets[g_dwNumTargets++] = dvidTarget;
	}

	return XDVSetTransmitTargets();
}

//-----------------------------------------------------------------------------
// Name: XDVRemoveTransmitTarget
// Desc: Removes a target from the transmit list.
//-----------------------------------------------------------------------------
HRESULT XDVRemoveTransmitTarget(DVID dvidTarget)
{
	DWORD i;

	for(i=0; i<g_dwNumTargets; i++)
		if(g_dvidTargets[i]==dvidTarget)
		{
			g_dvidTargets[i] = g_dvidTargets[--g_dwNumTargets];
			return XDVSetTransmitTargets();
		}

	return S_FALSE;
}

//-----------------------------------------------------------------------------
// Name: XDVSetTransmitTargets
// Desc: Sends the current target list to DVoice.
//-----------------------------------------------------------------------------
HRESULT XDVSetTransmitTargets()
{
	HRESULT hr;

	if(g_dwNumTargets!=0)
		hr = g_pDVClient->SetTransmitTargets(g_dvidTargets, g_dwNumTargets, 0);
	else
		hr = g_pDVClient->SetTransmitTargets(NULL, 0, 0);

    assert(SUCCEEDED(hr));

	return hr;
}

//-----------------------------------------------------------------------------
// Name: XDVClearTransmitTargets
// Desc: Clears the transmit target list.
//-----------------------------------------------------------------------------
HRESULT XDVClearTransmitTargets()
{
	HRESULT hr;

	g_dwNumTargets = 0;

    hr = g_pDVClient->SetTransmitTargets(NULL, 0, 0);
    assert(SUCCEEDED(hr));

	return hr;
}
