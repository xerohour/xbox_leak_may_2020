 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/**************************************************************************************************************
*
*   Header: bios.h
*
*   Description:
*       Definitions of BIOS structures.
*
*   Revision History:
*       Original -- 9/99 Jeff Westerinen
*
**************************************************************************************************************/

// We convert the bios data structures before they are used.  They are read from bios,
//   then each field is expanded into a 32 bit field into host byte order.
//   Each data structure has a format string below that describes its packed
//   layout in the rom.
//
// The format string looks like:
//
//    count defaults to 1
//    item is one of
//          b  (byte)
//          w  2 byte word
//          d  4 byte word
//
//  Eg: "1b3d"
//
//   means 1 byte field followed by 3 dword (32 bit) fields
//

#define bios_U008  U032
#define bios_U016  U032
#define bios_U032  U032
#define bios_S008  S032

// definition from nvidia.inc
#define EOL                     0xff    // end of list
#define INIT_PLL_REG            'p'     // Write a PLL value into the DAC (obsolete)
#define INIT_PLL                'y'     // Write an NV3 PLL value based on crystal
#define INIT_NV_REG             'n'     // Write an NV Register with mask
#define INIT_ZM_REG             'z'     // NV Register with 0 mask
#define INIT_ZM_WREG            'w'     // NV word Register with 0 mask
#define INIT_DAC_REG            'd'     // Write to a DAC register (obsolete)
#define INIT_TIME               't'     // Delay
#define INIT_IO                 'i'     // Write an IO value
#define INIT_INDEX_IO           'x'     // Write an indexed IO value
#define INIT_MEM_RESTRICT       'm'     // Memory restrict
#define INIT_STRAP_RESTRICT     's'     // Strap restrict
#define INIT_RESUME             'r'     // Resume operations
#define INIT_CONFIGURE_MEM      'f'     // Configure memory
#define INIT_CONFIGURE_CLK      'g'     // Configure clocks
#define INIT_CONFIGURE_PREINIT  'h'     // Configure RAM Type specifiers, etc
#define INIT_COMPUTE_MEM        'c'     // Compute memory size
#define INIT_RESET              'e'     // Reset chip
#define INIT_DONE               'q'     // Quit script

// new opocdes for BIP/3
#define INIT_ZM_IO              'a'     // Write an IO value with 0 mask
#define INIT_ZM_INDEX_IO        'b'     // Write an indexed IO value with 0 mask
#define INIT_EOS                'l'     // End of current script
#define INIT_SUB                'k'     // Execute another script as a subroutine
#define INIT_JUMP               'j'     // Jump to another script
#define INIT_MACRO              'o'     // Execute macro
#define INIT_CONDITION          'u'     // Restruct until INIT_RESUME based on condition
#define INIT_IO_CONDITION       'v'     // Restrict until INIT_RESUME based on I/O condition being true
#define INIT_IO_FLAG_CONDITION  '9'     // Restrict until INIT_RESUME based on I/O condition being true
#define INIT_RESTRICT_PROG      '1'     // Program a register from a value array based on another register
#define INIT_IO_RESTRICT_PROG   '2'     // Program a register from a value array based on an I/O port
#define INIT_REPEAT             '3'     // Repeat block N times up until INIT_END_REPEAT
#define INIT_END_REPEAT         '6'     // End repeat block
#define INIT_IO_RESTRICT_PLL    '4'     // Program a PLL clock from a value array based on an I/O port
#define INIT_COPY               '7'     // Copy a portion of a NV register to a IO register
#define INIT_NOT                '8'     // Invert the current condition status
#define INIT_FUNCTION           '5'     // Execute generic function !!!
#define INIT_INDEX_ADDRESS_LATCHED      'I' // Program a block of registers that are accessed via an index register
#define SCRIPT_TERMINATE        0x00    // End of sequential script indexes

// definitions from biosequ.inc
#define CRTC_INDEX_MONO             0x03B4
#define CRTC_INDEX_COLOR            0x03D4
#define ATC_INDEX                   0x03C0
#define SEQ_INDEX                   0x03C4
#define GDC_INDEX                   0x03CE

#define CRYSTAL_FREQ143KHZ          14318              // 14318 kHz
#define CRYSTAL_FREQ135KHZ          13500              // 13500 kHz

#define MAX_BIOS_SCAN               1024

// Hotkey bit definitions -- for test purposes.

// These bit definitions refer to CRTC register 4A, aka NV_CIO_CRE_SCRATCH5__INDEX.
#define NV_MOBILE_FLAG_DISPLAY_MASK     0x04    //SSF3
#define NV_MOBILE_FLAG_SCALE_MASK       0x10    //SSF5

// These bit definitions refer to CRTC register 4B, aka NV_CIO_CRE_SCRATCH6__INDEX.
#define NV_DRIVER_HANDLES_HOTKEYS       0x80    //SOS8
#define NV_IS_MOBILE_BIOS               0x40    //SOS7

