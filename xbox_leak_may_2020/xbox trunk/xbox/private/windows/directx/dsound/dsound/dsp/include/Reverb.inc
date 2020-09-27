;	File	:	Reverb.inc
;	Version	:	0.0
;	Date	:	22/8/2000
;
;	Author	:	RDA
;				Sensaura Ltd
;
;
;
; Description
; ===========
; Reverb definitions and macros
;
; Version history
; ===============
;


; Options : Don't preserve comments in macros
	opt     nocm





DMAcbs_pNxtCmd				set 0
DMAcbs_controlBits			set 1
DMAcbs_count				set 2
DMAcbs_pDspStart			set 3
DMAcbs_smOffset				set 4
DMAcbs_smBaseAddress		set 5
DMAcbs_smSizeMinus1			set 6

DMAcbs_SizeofDMAcbs			set 7

DMAcbs_eol					set $004000				; Set pNxtCmd=DMAcbs_eol to indicate the end of the linked list
DMAcbs_Bit_eol				set 14

; Control bit values
DMAcbs_InterleaveOFF		set $000000
DMAcbs_InterleaveON			set $000001

DMAcbs_HostToDsp			set $000000
DMAcbs_DspToHost			set $000002
DMAcbs_Bit_rdDSPmem			set 1

DMAcbs_IOCOFF				set $000000
DMAcbs_IOC1					set $000004
DMAcbs_IOC2					set $000005
DMAcbs_IOC3					set $000006

DMAcbs_smOffWrBkON			set $000010
DMAcbs_smOffWrBkOFF			set $000000

DMAcbs_smBufIdNotifier		set $000000
DMAcbs_smBufIdLinear		set $000040
DMAcbs_smBufIdDMAreg0		set $000100
DMAcbs_smBufIdDMAreg1		set $000120
DMAcbs_smBufIdDMAreg2		set $000140
DMAcbs_smBufIdDMAreg3		set $000160
DMAcbs_smBufIdCircular		set $0001C0
DMAcbs_smBufIdOffset		set $0001E0

DMAcbs_isoOFF				set $000000
DMAcbs_isoON				set $000200

DMAcbs_smDataFmt0			set $000000
DMAcbs_smDataFmt1			set $000400
DMAcbs_smDataFmt2			set $000800
DMAcbs_smDataFmt3			set $000C00
DMAcbs_smDataFmt4			set $001000

;
DMAcbs_inc					set $004000				; Multiply the required increment by this value to calculate 
													; the correct increment for the control block
DMAcbs_inc1					set $004000				; Increment by 1 after each transfer


DMAcbs_count0				set $000000
DMAcbs_count1				set $000010

DMAcbs_nul					set $004000


NewDMAcbs	MACRO M_pNxtCmd,M_controlbits,M_count,M_pDspStart,M_HostBase,M_HostSize,M_HostOffset
	dc	M_pNxtCmd
	dc	M_controlbits+DMAcbs_InterleaveOFF+DMAcbs_IOCOFF+DMAcbs_smOffWrBkON+DMAcbs_smBufIdCircular+DMAcbs_isoOFF+DMAcbs_smDataFmt2+DMAcbs_inc1
	dc	M_count
	dc	M_pDspStart
	dc	M_HostOffset
	dc	M_HostBase
	dc	M_HostSize-1
	ENDM






;==========================
;	IIR data structure
;==========================
;
; The notation used for IIR filters is as follows
;	'a'		A feedback (recursive) gain
;	'b'		A feedforward (non-recursive) gain

IIR_Offset					set 0
IIR_DelayElement			set IIR_Offset
IIR_Offset					set IIR_Offset+1

IIR_CoefficientA1			set IIR_Offset
IIR_Offset					set IIR_Offset+1

IIR_CoefficientB0			set IIR_Offset
IIR_Offset					set IIR_Offset+1

SizeOfIIR_Data				set	IIR_Offset


;	Parameters
;		M_A1	Initial coefficient for coefficient A1
;		M_B0	Initial coefficient for coefficient B0
NewIIR_Coefficients	MACRO M_A1,M_B0
	dc	0					; Initial delay element value
	dc	M_A1				; Initial value of coefficient A1
	dc	M_B0				; Initial value of coefficient B0
	ENDM

;                  __
;          ---A1--|__|--
;         |             |
;   --B0-(+)-----------------
;



;==========================
;	IIR2 data structure
;==========================
;	Stores data for a second order section

IIR2_Offset					set 0

IIR2_CoefficientA0			set IIR2_Offset
IIR2_Offset					set IIR2_Offset+1
IIR2_CoefficientA1			set IIR2_Offset
IIR2_Offset					set IIR2_Offset+1
IIR2_CoefficientA2			set IIR2_Offset
IIR2_Offset					set IIR2_Offset+1

