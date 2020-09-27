 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  NVIDIA, Corp. of Sunnyvale,  California owns     *|
|*     copyrights, patents, and has design patents pending on the design     *|
|*     and  interface  of the NV chips.   Users and  possessors  of this     *|
|*     source code are hereby granted a nonexclusive, royalty-free copy-     *|
|*     right  and design patent license  to use this code  in individual     *|
|*     and commercial software.                                              *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright  1993-1998  NVIDIA,  Corporation.   NVIDIA  has  design     *|
|*     patents and patents pending in the U.S. and foreign countries.        *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF THIS SOURCE CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS" WITH-     *|
|*     OUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPORATION     *|
|*     DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOURCE CODE, INCLUD-     *|
|*     ING ALL IMPLIED WARRANTIES  OF MERCHANTABILITY  AND FITNESS FOR A     *|
|*     PARTICULAR  PURPOSE.  IN NO EVENT  SHALL NVIDIA,  CORPORATION  BE     *|
|*     LIABLE FOR ANY SPECIAL,  INDIRECT,  INCIDENTAL,  OR CONSEQUENTIAL     *|
|*     DAMAGES, OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,     *|
|*     DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR     *|
|*     OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION  WITH THE     *|
|*     USE OR PERFORMANCE OF THIS SOURCE CODE.                               *|
|*                                                                           *|
|*     RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the     *|
|*     Government is subject  to restrictions  as set forth  in subpara-     *|
|*     graph (c) (1) (ii) of the Rights  in Technical Data  and Computer     *|
|*     Software  clause  at DFARS  52.227-7013 and in similar clauses in     *|
|*     the FAR and NASA FAR Supplement.                                      *|
|*                                                                           *|
 \***************************************************************************/

/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NVCM.H                                                            *
*   Windows Configuration Manager defines and prototypes.                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
*                                                                           *
\***************************************************************************/
#ifndef _NVCM_H_
#define _NVCM_H_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
//
//  Configuration Defines.
//
//---------------------------------------------------------------------------
#pragma pack(1) // assure byte alignment on structures shared among modules

//
// Index parameters to ConfigGet/Set.  All other values are reserved.
//

//
// NV_CFG_MANUFACTURER - Return the manufacturer of this device
//
//  0 - NVIDIA Corporation
//  (all other values are reserved)
//
#define NV_CFG_MANUFACTURER                     1

//
// NV_CFG_ARCHITECTURE - Return the architecture of this device
//
//  0 - NV0
//  1 - NV1, etc
//
#define NV_CFG_ARCHITECTURE                     2

//
// NV_CFG_REVISION - Return the major revision of this device
//
//  0 - RevA
//  1 - RevB, etc
//
#define NV_CFG_REVISION                         3

/*
** NV_CFG_BIOS_REVISION - Return the current revision of this device's BIOS
**
**  MMNNRRRR - MM   Major Revision
**             NN   Minor Revision
**             RRRR Release Revision
*/
#define NV_CFG_BIOS_REVISION                    4

//
// NV_CFG_BUS_TYPE - Return the bus implementation of this device
//
//  1 - PCI
//  2 - VL
//  4 - AGP
//  All other values are reserved
//
#define NV_CFG_BUS_TYPE                         5

//
// NV_CFG_CRYSTAL_FREQ - Return the PLL crystal frequency of this device
//
//  Value returned in Hz
//
#define NV_CFG_CRYSTAL_FREQ                     6

//
// NV_CFG_BIOS_OEM_REVISION - Return the current OEM revision of this
//                            device's BIOS
//
#define NV_CFG_BIOS_OEM_REVISION                7

//
// NV_CFG_IMPLEMENTATION - Return the implementation of a chip architecture.
//
#define NV_CFG_IMPLEMENTATION                   8

// When ARCHITECTURE is 0x4, IMPLEMENTATIION is:
#define NV_CFG_IMPLEMENTATION_NV04      0x0
#define NV_CFG_IMPLEMENTATION_NV05      0x1
#define NV_CFG_IMPLEMENTATION_NV0A      0x2
// When ARCHITECTURE is 0x10, IMPLEMENTATIION is:
#define NV_CFG_IMPLEMENTATION_NV10      0x0
#define NV_CFG_IMPLEMENTATION_NV15      0x5
#define NV_CFG_IMPLEMENTATION_NV11      0x1

//
// NV_CFG_ADDRESS_NVADDR - Return the linear address of (BAR0) of this device
//
#define NV_CFG_ADDRESS_NVADDR                   9

//
// NV_CFG_ADDRESS - Return the physical PCI address (BAR0) of this device
//
#define NV_CFG_ADDRESS                          10

//
// NV_CFG_IRQ - Return the PCI IRQ assigned to this device
//
#define NV_CFG_IRQ                              11

//
// NV_CFG_IO_NEEDED - Return the detection flag for chipsets that have broken
//                    PCI snoop mastering.
//
#define NV_CFG_IO_NEEDED                        12

//
// NV_CFG_PCI_ID - Return the PCI Vendor and Device ID assigned to this device
//
//      DDDDVVVV -  VVVV    PCI Vendor ID
//                  DDDD    PCI Device ID
//
#define NV_CFG_PCI_ID                           13

//
// NV_CFG_PCI_SUB_ID - Return the PCI Subsystem Vendor and Device ID assigned to this device
//
//      DDDDVVVV -  VVVV    PCI Subsystem Vendor ID
//                  DDDD    PCI Subsystem ID
//
#define NV_CFG_PCI_SUB_ID                       14

//
// NV_CFG_PROCESSOR_TYPE - Return the functionality of the system processor
//
// There are 2 pieces of data passed back, the processor indicator is in
// the low 8 bits and its bitmask of functionality is in the upper 24
//
#define NV_CPU_UNKNOWN         0x00000000    // Unknown / generic
// Intel
#define NV_CPU_P5              0x00000001
#define NV_CPU_P55             0x00000002    // P55C - MMX
#define NV_CPU_P6              0x00000003    // PPro
#define NV_CPU_P2              0x00000004    // PentiumII
#define NV_CPU_P2XC            0x00000005    // Xeon & Celeron
#define NV_CPU_CELA            0x00000006    // Celeron-A
#define NV_CPU_P3              0x00000007    // Pentium-III
#define NV_CPU_P3_INTL2        0x00000008    // Pentium-III w/ integrated L2 (fullspeed, on die, 256K)
// AMD
#define NV_CPU_K5              0x00000030
#define NV_CPU_K6              0x00000031
#define NV_CPU_K62             0x00000032    // K6-2 w/ 3DNow
#define NV_CPU_K63             0x00000033
#define NV_CPU_K7              0x00000034
// IDT/Centaur
#define NV_CPU_C6              0x00000060    // WinChip C6
#define NV_CPU_C62             0x00000061    // WinChip 2 w/ 3DNow
// Cyrix
#define NV_CPU_GX              0x00000070    // MediaGX
#define NV_CPU_M1              0x00000071    // 6x86
#define NV_CPU_M2              0x00000072    // M2
#define NV_CPU_MGX             0x00000073    // MediaGX w/ MMX
// PowerPC
#define NV_CPU_PPC603          0x00000090    // PowerPC 603
#define NV_CPU_PPC604          0x00000091    // PowerPC 604
#define NV_CPU_PPC750          0x00000092    // PowerPC 750

// Function bits
#define NV_CPU_FUNC_MMX        0x00000100    // supports MMX
#define NV_CPU_FUNC_SSE        0x00000200    // supports SSE
#define NV_CPU_FUNC_3DNOW      0x00000400    // supports 3DNow
#define NV_CPU_FUNC_SSE2       0x00000800    // supports SSE2
#define NV_CPU_FUNC_SFENCE     0x00001000    // supports SFENCE

