//****************************************************************************
//	    Filename: buffers.h
//       Project: USB Simulator
//	   Copyright: Microsoft, 2000
//
//   Description: This is the header file for the buffers module.  It contains
//				  information supporting buffers structures between the network
//                channel and the USB ports.
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/buffers.h $
// $Revision: 1.7 $
// $Date: 2001/03/23 20:03:17Z $ 
// $Author: klpayne $
// $Log: buffers.h $
// Revision 1.7  2001/03/23 20:03:17Z  klpayne
// Reduced buffer size, added more elements
// Revision 1.6  2001/01/29 16:07:30Z  klpayne
// Change xmit/recv buffers to allow 1024 size.
// Revision 1.5  2001/01/10 20:47:28Z  klpayne
// Reduce buffer element size, increase number of them
// Revision 1.4  2001/01/09 21:48:11Z  rjgardner
// Increase size of buffers (MAX_BUF_SIZE) to 1024
// Revision 1.3  2001/01/04 15:35:46Z  klpayne
// Interim checkin
// Revision 1.2  2000/12/19 13:16:12Z  klpayne
// Initial implementation of buffers.
// Revision 1.1  2000/12/06 19:40:01Z  rjgardner
// Initial revision
//
//****************************************************************************
#ifndef BUFFERS_H
#define BUFFERS_H

//-----------------------PUBLIC CONSTANTS AND MACROS---------------------------
#define MAX_BUF_SIZE	512	// maximum size of a buffer
#define NUM_XMIT_ELEMENTS	20		// maximum number of queues in the transmit array
#define NUM_RCV_ELEMENTS	10		// maximum number of queues in the receive array

enum _BUF_INDEX	// note, control channel (200) is handled directly and doesn't need a buffer 
{
	SOCK_201_XMIT,
	SOCK_201_RCV,
	SOCK_202_XMIT,
	SOCK_202_RCV,
	SOCK_203_XMIT,
	SOCK_203_RCV,
	SOCK_204_XMIT,
	SOCK_204_RCV,
    NUM_BUFFERS
};

//-------------------------EXTERN PUBLIC VARIABLES-----------------------------

//-----------------------PUBLIC STRUCTURE DEFINITIONS--------------------------
struct data_element {
	unsigned int size;
	unsigned char data[MAX_BUF_SIZE];
} ;

struct xmit_buffer_queue {
	struct data_element element[NUM_XMIT_ELEMENTS];	// array of data buffer structs 
	unsigned char inindx;	// index used to track putting data into array
	unsigned char outindx;	// index used to track taking data out of array
};

struct rcv_buffer_queue {
	struct data_element element[NUM_RCV_ELEMENTS];	// array of data buffer structs 
	unsigned char inindx;	// index used to track putting data into array
	unsigned char outindx;	// index used to track taking data out of array
};
//------------------------PUBLIC FUNCTION PROTOTYPES---------------------------
extern void Init_Buffers(void);
extern void Put_Buffer(unsigned char buf, unsigned char *ptr, unsigned int size);
extern unsigned int Get_Buffer(unsigned char buf, unsigned char *ptr);

#endif