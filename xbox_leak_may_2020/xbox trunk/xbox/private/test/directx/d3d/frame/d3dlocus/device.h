/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    device.h

Description:

    IDirect3DDevice classes.

*******************************************************************************/

#ifndef __DEVICE_H__
#define __DEVICE_H__

//******************************************************************************
// CDevice
//******************************************************************************

//******************************************************************************
class CDevice : public CObject {

protected:

public:

                        CDevice();
                        ~CDevice();
};

//******************************************************************************
class CDevice8 : public CDevice {

protected:

    LPDIRECT3DDEVICE8   m_pd3dd;
    CDirect3D8*         m_pDirect3D;
    CMap32*             m_pObjectMap;

public:

                        CDevice8();
                        ~CDevice8();
    virtual BOOL        Create(CDirect3D8* pDirect3D, LPDIRECT3DDEVICE8 pd3dd);

    virtual 
    LPDIRECT3DDEVICE8   GetIDirect3DDevice8();

    virtual BOOL        AddObject(LPVOID pvInterface, LPVOID pvObject);
    virtual BOOL        RemoveObject(LPVOID pvInterface);
    virtual BOOL        FindObject(LPVOID pvInterface, LPVOID* ppvObject);

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Resource Creation

#ifndef UNDER_XBOX
    virtual HRESULT     CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pd3dpp, CSwapChain8** ppSwapChain);
#endif // !UNDER_XBOX
    virtual HRESULT     CreateCubeTexture(UINT uEdgeLength, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CCubeTexture8** ppCubeTexture);
    virtual HRESULT     CreateDepthStencilSurface(UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, CSurface8** ppSurface);
    virtual HRESULT     CreateImageSurface(UINT uWidth, UINT uHeight, D3DFORMAT fmt, CSurface8** ppSurface);
    virtual HRESULT     CreateIndexBuffer(UINT uLength, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CIndexBuffer8** ppIndexBuffer);
    virtual HRESULT     CreateRenderTarget(UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, BOOL bLockable, CSurface8** ppSurface);
    virtual HRESULT     CreateTexture(UINT uWidth, UINT uHeight, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CTexture8** ppTexture);
    virtual HRESULT     CreateVertexBuffer(UINT uLength, DWORD dwUsage, DWORD dwFVF, D3DPOOL pool, CVertexBuffer8** ppVertexBuffer);
    virtual HRESULT     CreateVolumeTexture(UINT uWidth, UINT uHeight, UINT uDepth, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CVolumeTexture8** ppVolumeTexture);
#ifdef UNDER_XBOX
    virtual HRESULT     CreatePalette(D3DPALETTESIZE Size, CPalette8** ppPalette);
#endif

    // Device states

    virtual HRESULT     ApplyStateBlock(DWORD dwToken);
    virtual HRESULT     BeginStateBlock();
    virtual HRESULT     EndStateBlock(DWORD* pdwToken);
    virtual HRESULT     CreateStateBlock(D3DSTATEBLOCKTYPE sbt, DWORD* pdwToken);
    virtual HRESULT     DeleteStateBlock(DWORD dwToken);
    virtual HRESULT     CaptureStateBlock(DWORD dwToken);
#ifndef UNDER_XBOX
    virtual HRESULT     GetClipStatus(D3DCLIPSTATUS8* pd3dcs);
    virtual HRESULT     SetClipStatus(D3DCLIPSTATUS8* pd3dcs);
