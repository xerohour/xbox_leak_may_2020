;       Copyright (C) 1996-1997 NVidia Corporation
;
;       NV_MAC.INC - NVidia specific macro definitions for the VxD environment
;
;       Written by:     Mark Stephen Krueger
;       Date:           7/22/96
;
;       $Revision: 5 $
;       $Date: 3/10/98 10:36a $
;       $Logfile: /resman.040/win96/vdd/nv_mac.inc $
;       $Log: /resman.040/win96/vdd/nv_mac.inc $
; 
; 5     3/10/98 10:36a Jhinman
; Fix bug in reg_rd08 (read 32).
; 
; 4     2/13/98 3:36p Jhinman
; Add reg_wr08/reg_rd08 macros.
; 
; 3     1/09/98 2:03p Jhinman
; Address of chip (nvAddr) is now local, since RM is separate from
; miniVDD. Removed the underscore, since its not a "C" declaration.
; 
; 2     10/30/97 6:43p Jhinman
; Add underscore to nvAddr, since its now declared in osapi.c.
; 
; 1     5/13/97 10:43a Jeffwe
; 
; 1     5/12/97 6:49p Jeffwe
; 
; 4     1/23/97 10:57a Markk
; Removed tabs.
; 
; 3     11/15/96 6:07p Markk
; Added code for simulating NV accesses.
; 
; 2     11/13/96 12:33p Markk
; Fixed bugs with REG_WR32 and REG_RD32 macros.
; 
; 1     10/16/96 9:15a Markk
; Contains NV macros for the VDD environment.
;

;
;       REG_WR32 - Writes the value in eax to an NV register
;
;       Parameters: nv_address - Address to write to (optional)
;       Entry:  esi - Address to write to (optional)
;               eax - Value
;       Exit:   None
;       Return: None
;       Uses:   esi (if esi not already loaded with address)
;
REG_WR32 macro nv_address:=<esi>, data
    ifnb <data>
        mov     eax,data
    endif
IFDEF NVIDIA_SIMULATE
    ifidni <nv_address>, <esi>
        push    eax
        push    ebx
        mov     ebx,eax
        mov     eax,esi
        call    NVSimulateWriteReg
        pop     ebx
        pop     eax
    else
        mov     esi,nv_address
        push    eax
        push    ebx
        mov     ebx,eax
        mov     eax,esi
        call    NVSimulateWriteReg
        pop     ebx
        pop     eax
    endif
ELSE ; NVIDIA_SIMULATE
    ifidni <nv_address>, <esi>
        add     esi,nvAddr
        mov     [esi],eax
        sub     esi,nvAddr
    else
        mov     esi,nv_address
        add     esi,nvAddr
        mov     [esi],eax
;        sub     esi,nvAddr
    endif
ENDIF ; NVIDIA_SIMULATE
    exitm <>
endm
;
;       REG_RD32 - Reads the value in an NV register into eax
;
;       Parameters: nv_address - Address to read from  (optional)
;       Entry:  esi - Address to read from  (optional)
;       Exit:   eax - Value
;       Return: None
;       Uses:   esi (if esi not already loaded with address)
;
REG_RD32 macro nv_address:=<esi>
IFDEF NVIDIA_SIMULATE
    ifidni <nv_address>, <esi>
        push    ebx
        mov     eax,esi
        call    NVSimulateReadReg
        mov     eax,ebx
        pop     ebx
    else
        mov     esi,nv_address
        push    ebx
        mov     eax,esi
        call    NVSimulateReadReg
        mov     eax,ebx
        pop     ebx
    endif
ELSE ; NVIDIA_SIMULATE
    ifidni <nv_address>, <esi>
        add     esi,nvAddr
        mov     eax,[esi]
        sub     esi,nvAddr
    else
        mov     esi,nv_address
        add     esi,nvAddr
        mov     eax,[esi]         
;        sub     esi,nvAddr
    endif
ENDIF ; NVIDIA_SIMULATE
    exitm <>
