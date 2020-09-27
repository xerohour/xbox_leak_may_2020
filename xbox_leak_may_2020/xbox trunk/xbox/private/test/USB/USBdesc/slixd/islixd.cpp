/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    islixd.cpp

Abstract:

    interface to the slixbox driver

Author:

    Jason Gould (a-jasgou) July 2000

--*/


extern "C" {
#include <nt.h>
#include <ntos.h>
}

#include <xtl.h>

#include <stdio.h>
#include <usb.h>
#include "..\inc\slixdriver.h"
#include "..\inc\i_slixdriver.h"

//#include "slixdriver.cpp"
//extern void SLIX_RemoveDevice (IN IUsbDevice *Device);


///////////////////////#define _WAIT (733000 * 1000 * 30)  //50 milliseconds
#define _TICKS_PER_MILLISECOND 733000I64
const __int64 _WAIT = (_TICKS_PER_MILLISECOND * 1000 * 40);

#define PORT_FROM_ADDRESS(a) ((a) & 0x0f)
#define SLOT_FROM_ADDRESS(a) (((a) >> 4) & 0x7)

#define ENDPOINT_TYPE(e) ((e >> 4) & 3)
#define ENDPOINT_DIRECTION(e) (e & 0x80)

#define ENDPOINT_NUM(e) (ENDPOINT_DIRECTION(e)?(e & 15):((e & 15)+16))                                                  //changed by Bing from 15 to 31

#define ENDPOINT_NUM_DIRECTION(e) (e & 0x8f)


typedef VOID (*PURB_COMPLETE_PROC)(PURB Urb, PVOID Context);

volatile int gYouMayPass = 0;

#pragma warning(disable:4035) 
//disable "no return". rdtsc changes edx:eax, which is where __int64 is returned.
//this incriments roughly 733000000 times/second.
//733000 = 1 millisecond, etc.
__int64 __inline gettsc() {_asm {rdtsc} }	
#pragma warning(default:4035)




VOID CompleteProc (PURB urb, PVOID con)
{
 	gYouMayPass = ~urb->Header.Status;	//encoding it for the moment...
}

VOID IsochCompleteProc (USBD_ISOCH_TRANSFER_STATUS* puits, PVOID con)
{
 	gYouMayPass = ~puits->Status;	//encoding it for the moment...
}


