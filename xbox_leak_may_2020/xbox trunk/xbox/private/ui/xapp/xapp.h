// XApp.h -- Main Declarations for XApp
//

// Represent time values as a 'double' for good fractional precision after
// a large period of time (days).  The really proper fix would be to represent
// time using a DWORD, and convert to floating point after computing an
// age, but we don't have time to do that...
typedef double XTIME;

#define MAX_BLOCKS_TO_SHOW 50000

#define CNode CXAppNode // REVIEW: Name collision with D3DX...
#define classCNode classCXAppNode // REVIEW: Name collision with D3DX...

class CObject;
class CClass;
class CInstance;
class CNode;
class CNodeArray;
class CScreen;
class CViewpoint;
class CNavigationInfo;
class CNavigator;
class CBackground;
class CXApp;

////////////////////////////////////////////////////////////////////////////

class CXApp
{
public:
    CXApp();
    ~CXApp();

	bool InitApp();
	void CleanupApp();
	void GetStartupClassFile(TCHAR* szFileToLoad);

#if defined(_WINDOWS)
	HINSTANCE m_hInstance;
    HWND m_hWnd;
	TCHAR* m_szAppTitle;
	bool m_bFullScreen;
	int m_nCmdShow;
	bool m_bDrawFramerate;

	int m_nVertPerFrame;
	int m_nTriPerFrame;

	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	bool InitWindow();
	void OnSize(int nWidth, int nHeight);
	bool ParseCmdLine();
#endif

	void Advance();
    void Draw();

	D3DPRESENT_PARAMETERS m_pp;
    LPDIRECT3DDEVICE8 m_pD3DDev;
	LPDIRECT3D8 m_pD3D;
	bool InitD3D();
	void ReleaseD3D();

	bool m_bStretchWidescreen;
	float m_nViewWidth;
	float m_nViewHeight;

	bool m_bZBuffer;
	bool m_bProjectionDirty;
	ID3DXMatrixStack* m_worldStack;

	DWORD m_dwStartTick;
	DWORD m_dwFrameTick;
	XTIME m_now;

	// The XApplication Objects...
	CClass* m_pClass;
	CInstance* m_pObject;

	// Bound Nodes...
	CScreen* m_pScreen;
	CViewpoint* m_pViewpoint;
	CNavigationInfo* m_pNavigationInfo;
	CBackground* m_pBackground;

#ifdef _WINDOWS
	CNode* m_pGround;
	CNavigator* m_pNavigator;
#endif

	TCHAR* m_szAppDir;

    // Dashboard launch data
    bool m_bHasLaunchData;
    DWORD m_dwTitleID;
    DWORD m_dwLaunchReason;
    DWORD m_dwLaunchContext;
    DWORD m_dwLaunchParameter1;
    DWORD m_dwLaunchParameter2;

    DWORD m_dwMainThreadId;
};

extern CXApp theApp;

////////////////////////////////////////////////////////////////////////////

inline XTIME XAppGetNow() { return theApp.m_now; }

#ifdef _WINDOWS
inline HINSTANCE XAppGetInstanceHandle() { return theApp.m_hInstance; }
int XAppMessageBox(const TCHAR* szText, UINT uType = MB_OK);
#endif


#ifdef _DEBUG
void XAppGetErrorString(HRESULT hr, TCHAR* szBuf, int cchBuf);
const TCHAR* XAppGetErrorString(HRESULT hr);
void LogComError(HRESULT hr, const char* szFunc = NULL);
void LogError(const char* szFunc);
#else
inline void LogComError(HRESULT hr, const char* szFunc = NULL) {}
inline void LogError(const char* szFunc) {}
#endif


void MakeAbsoluteURL(TCHAR* szBuf, const TCHAR* szBase, const TCHAR* szURL);
void MakeAbsoluteURL(TCHAR* szBuf, const TCHAR* szURL);
void UpdateCurDirFromFile(const TCHAR* szURL);

void* XAppAllocMemory(int nBytes);
void XAppFreeMemory(void* pv);

#ifdef _XBOX
void* XAppD3D_AllocContiguousMemory(DWORD Size, DWORD Alignment);
void* XAppD3D_AllocNoncontiguousMemory(DWORD Size);
#endif

