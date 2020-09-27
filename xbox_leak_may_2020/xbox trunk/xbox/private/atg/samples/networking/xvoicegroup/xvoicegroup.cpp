//-----------------------------------------------------------------------------
// File: xvoicegroup.cpp
//
// Desc: DirectPlay Voice sample
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
#include "xdpvoice.h"


//-----------------------------------------------------------------------------
// UI Stuff
//-----------------------------------------------------------------------------
XTextBox *XBox;									// pun intended
DWORD g_dwMenuCommand;
XMenu *g_MainMenu;								// main menu
XMenu *g_NameMenu;								// choose name menu


//-----------------------------------------------------------------------------
// player status information
//-----------------------------------------------------------------------------
#define VSTATUS_EMPTY		0					// slot is empty
#define VSTATUS_OCCUPIED	1					// slot is occupied
#define VSTATUS_TALKING		2					// slot is talking
#define VSTATUS_INGROUP		4					// slot is member of transmit group

#define MAX_STATUS			8					// max # of players in status
XTextBox *StatusBox;							// player status box (talking/not)
XTextBoxItem *g_pStatus[MAX_STATUS];			// status box items

void UpdateSlot(DWORD slot, WCHAR *tstrName);	// update status line


//-----------------------------------------------------------------------------
// dplay message handler
//-----------------------------------------------------------------------------
HRESULT WINAPI DPMsgHandler(PVOID pvUserContext, DWORD dwMsgType, PVOID pMsg);


//-----------------------------------------------------------------------------
// GUID & port
//-----------------------------------------------------------------------------
GUID g_AppGUID = { 0x9f2da206, 0xd197, 0x4871, { 0xad, 0x1d, 0xd1, 0x2f, 0x59, 0x5, 0x8b, 0x5b } };
DWORD g_dwPort = 2399;							// game port
#define PCPORT 6073								// port for PC game search


//-----------------------------------------------------------------------------
// multiplayer menu
//-----------------------------------------------------------------------------
XMenu *g_MPMenu;								// multiplayer menu
DWORD DoMPMenu(DWORD cmd, XMenuItem *mi);		// init & display MP menu
DWORD Create(DWORD cmd, XMenuItem *mi);			// host a game
DWORD Connect(DWORD cmd, XMenuItem *mi);		// connect to an existing game
DWORD Disconnect(DWORD cmd, XMenuItem *mi);		// disconnect from current game


//-----------------------------------------------------------------------------
// game menu
//-----------------------------------------------------------------------------
XMenu *g_GameMenu;								// game menu
DWORD FindGames(DWORD cmd, XMenuItem *mi);		// find game to join
DWORD AbortFindGames(DWORD cmd, XMenuItem *mi);	// stop finding games to join
void AddGame(ActiveGame *pGame);				// add an active game to game menu
void DeleteGame(ActiveGame *pGame);				// removes an active game from the game menu


//-----------------------------------------------------------------------------
// group menu
//-----------------------------------------------------------------------------
void BuildGroupMenu();							// builds the add/remove menu
DWORD AddToGroup(DWORD cmd, XMenuItem *mi);		// add player to group
DWORD RemoveFromGroup(DWORD cmd, XMenuItem *mi); // remove player from group


//-----------------------------------------------------------------------------
// App specific Player information 
//-----------------------------------------------------------------------------
#define MAX_PLAYER_NAME 14
struct APP_PLAYER_INFO
{
    LONG  lRefCount;							// Ref count so we can cleanup when all threads 
												// are done w/ this object
    DPNID dpnidPlayer;							// DPNID of player
    WCHAR wstrPlayerName[MAX_PLAYER_NAME];		// Player name
	DWORD dwIndex;								// index in textbox
};

#define PLAYER_ADDREF(a)    if(a) a->lRefCount++;
#define PLAYER_RELEASE(a)   if(a) { a->lRefCount--; if(a->lRefCount<=0) delete a; } a = NULL;

HRESULT CreatePlayer(DPNMSG_CREATE_PLAYER *pCreatePlayerMsg);
void DeletePlayer(DPNMSG_DESTROY_PLAYER *pDestroyPlayerMsg);


