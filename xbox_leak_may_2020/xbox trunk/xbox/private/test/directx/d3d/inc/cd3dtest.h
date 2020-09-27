/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    cd3dtest.h

Author:

    Matt Bronder

Description:

    Framework library to facilitate the porting of Direct3D tests from the
    DirectX test team.

*******************************************************************************/

#ifndef __CD3DTEST_H__
#define __CD3DTEST_H__

#include "..\..\inc\dxlegacy.h"

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define CIL_MAKEPCT(x) ((float)x/255.0f)
#define CIL_MAKEDWORD(x) ((DWORD)(x * 255.0f))
#define CIL_RED(x) ( (x >> 16) & 0xFF)
#define CIL_GREEN(x) ( (x >> 8) & 0xFF)
#define CIL_BLUE(x) ( x & 0xFF)
#define CIL_ALPHA(x) ( (x >> 24) & 0xFF)

#define CIL_UNSIGNED 0x00000001

#define ALPHA_CHANNEL   3
#define   RED_CHANNEL   2
#define GREEN_CHANNEL   1
#define  BLUE_CHANNEL   0

#define D3DDP_DONOTCLIP             0x00000004l
#define D3DDP_DONOTUPDATEEXTENTS    0x00000008l
#define D3DDP_DONOTLIGHT            0x00000010l

//#define D3DFVF_RESERVED0        0x001
//#define D3DFVF_RESERVED1        0x020
//#define D3DFVF_RESERVED2        0xf000  // 4 reserved bits

#define MAX_D3D_TEXTURES        50
#define MAX_D3D_ZBUFFERS        20

// Additional constants for dwFlags argument of rendering functions
#define CD3D_SRC    0x80000000
#define CD3D_REF    0x40000000
#define CD3D_BOTH   (CD3D_SRC | CD3D_REF)

// Constants for texture coordinates size calculations
#define CDIRECT3D_TEXTUREFORMATS_SHIFT  16      // Number of bits to shift to get texture format descriptions
#define CDIRECT3D_TEXTUREFORMAT_MASK    0x03    // Mask to extract texture format description
#define CDIRECT3D_TEXTUREFORMAT_SIZE    2       // Number of bits for one texture format description

// Framework defines for Texture Filter type
#define TEXF_NONE                             0
#define TEXF_POINT                            1
#define TEXF_LINEAR                           2
#define TEXF_ANISOTROPIC                      3
#define TEXF_FLATCUBIC                        4
#define TEXF_GAUSSIANCUBIC                    5

// Release macro
#define RELEASE(x) { if(NULL != (x)) {(x)->Release(); (x)=NULL;} }

#define DECLARE(X)  X;
#define TESTFRAME(X) X

#define D3DVAL(x)   ((float)x)

// Legacy render state
#define D3DRENDERSTATE_TEXTUREMAPBLEND  ((D3DRENDERSTATETYPE)0x0DEF)

// Legacy texture blend caps
#define D3DPTBLENDCAPS_ADD              0x0001
#define D3DPTBLENDCAPS_COPY             0x0002
#define D3DPTBLENDCAPS_DECAL            0x0004
#define D3DPTBLENDCAPS_DECALALPHA       0x0008
#define D3DPTBLENDCAPS_DECALMASK        0x0010
#define D3DPTBLENDCAPS_MODULATE         0x0020
#define D3DPTBLENDCAPS_MODULATEALPHA    0x0040
#define D3DPTBLENDCAPS_MODULATEMASK     0x0080

//******************************************************************************
// Data types
//******************************************************************************

#ifdef UNDER_XBOX
typedef UCHAR UINT8;
typedef CHAR  INT8;
#endif // UNDER_XBOX

const float pi = 3.141592654f;

typedef enum _LEGACY_D3DTEXTUREBLEND { 
    D3DTBLEND_DECAL         = 1, 
    D3DTBLEND_MODULATE      = 2, 
    D3DTBLEND_DECALALPHA    = 3, 
    D3DTBLEND_MODULATEALPHA = 4, 
//    D3DTBLEND_DECALMASK     = 5, 
//    D3DTBLEND_MODULATEMASK  = 6, 
    D3DTBLEND_COPY          = 7, 
    D3DTBLEND_ADD           = 8, 
    D3DTBLEND_MODULATE_ALPHATEXTURE = 9, 
    D3DTBLEND_FORCE_DWORD   = 0x7fffffff, 
} LEGACY_D3DTEXTUREBLEND;

