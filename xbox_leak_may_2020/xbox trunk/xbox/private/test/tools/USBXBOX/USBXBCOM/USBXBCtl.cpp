// USBXBCtl.cpp : Implementation of CUSBXBCtl

#include "stdafx.h"
#include <process.h>    /* _beginthread, _endthread */

#include "USBXBCOM.h"
#include "USBXBCtl.h"


/////////////////////////////////////////////////////////////////////////////
// CUSBXBCtl

CUSBXBCtl::CUSBXBCtl()
{
		m_fInited = FALSE;
		m_Socket = 0;
		m_iLastError = 0;
		m_ulDataSizeIn = 0;
		m_hInsertMutex = CreateMutex(NULL,FALSE,NULL);
		m_hEventThread = INVALID_HANDLE_VALUE;
}

CUSBXBCtl::~CUSBXBCtl()
{
	m_fStayAlive = FALSE;  // Kill event thread
}



STDMETHODIMP CUSBXBCtl::get_LastXError(int *pVal)
{

	*pVal = m_iLastError;
	return S_OK;
}

// arbitrary assignments

#define XPort				"53331"
#define XBoxEventsPort		"53335"

SOCKET CUSBXBCtl::ssConnectTCP()
{
	SOCKET hsock;
	WSADATA wsadata;
	int ret;
	SOCKADDR_IN RemoteName;

	ret = WSAStartup(2, &wsadata);	//initialize Winsock
	if(ret != 0) {
		m_iLastError = ret; // save error code for possible retrieval
		return 0;
	}

	hsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//create a socket
	if(hsock==INVALID_SOCKET) 
	{
		m_iLastError = WSAGetLastError();
		return 0;
	}

	RemoteName.sin_addr.S_un.S_addr = GetAddr();	//get address of server
	if(RemoteName.sin_addr.S_un.S_addr == INADDR_NONE) {
		m_iLastError = WSAGetLastError();
		return 0;
	}

	RemoteName.sin_port = htons(XPortNumber); //GetPort(XPort);	//get port for the address
	if(RemoteName.sin_port==0)
	{
		m_iLastError =  WSAGetLastError();
		return 0;
	}

	RemoteName.sin_family = AF_INET;

	BOOL fTrue = TRUE;
	setsockopt(hsock, IPPROTO_TCP, TCP_NODELAY, (char*)&fTrue, sizeof(BOOLEAN));

/* bugbug disable rcv timeout while debugging
	int OptVal = 500; // mSecs
	int iResult = setsockopt(hsock, SOL_SOCKET, SO_RCVTIMEO, (char*)&OptVal, sizeof(int));
	if(iResult)
	{
		int wsaerror = WSAGetLastError();
	}
*/
	ret = connect(hsock, (LPSOCKADDR)&RemoteName, sizeof(SOCKADDR));  //connect to server
	if(ret == SOCKET_ERROR) 
	{
		m_iLastError =  WSAGetLastError();
		return INVALID_SOCKET;
	}

	// now hard work is done, setup sideband events port to use for xbox to pc event notifications
	m_hXBoxEventsSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//create a socket
	if(m_hXBoxEventsSocket==INVALID_SOCKET) 
	{
		m_iLastError = WSAGetLastError();
		return 0;
	}

 
	RemoteName.sin_port = htons(XBoxEventsPortNumber);	//get port for the address

	RemoteName.sin_family = AF_INET;


	fTrue = TRUE;
	setsockopt(m_hXBoxEventsSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&fTrue, sizeof(BOOLEAN));

	ret = connect(m_hXBoxEventsSocket, (LPSOCKADDR)&RemoteName, sizeof(SOCKADDR));  //connect to server
	if(ret == SOCKET_ERROR) 
	{
		m_iLastError =  WSAGetLastError();
		return INVALID_SOCKET;
	}


	return hsock;	

}