// These bit definitions refer to CRTC register 4C, aka NV_CIO_CRE_SCRATCH7__INDEX.
#define NV_MOBILE_DISPLAY_LCD_MASK      0x01
#define NV_MOBILE_DISPLAY_CRT_MASK      0x02
#define NV_MOBILE_DISPLAY_TV_MASK       0x04
#define NV_MOBILE_DISPLAY_DFP_MASK      0x08

#define NV_MOBILE_DISPLAY_SCALE_MASK    0x80

#define NV_MOBILE_DISPLAY_EVENTS_MASK   (NV_MOBILE_DISPLAY_LCD_MASK | NV_MOBILE_DISPLAY_CRT_MASK \
                                        | NV_MOBILE_DISPLAY_TV_MASK | NV_MOBILE_DISPLAY_DFP_MASK \
                                        | NV_MOBILE_DISPLAY_SCALE_MASK)
///


#pragma pack(1)

// BMP information structure
#define CTRL_BLK_ID                 0x564e7fff
#define BIOS_ROM_CODE_SIZE          0x10000
#define BMP_INIT_CODE_SIZE_MAX      8192
#define BMP_INIT_TABLE_VER_0        0
#define BMP_INIT_TABLE_VER_1        1
#define BMP_INIT_TABLE_VER_2        2
#define BMP_INIT_TABLE_VER_3        3
#define BMP_INIT_TABLE_VER_4        4
#define BMP_INIT_TABLE_VER_5        5
#define BMP_INIT_TABLE_VER_6        6
#define BMP_INIT_TABLE_VER_UNDEF    0xffffffff
#define BMP_INIT_TABLE_SUBVER_0x10  0x10


#define BMP_CONTROL_BLOCK_FMT "w3b3b"

typedef struct
{
    bios_U016 CTL_ID;                    // ID Marker (0x7FFF)
    bios_U008 CTL_Header[3];             // ID Header ("NV\0")
    bios_U008 CTL_Version;               // Control Block Version
    bios_U008 CTL_Flags;                 // Control Block Subversion
    bios_U008 CTL_Checksum;              // Checksum byte for Control Block
} BMP_Control_Block, *PBMP_Control_Block;


#define BMP_INIT1_FMT BMP_CONTROL_BLOCK_FMT "2b1d" "3w" "2w" "5w" "5w"
typedef struct
{
    BMP_Control_Block bmpCtrlBlk;        // BMP control block

    bios_U008 BIOS_Checksum;             // Checksum byte for BIOS
    bios_U008 BIOS_Features;             // BIOS Feature Byte (bitfields)
    bios_U032 BIOS_Version;              // BIOS Version in BCD (Major/Minor/Sub)
                            
    bios_U016 BMP_GenInitTable;          // Pointer to the General Initialization Table.
    bios_U016 BMP_ExtraInitTable;        // Pointer to Extra Initialization Table
    bios_U016 BMP_SizeInitTable;         // Max size of Initialization Table.

    bios_U016 szSignonMsg;               // Offset to signon string
    bios_U016 BMP_SizeSignonMsg;         // Current length of signon message
                           
    bios_U016 BMP_Fonts;                 // Pointer to pointers to fonts
    bios_U016 BMP_FontSize;              // Size of memory available for all fonts
    bios_U016 BMP_InitSavePtr;           // Pointer to initial save pointer area
    bios_U016 BMP_Functionality;         // Pointer to functionality table for Function 1Bh
    bios_U016 BMP_TblClockMNP;           // MNP parameters for clocks

    bios_U016 BMP_OEMString;             // OEM String to identify grahpics controller chip or product family (20)
    bios_U016 BMP_OEMVendorName;         // Name of the vendor that produced the display controller board product (35)
    bios_U016 BMP_OEMProductName;        // Product name of the controller board (35)
    bios_U016 BMP_OEMProductRev;         // Revision of manufacturing level of the display controller board (30)
    bios_U016 BMP_FifoTable;             // Pointer to DAC/CRTC fifo settings table

} BMP_Struc_Rev1, *PBMP_Struc_Rev1;


#define BMP_INIT2_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "3w" "2w" "5w" "5w"
typedef struct
{
    BMP_Control_Block bmpCtrlBlk;        // BMP control block

    bios_U008 BIOS_Checksum;             // Checksum byte for BIOS
    bios_U008 BIOS_Features;             // BIOS Feature Byte (bitfields)
    bios_U032 BIOS_Version;              // BIOS Version in BCD (Major/Minor/Sub)
    bios_U032 BIOS_DriverInfo;           // BIOS Version in BCD (Major/Minor/Sub)
                            
    bios_U016 BMP_GenInitTable;          // Pointer to the General Initialization Table.
    bios_U016 BMP_ExtraInitTable;        // Pointer to Extra Initialization Table
    bios_U016 BMP_SizeInitTable;         // Max size of Initialization Table.

    bios_U016 szSignonMsg;               // Offset to signon string
    bios_U016 BMP_SizeSignonMsg;         // Current length of signon message
                           
    bios_U016 BMP_Fonts;                 // Pointer to pointers to fonts
    bios_U016 BMP_FontSize;              // Size of memory available for all fonts
    bios_U016 BMP_InitSavePtr;           // Pointer to initial save pointer area
    bios_U016 BMP_Functionality;         // Pointer to functionality table for Function 1Bh
    bios_U016 BMP_TblClockMNP;           // MNP parameters for clocks

    bios_U016 BMP_OEMString;             // OEM String to identify grahpics controller chip or product family (20)
    bios_U016 BMP_OEMVendorName;         // Name of the vendor that produced the display controller board product (35)
    bios_U016 BMP_OEMProductName;        // Product name of the controller board (35)
    bios_U016 BMP_OEMProductRev;         // Revision of manufacturing level of the display controller board (30)
    bios_U016 BMP_FifoTable;             // Pointer to DAC/CRTC fifo settings table

} BMP_Struc_Rev2, *PBMP_Struc_Rev2;



