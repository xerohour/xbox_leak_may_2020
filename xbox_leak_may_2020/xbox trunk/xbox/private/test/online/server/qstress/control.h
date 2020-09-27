/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#ifndef __CONTROL_H_
#define __CONTROL_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif

#include <map>

#define NUMOBJLOCKS 100

//Define map node type
typedef struct UserIdMapNodeStruct{
	DWORD dwHiUserId;  //Hi dword of user id
	DWORD dwIndex;  //Index that this user exists at
}UserIdMapNode;

//Map will key on low dword and store high dword and index
using namespace std;
typedef multimap<DWORD, UserIdMapNode*> LOUSERID2INDEX;

//Operational state information state type enumeration
enum OpStateEnum{
	opstate_do_userinfos=0,
	opstate_do_adds=1,
	opstate_do_lists=2,
	opstate_do_deletes=3,
	opstate_endenum=4
};//endenum

//Operational state information
struct OpStateInfo{
	UINT uiState;  //User opstate
	QWORD qwUserId;  //User identification
	QWORD qwXIP; //XIP address
	DWORD dwSequence;  //Sequence number
	DWORD dwItemsAdded;   //Number of items we just added
	DWORD dwTimeToDie;  //Tick count to kill client after
};//endstruct

//Ack information
struct AckInfo{
	BOOL fAckReceived;  //Did we receive an ACK?
};//endstruct

//List reply information
struct ListReplyInfo{
	BOOL fListReplyReceived;  //Did we receive a LIST REPLY?
	HRESULT hr;  //Q_LIST_REPLY_MSG data
    QWORD qwCookie;  //Q_LIST_REPLY_MSG data
    QWORD qwNextQUID;  //Q_LIST_REPLY_MSG data
    DWORD dwQLeft;  //Q_LIST_REPLY_MSG data
    DWORD dwNumItems;  //Q_LIST_REPLY_MSG data
	QWORD dwLastQUID;  //QLIST_REPLY_MSG data
};//endstruct

//Tickle information
struct TickleInfo{
    DWORD dwQLength;  //Q_TICKLE_MSG data
};//endstruct

/////////////////////////////////////////////////////////////////////////////
// Controls operation of each user and manages each users queues
/////////////////////////////////////////////////////////////////////////////

class Control{

public:

	//Constructor
	Control(DWORD dwcUsers, DWORD dwcQueuesPerUser, HANDLE hShutdownEvent);

	//Constructor Initialized?
	bool IsInit(){return m_fInit;}

	//Destructor
	~Control();

	//Get the user index from the user id
	HRESULT GetUserIndex(QWORD qwUserID, DWORD* pdwIndex);

	//Get Operational State Information (non-thread-safe)
	OpStateInfo* GetOpStateInfo(DWORD dwUserIndex);

	//Get Ack Information (thread-safe through lock)
	AckInfo* GetAckInfo(DWORD dwUserIndex, HANDLE* phLock);

	//Get List Reply Information (thread-safe through lock)
	ListReplyInfo* GetListReplyInfo(DWORD dwUserIndex, DWORD dwQueue, HANDLE* phLock);

	//Get Tickle Information (thread-safe through lock)
	TickleInfo* GetTickleInfo(DWORD dwUserIndex, DWORD dwQueue, HANDLE* phLock);

	//Access to number of users and number of queues per user
	DWORD GetUserCount(){return m_dwcUsers;}
	DWORD GetQueuesPerUserCount(){return m_dwcQueuesPerUser;}
	
private:

	HANDLE m_hShutdownEvent;  //Shutdown event
	bool m_fInit;  //Instance initialized?
	LOUSERID2INDEX m_UserIdMap;  //UserId to index logarithic+1 time map
	UserIdMapNode* m_rgMapNodes;  //Nodes in map
	DWORD m_dwcUsers;  //User count
	DWORD m_dwcQueuesPerUser;  //Number of queues per user count
	OpStateInfo* m_rgOpStateInfo;  //Array of operational state information
	AckInfo* m_rgAckInfo;  //Array of ack information
	ListReplyInfo* m_rgListReplyInfo;  //Array of list reply information
	TickleInfo* m_rgTickleInfo;  //Array of tickle information
	HANDLE m_rgAckLocks[NUMOBJLOCKS];  //Array of ack object locks
	HANDLE m_rgListReplyLocks[NUMOBJLOCKS];  //Array of list reply object locks
	HANDLE m_rgTickleLocks[NUMOBJLOCKS];  //Array of tickle object locks
 
};//endclass

#endif