typedef struct _TRANGE {
    UINT            uStart;
    UINT            uStop;
    struct _TRANGE* ptrNext;
} TRANGE, *PTRANGE;

enum
{
    D3DTESTINIT_RUN=0,
    D3DTESTINIT_ABORT=1,
    D3DTESTINIT_SKIPALL=2,
};

enum
{
    D3DTESTFINISH_SKIPREMAINDER=0,
    D3DTESTFINISH_ABORT=1,
};

typedef struct
{
    bool    	fAllDevices;    // Specific to the primary
    bool    	fMultimon;      // Doing multi-mon is ok.
    bool    	fTextures;      // This test uses textures
    bool    	fCubemaps;      // This test uses cubemaps
    bool    	fVolumes;       // This test uses volumes
    DWORD       dwTexLoad;      // Enables different texture loading options
    bool    	fZBuffer;       // This test uses a Z Buffer
	bool		bFilterRes;		// Filter the resolutions to Common list
	DWORD		dwWinMode;		// Enables different windowing mode(full, etc)
    UINT    	uStencil;       // Requested Stencil format
    UINT    	uMinZBuffer;    // Minimum number of Z buffer bits
    UINT    	uMinTextureBits;// Minimum number of texture bits
    bool    	fFOURCCTexture; // FOURCC textures ok?
    bool    	fYUVTexture;    // YUV specific texture ok?
    bool    	fStress;        // Primarily a stress application?
    UINT    	uMinDXVersion;  // Minimum DX version required
    UINT    	uMaxDXVersion;  // Minimum DX version 
    bool    	fMultiThread;   // Enable multithread cycling
    bool    	fAntialias;     // Add HINTANTIALIAS during surface creation
    bool    	fReference;     // Does the test need reference device?
	bool		fAPI;			// API test flag
	bool		fRefEqualsSrc;  // Set the Ref = Src
	DWORD		dwDeviceTypes;  // Enable multiple HALs in conformance mode
	bool		bSWDevices;		// Enumerate software devices on SRC
    bool		bHWDevices;		// Enumerate hardware devices on SRC
	GUID		DefaultRef;		// Set the default reference device
	bool		bFlipVersions;	// Run the test on all supported versions
    bool		bConstructGroupNames;	// Generate group names(add device)
	bool		bFlipMultiSampleTypes;	// Run the test with different multisample types
} MODELISTOPTION, *PMODELISTOPTION;

typedef enum _RENDERPRMITIVEAPI // (Un/)Supported APIs
{
    RP_BE               = 0,    // Begin/Vertex/End
    RP_BIE              = 1,    // BeginIndexed/Index/End
    RP_DP               = 2,    // DrawPrimitive
    RP_DIP              = 3,    // DrawIndexedPrimitive
    RP_DPS              = 4,    // DrawPrimitiveStrided
    RP_DIPS             = 5,    // DrawIndexedPrimitiveStrided
    RP_DPVB             = 6,    // DrawPrimitiveVB
    RP_DIPVB            = 7,    // DrawIndexedPrimitiveVB
    RP_DPVB_PV          = 8,    // DrawPrimitiveVB after ProcessVertices
    RP_DIPVB_PV         = 9,    // DrawIndexedPrimitiveVB after ProcessVertices
    RP_DPVB_OPT         = 10,   // DrawPrimitiveVB with optimized vertex buffer
    RP_DIPVB_OPT        = 11,   // DrawIndexedPrimitiveVB with optimized vertex buffer
    RP_DPVB_OPT_PV      = 12,   // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DIPVB_OPT_PV     = 13,   // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DPVB_PVS         = 14,   // DrawPrimitiveVB after ProcessVerticesStrided
    RP_DIPVB_PVS        = 15,   // DrawIndexedPrimitiveVB after ProcessVerticesStrided
    RP_FORCE_DWORD      = 0x7fffffff
} RENDERPRIMITIVEAPI, *PRENDERPRIMITIVEAPI;

typedef FLOAT (*_fChannelFunction)(FLOAT u, FLOAT v);

