/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mp.hpp
 *  Content:    Hardware initialization
 *
 ***************************************************************************/

#include <conio.h>

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

// The system reserves memory page zero for graphics.  This structure
// lays out the information stored in that page.

struct PAGE_ZERO
{
    // The first four bytes are reserved to implement the initial
    // jump to the push buffer.
    //
    DWORD m_PushBufferJump;
};

static __forceinline PAGE_ZERO *PageZero()
{
    return (PAGE_ZERO *)0x80000000;
}

//
// Xbox software class (NVX_SOFTWARE_CLASS_HANDLE)
//

#define NVX_FLIP_IMMEDIATE                                       (0x00000001)
#define NVX_FLIP_SYNCHRONIZED                                    (0x00000002)
#define NVX_PUSH_BUFFER_RUN                                      (0x00000003)
#define NVX_PUSH_BUFFER_FIXUP                                    (0x00000004)
#define NVX_FENCE                                                (0x00000005)
#define NVX_READ_CALLBACK                                        (0x00000006)
#define NVX_WRITE_CALLBACK                                       (0x00000007)
#define NVX_DXT1_NOISE_ENABLE                                    (0x00000008)
#define NVX_WRITE_REGISTER_VALUE                                 (0x00000009)


#define BLANK_GAMMA_COLOR       0

//
// The maximum number of flips that the miniport can enqueue.  This number is
// directly related to the maximum number of flip buffers - if triple 
// buffering, there can be a maximum of 2 flips enqueued.
//
#define MAX_QUEUED_FLIPS        2

//
// Tile regions
//
#define TILE_FRAMEBUFFER        0
#define TILE_ZBUFFER            1

//
// Hardware hash function
//
#define FIFO_HASH(h,c)  (((h>>0)&0x7FF)^((h>>11)&0x7FF)^((h>>22)&0x7FF)^((c<<7)&0x7FF))


//
// Instance memory to reserve
//
#define INSTANCE_MEM_MAXSIZE          (20*1024)
#define INST_PAD_SIZE                 0

//
// Fifo context
//
#define NUM_FIFOS_NV20                2 //32
#define FC1_SIZE_NV20                 64
#define FC2_SIZE_NV20                 16


//#define NV20_GR_CONTEXT_SIZE          (0x375*16)
#define NV20_GR_CONTEXT_SIZE          0x37f0


//
// Min/max VCO values
//
#define DEFAULT_FMINVCO               175000
#define DEFAULT_FMAXVCO               300000

//
// Various Nvidia constants
//

#define ADDR_SYSMEM                 1
#define ADDR_FBMEM                  2              // Frame buffer memory space
#define ADDR_AGPMEM                 3              // AGP memory space

#define DISPLAY_TYPE_MONITOR        0
#define DISPLAY_TYPE_TV             1

#define BUFFER_DRAM                 0
#define BUFFER_SGRAM                1
#define BUFFER_SDRAM                2
#define BUFFER_DDRAM                3
#define BUFFER_CRUSH_UMA            4

#define FC_MIN_ALIGN_SHIFT          10


//
// RDI ram selects (pulled from nv20 diags).
//
#define RDI_RAMSEL_IDX_FMT         0x00000026
#define RDI_RAMSEL_CAS0            0x0000002c
#define RDI_RAMSEL_CAS1            0x0000002d
#define RDI_RAMSEL_CAS2            0x0000002e
#define RDI_RAMSEL_ASSM_STATE      0x0000002f
#define RDI_RAMSEL_IDX_CACHE0      0x00000020
#define RDI_RAMSEL_IDX_CACHE1      0x00000021
#define RDI_RAMSEL_IDX_CACHE2      0x00000022
#define RDI_RAMSEL_IDX_CACHE3      0x00000023
#define RDI_RAMSEL_VTX_FILE0       0x00000028
#define RDI_RAMSEL_VTX_FILE1       0x00000029
#define RDI_RAMSEL_XL_P            0x00000010
#define RDI_RAMSEL_XL_VAB          0x00000015
#define RDI_RAMSEL_XL_XFREG        0x0000001a
#define RDI_RAMSEL_XL_XFCTX        0x00000017
#define RDI_RAMSEL_XL_LTCTX        0x00000018
#define RDI_RAMSEL_XL_LTC          0x00000019
#define RDI_RAMSEL_CRSTR_STIPP     0x00000045
#define RDI_RAMSEL_FD_CTRL         0x0000003d
#define RDI_RAMSEL_ROP_REGS        0x00000050

