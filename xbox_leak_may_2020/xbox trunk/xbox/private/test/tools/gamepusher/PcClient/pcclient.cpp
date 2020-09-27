/*************************************************************************************************

  Copyright (c) 2001 Microsoft Corporation

  Module Name:

	pcclient.cpp

  Abstract:

	Defines the GamePusher PC "client" side functions which allow file data and commands to be
	broadcasted to listening Xboxes.

  Author:

	Mabel Santos (t-msanto) 03-Dec-2001

  Revision History:

*************************************************************************************************/

#include "pcclient.h"

#define PORT 2000

enum PacketTypes{ FILEDESC = 1, FILEDATA, FILEDONE };
enum QueryTypes{ REBOOTQUERY = 1, RECEIVEQUERY };
enum XboxStates{ REBOOTSTATE = 1, RECEIVESTATE, FINISHSTATE, GAMESTATE };

// number of threads to use for rebooting Xboxes
unsigned short NumThreads;

// number of seconds to wait before query timeout
unsigned short QueryTime;

// number of seconds to sleep between file sends
unsigned short FileSleep;

// number of seconds to sleep between packet sends
unsigned short PacketSleep;

// Xbox server app source path
char XbAppSrc[BUFFLEN];

// Xbox server app destination path
char XbAppDst[BUFFLEN];

// name of app to reboot into when finished 
char RebootApp[BUFFLEN];

// date/time identifier for log file
char FileStamp[BUFFLEN];

// log file name
char LogName[BUFFLEN];

// thread synchronization variables
CRITICAL_SECTION ThreadLock;
BOOL ThreadExitFlag;
unsigned short ThreadExitCount;

