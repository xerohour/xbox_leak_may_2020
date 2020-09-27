/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    slix.cpp

Abstract:

    main file for Slix2k

Author:

    Jason Gould (a-jasgou) June 2000

--*/

#include <stdio.h>
#include "slix.h"
#include "slixsock.h"
#include "myassert.h"

#undef IN
#undef OUT
#undef SETUP

//Get the name of the server, store it in gMachineName
BOOL CALLBACK GetMachineNameProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
char gMachineName[128] = {0};	//hold the name of the server
#define ADDRESS_FROM_PORT_SLOT(port, slot) ((slot << 4) + (port))

void DoBox(char * text, ...);

BOOL isqueue = 0;


/**********Global varaibles*************/
BOOL gInited = FALSE;			//are we properly initialized?
SOCKET gSock = INVALID_SOCKET;	//our winsock socket
//extern SOCKET ggSock;

extern "C" int kbhit();

#pragma warning(disable : 4035)
INT64 __inline GetTime() { 	_asm rdtsc }
#pragma warning(default : 4035)
#define measure(Description, code) {INT64 _ttt1, _ttt2; _ttt1 = GetTime(); code; _ttt2 = GetTime(); printf(Description ": %I64d\n", (_ttt2 - _ttt1) / 700);}

/*
void main()
{
	printf("sizeof(TRANSFERDATA) (should be 256): %d\n", sizeof(TRANSFERDATA));
	CBSEIDEClient arf;
	CUSBProvider u;
	BYTE data[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int size;
	BYTE temp[4];
	BYTE datapid = PID_DATA0;

	u.Initialize(&arf);

//	size = 8;
//	temp = u.SETUP(ADDRESS_FROM_PORT_SLOT(1,0), 0, &data[0], size);
//	printf("SETUP returned %d... \n", temp);

	while(1)
	{
		do {
			size = 8;
			temp[0] = u.IN(ADDRESS_FROM_PORT_SLOT(1,0), 0x01, &data[0], &size);
//			if(temp[0] != STATUS_Nak && temp[0] != STATUS_Success) {
//				printf("in returned 0x%x\n", temp[0]);
//			}
			if(temp[0] != STATUS_Success) {
				switch (temp[0]) {
				case STATUS_Nak				   :
					printf("NAK\n"); break;
				case STATUS_Stall              :
					printf("Stall\n"); break;
				case STATUS_Ignore             :
					printf("Ignore\n"); break;
				case STATUS_DataCRCError       :
					printf("CRC Error\n"); break;
				case STATUS_DataToggleError    :
					printf("DataToggle Error!\n"); break;
				case STATUS_SyncError          :
					printf("Sync Error!\n"); break;
				case STATUS_BabbleError        :
					printf("Babble Error!\n"); break;
				case STATUS_PIDError           :
					printf("PID Error!\n"); break;
				case STATUS_ShortPacketError   :
					printf("ShortPacket!\n"); break;
				case STATUS_ConfigurationError :
					printf("ConfigurationError!\n"); break;
				case STATUS_TransmissionError  :
					printf("TransmissionError!\n"); break;
				}
			}
		} while (temp[0] != STATUS_Success);

		size = 8;
		temp[1] = u.IN(ADDRESS_FROM_PORT_SLOT(1,0), 0x01, &data[8], &size);
		size = 8;
		temp[2] = u.IN(ADDRESS_FROM_PORT_SLOT(1,0), 0x01, &data[16], &size);
		size = 8;
		temp[3] = u.IN(ADDRESS_FROM_PORT_SLOT(1,0), 0x01, &data[24], &size);
		printf("IN:%2x%2x%2x%2x %016I64x %016I64x %016I64x %016I64x\n", 
			temp[0], temp[1], temp[2], temp[3], 
			*(INT64*)&data[0], *(INT64*)&data[8], *(INT64*)&data[16], *(INT64*)&data[24]);

		size = 4;
		data[100] = 0;	//set up the OUT packet to set motor speed...
		data[101] = 4;
		data[102] = data[8]; 
		data[103] = data[9];
		u.OUT(ADDRESS_FROM_PORT_SLOT(1,0), 0x02, datapid, &data[100], size);
		datapid ^= PID_DATA0 ^ PID_DATA1;
		
//	    SEND("OUT ADDR(" + Address$ + ") ENDP(02) DATA0 DATA(00 04 " + LeftSpd$ + " " + RightSpd$ + ")")
	}
	u.~CUSBProvider();
	while(!kbhit());
}
*/

