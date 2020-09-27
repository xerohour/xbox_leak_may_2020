#ifndef __TNCONTROL_MESSAGES__
#define __TNCONTROL_MESSAGES__
//#pragma message("Defining __TNCONTROL_MESSAGES__")




//==================================================================================
// Defines
//==================================================================================
#ifdef TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllexport)

#else // ! TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllimport)

#endif // ! TNCONTRL_EXPORTS

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG
#endif // not DEBUG





//==================================================================================
// Message IDs
//==================================================================================
//#define CTRLMSGID_GENERIC						0
#define CTRLMSGID_ENUM							1
#define CTRLMSGID_ENUMREPLY						2
#define CTRLMSGID_HANDSHAKE						3
#define CTRLMSGID_HANDSHAKEREPLY				4
#define CTRLMSGID_JOIN							5
#define CTRLMSGID_JOINREPLY						6
#define CTRLMSGID_READYTOTEST					7
#define CTRLMSGID_TESTINGSTATUS					8
#define CTRLMSGID_NEWTEST						9
#define CTRLMSGID_ANNOUNCESUBTEST				10
#define CTRLMSGID_ANNOUNCETESTREPLY				11
#define CTRLMSGID_REPORT						12
#define CTRLMSGID_WARNING						13
#define CTRLMSGID_REQUESTSYNC					14
#define CTRLMSGID_SYNCREPLY						15
#define CTRLMSGID_USERQUERY						16
#define CTRLMSGID_USERRESPONSE					17
#define CTRLMSGID_IDLE							18
#define CTRLMSGID_IDLEREPLY						19
#define CTRLMSGID_BREAK							20
#define CTRLMSGID_QUERYMETAMASTER				21
#define CTRLMSGID_METAMASTERREPLY				22
#define CTRLMSGID_REGISTERMASTER				23
#define CTRLMSGID_REGISTERMASTERREPLY			24
#define CTRLMSGID_REQUESTMASTERUPDATE			25
#define CTRLMSGID_MASTERUPDATE					26
#define CTRLMSGID_UNREGISTERMASTER				27
#define CTRLMSGID_MASTERPINGSLAVE				28
#define CTRLMSGID_MASTERPINGSLAVEREPLY			29
#define CTRLMSGID_DUMPLOG						30
#define CTRLMSGID_REQUESTPOKETEST				31
#define CTRLMSGID_LOSTTESTER					32
#define CTRLMSGID_ACCEPTREACHCHECK				33
#define CTRLMSGID_ACCEPTREACHCHECKREPLY			34
#define CTRLMSGID_CONNECTREACHCHECK				35
#define CTRLMSGID_CONNECTREACHCHECKREPLY		36
#define CTRLMSGID_INTERSLAVEREACHCHECK			37
#define CTRLMSGID_INTERSLAVEREACHCHECKREPLY		38
#define CTRLMSGID_FREEOUTPUTVARS				39
#define CTRLMSGID_GETMACHINEINFO				40
#define CTRLMSGID_GETMACHINEINFOREPLY			41



//==================================================================================
// Message structures
//==================================================================================
typedef struct tagCTRLMSG_GENERIC
{
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values
} CTRLMSG_GENERIC, * PCTRLMSG_GENERIC;

typedef struct tagCTRLMSG_ENUM
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwVersion; // version of control layer we're using (and want to connect to)
	TNMODULEID			moduleID; // structure that identifies the type of session we're looking for
	DWORD				dwMode; // session mode we are looking for

	// Anything after this is part of the user specified session ID string.
} CTRLMSG_ENUM, * PCTRLMSG_ENUM;

typedef struct tagCTRLMSG_ENUMREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
} CTRLMSG_ENUMREPLY, * PCTRLMSG_ENUMREPLY;

typedef struct tagCTRLMSG_HANDSHAKE
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwVersion; // version of control layer we're using (and want to connect to)
	TNMODULEID			moduleID; // structure that identifies the type of session we're looking for
	DWORD				dwMode; // session mode we are looking for

	// Anything after this is part of the user specified session ID string.
} CTRLMSG_HANDSHAKE, * PCTRLMSG_HANDSHAKE;

typedef struct tagCTRLMSG_HANDSHAKEREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwSessionID; // ID of this session
	DWORD				dwStartupDataSize; // size of data for use in slave's startup routine

	// Anything after this is part of the startup data blob
} CTRLMSG_HANDSHAKEREPLY, * PCTRLMSG_HANDSHAKEREPLY;