// CRC table
static const unsigned long CRCs[] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 
    0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2, 
    0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 
    0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C, 
    0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 
    0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106, 
    0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433, 
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 
    0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950, 
    0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65, 
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 
    0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA, 
    0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F, 
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 
    0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84, 
    0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1, 
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 
    0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E, 
    0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B, 
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 
    0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28, 
    0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D, 
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 
    0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242, 
    0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777, 
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 
    0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 
    0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9, 
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 
    0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/*************************************************************************************************

Routine Description:

	Default PcClient constructor.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

PcClient::PcClient(void)
{	
	// initialize Xbox list and tail pointer to NULL
	XboxList = NULL;
	LastXbox = NULL;
	
	//LogPrint("Initialized Xbox list to null\n");

	// set byte counter to 0
	ByteCount = 0;
	//LogPrint("Initialized byte counter to 0\n");

	// set packet counter to 0
	PacketCount = 0;
	//LogPrint("Initialized packet counter to 0\n");

	// set socket to invalid
	ClientSock = INVALID_SOCKET;
	//LogPrint("Set socket to invalid\n");

	// set WinSock flag to false
	WsaInitialized = false;
	//LogPrint("Set WinSock flag to false\n");
}

/*************************************************************************************************

Routine Description:

	Default PcClient destructor.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

PcClient::~PcClient(void)
{
	// close socket and clean up if needed
	if(ClientSock != INVALID_SOCKET)
	{
		closesocket(ClientSock);
	}

	if(WsaInitialized)
	{
		WSACleanup();
	}
}

/*************************************************************************************************

Routine Description:

	PrepareWinSock

	Initializes WinSock. Creates a UDP socket for broadcast use and binds the PC client to it.

Arguments:

	none

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
PcClient::PrepareWinSock(void)
{
	// initialize WinSock
	WORD Version = MAKEWORD(2,2);
	WSADATA WsaData;
	WSAStartup(Version, &WsaData);
	WsaInitialized = true;
	//LogPrint("Initialized WinSock\n");

	// create a UDP datagram socket
	ClientSock = socket(AF_INET, SOCK_DGRAM, 0);
	//LogPrint("Created UDP datagram socket\n");

	if(ClientSock == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) // 10093
	{
		WSAStartup(Version, &WsaData);
		WsaInitialized = true;
		//LogPrint("Initialized WinSock a second time\n");
		ClientSock = socket(AF_INET, SOCK_DGRAM, 0);
		//LogPrint("Created UDP datagram socket a second time\n");
	}

	// set socket up for broadcast use
	BOOL val = true;
	if(setsockopt(ClientSock, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof(BOOL)) == SOCKET_ERROR) 
	{
		LogPrint("SOCKET ERROR in setsockopt()\n");
		return false;
	}
	else 
	{
		//LogPrint("Set socket up for broadcast use\n");
	}

	// try to increase send buffer size
	int Size, BufSize = 0;
	int Err, Res;
	
	// start with 64K, then cut buffer size in half if necessary until request is granted
	for(Size = 65536; Size > 16384; Size >>=1)
	{
		// if got a socket error, try again with a smaller size
		if(setsockopt(ClientSock, SOL_SOCKET, SO_SNDBUF, (char*)&Size, sizeof(int)) == SOCKET_ERROR)
		{
			Err = WSAGetLastError();
			// if option is not supported, break out of loop
			if(Err == WSAENOPROTOOPT || Err == WSAEINVAL) break;
		}
		else
		{
			// reaching here means buffer size was successfully changed
			break;
		}
	}
	Res = sizeof(int);
	getsockopt(ClientSock, SOL_SOCKET, SO_SNDBUF, (char*)&BufSize, &Res);
	//LogPrint("Send buffer size = %d bytes\n", BufSize);

	// fill in address structure
	SOCKADDR_IN ClientAddr;
	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_port = htons(0);
	ClientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//LogPrint("Filled in address structure\n");

	// bind name to socket
	if(bind(ClientSock, (SOCKADDR*)&ClientAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		LogPrint("SOCKET ERROR in bind()\n");
		return false;
	}
	else 
	{
		//LogPrint("Bound name to socket\n");
	}
	return true;
}

/*************************************************************************************************

Routine Description:

	PrepareXboxes

	Assigns and starts threads to put each Xbox listed in the gamepusher.ini file in a listening
	state.

Arguments:

	none

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
PcClient::PrepareXboxes(void)
{
	int CurrThread = 0;

	//LogPrint("XbAppSrc: %s\n", XbAppSrc);
	//LogPrint("XbAppDst: %s\n", XbAppDst);

	FILE* f = fopen("C:\\GamePusher\\gamepusher.ini", "r");
	//FILE* f = fopen("D:\\GamePusher\\gamepusher.ini", "r");
	
	BOOL XboxFlag = false;

	if(f)
	{
		char line[BUFFLEN];
		char Xbox[BUFFLEN];
		char strtemp[BUFFLEN];
		int inttemp = 0;

		while(fgets(line, BUFFLEN, f))
		{
			// find start of Xboxlist section
			if(sscanf(line, "XboxList=%s", Xbox) == 1)
			{
				// set flag to indicate that we've found it
				XboxFlag = true;

				// if first Xbox is not on the exclude list, assign it to a thread
				if(!CheckExclList("C:\\GamePusher\\xboxexclude.txt", Xbox))
				//if(!CheckExclList("D:\\GamePusher\\xboxexclude.txt", Xbox))
				{
					AddXbox(Xbox, (CurrThread % NumThreads) + 1);
					++CurrThread;
				}
				else
				{
					//LogPrint("Xbox %s found on exclude list. Skipping...\n", Xbox);
				}
				break;
			}
		}

		while(XboxFlag && fgets(line, BUFFLEN, f))
		{
			// check if we've reached end of Xboxlist section
			if((sscanf(line, "%[^=]=%s", Xbox, strtemp) == 2) ||
				(sscanf(line, "%[^=]=%d", Xbox, inttemp) == 2))
			{
				// set flag to indicate that we're finished
				XboxFlag = false;
				break;
			}
			// reaching here means there's another Xbox on the list
			StringTrim(line);

			// if this Xbox is not on the exclude list, assign it to a thread
			if(!CheckExclList("C:\\GamePusher\\xboxexclude.txt", line))
			//if(!CheckExclList("D:\\GamePusher\\xboxexclude.txt", line))
			{
				AddXbox(line, (CurrThread % NumThreads) + 1);
				++CurrThread;
			}
			else
			{
				//LogPrint("Xbox %s found on exclude list. Skipping...\n", line);
			}
		}
		fclose(f);
	}
	else
	{
		LogPrint("ERROR: Could not open gamepusher.ini file\n");
		return false;
	}

	// initialize critical section variables
	InitializeCriticalSection(&ThreadLock);
	ThreadExitFlag = false;
	ThreadExitCount = 0;

	// start specified number of threads
	int ThreadCount;
	for(ThreadCount = 1; ThreadCount <= NumThreads; ++ThreadCount)
	{
		ThreadParam* ThreadArg = new ThreadParam;
		ThreadArg->Client = this;
		ThreadArg->ThreadNum = ThreadCount;
		CreateThread(NULL, 0, PrepareXboxThread, ThreadArg, 0, NULL);
	}

	// wait for all threads to finish
	while(!ThreadExitFlag)
	{
		Sleep(100);
		EnterCriticalSection(&ThreadLock);
		if(ThreadExitCount == NumThreads) ThreadExitFlag = true;
		LeaveCriticalSection(&ThreadLock);
	}

	DeleteCriticalSection(&ThreadLock);
	return true;
}

/*************************************************************************************************

Routine Description:

	PrepareXboxThread

	Puts Xboxes in a listening state by copying the XbServer app into each Xbox and rebooting it
	into the app.

Arguments:

	none

Return Value:

	DWORD	-	returns 1 after thread exits

*************************************************************************************************/