BYTE CUSBProvider::GetStatus(BYTE nPortStatus)
{
	return ROOT1_NO_ERROR;
}
BYTE CUSBProvider::PortPower(BOOL bOn)
{
	return ROOT1_NO_ERROR;
}
BYTE CUSBProvider::Suspend()
{
	return ROOT1_NO_ERROR;
}
BYTE CUSBProvider::Resume()
{
	return ROOT1_NO_ERROR;
}

BYTE CUSBProvider::Reset()
{
#undef MyAssertFunctionTitle
#define MyAssertFunctionTitle "CUSBProvider::Reset"
#undef MyAssertPostCode
#define MyAssertPostCode return FALSE;

	BYTE szEndpoint = 1;

	TRANSFERDATA sockdata;
	int temp;
	
	MyAssert(gSock != INVALID_SOCKET, 2, "Invalid Socket");

	sockdata.size = sizeof(TRANSFER_RESET);	//set up the packet to send to slixbox
	sockdata.type = PID_RESET;

	temp = ssSend(gSock, &sockdata, sockdata.size);
/*	if(temp <= 0) {
		printf("send failed, %d...", WSAGetLastError());
		return FALSE;
	} */

	MyAssert(temp > 0, 2, "SOCKET_ERROR during send()");

	if(isqueue) return 0;

	temp = recv(gSock, (char*)&sockdata, 256, 0);	//temp = num of bytes received
	if(temp <= 0) {
		printf("recv failed, %d...", WSAGetLastError());
		return STATUS_TransmissionError;
	}

	return sockdata.ret.ret;	//return the handshake packet from the device
}


typedef struct {
    int  status;
    char *name;
} STATUS_NAME1;

STATUS_NAME1 gStatusNames[] = 
{
    {ROOT1_STATUS_Success,              "Success"},
    {ROOT1_STATUS_Ack,                  "Ack"},
    {ROOT1_STATUS_Nak,                  "Nak"},
    {ROOT1_STATUS_Stall,                "Stall"},
    {ROOT1_STATUS_Ignore,               "Ignore"},
    {ROOT1_STATUS_DataCRCError,         "DataCRCError"},
    {ROOT1_STATUS_DataToggleError,      "DataToggleError"},
    {ROOT1_STATUS_SyncError,            "SyncError"},
    {ROOT1_STATUS_BabbleError,          "BabbleError"},
    {ROOT1_STATUS_PIDError,             "PIDError"},
    {ROOT1_STATUS_ShortPacketError,     "ShortPacketError"},
    {ROOT1_STATUS_ConfigurationError,   "ConfigurationError"},
    {STATUS_TransmissionError,			"TransmissionError"},
	{XSTATUS_UserError,					"UserError"},
    {0, NULL}
};
void CUSBProvider::InterpretStatus (BYTE nStatus, char *pszStatus, int size)
{
	int i=0;
    pszStatus[0] = 0;

    while (gStatusNames[i].name)
    {
        if (gStatusNames[i].status == nStatus)
        {
            strcpy(pszStatus, gStatusNames[i].name);
            break;
        }
        ++i;
    }
}


CUSBProvider::CUSBProvider() 
{
	Initialize(NULL);
}


/*************************************************************************************
Function:   CUSBProvider::Initialize
Purpose:	set up the winsock connection so we can talk with the Xbox running slixbox
Params:     pParent --- a pointer to CBSEIDEClient, that is currently unused
Return:     FALSE if it failed, non-FALSE if it worked
*************************************************************************************/
BOOL CUSBProvider::Initialize(void *pParent)
{
#undef MyAssertFunctionTitle
#define MyAssertFunctionTitle "CUSBProvider::Initialize"
#undef MyAssertPostCode
#define MyAssertPostCode return FALSE;

	if(gSock != INVALID_SOCKET) {
		ssCloseTCP(gSock, NULL, NULL);
	}
	
//	temp = DialogBox(NULL, MAKEINTRESOURCE(SLIX_GET_MACHINE_NAME), NULL, GetMachineNameProc);
//	if(temp == FALSE)
//		return FALSE;

	FILE* f = fopen("net.txt", "r");
	if(!f) {
		sprintf(gMachineName, "a-jasgoux2");
	} else {
		fscanf(f, "%s", gMachineName);
		fclose(f);
	}

	gSock = ssConnectTCP(gMachineName, "3331");
	MyAssert(gSock != INVALID_SOCKET, 3, "Couldn't connect to server");

	gInited = TRUE;
	return TRUE;
}


/*************************************************************************************
Function:   CUSBProvider::~CUSBProvider
Purpose:	deinitialize the winsock connection so we aren't left with an open port
Params:     (none)
Return:     (none)
*************************************************************************************/
CUSBProvider::~CUSBProvider(void)
{
	ssCloseTCP(gSock, NULL, NULL);
	gInited = FALSE;
}


