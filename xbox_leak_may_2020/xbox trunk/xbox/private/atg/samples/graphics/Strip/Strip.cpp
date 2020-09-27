//-----------------------------------------------------------------------------
// File: Strip.cpp
//
// Desc: Sample to show off tri-stripping performance results. This sample
//       creates a mesh, stripifies it, then displays several copies of it
//       along with performance data.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBPerf.h>
#include <XBStrip.h>
#include <XBUtil.h>
#include <XBHelp.h>
#include <XMenu.h>




//-----------------------------------------------------------------------------
// Help support
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_NormalHelpCallouts[] =
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate the model" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Zoom in/out" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_2, L"Bring up\noptions menu" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display\nhelp" },
};
#define MAX_NORMAL_HELP_CALLOUTS    4




//-----------------------------------------------------------------------------
// Global variables and definitions
//-----------------------------------------------------------------------------

// Constants definitions
#define COLOR_YELLOW                0xffffff00
#define COLOR_WHITE                 0xffffffff

#define MENUSELECT_DELETE           0
#define MENUSELECT_ADD              1
#define MENUSELECT_DISABLE          0
#define MENUSELECT_ENABLE           1

#define MENUCMD_DIRLIGHT            0x00000002
#define MENUCMD_POINTLIGHT          0x00000004
#define MENUCMD_SPOTLIGHT           0x00000008
#define MENUCMD_TEXTURES            0x00000020
#define MENUCMD_ZENABLE             0x00000040
#define MENUCMD_SPECULARENABLE      0x00000080
#define MENUCMD_LOCALVIEWER         0x00000100
#define MENUCMD_ANTIALIAS           0x00000200
#define MENUCMD_FILLMODE            0x00000400
#define MENUCMD_CULLMODE            0x00000800
#define MENUCMD_EXPENSIVEOP         0x00001000
#define MENUCMD_DISPLAYINFO         0x00002000


// Structure declarations
struct MODELVERT
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;
};


struct MODELDATA
{
    D3DXMATRIX* pMatrix;
    DWORD       dwNumVertices;
    MODELVERT*  pVertices;
    DWORD       dwNumIndices;
    WORD*       pIndices;
};


enum MESHTYPE { MESH_Original, MESH_Xbox, MESH_Max };


struct OPTIONS
{
    MESHTYPE    meshtype;
    UINT        dwNumTextures;

    BOOL        bZenable;
    BOOL        bSpecularenable;
    BOOL        bLocalviewer;
    BOOL        bAntiAlias;

    INT         nNumLights;
    INT         dwDirectionalLights;
    INT         dwSpotLights;
    INT         dwPointLights;

    BOOL        bRebuildVBs;
    BOOL        bLightsChanged;

    BOOL        bDisplayArgs;
    D3DCULL     cullmode;
    D3DFILLMODE FillMode;    // current fill mode
};

struct ROBOTSTATS
{
    DWORD       dwNumVertices;
    DWORD       dwCacheHits;
    DWORD       dwPagesCrossed;
    DWORD       dwDegenerateTris;
    
    DWORD       dwDrawPassesX;
    DWORD       dwDrawPassesY;


    double      fdAvgTriPerSec;
    double      fdAvgTriPerSec2;
    double      fdMaxTriPerSec;
    double      fdMinTriPerSec;
    DWORD       dwAvgCount;
    DWORD       dwTriCount;
    DWORD       dwIndCount;
    DWORD       dwTime;
    DWORD       dwFrames;
};

struct MESHINFO
{
    D3DPRIMITIVETYPE        dwPrimType;   // primitive type

    DWORD                   dwIndexCount; // index count
    WORD*                   pwIndices;    // index list
    LPDIRECT3DINDEXBUFFER8  pIndexBuffer; // dx8 index buffer

    DWORD                   dwNumVertices; // num verts
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer; // vbs

    DWORD                   dwPrimitiveCount;

    DWORD                   dwDegenerateTris;
    DWORD                   dwCacheHits;
    DWORD                   dwPagesCrossed;
};


// Function decls
VOID    ResetOptions();
DWORD   ToggleMeshMenuProc( DWORD cmd, XMenuItem* pmi );
DWORD   AddDeleteMenuProc( DWORD cmd, XMenuItem* pmi );
DWORD   CullModeMenuProc( DWORD cmd, XMenuItem* pmi );
DWORD   FillModeMenuProc( DWORD cmd, XMenuItem* pmi );
DWORD   ResetAllProc( DWORD cmd, XMenuItem* pmi );




// render 12 robots
static const int C_ROBOTS_X = 4;
static const int C_ROBOTS_Y = 3;




// Mesh data from MODELDATA.CPP
extern DWORD     g_cModelData;
extern MODELDATA g_ModelData[];

#define MAX_LIGHTS                  8

