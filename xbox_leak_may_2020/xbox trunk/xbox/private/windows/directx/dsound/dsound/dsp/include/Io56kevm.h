;*********************************************************************************

;

;     EQUATES for DSP56362 I/O registers and ports

;     Reference: DSP56362 Specifications Revision 0.2 

;

;     Last update: November 6, 1997

;

;*********************************************************************************



    page    132,55,0,0,0

    opt    mex



ioequ   ident   1,0

kRightReceive    equ     0 

kStackBase	equ $40
kStackTop	equ	kStackBase+32

;************************************************************
; mInitStack		
;************************************************************
mInitStack	macro

	ori     #$03,mr                ; mask interrupts

	movep   #$05000b,X:M_PCTL      ; DSP core at 8.192x12=98.3MHz
	move    #0,omr
	movec   #0,sp                  ; reset hardware stack pointer
	movep   #$000003,x:M_IPRP      ; ESAI int's enabled and top Priority
	move    #kStackBase,rStack            ; initialize stack pointer
	move    #-1,m6                 ; linear addressing

	andi    #$FC,mr                 ;enable all interrupt levels
	endm


;------------------------------------------------------------------------

;

;       EQUATES for I/O Port Programming

;

;------------------------------------------------------------------------



;       Register Addresses



M_HDR    EQU     $FFFFC9         ; Host port GPIO data Register

M_HDDR   EQU     $FFFFC8         ; Host port GPIO direction Register

M_PCRC   EQU     $FFFFBF         ; Port C Control Register

M_PRRC   EQU     $FFFFBE         ; Port C Direction Register

M_PDRC   EQU     $FFFFBD         ; Port C GPIO Data Register

M_PCRD   EQU     $FFFFD7         ; Port D Control register

M_PRRD   EQU     $FFFFD6         ; Port D Direction Data Register

M_PDRD   EQU     $FFFFD5         ; Port D GPIO Data Register

M_OGDB   EQU     $FFFFFC         ; OnCE GDB Register 







;------------------------------------------------------------------------

;

;       EQUATES for Exception Processing                                    

;

;------------------------------------------------------------------------



;       Register Addresses



M_IPRC   EQU     $FFFFFF         ; Interrupt Priority Register Core

M_IPRP   EQU     $FFFFFE         ; Interrupt Priority Register Peripheral



;       Interrupt Priority Register Core (IPRC) 



M_IAL    EQU     $7              ; IRQA Mode Mask

M_IAL0   EQU     0               ; IRQA Mode Interrupt Priority Level (low)

M_IAL1   EQU     1               ; IRQA Mode Interrupt Priority Level (high)

M_IAL2   EQU     2               ; IRQA Mode Trigger Mode

M_IBL    EQU     $38             ; IRQB Mode Mask

M_IBL0   EQU     3               ; IRQB Mode Interrupt Priority Level (low)

M_IBL1   EQU     4               ; IRQB Mode Interrupt Priority Level (high)

M_IBL2   EQU     5               ; IRQB Mode Trigger Mode

M_ICL    EQU     $1C0            ; IRQC Mode Mask

M_ICL0   EQU     6               ; IRQC Mode Interrupt Priority Level (low)

M_ICL1   EQU     7               ; IRQC Mode Interrupt Priority Level (high)

M_ICL2   EQU     8               ; IRQC Mode Trigger Mode

M_IDL    EQU     $E00            ; IRQD Mode Mask

M_IDL0   EQU     9               ; IRQD Mode Interrupt Priority Level (low)

M_IDL1   EQU     10              ; IRQD Mode Interrupt Priority Level (high)

M_IDL2   EQU     11              ; IRQD Mode Trigger Mode

M_D0L    EQU     $3000           ; DMA0 Interrupt priority Level Mask

M_D0L0   EQU     12              ; DMA0 Interrupt Priority Level (low)

M_D0L1   EQU     13              ; DMA0 Interrupt Priority Level (high)

M_D1L    EQU     $C000           ; DMA1 Interrupt Priority Level Mask

M_D1L0   EQU     14              ; DMA1 Interrupt Priority Level (low)

M_D1L1   EQU     15              ; DMA1 Interrupt Priority Level (high)

M_D2L    EQU     $30000          ; DMA2 Interrupt priority Level Mask

M_D2L0   EQU     16              ; DMA2 Interrupt Priority Level (low)

M_D2L1   EQU     17              ; DMA2 Interrupt Priority Level (high)

M_D3L    EQU     $C0000          ; DMA3 Interrupt Priority Level Mask

M_D3L0   EQU     18              ; DMA3 Interrupt Priority Level (low)

M_D3L1   EQU     19              ; DMA3 Interrupt Priority Level (high)

M_D4L    EQU     $300000         ; DMA4 Interrupt priority Level Mask

M_D4L0   EQU     20              ; DMA4 Interrupt Priority Level (low)

M_D4L1   EQU     21              ; DMA4 Interrupt Priority Level (high)

M_D5L    EQU     $C00000         ; DMA5 Interrupt priority Level Mask

M_D5L0   EQU     22              ; DMA5 Interrupt Priority Level (low)

M_D5L1   EQU     23              ; DMA5 Interrupt Priority Level (high)



;       Interrupt Priority Register Peripheral (IPRP)



M_ESL    EQU     $3              ; ESAI Interrupt Priority Level Mask

M_ESL0   EQU     0               ; ESAI Interrupt Priority Level (low)

M_ESL1   EQU     1               ; ESAI Interrupt Priority Level (high)

M_SHL    EQU     $C              ; SHI Interrupt Priority Level Mask

M_SHL0   EQU     2               ; SHI Interrupt Priority Level (low)

M_SHL1   EQU     3               ; SHI Interrupt Priority Level (high)

M_HDL    EQU     $30             ; HDI08 Interrupt Priority Level Mask

M_HDL0   EQU     4               ; HDI08 Interrupt Priority Level (low)

M_HDL1   EQU     5               ; HDI08 Interrupt Priority Level (high)