#endif // !UNDER_XBOX
    virtual HRESULT     GetRenderState(D3DRENDERSTATETYPE rst, DWORD* pdwValue);
    virtual HRESULT     SetRenderState(D3DRENDERSTATETYPE rst, DWORD dwValue);
    virtual HRESULT     GetRenderTarget(CSurface8** ppRenderTarget);
    virtual HRESULT     SetRenderTarget(CSurface8* pRenderTarget, CSurface8* pDepthStencil);
    virtual HRESULT     GetTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm);
    virtual HRESULT     SetTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm);
    virtual HRESULT     MultiplyTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm);

    // Viewports

    virtual HRESULT     Clear(DWORD dwCount, D3DRECT* prc, DWORD dwFlags, D3DCOLOR c, float fZ, DWORD dwStencil);
    virtual HRESULT     GetViewport(D3DVIEWPORT8* pviewport);
    virtual HRESULT     SetViewport(D3DVIEWPORT8* pviewport);

    // User-defined clip planes

    virtual HRESULT     GetClipPlane(DWORD dwIndex, float* pfPlane);
    virtual HRESULT     SetClipPlane(DWORD dwIndex, float* pfPlane);

    // Lights and materials

    virtual HRESULT     GetLight(DWORD dwIndex, D3DLIGHT8* plight);
    virtual HRESULT     SetLight(DWORD dwIndex, D3DLIGHT8* plight);
    virtual HRESULT     LightEnable(DWORD dwIndex, BOOL bEnable);
    virtual HRESULT     GetLightEnable(DWORD dwIndex, BOOL* pbEnable);
    virtual HRESULT     GetMaterial(D3DMATERIAL8* pmaterial);
    virtual HRESULT     SetMaterial(D3DMATERIAL8* pmaterial);
#ifdef UNDER_XBOX
    virtual HRESULT     GetBackMaterial(D3DMATERIAL8* pmaterial);
    virtual HRESULT     SetBackMaterial(D3DMATERIAL8* pmaterial);
#endif

    // Textures

    virtual HRESULT     GetTexture(DWORD dwStage, CBaseTexture8** ppTexture);
    virtual HRESULT     SetTexture(DWORD dwStage, CBaseTexture8* pTexture);
    virtual HRESULT     GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD* pdwValue);
    virtual HRESULT     SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD dwValue);
    virtual HRESULT     UpdateTexture(CBaseTexture8* pSourceTexture, CBaseTexture8* pDestinationTexture);
    virtual HRESULT     ValidateDevice(DWORD* pdwNumPasses);

    // Palettes

    virtual HRESULT     GetCurrentTexturePalette(UINT* puPaletteNumber);
    virtual HRESULT     SetCurrentTexturePalette(UINT uPaletteNumber);
    virtual HRESULT     GetPaletteEntries(UINT uPaletteNumber, PALETTEENTRY* ppe);
    virtual HRESULT     SetPaletteEntries(UINT uPaletteNumber, PALETTEENTRY* ppe);
#ifdef UNDER_XBOX
    virtual HRESULT     GetPalette(DWORD dwStage, CPalette8** ppPalette);
    virtual HRESULT     SetPalette(DWORD dwStage, CPalette8* pPalette);
#endif

    // Vertex shaders

    virtual HRESULT     CreateVertexShader(DWORD* pdwDeclaration, DWORD* pdwFunction, DWORD* pdwHandle, DWORD dwUsage);
    virtual HRESULT     CreateVertexShaderTok(DWORD* pdwDeclaration, DWORD* pdwFunction, DWORD* pdwHandle, DWORD dwUsage);
    virtual HRESULT     CreateVertexShaderAsm(DWORD* pdwDeclaration, LPVOID pvSourceCode, UINT uCodeLength, DWORD* pdwHandle, DWORD dwUsage);
    virtual HRESULT     DeleteVertexShader(DWORD dwHandle);
    virtual HRESULT     GetVertexShader(DWORD* pdwHandle);
    virtual HRESULT     SetVertexShader(DWORD dwHandle);
    virtual HRESULT     GetVertexShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount);
    virtual HRESULT     SetVertexShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount);
    virtual HRESULT     GetVertexShaderDeclaration(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData);
    virtual HRESULT     GetVertexShaderFunction(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData);