ULONG CUSBXBCtl::GetAddr()
{
	unsigned long ret = INADDR_ANY;
	LPHOSTENT host;

	ret = inet_addr(m_cServerName);	//check first for "#.#.#.#" format...
	if((ret == INADDR_NONE) && strcmp(m_cServerName, "255.255.255.255"))	//if that didn't work,
	{
		host = gethostbyname(m_cServerName);	//try to get it by name...
		if(!host) {						//if that didn't work,
			return INADDR_ANY;				//return 0
		}
		ret = *(unsigned long *)(host->h_addr_list[0]);	//return the address from hostname
	}
	return ret;	//return the address from "#.#.#.#" format

}

USHORT CUSBXBCtl::GetPort(char *service)
{
	LPSERVENT servent;
	if(isdigit(service[0]))
		return (USHORT) atoi(service);

	servent = getservbyname(service, "tcp");
	if(!servent)
		return 0;
	return servent->s_port;

}

STDMETHODIMP CUSBXBCtl::Reset(UCHAR Port,UCHAR Slot, UCHAR InterfaceNumber, UCHAR *ucReturn)
{

	TRANSFERDATA sockdata;
	int iResult;
	*ucReturn = 0; // error value
	if(FALSE == m_fInited)
	{
		Initialize();
		if(FALSE == m_fInited)
		{
			return E_FAIL; // failed initialization, error in m_iLastError
		}
	}
	sockdata.size = sizeof(TRANSFER_RESET);	//set up the packet to send to slixbox
	sockdata.type = PID_RESET;
	sockdata.reset.Port= Port;
	sockdata.reset.Slot = Slot;
	sockdata.reset.interfacenumber = InterfaceNumber;

	iResult = ssSend(m_Socket, &sockdata, sockdata.size);
	if(iResult <= 0)
	{
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	} 


	iResult = recv(m_Socket, (char*)&sockdata, 256, 0);	//temp = num of bytes received
	if(iResult <= 0) {
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	}

	*ucReturn = sockdata.ret.ret;	//return the handshake packet from the device

	return S_OK;
}


#define IniName "USBXBCom.ini"


void EventHandler(PVOID  pContext)
{

	CEventPacket ThisEvent;
	CUSBXBCtl * pXBCtl = (CUSBXBCtl *) pContext;
	int			iReceived;
	int			iErrorCounter;

	while(pXBCtl->m_fStayAlive)
	{
		// sit on listen/rcv

		OutputDebugString("Entering Event Receive\n");
		iReceived = recv(pXBCtl->m_hXBoxEventsSocket, (char*)&ThisEvent, sizeof(CEventPacket), 0);	//receive some data back

		if(iReceived <= 0) 
		{
			pXBCtl->m_iLastError = WSAGetLastError();
			pXBCtl->m_hEventThread = INVALID_HANDLE_VALUE;
			_endthread(); // exit thread if failure, reinit of main stream will restart
		}
		OutputDebugString("Processing Event\n");

		iErrorCounter = 0;
		// process packet
		switch(ThisEvent.m_iEventCode)
		{
			case eInsertionEvent:
			{
				// fire insertion event
				pXBCtl->Fire_InsertionEvent(ThisEvent.m_Topology.m_bPort,
							ThisEvent.m_Topology.m_bSlot,
							ThisEvent.m_Topology.m_bInterfaceNumber);
				break;
			}
			case eRemovalEvent:
			{
				// fire removal event
				pXBCtl->Fire_RemovalEvent(ThisEvent.m_Topology.m_bPort,
							ThisEvent.m_Topology.m_bSlot,
							ThisEvent.m_Topology.m_bInterfaceNumber);
				break;
			}
		}
		

	}// while
	
	pXBCtl->m_hEventThread = INVALID_HANDLE_VALUE;
	_endthread();

}