#define NV_CFG_PROCESSOR_TYPE                   15

//
// NV_CFG_PROCESSOR_SPEED - Return the speed of the processor in MHz
//
#define NV_CFG_PROCESSOR_SPEED                  16

//
// NV_CFG_GRAPHICS_CAPS - Return the capabilities of the graphics HW
//
#define NV_CFG_GRAPHICS_CAPS                    18

#define NV_CFG_GRAPHICS_CAPS_UNKNOWN            0x00000000
#define NV_CFG_GRAPHICS_CAPS_MAXCLIPS_MASK      0x000000FF  // bits 7:0
#define NV_CFG_GRAPHICS_CAPS_MAXCLIPS_SHIFT     0
#define NV_CFG_GRAPHICS_CAPS_AA_LINES           0x00000100  // bit  8
#define NV_CFG_GRAPHICS_CAPS_AA_POLYS           0x00000200  // bit  9
#define NV_CFG_GRAPHICS_CAPS_AKILL_USERCLIP     0x00000400  // bit 10
#define NV_CFG_GRAPHICS_CAPS_LOGIC_OPS          0x00000800  // bit 11
#define NV_CFG_GRAPHICS_CAPS_NV15_ALINES        0x00001000  // bit 12
#define NV_CFG_GRAPHICS_CAPS_2SIDED_LIGHTING    0x00002000  // bit 13
#define NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC     0x00004000  // bit 14

//
// NV_CFG_INSTANCE_SIZE - Return the size of the instance pool of this device
//
#define NV_CFG_INSTANCE_SIZE                    20

//
// NV_CFG_INSTANCE_SIZE - Return the size of the instance pool of this device
//
#define NV_CFG_INSTANCE_TOTAL                   21

//
// NV_CFG_FIFO_COUNT - Return the number of user fifos allowed in this device
//
#define NV_CFG_FIFO_COUNT                       32

//
// NV_CFG_FIFO_USE_COUNT - Return the number of fifos used in this device
//
#define NV_CFG_FIFO_USE_COUNT                   33

//
// NV_CFG_RAM_SIZE_MB - Return the video memory size (in MB) of this device
//
#define NV_CFG_RAM_SIZE_MB                      40

//
// NV_CFG_RAM_SIZE - Return the video memory size (in bytes) of this device
//
#define NV_CFG_RAM_SIZE                         41

//
// NV_CFG_RAM_TYPE - Return the video memory type of this device
//
//  0 - DRAM (EDO/FPM)
//  1 - SGRAM
//  2 - SDRAM
//  3 - DDR-RAM
//  All other values are reserved
//
#define NV_CFG_RAM_TYPE                         42

#define NV_CFG_RAM_TYPE_DRAM          0
#define NV_CFG_RAM_TYPE_SGRAM         1
#define NV_CFG_RAM_TYPE_SDRAM         2
#define NV_CFG_RAM_TYPE_DDRRAM        3


//
// NV_CFG_AGP_FULLSPEED_FW - Return if AGP Fast Writes are fullspeed
//
#define NV_CFG_AGP_FULLSPEED_FW                 43

//
// NV_CFG_AGP_PHYS_BASE - Return/Set the current physical GART address
//
#define NV_CFG_AGP_PHYS_BASE                    44

//
// NV_CFG_AGP_LINEAR_BASE - Return/Set the current linear GART address
//
#define NV_CFG_AGP_LINEAR_BASE                  45

//
// NV_CFG_AGP_LIMIT - Return/Set the current GART size
//
#define NV_CFG_AGP_LIMIT                        46

//
// NV_CFG_AGP_FW_ENABLE - Return if AGP Fast Writes are enabled
//
#define NV_CFG_AGP_FW_ENABLE                    47

//
// NV_CFG_AGP_HEAP_FREE - Return the amount of free AGP memory
//
#define NV_CFG_AGP_HEAP_FREE                    48

// XXX this isn't be used
#define NV_CFG_AGP_LOAD_DUMMY_PAGES             49

//
// NV_CFG_SCREEN_WIDTH - Return/Set the current display horizontal resolution
//
//  Value defined in pixels
//
// NOTE: Updates to this value will not take affect until NvConfigUpdate().
// This allows batching with other screen size parameters.
//
#define NV_CFG_SCREEN_WIDTH                     50

//
// NV_CFG_SCREEN_HEIGHT - Return/Set the current display vertical resolution
//
//  Value defined in pixels
//
// NOTE: Updates to this value will not take affect until NvConfigUpdate().
// This allows batching with other screen size parameters.
//
#define NV_CFG_SCREEN_HEIGHT                    51

//
// NV_CFG_PIXEL_DEPTH - Return/Set the current display pixel depth
//
//  Value defined in bits per pixel
//
// NOTE: Updates to this value will not take affect until NvConfigUpdate().
// This allows batching with other screen size parameters.
//
#define NV_CFG_PIXEL_DEPTH                      52

//
// NV_CFG_SCREEN_MAX_WIDTH - Returns the maximum horizontal resolution supported by the display type.
//
//  Value defined in pixels
//
#define NV_CFG_SCREEN_MAX_WIDTH                 53

//
// NV_CFG_SCREEN_MAX_HEIGHT - Returns the maximum vertical resolution supported by the display type.
//
//  Value defined in pixels
//
#define NV_CFG_SCREEN_MAX_HEIGHT                54

//
// NV_CFG_VIDEO_ENCODER_TYPE - Return the external video decoder being used by this device
//
#define NV_CFG_VIDEO_ENCODER_TYPE               55

#define TV_ENCODER_NONE                 0x000
#define TV_ENCODER_BROOKTREE            0x100
#define TV_ENCODER_CHRONTEL             0x200
#define TV_ENCODER_PHILIPS              0x400

#define TV_ENCODER_FAMILY( X )          (X & ~0xFF)

#define NV_ENCODER_NONE                 TV_ENCODER_NONE

#define NV_ENCODER_BROOKTREE_868        (TV_ENCODER_BROOKTREE + 1)
#define NV_ENCODER_BROOKTREE_869        (TV_ENCODER_BROOKTREE + 2)
#define NV_ENCODER_BROOKTREE_871        (TV_ENCODER_BROOKTREE + 3)

#define NV_ENCODER_CHRONTEL_7003        (TV_ENCODER_CHRONTEL + 3)
#define NV_ENCODER_CHRONTEL_7004        (TV_ENCODER_CHRONTEL + 4)
#define NV_ENCODER_CHRONTEL_7005        (TV_ENCODER_CHRONTEL + 5)
#define NV_ENCODER_CHRONTEL_7006        (TV_ENCODER_CHRONTEL + 6)
#define NV_ENCODER_CHRONTEL_7007        (TV_ENCODER_CHRONTEL + 7)
#define NV_ENCODER_CHRONTEL_7008        (TV_ENCODER_CHRONTEL + 8)

#define NV_ENCODER_PHILIPS_7102	        (TV_ENCODER_PHILIPS + 2)
#define NV_ENCODER_PHILIPS_7103	        (TV_ENCODER_PHILIPS + 3)
#define NV_ENCODER_PHILIPS_7108	        (TV_ENCODER_PHILIPS + 8)    // = 7102 encoder + 7114 decoder
#define NV_ENCODER_PHILIPS_7109	        (TV_ENCODER_PHILIPS + 9)    // = 7103 encoder + 7114 decoder
#define NV_ENCODER_PHILIPS_7108B        (TV_ENCODER_PHILIPS + 8 + 0x10)    // early version of 7108 with clock active edge reversed
//
// NV_CFG_PRIMARY_SURFACE_PITCH - Return the display pitch of the primary surface
//
//  Value defined in bytes
//
#define NV_CFG_PRIMARY_SURFACE_PITCH            56

