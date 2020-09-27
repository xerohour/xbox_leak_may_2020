#ifndef __IPMSG_H__
#define __IPMSG_H__

//***************
//*** Defines ***
//***************

#define IPMSG_TESTREQ		NETSYNC_MSG_USER + 1
#define IPMSG_TESTACK		NETSYNC_MSG_USER + 2
#define IPMSG_TESTDONE		NETSYNC_MSG_USER + 3

#define PROTOCOL_UDP		17

#define UDPHEADER_SIZE		8
#define	IPHEADER_SIZE		20

//****************
//*** Typedefs ***
//****************

// IP test case IDs
typedef enum _IPMSG_TESTTYPES
{
	// Tests for version field
	IPMSG_VERSION_SIX,
	IPMSG_VERSION_BAD,

	// Tests for header length field
	IPMSG_HEADLEN_BELOWMIN,
	IPMSG_HEADLEN_MAX,
	IPMSG_HEADLEN_ABOVETOTALLEN,
	IPMSG_HEADLEN_ABOVEAVAIL,

	// Tests for type-of-service
	IPMSG_TOS_NORMAL,
	IPMSG_TOS_NETCONTROL,
	IPMSG_TOS_LOWDELAY,
	IPMSG_TOS_HIGHTHROUGH,
	IPMSG_TOS_HIGHRELI,
	IPMSG_TOS_RESERVED,

	// Tests for total length
	IPMSG_LENGTH_BELOWMIN,
	IPMSG_LENGTH_ATMIN,
	IPMSG_LENGTH_ABOVEMIN,
	IPMSG_LENGTH_ABOVETOTAL,
	IPMSG_LENGTH_BELOWTOTAL,
	IPMSG_LENGTH_MAX,

	// Tests for ID
	IPMSG_ID_MIN,
	IPMSG_ID_MAX,

	// Tests for flags
	IPMSG_FLAG_RESERVEDSET,
	IPMSG_FLAG_DFANDMF,

	// Tests for time-to-live
	IPMSG_TTL_MIN,
	IPMSG_TTL_MAX,

	// Tests for protocol
	IPMSG_PRO_UNSUP,
	IPMSG_PRO_ICMP_TOOSMALL,
	IPMSG_PRO_IGMP_TOOSMALL,
	IPMSG_PRO_TCP_TOOSMALL,
	IPMSG_PRO_UDP_TOOSMALL,
	IPMSG_PRO_UNSUP_TOOSMALL,

	// Tests for checksum
	IPMSG_CHECKSUM_BAD,

	// Tests for options
	IPMSG_OPT_ABOVEAVAIL,
	IPMSG_OPT_EXACTSIZE,
	IPMSG_OPT_ENDOFOPSLAST,
	IPMSG_OPT_ENDOFOPSFIRST,

	// Tests for fragmentation
	IPMSG_FRAG_MAXSIMREASM,
	IPMSG_FRAG_ABOVEMAXSIMREASM,
	IPMSG_FRAG_FULLREVERSE,
	IPMSG_FRAG_MIDREVERSE,
	IPMSG_FRAG_LASTINSECOND,
	IPMSG_FRAG_MIXEDSIZE,
	IPMSG_FRAG_HUNDRED,
	IPMSG_FRAG_MULTILASTFRAG,
	IPMSG_FRAG_OVERLAPPING,
	IPMSG_FRAG_MAXDGRAMSIZE,
	IPMSG_FRAG_MAXREASMSIZE,
	IPMSG_FRAG_ABOVEMAXREASMSIZE,
	IPMSG_FRAG_OVERSIZED,

	// Net attacks
	IPMSG_ATTACK_TEARDROP,
	IPMSG_ATTACK_NEWTEAR,
	IPMSG_ATTACK_IMPTEAR,
	IPMSG_ATTACK_SYNDROP,
	IPMSG_ATTACK_BONK,
	IPMSG_ATTACK_ZEROLENOPT,
	IPMSG_ATTACK_NESTEA,

	IPMSG_MAXTEST
} IPMSG_TESTTYPES, *PIPMSG_TESTTYPES;

// IP test request packet
typedef struct _IP_TESTREQ {
    DWORD				dwMessageId;	// Specifies the message id
	IPMSG_TESTTYPES		TestId;			// Specifies the test requested
	CHAR				MacAddr[16];	// Specifies the MAC address of the client (make size a multiple of 4)
} IP_TESTREQ, *PIP_TESTREQ;

// IP test request ack
typedef struct _IP_TESTACK {
    DWORD				dwMessageId;	// Specifies the message id
	IPMSG_TESTTYPES		TestId;			// Specifies the test requested
} IP_TESTACK, *PIP_TESTACK;

#endif