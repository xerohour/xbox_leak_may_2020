//
// MODES.H - Header file for mode sets in 'C'.
//
// Copyright (c) 1997 - NVidia Corp.
// All Rights Reserved.
//
// Defines and structures for use with the 'C' based modeset code.
//
//

#ifdef DOS_COMPILE
typedef unsigned long U032;
typedef unsigned short U016;
typedef unsigned char U008;
#define VOID void
#endif


#define PACKED  0
#define PLANAR  1

#define NVRM_ACCESS         0x38
#define NVRM_ACCESS_MASK    0x01
#define NVRM_IDENT          0x00
#define NVRM_ADDRESS        0x02
#define NVRM_DATA           0x04
#define NVRM_DATA32         0x06
#define NVRM_SELECT_MASK    0x06    // Mask for decode bits
#define NVRM_SELECT         0x01    // Least signifcant bit is 1

#ifdef DOS_COMPILE
//
// These defines are the standard NV defines... the appropriate include
// file should be utilized, rather than these values.
//
#define NV_PFB_BOOT_0_RAM_AMOUNT_1MB       0x00000000 /* RW--V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_2MB       0x00000001 /* RW--V */
#define NV_PFB_BOOT_0_RAM_AMOUNT_4MB       0x00000002 /* RW--V */
#define NV_PRMVIO_MISC__WRITE              0x000C03c2 /* -W-1R */
#define NV_PRMVIO_SRX                      0x000C03c4 /* RW-1R */
#define NV_PRMVIO_GRX                      0x000C03ce /* RW-1R */
#define NV_PFB_BOOT_0                      0x00100000 /* RW-4R */
#define NV_PFB_CONFIG_0                    0x00100200 /* RW-4R */
#define NV_PRMCIO_ARX                      0x006013c0 /* RW-1R */
#define NV_PRMCIO_AR_PALETTE__WRITE        0x006013c0 /* -W-1R */
#define NV_PRMCIO_AR_PALETTE__READ         0x006013c1 /* R--1R */
#define NV_PRMCIO_CRX__COLOR               0x006013d4 /* RW-1R */
#define NV_PRMCIO_INP0__COLOR              0x006013da /* R--1R */
#define NV_PRAMDAC_VPLL_COEFF              0x00680508 /* RW-4R */
#define NV_PRAMDAC_PLL_COEFF_SELECT        0x0068050C /* RW-4R */
#define NV_PRAMDAC_GENERAL_CONTROL         0x00680600 /* RW-4R */
#define NV_USER_DAC_PIXEL_MASK             0x006813C6 /* RWI1R */
#define NV_USER_DAC_READ_MODE_ADDRESS      0x006813C7 /* RW-1R */
#define NV_USER_DAC_WRITE_MODE_ADDRESS     0x006813C8 /* RW-1R */
#define NV_USER_DAC_PALETTE_DATA           0x006813C9 /* RW-1R */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO  28    /* RWIVF */
#define NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE 16    /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_565_MODE     12    /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE      4    /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE     8    /* RWIVF */
#define NV_PRAMDAC_GENERAL_CONTROL_BPC          20    /* RWIVF */
#define NV_PFB_CONFIG_0_TILING                  12    /* RWIVF */
#define NV_PFB_CONFIG_0_TILING_DISABLED    0x00000001 /* RWI-V */
#endif // DOS_COMPILE


#define CRYSTAL_FREQ143         14318180           // 14.318,180 MHz
#define CRYSTAL_FREQ135         13500000           // 13.500,180 MHz
#define CRYSTAL_FREQ143KHZ      14318              // 14318 kHz
#define CRYSTAL_FREQ135KHZ      13500              // 13500 kHz

//
// VBE3.0-based CRTC Timings Table
//
typedef struct gtf_timings_structure
{
    U016    horiz_total;
    U016    horiz_start;
    U016    horiz_end;
    U016    vertical_total;
    U016    vertical_start;
    U016    vertical_end;
    U008    flags;
    U032    dot_clock;
    U016    refresh;
} GTF_TIMINGS, *PGTF_TIMINGS;

//
// DMT Override Table
//
typedef struct DMT_Overrides_structure
{
    U016    DotClock;
    U016    Polarity;
    U016    CR0;
    U016    CR2;
    U016    CR3;
    U016    CR4;
    U016    CR5;
    U016    CR6;
    U016    CR7;
    U016    CR10;
    U016    CR11;
    U016    CR15;
    U016    CR16;

} DMT_OVERRIDES, *PDMT_OVERRIDES;

