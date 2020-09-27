/*++

Copyright (c) 1996 Microsoft Corporation

Module Name:

    rtp.h

Abstract:

    Header for RTP/RTCP Protocol.

--*/
 
#if !defined(_INC_RTP_H_)
#define      _INC_RTP_H_

#define RTP_TYPE     2
#define RTP_VERSION  RTP_TYPE
#define RTP_MAX_SDES 256
#define RTP_MAX_EKEY 32

#define	NUM_COLLISION_ENTRIES	10
#define	MAX_ADDR_LEN			80

typedef struct _RTP_SDES_ITEM {
    BYTE Type;
    BYTE TextLength;
    BYTE Text[RTP_MAX_SDES];
} RTCP_SDES_ITEM, *PRTCP_SDES_ITEM;

// RTCP SDES (Source DEScription) types, as described in RFC1889
typedef enum
{
	RTCP_SDES_END,   // END:   SDES Items terminator
	RTCP_SDES_CNAME, // CNAME: Canonical end-point identifier SDES item
	RTCP_SDES_NAME,  // NAME:  User name SDES item
	RTCP_SDES_EMAIL, // EMAIL: Electronic mail address SDES item
	RTCP_SDES_PHONE, // PHONE: Phone number SDES item
	RTCP_SDES_LOC,   // LOC:   Geographic user location SDES item
	RTCP_SDES_TOOL,  // TOOL:  Application or tool name SDES item
	RTCP_SDES_NOTE,  // NOTE:  Notice/status SDES item
	RTCP_SDES_PRIV,  // PRIV:  Private extensions SDES
	RTCP_SDES_LAST   // just a count of the number of items
} RTCP_SDES_TYPE_T;

typedef struct _RTCP_SENDER_REPORT {
    DWORD NtpTimestampSec;
    DWORD NtpTimestampFrac;
    DWORD RtpTimestamp;
    DWORD TotalPackets;
    DWORD TotalOctets;
} RTCP_SENDER_REPORT, *PRTCP_SENDER_REPORT;
    
typedef struct _RTCP_RECEIVER_REPORT {
    DWORD FractionLost:8;
    DWORD TotalLostPackets:24;
    DWORD HighestSequenceNum;
    DWORD InterarrivalJitter;
    DWORD LastSRTimestamp;
    DWORD DelaySinceLastSR;
} RTCP_RECEIVER_REPORT, *PRTCP_RECEIVER_REPORT;

typedef struct _RTCP_PARTICIPANT_REPORT {
    DWORD                SSRC;
    RTCP_SENDER_REPORT   LastSR;
    RTCP_RECEIVER_REPORT LastIncomingRR;
    RTCP_RECEIVER_REPORT LastOutgoingRR;
} RTCP_PARTICIPANT_REPORT, *PRTCP_PARTICIPANT_REPORT;
    
typedef struct _RTP_HEADER {
//--- NETWORK BYTE ORDER BEGIN ---//
    WORD  NumCSRC:4;                                      
    WORD  fExtHeader:1;                                   
    WORD  fPadding:1;                                     
    WORD  Version:2;                                      
    WORD  PayloadType:7;
    WORD  fMarker:1;
//---- NETWORK BYTE ORDER END ----//
    WORD  SequenceNum;
    DWORD Timestamp;
    DWORD SSRC;
} RTP_HEADER, *PRTP_HEADER;

typedef struct _RTP_HEADER_X {
    WORD  Identifier;
    WORD  DataLength;
    DWORD Data[1];
} RTP_HEADER_X, *PRTP_HEADER_X;

typedef struct _RTP_ENCRYPTION_INFO {
    DWORD Scheme;
    DWORD Key[RTP_MAX_EKEY];
} RTP_ENCRYPTION_INFO, *PRTP_ENCRYPTION_INFO;


/////////////////////////////////////////////////
// DXMRTP RTP/RTCP events
/////////////////////////////////////////////////
// The real event received is e.g. for "new source",
// DXMRTP_EVENTBASE + DXMRTP_NEW_SOURCE_EVENT
/////////////////////////////////////////////////

#define B2M(b) (1 << (b))

#define DXMRTP_EVENTBASE (EC_USER+0)

typedef enum 
{
	DXMRTP_NO_EVENT,
	DXMRTP_NEW_SOURCE_EVENT,			// New SSRC detected
	DXMRTP_RECV_RTCP_RECV_REPORT_EVENT,	// RTCP RR received
	DXMRTP_RECV_RTCP_SNDR_REPORT_EVENT,	// RTCP SR received
	DXMRTP_LOCAL_COLLISION_EVENT,		// Collision detected
	DXMRTP_REMOTE_COLLISION_EVENT,		// Remote collision detected
	DXMRTP_TIMEOUT_EVENT,				// SSRC timed-out
	DXMRTP_BYE_EVENT,					// RTCP Bye received
	DXMRTP_RTCP_WS_RCV_ERROR,			// Winsock error on RTCP rcv
	DXMRTP_RTCP_WS_XMT_ERROR, 			// Winsock error on RTCP xmt
	DXMRTP_INACTIVE_EVENT,				// SSRC has been silent
	DXMRTP_ACTIVE_AGAIN_EVENT,			// SSRC has been heard again
    DXMRTP_LOSS_RATE_RR_EVENT,          // Loss rate as reported in an RR
    DXMRTP_LOSS_RATE_LOCAL_EVENT,       // Loss rate locally detected
	DXMRTP_LAST_EVENT
} DXMRTP_EVENT_T;

