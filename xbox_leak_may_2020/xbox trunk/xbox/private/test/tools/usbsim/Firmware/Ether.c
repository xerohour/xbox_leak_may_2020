//*****************************************************************************
//	    Filename: Ether.c
//       Project: Microsoft USB Simulator
//	   Copyright: INDesign-LLC, 2000
//
//	 Description:
//
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/Ether.c $
// $Revision: 1.24 $
// $Date: 2001/03/01 13:25:13Z $ 
// $Author: klpayne $
// $Log: Ether.c $
// Revision 1.24  2001/03/01 13:25:13Z  klpayne
// Change handling of IP xmit to loop until buffer empty of messages
// Revision 1.23  2001/02/15 14:00:43Z  klpayne
// Add support for GPIO set/response commands
// Revision 1.22  2001/02/07 15:50:16Z  rjgardner
// Add finer definitions for ethernet debug messages.  Debug options
// are turned off.
// Revision 1.21  2001/01/25 17:50:46Z  rjgardner
// Remove function Process_Connect and add funtionality into function
// Process_Control_Msg.  Remove function Process_Socket_In and
// add functionality into function Socket_Poll.
// Revision 1.20  2001/01/12 20:47:54Z  rjgardner
// Add new function Simulator_Reset to handle USB channel reset
// command from controller.
// Revision 1.19  2001/01/11 13:41:43Z  rjgardner
// Remove include for version.h and add declaration of extern const
// unsigned char Version[].
// Revision 1.18  2001/01/09 21:30:34Z  rjgardner
// Change response message header from static strings to dynamic
// build to simplify code.
// Revision 1.17  2001/01/08 14:51:15Z  rjgardner
// Add extra debug define to Socket_Poll to eliminate compiler warning
// when debug is turned off.
// Revision 1.16  2001/01/08 14:46:04Z  rjgardner
// Turn off expanded debug messages for release version.
// Revision 1.15  2001/01/05 19:10:08Z  rjgardner
// Give each USB channel its own socket address structure.
// Revision 1.14  2001/01/05 16:14:18Z  rjgardner
// Update processing of USB sockets.
// Revision 1.13  2001/01/05 14:46:50Z  rjgardner
// Update socket Open and Bind for remaining USB channels
// Revision 1.12  2001/01/03 21:46:10Z  rjgardner
// Add new function Open_USB_Socket and support for all four USB
// channels
// Revision 1.11  2000/12/28 21:46:04Z  rjgardner
// Change data size field of MAC response header to be four rather
// than six characters long as per flow document.
// Revision 1.10  2000/12/28 18:14:23Z  rjgardner
// Add new function Get_MAC_Addr to parse MAC address from the
// MAC.TXT file.
// Revision 1.9  2000/12/22 20:54:22Z  rjgardner
// Add logic to respond to status requests from controller.
// Revision 1.8  2000/12/19 20:35:03Z  rjgardner
// Consolidate controller IP address into single variable
// Revision 1.7  2000/12/19 19:57:05Z  rjgardner
// Change destination IP address on USB1 output messages
// Revision 1.6  2000/12/19 18:12:36Z  rjgardner
// Add input and output process on USB1
// Revision 1.5  2000/12/18 21:51:23Z  rjgardner
// Add functions for Process_Connect, Process_Socket_In and
// Process_Socket_Out 
// Revision 1.4  2000/12/16 18:40:37Z  rjgardner
// Consolidate function Check_Msg with function Socket_Poll.
// Revision 1.3  2000/12/14 21:21:40Z  rjgardner
// Add Socket_Poll function
// Revision 1.2  2000/12/13 22:29:08Z  rjgardner
// Add functions Socket_Open(), Socket_Bind() and Check_Msg().
// Revision 1.1  2000/12/06 19:18:02Z  rjgardner
// Initial revision
//
//*****************************************************************************

#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include "sim.h"
#define NETSOCK_MASTER
#include "netsock.h"
#include "Ether.h"
#include "buffers.h"

//------------------------LOCAL CONSTANTS AND MACROS---------------------------