#ifdef UNDER_XBOX
    virtual HRESULT     SetShaderConstantMode(D3DSHADERCONSTANTMODE Mode);
    virtual HRESULT     GetShaderConstantMode(D3DSHADERCONSTANTMODE *pMode);
    virtual HRESULT     LoadVertexShader(DWORD dwHandle, DWORD dwAddress);
    virtual HRESULT     SelectVertexShader(DWORD dwHandle, DWORD dwAddress);
    virtual HRESULT     RunVertexStateShader(DWORD dwAddress, CONST float* pfData);
    virtual HRESULT     GetVertexShaderSize(DWORD dwHandle, UINT* puSize);
    virtual HRESULT     GetVertexShaderType(DWORD dwHandle, DWORD* pdwType);
#endif

    // Pixel shaders

#ifndef UNDER_XBOX
    virtual HRESULT     CreatePixelShader(DWORD* pdwFunction, DWORD* pdwHandle);
#else
    virtual HRESULT     CreatePixelShader(const D3DPIXELSHADERDEF* pd3dpsdFunction, DWORD* pdwHandle);
#endif // UNDER_XBOX
    virtual HRESULT     CreatePixelShaderTok(DWORD* pdwFunction, DWORD* pdwHandle);
    virtual HRESULT     CreatePixelShaderAsm(LPVOID pvSourceCode, UINT uCodeLength, DWORD* pdwHandle);
    virtual HRESULT     DeletePixelShader(DWORD dwHandle);
    virtual HRESULT     GetPixelShader(DWORD* pdwHandle);
    virtual HRESULT     SetPixelShader(DWORD dwHandle);
    virtual HRESULT     GetPixelShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount);
    virtual HRESULT     SetPixelShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount);
#ifndef UNDER_XBOX
    virtual HRESULT     GetPixelShaderFunction(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData);
#else
    virtual HRESULT     GetPixelShaderFunction(DWORD dwHandle, D3DPIXELSHADERDEF* pd3dpsd);
#endif // UNDER_XBOX

    // Presentation

#ifndef UNDER_XBOX
    virtual HRESULT     Present(RECT* prectSrc, RECT* prectDst, HWND hDestWindowOverride, RGNDATA* prgnDirtyRegion);
#else
    virtual HRESULT     Present(RECT* prectSrc, RECT* prectDst, void* hDestWindowOverride, void* prgnDirtyRegion);
#endif // UNDER_XBOX
    virtual HRESULT     Reset(D3DPRESENT_PARAMETERS* pd3dpp);

    // Scenes

    virtual HRESULT     BeginScene();
    virtual HRESULT     EndScene();

    // Rendering

    virtual HRESULT     DrawIndexedPrimitive(D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uStartIndex, UINT uPrimitiveCount);
    virtual HRESULT     DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uPrimitiveCount, void* pvIndices, D3DFORMAT fmtIndex, void* pvVertices, UINT uStride);
    virtual HRESULT     DrawPrimitive(D3DPRIMITIVETYPE d3dpt, UINT uStartVertex, UINT uPrimitiveCount);
    virtual HRESULT     DrawPrimitiveUP(D3DPRIMITIVETYPE d3dpt, UINT uPrimitiveCount, void* pvVertices, UINT uStride);
#ifdef UNDER_XBOX
    virtual HRESULT     DrawVertices(D3DPRIMITIVETYPE d3dpt, UINT uStartVertex, UINT uVertexCount);
    virtual HRESULT     DrawIndexedVertices(D3DPRIMITIVETYPE d3dpt, UINT uStartIndex, CONST WORD* pwIndices);
    virtual HRESULT     DrawVerticesUP(D3DPRIMITIVETYPE d3dpt, UINT uVertexCount, CONST void* pvVertices, UINT uStride);
    virtual HRESULT     DrawIndexedVerticesUP(D3DPRIMITIVETYPE d3dpt, UINT uVertexCount, CONST void* pvIndices, CONST void* pvVertices, UINT uStride);
    virtual HRESULT     PrimeVertexCache(UINT VertexCount, CONST WORD *pIndexData);
    virtual HRESULT     Begin(D3DPRIMITIVETYPE d3dpt);
    virtual HRESULT     End();
    virtual void        KickPushBuffer();
