;++
;
;  Copyright (c) Microsoft Corporation. All rights reserved
;
;  Module Name:
;
;    initcode.inc
;
;  Abstract:
;
;    This module contains code to parse and process the init table opcodes
;--


;
; Load the start address of the init table 
;        
        mov  esi, ROMDEC_INIT_TABLE


InitLoop:
;
; Load command and parameters
;        
        mov     al, ds:[esi]
        mov     ebx, ds:[esi+1]
        mov     ecx, ds:[esi+5]
        
        .if al == COMMAND_USE_RESULT_AS_DATA
;
; Use Result As Data Command 
;        
        mov     edx, ecx
        mov     al, bl
        mov     ebx, edx
        mov     ecx, edi
        .endif


IFDEF MCP_REV_B01
        .if al == COMMAND_RESERVED
;
; This used to be the "Timing Delay Command" but was broken and not used
;
        mov     dx, 80h
        rep     insb

        .elseif al == COMMAND_READ_MEM
ELSE
        .if al == COMMAND_READ_MEM
        
ENDIF   
;
; Read Memory Command 
;        
;    Disallow read memory for top of address space.  The Rev B01
;    implementation used a cmp, others use an and instruction
;
        
IFDEF MCP_REV_B01

IFDEF MCP_XMODE2
        and     ebx, 00FFFFFFFh    
        mov     edi, [ebx]
ELSE
        cmp     ebx, 0FF000000h 
        ja      ReadNotAllowed
        mov     edi, [ebx]

ReadNotAllowed:
ENDIF
        
ELSE    
        and     ebx, 00FFFFFFFh    
        mov     edi, [ebx]
ENDIF   

        .elseif al == COMMAND_WRITE_MEM
;
; Write Memory Command
;        
        mov     [ebx], ecx

        .elseif al == COMMAND_RMW_RESULT
;
; Read Modify Write Result Command
;        
        and     edi, ebx
        or      edi, ecx
        
        .elseif al == COMMAND_PCI_CFG_WRITE 
        
IFNDEF MCP_XMODE2
;
; PCI Config Write Command
;
;    Check to see if the PCI write is for the General SM Ctrl 1 register
;    which is used to turn off the SB ROM.  If it is, turn off the bit 1
;    
        cmp     ebx, 80000880h
        jnz     OKToWrite
        and     ecx, 0FFFFFFFDh 
                
OKToWrite:    

ENDIF
        mov     eax, ebx
        mov     dx, 0cf8h
        out     dx, eax
        add     dl, 4
        mov     eax, ecx
        out     dx,eax

        .elseif al == COMMAND_PCI_CFG_READ
;
; PCI Config Read Command
;        
        mov     eax, ebx
        mov     dx, 0cf8h
        out     dx, eax
        add     dl, 4
        in      eax, dx
        mov     edi, eax

        .elseif al == COMMAND_COMPARE_RESULT_JNE
;
; Compare Result Jump Command
;       
        .if edi != ebx
        add     esi, ecx
        .endif
        
        .elseif al == COMMAND_JMP
;
; Jump Command  
;       
        add  esi, ecx

        .elseif al == COMMAND_RMW_ACCUM
;
; Read Modify Write Accumulator Command
;        
        and     ebp, ebx
        or      ebp, ecx
        mov     edi, ebp
        

        .elseif al == COMMAND_OUT_BYTE
;
; OUT
;       
        mov     edx, ebx
        mov     eax, ecx
        out     dx, al

        .elseif al == COMMAND_IN_BYTE
;
; IN
;       
        mov     edx, ebx
        in      al, dx
        movzx   edi, al
        
        .elseif al == COMMAND_QUIT
        jmp     InitDone
        .endif
        
;
; Advance to next entry in the table
;
Continue:
        add     esi, SIZEOF nv_command_struct
        
        jmp     InitLoop

InitDone:       

