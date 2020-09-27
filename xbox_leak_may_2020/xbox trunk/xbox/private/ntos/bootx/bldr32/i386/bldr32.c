/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    bldr32.c

Abstract:

    This module implements the initialization sequence required to jump to
    XBOXKRNL.EXE.

--*/

#include "bldr32.h"
#include "bldr.h"
#include "xpcicfg.h"
#include "enckey.h"

#ifdef BLDRMEMTEST
#include <ldi.h>
#include <sha.h>
#include <rc4.h>
#endif


VOID
DbgInitialize(
    VOID
    );

VOID
DbgPrintCharacter(
    IN UCHAR Character
    );

//
// Prototype for the entry point of XBOXKRNL.EXE.
//

typedef
int
(__cdecl *NTOS_ENTRY_POINT)(
    IN PUCHAR LoadOptions,
    IN const UCHAR* CryptKeys
    );


VOID
BldrDisableInteralROM(
    VOID
    );
VOID
BldrEncoderFixup(
    VOID
    );
VOID
BldrSetupUSB(
    VOID
    );

#ifdef BLDRMEMTEST
VOID
BldrTestMemory(
    ULONG StartAddress,
    ULONG EndAddress,
    BOOLEAN CompareDigests
    );

ULONG MemTestErrorCount = 0;
#endif


VOID
BldrStartup2(
    VOID
    )
{
    PIMAGE_NT_HEADERS NtHeader;
    NTOS_ENTRY_POINT NtosEntryPoint;
    PBOOTLDRPARAM BootLdrParam;
    const UCHAR* KeyToDecryptKernel;
    BOOLEAN FirstTime;

#ifdef RETAILXM3

    BldrDisableInteralROM();

    BldrEncoderFixup();

#endif

    BldrPrint(("Bldr: Entered BldrStartup2"));

#ifdef BLDRMEMTEST
    //
    // Memory test before calibration
    //

    //BldrTestMemory(0, BLDR_RELOCATED_ORIGIN - 1, TRUE);
    BldrTestMemory(BLDR_RELOCATED_ORIGIN + BLDR_BLOCK_SIZE, 0x3FFFFFF, TRUE);
#endif


    //
    // The boot loader was relocated from BLDR_BOOT_ORIGIN to
    // BLDR_RELOCATED_ORIGIN.  Code should no longer attempt to access any
    // memory at the boot origin because it will be overwritten when we
    // decompress the kernel into low memory.  Fill the old memory with a
    // garbage value to ensure that we don't accidently use that memory.
    //

    RtlFillMemory((PVOID)(0x80000000 + BLDR_BOOT_ORIGIN), BLDR_BLOCK_SIZE, 0xCC);

    //
    // Calculate a pointer to the boot parameters stored at the begining of
    // of the boot loader image eg command line.  In case of ROM, entire image
    // including the parameter structure is decrypted so its safe to use
    // the command line which will normally be empty.
    //

    BootLdrParam = (PBOOTLDRPARAM)(0x80000000 + BLDR_RELOCATED_ORIGIN);

    FirstTime = TRUE;
#ifndef RETAILXM3
    if (strstr(BootLdrParam->CommandLine, "SHADOW") != NULL) {
        FirstTime = FALSE;
    }
#endif

    if (FirstTime) {
        
        //
        // Set drive/slew from ROM.
        //

        BldrSetDrvSlwFromROMData((PVOID)XPCICFG_GPU_MEMORY_REGISTER_BASE_0);
    
        //
        // Setup LDT bus including DWORD flow control
        //
 
        BldrSetupLDTBus();

        //
        // Configure the USB ASRC
        //

        BldrSetupUSB();
    
    }

#ifdef BLDRMEMTEST
    
    //
    // Memory test after calibration
    //

    //BldrTestMemory(0, BLDR_RELOCATED_ORIGIN - 1, FALSE);
    BldrTestMemory(BLDR_RELOCATED_ORIGIN + BLDR_BLOCK_SIZE, 0x3FFFFFF, FALSE);
#endif


    //
    // Decrypt and decompress the kernel image from ROM to its base address 
    // in RAM.  Kernel encryption key is the 3rd key in the XboxCryptKeys
    //
    
    KeyToDecryptKernel = XboxCryptKeys + (2 * XBOX_KEY_LENGTH);

    BldrCopyROMToRAM(KeyToDecryptKernel);

    //
    // Go find the entry point.
    //

    NtHeader = RtlImageNtHeader(PsNtosImageBase);

    if (NtHeader == NULL) {

        //
        // Image is invalid
        //
        
        BldrPrint(("Bldr: Entry point not found--invalid image"));

        BldrShutdownSystem();
    }

    NtosEntryPoint = (NTOS_ENTRY_POINT)((ULONG_PTR)PsNtosImageBase +
        NtHeader->OptionalHeader.AddressOfEntryPoint);

    //
    // Pass control to XBOXKRNL.EXE.
    //
    
    BldrPrint(("Bldr: Calling into the kernel"));

#ifdef BLDRMEMTEST
    
    //
    // While testing memory, if there are no errors, tell the SMC to reboot
    //

    if (MemTestErrorCount == 0) {
        DbgPrint("Memory test finished with no errors, rebooting\r\n...");
        BldrWriteSMBusByte(SMC_SLAVE_ADDRESS, 0x2, 0x40);
    } else {
        DbgPrint("Memory test FAILED with %d errors", MemTestErrorCount);
    }

#endif

    NtosEntryPoint(BootLdrParam->CommandLine, XboxCryptKeys);
}

