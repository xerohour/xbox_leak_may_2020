//****************************************************************************
//	    Filename: USB_brd.h
//       Project: USB Simulator
//	   Copyright: Microsoft, 2000
//
//   Description: This is the header file for the USB_brd module.  It contains
//				  information supporting interfacing the USB_brd and the USB
//				  chip contained on that board.
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/USB_brd.h $
// $Revision: 1.9 $
// $Date: 2001/01/25 15:58:52Z $ 
// $Author: klpayne $
// $Log: USB_brd.h $
// Revision 1.9  2001/01/25 15:58:52Z  klpayne
// Add suspend/resume commands
// Revision 1.8  2001/01/11 14:34:43Z  klpayne
// Add support for USB Reset Command
// Revision 1.7  2001/01/05 19:01:42Z  klpayne
// Cleanup defines and data struct
// Revision 1.6  2001/01/04 19:24:47Z  klpayne
// Support IRQ mapping changes
// Revision 1.5  2000/12/15 21:52:49Z  klpayne
// Adjusted board addresses, moved some defines to sim.h
// Revision 1.4  2000/12/13 15:24:34Z  klpayne
// Move intr processing out of intr handler
// Revision 1.3  2000/12/06 22:05:03Z  klpayne
// Code cleanup
// Revision 1.2  2000/12/06 17:42:57Z  klpayne
// Added support for sim_cmd
// Revision 1.1  2000/12/05 22:04:53Z  klpayne
// Initial revision
//
//****************************************************************************
#ifndef USB_BRD_H
#define USB_BRD_H

//-----------------------PUBLIC CONSTANTS AND MACROS---------------------------

// defines for USB chip registers
#define USB_TXDAT        0x00 // Transmit FIFO Data              W 
#define USB_TXCNTL       0x01 // Tx FIFO Byte Count Low        R/W 
#define USB_TXCNTH       0x02 // Tx FIFO Byte Count High       R/W 
#define USB_TXCON        0x03 // Tx FIFO Control               R/W 
#define USB_TXFLG        0x04 // Tx FIFO Flag                      
#define USB_RXDAT        0x05 // Rx Data Receive FIFO          R   
#define USB_RXCNTL       0x06 // Rx FIFO Byte Count Low        R/W 
#define USB_RXCNTH       0x07 // Rx FIFO Byte Count High       R/W  
#define USB_RXCON        0x08 // Rx FIFO Control               R/W 
#define USB_RXFLG        0x09 // Rx FIFO Flags                 R/W  
#define USB_EPINDEX      0x0A // Endpoint Index                R/W 
#define USB_EPCON        0x0B // Endpoint Control              R/W 
#define USB_TXSTAT       0x0C // Tx FIFO Status                R/W  
#define USB_RXSTAT       0x0D // Rx FIFO Status                R/W   
#define USB_SOFL         0x0E // Start Of Frame Low            R/W 
#define USB_SOFH         0x0F // Start Of Frame High           R/W  
#define USB_FADDR        0x10 // Function Address              R/W 
#define USB_SCR          0x11 // System Control                R/W 
#define USB_SSR          0x12 // System Status                 R/W 
#define USB_SBI          0x14 // Serial Bus Interrupt          R/W 
#define USB_SBI1         0x15 // Serial Bus Interrupt 1        R/W 
#define USB_SBIE         0x16 // Serial Bus Interrupt Enable   R/W 
#define USB_SBIE1        0x17 // Serial Bus Interrupt Enable 1 R/W 
#define USB_REV          0x18 // Hardware Revision Number      R/                                  
#define USB_LOCK         0x19 // Suspend Power Off Locking     R/W 
#define USB_PEND         0x1A // Pend Hardware Status Update   R/W 
#define USB_SCRATCH      0x1B // Scratch Firmware Information  R/W 
#define USB_MCSR         0x1C // Misc. Control/Status          R/W 
#define USB_DSAV         0x1D // Data Set Available            R/W 
#define USB_DSAV1        0x1E // Data Set Available 1          R/W 

// base addresses for each USB board
// NOTE: dip switches on boards MUST be set to match these
#define USB_BRD_1	0x200	// must use IRQ10
#define USB_BRD_2	0x300	// must use IRQ11
#define USB_BRD_3	0x320	// must use IRQ12
#define USB_BRD_4	0x380	// must use IRQ15

// defines for IRQ addresses
#define IRQ09 0x71
#define IRQ10 0x72
#define IRQ14 0x76
#define IRQ15 0x77

