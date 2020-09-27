#ifndef __TCPMSG_H__
#define __TCPMSG_H__

//***************
//*** Defines ***
//***************

#define TCPMSG_TESTREQ		NETSYNC_MSG_USER + 1
#define TCPMSG_TESTACK		NETSYNC_MSG_USER + 2
#define TCPMSG_TESTDONE		NETSYNC_MSG_USER + 3

#define UDPHEADER_SIZE		8
#define	IPHEADER_SIZE		20
#define TCPHEADER_SIZE		20

//****************
//*** Typedefs ***
//****************

// TCP test case IDs
typedef enum _TCPMSG_TESTTYPES
{
	// Tests for sequence number field
	TCPMSG_SEQNUM_MAX,
	TCPMSG_SEQNUM_MIN,

	// Tests for data offset field
	TCPMSG_DATAOFF_BELOWMIN,
	TCPMSG_DATAOFF_ABOVEAVAIL,

#if 0
	// Tests for the reserved field
	TCPMSG_RESERVED_SET,

	// Tests for the window field
	TCPMSG_WINDOW_MIN,
	TCPMSG_WINDOW_MAX,

	// Tests for the checksum field
	TCPMSG_CHECKSUM_BAD,

	// Tests for the urgent pointer field
	TCPMSG_URGENT_AFTEREND,
	TCPMSG_URGENT_ATEND,
	TCPMSG_URGENT_BEFOREEND,
	TCPMSG_URGENT_MAX,
	TCPMSG_URGENT_NOFLAG,

	// Tests for TCP options
	TCPMSG_OPT_CUTOFF,
	TCPMSG_OPT_ATEND,
	TCPMSG_OPT_BEFOREEND,
	TCPMSG_OPT_NOOPTIONS,
	TCPMSG_OPT_MAXOPTIONS,
	TCPMSG_OPT_MAXOPTLEN,
	TCPMSG_OPT_ENDOPTFIRST,
	TCPMSG_OPT_MSSZERO,
	TCPMSG_OPT_MSSONE,
	TCPMSG_OPT_MSSMAX,

	// Tests for connection establishment
	TCPMSG_CONN_SYNANDNOACK,
	TCPMSG_CONN_EXTRASYN,
	TCPMSG_CONN_ACKWRONGSYN,
	TCPMSG_CONN_SYNACKWRONGSYN,
	TCPMSG_CONN_ACKNOCONN,
	TCPMSG_CONN_SYNACKLISTENER,

	// Tests for connection shutdown
	TCPMSG_SHUT_FINANDNOACK,
#endif

	TCPMSG_MAXTEST
} TCPMSG_TESTTYPES, *PTCPMSG_TESTTYPES;

// TCP test request packet
typedef struct _TCP_TESTREQ {
    DWORD				dwMessageId;	// Specifies the message id
	TCPMSG_TESTTYPES	TestId;			// Specifies the test requested
	CHAR				MacAddr[16];	// Specifies the MAC address of the client (make size a multiple of 4)
} TCP_TESTREQ, *PTCP_TESTREQ;

// TCP test request ack
typedef struct _TCP_TESTACK {
    DWORD				dwMessageId;	// Specifies the message id
	TCPMSG_TESTTYPES	TestId;			// Specifies the test requested
	CHAR				SpoofIp[16];	// Specifies the IP address that the server will spoof
} TCP_TESTACK, *PTCP_TESTACK;

#endif