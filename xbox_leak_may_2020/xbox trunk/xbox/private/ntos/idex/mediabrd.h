/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    mediabrd.h

Abstract:

    This module contains the definitions and structures for interfacing with a
    Sega media board.

--*/

#ifndef _MEDIABRD_
#define _MEDIABRD_

//
// Define the media board's extended IDE command values.
//

#define MEDIA_BOARD_COMMAND_REGISTER_READ16                 0x50
#define MEDIA_BOARD_COMMAND_REGISTER_WRITE16                0x51
#define MEDIA_BOARD_COMMAND_BOOT_ERASE                      0x52
#define MEDIA_BOARD_COMMAND_BOOT_WRITE                      0x53
#define MEDIA_BOARD_COMMAND_REGISTER_READ32                 0x60
#define MEDIA_BOARD_COMMAND_REGISTER_WRITE32                0x61

//
// Define the starting logical block addresses and sector count of the various
// areas of the media board.
//

#define MEDIA_BOARD_FILE_SYSTEM_AREA_START_LBA              0x0000000
#define MEDIA_BOARD_COMMUNICATION_AREA_ASIC_START_LBA       0x9000000
#define MEDIA_BOARD_COMMUNICATION_AREA_SECTOR_COUNT         0x8000
#define MEDIA_BOARD_BOOT_ROM_START_LBA                      0x8000000
#define MEDIA_BOARD_BOOT_ROM_FPGA_SECTOR_COUNT              0x1000
#define MEDIA_BOARD_BOOT_ROM_ASIC_SECTOR_COUNT              0x2000

//
// Define the static partition numbers.
//

#define MEDIA_BOARD_FILE_SYSTEM_PARTITION                   0
#define MEDIA_BOARD_COMMUNICATION_AREA_PARTITION            2
#define MEDIA_BOARD_BOOT_ROM_PARTITION                      3

#endif  // MEDIABRD
