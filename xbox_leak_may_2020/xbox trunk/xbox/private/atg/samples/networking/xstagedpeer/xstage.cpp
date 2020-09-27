//-----------------------------------------------------------------------------
// File: xstage.cpp
//
// Desc: staging routines for xstagedpeer
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "xdppeer.h"
#include "xmenu.h"
#include "xstage.h"
#include "xstagedpeer.h"

// slot info
#define SLOT_CLOSED		0
#define SLOT_OPEN		1
#define SLOT_OCCUPIED	2
#define SLOT_READY		3
#define SLOTCOLOR_CLOSED	0xffff0000
#define SLOTCOLOR_OPEN		0xffffffff
#define SLOTCOLOR_OCCUPIED	0xffffff00
#define SLOTCOLOR_READY		0xff00ff00

#define MAX_SLOTS 6
struct SlotData
{
	DWORD dwStatus;
	WCHAR wstrPlayerName[MAX_PLAYER_NAME];
	DWORD dpnidPlayer;
};

// mp stage messages
#define STAGEMSG_BASE			100
#define STAGEMSG_REJECT			STAGEMSG_BASE+0
#define STAGEMSG_SLOTNUMBER		STAGEMSG_BASE+1
#define STAGEMSG_SLOTREADY		STAGEMSG_BASE+2
#define STAGEMSG_SLOTOCCUPIED	STAGEMSG_BASE+3
#define STAGEMSG_SLOTUPDATE		STAGEMSG_BASE+4
#define STAGEMSG_SLOTOPEN		STAGEMSG_BASE+5
#define STAGEMSG_SLOTCLOSED		STAGEMSG_BASE+6
#define STAGEMSG_STARTGAME		STAGEMSG_BASE+7

#pragma pack(push, 1)
// mp message structures
struct STAGEMSG_GENERIC
{
	DWORD dwType;
};
struct STAGEMSG_SLOTINFO
{
	DWORD dwType;
	DWORD dwSlot;
};
struct STAGEMSG_SLOTDATA
{
    DWORD dwType;
	SlotData Slots[MAX_SLOTS];
};
#pragma pack(pop)

XMenu *MPStageMenu = NULL;			// multiplayer stage menu
BOOL g_bAllowJoin;
XMenuItem *g_MPSlots[8];
DWORD g_dwLocalSlot;
extern DWORD g_dwGameState;			// defined in app

// local functions
int XStage_FindSlot();
DWORD XStage_SlotFunction(DWORD cmd, XMenuItem *mi);
DWORD XStage_StartGame(DWORD cmd, XMenuItem *mi);
HRESULT XStage_SendSlotData();
void XStage_SlotUpdate(STAGEMSG_SLOTDATA *msg);
void XStage_SendReject(DWORD dpnidPlayer);
void XStage_SendSlotStatus(DWORD sendto, DWORD slot, DWORD msgtype);

//-----------------------------------------------------------------------------
// Name: XStage_FindSlot
// Desc: Finds an open slot in the MP stage
//-----------------------------------------------------------------------------
int XStage_FindSlot()
{
	DWORD i;

	for(i=1; i<8; i++)
		if(g_MPSlots[i]->val1==SLOT_OPEN)
			return i;

	return -1;
}

