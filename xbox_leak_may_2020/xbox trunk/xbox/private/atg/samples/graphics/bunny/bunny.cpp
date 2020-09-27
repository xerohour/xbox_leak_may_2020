//-----------------------------------------------------------------------------
// File: teddy.cpp
//
// Desc: This is the main file for the teddy demo, which illustrates using
//		 vertex shaders for shell expansion, lighting, and wind.
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBHelp.h>
#include <XBSound.h>
#include "xpath.h"
#include "xobj.h"
#include "xfpatch.h"
#include "xfpmesh.h"
#include "util.h"

bool	g_bDrawNormals = false;
bool    g_bDrawTexCoordFrames = false;

bool	g_bDrawFins = true;
bool	g_bDrawShells = true;
bool    g_bMultiShell = false;	// If set, draw multiple shells at once, using the vertex shader to compute the projected offsets.

bool	g_bWind = false;
bool	g_bLocalLighting = true;
bool	g_bSelfShadow = true;

D3DXVECTOR3 g_vLookAt;

D3DXVECTOR4 g_vWind1;
D3DXVECTOR4 g_vWind2;
D3DXVECTOR4 g_vWind3;
float g_fWindChoose;

// help screen definitions
XBHELP_CALLOUT g_NormalHelpCallouts[] =
{
    { XBHELP_LEFTSTICK, XBHELP_PLACEMENT_1, L"Rotate/Zoom the model"},
    { XBHELP_RIGHTSTICK, XBHELP_PLACEMENT_2, L"Rotate the model\nand light source"},
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_2, L"Display\nhelp" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Debug lighting mode" },
    { XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_1, L"Right trigger to use blower" },
};
#define MAX_NORMAL_HELP_CALLOUTS (sizeof(g_NormalHelpCallouts)/sizeof(XBHELP_CALLOUT))