// Camera location
D3DXVECTOR3 g_vEye(10.0f, -20.0f, 9.0f);
D3DXVECTOR3 g_vAt(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 g_vUp(0.0f, 0.0f, 1.0f);

// Global options
OPTIONS     g_Options;




//-----------------------------------------------------------------------------
// Name: class CRobotX
// Desc: Main Robot class
//-----------------------------------------------------------------------------
class CRobotX
{
    D3DMATERIAL8           m_mat;          // material

    DWORD                  m_dwNumMeshes;  // count of VBs to draw
    MESHINFO*              m_pMeshes;      // our list of VBs

    LPDIRECT3DTEXTURE8     m_pTexture1;    // texture1
    LPDIRECT3DCUBETEXTURE8 m_pTexture2;    // texture2

    LPDIRECT3DDEVICE8      m_pd3dDevice;   // d3d device

public:
    DWORD                  m_dwFVF;        // Mesh FVF
    DWORD                  m_dwVertexSize; // Vertex size

    HRESULT Init( LPDIRECT3DDEVICE8 );
    HRESULT InitVertexBuffers();
    VOID    Release();

    HRESULT SetStates();
    HRESULT RestoreStates();
    HRESULT Render( ROBOTSTATS* pStats = NULL );

    CRobotX();
    ~CRobotX() { Release(); }
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
private:
    // current state of the world
    struct STRIPSTATE
    {
//      BOOL        bDisplayHelp;
//      BOOL        bDisplayStats;
//      BOOL        bDisplaySysInfo;
//      UINT        cPause;
    };

    BOOL            m_bDisplayHelp;
    BOOL            m_bDisplayStats;
    UINT            m_cPause;

    STRIPSTATE      m_State;

    // the robot object
    ROBOTSTATS      m_Stats;
    CRobotX         m_Robot;

    // Font for rendering stats and help
    FLOAT           m_fFontHeight;
    FLOAT           m_fSecsPerTick;

    CXBFont         m_Font;
    CXBHelp         m_Help;

    // Menu system
    DWORD           m_dwMenuCommand;
    XMenu*          m_pMainMenu;
    XMenu*          m_pMeshMenu;
    XMenu*          m_pAddDeleteMenu;
    XMenu*          m_pEnableDisableMenu;
    XMenu*          m_pCullMenu;
    XMenu*          m_pFillMenu;

    HRESULT InitLights();
    HRESULT InitFrameworkStuff();
    BOOL    HandleInput();
    VOID    DisplayInfo( const WCHAR* str );
    VOID    DisplayArgs();
    VOID    DisplayStartupInfo();
    VOID    DisplayCmdParams();   

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();
    CXBoxSample();
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Main app constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
{
    // Enable anti-aliasing and unlimited frame rates
    m_d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Initialize derived member variables
    ZeroMemory( &m_State, sizeof(m_State) );

    ZeroMemory(&m_Stats, sizeof(m_Stats));
    m_Stats.dwDrawPassesX   = C_ROBOTS_X;
    m_Stats.dwDrawPassesY   = C_ROBOTS_Y;
    m_Stats.dwAvgCount      = 0;
    m_Stats.fdAvgTriPerSec  = 0.0;
    m_Stats.fdAvgTriPerSec2 = 0.0;
    m_Stats.fdMaxTriPerSec  = 0.0;
    m_Stats.fdMinTriPerSec  = 1e99;

    m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    m_pMainMenu = NULL;
    m_pMeshMenu = NULL;
    m_pAddDeleteMenu = NULL;
    m_pEnableDisableMenu = NULL;
    m_pCullMenu = NULL;
    m_pFillMenu = NULL;

    ResetOptions();
    g_Options.bRebuildVBs = false;
    g_Options.bLightsChanged = false;

}




//-----------------------------------------------------------------------------
// Name: ResetOptions()
// Desc: Reset options
//-----------------------------------------------------------------------------
void ResetOptions()
{
    g_Options.dwNumTextures = 2;
    g_Options.dwDirectionalLights = 0;
    g_Options.dwSpotLights = 0;
    g_Options.dwPointLights = 1;

    g_Options.bZenable        = TRUE;
    g_Options.bSpecularenable = TRUE;
    g_Options.bLocalviewer    = TRUE;
    g_Options.bAntiAlias      = TRUE;
    g_Options.cullmode = D3DCULL_CCW;
    g_Options.FillMode = D3DFILL_SOLID;

    g_Options.meshtype = MESH_Xbox; // start with tri-strips
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize all dependencies and states
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;

    g_Options.bDisplayArgs = TRUE;
    m_bDisplayStats        = TRUE;
    m_bDisplayHelp         = FALSE;
    m_cPause               = 0;

    hr = InitFrameworkStuff();
    if( FAILED(hr) )
        return hr;
    
    // Disable antialiasing by default
    m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );

    // Display initial wait screen
    m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L);
    DisplayStartupInfo();
    m_pd3dDevice->Present(NULL, NULL, NULL, NULL);

    // initialize robot data
    hr = m_Robot.Init(m_pd3dDevice);
    if( FAILED(hr) )
        return hr;
    
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &g_vEye, &g_vAt, &g_vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI * 100.0f / 360.0f,
                                640.0f  / 480.0f, 1.0f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Initalize our lights
    InitLights();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitFrameworkStuff()
// Desc: Initialize framework objects
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitFrameworkStuff()
{
    HRESULT             hr;

    if( FAILED( hr = m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( hr = m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    FLOAT fFontWidth;
    m_Font.GetTextExtent( L"M", &fFontWidth, &m_fFontHeight, TRUE );

    XMenuItem   *pmi;
    XMenu_SetFont( &m_Font );

    // sub-menu for mesh selection
    m_pMeshMenu = XMenu_Init(320.0f, 100.0f, 2, 0, NULL);
    m_pMeshMenu->topcolor    = 0xffffffff;
    m_pMeshMenu->bottomcolor = 0xffffffff;
    m_pMeshMenu->itemcolor   = 0xff000000;
    m_pMeshMenu->seltopcolor = 0xffcedfad;
    m_pMeshMenu->selbotcolor = 0xff84b642;

    XMenu_AddItem( m_pMeshMenu, MITEM_ROUTINE, L"Xbox Mesh",
                   ToggleMeshMenuProc);
    XMenu_AddItem( m_pMeshMenu, MITEM_ROUTINE, L"Original Mesh",
                   ToggleMeshMenuProc );

    // sub-menu for add/delete options
    m_pAddDeleteMenu = XMenu_Init(320.0f, 100.0f, 2, 0, NULL);
    m_pAddDeleteMenu->topcolor = 0xffffffff;
    m_pAddDeleteMenu->bottomcolor = 0xffffffff;
    m_pAddDeleteMenu->itemcolor = 0xff000000;
    m_pAddDeleteMenu->seltopcolor = 0xffcedfad;
    m_pAddDeleteMenu->selbotcolor = 0xff84b642;

    pmi = XMenu_AddItem( m_pAddDeleteMenu, MITEM_ROUTINE, L"Add",
                         AddDeleteMenuProc );
    pmi->val2 = MENUSELECT_ADD;
    pmi = XMenu_AddItem( m_pAddDeleteMenu, MITEM_ROUTINE, L"Delete",
                         AddDeleteMenuProc);
    pmi->val2 = MENUSELECT_DELETE;

    // sub-menu for enable/disable options
    m_pEnableDisableMenu = XMenu_Init(320.0f, 100.0f, 2, 0, NULL);
    m_pEnableDisableMenu->topcolor = 0xffffffff;
    m_pEnableDisableMenu->bottomcolor = 0xffffffff;
    m_pEnableDisableMenu->itemcolor = 0xff000000;
    m_pEnableDisableMenu->seltopcolor = 0xffcedfad;
    m_pEnableDisableMenu->selbotcolor = 0xff84b642;

    pmi = XMenu_AddItem( m_pEnableDisableMenu, MITEM_ROUTINE, L"Enable",
                         AddDeleteMenuProc );
    pmi->val2 = MENUSELECT_ENABLE;
    pmi = XMenu_AddItem( m_pEnableDisableMenu, MITEM_ROUTINE, L"Disable",
                         AddDeleteMenuProc );
    pmi->val2 = MENUSELECT_DISABLE;

    // sub-menu for cull mode
    m_pCullMenu = XMenu_Init(320.0f, 100.0f, 3, 0, NULL);
    m_pCullMenu->topcolor = 0xffffffff;
    m_pCullMenu->bottomcolor = 0xffffffff;
    m_pCullMenu->itemcolor = 0xff000000;
    m_pCullMenu->seltopcolor = 0xffcedfad;
    m_pCullMenu->selbotcolor = 0xff84b642;

    pmi = XMenu_AddItem( m_pCullMenu, MITEM_ROUTINE, L"None", CullModeMenuProc );
    pmi->val1 = D3DCULL_NONE;
    pmi = XMenu_AddItem( m_pCullMenu, MITEM_ROUTINE, L"CW", CullModeMenuProc );
    pmi->val1 = D3DCULL_CW;
    pmi = XMenu_AddItem( m_pCullMenu, MITEM_ROUTINE, L"CCW", CullModeMenuProc );
    pmi->val1 = D3DCULL_CCW;

    // sub-menu for fill mode
    m_pFillMenu = XMenu_Init(320.0f, 100.0f, 3, 0, NULL);
    m_pFillMenu->topcolor = 0xffffffff;
    m_pFillMenu->bottomcolor = 0xffffffff;
    m_pFillMenu->itemcolor = 0xff000000;
    m_pFillMenu->seltopcolor = 0xffcedfad;
    m_pFillMenu->selbotcolor = 0xff84b642;

    pmi = XMenu_AddItem( m_pFillMenu, MITEM_ROUTINE, L"Solid", FillModeMenuProc );
    pmi->val1 = D3DFILL_SOLID;
    pmi = XMenu_AddItem( m_pFillMenu, MITEM_ROUTINE, L"WireFrame", FillModeMenuProc );
    pmi->val1 = D3DFILL_WIREFRAME;
    pmi = XMenu_AddItem( m_pFillMenu, MITEM_ROUTINE, L"Point", FillModeMenuProc );
    pmi->val1 = D3DFILL_POINT;

    // main menu
    m_pMainMenu = XMenu_Init(320.0f, 100.0f, 17, MENU_WRAP, NULL);
    m_pMainMenu->topcolor = 0xffffffff;
    m_pMainMenu->bottomcolor = 0xffffffff;
    m_pMainMenu->itemcolor = 0xff000000;
    m_pMainMenu->seltopcolor = 0xffcedfad;
    m_pMainMenu->selbotcolor = 0xff84b642;

    XMenu_AddItem( m_pMainMenu, 0, L"Change Mesh", m_pMeshMenu );            
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"Expensive Operations",
                         m_pEnableDisableMenu );
    pmi->val1 = MENUCMD_EXPENSIVEOP;
    XMenu_AddItem( m_pMainMenu, 0, L"Cull mode", m_pCullMenu );
    XMenu_AddItem( m_pMainMenu, 0, L"Fill mode", m_pFillMenu );

    XMenu_AddItem(m_pMainMenu, MITEM_SEPARATOR, NULL, NULL);
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"Directional Light",
                         m_pAddDeleteMenu );
    pmi->val1 = MENUCMD_DIRLIGHT;
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"Point Light", m_pAddDeleteMenu );
    pmi->val1 = MENUCMD_POINTLIGHT;
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"Spot Light", m_pAddDeleteMenu );
    pmi->val1 = MENUCMD_SPOTLIGHT;

    XMenu_AddItem(m_pMainMenu, MITEM_SEPARATOR, NULL, NULL);
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"Texture", m_pAddDeleteMenu );
    pmi->val1 = MENUCMD_TEXTURES;
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"Z-enable", m_pEnableDisableMenu );
    pmi->val1 = MENUCMD_ZENABLE;
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"Specular", m_pEnableDisableMenu );
    pmi->val1 = MENUCMD_SPECULARENABLE;
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"LocalViewer", m_pEnableDisableMenu );
    pmi->val1 = MENUCMD_LOCALVIEWER;
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"AntiAliasing", m_pEnableDisableMenu );
    pmi->val1 = MENUCMD_ANTIALIAS;

    XMenu_AddItem(m_pMainMenu, MITEM_SEPARATOR, NULL, NULL);
    pmi = XMenu_AddItem( m_pMainMenu, 0, L"Toggle Display Info",
                         m_pEnableDisableMenu );
    pmi->val1 = MENUCMD_DISPLAYINFO;
    pmi = XMenu_AddItem( m_pMainMenu, MITEM_ROUTINE, L"Reset Parameters",
                         ResetAllProc );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ToggleMeshMenuProc()