////////////////////////////////////////////////////////////////////////////
// XApp Direct 3D Device Interfaces

#ifdef _XBOX // REVIEW: Nuke this when GetRenderState is implemented!
extern DWORD xapp_rgdwRenderStateCache [256];
extern bool xapp_rgbRenderStateCache [256];
#endif

inline LPDIRECT3DDEVICE8 XAppGetD3DDev()
{
	return theApp.m_pD3DDev;
}

inline void XAppSetRenderState(D3DRENDERSTATETYPE dwRenderStateType, DWORD dwRenderState)
{
#ifdef _XBOX // REVIEW: Nuke this when GetRenderState is implemented!
	ASSERT((UINT)dwRenderStateType < countof (xapp_rgdwRenderStateCache));
	xapp_rgdwRenderStateCache[(UINT)dwRenderStateType] = dwRenderState;
	xapp_rgbRenderStateCache[(UINT)dwRenderStateType] = true;
#endif

	VERIFYHR(XAppGetD3DDev()->SetRenderState(dwRenderStateType, dwRenderState));
}

inline void XAppGetRenderState(D3DRENDERSTATETYPE dwRenderStateType, LPDWORD lpdwRenderState)
{
#ifdef _XBOX // REVIEW: Nuke this when GetRenderState is implemented!
	ASSERT((UINT)dwRenderStateType < countof (xapp_rgdwRenderStateCache));
	ASSERT(xapp_rgbRenderStateCache[(UINT)dwRenderStateType]); // we don't know the state!
	*lpdwRenderState = xapp_rgdwRenderStateCache[(UINT)dwRenderStateType];
#else
	VERIFYHR(XAppGetD3DDev()->GetRenderState(dwRenderStateType, lpdwRenderState));
#endif
}

inline void XAppSetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, DWORD dwValue)
{
	VERIFYHR(XAppGetD3DDev()->SetTextureStageState(dwStage, dwState, dwValue));
}

inline void XAppGetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, LPDWORD lpdwValue)
{
	VERIFYHR(XAppGetD3DDev()->GetTextureStageState(dwStage, dwState, lpdwValue));
}

inline void XAppSetTexture(DWORD dwStage, LPDIRECT3DTEXTURE8 lpTexture)
{
	VERIFYHR(XAppGetD3DDev()->SetTexture(dwStage, lpTexture));
}

inline void XAppSetMaterial(D3DMATERIAL8* lpMaterial)
{
	VERIFYHR(XAppGetD3DDev()->SetMaterial(lpMaterial));
}

inline void XAppSetTransform(D3DTRANSFORMSTATETYPE dtstTransformStateType, D3DMATRIX* lpD3DMatrix)
{
	VERIFYHR(XAppGetD3DDev()->SetTransform(dtstTransformStateType, lpD3DMatrix));
}

inline void XAppGetTransform(D3DTRANSFORMSTATETYPE dtstTransformStateType, D3DMATRIX* lpD3DMatrix)
{
	VERIFYHR(XAppGetD3DDev()->GetTransform(dtstTransformStateType, lpD3DMatrix));
}

#ifndef _XBOX
inline void XAppSetClipPlane(DWORD dwIndex, D3DVALUE* pPlaneEquation)
{
#ifndef _XBOX
	VERIFYHR(XAppGetD3DDev()->SetClipPlane(dwIndex, pPlaneEquation));
#endif
}
#endif // _XBOX

#ifdef _LIGHTS
inline void XAppSetLight(DWORD dwLightIndex, D3DLIGHT8* lpLight)
{
	VERIFYHR(XAppGetD3DDev()->SetLight(dwLightIndex, lpLight));
}

inline void XAppLightEnable(DWORD dwLightIndex, bool bEnable)
{
	VERIFYHR(XAppGetD3DDev()->LightEnable(dwLightIndex, bEnable));
}
#endif

inline void XAppClear(D3DCOLOR color)
{
	DWORD dwFlags = D3DCLEAR_TARGET;

	if (theApp.m_bZBuffer)
		dwFlags |= D3DCLEAR_ZBUFFER;

	VERIFYHR(XAppGetD3DDev()->Clear(0, NULL, dwFlags, color, 1.0f, 0));
}

