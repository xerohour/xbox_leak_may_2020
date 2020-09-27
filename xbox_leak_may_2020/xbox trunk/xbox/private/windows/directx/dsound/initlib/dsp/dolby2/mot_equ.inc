; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			mot_equ.inc
;												
;   Contents:		EQUATES for 56301 registers and ports
;					Reference: 56301 Specifications Revision 3.00
;
;	Author:			jmw
;											
;	Revision:		1.00
;											
;	History:
;
;		Rev 1.00	9/19/2000 8:49:01 PM		jmw
;	Copied.
;
;
; ******************************************************************

	page	132,55,0,0,0
	opt	mex

ioequ   ident   1,0

;------------------------------------------------------------------------
;
;       EQUATES for I/O Port Programming
;
;------------------------------------------------------------------------

;       Register Addresses

M_DATH   EQU     $FFFFCF         ; Port B (host) GPIO data Register
M_DIRH   EQU     $FFFFCE         ; Port B (host) GPIO direction Register

M_PCRC   EQU     $FFFFBF         ; Port C (ESSI0) Control Register
M_PRRC   EQU     $FFFFBE         ; Port C (ESSI0) Direction Register
M_PDRC   EQU     $FFFFBD         ; Port C (ESSI0) GPIO Data Register

M_PCRD   EQU     $FFFFAF         ; Port D (ESSI1) Control register
M_PRRD   EQU     $FFFFAE         ; Port D (ESSI1) Direction Data Register
M_PDRD   EQU     $FFFFAD         ; Port D (ESSI1) GPIO Data Register

M_PCRE   EQU     $FFFF9F         ; Port E (SCI) Control register
M_PRRE   EQU     $FFFF9E         ; Port E (SCI) Direction Register
M_PDRE   EQU     $FFFF9D         ; Port E (SCI) Data Register

M_OGDB   EQU     $FFFFFC         ; OnCE GDB Register 

;------------------------------------------------------------------------
;
;       EQUATES for Host Interface
;
;------------------------------------------------------------------------

;       Register Addresses
 
M_DTXS   EQU     $FFFFCD         ; DSP SLAVE TRANSMIT DATA FIFO (DTXS)
M_DTXM   EQU     $FFFFCC         ; DSP MASTER TRANSMIT DATA FIFO (DTXM)
M_DRXR   EQU     $FFFFCB         ; DSP RECEIVE DATA FIFO (DRXR)
M_DPSR   EQU     $FFFFCA         ; DSP PCI STATUS REGISTER (DPSR)
M_DSR    EQU     $FFFFC9         ; DSP STATUS REGISTER (DSR)
M_DPAR   EQU     $FFFFC8         ; DSP PCI ADDRESS REGISTER (DPAR)
M_DPMC   EQU     $FFFFC7         ; DSP PCI MASTER CONTROL REGISTER (DPMC)
M_DPCR   EQU     $FFFFC6         ; DSP PCI CONTROL REGISTER (DPCR)
M_DCTR   EQU     $FFFFC5         ; DSP CONTROL REGISTER (DCTR)    
 
;       Host Control Register Bit Flags

M_HCIE   EQU     0               ; Host Command Interrupt Enable
M_STIE   EQU     1               ; Slave Transmit Interrupt Enable
M_SRIE   EQU     2               ; Slave Receive Interrupt Enable
M_HF35   EQU     $38             ; Host Flags 5-3 Mask
M_HF3    EQU     3               ; Host Flag 3
M_HF4    EQU     4               ; Host Flag 4
M_HF5    EQU     5               ; Host Flag 5
M_HINT   EQU     6               ; Host Interrupt A
M_HDSM   EQU     13              ; Host Data Strobe Mode
M_HRWP   EQU     14              ; Host RD/WR Polarity
M_HTAP   EQU     15              ; Host Transfer Acknowledge Polarity
M_HDRP   EQU     16              ; Host Dma Request Polarity
M_HRSP   EQU     17              ; Host Reset Polarity
M_HIRP   EQU     18              ; Host Interrupt  Request Polarity
M_HIRC   EQU     19              ; Host Interupt Request Control
M_HM0    EQU     20              ; Host Interface Mode
M_HM1    EQU     21              ; Host Interface Mode
M_HM2    EQU     22              ; Host Interface Mode
M_HM     EQU     $700000         ; Host Interface Mode Mask

;       Host PCI Control Register Bit Flags

