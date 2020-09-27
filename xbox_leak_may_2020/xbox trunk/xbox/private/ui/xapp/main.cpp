#include "std.h"
#include "xapp.h"
#include "FileUtil.h"
#include "xip.h"
#include "Node.h"
#include "Screen.h"
#include "Runner.h"
#include "Lerper.h"
#include "Camera.h"
#include "NavInfo.h"
#include "Viewpoint.h"
#include "Background.h"
#include "xlaunch.h"

extern void DSound_Exit();

extern void Memory_Init();

extern void Debug_Init();
extern void Debug_Exit();

extern void Class_Init();
extern void Class_Exit();

#ifdef _DEBUG
extern void Debug_Frame();
#endif // _DEBUG

extern void Xbox_Init();

extern void DiscDrive_Init();

extern void BackgroundLoader_Frame();

extern void Text_Exit();

extern void Locale_Exit();

extern void TitleArray_Init();

extern void Material_Init();
extern float g_nEffectAlpha;

#ifdef _DEBUG
bool g_bWireframe = false;
#endif // _DEBUG

////////////////////////////////////////////////////////////////////////////

CXApp theApp;

D3DXMATRIX g_matView;
D3DXMATRIX g_matPosition;
D3DXMATRIX g_matProjection;
D3DRECT	   g_scissorRect;
D3DRECT	   g_scissorRectx2;

