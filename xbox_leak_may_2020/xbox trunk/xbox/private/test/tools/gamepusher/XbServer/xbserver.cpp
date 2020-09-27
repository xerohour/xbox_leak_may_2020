/*************************************************************************************************

  Copyright (c) 2001 Microsoft Corporation

  Module Name:

	xbserver.cpp

  Abstract:

	Defines the GamePusher Xbox "server" side functions which allow file data and information to be
	received from a broadcasting PC.

  Author:

	Mabel Santos (t-msanto) 03-Dec-2001

  Revision History:

*************************************************************************************************/

#include "xbserver.h"

XbServer* Server;
int DebugOn;

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

	Default XbServer constructor.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

XbServer::XbServer(void)
{
	GraphicPrint("Creating XbServer object\n");

	// flash LED green to indicate that app has started
	WriteSMC(0x08, 0x0A);
	WriteSMC(0x07, 0x01);
	
	// set socket to invalid
	ServerSock = INVALID_SOCKET;
	DebugPrint("Set socket to invalid\n");

	// set WinSock flag to false
	WsaInitialized = false;
	DebugPrint("Set WinSock flag to false\n");

	// set error flag to 0
	ErrorFlag = 0;
	DebugPrint("Set error flag to 0\n");

	// set exit flag to false
	ExitFlag = false;
	DebugPrint("Set exit flag to false\n");

	// set byte counters to 0
	TotalBytes = 0;
	RecvdBytes = 0;
	DebugPrint("Set byte counters to 0\n");

	// set packet-tracking variables to 0
	BasePkt = 0;
	NumPkts = 0;
	RecvdPkts = 0;
	DebugPrint("Set packet-tracking variables to 0\n");

	// set filestream to null
	OutFile = NULL;
	DebugPrint("Set filestream to null\n");
	
	// set name buffer contents to null
	memset(NameBuf, '\0', sizeof(NameBuf));
	DebugPrint("Set name buffer contents to null\n");
}