//#define USB_ENDPOINT_TYPE_CONTROL                 0x00
//#define USB_ENDPOINT_TYPE_ISOCHRONOUS             0x01
//#define USB_ENDPOINT_TYPE_BULK                    0x02
//#define USB_ENDPOINT_TYPE_INTERRUPT               0x03
HANDLE GetEndpoint(IUsbDevice * pud, BYTE endpoint)
{
	URB urb;
	int temp;
	void** Endpoints = ((PDEVICE_EXTENSION)pud->GetExtension())->Endpoints;

	if((0 == ENDPOINT_NUM(endpoint)) || (16==ENDPOINT_NUM(endpoint)) ) {	//default (control) endpoint
		if(Endpoints[0] != (void*)-1) {	//if it's been opened already
			return Endpoints[0];		//return it...
		}

		RtlZeroMemory(&urb,sizeof(URB));
		USB_BUILD_OPEN_DEFAULT_ENDPOINT((&urb.OpenEndpoint));
		pud->SubmitRequest(&urb);
		Endpoints[endpoint] = NULL;		//NULL is always returned for the default endpoint handle
		return NULL;

	} else {	//normal endpoint

		if(ENDPOINT_TYPE(endpoint) == USB_ENDPOINT_TYPE_ISOCHRONOUS) { //isoch

			//if it's not already opened...
			if(Endpoints[ENDPOINT_NUM(endpoint)] == (void*) -1 || Endpoints[ENDPOINT_NUM(endpoint)] == NULL) {
				DBGPRINT(3, ("Attempting to open isoch endpoint: "));
				((DEVICE_EXTENSION*)pud->GetExtension())->EndpointType[ENDPOINT_NUM(endpoint)] 
					= USB_ENDPOINT_TYPE_ISOCHRONOUS; //so we can close it properly

				RtlZeroMemory(&urb,sizeof(URB));
                USB_BUILD_ISOCH_OPEN_ENDPOINT((&urb.IsochOpenEndpoint), ENDPOINT_NUM_DIRECTION(endpoint), 256, 0);
				temp = pud->SubmitRequest(&urb);
				DBGPRINT(3, ("returned 0x%8x\n", temp));
				Endpoints[ENDPOINT_NUM(endpoint)] = urb.IsochOpenEndpoint.EndpointHandle;

				URB urb2;
				RtlZeroMemory(&urb2,sizeof(URB));
				USB_BUILD_ISOCH_START_TRANSFER((&urb2.IsochStartTransfer), Endpoints[ENDPOINT_NUM(endpoint)], 
					0, URB_FLAG_ISOCH_START_ASAP);
				temp = pud->SubmitRequest(&urb2);
				DBGPRINT(3, ("StartTransfer returned 0x%8x\n", temp));

			}

			return Endpoints[ENDPOINT_NUM(endpoint)];

		} else { //not isoch
			if(Endpoints[ENDPOINT_NUM(endpoint)] != (void*) -1 && Endpoints[ENDPOINT_NUM(endpoint)] != NULL) {
				return Endpoints[ENDPOINT_NUM(endpoint)];
			}

			RtlZeroMemory(&urb,sizeof(URB));

			USB_BUILD_OPEN_ENDPOINT((&urb.OpenEndpoint), ENDPOINT_NUM_DIRECTION(endpoint), ENDPOINT_TYPE(endpoint), SLIX_MAX_BULK_PACKET_SIZE, 1);
			pud->SubmitRequest(&urb);
			Endpoints[ENDPOINT_NUM(endpoint)] = urb.OpenEndpoint.EndpointHandle;
			return urb.OpenEndpoint.EndpointHandle;
		}

	}
}



 

 


//callback for endpoint closes (below) to allow them to be asynchronous
//but I don't think this is a callback function, it just a stub, may useless called by IUsbDevice class.
//VOID arf (PURB urb, PVOID context)
//{ }

 