//
// NV_CFG_VIDEO_ENCODER_ID - Return the encoder ID (I2C address)
//
#define NV_CFG_VIDEO_ENCODER_ID                 57

//
// NV_CFG_NUMBER_OF_HEADS - Return the number of CRTC heads on this device
//
#define NV_CFG_NUMBER_OF_HEADS                  58

//
// NV_CFG_DEVICE_INSTANCE - Return the device instance
//
#define NV_CFG_DEVICE_INSTANCE                  59

//
// NV_CFG_NUMBER_OF_ACTIVE_HEADS - Return number of active CRTC heads on this device
//
#define NV_CFG_NUMBER_OF_ACTIVE_HEADS           60

//
// NV_CFG_INITIAL_BOOT_HEAD - Return the dac head that was used to boot the system
//
//  0 - HeadA
//  1 - HeadB
//  All other values are reserved
//
#define NV_CFG_INITIAL_BOOT_HEAD                61

//
// NV_CFG_VIDEO_DISPLAY_TYPE - Return the physical display being used by this device
//
//  0 - Monitor
//  1 - TV
//  2 - Dual Surface
//  3 - Flat Panel
//  All other values are reserved
//
#define NV_CFG_VIDEO_DISPLAY_TYPE               78

//
// NV_CFG_VIDEO_MONITOR_TYPE - Return the monitor format type being used by this device
//
//  0 - VGA
//  2 - NTSC TV
//  3 - PAL TV
//  4 - Flat Panel
//  All other values are reserved
//
#define NV_CFG_VIDEO_MONITOR_TYPE               79

#define MONITOR_TYPE_VGA                0
#define MONITOR_TYPE_NTSC               2
#define MONITOR_TYPE_PAL                3
#define MONITOR_TYPE_FLAT_PANEL         4

//
// NV_CFG_VIDEO_OVERLAY_ALLOWED - Return the video overlay validity in the current
//      desktop resolution
//
//  0 - Video Overlay not allowed
//  1 - Video Overlay is allowed
//  All other values are reserved
//
#define NV_CFG_VIDEO_OVERLAY_ALLOWED            80

//
// NV_CFG_VIDEO_DOWNSCALE_RATIO_X - Return the maximum video overlay downscale ratio
// supported for the given source width in the current desktop resolution
// (assuming the overlay is supported at all -- see above)
//
//  1  - Video Downscaling not supported with this source width
//  2  - Maximum 2X downscaling supported
//  4  - Maximum 4X downscaling supported
//  8  - Maximum 8X downscaling supported
//  16 - Maximum 16X downscaling supported
//  (and so on)
//
#define NV_CFG_VIDEO_DOWNSCALE_RATIO_768        82
#define NV_CFG_VIDEO_DOWNSCALE_RATIO_1280       83
#define NV_CFG_VIDEO_DOWNSCALE_RATIO_1920       84

//
// NV_CFG_FRAMEBUFFER_UNDERSCAN - Return/Set the NV3 (RIVA128) TV Underscan resolution
//
#define NV_CFG_FRAMEBUFFER_UNDERSCAN_X          85
#define NV_CFG_FRAMEBUFFER_UNDERSCAN_Y          86

//
// NV_CFG_VIDEO_OUTPUT_FORMAT - Return/Set the NV4/5/10 TV output format (S-Video, Composite, or Auto)
//
//  0 = Auto
//  1 = Composite
//  2 = S-Video 
//
#define NV_CFG_VIDEO_OUTPUT_FORMAT              90

#define NV_CFG_VIDEO_OUTPUT_FORMAT_AUTOSELECT          0x0
#define NV_CFG_VIDEO_OUTPUT_FORMAT_FORCE_COMPOSITE     0x1
#define NV_CFG_VIDEO_OUTPUT_FORMAT_FORCE_SVIDEO        0x2

//
// NV_CFG_VIDEO_TV_FORMAT - Return the current TV format
//
#define NV_CFG_VIDEO_TV_FORMAT                  109
// must be one of the following formats
#define NTSC_M  0
#define NTSC_J  1
#define PAL_M   2
#define PAL_A   3   // PAL B, D, G, H, I
#define PAL_N   4
#define PAL_NC  5
#define KNOWN_TV_FORMATS 5 // number of formats we know about + 1

//
// NV_CFG_DAC_INPUT_WIDTH - Return the width (in bits) of the current framebuffer bus
//
#define NV_CFG_DAC_INPUT_WIDTH                  110

//
// NV_CFG_DAC_VPLL_*
// NV_CFG_DAC_MPLL_* - Return the individual components of the memory and video PLL's
//
//  The definition of these values is reserved
//
#define NV_CFG_DAC_VPLL_M                       120
#define NV_CFG_DAC_VPLL_N                       121
#define NV_CFG_DAC_VPLL_O                       122
#define NV_CFG_DAC_VPLL_P                       123
#define NV_CFG_DAC_MPLL_M                       130
#define NV_CFG_DAC_MPLL_N                       131
#define NV_CFG_DAC_MPLL_O                       132
#define NV_CFG_DAC_MPLL_P                       133

//
// NV_CFG_BIOS_DEFAULT_TV_TYPE - The TV standard the BIOS uses by default.
//
//  The allowed values are defined as the values for NV_CFG_VIDEO_TV_FORMAT
//
#define NV_CFG_BIOS_DEFAULT_TV_TYPE             134

//
// NV_CFG_INTERRUPTS_ENABLED_STATE - Return / set NV interrupts enable
//
//  Return value of "get" form is 0 or 1.
//  When enabling interrupts, restores them to RM's idea of the current value
//
#define NV_CFG_INTERRUPTS_ENABLED_STATE         140

//
// NV_CFG_VIDEO_REFRESH_RATE - Return/Set the current display refresh rate (in Hz)
//
#define NV_CFG_VIDEO_REFRESH_RATE               170

//
// NV_CFG_VBLANK_TOGGLE - Return/Set the current value of the vertical sync toggle
//
//  This value is toggled to zero (0) during every vertical sync.  By setting it to
//  any other value and then monitoring, a client can detect the presence of a
//  vertical sync signal occuring within a defined time period.
//
#define NV_CFG_VBLANK_TOGGLE                    212

//
// NV_CFG_VBLANK_COUNTER - Return/Set the number of vertical blanks 
//
//  This value is initialized to zero and keeps a running count of the number of
//  vertical blanks since the last reset. Use ConfigSet to reset it to zero.
//
#define NV_CFG_VBLANK_COUNTER                   213

//
// NV_CFG_PRAM_CURRENT_SIZE - Return the currently allocated size of chip
//
#define NV_CFG_PRAM_CURRENT_SIZE                220

//
// NV_CFG_GET_ALL_DEVICES - Return all available display devices (connectors)
//  in bitmask form.
// bits 0..7 represent CRT0..CRT7
// bits 8..15 represent TV0..TV7
// bits 16..23 represent DD0..DD7
// bits 24..31 are reserved
//
#define NV_CFG_GET_ALL_DEVICES                  221

//Same format as NV_CFG_GET_ALL_DEVICES
#define NV_CFG_GET_BOOT_DEVICES                 222

//
// NV_CFG_DAC_PIXEL_CLOCK - Return the current video pixel clock (in Hz) for this device
//
#define NV_CFG_DAC_PIXEL_CLOCK                  250

//                                                              
// NV_CFG_DAC_MEMORY_CLOCK - Return the current memory clock (in Hz) for this device
//
#define NV_CFG_DAC_MEMORY_CLOCK                 251

//
// NV_CFG_DAC_GRAPHICS_CLOCK - Return the current graphics clock (in Hz) for this device
//
#define NV_CFG_DAC_GRAPHICS_CLOCK               253

//
// NV_CFG_DAC_PCLK_LIMIT_8BPP - Return the current 8bpp graphics clock limit (in Hz) for this device
//
#define NV_CFG_DAC_PCLK_LIMIT_8BPP              260

