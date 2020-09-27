/*===
		Direct Network Protocl   --   Frame format header file


		Evan Schrier	10/98

*/



/*	
		Direct Network Protocol

| MEDIA HEADER | Var Len DN Header | Client Data |

There are two types of packets that may be exchanged between Direct Network
endpoints:

	Data Packets				(D Frame)	User data transmission
	Control Packets				(C Frame)	Internal link-state packets with no user data



*/

/*
	COMMAND FIELD

		The command field is the first byte of all frames.  The first BIT of the command frame is always
	the COMMAND FRAME vs DATA FRAME opcode.  The seven high bits of the Command field are flags.  We have
	a requirement that the command field of all protocol packets must never be all zeros.  Therefore,  when
	the opcode bit is zero (COMMAND FRAME) we must be sure that one flag bit is always set.  The highest flag bit,
	the USER2 flag is not relevant to COMMAND frames so we will always set the most significant bit when the opcode
	bit is zero.

		The seven command field flag bits are defined as follows:

	RELIABLE	-	Data delivery of this frame is guarenteed
	SEQUENTIAL	-	Data in this frame must be delivered in the order it was sent, with respect to other SEQ frames
	POLL		-	Protocol requests an immediate acknowledgement to this frame
	NEW MESSAGE	-	This frame is the first or only frame in a message
	END MESSAGE -	This frame is the last or only frame in a message
	USER1		-	First flag controlled by the higher layer (direct play core)
	USER2		-	Second flag controlled by core.  These flags are specified in the send API and are delivered with the data


	DATA FRAMES

		Data frames are between 4 and 20 bytes in length.  They should typically be only 4 bytes.  Following the
	Command byte in all data frames in the Control byte.  This byte contains a 3-bit retry counter and 5 additional
	flags.  The Control byte flags are defined as follows:

	END OF STREAM	-	This frame is the last data frame the transmitting partner will send.
	SACK_MASK_ONE	-	The low 32-bit Selective Acknowledgment mask is present in this header
	SACK_MASK_TWO	-	The high 32-bit Selective Acknowledgment mask is present in this header
	SEND_MASK_ONE	-	The low 32-bit Cancel Send mask is present in this header
	SEND_MASK_TWO	-	The high 32-bit Cancel Send mask is present in this header

		After Control byte come two one byte values:  Sequence number for this frame, and Next Receive sequence number
	expected by this partner.  After these two bytes comes zero through four bitmasks as specified by the control flags.
	After the bitmasks,  the rest of the frame is user data to be delivered to the direct play core.
*/
#ifndef	_DNET_FRAMES_
#define	_DNET_FRAMES_

//#define		FRAME_NAMES
//#define		FRAME_DROPS

#ifdef	FRAME_NAMES

#define		DG_NAME			'MRGD'
#define		RELY_NAME		'DNES'
#define		RETRY_NAME		'YRTR'
#define		ACK_NAME		' KCA'
#define		DG_ACK_NAME		'KCAD'
#define		COMBI_ACK_NAME	'KCAC'
#define		CONNECT_NAME	'LLAC'
#define		CONNECTED_NAME	'NNOC'
#define		DISCON_NAME		'CSID'
#define		DISCONED_NAME	'ENOD'

#define		POLL_NAME		'LLOP'
#define		RESPONSE_NAME	'PSER'

#define		SELECT_NAME		'YTRS'				// Selective Ack
#define		KEEPALIVE_NAME	'EKOP'	

#endif
#ifdef	FRAME_DROPS

#define		GOOD_STATE		'DOOG'
#define		DROP_STATE		'PORD'

#endif

/*
	Command FRAME Extended Opcodes

	A CFrame without an opcode is a vehicle for non-piggybacked acknowledgement
	information.  The following sub-commands are defined at this time:

	NOP				- Only Ack/Nack info present
	CONNECT 		- Initialize a reliable connection
	CONNECTED		- Response to CONNECT request
	DISCONNECT		- Closing previous connection
	DISCONNECTED	- Response to DISCONNECT request
	REJECT			- Command rejected (connection refused,  invalid connection)
*/

#define		FRAME_EXOPCODE_CONNECT			1
#define		FRAME_EXOPCODE_CONNECTED		2
#define		FRAME_EXOPCODE_DISCONNECTED		4
#define		FRAME_EXOPCODE_SACK			6


typedef struct packetheader			PacketHeader, *PPacketHeader;
typedef struct dataframe			DFRAME, *PDFRAME;
typedef struct cframe				CFRAME, *PCFRAME;
typedef struct sackframe			SACKFRAME, *PSACKFRAME;
typedef struct sackframe_big		SFBIG, *PSFBIG;