//
// State flags for GTF structure
//
#define GTF_FLAGS_SINGLE_SCANNED    0x00
#define GTF_FLAGS_DOUBLE_SCANNED    0x01
#define GTF_FLAGS_NON_INTERLACED    0x00
#define GTF_FLAGS_INTERLACED        0x02
#define GTF_FLAGS_HSYNC_POSITIVE    0x00
#define GTF_FLAGS_HSYNC_NEGATIVE    0x04
#define GTF_FLAGS_VSYNC_POSITIVE    0x00
#define GTF_FLAGS_VSYNC_NEGATIVE    0x08


// Standard VGA Mode Table Structure
typedef struct _seq_
{
    U008 ClockMode;
    U008 MapMask;
    U008 FontAddr;
    U008 MemMode;
} SEQ;

typedef struct  _crtc_
{
    U008   HTotal;
    U008   HDispEnd;
    U008   HBlankS;
    U008   HBlankE;
    U008   HSyncS;
    U008   HSyncE;
    U008   VTotal;
    U008   Overflow;
    U008   PresetRowScan;
    U008   CellHeight;
    U008   CursorS;
    U008   CursorE;
    U008   RegenSHigh;
    U008   RegenSLow;
    U008   CursorPosHigh;
    U008   CursorPosLow;
    U008   VSyncS;
    U008   VSyncE;
    U008   VDispE;
    U008   RowOffset;
    U008   ULineRow;
    U008   VBlankS;
    U008   VBlandE;
    U008   Mode;
    U008   LineCompare;
} CRTC;

typedef struct  _gr_
{
    U008   SetReset;
    U008   EnableSetReset;
    U008   ColorCmp;
    U008   ROP;
    U008   ReadMap;
    U008   Mode;
    U008   Misc;
    U008   CDC;
    U008   BitMask;
} GR;

// Standard VGA Mode Table format.
typedef struct _vga_mode_
{
    U008   columns;
    U008   rows;
    U008   char_height;
    U016  RegenLenght;
    SEQ     seq_regs;              // SR1-4
    U008   PT_Misc;               // Misc Register
    CRTC    crtc_regs;             // CR0-18
    U008   PT_ATC[20];            // Attribute Controller
    GR      gr_regs;               // GR0-8
} VGATBL;

// Indexes into the clock table
#define MAX_DCLK    0x1F            // 229.50 Mhz
#define MAX_ATC     0x16            // 108 Mhz

#define    CLKID_50_35     0x00
#define    CLKID_56_64     0x01
#define    CLKID_33_25     0x02
#define    CLKID_52        0x03
#define    CLKID_80        0x04
#define    CLKID_63        0x05
#define    CLKID_EXT       0x06
#define    CLKID_75        0x07
#define    CLKID_25_175    0x08
#define    CLKID_28_322    0x09
#define    CLKID_31_5      0x0A
#define    CLKID_36        0x0B
#define    CLKID_40        0x0C
#define    CLKID_44_9      0x0D
#define    CLKID_50        0x0E
#define    CLKID_65        0x0F
#define    CLKID_108       0x10        // Requires clock doubler
#define    CLKID_135       0x11        // Requires clock doubler
#define    CLKID_49_5      0x12
#define    CLKID_56_25     0x13
#define    CLKID_78_75     0x14
#define    CLKID_94_5      0x15
#define    CLKID_108_5     0x16        // Requires clock doubler
#define    CLKID_35_5      0x17
#define    CLKID_158       0x18        // Requires clock doubler
#define    CLKID_55_86     0x19
#define    CLKID_20        0x1A
#define    CLKID_27_93     0x1B
#define    CLKID_32_5      0x1C
#define    CLKID_12_588    0x1D
#define    CLKID_81_62     0x1E
#define    CLKID_219_566   0x1F
#define    CLKID_172_798   0x20
#define    CLKID_193_156   0x21
#define    CLKID_229_5     0x22
#define    CLKID_136_36    0x23