DWORD WINAPI
PrepareXboxThread(void* ThreadArg)
{
	// get thread number
	int ThreadNum = ((ThreadParam*)ThreadArg)->ThreadNum;

	// get Xbox list
	Xbox* Curr;
	for(Curr = ((ThreadParam*)ThreadArg)->Client->XboxList; Curr != NULL; Curr = Curr->Next)
	{
		// if current Xbox belongs to this thread, prepare it
		if(Curr->Thread == ThreadNum)
		{	
			//LogPrint("CurrXbox: %s\n", Curr->Name);
			//LogPrint("Thread: %u\n", Curr->Thread);
			int Res;
		
			// resolve Xbox name into its IP addresses
			DmSetXboxName(Curr->Name);
			DWORD HostAddr;
			struct in_addr NetAddr;
			LPSTR IpAddr;

			// get debug IP
			DmResolveXboxName(&HostAddr);
			NetAddr.s_addr = htonl(HostAddr);
			IpAddr = inet_ntoa(NetAddr);
			//LogPrint("DebugIp: %s\n", IpAddr);
		
			// reboot Xbox
			char Cmd1[BUFFLEN];
			strcpy(Cmd1, "xbreboot -x ");
			strcat(Cmd1, Curr->Name);

			//LogPrint("%s\n", Cmd1);
			Res = system(Cmd1);
			if(Res)
			{
				LogPrint("WARNING: Could not reboot %s\n", Curr->Name);
				continue;
			}
			//LogPrint("Rebooted %s\n", Curr->Name);
		
			// sleep for 1 sec to make sure box has been rebooted
			Sleep(1000);

			// copy over Xbox application
			char Cmd2[BUFFLEN];
			strcpy(Cmd2, "xbcp -x ");
			strcat(Cmd2, Curr->Name);
			strcat(Cmd2, " -ty "); // options: create dest dir if it does not exist, don't prompt to overwrite
			strcat(Cmd2, XbAppSrc);
			strcat(Cmd2, " ");
			strcat(Cmd2, XbAppDst);

			//LogPrint("%s\n", Cmd2);
			Res = system(Cmd2);
			if(Res)
			{
				LogPrint("WARNING: Could not copy %s into %s\n", XbAppSrc, Curr->Name);
				continue;
			}
			//LogPrint("Copied %s into %s\n", XbAppSrc, XbAppDst);

			// sleep for 1 sec to make sure app has been copied over
			Sleep(1000);

			// reboot Xbox into application
			char Cmd3[BUFFLEN];
			strcpy(Cmd3, "xbreboot -x ");
			strcat(Cmd3, Curr->Name);
			strcat(Cmd3, " ");
			strcat(Cmd3, XbAppDst);

			//LogPrint("%s\n", Cmd3);
			Res = system(Cmd3);
			if(Res)
			{
				LogPrint("WARNING: Could not reboot %s into %s\n", Curr->Name, XbAppDst);
				continue;
			}
			//LogPrint("Rebooted %s into %s\n", Curr->Name, XbAppDst);
		}
	}

	// increment count of exited threads
	EnterCriticalSection(&ThreadLock);
	++ThreadExitCount;
	LeaveCriticalSection(&ThreadLock);

	delete ((ThreadParam*)ThreadArg);
	return 1;
}

/*************************************************************************************************

Routine Description:

	CheckExclList

	Checks if a given name is on a specified exclude list.

Arguments:

	char* List	-	list to search
	char* Name	-	name to look for

Return Value:

	BOOL	-	 true if name is found on list; false otherwise

*************************************************************************************************/

BOOL
PcClient::CheckExclList(char* List, char* Name)
{
	FILE* f = fopen(List, "r");

	if(f)
	{
		char line[BUFFLEN];

		// check if any names on exclude list match given name
		while(fgets(line, BUFFLEN, f))
		{
			StringTrim(line);
			if(_stricmp(line, Name) == 0)
			{
				return true;
			}
		}
		fclose(f);
	}
	// reaching here means name was not found
	return false;
}

/*************************************************************************************************

Routine Description:

	GetTotalBytes

	Adds up the size of each file listed in the gamepusher.ini file to determine the total number
	of bytes to be sent over the network.

Arguments:

	none

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
PcClient::GetTotalBytes(void)
{
	FILE* f = fopen("C:\\GamePusher\\gamepusher.ini", "r");
	//FILE* f = fopen("D:\\GamePusher\\gamepusher.ini", "r");
	BOOL FileFlag = false;

	if(f)
	{
		char line[BUFFLEN];
		char file[BUFFLEN];
		char strtemp[BUFFLEN];
		int inttemp = 0;
		
		while(fgets(line, BUFFLEN, f))
		{
			// find start of filelist section
			if(sscanf(line, "FileList=%s", file) == 1)
			{
				// set flag to indicate that we've found it
				FileFlag = true;
		
				// if first file is not on the exclude list, add its size to the byte counter
				if(!CheckExclList("C:\\GamePusher\\fileexclude.txt", file))
				//if(!CheckExclList("D:\\GamePusher\\fileexclude.txt", file))
				{
					GetFileBytes(file);
					
				}
				break;
			}
		}

		while(FileFlag && fgets(line, BUFFLEN, f))
		{
			// check if we've reached end of filelist section
			if((sscanf(line, "%[^=]=%s", file, strtemp) == 2) ||
			(sscanf(line, "%[^=]=%d", file, inttemp) == 2))
			{
				// set flag to indicate that we're finished
				FileFlag = false;
				break;
			}
			// reaching here means there's another file on the list
			StringTrim(line);
			
			// if this file is not on the exclude list, add its size to the byte counter
			if(!CheckExclList("C:\\GamePusher\\fileexclude.txt", line))
			//if(!CheckExclList("D:\\GamePusher\\fileexclude.txt", line))
			{
				GetFileBytes(line);
			}
		}
		fclose(f);
		//LogPrint("Total bytes to be sent: %u\n", ByteCount);
		return true;
	}
	else
	{
		LogPrint("ERROR: Could not open gamepusher.ini file\n");
		return false;
	}
}

/*************************************************************************************************

Routine Description:

	GetFileBytes

	Determines the size of a specified file in bytes.

Arguments:

	char* File	-	name of file whose size is to be determined

Return Value:

	none

*************************************************************************************************/