typedef	struct dataframe_masks		DFMASKS, *PDFMASKS;
typedef	struct dataframe_big		DFBIG, *PDFBIG;

//	Packet Header is common to all frame formats

#define	PACKET_COMMAND_DATA			0x01		// Frame contains user data
#define	PACKET_COMMAND_RELIABLE		0x02		// Frame should be delivered reliably
#define	PACKET_COMMAND_SEQUENTIAL	0x04		// Frame should be indicated sequentially
#define	PACKET_COMMAND_POLL			0x08		// Partner should acknowlege immediately
#define	PACKET_COMMAND_NEW_MSG		0x10		// Data frame is first in message
#define	PACKET_COMMAND_END_MSG		0x20		// Data frame is last in message
#define	PACKET_COMMAND_USER_1		0x40		// First user controlled flag
#define	PACKET_COMMAND_USER_2		0x80		// Second user controlled flag
#define	PACKET_COMMAND_CFRAME		0x80		// Set high-bit on command frames because first byte must never be zero

#define	PACKET_CONTROL_RETRY		0x01		// This flag designates this frame as a retry of a previously xmitted frame
#define	PACKET_CONTROL_CORRELATE	0x02		// Respond to this frame with a dedicated reply
#define	PACKET_CONTROL_RESPONSE		0x04		// THIS IS NOT CURRENTLY IMPLEMENTED - CORR RESPONSES WILL USE DEDICATED FRAMES
#define	PACKET_CONTROL_END_STREAM	0x08		// This packet serves as Disconnect frame.
#define	PACKET_CONTROL_SACK_MASK1	0x10		// The low 32-bit SACK mask is included in this frame.
#define	PACKET_CONTROL_SACK_MASK2	0x20		// The high 32 bit SACK mask is present
#define	PACKET_CONTROL_SEND_MASK1	0x40		// The low 32-bit SEND mask is included in this frame
#define	PACKET_CONTROL_SEND_MASK2	0x80		// The high 32-bit SEND mask is included in this frame

#define	PACKET_CONTROL_VARIABLE_MASKS	0xF0	// All four mask bits above

struct	packetheader {
#ifdef	FRAME_NAMES
	DWORD		Name;				// DEBUG FRAME NAME.  I will remove this when I have a sniffer parser
#endif
#ifdef	FRAME_DROPS
	DWORD		State;
#endif
	BYTE		bCommand;
};

/*		NEW DATA FRAMES
**
**		Here in the new unified world we have only two frame types!  CommandFrames and DataFrames...
**
*/

struct	dataframe {
#ifdef	FRAME_NAMES
	DWORD	Name;
#endif
#ifdef	FRAME_DROPS
	DWORD	State;
#endif
	BYTE	bCommand;
	BYTE	bControl;
	BYTE	bSeq;
	BYTE	bNRcv;
};

//	Following the 4 byte dataframe header will be between zero and four 32-bit masks,  as defined by the CONTROL flags,
// representing either Specific Acks (SACK) or unreliable dropped sends (SEND MASK).

struct	dataframe_masks {
	ULONG	rgMask[1];			// Zero to four 32-bit masks of either SACK info or SEND info
};

struct dataframe_big {
#ifdef	FRAME_NAMES
	DWORD	Name;
#endif
#ifdef	FRAME_DROPS
	DWORD	State;
#endif
	BYTE	bCommand;
	BYTE	bControl;
	BYTE	bSeq;
	BYTE	bNRcv;
	ULONG	rgMask[4];
};

/*	
**		COMMAND FRAMES
**
**		Command frames are everything that is not part of the reliable stream.  This is most of the control traffic
**	although some control traffic is part of the stream (keep-alive, End-of-Stream)
*/

struct	cframe {
#ifdef	FRAME_NAMES
	DWORD	Name;					// DEBUG FRAME NAME.  I will remove this when I have a sniffer parser
#endif
#ifdef	FRAME_DROPS
	DWORD	State;
#endif
	BYTE	bCommand;
	BYTE	bExtOpcode;				// CFrame sub-command
	BYTE	bMsgID;					// Correlator in case ExtOpcode requires a response
	BYTE	bRspID;					// Correlator in case this is a response
	DWORD	dwVersion;				// Protocol version #
	DWORD	dwSessID;				// Session identifier
	DWORD	tTimestamp;				// local tick count
};

/*	
**	Selective Acknowledgement packet format
**
**		When a specific acknowledgement frame is sent there may be two additional pieces
**	of data included with the frame.  One is a bitmask allowing selective acknowledgment
**	of non-sequential frames.  The other is timing information about the last frame acked
**  by this ACK (NRcv - 1).  Specifically,  it includes the lowest Retry number that this
**  node received,  and the ms delay between that packets arrival and the sending of this
**	Ack.
*/