typedef struct tagCTRLMSG_JOIN
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	CHECKSUM			testChecksum; // checksum for the test preparation, to make sure everyone's "on the same page"
	DWORD				dwMultiInstanceKey; // key generated to help detect multiple slave instances on the same machine
	DWORD				dwTimelimit; // allows slave to override master's testing time limit
	DWORD				dwInfoSize; // size of machine info buffer
	DWORD				dwMasterAddrSize; // size of master's address

	// Anything after this is part of the machine info and master's address blob
} CTRLMSG_JOIN, * PCTRLMSG_JOIN;

typedef struct tagCTRLMSG_JOINREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the joining slave
} CTRLMSG_JOINREPLY, * PCTRLMSG_JOINREPLY;


typedef struct tagCTRLMSG_READYTOTEST
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the slave
} CTRLMSG_READYTOTEST, * PCTRLMSG_READYTOTEST;


// All testing is completely finished, you are all done.
#define CTRLTS_ALLCOMPLETE				1
// The current pass is finished, reset yourself for the next pass.
#define CTRLTS_REPETITIONCOMPLETE		2

typedef struct tagCTRLMSG_TESTINGSTATUS
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwStatus; // the new status of testing
} CTRLMSG_TESTINGSTATUS, * PCTRLMSG_TESTINGSTATUS;


typedef struct tagCTRLMSG_NEWTEST
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwUniqueTestID; // ID of this test
	int					iNumMachines; // total number of testers running the instance
	DWORD				dwInputDataSize; // size of input data in msg data blob

	// Anything after this is part of the case string, instance string, slave
	// ID array, and input data blob
} CTRLMSG_NEWTEST, * PCTRLMSG_NEWTEST;


typedef struct tagCTRLMSG_ANNOUNCESUBTEST
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the announcer
	DWORD				dwRequestID; // ID used to correlate response to this request
	DWORD				dwTopLevelUniqueID; // ID of the top most test in the heirarchy
	DWORD				dwParentUniqueID; // ID of the parent test
	int					iNumMachines; // number of testers in the tester index list

	// Anything after this is part of the test ID string and tester index list
	// data blob
} CTRLMSG_ANNOUNCESUBTEST, * PCTRLMSG_ANNOUNCESUBTEST;

typedef struct tagCTRLMSG_ANNOUNCETESTREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwResponseID; // Response ID used to correlate this response to the instigating AnnounceSubTest or RequestPokeTest command
	DWORD				dwUniqueTestID; // Unique ID of the test
} CTRLMSG_ANNOUNCETESTREPLY, * PCTRLMSG_ANNOUNCETESTREPLY;

typedef struct tagCTRLMSG_REPORT
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the reporter
	DWORD				dwTopLevelUniqueID; // top level control defined test ID for report
	DWORD				dwTestUniqueID; // control defined test ID this pertains to
	HRESULT				hresult; // success or error code
	BOOL				fTestComplete; // does this stop the test?
	BOOL				fSuccess; // is this a success (duh)?
	DWORD				dwOutputDataSize; // size of output data
	DWORD				dwVarsSize; // size of variables list data

	// Anything after this is part of the output data and variables blob
} CTRLMSG_REPORT, * PCTRLMSG_REPORT;

typedef struct tagCTRLMSG_WARNING
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the reporter
	DWORD				dwTopLevelUniqueID; // top level control defined test ID for report
	DWORD				dwTestUniqueID; // control defined test ID this pertains to
	HRESULT				hresult; // warning hresult code
	DWORD				dwUserDataSize; // size of user data

	// Anything after this is part of the warning data blob
} CTRLMSG_WARNING, * PCTRLMSG_WARNING;

typedef struct tagCTRLMSG_REQUESTSYNC
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the requestor
	DWORD				dwRequestID; // ID used to correlate response to this request
	DWORD				dwTopLevelUniqueID; // top level control defined test ID for sync
	DWORD				dwTestUniqueID; // control defined test ID that's trying to sync
	DWORD				dwNameSize; // size of name of sync
	int					iNumMachines; // how many tester IDs
	DWORD				dwDataSize; // size of data to send to sync-ers

	// Anything after this is part of the sync name and data blob
} CTRLMSG_REQUESTSYNC, * PCTRLMSG_REQUESTSYNC;

typedef struct tagCTRLMSG_SYNCREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwResponseID; // ID used to correllate this response to the request
	DWORD				dwDataSize; // size of data received from sync-ers

	// Anything after this is part of the sync data blob
} CTRLMSG_SYNCREPLY, * PCTRLMSG_SYNCREPLY;

typedef struct tagCTRLMSG_USERQUERY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwQuery; // user defined query ID or command
	DWORD				dwDataSize; // size of extra query data

	// Anything after this is part of the query data blob
} CTRLMSG_USERQUERY, * PCTRLMSG_USERQUERY;

