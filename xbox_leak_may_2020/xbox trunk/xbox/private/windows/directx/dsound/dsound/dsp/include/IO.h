;**************************************************************************
;
;  IO.h    

;**************************************************************************        
	include 'io56kevm.h'


kFrameLength	equ		32
kPCValue		equ		0
kCounter		equ		0

;
; NOTE: keep this in sync with dsound.h and the number of fx sends
; dsound mcpx uses
;

NUM_FX_SEND_BINS    equ     20

; ==============================================================
; Global Processor (GP) Address Map as viewed from dma controller
; ==============================================================
; 24 bit words (3 bytes) in 32-bit containers (4 bytes)
;
; Peripheral registers (Control,Status) mapped to upper 128 words
; of X-memory (0xFFFF80 to 0xFFFFFF) (use compact movep instruction)
;
; DSP    memory addresses in words
; System memory address   in bytes 

kGPMemory_Size_XRAM		equ (4*1024)
kGPMemory_Size_YRAM		equ (2*1024)
kGPMemory_Size_PRAM		equ (4*1024)

kGPMemory_Base					equ $000000
kGPMemory_XRAM_Base				equ (kGPMemory_Base+$000000)		; $000000 .. $000BFF
kGPMemory_XRAM_MixBufferTemp	equ (kGPMemory_XRAM_Base+$000C00)	; $000C00 .. $000FFF
kGPMemory_XRAM_Unused			equ (kGPMemory_XRAM_Base+$001000)	; $001000 .. $0013FF
kGPMemory_XRAM_MixBufferRead	equ (kGPMemory_XRAM_Base+$001400)	; $001400 .. $0017FF
kGPMemory_XRAM_PeripheralRegs	equ (kGPMemory_XRAM_Base+$FFFF80)	; $FFFF80 .. $FFFFFF

kGPMemory_Base_YRAM				equ $1800
kGPMemory_Base_PRAM				equ $2800


; Define these so I don't have to update older files
kGPMemory_Base_XRAM	equ kGPMemory_XRAM_Base
kGPMemory_YRAM_Base	equ kGPMemory_Base_YRAM	

; ==============================================================
; Encode Processor (EP) Address Map 
; ==============================================================
; 24 bit words (3 bytes) in 32-bit containers (4 bytes)
;
; Peripheral registers (Control,Status) mapped to upper 128 words
; of X-memory (0xFFFF80 to 0xFFFFFF) (use compact movep instruction)
;
; DSP    memory addresses in words
; System memory address   in bytes 

kEPMemory_Size_XRAM		equ (3*1024)
kEPMemory_Size_YRAM		equ     256
kEPMemory_Size_YROM		equ (2*1024)
kEPMemory_Size_PRAM		equ (4*1024)

kEPMemory_Base				equ $000000
kEPMemory_Base_XRAM			equ (kEPMemory_Base+$000000)		; $000000 .. $000BFF
kEPMemory_Base_XRAM_PeripheralRegs	equ (kEPMemory_Base_XRAM+$FFFF80)	; $FFFF80 .. $FFFFFF

kEPMemory_Base_YRAM			equ (kEPMemory_Base+$001800)
kEPMemory_Base_YROM			equ (kEPMemory_Base_YRAM+$000800)
kEPMemory_Base_PRAM			equ (kEPMemory_Base+$002000)
kEPMemory_Base_PRAM_BootRom		equ (kEPMemory_Base_PRAM+$FF000)	; $FF0000 .. $FF003F

