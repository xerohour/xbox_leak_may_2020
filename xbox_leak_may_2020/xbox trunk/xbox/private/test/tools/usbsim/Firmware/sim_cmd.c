//*****************************************************************************
//	    Filename: sim_cmd.c
//       Project: Microsoft USB Simulator
//	   Copyright: Microsoft Corp, 2000
//
//	 Description: this file contains the functions associated with building and
//                parsing the commands between the simulator and the controller
//
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/sim_cmd.c $
// $Revision: 1.17 $
// $Date: 2001/05/08 14:59:30Z $ 
// $Author: klpayne $
// $Log: sim_cmd.c $
// Revision 1.17  2001/05/08 14:59:30Z  klpayne
// Add support for stall commands
// Revision 1.16  2001/02/14 17:59:23Z  klpayne
// Add parsing for support of endpoint halt command
// Revision 1.15  2001/01/29 16:07:30Z  klpayne
// Change xmit/recv buffers to allow 1024 size.
// Revision 1.14  2001/01/25 15:57:48Z  klpayne
// Add suspend/resume command support
// Revision 1.13  2001/01/23 18:44:48Z  klpayne
// Remove old set addr code
// Revision 1.12  2001/01/12 15:09:21Z  klpayne
// Modify to support local usb addr setting
// Revision 1.11  2001/01/11 14:32:20Z  klpayne
// Add USB Reset Command
// Revision 1.10  2001/01/10 20:48:14Z  klpayne
// Remove debug code, use real usb address
// Revision 1.9  2001/01/09 21:25:07Z  klpayne
// Remove new_usb_addr variable.
// Revision 1.8  2001/01/09 21:05:11Z  klpayne
// Code cleanup
// Revision 1.7  2001/01/05 19:02:28Z  klpayne
// Cleanup buffer handling
// Revision 1.6  2001/01/04 15:35:46Z  klpayne
// Interim checkin
// Revision 1.5  2000/12/20 18:06:16Z  klpayne
// Changed to use endpoint buffers.
// Changes from first system integration.
// Revision 1.4  2000/12/19 13:23:32Z  klpayne
// Initial support of buffers
// Revision 1.3  2000/12/15 21:52:11Z  klpayne
// Added test code, beginning of command parser
// Revision 1.2  2000/12/14 21:30:33Z  klpayne
// Added handshake command support
// Revision 1.1  2000/12/06 15:33:02Z  klpayne
// Initial revision
//
//*****************************************************************************
#include <stdio.h>
#include <dos.h>
#include <mem.h>
#include "sim.h"
#include "sim_cmd.h"
#include "buffers.h"
#include "USB_brd.h"

//------------------------LOCAL CONSTANTS AND MACROS---------------------------

//-----------------------------GLOBAL VARIABLES--------------------------------
unsigned char buf[1024];

unsigned char New_Usb_Addr;

//----------------------STATIC LOCAL FUNCTION PROTOTYPES-----------------------

//*****************************************************************************
//
//	   FUNCTION: void Sim_To_Cont_USBData(unsigned char brd, unsigned char pid, unsigned char ep, unsigned int size, unsigned char *ptr)
//	DESCRIPTION: this function loads up a USB data message to send to the controller   
//
//	     INPUTS: unsigned char brd - which brd we are using
//               unsigned char pid - which pid has been received
//				 unsigned char ep - which endpoint
//               unsigned int size - size of the data being sent (does not include PID)
//               unsigned char *ptr - pointer to data to load
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
void Sim_To_Cont_USBData(unsigned char brd, unsigned char pid, unsigned char ep, unsigned int size, unsigned char *ptr) {

    unsigned char *dptr;
    
    if((*ptr == 0x00) && (*(ptr+1) == 0x05)) {    // set address, handle locally
        New_Usb_Addr = *(ptr+2);  // get new addr
    }
    
    if((*ptr == 0x02) && (*(ptr+1) == 0x01) ) {    // clear feature - endpoint halt, need to clear seq
        if(*(ptr+4) <  USB_MAX_EP) {
            USB_SetupEP(brd,*(ptr+4));  // re-setup ep
        }
    }
    
    dptr = buf;
    *dptr++ = CMDUSBDATA; // command 1
    *dptr++ = SUBUSBDATA;   // sub command 0
    *dptr++ = ep;    // set endpoint
    *dptr++ = (size+1) % 256;  // little endian mode
    *dptr++ = (size+1) / 256;  // set size to data plus pid
    *dptr++ = pid;
    memcpy(dptr,ptr,size);

    Put_Buffer((SOCK_201_XMIT + ((brd-1)*2)), (unsigned char *)&buf, (size + 6));  
}