// NOTE: enable DEBUG in addition to any of the DEBUG_USBx defines
//#define DEBUG
//#define DEBUG_CNTRL
//#define DEBUG_USB1
//#define DEBUG_USB2
//#define DEBUG_USB3
//#define DEBUG_USB4

//-----------------------------GLOBAL VARIABLES--------------------------------

// Netsock data structure
static WSAData SocketData;

// Control socket variables
static SOCKET  CtrlSocket;
static struct sockaddr_in cntlr_ip;

// USB1 socket variables
static SOCKET  USB1Socket;
static struct sockaddr_in usb1_ip;

// USB2 socket variables
static SOCKET  USB2Socket;
static struct sockaddr_in usb2_ip;

// USB3 socket variables
static SOCKET  USB3Socket;
static struct sockaddr_in usb3_ip;

// USB4 socket variables
static SOCKET  USB4Socket;
static struct sockaddr_in usb4_ip;

// IP and MAC address storage variables
static unsigned char ipaddr[IPADDRSIZE];
static unsigned char macaddr[MACADDRSIZE];

// Declare access to version string
extern const unsigned char Version[];

//----------------------STATIC LOCAL FUNCTION PROTOTYPES-----------------------

unsigned char Socket_Open(int);
unsigned char Socket_Bind(int);
unsigned char Process_Control_Msg(unsigned char *, int);
void Get_MAC_Addr(void);
void Simulator_Reset(void);

// The following are public functions for Network access
//
//*****************************************************************************
//
//	   FUNCTION: unsigned char Ether_Init(void)
//	DESCRIPTION: This function initializes the Ethernet channel.
//
//	     INPUTS: None			
//	    RETURNS: TRUE if Ethernet channel could be initialized, FALSE if not		
//        NOTES: This function should be used to initialize the network connection.
//
//*****************************************************************************
unsigned char Ether_Init(void) {
    int err, i;
	struct NetsockConfig cfg;
	unsigned char status = TRUE;

	// GetMAC address
	Get_MAC_Addr();
	
	// Initiate Embedded Netsock stack and start underlying layers
	err = WSAStartup(VREQUEST, &SocketData);

#ifdef DEBUG_CNTRL
	// Display diagnostics
	switch(EmbeddedNetsockLoadError) {
		case 0:
			printf("Successful Netsock initialization!\n");
			break;
		case ENE_LDERR_BIOS:
			printf("System BIOS does not support Embedded Netsock!\n");
			break;
		case ENE_LDERR_ADAPTER:
			printf("No Network adapter found!\n");
			break;
		case ENE_LDERR_MEM:
			printf("Error allocating memory!\n");
			break;
		case ENE_LDERR_NETSOCK:
			printf("Netsock not available!\n");
			break;
	}
#endif

	// Check for stack startup errors
	if(err) {
		printf("WSAStartup failed with error %d\n", err);
		WSACleanup();	// Shut down Embedded Netsock
		status = FALSE;	// Return failure code
	} else {
		// Get our IP address
		ENgetnetconfig(&cfg, sizeof(cfg));

		// Save IP address
		for(i =0; i < IPADDRSIZE; i++) {
			ipaddr[i] = cfg.IPAddr[i];
		}
	}

	// Open control socket
	err = Socket_Open(CNTRL);

	// Check for control socket open errors
	if(err == FALSE) {
		printf("Control Socket_Open failed\n");
		WSACleanup();	// Shut down Embedded Netsock
		status = FALSE;	// Return failure code
	}

	// Bind control socket
	err = Socket_Bind(CNTRL);

	// Check for control socket bind errors
	if(err == FALSE) {
		printf("Control Socket_Bind failed\n");
		WSACleanup();	// Shut down Embedded Netsock
		status = FALSE;	// Return failure code
	}

	// Return status of network stack and control socket startup
	return(status);
}

