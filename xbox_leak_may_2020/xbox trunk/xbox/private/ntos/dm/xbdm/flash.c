/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    flash.c

Abstract:

    This module implements function used to program kernel image to flash
    ROM on EVT and DVT machine.

--*/

#include "dmp.h"
#include <pci.h>

#define ROM_VERSION_BYTE_OFFSET 0x78

typedef enum {
    FlashUnknownDevice = 0,
    FlashSST49LF040,
    FlashGeneric29F080,
    FlashGeneric29F040,
    FlashGeneric29F020,
} FlashDeviceID;

typedef struct {
    FlashDeviceID DeviceID;
    WCHAR* DeviceCodes;
} FlashDeviceClass;

typedef enum {
    FlashStatusReady,
    FlashStatusBusy,
    FlashStatusEraseSuspended,
    FlashStatusTimeout,
    FlashStatusError
} FlashStatus;

//
// Table of known flash devices list
//

FlashDeviceClass _FlashDeviceClass[] = {
    { FlashSST49LF040,    L"\xBF53" },
    { FlashGeneric29F080, L"\x01D5\x04D5\xADD5" },
    { FlashGeneric29F040, L"\x1F13\x20E2\xC2A4\xADA4" },
    { FlashGeneric29F020, L"\x1F07\x1F08\x1F0B\x2034\x20B0\xC234\xC2B0\xDA8C" },
};

//
// Mapped virtual address of ROM region
//

PVOID KernelRomBase;

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

#define FlashSize(ID)       FlashSizeMap[(ID)]

SIZE_T FlashSizeMap[] = {
    0,
    512 * 1024,     // SST 49LF040 Firmware Hub
    1024 * 1024,    // Generic 29F080
    512 * 1024,     // Generic 29F040
    256 * 1024,     // Generic 29F020
};

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

#define FlashBaseAddress(ID)    ((ULONG_PTR)(0xFFFFFFFF-FlashSize(ID)+1))

//
// Lowest possible base address of flash ROM and region size
//

#define FLASH_BASE_ADDRESS      0xFFF00000
#define FLASH_REGION_SIZE       (0xFFFFFFFF-FLASH_BASE_ADDRESS-1)

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

// VOID
// FlashWriteWord(
//     IN ULONG_PTR Physical,
//     IN WORD Word
//     )
// ++
//
// Routine Description:
//
//     This macro maps specified physical address of flash ROM into mapped
//     virtual address and writes two bytes to mapped address.
//
// Arguments:
//
//     Physical - Physical address of flash ROM to be read
//
//     Word - Data to be written to
//
// Return Value:
//
//     None
//
// --

#define FlashWriteWord(a, d) \
    (*(PWORD)((ULONG_PTR)KernelRomBase+(ULONG_PTR)(a)-FLASH_BASE_ADDRESS) = d)

FlashStatus
FlashGetStatus(
    IN FlashDeviceID ID,
    IN ULONG_PTR Address OPTIONAL,
    IN BYTE Data
    )
/*++

Routine Description:

    This routine checks status of flash chip using data# polling method.
    The data# polling bit, DQ7, indicates whether the Embeded Algorithm
    is in progress or completed.

Arguments:

    ID - Device identifier returned by FlashDetectDevice

    Address - Physical address of flash ROM to be checked

    Data - Expected data at specified address

Return Value:

    Status of flash chip, see the definition of FlashStatus above.

--*/
{
    UCHAR retry=1, d, t;

    if (!ARGUMENT_PRESENT(Address)) {
        Address = FlashBaseAddress(ID);
    }

    if (ID == FlashSST49LF040) {

        //
        // SST doesn't support Exceeded Timing Limits, DQ5
        //

        d = FlashReadByte(Address) & 0x80;
        t = Data & 0x80;

        if (t == d) {
            return FlashStatusReady;
        } else {
            return FlashStatusBusy;
        }

    } else {

again:
        d = FlashReadByte(Address) & 0x80;
        t = Data & 0x80;

        if (t == d) {
            return FlashStatusReady;        // data matches
        } else if (d & 0x20) {              // Timeout?
            d = FlashReadByte(Address) & 0x80;
            if (t == d) {
                return FlashStatusReady;    // data matches
            }
            if (retry--) {
                goto again;                 // may have been write completion
            }
            return FlashStatusTimeout;
        }

        if (retry--) {
            goto again;                     // may have been write completion
        } else {
            return FlashStatusError;
        }
    }
}