//
// NV_CFG_DAC_PCLK_LIMIT_16BPP - Return the current 16bpp graphics clock limit (in Hz) for this device
//
#define NV_CFG_DAC_PCLK_LIMIT_16BPP             261

//
// NV_CFG_DAC_PCLK_LIMIT_32BPP - Return the current 16bpp graphics clock limit (in Hz) for this device
//
#define NV_CFG_DAC_PCLK_LIMIT_32BPP             262

//
// NV_CFG_MAPPING_BUSDEVICEFUNC - Return the device type for use by the display driver configmgr code
//
//  This value is the device type or'd with the busdevicefunc info (9X specific)
//
#define NV_CFG_MAPPING_BUSDEVICEFUNC            263
              
//
// NV_CFG_VBLANK_STATUS - Return the current vertical blank status of this device
//
//  0 - device is not currently in vertical blank
//  1 - device is currently in vertical blank
//
#define NV_CFG_VBLANK_STATUS                    500

//
// NV_CFG_GE_STATUS - Return the current graphics activity state of this device
//
//  any value other than zero (0) denotes graphics activity is in progress
//
#define NV_CFG_GE_STATUS                        501

//
// NV_CFG_CURRENT_SCANLINE - Return the current active display scanline of this device
//
//  The value returned may be greater than the current resolution if the display
//  has moved into the vertical blanking period.
//
#define NV_CFG_CURRENT_SCANLINE                 502

//
// NV_CFG_FRAMECAPTURE_CALLBACK - Set the current 16bit callback (VFW) for framecapture
//
//  The value is specifically OS-dependent.  This setting may only be used with ConfigSet
//  and only on those operating systems requiring the callback mechanism.
//
#define NV_CFG_FRAMECAPTURE_CALLBACK            600

// Get the device handle for the device.
// Currently only defined for unix where it returns file descriptor
// for the opened device.  Needed for poll(2) on events

#define NV_CFG_DEVICE_HANDLE                    650

//
// NV_CFG_CRTC_* - Return/Set a CRTC (VGA) register in this device
//
//  All CRTC registers are accessed by adding the index the base value 
//  given here.
//  For example, CR1B is accessed using index (NV_CFG_CRTC+0x1B)
//
#define NV_CFG_CRTC                             768
//
// NOTE: CRTC RESERVED TO INDEX 832
//

//
// The following values are reserved for use by the resource manager 
// performance and debug tools
//
#define NV_CFG_SET_JUMP_WRITE_PUT_READ_GET      833
#define NV_STAT_INTERVAL_NSEC_LO                1000
#define NV_STAT_INTERVAL_NSEC_HI                1001
#define NV_STAT_FIFO_EXCEPTIONS                 1003
#define NV_STAT_FRAMEBUFFER_EXCEPTIONS          1004
#define NV_STAT_GRAPHICS_EXCEPTIONS             1005
#define NV_STAT_TIMER_EXCEPTIONS                1006
#define NV_STAT_CHIP_EXCEPTIONS                 1050
#define NV_STAT_MEM_ALLOCATED                   1100
#define NV_STAT_MEM_LOCKED                      1101
#define NV_STAT_EXEC_NSEC_LO                    1200
#define NV_STAT_EXEC_NSEC_HI                    1201
#define NV_STAT_INT_NSEC_LO                     1202
#define NV_STAT_INT_NSEC_HI                     1203

#define NV_CFG_REGISTRY_STRING                  1300

#define NV_CFG_DEBUG_LEVEL                      1350


//////////////////////////////////////////////////////////////////////////////
//
// The following Config equates are a 16bit subset of the corresponding 
// ConfigEX values
//
// Where possible, make sure to use the 32bit versions of these DDK interfaces
// rather than these legacy equates
//

//
// NV_CFG_GET_FLAT_PANEL_CONNECT_16 - Return the connection status of a 
// digital flat panel
// 
#define NV_CFG_FLAT_PANEL_CONNECT_16            30100

//
// NV_CFG_GET_FLAT_PANEL_CONFIG_16 - Return or update the current format 
// configuration of the active digital flat panel
//
//  0 - scaled
//  1 - centered
//  2 - native
// 
#define NV_CFG_FLAT_PANEL_CONFIG_16             30102

//
// NV_CFG_STEREO_CONFIG - Stereo support Config Parameters
//
//  The subcodes within this entry are NV reserved
//
#ifdef  STEREO_SUPPORT
#define NV_CFG_STEREO_CONFIG                    40000
#endif  //STEREO_SUPPORT

//
// Parameter to ConfigUpdate.
//
#define NV_CFG_PERMANENT                        1
#define NV_CFG_TEMPORARY                        2


////////////////////////////////////////////////////////////////////
//
// The following Config equates are to be used with the ConfigGetEx()
// and ConfigSetEx() functions.  Rather than just taking a DWORD
// data value, they allow the use of a per-function data structure
// to be used as a parameter block.  
//
// Be careful to only use these equates with the appropriate
// functions.
//

//
// NV_CFGEX_GET_DESKTOP_POSITION_MONITOR - Return the monitor alignment
//
#define NV_CFGEX_GET_DESKTOP_POSITION_MONITOR              100
// Param Structure
typedef struct {
    unsigned long  GetDefault;
    unsigned long  HBlankStart;
    unsigned long  HBlankEnd;
    unsigned long  VBlankStart;
    unsigned long  VBlankEnd;
    unsigned long  HRetraceStart;
    unsigned long  HRetraceEnd;
    unsigned long  VRetraceStart;
    unsigned long  VRetraceEnd;
} NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_PARAMS;

//
// NV_CFGEX_SET_DESKTOP_POSITION_MONITOR - Set the new monitor alignment
//
#define NV_CFGEX_SET_DESKTOP_POSITION_MONITOR               101
// Param Structure
typedef struct {
    unsigned long  CommitChanges;
    unsigned long  HRetraceStart;
    unsigned long  HRetraceEnd;
    unsigned long  VRetraceStart;
    unsigned long  VRetraceEnd;
} NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_PARAMS;

/************** Desktop position for TV ***********/
#define NV_CFGEX_GET_DESKTOP_POSITION_TV                    102
#define NV_CFGEX_COMMIT_DESKTOP_POSITION_TV                 103

typedef struct {
    unsigned long  HRetraceStart;
    unsigned long  HRetraceEnd;
    unsigned long  VRetraceStart;
    unsigned long  VRetraceEnd;
    unsigned char  reg0x80;
    unsigned char  reg0x82;
    unsigned char  reg0x92;
    unsigned char  reg0x98;
    unsigned char  reg0x9A;
    unsigned char  reg0xC8;
    unsigned char  reg0xCA;
    unsigned char  reg0xCC;
} PARAMS_BROOKTREE;

// Register values for flicker filter. Chroma FF is 5:3, Lum FF is 2:0. Control Panel has 4 settings, other values result in OFF.
#define BT_FLICKER_HI   0    // Max FF
#define BT_FLICKER_MED  0x12
#define BT_FLICKER_LO   0x09
#define BT_FLICKER_OFF  0x1b // Place holder in CP.
#define BT_FLICKER_DEF  BT_FLICKER_MED // RM default
// Register values for Luma (Brightness)
#define BT_LUMA_MAX     0xC0  // No attenuation
#define BT_LUMA_DEF     0xC3  // RM default  
#define BT871_LUMA_DEF  0xC4  // RM default  
#define BT_LUMA_MIN     0xC7
// Register values for Chroma (Saturation)
#define BT_CHROMA_MAX   0xC0
#define BT_CHROMA_DEF   0xC2  // RM default
#define BT871_CHROMA_DEF 0xC1  // RM default
#define BT_CHROMA_MIN   0xC7

