//*****************************************************************************
//	    Filename: USB_brd.c
//       Project: Microsoft USB Simulator
//	   Copyright: INDesign-LLC, 2000
//
//	 Description: This file contains all of the functions related to handling
//                the USB interface, both public and private functions.
//
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/USB_brd.c $
// $Revision: 1.28 $
// $Date: 2001/05/08 15:00:27Z $ 
// $Author: klpayne $
// $Log: USB_brd.c $
// Revision 1.28  2001/05/08 15:00:27Z  klpayne
// Add data toggle clear for xmit
// Add stall/unstall support
// Revision 1.27  2001/03/23 20:02:57Z  klpayne
// Revision 1.26  2001/02/14 17:58:53Z  klpayne
// Correct mistake in resetting SEQ bit in endpoint setup
// Revision 1.25  2001/02/09 12:53:08Z  klpayne
// Clean-up SOF intr
// Revision 1.24  2001/01/30 13:50:47Z  klpayne
// Allow disabling of endpoint 0
// Revision 1.23  2001/01/25 15:58:31Z  klpayne
// Add suspend/resume support.
// Make USB_SetAddr static.
// Revision 1.22  2001/01/23 19:37:19Z  klpayne
// Fix Endpoint array for 4th board.
// Revision 1.21  2001/01/18 16:27:54Z  klpayne
// Explore suspend/resume support
// Revision 1.20  2001/01/17 20:23:57Z  klpayne
// Support all 8 possible endpoints
// Revision 1.19  2001/01/17 15:10:42Z  klpayne
// Firm up board detect.
// Revision 1.18  2001/01/16 13:44:06Z  klpayne
// Build for new board irqs
// Revision 1.17  2001/01/12 15:09:45Z  klpayne
// Modify to support local usb addr setting
// Support autorepeat
// Revision 1.16  2001/01/11 14:34:17Z  klpayne
// Add support for USB Reset,
// Clean up after disconnect
// Add enable of SOF intr on isoc pipe setup
// Revision 1.15  2001/01/10 20:49:17Z  klpayne
// Clean up set endpoint
// Revision 1.14  2001/01/09 21:14:13Z  klpayne
// Clean up interrupt handling
// Revision 1.13  2001/01/08 21:19:39Z  klpayne
// Set board 1 to IRQ10/addr 200 to work with old hardware
// Revision 1.12  2001/01/05 19:03:10Z  klpayne
// Make endpoints/board
// Revision 1.11  2001/01/04 19:24:19Z  klpayne
// Support changes to IRQ mapping
// Revision 1.10  2001/01/04 15:35:46Z  klpayne
// Interim checkin
// Revision 1.9  2000/12/20 18:07:03Z  klpayne
// Added test code.
// Revision 1.8  2000/12/19 13:23:33Z  klpayne
// Initial support of buffers
// Revision 1.7  2000/12/15 21:52:25Z  klpayne
// USB operation fixes
// Revision 1.6  2000/12/14 21:32:58Z  klpayne
// Clean up intr handling
// Revision 1.5  2000/12/13 15:24:33Z  klpayne
// Move intr processing out of intr handler
// Revision 1.4  2000/12/06 22:04:46Z  klpayne
// Code cleanup
// Revision 1.3  2000/12/06 15:32:46Z  klpayne
// added calls to sim_cmd functions
// Revision 1.2  2000/12/06 14:11:34Z  klpayne
// Add transmit framework
// Revision 1.1  2000/12/05 22:04:53Z  klpayne
// Initial revision
//
//*****************************************************************************
#include <stdio.h>
#include <dos.h>
#include "sim.h"
#include "USB_brd.h"
#include "sim_cmd.h"

//#define OLDBRD 1
//#define DOSUSPEND 1
//------------------------LOCAL CONSTANTS AND MACROS---------------------------
static unsigned int BRD_ADDRESSES[] = {USB_BRD_1, USB_BRD_2, USB_BRD_3, USB_BRD_4};
static void interrupt (*USB_INTR_VECT[])() = {USB_Intr_Brd1, USB_Intr_Brd2, USB_Intr_Brd3, USB_Intr_Brd4}; // intr vectors/brd
#ifdef OLDBRD
static unsigned int DOS_INTR[] = {IRQ10, IRQ09, IRQ14, IRQ15};  // interrupts/brd
static unsigned char DOS_INTR_MASK[] = {IRQ10_MASK, IRQ09_MASK, IRQ14_MASK, IRQ15_MASK};    // intr masks/brd
#else
static unsigned int DOS_INTR[] = {IRQ09, IRQ10, IRQ14, IRQ15};  // interrupts/brd
static unsigned char DOS_INTR_MASK[] = {IRQ09_MASK, IRQ10_MASK, IRQ14_MASK, IRQ15_MASK};    // intr masks/brd
#endif
static unsigned char USB_BRD_RESET[] = {0x02, 0x08, 0x20, 0x80};    // bits to control reset/brd
static unsigned char USB_BRD_DPLUS[] = {0x01, 0x04, 0x10, 0x40};    // bits to control D+/brd

static unsigned char DSAV_RBITS[] = {DSAV_RXAV0, DSAV_RXAV1, DSAV_RXAV2, DSAV_RXAV3};   // bits for looking at DSAV Recieve flags/EP
static unsigned char DSAV1_RBITS[] = {DSAV1_RXAV4, DSAV1_RXAV5, DSAV1_RXAV6, DSAV1_RXAV7};   // bits for looking at DSAV Recieve flags/EP
static unsigned char DSAV_TBITS[] = {DSAV_TXAV0, DSAV_TXAV1, DSAV_TXAV2, DSAV_TXAV3};   // bits for looking at DSAV Transmit flags/EP
static unsigned char DSAV1_TBITS[] = {DSAV1_TXAV4, DSAV1_TXAV5, DSAV1_TXAV6, DSAV1_TXAV7};   // bits for looking at DSAV Recieve flags/EP
static unsigned char INTR_BITS[] = {(SBIE_FRXIE0 | SBIE_FTXIE0), (SBIE_FRXIE1 | SBIE_FTXIE1), (SBIE_FRXIE2 | SBIE_FTXIE2), (SBIE_FRXIE3 | SBIE_FTXIE3)};   // bits for enabling intrs/EP
static unsigned char INTR1_BITS[] = {(SBIE1_FRXIE4 | SBIE1_FTXIE4), (SBIE1_FRXIE5 | SBIE1_FTXIE5), (SBIE1_FRXIE6 | SBIE1_FTXIE6), (SBIE1_FRXIE7 | SBIE1_FTXIE7)};   // bits for enabling intrs/EP