M_DAL    EQU     $C0             ; DAX Interrupt Priority Level  Mask       

M_DAL0   EQU     6               ; DAX Interrupt Priority Level  (low)

M_DAL1   EQU     7               ; DAX Interrupt Priority Level  (high)

M_TAL   EQU     $300             ; Timer Interrupt Priority Level Mask

M_TAL0   EQU     8               ; Timer Interrupt Priority Level (low)

M_TAL1   EQU     9               ; Timer Interrupt Priority Level (high)



;------------------------------------------------------------------------

;

;       EQUATES for Direct Memory Access (DMA)                                


;

;------------------------------------------------------------------------



;       Register Addresses Of DMA



M_DSTR   EQU     $FFFFF4         ; DMA Status Register

M_DOR0   EQU     $FFFFF3         ; DMA Offset Register 0

M_DOR1   EQU     $FFFFF2         ; DMA Offset Register 1

M_DOR2   EQU     $FFFFF1         ; DMA Offset Register 2

M_DOR3   EQU     $FFFFF0         ; DMA Offset Register 3



;       Register Addresses Of DMA0



M_DSR0   EQU     $FFFFEF         ; DMA0 Source Address Register

M_DDR0   EQU     $FFFFEE         ; DMA0 Destination Address Register 

M_DCO0   EQU     $FFFFED         ; DMA0 Counter

M_DCR0   EQU     $FFFFEC         ; DMA0 Control Register 



;       Register Addresses Of DMA1



M_DSR1   EQU     $FFFFEB         ; DMA1 Source Address Register

M_DDR1   EQU     $FFFFEA         ; DMA1 Destination Address Register 

M_DCO1   EQU     $FFFFE9         ; DMA1 Counter

M_DCR1   EQU     $FFFFE8         ; DMA1 Control Register



;       Register Addresses Of DMA2



M_DSR2   EQU     $FFFFE7         ; DMA2 Source Address Register

M_DDR2   EQU     $FFFFE6         ; DMA2 Destination Address Register 

M_DCO2   EQU     $FFFFE5         ; DMA2 Counter

M_DCR2   EQU     $FFFFE4         ; DMA2 Control Register

 

;       Register Addresses Of DMA3



M_DSR3   EQU     $FFFFE3         ; DMA3 Source Address Register

M_DDR3   EQU     $FFFFE2         ; DMA3 Destination Address Register 

M_DCO3   EQU     $FFFFE1         ; DMA3 Counter

M_DCR3   EQU     $FFFFE0         ; DMA3 Control Register



;       Register Addresses Of DMA4



M_DSR4   EQU     $FFFFDF         ; DMA4 Source Address Register

M_DDR4   EQU     $FFFFDE         ; DMA4 Destination Address Register 

M_DCO4   EQU     $FFFFDD         ; DMA4 Counter

M_DCR4   EQU     $FFFFDC         ; DMA4 Control Register 



;       Register Addresses Of DMA5



M_DSR5   EQU     $FFFFDB         ; DMA5 Source Address Register

M_DDR5   EQU     $FFFFDA         ; DMA5 Destination Address Register 

M_DCO5   EQU     $FFFFD9         ; DMA5 Counter

M_DCR5   EQU     $FFFFD8         ; DMA5 Control Register



;       DMA Control Register



M_DSS    EQU     $3              ; DMA Source Space Mask (DSS0-Dss1)

M_DSS0   EQU     0               ; DMA Source Memory space 0

M_DSS1   EQU     1               ; DMA Source Memory space 1    

M_DDS    EQU     $C              ; DMA Destination Space Mask (DDS-DDS1)

M_DDS0   EQU     2               ; DMA Destination Memory Space 0

M_DDS1   EQU     3               ; DMA Destination Memory Space 1

M_DAM    EQU     $3f0            ; DMA Address Mode Mask (DAM5-DAM0)

M_DAM0   EQU     4               ; DMA Address Mode 0

M_DAM1   EQU     5               ; DMA Address Mode 1

M_DAM2   EQU     6               ; DMA Address Mode 2

M_DAM3   EQU     7               ; DMA Address Mode 3

M_DAM4   EQU     8               ; DMA Address Mode 4

M_DAM5   EQU     9               ; DMA Address Mode 5

M_D3D    EQU     10              ; DMA Three Dimensional Mode

M_DRS    EQU     $F800           ; DMA Request Source Mask (DRS0-DRS4)

M_DRS0    EQU     11             ; DMA Request Source bit 0

M_DRS1    EQU     12             ; DMA Request Source bit 1

M_DRS2    EQU     13             ; DMA Request Source bit 2

M_DRS3    EQU     14             ; DMA Request Source bit 3

M_DRS4    EQU     15             ; DMA Request Source bit 4

M_DCON   EQU     16              ; DMA Continuous Mode

M_DPR    EQU     $60000          ; DMA Channel Priority

M_DPR0   EQU     17              ; DMA Channel Priority Level (low)

M_DPR1   EQU     18              ; DMA Channel Priority Level (high)

M_DTM    EQU     $380000         ; DMA Transfer Mode Mask (DTM2-DTM0)

M_DTM0   EQU     19              ; DMA Transfer Mode 0

M_DTM1   EQU     20              ; DMA Transfer Mode 1

M_DTM2   EQU     21              ; DMA Transfer Mode 2

M_DIE    EQU     22              ; DMA Interrupt Enable bit

M_DE     EQU     23              ; DMA Channel Enable bit 



;       DMA Status Register



M_DTD    EQU     $3F             ; Channel Transfer Done Status MASK (DTD0-DTD5)

M_DTD0   EQU     0               ; DMA Channel Transfer Done Status 0

M_DTD1   EQU     1               ; DMA Channel Transfer Done Status 1

M_DTD2   EQU     2               ; DMA Channel Transfer Done Status 2

M_DTD3   EQU     3               ; DMA Channel Transfer Done Status 3

M_DTD4   EQU     4               ; DMA Channel Transfer Done Status 4

M_DTD5   EQU     5               ; DMA Channel Transfer Done Status 5

