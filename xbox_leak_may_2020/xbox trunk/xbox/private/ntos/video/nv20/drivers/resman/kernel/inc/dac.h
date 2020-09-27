#ifndef _DAC_H_
#define _DAC_H_
 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/
/********************************* DAC Module ******************************\
*                                                                           *
* Module: DAC.H                                                             *
*       DAC support routines.                                               *
*                                                                           *
\***************************************************************************/

#include <nvmisc.h>                 // pick up I2C structures

#include <nvcm.h>                   // ick ick ick ick

//
// Microport access macros.
//
#ifdef SOLARIS

// 3-6-96 With updated model simulation requires DAC initialization, so
//        we place back the DAC macros.
/*
#define DAC_WR08(rr,dd) 
#define DAC_INDEX_WR08(ii,dd) 
#define DAC_RD08(rr) \
    (REG_RD32(rr)&0xFF)
#define DAC_INDEX_RD08(ii,dd) \
    DAC_WR08(SGS_DAC_UPORT_INDEX_LO,(ii));   \
    DAC_WR08(SGS_DAC_UPORT_INDEX_HI,(ii)>>8);\
    (dd)=DAC_RD08(SGS_DAC_UPORT_INDEX_DATA)
*/

// Origional DAC Macros for RM.
#define DAC_WR08(rr,dd) \
    REG_WR32((rr),(dd)&0xFF)
#define DAC_INDEX_WR08(ii,dd) \
    DAC_WR08(SGS_DAC_UPORT_INDEX_LO,(ii));   \
    DAC_WR08(SGS_DAC_UPORT_INDEX_HI,(ii)>>8);\
    DAC_WR08(SGS_DAC_UPORT_INDEX_DATA,(dd))
#define DAC_RD08(rr) \
    (REG_RD32(rr)&0xFF)
#define DAC_INDEX_RD08(ii,dd) \
    DAC_WR08(SGS_DAC_UPORT_INDEX_LO,(ii));   \
    DAC_WR08(SGS_DAC_UPORT_INDEX_HI,(ii)>>8);\
    (dd)=DAC_RD08(SGS_DAC_UPORT_INDEX_DATA)


#else   // ifdef SOLARIS
#if 0
#define DAC_WR08(rr,dd) \
    REG_WR32((rr),(dd)&0xFF)
#endif
#define DAC_INDEX_WR08(ii,dd) \
    DAC_WR08(SGS_DAC_UPORT_INDEX_LO,(ii));   \
    DAC_WR08(SGS_DAC_UPORT_INDEX_HI,(ii)>>8);\
    DAC_WR08(SGS_DAC_UPORT_INDEX_DATA,(dd))
#if 0
#define DAC_RD08(rr) \
    (REG_RD32(rr)&0xFF)
#endif
#define DAC_INDEX_RD08(ii,dd) \
    DAC_WR08(SGS_DAC_UPORT_INDEX_LO,(ii));   \
    DAC_WR08(SGS_DAC_UPORT_INDEX_HI,(ii)>>8);\
    (dd)=DAC_RD08(SGS_DAC_UPORT_INDEX_DATA)
#endif // ifdef SOLARIS


/* XXX temporary until they appear in nv_def.h */
#define NV_CIO_CRE_DDC0_STATUS__INDEX                     0x00000036
#define NV_CIO_CRE_DDC0_WR__INDEX                         0x00000037

#define CRTC2   1

// These macros access the DAC (including PCRTC) for either of two heads
#define DAC_REG_WR08(a,d,h)         \
        REG_WR08(a + CurDacAdr(h), d)

#define DAC_REG_WR32(a,d,h)         \
        REG_WR32(a + CurDacAdr(h), d) 

#define DAC_REG_RD08(a,h)           \
        REG_RD08(a + CurDacAdr(h)) 

#define DAC_REG_RD32(a,h)           \
        REG_RD32(a + CurDacAdr(h)) 

// We need special versions of these macros for HAL use...
#define HAL_DAC_REG_WR32(a,d,o)         \
        REG_WR32(a + o, d) 

#define HAL_DAC_REG_RD32(a,o)           \
        REG_RD32(a + o) 