void CloseEndpoint(IUsbDevice * pud, BYTE endpoint, HANDLE eh)
{
//	URB urb;
//	int temp;
//	void** Endpoints = ((PDEVICE_EXTENSION)pud->GetExtension())->Endpoints;

//
//	if(0 == ENDPOINT_NUM(endpoint)) {	//default (control) endpoint
//		if(Endpoints[0] == (void*)-1) {	//if it's been closed already
//			return; //Endpoints[0];		//return it...
//		} else {

		//RtlZeroMemory(&urb,sizeof(URB));
//		USB_BUILD_CLOSE_DEFAULT_ENDPOINT(&urb.CloseEndpoint, arf,0);

		//USB_BUILD_OPEN_DEFAULT_ENDPOINT((&urb.OpenEndpoint));
//		pud->SubmitRequest(&urb);
		//Endpoints[endpoint] = NULL;		//NULL is always returned for the default endpoint handle
//		return;// NULL;
//		}

//	} else {//normal endpoint
//		if(ENDPOINT_TYPE(endpoint) == USB_ENDPOINT_TYPE_ISOCHRONOUS) { //isoch

			//if it's not already opened...

			/* we will consider this later on
			if(Endpoints[ENDPOINT_NUM(endpoint)] == (void*) -1 || Endpoints[ENDPOINT_NUM(endpoint)] == NULL) {
				DBGPRINT(3, ("Attempting to open isoch endpoint: "));
				((DEVICE_EXTENSION*)pud->GetExtension())->EndpointType[ENDPOINT_NUM(endpoint)] 
					= USB_ENDPOINT_TYPE_ISOCHRONOUS; //so we can close it properly

				RtlZeroMemory(&urb,sizeof(URB));
                USB_BUILD_ISOCH_OPEN_ENDPOINT((&urb.IsochOpenEndpoint), ENDPOINT_NUM_DIRECTION(endpoint), 256, 0);
				temp = pud->SubmitRequest(&urb);
				DBGPRINT(3, ("returned 0x%8x\n", temp));
				Endpoints[ENDPOINT_NUM(endpoint)] = urb.IsochOpenEndpoint.EndpointHandle;

				URB urb2;
				RtlZeroMemory(&urb2,sizeof(URB));
				USB_BUILD_ISOCH_START_TRANSFER((&urb2.IsochStartTransfer), Endpoints[ENDPOINT_NUM(endpoint)], 
					0, URB_FLAG_ISOCH_START_ASAP);
				temp = pud->SubmitRequest(&urb2);
				DBGPRINT(3, ("StartTransfer returned 0x%8x\n", temp));
            
			}
			*/

//			return; //Endpoints[ENDPOINT_NUM(endpoint)];

//		} else { //not isoch
//			if(Endpoints[ENDPOINT_NUM(endpoint)] == (void*) -1 || Endpoints[ENDPOINT_NUM(endpoint)] == NULL) {
				//already closed
//				return; //Endpoints[ENDPOINT_NUM(endpoint)];
//			} else {
			//RtlZeroMemory(&urb,sizeof(URB));

			//USB_BUILD_OPEN_ENDPOINT((&urb.OpenEndpoint), ENDPOINT_NUM_DIRECTION(endpoint), ENDPOINT_TYPE(endpoint), SLIX_MAX_BULK_PACKET_SIZE, 1);
			
			
//            USB_BUILD_CLOSE_ENDPOINT(&urb.CloseEndpoint,Endpoints[ENDPOINT_NUM(endpoint)] , arf,0);
//			pud->SubmitRequest(&urb);
			//Endpoints[ENDPOINT_NUM(endpoint)] = urb.OpenEndpoint.EndpointHandle;
//			return; // urb.OpenEndpoint.EndpointHandle;
//			}
//		}

//	}

	//intiliaze it again
  // Endpoints[ENDPOINT_NUM(endpoint)]=(void *) -1;
  //((PDEVICE_EXTENSION)pud->GetExtension())->EndpointType[ENDPOINT_NUM(endpoint)] = 0;



}


