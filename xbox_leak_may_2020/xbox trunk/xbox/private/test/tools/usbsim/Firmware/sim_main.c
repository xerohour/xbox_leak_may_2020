//*****************************************************************************
//	    Filename: sim_main.c
//       Project: Microsoft USB Simulator
//	   Copyright: INDesign-LLC, 2000
//
//	 Description:
//
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/sim_main.c $
// $Revision: 1.12 $
// $Date: 2001/01/25 17:46:59Z $ 
// $Author: rjgardner $
// $Log: sim_main.c $
// Revision 1.12  2001/01/25 17:46:59Z  rjgardner
// Drop call to Process_Socket_In and incorporate functionality into
// function Socket_Poll.  Drop call to Process_Connect and 
// incorporate functionality into function Process_Control_Msg.
// Revision 1.11  2001/01/18 16:27:27Z  klpayne
// Add support for ready LED
// Revision 1.10  2001/01/04 19:24:46Z  klpayne
// Support IRQ mapping changes
// Revision 1.9  2001/01/03 21:44:18Z  rjgardner
// Revision 1.8  2000/12/22 20:53:21Z  rjgardner
// Add storage declaration for device_status and add logic to initialize
// and set properly on USB1 connect.
// Revision 1.7  2000/12/21 14:31:18Z  rjgardner
// Add Version.h header file and code to print to the console.
// Revision 1.6  2000/12/20 18:07:52Z  klpayne
// Added call to command process for board.
// Revision 1.5  2000/12/18 22:32:30Z  rjgardner
// Attempt to Init USB board 1 on network connect
// Revision 1.4  2000/12/18 21:53:36Z  rjgardner
// Rough structure of main process loop
// Revision 1.3  2000/12/14 21:31:49Z  klpayne
// Remove debug toggle
// Revision 1.2  2000/12/13 15:24:33Z  klpayne
// Move intr processing out of intr handler
// Revision 1.1  2000/12/05 22:04:53Z  klpayne
// Initial revision
//
//*****************************************************************************

#include <stdio.h>
#include <dos.h>
#include "sim.h"
#include "USB_brd.h"
#include "sim_cmd.h"
#include "Ether.h"
#include "Version.h"

//------------------------LOCAL CONSTANTS AND MACROS---------------------------

//-----------------------------GLOBAL VARIABLES--------------------------------

unsigned char device_status = 0;

//----------------------STATIC LOCAL FUNCTION PROTOTYPES-----------------------


//*****************************************************************************
//
//	   FUNCTION: void main(void)
//	DESCRIPTION: The main loop.   
//
//	     INPUTS:			
//	    RETURNS:		
//        NOTES:
//
//*****************************************************************************
void main(void) {
	unsigned char Connected = FALSE;
	int socket = USB1;
    unsigned char brd = USB1;

	// Display version number
	printf("USB Simulator Version: %s\n", Version);

    Sim_Init();
    outp(GPIO_PORTB,0x01);  // illuminate ready LED
    while(1) {
		// Check if we are connected to a controller
		if(Connected) {
			// Check for USB data to controller
	        if(USB_INTR_FLAG[brd-1] == TRUE) {  // brd 1 has an intr
    	        USB_INTR_FLAG[brd-1] = FALSE;
        	    USB_Intr_Handler(brd);    // common handler
	        }
			// Check buffer for USB data from controller
            Cont_To_Sim_Parser(brd);

			// Poll socket for Controller PC messages
			Socket_Poll(socket);

			// Check buffer for data to Controller PC
			Process_Socket_Out(socket);

			 // Restart socket rotation
			if((++socket) > USB4) {
				socket = CNTRL;
			}
            // Restart board rotation
            if((++brd) > USB4) {
                brd = USB1;
            }
		} else {
			// Check for network connect
			if(Socket_Poll(CNTRL)) {
				// Flag connect
				Connected = TRUE;

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

				printf("con - %02X\r\n",device_status);                
			}
		}
    }
}