typedef struct _ChannelData
{   
    DWORD dwAlpha;
    DWORD dwRed;
    DWORD dwGreen;
    DWORD dwBlue;
    _fChannelFunction fAlpha;
    _fChannelFunction fRed;
    _fChannelFunction fGreen;
    _fChannelFunction fBlue;
} CHANNELDATA, *PCHANNELDATA;

typedef float D3DVALUE;

//******************************************************************************
class CD3DTest : public CScene {

protected:

    PTRANGE                         m_ptrSkipList;
    D3DCAPS8                        m_d3dcaps;
    MODELISTOPTION                  m_ModeOptions;
    bool                            m_fInTestCase;
    DWORD                           m_dwNumFailed;

    HRESULT                         m_hrLastError;

    DWORD                           m_dwVersion;

    LPCTSTR                         m_szTestName;       // Short name used to describe the test
    LPCTSTR                         m_szCommandKey;     // Command key used to manually set test ranges

    UINT                            m_uLastTestNum;
    bool                            m_fIncrementForward;

    RENDERPRIMITIVEAPI              m_dwRPCurrentAPI;
    DWORD                           m_dwRPAPICyclingFrequency;
    PRENDERPRIMITIVEAPI             m_pRPAPIList;                   // Pointer to an array of APIs to cycle
    DWORD                           m_dwRPAPIListSize;              // Size of this array
    DWORD                           m_dwRPAPICyclingIndex;

    float                           m_fPassPercentage;

    static RENDERPRIMITIVEAPI       m_pRPDefaultAPIList[6];
    static RENDERPRIMITIVEAPI       m_pRPFullAPIList[16];
    static RENDERPRIMITIVEAPI       m_pRPFullDX5APIList[4];
    static RENDERPRIMITIVEAPI       m_pRPFullDX6APIList[14];
    static RENDERPRIMITIVEAPI       m_pRPFullDX7APIList[14];
    static RENDERPRIMITIVEAPI       m_pRPFullDX8APIList[6];
    static DWORD                    m_dwRPDefaultAPIListSize;
    static DWORD                    m_dwRPFullAPIListSize;
    static DWORD                    m_dwRPFullDX5APIListSize;
    static DWORD                    m_dwRPFullDX6APIListSize;
    static DWORD                    m_dwRPFullDX7APIListSize;
    static DWORD                    m_dwRPFullDX8APIListSize;

    LPVOID                          m_pRPVertexArray;               // Pointer to the internal buffer used in API conversion
    DWORD                           m_dwRPVertexArraySize;          // Size (in bytes) of this array
    LPWORD                          m_pRPIndexArray;                // Pointer to the internal buffer used in API conversion
    DWORD                           m_dwRPIndexArraySize;           // Size (in WORDs) of this array

    DWORD                           m_dwClearColor;
    UINT                            m_uCommonTextureFormats;
    D3DFORMAT                       m_fmtCommon[17];
    UINT                            m_uCommonZBufferFormats;
    D3DFORMAT                       m_fmtdCommon[17];
    UINT                            m_uCommonVolumeFormats;
    D3DFORMAT                       m_fmtvCommon[17];
    UINT                            m_uCommonTargetFormats;
    D3DFORMAT                       m_fmtrCommon[17];

    CDirect3D8*                     m_pD3D8;
    CDevice8*                       m_pSrcDevice8;

    D3DVIEWPORT8                    m_vpFull;
    D3DVIEWPORT8                    m_vpTest;

public:

