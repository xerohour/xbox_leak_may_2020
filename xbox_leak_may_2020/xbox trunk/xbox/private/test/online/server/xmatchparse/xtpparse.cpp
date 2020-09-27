//============================================================================
//  FILE: xtpparse.c
//
//  Description: Xbox Authenticated Tunneling Protocol (XTP) Parser
//============================================================================

#include "stdafx.h"

//==================================================================================
// Defines
//==================================================================================
// Values for IP protocols recognized by Netmon
#define PROTO_ICMP_VALUE	1
#define PROTO_IGMP_VALUE	2
#define PROTO_IP_VALUE		4
#define PROTO_TCP_VALUE		6
#define PROTO_UDP_VALUE		17
#define PROTO_IP6_VALUE		41
#define PROTO_RSVP_VALUE	46
#define PROTO_GRE_VALUE		47
#define PROTO_ESP_VALUE		50
#define PROTO_AH_VALUE		51
#define PROTO_OSPF_VALUE	89

#define PROTO_NAME_MAX		8

// Friendly strings for IP protocols recognized by Netmon
#define PROTO_ICMP_NAME		"ICMP"
#define PROTO_IGMP_NAME		"IGMP"
#define PROTO_IP_NAME		"IP"
#define PROTO_TCP_NAME		"TCP"
#define PROTO_UDP_NAME		"UDP"
#define PROTO_IP6_NAME		"IPv6"
#define PROTO_RSVP_NAME		"RSVP"
#define PROTO_GRE_NAME		"GRE"
#define PROTO_ESP_NAME		"ESP"
#define PROTO_AH_NAME		"AH"
#define PROTO_OSPF_NAME		"OSPF"
#define PROTO_UNKNOWN_NAME	"Unknown"

//==================================================================================
// Globals
//==================================================================================
HPROTOCOL hXTP = NULL;
DWORD     Attached = 0;

// structure for decoding the header -----------------------------------------
typedef struct _XTPHEADER 
{
    DWORD		AuthData[3];
	DWORD		SourceAddr;
	DWORD		DestAddr;
	union
	{
		struct _GENFIELD {
			DWORD		Reserved1	: 4;
			DWORD		IHL			: 4;
			DWORD		Protocol	: 8;
			DWORD		Reserved2	: 16;
		} GenField;

		struct _UDPFIELD {
			WORD		SourcePort;
			WORD		DestPort;
		} UdpField;
	} VariableField;
	DWORD		SequenceNum	: 31;
	DWORD		UdpFlag		: 1;

} XTPHEADER, *PXTPHEADER;

//==================================================================================
// Functions
//==================================================================================
extern PPF_PARSERDLLINFO WINAPI ParserAutoInstallInfo();
extern VOID   WINAPI XTP_Register( HPROTOCOL hXTP);
extern VOID   WINAPI XTP_Deregister( HPROTOCOL hXTP);
extern LPBYTE WINAPI XTP_RecognizeFrame( HFRAME hFrame, 
                                              LPBYTE pMacFrame, 
                                              LPBYTE pXTPFrame, 
                                              DWORD MacType, 
                                              DWORD BytesLeft, 
                                              HPROTOCOL hPrevProtocol, 
                                              DWORD nPrevProtOffset,
                                              LPDWORD pProtocolStatus,
                                              LPHPROTOCOL phNextProtocol, 
                                              LPDWORD InstData);
extern LPBYTE WINAPI XTP_AttachProperties( HFRAME hFrame, 
                                                LPBYTE pMacFrame, 
                                                LPBYTE pXTPFrame, 
                                                DWORD MacType, 
                                                DWORD BytesLeft, 
                                                HPROTOCOL hPrevProtocol, 
                                                DWORD nPrevProtOffset,
                                                DWORD InstData);
extern DWORD  WINAPI XTP_FormatProperties( HFRAME hFrame, 
                                                LPBYTE pMacFrame, 
                                                LPBYTE pXTPFrame, 
                                                DWORD nPropertyInsts, 
                                                LPPROPERTYINST p);

VOID WINAPIV XTP_FormatSummary( LPPROPERTYINST pPropertyInst);

// Define the entry points that we will pass back at dll entry time ----------
ENTRYPOINTS XTPEntryPoints =
{
    // XTP Entry Points
    XTP_Register,
    XTP_Deregister,
    XTP_RecognizeFrame,
    XTP_AttachProperties,
    XTP_FormatProperties
};


