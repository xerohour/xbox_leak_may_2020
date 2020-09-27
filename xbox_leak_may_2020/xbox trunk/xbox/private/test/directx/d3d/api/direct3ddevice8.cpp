#include "d3dapi.hpp"

LINKME(6)

/*
    HRESULT WINAPI Reset(D3DPRESENT_PARAMETERS *pPresentationParameters);
    HRESULT WINAPI Present(CONST RECT *pSourceRect, CONST RECT *pDestRect, void *pUnused, CONST RGNDATA *pDirtyRegion);
    HRESULT WINAPI GetBackBuffer(UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8 **ppBackBuffer);
    HRESULT WINAPI GetRasterStatus(D3DRASTER_STATUS *pRasterStatus);

	HRESULT WINAPI CreateVertexBuffer(UINT Length, DWORD Usage, DWORD unused, D3DPOOL Pool, IDirect3DVertexBuffer8 **ppVertexBuffer);

    HRESULT WINAPI CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer8 **ppIndexBuffer);
    HRESULT WINAPI CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, IDirect3DSurface8 **ppSurface);
    HRESULT WINAPI CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface8 **ppSurface);
    HRESULT WINAPI CreateImageSurface(UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface8 **ppSurface);
    HRESULT WINAPI CopyRects(IDirect3DSurface8 *pSourceSurface, CONST RECT *pSourceRectsArray, UINT cRects, IDirect3DSurface8 *pDestinationSurface, CONST POINT *pDestPointsArray);
    HRESULT WINAPI GetFrontBuffer(IDirect3DSurface8 *pDestSurface);
    HRESULT WINAPI SetRenderTarget(IDirect3DSurface8 *pRenderTarget, IDirect3DSurface8 *pNewZStencil);
    HRESULT WINAPI GetRenderTarget(IDirect3DSurface8 **ppRenderTarget);
    HRESULT WINAPI GetDepthStencilSurface(IDirect3DSurface8 **ppZStencilSurface);
    HRESULT WINAPI BeginScene();
    HRESULT WINAPI EndScene();
    HRESULT WINAPI Clear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
    HRESULT WINAPI SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
    HRESULT WINAPI GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix);
    HRESULT WINAPI MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
    HRESULT WINAPI SetViewport(CONST D3DVIEWPORT8 *pViewport);
    HRESULT WINAPI GetViewport(D3DVIEWPORT8 *pViewport);
    HRESULT WINAPI SetMaterial(CONST D3DMATERIAL8 *pMaterial);
    HRESULT WINAPI GetMaterial(D3DMATERIAL8 *pMaterial);
    HRESULT WINAPI SetLight(DWORD Index, CONST D3DLIGHT8 *pLight);
    HRESULT WINAPI GetLight(DWORD Index, D3DLIGHT8 *pLight);
    HRESULT WINAPI LightEnable(DWORD Index, BOOL Enable);
    HRESULT WINAPI GetLightEnable(DWORD Index, BOOL *pEnable);
    HRESULT WINAPI SetClipPlane(DWORD Index, CONST float *pPlane);
    HRESULT WINAPI GetClipPlane(DWORD Index, float *pPlane);
    HRESULT WINAPI SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
    HRESULT WINAPI GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue);
    HRESULT WINAPI SetClipStatus(CONST D3DCLIPSTATUS8 *pClipStatus);
    HRESULT WINAPI GetClipStatus(D3DCLIPSTATUS8 *pClipStatus);
    HRESULT WINAPI ValidateDevice(DWORD *pNumPasses);
    HRESULT WINAPI GetInfo(DWORD DevInfoID, void *pDevInfoStruct, DWORD DevInfoStructSize);
    HRESULT WINAPI SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY *pEntries);
    HRESULT WINAPI GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY *pEntries);
    HRESULT WINAPI DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    HRESULT WINAPI DrawIndexedPrimitive(D3DPRIMITIVETYPE, UINT minIndex, UINT NumIndices, UINT startIndex, UINT primCount);
    HRESULT WINAPI DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
    HRESULT WINAPI DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
    HRESULT WINAPI ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer8 *pDestBuffer, DWORD Flags);
    HRESULT WINAPI CreateVertexShader(CONST DWORD *pDeclaration, CONST DWORD *pFunction, DWORD *pHandle, DWORD Usage);
    HRESULT WINAPI SetVertexShader(DWORD Handle);
    HRESULT WINAPI GetVertexShader(DWORD *pHandle);
    HRESULT WINAPI DeleteVertexShader(DWORD Handle);
    HRESULT WINAPI SetVertexShaderConstant(DWORD Register, CONST void *pConstantData, DWORD ConstantCount);
    HRESULT WINAPI GetVertexShaderConstant(DWORD Register, void *pConstantData, DWORD ConstantCount);
    HRESULT WINAPI GetVertexShaderDeclaration(DWORD Handle, void *pData, DWORD *pSizeOfData);
    HRESULT WINAPI GetVertexShaderFunction(DWORD Handle,void *pData, DWORD *pSizeOfData);
    HRESULT WINAPI SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8 *pStreamData, UINT Stride);
    HRESULT WINAPI GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8 **ppStreamData, UINT *pStride);
    HRESULT WINAPI SetIndices(IDirect3DIndexBuffer8* pIndexData, UINT BaseVertexIndex);
    HRESULT WINAPI GetIndices(IDirect3DIndexBuffer8** ppIndexData, UINT *pBaseVertexIndex);
    HRESULT WINAPI CreatePixelShader(CONST DWORD *pFunction, DWORD *pHandle);
    HRESULT WINAPI SetPixelShader(DWORD Handle);
    HRESULT WINAPI GetPixelShader(DWORD *pHandle);
    HRESULT WINAPI DeletePixelShader(DWORD Handle);
    HRESULT WINAPI SetPixelShaderConstant(DWORD Register, CONST void *pConstantData, DWORD ConstantCount);
    HRESULT WINAPI GetPixelShaderConstant(DWORD Register, void *pConstantData, DWORD ConstantCount);
    HRESULT WINAPI GetPixelShaderFunction(DWORD Handle, void *pData, DWORD *pSizeOfData);
    HRESULT WINAPI DrawRectPatch(UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo);
    HRESULT WINAPI DrawTriPatch(UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo);
    HRESULT WINAPI DeletePatch(UINT Handle);

********************************Unimportant routines*******************************
  
	HRESULT WINAPI SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8 *pCursorBitmap);
    void    WINAPI SetCursorPosition(UINT XScreenSpace, UINT YScreenSpace, DWORD Flags);
    BOOL    WINAPI ShowCursor(BOOL bShow);
#if 0 // NUGOOP
    HRESULT WINAPI CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain8 **pSwapChain);
#endif 0 // NUGOOP


*/


