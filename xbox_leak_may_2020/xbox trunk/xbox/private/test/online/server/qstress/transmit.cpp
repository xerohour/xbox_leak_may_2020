/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "transmit.h"
#include <comdef.h>

#define XRGPLACEHOLDER 0
#define XIPPLACEHOLDER 0

/////////////////////////////////////////////////////////////////////////////
// TransmitThreadFunc
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI TransmitThreadFunc(LPVOID pData){

	//Counters
	DWORD dwcADDs=0L;
	DWORD dwcDELETEs=0L;
	DWORD dwcLISTs=0L;
	DWORD dwcUSERINFOs=0L;
	DWORD dwcDEADXIPs=0L;

	//Verify parameters
	if (NULL==pData){
		return -1;
	}//endif

	//Initializes the COM library for use by the calling thread and sets the thread's concurrency model.
	if FAILED(CoInitializeEx(NULL,COINIT_MULTITHREADED)){
		return -1;
	}//endif

	//Cast parameters 
	TransmitThreadFuncParams* pParams=(TransmitThreadFuncParams*)pData;
	Channels* pChannels=pParams->pChannels;
	Control* pControl=pParams->pControl;
	Configuration* pConfig=pParams->pConfig;
	HANDLE hStop=pParams->hStop;
	HANDLE hStopped=pParams->hStopped;
	HANDLE hReady=pParams->hReady;
	HANDLE hStart=pParams->hStart;

	//Get the ip address of the local machine
	DWORD dwLocalIP;
	char pszHostname[256];
	gethostname(pszHostname,sizeof(pszHostname));
	_bstr_t bstrHostname(pszHostname);
	pChannels->m_rgChannels[0].pEngine->InternetAddress(bstrHostname,&dwLocalIP);

	//Store servername
	_bstr_t bstrServerName(pConfig->pszServerName);

	//Allocate space for multiple destinations
	QWORD* rgqwDestUserId=new QWORD[pConfig->dwDestinations];

	//Allocate items for each type of queue
	BYTE** rgrgbData=NULL;
	rgrgbData=new BYTE*[pConfig->dwQueues];
	for (DWORD dwi=0L; dwi<pConfig->dwQueues; dwi++){
		rgrgbData[dwi]=new BYTE[pConfig->rgdwMinSize[dwi]];
		memset(rgrgbData[dwi],0xFF,pConfig->rgdwMinSize[dwi]);
	}//endfor

	//Get number of users and channels
	DWORD dwcUsers=pControl->GetUserCount();
	DWORD dwcQueues=pControl->GetQueuesPerUserCount();
	UINT uicChannels=pChannels->GetNumOfChannels();

	//State pointers
	OpStateInfo* pOpStateInfo=NULL;
	AckInfo* pAckInfo=NULL;
	ListReplyInfo* pListReplyInfo=NULL;
	TickleInfo* pTickleInfo=NULL;

	//Common case
	DWORD dwUserIndex=0L;  //Random user
	UINT uiChannel=0;
	QWORD qwUserId=MAKEQWORD(0L,0L);
	DWORD dwSequence=0L;
	dwi=0L;
	QWORD qwQUID=MAKEQWORD(0L,0L);
	HANDLE hLock=NULL;

	//Add case
	DWORD dwBuddyIndex=0L;
	UINT uicDestinations=0;
	UINT uicActualDestinations=0;
	UINT uiQueue=0;

	//List case
	QWORD qwCookie=MAKEQWORD(0L,0L);
	DWORD dwMaxItems=0L;

	SetEvent(hReady);
	WaitForSingleObject(hStart,INFINITE);

	//While not time to shutdown perform actions
	while (WAIT_OBJECT_0!=WaitForSingleObject(hStop,0)){

		//Pick a random channel
		uiChannel=rand()%uicChannels;

		//Choose a random userid (rand sucks it choses a number between 0 and RAND_MAX=0x7fff)
		dwUserIndex=((rand()%0xff<<24)+(rand()%0xff<<16)+(rand()%0xff<<8)+rand()%0xff)%dwcUsers;

		//Get the operational state
		pOpStateInfo=pControl->GetOpStateInfo(dwUserIndex);
		if (NULL==pOpStateInfo){
			continue;
		}//endif

		//Get user id and sequence number
		qwUserId=pOpStateInfo->qwUserId;
		dwSequence= ++(pOpStateInfo->dwSequence);

		//Hash the connection (channel)
		uiChannel = (UINT)(qwUserId % uicChannels);

		/***********/
		/* CONNECT */
		/***********/

		if (false==pChannels->m_rgChannels[uiChannel].fUp){

			//Connect to server
			if FAILED(pChannels->m_rgChannels[uiChannel].pEngine->Connect(bstrServerName,pConfig->dwPort)){
				//TODO: ADD WARNING
				continue;
			}//endif

			//Send HELLO
			if FAILED(pChannels->m_rgChannels[uiChannel].pEngineEx->MsgHelloEx(false,0L,dwSequence,MAKELONG(9,0),OLESTR("QSTRESS"),7L)){
				//TODO: ADD WARNING
				continue;
			}//endif

			//Mark connection (channel) as up
			pChannels->m_rgChannels[uiChannel].fUp=true;

			continue;

		}//endif CONNECT

		/************/
		/* USERINFO */
		/************/

		//Only send a user info if one has not been sent before
		if (opstate_do_userinfos==pOpStateInfo->uiState){
	
			//Clear ack receipt
			pAckInfo=pControl->GetAckInfo(dwUserIndex,&hLock);
			pAckInfo->fAckReceived=FALSE;
			ReleaseMutex(hLock);

			//Send USERINFO
			pOpStateInfo->qwXIP=MAKEQWORD(RANDOMDWORD,RANDOMDWORD);
			if FAILED(pChannels->m_rgChannels[uiChannel].pEngineEx->MsgUserInfoEx(true,LODWORD(pOpStateInfo->qwXIP),dwSequence,qwUserId,pOpStateInfo->qwXIP,XRGPLACEHOLDER,dwLocalIP,TICKLEPORT)){
				//TODO: ADD WARNING
				continue;
			}//endif

			pOpStateInfo->dwTimeToDie=(RANDOMDWORD%(pConfig->dwMaxLifetime*1000))+GetTickCount();
			pOpStateInfo->uiState=opstate_do_adds;
			
			dwcUSERINFOs++;

			continue;

		}//endif USERINFO

		/***********/
		/* DEADXIP */
		/***********/

		if (GetTickCount()>=pOpStateInfo->dwTimeToDie){
			qwQUID=pOpStateInfo->qwXIP;
			pOpStateInfo->uiState=opstate_do_userinfos;
			for (dwi=0L; dwi<dwcUsers; dwi++){
				pOpStateInfo=pControl->GetOpStateInfo(dwi);
				if (NULL==pOpStateInfo){
					break;
				}//endif
				if (pOpStateInfo->qwXIP==qwQUID){
					if FAILED(pChannels->m_rgChannels[uiChannel].pEngineEx->MsgDeadXIPEx(false,LODWORD(pOpStateInfo->qwXIP),dwSequence,pOpStateInfo->qwXIP)){
						//TODO: ADD WARNING
						continue;
					}//endif
					pOpStateInfo->uiState=opstate_do_userinfos;
					pAckInfo=pControl->GetAckInfo(dwi,&hLock);
					pAckInfo->fAckReceived=FALSE;
					ReleaseMutex(hLock);
					dwcDEADXIPs++;
				}//endif
			}//endfor
			continue;
		}//endif

		/*******/
		/* ADD */
		/*******/

		pAckInfo=pControl->GetAckInfo(dwUserIndex,NULL);
		if ((opstate_do_adds==pOpStateInfo->uiState) && (TRUE==pAckInfo->fAckReceived)){

			//Random parameters
			uicDestinations=(rand()%(pConfig->dwDestinations-1))+1;
			uicActualDestinations=0;
			uiQueue=rand()%pConfig->dwQueues;

			//Populate destination list with users
			for (dwi=0; dwi<uicDestinations; dwi++){

				//Choose a random buddy index
				dwBuddyIndex=((rand()%0xff<<24)+(rand()%0xff<<16)+(rand()%0xff<<8)+rand()%0xff)%dwcUsers;

				//Check if destination has a queue
				pAckInfo=pControl->GetAckInfo(dwBuddyIndex,NULL);
				if (FALSE==pAckInfo->fAckReceived){
					continue;
				}//endif
				
				//Add destination to list
				rgqwDestUserId[uicActualDestinations]=pControl->GetOpStateInfo(dwBuddyIndex)->qwUserId;
				if ((TRUE==pConfig->rgfDuplicates[uiQueue]) || (0==pControl->GetOpStateInfo(dwBuddyIndex)->dwItemsAdded)){
					pControl->GetOpStateInfo(dwBuddyIndex)->dwItemsAdded++;
				}//endif
				uicActualDestinations++;

				//Clear tickle queue size
				pTickleInfo=pControl->GetTickleInfo(dwBuddyIndex,uiQueue,&hLock);
				pTickleInfo->dwQLength=0;
				ReleaseMutex(hLock);

			}//endif

			//Anyone's queue around to add to?
			if (0==uicActualDestinations){
				continue;
			}//endif

			//Perform action
			if (FAILED(pChannels->m_rgChannels[uiChannel].pEngineEx->MsgAddEx(false,LODWORD(pOpStateInfo->qwXIP),dwSequence,rgqwDestUserId,uicActualDestinations,uiQueue,rgrgbData[uiQueue],pConfig->rgdwMinSize[uiQueue]))){
				//TODO: ADD WARNING
				continue;
			}//endif

			pOpStateInfo->uiState=opstate_do_lists;

			dwcADDs++;

			continue;

		}//endif ADD

		/********/
		/* LIST */
		/********/

		if (opstate_do_lists==pOpStateInfo->uiState){

			qwCookie=MAKEQWORD(RANDOMDWORD,RANDOMDWORD);

			for (dwi=0; dwi<dwcQueues;dwi++){
				
				//Tickle controlled or based on internal state?
				if (TRUE==pConfig->fTickleControl){

					//Any thing to list in this queue?
					pTickleInfo=pControl->GetTickleInfo(dwUserIndex,dwi,NULL);
					if (pTickleInfo->dwQLength==0){
						continue;  //continue for loop
					}//endif

					dwMaxItems=rand()%pTickleInfo->dwQLength+1;

				}else{

					//Any thing to list in this queue?
					pOpStateInfo=pControl->GetOpStateInfo(dwUserIndex);
					if (pOpStateInfo->dwItemsAdded==0){
						continue;
					}//endif

					dwMaxItems=rand()%pOpStateInfo->dwItemsAdded+1;

				}//endif

				//Clear list reply acknowledgement
				pListReplyInfo=pControl->GetListReplyInfo(dwUserIndex,dwi,&hLock);
				pListReplyInfo->fListReplyReceived=FALSE;
				qwQUID=pListReplyInfo->qwNextQUID;
				ReleaseMutex(hLock);

				//Perform action
				if FAILED(pChannels->m_rgChannels[uiChannel].pEngineEx->MsgListEx(false,LODWORD(pOpStateInfo->qwXIP),dwSequence,qwUserId,dwi,qwCookie,/*startquid*/qwQUID,dwMaxItems,0xFFFFFFFF)){
					//TODO: ADD WARNING
					continue;
				}//endif

				pOpStateInfo->uiState=opstate_do_deletes;

				dwcLISTs++;

			}//endfor

			continue;

		}//endif LIST

		/**********/
		/* DELETE */
		/**********/

		if (opstate_do_deletes==pOpStateInfo->uiState){

			for (dwi=0; dwi<dwcQueues;dwi++){

				//Do we have a list reply on this queue yet?
				pListReplyInfo=pControl->GetListReplyInfo(dwUserIndex,dwi,NULL);
				if (FALSE==pListReplyInfo->fListReplyReceived){
					continue;
				}//endif

				qwQUID=pListReplyInfo->qwNextQUID;
				if (0==qwQUID){
					qwQUID=MAKEQWORD(0xFFFF,0xFFFF);
				}else{
					qwQUID--;
				}//endif

				//Perform action
				if FAILED(pChannels->m_rgChannels[uiChannel].pEngineEx->MsgDeleteEx(false,LODWORD(pOpStateInfo->qwXIP),dwSequence,uiQueue,qwUserId,MAKEQWORD(0L,0L),qwQUID)){
					//TODO: ADD WARNING
					continue;
				}//endif
	
				pOpStateInfo=pControl->GetOpStateInfo(dwUserIndex);
				pOpStateInfo->dwItemsAdded-=pListReplyInfo->dwNumItems;
				if (pOpStateInfo->dwItemsAdded>0){
					pOpStateInfo->uiState=opstate_do_lists;
				}else{
					pOpStateInfo->uiState=opstate_do_adds;
				}//endif

				dwcDELETEs++;

			}//endfor

			continue;

		}//endif XACTION_DELETE

		continue;

	}//endwhile

	//Deallocate destinations
	if (NULL!=rgqwDestUserId){
		delete [] rgqwDestUserId;
		rgqwDestUserId=NULL;
	}//endif

	//Deallocate queue item data
	for (dwi=0L; dwi<pConfig->dwQueues;dwi++){
		if (NULL!=rgrgbData[dwi]){
			delete [] rgrgbData[dwi];
			rgrgbData[dwi]=0L;
		}//endif
	}//endfor
	if (NULL!=rgrgbData){
		delete [] rgrgbData;
		rgrgbData=NULL;
	}//endif

	//Closes the COM library on the current thread.
	CoUninitialize();

	//Output counters
	printf("ADD: %u\n",dwcADDs);
	printf("DELETE: %u\n",dwcDELETEs);
	printf("LIST: %u\n",dwcLISTs);
	printf("USERINFO: %u\n",dwcUSERINFOs);
	printf("DEADXIP: %u\n",dwcDEADXIPs);

	for (dwi=0; dwi<uicChannels;dwi++){
		pChannels->m_rgChannels[dwi].pEngineEx->Flush(12000);
	}//endfor

	SetEvent(hStopped);

	return 0;

}//endmethod
