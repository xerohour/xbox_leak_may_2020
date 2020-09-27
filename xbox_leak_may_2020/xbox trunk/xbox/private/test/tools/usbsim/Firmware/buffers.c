//*****************************************************************************
//	    Filename: buffers.c
//       Project: Microsoft USB Simulator
//	   Copyright: Microsoft, 2000
//
//   Description: This is the source file for the buffers module.  It contains
//				  code to support communication between the network channel
//                and the USB ports.
//
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/buffers.c $
// $Revision: 1.5 $
// $Date: 2001/03/01 13:24:13Z $ 
// $Author: klpayne $
// $Log: buffers.c $
// Revision 1.5  2001/03/01 13:24:13Z  klpayne
// Add error printout if buffer overrun
// Revision 1.4  2001/01/29 16:07:29Z  klpayne
// Change xmit/recv buffers to allow 1024 size.
// Revision 1.3  2000/12/20 18:04:40Z  klpayne
// Zero'd out size field after using
// Revision 1.2  2000/12/19 13:16:12Z  klpayne
// Initial implementation of buffers.
// Revision 1.1  2000/12/06 19:40:02Z  rjgardner
// Initial revision
//
//*****************************************************************************
#include <mem.h>
#include <stdio.h>
#include "buffers.h"

//------------------------LOCAL CONSTANTS AND MACROS---------------------------

//-----------------------------GLOBAL VARIABLES--------------------------------
static struct xmit_buffer_queue Sock_201_Xmit_Buf;
static struct rcv_buffer_queue Sock_201_Recv_Buf;
static struct xmit_buffer_queue Sock_202_Xmit_Buf;
static struct rcv_buffer_queue Sock_202_Recv_Buf;
static struct xmit_buffer_queue Sock_203_Xmit_Buf;
static struct rcv_buffer_queue Sock_203_Recv_Buf;
static struct xmit_buffer_queue Sock_204_Xmit_Buf;
static struct rcv_buffer_queue Sock_204_Recv_Buf;
//----------------------STATIC LOCAL FUNCTION PROTOTYPES-----------------------

//----------------------PUBLIC FUNCTIONS---------------------------------------
//*****************************************************************************
//
//	   FUNCTION: unsigned int Get_Buffer(unsigned char buf, unsigned char *ptr)
//	DESCRIPTION: this function returns data from the selected buffer   
//
//	     INPUTS: unsigned char buf - which buffer to retrieve from
//				 unsigned char *ptr - pointer to load data into
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
unsigned int Get_Buffer(unsigned char buf, unsigned char *ptr) {

    unsigned char indx;
    unsigned int size;
    unsigned char *sptr;
    struct xmit_buffer_queue *xptr;
    struct rcv_buffer_queue *rptr;
    
    if((buf == SOCK_201_XMIT) || (buf == SOCK_202_XMIT) || (buf == SOCK_203_XMIT) || (buf == SOCK_204_XMIT)) { // its a xmit buffer 
        switch (buf) {
            case SOCK_201_XMIT: // socket 201 xmit
                xptr = &Sock_201_Xmit_Buf;
                break;
            case SOCK_202_XMIT: // socket 202 xmit
                xptr = &Sock_202_Xmit_Buf;
                break;
            case SOCK_203_XMIT: // socket 203 xmit
                xptr = &Sock_203_Xmit_Buf;
                break;
            case SOCK_204_XMIT: // socket 204 xmit
                xptr = &Sock_204_Xmit_Buf;
                break;
        }
        if(xptr->inindx == xptr->outindx) {   // no data in buffers
            return(0);  // return 0 size
        }
        
        indx = xptr->outindx; // set up index into buffer array
        size = xptr->element[indx].size;
        sptr = xptr->element[indx].data;
        memcpy(ptr,sptr,size);
        xptr->element[indx].size = 0;   // make sure it doesn't accidently get reused
    
        xptr->outindx++;
        if(xptr->outindx == NUM_XMIT_ELEMENTS) {    // wrap index
            xptr->outindx = 0;
        }
        return size;
    }
    else {
        switch (buf) {
            case SOCK_201_RCV: // socket 201 recv
                rptr = &Sock_201_Recv_Buf;
                break;
            case SOCK_202_RCV: // socket 202 recv
                rptr = &Sock_202_Recv_Buf;
                break;
            case SOCK_203_RCV: // socket 203 recv
                rptr = &Sock_203_Recv_Buf;
                break;
            case SOCK_204_RCV: // socket 204 recv
                rptr = &Sock_204_Recv_Buf;
                break;
        }
        if(rptr->inindx == rptr->outindx) {   // no data in buffers
            return(0);  // return 0 size
        }
        
        indx = rptr->outindx; // set up index into buffer array
        size = rptr->element[indx].size;
        sptr = rptr->element[indx].data;
        memcpy(ptr,sptr,size);
        rptr->element[indx].size = 0;   // make sure it doesn't accidently get reused
    
        rptr->outindx++;
        if(rptr->outindx == NUM_RCV_ELEMENTS) {    // wrap index
            rptr->outindx = 0;
        }
        return size;
    }

}