/*************************************************************************************
Function:   SlixdIn
Purpose:	create & send an IN packet, receive the data, return any errors
Params:     in Address --- the topology of the device to receive from (see PORT_FROM_ADDRESS above)
			in Endpoint --- the endpoint of the device to receive from 
			out data --- pointer to the buffer to receive data
			in out len --- pointer to max length of data buffer, receives size of data received
Return:     a USBD_STATUS code indicating error or success.
*************************************************************************************/
USBD_STATUS SlixdIn(_IN BYTE Address, _IN BYTE Endpoint, _OUT char * data, _IN _OUT DWORD * len, _IN BYTE USBInterface)
{	DEVICE_EXTENSION * de;
	URB urb;
	IUsbDevice * pud;
	LONG templen = 8;
	USBD_STATUS retcode = USBD_STATUS_ERROR;
	HANDLE ourendpoint;
	BYTE closeendpoint = Endpoint;
	int port, slot;
	__int64 maxwaittime;

	port = PORT_FROM_ADDRESS(Address);
	slot = SLOT_FROM_ADDRESS(Address);

	de = &gpDriverExtension->Nodes[port][slot][USBInterface];
	pud = de->Device;

	if (pud == NULL) {
		//the device isn't connected
		DBGPRINT(2,("SlixdIn: Port %d, Slot %d, pud==NULL\n", port, slot));
		*len = 0;
		return USBD_STATUS_ERROR;
	}

	if(!(de->flags & DF_CONNECTED)){
		DBGPRINT(2,("SlixdIn: de->DeviceFlags & DF_CONNECTED"));
		*len = 0;
		return USBD_STATUS_ERROR;
	}
	
	if(data == NULL) {
		if(len != NULL) *len = 0;
		return USBD_STATUS_ERROR;
	}

	if(Endpoint == 0) {
		DBGPRINT(3, ("ERROR! Can't send additional INs to endpoint 0\n"));
		*len = 0;
		return USBD_STATUS_ERROR;
	}

	Endpoint |= 0x80;

	ourendpoint = GetEndpoint(pud, Endpoint);

	if(ourendpoint == NULL && (ENDPOINT_NUM(Endpoint) != 0)) {
		//the endpoint couldn't be opened... if Endpoint == 0, the endpoint handle is always NULL.
		DBGPRINT(3, ("ourendpoint == NULL && Endpoint != 0\n"));
		if(len) *len = 0;
		return USBD_STATUS_ERROR;
	} 

	if(ENDPOINT_TYPE(Endpoint) == USB_ENDPOINT_TYPE_ISOCHRONOUS) {		//isoch
		USBD_ISOCH_BUFFER_DESCRIPTOR bufd;

		bufd.Context = (void*)pud;
		bufd.FrameCount = 1;
		bufd.Pattern[0] = (USHORT)*len;
		bufd.Pattern[1] = 0;
		bufd.TransferBuffer = data;
		bufd.TransferComplete = IsochCompleteProc; 
		RtlZeroMemory(&urb, sizeof(URB));
		USB_BUILD_ISOCH_ATTACH_BUFFER((&urb.IsochAttachBuffer), ourendpoint, USBD_DELAY_INTERRUPT_0_MS, &bufd);
		maxwaittime = _WAIT;	//3.1 milliseconds

	} else {	//non-isoch
		*len = (*len + 63) & ~63;	//round up to the nearest 64 bytes so we don't get buffer overflows
	
		if(!*len) {	//len == 0
			DBGPRINT(3, ("*len == 0!!!\n"));
			*len = 64;
		}
	
		templen = *len;
	
		RtlZeroMemory(&urb,sizeof(URB));
		USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(&urb.BulkOrInterruptTransfer,
			ourendpoint, data, templen, USB_TRANSFER_DIRECTION_IN, 
			CompleteProc, (void*)pud, TRUE);
		maxwaittime = _WAIT;	//roughly 3 milliseconds
		//(if a 64-byte packet comes back, ohci requests another one until our buffer is full)
	}
	gYouMayPass = 0;

	__int64 tim = gettsc();
	retcode = pud->SubmitRequest(&urb);

	while(!gYouMayPass && (gettsc() - tim) < maxwaittime) { }	//wait to time out, or for request to come back

	if((gettsc() - tim) > maxwaittime) {	//if we timed out
		pud->CancelRequest(&urb);			//cancel request
		while(!gYouMayPass) { }				//and wait for the thing to cancel
	}

	//we got a response!
	if(~gYouMayPass != USBD_STATUS_CANCELED) {
		retcode = ~gYouMayPass;
		if(retcode != USBD_STATUS_SUCCESS) {
			DBGPRINT(3, ("request completed, with error: %x\n"
				"returned data size: %d, first 8 bytes = %016I64x\n", 
				retcode, urb.BulkOrInterruptTransfer.TransferBufferLength, *(INT64*)data));
		}
	}

	if(ENDPOINT_TYPE(Endpoint) == USB_ENDPOINT_TYPE_ISOCHRONOUS) { 
		//*len = *len //hope that we really got that much data back
	} else {
		*len = (USHORT)urb.BulkOrInterruptTransfer.TransferBufferLength;
	}

	//CloseEndpoint(pud, Endpoint, ourendpoint);
	CloseEndpoint(pud, closeendpoint, ourendpoint);

	return retcode;
}

