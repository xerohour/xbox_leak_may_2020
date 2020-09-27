;       Copyright (C) 1996-1997 NVidia Corporation
;
;       NV_SIM.INC - NVidia simulation specific macro definitions
;
;       Written by:     Mark Stephen Krueger
;       Date:           9/13/96
;
;       $Revision: 1 $
;       $Date: 5/13/97 10:43a $
;       $Logfile: /resman.040/win96/vdd/nv_sim.inc $
;       $Log: /resman.040/win96/vdd/nv_sim.inc $
; 
; 1     5/13/97 10:43a Jeffwe
; 
; 1     5/12/97 6:49p Jeffwe
; 
; 2     1/23/97 10:58a Markk
; Removed tabs.
; Fixed copyright.
; 
; 1     11/15/96 6:12p Markk
; Used for simulating NV3.
; 
; 1     9/24/96 3:44p Markk
; Created from part of nv_mac.inc.
; 
;
;       SIM_NO - Simulate an out to an NV I/O port
;
;       Parameters: port - dx or constant I/O port below 00100h
;                   value - ax or al
;       Entry:  dx - I/O port (optional)
;               ax/al - Value
;       Exit:   None
;       Return: None
;       Uses:   None
;
SIM_NO macro port:req, value:req
    IFDEF SERIAL_OUT_OPS
        ifidni <port>, <DX>
            ifidni <value>, <AX>
                call    NVLogOutWord
            else
                call    NVLogOutByte
            endif
        else
            ifidni <value>, <AX>
                push    dx
                mov     dx, port
                call    NVLogOutWord
                pop     dx
            else
                push    dx
                mov     dx, port
                call    NVLogOutByte
                pop     dx
            endif
        endif
    ELSE ; SERIAL_OUT_OPS
        IFDEF NVIDIA_SIMULATE
                ifidni <port>, <DX>
                    ifidni <value>, <AX>
                        call    NVSimulateOutWord
                    else
                        call    NVSimulateOutByte
                    endif
                else
                    ifidni <value>, <AX>
                        push    dx
                        mov     dx, port
                        call    NVSimulateOutWord
                        pop     dx
                    else
                        push    dx
                        mov     dx, port
                        call    NVSimulateOutByte
                        pop     dx
                    endif
                endif
        ELSE ; NVIDIA_SIMULATE
                IFNDEF WEITEK_PROTO
                    out port, value
                ENDIF ; WEITEK_PROTO
        ENDIF ; NVIDIA_SIMULATE
    ENDIF ; SERIAL_OUT_OPS
endm
;
;       SIM_WO - Simulate an out to a Weitek I/O port
;
;       Parameters: port - dx or constant I/O port below 00100h
;                   value - ax or al
;       Entry:  dx - I/O port (optional)
;               ax/al - Value
;       Exit:   None
;       Return: None
;       Uses:   None
;
SIM_WO macro port:req, value:req
    IFDEF WEITEK_PROTO
        out port, value
    ELSE
        .err <Use of SIM_WO without WEITEK_PROTO defined.>
    ENDIF ; WEITEK_PROTO
endm
;
;       SIM_NI - Simulate an in from an NV I/O port
;
;       Parameters: dest - ax or al
;                   port - dX or constant I/O port below 00100h
;       Entry:  dx - I/O port (optional)
;       Exit:   al or ax - value
;       Return: None
;       Uses:   None
;
SIM_NI macro dest:req, port:req
    IFDEF NVIDIA_SIMULATE
        ifidni <port>, <DX>
            ifidni <dest>, <AX>
                call    NVSimulateInWord
            else
                call    NVSimulateInByte
            endif
        else
            .err <Can not do SIM_NI from system port>
        endif
    ELSE ; NVIDIA_SIMULATE
        in dest, port
    ENDIF ; NVIDIA_SIMULATE
endm
;
;       SIM_WI - Simulate an in from a Weitek I/O port
;
;       Parameters: dest - ax or al
;                   port - dX or constant I/O port below 00100h
;       Entry:  dx - I/O port (optional)
;       Exit:   al or ax - value
;       Return: None
;       Uses:   None
;
SIM_WI macro dest:req, port:req
    IFDEF WEITEK_PROTO
        in dest, port
    ELSE
        .err <Use of SIM_WI without WEITEK_PROTO defined.>
    ENDIF ; WEITEK_PROTO
endm
;
;       SIM_GET_INDEX_REGISTER_WTK - Simulate an in from a Weitek indexed I/O port
;
;       Parameters: None
;       Entry:  dx - I/O port
;               al - Index
;       Exit:   ah - Value
;       Return: None
;       Uses:   None
;
SIM_GET_INDEX_REGISTER_WTK macro
        IFDEF NVIDIA_SIMULATE
                call    WTKGetIndexRegister
        ELSE ; NVIDIA_SIMULATE
                call    GetIndexRegister
        ENDIF ; NVIDIA_SIMULATE
endm
;
;       SIM_GET_INDEX_REGISTER_NV - Simulate an in from an NV indexed I/O port
;
;       Parameters: None
;       Entry:  dx - I/O port
;               al - Index
;       Exit:   ah - Value
;       Return: None
;       Uses:   None
;
SIM_GET_INDEX_REGISTER_NV macro
        IFDEF NVIDIA_SIMULATE
                call    NVGetIndexRegister
        ELSE ; NVIDIA_SIMULATE
                call    GetIndexRegister
        ENDIF ; NVIDIA_SIMULATE
endm