#endif

    // High order surfaces

    virtual HRESULT     DeletePatch(UINT uHandle);
    virtual HRESULT     DrawRectPatch(UINT uHandle, float* pfNumSegs, D3DRECTPATCH_INFO* prpi);
    virtual HRESULT     DrawTriPatch(UINT uHandle, float* pfNumSegs, D3DTRIPATCH_INFO* ptpi);

    // Stream data

    virtual HRESULT     GetStreamSource(UINT uStream, CVertexBuffer8** ppVertexData, UINT* puStride);
    virtual HRESULT     SetStreamSource(UINT uStream, CVertexBuffer8* pVertexData, UINT uStride);

    // Index data

    virtual HRESULT     GetIndices(CIndexBuffer8** ppIndexData, UINT* puBaseIndex);
    virtual HRESULT     SetIndices(CIndexBuffer8* pIndexData, UINT uBaseIndex);

    // Information

    virtual UINT        GetAvailableTextureMem();
    virtual HRESULT     GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pd3dcp);
    virtual HRESULT     GetDeviceCaps(D3DCAPS8* pd3dcaps);
    virtual HRESULT     GetDirect3D(CDirect3D8** ppDirect3D8);
    virtual HRESULT     GetInfo(DWORD dwDevInfoType, void* pvDevInfo, DWORD dwDevInfoSize);
    virtual HRESULT     GetRasterStatus(D3DRASTER_STATUS* prast);
    virtual HRESULT     GetDisplayMode(D3DDISPLAYMODE* pMode);

    // Surfaces

    virtual HRESULT     GetBackBuffer(UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, CSurface8** ppBackBuffer);
    virtual HRESULT     GetDepthStencilSurface(CSurface8** ppZStencilSurface);

    // Gamma ramps

    virtual void        GetGammaRamp(D3DGAMMARAMP* pgrRamp);
    virtual void        SetGammaRamp(DWORD dwFlags, D3DGAMMARAMP* pgrRamp);

    // Miscellaneous

    virtual HRESULT     CopyRects(CSurface8* pSrcSurface, RECT* prectSrcRects, UINT uNumSrcRects, CSurface8* pDstSurface, POINT* ppntDstPoints);
    virtual HRESULT     GetFrontBuffer(CSurface8* pDstSurface);
    virtual HRESULT     ProcessVertices(UINT uStartVertexSrc, UINT uStartVertexDst, UINT uNumVertices, CVertexBuffer8* pDstBuffer, DWORD dwFlags);
    virtual HRESULT     ResourceManagerDiscardBytes(DWORD dwNumBytes);
    virtual HRESULT     TestCooperativeLevel();
#ifdef UNDER_XBOX
    virtual HRESULT     PersistDisplay();
    virtual HRESULT     SetTile(DWORD dwIndex, D3DTILE* pTile);
    virtual HRESULT     GetTile(DWORD dwIndex, D3DTILE* pTile);
#endif

    // Cursors

    virtual void        SetCursorPosition(UINT uSX, UINT uSY, DWORD dwFlags);
    virtual HRESULT     SetCursorProperties(UINT uHotSpotX, UINT uHotSpotY, CSurface8* pCursorBitmap);
    virtual BOOL        ShowCursor(BOOL bShow);