BOOL CUSBProvider::BeginQueue(BOOL bSuccessOnNak)
{
#undef MyAssertFunctionTitle
#define MyAssertFunctionTitle "CUSBProvider::BeginQueue"
#undef MyAssertPostCode
#define MyAssertPostCode return FALSE;

	TRANSFERDATA sockdata;
	int temp;
	
	MyAssert(gSock != INVALID_SOCKET, 2, "Invalid Socket");

	CREATE_BEGIN_QUEUE(sockdata, bSuccessOnNak);

	temp = ssSend(gSock, &sockdata, sockdata.size);
	if(temp <= 0) {
		printf("send failed, %d...", WSAGetLastError());
		return FALSE;
	}
	MyAssert(temp > 0, 2, "SOCKET_ERROR during send()");
	isqueue = TRUE;
	return TRUE;
}

BYTE CUSBProvider::EndQueue(DWORD* CommandThatFailed)
{
#undef MyAssertFunctionTitle
#define MyAssertFunctionTitle "CUSBProvider::EndQueue"
#undef MyAssertPostCode
#define MyAssertPostCode return XSTATUS_TransmissionError;

	TRANSFERDATA sockdata;
	int temp;
	
	isqueue = FALSE;

	MyAssert(gSock != INVALID_SOCKET, 2, "Invalid Socket");
	MyAssert(CommandThatFailed, 3, "CommandThatFailed is NOT an optional paramater!");

	CREATE_INIT_END_QUEUE(sockdata);

	temp = ssSend(gSock, &sockdata, sockdata.size);
	if(temp <= 0) {
		printf("send failed, %d...", WSAGetLastError());
		return FALSE;
	}

//	MyAssert(temp > 0, 2, "SOCKET_ERROR during send()");


	temp = recv(gSock, (char*)&sockdata, sizeof(TRANSFER_RETURN_END_QUEUE), 0);	//receive some data back
	
	if(temp <= 0) {
		printf("recv failed, %d...", WSAGetLastError());
		return STATUS_TransmissionError;
	}

	*CommandThatFailed = sockdata.retendqueue.linefailed;

	return sockdata.ret.ret;											//return status of the IN
}

	// IN should return STATUS_Success, or the appropriate error
/*************************************************************************************
Function:   CUSBProvider::IN
Purpose:	request an IN From slixbox, receive data, put it in pszData
Params:     [IN]  szAddress --- the USB address for slixbox to send the request
			[IN]  szEndpoint --- the USB endpoint for slixbox to send the request
			[OUT] pszData --- pointer to the buffer to receive data from the device
			[OUT] pDataSize --- pointer to receive the length of data received
Return:     STATUS_Success if it worked, or other STATUS_* for errors
*************************************************************************************/
BYTE CUSBProvider::In(BYTE szAddress, BYTE szEndpoint, BYTE szDataPID, int *iReturnDataPID, BYTE *pszData, int *pDataSize, BOOL *bToggleCorrect)
{
#undef MyAssertFunctionTitle
#define MyAssertFunctionTitle "CUSBProvider::IN"
#undef MyAssertPostCode
#define MyAssertPostCode return STATUS_TransmissionError;


	*bToggleCorrect = TRUE;

	TRANSFERDATA sockdata;		//network data buffer
	TRANSFERDATA sockdata2;
	int temp;
	int received = 0;			//data received so far
	int totaltoreceive = sizeof(TRANSFERDATA);	//max data to receive

	char buf[256] = {'\0'};

	MyAssert(gSock != INVALID_SOCKET, 3, "invalid socket");
	MyAssert(pszData != NULL, 3, "bad pointer to data buffer");
	MyAssert(pDataSize != NULL, 3, "bad pointer to data size buffer");
	MyAssert(*pDataSize != 0, 2, "expected data size == 0!");

//	for (i = 0; i < 10000; i++) {
	
		CREATE_ININIT(sockdata, szAddress, szEndpoint, *pDataSize);		//set up the request for an IN

	//measure("ssSend",
		temp = ssSend(gSock, &sockdata, sockdata.size); 
	//);

		MyAssert(temp > 0, 3, "send failed");
//		if(temp <= 0) {
//			printf("send failed, %d", WSAGetLastError());
//			return STATUS_TransmissionError;
//		}

//tryagain:
		if(isqueue) return 0;

		received = recv(gSock, (char*)&sockdata2, totaltoreceive - received, 0);	//receive some data back

		if(received <= 0) {
//			if(WSAGetLastError() == 0) continue;
			sprintf(buf, "recv failed, %d", WSAGetLastError());
			MessageBox(NULL, buf, buf, MB_OK);
		}
//		MyAssert(received >= 0, 3, "recv failed");
//		if(received < 0) {
//			printf("recv failed, %d...", WSAGetLastError());
//			return STATUS_TransmissionError;
//		}

		for(temp = 0; temp < received - (int)(sizeof(TRANSFER_IN)); temp++) {	//copy the received data into 
			pszData[temp] = sockdata2.in.data[temp];						//the supplied buffer
		}

		*pDataSize = received - sizeof(TRANSFER_IN);					//set the received data length

	//	sprintf(buf, "Incoming data length: %d, %016I64x", *pDataSize, *(INT64*)&pszData[0]);
	//	MessageBox(NULL, buf, "Incoming data!", MB_OK);

		if(sockdata2.in.ret == STATUS_DataToggleError) *bToggleCorrect = FALSE;
//	}

	return sockdata2.in.ret;											//return status of the IN
}

	// OUT and SETUP should return what happened (Ack, Nak, Stall, etc.)