D3DXMATRIX g_matIdentity(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

#ifdef COOL_XDASH
UINT g_uMesh;
UINT g_uMeshRef;
#endif

extern "C" void Alert(const TCHAR* szMsg, ...)
{
	va_list args;
	va_start(args, szMsg);

	TCHAR szBuffer [512];
	_vsntprintf(szBuffer, countof(szBuffer), szMsg, args);
	Trace(_T("\007%s\n"), szBuffer);
	va_end(args);
}

#ifdef _UNICODE
void Unicode(TCHAR* wsz, const char* sz, int nMaxChars)
{
	while (nMaxChars-- > 0)
	{
		if ((*wsz++ = (unsigned char)*sz++) == 0)
			return;
	}
}

void Ansi(char* sz, const TCHAR* wsz, int nMaxChars)
{
	while (nMaxChars-- > 0)
	{
		if ((*sz++ = (char)*wsz++) == 0)
			return;
	}
}
#endif // _UNICODE

extern "C" PLAUNCH_DATA_PAGE* LaunchDataPage;

CXApp::CXApp()
{
	m_dwStartTick = m_dwFrameTick = GetTickCount();
	m_now = (float)m_dwFrameTick / 1000.0f;

	m_pD3D = NULL;
    m_pD3DDev = NULL;

	m_pViewpoint = NULL;
	m_pNavigationInfo = NULL;
	m_pBackground = NULL;

	m_worldStack = NULL;
	m_bProjectionDirty = true;

	m_bZBuffer = false;

	m_pScreen = NULL;

	m_pClass = new CClass;
	m_pObject = NULL;

	m_bStretchWidescreen = false;

	m_szAppDir = NULL;

    // Retrieve the launch data, if any
    if (*LaunchDataPage && (*LaunchDataPage)->Header.dwLaunchDataType == LDT_LAUNCH_DASHBOARD) {
        PLD_LAUNCH_DASHBOARD pLaunchDashboard =
            (PLD_LAUNCH_DASHBOARD) ((*LaunchDataPage)->LaunchData);

        m_bHasLaunchData = true;
        m_dwTitleID = (*LaunchDataPage)->Header.dwTitleId;
        m_dwLaunchReason = pLaunchDashboard->dwReason;
        m_dwLaunchContext = pLaunchDashboard->dwContext;
        m_dwLaunchParameter1 = pLaunchDashboard->dwParameter1;
        m_dwLaunchParameter2 = pLaunchDashboard->dwParameter2;

        PLAUNCH_DATA_PAGE pTemp = *LaunchDataPage;
        *LaunchDataPage = NULL;
        MmFreeContiguousMemory(pTemp);
    } else {
        m_bHasLaunchData = false;
        m_dwTitleID =
        m_dwLaunchReason =
        m_dwLaunchContext =
        m_dwLaunchParameter1 =
        m_dwLaunchParameter2 = 0;
    }

    m_dwMainThreadId = GetCurrentThreadId();
}

CXApp::~CXApp()
{
	// All cleanup code should be in CleanupApp or the debug code
	// will complain about it not happening!
}

// REVIEW: Nuke this when GetRenderState is implemented!

DWORD xapp_rgdwRenderStateCache [256];
bool xapp_rgbRenderStateCache [256];
float g_transitionMotionBlur;

void XboxInitRenderState()
{
	float zero = 0.0f;
	float one = 1.0f;

	XAppSetRenderState(D3DRS_ZENABLE, theApp.m_bZBuffer ? D3DZB_TRUE : D3DZB_FALSE);
	XAppSetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	XAppSetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	XAppSetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	XAppSetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	XAppSetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	XAppSetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	XAppSetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	XAppSetRenderState(D3DRS_ALPHAREF, 0);
	XAppSetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	XAppSetRenderState(D3DRS_DITHERENABLE, TRUE);
	XAppSetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	XAppSetRenderState(D3DRS_FOGENABLE, FALSE);
	XAppSetRenderState(D3DRS_SPECULARENABLE, FALSE);
	XAppSetRenderState(D3DRS_FOGCOLOR, 0);
	XAppSetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
//	XAppSetRenderState(D3DRS_FOGSTART, ); // Undocumented!
//	XAppSetRenderState(D3DRS_FOGEND, ); // Undocumented!
	XAppSetRenderState(D3DRS_FOGDENSITY, *(LPDWORD)&one);
	XAppSetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
	XAppSetRenderState(D3DRS_ZBIAS, 0);
	XAppSetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
	XAppSetRenderState(D3DRS_STENCILENABLE, FALSE);
	XAppSetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	XAppSetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	XAppSetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	XAppSetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	XAppSetRenderState(D3DRS_STENCILREF, 0);
	XAppSetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	XAppSetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
//	XAppSetRenderState(D3DRS_TEXTUREFACTOR, ); // Undocumented!
	XAppSetRenderState(D3DRS_WRAP0, 0);
	XAppSetRenderState(D3DRS_WRAP1, 0);
	XAppSetRenderState(D3DRS_WRAP2, 0);
	XAppSetRenderState(D3DRS_WRAP3, 0);
	XAppSetRenderState(D3DRS_LIGHTING, TRUE);
	XAppSetRenderState(D3DRS_AMBIENT, 0);
	XAppSetRenderState(D3DRS_COLORVERTEX, TRUE);
	XAppSetRenderState(D3DRS_LOCALVIEWER, TRUE);
	XAppSetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	XAppSetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	XAppSetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
	XAppSetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR2);
	XAppSetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	XAppSetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
	XAppSetRenderState(D3DRS_POINTSIZE, *(LPDWORD)&one);
	XAppSetRenderState(D3DRS_POINTSIZE_MIN, *(LPDWORD)&one);
	XAppSetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	XAppSetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	XAppSetRenderState(D3DRS_POINTSCALE_A, *(LPDWORD)&one);
	XAppSetRenderState(D3DRS_POINTSCALE_B, *(LPDWORD)&zero);
	XAppSetRenderState(D3DRS_POINTSCALE_C, *(LPDWORD)&zero);
	XAppSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	XAppSetRenderState(D3DRS_MULTISAMPLEMASK, 0xffffffff);
//	XAppSetRenderState(D3DRS_PATCHEDGESTYLE, ); // Undocumented!
//	XAppSetRenderState(D3DRS_PATCHSEGMENTS, ); // Undocumented!
//	XAppSetRenderState(D3DRS_DEBUGMONITORTOKEN, ); // Undocumented!
//	XAppSetRenderState(D3DRS_POINTSIZE_MAX, ); // Undocumented!
	XAppSetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
//	XAppSetRenderState(D3DRS_TWEENFACTOR, ); // Undocumented!

	// Initalize the motion blur
	g_transitionMotionBlur = 1.0f;
}