#define MAX_CRTCS                  2

#define PRAMIN_OFFSET              DEVICE_BASE(NV_PRAMIN)

//
// Number of timing values in the timing table
//
#define VIDEO_TIMING_VALUES        16

#define BUFFER_HSYNC_NEGATIVE      1
#define BUFFER_HSYNC_POSITIVE      0
#define BUFFER_VSYNC_NEGATIVE      1
#define BUFFER_VSYNC_POSITIVE      0


//
// TV standards
//
#define NTSC_M                     0
#define NTSC_J                     1
#define PAL_M                      2
#define PAL_A                      3   // PAL B, D, G, H, I
#define PAL_N                      4
#define PAL_NC                     5

//
// Helper macros
//
#define MpIsFlagSet(flagset, flag)           (((flagset) & (flag)) != 0)
#define MpIsFlagClear(flagset, flag)         (((flagset) & (flag)) == 0)
#define MP_ROUND_TO_SIZE(LENGTH,ALIGNMENT)   (((LENGTH) + ((ALIGNMENT) - 1)) & ~((ALIGNMENT) - 1))


#define BIT(b)                  (1<<(b))
#define DEVICE_BASE(d)          (0?d)
#define DEVICE_EXTENT(d)        (1?d)
#define DRF_SHIFT(drf)          ((0?drf) % 32)
#define DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUM(d,r,f,n)        (((n)&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUMFAST(d,r,f,n)    ((n)<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_VAL(d,r,f,v)        (((v)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
#define DISPLAYTYPEBIT(d)       (1 << d)


//
// Macros/functions to access hardware registers
//

FORCEINLINE ULONG
REG_RD32(VOID* Ptr, ULONG Addr)
{
   return *((volatile DWORD*)((BYTE*)(Ptr) + (Addr)));
}

FORCEINLINE VOID
REG_WR32(VOID* Ptr, ULONG Addr, ULONG Val)
{
    *((volatile DWORD*)((BYTE*)(Ptr) + (Addr))) = (DWORD)(Val);
}

FORCEINLINE BYTE
REG_RD08(VOID* Ptr, ULONG Addr)
{
    return *((volatile BYTE*)((BYTE*)(Ptr) + (Addr)));
}

FORCEINLINE VOID
REG_WR08(VOID* Ptr, ULONG Addr, BYTE Val)
{
    *((volatile BYTE*)((BYTE*)(Ptr) + (Addr))) = (BYTE)(Val);
}



#define REG_RD_DRF(Ptr,d,r,f)       (((REG_RD32(Ptr,NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

#define REG_WR_DRF_NUM(Ptr,d,r,f,n) REG_WR32(Ptr, NV ## d ## r, DRF_NUM(d,r,f,n))
#define REG_WR_DRF_DEF(Ptr,d,r,f,c) REG_WR32(Ptr, NV ## d ## r, DRF_DEF(d,r,f,c))
#define FLD_WR_DRF_NUM(Ptr,d,r,f,n) REG_WR32(Ptr, NV##d##r,(REG_RD32(Ptr, NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
#define FLD_WR_DRF_DEF(Ptr,d,r,f,c) REG_WR32(Ptr, NV##d##r,(REG_RD32(Ptr, NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))