M_PMTIE  EQU     1               ; PCI Master Transmit         Interrupt Enable
M_PMRIE  EQU     2               ; PCI Master Receive          Interrupt Enable
M_PMAIE  EQU     4               ; PCI Master Address          Interrupt Enable
M_PPEIE  EQU     5               ; PCI Parity Error            Interrupt Enable
M_PTAIE  EQU     7               ; PCI Transaction Abort       Interrupt Enable
M_PTTIE  EQU     9               ; PCI Transaction Termination Interrupt Enable
M_PTCIE  EQU     12              ; PCI Transfer Complete       Interrupt Enable
M_CLRT   EQU     14              ; Clear Transmitter
M_MTT    EQU     15              ; Master Transfer Terminate
M_SERF   EQU     16              ; HSERR~ Force
M_MACE   EQU     18              ; Master Access Counter Enable
M_MWSD   EQU     19              ; Master Wait States Disable
M_RBLE   EQU     20              ; Receive Buffer Lock Enable
M_IAE    EQU     21              ; Insert Address Enable 

;       Host PCI Master Control Register Bit Flags

M_ARH    EQU     $00ffff         ; DSP PCI Transaction Address (High)
M_BL     EQU     $3f0000         ; PCI Data Burst Length
M_FC     EQU     $c00000         ; Data Transfer Format Control

;       Host PCI Address Register Bit Flags

M_ARL    EQU     $00ffff         ; DSP PCI Transaction Address (Low)
M_C      EQU     $0f0000         ; PCI Bus Command
M_BE     EQU     $f00000         ; PCI Byte Enables

;       DSP Status Register Bit Flags
 
M_HCP    EQU     0               ; Host Command pending
M_STRQ   EQU     1               ; Slave Transmit Data Request
M_SRRQ   EQU     2               ; Slave Receive Data Request
M_HF02   EQU     $38             ; Host Flag 0-2 Mask
M_HF0    EQU     3               ; Host Flag 0
M_HF1    EQU     4               ; Host Flag 1
M_HF2    EQU     5               ; Host Flag 2

;       DSP PCI Status Register Bit Flags

M_MWS    EQU    0               ; PCI Master Wait States
M_MTRQ   EQU    1               ; PCI Master Transmit Data Request
M_MRRQ   EQU    2               ; PCI Master Receive Data Request
M_MARQ   EQU    4               ; PCI Master Address Request
M_APER   EQU    5               ; PCI Address Parity Error
M_DPER   EQU    6               ; PCI Data Parity Error
M_MAB    EQU    7               ; PCI Master Abort
M_TAB    EQU    8               ; PCI Target Abort
M_TDIS   EQU    9               ; PCI Target Disconnect
M_TRTY   EQU    10              ; PCI Target Retry
M_TO     EQU    11              ; PCI Time Out Termination
M_RDC    EQU    $3F0000         ; Remaining Data Count Mask (RDC5-RDC0)
M_RDC0   EQU    16              ; Remaining Data Count  0
M_RDC1   EQU    17              ; Remaining Data Count  1
M_RDC2   EQU    18              ; Remaining Data Count  2
M_RDC3   EQU    19              ; Remaining Data Count  3
M_RDC4   EQU    20              ; Remaining Data Count  4
M_RDC5   EQU    21              ; Remaining Data Count  5
M_HACT   EQU    23              ; Hi32 Active


;------------------------------------------------------------------------
;
;       EQUATES for Serial Communications Interface (SCI)
;
;------------------------------------------------------------------------

;       Register Addresses

M_STXH   EQU     $FFFF97         ; SCI Transmit Data Register (high)
M_STXM   EQU     $FFFF96         ; SCI Transmit Data Register (middle)
M_STXL   EQU     $FFFF95         ; SCI Transmit Data Register (low)
M_SRXH   EQU     $FFFF9A         ; SCI Receive Data Register (high)
M_SRXM   EQU     $FFFF99         ; SCI Receive Data Register (middle)
M_SRXL   EQU     $FFFF98         ; SCI Receive Data Register (low)
M_STXA   EQU     $FFFF94         ; SCI Transmit Address Register
M_SCR    EQU     $FFFF9C         ; SCI Control Register
M_SSR    EQU     $FFFF93         ; SCI Status Register
M_SCCR   EQU     $FFFF9B         ; SCI Clock Control Register

;       SCI Control Register Bit Flags

