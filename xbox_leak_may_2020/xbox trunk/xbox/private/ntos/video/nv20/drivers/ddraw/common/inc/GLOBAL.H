#ifndef _global_h // multiple include protection
#define _global_h

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// All globals go into the 'global' structure. We divie the big structure up into
// banks. Every bank is a multiple of 32 bytes big. We place globals that are used
// together in inner loops in the same bank. This way we minimize pentium cache
// misses
//
// we assume that this structure starts 32-byte aligned. Will be so once all globs
// are placed here - and the C libraries do not mess with us... Look at .map output
// for alignment info
//
#pragma pack(push,1) // we control alignment

#ifdef SW_WC
#define SW_WC_LINES2FILL    48
#define SW_WC_LINESIZE      64
#define SW_WC_BYTES         (SW_WC_LINES2FILL*SW_WC_LINESIZE)
#define SW_WC_SIZE          (SW_WC_BYTES/4)     //# of DWORDS
#endif

typedef struct KATMAI_STATE
{
    // nv4/5
    float fFogC1[4];
    float fFogC2[4];
    float fFogTableDensity[4];
    float fFogTableLinearScale[4];
    float fFogTableStart[4];
    float fFogTableEnd[4];
    float fRHW[4];
    DWORD dwFogValue[4];
    float fZero[4];
    float fOne[4];
    float f255[4];
    DWORD dwMask3[4];
    // supertri
#ifdef SW_WC
    DWORD dwTriDispatch[SW_WC_SIZE];
#else
    DWORD dwTriDispatch[4];
#endif

#if (NVARCH >= 0x020)
#ifdef SW_WC
    DWORD dwKelvinTriDispatch[SW_WC_SIZE];
#else
    DWORD dwKelvinTriDispatch[4];
#endif
#endif
    struct TLVERTEX
    {
        float x,y,z,w;
        DWORD outCode;
        DWORD signOfW;
        DWORD _pad[2];
    } adwTLVertexCache[VB_CACHE_SIZE];
} KATMAI_STATE;

//---------------------------------------------------------------------------

#define SOFT_WC_SIZE    64 // must be power of 2, must be less than 120
typedef struct _GLOBALCELSIUSDATA
{
// bank
    DWORD  pContext;
    DWORD  pContext_hwState;
    DWORD  dwPrimCount;
    DWORD  pIndices;
    DWORD  pVertices;
    DWORD  dwVertexSN;
    DWORD  dwSoftWCIndex;
    DWORD  dwLoopCache;
// bank
    DWORD  dwVBIndex;
    DWORD  dwVBOffset;
    DWORD  dwVBGetOffset;
    DWORD  dwVBThreshold;
    DWORD  dwVerticesBatched;
    DWORD  dwPrimsBatched;
    DWORD  dwPrefetchBase;
    DWORD  dwPrefetchMax;
// bank
    DWORD  dwCullValue;
    DWORD  pIndexBase;          // used by inl prim & legacy
    DWORD  dwVertexStride;
    DWORD  dwIndexStride;
    DWORD  dwPrefMaxIdx;
    DWORD  dwEyeAddr;
    DWORD  dwCullAddr;
    DWORD  dwBaseVertex;
// bank
    float  pafMinInfo[4];
    float  pafMaxInfo[4];
// bank
    unsigned __int64 qwBegin3;
    unsigned __int64 qwEnd3;
    DWORD  _pad_c5[4];
// bank
    DWORD  dwOffsetXYZ;
    DWORD  dwOffsetRHW;
    DWORD  dwOffsetDiffuse;
    DWORD  dwOffsetSpecular;
    DWORD  dwOffsetNormal;
    DWORD  dwOffsetWeight;
    DWORD  _pad_c6[2];
// bank
    DWORD  dwOffsetUV[8];
// bank
    DWORD  dwSoftWCBuffer[SOFT_WC_SIZE];
} GLOBALCELSIUSDATA;

//---------------------------------------------------------------------------

#define KELVIN_MAX_REUSE 4
#define KELVIN_REUSE_MASK (KELVIN_MAX_REUSE-1)

