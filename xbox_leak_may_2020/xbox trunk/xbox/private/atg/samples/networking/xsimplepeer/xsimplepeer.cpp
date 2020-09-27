//-----------------------------------------------------------------------------
// File: xsimplepeer.cpp
//
// Desc: Simple peer to peer networking sample.
//
// Note: Port choices should be in the range 2302-2400 as these are
//       reserved for DPlay applications.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <assert.h>

#include "xmenu.h"
#include "xtextbox.h"
#include "xdppeer.h"

// UI Stuff
XTextBox *XBox;									// pun intended
DWORD g_dwMenuCommand;
XMenu *g_MainMenu;								// main menu
XMenu *g_NameMenu;								// choose name menu

// dplay message handler
HRESULT WINAPI DPMsgHandler(PVOID pvUserContext, DWORD dwMsgType, PVOID pMsg);

// GUID & port
GUID g_AppGUID = {0x2ae835d, 0x9179, 0x485f, { 0x83, 0x43, 0x90, 0x1d, 0x32, 0x7c, 0xe7, 0x94 }};
DWORD g_dwPort = 2399;							// game port - see comment above
#define PCPORT 6073								// port for PC game search

// multiplayer menu
XMenu *g_MPMenu;								// multiplayer menu
DWORD DoMPMenu(DWORD cmd, XMenuItem *mi);		// init & display MP menu
DWORD Create(DWORD cmd, XMenuItem *mi);			// host a game
DWORD Connect(DWORD cmd, XMenuItem *mi);		// connect to an existing game
DWORD Disconnect(DWORD cmd, XMenuItem *mi);		// disconnect from current game

// game menu
XMenu *g_GameMenu;								// game menu
DWORD FindGames(DWORD cmd, XMenuItem *mi);		// find game to join
DWORD AbortFindGames(DWORD cmd, XMenuItem *mi);	// stop finding games to join
void AddGame(ActiveGame *pGame);				// add an active game to game menu
void DeleteGame(ActiveGame *pGame);				// removes an active game from the game menu

//-----------------------------------------------------------------------------
// App specific Player information 
//-----------------------------------------------------------------------------
#define MAX_PLAYER_NAME 14
struct APP_PLAYER_INFO
{
    LONG  lRefCount;                        // Ref count so we can cleanup when all threads 
                                            // are done w/ this object
    DPNID dpnidPlayer;                      // DPNID of player
    WCHAR wstrPlayerName[MAX_PLAYER_NAME];	// Player name
};
#define PLAYER_ADDREF(a)    if(a) a->lRefCount++;
#define PLAYER_RELEASE(a)   if(a) { a->lRefCount--; if(a->lRefCount<=0) delete a; } a = NULL;

HRESULT CreatePlayer(DPNMSG_CREATE_PLAYER *pCreatePlayerMsg);
void DeletePlayer(DPNMSG_DESTROY_PLAYER *pDestroyPlayerMsg);

//-----------------------------------------------------------------------------
// App specific DirectPlay messages and structures 
//-----------------------------------------------------------------------------
#define GAME_MSGID_WAVE 1

#pragma pack(push, 1)
struct GAMEMSG_GENERIC
{
    DWORD dwType;
};
#pragma pack(pop)

//-----------------------------------------------------------------------------
// help screen definitions
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_NormalHelpCallouts[] =
{
    { XBHELP_A_BUTTON,  XBHELP_PLACEMENT_1, L"Wave to Players"},
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Toggle Help\nMenu Back"},
    { XBHELP_START_BUTTON,  XBHELP_PLACEMENT_1, L"Menu Activate\nMenu Select"},
};
#define MAX_NORMAL_HELP_CALLOUTS 3

BOOL g_bDrawHelp = FALSE;
DWORD Help(DWORD cmd, XMenuItem *mi);		// help activate menu item

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Font for rendering stats and help
    CXBFont m_Font;
    CXBHelp m_Help;

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();
	HRESULT Cleanup();
	void InitNameMenu();

    CXBoxSample();
};

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
void __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}

