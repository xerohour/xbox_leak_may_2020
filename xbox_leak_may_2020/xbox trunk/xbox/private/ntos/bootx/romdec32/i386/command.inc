IFDEF MCP_XMODE2

;
; Opcodes for devkits (XMODE2)

COMMAND_RESERVED             EQU 0E7h
COMMAND_READ_MEM             EQU 09Ah
COMMAND_WRITE_MEM            EQU 05Bh
COMMAND_PCI_CFG_WRITE        EQU 0F9h
COMMAND_PCI_CFG_READ         EQU 0F5h
COMMAND_RMW_RESULT           EQU 0EDh
COMMAND_USE_RESULT_AS_DATA   EQU 068h
COMMAND_COMPARE_RESULT_JNE   EQU 004h
COMMAND_JMP                  EQU 025h
COMMAND_RMW_ACCUM            EQU 06Ch
COMMAND_OUT_BYTE             EQU 03Ch
COMMAND_IN_BYTE              EQU 0C8h
COMMAND_QUIT                 EQU 0BFh

ELSE

;
; Opcodes for internal ROM (XMODE3)
;

COMMAND_RESERVED             EQU 1h
COMMAND_READ_MEM             EQU 2h
COMMAND_WRITE_MEM            EQU 3h
COMMAND_PCI_CFG_WRITE        EQU 4h
COMMAND_PCI_CFG_READ         EQU 5h
COMMAND_RMW_RESULT           EQU 6h
COMMAND_USE_RESULT_AS_DATA   EQU 7h
COMMAND_COMPARE_RESULT_JNE   EQU 8h
COMMAND_JMP                  EQU 9h
COMMAND_RMW_ACCUM            EQU 10h
COMMAND_OUT_BYTE             EQU 11h
COMMAND_IN_BYTE              EQU 12h
COMMAND_QUIT                 EQU 0EEh

COMMAND_UNUSED1              EQU 0F5h
COMMAND_UNUSED2              EQU 080h

ENDIF


DONT_CARE                    EQU 0

nv_command_struct struct
        nv_command db 0
        nv_address dd 0
        nv_data    dd 0
nv_command_struct ends


