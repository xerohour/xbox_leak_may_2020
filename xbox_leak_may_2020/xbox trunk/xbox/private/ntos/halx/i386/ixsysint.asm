;++
;
;Copyright (c) 1991  Microsoft Corporation
;
;Module Name:
;
;    ixsysint.asm
;
;Abstract:
;
;    This module implements the HAL routines to enable/disable system
;    interrupts.
;
;Author:
;
;    John Vert (jvert) 22-Jul-1991
;
;Environment:
;
;    Kernel Mode
;
;Revision History:
;
;--


.386p
        .xlist
include hal386.inc
include callconv.inc                    ; calling convention macros
include i386\ix8259.inc
include i386\kimacro.inc
        .list

        extrn   KiI8259MaskTable:DWORD
        extrn   _KiPCR:DWORD
        EXTRNP  Kei386EoiHelper

_DATA   SEGMENT DWORD PUBLIC 'DATA'

align   dword

;
; Interrupt Request Register.  Indicates which software and hardware interrupts
; are pending.  When lowering IRQL either from exiting an ISR or calling
; KfLowerIrql, this value is checked for delayed software or hardware
; interrupts.  Any interrupt above the new IRQL level is then dispatched.
;

        public  HalpIRR
HalpIRR         dd  0

;
; Interrupt Request Active Register.  Indicates which hardware interrupts are
; currently being serviced.  Used to prevent recursion in HalEndSystemInterrupt.
;

        public  HalpIRRActive
HalpIRRActive   dd  0

;
; Interrupt Disable Register.  Indicates which interrupts are physically masked
; at the PIC.  IRQ02, the link the secondary interrupt controller, is never
; disabled.
;

        public  HalpIDR
HalpIDR         dd  0FFFFFFFBh

;
; Edge/level Control Register.  Indicates which IRQs are edge or level
; sensitive.
;
        public  HalpEisaELCR
HalpEisaELCR    dd  0

;
; HalDismissSystemInterrupt does an indirect jump through this table so it
; can quickly execute specific code for different interrupts.
;
        public  HalpSpecialDismissTable
HalpSpecialDismissTable label   dword
        dd      offset FLAT:HalpDismissNormal   ; irq 0
        dd      offset FLAT:HalpDismissNormal   ; irq 1
        dd      offset FLAT:HalpDismissNormal   ; irq 2
        dd      offset FLAT:HalpDismissNormal   ; irq 3
        dd      offset FLAT:HalpDismissNormal   ; irq 4
        dd      offset FLAT:HalpDismissNormal   ; irq 5
        dd      offset FLAT:HalpDismissNormal   ; irq 6
        dd      offset FLAT:HalpDismissIrq07    ; irq 7
        dd      offset FLAT:HalpDismissNormal   ; irq 8
        dd      offset FLAT:HalpDismissNormal   ; irq 9
        dd      offset FLAT:HalpDismissNormal   ; irq A
        dd      offset FLAT:HalpDismissNormal   ; irq B
        dd      offset FLAT:HalpDismissNormal   ; irq C
        dd      offset FLAT:HalpDismissNormal   ; irq D
        dd      offset FLAT:HalpDismissNormal   ; irq E
        dd      offset FLAT:HalpDismissIrq0f    ; irq F

_DATA   ENDS

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        public  HalpSpecialDismissLevelTable
HalpSpecialDismissLevelTable label   dword
        dd      offset FLAT:HalpDismissLevel        ; irq 0
        dd      offset FLAT:HalpDismissLevel        ; irq 1
        dd      offset FLAT:HalpDismissLevel        ; irq 2
        dd      offset FLAT:HalpDismissLevel        ; irq 3
        dd      offset FLAT:HalpDismissLevel        ; irq 4
        dd      offset FLAT:HalpDismissLevel        ; irq 5
        dd      offset FLAT:HalpDismissLevel        ; irq 6
        dd      offset FLAT:HalpDismissIrq07Level   ; irq 7
        dd      offset FLAT:HalpDismissLevel        ; irq 8
        dd      offset FLAT:HalpDismissLevel        ; irq 9
        dd      offset FLAT:HalpDismissLevel        ; irq A
        dd      offset FLAT:HalpDismissLevel        ; irq B
        dd      offset FLAT:HalpDismissLevel        ; irq C
        dd      offset FLAT:HalpDismissLevel        ; irq D
        dd      offset FLAT:HalpDismissLevel        ; irq E
        dd      offset FLAT:HalpDismissIrq0fLevel   ; irq F