VOID
FlashResetDevice(
    VOID
    )
/*++

Routine Description:

    This routine resets flash ROM back to read mode if device is in ID command
    mode or during a program or erase operation

Arguments:

    None

Return Value:

    None

--*/
{
    FlashWriteByte(0xFFFF5555, 0xAA);
    FlashWriteByte(0xFFFF2AAA, 0x55);
    FlashWriteByte(0xFFFF5555, 0xF0);
    KeStallExecutionProcessor(150000);
}

FlashDeviceID
FlashDetectDevice(
    VOID
    )
/*++

Routine Description:

    This routine detects the device and manufacturer id of flash device on
    the system.  The device on Xbus will be detected first and if no device
    detected, LPC bus will be next.

Arguments:

    None

Return Value:

    Type of flash id installed in the system or FlashUnknownDevice

--*/
{
    BYTE byte;
    BYTE id1=0, id2=0;
    WORD DeviceID;
    BOOL FirstTime = TRUE;
    PCI_SLOT_NUMBER PCISlotNumber;
    PCI_COMMON_CONFIG Configuration;
    SIZE_T loop;

detect:

    FlashWriteByte(0xFFFF5555, 0xAA);
    FlashWriteByte(0xFFFF2AAA, 0x55);
    FlashWriteByte(0xFFFF5555, 0x90);

    KeStallExecutionProcessor(1);

    id1 = FlashReadByte(0xFFFF0000);
    id2 = FlashReadByte(0xFFFF0001);
    DeviceID = id1 << 8 | id2;

    FlashResetDevice();

    for (loop=0; loop<sizeof(_FlashDeviceClass)/sizeof(_FlashDeviceClass[0]); loop++) {
        if (wcschr(_FlashDeviceClass[loop].DeviceCodes, DeviceID)) {
            return _FlashDeviceClass[loop].DeviceID;
        }
    }

    if (FirstTime == TRUE) {

        //
        // We are here because we couldn't find any flash ROM on Xbus.
        // Next thing is to see if this is a EVT board and enable ROM
        // write bus cycle to LPC interface.  By default the write cycle
        // to ROM will drop.
        //

        FirstTime = FALSE;

        //
        // Looking for PCI-to-LPC bridge
        //

        for (byte=0x00; byte<=0xff; byte++) {
            PCISlotNumber.u.AsULONG = byte;
            HalReadPCISpace(0, PCISlotNumber.u.AsULONG, 0, &Configuration, sizeof(Configuration));
            if (Configuration.BaseClass == 6 && Configuration.SubClass == 1) {
                break;
            }
        }

        //
        // If it is Nvidia PCI-to-LPC bridge, enable LPC ROM write
        //

        if (Configuration.VendorID == 0x10DE && Configuration.DeviceID == 0x01B2) {
            byte = 0x01;
            HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x45, &byte, sizeof(byte));
            goto detect;
        }

    } else {

        //
        // If we still couldn't find any flash ROM on LPC bus, disable LPC
        // ROM write
        //

        byte = 0x00;
        HalWritePCISpace(0, PCISlotNumber.u.AsULONG, 0x45, &byte, sizeof(byte));
    }

    return FlashUnknownDevice;
}

BOOL
FlashEraseChip(
    FlashDeviceID ID
    )
