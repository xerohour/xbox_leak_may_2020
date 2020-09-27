#ifndef __DPMSG_H__
#define __DPMSG_H__

//***************
//*** Defines ***
//***************

#define DPMSG_SYNCWAIT		NETSYNC_MSG_USER + 1
#define DPMSG_SYNCSIGNAL	NETSYNC_MSG_USER + 2
#define DPMSG_HOSTINFO		NETSYNC_MSG_USER + 3
#define DPMSG_DONE			NETSYNC_MSG_USER + 4
#define DPMSG_NEWHOST		NETSYNC_MSG_USER + 5
#define DPMSG_HOSTKEEPALIVE	NETSYNC_MSG_USER + 6

#define MAX_DPTEST_CLIENTS	10

//****************
//*** Typedefs ***
//****************

// DP test case IDs
typedef enum _DPMSG_NET_TESTTYPES
{

	DPMSG_NET_MAXTEST
} DPMSG_NET_TESTTYPES, *PDPMSG_NET_TESTTYPES;


// Generic message header
typedef struct _DP_GENERIC {
    DWORD				dwMessageId;	// Specifies the message id
} DP_GENERIC, *PDP_GENERIC;

// Message from a machine connected to this session that indicates it is waiting for the named "event"
// If data needs to be exchanged along with this synchronization event, it is provided.  Only one connected
// (presumably the host or server) machine is allowed to send data with its wait message
typedef struct _DP_SYNCWAIT {
	DWORD				dwMessageId;
	DWORD				dwSyncDataSize;				// Size of the variable data provided at the end of the message
	DWORD				dwMinMachinesRequired;		// Number of machines that must be waiting before the server sends
													// a "signal" message.  If this is 0, then all machines must "wait"
	CHAR				szSyncDataName[16];			// Unique name of event that is
} DP_SYNCWAIT, *PDP_SYNCWAIT;

// Message from the netsync server indicating that all connected machines have signalled that are waiting
// for the specified message
typedef struct _DP_SYNCSIGNAL {
	DWORD				dwMessageId;
	DWORD				dwSyncDataSize;
	CHAR				szSyncDataName[16];
} DP_SYNCSIGNAL, *PDP_SYNCSIGNAL;

// Message from the netsync server with information on which machine has been selected to be the
// host-peer or server and which has been seletect to be the non-host-peer or client
typedef struct _DP_HOSTINFO {
    DWORD				dwMessageId;		// Specifies the message id
	DWORD				dwTotalMachines;	// Number of machines in the test
	DWORD				dwHostAddr;			// Address of the machine chosen as host
	DWORD				dwNonHostAddrs[1];	// Addresses of the machines chosen as non-hosts
} DP_HOSTINFO, *PDP_HOSTINFO;

// Message from the netsync client indicating that the host migrated in a peer-to-peer session
// Sent from the new host so that the Netsync server will send the correct information to new
// peers as the try to connect
typedef struct _DP_NEWHOST {
    DWORD				dwMessageId;		// Specifies the message id
	DWORD				dwHostAddr;			// Address of the new machine chosen as host
} DP_NEWHOST, *PDP_NEWHOST;

// Message from the netsync client that is acting as DirectPlay host.  This is necessary so that the
// Netsync server can be sure that the current host is still alive.
typedef struct _DP_HOSTKEEPALIVE {
    DWORD				dwMessageId;		// Specifies the message id
} DP_HOSTKEEPALIVE, *PDP_HOSTKEEPALIVE;

#endif

