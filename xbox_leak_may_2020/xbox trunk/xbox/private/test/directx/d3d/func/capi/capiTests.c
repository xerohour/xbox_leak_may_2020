/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	capiTests.c

Abstract:

	capi tests - Created to test Xbox capi

Author:

	Jeff Sullivan (jeffsul) 26-Jun-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	26-Jun-2001 jeffsul
		Initial Version

--*/

#include "capiTests.h"


/*++

	RunTests

Routine Description:

	includes all C API function calls to make sure that they compile.
	Note: These functions are not really intended to be called.

Arguments:

	None

Return Value:

	S_OK on success
    E_XX on failure

--*/

HRESULT RunTests()
{

	/* function logic variables */
	HRESULT								hr							= S_OK;
	BOOL								bRunTestsD3D				= FALSE;
	BOOL								bRunTestsD3DX				= FALSE;

	/* basic parameter variables */
	DWORD								dw							= 0;
	DWORD*								pdw							= NULL;
	DWORD**								ppdw						= NULL;
	DWORD								adw[MAX_FVF_DECL_SIZE];
	FLOAT								f							= 0.0f;
	FLOAT*								pf							= NULL;
	UINT								ui							= 0;
	UINT*								pui							= NULL;
	BYTE**								ppb							= NULL;
	LPVOID*								ppv							= NULL;
	WORD*								pw							= NULL;
	SHORT								s							= 0;
	BOOL								b							= FALSE;
	BOOL*								pb							= NULL;
	VOID*								pv							= NULL;
	INT									n							= 0;
	BYTE								byte						= 0;
	BYTE**								ppbyte						= NULL;
	ULONGLONG*							pull						= NULL;
	RECT*								pRect						= NULL;
	POINT*								pPoint						= NULL;
	REFGUID								refguid						= 0;
	REFIID								iid							= 0;
	IStream*							pIStream					= NULL;

	/* Direct3D variables */
	LPDIRECT3D8*						ppDirect3D					= NULL;
	LPDIRECT3DDEVICE8					pDevice						= NULL;
	LPDIRECT3DDEVICE8*					ppDevice					= NULL;
	LPDIRECT3DSURFACE8					pSurface					= NULL;
	LPDIRECT3DSURFACE8*					ppSurface					= NULL;
	LPDIRECT3DPALETTE8					pPalette					= NULL;
	LPDIRECT3DPALETTE8*					ppPalette					= NULL;
	LPDIRECT3DBASETEXTURE8				pBaseTexture				= NULL;
	LPDIRECT3DBASETEXTURE8*				ppBaseTexture				= NULL;
	LPDIRECT3DTEXTURE8					pTexture					= NULL;
	LPDIRECT3DTEXTURE8*					ppTexture					= NULL;
	LPDIRECT3DCUBETEXTURE8				pCubeTexture				= NULL;
	LPDIRECT3DCUBETEXTURE8*				ppCubeTexture				= NULL;
	LPDIRECT3DVOLUMETEXTURE8*			ppVolumeTexture				= NULL;
	LPDIRECT3DVOLUME8*					ppVolume					= NULL;
	LPDIRECT3DVERTEXBUFFER8				pVertexBuffer				= NULL;
	LPDIRECT3DVERTEXBUFFER8*			ppVertexBuffer				= NULL;
	LPDIRECT3DINDEXBUFFER8				pIndexBuffer				= NULL;
	LPDIRECT3DINDEXBUFFER8*				ppIndexBuffer				= NULL;
	LPDIRECT3DPUSHBUFFER8				pPushBuffer					= NULL;
	LPDIRECT3DPUSHBUFFER8*				ppPushBuffer				= NULL;
	LPDIRECT3DFIXUP8					pFixup						= NULL;
	LPDIRECT3DFIXUP8*					ppFixup						= NULL;

	D3DFORMAT							Format						= 0;
	D3DMEMORY							where						= 0;
	D3DPOOL								Pool						= 0;
	D3DCALLBACK							Callback					= 0;
	D3DCALLBACKTYPE						CallbackType				= 0;
	D3DDEVTYPE							DevType						= 0;
	D3DRESOURCETYPE						ResourceType				= 0;
	D3DTRANSFORMSTATETYPE				TransformStateType			= 0;
	D3DRENDERSTATETYPE					RenderStateType				= 0;
	D3DTEXTURESTAGESTATETYPE			TextureStageStateType		= 0;
	D3DSTATEBLOCKTYPE					StateBlockType				= 0;
	D3DPRIMITIVETYPE					PrimitiveType				= 0;
	D3DMULTISAMPLE_TYPE					MultiSampleType				= 0;
	D3DBACKBUFFER_TYPE					BackBufferType				= 0;
	D3DCUBEMAP_FACES					CubeMapFaces				= 0;
	D3DPALETTESIZE						PaletteSize					= 0;
	D3DCOLOR							Color						= 0;
	D3DCOLOR**							ppColor						= NULL;
	D3DADAPTER_IDENTIFIER8*				pAdapterIdentifier			= NULL;
	D3DDISPLAYMODE*						pDisplayMode				= NULL;
	D3DCAPS8*							pCaps						= NULL;
	D3DPRESENT_PARAMETERS*				pPresentationParameters		= NULL;
	D3DDEVICE_CREATION_PARAMETERS*		pCreationParameters			= NULL;
	D3DRASTER_STATUS*					pRasterStatus				= NULL;
	D3DGAMMARAMP*						pGammaRamp					= NULL;
	D3DRECT*							pD3DRect					= NULL; /* named pD3DRect to not conflict with RECT* pRect */
	D3DBOX*								pBox						= NULL;
	D3DLOCKED_BOX*						pLockedBox					= NULL;
	D3DLOCKED_RECT*						pLockedRect					= NULL;
	D3DMATRIX*							pMatrix						= NULL;
	D3DVIEWPORT8*						pViewport					= NULL;
	D3DMATERIAL8*						pMaterial					= NULL;
	D3DLIGHT8*							pLight						= NULL;
	D3DSHADERCONSTANTMODE*				pShaderConstantMode			= NULL;
	D3DPIXELSHADERDEF*					pPixelShaderDef				= NULL;
	D3DRECTPATCH_INFO*					pRectPatchInfo				= NULL;
	D3DTRIPATCH_INFO*					pTriPatchInfo				= NULL;
	D3DFIELD_STATUS*					pFieldStatus				= NULL;
	D3DSTREAM_INPUT*					pStreamInputs				= NULL;
	D3DTILE*							pTile						= NULL;
	D3DVOLUME_DESC*						pVolumeDesc					= NULL;
	D3DSURFACE_DESC*					pSurfaceDesc				= NULL;
	D3DVERTEXBUFFER_DESC*				pVertexBufferDesc			= NULL;
	D3DINDEXBUFFER_DESC*				pIndexBufferDesc			= NULL;

	/* Direct3DX Variables */
	LPD3DXMATRIX						pM							= NULL;
	LPD3DXVECTOR2						pV2							= NULL;
	LPD3DXVECTOR3						pV3							= NULL;
	LPD3DXVECTOR4						pV4							= NULL;
	LPD3DXMESH							pMesh						= NULL;
	LPD3DXMESH*							ppMesh						= NULL;
	LPD3DXPMESH*						ppPMesh						= NULL;
	LPD3DXMATERIAL						pMaterials					= NULL;
	LPD3DXTECHNIQUE*					ppTechnique					= NULL;
	LPD3DXEFFECT*						ppEffect					= NULL;
	LPD3DXBUFFER*						ppXBuffer					= NULL; 
	LPD3DXATTRIBUTERANGE				pAttributeRange				= NULL;
	D3DXRTS_DESC*						pRTSDesc					= NULL;
	D3DXRTE_DESC*						pRTEDesc					= NULL;
	D3DXTECHNIQUE_DESC*					pTechniqueDesc				= NULL;
	D3DXEFFECT_DESC*					pEffectDesc					= NULL;
	D3DXPARAMETER_DESC*					pParameterDesc				= NULL;
	D3DXPASS_DESC*						pPassDesc					= NULL;

	/* variables to hold return values of functions */
	ULONG								ulVar;
	UINT								uiVar;
	HRESULT								hrVar;
	DWORD								dwVar;
	BOOL								bVar;
	D3DRESOURCETYPE						d3drt;
	D3DPALETTESIZE						d3dps;
	LPVOID								pvVar;
	D3DXMATRIX*							pMVar;

	
	if( bRunTestsD3D )
	{
		//------------------------------------
		// begin d3d8.h
		//------------------------------------
		
		/* Direct3D Variables */
		Direct3D*							pThisDirect3D				= NULL;
		D3DDevice*							pThisDevice					= NULL;
		D3DResource*						pThisResource				= NULL;
		D3DBaseTexture*						pThisBaseTexture			= NULL;
		D3DTexture*							pThisTexture				= NULL;
		D3DVolumeTexture*					pThisVolumeTexture			= NULL;
		D3DCubeTexture*						pThisCubeTexture			= NULL;
		D3DVertexBuffer*					pThisVertexBuffer			= NULL;
		D3DIndexBuffer*						pThisIndexBuffer			= NULL;
		D3DPalette*							pThisPalette				= NULL;
		D3DPushBuffer*						pThisPushBuffer				= NULL;
		D3DFixup*							pThisFixup					= NULL;
		D3DSurface*							pThisSurface				= NULL;
		D3DVolume*							pThisVolume					= NULL;

		/* Direct3D */
		ulVar =  Direct3D_AddRef();
		ulVar =  Direct3D_Release();
		uiVar =  Direct3D_GetAdapterCount();
		hrVar =  Direct3D_GetAdapterIdentifier(ui, dw, pAdapterIdentifier);
		uiVar =  Direct3D_GetAdapterModeCount( ui );
		hrVar =  Direct3D_EnumAdapterModes( ui, ui, pDisplayMode);
		hrVar =  Direct3D_GetAdapterDisplayMode( ui, pDisplayMode);
		hrVar =  Direct3D_CheckDeviceType( ui, DevType, Format, Format, b);
		hrVar =  Direct3D_CheckDeviceFormat(ui, DevType, Format, dw, ResourceType, Format);
		hrVar =  Direct3D_CheckDeviceMultiSampleType(ui, DevType, Format, b, MultiSampleType);
		hrVar =  Direct3D_CheckDepthStencilMatch(ui, DevType, Format, Format, Format);
		hrVar =  Direct3D_GetDeviceCaps(ui,DevType,pCaps);
		hrVar =  Direct3D_CreateDevice(ui, DevType, pv, dw, pPresentationParameters, ppDevice);
		/*void*/ Direct3D_SetPushBufferSize(dw, dw);
		
		ulVar = IDirect3D8_AddRef(pThisDirect3D);
		ulVar = IDirect3D8_Release(pThisDirect3D);
		uiVar = IDirect3D8_GetAdapterCount(pThisDirect3D);
		hrVar = IDirect3D8_GetAdapterIdentifier(pThisDirect3D, ui, dw, pAdapterIdentifier);
		uiVar = IDirect3D8_GetAdapterModeCount(pThisDirect3D, ui);
		hrVar = IDirect3D8_EnumAdapterModes(pThisDirect3D, ui, ui, pDisplayMode);
		hrVar = IDirect3D8_GetAdapterDisplayMode(pThisDirect3D, ui, pDisplayMode);
		hrVar = IDirect3D8_CheckDeviceType(pThisDirect3D, ui, DevType, Format, Format, b);
		hrVar = IDirect3D8_CheckDeviceFormat(pThisDirect3D, ui, DevType, Format, dw, ResourceType, Format);
		hrVar = IDirect3D8_CheckDeviceMultiSampleType(pThisDirect3D, ui, DevType, Format, b, MultiSampleType);
		hrVar = IDirect3D8_CheckDepthStencilMatch(pThisDirect3D, ui, DevType, Format, Format, Format);
		hrVar = IDirect3D8_GetDeviceCaps(pThisDirect3D, ui,DevType,pCaps);
		hrVar = IDirect3D8_CreateDevice(pThisDirect3D, ui, DevType, pv, dw, pPresentationParameters, ppDevice);
		hrVar = IDirect3D8_SetPushBufferSize(pThisDirect3D, dw, dw);
		

		/* D3DDevice */
		ulVar = D3DDevice_AddRef();
		ulVar = D3DDevice_Release();
		/*void*/ D3DDevice_GetDirect3D(ppDirect3D);
		/*void*/D3DDevice_GetDeviceCaps(pCaps);
		/*void*/D3DDevice_GetDisplayMode(pDisplayMode);
		/*void*/D3DDevice_GetCreationParameters(pCreationParameters);
		hrVar = D3DDevice_Reset(pPresentationParameters);
		/*void*/D3DDevice_Present(pRect, pRect, pv, pv);
		/*void*/D3DDevice_GetBackBuffer(n, BackBufferType, ppSurface);
		/*void*/D3DDevice_GetRasterStatus(pRasterStatus);
		/*void*/D3DDevice_SetFlickerFilter(dw);
		/*void*/D3DDevice_SetGammaRamp(dw, pGammaRamp);
		/*void*/D3DDevice_GetGammaRamp(pGammaRamp);
		hrVar = D3DDevice_CreateTexture(ui, ui, ui, dw, Format, Pool, ppTexture);
		hrVar = D3DDevice_CreateVolumeTexture(ui, ui, ui, ui, dw, Format, Pool, ppVolumeTexture);
		hrVar = D3DDevice_CreateCubeTexture(ui, ui, dw, Format, Pool, ppCubeTexture);
		hrVar = D3DDevice_CreateVertexBuffer(ui, dw, dw, Pool, ppVertexBuffer);
		hrVar = D3DDevice_CreateIndexBuffer(ui, dw, Format, Pool, ppIndexBuffer);
		hrVar = D3DDevice_CreatePalette(PaletteSize, ppPalette);
		hrVar = D3DDevice_CreateRenderTarget(ui, ui, Format, MultiSampleType, b, ppSurface);
		hrVar = D3DDevice_CreateDepthStencilSurface(ui, ui, Format, MultiSampleType, ppSurface);
		hrVar = D3DDevice_CreateImageSurface(ui, ui, Format, ppSurface);
		/*void*/D3DDevice_CopyRects(pSurface, pRect, ui, pSurface, pPoint);
		/*void*/D3DDevice_UpdateTexture(pBaseTexture, pBaseTexture);
		/*void*/D3DDevice_SetRenderTarget(pSurface, pSurface);
		/*void*/D3DDevice_GetRenderTarget(ppSurface);
		hrVar = D3DDevice_GetDepthStencilSurface(ppSurface);
		/*void*/D3DDevice_BeginScene();
		/*void*/D3DDevice_EndScene();
		/*void*/D3DDevice_Clear(dw, pD3DRect, dw, Color, f, dw);
		/*void*/D3DDevice_SetTransform(TransformStateType, pMatrix);
		/*void*/D3DDevice_GetTransform(TransformStateType, pMatrix);
		/*void*/D3DDevice_MultiplyTransform(TransformStateType, pMatrix);
		/*void*/D3DDevice_SetViewport(/*CONST*/ pViewport);
		/*void*/D3DDevice_GetViewport(pViewport);
		/*void*/D3DDevice_SetMaterial(/*CONST*/ pMaterial);
		/*void*/D3DDevice_GetMaterial(pMaterial);
		/*void*/D3DDevice_SetBackMaterial(/*CONST*/ pMaterial);
		/*void*/D3DDevice_GetBackMaterial(pMaterial);
		hrVar = D3DDevice_SetLight(dw, /*CONST*/ pLight);
		/*void*/D3DDevice_GetLight(dw, pLight);
		hrVar = D3DDevice_LightEnable(dw, b);
		/*void*/D3DDevice_GetLightEnable(dw, pb);
		/*void*/D3DDevice_SetRenderStateNotInline(TransformStateType, dw);
		hrVar = D3DDevice_SetRenderState_ParameterCheck(TransformStateType, dw);
		/*void*/D3DDevice_SetRenderState_Simple(dw, dw);
		/*void*/D3DDevice_SetRenderState_Deferred(TransformStateType, dw);
		/*void*/D3DDevice_SetRenderState_PSTextureModes(dw);
		/*void*/D3DDevice_SetRenderState_VertexBlend(dw);
		/*void*/D3DDevice_SetRenderState_FogColor(dw);
		/*void*/D3DDevice_SetRenderState_FillMode(dw);
		/*void*/D3DDevice_SetRenderState_BackFillMode(dw);
		/*void*/D3DDevice_SetRenderState_TwoSidedLighting(dw);
		/*void*/D3DDevice_SetRenderState_NormalizeNormals(dw);
		/*void*/D3DDevice_SetRenderState_ZEnable(dw);
		/*void*/D3DDevice_SetRenderState_StencilEnable(dw);
		/*void*/D3DDevice_SetRenderState_StencilFail(dw);
		/*void*/D3DDevice_SetRenderState_CullMode(dw);
		/*void*/D3DDevice_SetRenderState_FrontFace(dw);
		/*void*/D3DDevice_SetRenderState_TextureFactor(dw);
		/*void*/D3DDevice_SetRenderState_ZBias(dw);
		/*void*/D3DDevice_SetRenderState_LogicOp(dw);
		/*void*/D3DDevice_SetRenderState_EdgeAntiAlias(dw);
		/*void*/D3DDevice_SetRenderState_MultiSampleAntiAlias(dw);
		/*void*/D3DDevice_SetRenderState_MultiSampleMask(dw);
		/*void*/D3DDevice_SetRenderState_MultiSampleType(dw);
		/*void*/D3DDevice_SetRenderState_ShadowFunc(dw);
		/*void*/D3DDevice_SetRenderState_LineWidth(dw);
		/*void*/D3DDevice_SetRenderState_Dxt1NoiseEnable(dw);
		/*void*/D3DDevice_SetRenderState_YuvEnable(dw);
		/*void*/D3DDevice_SetRenderState_OcclusionCullEnable(dw);
		/*void*/D3DDevice_SetRenderState_StencilCullEnable(dw);
		/*void*/D3DDevice_SetRenderState_RopZCmpAlwaysRead(dw);
		/*void*/D3DDevice_SetRenderState_RopZRead(dw);
		/*void*/D3DDevice_SetRenderState_DoNotCullUncompressed(dw);
#pragma MESSAGE( "Removed NotInline functions from API" )
//		/*void*/D3DDevice_GetRenderStateNotInline(RenderStateType, pdw);
//		/*void*/D3DDevice_GetTextureStageStateNotInline(dw, TextureStageStateType, pdw);

		/*void*/D3DDevice_SetTextureStageStateNotInline(dw, RenderStateType, dw);
		hrVar = D3DDevice_SetTextureState_ParameterCheck(dw, RenderStateType, dw);
		/*void*/D3DDevice_SetTextureState_Deferred(dw, RenderStateType, dw);
		/*void*/D3DDevice_SetTextureState_BumpEnv(dw, RenderStateType, dw);
		/*void*/D3DDevice_SetTextureState_TexCoordIndex(dw, dw);
		/*void*/D3DDevice_SetTextureState_BorderColor(dw, dw);
		/*void*/D3DDevice_SetTextureState_ColorKeyColor(dw, dw);
#if D3DCOMPILE_BEGINSTATEBLOCK
		/*void*/D3DDevice_BeginStateBlock();
		hrVar = D3DDevice_EndStateBlock(pdw);
#endif
		/*void*/D3DDevice_ApplyStateBlock(dw);
		/*void*/D3DDevice_CaptureStateBlock(dw);
		/*void*/D3DDevice_DeleteStateBlock(dw);
		hrVar = D3DDevice_CreateStateBlock(StateBlockType,pdw);
		/*void*/D3DDevice_GetTexture(dw, ppBaseTexture);
		/*void*/D3DDevice_SetTexture(dw, pBaseTexture);
		/*void*/D3DDevice_GetPalette(dw, ppPalette);
		/*void*/D3DDevice_SetPalette(dw, pPalette);
		/*void*/D3DDevice_DrawVertices(PrimitiveType, ui, ui);
		/*void*/D3DDevice_DrawIndexedVertices(PrimitiveType, ui, /*CONST*/ pw);
		/*void*/D3DDevice_DrawVerticesUP(PrimitiveType, ui, /*CONST*/ pv, ui);
		/*void*/D3DDevice_DrawIndexedVerticesUP(PrimitiveType, ui, /*CONST*/ pIndexBuffer, /*CONST*/ pv, ui);
		/*void*/D3DDevice_PrimeVertexCache(ui, /*CONST*/ pw);
		hrVar = D3DDevice_CreateVertexShader(/*CONST*/ pdw, /*CONST*/ pdw, pdw, dw);
		/*void*/D3DDevice_SetVertexShader(dw);
		/*void*/D3DDevice_GetVertexShader(pdw);
		/*void*/D3DDevice_DeleteVertexShader(dw);
		/*void*/D3DDevice_SetVertexShaderConstant(n, /*CONST*/ pv, dw);
		/*void*/D3DDevice_GetVertexShaderConstant(n, pv, dw);
		/*void*/D3DDevice_SetShaderConstantMode(ui);
		/*void*/D3DDevice_GetShaderConstantMode(pShaderConstantMode);
		/*void*/D3DDevice_LoadVertexShader(dw, dw);
		/*void*/D3DDevice_LoadVertexShaderProgram(pdw, dw);
		/*void*/D3DDevice_SelectVertexShader(dw, dw);
		/*void*/D3DDevice_RunVertexStateShader(dw, /*CONST*/ pf);
		/*void*/D3DDevice_GetVertexShaderSize(dw, pui);
		/*void*/D3DDevice_GetVertexShaderType(dw, pdw);
		hrVar = D3DDevice_GetVertexShaderDeclaration(dw, pf, pdw);
		hrVar = D3DDevice_GetVertexShaderFunction(dw,pf, pdw);
		/*void*/D3DDevice_SetStreamSource(ui, pVertexBuffer, ui);
		/*void*/D3DDevice_GetStreamSource(ui, ppVertexBuffer, pui);
		/*void*/D3DDevice_SetIndices(pIndexBuffer, ui);
		/*void*/D3DDevice_GetIndices(ppIndexBuffer, pui);
		/*void*/D3DDevice_CreatePixelShader(/*CONST*/ pPixelShaderDef, pdw);
		/*void*/D3DDevice_SetPixelShader(dw);
		/*void*/D3DDevice_SetPixelShaderProgram(pPixelShaderDef);
		/*void*/D3DDevice_GetPixelShader(pdw);
		/*void*/D3DDevice_DeletePixelShader(dw);
		/*void*/D3DDevice_SetPixelShaderConstant(n, /*CONST*/ pv, dw);
		/*void*/D3DDevice_GetPixelShaderConstant(n, pv, dw);
		/*void*/D3DDevice_GetPixelShaderFunction(dw, pPixelShaderDef);
		hrVar = D3DDevice_DrawRectPatch(dw, /*CONST*/ pf, /*CONST*/ pRectPatchInfo);
		hrVar = D3DDevice_DrawTriPatch(dw, /*CONST*/ pf, /*CONST*/ pTriPatchInfo);
		/*void*/D3DDevice_DeletePatch(dw);
		/*void*/D3DDevice_UpdateOverlay(pSurface, pRect, pRect, b, Color);
		/*void*/D3DDevice_EnableOverlay(b);
		/*void*/D3DDevice_EnableCC(b);
		/*void*/D3DDevice_SendCC(b, byte, byte);
		/*void*/D3DDevice_GetCCStatus(pb, pb);
		/*void*/D3DDevice_BeginVisibilityTest();
		hrVar = D3DDevice_EndVisibilityTest(dw);
		hrVar = D3DDevice_GetVisibilityTestResult(dw, pui, pull);
		bVar  = D3DDevice_IsBusy();
		/*void*/D3DDevice_BlockUntilIdle();
		/*void*/D3DDevice_KickPushBuffer();
		/*void*/D3DDevice_SetVerticalBlankCallback(Callback);
		/*void*/D3DDevice_BlockUntilVerticalBlank();
		dwVar = D3DDevice_InsertFence();
		bVar  = D3DDevice_IsFencePending(dw);
		/*void*/D3DDevice_BlockOnFence(dw);
		/*void*/D3DDevice_InsertCallback(CallbackType, Callback, dw);
		/*void*/D3DDevice_FlushVertexCache();
		hrVar = D3DDevice_PersistDisplay();
		bVar  = D3DDevice_GetOverlayUpdateStatus();
		/*void*/D3DDevice_GetDisplayFieldStatus(pFieldStatus);
		/*void*/D3DDevice_SetVertexData2f(n, f, f);
		/*void*/D3DDevice_SetVertexData4f(n, f, f, f, f);
		/*void*/D3DDevice_SetVertexData2s(n, s, s);
		/*void*/D3DDevice_SetVertexData4s(n, s, s, s, s);
		/*void*/D3DDevice_SetVertexData4ub(n, byte, byte, byte, byte);
		/*void*/D3DDevice_SetVertexDataColor(n, Color);
		/*void*/D3DDevice_Begin(PrimitiveType);
		/*void*/D3DDevice_End();
		hrVar = D3DDevice_CreateFixup(ui, ppFixup);
		hrVar = D3DDevice_CreatePushBuffer(ui, b, ppPushBuffer);
		/*void*/D3DDevice_BeginPushBuffer(pPushBuffer);
		hrVar = D3DDevice_EndPushBuffer();
		/*void*/D3DDevice_RunPushBuffer(pPushBuffer, pFixup);
		/*void*/D3DDevice_GetPushBufferOffset(pdw);
		/*void*/D3DDevice_Nop();
		/*void*/D3DDevice_GetProjectionViewportMatrix(pMatrix);
		/*void*/D3DDevice_SetModelView(/*CONST*/ pMatrix, /*CONST*/ pMatrix, /*CONST*/ pMatrix);
		hrVar = D3DDevice_GetModelView(pMatrix);
		/*void*/D3DDevice_SetVertexBlendModelView(dw, /*CONST*/ pMatrix, /*CONST*/ pMatrix, /*CONST*/ pMatrix);
		hrVar = D3DDevice_GetVertexBlendModelView(dw, pMatrix, /*CONST*/ pMatrix);
		/*void*/D3DDevice_SetVertexShaderInput(dw, ui, /*CONST*/ pStreamInputs);
		hrVar = D3DDevice_GetVertexShaderInput(pdw, pui, pStreamInputs);
		/*void*//*CALL*/ D3DDevice_SwitchTexture(dw, dw, Format);
		/*void*/D3DDevice_Suspend();
		/*void*/D3DDevice_Resume(b);
		/*void*/D3DDevice_SetScissors(dw, b, pD3DRect);
		/*void*/D3DDevice_GetScissors(pdw, pb, pD3DRect);
		/*void*/D3DDevice_SetTile(dw, pTile);
		/*void*/D3DDevice_GetTile(dw, pTile);
		dwVar = D3DDevice_GetTileCompressionTags(dw, dw);
		/*void*/D3DDevice_SetRenderState(RenderStateType, dw);
		/*void*/D3DDevice_GetRenderState(RenderStateType, pdw);
		/*void*/D3DDevice_SetTextureStageState(dw, CallbackType, dw);
		/*void*/D3DDevice_GetTextureStageState(dw, CallbackType, pdw);
		
		ulVar = IDirect3DDevice8_AddRef(pThisDevice);
		ulVar = IDirect3DDevice8_Release(pThisDevice);
		hrVar = IDirect3DDevice8_GetDirect3D(pThisDevice, ppDirect3D);
		hrVar = IDirect3DDevice8_GetDeviceCaps(pThisDevice, pCaps);
		hrVar = IDirect3DDevice8_GetDisplayMode(pThisDevice, pDisplayMode);
		hrVar = IDirect3DDevice8_GetCreationParameters(pThisDevice, pCreationParameters);
		hrVar = IDirect3DDevice8_Reset(pThisDevice, pPresentationParameters);
		hrVar = IDirect3DDevice8_Present(pThisDevice, pRect, pRect, pv, pv);
		hrVar = IDirect3DDevice8_GetBackBuffer(pThisDevice, n, CallbackType, ppSurface);
		hrVar = IDirect3DDevice8_GetRasterStatus(pThisDevice, pRasterStatus);
		/*void*/IDirect3DDevice8_SetFlickerFilter(pThisDevice, dw);
		/*void*/IDirect3DDevice8_SetGammaRamp(pThisDevice, dw, pGammaRamp);
		/*void*/IDirect3DDevice8_GetGammaRamp(pThisDevice, pGammaRamp);
		hrVar = IDirect3DDevice8_CreateTexture(pThisDevice, ui, ui, ui, dw, Format, Pool, ppTexture);
		hrVar = IDirect3DDevice8_CreateVolumeTexture(pThisDevice, ui, ui, ui, ui, dw, Format, Pool, ppVolumeTexture);
		hrVar = IDirect3DDevice8_CreateCubeTexture(pThisDevice, ui, ui, dw, Format, Pool, ppCubeTexture);
		hrVar = IDirect3DDevice8_CreateVertexBuffer(pThisDevice, ui, dw, dw, Pool, ppVertexBuffer);
		hrVar = IDirect3DDevice8_CreateIndexBuffer(pThisDevice, ui, dw, Format, Pool, ppIndexBuffer);
		hrVar = IDirect3DDevice8_CreatePalette(pThisDevice, ui, ppPalette);
		hrVar = IDirect3DDevice8_CreateRenderTarget(pThisDevice, ui, ui, Format, MultiSampleType, b, ppSurface);
		hrVar = IDirect3DDevice8_CreateDepthStencilSurface(pThisDevice, ui, ui, Format, MultiSampleType, ppSurface);
		hrVar = IDirect3DDevice8_CreateImageSurface(pThisDevice, ui, ui, Format, ppSurface);
		hrVar = IDirect3DDevice8_CopyRects(pThisDevice, pSurface, pRect, ui, pSurface, pPoint);
		hrVar = IDirect3DDevice8_UpdateTexture(pThisDevice, pBaseTexture, pBaseTexture);
		hrVar = IDirect3DDevice8_SetRenderTarget(pThisDevice, pSurface, pSurface);
		hrVar = IDirect3DDevice8_GetRenderTarget(pThisDevice, ppSurface);
		hrVar = IDirect3DDevice8_GetDepthStencilSurface(pThisDevice, ppSurface);
		hrVar = IDirect3DDevice8_BeginScene(pThisDevice);
		hrVar = IDirect3DDevice8_EndScene(pThisDevice);
		hrVar = IDirect3DDevice8_Clear(pThisDevice, dw, pD3DRect, dw, Color, f, dw);
		hrVar = IDirect3DDevice8_SetTransform(pThisDevice, TransformStateType, pMatrix);
		hrVar = IDirect3DDevice8_GetTransform(pThisDevice, TransformStateType, pMatrix);
		hrVar = IDirect3DDevice8_MultiplyTransform(pThisDevice, TransformStateType, pMatrix);
		hrVar = IDirect3DDevice8_SetViewport(pThisDevice, /*CONST*/ pViewport);
		hrVar = IDirect3DDevice8_GetViewport(pThisDevice, pViewport);
		hrVar = IDirect3DDevice8_SetMaterial(pThisDevice, /*CONST*/ pMaterial);
		hrVar = IDirect3DDevice8_GetMaterial(pThisDevice, pMaterial);
		hrVar = IDirect3DDevice8_SetBackMaterial(pThisDevice, /*CONST*/ pMaterial);
		hrVar = IDirect3DDevice8_GetBackMaterial(pThisDevice, pMaterial);
		hrVar = IDirect3DDevice8_SetLight(pThisDevice, dw, /*CONST*/ pLight);
		hrVar = IDirect3DDevice8_GetLight(pThisDevice, dw, pLight);
		hrVar = IDirect3DDevice8_LightEnable(pThisDevice, dw, b);
		hrVar = IDirect3DDevice8_GetLightEnable(pThisDevice, dw, pb);
		hrVar = IDirect3DDevice8_SetRenderState(pThisDevice, RenderStateType, dw);
		hrVar = IDirect3DDevice8_SetRenderStateNotInline(pThisDevice, RenderStateType, dw);
		hrVar = IDirect3DDevice8_GetRenderState(pThisDevice, RenderStateType, pdw);
#pragma MESSAGE( "Removed NotInline functions from API" )
//		hrVar = IDirect3DDevice8_GetRenderStateNotInline(pThisDevice, RenderStateType, pdw);
		
#if D3DCOMPILE_BEGINSTATEBLOCK
		hrVar = IDirect3DDevice8_BeginStateBlock(pThisDevice);
		hrVar = IDirect3DDevice8_EndStateBlock(pThisDevice, pdw);
#endif
		
		hrVar = IDirect3DDevice8_ApplyStateBlock(pThisDevice, dw);
		hrVar = IDirect3DDevice8_CaptureStateBlock(pThisDevice, dw);
		hrVar = IDirect3DDevice8_DeleteStateBlock(pThisDevice, dw);
		hrVar = IDirect3DDevice8_CreateStateBlock(pThisDevice, CallbackType,pdw);
		hrVar = IDirect3DDevice8_GetTexture(pThisDevice, dw, ppBaseTexture);
		hrVar = IDirect3DDevice8_SetTexture(pThisDevice, dw, pBaseTexture);
		hrVar = IDirect3DDevice8_GetPalette(pThisDevice, dw, ppPalette);
		hrVar = IDirect3DDevice8_SetPalette(pThisDevice, dw, pPalette);
		hrVar = IDirect3DDevice8_GetTextureStageState(pThisDevice, dw, CallbackType, pdw);
#pragma MESSAGE( "Removed NotInline functions from API" )
//		hrVar = IDirect3DDevice8_GetTextureStageStateNotInline(pThisDevice, dw, TextureStageStateType, pdw);

		hrVar = IDirect3DDevice8_SetTextureStageState(pThisDevice, dw, CallbackType, dw);
		hrVar = IDirect3DDevice8_SetTextureStageStateNotInline(pThisDevice, dw, CallbackType, dw);
		hrVar = IDirect3DDevice8_DrawPrimitive(pThisDevice, PrimitiveType, ui, ui);
		hrVar = IDirect3DDevice8_DrawIndexedPrimitive(pThisDevice, PrimitiveType, ui, ui, ui, ui);
		hrVar = IDirect3DDevice8_DrawPrimitiveUP(pThisDevice, PrimitiveType, ui, /*CONST*/ pv, ui);
		hrVar = IDirect3DDevice8_DrawIndexedPrimitiveUP(pThisDevice, PrimitiveType, ui, ui, ui, /*CONST*/ pIndexBuffer, Format, /*CONST*/ pv, ui);
		hrVar = IDirect3DDevice8_DrawVertices(pThisDevice, PrimitiveType, ui, ui);
		hrVar = IDirect3DDevice8_DrawIndexedVertices(pThisDevice, PrimitiveType, ui, pw);
		hrVar = IDirect3DDevice8_DrawVerticesUP(pThisDevice, PrimitiveType, ui, /*CONST*/ pv, ui);
		hrVar = IDirect3DDevice8_DrawIndexedVerticesUP(pThisDevice, PrimitiveType, ui, /*CONST*/ pw, /*CONST*/ pv, ui);
		hrVar = IDirect3DDevice8_PrimeVertexCache(pThisDevice, ui, /*CONST*/ pw);
		hrVar = IDirect3DDevice8_CreateVertexShader(pThisDevice, /*CONST*/ pdw, /*CONST*/ pdw, pdw, dw);
		hrVar = IDirect3DDevice8_SetVertexShader(pThisDevice, dw);
		hrVar = IDirect3DDevice8_GetVertexShader(pThisDevice, pdw);
		hrVar = IDirect3DDevice8_DeleteVertexShader(pThisDevice, dw);
		hrVar = IDirect3DDevice8_SetVertexShaderConstant(pThisDevice, n, /*CONST*/ pv, dw);
		hrVar = IDirect3DDevice8_GetVertexShaderConstant(pThisDevice, n, pv, dw);
		hrVar = IDirect3DDevice8_SetShaderConstantMode(pThisDevice, ui);
		hrVar = IDirect3DDevice8_GetShaderConstantMode(pThisDevice, pShaderConstantMode);
		hrVar = IDirect3DDevice8_LoadVertexShader(pThisDevice, dw, dw);
		hrVar = IDirect3DDevice8_LoadVertexShaderProgram(pThisDevice, pdw, dw);
		hrVar = IDirect3DDevice8_SelectVertexShader(pThisDevice, dw, dw);
		hrVar = IDirect3DDevice8_RunVertexStateShader(pThisDevice, dw, /*CONST*/ pf);
		hrVar = IDirect3DDevice8_GetVertexShaderSize(pThisDevice, dw, pui);
		hrVar = IDirect3DDevice8_GetVertexShaderType(pThisDevice, dw, pdw);
		hrVar = IDirect3DDevice8_GetVertexShaderDeclaration(pThisDevice, dw, pf, pdw);
		hrVar = IDirect3DDevice8_GetVertexShaderFunction(pThisDevice, dw,pf, pdw);
		hrVar = IDirect3DDevice8_SetStreamSource(pThisDevice, ui, pVertexBuffer, ui);
		hrVar = IDirect3DDevice8_GetStreamSource(pThisDevice, ui, ppVertexBuffer, pui);
		hrVar = IDirect3DDevice8_SetIndices(pThisDevice, pIndexBuffer, ui);
		hrVar = IDirect3DDevice8_GetIndices(pThisDevice, ppIndexBuffer, pui);
		hrVar = IDirect3DDevice8_CreatePixelShader(pThisDevice, /*CONST*/ pPixelShaderDef, pdw);
		hrVar = IDirect3DDevice8_SetPixelShader(pThisDevice, dw);
		hrVar = IDirect3DDevice8_SetPixelShaderProgram(pThisDevice, pPixelShaderDef);
		hrVar = IDirect3DDevice8_GetPixelShader(pThisDevice, pdw);
		hrVar = IDirect3DDevice8_DeletePixelShader(pThisDevice, dw);
		hrVar = IDirect3DDevice8_SetPixelShaderConstant(pThisDevice, n, /*CONST*/ pv, dw);
		hrVar = IDirect3DDevice8_GetPixelShaderConstant(pThisDevice, n, pv, dw);
		hrVar = IDirect3DDevice8_GetPixelShaderFunction(pThisDevice, dw, pPixelShaderDef);
		hrVar = IDirect3DDevice8_DrawRectPatch(pThisDevice, dw, /*CONST*/ pf, /*CONST*/ pRectPatchInfo);
		hrVar = IDirect3DDevice8_DrawTriPatch(pThisDevice, dw, /*CONST*/ pf, /*CONST*/ pTriPatchInfo);
		hrVar = IDirect3DDevice8_DeletePatch(pThisDevice, dw);
		bVar  = IDirect3DDevice8_IsBusy(pThisDevice);
		/*void*/IDirect3DDevice8_BlockUntilIdle(pThisDevice);
		/*void*/IDirect3DDevice8_KickPushBuffer(pThisDevice);
		/*void*/IDirect3DDevice8_SetVerticalBlankCallback(pThisDevice, Callback);
		/*void*/IDirect3DDevice8_BlockUntilVerticalBlank(pThisDevice);
		dwVar = IDirect3DDevice8_InsertFence(pThisDevice);
		bVar  = IDirect3DDevice8_IsFencePending(pThisDevice, dw);
		/*void*/IDirect3DDevice8_BlockOnFence(pThisDevice, dw);
		/*void*/IDirect3DDevice8_InsertCallback(pThisDevice, CallbackType, Callback, dw);
		/*void*/IDirect3DDevice8_FlushVertexCache(pThisDevice);
		hrVar = IDirect3DDevice8_PersistDisplay(pThisDevice);
		hrVar = IDirect3DDevice8_UpdateOverlay(pThisDevice, pSurface, pRect, pRect, b, Color);
		hrVar = IDirect3DDevice8_EnableOverlay(pThisDevice, b);
		hrVar = IDirect3DDevice8_EnableCC(pThisDevice, b);
		hrVar = IDirect3DDevice8_SendCC(pThisDevice, b, byte, byte);
		hrVar = IDirect3DDevice8_GetCCStatus(pThisDevice, pb, pb);
		hrVar = IDirect3DDevice8_BeginVisibilityTest(pThisDevice);
		hrVar = IDirect3DDevice8_EndVisibilityTest(pThisDevice, dw);
		hrVar = IDirect3DDevice8_GetVisibilityTestResult(pThisDevice, dw, pui, pull);
		bVar  = IDirect3DDevice8_GetOverlayUpdateStatus(pThisDevice);
		hrVar = IDirect3DDevice8_GetDisplayFieldStatus(pThisDevice, pFieldStatus);
		hrVar = IDirect3DDevice8_SetVertexData2f(pThisDevice, n, f, f);
		hrVar = IDirect3DDevice8_SetVertexData4f(pThisDevice, n, f, f, f, f);
		hrVar = IDirect3DDevice8_SetVertexData2s(pThisDevice, n, s, s);
		hrVar = IDirect3DDevice8_SetVertexData4s(pThisDevice, n, s, s, s, s);
		hrVar = IDirect3DDevice8_SetVertexData4ub(pThisDevice, n, byte, byte, byte, byte);
		hrVar = IDirect3DDevice8_SetVertexDataColor(pThisDevice, n, Color);
		hrVar = IDirect3DDevice8_Begin(pThisDevice, PrimitiveType);
		hrVar = IDirect3DDevice8_End(pThisDevice);
		hrVar = IDirect3DDevice8_CreateFixup(pThisDevice, ui, ppFixup);
		hrVar = IDirect3DDevice8_CreatePushBuffer(pThisDevice, ui, b, ppPushBuffer);
		hrVar = IDirect3DDevice8_BeginPushBuffer(pThisDevice, pPushBuffer);
		hrVar = IDirect3DDevice8_EndPushBuffer(pThisDevice);
		hrVar = IDirect3DDevice8_RunPushBuffer(pThisDevice, pPushBuffer, pFixup);
		hrVar = IDirect3DDevice8_GetPushBufferOffset(pThisDevice, pdw);
		hrVar = IDirect3DDevice8_Nop(pThisDevice);
		hrVar = IDirect3DDevice8_GetProjectionViewportMatrix(pThisDevice, pMatrix);
		hrVar = IDirect3DDevice8_SetModelView(pThisDevice, /*CONST*/ pMatrix, /*CONST*/ pMatrix, /*CONST*/ pMatrix);
		hrVar = IDirect3DDevice8_GetModelView(pThisDevice, pMatrix);
		hrVar = IDirect3DDevice8_SetVertexBlendModelView(pThisDevice, dw, /*CONST*/ pMatrix, /*CONST*/ pMatrix, /*CONST*/ pMatrix);
		hrVar = IDirect3DDevice8_GetVertexBlendModelView(pThisDevice, dw, pMatrix, /*CONST*/ pMatrix);
		hrVar = IDirect3DDevice8_SetVertexShaderInput(pThisDevice, dw, ui, /*CONST*/ pStreamInputs);
		hrVar = IDirect3DDevice8_GetVertexShaderInput(pThisDevice, pdw, pui, pStreamInputs);
		hrVar = IDirect3DDevice8_SwitchTexture(pThisDevice, dw, pBaseTexture);
		hrVar = IDirect3DDevice8_Suspend(pThisDevice);
		hrVar = IDirect3DDevice8_Resume(pThisDevice, b);
		hrVar = IDirect3DDevice8_SetScissors(pThisDevice, dw, b, pD3DRect);
		hrVar = IDirect3DDevice8_GetScissors(pThisDevice, pdw, pb, pD3DRect);
		hrVar = IDirect3DDevice8_SetTile(pThisDevice, dw, pTile);
		hrVar = IDirect3DDevice8_GetTile(pThisDevice, dw, pTile);
		dwVar = IDirect3DDevice8_GetTileCompressionTags(pThisDevice, dw, dw);
		

		/* D3DResource */
		ulVar = D3DResource_AddRef(pThisResource);
		ulVar = D3DResource_Release(pThisResource);
		/*void*/D3DResource_GetDevice(pThisResource, ppDevice);
		hrVar = D3DResource_SetPrivateData(pThisResource, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DResource_GetPrivateData(pThisResource, refguid, pf, pdw);
		/*void*/D3DResource_FreePrivateData(pThisResource, refguid);
		d3drt = D3DResource_GetType(pThisResource);
		bVar  = D3DResource_IsBusy(pThisResource);
		/*void*/D3DResource_BlockUntilNotBusy(pThisResource);
		/*void*/D3DResource_Register(pThisResource, pv);
		/*void*/D3DResource_MoveResourceMemory(pThisResource, where);
		
		ulVar = IDirect3DResource8_AddRef(pThisResource);
		ulVar = IDirect3DResource8_Release(pThisResource);
		hrVar = IDirect3DResource8_GetDevice(pThisResource, ppDevice);
		hrVar = IDirect3DResource8_SetPrivateData(pThisResource, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DResource8_GetPrivateData(pThisResource, refguid, pf, pdw);
		hrVar = IDirect3DResource8_FreePrivateData(pThisResource, refguid);
		d3drt = IDirect3DResource8_GetType(pThisResource);
		bVar  = IDirect3DResource8_IsBusy(pThisResource);
		/*void*/IDirect3DResource8_BlockUntilNotBusy(pThisResource);
		/*void*/IDirect3DResource8_MoveResourceMemory(pThisResource, where);
		/*void*/IDirect3DResource8_Register(pThisResource, pv);
		

		/* D3DBaseTexture */	
		ulVar = D3DBaseTexture_AddRef(pThisBaseTexture);
		ulVar = D3DBaseTexture_Release(pThisBaseTexture);
		/*void*/D3DBaseTexture_GetDevice(pThisBaseTexture, ppDevice);
		d3drt = D3DBaseTexture_GetType(pThisBaseTexture);
		bVar  = D3DBaseTexture_IsBusy(pThisBaseTexture);
		/*void*/D3DBaseTexture_BlockUntilNotBusy(pThisBaseTexture);
		/*void*/D3DBaseTexture_MoveResourceMemory(pThisBaseTexture, where);
		/*void*/D3DBaseTexture_Register(pThisBaseTexture, pv);
		hrVar = D3DBaseTexture_SetPrivateData(pThisBaseTexture, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DBaseTexture_GetPrivateData(pThisBaseTexture, refguid, pf, pdw);
		/*void*/D3DBaseTexture_FreePrivateData(pThisBaseTexture, refguid);
		dwVar = D3DBaseTexture_GetLevelCount(pThisBaseTexture);
		
		ulVar = IDirect3DBaseTexture8_AddRef(pThisBaseTexture);
		ulVar = IDirect3DBaseTexture8_Release(pThisBaseTexture);
		hrVar = IDirect3DBaseTexture8_GetDevice(pThisBaseTexture, ppDevice);
		d3drt = IDirect3DBaseTexture8_GetType(pThisBaseTexture);
		bVar  = IDirect3DBaseTexture8_IsBusy(pThisBaseTexture);
		/*void*/IDirect3DBaseTexture8_BlockUntilNotBusy(pThisBaseTexture);
		/*void*/IDirect3DBaseTexture8_MoveResourceMemory(pThisBaseTexture, where);
		/*void*/IDirect3DBaseTexture8_Register(pThisBaseTexture, pv);
		hrVar = IDirect3DBaseTexture8_SetPrivateData(pThisBaseTexture, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DBaseTexture8_GetPrivateData(pThisBaseTexture, refguid, pf, pdw);
		hrVar = IDirect3DBaseTexture8_FreePrivateData(pThisBaseTexture, refguid);
		dwVar = IDirect3DBaseTexture8_GetLevelCount(pThisBaseTexture);
		

		/* D3DTexture */
		ulVar = D3DTexture_AddRef(pThisTexture);
		ulVar = D3DTexture_Release(pThisTexture);
		/*void*/D3DTexture_GetDevice(pThisTexture, ppDevice);
		d3drt = D3DTexture_GetType(pThisTexture);
		bVar  = D3DTexture_IsBusy(pThisTexture);
		/*void*/D3DTexture_BlockUntilNotBusy(pThisTexture);
		/*void*/D3DTexture_MoveResourceMemory(pThisTexture, where);
		/*void*/D3DTexture_Register(pThisTexture, pv);
		dwVar = D3DTexture_GetLevelCount(pThisTexture);
		hrVar = D3DTexture_SetPrivateData(pThisTexture, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DTexture_GetPrivateData(pThisTexture, refguid, pf, pdw);
		/*void*/D3DTexture_FreePrivateData(pThisTexture, refguid);
		/*void*/D3DTexture_GetLevelDesc(pThisTexture, ui, pSurfaceDesc);
		hrVar = D3DTexture_GetSurfaceLevel(pThisTexture, ui, ppSurface);
		/*void*/D3DTexture_LockRect(pThisTexture, ui, pLockedRect, /*CONST*/ pRect, dw);
		/*void*/D3DTexture_UnlockRect(pThisTexture, ui);
		
		ulVar = IDirect3DTexture8_AddRef(pThisTexture);
		ulVar = IDirect3DTexture8_Release(pThisTexture);
		hrVar = IDirect3DTexture8_GetDevice(pThisTexture, ppDevice);
		d3drt = IDirect3DTexture8_GetType(pThisTexture);
		bVar  = IDirect3DTexture8_IsBusy(pThisTexture);
		/*void*/IDirect3DTexture8_BlockUntilNotBusy(pThisTexture);
		/*void*/IDirect3DTexture8_MoveResourceMemory(pThisTexture, where);
		/*void*/IDirect3DTexture8_Register(pThisTexture, pv);
		dwVar = IDirect3DTexture8_GetLevelCount(pThisTexture);
		hrVar = IDirect3DTexture8_SetPrivateData(pThisTexture, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DTexture8_GetPrivateData(pThisTexture, refguid, pf, pdw);
		hrVar = IDirect3DTexture8_FreePrivateData(pThisTexture, refguid);
		hrVar = IDirect3DTexture8_GetLevelDesc(pThisTexture, ui, pSurfaceDesc);
		hrVar = IDirect3DTexture8_GetSurfaceLevel(pThisTexture, ui, ppSurface);
		hrVar = IDirect3DTexture8_LockRect(pThisTexture, ui, pLockedRect, /*CONST*/ pRect, dw);
		hrVar = IDirect3DTexture8_UnlockRect(pThisTexture, ui);
		

		/* D3DVolumeTexture */	
		ulVar = D3DVolumeTexture_AddRef(pThisVolumeTexture);
		ulVar = D3DVolumeTexture_Release(pThisVolumeTexture);
		/*void*/D3DVolumeTexture_GetDevice(pThisVolumeTexture, ppDevice);
		d3drt = D3DVolumeTexture_GetType(pThisVolumeTexture);
		bVar  = D3DVolumeTexture_IsBusy(pThisVolumeTexture);
		/*void*/D3DVolumeTexture_BlockUntilNotBusy(pThisVolumeTexture);
		/*void*/D3DVolumeTexture_MoveResourceMemory(pThisVolumeTexture, where);
		/*void*/D3DVolumeTexture_Register(pThisVolumeTexture, pv);
		dwVar = D3DVolumeTexture_GetLevelCount(pThisVolumeTexture);
		hrVar = D3DVolumeTexture_SetPrivateData(pThisVolumeTexture, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DVolumeTexture_GetPrivateData(pThisVolumeTexture, refguid, pf, pdw);
		/*void*/D3DVolumeTexture_FreePrivateData(pThisVolumeTexture, refguid);
		/*void*/D3DVolumeTexture_GetLevelDesc(pThisVolumeTexture, ui, pVolumeDesc);
		hrVar = D3DVolumeTexture_GetVolumeLevel(pThisVolumeTexture, ui, ppVolume);
		/*void*/D3DVolumeTexture_LockBox(pThisVolumeTexture, ui, pLockedBox, /*CONST*/ pBox, dw);
		/*void*/D3DVolumeTexture_UnlockBox(pThisVolumeTexture, ui);
	
		ulVar = IDirect3DVolumeTexture8_AddRef(pThisVolumeTexture);
		ulVar = IDirect3DVolumeTexture8_Release(pThisVolumeTexture);
		hrVar = IDirect3DVolumeTexture8_GetDevice(pThisVolumeTexture, ppDevice);
		d3drt = IDirect3DVolumeTexture8_GetType(pThisVolumeTexture);
		bVar  = IDirect3DVolumeTexture8_IsBusy(pThisVolumeTexture);
		/*void*/IDirect3DVolumeTexture8_BlockUntilNotBusy(pThisVolumeTexture);
		/*void*/IDirect3DVolumeTexture8_MoveResourceMemory(pThisVolumeTexture, where);
		/*void*/IDirect3DVolumeTexture8_Register(pThisVolumeTexture, pv);
		dwVar = IDirect3DVolumeTexture8_GetLevelCount(pThisVolumeTexture);
		hrVar = IDirect3DVolumeTexture8_SetPrivateData(pThisVolumeTexture, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DVolumeTexture8_GetPrivateData(pThisVolumeTexture, refguid, pf, pdw);
		hrVar = IDirect3DVolumeTexture8_FreePrivateData(pThisVolumeTexture, refguid);
		hrVar = IDirect3DVolumeTexture8_GetLevelDesc(pThisVolumeTexture, ui, pVolumeDesc);
		hrVar = IDirect3DVolumeTexture8_GetVolumeLevel(pThisVolumeTexture, ui, ppVolume);
		hrVar = IDirect3DVolumeTexture8_LockBox(pThisVolumeTexture, ui, pLockedBox, /*CONST*/ pBox, dw);
		hrVar = IDirect3DVolumeTexture8_UnlockBox(pThisVolumeTexture, ui);
		

		/* D3DCubeTexture */
		ulVar = D3DCubeTexture_AddRef(pThisCubeTexture);
		ulVar = D3DCubeTexture_Release(pThisCubeTexture);
		/*void*/D3DCubeTexture_GetDevice(pThisCubeTexture, ppDevice);
		d3drt = D3DCubeTexture_GetType(pThisCubeTexture);
		bVar  = D3DCubeTexture_IsBusy(pThisCubeTexture);
		/*void*/D3DCubeTexture_BlockUntilNotBusy(pThisCubeTexture);
		/*void*/D3DCubeTexture_MoveResourceMemory(pThisCubeTexture, where);
		/*void*/D3DCubeTexture_Register(pThisCubeTexture, pv);
		dwVar = D3DCubeTexture_GetLevelCount(pThisCubeTexture);
		hrVar = D3DCubeTexture_SetPrivateData(pThisCubeTexture, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DCubeTexture_GetPrivateData(pThisCubeTexture, refguid, pf, pdw);
		/*void*/D3DCubeTexture_FreePrivateData(pThisCubeTexture, refguid);
		/*void*/D3DCubeTexture_GetLevelDesc(pThisCubeTexture, ui, pSurfaceDesc);
		hrVar = D3DCubeTexture_GetCubeMapSurface(pThisCubeTexture, CubeMapFaces, ui, ppSurface);
		/*void*/D3DCubeTexture_LockRect(pThisCubeTexture, CubeMapFaces, ui, pLockedRect, /*CONST*/ pRect, dw);
		/*void*/D3DCubeTexture_UnlockRect(pThisCubeTexture, CubeMapFaces, ui);

		ulVar = IDirect3DCubeTexture8_AddRef(pThisCubeTexture);
		ulVar = IDirect3DCubeTexture8_Release(pThisCubeTexture);
		hrVar = IDirect3DCubeTexture8_GetDevice(pThisCubeTexture, ppDevice);
		d3drt = IDirect3DCubeTexture8_GetType(pThisCubeTexture);
		bVar  = IDirect3DCubeTexture8_IsBusy(pThisCubeTexture);
		/*void*/IDirect3DCubeTexture8_BlockUntilNotBusy(pThisCubeTexture);
		/*void*/IDirect3DCubeTexture8_MoveResourceMemory(pThisCubeTexture, where);
		/*void*/IDirect3DCubeTexture8_Register(pThisCubeTexture, pv);
		dwVar = IDirect3DCubeTexture8_GetLevelCount(pThisCubeTexture);
		hrVar = IDirect3DCubeTexture8_SetPrivateData(pThisCubeTexture, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DCubeTexture8_GetPrivateData(pThisCubeTexture, refguid, pf, pdw);
		hrVar = IDirect3DCubeTexture8_FreePrivateData(pThisCubeTexture, refguid);
		hrVar = IDirect3DCubeTexture8_GetLevelDesc(pThisCubeTexture, ui, pSurfaceDesc);
		hrVar = IDirect3DCubeTexture8_GetCubeMapSurface(pThisCubeTexture, CubeMapFaces, ui, ppSurface);
		hrVar = IDirect3DCubeTexture8_LockRect(pThisCubeTexture, CubeMapFaces, ui, pLockedRect, /*CONST*/ pRect, dw);
		hrVar = IDirect3DCubeTexture8_UnlockRect(pThisCubeTexture, CubeMapFaces, ui);
		

		/* D3DVertexBuffer */	
		ulVar = D3DVertexBuffer_AddRef(pThisVertexBuffer);
		ulVar = D3DVertexBuffer_Release(pThisVertexBuffer);
		/*void*/D3DVertexBuffer_GetDevice(pThisVertexBuffer, ppDevice);
		d3drt = D3DVertexBuffer_GetType(pThisVertexBuffer);
		bVar  = D3DVertexBuffer_IsBusy(pThisVertexBuffer);
		/*void*/D3DVertexBuffer_BlockUntilNotBusy(pThisVertexBuffer);
		/*void*/D3DVertexBuffer_MoveResourceMemory(pThisVertexBuffer, where);
		/*void*/D3DVertexBuffer_Register(pThisVertexBuffer, pv);
		hrVar = D3DVertexBuffer_SetPrivateData(pThisVertexBuffer, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DVertexBuffer_GetPrivateData(pThisVertexBuffer, refguid, pf, pdw);
		/*void*/D3DVertexBuffer_FreePrivateData(pThisVertexBuffer, refguid);
		/*void*/D3DVertexBuffer_Lock(pThisVertexBuffer, ui, ui, ppbyte, dw);
		/*void*/D3DVertexBuffer_GetDesc(pThisVertexBuffer, pVertexBufferDesc);
		/*void*/D3DVertexBuffer_Unlock(pThisVertexBuffer);

		ulVar = IDirect3DVertexBuffer8_AddRef(pThisVertexBuffer);
		ulVar = IDirect3DVertexBuffer8_Release(pThisVertexBuffer);
		hrVar = IDirect3DVertexBuffer8_GetDevice(pThisVertexBuffer, ppDevice);
		d3drt = IDirect3DVertexBuffer8_GetType(pThisVertexBuffer);
		bVar  = IDirect3DVertexBuffer8_IsBusy(pThisVertexBuffer);
		/*void*/IDirect3DVertexBuffer8_BlockUntilNotBusy(pThisVertexBuffer);
		/*void*/IDirect3DVertexBuffer8_MoveResourceMemory(pThisVertexBuffer, where);
		/*void*/IDirect3DVertexBuffer8_Register(pThisVertexBuffer, pv);
		hrVar = IDirect3DVertexBuffer8_SetPrivateData(pThisVertexBuffer, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DVertexBuffer8_GetPrivateData(pThisVertexBuffer, refguid, pf, pdw);
		hrVar = IDirect3DVertexBuffer8_FreePrivateData(pThisVertexBuffer, refguid);
		hrVar = IDirect3DVertexBuffer8_Lock(pThisVertexBuffer, ui, ui, ppbyte, dw);
		hrVar = IDirect3DVertexBuffer8_Unlock(pThisVertexBuffer);
		hrVar = IDirect3DVertexBuffer8_GetDesc(pThisVertexBuffer, pVertexBufferDesc);
		

		/* D3DIndexBuffer */	
		ulVar = D3DIndexBuffer_AddRef(pThisIndexBuffer);
		ulVar = D3DIndexBuffer_Release(pThisIndexBuffer);
		/*void*/D3DIndexBuffer_GetDevice(pThisIndexBuffer, ppDevice);
		d3drt = D3DIndexBuffer_GetType(pThisIndexBuffer);
		bVar  = D3DIndexBuffer_IsBusy(pThisIndexBuffer);
		/*void*/D3DIndexBuffer_BlockUntilNotBusy(pThisIndexBuffer);
		/*void*/D3DIndexBuffer_MoveResourceMemory(pThisIndexBuffer, where);
		/*void*/D3DIndexBuffer_Register(pThisIndexBuffer, pv);
		hrVar = D3DIndexBuffer_SetPrivateData(pThisIndexBuffer, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DIndexBuffer_GetPrivateData(pThisIndexBuffer, refguid, pf, pdw);
		/*void*/D3DIndexBuffer_FreePrivateData(pThisIndexBuffer, refguid);
		/*void*/D3DIndexBuffer_Lock(pThisIndexBuffer, ui, ui, ppbyte, dw);
		/*void*/D3DIndexBuffer_Unlock(pThisIndexBuffer);
		/*void*/D3DIndexBuffer_GetDesc(pThisIndexBuffer, pIndexBufferDesc);
		
		ulVar = IDirect3DIndexBuffer8_AddRef(pThisIndexBuffer);
		ulVar = IDirect3DIndexBuffer8_Release(pThisIndexBuffer);
		hrVar = IDirect3DIndexBuffer8_GetDevice(pThisIndexBuffer, ppDevice);
		d3drt = IDirect3DIndexBuffer8_GetType(pThisIndexBuffer);
		bVar  = IDirect3DIndexBuffer8_IsBusy(pThisIndexBuffer);
		/*void*/IDirect3DIndexBuffer8_BlockUntilNotBusy(pThisIndexBuffer);
		/*void*/IDirect3DIndexBuffer8_MoveResourceMemory(pThisIndexBuffer, where);
		/*void*/IDirect3DIndexBuffer8_Register(pThisIndexBuffer, pv);
		hrVar = IDirect3DIndexBuffer8_SetPrivateData(pThisIndexBuffer, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DIndexBuffer8_GetPrivateData(pThisIndexBuffer, refguid, pf, pdw);
		hrVar = IDirect3DIndexBuffer8_FreePrivateData(pThisIndexBuffer, refguid);
		hrVar = IDirect3DIndexBuffer8_Lock(pThisIndexBuffer, ui, ui, ppbyte, dw);
		hrVar = IDirect3DIndexBuffer8_Unlock(pThisIndexBuffer);
		hrVar = IDirect3DIndexBuffer8_GetDesc(pThisIndexBuffer, pIndexBufferDesc);
		

		/* D3DPalette */
		ulVar = D3DPalette_AddRef(pThisPalette);
		ulVar = D3DPalette_Release(pThisPalette);
		/*void*/D3DPalette_GetDevice(pThisPalette, ppDevice);
		d3drt = D3DPalette_GetType(pThisPalette);
		bVar  = D3DPalette_IsBusy(pThisPalette);
		/*void*/D3DPalette_BlockUntilNotBusy(pThisPalette);
		/*void*/D3DPalette_MoveResourceMemory(pThisPalette, where);
		/*void*/D3DPalette_Register(pThisPalette, pv);
		hrVar = D3DPalette_SetPrivateData(pThisPalette, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DPalette_GetPrivateData(pThisPalette, refguid, pf, pdw);
		/*void*/D3DPalette_FreePrivateData(pThisPalette, refguid);
		/*void*/D3DPalette_Lock(pThisPalette, ppColor, dw);
		d3dps = D3DPalette_GetSize(pThisPalette);
		/*void*/D3DPalette_Unlock(pThisPalette);

		ulVar = IDirect3DPalette8_AddRef(pThisPalette);
		ulVar = IDirect3DPalette8_Release(pThisPalette);
		hrVar = IDirect3DPalette8_GetDevice(pThisPalette, ppDevice);
		d3drt = IDirect3DPalette8_GetType(pThisPalette);
		bVar  = IDirect3DPalette8_IsBusy(pThisPalette);
		/*void*/IDirect3DPalette8_BlockUntilNotBusy(pThisPalette);
		/*void*/IDirect3DPalette8_MoveResourceMemory(pThisPalette, where);
		/*void*/IDirect3DPalette8_Register(pThisPalette, pv);
		hrVar = IDirect3DPalette8_SetPrivateData(pThisPalette, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DPalette8_GetPrivateData(pThisPalette, refguid, pf, pdw);
		hrVar = IDirect3DPalette8_FreePrivateData(pThisPalette, refguid);
		hrVar = IDirect3DPalette8_Lock(pThisPalette, ppColor, dw);
		hrVar = IDirect3DPalette8_Unlock(pThisPalette);
		d3dps = IDirect3DPalette8_GetSize(pThisPalette);
		

		/* D3DPushBuffer */
		ulVar = D3DPushBuffer_AddRef(pThisPushBuffer);
		ulVar = D3DPushBuffer_Release(pThisPushBuffer);
		/*void*/D3DPushBuffer_GetDevice(pThisPushBuffer, ppDevice);
		d3drt = D3DPushBuffer_GetType(pThisPushBuffer);
		bVar  = D3DPushBuffer_IsBusy(pThisPushBuffer);
		/*void*/D3DPushBuffer_BlockUntilNotBusy(pThisPushBuffer);
		/*void*/D3DPushBuffer_MoveResourceMemory(pThisPushBuffer, where);
		/*void*/D3DPushBuffer_Register(pThisPushBuffer, pv);
		hrVar = D3DPushBuffer_SetPrivateData(pThisPushBuffer, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DPushBuffer_GetPrivateData(pThisPushBuffer, refguid, pf, pdw);
		/*void*/D3DPushBuffer_FreePrivateData(pThisPushBuffer, refguid);
		/*void*/D3DPushBuffer_Verify(pPushBuffer, b);
		/*void*/D3DPushBuffer_BeginFixup(pPushBuffer, pFixup, b);
		hrVar = D3DPushBuffer_EndFixup(pPushBuffer);
		/*void*/D3DPushBuffer_RunPushBuffer(pPushBuffer, dw, pPushBuffer, pFixup);
		/*void*/D3DPushBuffer_SetModelView(pPushBuffer, dw, /*CONST*/ pMatrix, /*CONST*/ pMatrix, /*CONST*/ pMatrix);
		/*void*/D3DPushBuffer_SetVertexBlendModelView(pPushBuffer, dw, dw, /*CONST*/ pMatrix, /*CONST*/ pMatrix, /*CONST*/ pMatrix);
		/*void*/D3DPushBuffer_SetVertexShaderInput(pPushBuffer, dw, dw, ui, /*CONST*/ pStreamInputs);
		/*void*/D3DPushBuffer_SetRenderTarget(pPushBuffer, dw, pSurface, pSurface);
		/*void*/D3DPushBuffer_SetTexture(pPushBuffer, dw, dw, pBaseTexture);
		/*void*/D3DPushBuffer_SetPalette(pPushBuffer, dw, dw,pPalette);
		hrVar = D3DPushBuffer_EndVisibilityTest(pPushBuffer, dw, dw);
		/*void*/D3DPushBuffer_SetVertexShaderConstant(pPushBuffer, dw, n, /*CONST*/ pv, dw);
		/*void*/D3DPushBuffer_Jump(pPushBuffer, dw, ui);

		ulVar = IDirect3DPushBuffer8_AddRef(pThisPushBuffer);
		ulVar = IDirect3DPushBuffer8_Release(pThisPushBuffer);
		hrVar = IDirect3DPushBuffer8_GetDevice(pThisPushBuffer, ppDevice);
		d3drt = IDirect3DPushBuffer8_GetType(pThisPushBuffer);
		bVar  = IDirect3DPushBuffer8_IsBusy(pThisPushBuffer);
		/*void*/IDirect3DPushBuffer8_BlockUntilNotBusy(pThisPushBuffer);
		/*void*/IDirect3DPushBuffer8_MoveResourceMemory(pThisPushBuffer, where);
		/*void*/IDirect3DPushBuffer8_Register(pThisPushBuffer, pv);
		hrVar = IDirect3DPushBuffer8_SetPrivateData(pThisPushBuffer, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DPushBuffer8_GetPrivateData(pThisPushBuffer, refguid, pf, pdw);
		hrVar = IDirect3DPushBuffer8_FreePrivateData(pThisPushBuffer, refguid);
		hrVar = IDirect3DPushBuffer8_Verify(pPushBuffer, b);
		hrVar = IDirect3DPushBuffer8_BeginFixup(pPushBuffer, pFixup, b);
		hrVar = IDirect3DPushBuffer8_EndFixup(pPushBuffer);
		hrVar = IDirect3DPushBuffer8_RunPushBuffer(pPushBuffer, dw, pPushBuffer, pFixup);
		hrVar = IDirect3DPushBuffer8_SetModelView(pPushBuffer, dw, /*CONST*/ pMatrix, /*CONST*/ pMatrix, /*CONST*/ pMatrix);
		hrVar = IDirect3DPushBuffer8_SetVertexBlendModelView(pPushBuffer, dw, dw, /*CONST*/ pMatrix, /*CONST*/ pMatrix, /*CONST*/ pMatrix);
		hrVar = IDirect3DPushBuffer8_SetVertexShaderInput(pPushBuffer, dw, dw, ui, /*CONST*/ pStreamInputs);
		hrVar = IDirect3DPushBuffer8_SetRenderTarget(pPushBuffer, dw, pSurface, pSurface);
		hrVar = IDirect3DPushBuffer8_SetTexture(pPushBuffer, dw, dw, pBaseTexture);
		hrVar = IDirect3DPushBuffer8_SetPalette(pPushBuffer, dw, dw,pPalette);
		hrVar = IDirect3DPushBuffer8_EndVisibilityTest(pPushBuffer, dw, dw);
		hrVar = IDirect3DPushBuffer8_SetVertexShaderConstant(pPushBuffer, dw, n, /*CONST*/ pv, dw);
		hrVar = IDirect3DPushBuffer8_Jump(pPushBuffer, dw, ui);
		hrVar = IDirect3DPushBuffer8_GetSize(pPushBuffer, pui);
		

		/* D3DFixup */
		ulVar = D3DFixup_AddRef(pThisFixup);
		ulVar = D3DFixup_Release(pThisFixup);
		/*void*/D3DFixup_GetDevice(pThisFixup, ppDevice);
		d3drt = D3DFixup_GetType(pThisFixup);
		bVar  = D3DFixup_IsBusy(pThisFixup);
		/*void*/D3DFixup_BlockUntilNotBusy(pThisFixup);
		/*void*/D3DFixup_MoveResourceMemory(pThisFixup, where);
		/*void*/D3DFixup_Register(pThisFixup, pv);
		hrVar = D3DFixup_SetPrivateData(pThisFixup, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DFixup_GetPrivateData(pThisFixup, refguid, pf, pdw);
		/*void*/D3DFixup_FreePrivateData(pThisFixup, refguid);
		/*void*/D3DFixup_Reset(pFixup);
		/*void*/D3DFixup_GetSize(pFixup, pui);
		/*void*/D3DFixup_GetSpace(pFixup, pdw);

		ulVar = IDirect3DFixup8_AddRef(pThisFixup);
		ulVar = IDirect3DFixup8_Release(pThisFixup);
		hrVar = IDirect3DFixup8_GetDevice(pThisFixup, ppDevice);
		d3drt = IDirect3DFixup8_GetType(pThisFixup);
		bVar  = IDirect3DFixup8_IsBusy(pThisFixup);
		/*void*/IDirect3DFixup8_BlockUntilNotBusy(pThisFixup);
		/*void*/IDirect3DFixup8_MoveResourceMemory(pThisFixup, where);
		/*void*/IDirect3DFixup8_Register(pThisFixup, pv);
		hrVar = IDirect3DFixup8_SetPrivateData(pThisFixup, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DFixup8_GetPrivateData(pThisFixup, refguid, pf, pdw);
		hrVar = IDirect3DFixup8_FreePrivateData(pThisFixup, refguid);
		hrVar = IDirect3DFixup8_Reset(pFixup);                
		hrVar = IDirect3DFixup8_GetSize(pFixup, pdw); 
		hrVar = IDirect3DFixup8_GetSpace(pFixup, pdw); 
		

		/* D3DSurface */
		ulVar = D3DSurface_AddRef(pThisSurface);
		ulVar = D3DSurface_Release(pThisSurface);
		/*void*/D3DSurface_GetDevice(pThisSurface, ppDevice);
		d3drt = D3DSurface_GetType(pThisSurface);
		bVar  = D3DSurface_IsBusy(pThisSurface);
		/*void*/D3DSurface_BlockUntilNotBusy(pThisSurface);
		/*void*/D3DSurface_MoveResourceMemory(pThisSurface, where);
		/*void*/D3DSurface_Register(pThisSurface, pv);
		hrVar = D3DSurface_SetPrivateData(pThisSurface, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DSurface_GetPrivateData(pThisSurface, refguid, pf, pdw);
		/*void*/D3DSurface_FreePrivateData(pThisSurface, refguid);
		hrVar = D3DSurface_GetContainer(pThisSurface, ppBaseTexture);
		/*void*/D3DSurface_GetDesc(pThisSurface, pSurfaceDesc);
		/*void*/D3DSurface_LockRect(pThisSurface, pLockedRect,/*CONST*/ pRect, dw);
		/*void*/D3DSurface_UnlockRect(pThisSurface);

		ulVar = IDirect3DSurface8_AddRef(pThisSurface);
		ulVar = IDirect3DSurface8_Release(pThisSurface);
		hrVar = IDirect3DSurface8_GetDevice(pThisSurface, ppDevice);
		d3drt = IDirect3DSurface8_GetType(pThisSurface);
		bVar  = IDirect3DSurface8_IsBusy(pThisSurface);
		/*void*/IDirect3DSurface8_BlockUntilNotBusy(pThisSurface);
		/*void*/IDirect3DSurface8_MoveResourceMemory(pThisSurface, where);
		/*void*/IDirect3DSurface8_Register(pThisSurface, pv);
		hrVar = IDirect3DSurface8_SetPrivateData(pThisSurface, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DSurface8_GetPrivateData(pThisSurface, refguid, pf, pdw);
		hrVar = IDirect3DSurface8_FreePrivateData(pThisSurface, refguid);
		hrVar = IDirect3DSurface8_GetContainer(pThisSurface, ppBaseTexture);
		hrVar = IDirect3DSurface8_GetDesc(pThisSurface, pSurfaceDesc);
		hrVar = IDirect3DSurface8_LockRect(pThisSurface, pLockedRect,/*CONST*/ pRect, dw);
		hrVar = IDirect3DSurface8_UnlockRect(pThisSurface);
		

		/* D3DVolume */
		ulVar = D3DVolume_AddRef(pThisVolume);
		ulVar = D3DVolume_Release(pThisVolume);
		/*void*/D3DVolume_GetDevice(pThisVolume, ppDevice);
		d3drt = D3DVolume_GetType(pThisVolume);
		bVar  = D3DVolume_IsBusy(pThisVolume);
		/*void*/D3DVolume_BlockUntilNotBusy(pThisVolume);
		/*void*/D3DVolume_MoveResourceMemory(pThisVolume, where);
		/*void*/D3DVolume_Register(pThisVolume, pv);
		hrVar = D3DVolume_SetPrivateData(pThisVolume, refguid, /*CONST*/ pf, dw, dw);
		hrVar = D3DVolume_GetPrivateData(pThisVolume, refguid, pf, pdw);
		/*void*/D3DVolume_FreePrivateData(pThisVolume, refguid);
		/*void*/D3DVolume_GetContainer(pThisVolume, ppBaseTexture);
		/*void*/D3DVolume_GetDesc(pThisVolume, pVolumeDesc);
		/*void*/D3DVolume_LockBox(pThisVolume, pLockedBox, /*CONST*/ pBox, dw);
		/*void*/D3DVolume_UnlockBox(pThisVolume);

		ulVar = IDirect3DVolume8_AddRef(pThisVolume);
		ulVar = IDirect3DVolume8_Release(pThisVolume);
		hrVar = IDirect3DVolume8_GetDevice(pThisVolume, ppDevice);
		d3drt = IDirect3DVolume8_GetType(pThisVolume);
		bVar  = IDirect3DVolume8_IsBusy(pThisVolume);
		/*void*/IDirect3DVolume8_BlockUntilNotBusy(pThisVolume);
		/*void*/IDirect3DVolume8_MoveResourceMemory(pThisVolume, where);
		/*void*/IDirect3DVolume8_Register(pThisVolume, pv);
		hrVar = IDirect3DVolume8_SetPrivateData(pThisVolume, refguid, /*CONST*/ pf, dw, dw);
		hrVar = IDirect3DVolume8_GetPrivateData(pThisVolume, refguid, pf, pdw);
		hrVar = IDirect3DVolume8_FreePrivateData(pThisVolume, refguid);
		hrVar = IDirect3DVolume8_GetContainer(pThisVolume, ppBaseTexture);
		hrVar = IDirect3DVolume8_GetDesc(pThisVolume, pVolumeDesc);
		hrVar = IDirect3DVolume8_LockBox(pThisVolume, pLockedBox, /*CONST*/ pBox, dw);
		hrVar = IDirect3DVolume8_UnlockBox(pThisVolume);
		
		//------------------------------------
		// end d3d8.h
		//------------------------------------
	}


	if ( bRunTestsD3DX )
	{
		//------------------------------------
		// begin d3dx8.h
		//------------------------------------

		/* Direct3DX Variables */
		ID3DXBuffer*						pXBuffer					= NULL;
		ID3DXSprite*						pXSprite					= NULL;
		ID3DXRenderToSurface*				pXRenderToSurface			= NULL;
		ID3DXRenderToEnvMap*				pXRenderToEnvMap			= NULL;
		ID3DXTechnique*						pXTechnique					= NULL;
		ID3DXEffect*						pXEffect					= NULL;
		ID3DXMatrixStack*					pXMatrixStack				= NULL;
		ID3DXBaseMesh*						pXBaseMesh					= NULL;
		ID3DXMesh*							pXMesh						= NULL;
		ID3DXPMesh*							pXPMesh						= NULL;
		ID3DXSkinMesh*						pXSkinMesh					= NULL;


		/* d3dx8core.h */

		/* ID3DXBuffer */
		hrVar = pXBuffer->lpVtbl->QueryInterface( pXBuffer, iid, ppv );
		hrVar = pXBuffer->lpVtbl->AddRef( pXBuffer );
		hrVar = pXBuffer->lpVtbl->Release( pXBuffer );
		pvVar = pXBuffer->lpVtbl->GetBufferPointer( pXBuffer );
		hrVar = pXBuffer->lpVtbl->GetBufferSize( pXBuffer );

		/* ID3DXSprite */
		hrVar = pXSprite->lpVtbl->QueryInterface( pXSprite, iid, ppv);
		hrVar = pXSprite->lpVtbl->AddRef( pXSprite );
		hrVar = pXSprite->lpVtbl->Release( pXSprite );
		hrVar = pXSprite->lpVtbl->GetDevice( pXSprite, ppDevice);
		hrVar = pXSprite->lpVtbl->Begin( pXSprite );
		hrVar = pXSprite->lpVtbl->Draw( pXSprite, pTexture, pRect, pV2, pV2, f, pV2, Color);
		hrVar = pXSprite->lpVtbl->DrawTransform( pXSprite,  pTexture, pRect, pM, Color);
		hrVar = pXSprite->lpVtbl->End( pXSprite );

		/* ID3DXRenderToSurface */
		hrVar = pXRenderToSurface->lpVtbl->QueryInterface(pXRenderToSurface, iid, ppv);
		hrVar = pXRenderToSurface->lpVtbl->AddRef( pXRenderToSurface );
		hrVar = pXRenderToSurface->lpVtbl->Release( pXRenderToSurface );
		hrVar = pXRenderToSurface->lpVtbl->GetDevice( pXRenderToSurface, ppDevice ) ;
		hrVar = pXRenderToSurface->lpVtbl->GetDesc( pXRenderToSurface, pRTSDesc);
		hrVar = pXRenderToSurface->lpVtbl->BeginScene( pXRenderToSurface, pSurface, pViewport);
		hrVar = pXRenderToSurface->lpVtbl->EndScene( pXRenderToSurface);
		/* ID3DXRenderToEnvMap */
		hrVar = pXRenderToEnvMap->lpVtbl->QueryInterface(pXRenderToEnvMap, iid, ppv);
		hrVar = pXRenderToEnvMap->lpVtbl->AddRef(pXRenderToEnvMap);
		hrVar = pXRenderToEnvMap->lpVtbl->Release(pXRenderToEnvMap);
		hrVar = pXRenderToEnvMap->lpVtbl->GetDevice( pXRenderToEnvMap, ppDevice);
		hrVar = pXRenderToEnvMap->lpVtbl->GetDesc( pXRenderToEnvMap, pRTEDesc);
		hrVar = pXRenderToEnvMap->lpVtbl->BeginCube( pXRenderToEnvMap, pCubeTexture);
		hrVar = pXRenderToEnvMap->lpVtbl->BeginSphere( pXRenderToEnvMap, pTexture);
		hrVar = pXRenderToEnvMap->lpVtbl->BeginHemisphere( pXRenderToEnvMap, pTexture, pTexture);
		hrVar = pXRenderToEnvMap->lpVtbl->BeginParabolic( pXRenderToEnvMap, pTexture, pTexture);
		hrVar = pXRenderToEnvMap->lpVtbl->Face( pXRenderToEnvMap, CubeMapFaces);
		hrVar = pXRenderToEnvMap->lpVtbl->End(pXRenderToEnvMap);


		/* d3dx8effect.h */

		/* ID3DXTechnique */
		hrVar = pXTechnique->lpVtbl->QueryInterface( (IDirectXFileBinary *)pXTechnique, iid, ppv);
		hrVar = pXTechnique->lpVtbl->AddRef( (IDirectXFileBinary *)pXTechnique);
		hrVar = pXTechnique->lpVtbl->Release( (IDirectXFileBinary *)pXTechnique);
		hrVar = pXTechnique->lpVtbl->GetDevice( (IDirectXFileBinary *)pXTechnique, ppDevice);
		hrVar = pXTechnique->lpVtbl->GetDesc( (IDirectXFileBinary *)pXTechnique, pTechniqueDesc);
		hrVar = pXTechnique->lpVtbl->GetPassDesc( (IDirectXFileBinary *)pXTechnique, ui, pPassDesc);
		hrVar = pXTechnique->lpVtbl->IsParameterUsed( (IDirectXFileBinary *)pXTechnique, dw);
		hrVar = pXTechnique->lpVtbl->Validate( (IDirectXFileBinary *)pXTechnique);
		hrVar = pXTechnique->lpVtbl->Begin( (IDirectXFileBinary *)pXTechnique, pui);
		hrVar = pXTechnique->lpVtbl->Pass( (IDirectXFileBinary *)pXTechnique, ui);
		hrVar = pXTechnique->lpVtbl->End( (IDirectXFileBinary *)pXTechnique);

		/* ID3DXEffect */
		hrVar = pXEffect->lpVtbl->QueryInterface( (IDirectXFileBinary *)pXEffect, iid, ppv);
		hrVar = pXEffect->lpVtbl->AddRef( (IDirectXFileBinary *)pXEffect);
		hrVar = pXEffect->lpVtbl->Release( (IDirectXFileBinary *)pXEffect);
		hrVar = pXEffect->lpVtbl->GetDevice( (IDirectXFileBinary *)pXEffect, ppDevice);
		hrVar = pXEffect->lpVtbl->GetDesc( (IDirectXFileBinary *)pXEffect, pEffectDesc);
		hrVar = pXEffect->lpVtbl->GetParameterDesc( (IDirectXFileBinary *)pXEffect, ui, pParameterDesc);
		hrVar = pXEffect->lpVtbl->GetTechniqueDesc( (IDirectXFileBinary *)pXEffect, ui, pTechniqueDesc);
		hrVar = pXEffect->lpVtbl->SetDword( (IDirectXFileBinary *)pXEffect, dw, dw);
		hrVar = pXEffect->lpVtbl->GetDword( (IDirectXFileBinary *)pXEffect, dw, pdw);
		hrVar = pXEffect->lpVtbl->SetFloat( (IDirectXFileBinary *)pXEffect, dw, f);
		hrVar = pXEffect->lpVtbl->GetFloat( (IDirectXFileBinary *)pXEffect, dw, pf);
		hrVar = pXEffect->lpVtbl->SetVector( (IDirectXFileBinary *)pXEffect, dw, pV4);
		hrVar = pXEffect->lpVtbl->GetVector( (IDirectXFileBinary *)pXEffect, dw, pV4);
		hrVar = pXEffect->lpVtbl->SetMatrix( (IDirectXFileBinary *)pXEffect, dw, pM);
		hrVar = pXEffect->lpVtbl->GetMatrix( (IDirectXFileBinary *)pXEffect, dw, pM);
		hrVar = pXEffect->lpVtbl->SetTexture( (IDirectXFileBinary *)pXEffect, dw, pBaseTexture);
		hrVar = pXEffect->lpVtbl->GetTexture( (IDirectXFileBinary *)pXEffect, dw, ppBaseTexture);
		hrVar = pXEffect->lpVtbl->SetVertexShader( (IDirectXFileBinary *)pXEffect, dw, dw);
		hrVar = pXEffect->lpVtbl->GetVertexShader( (IDirectXFileBinary *)pXEffect, dw, pdw);
		hrVar = pXEffect->lpVtbl->SetPixelShader( (IDirectXFileBinary *)pXEffect, dw, dw);
		hrVar = pXEffect->lpVtbl->GetPixelShader( (IDirectXFileBinary *)pXEffect, dw, pdw);
		hrVar = pXEffect->lpVtbl->GetTechnique( (IDirectXFileBinary *)pXEffect, ui,  ppTechnique);
		hrVar = pXEffect->lpVtbl->CloneEffect( (IDirectXFileBinary *)pXEffect, pDevice, dw, ppEffect);


		/* d3dx8math.h */

		/* ID3DXMatrixStack */
		hrVar = pXMatrixStack->lpVtbl->QueryInterface( pXMatrixStack, iid, ppv);
		hrVar = pXMatrixStack->lpVtbl->AddRef(pXMatrixStack);
		hrVar = pXMatrixStack->lpVtbl->Release(pXMatrixStack);
		hrVar = pXMatrixStack->lpVtbl->Pop(pXMatrixStack);
		hrVar = pXMatrixStack->lpVtbl->Push(pXMatrixStack);
		hrVar = pXMatrixStack->lpVtbl->LoadIdentity(pXMatrixStack);
		hrVar = pXMatrixStack->lpVtbl->LoadMatrix( pXMatrixStack, pM );
		hrVar = pXMatrixStack->lpVtbl->MultMatrix( pXMatrixStack, pM );
		hrVar = pXMatrixStack->lpVtbl->MultMatrixLocal( pXMatrixStack, pM );
		hrVar = pXMatrixStack->lpVtbl->RotateAxis( pXMatrixStack, pV3, f);
		hrVar = pXMatrixStack->lpVtbl->RotateAxisLocal( pXMatrixStack, pV3, f);
		hrVar = pXMatrixStack->lpVtbl->RotateYawPitchRoll( pXMatrixStack, f, f, f);
		hrVar = pXMatrixStack->lpVtbl->RotateYawPitchRollLocal( pXMatrixStack, f, f, f);
		hrVar = pXMatrixStack->lpVtbl->Scale( pXMatrixStack, f, f, f);
		hrVar = pXMatrixStack->lpVtbl->ScaleLocal( pXMatrixStack, f, f, f);
		hrVar = pXMatrixStack->lpVtbl->Translate( pXMatrixStack, f, f, f );
		hrVar = pXMatrixStack->lpVtbl->TranslateLocal( pXMatrixStack, f, f, f);
		pMVar = pXMatrixStack->lpVtbl->GetTop(pXMatrixStack);


		/* d3dx8mesh.h */

		/* ID3DXBaseMesh */
		hrVar = pXBaseMesh->lpVtbl->QueryInterface( (IDirectXFileBinary *)pXBaseMesh, iid, ppv);
		hrVar = pXBaseMesh->lpVtbl->AddRef( (IDirectXFileBinary *)pXBaseMesh);
		hrVar = pXBaseMesh->lpVtbl->Release( (IDirectXFileBinary *)pXBaseMesh);
		hrVar = pXBaseMesh->lpVtbl->DrawSubset( (IDirectXFileBinary *)pXBaseMesh, dw);
		hrVar = pXBaseMesh->lpVtbl->GetNumFaces( (IDirectXFileBinary *)pXBaseMesh);
		hrVar = pXBaseMesh->lpVtbl->GetNumVertices( (IDirectXFileBinary *)pXBaseMesh);
		hrVar = pXBaseMesh->lpVtbl->GetFVF( (IDirectXFileBinary *)pXBaseMesh);
		hrVar = pXBaseMesh->lpVtbl->GetDeclaration( (IDirectXFileBinary *)pXBaseMesh, adw);
		hrVar = pXBaseMesh->lpVtbl->GetOptions( (IDirectXFileBinary *)pXBaseMesh);
		hrVar = pXBaseMesh->lpVtbl->GetDevice( (IDirectXFileBinary *)pXBaseMesh, ppDevice);
		hrVar = pXBaseMesh->lpVtbl->CloneMeshFVF( (IDirectXFileBinary *)pXBaseMesh, dw, dw, pDevice, ppMesh);
		hrVar = pXBaseMesh->lpVtbl->CloneMesh( (IDirectXFileBinary *)pXBaseMesh, dw, /*CONST*/ pdw, pDevice, ppMesh);
		hrVar = pXBaseMesh->lpVtbl->GetVertexBuffer( (IDirectXFileBinary *)pXBaseMesh, ppVertexBuffer);
		hrVar = pXBaseMesh->lpVtbl->GetIndexBuffer( (IDirectXFileBinary *)pXBaseMesh, ppIndexBuffer);
		hrVar = pXBaseMesh->lpVtbl->LockVertexBuffer( (IDirectXFileBinary *)pXBaseMesh, dw, ppb);
		hrVar = pXBaseMesh->lpVtbl->UnlockVertexBuffer( (IDirectXFileBinary *)pXBaseMesh);
		hrVar = pXBaseMesh->lpVtbl->LockIndexBuffer( (IDirectXFileBinary *)pXBaseMesh, dw, ppb);
		hrVar = pXBaseMesh->lpVtbl->UnlockIndexBuffer( (IDirectXFileBinary *)pXBaseMesh);
		hrVar = pXBaseMesh->lpVtbl->GetAttributeTable( (IDirectXFileBinary *)pXBaseMesh, pAttributeRange, pdw);

		/* ID3DXMesh */
		hrVar = pXMesh->lpVtbl->QueryInterface( (IDirectXFileBinary *)pXMesh, iid, ppv);
		hrVar = pXMesh->lpVtbl->AddRef( (IDirectXFileBinary *)pXMesh);
		hrVar = pXMesh->lpVtbl->Release( (IDirectXFileBinary *)pXMesh);
		hrVar = pXMesh->lpVtbl->DrawSubset( (IDirectXFileBinary *)pXMesh, dw);
		hrVar = pXMesh->lpVtbl->GetNumFaces( (IDirectXFileBinary *)pXMesh);
		hrVar = pXMesh->lpVtbl->GetNumVertices( (IDirectXFileBinary *)pXMesh);
		hrVar = pXMesh->lpVtbl->GetFVF( (IDirectXFileBinary *)pXMesh);
		hrVar = pXMesh->lpVtbl->GetDeclaration( (IDirectXFileBinary *)pXMesh, adw);
		hrVar = pXMesh->lpVtbl->GetOptions( (IDirectXFileBinary *)pXMesh);
		hrVar = pXMesh->lpVtbl->GetDevice( (IDirectXFileBinary *)pXMesh, ppDevice);
		hrVar = pXMesh->lpVtbl->CloneMeshFVF( (IDirectXFileBinary *)pXMesh, dw, dw, pDevice, ppMesh);
		hrVar = pXMesh->lpVtbl->CloneMesh( (IDirectXFileBinary *)pXMesh, dw, /*CONST*/ pdw, pDevice, ppMesh);
		hrVar = pXMesh->lpVtbl->GetVertexBuffer( (IDirectXFileBinary *)pXMesh, ppVertexBuffer);
		hrVar = pXMesh->lpVtbl->GetIndexBuffer( (IDirectXFileBinary *)pXMesh, ppIndexBuffer);
		hrVar = pXMesh->lpVtbl->LockVertexBuffer( (IDirectXFileBinary *)pXMesh, dw, ppb);
		hrVar = pXMesh->lpVtbl->UnlockVertexBuffer( (IDirectXFileBinary *)pXMesh);
		hrVar = pXMesh->lpVtbl->LockIndexBuffer( (IDirectXFileBinary *)pXMesh, dw, ppb);
		hrVar = pXMesh->lpVtbl->UnlockIndexBuffer( (IDirectXFileBinary *)pXMesh);
		hrVar = pXMesh->lpVtbl->GetAttributeTable( (IDirectXFileBinary *)pXMesh, pAttributeRange, pdw);
		hrVar = pXMesh->lpVtbl->LockAttributeBuffer( (IDirectXFileBinary *)pXMesh, dw, ppdw);
		hrVar = pXMesh->lpVtbl->UnlockAttributeBuffer( (IDirectXFileBinary *)pXMesh);
		hrVar = pXMesh->lpVtbl->ConvertPointRepsToAdjacency( (IDirectXFileBinary *)pXMesh, /*CONST*/ pdw, pdw);
		hrVar = pXMesh->lpVtbl->ConvertAdjacencyToPointReps( (IDirectXFileBinary *)pXMesh, /*CONST*/ pdw, pdw);
		hrVar = pXMesh->lpVtbl->GenerateAdjacency( (IDirectXFileBinary *)pXMesh, f, pdw);
		hrVar = pXMesh->lpVtbl->Optimize( (IDirectXFileBinary *)pXMesh, dw, /*CONST*/ pdw, pdw, pdw, ppXBuffer, ppMesh);
		hrVar = pXMesh->lpVtbl->OptimizeInplace( (IDirectXFileBinary *)pXMesh, dw, /*CONST*/ pdw, pdw, pdw, ppXBuffer);

		/* ID3DXPMesh */
		hrVar = pXPMesh->lpVtbl->QueryInterface( (IDirectXFileBinary *)pXPMesh, iid, ppv);
		hrVar = pXPMesh->lpVtbl->AddRef((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->Release((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->DrawSubset( (IDirectXFileBinary *)pXPMesh, dw);
		hrVar = pXPMesh->lpVtbl->GetNumFaces((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetNumVertices((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetFVF((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetDeclaration( (IDirectXFileBinary *)pXPMesh, adw);
		hrVar = pXPMesh->lpVtbl->GetOptions((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetDevice( (IDirectXFileBinary *)pXPMesh, ppDevice);
		hrVar = pXPMesh->lpVtbl->CloneMeshFVF( (IDirectXFileBinary *)pXPMesh, dw, dw, pDevice, ppMesh);
		hrVar = pXPMesh->lpVtbl->CloneMesh( (IDirectXFileBinary *)pXPMesh, dw, /*CONST*/ pdw, pDevice, ppMesh);
		hrVar = pXPMesh->lpVtbl->GetVertexBuffer( (IDirectXFileBinary *)pXPMesh, ppVertexBuffer);
		hrVar = pXPMesh->lpVtbl->GetIndexBuffer( (IDirectXFileBinary *)pXPMesh, ppIndexBuffer);
		hrVar = pXPMesh->lpVtbl->LockVertexBuffer( (IDirectXFileBinary *)pXPMesh, dw, ppb);
		hrVar = pXPMesh->lpVtbl->UnlockVertexBuffer((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->LockIndexBuffer( (IDirectXFileBinary *)pXPMesh, dw, ppb);
		hrVar = pXPMesh->lpVtbl->UnlockIndexBuffer((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetAttributeTable( (IDirectXFileBinary *)pXPMesh, pAttributeRange, pdw);
		hrVar = pXPMesh->lpVtbl->ClonePMeshFVF( (IDirectXFileBinary *)pXPMesh, dw, dw, pDevice, ppPMesh);
		hrVar = pXPMesh->lpVtbl->ClonePMesh( (IDirectXFileBinary *)pXPMesh, dw, /*CONST*/ pdw, pDevice, ppPMesh);
		hrVar = pXPMesh->lpVtbl->SetNumFaces( (IDirectXFileBinary *)pXPMesh, dw);
		hrVar = pXPMesh->lpVtbl->SetNumVertices( (IDirectXFileBinary *)pXPMesh, dw);
		hrVar = pXPMesh->lpVtbl->GetMaxFaces((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetMinFaces((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetMaxVertices((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetMinVertices((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->Save( (IDirectXFileBinary *)pXPMesh, pIStream, pMaterials, dw);
		hrVar = pXPMesh->lpVtbl->Optimize( (IDirectXFileBinary *)pXPMesh, dw, pdw, pdw, ppXBuffer, ppMesh);
		hrVar = pXPMesh->lpVtbl->GetAdjacency( (IDirectXFileBinary *)pXPMesh, pdw);
		hrVar = pXPMesh->lpVtbl->QueryInterface( (IDirectXFileBinary *)pXPMesh, iid, ppv);
		hrVar = pXPMesh->lpVtbl->AddRef((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->Release((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetNumFaces((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetNumVertices((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetFVF((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetDeclaration( (IDirectXFileBinary *)pXPMesh, adw);
		hrVar = pXPMesh->lpVtbl->GetOptions((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetDevice( (IDirectXFileBinary *)pXPMesh, ppDevice);
		hrVar = pXPMesh->lpVtbl->CloneMeshFVF( (IDirectXFileBinary *)pXPMesh, dw, dw, pDevice, ppMesh);
		hrVar = pXPMesh->lpVtbl->CloneMesh( (IDirectXFileBinary *)pXPMesh, dw, /*CONST*/ pdw, pDevice, ppMesh);
		hrVar = pXPMesh->lpVtbl->ClonePMeshFVF( (IDirectXFileBinary *)pXPMesh, dw, dw, pDevice, ppPMesh);
		hrVar = pXPMesh->lpVtbl->ClonePMesh( (IDirectXFileBinary *)pXPMesh, dw, /*CONST*/ pdw, pDevice, ppPMesh);
#pragma MESSAGE( "C API bug - Resolved as Won't Fix by andrewgo in 01.00.3001.01 on 06/25/2001 04:50PM" )
//		hrVar = pXPMesh->lpVtbl->ReduceFaces( (IDirectXFileBinary *)pXPMesh, dw);
//		hrVar = pXPMesh->lpVtbl->ReduceVertices( (IDirectXFileBinary *)pXPMesh, dw);

		hrVar = pXPMesh->lpVtbl->GetMaxFaces((IDirectXFileBinary *)pXPMesh);
		hrVar = pXPMesh->lpVtbl->GetMaxVertices((IDirectXFileBinary *)pXPMesh);

		/* ID3DXSkinMesh */
		hrVar = pXSkinMesh->lpVtbl->QueryInterface( (IDirectXFileBinary *)pXSkinMesh, iid, ppv);
		hrVar = pXSkinMesh->lpVtbl->AddRef((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->Release((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->GetNumFaces((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->GetNumVertices((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->GetFVF((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->GetDeclaration( (IDirectXFileBinary *)pXSkinMesh, adw);
		hrVar = pXSkinMesh->lpVtbl->GetOptions((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->GetDevice( (IDirectXFileBinary *)pXSkinMesh, ppDevice);
		hrVar = pXSkinMesh->lpVtbl->GetVertexBuffer( (IDirectXFileBinary *)pXSkinMesh, ppVertexBuffer);
		hrVar = pXSkinMesh->lpVtbl->GetIndexBuffer( (IDirectXFileBinary *)pXSkinMesh, ppIndexBuffer);
		hrVar = pXSkinMesh->lpVtbl->LockVertexBuffer( (IDirectXFileBinary *)pXSkinMesh, dw, ppb);
		hrVar = pXSkinMesh->lpVtbl->UnlockVertexBuffer((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->LockIndexBuffer( (IDirectXFileBinary *)pXSkinMesh, dw, ppb);
		hrVar = pXSkinMesh->lpVtbl->UnlockIndexBuffer((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->LockAttributeBuffer( (IDirectXFileBinary *)pXSkinMesh, dw, ppdw);
		hrVar = pXSkinMesh->lpVtbl->UnlockAttributeBuffer((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->GetNumBones((IDirectXFileBinary *)pXSkinMesh);
		hrVar = pXSkinMesh->lpVtbl->GetOriginalMesh( (IDirectXFileBinary *)pXSkinMesh, ppMesh);
		hrVar = pXSkinMesh->lpVtbl->SetBoneInfluence( (IDirectXFileBinary *)pXSkinMesh, dw, dw, /*CONST*/ pdw, /*CONST*/ pf);
		hrVar = pXSkinMesh->lpVtbl->GetNumBoneInfluences( (IDirectXFileBinary *)pXSkinMesh, dw);
		hrVar = pXSkinMesh->lpVtbl->GetBoneInfluence( (IDirectXFileBinary *)pXSkinMesh, dw, pdw, pf);
		hrVar = pXSkinMesh->lpVtbl->GetMaxVertexInfluences( (IDirectXFileBinary *)pXSkinMesh, pdw);
		hrVar = pXSkinMesh->lpVtbl->GetMaxFaceInfluences( (IDirectXFileBinary *)pXSkinMesh, pdw);
		hrVar = pXSkinMesh->lpVtbl->ConvertToBlendedMesh( (IDirectXFileBinary *)pXSkinMesh, dw, /*CONST*/ pdw, pdw, pdw, ppXBuffer, ppMesh);
		hrVar = pXSkinMesh->lpVtbl->ConvertToIndexedBlendedMesh( (IDirectXFileBinary *)pXSkinMesh, dw, pdw, dw, pdw, pdw, ppXBuffer, ppMesh);
		hrVar = pXSkinMesh->lpVtbl->GenerateSkinnedMesh( (IDirectXFileBinary *)pXSkinMesh, dw, f, /*CONST*/ pdw, pdw, ppMesh);
		hrVar = pXSkinMesh->lpVtbl->UpdateSkinnedMesh( (IDirectXFileBinary *)pXSkinMesh, /*CONST*/ pM, pMesh);

		//------------------------------------
		// end d3dx8.h
		//------------------------------------

	}

	return hr;
}