//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;

    // Create a font
    if(FAILED(hr = m_Font.Create(m_pd3dDevice, "Font.xpr")))
        return XBAPPERR_MEDIANOTFOUND;

	// Initialize the help system
    if(FAILED(hr = m_Help.Create(m_pd3dDevice, "Gamepad.xpr")))
        return XBAPPERR_MEDIANOTFOUND;

	// set menu and text box fonts
	XMenu_SetFont(&m_Font);
	XTextBox_SetFont(&m_Font);

    // Set projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/3, 640.0f/480.0f, 1.0f, 1000.0f);
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// init the 'choose player name' menu
	InitNameMenu();

	// init the main menu
	g_MainMenu = XMenu_Init(320.0f, 120.0f, 5, 0, NULL);
	XMenu_SetTitle(g_MainMenu, L"Main Menu", 0xffffff00);
	XMenu_AddItem(g_MainMenu, 0, L"SINGLE PLAYER", NULL);
	XMenu_AddItem(g_MainMenu, MITEM_ROUTINE, L"MULTIPLAYER", DoMPMenu);
	XMenu_AddItem(g_MainMenu, MITEM_SEPARATOR, NULL, NULL);
	XMenu_AddItem(g_MainMenu, 0, L"OPTIONS", NULL);
	XMenu_AddItem(g_MainMenu, MITEM_ROUTINE, L"HELP", Help);

	// init a textbox
	XBox = XTextBox_Init(64, 480.0f-50.0f-(float)m_Font.GetFontHeight()*5.0f,
						640.0f-128.0f, 4, 0);
	XBox->expiretime = 3000;		// keep messages up for 3 seconds
	XBox->topcolor = 0xff000000;
	XBox->bottomcolor = 0xff0000c0;
	XTextBox_AddItem(XBox, L"Welcome to XSimplePeer.");

	// init dplay
	XDPInit(DPMsgHandler);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Clean up after ourselves.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Cleanup()
{
	XMenu_Delete(g_MainMenu);
	if(g_MPMenu)
		XMenu_Delete(g_MPMenu);

	// shut down dplay
	XDPShutdown();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	// do DPlay processing
	XDPDoWork(0);

	// process menu input
	if(!g_bDrawHelp)
	{
		g_dwMenuCommand = XMenu_GetCommand(&m_DefaultGamepad);
		if(g_dwMenuCommand==MENU_ACTIVATE)
			XMenu_Activate(g_MainMenu);
		if(XMenu_IsActive())
			return S_OK;
	}

	// toggle help menu
	if(m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_BACK)
		g_bDrawHelp = !g_bDrawHelp;
	if(g_bDrawHelp)
		return S_OK;

	// process game input
	if(XDPIsConnected())
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			// Send a message to all of the players
			GAMEMSG_GENERIC msgWave;
			msgWave.dwType = GAME_MSGID_WAVE;

			DPN_BUFFER_DESC bufferDesc;
			bufferDesc.dwBufferSize = sizeof(GAMEMSG_GENERIC);
			bufferDesc.pBufferData = (BYTE *)&msgWave;

			DPNHANDLE hAsync;
			g_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1,
						   0, NULL, &hAsync, DPNSEND_NOLOOPBACK);

			XTextBox_AddItem(XBox, L"You waved to everyone.");
		}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	WCHAR s[80];

    // clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0, 1.0f, 0L );

	// process the menu
	XMenu_Routine(g_dwMenuCommand);
	XTextBox_Display(XBox);

	// show game title
    m_Font.DrawText(64, 50, 0xffffffff, L"SIMPLE PEER");
    m_Font.DrawText(640-64, 50, 0xffffff00, m_strFrameRate, XBFONT_RIGHT);

	// show our current status
	switch(g_dwConnectStatus)
	{
		case GAME_HOSTING:
			m_Font.DrawText(64, 70, 0xff00ff00, L"HOSTING");
			break;
		case GAME_ENUMERATING:
			m_Font.DrawText(64, 70, 0xffffff00, L"ENUMERATING");
			break;
		case GAME_CONNECTED:
			m_Font.DrawText(64, 70, 0xff00ff00, L"CONNECTED");
			break;
		case GAME_NOTCONNECTED:
			m_Font.DrawText(64, 70, 0xffff0000, L"NOT CONNECTED");
			break;
		case GAME_CONNECTING:
			m_Font.DrawText(64, 70, 0xffffff00, L"CONNECTING");
			break;
	}

	// show # of players
	if(XDPIsConnected())
	{
		swprintf(s, L"%d PLAYERS", g_dwNumPlayers);
		m_Font.DrawText(640-64, 70, 0xffffffff, s, XBFONT_RIGHT);
	}		

	// draw help
	if(g_bDrawHelp)
		m_Help.Render(&m_Font, g_NormalHelpCallouts, MAX_NORMAL_HELP_CALLOUTS);

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Help
// Desc: Activates help screen from main menu.
//-----------------------------------------------------------------------------
DWORD Help(DWORD cmd, XMenuItem *mi)
{
	g_bDrawHelp = TRUE;
	return MROUTINE_DIE;
}