// defines for IRQ mask bits
#define IRQ09_MASK 0x02
#define IRQ10_MASK 0x04
#define IRQ14_MASK 0x40
#define IRQ15_MASK 0x80

// value returned when no board is present
#define NO_USB_BRD 0x00

// defines for all/no bits in a register
#define USB_REG_ALL_BITS 0xff
#define USB_REG_NO_BITS 0x00
                                                                 
//    Register definitions for USV_TXCON                        
//    Note: FIFO specified by EPINDEX                              
#define TXCON_TXCLR        0x80 // big Clear & Flush
                                // bits 6:5 determine FIFO size
#define TXCON_NONISO_16    0x00
#define TXCON_NONISO_64    0x20
#define TXCON_NONISO_8     0x40
#define TXCON_NONISO_32    0x60

#define TXCON_ISO_64       0x00
#define TXCON_ISO_256      0x20
#define TXCON_ISO_512      0x40
#define TXCON_ISO_1024     0x60  

#define TXCON_RESERVED     0x10 // RESERVED bit always 0 

#define TXCON_TXISO        0x08 // Tx Iso Data           R/W 
#define TXCON_ATM          0x04 // Auto Tx Management    R/W 
#define TXCON_ADVRM        0x02 // Advance Read Marker   R/W 
#define TXCON_REVRP        0x01 // Reverse Read Pointer  R/W 

//    Register definitions for USB_TXFLG
 
#define TXFLG_TXFIF1      0x80 // TXFIF[1:0]           R   
#define TXFLG_TXFIF0      0x40 // Tx FIFO Index            

#define TXFLG_RESERVED0   0x20
#define TXFLG_RESERVED1   0x10

#define TXFLG_TXEMP       0x08 // Tx Empty             R   
#define TXFLG_TXFULL      0x04 // Tx Full              R   
#define TXFLG_TXURF       0x02 // Tx FIFO Underrun     R/W   
#define TXFLG_TXOVF       0x01 // Tx FIFO Overrun      R/W 
                                                                    
//    Register definitions for USB_RXCON
//    Note: FIFO specified by EPINDEX
#define RXCON_RXCLR        0x80 // big Clear & Flush            
                                // bits 6:5 determine FIFO size 
#define RXCON_NONISO_16    0x00
#define RXCON_NONISO_64    0x20
#define RXCON_NONISO_8     0x40
#define RXCON_NONISO_32    0x60

#define RXCON_ISO_64       0x00
#define RXCON_ISO_256      0x20
#define RXCON_ISO_512      0x40
#define RXCON_ISO_1024     0x60

#define RXCON_RXFFRC       0x10 // FIFO Read Complete    R/W 
#define RXCON_RXISO        0x08 // Rx Iso Data           R/W 
#define RXCON_ARM          0x04 // Auto Rx Management    R/W 
#define RXCON_ADVRM        0x02 // Advance Write Marker  R/W 
#define RXCON_REVRP        0x01 // Reverse Write Pointer R/W 

//    Register definitions for USB_RXFLG
#define RXFLG_RXFIF1      0x80 // RXFIF[1:0]           R   
#define RXFLG_RXFIF0      0x40 // Rx FIFO Index            

#define RXFLG_RESERVED    0x20
#define RXFLG_RXFLUSH     0x10 // Rx FIFO Flush        R   

#define RXFLG_RXEMP       0x08 // Rx Empty             R   
#define RXFLG_RXFULL      0x04 // Rx Full              R   
#define RXFLG_RXURF       0x02 // Rx FIFO Underrun     R/W   
#define RXFLG_RXOVF       0x01 // Rx FIFO Overrun      R/W 

//    Register definitions for USB_EPINDEX  
//    Note: Bits 3:7 are unused
//        & bits 2:0 determine the endpoint Tx Rx pair
#define EPINDEX_EP0        0x00
#define EPINDEX_EP1        0x01
#define EPINDEX_EP2        0x02
#define EPINDEX_EP3        0x03
#define EPINDEX_EP4        0x04
#define EPINDEX_EP5        0x05
#define EPINDEX_EP6        0x06
#define EPINDEX_EP7        0x07

//    Register definitions for USB_EPCON
//    Endpoint Control Register
#define EPCON_RXSTL        0x80 // Stall Rx Endpoint      R/W 
#define EPCON_TXSTL        0x40 // Stall Tx Endpoint      R/W 
#define EPCON_CTLEP        0x20 // Control Endpoint       R/W 
#define EPCON_RXSPM        0x10 // Rx Single-Packet Mode  R/W 
#define EPCON_RXIE         0x08 // Rx Input Enable        R/W 
#define EPCON_RXEPEN       0x04 // Rx Endpoint Enable     R/W 
#define EPCON_TXOE         0x02 // Tx Output Enable       R/W 
#define EPCON_TXEPEN       0x01 // Tx Endpoint Enable     R/W 
                                                                        