/*************************************************************************************
Function:   SlixdOut
Purpose:	create & send an OUT packet, send the data, return any errors
Params:     in Address --- the topology of the device to send to (see PORT_FROM_ADDRESS above)
			in Endpoint --- the endpoint of the device to send to
			in data --- pointer to the buffer to receive data
			in len --- number of bytes to send
Return:     a USBD_STATUS code indicating error or success.
*************************************************************************************/
USBD_STATUS SlixdOut(_IN BYTE Address, _IN BYTE Endpoint, _IN char * data, _IN DWORD len, _IN BYTE USBInterface)
{
	DEVICE_EXTENSION * de;
	URB urb;
	IUsbDevice * pud;
	LONG temp;
	USBD_STATUS retcode = USBD_STATUS_ERROR;
	HANDLE ourendpoint;
	int port, slot;
	__int64 maxwaittime;

	port = PORT_FROM_ADDRESS(Address);
	slot = SLOT_FROM_ADDRESS(Address);

	de = &gpDriverExtension->Nodes[port][slot][USBInterface];
	pud = de->Device;

	if (pud == NULL) {
		//the device isn't connected
		return USBD_STATUS_ERROR;
	}

	if(Endpoint == 0) {
		DBGPRINT(3,("ERROR! Can't send additional OUTs to endpoint 0\n"));
//		*len = 0;
		return USBD_STATUS_ERROR;
	}

	ourendpoint = GetEndpoint(pud, Endpoint);
	
	if(data == NULL) {
		if(len != 0) {
			return USBD_STATUS_ERROR;	//trying to send data that doesn't exist??
		}
	}

	//if(ourendpoint == NULL && Endpoint != 0) {
	if(ourendpoint == NULL && (ENDPOINT_NUM(Endpoint) != 0)) {                                           //changed by Bing from above line to this, this should be the same as SlixDIN().
		//the endpoint couldn't be opened... if Endpoint == 0, the endpoint handle is always NULL.
		return USBD_STATUS_ERROR;
	}


	if(ENDPOINT_TYPE(Endpoint) == USB_ENDPOINT_TYPE_ISOCHRONOUS) {		//isoch
		USBD_ISOCH_BUFFER_DESCRIPTOR bufd;

		bufd.Context = (void*)pud;
		bufd.FrameCount = 1;
		bufd.Pattern[0] = (USHORT)len;
		bufd.Pattern[1] = 0;
		bufd.TransferBuffer = data;
		bufd.TransferComplete = IsochCompleteProc; 
		RtlZeroMemory(&urb, sizeof(URB));
		USB_BUILD_ISOCH_ATTACH_BUFFER((&urb.IsochAttachBuffer), ourendpoint, USBD_DELAY_INTERRUPT_0_MS, &bufd);
		maxwaittime = _WAIT;	//3.1 milliseconds

	} else {	//non-isoch

		RtlZeroMemory(&urb,sizeof(URB));
		USB_BUILD_BULK_OR_INTERRUPT_TRANSFER(&urb.BulkOrInterruptTransfer,
			ourendpoint, data, len, USB_TRANSFER_DIRECTION_OUT, 
			CompleteProc, (void*)pud, TRUE);
		maxwaittime = _WAIT;		//wait 3 milliseconds

	}
	gYouMayPass = 0;



	__int64 tim = gettsc();
	retcode = pud->SubmitRequest(&urb);

	while(!gYouMayPass && (gettsc() - tim) < maxwaittime) { }	//wait to time out, or for response

	if((gettsc() - tim) > maxwaittime) {
		pud->CancelRequest(&urb);
		while(!gYouMayPass) { }
	}

	if(~gYouMayPass != USBD_STATUS_CANCELED) {
		retcode = ~gYouMayPass;
		if(retcode != USBD_STATUS_SUCCESS) {
			DBGPRINT(3, ("request completed, with error: %x\n", retcode));
			DBGPRINT(3, ("returned data size: %d, first 8 bytes = %016I64x\n", 
				urb.BulkOrInterruptTransfer.TransferBufferLength, *(INT64*)data));
		}
	}

	CloseEndpoint(pud, Endpoint, ourendpoint);

	return retcode;
}

