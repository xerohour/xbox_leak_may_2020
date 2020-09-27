;++
;
;  Copyright (c) 2000-2001  Microsoft Corporation
;
;  Module Name:
;
;     startup.asm
;
;  Abstract:
;
;     This module implements the entry point for the 32-bit boot loader code.
;
;  Environment:
;
;     32-bit protected mode.
;
;--

        .586p

        INCLUDE bldr.inc
        INCLUDE ks386.inc

        EXTERN  _BldrStartup2@0:NEAR
        EXTERN  _BldrTopOfROMAddress:DWORD
        EXTERN  _XboxCryptKeys:DWORD
        EXTERN  _BldrReencryptROM@0:NEAR

_TEXT   SEGMENT PARA USE32 PUBLIC 'CODE'

        ASSUME  DS:_TEXT, ES:_TEXT, SS:_TEXT, FS:NOTHING, GS:NOTHING

;
; BldrStartup
;
; Entry point for the 32-bit boot loader.
;
; The DWORD immediately before the entry point is the address of the routine to
; reload a media ROM.  When the kernel loads a ROM from the hard disk or CD-ROM,
; it will decrypt the boot loader, extract this function pointer relative to the
; boot loader entry point and jump to that code.
;
; The DWORD before the above is used to point to key data.  ROMBLD uses this
; to find where the keys are located in this image
;

        dd      OFFSET _XboxCryptKeys
        dd      OFFSET @BldrLoadMediaROM@4
        PUBLIC  _BldrStartup
_BldrStartup PROC

;
; Flush the processor's caches and disable the MTRRs in order to change the MTRR
; policy.  At this point, the MTRRs are initialized to map memory either as
; uncached or write-back.
;

        mov     eax, cr0                ; disable and flush cache
        or      eax, (CR0_CD OR CR0_NW)
        mov     cr0, eax
        wbinvd

        mov     eax, cr3                ; flush TLB
        mov     cr3, eax

        mov     ecx, 2FFh               ; disable MTRR
        xor     eax, eax
        xor     edx, edx
        wrmsr

;
; Set up the first 64MB (or 128MB for DEVKIT) to writeback using the first
; variable-range MTRR.
;

        mov     ecx, 200h
        mov     eax, 6                  ; physical base 00000h as writeback
        xor     edx, edx
        wrmsr

        inc     ecx
ifdef DEVKIT
        mov     eax, 0F8000800h         ; physical mask FF8000h, 128MB for DEVKIT
else
        mov     eax, 0FC000800h         ; physical mask FFC000h, 64MB
endif
        mov     edx, 00000000Fh
        wrmsr

;
; Set up the top 512KB to write-protected using the second variable-range MTRR.
;

        inc     ecx
        mov     eax, 0FFF80005h         ; physical base 0FFF80h as write-protected
        xor     edx, edx
        wrmsr

        inc     ecx
        mov     eax, 0FFF80800h         ; physical mask FFFF80h
        mov     edx, 00000000Fh
        wrmsr

;
; Reset the contents of the rest of the variable-range MTRRs.
;

        inc     ecx
        xor     eax, eax
        xor     edx, edx
@@:
        wrmsr
        inc     ecx
        cmp     ecx, 20Fh
        jbe     @B

;
; Enable MTRR, disable fix-range MTRRs and set default memory type to UC.
;

        mov     ecx, 2FFh
        mov     eax, 800h
        wrmsr

;
; Enable the processor cache by clearing cache disable and not-write-through
; flags in CR0.
;

        mov     eax, cr0
        and     eax, NOT (CR0_CD OR CR0_NW)
        mov     cr0, eax

;
; Load the various segment registers with the expected values.
;

        mov     eax, KGDT_R0_DATA
        mov     ds, eax
        mov     es, eax
        mov     ss, eax
        mov     esp, BLDR_RELOCATED_ORIGIN

        xor     eax, eax
        mov     fs, eax
        mov     gs, eax

;
; Copy the boot loader to the relocated boot origin so that once we jump to
; BldrStartup2, we'll be out of the way of the base address of XBOXKRNL.EXE.
;

        cld
ifdef MCP_B02XM3
        mov     esi, BLDR_BOOT_ORIGIN
else
        mov     esi, ebp