typedef struct _GLOBALKELVINDATA
{
// bank
    DWORD  pContext;
    DWORD  pContext_kelvinState;
    DWORD  dwPrimCount;
    DWORD  pIndices;
    DWORD  pVertices;
    DWORD  dwVertexSN;
    DWORD  dwSoftWCIndex;
    DWORD  dwLoopCache;
// bank
    DWORD  dwVBIndex;
    DWORD  dwVBOffset;
    DWORD  dwVBGetOffset;
    DWORD  dwVBThreshold;
    DWORD  dwVerticesBatched;
    DWORD  dwPrimsBatched;
    DWORD  dwPrefetchBase;
    DWORD  dwPrefetchMax;
// bank
    DWORD  dwCullValue;
    DWORD  pIndexBase;          // used by inl prim & legacy
    DWORD  dwVertexStride;
    DWORD  dwIndexStride;
    DWORD  dwPrefMaxIdx;
    DWORD  dwEyeAddr;
    DWORD  dwCullAddr;
    DWORD  dwBaseVertex;
// bank
    float  pafMinInfo[4];
    float  pafMaxInfo[4];
// bank
    unsigned __int64 qwBegin;
    unsigned __int64 qwEnd;
    DWORD  _pad_c5[4];
// bank
// bank
    DWORD  dwOffsetXYZ;
    DWORD  dwOffsetRHW;
    DWORD  dwOffsetDiffuse;
    DWORD  dwOffsetSpecular;
    DWORD  dwOffsetNormal;
    DWORD  dwOffsetWeight;
    DWORD  dwOffsetPointSize;
    DWORD  _pad_c6[1];
// bank
    DWORD  dwOffsetUV[8];
// bank
    DWORD dwSoftWCBuffer[SOFT_WC_SIZE];
// bank
    DWORD   dwVertexReuse[KELVIN_MAX_REUSE];
// bank
    DWORD   dwCurrentHistoryIndex;
    DWORD   dwHistoryIndex;
    DWORD   __pad_c7[6];

} GLOBALKELVINDATA;

//---------------------------------------------------------------------------

typedef struct {
//////////
// debug and retail banks
// bank
    DWORD nvCachedPut;          // only used by inner loops
    DWORD nvCachedThreshold;    // only used by inner loops
    BYTE* dwILCData;
    DWORD dwILCCount;
    DWORD dwILCMax;
    DWORD dwCullMask1;
    DWORD dwCullMask2;
// bank
    DWORD    dwDelayCount;
    float    fPixelCount;
    double   fdMushLimitClocksPerTri;
    double   fdMushLimitClocksPerPixel;
//bank
    double   fdMushLimitClocksPerFrame;
    __int64  i64MushStart;
    DWORD    dwTriCountTotal;
    DWORD   _dwPad001[3];

// bank
    GLOBALCELSIUSDATA celsius;

// bank
    GLOBALKELVINDATA  kelvin;

    // dx5/dx6
// bank
    DWORD dwPrimCount;          // # of primitives to render
    DWORD pVertices;            // vertex list
    DWORD pIndices;             // index list (NULL for ordered prims)
    DWORD dwPrimBatchCount;     // # of prims per batch
    DWORD dwStripSense;         // strip sense (0 or 4)
    DWORD dwVertexSN;           // current vertex cache serial number
    DWORD dwVertexCount;        // vertex dispatcher vertex count
    float fCTC1;
// bank
    DWORD dwIndex0;             // )
    DWORD dwIndex1;             //  ) current triangle index
    DWORD dwIndex2;             // )
    DWORD dwVertex0;            // )
    DWORD dwVertex1;            //  ) current triangle
    DWORD dwVertex2;            // )
    DWORD dwBlendRegister;      // blend register (with spec enabled)
    DWORD dwSpecularCount;      // timeout for spec hack
// bank
    DWORD dwFlatColor;          // color used for flat shading
    DWORD dwFlatSpecular;       // specular used for flat shading
    float fRWFar;
    DWORD dwTemp;
#ifdef INSTRUMENT_INNER_LOOPS
    DWORD dwBytesCopied;
#else
    DWORD _reserved;
#endif
    DWORD dwMaxVertexOffset;    // used under NT only to validate vertices
    DWORD _pad2_3[1];
    KATMAI_STATE *pKNI;

    // bank 2.4 - note that the following variables start on the next 16-byte boundary and
    //            not neccesseraly exactly where this definition shows
    BYTE  kni[sizeof(KATMAI_STATE) + 16];
    //          - end alignment exception
    // bank 2.5
    DWORD dwVertexCacheIndex[VB_CACHE_SIZE]; // indices of cache hits
    DWORD dwVertexCache[VB_CACHE_SIZE];     // vertex cache
    // bank 2.6
    DWORD dwDXRuntimeVersion;
    BOOL  b16BitCode;                       // the current call stack has propogated from 16-bit code
    DWORD dwVertexCacheDate;
    DWORD dwSpecularState;                  // used to cache whether specular is on or off
                                            // for the specular optimzation in the inner loops (0/1)

    // pointer to GDI's display enumeration function
#ifdef WINNT
    DWORD _pad2_6[4];
#else
    BOOL   (WINAPI* pfEnumDisplayDevices)(LPCSTR, DWORD, PDISPLAY_DEVICE, DWORD);
    HANDLE (WINAPI *pfOpenVxDHandle)(HANDLE);
    DWORD _pad2_6[2];
#endif

    // banks 5-36
    DWORD adwMemCopy[256];

    // bank 37
    CNvObject                  *pNvObjectHead;
    struct _NV_OBJECT_LIST     *pNvSurfaceLists;     // list of surface handle->object lists
    struct _NV_OBJECT_LIST     *pNvPaletteLists;     // list of palette handle->object lists
    struct _NV_OBJECT_LIST     *pNvVShaderLists;     // list of vertex shader handle->object lists
    struct _NV_OBJECT_LIST     *pNvPShaderLists;     // list of pixel shader handle->object lists
    struct _NV_OBJECT_LIST     *pNvPatchLists;       // list of patch handle->object lists

    // bank 38
    double fdClockPerTri;
    double fdClockPerPixel;
    float  fTotalPixelsTimes2;
    DWORD _pad_5[3];

#if (NVARCH >= 0x010)
    PCELSIUSCOMBINERPROGRAM     celsiusCombinerProgramTable[CELSIUSCPTABLE_SIZE];
#endif // NVARCH == 0x010


} global_struc;