typedef struct _SDES_DATA
{
	DWORD				dwSdesType;			// SDES type: CNAME/NAME/...
	char				sdesBfr[RTP_MAX_SDES];
	DWORD				dwSdesLength;		// SDES length
	DWORD				dwSdesFrequency;	// SDES frequency
	DWORD				dwSdesEncrypted;	// SDES encrypted Y/N ?
} SDES_DATA, *PSDES_DATA;

/////////////////////////////////////////////////
// DXMRTP QOS events
/////////////////////////////////////////////////
// The real event received is e.g. for "receivers",
// DXMRTP_QOSEVENTBASE + DXMRTP_QOSEVENT_RECEIVERS
/////////////////////////////////////////////////

#define DXMRTP_QOSEVENTBASE (DXMRTP_EVENTBASE + 32)

//
// NOTE!:
// Other events may be added related to trying to set up QoS
// (before any QoS event could be fired)
//
typedef enum
{
	DXMRTP_QOSEVENT_NOQOS,
	/* no QoS support is available */
	DXMRTP_QOSEVENT_RECEIVERS,
	/* at least one Reserve has arrived */
	DXMRTP_QOSEVENT_SENDERS,
	/* at least one Path has arrived */
	DXMRTP_QOSEVENT_NO_SENDERS,
	/* there are no senders */
	DXMRTP_QOSEVENT_NO_RECEIVERS,
	/* there are no receivers */
	DXMRTP_QOSEVENT_REQUEST_CONFIRMED,
	/* Reserve has been confirmed */
	DXMRTP_QOSEVENT_ADMISSION_FAILURE,
	/* error due to lack of resources */
	DXMRTP_QOSEVENT_POLICY_FAILURE,
	/* rejected for administrative reasons - bad credentials */
	DXMRTP_QOSEVENT_BAD_STYLE,
	/* unknown or conflicting style */
	DXMRTP_QOSEVENT_BAD_OBJECT,
	/* problem with some part of the filterspec or providerspecific
	 * buffer in general */
	DXMRTP_QOSEVENT_TRAFFIC_CTRL_ERROR,
	/* problem with some part of the flowspec */
	DXMRTP_QOSEVENT_GENERIC_ERROR,
	/* general error */
	DXMRTP_QOSEVENT_NOT_ALLOWEDTOSEND,
	/* sender is not allowed to send */
	DXMRTP_QOSEVENT_ALLOWEDTOSEND,
	/* sender is not allowed to send */
	DXMRTP_QOSEVENT_LAST
} DXMRTP_QOSEVENT_T;

/////////////////////////////////////////////////
// DXMRTP DEMUX events
/////////////////////////////////////////////////
// The real event received is e.g. for "ssrc mapped",
// RTPDMX_EVENTBASE + RTPDEMUX_SSRC_MAPPED
/////////////////////////////////////////////////
#if !defined(RTPDMX_EVENTBASE)
#define RTPDMX_EVENTBASE (EC_USER+100)

typedef enum {
	RTPDEMUX_SSRC_MAPPED,       // The specific SSRC has been mapped
	RTPDEMUX_SSRC_UNMAPPED,     // The specific SSRC has been unmapped
	RTPDEMUX_PIN_MAPPED,        // The specific Pin has been mapped
	RTPDEMUX_PIN_UNMAPPED,      // The specific Pin has been unmapped
	RTPDEMUX_NO_PINS_AVAILABLE, // PT was found, but pin was already mapped
	RTPDEMUX_NO_PAYLOAD_TYPE    // PT was not found
} RTPDEMUX_EVENT_t;
// The Pin passed as a parameter along with PIN_MAPPED and PIN_UNMAPPED
// is a pointer to the connected pin

#endif

// Maximum number of classes per CPU in
// in the RTP Source Shared Thread scheme
#define RTP_MAX_CLASSES 4

// Actually two classes are defined
enum {
	RTP_CLASS_AUDIO,
	RTP_CLASS_VIDEO
};

// Reservation styles
enum {
	DXMRTP_RESERVE_WILCARD,  // Usually for N times the AUDIO flow spec
	DXMRTP_RESERVE_EXPLICIT  // Usually for designated VIDEO streams
};

#endif // _INC_RTP