; ==============================================================
; NOTE:Scratch memory offsets for various GP data blocks
; this MUST match what is in tools\xps2\dsp.h!!!!!
; ==============================================================
; Byte Addresses of Command Node data blocks (DMA'd on the fly)
SM_OFFSET_COMMAND_BLOCK		equ		 2048 


; ==============================================================
; Command block : used to communicate with DSP default loader program
; ==============================================================

COMMAND_BLOCK_FIELD_OFFSET      						equ	0
COMMAND_BLOCK_FIELD_CODE_LENGTH							equ	1
COMMAND_BLOCK_FIELD_STATE_OFFSET                        equ 2
COMMAND_BLOCK_FIELD_STATE_LENGTH						equ	3
COMMAND_BLOCK_FIELD_COMMAND_FLAGS						equ	4
COMMAND_BLOCK_FIELD_STATUS   							equ	5

DATASIZE_DWORD                  equ 4
DATASIZE_COMMAND_BLOCK	        equ	(1+COMMAND_BLOCK_FIELD_STATUS)
DATASIZE_BYTES_COMMAND_BLOCK    equ (DATASIZE_COMMAND_BLOCK*4)


;
; the AC3 dma output gets placed right after the end of scratch space
; NOTE: THIS MUST STAY IN SYNC WITH OFFSET CALCULATED IN DSP.H

SM_OFFSET_AC3_DMA_OUTPUT        equ $8000

;
; the number belows comes form the following calc in mcpxhw.h
; #define MCPX_HW_AC3_NUM_INPUT_PAGES     (((AC3_BUFFER_COUNT * AC3_FRAME_SIZE * AC3_CONTAINER_SIZE * AC3_NUM_CHANNELS) + PAGE_SIZE-1)  / PAGE_SIZE)
;

DATASIZE_BYTES_AC3_DMA_OUTPUT   equ (4096*3)

SM_OFFSET_CODE_BLOCK            equ (SM_OFFSET_COMMAND_BLOCK+DATASIZE_BYTES_COMMAND_BLOCK)
SM_OFFSET_MULTIPASS_BUFFER      equ (SM_OFFSET_AC3_DMA_OUTPUT+DATASIZE_BYTES_AC3_DMA_OUTPUT)
DATASIZE_MULTIPASS_BUFFER       equ (kFrameLength*NUM_FX_SEND_BINS)
DATASIZE_BYTES_MULTIPASS_BUFFER equ (DATASIZE_MULTIPASS_BUFFER*DATASIZE_DWORD)

BIT_COMMAND_BLOCK_COMMAND_PRESENT			equ	0
BIT_COMMAND_BLOCK_COMMAND_LOAD_CODE         equ 0
BIT_COMMAND_BLOCK_COMMAND_LOAD_XRAM_STATE   equ 1
BIT_COMMAND_BLOCK_COMMAND_UPDATE_XRAM_STATE equ 2
    
; status definitions

COMMAND_BLOCK_STATUS_IDLE         equ $AA
COMMAND_BLOCK_STATUS_PROCESSED_CMD          equ $FF


; the global block of the concatenated FX state will be placed at the
; the following offset

GLOBAL_FX_STATE_XRAM_BASE_OFFSET   equ 128 ; eq of 512 in SM
DATASIZE_GLOBAL_FX_STATE           equ 512 ; eq. of 2048 bytes in system ram

AC3_BUFFER_SIZE                    equ $800

;
; debug variables
;

DATASIZE_DEBUG_VARIABLES           equ 4
DEBUG_VARIABLES_OFFSET             equ (GLOBAL_FX_STATE_XRAM_BASE_OFFSET-DATASIZE_DEBUG_VARIABLES)


; definition of the DSP_STATE_FIXED data struct that each FX state block
; has at the base of its X-memory state area

FX_STATE_SCRATCH_BASE    equ 0
FX_STATE_SCRATCH_LENGTH  equ 1
FX_STATE_YMEM_BASE       equ 2
FX_STATE_YMEM_LENGTH     equ 3
FX_STATE_FLAGS           equ 4

DATASIZE_FX_STATE_FIXED  equ 5

;
; flag definitions
;

BIT_FX_STATE_FLAG_INITIALIZED equ $0
VALUE_FX_STATE_FLAG_INITIALIZED  equ $1

BIT_FX_STATE_FLAG_GLOBAL_FX   equ $1
VALUE_FX_STATE_FLAG_GLOBAL_FX equ $2

BIT_FX_STATE_FLAG_UPDATE   equ $2
VALUE_FX_STATE_FLAG_UPDATE equ $4

BIT_FX_STATE_FLAG_PROCESS_STEREO   equ $3
VALUE_FX_STATE_FLAG_PROCESS_STEREO equ $8

; zero inputs Data struct

FX_STATE_IO0_OUT0		    equ	(DATASIZE_FX_STATE_FIXED)
FX_STATE_IO0_OUT1		    equ	(DATASIZE_FX_STATE_FIXED+1)
FX_STATE_IO0_OUT2		    equ	(DATASIZE_FX_STATE_FIXED+2)
FX_STATE_IO0_OUT3		    equ	(DATASIZE_FX_STATE_FIXED+3)
FX_STATE_IO0_OUT4		    equ	(DATASIZE_FX_STATE_FIXED+4)
FX_STATE_IO0_OUT5		    equ	(DATASIZE_FX_STATE_FIXED+5)
FX_STATE_IO0_OUT6		    equ	(DATASIZE_FX_STATE_FIXED+6)
FX_STATE_IO0_OUT7		    equ	(DATASIZE_FX_STATE_FIXED+7)



; Mono In, Mono Out Data Structure
FX_STATE_IO1_IN			    equ	(DATASIZE_FX_STATE_FIXED)
FX_STATE_IO1_OUT0		    equ	(DATASIZE_FX_STATE_FIXED+1)
FX_STATE_IO1_OUT1		    equ	(DATASIZE_FX_STATE_FIXED+2)
FX_STATE_IO1_OUT2		    equ	(DATASIZE_FX_STATE_FIXED+3)
FX_STATE_IO1_OUT3		    equ	(DATASIZE_FX_STATE_FIXED+4)
FX_STATE_IO1_OUT4		    equ	(DATASIZE_FX_STATE_FIXED+5)
FX_STATE_IO1_OUT5		    equ	(DATASIZE_FX_STATE_FIXED+6)
FX_STATE_IO1_OUT6		    equ	(DATASIZE_FX_STATE_FIXED+7)
FX_STATE_IO1_OUT7		    equ	(DATASIZE_FX_STATE_FIXED+8)


; Stereo In, Stereo Out Data Structure
FX_STATE_IO2_INL			equ	(DATASIZE_FX_STATE_FIXED)
FX_STATE_IO2_INR			equ	(DATASIZE_FX_STATE_FIXED+1)
FX_STATE_IO2_OUTL		    equ	(DATASIZE_FX_STATE_FIXED+2)
FX_STATE_IO2_OUTR		    equ	(DATASIZE_FX_STATE_FIXED+3)

; 2 inputs, 1 outputs
FX_STATE_IO2_IN0			equ	(DATASIZE_FX_STATE_FIXED)
FX_STATE_IO2_IN1			equ	(DATASIZE_FX_STATE_FIXED+1)
FX_STATE_IO2_OUT0		    equ	(DATASIZE_FX_STATE_FIXED+2)

; 2 inputs, 4 outputs
FX_STATE_IO2_OUT1		    equ	(DATASIZE_FX_STATE_FIXED+3)
FX_STATE_IO2_OUT2		    equ	(DATASIZE_FX_STATE_FIXED+4)
FX_STATE_IO2_OUT3		    equ	(DATASIZE_FX_STATE_FIXED+5)

; 3 inputs, 2 outputs
FX_STATE_IO3_IN0			equ	(DATASIZE_FX_STATE_FIXED)
FX_STATE_IO3_IN1			equ	(DATASIZE_FX_STATE_FIXED+1)
FX_STATE_IO3_IN2			equ	(DATASIZE_FX_STATE_FIXED+2)
FX_STATE_IO3_OUT0		    equ	(DATASIZE_FX_STATE_FIXED+3)
FX_STATE_IO3_OUT1		    equ	(DATASIZE_FX_STATE_FIXED+4)

; 4 inputs, 2 outputs
FX_STATE_IO4_IN0			equ	(DATASIZE_FX_STATE_FIXED)
FX_STATE_IO4_IN1			equ	(DATASIZE_FX_STATE_FIXED+1)
FX_STATE_IO4_IN2			equ	(DATASIZE_FX_STATE_FIXED+2)
FX_STATE_IO4_IN3			equ	(DATASIZE_FX_STATE_FIXED+3)
FX_STATE_IO4_OUT0		    equ	(DATASIZE_FX_STATE_FIXED+4)
FX_STATE_IO4_OUT1		    equ	(DATASIZE_FX_STATE_FIXED+5)

; 4 inputs, 4 outputs
FX_STATE_IO4_OUT2		    equ	(DATASIZE_FX_STATE_FIXED+6)
FX_STATE_IO4_OUT3		    equ	(DATASIZE_FX_STATE_FIXED+7)

; 6 inputs, 1 output
FX_STATE_IO6_IN0			equ	(DATASIZE_FX_STATE_FIXED)
FX_STATE_IO6_IN1			equ	(DATASIZE_FX_STATE_FIXED+1)
FX_STATE_IO6_IN2			equ	(DATASIZE_FX_STATE_FIXED+2)
FX_STATE_IO6_IN3			equ	(DATASIZE_FX_STATE_FIXED+3)
FX_STATE_IO6_IN4			equ	(DATASIZE_FX_STATE_FIXED+4)
FX_STATE_IO6_IN5			equ	(DATASIZE_FX_STATE_FIXED+5)
FX_STATE_IO6_OUT0		    equ	(DATASIZE_FX_STATE_FIXED+6)

DATASIZE_DSP_FX_STATE_MONO      equ (FX_STATE_IO1_OUT0+1)
DATASIZE_DSP_FX_STATE_1IN_2OUT  equ (FX_STATE_IO1_OUT1+1)
DATASIZE_DSP_FX_STATE_1IN_4OUT  equ (FX_STATE_IO1_OUT3+1)
DATASIZE_DSP_FX_STATE_1IN_8OUT  equ (FX_STATE_IO1_OUT7+1)

DATASIZE_DSP_FX_STATE_STEREO    equ (FX_STATE_IO2_OUTR+1)

DATASIZE_DSP_FX_STATE_2IN_1OUT  equ (FX_STATE_IO2_OUT0+1)
DATASIZE_DSP_FX_STATE_2IN_4OUT  equ (FX_STATE_IO2_OUT3+1)

DATASIZE_DSP_FX_STATE_3IN_2OUT  equ (FX_STATE_IO3_OUT1+1)

DATASIZE_DSP_FX_STATE_4IN_2OUT  equ (FX_STATE_IO4_OUT1+1)
DATASIZE_DSP_FX_STATE_4IN_4OUT  equ (FX_STATE_IO4_OUT3+1)

DATASIZE_DSP_FX_STATE_6IN_1OUT  equ (FX_STATE_IO6_OUT0+1)

DATASIZE_DSP_FX_STATE_6IN_0OUT  equ (FX_STATE_IO6_OUT0)


kFxOutShift	equ	2
kFxOutGain	set	32.0
kFxInGain	set	(1.0/2.0)

; **** Mix Buffer definitions
; The 1K of Mix buffer memory can be used as "standard" 
; read/write memory at X-memory address locations 0x0C00 
; through 0x0FFF (24-bit word address).  However, 
; since this memory is "flipped" at each frame, only 
; variables that do not need to persist beyond a frame 
; boundary can be stored here.

; The 1K of Mix buffer memory can be read at a second 
; address region, X-memory address locations 0x1400 through
; 0x17FF.  In this area, the 32 "mix valid" bits are used 
; to force the data to "0" when the bin is "not valid".  
; this guarantees that bins without voices will read "0" 
; samples (quiet audio).

; Lower 16 mix buffers: 512 words, 32 words x 16 buffers
; Address $C00..$DFF = 3072 ..3583
kMixBufBase	equ		$1400		
kMixBuf00	equ		(kMixBufBase)
kMixBuf01	equ		(kMixBuf00+kFrameLength)
kMixBuf02	equ		(kMixBuf01+kFrameLength)
kMixBuf03	equ		(kMixBuf02+kFrameLength)
kMixBuf04	equ		(kMixBuf03+kFrameLength)
kMixBuf05	equ		(kMixBuf04+kFrameLength)
kMixBuf06	equ		(kMixBuf05+kFrameLength)
kMixBuf07	equ		(kMixBuf06+kFrameLength)
kMixBuf08	equ		(kMixBuf07+kFrameLength)
kMixBuf09	equ		(kMixBuf08+kFrameLength)
kMixBuf10	equ		(kMixBuf09+kFrameLength)
kMixBuf11	equ		(kMixBuf10+kFrameLength)
kMixBuf12	equ		(kMixBuf11+kFrameLength)
kMixBuf13	equ		(kMixBuf12+kFrameLength)
kMixBuf14	equ		(kMixBuf13+kFrameLength)
kMixBuf15	equ		(kMixBuf14+kFrameLength)

; Lower 16 mix buffers: 512 words, 32 words x 16 buffers
; Address $E00..$FFF = 3584 ..3FFF
kMixBuf16	equ		(kMixBuf15+kFrameLength)
kMixBuf17	equ		(kMixBuf16+kFrameLength)
kMixBuf18	equ		(kMixBuf17+kFrameLength)
kMixBuf19	equ		(kMixBuf18+kFrameLength)
kMixBuf20	equ		(kMixBuf19+kFrameLength)
kMixBuf21	equ		(kMixBuf20+kFrameLength)
kMixBuf22	equ		(kMixBuf21+kFrameLength)
kMixBuf23	equ		(kMixBuf22+kFrameLength)
kMixBuf24	equ		(kMixBuf23+kFrameLength)
kMixBuf25	equ		(kMixBuf24+kFrameLength)
kMixBuf26	equ		(kMixBuf25+kFrameLength)
kMixBuf27	equ		(kMixBuf26+kFrameLength)
kMixBuf28	equ		(kMixBuf27+kFrameLength)
kMixBuf29	equ		(kMixBuf28+kFrameLength)
kMixBuf30	equ		(kMixBuf29+kFrameLength)
kMixBuf31	equ		(kMixBuf30+kFrameLength)

kMixBuf0	equ		kMixBuf00
kMixBuf1	equ		kMixBuf01
kMixBuf2	equ		kMixBuf02
kMixBuf3	equ		kMixBuf03
kMixBuf4	equ		kMixBuf04
kMixBuf5	equ		kMixBuf05
kMixBuf6	equ		kMixBuf06
kMixBuf7	equ		kMixBuf07
kMixBuf8	equ		kMixBuf08
kMixBuf9	equ		kMixBuf09

kTmpBufBase	equ		(GLOBAL_FX_STATE_XRAM_BASE_OFFSET+DATASIZE_GLOBAL_FX_STATE)	
kTmpBufCount	equ		4	
kTmpBuf00	equ		kTmpBufBase
kTmpBuf01	equ		(kTmpBuf00+kFrameLength)
kTmpBuf02	equ		(kTmpBuf01+kFrameLength)
kTmpBuf03	equ		(kTmpBuf02+kFrameLength)

kTmpBuf0	equ		kTmpBuf00
kTmpBuf1	equ		kTmpBuf01
kTmpBuf2	equ		kTmpBuf02
kTmpBuf3	equ		kTmpBuf03


kReadWriteMixBufBase    equ     $C00
kReadWriteMixBuf00	equ		(kReadWriteMixBufBase)
kReadWriteMixBuf01	equ		(kReadWriteMixBuf00+kFrameLength)
kReadWriteMixBuf02	equ		(kReadWriteMixBuf01+kFrameLength)
kReadWriteMixBuf03	equ		(kReadWriteMixBuf02+kFrameLength)
kReadWriteMixBuf04	equ		(kReadWriteMixBuf03+kFrameLength)
kReadWriteMixBuf05	equ		(kReadWriteMixBuf04+kFrameLength)
kReadWriteMixBuf06	equ		(kReadWriteMixBuf05+kFrameLength)
kReadWriteMixBuf07	equ		(kReadWriteMixBuf06+kFrameLength)
kReadWriteMixBuf08	equ		(kReadWriteMixBuf07+kFrameLength)
kReadWriteMixBuf09	equ		(kReadWriteMixBuf08+kFrameLength)
kReadWriteMixBuf10	equ		(kReadWriteMixBuf09+kFrameLength)
kReadWriteMixBuf11	equ		(kReadWriteMixBuf10+kFrameLength)
kReadWriteMixBuf12	equ		(kReadWriteMixBuf11+kFrameLength)
kReadWriteMixBuf13	equ		(kReadWriteMixBuf12+kFrameLength)
kReadWriteMixBuf14	equ		(kReadWriteMixBuf13+kFrameLength)
kReadWriteMixBuf15	equ		(kReadWriteMixBuf14+kFrameLength)

; Lower 16 mix buffers: 512 words, 32 words x 16 buffers
; Address $E00..$FFF = 3584 ..3FFF
kReadWriteMixBuf16	equ		(kReadWriteMixBuf15+kFrameLength)
kReadWriteMixBuf17	equ		(kReadWriteMixBuf16+kFrameLength)
kReadWriteMixBuf18	equ		(kReadWriteMixBuf17+kFrameLength)
kReadWriteMixBuf19	equ		(kReadWriteMixBuf18+kFrameLength)
kReadWriteMixBuf20	equ		(kReadWriteMixBuf19+kFrameLength)
kReadWriteMixBuf21	equ		(kReadWriteMixBuf20+kFrameLength)
kReadWriteMixBuf22	equ		(kReadWriteMixBuf21+kFrameLength)
kReadWriteMixBuf23	equ		(kReadWriteMixBuf22+kFrameLength)
kReadWriteMixBuf24	equ		(kReadWriteMixBuf23+kFrameLength)
kReadWriteMixBuf25	equ		(kReadWriteMixBuf24+kFrameLength)
kReadWriteMixBuf26	equ		(kReadWriteMixBuf25+kFrameLength)
kReadWriteMixBuf27	equ		(kReadWriteMixBuf26+kFrameLength)
kReadWriteMixBuf28	equ		(kReadWriteMixBuf27+kFrameLength)
kReadWriteMixBuf29	equ		(kReadWriteMixBuf28+kFrameLength)
kReadWriteMixBuf30	equ		(kReadWriteMixBuf29+kFrameLength)
kReadWriteMixBuf31	equ		(kReadWriteMixBuf30+kFrameLength)

;
; friendly names that match mixbins indices in dsound.h
;

kFrontLeftMixBufRW	    equ		(kReadWriteMixBufBase)
kFrontRightMixBufRW	    equ		(kReadWriteMixBuf00+kFrameLength)
kFrontCenterMixBufRW	equ		(kReadWriteMixBuf01+kFrameLength)
kLFEMixBufRW      	    equ		(kReadWriteMixBuf02+kFrameLength)
kBackLeftMixBufRW 	    equ		(kReadWriteMixBuf03+kFrameLength)
kBackRightMixBufRW	    equ		(kReadWriteMixBuf04+kFrameLength)
kXTLKFrontLeftMixBufRW	equ		(kReadWriteMixBuf05+kFrameLength)
kXTLKFrontRightMixBufRW	equ		(kReadWriteMixBuf06+kFrameLength)
kXTLKBackLeftMixBufRW	equ		(kReadWriteMixBuf07+kFrameLength)
kXTLKBackRightMixBufRW	equ		(kReadWriteMixBuf08+kFrameLength)
kI3DL2MixBufRW    	    equ		(kReadWriteMixBuf09+kFrameLength)
kFxSendMixBuf00RW 	    equ		(kReadWriteMixBuf10+kFrameLength)

kFrontLeftMixBuf	    equ		(kMixBufBase)
kFrontRightMixBuf	    equ		(kMixBuf00+kFrameLength)
kFrontCenterMixBuf	    equ		(kMixBuf01+kFrameLength)
kLFEMixBuf      	    equ		(kMixBuf02+kFrameLength)
kBackLeftMixBuf 	    equ		(kMixBuf03+kFrameLength)
kBackRightMixBuf	    equ		(kMixBuf04+kFrameLength)
kXTLKFrontLeftMixBuf	equ		(kMixBuf05+kFrameLength)
kXTLKFrontRightMixBuf	equ		(kMixBuf06+kFrameLength)
kXTLKBackLeftMixBuf 	equ		(kMixBuf07+kFrameLength)
kXTLKBackRightMixBuf	equ		(kMixBuf08+kFrameLength)
kI3DL2MixBuf    	    equ		(kMixBuf09+kFrameLength)
kFxSendMixBuf00 	    equ		(kMixBuf10+kFrameLength)
kFxSendMixBuf01 	    equ		(kMixBuf11+kFrameLength)
kFxSendMixBuf02 	    equ		(kMixBuf12+kFrameLength)
kFxSendMixBuf03 	    equ		(kMixBuf13+kFrameLength)
kFxSendMixBuf04 	    equ		(kMixBuf14+kFrameLength)
kFxSendMixBuf05 	    equ		(kMixBuf15+kFrameLength)
kFxSendMixBuf06 	    equ		(kMixBuf16+kFrameLength)
kFxSendMixBuf07 	    equ		(kMixBuf17+kFrameLength)
kFxSendMixBuf08 	    equ		(kMixBuf18+kFrameLength)
kFxSendMixBuf09 	    equ		(kMixBuf19+kFrameLength)
kFxSendMixBuf10 	    equ		(kMixBuf20+kFrameLength)


;************************************************************
; mSetUpIO		Set up analog I/O for Motorola EVM board
;************************************************************
mSetUpIO	macro

	ori     #$03,mr                ; mask interrupts

; Clear inputs & outputs
	move    #>(ioData+IO_RX_BUFF_BASE),r0      
	move    #>$FFFF,m0
	move    #0,x0
	rep     #4
	move    x0,x:(r0)+              

	move    #>(ioData+IO_RX_BUFF_BASE),x0
	move    x0,x:(ioData+IO_RX_PTR)
	move    #>(ioData+IO_TX_BUFF_BASE),x0
	move    x0,x:(ioData+IO_TX_PTR)

;------------------------------------------------------------
; FST/FSR and SCKT/SCKR are generated from the PLD
; and fed to the DSP, A/D and D/A converters
;------------------------------------------------------------

	movep   #$0c0200,x:M_TCCR
		;FST is input                            (bit22=0)
		;external clock source drives SCKT       (bit21=0)
		;negative FST polarity                   (bit19=1)
		;data & FST clocked out on rising edge  (bit18=1)
		;2 words per frame                       (bit13:9=00001)

	 movep   #$0c0200,x:M_RCCR
			       
       ; movep   #$080200,x:M_RCCR
		;FSR is input                            (bit22=0)
		;external clock source drives SCKR       (bit21=0)
		;negative FSR polarity                   (bit19=1)
		;data & FSR clocked in on rising edge   (bit18=0)BAK(121997)
		;2 words per frame                       (bit13:9=00001)

	movep   #$000000,x:M_SAICR

	movep   #$d17D01,x:M_RCR                        ;RX0 enabled
		;RX1 enabled                             (bit1=1) MLS 12/20/97
		;RX0, RX2, RX3 disabled                  (bit3:2,0=000)
		;reserved                                (bit5:4=00)  
		;MSB shifted first                       (bit6=0)
		;word left-aligned                       (bit7=0)
		;network mode                            (bit9:8=01)
		;32-bit slot length, 24-bit word length  (bit14:10=11111)
		;word-length frame sync                  (bit15=0)
		;frame sync occurs 1 clock cycle earlier (bit16=1)
		;reserved                                (bit19:17=000)
		;RLIE, RIE, REIE enabled                 (bit23:20=0101)
		;bit23 RLIE
		;bit22 RIE
		;bit21 REDIE
		;bit20 REIE

	movep   #$d17d00,x:M_TCR                      ; Mithra 08-09-99
		;TX0, TX1, TX2 enabled                   (bit3:0=0011)
		;TX3, TX4, TX5 disabled                  (bit5:4=00)
		;MSB shifted first                       (bit6=0)
		;word left-aligned                       (bit7=0)
		;network mode                            (bit9:8=01)
		;32-bit slot length, 24-bit word length  (bit14:10=11111)
		;word length frame sync                  (bit15=0)
		;frame sync occurs 1 clock cycle earlier (bit16=0)
		;reserved                                (bit19:17=000)
		;TLIE, TIE, TEIE enabled                 (bit23:20=0101)
		;bit23 TLIE
		;bit22 TIE
		;bit21 TEDIE
		;bit20 TEIE


	movep   #$000fdb,x:M_PCRC       ; SDO0-SDO3 set for ESAI  - Mithra	 09-08-99
	movep   #$000fdb,x:M_PRRC       ; SDO0-SDO3 set for ESAI  - Mithra	 09-08-99


	movep   #$ffffff,x:M_RSMA        ;MLS 12/20/97
	movep   #$ffffff,x:M_RSMB        ;MLS 12/20/97

	movep   #$000003,x:M_TSMA
	movep   #$000003,x:M_TSMB
	movep   #$000000,x:M_TX0        ;zero out transmitter 0
	movep   #$000000,x:M_TX1        ;zero out transmitter 1
	movep   #$000000,x:M_TX2        ;zero out transmitter 2
	movep   #$000000,x:M_TX3        ;zero out transmitter 3
	bset    #0,x:M_TCR              ;now enable TX0
	bset    #1,x:M_TCR              ;now enable TX1
	bset    #2,x:M_TCR              ;now enable TX2
	bset    #3,x:M_TCR              ;now enable TX3

	andi    #$FC,mr                 ;enable all interrupt levels

	endm
; ---- end mSetUpIO ----

;************************************************************
; mWaitForInputData	
;************************************************************
mWaitForInputData	macro
       jclr    #kRightReceive,X:(ioData+IO_FLAGS),*
       bclr    #kRightReceive,X:(ioData+IO_FLAGS)
	endm

;************************************************************
; mMonoOut	Output reg A to left and right channels
;************************************************************
mMonoOut	macro

; Output 
	move    a,x:(ioData+IO_TX_BUFF_BASE)       ; left
	move    a,x:(ioData+IO_TX_BUFF_BASE+1)      ; right
	endm

;************************************************************
; mStereoOut	Output reg A,B to left and right channels
;************************************************************
mStereoOut	macro

; Output 
	move    a,x:(ioData+IO_TX_BUFF_BASE)       ; left
	move    b,x:(ioData+IO_TX_BUFF_BASE+1)      ; right
	endm

;************************************************************
; mFxStereoIn	
;************************************************************
mFxStereoIn	macro
	move	#>kFxInGain,y0
	move	x:(ioData+IO_RX_BUFF_BASE),x0
	mpy		x0,y0,a
	move	a,x:(fxIOData+FXIO_INL)
	move	x:(ioData+IO_RX_BUFF_BASE+1),x0
	mpy		x0,y0,a
	move	a,x:(fxIOData+FXIO_INR)
	endm

;************************************************************
; mFxOutLToIn	
;************************************************************
mFxOutLToIn	macro
	move	x:(fxIOData+FXIO_OUTL),x0
	move	x0,x:(fxIOData+FXIO_INL)
	endm

;************************************************************
; mFxInLToOutL	
;************************************************************
mFxInLToOutL	macro
	move	x:(fxIOData+FXIO_INL),x0
	move	x0,x:(fxIOData+FXIO_OUTL)
	endm

;************************************************************
; mFxOutLToOutR	
;************************************************************
mFxOutLToOutR	macro
	move	x:(fxIOData+FXIO_OUTL),x0
	move	x0,x:(fxIOData+FXIO_OUTR)
	endm

;************************************************************
; mFxMonoOut	Copy left channel to right and output
;************************************************************
mFxMonoOut	macro
;	move	#>0.99,y0
;	move	x:(fxIOData+FXIO_OUTL),x0
;	mpy	x0,y0,a
;	asl	#>kFxOutShift,a,a
;	move	x:(fxIOData+FXIO_OUTR),x0
;	mpy	x0,y0,b
;	asl	#fxOutShift,b,b
	move	x:(fxIOData+FXIO_OUTL),a

; Output 
	move    a,x:(ioData+IO_TX_BUFF_BASE)       ; left
	move    a,x:(ioData+IO_TX_BUFF_BASE+1)      ; right
	endm

;************************************************************
; mFxStereoOut	
;************************************************************
mFxStereoOut	macro
;	move	#>0.99,y0
;	move	x:(fxIOData+FXIO_OUTL),x0
;	mpy	x0,y0,a
;	asl	#>kFxOutShift,a,a
;	move	x:(fxIOData+FXIO_OUTR),x0
;	mpy	x0,y0,b
;	asl	#fxOutShift,b,b

	move	x:(fxIOData+FXIO_OUTL),a
	move	x:(fxIOData+FXIO_OUTR),b
; Output 
	move    a,x:(ioData+IO_TX_BUFF_BASE)       ; left
	move    b,x:(ioData+IO_TX_BUFF_BASE+1)      ; right
	endm

;************************************************************
; mResetFxIn	Clear FX variable state
;************************************************************
mResetFxIn	macro
; Clear Fx Outputs
	move	#0,x0
	move	x0,x:(fxIOData+FXIO_INL)
	move	x0,x:(fxIOData+FXIO_INR)
	endm

;************************************************************
; mResetFxOut	Clear FX variable state
;************************************************************
mResetFxOut	macro
; Clear Fx Outputs
	move	#0,x0
	move	x0,x:(fxIOData+FXIO_OUTL)
	move	x0,x:(fxIOData+FXIO_OUTR)
	endm

;************************************************************
; mResetFxIO	Clear FX variable state
;************************************************************
mResetFxIO	macro
	mResetFxIn
	mResetFxOut
	endm

;************************************************************
; mFxInToModuleIO1	Copy FX input to module input of IO1 structure
;************************************************************
mFxInToModuleIO1	macro
	move	x:(fxIOData+FXIO_INL),x0
	move	x0,x:(rD+IO1_IN)
	endm

;************************************************************
; mFxInToModuleIO2	Copy FX input to module input of IO2 structure
;************************************************************
mFxInToModuleIO2	macro
	move	x:(fxIOData+FXIO_INL),x0
	move	x0,x:(rD+IO2_IN1)
	endm

;************************************************************
; mModuleIO1toFxOutL	Copy output of IO1 structure to FX output
;************************************************************
mModuleIO1toFxOutL	macro
	move	x:(rD+IO1_OUT),x0
	move	x0,x:(fxIOData+FXIO_OUTL)
	endm

;************************************************************
; mModuleIO1AddtoFxOutL	Add output of IO1 structure to FX output
;************************************************************
mModuleIO1AddtoFxOutL	macro
	move	x:(fxIOData+FXIO_OUTL),a
	move	x:(rD+IO1_OUT),x0
	add		x0,a
	move	a,x:(fxIOData+FXIO_OUTL)
	endm

;************************************************************
; mFxOutLToModuleIO1In	Copy output of IO1 structure to FX output
;************************************************************
mFxOutLToModuleIO1In	macro
	move	x:(fxIOData+FXIO_OUTL),x0
	move	x0,x:(rD+IO1_IN)
	endm

;************************************************************
; mModuleIO1toFxOutR	Copy output of IO1 structure to FX output
;************************************************************
mModuleIO1toFxOutR	macro
	move	x:(rD+IO1_OUT),x0
	move	x0,x:(fxIOData+FXIO_OUTR)
	endm

;************************************************************
; mModuleIO1AddtoFxOutR	Add output of IO1 structure to FX output
;************************************************************
mModuleIO1AddtoFxOutR	macro
	move	x:(fxIOData+FXIO_OUTR),a
	move	x:(rD+IO1_OUT),x0
	add		x0,a
	move	a,x:(fxIOData+FXIO_OUTR)
	endm

;************************************************************
; mFxOutRToModuleIO1In	Copy output of IO1 structure to FX output
;************************************************************
mFxOutRToModuleIO1In	macro
	move	x:(fxIOData+FXIO_OUTR),x0
	move	x0,x:(rD+IO1_IN)
	endm