M_WDS    EQU     $7              ; Word Select Mask (WDS0-WDS3)
M_WDS0   EQU     0               ; Word Select 0
M_WDS1   EQU     1               ; Word Select 1
M_WDS2   EQU     2               ; Word Select 2
M_SSFTD  EQU     3               ; SCI Shift Direction 
M_SBK    EQU     4               ; Send Break
M_WAKE   EQU     5               ; Wakeup Mode Select
M_RWU    EQU     6               ; Receiver Wakeup Enable
M_WOMS   EQU     7               ; Wired-OR Mode Select
M_SCRE   EQU     8               ; SCI Receiver Enable
M_SCTE   EQU     9               ; SCI Transmitter Enable
M_ILIE   EQU     10              ; Idle Line Interrupt Enable
M_SCRIE  EQU     11              ; SCI Receive Interrupt Enable
M_SCTIE  EQU     12              ; SCI Transmit Interrupt Enable
M_TMIE   EQU     13              ; Timer Interrupt Enable
M_TIR    EQU     14              ; Timer Interrupt Rate
M_SCKP   EQU     15              ; SCI Clock Polarity
M_REIE   EQU     16              ; SCI Error Interrupt Enable (REIE)

;       SCI Status Register Bit Flags

M_TRNE   EQU     0               ; Transmitter Empty
M_TDRE   EQU     1               ; Transmit Data Register Empty
M_RDRF   EQU     2               ; Receive Data Register Full
M_IDLE   EQU     3               ; Idle Line Flag
M_OR     EQU     4               ; Overrun Error Flag 
M_PE     EQU     5               ; Parity Error
M_FE     EQU     6               ; Framing Error Flag
M_R8     EQU     7               ; Received Bit 8 (R8) Address

;       SCI Clock Control Register 

M_CD     EQU     $FFF            ; Clock Divider Mask (CD0-CD11)
M_COD    EQU     12              ; Clock Out Divider
M_SCP    EQU     13              ; Clock Prescaler
M_RCM    EQU     14              ; Receive Clock Mode Source Bit
M_TCM    EQU     15              ; Transmit Clock Source Bit

;------------------------------------------------------------------------
;
;       EQUATES for Enhanced Synchronous Serial Interface (ESSI)
;
;------------------------------------------------------------------------

;
;       Register Addresses Of ESSI0 
M_TX00   EQU     $FFFFBC         ; ESSI0 Transmit Data Register 0
M_TX01   EQU     $FFFFBB         ; ESSIO Transmit Data Register 1
M_TX02   EQU     $FFFFBA         ; ESSIO Transmit Data Register 2
M_TSR0   EQU     $FFFFB9         ; ESSI0 Time Slot Register
M_RX0    EQU     $FFFFB8         ; ESSI0 Receive Data Register
M_SSISR0 EQU     $FFFFB7         ; ESSI0 Status Register
M_CRB0   EQU     $FFFFB6         ; ESSI0 Control Register B
M_CRA0   EQU     $FFFFB5         ; ESSI0 Control Register A
M_TSMA0  EQU     $FFFFB4         ; ESSI0 Transmit Slot Mask Register A
M_TSMB0  EQU     $FFFFB3         ; ESSI0 Transmit Slot Mask Register B
M_RSMA0  EQU     $FFFFB2         ; ESSI0 Receive Slot Mask Register A
M_RSMB0  EQU     $FFFFB1         ; ESSI0 Receive Slot Mask Register B

;       Register Addresses Of ESSI1                                        
M_TX10   EQU     $FFFFAC         ; ESSI1 Transmit Data Register 0
M_TX11   EQU     $FFFFAB         ; ESSI1 Transmit Data Register 1
M_TX12   EQU     $FFFFAA         ; ESSI1 Transmit Data Register 2
M_TSR1   EQU     $FFFFA9         ; ESSI1 Time Slot Register
M_RX1    EQU     $FFFFA8         ; ESSI1 Receive Data Register
M_SSISR1 EQU     $FFFFA7         ; ESSI1 Status Register
M_CRB1   EQU     $FFFFA6         ; ESSI1 Control Register B
M_CRA1   EQU     $FFFFA5         ; ESSI1 Control Register A
M_TSMA1  EQU     $FFFFA4         ; ESSI1 Transmit Slot Mask Register A
M_TSMB1  EQU     $FFFFA3         ; ESSI1 Transmit Slot Mask Register B
M_RSMA1  EQU     $FFFFA2         ; ESSI1 Receive Slot Mask Register A
M_RSMB1  EQU     $FFFFA1         ; ESSI1 Receive Slot Mask Register B