M_DACT   EQU     8               ; DMA Active State

M_DCH    EQU     $E00            ; DMA Active Channel Mask (DCH0-DCH2)

M_DCH0   EQU     9               ; DMA Active Channel 0

M_DCH1   EQU     10              ; DMA Active Channel 1

M_DCH2   EQU     11              ; DMA Active Channel 2



;------------------------------------------------------------------------

;

;       EQUATES for Phase Locked Loop (PLL) 

;

;------------------------------------------------------------------------



;       Register Addresses Of PLL



M_PCTL   EQU     $FFFFFD         ; PLL Control Register



;       PLL Control Register



M_MF     EQU     $FFF            ; Multiplication Factor Bits Mask (MF0-MF11)

M_MF0    EQU   0                 ; Multiplication Factor bit 0 

M_MF1    EQU   1                 ; Multiplication Factor bit 1 

M_MF2    EQU   2                 ; Multiplication Factor bit 2 

M_MF3    EQU   3                 ; Multiplication Factor bit 3 

M_MF4    EQU   4                 ; Multiplication Factor bit 4

M_MF5    EQU   5                 ; Multiplication Factor bit 5 

M_MF6    EQU   6                 ; Multiplication Factor bit 6 

M_MF7    EQU   7                 ; Multiplication Factor bit 7 

M_MF8    EQU   8                 ; Multiplication Factor bit 8 

M_MF9    EQU   9                 ; Multiplication Factor bit 9 

M_MF10    EQU   10               ; Multiplication Factor bit 10

M_MF11    EQU   11               ; Multiplication Factor bit 11

M_DF     EQU     $7000           ; Division Factor Bits Mask (DF0-DF2)

M_DF0    EQU   12                ; Division Factor bit 0

M_DF1    EQU   13                ; Division Factor bit 1

M_DF2    EQU   14                ; Division Factor bit 2

M_XTLR   EQU     15              ; XTAL Range select bit

M_XTLD   EQU     16              ; XTAL Disable Bit

M_PSTP   EQU     17              ; STOP Processing State Bit 

M_PEN    EQU     18              ; PLL Enable Bit

M_COD   EQU     19               ; PLL Clock Output Disable Bit

M_PD     EQU     $F00000         ; PreDivider Factor Bits Mask (PD0-PD3)

M_PD0    EQU   20                ; PreDivider Factor bit 0

M_PD1    EQU   21                ; PreDivider Factor bit 1

M_PD2    EQU   22                ; PreDivider Factor bit 2

M_PD3    EQU   23                ; PreDivider Factor bit 3



;------------------------------------------------------------------------

;

;       EQUATES for BIU 

;

;------------------------------------------------------------------------



;       Register Addresses Of BIU



M_BCR    EQU     $FFFFFB         ; Bus Control Register

M_DCR    EQU     $FFFFFA         ; DRAM Control Register

M_AAR0   EQU     $FFFFF9         ; Address Attribute Register 0 

M_AAR1   EQU     $FFFFF8         ; Address Attribute Register 1 

M_AAR2   EQU     $FFFFF7         ; Address Attribute Register 2 

M_AAR3   EQU     $FFFFF6         ; Address Attribute Register 3 

M_IDR    EQU     $FFFFF5         ; ID Register



;       Bus Control Register



M_BA0W   EQU     $1F             ; Area 0 Wait Control Mask (BA0W0-BA0W4)

M_BA0W0   EQU   0                ; Area 0 Wait Control Bit 0

M_BA0W1   EQU   1                ; Area 0 Wait Control Bit 1

M_BA0W2   EQU   2                ; Area 0 Wait Control Bit 2

M_BA0W3   EQU   3                ; Area 0 Wait Control Bit 3

M_BA0W4   EQU   4                ; Area 0 Wait Control Bit 4

M_BA1W   EQU     $3E0            ; Area 1 Wait Control Mask (BA1W0-BA14)

M_BA1W0   EQU   5                ; Area 1 Wait Control Bit 0

M_BA1W1   EQU   6                ; Area 1 Wait Control Bit 1

M_BA1W2   EQU   7                ; Area 1 Wait Control Bit 2

M_BA1W3   EQU   8                ; Area 1 Wait Control Bit 3

M_BA1W4   EQU   9                ; Area 1 Wait Control Bit 4

M_BA2W   EQU     $1C00           ; Area 2 Wait Control Mask (BA2W0-BA2W2)

M_BA2W0   EQU   10               ; Area 2 Wait Control Bit 0

M_BA2W1   EQU   11               ; Area 2 Wait Control Bit 1

M_BA2W2   EQU   12               ; Area 2 Wait Control Bit 2

M_BA3W   EQU     $E000           ; Area 3 Wait Control Mask (BA3W0-BA3W3)

M_BA3W0   EQU   13               ; Area 3 Wait Control Bit 0

M_BA3W1   EQU   14               ; Area 3 Wait Control Bit 1

M_BA3W2   EQU   15               ; Area 3 Wait Control Bit 2

M_BDFW   EQU     $1F0000         ; Default Area Wait Control Mask (BDFW0-BDFW4)

M_BDFW0   EQU     16             ; Default Area Wait Control bit 0

M_BDFW1   EQU     17             ; Default Area Wait Control bit 1

M_BDFW2   EQU     18             ; Default Area Wait Control bit 2

M_BDFW3   EQU     19             ; Default Area Wait Control bit 3

M_BDFW4   EQU     20             ; Default Area Wait Control bit 4

M_BBS    EQU     21              ; Bus State

M_BLH    EQU     22              ; Bus Lock Hold

M_BRH    EQU     23              ; Bus Request Hold



;       DRAM Control Register



M_BCW    EQU     $3              ; In Page Wait States Bits Mask (BCW0-BCW1)

M_BCW0    EQU     0              ; In Page Wait States Bit 0

M_BCW1   EQU     1               ; In Page Wait States Bit 1

M_BRW    EQU     $C              ; Out Of Page Wait States Bits Mask (BRW0-BRW1)

M_BRW0    EQU     2              ; Out of Page Wait States bit 0