#define HAL_CRTC_WR(Ptr,i,d,a)      {REG_WR08(Ptr, NV_PRMCIO_CRX__COLOR + a, i);\
                                     REG_WR08(Ptr, NV_PRMCIO_CR__COLOR + a, d);}
#define HAL_CRTC_RD(Ptr,i,d,a)      {REG_WR08(Ptr, NV_PRMCIO_CRX__COLOR + a, i);\
                                     (d) = REG_RD08(Ptr, NV_PRMCIO_CR__COLOR + a);}

#define INST_OFFSET(i,o)            (DEVICE_BASE(NV_PRAMIN)+((i)<<4)+(o))
#define INST_WR32(Ptr,i,o,d)        REG_WR32(Ptr, DEVICE_BASE(NV_PRAMIN)+((i)<<4)+(o),(d))
#define INST_RD32(Ptr,i,o)          REG_RD32(Ptr, DEVICE_BASE(NV_PRAMIN)+((i)<<4)+(o))
#define SF_OFFSET(sf)               (((0?sf)/32)<<2)
#define SF_SHIFT(sf)                ((0?sf)&31)
#define SF_MASK(sf)                 (0xFFFFFFFF>>(31-(1?sf)+(0?sf)))
#define SF_DEF(s,f,c)               ((NV ## s ## f ## c)<<SF_SHIFT(NV ## s ## f))
#define SF_NUM(s,f,n)               (((n)&SF_MASK(NV ## s ## f))<<SF_SHIFT(NV ## s ## f))
#define SF_VAL(s,f,v)               (((v)>>SF_SHIFT(NV ## s ## f))&SF_MASK(NV ## s ## f))

#define CRTC_WR(Ptr,i,d)            {REG_WR08(Ptr, NV_PRMCIO_CRX__COLOR, (i) & 0x3F);\
                                     REG_WR08(Ptr, NV_PRMCIO_CR__COLOR, (d));}
#define CRTC_RD(Ptr,i,d)            {REG_WR08(Ptr, NV_PRMCIO_CRX__COLOR, (i) & 0x3F);\
                                     (d) = REG_RD08(Ptr, NV_PRMCIO_CR__COLOR);}

#define SF_OFFSET(sf)               (((0?sf)/32)<<2)
#define SF_SHIFT(sf)                ((0?sf)&31)
#define SF_MASK(sf)                 (0xFFFFFFFF>>(31-(1?sf)+(0?sf)))
#define SF_VAL(s,f,v)               (((v)>>SF_SHIFT(NV ## s ## f))&SF_MASK(NV ## s ## f))

#define RDI_REG_WR32(Ptr,select,addr,data)  {REG_WR32(Ptr, NV_PGRAPH_RDI_INDEX,                 \
                                             DRF_NUM(_PGRAPH, _RDI_INDEX, _SELECT, select) |    \
                                             DRF_NUM(_PGRAPH, _RDI_INDEX, _ADDRESS, addr));     \
                                             REG_WR32(Ptr, NV_PGRAPH_RDI_DATA, data);}

#define DAC_REG_WR_DRF_DEF(Ptr,d,r,f,c)     REG_WR32(Ptr, NV ## d ## r, DRF_DEF(d,r,f,c))


//
// MaskRevision constants.
//
#define MC_MASK_REVISION_A1             0xA1
#define MC_MASK_REVISION_A2             0xA2
#define MC_MASK_REVISION_A3             0xA3
#define MC_MASK_REVISION_A6             0xA6
#define MC_MASK_REVISION_B1             0xB1
#define MC_MASK_REVISION_B2             0xB2


//
// Drive/slew calculation constants.
//

// Operating condition based on speedometer
#define SLOW_COUNT_EXT                  91
#define SLOW_COUNT_AVG                  100
#define TYPI_COUNT                      108
#define FAST_COUNT_AVG                  116
#define FAST_COUNT_EXT                  123