/*++

Routine Description:

    This routine erase the content of entire flash ROM to 0xFF.

Arguments:

    ID - Device identifier returned by FlashDetectDevice

Return Value:

    TRUE if operation success, FALSE otherwise

--*/
{
    ULONG_PTR FlashPtr;
    BOOL fBlank = FALSE;
    SIZE_T Count, Retries=5;

    while (Retries--) {

        FlashWriteByte(0xFFFF5555, 0xAA);
        FlashWriteByte(0xFFFF2AAA, 0x55);
        FlashWriteByte(0xFFFF5555, 0x80);
        FlashWriteByte(0xFFFF5555, 0xAA);
        FlashWriteByte(0xFFFF2AAA, 0x55);
        FlashWriteByte(0xFFFF5555, 0x10);

        FlashPtr = ~0L;

        //
        // Wait until flash chip is ready and completely erased.
        //

        Count = 0x100000;
        while (FlashGetStatus(ID, FlashPtr, 0xFF) != FlashStatusReady && Count) {
            Count--;
        }

        //
        // For some reasons in the FIB DVT, even the flash status is
        // ready but the actual data is not written to the chip.
        // We just have to poll the actual data and spin for a while
        // if it didn't get through.
        //

        Count = 0x100000;
        while (FlashReadByte(FlashPtr) != 0xFF && Count) {
            KeStallExecutionProcessor(150000);
            Count--;
        }

        //
        // Perform a blank check by compare all the content with 0xFF
        //

        Count = FlashSize(ID);
        FlashResetDevice();
        FlashPtr = FlashBaseAddress(ID);

        ASSERTMSG("Size of flash ROM must be power of two", (Count & (Count-1)) == 0);

        while (Count) {
            if (FlashReadWord(FlashPtr) != 0xFFFF) {
                DbgPrint("FLASH: blank check failed (retries=%d)\n", Retries);
                break;
            }

            Count -= sizeof(WORD);
            FlashPtr += sizeof(WORD);
        }

        if (Count == 0) {
            fBlank = TRUE;
            break;
        }

        FlashResetDevice();
    }

    return fBlank;
}

BOOL
FlashProgramImage(
    IN FlashDeviceID ID,
    IN PVOID ImageBuffer
    )
/*++

Routine Description:

    This routine programs the content of flash ROM with new image.  The flash
    ROM has to be blank first before programming.

Arguments:

    ID - Device identifier returned by FlashDetectDevice

    ImageBuffer - Buffer contains the content to be programmed

Return Value:

    TRUE if operation success, FALSE otherwise

--*/
{
    BYTE b;
    PBYTE pb;
    PWORD pw;
    ULONG TimeOut;
    ULONG_PTR FlashPtr;
    BYTE TrueData, CurrData;
    BOOL Loop, fSuccess = FALSE;
    SIZE_T Count, Retries = 5;
    SIZE_T Again;

    while (Retries--) {

        Count = FlashSize(ID);
        pb = (PBYTE)ImageBuffer;
        FlashPtr = FlashBaseAddress(ID);

        while (Count--) {

            b = *pb++;

            if (b != 0xFF) {
                FlashWriteByte(0xFFFF5555, 0xAA);
                FlashWriteByte(0xFFFF2AAA, 0x55);
                FlashWriteByte(0xFFFF5555, 0xA0);
                FlashWriteByte(FlashPtr, b);

                //
                // Wait until flash chip is ready for next command
                //

                Again = 0x100000;
                while (FlashGetStatus(ID, FlashPtr, b) != FlashStatusReady && Again) {
                    Again--;
                }

                //
                // For some reasons in the FIB DVT, even the flash status is
                // ready but the actual data is not written to the chip.
                // We just have to poll the actual data and spin for a while
                // if it didn't get through.
                //

                Again = 0x100000;
                while (FlashReadByte(FlashPtr) != b && Again) {
                    Again--;
                }
            }

            FlashPtr++;
        }

        //
        // Verify the content that just has been programmed
        //

        Count = FlashSize(ID);
        pw = (PWORD)ImageBuffer;
        FlashPtr = FlashBaseAddress(ID);
        FlashResetDevice();

        while (Count) {
            if (FlashReadWord(FlashPtr) != *pw++) {
                DbgPrint("FLASH: verification failed (retries=%d)\n", Retries);
                break;
            }

            Count -= sizeof(WORD);
            FlashPtr += sizeof(WORD);
        }

        if (Count == 0) {
            fSuccess = TRUE;
            break;
        }

        FlashResetDevice();
    }

    return fSuccess;
}

