/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "receive.h"
#include "xqprotocol.h"
#include "connection.h"

#define PROCESSINGBUFFERSIZE 4096

typedef struct ChannelState_struct{
	BYTE rgbData[PROCESSINGBUFFERSIZE];  //Buffer
	DWORD dwGot;  //How many bytes we got this read
	DWORD dwOffset;  //Offset index to write/process in buffer
	DWORD dwLeft;  //Bytes left to process in buffer
	BYTE* pbData;  //Traveling pointer in buffer to write/process
}ChannelState;

/////////////////////////////////////////////////////////////////////////////
// ReceiveThreadFunc
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI ReceiveThreadFunc(LPVOID pData){

	//Counters
	DWORD dwcACKs=0L;
	DWORD dwcLISTREPLYs=0L;

	//Verify parameters
	if (NULL==pData){
		return -1;
	}//endif
	
	//Initializes the COM library for use by the calling thread and sets the thread's concurrency model.
	if FAILED(CoInitializeEx(NULL,COINIT_MULTITHREADED)){
		return -1;
	}//endif

	//Cast thread parameters 
	ReceiveThreadFuncParams* pParams=(ReceiveThreadFuncParams*)pData;
	Channels* pChannels=pParams->pChannels;  //Pointer to channel
	Control* pControl=pParams->pControl;  //Pointer to control object
	HANDLE hStop=pParams->hStop;
	HANDLE hStopped=pParams->hStopped;
	HANDLE hReady=pParams->hReady;
	HANDLE hStart=pParams->hStart;

	//Lock object
	HANDLE hLock=NULL;

	//Pointers to storage objects
	AckInfo* pAckInfo=NULL;  //Pointer to ack info object
	ListReplyInfo* pListReplyInfo=NULL;  //Pointer to list reply object

	//Pointers to view array of bytes as
	BASE_ACK_MSG* pAck=NULL;;  //Pointer to ACK
	Q_LIST_REPLY_MSG* pListReply=NULL;  //Pointer to LIST_REPLY

	DWORD dwUserIndex=0L; //User index

	//Setup channel traversal
	DWORD dwChannelIndex=0L;
	DWORD dwcChannels=pChannels->GetNumOfChannels();
	Channel* pChannel=NULL;

	//Allocate channel states
	ChannelState* rgState = new ChannelState[dwcChannels];
	ZeroMemory(rgState,sizeof(ChannelState)*dwcChannels);
	
	SetEvent(hReady);
	WaitForSingleObject(hStart,INFINITE);

	//While it is not time to shutdown perform actions
	while (WAIT_OBJECT_0!=WaitForSingleObject(hStop,0)){

		if (++dwChannelIndex>=dwcChannels){
			dwChannelIndex=0;
		}//endif
		pChannel=&(pChannels->m_rgChannels[dwChannelIndex]);

		//Try and get some receive data
		rgState[dwChannelIndex].dwGot=0L;
		rgState[dwChannelIndex].pbData=rgState[dwChannelIndex].rgbData+rgState[dwChannelIndex].dwOffset;

		getdata:
		if FAILED(pChannel->pEngineEx->GetResponseData((unsigned char*)(rgState[dwChannelIndex].pbData),PROCESSINGBUFFERSIZE-rgState[dwChannelIndex].dwOffset,&(rgState[dwChannelIndex].dwGot))){

			//Shutdown time?
			if (WAIT_OBJECT_0==WaitForSingleObject(hStop,0)){
				break;
			}//endif

			//Connection might be closed wait a few msecs and try again
			Sleep(100);

			goto getdata;

		}//endif

		rgState[dwChannelIndex].dwGot+=rgState[dwChannelIndex].dwOffset;
	
		//Got some data; time to process
		rgState[dwChannelIndex].dwOffset=0L;
		while (WAIT_OBJECT_0!=WaitForSingleObject(hStop,0)){

			rgState[dwChannelIndex].dwLeft=rgState[dwChannelIndex].dwGot-rgState[dwChannelIndex].dwOffset;
			rgState[dwChannelIndex].pbData=rgState[dwChannelIndex].rgbData+rgState[dwChannelIndex].dwOffset;

			//Do we have the first 2 critical dwords of the message?
			if (rgState[dwChannelIndex].dwLeft>sizeof(DWORD)*2){

				//Do we have the entire message?
				if (rgState[dwChannelIndex].dwLeft>=((BASE_MSG_HEADER*)(rgState[dwChannelIndex].pbData))->dwMsgLen+sizeof(BASE_MSG_HEADER)){

					//What type of command is it?
					switch (((BASE_MSG_HEADER*)(rgState[dwChannelIndex].pbData))->dwMsgType){
					
						/*******************/
						/* QMSG_LIST_REPLY */
						/*******************/

						case QMSG_LIST_REPLY:

							//Cast to list reply
							pListReply=(Q_LIST_REPLY_MSG*)(rgState[dwChannelIndex].pbData);

							//Lookup user index
							if FAILED(pControl->GetUserIndex(pListReply->qwUserID,&dwUserIndex)){
								continue;
							}//endif
							
							//Lock storage object
							pListReplyInfo=pControl->GetListReplyInfo(dwUserIndex,pListReply->dwQType,&hLock);
							if (NULL==pListReplyInfo){
								continue;
							}//endif 
							
							//Store information
							pListReplyInfo->dwNumItems=pListReply->dwNumItems;
							pListReplyInfo->dwQLeft=pListReply->dwQLeft;
							pListReplyInfo->hr=pListReply->hr;
							_ASSERT(S_OK==pListReplyInfo->hr);
							pListReplyInfo->qwCookie=pListReply->qwCookie;
							pListReplyInfo->qwNextQUID=pListReply->qwNextQUID;
							pListReplyInfo->fListReplyReceived=TRUE;

							//Release mutex
							ReleaseMutex(hLock);

							//Increment counter
							dwcLISTREPLYs++;

						break;

						/***************/
						/* MSGTYPE_ACK */
						/***************/

						case MSGTYPE_ACK:

							//Cast to ACK message
							pAck=(BASE_ACK_MSG*)(rgState[dwChannelIndex].pbData);

							//Lookup user index
							if FAILED(pControl->GetUserIndex(pAck->qwAckUserID,&dwUserIndex)){
								continue;
							}//endif

							//Lock storage object
							pAckInfo=pControl->GetAckInfo(dwUserIndex,&hLock);
							if (NULL==pAckInfo){
								continue;
							}//endif

							//Store information
							pAckInfo->fAckReceived=TRUE;

							//Release mutex
							ReleaseMutex(hLock);
							
							//Increment counter
							dwcACKs++;

						break;

						default:
							_ASSERT(0);  //We should never get here!

					}//endswitch

					//Advance to next message
					rgState[dwChannelIndex].dwOffset+=((BASE_MSG_HEADER*)(rgState[dwChannelIndex].pbData))->dwMsgLen+sizeof(BASE_MSG_HEADER);
					continue;

				}//endif
			}//endif
			
			//Partial message; move to front of buffer and reload
			if (rgState[dwChannelIndex].dwLeft!=0){
				memmove(rgState[dwChannelIndex].rgbData,rgState[dwChannelIndex].pbData,rgState[dwChannelIndex].dwLeft);
			}//endif
			rgState[dwChannelIndex].dwOffset=rgState[dwChannelIndex].dwLeft;

			//Exit processing loop to get more data
			break;

		}//endwhile (processing data)

	}//endwhile (while not shutdown)

	//Deallocate channel state
	if (NULL!=rgState){
		delete [] rgState;
		rgState=NULL;
	}//endif

	//Closes the COM library on the current thread.
	CoUninitialize();

	//Output counters
	printf ("ACK: %u\n",dwcACKs);
	printf ("LISTREPY: %u\n",dwcLISTREPLYs);

	SetEvent(hStopped);

	return 0;

}//endmethod