void CUSBXBCtl::Initialize()
{
	m_fInited = FALSE; // default to fail
	// get server name
	char NameBuffer[100];
	DWORD dwResult;
	dwResult = GetPrivateProfileString(
					"USBXBConfig",		// No section
					"ServerName",		// key name caseinsensitive
					"Fail",				// Default Name
					NameBuffer,			// targer buffer
					sizeof(NameBuffer),
					IniName		// needs to be in Windows directory
					);

	if(0 == dwResult || !strcmp(NameBuffer,"Fail"))
	{
		// if no name in .ini file, try an ip address
		dwResult = GetPrivateProfileString(
					"USBXBConfig",		// No section
					"IPAddress",		// key name caseinsensitive
					"BadIni",			// Default Name
					NameBuffer,			// targer buffer
					sizeof(NameBuffer),
					IniName				// needs to be in Windows directory
					);

	}
	if(0 == dwResult)
	{
		return;
	}

	strcpy(m_cServerName,NameBuffer);
	m_Socket = ssConnectTCP();

	if(INVALID_SOCKET == m_Socket)
	{
		m_iLastError = WSAGetLastError();
		return;
	}
	m_fInited = TRUE;
	return;

}

int CUSBXBCtl::ssSend(SOCKET socket, TRANSFERDATA *pSockData, int iSize)
{
	iSize = pSockData->size;
	int sent = 0;
	int iResult;
	while(sent < iSize) {
		iResult = send(socket, &(pSockData->all[sent]), iSize, 0);
		if(iResult == SOCKET_ERROR) {
			m_iLastError = WSAGetLastError();
			return 0;
		}
		sent += iResult;
	}
	return sent;

}

STDMETHODIMP CUSBXBCtl::In(UCHAR Port,UCHAR Slot, UCHAR Interface , UCHAR Endpoint, UCHAR Format, BSTR *pbsRet)
{
	UCHAR cTempBuffer[sizeof(TRANSFERDATA)];
	char cSpacedBuffer[sizeof(TRANSFERDATA) + (sizeof(TRANSFERDATA)/3)];
	char cTempBuffer2[(sizeof(TRANSFERDATA)*2) + 50];

	TRANSFERDATA sockdata;		//network data buffer
	TRANSFERDATA sockdata2;
	int temp;
	int received = 0;			//data received so far
	int totaltoreceive = sizeof(TRANSFERDATA);	//max data to receive


	// Insure we have active connection to XBOX server
	if(FALSE == m_fInited)
	{
		Initialize();
		if(FALSE == m_fInited)
		{
			return E_FAIL; // failed initialization, error in m_iLastError
		}
	}

	if(m_Socket == INVALID_SOCKET || 0 == m_Socket)
	{
		m_iLastError = INVALID_SOCKET;
		return E_FAIL;
	}

	
	CREATE_ININIT(sockdata, Port,Slot ,Interface, Endpoint,totaltoreceive /*m_ulDataSizeIn*/);		//set up the request for an IN

	temp = ssSend(m_Socket, &sockdata, sockdata.size); 

	if(temp <= 0) 
	{
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	}

	received = recv(m_Socket, (char*)&sockdata2, totaltoreceive - received, 0);	//receive some data back

	if(received <= 0) 
	{
		*pbsRet = NULL;
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	}

	if(sockdata2.in.ret)
	{
		*pbsRet = NULL;  // if not success
		m_iLastError = sockdata2.in.ret;
		return E_FAIL;
	}

	for(temp = 0; temp < received - (int)(sizeof(TRANSFER_IN)); temp++) 
	{	//copy the received data into 
			cTempBuffer[temp] = sockdata2.in.data[temp];						//the supplied buffer
	}


	m_ulDataSizeIn = received - sizeof(TRANSFER_IN);					//set the received data length

	if(0 == m_ulDataSizeIn)
	{
		*pbsRet = NULL;
	} else
	{
		MakeStringOfBytes(cTempBuffer,m_ulDataSizeIn,cTempBuffer2,sizeof(cTempBuffer2));

		char * pSourceBuffer = cTempBuffer2;

		if(MAKE_SPACE & Format)
		{
			// reformat string with spaces every two characters
			int icharcount = strlen(cTempBuffer2);
			memset(cSpacedBuffer,0x00,sizeof(cSpacedBuffer));
			char * cTarget = cSpacedBuffer;
			for(int i = 0; i < icharcount; i++)
			{
				*(cTarget++) = cTempBuffer2[i];
				if( i % 2)
				{
					*(cTarget++) = 0x20;
				}
			}
			pSourceBuffer = cSpacedBuffer;
		}

		*pbsRet = CreateBstrFromAnsi(pSourceBuffer);
	}
	
	return S_OK;
}