void
PcClient::GetFileBytes(char* File)
{
	char PcFile[BUFFLEN];
	
	int Mid = strcspn(File, ",");
	int End = strcspn(File, "\n");

	// copy source filename into a temp buffer
	memmove(PcFile, File, Mid);
	memset(PcFile+Mid, '\0', BUFFLEN-Mid);
	
	HANDLE FileHandle;
	DWORD FileSize;
	
	// get file size
	FileHandle = CreateFile(PcFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	FileSize = GetFileSize(FileHandle, NULL);
	ByteCount += FileSize;
	CloseHandle(FileHandle);
}

/*************************************************************************************************

Routine Description:

	SendQuery

	Sends a broadcast message and waits for responses to see which Xboxes are in a listening
	state.

Arguments:

	int QueryType	-	type of query being sent
						REBOOTQUERY: sent after Xboxes reboot into XbServer app
						RECEIVEQUERY: sent after Xboxes receive file data

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
PcClient::SendQuery(int QueryType)
{
	int Result;
	SOCKADDR_IN DestAddr;
	int AddrSize = sizeof(SOCKADDR);

	DestAddr.sin_family = AF_INET;
	DestAddr.sin_port = htons(PORT);
	DestAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	// Send broadcast query to servers
	char QueryBuf[BUFFLEN];

	if(QueryType == REBOOTQUERY)
	{
		// if this is the first query, include total byte count so Xboxes know how much data to expect
		_ultoa(ByteCount, QueryBuf, 10);

		if(sendto(ClientSock, QueryBuf, strlen(QueryBuf), 0, (SOCKADDR*)&DestAddr, AddrSize) == SOCKET_ERROR)
		{
			LogPrint("SOCKET ERROR in sendto()\n");
			return false;
		}
	}
	else
	{
		if(sendto(ClientSock, NULL, 0, 0, (SOCKADDR*)&DestAddr, AddrSize) == SOCKET_ERROR)
		{
			LogPrint("SOCKET ERROR in sendto()\n");
			return false;
		}
	}

	//LogPrint("Sent broadcast query to servers\n");

	// wait for reply from each server
	//LogPrint("Waiting for reply from servers\n");

	TIMEVAL Timeout;
	FD_SET Bucket;

	// timeout after number of seconds specified in .INI file
	time_t Start, Curr;

	for(time(&Start), time(&Curr); difftime(Curr, Start) <= QueryTime; time(&Curr))
	{
		Bucket.fd_count = 1;
		Bucket.fd_array[0] = ClientSock;
		Timeout.tv_sec = 5;
		Timeout.tv_usec = 0;

		// clear buffer
		memset(QueryBuf, '\0', sizeof(QueryBuf));

		// wait for response
		Result = select(0, &Bucket, NULL, NULL, &Timeout);

		if(Result == 0) continue; // timed out so try again
		
		if(Result == SOCKET_ERROR)
		{
			LogPrint("SOCKET ERROR in select()\n");
			return false;
		}

		// grab response
		Result = recvfrom(ClientSock, QueryBuf, sizeof(QueryBuf), 0, (SOCKADDR*)&DestAddr, &AddrSize);	  
		
		if(Result == 0) 
		{
			// no more data
			//LogPrint("No more data. Breaking out of loop...\n");
			break;
		}

		if(Result == SOCKET_ERROR)
		{
			LogPrint("SOCKET ERROR in recvfrom()\n");
			return false;
		}

		// reaching here means we got a valid response	
		/*LogPrint("Received reply from server %u.%u.%u.%u \"%s\"\n", 
					DestAddr.sin_addr.S_un.S_un_b.s_b1, DestAddr.sin_addr.S_un.S_un_b.s_b2,
					DestAddr.sin_addr.S_un.S_un_b.s_b3, DestAddr.sin_addr.S_un.S_un_b.s_b4,
					QueryBuf);*/

		// change Xbox state according to the query type
		switch(QueryType)
		{
			// Xboxes that respond are rebooted into listening app and ready to receive files
			case REBOOTQUERY:

				SetXboxState(QueryBuf, RECEIVESTATE);
				break;

			// Xboxes that respond are done receiving files
			case RECEIVEQUERY:

				SetXboxState(QueryBuf, FINISHSTATE);
				break;
		}
	}

	// reaching here means we've timed out, so report which Xboxes responded in time and which ones did not
	switch(QueryType)
	{
		case REBOOTQUERY:
	
			LogPrint("Boxes that responded after rebooting:\n");
			//LogPrint("Got a response from the following Xboxes:\n");
			GetXboxes(RECEIVESTATE);
			LogPrint("Boxes that did NOT respond after rebooting:\n");
			//LogPrint("Did NOT get a response from the following Xboxes:\n");
			GetXboxes(REBOOTSTATE);
			break;

		case RECEIVEQUERY:

			LogPrint("Boxes that responded after receiving files:\n");
			//LogPrint("Got a response from the following Xboxes:\n");
			GetXboxes(FINISHSTATE);
			LogPrint("Boxes that did NOT respond after receiving files:\n");
			//LogPrint("Did NOT get a response from the following Xboxes:\n");
			GetXboxes(RECEIVESTATE);
			break;
	}
	return true;
}