M_BRW1    EQU     3              ; Out of Page Wait States bit 1

M_BPS    EQU     $300            ; DRAM Page Size Bits Mask (BPS0-BPS1)

M_BPS0    EQU     4              ; DRAM Page Size Bits 0
                                 
M_BPS1    EQU     5              ; DRAM Page Size Bits 1

M_BPLE   EQU     11              ; Page Logic Enable

M_BME    EQU     12              ; Mastership Enable

M_BRE    EQU     13              ; Refresh Enable

M_BSTR   EQU     14              ; Software Triggered Refresh

M_BRF    EQU     $7F8000         ; Refresh Rate Bits Mask (BRF0-BRF7)

M_BRF0    EQU     15             ; Refresh Rate Bit 0

M_BRF1    EQU     16             ; Refresh Rate Bit 1

M_BRF2    EQU     17             ; Refresh Rate Bit 2

M_BRF3    EQU     18             ; Refresh Rate Bit 3

M_BRF4    EQU     19             ; Refresh Rate Bit 4

M_BRF5    EQU     20             ; Refresh Rate Bit 5

M_BRF6    EQU     21             ; Refresh Rate Bit 6

M_BRF7    EQU     22             ; Refresh Rate Bit 7

M_BRP    EQU     23              ; Refresh prescaler



;       Address Attribute Registers



M_BAT    EQU     $3              ; External Access Type and Pin Definition
                                 ; Bits Mask (BAT0-BAT1)

M_BAT0    EQU     0             ; External Access Type and Pin Definition
                                ; Bits 0

M_BAT1    EQU     1             ; External Access Type and Pin Definition
                                ; Bits 1

M_BAAP   EQU     2              ; Address Attribute Pin Polarity

M_BPEN   EQU     3              ; Program Space Enable

M_BXEN   EQU     4              ; X Data Space Enable

M_BYEN   EQU     5              ; Y Data Space Enable

M_BAM    EQU     6              ; Address Muxing

M_BPAC   EQU     7              ; Packing Enable

M_BNC    EQU     $F00           ; Number of Address Bits to Compare Mask
                                ; (BNC0-BNC3)

M_BNC0   EQU     8              ; Number of Address Bits to Compare 0

M_BNC1   EQU     9              ; Number of Address Bits to Compare 1

M_BNC2   EQU     10             ; Number of Address Bits to Compare 2

M_BNC3   EQU     11             ; Number of Address Bits to Compare 3

M_BAC    EQU     $FFF000        ; Address to Compare Bits Mask (BAC0-BAC11)

M_BAC0   EQU     12             ; Address to Compare Bits 0

M_BAC1   EQU     13             ; Address to Compare Bits 1

M_BAC2   EQU     14             ; Address to Compare Bits 2

M_BAC3   EQU     15             ; Address to Compare Bits 3

M_BAC4   EQU     16             ; Address to Compare Bits 4

M_BAC5   EQU     17             ; Address to Compare Bits 5

M_BAC6   EQU     18             ; Address to Compare Bits 6

M_BAC7   EQU     19             ; Address to Compare Bits 7

M_BAC8   EQU     20             ; Address to Compare Bits 8

M_BAC9   EQU     21             ; Address to Compare Bits 9

M_BAC10  EQU    22              ; Address to Compare Bits 10

M_BAC11  EQU    23              ; Address to Compare Bits 11



;       control and status bits in SR



M_C     EQU      0                ; Carry

M_V      EQU     1               ; Overflow      

M_Z      EQU     2               ; Zero

M_N      EQU     3               ; Negative      

M_U      EQU     4               ; Unnormalized

M_E      EQU     5               ; Extension     

M_L      EQU     6               ; Limit

M_S      EQU     7               ; Scaling Bit   

M_I0     EQU     8               ; Interupt Mask Bit 0

M_I1     EQU     9               ; Interupt Mask Bit 1

M_S0     EQU     10              ; Scaling Mode Bit 0

M_S1     EQU     11              ; Scaling Mode Bit 1

M_SC     EQU     13              ; Sixteen_Bit Compatibility

M_DM     EQU     14              ; Double Precision Multiply

M_LF     EQU     15              ; DO-Loop Flag

M_FV     EQU     16              ; DO-Forever Flag

M_SA     EQU     17              ; Sixteen-Bit Arithmetic

M_CE     EQU     19              ; Instruction Cache Enable

M_SM     EQU     20              ; Arithmetic Saturation

M_RM     EQU     21              ; Rounding Mode

M_CP     EQU     $c00000         ; mask for CORE-DMA priority bits in SR 

M_CP0    EQU     22              ; bit 0 of priority bits in SR

M_CP1    EQU     23              ; bit 1 of priority bits in SR



;       control and status bits in OMR



M_MA     EQU     0               ; Operating Mode A

M_MB     EQU     1               ; Operating Mode B

M_MC     EQU     2               ; Operating Mode C

M_MD     EQU     3               ; Operating Mode D

M_EBD    EQU     4               ; External Bus Disable bit in OMR

M_SD     EQU     6               ; Stop Delay 

M_MS     EQU     7               ; Memory Switch Mode 

M_CDP    EQU     $300            ; mask for CORE-DMA priority bits in OMR

M_CDP0   EQU     8               ; bit 0 of priority bits in OMR Core DMA

M_CDP1   EQU     9               ; bit 1 of priority bits in OMR Core DMA

M_BE    EQU     10               ; Burst Enable 

M_TAS    EQU     11              ; TA Synchronize Select

M_BRT    EQU     12              ; Bus Release Timing 

M_ABE   EQU     13               ; Async. Bus Arbitration Enable

M_APD   EQU     14               ; Addess Priority Disable

M_ATE   EQU     15               ; Address Tracing Enable

M_XYS    EQU     16              ; Stack Extension space select bit in OMR.

M_EUN    EQU     17              ; Extensed stack UNderflow flag in OMR.

M_EOV    EQU     18              ; Extended stack OVerflow flag in OMR.

M_WRP    EQU     19              ; Extended WRaP flag in OMR.