UINT64
FASTCALL
FlashReadMSR(
    IN ULONG Address
    )
/*++

Routine Description:

    This routine reads Pentium III Model-Specific Register (MSR) specified
    by Address

Arguments:

    Address - Register address to read

Return Value:

    64-bit value of specified MSR

--*/
{
    __asm {
        rdmsr
    }
}

VOID
FASTCALL
FlashWriteMSR(
    IN ULONG Address,
    IN UINT64 Value
    )
/*++

Routine Description:

    This routine writes Pentium III Model-Specific Register (MSR) specified
    by Address

Arguments:

    Address - Register address to read

    Value - 64-bit value to be written

Return Value:

    None

--*/
{
    __asm {
        mov     eax, DWORD PTR [Value]
        mov     edx, DWORD PTR [Value+4]
        wrmsr
    }
}

#ifdef FLASH_TIME

UINT64
FlashReadTSC(
    VOID
    )
/*++

Routine Description:

    This routine reads processor's time-stamp counter.  The time-stamp counter
    is contained in a 64-bit MSR.  The high-order of 32 bits MSR are loaded
    into the EDX register, and the low-order 32 bits are loaded into the EAX
    register.  The processor increments the time-stamp counter MSR every
    clock cycle and resets it to 0 whenever the processor reset.

Arguments:

    None

Return Value:

    64-bit MSR of time-stamp counter

--*/
{
    __asm {
        rdtsc
    }
}

#endif // FLASH_TIME

VOID
FlashChangeRomCaching(
    BOOL EnableCache
    )
/*++

Routine Description:

    This routine searches for ROM cache setting in MTRR and disable it.  It is
    necessary to disable and flash cache before changing MTRR.  The following
    steps are recommended by Intel in order to change MTRR settings. Save CR4,
    disable and flush processor cache, flush TLB, disable MTRR, change MTRR
    settings, flush cache and TLB, enable MTRR and restore CR4

Arguments:

    EnableCache - TRUE to enable caching, FALSE to disable

Return Value:

    None

--*/
{
    ULONG MTRR;
    UINT64 v, MTRRdeftype;
    ULONG Base, Type;


    __asm {
        push    ecx
        push    edx

        _emit   0fh                 ; mov  eax, cr4
        _emit   20h
        _emit   0e0h

        push    eax                 ; save content of cr4

        mov     eax, cr0            ; disable and flush cache
        push    eax                 ; save content of cr0
        or      eax, 060000000H
        mov     cr0, eax
        wbinvd

        mov     eax, cr3            ; flush TLB
        mov     cr3, eax
    }

    //
    // Save the content of MTRR deftype and disable MTRR
    //

    MTRRdeftype = FlashReadMSR(0x2FF);
    FlashWriteMSR(0x2FF, 0);

    for (MTRR=0x200; MTRR<0x20F; MTRR+=2) {
        v = FlashReadMSR(MTRR);
        Base = (ULONG)((v >> 12) & 0xFFFFFF);
        Type = (BYTE)v;

        //
        // Set or reset valid bit according to cache enable flag
        //

        if (Base >= (FLASH_BASE_ADDRESS >> 12) && Type != 0) {
            v = FlashReadMSR(MTRR+1);
            v = EnableCache ? (v | 0x800) : (v & (~0x800));
            FlashWriteMSR(MTRR+1, v);
        }
    }

    __asm {
        wbinvd                      ; flush cache
        mov     eax, cr3            ; flush TLB
        mov     cr3, eax
    }

    //
    // Restore content of MTRR deftype, MTRR should be re-enabled
    //

    FlashWriteMSR(0x2FF, MTRRdeftype);

    __asm {
        pop     eax                 ; restore cr0
        mov     cr0, eax

        pop     eax                 ; restore cr4

        _emit   0fh                 ; mov  cr4, eax
        _emit   22h
        _emit   0e0h

        pop     edx
        pop     ecx
    }
}

