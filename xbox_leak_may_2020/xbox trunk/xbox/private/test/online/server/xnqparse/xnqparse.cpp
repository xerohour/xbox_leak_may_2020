//============================================================================
//  FILE: xqparse.c
//
//  Description: Xbox Notification Queue (XNQ) Parser
//============================================================================

#include "stdafx.h"

//==================================================================================
// Globals
//==================================================================================
HPROTOCOL hXNQ = NULL;
DWORD     Attached = 0;

typedef struct BASE_MSG_HEADER BASE_HEADER, *PBASE_HEADER;
typedef struct Q_HELLO_MSG QHELLO_MSG, *PQHELLO_MSG;
typedef struct Q_USER_INFO_MSG QUSERINFO_MSG, *PQUSERINFO_MSG;
typedef struct Q_ADD_MSG QADD_MSG, *PQADD_MSG;
typedef struct Q_DELETE_MSG QDELETE_MSG, *PQDELETE_MSG;
typedef struct Q_DELETE_MATCHES_MSG QDELETEMATCHES_MSG, *PQDELETEMATCHES_MSG;
typedef struct Q_DEAD_XIP_MSG QDEADXIP_MSG, *PQDEADXIP_MSG;
typedef struct Q_DEAD_XRG_MSG QDEADXRG_MSG, *PQDEADXRG_MSG;
typedef struct Q_LIST_MSG QLIST_MSG, *PQLIST_MSG;
typedef struct Q_LIST_REPLY_MSG QLISTREPLY_MSG, *PQLISTREPLY_MSG;
typedef struct Q_TICKLE_MSG QTICKLE_MSG, *PQTICKLE_MSG;
typedef struct BASE_ACK_MSG BASEACK_MSG, *PBASEACK_MSG;

//==================================================================================
// Functions
//==================================================================================
extern PPF_PARSERDLLINFO WINAPI ParserAutoInstallInfo();
extern VOID   WINAPI XNQ_Register(HPROTOCOL hXNQ);
extern VOID   WINAPI XNQ_Deregister(HPROTOCOL hXNQ);
extern LPBYTE WINAPI XNQ_RecognizeFrame(HFRAME hFrame, 
                                        LPBYTE pMacFrame, 
                                        LPBYTE pXNQFrame, 
                                        DWORD MacType, 
                                        DWORD BytesLeft, 
                                        HPROTOCOL hPrevProtocol, 
                                        DWORD nPrevProtOffset,
                                        LPDWORD pProtocolStatus,
                                        LPHPROTOCOL phNextProtocol, 
                                        LPDWORD InstData);
extern LPBYTE WINAPI XNQ_AttachProperties(HFRAME hFrame, 
                                          LPBYTE pMacFrame, 
                                          LPBYTE pXNQFrame, 
                                          DWORD MacType, 
                                          DWORD BytesLeft, 
                                          HPROTOCOL hPrevProtocol, 
                                          DWORD nPrevProtOffset,
                                          DWORD InstData);
extern DWORD  WINAPI XNQ_FormatProperties(HFRAME hFrame, 
                                          LPBYTE pMacFrame, 
                                          LPBYTE pXNQFrame, 
                                          DWORD nPropertyInsts, 
                                          LPPROPERTYINST p);

VOID WINAPIV XNQ_FormatSummary( LPPROPERTYINST pPropertyInst);
VOID WINAPIV XNQ_FormatQueueID( LPPROPERTYINST pPropertyInst);
VOID WINAPIV XNQ_FormatUserCookie( LPPROPERTYINST pPropertyInst);
VOID WINAPIV XNQ_FormatUserID( LPPROPERTYINST pPropertyInst);

// Define the entry points that we will pass back at dll entry time ----------
ENTRYPOINTS XNQEntryPoints =
{
    // XNQ Entry Points
    XNQ_Register,
    XNQ_Deregister,
    XNQ_RecognizeFrame,
    XNQ_AttachProperties,
    XNQ_FormatProperties
};

//==================================================================================
// Properties
//==================================================================================
typedef enum
{
    XNQ_SUMMARY = 0,
    XNQ_MESSAGE_TYPE,
    XNQ_MESSAGE_LENGTH,
	XNQ_SEQUENCE_NUM,
	XNQ_SESSION_ID,
	XNQ_VERSION,
	XNQ_VERSION_DESC,
	XNQ_USERID,
	XNQ_XIP,
	XNQ_XRG,
	XNQ_TICKLE_IP,
	XNQ_TICKLE_PORT,
	XNQ_QUEUE_TYPE,
	XNQ_USER_COUNT,
	XNQ_COOKIE,
	XNQ_START_QUID,
	XNQ_MAX_ITEMS,
	XNQ_MAX_DATASIZE,
	XNQ_NEXT_QUID,
	XNQ_QUEUE_LEFT,
	XNQ_ITEMS_RETURNED,
	XNQ_RETURNED_DATASIZE,
	XNQ_FIRST_QUID,
	XNQ_LAST_QUID,
	XNQ_ATTRIB_ID,
	XNQ_ATTRIB_DATA,
	XNQ_RESULT,
	XNQ_ADDED_DATA,
	XNQ_RETURNED_DATA
};