// Desc: Toggles the mesh to use
//-----------------------------------------------------------------------------
DWORD ToggleMeshMenuProc(DWORD cmd, XMenuItem *pmi)
{

    if (g_Options.meshtype == MESH_Original)
    {
        g_Options.meshtype = MESH_Xbox;
        g_Options.bRebuildVBs = true;
    }
    else
    {
        g_Options.meshtype = MESH_Original;
        g_Options.bRebuildVBs = true;
    }

    return MROUTINE_DIE;
}




//-----------------------------------------------------------------------------
// Name: CullModeMenuProc()
// Desc: Changes the cull mode (none, cw, ccw)
//-----------------------------------------------------------------------------
DWORD CullModeMenuProc(DWORD cmd, XMenuItem *pmi)
{
    g_Options.cullmode = D3DCULL(pmi->val1);
    
    return MROUTINE_DIE;
}




//-----------------------------------------------------------------------------
// Name: FillModeMenuProc()
// Desc: Changes the fill mode (solid, wireframe, point)
//-----------------------------------------------------------------------------
DWORD FillModeMenuProc(DWORD cmd, XMenuItem *pmi)
{
    g_Options.FillMode = D3DFILLMODE(pmi->val1);

    return MROUTINE_DIE;
}




//-----------------------------------------------------------------------------
// Name: AddDeleteMenuProc()
// Desc: Menu callback for submenus add/delete and enable/disable.
//-----------------------------------------------------------------------------
DWORD AddDeleteMenuProc(DWORD cmd, XMenuItem *pmi)
{
    _xmenu           *pMenuParent = pmi->menu->parent;
    DWORD            val1Parent;
    
    // get the val1 of the parent menu
    val1Parent = pMenuParent->items[pMenuParent->curitem].val1;

    // pmi->val2 holds the action (add/delete)
    switch (val1Parent)
    {
        case MENUCMD_DIRLIGHT:
            if (pmi->val2)
            {
                g_Options.dwDirectionalLights = (g_Options.dwDirectionalLights + 
                        MAX_LIGHTS + 1) % MAX_LIGHTS;
            }
            else
            {
                g_Options.dwDirectionalLights = (g_Options.dwDirectionalLights +
                        MAX_LIGHTS - 1) % MAX_LIGHTS;
            }
            g_Options.bLightsChanged = true;
            break;

        case MENUCMD_POINTLIGHT:
            if (pmi->val2)
            {
                g_Options.dwPointLights = (g_Options.dwPointLights +
                        MAX_LIGHTS + 1) % MAX_LIGHTS;
            }
            else
            {
                g_Options.dwPointLights = (g_Options.dwPointLights +
                        MAX_LIGHTS - 1) % MAX_LIGHTS;
            }
            g_Options.bLightsChanged = true;
            break;

        case MENUCMD_SPOTLIGHT:
            if (pmi->val2)
            {
                g_Options.dwSpotLights = (g_Options.dwSpotLights + MAX_LIGHTS +
                        1) % MAX_LIGHTS;
            }
            else
            {
                g_Options.dwSpotLights = (g_Options.dwSpotLights + MAX_LIGHTS -
                        1) % MAX_LIGHTS;
            }
            g_Options.bLightsChanged = true;
            break;


        case MENUCMD_TEXTURES:
            if (pmi->val2)
            {
                g_Options.dwNumTextures++;

                if(g_Options.dwNumTextures > 2)
                    g_Options.dwNumTextures = 2;
            }
            else
            {
                g_Options.dwNumTextures--;

                if(g_Options.dwNumTextures < 0)
                    g_Options.dwNumTextures = 0;
            }
            break;

        case MENUCMD_ZENABLE:
            g_Options.bZenable = (pmi->val2) ? TRUE : FALSE;
            break;

        case MENUCMD_SPECULARENABLE:
            g_Options.bSpecularenable = (pmi->val2) ? TRUE : FALSE;
            break;

        case MENUCMD_LOCALVIEWER:
            g_Options.bLocalviewer = (pmi->val2) ? TRUE : FALSE;
            break;

        case MENUCMD_ANTIALIAS:
            g_Options.bAntiAlias = (pmi->val2) ? TRUE : FALSE;
            break;

        case MENUCMD_EXPENSIVEOP:   // toggles expensive op on/off
            if (pmi->val2)
            {
                g_Options.dwDirectionalLights = 0;
                g_Options.dwSpotLights    = 0;
                g_Options.dwPointLights   = 1;
                g_Options.dwNumTextures   = 2;
                g_Options.bSpecularenable = TRUE;
                g_Options.bLocalviewer    = TRUE;
                g_Options.bAntiAlias      = TRUE;
            }
            else
            {
                g_Options.dwDirectionalLights = 1;
                g_Options.dwSpotLights    = 0;
                g_Options.dwPointLights   = 0;
                g_Options.dwNumTextures   = 0;
                g_Options.bSpecularenable = FALSE;
                g_Options.bLocalviewer    = FALSE;
                g_Options.bAntiAlias      = FALSE;
            }
            g_Options.bRebuildVBs = true;
            break;

        case MENUCMD_DISPLAYINFO:
            g_Options.bDisplayArgs = pmi->val2 ? true : false;
            break;

    }

    return MROUTINE_DIE;
}




