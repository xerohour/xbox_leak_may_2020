//-----------------------------------------------------------------------------
// File: Fur.cpp
//
// Desc: This is the main file for the fur demo, which shows how to draw fur
//       using vertex shaders (for shell expansion, lighting, and wind) and
//       pixel shaders (for combining slice textures with a hair lighting
//       texture and for fading-in level-of-detail layers.)
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBHelp.h>
#include <wchar.h>
#include "Clip.h"
#include "XBFur.h"
#include "XBFurMesh.h"
#include "xobj.h"
#include "xpath.h"
#include "util.h"

// help screen definitions
XBHELP_CALLOUT g_NormalHelpCallouts[] =
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate/Zoom the model"},
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Rotate the model\nand light source"},
    { XBHELP_DPAD,         XBHELP_PLACEMENT_1, L"Model Count" },
    { XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_1, L"Right trigger to use blower" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Toggle help" },
};
#define MAX_NORMAL_HELP_CALLOUTS (sizeof(g_NormalHelpCallouts)/sizeof(XBHELP_CALLOUT))

bool  g_bDrawFins = true;
bool  g_bDrawShells = true;

bool  g_bWind = false;
bool  g_bLocalLighting = false;
bool  g_bSelfShadow = false;

DWORD g_dwNumShellsDrawn;

D3DXVECTOR4 g_vWind1;   // wind origin
D3DXVECTOR4 g_vWind2;   // wind up vector
D3DXVECTOR4 g_vWind3;   // wind left vector
float g_fWindChoose;    // wind smooth start / stop

D3DXVECTOR3 g_LightPos;                 // current light position
D3DXVECTOR3 g_EyePos;                   // current eye position
D3DXVECTOR3 g_vLookAt;

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
    D3DXVECTOR3 m_vCameraPos;       // camera rest position
    D3DXVECTOR3 m_vEyePos;          // eye position
    D3DXMATRIX m_matProjection;     // projection matrix
    D3DXMATRIX m_matView;           // view matrix
    D3DXVECTOR3 m_vLightPos;
    D3DLIGHT8 m_Light0;             // d3d8 light
    FLOAT m_fLightAngle;
    CXObject m_LightObj;            // light icon
#define TEDDYCOUNT 3
    struct Teddy {                  // the teddy model comes in several levels of geometric detail
        D3DXVECTOR3 m_vMin, m_vMax; // bounding box
        CXBMesh m_Mesh;             // skin base mesh, plus eyes and nose
        CXBFurMesh m_FurMesh;       // base mesh for fur + "fins" to get better silhouettes
    } m_rTeddy[TEDDYCOUNT];
    CXBFur m_Fur;                   // fur texture
    DWORD m_dwLoadPhase;            // keeps track of current loading stage
    FLOAT m_fLevelOfDetail;         // scale factor for level of detail calculation
    INT m_iNumInstances;
#define NMAXINSTANCE 1000
    struct Instance {
        D3DXVECTOR3 vPosition;      // position of instance
        FLOAT fLevelOfDetail;       // level-of-detail value for this object
        UINT iModel;                // which model to use for this object
        D3DXMATRIX matWorld;        // world matrix for instance
    } m_rInstance[NMAXINSTANCE];    // model instances, sorted by distance from eye
    INT m_iNumActiveInstances;
    struct ActiveInstance {
        FLOAT fDist2;       // squared distance from eye. This field must be first for qsort to work.
        INT iInstance;      // index of instance
    } m_rActiveInstance[NMAXINSTANCE];
    INT m_iNumSlices;       // number of slices for the fur texture
    UINT m_iTextureIndex;   // most recently compressed texture index
    
#define VERTEXSHADER_CONFIGURATIONS 8    /* three bits: wind local_lighting self_shadowing */
    DWORD m_rdwFurVS[VERTEXSHADER_CONFIGURATIONS];
    DWORD m_rdwFinVS[VERTEXSHADER_CONFIGURATIONS];

#define PIXELSHADER_CONFIGURATIONS 3    /* 0=texture + hairlighting,  1=blend LOD texture + hairlighting, 2=fade texture + lighting */
    DWORD m_rdwFurPS[PIXELSHADER_CONFIGURATIONS];
    