typedef struct {
    unsigned char  reg0x01; // Flicker Filter
    unsigned char  reg0x08; // overflow bits for V & H position
    unsigned char  reg0x0A; // Horizontal Position
    unsigned char  reg0x0B; // Vertical Position
    unsigned char  reg0x09; // Black Level (Brightness)
    unsigned char  reg0x11; // Contrast
} PARAMS_CHRONTEL;

// The way H2INC groks this exposes an internal error in ML 6.13
#ifndef H2INC
// Param Structure
typedef struct {
    unsigned long  Encoder_ID;  // NV_ENCODER_ type
    unsigned char  Default;     // get default setting if set, else get current
    union {
        PARAMS_BROOKTREE regsBt;
        PARAMS_CHRONTEL  regsCh;
    } u;     // NOTE: some compilers (eg: gcc) does not allow for anonymous sructs
} NV_CFGEX_DESKTOP_POSITION_TV_PARAMS;
#endif  // H2INC

/************** TV Encoder ***********************************/
#define NV_CFGEX_GET_TV_ENCODER_INFO                        104
// Param Structure
// EncoderType:
//      0 - None
//      1 - Chrontel 7003
//      2 - Brooktree 868
//      3 - Brooktree 869
//      All other values are reserved
// TVConnected:
//      0 - Not connected
//      1 - Connected
// EncoderID: I2C address of encoder
typedef struct {
    unsigned long  EncoderType;
    unsigned long  EncoderID;
    unsigned long  TVConnected;
} NV_CFGEX_TV_ENCODER_PARAMS;
#define TV_NOT_CONNECTED    0
#define TV_CONNECTED        1

#define NV_CFGEX_SET_TV_ENCODER_INFO                        105

/************** Flat Panel ***********************************/
#define NV_CFGEX_GET_FLAT_PANEL_INFO                        106
// Param structure
// FlatPanelMode:
//     0 - Not Connected
//     1 - Centered
//     2 - Native
//     3 - Scaled mode
// FlatPanelSizeX:
//     Max horizontal resolution.
// FlatPanelSizeY:
//     Max vertical resolution.
// FlatPanelNativeSupported
typedef struct {
    unsigned long FlatPanelMode;
    unsigned long FlatPanelSizeX;
    unsigned long FlatPanelSizeY;
    long          FlatPanelConnected;
    long          FlatPanelNativeSupported;
} NV_CFGEX_GET_FLATPANEL_INFO_PARAMS;
#define NV_CFGEX_GET_FLATPANEL_INFO_NOT_CONNECTED   0
#define NV_CFGEX_GET_FLATPANEL_INFO_CONNECTED       1
#define NV_CFGEX_GET_FLATPANEL_INFO_SCALED          0
#define NV_CFGEX_GET_FLATPANEL_INFO_CENTERED        1
#define NV_CFGEX_GET_FLATPANEL_INFO_NATIVE          2


#define NV_CFGEX_SET_FLAT_PANEL_INFO                        107
// Param structure
typedef struct {
    unsigned long CommitChanges;
    unsigned long FlatPanelMode;
} NV_CFGEX_SET_FLATPANEL_INFO_PARAMS;

/************** Monitor **************************************/
#define NV_CFGEX_GET_MONITOR_INFO                           108
// Param structure
// MonitorConnected:
//      0 - Not Connected
//      1 - Connected
typedef struct {
    unsigned long MonitorConnected;
} NV_CFGEX_GET_MONITOR_INFO_PARAMS;

#define NV_CFGEX_GET_MONITOR_INFO_NOT_CONNECTED                0
#define NV_CFGEX_GET_MONITOR_INFO_CONNECTED                    1

/************** Get the current Windows refresh rate and state (real, default or optimal) **************/
#define NV_CFGEX_GET_REFRESH_INFO             111
// Param Structure
typedef struct {
    unsigned long  Head;
    unsigned long ulRefreshRate;
    unsigned long ulState;
} NV_CFGEX_GET_REFRESH_INFO_PARAMS;

// definitions of state flags
#define NV_CFG_REFRESH_FLAG_SPECIFIC          0x00000000 /* use specific value contained in ulRefreshRate */
#define NV_CFG_REFRESH_FLAG_ADAPTER_DEFAULT   0x00000001 /* Windows is using "Adapter Default" setting    */
#define NV_CFG_REFRESH_FLAG_OPTIMAL           0x00000002 /* Windows is using "Optimal" setting            */

/************** Multihead CFGEX_SET **************/

#define NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_MULTI        112
// Param Structure
typedef struct {
    unsigned long  Head;
    unsigned long  CommitChanges;
    unsigned long  HRetraceStart;
    unsigned long  HRetraceEnd;
    unsigned long  VRetraceStart;
    unsigned long  VRetraceEnd;
} NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_MULTI_PARAMS;

#define NV_CFGEX_SET_FLAT_PANEL_INFO_MULTI                  113
// Param structure
typedef struct {
    unsigned long Head;
    unsigned long CommitChanges;
    unsigned long FlatPanelMode;
} NV_CFGEX_SET_FLATPANEL_INFO_MULTI_PARAMS;

// The way H2INC groks this exposes an internal error in ML 6.13
#ifndef H2INC
#define NV_CFGEX_COMMIT_DESKTOP_POSITION_TV_MULTI           114
// Param Structure
typedef struct {
    unsigned long  Head;
    unsigned long  Encoder_ID;  // NV_ENCODER_ type
    unsigned char  Default;     // get default setting if set, else get current
    union {
        PARAMS_BROOKTREE regsBt;
        PARAMS_CHRONTEL  regsCh;
    } u;     // NOTE: some compilers (eg: gcc) does not allow for anonymous sructs
} NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS;

#define NV_CFGEX_GET_DESKTOP_POSITION_TV_MULTI              115

#define NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_MULTI         116
// Param Structure
typedef struct {
    unsigned long  Head;
    unsigned long  GetDefault;
    unsigned long  HBlankStart;
    unsigned long  HBlankEnd;
    unsigned long  VBlankStart;
    unsigned long  VBlankEnd;
    unsigned long  HRetraceStart;
    unsigned long  HRetraceEnd;
    unsigned long  VRetraceStart;
    unsigned long  VRetraceEnd;
} NV_CFGEX_GET_DESKTOP_POSITION_MONITOR_MULTI_PARAMS;
#endif

/************** Get the offset within the AGP aperture (can't just subtract from linearbase anymore) ***/
#define NV_CFGEX_GET_AGP_OFFSET                 138
// Param Structure
typedef struct {
    void* linaddr;                      // passed in linear address, returned from a AGP space alloc
    void* physaddr;                     // returned physical address
    unsigned long offset;               // returned offset within the AGP aperture
} NV_CFGEX_GET_AGP_OFFSET_PARAMS;

/************** Return/Set the current linear GART address (64 bit clean version of NV_CFG_AGP_LINEAR_BASE above) ***/
#define NV_CFGEX_AGP_LINEAR_BASE                139
// Param Structure
typedef struct {
    void* linaddr;                      // passed in/returned linear address
} NV_CFGEX_AGP_LINEAR_BASE_PARAMS;

/************** Get the Display Type **************/
#define NV_CFGEX_GET_DISPLAY_TYPE               140
// Param Structure
typedef struct {
    unsigned long Type;
    unsigned long TVType;
} NV_CFGEX_GET_DISPLAY_TYPE_PARAMS;

// definitions of Display Types
#define DISPLAY_TYPE_MONITOR            0
#define DISPLAY_TYPE_TV                 1
#define DISPLAY_TYPE_DUALSURFACE        2
#define DISPLAY_TYPE_FLAT_PANEL         3
// definitions of TV types: see NV_CFG_VIDEO_TV_FORMAT