;       ESSI Control Register A Bit Flags

M_PM     EQU     $FF             ; Prescale Modulus Select Mask (PM0-PM7)              
M_PSR    EQU     11              ; Prescaler Range       
M_DC     EQU     $1F000          ; Frame Rate Divider Control Mask (DC0-DC7)
M_ALC    EQU     18	         ; Alignment Control (ALC)
M_WL     EQU     $380000         ; Word Length Control Mask (WL0-WL7)
M_SSC1   EQU     22	         ; Select SC1 as TR #0 drive enable (SSC1)

;       ESSI Control Register B Bit Flags                                   

M_OF     EQU     $3              ; Serial Output Flag Mask
M_OF0    EQU     0               ; Serial Output Flag 0                     
M_OF1    EQU     1               ; Serial Output Flag 1                     
M_SCD    EQU     $1C             ; Serial Control Direction Mask            
M_SCD0   EQU     2               ; Serial Control 0 Direction                
M_SCD1   EQU     3               ; Serial Control 1 Direction               
M_SCD2   EQU     4               ; Serial Control 2 Direction               
M_SCKD   EQU     5               ; Clock Source Direction
M_SHFD   EQU     6               ; Shift Direction                          
M_FSL    EQU     $180            ; Frame Sync Length Mask (FSL0-FSL1)
M_FSL0   EQU     7               ; Frame Sync Length 0
M_FSL1   EQU     8               ; Frame Sync Length 1
M_FSR    EQU     9               ; Frame Sync Relative Timing
M_FSP    EQU     10              ; Frame Sync Polarity
M_CKP    EQU     11              ; Clock Polarity                           
M_SYN    EQU     12              ; Sync/Async Control                       
M_MOD    EQU     13              ; ESSI Mode Select
M_SSTE   EQU     $1C000          ; ESSI Transmit enable Mask                  
M_SSTE2  EQU     14              ; ESSI Transmit #2 Enable                   
M_SSTE1  EQU     15              ; ESSI Transmit #1 Enable                    
M_SSTE0  EQU     16              ; ESSI Transmit #0 Enable                    
M_SSRE   EQU     17              ; ESSI Receive Enable                       
M_SSTIE  EQU     18              ; ESSI Transmit Interrupt Enable            
M_SSRIE  EQU     19              ; ESSI Receive Interrupt Enable              
M_STLIE  EQU     20              ; ESSI Transmit Last Slot Interrupt Enable 
M_SRLIE  EQU     21              ; ESSI Receive Last Slot Interrupt Enable 
M_STEIE  EQU     22              ; ESSI Transmit Error Interrupt Enable 
M_SREIE  EQU     23              ; ESSI Receive Error Interrupt Enable              

;       ESSI Status Register Bit Flags                                       

M_IF     EQU     $3              ; Serial Input Flag Mask           
M_IF0    EQU     0               ; Serial Input Flag 0                      
M_IF1    EQU     1               ; Serial Input Flag 1                      
M_TFS    EQU     2               ; Transmit Frame Sync Flag                 
M_RFS    EQU     3               ; Receive Frame Sync Flag                  
M_TUE    EQU     4               ; Transmitter Underrun Error FLag          
M_ROE    EQU     5               ; Receiver Overrun Error Flag              
M_TDE    EQU     6               ; Transmit Data Register Empty             
M_RDF    EQU     7               ; Receive Data Register Full

;       ESSI Transmit Slot Mask Register A

M_SSTSA  EQU     $FFFF           ; ESSI Transmit Slot Bits Mask A (TS0-TS15)

;       ESSI Transmit Slot Mask Register B

M_SSTSB  EQU     $FFFF           ; ESSI Transmit Slot Bits Mask B (TS16-TS31)

;       ESSI Receive Slot Mask Register A

M_SSRSA  EQU     $FFFF           ; ESSI Receive Slot Bits Mask A (RS0-RS15)
 
;       ESSI Receive Slot Mask Register B

M_SSRSB  EQU     $FFFF           ; ESSI Receive Slot Bits Mask B (RS16-RS31)

              

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