//==================================================================================
// Property labels
//==================================================================================
LABELED_BIT XTPUDPFlag[] =
{
    // bit 7 = Is this the UDP special case
    {   7,
	    "UDP special case",
	    "Generic IP case",
    }
};
SET XTPUdpFlagSET = {sizeof(XTPUDPFlag)/sizeof(LABELED_BIT), XTPUDPFlag};



//==================================================================================
// Properties
//==================================================================================
typedef enum
{
    XTP_SUMMARY = 0,
    XTP_AUTHDATA,
	XTP_ORIGSOURCEADDR,
	XTP_ORIGDESTADDR,
	XTP_ORIGSOURCEPORT,
	XTP_ORIGDESTPORT,
	XTP_RESERVED1,
	XTP_ORIGIHL,
	XTP_ORIGPROTO,
	XTP_RESERVED2,
	XTP_SEQNUM,
	XTP_UDPFLAG,
};

PROPERTYINFO  XTPPropertyTable[] = 
{
    // XTP_SUMMARY
    { 0, 0,
      "Summary",
      "Summary of the XTP Packet",
      PROP_TYPE_SUMMARY,
      PROP_QUAL_NONE,
      NULL,
      80,
      XTP_FormatSummary
    },

    // XTP_AUTHDATA
    { 0, 0,
      "Auth data",
      "Authentication data",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XTP_ORIGSOURCEADDR
    { 0, 0,
      "Source IP",
      "Source address of original IP packet",
      PROP_TYPE_IP_ADDRESS,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XTP_ORIGDESTADDR
    { 0, 0,
      "Destination IP",
      "Destination address of original IP packet",
      PROP_TYPE_IP_ADDRESS,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XTP_ORIGSOURCEPORT
    { 0, 0,
      "Source port",
      "Source port of original UDP packet",
      PROP_TYPE_WORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XTP_ORIGDESTPORT
    { 0, 0,
      "Destination port",
      "Destination port of original UDP packet",
      PROP_TYPE_WORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XTP_RESERVED1
    { 0, 0,
      "Reserved",
      "Reserved for future use",
      PROP_TYPE_BYTE,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XTP_ORIGIHL
    { 0, 0,
      "IHL",
      "Internet header length of original IP packet",
      PROP_TYPE_BYTE,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XTP_ORIGPROTO
    { 0, 0,
      "Protocol",
      "Protcol type of original IP packet",
      PROP_TYPE_BYTE,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance	// BUGBUG - Probably want a new format for this to show the protocol name
    },

    // XTP_RESERVED2
    { 0, 0,
      "Reserved",
      "Reserved for future use",
      PROP_TYPE_WORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XTP_SEQNUM
    { 0, 0,
      "Sequence number",
      "Authentication sequence number",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XTP_UDPFLAG
    { 0, 0,
      "UDP flag",
      "Flag that specifies if this is a UDP packet special case",
      PROP_TYPE_BYTE,
      PROP_QUAL_FLAGS,
      &XTPUdpFlagSET,
      80,
      FormatPropertyInstance
    }

};

DWORD nNumXTPProps = (sizeof(XTPPropertyTable)/sizeof(PROPERTYINFO));

//==================================================================================
// DllMain
//----------------------------------------------------------------------------------
//
// Description: (Called by the OS) Tell the kernel about our entry points.
//
// Returns: Always returns TRUE
//==================================================================================
BOOL WINAPI DllMain( HANDLE hInstance, ULONG Command, LPVOID Reserved)
{

    // what type of call is this
    switch( Command )
    {
        case DLL_PROCESS_ATTACH:
            // are we loading for the first time?
            if( Attached == 0 )
            {
                // the first time in we need to tell the kernel 
                // about ourselves
                hXTP = CreateProtocol( "XTP", 
                                            &XTPEntryPoints, 
                                            ENTRYPOINTS_SIZE);
            }
            Attached++;
            break;

        case DLL_PROCESS_DETACH:
            // are we detaching our last instance?
            Attached--;
            if( Attached == 0 )
            {
                // last guy out needs to clean up
                DestroyProtocol( hXTP);
            }
            break;
    }

    // Netmon parsers ALWAYS return TRUE.
    return TRUE;
}

//==================================================================================
// ParserAutoInstallInfo
//----------------------------------------------------------------------------------
//
// Description: Function called by Netmon to automatically install the parser
//
// Arguments: none
//
// Returns: On success, returns a pointer to a PF_PARSERDLLINFO structure containing
//	information on the parsers in this DLL.  Otherwise, returns NULL
//==================================================================================
PPF_PARSERDLLINFO WINAPI ParserAutoInstallInfo() 
{
    PPF_PARSERDLLINFO pParserDllInfo; 
    PPF_PARSERINFO    pParserInfo;
    DWORD dwNumProtocols;

    PPF_HANDOFFSET    pIncomingHandoff;
    PPF_HANDOFFENTRY  pHandoffEntry;
    DWORD dwIncomingHandoffs;
    // BUGBUG - later, the hand-off set for this protocol will need to be fleshed out
    /*
    PPF_HANDOFFSET    pOutgoingHandoff;
    DWORD dwNumOutgoingHandoffs;
    */

    // Base structure ========================================================

    // Allocate memory for parser info:
    dwNumProtocols = 1;
    pParserDllInfo = (PPF_PARSERDLLINFO)HeapAlloc( GetProcessHeap(),
                                                   HEAP_ZERO_MEMORY,
                                                   sizeof( PF_PARSERDLLINFO ) +
                                                   dwNumProtocols * sizeof( PF_PARSERINFO) );
    if( pParserDllInfo == NULL)
    {
        return NULL;
    }       
    
    // Fill in the parser DLL info
    pParserDllInfo->nParsers = dwNumProtocols;

    // Fill in the individual parser infos...

    // Enter XTP information
    pParserInfo = &(pParserDllInfo->ParserInfo[0]);
    sprintf( pParserInfo->szProtocolName, "XTP" );
    sprintf( pParserInfo->szComment,      "Xbox Authenticated Tunneling Protocol" );
    sprintf( pParserInfo->szHelpFile,     "");

    // the incoming handoff set ----------------------------------------------
    // allocate
    dwIncomingHandoffs = 1;
    pIncomingHandoff = (PPF_HANDOFFSET)HeapAlloc( GetProcessHeap(),
                                             HEAP_ZERO_MEMORY,
                                             sizeof( PF_HANDOFFSET ) +
                                             dwIncomingHandoffs * sizeof( PF_HANDOFFENTRY) );
    if( pIncomingHandoff == NULL )
    {
        return pParserDllInfo;
    }

    // Fill in the incoming handoff set
    pParserInfo->pWhoHandsOffToMe = pIncomingHandoff;
    pIncomingHandoff->nEntries = dwIncomingHandoffs;

    // UDP PORT 500
    pHandoffEntry = &(pIncomingHandoff->Entry[0]);
    sprintf( pHandoffEntry->szIniFile,    "TCPIP.INI" );
    sprintf( pHandoffEntry->szIniSection, "UDP_HandoffSet" );
    sprintf( pHandoffEntry->szProtocol,   "XTP" );
	// BUGBUG - Use 500 for now, but this conflicts with ISAKMP
    pHandoffEntry->dwHandOffValue =        500;
    pHandoffEntry->ValueFormatBase =       HANDOFF_VALUE_FORMAT_BASE_DECIMAL;    

    // BUGBUG - Later, the outgoing hand-off set for this protocol will need to be
	// updated to include most major protocols (is there a way to piggy back off of TCP/IP?)

    return pParserDllInfo;
}

//==================================================================================
// XTP_Register
//----------------------------------------------------------------------------------
//
// Description: Creates the property database for XTP
//
// Arguments:
//	HPROTOCOL		hXTP			Handle to the XTP protocol
//
// Returns: void
//==================================================================================
void WINAPI XTP_Register( HPROTOCOL hXTP)
{
    WORD  i;

    // Tell Netmon to make reserve some space for our property table
    CreatePropertyDatabase( hXTP, nNumXTPProps);

    // Add our properties to the Netmon's database
    for( i = 0; i < nNumXTPProps; i++)
    {
        AddProperty( hXTP, &XTPPropertyTable[i]);
    }
}

//==================================================================================
// ParserAutoInstallInfo
//----------------------------------------------------------------------------------
//
// Description: Removes the property database for XTP
//
// Arguments:
//	HPROTOCOL		hXTP			Handle to the XTP protocol
//
// Returns: void
//==================================================================================
VOID WINAPI XTP_Deregister(HPROTOCOL hXTP)
{
    // Tell the Netmon that it may now free our database
    DestroyPropertyDatabase( hXTP);
}

//==================================================================================
// XTP_RecognizeFrame
//----------------------------------------------------------------------------------
//
// Description: Called by Netmon to determine if the parser recognizes this frame as XTP
//
// Arguments:
//	HFRAME			hFrame			Handle to the frame that contains the data.
//	LPBYTE			pMacFrame		Pointer to the first byte of a frame. The pointer 
//									provides a way to view data that other parsers recognize.
//	LPBYTE			pXTPFrame		Pointer to the start of the unclaimed data. Typically,
//									the unclaimed data is located in the middle of a frame
//									because a previous parser has claimed data before this
//									parser. The parser must test the unclaimed data first. 
//	DWORD			MacType			MAC value of the first protocol in a frame. Typically,
//									the MacType value is used when the parser must identify
//									the first protocol in a frame.
//	DWORD			BytesLeft		The remaining number of bytes from a location in a frame
//									to the end of the frame. 
//	HPROTOCOL		hPrevProtocol	Handle of the previous protocol. 
//	DWORD			nPrevProtOffset	Offset of the previous protocol - beginning of the frame.
//	LPDWORD			pProtocolStatus	Protocol status indicator.
//	LPHPROTOCOL		phNextProtocol	Pointer to the handle of the next protocol. This
//									parameter is set when a protocol identifies the protocol
//									that follows a protocol. To obtain the handle of the next
//									protocol, call the GetProtocolFromTable function. 
//	LPDWORD			InstData		On input, a pointer to the instance data from the
//									previous protocol. On output, a pointer to the instance
//									data for the current protocol. 
//
// Returns: When XTP protocol is recognized, either returns a pointer to the next byte of
//			unclaimed data or NULL to indicate there is no more protocol data in the packet.
//			When XTP protocol isn't recognized, returns the original pXTPFrame value to submit
//			the packet back to Netmon to find a proper protocol match
//==================================================================================
LPBYTE WINAPI XTP_RecognizeFrame(HFRAME      hFrame,         
								LPBYTE      pMacFrame,      
								LPBYTE      pXTPFrame, 
								DWORD       MacType,        
								DWORD       BytesLeft,      
								HPROTOCOL   hPrevProtocol,  
								DWORD       nPrevProtOffset,
								LPDWORD     pProtocolStatus,
								LPHPROTOCOL phNextProtocol,
								LPDWORD     InstData)       
{
	PXTPHEADER pXTPHeader = (PXTPHEADER) pXTPFrame;
	DWORD dwOptionsLen = 0;

	// If this is the general case, determine the size of the encapsulated IP header
	if(!(pXTPHeader->UdpFlag))
	{
		// If the IP header is too small, discard the packet without recognizing it
		if(pXTPHeader->VariableField.GenField.IHL < 5)
		{
			*pProtocolStatus = PROTOCOL_STATUS_NOT_RECOGNIZED;
			return pXTPFrame;
		}

		// The options len is the total header len minus the standard header len (5*4=20)
		dwOptionsLen = (pXTPHeader->VariableField.GenField.IHL - 5) * 4;
	}

	// If this frame isn't large enough to be an XTP frame, indicate that its not recognized
	if(BytesLeft < (sizeof(XTPHEADER) + dwOptionsLen))
	{
		*pProtocolStatus = PROTOCOL_STATUS_NOT_RECOGNIZED;
		return pXTPFrame;
	}

	// BUGBUG - in the future, verify that the passed in frame is actually an XTP frame
	// For now, just assume that since we're being called, it must be
    *pProtocolStatus = PROTOCOL_STATUS_CLAIMED;

	// BUGBUG - in the future, find out what the next protocol is.  Then, set the phNextProtocol
	// and the return point accordingly to indicate if we are followed by a parsable protocol
	pXTPFrame += (sizeof(XTPHEADER) + dwOptionsLen);
    return pXTPFrame;
}

//==================================================================================
// XTP_RecognizeFrame
//----------------------------------------------------------------------------------
//
// Description: Called by Netmon to determine if the parser recognizes this frame as XTP
//
// Arguments:
//	HFRAME			hFrame			Handle of the frame that is being parsed. 
//	LPBYTE			lpFrame			Pointer to the first byte in a frame.
//	LPBYTE			pXTPFrame		Pointer to the start of the recognized data.
//	DWORD			MacType			MAC value of the first protocol in a frame.
//	DWORD			BytesLeft		The remaining number of bytes in a frame, starting
//									at the beginning of the recognized data.
//	HPROTOCOL		hPrevProtocol	Handle of the previous protocol.
//	DWORD			nPrevProtOffset	Offset of the previous protocol, starting at the
//									beginning of the frame.
//	DWORD			InstData		Pointer to the instance data from the previous protocol.
//
// Returns: When the function is successful, either returns a pointer to the next byte of
//			unclaimed data or NULL to indicate there is no more protocol data in the packet.
//			When the function isn't successful, returns the original pXTPFrame value to
//			submit the packet back to Netmon to find a proper protocol match
//==================================================================================
LPBYTE WINAPI XTP_AttachProperties(	HFRAME      hFrame,         
									LPBYTE      pMacFrame,     
									LPBYTE      pXTPFrame,   
									DWORD       MacType,        
									DWORD       BytesLeft,      
									HPROTOCOL   hPrevProtocol,  
									DWORD       nPrevProtOffset,
									DWORD       InstData)       

{
    // Apply the header overlay in order to decode the protocol
    PXTPHEADER	pXTPHeader = (PXTPHEADER) pXTPFrame;
	DWORD		dwOptionsLen = 0, dwSequenceNum = 0;
	BYTE		bUdpFlag = 0;

	// If this is the general case, determine the size of the encapsulated IP header
	if(!(pXTPHeader->UdpFlag))
	{
		// If the IP header is too small, discard the frame
		if(pXTPHeader->VariableField.GenField.IHL < 5)
		{
			goto Exit;
		}

		// The options len is the total header len minus the standard header len (5*4=20)
		dwOptionsLen = (pXTPHeader->VariableField.GenField.IHL - 5) * 4;
	}

	// If this frame isn't large enough to be an XTP frame, discard it
	if(BytesLeft < (sizeof(XTPHEADER) + dwOptionsLen))
	{
		goto Exit;
	}

    //
    // Attach Properties...
    //

    // XTP summary
    if(!AttachPropertyInstance(hFrame, XTPPropertyTable[XTP_SUMMARY].hProperty, (WORD)BytesLeft,
		(LPBYTE)pXTPFrame, 0, 0, 0))
	{
		goto Exit;
	}

    // Authentication data
    if(!AttachPropertyInstance(hFrame, XTPPropertyTable[XTP_AUTHDATA].hProperty, sizeof(pXTPHeader->AuthData),
		(LPBYTE) pXTPHeader->AuthData, 0, 1, 0))
	{
		goto Exit;
	}

    // Original source IP address
    if(!AttachPropertyInstance(hFrame, XTPPropertyTable[XTP_ORIGSOURCEADDR].hProperty, sizeof(pXTPHeader->SourceAddr),
		(LPBYTE) &(pXTPHeader->SourceAddr), 0, 1, 0))
	{
		goto Exit;
	}

    // Original destination IP address
    if(!AttachPropertyInstance(hFrame, XTPPropertyTable[XTP_ORIGDESTADDR].hProperty, sizeof(pXTPHeader->DestAddr),
		(LPBYTE) &(pXTPHeader->DestAddr), 0, 1, 0))
	{
		goto Exit;
	}

	// For the UDP special case, interpret the next DWORD as a source and destination port
	if(pXTPHeader->UdpFlag)
	{
		// Original source UDP port
		if(!AttachPropertyInstance(hFrame, XTPPropertyTable[XTP_ORIGSOURCEPORT].hProperty,
			sizeof(pXTPHeader->VariableField.UdpField.SourcePort),
			(LPBYTE) &(pXTPHeader->VariableField.UdpField.SourcePort), 0, 1, 0))
		{
			goto Exit;
		}
		
		// Original destination UDP port
		if(!AttachPropertyInstance(hFrame, XTPPropertyTable[XTP_ORIGDESTPORT].hProperty,
			sizeof(pXTPHeader->VariableField.UdpField.DestPort),
			(LPBYTE) &(pXTPHeader->VariableField.UdpField.DestPort), 0, 1, 0))
		{
			goto Exit;
		}
	}
	// For the generic case, interpret the next DWORD as the IHL and protocol of the encapsulated IP packet
	else
	{
		BYTE bReserved1 = 0, bIHL = 0, bProtocol = 0;
		WORD wReserved2 = 0;

		// Many of the fields in the general case header will wind up overlapping since they are
		// less than the size of a byte and Netmon only allows properties of a byte or larger
		// AttachPropertyInstanceEx is used to accurately display the values of overlapped fields

		// Go ahead and retrieve the correct values before mucking with the frame
		bReserved1 = (BYTE) pXTPHeader->VariableField.GenField.Reserved1;
		bIHL = (BYTE) pXTPHeader->VariableField.GenField.IHL;
		bProtocol = (BYTE) pXTPHeader->VariableField.GenField.Protocol;
		wReserved2 = (WORD) pXTPHeader->VariableField.GenField.Reserved2;

		// Reserved half-byte
		if(!AttachPropertyInstanceEx(hFrame, XTPPropertyTable[XTP_RESERVED1].hProperty,
			sizeof(BYTE), pXTPFrame + 5 * sizeof(DWORD), sizeof(BYTE), &bReserved1, 0, 1, 0))
		{
			goto Exit;
		}

		// Original IHL
		if(!AttachPropertyInstanceEx(hFrame, XTPPropertyTable[XTP_ORIGIHL].hProperty,
			sizeof(BYTE), pXTPFrame + 5 * sizeof(DWORD), sizeof(BYTE), &bIHL, 0, 1, 0))
		{
			goto Exit;
		}

		// Original protocol
		if(!AttachPropertyInstanceEx(hFrame, XTPPropertyTable[XTP_ORIGPROTO].hProperty,
			sizeof(BYTE), pXTPFrame + 5 * sizeof(DWORD) + 1 * sizeof(BYTE), sizeof(BYTE), &bProtocol, 0, 1, 0))
		{
			goto Exit;
		}

		// Reserved word
		if(!AttachPropertyInstanceEx(hFrame, XTPPropertyTable[XTP_RESERVED2].hProperty,
			sizeof(BYTE), pXTPFrame + 5 * sizeof(DWORD) + 2 * sizeof(BYTE), sizeof(WORD), &wReserved2, 0, 1, 0))
		{
			goto Exit;
		}
	}

	// Finally, handle the sequence number and the UDP/general-case flag
	dwSequenceNum = (DWORD) pXTPHeader->SequenceNum;
	bUdpFlag = (BYTE) pXTPHeader->UdpFlag;

	// Authentication sequence number
	if(!AttachPropertyInstanceEx(hFrame, XTPPropertyTable[XTP_SEQNUM].hProperty,
		sizeof(DWORD), pXTPFrame + 6 * sizeof(DWORD), sizeof(DWORD), &dwSequenceNum, 0, 1, 0))
	{
		goto Exit;
	}

	// UDP or general case flag
	if(!AttachPropertyInstanceEx(hFrame, XTPPropertyTable[XTP_UDPFLAG].hProperty,
		sizeof(BYTE), pXTPFrame + 6 * sizeof(DWORD) + 3 * sizeof(BYTE), sizeof(BYTE), &bUdpFlag, 0, 1, 0))
	{
		goto Exit;
	}

	// BUGBUG - For now, just pass on the data beyond the XTP header
	pXTPFrame += (sizeof(XTPHEADER) + dwOptionsLen);

Exit:

	return pXTPFrame;
}

//==================================================================================
// XTP_GetProtocolName
//----------------------------------------------------------------------------------
//
// Description: Helper function for protocol parsing.  Returns friendly string for an
//	IP protocol
//
// Arguments:
//	BYTE				bProtocol			Byte containing protocol value from packet
//	LPSTR				szProtocolName		String that is filled with the friendly name
//											of the protocol
//
// Returns: TRUE on successful formatting of all properties, FALSE otherwise
//==================================================================================
BOOL WINAPI XTP_GetProtocolName(BYTE bProtocol, LPSTR szProtocolName)
{
	// Make sure we have a valid protocol string holder
	if(!szProtocolName || IsBadWritePtr(szProtocolName, PROTO_NAME_MAX))
		return FALSE;

	// Not sure what alot of these protocols are, but Netmon has them listed
	// in TCPIP.INI so since we can hand them off, we should probably print
	// a friendly name for each of them
	switch(bProtocol)
	{
	case PROTO_ICMP_VALUE:
		strcpy(szProtocolName, PROTO_ICMP_NAME);
		break;
	case PROTO_IGMP_VALUE:
		strcpy(szProtocolName, PROTO_IGMP_NAME);
		break;
	case PROTO_IP_VALUE:
		strcpy(szProtocolName, PROTO_IP_NAME);
		break;
	case PROTO_TCP_VALUE:
		strcpy(szProtocolName, PROTO_TCP_NAME);
		break;
	case PROTO_UDP_VALUE:
		strcpy(szProtocolName, PROTO_UDP_NAME);
		break;
	case PROTO_IP6_VALUE:
		strcpy(szProtocolName, PROTO_IP6_NAME);
		break;
	case PROTO_RSVP_VALUE:
		strcpy(szProtocolName, PROTO_RSVP_NAME);
		break;
	case PROTO_GRE_VALUE:
		strcpy(szProtocolName, PROTO_GRE_NAME);
		break;
	case PROTO_ESP_VALUE:
		strcpy(szProtocolName, PROTO_ESP_NAME);
		break;
	case PROTO_AH_VALUE:
		strcpy(szProtocolName, PROTO_AH_NAME);
		break;
	case PROTO_OSPF_VALUE:
		strcpy(szProtocolName, PROTO_OSPF_NAME);
		break;
	default:
		strcpy(szProtocolName, PROTO_ICMP_NAME);
		break;
	}

	return TRUE;
}

//==================================================================================
// XTP_FormatProperties
//----------------------------------------------------------------------------------
//
// Description: Called by Netmon to format each property that was attached by
//	XTP_AttachProperties
//
// Arguments:
//	HFRAME			hFrame			Handle to the frame that is being parsed.
//	LPBYTE			pMacFrame		Pointer to the first byte of a frame.
//	LPBYTE			pXTPFrame		Pointer to the beginning of the protocol data in a frame.
//	DWORD			nPropertyInsts	Number of PROPERTYINST structures provided by lpPropInst. 
//	LPPROPERTYINST	p				Pointer to an array of PROPERTYINST structures. 
//
// Returns: TRUE on successful formatting of all properties, FALSE otherwise
//==================================================================================
DWORD WINAPI XTP_FormatProperties(	HFRAME          hFrame,
									LPBYTE          pMacFrame,
									LPBYTE          pXTPFrame,
									DWORD           nPropertyInsts,
									LPPROPERTYINST  p)
{
    // loop through the property instances
    while( nPropertyInsts-- > 0)
    {
        // and call the formatter for each
        ( (FORMAT)(p->lpPropertyInfo->InstanceData) )( p);
        p++;
    }

    return NMERR_SUCCESS;
}

//============================================================================
//  Function: XTP_FormatSummary
// 
//  Description: The custom formatter for the summary property
//
//  Modification History
//
//  Your Name Here      01/01/70    Created
//============================================================================
VOID WINAPIV XTP_FormatSummary( LPPROPERTYINST pPropertyInst)
{
    LPSTR		pReturnedString = pPropertyInst->szPropertyText;
    PXTPHEADER	pXTPHeader = (PXTPHEADER)(pPropertyInst->lpData);
	IN_ADDR		SourceAddr, DestAddr;

	SourceAddr.S_un.S_addr = pXTPHeader->SourceAddr;
	DestAddr.S_un.S_addr = pXTPHeader->DestAddr;

	if(pXTPHeader->UdpFlag)
	{
	    // Print "UDP-case, Source: <SourceIP>:<DestPort>, Dest: <DestIP>:<DestPort>"
		 sprintf( pReturnedString, "UDP-case, Source %s:%u, Dest: %s:%u",
			 inet_ntoa(SourceAddr), pXTPHeader->VariableField.UdpField.SourcePort,
			 inet_ntoa(DestAddr), pXTPHeader->VariableField.UdpField.DestPort);
	}
	else
	{
		CHAR	szProtocolName[PROTO_NAME_MAX];

		XTP_GetProtocolName((BYTE) pXTPHeader->VariableField.GenField.Protocol, szProtocolName);

		// Print "General-case, Source: <SourceIP>, Dest: <DestIP>, Protocol: %s"
		sprintf(pReturnedString, "General-case, Source %s, Dest: %s, Protocol: %s",
			inet_ntoa(SourceAddr), inet_ntoa(DestAddr), szProtocolName);
	}
}