HRESULT
FlashKernelImage(
    IN  PVOID  ImageBuffer,
    IN  SIZE_T ImageSize,
    OUT LPSTR  szResp,
    IN  DWORD  cchResp,
    IN  BOOL   IgnoreVersionChecking
    )
{
#ifdef FLASH_TIME
    UINT64 ClockTick;
#endif

    FlashDeviceID ID;
    HRESULT hr = XBDM_NOERR;
    BYTE RomVersion, ImageVersion;

#ifdef FLASH_TIME
    ClockTick = FlashReadTSC();
#endif

    if (IsBadReadPtr(ImageBuffer, ImageSize) || \
        IsBadWritePtr(szResp, cchResp)) {
        return HRESULT_FROM_WIN32(ERROR_NOACCESS);
    }

    //
    // Map top 1MB of physical memory of ROM region (FFF00000-FFFFFFFF)
    //

    KernelRomBase = MmMapIoSpace(FLASH_BASE_ADDRESS, FLASH_REGION_SIZE,
                                 PAGE_READWRITE | PAGE_NOCACHE);

    if (!KernelRomBase) {
        _snprintf(szResp, cchResp, "unable to map i/o space");
        return E_FAIL;
    }

    //
    // Before disable all interrupts, sleep 300 msec so that the TCP/IP stack
    // get a chance a respond ACK to the other end
    //

    Sleep(300);

    __asm cli

    DbgPrint("FLASH: interrupts are now disabled\n");

    //
    // Disable ROM caching
    //

    FlashChangeRomCaching(FALSE);

    ID = FlashDetectDevice();

    if (ID == FlashUnknownDevice) {
        _snprintf(szResp, cchResp, "Unknown flash device id");
        hr = E_FAIL;
        goto cleanup;
    }

    if (FlashSize(ID) != ImageSize) {
        _snprintf(szResp, cchResp, "Invalid image size");
        hr = E_FAIL;
        goto cleanup;
    }

    RomVersion = FlashReadByte(FLASH_BASE_ADDRESS + ROM_VERSION_BYTE_OFFSET);
    ImageVersion = ((PBYTE)ImageBuffer)[ROM_VERSION_BYTE_OFFSET];

    if (!IgnoreVersionChecking && RomVersion != ImageVersion) {
        _snprintf(szResp, cchResp, "Mismatch ROM version (rom=%x, image=%x)",
            RomVersion, ImageVersion);
        hr = E_FAIL;
        goto cleanup;
    }

    DbgPrint("FLASH: erasing and blank checking...\n");

    if ( !FlashEraseChip(ID) ) {
        _snprintf(szResp, cchResp, "Failed to erase flash chip");
        hr = E_FAIL;
        goto cleanup;
    }

    DbgPrint("FLASH: programming...\n");

    if (!FlashProgramImage(ID, ImageBuffer)) {
        _snprintf(szResp, cchResp, "Failed to program kernel image (verify failed)");
        hr = E_FAIL;
        goto cleanup;
    }

    DbgPrint("FLASH: done...\n");

    if (SUCCEEDED(hr)) {
        _snprintf(szResp, cchResp, "Done, new image flashed");
    }

    hr = XBDM_NOERR;

cleanup:

    __asm sti

    DbgPrint("FLASH: interrupts are now enabled\n");

    MmUnmapIoSpace(KernelRomBase, FLASH_REGION_SIZE);

    //
    // Re-enable ROM caching as needed
    //

    FlashChangeRomCaching(TRUE);

#ifdef FLASH_TIME
    ClockTick = FlashReadTSC() - ClockTick;
    DbgPrint("FLASH: elapsed time %I64u seconds\n", ClockTick / 733000000UI64);
#endif

    return hr;
}