STDMETHODIMP CUSBXBCtl::Out(UCHAR Port,UCHAR Slot, UCHAR Interface, UCHAR Endpoint, UCHAR pid, BSTR * pData, int iDataSize, /*[out, retval]*/ UCHAR * ReturnValue )
{
	BYTE szDataPid = 0;
	TRANSFERDATA sockdata;
	int temp;


	// Insure we have active connection to XBOX server
	if(FALSE == m_fInited)
	{
		Initialize();
		if(FALSE == m_fInited)
		{
			return E_FAIL; // failed initialization, error in m_iLastError
		}
	}
	if(m_Socket == INVALID_SOCKET || 0 == m_Socket)
	{
		m_iLastError = INVALID_SOCKET;
		return E_FAIL;
	}
		
	if(iDataSize >= 1024)
	{
		m_iLastError = ERROR_NOT_ENOUGH_MEMORY;
		return E_FAIL;
	}

	//set up the data packet to send to slixbox
	BYTE bBuffer[1026];
	int iBytesConverted = BstrToBinary(pData, bBuffer, sizeof(bBuffer));


	CREATE_OUT(sockdata, Port,Slot, Interface, Endpoint, szDataPid, bBuffer, iBytesConverted);

	//send the data to slixbox
	temp = ssSend(m_Socket, &sockdata, sockdata.size);
	if(temp <= 0) 
	{
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	}

//	if(isqueue) return 0;

	//try to receive a handshake packet back...
	temp = recv(m_Socket, (char*)&sockdata, 256, 0);	//temp = num of bytes received
	if(temp <= 0) 
	{
		m_iLastError = WSAGetLastError();
		return E_FAIL;

	}

    *ReturnValue = sockdata.ret.ret;	//return the handshake packet from the device

	return S_OK;
}


STDMETHODIMP CUSBXBCtl::Setup(UCHAR Port,UCHAR Slot, UCHAR Interface, UCHAR Endpoint, BSTR * pData, /*[out, retval]*/UCHAR * pRetVal)
{
	int sdcount = 0, i = 0;
	TRANSFERDATA sockdata;
	int temp;

	if(FALSE == m_fInited)
	{
		Initialize();
		if(FALSE == m_fInited)
		{
			*pRetVal = XSTATUS_ConfigurationError;
			return E_FAIL; // failed initialization, error in m_iLastError
		}
	}

	if(m_Socket == INVALID_SOCKET || 0 == m_Socket)
	{
		*pRetVal = XSTATUS_ConfigurationError;
		m_iLastError = INVALID_SOCKET;
		return E_FAIL;
	}

	if(NULL == pData)
	{
		*pRetVal = XSTATUS_ConfigurationError;
		m_iLastError = ERROR_INVALID_DATA;
		return E_FAIL;
	}

	BYTE bBuffer[1026];
	int iBytesConverted = BstrToBinary(pData, bBuffer, sizeof(bBuffer));

	if(0 == iBytesConverted)
	{
		*pRetVal = XSTATUS_ConfigurationError;
		m_iLastError = ERROR_INVALID_DATA;
		return E_FAIL;
	}

	//set up the data packet to send to slixbox
	CREATE_SETUP(sockdata, Port,Slot, Interface, Endpoint, bBuffer, iBytesConverted);

	//send the data to slixbox
	temp = ssSend(m_Socket, &sockdata, sockdata.size);
	if(temp <= 0) 
	{
		*pRetVal = XSTATUS_ConfigurationError;
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	}

	//try to receive a handshake packet back...
	temp = recv(m_Socket, (char*)&sockdata, 256, 0);	//temp = num of bytes received
	if(temp <= 0) 
	{
		*pRetVal = XSTATUS_ConfigurationError;
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	}

	*pRetVal = sockdata.ret.ret;	//return the handshake packet from the device
	return S_OK;


}