#define BMP_INIT3_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "6w" "2w" "5w" "5w"

typedef struct
{
    BMP_Control_Block bmpCtrlBlk;   // BMP control block

    bios_U008 BIOS_Checksum;             // Checksum byte for BIOS
    bios_U008 BIOS_Features;             // BIOS Feature Byte (bitfields)
    bios_U032 BIOS_Version;              // BIOS Version in BCD (Major/Minor/Sub)
    bios_U032 BIOS_DriverInfo;           // BIOS Version in BCD (Major/Minor/Sub)
                            
    bios_U016 BMP_GenInitTable;          // Offset to the General Initialization Table.
    bios_U016 BMP_ExtraInitTable;        // Offset to Extra Initialization Table.
    bios_U016 BMP_SizeInitTable;         // Max size of Initialization Table.
    bios_U016 BMP_MemInitTable;          // Offset to the Memory Initialization Table.
    bios_U016 BMP_SDRSequenceTable;      // Offset to the SDR Sequence Table.
    bios_U016 BMP_DDRSequenceTable;      // Offset to the DDR Sequence Table.

    bios_U016 szSignonMsg;               // Offset to signon string
    bios_U016 BMP_SizeSignonMsg;         // Current length of signon message
                           
    bios_U016 BMP_Fonts;                 // Pointer to pointers to fonts
    bios_U016 BMP_FontSize;              // Size of memory available for all fonts
    bios_U016 BMP_InitSavePtr;           // Pointer to initial save pointer area
    bios_U016 BMP_Functionality;         // Pointer to functionality table for Function 1Bh
    bios_U016 BMP_TblClockMNP;           // MNP parameters for clocks

    bios_U016 BMP_OEMString;             // OEM String to identify grahpics controller chip or product family (20)
    bios_U016 BMP_OEMVendorName;         // Name of the vendor that produced the display controller board product (35)
    bios_U016 BMP_OEMProductName;        // Product name of the controller board (35)
    bios_U016 BMP_OEMProductRev;         // Revision of manufacturing level of the display controller board (30)
    bios_U016 BMP_FifoTable;             // Pointer to DAC/CRTC fifo settings table

} BMP_Struc_Rev3, *PBMP_Struc_Rev3;


#define BMP_INIT4_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "6w" "2w" "5w" "5w" "4b2w"

typedef struct
{
    BMP_Control_Block bmpCtrlBlk;        // BMP control block

    bios_U008 BIOS_Checksum;             // Checksum byte for BIOS
    bios_U008 BIOS_Features;             // BIOS Feature Byte (bitfields)
    bios_U032 BIOS_Version;              // BIOS Version in BCD (Major/Minor/Sub)
    bios_U032 BIOS_DriverInfo;           // BIOS Version in BCD (Major/Minor/Sub)
                            
    bios_U016 BMP_GenInitTable;          // Offset to the General Initialization Table.
    bios_U016 BMP_ExtraInitTable;        // Offset to Extra Initialization Table.
    bios_U016 BMP_SizeInitTable;         // Max size of Initialization Table.
    bios_U016 BMP_MemInitTable;          // Offset to the Memory Initialization Table.
    bios_U016 BMP_SDRSequenceTable;      // Offset to the SDR Sequence Table.
    bios_U016 BMP_DDRSequenceTable;      // Offset to the DDR Sequence Table.

    bios_U016 szSignonMsg;               // Offset to signon string
    bios_U016 BMP_SizeSignonMsg;         // Current length of signon message
                           
    bios_U016 BMP_Fonts;                 // Pointer to pointers to fonts
    bios_U016 BMP_FontSize;              // Size of memory available for all fonts
    bios_U016 BMP_InitSavePtr;           // Pointer to initial save pointer area
    bios_U016 BMP_Functionality;         // Pointer to functionality table for Function 1Bh
    bios_U016 BMP_TblClockMNP;           // MNP parameters for clocks

    bios_U016 BMP_OEMString;             // OEM String to identify grahpics controller chip or product family (20)
    bios_U016 BMP_OEMVendorName;         // Name of the vendor that produced the display controller board product (35)
    bios_U016 BMP_OEMProductName;        // Product name of the controller board (35)
    bios_U016 BMP_OEMProductRev;         // Revision of manufacturing level of the display controller board (30)
    bios_U016 BMP_FifoTable;             // Pointer to DAC/CRTC fifo settings table

    bios_U008 BMP_CRTI2CPort;            // Default I2C port for CRT
    bios_U008 BMP_TVI2CPort;             // Default I2C port for TV
    bios_U008 BMP_DFPI2CPort;            // Default I2C port for DFP
    bios_U008 BMP_RsvdI2CPort;           // Reserved for future expansion
    bios_U016 BMP_I2CPort0IO;            // Read/Write reg for I2C Port 0
    bios_U016 BMP_I2CPort1IO;            // Read/Write reg for I2C Port 1

} BMP_Struc_Rev4, *PBMP_Struc_Rev4;


