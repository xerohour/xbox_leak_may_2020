/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "threadpool.h"
#include "connection.h"
#include "transmit.h"
#include "receive.h"
#include "tickles.h"
#include "config.h"

enum EVENTENUM{
	EVENTENUM_TRANSMITSTOP=1,
	EVENTENUM_TRANSMITSTOPPED=2,
	EVENTENUM_TRANSMITREADY=3,
	EVENTENUM_TRANSMITSTART=4,
	EVENTENUM_TICKLESTOP=5,
	EVENTENUM_TICKLESTOPPED=6,
	EVENTENUM_TICKLEREADY=7,
	EVENTENUM_TICKLESTART=8,
	EVENTENUM_RECEIVESTOP=9,
	EVENTENUM_RECEIVESTOPPED=10,
	EVENTENUM_RECEIVEREADY=11,
	EVENTENUM_RECEIVESTART=12,
	EVENTENUM_CONTROLLOCKS=13,
	EVENTENUM_ENDENUM=14
};//endenum

/////////////////////////////////////////////////////////////////////////////
// ShowUsage
/////////////////////////////////////////////////////////////////////////////

void ShowUsage(int argc, char *argv[]){
	char* pstr=strrchr(argv[0],'\\');
	if ((NULL==pstr)||('\0'==pstr)){
		pstr=argv[0];
	}else{
		pstr++;
	}//endif
	printf("usage: %s -s servername [-c #] [-u #] [-d #] [-t #] -f configfile\n",pstr);
	printf("  -s servername (server name or ip address) [required]\n");
	printf("  -c # (number of connections to server) [default: %d]\n",default_channels);
	printf("  -u # (number of unique users to simulate >=1000) [default: %d]\n",default_users);
	printf("  -d # (max number of destinations a single add can have) [default: %d]\n",default_destinations);
	printf("  -m # (minutes to execute) [default: %d]\n",default_runtime);
	printf("  -l # (max seconds a client can live) [default: %d]\n",default_maxttl);
	printf("  -t {yes|no} (tickles control actions; not internal state) [default:");
	if (TRUE==default_ticklecontrol){
		printf("yes]\n");
	}else{
		printf("no]\n");
	}//endif
	printf("  -f configfile (server configuration file) [required]\n\n");
	printf("examples:\n");
	printf("%s -s cutthroat -f c:\\xqserver\\xqserver.cfg\n",pstr);
	printf("%s -s chumdinger -c 5 -u 100000 -d 25 -m 2 -l 1800 -t yes -f c:\\xqserver\\xqserver.cfg\n\n",pstr);
}//endmethod

/////////////////////////////////////////////////////////////////////////////
// ParseCommandLine
/////////////////////////////////////////////////////////////////////////////