bool CXApp::InitD3D()
{
	m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
	if (m_pD3D == NULL)
	{
		Alert(_T("Cannot initialize Direct3D! (D3D_SDK_VERSION=%d)"), D3D_SDK_VERSION);
		return false;
	}

	D3DDEVTYPE devtype = D3DDEVTYPE_HAL;

	ZeroMemory(&m_pp, sizeof (m_pp));
	m_pp.BackBufferCount = 1;
	m_pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_pp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	//m_pp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	m_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	//m_pp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR;
	m_pp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN;
	m_pp.EnableAutoDepthStencil = m_bZBuffer;
	if (m_bZBuffer)
		m_pp.AutoDepthStencilFormat = D3DFMT_D24S8;

    DWORD dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;

    m_pp.Flags = D3DPRESENTFLAG_INTERLACED;
	
    m_bStretchWidescreen = (XGetVideoFlags() & XC_VIDEO_FLAGS_WIDESCREEN) != 0;
    if (m_bStretchWidescreen)
    {
        m_pp.Flags |= D3DPRESENTFLAG_WIDESCREEN;
    }

    switch (XGetAVPack())
    {
    default:
        m_pp.BackBufferWidth = 640;
        m_pp.BackBufferHeight = 480;
        break;

    case XC_AV_PACK_STANDARD:
    case XC_AV_PACK_SVIDEO:
    case XC_AV_PACK_RFU:
    case XC_AV_PACK_SCART:
        m_pp.BackBufferWidth = g_nDiscType == DISC_VIDEO ? 720 : 640;
        if (XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
            m_pp.BackBufferHeight = 576;
        else
            m_pp.BackBufferHeight = 480;
        break;

    case XC_AV_PACK_HDTV:
        m_pp.BackBufferWidth = g_nDiscType == DISC_VIDEO ? 720 : 640;
        m_pp.BackBufferHeight = 480;

        // If we are about to play DVD, check if 480p is enabled
#ifdef COOL_XDASH
        if (g_nDiscType == DISC_VIDEO)
#else
        if ((XBOX_480P_MACROVISION_ENABLED & XboxHardwareInfo->Flags) && g_nDiscType == DISC_VIDEO)
#endif
        {
            DWORD dwFlags, dwType;
            if (XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwFlags, 4, NULL) == ERROR_SUCCESS)
            {
                if (dwFlags & AV_FLAGS_HDTV_480p)
                {
                    m_pp.Flags &= ~D3DPRESENTFLAG_INTERLACED;
                    m_pp.Flags |= D3DPRESENTFLAG_PROGRESSIVE;
                }
            }
        }

        break;
    }

	TRACE(_T("\001\nVideo Settings: %dx%d%c %s\n"), m_pp.BackBufferWidth, m_pp.BackBufferHeight,
          m_pp.Flags & D3DPRESENTFLAG_PROGRESSIVE ? 'p' : 'i', m_bStretchWidescreen ? _T("16:9") : _T("4:3"));

	m_pp.BackBufferFormat = D3DFMT_X8R8G8B8;//D3DFMT_R5G6B5; // REVIEW: Really want D3DFMT_R8G8B8
//	m_pp.AutoDepthStencilFormat = D3DFMT_D16; // REVIEW: Really want D3DFMT_D24S8

	m_pp.hDeviceWindow = NULL;
	m_pp.Windowed = FALSE;

#ifdef _DEBUG
	m_pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_UNLIMITED;
#endif

	m_nViewWidth = (float)m_pp.BackBufferWidth;
	m_nViewHeight = (float)m_pp.BackBufferHeight;

	HRESULT hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devtype, m_pp.hDeviceWindow, dwBehavior, &m_pp, &m_pD3DDev);
	if (FAILED(hr))
	{
		LogComError(hr, "InitD3D");
		return false;
	}

	LPDIRECT3DSURFACE8 pFrontBuffer;
	D3DSURFACE_DESC d3dsd;
	m_pD3DDev->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer);
	pFrontBuffer->GetDesc(&d3dsd);
	pFrontBuffer->Release();
	g_scissorRect.x1 = 0;
	g_scissorRect.y1 = 0;
	g_scissorRect.x2 = d3dsd.Width;
	g_scissorRect.y2 = d3dsd.Height;

	m_pD3DDev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer);
	pFrontBuffer->GetDesc(&d3dsd);
	pFrontBuffer->Release();
	g_scissorRectx2.x1 = 0;
	g_scissorRectx2.y1 = 0;
	g_scissorRectx2.x2 = d3dsd.Width;
	g_scissorRectx2.y2 = d3dsd.Height;

	