M_HPL    EQU     $3              ; Host Interrupt Priority Level Mask
M_HPL0   EQU     0               ; Host Interrupt Priority Level (low)
M_HPL1   EQU     1               ; Host Interrupt Priority Level (high)
M_S0L    EQU     $C              ; ESSI0 Interrupt Priority Level Mask
M_S0L0   EQU     2               ; ESSI0 Interrupt Priority Level (low)
M_S0L1   EQU     3               ; ESSI0 Interrupt Priority Level (high)
M_S1L    EQU     $30             ; ESSI1 Interrupt Priority Level Mask
M_S1L0   EQU     4               ; ESSI1 Interrupt Priority Level (low)
M_S1L1   EQU     5               ; ESSI1 Interrupt Priority Level (high)
M_SCL    EQU     $C0             ; SCI  Interrupt Priority Level  Mask       
M_SCL0   EQU     6               ; SCI  Interrupt Priority Level  (low)
M_SCL1   EQU     7               ; SCI  Interrupt Priority Level  (high)
M_T0L    EQU     $300            ; TIMER Interrupt Priority Level Mask
M_T0L0   EQU     8               ; TIMER Interrupt Priority Level (low)
M_T0L1   EQU     9               ; TIMER Interrupt Priority Level (high)


;------------------------------------------------------------------------
;
;       EQUATES for TIMER 
;
;------------------------------------------------------------------------

;       Register Addresses Of TIMER0

M_TCSR0  EQU     $FFFF8F         ; TIMER0 Control/Status Register            
M_TLR0	 EQU	 $FFFF8E	 ; TIMER0 Load Reg   
M_TCPR0  EQU     $FFFF8D         ; TIMER0 Compare Register
M_TCR0	 EQU	 $FFFF8C 	 ; TIMER0 Count Register

;       Register Addresses Of TIMER1

M_TCSR1  EQU     $FFFF8B         ; TIMER1 Control/Status Register            
M_TLR1	 EQU	 $FFFF8A	 ; TIMER1 Load Reg   
M_TCPR1  EQU     $FFFF89         ; TIMER1 Compare Register
M_TCR1	 EQU	 $FFFF88	 ; TIMER1 Count Register


;       Register Addresses Of TIMER2

M_TCSR2  EQU     $FFFF87         ; TIMER2 Control/Status Register            
M_TLR2	 EQU	 $FFFF86	 ; TIMER2 Load Reg   
M_TCPR2  EQU     $FFFF85         ; TIMER2 Compare Register
M_TCR2	 EQU	 $FFFF84	 ; TIMER2 Count Register
M_TPLR	 EQU	 $FFFF83	 ; TIMER Prescaler Load Register
M_TPCR	 EQU	 $FFFF82	 ; TIMER Prescalar Count Register


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
M_PCE	 EQU	 15		 ; Prescaled Clock Enable
M_TOF    EQU     20              ; Timer Overflow Flag
M_TCF    EQU     21              ; Timer Compare Flag 

;       Timer Prescaler Register Bit Flags                                        

M_PS	EQU	 $600000 	 ; Prescaler Source Mask
M_PS0	EQU	 21
M_PS1	EQU	 22

;	Timer Control Bits
M_TC0    EQU     4               ; Timer Control 0
M_TC1    EQU     5               ; Timer Control 1
M_TC2    EQU     6               ; Timer Control 2
M_TC3    EQU     7               ; Timer Control 3


;------------------------------------------------------------------------
;
;       EQUATES for Direct Memory Access (DMA)                                 
;
;------------------------------------------------------------------------

;       Register Addresses Of DMA
M_DSTR	 EQU	$FFFFF4		; DMA Status Register
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
 
;       Register Addresses Of DMA4

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

;	DMA Control Register