IIR2_CoefficientB1			set IIR2_Offset
IIR2_Offset					set IIR2_Offset+1
IIR2_CoefficientB2			set IIR2_Offset
IIR2_Offset					set IIR2_Offset+1

SizeOfIIR2_Coefficients		set	IIR2_Offset



;	Parameters
;		M_A0	Initial coefficient for coefficient A0
;		M_A1	Initial coefficient for coefficient A1
;		M_A2	Initial coefficient for coefficient A2
;		M_B1	Initial coefficient for coefficient B1
;		M_B2	Initial coefficient for coefficient B2
IIR2_Coefficients	MACRO M_A0,M_A1,M_A2,M_B1,M_B2
	dc	M_A0				; Initial value of coefficient A0
	dc	M_A1				; Initial value of coefficient A1
	dc	M_A2				; Initial value of coefficient A2
	dc	M_B1				; Initial value of coefficient B1
	dc	M_B2				; Initial value of coefficient B2
	ENDM



;                 _
;         ,--B2--|_|--A2--.
;         |       |       |
;         |       _       |
;         |--B1--|_|--A1--|
;         |       |       |
;   -----(+)----------A0-(+)----
;



;==========================
;	Data structure for the second order interpolation filters
;==========================
;	Stores data for a second order section

IIR2_CoefficientY0			set 0
IIR2_CoefficientY1			set 1

SizeOfIIR2_Data				set	2


;	Parameters
;		None
IIR2_Data	MACRO
	dc	0					; Initial delay element value
	dc	0					; Initial delay element value

	ENDM


; See 'Parameters.inc' for the definition of the IIR parameters


        IF @DEF('DEBUG')
              
DEBUG_IWOZHERE_OFFSET           equ $800
DEBUG_IWOZHERE_COUNTER          equ (DEBUG_IWOZHERE_OFFSET)
DEBUG_IWOZHERE_TEMP             equ (DEBUG_IWOZHERE_OFFSET+1)
DEBUG_IWOZHERE_DATA             equ (DEBUG_IWOZHERE_OFFSET+2)
SIZEOFDEBUGLOG                  set 256
IWOZHERE MACRO IWOZ_ID
	;DEBUGGERLOG
        move	a,x:(DEBUG_IWOZHERE_TEMP)
        move	IWOZ_ID,a
        move    x:(DEBUG_IWOZHERE_COUNTER),n7
        move    #DEBUG_IWOZHERE_DATA,r7
        move    a,x:(r7+n7)
        move    n7,a
        add     #1,a
	cmp	#SIZEOFDEBUGLOG,a
	clr	a	ifge
        move    a,x:(DEBUG_IWOZHERE_COUNTER)
        move	x:(DEBUG_IWOZHERE_TEMP),a

	ENDM

        ELSE

IWOZHERE MACRO IWOZ_ID
        ENDM

        ENDIF

; A debugging macro to log when a frame contains a non-zero value
LOGIFFRAMENONZERO MACRO PFRAMEADDRESS
	move	r0,x:(DEBUG_IWOZHERE_TEMP+1)
	move	a,x:(DEBUG_IWOZHERE_TEMP+2)
	move	x:(r5+PFRAMEADDRESS-StaticWorkspaceBase),r0
	
	do		#SizeOfFrame,_debugloop
		move	x:(r0)+,a
		cmp		#0,a
		beq		_debugequaltozero
		DEBUGGERLOG	#*
		DEBUGGERLOG	r0
_debughang
		bra	_debughang
_debugequaltozero
		nop
_debugloop
	move	x:(DEBUG_IWOZHERE_TEMP+2),a
	move	x:(DEBUG_IWOZHERE_TEMP+1),r0
	ENDM



; A debugging macro to log when a frame contains a non-zero value
LOGIFBLOCKNONZERO MACRO ADDR_START,ADDR_END
	move	r0,x:(DEBUG_IWOZHERE_TEMP+1)
	move	a,x:(DEBUG_IWOZHERE_TEMP+2)
	
	move	#ADDR_START,r0

	do		#(ADDR_END-ADDR_START)/4,_debugloop1
		move	x:(r0),a
;		cmp		#0,a
;		beq		_debugequaltozero1
;		DEBUGGERLOG #$123456
;		DEBUGGERLOG #$123456
;		DEBUGGERLOG	#*
;		DEBUGGERLOG	r0
;_debughang1
;		bra	_debughang1
;_debugequaltozero1
;		move	r0,a
;		add		#4,a
;		move	a,r0
_debugloop1
	move	x:(DEBUG_IWOZHERE_TEMP+2),a
	move	x:(DEBUG_IWOZHERE_TEMP+1),r0
	ENDM