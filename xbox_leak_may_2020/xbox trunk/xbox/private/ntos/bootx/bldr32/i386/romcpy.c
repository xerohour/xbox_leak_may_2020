/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    romcpy.c

Abstract:

    This module implements code to uncompress, copy, and check Xbox ROM

--*/

#include "bldr32.h"
#include <ldi.h>
#include <bldr.h>
#include <sha.h>
#include <rc4.h>

//
// Local support
//

BOOLEAN
BldrDecompress(
    IN  ULONG FromAddress,
    IN  ULONG ToAddress,
    IN  ULONG UncompressedSize
    );

MI_MEMORY
DIAMONDAPI
BldrAlloc(
    ULONG NumberOfBytes
    );

VOID
DIAMONDAPI
BldrFree(
    MI_MEMORY pointer
    );

//
// Location of the topmost byte, not inclusive, of the ROM.
//
ULONG BldrTopOfROMAddress = 0;

//
// Number of bytes that have been allocated for LZX decompression.  Used to mark
// the top of the heap.
//
ULONG BldrNumberOfBytesAllocated;

VOID
BldrCopyROMToRAM(
    IN const UCHAR* DecryptionKey
    )
/*++

Routine Description:

    Decrypts and decompresses kernel image from ROM to the RAM.  

Arguments:

    DecryptionKey - Key used to decrypt the kernel image

Return Value:

    None.

--*/
{
    ULONG BldrAddress;
    PBOOTLDRPARAM BootLdrParam;
    PXBOOT_PARAM BootParam;
    ULONG CompressedKernelAddress;
    ULONG UncompressedKernelDataAddress;
#ifdef MCP_B02XM3
    A_SHA_CTX SHAHash;
    UCHAR SHADigest[A_SHA_DIGEST_LEN];
    ULONG InitTableAddress;
#endif
    PUCHAR KernelBuffer;

    //
    // Start address for the boot loader image
    //
    BldrAddress = 0x80000000 + BLDR_RELOCATED_ORIGIN;

    //
    // Calculate a pointer to the boot parameters stored at the begining of
    // of the boot loader image eg command line.  In case of ROM, entire image
    // including the parameter structure is decrypted so its safe to use
    // the command line which will normally be empty.
    //
    BootLdrParam = (PBOOTLDRPARAM)BldrAddress;

    //
    // Boot param structure is at the end of the boot loader block
    //
    BootParam = (PXBOOT_PARAM)(BldrAddress + BLDR_BLOCK_SIZE - sizeof(XBOOT_PARAM));

    //
    // Compute the address of the uncompressed kernel data block
    //
    UncompressedKernelDataAddress = 0 - ROM_DEC_SIZE -
        BLDR_BLOCK_SIZE - BootParam->UncompressedKernelDataSize;

    //
    // Compute the address of the compressed kernel image
    //
    CompressedKernelAddress = UncompressedKernelDataAddress -
        BootParam->CompressedKernelSize;

#ifdef MCP_B02XM3

    //
    // Calculate SHA1 digest for the compressed kernel image and uncompressed
    // kernel data block.
    //
    // Compute the address of the init table.  The ROM image is aliased
    // through out the top 16MB of address space.  For a 1MB image, it will
    // be aliased 16 times.  We use the top 1MB alias and the address mapped
    // in startup.asm.  This should work fine for 512K or 256K but will not
    // work for parts larger than 1MB.
    //
    InitTableAddress = 0UL - 0x100000;

    BldrPrint(("Bldr: Init Table=%08x, Version=%x", InitTableAddress, 
        *((PUCHAR)(InitTableAddress + 0x78))));

    //
    // Calculate a SHA1 digest with the following ROM components
    //     1. Size and contents of compressed and encrypted kernel in ROM
    //     2. Size and contents of uncompressed data
    //     3. Size and contents of init table
    //
    // Note: We calculate the digests directly from ROM rather than 
    // depending on the contents of RAM at this point
    //

    A_SHAInit(&SHAHash);
    A_SHAUpdate(&SHAHash, (PUCHAR)DecryptionKey, 16);
    A_SHAUpdate(&SHAHash, (PUCHAR)&BootParam->CompressedKernelSize, sizeof(ULONG));
    A_SHAUpdate(&SHAHash, (PVOID)CompressedKernelAddress, BootParam->CompressedKernelSize);
    
    A_SHAUpdate(&SHAHash, (PUCHAR)&BootParam->UncompressedKernelDataSize, sizeof(ULONG));
    A_SHAUpdate(&SHAHash, (PVOID)UncompressedKernelDataAddress, BootParam->UncompressedKernelDataSize);
    
#ifdef DEVKIT
    //
    // Hack to allow manual editing of the init table.  For devkit systems, if
    // the InitTableSize is 0, we skip calculating the digest for the init table
    // This can be initiatiated by ROMBLD's /HACKINITTABLE option
    //
    if (BootParam->InitTableSize != 0) {
        A_SHAUpdate(&SHAHash, (PUCHAR)&BootParam->InitTableSize, sizeof(ULONG));
        A_SHAUpdate(&SHAHash, (PVOID)InitTableAddress, BootParam->InitTableSize);
    }

#else
    
    A_SHAUpdate(&SHAHash, (PUCHAR)&BootParam->InitTableSize, sizeof(ULONG));
    A_SHAUpdate(&SHAHash, (PVOID)InitTableAddress, BootParam->InitTableSize);

#endif

    A_SHAFinal(&SHAHash, SHADigest);
    A_SHAInit(&SHAHash);
    A_SHAUpdate(&SHAHash, (PUCHAR)DecryptionKey, 16);
    A_SHAUpdate(&SHAHash, SHADigest, sizeof SHADigest);
    A_SHAFinal(&SHAHash, SHADigest);

    //
    // Compare the calculated digest with the digest in the boot param
    //
    if (memcmp(SHADigest, BootParam->MainRomDigest, sizeof(SHADigest)) != 0) {
        BldrPrint(("Bldr: ROM digests do not match"));

        BldrShutdownSystem();
    }

#endif // MCP_B02XM3

    //
    // If we got here, the ROM image is OK to decrypt.  Allocate enough space
    // so we can copy the compessed encrypted kernel image to RAM.  We don't have to
    // worry about freeing this memory.
    //
    KernelBuffer = (PUCHAR)BldrAlloc(BootParam->CompressedKernelSize);

    //
    // Copy the compress encrypted kernel image from ROM to RAM
    //
    BldrPrint(("Bldr: Copying from %08x to %08x", CompressedKernelAddress, KernelBuffer));
    memcpy(KernelBuffer, (PUCHAR)CompressedKernelAddress, BootParam->CompressedKernelSize);

    //
    // Decrypt the RAM kernel buffer in place
    //
    BldrEncDec(KernelBuffer, BootParam->CompressedKernelSize, DecryptionKey, 16);

    //
    // Decompress the decrypted RAM kernel buffer to the location kernel image
    // needs to run from
    //
    BldrDecompress((ULONG_PTR)KernelBuffer, (ULONG_PTR)PsNtosImageBase,
        BootParam->CompressedKernelSize);

    //
    // Fixup the pointer to the uncompressed .data with the actual load address
    // of the ROM.  If the ROM is loaded at the top of memory, then this doesn't
    // affect the pointer.
    //
    ((PXDATA_SECTION_HEADER)(((PIMAGE_DOS_HEADER)PsNtosImageBase)->e_res2))->PointerToRawData +=
        BldrTopOfROMAddress;
}

