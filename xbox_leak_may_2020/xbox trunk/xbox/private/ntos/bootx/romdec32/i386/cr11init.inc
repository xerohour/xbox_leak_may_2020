;++
;
;  Copyright (c) Microsoft Corporation. All rights reserved
;
;  Module Name:
;
;    cr11init.inc
;
;  Abstract:
;
;    Crush11 initialization table
;
;    This module contains table of commands used by South Bridge to initialize
;    hardware and test memory during boot startup.  For definition of each
;    command, please see initcode.inc and command.inc
;--

        page    84,132

        .MODEL  compact
        .486p

DATA_TABLE_OFFSET  EQU 0FFFFFE00h

        INCLUDE command.inc

CODE    SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:CODE, ES:NOTHING, SS:NOTHING
        ORG     0000H

IFDEF OLDNBDATA

;
; NB data for Crush 11 on the emulator
;

        dd      2B16D065h
        dd      (1 + (1 SHL 2))                 ; Intel
        dd      0
        dd      7 SHL 28
        dd      11 SHL 28
        dd      0
        dd      0
        dd      0
        dd      0
        dd      0FFFFFF7Fh
        dd      0FFFFFFFFh
        dd      (28-11) dup (0h)
        dd      0Fh
        dd      00000000h                       ; Intel
        dd      0
        dd      0
        dd      19234121h, 25268001h, 19234121h, 25268001h
        dd      19234121h, 25268001h, 19234121h, 25268001h
        dd      19234121h, 25268001h, 19234121h, 25268001h
        dd      19234121h, 25268001h, 19234121h, 25268001h
        dd      19234121h, 25268001h, 19234121h, 25268001h
        dd      19234121h, 25268001h, 19234121h, 25268001h
        dd      19234121h, 25268001h, 19234121h, 25268001h
        dd      19234121h, 25268001h, 19234121h, 25268001h

ELSE

IFDEF FIRST_TRY

;
; NB data for Crush 11 on the EVT boards
;

        db      065h, 0D0h, 016h, 02Bh          ; 2B16D065
        db      04Dh, 043h, 046h, 033h          ; 3346434D
        db      003h, 003h, 003h, 003h          ; 03030303
        db      00Eh, 015h, 01Ch, 01Ch          ; 1C1C150E
        db      003h, 015h, 000h, 000h          ; 00001503
        db      08Ah, 070h, 0E4h, 0A8h          ; A8E4708A
        db      030h, 002h, 0FDh, 045h          ; 45FD0230
        db      001h, 000h, 0E2h, 010h          ; 10E20001
        db      000h, 000h, 000h, 0F0h          ; F0000000
        dd      (55) dup (0FFFFFFFFh)           ; FFFFFFFF, ETC

ELSE

;
; NB data for Crush 11 on the EVT boards
;

        dd      2B16D065h
        dd      4444444Dh                       ; Intel
        dd      03030303h
        dd      15151515h
        dd      1503h
        dd      0A8E4708Ah
        dd      45FD0230h
        dd      10E20001h
        dd      0F0000000h
        dd      0FFFFFFFFh
        dd      0FFFFFFFFh
        dd      (28-11) dup (0h)                ; 4 DWORD follow to fill up to 128 bytes

;
; SB data for MCP1 on the EVT boards
;

        dd      0Fh
        dd      00000000h                       ; Intel
        dd      0                               ; pad for DWORD 30
        dd      0                               ; pad for DWORD 31 (32*4 = byte location 128)

;
; Force 256 byte alignment.
;

        db      (080h) dup (0)                  ; dup 0's into remaining 0x80/128 bytes

ENDIF   ; FIRST_TRY

ENDIF   ; OLDNBDATA


        ORG     0100h

data_table_test LABEL BYTE

;c00 nv_command_struct <COMMAND_TIMING_DELAY ,      5 ,                    DONT_CARE >
;c01 nv_command_struct <COMMAND_READ_MEM,           050000000h,            DONT_CARE >
;c02 nv_command_struct <COMMAND_WRITE_MEM,          050000000h,            55aa55aah >
;c04 nv_command_struct <COMMAND_PCI_CFG_WRITE,      80000004h,             7         >
;c05 nv_command_struct <COMMAND_PCI_CFG_READ,       80000004h,             DONT_CARE >
;c06 nv_command_struct <COMMAND_RMW_RESULT,         5,                     00007700h >
;c07 nv_command_struct <COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, 80000004h >

;
; Crush11 init sequence.
;

nv_command_struct< COMMAND_PCI_CFG_WRITE, 8000017Ch, 170F17C0h >        ; CR_CMC_CFG0, 0x170f17c0 | CR_CMC_CFG0_PART_INTLV_32B = 0
nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000164h, 00000001h >        ; CR_CMC_NVM, 0x1
nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000240h, 00110401h >        ; CR_XL_DIMM_CFG_0, 0x1 | 0x10000 | 0x100000 | 0x0 | 0x400 = 0x110401
nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000170h, 00000038h >        ; CR_CMC_MEMIO_CFG0, 0x38
nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000190h, 22228807h >        ; CR_CMC_TIMING0, 0x22228807

nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000194h, 22452250h >        ; CR_CMC_TIMING1, 0x22452250
nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000198h, 036900FFh >        ; CR_CMC_TIMING2, 0x032100FF / 0x036900FF
nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000178h, 0100021Fh >        ; CR_CMC_MEMIO_CFG2, 0x0100021F
nv_command_struct< COMMAND_PCI_CFG_WRITE, 8000019Ch, 00000020h >        ; CR_CMC_ARB_PREDIVIDER, 0x20
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001C0h, 00000013h >        ; CR_CMC_ARB, 0x13

nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001CCh, 00028880h >        ; CR_CMC_ARB_TIMEOUT, 0x28880
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001D0h, 00004C40h >        ; CR_CMC_ARB_XFER_SZ, 0x4C40
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001D4h, 00002220h >        ; CR_CMC_ARB_XFER_REM, 0x2220
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001D8h, 0000000Ch >        ; CR_CMC_ARB_DIFF_BANK, 0xC
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001DCh, 0000000Fh >        ; CR_CMC_CLOSE_PAGE0, 0xF

nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001E0h, 0000000Bh >        ; CR_CMC_CLOSE_PAGE1, 0xB
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001E4h, 000001CFh >        ; CR_CMC_CLOSE_PAGE2, 0x1CF
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001ECh, 00000F31h >        ; CR_CMC_AUTOCLOSE, 0xF31
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001F0h, 11001018h >        ; CR_CMC_WBC, 0x11001018
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001F8h, 00000401h >        ; CR_CMC_CPU_RRQ, 0x401

nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001FCh, 02750031h >        ; CR_CMC_BYPASS, 0x2750031
nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000168h, 00000001h >        ; CR_CMC_PIN, 0x1
nv_command_struct< COMMAND_PCI_CFG_WRITE, 8000016Ch, 00000001h >        ; CR_CMC_PAD, 0x1
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001E8h, 24924488h >        ; CR_CMC_CMDQ, 0x24924488
nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001F4h, 00001008h >        ; CR_CMC_CMDQ_PRT, 0x1008

nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001c8h, 00000001h >        ; CR_CMC_PRE, 0x1

;.repeat
;  in   eax,dx
;  not  eax
;.until eax & 1

nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001a0h, 8000002Ah >        ; CR_CMC_MRS_DIMM0, 0x8000002A

;.repeat
;  in   eax,dx
;  not  eax
;.until eax & 80000000h

nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001a4h, 80200001h >        ; CR_CMC_EMRS_DIMM0, 0x80200001

;.repeat
;  in   eax,dx
;  not  eax
;.until eax & 80000000h

nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001c8h, 00000001h >        ; CR_CMC_PRE, 0x1

;.repeat
;  in   eax,dx
;  not  eax
;.until eax & 1

; do 8 memory refresh cycles:

nv_command_struct< COMMAND_PCI_CFG_WRITE, 800001c4h, 00000001h >        ; CR_CMC_REF, 0x1

;.repeat
;  in   eax,dx
;  not  eax
;.until eax & 1

nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000168h, 00000001h >        ; CR_CMC_PIN, 0x1
nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000160h, 80000000h >        ; CR_CMC_REFCTRL, 0x80000000
nv_command_struct< COMMAND_PCI_CFG_WRITE, 80000084h, 03000000h >        ; CR_CPU_MEMTOP, 0x3000000 = 64MB limit (as a RMW, reads as 0x3FFFFFF)

IFDEF MEMTEST

;
; Memory test
;

nv_command_struct< COMMAND_RMW_ACCUM, 0, 0 >                              ; clear accumulator

;
; dram0 chip
;

dram0_begin:

MemBase = 0
MemChip = 0

nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 002555558h), 05A5A5A5Ah >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 000555554h), 0FFFFFFFFh >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 00055555Ch), 0FFFFFFFFh >

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 05A5A5A5Ah, (dram0_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555554h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0FFFFFFFFh, (dram0_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0A5A5A5A5h, (dram0_2MB-$-4) >    ; if edi != val, jump

dram0_4mb:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) >             ; indicate 4mb for this chip
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram0_end-$-4) >

dram0_2mb:
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram0_end-$-4) >

dram0_err:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) shl 8 >       ; indicate error for this chip

dram0_end:

;
; dram1 chip
;

dram1_begin:

MemBase = 010h
MemChip = 1

nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 002555558h), 05A5A5A5Ah >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 000555554h), 0FFFFFFFFh >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 00055555Ch), 0FFFFFFFFh >

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 05A5A5A5Ah, (dram1_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555554h), DONT_CARE>         ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0FFFFFFFFh, (dram1_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0A5A5A5A5h, (dram1_2MB-$-4) >    ; if edi != val, jump

dram1_4mb:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) >             ; indicate 4mb for this chip
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram1_end-$-4) >