inline void XAppBeginScene()
{
	VERIFYHR(XAppGetD3DDev()->BeginScene());
}

inline void XAppEndScene()
{
	VERIFYHR(XAppGetD3DDev()->EndScene());
}

inline void XAppPresent()
{
	VERIFYHR(XAppGetD3DDev()->Present(NULL, NULL, NULL, NULL));
}

inline void XAppGetTextureSize(LPDIRECT3DTEXTURE8 pTexture, int& nWidth, int& nHeight)
{
	D3DSURFACE_DESC sd;
	VERIFYHR(pTexture->GetLevelDesc(0, &sd));
	nWidth = (int)sd.Width;
	nHeight = (int)sd.Height;
}

inline void XAppGetTextureSize(LPDIRECT3DTEXTURE8 pTexture, float& nWidth, float& nHeight)
{
	int iWidth, iHeight;
	XAppGetTextureSize(pTexture, iWidth, iHeight);
	nWidth = (float)iWidth;
	nHeight = (float)iHeight;
}

inline void XAppSetVertexShader(DWORD Handle)
{
	VERIFYHR(XAppGetD3DDev()->SetVertexShader(Handle));
}

inline void XAppSetVertexShaderConstant(DWORD Register, const void* pConstantData, DWORD ConstantCount)
{
	VERIFYHR(XAppGetD3DDev()->SetVertexShaderConstant(Register, pConstantData, ConstantCount));
}

inline void XAppSetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8* pStreamData, UINT Stride)
{
	VERIFYHR(XAppGetD3DDev()->SetStreamSource(StreamNumber, pStreamData, Stride));
}

inline void XAppSetIndices(IDirect3DIndexBuffer8* pIndexData, UINT BaseVertexIndex)
{
	VERIFYHR(XAppGetD3DDev()->SetIndices(pIndexData, BaseVertexIndex));
}

inline void XAppDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	VERIFYHR(XAppGetD3DDev()->DrawIndexedPrimitive(Type, MinIndex, NumVertices, StartIndex, PrimitiveCount));
}

extern int __cdecl NewFailed(size_t nBytes);

inline void XAppCreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer8** ppIndexBuffer)
{
	HRESULT hr;

	do
	{
		hr = XAppGetD3DDev()->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer);
		if (hr != E_OUTOFMEMORY)
			break;
	}
	while (NewFailed(Length) != 0);

	VERIFYHR(hr);
}

inline void XAppCreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer8** ppVertexBuffer)
{
	HRESULT hr;

	do
	{
		hr = XAppGetD3DDev()->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer);
		if (hr != E_OUTOFMEMORY)
			break;
	}
	while (NewFailed(Length) != 0);

	VERIFYHR(hr);
}

inline void XAppCreateVertexShader(CONST DWORD* pDeclaration, CONST DWORD* pFunction, DWORD* pHandle, DWORD Usage)
{
	VERIFYHR(XAppGetD3DDev()->CreateVertexShader(pDeclaration, pFunction, pHandle, Usage));
}

////////////////////////////////////////////////////////////////////////////
// World Matrix Stack

inline D3DXMATRIX* XAppGetWorld()
{
	ASSERT(theApp.m_worldStack != NULL);
	return theApp.m_worldStack->GetTop();
}

extern void SetFalloffShaderFrameValues();

inline void XAppUpdateWorld()
{
	ASSERT(theApp.m_worldStack != NULL);
	XAppSetTransform(D3DTS_WORLD, XAppGetWorld());
	SetFalloffShaderFrameValues();
}

inline void XAppPushWorld()
{
	ASSERT(theApp.m_worldStack != NULL);
	VERIFYHR(theApp.m_worldStack->Push());
}

inline void XAppPopWorld()
{
	ASSERT(theApp.m_worldStack != NULL);
	VERIFYHR(theApp.m_worldStack->Pop());
	XAppUpdateWorld();
}

inline void XAppMultWorld(const D3DXMATRIX* pMat)
{
	ASSERT(theApp.m_worldStack != NULL);
	VERIFYHR(theApp.m_worldStack->MultMatrixLocal(pMat));
}

