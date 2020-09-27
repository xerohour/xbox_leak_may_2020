/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "control.h"
#include "time.h"

//Macro for indexing queue information by user index and queue number
#define QUEUEINDEX(UserIndex,QueueIndex) (UserIndex*m_dwcQueuesPerUser+QueueIndex)

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
Control::Control(DWORD dwcUsers, DWORD dwcQueuesPerUser, HANDLE hShutdownEvent):
m_hShutdownEvent(hShutdownEvent),
m_fInit(FALSE),
m_rgMapNodes(NULL),
m_dwcUsers(dwcUsers),
m_dwcQueuesPerUser(dwcQueuesPerUser),
m_rgOpStateInfo(NULL),
m_rgAckInfo(NULL),
m_rgListReplyInfo(NULL),
m_rgTickleInfo(NULL)
{

	printf("Allocating user data...");

	//Allocate operational state information array
	m_rgOpStateInfo=new OpStateInfo[m_dwcUsers];
	ZeroMemory(m_rgOpStateInfo,sizeof(OpStateInfo)*m_dwcUsers);
	_ASSERT(NULL!=m_rgOpStateInfo);
	if (NULL==m_rgOpStateInfo){
		return;
	}//endif

	//Allocate ack information array
	m_rgAckInfo=new AckInfo[m_dwcUsers];
	ZeroMemory(m_rgAckInfo,sizeof(AckInfo)*m_dwcUsers);
	_ASSERT(NULL!=m_rgAckInfo);
	if (NULL==m_rgAckInfo){
		return;
	}//endif
	
	//Allocate tickle information array
	m_rgTickleInfo=new TickleInfo[m_dwcUsers*m_dwcQueuesPerUser];
	ZeroMemory(m_rgTickleInfo,sizeof(TickleInfo)*m_dwcUsers*m_dwcQueuesPerUser);
	_ASSERT(NULL!=m_rgTickleInfo);
	if (NULL==m_rgTickleInfo){
		return;
	}//endif

	//Allocate list reply information array
	m_rgListReplyInfo=new ListReplyInfo[m_dwcUsers*m_dwcQueuesPerUser];
	ZeroMemory(m_rgListReplyInfo,sizeof(ListReplyInfo)*m_dwcUsers*m_dwcQueuesPerUser);
	_ASSERT(NULL!=m_rgListReplyInfo);
	if (NULL==m_rgListReplyInfo){
		return;
	}//endif

	//Create mutexes
	for (DWORD dwi=0; dwi<NUMOBJLOCKS; dwi++){
		m_rgAckLocks[dwi]=CreateMutex(NULL,FALSE,NULL);
		m_rgListReplyLocks[dwi]=CreateMutex(NULL,FALSE,NULL);
		m_rgTickleLocks[dwi]=CreateMutex(NULL,FALSE,NULL);
	}//endfor

	//Create lookup map nodes
	m_rgMapNodes=new UserIdMapNode[m_dwcUsers];
	_ASSERT(NULL!=m_rgMapNodes);
	if (NULL==m_rgMapNodes){
		return;
	}//endif

	//Create lookup map and populate operational state information with userids
	srand( (unsigned)time( NULL ) );
	QWORD qwUserId;
	DWORD dw;
	for (dwi=0; dwi<m_dwcUsers; dwi++){
		do{
			qwUserId=MAKEQWORD(RANDOMDWORD,RANDOMDWORD);
		}while(SUCCEEDED(GetUserIndex(qwUserId,&dw)));
		m_rgOpStateInfo[dwi].qwUserId=qwUserId;
		m_rgMapNodes[dwi].dwHiUserId=HIDWORD(qwUserId);
		m_rgMapNodes[dwi].dwIndex=dwi;
		m_UserIdMap.insert(LOUSERID2INDEX::value_type(LODWORD(qwUserId),&m_rgMapNodes[dwi]));
	}//endfor

	printf ("done!\n");

	//Initialization success
	m_fInit=true;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////

Control::~Control(){

	printf("Deallocating user data...");

	//Close mutexes
	for (DWORD dwi=0; dwi<NUMOBJLOCKS; dwi++){
		CloseHandle(m_rgAckLocks[dwi]);
		CloseHandle(m_rgListReplyLocks[dwi]);
		CloseHandle(m_rgTickleLocks[dwi]);
	}//endfor

	//Deallocate tickle information arrays
	if (NULL!=m_rgTickleInfo){
		delete[] m_rgTickleInfo;
		m_rgTickleInfo=NULL;
	}//endif

	//Deallocate list reply information arrays
	if (NULL!=m_rgListReplyInfo){
		delete[] m_rgListReplyInfo;
		m_rgListReplyInfo=NULL;
	}//endif
 
	//Deallocate ack information arrays
	if (NULL!=m_rgAckInfo){
		delete[] m_rgAckInfo;
		m_rgAckInfo=NULL;
	}//endif

	//Deallocate operational state information arrays
	if (NULL!=m_rgOpStateInfo){
		delete[] m_rgOpStateInfo;
		m_rgOpStateInfo=NULL;
	}//endif

	//Clear lookup map
	m_UserIdMap.clear();

	//Deallocate map nodes
	if (NULL!=m_rgMapNodes){
		delete[] m_rgMapNodes;
		m_rgMapNodes=NULL;
	}//endif

	printf ("done!\n");

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// GetUserIndex
/////////////////////////////////////////////////////////////////////////////

HRESULT Control::GetUserIndex(QWORD qwUserID, DWORD* pdwIndex){
	
	LOUSERID2INDEX::iterator MapIterator;  //Map iterator

	//Do logarithmic time search for userid
    MapIterator = m_UserIdMap.find(LODWORD(qwUserID));
	
	//If there is a match on the low word we have the right bucket; find the node...
	while (MapIterator!=m_UserIdMap.end()){
		UserIdMapNode* pNode=(*MapIterator).second;
		if (pNode->dwHiUserId==HIDWORD(qwUserID)){
			*pdwIndex=pNode->dwIndex;
			return S_OK;
		}//endif
		MapIterator++;
	}//endwhile

	return E_FAIL;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// GetOpStateInfo
/////////////////////////////////////////////////////////////////////////////

OpStateInfo* Control::GetOpStateInfo(DWORD dwUserIndex){

	return &m_rgOpStateInfo[dwUserIndex];

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// GetAckInfo
/////////////////////////////////////////////////////////////////////////////

AckInfo* Control::GetAckInfo(DWORD dwUserIndex, HANDLE* phLock){

	//Lock?
	if (NULL != phLock){
		HANDLE rghEvents[2];
		rghEvents[0]=m_rgAckLocks[dwUserIndex%NUMOBJLOCKS];
		rghEvents[1]=m_hShutdownEvent;
		if (0!=WaitForMultipleObjects(2,rghEvents,false,INFINITE)-WAIT_OBJECT_0){
			return NULL;
		}//endif
		*phLock=m_rgAckLocks[dwUserIndex%NUMOBJLOCKS];
	}//endif

	return &m_rgAckInfo[dwUserIndex];

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// GetListReplyInfo
/////////////////////////////////////////////////////////////////////////////

ListReplyInfo* Control::GetListReplyInfo(DWORD dwUserIndex, DWORD dwQueue, HANDLE* phLock){

	//Lock?
	if (NULL != phLock){
		HANDLE rghEvents[2];
		rghEvents[0]=m_rgListReplyLocks[dwUserIndex%NUMOBJLOCKS];
		rghEvents[1]=m_hShutdownEvent;
		if (0!=WaitForMultipleObjects(2,rghEvents,false,INFINITE)-WAIT_OBJECT_0){
			return NULL;
		}//endif
		*phLock=m_rgListReplyLocks[dwUserIndex%NUMOBJLOCKS];
	}//endif

	return &m_rgListReplyInfo[QUEUEINDEX(dwUserIndex,dwQueue)];

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// GetTickleInfo
/////////////////////////////////////////////////////////////////////////////

TickleInfo* Control::GetTickleInfo(DWORD dwUserIndex, DWORD dwQueue, HANDLE* phLock){

	//Lock?
	if (NULL != phLock){
		HANDLE rghEvents[2];
		rghEvents[0]=m_rgTickleLocks[dwUserIndex%NUMOBJLOCKS];
		rghEvents[1]=m_hShutdownEvent;
		if (0!=WaitForMultipleObjects(2,rghEvents,false,INFINITE)-WAIT_OBJECT_0){
			return NULL;
		}//endif
		*phLock=m_rgTickleLocks[dwUserIndex%NUMOBJLOCKS];
	}//endif

	return &m_rgTickleInfo[QUEUEINDEX(dwUserIndex,dwQueue)];

}//endmethod