HRESULT ParseCommandLine(int argc, char *argv[ ], Configuration* pConfig){

	BOOL fRequiredServerName=FALSE;
	BOOL fRequiredConfigFile=FALSE;
	UINT uiCurrIndex=1;

	//Check for minimal set of arguements and that we have pairing
	if ((argc<5)||((argc-1)%2!=0)){
		goto showusage;
	}//endif
	
	//Parse arguement when we still have some left
	while (uiCurrIndex<argc){

		//Is this a valid format for a switch?
		if ((2>strlen(argv[uiCurrIndex])) || ('-' != argv[uiCurrIndex][0])){
			goto showusage;
		}//endif

		//Parse the switch
		switch(argv[uiCurrIndex][1]){
			case 's':
			case 'S':
				uiCurrIndex++;
				strcpy(pConfig->pszServerName,argv[uiCurrIndex]);
				fRequiredServerName=TRUE;
			break;
			case 'f':
			case 'F':
				uiCurrIndex++;
				strcpy(pConfig->pszConfigFile,argv[uiCurrIndex]);
				fRequiredConfigFile=TRUE;
			break;
			case 'c':
			case 'C':
				uiCurrIndex++;
				pConfig->dwChannels=atol(argv[uiCurrIndex]);
			break;
			case 'u':
			case 'U':
				uiCurrIndex++;
				pConfig->dwUsers=atol(argv[uiCurrIndex]);
				if (pConfig->dwUsers<1000){
					printf("Unique users must be >= 1000\n");
					goto showusage;
				}//endif
			break;
			case 'd':
			case 'D':
				uiCurrIndex++;
				pConfig->dwDestinations=atol(argv[uiCurrIndex]);
			break;
			case 'm':
			case 'M':
				uiCurrIndex++;
				pConfig->dwRuntime=atol(argv[uiCurrIndex]);
			break;
			case 'l':
			case 'L':
				uiCurrIndex++;
				pConfig->dwMaxLifetime=atol(argv[uiCurrIndex]);
			break;
			case 't':
			case 'T':
				uiCurrIndex++;
				if (0==_stricmp(argv[uiCurrIndex],"yes")){
					pConfig->fTickleControl=TRUE;
				}else{
					pConfig->fTickleControl=FALSE;
				}//endif
			break;
			default:
				goto showusage;
		}//endswitch
		uiCurrIndex++;

	}//endwhie

	//Do we have required parameters
	if ((FALSE==fRequiredServerName)||(FALSE==fRequiredConfigFile)){
		goto showusage;
	}//endif

	printf("Configuration:\n");
	printf("server: %s\n",pConfig->pszServerName);
	printf("config file: %s\n",pConfig->pszConfigFile);
	printf("unique users: %u\n",pConfig->dwUsers);
	printf("connections: %u\n",pConfig->dwChannels);
	printf("max destinations per add: %u\n",pConfig->dwDestinations);
	printf("runtime (in minutes): %u\n",pConfig->dwRuntime);
	printf("tickles control actions: ");
	if (TRUE==pConfig->fTickleControl){
		printf("yes\n");
	}else{
		printf("no\n");
	}//endif
	printf("client session time to live (in seconds): %u\n",pConfig->dwMaxLifetime);
	printf("\n");

	return S_OK;

showusage:
	ShowUsage(argc,argv);
	return E_INVALIDARG;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// LoadConfigFile
/////////////////////////////////////////////////////////////////////////////

HRESULT LoadConfigFile(Configuration* pConfig){
	const UINT uiSize=256;
	TCHAR rgchBuffer[uiSize];

	//Get the port number
	if (0==GetPrivateProfileString(_TEXT("Global"),_TEXT("PortNum"),_TEXT(""),rgchBuffer,uiSize,pConfig->pszConfigFile)){
		return E_FAIL;
	}//endif

	//Get the number of queues
	pConfig->dwPort=atol(rgchBuffer);
	if (0==GetPrivateProfileString(_TEXT("Global"),_TEXT("NumQueueTypes"),_TEXT(""),rgchBuffer,uiSize,pConfig->pszConfigFile)){
		return E_FAIL;
	}//endif
	pConfig->dwQueues=atol(rgchBuffer);
	if (0>=pConfig->dwQueues){
		return E_FAIL;
	}//endif

	//Read information about each queue type
	pConfig->rgdwMaxSize=new DWORD[pConfig->dwQueues];
	pConfig->rgdwMinSize=new DWORD[pConfig->dwQueues];
	pConfig->rgfDuplicates=new BOOL[pConfig->dwQueues];
	TCHAR rgchSection[50];
	for (DWORD dwi=0; dwi<pConfig->dwQueues; dwi++){
		_stprintf(rgchSection,"Queue Type %u",dwi);
		if (0==GetPrivateProfileString(rgchSection,_TEXT("MinDataSize"),_TEXT(""),rgchBuffer,uiSize,pConfig->pszConfigFile)){
			return E_FAIL;
		}//endif
		pConfig->rgdwMinSize[dwi]=atol(rgchBuffer);
		if (0==GetPrivateProfileString(rgchSection,_TEXT("MaxDataSize"),_TEXT(""),rgchBuffer,uiSize,pConfig->pszConfigFile)){
			return E_FAIL;
		}//endif
		pConfig->rgdwMaxSize[dwi]=atol(rgchBuffer);
		if (0==GetPrivateProfileString(rgchSection,_TEXT("AllowDuplicates"),_TEXT(""),rgchBuffer,uiSize,pConfig->pszConfigFile)){
			return E_FAIL;
		}//endif
		if (0==_tcsicmp(rgchBuffer,"yes")){
			pConfig->rgfDuplicates[dwi]=TRUE;
		}else{
			pConfig->rgfDuplicates[dwi]=FALSE;
		}//endif
	}//endfor

	return S_OK;
}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Entry point
/////////////////////////////////////////////////////////////////////////////

void __cdecl main(int argc, char *argv[ ], char *envp[ ]){

	//Send memory reports to debug window and console
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT);

	//Send asserts to dialog
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW);

	//Allocate configuration object
	Configuration* pConfig = NULL;
	pConfig=new Configuration();
	_ASSERT(NULL!=pConfig);
	if (NULL==pConfig){
		return;
	}//endif

	//Parse command line parameters
	if FAILED(ParseCommandLine(argc,argv,pConfig)){
		return;
	}//endif

	//Load configuration file parameters
	if FAILED(LoadConfigFile(pConfig)){
		return;
	}//endif

	//Bring winsock up
	WinsockUP();

	//Initializes the COM library for use by the calling thread and sets the thread's concurrency model.
	if FAILED(CoInitializeEx(NULL,COINIT_MULTITHREADED)){
		return;
	}//endif

	//Create synchronization events
	HANDLE rghEvents[EVENTENUM_ENDENUM];
	for (DWORD dwi=0; dwi<EVENTENUM_ENDENUM;dwi++){
		rghEvents[dwi]=CreateEvent(NULL,TRUE,FALSE,NULL);
	}//endfor

	//Allocate control object
	Control* pControl=NULL;
	pControl=new Control(pConfig->dwUsers,pConfig->dwQueues,rghEvents[EVENTENUM_CONTROLLOCKS]);
	_ASSERT(NULL!=pControl);
	if (NULL==pControl){
		return;
	}//endif
	
	//Allocate connection channels
	Channels* pChannels=NULL;
	pChannels=new Channels(pConfig->dwChannels);
	_ASSERT(NULL!=pChannels);
	if (NULL==pChannels){
		return;
	}//endif

	//Allocate transmit thread parameters
	TransmitThreadFuncParams* pTransmitThreadParams;
	pTransmitThreadParams=new TransmitThreadFuncParams;
	pTransmitThreadParams->hStop=rghEvents[EVENTENUM_TRANSMITSTOP];
	pTransmitThreadParams->hStopped=rghEvents[EVENTENUM_TRANSMITSTOPPED];
	pTransmitThreadParams->hReady=rghEvents[EVENTENUM_TRANSMITREADY];
	pTransmitThreadParams->hStart=rghEvents[EVENTENUM_TRANSMITSTART];
	pTransmitThreadParams->pControl=pControl;
	pTransmitThreadParams->pChannels=pChannels;
	pTransmitThreadParams->pConfig=pConfig;

	//Create transmit thread
	DWORD dwTransmitThreadId=0L;
	HANDLE hTransmitThread=NULL;	
	hTransmitThread=CreateThread(NULL,0L,TransmitThreadFunc,pTransmitThreadParams,0L,&dwTransmitThreadId);
	_ASSERT(hTransmitThread!=NULL);
	if (NULL==hTransmitThread){
		return;
	}//endif

	//Allocate tickle thread parameters
	TickleThreadFuncParams* pTickleThreadParams;
	pTickleThreadParams=new TickleThreadFuncParams;
	pTickleThreadParams->hStop=rghEvents[EVENTENUM_TICKLESTOP];
	pTickleThreadParams->hStopped=rghEvents[EVENTENUM_TICKLESTOPPED];
	pTickleThreadParams->hReady=rghEvents[EVENTENUM_TICKLEREADY];
	pTickleThreadParams->hStart=rghEvents[EVENTENUM_TICKLESTART];
	pTickleThreadParams->pControl=pControl;

	//Create tickle processing thread
	DWORD dwTickleThreadId=0L;
	HANDLE hTickleThread=NULL;
	hTickleThread=CreateThread(NULL,0L,TickleThreadFunc,pTickleThreadParams,0L,&dwTickleThreadId);
	_ASSERT(hTickleThread!=NULL);
	if (NULL==hTickleThread){
		return;
	}//endif

	//Allocate receive thread parameters
	ReceiveThreadFuncParams* pReceiveThreadParams;
	pReceiveThreadParams=new ReceiveThreadFuncParams;
	pReceiveThreadParams->hStop=rghEvents[EVENTENUM_RECEIVESTOP];
	pReceiveThreadParams->hStopped=rghEvents[EVENTENUM_RECEIVESTOPPED];
	pReceiveThreadParams->hReady=rghEvents[EVENTENUM_RECEIVEREADY];
	pReceiveThreadParams->hStart=rghEvents[EVENTENUM_RECEIVESTART];
	pReceiveThreadParams->pControl=pControl;
	pReceiveThreadParams->pChannels=pChannels;

	//Create receive processing thread
	DWORD dwReceiveThreadId=0L;
	HANDLE hReceiveThread=NULL;
	hReceiveThread=CreateThread(NULL,0L,ReceiveThreadFunc,pReceiveThreadParams,0L,&dwReceiveThreadId);
	_ASSERT(hReceiveThread!=NULL);
	if (NULL==hReceiveThread){
		return;
	}//endif

	//Wait till threads are running
	HANDLE rghWaitEvents[3];
	rghWaitEvents[0]=rghEvents[EVENTENUM_TICKLEREADY];
	rghWaitEvents[1]=rghEvents[EVENTENUM_TRANSMITREADY];
	rghWaitEvents[2]=rghEvents[EVENTENUM_RECEIVEREADY];
	if (WAIT_TIMEOUT==WaitForMultipleObjects(3,rghWaitEvents,TRUE,10000)){
		return;
	}//endif

	//Unlease the power
	SetEvent(rghEvents[EVENTENUM_TICKLESTART]);
	SetEvent(rghEvents[EVENTENUM_TRANSMITSTART]);
	SetEvent(rghEvents[EVENTENUM_RECEIVESTART]);

	//Run for timeframe
	Sleep(60000*pConfig->dwRuntime);

	//Execute staged shutdown
	SetEvent(rghEvents[EVENTENUM_TRANSMITSTOP]); //Signal transmit thread shutdown
	WaitForSingleObject(rghEvents[EVENTENUM_TRANSMITSTOPPED],5000);  //Wait for send buffers to flush
	Sleep(5000);  //Give extra time to process last few incoming commands
	SetEvent(rghEvents[EVENTENUM_TICKLESTOP]); //Signal tickle thread shutdown
	SetEvent(rghEvents[EVENTENUM_RECEIVESTOP]); //Signal receive thread shutdown
	SetEvent(rghEvents[EVENTENUM_CONTROLLOCKS]);  //Break any bad locks

	//Break any defunct channel connections
	for (UINT ui=0; ui<pConfig->dwChannels; ui++){
		pChannels->m_rgChannels[ui].pEngineEx->SetShutdownEvent();
	}//endfor

	//Reclaim receive thread
	if (WAIT_TIMEOUT==WaitForSingleObject(rghEvents[EVENTENUM_RECEIVESTOPPED],10000)){
		TerminateThread(hReceiveThread,-999);
	}//endfor
	CloseHandle(hReceiveThread);
	hReceiveThread=NULL;

	//Deallocate receive thread parameters
	if (NULL!=pReceiveThreadParams){
		delete pReceiveThreadParams;
		pReceiveThreadParams;
	}//endif

	//Reclaim tickle thread
	if (WAIT_TIMEOUT==WaitForSingleObject(rghEvents[EVENTENUM_TICKLESTOPPED],10000)){
		TerminateThread(hTickleThread,-999);
	}//endfor
	CloseHandle(hTickleThread);
	hTickleThread=NULL;

	//Deallocate tickle thread parameters
	if (NULL!=pTickleThreadParams){
		delete pTickleThreadParams;
		pTickleThreadParams;
	}//endif

	//Reclaim transmit thread
	if (WAIT_TIMEOUT==WaitForSingleObject(rghEvents[EVENTENUM_TRANSMITSTOPPED],10000)){
		TerminateThread(hTransmitThread,-999);
	}//endfor
	CloseHandle(hTransmitThread);
	hTransmitThread=NULL;

	//Deallocate transmit thread parameters
	if (NULL!=pTransmitThreadParams){
		delete pTransmitThreadParams;
		pTransmitThreadParams;
	}//endif

	//Deallocate connection channels
	if (NULL!=pChannels){
		delete pChannels;
		pChannels=NULL;
	}//endif

	//Deallocate control object
	if (NULL!=pControl){
		delete pControl;
		pControl=NULL;
	}//endif

	//Close synchronization events
	for (dwi=0; dwi<EVENTENUM_ENDENUM;dwi++){
		if (NULL!=rghEvents[dwi]){
			CloseHandle(rghEvents[dwi]);
			rghEvents[dwi]=NULL;
		}//endif
	}//endfor

	//Deallocate configuration object
	if (NULL!=pConfig){
		delete pConfig;
		pConfig=NULL;
	}//endif 

	//Closes the COM library on the current thread.
	CoUninitialize();

	//Bring winsock down
	WinsockDOWN();

	//Detect memory leaks
	_CrtDumpMemoryLeaks( );

}//endmain