//-----------------------------------------------------------------------------
// Name: GotName and InitNameMenu
// Desc: Creates a menu of names to choose from. Since we dont have a
//       virtual keyboard, this lets us have clients with different names.
//       Also assembles the host session name to be "playername - XSimplePeer"
//-----------------------------------------------------------------------------
DWORD GotName(DWORD cmd, XMenuItem *mi)
{
	// set player name
	wcscpy(g_wszXDPPlayerName, mi->string);

	// set session name
	wcscpy(g_wszXDPSessionName, g_wszXDPPlayerName);
	wcscat(g_wszXDPSessionName, L" - XSimplePeer");

	XMenu_Delete(g_NameMenu);
	XMenu_Activate(g_MainMenu);

	return MROUTINE_RETURN;
}

void CXBoxSample::InitNameMenu()
{
	// init the choose player name menu
	g_NameMenu = XMenu_Init(320.0f, 120.0f, 4, MENU_NOBACK, NULL);
	XMenu_SetTitle(g_NameMenu, L"Choose Name", 0xffffff00);
	XMenu_AddItem(g_NameMenu, MITEM_ROUTINE, L"Maximus", GotName);
	XMenu_AddItem(g_NameMenu, MITEM_ROUTINE, L"Minimus", GotName);
	XMenu_AddItem(g_NameMenu, MITEM_ROUTINE, L"Animus", GotName);
	XMenu_AddItem(g_NameMenu, MITEM_ROUTINE, L"Haxor", GotName);
	XMenu_Activate(g_NameMenu);
}