// Video Mode Control Structure
typedef struct _vmode_
{
    U016  mdVMode;                // Mode Number of this mode
    U008   mdMode;                 // Internal Mode Number
    U008   mdIMode;                // Standard Mode Table Index
    U008   mdEMode;                // Extended Mode Table Index
    U008   mdTVMode;               // TV Mode Table Index
    U008   mdCMode;                // Extended CRTC Override Index
    U008   mdBPP;                  // Bits per Pixel
    U016  mdXRes;                 // X resolutio
    U016  mdYRes;                 // Y resolutionn
    U016  mdRowOff;               // Scanline Row Offset
    U016  mdMaxDotClock;          // Maximum DCLK

} MODEDESC;


typedef struct _ext_regs_
{
                            // ++++------- Reserved
                            // |||| +----- NV_PRAMDAC_GENERAL_CONTROL[20] - DAC Width
                            // |||| |+---- CR1C[2] - Sequential Chain 4
                            // |||| ||+--- CR1C[1] - Page Select Control
                            // |||| |||+-- CR1A[0] - Address Wrap
    U008   xrFlags;         // VVVV VVVV
                            // 7654 3210

                            // +---------- NV_PRAMDAC_GENERAL_CONTROL[12] - 565_MODE
                            // |+++------- NV_PRAMDAC_GENERAL_CONTROL[20] - DAC Width
                            // |||| +----- CR25[5] - Bit 11 of offset
                            // |||| |+++-- CR19[7:5] - Bits 8->10 of Offset
    U008   xrOffset;        // VVVV VVVV
                            // 7654 3210

                            // ++--------- CR2D[3:2] - Bit 8 of HRetrace & HBlank Starts
                            // ||+-------- CR1A[2] - Large Screen bit
                            // |||+------- CR25[4] - Bit 6 of HBlank End
                            // |||| ++---- Bit 10 of VBlank & VRetrace Start
                            // |||| ||++-- VDisp End, VTotal
    U008   xrVOver;         // VVVV VVVV
                            // 7654 3210

                            // ++--------- CR2D[1:0] - Bit 8 of DHisp End and HTotoal
                            // ||++------- CR1C[4:3] - Optimized chain-4 write/read access
                            // |||| +----- CR28[3] - NV_PRAMDAC_PLL_COEFF_SELECT[28]
                            // |||| |                VCLK Divide by 1 or 2
                            // |||| |+---- CR28[2] - Linear/Tile mode
                            // |||| ||++-- CR28[1:0] - Pixel Format
    U008   xrPixFmt;        // VVVV VVVV
                            // 7654 3210

    U008   xrVOffset;      // CR13 - VGA Offset register
    U008   xrIntlace;      // CR39 - Interlace Half Field Start
    U016  xrRegenLength;   // Override of PT_RegenLength (res+color depth specific)
    U008   xrClkSel;       // Pixel Clock Select

} EXTREGS;

typedef struct _tv_regs_
{
    U008   tvIntlace;              // CRTC[39] (Interlace Half Field Start)
    U008   tvDecimate_NTSC;        // CRTC[29] (Decimate for TV)
    U008   tvHT_NTSC;              // CRTC[00] (Horiztonal total for NTSC)
    U008   tvHRS_NTSC;             // CRTC[04] (Horizontal retrace start for NTSC)
    U008   tvVT_NTSC;              // CRTC[06] (Vertical total for NTSC)
    U008   tvVO_NTSC;              // CRTC[07] (Vertical overflow for NTSC)
    U008   tvVRS_NTSC;             // CRTC[10] (Vertical retrace start for NTSC)
    U008   tvHDE_NTSC;             // CRTC[01] (Horizontal Display End)
    U008   tvHBS_NTSC;             // CRTC[02] (Horizontal Blank Start)
    U008   tvHBE_NTSC;             // CRTC[03] (Horizontal Blank End)
    U008   tvHRE_NTSC;             // CRTC[05] (Horizontal Retrace End)
    U008   tvVRE_NTSC;             // CRTC[11] (Vertical Retrace End)
    U008   tvVDE_NTSC;             // CRTC[12] (Vertical Display End)
    U008   tvVBS_NTSC;             // CRTC[15] (Vertical Blank Start)
    U008   tvVBE_NTSC;             // CRTC[16] (Vertical Blank End)
    U008   tvDecimate_PAL;         // CRTC[29] (Decimate for TV)
    U008   tvHT_PAL;               // CRTC[00] (Horiztonal total for PAL)
    U008   tvHRS_PAL;              // CRTC[04] (Horizontal retrace start for PAL)
    U008   tvVT_PAL;               // CRTC[06] (Vertical total for PAL)
    U008   tvVO_PAL;               // CRTC[07] (Vertical overflow for PAL)
    U008   tvVRS_PAL;              // CRTC[10] (Vertical retrace start for PAL)
    U008   tvHDE_PAL;              // CRTC[01] (Horizontal Display End)
    U008   tvHBS_PAL;              // CRTC[02] (Horizontal Blank Start)
    U008   tvHBE_PAL;              // CRTC[03] (Horizontal Blank End)
    U008   tvHRE_PAL;              // CRTC[05] (Horizontal Retrace End)
    U008   tvVRE_PAL;              // CRTC[11] (Vertical Retrace End)
    U008   tvVDE_PAL;              // CRTC[12] (Vertical Display End)
    U008   tvVBS_PAL;              // CRTC[15] (Vertical Blank Start)
    U008   tvVBE_PAL;              // CRTC[16] (Vertical Blank End)

} TVREGS;