//-----------------------------------------------------------------------------
// Name: ResetAllProc()
// Desc: Menu callback to reset all options to the original states.
//-----------------------------------------------------------------------------
DWORD ResetAllProc(DWORD cmd, XMenuItem *pmi)
{
    ResetOptions();

    // rebuild everything
    g_Options.bRebuildVBs = true;
    g_Options.bLightsChanged = true;
    return MROUTINE_DIE;
}




//-----------------------------------------------------------------------------
// Name: InitLights()
// Desc: Initialize all lights
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitLights()
{
    // disable all our existing lights
    while( g_Options.nNumLights-- )
        m_pd3dDevice->LightEnable( g_Options.nNumLights, FALSE );

    // get a count of the new dudes
    g_Options.nNumLights = g_Options.dwDirectionalLights +
                           g_Options.dwPointLights +
                           g_Options.dwSpotLights;

    // setup lights
    float fIntensity = 1.0f / ( g_Options.nNumLights );

    DWORD dwIndex = 0;
    for (int nT = 0; nT < g_Options.dwDirectionalLights; nT++)
    {
        D3DLIGHT8 light;

        ZeroMemory(&light, sizeof(light));
        light.Type        =  D3DLIGHT_DIRECTIONAL;
        light.Diffuse.r   =  1.0f * fIntensity;
        light.Diffuse.g   =  1.0f * fIntensity;
        light.Diffuse.b   =  1.0f * fIntensity;
        light.Direction.x =  0.2f;
        light.Direction.y =  0.3f;
        light.Direction.z = -0.8f;
        m_pd3dDevice->SetLight(dwIndex, &light);
        m_pd3dDevice->LightEnable(dwIndex, TRUE);

        dwIndex++;
    }
    for (nT = 0; nT < g_Options.dwPointLights; nT++)
    {
        D3DLIGHT8 light;

        ZeroMemory(&light, sizeof(light));
        light.Type         =  D3DLIGHT_POINT;
        light.Diffuse.r    =  1.0f * fIntensity;
        light.Diffuse.g    =  1.0f * fIntensity;
        light.Diffuse.b    =  1.0f * fIntensity;
        light.Specular.r   =  0.5f * fIntensity;
        light.Specular.g   =  0.5f * fIntensity;
        light.Specular.b   =  0.5f * fIntensity;
        light.Position.x   =  0.0f;
        light.Position.y   =  -10.0f;
        light.Position.z   =  40.0f;
        light.Range        = 10000.0f;
        light.Attenuation0 = 0.0f;
        light.Attenuation1 = 0.02f;
        light.Attenuation2 = 0.0f;
        m_pd3dDevice->SetLight(dwIndex, &light);
        m_pd3dDevice->LightEnable(dwIndex, TRUE);

        dwIndex++;
    }
    for (nT = 0; nT < g_Options.dwSpotLights; nT++)
    {
        D3DLIGHT8 light;

        ZeroMemory(&light, sizeof(light));
        light.Type         =  D3DLIGHT_SPOT;
        light.Diffuse.r    =  1.0f * fIntensity;
        light.Diffuse.g    =  1.0f * fIntensity;
        light.Diffuse.b    =  1.0f * fIntensity;
        light.Position.x   = -1.0f;
        light.Position.y   = -5.0f;
        light.Position.z   =  6.0f;
        light.Direction.x  =  0.1f;
        light.Direction.y  =  0.5f;
        light.Direction.z  = -0.6f;
        light.Range        = 999.0f;
        light.Theta        = 0.1f;
        light.Phi          = 0.5f;
        light.Falloff      = 1.0f;
        light.Attenuation0 = 1.0f;
        m_pd3dDevice->SetLight(dwIndex, &light);
        m_pd3dDevice->LightEnable(dwIndex, TRUE);

        dwIndex++;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    m_dwMenuCommand = XMenu_GetCommand(&m_DefaultGamepad);
    if (m_dwMenuCommand == MENU_ACTIVATE)
        XMenu_Activate(m_pMainMenu);

    if (XMenu_IsActive())
        return S_OK;

    HandleInput();

    if (g_Options.bRebuildVBs)
    {
        m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);
        DisplayInfo( L"Recalculating...." );
        m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        m_Robot.InitVertexBuffers();
        g_Options.bRebuildVBs = false;

        // Clear the averages
        m_Stats.dwAvgCount      = 0;
        m_Stats.fdAvgTriPerSec  = 0.0;
        m_Stats.fdAvgTriPerSec2 = 0.0;
        m_Stats.fdMaxTriPerSec  = 0.0;
        m_Stats.fdMinTriPerSec  = 1e99;
    }

    if (g_Options.bLightsChanged)
    {
        InitLights();
        g_Options.bLightsChanged = false;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: HandleInput()
// Desc: Handle input from the gamepad. A return of true means new options
//       require re-initializing vertex/index buffers
//-----------------------------------------------------------------------------
BOOL CXBoxSample::HandleInput()
{
    static short        sCullMode = 0;
    float               fX = 0.0f;
    float               fLY = 0.0f;
    float               fRY = 0.0f;

    if(m_DefaultGamepad.fX1)
    {
        fX = m_DefaultGamepad.fX1 * 3.0f;
    }
    if(m_DefaultGamepad.fY1)
    {
        fLY = m_DefaultGamepad.fY1 * 3.0f;
    }
    if(m_DefaultGamepad.fY2)
    {
        fRY = m_DefaultGamepad.fY2 * 3.0f;
    }

    if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK) 
    {
        // display the help screen
        static BOOL     bPrevDisplayArgs = g_Options.bDisplayArgs;

        g_Options.bDisplayArgs = false;
        m_bDisplayHelp = !m_bDisplayHelp;

        if (!m_bDisplayHelp)
            g_Options.bDisplayArgs = bPrevDisplayArgs;
    }


    if (fX || fLY || fRY)
    {
        D3DXMATRIX matView;

        // in / out
        float fLength = D3DXVec3LengthSq(&g_vEye);
        D3DXVECTOR3 vEyeNorm = g_vEye / fLength;
        g_vEye += vEyeNorm * fRY * .75f;

        // up / down
        g_vEye.z += fLY;

        // left / right
        float fCos = cosf(fX * D3DX_PI / 180);
        float fSin = sinf(fX * D3DX_PI / 180);
        g_vEye.x = g_vEye.x * fCos - g_vEye.y * fSin;
        g_vEye.y = g_vEye.x * fSin + g_vEye.y * fCos;

        D3DXMatrixLookAtLH(&matView, &g_vEye, &g_vAt, &g_vUp);
        m_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    static WCHAR s_strStats[MAX_PATH] = L"Running...";
    DWORD       dwFrames         = 0;
    DWORD       dwPrimitiveCount = 0;
    DWORD       dwTriCount       = 0;
    DWORD       dwIndCount       = 0;
    DWORD       dwNumVertices    = 0;
    DWORD       dwCacheHits      = 0;
    DWORD       dwPagesCrossed   = 0;
    DWORD       dwDegenerateTris = 0;

    DWORD       dwStart = GetTickCount();
    DWORD       dwNow   = dwStart;
    DWORD       dwNext  = dwStart + 2000;

    // Clear the viewport
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000,
                         1.0f, 0L );

    // Render the robot
    m_Robot.SetStates();
    
    // Draw robot a bunch of times
    for( DWORD dwX = 0; dwX < m_Stats.dwDrawPassesX; dwX++ )
    {
        for( DWORD dwY = 0; dwY < m_Stats.dwDrawPassesY; dwY++ )
        {
            D3DXMATRIX matWorld;
            D3DXMatrixTranslation( &matWorld, (dwX - m_Stats.dwDrawPassesX / 2.0f) * 3.5f, 
                                              0.0f, 
                                              (dwY - m_Stats.dwDrawPassesY / 2.0f) * 4.0f );
            m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

            m_Robot.Render( &m_Stats );
        }
    }

    m_Robot.RestoreStates();

    dwFrames++;
    dwNow = GetTickCount();

    DisplayInfo( s_strStats );

    XMenu_Routine(m_dwMenuCommand); 

    if (m_cPause == 1)
        return S_OK;

    FLOAT fElapsedTime = 0.001f * (dwNow-dwStart);
    FLOAT tps = 1e-6f * (m_Stats.dwTriCount)   / fElapsedTime;
    FLOAT sps = 1e-6f * (m_Stats.dwIndCount)   / fElapsedTime;
    FLOAT ips = 1e-6f * (m_Stats.dwIndCount*2) / fElapsedTime;
    swprintf( s_strStats, L"%6.1f MTri/s,   %6.1f MVerts/s", tps, sps );

    m_Stats.dwAvgCount++;
    m_Stats.fdAvgTriPerSec  += tps;
    m_Stats.fdAvgTriPerSec2 += 1e-6f * (m_Stats.dwTriCount - m_Stats.dwDegenerateTris) / fElapsedTime;
    m_Stats.fdMaxTriPerSec   = max(tps, m_Stats.fdMaxTriPerSec);
    m_Stats.fdMinTriPerSec   = min(tps, m_Stats.fdMinTriPerSec);
    m_Stats.dwTime           = dwNow - dwStart;
    m_Stats.dwFrames         = dwFrames;

    m_Stats.dwTriCount       = dwTriCount;
    m_Stats.dwIndCount       = dwIndCount;
    m_Stats.dwNumVertices    = dwNumVertices;
    m_Stats.dwCacheHits      = dwCacheHits;
    m_Stats.dwPagesCrossed   = dwPagesCrossed;
    m_Stats.dwDegenerateTris = dwDegenerateTris;

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DisplayStartupInfo()
// Desc: Display the various system info.
//-----------------------------------------------------------------------------
VOID CXBoxSample::DisplayStartupInfo()
{
    WCHAR        str[128];
    static DWORD s_dwCpuSpeed = (DWORD)XBPerf_GetCpuSpeed();

    m_Font.DrawText(  64,  50,  COLOR_WHITE, L"Xbox Tri-stripper demo" );
    m_Font.DrawText(  64,  75,   0xff00ffff, L"Calculating data. Please wait..." );

    swprintf( str, L"%d MHz", s_dwCpuSpeed);
    m_Font.DrawText(  64, 125,  COLOR_WHITE, L"CPU speed:" );
    m_Font.DrawText( 200, 125, COLOR_YELLOW, str );

    swprintf( str, L"%d", m_d3dpp.BackBufferWidth );
    m_Font.DrawText(  64, 150,  COLOR_WHITE, L"Width:" );
    m_Font.DrawText( 200, 150, COLOR_YELLOW, str );

    swprintf( str, L"%d", m_d3dpp.BackBufferHeight );
    m_Font.DrawText(  64, 175,  COLOR_WHITE, L"Height:" );
    m_Font.DrawText( 200, 175, COLOR_YELLOW, str );
}




//-----------------------------------------------------------------------------
// Name: DisplayInfo()
// Desc: Display the various info.
//-----------------------------------------------------------------------------
VOID CXBoxSample::DisplayInfo( const WCHAR* str )
{

    // Update status
    if( m_bDisplayStats )
        m_Font.DrawText( 64.0f, 75.0f, 0xff00ffff, str );

    // Display help/args
    if( g_Options.bDisplayArgs )
        DisplayArgs();

    if( m_bDisplayHelp )
        m_Help.Render( &m_Font, g_NormalHelpCallouts, MAX_NORMAL_HELP_CALLOUTS );
    else
    {
        // Draw title and framerate
        m_Font.DrawText(  64, 50, 0xffffffff, L"Strip" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
    }
}




//-----------------------------------------------------------------------------
// Name: DisplayArgs()
// Desc: Display the various stats and options.
//-----------------------------------------------------------------------------
void CXBoxSample::DisplayArgs()
{
    WCHAR str[128];
    float fX[4]   = { 64.0f, 250.0f, 365.0f, 500.0f };
    float fY      = 125.0f;
    float fHeight = m_fFontHeight;

    // Draw left-hand column
    fY      = 125.0f;

    DWORD dwRobotsDrawn = max(1, ( m_Stats.dwFrames * m_Stats.dwDrawPassesX * m_Stats.dwDrawPassesY ) );

    swprintf( str, L"%d", m_Stats.dwTriCount );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Num triangles:" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d", m_Stats.dwDegenerateTris );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Num degenerate:" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d", m_Stats.dwNumVertices );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Num vertices:" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d", m_Stats.dwIndCount );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Num indices:" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%.3f", m_Stats.fdMaxTriPerSec );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Max tri/s" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%.3f", m_Stats.fdMinTriPerSec );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Min tri/s" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%.3f", m_Stats.fdAvgTriPerSec / m_Stats.dwAvgCount );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Avg tri/s" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%.3f", m_Stats.fdAvgTriPerSec2 / m_Stats.dwAvgCount );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Real avg tri/s:" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d", m_Stats.dwCacheHits );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"GPU cache hits:" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    DWORD vbpages = (m_Stats.dwNumVertices + m_Stats.dwNumVertices * m_Robot.m_dwVertexSize) / (1024 * 4);
    swprintf( str, L"%d", vbpages );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"VB pages:" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d", m_Stats.dwPagesCrossed );
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Pages crossed:" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d ms", m_Stats.dwTime);
    m_Font.DrawText( fX[0], fY,  COLOR_WHITE, L"Time/frame:" );
    m_Font.DrawText( fX[1], fY, COLOR_YELLOW, str );
    fY += fHeight;

    // Draw right-hand column
    fY = 125.0f;
    
    static const WCHAR *rgMeshType[MESH_Max] = { L"Original", L"Stripped" };
    swprintf( str, L"%s", rgMeshType[g_Options.meshtype] );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"Mesh:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;

    static const WCHAR *rgszCullMode[] = { L"None", L"CCW", L"CW" };
    swprintf( str, L"%s", rgszCullMode[(g_Options.cullmode == 0) ? 0 : ((g_Options.cullmode & 0xf) + 1)] );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"Cull mode:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d", g_Options.dwNumTextures );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"Tex (2 max):" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;

    static const WCHAR *rgszFillMode[] = { L"Point", L"Wireframe", L"Solid" };
    swprintf( str, L"%s", rgszFillMode[g_Options.FillMode & 0xf] );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"Fill mode:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d", g_Options.dwSpotLights );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"Spotlights:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%s", g_Options.bZenable ? L"Yes" : L"No" );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"Z-enable:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d", g_Options.dwDirectionalLights );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"DirLights:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%s", g_Options.bSpecularenable ? L"Yes" : L"No" );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"Specular:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%d", g_Options.dwPointLights );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"Pointlights:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;

    swprintf( str, L"%s", g_Options.bLocalviewer ? L"Yes" : L"No" );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"LocalViewer:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;
    
    swprintf( str, L"%s", g_Options.bAntiAlias ? L"Yes" : L"No" );
    m_Font.DrawText( fX[2], fY,  COLOR_WHITE, L"AntiAliasing:" );
    m_Font.DrawText( fX[3], fY, COLOR_YELLOW, str );
    fY += fHeight;
}