//    Register definitions for USB_TXSTAT
//    Endpoint Transmit Status
#define TXSTAT_TXSEQ       0x80 // Tx Current Seq          R/W 
#define TXSTAT_TXDSAM      0x40 // Tx Data-Set-Avail Mode  R/W 
#define TXSTAT_TXNAKE      0x20 // Tx NAK Mode             R/W 
#define TXSTAT_TXFLUSH     0x10 // Tx FIFO Packet Flush    R   
#define TXSTAT_TXSOVW      0x08 // Tx Data Seq Overwrite   R/W 
#define TXSTAT_TXVOID      0x04 // Tx Void                 R/W 
#define TXSTAT_TXERR       0x02 // Tx Error                R   
#define TXSTAT_TXACK       0x01 // Tx Ack                  R   

//    Register definitions for USB_RXSTAT           
//    Endpoint Receive Status
#define RXSTAT_RXSEQ       0x80 // Rx Endpoint Seq         R/W 
#define RXSTAT_RXSETUP     0x40 // Rx SETUP Token          R/W 
#define RXSTAT_STOVW       0x20 // Start Overwrite Flag    R   
#define RXSTAT_EDOVW       0x10 // End Overwrite Flag      R/W 
#define RXSTAT_RXSOVW      0x08 // Rx Data Seq Overwrite     W 
#define RXSTAT_RXVOID      0x04 // Rx Void                 R   
#define RXSTAT_RXERR       0x02 // Rx Error                R   
#define RXSTAT_RXACK       0x01 // Rx Ack                  R   


//    Register definitions for USB_SOFH
//    Start of Frame High
#define SOFH_SOFACK        0x80 // SOF Token Rx Without Error R   
#define SOFH_ASOF          0x40 // Any SOF                    R/W 
#define SOFH_SOFIE         0x20 // SOF Interrupt Enable       R/W 
#define SOFH_FTLOCK        0x10 // Frame Timer Lock           R   
#define SOFH_SOFODIS       0x08 // SOF Pin Output Diable      R/W 

//    Register definitions for USB_SCR
//    System Control
#define SCR_IRQPOL         0x80 // IRQ Polarity               R/W 
#define SCR_RWUPE          0x40 // Enable Remote Wake-Up      R/W 
#define SCR_IE_SUSP        0x20 // Enable Suspend Int         R/W 
#define SCR_IE_RESET       0x10 // Enable Reset Int           R/W 
#define SCR_SRESET         0x08 // Software Reset             R/W 
#define SCR_IRQLVL         0x04 // Int Mode                   R/W 
#define SCR_T_IRQ          0x02 // Global Int Enable          R/W 
#define SCR_RESERVED       0x01 // RESERVED bit always 0          

//    Register definitions for USB_SSR
//    Serial Bus Interrupt
#define SSR_RESERVED       0xE0 // RESERVED bit always 0          
#define SSR_SUSPPO         0x10 // Suspend Power Off          R/W 
#define SSR_SUSDIS         0x08 // Suspend Disable            R/W 
#define SSR_RESUME         0x04 // Resume Detected            R/W 
#define SSR_SUSPEND        0x02 // Suspend Detected           R/W 
#define SSR_RESET          0x01 // USB Reset Detected         R/W 

//    Register definitions for USB_SBI 
//    Serial Bus Interrupt
#define SBI_FRXD3          0x80 // Endpoint 3  R/W 
#define SBI_FTXD3          0x40
#define SBI_FRXD2          0x20 // Endpoint 2  R/W 
#define SBI_FTXD2          0x10
#define SBI_FRXD1          0x08 // Endpoint 1  R/W 
#define SBI_FTXD1          0x04
#define SBI_FRXD0          0x02 // Endpoint 0  R/W 
#define SBI_FTXD0          0x01
                        
//    Register definitions for USB_SBI1
//    Serial Bus Interrupt 
#define SBI1_FRXD7         0x80 // Endpoint 7  R/W 
#define SBI1_FTXD7         0x40
#define SBI1_FRXD6         0x20 // Endpoint 6  R/W 
#define SBI1_FTXD6         0x10
#define SBI1_FRXD5         0x08 // Endpoint 5  R/W 
#define SBI1_FTXD5         0x04
#define SBI1_FRXD4         0x02 // Endpoint 4  R/W 
#define SBI1_FTXD4         0x01

