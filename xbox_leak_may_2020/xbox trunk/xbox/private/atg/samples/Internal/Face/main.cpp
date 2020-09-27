//-----------------------------------------------------------------------------
// File: main.cpp
//
// Desc: This is the main file for the facial animation demo.
//       It illustrates using vertex shaders for Lafortune lighting
//       which is a BRDF lighting model, and for the hair found
//       on the goatee (which does normal expansion and directional
//       lighting).
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBHelp.h>

#include "xpath.h"
#include "face.h"
#include "xfpatch.h"
#include "xfpmesh.h"

#define PI 3.14159f
#define TWOPI (PI*2.0f)

// these variables are global because the face
// uses them
D3DXVECTOR4 g_vEyePos;							// eye position
D3DXMATRIX g_mProj;								// projection matrix
D3DXMATRIX g_mView;								// view matrix
D3DLIGHT8 g_Light0;								// d3d8 light

// lafortune shader
DWORD g_vsLafortune;							// lafortune shader
D3DXVECTOR4	g_vLafConst[4];						// lafortune constants
BOOL g_bLafActive=TRUE;							// lafortune active flag

// face stuff
CFace g_Face;
CFaceAni g_FaceAni;
BOOL g_dwLoadPhase = 0;
float g_fLOD = 0.9f;

// hair stuff
CXFPatch g_FPatch;
CXFPatchMesh g_FPatchMesh;
DWORD g_BushVS;

