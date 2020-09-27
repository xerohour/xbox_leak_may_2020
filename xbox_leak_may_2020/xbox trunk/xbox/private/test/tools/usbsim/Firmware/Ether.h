//*****************************************************************************
//	    Filename: Ether.h
//       Project: Microsoft USB Simulator
//	   Copyright: INDesign-LLC, 2000
//
//   Description: This is the header file for the ether module.  It contains
//				  information supporting communications thru the network
//                channel.
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/Ether.h $
// $Revision: 1.16 $
// $Date: 2001/02/15 14:01:01Z $ 
// $Author: klpayne $
// $Log: Ether.h $
// Revision 1.16  2001/02/15 14:01:01Z  klpayne
// Add support for GPIO set/response commands
// Revision 1.15  2001/02/07 15:37:36Z  rjgardner
// Change number of MAC address bytes retrieved as per MS request.
// Revision 1.14  2001/01/25 15:02:58Z  rjgardner
// Remove public function prototype declarations
// Revision 1.13  2001/01/12 20:46:36Z  rjgardner
// Add defines for USB socket reset command
// Revision 1.12  2001/01/10 20:44:56Z  rjgardner
// Return version number size to 5 bytes as per specification.
// Revision 1.11  2001/01/09 21:29:27Z  rjgardner
// Combine IP and status response buffer size define into a single
// define RESPONSESIZE.  Add a size define for the device status
// data length DEVSTATSIZE.
// Revision 1.10  2001/01/03 21:46:49Z  rjgardner
// Add prototype for Open_USB_Socket
// Revision 1.9  2000/12/28 18:12:33Z  rjgardner
// Add MAC address constants
// Revision 1.8  2000/12/22 20:55:27Z  rjgardner
// Add defines for response to status commands from the controller.
// Revision 1.7  2000/12/19 19:57:50Z  rjgardner
// Add new constant IPRESPONSEOFFSET
// Revision 1.6  2000/12/19 18:13:23Z  rjgardner
// Remove global variable extern declarations
// Revision 1.5  2000/12/18 21:52:40Z  rjgardner
// Add support for new functions Process_Connect,
// Process_Socket_In and Process_Socket_Out.
// Revision 1.4  2000/12/16 18:41:14Z  rjgardner
// Add updates for Ether.c revision.
// Revision 1.3  2000/12/14 21:22:34Z  rjgardner
// Update for new Ether.c
// Revision 1.2  2000/12/13 22:30:04Z  rjgardner
// Remove unnecessary definitions.
// Revision 1.1  2000/12/06 19:18:17Z  rjgardner
// Initial revision
//
//*****************************************************************************
#ifndef ETHER_H
#define ETHER_H

//-----------------------PUBLIC CONSTANTS AND MACROS---------------------------
// Version Request command for NETSOCK
#define VREQUEST    0x101

// UDP Port Defines
#define CONTROLPORT 200
#define USB1PORT    201
#define USB2PORT    202
#define USB3PORT    203
#define USB4PORT    204

// Datagram size defines
#define MAXDGRAMSIZE		1024
#define RESPONSESIZE		16

// IP request command defines
#define IPREQUESTSIZE		5
#define IPADDRSIZE			4
#define IPREQUESTCMD		3
#define IPREQUESTSCMD		0

// Status command defines
#define STATUSCOMMAND		4
#define STATUSCMDSIZE		5

// Status subcommand defines
#define VERSIONSIZE 		5
#define MACADDRSIZE			6
#define DEVSTATSIZE			1

// Valid status subcommands
enum subcommands {
	VERSION = 1,
	MACADDR,
	DEVSTATUS
};

// Reset command defines
#define RESETCMD			5
#define RESETSCMD			4
#define RESETCMDSIZE		5

// GPIO set command defines
#define GPIOSETCMDSIZE 		6
#define GPIOSETCMD			5
#define GPIOSETSCMD			5

// GPIO request command defines
#define GPIOREQCMDSIZE		5
#define GPIOREQCMD			5
#define GPIOREQSCMD			6
#define GPIORESPSIZE		1

// Define socket channel designators
enum sockets {
	CNTRL = 0,
	USB1,
	USB2,
	USB3,
	USB4
};

// MAC address defines
#define MAXLINELEN	80
#define MACOFFSET	23

//-------------------------EXTERN PUBLIC VARIABLES-----------------------------

//-----------------------PUBLIC STRUCTURE DEFINITIONS--------------------------

//------------------------PUBLIC FUNCTION PROTOTYPES---------------------------

#endif