//-----------------------------------------------------------------------------
// Name: DPMsgHandler
// Desc: Direct Play message handler callback routine.
//-----------------------------------------------------------------------------
HRESULT WINAPI DPMsgHandler(PVOID pvUserContext, DWORD dwMsgType, PVOID pMsg)
{
    DPNMSG_RECEIVE *pReceiveMsg;
	DPNMSG_CREATE_PLAYER *pCreatePlayerMsg;
	DPNMSG_DESTROY_PLAYER *pDestroyPlayerMsg;
	GAMEMSG_GENERIC *msg;

	APP_PLAYER_INFO *pPlayerInfo;
	ActiveGame *pGame;
	WCHAR s[80];

    switch(dwMsgType)
    {
		// handle adding and deleting games
        case DPN_MSGID_ENUM_HOSTS_RESPONSE:
			pGame = XDPAddGame((DPNMSG_ENUM_HOSTS_RESPONSE *)pMsg);
			AddGame(pGame);
			break;
		case DPPEER_MSGID_DELETE_GAME:
			DeleteGame((ActiveGame *)pMsg);
			break;

		// handle player messages
        case DPN_MSGID_CREATE_PLAYER:
			pCreatePlayerMsg = (DPNMSG_CREATE_PLAYER *)pMsg;
			CreatePlayer(pCreatePlayerMsg);
			pPlayerInfo = (APP_PLAYER_INFO *)pCreatePlayerMsg->pvPlayerContext;
			swprintf(s, L"%s joined the game.", pPlayerInfo->wstrPlayerName);
			XTextBox_AddItem(XBox, s);
			break;
        case DPN_MSGID_DESTROY_PLAYER:
			pDestroyPlayerMsg = (DPNMSG_DESTROY_PLAYER *)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pDestroyPlayerMsg->pvPlayerContext;
			swprintf(s, L"%s left the game.", pPlayerInfo->wstrPlayerName);
			DeletePlayer(pDestroyPlayerMsg);
			XTextBox_AddItem(XBox, s);
			break;

		// handle incoming game messages
        case DPN_MSGID_RECEIVE:
		    pReceiveMsg = (PDPNMSG_RECEIVE)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pReceiveMsg->pvPlayerContext;
            msg = (GAMEMSG_GENERIC *)pReceiveMsg->pReceiveData;
            if(msg->dwType == GAME_MSGID_WAVE)
			{
				swprintf(s, L"%s waved to you.", pPlayerInfo->wstrPlayerName);
				XTextBox_AddItem(XBox, s);
			}
			break;

		default:
			break;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DoMPMenu
// Desc: Builds multiplayer menu
//-----------------------------------------------------------------------------
DWORD DoMPMenu(DWORD cmd, XMenuItem *mi)
{
	XMenuItem *mitem;

	if(g_MPMenu)
		delete g_MPMenu;

	g_MPMenu = XMenu_Init(320.0f, 120.0f, 4, 0, NULL);
	XMenu_SetTitle(g_MPMenu, L"Multiplayer", 0xffffff00);
	XMenu_AddItem(g_MPMenu, MITEM_ROUTINE, L"CREATE GAME", Create);
	mitem = XMenu_AddItem(g_MPMenu, MITEM_ROUTINE, L"SEARCH FOR XBOX GAMES", FindGames);
	mitem->val1 = g_dwPort;
	mitem = XMenu_AddItem(g_MPMenu, MITEM_ROUTINE, L"SEARCH FOR PC GAMES", FindGames);
	mitem->val1 = PCPORT;
	XMenu_AddItem(g_MPMenu, MITEM_ROUTINE, L"DISCONNECT FROM GAME", Disconnect);

	XMenu_Activate(g_MPMenu);

	return MROUTINE_RETURN;
}


//-----------------------------------------------------------------------------
// Name: Create
// Desc: Host a game
//-----------------------------------------------------------------------------
DWORD Create(DWORD cmd, XMenuItem *mi)
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
DWORD Disconnect(DWORD cmd, XMenuItem *mi)
{
	// dont try to disconnect if we are not in a game
	if(!XDPIsConnected())
	{
		XTextBox_AddItem(XBox, L"You are not connected to a game.");
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
DWORD FindGames(DWORD cmd, XMenuItem *mi)
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

	g_GameMenu = XMenu_Init(320.0f, 140.0f, 20, 0, AbortFindGames);
	XMenu_SetTitle(g_GameMenu, L"Games", 0xffffff00);
	g_GameMenu->w = 440.0f;
	XMenu_SetMaxShow(g_GameMenu, 6);

	XMenu_AddItem(g_GameMenu, 0, L"ABORT", NULL);
	XMenu_Activate(g_GameMenu);

	// search for game on port specified in mi->val1
	// this is so we can find games on g_dwPort (other xboxes) and
	// PCPORT (pc's running dx8 dplay)
	// normally you would use g_dwPort value here
	XDPEnumHosts(mi->val1, &g_AppGUID);

	return MROUTINE_RETURN;
}

//-----------------------------------------------------------------------------
// Name: AbortFindGames
// Desc: Aborts finding active games on the network.
//-----------------------------------------------------------------------------
DWORD AbortFindGames(DWORD cmd, XMenuItem *mi)
{
	XDPCancelEnumHosts();
	return MROUTINE_DIE;
}

//-----------------------------------------------------------------------------
// Name: AddGame
// Desc: Adds to the menu of currently active games.
//-----------------------------------------------------------------------------
void AddGame(ActiveGame *pGame)
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
		mi = XMenu_AddItem(g_GameMenu, MITEM_ROUTINE, s, Connect);
		mi->val1 = (DWORD)pGame;		// save pointer to this game
	}
	else
		XMenu_SetItemText(mi, s);
}

//-----------------------------------------------------------------------------
// Name: DeleteGame
// Desc: Deletes from the menu of currently active games.
//-----------------------------------------------------------------------------
void DeleteGame(ActiveGame *pGame)
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
DWORD Connect(DWORD cmd, XMenuItem *mi)
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
HRESULT CreatePlayer(DPNMSG_CREATE_PLAYER *pCreatePlayerMsg)
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
void DeletePlayer(DPNMSG_DESTROY_PLAYER *pDestroyPlayerMsg)
{
    APP_PLAYER_INFO *pPlayerInfo;

	// get a pointer to the context of the player being deleted
	pPlayerInfo = (APP_PLAYER_INFO *)pDestroyPlayerMsg->pvPlayerContext;

	// release player info
	PLAYER_RELEASE(pPlayerInfo);  

	// decrement # of players
	g_dwNumPlayers--;
}