//    Register definitions for USB_SBIE 
//    Serial Bus Interrupt Enable
#define SBIE_FRXIE3        0x80 // Endpoint 3  R/W 
#define SBIE_FTXIE3        0x40
#define SBIE_FRXIE2        0x20 // Endpoint 2  R/W 
#define SBIE_FTXIE2        0x10
#define SBIE_FRXIE1        0x08 // Endpoint 1  R/W 
#define SBIE_FTXIE1        0x04
#define SBIE_FRXIE0        0x02 // Endpoint 0  R/W 
#define SBIE_FTXIE0        0x01

//    Register definitions for USB_SBIE1
//    Serial Bus Interrupt Enable
#define SBIE1_FRXIE7       0x80 // Endpoint 7  R/W 
#define SBIE1_FTXIE7       0x40
#define SBIE1_FRXIE6       0x20 // Endpoint 6  R/W 
#define SBIE1_FTXIE6       0x10
#define SBIE1_FRXIE5       0x08 // Endpoint 5  R/W 
#define SBIE1_FTXIE5       0x04
#define SBIE1_FRXIE4       0x02 // Endpoint 4  R/W 
#define SBIE1_FTXIE4       0x01

//    Register definitions for USB_LOCK
//    Suspend Power-Off Locking
#define LOCK_RESERVED       0xFE // RESERVED bit always 0          
#define LOCK_UNLOCKED       0x01 // Locking Control/Status     R/W 

//    Register definitions for USB_PEND
//    Pend Hardware Status Update
#define PEND_RESERVED       0xFE // RESERVED bit always 0          
#define PEND_PEND           0x01 // Pend                       R/W 

//    Register definitions for USB_SCRATCH
//    Scratch Firmware Inforamtion
#define SCRATCH_IE_RESUME   0x80 // Enable Resume Int          R/W 
#define SCRATCH_SCRATCH     0x7F // Scratch Information        R/W 

//    Register definitions for USB_MCSR
//    Misc Control/Status
#define MCSR_RWUPR          0x80 // Remote Wake-Up Remember    R/W 
#define MCSR_INIT           0x40 // Device Init                R/W 
#define MCSR_SUSPS          0x20 // Suspend Status             R/W 
#define MCSR_PKGID          0x10 // Package ID                 R/W 
#define MCSR_FEAT           0x08 // Feature Enable             R/W 
#define MCSR_BDFEAT         0x04 // Board Feature Enable       R/W 
#define MCSR_SUSPLOE        0x02 // Suspend Lock Out Enable    R/W 
#define MCSR_DPEN           0x01 // DPSL Pull-Up Enable        R/W 

//    Register definitions for USB_DSAV
//    Data Set Available
#define DSAV_RXAV3          0x80 // Rx/Tx Data Set Available 3 R   
#define DSAV_TXAV3          0x40
#define DSAV_RXAV2          0x20 // Rx/Tx Data Set Available 2 R   
#define DSAV_TXAV2          0x10
#define DSAV_RXAV1          0x08 // Rx/Tx Data Set Available 1 R   
#define DSAV_TXAV1          0x04
#define DSAV_RXAV0          0x02 // Rx/Tx Data Set Available 0 R   
#define DSAV_TXAV0          0x01

//    Register definitions for USB_DSAV1
//    Data Set Available
#define DSAV1_RXAV7         0x80 // Rx/Tx Data Set Available 7 R   
#define DSAV1_TXAV7         0x40
#define DSAV1_RXAV6         0x20 // Rx/Tx Data Set Available 6 R   
#define DSAV1_TXAV6         0x10
#define DSAV1_RXAV5         0x08 // Rx/Tx Data Set Available 5 R   
#define DSAV1_TXAV5         0x04
#define DSAV1_RXAV4         0x02 // Rx/Tx Data Set Available 4 R   
#define DSAV1_TXAV4         0x01

// PIDs, handshakes for sending to controller
#define PID_SETUP 0xB4
#define PID_IN 0x96
#define PID_OUT 0x87
#define USB_ACK 0x4B
#define USB_NAK 0x5A
#define USB_DATA0 0xC3
#define USB_DATA1 0xD2
#define USB_RESET 0xFF	// not a real USB PID
#define USB_SUSPEND 0xFE	// not a real USB PID
#define USB_RESUME 0xFD	// not a real USB PID

//-------------------------EXTERN PUBLIC VARIABLES-----------------------------

//-----------------------PUBLIC STRUCTURE DEFINITIONS--------------------------

//------------------------PUBLIC FUNCTION PROTOTYPES---------------------------
#endif