VOID
BldrReencryptROM(
    VOID
    )
{
    BldrEncDec((PUCHAR)0 - ROM_DEC_SIZE - BLDR_BLOCK_SIZE,
        BLDR_BLOCK_SIZE, KeyToDecryptBldr, 16);
}


#ifdef RETAILXM3

VOID
BldrDisableInteralROM(
    VOID
    )
{
    _outpd(PCI_TYPE1_ADDR_PORT, 0x80000880);
    _outp(PCI_TYPE1_DATA_PORT, 2);
}


#include <bldrtran.h>


VOID
BldrEncoderFixup(
    VOID
    )
{
    UCHAR i0; 
    UCHAR i1; 
    UCHAR i2; 
    UCHAR i3;
    UCHAR o0;
    UCHAR o1;
    ULONG Val;

    //
    // Read the inputs
    //
    
    Val = 0;
    BldrReadSMBusByte(SMC_SLAVE_ADDRESS, 0x1c, &Val);
    i0 = (UCHAR)Val;
    BldrReadSMBusByte(SMC_SLAVE_ADDRESS, 0x1d, &Val);
    i1 = (UCHAR)Val;
    BldrReadSMBusByte(SMC_SLAVE_ADDRESS, 0x1e, &Val);
    i2 = (UCHAR)Val;
    BldrReadSMBusByte(SMC_SLAVE_ADDRESS, 0x1f, &Val);
    i3 = (UCHAR)Val;

    if (i0 == 0 && i1 == 0 && i2 == 0 && i3 == 0) {
        BldrShutdownSystem();    
    }

    BldrEncoderTranslate(i0, i1, i2, i3, &o0, &o1);

    //
    // Write the outputs
    //
    
    BldrWriteSMBusByte(SMC_SLAVE_ADDRESS, 0x20, o0);
    BldrWriteSMBusByte(SMC_SLAVE_ADDRESS, 0x21, o1);

    BldrPrint(("\n%d %d %d %d => %d %d", i0, i1, i2, i3, o0, o1));


    //
    // Reset the SMC revision pointer
    //
    
    BldrWriteSMBusByte(SMC_SLAVE_ADDRESS, 0x1, 0);

    //
    // Reenable all MCP devices.  Specifically NIC and IDE which
    // were disabled in the init table strap value
    //
    
    _outpd(PCI_TYPE1_ADDR_PORT, 0x8000088C);
    _outpd(PCI_TYPE1_DATA_PORT, 0x40000000);
}