/*************************************************************************************************

Routine Description:

	Default XbServer destructor.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

XbServer::~XbServer(void)
{
	GraphicPrint("Deleting XbServer object\n");

	// close socket and clean up if needed
	if(ServerSock != INVALID_SOCKET)
	{
		closesocket(ServerSock);
	}

	if(WsaInitialized)
	{
		WSACleanup();
	}
}

/*************************************************************************************************

Routine Description:

	PrepareWinSock

	Initializes WinSock.  Creates a UDP socket for listening on port 2000 and binds the Xbox
	server to it.

Arguments:

	none

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
XbServer::PrepareWinSock(void)
{
	GraphicPrint("Preparing WinSock\n");

	// initialize WinSock
	WORD Version = MAKEWORD(2,2);
	WSADATA WsaData;
	WSAStartup(Version, &WsaData);
	WsaInitialized = true;
	DebugPrint("Initialized WinSock\n");

	// create a UDP datagram socket
	ServerSock = socket(AF_INET, SOCK_DGRAM, 0);
	DebugPrint("Created UDP datagram socket\n");
	
	if(ServerSock == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) // 10093
	{
		WSAStartup(Version, &WsaData);
		WsaInitialized = true;
		DebugPrint("Initialized WinSock a second time\n");
		ServerSock = socket(AF_INET, SOCK_DGRAM, 0);
		DebugPrint("Created UDP datagram socket a second time\n");
	}

	// try to increase receive buffer size
	int Size, BufSize = 0;
	int Err, Res;
	
	// start with 64K, then cut buffer size in half if necessary until request is granted
	for(Size = 65536; Size > 16384; Size >>=1)
	{
		// if got a socket error, try again with a smaller size
		if(setsockopt(ServerSock, SOL_SOCKET, SO_RCVBUF, (char*)&Size, sizeof(int)) == SOCKET_ERROR)
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
	getsockopt(ServerSock, SOL_SOCKET, SO_RCVBUF, (char*)&BufSize, &Res);
	DebugPrint("Receive buffer size = %d bytes\n", BufSize);

	// fill in address structure
	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(PORT);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	DebugPrint("Filled in address structure\n");

	// bind name to socket
	if(bind(ServerSock, (SOCKADDR*)&ServerAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		DebugPrint("SOCKET ERROR in PrepareWinSock(): bind()\n");
		OutputDebugString("SOCKET ERROR in PrepareWinSock(): bind()\n");
		return false;
	}
	else 
	{
		DebugPrint("Bound name to socket\n");
	}
	return true;
}

/*************************************************************************************************

Routine Description:

	MapDrives

	Creates symbolic links to areas on the hard disk so they can be accessed using drive letters.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void
XbServer::MapDrives(void)
{
	GraphicPrint("Mapping drives\n");

	DWORD Res;

	// map C drive
	Res = MapDrive('C', "\\Device\\Harddisk0\\Partition1");
	if(Res == 0)
	{
		DebugPrint("Mapped Drive C\n");
	}
	else
	{
		DebugPrint("Mapping drive C failed with error code %d\n", Res);
	}

	// map Y drive
	Res = MapDrive('Y', "\\Device\\Harddisk0\\Partition2");
	if(Res == 0)
	{
		DebugPrint("Mapped Drive Y\n");
	}
	else
	{
		DebugPrint("Mapping drive Y failed with error code %d\n", Res);
	}

	// create directories needed to map drives E and S
	CreateDirectory("C:\\devkit", NULL);
	CreateDirectory("C:\\TDATA", NULL);

	// map E drive
	Res = MapDrive('E', "\\Device\\Harddisk0\\Partition1\\devkit");
	if(Res == 0)
	{
		DebugPrint("Mapped Drive E\n");
	}
	else
	{
		DebugPrint("Mapping drive E failed with error code %d\n", Res);
	}

	// map S drive
	Res = MapDrive('S', "\\Device\\Harddisk0\\Partition1\\TDATA");
	if(Res == 0)
	{
		DebugPrint("Mapped Drive S\n");
	}
	else
	{
		DebugPrint("Mapping drive S failed with error code %d\n", Res);
	}
}

/*************************************************************************************************

Routine Description:

	SendReply

	Waits for a message and sends a response to let the PC know that this Xbox is in a listening
	state.

Arguments:

	none

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
XbServer::SendReply(void)
{
	char ReplyBuf[BUFFLEN];
	SOCKADDR_IN DestAddr;
	int Result;
	int AddrSize = sizeof(SOCKADDR_IN);
	DebugPrint("Waiting for message from GamePusher\n");
	GraphicPrint("Waiting for message from GamePusher\n");

	TIMEVAL Timeout;
	FD_SET Bucket;

	do
	{ 
		Bucket.fd_count = 1;
		Bucket.fd_array[0] = ServerSock;
		Timeout.tv_sec = 5;
		Timeout.tv_usec = 0;

		// wait for message from client
		Result = select(0, &Bucket, NULL, NULL, &Timeout);
		
		if(Result == SOCKET_ERROR)
		{
			DebugPrint("SOCKET ERROR in select()\n");
			OutputDebugString("SOCKET ERROR in SendReply(): select()\n");
			return false;
		}
		
	} while(Result == 0); // try again if hit timeout

	// grab message
	Result = recvfrom(ServerSock, ReplyBuf, sizeof(ReplyBuf), 0, (SOCKADDR*)&DestAddr, &AddrSize);	  
	
	if(Result == SOCKET_ERROR)
	{
		DebugPrint("SOCKET ERROR in recvfrom()\n");
		OutputDebugString("SOCKET ERROR in SendReply(): recvfrom()\n");
		return false;
	}

	DebugPrint("Received message from GamePusher\n");
	GraphicPrint("Received message from GamePusher\n");

	if(ReplyBuf)
	{
		// if message contains total number of bytes to be received, store the value
		TotalBytes = strtoul(ReplyBuf, NULL, 10);
		GraphicPrint("Total bytes to be received: %u\n", TotalBytes);
	}
	
	// reply with machine name so client knows which Xbox responded
	DWORD Len = BUFFLEN;

	// keep track of machine name so we can rename the box later
	DmGetXboxName(NameBuf, &Len);
	DebugPrint("Got Xbox name: %s\n", NameBuf);
	GraphicPrint("Got Xbox name: %s\n", NameBuf);
	
	Result = sendto(ServerSock, NameBuf, strlen(NameBuf), 0, (SOCKADDR*)&DestAddr, sizeof(SOCKADDR));	

	if(Result == SOCKET_ERROR)
	{
		DebugPrint("SOCKET ERROR in recvfrom()\n");
		OutputDebugString("SOCKET ERROR in SendReply(): recvfrom()\n");
		return false;
	}

	DebugPrint("Sent response back to GamePusher\n");
	GraphicPrint("Sent response back to GamePusher\n");
	return true;
}

/*************************************************************************************************

Routine Description:

	RecvFiles

	Receives file data and commands through the network.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void
XbServer::RecvFiles(void)
{
	char RecvBuf[sizeof(FilePacketHeader) + BUFFLEN];
	FilePacket* RecvPkt = (FilePacket*)RecvBuf;
	SOCKADDR_IN SrcAddr;
	int Result;
	int AddrSize = sizeof(SOCKADDR_IN);
	DebugPrint("Waiting to receive files\n");
	GraphicPrint("Waiting to receive files\n");

	TIMEVAL Timeout;
	FD_SET Bucket;

	while(!ErrorFlag && !ExitFlag) // keep going as long as error condition hasn't occurred
								   // and there is still more data to be received
	{
		Bucket.fd_count = 1;
		Bucket.fd_array[0] = ServerSock;
		Timeout.tv_sec = 5;
		Timeout.tv_usec = 0;

		// clear buffer
		memset(RecvBuf, '\0', sizeof(RecvBuf));

		// wait for data packet from client
		Result = select(0, &Bucket, NULL, NULL, &Timeout);

		if(Result == 0) continue; // timed out so try again
		
		if(Result == SOCKET_ERROR)
		{
			DebugPrint("SOCKET ERROR in select()\n");
			OutputDebugString("SOCKET ERROR in RecvFiles(): select()\n");
			break;
		}

		// grab data packet
		Result = recvfrom(ServerSock, RecvBuf, sizeof(RecvBuf), 0, (SOCKADDR*)&SrcAddr, &AddrSize);	  
		
		if(Result == 0)
		{
			// no more data
			DebugPrint("No more data. Breaking out of loop...\n");
			break;
		}

		if(Result == SOCKET_ERROR)
		{
			DebugPrint("SOCKET ERROR in recvfrom()\n");
			OutputDebugString("SOCKET ERROR in RecvFiles(): recvfrom()\n");
			break;
		}

		// process data packet
		ReadPacket(RecvPkt);
	}

	// check if we broke out of loop prematurely because of an error condition
	if(ErrorFlag)
	{
		// if so, flash the LED
		FlashLED();
	}
}

/*************************************************************************************************

Routine Description:

	ReadPacket

	Reads a data packet and responds according to its packet type.
	FILEDESC: create directories if needed and start new file
	FILEDATA: write file data to hard drive
	FILEDONE: close filestream and check if all packets have been received

Arguments:

	FilePacket* Packet	-	packet to be read
							
Return Value:

	none

*************************************************************************************************/

