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


//KDA buffer definitions -- describes layout of a shared memory region
// in the frame buffer that is used for the RM and VBIOS to exchange info
// General KDA attributes
//#define KDA_BUFFER_ENABLED  //Temporary compile option: Turn on or off KDA buffer.

#define KDA_MEM_SIZE                    0x5000 //0x11000  //total size of buffer
#define KDA_HARDWIRED_PTR_LOC           0xFFF0  //hardwired location, in 64K bios block of the ptr to the buffer

// KDA header information
#define KDA_HEADER_KEY_LOC              0x00    //offset from start of buffer
#define KDA_HEADER_REV_LOC              0x08    //offset from start of buffer
#define KDA_HEADER_BUFFER_SIZE_LOC      0x0C    //offset from start of buffer
#define KDA_HEADER_HEADER_SIZE_LOC      0x10    //offset from start of buffer
#define KDA_HEADER_VBIOS_FLAGS_LOC      0x14    //offset from start of buffer
#define KDA_HEADER_DRIVER_FLAGS_LOC     0x18    //offset from start of buffer
#define KDA_HEADER_NUM_CELLS_LOC        0x1C    //offset from start of buffer
#define KDA_BUFFER_CELL_START           0x40    //offset from start of buffer
#define KDA_BUFFER_BUSY_MASK            0x0001   //bit 0, both in vbios and driver flags
//#define KDA_VBIOS_FLAGS     0x14 //0   //offset in buffer
//#define KDA_DRIVER_FLAGS    0x18 //4   //offset in buffer
//#define KDA_MEM_BUSY        1   //bit 0, both in vbios and driver flags

// General cell attributes
#define KDA_CELL_TYPE_LOC               0x00    //offset from start of cell
#define KDA_CELL_TYPE_SCRATCH           0       //scratch space for VBIOS
#define KDA_CELL_TYPE_HW_ICON           1       //HW icon
#define KDA_CELL_TYPE_EDID_BUF          2       //EDID buffer
#define KDA_CELL_TYPE_APM_RTN           3       //APM helper routine

#define KDA_CELL_SIZE_LOC               0x04    //offset from start of cell

// Cell type 1 (cursor data for HW icon)
#define KDA_CELL_HWICON_HEAD_LOC              0x10    //offset from start of cell
#define KDA_CELL_HWICON_VBIOS_FLAGS_LOC       0x14    //offset from start of cell
#define KDA_CELL_HWICON_DRIVER_FLAGS_LOC      0x18    //offset from start of cell
#define KDA_CELL_HWICON_BUSY_MASK             0x0001   //bit 0, both in vbios and driver flags
//


// Hotkey bit definitions -- for test purposes.

// These bit definitions refer to CRTC register 4A, aka NV_CIO_CRE_SCRATCH5__INDEX.
#define NV_MOBILE_FLAG_DISPLAY_CHANGING 0x01    //SSF1
#define NV_MOBILE_FLAG_SBIOS_CRIT_SEC   0x02    //SSF2
#define NV_MOBILE_FLAG_DISPLAY_MASK     0x04    //SSF3
#define NV_MOBILE_FLAG_SCALE_MASK       0x10    //SSF5

// These bit definitions refer to CRTC register 4B, aka NV_CIO_CRE_SCRATCH6__INDEX.
#define NV_DRIVER_HANDLES_HOTKEYS       0x80    //SOS8
#define NV_IS_MOBILE_BIOS               0x40    //SOS7
#define NV_EXTENDED_DESKTOP             0x20    //SOS6
#define NV_CLONE_MODE                   0x10    //SOS5
#define NV_SPANNING_MODE                0x08    //SOS4
#define NV_CORRECT_ARG_ORDER            0x01    //Correct for Toshiba bug in shipping SBIOS.

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
#define BMP_INIT_TABLE_SUBVER_0x11  0x11
#define BMP_INIT_TABLE_SUBVER_0x12  0x12
#define BMP_INIT_TABLE_SUBVER_0x13  0x13
#define BMP_INIT_TABLE_SUBVER_0x14  0x14


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