typedef struct tagCTRLMSG_USERRESPONSE
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwResponse; // user defined response ID or command
	DWORD				dwDataSize; // size of extra response data

	// Anything after this is part of the response data blob
} CTRLMSG_USERRESPONSE, * PCTRLMSG_USERRESPONSE;


#define CTRLIDLE_NOTHING			1
#define CTRLIDLE_ANNOUNCESUBTEST	2
#define CTRLIDLE_SYNC				3

typedef struct tagCTRLMSG_IDLE
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the sender
	DWORD				dwWaitingOp; // one of the above current operations that is waiting
} CTRLMSG_IDLE, * PCTRLMSG_IDLE;


#define CTRLIDLEREPLY_WAITINGFOROTHERTESTERS	1

typedef struct tagCTRLMSG_IDLEREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwStatus; // one of the above possible status codes
} CTRLMSG_IDLEREPLY, * PCTRLMSG_IDLEREPLY;


typedef struct tagCTRLMSG_BREAK
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwUniqueTestID; // ID of test this break request pertains to
} CTRLMSG_BREAK, * PCTRLMSG_BREAK;


typedef struct tagCTRLMSG_QUERYMETAMASTER
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwMetaVersion; // version of the meta-master control layer API 
	DWORD				dwControlVersion; // version of the main control layer API 
	TNMODULEID			moduleID; // structure that identifies the type of session we're looking for
	DWORD				dwMode; // session mode we are looking for

	// Anything after this is part of the user specified session ID string.
} CTRLMSG_QUERYMETAMASTER, * PCTRLMSG_QUERYMETAMASTER;


#define CTRLMETAMASTERREPLY_NOMASTERSAVAILABLE		1
#define CTRLMETAMASTERREPLY_FOUNDMASTER				2

typedef struct tagCTRLMSG_METAMASTERREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwStatus; // what type of status this is

	// Anything after this is part of the master's address string.
} CTRLMSG_METAMASTERREPLY, * PCTRLMSG_METAMASTERREPLY;


typedef struct tagCTRLMSG_REGISTERMASTER
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwMetaVersion; // version of the meta-master control layer API
	DWORD				dwControlVersion; // version of the main control layer API
	DWORD				dwSessionID; // self-described ID of the session
	TNMODULEID			moduleID; // structure that identifies the type of session we're looking for
	DWORD				dwMode; // session mode we are looking for
	DWORD				dwAddressSize; // size of address string
	DWORD				dwSessionFilterSize; // size of session filter string

	// Anything after this is part of the address and user specified session
	// filter strings.
} CTRLMSG_REGISTERMASTER, * PCTRLMSG_REGISTERMASTER;

typedef struct tagCTRLMSG_REGISTERMASTERREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the registered master
} CTRLMSG_REGISTERMASTERREPLY, * PCTRLMSG_REGISTERMASTERREPLY;

typedef struct tagCTRLMSG_REQUESTMASTERUPDATE
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
} CTRLMSG_REQUESTMASTERUPDATE, * PCTRLMSG_REQUESTMASTERUPDATE;

typedef struct tagCTRLMSG_MASTERUPDATE
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the registered master
	int					iNumSlaves; // how many slaves are currently in the session
	BOOL				fJoinersAllowed; // whether new joiners are currently allowed
} CTRLMSG_MASTERUPDATE, * PCTRLMSG_MASTERUPDATE;

typedef struct tagCTRLMSG_UNREGISTERMASTER
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the registered master
} CTRLMSG_UNREGISTERMASTER, * PCTRLMSG_UNREGISTERMASTER;

typedef struct tagCTRLMSG_MASTERPINGSLAVE
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

} CTRLMSG_MASTERPINGSLAVE, * PCTRLMSG_MASTERPINGSLAVE;

typedef struct tagCTRLMSG_MASTERPINGSLAVEREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the joining slave
} CTRLMSG_MASTERPINGSLAVEREPLY, * PCTRLMSG_MASTERPINGSLAVEREPLY;


typedef struct tagCTRLMSG_DUMPLOG
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwUniqueTestID; // test ID (basically the reason) that this was generated from
	DWORD				dwPathSize; // size of path to dump memory log to

	// Anything after this is part of the path to dump to.
} CTRLMSG_DUMPLOG, * PCTRLMSG_DUMPLOG;

typedef struct tagCTRLMSG_REQUESTPOKETEST
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the announcer
	DWORD				dwRequestID; // ID used to correlate response to this request
	int					iNumMachines; // number of slave IDs in the tester list
	DWORD				dwInputDataSize; // size of input data being used

	// Anything after this is part of the test ID string, slave ID list and input
	// data blob.
} CTRLMSG_REQUESTPOKETEST, * PCTRLMSG_REQUESTPOKETEST;

