/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    slixbox.cpp

Abstract:

    slixbox main loop, initialization, and other stuff

Author:

    Jason Gould (a-jasgou) June 2000

--*/
#include "slixbox.h"
#include "slixdriver.h"
//#include "..\..\..\..\public\ddk\inc\usb.h"

//#include <mmsystem.h>







//__inline void free(void * a) {delete[] a; }
//__inline void * malloc(size_t s) {return (void*)(new char[s]); }

#pragma warning(disable : 4035)
INT64 __inline GetTime() { 	_asm rdtsc }
#pragma warning(default : 4035)
#define measure(Description, code) {INT64 _ttt1, _ttt2; _ttt1 = GetTime(); code; _ttt2 = GetTime(); DebugPrint(Description ": %I64d\n", (_ttt2 - _ttt1) / 733);}

//extern "C" void  __stdcall free(void*);
//extern "C" void* __stdcall malloc(size_t);
BYTE UsbdCodeToSliCode(USBD_STATUS u, BOOL isIn);

SOCKET gSock;

//
//  Call USB's init function directly since we do not link to xapi.
//

/*************************************************************************************
Function:   init
Purpose:	do all initialization for slixbox
Params:     none
Return:     0 if good, non-0 if bad
*************************************************************************************/
int init()
{
	gSock = ssInitServer();
	return 0;
}

extern SOCKET ggSock;

#define SETUP_IN 1
#define SETUP_OUT 2
#define SETUP_NONE 0
typedef struct {
	char * data2;		//to be malloced as needed. Holds the big data buffer
	char data[8];		//the main setup packet
	long datasize;		//size of data[]
	BYTE type;			//SETUP_IN, SETUP_OUT, or SETUP_NONE, depending on the type of setup
	USHORT expected;	//total size of data2, the number of bytes to send to either islixboxd or slix
	USHORT current;		//number of bytes sent so far to islixboxd or slix
} SetupBuffer;

typedef struct _QUEUE {
	struct _QUEUE* next;
	TRANSFERDATA * d;
} QUEUE, *PQUEUE;

QUEUE q = {0,0};

SetupBuffer sbs[128][16];	//[address][endpoint] //////port //slot //endpoint