#define		SACK_FLAGS_RESPONSE			0x01	// indicates that Retry and Timestamp fields are valid
#define		SACK_FLAGS_SACK_MASK1		0x02
#define		SACK_FLAGS_SACK_MASK2		0x04
#define		SACK_FLAGS_SEND_MASK1		0x08
#define		SACK_FLAGS_SEND_MASK2		0x10

//	First format is used when DATAGRAM_INFO flag is clear

struct	sackframe {	
#ifdef	FRAME_NAMES
	DWORD		Name;					// DEBUG FRAME NAME.  I will remove this when I have a sniffer parser
#endif
#ifdef	FRAME_DROPS
	DWORD		State;
#endif
	BYTE		bCommand;				// As above
	BYTE		bExtOpcode;				// As above
	BYTE		bFlags;					// Additional flags for sack frame
	BYTE		bRetry;
	BYTE		bNSeq;					// Since this frame has no sequence number, this is the next Seq we will send
	BYTE		bNRcv;					// As above
	DWORD		tTimestamp;				// Local timestamp when packet (NRcv - 1) arrived
};

struct sackframe_big {
#ifdef	FRAME_NAMES
	DWORD		Name;					// DEBUG FRAME NAME.  I will remove this when I have a sniffer parser
#endif
#ifdef	FRAME_DROPS
	DWORD		State;
#endif
	BYTE		bCommand;				// As above
	BYTE		bExtOpcode;				// As above
	BYTE		bFlags;					// Additional flags for sack frame
	BYTE		bRetry;
	BYTE		bNSeq;					// Since this frame has no sequence number, this is the next Seq we
	BYTE		bNRcv;					// As above
	DWORD		tTimestamp;				// Local stamp when packet arrived
	ULONG		rgMask[4];
};

#ifdef	OLD_CODE

//	Reliable Stream packet format
//
//	If the SACK Flag is set in an SFrame then a DWORD of ack flags are present
//	after N(S) & N(R) and before the payload.

struct	sframe {
#ifdef	FRAME_NAMES
	DWORD		Name;				// DEBUG FRAME NAME.  I will remove this when I have a sniffer parser
#endif
#ifdef	FRAME_DROPS
	DWORD		State;				// DEBUG Allow frames to be "damaged" to simulate drops
#endif
	BYTE		bCommand;
	BYTE		bRetry;
	USHORT		usSeq;				// Sequence number of this frame N(S)
	USHORT		usNRcv;				// Next in-order sequence number expected N(R)
	USHORT		usDrops;			// Total number of datagrams detected as dropped by this station
};

//	Unreliable Datagram - single frame message limit means less state needed in message...

struct	uframe {
#ifdef	FRAME_NAMES
	DWORD		Name;				// DEBUG FRAME NAME.  I will remove this when I have a sniffer parser
#endif
#ifdef	FRAME_DROPS
	DWORD		State;
#endif
	BYTE	bCommand;
	BYTE	bMsgID;					// Used to correlate replies and to detect lost frames, even tho they're not acked
};

//	Unreliable response frame - additional fields in a UFrame if RESP flag is set

struct respframe {
	BYTE	bRspID;					// valid if RESP flag is set: Msg ID of paired Poll
	BYTE	bResv;					// reserved byte
	USHORT	usDrops;				// total number of frames detected as lost (partner will compute difference)
	USHORT	usDelay;				// ms delay between arrival of POLL frame and transmission of this RESP
	DWORD	tTimestamp;
//	BYTE	data[];
};

struct uframe_rsp {
	UFRAME		UFrame;
	RESPFRAME	Resp;
};


//	This format is used when DATAGRAM_INFO flag is set

struct	sackframe_datagram {
#ifdef	FRAME_NAMES
	DWORD		Name;					// DEBUG FRAME NAME.  I will remove this when I have a sniffer parser
#endif
#ifdef	FRAME_DROPS
	DWORD		State;
#endif
	BYTE		bCommand;				// As above
	BYTE		bExtOpcode;				// As above
	BYTE		bFlags;					// Additional flags for sack frame
	BYTE		bRetry;					// Retry code of frame we are acknowledging
	USHORT		usNSeq;					// As above
	USHORT		usNRcv;					// As above
	DWORD		tTimestamp;				// Local stamp when packet arrived
	DWORD		dwDelay;				// ms count since identified frame arrived
	ULONG		ulMask;					// Bitmask SACKing frames to left of bNSeq
	ULONG		ulMaskHigh;				// 2nd 32 bits of out of order mask
	RESPFRAME	DatagramResp;
};

#endif
#endif