static unsigned int NON_ISOC_FIFO_SIZE[] = {16, 64, 8, 32}; // actual fifo sizes for non-isoc fifos
static unsigned int ISOC_FIFO_SIZE[] = {64, 256, 512, 1024}; // actual fifo sizes for isoc fifos

//-----------------------------GLOBAL VARIABLES--------------------------------
unsigned char usbdata[MAXUSBDATA];    // usb data buffer
struct EndpointSetup Endpoints[MAXUSBBRDS+1][USB_MAX_EP]; // array of endpoint structs
static unsigned char Usbaddr;   // usb device addr
unsigned char USB_INTR_FLAG[MAXUSBBRDS]; // flag indicating usb intr has occurred/board

extern unsigned char New_Usb_Addr;

//----------------------STATIC LOCAL FUNCTION PROTOTYPES-----------------------

// The following are local, private functions for USB access
//
//*****************************************************************************
//
//	   FUNCTION: static void USB_Write(unsigned char brd, unsigned char reg, unsigned char val)
//	DESCRIPTION: this function handles the low-level writing to the USB chip non-special shared registers   
//
//	     INPUTS: unsigned char brd - which board (address) to write to
//				 unsigned char reg - which reg of the chip to write to
//				 unsigned char val - value to write to reg			
//	    RETURNS: nothing		
//        NOTES: certain registers are shared access registers (both firmware and hardware
//				 can write to them).  these registers should be accessed via USB_Write_SR()
//
//*****************************************************************************
static void USB_Write(unsigned char brd, unsigned char reg, unsigned char val) {

    if (reg > USB_DSAV1) {	// illegal register value
        return;
	}

	outp((BRD_ADDRESSES[brd-1] + reg),(int)val); // set value

}

//*****************************************************************************
//
//	   FUNCTION: static void USB_Write_SR(unsigned char brd, unsigned char reg, unsigned char mask, unsigned char val)
//	DESCRIPTION: this function handles the low-level writing to the USB chip special shared registers   
//
//	     INPUTS: unsigned char brd - which board (address) to write to
//				 unsigned char reg - which reg of the chip to write to
//               unsigned char mask - mask to use in read/modify/write action
//				 unsigned char val - value to write to reg			
//	    RETURNS: nothing		
//        NOTES: certain registers are shared access registers (both firmware and hardware
//				 can write to them).  these registers must have the PEND bit set
//				 before access and then a read/modify/write sequence performed
//
//*****************************************************************************
static void USB_Write_SR(unsigned char brd, unsigned char reg, unsigned char mask, unsigned char val) {

	unsigned char temp_val;

    if (reg > USB_DSAV1) {	// illegal register value
        return;
	}

	outp((BRD_ADDRESSES[brd-1] + USB_PEND),(int)1); // set pending bit

    // Read-Modify-Write register
	temp_val = inp(BRD_ADDRESSES[brd-1] + reg);
	temp_val &= ~mask;
	outp((BRD_ADDRESSES[brd-1] + reg),(int)(temp_val | (val & mask)));

	outp((BRD_ADDRESSES[brd-1] + USB_PEND),(int)0); // clear pending bit

}

//*****************************************************************************
//
//	   FUNCTION: static unsigned char USB_Read(unsigned char brd, unsigned char reg)
//	DESCRIPTION: this function handles the low-level reading from the USB chip registers   
//
//	     INPUTS: unsigned char brd - which board (address) to read from
//				 unsigned char reg - which reg of the chip to read from
//	    RETURNS: unsigned char value of register		
//        NOTES:
//
//*****************************************************************************
static unsigned char USB_Read(unsigned char brd, unsigned char reg) {
    
	return(inp(BRD_ADDRESSES[brd-1] + reg));
}

//*****************************************************************************
//
//	   FUNCTION: static void USB_Set_Address(unsigned char brd, unsigned char addr)
//	DESCRIPTION: this function handles setting the new USB device address  
//
//	     INPUTS: unsigned char brd - which brd to set address
//               unsigned char addr - new address
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
static void USB_Set_Address(unsigned char brd, unsigned char addr) {

    Usbaddr = addr;
	USB_Write(brd,USB_FADDR,Usbaddr);	// set USB address
}

//*****************************************************************************
//
//	   FUNCTION: static void USB_SOF_Handler(unsigned char brd)
//	DESCRIPTION: this function handles a SOF interrupt  
//
//	     INPUTS: unsigned char brd - the active board
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
static void USB_SOF_Handler(unsigned char brd) {

    unsigned char ep, active;
    unsigned int rcvsize, i;

    for(ep=1;ep<USB_MAX_EP;ep++) {   // see if we have an isoc pipe, ep 0 can not be
        if(Endpoints[brd][ep].type == USB_ISOC_PIPE) {    // endpoint is set up as a isoc pipe

            USB_Write(brd,USB_EPINDEX,EPINDEX_EP0+ep); // select EP
            
            if(ep < EPINDEX_EP4) {
                active = USB_Read(brd,USB_DSAV) & DSAV_RBITS[ep]; 
            }
            else {
                active = USB_Read(brd,USB_DSAV1) & DSAV1_RBITS[ep]; 
            }

            if(active) {   // see if data received
                if(USB_Read(brd,USB_RXSTAT) & RXSTAT_RXERR) {   // check error handling
                    if(USB_Read(brd,USB_RXFLG) & RXFLG_RXOVF) {
                        USB_Write(brd,USB_RXCON, USB_Read(brd,USB_RXCON) | RXCON_RXCLR);
// log error to controller????
                    }
                }		
                else {  // no error, process data
			        rcvsize = USB_Read(brd,USB_RXCNTH) << 8;    // get number of bytes received
			        rcvsize += USB_Read(brd,USB_RXCNTL);
                    for(i=0;i<rcvsize;i++) {    // get data
                        usbdata[i] = USB_Read(brd,USB_RXDAT);
                    }
    	    		if (USB_Read(brd,USB_RXFLG) & RXFLG_RXURF) {    // if underflow, clear, otherwise unlock
	    	    		USB_Write(brd,USB_RXCON, USB_Read(brd,USB_RXCON) | RXCON_RXCLR);
// log error to controller????
		    	    }
    		    	else {
	    		    	USB_Write(brd,USB_RXCON, USB_Read(brd,USB_RXCON) | RXCON_RXFFRC);
    		    	}

                    if(USB_Read(brd,USB_RXSTAT) & RXSTAT_RXSEQ) {   // DATA0 must have been last one
                        Sim_To_Cont_USBData(brd,USB_DATA0,ep,rcvsize,usbdata);  // notify controller of data
                    }
                    else {  // DATA1 must have been last one
                        Sim_To_Cont_USBData(brd,USB_DATA1,ep,rcvsize,usbdata);  // notify controller of data
                    }

                }
            }
            						
            if(ep < EPINDEX_EP4) {
                active = USB_Read(brd,USB_DSAV) & DSAV_TBITS[ep]; 
            }
            else {
                active = USB_Read(brd,USB_DSAV1) & DSAV1_TBITS[ep]; 
            }

            if(active) {   // check for transmit available
                // see if we have anything to xmit, may be in the middle of a large packet, or may have received
                // data from last check.  if data has been received, the appropriate endpoint buffer will be loaded
                Check_USB_Xmit(brd, ep);
            }
        }
    }
    USB_Write_SR(brd,USB_SOFH,SOFH_ASOF,USB_REG_NO_BITS);   // clear intr bit
}