// EXTREME FAST Case Values
#define ADR_DRV_FALL_FAST_EXT           0xa
#define ADR_DRV_RISE_FAST_EXT           0x8
#define ADR_SLW_FALL_FAST_EXT           0xb
#define ADR_SLW_RISE_FAST_EXT           0x8

#define CLK_DRV_FALL_FAST_EXT           0xa
#define CLK_DRV_RISE_FAST_EXT           0x8
#define CLK_SLW_FALL_FAST_EXT           0xb
#define CLK_SLW_RISE_FAST_EXT           0x8

#define DAT_DRV_FALL_FAST_EXT           0x9
#define DAT_DRV_RISE_FAST_EXT           0x9
#define DAT_SLW_FALL_FAST_EXT           0xe
#define DAT_SLW_RISE_FAST_EXT           0x8

#define DQS_DRV_FALL_FAST_EXT           0x9
#define DQS_DRV_RISE_FAST_EXT           0x9
#define DQS_SLW_FALL_FAST_EXT           0xe
#define DQS_SLW_RISE_FAST_EXT           0x8

#define DQS_INB_DELY_FAST_EXT           0x6


// Average FAST Case Values
#define ADR_DRV_FALL_FAST_AVG           0xa
#define ADR_DRV_RISE_FAST_AVG           0x8
#define ADR_SLW_FALL_FAST_AVG           0xb
#define ADR_SLW_RISE_FAST_AVG           0x8

#define CLK_DRV_FALL_FAST_AVG           0xa
#define CLK_DRV_RISE_FAST_AVG           0x8
#define CLK_SLW_FALL_FAST_AVG           0xb
#define CLK_SLW_RISE_FAST_AVG           0x8

#define DAT_DRV_FALL_FAST_AVG           0x9
#define DAT_DRV_RISE_FAST_AVG           0x9
#define DAT_SLW_FALL_FAST_AVG           0xe
#define DAT_SLW_RISE_FAST_AVG           0x8

#define DQS_DRV_FALL_FAST_AVG           0x9
#define DQS_DRV_RISE_FAST_AVG           0x9
#define DQS_SLW_FALL_FAST_AVG           0xe
#define DQS_SLW_RISE_FAST_AVG           0x8

#define DQS_INB_DELY_FAST_AVG           0x6


// TYPICAL Case Values
#define ADR_DRV_FALL_TYPI               0xa
#define ADR_DRV_RISE_TYPI               0x8
#define ADR_SLW_FALL_TYPI               0xb
#define ADR_SLW_RISE_TYPI               0x8

#define CLK_DRV_FALL_TYPI               0xa
#define CLK_DRV_RISE_TYPI               0x8
#define CLK_SLW_FALL_TYPI               0xb
#define CLK_SLW_RISE_TYPI               0x8

#define DAT_DRV_FALL_TYPI               0x9
#define DAT_DRV_RISE_TYPI               0x9
#define DAT_SLW_FALL_TYPI               0xe
#define DAT_SLW_RISE_TYPI               0x8

#define DQS_DRV_FALL_TYPI               0x9
#define DQS_DRV_RISE_TYPI               0x9
#define DQS_SLW_FALL_TYPI               0xe
#define DQS_SLW_RISE_TYPI               0x8

#define DQS_INB_DELY_TYPI               0x5


// Average SLOW Case Values
#define ADR_DRV_FALL_SLOW_AVG           0xc
#define ADR_DRV_RISE_SLOW_AVG           0xb
#define ADR_SLW_FALL_SLOW_AVG           0xb
#define ADR_SLW_RISE_SLOW_AVG           0x8

#define CLK_DRV_FALL_SLOW_AVG           0xc
#define CLK_DRV_RISE_SLOW_AVG           0xb
#define CLK_SLW_FALL_SLOW_AVG           0xb
#define CLK_SLW_RISE_SLOW_AVG           0x8