#define BMP_INIT5_0x12_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "6w" "2w" "5w" "5w" "4b2w" "1b2w" "2d" "7w" "8w" "1w" "1w" "2w" "1w"
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
    bios_U016 BMP_FPXlateTablePtr;          // Pointer to internal LCD translate table

    bios_U016 BMP_Int15CallbacksPost;       // INT15 POST Callbacks
    bios_U016 BMP_Int15CallbacksSystem;     // INT15 SYSTEM Callbacks

    bios_U016 BMP_Display_Config_Block;     // Legacy display config block

} BMP_Struc_Rev5_0x12, *PBMP_Struc_Rev5_0x12;

#define BMP_INIT5_0x13_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "6w" "2w" "5w" "5w" "4b2w" "1b2w" "2d" "7w" "8w" "1w" "1w" "2w" "1w" "1w"
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
    bios_U016 BMP_FPXlateTablePtr;          // Pointer to internal LCD translate table

    bios_U016 BMP_Int15CallbacksPost;       // INT15 POST Callbacks
    bios_U016 BMP_Int15CallbacksSystem;     // INT15 SYSTEM Callbacks

    bios_U016 BMP_Display_Config_Block;     // Legacy display config block

    bios_U016 BMP_DrvSlwParms_Block;        // Drive/slew parameters block

} BMP_Struc_Rev5_0x13, *PBMP_Struc_Rev5_0x13;


//#define BMP_INIT5_0x14_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "6w" "2w" "5w" "5w" "4b2w" "1b2w" "2d" "7w" "8w" "1w" "1w" "2w" "1w" "1w" "3w"
#define BMP_INIT5_0x14_FMT BMP_CONTROL_BLOCK_FMT "2b2d" "6w" "2w" "5w" "5w" "4b2w" "1b2w" "2d" "7w" "8w" "2w" "2w" "1w" "1w" "3w"
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
    bios_U016 BMP_FPXlateTablePtr;          // Pointer to internal LCD translate table

    bios_U016 BMP_Int15CallbacksPost;       // INT15 POST Callbacks
    bios_U016 BMP_Int15CallbacksSystem;     // INT15 SYSTEM Callbacks

    bios_U016 BMP_Display_Config_Block;     // Legacy display config block

    bios_U016 BMP_DrvSlwParms_Block;        // Drive/slew parameters block

    bios_U016 BMP_LVDSManufacturerPtr;      // pointer to internal LCD Manufacturer table
    bios_U016 BMP_FPXlateManufacturerTablePtr;  // pointer to internal LCD Manufacturer translate Table
    bios_U016 BMP_FPXlateFlagTablePtr;      // pointer to internal LCD translate Flag Table
} BMP_Struc_Rev5_0x14, *PBMP_Struc_Rev5_0x14;


// Manufacturer's Table Header
#define INIT_MANUFACTURER_TABLE_HEADER_FORMAT "2b"
typedef struct {
    bios_U008 Version;            // Version of the block
    bios_U008 EntrySize;          // Size of each entry ( to allow fields to be added without breaking the old drivers )
} PANEL_MANUFACTURER_TABLE_HEADER;

#define INIT_MANUFACTURER_TABLE_ENTRY_VER10_FORMAT "7b" "1w"
typedef struct {
    bios_U008 Flags;              // Script processing flags
#define PANEL_POWER_OFF_REQUIRED  0x1 //   bit 0   : 1=power off required for reprogramming
#define PANEL_RESET_REQUIRED      0x2 //   bit 1   : 1=reset required after PCLK change
                                      //   bit 2-7 : reserved
    bios_U008 LVDSInitScript;     // Panel link initialization script index
    bios_U008 LVDSResetScript;    // Panel link reprogram script index 
                                  //   (only needed if Flags[1]=1
    bios_U008 BackLightOnScript;  // Back light ON  single link B table
    bios_U008 BackLightOffScript; // Back light OFF single link A table
    bios_U008 PanelOnScript;      // Panel and backlight ON single link A table
    bios_U008 PanelOffScript;     // Panel and backlight OFF single link A table
    bios_U016 PanelOffOnDelay;    // Minimum delay between panel off and panel on in 
                                  //   milliseconds (NOT microseconds like the scripts)
} PANEL_MANUFACTURER_TABLE_ENTRY_VER10;


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

#define BMP_BPP_FMT "4b"
typedef struct
{
    bios_U008 Red;
    bios_U008 Green;
    bios_U008 Blue;
    bios_U008 Reserved;
}BIOSBPP, *PBIOSBPP;