#ifdef _DEBUG
	{
		D3DCAPS8 d3dCaps;

		m_pD3DDev->GetDeviceCaps(&d3dCaps);

		// Does this device support the two bump mapping blend operations?
		if ((d3dCaps.TextureOpCaps & (D3DTEXOPCAPS_BUMPENVMAP | D3DTEXOPCAPS_BUMPENVMAPLUMINANCE)) == 0)
			TRACE(_T("Device does not support bump mapping\n"));

		// Does this device support up to three blending stages?
		if (d3dCaps.MaxTextureBlendStages < 3)
			TRACE(_T("Device supports less than 3 blending stages\n"));
	}
#endif


    // REVIEW: Nuke this when GetRenderState is implemented!
	XboxInitRenderState();

	XAppSetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	XAppSetRenderState(D3DRS_AMBIENT, 0xffffffff);

	XAppSetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC);
	XAppSetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC);

	XAppSetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC);
	XAppSetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC);

	XAppSetTextureStageState(0,D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	TRACE(_T("Display sub-system initialized.\n"));

	return true;
}

void CXApp::ReleaseD3D()
{
	RELEASENULL(m_worldStack);

	if (m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

struct COLORVERTEX
{
	float dvX, dvY, dvZ;
	DWORD color;
};

void CXApp::Draw()
{
	// Setup projection transform...
	if (m_bProjectionDirty)
	{
		m_bProjectionDirty = false;

		float nNear = 0.1f;
		float nFar = 1000.0f;
		float fieldOfView = D3DX_PI / 2.0f;

		if (m_pNavigationInfo != NULL)
		{
			nNear = m_pNavigationInfo->m_avatarSize.x / 2.0f;
			if (m_pNavigationInfo->m_visibilityLimit != 0.0f)
				nFar = m_pNavigationInfo->m_visibilityLimit;
		}

		if (m_pViewpoint != NULL)
		{
			fieldOfView = m_pViewpoint->m_fieldOfView;
		}

		//float aspect = m_nViewWidth / m_nViewHeight;
		float aspect = 720.0f / 480.0f;
		if (m_bStretchWidescreen)
		{
			// This adjusts for 4x3 style resolutions being displayed widescreen on a 16x9 monitor...
			aspect *= 1.25f;
		}

		D3DXMatrixPerspectiveFovLH(&g_matProjection, fieldOfView, aspect, nNear, nFar);
		XAppSetTransform(D3DTS_PROJECTION, &g_matProjection);
	}

#ifdef COOL_XDASH
    g_uMesh = 0;
    g_uMeshRef = 0;
#endif

	XAppBeginScene();

#ifdef _DEBUG
	XAppSetRenderState(D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
#endif

	//const float blurAlpha = 0.25f; // bigger number -> less blur
	//const float blurAlpha = 1.0f; // bigger number -> less blur
	const float blurAlpha = g_transitionMotionBlur;

	D3DCOLOR color;
	bool bBackdrop = false;
	if (m_pBackground == NULL)
		color = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.3f, 1.0f);
	else
		color = D3DCOLOR_COLORVALUE(m_pBackground->m_skyColor.x, m_pBackground->m_skyColor.y, m_pBackground->m_skyColor.z, 1.0f);

	if (m_pBackground != NULL && m_pBackground->m_backdrop != NULL)
		bBackdrop = true;

	// REVIEW: don't clear the back buffer if we're going to fill it with a background anyway!
	static int nBeenHere;
	if (blurAlpha >= 1.0f || nBeenHere++ < 2 || color != D3DCOLOR_XRGB(0, 0, 0) || bBackdrop)
	{
		XAppClear(color); // NOTE: Clears back and Z buffers (if Z buffer is enabled)

		if (bBackdrop)
			m_pBackground->RenderBackdrop();
	}
	else
	{
		static LPDIRECT3DVERTEXBUFFER8 m_pVB;
		if (m_pVB == NULL)
		{
			VERIFYHR(XAppGetD3DDev()->CreateVertexBuffer(4 * sizeof(COLORVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &m_pVB));

			COLORVERTEX* verts;
			VERIFYHR(m_pVB->Lock(0, 4 * sizeof (COLORVERTEX), (BYTE**)&verts, 0));

			verts[0].dvX = (float)theApp.m_nViewWidth / 2.0f;
			verts[0].dvY = -(float)theApp.m_nViewHeight / 2.0f;
			verts[0].dvZ = 0.0f;
			verts[0].color = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, blurAlpha);

			verts[1].dvX = -(float)theApp.m_nViewWidth / 2.0f;
			verts[1].dvY = -(float)theApp.m_nViewHeight / 2.0f;
			verts[1].dvZ = 0.0f;
			verts[1].color = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, blurAlpha);

			verts[2].dvX = (float)theApp.m_nViewWidth / 2.0f;
			verts[2].dvY = (float)theApp.m_nViewHeight / 2.0f;
			verts[2].dvZ = 0.0f;
			verts[2].color = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, blurAlpha);

			verts[3].dvX = -(float)theApp.m_nViewWidth / 2.0f;
			verts[3].dvY = (float)theApp.m_nViewHeight / 2.0f;
			verts[3].dvZ = 0.0f;
			verts[3].color = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, blurAlpha);

			VERIFYHR(m_pVB->Unlock());
		}

    	D3DXMATRIX matProjection, matProjectionSave, matWorldSave, matViewSave;

    	XAppGetTransform(D3DTS_PROJECTION, &matProjectionSave);
    	XAppGetTransform(D3DTS_WORLD, &matWorldSave);
    	XAppGetTransform(D3DTS_VIEW, &matViewSave);

    	//D3DXMatrixOrthoLH(&matProjection, 720.0f, 480.0f, -10000.0f, 10000.0f);
		D3DXMatrixOrthoLH(&matProjection, theApp.m_nViewWidth, theApp.m_nViewHeight, -10000.0f, 10000.0f);
    	XAppSetTransform(D3DTS_PROJECTION, &matProjection);
    	XAppSetTransform(D3DTS_WORLD, &g_matIdentity);
    	XAppSetTransform(D3DTS_VIEW, &g_matIdentity);

        XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        XAppSetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
        XAppSetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
        XAppSetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    	XAppSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    	XAppSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        VERIFYHR(XAppGetD3DDev()->SetStreamSource(0, m_pVB, sizeof (COLORVERTEX)));
        VERIFYHR(XAppGetD3DDev()->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE));
        VERIFYHR(XAppGetD3DDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));

    	XAppSetTransform(D3DTS_PROJECTION, &matProjectionSave);
    	XAppSetTransform(D3DTS_WORLD, &matWorldSave);
    	XAppSetTransform(D3DTS_VIEW, &matViewSave);
    }