PROPERTYINFO  XNQPropertyTable[] = 
{
    // XNQ_SUMMARY
    { 0, 0,
      "Summary",
      "Summary of the queue tickle",
      PROP_TYPE_SUMMARY,
      PROP_QUAL_NONE,
      NULL,
      80,
      XNQ_FormatSummary
    },

    // XNQ_MESSAGE_TYPE
    { 0, 0,
      "Msg type",
      "Message type",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_MESSAGE_LENGTH
    { 0, 0,
      "Msg len",
      "Message length",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_SEQUENCE_NUM
    { 0, 0,
      "Seq num",
      "Sequence number",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_SESSION_ID
    { 0, 0,
      "Sess ID",
      "Session ID",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_VERSION
    { 0, 0,
      "Ver",
      "Version",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_VERSION_DESC
    { 0, 0,
      "Ver desc",
      "Version description",
      PROP_TYPE_STRING,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_USERID
    { 0, 0,
      "User ID",
      "User ID",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      XNQ_FormatUserID
    },

    // XNQ_XIP

    { 0, 0,
      "XIP",
      "Xbox Address",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_XRG
    { 0, 0,
      "XRG",
      "XRG Address",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_TICKLE_IP
    { 0, 0,
      "Tickle IP",
      "Tickle IP for Xbox",
      PROP_TYPE_IP_ADDRESS,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_TICKLE_PORT
    { 0, 0,
      "Tickle Port",
      "Tickle Port for Xbox",
      PROP_TYPE_WORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_QUEUE_TYPE
    { 0, 0,
      "Queue Type",
      "Queue Type",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_USER_COUNT
    { 0, 0,
      "User count",
      "User count",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_COOKIE
    { 0, 0,
      "Cookie",
      "Client request cookie",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      XNQ_FormatUserCookie
    },

    // XNQ_START_QUID
    { 0, 0,
      "Start QUID",
      "ID of first queue item to return",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      XNQ_FormatQueueID
    },

    // XNQ_MAX_ITEMS
    { 0, 0,
      "Max items",
      "Max queue items to return",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_MAX_DATASIZE
    { 0, 0,
      "Max data",
      "Max queue data to return",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_NEXT_QUID
    { 0, 0,
      "Next QUID",
      "ID of next unreturned item in queue",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      XNQ_FormatQueueID
    },

    // XNQ_QUEUE_LEFT
    { 0, 0,
      "Queue left",
      "Number of items left in queue",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_ITEMS_RETURNED
    { 0, 0,
      "Num items",
      "Number of items being returned",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_RETURNED_DATASIZE
    { 0, 0,
      "Item size",
      "Total size of items being returned",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_FIRST_QUID
    { 0, 0,
      "First QUID",
      "ID of first QUID to delete",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      XNQ_FormatQueueID
    },

    // XNQ_LAST_QUID
    { 0, 0,
      "Last QUID",
      "ID of last QUID to delete",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      XNQ_FormatQueueID
    },

    // XNQ_ATTRIB_ID
    { 0, 0,
      "Attrib ID",
      "Attribute ID",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_ATTRIB_DATA
    { 0, 0,
      "Attrib data",
      "Attribute data",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_RESULT
    { 0, 0,
      "Result code",
      "Result code from ACK'd operation",
      PROP_TYPE_DWORD,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_ADDED_DATA
    { 0, 0,
      "Added data",
      "Data being added to queue",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    },

    // XNQ_RETURNED_DATA
    { 0, 0,
      "Returned data",
      "Data being returned from queue",
      PROP_TYPE_RAW_DATA,
      PROP_QUAL_NONE,
      NULL,
      80,
      FormatPropertyInstance
    }
};


DWORD nNumXNQProps = (sizeof(XNQPropertyTable)/sizeof(PROPERTYINFO));

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
                // Create the queue tickle protocol
                hXNQ = CreateProtocol("XNQ", 
                                      &XNQEntryPoints, 
                                      ENTRYPOINTS_SIZE);
            }
            Attached++;
            break;

        case DLL_PROCESS_DETACH:
            // are we detaching our last instance?
            Attached--;
            if( Attached == 0 )
            {
                // Clean up the queue tickle protocol
                DestroyProtocol(hXNQ);
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

    PPF_FOLLOWSET    pOutgoingFollow;
    PPF_FOLLOWENTRY  pFollowEntry;
    DWORD dwOutgoingFollows;

    // Base structure ========================================================

    // Allocate memory for parser info:
    dwNumProtocols = 1;
    pParserDllInfo = (PPF_PARSERDLLINFO)HeapAlloc(GetProcessHeap(),
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

    // Enter XNQ information
    pParserInfo = &(pParserDllInfo->ParserInfo[0]);
    sprintf( pParserInfo->szProtocolName, "XNQ" );
    sprintf( pParserInfo->szComment,      "Xbox Notification Queue Protocol" );
    sprintf( pParserInfo->szHelpFile,     "");

    // the incoming handoff set ----------------------------------------------
    // allocate
    dwIncomingHandoffs = 1;
    pIncomingHandoff = (PPF_HANDOFFSET)HeapAlloc(GetProcessHeap(),
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

    // TCP port 3000
    pHandoffEntry = &(pIncomingHandoff->Entry[0]);
    sprintf( pHandoffEntry->szIniFile,    "TCPIP.INI" );
    sprintf( pHandoffEntry->szIniSection, "TCP_HandoffSet" );
    sprintf( pHandoffEntry->szProtocol,   "XNQ" );
    pHandoffEntry->dwHandOffValue =        3000;
    pHandoffEntry->ValueFormatBase =       HANDOFF_VALUE_FORMAT_BASE_DECIMAL;    

    // the outgoing follow set ----------------------------------------------
    // allocate
    dwOutgoingFollows = 1;
    pOutgoingFollow = (PPF_FOLLOWSET)HeapAlloc(GetProcessHeap(),
                                                 HEAP_ZERO_MEMORY,
                                                 sizeof( PF_FOLLOWSET ) +
                                                 dwOutgoingFollows * sizeof( PF_FOLLOWENTRY) );
    if( pOutgoingFollow == NULL )
    {
        return pParserDllInfo;
    }

    // Fill in the incoming handoff set
    pParserInfo->pWhoCanFollowMe = pOutgoingFollow;
    pOutgoingFollow->nEntries = dwOutgoingFollows;

    // XNQ can follow XNQ
    pFollowEntry = &(pOutgoingFollow->Entry[0]);
    sprintf( pFollowEntry->szProtocol,   "XNQ" );

    return pParserDllInfo;
}

//==================================================================================
// XNQ_Register
//----------------------------------------------------------------------------------
//
// Description: Creates the property database for XNQ
//
// Arguments:
//	HPROTOCOL		hXNQ			Handle to the XNQ protocol
//
// Returns: void
//==================================================================================
void WINAPI XNQ_Register( HPROTOCOL hXNQ)
{
    WORD  i;

    // Tell Netmon to make reserve some space for our property table
    CreatePropertyDatabase( hXNQ, nNumXNQProps);

    // Add our properties to the Netmon's database
    for( i = 0; i < nNumXNQProps; i++)
    {
        AddProperty( hXNQ, &XNQPropertyTable[i]);
    }
}

//==================================================================================
// ParserAutoInstallInfo
//----------------------------------------------------------------------------------
//
// Description: Removes the property database for XNQ
//
// Arguments:
//	HPROTOCOL		hXNQ			Handle to the XNQ protocol
//
// Returns: void
//==================================================================================
VOID WINAPI XNQ_Deregister(HPROTOCOL hXNQ)
{
    // Tell the Netmon that it may now free our database
    DestroyPropertyDatabase( hXNQ);
}

//==================================================================================
// XNQ_RecognizeFrame
//----------------------------------------------------------------------------------
//
// Description: Called by Netmon to determine if the parser recognizes this frame as XNQ
//
// Arguments:
//	HFRAME			hFrame			Handle to the frame that contains the data.
//	LPBYTE			pMacFrame		Pointer to the first byte of a frame. The pointer 
//									provides a way to view data that other parsers recognize.
//	LPBYTE			pXNQFrame		Pointer to the start of the unclaimed data. Typically,
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
// Returns: When XNQ protocol is recognized, either returns a pointer to the next byte of
//			unclaimed data or NULL to indicate there is no more protocol data in the packet.
//			When XNQ protocol isn't recognized, returns the original pXNQFrame value to submit
//			the packet back to Netmon to find a proper protocol match
//==================================================================================
LPBYTE WINAPI XNQ_RecognizeFrame(HFRAME      hFrame,         
								LPBYTE      pMacFrame,      
								LPBYTE      pXNQFrame, 
								DWORD       MacType,        
								DWORD       BytesLeft,      
								HPROTOCOL   hPrevProtocol,  
								DWORD       nPrevProtOffset,
								LPDWORD     pProtocolStatus,
								LPHPROTOCOL phNextProtocol,
								LPDWORD     InstData)       
{
	PBASE_HEADER pXNQHeader = (PBASE_HEADER) pXNQFrame;
	DWORD dwOptionsLen = 0, dwCurrentPacketLen = 0;

	// Make sure there's enough room to parse the protocol
	if(BytesLeft < sizeof(BASE_HEADER))
	{
		*pProtocolStatus = PROTOCOL_STATUS_NOT_RECOGNIZED;
		return pXNQFrame;
	}

	// If the message is of an unknown type then we can't parse it
	if((pXNQHeader->dwMsgType > QMSG_MAX_REPLY_TYPE) && (pXNQHeader->dwMsgType != MSGTYPE_ACK))
	{
		*pProtocolStatus = PROTOCOL_STATUS_NOT_RECOGNIZED;
		return pXNQFrame;
	}

	// If we can tell that there's room at the end of this packet then we know it must be XNQ
	// Otherwise, we just claim the rest of the packet
	dwCurrentPacketLen = pXNQHeader->dwMsgLen + sizeof(BASE_HEADER);
	if(dwCurrentPacketLen > BytesLeft)
	{
		*pProtocolStatus = PROTOCOL_STATUS_CLAIMED;
		pXNQFrame = NULL;
		goto Exit;
	}

    *pProtocolStatus = PROTOCOL_STATUS_NEXT_PROTOCOL;
	*phNextProtocol = GetProtocolFromName("XNQ");
	pXNQFrame += dwCurrentPacketLen;

Exit:
	return pXNQFrame;
}

//==================================================================================
// XNQ_AttachProperties
//----------------------------------------------------------------------------------
//
// Description: Called by Netmon to determine if the parser recognizes this frame as XNQ
//
// Arguments:
//	HFRAME			hFrame			Handle of the frame that is being parsed. 
//	LPBYTE			lpFrame			Pointer to the first byte in a frame.
//	LPBYTE			pXNQFrame		Pointer to the start of the recognized data.
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
//			When the function isn't successful, returns the original pXNQFrame value to
//			submit the packet back to Netmon to find a proper protocol match
//==================================================================================
LPBYTE WINAPI XNQ_AttachProperties(	HFRAME      hFrame,         
								   LPBYTE      pMacFrame,     
								   LPBYTE      pXNQFrame,   
								   DWORD       MacType,        
								   DWORD       BytesLeft,      
								   HPROTOCOL   hPrevProtocol,  
								   DWORD       nPrevProtOffset,
								   DWORD       InstData)       
								   
{	
	DWORD dwPacketIndex = 0, dwItemCounter = 0;
	DWORD dwCurrentPacketLen = 0, dwTempAddr = 0;
	
	// Apply the header overlay in order to decode the protocol
	PBASE_HEADER pXNQHeader = (PBASE_HEADER) pXNQFrame;
	
	// Go ahead and apply all packet overlays (easier to read than casting repeatedly)
	PQHELLO_MSG pQHelloMsg = (PQHELLO_MSG) pXNQHeader;
	PQUSERINFO_MSG pQUserInfoMsg = (PQUSERINFO_MSG) pXNQHeader;
	PQADD_MSG pQAddMsg = (PQADD_MSG) pXNQHeader;
	PQDELETE_MSG pQDeleteMsg = (PQDELETE_MSG) pXNQHeader;
	PQDELETEMATCHES_MSG pQDeleteMatchesMsg = (PQDELETEMATCHES_MSG) pXNQHeader;
	PQDEADXIP_MSG pQDeadXipMsg = (PQDEADXIP_MSG) pXNQHeader;
	PQDEADXRG_MSG pQDeadXrgMsg = (PQDEADXRG_MSG) pXNQHeader;
	PQLIST_MSG pQListMsg = (PQLIST_MSG) pXNQHeader;
	PQLISTREPLY_MSG pQListReplyMsg = (PQLISTREPLY_MSG) pXNQHeader;
	PBASEACK_MSG pBaseAckMsg = (PBASEACK_MSG) pXNQHeader;

	// Verify there is enough data for a basic header
	if(BytesLeft < sizeof(BASE_HEADER))
	{
		pXNQFrame = NULL;
		goto Exit;
	}

	// Determine the size of this packet and verify that there is enough data left to parse it
	dwCurrentPacketLen = pXNQHeader->dwMsgLen + sizeof(BASE_HEADER);
	if(dwCurrentPacketLen > BytesLeft)
	{
		pXNQFrame = NULL;
		goto Exit;
	}
	
	//
	// Attach the header information...
	//
	
	if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_SUMMARY].hProperty, dwCurrentPacketLen, (LPBYTE)pXNQFrame, 0, 0, 0))
	{
		goto SkipToNextPacket;
	}
	
	if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_MESSAGE_TYPE].hProperty, sizeof(pXNQHeader->dwMsgType),
		(LPBYTE) &(pXNQHeader->dwMsgType), 0, 1, 0))
	{
		goto SkipToNextPacket;
	}
	
	if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_MESSAGE_LENGTH].hProperty, sizeof(pXNQHeader->dwMsgLen),
		(LPBYTE) &(pXNQHeader->dwMsgLen), 0, 1, 0))
	{
		goto SkipToNextPacket;
	}
	
	if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_SEQUENCE_NUM].hProperty, sizeof(pXNQHeader->dwSeqNum),
		(LPBYTE) &(pXNQHeader->dwSeqNum), 0, 1, 0))
	{
		goto SkipToNextPacket;
	}
	
	if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_SESSION_ID].hProperty, sizeof(pXNQHeader->dwSessID),
		(LPBYTE) &(pXNQHeader->dwSessID), 0, 1, 0))
	{
		goto SkipToNextPacket;
	}
	
	//
	// Attach the packet-specific information...
	//
	switch(pXNQHeader->dwMsgType)
	{
	case QMSG_HELLO:
		if(dwCurrentPacketLen < sizeof(QHELLO_MSG))
		{
			break;
		}
		
		// Attach the version number
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_VERSION].hProperty, sizeof(pQHelloMsg->dwProtocolVersion),
			(LPBYTE) &(pQHelloMsg->dwProtocolVersion), 0, 1, 0))
		{
			break;
		}
		
		// If there are no more bytes for the version description then exit
		if(dwCurrentPacketLen <= sizeof(QHELLO_MSG))
		{
			break;
			
		}
		
		for(dwPacketIndex = sizeof(QHELLO_MSG); dwPacketIndex < dwCurrentPacketLen; ++dwPacketIndex)
		{
			if(!pXNQFrame[dwPacketIndex])
				goto SkipToNextPacket;
		}
		
		// If the string was longer than 0, then display it to Netmon
		if(dwPacketIndex - sizeof(QHELLO_MSG))
		{
			
			if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_VERSION_DESC].hProperty, dwPacketIndex - sizeof(QHELLO_MSG),
				(LPBYTE) pXNQFrame + sizeof(QHELLO_MSG), 0, 1, 0))
			{
				break;
			}
		}
		
		break;
		
	case QMSG_USER_INFO:
		if(dwCurrentPacketLen < sizeof(QUSERINFO_MSG))
		{
			break;
		}
		
		// Attach the user ID
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_USERID].hProperty, sizeof(pQUserInfoMsg->qwUserID),
			(LPBYTE) &(pQUserInfoMsg->qwUserID), 0, 1, 0))
		{
			break;
		}
		
		// Attach the Xbox IP
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_XIP].hProperty, sizeof(pQUserInfoMsg->qwXIP),
			(LPBYTE) &(pQUserInfoMsg->qwXIP), 0, 1, 0))
		{
			break;
		}
		
		// Attach the XRG address
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_XRG].hProperty, sizeof(pQUserInfoMsg->qwXRG),
			(LPBYTE) &(pQUserInfoMsg->qwXRG), 0, 1, 0))
		{
			break;
		}
		
		dwTempAddr = ntohl(pQUserInfoMsg->dwTickleIP);

		// Attach the tickle IP
		if(!AttachPropertyInstanceEx(hFrame, XNQPropertyTable[XNQ_TICKLE_IP].hProperty, sizeof(pQUserInfoMsg->dwTickleIP),
			(LPBYTE) &(pQUserInfoMsg->dwTickleIP), sizeof(DWORD), &dwTempAddr, 0, 1, 0))
		{
			break;
		}
		
		// Attach the tickle port
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_TICKLE_PORT].hProperty, sizeof(pQUserInfoMsg->wTicklePort),
			(LPBYTE) &(pQUserInfoMsg->wTicklePort), 0, 1, 0))
		{
			break;
		}
		
		break;
		
	case QMSG_ADD:
		if(dwCurrentPacketLen < sizeof(QADD_MSG))
		{
			break;
		}
		
		// Attach the queue type
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_QUEUE_TYPE].hProperty, sizeof(pQAddMsg->dwQType),
			(LPBYTE) &(pQAddMsg->dwQType), 0, 1, 0))
		{
			break;
		}
		
		// Attach the user count
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_USER_COUNT].hProperty, sizeof(pQAddMsg->dwUserCount),
			(LPBYTE) &(pQAddMsg->dwUserCount), 0, 1, 0))
		{
			break;
		}
		
		// If we don't have room for the user ID's then move on to the next packet
		if((sizeof(QADD_MSG) + pQAddMsg->dwUserCount * sizeof(QWORD)) > dwCurrentPacketLen)
		{
			break;
		}
		
		for(dwItemCounter = 0; dwItemCounter < pQAddMsg->dwUserCount; ++dwItemCounter)
		{
			dwPacketIndex = sizeof(QADD_MSG) + dwItemCounter * sizeof(QWORD);
			
			// Attach the user IDs
			if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_USERID].hProperty, sizeof(QWORD),
				(LPBYTE) pXNQFrame + dwPacketIndex, 0, 1, 0))
			{
				goto SkipToNextPacket;
			}
		}
		
		// If we don't have room for the add data then move on to the next packet
		if((sizeof(QADD_MSG) + pQAddMsg->dwUserCount * sizeof(QWORD)) == dwCurrentPacketLen)
		{
			break;
		}
		
		dwPacketIndex = sizeof(QADD_MSG) + pQAddMsg->dwUserCount * sizeof(QWORD);
		
		// Attached the added data
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_ADDED_DATA].hProperty, dwCurrentPacketLen - dwPacketIndex,
			(LPBYTE) pXNQFrame + dwPacketIndex, 0, 1, 0))
		{
			break;
		}
		
		break;
		
	case QMSG_DELETE:
		if(dwCurrentPacketLen < sizeof(QDELETE_MSG))
		{
			break;
		}
		
		// Attach the user ID
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_USERID].hProperty, sizeof(pQDeleteMsg->qwUserID),
			(LPBYTE) &(pQDeleteMsg->qwUserID), 0, 1, 0))
		{
			break;
		}
		
		// Attach the queue type
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_QUEUE_TYPE].hProperty, sizeof(pQDeleteMsg->dwQType),
			(LPBYTE) &(pQDeleteMsg->dwQType), 0, 1, 0))
		{
			break;
		}
		
		// Attach the ID for the first element to delete
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_FIRST_QUID].hProperty, sizeof(pQDeleteMsg->qwFirstQUID),
			(LPBYTE) &(pQDeleteMsg->qwFirstQUID), 0, 1, 0))
		{
			break;
		}
		
		// Attach the ID for the last element to delete
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_LAST_QUID].hProperty, sizeof(pQDeleteMsg->qwLastQUID),
			(LPBYTE) &(pQDeleteMsg->qwLastQUID), 0, 1, 0))
		{
			break;
		}
		
		break;
		
	case QMSG_DELETE_MATCHES:
		if(dwCurrentPacketLen < sizeof(QDELETEMATCHES_MSG))
		{
			break;
		}
				
		// Attach the queue type
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_QUEUE_TYPE].hProperty, sizeof(pQDeleteMatchesMsg->dwQType),
			(LPBYTE) &(pQDeleteMatchesMsg->dwQType), 0, 1, 0))
		{
			break;
		}
		
		// Attach the attrib ID
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_ATTRIB_ID].hProperty, sizeof(pQDeleteMatchesMsg->dwMatchAttrNum),
			(LPBYTE) &(pQDeleteMatchesMsg->dwMatchAttrNum), 0, 1, 0))
		{
			break;
		}
		
		// Attach the user count
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_USER_COUNT].hProperty, sizeof(pQDeleteMatchesMsg->dwUserCount),
			(LPBYTE) &(pQDeleteMatchesMsg->dwUserCount), 0, 1, 0))
		{
			break;
		}
		
		// If we don't have room for the user ID's then move on to the next packet
		if((sizeof(QDELETEMATCHES_MSG) + pQDeleteMatchesMsg->dwUserCount * sizeof(QWORD)) > dwCurrentPacketLen)
		{
			break;
		}
		
		for(dwItemCounter = 0; dwItemCounter < pQDeleteMatchesMsg->dwUserCount; ++dwItemCounter)
		{
			dwPacketIndex = sizeof(QDELETEMATCHES_MSG) + dwItemCounter * sizeof(QWORD);
			
			// Attach the user IDs
			if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_USERID].hProperty, sizeof(QWORD),
				(LPBYTE) pXNQFrame + dwPacketIndex, 0, 1, 0))
			{
				goto SkipToNextPacket;
			}
		}

		// If we don't have room for the attribute data then move on to the next packet
		if((sizeof(QDELETEMATCHES_MSG) + pQDeleteMatchesMsg->dwUserCount * sizeof(QWORD)) == dwCurrentPacketLen)
		{
			break;
		}
		
		dwPacketIndex = sizeof(QDELETEMATCHES_MSG) + pQDeleteMatchesMsg->dwUserCount * sizeof(QWORD);
		
		// Attached the deleted data
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_ATTRIB_DATA].hProperty, dwCurrentPacketLen - dwPacketIndex,
			(LPBYTE) pXNQFrame + dwPacketIndex, 0, 1, 0))
		{
			break;
		}
		
		break;
		
	case QMSG_DEAD_XIP:
		if(dwCurrentPacketLen < sizeof(QDEADXIP_MSG))
		{
			break;
		}
		
		// Attach the Xbox IP
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_XIP].hProperty, sizeof(pQDeadXipMsg->qwXIP),
			(LPBYTE) &(pQDeadXipMsg->qwXIP), 0, 1, 0))
		{
			break;
		}
		
		break;
	case QMSG_DEAD_XRG:
		if(dwCurrentPacketLen < sizeof(QDEADXRG_MSG))
		{
			break;
		}
		
		// Attach the XRG IP
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_XRG].hProperty, sizeof(pQDeadXrgMsg->qwXRG),
			(LPBYTE) &(pQDeadXrgMsg->qwXRG), 0, 1, 0))
		{
			break;
		}
		
		break;
		
	case QMSG_LIST:
		if(dwCurrentPacketLen < sizeof(QLIST_MSG))
		{
			break;
		}
		
		// Attach the user ID
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_USERID].hProperty, sizeof(pQListMsg->qwUserID),
			(LPBYTE) &(pQListMsg->qwUserID), 0, 1, 0))
		{
			break;
		}
		
		// Attach the queue type
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_QUEUE_TYPE].hProperty, sizeof(pQListMsg->dwQType),
			(LPBYTE) &(pQListMsg->dwQType), 0, 1, 0))
		{
			break;
		}
		
		// Attach the cookie
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_COOKIE].hProperty, sizeof(pQListMsg->qwCookie),
			(LPBYTE) &(pQListMsg->qwCookie), 0, 1, 0))
		{
			break;
		}
		
		// Attach the ID of the first queue item to return
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_START_QUID].hProperty, sizeof(pQListMsg->qwStartQUID),
			(LPBYTE) &(pQListMsg->qwStartQUID), 0, 1, 0))
		{
			break;
		}
		
		// Attach the max number of items to return
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_MAX_DATASIZE].hProperty, sizeof(pQListMsg->dwMaxTotalItemDataSize),
			(LPBYTE) &(pQListMsg->dwMaxTotalItemDataSize), 0, 1, 0))
		{
			break;
		}
		
		// Attach the max number of items to return
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_MAX_ITEMS].hProperty, sizeof(pQListMsg->dwMaxItems),
			(LPBYTE) &(pQListMsg->dwMaxItems), 0, 1, 0))
		{
			break;
		}
		
		break;
		
	case QMSG_LIST_REPLY:
		if(dwCurrentPacketLen < sizeof(QLISTREPLY_MSG))
		{
			break;
		}
		
		// Attach the user ID
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_USERID].hProperty, sizeof(pQListReplyMsg->qwUserID),
			(LPBYTE) &(pQListReplyMsg->qwUserID), 0, 1, 0))
		{
			break;
		}
		
		// Attach the queue type
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_QUEUE_TYPE].hProperty, sizeof(pQListReplyMsg->dwQType),
			(LPBYTE) &(pQListReplyMsg->dwQType), 0, 1, 0))
		{
			break;
		}
		
		// Attach the reply result
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_RESULT].hProperty, sizeof(pQListReplyMsg->hr),
			(LPBYTE) &(pQListReplyMsg->hr), 0, 1, 0))
		{
			break;
		}
		
		// Attach the cookie
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_COOKIE].hProperty, sizeof(pQListReplyMsg->qwCookie),
			(LPBYTE) &(pQListReplyMsg->qwCookie), 0, 1, 0))
		{
			break;
		}
		
		// Attach the ID of the next unreturned item in the queue
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_NEXT_QUID].hProperty, sizeof(pQListReplyMsg->qwNextQUID),
			(LPBYTE) &(pQListReplyMsg->qwNextQUID), 0, 1, 0))
		{
			break;
		}
		
		// Attach the number of items left in the queue
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_QUEUE_LEFT].hProperty, sizeof(pQListReplyMsg->dwQLeft),
			(LPBYTE) &(pQListReplyMsg->dwQLeft), 0, 1, 0))
		{
			break;
		}
		
		// Attach the number of items being returned
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_ITEMS_RETURNED].hProperty, sizeof(pQListReplyMsg->dwNumItems),
			(LPBYTE) &(pQListReplyMsg->dwNumItems), 0, 1, 0))
		{
			break;
		}
		
		// Attach the total size of all queue items being returned
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_RETURNED_DATASIZE].hProperty, sizeof(pQListReplyMsg->dwTotalItemDataSize),
			(LPBYTE) &(pQListReplyMsg->dwTotalItemDataSize), 0, 1, 0))
		{
			break;
		}
		
		if(dwCurrentPacketLen == sizeof(QLISTREPLY_MSG))
		{
			break;
		}
		
		dwPacketIndex = sizeof(QLISTREPLY_MSG);
		
		// Attached the returned data
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_RETURNED_DATA].hProperty, dwCurrentPacketLen - dwPacketIndex,
			(LPBYTE) pXNQFrame + dwPacketIndex, 0, 1, 0))
		{
			break;
		}
		
		
		break;
		
	case MSGTYPE_ACK:
		if(dwCurrentPacketLen < sizeof(BASEACK_MSG))
		{
			break;
		}
		
		// Attach the acked message type
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_MESSAGE_TYPE].hProperty, sizeof(pBaseAckMsg->dwAckMsgType),
			(LPBYTE) &(pBaseAckMsg->dwAckMsgType), 0, 1, 0))
		{
			break;
		}
		
		// Attach the acked sequence number
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_SEQUENCE_NUM].hProperty, sizeof(pBaseAckMsg->dwAckSeqNum),
			(LPBYTE) &(pBaseAckMsg->dwAckSeqNum), 0, 1, 0))
		{
			break;
		}
		
		// Attach the user ID
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_USERID].hProperty, sizeof(pBaseAckMsg->qwAckUserID),
			(LPBYTE) &(pBaseAckMsg->qwAckUserID), 0, 1, 0))
		{
			break;
		}
		
		// Attach the acked result
		if(!AttachPropertyInstance(hFrame, XNQPropertyTable[XNQ_RESULT].hProperty, sizeof(pBaseAckMsg->hrAck),
			(LPBYTE) &(pBaseAckMsg->hrAck), 0, 1, 0))
		{
			break;
		}
		
		break;
		// We should never get here since we already scanned for this in RecognizeFrame.
		// Don't claim any data, just exit
	default:
		break;
	}