/*************************************************************************************************

Routine Description:

	SendFiles

	Sends files listed in the gamepusher.ini file over the network.

Arguments:

	none

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
PcClient::SendFiles(void)
{
	FILE* f = fopen("C:\\GamePusher\\gamepusher.ini", "r");
	//FILE* f = fopen("D:\\GamePusher\\gamepusher.ini", "r");
	BOOL FileFlag = false;

	if(f)
	{
		char line[BUFFLEN];
		char file[BUFFLEN];
		char strtemp[BUFFLEN];
		int inttemp = 0;
		
		while(fgets(line, BUFFLEN, f))
		{
			// find start of filelist section
			if(sscanf(line, "FileList=%s", file) == 1)
			{
				// set flag to indicate that we've found it
				FileFlag = true;
		
				// if first file is not on the exclude list, send it
				if(!CheckExclList("C:\\GamePusher\\fileexclude.txt", file))
				//if(!CheckExclList("D:\\GamePusher\\fileexclude.txt", file))
				{
					if(!SendFile(file))
					{
						return false;
					}
				}
				else
				{
					//LogPrint("File %s found on exclude list. Skipping...\n", file);
				}
				break;
			}
		}

		while(FileFlag && fgets(line, BUFFLEN, f))
		{
			// check if we've reached end of filelist section
			if((sscanf(line, "%[^=]=%s", file, strtemp) == 2) ||
			(sscanf(line, "%[^=]=%d", file, inttemp) == 2))
			{
				// set flag to indicate that we're finished
				FileFlag = false;
				break;
			}
			// reaching here means there's another file on the list
			StringTrim(line);
			
			// if this file is not on the exclude list, send it
			if(!CheckExclList("C:\\GamePusher\\fileexclude.txt", line))
			//if(!CheckExclList("D:\\GamePusher\\fileexclude.txt", line))
			{
				if(!SendFile(line))
				{
					return false;
				}
			}
			else
			{
				//LogPrint("File %s found on exclude list. Skipping...\n", line);
			}
		}
		fclose(f);
		return true;
	}
	else
	{
		LogPrint("ERROR: Could not open gamepusher.ini file\n");
		return false;
	}
}

/*************************************************************************************************

Routine Description:

	SendFile

	Sends a specified file over the network.

Arguments:

	char* File	-	 name of file to be sent

Return Value:

	BOOL	-	 true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
PcClient::SendFile(char* File)
{
	// sleep before sending another file to allow Xboxes to receive previous data
	if(FileSleep > 0)
	{
		Sleep(FileSleep);
	}

	char PcFile[BUFFLEN];
	char XbFile[BUFFLEN];
	
	int Mid = strcspn(File, ",");
	int End = strcspn(File, "\n");

	// copy source filename into a temp buffer
	memmove(PcFile, File, Mid);
	memset(PcFile+Mid, '\0', BUFFLEN-Mid);
	//LogPrint("Source: %s\n", PcFile);

	// copy destination filename into a temp buffer
	memmove(XbFile, File+(Mid+1), End-(Mid+1));
	memset(XbFile+(End-(Mid+1)), '\0', BUFFLEN-(End-(Mid+1)));
	//LogPrint("Destination: %s\n", XbFile);

	HANDLE FileHandle;
	DWORD FileSize;
	char SendBuf[BUFFLEN];
	size_t ReadRes;
	
	// get file size
	FileHandle = CreateFile(PcFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	FileSize = GetFileSize(FileHandle, NULL);
	//LogPrint("Size: %d bytes\n", FileSize);
	CloseHandle(FileHandle);

	// open infile for reading in binary mode
	FILE* InFile = fopen(PcFile, "rb");

	// skip file if we can't find/open it
	if(InFile == NULL)
	{
		LogPrint("WARNING: Could not open file %s\n", PcFile);
		LogPrint("WARNING: File %s not sent\n", PcFile);
		return true;
	}
	
	// send file description packet
	if(!SendPacket(FILEDESC, FileSize, strlen(XbFile), XbFile))
	{
		LogPrint("ERROR: File description not sent\n");
		LogPrint("ERROR: File %s not sent\n", PcFile);
		return false;
	}
	//LogPrint("Sent file description packet\n");

	while(!feof(InFile))
	{
		// clear buffer and read 1K of data from infile
		memset(SendBuf, '\0', sizeof(SendBuf));
		ReadRes = fread(SendBuf, sizeof(char), BUFFLEN, InFile);

		// if there is a problem with reading from the file, return
		if(ferror(InFile))
		{
			LogPrint("ERROR: Could not read data from file\n");
			LogPrint("ERROR: File %s not sent\n", PcFile);
			return false;
		}

		// if read 0 bytes, end of file has been reached so break out of loop
		if(ReadRes == 0)
		{
			break;
		}

		//LogPrint("Read %dB of data from file\n", ReadRes);

		// send file data packet
		if(!SendPacket(FILEDATA, FileSize, ReadRes, SendBuf))
		{
			LogPrint("ERROR: Could not send file data\n");
			LogPrint("ERROR: File %s not sent\n", PcFile);
			return false;
		}
		//LogPrint("Sent file data packet\n");
	}
	
	// close infile stream
	fclose(InFile);
	//LogPrint("Sent file %s successfully\n", PcFile);
	return true;
}

/*************************************************************************************************

Routine Description:

	SendFlashCmd

	Sends a command for Xboxes to flash ROM over the network.

Arguments:

	none

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
PcClient::SendFinishMsg(void)
{	
	if(!SendPacket(FILEDONE, 0, 0, NULL))
	{
		LogPrint("ERROR: Finish message not sent\n");
		return false;
	}

	//LogPrint("Sent finish message\n");
	return true;
}

/*************************************************************************************************

Routine Description:

	RebootXboxes

	Assigns and starts threads to reboot each Xbox listed in the gamepusher.ini file that is in a
	listening state.

Arguments:

	none

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

void
PcClient::RebootXboxes(void)
{
	if(strcmp(RebootApp, "NONE") != 0)
	{
		//LogPrint("RebootApp: %s\n", RebootApp);
	}

	// initialize critical section variables
	InitializeCriticalSection(&ThreadLock);
	ThreadExitFlag = false;
	ThreadExitCount = 0;

	// start specified number of threads
	int ThreadCount;
	for(ThreadCount = 1; ThreadCount <= NumThreads; ++ThreadCount)
	{
		ThreadParam* ThreadArg = new ThreadParam;
		ThreadArg->Client = this;
		ThreadArg->ThreadNum = ThreadCount;
		CreateThread(NULL, 0, RebootXboxThread, ThreadArg, 0, NULL);
	}

	// wait for all threads to finish
	while(!ThreadExitFlag)
	{
		Sleep(100);
		EnterCriticalSection(&ThreadLock);
		if(ThreadExitCount == NumThreads) ThreadExitFlag = true;
		LeaveCriticalSection(&ThreadLock);
	}

	DeleteCriticalSection(&ThreadLock);
}

/*************************************************************************************************

Routine Description:

	RebootXboxThread

	Reboots Xboxes into app listed in the gamepusher.ini file, or into the XDK launcher if there is
	no app specified.

Arguments:

	none

Return Value:

	DWORD	-	returns 1 after thread exits

*************************************************************************************************/