//-----------------------------------------------------------------------------
// Name: XStage_SlotFunction
// Desc: Called when a slot is chosen by a player
//-----------------------------------------------------------------------------
DWORD XStage_SlotFunction(DWORD cmd, XMenuItem *mi)
{
	DWORD slot, i;

	// figure out which MP slot was chosen
	slot = -1;
	for(i=0; i<MAX_SLOTS; i++)
		if(g_MPSlots[i]==mi)
		{
			slot = i;
			break;
		}

	if(g_dwConnectStatus==GAME_HOSTING)
	{
		// if we clicked on ourself, just return
		if(slot==0)
			return MROUTINE_RETURN;

		// otherwise toggle slot status
		switch(mi->val1)
		{
			// if slot was closed, open it
			case SLOT_CLOSED:
				XMenu_SetItemText(mi, L"Open");
				mi->color = SLOTCOLOR_OPEN;
				mi->val1 = SLOT_OPEN;
				XStage_SendSlotStatus(DPNID_ALL_PLAYERS_GROUP, slot, STAGEMSG_SLOTOPEN);
				break;

			// if slot was open, close it
			case SLOT_OPEN:
				XMenu_SetItemText(mi, L"Closed");
				mi->color = SLOTCOLOR_CLOSED;
				mi->val1 = SLOT_CLOSED;
				XStage_SendSlotStatus(DPNID_ALL_PLAYERS_GROUP, slot, STAGEMSG_SLOTCLOSED);
				break;

			// if slot was occupied or ready, just kick the player
			case SLOT_READY:
			case SLOT_OCCUPIED:
				XMenu_SetItemText(mi, L"Open");
				mi->color = SLOTCOLOR_OPEN;
				mi->val1 = SLOT_OPEN;
				XStage_SendReject(mi->val2);
				XStage_SendSlotStatus(DPNID_ALL_PLAYERS_GROUP, slot, STAGEMSG_SLOTOPEN);
				break;
		}
	}
	else	
	{		
		// otherwise, we connected to an existing game
		// and all we can do is toggle our readiness

		// make sure we are on our own slot
		if(g_dwLocalSlot==slot)
		{
			// when we toggle our status, all we do is send a message
			// to the server so we wont show ready until the server
			// acks it
			if(mi->val1==SLOT_READY)
				XStage_SendSlotStatus(g_dpnidHost, g_dwLocalSlot, STAGEMSG_SLOTOCCUPIED);
			else
				XStage_SendSlotStatus(g_dpnidHost, g_dwLocalSlot, STAGEMSG_SLOTREADY);
		}
	}

	return MROUTINE_RETURN;
}

//-----------------------------------------------------------------------------
// Name: XStage_AbortMPStage
// Desc: Called when a the game is aborted for any reason.
//       Calls terminate if player was the host otherwise
//       calls disconnect.
//-----------------------------------------------------------------------------
DWORD XStage_AbortMPStage(DWORD cmd, XMenuItem *mi)
{
	// if we are hosting, terminate the game
	// otherwise, just disconnect
	if(g_dwConnectStatus==GAME_HOSTING)
		XDPTerminate();
	else
		XDPDisconnect();

	g_dwGameState = GAMESTATE_ATTRACT;

	return MROUTINE_DIE;
}

//-----------------------------------------------------------------------------
// Name: XStage_DoMPStage
// Desc: Builds the multiplayer staging menu.
//       If the player is the host, start game item is included.
//-----------------------------------------------------------------------------
void XStage_DoMPStage(WCHAR *hostname)
{
	DWORD i;
	XMenuItem *mi;

	// create stage menu
	if(MPStageMenu)
		XMenu_Delete(MPStageMenu);

	// if hostname is defined, we are the host so do the host menu
	if(hostname)
	{
		MPStageMenu = XMenu_Init(200.0f, 100.0f, MAX_SLOTS+3, 0, XStage_AbortMPStage);
		XMenu_AddItem(MPStageMenu, MITEM_ROUTINE, L"Start Game", XStage_StartGame);
		XMenu_AddItem(MPStageMenu, MITEM_ROUTINE, L"Abort Game", XStage_AbortMPStage);
		XMenu_AddItem(MPStageMenu, MITEM_SEPARATOR, NULL, NULL);

		// add host player
		g_MPSlots[0] = XMenu_AddItem(MPStageMenu, MITEM_ROUTINE, hostname, XStage_SlotFunction);
		g_MPSlots[0]->val1 = SLOT_READY;
		g_MPSlots[0]->color = SLOTCOLOR_READY;

		// flag join allowed
		g_bAllowJoin = TRUE;

		// set our local slot number
		g_dwLocalSlot = 0;
	}
	else
	{
		MPStageMenu = XMenu_Init(200.0f, 100.0f, MAX_SLOTS+2, 0, XStage_AbortMPStage);
		XMenu_AddItem(MPStageMenu, MITEM_ROUTINE, L"Abort Game", XStage_AbortMPStage);
		XMenu_AddItem(MPStageMenu, MITEM_SEPARATOR, NULL, NULL);
		g_MPSlots[0] = XMenu_AddItem(MPStageMenu, MITEM_ROUTINE, L"HOST", XStage_SlotFunction);
		g_MPSlots[0]->color = SLOTCOLOR_OCCUPIED;

		// flag join not allowed
		g_bAllowJoin = FALSE;

		// set our local slot number
		g_dwLocalSlot = -1;
	}

	// add player slots
	for(i=1; i<MAX_SLOTS; i++)
	{
		mi = XMenu_AddItem(MPStageMenu, MITEM_ROUTINE, L"Open", XStage_SlotFunction);
		mi->color = SLOTCOLOR_OPEN;
		mi->val1 = SLOT_OPEN;
		mi->val2 = 0;

		g_MPSlots[i] = mi;
	}

	MPStageMenu->w = 240.0f;	
	MPStageMenu->flags |= MENU_LEFT;
	XMenu_Activate(MPStageMenu);

	g_dwGameState = GAMESTATE_STAGING;
}

