//*****************************************************************************
//	    Filename: sim.h
//       Project: Microsoft USB Simulator
//	   Copyright: Microsoft, 2000
//
//   Description: This is the common header file for the project
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/sim.h $
// $Revision: 1.18 $
// $Date: 2001/05/08 14:59:56Z $ 
// $Author: klpayne $
// $Log: sim.h $
// Revision 1.18  2001/05/08 14:59:56Z  klpayne
// Add prototypes for stall
// Revision 1.17  2001/01/25 17:48:51Z  rjgardner
// Remove public declaration for function Process_Connect.
// Revision 1.16  2001/01/25 15:03:52Z  rjgardner
// Add public function declarations for Ether.c which were originally
// in Ether.h
// Revision 1.15  2001/01/23 19:37:19Z  klpayne
// Fix Endpoint array for 4th board.
// Revision 1.14  2001/01/18 16:27:27Z  klpayne
// Add support for ready LED
// Revision 1.13  2001/01/17 20:23:57Z  klpayne
// Support all 8 possible endpoints
// Revision 1.12  2001/01/12 15:09:01Z  klpayne
// Support autorepeat
// Revision 1.11  2001/01/09 21:46:28Z  rjgardner
// Increase maximum USB data transfer size (MAXUSBDATA) to 1024
// Revision 1.10  2001/01/09 21:05:44Z  klpayne
// Make USB_Set_Address public
// Revision 1.9  2001/01/05 19:01:42Z  klpayne
// Cleanup defines and data struct
// Revision 1.8  2001/01/04 19:24:46Z  klpayne
// Support IRQ mapping changes
// Revision 1.7  2001/01/04 15:35:46Z  klpayne
// Interim checkin
// Revision 1.6  2000/12/22 20:51:52Z  rjgardner
// Add extern declaration for device_status byte
// Revision 1.5  2000/12/20 18:05:03Z  klpayne
// Added xmit/receive buffers to endpoint struct
// Revision 1.4  2000/12/15 21:51:46Z  klpayne
// Moved some defines from usb_brd.h to here
// Revision 1.3  2000/12/13 15:21:10Z  klpayne
// Add in GPIO support
// Revision 1.2  2000/12/06 19:20:19Z  rjgardner
// add prototype for Ether_Init
// Revision 1.1  2000/12/05 22:04:54Z  klpayne
// Initial revision
//
//*****************************************************************************
#ifndef SIM_H
#define SIM_H

//-----------------------PUBLIC CONSTANTS AND MACROS---------------------------
#define FALSE 0
#define TRUE 1

// GPIO defines
#define GPIO_CTRL 0x35B	// control reg
#define GPIO_PORTA 0x358	// GPIO Port A
#define GPIO_PORTB 0x359	// GPIO Port B
#define GPIO_PORTC 0x35A	// GPIO Port C

#define GPIO_CTRL_DATA 0x89	// Mode 0, Port A out, Port B out, Port C in (all)

// USB channel connect status defines
#define USB1_CONNECT 0x01
#define USB2_CONNECT 0x02
#define USB3_CONNECT 0x04
#define USB4_CONNECT 0x08

#define MAXUSBDATA	1024	// max usb data transfer size
#define MAXUSBBRDS	4		// max number of usb boards

// number of endpoints
#define USB_MAX_EP 8

//-------------------------EXTERN PUBLIC VARIABLES-----------------------------
// defines associated with global USB info
//
struct EndpointSetup
{
    unsigned char fifosize;	// size of endpoint fifo
    unsigned char type; // type of endpoint (see enum)
    unsigned char autorepeat; // autorepeat on/off
	unsigned char ack;	// indicates need to send an ack phase
	int xmitbuffercnt;	// count of chars to xmit
    int xmitrepeatcnt;	// retains count if repeat is on
	unsigned char *xmitbufferptr;	// ptr to buffer of chars to xmit
    unsigned char xmit_buf[MAXUSBDATA];	// buffer for xmitting to controller
};

// endpoint type setting
enum Endpoint_Type
{
	USB_DISABLE_PIPE = 0,
	USB_ISOC_PIPE,
	USB_NISOC_PIPE,
	USB_CTRL_PIPE
};

extern unsigned char USB_INTR_FLAG[];
extern struct EndpointSetup Endpoints[MAXUSBBRDS+1][USB_MAX_EP]; // array of endpoint structs
extern unsigned char new_usb_addr;
extern unsigned char device_status;

//-----------------------PUBLIC STRUCTURE DEFINITIONS--------------------------

//------------------------PUBLIC FUNCTION PROTOTYPES---------------------------
extern void Sim_Init(void);
extern unsigned char USB_Init(unsigned char);
extern void interrupt USB_Intr_Brd1();
extern void interrupt USB_Intr_Brd2();
extern void interrupt USB_Intr_Brd3();
extern void interrupt USB_Intr_Brd4();
extern void USB_Intr_Handler(unsigned char);
extern void USB_SetupEP(unsigned char, unsigned char);
extern void USB_Connect(unsigned char);
extern void USB_Disconnect(unsigned char);
extern void USB_Stall(unsigned char, unsigned char);
extern void USB_UnStall(unsigned char, unsigned char);
extern void Check_USB_Xmit(unsigned char, unsigned char);
extern void USB_Set_Address(unsigned char,unsigned char);
extern unsigned char Ether_Init(void);
extern unsigned char Open_USB_Socket(int);
extern unsigned char Socket_Poll(int);
extern void Process_Socket_Out(int);

#endif