//*****************************************************************************
//
//	   FUNCTION: static void USB_EP_Rcv(unsigned char brd, unsigned char ep)
//	DESCRIPTION: this function handles endpoint receive intrs for non-isoc pipes,
//               isoc pipes are handled in sof handler
//
//	     INPUTS: unsigned char brd - the active board
//               unsigned char ep - the active endpoint
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
static void USB_EP_Rcv(unsigned char brd, unsigned char ep) {
    
    unsigned int rcvsize, i;
    unsigned char rcvstat;

    USB_Write(brd,USB_EPINDEX,EPINDEX_EP0+ep); // select EP

    if(Endpoints[brd][ep].type == USB_CTRL_PIPE) {    // endpoint is set up as a control pipe

        rcvstat = USB_Read(brd,USB_RXSTAT); // get recv status
		
        if(rcvstat & RXSTAT_RXSETUP) {	// we have a SETUP packet
            usbdata[0] = Usbaddr;
            usbdata[1] = ep;
            Sim_To_Cont_USBData(brd,PID_SETUP, ep, (unsigned int)2, usbdata); // notify controller of SETUP PID

            USB_Write_SR(brd,USB_RXSTAT,RXSTAT_EDOVW,USB_REG_NO_BITS);   // clear bit to unlock fifo
			rcvsize = USB_Read(brd,USB_RXCNTH) << 8;    // get number of bytes received
			rcvsize += USB_Read(brd,USB_RXCNTL);
            for(i=0;i<rcvsize;i++) {    // get data
                usbdata[i] = USB_Read(brd,USB_RXDAT);
            }

            rcvstat = USB_Read(brd,USB_RXSTAT); // re-read status reg
            USB_Write_SR(brd,USB_RXSTAT,RXSTAT_RXSETUP,USB_REG_NO_BITS);   // clear bit to indicate done w/setup
		
            if(rcvstat & RXSTAT_RXSEQ) {   // DATA0 must have been last one
                Sim_To_Cont_USBData(brd,USB_DATA0,ep,rcvsize,usbdata);  // notify controller of data
            }
            else {  // DATA1 must have been last one
                Sim_To_Cont_USBData(brd,USB_DATA1,ep,rcvsize,usbdata);  // notify controller of data
            }

            if(rcvstat & RXSTAT_RXACK) {	// we have an ack
                Sim_To_Cont_USBHS(brd,USB_ACK, ep, (unsigned int)0, usbdata); // notify controller of ACK
            }

            if (!(rcvstat & (RXSTAT_EDOVW | RXSTAT_STOVW))) {  // unlock current packet
					USB_Write(brd,USB_RXCON, USB_Read(brd,USB_RXCON) | RXCON_RXFFRC);
            }
            Endpoints[brd][ep].xmitbuffercnt = 0;   // flush any unsent data

        }
        else {  // not a setup, get data received and notify controller
            // Error handling for overflow case. Clear FIFO
            // and break out.
            if (rcvstat & RXSTAT_RXERR) {
                if (USB_Read(brd,USB_RXFLG) & RXFLG_RXOVF) {
                    USB_Write(brd,USB_RXCON, USB_Read(brd,USB_RXCON) | RXCON_RXCLR);
// log error to controller????
                }
            }
            else {  // we have received valid data, grab it and send to controller
                usbdata[0] = Usbaddr;
                usbdata[1] = ep;
                Sim_To_Cont_USBData(brd,PID_OUT, ep, (unsigned int)2, usbdata); // notify controller of OUT PID
                
    			rcvsize = USB_Read(brd,USB_RXCNTH) << 8;    // get number of bytes received
	    		rcvsize += USB_Read(brd,USB_RXCNTL);
                for(i=0;i<rcvsize;i++) {    // get data
                    usbdata[i] = USB_Read(brd,USB_RXDAT);
                }
	    		if (USB_Read(brd,USB_RXFLG) & RXFLG_RXURF) {    // if underflow, clear, otherwise unlock
		    		USB_Write(brd,USB_RXCON, USB_Read(brd,USB_RXCON) | RXCON_RXCLR);
// log error to controller????
			    }
    			else {
	    			USB_Write(brd,USB_RXCON, USB_Read(brd,USB_RXCON) | RXCON_RXFFRC);
		    	}

                if(rcvstat & RXSTAT_RXSEQ) {   // DATA0 must have been last one
                    Sim_To_Cont_USBData(brd,USB_DATA0,ep,rcvsize,usbdata);  // notify controller of data
                }
                else {  // DATA1 must have been last one
                    Sim_To_Cont_USBData(brd,USB_DATA1,ep,rcvsize,usbdata);  // notify controller of data
                }

                if(rcvstat & RXSTAT_RXACK) {	// we have an ack
                    Sim_To_Cont_USBHS(brd,USB_ACK, ep, (unsigned int)0, usbdata); // notify controller of ACK
                }
            }
        }
    }
    else {  // not control pipe, isoc pipe handled by SOF intr, process everything else the same
        rcvstat = USB_Read(brd,USB_RXSTAT); // get recv status
        // Error handling for overflow case. Clear FIFO
        // and break out.
        if (rcvstat & RXSTAT_RXERR) {
            if (USB_Read(brd,USB_RXFLG) & RXFLG_RXOVF) {
                USB_Write(brd,USB_RXCON, USB_Read(brd,USB_RXCON) | RXCON_RXCLR);
// log error to controller????
                printf("rcv ovf error, ep%d\r\n",ep);
            }
        }
        else {  // we have received valid data, grab it and send to controller
            usbdata[0] = Usbaddr;
            usbdata[1] = ep;
            Sim_To_Cont_USBData(brd,PID_OUT, ep, (unsigned int)2, usbdata); // notify controller of OUT PID
                
			rcvsize = USB_Read(brd,USB_RXCNTH) << 8;    // get number of bytes received
			rcvsize += USB_Read(brd,USB_RXCNTL);
            for(i=0;i<rcvsize;i++) {    // get data
                usbdata[i] = USB_Read(brd,USB_RXDAT);
            }
            if (USB_Read(brd,USB_RXFLG) & RXFLG_RXURF) {    // we read too many things
                USB_Write(brd,USB_RXCON, USB_Read(brd,USB_RXCON) | RXCON_RXCLR);
// log error to controller????
                printf("rcv urf error, ep%d\r\n",ep);
            }
            else {  // everything okay, clear fifo
                USB_Write(brd,USB_RXCON, (USB_Read(brd,USB_RXCON) | RXCON_RXFFRC));
            }

            if(USB_Read(brd,USB_RXSTAT) & RXSTAT_RXSEQ) {   // DATA0 must have been last one
                Sim_To_Cont_USBData(brd,USB_DATA0,ep,rcvsize,usbdata);  // notify controller of data
            }
            else {  // DATA1 must have been last one
                Sim_To_Cont_USBData(brd,USB_DATA1,ep,rcvsize,usbdata);  // notify controller of data
            }

            if(rcvstat & RXSTAT_RXACK) {	// we have an ack
                Sim_To_Cont_USBHS(brd,USB_ACK, ep, (unsigned int)0, usbdata); // notify controller of ACK
            }
        }
    }

}

