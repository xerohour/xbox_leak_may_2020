#ifndef __UDPMSG_H__
#define __UDPMSG_H__

//***************
//*** Defines ***
//***************

#define UDPMSG_TESTREQ		NETSYNC_MSG_USER + 1
#define UDPMSG_TESTACK		NETSYNC_MSG_USER + 2
#define UDPMSG_TESTDONE		NETSYNC_MSG_USER + 3

#define PROTOCOL_UDP		17

#define UDPHEADER_SIZE		8
#define	IPHEADER_SIZE		20

//****************
//*** Typedefs ***
//****************

// UDP test case IDs
typedef enum _UDPMSG_TESTTYPES
{
	// Tests for source port field
	UDPMSG_SOURCEPORT_MAX,
	UDPMSG_SOURCEPORT_MIN,
	UDPMSG_SOURCEPORT_ZERO,

	// Tests for dest port field
	UDPMSG_DESTPORT_MAX,
	UDPMSG_DESTPORT_MIN,
	UDPMSG_DESTPORT_ZERO,

	// Tests for length field
	UDPMSG_LENGTH_BELOWMIN,
	UDPMSG_LENGTH_ATMIN,
	UDPMSG_LENGTH_ABOVEMIN,
	UDPMSG_LENGTH_ABOVETOTAL,
	UDPMSG_LENGTH_BELOWTOTAL,
	UDPMSG_LENGTH_MAXIMUM,

	// Tests for checksum field
	UDPMSG_CHECKSUM_ZEROS,

	UDPMSG_MAXTEST
} UDPMSG_TESTTYPES, *PUDPMSG_TESTTYPES;

// UDP test request packet
typedef struct _UDP_TESTREQ {
    DWORD				dwMessageId;	// Specifies the message id
	UDPMSG_TESTTYPES	TestId;			// Specifies the test requested
	CHAR				MacAddr[16];	// Specifies the MAC address of the client (make size a multiple of 4)
} UDP_TESTREQ, *PUDP_TESTREQ;

// UDP test request ack
typedef struct _UDP_TESTACK {
    DWORD				dwMessageId;	// Specifies the message id
	UDPMSG_TESTTYPES	TestId;			// Specifies the test requested
} UDP_TESTACK, *PUDP_TESTACK;

#endif