STDMETHODIMP CUSBXBCtl::get_TimeoutValue(short *pVal)
{
	*pVal = m_sTimeout;
	return S_OK;
}


STDMETHODIMP CUSBXBCtl::put_TimeoutValue(short newVal)
{
	m_sTimeout = newVal;

	// set the socket with the new timeout value
	int iResult = setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &newVal, sizeof(int));
	if(iResult)
	{
		m_iLastError = WSAGetLastError();
	}
	return S_OK;
}


STDMETHODIMP CUSBXBCtl::SetUsbTimeout(ULONG ulTimeout, UCHAR * ucReturn)
{
	TRANSFERDATA sockdata;
	int iResult;
	*ucReturn = 0; // error value
	if(FALSE == m_fInited)
	{
		Initialize();
		if(FALSE == m_fInited)
		{
			return E_FAIL; // failed initialization, error in m_iLastError
		}
	}
	sockdata.size = sizeof(TRANSFER_USB_TIMEOUT);	//set up the packet to send to slixbox
	sockdata.type = PID_SET_USB_TIMEOUT;
	sockdata.UsbTimeout.ulTimeoutValue = ulTimeout;

	iResult = ssSend(m_Socket, &sockdata, sockdata.size);
	if(iResult <= 0)
	{
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	} 


	iResult = recv(m_Socket, (char*)&sockdata, 256, 0);	//temp = num of bytes received
	if(iResult <= 0) {
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	}

	*ucReturn = sockdata.ret.ret;	//return the handshake packet from the device

	return S_OK;
}



STDMETHODIMP CUSBXBCtl::EnableEvents(BYTE bEventMask)
{

	if(FALSE == m_fInited)
	{
		Initialize();
		if(FALSE == m_fInited)
		{
			return E_FAIL; // failed initialization, error in m_iLastError
		}
	}

	// start thread to listen for xbox events
	if(INVALID_HANDLE_VALUE == m_hEventThread)
	{
		m_hEventThread = (HANDLE) _beginthread(EventHandler,0,this);
		// start thread to listen for xbox events
		if(INVALID_HANDLE_VALUE == m_hEventThread )
		{
			ReleaseMutex(m_hInsertMutex);
			return E_FAIL;
		}
	}
	// release the mutex and let the event thread start processing current events
	ReleaseMutex(m_hInsertMutex);
	
	return S_OK;
}

STDMETHODIMP CUSBXBCtl::InArray(BYTE Port, BYTE Slot, BYTE Interface, SAFEARRAY *Data)
{
	return S_OK;
}

STDMETHODIMP CUSBXBCtl::RefreshConfiguration(UCHAR * pVal)
{

	TRANSFERDATA sockdata;
	int iResult;

	if(FALSE == m_fInited)
	{
		Initialize();
		if(FALSE == m_fInited)
		{
			return E_FAIL; // failed initialization, error in m_iLastError
		}
	}

	sockdata.size = sizeof(TRANSFER_REFRESH_CONFIG);	//set up the packet to send to slixbox
	sockdata.type = PID_GET_DEVICE_STATE;

	iResult = ssSend(m_Socket, &sockdata, sockdata.size);
	if(iResult <= 0)
	{
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	} 


	iResult = recv(m_Socket, (char*)&sockdata, sizeof(TRANSFERDATA), 0);	
	if(iResult <= 0) {
		m_iLastError = WSAGetLastError();
		return E_FAIL;
	}

	// now fire events for each device topology received
	for(int i = 0; i < sockdata.DeviceState.bDeviceCount; i++)
	{
		// fire insertion event
		Fire_InsertionEvent(
				sockdata.DeviceState.TopArray[i].bPort,
				sockdata.DeviceState.TopArray[i].bSlot,
				sockdata.DeviceState.TopArray[i].bInterfaceNumber);
	}

	*pVal = sockdata.ret.ret;	//return the handshake packet from the device


	return S_OK;
}