//*****************************************************************************
//
//	   FUNCTION: void Put_Buffer(unsigned char buf, unsigned char *ptr, unsigned int size)
//	DESCRIPTION: this function loads data into the selected buffer   
//
//	     INPUTS: unsigned char buf - which buffer to load data to
//				 unsigned char *ptr - pointer to load data from
//               unsigned int size - size of data to load
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
void Put_Buffer(unsigned char buf, unsigned char *ptr, unsigned int size) {

    unsigned char indx;
    unsigned char *dptr;
    struct xmit_buffer_queue *xptr;
    struct rcv_buffer_queue *rptr;
    
    if((buf == SOCK_201_XMIT) || (buf == SOCK_202_XMIT) || (buf == SOCK_203_XMIT) || (buf == SOCK_204_XMIT)) { // its a xmit buffer 
        switch (buf) {
            case SOCK_201_XMIT: // socket 201 xmit
                xptr = &Sock_201_Xmit_Buf;
                break;
            case SOCK_202_XMIT: // socket 202 xmit
                xptr = &Sock_202_Xmit_Buf;
                break;
            case SOCK_203_XMIT: // socket 203 xmit
                xptr = &Sock_203_Xmit_Buf;
                break;
            case SOCK_204_XMIT: // socket 204 xmit
                xptr = &Sock_204_Xmit_Buf;
                break;
        }
        indx = xptr->inindx; // set up index into buffer array
        dptr = xptr->element[indx].data;
        memcpy(dptr,ptr,size);
        xptr->element[indx].size = size;
    
        xptr->inindx++;
        if(xptr->inindx == NUM_XMIT_ELEMENTS) {    // wrap index
            xptr->inindx = 0;
        }
        if(xptr->inindx == xptr->outindx) {   // we have problem
            printf("Buffer overrun at %d\r\n",xptr->inindx);      
        }  
    }
    else {
        switch (buf) {
            case SOCK_201_RCV: // socket 201 recv
                rptr = &Sock_201_Recv_Buf;
                break;
            case SOCK_202_RCV: // socket 202 recv
                rptr = &Sock_202_Recv_Buf;
                break;
            case SOCK_203_RCV: // socket 203 recv
                rptr = &Sock_203_Recv_Buf;
                break;
            case SOCK_204_RCV: // socket 204 recv
                rptr = &Sock_204_Recv_Buf;
                break;
        }
        indx = rptr->inindx; // set up index into buffer array
        dptr = rptr->element[indx].data;
        memcpy(dptr,ptr,size);
        rptr->element[indx].size = size;
    
        rptr->inindx++;
        if(rptr->inindx == NUM_RCV_ELEMENTS) {    // wrap index
            rptr->inindx = 0;
        }
    }

}

//*****************************************************************************
//
//	   FUNCTION: void Init_Buffers(void)
//	DESCRIPTION: this function initializes the buffers   
//
//	     INPUTS: none
//	    RETURNS: nothing		
//        NOTES: 
//
//*****************************************************************************
void Init_Buffers(void) {

    unsigned char i;

    Sock_201_Xmit_Buf.inindx = Sock_201_Xmit_Buf.outindx = 0; // init indicies to beginning
    for(i=0;i<NUM_XMIT_ELEMENTS;i++) {
        Sock_201_Xmit_Buf.element[i].size = 0; // just a precation
    }
    Sock_201_Recv_Buf.inindx = Sock_201_Recv_Buf.outindx = 0; // init indicies to beginning
    for(i=0;i<NUM_RCV_ELEMENTS;i++) {
        Sock_201_Recv_Buf.element[i].size = 0; // just a precation
    }
        
    Sock_202_Xmit_Buf.inindx = Sock_202_Xmit_Buf.outindx = 0; // init indicies to beginning
    for(i=0;i<NUM_XMIT_ELEMENTS;i++) {
        Sock_202_Xmit_Buf.element[i].size = 0; // just a precation
    }
    Sock_202_Recv_Buf.inindx = Sock_202_Recv_Buf.outindx = 0; // init indicies to beginning
    for(i=0;i<NUM_RCV_ELEMENTS;i++) {
        Sock_202_Recv_Buf.element[i].size = 0; // just a precation
    }
        
    Sock_203_Xmit_Buf.inindx = Sock_203_Xmit_Buf.outindx = 0; // init indicies to beginning
    for(i=0;i<NUM_XMIT_ELEMENTS;i++) {
        Sock_203_Xmit_Buf.element[i].size = 0; // just a precation
    }
    Sock_203_Recv_Buf.inindx = Sock_203_Recv_Buf.outindx = 0; // init indicies to beginning
    for(i=0;i<NUM_RCV_ELEMENTS;i++) {
        Sock_203_Recv_Buf.element[i].size = 0; // just a precation
    }
        
    Sock_204_Xmit_Buf.inindx = Sock_204_Xmit_Buf.outindx = 0; // init indicies to beginning
    for(i=0;i<NUM_XMIT_ELEMENTS;i++) {
        Sock_204_Xmit_Buf.element[i].size = 0; // just a precation
    }
    Sock_204_Recv_Buf.inindx = Sock_204_Recv_Buf.outindx = 0; // init indicies to beginning
    for(i=0;i<NUM_RCV_ELEMENTS;i++) {
        Sock_204_Recv_Buf.element[i].size = 0; // just a precation
    }
        
}