void
XbServer::ReadPacket(FilePacket* Packet)
{
	// display packet contents
	//DebugPrint("PacketType: %d\n", Packet->PacketType);
	//DebugPrint("SequenceNo: %d\n", Packet->SequenceNo);
	//DebugPrint("DataSize: %d\n", Packet->DataSize);
	//DebugPrint("Crc: %d\n", Packet->Crc);
	//if(Packet->Data) DebugPrint("Data: %s\n", Packet->Data);

	// if CRC does not match, set flag and return
	int Crc = GetCRC((unsigned char*)Packet->Data, Packet->DataSize);
	if(Crc != Packet->Crc)
	{
		DebugPrint("ERROR: GetCRC(Packet->Data, Packet->DataSize)(%d) != Packet->Crc(%d)\n", Crc, Packet->Crc);
		DebugPrint("Setting error flag to indicate checksum error...\n");
		ErrorFlag = CHKSMERROR;
		OutputDebugString("ERROR in ReadPacket(): CRCs do not match\n");
		return;
	}

	switch(Packet->PacketType)
	{
		case FILEDONE: // packet contains finish message

			// check if all previous packets have been received
			if(!CheckPkts()) return;

			// set exit flag to indicate that we're done
			ExitFlag = true;
			
			// unmap drives
			UnMapDrives();
			break;

		case FILEDESC: // packet contains file description
			
			// check if all previous packets have been received
			if(!CheckPkts()) return;

			// create any directories in the file path that do not exist
			// if there is a problem with creating a directory, set flag and return
			if(!CreateDirs(Packet->Data)) return;

			// open new filestream
			OutFile = fopen(Packet->Data, "wb");

			// if there is a problem with opening the file, set flag and return
			if(!OutFile)
			{
				DebugPrint("ERROR: Could not start new file %s\n", Packet->Data);
				DebugPrint("Setting error flag to indicate file error...\n");
				ErrorFlag = FILEERROR;
				OutputDebugString("ERROR in ReadPacket(): Could not start new file\n");
				return;	
			}

			// reaching here means file was opened successfully
			DebugPrint("Started new file: %s\n", Packet->Data);
			//GraphicPrint("%s\n", Packet->Data);

			// reset packet-tracking variables
			StartFile(Packet->FileSize, Packet->SequenceNo);
			break;
			
		case FILEDATA: // packet contains file data

			// check if packet belongs to the same file
			DWORD Offset = Packet->SequenceNo - BasePkt - 1;
			//DebugPrint("Offset: %d\n", Offset);
			if(Offset >= NumPkts)
			{
				// if packet is from another file, set error flag and return
				DebugPrint("ERROR: Packet belongs to another file\n");
				DebugPrint("Setting error flag to indicate packet error...\n");
				ErrorFlag = PKTERROR;
				OutputDebugString("ERROR in ReadPacket(): Packet belongs to another file\n");
				return;
			}

			// write file data to hard drive
			// set file position according to packet's sequence number
			fseek(OutFile, Offset*BUFFLEN, SEEK_SET);
			size_t WriteRes = fwrite(Packet->Data, sizeof(char), Packet->DataSize, OutFile);

			// if there is a problem with writing to the file, set flag and return
			if(WriteRes < Packet->DataSize) 
			{
				DebugPrint("ERROR: Could not write data to file\n");
				DebugPrint("Setting error flag to indicate file error...\n");
				ErrorFlag = FILEERROR;
				OutputDebugString("ERROR in ReadPacket(): Could not write data to file\n");
				return;
			}
			// reaching here means data was properly written
			//DebugPrint("Wrote %dB of data to file\n", WriteRes);
			//DebugPrint("Wrote %dB of data to file: %s\n", WriteRes, Packet->Data);

			// increment received byte counter
			RecvdBytes += Packet->DataSize;

			if(TotalBytes > 0)
				GraphicPrint("Receiving files: %.2f%% done\n", RecvdBytes/(TotalBytes*0.01));

			// increment received packet counter
			++RecvdPkts;
			//DebugPrint("Incremented received packet counter to %d\n", RecvdPkts);
			break;
	}
}