endm
;
;
;       REG_WR08 - Writes the value in eax to an NV register
;
;       Parameters: nv_address - Address to write to (optional)
;       Entry:  esi - Address to write to (optional)
;               al - Value
;       Exit:   None
;       Return: None
;       Uses:   esi (if esi not already loaded with address)
;
REG_WR08 macro nv_address:=<esi>, data
    ifnb <data>
        mov     eax,data
    endif
    ifidni <nv_address>, <esi>
        add     esi,nvAddr
        mov     [esi],al
        sub     esi,nvAddr
    else
        mov     esi,nv_address
        add     esi,nvAddr
        mov     [esi],al
    endif
    exitm <>
endm
;
;       REG_RD08 - Reads the value in an NV register into eax
;
;       Parameters: nv_address - Address to read from  (optional)
;       Entry:  esi - Address to read from  (optional)
;       Exit:   al - Value
;       Return: None
;       Uses:   esi (if esi not already loaded with address)
;
REG_RD08 macro nv_address:=<esi>
    ifidni <nv_address>, <esi>
        add     esi,nvAddr
        mov     al,[esi]
        sub     esi,nvAddr
    else
        mov     esi,nv_address
        add     esi,nvAddr
        mov     al,[esi]         
    endif
    exitm <>
endm

;----------------------------------------------------------------------
; hex = 0x00000000
; decimal = 0
; Note: if esi and eax usage is hard to work with, could add push/pop's
;----------------------------------------------------------------------
;
;       BIT(b) - Returns a bit mask with bit 'b' set
;
;       Parameters: b - Bit
;       Entry:  None
;       Exit:   None
;       Return: Mask
;
;#define BIT(b)                  (1<<(b))
BIT macro b
    exitm %(1 SHL b)
endm
;
;       DEVICE_BASE(d) - Returns the base address of a device
;
;       Parameters: d - Device
;       Entry:  None
;       Exit:   None
;       Return: Base address in hex
;
;       Requires that definition of device be hex:hex
;
;#define DEVICE_BASE(d)          (0?d)
DEVICE_BASE macro nv_device
    local pos
    pos = @InStr(1,%nv_device,<:>)
    exitm @CatStr(@SubStr( %nv_device, %pos + 3, %@SizeStr(%nv_device) - pos - 2),<h>)
endm
;
;       DRF_SHIFT(drf) - Returns the least significant bit of a field
;
;       Parameters: drf - Field
;       Entry:  None
;       Exit:   None
;       Return: Bit
;
;       Requires that definition of drf be decimal:decimal
;
;#define DRF_SHIFT(drf)          (0?drf)
DRF_SHIFT macro drf
        local pos
;        echo    DRF_SHIFT
;%       echo    drf
    pos = @InStr(1,%drf,<:>)
;temp3   TEXTEQU %pos
;%       echo    temp3
    exitm @SubStr( %drf, %pos + 1, %@SizeStr(%drf) - pos)
endm
;
;       DRF_MASK(drf) - Returns the bit mask of a field
;
;       Parameters: drf - Field
;       Entry:  None
;       Exit:   None
;       Return: Mask
;
;       Requires that definition of drf be decimal:decimal
;
;#define DRF_MASK(drf)           (0xFFFFFFFF>>(31-(1?drf)+(0?drf)))
DRF_MASK macro drf
    local pos, value1, value0, result
    pos = @InStr(1,%drf,<:>)
;temp3 TEXTEQU %pos
;%   echo temp3
    value1 = @SubStr( %drf, 1, %pos - 1)
    value0 = @SubStr( %drf, %pos + 1, %@SizeStr(%drf) - pos)
;    result TEXTEQU %(0FFFFFFFFh SHR (31 - value1 + value0))
;temp3 TEXTEQU %result
;%   echo temp3
    exitm %(0FFFFFFFFh SHR (31 - value1 + value0))
endm
;
;       DRF_DEF(d,r,f,c) - Returns the value of a constant shifted into a field
;
;       Parameters: d - Device
;                   r - Register
;                   f - Field
;                   c - Constant
;       Entry:  None
;       Exit:   None
;       Return: Value
;
;#define DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV ## d ## r ## f))
DRF_DEF macro d, r, f, c
    local value, shift