#endif


VOID
BldrSetupUSB(
    VOID
    )
{

    UCHAR MCPRevisionID;

    //
    // Setup the USB ASRC (Automatic Slew Rate Compensation) which affects how the 
    // USB signal is driven.  This change is required only for MCP revision D01 and later.
    // Read the MCP revision to see if the change needs to be applied.
    //
    
    _outpd(PCI_TYPE1_ADDR_PORT, 0x80000808);
    
    MCPRevisionID = (UCHAR)_inp(PCI_TYPE1_DATA_PORT);    
    
    if (MCPRevisionID >= 0xD1) {
        _outpd(PCI_TYPE1_ADDR_PORT, 0x800008C8);
        _outpd(PCI_TYPE1_DATA_PORT, 0x00008F00);
    }
}


#ifdef BLDRMEMTEST
VOID
BldrTestMemory(
    ULONG StartAddress,
    ULONG EndAddress,
    BOOLEAN CompareDigests
    )                                                           
{
    UCHAR Key[16];
    ULONG Index;        
    UCHAR SHADigest[A_SHA_DIGEST_LEN];
    A_SHA_CTX SHAHash;

    if (CompareDigests) {
        A_SHAInit(&SHAHash);
        A_SHAUpdate(&SHAHash, (PVOID)BLDR_RELOCATED_ORIGIN, BLDR_BLOCK_SIZE);
        A_SHAFinal(&SHAHash, SHADigest);
        
        DbgPrintHex(SHADigest, 20);
    
        A_SHAInit(&SHAHash);
        A_SHAUpdate(&SHAHash, (PVOID)0xFFFF9E00, BLDR_BLOCK_SIZE);
        A_SHAFinal(&SHAHash, SHADigest);
        
        DbgPrintHex(SHADigest, 20);
    }


    DbgPrint("Testing memory from %x to %x with 00s", StartAddress, EndAddress);

    RtlFillMemory((PVOID)StartAddress, EndAddress - StartAddress + 1, 0);

    for (Index = 0; Index < 16; Index++) {
        Key[Index] = (UCHAR)Index;
    }
    BldrEncDec((PUCHAR)StartAddress, EndAddress - StartAddress + 1, Key, sizeof(Key));

    __asm wbinvd
    
    for (Index = 0; Index < 16; Index++) {
        Key[Index] = (UCHAR)Index;
    }
    BldrEncDec((PUCHAR)StartAddress, EndAddress - StartAddress + 1, Key, sizeof(Key));

    __asm wbinvd

    for (Index = StartAddress; Index <= EndAddress; Index++) {
        if ((*(UCHAR*)Index) != 0) {
            MemTestErrorCount++;
            DbgPrint("***Mem test failed at %x (%d)", Index, (*(UCHAR*)Index)); 
        }
    }
    
    DbgPrint("Testing memory from %x to %x with FFs", StartAddress, EndAddress);

    RtlFillMemory((PVOID)StartAddress, EndAddress - StartAddress + 1, 0xFF);

    for (Index = 0; Index < 16; Index++) {
        Key[Index] = (UCHAR)Index;
    }
    BldrEncDec((PUCHAR)StartAddress, EndAddress - StartAddress + 1, Key, sizeof(Key));

    __asm wbinvd
    
    for (Index = 0; Index < 16; Index++) {
        Key[Index] = (UCHAR)Index;
    }
    BldrEncDec((PUCHAR)StartAddress, EndAddress - StartAddress + 1, Key, sizeof(Key));

    __asm wbinvd

    for (Index = StartAddress; Index <= EndAddress; Index++) {
        if ((*(UCHAR*)Index) != 0xFF) {
            MemTestErrorCount++;
            DbgPrint("***Mem test failed at %x (%d)", Index, (*(UCHAR*)Index)); 
        }
    }
}
#endif
