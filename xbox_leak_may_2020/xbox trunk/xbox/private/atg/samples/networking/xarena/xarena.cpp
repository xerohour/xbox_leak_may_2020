//-----------------------------------------------------------------------------
// File: xarena.cpp
//
// Desc: Sample game to illustrate online functionality for the xbox.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <assert.h>

#include "xdppeer.h"
#include "xpath.h"
#include "xmenu.h"
#include "xtextbox.h"
#include "xobj.h"
#include "xpart.h"

#include "xarena.h"
#include "net.h"
#include "xtank.h"

extern CXModel Weapon_GunModel;
CXParticleSystem *XAPart = NULL;

// UI Stuff
XTextBox *XBox;									// pun intended
DWORD g_dwMenuCommand;
XMenu *g_MainMenu;								// main menu
XMenu *g_NameMenu;								// choose name menu

D3DXMATRIX g_mView;								// view matrix

// tanks
CXTank *g_pTank[MAX_TANKS];
DWORD g_dwNumTanks;

CXObject g_objArena;
CXBMesh *g_pxbmArena;

//-----------------------------------------------------------------------------
// help screen definitions
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_NormalHelpCallouts[] =
{
    { XBHELP_LEFTSTICK,  XBHELP_PLACEMENT_1, L"Move Tank"},
    { XBHELP_RIGHTSTICK,  XBHELP_PLACEMENT_1, L"Rotate Tank"},
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Toggle Help\nMenu Back"},
    { XBHELP_START_BUTTON,  XBHELP_PLACEMENT_1, L"Menu Activate\nMenu Select"},
	{ XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_1, L"Left Trigger: Switch Weapons\nRight Trigger: Fire Weapon"}
};
#define MAX_NORMAL_HELP_CALLOUTS 5