/************** Set the Display Type **************/
#define NV_CFGEX_SET_DISPLAY_TYPE               141
// Param Structure
typedef struct {
    unsigned long Type;
    unsigned long TVType;
} NV_CFGEX_SET_DISPLAY_TYPE_PARAMS;


/************** Get the Display Type for Specified Head **************/
#define NV_CFGEX_GET_DISPLAY_TYPE_MULTI         142
// Param Structure
typedef struct {
    unsigned long Head;
    unsigned long Type;
    unsigned long TVType;
} NV_CFGEX_GET_DISPLAY_TYPE_MULTI_PARAMS;

/************** Get the Display Type of the Specified Head **************/
#define NV_CFGEX_SET_DISPLAY_TYPE_MULTI         143
// Param Structure
typedef struct {
    unsigned long Head;
    unsigned long Type;
    unsigned long TVType;
} NV_CFGEX_SET_DISPLAY_TYPE_MULTI_PARAMS;

/************** Get the Raster Postion for the Specified Head ***********/
#define NV_CFGEX_CURRENT_SCANLINE_MULTI         144
// Param Structure
typedef struct {
    unsigned long Head;
    unsigned long RasterPosition;
} NV_CFGEX_CURRENT_SCANLINE_MULTI_PARAMS;

/************** CRTC Timing ***********/
#define NV_CFGEX_CRTC_TIMING_MULTI              145
// Param Structure
typedef struct {
    unsigned long  Head;
    unsigned long  Reg;                     // data location, see below
    unsigned long  HTotal;
    unsigned long  HDisplayEnd;
    unsigned long  HBlankStart;
    unsigned long  HBlankEnd;
    unsigned long  HRetraceStart;
    unsigned long  HRetraceEnd;
    unsigned long  VTotal;
    unsigned long  VDisplayEnd;
    unsigned long  VBlankStart;
    unsigned long  VBlankEnd;
    unsigned long  VRetraceStart;
    unsigned long  VRetraceEnd;
} NV_CFGEX_CRTC_TIMING_PARAMS;
#define NV_CFGEX_CRTC_PARAMS    14          // number of 32 bit values (HTotal - VRetraceEnd) 
#define NV_CFGEX_CRTC_TIMING_REGISTER 1     // get/set CRTC registers
#define NV_CFGEX_CRTC_TIMING_REGISTRY 2     // get/set Windows registry
#define NV_CFGEX_CRTC_TIMING_DEFAULT  4     // get default registers (set not valid)

/** Return the linear address of (BAR0) of this device (64 bit clean version) */
#define NV_CFGEX_ADDRESS_NVADDR                 146
// Param Structure
typedef struct {
    void* nvaddr;
} NV_CFGEX_ADDRESS_NVADDR_PARAMS;

/************** RESERVED **************/
#define NV_CFGEX_RESERVED                       150

/************** Enable tiling ********************************/
#define NV_CFGEX_ENABLE_TETRIS_TILING                       200

/************** Disable tiling *******************************/
#define NV_CFGEX_DISABLE_TETRIS_TILING                      201

/************** Get the monitor EDID *************************/
#define NV_CFGEX_GET_EDID                                   210
// param structure
typedef struct {
    unsigned long displayType;  // see NV_CFGEX_GET_DISPLAY_TYPE for legal values
    unsigned char *edidBuffer;
    unsigned long *bufferLength;  // in/out
} NV_CFGEX_GET_EDID_PARAMS;

/************** Get the current device BIOS ******************/
#define NV_CFGEX_GET_BIOS                                   215
// param structure
typedef struct {
    unsigned char *biosBuffer;
    unsigned long *bufferLength;  // in/out
} NV_CFGEX_GET_BIOS_PARAMS;

//
//  New versions of ConfigGetEx for multi-head devices
//

//
// NV_CFG_DAC_PCLK_LIMIT_8BPP - Return the current 8bpp graphics clock limit (in Hz) for specified head on this device 
//
#define NV_CFGEX_DAC_PCLK_LIMIT_8BPP            264
typedef struct {
    unsigned long Head;
    unsigned long pclkLimit;
} NV_CFGEX_DAC_PCLK_LIMIT_PARAMS;

//
// NV_CFG_DAC_PCLK_LIMIT_16BPP - Return the current 16bpp graphics clock limit (in Hz) for specified head on this device
//
#define NV_CFGEX_DAC_PCLK_LIMIT_16BPP           265

//
// NV_CFG_DAC_PCLK_LIMIT_32BPP - Return the current 16bpp graphics clock limit (in Hz) for specified head onthis device
//
#define NV_CFGEX_DAC_PCLK_LIMIT_32BPP           266

//
// NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST - Return the pre-dac NV11 color saturation boost value
//
#define NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST  268
typedef struct {
    unsigned long Head;
    unsigned char boostValue;
} NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_PARAMS;

#define NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_0          0x00000000
#define NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_188        0x00000001
#define NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_375        0x00000002
#define NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_750        0x00000003

// Another get info for a specified head
#define NV_CFGEX_GET_TV_ENCODER_INFO_MULTI      270
typedef struct {
    unsigned long  Head;
    unsigned long  EncoderType;
    unsigned long  EncoderID;
    unsigned long  TVConnected;
} NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS;

// Another get info for a specified head
#define NV_CFGEX_GET_FLAT_PANEL_INFO_MULTI       271
// Param structure
// FlatPanelMode:
//     0 - Not Connected
//     1 - Centered
//     2 - Native
//     3 - Scaled mode
// FlatPanelSizeX:
//     Max horizontal resolution.
// FlatPanelSizeY:
//     Max vertical resolution.
// FlatPanelNativeSupported
typedef struct {
    unsigned long Head;
    unsigned long FlatPanelMode;
    unsigned long FlatPanelSizeX;
    unsigned long FlatPanelSizeY;
    long          FlatPanelConnected;
    long          FlatPanelNativeSupported;
} NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS;

// Return TV encoder type on specified head
#define NV_CFGEX_VIDEO_ENCODER_TYPE             272
typedef struct {
    unsigned long Head;
    unsigned long EncoderType;
} NV_CFGEX_VIDEO_ENCODER_TYPE_PARAMS;

// Return TV encoder ID on specified head
#define NV_CFGEX_VIDEO_ENCODER_ID               273
typedef struct {
    unsigned long Head;
    unsigned long EncoderID;
} NV_CFGEX_VIDEO_ENCODER_ID_PARAMS;

#define NV_CFGEX_GET_EDID_MULTI                 274
// param structure
typedef struct {
    unsigned long Head;
    unsigned long displayType;  // see NV_CFGEX_GET_DISPLAY_TYPE for legal values
    unsigned char *edidBuffer;
    unsigned long *bufferLength;  // in/out
} NV_CFGEX_GET_EDID_MULTI_PARAMS;

#define NV_CFGEX_GET_MONITOR_INFO_MULTI         275
// Param structure
// MonitorConnected:
//      0 - Not Connected
//      1 - Connected
typedef struct {
    unsigned long Head;
    unsigned long MonitorConnected;
} NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS;

#define NV_CFGEX_SYNC_STATE                     276
// Param structure
//     set/get the state of HSYNC and VSYNC
typedef struct {
    unsigned long Head;
    unsigned long Hsync;              // non-zero to enable
    unsigned long Vsync;              // non-zero to enable
    unsigned long oldHsync;           // on SETEX, what was previous value
    unsigned long oldVsync;           // ""
} NV_CFGEX_SYNC_STATE_PARAMS;

#define NV_CFGEX_PRIMARY_INFO                    277
// Param structure
//     get the state of the primary surface for the given head
typedef struct {
    unsigned long Head;               // input param; 0 or 1
    unsigned long Pitch;              // output
    unsigned long Offset;             // output; byte offset from beginning of FB
} NV_CFGEX_PRIMARY_INFO_PARAMS;

