//-----------------------------------------------------------------------------
// Notes: This attempts to show a basic usage of networking on
//        the XBox with DPlay. It absolutely does *NOT* illustrate
//        an adequate implementation of game useable networking
//        over a WAN. Much better dead-reckoning and prediction
//        would be necessary for reliable gameplay over a WAN.
//        Further releases will attempt to improve this, but 
//        this one does not.
//
//        It also does not handle arbitration at all. There needs
//        to be a final arbiter of events like getting hit, picking
//        up powerups, etc.
//
//        Time synchronization is also not handled. This would improve
//        prediction ability as well. The lack of synchronization will
//        cause large inconsistancies in the locations of 
//        weapons from client to client.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <assert.h>

#include "xmenu.h"
#include "xtextbox.h"
#include "xdppeer.h"
#include "net.h"

#include "xarena.h"

extern XTextBox *XBox;

//-----------------------------------------------------------------------------
// dplay message handler
//-----------------------------------------------------------------------------
HRESULT WINAPI Net_DPMsgHandler(PVOID pvUserContext, DWORD dwMsgType, PVOID pMsg);

//-----------------------------------------------------------------------------
// GUID & port (fake guid)
//-----------------------------------------------------------------------------
GUID g_AppGUID = {0x2ae835e, 0x9179, 0x485f, { 0x83, 0x43, 0x90, 0x1d, 0x32, 0x7c, 0xe7, 0x94 }};
DWORD g_dwPort = 2399;								// game port

//-----------------------------------------------------------------------------
// multiplayer menu
//-----------------------------------------------------------------------------
XMenu *g_MPMenu;									// multiplayer menu
DWORD Net_Create(DWORD cmd, XMenuItem *mi);			// host a game
DWORD Net_Connect(DWORD cmd, XMenuItem *mi);		// connect to an existing game
DWORD Net_Disconnect(DWORD cmd, XMenuItem *mi);		// disconnect from current game

//-----------------------------------------------------------------------------
// game menu
//-----------------------------------------------------------------------------
XMenu *g_GameMenu;									// game menu
DWORD Net_FindGames(DWORD cmd, XMenuItem *mi);		// find game to join
DWORD Net_AbortFindGames(DWORD cmd, XMenuItem *mi);	// stop finding games to join
void Net_AddGame(ActiveGame *pGame);				// add an active game to game menu
void Net_DeleteGame(ActiveGame *pGame);				// removes an active game from the game menu

//-----------------------------------------------------------------------------
// App specific Player information 
//-----------------------------------------------------------------------------
#define PLAYER_ADDREF(a)    if(a) a->lRefCount++;
#define PLAYER_RELEASE(a)   if(a) {a->lRefCount--; if(a->lRefCount<=0) delete a;} a = NULL;

HRESULT Net_CreatePlayer(DPNMSG_CREATE_PLAYER *pCreatePlayerMsg);
void Net_DeletePlayer(DPNMSG_DESTROY_PLAYER *pDestroyPlayerMsg);

//-----------------------------------------------------------------------------
// App specific DirectPlay messages and structures 
//-----------------------------------------------------------------------------
#define MSGID_TANKUPDATE	0x01		// tank update
#define MSGID_FIRE			0x02		// weapon fire
#define MSGID_KILL			0x03		// someone was killed

#pragma pack(push, 1)
struct MSG_TYPE
{
    BYTE bType;
};

struct MSG_TANKUPDATE
{
	BYTE bType;

	WORD wPosx, wPosy, wPosz;
	WORD wVelx, wVely, wVelz;
	WORD wAccx, wAccz;
	WORD wYRot;
	WORD wRotVel;
};

void Net_AddTank(APP_PLAYER_INFO *pPlayerInfo);
void Net_UpdateTank(MSG_TANKUPDATE *tu, CXTank *tank);

struct MSG_FIRE
{
	BYTE bType;
	BYTE bWeaponType;
	WORD wPosx, wPosy, wPosz;
	float fRot;
};