BOOL g_bDrawHelp = FALSE;
DWORD Help(DWORD cmd, XMenuItem *mi);		// help activate menu item

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont m_Font;
    CXBHelp m_Help;

	D3DXVECTOR3	m_vCameraPos;
	D3DXVECTOR3	m_vCameraRot;

	D3DLIGHT8 m_Light;

	DWORD m_dwBytesSentPerSecond;
	DWORD m_dwBytesReceivedPerSecond;

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
	m_vCameraPos = D3DXVECTOR3(0.0f, 10.0f, -15.0f);
	m_vCameraRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;
	D3DXVECTOR3 v;

    // Create a font
    if(FAILED(hr = m_Font.Create(m_pd3dDevice, "Font.xpr")))
        return XBAPPERR_MEDIANOTFOUND;

	// Initialize the help system
    if(FAILED(hr = m_Help.Create(m_pd3dDevice, "Gamepad.xpr")))
        return XBAPPERR_MEDIANOTFOUND;

	XMenu_SetFont(&m_Font);
	XTextBox_SetFont(&m_Font);

    // Set projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 640.0f/480.0f, 1.0f, 200.0f);
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	// init a directional light
	XBUtil_InitLight(m_Light, D3DLIGHT_DIRECTIONAL, 1.0f, 0.0f, 1.0f);
	m_pd3dDevice->SetLight(0, &m_Light);
	m_pd3dDevice->LightEnable(0, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x80808080);

	// init the 'choose player name' menu
	InitNameMenu();

	// init a menu
	g_MainMenu = XMenu_Init(320.0f, 120.0f, 5, 0, NULL);
	XMenu_AddItem(g_MainMenu, 0, L"SINGLE PLAYER", NULL);
	XMenu_AddItem(g_MainMenu, MITEM_ROUTINE, L"MULTIPLAYER", Net_Menu);
	XMenu_AddItem(g_MainMenu, MITEM_SEPARATOR, NULL, NULL);
	XMenu_AddItem(g_MainMenu, 0, L"OPTIONS", NULL);
	XMenu_AddItem(g_MainMenu, MITEM_ROUTINE, L"HELP", Help);

	// init a textbox
	XBox = XTextBox_Init(64, 480.0f-50.0f-(float)m_Font.GetFontHeight()*4.0f,
						640.0f-128.0f, 3, TBOX_POPUP);
	XBox->expiretime = 2000;		// keep messages up for 2 seconds

	// set base path to point to our data
	// all the _FNA commands use this
	XPath_SetBasePath(_T("d:\\media\\"));

	// arena
	g_pxbmArena = new CXBMesh;
	g_pxbmArena->Create(g_pd3dDevice, "models\\arena.xbg" );
	g_objArena.SetXBMesh(g_pxbmArena);
	g_objArena.SetRotation(-1.57f, 0.0f, 0.0f);

	// tanks
	srand(GetTickCount());
	v = D3DXVECTOR3(frand(80.0f)-40.0f, 0.0f, frand(80.0f)-40.0f);
	g_pTank[0] = new CXTank(&v, 0.0f);
	g_dwNumTanks = 1;

	// weapons
	Weapon_Init();

	// particle system
	XAPart = new CXParticleSystem(500, "textures\\bullet2.bmp");

	// networking
	Net_Init();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Clean up after ourselves.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Cleanup()
{
	DWORD i;

	XMenu_Delete(g_MainMenu);

	for(i=0; i<g_dwNumTanks; i++)
		delete g_pTank[0];

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	D3DXMATRIX m;
	static float fUpdateTime = 0.0f;
	static float fStatTime = 0.0f;
	static DWORD dwBytesSentBase = 0;
	static DWORD dwBytesReceivedBase = 0;
	float time;

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

	// update local tank
	g_pTank[0]->Update(&m_DefaultGamepad, m_fElapsedTime);

	// update remote tanks
	for(DWORD i=1; i<g_dwNumTanks; i++)
		g_pTank[i]->Update(NULL, m_fElapsedTime);

	// send update if we are in a game
	// also do bandwidth calcs every second
	if(XDPIsConnected())
	{
		time = XBUtil_Timer(TIMER_GETAPPTIME);
		if(time>fUpdateTime)
		{
			Net_SendUpdateTank(g_pTank[0]);
			fUpdateTime += 0.1f;			// 100msec
		}

		if(time>fStatTime)
		{
			m_dwBytesSentPerSecond = g_dwBytesSent - dwBytesSentBase;
			m_dwBytesReceivedPerSecond = g_dwBytesReceived - dwBytesReceivedBase;

			dwBytesSentBase = g_dwBytesSent;
			dwBytesReceivedBase = g_dwBytesReceived;

			fStatTime += 1.0f;
		}
	}

	// camerapos = tank pos
	m_vCameraPos = g_pTank[0]->m_pObj->m_vPosition;
	D3DXMatrixTranslation(&g_mView, -m_vCameraPos.x, -m_vCameraPos.y, -m_vCameraPos.z);

	// camera rotation = tank rotation
	D3DXMatrixRotationY(&m, -g_pTank[0]->m_pObj->m_vRotation.y);
	D3DXMatrixMultiply(&g_mView, &g_mView, &m);

	m_pd3dDevice->SetTransform(D3DTS_VIEW, &g_mView);

	// weapon update
	Weapon_Update(m_fElapsedTime);

	// particle update
	XAPart->Update(m_fElapsedTime);

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
	DWORD i;

    // clear the viewport
    m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff400000, 1.0f, 0L );

	// restore state that text clobbers
    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// render objects with modulate tex/diffuse
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	g_objArena.Render(0);				// render arena

	Weapon_Render();					// render weapons
	
	for(i=1; i<g_dwNumTanks; i++)		// render all tanks but local
		g_pTank[i]->m_pObj->Render(0);

	XAPart->Render();					// render particles

	g_pTank[0]->Hud(&m_Font);			// show tank hud

	// process the menu
	XMenu_Routine(g_dwMenuCommand);
	XTextBox_Display(XBox);

	// show game title
	m_Font.DrawText(64, 50, 0xffffffff, L"X-ARENA");
	m_Font.DrawText(640-64, 50, 0xffffff00, m_strFrameRate, XBFONT_RIGHT);

	// show our current status
	switch(g_dwConnectStatus)
	{
		case GAME_HOSTING:
			m_Font.DrawText(640-64, 70, 0xff00ff00, L"HOSTING", XBFONT_RIGHT);
			break;
		case GAME_ENUMERATING:
			m_Font.DrawText(640-64, 70, 0xffffff00, L"ENUMERATING", XBFONT_RIGHT);
			break;
		case GAME_CONNECTED:
			m_Font.DrawText(640-64, 70, 0xff00ff00, L"CONNECTED", XBFONT_RIGHT);
			break;
		case GAME_NOTCONNECTED:
			m_Font.DrawText(640-64, 70, 0xffff0000, L"NOT CONNECTED", XBFONT_RIGHT);
			break;
		case GAME_CONNECTING:
			m_Font.DrawText(640-64, 70, 0xffffff00, L"CONNECTING", XBFONT_RIGHT);
			break;
	}

	// show # of players & bandwidth usage
	if(XDPIsConnected())
	{
		WCHAR s[40];
		swprintf(s, L"%d PLAYERS", g_dwNumPlayers);
		m_Font.DrawText(640-64, 90, 0xffffffff, s, XBFONT_RIGHT);

		swprintf(s, L"SENT: %d, RECV: %d", m_dwBytesSentPerSecond, m_dwBytesReceivedPerSecond);
		m_Font.DrawText(640-64, 110, 0xffffffff, s, XBFONT_RIGHT);
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
//       Also assembles the host session name to be "playername - XArena"
//-----------------------------------------------------------------------------
DWORD GotName(DWORD cmd, XMenuItem *mi)
{
	WCHAR s[80];

	// set player name
	wcscpy(g_wszXDPPlayerName, mi->string);

	// set session name
	wcscpy(g_wszXDPSessionName, g_wszXDPPlayerName);
	wcscat(g_wszXDPSessionName, L" - XArena");

	// welcome our new player!
	swprintf(s, L"Welcome to X-ARENA %s", mi->string);
	XTextBox_AddItem(XBox, s);

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