#define BMP_INIT5_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "6w" "2w" "5w" "5w" "4b2w" "1b2w"

typedef struct
{
    BMP_Control_Block bmpCtrlBlk;   // BMP control block

    bios_U008 BIOS_Checksum;             // Checksum byte for BIOS
    bios_U008 BIOS_Features;             // BIOS Feature Byte (bitfields)
    bios_U032 BIOS_Version;              // BIOS Version in BCD (Major/Minor/Sub)
    bios_U032 BIOS_DriverInfo;           // BIOS Version in BCD (Major/Minor/Sub)
                            
    bios_U016 BMP_GenInitTable;          // Offset to the General Initialization Table.
    bios_U016 BMP_ExtraInitTable;        // Offset to Extra Initialization Table.
    bios_U016 BMP_SizeInitTable;         // Max size of Initialization Table.
    bios_U016 BMP_MemInitTable;          // Offset to the Memory Initialization Table.
    bios_U016 BMP_SDRSequenceTable;      // Offset to the SDR Sequence Table.
    bios_U016 BMP_DDRSequenceTable;      // Offset to the DDR Sequence Table.

    bios_U016 szSignonMsg;               // Offset to signon string
    bios_U016 BMP_SizeSignonMsg;         // Current length of signon message
                           
    bios_U016 BMP_Fonts;                 // Pointer to pointers to fonts
    bios_U016 BMP_FontSize;              // Size of memory available for all fonts
    bios_U016 BMP_InitSavePtr;           // Pointer to initial save pointer area
    bios_U016 BMP_Functionality;         // Pointer to functionality table for Function 1Bh
    bios_U016 BMP_TblClockMNP;           // MNP parameters for clocks

    bios_U016 BMP_OEMString;             // OEM String to identify grahpics controller chip or product family (20)
    bios_U016 BMP_OEMVendorName;         // Name of the vendor that produced the display controller board product (35)
    bios_U016 BMP_OEMProductName;        // Product name of the controller board (35)
    bios_U016 BMP_OEMProductRev;         // Revision of manufacturing level of the display controller board (30)
    bios_U016 BMP_FifoTable;             // Pointer to DAC/CRTC fifo settings table

    bios_U008 BMP_CRTI2CPort;            // Default I2C port for CRT
    bios_U008 BMP_TVI2CPort;             // Default I2C port for TV
    bios_U008 BMP_DFPI2CPort;            // Default I2C port for DFP
    bios_U008 BMP_RsvdI2CPort;           // Reserved for future expansion
    bios_U016 BMP_I2CPort0IO;            // Read/Write reg for I2C Port 0
    bios_U016 BMP_I2CPort1IO;            // Read/Write reg for I2C Port 1

    bios_U008 BMP_OEMRevision;           // OEM Version Number (00 = Generic)
    bios_U016 BMP_LCDEDID;               // Pointer to LCD_EDID
    bios_U016 BMP_FPParams;              // Pointer to FP_Params
    
} BMP_Struc_Rev5, *PBMP_Struc_Rev5;