DWORD WINAPI
RebootXboxThread(void* ThreadArg)
{
	// get thread number
	int ThreadNum = ((ThreadParam*)ThreadArg)->ThreadNum;

	if(strcmp(RebootApp, "NONE") != 0)
	{
		// get Xbox list
		Xbox* Curr;
		for(Curr = ((ThreadParam*)ThreadArg)->Client->XboxList; Curr != NULL; Curr = Curr->Next)
		{
			// reboot each Xbox that recovered successfully into specified application
			if(Curr->State == FINISHSTATE && Curr->Thread == ThreadNum)
			//if(Curr->Thread == ThreadNum)
			{
				//LogPrint("CurrXbox: %s\n", Curr->Name);
				//LogPrint("Thread: %u\n", Curr->Thread);
				int Res;
				char XboxName[BUFFLEN];
				strcpy(XboxName, Curr->Name);
				char Cmd1[BUFFLEN];
				strcpy(Cmd1, "xbreboot -xc ");
				strcat(Cmd1, XboxName);
				strcat(Cmd1, " ");
				strcat(Cmd1, RebootApp);

				//LogPrint("%s\n", Cmd1);

				Res = system(Cmd1);
				if(Res)
				{
					LogPrint("WARNING: Could not reboot %s into %s\n", XboxName, RebootApp);
				}
				else
				{
					//LogPrint("Rebooted %s into %s\n", XboxName, RebootApp);
				
					// set Xbox state to indicate that it's been rebooted
					((ThreadParam*)ThreadArg)->Client->SetXboxState(XboxName, GAMESTATE);
				}
			}
		}
	}
	else
	{
		Xbox* Curr;
		for(Curr = ((ThreadParam*)ThreadArg)->Client->XboxList; Curr != NULL; Curr = Curr->Next)
		{
			// reboot each Xbox that recovered successfully into XDK launcher
			if(Curr->State == FINISHSTATE && Curr->Thread == ThreadNum)
			//if(Curr->Thread == ThreadNum)
			{
				//LogPrint("CurrXbox: %s\n", Curr->Name);
				//LogPrint("Thread: %u\n", Curr->Thread);
				int Res;
				char XboxName[BUFFLEN];
				strcpy(XboxName, Curr->Name);
				char Cmd1[BUFFLEN];
				strcpy(Cmd1, "xbreboot -xc ");
				strcat(Cmd1, XboxName);

				//LogPrint("%s\n", Cmd1);

				Res = system(Cmd1);
				if(Res)
				{
					LogPrint("WARNING: Could not reboot %s\n", XboxName);
				}
				else
				{
					//LogPrint("Rebooted %s\n", XboxName);
				
					// set Xbox state to indicate that it's been rebooted
					((ThreadParam*)ThreadArg)->Client->SetXboxState(XboxName, GAMESTATE);
				}
			}
		}
	}

	// increment count of exited threads
	EnterCriticalSection(&ThreadLock);
	++ThreadExitCount;
	LeaveCriticalSection(&ThreadLock);

	delete ((ThreadParam*)ThreadArg);
	return 1;
}

/*************************************************************************************************

Routine Description:

	ReportResults

	Lists which boxes successfully received data and commands, and which ones failed.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void
PcClient::ReportResults(void)
{
/*
	LogPrint("Boxes that did not respond after rebooting:\n");
	GetXboxes(REBOOTSTATE);

	LogPrint("Boxes that did not respond after receiving files:\n");
	GetXboxes(RECEIVESTATE);
*/
	if(strcmp(RebootApp, "NONE") != 0)
	{
		LogPrint("Boxes that have been rebooted into %s:\n", RebootApp);
		GetXboxes(GAMESTATE);

		LogPrint("Boxes that could NOT be rebooted into %s:\n", RebootApp);
		GetXboxes(FINISHSTATE);
	}
	else
	{
		LogPrint("Boxes that have been rebooted into XDK launcher:\n");
		GetXboxes(GAMESTATE);
		
		LogPrint("Boxes that could NOT be rebooted into XDK launcher:\n");
		GetXboxes(FINISHSTATE);
	}
}

/*************************************************************************************************

Routine Description:

	SendPacket

	Sends a data packet over the network.

Arguments:

	BYTE PacketType	-	type of packet being sent
						FILEDESC: file description and path
						FILEDATA: file data
						FILEDONE: notification that all files have been sent
	DWORD FileSize	-	total size of current file in bytes
	WORD DataSize	-	size of data being sent in bytes
	char* Data		-	data being sent

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
PcClient::SendPacket(BYTE PacketType, DWORD FileSize, WORD DataSize, char* Data)
{
	// sleep before sending another packet to allow Xboxes to receive previous data
	if(PacketSleep > 0)
	{
		Sleep(PacketSleep);
	}

	int Result;
	SOCKADDR_IN dest;
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	dest.sin_port = htons(PORT);

	// increment packet count
	++PacketCount;
	//LogPrint("Incremented packet count to %d\n", PacketCount);

	char SendBuf[sizeof(FilePacketHeader) + BUFFLEN];
	memset(SendBuf, '\0', sizeof(SendBuf));
	FilePacket* Packet = (FilePacket*)SendBuf;
	Packet->PacketType = PacketType;
	Packet->SequenceNo = PacketCount;
	Packet->FileSize = FileSize;
	Packet->DataSize = DataSize;
	memmove(Packet->Data, Data, DataSize);
	Packet->Crc = GetCRC((unsigned char*)Packet->Data, Packet->DataSize);
	

	// display packet info
	//LogPrint("PacketType: %d\n", Packet->PacketType);
	//LogPrint("SequenceNo: %d\n", Packet->SequenceNo);
	//LogPrint("DataSize: %d\n", Packet->DataSize);
	//LogPrint("Crc: %d\n", Packet->Crc);

	// send packet
	Result = sendto(ClientSock, SendBuf, DataSize + sizeof(FilePacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR));
	if(Result == SOCKET_ERROR)
	{
		LogPrint("SOCKET ERROR in sendto()\n");
		return false;
	}
	return true;
}

/*************************************************************************************************

Routine Description:

	GetCRC

	Determines the CRC for the specified data.

Arguments:

	unsigned char* Data	-	data whose CRC is to be determined
	WORD Size			-	size of data

Return Value:

	DWORD	-	CRC for data

*************************************************************************************************/