/*************************************************************************************
Function:   SlixdSetup
Purpose:	create & send a SETUP packet, send the data, return any errors
note: more stuff may be needed to receive or send additional data...
Params:     in Address --- the topology of the device to send to (see PORT_FROM_ADDRESS above)
			in Endpoint --- the endpoint of the device to send to
			in data --- pointer to the buffer to receive data
			in len --- number of bytes to send
			data2 --- pointer to buffer to receive or send data. OPT if data[6]==0
			len2 --- p to length of data2 if sending, p to max length if receiving, & 
				will receive length of data received. Not optional.
Return:     a USBD_STATUS code indicating error or success.
*************************************************************************************/
USBD_STATUS SlixdSetup(_IN BYTE Address, _IN BYTE Endpoint, _IN char * data, _IN DWORD len, 
					   _OPT _IN _OUT char * data2, _IN _OUT USHORT *len2, _IN BYTE USBInterface)
{
	DEVICE_EXTENSION * de;
	URB urb;
	IUsbDevice * pud;
	LONG temp;
	USBD_STATUS retcode = USBD_STATUS_ERROR;
	HANDLE ourendpoint;
	USHORT extlen;
	BYTE direction;
	int port, slot;
	__int64 maxwaittime;

	port = PORT_FROM_ADDRESS(Address);
	slot = SLOT_FROM_ADDRESS(Address);

	de = &gpDriverExtension->Nodes[port][slot][USBInterface];
	pud = de->Device;

	if (pud == NULL) {
		//the device isn't connected
		return USBD_STATUS_ERROR;
	}

	if(data == NULL) {
		DBGPRINT(1, ("data == NULL"));
		return USBD_STATUS_ERROR;
	} else if(len != 8) {
		DBGPRINT(1, ("Setup packets should be 8 bytes! len = %d, data = %016I64x, len2 = %d", len, *(INT64*)data, *len2));
		return USBD_STATUS_ERROR;
	} 

	extlen = (*(USHORT*)&data[6]); //get the length of the data to send/recv
	if(!len2) {
		DBGPRINT(1, ("Setup: len2 is NOT optional!"));
		return USBD_STATUS_ERROR;
	} else if(*len2 < extlen) {
		DBGPRINT(1, ("sending data without enough buffer length?? len2: %d, extlen: %d", *len2, extlen));
		return USBD_STATUS_ERROR;
	}

	if(*len2 > 2048) *len2 = 2048;	//can't send or expect too much data, due to ohci requrements

	if(*len2 > 0) {
		if(data2 == NULL) {
			DBGPRINT(1, ("data2 == NULL"));
			return USBD_STATUS_ERROR;
		}

		direction = (data[0] & 128) ? USB_TRANSFER_DIRECTION_IN : USB_TRANSFER_DIRECTION_OUT;
	} else {
		direction = 0;
	}

	ourendpoint = GetEndpoint(pud, Endpoint);

	if(ourendpoint == NULL && Endpoint != 0) {
		//the endpoint couldn't be opened... if Endpoint == 0, the endpoint handle is always NULL.
		return USBD_STATUS_ERROR;
	}
	
	RtlZeroMemory(&urb,sizeof(URB));
	USB_BUILD_CONTROL_TRANSFER(&urb.ControlTransfer,
		ourendpoint, data2, *len2, direction,		////////////////////data2
		CompleteProc, (void*)pud, TRUE,
		data[0], data[1], (*(USHORT*)&data[2]), (*(USHORT*)&data[4]), (*(USHORT*)&data[6]) );


	gYouMayPass = 0;
	__int64 tim = gettsc();
	retcode = pud->SubmitRequest(&urb);

    maxwaittime = _WAIT;        //wait 3 milliseconds
	
    while(!gYouMayPass && (gettsc() - tim) < maxwaittime) { }               //wait for 3 milliseconds, or for the response.

	if((gettsc() - tim) > maxwaittime) {
		pud->CancelRequest(&urb);
		while(!gYouMayPass) { }
	}

	if(~gYouMayPass != USBD_STATUS_CANCELED) {
		retcode = ~gYouMayPass;
		if(retcode != USBD_STATUS_SUCCESS) {
			DBGPRINT(3, ("request completed, with error: %x\n", retcode));
			DBGPRINT(3, ("returned data size: %d, first 8 bytes = %016I64x\n", 
				urb.BulkOrInterruptTransfer.TransferBufferLength, *(INT64*)data));
		}
	}


	*len2 = (USHORT)urb.ControlTransfer.TransferBufferLength;

	CloseEndpoint(pud, Endpoint, ourendpoint);
	return retcode;
}