//-----------------------------------------------------------------------------
// help screen definitions
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_NormalHelpCallouts[] =
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Toggle Help\nMenu Back"},
    { XBHELP_START_BUTTON,  XBHELP_PLACEMENT_1, L"Menu Activate\nMenu Select"},
    { XBHELP_X_BUTTON,  XBHELP_PLACEMENT_1, L"Group Menu"},
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
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/3, 640.0f/480.0f, 1.0f, 10000.0f);
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
	XBox = XTextBox_Init(64, 480.0f-50.0f-(float)m_Font.GetFontHeight()*4.0f,
						640.0f-128.0f, 3, TBOX_POPUP);
	XBox->expiretime = 2000;		// keep messages up for 2 seconds
	XTextBox_AddItem(XBox, L"Welcome to XVoiceGroup");

	// init status textbox
	StatusBox = XTextBox_Init(128.0f, 110.0f, 640.0f-256.0f, 8, 0);
	StatusBox->topcolor = 0xff000000;
	StatusBox->bottomcolor = 0xffc0c000;
	for(DWORD i=0; i<MAX_STATUS; i++)
	{
		g_pStatus[i] = XTextBox_AddItem(StatusBox, L" ");
		g_pStatus[i]->val1 = VSTATUS_EMPTY;
	}

	// init dplay & dvoice
	XDPInit(DPMsgHandler);
	XDVInit(DPMsgHandler);

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
	// do our dplay and dvoice work
	XDPDoWork(0);
	XDVDoWork();

	// process menu input
	if(!g_bDrawHelp)
	{
		g_dwMenuCommand = XMenu_GetCommand(&m_DefaultGamepad);

		// check for main menu
		if(g_dwMenuCommand==MENU_ACTIVATE && !XMenu_IsActive())
			XMenu_Activate(g_MainMenu);

		// check for group menu
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] && !XMenu_IsActive())
			BuildGroupMenu();
	}

	// check for help screen
	if(m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_BACK && !XMenu_IsActive())
	{
		g_bDrawHelp = !g_bDrawHelp;
		if(g_bDrawHelp)
			return S_OK;
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

	if(!XMenu_IsActive())
		XTextBox_Display(StatusBox);

	// show game title
    m_Font.DrawText(64, 50, 0xffffffff, L"VOICE GROUP");
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
//       Also assembles the host session name to be "playername - XVoiceGroup"
//-----------------------------------------------------------------------------
DWORD GotName(DWORD cmd, XMenuItem *mi)
{
	// set player name
	wcscpy(g_wszXDPPlayerName, mi->string);

	// set session name
	wcscpy(g_wszXDPSessionName, g_wszXDPPlayerName);
	wcscat(g_wszXDPSessionName, L" - XVoiceGroup");

	XMenu_Delete(g_NameMenu);
	XMenu_Activate(g_MainMenu);

	return MROUTINE_RETURN;
}

void CXBoxSample::InitNameMenu()
{
	// init the choose player name menu
	g_NameMenu = XMenu_Init(320.0f, 100.0f, 4, MENU_NOBACK, NULL);
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
	DPNMSG_CREATE_PLAYER *pCreatePlayerMsg;
	DPNMSG_DESTROY_PLAYER *pDestroyPlayerMsg;
	DVMSG_CREATEVOICEPLAYER *pCreateDVPlayerMsg;
	DVMSG_DELETEVOICEPLAYER *pDeleteDVPlayerMsg;
	DVMSG_RECORDSTART *pRecStartMsg;
	DVMSG_RECORDSTOP *pRecStopMsg;
	DVMSG_PLAYERVOICESTART *pVoiceStartMsg;
	DVMSG_PLAYERVOICESTOP *pVoiceStopMsg;

	APP_PLAYER_INFO *pPlayerInfo;
	ActiveGame *pGame;
	WCHAR s[80];

    switch(dwMsgType)
    {
		// handle connecting to the DVoice session after
		// dplay connect is complete
        case DPN_MSGID_CONNECT_COMPLETE:
			if(g_dwConnectStatus!=GAME_HOSTING)
				XDVConnect(DVCLIENTCONFIG_MANUALVOICEACTIVATED);
			break;

		// set transmit targets to nobody when dvoice
		// has finished connecting
		case DVMSGID_CONNECTRESULT:
			XDVClearTransmitTargets();
			break;

		// handle adding and deleting games
        case DPN_MSGID_ENUM_HOSTS_RESPONSE:
			pGame = XDPAddGame((DPNMSG_ENUM_HOSTS_RESPONSE *)pMsg);
			AddGame(pGame);
			break;
		case DPPEER_MSGID_DELETE_GAME:
			DeleteGame((ActiveGame *)pMsg);
			break;

		// handle player create
        case DPN_MSGID_CREATE_PLAYER:
			pCreatePlayerMsg = (DPNMSG_CREATE_PLAYER *)pMsg;
			CreatePlayer(pCreatePlayerMsg);
			pPlayerInfo = (APP_PLAYER_INFO *)pCreatePlayerMsg->pvPlayerContext;
			swprintf(s, L"%s joined the game.", pPlayerInfo->wstrPlayerName);
			XTextBox_AddItem(XBox, s);
			break;
		case DVMSGID_CREATEVOICEPLAYER:
			pCreateDVPlayerMsg = (DVMSG_CREATEVOICEPLAYER *)pMsg;
			g_pDP->GetPlayerContext(pCreateDVPlayerMsg->dvidPlayer, (void **)&pPlayerInfo, 0);
			PLAYER_ADDREF(pPlayerInfo);
			pCreateDVPlayerMsg->pvPlayerContext = pPlayerInfo;

			// find an open slot in the status table
			pPlayerInfo->dwIndex = -1;
			for(int i=0; i<MAX_STATUS; i++)
				if(g_pStatus[i]->val1==VSTATUS_EMPTY)
				{
					pPlayerInfo->dwIndex = i;
					break;
				}

			assert(pPlayerInfo->dwIndex!=-1);

			// update display slot
			g_pStatus[pPlayerInfo->dwIndex]->val1 = VSTATUS_OCCUPIED;
			g_pStatus[pPlayerInfo->dwIndex]->val2 = (DWORD)pPlayerInfo;
			UpdateSlot(pPlayerInfo->dwIndex, pPlayerInfo->wstrPlayerName);
			break;

		// handle player destroy
        case DPN_MSGID_DESTROY_PLAYER:
			pDestroyPlayerMsg = (DPNMSG_DESTROY_PLAYER *)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pDestroyPlayerMsg->pvPlayerContext;
			swprintf(s, L"%s left the game.", pPlayerInfo->wstrPlayerName);
			DeletePlayer(pDestroyPlayerMsg);
			XTextBox_AddItem(XBox, s);
			break;

		case DVMSGID_DELETEVOICEPLAYER:
			pDeleteDVPlayerMsg = (DVMSG_DELETEVOICEPLAYER *)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pDeleteDVPlayerMsg->pvPlayerContext;
            swprintf(s, L"%s left the game.", pPlayerInfo->wstrPlayerName);
            XTextBox_AddItem(XBox, s);

			// free slot in the status table
			g_pStatus[pPlayerInfo->dwIndex]->val1 = VSTATUS_EMPTY;
			UpdateSlot(pPlayerInfo->dwIndex, L" ");

			PLAYER_RELEASE(pPlayerInfo);
			break;


		// update player talking/not talking display in 
		// response to these messages
		case DVMSGID_RECORDSTART:
			pRecStartMsg = (DVMSG_RECORDSTART *)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pRecStartMsg->pvLocalPlayerContext;
			g_pStatus[pPlayerInfo->dwIndex]->val1 |= VSTATUS_TALKING;
			UpdateSlot(pPlayerInfo->dwIndex, pPlayerInfo->wstrPlayerName);
			break;

		case DVMSGID_RECORDSTOP:
			pRecStopMsg = (DVMSG_RECORDSTOP *)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pRecStopMsg->pvLocalPlayerContext;
			g_pStatus[pPlayerInfo->dwIndex]->val1 &= ~VSTATUS_TALKING;
			UpdateSlot(pPlayerInfo->dwIndex, pPlayerInfo->wstrPlayerName);
			break;

		case DVMSGID_PLAYERVOICESTART:
			pVoiceStartMsg = (DVMSG_PLAYERVOICESTART *)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pVoiceStartMsg->pvPlayerContext;
			g_pStatus[pPlayerInfo->dwIndex]->val1 |= VSTATUS_TALKING;
			UpdateSlot(pPlayerInfo->dwIndex, pPlayerInfo->wstrPlayerName);
			break;

		case DVMSGID_PLAYERVOICESTOP:
			pVoiceStopMsg = (DVMSG_PLAYERVOICESTOP *)pMsg;
			pPlayerInfo = (APP_PLAYER_INFO *)pVoiceStopMsg->pvPlayerContext;
			g_pStatus[pPlayerInfo->dwIndex]->val1 &= ~VSTATUS_TALKING;
			UpdateSlot(pPlayerInfo->dwIndex, pPlayerInfo->wstrPlayerName);
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

	g_MPMenu = XMenu_Init(320.0f, 100.0f, 4, 0, NULL);
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
	XDPCreate(g_dwPort, &g_AppGUID, 8, 0);
	XDVCreate(DVSESSIONTYPE_PEER, DSCTID_SC06);

	// connect to the dvoice session we just started serving
	XDVConnect(DVCLIENTCONFIG_MANUALVOICEACTIVATED);

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

	// disconnect from session
	if(g_dwConnectStatus==GAME_HOSTING)
	{
		XDVStopSession();
		XDVDisconnect();
		XDPTerminate();
	}
	else
	{
		XDVDisconnect();
		XDPDisconnect();
	}

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

	g_GameMenu = XMenu_Init(320.0f, 100.0f, 20, 0, AbortFindGames);
	XMenu_SetTitle(g_GameMenu, L"Games", 0xffffff00);
	g_GameMenu->w = 440.0f;
	XMenu_SetMaxShow(g_GameMenu, 8);

	XMenu_AddItem(g_GameMenu, 0, L"ABORT", NULL);
	XMenu_Activate(g_GameMenu);

	// search for game on port specified in mi->val1
	// this is so we can find games on 2399 (other xboxes) and
	// 6073 (pc's running dx8 dplay)
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
	WCHAR s[80], name[80];
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

	// we still need to connect to the dvoice session
	// however, we wait until we receive a CONNECT_COMPLETE
	// message. see the CONNECT_COMPLETE message in the message
	// handler for this processing.

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

	// decrement number of players
	g_dwNumPlayers--;
}

//-----------------------------------------------------------------------------
// Name: BuildGroupMenu & related routines
// Desc: Builds the menu of players that can be added to or
//       removed from the group.
//-----------------------------------------------------------------------------
DWORD AddToGroup(DWORD cmd, XMenuItem *mi)
{
	APP_PLAYER_INFO *pPlayerInfo;

	pPlayerInfo = (APP_PLAYER_INFO *)mi->val2;

	g_pStatus[mi->val1]->val1 |= VSTATUS_INGROUP;
	UpdateSlot(mi->val1, pPlayerInfo->wstrPlayerName);

	XDVAddTransmitTarget((DVID)pPlayerInfo->dpnidPlayer);

	return MROUTINE_DIE;
}

DWORD RemoveFromGroup(DWORD cmd, XMenuItem *mi)
{
	APP_PLAYER_INFO *pPlayerInfo;

	pPlayerInfo = (APP_PLAYER_INFO *)mi->val2;

	g_pStatus[mi->val1]->val1 &= ~VSTATUS_INGROUP;
	UpdateSlot(mi->val1, pPlayerInfo->wstrPlayerName);

	XDVRemoveTransmitTarget(pPlayerInfo->dpnidPlayer);

	return MROUTINE_DIE;
}

void BuildGroupMenu()
{
	XMenu *pGroupMenu;
	XMenuItem *mi;
	APP_PLAYER_INFO *pPlayerInfo;
	DWORD i, val;
	WCHAR tstr[80];

	// init the group menu
	pGroupMenu = XMenu_Init(320.0f, 120.0f, MAX_STATUS, 0, NULL);
	XMenu_SetTitle(pGroupMenu, L"Group", 0xffffff00);

	// scan for players to add
	for(i=0; i<MAX_STATUS; i++)
	{
		val = g_pStatus[i]->val1;

		if(val&VSTATUS_OCCUPIED)
		{
			if(!(val&VSTATUS_INGROUP))
			{
				pPlayerInfo = (APP_PLAYER_INFO *)g_pStatus[i]->val2;

				// only add non-local players
				if(pPlayerInfo->dpnidPlayer!=g_dpnidLocalPlayer)
				{
					swprintf(tstr, L"Add %s", pPlayerInfo->wstrPlayerName);

					mi = XMenu_AddItem(pGroupMenu, MITEM_ROUTINE, tstr, AddToGroup);
					mi->val1 = i;
					mi->val2 = (DWORD)pPlayerInfo;
				}
			}
		}
	}

	// scan for players to remove
	for(i=0; i<MAX_STATUS; i++)
	{
		val = g_pStatus[i]->val1;

		if(val&VSTATUS_OCCUPIED)
		{
			if(val&VSTATUS_INGROUP)
			{
				pPlayerInfo = (APP_PLAYER_INFO *)g_pStatus[i]->val2;
				swprintf(tstr, L"Remove %s", pPlayerInfo->wstrPlayerName);

				mi = XMenu_AddItem(pGroupMenu, MITEM_ROUTINE, tstr, RemoveFromGroup);
				mi->val2 = (DWORD)pPlayerInfo;
				mi->val1 = i;
			}
		}
	}

	XMenu_Activate(pGroupMenu);
}


//-----------------------------------------------------------------------------
// Name: UpdateSlot
// Desc: Update player talking information
//-----------------------------------------------------------------------------
void UpdateSlot(DWORD slot, WCHAR *tstrName)
{
	assert(slot>=0 && slot<MAX_STATUS);

	if(g_pStatus[slot]->val1&VSTATUS_INGROUP)
		swprintf(g_pStatus[slot]->string, L"[%s]", tstrName);
	else
		wcsncpy(g_pStatus[slot]->string, tstrName, TBOX_STRINGLEN);

	if(g_pStatus[slot]->val1&VSTATUS_TALKING)
	{
		g_pStatus[slot]->color = 0xff00ff00;
		wcscat(g_pStatus[slot]->string, L" is talking");
	}
	else
		g_pStatus[slot]->color = 0xffffffff;
}