// help screen definitions
XBHELP_CALLOUT g_NormalHelpCallouts[] =
{
    { XBHELP_LEFTSTICK,   XBHELP_PLACEMENT_1, L"Rotate/Zoom the model" },
    { XBHELP_RIGHTSTICK,  XBHELP_PLACEMENT_1, L"Rotate the model\nand light source" },
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, L"Display\nhelp" },
    { XBHELP_A_BUTTON,    XBHELP_PLACEMENT_1, L"Blink" },
    { XBHELP_B_BUTTON,    XBHELP_PLACEMENT_1, L"Toggle Lafortune" },
};
#define MAX_NORMAL_HELP_CALLOUTS 5

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont m_Font;
    CXBHelp m_Help;
	BOOL m_bDrawHelp;

	D3DVECTOR m_vViewAngle;
	D3DXVECTOR3	m_vCameraPos;

	D3DXVECTOR3 m_vLightPos;
	float m_fLightAng;
	D3DMATERIAL8 m_Material;

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();
	HRESULT Cleanup();

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
	m_bDrawHelp = FALSE;
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

	// initialize the help system
    m_Help.Create(m_pd3dDevice, "Gamepad.xpr");

    // Set projection transform
    D3DXMatrixPerspectiveFovLH(&g_mProj, D3DX_PI/4, 640.0f/480.0f, 0.005f, 2.0f);
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_mProj);

	// create lafortune vertex shader
	DWORD vsdec[] = 
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),		// vertex
		D3DVSD_REG(1, D3DVSDT_FLOAT3),		// normal
		D3DVSD_REG(2, D3DVSDT_FLOAT2),		// texture 0
		D3DVSD_END()
	};
	XBUtil_CreateVertexShader(m_pd3dDevice, _FNA("lafortn.xvu"), vsdec, &g_vsLafortune);
	if(!g_vsLafortune)
		OUTPUT_DEBUG_STRING( "Initialize : error loading lafortn.xvu\n" );

	// init lafortune constants
	g_vLafConst[0].x = -1.016939f;			// Cxy1
	g_vLafConst[0].y = -0.643533f;			// Cxy2
	g_vLafConst[0].z = -1.020153f;			// Cxy3
	g_vLafConst[0].w = 5.0f;				// w1
	g_vLafConst[1].x = -1.462488f;			// Cz1
	g_vLafConst[1].y = 0.410559f;			// Cz2
	g_vLafConst[1].z = 0.703913f;			// Cz3
	g_vLafConst[1].w = 5.0f;				// w2
	g_vLafConst[2].x = 3.699932f;			// n1
	g_vLafConst[2].y = 4.266495f;			// n2
	g_vLafConst[2].z = 63.919687f;			// n3
	g_vLafConst[2].w = 5.0f;				// w3
	g_vLafConst[3] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

	// load the hair vertex shader
	// uses the same declaration as the lafortune shader
	XBUtil_CreateVertexShader(m_pd3dDevice, _FNA("bush.xvu"), vsdec, &g_BushVS);
	if(!g_BushVS)
		OUTPUT_DEBUG_STRING( "Initialize : error loading bush.xvu\n" );

	// enable lighting
	m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x0);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// initalize material
	memset(&m_Material, 0, sizeof(D3DMATERIAL8));
	m_Material.Diffuse.r = 1.0f;
	m_Material.Diffuse.g = 1.0f;
	m_Material.Diffuse.b = 1.0f;
	m_Material.Specular.r = 0.25f;
	m_Material.Specular.g = 0.25f;
	m_Material.Specular.b = 0.25f;
	m_Material.Power = 16.0f;
	m_pd3dDevice->SetMaterial(&m_Material);

	// init light position and color
	memset(&g_Light0, 0, sizeof(D3DLIGHT8));
	g_Light0.Type = D3DLIGHT_POINT;
	g_Light0.Position.x = -0.5f;
	g_Light0.Position.y = 0.0f;
	g_Light0.Position.z = -0.5f;
	g_Light0.Diffuse.r = 1.0f;
	g_Light0.Diffuse.g = 1.0f;
	g_Light0.Diffuse.b = 1.0f;
	g_Light0.Specular.r = 1.0f;
	g_Light0.Specular.g = 1.0f;
	g_Light0.Specular.b = 1.0f;
	g_Light0.Range = 1000.0f;
	g_Light0.Attenuation0 = 1.0f;
	g_pd3dDevice->LightEnable(0, TRUE);
	g_pd3dDevice->SetLight(0, &g_Light0);

	// init the rest of the stuff
	m_vCameraPos = D3DXVECTOR3(0.0f, 0.0f, 0.6f);
	m_vLightPos = D3DXVECTOR3(0.0f, 0.1f, -1.0f);
	m_fLightAng = PI;
	m_vViewAngle = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// set base path to point to our face data
	// all the _FNA commands use this
	XPath_SetBasePath(_T("d:\\media\\"));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Clean up after ourselves.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Cleanup()
{
    m_Help.Destroy();
	m_Font.Destroy();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXVECTOR4 lpos;
	D3DXMATRIX m, m2;

	// move the camera around the head and always point
	// right at it
	m_vViewAngle.y -= m_DefaultGamepad.fX1*1.0f*m_fElapsedTime;
	if(m_vViewAngle.y>TWOPI)
		m_vViewAngle.y -= TWOPI;
	if(m_vViewAngle.y<0.0f)
		m_vViewAngle.y += TWOPI;

	m_vViewAngle.x += m_DefaultGamepad.fY2*1.0f*m_fElapsedTime;
	if(m_vViewAngle.x>1.0f)
		m_vViewAngle.x = 1.0f;
	if(m_vViewAngle.x<-1.0f)
		m_vViewAngle.x = -1.0f;

	m_vCameraPos.z -= m_DefaultGamepad.fY1*0.5f*m_fElapsedTime;
	if(m_vCameraPos.z<0.2f)
		m_vCameraPos.z = 0.2f;

	D3DXMatrixRotationYawPitchRoll(&m, m_vViewAngle.y, m_vViewAngle.x, m_vViewAngle.z);
	D3DXVec3Transform(&g_vEyePos, &m_vCameraPos, &m);
	D3DXMatrixLookAtLH(&m2, (D3DXVECTOR3 *)&g_vEyePos, (D3DXVECTOR3 *)&g_Face.m_objFace.m_vPosition, &up);
	D3DXMatrixTranslation(&m, 0.0f, -0.16f, 0.0f);
	D3DXMatrixMultiply(&g_mView, &m2, &m);
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &g_mView);

	// move the light around the face
	m_fLightAng -= m_DefaultGamepad.fX2*1.0f*m_fElapsedTime;
	if(m_fLightAng>TWOPI)
		m_fLightAng -= TWOPI;
	if(m_fLightAng<0.0f)
		m_fLightAng += TWOPI;

	D3DXMatrixRotationY(&m, m_fLightAng);
	D3DXVec3Transform(&lpos, &m_vLightPos, &m);
	g_Light0.Position.x = lpos.x;
	g_Light0.Position.y = lpos.y;
	g_Light0.Position.z = lpos.z;
	m_pd3dDevice->SetLight(0, &g_Light0);

	// set light position for vertex shader
	g_LightPos = g_Light0.Position;

	// blink
	if(m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_A])
		g_Face.m_fBlink = 0.6f;
	else
		g_Face.m_fBlink = 0.0f;

	// toggle help
    if(m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_BACK)
        m_bDrawHelp = !m_bDrawHelp;

	// toggle lafortune
	if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		g_bLafActive = !g_bLafActive;

	// load the face in chunks so we dont wait several
	// seconds before the app starts up
	switch(g_dwLoadPhase)
	{
		case 0:
			g_dwLoadPhase++;
			break;

		case 1:
			// load the face
			g_Face.Load(_FNA("face1.txt"), NULL);
			g_FaceAni.LoadAni(_FNA("talklo.off"), _FNA("talklo.jaw"));
			g_Face.m_Ani = &g_FaceAni;
			g_dwLoadPhase++;
			break;

		case 2:
			// load hair stuff
			g_FPatchMesh.Load(_FNA("goatee.fpm"));
			g_FPatchMesh.m_Obj = &g_Face.m_objFace;
			g_FPatchMesh.Build(1);
			g_FPatch.Load(_FNA(g_FPatchMesh.m_strFPatchName));
			g_dwLoadPhase++;
			break;
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
	D3DXVECTOR4 f;

    // Clear the viewport
    m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0L);

	// font code clobbers this state so we reset it here every frame
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	// render the face object
	if(g_dwLoadPhase>1)
		g_Face.Render();

	// build the patch mesh and render it
	if(g_dwLoadPhase>2)
	{
		g_FPatchMesh.m_bBuildFlag = 1;
		g_FPatchMesh.Build(0);
		g_matWorld = (D3DXMATRIX)g_Face.m_objFace.m_matOrientation;
		g_matView = g_mView;
		g_matProj = g_mProj;
		g_dwFuzzVS = g_BushVS;
		g_FPatch.Render(g_fLOD, &g_FPatchMesh);
	}

	// Begin text drawing
	m_Font.Begin();

	// show title
    m_Font.DrawText(64, 50, 0xffffffff, L"FACE");
    m_Font.DrawText(640-64, 50, 0xffffff00, m_strFrameRate, XBFONT_RIGHT);

	// show status
	if(g_dwLoadPhase==1)
		m_Font.DrawText(320, 50, 0xffffff00, L"LOADING FACE", XBFONT_CENTER_X);
	else if(g_dwLoadPhase==2)
		m_Font.DrawText(320, 50, 0xffffff00, L"GENERATING HAIR", XBFONT_CENTER_X);

	if(g_bLafActive)
	    m_Font.DrawText(64, 70, 0xff00ff00, L"LAFORTUNE ON");
	else
		m_Font.DrawText(64, 70, 0xffff0000, L"LAFORTUNE OFF");

	// End text drawing
	m_Font.End();

	if(m_bDrawHelp)
		m_Help.Render(&m_Font, g_NormalHelpCallouts, MAX_NORMAL_HELP_CALLOUTS);

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}