#define NV_CFGEX_FLAT_PANEL_BRIGHTNESS		278
//param structure
//   -this structure is used for setting and getting the settings of the PWM brightness output.
//   -when the getex command is called for this structure, the only two valid values on return will
//    be PWMControllerEnable and PercentRelativeBrightness. 
typedef struct
{
	unsigned long Head;
    unsigned long FlatPanelBrightnessControlFrequency;
    unsigned long PercentMaximumPWMDutyCycle;
    unsigned long PercentMinimumPWMDutyCycle;
    unsigned long PercentRelativeBrightness;
    signed long PWMControllerEnable;
} NV_CFGEX_FLATPANEL_BRIGHTNESS_PARAMS;

/************** Return Pitch given Width/Depth ***************/
#define NV_CFGEX_GET_SURFACE_PITCH                          300
// Param structure
typedef struct {
    unsigned long Width;          // in (pixels)
    unsigned long Depth;          // in (bits per pixel)
    unsigned long Pitch;          // out
} NV_CFGEX_GET_SURFACE_PITCH_PARAMS;

/************** Return Pitch/Size given Width/Height/Depth ***/
#define NV_CFGEX_GET_SURFACE_DIMENSIONS                     301
// Param structure
typedef struct {
    unsigned long Width;          // in (pixels)
    unsigned long Height;         // in (pixels)
    unsigned long Depth;          // in (bits per pixel)
    unsigned long Pitch;          // out
    unsigned long Size;           // out
} NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS;

/************** Return Supported Classes for a given NV chip */
#define NV_CFGEX_GET_SUPPORTED_CLASSES                      310
//
// Pass in a NULL classBuffer pointer to have the RM fill in the numClasses field.
// This is the number of dword entries that should be allocated by the caller for
// the classBuffer.
//
// Then, make this call again with the filled in classBuffer and the RM will verify
// the numClasses is large enough and fill in classBuffer with the supported class
// numbers.
//
// Param structure
typedef struct {
    unsigned long  numClasses;    // number of supported classes
    unsigned long *classBuffer;   // dword buffer holding supported class values
} NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS;

/************** Return RM mappings to device's regs/fb *******/
#define NV_CFGEX_GET_DEVICE_MAPPINGS                        311
//
// This call is used by NVDE debugger to retrieve the RM's
// (linear) mappings to a given device's registers and framebuffer.
//
// Param structure
typedef struct {
    void *nvVirtAddr;               // register virtual address
    void *nvPhysAddr;               // register physical address
    void *fbVirtAddr;               // fb virtual address
    void *fbPhysAddr;               // fb physical address
} NV_CFGEX_GET_DEVICE_MAPPINGS_PARAMS;

/************** Set Macrovision Mode *************************/
#define NV_CFGEX_SET_MACROVISION_MODE                       320 
typedef struct {
    unsigned long   head;
    unsigned long   mode;
    unsigned long   resx;
    unsigned long   resy;
    unsigned long   status;
} NV_CFGEX_SET_MACROVISION_MODE_PARAMS;

//
// NV_CFGEX_CHECK_CONNECTED_DEVICES - Return connect status of the specified
//  devices.
// Input: a bitmask that is a subset of the bitmask returned by the
//          NV_CFG_GET_ALL_DEVICES call.  set bits indicate the calling routine
//          want to check the connect status of the corresponding devices.
// Output: set bits in the bitmask indicating the corresponding devices are
//          connected.
//
// bits 0..7 represent CRT0..CRT7
// bits 8..15 represent TV0..TV7
// bits 16..23 represent DD0..DD7
// bits 24..31 are reserved
//
#define NV_CFGEX_CHECK_CONNECTED_DEVICES        321

//
// NV_CFGEX_GET_DEVICES_CONFIGURATION - Return possible configuration for the
//  specified devices.  Pass in OldDevicesConfig and OldDevicesAllocationMap
//  if you want the routine to try to keep existing attached devices on the
//  same heads whenever possible.  Pass in zero for OldDevicesConfig otherwise.
//  
// Input: DevicesConfig - a bitmask that is a subset of the bitmask returned
//              by the NV_CFGEX_CHECK_CONNECTED_DEVICES call. set bits indicate
//              devices to be in the configuration. up to eight bits can be set.
//        OldDevicesConfig - DevicesConfig used from the last call to
//              NV_CFGEX_GET_DEVICES_CONFIGURATION.
//        OldDevicesAllocationMap - DevicesAllocationMap returned by the last
//              call to NV_CFGEX_GET_DEVICES_CONFIGURATION.
// Output: DevicesAllocationMap - a 32-bit value in the following format:
//          nibble0 - head that can be used to connect to device0 (device
//                      corresponds to the rightmost set bit.
//          nibble1 - head that can be used to connect to device1 (device
//                      corresponds to the second rightmost set bit.
//          .
//          .
//
// bits 0..7 represent CRT0..CRT7
// bits 8..15 represent TV0..TV7
// bits 16..23 represent DD0..DD7
// bits 24..31 are reserved
//
typedef struct {
    unsigned long  DevicesConfig;
    unsigned long  DevicesAllocationMap;
    unsigned long  OldDevicesConfig;
    unsigned long  OldDevicesAllocationMap;
} NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS;
#define NV_CFGEX_GET_DEVICES_CONFIGURATION      322

// NV_CFGEX_GET_HOTKEY_EVENT - Report Fn+x hotkey events on a mobile system.
//  Some events are informational -- the BIOS has already acted on the event.
//  Other events indicate an action that must be taken care of by the driver.
//  Currently, only the display enable/disable events fall into this category.
//
// The RM checks the BIOS for new events every vblank.  Note that more than one
//  event may be added to the queue at a time (e.g., display enable/disable events
//  will often occur in sets).  The caller should continue to poll until the
//  call returns NV_HOTKEY_EVENT_NONE_PENDING.
//
// If the BIOS does not support this feature, the RM will return NV_HOTKEY_EVENT_NOT_SUPPORTED.
// 
// Input: a pointer to a NV_CFGEX_GET_HOTKEY_EVENT_PARAMS paramStruct
// Output: the event field will report a number from the event list below
//         the status field, where appropriate, indicates the new state the event is reporting
//
#define NV_CFGEX_GET_HOTKEY_EVENT                            330

//Queue Commands
#define NV_HOTKEY_EVENT_NOT_SUPPORTED        0  //No BIOS hotkey support.  Further polling not required.
#define NV_HOTKEY_EVENT_NONE_PENDING         1  //No hotkey events currently pending
#define NV_HOTKEY_EVENT_DISPLAY_ENABLE       2  //status bits decode to disable/enable state for
                                                // each display (definitions below) (implemented by driver)
//#define NV_HOTKEY_EVENT_DISPLAY_LCD          2  //status 0/1 = disable/enable (implemented by driver)
//#define NV_HOTKEY_EVENT_DISPLAY_CRT          3  //status 0/1 = disable/enable (implemented by driver)
//#define NV_HOTKEY_EVENT_DISPLAY_TV           4  //status 0/1 = disable/enable (implemented by driver)
//#define NV_HOTKEY_EVENT_DISPLAY_DFP          5  //status 0/1 = disable/enable (implemented by driver)
#define NV_HOTKEY_EVENT_DISPLAY_CENTERING    6  //scaled/centered display - status values define below (implemented by BIOS)
                                                // mirrors settings for NV_PRAMDAC_FP_TG_CONTROL_MODE in nv_ref.h

// Status bit definitions for NV_HOTKEY_EVENT_DISPLAY_ENABLE event
// Currently identical to the BIOS register bit definitions, but
// we decode it explicitly to avoid implicit dependencies.
#define NV_HOTKEY_STATUS_DISPLAY_ENABLE_LCD 0x01
#define NV_HOTKEY_STATUS_DISPLAY_ENABLE_CRT 0x02
#define NV_HOTKEY_STATUS_DISPLAY_ENABLE_TV  0x04
#define NV_HOTKEY_STATUS_DISPLAY_ENABLE_DFP 0x08