// CRTC Override Structure. This structure overrides the settings selected
// from the standard modeset mode table.
//
typedef struct _crtc_ovr_
{
    U008   columns;
    U008   rows;
    U008   char_height;
    U008   CO_Misc;                // Misc Register
    U008   CO_ClockMode;           // SR1
    U008   HTotal;                 // CR00
    U008   HDispEnd;               // CR01
    U008   HBlankS;                // CR02
    U008   HBlankE;                // CR03
    U008   HSyncS;                 // CR04
    U008   HSyncE;                 // CR05
    U008   VTotal;                 // CR06
    U008   Overflow;               // CR07
    U008   CO_CellHeight;          // CR09
    U008   CO_VSyncStart;          // CR10
    U008   VSyncE;                 // CR11
    U008   VDispE;                 // CR12
    U008   CO_VBlankStart;         // CR15
    U008   VBlandE;                // CR16

}CRTC_OVERRIDE;

//
// Fifo settings
//
typedef struct _modeset_fifo_
{
    U016  Freq;                   // Maximum frequency for these water marks
    U008   LowWaterMark64;         // Low water mark for 64 bit bus
    U008   BurstSize64;            // Burst size for 64 bit bus
    U008   LowWaterMark128;        // Low water mark for 128 bit bus
    U008   BurstSize128;           // Burst size for 128 bit bus

} MODESET_FIFO;

//
// CRTC access state (for dos compile)
//
typedef struct _save_astate_
{
    U016   crtc_addr;          // CRTC Address
    U032   nv_address;         // 32-bit NV address
    U008   access_bits;        // Access Control Bits
    U008   access_reg;         // Access Register (NVRM_ACCESS)
    U016   data_low;           // Low 32-bits of data register
} ACCESS_STATE;


//
// Modeset function prototypes
//
VOID NvPost(VOID);
U016 VBESetMode(U016, PGTF_TIMINGS,PDMT_OVERRIDES,U016);         // VESA SetMode
U016 VBESetRefresh(PGTF_TIMINGS);
MODEDESC *ValidateMode(U016 );    // Validate VESA Mode
U008 isTV(VOID);
U008 OEMValidateMode(MODEDESC *);
U016 OEMGetMemSize(VOID);
VOID SaveNVAccessState(ACCESS_STATE *);
VOID RestoreNVAccessState(ACCESS_STATE *);
U032 ReadPriv32(U032 );
U016 ReadIndexed(U032 , U008 );
VOID WriteIndexed(U032, U016);
VOID OEMPreSetRegs(VOID);
VOID SetRegs(MODEDESC *);
VOID LoadIndexRange(U032, U008, U008, U008 *);
VOID ATCOff(VOID);
VOID ATCOn(VOID);
VOID OEMSetRegs(MODEDESC *);
VOID FullCpuOn(VOID);
VOID FullCpuOff(VOID);
U032 CalcMNP(U016 *);
VOID OEMSetClock(U016 );
VOID WritePriv32(U032 , U032 );
VOID LoadDefaultATCAndSavePtr(U008 *);
VOID LoadDefaultRAMDAC(MODEDESC *);
VOID LoadPalette(MODEDESC *);
VOID LoadColorDACTable(VOID);
VOID LoadColor256DAC(VOID);
VOID LoadIdentityDAC(VOID);
VOID WriteColor(U008 );
VOID WriteColor2(U016 , U016 , U016 , U016 );
VOID FixLowWaterMark(U016 );
VOID SetDACClock(U032 );
VOID WritePriv08(U032 , U008 );
U016 h2i(U008 *);                // Hex 2 Integer