#ifdef UNDER_XBOX

    // Visibility testing

    virtual HRESULT     BeginVisibilityTest();
    virtual HRESULT     EndVisibilityTest(DWORD dwIndex);
    virtual HRESULT     GetVisibilityTestResult(DWORD dwIndex, UINT* puResult, ULONGLONG* puuTimeStamp);

    // Persistent vertex attributes

    virtual HRESULT     SetVertexData2f(int nRegister, float a, float b);
    virtual HRESULT     SetVertexData4f(int nRegister, float a, float b, float c, float d);
    virtual HRESULT     SetVertexData2s(int nRegister, short a, short b);
    virtual HRESULT     SetVertexData4s(int nRegister, short a, short b, short c, short d);
    virtual HRESULT     SetVertexData4ub(int nRegister, BYTE a, BYTE b, BYTE c, BYTE d);

    // Notifications

    virtual BOOL        IsBusy();
    virtual void        BlockUntilIdle();
#ifndef UNDER_XBOX
    virtual void        SetVerticalBlankCallback(D3DCALLBACK pCallback);
#else
    virtual void        SetVerticalBlankCallback(D3DVBLANKCALLBACK pCallback);
#endif
    virtual void        BlockUntilVerticalBlank();
    virtual DWORD       InsertFence();
    virtual BOOL        IsFencePending(DWORD dwFence);
    virtual void        BlockOnFence(DWORD dwFence);
    virtual void        InsertCallback(D3DCALLBACKTYPE Type, D3DCALLBACK pCallback, DWORD dwContext);

    // Push buffers

    virtual HRESULT     BeginPushBuffer(void* pvBuffer, UINT uSize);
    virtual HRESULT     EndPushBuffer(D3DPushBuffer* pPushBuffer);
    virtual HRESULT     RunPushBuffer(D3DPushBuffer* pPushBuffer, DWORD* pdwFixUps);
    virtual HRESULT     GetPushBufferOffset(DWORD* pdwOffset);
    virtual HRESULT     GetProjectionViewportMatrix(D3DMATRIX* pmMatrix);
    virtual HRESULT     SetModelView(CONST D3DMATRIX* pmModelView, CONST D3DMATRIX* pmInverseModelView, CONST D3DMATRIX* pmComposite);
    virtual HRESULT     GetModelView(D3DMATRIX* pmModelView);

    virtual HRESULT     SetVertexBlendModelView(UINT uCount, CONST D3DMATRIX* pmModelViews, CONST D3DMATRIX* pmInverseModelViews, CONST D3DMATRIX* pProjectionViewport);
    virtual HRESULT     GetVertexBlendModelView(UINT uCount, D3DMATRIX* pmModelViews, D3DMATRIX* pProjectionViewport);
    virtual HRESULT     SetVertexInput(ULONG uCount, CONST DWORD* pdwAddresses, CONST DWORD* pdwFormats);
    virtual HRESULT     GetVertexInput(ULONG uCount, DWORD* pdwAddresses, DWORD* pdwFormats);
    virtual void        FlushVertexCache();

#endif
};

//******************************************************************************
class CCDevice8 : public CDevice8 {

protected:

    CClient*            m_pClient;

    BOOL                m_bServerAffinity;
    BOOL                m_bVerifyFrames;
    float               m_fThreshold;
    float               m_fFrameMatch;
    UINT                m_uFramesPresented;
    DWORD               m_dwCRCTable[256];

    CTexture8*          m_pd3dtFrameSrc;
    CTexture8*          m_pd3dtFrameRef;
    CTexture8*          m_pd3dtFrameVar;
    UINT                m_uShowVerification;

    virtual void        OverlayVerificationResults();
    virtual BOOL        UpdateFBTexture(CTexture8* pd3dtDst, LPDIRECT3DSURFACE8 pd3dsSrc);

public:

                        CCDevice8();
                        ~CCDevice8();
    virtual BOOL        Create(CClient* pClient, CDirect3D8* pDirect3D, LPDIRECT3DDEVICE8 pd3dd);

    virtual 
    LPDIRECT3DSURFACE8  GetFrontBufferContents();
    virtual DWORD       ComputeSurfaceCRC32(LPDIRECT3DSURFACE8 pd3ds);
    virtual float       CompareFrames(LPDIRECT3DSURFACE8 pd3dsSrc, LPDIRECT3DSURFACE8 pd3dsRef, LPDIRECT3DSURFACE8 pd3dsDelta);