                                    CD3DTest();
                                    ~CD3DTest();

//    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Initialize();
    virtual void                    Efface();
//    virtual void                    Update(float fTime);
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);

    virtual bool                    SetDefaultMatrices() { return true; };
    virtual bool                    SetDefaultMaterials() { return true; };
    virtual bool                    SetDefaultLights() { return true; };
    virtual bool                    SetDefaultLightStates() { return true; };
    virtual bool                    SetDefaultRenderStates() { return true; };

    virtual UINT                    TestInitialize() { return D3DTESTINIT_RUN; };
    virtual bool                    TestTerminate() { return true; };
    virtual void                    TestFinish(UINT uState) {};
    virtual bool                    ExecuteTest(UINT) { return false; };
    virtual void                    SceneRefresh(void) {};
    virtual bool                    ProcessFrame(void) { return true; };
    virtual bool                    CompareImages(void) { return true; };
    virtual bool                    GetCompareResult(float fColorVariation, float fColorRange = 0.78f, UINT uMinPixels=1, UINT uMax = INT_MAX);
    virtual bool                    KeySet(LPSTR pKey, bool bRemove=false) { return false; };
    virtual bool                    KeySet(LPWSTR pKey, bool bRemove=false) { return false; };

    virtual bool                    SetTestRange(UINT uStart, UINT uStop);
    virtual bool                    AddTestRange(UINT,UINT,bool fConstraint = false);
    virtual bool                    AddTest(UINT uTest);
    virtual bool                    SkipTests(UINT uCount);
    virtual void                    ClearRangeList();
    virtual void                    Pass(void);
    virtual void                    Fail(void);
    virtual void                    Abort(void);
    virtual void                    BeginTestCase(LPCSTR szTestCase, UINT uTestNumber = 0);
    virtual void                    BeginTestCase(LPCWSTR szTestCase, UINT uTestNumber = 0);
    virtual void                    EndTestCase(void);
    virtual void                    WriteToLog(LPCSTR szFormat, ...);
    virtual void                    WriteToLog(LPCWSTR szFormat, ...);
    virtual void                    WriteToLog(LOGLEVEL ll, LPCSTR szFormat, ...);
    virtual void                    WriteToLog(LOGLEVEL ll, LPCWSTR szFormat, ...);
    virtual void                    WriteStatus(const char *ptcsKey, const char *ptcsFmt,...) {};
    virtual void                    WriteStatus(const WCHAR *ptcsKey, const WCHAR *ptcsFmt,...) {};
    virtual void                    WriteStatus(int nLevel, const char *ptcsKey, const char *ptcsFmt,...) {};
    virtual void                    WriteStatus(int nLevel, const WCHAR *ptcsKey, const WCHAR *ptcsFmt,...) {};
    virtual void                    ReadInteger(LPSTR pKey, int iDefault, int * szValue, BOOL bRemove = FALSE);
    virtual void                    ReadInteger(LPWSTR pKey, int iDefault, int * szValue, BOOL bRemove = FALSE);

    virtual HRESULT                 GetLastError(void);
    virtual bool                    SetLastError(HRESULT);

    virtual bool                    BeginScene(void);
    virtual bool                    EndScene(void);
    virtual bool                    ClearFrame(void);
    virtual bool                    Clear(D3DCOLOR c);
#ifndef UNDER_XBOX
    virtual bool                    Clear(DWORD dwRGB, float fZ, DWORD dwStencil = 0, DWORD dwFlags = (0x01 | 0x02 | 0x04) /*D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL | D3DCLEAR_TARGET*/, DWORD dwCount=0, D3DRECT* lpRects=NULL);
#else
    virtual bool                    Clear(DWORD dwRGB, float fZ, DWORD dwStencil = 0, DWORD dwFlags = (0x000000f0l | 0x00000001l | 0x00000002l) /*D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL | D3DCLEAR_TARGET*/, DWORD dwCount=0, D3DRECT* lpRects=NULL);