M_SEN    EQU     20              ; Stack Extension Enable bit in OMR.

M_PAEN   EQU      23             ; Patch Enable



;------------------------------------------------------------------------

;

;       EQUATES for DAX (SPDIF Tx)

;

;------------------------------------------------------------------------



;       Register Addresses

 

M_XSTR   EQU     $FFFFD4         ; DAX Status Register (XSTR)

M_XADRB  EQU     $FFFFD3         ; DAX Audio Data Register B (XADRB)

M_XADR    EQU     $FFFFD2        ; DAX Audio Data Register (XADR)

M_XADRA  EQU     $FFFFD2         ; DAX Audio Data Register A (XADRA)

M_XNADR  EQU     $FFFFD1         ; DAX Non-Audio Data Register (XNADR)

M_XCTR   EQU     $FFFFD0         ; DAX Control Register (XCTR)



;       status bits in XSTR



M_XADE   EQU     0               ; DAX Audio Data Register Empty (XADE)

M_XAUR   EQU     1               ; DAX Trasmit Underrun Error Flag (XAUR)

M_XBLK   EQU     2               ; DAX Block Transferred (XBLK)



;       non-audio bits in XNADR



M_XVA    EQU     10              ; DAX Channel A Validity (XVA)

M_XUA    EQU     11              ; DAX Channel A User Data (XUA)

M_XCA    EQU     12              ; DAX Channel A Channel Status (XCA)

M_XVB    EQU     13              ; DAX Channel B Validity (XVB)

M_XUB    EQU     14              ; DAX Channel B User Data (XUB)

M_XCB    EQU     15              ; DAX Channel B Channel Status (XCB)



;       control bits in XCTR



M_XDIE   EQU     0               ; DAX Audio Data Register Empty Interrupt
                                 ; Enable (XDIE)

M_XUIE   EQU     1               ; DAX Underrun Error Interrupt Enable (XUIE)

M_XBIE   EQU     2               ; DAX Block Transferred Interrupt Enable
                                 ; (XBIE)

M_XCS0   EQU     3               ; DAX Clock Input Select 0 (XCS0)

M_XCS1   EQU     4               ; DAX Clock Input Select 1 (XCS1)

M_XSB    EQU     5               ; DAX Start Block (XSB)



;------------------------------------------------------------------------

;

;       EQUATES for SHI 

;

;------------------------------------------------------------------------



;       Register Addresses

 

M_HRX    EQU     $FFFF94         ; SHI Receive FIFO (HRX)

M_HTX    EQU     $FFFF93         ; SHI Transmit Register (HTX)

M_HSAR   EQU     $FFFF92         ; SHI I2C Slave Address Register (HSAR)

M_HCSR   EQU     $FFFF91         ; SHI Control/Status Register (HCSR)

M_HCKR   EQU     $FFFF90         ; SHI Clock Control Register (HCKR)



;       HSAR bits



M_HA6    EQU     23              ; SHI I2C Slave Address (HA6)

M_HA5    EQU     22              ; SHI I2C Slave Address (HA5)

M_HA4    EQU     21              ; SHI I2C Slave Address (HA4)

M_HA3    EQU     20              ; SHI I2C Slave Address (HA3)

M_HA1    EQU     18              ; SHI I2C Slave Address (HA1)



;      control and status bits in HCSR



M_HBUSY  EQU     22              ; SHI Host Busy (HBUSY)

M_HBER   EQU     21              ; SHI Bus Error (HBER)

M_HROE   EQU     20              ; SHI Receive Overrun Error (HROE)

M_HRFF   EQU     19              ; SHI Receivr FIFO Full (HRFF)

M_HRNE   EQU     17              ; SHI Receive FIFO Not Empty (HRNE)

M_HTDE   EQU     15              ; SHI Host Transmit data Empty (HTDE)

M_HTUE   EQU     14              ; SHI Host Transmit Underrun Error (HTUE)

M_HRIE1  EQU     13              ; SHI Receive Interrupt Enable (HRIE1)

M_HRIE0  EQU     12              ; SHI Receive Interrupt Enable (HRIE0)

M_HTIE   EQU     11              ; SHI Transmit Interrupt Enable (HTIE)

M_HBIE   EQU     10              ; SHI Bus-Error Interrupt Enable (HBIE)

M_HIDLE  EQU     9               ; SHI Idle (HIDLE)

M_HRQE1  EQU     8               ; SHI Host Request Enable (HRQE1)

M_HRQE0  EQU     7               ; SHI Host Request Enable (HRQE0)

M_HMST   EQU     6               ; SHI Master Mode (HMST)

M_HFIFO  EQU     5               ; SHI FIFO Enable Control (HFIFO)

M_HCKFR  EQU     4               ; SHI Clock Freeze (HCKFR)

M_HM1    EQU     3               ; SHI Serial Host Interface Mode (HM1)

M_HM0    EQU     2               ; SHI Serial Host Interface Mode (HM0)

M_HI2C   EQU     1               ; SHI I2c/SPI Selection (HI2C)

M_HEN    EQU     0               ; SHI Host Enable (HEN)



;       control bits in HCKR



M_HFM1   EQU     13              ; SHI Filter Model (HFM1)

M_HFM0   EQU     12              ; SHI Filter Model (HFM0)

M_HDM7   EQU     10              ; SHI Divider Modulus Select (HDM7)

M_HDM6   EQU     9               ; SHI Divider Modulus Select (HDM6)

M_HDM5   EQU     8               ; SHI Divider Modulus Select (HDM5)

M_HDM4   EQU     7               ; SHI Divider Modulus Select (HDM4)

M_HDM3   EQU     6               ; SHI Divider Modulus Select (HDM3)

M_HDM2   EQU     5               ; SHI Divider Modulus Select (HDM2)

M_HDM1   EQU     4               ; SHI Divider Modulus Select (HDM1)

M_HDM0   EQU     3               ; SHI Divider Modulus Select (HDM0)

M_HRS    EQU     2               ; SHI Prescalar Rate Select (HRS)

