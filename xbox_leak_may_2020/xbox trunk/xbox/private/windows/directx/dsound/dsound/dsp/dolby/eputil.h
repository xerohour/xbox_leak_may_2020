;*******************************************************************
;
;    EpUtil.h		Global header file for Encode Audio Processor   
;
;
;       Modified by Ethan Brodsky
;		Based on EpUtil.h Written by Gints Klimanis, September 2000
;
;
;******************************************************************        

kFrameLength	equ		32
kPCValue		equ		0
kCounter		equ		0

; ==============================================================
; EP Address Map 
; ==============================================================
; Relative to EPBase (24 bit words (3 bytes) in 32-bit containers (4 bytes))
;     0x0000 - 0x2FFC   EP DSP X-Memory 3K Words (24 lsbits of Dword)     
;     0x6000 - 0x63FC   EP DSP Y-Memory 256 Words (24 lsbits of Dword) 
;     0xA000 - 0xDFFC   EP DSP P-Memory 4K Words (24 lsbits of Dword) 
;     0xFE00 - 0xFFF8   EP DSP Peripheral Registers
;     0xFFFC            EP DSP Reset
;
; Peripheral registers (Control,Status) mapped to upper 128 words
; of X-memory (0xFFFF80 to 0xFFFFFF) so they can be accessed with movep 
;
; EP DSP memory addresses 24-bit words
; System memory address    8-bit words 

kEPMemory_Base		equ $000000
kEPMemory_Base_XRAM	equ (kEPMemory_Base+$000000)
kEPMemory_Base_YRAM	equ (kEPMemory_Base+$001800)
kEPMemory_Base_PRAM	equ (kEPMemory_Base+$002800)

kEPMemory_Size_XRAM	equ 3072
kEPMemory_Size_YRAM	equ 256
kEPMemory_Size_PRAM	equ 4096

kEPMemory_Base_BootROM			equ $FF0000	; X-memory, 128 words
;kEPMemory_Base_PeripheralRegisters	equ $FFFF80	; X-memory, to 0xFFFFFF

INTERRUPT_TABLE_BASE 		equ $FF0000

; VBA at $FF0000 (requires 256 word alignment).  
; The vector table is exactly where the power-up reset starts fetching instructions.
; The first interrupt vector is hardware RESET.
; Only needs up to $24 (36d) .. $44 (68d) interrupts or so, not full 256 word table.

INTERRUPT_REGISTER		equ $FFFFC5	; Interrupt request/clear register
kAbortFrameBit		equ		0			; Bit 0
kStartFrameBit		equ		1			
kPutUpdateBit		equ		2
kMailInUpdateBit	equ		3
kDMAComplete1Bit	equ		4
kDMAComplete2Bit	equ		5
kDMAComplete3Bit	equ		6	
kDMAEndOfListBit	equ		7
kTimer1Bit			equ		8
kTimer2Bit			equ		9
kDMAErrorBit		equ		10
kAddressErrorBit	equ		11

; ==============================================================
;  FIFO stuff  
; ==============================================================
FIFOOUT_CONTROL_REGISTER	equ		$FFFFC8
FIFOIN_CONTROL_REGISTER		equ		$FFFFC9
SAMPLE_CONTROL_REGISTER		equ		$FFFFCA

kFIFO_Channels1			equ	$0
kFIFO_Channels2			equ	$1
kFIFO_Channels4			equ	$2
kFIFO_Channels6			equ	$3

kFIFO_SampleFormat08	equ	$0
kFIFO_SampleFormat16	equ	$1
kFIFO_SampleFormat24	equ	$2
kFIFO_SampleFormat32	equ	$3

; Output FIFO Configuration word:  
;   Bits 4:3  Sample Format = 10  (24-bits, 3 byte words ??)
;   Bits   2  Iso Bit       = 0 
;   Bits 1:0  # channels    = 2    (Stereo)
; 10-0-10 = $12
kFifoOut0_DefaultWord	equ	$12	

; Current Value is bits 23:2  (22 bits)	(from dev_apu.ref)
NV_PAPU_EPOFCUR0  equ	$0000302C 
NV_PAPU_EPOFCUR1  equ	$0000303C 
NV_PAPU_EPOFCUR2  equ	$0000304C 
NV_PAPU_EPOFCUR3  equ	$0000305C 