#endif
    virtual bool                    SetTexture(int nIndex, CBaseTexture8* pTextures, DWORD dwFlags = 0);
    virtual bool                    SetRenderState(D3DRENDERSTATETYPE Type, DWORD dwValue, DWORD dwFlags = 0);
    virtual bool                    SetTransform(D3DTRANSFORMSTATETYPE Type, D3DMATRIX* Matrix, DWORD dwFlags = 0);
    virtual bool                    SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue, DWORD dwFlags = 0);
    virtual bool                    GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pdwSrcValue = NULL, DWORD *pdwRefValue = NULL, DWORD dwFlags = 0);
    virtual bool                    SetViewport(UINT nX, UINT nY, UINT nWidth, UINT nHeight, float fMinZ=0.0f, float fMaxZ=1.0f, DWORD dwFlags = CD3D_BOTH);
    virtual bool                    SetClipPlane(DWORD dwIndex, float* pPlane, DWORD dwFlags = CD3D_BOTH);
    virtual bool                    SetVertexShader(DWORD dwHandle, DWORD dwFlags = CD3D_BOTH);
    virtual bool                    SetStreamSource(DWORD dwStream, CVertexBuffer8 *pSrcVB, CVertexBuffer8 *pRefVB, DWORD dwStride, DWORD dwFlags);
    virtual bool                    SetIndices(CIndexBuffer8 *pSrcIB, CIndexBuffer8 *pRefIB, DWORD dwBaseVertexIndex, DWORD dwFlags);
    virtual bool                    LightEnable(DWORD dwLightNum, DWORD dwEnable, DWORD dwFlags = CD3D_BOTH);
    virtual bool                    CreateVertexShader(LPDWORD pdwDeclaration, LPDWORD pdwFunction, LPDWORD pdwHandle, DWORD dwUsage, DWORD dwFlags = CD3D_BOTH);
    virtual bool                    DeleteVertexShader(DWORD dwHandle);
    virtual bool                    SetVertexShaderConstant(DWORD dwRegister, LPVOID lpvConstantData, DWORD dwConstantCount, DWORD dwFlags = CD3D_BOTH);
    virtual bool                    ValidateDevice(HRESULT *phSrcResult = NULL, DWORD *pdwSrcPasses = NULL, HRESULT *phRefResult = NULL, DWORD *pdwRefPasses = NULL);
    virtual bool                    SetupViewport(void);

    virtual void                    SetAPI(RENDERPRIMITIVEAPI dwAPI);
    virtual void                    SetAPI(PRENDERPRIMITIVEAPI pAPIList, DWORD dwAPIListSize, DWORD dwAPICyclingFrequency);
    virtual void                    SwitchAPI(DWORD dwTestNumber);
    virtual RENDERPRIMITIVEAPI      GetAPI(void);
    virtual DWORD                   RemapAPI(DWORD dwCurrentAPI, D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexTypeDesc);

    virtual bool                    RenderPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexTypeDesc,
                                       LPVOID lpvVertices, DWORD dwVertexCount,
                                       LPWORD lpwIndices = NULL, DWORD dwIndexCount = 0, DWORD dwFlags = CD3D_BOTH);

    virtual DWORD                   GetVertexSize(DWORD dwVertexTypeDesc);
    virtual LPVOID                  CreateVertexArray(LPVOID lpvVertices, DWORD  dwVertexCount, DWORD dwVertexSize, LPWORD lpwIndices, DWORD  dwIndexCount);
    virtual LPWORD                  CreateIndexArray(DWORD dwSize);

    virtual CTexture8*              CreateCommonTexture(CDevice8* pDevice, UINT uWidth, UINT uHeight, UINT uFormat, LPCTSTR szName = NULL);
    virtual bool                    ColorFillTarget(DWORD dwRGB, LPRECT pRect = NULL);
    virtual TCHAR*                  D3DFmtToString(D3DFORMAT d3dfmt);
};

// Texture utility prototypes
CTexture8*                          CreateGradientTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, 
                                        DWORD dwColors[4], D3DFORMAT fmt = D3DFMT_A8R8G8B8);
CVolumeTexture8*                    CreateGradientVolumeTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, 
                                        DWORD dwDepth, DWORD* dwColors, DWORD dwColorStride, D3DFORMAT fmt = D3DFMT_A8R8G8B8);
CTexture8*                          CreateStripedTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, 
                                        DWORD dwStripes, DWORD dwColorArray[], bool bHorizontal,
                                        bool bDiagonal = false, D3DFORMAT fmt = D3DFMT_A8R8G8B8);
CVolumeTexture8*                    CreateStripedVolumeTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, 
                                        DWORD dwDepth, DWORD dwStripes, DWORD dwColorArray[], DWORD dwArrayStride, 
                                        bool bHorizontal, bool bDiagonal = false, D3DFORMAT fmt = D3DFMT_A8R8G8B8);
CTexture8*                          CreateChannelDataTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, 
                                        PCHANNELDATA pData, D3DFORMAT fmt);

// Vertex utility prototypes
void                                SetVertex(D3DVERTEX *lpVrt, float x, float y, float z, float nx, float ny, float nz);
void                                SetVertex(D3DTLVERTEX *lpVrt, float x, float y, float z);

#endif //__CD3DTEST_H__