CXObject *Net_FireWeapon(MSG_FIRE *fw, CXTank *tank);

struct MSG_KILL
{
	BYTE bType;
	DPNID dpnidKiller;
};

void Net_ShowKill(MSG_KILL *km, APP_PLAYER_INFO *pPlayerInfo);		// show kill message

#pragma pack(pop)


//-----------------------------------------------------------------------------
// Helper routines to minimize bandwidth usage
//-----------------------------------------------------------------------------
DWORD g_dwBytesSent=0;
DWORD g_dwBytesReceived=0;

void Net_VPosToWPos(D3DXVECTOR3 *pPos, WORD *wPosx, WORD *wPosy, WORD *wPosz);
void Net_WPosToVPos(WORD wPosx, WORD wPosy, WORD wPosz, D3DXVECTOR3 *pPos);


//-----------------------------------------------------------------------------
// Name: DPMsgHandler
// Desc: Direct Play message handler callback routine.
//-----------------------------------------------------------------------------
HRESULT Net_Init()
{
	// init dplay
	XDPInit(Net_DPMsgHandler);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DPMsgHandler
// Desc: Direct Play message handler callback routine.
//-----------------------------------------------------------------------------
HRESULT WINAPI Net_DPMsgHandler(PVOID pvUserContext, DWORD dwMsgType, PVOID pMsg)
{
    DPNMSG_RECEIVE *pReceiveMsg;
	DPNMSG_CREATE_PLAYER *pCreatePlayerMsg;
	DPNMSG_DESTROY_PLAYER *pDestroyPlayerMsg;
	
	MSG_TYPE *msg;

	APP_PLAYER_INFO *pPlayerInfo;
	ActiveGame *pGame;
	WCHAR s[80];

    switch(dwMsgType)
    {
		// handle adding and deleting games
        case DPN_MSGID_ENUM_HOSTS_RESPONSE:
			pGame = XDPAddGame((DPNMSG_ENUM_HOSTS_RESPONSE *)pMsg);
			Net_AddGame(pGame);
			break;
		case DPPEER_MSGID_DELETE_GAME:
			Net_DeleteGame((ActiveGame *)pMsg);
			break;

		// handle player messages
        case DPN_MSGID_CREATE_PLAYER:
			pCreatePlayerMsg = (DPNMSG_CREATE_PLAYER *)pMsg;
			Net_CreatePlayer(pCreatePlayerMsg);
			pPlayerInfo = (APP_PLAYER_INFO *)pCreatePlayerMsg->pvPlayerContext;
			swprintf(s, L"%s joined the game.", pPlayerInfo->wstrPlayerName);
			XTextBox_AddItem(XBox, s);
			break;
        case DPN_MSGID_DESTROY_PLAYER:
			pDestroyPlayerMsg = (DPNMSG_DESTROY_PLAYER *)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pDestroyPlayerMsg->pvPlayerContext;
			swprintf(s, L"%s left the game.", pPlayerInfo->wstrPlayerName);
			Net_DeletePlayer(pDestroyPlayerMsg);
			XTextBox_AddItem(XBox, s);
			break;

		// handle incoming game messages
        case DPN_MSGID_RECEIVE:
		    pReceiveMsg = (PDPNMSG_RECEIVE)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pReceiveMsg->pvPlayerContext;
            msg = (MSG_TYPE *)pReceiveMsg->pReceiveData;
			
			switch(msg->bType)
			{
				case MSGID_TANKUPDATE:
					g_dwBytesReceived += sizeof(MSG_TANKUPDATE);

					Net_UpdateTank((MSG_TANKUPDATE *)pReceiveMsg->pReceiveData, pPlayerInfo->pTank);
					break;

				case MSGID_FIRE:
					g_dwBytesReceived += sizeof(MSG_FIRE);

					CXObject *obj;
					obj = Net_FireWeapon((MSG_FIRE *)pReceiveMsg->pReceiveData, pPlayerInfo->pTank);
					obj->m_pvInfo = (void *)pPlayerInfo->dpnidPlayer;
					break;

				case MSGID_KILL:
					g_dwBytesReceived += sizeof(MSG_KILL);

					Net_ShowKill((MSG_KILL *)pReceiveMsg->pReceiveData, pPlayerInfo);
					break;

			}
			break;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Net_Menu
// Desc: Builds multiplayer menu
//-----------------------------------------------------------------------------
DWORD Net_Menu(DWORD cmd, XMenuItem *mi)
{
	XMenuItem *mitem;

	if(g_MPMenu)
		delete g_MPMenu;

	g_MPMenu = XMenu_Init(320.0f, 120.0f, 4, 0, NULL);
	XMenu_SetTitle(g_MPMenu, L"Multiplayer Menu", 0xffffff00);

	XMenu_AddItem(g_MPMenu, MITEM_ROUTINE, L"CREATE GAME", Net_Create);
	mitem = XMenu_AddItem(g_MPMenu, MITEM_ROUTINE, L"SEARCH FOR XBOX GAMES", Net_FindGames);
	mitem = XMenu_AddItem(g_MPMenu, MITEM_ROUTINE, L"SEARCH FOR PC GAMES", Net_FindGames);
	XMenu_AddItem(g_MPMenu, MITEM_ROUTINE, L"DISCONNECT FROM GAME", Net_Disconnect);

	XMenu_Activate(g_MPMenu);

	return MROUTINE_RETURN;
}

//-----------------------------------------------------------------------------
// Name: Create
// Desc: Host a game
//-----------------------------------------------------------------------------
DWORD Net_Create(DWORD cmd, XMenuItem *mi)
{
	// dont try to create if we are already in a game
	if(XDPIsConnected())
	{
		XTextBox_AddItem(XBox, L"Disconnect before creating a new game.");
		return MROUTINE_DIE;
	}

	// create the game
	XDPCreate(g_dwPort, &g_AppGUID, 6, 0);

	return MROUTINE_DIE;
}

//-----------------------------------------------------------------------------
// Name: Disconnect
// Desc: Disconnect from the current game
//-----------------------------------------------------------------------------
DWORD Net_Disconnect(DWORD cmd, XMenuItem *mi)
{
	// dont try to disconnect if we are not in a game
	if(!XDPIsConnected())
	{
		XTextBox_AddItem(XBox, L"Not connected to a game.");
		return MROUTINE_DIE;
	}

	if(g_dwConnectStatus==GAME_HOSTING)
		XDPTerminate();
	else
		XDPDisconnect();

	XTextBox_AddItem(XBox, L"Disconnected from the game.");

	return MROUTINE_DIE;
}

//-----------------------------------------------------------------------------
// Name: FindGames
// Desc: Finds active games on the network.
//-----------------------------------------------------------------------------
DWORD Net_FindGames(DWORD cmd, XMenuItem *mi)
{
	// dont try to create if we are already in a game
	if(XDPIsConnected())
	{
		XTextBox_AddItem(XBox, L"Disconnect before searching for new games.");
		return MROUTINE_DIE;
	}

	// init game menu
	if(g_GameMenu)
		XMenu_Delete(g_GameMenu);

	g_GameMenu = XMenu_Init(320.0f, 140.0f, 20, 0, Net_AbortFindGames);
	XMenu_SetTitle(g_GameMenu, L"Game Menu", 0xffffff00);
	g_GameMenu->w = 440.0f;
	XMenu_SetMaxShow(g_GameMenu, 6);

	XMenu_AddItem(g_GameMenu, 0, L"ABORT", NULL);
	XMenu_Activate(g_GameMenu);

	XDPEnumHosts(g_dwPort, &g_AppGUID);

	return MROUTINE_RETURN;
}

//-----------------------------------------------------------------------------
// Name: AbortFindGames
// Desc: Aborts finding active games on the network.
//-----------------------------------------------------------------------------
DWORD Net_AbortFindGames(DWORD cmd, XMenuItem *mi)
{
	XDPCancelEnumHosts();
	return MROUTINE_DIE;
}

//-----------------------------------------------------------------------------
// Name: AddGame
// Desc: Adds to the menu of currently active games.
//-----------------------------------------------------------------------------
void Net_AddGame(ActiveGame *pGame)
{
	XMenuItem *mi;
	const DPN_APPLICATION_DESC *pApp;
	WCHAR name[80], s[80];
	DWORD i;

	assert(pGame);

	pApp = &pGame->AppDesc;

	// see if this item already in the menu
	mi = NULL;
	for(i=0; i<g_GameMenu->nitems; i++)
		if((DWORD)pGame==g_GameMenu->items[i].val1)
		{
			mi = &g_GameMenu->items[i];
			break;
		}

	// build menu item string
	wcscpy(name, pApp->pwszSessionName);
	swprintf(s, L"%s (%d, %d/%d)", name, pGame->dwPing, pApp->dwCurrentPlayers, pApp->dwMaxPlayers);

	// add new item to menu or update existing item
	if(!mi)
	{
		mi = XMenu_AddItem(g_GameMenu, MITEM_ROUTINE, s, Net_Connect);
		mi->val1 = (DWORD)pGame;		// save pointer to this game
	}
	else
		XMenu_SetItemText(mi, s);
}

//-----------------------------------------------------------------------------
// Name: DeleteGame
// Desc: Deletes from the menu of currently active games.
//-----------------------------------------------------------------------------
void Net_DeleteGame(ActiveGame *pGame)
{
	DWORD i;

	for(i=0; i<g_GameMenu->nitems; i++)
		if((DWORD)pGame==g_GameMenu->items[i].val1)
		{
			XMenu_DeleteItem(&g_GameMenu->items[i]);
			break;
		}
}

//-----------------------------------------------------------------------------
// Name: Connect
// Desc: Connect to an existing game
//-----------------------------------------------------------------------------
DWORD Net_Connect(DWORD cmd, XMenuItem *mi)
{
	ActiveGame *pGame;

	// dont try to connect if we are already in a game
	if(XDPIsConnected())
	{
		XTextBox_AddItem(XBox, L"Disconnect before connecting to a new game.");
		return MROUTINE_DIE;
	}

	pGame = (ActiveGame *)mi->val1;			// get pointer to desired game
	XDPConnect(&pGame->AppDesc, pGame->pHostAddr, pGame->pDevAddr);

	return MROUTINE_DIE;
}

//-----------------------------------------------------------------------------
// Name: CreatePlayer
// Desc: Add a player to the current game.
//       Called from DPMsgHandler in response to DPN_MSGID_CREATE_PLAYER
//-----------------------------------------------------------------------------
HRESULT Net_CreatePlayer(DPNMSG_CREATE_PLAYER *pCreatePlayerMsg)
{
    HRESULT hr;
    DWORD dwSize = 0;
    DPN_PLAYER_INFO *pdpPlayerInfo = NULL;

    // Get the peer info and extract its name
    hr = g_pDP->GetPeerInfo(pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0);
    if(FAILED(hr) && hr!=DPNERR_BUFFERTOOSMALL)
        return hr;

    pdpPlayerInfo = (DPN_PLAYER_INFO *) new BYTE[dwSize];
    ZeroMemory(pdpPlayerInfo, dwSize);
    pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);

    hr = g_pDP->GetPeerInfo(pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0);
    if(FAILED(hr))
        return hr;

    // Create a new and fill in a APP_PLAYER_INFO
    APP_PLAYER_INFO *pPlayerInfo = new APP_PLAYER_INFO;
    ZeroMemory(pPlayerInfo, sizeof(APP_PLAYER_INFO));
    pPlayerInfo->lRefCount = 1;				   
    pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;

	// copy players name
	wcsncpy(pPlayerInfo->wstrPlayerName, pdpPlayerInfo->pwszName, MAX_PLAYER_NAME);

	// check for local player dpnid
	if(pdpPlayerInfo->dwPlayerFlags&DPNPLAYER_LOCAL)
		g_dpnidLocalPlayer = pCreatePlayerMsg->dpnidPlayer;
	else
		Net_AddTank(pPlayerInfo);		// if not local, add the tank

	// check for host player dpnid
    if(pdpPlayerInfo->dwPlayerFlags&DPNPLAYER_HOST)
        g_dpnidHost = pPlayerInfo->dpnidPlayer;

    // Tell DirectPlay to store this pPlayerInfo pointer in the pvPlayerContext.
    pCreatePlayerMsg->pvPlayerContext = pPlayerInfo;

	// increment number of players
	g_dwNumPlayers++;

	return hr;
}

//-----------------------------------------------------------------------------
// Name: DeletePlayer
// Desc: Deletes a player from the current game.
//       Called from DPMsgHandler in response to DPN_MSGID_DELETE_PLAYER
//-----------------------------------------------------------------------------
void Net_DeletePlayer(DPNMSG_DESTROY_PLAYER *pDestroyPlayerMsg)
{
    APP_PLAYER_INFO *pPlayerInfo;
	DWORD dwTankIdx;

	// get a pointer to the context of the player being deleted
	pPlayerInfo = (APP_PLAYER_INFO *)pDestroyPlayerMsg->pvPlayerContext;
	dwTankIdx = pPlayerInfo->dwTankIdx;

	// only delete tank if its remotely controlled
	if(g_pTank[dwTankIdx]->m_dwFlags&TANK_REMOTE)
	{
		delete g_pTank[dwTankIdx];
		g_dwNumTanks--;

		if(dwTankIdx<g_dwNumTanks)
		{
			g_pTank[dwTankIdx] = g_pTank[g_dwNumTanks];
			g_pTank[dwTankIdx]->m_dwTankIdx = dwTankIdx;
		}
	}

	// release player info
	PLAYER_RELEASE(pPlayerInfo);  

	// decrement # of players
	g_dwNumPlayers--;
}


//-----------------------------------------------------------------------------
// Name: VPosToWPos & WPosToVPos
// Desc: Convert 96 bit floating point position to 48 bit word position.
//       Used to minimize bandwidth usage.
//-----------------------------------------------------------------------------
void Net_VPosToWPos(D3DXVECTOR3 *pPos, WORD *wPosx, WORD *wPosy, WORD *wPosz)
{
	*wPosx = (WORD)((pPos->x+(FLOORX/2.0f)) * (float)(1<<(16-POSXBITS)));
	*wPosy = (WORD)((pPos->y+(FLOORY/2.0f)) * (float)(1<<(16-POSYBITS)));
	*wPosz = (WORD)((pPos->z+(FLOORZ/2.0f)) * (float)(1<<(16-POSZBITS)));
}

void Net_WPosToVPos(WORD wPosx, WORD wPosy, WORD wPosz, D3DXVECTOR3 *pPos)
{
	pPos->x = (float)wPosx / (float)(1<<(16-POSXBITS)) - (FLOORX/2.0f);
	pPos->y = (float)wPosy / (float)(1<<(16-POSYBITS)) - (FLOORY/2.0f);
	pPos->z = (float)wPosz / (float)(1<<(16-POSZBITS)) - (FLOORZ/2.0f);
}

void Net_FToW(float f, WORD *w, float lo, float hi)
{
	assert(f>=lo);
	assert(f<=hi);

	*w = (WORD)((f-lo)/(hi-lo)*65535.0f);
}

void Net_WToF(WORD w, float *f, float lo, float hi)
{
	*f = (float)w/65535.0f * (hi-lo) + lo;
}

void Net_FVecToWVec(D3DXVECTOR3 *v, WORD *wx, WORD *wy, WORD *wz, float lo, float hi)
{
	float d;

	d = 65535.0f/(hi-lo);

	*wx = (WORD)((v->x-lo)*d);
	*wy = (WORD)((v->y-lo)*d);
	*wz = (WORD)((v->z-lo)*d);
}

void Net_WVecToFVec(WORD wx, WORD wy, WORD wz, D3DXVECTOR3 *v, float lo, float hi)
{
	float d;

	d = (hi-lo)/65535.0f;

	v->x = (float)wx*d + lo;
	v->y = (float)wy*d + lo;
	v->z = (float)wz*d + lo;
}


//-----------------------------------------------------------------------------
// Name: Game Routines
// Desc: These routines handle the sending and receiving of the primary
//       data that makes up the game state.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: AddTank
// Desc: Called when another player enters the game.
//-----------------------------------------------------------------------------
void Net_AddTank(APP_PLAYER_INFO *pPlayerInfo)
{
	D3DXVECTOR3 v1;

	v1 = D3DXVECTOR3(100.0f, 0.0f, 100.0f);
	pPlayerInfo->pTank = new CXTank(&v1, D3DX_PI);
	pPlayerInfo->dwTankIdx = g_dwNumTanks;
	g_pTank[g_dwNumTanks] = pPlayerInfo->pTank;
	g_pTank[g_dwNumTanks]->m_dwTankIdx = g_dwNumTanks;
	g_pTank[g_dwNumTanks]->m_dwFlags |= TANK_REMOTE;
	g_dwNumTanks++;
}

//-----------------------------------------------------------------------------
// Name: SendUpdateTank
// Desc: Sends tank information to other clients.
//-----------------------------------------------------------------------------
void Net_SendUpdateTank(CXTank *tank)
{
	MSG_TANKUPDATE tu;
	WORD w;	// dummy

	tu.bType = MSGID_TANKUPDATE;

	Net_FVecToWVec(&tank->m_pObj->m_vPosition,
					&tu.wPosx, &tu.wPosy, &tu.wPosz,
					-FLOORX/2.0f, FLOORX/2.0f);

	Net_FVecToWVec(&tank->m_vVel, &tu.wVelx, &tu.wVely, &tu.wVelz,
					-100.0f, 100.0f);
	Net_FVecToWVec(&tank->m_vAcc, &tu.wAccx, &w, &tu.wAccz,
					-100.0f, 100.0f);

	Net_FToW(tank->m_pObj->m_vRotation.y, &tu.wYRot, -10.0f, 10.0f);
	Net_FToW(tank->m_fRotVel, &tu.wRotVel, -10.0f, 10.0f);

	DPN_BUFFER_DESC bufferDesc;
	bufferDesc.dwBufferSize = sizeof(MSG_TANKUPDATE);
	bufferDesc.pBufferData = (BYTE *)&tu;

	DPNHANDLE hAsync;
	g_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1,
				   0, NULL, &hAsync, DPNSEND_NOLOOPBACK);

	g_dwBytesSent += sizeof(MSG_TANKUPDATE);
}

//-----------------------------------------------------------------------------
// Name: UpdateTank
// Desc: Updates tanks based on information received from other clients.
//-----------------------------------------------------------------------------
void Net_UpdateTank(MSG_TANKUPDATE *tu, CXTank *tank)
{
	D3DXVECTOR3 vPos;

	Net_WVecToFVec(tu->wPosx, tu->wPosy, tu->wPosz, &vPos, -FLOORX/2.0f, FLOORX/2.0f);

	// check for warp
	D3DXVECTOR3 dp = vPos - tank->m_pObj->m_vPosition;
	float dist = D3DXVec3Length(&dp);
	if((dist>10.0f) || (dist<1.0f))
	{
		tank->m_dwNumWarpFrames = 0;
		tank->m_pObj->m_vPosition = vPos;
	}
	else
	{
		tank->m_dwNumWarpFrames = (DWORD)dist;
		tank->m_vWarpDist = dp/(float)tank->m_dwNumWarpFrames;
	}

	Net_WVecToFVec(tu->wVelx, tu->wVely, tu->wVelz, &tank->m_vVel, -100.0f, 100.0f);
	Net_WVecToFVec(tu->wAccx, 0, tu->wAccz, &tank->m_vAcc, -100.0f, 100.0f);

	Net_WToF(tu->wYRot, &tank->m_pObj->m_vRotation.y, -10.0f, 10.0f);
	Net_WToF(tu->wRotVel, &tank->m_fRotVel, -10.0f, 10.0f);
}



//-----------------------------------------------------------------------------
// Name: SendFireWeapon
// Desc: Sends weapon fire event to other clients.
//-----------------------------------------------------------------------------
void Net_SendFireWeapon(CXObject *pObj, BYTE bType)
{
	MSG_FIRE fw;

	fw.bType = MSGID_FIRE;
	fw.bWeaponType = bType;
	Net_VPosToWPos(&pObj->m_vPosition, &fw.wPosx, &fw.wPosy, &fw.wPosz);
	fw.fRot = pObj->m_vRotation.y;

	DPN_BUFFER_DESC bufferDesc;
	bufferDesc.dwBufferSize = sizeof(MSG_FIRE);
	bufferDesc.pBufferData = (BYTE *)&fw;

	DPNHANDLE hAsync;
	g_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1,
				   0, NULL, &hAsync, DPNSEND_NOLOOPBACK);

	g_dwBytesSent += sizeof(MSG_FIRE);
}