//*****************************************************************************
//
//	   FUNCTION: unsigned char Socket_Open(int netsock)
//	DESCRIPTION: This function opens the requested socket.
//
//	     INPUTS: Socket Identifier	
//	    RETURNS: TRUE if socket could be allocated, FALSE if not		
//        NOTES: This function should be used to open a network socket.
//
//*****************************************************************************
unsigned char Socket_Open(int netsock) {
	unsigned char status = TRUE;

	// Open the selected network socket
	switch(netsock) {
		case CNTRL:
			// Allocate the requested socket
			CtrlSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			// Check for command socket error
			if(CtrlSocket == SOCKET_ERROR) {
				printf("Error %d from CmdSocket\n", CtrlSocket);
				status = FALSE;	// Return failure code
			}
			break;

		case USB1:
			// Allocate the requested socket
			USB1Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			// Check for USB1 socket error
			if(USB1Socket == SOCKET_ERROR) {
				printf("Error %d from USB1Socket\n", USB1Socket);
				status = FALSE;	// Return failure code
			}
			break;

		case USB2:
			// Allocate the requested socket
			USB2Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			// Check for USB2 socket error
			if(USB2Socket == SOCKET_ERROR) {
				printf("Error %d from USB2Socket\n", USB2Socket);
				status = FALSE;	// Return failure code
			}
			break;

		case USB3:
			// Allocate the requested socket
			USB3Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			// Check for USB3 socket error
			if(USB3Socket == SOCKET_ERROR) {
				printf("Error %d from USB3Socket\n", USB3Socket);
				status = FALSE;	// Return failure code
			}
			break;

		case USB4:
			// Allocate the requested socket
			USB4Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			// Check for USB4 socket error
			if(USB4Socket == SOCKET_ERROR) {
				printf("Error %d from USB4Socket\n", USB4Socket);
				status = FALSE;	// Return failure code
			}
			break;

		default:
			break;
	}

	// Return status of socket allocation
	return(status);
}

//*****************************************************************************
//
//	   FUNCTION: unsigned char Socket_Bind(int netsock)
//	DESCRIPTION: This function binds the requested socket.
//
//	     INPUTS: Socket Identifier	
//	    RETURNS: TRUE if socket bind successful, FALSE if not		
//        NOTES: This function should be used to bind a network socket.
//
//*****************************************************************************
unsigned char Socket_Bind(int netsock) {
	int err;
	unsigned char status = TRUE;
	struct sockaddr_in local;

	// Init common variable in local structure for all sockets
	local.sin_family = AF_INET;

	// Bind the selected network socket
	switch(netsock) {
		case CNTRL:
			// Init socket specific variable in command socket local structure
			local.sin_port = htons(CONTROLPORT);

			// Bind command socket
			err = bind(CtrlSocket, &local, sizeof(local));
			break;

		case USB1:
			// Init socket specific variable in USB1 socket local structure
			local.sin_port = htons(USB1PORT);

			// Bind USB1 socket
			err = bind(USB1Socket, &local, sizeof(local));
			break;

		case USB2:
			// Init socket specific variable in USB2 socket local structure
			local.sin_port = htons(USB2PORT);

			// Bind USB2 socket
			err = bind(USB2Socket, &local, sizeof(local));
			break;

		case USB3:
			// Init socket specific variable in USB3 socket local structure
			local.sin_port = htons(USB3PORT);

			// Bind USB3 socket
			err = bind(USB3Socket, &local, sizeof(local));
			break;

		case USB4:
			// Init socket specific variable in USB4 socket local structure
			local.sin_port = htons(USB4PORT);

			// Bind USB4 socket
			err = bind(USB4Socket, &local, sizeof(local));
			break;

		default:
			break;
	}

	// Check for bind error
	if(err == SOCKET_ERROR) {
		printf("Error binding socket: Error %d\n", WSAGetLastError());
		status = FALSE;	// Return failure code
	}

	// Return status of socket allocation
	return(status);
}