;++
;VOID
;FASTCALL
;HalBeginSystemInterrupt(
;    IN CCHAR Vector,
;    IN KIRQL Irql,
;    OUT PKIRQL OldIrql
;    )
;
;Routine Description:
;
;    This routine is used to dismiss the specified vector number.  It is called
;    before any interrupt service routine code is executed.
;
;    N.B.  This assumes that [esp+4] is OldIrql.
;
;    N.B.  This routine automatically IRETs if the interrupt is spurious.
;
;    On a UP machine the interrupt dismissed at BeginSystemInterrupt time.
;    This is fine since the irql is being raise to mask it off.
;    HalEndSystemInterrupt is simply a LowerIrql request.
;
;Arguments:
;
;    Vector - Supplies the vector of the interrupt to be dismissed
;
;    Irql   - Supplies the IRQL to raise to
;
;    OldIrql- Location to return OldIrql
;
;Return Value:
;
;    None.
;
;--
align dword

cPublicFastCall HalBeginSystemInterrupt ,2

;
; N.B. The interrupt dispatch code jumps through HalpSpecialDismissTable to
; avoid skipping through to the next instruction.
;

        jmp     HalpSpecialDismissTable[ecx*4]

HalpDismissIrq0f:
;
; Check to see if this is a spurious interrupt
;
        mov     al, OCW3_READ_ISR       ; tell 8259 we want to read ISR
        out     PIC2_PORT0, al
        IODelay                         ; delay
        in      al, PIC2_PORT0          ; (al) = content of PIC 1 ISR
        test    al, 10000000B           ; Is In-Service register set?
        jnz     short HalpDismissNormal ; No, this is NOT a spurious int,
                                        ; go do the normal interrupt stuff
HalpIrq0fSpurious:
;
; This is a spurious interrupt.
; Because the slave PIC is cascaded to irq2 of master PIC, we need to
; dismiss the interupt on master PIC's irq2.
;

        mov     al, PIC2_EOI            ; Specific eoi to master for pic2 eoi
        out     PIC1_PORT0, al          ; send irq2 specific eoi to master
        add     esp, 8                  ; clear return address and OldIrql
        SPURIOUS_INTERRUPT_EXIT

HalpDismissIrq07:
;
; Check to see if this is a spurious interrupt
;
        mov     al, OCW3_READ_ISR       ; tell 8259 we want to read ISR
        out     PIC1_PORT0, al
        IODelay                         ; delay
        in      al, PIC1_PORT0          ; (al) = content of PIC 1 ISR
        test    al, 10000000B           ; Is In-Service register set?
        jnz     HalpDismissNormal       ; No, so this is NOT a spurious int

        add     esp, 8
        SPURIOUS_INTERRUPT_EXIT

align 4

;
; The clock and profile interrupts bypass HalpSpecialDismissTable and enter at
; this point with the same entry conditions as HalBeginSystemInterrupt.
;

        public  @HalBeginClockInterrupt@8
@HalBeginClockInterrupt@8 label byte
        public  @HalBeginProfileInterrupt@8
@HalBeginProfileInterrupt@8 label byte

HalpDismissNormal:
;
; Raise IRQL to requested level
;
        xor     ebx, ebx
        mov     bl, PCRB[PcIrql]        ; (ebx) = Current Irql