M_CPOL   EQU     1               ; SHI Clock Polarity (CPOL)

M_CPHA   EQU     0               ; SHI Clock Phase (CPHA)



;------------------------------------------------------------------------

;

;       EQUATES for ESAI 

;

;------------------------------------------------------------------------



;       Register Addresses

 

M_RSMB   EQU     $FFFFBC         ; ESAI Receive Slot Mask Register B (RSMB)

M_RSMA   EQU     $FFFFBB         ; ESAI Receive Slot Mask Register A (RSMA)

M_TSMB   EQU     $FFFFBA         ; ESAI Transmit Slot Mask Register B (TSMB)

M_TSMA   EQU     $FFFFB9         ; ESAI Transmit Slot Mask Register A (TSMA)

M_RCCR   EQU     $FFFFB8         ; ESAI Receive Clock Control Register (RCCR)

M_RCR    EQU     $FFFFB7         ; ESAI Receive Control Register (RCR)

M_TCCR   EQU     $FFFFB6         ; ESAI Transmit Clock Control Register (TCCR)

M_TCR    EQU     $FFFFB5         ; ESAI Transmit Control Register (TCR)

M_SAICR  EQU     $FFFFB4         ; ESAI Control Register (SAICR)

M_SAISR  EQU     $FFFFB3         ; ESAI Status Register (SAISR)

M_RX3    EQU     $FFFFAB         ; ESAI Receive Data Register 3 (RX3)

M_RX2    EQU     $FFFFAA         ; ESAI Receive Data Register 2 (RX2)

M_RX1    EQU     $FFFFA9         ; ESAI Receive Data Register 1 (RX1)

M_RX0    EQU     $FFFFA8         ; ESAI Receive Data Register 0 (RX0)

M_TSR    EQU     $FFFFA6         ; ESAI Time Slot Register (TSR)

M_TX5    EQU     $FFFFA5         ; ESAI Transmit Data Register 5 (TX5)

M_TX4    EQU     $FFFFA4         ; ESAI Transmit Data Register 4 (TX4)

M_TX3    EQU     $FFFFA3         ; ESAI Transmit Data Register 3 (TX3)

M_TX2    EQU     $FFFFA2         ; ESAI Transmit Data Register 2 (TX2)

M_TX1    EQU     $FFFFA1         ; ESAI Transmit Data Register 1 (TX1)

M_TX0    EQU     $FFFFA0         ; ESAI Transmit Data Register 0 (TX0)



;       RSMB Register bits



M_RS31   EQU     15              ; ESAI 

M_RS30   EQU     14              ; ESAI 

M_RS29   EQU     13              ; ESAI 

M_RS28   EQU     12              ; ESAI 

M_RS27   EQU     11              ; ESAI 

M_RS26   EQU     10              ; ESAI 

M_RS25   EQU     9               ; ESAI 

M_RS24   EQU     8               ; ESAI 

M_RS23   EQU     7               ; ESAI 

M_RS22   EQU     6               ; ESAI 

M_RS21   EQU     5               ; ESAI 

M_RS20   EQU     4               ; ESAI 

M_RS19   EQU     3               ; ESAI 

M_RS18   EQU     2               ; ESAI 

M_RS17   EQU     1               ; ESAI 

M_RS16   EQU     0               ; ESAI 



;       RSMA Register bits



M_RS15   EQU     15              ; ESAI 

M_RS14   EQU     14              ; ESAI 

M_RS13   EQU     13              ; ESAI 

M_RS12   EQU     12              ; ESAI 

M_RS11   EQU     11              ; ESAI 

M_RS10   EQU     10              ; ESAI 

M_RS9    EQU     9               ; ESAI 

M_RS8    EQU     8               ; ESAI 

M_RS7    EQU     7               ; ESAI 

M_RS6    EQU     6               ; ESAI 

M_RS5    EQU     5               ; ESAI 

M_RS4    EQU     4               ; ESAI 

M_RS3    EQU     3               ; ESAI 

M_RS2    EQU     2               ; ESAI 

M_RS1    EQU     1               ; ESAI 

M_RS0    EQU     0               ; ESAI 



;       TSMB Register bits



M_TS31   EQU     15              ; ESAI 

M_TS30   EQU     14              ; ESAI 

M_TS29   EQU     13              ; ESAI 

M_TS28   EQU     12              ; ESAI 

M_TS27   EQU     11              ; ESAI 

M_TS26   EQU     10              ; ESAI 

M_TS25   EQU     9               ; ESAI 

M_TS24   EQU     8               ; ESAI 

M_TS23   EQU     7               ; ESAI 

M_TS22   EQU     6               ; ESAI 

M_TS21   EQU     5               ; ESAI 

M_TS20   EQU     4               ; ESAI 

M_TS19   EQU     3               ; ESAI 

M_TS18   EQU     2               ; ESAI 

M_TS17   EQU     1               ; ESAI 

M_TS16   EQU     0               ; ESAI 



;       TSMA Register bits



M_TS15   EQU     15              ; ESAI 

M_TS14   EQU     14              ; ESAI 

M_TS13   EQU     13              ; ESAI 

M_TS12   EQU     12              ; ESAI 

M_TS11   EQU     11              ; ESAI 

M_TS10   EQU     10              ; ESAI 

M_TS9    EQU     9               ; ESAI 

M_TS8    EQU     8               ; ESAI 

M_TS7    EQU     7               ; ESAI 

M_TS6    EQU     6               ; ESAI 

M_TS5    EQU     5               ; ESAI 

M_TS4    EQU     4               ; ESAI 

M_TS3    EQU     3               ; ESAI 

M_TS2    EQU     2               ; ESAI 

M_TS1    EQU     1               ; ESAI 

M_TS0    EQU     0               ; ESAI 



;       RCCR Register bits



M_RHCKD  EQU     23              ; ESAI 

M_RFSD   EQU     22              ; ESAI 

M_RCKD   EQU     21              ; ESAI 

M_RHCKP   EQU     20             ; ESAI

M_RFSP   EQU     19              ; ESAI

