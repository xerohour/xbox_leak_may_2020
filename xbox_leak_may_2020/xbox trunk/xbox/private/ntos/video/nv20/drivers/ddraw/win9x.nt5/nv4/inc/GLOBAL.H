#ifndef _global_h // multiple include protection
#define _global_h

//////////////////////////////////////////////////////////////////////////////
// global flags
//
#define FS_MMX          0x00000001      //  ) do not change - some code 
#define FS_KATMAI       0x00000002      // )          relies on these values

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
typedef struct {
//////////
// debug and retail banks
    // bank 1
    PNVD3DCONTEXT pCurrentContext;
    DWORD         nvFifo;
    DWORD         nvFreeCount;
    DWORD         nvDmaPushOffsetMax;
    DWORD         nvDmaPushBase;
    DWORD         nvDmaLastPutAddr;
    DWORD         nvDmaMinPushCount;
    DWORD         dwCPUFeatureSet;

#ifdef NV_FASTLOOPS
    // bank 2.0
    DWORD         dwPrimCount;          // # of primitives to render
    DWORD         dwStrides;            // primitive stride info
    DWORD         pVertices;            // vertex base
    DWORD         pIndices;             // index base
    DWORD         dwIndex0;             // )
    DWORD         dwIndex1;             //  ) current index
    DWORD         dwIndex2;             // )
    DWORD         dwVertexSN;           // current vertex cache serial number
    // bank 2.1
    DWORD         dwVertex0;            // )
    DWORD         dwVertex1;            //  ) current vertex
    DWORD         dwVertex2;            // )
    DWORD         dwPrimSubCount;       // # of primitives in this batch
    DWORD        *pdwUVOffset;          // dx6 uv coord offset array
    DWORD         dwStripFanData;       // strip & fan storage (cull history & cull sense)
    DWORD         dwCullMask1;          //  ) cull masks
    DWORD         dwCullMask2;          // )
    // bank 2.2
    DWORD         dwTemp;               // temp for culling and fog calculations
    float         fRWFar;               // fixed w buffer scale
    DWORD         dwSpecCurr;           // )
    DWORD         dwBlend;              //  ) spec hack
    DWORD         dwSpecCount;          //  )
    DWORD         dwSpecularState;      // )
    DWORD         _pad2_2[2];
    // bank 2.3
    DWORD         dwVertexCache[16];    // vertex cache
    // bank 2.4
    float         fFogTableDensity;     // )
    float         fFogTableLinearScale; //  )
    float         fFogTableEnd;         //   ) fog parameters
    float         fFogC1;               //  )
    float         fFogC2;               // )
    DWORD         dwWMask1;             // ) w parameters
    DWORD         dwWMask2;             // )
    float         fTriangleArea;
    // bank 2.5
    DWORD         dwVertexGatherBuf[8];
    // bank 2.6
    DWORD         dwVertexGatherBuf2[3];
    BYTE*         dwILCData;
    DWORD         dwILCCount;
    DWORD         dwILCMax;
    DWORD         dwMemCopyOffset;
    float         fCTC1;
    DWORD         _pad2_6[1];
#else //!NV_FASTLOOPS
    // bank 2.1
    DWORD         dwVertexSN;
    DWORD         _pad2_1[7];
    // bank 2.2
    DWORD         dwVertexCache[16];
#endif //!NV_FASTLOOPS

#ifdef  NVD3D_DX6
    // bank 3
    NVD3DFVFDATA    fvfData;
#endif  // NVD3D_DX6

    // bank 4
    DWORD         dwProcessorSpeed;
    DWORD         dwHWTextureDate;
    DWORD         dwSWTextureDate;
    DWORD         _pad2[3];

//////////
// debug only banks
#ifdef _DEBUG
#endif

} global_struc;
extern global_struc global;
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////
// To simplify implementation, the #define list below alias original names
#define nvDmaPushOffsetMax              global.nvDmaPushOffsetMax
#define nvDmaPushBase                   global.nvDmaPushBase
#define nvDmaLastPutAddr                global.nvDmaLastPutAddr
#define VCacheSerialNumber              global.VCacheSerialNumber
#define nvFifo                          global.nvFifo
#define nvFreeCount                     global.nvFreeCount
#define nvDmaMinPushCount               global.nvDmaMinPushCount
#define pCurrentContext                 global.pCurrentContext

#ifndef NV_FASTLOOPS
#define dwVertexSN                      global.dwVertexSN
#define dwVertexCache                   global.dwVertexCache
#endif //!NV_FASTLOOPS

#ifdef  NVD3D_DX6
#define fvfData                         global.fvfData
#endif  // NVD3D_DX6


//////////////////////////////////////////////////////////////////////////////
// this function initializes globals - see global.c
void init_globals (void);

//////////////////////////////////////////////////////////////////////////////
// Inter Process Memory
void* AllocIPM (unsigned size);
void* ReallocIPM (void* ptr,unsigned size);
void FreeIPM (void* ptr);
void DestroyIPM (void);

//////////////////////////////////////////////////////////////////////////////
// nvidia mem copy
#define NV_MEMCOPY_DONTCARE             0   // default to dest aligned
#define NV_MEMCOPY_WANTSRCALIGNED       1
#define NV_MEMCOPY_WANTDESTALIGNED      2

void nvMemCopy  (DWORD dwDest,DWORD dwSrc,DWORD dwCount,DWORD dwFlags);
void nvMemTouch (DWORD dwSrc,DWORD dwCount);

#endif