/*************************************************************************************************

Routine Description:

	CheckPkts

	Checks if all data packets for the most recent file have been received.

Arguments:

	none

Return Value:

	BOOL	-	true if all packets have been received; false otherwise

*************************************************************************************************/

BOOL
XbServer::CheckPkts(void)
{
	// if there is an open filestream, close it
	if(OutFile) 
	{
		fclose(OutFile);
		OutFile = NULL;
	}

    // check if all packets for the previous file have been received
    if(RecvdPkts != NumPkts)
    {
		DebugPrint("ERROR: Not all packets received for previous file\n");
		DebugPrint("Setting error flag to indicate packet error...\n");
		ErrorFlag = PKTERROR;
		OutputDebugString("ERROR in CheckPkts(): Not all packets received for previous file\n");
		return false;
    }

	// reaching here means all packets have been received
	DebugPrint("Done receiving file\n");
	//GraphicPrint("Done receiving file\n");
	return true;
}

/*************************************************************************************************

Routine Description:

	StartFile

	Prepares to receive a new file by resetting all packet-tracking variables.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void
XbServer::StartFile(DWORD FileSize, DWORD PktNum)
{
	BasePkt = PktNum;
	DebugPrint("BasePkt: %d\n", BasePkt);
	DebugPrint("FileSize: %d bytes\n", FileSize);
	NumPkts = FileSize/BUFFLEN + (FileSize%BUFFLEN > 0 ? 1 : 0);
	DebugPrint("NumPkts: %d\n", NumPkts);
	RecvdPkts = 0;
	//DebugPrint("RecvdPkts: %d\n", RecvdPkts);
}

/*************************************************************************************************

Routine Description:

	CreateDirs

	Steps through a given path and creates any directories that do not already exist.

Arguments:

	char* PathName	-	path to be traversed

Return Value:

	BOOL	-	true if no errors are encountered; false otherwise

*************************************************************************************************/