typedef struct tagCTRLMSG_LOSTTESTER
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwTopLevelUniqueID; // top level control defined test ID for test
	DWORD				dwTestUniqueID; // control defined test ID for test
	int					iTesterNum; // tester who was lost
} CTRLMSG_LOSTTESTER, * PCTRLMSG_LOSTTESTER;

typedef struct tagCTRLMSG_ACCEPTREACHCHECK
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwID; // ID of this reach check
	DWORD				dwMethod; // type of communication that should be accepted
	DWORD				dwMethodDataSize; // size of extra method data, if any

	// Anything after this is part of the method data.
} CTRLMSG_ACCEPTREACHCHECK, * PCTRLMSG_ACCEPTREACHCHECK;

typedef struct tagCTRLMSG_ACCEPTREACHCHECKREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the machine replying
	DWORD				dwID; // ID of this reach check
	BOOL				fReady; // whether the slave can accept communication or not
	HRESULT				hresult; // failure code of the operation, if not ready
} CTRLMSG_ACCEPTREACHCHECKREPLY, * PCTRLMSG_ACCEPTREACHCHECKREPLY;

typedef struct tagCTRLMSG_CONNECTREACHCHECK
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwID; // ID of this reach check
	TNCTRLMACHINEID		idTarget; // ID of the target to reach
	DWORD				dwMethod; // type of communication that should be accepted
	DWORD				dwMethodDataSize; // size of extra method data, if any
	DWORD				dwAddressesSize; // size of addresses to use buffer
	TNCTRLMACHINEID		idPreviousTarget; // ID of target on same machine already checked, if any

	// Anything after this is part of the method data and addresses buffer.
} CTRLMSG_CONNECTREACHCHECK, * PCTRLMSG_CONNECTREACHCHECK;

typedef struct tagCTRLMSG_CONNECTREACHCHECKREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the machine replying
	DWORD				dwID; // ID of this reach check
	BOOL				fReachable; // whether the slave can reach the target or not
	HRESULT				hresult; // failure code of the operation, if unreachable
} CTRLMSG_CONNECTREACHCHECKREPLY, * PCTRLMSG_CONNECTREACHCHECKREPLY;

typedef struct tagCTRLMSG_INTERSLAVEREACHCHECK
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the machine sending
	DWORD				dwID; // ID of reach check the message is being sent for
	BOOL				fGuaranteed; // whether this item is being sent guaranteed or not
	DWORD				dwUsedAddressSize; // size of address data this message is being sent to

	// Anything after this is part of the address data.
} CTRLMSG_INTERSLAVEREACHCHECK, * PCTRLMSG_INTERSLAVEREACHCHECK;

typedef struct tagCTRLMSG_INTERSLAVEREACHCHECKREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the machine replying
	DWORD				dwID; // ID of reach check the message was received for
	DWORD				dwUsedAddressSize; // size of echoed data from message being replied to

	// Anything after this is part of the address data.
} CTRLMSG_INTERSLAVEREACHCHECKREPLY, * PCTRLMSG_INTERSLAVEREACHCHECKREPLY;

typedef struct tagCTRLMSG_FREEOUTPUTVARS
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the machine replying
	DWORD				dwCaseIDSize; // size of the case ID string to match, if any
	DWORD				dwInstanceIDSize; // size of the instance ID string to match, if any
	DWORD				dwNameSize; // size of the output variable name string to match, if any
	DWORD				dwTypeSize; // size of the output variable type string to match, if any

	// Anything after this is part of the case ID, instance ID, name and type
	// strings.
} CTRLMSG_FREEOUTPUTVARS, * PCTRLMSG_FREEOUTPUTVARS;

typedef struct tagCTRLMSG_GETMACHINEINFO
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	TNCTRLMACHINEID		id; // the ID of the machine sending
	DWORD				dwRequestID; // ID used to correlate response to this request
	TNCTRLMACHINEID		idMachineForInfo; // the ID of the machine whose info is to be retrieved
} CTRLMSG_GETMACHINEINFO, * PCTRLMSG_GETMACHINEINFO;

typedef struct tagCTRLMSG_GETMACHINEINFOREPLY
{
	// These fields must be the same as the ones in CTRLMSG_GENERIC
	DWORD				dwSize; // how big is this message
	DWORD				dwType; // ID of this message, see CTRLMSGID defines for values

	// These fields are unique to this message
	DWORD				dwResponseID; // echo of ID of request that generated this response
	DWORD				dwMachineInfoSize; // size of machine info

	// Anything after this is part of the machine info data.
} CTRLMSG_GETMACHINEINFOREPLY, * PCTRLMSG_GETMACHINEINFOREPLY;







#else //__TNCONTROL_MESSAGES__
//#pragma message("__TNCONTROL_MESSAGES__ already included!")
#endif //__TNCONTROL_MESSAGES__