    virtual void        EnableFrameVerification(BOOL bEnable);
    virtual void        SetVerificationThreshold(float fThreshold);
    virtual void        EnableVerificationDisplay(BOOL bEnable);
    virtual void        ToggleVerificationDisplay();

    // IUnknown

    virtual ULONG       AddRef();
    virtual ULONG       Release();

    // Resource Creation

#ifndef UNDER_XBOX
    virtual HRESULT     CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pd3dpp, CSwapChain8** ppSwapChain);
#endif // !UNDER_XBOX
    virtual HRESULT     CreateCubeTexture(UINT uEdgeLength, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CCubeTexture8** ppCubeTexture);
    virtual HRESULT     CreateDepthStencilSurface(UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, CSurface8** ppSurface);
    virtual HRESULT     CreateImageSurface(UINT uWidth, UINT uHeight, D3DFORMAT fmt, CSurface8** ppSurface);
    virtual HRESULT     CreateIndexBuffer(UINT uLength, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CIndexBuffer8** ppIndexBuffer);
    virtual HRESULT     CreateRenderTarget(UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, BOOL bLockable, CSurface8** ppSurface);
    virtual HRESULT     CreateTexture(UINT uWidth, UINT uHeight, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CTexture8** ppTexture);
    virtual HRESULT     CreateVertexBuffer(UINT uLength, DWORD dwUsage, DWORD dwFVF, D3DPOOL pool, CVertexBuffer8** ppVertexBuffer);
    virtual HRESULT     CreateVolumeTexture(UINT uWidth, UINT uHeight, UINT uDepth, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CVolumeTexture8** ppVolumeTexture);

    // Device states

    virtual HRESULT     ApplyStateBlock(DWORD dwToken);
    virtual HRESULT     BeginStateBlock();
    virtual HRESULT     EndStateBlock(DWORD* pdwToken);
    virtual HRESULT     CreateStateBlock(D3DSTATEBLOCKTYPE sbt, DWORD* pdwToken);
    virtual HRESULT     DeleteStateBlock(DWORD dwToken);
    virtual HRESULT     CaptureStateBlock(DWORD dwToken);
#ifndef UNDER_XBOX
    virtual HRESULT     GetClipStatus(D3DCLIPSTATUS8* pd3dcs);
    virtual HRESULT     SetClipStatus(D3DCLIPSTATUS8* pd3dcs);