M_RCKP   EQU     18              ; ESAI 

M_RFP     EQU      $3C000        ; ESAI MASK

M_RFP3   EQU     17              ; ESAI 

M_RFP2   EQU     16              ; ESAI 

M_RFP1   EQU     15              ; ESAI 

M_RFP0   EQU     14              ; ESAI

M_RDC     EQU     $3E00          ; ESAI MASK 

M_RDC4   EQU     13              ; ESAI 

M_RDC3   EQU     12              ; ESAI 

M_RDC2   EQU     11              ; ESAI 

M_RDC1   EQU     10              ; ESAI 

M_RDC0   EQU     9               ; ESAI 

M_RPSR   EQU     8               ; ESAI 

M_RPM     EQU    $FF

M_RPM7   EQU     7               ; ESAI 

M_RPM6   EQU     6               ; ESAI 

M_RPM5   EQU     5               ; ESAI 

M_RPM4   EQU     4               ; ESAI 

M_RPM3   EQU     3               ; ESAI 

M_RPM2   EQU     2               ; ESAI 

M_RPM1   EQU     1               ; ESAI 

M_RPM0   EQU     0               ; ESAI 



;       RCR Register bits



M_RLIE   EQU     23              ; ESAI

M_RIE    EQU     22              ; ESAI

M_REDIE  EQU     21              ; ESAI

M_REIE   EQU     20              ; ESAI

M_RFSR   EQU     16              ; ESAI

M_RFSL   EQU     15              ; ESAI

M_RSWS    EQU       $7C00        ; ESAI MASK

M_RSWS4   EQU     14             ; ESAI

M_RSWS3   EQU     13             ; ESAI

M_RSWS2   EQU     12             ; ESAI

M_RSWS1   EQU     11             ; ESAI 

M_RSWS0   EQU     10             ; ESAI 

M_RMOD      EQU       $300

M_RMOD1  EQU     9               ; ESAI

M_RMOD0  EQU     8               ; ESAI

M_RWA   EQU     7                ; ESAI

M_RSHFD  EQU     6               ; ESAI

M_RE      EQU      $F

M_RE3    EQU     3               ; ESAI

M_RE2    EQU     2               ; ESAI

M_RE1    EQU     1               ; ESAI

M_RE0    EQU     0               ; ESAI



;       TCCR Register bits



M_THCKD  EQU     23              ; ESAI 

M_TFSD   EQU     22              ; ESAI 

M_TCKD   EQU     21              ; ESAI 

M_THCKP   EQU     20             ; ESAI 

M_TFSP   EQU     19              ; ESAI 

M_TCKP   EQU     18              ; ESAI

M_TFP    EQU      $3C000

M_TFP3   EQU     17              ; ESAI 

M_TFP2   EQU     16              ; ESAI 

M_TFP1   EQU     15              ; ESAI 

M_TFP0   EQU     14              ; ESAI

M_TDC     EQU     $3E00       

M_TDC4   EQU     13              ; ESAI 

M_TDC3   EQU     12              ; ESAI 

M_TDC2   EQU     11              ; ESAI 

M_TDC1   EQU     10              ; ESAI 

M_TDC0   EQU     9               ; ESAI 

M_TPSR   EQU     8               ; ESAI

M_TPM     EQU    $FF             

M_TPM7   EQU     7               ; ESAI 

M_TPM6   EQU     6               ; ESAI 

M_TPM5   EQU     5               ; ESAI 

M_TPM4   EQU     4               ; ESAI 

M_TPM3   EQU     3               ; ESAI 

M_TPM2   EQU     2               ; ESAI 

M_TPM1   EQU     1               ; ESAI 

M_TPM0   EQU     0               ; ESAI 



;       TCR Register bits



M_TLIE   EQU     23              ; ESAI

M_TIE    EQU     22              ; ESAI

M_TEDIE  EQU     21              ; ESAI

M_TEIE   EQU     20              ; ESAI

M_TFSR   EQU     16              ; ESAI

M_TFSL   EQU     15              ; ESAI

M_TSWS   EQU    $7C00

M_TSWS4   EQU     14             ; ESAI

M_TSWS3   EQU     13             ; ESAI

M_TSWS2   EQU     12             ; ESAI

M_TSWS1   EQU     11             ; ESAI

M_TSWS0   EQU     10             ; ESAI

M_TMOD    EQU      $300

M_TMOD1  EQU     9               ; ESAI

M_TMOD0  EQU     8               ; ESAI

M_TWA   EQU     7                ; ESAI

M_TSHFD  EQU     6               ; ESAI

M_TEM       EQU     $3F

M_TE5    EQU     5               ; ESAI

M_TE4    EQU     4               ; ESAI

M_TE3    EQU     3               ; ESAI

M_TE2    EQU     2               ; ESAI

M_TE1    EQU     1               ; ESAI

M_TE0    EQU     0               ; ESAI



;       control bits of SAICR



M_ALC   EQU   8                  ;ESAI

M_TEBE   EQU     7               ; ESAI 

M_SYN    EQU     6               ; ESAI 

M_OF2    EQU     2               ; ESAI 

M_OF1    EQU     1               ; ESAI 

M_OF0    EQU     0               ; ESAI 



;       status bits of SAISR



M_TODE   EQU     17              ; ESAI

M_TEDE   EQU     16              ; ESAI

M_TDE    EQU     15              ; ESAI

M_TUE    EQU     14              ; ESAI

M_TFS    EQU     13              ; ESAI

M_RODF   EQU     10              ; ESAI

M_REDF   EQU     9               ; ESAI

M_RDF    EQU     8               ; ESAI

M_ROE    EQU     7               ; ESAI

M_RFS    EQU     6               ; ESAI

M_IF2    EQU     2               ; ESAI

M_IF1    EQU     1               ; ESAI

M_IF0    EQU     0               ; ESAI



;------------------------------------------------------------------------

;

;       EQUATES for HDI08 

;

;------------------------------------------------------------------------



;       Register Addresses

 