//*****************************************************************************
//
//	   FUNCTION: static void USB_EP_Xmit(unsigned char brd, unsigned char ep)
//	DESCRIPTION: this function handles endpoint transmit intrs, for non-isoc pipes
//               isoc-pipes are handled in sof handler  
//
//	     INPUTS: unsigned char brd - the active board
//               unsigned char ep - the active endpoint
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
static void USB_EP_Xmit(unsigned char brd, unsigned char ep) {

    unsigned char txstat;

    USB_Write(brd,USB_EPINDEX,EPINDEX_EP0+ep); // select EP

    txstat = USB_Read(brd,USB_TXSTAT);  // read tx status
    if(txstat & TXSTAT_TXVOID) {  // we have nak'd, let controller know
		USB_Write(brd,USB_TXSTAT, USB_Read(brd,USB_TXSTAT) & ~(TXSTAT_TXVOID | TXSTAT_TXNAKE));    // clear & disable nak indication

        usbdata[0] = Usbaddr;
        usbdata[1] = ep;
        Sim_To_Cont_USBData(brd,PID_IN, ep, (unsigned int)2, usbdata); // notify controller of IN PID
        Sim_To_Cont_USBHS(brd,USB_NAK, ep, (unsigned int)0, usbdata); // notify controller of NAK
    }
    else if(txstat & TXSTAT_TXACK) {  // we have ack'd, let controller know
        USB_Write(brd,USB_TXSTAT,USB_Read(brd,USB_TXSTAT) | TXSTAT_TXNAKE);   // enable NAK intrs on IN PIDs

        usbdata[0] = Usbaddr;
        usbdata[1] = ep;
        Sim_To_Cont_USBData(brd,PID_IN, ep, (unsigned int)2, usbdata); // notify controller of IN PID
        Sim_To_Cont_USBHS(brd,USB_ACK, ep, (unsigned int)0, usbdata); // notify controller of ACK
        if(New_Usb_Addr != 0) { // we've just finished the status phase of a set addr command
            USB_Set_Address(brd,New_Usb_Addr);
            New_Usb_Addr = 0;   // clear indication
        }
        
    }
    Check_USB_Xmit(brd,ep); // check and see if we have anything to xmit
    
}