#define DAT_DRV_FALL_SLOW_AVG           0xa
#define DAT_DRV_RISE_SLOW_AVG           0xb
#define DAT_SLW_FALL_SLOW_AVG           0xe
#define DAT_SLW_RISE_SLOW_AVG           0x8

#define DQS_DRV_FALL_SLOW_AVG           0xa
#define DQS_DRV_RISE_SLOW_AVG           0xb
#define DQS_SLW_FALL_SLOW_AVG           0xe
#define DQS_SLW_RISE_SLOW_AVG           0x8

#define DQS_INB_DELY_SLOW_AVG           0x4

// EXTREME SLOW Case Values
#define ADR_DRV_FALL_SLOW_EXT           0xe
#define ADR_DRV_RISE_SLOW_EXT           0xe
#define ADR_SLW_FALL_SLOW_EXT           0xb
#define ADR_SLW_RISE_SLOW_EXT           0x8

#define CLK_DRV_FALL_SLOW_EXT           0xe
#define CLK_DRV_RISE_SLOW_EXT           0xe
#define CLK_SLW_FALL_SLOW_EXT           0xb
#define CLK_SLW_RISE_SLOW_EXT           0x8

#define DAT_DRV_FALL_SLOW_EXT           0xb
#define DAT_DRV_RISE_SLOW_EXT           0xe
#define DAT_SLW_FALL_SLOW_EXT           0xe
#define DAT_SLW_RISE_SLOW_EXT           0x8

#define DQS_DRV_FALL_SLOW_EXT           0xb
#define DQS_DRV_RISE_SLOW_EXT           0xe
#define DQS_SLW_FALL_SLOW_EXT           0xe
#define DQS_SLW_RISE_SLOW_EXT           0x8

#define DQS_INB_DELY_SLOW_EXT           0x2

#define VID_NVNOTIFICATION_SIZE               (16)
#define VID_NVNOTIFICATION_DMA_BUFFER_SIZE    (VID_NVNOTIFICATION_SIZE * 32)

#define CRYSTAL_135                     13500000
#define CRYSTAL_166                     16666666

struct DISPLAYMODE
{
    ULONG  AvInfo;
    USHORT Width;
    USHORT Height;
    ULONG  DisplayMode;
};

extern const DISPLAYMODE g_DisplayModes[];
extern const DWORD g_DisplayModeCount;

const DISPLAYMODE *GetTableForCurrentAvInfo();

//
// Miniport class
//
class CMiniport
{
public:

    //
    // Nested data types
    //

    struct GENERALINFO
    {
        ULONG ChipId;
        ULONG VideoRamSize;
        ULONG VideoRamType;
        ULONG ChipIntrEn0;
        ULONG MpVIPSlavePresent;
        ULONG CrystalFreq;
        ULONG MaskRevision;
    };

    struct DACINFO
    {
        // DACHALINFO
        ULONG MClk;
        ULONG VClk;
        ULONG NVClk;
        ULONG MPllM;
        ULONG MPllN;
        ULONG MPllO;
        ULONG MPllP;
        ULONG VPllM;
        ULONG VPllN;
        ULONG VPllO;
        ULONG VPllP;
        ULONG NVPllM;
        ULONG NVPllN;
        ULONG NVPllO;
        ULONG NVPllP;
    };  

    struct HALINFO
    {
        // Fifo
        LONG  FifoChID;
        ULONG FifoMode;
        BOOL  FifoInUse;
        ULONG FifoInstance;
        ULONG FifoAllocCount;
        LONG  FifoCacheDepth;
        ULONG FifoObjectCount;
        ULONG FifoIntrEn0;                         // interrupt enable mask
        ULONG FifoRetryCount;
        ULONG FifoUserBase;                        // offset of user channel area
        ULONG FifoContextAddr1;                    // size=NUM_FIFOS_NV20*FC1_SIZE_NV20
        ULONG FifoContextAddr2;                    // size=NUM_FIFOS_NV20*FC2_SIZE_NV20