//*****************************************************************************
//
//	   FUNCTION: unsigned char Socket_Poll(int netsock)
//	DESCRIPTION: This function checks for and retrieves a message on the
//               requested socket if available.
//
//	     INPUTS: Socket Identifier	
//	    RETURNS: TRUE if control socket connect, FALSE if not		
//        NOTES: this function should be used to check for socket datagrams.
//
//*****************************************************************************
unsigned char Socket_Poll(int netsock) {
	unsigned char status = FALSE;
	char datagram[MAXDGRAMSIZE];
	int err, fromlen, bytecnt;
	unsigned long messagesize;
#ifdef DEBUG
	int i;
#endif

	// Process selected socket
	switch(netsock) {
		case CNTRL:
			// Check for message on control socket
			err = ioctlsocket(CtrlSocket, FIONREAD, &messagesize);

			// Check for socket read error
			if(err == SOCKET_ERROR) {
				printf("ioctlsocket failed with error %d\n", WSAGetLastError());
			} else { // No socket read error
				if(messagesize) { // Check if message available
		
					// Retrieve datagram
					fromlen = sizeof(cntlr_ip);
					bytecnt = recvfrom(CtrlSocket, datagram, sizeof(datagram), FLAGS_ZERO,
			                                &cntlr_ip, (int far *) &fromlen);

					// Check for socket read error
					if(bytecnt == SOCKET_ERROR) {
						printf("ioctlsocket on CNTRL failed with error %d\n", WSAGetLastError());
					} else { // Process message

#ifdef DEBUG_CNTRL
						printf("\nControl socket message = ");
						for(i = 0; i < bytecnt; i++) {
							printf(" %0.2x", datagram[i]);
						}
						printf("\n");
#endif		

						// Pass on control channel connect flag
						status = Process_Control_Msg((unsigned char *)&datagram, bytecnt);
					}
				}
			}		
			break;

		case USB1:
			// process only if USB1 is connected
			if(device_status & USB1_CONNECT) {
				// Check for message on USB1 socket
				err = ioctlsocket(USB1Socket, FIONREAD, &messagesize);

				// Check for socket read error
				if(err == SOCKET_ERROR) {
					printf("ioctlsocket on USB1 failed with error %d\n", WSAGetLastError());
				} else { // No socket read error
					if(messagesize) { // Check if message available
		
						// Retrieve datagram
						fromlen = sizeof(usb1_ip);
						bytecnt = recvfrom(USB1Socket, datagram, sizeof(datagram), FLAGS_ZERO,
			                                   &usb1_ip, (int far *) &fromlen);

						// Check for socket read error
						if(bytecnt == SOCKET_ERROR) {
							printf("recvfrom on USB1 failed with error %d\n", WSAGetLastError());
						} else { // Route message

#ifdef DEBUG_USB1
						    printf("\nUSB1 socket message = ");
						    for(i = 0; i < bytecnt; i++) {
							    printf(" %0.2x", datagram[i]);
						    }
						    printf("\n");
#endif		

							Put_Buffer(SOCK_201_RCV, (unsigned char *)&datagram, bytecnt);
						}
					}
				}
			}		
			break;

		case USB2:
			// process only if USB2 is connected
			if(device_status & USB2_CONNECT) {
				// Check for message on USB2 socket
				err = ioctlsocket(USB2Socket, FIONREAD, &messagesize);

				// Check for socket read error
				if(err == SOCKET_ERROR) {
					printf("ioctlsocket on USB2 failed with error %d\n", WSAGetLastError());
				} else { // No socket read error
					if(messagesize) { // Check if message available
		
						// Retrieve datagram
						fromlen = sizeof(usb2_ip);
						bytecnt = recvfrom(USB2Socket, datagram, sizeof(datagram), FLAGS_ZERO,
			                                   &usb2_ip, (int far *) &fromlen);

						// Check for socket read error
						if(bytecnt == SOCKET_ERROR) {
							printf("recvfrom on USB2 failed with error %d\n", WSAGetLastError());
						} else { // Route message

#ifdef DEBUG_USB2
						    printf("\nUSB2 socket message = ");
						    for(i = 0; i < bytecnt; i++) {
							    printf(" %0.2x", datagram[i]);
						    }
						    printf("\n");
#endif		

							Put_Buffer(SOCK_202_RCV, (unsigned char *)&datagram, bytecnt);
						}
					}
				}
			}		
			break;

		case USB3:
			// process only if USB3 is connected
			if(device_status & USB3_CONNECT) {
				// Check for message on USB3 socket
				err = ioctlsocket(USB3Socket, FIONREAD, &messagesize);

				// Check for socket read error
				if(err == SOCKET_ERROR) {
					printf("ioctlsocket on USB3 failed with error %d\n", WSAGetLastError());
				} else { // No socket read error
					if(messagesize) { // Check if message available
		
						// Retrieve datagram
						fromlen = sizeof(usb3_ip);
						bytecnt = recvfrom(USB3Socket, datagram, sizeof(datagram), FLAGS_ZERO,
			                                   &usb3_ip, (int far *) &fromlen);

						// Check for socket read error
						if(bytecnt == SOCKET_ERROR) {
							printf("recvfrom on USB3 failed with error %d\n", WSAGetLastError());
						} else { // Route message

#ifdef DEBUG_USB3
						    printf("\nUSB3 socket message = ");
						    for(i = 0; i < bytecnt; i++) {
							    printf(" %0.2x", datagram[i]);
						    }
						    printf("\n");
#endif		

							Put_Buffer(SOCK_203_RCV, (unsigned char *)&datagram, bytecnt);
						}
					}
				}
			}		
			break;

		case USB4:
			// process only if USB4 is connected
			if(device_status & USB4_CONNECT) {
				// Check for message on USB4 socket
				err = ioctlsocket(USB4Socket, FIONREAD, &messagesize);

				// Check for socket read error
				if(err == SOCKET_ERROR) {
					printf("ioctlsocket on USB4 failed with error %d\n", WSAGetLastError());
				} else { // No socket read error
					if(messagesize) { // Check if message available
		
						// Retrieve datagram
						fromlen = sizeof(usb4_ip);
						bytecnt = recvfrom(USB4Socket, datagram, sizeof(datagram), FLAGS_ZERO,
			                                   &usb4_ip, (int far *) &fromlen);

						// Check for socket read error
					   	if(bytecnt == SOCKET_ERROR) {
							printf("ioctlsocket on USB4 failed with error %d\n", WSAGetLastError());
						} else { // Route message

#ifdef DEBUG_USB4
						    printf("\nUSB4 socket message = ");
						    for(i = 0; i < bytecnt; i++) {
							    printf(" %0.2x", datagram[i]);
						    }
						    printf("\n");
#endif		

							Put_Buffer(SOCK_204_RCV, (unsigned char *)&datagram, bytecnt);
						}
					}
				}
			}		
			break;

		default:
			break;
	}

	// Return socket read status
	return(status);
}

