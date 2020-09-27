/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "tickles.h"
#include "connection.h"

/////////////////////////////////////////////////////////////////////////////
// TickleThreadFunc
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI TickleThreadFunc(LPVOID pData){

	//Counter
	DWORD dwcTICKLEs=0L;

	//Verify parameters
	if (NULL==pData){
		return -1;
	}//endif

	//Cast thread parameters 
	TickleThreadFuncParams* pParams=(TickleThreadFuncParams*)pData;
	Control* pControl=pParams->pControl;
	HANDLE hStop=pParams->hStop;
	HANDLE hStopped=pParams->hStopped;
	HANDLE hReady=pParams->hReady;
	HANDLE hStart=pParams->hStart;

	//Create UDP connection
	DWORD dwReceiveBufferSize=pControl->GetUserCount()*sizeof(Q_TICKLE_MSG);
	if (dwReceiveBufferSize>1048576*10){
		dwReceiveBufferSize=1048576*10;
	}//endif
	UDPConnection TickleCon(hStop,1,256,dwReceiveBufferSize);
	TickleCon.Bind(TICKLEPORT);

	//Statically allocate space for tickles
	Q_TICKLE_MSG Tickle;
	ZeroMemory(&Tickle,sizeof(Q_TICKLE_MSG));

	DWORD dwBytesRead=0L;  //Number of bytes read when tickle was retrieved
	DWORD dwUserIndex=0L;  //User index that matches userid
	TickleInfo* pTickle=NULL;  //Pointer to Queue object
	HANDLE hLock=NULL;  //Lock event

	SetEvent(hReady);
	WaitForSingleObject(hStart,INFINITE);

	//While it is not time to shutdown perform actions
	while (WAIT_OBJECT_0!=WaitForSingleObject(hStop,0)){

		//Retrieve a tickle
		dwBytesRead=0L;
		TickleCon.ReceiveFrom((char*)&Tickle,sizeof(Q_TICKLE_MSG),&dwBytesRead);
		if (dwBytesRead<sizeof(Q_TICKLE_MSG)){
			continue;
		}//endif

		//Update tickle information
		pControl->GetUserIndex(Tickle.qwUserID,&dwUserIndex);
		pTickle=pControl->GetTickleInfo(dwUserIndex,Tickle.dwQType,&hLock);
		if (NULL==pTickle){
			continue;
		}//endif
		pTickle->dwQLength=Tickle.dwQLength;
		ReleaseMutex(hLock);

		//Increment counter
		dwcTICKLEs++;

	}//endwhile
	
	//Output counters
	printf("TICKLE: %u\n",dwcTICKLEs);

	SetEvent(hStopped);

	return 0;

}//endmethod