#endif // !UNDER_XBOX
    virtual HRESULT     GetRenderState(D3DRENDERSTATETYPE rst, DWORD* pdwValue);
    virtual HRESULT     SetRenderState(D3DRENDERSTATETYPE rst, DWORD dwValue);
    virtual HRESULT     GetRenderTarget(CSurface8** ppRenderTarget);
    virtual HRESULT     SetRenderTarget(CSurface8* pRenderTarget, CSurface8* pDepthStencil);
    virtual HRESULT     GetTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm);
    virtual HRESULT     SetTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm);
    virtual HRESULT     MultiplyTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm);

    // Viewports

    virtual HRESULT     Clear(DWORD dwCount, D3DRECT* prc, DWORD dwFlags, D3DCOLOR c, float fZ, DWORD dwStencil);
    virtual HRESULT     GetViewport(D3DVIEWPORT8* pviewport);
    virtual HRESULT     SetViewport(D3DVIEWPORT8* pviewport);

    // User-defined clip planes

    virtual HRESULT     GetClipPlane(DWORD dwIndex, float* pfPlane);
    virtual HRESULT     SetClipPlane(DWORD dwIndex, float* pfPlane);

    // Lights and materials

    virtual HRESULT     GetLight(DWORD dwIndex, D3DLIGHT8* plight);
    virtual HRESULT     SetLight(DWORD dwIndex, D3DLIGHT8* plight);
    virtual HRESULT     LightEnable(DWORD dwIndex, BOOL bEnable);
    virtual HRESULT     GetLightEnable(DWORD dwIndex, BOOL* pbEnable);
    virtual HRESULT     GetMaterial(D3DMATERIAL8* pmaterial);
    virtual HRESULT     SetMaterial(D3DMATERIAL8* pmaterial);

    // Textures

    virtual HRESULT     GetTexture(DWORD dwStage, CBaseTexture8** ppTexture);
    virtual HRESULT     SetTexture(DWORD dwStage, CBaseTexture8* pTexture);
    virtual HRESULT     GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD* pdwValue);
    virtual HRESULT     SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD dwValue);
    virtual HRESULT     UpdateTexture(CBaseTexture8* pSourceTexture, CBaseTexture8* pDestinationTexture);
    virtual HRESULT     ValidateDevice(DWORD* pdwNumPasses);

    // Palettes

    virtual HRESULT     GetCurrentTexturePalette(UINT* puPaletteNumber);
    virtual HRESULT     SetCurrentTexturePalette(UINT uPaletteNumber);
    virtual HRESULT     GetPaletteEntries(UINT uPaletteNumber, PALETTEENTRY* ppe);
    virtual HRESULT     SetPaletteEntries(UINT uPaletteNumber, PALETTEENTRY* ppe);

    // Vertex shaders

    virtual HRESULT     CreateVertexShader(DWORD* pdwDeclaration, DWORD* pdwFunction, DWORD* pdwHandle, DWORD dwUsage);
    virtual HRESULT     CreateVertexShaderTok(DWORD* pdwDeclaration, DWORD* pdwFunction, DWORD* pdwHandle, DWORD dwUsage);
    virtual HRESULT     CreateVertexShaderAsm(DWORD* pdwDeclaration, LPVOID pvSourceCode, UINT uCodeLength, DWORD* pdwHandle, DWORD dwUsage);
    virtual HRESULT     DeleteVertexShader(DWORD dwHandle);
    virtual HRESULT     GetVertexShader(DWORD* pdwHandle);
    virtual HRESULT     SetVertexShader(DWORD dwHandle);
    virtual HRESULT     GetVertexShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount);
    virtual HRESULT     SetVertexShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount);
    virtual HRESULT     GetVertexShaderDeclaration(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData);
    virtual HRESULT     GetVertexShaderFunction(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData);

    // Pixel shaders

#ifndef UNDER_XBOX
    virtual HRESULT     CreatePixelShader(DWORD* pdwFunction, DWORD* pdwHandle);
#else
    virtual HRESULT     CreatePixelShader(const D3DPIXELSHADERDEF* pd3dpsdFunction, DWORD* pdwHandle);
#endif // UNDER_XBOX
    virtual HRESULT     CreatePixelShaderTok(DWORD* pdwFunction, DWORD* pdwHandle);
    virtual HRESULT     CreatePixelShaderAsm(LPVOID pvSourceCode, UINT uCodeLength, DWORD* pdwHandle);
    virtual HRESULT     DeletePixelShader(DWORD dwHandle);
    virtual HRESULT     GetPixelShader(DWORD* pdwHandle);
    virtual HRESULT     SetPixelShader(DWORD dwHandle);
    virtual HRESULT     GetPixelShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount);
    virtual HRESULT     SetPixelShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount);
#ifndef UNDER_XBOX
    virtual HRESULT     GetPixelShaderFunction(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData);
#else
    virtual HRESULT     GetPixelShaderFunction(DWORD dwHandle, D3DPIXELSHADERDEF* pd3dpsd);
#endif // UNDER_XBOX

    // Presentation

#ifndef UNDER_XBOX
    virtual HRESULT     Present(RECT* prectSrc, RECT* prectDst, HWND hDestWindowOverride, RGNDATA* prgnDirtyRegion);
#else
    virtual HRESULT     Present(RECT* prectSrc, RECT* prectDst, void* hDestWindowOverride, void* prgnDirtyRegion);