//*****************************************************************************
//
//	   FUNCTION: static void USB_EP_Handler(unsigned char brd)
//	DESCRIPTION: this function handles endpoint intrs  
//
//	     INPUTS: unsigned char brd - the active board
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
static void USB_EP_Handler(unsigned char brd) {

    unsigned char sbi;

    sbi = USB_Read(brd,USB_SBI);    // check for endpoint intrs on first 4 EPs
    while(sbi) {
		
        if(sbi & SBI_FRXD0) {   // check for receive intr, EP0
            USB_EP_Rcv(brd,EPINDEX_EP0);   // process endpoint
            USB_Write_SR(brd,USB_SBI,SBI_FRXD0,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI_FRXD1) {   // check for receive intr, EP1
            USB_EP_Rcv(brd,EPINDEX_EP1);   // process endpoint
            USB_Write_SR(brd,USB_SBI,SBI_FRXD1,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI_FRXD2) {   // check for receive intr, EP2
            USB_EP_Rcv(brd,EPINDEX_EP2);   // process endpoint
            USB_Write_SR(brd,USB_SBI,SBI_FRXD2,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI_FRXD3) {   // check for receive intr, EP3
            USB_EP_Rcv(brd,EPINDEX_EP3);   // process endpoint
            USB_Write_SR(brd,USB_SBI,SBI_FRXD3,USB_REG_NO_BITS);   // clear intr bit
        }

        // now check transmit intrs
        if(sbi & SBI_FTXD0) {   // check for transmit intr, EP0
            USB_EP_Xmit(brd,EPINDEX_EP0);   // process endpoint
            USB_Write_SR(brd,USB_SBI,SBI_FTXD0,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI_FTXD1) {   // check for transmit intr, EP1
            USB_EP_Xmit(brd,EPINDEX_EP1);   // process endpoint
            USB_Write_SR(brd,USB_SBI,SBI_FTXD1,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI_FTXD2) {   // check for transmit intr, EP2
            USB_EP_Xmit(brd,EPINDEX_EP2);   // process endpoint
            USB_Write_SR(brd,USB_SBI,SBI_FTXD2,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI_FTXD3) {   // check for transmit intr, EP3
            USB_EP_Xmit(brd,EPINDEX_EP3);   // process endpoint
            USB_Write_SR(brd,USB_SBI,SBI_FTXD3,USB_REG_NO_BITS);   // clear intr bit
        }
        sbi = USB_Read(brd,USB_SBI);    // check for endpoint intrs
    }

    sbi = USB_Read(brd,USB_SBI1);    // check for endpoint intrs on second 4 EPs
    while(sbi) {
        if(sbi & SBI1_FRXD4) {   // check for receive intr, EP4
            USB_EP_Rcv(brd,EPINDEX_EP4);   // process endpoint
            USB_Write_SR(brd,USB_SBI1,SBI1_FRXD4,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI1_FRXD5) {   // check for receive intr, EP5
            USB_EP_Rcv(brd,EPINDEX_EP5);   // process endpoint
            USB_Write_SR(brd,USB_SBI1,SBI1_FRXD5,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI1_FRXD6) {   // check for receive intr, EP6
            USB_EP_Rcv(brd,EPINDEX_EP6);   // process endpoint
            USB_Write_SR(brd,USB_SBI1,SBI1_FRXD6,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI1_FRXD7) {   // check for receive intr, EP7
            USB_EP_Rcv(brd,EPINDEX_EP7);   // process endpoint
            USB_Write_SR(brd,USB_SBI1,SBI1_FRXD7,USB_REG_NO_BITS);   // clear intr bit
        }

        // now check transmit intrs
        if(sbi & SBI1_FTXD4) {   // check for transmit intr, EP4
            USB_EP_Xmit(brd,EPINDEX_EP4);   // process endpoint
            USB_Write_SR(brd,USB_SBI1,SBI1_FTXD4,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI1_FTXD5) {   // check for transmit intr, EP5
            USB_EP_Xmit(brd,EPINDEX_EP5);   // process endpoint
            USB_Write_SR(brd,USB_SBI1,SBI1_FTXD5,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI1_FTXD6) {   // check for transmit intr, EP6
            USB_EP_Xmit(brd,EPINDEX_EP6);   // process endpoint
            USB_Write_SR(brd,USB_SBI1,SBI1_FTXD6,USB_REG_NO_BITS);   // clear intr bit
        }
        if(sbi & SBI1_FTXD7) {   // check for transmit intr, EP7
            USB_EP_Xmit(brd,EPINDEX_EP7);   // process endpoint
            USB_Write_SR(brd,USB_SBI1,SBI1_FTXD7,USB_REG_NO_BITS);   // clear intr bit
        }
        sbi = USB_Read(brd,USB_SBI1);    // check for endpoint intrs
    }
}

// The following are public functions for USB access
//
//*****************************************************************************
//
//	   FUNCTION: unsigned char USB_Init(unsigned char brd)
//	DESCRIPTION: this function initializes the USB chip on the selected board,
//				 if its there and alive   
//
//	     INPUTS: unsigned char brd - which board (address) to init			
//	    RETURNS: TRUE if board was there and could be initialized, FALSE if not		
//        NOTES: this function should be used to initially bring a board up.
//				 USB_SetEP() should be used to set up each endpoint after that.
//
//*****************************************************************************
unsigned char USB_Init(unsigned char brd) {

    unsigned char i;
    unsigned int temp;

    outp(GPIO_PORTA,(inp(GPIO_PORTA) | USB_BRD_RESET[brd-1]));  // un-reset board

    // init the endpoint structures, ep0 is always control
    Endpoints[brd][0].fifosize = 0x02;  // non-isoc size of 8
    Endpoints[brd][0].type = USB_CTRL_PIPE;   // control endpoint         
    Endpoints[brd][0].autorepeat = 0x00; // no auto repeat
    Endpoints[brd][0].xmitbuffercnt = 0x00;  // no chars
    Endpoints[brd][0].xmitbufferptr = (unsigned char *)NULL; // null ptr
    for(i=1;i<USB_MAX_EP;i++) { // remaining endpoints
        Endpoints[brd][i].fifosize = 00;  // non-isoc size of 16
        Endpoints[brd][i].type = USB_DISABLE_PIPE;   // endpoint disabled        
        Endpoints[brd][i].autorepeat = 0x00; // no auto repeat
        Endpoints[brd][i].xmitbuffercnt = 0x00;  // no chars
        Endpoints[brd][i].xmitbufferptr = (unsigned char *)NULL; // null ptr
    }

    USB_INTR_FLAG[brd-1] = FALSE;

    // perform a software reset to be sure we are starting from a known point
    USB_Write(brd,USB_SCR,(SCR_SRESET));
    for(temp=0;temp<10000;temp++) {    // give reset time
        ;
    }
    USB_Write(brd,USB_SCR,0x00);
    for(temp=0;temp<10000;temp++) {    // give reset time
        ;
    }

    // see if we have a board by writing/reading from the scratch register    
    USB_Write(brd,USB_SCRATCH, 0x55);
    USB_Write(brd,USB_SCR,0x00);    // dummy write to clear bus if no board?
	if(USB_Read(brd,USB_SCRATCH) != 0x55) {	// if no board, return FALSE
	    return FALSE;
	}

    // enable Endpoint 0 only and its RX and TX done intrs, also disable SOF intrs
    USB_Write(brd,USB_EPINDEX,EPINDEX_EP0); // select EP0
    
    USB_Write_SR(brd,USB_EPCON,(EPCON_CTLEP|EPCON_RXSPM|EPCON_RXIE|EPCON_RXEPEN|EPCON_TXOE|EPCON_TXEPEN),
     USB_REG_ALL_BITS);
    
    USB_Write(brd,USB_TXSTAT,TXSTAT_TXNAKE);   // enable NAK intrs on IN PIDs
    USB_Write(brd,USB_RXCON,(RXCON_RXCLR | RXCON_NONISO_8 | RXCON_ARM));
    USB_Write(brd,USB_TXCON,(TXCON_TXCLR | TXCON_NONISO_8 | TXCON_ATM));

    USB_Write_SR(brd,USB_SOFH,SOFH_SOFIE,USB_REG_NO_BITS);  // disable SOF interrupt 
    
    // Disable all other endpoints 
    for (i = 1; i < USB_MAX_EP; i++)
    {
        USB_Write(brd,USB_EPINDEX,EPINDEX_EP0+i); // select EP
        USB_Write_SR(brd,USB_EPCON,USB_REG_ALL_BITS,USB_REG_NO_BITS);   // set all bits to 0
    }
    
    USB_Write(brd,USB_EPINDEX,EPINDEX_EP0); // select EP0, again

    // enable endpoint 0
    USB_Write_SR(brd,USB_SBIE,(SBIE_FRXIE0 | SBIE_FTXIE0),USB_REG_ALL_BITS);   // enable EP0 xmit/rcv intr
    USB_Write_SR(brd,USB_SBIE1,USB_REG_ALL_BITS,USB_REG_NO_BITS);   // set all bits to 0, disable all intrs

    // no suspend/resume or remote wakeup interrupts, intr polarity is active hi 
    USB_Write(brd,USB_SCR,(SCR_T_IRQ | SCR_IRQPOL | SCR_IE_RESET | SCR_IE_SUSP));
    USB_Write_SR(brd,USB_SSR,SSR_RESET,USB_REG_NO_BITS);   // clear reset bit

    // DO NOT connect to USB bus (wait for connect command) and enable the new chip feature  
    USB_Write(brd,USB_MCSR, (USB_Read(brd,USB_MCSR) | MCSR_FEAT));
    outp(GPIO_PORTA,(inp(GPIO_PORTA) | USB_BRD_DPLUS[brd-1]));  // disable D+ on board

    Usbaddr = 0;
	USB_Write(brd,USB_FADDR,0x00);	// set USB address to 0

    // set DOS intr vector addr
    disable();
    setvect(DOS_INTR[brd-1],USB_INTR_VECT[brd-1]);
    outp(0xA1,(inp(0xA1) & ~DOS_INTR_MASK[brd-1]));
    enable();

    return TRUE;    // we have a board, its set up
}

//*****************************************************************************
//
//	   FUNCTION: void USB_SetupEP(unsigned char brd, unsigned char ep)
//	DESCRIPTION: this function sets up an Endpoint   
//
//	     INPUTS: unsigned char brd - which board (address) to use
//               unsigned char ep - which endpoint to set up			
//	    RETURNS: nothing		
//        NOTES: this function does not assume any other EP settings, that is,
//               it doesn't re-init the chip before making changes, it assumes
//               the calling function handles that, if necessary.  This function
//               will disable EP0 if requested.
//
//*****************************************************************************
void USB_SetupEP(unsigned char brd, unsigned char ep) {

    unsigned char temp;

    USB_Write(brd,USB_EPINDEX,EPINDEX_EP0+ep); // select EP

    if(Endpoints[brd][ep].type == USB_DISABLE_PIPE) {  // disable EP, even EP0
        USB_Write_SR(brd,USB_EPCON,USB_REG_ALL_BITS,USB_REG_NO_BITS);   // clear all bits
        if(ep < EPINDEX_EP4) {   // clear intr bits
            USB_Write_SR(brd,USB_SBIE,INTR_BITS[ep],USB_REG_NO_BITS);
        }
        else {
            USB_Write_SR(brd,USB_SBIE1,INTR1_BITS[ep],USB_REG_NO_BITS);
        }
        // not set global structure
        Endpoints[brd][ep].fifosize = 00;  // non-isoc size of 16
        Endpoints[brd][ep].type = USB_DISABLE_PIPE;   // endpoint disabled        
        Endpoints[brd][ep].autorepeat = 0x00; // no auto repeat
        Endpoints[brd][ep].xmitbuffercnt = 0x00;  // no chars
        Endpoints[brd][ep].xmitbufferptr = (unsigned char *)NULL; // null ptr
    }
    else {
        if((ep == EPINDEX_EP0) || (Endpoints[brd][ep].type == USB_CTRL_PIPE)) {  // control eps are always single buffering, control pipe, others always double
            USB_Write(brd,USB_RXCON, (RXCON_RXCLR | (Endpoints[brd][ep].fifosize << 5) | RXCON_ARM));
            USB_Write(brd,USB_TXCON, (TXCON_TXCLR | (Endpoints[brd][ep].fifosize << 5) | TXCON_ATM));
            USB_Write_SR(brd,USB_EPCON,(EPCON_CTLEP|EPCON_RXSPM|EPCON_RXIE|EPCON_RXEPEN|EPCON_TXOE|EPCON_TXEPEN),USB_REG_ALL_BITS);
            temp = USB_Read(brd,USB_TXSTAT);    // get current values
            temp &= ~TXSTAT_TXSEQ;   // reset xmit data toggle
            temp |= TXSTAT_TXSOVW;  // set bit to allow data toggle reset
            temp |= TXSTAT_TXNAKE;  // enable NAK intrs on IN PIDS
            USB_Write(brd,USB_TXSTAT,temp);   // enable NAK intrs on IN PIDs, reset xmit seq
            USB_Write_SR(brd,USB_RXSTAT,(RXSTAT_RXSOVW | RXSTAT_RXSEQ),(RXSTAT_RXSOVW | ~RXSTAT_RXSEQ));    // reset recv seq 
        }
        else if(Endpoints[brd][ep].type == USB_ISOC_PIPE) {  // set for isoc mode
            USB_Write(brd,USB_RXCON, (RXCON_RXCLR | (Endpoints[brd][ep].fifosize << 5) |RXCON_RXISO | RXCON_ARM));
            USB_Write(brd,USB_TXCON, (TXCON_TXCLR | (Endpoints[brd][ep].fifosize << 5) |TXCON_TXISO | TXCON_ATM));
            USB_Write_SR(brd,USB_EPCON,(EPCON_RXIE|EPCON_RXEPEN|EPCON_TXOE|EPCON_TXEPEN),USB_REG_ALL_BITS);
            temp = USB_Read(brd,USB_TXSTAT);    // get current values
            temp &= ~TXSTAT_TXSEQ;   // reset xmit data toggle
            temp |= TXSTAT_TXSOVW;  // set bit to allow data toggle reset
            temp &= ~TXSTAT_TXNAKE;  // disable NAK intrs on IN PIDS
            USB_Write(brd,USB_TXSTAT,temp);   // disable NAK intrs on IN PIDs, reset xmit seq
            USB_Write_SR(brd,USB_RXSTAT,(RXSTAT_RXSOVW | RXSTAT_RXSEQ),(RXSTAT_RXSOVW | ~RXSTAT_RXSEQ));    // reset recv seq 
//            USB_Write_SR(brd,USB_SOFH,SOFH_SOFIE,USB_REG_ALL_BITS);  // enable SOF interrupt 

        }
        else {  // enabled, not control or isoc pipe
            USB_Write(brd,USB_RXCON, (RXCON_RXCLR | (Endpoints[brd][ep].fifosize << 5) | RXCON_ARM));
            USB_Write(brd,USB_TXCON, (TXCON_TXCLR | (Endpoints[brd][ep].fifosize << 5) | TXCON_ATM));
            USB_Write_SR(brd,USB_EPCON,(EPCON_RXSPM|EPCON_RXIE|EPCON_RXEPEN|EPCON_TXOE|EPCON_TXEPEN),USB_REG_ALL_BITS);
            temp = USB_Read(brd,USB_TXSTAT);    // get current values
            temp &= ~TXSTAT_TXSEQ;   // reset xmit data toggle
            temp |= TXSTAT_TXSOVW;  // set bit to allow data toggle reset
            temp |= TXSTAT_TXNAKE;  // enable NAK intrs on IN PIDS
            USB_Write(brd,USB_TXSTAT,temp);   // enable NAK intrs on IN PIDs, reset xmit seq
            USB_Write_SR(brd,USB_RXSTAT,(RXSTAT_RXSOVW | RXSTAT_RXSEQ),(RXSTAT_RXSOVW | ~RXSTAT_RXSEQ));    // reset recv seq 
        }
        if(ep < EPINDEX_EP4) {   // enable xmit/rcv intrs
            USB_Write_SR(brd,USB_SBIE,INTR_BITS[ep],USB_REG_ALL_BITS);
        }
        else {
            USB_Write_SR(brd,USB_SBIE1,INTR1_BITS[ep],USB_REG_ALL_BITS);
        }
    }
}


//*****************************************************************************
//
//	   FUNCTION: void USB_Intr_Handler(unsigned char brd)
//	DESCRIPTION: this function is the common handler for all USB board interrupts  
//
//	     INPUTS: unsigned char brd - the active board
//	    RETURNS: nothing		
//        NOTES: its an interrupt, as usual, get in and out as fast as possible
//               a SOF intr is check for first, and if there is one, the SOF
//               handler is called to process any ISOC channels
//               next the System Status register is checked for suspend/resume intrs
//               lastly the Serial Bus Interrupts are checked for any endpoint intrs
//
//*****************************************************************************
void USB_Intr_Handler(unsigned char brd) {

    unsigned char val;


    val = USB_Read(brd,USB_SOFH);   // check for SOF intr

    // if the interrupt is enabled & a start of frame is received,
    // then it is a possible an ISOC Interrupt is here
	if((val & SOFH_SOFIE) && (val & SOFH_ASOF))	{
        USB_SOF_Handler(brd);
    }

    // read the ssr (System Status) register
    val = USB_Read(brd,USB_SSR);
    if(val & (SSR_RESET | SSR_RESUME | SSR_SUSPEND))	{	
        
        if (val & SSR_RESET) {    // USB reset intr
            USB_Write(brd,USB_SSR,0x00);   // clear all intrs
            USB_Write(brd,USB_PEND,0x00);   // clear pending bit
            USB_Write(brd,USB_SCR,(SCR_T_IRQ | SCR_IRQPOL | SCR_IE_RESET | SCR_IE_SUSP));
            USB_Write_SR(brd,USB_SSR,SSR_RESUME,USB_REG_NO_BITS);   // clear resume bit
            
            Sim_To_Cont_USBHS(brd,USB_RESET, 0, (unsigned int)0, usbdata); // notify controller of RESET
//            val = USB_Init(brd); // reset and re-connect to USB bus
            return;
        }

        if (val & SSR_RESUME) { // USB resume intr, note: pending bit is still set from suspend
            USB_Write(brd,USB_SSR,0x00);   // clear all intrs
            USB_Write(brd,USB_PEND,0x00);   // clear pending bit
            USB_Write(brd,USB_SCR,(SCR_T_IRQ | SCR_IRQPOL | SCR_IE_RESET | SCR_IE_SUSP));
            USB_Write_SR(brd,USB_SSR,SSR_RESUME,USB_REG_NO_BITS);   // clear resume bit
            Sim_To_Cont_USBHS(brd,0xFD, 0, (unsigned int)0, usbdata); // notify controller of RESUME
        }

        if (val & SSR_SUSPEND) {  // USB suspend intr
#if DOSUSPEND
            USB_Write(brd,USB_PEND,0x01);   // set pending bit, leave it set until resume
            USB_Write(brd,USB_SSR, (USB_Read(brd,USB_SSR) & ~SSR_SUSPEND));    // clear suspend
            USB_Write(brd,USB_SCRATCH, (USB_Read(brd,USB_SCRATCH) | SCRATCH_IE_RESUME));    // enable resume intr
            val = USB_Read(brd,USB_SSR); // re-read SSR
            if (val & SSR_SUSPEND) {  // still have suspend?
                USB_Write(brd,USB_SCR, USB_Read(brd,USB_SCR) & (~SCR_IE_SUSP)); // disable suspend intr
                
                USB_Write_SR(brd,USB_SBI,USB_REG_ALL_BITS,USB_REG_NO_BITS);   // clear any pending intr bit
                USB_Write_SR(brd,USB_SBI1,USB_REG_ALL_BITS,USB_REG_NO_BITS);   // clear any pending intr bit

                // go into suspend mode by setting/clearing suspend bit
                USB_Write(brd,USB_SSR, SSR_SUSPEND);    // set suspend
                USB_Write(brd,USB_SSR, 0x00);   // reset suspend bit

                Sim_To_Cont_USBHS(brd,0xFE, 0, (unsigned int)0, usbdata); // notify controller of SUSPEND
                return; // we are now "asleep", return
			}
#else
            USB_Write_SR(brd,USB_SSR,SSR_SUSPEND,USB_REG_NO_BITS);   // clear suspend bit
            USB_Write(brd,USB_SCR, USB_Read(brd,USB_SCR) & (~SCR_IE_SUSP)); // disable suspend intr
            Sim_To_Cont_USBHS(brd,0xFE, 0, (unsigned int)0, usbdata); // notify controller of SUSPEND
#endif
        }
    }
    USB_EP_Handler(brd);  // check endpoint intrs
}

//*****************************************************************************
//
//	   FUNCTION: void interrupt USB_Intr_Brd1()
//	DESCRIPTION: this function handles an interrupt received from the USB brd 1
//               this function calls the common handler with the approp. brd number  
//
//	     INPUTS: none
//	    RETURNS: nothing		
//        NOTES: its an interrupt, as usual, get in and out as fast as possible
//
//*****************************************************************************
void interrupt USB_Intr_Brd1() {

    USB_INTR_FLAG[0] = TRUE;    // flag intr happened
    outp(0xA0,0x20);    // re-enable intrs
    outp(0x20,0x20);

}

//*****************************************************************************
//
//	   FUNCTION: void interrupt USB_Intr_Brd2()
//	DESCRIPTION: this function handles an interrupt received from the USB brd 2
//               this function calls the common handler with the approp. brd number  
//
//	     INPUTS: none
//	    RETURNS: nothing		
//        NOTES: its an interrupt, as usual, get in and out as fast as possible
//
//*****************************************************************************
void interrupt USB_Intr_Brd2() {

    USB_INTR_FLAG[1] = TRUE;    // flag intr happened
    outp(0xA0,0x20);    // re-enable intrs
    outp(0x20,0x20);

}

//*****************************************************************************
//
//	   FUNCTION: void interrupt USB_Intr_Brd3()
//	DESCRIPTION: this function handles an interrupt received from the USB brd 3
//               this function calls the common handler with the approp. brd number  
//
//	     INPUTS: none
//	    RETURNS: nothing		
//        NOTES: its an interrupt, as usual, get in and out as fast as possible
//
//*****************************************************************************
void interrupt USB_Intr_Brd3() {

    USB_INTR_FLAG[2] = TRUE;    // flag intr happened
    outp(0xA0,0x20);    // re-enable intrs
    outp(0x20,0x20);

}

//*****************************************************************************
//
//	   FUNCTION: void interrupt USB_Intr_Brd4()
//	DESCRIPTION: this function handles an interrupt received from the USB brd 4
//               this function calls the common handler with the approp. brd number  
//
//	     INPUTS: none
//	    RETURNS: nothing		
//        NOTES: its an interrupt, as usual, get in and out as fast as possible
//
//*****************************************************************************
void interrupt USB_Intr_Brd4() {

    USB_INTR_FLAG[3] = TRUE;    // flag intr happened
    outp(0xA0,0x20);    // re-enable intrs
    outp(0x20,0x20);

}

//*****************************************************************************
//
//	   FUNCTION: void USB_Connect(void)
//	DESCRIPTION: this function handles connecting the USB board  
//
//	     INPUTS: none
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
void USB_Connect(unsigned char brd) {

    // connect to USB bus 
    USB_Write(brd,USB_MCSR, (USB_Read(brd,USB_MCSR) | MCSR_DPEN));
    outp(GPIO_PORTA,(inp(GPIO_PORTA) & ~USB_BRD_DPLUS[brd-1]));  // enable D+ on board
}

//*****************************************************************************
//
//	   FUNCTION: void USB_Disconnect(unsigned char brd)
//	DESCRIPTION: this function handles dis-connecting the USB board  
//
//	     INPUTS: unsigned char brd - board to use
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
void USB_Disconnect(unsigned char brd) {

    unsigned char i;

    // connect to USB bus 
    USB_Write(brd,USB_MCSR, (USB_Read(brd,USB_MCSR) & ~MCSR_DPEN));
    outp(GPIO_PORTA,(inp(GPIO_PORTA) | USB_BRD_DPLUS[brd-1]));  // disable D+ on board
    for(i=1;i<USB_MAX_EP;i++) { // leave EP0 as control, disable remaining endpoints
        Endpoints[brd][i].fifosize = 00;  // non-isoc size of 16
        Endpoints[brd][i].type = USB_DISABLE_PIPE;   // endpoint disabled        
        Endpoints[brd][i].autorepeat = 0x00; // no auto repeat
        Endpoints[brd][i].xmitbuffercnt = 0x00;  // no chars
        Endpoints[brd][i].xmitbufferptr = (unsigned char *)NULL; // null ptr
        USB_SetupEP(brd,i);
    }
    USB_Write_SR(brd,USB_SOFH,SOFH_SOFIE,USB_REG_NO_BITS);  // disable SOF interrupt 
}

//*****************************************************************************
//
//	   FUNCTION: void USB_Stall(unsigned char brd, unsigned char ep)
//	DESCRIPTION: this function handles stalling an endpoint  
//
//	     INPUTS: unsigned char brd - board to use
//               unsigned char ep - endpoint to stall
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
void USB_Stall(unsigned char brd, unsigned char ep) {

    USB_Write(brd,USB_EPINDEX,EPINDEX_EP0+ep); // select EP

    USB_Write_SR(brd,USB_EPCON,(EPCON_TXSTL | EPCON_RXSTL),USB_REG_ALL_BITS);   // set stall bits
}

//*****************************************************************************
//
//	   FUNCTION: void USB_UnStall(unsigned char brd, unsigned char ep)
//	DESCRIPTION: this function handles un-stalling an endpoint  
//
//	     INPUTS: unsigned char brd - board to use
//               unsigned char ep - endpoint to stall
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
void USB_UnStall(unsigned char brd, unsigned char ep) {

    USB_Write(brd,USB_EPINDEX,EPINDEX_EP0+ep); // select EP

    USB_Write_SR(brd,USB_EPCON,(EPCON_TXSTL | EPCON_RXSTL),USB_REG_NO_BITS);   // clear stall bits
}

//*****************************************************************************
//
//	   FUNCTION: void Check_USB_Xmit(unsigned char brd, unsigned char ep)
//	DESCRIPTION: this function handles transmitting any data, if any  
//
//	     INPUTS: unsigned char brd - the active board
//               unsigned char ep - the active endpoint
//	    RETURNS: nothing		
//        NOTES: see if we have anything to xmit, may be in the middle of a large packet, or may have received
//               data from last check.  if data has been received, the appropriate endpoint buffer will be loaded
//
//*****************************************************************************
void Check_USB_Xmit(unsigned char brd, unsigned char ep) {

    int txsize;
    unsigned char i;
    int fifosize;

    USB_Write(brd,USB_EPINDEX,EPINDEX_EP0+ep); // select EP

    if(Endpoints[brd][ep].xmitbuffercnt) {    // we have characters to xmit
        if(Endpoints[brd][ep].type == USB_ISOC_PIPE){    // figure out actual fifo size
            fifosize = ISOC_FIFO_SIZE[Endpoints[brd][ep].fifosize];
        }
        else {
            fifosize = NON_ISOC_FIFO_SIZE[Endpoints[brd][ep].fifosize];
        }
        if(Endpoints[brd][ep].xmitbuffercnt > fifosize) {  // still more than we can send at once
            txsize = fifosize;
        }
        else { // use all
            txsize = Endpoints[brd][ep].xmitbuffercnt;
        }

        for(i=0;i<txsize;i++) {
            USB_Write(brd,USB_TXDAT,*Endpoints[brd][ep].xmitbufferptr++); // load up transmit buffer
        }
			/* Check for overflow of Tx FIFO */
        if (USB_Read(brd,USB_TXFLG) & TXFLG_TXOVF) {    // check for overflwo
				USB_Write(brd,USB_TXCON, USB_Read(brd,USB_TXCON) | TXCON_TXCLR);
// log error to controller????
                printf("TX%d Overflow\r\n",ep);
        }
        else {
            Endpoints[brd][ep].xmitbuffercnt -= txsize;
            if ( Endpoints[brd][ep].xmitbuffercnt <= 0 ) {
                Endpoints[brd][ep].xmitbuffercnt = 0;
            }
            if(Endpoints[brd][ep].autorepeat) { // if autorepeat is on, reload ptr and count
                Endpoints[brd][ep].xmitbuffercnt = Endpoints[brd][ep].xmitrepeatcnt;
                Endpoints[brd][ep].xmitbufferptr = Endpoints[brd][ep].xmit_buf;
            }
        }
        USB_Write(brd,USB_TXCNTH, ((txsize & 0xff00) >> 8));
        USB_Write(brd,USB_TXCNTL, (txsize & 0x00ff));
    }
    else if(Endpoints[brd][ep].ack) {    // send ack phase
        USB_Write(brd,USB_TXCNTH, 0x00);
        USB_Write(brd,USB_TXCNTL, 0x00);
        Endpoints[brd][ep].ack = 0;
    }
}

