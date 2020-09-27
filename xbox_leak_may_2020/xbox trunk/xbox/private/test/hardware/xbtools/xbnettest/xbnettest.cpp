#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <XboxDbg.h>

#define BUFSIZE 1400

VOID
UsageFailure() {
	fprintf(stderr, "xbnettest\n Echos network data from the Xbox target system\n");
	fprintf(stderr, "\nusage: xbnettest address\n");
	fprintf(stderr, "\taddress = IP Address of target Xbox\n");
}

int
__cdecl
main(int argc, char* argv[]) {
	WSADATA			wsaData;
	SOCKET			sock;
	sockaddr_in		saXboxAddr, saLocalAddr, saFromAddr;
	u_short			sXboxPort;
	int				iRet, iBytesRecv, iBytesSent, iMsgs;
	char			cBuf[BUFSIZE];
	ULONG			ulXboxAddr;
	BYTE			*pb = (BYTE *)&ulXboxAddr;
	int				iSize;

	sXboxPort = IPPORT_RESERVED + 1;
	iBytesRecv = 0;
	iBytesSent = 0;

	// Get IP address from command line
	if(argc != 2) {
		// no IP address on command line
		UsageFailure();
		exit(1);
	}

	ulXboxAddr = inet_addr(argv[1]);
	printf("Xbox target system (%d.%d.%d.%d)\n", pb[0], pb[1], pb[2], pb[3]);

	// Start winsock
	if(WSAStartup( 0x202, &wsaData ) == SOCKET_ERROR) {
		fprintf(stderr,"WAStartup failed: error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	
	// Create UDP socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == INVALID_SOCKET) {
		fprintf(stderr,"socket() failed: error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Bind socket to local address
	saLocalAddr.sin_family = AF_INET;
	saLocalAddr.sin_addr.s_addr = INADDR_ANY;
	saLocalAddr.sin_port = htons(sXboxPort);

	iRet = bind(sock, (sockaddr *)&saLocalAddr, sizeof(saLocalAddr));
	if(iRet == SOCKET_ERROR) {
		fprintf(stderr,"bind() failed: error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Populate Xbox address
	saXboxAddr.sin_family = AF_INET;
	saXboxAddr.sin_addr.s_addr = ulXboxAddr;
	saXboxAddr.sin_port = htons(sXboxPort);

	// Do a sendto() to inform Xbox of my address
	iRet = sendto(sock, cBuf, BUFSIZE, 0,(sockaddr *)&saXboxAddr, sizeof(saXboxAddr));
	if((iRet == SOCKET_ERROR)) {
		fprintf(stderr,"sendto() failed: error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	if(iRet != BUFSIZE) {
		fprintf(stderr,"sendto() only sent %d bytes of %d\n", iRet, BUFSIZE);
		WSACleanup();
		return 1;
	}

	// Echo data
	iMsgs = 0;
	for(;;) {
		// Receive data
		iSize = sizeof(saFromAddr);
		iRet = recvfrom(sock, cBuf, BUFSIZE, 0,(sockaddr *)&saFromAddr, &iSize);
		if((iRet == SOCKET_ERROR)) {
			fprintf(stderr,"recvfrom() failed: error %d\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
		if(iRet == 0) {
			// Xbox done, quit
			fprintf(stdout,"recvfrom() 0 bytes, exiting\n");
			break;
		}
		iBytesRecv += iRet;


		// Send data back
		iRet = sendto(sock, cBuf, BUFSIZE, 0,(sockaddr *)&saXboxAddr, sizeof(saXboxAddr));
		if((iRet == SOCKET_ERROR)) {
			fprintf(stderr,"sendto() failed: error %d\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
		if(iRet != BUFSIZE) {
			fprintf(stderr,"sendto() only sent %d bytes of %d\n", iRet, BUFSIZE);
			WSACleanup();
			return 1;
		}
		iBytesSent += iRet;

		iMsgs++;
	}

	fprintf(stderr, "%d Messages Echoed Bytes Read = %d, Sent = %d\n", iMsgs, iBytesRecv, iBytesSent);

	closesocket(sock);
	WSACleanup();
	return 0;
}