inline void XAppTranslateWorld(float x, float y, float z)
{
	ASSERT(theApp.m_worldStack != NULL);
	VERIFYHR(theApp.m_worldStack->TranslateLocal(x, y, z));
}

inline void XAppRotateWorld(const D3DXVECTOR3* pV, float angle)
{
	ASSERT(theApp.m_worldStack != NULL);
	VERIFYHR(theApp.m_worldStack->RotateAxisLocal(pV, angle));
}

inline void XAppIdentityWorld()
{
	ASSERT(theApp.m_worldStack != NULL);
	VERIFYHR(theApp.m_worldStack->LoadIdentity());
}

#ifndef _XBOX
#undef D3DLOCK_DISCARD
#define D3DLOCK_DISCARD 0
#endif

////////////////////////////////////////////////////////////////////////////

inline float pos(float n)
{
	if (n > 0.0f)
		return n;

	return 0.0f;
}

inline float rnd(float n)
{
	return (float)rand() * n / 32767.0f;
}

inline float wrap(float n)
{
	return n - floorf(n);
}

#define trunc(n) ((int)(n))


inline int clamp(int x, int a, int b)
{
	return (x < a ? a : (x > b ? b : x));
}

inline float clampf(float x, float a, float b)
{
	return (x < a ? a : (x > b ? b : x));
}

////////////////////////////////////////////////////////////////////////////

extern bool CallFunction(CObject* pObject, const TCHAR* szFunc, int nParam = 0, CObject** rgParam = NULL);

////////////////////////////////////////////////////////////////////////////

extern TCHAR g_szCurDir [];

class CDirPush
{
public:
	CDirPush(const TCHAR* szFile = NULL)
	{
        ASSERT(theApp.m_dwMainThreadId == GetCurrentThreadId());
		_tcscpy(m_szCurDirSav, g_szCurDir);

		if (szFile != NULL)
		{
			UpdateCurDirFromFile(szFile);
		}
	}

	~CDirPush()
	{
        ASSERT(theApp.m_dwMainThreadId == GetCurrentThreadId());
		_tcscpy(g_szCurDir, m_szCurDirSav);
//		TRACE(_T("Resetting base directory to %s\n"), g_szCurDir);
	}

	TCHAR m_szCurDirSav [1024];
};

////////////////////////////////////////////////////////////////////////////

extern void CleanFilePath(FSCHAR* szPath, const TCHAR* szSrcPath);

inline HANDLE XAppCreateFile(const TCHAR* szFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes = 0, HANDLE hTemplateFile = NULL)
{
	FSCHAR sszFileName [MAX_PATH];
	CleanFilePath(sszFileName, szFileName);
	return CreateFile(sszFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

inline bool XAppCreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL)
{
	FSCHAR sszFileName [MAX_PATH];
	CleanFilePath(sszFileName, lpPathName);
	return CreateDirectory(sszFileName, lpSecurityAttributes) != FALSE;
}

////////////////////////////////////////////////////////////////////////////

const CHAR XappTempPcmFileA[]  =  "t:\\tempcda.cda";
const WCHAR XappTempPcmFileW[] = L"t:\\tempcda.cda";
const CHAR XappTempWmaFileA[]  =  "t:\\tempwma.wma";
const WCHAR XappTempWmaFileW[] = L"t:\\tempwma.wma";

#ifdef _UNICODE
#define XappTempPcmFile XappTempPcmFileW
#else  // _UNICODE
#define XappTempPcmFile XappTempPcmFileA
#endif // _UNICODE

////////////////////////////////////////////////////////////////////////////

extern int g_nDiscType;

#define DISC_NONE   0
#define DISC_BAD    1
#define DISC_TITLE  2
#define DISC_AUDIO  3
#define DISC_VIDEO  4

////////////////////////////////////////////////////////////////////////////

extern bool ResetScreenSaver();

////////////////////////////////////////////////////////////////////////////

#define D3DFVF_NORMPACKED3		0x20000000

extern DWORD CompressNormal(float* pvNormal);

////////////////////////////////////////////////////////////////////////////

extern DWORD GetFixedFunctionShader(DWORD fvf);