#define DAC_FLD_WR_DRF_DEF(d,r,f,c,h) \
        REG_WR32(NV##d##r + CurDacAdr(h),(REG_RD32(NV##d##r + CurDacAdr(h))&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))

// HAL Version
#define HAL_DAC_FLD_WR_DRF_DEF(d,r,f,c,o) \
        REG_WR32(NV##d##r + o,(REG_RD32(NV##d##r + o)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))

#define DAC_FLD_WR_DRF_NUM(d,r,f,n,h) \
        REG_WR32(NV##d##r + CurDacAdr(h),(REG_RD32(NV##d##r + CurDacAdr(h))&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n)) 

#define DAC_REG_WR_DRF_DEF(d,r,f,c,h) \
        REG_WR32(NV ## d ## r + CurDacAdr(h), DRF_DEF(d,r,f,c))

#define DAC_REG_RD_DRF(d,r,f,h) \
    ((REG_RD32(NV##d##r + CurDacAdr(h))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

// HAL Version
#define HAL_DAC_REG_RD_DRF(d,r,f,o) \
    ((REG_RD32(NV##d##r + o)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

//---------------------------------------------------------------------------
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


//  TVformat types
#define NTSC    0
#define PAL     1
//  TV encoders
#define NO_ENCODER 0
#define BROOKTREE  1
#define CHRONTEL   2
#define BROOKTREE_ID    0x8A
#define CHRONTEL_ID     0xEA

#define ERR_I2C_MODE    1
#define ERR_I2C_COMM    2
#define ERR_I2C_UNKNOWN 3
// Timing parameters, defined by CRTC containing low order bits
#define H_TOTAL         0
#define H_DISPLAY_END   1
#define H_BLANK_START   2
#define H_BLANK_END     3
#define H_RETRACE_START 4
#define H_RETRACE_END   5
#define V_TOTAL         6
#define V_DISPLAY_END   0x12
#define V_RETRACE_START 0x10
#define V_RETRACE_END   0x11
#define V_BLANK_START   0x15
#define V_BLANK_END     0x16

// Adjustments for TV
typedef struct
{
    U032    hRes;
    U032    vRes;
    U008    SR01;
    U008    CR00;
    U008    CR04;
    U008    CR06;
    U008    CR07;
    U008    CR08;
    U008    CR10;
    U008    CR12;
    U008    CR28;
    U008   *encoderString;
    U032    hTotalPM;
    U032    vTotalPM;
    U008    hDelayPM;
} TV_ADJUST;      

//typedef struct _I2C_string_struct_
typedef struct 
{
    U008    Dev;
    U008    reg0x00;
    U008    data0x00;
    U008    reg0x07;
    U008    data0x07;
    U008    reg0x0A;
    U008    data0x0A;
    U008    reg0x0B;
    U008    data0x0B;
    U008    reg0x13;
    U008    data0x13;
    U008    data0x14;
    U008    data0x15;
    U008    terminator;
} CH_string, *PCH_string;
    
    
//  Brooktree values -- 1 byte device ID, 6 bytes centering info, 33 bytes resolution info
typedef struct 
{
    U008    Dev;
    U008    btc1;
    U008    btc2;
    U008    btc3;
    U008    btc4;
    U008    btc5;
    U008    btc6;
    U008    bt1;
    U008    bt2;
    U008    bt3;
    U008    bt4;
    U008    bt5;
    U008    bt6;
    U008    bt7;
    U008    bt8;
    U008    bt9;
    U008    bt10;
    U008    bt11;
    U008    bt12;
    U008    bt13;
    U008    bt14;
    U008    bt15;
    U008    bt16;
    U008    bt17;
    U008    bt18;
    U008    bt19;
    U008    bt20;
    U008    bt21;
    U008    bt22;
    U008    bt23;
    U008    bt24;
    U008    bt25;
    U008    bt26;
    U008    bt27;
    U008    bt28;
    U008    bt29;
    U008    bt30;
    U008    bt31;
    U008    bt32;
    U008    bt33;
    U008    termination;
} BT_string, *PBT_string;

// DAC Flat Panel Timing struture
// Corresponds to HW regs which must be programmed for flat panel.
typedef struct DacFlatPanelTimings
{
    U032    PixClk;
    U032    HActive;
    U032    HDispEnd;
    U032    HValidStart;
    U032    HValidEnd;
    U032    HSyncStart;
    U032    HSyncEnd;
    U032    HTotal;
    U032    VActive;
    U032    VDispEnd;
    U032    VValidStart;
    U032    VValidEnd;
    U032    VSyncStart;
    U032    VSyncEnd;
    U032    VTotal;
    U032    Flags;
} DACFPTIMING, * PDACFPTIMING;

#define NV10_VSCALAR_TV_ADJUST  0x00010004  // adjustment for video overlay on TV

//
// Monitor position state.  Set during modeset and returned
// to clients via cfg get interfaces.
//
typedef struct _dac_monitor_position {
    U032 Hbs;
    U032 Hbe;
    U032 Vbs;
    U032 Vbe;
    U032 Hrs;
    U032 Hre;
    U032 Vrs;
    U032 Vre;
} DACMONITORPOSITION, *PDACMONITORPOSITION;

//
// Per-CRTC information.
// There is one of these structures for each possible CRTC in
// the system.
//
typedef struct _dac_crtc_info
{
    // Number of VIDEO_LUT_CURSOR_DAC objects allocated for this head
    U032 RefCount;

    VOID_PTR pVidLutCurDac; //Head-specific pointer, null if head is disabled.

#define DAC_CRTCINFO_VBLANK_ENABLED         0x0001
    U032 StateFlags;
    U032 UpdateFlags;

    // Panning/Buffer Flip data (used across LUT_CURSOR_DAC objects)
    U032 CurrentPanOffset;
    U032 CurrentImageOffset;

    // Display info
    U032 CurrentDisplay;
    U032 DisplayChanged;
    U032 CurrentVsyncState;
    U032 CurrentHsyncState;
    BOOL SyncStateIsSaved;
    U032 CurrentPowerState;
    U032 MonitorPowerSetMode;

    // VBlank data
    BOOL IsVBlank;
    U032 VBlankToggle;
    U032 VBlankCounter;
    VOID_PTR VBlankCallbackList;
    U032 VBlankTemporalDitherSafetyCounter; //this counter willl keep track of the number of vblanks since it was reset
    //the purpose is to make sure the machine enables dithering after interrupts are coming in fairly reliably.

    // NvSysMoveCursor data
    S032 CursorPosX;
    S032 CursorPosY;

    // NvSysSetCursorImage data
    U032 CursorWidth;
    U032 CursorHeight;
    U032 CursorOffset;
    U032 CursorColorFormat;

    // NvSysSetCursor data
    U008 CursorImagePlane[2][32 * sizeof(U032)];
    U032 CursorType;

    // NvSysUpdateImageFormat data
    U032 ImageOffset;
    U032 ImageFormat;
    U032 ChID;
    U032 ObjectHandle;

    // Monitor position data
    DACMONITORPOSITION MonitorPosition;

    // EDID data
    U008 EdidVersion;
    U008 EdidDisplayType;
    U008 EDID[256];

    U032    PrimaryDevice;
} DACCRTCINFO, *PDACCRTCINFO;

// defines for the BIOS Display Config Block
#define DCB_PTR_OFFSET          0x0036      // from VGA BIOS base

// following offset are relative to the DCB base
#define DCB_REVISION_OFFSET     0x0000
#define DCB_SIZE_128B_OFFSET    0x0001      // size is in 128 bytes block
#define DCB_ICB_OFFSET          0x0002      // I2C ports configuration block
#define DCB_DEV_DESC_OFFSET     0x0004

#define DCB_FIRST_REVISION      0x14        // revision in BCD format
                                            // 1st revision that supports DCB
#define DCB_BLOCK_SIZE          128

// each device descriptor consists of 2 32-bit words and a short pointer,
// the first 32-bit word is the device type record, the second one is the data record.

// TYPE field definitions
#define DCB_DEV_REC_TYPE_CRT                    0x0
#define DCB_DEV_REC_TYPE_TV                     0x1
#define DCB_DEV_REC_TYPE_DD_SLINK               0x2     // Single link digital display
#define DCB_DEV_REC_TYPE_DD_DLINK               0x3     // Dual link digital display
#define DCB_DEV_REC_TYPE_DD                     0x4     // Digital display
#define DCB_DEV_REC_TYPE_EOL                    0xf     // End of list
#define DCB_DEV_REC_TYPE_UNUSED                 0xf

// SUBTYPE field definitions
#define DCB_DEV_REC_SUBTYPE_UNUSED              0xf

// CRT subtypes
#define DCB_DEV_REC_SUBTYPE_CRT_DAC_INTERNAL    0x0
#define DCB_DEV_REC_SUBTYPE_CRT_DAC_FAIRCHILD   0x1
#define DCB_DEV_REC_SUBTYPE_CRT_UNUSED          DCB_DEV_REC_SUBTYPE_UNUSED

// TV subtypes
#define DCB_DEV_REC_SUBTYPE_TV_BROOKTREE        0x0
#define DCB_DEV_REC_SUBTYPE_TV_CHRONTEL         0x1
#define DCB_DEV_REC_SUBTYPE_TV_PHILIPS          0x2
#define DCB_DEV_REC_SUBTYPE_TV_UNUSED           DCB_DEV_REC_SUBTYPE_UNUSED

// digital display subtypes
#define DCB_DEV_REC_SUBTYPE_DD_TMDS             0x0
#define DCB_DEV_REC_SUBTYPE_DD_LVDS             0x1
#define DCB_DEV_REC_SUBTYPE_DD_UNUSED           DCB_DEV_REC_SUBTYPE_UNUSED

// FORMAT field definitions
#define DCB_DEV_REC_FMT_UNUSED                  0x3f

// CRT format definitions
#define DCB_DEV_REC_FMT_CRT_UNUSED              DCB_DEV_REC_FMT_UNUSED

// TV format definitions
#define DCB_DEV_REC_FMT_TV_NTSC_M               0x00
#define DCB_DEV_REC_FMT_TV_NTSC_J               0x01
#define DCB_DEV_REC_FMT_TV_PAL_M                0x02
#define DCB_DEV_REC_FMT_TV_PAL_BDGHI            0x03
#define DCB_DEV_REC_FMT_TV_PAL_N                0x04
#define DCB_DEV_REC_FMT_TV_PAL_NC               0x05
#define DCB_DEV_REC_FMT_TV_SECAM                0x06
#define DCB_DEV_REC_FMT_TV_STRAPS               0x0f
#define DCB_DEV_REC_FMT_TV_UNUSED               DCB_DEV_REC_FMT_UNUSED

// DFP format definitions
#define DCB_DEV_REC_FMT_DD_FIXED_00             0x00
#define DCB_DEV_REC_FMT_DD_FIXED_01             0x01
#define DCB_DEV_REC_FMT_DD_FIXED_02             0x02
#define DCB_DEV_REC_FMT_DD_FIXED_03             0x03
#define DCB_DEV_REC_FMT_DD_FIXED_04             0x04
#define DCB_DEV_REC_FMT_DD_FIXED_05             0x05
#define DCB_DEV_REC_FMT_DD_FIXED_06             0x06
#define DCB_DEV_REC_FMT_DD_FIXED_07             0x07
#define DCB_DEV_REC_FORMAT_DD_DDC               0x10
#define DCB_DEV_REC_FMT_DD_STRAPS               0x1f
#define DCB_DEV_REC_FMT_DD_UNUSED               DCB_DEV_REC_FMT_UNUSED

// I2C_PORT field definitions
#define DCB_DEV_REC_LOGICAL_PORT_0              0x0
#define DCB_DEV_REC_LOGICAL_PORT_1              0x1
#define DCB_DEV_REC_LOGICAL_PORT_2              0x2
#define DCB_DEV_REC_LOGICAL_PORT_UNUSED         0xf

// HEAD field definitions
#define DCB_DEV_REC_HEAD_A                      0x0
#define DCB_DEV_REC_HEAD_B                      0x1
#define DCB_DEV_REC_HEAD_UNUSED                 0x7

// LOCATION field definitions
#define DCB_DEV_REC_LOCATION_CHIP               0x0
#define DCB_DEV_REC_LOCATION_BOARD              0x1
#define DCB_DEV_REC_LOCATION_UNUSED             0xf

// BUS field definitions
#define DCB_DEV_REC_BUS_0                       0x0
#define DCB_DEV_REC_BUS_1                       0x1
#define DCB_DEV_REC_BUS_2                       0x2
#define DCB_DEV_REC_BUS_3                       0x3
#define DCB_DEV_REC_BUS_4                       0x4
#define DCB_DEV_REC_BUS_5                       0x5
#define DCB_DEV_REC_BUS_6                       0x6
#define DCB_DEV_REC_BUS_UNUSED                  0x7

// RSVD field definitions
#define DCB_DEV_REC_RSVD_UNUSED                 0xf

#define DCB_MAX_NUM_DEVDESCS    16      // max # of device descriptors and I2C
#define DCB_MAX_NUM_I2C_RECORDS 16      // records supported by current DCB

typedef struct {
    union {
        struct {
            U032    Type        :4;     // CRT, TV, DD
            U032    Subtype     :4;
            U032    Fmt         :6;
            U032    IdxToICB    :4;     // index into I2C Config Block
            U032    Head        :3;
            U032    Location    :4;     // chip, board
            U032    Bus         :3;
            U032    Rsvd        :4;
        } DevRec;

        U032 DevRec_U032;
    } DCBDevRec;

    U016    DataPtr;

    union {
        U032    DataReserved;
        U008    DataByte;
        U016    DataWord;
        U032    DataDword;
    } DCBDevData;

    // this var tells device type and unit by the lone set bit
    U032    DevTypeUnit;    // bits 0..7 map to CRT0..CRT7
                            // bits 8..15 map to TV0..TV7
                            // bits 16..23 map to DD0..DD7
} DACDCBDEVDESC, *PDACDCBDEVDESC;

// shortcuts for accessing DCB device descriptor field
#define DCBRecType        DCBDevRec.DevRec.Type
#define DCBRecSubtype     DCBDevRec.DevRec.Subtype
#define DCBRecFmt         DCBDevRec.DevRec.Fmt
#define DCBRecIdxToICB    DCBDevRec.DevRec.IdxToICB
#define DCBRecHead        DCBDevRec.DevRec.Head
#define DCBRecLocation    DCBDevRec.DevRec.Location
#define DCBRecBus         DCBDevRec.DevRec.Bus

#define I2C_PHYSICAL_PORT_A_STATUS         0x3E        // CR3E
#define I2C_PHYSICAL_PORT_A_DATA           0x3F        // CR3F
#define I2C_PHYSICAL_PORT_B_STATUS         0x36        // CR36
#define I2C_PHYSICAL_PORT_B_DATA           0x37        // CR37
#define I2C_PHYSICAL_PORT_C_STATUS         0x50        // CR50
#define I2C_PHYSICAL_PORT_C_DATA           0x51        // CR51
#define I2C_PHYSICAL_PORT_UNUSED           0xFF        // Unused I2C Port

#define I2C_PORT_UNUSED                    0xFF        // Port is not used

// i2c_access field - Access methods for I2C Port
#define I2C_PORT_ACCESS_UNUSED             0x07        // No access method defined
#define I2C_PORT_ACCESS_CRTC_INDEXED       0x00        // Indexed IO at 3x4/3x5
#define I2C_PORT_ACCESS_DIRECT_IO          0x01        // Direct IO (not indexed)
#define I2C_PORT_ACCESS_VIRTUAL            0x02        // Virtual IO

// i2c_rsvd field
#define I2C_PORT_RSVD_UNUSED               0x1F        // Unused
                                    
#define I2C_PORT_VIRTUAL_UNUSED            0xFF        // Unused
#define I2C_PORT_IOPORT_HI_UNUSED          0xFF        // Unused


typedef struct {
    U008    WritePort;
    U008    ReadPort;
    U016    IoPortHi    :8;
    U016    I2cAccess   :3;
    U016    Reserved    :5;

} DACDCBI2CRECORD, *pDACDCBI2CRECORD;

//
//  Function prototypes for Video DAC.
//
//---------------------------------------------------------------------------

RM_STATUS initDac(PHWINFO);
RM_STATUS dacCreateObj(VOID *, PCLASSOBJECT, U032, POBJECT *, VOID*);
RM_STATUS dacDestroyObj(VOID *, POBJECT);
RM_STATUS dacSetDpmLevel(PHWINFO);
RM_STATUS dacLoadReg(U032, U032, U032);
RM_STATUS dacLoadWidthDepth(PHWINFO, U032);
RM_STATUS dacCalcPLL(PHWINFO);
U032      dacCalcMNP(PHWINFO, U032, U032, U032 *, U032 *, U032 *);
U032      dacCalcMNP_MaxM(PHWINFO, U032, U032, U032 *, U032 *, U032 *, U032);
RM_STATUS dacLoadPClkVClkRatio(VOID);
RM_STATUS dacCalcPClkVClkRatio(VOID);
RM_STATUS dacGetDisplayInfo(PHWINFO, U032, U032 *, U032 *, U032 *, U032 *);
RM_STATUS dacProgramPClk(PHWINFO, U032, U032);
RM_STATUS dacProgramMClk(PHWINFO, U032);
RM_STATUS dacProgramNVClk(PHWINFO);
RM_STATUS dacVBlank(PHWINFO);
RM_STATUS dacUpdateEmulatedCursor(VOID);
RM_STATUS dacHideEmulatedCursor(VOID);
VOID      dacEnableCursor(PHWINFO, U032);
VOID      dacDisableCursor(PHWINFO, U032);
VOID      dacEnableDac(PHWINFO, U032);
VOID      dacDisableDac(PHWINFO, U032);
VOID      dacDisableImage(PHWINFO, U032);
VOID      dacEnableImage(PHWINFO, U032);
VOID      dacDisableTV(PHWINFO, U032);
VOID      dacEnableTV(PHWINFO, U032);
RM_STATUS dacProgramCursorImage(PHWINFO, U032, U032, U032, U032, U032);
RM_STATUS dacProgramCursorPosition(PHWINFO, U032, U032, U032);
RM_STATUS dacProgramVideoStart(PHWINFO, U032, U032, U032);
RM_STATUS dacProgramLUT(PHWINFO, U032, U032*, U032);
RM_STATUS dacProgramScanRegion(PHWINFO, U032, U032, U032, U032);
RM_STATUS dacSetMonitorPosition(PHWINFO, U032, U032, U032, U032, U032);
VOID      dacGetMonitorDefaultPosition(PHWINFO, U032, U032 *, U032 *, U032 *, 
                                       U032 *, U032 *, U032 *, U032 *, U032 *);
VOID      dacSaveMonitorDefaultPosition(PHWINFO, U032);
VOID      dacGetDefaultTVPosition(PHWINFO, U032, U032, U008, U032 *);
VOID      dacSetTVPosition(PHWINFO, U032, U032 *);
RM_STATUS dacSetDesktopPosition(PHWINFO, U032, U032);
RM_STATUS dacPositionTV(PHWINFO, U032, U032);
RM_STATUS dacWriteDesktopPositionToRegistry(PHWINFO, U032, BOOL, U008 *, U032);
RM_STATUS dacReadDesktopPositionFromRegistry(PHWINFO, U032, BOOL, U008 *, U032 *);
RM_STATUS dacReadTVDesktopPositionFromRegistry(PHWINFO, U032, U008 *, U032 *);
RM_STATUS dacReadTVOutFromRegistry(PHWINFO, U032, U032 *);
RM_STATUS dacWriteTVOutToRegistry(PHWINFO, U032, U032);
RM_STATUS dacReadTVPhilipsBFromRegistry(PHWINFO, U032, U032 *);
RM_STATUS dacReadMonitorSyncsFromRegistry(PHWINFO, U032 *);
RM_STATUS dacGetBiosDefaultTVType(PHWINFO, U032 *);
RM_STATUS dacSetBiosDefaultTVType(PHWINFO, U032);
VOID      dacTV871Readback(PHWINFO, U032, U008);
RM_STATUS dacSetBiosDefaultTVType(PHWINFO, U032);
BOOL       dacMonitorConnectStatus(PHWINFO, U032);
RM_STATUS  dacGetFlatPanelInfo(PHWINFO, U032, U032 *, U032 *, U032 *, BOOL *, BOOL *, BOOL);
RM_STATUS  dacSetFlatPanelMode(PHWINFO, U032, PVIDEO_LUT_CURSOR_DAC_OBJECT, U032, BOOL);
RM_STATUS  dacSetFlatPanelScaling(PHWINFO, U032, U032, BOOL);
RM_STATUS  dacSetFlatPanelScalingCMOS(PHWINFO, U032, U032, BOOL);
U032       dacGetFlatPanelConfig(PHWINFO, U032);
BOOL       dacFlatPanelConnectStatus(PHWINFO pDev, U032);
RM_STATUS  dacGetSyncState(PHWINFO, U032 Head, U032 *pHsync, U032 *pVsync);
RM_STATUS  dacSetSyncState(PHWINFO, U032 Head, U032 Hsync, U032 Vsync);
RM_STATUS  dacGetFlatPanelBrightness(PHWINFO, U032 Head, U032 * Brightness, BOOL * ControllerIsOn, BOOL * InverterCurrentMode);
RM_STATUS  dacSetFlatPanelBrightness(PHWINFO, U032 Head, U032   Brightness, U032 Frequency, U032 MaxPercent, U032 MinPercent,BOOL ControllerIsOn,BOOL InverterCurrentMode);
RM_STATUS  dacWriteLinkPLL(PHWINFO, U032, U032);
RM_STATUS dacProgramBrooktreeAuto(PHWINFO, U032);
RM_STATUS dacWriteTVStandardToRegistry(PHWINFO, U032);
RM_STATUS dacReadTVStandardFromRegistry(PHWINFO, U032 *);
RM_STATUS  dacGetCrcValues(PHWINFO, U032, U032 *, U032 *, U032 *);
RM_STATUS dacSetHotPlugInterrupts(PHWINFO, U032, U032, U032);
RM_STATUS dacGetHotPlugInterrupts(PHWINFO, U032, U032 *, U032 *, BOOL);
RM_STATUS initRmPVidLutCurDacStructs(PHWINFO pDev, U032 Head, PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac);
RM_STATUS initPVidLutCurDacStructs(PHWINFO pDev);
RM_STATUS ExtractLastModeset(PHWINFO pDev, U032 Head, PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pHalVidLutCurDac);

U008       dacTVReadReg(PHWINFO, U032, U008);

BOOL      dacRestoreBroadcast(PHWINFO, U032, U008);
BOOL      dacDisableBroadcast(PHWINFO, U032, U008 *);

//current control for external inverter connected to GPIO5 on P11 macintosh
void dacSetExternalInverterCurrentMode(PHWINFO pDev);
void dacGetExternalInverterCurrentMode(PHWINFO pDev);

//housekeeping PWM functions, reduces large spread of reg-rds to 3 closely spaced spots
void dacZeroPWMController(PHWINFO);
void dacSetPWMController(PHWINFO);
void dacGetPWMController(PHWINFO);

//an abstraction of flat panel operations
BOOL dacIsFlatPanelOn(PHWINFO pDev,U032 Head);

//some directly associated routines for adjusting brightness
U032 GetTicksForFrequency(U032 Frequency);
U032 GetRequiredTickCount(U032 Frequency,U032 MaxPercent,U032 MinPercent,U032 BaseMultiplier);
U032 GetPortionOfTotalSizeBasedOnPercentage(U032 TotalSize,U032 Percentage);
U032 DetermineFrequencyFromTickCount(U032 NumTicks);

//add gray code functionality for clock PDiv programming
void RmProperClockPdivProgrammer(PHWINFO pDev,U032 ClockAddress,U032 NewValue);

U032       dacGetCRTC(PHWINFO, U032, U032); 
VOID       dacSetCRTC(PHWINFO, U032, U032, U032);
RM_STATUS  dacGetCRTCTiming(PHWINFO, U032, NV_CFGEX_CRTC_TIMING_PARAMS *);
RM_STATUS  dacSetCRTCTiming(PHWINFO, U032, NV_CFGEX_CRTC_TIMING_PARAMS *);
RM_STATUS  dacSetMode(PHWINFO, U032);
RM_STATUS  dacSetSpanning(PHWINFO, U032);
RM_STATUS  dacSetModeStart(PHWINFO, U032);
RM_STATUS  dacSetModeEnd(PHWINFO, U032);
RM_STATUS  dacSetModeStartEx(PHWINFO, U032, U032);
RM_STATUS  dacSetModeEndEx(PHWINFO, U032, U032);
RM_STATUS  dacSetModeMulti(PHWINFO, PVIDEO_LUT_CURSOR_DAC_OBJECT);
RM_STATUS  dacSetModeMini(PHWINFO, PVIDEO_LUT_CURSOR_DAC_OBJECT);
RM_STATUS  dacSetModeFromObjectInfo(PHWINFO, PVIDEO_LUT_CURSOR_DAC_OBJECT);
RM_STATUS  dacSetupTVEncoder(PHWINFO, U032);
RM_STATUS  dacAdjustCRTCForTV(PHWINFO, U032);
RM_STATUS  dacAdjustCRTCForFlatPanel(PHWINFO, U032, PVIDEO_LUT_CURSOR_DAC_OBJECT);
VOID       dacTMDSWrite(PHWINFO, U032 Head, U008 Addr, U008 Data);
U008       dacTMDSRead(PHWINFO, U032 Head, U008 Addr);

VOID       hotkeyAddEvent(PHWINFO, U032, U032);
RM_STATUS  dacDetectDevices(PHWINFO, BOOL, BOOL);
U032       dacGetBiosDisplayType(PHWINFO, U032);
U032       dacGetBiosDisplayHead(PHWINFO);
BOOL       dacIsPAL(VOID);
U008       ReadCRTCLock(PHWINFO, U032);

RM_STATUS  dacGetMonitorInfo(PHWINFO, U032, U032*);
VOID       dacSetCursorBlink(PHWINFO, U032, U008);
VOID       dacSetFan(PHWINFO, U008);
RM_STATUS  dacGetColorSaturationBoost(PHWINFO, U032, U008*);
RM_STATUS  dacSetColorSaturationBoost(PHWINFO, U032, U008);
BOOL       dacTVConnectStatus(PHWINFO, U032);
U008       dacTVReadModifyWrite(PHWINFO, U032, U008, U008, U008);
//RM_STATUS  IsMonitorDDC(PHWINFO, U032, U032, U008 *);
RM_STATUS dacReadBIOSI2CSettings(PHWINFO pDev);
VOID dacDetectEncoder(PHWINFO, U032);
BOOL dacVGAConnectStatus(PHWINFO, U032);
RM_STATUS  dacPowerOnMobilePanel(PHWINFO, U032);
RM_STATUS  dacPowerOffMobilePanel(PHWINFO, U032);
RM_STATUS  dacBacklightOnMobilePanel(PHWINFO);
RM_STATUS  dacBacklightOffMobilePanel(PHWINFO);

RM_STATUS dacMobileDisplayNotifyBegin(PHWINFO, U032, U032);
RM_STATUS dacMobileDisplayNotifyEnd(PHWINFO, U032, U032);

// EXPER:
BOOL dacIsNV5orBetter(PHWINFO pDev);
RM_STATUS i2cAccess(PHWINFO, U032, NVRM_I2C_ACCESS_CONTROL *);
VOID RestoreLock(PHWINFO, U032, U008);
VOID SeqWr(PHWINFO, U032, U008, U008);
U008 SeqRd(PHWINFO, U032, U008);
VOID EnableSeq(PHWINFO, U032);
U008 UnlockCRTC();
VOID RestoreCRTC(U008);
U032 CurrentHeadDDC(PHWINFO);
VOID AssocDDC(PHWINFO, U032);
VOID AssocTV(PHWINFO, U032);
VOID AssocVideoScalar(PHWINFO, U032);
VOID EnableHead(PHWINFO, U032);
VOID EnableMobileHotkeyHandling(PHWINFO pDev);
VOID DisableMobileHotkeyHandling(PHWINFO pDev);
V032 dacService(PHWINFO);
RM_STATUS dacParseDCBFromBIOS(PHWINFO);
VOID dacDevicesConnectStatus(PHWINFO, U032 *, U032);
RM_STATUS dacFindDevicesConfiguration(PHWINFO, U032, U032 *, U032, U032);
void RmProperClockPdivProgrammer(PHWINFO pDev,U032 ClockAddress,U032 NewValue);
VOID dacProgramCRTC(PHWINFO, U032, PVIDEO_LUT_CURSOR_DAC_OBJECT, U032, U032, U032, U032, U032, U032, U032, U032, U032, U032, U032, 
                    U032, U032, U032, U032, U032, U032);
VOID dacDisableCRTCSlave(PHWINFO, U032); 
BOOL EDIDCheckSumOk(PHWINFO, U032, U008 *, U032, U008 *);
VOID dacPreInitEDID(PHWINFO pDev);
#endif // 