;
; Now we check to make sure the Irql of this interrupt > current Irql.
; If it is not, we dismiss it as spurious and set the appropriate bit
; in the IRR so we can dispatch the interrupt when Irql is lowered
;
        cmp     dl, bl
        jbe     Hdsi300

        mov     PCRB[PcIrql], dl        ; set new Irql
        mov     byte ptr [esp+4], bl    ; save current irql to OldIrql variable

;
; Dismiss interrupt.
;
        mov     eax, ecx                ; (eax) = IRQ #
        cmp     eax, 8                  ; EOI to master or slave?
        jae     short Hbsi100           ; EIO to both master and slave

        or      al, PIC1_EOI_MASK       ; create specific eoi mask for master
        out     PIC1_PORT0, al          ; dismiss the interrupt
        sti
        fstRET  HalBeginSystemInterrupt

align 4
Hbsi100:
        add     al, OCW2_SPECIFIC_EOI - 8   ; specific eoi to slave
        out     PIC2_PORT0, al

        mov     al, PIC2_EOI            ; specific eoi to master for pic2 eoi
        out     PIC1_PORT0, al          ; send irq2 specific eoi to master
        sti
        fstRET  HalBeginSystemInterrupt

align 4
Hdsi300:
;
; An interrupt has come in at a lower Irql, so we dismiss it as spurious and
; set the appropriate bit in the IRR so that KeLowerIrql knows to dispatch
; it when Irql is lowered.
;
; (ecx) = 8259 IRQ#
; (al)  = New Irql
; (ebx) = Current Irql
;

        mov     eax, 1
        add     ecx, 4                  ; (ecx) = Irq # + 4
        shl     eax, cl
        or      HalpIRR, eax

;
; Raise Irql to prevent it from happening again
;

;
; Get the PIC masks for Irql
;

        mov     eax, KiI8259MaskTable[ebx*4]
        or      eax, HalpIDR

;
; Write the new interrupt mask register back to the 8259
;
        SET_8259_MASK

Hbsi390:
        add     esp, 8
        SPURIOUS_INTERRUPT_EXIT

HalpDismissIrq0fLevel:
;
; Check to see if this is a spurious interrupt
;
        mov     al, OCW3_READ_ISR       ; tell 8259 we want to read ISR
        out     PIC2_PORT0, al
        IODelay                         ; delay
        in      al, PIC2_PORT0          ; (al) = content of PIC 1 ISR
        test    al, 10000000B           ; Is In-Service register set?
        jnz     short HalpDismissLevel  ; No, this is NOT a spurious int,
                                        ; go do the normal interrupt stuff
        jmp     HalpIrq0fSpurious

HalpDismissIrq07Level:
;
; Check to see if this is a spurious interrupt
;
        mov     al, OCW3_READ_ISR       ; tell 8259 we want to read ISR
        out     PIC1_PORT0, al
        IODelay                         ; delay
        in      al, PIC1_PORT0          ; (al) = content of PIC 1 ISR
        test    al, 10000000B           ; Is In-Service register set?
        jnz     short HalpDismissLevel  ; No, so this is NOT a spurious int
        add     esp, 8                  ; clear return address and OldIrql
        SPURIOUS_INTERRUPT_EXIT

align 4
;
; The system control interrupt (SCI) and SMBus interrupt bypasses
; HalpSpecialDismissTable and enter at this point with the same entry conditions
; as HalBeginSystemInterrupt.
;

        public  @HalBeginSystemControlInterrupt@8
@HalBeginSystemControlInterrupt@8 label byte
        public  @HalBeginSMBusInterrupt@8
@HalBeginSMBusInterrupt@8 label byte

HalpDismissLevel:
;
; Raise IRQL to requested level
;
        xor     ebx, ebx
        mov     bl, PCRB[PcIrql]        ; (ebx) = Current Irql

        cmp     ecx, 8                  ; EOI for irq2?
        jb      Hbsi400

        mov     al, PIC2_EOI            ; specific eoi to master for pic2 eoi
        out     PIC1_PORT0, al          ; send irq2 specific eoi to master