DWORD
PcClient::GetCRC(unsigned char* Data, WORD Size)
{
	DWORD Result = 0;
	DWORD Shift = 0;
	DWORD Index = 0;

	while(Size--)
	{
		Result = (Result >> 8) ^ CRCs[(BYTE)(Result ^ *Data++)];
	}
	return Result;
}

/*************************************************************************************************

Routine Description:

	AddXbox

	Adds an Xbox to the Xbox list and assigns it to a thread.

Arguments:

	char* Name	-	name of Xbox
	int Thread	-	thread number assigned to Xbox

Return Value:

	none

*************************************************************************************************/

void
PcClient::AddXbox(char* Name, int Thread)
{
	// make new node
	Xbox* NewXbox = new Xbox;
	strcpy(NewXbox->Name, Name);
	NewXbox->State = REBOOTSTATE;
	NewXbox->Thread = Thread;
	NewXbox->Next = NULL;

	// if the list is currently empty, make this node the first and last
	if(XboxList == NULL && LastXbox == NULL)
	{
		XboxList = NewXbox;
		LastXbox = NewXbox;
	}
	else // else just add this node to the tail of the list
	{
		LastXbox->Next = NewXbox;
		LastXbox = NewXbox;
	}
}

/*************************************************************************************************

Routine Description:

	SetXboxState

	Sets the state of an Xbox on the Xbox list to a new value.

Arguments:

	char* Name	-	name of Xbox whose state is to be changed
	int State	-	new Xbox state
					REBOOTSTATE: box needs to reboot into listening app
					RECEIVESTATE: box needs to receive file data
					FINISHSTATE: box has finished receiving files and needs to be rebooted
					GAMESTATE: box has been rebooted

Return Value:

	none

*************************************************************************************************/

void
PcClient::SetXboxState(char* Name, int State)
{
	Xbox* Curr;
	for(Curr = XboxList; Curr != NULL; Curr = Curr->Next)
	{
		if(_stricmp(Curr->Name, Name) == 0) // we've found the node we're looking for
		{
			Curr->State = State;
		}
	}
}

/*************************************************************************************************

Routine Description:

	GetXboxes

	Lists Xboxes on the Xbox list with the specified state.

Arguments:

	int State	-	state to search for

Return Value:

	none

*************************************************************************************************/

void
PcClient::GetXboxes(int State)
{
	BOOL Found = false;
	Xbox* Curr;
	for(Curr = XboxList; Curr != NULL; Curr = Curr->Next)
	{
		// if current Xbox has the state we're looking for, print it out
		if(Curr->State == State)
		{
			LogPrint("%s\n", Curr->Name);
			Found = true;
		}
	}
	if(!Found)
	{
		LogPrint("NONE\n");
	}
}

/*************************************************************************************************

Routine Description:

	ClearXboxes

	Deletes all Xboxes on the Xbox list.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void
PcClient::ClearXboxes(void)
{
	while(XboxList != NULL)
	{
		// delete all nodes on the list
		Xbox* Curr = XboxList;
		XboxList = Curr->Next;
		delete Curr;
	}
	LastXbox = NULL;
}

/*************************************************************************************************

Routine Description:

	StringTrim

	Trims off tabs and newlines from a given string.

Arguments:

	char* String	-	string to be trimmed

Return Value:

	none

*************************************************************************************************/

void
StringTrim(char* String)
{
	// trim off tabs and/or newlines
	int Pos = strcspn(String, "\t\n");
	String[Pos] = '\0';
}

/*************************************************************************************************

Routine Description:

	LogPrint

	Prints a line to standard output and to a logfile.

Arguments:

	char* Format, ...	-	format string	

Return Value:

	none

*************************************************************************************************/

void
LogPrint(char* Format, ...)
{
	va_list ArgList;
	va_start(ArgList, Format);
	
	// print to stdout
	vprintf(Format, ArgList);

	// print to logfile
	FILE* LogFile = fopen(LogName, "a");

	if(LogFile)
	{
		vfprintf(LogFile, Format, ArgList);
		fclose(LogFile);
	}

	va_end(ArgList);
}