//*****************************************************************************
//
//	   FUNCTION: unsigned char Open_USB_Socket(int netsock)
//	DESCRIPTION: This function opens the requested USB socket if available.
//
//	     INPUTS: Socket Identifier	
//	    RETURNS: TRUE if socket open successful, FALSE if not		
//        NOTES: this function should be used to open USB socket ports.
//
//*****************************************************************************
unsigned char Open_USB_Socket(int netsock) {
	int err;
	unsigned char status = TRUE;

	// Process selected USB socket
	err = Socket_Open(netsock);

	// Check for USB socket open errors
	if(err == FALSE) {
		printf("Socket_Open on USB%d failed\n", netsock);
		status = FALSE;	// Return failure code
	}

	// Bind USB socket
	err = Socket_Bind(netsock);

	// Check for USB socket bind errors
	if(err == FALSE) {
		printf("Socket_Bind on USB%d failed\n", netsock);
		status = FALSE;	// Return failure code
	}

	// Return socket status
	return(status);
}

//*****************************************************************************
//
//	   FUNCTION: void Process_Socket_Out(int netsock)
//	DESCRIPTION: This function checks for outgoing messages to the controller.
//
//	     INPUTS: Socket Identifier	
//	    RETURNS: None		
//        NOTES: This function check for data to send out selected socket.
//
//*****************************************************************************
void Process_Socket_Out(int netsock) {
	unsigned char outbuf[MAXDGRAMSIZE];
	int err = 0, outbufsize = 0;

	// Handle each socket
	switch(netsock) {
		case USB1:
			// process only if USB1 is connected
			if(device_status & USB1_CONNECT) {
				// Check for USB1 output data
				outbufsize = Get_Buffer(SOCK_201_XMIT, (unsigned char *)&outbuf);
                err = 0;

				// Send out message if available
                while((outbufsize) && (err != SOCKET_ERROR)) {  // keep sending till buffer empty or error
					err = sendto(USB1Socket, (char *)&outbuf, outbufsize, FLAGS_ZERO,
		                                             &usb1_ip, sizeof(usb1_ip));
    				outbufsize = Get_Buffer(SOCK_201_XMIT, (unsigned char *)&outbuf);
				}
			}
			break;

		case USB2:
			// process only if USB2 is connected
			if(device_status & USB2_CONNECT) {
				// Check for USB1 output data
				outbufsize = Get_Buffer(SOCK_202_XMIT, (unsigned char *)&outbuf);
                err = 0;

				// Send out message if available
                while((outbufsize) && (err != SOCKET_ERROR)) {  // keep sending till buffer empty or error
					err = sendto(USB2Socket, (char *)&outbuf, outbufsize, FLAGS_ZERO,
		                                             &usb2_ip, sizeof(usb2_ip));
    				outbufsize = Get_Buffer(SOCK_202_XMIT, (unsigned char *)&outbuf);
				}
			}
			break;

		case USB3:
			// process only if USB3 is connected
			if(device_status & USB3_CONNECT) {
				// Check for USB3 output data
				outbufsize = Get_Buffer(SOCK_203_XMIT, (unsigned char *)&outbuf);
                err = 0;

				// Send out message if available
                while((outbufsize) && (err != SOCKET_ERROR)) {  // keep sending till buffer empty or error
					err = sendto(USB3Socket, (char *)&outbuf, outbufsize, FLAGS_ZERO,
		                                             &usb3_ip, sizeof(usb3_ip));
    				outbufsize = Get_Buffer(SOCK_203_XMIT, (unsigned char *)&outbuf);
				}
			}
			break;

		case USB4:
			// process only if USB4 is connected
			if(device_status & USB4_CONNECT) {
				// Check for USB4 output data
				outbufsize = Get_Buffer(SOCK_204_XMIT, (unsigned char *)&outbuf);
                err = 0;

				// Send out message if available
                while((outbufsize) && (err != SOCKET_ERROR)) {  // keep sending till buffer empty or error
					err = sendto(USB4Socket, (char *)&outbuf, outbufsize, FLAGS_ZERO,
		                                             &usb4_ip, sizeof(usb4_ip));
    				outbufsize = Get_Buffer(SOCK_204_XMIT, (unsigned char *)&outbuf);
				}
			}
			break;

		default:
			break;
	}

	// Check for socket error
	if(err == SOCKET_ERROR) {
		printf("Process_Socket_Out: USB%d Error %d\n", netsock, WSAGetLastError());
	}
}