//*****************************************************************************
//
//	   FUNCTION: void Sim_To_Cont_USBHS(unsigned char brd, unsigned char pid, unsigned char ep, unsigned int size, unsigned char *ptr)
//	DESCRIPTION: this function loads up a USB handshake message to send to the controller   
//
//	     INPUTS: unsigned char brd - which brd we are using
//               unsigned char pid - which pid has been received
//				 unsigned char ep - which endpoint
//               unsigned int size - size of the data being sent (does not include PID)
//               unsigned char *ptr - pointer to data to load
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
void Sim_To_Cont_USBHS(unsigned char brd, unsigned char pid, unsigned char ep, unsigned int size, unsigned char *ptr) {

    unsigned char *dptr;
    
    dptr = buf;
    *dptr++ = CMDUSBHS; // command 2
    if((pid != USB_ACK) && (pid != USB_NAK)) {
        *dptr++ = SUBUSBHSRESET;   // sub command 3 (reset, suspend, resume)
    }
    else {
        *dptr++ = SUBUSBHSACK;   // sub command 1
    }
    *dptr++ = ep;    // set endpoint
    *dptr++ = (size+1) % 256;  // little endian mode
    *dptr++ = (size+1) / 256;  // set size to data plus pid
    *dptr++ = pid;
    memcpy(dptr,ptr,size);

    Put_Buffer((SOCK_201_XMIT + ((brd-1)*2)), (unsigned char *)&buf, (size + 6));  

}

//*****************************************************************************
//
//	   FUNCTION: void Cont_To_Sim_Parser(unsigned char brd)
//	DESCRIPTION: this function parses commands received from the controller   
//
//	     INPUTS: unsigned char brd - which board commands are meant for
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
void Cont_To_Sim_Parser(unsigned char brd) {

    unsigned char *ptr;
    unsigned char ep;
    unsigned char i;
    
    if(Get_Buffer((SOCK_201_RCV + ((brd-1)*2)), (unsigned char *)&buf) == 0) {    // no data, return
        return;
    }
    
    ptr = buf;
    if(*ptr == CMDUSBDATA) {  // usb data (1)
        ptr += 2;   // get endpoint
        ep = *ptr++;
        Endpoints[brd][ep].xmitbuffercnt = *ptr++;    // get size of data
        Endpoints[brd][ep].xmitbuffercnt += (*ptr * 256);    // little endian format
        Endpoints[brd][ep].xmitbuffercnt--;    // ignore PID
        if(Endpoints[brd][ep].autorepeat) { // if autorepeat is on, save count
            Endpoints[brd][ep].xmitrepeatcnt = Endpoints[brd][ep].xmitbuffercnt;
        }
        ptr += 2;   // skip PID portion of data
        for(i=0;i<Endpoints[brd][ep].xmitbuffercnt;i++) { // load up receive buffer
            Endpoints[brd][ep].xmit_buf[i] = *ptr++;
        }
        Endpoints[brd][ep].xmitbufferptr = Endpoints[brd][ep].xmit_buf;
        
        if(Endpoints[brd][ep].xmitbuffercnt == 0) { // need to send null ack phase
            Endpoints[brd][ep].ack = 1;
        }
        Check_USB_Xmit(brd,ep);    // force transmit
    }
    else if(*ptr == CMDSIMSETUP) {   // sim setup (5)
        ptr++;  // check sub command
        if(*ptr == SUBATTRIB) {   // attrib (EP) setup (1)
            ptr++;  // get to ep
            ep = *ptr;
            ptr += 3;   // skip field size, get data
            Endpoints[brd][ep].fifosize = *ptr++;    // get fifo size
            Endpoints[brd][ep].type = *ptr++;    // get type
            Endpoints[brd][ep].autorepeat = *ptr++;
            USB_SetupEP(brd,ep);  // setup new ep parameters
        }
        else if(*ptr == SUBCONNECT) {   // connect/disconnect (3)
            ptr += 4;   // get to connect info
            if(*ptr == 0x00) {    // disconnect
                USB_Disconnect(brd);
            }
            else {
                USB_Connect(brd);
            }
        }
        else if(*ptr == SUBSTALL) {   // stall/unstall (7)
            ptr++;  // get to ep
            ep = *ptr;
            ptr += 3;   // skip field size, get data
            if(*ptr == 0x01) {  // stall
                USB_Stall(brd,ep);
            }
            else {  // unstall
                USB_UnStall(brd,ep);
            }
        }
        else {
            ;
        }
    }
    else {
        ;
    }

}




