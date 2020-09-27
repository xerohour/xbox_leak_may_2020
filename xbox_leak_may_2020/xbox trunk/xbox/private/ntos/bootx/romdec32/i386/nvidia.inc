
COMMAND_QUIT		   EQU 0
COMMAND_TIMING_DELAY       EQU 00000001b
COMMAND_READ_MEM           EQU 00000010b
COMMAND_WRITE_MEM          EQU 00000100b
COMMAND_PCI_CFG_WRITE      EQU 00001000b
COMMAND_PCI_CFG_READ       EQU 00010000b
COMMAND_RMW_RESULT         EQU 00100000b
COMMAND_USE_RESULT_AS_DATA EQU 01000000b

DONT_CARE		   EQU 0

nv_command_struct struct
  nv_command db 0
  nv_address dd 0
  nv_data    dd 0
nv_command_struct ends

;
; Sample Table
;
;data_table_test LABEL BYTE
;c00 nv_command_struct <COMMAND_TIMING_DELAY ,      5 ,                    DONT_CARE >
;c01 nv_command_struct <COMMAND_READ_MEM,           050000000h,            DONT_CARE >
;c02 nv_command_struct <COMMAND_WRITE_MEM,          050000000h,            55aa55aah >
;c04 nv_command_struct <COMMAND_PCI_CFG_WRITE,      80000004h,             7         >
;c05 nv_command_struct <COMMAND_PCI_CFG_READ,       80000004h,             DONT_CARE >
;c06 nv_command_struct <COMMAND_RMW_RESULT,         5,                     00007700h >
;c07 nv_command_struct <COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, 80000004h >
;c19 nv_command_struct <>



  mov  esi, ROMDEC_NVIDIA_TABLE

.repeat

  mov  al, ds:[esi]
  mov  ebx, ds:[esi+1]
  mov  ecx, ds:[esi+5]
  
  .if al & COMMAND_USE_RESULT_AS_DATA
    mov  edx, ecx
    mov  al, bl
    mov  ebx, edx
    mov  ecx, edi
  .endif
  
  .if al & COMMAND_TIMING_DELAY
; cx will already have the io's to do.
    mov  dx, 80h
    rep  insb
  .elseif al & COMMAND_READ_MEM
    mov  edi, [ebx]
  .elseif al & COMMAND_WRITE_MEM
    mov  [ebx], ecx
  .elseif al & COMMAND_RMW_RESULT
    and  edi, ebx
    or   edi, ecx
  .elseif al & COMMAND_PCI_CFG_WRITE 
    mov  eax, ebx
    mov  dx, 0cf8h
    out  dx, eax
    add  dx, 4
    mov  eax, ecx
    out  dx,eax
  .elseif al & COMMAND_PCI_CFG_READ
    mov  eax, ebx
    mov  dx, 0cf8h
    out  dx, eax
    add  dx, 4
    in   eax, dx
    mov  edi, eax
  .elseif al == COMMAND_QUIT
    mov  esi, -9
  .endif
  add  esi, SIZEOF nv_command_struct
.until esi == 0