;
; Now we check to make sure the Irql of this interrupt > current Irql.
; If it is not, we dismiss it as spurious and set the appropriate bit
; in the IRR so we can dispatch the interrupt when Irql is lowered
;
Hbsi400:
        cmp     dl, bl
        jbe     Hdsi300

        mov     PCRB[PcIrql], dl        ; set new Irql
        mov     byte ptr [esp+4], bl    ; save current irql to OldIrql variable

        sti
        fstRET  HalBeginSystemInterrupt

fstENDP HalBeginSystemInterrupt

;++
;VOID
;HalDisableSystemInterrupt(
;    IN ULONG BusInterruptLevel
;    )
;
;Routine Description:
;
;    Disables a system interrupt.
;
;Arguments:
;
;    BusInterruptLevel - Supplies the IRQ of the interrupt to be disabled
;
;Return Value:
;
;    None.
;
;--
cPublicProc _HalDisableSystemInterrupt      ,1
.FPO ( 0, 1, 0, 0, 0, 0 )

        movzx   ecx, byte ptr [esp+4]       ; (ecx) = 8259 irq #
        mov     edx, 1
        shl     edx, cl                     ; (ebx) = bit in IMR to disable
        cli
        or      HalpIDR, edx
        xor     eax, eax

;
; Get the current interrupt mask register from the 8259
;
        in      al, PIC2_PORT1
        shl     eax, 8
        in      al, PIC1_PORT1
;
; Mask off the interrupt to be disabled
;
        or      eax, edx
;
; Write the new interrupt mask register back to the 8259
;
        out     PIC1_PORT1, al
        shr     eax, 8
        out     PIC2_PORT1, al
        PIC2DELAY

        sti
        stdRET    _HalDisableSystemInterrupt

stdENDP _HalDisableSystemInterrupt

;++
;
;VOID
;HalEnableSystemInterrupt(
;    IN ULONG BusInterruptLevel,
;    IN KINTERRUPT_MODE InterruptMode
;    )
;
;Routine Description:
;
;    Enables a system interrupt
;
;Arguments:
;
;    BusInterruptLevel - Supplies the IRQ of the interrupt to be enabled
;
;Return Value:
;
;    None.
;
;--
cPublicProc _HalEnableSystemInterrupt       ,2
.FPO ( 0, 2, 0, 0, 0, 0 )

        movzx   ecx, byte ptr [esp+4]       ; (ecx) = 8259 irq #

;
; Clear or set the edge\level mask bit depending on what the caller wants.
;
        btr     HalpEisaELCR, ecx
        mov     al, [esp+8]
        cmp     al, 0
        jnz     short hes_edge

        ; Caller wants level triggered interrupts
        bts     HalpEisaELCR, ecx

        mov     edx, HalpSpecialDismissLevelTable[ecx*4]
        mov     HalpSpecialDismissTable[ecx*4], edx

hes_edge:
;
; Program the HW to make it match the callers request.
;
        mov     eax, HalpEisaELCR
        mov     edx, EISA_EDGE_LEVEL0
        out     dx, al
        IODelay
        mov     al, ah
        inc     edx
        out     dx, al

hes_ProgPIC:

        mov     eax, 1
        shl     eax, cl                         ; (ebx) = bit in IMR to enable
        not     eax

        cli
        and     HalpIDR, eax

;
; Get the PIC masks for Irql 0
;
        mov     eax, KiI8259MaskTable[0]
        or      eax, HalpIDR
;
; Write the new interrupt mask register back to the 8259
;
        SET_8259_MASK

        sti
        stdRET    _HalEnableSystemInterrupt

stdENDP _HalEnableSystemInterrupt

_TEXT   ENDS

        END