M_HOTX    EQU     $FFFFC7        ; HOST Transmit Register (HOTX)

M_HORX    EQU     $FFFFC6        ; HOST Receive Register (HORX)

M_HBAR   EQU     $FFFFC5         ; HOST Base Address Register (HBAR)

M_HPCR   EQU     $FFFFC4         ; HOST Polarity Control Register (HPCR)

M_HSR    EQU     $FFFFC3         ; HOST Status Register (HSR)

M_HCR    EQU     $FFFFC2         ; HOST Control Register (HCR)





;       HCR bits



M_HRIE   EQU     $0              ; HOST Receive interrupts Enable

M_HOTIE   EQU     $1             ; HOST Transmit Interrupt Enable

M_HCIE   EQU     $2              ; HOST Command Interrupt Enable

M_HF2    EQU     $3              ; HOST Flag 2

M_HF3    EQU     $4              ; HOST Flag 3

M_HODM0   EQU     $5             ; HOST DMA Mode Control Bit 0

M_HODM1   EQU     $6             ; HOST DMA Mode Control Bit 1

M_HODM2   EQU     $7             ; HOST DMA Mode Control Bit 2

        

;       HSR bits



M_HRDF   EQU     $0              ; HOST Receive Data Full

M_HOTDE   EQU     $1             ; HOST Receive Data Emptiy

M_HCP    EQU     $2              ; HOST Command Pending

M_HF0    EQU     $3              ; HOST Flag 0

M_HF1    EQU     $4              ; HOST Flag 1

M_DMA    EQU     $7              ; HOST DMA Status

        

;       HPCR bits



M_HGEN   EQU     $0              ; HOST Port Enable

M_HA8EN  EQU     $1              ; HOST Address 8 Enable

M_HA9EN  EQU     $2              ; HOST Address 9 Enable

M_HCSEN  EQU     $3              ; HOST Chip Select Enable

M_HREN   EQU     $4              ; HOST Request Enable

M_HAEN   EQU     $5              ; HOST Acknowledge Enable

M_HOEN    EQU     $6             ; HOST Enable

M_HROD    EQU     $8             ; HOST Request Open Dranin mode

M_HDSP   EQU     $9              ; HOST Data Strobe Polarity

M_HASP   EQU     $a              ; HOST Address Strobe Polarity

M_HMUX   EQU     $b              ; HOST Multiplexed bus select

M_HDDS  EQU     $c               ; HOST Double/Single Strobe select

M_HCSP   EQU     $d              ; HOST Chip Select Polarity

M_HRP    EQU     $e              ; HOST Request PolarityPolarity

M_HAP    EQU     $f              ; HOST Acknowledge Polarity



;                  HBAR  BITS



M_BA   EQU   $FF

M_BA10   EQU   7

M_BA9   EQU   6

M_BA8   EQU   5

M_BA7   EQU   4

M_BA6   EQU   3

M_BA5   EQU   2

M_BA4   EQU   1

M_BA3   EQU   0



;-----------------------------------------------------------------------

;

;       EQUATES for TIMER 

;

;------------------------------------------------------------------------



;       Register Addresses Of TIMER0



M_TCSR0  EQU     $FFFF8F         ; TIMER0 Control/Status Register        

M_TLR0   EQU     $FFFF8E         ; TIMER0 Load Reg   

M_TCPR0  EQU     $FFFF8D         ; TIMER0 Compare Register

M_TCR0   EQU     $FFFF8C         ; TIMER0 Count Register



;       Register Addresses Of TIMER1



M_TCSR1  EQU     $FFFF8B         ; TIMER1 Control/Status Register        

M_TLR1   EQU     $FFFF8A         ; TIMER1 Load Reg   

M_TCPR1  EQU     $FFFF89         ; TIMER1 Compare Register

M_TCR1   EQU     $FFFF88         ; TIMER1 Count Register





;       Register Addresses Of TIMER2



M_TCSR2  EQU     $FFFF87         ; TIMER2 Control/Status Register        

M_TLR2   EQU     $FFFF86         ; TIMER2 Load Reg   

M_TCPR2  EQU     $FFFF85         ; TIMER2 Compare Register

M_TCR2   EQU     $FFFF84         ; TIMER2 Count Register

M_TPLR   EQU     $FFFF83         ; TIMER Prescaler Load Register

M_TPCR   EQU     $FFFF82         ; TIMER Prescalar Count Register



;       Timer Control/Status Register Bit Flags        



M_TE     EQU     0               ; Timer Enable 

M_TOIE   EQU     1               ; Timer Overflow Interrupt Enable

M_TCIE   EQU     2               ; Timer Compare Interrupt Enable

M_TC     EQU     $F0             ; Timer Control Mask (TC0-TC3)

M_INV    EQU     8               ; Inverter Bit

M_TRM    EQU     9               ; Timer Restart Mode 

M_DIR    EQU     11              ; Direction Bit

M_DI     EQU     12              ; Data Input

M_DO     EQU     13              ; Data Output

M_PCE    EQU     15              ; Prescaled Clock Enable

M_TOF    EQU     20              ; Timer Overflow Flag

M_TCF    EQU     21              ; Timer Compare Flag 



;       Timer Prescaler Register Bit Flags        



M_PS    EQU  $600000             ; Prescaler Source Mask

M_PS0   EQU  21

M_PS1   EQU  22



;   Timer Control Bits

M_TC0    EQU     4               ; Timer Control 0

M_TC1    EQU     5               ; Timer Control 1

M_TC2    EQU     6               ; Timer Control 2

M_TC3    EQU     7               ; Timer Control 3


; nVidia timer registers

TIMER1_CONFIG               equ $FFFFB0
TIMER2_CONFIG               equ $FFFFB4

TIMER1_CONTROL              equ $FFFFB1
TIMER2_CONTROL              equ $FFFFB5

TIMER1_TERMINAL_COUNT       equ $ffffB2
TIMER2_TERMINAL_COUNT       equ $ffffB6

TIMER1_COUNT                equ $ffffB3
TIMER2_COUNT                equ $ffffB7

