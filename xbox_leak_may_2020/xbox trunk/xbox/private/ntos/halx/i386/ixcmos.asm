        title  "Cmos Access Routines"
;++
;
; Module Name:
;
;    ixcmos.asm
;
; Abstract:
;
;    Procedures necessary to access CMOS/ECMOS information.
;
; Author:
;
;    David Risner (o-ncrdr) 20 Apr 1992
;
; Revision History:
;
;    Landy Wang (corollary!landy) 04 Dec 1992
;    - Move much code from ixclock.asm to here so different HALs
;      can reuse the common functionality.
;
;--

.386p
        .xlist
include hal386.inc
include callconv.inc                    ; calling convention macros
include i386\ix8259.inc
include i386\ixcmos.inc
        .list

        EXTRNP  _DbgBreakPoint,0

_DATA   SEGMENT  DWORD PUBLIC 'DATA'

;
; Holds the value of the eflags register before a cmos spinlock is
; acquired (used in HalpAcquire/ReleaseCmosSpinLock().
;
_HalpHardwareLockFlags   dd      0

_DATA   ends

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page ,132
        subttl  "Read System Time"
;++
;
; BOOLEAN
; HalQueryRealTimeClock (
;    PTIME_FIELDS TimeFields
;    )
;
; Routine Description:
;
;    This routine reads current time from CMOS memory and stores it
;    in the TIME_FIELDS structure passed in by caller.
;
; Arguments:
;
;    TimeFields - A pointer to the TIME_FIELDS structure.
;
; Return Value:
;
;    If the power to the realtime clock has not failed, then the time
;    values are read from the realtime clock and a value of TRUE is
;    returned. Otherwise, a value of FALSE is returned.
;
;--

;
; Parameters:
;

KrctPTimeFields equ [esp+4]

cPublicProc _HalQueryRealTimeClock ,1

if DBG
krctwait0:
        mov     ecx, 100
krctwait:
        push    ecx
else
krctwait:
endif
        stdCall   _HalpAcquireCmosSpinLock
        mov     ecx, 100
        align   4
krct00: mov     al, 0Ah                 ; Specify register A
        CMOS_READ                       ; (al) = CMOS register A
        test    al, CMOS_STATUS_BUSY    ; Is time update in progress?
        jz      short krct10            ; if z, no, go read CMOS time
        loop    short krct00            ; otherwise, try again.

;
; CMOS is still busy. Try again ...
;

        stdCall _HalpReleaseCmosSpinLock
if DBG
        pop     ecx
        loop    short krctwait
        stdCall _DbgBreakPoint
        jmp     short krctwait0
else
        jmp     short krctwait
endif
        align   4
if DBG
krct10:
        pop     ecx
else
krct10:
endif
        mov     edx, KrctPTimeFields    ; (edx)-> TIME_FIELDS structure
        xor     eax, eax                ; (eax) = 0

        ;
        ; The RTC is only accurate within one second.  So
        ; add a half a second so that we are closer, on average,
        ; to the right answer.
        ;
        mov     word ptr [edx].TfMilliseconds, 500      ; add a half a second
        
        mov     al, RTC_OFFSET_SECOND
        CMOS_READ                       ; (al) = second in BCD form
        BCD_TO_BIN                      ; (ax) = second
        mov     [edx].TfSecond, ax      ; set second in TIME_FIELDS
        
        mov     al, RTC_OFFSET_MINUTE
        CMOS_READ                       ; (al) = minute in BCD form
        BCD_TO_BIN                      ; (ax) = Minute
        mov     [edx].TfMinute, ax      ; set minute in TIME_FIELDS

        mov     al, RTC_OFFSET_HOUR
        CMOS_READ                       ; (al) = hour in BCD form
        BCD_TO_BIN                      ; (ax) = Hour
        mov     [edx].TfHour, ax        ; set hour in TIME_FIELDS

        mov     al, RTC_OFFSET_DAY_OF_WEEK
        CMOS_READ                       ; (al) = day-of-week in BCD form
        BCD_TO_BIN                      ; (ax) = day-of-week
        mov     [edx].TfWeekday, ax     ; set Weekday in TIME_FIELDS

        mov     al, RTC_OFFSET_DATE_OF_MONTH
        CMOS_READ                       ; (al) = date-of-month in BCD form
        BCD_TO_BIN                      ; (ax) = date_of_month
        mov     [edx].TfDay, ax         ; set day in TIME_FIELDS

        mov     al, RTC_OFFSET_MONTH
        CMOS_READ                       ; (al) = month in BCD form
        BCD_TO_BIN                      ; (ax) = month
        mov     [edx].TfMonth, ax       ; set month in TIME_FIELDS

        mov     al, RTC_OFFSET_YEAR
        CMOS_READ                       ; (al) = year in BCD form
        BCD_TO_BIN                      ; (ax) = year
        push    eax                     ; save year in stack

        mov     al, RTC_OFFSET_CENTURY
        CMOS_READ                       ; (al) = century byte in BCD form
        BCD_TO_BIN                      ; (ax) = century

        mov     ah, 100
        mul     ah                      ; (ax) = century * 100
        pop     ecx                     ; (cx) = year
        add     ax, cx                  ; (ax)= year

        cmp     ax, 1900                ; Is year > 1900
        jb      short krct40
        cmp     ax, 1920                ; and < 1920
        jae     short krct40
        add     ax, 100                 ; Compensate for century field

krct40:
        mov     [edx].TfYear, ax        ; set year in TIME_FIELDS

        stdCall   _HalpReleaseCmosSpinLock

        mov     al, 1                   ; return TRUE

        stdRET    _HalQueryRealTimeClock

stdENDP _HalQueryRealTimeClock

        page ,132
        subttl  "Write System Time"
;++
;
; BOOLEAN
; HalSetRealTimeClock (
;    PTIME_FIELDS TimeFields
;    )
;
; Routine Description:
;
;    This routine writes current time from TIME_FILEDS structure
;    to CMOS memory.
;
; Arguments:
;
;    TimeFields - A pointer to the TIME_FIELDS structure.
;
; Return Value:
;
;    If the power to the realtime clock has not failed, then the time
;    values are written to the realtime clock and a value of TRUE is
;    returned. Otherwise, a value of FALSE is returned.
;
;--

;
; Parameters:
;

KrctPTimeFields equ [esp+4]

cPublicProc _HalSetRealTimeClock ,1

if DBG
kwctwait0:
        mov     ecx, 100
kwctwait:
        push    ecx
else
kwctwait:
endif
        stdCall   _HalpAcquireCmosSpinLock
        mov     ecx, 100
        align   4
kwct00: mov     al, 0Ah                 ; Specify register A
        CMOS_READ                       ; (al) = CMOS register A
        test    al, CMOS_STATUS_BUSY    ; Is time update in progress?
        jz      short kwct10            ; if z, no, go write CMOS time
        loop    short kwct00            ; otherwise, try again.

;
; CMOS is still busy. Try again ...
;

        stdCall _HalpReleaseCmosSpinLock
if DBG
        pop     ecx
        loop    short kwctwait
        stdCall _DbgBreakPoint
        jmp     short kwctwait0
else
        jmp     short kwctwait
endif
        align   4
if DBG
kwct10:
        pop     ecx
else
kwct10:
endif
        mov     edx, KrctPTimeFields    ; (edx)-> TIME_FIELDS structure

        mov     al, [edx].TfSecond      ; Read second in TIME_FIELDS
        BIN_TO_BCD
        mov     ah, al
        mov     al, RTC_OFFSET_SECOND
        CMOS_WRITE

        mov     al, [edx].TfMinute      ; Read minute in TIME_FIELDS
        BIN_TO_BCD
        mov     ah, al
        mov     al, RTC_OFFSET_MINUTE
        CMOS_WRITE

        mov     al, [edx].TfHour        ; Read Hour in TIME_FIELDS
        BIN_TO_BCD
        mov     ah, al
        mov     al, RTC_OFFSET_HOUR
        CMOS_WRITE

        mov     al, [edx].TfWeekDay     ; Read WeekDay in TIME_FIELDS
        BIN_TO_BCD
        mov     ah, al
        mov     al, RTC_OFFSET_DAY_OF_WEEK
        CMOS_WRITE

        mov     al, [edx].TfDay         ; Read day in TIME_FIELDS
        BIN_TO_BCD
        mov     ah, al
        mov     al, RTC_OFFSET_DATE_OF_MONTH
        CMOS_WRITE

        mov     al, [edx].TfMonth       ; Read month in TIME_FIELDS
        BIN_TO_BCD
        mov     ah, al
        mov     al, RTC_OFFSET_MONTH
        CMOS_WRITE

        mov     ax, [edx].TfYear        ; Read Year in TIME_FIELDS
        cmp     ax, 9999
        jbe     short kwct15
        mov     ax, 9999

        align   4
kwct15:
        mov     cl, 100
        div     cl                      ; [ax]/[cl]->al=quo, ah=rem
        push    eax

        BIN_TO_BCD
        mov     ah, al                  ; [ah] = Century in BCD form
        mov     al, RTC_OFFSET_CENTURY
        CMOS_WRITE

        pop     eax
        mov     al, ah                  ; [al] = Year
        BIN_TO_BCD
        mov     ah, al                  ; [ah] = year in BCD
        mov     al, RTC_OFFSET_YEAR
        CMOS_WRITE

        stdCall _HalpReleaseCmosSpinLock

        mov     al, 1                   ; return TRUE

        stdRET  _HalSetRealTimeClock

stdENDP _HalSetRealTimeClock

        page ,132
        subttl  "CMOS Subrange Validity Check"
;++
;
; BOOLEAN
; HalpIsCmosSubrangeValid(
;    IN ULONG NumberOfBytes,
;    IN ULONG RegisterNumber
;    )
;
; Routine Description:
;
;    This routine checks if the CMOS subrange data is valid.
;
; Arguments:
;
;    NumberOfBytes - Number of bytes in the subrange.
;
;    RegisterNumber - First register of the subrange to check.
;
; Return Value:
;
;    Returns ZF=1 if data stored in the CMOS subrange is valid, else ZF=0.
;
;--

cPublicFastCall HalpIsCmosSubrangeValid, 2

        mov     ah, 0AAh                ; opposite of first expected bit pattern
icsv10: mov     al, dl                  ; CMOS register to index
        ECMOS_READ
        not     ah                      ; flip bit pattern
        inc     edx
        cmp     al, ah                  ; check if the byte matches
        loope   icsv10                  ; loop while bytes remaining and equal

        fstRET  HalpIsCmosSubrangeValid

fstENDP HalpIsCmosSubrangeValid

        page ,132
        subttl  "CMOS Validity Check"
;++
;
; BOOLEAN
; HalIsCmosValid(
;    VOID
;    )
;
; Routine Description:
;
;    This routine checks if the data stored in the CMOS is valid.
;
; Arguments:
;
;    None.
;
; Return Value:
;
;    Returns TRUE if the data stored in the CMOS is valid, else FALSE.
;
;--

cPublicProc _HalIsCmosValid, 0

        stdCall _HalpAcquireCmosSpinLock

;
; Break the check of the CMOS data into two subranges, because some of the
; registers are used for the real time clock or control registers.
;

        mov     ecx, 60h
        mov     edx, 10h
        fstCall HalpIsCmosSubrangeValid
        jne     icv10

        mov     ecx, 80h
        mov     edx, ecx
        fstCall HalpIsCmosSubrangeValid

icv10:  sete    al
        stdCall _HalpReleaseCmosSpinLock

        stdRET  _HalIsCmosValid

stdENDP _HalIsCmosValid

        page ,132
        subttl  "CMOS Subrange Validity Check"
;++
;
; VOID
; HalpMarkCmosSubrangeValid(
;    IN ULONG NumberOfBytes,
;    IN ULONG RegisterNumber
;    )
;
; Routine Description:
;
;    This routine checks if the CMOS subrange data is valid.
;
; Arguments:
;
;    NumberOfBytes - Number of bytes in the subrange.
;
;    RegisterNumber - First register of the subrange to check.
;
; Return Value:
;
;    Returns ZF=1 if data stored in the CMOS subrange is valid, else ZF=0.
;
;--

cPublicFastCall HalpMarkCmosSubrangeValid, 0

        mov     ah, 055h                ; first expected bit pattern
mcrv10: mov     al, dl                  ; CMOS register to index
        ECMOS_WRITE
        not     ah                      ; flip bit pattern
        inc     edx
        loop    mcrv10

        fstRET  HalpMarkCmosSubrangeValid

fstENDP HalpMarkCmosSubrangeValid

        page ,132
        subttl  "Marks CMOS data as valid"
;++
;
; VOID
; HalMarkCmosValid(
;    VOID
;    )
;
; Routine Description:
;
;    This routine sets the real time clock data to the valid state.
;
; Arguments:
;
;    None.
;
; Return Value:
;
;    None.
;
;--

cPublicProc _HalMarkCmosValid, 0

        stdCall _HalpAcquireCmosSpinLock

;
; Break the marking of the CMOS data into two subranges, because some of the
; registers are used for the real time clock or control registers.
;

        mov     ecx, 60h
        mov     edx, 10h
        fstCall HalpMarkCmosSubrangeValid

        mov     ecx, 80h
        mov     edx, ecx
        fstCall HalpMarkCmosSubrangeValid

        stdCall _HalpReleaseCmosSpinLock

        stdRET  _HalMarkCmosValid

stdENDP _HalMarkCmosValid

;++
;
; Routine Description:
;
;   Acquires a spinlock to access the cmos chip. The cmos chip is
;   accessed at different irql levels, so to be safe, we 'cli'.
;   We could replace that to raise irql to PROFILE_LEVEL, but that's
;   a lot of code.
;
; Arguments:
;
;    None
;
; Return Value:
;
;    Interrupt is disabled.
;    Irql level not affected.
;    Flags saved in _HalpHardwareLockFlags.
;--

cPublicProc _HalpAcquireCmosSpinLock  ,0

        pushfd
        cli
        pop     _HalpHardwareLockFlags          ; save flags for release S.L.
        stdRET    _HalpAcquireCmosSpinLock

stdENDP _HalpAcquireCmosSpinLock

;++
;
; Routine Description:
;
;   Release spinlock, and restore flags to the state it was before
;   acquiring the spinlock.
;
; Arguments:
;
;   None
;
; Return Value:
;
;   Interrupts restored to their state before acquiring spinlock.
;   Irql level not affected.
;
;--

cPublicProc _HalpReleaseCmosSpinLock  ,0

        ;
        ; restore eflags as it was before acquiring spinlock. Put it on
        ; stack before releasing spinlock (so other cpus cannot overwrite
        ; it with their own eflags).
        ;
        push    _HalpHardwareLockFlags          ; old eflags on stack.
        popfd                                   ; restore eflags.
        stdRET    _HalpReleaseCmosSpinLock

stdENDP _HalpReleaseCmosSpinLock

_TEXT   ends

        end