#define BMP_INIT5_0x10_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "6w" "2w" "5w" "5w" "4b2w" "1b2w" "2d" "7w"
typedef struct
{
    BMP_Control_Block bmpCtrlBlk;       // BMP control block

    bios_U008 BIOS_Checksum;            // Checksum byte for BIOS
    bios_U008 BIOS_Features;            // BIOS Feature Byte (bitfields)
    bios_U032 BIOS_Version;             // BIOS Version in BCD (Major/Minor/Sub)
    bios_U032 BIOS_DriverInfo;          // BIOS Version in BCD (Major/Minor/Sub)
                            
    bios_U016 BMP_GenInitTable;         // Offset to the General Initialization Table.
    bios_U016 BMP_ExtraInitTable;       // Offset to Extra Initialization Table.
    bios_U016 BMP_SizeInitTable;        // Max size of Initialization Table.
    bios_U016 BMP_MemInitTable;         // Offset to the Memory Initialization Table.
    bios_U016 BMP_SDRSequenceTable;     // Offset to the SDR Sequence Table.
    bios_U016 BMP_DDRSequenceTable;     // Offset to the DDR Sequence Table.

    bios_U016 szSignonMsg;              // Offset to signon string
    bios_U016 BMP_SizeSignonMsg;        // Current length of signon message
                           
    bios_U016 BMP_Fonts;                // Pointer to pointers to fonts
    bios_U016 BMP_FontSize;             // Size of memory available for all fonts
    bios_U016 BMP_InitSavePtr;          // Pointer to initial save pointer area
    bios_U016 BMP_Functionality;        // Pointer to functionality table for Function 1Bh
    bios_U016 BMP_TblClockMNP;          // MNP parameters for clocks

    bios_U016 BMP_OEMString;            // OEM String to identify grahpics controller chip or product family (20)
    bios_U016 BMP_OEMVendorName;        // Name of the vendor that produced the display controller board product (35)
    bios_U016 BMP_OEMProductName;       // Product name of the controller board (35)
    bios_U016 BMP_OEMProductRev;        // Revision of manufacturing level of the display controller board (30)
    bios_U016 BMP_FifoTable;            // Pointer to DAC/CRTC fifo settings table

    bios_U008 BMP_CRTI2CPort;           // Default I2C port for CRT
    bios_U008 BMP_TVI2CPort;            // Default I2C port for TV
    bios_U008 BMP_DFPI2CPort;           // Default I2C port for DFP
    bios_U008 BMP_RsvdI2CPort;          // Reserved for future expansion
    bios_U016 BMP_I2CPort0IO;           // Read/Write reg for I2C Port 0
    bios_U016 BMP_I2CPort1IO;           // Read/Write reg for I2C Port 1

    bios_U008 BMP_OEMRevision;          // OEM Version Number (00 = Generic)
    bios_U016 BMP_LCDEDID;              // Pointer to LCD_EDID
    bios_U016 BMP_FPParams;             // Pointer to FP_Params
    
    bios_U032 BMP_FmaxVco;              // Maximum internal PLL frequency
    bios_U032 BMP_FminVco;              // Minimum internal PLL frequency

    bios_U016 BMP_InitScriptTablePtr;       // Init script table pointer
    bios_U016 BMP_MacroIndexTablePtr;       // Macro index table pointer
    bios_U016 BMP_MacroTablePtr;            // Macro table pointer
    bios_U016 BMP_ConditionTablePtr;        // Condition table pointer
    bios_U016 BMP_IOConditionTablePtr;      // IO Condition table pointer
    bios_U016 BMP_IOFlagConditionTablePtr;  // IO Flag Condition table pointer
    bios_U016 BMP_InitFunctionTablePtr;     // Init function table pointer

} BMP_Struc_Rev5_0x10, *PBMP_Struc_Rev5_0x10;


#define BMP_INIT5_0x11_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "6w" "2w" "5w" "5w" "4b2w" "1b2w" "2d" "7w" "8w" "1w"
typedef struct
{
    BMP_Control_Block bmpCtrlBlk;       // BMP control block

    bios_U008 BIOS_Checksum;            // Checksum byte for BIOS
    bios_U008 BIOS_Features;            // BIOS Feature Byte (bitfields)
    bios_U032 BIOS_Version;             // BIOS Version in BCD (Major/Minor/Sub)
    bios_U032 BIOS_DriverInfo;          // BIOS Version in BCD (Major/Minor/Sub)
                            
    bios_U016 BMP_GenInitTable;         // Offset to the General Initialization Table.
    bios_U016 BMP_ExtraInitTable;       // Offset to Extra Initialization Table.
    bios_U016 BMP_SizeInitTable;        // Max size of Initialization Table.
    bios_U016 BMP_MemInitTable;         // Offset to the Memory Initialization Table.
    bios_U016 BMP_SDRSequenceTable;     // Offset to the SDR Sequence Table.
    bios_U016 BMP_DDRSequenceTable;     // Offset to the DDR Sequence Table.

    bios_U016 szSignonMsg;              // Offset to signon string
    bios_U016 BMP_SizeSignonMsg;        // Current length of signon message
                           
    bios_U016 BMP_Fonts;                // Pointer to pointers to fonts
    bios_U016 BMP_FontSize;             // Size of memory available for all fonts
    bios_U016 BMP_InitSavePtr;          // Pointer to initial save pointer area
    bios_U016 BMP_Functionality;        // Pointer to functionality table for Function 1Bh
    bios_U016 BMP_TblClockMNP;          // MNP parameters for clocks

    bios_U016 BMP_OEMString;            // OEM String to identify grahpics controller chip or product family (20)
    bios_U016 BMP_OEMVendorName;        // Name of the vendor that produced the display controller board product (35)
    bios_U016 BMP_OEMProductName;       // Product name of the controller board (35)
    bios_U016 BMP_OEMProductRev;        // Revision of manufacturing level of the display controller board (30)
    bios_U016 BMP_FifoTable;            // Pointer to DAC/CRTC fifo settings table

    bios_U008 BMP_CRTI2CPort;           // Default I2C port for CRT
    bios_U008 BMP_TVI2CPort;            // Default I2C port for TV
    bios_U008 BMP_DFPI2CPort;           // Default I2C port for DFP
    bios_U008 BMP_RsvdI2CPort;          // Reserved for future expansion
    bios_U016 BMP_I2CPort0IO;           // Read/Write reg for I2C Port 0
    bios_U016 BMP_I2CPort1IO;           // Read/Write reg for I2C Port 1

    bios_U008 BMP_OEMRevision;          // OEM Version Number (00 = Generic)
    bios_U016 BMP_LCDEDID;              // Pointer to LCD_EDID
    bios_U016 BMP_FPParams;             // Pointer to FP_Params
    
    bios_U032 BMP_FmaxVco;              // Maximum internal PLL frequency
    bios_U032 BMP_FminVco;              // Minimum internal PLL frequency

    bios_U016 BMP_InitScriptTablePtr;       // Init script table pointer
    bios_U016 BMP_MacroIndexTablePtr;       // Macro index table pointer
    bios_U016 BMP_MacroTablePtr;            // Macro table pointer
    bios_U016 BMP_ConditionTablePtr;        // Condition table pointer
    bios_U016 BMP_IOConditionTablePtr;      // IO Condition table pointer
    bios_U016 BMP_IOFlagConditionTablePtr;  // IO Flag Condition table pointer
    bios_U016 BMP_InitFunctionTablePtr;     // Init function table pointer

    bios_U016 BMP_TMDSSingleAPtr;           // TMDS single link A table
    bios_U016 BMP_TMDSSingleBPtr;           // TMDS single link B table
    bios_U016 BMP_TMDSDualPtr;              // TMDS dual link table
    bios_U016 BMP_LVDSSingleAPtr;           // LVDS single link A table
    bios_U016 BMP_LVDSSingleBPtr;           // LVDS single link B table
    bios_U016 BMP_LVDSDualPtr;              // LVDS dual link table
    bios_U016 BMP_OffSingleAPtr;            // LVDS dual link table
    bios_U016 BMP_OffSingleBPtr;            // LVDS dual link table

    bios_U016 BMP_FPTablePtr;               // Pointer to internal LCD tables

} BMP_Struc_Rev5_0x11, *PBMP_Struc_Rev5_0x11;

