// File created by FindTrace
// Last modified: Wed Jan 03 13:49:51 2001
 

#ifndef STATDEF_H
#define STATDEF_H

typedef struct LENHASH
{
	union
	{
		DWORD offset;
		char *strPtr;
	};
	union
	{
		struct
		{
			WORD numEntries;
			WORD startID;
		};
		DWORD strLen;
	};
} LENHASH;

#define STAT_NUM_FUNCTIONS		382
#define STAT_HASH_TABLE_SIZE	122

#ifdef STAT_INSTANIATE_TABLE
LENHASH statDefHash[STAT_HASH_TABLE_SIZE] = {
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0, {2, 0} },
	{14, {3, 2} },
	{38, {1, 5} },
	{47, {2, 6} },
	{67, {4, 8} },
	{111, {9, 12} },
	{219, {7, 21} },
	{310, {17, 28} },
	{548, {20, 45} },
	{848, {24, 65} },
	{1232, {25, 89} },
	{1657, {22, 114} },
	{2053, {25, 136} },
	{2528, {22, 161} },
	{2968, {23, 183} },
	{3451, {22, 206} },
	{3935, {22, 228} },
	{4441, {19, 250} },
	{4897, {19, 269} },
	{5372, {18, 288} },
	{5840, {9, 306} },
	{6083, {14, 315} },
	{6475, {4, 329} },
	{6591, {7, 333} },
	{6801, {8, 340} },
	{7049, {5, 348} },
	{7209, {5, 353} },
	{7374, {2, 358} },
	{7442, {2, 360} },
	{7512, {1, 362} },
	{7548, {2, 363} },
	{7622, {3, 365} },
	{7736, {1, 368} },
	{7775, {1, 369} },
	{7815, {1, 370} },
	{7856, {1, 371} },
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{7898, {1, 372} },
	{0xffffffff, {0, 0}},
	{7944, {1, 373} },
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{7992, {1, 374} },
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{8044, {1, 375} },
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{8114, {1, 376} },
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{8187, {1, 377} },
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{8267, {1, 378} },
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{8355, {1, 379} },
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{8447, {1, 380} },
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{0xffffffff, {0, 0}},
	{8549, {1, 381} }
};