M_DSS	 EQU	$3		; DMA Source Space Mask (DSS0-Dss1)
M_DSS0	 EQU	0		; DMA Source Memory space 0
M_DSS1	 EQU	1		; DMA Source Memory space 1	
M_DDS	 EQU	$C		; DMA Destination Space Mask (DDS-DDS1)
M_DDS0	 EQU	2		; DMA Destination Memory Space 0
M_DDS1	 EQU	3		; DMA Destination Memory Space 1
M_DAM	 EQU	$3f0		; DMA Address Mode Mask (DAM5-DAM0)
M_DAM0 	 EQU	4		; DMA Address Mode 0
M_DAM1 	 EQU	5		; DMA Address Mode 1
M_DAM2 	 EQU	6		; DMA Address Mode 2
M_DAM3 	 EQU	7		; DMA Address Mode 3
M_DAM4 	 EQU	8		; DMA Address Mode 4
M_DAM5 	 EQU	9		; DMA Address Mode 5
M_D3D	 EQU 	10		; DMA Three Dimensional Mode
M_DRS	 EQU	$F800		; DMA Request Source Mask (DRS0-DRS4)
M_DCON	 EQU	16		; DMA Continuous Mode
M_DPR	 EQU	$60000		; DMA Channel Priority
M_DPR0	 EQU	17		; DMA Channel Priority Level (low)
M_DPR1	 EQU	18		; DMA Channel Priority Level (high)
M_DTM	 EQU	$380000		; DMA Transfer Mode Mask (DTM2-DTM0)
M_DTM0	 EQU	19		; DMA Transfer Mode 0
M_DTM1	 EQU	20		; DMA Transfer Mode 1
M_DTM2	 EQU	21		; DMA Transfer Mode 2
M_DIE	 EQU	22		; DMA Interrupt Enable bit
M_DE	 EQU	23		; DMA Channel Enable bit 

;       DMA Status Register

M_DTD	 EQU	$3F		; Channel Transfer Done Status MASK (DTD0-DTD5)
M_DTD0   EQU     0               ; DMA Channel Transfer Done Status 0
M_DTD1   EQU     1               ; DMA Channel Transfer Done Status 1
M_DTD2   EQU     2               ; DMA Channel Transfer Done Status 2
M_DTD3   EQU     3               ; DMA Channel Transfer Done Status 3
M_DTD4   EQU     4               ; DMA Channel Transfer Done Status 4
M_DTD5   EQU     5               ; DMA Channel Transfer Done Status 5
M_DACT	 EQU	 8		 ; DMA Active State
M_DCH	 EQU	 $E00		 ; DMA Active Channel Mask (DCH0-DCH2)
M_DCH0	 EQU	 9		 ; DMA Active Channel 0
M_DCH1	 EQU	 10		 ; DMA Active Channel 1
M_DCH2	 EQU	 11	 	 ; DMA Active Channel 2


;------------------------------------------------------------------------
;
;       EQUATES for Phase Locked Loop (PLL) 
;
;------------------------------------------------------------------------

;       Register Addresses Of PLL

M_PCTL   EQU     $FFFFFD         ; PLL Control Register

;       PLL Control Register

M_MF     EQU     $FFF            ; Multiplication Factor Bits Mask (MF0-MF11)
M_DF     EQU     $7000           ; Division Factor Bits Mask (DF0-DF2)
M_XTLR   EQU     15              ; XTAL Range select bit
M_XTLD   EQU     16              ; XTAL Disable Bit
M_PSTP   EQU     17              ; STOP Processing State Bit 
M_PEN    EQU     18              ; PLL Enable Bit
M_PCOD   EQU     19              ; PLL Clock Output Disable Bit
M_PD	 EQU	$F00000		; PreDivider Factor Bits Mask (PD0-PD3)


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
M_IDR	 EQU	$FFFFF5		; ID Register

;       Bus Control Register

M_BA0W   EQU     $1F             ; Area 0 Wait Control Mask (BA0W0-BA0W4)
M_BA1W   EQU     $3E0            ; Area 1 Wait Control Mask (BA1W0-BA14)
M_BA2W   EQU     $1C00           ; Area 2 Wait Control Mask (BA2W0-BA2W2)
M_BA3W   EQU     $E000           ; Area 3 Wait Control Mask (BA3W0-BA3W3)
M_BDFW   EQU     $1F0000         ; Default Area Wait Control Mask (BDFW0-BDFW4)
M_BBS    EQU     21              ; Bus State
M_BLH    EQU     22              ; Bus Lock Hold
M_BRH    EQU     23              ; Bus Request Hold

;       DRAM Control Register

M_BCW    EQU     $3              ; In Page Wait States Bits Mask (BCW0-BCW1)
M_BRW    EQU     $C              ; Out Of Page Wait States Bits Mask (BRW0-BRW1)
M_BPS    EQU     $300            ; DRAM Page Size Bits Mask (BPS0-BPS1)
M_BPLE   EQU     11              ; Page Logic Enable
M_BME    EQU     12              ; Mastership Enable
M_BRE    EQU     13              ; Refresh Enable
M_BSTR   EQU     14              ; Software Triggered Refresh
M_BRF    EQU     $7F8000         ; Refresh Rate Bits Mask (BRF0-BRF7)
M_BRP    EQU     23              ; Refresh prescaler