extern global_struc global;

//---------------------------------------------------------------------------

__forceinline void nvDelay
(
    void
)
{
    DWORD dw = global.dwDelayCount;
    __asm
    {
    push ecx
        mov ecx,[dw]
xxx:
        loop xxx
    pop ecx
    }
}

//---------------------------------------------------------------------------

// encrypted registry keys

#define NV4D3D_REG_STRING_SIZE_MAX  32

typedef char reg_entry[NV4D3D_REG_STRING_SIZE_MAX];

// keep this structure in sync with its initialization in global.cpp
typedef struct
{
    DWORD dwID[8];
    DWORD dwSize;
    /*
     * Encrypted registry key strings.
     */
    // booleans
    reg_entry szAntiAliasEnableString;             // ANTIALIASENABLE
    reg_entry szAntiAliasForceEnableString;        // ANTIALIASFORCEENABLE
    reg_entry szAntiAliasDynamicEnableString;      // ANTIALIASDYNAMICENABLE
    reg_entry szAntiAliasConvolutionFastModeString;// ANTIALIASCONVOLUTIONFASTMODE
    reg_entry szCKCompatabilityEnableString;       // CKCOMPATABILITYENABLE
    reg_entry szDirectMapEnableString;             // DIRECTMAPENABLE
    reg_entry szEnforceStrictTrilinearString;      // ENFORCESTRICTTRILINEAR
    reg_entry szFlushAfterBlitEnableString;        // FLUSHAFTERBLITENABLE
    reg_entry szFogTableString;                    // FOGTABLEENABLE
    reg_entry szForceBlitWaitFlagEnableString;     // FORCEBLITWAITFLAGENABLE
    reg_entry szLimitQueuedFBBlitsEnableString;    // LIMITQUEUEDFBBLITSENABLE
    reg_entry szLogoEnableString;                  // LOGOENABLE
#ifndef TEXFORMAT_CRD
    reg_entry szPaletteEnableString;               // PALETTEENABLE
#endif  // !TEXFORMAT_CRD
    reg_entry szSquashWString;                     // SQUASHW
    reg_entry szSceneSyncString;                   // SCENESYNC
#ifndef TEXFORMAT_CRD
    reg_entry szTextureCompressionEnableString;    // TEXTURECOMPRESSIONENABLE
#endif  // !TEXFORMAT_CRD
    reg_entry szTextureManagementEnableString;     // TEXTUREMANAGEMENTENABLE
    reg_entry szTilingString;                      // TILINGENABLE
    reg_entry szZCullEnableString;                 // ZCULLENABLE
    reg_entry szUserMipMapsString;                 // USERMIPMAPS
    reg_entry szVideoTextureString;                // VIDEOTEXTURE
    reg_entry szVS_HOS_EmulationString;            // VS_HOS_EMULATION
    reg_entry szWEnableString;                     // WENABLE
    reg_entry szZ24EnableString;                   // Z24ENABLE
    reg_entry szZCompressEnableString;             // ZCOMPRESSENABLE
    DWORD dwSyncPoint0;
    // non-boolean enumerated types
    reg_entry szAntiAliasQualityString;            // ANTIALIASQUALITY
    reg_entry szAntiAliasDynamicFPS;               // ANTIALIASDYNAMICFPS
    reg_entry szCaptureConfigString;               // CAPTURECONFIG
    reg_entry szCaptureEnableString;               // CAPTUREENABLE
    reg_entry szCKRefString;                       // CKREF
    reg_entry szAnisotropicLevelString;            // ANISOTROPICLEVEL
    reg_entry szMipMapDitherEnableString;          // MIPMAPDITHERENABLE
#ifdef TEXFORMAT_CRD
    reg_entry szSurfaceFormatsDX7String;           // SURFACEFORMATSDX7
    reg_entry szSurfaceFormatsDX8String;           // SURFACEFORMATSDX8
#endif  // TEXFORMAT_CRD
    reg_entry szTexelAlignmentString;              // TEXELALIGNMENT
    reg_entry szTextureManageStrategyString;       // TEXTUREMANAGESTRATEGY
    reg_entry szValidateZMethodString;             // VALIDATEZMETHOD
    reg_entry szVSyncModeString;                   // VSYNCMODE
    reg_entry szW16FormatString;                   // W16FORMAT
    reg_entry szW32FormatString;                   // W32FORMAT
    DWORD dwSyncPoint1;
    // non-boolean non-enumerated types
    reg_entry szAACompatibilityBitsString;         // AACOMPATIBILITYBITS
    reg_entry szAAReadCompatibilityFileString;     // AAREADCOMPATIBILITYFILE
    reg_entry szAGPTextureCutoff;                  // AGPTEXCUTOFF
    reg_entry szCapturePlayFilenum;                // D3D_REG_CAPTUREPLAYFILENUM
    reg_entry szCaptureRecordFilenum;              // D3D_REG_CAPTUREPLAYRECORDNUM
    reg_entry szD3DContextMaxString;               // D3DCONTEXTMAX
    reg_entry szD3DTextureMaxString;               // D3DTEXTUREMAX
    reg_entry szDebugLevelString;                  // DEBUGLEVEL
    reg_entry szLodBiasAdjustString;               // LODBIASADJUST
    reg_entry szMinVideoTexSizeString;             // MINVIDEOTEXSIZE
    reg_entry szPCITexHeapSizeString;              // PCITEXHEAPSIZE
    reg_entry szPerformanceStrategyOrString;       // PSOR
    reg_entry szPerformanceStrategyAndString;      // PSAND
    reg_entry szPreRenderLimitString;              // NOVSYNCPRERENDERLIMIT
    reg_entry szPushBufferSizeMaxString;           // DMAPUSHBUFFERSIZEMAX
    reg_entry szWScale16String;                    // WSCALE16
    reg_entry szWScale24String;                    // WSCALE24
    reg_entry szZBiasString;                       // ZBIAS
    DWORD dwSyncPoint2;
    // strings
    reg_entry szCapturePlayFilenameString;         // CAPTUREPLAYFILENAME
    reg_entry szCapturePlayPathString;             // CAPTUREPLAYPATH
    reg_entry szCaptureRecordFilenameString;       // CAPTURERECORDFILENAME
    reg_entry szCaptureRecordPathString;           // CAPTURERECORDPATH
    DWORD dwSyncPoint3;
} reg_struc;
extern reg_struc reg;

//---------------------------------------------------------------------------

void decryptRegistryKeys (void);


#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////
// this function initializes globals - see global.c
void __stdcall init_globals (void);

// GUID compare functions which can be compiled as C or CPP
#if defined(__cplusplus) && !defined(WINNT)
#define IsEqualGUID_C(rguid1, rguid2) IsEqualGUID((rguid1), (rguid2))
#define IsEqualIID_C(riid1, riid2) IsEqualIID((riid1), (riid2))
#else
#define IsEqualGUID_C(rguid1, rguid2) IsEqualGUID(&(rguid1), &(rguid2))
#define IsEqualIID_C(riid1, riid2) IsEqualIID(&(riid1), &(riid2))
#endif  //  __cplusplus

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _global_h