//*****************************************************************************
//
//	   FUNCTION: unsigned char Process_Control_Msg(void)
//	DESCRIPTION: This function processes command messages from the controller.
//
//	     INPUTS: None	
//	    RETURNS: TRUE if IP request message, FALSE otherwise		
//        NOTES: This function processes command socket messages.
//
//*****************************************************************************
unsigned char Process_Control_Msg(unsigned char *cntrlbuf, int cntrlbytecnt) {
	unsigned char connect = FALSE;
	unsigned char command = cntrlbuf[0];
	unsigned char subcommand = cntrlbuf[1];
	unsigned char responsebuf[RESPONSESIZE];
	int err, i, responsebufsize = 0;
    unsigned char temp;

	// Is valid IP request?
	if((cntrlbytecnt == IPREQUESTSIZE) && (command == IPREQUESTCMD) && (subcommand == IPREQUESTSCMD)) {
		// Load IP response header
		responsebuf[responsebufsize++] = IPREQUESTCMD;
		responsebuf[responsebufsize++] = IPREQUESTSCMD;
		responsebuf[responsebufsize++] = NULL;
		responsebuf[responsebufsize++] = IPADDRSIZE;
		responsebuf[responsebufsize++] = NULL;

		// Load IP message data
		for(i = 0; i < IPADDRSIZE; i++) {
			responsebuf[responsebufsize++] = ipaddr[i];
		}

		// Flag as connect request
		connect = TRUE;
	}

	// Is valid status command?
	if((cntrlbytecnt == STATUSCMDSIZE) && (command == STATUSCOMMAND)) {
		// Handle subcommand
		switch(subcommand) {
			// Version request
			case VERSION:
				// Load version response message header
				responsebuf[responsebufsize++] = STATUSCOMMAND;
				responsebuf[responsebufsize++] = VERSION;
				responsebuf[responsebufsize++] = NULL;
				responsebuf[responsebufsize++] = VERSIONSIZE;
				responsebuf[responsebufsize++] = NULL;

				// Load version response message data
				for(i = 0; i < VERSIONSIZE; i++) {
					responsebuf[responsebufsize++] = Version[i];
				}
				break;

			// MAC address request
			case MACADDR:
				// Load MAC address response message header
				responsebuf[responsebufsize++] = STATUSCOMMAND;
				responsebuf[responsebufsize++] = MACADDR;
				responsebuf[responsebufsize++] = NULL;
				responsebuf[responsebufsize++] = MACADDRSIZE;
				responsebuf[responsebufsize++] = NULL;

				// Load MAC address response message data
				for(i = 0; i < MACADDRSIZE; i++) {
					responsebuf[responsebufsize++] = macaddr[i];
				}
				break;

			// Device status request
			case DEVSTATUS:
				// Load device status response message header
				responsebuf[responsebufsize++] = STATUSCOMMAND;
				responsebuf[responsebufsize++] = DEVSTATUS;
				responsebuf[responsebufsize++] = NULL;
				responsebuf[responsebufsize++] = DEVSTATSIZE;
				responsebuf[responsebufsize++] = NULL;

				// Load device status response message data
				responsebuf[responsebufsize++] = device_status;
				break;

			// Invalid subcommand
			default:
				break;
		}
	}

	// Is valid reset request?
	if((cntrlbytecnt == RESETCMDSIZE) && (command == RESETCMD) && (subcommand == RESETSCMD)) {
		// Load reset device response message header
		responsebuf[responsebufsize++] = RESETCMD;
		responsebuf[responsebufsize++] = RESETSCMD;
		responsebuf[responsebufsize++] = NULL;
		responsebuf[responsebufsize++] = DEVSTATSIZE;
		responsebuf[responsebufsize++] = NULL;

		// Reset simulator
		Simulator_Reset();
		
		// Load reset device status response message data
		responsebuf[responsebufsize++] = device_status;
	}

	// Is valid GPIO set request?
	if((cntrlbytecnt == GPIOSETCMDSIZE) && (command == GPIOSETCMD) && (subcommand == GPIOSETSCMD)) {
        temp = inp(GPIO_PORTB);
        temp &= 0x0F;   // keep ready LED settings
        temp |= (cntrlbuf[5] << 4);
        outp(GPIO_PORTB,temp);  // output new setting
    }

	// Is valid GPIO request?
	if((cntrlbytecnt == GPIOREQCMDSIZE) && (command == GPIOREQCMD) && (subcommand == GPIOREQSCMD)) {
		// Load GPIO response message header
		responsebuf[responsebufsize++] = GPIOREQCMD;
		responsebuf[responsebufsize++] = GPIOREQSCMD;
		responsebuf[responsebufsize++] = NULL;
		responsebuf[responsebufsize++] = GPIORESPSIZE;
		responsebuf[responsebufsize++] = NULL;

		// Load GPIO Port C response message data
		responsebuf[responsebufsize++] = inp(GPIO_PORTC);
	}

	// Check if response message to send
	if(responsebufsize) {
		// Send response
		err = sendto(CtrlSocket, (char *)&responsebuf, responsebufsize, FLAGS_ZERO,
			                             &cntlr_ip, sizeof(cntlr_ip));
#ifdef DEBUG_CNTRL
		printf("Response msg =");
		for(i = 0; i < responsebufsize; i++) {
			printf(" %x", responsebuf[i]);
		}
		printf("\n");
#endif

		// Check for socket error
		if(err == SOCKET_ERROR) {
			printf("Process_Control_Msg: Error %d\n", WSAGetLastError());
		}
	}

	// Return connect indication
	return(connect);
}