#ifdef _LIGHTS
	// Setup lights...
	{
		static int nLastLight = -1;
		int nLight = 0;
		D3DCOLORVALUE ambient;
		ambient.r = 0.0f;
		ambient.g = 0.0f;
		ambient.b = 0.0f;

		// Headlight...
		if (m_pNavigationInfo == NULL || m_pNavigationInfo->m_headlight)
		{
			XAppSetTransform(D3DTS_WORLD, &g_matIdentity);

			D3DLIGHT8 d3dLight;
			ZeroMemory(&d3dLight, sizeof(d3dLight));

			d3dLight.Type = D3DLIGHT_DIRECTIONAL;

			d3dLight.Diffuse.r = 0.5f;
			d3dLight.Diffuse.g = 0.5f;
			d3dLight.Diffuse.b = 0.5f;

			d3dLight.Specular.r = 0.75f;
			d3dLight.Specular.g = 0.75f;
			d3dLight.Specular.b = 0.75f;

			d3dLight.Ambient.r = 0.2f;
			d3dLight.Ambient.g = 0.2f;
			d3dLight.Ambient.b = 0.2f;

			ambient.g += d3dLight.Ambient.r;
			ambient.g += d3dLight.Ambient.g;
			ambient.b += d3dLight.Ambient.b;

			D3DXVECTOR3 dir(0.0f, 0.0f, -1.0f);
			D3DXVec3TransformNormal(&dir, &dir, &g_matPosition);
			d3dLight.Direction = dir;

			// BLOCK: Extra spotlight stuff...
			if (0)
			{
				d3dLight.Type = D3DLIGHT_SPOT;
				d3dLight.Position = theCamera.m_position;
				d3dLight.Range = 100.0f;
				d3dLight.Falloff = 1.0f;
				d3dLight.Attenuation0 = 1.0f;
				d3dLight.Attenuation1 = 0.0f;
				d3dLight.Attenuation2 = 0.0f;
				d3dLight.Theta = 0.1f;
				d3dLight.Phi = 0.7f;
			}

			XAppSetLight(nLight, &d3dLight);
			XAppLightEnable(nLight, true);
			nLight += 1;
		}

		m_pObject->SetLight(nLight, ambient);

		if (ambient.r < 0.0f)
			ambient.r = 0.0f;
		else if (ambient.r > 1.0f)
			ambient.r = 1.0f;
		if (ambient.g < 0.0f)
			ambient.g = 0.0f;
		else if (ambient.g > 1.0f)
			ambient.g = 1.0f;
		if (ambient.b < 0.0f)
			ambient.b = 0.0f;
		else if (ambient.b > 1.0f)
			ambient.b = 1.0f;

		XAppSetRenderState(D3DRS_AMBIENT, D3DCOLOR_COLORVALUE(ambient.r, ambient.g, ambient.b, 1.0f));

#ifdef _DEBUG
		{
			if (nLight != nLastLight)
			{
				nLastLight = nLight;
				TRACE(_T("Lighting: %s (%d lights)\n"), nLight > 0 ? _T("enabled") : _T("disabled"), nLight);
			}
		}
#endif

		// Turn off lights we are not using...
		for (int i = nLight; i < nLastLight; i += 1)
			XAppLightEnable(i, false);

		XAppSetRenderState(D3DRS_LIGHTING, nLight > 0);
		nLastLight = nLight;
	}