// init code instruction formats

#define INIT_FMT1_FMT "1b3d"
typedef struct
{
    bios_U008 opcode;
    bios_U032 operand1;
    bios_U032 operand2;
    bios_U032 operand3;

} BMP_FORMAT_1;


#define INIT_FMT2_FMT  "1b2d"
typedef struct
{
    bios_U008 opcode;
    bios_U032 operand1;
    bios_U032 operand2;
} BMP_FORMAT_2;


#define INIT_FMT3_FMT "1b1d1w"
typedef struct
{
    bios_U008 opcode;
    bios_U032 operand1;
    bios_U016 operand2;
} BMP_FORMAT_3;


#define INIT_FMT4_FMT "1b1w"
typedef struct
{
    bios_U008 opcode;
    bios_U016 operand1;
} BMP_FORMAT_4;

#define INIT_FMT5_FMT "1b1w3b"
typedef struct
{
    bios_U008 opcode;
    bios_U016 operand1;
    bios_U008 operand2;
    bios_U008 operand3;
    bios_U008 operand4;
} BMP_FORMAT_5;


#define INIT_FMT6_FMT "3b"
typedef struct
{
    bios_U008 opcode;
    bios_U008 operand1;
    bios_U008 operand2;
} BMP_FORMAT_6;


#define INIT_FMT7_FMT "1b"
typedef struct
{
    bios_U008 opcode;
} BMP_FORMAT_7;


#define INIT_FMT8_FMT "2b"
typedef struct
{
    bios_U008 opcode;
    bios_U008 operand1;
} BMP_FORMAT_8;


#define INIT_FMT9_FMT "1b1w2b"
typedef struct
{
    bios_U008 opcode;
    bios_U016 operand1;
    bios_U008 operand2;
    bios_U008 operand3;
} BMP_FORMAT_9;

#define INIT_FMT10_FMT "1b1w1b"
typedef struct
{
    bios_U008 opcode;
    bios_U016 operand1;
    bios_U008 operand2;
} BMP_FORMAT_10;

#define INIT_FMT11_FMT "1b1d2b1w2b"
typedef struct
{
    bios_U008 opcode;
    bios_U032 operand1;
    bios_S008 operand2;
    bios_U008 operand3;
    bios_U016 operand4;
    bios_U008 operand5;
    bios_U008 operand6;
} BMP_FORMAT_11;

#define INIT_FMT12_FMT "1b2d2bd"
typedef struct
{
    bios_U008 opcode;
    bios_U032 operand1;
    bios_U032 operand2;
    bios_U008 operand3;
    bios_U008 operand4;
    bios_U032 operand5;
    // instruction followed by a U032 array
} BMP_FORMAT_12;

#define INIT_FMT13_FMT "1b1w4bd"
typedef struct
{
    bios_U008 opcode;
    bios_U016 operand1;
    bios_U008 operand2;
    bios_U008 operand3;
    bios_U008 operand4;
    bios_U008 operand5;
    bios_U032 operand6;
    // instruction followed by a U032 array
} BMP_FORMAT_13;