endif
        mov     edi, BLDR_RELOCATED_ORIGIN
        mov     ecx, ROMLDR_SIZE / 4
        rep     movsd

;
; Fill in the page directory with identify mappings for the first 256M of memory
; at both linear address 0x00000000 and 0x80000000.  Large pages (4MB) are used
; for this mapping.  Zero out the rest of the page directory.
;

        mov     edi, PAGE_DIRECTORY_PHYSICAL_ADDRESS
        mov     ecx, 64
        mov     eax, BLDR_VALID_KERNEL_LARGE_PTE_BITS   ; Set valid, write, large Page, PFN=0
@@:
        mov     DWORD PTR [edi+800h], eax
        stosd
        add     eax, 400000h                            ; Advanced PFN to the next 4MB page.
        loop    @B

        mov     ecx, 448
        xor     eax, eax
@@:
        mov     DWORD PTR [edi+800h], eax
        stosd
        loop    @B

;
; Double map the page directory page.
;

        mov     edi, PAGE_DIRECTORY_PHYSICAL_ADDRESS
        mov     eax, PAGE_DIRECTORY_PHYSICAL_ADDRESS + BLDR_VALID_KERNEL_PTE_BITS
        mov     DWORD PTR [edi + ((0C0000000h SHR 22) SHL 2)], eax

;
; Identity map 4MB of ROM space to the page directory.
;

        mov     eax, 0FFC00000h + BLDR_VALID_KERNEL_LARGE_PTE_BITS
        mov     DWORD PTR [edi + ((0FFC00000h SHR 22) SHL 2)], eax

;
; Identity map 16MB of GPU register space to the page directory (uncached)
;
        mov     eax, XPCICFG_GPU_MEMORY_REGISTER_BASE_0 + BLDR_VALID_KERNEL_LARGE_PTE_UC_BITS
        mov     ebx, eax
        shr     ebx, (22 - 2)
        add     edi, ebx
        mov     DWORD PTR [edi], eax    ; 4MB
        add     edi, 4
        add     eax, 400000h
        mov     DWORD PTR [edi], eax    ; 8MB
        add     edi, 4
        add     eax, 400000h
        mov     DWORD PTR [edi], eax    ; 12MB
        add     edi, 4
        add     eax, 400000h
        mov     DWORD PTR [edi], eax    ; 16MB

;
; Initialize the page attribute table (PAT_TYPE_WB, PAT_TYPE_USWC,
; PAT_TYPE_WEAK_UC, PAT_TYPE_STRONG_UC in the low and high elements of the
; table).
;

        mov     eax, cr0
        mov     ebx, eax
        and     eax, NOT CR0_NW
        or      eax, CR0_CD
        mov     cr0, eax                ; Disable caching and line fill
        wbinvd
        mov     ecx, 277h               ; PAT register
        mov     eax, 00070106h          ; STRONG_UC, WEAK_UC, UWSC, WB
        mov     edx, eax
        wrmsr
        wbinvd
        mov     cr0, ebx                ; Restore cr0

;
; Enable the processor's large page support, FXSR support, and XMMI exception
; handling.
;

        mov     eax, cr4
        or      eax, CR4_PSE + CR4_FXSR + CR4_XMMEXCPT
        mov     cr4, eax

;
; Load the address of the page directory into the processor.
;

        mov     eax, PAGE_DIRECTORY_PHYSICAL_ADDRESS
        mov     cr3, eax

;
; Enable the paging support, and numeric exception support.
;

        mov     eax, cr0
        or      eax, CR0_PG + CR0_WP + CR0_NE
        mov     cr0, eax

        jmp     @F
@@:

;
; Reload the stack segment register with its mapped address.
;

        mov     esp, 080000000h + BLDR_RELOCATED_ORIGIN

;
; Call the C entry point of the boot loader.  Note that we need to load the
; address of the C entry point into a register in order to obtain the absolute
; address of the entry point.  If we call BldrStartup2 directly, then a EIP
; relative call will be generated and we'll run from the boot origin instead of
; the relocated origin.
;

        lea     eax, _BldrStartup2@0
        call    eax

;
; Spin. We should not get here
;

@@:     jmp     @B

_BldrStartup ENDP