MI_MEMORY
DIAMONDAPI
BldrAlloc(
    ULONG NumberOfBytes
    )
/*++

Routine Description:

    Simple memory allocation used for limited known allocation

Arguments:

Return Value:

    A pointer to where the memory was allocated

--*/
{
    PVOID BaseAddress;

    BaseAddress = (PVOID)(0x80000000 + BLDR_RELOCATED_ORIGIN + BLDR_BLOCK_SIZE +
        BldrNumberOfBytesAllocated);

    BldrNumberOfBytesAllocated += NumberOfBytes;

    BldrPrint(("Bldr: Allocate %d bytes at %x", NumberOfBytes, BaseAddress));

    return BaseAddress;
}

VOID
DIAMONDAPI
BldrFree(
    MI_MEMORY pointer
    )
{
    // we don't need to free the memory
}

BOOLEAN
BldrDecompress(
    IN  ULONG FromAddress,
    IN  ULONG ToAddress,
    IN  ULONG UncompressedSize
    )
{
    PLZXBOX_BLOCK Block;
    ULONG Source;
    ULONG Dest;
    UINT SourceSize;
    LZXDECOMPRESS Decomp;
    UINT DestSize;
    ULONG BytesDecompressed;
    LDI_CONTEXT_HANDLE Handle;
    BOOLEAN Success = FALSE;

    BldrPrint(("Bldr: Decompressing from=0x%08X to=0x%08X bytes=%d", 
        FromAddress, ToAddress, UncompressedSize));
    
    //
    // Initialize decompression engine
    //
    Decomp.fCPUtype = LDI_CPU_80386;
    Decomp.WindowSize = LZX_WINDOW_SIZE;

    SourceSize = LZX_CHUNK_SIZE;
    if (LDICreateDecompression(&SourceSize, &Decomp, BldrAlloc, BldrFree, &DestSize,
        &Handle, NULL,NULL,NULL,NULL,NULL) != MDI_ERROR_NO_ERROR) {
        
        BldrPrint(("Bldr: Failed to init decompression engine"));
        
        return Success;
    }

    Source = FromAddress;
    Dest = ToAddress;
    for (;;) {

        //
        // Read the compression block
        //
        Block = (PLZXBOX_BLOCK)Source;
        Source += sizeof(LZXBOX_BLOCK);

        //
        // Perform decompression
        //
        BytesDecompressed = Block->UncompressedSize;
        if (LDIDecompress(Handle, (PVOID)Source, Block->CompressedSize, (PVOID)Dest,
            &BytesDecompressed) != MDI_ERROR_NO_ERROR) {
            goto CleanupAndExit;
        }

        //
        // Advance the pointers
        //
        Source += Block->CompressedSize;
        Dest += Block->UncompressedSize;

        //
        // Check to see if we are done
        //
        if ((Source - FromAddress) >= UncompressedSize) {
            break;
        }
    }

    Success = TRUE;

CleanupAndExit:
    (VOID)LDIDestroyDecompression(Handle);

    BldrPrint(("Bldr: Decompression %s", (Success ? "successful" : "failed")));

    return Success;
}