SkipToNextPacket:
	pXNQFrame += dwCurrentPacketLen;

Exit:
	return pXNQFrame;
}

//==================================================================================
// XNQ_FormatProperties
//----------------------------------------------------------------------------------
//
// Description: Called by Netmon to format each property that was attached by
//	XNQ_AttachProperties
//
// Arguments:
//	HFRAME			hFrame			Handle to the frame that is being parsed.
//	LPBYTE			pMacFrame		Pointer to the first byte of a frame.
//	LPBYTE			pXNQFrame		Pointer to the beginning of the protocol data in a frame.
//	DWORD			nPropertyInsts	Number of PROPERTYINST structures provided by lpPropInst. 
//	LPPROPERTYINST	p				Pointer to an array of PROPERTYINST structures. 
//
// Returns: TRUE on successful formatting of all properties, FALSE otherwise
//==================================================================================
DWORD WINAPI XNQ_FormatProperties(	HFRAME          hFrame,
									LPBYTE          pMacFrame,
									LPBYTE          pXNQFrame,
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
//  Function: XNQ_FormatSummary
// 
//  Description: The custom formatter for the summary property
//
//  Modification History
//
//  Your Name Here      01/01/70    Created
//============================================================================
VOID WINAPIV XNQ_FormatSummary( LPPROPERTYINST pPropertyInst)
{
    LPSTR pReturnedString = pPropertyInst->szPropertyText;

//     Print "XQueue Message"
	 sprintf( pReturnedString, "XQueue Message");
}


//============================================================================
//  Function: XNQ_FormatQueueID
// 
//  Description: The custom formatter for the summary property
//
//  Modification History
//
//  Your Name Here      01/01/70    Created
//============================================================================
VOID WINAPIV XNQ_FormatQueueID( LPPROPERTYINST pPropertyInst)
{
    LPSTR pReturnedString = pPropertyInst->szPropertyText;
	PBYTE pByteIndex = (PBYTE) (pPropertyInst->lpData);

	if(pPropertyInst->DataLength < sizeof(QWORD))
	{
		sprintf( pReturnedString, "QueueID: <not enough data to parse>");
	}
	else
	{
		sprintf( pReturnedString, "QueueID: 0x%02X%02X%02X%02X%02X%02X%02X%02X",
			pByteIndex[7], pByteIndex[6], pByteIndex[5], pByteIndex[4], pByteIndex[3], pByteIndex[2], pByteIndex[1], pByteIndex[0]);
	}
}


//============================================================================
//  Function: XNQ_FormatUserCookie
// 
//  Description: The custom formatter for the summary property
//
//  Modification History
//
//  Your Name Here      01/01/70    Created
//============================================================================
VOID WINAPIV XNQ_FormatUserCookie( LPPROPERTYINST pPropertyInst)
{
    LPSTR pReturnedString = pPropertyInst->szPropertyText;
	PBYTE pByteIndex = (PBYTE) (pPropertyInst->lpData);

	if(pPropertyInst->DataLength < sizeof(QWORD))
	{
		sprintf( pReturnedString, "User cookie: <not enough data to parse>");
	}
	else
	{
		sprintf( pReturnedString, "User cookie: 0x%02X%02X%02X%02X%02X%02X%02X%02X",
			pByteIndex[7], pByteIndex[6], pByteIndex[5], pByteIndex[4], pByteIndex[3], pByteIndex[2], pByteIndex[1], pByteIndex[0]);
	}
}


//============================================================================
//  Function: XNQ_FormatUserID
// 
//  Description: The custom formatter for the summary property
//
//  Modification History
//
//  Your Name Here      01/01/70    Created
//============================================================================
VOID WINAPIV XNQ_FormatUserID( LPPROPERTYINST pPropertyInst)
{
    LPSTR pReturnedString = pPropertyInst->szPropertyText;
	PBYTE pByteIndex = (PBYTE) (pPropertyInst->lpData);

	if(pPropertyInst->DataLength < sizeof(QWORD))
	{
		sprintf( pReturnedString, "UserID: <not enough data to parse>");
	}
	else
	{
		sprintf( pReturnedString, "UserID: 0x%02X%02X%02X%02X%02X%02X%02X%02X",
			pByteIndex[7], pByteIndex[6], pByteIndex[5], pByteIndex[4], pByteIndex[3], pByteIndex[2], pByteIndex[1], pByteIndex[0]);
	}
}
