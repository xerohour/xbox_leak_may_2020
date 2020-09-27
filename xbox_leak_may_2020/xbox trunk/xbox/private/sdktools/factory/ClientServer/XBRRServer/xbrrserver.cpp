//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include "XBRRServer.h"


void 
__cdecl main()
{
	CNetWork cn;
	SOCKET   sAccept;
//_asm Int 3      
	while (1) 
	{
		// Accept will block then thread will start.
		if ( INVALID_SOCKET == (sAccept= cn.Accept()))
		{
			WSACleanup();
			DebugPrint("INVALID_SOCKET on calling accept....\n");
			return; // fatal error, server dies
		}
		DebugPrint("Starting Server Session\n");
		StartThread( ServerSession,(LPVOID)sAccept);
   }
}




DWORD WINAPI 
ServerSession(LPVOID lpParam)
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    4/25/2001

    Routine Description:
        This is the thread function that will Do the receving work.
        it handles a given client connection.  The parameter passed 
        in is the socket handle returned from an accept() call.  
        This function reads data from the client and writes it back.

    Arguments:
        A Variable void pointer.
    Return Value:

--*/

{
   
   HRESULT hr;
   SOCKET  RecvSocket = (SOCKET)lpParam;
   CNetWorkConnection cnc( (SOCKET)lpParam );
   DATA_PACKET InBuffer;
   // Read Command Buffer.
   ZeroMemory(&InBuffer, sizeof(InBuffer));
   if ( SOCKET_ERROR == cnc.RecvData(&InBuffer, sizeof(InBuffer)))
   {
//		DebugPrint("SOCKET_ERROR on Receive Data....\n");
		ExitThread(0);
   }
//	DebugPrint("Processing command %d\n",InBuffer.dwCommand);
	
   switch(InBuffer.dwCommand)
   {
		case DO_WRITEPERBOXDATA:
			DoWritePerBoxData( &cnc,&InBuffer );  
			break;

		case DO_VERIFYPERBOXDATA:
			DoVerifyPerBoxData( &cnc, &InBuffer );
			break;

		case DO_LOCK_HARDDRIVE:
			LockHardDrive( &cnc, &InBuffer  );
			break;

		default:
			ReturnInvalidCommandError(&cnc, &InBuffer);
			break;

   }
	
	ExitThread(0);
}

HRESULT ReturnInvalidCommandError(IN CNetWorkConnection *c, PDATA_PACKET pPacket)
{
	HRESULT hr = S_OK;

	pPacket->dwStatus = ERROR_BAD_COMMAND;
   if ( SOCKET_ERROR == c->SendData(pPacket, sizeof(DATA_PACKET)))
   {
      hr = E_FAIL;
   }
   return hr;
}


HRESULT
DoWritePerBoxData(IN CNetWorkConnection *c, IN DATA_PACKET * pDataPacket)
{
	HRESULT hr = S_OK;

	pDataPacket->dwStatus = 0; // set initial status to success

	// Do the WritePerBoxData

	hr = WritePerBoxData( pDataPacket->byteData , pDataPacket->ulDataSize );

	if ( FAILED( hr ) )
	{

	  hr = E_FAIL;
	  pDataPacket->dwStatus = E_FAIL;
	}


// Send the STATUS back

   if ( SOCKET_ERROR == c->SendData(pDataPacket, sizeof(DATA_PACKET)))
   {
      hr = E_FAIL;
   }

   return hr;
}

// locks hard drive and clears mfg region
HRESULT
DoVerifyPerBoxData(IN CNetWorkConnection *c, PDATA_PACKET pPacket)
{
   HRESULT hResult = VerifyPerBoxData();

   pPacket->dwStatus = hResult;

   if ( SOCKET_ERROR == c->SendData(pPacket, sizeof(DATA_PACKET)))
   {
		DebugPrint("Sendback of Verify failed\n");
	   hResult = E_FAIL;
   }

   return hResult;
}

// locks hard drive but does not clear mfg region

HRESULT
LockHardDrive(IN CNetWorkConnection *c, PDATA_PACKET pPacket)
{
   HRESULT hResult = VerifyPerBoxDataEx(FALSE); // don't reset mfg region

   pPacket->dwStatus = hResult;

   if ( SOCKET_ERROR == c->SendData(pPacket, sizeof(DATA_PACKET)))
   {
		DebugPrint("Sendback of Verify failed\n");
	   hResult = E_FAIL;
   } else
   {
	   DebugPrint("Sendback of Verify Succeeded\n");
   }

   return hResult;
}



BOOL
StartThread( LPTHREAD_START_ROUTINE lpStartAddress, 
             LPVOID Param )
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    5/7/2001

    Routine Description:
        This routine creates a thread.
    Arguments:
        The Thread Function address.
        The Paremeter to the function.
    Return Value:

--*/

{
   HANDLE   hThread;
   hThread = CreateThread(NULL,
                          0,
                          lpStartAddress,
                          (LPVOID)Param,
                          0,
                          NULL);
   if (hThread == NULL) {
      return FALSE;
   }
   CloseHandle(hThread);
   return TRUE;
}