//-----------------------------------------------------------------------------
// Name: XStage_SendSlotData
// Desc: Sends updated slot data to all players.
//       Called by host whenever a player enters or leaves the game.
//-----------------------------------------------------------------------------
HRESULT XStage_SendSlotData()
{
	DWORD i;
	XMenuItem *mi;
    STAGEMSG_SLOTDATA msg;
    DPN_BUFFER_DESC buf;
    DPNHANDLE hAsync;

    if(g_dwConnectStatus!=GAME_HOSTING)
        return E_FAIL;

	// fill in slot update message
    msg.dwType = STAGEMSG_SLOTUPDATE;

	for(i=0; i<MAX_SLOTS; i++)
	{
		mi = g_MPSlots[i];
	
		wcsncpy(msg.Slots[i].wstrPlayerName, mi->string, MAX_PLAYER_NAME);
		msg.Slots[i].dwStatus = mi->val1;
		msg.Slots[i].dpnidPlayer = mi->val2;
	}

    buf.dwBufferSize = sizeof(STAGEMSG_SLOTDATA);
    buf.pBufferData = (BYTE *)&msg;

    g_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &buf, 1, 0, NULL, &hAsync, 0 );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XStage_SlotUpdate
// Desc: Updates slot information from network message.
//       Called from DPMsgHandler in response to MSG_SLOTUPDATE
//-----------------------------------------------------------------------------
void XStage_SlotUpdate(STAGEMSG_SLOTDATA *msg)
{
	DWORD i;
	XMenuItem *mi;

	for(i=0; i<MAX_SLOTS; i++)
	{
		mi = g_MPSlots[i];
		XMenu_SetItemText(mi, msg->Slots[i].wstrPlayerName);
		mi->val1 = msg->Slots[i].dwStatus;
		mi->val2 = msg->Slots[i].dpnidPlayer;

		switch(mi->val1)
		{
			case SLOT_CLOSED:
				mi->color = SLOTCOLOR_CLOSED;
				break;
			case SLOT_OPEN:
				mi->color = SLOTCOLOR_OPEN;
				break;
			case SLOT_OCCUPIED:
				mi->color = SLOTCOLOR_OCCUPIED;
				break;
			case SLOT_READY:
				mi->color = SLOTCOLOR_READY;
				break;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: XStage_SendReject
// Desc: Sends a message to a player indicating they were rejected
//-----------------------------------------------------------------------------
void XStage_SendReject(DWORD dpnidPlayer)
{
	STAGEMSG_GENERIC msg;
    DPN_BUFFER_DESC buf;
    DPNHANDLE hAsync;

	msg.dwType = STAGEMSG_REJECT;
	buf.dwBufferSize = sizeof(STAGEMSG_GENERIC);
	buf.pBufferData = (BYTE *)&msg;
	g_pDP->SendTo(dpnidPlayer, &buf, 1, 0, NULL, &hAsync, 0);
}

//-----------------------------------------------------------------------------
// Name: XStage_SendSlotStatus
// Desc: Sends a message to the specified recipient indicating
//       the status of a particular slot.
//-----------------------------------------------------------------------------
void XStage_SendSlotStatus(DWORD sendto, DWORD slot, DWORD msgtype)
{
	STAGEMSG_SLOTINFO msg;
    DPN_BUFFER_DESC buf;
    DPNHANDLE hAsync;

	msg.dwType = msgtype;
	msg.dwSlot = slot;
	buf.dwBufferSize = sizeof(STAGEMSG_SLOTINFO);
	buf.pBufferData = (BYTE *)&msg;
	g_pDP->SendTo(sendto, &buf, 1, 0, NULL, &hAsync, DPNSEND_NOLOOPBACK);
}

//-----------------------------------------------------------------------------
// Name: XStage_StartGame
// Desc: Sends a message to all players indicating the start of the game
//-----------------------------------------------------------------------------
DWORD XStage_StartGame(DWORD cmd, XMenuItem *mi)
{
	DWORD i;
	BOOL flag;
	STAGEMSG_GENERIC msg;
    DPN_BUFFER_DESC buf;
    DPNHANDLE hAsync;

	// make sure everyone is ready
	flag = TRUE;
	for(i=1; i<MAX_SLOTS; i++)
		if(g_MPSlots[i]->val1==SLOT_OCCUPIED)
		{
			flag = FALSE;
			break;
		}

	// if everyone not ready, just return
	if(flag==FALSE)
		return MROUTINE_RETURN;

	// everyone was ready
	g_bAllowJoin = FALSE;				// dont allow anyone else to join
	g_dwGameState = GAMESTATE_PLAYING;

	// send start game message
	msg.dwType = STAGEMSG_STARTGAME;
	buf.dwBufferSize = sizeof(STAGEMSG_GENERIC);
	buf.pBufferData = (BYTE *)&msg;
	g_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &buf, 1, 0, NULL, &hAsync, 0);

	return MROUTINE_DIE;
}

//-----------------------------------------------------------------------------
// Name: XStage_AddPlayer
// Desc: Add a player to the MP stage
//-----------------------------------------------------------------------------
void XStage_AddPlayer(WCHAR *pName, DPNID dpnidPlayer)
{
	int slot;
	XMenuItem *mi;
	DPN_BUFFER_DESC buf;
	DPNHANDLE hAsync;

	slot = XStage_FindSlot();
	if(slot!=-1 && g_bAllowJoin)	// if we found a slot & join allowed
	{
		// fill in slot with new player info
		mi = g_MPSlots[slot];
		XMenu_SetItemText(mi, pName);
		mi->color = SLOTCOLOR_OCCUPIED;
		mi->val1 = SLOT_OCCUPIED;
		mi->val2 = dpnidPlayer;

		// send slot data to all players
		XStage_SendSlotData();

		// send slot number to the new player
		STAGEMSG_SLOTINFO msg;
		msg.dwType = STAGEMSG_SLOTNUMBER;
		msg.dwSlot = slot;
		buf.dwBufferSize = sizeof(STAGEMSG_SLOTINFO);
		buf.pBufferData = (BYTE *)&msg;
		g_pDP->SendTo(dpnidPlayer, &buf, 1, 0, NULL, &hAsync, 0);
	}
	else
	{
		XStage_SendReject(dpnidPlayer);
	}
}

//-----------------------------------------------------------------------------
// Name: XStage_MsgHandler
// Desc: Handles messages for mp staging menu
//       Note that the parameter is a message which is different
//       than a regular DPlay message handler
//-----------------------------------------------------------------------------
DWORD XStage_MsgHandler(void *pMsg)
{
	STAGEMSG_SLOTINFO *simsg;
	STAGEMSG_GENERIC *msg;

	msg = (STAGEMSG_GENERIC *)pMsg;

	switch(msg->dwType)
	{
		case STAGEMSG_STARTGAME:
			g_dwGameState = GAMESTATE_PLAYING;
			XMenu_Activate(NULL);				// kill mp stage menu
			break;

		// the following messages:
		//    SLOTOPEN, SLOTCLOSED, SLOTREADY, SLOTOCCUPIED
		// are sent from player to host, then propagated to
		// all other players to indicate slot status
		// 
		case STAGEMSG_SLOTOPEN:
			simsg = (STAGEMSG_SLOTINFO *)msg;
			g_MPSlots[simsg->dwSlot]->color = SLOTCOLOR_OPEN;
			g_MPSlots[simsg->dwSlot]->val1 = SLOT_OPEN;
			XMenu_SetItemText(g_MPSlots[simsg->dwSlot], L"Open");
			if(g_dwConnectStatus==GAME_HOSTING)
				XStage_SendSlotStatus(DPNID_ALL_PLAYERS_GROUP, simsg->dwSlot, STAGEMSG_SLOTOPEN);
			break;

		case STAGEMSG_SLOTCLOSED:
			simsg = (STAGEMSG_SLOTINFO *)msg;
			g_MPSlots[simsg->dwSlot]->color = SLOTCOLOR_CLOSED;
			g_MPSlots[simsg->dwSlot]->val1 = SLOT_CLOSED;
			XMenu_SetItemText(g_MPSlots[simsg->dwSlot], L"Closed");
			if(g_dwConnectStatus==GAME_HOSTING)
				XStage_SendSlotStatus(DPNID_ALL_PLAYERS_GROUP, simsg->dwSlot, STAGEMSG_SLOTCLOSED);
			break;

		case STAGEMSG_SLOTREADY:
			simsg = (STAGEMSG_SLOTINFO *)msg;
			g_MPSlots[simsg->dwSlot]->color = SLOTCOLOR_READY;
			g_MPSlots[simsg->dwSlot]->val1 = SLOT_READY;
			if(g_dwConnectStatus==GAME_HOSTING)
				XStage_SendSlotStatus(DPNID_ALL_PLAYERS_GROUP, simsg->dwSlot, STAGEMSG_SLOTREADY);
			break;

		case STAGEMSG_SLOTOCCUPIED:
			simsg = (STAGEMSG_SLOTINFO *)msg;
			g_MPSlots[simsg->dwSlot]->color = SLOTCOLOR_OCCUPIED;
			g_MPSlots[simsg->dwSlot]->val1 = SLOT_OCCUPIED;
			if(g_dwConnectStatus==GAME_HOSTING)
				XStage_SendSlotStatus(DPNID_ALL_PLAYERS_GROUP, simsg->dwSlot, STAGEMSG_SLOTOCCUPIED);
			break;

		// sent from host to a specific player to indicate
		// which slot we have been assigned
		case STAGEMSG_SLOTNUMBER:
			simsg = (STAGEMSG_SLOTINFO *)msg;
			g_dwLocalSlot = simsg->dwSlot;
			break;

		// update our table with the incoming slot data
		// sent from host to all players to perform a full
		// table update
		case STAGEMSG_SLOTUPDATE:
			if(g_dwConnectStatus!=GAME_HOSTING)
				XStage_SlotUpdate((STAGEMSG_SLOTDATA *)msg);
			break;

		// if we were rejected, disconnect
		case STAGEMSG_REJECT:
			g_dwConnectStatus = GAME_DISCONNECTING;		// flag DPlay disconnect
			g_dwGameState = GAMESTATE_ATTRACT;
			XMenu_Activate(NULL);						// kill mp stage menu
			break;
	}

	return 0;
}


void XStage_DeletePlayer(DPNID dpnid)
{
	XMenuItem *mi;
	DWORD i;

	// only do delete player if we are still staging
	if(g_dwGameState!=GAMESTATE_STAGING)
		return;

	// find this players slot
	for(i=0; i<MAX_SLOTS; i++)
	{
		mi = g_MPSlots[i];

		if(mi->val2==dpnid)
		{
			mi->color = SLOTCOLOR_OPEN;
			mi->val1 = SLOT_OPEN;
			wcscpy(mi->string, L"Open");
		}
	}

	// update everyones slot data
	XStage_SendSlotData();
}