#else // !_LIGHTS
	XAppSetRenderState(D3DRS_LIGHTING, FALSE);
#endif // _LIGHTS

#ifndef _XBOX
	if (m_pNavigator != NULL)
		m_pNavigator->Render();
#endif

    // BLOCK: Setup a default material
	{
		static D3DMATERIAL8 mat;
		if (mat.Diffuse.r == 0.0f)
		{
			mat.Diffuse.r = 0.5f;
			mat.Diffuse.g = 0.5f;
			mat.Diffuse.b = 0.5f;
			mat.Diffuse.a = 1.0f;

			mat.Ambient.r = 0.5f;
			mat.Ambient.g = 0.5f;
			mat.Ambient.b = 0.5f;
			mat.Ambient.a = 1.0f;

			mat.Specular.r = 0.5f;
			mat.Specular.g = 0.5f;
			mat.Specular.b = 0.5f;
			mat.Specular.a = 1.0f;

			mat.Emissive.r = 0.5f;
			mat.Emissive.g = 0.5f;
			mat.Emissive.b = 0.5f;
			mat.Emissive.a = 1.0f;

			mat.Power = 0.0f;
		}

		XAppSetMaterial(&mat);
	}

//		if (m_pGround != NULL)
//			m_pGround->RenderGround();

	// Draw the world...
	{
		XAppPushWorld();
		XAppIdentityWorld();
		XAppUpdateWorld();
		m_pObject->Render();
		XAppPopWorld();
	}

	XAppEndScene();
    XAppSetRenderState(D3DRS_SWATHWIDTH, D3DSWATH_OFF);
	XAppPresent();
    XAppSetRenderState(D3DRS_SWATHWIDTH, D3DSWATH_128);

	m_dwFrameTick = GetTickCount();
}