;
; BldrLoadMediaROM
;
; Invoked by XBOXKRNL.EXE to load another instance of the ROM from the CD-ROM or
; hard disk.
;

        PUBLIC  @BldrLoadMediaROM@4

@BldrLoadMediaROM@4 PROC

IFNDEF MCP_XMODE3
        
        cli
        mov     edx, ecx

;
; Fill in the page directory with identify mappings for the first 256M of memory
; at both linear address 0x00000000 and 0x80000000.  Large pages (4MB) are used
; for this mapping.  Zero out the rest of the page directory.
;

        mov     edi, 080000000h + PAGE_DIRECTORY_PHYSICAL_ADDRESS
        mov     ecx, 64
        mov     eax, BLDR_VALID_KERNEL_LARGE_PTE_BITS   ; Set valid, write, large Page, PFN=0
@@:
        mov     DWORD PTR [edi+800h], eax
        stosd
        add     eax, 400000h                            ; Advanced PFN to the next 4MB page.
        loop    @B

        mov     ecx, 448
        xor     eax, eax
@@:
        mov     DWORD PTR [edi+800h], eax
        stosd
        loop    @B

;
; Double map the page directory page.
;

        mov     edi, 080000000h + PAGE_DIRECTORY_PHYSICAL_ADDRESS
        mov     eax, PAGE_DIRECTORY_PHYSICAL_ADDRESS + BLDR_VALID_KERNEL_PTE_BITS
        mov     DWORD PTR [edi + ((0C0000000h SHR 22) SHL 2)], eax

;
; Map the shadow ROM into the top of memory.  This can't be done with large
; page mapping, so we borrow the page below the page directory for the
; page table.  We'll map the upper 1MB and leave the rest of it undefined
;
        sub     eax, 01000h
        mov     DWORD PTR [edi + ((0FFF00000h SHR 22) SHL 2)], eax
        sub     edi, 0400h
        lea     eax, [edx + BLDR_VALID_KERNEL_PTE_BITS]
        mov     ecx, 0100h
@@:
        stosd
        add     eax, 01000h
        loop    @B

;
; Load the address of the page directory into the processor.  This should end up
; being the same page directory that we're already running on, but this will
; flush out the stale entries in the TLB.
;

        mov     eax, PAGE_DIRECTORY_PHYSICAL_ADDRESS
        mov     cr3, eax

;
; We're going to be overwriting the current kernel image, including the current
; global descriptor table, so move us to a safe table.
;

        lgdt    FWORD PTR [BldrRestartGDTFWORD]

;
; Execute a 16:32 jump to reload the code selector.
;

        db      0EAh
        dd      OFFSET lmr10
        dw      KGDT_R0_CODE
lmr10:

;
; Load the various segment registers with the expected values.
;

        mov     eax, KGDT_R0_DATA
        mov     ds, eax
        mov     es, eax
        mov     ss, eax
        mov     esp, 080000000h + BLDR_RELOCATED_ORIGIN

        xor     eax, eax
        mov     fs, eax
        mov     gs, eax

;
; Store the virtual address of the shadow ROM in the global pointer so that we
; don't go back to the real ROM.
;

        add     edx, 080000000h + ROM_SHADOW_SIZE
        mov     DWORD PTR [_BldrTopOfROMAddress], edx

;
; Reencrypt the boot loader to restore the original ROM image
;

        call    _BldrReencryptROM@0

;
; Call the C entry point of the boot loader.
;

        lea     eax, _BldrStartup2@0
        call    eax
ENDIF
;
; Spin. We should not get here.
;

@@:     jmp     @B

@BldrLoadMediaROM@4 ENDP

;
; Values to initialize the processor's descriptor tables.
;

        ALIGN   4
BldrRestartGDT LABEL DWORD
        dd      0                       ; KGDT_NULL
        dd      0
        dd      00000FFFFh              ; KGDT_R0_CODE
        dd      000CF9B00h
        dd      00000FFFFh              ; KGDT_R0_DATA
        dd      000CF9300h
BldrRestartGDTEnd LABEL DWORD

        ALIGN   4
BldrRestartGDTFWORD LABEL FWORD
        dw      OFFSET BldrRestartGDTEnd - OFFSET BldrRestartGDT
        dd      OFFSET BldrRestartGDT

_TEXT   ENDS

        END