; End Value is bits 23:8  (16 bits) (from dev_apu.ref)
NV_PAPU_EPOFEND0  equ	$00003028 
NV_PAPU_EPOFEND1  equ	$00003038 
NV_PAPU_EPOFEND2  equ	$00003048 
NV_PAPU_EPOFEND3  equ	$00003058 

; Base Value is bits 23:8 (16 bits) (from dev_apu.ref)
NV_PAPU_EPOFBASE0  equ	$00003024 
NV_PAPU_EPOFBASE1  equ	$00003034 
NV_PAPU_EPOFBASE2  equ	$00003044 
NV_PAPU_EPOFBASE3  equ	$00003054 

; ==============================================================
;  Command stuff		Bits 11:2  offset in 4KB circular buffer
; ==============================================================
kEPMemory_Base_CommandCue		equ $FFF000		; Top 4KByte page

COMMAND_GET_REGISTER 		equ $FFFFC0	; Ptr to next read location
COMMAND_PUT_REGISTER 		equ $FFFFC1	; Ptr to 1st empty write location


MAILBOX_OUT                 equ $ffffC2
MAILBOX_IN                  equ $ffffC3

TIMER1_CONFIG               equ $FFFFB0
TIMER2_CONFIG               equ $FFFFB4

TIMER1_CONTROL              equ $FFFFB1
TIMER2_CONTROL              equ $FFFFB5

TIMER1_TERMINAL_COUNT       equ $ffffB2
TIMER2_TERMINAL_COUNT       equ $ffffB6

TIMER1_COUNT                equ $ffffB3
TIMER2_COUNT                equ $ffffB7



; ==============================================================
;  Processing control
; ==============================================================
PROCESSING_CONTROL_REGISTER		equ $FFFFC4	
kProcessingControl_SetIdleBit				equ 0		; Bit 0
kProcessingControl_NotifyInterruptBit		equ 1		

; ==============================================================
;  Global Sample Count Register		Bits 23:0
; ==============================================================
GLOBAL_SAMPLE_COUNT_REGISTER		equ $FFFFC6	

;************************************************************
; EPClearInterruptRegister	Clear  EP register
;************************************************************
EPClearInterruptRegister macro
      	movep    #$FFF,x:INTERRUPT_REGISTER	 ; "Write-1-To-Clear-One"
	endm
; ---- end EPClearInterruptRegister ----

;************************************************************
; EPClearProcessingControlRegister	Clear  EP register
;************************************************************
EPClearProcessingControlRegister macro
      	movep    #$3,x:PROCESSING_CONTROL_REGISTER	 ; "Write-1-To-Clear-One"	
	endm
; ---- end EPClearProcessingControlRegister ----

;************************************************************
; EPClearSampleControlRegister	Clear  EP register
;************************************************************
EPClearSampleControlRegister macro
      	movep    #$0,x:SAMPLE_CONTROL_REGISTER	 
	endm
; ---- end EPClearSampleControlRegister ----

;************************************************************
; EPSetIdleBit		Assert EP Idle signal (Bit 0)
;************************************************************
EPSetIdleBit macro
	movep    #1,x:PROCESSING_CONTROL_REGISTER	 ; "Write-1-To-Clear-One"
	endm
; ---- end EPSetIdleBit ----

;************************************************************
; EPClearStartFrameBit		Start Frame (Bit 1)
;************************************************************
EPClearStartFrameBit macro
	movep    #2,x:INTERRUPT_REGISTER  ; "Write-1-To-Clear-One"	
	endm
; ---- end EPClearStartFrameBit ----

;************************************************************
; EPClearDataRAM	 Zero-fill X and Y memories
;************************************************************
EPClearDataRAM macro
	jsr	 EPClearXRAM
	jsr	 EPClearYRAM
	endm
; ---- end EPClearDataRAM ----

;************************************************************
; EPWritePC	 For IKOS debug, write Program Counter value to memory
;************************************************************
EPWritePC macro
	move	#0,r5
	lra		r5,x0	; yep, here's the PC relative address PC+0
	move	x0,a
	nop				; to avoid warnings
	dec		a
	nop				; to avoid warnings
	move	a,y:kCounter
	endm
; ---- end EPWritePC ----



