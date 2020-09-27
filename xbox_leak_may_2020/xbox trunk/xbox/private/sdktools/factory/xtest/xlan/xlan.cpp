// (c) Copyright 2001 Intel Corp. All rights reserved.
//
// Title: xlan  
//
// History:
//
// 8/20/2001 V1.00 SSS Release V1.00.
//
// INTEL CONFIDENTIAL.  Copyright (c) 1997-2001 Intel Corp. All rights reserved.
// Disable the compiler warning for a non threatening condition.
// C4512: unable to generate an assignment operator for the given class. 
// version number

#pragma comment(exestr,"version_number=1.00.0.010")
#include "..\stdafx.h"
#include "..\testobj.h"
#include <winsockx.h>
#include "..\host.h"
#include "xlan.h"

IMPLEMENT_MODULEUNLOCKED (CXModuleLAN);

bool CXModuleLAN::InitializeParameters ()
{
	// Call the base class.  This is REQUIRED

	if (!CTestObj::InitializeParameters ())
		return false;

	return true;
}


IMPLEMENT_TESTUNLOCKED (LAN, UDPResponder, 1)
{
	TCHAR StrOutput[512];
	wchar_t TestName[] = L"UDPResponder";
	int Status;
	int enable = 1;
	int iError;
	DWORD x;
	DWORD ConnectTries;
	
	try 
	{
		// clear the error code
		Code = 0;
		_stprintf(StrOutput,_T("in code\n"));
		OutputDebugString(StrOutput);
		TestNamePtr = TestName;

	// start the UDP responder app on the host

	if ((iError = g_host.iSendHost(MID_UDP_RESPONDER, NULL, 0)) != EVERYTHINGS_OK)
	{
		// Process a communication error
		// This test module just reports the number of the communication error that was received
		ReportError(1,L"iSendHostError, Code = %x",iError);
		return;
	}


  // initialize the Windows Socket DLL 
  Status=WSAStartup(MAKEWORD(1, 1), &Data);
  if (Status != 0)
  {
    ReportDebug(2,L"ERROR: WSAStartup unsuccessful");
	exit(1);

  }

  // specify the IP address 
  destSockAddr.sin_addr.s_addr=
    htonl(INADDR_BROADCAST);
  // specify the port portion of the address 
  destSockAddr.sin_port=htons(PORT);
  // specify the address family as Internet 
  destSockAddr.sin_family=AF_INET;

  // create a socket 
  destSocket=socket(AF_INET, SOCK_DGRAM, 0);
  if (destSocket == INVALID_SOCKET)
  {
    ReportError(1,L"ERROR: socket unsuccessful");
    Status=WSACleanup();
    if (Status == SOCKET_ERROR)
	{
      ReportError(1,L"ERROR: WSACleanup unsuccessful");
	}
    return;
  }

  // permit broadcasting on the socket 
  Status=setsockopt(destSocket, SOL_SOCKET,
	  SO_BROADCAST, (char *) &enable,
      sizeof(enable));
  if (Status != 0)
	  ReportError(1,L"ERROR: setsockopt unsuccessful");
  
  //Poll for a responce from the responder
  
  	if((ConnectTries = GetCfgUint(L"ConenctTries",1)) == 0)
	{
		ReportError(0x10,L"ConnectTries config param missing");
		return;
	}

  for(x=0;x<ConnectTries;x++)
  {
	  SendBuffer[0] = 0x69;
	  SendPattern();
	  // look for responce, no errors 1 second delay
	  RecvPattern(0,1);
	  if( RecvBuffer[0] == 0x69)
		  break;
	  // wait a second before trying again
	  Sleep(1000); 
  }
  
	// send and recv the test pattern
	NoisePattern();

	Status=closesocket(destSocket);
	if (Status == SOCKET_ERROR)
	{
		ReportError(1,L"ERROR: closesocket unsuccessful");
	}
	Status=WSACleanup();
	if (Status == SOCKET_ERROR)
	{
		ReportError(1,L"ERROR: WSACleanup unsuccessful");
	}

	}
	catch (CXmtaException &e)
	{
		// error already reported
		if (Code == 99) 
			return;
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}

void CXModuleLAN::SendPattern()
{
	int numsnt;
	int Status;
	ReportDebug(2,L"Sending...");
    numsnt=sendto(destSocket, SendBuffer,
      DATASIZE, NO_FLAGS_SET,
      (LPSOCKADDR) &destSockAddr,
        sizeof(destSockAddr));
    if (numsnt != DATASIZE)
    {
      ReportError(1,L"ERROR: sendto unsuccessful");
      Status=closesocket(destSocket);
      if (Status == SOCKET_ERROR)
        ReportError(1,L"ERROR: closesocket unsuccessful");
      Status=WSACleanup();
      if (Status == SOCKET_ERROR)
        ReportError(1,L"ERROR: WSACleanup unsuccessful");
    }
}

BOOL CXModuleLAN::RecvPattern( BOOL ErrorsOn, char LookTime)
{
	TIMEVAL WaitTime = {1,0}; // wait 270 seconds before error
	FD_SET FDReadSocket;
	FD_SET FDWriteSocket;
	int Status;
	int numrcv;

	FD_ZERO(&FDWriteSocket);
	FD_ZERO(&FDReadSocket);
	FD_SET(destSocket,&FDReadSocket);
	ReportDebug(2,L"Receiving...");

		Status = select(0,&FDReadSocket,&FDWriteSocket,&FDWriteSocket,&WaitTime);
		if(Status == 0)
		{
			if(ErrorsOn)
				ReportError(0x11,L"rec time expired\n");
			return 0;
		}


		// get the data from the socket
		numrcv=recvfrom(destSocket, RecvBuffer, DATASIZE,
			NO_FLAGS_SET, NULL, NULL);
		if (numrcv == SOCKET_ERROR)
		{
			ReportError(0x12,L"ERROR: UDP recvfrom host unsuccessful");
			Status=closesocket(destSocket);
			if (Status == SOCKET_ERROR)
			{
				ReportError(0x13,L"ERROR: closesocket unsuccessful");
			}
			Status=WSACleanup();
			if (Status == SOCKET_ERROR)
			{
				ReportError(0x14,L"ERROR: WSACleanup unsuccessful");
			}
			return 0;
		}
		return 1;
}

void CXModuleLAN::NoisePattern()
{
	DWORD DataHigh,DataLow;
	int bPatterns = 0;
	DWORD NoiseData =  0x00010001;
	int x;
	DWORD dwExpectedLoss;
	DWORD PacketLoss = 0;
	DWORD Loops,y;
	BYTE Checksum;
	
	if((Loops = GetCfgUint(L"Loops",1)) == 0)
	{
		ReportError(0x10,L"LAN Loops config param missing");
		return;
	}
	for(y=0;y<Loops;y++)
	{
		ReportDebug(4,L"Loop %x",y);
		for(bPatterns = 0; bPatterns<32; bPatterns++)
		{
			if (bPatterns < 16)
			{
				DataLow =  0xFFFFFFFF & ~(NoiseData << bPatterns);
				DataHigh = ~DataLow;
			}
			// 0111, 1011, 1101, 1110
			else
			{
				DataHigh =  0xFFFFFFFF & ~(NoiseData << bPatterns);
				DataLow = ~DataHigh;	
			}
			for(x=0;x<DATASIZE;x=x+8)
			{
				*((DWORD *)(SendBuffer+x)) = DataLow;
				*((DWORD *)(SendBuffer+x+4)) = DataHigh;
			}
			ReportDebug(2,L"Pattern x =%x Data = %8.8X %8.8X\n",bPatterns,*((DWORD *)(SendBuffer+x)),*((DWORD *)(SendBuffer+x+4)) );
			SendBuffer[2] = (BYTE)bPatterns; 
			SendBuffer[0] = 0;
			for(x=0;x<DATASIZE;x++)
			{
				SendBuffer[0] = SendBuffer[0] + SendBuffer[x];
			}
			ReportDebug(2,L"Checksum on outgoing %x",SendBuffer[0]);
			SendPattern();
			// clear recv buffer
			if(RecvPattern(0,5) == 0) // check for recv eror, message already printed
			{
				ReportDebug(0,L"A packet %x was lost",bPatterns);
				PacketLoss++;
				continue;
			}
			Checksum = 0;
			for(x=1;x<DATASIZE;x++)
			{
				Checksum = RecvBuffer[x] + Checksum;
			}
			if((BYTE)Checksum != (BYTE)RecvBuffer[0])
			{
				ReportError(1,L"LAN data checksum error Exp = 0x%2.2x Rec = 0x%2.2x Pattern = %8.8X",(BYTE)RecvBuffer[0],Checksum,*((DWORD *)(RecvBuffer+8)));
				break;
			}
			
		}
	}
	if((dwExpectedLoss =GetCfgUint(L"PacketLoss",0)) == 0)
	{
		ReportError(0x10,L"Lan PackLoss config param missing");
		return;
	}
	//scale by the amount sent
	if( (PacketLoss/Loops) > dwExpectedLoss)
	{
		ReportError(1,L"To many packets lost!  exp < %x rec = %x",dwExpectedLoss, PacketLoss);
	}
	
	// send a 42 to the host tester to tell it to exit
	ReportDebug(2,L"Send done code to responder");
	SendBuffer[0] = 0x42;
	SendPattern();
	
}