BOOL
XbServer::CreateDirs(char* PathName)
{
	unsigned int Start, End;
	
	// step through each directory in the path
	for(Start = 0, End = strcspn(PathName, "\\") + 1;
		End < strlen(PathName);
		Start = End, End = strcspn(PathName+Start, "\\") + Start + 1)
	{
		char Temp = PathName[End];
		PathName[End] = '\0';
		DebugPrint("CurrDir: %s\n", PathName);
		
		// try to create directory
		if(!CreateDirectory(PathName, NULL))
		{	
			// if directory does not exist but can not be created, set error flag and return
			if(GetLastError() != ERROR_ALREADY_EXISTS)
			{
				DebugPrint("ERROR: Could not create directory %s\n", PathName);
				ErrorFlag = DIRERROR;
				OutputDebugString("ERROR in CreateDirs(): Could not create directory\n");
				return false;
			}
			else // directory already exists, so no harm done
			{
				//DebugPrint("Directory %s already exists\n", PathName);
			}
		}
		else
		{
			// reaching here means directory did not exist and was created successfully
			DebugPrint("Created new directory: %s\n", PathName);
			//GraphicPrint("Created new directory: %s\n", PathName);
		}
		PathName[End] = Temp;
	}
	return true;
}

/*************************************************************************************************

Routine Description:

	UnMapDrives

	Removes symbolic links to areas on the hard disk that allow them to be accessed using drive
	letters.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void
XbServer::UnMapDrives(void)
{
	GraphicPrint("Unmapping drives\n");

	DWORD Res;

	// unmap C drive
	Res = UnMapDrive('C');
	if(Res == 0)
	{
		DebugPrint("Unmapped Drive C\n");
	}
	else
	{
		DebugPrint("Unmapping drive C failed with error code %d\n", Res);
	}
	
	// unmap Y drive
	Res = UnMapDrive('Y');
	if(Res == 0)
	{
		DebugPrint("Unmapped Drive Y\n");
	}
	else
	{
		DebugPrint("Unmapping drive Y failed with error code %d\n", Res);
	}

	// unmap E drive
	Res = UnMapDrive('E');
	if(Res == 0)
	{
		DebugPrint("Unmapped Drive E\n");
	}
	else
	{
		DebugPrint("Unmapping drive E failed with error code %d\n", Res);
	}

	// unmap S drive
	Res = UnMapDrive('S');
	if(Res == 0)
	{
		DebugPrint("Unmapped Drive S\n");
	}
	else
	{
		DebugPrint("Unmapping drive S failed with error code %d\n", Res);
	}
}

/*************************************************************************************************

Routine Description:

	FlashLED

	Makes the Xbox LED flash red to indicate that an error has occurred.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void
XbServer::FlashLED(void)
{
	DebugPrint("Error encountered:\n");
	OutputDebugString("Error encountered:\n");

	switch(ErrorFlag)
	{

	case PKTERROR:
		DebugPrint("Packet missing or out of order\n");
		OutputDebugString("Packet missing or out of order\n");
		GraphicPrint("Packet missing or out of order\n");
		// flash LED red
		//WriteSMC(0x08, 0xA0);
		//WriteSMC(0x07, 0x01);
		break;
	
	case CHKSMERROR:
		DebugPrint("Packet CRCs do not match\n");
		OutputDebugString("Packet CRCs do not match\n");
		GraphicPrint("Packet CRCs do not match\n");
		// flash LED orange
		//WriteSMC(0x08, 0xAA);
		//WriteSMC(0x07, 0x01);
		break;

	case DIRERROR:
		DebugPrint("Directory not created\n");
		OutputDebugString("Directory not created\n");
		GraphicPrint("Directory not created\n");
		// flash LED green
		//WriteSMC(0x08, 0x0A);
		//WriteSMC(0x07, 0x01);
		break;

	case FILEERROR:
		DebugPrint("File not written\n");
		OutputDebugString("File not written\n");
		GraphicPrint("File not written\n");
		// flash LED red and orange
		//WriteSMC(0x08, 0xF5);
		//WriteSMC(0x07, 0x01);
		break;

	case KRNLERROR:
		DebugPrint("Kernel not received\n");
		OutputDebugString("Kernel not received\n");
		GraphicPrint("Kernel not received\n");
		// flash LED orange and green
		//WriteSMC(0x08, 0xAF);
		//WriteSMC(0x07, 0x01);
		break;

	case FLASHERROR:
		DebugPrint("ROM not flashed\n");
		OutputDebugString("ROM not flashed\n");
		GraphicPrint("ROM not flashed\n");
		// flash LED red and green
		//WriteSMC(0x08, 0xA5);
		//WriteSMC(0x07, 0x01);
		break;

	default:
		DebugPrint("Socket error, format error or flash error\n");
		OutputDebugString("Socket error, format error or flash error\n");
		GraphicPrint("Socket error, format error or flash error\n");
		break;
	}
	// flash LED red to indicate that an error has been encountered
	WriteSMC(0x08, 0xA0);
	WriteSMC(0x07, 0x01);
	UnMapDrives();
	Sleep(INFINITE);
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
XbServer::GetCRC(unsigned char* Data, WORD Size)
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

	DebugPrint

	Prints a line to standard output if the DebugOn flag is set.

Arguments:

	char* ptszFormat, ...	-	format string	

Return Value:

	none

*************************************************************************************************/