#define BMP_FP_PWM_FMT "4b"
typedef struct
{
   bios_U008 Frequency;
   bios_U008 MinFreq; 
   bios_U008 MaxFreq;
   bios_U008 PWMRsvd;
}BIOSFPPWM, *PBIOSFPPWM;

#define BMP_FP_TABLE_SIZE 0x2C //in bytes
#define BMP_FP_TABLE_FMT "1b1w" BMP_BPP_FMT "1w" "7w" "7w" "1b1w" BMP_FP_PWM_FMT
typedef struct
{
    bios_U008 Table_Rev;
    bios_U016 FPType;
    BIOSBPP   RGB_Bits;

    bios_U016 PixClk;
    
    bios_U016 HActive;
    bios_U016 HDispEnd;
    bios_U016 HValidStart;
    bios_U016 HValidEnd;
    bios_U016 HSyncStart;
    bios_U016 HSyncEnd;
    bios_U016 HTotal;

    bios_U016 VActive;
    bios_U016 VDispEnd;
    bios_U016 VValidStart;
    bios_U016 VValidEnd;
    bios_U016 VSyncStart;
    bios_U016 VSyncEnd;
    bios_U016 VTotal;

    bios_U008 Flags;
    bios_U016 Spread;
    BIOSFPPWM PWM;
}BIOSFPTable, *PBIOSFPTable;

#define BMP_FP_XLATE_TABLE_FMT "16b"