//*****************************************************************************
//
//	   FUNCTION: void Get_MAC_Addr(void)
//	DESCRIPTION: This function parses MAC.TXT file for MAC address.
//
//	     INPUTS: None	
//	    RETURNS: None		
//        NOTES: This function parses the MAC address.
//
//*****************************************************************************
void Get_MAC_Addr(void) {
	FILE *fp;
	unsigned char ch;
	char *chp;
	char line[MAXLINELEN];
	int i, j;

	// Open and scan MAC.TXT file for MAC address line
	chp = line;
	fp = fopen("MAC.TXT", "r");
	do {
		fgets(line, MAXLINELEN, fp);
	} while(*chp != 'M');
	fclose(fp);

	// Parse out MAC address
	chp = &line[MACOFFSET];
	for(i = 0; i < MACADDRSIZE; i++) { // Cycle through required number of bytes
		for(j = 0; j < 2; j++) {	// Cycle through two characters for each byte
			ch = *chp;				// Get nybble character
			if(isalpha(ch)) {		// Check if alpha character
				ch = toupper(ch);	// Make sure it is upper case
				ch -= 0x37;			// Subtract ascii hex alpha offset
			} else {				// Convert digit character
				ch -= 0x30;			// Subtract ascii hex digit offset
			}
			if(j == 0) {			// Check if most significant nybble
				ch = ch << 4;		// Shift nybble into position
				macaddr[i] = ch;	// Save first nybble
				chp++;				// Go to next nybble
			} else {
				macaddr[i] |= ch;	// Or in second nybble
				chp += 2;			// Skip over ':' to next digit
			}
		}
	}
}