//goes after Direct3d8 & testDirect3dDevice8
#pragma data_seg(".d3dapi$test060") 

BEGINTEST(testDeviceGetDirect3D)
{
	DWORD d;
	IDirect3D8* r;
	xStartVariation(hlog, "DeviceGetDirect3D");
	STARTLEAKCHECK();
	d = g_pd3dd8->GetDirect3D(&r);
	CHECKRESULT((d == D3D_OK) && (r == g_pd3d8));
//	if(!((DWORD(r)) & 0xfffffff0)) xLog(hlog, XLL_INFO, "r == %p", r);
	if(WASBADRESULT()) xLog(hlog, XLL_INFO, "result: %d, real d3d8: %p, received d3d8: %p", d, g_pd3d8, r);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testDeviceGetDeviceCaps)
{
	DWORD d;
	D3DCAPS8 p;


	xStartVariation(hlog, "GetDeviceCaps(f's)");
	STARTLEAKCHECK();
	p.Caps = 0xffffffff;
	d = g_pd3dd8->GetDeviceCaps(&p);
	CHECKRESULT((d == D3D_OK) && (p.Caps != 0xffffffff));
	if(WASBADRESULT()) xLog(hlog, XLL_INFO, "result: %d", d);

	xEndVariation(hlog);
	xStartVariation(hlog, "GetDeviceCaps(0's)");
	
	p.Caps = 0;
	d = g_pd3dd8->GetDeviceCaps(&p);
	CHECKRESULT((d == D3D_OK) && (p.Caps != 0));
	if(WASBADRESULT()) xLog(hlog, XLL_INFO, "result: %d", d);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()


BEGINTEST(testDeviceGetDisplayMode)
{
	DWORD d;
	D3DDISPLAYMODE m = {0,0,0,(D3DFORMAT)0};

	xStartVariation(hlog, "GetDisplayMode");
	STARTLEAKCHECK();
	d = g_pd3dd8->GetDisplayMode(&m);
	CHECKRESULT((d == D3D_OK) && (m.Width != 0) && (m.Height != 0) && (m.RefreshRate != 0) && (m.Format != (D3DFORMAT)0));
	if(WASBADRESULT()) xLog(hlog, XLL_INFO, "ret %d, wid %d, hei %d, rr %d, for %d", d, m.Width, m.Height, m.RefreshRate, m.Format);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()

/*
BEGINTEST(testGetCreationParameters)
{
	DWORD d;
	D3DDEVICE_CREATION_PARAMETERS p = {777, (D3DDEVTYPE)777, (HWND)777, 777};
	xStartVariation(hlog, "GetCreationParameters");
	STARTLEAKCHECK();
	d = g_pd3dd8->GetCreationParameters(&p);
	CHECKRESULT((d == D3D_OK) && (p.AdapterOrdinal != 777) && (p.DeviceType != 777) && (p.BehaviorFlags != 777));
	if(WASBADRESULT()) xLog(hlog, XLL_INFO, "ret %d adapO %d DevType %d Hwnd %d BehF %d", 
		d, p.AdapterOrdinal, p.DeviceType, p.hFocusWindow, p.BehaviorFlags);
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()
*/

BEGINTEST(testGammaRamp)
{
	D3DGAMMARAMP o, n, c1, c2;
	int i;
    DWORD numbad = 0;

	for(i = 0; i < 256; i++) {
		n.red[i] = 0 + i;
		n.green[i] = 0 - i;
		n.blue[i] = 5358 * i;
	}
	
	xStartVariation(hlog, "GammaRamps");
	STARTLEAKCHECK();
	g_pd3dd8->GetGammaRamp(&o);
	g_pd3dd8->SetGammaRamp(D3DSGR_NO_CALIBRATION, &n);
	g_pd3dd8->GetGammaRamp(&c1);
	g_pd3dd8->SetGammaRamp(D3DSGR_NO_CALIBRATION, &o);
	g_pd3dd8->GetGammaRamp(&c2);

	for(i = 0; i < 256; i++) {
		if((n.red[i] != c1.red[i]) || (n.green[i] != c1.green[i]) || (n.blue[i] != c1.blue[i])) 
            numbad |= 1;
		if((o.red[i] != c2.red[i]) || (o.green[i] != c2.green[i]) || (o.blue[i] != c2.blue[i])) 
            numbad |= 2;
	}
    CHECKRESULT(numbad == 0);
    if(WASBADRESULT()) xLog(hlog, XLL_INFO, "numbad = %d", numbad);
	CHECKLEAKS();
	xEndVariation(hlog);
}	
ENDTEST()

#pragma data_seg()