#endif // UNDER_XBOX
    virtual HRESULT     Reset(D3DPRESENT_PARAMETERS* pd3dpp);

    // Scenes

    virtual HRESULT     BeginScene();
    virtual HRESULT     EndScene();

    // Rendering

    virtual HRESULT     DrawIndexedPrimitive(D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uStartIndex, UINT uPrimitiveCount);
    virtual HRESULT     DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uPrimitiveCount, void* pvIndices, D3DFORMAT fmtIndex, void* pvVertices, UINT uStride);
    virtual HRESULT     DrawPrimitive(D3DPRIMITIVETYPE d3dpt, UINT uStartVertex, UINT uPrimitiveCount);
    virtual HRESULT     DrawPrimitiveUP(D3DPRIMITIVETYPE d3dpt, UINT uPrimitiveCount, void* pvVertices, UINT uStride);

    // High order surfaces

    virtual HRESULT     DeletePatch(UINT uHandle);
    virtual HRESULT     DrawRectPatch(UINT uHandle, float* pfNumSegs, D3DRECTPATCH_INFO* prpi);
    virtual HRESULT     DrawTriPatch(UINT uHandle, float* pfNumSegs, D3DTRIPATCH_INFO* ptpi);

    // Stream data

    virtual HRESULT     GetStreamSource(UINT uStream, CVertexBuffer8** ppVertexData, UINT* puStride);
    virtual HRESULT     SetStreamSource(UINT uStream, CVertexBuffer8* pVertexData, UINT uStride);

    // Index data

    virtual HRESULT     GetIndices(CIndexBuffer8** ppIndexData, UINT* puBaseIndex);
    virtual HRESULT     SetIndices(CIndexBuffer8* pIndexData, UINT uBaseIndex);

    // Information

    virtual UINT        GetAvailableTextureMem();
    virtual HRESULT     GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pd3dcp);
    virtual HRESULT     GetDeviceCaps(D3DCAPS8* pd3dcaps);
    virtual HRESULT     GetDirect3D(CDirect3D8** ppDirect3D8);
    virtual HRESULT     GetInfo(DWORD dwDevInfoType, void* pvDevInfo, DWORD dwDevInfoSize);
    virtual HRESULT     GetRasterStatus(D3DRASTER_STATUS* prast);
    virtual HRESULT     GetDisplayMode(D3DDISPLAYMODE* pMode);

    // Surfaces

    virtual HRESULT     GetBackBuffer(UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, CSurface8** ppBackBuffer);
    virtual HRESULT     GetDepthStencilSurface(CSurface8** ppZStencilSurface);

    // Gamma ramps

    virtual void        GetGammaRamp(D3DGAMMARAMP* pgrRamp);
    virtual void        SetGammaRamp(DWORD dwFlags, D3DGAMMARAMP* pgrRamp);

    // Miscellaneous

    virtual HRESULT     CopyRects(CSurface8* pSrcSurface, RECT* prectSrcRects, UINT uNumSrcRects, CSurface8* pDstSurface, POINT* ppntDstPoints);
    virtual HRESULT     GetFrontBuffer(CSurface8* pDstSurface);
    virtual HRESULT     ProcessVertices(UINT uStartVertexSrc, UINT uStartVertexDst, UINT uNumVertices, CVertexBuffer8* pDstBuffer, DWORD dwFlags);
    virtual HRESULT     ResourceManagerDiscardBytes(DWORD dwNumBytes);
    virtual HRESULT     TestCooperativeLevel();

    // Cursors

    virtual void        SetCursorPosition(UINT uSX, UINT uSY, DWORD dwFlags);
    virtual HRESULT     SetCursorProperties(UINT uHotSpotX, UINT uHotSpotY, CSurface8* pCursorBitmap);
    virtual BOOL        ShowCursor(BOOL bShow);
};

#endif //__DEVICE_H__