char statDefList[] = 
		"Blit32\0"
		"Flip32\0"
		"DllMain\0"
		"__Reset\0"
		"nvClear\0"
		"nvClear2\0"
		"SetMode32\0"
		"bltStrBlt\0"
		"DriverInit\0"
		"bltTexture\0"
		"nvDP2Clear\0"
		"nvEnable32\0"
		"FreeAgpHeap\0"
		"MyExtEscape\0"
		"SetDCHandle\0"
		"WaitForIdle\0"
		"blt4CCto4CC\0"
		"nvDP2Points\0"
		"nvDP2TexBlt\0"
		"nvDisable32\0"
		"nvEnableD3D\0"
		"AllocAGPHeap\0"
		"GetModePitch\0"
		"SetEntries32\0"
		"SetPalette32\0"
		"getRootIndex\0"
		"nvDisableD3D\0"
		"nvPaletteBlt\0"
		"ColourControl\0"
		"FirstTimeInit\0"
		"GetScanLine32\0"
		"blt8bppStrBlt\0"
		"bltColourFill\0"
		"bltSimpleCopy\0"
		"bltUpdateClip\0"
		"nvD3DReenable\0"
		"nvDP2LineList\0"
		"nvDP2SetLight\0"
		"nvDP2StateSet\0"
		"nvEnableHeaps\0"
		"nvFreeContext\0"
		"nvRenderState\0"
		"nvSetDX6State\0"
		"nvTextureLock\0"
		"nvUnpalettize\0"
		"AddModeToTable\0"
		"GetBltStatus32\0"
		"GetFlatCodeSel\0"
		"GetFlatDataSel\0"
		"bltAllocAGPMem\0"
		"bltAllocSysMem\0"
		"bltAllocVidMem\0"
		"bltSetBltState\0"
		"bltSlowStoVBlt\0"
		"bltSlowVtoSBlt\0"
		"cacheFogValues\0"
		"nvDP2LineStrip\0"
		"nvDP2SetTexLOD\0"
		"nvDP2SetWRange\0"
		"nvDP2SetZRange\0"
		"nvDP2StreamEnd\0"
		"nvHWSwizzleBlt\0"
		"nvResetContext\0"
		"nvSceneCapture\0"
		"nvSetupContext\0"
		"AboutToBeLoaded\0"
		"CTexture::evict\0"
		"CTexture::fetch\0"
		"CreatePalette32\0"
		"CreateSurface32\0"
		"D3DReadRegistry\0"
		"DestroyDriver32\0"
		"FreeTextureHeap\0"
		"GetDriverInfo32\0"
		"ScaleZFillDepth\0"
		"getAdapterIndex\0"
		"nvContextCreate\0"
		"nvCreateZBuffer\0"
		"nvDP2BufferBlit\0"
		"nvDP2SetIndices\0"
		"nvDP2SetPalette\0"
		"nvDP2VolumeBlit\0"
		"nvDrawPointList\0"
		"nvEnableOverlay\0"
		"nvInitRefCounts\0"
		"nvTexManageInit\0"
		"nvTextureBltDX7\0"
		"nvTextureCreate\0"
		"nvTextureUnlock\0"
		"AcquireUserMutex\0"
		"AllocTextureHeap\0"
		"CTexture::hwLock\0"
		"DestroyPalette32\0"
		"DestroySurface32\0"
		"DrawPrimitives32\0"
		"ReleaseUserMutex\0"
		"_CTexture_rename\0"
		"bltSetSurfaces2D\0"
		"bltSimpleVtoVBlt\0"
		"buildDDHALInfo32\0"
		"nvContextDestroy\0"
		"nvDP2CreateLight\0"
		"nvDP2LineListImm\0"
		"nvDP2SetMaterial\0"
		"nvDP2SetPriority\0"
		"nvDP2SetViewport\0"
		"nvDP2TriangleFan\0"
		"nvDestroyDDLocal\0"
		"nvDrawPrimitives\0"
		"nvFindObjectList\0"
		"nvInitD3DObjects\0"
		"nvTexManageEvict\0"
		"nvTexManageFetch\0"
		"nvTexManageTouch\0"
		"AboutToBeUnloaded\0"
		"CreateDmaContexts\0"
		"CreateTextureHeap\0"
		"GetK32ProcAddress\0"
		"SortDDHalModeList\0"
		"bltAllocWorkSpace\0"
		"bltControlTraffic\0"
		"getDXShareForHead\0"
		"nvAddObjectToList\0"
		"nvCelsiusAACreate\0"
		"nvCelsiusAAMinify\0"
		"nvCreateSurfaceEx\0"
		"nvDP2SetClipPlane\0"
		"nvDP2SetTransform\0"
		"nvDP2TriangleList\0"
		"nvEnableNotifiers\0"
		"nvFindGenericList\0"
		"nvSetContextState\0"
		"nvSetHardwareCaps\0"
		"nvSetRenderTarget\0"
		"nvSwizBlt_lin2swz\0"
		"updateMipMapChain\0"
		"CDefaultVB::create\0"
		"CanCreateSurface32\0"
		"DestroyDmaContexts\0"
		"DestroyTextureHeap\0"
		"DrawOnePrimitive32\0"
		"GetHeapAlignment32\0"
		"bltEarlyErrorCheck\0"
		"getDisplayDuration\0"
		"nvCelsiusAADestroy\0"
		"nvCelsiusAAMagnify\0"
		"nvCelsiusAAMinifyZ\0"
		"nvCreateObjectList\0"
		"nvDP2DrawPrimitive\0"
		"nvDP2TriangleStrip\0"
		"nvDP2UpdatePalette\0"
		"nvDeleteObjectList\0"
		"nvDrawOnePrimitive\0"
		"nvEnableSemaphores\0"
		"nvExpandObjectList\0"
		"nvFVFDrawPointList\0"
		"nvFindSurfaceChain\0"
		"nvInitDDrawObjects\0"
		"nvSetHardwareState\0"
		"nvTextureColorFill\0"
		"nvTranslateLODBias\0"
		"BuildDDHalModeTable\0"
		"CDefaultVB::destroy\0"
		"GetOffsetTranslated\0"
		"GetTranslatedOffset\0"
		"SafeSyncSurfaceData\0"
		"_CTexture_canRename\0"
		"bltCreateContextDMA\0"
		"nvCalculateColorKey\0"
		"nvCelsiusAAMagnifyZ\0"
		"nvContextDestroyAll\0"
		"nvCreateGenericList\0"
		"nvDP2DrawPrimitive2\0"
		"nvDP2DrawTriSurface\0"
		"nvDP2RecordSetLight\0"
		"nvDP2SetPixelShader\0"
		"nvDP2SetRenderState\0"
		"nvDP2TriangleFanImm\0"
		"nvDeleteGenericList\0"
		"nvDrawPointTriangle\0"
		"nvExpandGenericList\0"
		"nvTexManageResetPtr\0"
		"nvTexManageSceneEnd\0"
		"UpdateNonLocalHeap32\0"
		"__DDHAL32_VidMemFree\0"
		"nvCelsiusAAScaledBlt\0"
		"nvDP2CaptureSetLight\0"
		"nvDP2DrawRectSurface\0"
		"nvDP2IndexedLineList\0"
		"nvDP2RecordExtension\0"
		"nvDP2RecordSetWRange\0"
		"nvDP2RecordSetZRange\0"
		"nvDP2SetRenderTarget\0"
		"nvDP2SetStreamSource\0"
		"nvDP2SetVertexShader\0"
		"nvRefreshSurfaceInfo\0"
		"nvSetD3DSurfaceState\0"
		"nvSetDX6TextureState\0"
		"nvSetSurfaceViewport\0"
		"nvStencilBufferInUse\0"
		"nvSwizzleBlt_Lin_Lin\0"
		"nvSwizzleBlt_Lin_Swz\0"
		"nvSwizzleBlt_Swz_Lin\0"
		"nvSwizzleBlt_Swz_Swz\0"
		"nvTextureDefaultFree\0"
		"nvTextureStretchBlit\0"
		"GetPointerTextureHeap\0"
		"SafeSyncVideoPortData\0"
		"__DDHAL32_VidMemAlloc\0"
		"nvAllocateContextDMAs\0"
		"nvDP2CaptureExtension\0"
		"nvDP2CaptureSetWRange\0"
		"nvDP2CaptureSetZRange\0"
		"nvDP2IndexedLineList2\0"
		"nvDP2IndexedLineStrip\0"
		"nvDP2RecordSetIndices\0"
		"nvFindNextCubemapFace\0"
		"nvGetObjectFromHandle\0"
		"nvSetDx5TriangleState\0"
		"nvStencilBufferExists\0"
		"nvSwizBlt_cpu_lin2lin\0"
		"nvSwizBlt_cpu_lin2swz\0"
		"nvSwizBlt_cpu_swz2lin\0"
		"nvSwizBlt_cpu_swz2swz\0"
		"nvSwizBlt_gpu_lin2swz\0"
		"nvTexManageAddTexture\0"
		"nvTexManageAdvancePtr\0"
		"nvTextureDefaultAlloc\0"
		"CDefaultVB::CDefaultVB\0"
		"CDriverContext::create\0"
		"CSimpleSurface::create\0"
		"CSimpleSurface::isBusy\0"
		"D3DLockExecuteBuffer32\0"
		"GetAvailDriverMemory32\0"
		"WaitForVerticalBlank32\0"
		"invalidateTextureCache\0"
		"nvAllocateDDrawObjects\0"
		"nvCelsiusAAScaledBltHW\0"
		"nvClearObjectListEntry\0"
		"nvDP2CaptureSetIndices\0"
		"nvDP2CreatePixelShader\0"
		"nvDP2DeletePixelShader\0"
		"nvDP2MultiplyTransform\0"
		"nvDP2RecordSetMaterial\0"
		"nvDP2RecordSetViewport\0"
		"nvDP2SetStreamSourceUM\0"
		"nvFVFDrawPointTriangle\0"
		"nvIndexedPointTriangle\0"
		"nvTexManageAdvanceList\0"
		"nvTextureCreateSurface\0"
		"CDefaultVB::~CDefaultVB\0"
		"CDriverContext::destroy\0"
		"CSimpleSurface::cpuLock\0"
		"Lock32: fpvidmem = %08x\0"
		"nvAddSurfaceChainToList\0"
		"nvAllocDriverStructures\0"
		"nvCalculateBumpMapState\0"
		"nvClearGenericListEntry\0"
		"nvCreateContextListHeap\0"
		"nvDP2CaptureSetMaterial\0"
		"nvDP2CaptureSetViewport\0"
		"nvDP2ClippedTriangleFan\0"
		"nvDP2CreateVertexShader\0"
		"nvDP2DeleteVertexShader\0"
		"nvDP2IndexedTriangleFan\0"
		"nvDP2RecordSetClipPlane\0"
		"nvDP2RecordSetTransform\0"
		"nvSetD3DSurfaceViewport\0"
		"nvTextureDestroySurface\0"
		"CComplexSurface::cpuLock\0"
		"CSimpleSurface::hwUnlock\0"
		"CTexture::calcMipMapSize\0"
		"CTexture::enableRenaming\0"
		"CTexture::internalRename\0"
		"D3DCreateExecuteBuffer32\0"
		"D3DCreateTextureContexts\0"
		"D3DUnlockExecuteBuffer32\0"
		"bltDestroyAllContextDMAs\0"
		"bltPotentialFaultHandler\0"
		"nvCalculateSurfaceParams\0"
		"nvCalculateZBufferParams\0"
		"nvDP2CaptureSetClipPlane\0"
		"nvDP2CaptureSetTransform\0"
		"nvDP2IndexedTriangleList\0"
		"nvDP2SetPixelShaderConst\0"
		"nvDestroyContextListHeap\0"
		"nvDrawPrimitives2 (%08x)\0"
		"nvTexManageRemoveTexture\0"
		"CComplexSurface::disown()\0"
		"CTexture[%08x]::destroy()\0"
		"D3DDestroyExecuteBuffer32\0"
		"D3DDestroyTextureContexts\0"
		"DrawOneIndexedPrimitive32\0"
		"Unlock32: fpvidmem = %08x\0"
		"nvCelsiusAAMinifyInternal\0"
		"nvCreateSurfaceWithFormat\0"
		"nvDP2DrawIndexedPrimitive\0"
		"nvDP2IndexedTriangleList2\0"
		"nvDP2IndexedTriangleStrip\0"
		"nvDP2RecordSetPixelShader\0"
		"nvDP2RecordSetRenderState\0"
		"nvDP2SetTextureStageState\0"
		"nvDP2SetVertexShaderConst\0"
		"nvDrawOneIndexedPrimitive\0"
		"nvFVFIndexedPointTriangle\0"
		"nvSetDx6MultiTextureState\0"
		"BuildDefaultDDHalModeTable\0"
		"CTexture[%08x]::CTexture()\0"
		"nvCelsiusAAMagnifyInternal\0"
		"nvCheckSystemMemorySurface\0"
		"nvDP2CaptureSetPixelShader\0"
		"nvDP2CaptureSetRenderState\0"
		"nvDP2DrawIndexedPrimitive2\0"
		"nvDP2RecordSetStreamSource\0"
		"nvDP2RecordSetVertexShader\0"
		"CNvObject::dbgTestIntegrity\0"
		"CTexture::getCubeMapFaceNum\0"
		"CTexture::internalCanRename\0"
		"CTexture[%08x]::~CTexture()\0"
		"D3DCanCreateExecuteBuffer32\0"
		"nvAutoPaletteCheckAndRevert\0"
		"nvCalculateHardwareStateDX5\0"
		"nvCelsiusSet2Stage1Combiner\0"
		"nvCreateSystemMemorySurface\0"
		"nvDP2CaptureSetStreamSource\0"
		"nvDP2CaptureSetVertexShader\0"
		"nvDeleteSystemMemorySurface\0"
		"nvValidateTextureStageState\0"
		"nvVolumeSwizBlt_cpu_lin2swz\0"
		"nv4ValidateTextureStageState\0"
		"nvCreateSurfaceWithoutFormat\0"
		"nvDP2RecordSetStreamSourceUM\0"
		"nvGetWin16MutexProcAddresses\0"
		"CComplexSurface[%08x]::swap()\0"
		"CSimpleSurface[%08x]::reset()\0"
		"GetSelectorsFromDisplayDriver\0"
		"nvCelsiusAASelectSuperBuffers\0"
		"nvDP2CaptureSetStreamSourceUM\0"
		"nvSetCelsuisBumpMap1Combiners\0"
		"nvSetCelsuisBumpMap2Combiners\0"
		"CSimpleSurface[%08x]::disown()\0"
		"FreeSelectorsFromDisplayDriver\0"
		"nvCalculateDirectHardwareState\0"
		"nvCelsiusAASelectNormalBuffers\0"
		"nvDP2RecordSetPixelShaderConst\0"
		"nvSetMultiTextureHardwareState\0"
		"nvTextureDestroy - handle=%08x\0"
		"nvTextureGetSurf - handle=%08x\0"
		"CSimpleSurface[%08x]::destroy()\0"
		"nvConvertTextureStageToDX5Class\0"
		"nvDP2CaptureSetPixelShaderConst\0"
		"nvDP2RecordSetTextureStageState\0"
		"nvDP2RecordSetVertexShaderConst\0"
		"CComplexSurface::tagRenameEnable\0"
		"CComplexSurface[%08x]::destroy()\0"
		"nvDP2CaptureSetTextureStageState\0"
		"nvDP2CaptureSetVertexShaderConst\0"
		"nvSwizzleBlt_Lin_Swz_FullTexture\0"
		"nvKelvinValidateTextureStageState\0"
		"nvPointIndexedTriangle (%d,%x,%p)\0"
		"nvCelsiusValidateTextureStageState\0"
		"nvTexManageEvictAll: pContext=%08x\0"
		"nvCelsiusSetTextures3StageCombiners\0"
		"nvCreateVidHeapVarsAndPseudoNotifier\0"
		"nvPointNonIndexedTriangle (%d,%x,%p)\0"
		"CTexture[%08x]::updateLinearSurface()\0"
		"nvSetDefaultMultiTextureHardwareState\0"
		"nvWireframeIndexedTriangle (%d,%x,%x)\0"
		"CTexture[%08x]::updateSwizzleSurface()\0"
		"nvSetRenderTargetDX7 (rt = %d, zb = %d)\0"
		"nvWireframeNonIndexedTriangle (%d,%x,%x)\0"
		"CSimpleSurface[%08x]::swap(pSurface=%08x)\0"
		"CTexture[%08x]::prepareLinearAsRenderTarget()\0"
		"nvTextureSwap - hContext=%08x, h1=%08x, h2=%08x\0"
		"CSimpleSurface[%08x]::create(pTemplateSurface=%08x)\0"
		"SetSurfaceColorKey32: lpSetCK->lpDD=%08x, lpSetCK->lpDDSurface = %08x\0"
		"CTexture[%08x]::create(dwThisFace=%d,pBaseTexture=%08x,dwMipMapLevel=%d)\0"
		"CComplexSurface[%08x]::create(dwSize=%08x,dwAllowedHeaps=%x,dwPreferredHeap=%x)\0"
		"CSimpleSurface[%08x]::own(dwAddress=%08x,dwPitch=%08x,dwH=%08x,dwHeapLoc=%x,bOwnMem=%d)\0"
		"CComplexSurface[%08x]::own(dwAddress=%08x,dwPitch=%08x,dwLogH=%08x,dwHeapLoc=%x,bOwnMem=%d)\0"
		"CSimpleSurface[%08x]::create(dwPitch=%08x,dwHt=%08x,dwDp=%08x,dwAllHeaps=%x,dwPrefHeap=%x,dwFlags=%x)\0"
		"CTexture[%08x]::create(dwWidth=%d,dwHeight=%d,dwBPP=%d,dwMipMapCount=%d,dwFormat=%x,dwAllowedHeaps=%x,dwPreferredHeap=%x)\0";
#endif	 //STAT_INSTANIATE_TABLE
#endif	// ifndef <filename>