//-----------------------------------------------------------------------------
// Name: FireWeapon
// Desc: Fires weapons locally based on messages received from other clients.
//-----------------------------------------------------------------------------
CXObject *Net_FireWeapon(MSG_FIRE *fw, CXTank *tank)
{
	D3DXVECTOR3 vPos;

	Net_WPosToVPos(fw->wPosx, fw->wPosy, fw->wPosz, &vPos);

	return Weapon_Fire(&vPos, fw->fRot, fw->bWeaponType, WF_REMOTE);
}


//-----------------------------------------------------------------------------
// Name: SendKill
// Desc: Tell everyone we were killed. Dont use NOLOOPBACK flag so we 
//       get the message too.
//-----------------------------------------------------------------------------
void Net_SendKill(DPNID dpnidKiller)
{
	MSG_KILL km;

	km.bType = MSGID_KILL;
	km.dpnidKiller = dpnidKiller;

	DPN_BUFFER_DESC bufferDesc;
	bufferDesc.dwBufferSize = sizeof(MSG_KILL);
	bufferDesc.pBufferData = (BYTE *)&km;

	DPNHANDLE hAsync;
	g_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 
					0, NULL, &hAsync, 0);

	g_dwBytesSent += sizeof(MSG_KILL);
}

//-----------------------------------------------------------------------------
// Name: ShowKill
// Desc: Display a humiliating kill message.
//-----------------------------------------------------------------------------
void Net_ShowKill(MSG_KILL *km, APP_PLAYER_INFO *pPlayerInfo)
{
	WCHAR s[80];
	APP_PLAYER_INFO *pKiller;
	DPNID Killed, Killer;

	Killer = km->dpnidKiller;
	g_pDP->GetPlayerContext(Killer, (VOID **)&pKiller, 0);
	Killed = pPlayerInfo->dpnidPlayer;

	if(Killed==g_dpnidLocalPlayer)
	{
		// local player was killed, check if he killed himself
		if(Killed==Killer)
			swprintf(s, L"You killed yourself");
		else
			swprintf(s, L"You were killed by %s", pKiller->wstrPlayerName);
	}
	else if(Killer==g_dpnidLocalPlayer)
		swprintf(s, L"You killed %s", pPlayerInfo->wstrPlayerName);
	else
	{
		if(Killed==Killer)
			swprintf(s, L"%s killed himself", pPlayerInfo->wstrPlayerName);
		else
			swprintf(s, L"%s killed %s", pKiller->wstrPlayerName, pPlayerInfo->wstrPlayerName);
	}

	XTextBox_AddItem(XBox, s);	
}