;    value = %@CatStr(<NV>, %d, %r, %f, %c)
    value = @CatStr(<NV>, d, r, f, c)
;    shift = %DRF_SHIFT(%@CatStr(<NV>, %d, %r, %f))
    shift = DRF_SHIFT(@CatStr(<NV>, d, r, f))
    exitm %(value SHL shift)
endm
;
;       DRF_NUM(d,r,f,n) - Returns the value of a number shifted into a field
;
;       Parameters: d - Device
;                   r - Register
;                   f - Field
;                   n - Number
;       Entry:  None
;       Exit:   eax - result, if number is not a constant value
;       Return: Value, if number is a constant value
;
;#define DRF_NUM(d,r,f,n)        (((n)&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
DRF_NUM macro d, r, f, n
    local value, shift
    value = DRF_MASK(@CatStr(<NV>, d, r, f))
    shift = DRF_SHIFT(@CatStr(<NV>, d, r, f))
    if (((opattr n) AND 4) NE 0)        ; Check for immediate value
        exitm %((n AND value) SHL shift)
    else                                ; Must be non-immediate, move to eax
        ifdifi <n>, <eax>               ; See if its already in eax
                if ((type n) NE 4)      ; Make sure its a dword value
                        .err <Must pass a dword number for n in DRF_NUM>
                else
                        mov     eax, n
                endif
        endif
        and     eax, value
        if (shift gt 0)
            shl     eax, shift
        endif
        exitm <>
    endif
endm
;
;       DRF_VAL(d,r,f,v) - Returns the contents of a field within a value
;
;       Parameters: d - Device
;                   r - Register
;                   f - Field
;                   v - Value
;       Entry:  None
;       Exit:   eax - result, if number is not a constant value
;       Return: Value, if number is a constant value
;
;#define DRF_VAL(d,r,f,v)        (((v)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
DRF_VAL macro d, r, f, v:=<eax>
    local value, shift
    value = DRF_MASK(@CatStr(<NV>, d, r, f))
    shift = DRF_SHIFT(@CatStr(<NV>, d, r, f))
    if (((opattr v) AND 4) NE 0)        ; Check for immediate value
            exitm %((v SHR shift) AND value)
    else                                ; Must be non-immediate, move to eax
        ifdifi <v>, <eax>               ; See if its already in eax
                if ((type v) NE 4)      ; Make sure its a dword value
                        .err <Must pass a dword number for n in DRF_NUM>
                else
                        mov     eax, v
                endif
        endif
        if (shift gt 0)
            shr     eax, shift
        endif
        and     eax, value
        exitm <>
    endif
endm
;
;       REG_WR_DRF_NUM(d,r,f,n) - Writes a field number into an NV register
;
;       Parameters: d - Device
;                   r - Register
;                   f - Field
;                   n - Number
;       Entry:  None
;       Exit:   eax - result, if number is not a constant value
;       Return: Value, if number is a constant value
;       Uses:   esi, eax
;
;#define REG_WR_DRF_NUM(d,r,f,n) REG_WR32(NV ## d ## r, DRF_NUM(d,r,f,n))
REG_WR_DRF_NUM macro d, r, f, n:=<eax>
    mov     esi,@CatStr(<NV>,d,r)
    if (((opattr n) AND 4) NE 0)        ; Check for immediate value
            mov     eax,DRF_NUM(d,r,f,n)
    else                                ; Must be non-immediate, move to eax
        DRF_NUM(d,r,f,n)
    endif
    REG_WR32 esi
    exitm <>
endm
;
;       REG_WR_DRF_DEF(d,r,f,c) - Writes a field constant into an NV register
;
;       Parameters: d - Device
;                   r - Register
;                   f - Field
;                   c - Constant
;       Entry:  None
;       Exit:   None
;       Return: None
;       Uses:   esi, eax
;
;#define REG_WR_DRF_DEF(d,r,f,c) REG_WR32(NV ## d ## r, DRF_DEF(d,r,f,c))
REG_WR_DRF_DEF macro d, r, f, c
    mov     esi,@CatStr(<NV>,d,r)
    mov     eax,DRF_DEF(d,r,f,c)
    REG_WR32 esi
    exitm <>