        // Hash table
        ULONG HashTableAddr;                       // size=HASH_TABLE_SIZE

        // Gr
        ULONG GrChID;
        ULONG GrCtxTable[NUM_FIFOS_NV20];          // graphics channel context table entries
        ULONG GrCtxTableBase;                      // base instance offset of table (aka grInstMem)
        ULONG GrCurrentObjects3d[NUM_FIFOS_NV20];

        // Fb
        ULONG FbSave0;
        ULONG FbSave1;

        // Mc
        ULONG McSave;
        ULONG McSaveIntrEn0;
    };

    struct VIDEOMODETIMING
    {
        ULONG HorizontalVisible;
        ULONG VerticalVisible;
        ULONG Refresh;
        ULONG HorizontalTotal;
        ULONG HorizontalBlankStart;
        ULONG HorizontalRetraceStart;
        ULONG HorizontalRetraceEnd;
        ULONG HorizontalBlankEnd;
        ULONG VerticalTotal;
        ULONG VerticalBlankStart;
        ULONG VerticalRetraceStart;
        ULONG VerticalRetraceEnd;
        ULONG VerticalBlankEnd;
        ULONG PixelClock;
        ULONG HorizontalSyncPolarity;
        ULONG VerticalSyncPolarity;
    };

    struct HW_HASHENTRY
    {
        ULONG ht_ObjectHandle;
        ULONG ht_Context;
    };


    //
    // Object information
    //

    struct OBJECTINFO
    {
        ULONG Handle;
        USHORT SubChannel;
        USHORT Engine;
        ULONG ClassNum;
        ULONG Instance;

        VOID Init()
        {
            memset(this, 0, sizeof(OBJECTINFO));
        }
    };


    struct VBLANKFLIPS
    {
        BOOL   Pending;
        ULONG  Offset;
    };


    //
    // Public member variables
    //

    PVOID            m_RegisterBase;

    ULONG            m_SurfacePitch;
    ULONG            m_DisplayMode;
    ULONG            m_Format;

    PVOID            m_InstMem;

    KINTERRUPT       m_InterruptObject;
    KDPC             m_Dpc;
    BOOL             m_InterruptsEnabled;
    BOOL             m_UnhandledVBI;

    GENERALINFO      m_GenInfo;
    DACINFO          m_DacInfo;
    HALINFO          m_HalInfo;

    VIDEOMODETIMING  m_VideoModeTiming;
    ULONG            m_VideoModeDepth;
    ULONG            m_FreeInstAddr;

    HAL_SHUTDOWN_REGISTRATION m_ShutdownRegistration;

    VBLANKFLIPS      m_VBlankFlips[MAX_QUEUED_FLIPS];
    D3DSWAPCALLBACK  m_pSwapCallback;
    D3DVBLANKCALLBACK m_pVerticalBlankCallback;
    KEVENT           m_VerticalBlankEvent;

    KEVENT           m_BusyBlockEvent;

    // Current AV mode
    DWORD            m_CurrentAvInfo;
    BOOL             m_FirstFlip;

    ULONG            m_VBlankFlipCount;
    ULONG            m_VBlankCount;
    ULONG            m_VBlankCountAtLastFlip;
    ULONG            m_VBlanksBetweenFlips;
    ULONG            m_FlipCount;
    BOOL             m_OrImmediate;
    ULONG            m_IsOddField;

    ULONG            m_TimeBetweenVBlanks;
    ULONG            m_TimeOfLastVBlank;
    ULONG            m_TimeOfLastFlip;

    D3DGAMMARAMP     m_GammaRamp[2];
    BOOL             m_GammaUpdated[2];
    ULONG            m_GammaCurrentIndex;

    ULONG            m_OverlayVBlank;
    ULONG            m_DebugRegister[11];

    //
    // Data for handling push-buffer fix-ups
    //