/*************************************************************************************
Function:   CUSBProvider::OUT
Purpose:	send data to slixbox to send to device; receive handshake from device
Params:     [IN]  szAddress --- the USB address for slixbox to send the data
			[IN]  szEndpoint --- the USB endpoint for slixbox to send the data
			[IN]  szDataPid --- either PID_DATA0 or PID_DATA1
			[IN]  pszData --- pointer to the buffer that holds the data to be sent
			[IN]  iDataSize --- number of bytes of data in pszData
Return:     STATUS_Ack, STATUS_Nak, STATUS_Stall, or any other STATUS_*
*************************************************************************************/
BYTE CUSBProvider::Out(BYTE szAddress, BYTE szEndpoint, BYTE pid, BYTE *pszData, int iDataSize)
{  
#undef MyAssertFunctionTitle
#define MyAssertFunctionTitle "CUSBProvider::OUT"
#undef MyAssertPostCode
#define MyAssertPostCode return STATUS_TransmissionError;

	BYTE szDataPid = 0;

	int sdcount = 0, i = 0;
	TRANSFERDATA sockdata;
	int temp;

	MyAssert(gSock != INVALID_SOCKET, 3, "Invalid Socket");
	MyAssert(iDataSize < 1024, 2, "Too much data to send");
	MyAssert(pszData != NULL, 3, "bad pointer to data buffer");

	//set up the data packet to send to slixbox
	CREATE_OUT(sockdata, szAddress, szEndpoint, szDataPid, pszData, iDataSize);

	//send the data to slixbox
	temp = ssSend(gSock, &sockdata, sockdata.size);
	if(temp <= 0) {
		printf("send failed, %d...", WSAGetLastError());
		return STATUS_TransmissionError;
	}

	if(isqueue) return 0;

	//try to receive a handshake packet back...
	temp = recv(gSock, (char*)&sockdata, 256, 0);	//temp = num of bytes received
	if(temp <= 0) {
		printf("recv failed, %d...", WSAGetLastError());
		return STATUS_TransmissionError;
	}

	return sockdata.ret.ret;	//return the handshake packet from the device
}