#define INIT_FMT14_FMT "1b1w5b1d"
typedef struct
{
    bios_U008 opcode;
    bios_U016 operand1;
    bios_U008 operand2;
    bios_U008 operand3;
    bios_U008 operand4;
    bios_U008 operand5;
    bios_U008 operand6;
    bios_U032 operand7;
    // instruction followed by a U016 array
} BMP_FORMAT_14;

#define INIT_FMT15_FMT "1b4d1b"
typedef struct
{
    bios_U008 opcode;
    bios_U032 operand1;
    bios_U032 operand2;
    bios_U032 operand3;
    bios_U032 operand4;
    bios_U008 operand5;
    // instruction followed by a U008 array
} BMP_FORMAT_15;


// biggest simple BMP item should be no bigger than this..
#define BMP_MAX_PADDED_SIZE 256


// memory initialization structs

#define MEM_INIT_STRAP_MAX          0xf        // the *max* value, so can have 16
#define MEM_INIT_SEQ_TABLE_MAX      32
#define SDR                         0
#define DDR                         1
#define MEM_INIT_END_OF_SEQUENCE    0xffffffff
#define SPECIAL_32                  0x00000001 // Mask for SPECIAL_32 bit 
#define SCRAMBLE_RAM                0x00000080 // Mask for scrambling the RAM
#define NV_PFB_CONFIG_0_SCRAMBLE    29:29 /* RWIVF */

/* SDR and DDR Sequence Table definitions */
#define MEM_SEQ_FMT "32d"


typedef struct
{
    bios_U032 PFB_BOOT_0_value;
    bios_U032 PFB_CONFIG_1_value;

} bmpMemInitData;

// NV10 memory init structure
/*
    _Flags          dw      ?               ; Bit0: 0 = SDR, 1 = DDR
    _MClk_PLL       dw      ?
    _NVClk_PLL      dw      ?
    _refctrl1       dd      NV_PFB_REFCTRL_VALID_0  ; NV_PFB_REFCTRL
    _cfg            dd      ?               ; NV_PFB_CFG
    _timing0        dd      ?               ; NV_PFB_TIMING0
    _timing1        dd      ?               ; NV_PFB_TIMING1
    _timing2        dd      ?               ; NV_PFB_TIMING2
    _ctrim4         dd      0
    _pin            dd      NV_PFB_PIN_CKE_NORMAL OR NV_PFB_PIN_DQM_NORMAL
    _emrs           dd      ?               ; NV_PFB_EMRS
    _mrs1           dd      ?               ; NV_PFB_MRS
    _mrs2           dd      ?               ; NV_PFB_MRS
    _refctrl2       dd      NV_PFB_REFCTRL_VALID_1  ; NV_PFB_REFCTRL
    _expand1        dd      0
    _expand2        dd      0
*/


#define MEM_INIT_VALS_NV10_FMT           "3w13d"
typedef struct
{
    bios_U016 Flags;         // Bit0: 0 = SDR, 1 = DDR
    bios_U016 MClk_PLL;
    bios_U016 NVClk_PLL;
    bios_U032 data[13];

} MEM_INIT_VALS_NV10;

// NV15 memory init structure
/*
    _Flags          dw      ?               ; Bit0: 0 = SDR, 1 = DDR
    _MClk_PLL       dw      ?
    _NVClk_PLL      dw      ?
    _pbus_debug_0   dd      ?
    _refctrl1       dd      NV_PFB_REFCTRL_VALID_0  ; NV_PFB_REFCTRL
    _cfg            dd      ?               ; NV_PFB_CFG
    _timing0        dd      ?               ; NV_PFB_TIMING0
    _timing1        dd      ?               ; NV_PFB_TIMING1
    _timing2        dd      ?               ; NV_PFB_TIMING2
    _ctrim4         dd      06530110h
    _ctrim5         dd      0777FFF0h
    _pin            dd      NV_PFB_PIN_CKE_NORMAL OR NV_PFB_PIN_DQM_NORMAL
    _emrs           dd      ?               ; NV_PFB_EMRS
    _mrs1           dd      ?               ; NV_PFB_MRS
    _mrs2           dd      ?               ; NV_PFB_MRS
    _refctrl2       dd      NV_PFB_REFCTRL_VALID_1  ; NV_PFB_REFCTRL
    _expand1        dd      0FFFFFFFFh
    _expand2        dd      0FFFFFFFFh
*/


#define MEM_INIT_VALS_NV15_FMT           "3w15d"
typedef struct
{
    bios_U016 Flags;         // Bit0: 0 = SDR, 1 = DDR
    bios_U016 MClk_PLL;
    bios_U016 NVClk_PLL;
    bios_U032 data[15];
} MEM_INIT_VALS_NV15;