XBHELP_CALLOUT g_LightingHelpCallouts[] =
{
    { XBHELP_LEFTSTICK, XBHELP_PLACEMENT_1, L"Rotate/Zoom the model"},
    { XBHELP_RIGHTSTICK, XBHELP_PLACEMENT_2, L"Rotate the model\nand light source"},
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_2, L"Display\nhelp" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Debug shell mode" },
    { XBHELP_X_BUTTON, XBHELP_PLACEMENT_1, L"+ TexOP1" },
    { XBHELP_Y_BUTTON, XBHELP_PLACEMENT_1, L"- TexOP1" },
    { XBHELP_A_BUTTON, XBHELP_PLACEMENT_1, L"+ TexOP2" },
    { XBHELP_B_BUTTON, XBHELP_PLACEMENT_1, L"- TexOP2" },
	{ XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Toggle\nNormals" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_1, L"Toggle\nLighting" },
};
#define MAX_LIGHTING_HELP_CALLOUTS (sizeof(g_LightingHelpCallouts)/sizeof(XBHELP_CALLOUT))

XBHELP_CALLOUT g_ShellHelpCallouts[] =
{
    { XBHELP_LEFTSTICK, XBHELP_PLACEMENT_1, L"Rotate/Zoom the model"},
    { XBHELP_RIGHTSTICK, XBHELP_PLACEMENT_2, L"Rotate the model\nand light source"},
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_2, L"Display\nhelp" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Default mode" },
    { XBHELP_X_BUTTON, XBHELP_PLACEMENT_1, L"Toggle shells" },
    { XBHELP_Y_BUTTON, XBHELP_PLACEMENT_1, L"Toggle multishell" },
    { XBHELP_A_BUTTON, XBHELP_PLACEMENT_1, L"Toggle fins" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_1, L"Reload" },
	{ XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Toggle\nTextures" },
};
#define MAX_SHELL_HELP_CALLOUTS (sizeof(g_ShellHelpCallouts)/sizeof(XBHELP_CALLOUT))


//-----------------------------------------------------------------------------
// Name: class CXBSound2
// Desc: subclass to allow volume setting
//-----------------------------------------------------------------------------
class CXBSound2 : public CXBSound {
    HRESULT SetVolume( LONG lVolume )
	{
		if( NULL == m_pDSoundBuffer )
			return E_INVALIDARG;
		return m_pDSoundBuffer->SetVolume(lVolume);
	}
};


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
	enum {
		DEFAULT_MODE,	// default mode, with just object and light
		DEBUG_LIGHTING_MODE,		// show lighting coordinates, activate pixel shader controls
		DEBUG_SHELL_MODE
	} m_mode;
	
    CXBFont m_Font;
    CXBHelp m_Help;
	BOOL m_bDrawHelp;
	BOOL m_bShowTextures;
	
	D3DVECTOR m_vViewAngle;
	D3DXVECTOR3	m_vCameraPos;		// camera rest position
	D3DXVECTOR3 m_vEyePos;			// eye position
	D3DXMATRIX m_matProjection;		// projection matrix
	D3DXMATRIX m_matView;			// view matrix
	
	D3DXVECTOR3 m_vLightPos;		// current position of light
	D3DLIGHT8 m_Light0;				// d3d8 light
	float m_fLightAngle;			// direction of light around the model
	CXObject m_LightObj;			// light icon
	
	D3DMATERIAL8 m_Material;
	
	CXObject m_teddy, m_teddyEyes, m_teddyNose;
	CXFPatch m_FPatch;
	CXFPatchMesh m_FPatchMesh;

	BOOL m_dwLoadPhase;
	float m_fLOD;
	
#define VS_CONFIGURATIONS 16	 /* four bits: wind local_lighting self_shadowing multishell */
	DWORD m_adwFurVS[VS_CONFIGURATIONS];
	DWORD m_adwFinVS[VS_CONFIGURATIONS];
	
    // CXBSound2   m_BlowerSound;
public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();
	HRESULT Cleanup();

    CXBoxSample();
	HRESULT DrawLightIcon();
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
#ifdef _DEBUG	
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif

	m_bDrawHelp = FALSE;
	m_bShowTextures = FALSE;
	m_dwLoadPhase = 0;
	m_fLOD = 0.9f;
	for (UINT iConfig = 0; iConfig < VS_CONFIGURATIONS; iConfig++)
	{
		m_adwFurVS[iConfig] = 0;
		m_adwFinVS[iConfig] = 0;
	}
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;
	m_mode = DEFAULT_MODE;

    // Create a font
    if( FAILED( hr = m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

	// initialize the help system
    m_Help.Create( m_pd3dDevice, "Gamepad.xpr" );

    // Set projection transform
	float fFOV = D3DX_PI/4;
    D3DXMatrixPerspectiveFovLH(&m_matProjection, fFOV, 640.0f/480.0f, 0.1f, 10.0f);
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProjection);

	// load the fur and fin shaders
	for (UINT iConfig = 0; iConfig < VS_CONFIGURATIONS; iConfig++)
	{
		int iWind = (iConfig >> 2) & 1;
		int iLocal = (iConfig >> 1) & 1;
		int iSelf = iConfig & 1;
		
		// load the fur vertex shader
		if (iConfig < VS_CONFIGURATIONS / 2)
		{
			DWORD vsdecl[] = 
			{
				D3DVSD_STREAM(0),
				D3DVSD_REG(0, D3DVSDT_FLOAT3),		// vertex
				D3DVSD_REG(1, D3DVSDT_FLOAT3),		// normal
				D3DVSD_REG(2, D3DVSDT_FLOAT2),		// texture 0
				D3DVSD_END()
			};
			CHAR name[_MAX_PATH];
			_snprintf(name, _MAX_PATH, "shaders\\fur_wind%d_local%d_self%d.xvu", iWind, iLocal, iSelf);
			XBUtil_CreateVertexShader(m_pd3dDevice, _FNA(name), vsdecl, &m_adwFurVS[iConfig]);
			if (!m_adwFurVS[iConfig])
			{
				OUTPUT_DEBUG_STRING( "Initialize : error loading \"" );
				OUTPUT_DEBUG_STRING(name);
				OUTPUT_DEBUG_STRING( "\"\n" );
			}
		}
		else
		{
			// Multishell shaders need a coordinate frame at each vertex
			DWORD vsdecl[] = 
			{
				D3DVSD_STREAM(0),
				D3DVSD_REG(0, D3DVSDT_FLOAT3),		// vertex
				D3DVSD_REG(1, D3DVSDT_FLOAT3),		// normal
				D3DVSD_REG(2, D3DVSDT_FLOAT2),		// texture 0
				D3DVSD_STREAM(1),
				D3DVSD_REG(3, D3DVSDT_FLOAT3),		// S texture vector
				D3DVSD_REG(4, D3DVSDT_FLOAT3),		// T texture vector
				D3DVSD_END()
			};
			CHAR name[_MAX_PATH];
			_snprintf(name, _MAX_PATH, "shaders\\fur_wind%d_local%d_self%d_multishell.xvu", iWind, iLocal, iSelf);
			XBUtil_CreateVertexShader(m_pd3dDevice, _FNA(name), vsdecl, &m_adwFurVS[iConfig]);
			if (!m_adwFurVS[iConfig])
			{
				OUTPUT_DEBUG_STRING( "Initialize : error loading \"" );
				OUTPUT_DEBUG_STRING(name);
				OUTPUT_DEBUG_STRING( "\"\n" );
			}
		}
		
		// load the fin vertex shader
		{
			DWORD vsdecl[] = 
			{
				D3DVSD_STREAM(0),				// vertex 1
				D3DVSD_REG(0, D3DVSDT_FLOAT3),		// vertex
				D3DVSD_REG(1, D3DVSDT_FLOAT3),		// normal
				D3DVSD_REG(2, D3DVSDT_FLOAT2),		// u,v
				D3DVSD_STREAM(1),				// vertex 2
				D3DVSD_SKIP(4),	// "half-stride", size(FVFT_XYZNORMTEX1) = (3 + 3 + 2) * sizeof(DWORD), so half size = 4 * sizeof(DWORD)
				D3DVSD_REG(3, D3DVSDT_FLOAT3),		// vertex
				D3DVSD_REG(4, D3DVSDT_FLOAT3),		// normal
				D3DVSD_REG(5, D3DVSDT_FLOAT2),		// u,v
				D3DVSD_STREAM(2),
				D3DVSD_REG(6, D3DVSDT_FLOAT1),	// selector
				D3DVSD_END()
			};
			CHAR name[_MAX_PATH];
			_snprintf(name, _MAX_PATH, "shaders\\fin_wind%d_local%d_self%d.xvu", iWind, iLocal, iSelf);
			XBUtil_CreateVertexShader(m_pd3dDevice, _FNA(name), vsdecl, &m_adwFinVS[iConfig]);
			if (!m_adwFinVS[iConfig])
			{
				OUTPUT_DEBUG_STRING( "Initialize : error loading \"" );
				OUTPUT_DEBUG_STRING(name);
				OUTPUT_DEBUG_STRING( "\"\n" );
			}
		}
	}

	// enable lighting
	m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x0);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// initalize material
	memset(&m_Material, 0, sizeof(D3DMATERIAL8));
	m_Material.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	m_Material.Diffuse = D3DXCOLOR(0.4f*0.85f, 0.4f*0.6f, 0.4f*0.55f, 1.f);
	m_Material.Specular = D3DXCOLOR(0.4f*0.85f, 0.4f*0.6f, 0.4f*0.55f, 1.f);
	m_Material.Power = 16.0f;
	m_pd3dDevice->SetMaterial(&m_Material);

	// init the rest of the stuff
	m_vCameraPos = D3DXVECTOR3(0.f, 0.3f, 2.f);
	m_vLightPos = D3DXVECTOR3(0.f, 0.7f, 0.7f);
	m_fLightAngle = 0.f;
	m_vViewAngle = D3DXVECTOR3(0.f, 0.f, 0.f);

	// init light position and color
	memset(&m_Light0, 0, sizeof(D3DLIGHT8));
	m_Light0.Type = D3DLIGHT_SPOT;
	m_Light0.Position = m_vLightPos;
	D3DXVECTOR3 vLightDir;
	D3DXVec3Normalize(&vLightDir, &m_vLightPos);
	m_Light0.Direction = -vLightDir;
	m_Light0.Diffuse.r = 1.0f;
	m_Light0.Diffuse.g = 1.0f;
	m_Light0.Diffuse.b = 1.0f;
	m_Light0.Specular.r = 1.0f;
	m_Light0.Specular.g = 1.0f;
	m_Light0.Specular.b = 1.0f;
	m_Light0.Range = 1000.0f;
	m_Light0.Attenuation0 = 1.0f;
	m_Light0.Phi = D3DX_PI;
	m_Light0.Theta = D3DX_PI/4.0;
	m_Light0.Falloff = 0.f;
	m_pd3dDevice->LightEnable(0, TRUE);
	m_pd3dDevice->SetLight(0, &m_Light0);

	// light
	float fLength = 0.2f;
	float fRadius0 = 0.01f;
	float fRadius1 = fLength * sinf(m_Light0.Theta);	// inner spotlight radius
	m_LightObj.m_Model = new CXModel;
	m_LightObj.m_Model->Cylinder(fRadius0, 0xc0ffffff, fRadius1, 0x00ffffff, fLength, 40, FVF_XYZDIFF, NULL, 1.0f);

	// set base path to our media directory
	// all the _FNA commands use this
	XPath_SetBasePath(_T("d:\\media\\"));

	// Add a sound for the blower
   // m_BlowerSound.Create( "Sounds\\blower.wav", DSBCAPS_CTRL3D );
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
	SAFE_RELEASE(m_LightObj.m_Model);
	for (UINT iConfig = 0; iConfig < VS_CONFIGURATIONS; iConfig++)
	{
		if (m_adwFurVS[iConfig])
		{
			m_pd3dDevice->DeleteVertexShader(m_adwFurVS[iConfig]);
			m_adwFurVS[iConfig] = 0;
		}
		if (m_adwFinVS[iConfig])
		{
			m_pd3dDevice->DeleteVertexShader(m_adwFinVS[iConfig]);
			m_adwFinVS[iConfig] = 0;
		}
	}
	// m_BlowerSound.Destroy();
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------

// for pixel shader debugging display
DWORD g_iCOLOROP1 = D3DTOP_MODULATE2X;
DWORD g_iCOLOROP2 = D3DTOP_MODULATE2X;

HRESULT CXBoxSample::FrameMove()
{
	// Get input from the keypad
	m_vViewAngle.y -= m_DefaultGamepad.fX1*1.0f*m_fElapsedTime;
	if(m_vViewAngle.y>D3DX_PI*2)
		m_vViewAngle.y -= D3DX_PI*2;
	if(m_vViewAngle.y<0.0f)
		m_vViewAngle.y += D3DX_PI*2;

	m_vViewAngle.x += m_DefaultGamepad.fY2*1.0f*m_fElapsedTime;
	if(m_vViewAngle.x>1.0f)
		m_vViewAngle.x = 1.0f;
	if(m_vViewAngle.x<-1.0f)
		m_vViewAngle.x = -1.0f;

	m_vCameraPos.z -= m_DefaultGamepad.fY1*0.5f*m_fElapsedTime;
	if(m_vCameraPos.z<0.2f)
		m_vCameraPos.z = 0.2f;

	if (m_mode == DEBUG_LIGHTING_MODE)
	{
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
			g_iCOLOROP1++;
			if (g_iCOLOROP1 > D3DTOP_BLENDCURRENTALPHA) g_iCOLOROP1 = D3DTOP_BLENDCURRENTALPHA;
		}
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			g_iCOLOROP1--;
			if (g_iCOLOROP1 < D3DTOP_SELECTARG1) g_iCOLOROP1 = D3DTOP_SELECTARG1;
		}
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			g_iCOLOROP2++;
			if (g_iCOLOROP2 > D3DTOP_BLENDCURRENTALPHA) g_iCOLOROP2 = D3DTOP_BLENDCURRENTALPHA;
		}
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			g_iCOLOROP2--;
			if (g_iCOLOROP2 < D3DTOP_SELECTARG1) g_iCOLOROP2 = D3DTOP_SELECTARG1;
		}

		// toggle normals and tex coord frames
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
			// three states: none, normals, both normals and tex coord frames
			if (g_bDrawNormals)
			{
				if (!g_bDrawTexCoordFrames)	
					g_bDrawTexCoordFrames = true;
				else
					g_bDrawNormals = g_bDrawTexCoordFrames = false;
			}
			else
				g_bDrawNormals = true;
		}

		// advance lighting mode
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
			UINT iConfig = (g_bLocalLighting ? (1 << 1) : 0) | (g_bSelfShadow ? 1 : 0);
			iConfig++;
			g_bLocalLighting = (iConfig >> 1) & 1;
			g_bSelfShadow = iConfig & 1;
		}

		// toggle mode
	    if(m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_START)
		    m_mode = DEBUG_SHELL_MODE;
	}
	else if (m_mode == DEBUG_SHELL_MODE)
	{
		// toggle fins
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			g_bDrawFins = !g_bDrawFins;

		// toggle shells
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X])
			g_bDrawShells = !g_bDrawShells;

		// toggle multishell
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
			g_bMultiShell = !g_bMultiShell;

		// toggle display of textures
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
			m_bShowTextures = !m_bShowTextures;

		// toggle mode
	    if(m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_START)
		    m_mode = DEFAULT_MODE;
	}
	else
	{
		// toggle mode
	    if(m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_START)
		    m_mode = DEBUG_LIGHTING_MODE;
	}

	// toggle help
    if(m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_BACK)
        m_bDrawHelp = !m_bDrawHelp;

	// load the bear in chunks so we dont wait several
	// seconds before the app starts up
	switch(m_dwLoadPhase)
	{
		case 0:
			m_dwLoadPhase++;
			break;

		case 1:
		{
			SAFE_RELEASE(m_teddy.m_Model);
			SAFE_DELETE(m_teddy.m_Material);
			SAFE_RELEASE(m_teddyEyes.m_Model);
			SAFE_DELETE(m_teddyEyes.m_Material);
			SAFE_RELEASE(m_teddyNose.m_Model);
			SAFE_DELETE(m_teddyNose.m_Material);

			// increment through several levels of detail and scales of fur
			struct TeddyModel {
				CHAR *Fur;
				CHAR *Eyes;
				CHAR *Nose;
			} rTeddy[] = {	
				{ "Models\\MBFur_scale10.m",     "Models\\MBEyes.m",     "Models\\MBNose.m" },
				{ "Models\\2000a_Fur_scale10.m", "Models\\2000a_Eyes.m", "Models\\2000a_Nose.m" },
				{ "Models\\1000a_Fur_scale10.m", "Models\\1000a_Eyes.m", "Models\\1000a_Nose.m" },
				{ "Models\\800a_Fur_scale10.m",  "Models\\800a_Eyes.m",  "Models\\800a_Nose.m" },
				{ "Models\\600a_Fur_scale10.m",  "Models\\600a_Eyes.m",  "Models\\600a_Nose.m" },
				{ "Models\\400a_Fur_scale10.m",  "Models\\400a_Eyes.m",  "Models\\400a_Nose.m" },
			};
			static int TeddyModelCount = sizeof(rTeddy) / sizeof(TeddyModel);
			static int iModel = 3;
			iModel++;
			if (iModel >= TeddyModelCount)
				iModel = 0;
			
			// load the teddy model
			CXModel *pModel = new CXModel;
			pModel->Read_M(_FNA(rTeddy[iModel].Fur), NULL, FVF_XYZNORMTEX1, 0, NULL);
			D3DMATERIAL8 *pMaterial = new D3DMATERIAL8;
			ZeroMemory(pMaterial, sizeof(D3DMATERIAL8));
			pMaterial->Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
			pMaterial->Diffuse = D3DXCOLOR(0.7f*0.85f, 0.7f*0.6f, 0.7f*0.45f, 1.f);
			pMaterial->Specular = D3DXCOLOR(0.7f*0.85f, 0.7f*0.6f, 0.7f*0.45f, 1.f);
			pMaterial->Power = 16.0f;
			m_teddy.m_Material = pMaterial;
			m_teddy.SetModel(pModel);

			// Add eyes and nose that are not furry
			{
				CXModel *pModelEyes = new CXModel;
				pModelEyes->Read_M(_FNA(rTeddy[iModel].Eyes), NULL, FVF_XYZNORMTEX1, 0, NULL);
				D3DMATERIAL8 *pMaterialEyes = new D3DMATERIAL8;
				ZeroMemory(pMaterialEyes, sizeof(D3DMATERIAL8));
				pMaterialEyes->Ambient  = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
				pMaterialEyes->Diffuse  = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
				pMaterialEyes->Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
				pMaterialEyes->Power = 40.0f;
				m_teddyEyes.m_Material = pMaterialEyes;
				m_teddyEyes.SetModel(pModelEyes);
				pModelEyes->Release();
			}
			{
				CXModel *pModelNose = new CXModel;
				pModelNose->Read_M(_FNA(rTeddy[iModel].Nose), NULL, FVF_XYZNORMTEX1, 0, NULL);
				D3DMATERIAL8 *pMaterialNose = new D3DMATERIAL8;
				ZeroMemory(pMaterialNose, sizeof(D3DMATERIAL8));
				pMaterialNose->Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
				pMaterialNose->Diffuse = D3DXCOLOR(0.7f*0.85f, 0.7f*0.6f, 0.7f*0.45f, 1.f);
				pMaterialNose->Specular = D3DXCOLOR(0.7f*0.85f, 0.7f*0.6f, 0.7f*0.45f, 1.f);
				pMaterialNose->Power = 16.0f;
				m_teddyNose.m_Material = pMaterialNose;
				m_teddyNose.SetModel(pModelNose);
				pModelNose->Release();
			}

			// calc bounding sphere
			BYTE *bptr;
			pModel->LockVB(&bptr, 0L);
			D3DXVECTOR3 vCenter;
			float fRadius;
			ComputeBoundingSphere(bptr, pModel->m_dwNumVertices, pModel->m_dwFVF, &vCenter, &fRadius);
			pModel->UnlockVB();
			pModel->Release();
			static D3DXVECTOR3 vCenterOffset(0.f, 0.15f, 0.f);	// move object down, slightly, by moving center of view up
			g_vLookAt = vCenter + vCenterOffset;
			m_dwLoadPhase++;
			break;
		}

		case 2:
			// copy patch mesh from input model and generate hair
			m_FPatchMesh.Initialize(&m_teddy, NULL, -1);
			m_FPatchMesh.ExtractFins();
			DWORD numfuzz = 8000;
			DWORD numfuzzlib = 32;
			DWORD numlayers = 12; // 16;
			DWORD volxsize = 128;
			DWORD volzsize = 128;
			m_FPatch.m_fXSize = 0.1f;
			m_FPatch.m_fYSize = 0.01f;
			m_FPatch.m_fZSize = 0.1f;
			m_FPatch.m_dwNumSegments	= 4;
			m_FPatch.m_fuzzRandom.colorBase = D3DXCOLOR(0.1f, 0.05f, 0.01f, 0.1f);
			m_FPatch.m_fuzzCenter.colorBase = D3DXCOLOR(0.501961f, 0.250980f, 0.1f, 1.f) - m_FPatch.m_fuzzRandom.colorBase;
			m_FPatch.m_fuzzRandom.colorTip = D3DXCOLOR(0.1f, 0.05f, 0.1f, 0.25f);
			m_FPatch.m_fuzzCenter.colorTip = D3DXCOLOR(0.8f, 0.7f, 0.5f, 0.5f) - m_FPatch.m_fuzzRandom.colorTip;
			m_FPatch.m_fuzzCenter.dp = D3DXVECTOR3(0.0f, 4.f, 0.5f);
			m_FPatch.m_fuzzRandom.dp = D3DXVECTOR3(0.25f, 0.25f, 0.25f);
			m_FPatch.m_fuzzCenter.ddp = D3DXVECTOR3(0.f, 0.f, 0.0f);
			m_FPatch.m_fuzzRandom.ddp = D3DXVECTOR3(0.5f, 0.5f, 0.5f);
			m_FPatch.InitFuzz(numfuzz, numfuzzlib);
			m_FPatch.GenVolume(numlayers, volxsize, volzsize);
			D3DMATERIAL8 material;
			ZeroMemory(&material, sizeof(material));
			material.Ambient  = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
			material.Diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
			material.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			material.Power = 40.0f;
			m_FPatch.SetHairLightingMaterial(&material);
			m_dwLoadPhase++;
			break;
	}
	
	// move the camera around the model and always point right at it
	D3DXMATRIX m, m2;
	D3DXMatrixRotationYawPitchRoll(&m, m_vViewAngle.y, m_vViewAngle.x, m_vViewAngle.z);
	D3DXVec3TransformCoord(&m_vEyePos, &m_vCameraPos, &m);
	m_vEyePos += g_vLookAt;
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_matView, &m_vEyePos, &g_vLookAt, &up);
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

	// move the light around the model
	m_fLightAngle -= m_DefaultGamepad.fX2*1.0f*m_fElapsedTime;
	if(m_fLightAngle>D3DX_PI*2)
		m_fLightAngle -= D3DX_PI*2;
	if(m_fLightAngle<0.0f)
		m_fLightAngle += D3DX_PI*2;
	D3DXMatrixRotationY(&m, m_fLightAngle);
	D3DXVECTOR3 lpos;
	D3DXVec3TransformCoord(&lpos, &m_vLightPos, &m);
	D3DXVECTOR3 ldir;
	D3DXVec3Normalize(&ldir, &lpos);
	lpos += g_vLookAt;
	m_Light0.Position = lpos;
	m_Light0.Direction = -ldir;
	m_pd3dDevice->SetLight(0, &m_Light0);
	m_LightObj.SetPosition(m_Light0.Position.x, m_Light0.Position.y, m_Light0.Position.z);

	// set world-space light and eye positions for vertex shader
	g_LightPos = lpos;
	g_EyePos = m_vEyePos;
	
	// light looks at g_vLookAt
	D3DXMatrixLookAtLH(&m_LightObj.m_matOrientation, &m_LightObj.m_vPosition, &g_vLookAt, &up);
	D3DXMatrixInverse(&m_LightObj.m_matOrientation, NULL, &m_LightObj.m_matOrientation);

	// Set wind parameters
	static float fWindAmplitude = 0.01f;
	static float fWindFrequency = 2.f * D3DX_PI / 0.2f;
	static float fWindZero = -0.25f;
	// static float fPenalty = 1.5f;
	static float fTangentPlaneFraction = 0.9f;
	static float fWindStart = -7.f;	// start the wind gradually
	static float fWindDecay = -5.f;	// stop the wind gradually 
	static float fWindSwirlRadius = 0.1f;
	static float fWindSwirlFrequency = 2.f * D3DX_PI / 0.3f;
	g_vWind1.x = g_LightPos.x;
	g_vWind1.y = g_LightPos.y;
	g_vWind1.z = g_LightPos.z;
	D3DXMATRIX *pmat = &m_LightObj.m_matOrientation;	// grab left and up out of light matrix
	D3DXVECTOR3 vX(pmat->m[0][0], pmat->m[0][1], pmat->m[0][2]);
	D3DXVECTOR3 vY(pmat->m[1][0], pmat->m[1][1], pmat->m[1][2]);
	*(D3DXVECTOR3 *)&g_vWind2 = vY;
	g_vWind2.w = fTangentPlaneFraction;
	*(D3DXVECTOR3 *)&g_vWind3 = vX;
	g_vWind3.w = 0.f;
	if(m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
	{
		g_bWind = true;

		// blow-dryer
		float fWind = fWindZero + fWindAmplitude * cosf(m_fTime * fWindFrequency); 
		g_fWindChoose *= expf(fWindStart * m_fElapsedTime);
		g_vWind1.w = (1.f - g_fWindChoose) * fWind;
		// move source in small swirl around light source position
		float fWindSwirlX = fWindSwirlRadius * cosf(m_fTime * fWindSwirlFrequency);
		float fWindSwirlY = fWindSwirlRadius * sinf(m_fTime * fWindSwirlFrequency);
		*(D3DXVECTOR3 *)&g_vWind1 += fWindSwirlX * vX + fWindSwirlY * vY;
		
#if 0
		// update the sound
		D3DXVECTOR3 vSoundPosition( *(D3DXVECTOR3 *)&g_vWind1 );
        D3DXVECTOR3 vSoundVelocity = fWindSwirlX * vY - fWindSwirlY * vX; // velocity is perpendicular
		m_BlowerSound.Play();
        m_BlowerSound.SetPosition( vSoundPosition );
        m_BlowerSound.SetVelocity( vSoundVelocity );
#endif
#if 0
		// ambient wind
		static D3DXVECTOR3 fAmbientWindAmplitude(0.001f, 0.001f, 0.001f);
		static D3DXVECTOR3 fAmbientWindFrequency(2.f * D3DX_PI / 0.25f, 2.f * D3DX_PI / 0.3f, 2.f * D3DX_PI / 0.1f);
		static D3DXVECTOR3 fAmbientWindZero(0.f, 0.f, 0.f);
		g_vWind1.x = fAmbientWindZero.x + fAmbientWindAmplitude.x * cosf(m_fTime * fAmbientWindFrequency.x);
		g_vWind1.y = fAmbientWindZero.y + fAmbientWindAmplitude.y * cosf(m_fTime * fAmbientWindFrequency.y);
		g_vWind1.z = fAmbientWindZero.z + fAmbientWindAmplitude.z * cosf(m_fTime * fAmbientWindFrequency.z);
#endif
	}
	else
	{
		// turn-off wind
		g_fWindChoose = 1.f;
		g_vWind1.w *= expf(fWindDecay * m_fElapsedTime);

		// wait until wind has died out to turn off the wind vertex shader
		float fWindEpsilon = 1e-3f;
		if (fabsf(g_vWind1.w) < fWindEpsilon)
			g_bWind = false;
		
		// m_BlowerSound.Stop();
	}
	D3DXMATRIX matWorldInverse;
	D3DXMatrixInverse(&matWorldInverse, NULL, &g_matWorld);
	D3DXVec3TransformCoord((D3DXVECTOR3 *)&g_vWind1, (D3DXVECTOR3 *)&g_vWind1, &matWorldInverse);
	D3DXVec3TransformNormal((D3DXVECTOR3 *)&g_vWind2, (D3DXVECTOR3 *)&g_vWind2, &matWorldInverse);
	D3DXVec3TransformNormal((D3DXVECTOR3 *)&g_vWind3, (D3DXVECTOR3 *)&g_vWind3, &matWorldInverse);

	// Do reload
	if(m_mode == DEBUG_SHELL_MODE
		&& m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
	{
		m_dwLoadPhase = 0;
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawLightIcon()
// Desc: Draw light as a bright cone
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawLightIcon()
{
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE /*D3DBLEND_INVSRCALPHA*/);
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_LightObj.Render(OBJ_NOMCALCS);
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
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
	D3DXVECTOR3 L(g_LightPos - g_vLookAt), E(g_EyePos - g_vLookAt);
	bool bLightBehind = (D3DXVec3Dot(&L, &E) < 0.f);

    // Clear the viewport
	RenderGradientBackground(D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f), D3DXCOLOR(0.3f, 0.3f, 0.4f, 1.f));
    //m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DXCOLOR(0.3f, 0.3f, 0.4f, 1.f), 1.0f, 0L);

    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_LightObj.m_matOrientation);
	if (m_mode == DEBUG_LIGHTING_MODE)
		DrawAxes();		// draw coordinate axes for light
	if (bLightBehind) 
		DrawLightIcon();			// draw the light icon behind the fur

	// font code clobbers this state so we reset it here every frame
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	
	// render the teddy object
	if(m_dwLoadPhase>1)
	{
		m_teddy.m_Model->m_dwVShader = m_teddy.m_Model->m_dwFVF;
		m_teddy.Render(0);
		if (m_teddyEyes.m_Model)
		{
			m_teddyEyes.m_Model->m_dwVShader = m_teddyEyes.m_Model->m_dwFVF;
			m_teddyEyes.Render(0);
		}
		if (m_teddyNose.m_Model)
		{
			m_teddyNose.m_Model->m_dwVShader = m_teddyNose.m_Model->m_dwFVF;
			m_teddyNose.Render(0);
		}
		if (m_mode != DEFAULT_MODE)
		{
			if (g_bDrawNormals)
			{
				float fNormalScale = m_FPatch.m_fYSize;
				DrawNormals(m_teddy.m_Model->m_dwNumVertices, m_teddy.m_Model->m_pVB, m_teddy.m_Model->m_dwFVF, fNormalScale);
			}
			if (g_bDrawTexCoordFrames && m_dwLoadPhase > 2)
			{
				static float fVectorScale = 0.004f;
				// Draw S vector
				DrawVertexVectors(m_FPatchMesh.m_dwNumVertices,
								  m_FPatchMesh.m_pVB, sizeof(FVFT_XYZNORMTEX1), 0,
								  m_FPatchMesh.m_pVBTex, sizeof(D3DVECTOR) * 2, 0,
								  fVectorScale,
								  D3DCOLOR_RGBA(0x88, 0x00, 0x00, 0xff),
								  D3DCOLOR_RGBA(0xff, 0x00, 0x00, 0xff));
				// Draw T vector
				DrawVertexVectors(m_FPatchMesh.m_dwNumVertices,
								  m_FPatchMesh.m_pVB, sizeof(FVFT_XYZNORMTEX1), 0,
								  m_FPatchMesh.m_pVBTex, sizeof(D3DVECTOR) * 2, sizeof(D3DVECTOR),
								  fVectorScale,
								  D3DCOLOR_RGBA(0x00, 0x88, 0x00, 0xff),
								  D3DCOLOR_RGBA(0x00, 0xff, 0x00, 0xff));
			}
			DrawAxes();	// draw coordinate axes
		}
	}

	// render the patch mesh
	if(m_dwLoadPhase>2)
	{
		g_matWorld = (D3DXMATRIX)m_teddy.m_matOrientation;
		g_matView = m_matView;
		g_matProj = m_matProjection;
		// pick vertex shaders depending on current settings
		UINT iConfig = (g_bMultiShell ? (1 << 3) : 0) | (g_bWind ? (1 << 2) : 0) | (g_bLocalLighting ? (1 << 1) : 0) | (g_bSelfShadow ? 1 : 0);
		g_dwFuzzVS = m_adwFurVS[iConfig];
		g_dwFinVS = m_adwFinVS[iConfig];
		m_FPatch.Render(m_fLOD, &m_FPatchMesh);
	}
	if (!bLightBehind)	
		DrawLightIcon();			// draw light icon to overlay the fur

	// Start text drawing
    m_Font.Begin();

	// show title
    m_Font.DrawText(64, 50, 0xffffffff, L"TEDDY");
#ifdef _DEBUG
    m_Font.DrawText(450, 50, 0xffffff00, m_strFrameRate);
#endif

	// show status
	if(m_dwLoadPhase==1)
		m_Font.DrawText(320, 50, 0xffffff00, L"LOADING MODEL", XBFONT_CENTER_X);
	else if(m_dwLoadPhase==2)
		m_Font.DrawText(320, 50, 0xffffff00, L"GENERATING HAIR", XBFONT_CENTER_X);

	float y = 50, ydelta = 25;
	WCHAR buf[100];
	if (m_mode == DEFAULT_MODE)
	{
		if(m_bDrawHelp)
			m_Help.Render(&m_Font, g_NormalHelpCallouts, MAX_NORMAL_HELP_CALLOUTS);
	}
	else if (m_mode == DEBUG_LIGHTING_MODE)
	{
		m_Font.DrawText(64, y += ydelta, 0xffffffff, L"light");
		_snwprintf(buf, 100, L"%g", g_LightPos.x);
		m_Font.DrawText(64, y += ydelta, 0xffff0000, buf);
		_snwprintf(buf, 100, L"%g", g_LightPos.y);
		m_Font.DrawText(64, y += ydelta, 0xff00ff00, buf);
		_snwprintf(buf, 100, L"%g", g_LightPos.z);
		m_Font.DrawText(64, y += ydelta, 0xff0000ff, buf);
		m_Font.DrawText(64, y += ydelta, 0xffffffff, L"eye");
		_snwprintf(buf, 100, L"%g", g_EyePos.x);
		m_Font.DrawText(64, y += ydelta, 0xffff0000, buf);
		_snwprintf(buf, 100, L"%g", g_EyePos.y);
		m_Font.DrawText(64, y += ydelta, 0xff00ff00, buf);
		_snwprintf(buf, 100, L"%g", g_EyePos.z);
		m_Font.DrawText(64, y += ydelta, 0xff0000ff, buf);
		m_Font.DrawText(64, y += ydelta, 0xffffffff, GetString_D3DTEXTUREOP(g_iCOLOROP1));
		m_Font.DrawText(64, y += ydelta, 0xffffffff, GetString_D3DTEXTUREOP(g_iCOLOROP2));
		m_Font.DrawText(64, y += ydelta, 0xffffffff, g_bLocalLighting ? L"Local lighting" : L"Directional lighting");
		m_Font.DrawText(64, y += ydelta, 0xffffffff, g_bSelfShadow ? L"Self-shadowing" : L"No self-shadowing");

		if(m_bDrawHelp)
			m_Help.Render(&m_Font, g_LightingHelpCallouts, MAX_LIGHTING_HELP_CALLOUTS);
	}
	else if (m_mode == DEBUG_SHELL_MODE)
	{
		m_Font.DrawText(64, y += ydelta, 0xffffffff, L"Debug Shells");
		m_Font.DrawText(64, y += ydelta, 0xffffffff, g_bDrawFins ? L"Draw fins" : L"No fins");
		m_Font.DrawText(64, y += ydelta, 0xffffffff, g_bDrawShells ? L"Draw shells" : L"No shells");
		if (m_bShowTextures)
		{
			// show hair textures
			float fAlpha = 1.f;
			PrepareDeviceForDrawSprite(m_pd3dDevice);
			float fScreenLeft = 640.f * 0.2f;
			float fScreenTop = 480.f * 0.0f;
			float fScreenRight = 640.f * 0.9f;
			float fScreenBottom = 480.f * 0.9f;
			float fScreenWidth = fScreenRight - fScreenLeft;
			float fScreenHeight = fScreenBottom - fScreenTop;
			float fBorder = 5.f;
			float fSpriteWidth = (float)m_FPatch.m_dwVolXSize;
			float fSpriteHeight = (float)m_FPatch.m_dwVolZSize;
			UINT nSprite = m_FPatch.m_dwNumLayers + 1; // one fin texture
			UINT nRow, nColumn = 3;
			float fScaleSprite, fWidth, fHeight;
			do {
				nColumn++;
				nRow = (nSprite + nColumn - 1) / nColumn;
				fWidth = (fScreenWidth - (nColumn - 1) * fBorder) / nColumn;
				fScaleSprite = fWidth / fSpriteWidth;
				fHeight = fSpriteHeight * fScaleSprite;
			} while (nRow * fHeight + (nRow - 1) * fBorder > fScreenHeight);
			UINT iColumn = 0, iRow = 0;
			float fWidthRemaining = fScreenWidth - (nColumn * fWidth + (nColumn - 1) * fBorder);
			float fHeightRemaining = fScreenHeight - (nRow * fHeight + (nRow - 1) * fBorder);
			D3DXVECTOR3 vDest(fScreenLeft + 0.5f*fWidthRemaining, fScreenTop + 0.5f * fHeightRemaining, 0.f);
			LPDIRECT3DTEXTURE8 pTexture;
			for (UINT iLayer = 0; iLayer < m_FPatch.m_dwNumLayers; iLayer++)
			{
				pTexture = m_FPatch.m_pVolTexture[iLayer];
				DrawSpriteSimple(m_pd3dDevice, pTexture, vDest, fAlpha, fScaleSprite, NULL);
				iColumn++;
				if (iColumn == nColumn)
				{
					iColumn = 0;
					iRow++;
					vDest.x = fScreenLeft;
					vDest.y += fHeight + fBorder;
				}
				else
				{
					vDest.x += fWidth + fBorder;
				}
			}

			// Currently, fin texture is the same size as the rest of the layers
			pTexture = m_FPatch.m_pFinTexture;
			DrawSpriteSimple(m_pd3dDevice, pTexture, vDest, fAlpha, fScaleSprite, NULL);
		}

		if(m_bDrawHelp)
			m_Help.Render(&m_Font, g_ShellHelpCallouts, MAX_SHELL_HELP_CALLOUTS);
	}

	// End text drawing
    m_Font.End();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}
