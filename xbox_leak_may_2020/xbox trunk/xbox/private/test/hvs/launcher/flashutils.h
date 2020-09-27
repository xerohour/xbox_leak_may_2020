/*****************************************************
*** flashutils.h
***
*** Header file for our Flash utilities library
*** These functions will allow the kernel of the
*** Xbox to be Flashed.
***
*** by James N. Helm
*** January 21st, 2002
***
*****************************************************/

#ifndef _FLASHUTILS_H_
#define _FLASHUTILS_H_

#include <mm.h>
#include <pci.h>

//
// Manufacturer and chip identifiers
//

#define SST_ID                  0xBF
#define SST_49LF040             0x53

#define AMD_ID                  0x01
#define FUJITSU_ID              0x04
#define HYUNDAI_ID              0xAD
#define GENERIC_29F080          0xD5

#define ST_ID                   0x20
#define M29F080A                0xF1


typedef enum {
    FlashUnknownDevice = 0,
    FlashSST49LF040,
    FlashGeneric29F080
} FlashDeviceID;

typedef enum {
    FlashStatusReady,
    FlashStatusBusy,
    FlashStatusEraseSuspended,
    FlashStatusTimeout,
    FlashStatusError
} FlashStatus;

// SIZE_T
// FlashSize(
//     IN FlashDeviceID ID
//     )
// ++
//
// Routine Description:
//
//     This macro returns size of flash ROM specified by device identifier id
//
// Arguments:
//
//     ID - Device identifier returned by FlashDetectDevice
//
// Return Value:
//
//     Size of flash ROM for specified device
//
// --

#define FlashSize( ID )         FlashSizeMap[(ID)]

// ULONG_PTR
// FlashBaseAddress(
//     IN FlashDeviceID ID
//     )
// ++
//
// Routine Description:
//
//     This macro calculates the base address of flash ROM specified by ID,
//     based on the size of the chip
//
// Arguments:
//
//     ID - Device identifier returned by FlashDetectDevice
//
// Return Value:
//
//     Base address of flash ROM from top 4GB
//
// --

#define FlashBaseAddress( ID )  ((ULONG_PTR)(0xFFFFFFFF-FlashSize(ID)+1))

//
// Lowest possible base address of flash ROM and region size
//

#define FLASH_BASE_ADDRESS      0xFFF00000
#define FLASH_REGION_SIZE       (0xFFFFFFFF-FLASH_BASE_ADDRESS-1)


////////////
// Macros
////////////

// BYTE
// FlashReadByte(
//     IN ULONG_PTR Physical
//     )
// ++
//
// Routine Description:
//
//     This macro maps specified physical address of flash ROM into mapped
//     virtual address and reads one byte from mapped address.
//
// Arguments:
//
//     Physical - Physical address of flash ROM to be read
//
// Return Value:
//
//     A read byte from specified address
//
// --

#define FlashReadByte(a) \
    (*(PBYTE)((ULONG_PTR)KernelRomBase+(ULONG_PTR)(a)-FLASH_BASE_ADDRESS))

// VOID
// FlashWriteByte(
//     IN ULONG_PTR Physical,
//     IN BYTE Byte
//     )
// ++
//
// Routine Description:
//
//     This macro maps specified physical address of flash ROM into mapped
//     virtual address and writes one byte to mapped address.
//
// Arguments:
//
//     Physical - Physical address of flash ROM to be read
//
//     Byte - Data to be written to
//
// Return Value:
//
//     None
//
// --

#define FlashWriteByte(a, d) \
    (*(PBYTE)((ULONG_PTR)KernelRomBase+(ULONG_PTR)(a)-FLASH_BASE_ADDRESS) = d)

// WORD
// FlashReadWord(
//     IN ULONG_PTR Physical
//     )
// ++
//
// Routine Description:
//
//     This macro maps specified physical address of flash ROM into mapped
//     virtual address and reads two bytes from mapped address.
//
// Arguments:
//
//     Physical - Physical address of flash ROM to be read
//
// Return Value:
//
//     Two byte from specified address
//
// --

#define FlashReadWord(a) \
    (*(PWORD)((ULONG_PTR)KernelRomBase+(ULONG_PTR)(a)-FLASH_BASE_ADDRESS))


////////////
// Functions
////////////
FlashStatus FlashGetStatus( IN FlashDeviceID ID,       // This routine checks status of flash chip using data# polling method.
                            IN ULONG_PTR Address OPTIONAL,
                            IN BYTE Data );
VOID FlashResetDevice( VOID );                          // This routine resets flash ROM back to read mode if device is in ID
                                                        //   command mode or during a program or erase operation
FlashDeviceID FlashDetectDevice( VOID );                // Detects the device and manufacturer id of flash device on the system
BOOL FlashEraseChip( FlashDeviceID ID );                // This routine erase the content of entire flash ROM to 0xFF.
BOOL FlashProgramImage( IN FlashDeviceID ID,            // This routine programs the content of flash ROM with new image.
                        IN PVOID ImageBuffer );
UINT64 FASTCALL FlashReadMSR( IN ULONG Address );       // Reads Pentium III Model-Specific Register (MSR) specified by Address
VOID FASTCALL FlashWriteMSR( IN ULONG Address,          // Writes Pentium III Model-Specific Register (MSR) specified by Address
                             IN UINT64 Value );
VOID FlashChangeRomCaching( BOOL EnableCache );         // Adjusts caching to support ROM flashing
HRESULT FlashKernelImage( IN  PVOID  ImageBuffer,       // Flash the kernel image to the Xbox
                          IN  SIZE_T ImageSize,
                          OUT LPSTR  szResp,
                          IN  DWORD  cchResp );


#endif // _FLASHUTILS_H_