// generic memory init types
typedef MEM_INIT_VALS_NV10 MEM_INIT_TABLE_NV10[MEM_INIT_STRAP_MAX + 1];
typedef MEM_INIT_VALS_NV15 MEM_INIT_TABLE_NV15[MEM_INIT_STRAP_MAX + 1];

#define MEM_INIT_VALS_MAX_BYTE_SIZE 80

#define BMP_I2C_FMT "2d" "4b" "2w"
typedef struct
{
    bios_U032 Reserved1;
    bios_U032 Reserved2;

    bios_U008 CRTI2CPort;
    bios_U008 TVI2CPort;
    bios_U008 DFPI2CPort;
    bios_U008 Reserved3;

    bios_U016 I2CPort0IO;
    bios_U016 I2CPort1IO;

} BIOSI2CTable, *PBIOSI2CTable;

// BMP script

// script table
typedef bios_U016 SCRIPT_TABLE_ENTRY;

// Macro Index Table Entry structure
#define MACRO_INDEX_ENTRY_FMT  "bb"
typedef struct
{
    bios_U008 macroIndex;
    bios_U008 macroCount;
} MACRO_INDEX_ENTRY;

// Macro Table Entry structure
#define MACRO_ENTRY_FMT  "dd"
typedef struct
{
    bios_U032 macroAddress;
    bios_U032 macroValue;
} MACRO_ENTRY;

// Condition Table Entry structure
#define CONDITION_ENTRY_FMT "3d"
typedef struct
{
    bios_U032 condAddress;
    bios_U032 condMask;
    bios_U032 condCompare;
} CONDITION_ENTRY;

// I/O Condition Table Entry structure
#define NO_CONDITION 0xff
#define IO_CONDITION_ENTRY_FMT "wbbb"
typedef struct
{
    bios_U016 iocondPort;
    bios_U008 iocondIndex;
    bios_U008 iocondMask;
    bios_U008 iocondCompare;
} IO_CONDITION_ENTRY;

// I/O Flag Condition Table Entry structure
#define IO_FLAG_CONDITION_ENTRY_FMT         "wbbbwbb"
typedef struct
{
    bios_U016 iofcondPort;
    bios_U008 iofcondIndex;
    bios_U008 iofcondMask;
    bios_U008 iofcondShift;
    bios_U016 iofcondFlagArray;
    bios_U008 iofcondFlagMask;
    bios_U008 iofcondFlagCompare;
} IO_FLAG_CONDITION_ENTRY;

// Function Table Entry structure
#define FUNCTION_ENTRY_FMT "w"
typedef bios_U016 FUNCTION_TABLE_ENTRY;

#define TMDS_MODE_FORMAT        "1w1b"
typedef struct
{
    bios_U016 tmdsmFreq;
    bios_U008 tmdsmScript;
} TMDS_MODE_ENTRY;


#define DEV_HEAD_FMT "1b1b1w"
typedef struct
{
    bios_U008 version;
    bios_U008 blocks;
    bios_U016 i2c_port_list;
} DEV_HEAD, *PDEV_HEAD;


#define DEV_REC_STRUC_FMT "1d1w1d"
typedef struct
{
    bios_U032 dev_type;
    bios_U016 data_ptr;
    bios_U032 data_rsvr;
} DEV_REC_STRUC, *PDEV_REC_STRUC;

#define DEV_REC_UNUSED_DEV      0xFFFFFFFF

/*
DEV_REC    RECORD  rec_rsvd:4=0, \
                   dbus:3=0, \
                   location:4=0, \
                   head:3=0, \
                   i2c:4=0, \
                   format:6=0, \
                   subtype:4=0, \
                   dtype:4=0
*/

// DEV_REC { X, X, DEV_REC_LOCATION_BOARD=1, Head, X, X, X, DEV_REC_TYPE_CRT=0}                            
// mask:  0000 000 1111 111 0000 000000 0000 1111
// value: 0000 000 0001 hhh 0000 000000 0000 0000

#define DEV_REC_CRT_DEV_MASK    0x01FC000F
#define DEV_REC_CRT_DEV_EXTERNAL_DAC 0x00200000
#define DEV_REC_HEAD_SHIFT      18

#pragma pack()

RM_STATUS BiosReadBytes            (PHWINFO, U008[], U032 offset, U032 size);
RM_STATUS BiosCalculateSizes       (const char *, U032 *, U032 *);
RM_STATUS BiosUnpackLittleEndianStructure(U008 *packedLEData, U032 *unpackedData, const char *format, U032 *numfields, U032 *unpacked_bytes);
RM_STATUS BiosReadStructure        (PHWINFO, void *, U032, U032*, const char *);
RM_STATUS BiosReadArray            (PHWINFO, void *, U032, U032, U032*, const char *);
U008 BiosRead8                     (PHWINFO, U032);
U016 BiosRead16                    (PHWINFO, U032);
U032 BiosRead32                    (PHWINFO, U032);
RM_STATUS BiosFindString           (PHWINFO, U008 string[], U032*, U032 string_size);
RM_STATUS BiosGetMaxExternalPCLKFreq(PHWINFO, U032 head, U032* freq);