bool CXApp::InitApp()
{
//	START_PROFILE();

	Memory_Init();
	Xbox_Init();

	srand(GetTickCount());

#ifdef _DEBUG
	Debug_Init();
#endif

	TRACE(_T("Initializing XApp...\n"));

	DiscDrive_Init();

	if (!InitD3D())
		return false;

	LoadXIP(_T("y:\\default.xip"), true);

	Class_Init();
	TitleArray_Init();
	Material_Init();

	// BLOCK: Load the appropriate class file...
	{
		TCHAR szFileToLoad [MAX_PATH];
		szFileToLoad[0] = 0;

		GetStartupClassFile(szFileToLoad);
		ASSERT(szFileToLoad[0] != 0);

		TRACE(_T("Loading %s\n"), szFileToLoad);
		if (!m_pClass->Load(szFileToLoad))
		{
			extern bool g_bParseError;
			if (!g_bParseError)
				Alert(_T("%s\n\nCannot open file."), szFileToLoad);
			return false;
		}
	}

	m_pObject = (CInstance*)m_pClass->CreateNode();
	ASSERT(m_pObject != NULL);


	VERIFYHR(D3DXCreateMatrixStack(0, &m_worldStack));
	ASSERT(m_worldStack != NULL);

	CallFunction(m_pObject, _T("initialize"));

//	END_PROFILE();
	return true;
}

void CXApp::CleanupApp()
{
	ReleaseD3D();

	delete m_pObject;
	m_pObject = NULL;

	delete m_pClass;
	m_pClass = NULL;

	Class_Exit();
	Locale_Exit();
	Text_Exit();

	DSound_Exit();

	delete [] m_szAppDir;
	m_szAppDir = NULL;

#ifdef _DEBUG
	Debug_Exit();
#endif
}

void CXApp::Advance()
{
	XTIME now = (float)GetTickCount() / 1000.0f;
	float nDelta = (float) (now - m_now);
	if (nDelta == 0.0f)
		nDelta = 0.001f;
	m_now = now;

	ASSERT(g_nEffectAlpha == 1.0f); // this should always have been restored here!

#ifndef _XBOX
	if (m_pNavigator != NULL)
		m_pNavigator->Advance(nDelta);
#endif

	theCamera.Advance(nDelta);

	CLerper::AdvanceAll();
	m_pObject->Advance(nDelta);

	BackgroundLoader_Frame();

#ifdef _DEBUG
	Debug_Frame();
#endif
}

void XApp()
{
	if (!theApp.InitApp())
	{
		TRACE(_T("\001Initialization failed; exiting...\n"));
		theApp.CleanupApp();
		return;
	}

	TRACE(_T("Initialization completed...\n"));

	for (;;)
	{
		theApp.Advance();
		theApp.Draw();
	}

	theApp.CleanupApp();
}

void CXApp::GetStartupClassFile(TCHAR* szFileToLoad)
{
	TCHAR szAppDir [MAX_PATH];

	_tcscpy(szFileToLoad, _T("y:\\default.xap"));
	_tcscpy(szAppDir, _T("y:\\"));
	_tcscpy(g_szCurDir, _T("y:/default.xap"));

	m_szAppDir = new TCHAR [_tcslen(szAppDir) + 1];
	_tcscpy(m_szAppDir, szAppDir);
}

LONG XdashUnhandledExceptionFilter(LPEXCEPTION_POINTERS *pEx)
{
    HalReturnToFirmware(HalRebootRoutine);
    __asm { hlt }   // Shouldn't be here
}

void __cdecl main(int /*argc*/, char* /*argv*/ [])
{
#ifndef DEVKIT
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)XdashUnhandledExceptionFilter);
#endif
	XApp();
}