;       Address Attribute Registers

M_BAT    EQU     $3              ; External Access Type and Pin Definition Bits Mask (BAT0-BAT1)
M_BAAP   EQU     2               ; Address Attribute Pin Polarity
M_BPEN   EQU     3               ; Program Space Enable
M_BXEN   EQU     4               ; X Data Space Enable
M_BYEN   EQU     5               ; Y Data Space Enable
M_BAM    EQU     6               ; Address Muxing
M_BPAC 	 EQU	 7		 ; Packing Enable
M_BNC    EQU     $F00            ; Number of Address Bits to Compare Mask (BNC0-BNC3)
M_BAC    EQU     $FFF000         ; Address to Compare Bits Mask (BAC0-BAC11)

;       control and status bits in SR

M_CP	EQU	$c00000          ; mask for CORE-DMA priority bits in SR
M_CA    equ     0                ; Carry
M_V     equ     1                ; Overflow      
M_Z     equ     2                ; Zero
M_N     equ     3                ; Negative      
M_U     equ     4                ; Unnormalized
M_E     equ     5                ; Extension     
M_L     equ     6                ; Limit
M_S     equ     7                ; Scaling Bit   
M_I0    equ     8                ; Interupt Mask Bit 0
M_I1    equ     9                ; Interupt Mask Bit 1
M_S0    equ     10               ; Scaling Mode Bit 0
M_S1    equ     11               ; Scaling Mode Bit 1
M_SC    equ     13               ; Sixteen_Bit Compatibility
M_DM    equ     14               ; Double Precision Multiply
M_LF    equ     15               ; DO-Loop Flag
M_FV    equ     16               ; DO-Forever Flag
M_SA    equ     17               ; Sixteen-Bit Arithmetic
M_CE    equ     19               ; Instruction Cache Enable
M_SM    equ     20               ; Arithmetic Saturation
M_RM    equ     21               ; Rounding Mode
M_CP0	EQU	22               ; bit 0 of priority bits in SR
M_CP1   EQU     23               ; bit 1 of priority bits in SR

;       control and status bits in OMR
M_CDP	EQU	$300             ; mask for CORE-DMA priority bits in OMR
M_MA    equ     0                ; Operating Mode A
M_MB   equ      1                ; Operating Mode B
M_MC    equ     2                ; Operating Mode C
M_MD    equ     3                ; Operating Mode D
M_EBD   equ     4                ; External Bus Disable bit in OMR
M_SD    equ     6                ; Stop Delay  
M_CDP0	EQU	8                ; bit 0 of priority bits in OMR
M_CDP1	EQU	9                ; bit 1 of priority bits in OMR
M_BEN   EQU     10               ; Burst Enable 
M_TAS   EQU     11               ; TA Synchronize Select
M_BRT   EQU     12               ; Bus Release Timing 
M_XYS   equ     16               ; Stack Extension space select bit in OMR.
M_EUN   equ     17               ; Extensed stack UNderflow flag in OMR.
M_EOV   equ     18               ; Extended stack OVerflow flag in OMR.
M_WRP   equ     19               ; Extended WRaP flag in OMR.
M_SEN   equ     20               ; Stack Extension Enable bit in OMR.




;*********************************************************************************
;
;     EQUATES for 56301 interrupts
;     Reference: 56301 Specifications Revision 3.00 
;
;     Last update: August   16 1994
;
;*********************************************************************************

	page	132,55,0,0,0
	opt	mex

intequ  ident   1,0

	if	@DEF(I_VEC)
	;leave user definition as is.
	else
I_VEC	equ	$0
	endif

;------------------------------------------------------------------------
; Non-Maskable interrupts
;------------------------------------------------------------------------
I_RESET  EQU  I_VEC+$00   ; Hardware RESET
I_STACK  EQU  I_VEC+$02   ; Stack Error
I_ILL    EQU  I_VEC+$04   ; Illegal Instruction
I_DBG    EQU  I_VEC+$06   ; Debug Request      
I_TRAP   EQU  I_VEC+$08   ; Trap
I_NMI    EQU  I_VEC+$0A   ; Non Maskable Interrupt