//*****************************************************************************
//
//	   FUNCTION: void Simulator_Reset(void)
//	DESCRIPTION: This function closes USB sockets then resets each USB board.
//
//	     INPUTS: None	
//	    RETURNS: None		
//        NOTES: This function resets the simulator USB channels.
//
//*****************************************************************************
void Simulator_Reset(void) {
	int err;

	// close only if USB1 is connected
	if(device_status & USB1_CONNECT) {
		// Close USB1 socket
		err = closesocket(USB1Socket);

		// Check for socket close error
		if(err == SOCKET_ERROR) {
			printf("Simulator_Reset USB1: Error %d\n", WSAGetLastError());
		}
	}

	// close only if USB2 is connected
	if(device_status & USB2_CONNECT) {
		// Close USB2 socket
		err = closesocket(USB2Socket);

		// Check for socket close error
		if(err == SOCKET_ERROR) {
			printf("Simulator_Reset USB2: Error %d\n", WSAGetLastError());
		}
	}

	// close only if USB3 is connected
	if(device_status & USB3_CONNECT) {
		// Close USB3 socket
		err = closesocket(USB3Socket);

		// Check for socket close error
		if(err == SOCKET_ERROR) {
			printf("Simulator_Reset USB3: Error %d\n", WSAGetLastError());
		}
	}

	// close only if USB4 is connected
	if(device_status & USB4_CONNECT) {
		// Close USB4 socket
		err = closesocket(USB4Socket);

		// Check for socket close error
		if(err == SOCKET_ERROR) {
			printf("Simulator_Reset USB4: Error %d\n", WSAGetLastError());
		}
	}

	// Reset device status word
	device_status = 0;

	// Check for connect on USB channel 1
	if(USB_Init(USB1)) {
		if(Open_USB_Socket(USB1)) {
			device_status |= USB1_CONNECT;
		}
	}

	// Check for connect on USB channel 2
	if(USB_Init(USB2)) {
		if(Open_USB_Socket(USB2)) {
			device_status |= USB2_CONNECT;
		}
	}

	// Check for connect on USB channel 3
	if(USB_Init(USB3)) {
		if(Open_USB_Socket(USB3)) {
			device_status |= USB3_CONNECT;
		}
	}

	// Check for connect on USB channel 4
	if(USB_Init(USB4)) {
		if(Open_USB_Socket(USB4)) {
			device_status |= USB4_CONNECT;
		}
	}
}
