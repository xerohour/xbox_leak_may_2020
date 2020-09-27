        title  "ACPI Timer Functions"
;++
;
; Copyright (c) 1989-2001  Microsoft Corporation
;
; Module Name:
;
;    pmtimer.asm
;
; Abstract:
;
;    This module implements the code for ACPI-related timer functions.
;
; Environment:
;
;    Kernel mode only.
;
;--

.386p
        .xlist
include hal386.inc
include callconv.inc
include i386\ix8259.inc
INCLUDE i386\mcpxacpi.inc
        .list

_DATA   SEGMENT  DWORD PUBLIC 'DATA'

MSBMASK32               EQU     80000000h

        PUBLIC  HalpPerformanceCounterLow, HalpPerformanceCounterHigh1, HalpPerformanceCounterHigh2
HalpPerformanceCounterLow       dd      0
HalpPerformanceCounterHigh1     dd      0
HalpPerformanceCounterHigh2     dd      0

_DATA   ends

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page ,132
        subttl  "Query Performance Counter"
;++
;
; LARGE_INTEGER
; KeQueryPerformanceCounter (
;    VOID
;    )
;
; Routine Description:
;
;    This routine returns the current 64-bit performance counter.
;
;    Also note that the performace counter returned by this routine
;    is not necessary the value when this routine is just entered.
;    The value returned is actually the counter value at any point
;    between the routine is entered and is exited.
;
; Arguments:
;
;    None.
;
; Return Value:
;
;    Current value of the performance counter will be returned.
;
;--

cPublicProc _KeQueryPerformanceCounter, 0
cPublicFpo 0, 1

        push    ebx

        ;
        ; Snap current times
        ;

kqpc10: mov     ecx, HalpPerformanceCounterHigh2
        mov     ebx, HalpPerformanceCounterLow

        cmp     ecx, HalpPerformanceCounterHigh1
        jne     short kqpc10        ; Loop until consistent copy read

        mov     edx, XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_PM_TIMER_REGISTER
        in      eax, dx

        ;
        ; See if h/w MSb matches s/w copy
        ;

        mov     edx, eax
        xor     edx, ebx
        and     edx, MSBMASK32      ; Isolate MSb match or mismatch

        ;
        ; Strip high hardware bit
        ;

        and     eax, NOT MSBMASK32

        ;
        ; merge low bits
        ;

        and     ebx, MSBMASK32
        or      eax, ebx

        ;
        ; If there was a mismatch, add a tick
        ;

        add     eax, edx
        adc     ecx, 0

        mov     edx, ecx                ; get the top-half of the return value

kqpc40: pop     ebx

        stdRET  _KeQueryPerformanceCounter

stdENDP _KeQueryPerformanceCounter

        page ,132
        subttl  "Query Performance Counter"
;++
;
; LARGE_INTEGER
; KeQueryPerformanceFrequency (
;    VOID
;    )
;
; Routine Description:
;
;    This routine returns the current 64-bit performance frequency.
;
; Arguments:
;
;    None.
;
; Return Value:
;
;    Current frequency of the performance counter will be returned.
;
;--

cPublicProc _KeQueryPerformanceFrequency, 0
cPublicFpo 0, 0

;
; Return the frequency of the legacy core frequency (13.5Mhz) divided by four to
; obtain the ACPI timer frequency (3.375Mhz).
;
        mov     eax, 3375000
        xor     edx, edx
        stdRET  _KeQueryPerformanceFrequency

stdENDP _KeQueryPerformanceFrequency

;++
;
; VOID
; HalpAcpiTimerCarry (
;    VOID
;    )
;
; Routine Description:
;
;    This routine is called to service the PM timer carry interrupt
;
;    N.B. This function is called at interrupt time and assumes the
;    caller clears the interrupt
;
; Arguments:
;
;    None
;
; Return Value:
;
;    None
;
;--
cPublicProc _HalpAcpiTimerCarry, 0
cPublicFpo 0, 0

        ;
        ; Get current time from h/w
        ;

        mov     edx, XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + MCPX_ACPI_PM_TIMER_REGISTER
        in      eax, dx
        mov     ecx, eax

        mov     eax, HalpPerformanceCounterLow
        mov     edx, HalpPerformanceCounterHigh2

        ;
        ; Add one tick
        ;

        add     eax, MSBMASK32
        adc     edx, 0

        ;
        ; MSb of h/w should now match s/w.  If not, add another tick
        ; to get them back in sync.  (debugger might knock them
        ; out of sync)
        ;

        xor     ecx, eax
        and     ecx, MSBMASK32
        add     eax, ecx
        adc     edx, 0

        ;
        ; Store in reverse order of code which reads it
        ;

        mov     HalpPerformanceCounterHigh1, edx
        mov     HalpPerformanceCounterLow, eax
        mov     HalpPerformanceCounterHigh2, edx

        stdRET  _HalpAcpiTimerCarry
stdENDP _HalpAcpiTimerCarry

_TEXT   ends
        end