/*************************************************************************************************

Routine Description:

	main function

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void
main(void)
{
	char DateBuf[9];
	char TimeBuf[9];

	_strdate(DateBuf);
	_strtime(TimeBuf);

	// get date without year for file stamp
	DateBuf[5] = '\0';
	// change '/' in date to '-' so fopen is happy
	DateBuf[2] = '-';

	// get time without secs for file stamp
	TimeBuf[5] = '\0';
	// change ':' in time to '-' so fopen is happy
	TimeBuf[2] = '-';

	// set date/time file stamp
	strcpy(FileStamp, DateBuf);
	strcat(FileStamp, "_");
	strcat(FileStamp, TimeBuf);

	// set log file name
	strcpy(LogName, "log_");
	strcat(LogName, FileStamp);
	strcat(LogName, ".txt");

	// restore original date
	DateBuf[2] = '/';
	DateBuf[5] = '/';

	// restore original time
	TimeBuf[2] = ':';
	TimeBuf[5] = ':';

	// get start time
	LogPrint("Start Time: %s %s\n", DateBuf, TimeBuf);

	// get number of threads to use for rebooting Xboxes from .INI file
	NumThreads = GetPrivateProfileIntA("Settings", "NumThreads", 5, "C:\\GamePusher\\gamepusher.ini");
	//NumThreads = GetPrivateProfileIntA("Settings", "NumThreads", 5, "D:\\GamePusher\\gamepusher.ini");
	//LogPrint("NumThreads: %d\n", NumThreads);

	// get number of seconds to wait before query timeout from .INI file
	QueryTime = GetPrivateProfileIntA("Settings", "QueryTime", 30, "C:\\GamePusher\\gamepusher.ini");
	//QueryTime = GetPrivateProfileIntA("Settings", "QueryTime", 30, "D:\\GamePusher\\gamepusher.ini");
	//LogPrint("QueryTime: %d\n", QueryTime);

	// get number of seconds to sleep before file sends from .INI file
	FileSleep = GetPrivateProfileIntA("Settings", "FileSleep", 2, "C:\\GamePusher\\gamepusher.ini");
	//FileSleep = GetPrivateProfileIntA("Settings", "FileSleep", 2, "D:\\GamePusher\\gamepusher.ini");
	//LogPrint("FileSleep: %d\n", FileSleep);

	// get number of seconds to sleep before packet sends from .INI file
	PacketSleep = GetPrivateProfileIntA("Settings", "PacketSleep", 1, "C:\\GamePusher\\gamepusher.ini");
	//PacketSleep = GetPrivateProfileIntA("Settings", "PacketSleep", 1, "D:\\GamePusher\\gamepusher.ini");
	//LogPrint("PacketSleep: %d\n", PacketSleep);

	// get Xbox app source path from .INI file
	GetPrivateProfileStringA("Settings", "XbAppSrc", "C:\\GamePusher\\XbServer.xbe", XbAppSrc, BUFFLEN, "C:\\GamePusher\\gamepusher.ini");
	//GetPrivateProfileStringA("Settings", "XbAppSrc", "D:\\GamePusher\\XbServer.xbe", XbAppSrc, BUFFLEN, "D:\\GamePusher\\gamepusher.ini");
	
	// get Xbox app destination path from .INI file
	GetPrivateProfileStringA("Settings", "XbAppDst", "xE:\\GamePusher\\XbServer.xbe", XbAppDst, BUFFLEN, "C:\\GamePusher\\gamepusher.ini");
	//GetPrivateProfileStringA("Settings", "XbAppDst", "xE:\\GamePusher\\XbServer.xbe", XbAppDst, BUFFLEN, "D:\\GamePusher\\gamepusher.ini");

	// get app to reboot into when finished from .INI file
	GetPrivateProfileStringA("Settings", "RebootApp", "NONE", RebootApp, BUFFLEN, "C:\\GamePusher\\gamepusher.ini");
	//GetPrivateProfileStringA("Settings", "RebootApp", "NONE", RebootApp, BUFFLEN, "D:\\GamePusher\\gamepusher.ini");

	// make a PcClient object
	PcClient* Client = new PcClient;

	// set up network stuff
	if(!Client->PrepareWinSock())
	{
		Client->ClearXboxes();
		delete Client;
		return;
	}

	LogPrint("Preparing Xboxes...\n");
	// set up Xbox servers
	if(!Client->PrepareXboxes())
	{
		Client->ClearXboxes();
		delete Client;
		return;
	}

	// add up total number of data bytes to send over the network
	if(!Client->GetTotalBytes())
	{
		Client->ClearXboxes();
		delete Client;
		return;
	}

	// sleep for 10 secs to make sure Xboxes are ready to receive query
	Sleep(10000);

	LogPrint("Pinging Xboxes...\n");
	// send reboot query to servers and wait for response
	if(!Client->SendQuery(REBOOTQUERY))
	{
		Client->ClearXboxes();
		delete Client;
		return;
	}

	LogPrint("Sending files...\n");
	// send files
	if(!Client->SendFiles())
	{
		Client->ClearXboxes();
		delete Client;
		return;
	}

	LogPrint("Sending finish message...\n");
	// send finish message
	if(!Client->SendFinishMsg())
	{
		Client->ClearXboxes();
		delete Client;
		return;
	}

	// sleep for 10 secs to make sure Xboxes are ready to receive query
	Sleep(10000);

	LogPrint("Pinging Xboxes...\n");
	// send receive query to servers and wait for response 
	if(!Client->SendQuery(RECEIVEQUERY))
	{
		Client->ClearXboxes();
		delete Client;
		return;
	}

	LogPrint("Rebooting Xboxes...\n");
	// reboot Xboxes into specified application
	Client->RebootXboxes();

	// report which Xboxes recovered successfully and which ones did not
	Client->ReportResults();

	// get end time
	_strdate(DateBuf);
	_strtime(TimeBuf);
	LogPrint("End Time: %s %s\n", DateBuf, TimeBuf);

	// return memory to heap
	Client->ClearXboxes();
	delete Client;
}