//-----------------------------------------------------------------------------
// Name: CRobotX()
// Desc: CRobotX constructor
//-----------------------------------------------------------------------------
CRobotX::CRobotX()
{
    m_dwNumMeshes = 0;
    m_pMeshes     = NULL;

    m_pTexture1  = NULL;
    m_pTexture2  = NULL;
    m_pd3dDevice = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CRobotX()
// Desc: CRobotX destructor
//-----------------------------------------------------------------------------
void CRobotX::Release()
{
    for( DWORD i = 0; i < m_dwNumMeshes; i++ )
    {
        SAFE_RELEASE( m_pMeshes[i].pIndexBuffer );
        SAFE_RELEASE( m_pMeshes[i].pVertexBuffer );
    }

    SAFE_DELETE_ARRAY( m_pMeshes );
    m_dwNumMeshes = 0;

    SAFE_RELEASE( m_pTexture1 );
    SAFE_RELEASE( m_pTexture2 );
}




//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize robot dependencies
//-----------------------------------------------------------------------------
HRESULT CRobotX::Init( LPDIRECT3DDEVICE8 pd3dDevice )
{
    HRESULT     hr;

    if( pd3dDevice == NULL )
        return E_INVALIDARG;

    m_pd3dDevice = pd3dDevice;

    // Create our vertex buffers and indices
    InitVertexBuffers();

    // Set material
    ZeroMemory( &m_mat, sizeof(D3DMATERIAL8) );
    m_mat.Diffuse.r  =  1.0f;
    m_mat.Diffuse.g  =  1.0f;
    m_mat.Diffuse.b  =  1.0f;
    m_mat.Diffuse.a  =  1.0f;
    m_mat.Ambient.r  =  1.0f;
    m_mat.Ambient.g  =  1.0f;
    m_mat.Ambient.b  =  1.0f;
    m_mat.Ambient.a  =  1.0f;
    m_mat.Specular.r =  0.8f;
    m_mat.Specular.g =  0.8f;
    m_mat.Specular.b =  0.8f;
    m_mat.Specular.a =  0.4f;
    m_mat.Power      = 16.0f;

    // Load the texture
    hr = D3DXCreateTextureFromFileA( pd3dDevice, "D:\\Media\\Textures\\Strip_T.dds", 
                                     &m_pTexture1 );
    if (FAILED(hr))
        return hr;

    // Load the environment map textures
    hr = D3DXCreateCubeTextureFromFileA( pd3dDevice, "D:\\Media\\Textures\\Strip_C.dds", 
                                         &m_pTexture2 );
    if (FAILED(hr))
        return hr;

    OUTPUT_DEBUG_STRING( "Strip: Successfull CRobotX::Init()\n" );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the robot mesh 12 times and calc stats
//-----------------------------------------------------------------------------
HRESULT CRobotX::Render( ROBOTSTATS* pStats )
{
    D3DXMATRIX matWorld;
    m_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
    
    // Render each part
    for( DWORD iVB = 0; iVB < m_dwNumMeshes; iVB++ )
    {
        // Set our world transform
        D3DXMATRIX matObject(*g_ModelData[iVB].pMatrix);
        D3DXMatrixMultiply( &matObject, &matObject, &matWorld );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matObject );

        // Draw robot
        m_pd3dDevice->SetVertexShader( m_dwFVF );
        m_pd3dDevice->SetStreamSource( 0, m_pMeshes[iVB].pVertexBuffer, m_dwVertexSize );
        m_pd3dDevice->SetIndices( m_pMeshes[iVB].pIndexBuffer, 0 );
        m_pd3dDevice->DrawIndexedPrimitive( m_pMeshes[iVB].dwPrimType, 
                                            0, m_pMeshes[iVB].dwNumVertices, 
                                            0, m_pMeshes[iVB].dwPrimitiveCount );

        // Record stats
        pStats->dwTriCount       += m_pMeshes[iVB].dwPrimitiveCount;
        pStats->dwIndCount       += m_pMeshes[iVB].dwIndexCount;
        pStats->dwNumVertices    += m_pMeshes[iVB].dwNumVertices;
        pStats->dwCacheHits      += m_pMeshes[iVB].dwCacheHits;
        pStats->dwPagesCrossed   += m_pMeshes[iVB].dwPagesCrossed;
        pStats->dwDegenerateTris += m_pMeshes[iVB].dwDegenerateTris;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetStates()
// Desc: Set states that will be used to render the scene
//-----------------------------------------------------------------------------
HRESULT CRobotX::SetStates()
{
    m_pd3dDevice->SetMaterial( &m_mat );

    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_COLORVALUE(.5, .5, .5, 1.0f));
    m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, g_Options.bZenable);
    m_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, g_Options.bSpecularenable);
    m_pd3dDevice->SetRenderState(D3DRS_LOCALVIEWER, g_Options.bLocalviewer);
    m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, g_Options.bAntiAlias);

    for(DWORD dwStage = 0; dwStage < 2; dwStage++)
    {
        m_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_MAXANISOTROPY, 3);
        m_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC);
        m_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC);
        m_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    }

    m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, g_Options.cullmode);
    m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, g_Options.FillMode);

    if(g_Options.dwNumTextures)
    {
        m_pd3dDevice->SetTexture(0, m_pTexture1);

        // Modulate texture 0 with diffuse
        m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

        if(g_Options.dwNumTextures > 1)
        {
            // transform incoming camera space reflection vectors to world space
            D3DXMATRIX matTex;

            m_pd3dDevice->GetTransform(D3DTS_VIEW, &matTex);
            matTex.m[3][0] = 0.0f;
            matTex.m[3][1] = 0.0f;
            matTex.m[3][2] = 0.0f;

            D3DXMatrixInverse(&matTex, NULL, &matTex);
            m_pd3dDevice->SetTransform(D3DTS_TEXTURE1, &matTex);

            m_pd3dDevice->SetTexture(1, m_pTexture2);

            // Modulate the color of the second argument, using the alpha of the first argument;
            // then add the result to arg one.
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATEALPHA_ADDCOLOR);
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

            // Generate camera-space reflection vectors as tex coords
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,
                D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 1);

            // Setup the texture transform pipeline for 3d tex coords
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS,
                D3DTTFF_COUNT3);

            m_pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
        }
        else
        {
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

            D3DXMATRIX matIdentity;
            D3DXMatrixIdentity(&matIdentity);
            m_pd3dDevice->SetTransform(D3DTS_TEXTURE1, &matIdentity);

            m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
        }
    }
    else
    {
        m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
        m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreStates()
// Desc: Restore all used states
//-----------------------------------------------------------------------------
HRESULT CRobotX::RestoreStates()
{
    if(g_Options.dwNumTextures)
    {
        m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
        m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        if(g_Options.dwNumTextures > 1)
        {
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

            D3DXMATRIX matIdentity;
            D3DXMatrixIdentity(&matIdentity);
            m_pd3dDevice->SetTransform(D3DTS_TEXTURE1, &matIdentity);

            m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
            m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
        }
    }

    m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    for(DWORD dwStage = 0; dwStage < 2; dwStage++)
    {
        m_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_MAXANISOTROPY, 1);
        m_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_MINFILTER, D3DTEXF_POINT);
        m_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_MAGFILTER, D3DTEXF_POINT);
        m_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    }

    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,          0x00000000 );
    m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE);
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          D3DZB_TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LOCALVIEWER,      TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitVertexBuffers()
// Desc: Create vertex and index buffers based on the mesh type being used
//-----------------------------------------------------------------------------
HRESULT CRobotX::InitVertexBuffers()
{
    OUTPUT_DEBUG_STRING( "Strip: Begin initializing vertex buffers\n" );

    // Release any previously allocated meshes
    for( DWORD i = 0; i < m_dwNumMeshes; i++ )
    {
        SAFE_RELEASE( m_pMeshes[i].pIndexBuffer );
        SAFE_RELEASE( m_pMeshes[i].pVertexBuffer );
    }
    SAFE_DELETE_ARRAY( m_pMeshes );
    m_dwNumMeshes = 0;

    // FVF for creating vertex buffers
    m_dwFVF        = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
    m_dwVertexSize = D3DXGetFVFVertexSize(m_dwFVF);

    // Initialize new mesh array
    m_dwNumMeshes = g_cModelData;
    m_pMeshes     = new MESHINFO[m_dwNumMeshes];
    ZeroMemory( m_pMeshes, m_dwNumMeshes * sizeof(MESHINFO) );

    for( i = 0; i < m_dwNumMeshes; i++ )
    {
        MODELDATA* pModelData = &g_ModelData[i];
        MESHINFO*  pMesh      = &m_pMeshes[i];

        pMesh->dwNumVertices = pModelData->dwNumVertices;
        pMesh->dwIndexCount  = pModelData->dwNumIndices;
        pMesh->pwIndices     = pModelData->pIndices;

        if( g_Options.meshtype == MESH_Xbox )
        {
            DWORD dwNumVertices = pMesh->dwNumVertices;
            DWORD dwStrippedIndexCount;   // Tristrip count
            WORD* pwStrippedIndices;      // Tristrip indices
            WORD* pwVertexPermutation;    // Array for sorting
            
            // Run the tri-list through our tri-stripper
            Stripify( pModelData->dwNumIndices / 3, pModelData->pIndices,
                      &dwStrippedIndexCount, &pwStrippedIndices );

            // Sort the vertices...
            ComputeVertexPermutation( dwStrippedIndexCount, pwStrippedIndices,
                                      dwNumVertices, &pwVertexPermutation );

            // Create a vertex buffer
            m_pd3dDevice->CreateVertexBuffer( dwNumVertices * m_dwVertexSize, 
                                              D3DUSAGE_WRITEONLY, m_dwFVF, 
                                              D3DPOOL_DEFAULT, &pMesh->pVertexBuffer );

            // Lock and fill the vertex buffer, remapping vertices through the
            // vertex permutation array.
            MODELVERT* pVertices;
            pMesh->pVertexBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 );
            for( DWORD i = 0; i < dwNumVertices; i++ )
                pVertices[i] = pModelData->pVertices[pwVertexPermutation[i]];
            pMesh->pVertexBuffer->Unlock();

            // Free the array allocated by the ComputeVertexPermutation() call
            SAFE_DELETE_ARRAY( pwVertexPermutation );

            pMesh->dwPrimType       = D3DPT_TRIANGLESTRIP;
            pMesh->dwPrimitiveCount = dwStrippedIndexCount - 2;

            // Create an index buffer for using DrawIndexedPrimitive.
            m_pd3dDevice->CreateIndexBuffer( dwStrippedIndexCount * sizeof(WORD),
                                             D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                             D3DPOOL_DEFAULT, &pMesh->pIndexBuffer );

            // Lock and fill the index buffer
            WORD* pIndices;
            pMesh->pIndexBuffer->Lock( 0, 0, (BYTE**)&pIndices, 0 );
            memcpy( pIndices, pwStrippedIndices, dwStrippedIndexCount * sizeof(WORD) );
            pMesh->pIndexBuffer->Unlock();

            // Free the array allocated by the Stripify() call
            SAFE_DELETE_ARRAY( pwStrippedIndices );
        }
        else // original mesh
        {
            // Create a vertex buffers
            m_pd3dDevice->CreateVertexBuffer( pMesh->dwNumVertices * m_dwVertexSize, 
                                              D3DUSAGE_WRITEONLY, m_dwFVF, 
                                              D3DPOOL_DEFAULT, &pMesh->pVertexBuffer );

            // Lock and fill the vertex buffer
            MODELVERT* pVertices;
            pMesh->pVertexBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 );
            memcpy( pVertices, pModelData->pVertices, pMesh->dwNumVertices * m_dwVertexSize );
            pMesh->pVertexBuffer->Unlock();

            pMesh->dwPrimType       = D3DPT_TRIANGLELIST;
            pMesh->dwPrimitiveCount = pModelData->dwNumIndices / 3;

            // Create an index buffer for using DrawIndexedPrimitive.
            m_pd3dDevice->CreateIndexBuffer( pMesh->dwIndexCount * sizeof(WORD),
                                             D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                             D3DPOOL_DEFAULT, &pMesh->pIndexBuffer );

            // Lock and fill the index buffer
            WORD* pIndices;
            pMesh->pIndexBuffer->Lock( 0, 0, (BYTE**)&pIndices, 0 );
            memcpy( pIndices, pModelData->pIndices, pMesh->dwIndexCount*sizeof(WORD) );
            pMesh->pIndexBuffer->Unlock();
        }

        // Figure out how many degenerate triangles and cache hits we've got
        XBPerf_CalcCacheHits( pMesh->dwPrimType, m_dwVertexSize,
                              pMesh->pwIndices, pMesh->dwIndexCount,
                              &pMesh->dwDegenerateTris,
                              &pMesh->dwCacheHits,
                              &pMesh->dwPagesCrossed );
    }

    OUTPUT_DEBUG_STRING( "Strip: Finished initializing vertex buffers\n" );
    return S_OK;
}