/*************************************************************************************
Function:   CUSBProvider::SETUP
Purpose:	send SETUP data to slixbox to send to device; receive handshake from device
Params:     [IN]  szAddress --- the USB address for slixbox to send the data
			[IN]  szEndpoint --- the USB endpoint for slixbox to send the data
			[IN]  pszData --- pointer to the buffer that holds the data to be sent
			[IN]  iDataSize --- number of bytes of data in pszData
Return:     STATUS_Ack, STATUS_Nak, STATUS_Stall, or any other STATUS_*
Note:		for SETUP, DataPid is always 0 (Data0, instead of alternating to Data1)
*************************************************************************************/
BYTE CUSBProvider::Setup (BYTE szAddress, BYTE szEndpoint, BYTE *pszData, int iDataSize)
{
#undef MyAssertFunctionTitle
#define MyAssertFunctionTitle "CUSBProvider::SETUP"
#undef MyAssertPostCode
#define MyAssertPostCode return STATUS_TransmissionError;

	int sdcount = 0, i = 0;
	TRANSFERDATA sockdata;
	int temp;

	MyAssert(gSock != INVALID_SOCKET, 3, "Invalid Socket");
	MyAssert(iDataSize < 250, 2, "Too much data to send");
	MyAssert(pszData != NULL, 3, "bad pointer to data buffer");

	//set up the data packet to send to slixbox
	CREATE_SETUP(sockdata, szAddress, szEndpoint, pszData, iDataSize);

	//send the data to slixbox
	temp = ssSend(gSock, &sockdata, sockdata.size);
	if(temp <= 0) {
		printf("send failed, %d...", WSAGetLastError());
		return STATUS_TransmissionError;
	}
	//MyAssert(temp > 0, 2, "Socket error during send");

	if(isqueue) return 0;

	//try to receive a handshake packet back...
	temp = recv(gSock, (char*)&sockdata, 256, 0);	//temp = num of bytes received
	if(temp <= 0) {
		printf("recv failed, %d...", WSAGetLastError());
		return STATUS_TransmissionError;
	}

	MyAssert(temp > 0, 2, "0-byte receive");
	MyAssert(temp != SOCKET_ERROR, 2, "Socket error during receive");
	MyAssert(sockdata.size == sizeof(TRANSFER_RETURN), 2, "Not enough data received");
	MyAssert(sockdata.type == PID_RET, 2, "Return data type wasn't PID_RET!");

	return sockdata.ret.ret;	//return the handshake packet from the device
}

	
/*************************************************************************************
Function:   CUSBProvider::ClearDataToggle
Purpose:	send switch the expected data toggle to Data0
Params:     [IN]  szEndpoint --- the USB endpoint to expect Data0 from
Return:     none
*************************************************************************************/

BOOL CUSBProvider::ClearDataToggle(int szAddress, int szEndpoint)
{
#undef MyAssertFunctionTitle
#define MyAssertFunctionTitle "CUSBProvider::ClearDataToggle"
#undef MyAssertPostCode
#define MyAssertPostCode return FALSE;

	TRANSFERDATA sockdata;
	int temp;
	
	MyAssert(gSock != INVALID_SOCKET, 2, "Invalid Socket");

	sockdata.size = sizeof(TRANSFER_DATA_TOGGLE);	//set up the packet to send to slixbox
	sockdata.type = PID_DATA0;
	sockdata.toggle.endpoint = szEndpoint;

	temp = ssSend(gSock, &sockdata, sockdata.size);
	if(temp <= 0) {
		printf("send failed, %d...", WSAGetLastError());
		return FALSE;
	}
//	MyAssert(temp > 0, 2, "SOCKET_ERROR during send()");
	return TRUE;
}

/*************************************************************************************
Function:   CUSBProvider::SetDataToggle
Purpose:	send switch the expected data toggle to Data1
Params:     [IN]  szEndpoint --- the USB endpoint to expect Data1 from
Return:     none
*************************************************************************************/

BOOL CUSBProvider::SetDataToggle(int szEndpoint, int szAddress)
{
#undef MyAssertFunctionTitle
#define MyAssertFunctionTitle "CUSBProvider::SetDataToggle"
#undef MyAssertPostCode
#define MyAssertPostCode return FALSE;

	TRANSFERDATA sockdata;
	int temp;
	
	MyAssert(gSock != INVALID_SOCKET, 2, "Invalid Socket");

	sockdata.size = sizeof(TRANSFER_DATA_TOGGLE);	//set up the packet to send to slixbox
	sockdata.type = PID_DATA1;
	sockdata.toggle.endpoint = szEndpoint;

	temp = ssSend(gSock, &sockdata, sockdata.size);
	if(temp <= 0) {
		printf("send failed, %d...", WSAGetLastError());
		return FALSE;
	}
//	MyAssert(temp > 0, 2, "SOCKET_ERROR during send()");
	return TRUE;
}


/*************************************************************************************
Function:   GetMachineNameProc
Purpose:	dialog proc to get user input for the name of the server computer
Params:     standard dialog proc params...
Return:     FALSE if DialogBox() needs to do stuff after calling this, TRUE otherwise
*************************************************************************************/
BOOL CALLBACK GetMachineNameProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	sprintf(gMachineName, "a-jasgoutst");
	EndDialog(hDlg, TRUE);
	return TRUE;
/*/
	switch (message)
	{
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hDlg, SLIX_EDIT));			//the edit box, so the user can type
		return FALSE;
	case WM_COMMAND:
		if(LOWORD(wParam) == SLIX_OK) {					//if they pressed "OK"
			GetDlgItemText(hDlg, SLIX_EDIT, gMachineName, 127);	//get the machine name
			EndDialog(hDlg, TRUE);						//and quit
			return TRUE;
		} else if(LOWORD(wParam) == SLIX_CANCEL) {		//if they pressed "Cancel"
			EndDialog(hDlg, FALSE);						//don't get the machine name ... quit ...
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
*/
}
 