dram1_2mb:
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram1_end-$-4) >

dram1_err:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) shl 8 >       ; indicate error for this chip

dram1_end:

;
; dram2 chip
;

dram2_begin:

MemBase = 020h
MemChip = 2

nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 002555558h), 05A5A5A5Ah >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 000555554h), 0FFFFFFFFh >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 00055555Ch), 0FFFFFFFFh >

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 05A5A5A5Ah, (dram2_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555554h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0FFFFFFFFh, (dram2_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0A5A5A5A5h, (dram2_2MB-$-4)>     ; if edi != val, jump

dram2_4mb:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) >             ; indicate 4mb for this chip
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram2_end-$-4) >

dram2_2mb:
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram2_end-$-4) >

dram2_err:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) shl 8 >       ; indicate error for this chip

dram2_end:

;
; dram3 chip
;

dram3_begin:

MemBase = 030h
MemChip = 3

nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 002555558h), 05A5A5A5Ah >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 000555554h), 0FFFFFFFFh >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 00055555Ch), 0FFFFFFFFh >

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 05A5A5A5Ah, (dram3_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555554h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0FFFFFFFFh, (dram3_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0A5A5A5A5h, (dram3_2MB-$-4) >    ; if edi != val, jump

dram3_4mb:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) >             ; indicate 4mb for this chip
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram3_end-$-4) >

dram3_2mb:
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram3_end-$-4) >

dram3_err:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) shl 8 >       ; indicate error for this chip

dram3_end:

;
; dram4 chip
;

dram4_begin:

MemBase = 04000000h
MemChip = 4

nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 002555558h), 05A5A5A5Ah >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 000555554h), 0FFFFFFFFh >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 00055555Ch), 0FFFFFFFFh >

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 05A5A5A5Ah, (dram4_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555554h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0FFFFFFFFh, (dram4_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0A5A5A5A5h, (dram4_2MB-$-4) >    ; if edi != val, jump

dram4_4mb:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) >             ; indicate 4mb for this chip
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram4_end-$-4) >

dram4_2mb:
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram4_end-$-4) >

dram4_err:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) shl 8 >       ; indicate error for this chip

dram4_end:

;
; dram5 chip
;

dram5_begin:

MemBase = 04000010h
MemChip = 5

nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 002555558h), 05A5A5A5Ah >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 000555554h), 0FFFFFFFFh >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 00055555Ch), 0FFFFFFFFh >

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 05A5A5A5Ah, (dram5_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555554h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0FFFFFFFFh, (dram5_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0A5A5A5A5h, (dram5_2MB-$-4) >    ; if edi != val, jump

dram5_4mb:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) >             ; indicate 4mb for this chip
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram5_end-$-4) >

dram5_2mb:
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram5_end-$-4) >

dram5_err:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) shl 8 >       ; indicate error for this chip

dram5_end:

;
; dram6 chip
;

dram6_begin:

MemBase = 04000020h
MemChip = 6

nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 002555558h), 05A5A5A5Ah >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 000555554h), 0FFFFFFFFh >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 00055555Ch), 0FFFFFFFFh >

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 05A5A5A5Ah, (dram6_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555554h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0FFFFFFFFh, (dram6_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0A5A5A5A5h, (dram6_2MB-$-4) >    ; if edi != val, jump

dram6_4mb:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) >             ; indicate 4mb for this chip
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram6_end-$-4) >

dram6_2mb:
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram6_end-$-4) >

dram6_err:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) shl 8 >       ; indicate error for this chip

dram6_end:

;
; dram7 chip
;

dram7_begin:

MemBase = 04000030h
MemChip = 7

nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 002555558h), 05A5A5A5Ah >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 000555554h), 0FFFFFFFFh >
nv_command_struct< COMMAND_WRITE_MEM, (MemBase + 00055555Ch), 0FFFFFFFFh >

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 05A5A5A5Ah, (dram7_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555554h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0FFFFFFFFh, (dram7_err-$-4) >    ; if edi != val, jump

nv_command_struct< COMMAND_READ_MEM, (MemBase + 002555558h), DONT_CARE >        ; read mem in edi
nv_command_struct< COMMAND_COMPARE_RESULT_JNE, 0A5A5A5A5h, (dram7_2MB-$-4) >    ; if edi != val, jump

dram7_4mb:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) >             ; indicate 4mb for this chip
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram7_end-$-4) >

dram7_2mb:
nv_command_struct< COMMAND_JMP, DONT_CARE, (dram7_end-$-4) >

dram7_err:
nv_command_struct< COMMAND_RMW_ACCUM, 0FFFFFFFFh, (1 shl MemChip) shl 8 >       ; indicate error for this chip

dram7_end:

ENDIF

;
; End of table
;

nv_command_struct < COMMAND_QUIT, DONT_CARE, DONT_CARE >

        ORG     07FFh           ; 0FFFFh

        db      00h

CODE    ENDS