typedef void (CALLBACK TIMECALLBACK)(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

BYTE parse(TRANSFERDATA * c, BOOL isqueue) 
{
	TRANSFERDATA t;
	SetupBuffer *tempsb;
	USBD_STATUS temp;
	int i;
	USHORT sTemp = 0;
	int size;
	static int slixdout=0,slixdin=0;


	switch(c->type) {	//the type of packet we receive
	case PID_INIT_IN:	//a request for an IN
		if(sbs[c->initin.address][c->initin.endpoint].type != SETUP_IN) {
			if(c->initin.endpoint == 0) {
				t.in.ret = XSTATUS_UserError;
			} else {
				//normal in request... (no setup packet buffering)
				//send in the request!
				slixdin++;
				temp = SlixdIn(c->initin.address, c->initin.endpoint, &t.in.data[0], &c->initin.max);
				t.in.ret = UsbdCodeToSliCode(temp, TRUE);	//prepare the return code
//				DebugPrint("Printing from Slixbox.cpp->parse->IN, Slixdout=%d, Slixdin=%d\n", slixout, slixdin);
//				DebugPrint("from SlixdIn, code %02x, data %d %016I64x\n", t.in.ret, c->initin.max, *(__int64*)&t.in.data[0]);
			}

			t.type = PID_IN;							//and the type of packet we're sending back
			t.size = c->initin.max + sizeof(TRANSFER_IN);//size of packet we're sending

			if(isqueue) return t.in.ret;

			temp = send(gSock, (char*)&t, t.size, 0);			//send the packet

//				DebugPrint("send returned %d\n", temp);
			if(temp <= 0) DebugPrint("send error: %d\n", WSAGetLastError());
			break;										//and go back to the main loop again
			
		} else {
			//DebugPrint("buffered in...\n");
			//we're doing setup packet buffering for this address!
			tempsb = &sbs[c->initin.address][c->initin.endpoint];

			t.size = (sizeof(TRANSFER_IN) +	//size of the packet to send...
				min((signed)c->initin.max, (tempsb->expected - tempsb->current)));

			t.type = PID_IN;	//type of the packet to send
			t.in.ret = XSTATUS_Success;	//"handshake" to send back

			if (tempsb->current >= tempsb->expected && ((t.size - sizeof(TRANSFER_IN)) < 8)) {	//if we've sent all the data...
				//DebugPrint("All data sent. (current %d, expected %d) Setup packet is completed...\n", tempsb->current, tempsb->expected);
				free(tempsb->data2);					//free our buffer
                tempsb->data2 = NULL;
				tempsb->type = SETUP_NONE;				//and stop buffering this endpoint
			}


			for(i = 0; i < (signed)t.size - (signed)sizeof(TRANSFER_IN); i++) {		//copy the data to the slixboxsock buffer
				t.in.data[i] = tempsb->data2[tempsb->current++];
			}
			
			if(isqueue) return t.in.ret;

			temp = send(gSock, (char*)&t, t.size, 0);				//& send that data !
//				DebugPrint("send returned %d\n", temp);
			if(temp <= 0) DebugPrint("send error: %d\n", WSAGetLastError());

			break;
		}

	case PID_OUT:
		if(sbs[c->out.address][c->out.endpoint].type != SETUP_OUT) {
			//normal OUT operation
//				DebugPrint("Out request received\n");

			//send the data out!
			slixdout++;
			temp = SlixdOut(c->out.address, c->out.endpoint, c->out.data, c->out.size - sizeof(c->out));

			t.ret.ret = UsbdCodeToSliCode(temp, FALSE);		//get our return code
			

			t.type = PID_RET;								//prepare our packet
			t.size = sizeof(TRANSFER_RETURN);				//size of the packet

			if(isqueue) return t.ret.ret;
			
			temp = send(gSock, (char*)&t, t.size, 0);		// & send the packet!
			if(temp <= 0) DebugPrint("send error: %d\n", WSAGetLastError());
			break;

		} else {
			//DebugPrint("Buffered Out...\n");
			//we're doing SETUP packet buffering...
			tempsb = &sbs[c->out.address][c->out.endpoint];

			c->out.size = min((signed)c->out.size, (tempsb->expected - tempsb->current)); //so we don't over-run data2
//                       (signed)c->out.size - (signed)sizeof(TRANSFER_OUT); i++){	//copy the data into the buffer for islixboxd
			for(i=0; i < (signed)c->out.size; i++){	//copy the data into the buffer for islixboxd
				tempsb->data2[tempsb->current++] = c->out.data[i];
			}

			//if we have enough to send...
			if(tempsb->current >= tempsb->expected) {
				//DebugPrint("Sending our buffer, %d bytes", tempsb->expected);
				//sned the stuff to islixboxd
				temp = SlixdSetup(c->out.address, c->out.endpoint, 
					tempsb->data, tempsb->datasize, 
					tempsb->data2, &tempsb->expected);

				free(tempsb->data2);	//release the data buffer
                tempsb->data2 = NULL;
				tempsb->type = SETUP_NONE;	//and stop buffering this endpoint

				t.ret.ret = UsbdCodeToSliCode(temp, FALSE); //prepare the return value

				t.type = PID_RET;							//prepare the slixboxsock packet ...
				t.size = sizeof(TRANSFER_RETURN);
				
				if(isqueue) return t.ret.ret;

				temp = send(gSock, (char*)&t, t.size, 0);	//send that packet
				if(temp <= 0) DebugPrint("send error: %d\n", WSAGetLastError());

				break;
			} else {
				//DebugPrint("Bytes collected: %d, bytes expected: %d\n", tempsb->current, tempsb->expected);
				//produce a fake packet to send back, so recv is happy on slix.
				t.ret.ret = XSTATUS_Ack;


				t.type = PID_RET;
				t.size = sizeof(TRANSFER_RETURN);
				
				if(isqueue) return t.ret.ret;

				temp = send(gSock, (char*)&t, t.size, 0);
				if(temp <= 0) DebugPrint("send error: %d\n", WSAGetLastError());
				break;
			}
		}

	case PID_SETUP:
//			DebugPrint("PID_SETUP received");
		sTemp = *(USHORT*)&(c->setup.data[6]); //get the USHORT stored starting at &data[6]
		tempsb = &sbs[c->setup.address][c->setup.endpoint];
		size = c->setup.size - sizeof(c->setup);

		if(sTemp == 0 ) {
			//this is a "no-data" setup transfer (just the setup packet)
//			DebugPrint("no-data setup transfer.\n");

			//send our data!
//				DebugPrint("SlixBox Setup: address %x, endpoint %x, data %16I64x, size %d, size2 %d", c->setup.address, c->setup.endpoint, *(INT64*)c->setup.data, size, sTemp);
			temp = SlixdSetup(c->setup.address, c->setup.endpoint, c->setup.data, size, NULL, &sTemp);
//				DebugPrint("SlixdSetup returned %x", temp);

			t.ret.ret = UsbdCodeToSliCode(temp, FALSE);	//prepare the return value
			

			t.type = PID_RET;							//and prepare the return packet
			t.size = sizeof(TRANSFER_RETURN);

			if(isqueue) return t.ret.ret;
			
			temp = send(gSock, (char*)&t, t.size, 0);			//send the return packet
			if(temp <= 0) DebugPrint("send error: %d\n", WSAGetLastError());
			break;

		} else {
			//this setup transfer requires some data!
//			DebugPrint("setup requires data.\n");

			if(tempsb->data2) {			//if we didn't get around to freeing the data before... (it could happen)
				free(tempsb->data2);	//do it now...
                tempsb->data2 = NULL;
			}

			//prepare the setup buffering structure
			
			tempsb->expected = *(USHORT*)&c->setup.data[6];				//size of data2 to buffer
			tempsb->data2 = (char*)malloc(*(USHORT*)&c->setup.data[6]);	//create that buffer
			tempsb->current = 0;										//we haven't collected any yet!

			if(c->setup.data[0] & 0x80) {								
				//this is a SETUP_IN transfer, so we do the INs now, & send data to slix when requested

				tempsb->type = SETUP_IN;		

				//send the setup packet, get the data back
				temp = SlixdSetup(c->setup.address, c->setup.endpoint, c->setup.data, size, 
					tempsb->data2, &tempsb->expected);

//				DebugPrint("Setup requires INs. %d bytes buffered.\n", tempsb->expected);

			} else {
				//this is a SETUP_OUT transfer, so we will wait to collect the data from the user.

				//store our setup packet
				for(i = 0; i < size; i++) {
					tempsb->data[i] = c->setup.data[i];
				}
				tempsb->datasize = size;		//size of the setup packet
				tempsb->type = SETUP_OUT;

				temp = USBD_STATUS_SUCCESS;		//bogus return value ... 
			}
		}
	
		t.ret.ret = UsbdCodeToSliCode(temp, FALSE);	//convert the return code


		t.type = PID_RET;							//prepare the return packet
		t.size = sizeof(TRANSFER_RETURN);
		
		if(isqueue) return t.ret.ret;

		temp = send(gSock, (char*)&t, t.size, 0);	//send the return packet	
		if(temp <= 0) DebugPrint("send error: %d\n", WSAGetLastError());
		break;

	case PID_DATA0:
		//data toggle tweaks... 
//			DebugPrint("data0 requested\n");
		SlixdData0(c->toggle.endpoint);

		if(isqueue) return XSTATUS_Success;
		
		break;
	case PID_DATA1:
//			DebugPrint("data1 requested\n");
		//data toggle tweaks... 
		SlixdData1(c->toggle.endpoint);
		
		if(isqueue) return XSTATUS_Success;
		
		break;
	case PID_RESET:
		temp = SlixdReset();

		t.ret.ret = UsbdCodeToSliCode(temp, TRUE);


		t.type = PID_RET;
		t.size = sizeof(TRANSFER_RETURN);

		if(isqueue) return t.ret.ret;
		
		temp = send(gSock, (char*)&t, t.size, 0);
		if(temp <= 0) DebugPrint("send error: %d\n", WSAGetLastError());

		break;
	default:
		DebugPrint("ssRecv received data of unknown type 0x%02x.\n", c->type);
		return XSTATUS_TransmissionError;
		break;
	}
	return XSTATUS_Success;
}

/*************************************************************************************
Function:   main
Purpose:	entrypoint, main loop, etc
Params:     none
Return:     it shouldn't ever... this is an xbox app.
*************************************************************************************/
void __cdecl main()
{
	TRANSFERDATA c;	//in-bound stuff

	TRANSFERDATA t;	//out-bound stuff

    XInitDevices(0,NULL);
	
	DebugPrint("sizeof(TRANSFERDATA) (should be 256): %d\n", sizeof(TRANSFERDATA));

 	DebugPrint("SliXbox has begun!\n");
	int r;
	int count = 3;
	USHORT sTemp = 0;
	int size;
	int i;
	SetupBuffer *tempsb;
	USBD_STATUS temp;
	int queue = 0;
	QUEUE *pq, *pq2;
	BYTE NakIsOK;

	//[topology][endpoint] // even though endpoint should only be 0...
	//this holds all the stuff for the setup packet "buffering"
	
	memset(sbs, 0, sizeof(sbs));//zero the sbs array

	while ((r = init()) != 0) {	//keep trying to init since there is no point in quitting
		DebugPrint("init failed with return value %d. Trying again.\n", r);
		Sleep(2000);
	}
	
	DebugPrint("init worked!\n");

	while(1) {
		temp = recv(gSock, (char*)&c, 256, 0);

		if(temp <= 0) {
			if(WSAGetLastError() == 10103) {
				continue;
			}

			DebugPrint("recv failed, error %d\n", WSAGetLastError());
			if (SOCKET_ERROR == closesocket(gSock))	{
				DebugPrint("closesocket failed, error %d\n", WSAGetLastError());
			}
			if (SOCKET_ERROR == listen(ggSock, 1)) {
				DebugPrint("listen failed, error %d\n", WSAGetLastError());
			}
			gSock = accept(ggSock, NULL, NULL);
			if(gSock == INVALID_SOCKET) {
				DebugPrint("accept failed, error %d\n", WSAGetLastError());
			}

			continue;
		} else {
		}

		switch(c.type) {	//the type of packet we receive
		case PID_BEGIN_QUEUE:
			if(queue) {
				DebugPrint("Error! Queue already in progress!\n");
				break;
			}
			NakIsOK = c.beginqueue.successonnak;
			queue = 1;
			pq = &q;
			break;
		case PID_INIT_END_QUEUE:
			t.type = PID_RETURN_END_QUEUE;
			t.size = sizeof(TRANSFER_RETURN_END_QUEUE);
			t.retendqueue.linefailed = 0;
			t.retendqueue.ret = XSTATUS_Success;
			if(!queue) {
				DebugPrint("Error! no queue to end!\n");
				break;
			}
			for(pq = q.next; pq;)	{
				if(0 == t.retendqueue.linefailed) {
					temp = parse((TRANSFERDATA*)pq->d, TRUE);
				}
				pq2 = pq;
				pq = pq->next;
				free(pq2);
                pq2 = NULL;
				if((temp == XSTATUS_Ack) || (temp == XSTATUS_Success) || ((temp == XSTATUS_Nak) && NakIsOK)) {
				} else {
					t.retendqueue.linefailed = queue;
					t.retendqueue.ret = (BYTE)temp;
				}
				queue++;
			}
			temp = send(gSock, (char*)&t, t.size, 0);			//send the packet
			if(temp <= 0) DebugPrint("send error: %d\n", WSAGetLastError());
			queue = 0;
			break;
		default:
			if(queue == 0) {
				parse(&c, FALSE);
			} else {
				pq->next = (QUEUE*)malloc(sizeof(QUEUE));
				pq = pq->next;
				pq->next = 0;
				pq->d = (TRANSFERDATA*)malloc(c.size);
				memcpy(pq->d, &c, c.size);
			}
			break;
		}

	} 
}



/*************************************************************************************
Function:   UsbdCodeToSliCode
Purpose:	convert status codes from usbd to slix
Params:		u --- the usbd status code to convert
			isIn --- TRUE if this code is from an IN, FALSE if it is from an OUT or SETUP
Return:		the sli status code
Note:		if (isIn), USBD_STATUS_SUCCESS becomes STATUS_Success, otherwise, XSTATUS_Ack
*************************************************************************************/
BYTE UsbdCodeToSliCode(USBD_STATUS u, BOOL isIn)
{
	switch (u)
	{
	case USBD_STATUS_SUCCESS:
		if(isIn)
			return XSTATUS_Success;
		else
			return XSTATUS_Ack;
	case USBD_STATUS_PENDING:
	case 0xC000000F:			//this "error" doesn't seem to be defined anywhere, 
								//but it is sent to the callback when the request is cancelled.
		return XSTATUS_Nak;

	case USBD_STATUS_CRC:
		return XSTATUS_DataCRCError;
	case USBD_STATUS_BTSTUFF:
		return XSTATUS_SyncError;
	case USBD_STATUS_DATA_TOGGLE_MISMATCH:
		return XSTATUS_DataToggleError;
	case USBD_STATUS_STALL_PID:
		return XSTATUS_Stall;
	case USBD_STATUS_PID_CHECK_FAILURE:
	case USBD_STATUS_UNEXPECTED_PID:
		return XSTATUS_PIDError;
	case USBD_STATUS_DATA_OVERRUN:
		return XSTATUS_ShortPacketError;
	case USBD_STATUS_DATA_UNDERRUN:
		return XSTATUS_ShortPacketError;

	case USBD_STATUS_HALTED:
		DebugPrint("Usbd error: halted 0x%x\n", u);
		goto narf;
	case USBD_STATUS_DEV_NOT_RESPONDING:
		DebugPrint("Usbd error: dev_not_responding 0x%x\n", u);
		goto narf;
	case USBD_STATUS_ERROR:
		DebugPrint("Usbd error: \"ERROR\" 0x%x\n", u);
		goto narf;
	case USBD_STATUS_RESERVED1:
		DebugPrint("Usbd error: reserved1 0x%x\n", u);
		goto narf;
	case USBD_STATUS_RESERVED2:
		DebugPrint("Usbd error: reserved2 0x%x\n", u);
		goto narf;
	case USBD_STATUS_BUFFER_OVERRUN:
		DebugPrint("Usbd error: buffer_overrun 0x%x\n", u);
		goto narf;
	case USBD_STATUS_BUFFER_UNDERRUN:
		DebugPrint("Usbd error: buffer_underrun 0x%x\n", u);
		goto narf;
	case USBD_STATUS_NOT_ACCESSED:
		DebugPrint("Usbd error: not_accessed 0x%x\n", u);
		goto narf;
	case USBD_STATUS_FIFO:
		DebugPrint("Usbd error: fifo 0x%x\n", u);
		goto narf;
	default:
		DebugPrint("Unknown usbd error! 0x%x\n", u);
narf:
		return XSTATUS_ConfigurationError;
	}
}


/*************************************************************************************
Function:   deinit
Purpose:	deinitialization routine?
Note:		there is no function. This program should never quit.
*************************************************************************************/