//Enumeration of centering/scaling settings used with
// NV_HOTKEY_EVENT_DISPLAY_CENTERING event
#define NV_HOTKEY_STATUS_DISPLAY_SCALED     0x00
#define NV_HOTKEY_STATUS_DISPLAY_CENTERED   0x01
#define NV_HOTKEY_STATUS_DISPLAY_NATIVE     0x02

typedef struct {
    unsigned long event;
    unsigned long status;
} NV_CFGEX_GET_HOTKEY_EVENT_PARAMS;


#define NV_CFGEX_GET_LOGICAL_DEV_EDID                331 
// param structure
typedef struct {
    unsigned long displayMap;
    unsigned char *edidBuffer;
    unsigned long *bufferLength;  // in/out
} NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS;

#pragma pack() //


// NvRmOsConfigSet/Get parameters
//
// definitions for OS-specific versions of the config get/set calls.
//   When possible, new config calls should be added to the standard
//   config calls instead of here.
//

// NV_OSCFG_VSL_MODE - Return/Set the passing of VSL to the display manager
//
//  Initial value is 1
// 

#define NV_OSCFG_VSL_MODE                        0x0001


////////////////////////////////////////////////////////////////////
//
// NvRmOsConfigSetEx/GetEx parameters
//
//

//
// Get info about the cards in the system
//
typedef struct
{
    int    flags;               // see below
    int    instance;            // resman's ordinal for the card
    int    bus;                 // bus number (PCI, AGP, etc)
    int    slot;                // card slot
    int    vendor_id;           // PCI vendor id
    int    device_id;
    int    interrupt_line;
} NVCARDINFO, *PNVCARDINFO;

#define NV_CARD_INFO_FLAG_PRESENT       0x0001

#define NV_OSCFGEX_GET_CARD_INFO           100

typedef struct {
    unsigned long NumCards;                // input size of buffer; output # cards
    PNVCARDINFO   pCardInfo;
} NV_OSCFGEX_GET_CARD_INFO_PARAMS;


//---------------------------------------------------------------------------
//
//  Configuration Manager API.
//
//---------------------------------------------------------------------------
//
//
// DWORD NvConfigVersion(VOID)
//
//  Returns the revision of the ddk (config) interface built into the resource manager.
//  This is used for version continuity between all the resource manager files,
//  as well as provides the interface version for people using the config interface.
//
//  The format of this is 0xAAAABBCC, where
//   - 0xAAAA is [nvidia internal]
//   -   0xBB is the software release revision
//   -   0xCC is the minor revision
//
//
// DWORD NvConfigGet(DWORD Index, DWORD DeviceHandle)
//
//  Given an Index from NVCM.H and a pointer to a specific device (see SDK), return
//  the current configuration value.  The format of the value is dependent on the
//  index requested.
//
//
// DWORD NvConfigSet(DWORD Index, DWORD NewValue, DWORD DeviceHandle)
//
//  Given an Index from NVCM.H, a pointer to a specific device (see SDK), and a new
//  value, update the current configuration value.  This call returns the original
//  value in that configuration index.  In general, most new values do not take affect
//  until NvConfigUpdate() is called.  This allows multiple values to be batched
//  together before enabling the change.
//
//
// DWORD NvConfigUpdate(DWORD UpdateFlag, DWORD DeviceHandle)
//
//  Given an update flag and pointer to a specific device (see SDK), perform any previously
//  batched NvConfigSet() calls.  This call returns an error code (0 denotes success).
//  The only UpdateFlag currently in use is NV_CFG_PERMANENT.
//

#ifndef _WIN32
#if defined(__GNUC__) || defined(MACOS)
int NvConfigVersion(int);
int NvConfigGet(int, int);
int NvConfigSet(int, int, int);
int NvConfigUpdate(int, int);
int NvConfigVga(int, int);
#elif !defined(__WATCOMC__)
DWORD FAR PASCAL NvConfigVersion(DWORD);
DWORD FAR PASCAL NvConfigGet(DWORD, DWORD);
DWORD FAR PASCAL NvConfigSet(DWORD, DWORD, DWORD);
DWORD FAR PASCAL NvConfigUpdate(DWORD, DWORD);
DWORD FAR PASCAL NvConfigVga(DWORD, DWORD);
#endif // ! __WATCOMC__

#elif !defined(WINNT)
int __stdcall NvConfigVersion(void);
int __stdcall NvConfigGet(int, int);
int __stdcall NvConfigSet(int, int, int);
int __stdcall NvConfigUpdate(int);
#ifdef __WATCOMC__
#pragma aux (__stdcall) NvConfigVersion "*";
#pragma aux (__stdcall) NvConfigGet "*";
#pragma aux (__stdcall) NvConfigSet "*";
#pragma aux (__stdcall) NvConfigUpdate "*";
#endif // __WATCOMC__
#endif // _WIN32


/////////////////////////////////////////////////////////////////////////////
//
// THE FOLLOWING DEFINES AND ENTRY POINTS ARE NVIDIA RESERVED
//
//---------------------------------------------------------------------------
//
//  Device Defines.
//
//---------------------------------------------------------------------------

//
// Parameter to DeviceGet.
//
#define NV_DEV_BASE                             1
#define NV_DEV_ALTERNATE                        2
#define NV_DEV_BUFFER_0                         3
#define NV_DEV_BUFFER_1                         4
#define NV_DEV_TIMER                            5
#define NV_DEV_PFB                              6
#define NV_DEV_PGRAPH                           7
#define NV_DEV_PRMCIO                           8
#define NV_DEV_PRMVIO                           9
#define NV_DEV_AGP                              10
#define NV_DEV_GAMMA                            11
#define NV_DEV_PRAMDAC                          12
#define NV_DEV_PCRTC                            13
#define NV_DEV_MAX                              13

//---------------------------------------------------------------------------
//
//  Device Pointer API.
//
//---------------------------------------------------------------------------
#ifndef WINNT
#ifndef _WIN32
#if defined(__GNUC__) || defined(MACOS)
int NvIoControl(int, int);
int NvDeviceBaseGet(int, int);
int NvDeviceLimitGet(int, int);
int NvDeviceSelectorGet(int, int);
int NvGetHardwarePointers(int *, int *, int *, int *);
#elif !defined(__WATCOMC__)
//DWORD FAR PASCAL NvIoControl(DWORD, DWORD);
DWORD FAR PASCAL NvDeviceBaseGet(DWORD, DWORD);
DWORD FAR PASCAL NvDeviceLimitGet(DWORD, DWORD);
WORD  FAR PASCAL NvDeviceSelectorGet(DWORD, DWORD);
DWORD FAR PASCAL NvGetHardwarePointers(DWORD *, DWORD *, DWORD *, DWORD *);
#endif // __WATCOMC__
#else  /* WIN32 */
void __stdcall NvIoControl(int, void *);
int  __stdcall NvDeviceBaseGet(int, int);
int  __stdcall NvDeviceLimitGet(int, int);
int  __stdcall NvDeviceSelectorGet(int, int);
int  __stdcall NvGetHardwarePointers(int *, int *, int *, int *, int);
#ifdef __WATCOMC__
#pragma aux (__stdcall) NvIoControl "*";
#pragma aux (__stdcall) NvDeviceBaseGet "*";
#pragma aux (__stdcall) NvDeviceLimitGet "*";
#pragma aux (__stdcall) NvDeviceSelectorGet "*";
#pragma aux (__stdcall) NvGetHardwarePointers "*";
#endif // __WATCOMC__
#endif // _WIN32
#endif // !WINNT
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NVCM_H_