;------------------------------------------------------------------------
; Interrupt Request Pins
;------------------------------------------------------------------------
I_IRQA   EQU  I_VEC+$10   ; IRQA
I_IRQB   EQU  I_VEC+$12   ; IRQB
I_IRQC   EQU  I_VEC+$14   ; IRQC
I_IRQD   EQU  I_VEC+$16   ; IRQD

;------------------------------------------------------------------------
; DMA Interrupts
;------------------------------------------------------------------------
I_DMA0   EQU  I_VEC+$18   ; DMA Channel 0
I_DMA1   EQU  I_VEC+$1A   ; DMA Channel 1
I_DMA2   EQU  I_VEC+$1C   ; DMA Channel 2
I_DMA3   EQU  I_VEC+$1E   ; DMA Channel 3
I_DMA4   EQU  I_VEC+$20   ; DMA Channel 4
I_DMA5   EQU  I_VEC+$22   ; DMA Channel 5

;------------------------------------------------------------------------
; Timer Interrupts
;------------------------------------------------------------------------
I_TIM0C  EQU  I_VEC+$24   ; TIMER 0 compare
I_TIM0OF EQU  I_VEC+$26   ; TIMER 0 overflow
I_TIM1C  EQU  I_VEC+$28   ; TIMER 1 compare
I_TIM1OF EQU  I_VEC+$2A   ; TIMER 1 overflow
I_TIM2C  EQU  I_VEC+$2C   ; TIMER 2 compare
I_TIM2OF EQU  I_VEC+$2E   ; TIMER 2 overflow

;------------------------------------------------------------------------
; ESSI Interrupts
;------------------------------------------------------------------------
I_SI0RD  EQU  I_VEC+$30   ; ESSI0 Receive Data
I_SI0RDE EQU  I_VEC+$32   ; ESSI0 Receive Data With Exception Status
I_SI0RLS EQU  I_VEC+$34   ; ESSI0 Receive last slot
I_SI0TD  EQU  I_VEC+$36   ; ESSI0 Transmit data
I_SI0TDE EQU  I_VEC+$38   ; ESSI0 Transmit Data With Exception Status
I_SI0TLS EQU  I_VEC+$3A   ; ESSI0 Transmit last slot
I_SI1RD  EQU  I_VEC+$40   ; ESSI1 Receive Data
I_SI1RDE EQU  I_VEC+$42   ; ESSI1 Receive Data With Exception Status
I_SI1RLS EQU  I_VEC+$44   ; ESSI1 Receive last slot
I_SI1TD  EQU  I_VEC+$46   ; ESSI1 Transmit data
I_SI1TDE EQU  I_VEC+$48   ; ESSI1 Transmit Data With Exception Status
I_SI1TLS EQU  I_VEC+$4A   ; ESSI1 Transmit last slot

;------------------------------------------------------------------------
; SCI Interrupts
;------------------------------------------------------------------------
I_SCIRD  EQU  I_VEC+$50   ; SCI Receive Data 
I_SCIRDE EQU  I_VEC+$52   ; SCI Receive Data With Exception Status
I_SCITD  EQU  I_VEC+$54   ; SCI Transmit Data
I_SCIIL  EQU  I_VEC+$56   ; SCI Idle Line
I_SCITM  EQU  I_VEC+$58   ; SCI Timer

;------------------------------------------------------------------------
; HOST Interrupts
;------------------------------------------------------------------------
I_HPTT   EQU  I_VEC+$60   ; Host PCI Transaction Termination
I_HPTA   EQU  I_VEC+$62   ; Host PCI Transaction Abort
I_HPPE   EQU  I_VEC+$64   ; Host PCI Parity Error
I_HPTC   EQU  I_VEC+$66   ; Host PCI Transfer Complete
I_HPMR   EQU  I_VEC+$68   ; Host PCI Master Receive
I_HSR    EQU  I_VEC+$6A   ; Host Slave Receive
I_HPMT   EQU  I_VEC+$6C   ; Host PCI Master Transmit
I_HST    EQU  I_VEC+$6E   ; Host Slave Transmit
I_HPMA   EQU  I_VEC+$70   ; Host PCI Master Address
I_HCNMI  EQU  I_VEC+$72   ; Host Command/Host NMI (Default)

;------------------------------------------------------------------------
; INTERRUPT ENDING ADDRESS
;------------------------------------------------------------------------
I_INTEND EQU  I_VEC+$FF   ; last address of interrupt vector space