public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();
    HRESULT Cleanup();

    CXBoxSample();
    HRESULT DrawLightIcon();
    HRESULT LoadModels();			// load geometric models
    HRESULT ExtractFins();			// group mesh edges of original model according to view direction
	HRESULT CreateHairTextures();	// slice particle system hair into textures
    HRESULT UpdateInstances();
} g_xbApp;

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
void __cdecl main()
{
    if( FAILED( g_xbApp.Create() ) )
        return;
    g_xbApp.Run();
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
    m_dwLoadPhase = 0;
    m_fLevelOfDetail = 0.045f;
    m_iNumInstances = 25;   // start with a bunch of teddy bears
    m_iNumSlices = 8;   // Increase this number to get finer detail along the length of the fur
    for (UINT iConfig = 0; iConfig < VERTEXSHADER_CONFIGURATIONS; iConfig++)
    {
        m_rdwFurVS[iConfig] = 0;
        m_rdwFinVS[iConfig] = 0;
    }
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;

    // Create a font
    if( FAILED( hr = m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // initialize the help system
    m_Help.Create( m_pd3dDevice, "Gamepad.xpr" );

    // Set projection transform
    float fFOV = D3DX_PI/4;
    D3DXMatrixPerspectiveFovLH(&m_matProjection, fFOV, 640.0f/480.0f, 0.4f, 40.0f);
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProjection);

    // load the fur and fin vertex shaders
    for (UINT iConfig = 0; iConfig < VERTEXSHADER_CONFIGURATIONS; iConfig++)
    {
        int iWind = (iConfig >> 2) & 1;
        int iLocal = (iConfig >> 1) & 1;
        int iSelf = iConfig & 1;
        
        // load the fur vertex shader
        {
            DWORD vsdecl[] = 
            {
                D3DVSD_STREAM(0),
                D3DVSD_REG(0, D3DVSDT_FLOAT3),      // vertex
                D3DVSD_REG(1, D3DVSDT_FLOAT3),      // normal / hair tangent
                D3DVSD_REG(2, D3DVSDT_FLOAT2),      // texture 0
                D3DVSD_END()
            };
            CHAR name[_MAX_PATH];
            _snprintf(name, _MAX_PATH, "Shaders\\fur_wind%d_local%d_self%d.xvu", iWind, iLocal, iSelf);
			name[_MAX_PATH - 1] = '\0';
            XBUtil_CreateVertexShader(m_pd3dDevice, _FNA(name), vsdecl, &m_rdwFurVS[iConfig]);
            if (!m_rdwFurVS[iConfig])
            {
                OUTPUT_DEBUG_STRING("Initialize : error loading \"");
                OUTPUT_DEBUG_STRING(name);
                OUTPUT_DEBUG_STRING("\"\n");
            }
        }
        
        // load the fin vertex shader
        {
            DWORD vsdecl[] = 
            {
                D3DVSD_STREAM(0),
                D3DVSD_REG(0, D3DVSDT_FLOAT3),      // vertex
                D3DVSD_REG(1, D3DVSDT_FLOAT3),      // normal / hair tangent
                D3DVSD_REG(2, D3DVSDT_FLOAT2),      // u,v
                D3DVSD_REG(6, D3DVSDT_FLOAT3),      // fin face normal
                D3DVSD_END()
            };
            CHAR name[_MAX_PATH];
            _snprintf(name, _MAX_PATH, "Shaders\\fin_wind%d_local%d_self%d.xvu", iWind, iLocal, iSelf);
			name[_MAX_PATH - 1] = '\0';
            XBUtil_CreateVertexShader(m_pd3dDevice, _FNA(name), vsdecl, &m_rdwFinVS[iConfig]);
            if (!m_rdwFinVS[iConfig])
            {
                OUTPUT_DEBUG_STRING("Initialize : error loading \"");
                OUTPUT_DEBUG_STRING(name);
                OUTPUT_DEBUG_STRING("\"\n");
            }
        }
    }

    // create the fur pixel shaders
#pragma warning(push)
#pragma warning(disable: 4245)	// conversion from int to DWORD
    {
#include "furfade0.inl"
        g_pd3dDevice->CreatePixelShader(&psd, &m_rdwFurPS[0]);
    }
    {
#include "furfade1.inl"
        g_pd3dDevice->CreatePixelShader(&psd, &m_rdwFurPS[1]);
    }
    {
#include "furfade2.inl"
        g_pd3dDevice->CreatePixelShader(&psd, &m_rdwFurPS[2]);
    }
#pragma warning(pop)

    // enable lighting
    m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x0);
    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

    // init the rest of the stuff
    float fRadius = 1.f;
    m_vCameraPos = D3DXVECTOR3(0.f, fRadius * 0.3f, fRadius * 2.f);
    m_vLightPos = D3DXVECTOR3(0.f, 0.7f + fRadius * 0.3f, fRadius*0.7f);
    m_fLightAngle = 0.7f * D3DX_PI;
    m_vViewAngle = D3DXVECTOR3(0.f, 0.8 * D3DX_PI, 0.f);

    // init light position and color
    memset(&m_Light0, 0, sizeof(D3DLIGHT8));
    m_Light0.Type = D3DLIGHT_DIRECTIONAL;
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
    float fRadius1 = fLength * sinf(m_Light0.Theta);    // inner spotlight radius
    m_LightObj.m_Model = new CXModel;
    m_LightObj.m_Model->Cylinder(fRadius0, 0xc0ffffff, fRadius1, 0x00ffffff, fLength, 40, FVF_XYZDIFF, NULL, 1.0f);

    // set base path to our media directory
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
    SAFE_RELEASE(m_LightObj.m_Model);
    UINT iConfig;
    for (iConfig = 0; iConfig < VERTEXSHADER_CONFIGURATIONS; iConfig++)
    {
        if (m_rdwFurVS[iConfig])
        {
            m_pd3dDevice->DeleteVertexShader(m_rdwFurVS[iConfig]);
            m_rdwFurVS[iConfig] = 0;
        }
        if (m_rdwFinVS[iConfig])
        {
            m_pd3dDevice->DeleteVertexShader(m_rdwFinVS[iConfig]);
            m_rdwFinVS[iConfig] = 0;
        }
    }
    for (iConfig = 0; iConfig < PIXELSHADER_CONFIGURATIONS; iConfig++)
    {
        if (m_rdwFurPS[iConfig])
        {
            m_pd3dDevice->DeletePixelShader(m_rdwFurPS[iConfig]);
            m_rdwFurPS[iConfig] = 0;
        }
    }
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CompareDist2()
// Desc: Used by SortInstances as an ordering function to sort models from
//   near to far.
//-----------------------------------------------------------------------------
static int __cdecl CompareDist2(const void *arg1, const void *arg2 )
{
    float f1 = *(float *)arg1;
    float f2 = *(float *)arg2;
    if (f1 < f2) 
        return -1;
    else if (f1 > f2) 
        return 1;
    else
        return 0;
}

//-----------------------------------------------------------------------------
// Name: UpdateInstances()
// Desc: Arrange the instances in a triangular grid,
//       cull based on current view, and then sort front to back.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::UpdateInstances()
{
    // get Blinn-style clipping matrix for bounding box culling
    D3DXMATRIX matViewProj = m_matView * m_matProjection;
    D3DXMATRIX matViewProjClip;
    BlinnClipMatrix(&matViewProjClip, &matViewProj);

    FLOAT fScale = 3.0f;
    D3DXVECTOR3 vPosition(0.f, 0.f, 0.f);
    D3DXVECTOR3 vDir(fScale, 0.f, 0.f);
    UINT iStep = 0; // current position in leg
    UINT nStep2 = 0;  // number of steps in leg * 2
    m_iNumActiveInstances = 0;
    for (INT iInstance = 0; iInstance < m_iNumInstances; iInstance++)
    {
        // Set current position
        m_rInstance[iInstance].vPosition = vPosition;

        // set geometric level of detail
        // Simple scheme: high detail in center, less on outer rings
        if (iInstance == 0)
            m_rInstance[iInstance].iModel = 0;
        else if (iInstance < 25)
            m_rInstance[iInstance].iModel = 1;
        else
            m_rInstance[iInstance].iModel = 2;

        // Compute world matrix
        Teddy *pTeddy = &m_rTeddy[m_rInstance[iInstance].iModel];
        D3DXVECTOR3 *p = &m_rInstance[iInstance].vPosition;
        D3DXMatrixTranslation(&m_rInstance[iInstance].matWorld, p->x, p->y, p->z);

        // Cull if completely outside view frustum
        D3DXMATRIX matWorldViewProjClip = m_rInstance[iInstance].matWorld * matViewProjClip;
        if (BoundingBoxInFrustum(matWorldViewProjClip, pTeddy->m_vMin, pTeddy->m_vMax))
        {
            // compute distance squared
            D3DXVECTOR3 vEye = m_rInstance[iInstance].vPosition - m_vEyePos;
            FLOAT fDist2 = D3DXVec3LengthSq(&vEye);

            // compute level of detail based on scaled squared distance
            m_rInstance[iInstance].fLevelOfDetail = m_fLevelOfDetail * fDist2;

            // Add to active list
            m_rActiveInstance[m_iNumActiveInstances].fDist2 = fDist2;
            m_rActiveInstance[m_iNumActiveInstances].iInstance = iInstance;
            m_iNumActiveInstances++;
        }
        
        // Move to next grid position
        vPosition += vDir;
        iStep++;
        if (iStep * 2 > nStep2)
        {
            iStep = 0;
            nStep2++; // increase number of steps every two legs
            vDir = D3DXVECTOR3(-vDir.z, 0.f, vDir.x);           // Rotate direction
        }
    }
    qsort( (void *)m_rActiveInstance, m_iNumActiveInstances, sizeof(ActiveInstance), &CompareDist2 );
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LoadModels
// Desc: load model files
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::LoadModels()
{
    static CHAR *rstrTeddy[TEDDYCOUNT] = {
        "Models\\teddy2000.xbg",
        "Models\\teddy1000.xbg",
        "Models\\teddy0500.xbg",
    };
    for (UINT iTeddy = 0; iTeddy < TEDDYCOUNT; iTeddy++)
    {
        Teddy *pTeddy = &m_rTeddy[iTeddy];

        // Load meshes from xbg files
        if( FAILED( pTeddy->m_Mesh.Create( m_pd3dDevice, rstrTeddy[iTeddy], NULL )))
            return XBAPPERR_MEDIANOTFOUND;

        // Copy VB and IB pointers from first mesh of input model
        XBMESH_DATA *pMeshData = pTeddy->m_Mesh.GetMesh( 0 );
        pTeddy->m_FurMesh.Initialize(pMeshData->m_dwFVF, pMeshData->m_dwNumVertices, &pMeshData->m_VB,
                                     pMeshData->m_dwNumIndices, &pMeshData->m_IB);
        
        // Calc bounding box
        pTeddy->m_Mesh.ComputeBoundingBox(&pTeddy->m_vMin, &pTeddy->m_vMax);
		
        if (iTeddy == 0)    // set lookat to center of most detailed model
            g_vLookAt = 0.5f * (pTeddy->m_vMin +  pTeddy->m_vMax);  // center of bb
    }
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ExtractFins
// Desc: extract the fins from all the models
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::ExtractFins()
{
    for (UINT iTeddy = 0; iTeddy < TEDDYCOUNT; iTeddy++)
    {
        Teddy *pTeddy = &m_rTeddy[iTeddy];
        
        // extract fins
        UINT BinFactor = 5; // factor that chooses the number of angle bins to discretize.  See XBFurMesh.cpp for more.
        float fFinDotProductThreshold = 0.75f; // range of dot products of fin's face normal with eye vector
        float fFinEdgeTextureScale = 7.0f; // scaling term matches scale of texture as applied to underlying model
        pTeddy->m_FurMesh.ExtractFins(BinFactor, fFinDotProductThreshold, fFinEdgeTextureScale);
    }
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateHairTextures
// Desc: Create a particle system patch of hair and then sample
//       the patch into a slice texture.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateHairTextures()
{
	// setup hair parameters
    DWORD numfuzz = 8000;
    DWORD slicexsize = 128;
    DWORD slicezsize = 128;
    DWORD numfuzzlib = 32;
    DWORD numslices = m_iNumSlices;
    DWORD finWidth = 256; // we can afford to make the fin texture a little larger, since there's just one instead of numslices
    DWORD finHeight = 128;
    m_Fur.m_fXSize = 0.1f;
    m_Fur.m_fYSize = 0.01f;
    m_Fur.m_fZSize = 0.1f;
    m_Fur.m_dwNumSegments   = 4;
    m_Fur.m_fuzzRandom.colorBase = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.0f);
    m_Fur.m_fuzzCenter.colorBase = D3DXCOLOR(0.501961f, 0.250980f, 0.1f, 1.f) - 0.5f * m_Fur.m_fuzzRandom.colorBase;
    m_Fur.m_fuzzRandom.colorTip = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.1f);
    m_Fur.m_fuzzCenter.colorTip = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.1f);
    m_Fur.m_fuzzCenter.dp = D3DXVECTOR3(0.0f, 4.f, 0.5f);
    m_Fur.m_fuzzRandom.dp = D3DXVECTOR3(0.25f, 0.25f, 0.25f);
    m_Fur.m_fuzzCenter.ddp = D3DXVECTOR3(0.f, 0.f, 0.0f);
    m_Fur.m_fuzzRandom.ddp = D3DXVECTOR3(0.5f, 0.5f, 0.5f);
	
    // generate hair texture
    m_Fur.InitFuzz(numfuzz, numfuzzlib);
    m_Fur.GenSlices(numslices, slicexsize, slicezsize);
    // Generate fin texture with more image samples and more fuzz to get scaling right
    static float s_fFinXFraction = 0.25f; // proportion of fur that is projected in fin texture
    static float s_fFinZFraction = 0.05f;
    m_Fur.GenFin(finWidth, finHeight, s_fFinXFraction, s_fFinZFraction);
    m_Fur.ComputeLevelOfDetailTextures();
    m_Fur.SetLevelOfDetail(0.f);
    m_Fur.m_fYSize = 0.055f;
    
    // make hair lighting texture
    D3DMATERIAL8 material;
    ZeroMemory(&material, sizeof(material));
    material.Ambient  = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
    material.Diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
    material.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    material.Power = 40.0f;
    m_Fur.SetHairLightingMaterial(&material);
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // toggle help
    if(m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_BACK)
        m_bDrawHelp = !m_bDrawHelp;

    // load the scene in chunks so we dont wait several
    // seconds before the app starts up
    if (m_dwLoadPhase == 0)
    {
        // load the models
        LoadModels();
        m_dwLoadPhase++;
    }
    else if (m_dwLoadPhase == 1)
    {
        // compute the fins
        ExtractFins();
        m_dwLoadPhase++;
    }
    else if (m_dwLoadPhase == 2)
    {
		CreateHairTextures();
        m_dwLoadPhase++;
    }
    else if (m_dwLoadPhase == 3)
    {
        if (m_Fur.CompressNextTexture(D3DFMT_DXT4, &m_iTextureIndex) == S_OK)
            m_dwLoadPhase++;
        else 
            return S_OK;
    }

    // set number of models
    if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
    {
        if (m_iNumInstances < NMAXINSTANCE)
            m_iNumInstances ++ ;
    }
    if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
    {
        if (m_iNumInstances >1)
            m_iNumInstances -- ;
    }
   
    // move view
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

    m_vCameraPos.z -= m_DefaultGamepad.fY1*2.f*m_fElapsedTime;
    if(m_vCameraPos.z<0.2f)
        m_vCameraPos.z = 0.2f;

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
    static float fWindStart = -7.f; // start the wind gradually
    static float fWindDecay = -5.f; // stop the wind gradually 
    static float fWindSwirlRadius = 0.1f;
    static float fWindSwirlFrequency = 2.f * D3DX_PI / 0.3f;
    g_vWind1.x = g_LightPos.x;
    g_vWind1.y = g_LightPos.y;
    g_vWind1.z = g_LightPos.z;
    D3DXMATRIX *pmat = &m_LightObj.m_matOrientation;    // grab left and up out of light matrix
    D3DXVECTOR3 vX(pmat->m[0][0], pmat->m[0][1], pmat->m[0][2]);
    D3DXVECTOR3 vY(pmat->m[1][0], pmat->m[1][1], pmat->m[1][2]);
    *(D3DXVECTOR3 *)&g_vWind2 = vY;
    g_vWind2.w = fTangentPlaneFraction;
    *(D3DXVECTOR3 *)&g_vWind3 = vX;
    g_vWind3.w = 0.f;
    if(m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
    {
        // blow-dryer
        g_bWind = true;
        float fWind = fWindZero + fWindAmplitude * cosf(m_fTime * fWindFrequency); 
        g_fWindChoose *= expf(fWindStart * m_fElapsedTime);
        g_vWind1.w = (1.f - g_fWindChoose) * fWind;
        // move source in small swirl around light source position
        float fWindSwirlX = fWindSwirlRadius * cosf(m_fTime * fWindSwirlFrequency);
        float fWindSwirlY = fWindSwirlRadius * sinf(m_fTime * fWindSwirlFrequency);
        *(D3DXVECTOR3 *)&g_vWind1 += fWindSwirlX * vX + fWindSwirlY * vY;
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
    }

    // Position, sort, and cull the instances
    UpdateInstances();
    
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: DrawLightIcon()
// Desc: Draw light as a bright cone
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawLightIcon()
{
    // set and save state
    SET_D3DRS(D3DRS_LIGHTING, FALSE);
    SET_D3DRS(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
    SET_D3DRS(D3DRS_ALPHABLENDENABLE, TRUE);
    SET_D3DRS(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    SET_D3DRS(D3DRS_DESTBLEND, D3DBLEND_ONE);
    SET_D3DRS(D3DRS_CULLMODE, D3DCULL_NONE);
    SET_D3DRS(D3DRS_ZWRITEENABLE, FALSE);
    SET_D3DTSS(0, D3DTSS_COLOROP,       D3DTOP_SELECTARG1);
    SET_D3DTSS(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    SET_D3DTSS(0, D3DTSS_ALPHAOP,       D3DTOP_SELECTARG1);
    SET_D3DTSS(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

    // draw icon
    m_LightObj.Render(OBJ_NOMCALCS);
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

    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_LightObj.m_matOrientation);
    if (bLightBehind) 
        DrawLightIcon();            // draw the light icon behind the fur
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
    m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); // This is 

    // render the base meshes from near to far to initialize the skin and zbuffer
    if(m_dwLoadPhase>0)
    {
        for (INT iActiveInstance = 0; iActiveInstance < m_iNumActiveInstances; iActiveInstance++)       // near to far
        {
            INT iInstance = m_rActiveInstance[iActiveInstance].iInstance;
            UINT iTeddy = m_rInstance[iInstance].iModel;
            Teddy *pTeddy = &m_rTeddy[iTeddy];
            m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_rInstance[iInstance].matWorld);  // set world transformation
            pTeddy->m_Mesh.Render(m_pd3dDevice);    // draws skin, nose, and ears
        }
    }

    // render the fur in the oppposite order, from far to near, so that alpha-blending works
    g_dwNumShellsDrawn = 0;
    if(m_dwLoadPhase>2 && (g_bDrawFins || g_bDrawShells))
    {
        // pick vertex shaders depending on current settings
        UINT iConfig = (g_bWind ? (1 << 2) : 0) | (g_bLocalLighting ? (1 << 1) : 0) | (g_bSelfShadow ? 1 : 0);
        DWORD dwFurVS = m_rdwFurVS[iConfig];
        DWORD dwFinVS = m_rdwFinVS[iConfig];
        static float fFinLODFull = 3.5f;    // fade out between fFinFull and fFinCutoff
        static float fFinLODCutoff = 4.f;   // LOD's above this value don't get fins
        static float fFinExtraNormalScale = 1.25f;
        D3DXMATRIX matViewProjection;
        D3DXMatrixMultiply(&matViewProjection, &m_matView, &m_matProjection);
        for (INT iActiveInstance = m_iNumActiveInstances - 1; iActiveInstance >= 0; iActiveInstance--)      // far to near
        {
            INT iInstance = m_rActiveInstance[iActiveInstance].iInstance;
            UINT iTeddy = m_rInstance[iInstance].iModel;
            Teddy *pTeddy = &m_rTeddy[iTeddy];
            pTeddy->m_FurMesh.Begin(&g_EyePos, &g_LightPos, &matViewProjection);
            D3DXMATRIX *pmatWorld = &m_rInstance[iInstance].matWorld;
            D3DXMATRIX matWorldInverse;
            D3DXMatrixInverse(&matWorldInverse, NULL, pmatWorld);
            m_Fur.SetLevelOfDetail(m_rInstance[iInstance].fLevelOfDetail);
            pTeddy->m_FurMesh.BeginObject(pmatWorld, &matWorldInverse);
            if (g_bDrawFins) pTeddy->m_FurMesh.DrawFins(&m_Fur, dwFinVS, fFinLODFull, fFinLODCutoff, fFinExtraNormalScale);
            if (g_bDrawShells)
            {
                pTeddy->m_FurMesh.DrawShells(&m_Fur, dwFurVS, m_rdwFurPS);
                g_dwNumShellsDrawn += m_Fur.m_dwNumSlicesLOD;
            }
            pTeddy->m_FurMesh.EndObject();
            pTeddy->m_FurMesh.End();
        }
    }

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    if (!bLightBehind)  
        DrawLightIcon();            // draw light icon to overlay the fur

    // show title and framerate
    m_Font.Begin();
    m_Font.DrawText(  64, 50, 0xffffffff, L"Fur" );
    if (m_dwLoadPhase > 3) 
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

    // show shell and instance stats
    {
        const int buflen = 100;
        WCHAR buf[buflen];

        _snwprintf(buf, buflen, L"Shells Drawn %d", g_dwNumShellsDrawn);
		buf[buflen - 1] = L'\0';
        m_Font.DrawText( 576, 100, 0xffffff00, buf, XBFONT_RIGHT );

        _snwprintf(buf, buflen, L"Model Count %d", m_iNumInstances);
		buf[buflen - 1] = L'\0';
        m_Font.DrawText( 576, 125, 0xffffff00, buf, XBFONT_RIGHT );

        _snwprintf(buf, buflen, L"Active Count %d", m_iNumActiveInstances);
		buf[buflen - 1] = L'\0';
        m_Font.DrawText( 576, 150, 0xffffff00, buf, XBFONT_RIGHT );
     }

    // show status
    if ( m_dwLoadPhase == 0 )
        m_Font.DrawText( 280, 50, 0xff00ffff, L"Loading models", XBFONT_CENTER_X );
    else if ( m_dwLoadPhase == 1 )
        m_Font.DrawText( 280, 50, 0xff00ffff, L"Extracting fins", XBFONT_CENTER_X );
    else if ( m_dwLoadPhase == 2 )
        m_Font.DrawText( 280, 50, 0xff00ffff, L"Generating hair", XBFONT_CENTER_X) ;
    else if ( m_dwLoadPhase == 3 )
    {
        const int buflen = 100;
        WCHAR buf[buflen];
        _snwprintf(buf, buflen, L"Compressing texture %d of %d", m_iTextureIndex + 1, m_Fur.TotalTextureCount());
		buf[buflen - 1] = L'\0';
        m_Font.DrawText( 280, 50, 0xff00ffff, buf, XBFONT_CENTER_X);
    }
    
    // show help
    if(m_bDrawHelp)
        m_Help.Render(&m_Font, g_NormalHelpCallouts, MAX_NORMAL_HELP_CALLOUTS);
    m_Font.End();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}