void
DebugPrint(char *ptszFormat, ...)
{
	if(DebugOn)
	{
		char tszBuf[BUFFLEN];
		va_list arglist;

		va_start( arglist, ptszFormat );
		_vsnprintf( tszBuf, sizeof(tszBuf), ptszFormat, arglist );
		OutputDebugStringA( tszBuf );
		va_end(arglist);
	}
}

/*************************************************************************************************

Routine Description:

	main function

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

void main(void)
{	
	DebugOn = 0;

	InitGraphics();

	XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY, 255};
    int nXnetResult = XNetStartup( &xnsp );
    if(nXnetResult != NO_ERROR)
    {
		DebugPrint("XNetStartup() Error: %u\n", nXnetResult);
		GraphicPrint("XNetStartup() Error: %u\n", nXnetResult);
    }
	// sleep for 5 secs because sockets are not fully initialized immediately after call to
	// XNetStartup()
    Sleep(5000);

	// make an XbServer object
	Server = new XbServer;

	// set up network stuff
	if(!Server->PrepareWinSock())
	{
		Server->FlashLED();
		Sleep(INFINITE);
	}

	// map drives
	Server->MapDrives();

	// notify client that this box is listening
	if(!Server->SendReply())
	{
		Server->FlashLED();
		Sleep(INFINITE);
	}

	// wait to receive files from network
	Server->RecvFiles();

	// notify client that this box is done receiving files
	if(!Server->SendReply())
	{
		Server->FlashLED();
		Sleep(INFINITE);
	}

	// delete XbServer object
	delete Server;

	GraphicPrint("Done receiving files. Waiting for reboot...\n");
	Sleep(INFINITE);
}