    struct PUSHBUFFERFIXUPINFO
    {
        DWORD*  pFixupData;
        BYTE*   pStart;
        DWORD   ReturnOffset;
        DWORD*  ReturnAddress;
    };

    DWORD            m_PusherGetRunTotal;

    //
    // Public member functions
    //

    BOOL InitHardware();

    static DWORD GetPresentFlagsFromAvInfo(DWORD AvInfo);
    static DWORD GetDisplayCapabilities();

    void SetVideoMode(ULONG Width, ULONG Height, ULONG Refresh, ULONG Flags, D3DFORMAT Format, ULONG PresentationInterval, ULONG Stride);

    BOOL InitDMAChannel(ULONG Class, OBJECTINFO* ErrorContext, OBJECTINFO* DataContext, ULONG Offset, PVOID* ppChannel);
    BOOL BindToChannel(OBJECTINFO* Object);
    BOOL CreateGrObject(ULONG Handle, ULONG ClassNum, OBJECTINFO* Object);
    BOOL CreateCtxDmaObject(ULONG Dma, ULONG ClassNum, PVOID Base, ULONG Limit, OBJECTINFO* Object);
    #if DBG && ENABLE_DMA_RANGE
    DWORD SetDmaRange(ULONG DmaContext, D3DSurface* pSurface);
    #else
    DWORD SetDmaRange(ULONG DmaContext, D3DSurface* pSurface) { return 0; } // Do nothing 
    #endif

    BOOL CreateTile(ULONG TileRegion, ULONG Offset, ULONG Size, ULONG Pitch, ULONG MemType, ULONG ZSlot, ULONG ZOffset);
    BOOL DestroyTile(ULONG TileRegion, ULONG ZOffset);
    VOID ShutdownEngines();
    VOID DacProgramGammaRamp(D3DGAMMARAMP* GammaRamp);
    BOOL IsFlipPending();

    VOID DacProgramVideoStart(ULONG startAddr);

    VOID DisableInterrupts(PVOID regbase)
    {
        REG_WR32(regbase, NV_PMC_INTR_EN_0, 0);
    }

    VOID EnableInterrupts(PVOID regbase)
    {
        REG_WR32(regbase, NV_PMC_INTR_EN_0, m_GenInfo.ChipIntrEn0);
    }

    ULONG GetRefreshRate()
    {
        if (m_CurrentAvInfo & AV_FLAGS_60Hz)
        {
            return 60;
        }
        else
        {
            return 50;
        }
    }

    ULONG GetTime()
    {
        __asm rdtsc
    }

    static BOOLEAN Isr(PKINTERRUPT InterruptObject, PVOID ServiceContext);
    static VOID Dpc(PKDPC Dpc, PVOID DpcContext, PVOID Arg1, PVOID Arg2);
    static VOID ShutdownNotification(PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration);

    VOID TilingUpdateIdle(ULONG* dmapush);
    VOID FixupPushBuffer(PUSHBUFFERFIXUPINFO *pPushBufferFixupInfo, ULONG Method);

private:

    //
    // Private member functions
    //

    BOOL InitEngines();
    BOOL LoadEngines();
    BOOL MapRegisters();
    BOOL GetGeneralInfo();

    VOID InitGammaRamp(ULONG RampNo);

    VOID SoftwareMethod(ULONG Method, ULONG Data);

    VOID SetupPaletteAndGamma();

    ULONG ReserveInstMem(ULONG Size);

    VOID GetAddressInfo(PVOID pLinearAddress, PVOID* ppAddress, ULONG* pAddressSpace, BOOL IsAlias);

    ULONG ServiceGrInterrupt();
    ULONG VBlank();
    VOID  VBlankFlip(ULONG Offset, ULONG FlipTime);
    ULONG ServiceFifoInterrupt();
    ULONG ServiceMediaPortInterrupt();
    ULONG ServiceVideoInterrupt();

    VOID HalMcControlInit();
    VOID HalFbControlInit();
    VOID HalVideoControlInit();

