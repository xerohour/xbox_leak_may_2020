/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"  //Precompiled header
#include "stdlib.h"	 //atol
#include "connection.h"  //Network connection wrapper and helper functions
#include "xqprotocol.h"  //Protocol structures

/////////////////////////////////////////////////////////////////////////////
// Usage - Displays the command line usage of this program.
/////////////////////////////////////////////////////////////////////////////

void Usage(char *argv[ ]){
	fprintf(stdout,"usage: %s -m machine [-p #] [-u #] [-q #] [-n #] [-?|h]\n",argv[0]);
	fprintf(stdout,"where:\n");
	fprintf(stdout,"  -m is the name or ip address of the target machine\n");
	fprintf(stdout,"  -p is the port to use\n");
	fprintf(stdout,"  -u # is the userid.\n");
	fprintf(stdout,"  -q # is the qtype.\n");
	fprintf(stdout,"  -n # is the number of items in the queue\n");
	fprintf(stdout,"\nexamples:\n");
	fprintf(stdout,"  %s -m 10.0.0.1 -p 1001 -u 1 -q 0 -n 5\n",argv[0]);
	fprintf(stdout,"  %s -m machinename -p 1001 -u 1 -q 0 -n 5\n",argv[0]);
}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Main block
/////////////////////////////////////////////////////////////////////////////

int __cdecl main(int argc, char *argv[ ]){

	//Local vars
	HRESULT hr=S_OK;
	SOCKADDR_IN sin;
	DWORD dwIP=INADDR_NONE;
	UDPConnection* pUdp;
	bool fPass=false;

	//Parameters
	QWORD qwUserID=MAKEQWORD(0,0);
	WORD wPort=0L;
	DWORD dwQType=0L;
	DWORD dwNumItems=0L;
	char rgchName[1024];

	//Check if there are any command line arguements and whether those arguements are divisible by 2
	int i=1;
	if ((argc<=1) || (((argc-1)%2)!=0) ) goto show_usage;
	
	//Parse the command line arguements
	fPass=false;
	while (i<argc){
		if (argv[i][0]!='-') goto show_usage;
		switch (argv[i++][1]){
		case 'm':
		case 'M':
			strcpy(rgchName,argv[i++]);;
			fPass=true;
			break;
		case 'u':
		case 'U':
			qwUserID=_atoi64(argv[i++]);
			break;
		case 'q':
		case 'Q':
			dwQType=atol(argv[i++]);
			break;
		case 'p':
		case 'P':
			wPort=(unsigned short)atoi(argv[i++]);
			break;
		case 'n':
		case 'N':
			dwNumItems=atol(argv[i++]);
			break;
		default:
			goto show_usage;
		}//endswitch
	}//endwhile

	if (!fPass) return -1;

	//Bring winsock up
	if FAILED(WinsockUP()){
		printf("Winsock won't initialize\n");
		return -1;
	}//endif

	//Resolve machine name
	dwIP=InternetAddress(rgchName);
	if (dwIP==INADDR_NONE){
		printf("Invalid machine name.\n");
		WinsockDOWN();
		return -1;
	}//endif

	//Build tickle
	Q_TICKLE_MSG tickle;
	tickle.qwUserID=qwUserID;
	tickle.dwQType=dwQType;
	tickle.dwQLength=dwNumItems;
	
	//Dump to bytes
	char rgchBuffer[sizeof(Q_TICKLE_MSG)];
	memcpy(rgchBuffer,&tickle,sizeof(Q_TICKLE_MSG));
	
	//Name connection
	sin.sin_family = AF_INET;
	sin.sin_port = htons(wPort);
	sin.sin_addr.s_addr = dwIP;
	
	//Send
	pUdp=new UDPConnection(NULL);
	pUdp->SendTo(rgchBuffer,sizeof(Q_TICKLE_MSG),TRUE,WSA_INFINITE,(const struct sockaddr*)&sin,sizeof(sin));
	delete pUdp;

	//Bring winsock down
	WinsockDOWN();

	printf("operation complete.\n");
	return 0;

show_usage:

	Usage(argv);
	return -1;

}//endmethod