VOID
BldrShutdownSystem(
    VOID
    )
/*++

Routine Description:

    Shutdown the system.

Arguments:

    None.

Return Value:

    None.

--*/
{
    BldrPrint(("Bldr: *** Shutdown occurred"));

#ifdef RETAILXM3
    //
    // Flat line the clocks
    //

    _outpd(PCI_TYPE1_ADDR_PORT, 0x8000036C);
    _outpd(PCI_TYPE1_DATA_PORT, 0x01000000);
    
#endif

    __asm {
        hlt
    }
}


VOID
BldrEncDec(
    IN  PUCHAR  MsgData,
    IN  ULONG   MsgDataLen,
    IN  const UCHAR* Key,
    IN  ULONG   KeyBytes
    )
/*++

Routine Description:

    Encrypt or decrypt the given data buffer in-place using RC4 which is symmetric algorithm.

Arguments:


Return Value:

    None

--*/
{
    struct RC4_KEYSTRUCT rc4KeyCtl;

    BldrPrint(("Bldr: Decrypting %d bytes at %x", MsgDataLen, MsgData));

    rc4_key(&rc4KeyCtl, (UINT)KeyBytes, (PUCHAR)Key);

    rc4(&rc4KeyCtl, (UINT)MsgDataLen, MsgData);
}