// NV20 drive/slew parameter table
/*        public  pDrvSlwParms
 pDrvSlwParms    label   byte
    db     10h        ; Version number
    dw     20000      ; MaxMClk (200Mhz)
    db     100        ; SlowCountAvg
    db     108        ; TypiCount
    db     116        ; FastCountExt
    db     02h        ; FbioInterpMode

    db     0Ah        ; AdrDrvFallFastAvg
    db     08h        ; AdrDrvRiseFastAvg
    db     0Bh        ; AdrSlwFallFastAvg
    db     08h        ; AdrSlwRiseFastAvg

    db     0Ah        ; ClkDrvFallFastAvg
    db     08h        ; ClkDrvRiseFastAvg
    db     0Bh        ; ClkSlwFallFastAvg
    db     08h        ; ClkSlwRiseFastAvg

    db     09h        ; DatDrvFallFastAvg
    db     09h        ; DatDrvRiseFastAvg
    db     0Eh        ; DatSlwFallFastAvg
    db     08h        ; DatSlwRiseFastAvg

    db     09h        ; DqsDrvFallFastAvg
    db     09h        ; DqsDrvRiseFastAvg
    db     0Eh        ; DqsSlwFallFastAvg
    db     08h        ; DqsSlwRiseFastAvg

    db     06h        ; DqsInbDelyFastAvg

    db     0Ah        ; AdrDrvFallTypi
    db     08h        ; AdrDrvRiseTypi
    db     0Bh        ; AdrSlwFallTypi
    db     08h        ; AdrSlwRiseTypi

    db     0Ah        ; ClkDrvFallTypi
    db     08h        ; ClkDrvRiseTypi
    db     0Bh        ; ClkSlwFallTypi
    db     08h        ; ClkSlwRiseTypi

    db     09h        ; DatDrvFallTypi
    db     09h        ; DatDrvRiseTypi
    db     0Eh        ; DatSlwFallTypi
    db     08h        ; DatSlwRiseTypi

    db     09h        ; DqsDrvFallTypi
    db     09h        ; DqsDrvRiseTypi
    db     0Eh        ; DqsSlwFallTypi
    db     08h        ; DqsSlwRiseTypi

    db     05h        ; DqsInbDelyTypi

    db     0Ch        ; AdrDrvFallSlowAvg
    db     0Bh        ; AdrDrvRiseSlowAvg
    db     0Bh        ; AdrSlwFallSlowAvg
    db     08h        ; AdrSlwRiseSlowAvg

    db     0Ch        ; ClkDrvFallSlowAvg
    db     0Bh        ; ClkDrvRiseSlowAvg
    db     0Bh        ; ClkSlwFallSlowAvg
    db     08h        ; ClkSlwRiseSlowAvg

    db     0Ah        ; DatDrvFallSlowAvg
    db     0Bh        ; DatDrvRiseSlowAvg
    db     0Eh        ; DatSlwFallSlowAvg
    db     08h        ; DatSlwRiseSlowAvg

    db     0Ah        ; DqsDrvFallSlowAvg
    db     0Bh        ; DqsDrvRiseSlowAvg
    db     0Eh        ; DqsSlwFallSlowAvg
    db     08h        ; DqsSlwRiseSlowAvg

    db     04h        ; DqsInbDelySlowAvg
*/
#define BMP_DRVSLW_PARMS "1b" "1w" "58b"
typedef struct
{
    bios_U008 Version;

    bios_U016 MaxMClk;

    bios_U008 SlowCountAvg;
    bios_U008 TypiCount;
    bios_U008 FastCountAvg;
    bios_U008 FbioInterpMode;

    bios_U008 SlowCountBAvg;
    bios_U008 TypiCountB;
    bios_U008 FastCountBAvg;

    bios_U008 AdrDrvFallFastAvg;
    bios_U008 AdrDrvRiseFastAvg;
    bios_U008 AdrSlwFallFastAvg;
    bios_U008 AdrSlwRiseFastAvg;

    bios_U008 ClkDrvFallFastAvg;
    bios_U008 ClkDrvRiseFastAvg;
    bios_U008 ClkSlwFallFastAvg;
    bios_U008 ClkSlwRiseFastAvg;

    bios_U008 DatDrvFallFastAvg;
    bios_U008 DatDrvRiseFastAvg;
    bios_U008 DatSlwFallFastAvg;
    bios_U008 DatSlwRiseFastAvg;

    bios_U008 DqsDrvFallFastAvg;
    bios_U008 DqsDrvRiseFastAvg;
    bios_U008 DqsSlwFallFastAvg;
    bios_U008 DqsSlwRiseFastAvg;

    bios_U008 DqsInbDelyFastAvg;

    bios_U008 AdrDrvFallTypi;
    bios_U008 AdrDrvRiseTypi;
    bios_U008 AdrSlwFallTypi;
    bios_U008 AdrSlwRiseTypi;

    bios_U008 ClkDrvFallTypi;
    bios_U008 ClkDrvRiseTypi;
    bios_U008 ClkSlwFallTypi;
    bios_U008 ClkSlwRiseTypi;

    bios_U008 DatDrvFallTypi;
    bios_U008 DatDrvRiseTypi;
    bios_U008 DatSlwFallTypi;
    bios_U008 DatSlwRiseTypi;

    bios_U008 DqsDrvFallTypi;
    bios_U008 DqsDrvRiseTypi;
    bios_U008 DqsSlwFallTypi;
    bios_U008 DqsSlwRiseTypi;

    bios_U008 DqsInbDelyTypi;

    bios_U008 AdrDrvFallSlowAvg;
    bios_U008 AdrDrvRiseSlowAvg;
    bios_U008 AdrSlwFallSlowAvg;
    bios_U008 AdrSlwRiseSlowAvg;

    bios_U008 ClkDrvFallSlowAvg;
    bios_U008 ClkDrvRiseSlowAvg;
    bios_U008 ClkSlwFallSlowAvg;
    bios_U008 ClkSlwRiseSlowAvg;

    bios_U008 DatDrvFallSlowAvg;
    bios_U008 DatDrvRiseSlowAvg;
    bios_U008 DatSlwFallSlowAvg;
    bios_U008 DatSlwRiseSlowAvg;

    bios_U008 DqsDrvFallSlowAvg;
    bios_U008 DqsDrvRiseSlowAvg;
    bios_U008 DqsSlwFallSlowAvg;
    bios_U008 DqsSlwRiseSlowAvg;

    bios_U008 DqsInbDelySlowAvg;
} BIOSDRVSLWPARMS, *PBIOSDRVSLWPARMS;

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

#define MAX_FP_STRAPS 16
#define SIZE_FP_XLATE_TABLE MAX_FP_STRAPS
#define SIZE_FP_XLATE_FLAG_TABLE MAX_FP_STRAPS
#define SIZE_FP_XLATE_MANUFACTURER_TABLE MAX_FP_STRAPS

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