    VOID HalMpControlInit();

    VOID HalGrControlInit();
    VOID HalGrControlLoad();
    VOID HalGrIdle();
    VOID HalGrLoadChannelContext(ULONG ChID);
    VOID HalGrUnloadChannelContext(ULONG ChID);
    VOID HalGrInitObjectContext(ULONG Instance,ULONG ClassNum);
    VOID HalGrInit3d();

    VOID HalFifoControlInit();
    VOID HalFifoControlLoad();
    VOID HalFifoContextSwitch(ULONG ChID);
    VOID HalFifoAllocDMA(ULONG fetchTrigger, ULONG fetchSize, ULONG fetchRequests,
                         OBJECTINFO* DataContext);
    VOID HalFifoHashAdd(ULONG entry, ULONG handle, ULONG chid, ULONG instance, ULONG engine);

    VOID HalDacControlInit();
    VOID HalDacLoad();
    VOID HalDacUnload();
    VOID HalDacProgramMClk();
    VOID HalDacProgramNVClk();
    VOID HalDacProgramPClk();

    #if DBG
    VOID DumpClocks();
    #else
    VOID DumpClocks() {}    // do nothing on retail builds
    #endif
    //
    // Inline functions
    //


    VOID GrDone()
    {
        while (REG_RD32(m_RegisterBase, NV_PGRAPH_STATUS))
            ;
    }

    VOID TmrDelay(ULONG nsec);


    BYTE UnlockCRTC()
    {
        BYTE lock = ReadCRTCLock();
        CRTC_WR(m_RegisterBase, NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE);
        return lock;
    }

    VOID RestoreCRTCLock(BYTE lock)
    {
        CRTC_WR(m_RegisterBase, NV_CIO_SR_LOCK_INDEX, lock);
    }

    BYTE ReadCRTCLock()
    {
        BYTE data;

        CRTC_RD(m_RegisterBase, NV_CIO_SR_LOCK_INDEX, data);
        return (BYTE)((data == 0x03) ? 0x57 : (data == 0x01) ? 0x75 : 0x99);
    }

public:

    BOOL IsOddField()
    {
        return m_IsOddField;
    }
};




// Standard VGA Mode Table format.
struct VGATBL
{
    struct SEQ
    {
        BYTE ClockMode;
        BYTE MapMask;
        BYTE FontAddr;
        BYTE MemMode;
    };

    struct CRTC
    {
        BYTE   HTotal;
        BYTE   HDispEnd;
        BYTE   HBlankS;
        BYTE   HBlankE;
        BYTE   HSyncS;
        BYTE   HSyncE;
        BYTE   VTotal;
        BYTE   Overflow;
        BYTE   PresetRowScan;
        BYTE   CellHeight;
        BYTE   CursorS;
        BYTE   CursorE;
        BYTE   RegenSHigh;
        BYTE   RegenSLow;
        BYTE   CursorPosHigh;
        BYTE   CursorPosLow;
        BYTE   VSyncS;
        BYTE   VSyncE;
        BYTE   VDispE;
        BYTE   RowOffset;
        BYTE   ULineRow;
        BYTE   VBlankS;
        BYTE   VBlandE;
        BYTE   Mode;
        BYTE   LineCompare;
    };

    struct GR
    {
        BYTE   SetReset;
        BYTE   EnableSetReset;
        BYTE   ColorCmp;
        BYTE   ROP;
        BYTE   ReadMap;
        BYTE   Mode;
        BYTE   Misc;
        BYTE   CDC;
        BYTE   BitMask;
    };

    BYTE   columns;
    BYTE   rows;
    BYTE   char_height;
    U016   RegenLenght;
    SEQ    seq_regs;
    BYTE   PT_Misc;
    CRTC   crtc_regs;
    BYTE   PT_ATC[20];
    GR     gr_regs;
};

} // end namespace