endm
;
;       FLD_WR_DRF_NUM(d,r,f,n) - Writes a number into an NV field
;
;       Parameters: d - Device
;                   r - Register
;                   f - Field
;                   n - Number
;       Entry:  None
;       Exit:   None
;       Return: None
;       Uses:   esi, eax, ebx
;
;#define FLD_WR_DRF_NUM(d,r,f,n) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_NUM(d,r,f,n))
FLD_WR_DRF_NUM macro d, r, f, n:=<eax>
    ifidni  <n>, <eax>                  ; See if its already in eax
        push    eax                     ; Save it on stack
    endif
    mov     esi,@CatStr(<NV>,d,r)
    REG_RD32  esi
    and     eax,NOT (DRF_MASK(@CatStr(<NV>, d, r, f)) SHL DRF_SHIFT(@CatStr(<NV>, d, r, f)))
    
    ifidni  <n>, <eax>                  ; See if its already in eax
        mov     ebx, eax                ; Move read value into ebx
        pop     eax                     ; Restore eax for DRF_NUM function (n=eax)
        DRF_NUM(d,r,f,n)                ; Loads number into eax (same as DRF_NUM(d,r,f,eax))
        or      eax, ebx                ; OR with value read
    elseif (((opattr n) AND 4) NE 0)    ; Check for immediate value
        or     eax,DRF_NUM(d,r,f,n)
    else                                ; Must be non-immediate, move to eax
        push    eax
        DRF_NUM(d,r,f,n)                ; Loads number into eax
        pop     ebx                     ; Pop saved eax into ebx
        or      eax, ebx
    endif
    REG_WR32 esi
    exitm <>
endm
;
;       FLD_WR_DRF_DEF(d,r,f,c) - Writes a constant into an NV field
;
;       Parameters: d - Device
;                   r - Register
;                   f - Field
;                   c - Constant
;       Entry:  None
;       Exit:   None
;       Return: None
;       Uses:   esi, eax
;
;#define FLD_WR_DRF_DEF(d,r,f,c) REG_WR32(NV##d##r,(REG_RD32(NV##d##r)&~(DRF_MASK(NV##d##r##f)<<DRF_SHIFT(NV##d##r##f)))|DRF_DEF(d,r,f,c))
FLD_WR_DRF_DEF macro d, r, f, c
    mov     esi,@CatStr(<NV>,d,r)
    REG_RD32  esi
    and     eax,NOT (DRF_MASK(@CatStr(<NV>, d, r, f)) SHL DRF_SHIFT(@CatStr(<NV>, d, r, f)))
    or      eax,DRF_DEF(d,r,f,c)
    REG_WR32 esi
    exitm <>
endm
;
;       REG_RD_DRF(d,r,f) - Reads the contents of an NV field
;
;       Parameters: d - Device
;                   r - Register
;                   f - Field
;       Entry:  None
;       Exit:   None
;       Return: eax - Field contents
;       Uses:   esi
;
;#define REG_RD_DRF(d,r,f)       (((REG_RD32(NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
REG_RD_DRF macro d, r, f
    local shift
    shift = DRF_SHIFT(@CatStr(<NV>, d, r, f))
    mov     esi,@CatStr(<NV>,d,r)
    REG_RD32(esi)
;temp9 TEXTEQU %shift
;%   echo temp9
    if (shift gt 0)
        shr     eax,shift
    endif
    and     eax,DRF_MASK(@CatStr(<NV>, d, r, f))
    exitm <>
endm
; Dumb macro to read PCI Config space.
ReadPCI	macro
  push   eax
  
  mov    dx, 0cf8h
  out    dx, eax
  mov    dx, 0cfch
  in     eax, dx
  mov    edx,eax
  
  pop    eax
endm
