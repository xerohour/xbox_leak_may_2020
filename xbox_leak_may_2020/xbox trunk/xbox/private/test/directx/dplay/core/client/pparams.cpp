//==================================================================================
// Includes
//==================================================================================
#include "dptest.h"
#define BAD_PARAM_TESTS

#define DEBUGBREAK() { _asm int 3 }

//==================================================================================
// Callback contexts
//==================================================================================
typedef struct tagPEERCANCELCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	HANDLE					hConnectCompleteEvent; // event to set when connect completes
	DPNHANDLE				dpnhExpectedAsyncOp; // the expected handle of the completing connect
	HRESULT					hrExpectedResult; // the expected result code of the connect completion
	BOOL					fCompleted; // whether the connect has completed or not
} PEERCANCELCONTEXT, * PPEERCANCELCONTEXT;

typedef struct tagPEERCONNECTCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	HRESULT					hrExpectedResult; // the expected result code of the connect completion
	BOOL					fCompleted; // whether the connect has completed or not
	HANDLE					hConnectCompleteEvent; // event to set when connect completes
} PEERCONNECTCONTEXT, * PPEERCONNECTCONTEXT;

typedef struct tagPEERSENDTOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fSendCanComplete; // whether the send can complete or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the completed send
	HRESULT					hrExpectedResult; // the expected result code of the send completion
	BOOL					fSendCompleted; // whether the send has completed or not
	HANDLE					hSendCompleteEvent; // event to set when send completes
	BOOL					fCanReceive; // whether a (the) receive is allowed or not
	DWORD					dwExpectedReceiveSize; // expected size of the received message
	BOOL					fReceived; // whether the receive has arrived or not
	HANDLE					hReceiveEvent; // event to set when message is received
} PEERSENDTOCONTEXT, * PPEERSENDTOCONTEXT;

typedef struct tagPEERGETSENDQINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
} PEERGETSENDQINFOCONTEXT, * PPEERGETSENDQINFOCONTEXT;

typedef struct tagPEERHOSTCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	DWORD					dwUnused; // ??????????
} PEERHOSTCONTEXT, * PPEERHOSTCONTEXT;

typedef struct tagPEERSETAPPDESCCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fCanGetAppDescUpdate; // whether the app desc message can arrive or not
	BOOL					fGotAppDescUpdate; // whether the app desc message has arrived or not
	HANDLE					hGotAppDescUpdateEvent; // event to set when app desc message is received
} PEERSETAPPDESCCONTEXT, * PPEERSETAPPDESCCONTEXT;

typedef struct tagPEERCREATEGROUPCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	HRESULT					hrExpectedResult; // the expected result code of the connect/group creation
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	HANDLE					hAsyncOpCompletedEvent; // event to set when async op completes
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	DPNID					dpnidGroupCreated; // the DPNID of the group created
	HANDLE					hGotCreateGroupEvent; // event to set when create group indication is received
} PEERCREATEGROUPCONTEXT, * PPEERCREATEGROUPCONTEXT;

typedef struct tagPEERDESTROYGROUPCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	HRESULT					hrExpectedResult; // the expected result code of the connect/group creation
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	HANDLE					hAsyncOpCompletedEvent; // event to set when async op completes
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	BOOL					fCanGetDestroyGroup; // whether the destroy group indication can arrive or not
	DPNID					dpnidGroup; // the DPNID of the group created/to be destroyed
	HANDLE					hGotGroupMsgEvent; // event to set when create/destroy group indication is received
} PEERDESTROYGROUPCONTEXT, * PPEERDESTROYGROUPCONTEXT;

typedef struct tagPEERADDTOGROUPCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	HRESULT					hrExpectedResult; // the expected result code of the connect/group creation
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	HANDLE					hAsyncOpCompletedEvent; // event to set when async op completes
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	BOOL					fCanGetAddToGroup; // whether the add player to group indication can arrive or not
	DPNID					dpnidGroup; // the DPNID of the group created/to be destroyed
	DPNID					dpnidExpectedPlayer; // the DPNID of the player being added
	HANDLE					hGotGroupMsgEvent; // event to set when create/destroy/addto group indication is received
	BOOL					fGotAddToGroup; // whether the add player to group indication did arrive
} PEERADDTOGROUPCONTEXT, * PPEERADDTOGROUPCONTEXT;

typedef struct tagPEERREMOVEFROMGROUPCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	HRESULT					hrExpectedResult; // the expected result code of the connect/group creation
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	HANDLE					hAsyncOpCompletedEvent; // event to set when async op completes
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	BOOL					fCanGetAddToGroup; // whether the add player to group indication can arrive or not
	BOOL					fCanGetRemoveFromGroup; // whether the remove player from group indication can arrive or not
	DPNID					dpnidGroup; // the DPNID of the group created/to be destroyed
	DPNID					dpnidExpectedPlayer; // the DPNID of the player being added
	HANDLE					hGotGroupMsgEvent; // event to set when create/destroy/addto/removefrom group indication is received
	BOOL					fGotAddToGroup; // whether the add player to group indication did arrive
	BOOL					fGotRemoveFromGroup; // whether the remove player from group indication did arrive
} PEERREMOVEFROMGROUPCONTEXT, * PPEERREMOVEFROMGROUPCONTEXT;

typedef struct tagPEERSETGROUPINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	HRESULT					hrExpectedResult; // the expected result code of the connect/group creation
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	HANDLE					hAsyncOpCompletedEvent; // event to set when async op completes
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	BOOL					fCanGetGroupInfo; // whether the add player to group indication can arrive or not
	DPNID					dpnidGroup; // the DPNID of the group created/to be destroyed
	HANDLE					hGotGroupMsgEvent; // event to set when create/destroy/addto group indication is received
	BOOL					fGotGroupInfo; // whether the add player to group indication did arrive
} PEERSETGROUPINFOCONTEXT, * PPEERSETGROUPINFOCONTEXT;

typedef struct tagPEERGETGROUPINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	DPNID					dpnidGroup; // the DPNID of the group created/to be destroyed
	HANDLE					hGotGroupMsgEvent; // event to set when create/destroy/addto group indication is received
} PEERGETGROUPINFOCONTEXT, * PPEERGETGROUPINFOCONTEXT;

typedef struct tagPEERENUMPANDGCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	DPNID					dpnidGroup; // the DPNID of the group created/to be destroyed
	HANDLE					hGotGroupMsgEvent; // event to set when create/destroy/addto group indication is received
} PEERENUMPANDGCONTEXT, * PPEERENUMPANDGCONTEXT;

typedef struct tagPEERENUMGROUPMEMBERSCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	DPNID					dpnidExpectedPlayer; // the DPNID of the player being added
	DPNID					dpnidGroup; // the DPNID of the group created/to be destroyed
	HANDLE					hGotGroupMsgEvent; // event to set when create/destroy/addto group indication is received
	BOOL					fCanGetAddToGroup; // whether the add player to group indication can arrive or not
	BOOL					fGotAddToGroup; // whether the add player to group indication did arrive
} PEERENUMGROUPMEMBERSCONTEXT, * PPEERENUMGROUPMEMBERSCONTEXT;

typedef struct tagPEERSETPEERINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	HANDLE					hAsyncOpCompletedEvent; // event to set when async op completes
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	BOOL					fCanGetPeerInfoUpdate; // whether the peer info indication can arrive or not
	BOOL					fGotPeerInfoUpdate; // whether the peer info indication did arrive
	HANDLE					hGotPeerInfoUpdateEvent; // event to set when peer info message is received
} PEERSETPEERINFOCONTEXT, * PPEERSETPEERINFOCONTEXT;

typedef struct tagPEERGETPEERINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
} PEERGETPEERINFOCONTEXT, * PPEERGETPEERINFOCONTEXT;

typedef struct tagPEERCLOSECONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
} PEERCLOSECONTEXT, * PPEERCLOSECONTEXT;

typedef struct tagPEERENUMHOSTSCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					afAsyncOpCanComplete[2]; // whether the async op completion can arrive or not
	BOOL					afAsyncOpCompleted[2]; // whether the async op has completed or not
	DPNHANDLE				adpnhCompletedAsyncOp[2]; // the handle of the asynchronous operation given in the completion
	HRESULT					ahrExpectedResult[2]; // the expected result code of the async op
	HANDLE					ahAsyncOpCompletedEvent[2]; // event to set when async op completes
	PDIRECTPLAY8ADDRESS		apExpectedDP8AddressSender[2]; // expected address of sender
	PDIRECTPLAY8ADDRESS		apExpectedDP8AddressDevice[2]; // expected address of device
	BOOL					afCanGetResponses[2]; // are queries allowed
	DWORD					dwNumQueries; // number of queries received
	DWORD					adwNumResponses[2]; // number of responses received
} PEERENUMHOSTSCONTEXT, * PPEERENUMHOSTSCONTEXT;

typedef struct tagPEERDESTROYPEERCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					afExpectCreatePlayer[2]; // whether create player messages are allowed or not
	DPNID					adpnidPlayers[2]; // IDs of the players
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	BOOL					afExpectDestroyPlayer[2]; // whether destroy players are allowed or not
	DWORD					dwNumDestroyPlayersRemaining; // number of destroy player messages that should happen
	HANDLE					hLastPlayerMsgEvent; // event to set when last create/destroy player expected arrives
	BOOL					fExpectTerminateSession; // whether the terminate session message is allowed or not
	BOOL					fGotTerminateSession; // whether the terminate session message arrived or not
} PEERDESTROYPEERCONTEXT, * PPEERDESTROYPEERCONTEXT;

typedef struct tagPEERRETURNCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fExpectReceive; // whether receives are allowed or not
	DPNHANDLE				dpnhBuffer; // handle to buffer kept
	HANDLE					hReceivedMsgEvent; // handle to event to set when message arrives
} PEERRETURNCONTEXT, * PPEERRETURNCONTEXT;

typedef struct tagPEERGETPLAYERCONTEXTCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					afExpectCreatePlayer[2]; // whether create player messages are allowed or not
	DPNID					adpnidPlayers[2]; // IDs of the players
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					afExpectDestroyPlayer[2]; // whether destroy players are allowed or not
	DWORD					dwNumDestroyPlayersRemaining; // number of destroy player messages that should happen
	HANDLE					hLastPlayerMsgEvent; // event to set when last create/destroy player expected arrives
} PEERGETPLAYERCONTEXTCONTEXT, * PPEERGETPLAYERCONTEXTCONTEXT;

typedef struct tagPEERGETGROUPCONTEXTCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	BOOL					fCanGetCreateGroup; // whether the create group indication can arrive or not
	DPNID					dpnidExpectedGroupOwner; // expected group owner DPNID
	DPNID					dpnidGroup; // the DPNID of the group created/to be destroyed
	HANDLE					hGotGroupMsgEvent; // event to set when create/destroy/addto group indication is received
} PEERGETGROUPCONTEXTCONTEXT, * PPEERGETGROUPCONTEXTCONTEXT;

typedef struct tagPEERGETCONNINFOCONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
} PEERGETCONNINFOCONTEXT, * PPEERGETCONNINFOCONTEXT;

typedef struct tagPEERTERMINATECONTEXT
{
	HANDLE					hLog; // Handle to logging subsystem
	BOOL					afExpectCreatePlayer[2]; // whether create player messages are allowed or not
	DPNID					adpnidPlayers[2]; // IDs of the players
	BOOL					fConnectCanComplete; // whether the connect can complete or not
	BOOL					fConnectCompleted; // whether the connect has completed or not
	DPNHANDLE				dpnhCompletedAsyncOp; // the handle of the asynchronous operation given in the completion
	BOOL					fAsyncOpCanComplete; // whether the async op completion can arrive or not
	BOOL					fAsyncOpCompleted; // whether the async op has completed or not
	BOOL					afExpectDestroyPlayer[2]; // whether destroy players are allowed or not
	DWORD					dwNumDestroyPlayersRemaining; // number of destroy player messages that should happen
	HANDLE					hLastPlayerMsgEvent; // event to set when last create/destroy player expected arrives
	BOOL					fExpectTerminateSession; // whether the terminate session message is allowed or not
	BOOL					fGotTerminateSession; // whether the terminate session message arrived or not
} PEERTERMINATECONTEXT, * PPEERTERMINATECONTEXT;

//==================================================================================
// Helper functions
//==================================================================================

//==================================================================================
// TRACE
//----------------------------------------------------------------------------------
//
// Description: Wrapper for the logging stuff that was imported from testnet
//
// Arguments:
//	INT nLogLevel						Indicates level to log message as
//	LPCTSTR szFormat					Buffer that contains the format string
//	INT nNumArgs						Indicates the number of variable arguments present
//
// Returns: nothing
//==================================================================================
void WINAPI TRACE(HANDLE hLog, LPCSTR szFormat, INT nNumArgs ...) {
	CHAR szBuffer[1024] = "";
	va_list pArgs; 

	va_start(pArgs, nNumArgs);
	
	if(!hLog)
	{
		vsprintf(szBuffer, szFormat, pArgs);
		DbgPrint(szBuffer);
	}
	else
		xLog_va(hLog, XLL_INFO, szFormat, pArgs);
	
	va_end(pArgs);
}

//==================================================================================
// PeerCreateHost
//----------------------------------------------------------------------------------
//
// Description: Creates a new DirectPlay8Peer object that is hosting based on the
//				given message handler and application description.  The first hosting
//				address can optionally also be returned.
//
// Arguments:
//	HANDLE hLog								Handle to logging subsystem
//	PFNDPNMESSAGEHANDLER pfn				Message handler to use.
//	PVOID pvContext							Pointer to context for message handler.
//	PDPN_APPLICATION_DESC pdpnad			Pointer to application desc to use when
//											hosting the session
//	PDIRECTPLAY8PEER* ppPeerHost			Place to store pointer to DirectPlay8Peer
//											interface created
//	PVOID pvPlayerContext					Player context for local host player.
//	PDIRECTPLAY8ADDRESS* ppDP8AddressHost	Optional place to store pointer to first
//											address the object is hosting on.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT PeerCreateHost(HANDLE hLog,
							PFNDPNMESSAGEHANDLER pfn,
							PVOID pvContext,
							PDPN_APPLICATION_DESC pdpnad,
							PDIRECTPLAY8PEER* ppPeerHost,
							PVOID pvPlayerContext,
							PDIRECTPLAY8ADDRESS* ppDP8AddressHost)
{
	PDIRECTPLAY8ADDRESS pDP8AddressDevice = NULL;
	PDIRECTPLAY8ADDRESS* paDP8HostAddresses = NULL;
	HRESULT hr;
	LPVOID pVoice
	DWORD dwNumAddresses = 0;

	// Verify parameters
	if ((*ppPeerHost) != NULL)
	{
		xLog(hLog, XLL_INFO, "Already have peer object (%x)!", (*ppPeerHost));
		return (ERROR_INVALID_PARAMETER);
	}

	// Create the DirectPlay8Peer
	hr = DirectPlay8PeerCreate(ppPeerHost, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_INFO, "Failed with %u", hr);
		goto ERROR_EXIT;
	}
	else
		xLog(hLog, XLL_INFO, "Returned S_OK successfully");

	// Initialize the peer with the given callback function
	hr = (*ppPeerHost)->Initialize(pvContext, pfn, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_INFO, "Initializing DP8Peer object failed!");
		goto ERROR_EXIT;
	}

	// Create the device address
	hr = DirectPlay8AddressCreate(&pDP8AddressDevice)
	if (hr != S_OK)
	{
		xLog(hLog, XLL_INFO, "Couldn't CoCreate host DirectPlay8Address object!");
		goto ERROR_EXIT;
	}

	// Set the device service provider to TCP
	hr = pDP8AddressDevice->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_INFO, "Couldn't set DirectPlay8Address SP!");
		goto ERROR_EXIT;
	}

	// Start hosting a session with the given application description and player context
	hr = (*ppPeerHost)->Host(pdpnad, &pDP8AddressDevice, 1, NULL, NULL, pvPlayerContext, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_INFO, "Hosting failed!");
		goto ERROR_EXIT;
	}

	// Get rid of the address
	pDP8AddressDevice->Release();
	pDP8AddressDevice = NULL;

	// If the caller wants an address, give one to him.
	if (ppDP8AddressHost != NULL)
	{
		// Ignore error
		(*ppPeerHost)->GetLocalHostAddresses(NULL, &dwNumAddresses, 0);

		paDP8HostAddresses = (PDIRECTPLAY8ADDRESS*) LocalAlloc(LPTR, (dwNumAddresses * sizeof (PDIRECTPLAY8ADDRESS)));
		if (paDP8HostAddresses == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		}


		hr = (*ppPeerHost)->GetLocalHostAddresses(paDP8HostAddresses, &dwNumAddresses, 0);
		if (hr != DPN_OK)
		{
			xLog(hLog, XLL_INFO, "Getting local addresses failed!");
			goto ERROR_EXIT;
		}

		// Move the first address to the caller's pointer.
		(*ppDP8AddressHost) = paDP8HostAddresses[0];
		paDP8HostAddresses[0] = NULL;


		// Release all the other addresses.
		while (dwNumAddresses > 1)
		{
			dwNumAddresses--;
			paDP8HostAddresses[dwNumAddresses]->Release();
			paDP8HostAddresses[dwNumAddresses] = NULL;
		}


		LocalFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	}


	return (DPN_OK);


ERROR_EXIT:


	if (paDP8HostAddresses != NULL)
	{
		LocalFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	}

	if (pDP8AddressDevice != NULL)
	{
		pDP8AddressDevice->Release();
		pDP8AddressDevice = NULL;
	}

	if ((*ppPeerHost) != NULL)
	{
		delete (*ppPeerHost);
		(*ppPeerHost) = NULL;
	}

	return (hr);
}

//==================================================================================
// PeerCompareAppDesc
//----------------------------------------------------------------------------------
//
// Description: Makes sure the two application desc structures are essentially the
//				same.
//
// Arguments:
//	HANDLE hLog								Handle to event logging subsystem
//	PDPN_APPLICATION_DESC pdpnadCompare		Pointer to app desc to compare.
//	PDPN_APPLICATION_DESC pdpnadExpected	Pointer to app desc to use as reference.
//
// Returns: DPN_OK if they match, a test result error code otherwise.
//==================================================================================
HRESULT PeerCompareAppDesc(HANDLE hLog, PDPN_APPLICATION_DESC pdpnadCompare, PDPN_APPLICATION_DESC pdpnadExpected)
{
	// Compare the size.
	if (pdpnadCompare->dwSize != sizeof (DPN_APPLICATION_DESC))
	{
		TRACE(hLog, "Application desc size is wrong (%u != %u)!",
			2, pdpnadCompare->dwSize, sizeof (DPN_APPLICATION_DESC));
		return (ERROR_NO_MATCH);
	} // end if (size differs)


	// Compare the flags.
	if (pdpnadCompare->dwFlags != pdpnadExpected->dwFlags)
	{
		TRACE(hLog, "Application desc flags are wrong (%x != %x)!",
			2, pdpnadCompare->dwFlags, pdpnadExpected->dwFlags);
		return (ERROR_NO_MATCH);
	} // end if (flags differ)


	// Can't compare instance GUID, but we can make sure it's not GUID_NULL.
	if (IsZero(&pdpnadCompare->guidApplication, sizeof (GUID)))
	{
		TRACE(hLog, "Application desc instance GUID was not filled in!", 0);
		return (ERROR_NO_DATA);
	} // end if (no instance GUID)


	// Compare the app GUID.
	if (memcmp(&pdpnadCompare->guidApplication, &pdpnadExpected->guidApplication, sizeof (GUID)) != 0)
	{
		TRACE(hLog, "Application desc app GUID is wrong (%g != %g)!",
			2, &pdpnadCompare->guidApplication, &pdpnadExpected->guidApplication);
		return (ERROR_NO_MATCH);
	} // end if (flags differ)


	// Compare the max players.
	if (pdpnadCompare->dwMaxPlayers != pdpnadExpected->dwMaxPlayers)
	{
		TRACE(hLog, "Application desc max players is wrong (%u != %u)!",
			2, pdpnadCompare->dwMaxPlayers, pdpnadExpected->dwMaxPlayers);
		return (ERROR_NO_MATCH);
	} // end if (max players differs)


	// We only expect one player.
	if (pdpnadCompare->dwCurrentPlayers != 1)
	{
		TRACE(hLog, "Application desc current players is unexpected (%u != 1)!",
			1, pdpnadCompare->dwCurrentPlayers);
		return (ERROR_NO_MATCH);
	} // end if (current players unexpected)


	// Compare the session name.
	if (pdpnadExpected->pwszSessionName != NULL)
	{
		if (pdpnadCompare->pwszSessionName != NULL)
		{
			if (wcscmp(pdpnadCompare->pwszSessionName, pdpnadExpected->pwszSessionName) != 0)
			{
				TRACE(hLog, "Application desc session name is wrong (\"%S\"!= \"%S\")!",
					2, pdpnadCompare->pwszSessionName, pdpnadExpected->pwszSessionName);
				return (ERROR_NO_MATCH);
			} // end if (name differs)
		} // end if (received session name)
		else
		{
			TRACE(hLog, "Application desc session name (\"%S\") was not returned!",
				1, pdpnadExpected->pwszSessionName);
			return (ERROR_NO_MATCH);
		} // end if (no name specified)
	} // end if (gave session name)
	else
	{
		if (pdpnadCompare->pwszSessionName != NULL)
		{
			TRACE(hLog, "Unexpected application desc session name was returned (\"%S\")!",
				1, pdpnadCompare->pwszSessionName);
			return (ERROR_NO_MATCH);
		} // end if (name returned)
	} // end else (didn't specify session name)


	// Compare the password.
	if (pdpnadExpected->pwszPassword != NULL)
	{
		if (pdpnadCompare->pwszPassword != NULL)
		{
			if (wcscmp(pdpnadCompare->pwszPassword, pdpnadExpected->pwszPassword) != 0)
			{
				TRACE(hLog, "Application desc password is wrong (\"%S\"!= \"%S\")!",
					2, pdpnadCompare->pwszPassword, pdpnadExpected->pwszPassword);
				return (ERROR_NO_MATCH);
			} // end if (password differs)
		} // end if (received password)
		else
		{
			TRACE(hLog, "Application desc password (\"%S\") was not returned!",
				1, pdpnadExpected->pwszPassword);
			return (ERROR_NO_MATCH);
		} // end if (no password specified)
	} // end if (gave password)
	else
	{
		if (pdpnadCompare->pwszPassword != NULL)
		{
			TRACE(hLog, "Unexpected application desc password was returned (\"%S\")!",
				1, pdpnadCompare->pwszPassword);
			return (ERROR_NO_MATCH);
		} // end if (password returned)
	} // end else (didn't specify password)


	// Reserved data is not allowed or expected.
	if ((pdpnadCompare->pvReservedData != NULL) || (pdpnadCompare->dwReservedDataSize != 0))
	{
		TRACE(hLog, "Application desc reserved data was returned (ptr %x, size %u)!",
			2, pdpnadCompare->pvReservedData, pdpnadCompare->dwReservedDataSize);
		return (ERROR_NO_MATCH);
	} // end if (reserved data returned)


	// Compare the application reserved data.
	if (pdpnadExpected->pvApplicationReservedData != NULL)
	{
		if ((pdpnadCompare->pvApplicationReservedData != NULL) &&
			(pdpnadCompare->dwApplicationReservedDataSize == pdpnadExpected->dwApplicationReservedDataSize))
		{
			if (memcmp(pdpnadCompare->pvApplicationReservedData, pdpnadExpected->pvApplicationReservedData, pdpnadCompare->dwApplicationReservedDataSize) != 0)
			{
				TRACE(hLog, "Application desc app reserved data is wrong (%u byte data at \"%x\"!= data at \"%x\")!",
					3, pdpnadCompare->dwApplicationReservedDataSize,
					pdpnadCompare->pvApplicationReservedData,
					pdpnadExpected->pvApplicationReservedData);
				return (ERROR_NO_MATCH);
			} // end if (app reserved data differs)
		} // end if (received app reserved data)
		else
		{
			TRACE(hLog, "Application desc app reserved data (data \"%x\", size %u) was not returned, data ptr was %x and size was %u!",
				4, pdpnadExpected->pvApplicationReservedData,
				pdpnadExpected->dwApplicationReservedDataSize,
				pdpnadCompare->pvApplicationReservedData,
				pdpnadCompare->dwApplicationReservedDataSize);
			return (ERROR_NO_MATCH);
		} // end if (no app reserved data specified)
	} // end if (gave app reserved data)
	else
	{
		if ((pdpnadCompare->pvApplicationReservedData != NULL) ||
			(pdpnadCompare->dwApplicationReservedDataSize != 0))
		{
			TRACE(hLog, "Unexpected application desc app reserved data was returned (data \"%x\", size = %u)!",
				2, pdpnadCompare->pvApplicationReservedData,
				pdpnadCompare->dwApplicationReservedDataSize);
			return (ERROR_NO_MATCH);
		} // end if (app reserved data returned)
	} // end else (didn't specify app reserved data)

	return (DPN_OK);
}

//==================================================================================
// Callback functions
//==================================================================================

//==================================================================================
// PeerNoMessagesHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: always returns E_FAIL
//==================================================================================
HRESULT PeerNoMessagesHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	TRACE(NULL, "Got unexpected message type %x!  DEBUGBREAK()-ing.",1, dwMsgType);
	DEBUGBREAK();

	return (E_NOTIMPL);
}

//==================================================================================
// PeerCancelHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT PeerCancelHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PPEERCANCELCONTEXT		pContext = (PPEERCANCELCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			TRACE(pContext->hLog, "     hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			TRACE(pContext->hLog, "     pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			TRACE(pContext->hLog, "     hResultCode = %e", 1, pConnectCompleteMsg->hResultCode);
			TRACE(pContext->hLog, "     pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			TRACE(pContext->hLog, "     dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion
			if (! pContext->fConnectCanComplete)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the completion
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				break;
			} // end if (message unexpected)

			if (pConnectCompleteMsg->hAsyncOp != pContext->dpnhExpectedAsyncOp)
			{
				TRACE(pContext->hLog, "Connect completed with wrong async op handle (%x != %x)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->hAsyncOp, pContext->dpnhExpectedAsyncOp);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)

			if (pConnectCompleteMsg->pvUserContext != pContext->hConnectCompleteEvent)
			{
				TRACE(pContext->hLog, "Connect completed with wrong context (%x != %x)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvUserContext,
					pContext->hConnectCompleteEvent);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)

			if (pConnectCompleteMsg->hResultCode != pContext->hrExpectedResult)
			{
				TRACE(pContext->hLog, "Connect completed with wrong error code (%x != %x)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->hResultCode, pContext->hrExpectedResult);
				DEBUGBREAK();
				break;
			} // end if (unexpected result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Make sure this isn't a double completion.
			if (pContext->fCompleted)
			{
				TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fCompleted = TRUE;

			if (! SetEvent(pContext->hConnectCompleteEvent))
			{
				hr = GetLastError();
				TRACE(pContext->hLog, "Couldn't set connect completion event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hConnectCompleteEvent);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				break;
			} // end if (couldn't set event)
		  break;

		default:
			TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
}

//==================================================================================
// PeerConnectHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT PeerConnectHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PPEERCONNECTCONTEXT		pContext = (PPEERCONNECTCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			TRACE(pContext->hLog, "     hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			TRACE(pContext->hLog, "     pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			TRACE(pContext->hLog, "     hResultCode = %e", 1, pConnectCompleteMsg->hResultCode);
			TRACE(pContext->hLog, "     pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			TRACE(pContext->hLog, "     dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != pContext->hrExpectedResult)
			{
				TRACE(pContext->hLog, "Connect completed with wrong error code (%x != %x)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->hResultCode, pContext->hrExpectedResult);
				DEBUGBREAK();
				break;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				break;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fCompleted)
			{
				TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fCompleted = TRUE;

			// Remember the async op handle for comparison by the main thread.
			pContext->dpnhCompletedAsyncOp = pConnectCompleteMsg->hAsyncOp;

			if (pContext->hConnectCompleteEvent != NULL)
			{
				if (! SetEvent(pContext->hConnectCompleteEvent))
				{
					hr = GetLastError();
					TRACE(pContext->hLog, "Couldn't set connect completion event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hConnectCompleteEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					break;
				} // end if (couldn't set event)
			} // end if (there's an event to set)
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		default:
			TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
}

//==================================================================================
// PeerNewPlayerHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT PeerNewPlayerHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT		hr = DPN_OK;


	switch (dwMsgType)
	{
		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			TRACE(NULL, "DPN_MSGID_CREATE_PLAYER", 0);
			TRACE(NULL, "     dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			TRACE(NULL, "     dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			TRACE(NULL, "     pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			TRACE(NULL, "DPN_MSGID_DESTROY_PLAYER", 0);
			TRACE(NULL, "     dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			TRACE(NULL, "     dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			TRACE(NULL, "     pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			TRACE(NULL, "DPN_MSGID_INDICATE_CONNECT", 0);
			TRACE(NULL, "     dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			TRACE(NULL, "     pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			TRACE(NULL, "     dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			TRACE(NULL, "     pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			TRACE(NULL, "     dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			TRACE(NULL, "     pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			TRACE(NULL, "     pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			TRACE(NULL, "     pAddressPlayer = %a", 1, pIndicateConnectMsg->pAddressPlayer);
			TRACE(NULL, "     pAddressDevice = %a", 1, pIndicateConnectMsg->pAddressDevice);
		  break;

		default:
			TRACE(NULL, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
}

//==================================================================================
// PeerSendToHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT PeerSendToHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT					hr = DPN_OK;
	PPEERSENDTOCONTEXT		pContext = (PPEERSENDTOCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_RECEIVE:
			PDPNMSG_RECEIVE		pReceiveMsg;


			pReceiveMsg = (PDPNMSG_RECEIVE) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_RECEIVE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pReceiveMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidSender = %u/%x", 2, pReceiveMsg->dpnidSender, pReceiveMsg->dpnidSender);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pReceiveMsg->pvPlayerContext);
			TRACE(pContext->hLog, "     pReceiveData = %x", 1, pReceiveMsg->pReceiveData);
			TRACE(pContext->hLog, "     dwReceiveDataSize = %u", 1, pReceiveMsg->dwReceiveDataSize);
			TRACE(pContext->hLog, "     hBufferHandle = %x", 1, pReceiveMsg->hBufferHandle);


			// Make sure we expect the receive.
			if (! pContext->fCanReceive)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_RECEIVE (%x)!  DEBUGBREAK()-ing.",
					1, pReceiveMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the receive.
			if (pReceiveMsg->dwSize != sizeof (DPNMSG_RECEIVE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_RECEIVE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pReceiveMsg->dwSize, sizeof (DPNMSG_RECEIVE));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

#pragma TODO(vanceo, "Validate player ID")

			if (pReceiveMsg->pvPlayerContext != NULL)
			{
				TRACE(pContext->hLog, "Receive indicated with wrong player context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pReceiveMsg->pvPlayerContext);
				DEBUGBREAK();
				break;
			} // end if (player context incorrect)

			if (pReceiveMsg->pReceiveData == NULL)
			{
				TRACE(pContext->hLog, "Receive indicated with NULL data pointer!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			} // end if (receive buffer invalid)

			if (pReceiveMsg->dwReceiveDataSize != pContext->dwExpectedReceiveSize)
			{
				TRACE(pContext->hLog, "Receive indicated with wrong size (%u != %u)!  DEBUGBREAK()-ing.",
					2, pReceiveMsg->dwReceiveDataSize, pContext->dwExpectedReceiveSize);
				DEBUGBREAK();
				break;
			} // end if (wrong buffer size)

#pragma TODO(vanceo, "Validate buffer")


			if (pReceiveMsg->hBufferHandle == NULL)
			{
				TRACE(pContext->hLog, "Receive indicated with NULL buffer handle!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			} // end if (buffer handle invalid)



			// Make sure this isn't a double receive.
			if (pContext->fReceived)
			{
				TRACE(pContext->hLog, "Receive indicated twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pReceiveMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the receive came in.
			pContext->fReceived = TRUE;

			if (! SetEvent(pContext->hReceiveEvent))
			{
				hr = GetLastError();
				TRACE(pContext->hLog, "Couldn't set receive event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hReceiveEvent);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				break;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_SEND_COMPLETE:
			PDPNMSG_SEND_COMPLETE	pSendCompleteMsg;


			pSendCompleteMsg = (PDPNMSG_SEND_COMPLETE) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_SEND_COMPLETE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pSendCompleteMsg->dwSize);
			TRACE(pContext->hLog, "     hAsyncOp = %x", 1, pSendCompleteMsg->hAsyncOp);
			TRACE(pContext->hLog, "     pvUserContext = %x", 1, pSendCompleteMsg->pvUserContext);
			TRACE(pContext->hLog, "     hResultCode = %e", 1, pSendCompleteMsg->hResultCode);
			TRACE(pContext->hLog, "     dwSendTime = %u", 1, pSendCompleteMsg->dwSendTime);


			// Make sure we expect the completion.
			if (! pContext->fSendCanComplete)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_SEND_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pSendCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the completion.
			if (pSendCompleteMsg->dwSize != sizeof (DPNMSG_SEND_COMPLETE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_SEND_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pSendCompleteMsg->dwSize, sizeof (DPNMSG_SEND_COMPLETE));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pSendCompleteMsg->pvUserContext != NULL)
			{
				TRACE(pContext->hLog, "Send completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pSendCompleteMsg->pvUserContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)

			if (pSendCompleteMsg->hResultCode != pContext->hrExpectedResult)
			{
				TRACE(pContext->hLog, "Send completed with wrong error code (%x != %x)!  DEBUGBREAK()-ing.",
					2, pSendCompleteMsg->hResultCode, pContext->hrExpectedResult);
				DEBUGBREAK();
				break;
			} // end if (wrong result code)



			// Make sure this isn't a double completion.
			if (pContext->fSendCompleted)
			{
				TRACE(pContext->hLog, "Send completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pSendCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fSendCompleted = TRUE;

			// Remember the async op handle for comparison by the main thread.
			pContext->dpnhCompletedAsyncOp = pSendCompleteMsg->hAsyncOp;

			if (pContext->hSendCompleteEvent != NULL)
			{
				if (! SetEvent(pContext->hSendCompleteEvent))
				{
					hr = GetLastError();
					TRACE(pContext->hLog, "Couldn't set send completion event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hSendCompleteEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					break;
				} // end if (couldn't set event)
			} // end if (there's an event to set)
		  break;

		default:
			TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
}

//==================================================================================
// PeerGetSendQInfoHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT PeerGetSendQInfoHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PPEERGETSENDQINFOCONTEXT	pContext = (PPEERGETSENDQINFOCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;
			
			TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			TRACE(pContext->hLog, "     hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			TRACE(pContext->hLog, "     pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			TRACE(pContext->hLog, "     hResultCode = %e", 1, pConnectCompleteMsg->hResultCode);
			TRACE(pContext->hLog, "     pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			TRACE(pContext->hLog, "     dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);

			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pConnectCompleteMsg->hAsyncOp != NULL)
			{
				TRACE(pContext->hLog, "Connect completed with an async op handle (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				break;
			} // end if (wrong op handle)

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				TRACE(pContext->hLog, "Connect completed with failure!  DEBUGBREAK()-ing.  %e",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				break;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				break;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			TRACE(pContext->hLog, "    pAddressPlayer = %a", 1, pIndicateConnectMsg->pAddressPlayer);
			TRACE(pContext->hLog, "    pAddressDevice = %a", 1, pIndicateConnectMsg->pAddressDevice);

#pragma TODO(vanceo, "Validate DPN_MSGID_INDICATE_CONNECT")
		  break;

		default:
			TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
}

//==================================================================================
// PeerHostHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT PeerHostHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT				hr = DPN_OK;
	PPEERHOSTCONTEXT	pContext = (PPEERHOSTCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		default:
			TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
}

//==================================================================================
// PeerSetAppDescHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT PeerSetAppDescHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PPEERSETAPPDESCCONTEXT		pContext = (PPEERSETAPPDESCCONTEXT) pvContext;

	switch (dwMsgType)
	{
		case DPN_MSGID_APPLICATION_DESC:
			// Make sure we expect the indication.
			if (! pContext->fCanGetAppDescUpdate)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_APPLICATION_DESC!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Make sure this isn't a double indication.
			if (pContext->fGotAppDescUpdate)
			{
				TRACE(pContext->hLog, "App desc update was indicated twice!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the indication came in.
			pContext->fGotAppDescUpdate = TRUE;

			if (! SetEvent(pContext->hGotAppDescUpdateEvent))
			{
				hr = GetLastError();
				TRACE(pContext->hLog, "Couldn't set got app desc update event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hGotAppDescUpdateEvent);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				break;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			TRACE(pContext->hLog, "     hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			TRACE(pContext->hLog, "     pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			TRACE(pContext->hLog, "     hResultCode = %e", 1, pConnectCompleteMsg->hResultCode);
			TRACE(pContext->hLog, "     pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			TRACE(pContext->hLog, "     dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pConnectCompleteMsg->hAsyncOp != NULL)
			{
				TRACE(pContext->hLog, "Connect completed with an async op handle (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				break;
			} // end if (wrong op handle)

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				TRACE(pContext->hLog, "Connect completed with failure!  DEBUGBREAK()-ing.  %e",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				break;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				break;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			TRACE(pContext->hLog, "    pAddressPlayer = %a", 1, pIndicateConnectMsg->pAddressPlayer);
			TRACE(pContext->hLog, "    pAddressDevice = %a", 1, pIndicateConnectMsg->pAddressDevice);

#pragma TODO(vanceo, "Validate DPN_MSGID_INDICATE_CONNECT")
		  break;

		default:
			TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
}

//==================================================================================
// PeerCreateGroupHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT PeerCreateGroupHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PPEERCREATEGROUPCONTEXT		pContext = (PPEERCREATEGROUPCONTEXT) pvContext;

	switch (dwMsgType)
	{
		case DPN_MSGID_ASYNC_OP_COMPLETE:
			PDPNMSG_ASYNC_OP_COMPLETE	pAsyncOpCompleteMsg;


			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_ASYNC_OP_COMPLETE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pAsyncOpCompleteMsg->dwSize);
			TRACE(pContext->hLog, "     hAsyncOp = %x", 1, pAsyncOpCompleteMsg->hAsyncOp);
			TRACE(pContext->hLog, "     pvUserContext = %x", 1, pAsyncOpCompleteMsg->pvUserContext);
			TRACE(pContext->hLog, "     hResultCode = %e", 1, pAsyncOpCompleteMsg->hResultCode);


			// Make sure we expect the completion.
			if (! pContext->fAsyncOpCanComplete)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_ASYNC_OP_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pAsyncOpCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the completion.
			if (pAsyncOpCompleteMsg->dwSize != sizeof (DPNMSG_ASYNC_OP_COMPLETE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_ASYNC_OP_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pAsyncOpCompleteMsg->dwSize, sizeof (DPNMSG_ASYNC_OP_COMPLETE));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pAsyncOpCompleteMsg->pvUserContext != NULL)
			{
				TRACE(pContext->hLog, "Async op completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pAsyncOpCompleteMsg->pvUserContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)

			if (pAsyncOpCompleteMsg->hResultCode != DPN_OK)
			{
				TRACE(pContext->hLog, "Async op completed with failure!  DEBUGBREAK()-ing.  %e",
					1, pAsyncOpCompleteMsg->hResultCode);
				DEBUGBREAK();
				break;
			} // end if (wrong result code)


			// Make sure this isn't a double completion.
			if (pContext->fAsyncOpCompleted)
			{
				TRACE(pContext->hLog, "Async op completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pAsyncOpCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fAsyncOpCompleted = TRUE;

			// Remember the async op handle for comparison by the main thread.
			pContext->dpnhCompletedAsyncOp = pAsyncOpCompleteMsg->hAsyncOp;

			if (! SetEvent(pContext->hAsyncOpCompletedEvent))
			{
				hr = GetLastError();
				TRACE(pContext->hLog, "Couldn't set async op completed event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hAsyncOpCompletedEvent);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				break;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			TRACE(pContext->hLog, "     hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			TRACE(pContext->hLog, "     pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			TRACE(pContext->hLog, "     hResultCode = %e", 1, pConnectCompleteMsg->hResultCode);
			TRACE(pContext->hLog, "     pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			TRACE(pContext->hLog, "     dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pConnectCompleteMsg->hAsyncOp != NULL)
			{
				TRACE(pContext->hLog, "Connect completed with an async op handle (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				break;
			} // end if (wrong op handle)

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				TRACE(pContext->hLog, "Connect completed with failure!  DEBUGBREAK()-ing.  %e",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				break;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				break;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;
		  break;

		case DPN_MSGID_CREATE_GROUP:
			PDPNMSG_CREATE_GROUP	pCreateGroupMsg;


			pCreateGroupMsg = (PDPNMSG_CREATE_GROUP) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CREATE_GROUP", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pCreateGroupMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidGroup = %u/%x", 2, pCreateGroupMsg->dpnidGroup, pCreateGroupMsg->dpnidGroup);
			TRACE(pContext->hLog, "     dpnidOwner = %u/%x", 2, pCreateGroupMsg->dpnidOwner, pCreateGroupMsg->dpnidOwner);
			TRACE(pContext->hLog, "     pvGroupContext = %x", 1, pCreateGroupMsg->pvGroupContext);


			// Make sure we expect the indication.
			if (! pContext->fCanGetCreateGroup)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CREATE_GROUP (at %x)!  DEBUGBREAK()-ing.",
					1, pCreateGroupMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)

			// Validate the message.
			if (pCreateGroupMsg->dpnidGroup == 0)
			{
				TRACE(pContext->hLog, "Group created got invalid ID of 0!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			} // end if (group ID is 0)

			if (pCreateGroupMsg->dpnidOwner != pContext->dpnidExpectedGroupOwner)
			{
				TRACE(pContext->hLog, "Group owner was wrong (%u/%x != %u/%x)!  DEBUGBREAK()-ing.",
					4, pCreateGroupMsg->dpnidOwner, pCreateGroupMsg->dpnidOwner,
					pContext->dpnidExpectedGroupOwner, pContext->dpnidExpectedGroupOwner);
				DEBUGBREAK();
				break;
			} // end if (group owner is wrong)

			if (pCreateGroupMsg->pvGroupContext != NULL)
			{
				TRACE(pContext->hLog, "Group has wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pCreateGroupMsg->pvGroupContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)



			// Make sure this isn't a double indication.
			if (pContext->dpnidGroupCreated != 0)
			{
				TRACE(pContext->hLog, "Got create group twice!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the indication came in.
			pContext->dpnidGroupCreated = pCreateGroupMsg->dpnidGroup;

			if (pContext->hGotCreateGroupEvent != NULL)
			{
				if (! SetEvent(pContext->hGotCreateGroupEvent))
				{
					hr = GetLastError();
					TRACE(pContext->hLog, "Couldn't set got create group event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hGotCreateGroupEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					break;
				} // end if (couldn't set event)
			} // end if (there's a create group event)
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_GROUP:
			PDPNMSG_DESTROY_GROUP	pDestroyGroupMsg;


			pDestroyGroupMsg = (PDPNMSG_DESTROY_GROUP) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_DESTROY_GROUP", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyGroupMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidGroup = %u/%x", 2, pDestroyGroupMsg->dpnidGroup, pDestroyGroupMsg->dpnidGroup);
			TRACE(pContext->hLog, "     pvGroupContext = %x", 1, pDestroyGroupMsg->pvGroupContext);
			TRACE(pContext->hLog, "     dwReason = %u", 1, pDestroyGroupMsg->dwReason);

#pragma TODO(vanceo, "Handle/validate destroy group")
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			TRACE(pContext->hLog, "    pAddressPlayer = %a", 1, pIndicateConnectMsg->pAddressPlayer);
			TRACE(pContext->hLog, "    pAddressDevice = %a", 1, pIndicateConnectMsg->pAddressDevice);

#pragma TODO(vanceo, "Validate DPN_MSGID_INDICATE_CONNECT")
		  break;

		default:
			TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
}

//==================================================================================
// PeerCreateGroupHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT PeerCreateGroupHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;
	PPEERDESTROYGROUPCONTEXT	pContext = (PPEERDESTROYGROUPCONTEXT) pvContext;


	switch (dwMsgType)
	{
		case DPN_MSGID_ASYNC_OP_COMPLETE:
			PDPNMSG_ASYNC_OP_COMPLETE	pAsyncOpCompleteMsg;


			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_ASYNC_OP_COMPLETE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pAsyncOpCompleteMsg->dwSize);
			TRACE(pContext->hLog, "     hAsyncOp = %x", 1, pAsyncOpCompleteMsg->hAsyncOp);
			TRACE(pContext->hLog, "     pvUserContext = %x", 1, pAsyncOpCompleteMsg->pvUserContext);
			TRACE(pContext->hLog, "     hResultCode = %e", 1, pAsyncOpCompleteMsg->hResultCode);


			// Make sure we expect the completion.
			if (! pContext->fAsyncOpCanComplete)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_ASYNC_OP_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pAsyncOpCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the completion.
			if (pAsyncOpCompleteMsg->dwSize != sizeof (DPNMSG_ASYNC_OP_COMPLETE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_ASYNC_OP_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pAsyncOpCompleteMsg->dwSize, sizeof (DPNMSG_ASYNC_OP_COMPLETE));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pAsyncOpCompleteMsg->pvUserContext != NULL)
			{
				TRACE(pContext->hLog, "Async op completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pAsyncOpCompleteMsg->pvUserContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)

			if (pAsyncOpCompleteMsg->hResultCode != DPN_OK)
			{
				TRACE(pContext->hLog, "Async op completed with failure!  DEBUGBREAK()-ing.  %e",
					1, pAsyncOpCompleteMsg->hResultCode);
				DEBUGBREAK();
				break;
			} // end if (wrong result code)


			// Make sure this isn't a double completion.
			if (pContext->fAsyncOpCompleted)
			{
				TRACE(pContext->hLog, "Async op completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pAsyncOpCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fAsyncOpCompleted = TRUE;

			// Remember the async op handle for comparison by the main thread.
			pContext->dpnhCompletedAsyncOp = pAsyncOpCompleteMsg->hAsyncOp;

			if (! SetEvent(pContext->hAsyncOpCompletedEvent))
			{
				hr = GetLastError();
				TRACE(pContext->hLog, "Couldn't set async op completed event (%x)!  DEBUGBREAK()-ing.",
					1, pContext->hAsyncOpCompletedEvent);

				DEBUGBREAK();

				if (hr == S_OK)
					hr = E_FAIL;

				break;
			} // end if (couldn't set event)
		  break;

		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE	pConnectCompleteMsg;


			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CONNECT_COMPLETE", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pConnectCompleteMsg->dwSize);
			TRACE(pContext->hLog, "     hAsyncOp = %x", 1, pConnectCompleteMsg->hAsyncOp);
			TRACE(pContext->hLog, "     pvUserContext = %x", 1, pConnectCompleteMsg->pvUserContext);
			TRACE(pContext->hLog, "     hResultCode = %e", 1, pConnectCompleteMsg->hResultCode);
			TRACE(pContext->hLog, "     pvApplicationReplyData = %x", 1, pConnectCompleteMsg->pvApplicationReplyData);
			TRACE(pContext->hLog, "     dwApplicationReplyDataSize = %u", 1, pConnectCompleteMsg->dwApplicationReplyDataSize);


			// Make sure we expect the completion.
			if (! pContext->fConnectCanComplete)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CONNECT_COMPLETE (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the completion.
			if (pConnectCompleteMsg->dwSize != sizeof (DPNMSG_CONNECT_COMPLETE))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_CONNECT_COMPLETE is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->dwSize, sizeof (DPNMSG_CONNECT_COMPLETE));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pConnectCompleteMsg->hAsyncOp != NULL)
			{
				TRACE(pContext->hLog, "Connect completed with an async op handle (%x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->hAsyncOp);
				DEBUGBREAK();
				break;
			} // end if (wrong op handle)

			if (pConnectCompleteMsg->pvUserContext != NULL)
			{
				TRACE(pContext->hLog, "Connect completed with wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg->pvUserContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)

			if (pConnectCompleteMsg->hResultCode != DPN_OK)
			{
				TRACE(pContext->hLog, "Connect completed with failure!  DEBUGBREAK()-ing.  %e",
					1, pConnectCompleteMsg->hResultCode);
				DEBUGBREAK();
				break;
			} // end if (wrong result code)

			if ((pConnectCompleteMsg->pvApplicationReplyData != NULL) ||
				(pConnectCompleteMsg->dwApplicationReplyDataSize != 0))
			{
				TRACE(pContext->hLog, "Connect completed with unexpected reply data (ptr %x != NULL or size %u != 0)!  DEBUGBREAK()-ing.",
					2, pConnectCompleteMsg->pvApplicationReplyData,
					pConnectCompleteMsg->dwApplicationReplyDataSize);
				DEBUGBREAK();
				break;
			} // end if (unexpected reply data)


			// Make sure this isn't a double completion.
			if (pContext->fConnectCompleted)
			{
				TRACE(pContext->hLog, "Connect completed twice (msg = %x)!  DEBUGBREAK()-ing.",
					1, pConnectCompleteMsg);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the completion came in.
			pContext->fConnectCompleted = TRUE;
		  break;

		case DPN_MSGID_CREATE_GROUP:
			PDPNMSG_CREATE_GROUP	pCreateGroupMsg;


			pCreateGroupMsg = (PDPNMSG_CREATE_GROUP) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CREATE_GROUP", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pCreateGroupMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidGroup = %u/%x", 2, pCreateGroupMsg->dpnidGroup, pCreateGroupMsg->dpnidGroup);
			TRACE(pContext->hLog, "     dpnidOwner = %u/%x", 2, pCreateGroupMsg->dpnidOwner, pCreateGroupMsg->dpnidOwner);
			TRACE(pContext->hLog, "     pvGroupContext = %x", 1, pCreateGroupMsg->pvGroupContext);


			// Make sure we expect the indication.
			if (! pContext->fCanGetCreateGroup)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_CREATE_GROUP (at %x)!  DEBUGBREAK()-ing.",
					1, pCreateGroupMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)

			// Validate the message.
			if (pCreateGroupMsg->dwSize != sizeof (DPNMSG_CREATE_GROUP))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_CREATE_GROUP is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pCreateGroupMsg->dwSize, sizeof (DPNMSG_CREATE_GROUP));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pCreateGroupMsg->dpnidGroup == 0)
			{
				TRACE(pContext->hLog, "Group created got invalid ID of 0!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			} // end if (group ID is 0)

			if (pCreateGroupMsg->dpnidOwner != pContext->dpnidExpectedGroupOwner)
			{
				TRACE(pContext->hLog, "Group owner was wrong (%u/%x != %u/%x)!  DEBUGBREAK()-ing.",
					4, pCreateGroupMsg->dpnidOwner, pCreateGroupMsg->dpnidOwner,
					pContext->dpnidExpectedGroupOwner, pContext->dpnidExpectedGroupOwner);
				DEBUGBREAK();
				break;
			} // end if (group owner is wrong)

			if (pCreateGroupMsg->pvGroupContext != NULL)
			{
				TRACE(pContext->hLog, "Group has wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pCreateGroupMsg->pvGroupContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)



			// Make sure this isn't a double indication.
			if (pContext->dpnidGroup != 0)
			{
				TRACE(pContext->hLog, "Got create group twice!  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
				break;
			} // end if (double completion)

			// Alert the test case that the indication came in.
			pContext->dpnidGroup = pCreateGroupMsg->dpnidGroup;

			if (pContext->hGotGroupMsgEvent != NULL)
			{
				if (! SetEvent(pContext->hGotGroupMsgEvent))
				{
					hr = GetLastError();
					TRACE(pContext->hLog, "Couldn't set got group msg event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hGotGroupMsgEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					break;
				} // end if (couldn't set event)
			} // end if (there's a group msg event)
		  break;

		case DPN_MSGID_CREATE_PLAYER:
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;


			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_CREATE_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pCreatePlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pCreatePlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate CREATE_PLAYER")
		  break;

		case DPN_MSGID_DESTROY_GROUP:
			PDPNMSG_DESTROY_GROUP	pDestroyGroupMsg;


			pDestroyGroupMsg = (PDPNMSG_DESTROY_GROUP) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_DESTROY_GROUP", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyGroupMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidGroup = %u/%x", 2, pDestroyGroupMsg->dpnidGroup, pDestroyGroupMsg->dpnidGroup);
			TRACE(pContext->hLog, "     pvGroupContext = %x", 1, pDestroyGroupMsg->pvGroupContext);
			TRACE(pContext->hLog, "     dwReason = %u", 1, pDestroyGroupMsg->dwReason);


			// Make sure we expect the indication.
			if (! pContext->fCanGetDestroyGroup)
			{
				TRACE(pContext->hLog, "Got unexpected DPN_MSGID_DESTROY_GROUP (at %x)!  DEBUGBREAK()-ing.",
					1, pDestroyGroupMsg);
				DEBUGBREAK();
				break;
			} // end if (message unexpected)


			// Validate the message.
			if (pDestroyGroupMsg->dwSize != sizeof (DPNMSG_DESTROY_GROUP))
			{
				TRACE(pContext->hLog, "Size of DPNMSG_DESTROY_GROUP is incorrect (%u != %u)!  DEBUGBREAK()-ing.",
					2, pDestroyGroupMsg->dwSize, sizeof (DPNMSG_DESTROY_GROUP));
				DEBUGBREAK();
				break;
			} // end if (size incorrect)

			if (pDestroyGroupMsg->dpnidGroup != pContext->dpnidGroup)
			{
				TRACE(pContext->hLog, "Group being destroy has wrong ID (%u/%x != %u/%x)!  DEBUGBREAK()-ing.",
					4, pDestroyGroupMsg->dpnidGroup, pDestroyGroupMsg->dpnidGroup,
					pContext->dpnidGroup, pContext->dpnidGroup);
				DEBUGBREAK();
				break;
			} // end if (group ID is wrong)

			if (pDestroyGroupMsg->pvGroupContext != NULL)
			{
				TRACE(pContext->hLog, "Group has wrong context (%x != NULL)!  DEBUGBREAK()-ing.",
					1, pDestroyGroupMsg->pvGroupContext);
				DEBUGBREAK();
				break;
			} // end if (wrong context)

			if (pDestroyGroupMsg->dwReason != DPNDESTROYGROUPREASON_NORMAL)
			{
				TRACE(pContext->hLog, "Reason group being destroyed is wrong (%x != %x)!  DEBUGBREAK()-ing.",
					2, pDestroyGroupMsg->dwReason, DPNDESTROYGROUPREASON_NORMAL);
				DEBUGBREAK();
				break;
			} // end if (reason is wrong)

			// Alert the test case that the indication came in.
			pContext->dpnidGroup = 0;

			if (pContext->hGotGroupMsgEvent != NULL)
			{
				if (! SetEvent(pContext->hGotGroupMsgEvent))
				{
					hr = GetLastError();
					TRACE(pContext->hLog, "Couldn't set got group msg event (%x)!  DEBUGBREAK()-ing.",
						1, pContext->hGotGroupMsgEvent);

					DEBUGBREAK();

					if (hr == S_OK)
						hr = E_FAIL;

					break;
				} // end if (couldn't set event)
			} // end if (there's a group msg event)
		  break;

		case DPN_MSGID_DESTROY_PLAYER:
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;


			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_DESTROY_PLAYER", 0);
			TRACE(pContext->hLog, "     dwSize = %u", 1, pDestroyPlayerMsg->dwSize);
			TRACE(pContext->hLog, "     dpnidPlayer = %u/%x", 2, pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
			TRACE(pContext->hLog, "     pvPlayerContext = %x", 1, pDestroyPlayerMsg->pvPlayerContext);

#pragma TODO(vanceo, "Validate DPN_MSGID_DESTROY_PLAYER")
		  break;

		case DPN_MSGID_INDICATE_CONNECT:
			PDPNMSG_INDICATE_CONNECT	pIndicateConnectMsg;


			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;

			TRACE(pContext->hLog, "DPN_MSGID_INDICATE_CONNECT", 0);
			TRACE(pContext->hLog, "    dwSize = %u", 1, pIndicateConnectMsg->dwSize);
			TRACE(pContext->hLog, "    pvUserConnectData = %x", 1, pIndicateConnectMsg->pvUserConnectData);
			TRACE(pContext->hLog, "    dwUserConnectDataSize = %u", 1, pIndicateConnectMsg->dwUserConnectDataSize);
			TRACE(pContext->hLog, "    pvReplyData = %x", 1, pIndicateConnectMsg->pvReplyData);
			TRACE(pContext->hLog, "    dwReplyDataSize = %u", 1, pIndicateConnectMsg->dwReplyDataSize);
			TRACE(pContext->hLog, "    pvReplyContext = %x", 1, pIndicateConnectMsg->pvReplyContext);
			TRACE(pContext->hLog, "    pvPlayerContext = %x", 1, pIndicateConnectMsg->pvPlayerContext);
			TRACE(pContext->hLog, "    pAddressPlayer = %a", 1, pIndicateConnectMsg->pAddressPlayer);
			TRACE(pContext->hLog, "    pAddressDevice = %a", 1, pIndicateConnectMsg->pAddressDevice);

#pragma TODO(vanceo, "Validate DPN_MSGID_INDICATE_CONNECT")
		  break;

		default:
			TRACE(pContext->hLog, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
				1, dwMsgType);
			DEBUGBREAK();
			hr = E_NOTIMPL;
		  break;
	} // end switch (on message type)

	return (hr);
}

//==================================================================================
// Test case functions
//==================================================================================

//==================================================================================
// PeerParamsDPCreateTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.1 - Peer DirectPlay8Create parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
BOOL PeerParamsDPCreateTests(HANDLE hLog)
{
	HRESULT	hr;
	LPVOID pPeer, pVoice;

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "DirectPlay8PeerCreate with both pointers NULL"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8PeerCreate with both pointers null");
	
	hr = DirectPlay8PeerCreate(NULL, NULL);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "Didn't fail with expected error INVALIDPOINTER!");
	else
		xLog(hLog, XLL_PASS, "Returned INVALIDPOINTER successfully");

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "DirectPlay8PeerCreate with voice pointer NULL"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8PeerCreate with voice pointer null");
	
	pPeer = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, NULL);
	if (hr != DPNERR_INVALIDPOINTER)
	{
		xLog(hLog, XLL_FAIL, "Didn't fail with expected error INVALIDPOINTER!");
		((PDIRECTPLAY8PEER) pPeer)->Release();
	}
	else
		xLog(hLog, XLL_PASS, "Returned INVALIDPOINTER successfully");

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "DirectPlay8PeerCreate with peer pointer NULL"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8PeerCreate with peer pointer null");

	pVoice = NULL
	hr = DirectPlay8PeerCreate(NULL, &pVoice);
	if (hr != DPNERR_INVALIDPOINTER)
	{
		xLog(hLog, XLL_FAIL, "Didn't fail with expected error INVALIDPOINTER!");
		// TODO - Need to release the voice pointer
	}
	else
		xLog(hLog, XLL_PASS, "Returned INVALIDPOINTER successfully");

	xEndVariation(hLog);

#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "DirectPlay8PeerCreate with both pointers valid");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8PeerCreate with peer pointer null");

	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Failed with %u", hr);
	else
	{
		xLog(hLog, XLL_PASS, "Returned S_OK successfully");
		((PDIRECTPLAY8PEER) pPeer)->Release();
	}

	xEndVariation(hLog);
		
	return TRUE;
}

//==================================================================================
// PeerParamsInitTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.2 - Peer Initialize parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
BOOL PeerParamsInitTests(HANDLE hLog)
{
	PDIRECTPLAY8PEER pPeer;
	HRESULT	hr;
	LPVOID pVoice;
	BOOL fSuccess = TRUE;
	
	// Creating DirectPlay8Peer object
	pPeer = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Initializing with NULL context, handler, 0 flags using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Initialize with NULL context, handler, 0 flags using C++ macro");

	hr = IDirectPlay8Peer_Initialize(pPeer, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
	{
		xLog(hLog, XLL_FAIL, "Initialize didn't fail with expected error INVALIDPOINTER!");

		// This peer interface is tainted now, release it and get a new one before moving on to the next test
		pPeer ? pPeer->Release() : 0;
		pPeer = NULL;

		// TODO - release the voice pointer

		if((hr = DirectPlay8PeerCreate(&pPeer, &pVoice)) != S_OK)
		{
			xEndVariation(hLog);
			
			xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
			fSuccess = FALSE;
			goto Exit;
		}
	}
	else
		xLog(hLog, XLL_PASS, "Initialize returned INVALIDPOINTER successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Initializing with NULL context, handler, 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Initialize with NULL context, handler, 0 flags");
	
	hr = pPeer->Initialize(NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
	{
		xLog(hLog, XLL_FAIL, "Initialize didn't fail with expected error INVALIDPOINTER!");

		// This peer interface is tainted now, release it and get a new one before moving on to the next test
		pPeer ? pPeer->Release() : 0;
		pPeer = NULL;

		// TODO - release the voice pointer

		if((hr = DirectPlay8PeerCreate(&pPeer, &pVoice)) != S_OK)
		{
			xEndVariation(hLog);

			xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
			fSuccess = FALSE;
			goto Exit;
		}
	}
	else
		xLog(hLog, XLL_PASS, "Initialize returned INVALIDPOINTER successfully");
	
	xEndVariation(hLog);

#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Initializing with NULL context, invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Initialize with NULL context, invalid flags");
	
	hr = pPeer->Initialize(NULL, PeerNoMessagesHandler, 0x666);
	if (hr != DPNERR_INVALIDFLAGS)
	{
		xLog(hLog, XLL_FAIL, "Initialize didn't fail with expected error INVALIDPOINTER!");

		// This peer interface is tainted now, release it and get a new one before moving on to the next test
		pPeer ? pPeer->Release() : 0;
		pPeer = NULL;

		// TODO - release the voice pointer

		if((hr = DirectPlay8PeerCreate(&pPeer, &pVoice)) != S_OK)
		{
			xEndVariation(hLog);

			xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
			fSuccess = FALSE;
			goto Exit;
		}
	}
	else
		xLog(hLog, XLL_PASS, "Initialize returned INVALIDPOINTER successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Initializing with 0 flags using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Initialize with 0 flags using C++ macro");

	hr = IDirectPlay8Peer_Initialize(pPeer, NULL, PeerNoMessagesHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing object using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Close using C++ macro");
	
	hr = IDirectPlay8Peer_Close(pPeer, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Initializing with 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Initialize with 0 flags");
	
	hr = pPeer->Initialize(NULL, PeerNoMessagesHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Initializing with 0 flags again"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Initialize with 0 flags again");
	
	hr = pPeer->Initialize(NULL, PeerNoMessagesHandler, 0);
	if (hr != DPNERR_ALREADYINITIALIZED)
		xLog(hLog, XLL_FAIL, "Initialize didn't fail with expected error ALREADYINITIALIZED!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned ALREADYINITIALIZED successfully");
	
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Close after initializing");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);	
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Re-initializing with 0 flags after init and close"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Initialize with 0 flags after init and close");
	
	hr = pPeer->Initialize(NULL, PeerNoMessagesHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");	
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Close after third initialization");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Release after three init and closes");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeer = NULL;

	xEndVariation(hLog);	
	
Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// TODO - Need to release the voice interface as well

	return fSuccess;
}

//==================================================================================
// PeerParamsEnumSPTests
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.3 - Peer EnumServiceProviders parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
BOOL PeerParamsEnumSPTests(HANDLE hLog)
{
	PDPN_SERVICE_PROVIDER_INFO	pSPInfo;
	PDIRECTPLAY8PEER pPeer;
	HRESULT	hr;
	LPVOID pVoice;
	DWORD dwNumSPs, dwSPBufferSize, dwExpectedNumSPs, dwExpectedSPBufferSize;
	BOOL fSuccess = TRUE;
	
	// Creating DirectPlay8Peer object
	pPeer = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");
	
	
#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating service providers with all NULLs and 0 flags using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with all NULLs and 0 flags using C++ macro");

	hr = IDirectPlay8Peer_EnumServiceProviders(pPeer, NULL, NULL, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error INVALIDPOINTER!");
	else
		xLog(hLog, XLL_PASS, "EnumSP returned INVALIDPOINTER successfully");
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating service providers with all NULLs and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with all NULLs and 0 flags");
	
	hr = pPeer->EnumServiceProviders(NULL, NULL, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error INVALIDPOINTER!");
	else
		xLog(hLog, XLL_PASS, "EnumSP returned INVALIDPOINTER successfully");
	
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating service providers with NULL GUIDs, NULL buffer, NULL size, and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with NULL GUIDs, NULL buffer, NULL size, and 0 flags");
	
	dwNumSPs = 0;
	hr = pPeer->EnumServiceProviders(NULL, NULL, NULL, NULL, &dwNumSPs, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error INVALIDPOINTER!");
	else
	{
		xLog(hLog, XLL_INFO, "EnumSP returned INVALIDPOINTER successfully");

		if (dwNumSPs != 0)
			xLog(hLog, XLL_FAIL, "Number of SPs modified (%u != 0)!", dwNumSPs);
		else
			xLog(hLog, XLL_PASS, "Number of SPs wasn't modified");
	}
		
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating service providers with NULL GUIDs, NULL buffer, NULL count, and 0 flags");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with NULL GUIDs, NULL buffer, NULL count, and 0 flags");
	
	dwSPBufferSize = 0;
	hr = pPeer->EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error INVALIDPOINTER!");
	else
	{
		xLog(hLog, XLL_INFO, "EnumSP returned INVALIDPOINTER successfully");

		if (dwSPBufferSize != 0)
			xLog(hLog, XLL_FAIL, "Buffer size modified (%u != 0)!", dwNumSPs);
		else
			xLog(hLog, XLL_PASS, "Buffer size wasn't modified");
	}
		
	xEndVariation(hLog);	
	
#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating service providers with NULL GUIDs, NULL buffer, and invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with NULL GUIDs, NULL buffer, and invalid flags");
	
	dwSPBufferSize = 0;
	dwNumSPs = 0;
	hr = pPeer->EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize, &dwNumSPs, 0x666);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error INVALIDFLAGS!");
	else
	{
		xLog(hLog, XLL_INFO, "EnumSP returned INVALIDFLAGS successfully");

		if (dwSPBufferSize != 0)
			xLog(hLog, XLL_FAIL, "Buffer size modified (%u != 0)!", dwSPBufferSize);
		else
		{
			xLog(hLog, XLL_INFO, "Buffer size wasn't modified");
			if(dwNumSPs != 0)
				xLog(hLog, XLL_FAIL, "Number of SPs modified (%u != 0)!", dwNumSPs);
			else
				xLog(hLog, XLL_PASS, "Number of SPs wasn't modified");
		}
	}
		
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Initializing with NULL context, 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP-Initialize with NULL context, 0 flags");
	
	hr = pPeer->Initialize(NULL, PeerNoMessagesHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");	

	xEndVariation(hLog);	

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating service providers with NULL GUIDs, NULL buffer, non-zero size, and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with NULL GUIDs, NULL buffer, non-zero size, and 0 flags");
	
	dwSPBufferSize = 12345;
	dwNumSPs = 0;
	hr = pPeer->EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize, &dwNumSPs, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error INVALIDPOINTER!");
	else
	{
		xLog(hLog, XLL_INFO, "EnumSP returned INVALIDPOINTER successfully");

		if (dwSPBufferSize != 12345)
			xLog(hLog, XLL_FAIL, "Buffer size modified (%u != 12345)!", dwSPBufferSize);
		else
		{
			xLog(hLog, XLL_INFO, "Buffer size wasn't modified");
			if(dwNumSPs != 0)
				xLog(hLog, XLL_FAIL, "Number of SPs modified (%u != 0)!", dwNumSPs);
			else
				xLog(hLog, XLL_PASS, "Number of SPs wasn't modified");
		}
	}

	xEndVariation(hLog);	
	
#endif // BAD_PARAM_TESTS
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating service providers with NULL GUIDs, NULL buffer, and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with NULL GUIDs, NULL buffer, and 0 flags");
	
	dwSPBufferSize = 0;
	// We expect to see at least the IP Service Provider.  We could technically
	// make sure there's room for the string, too, but that would have to be
	// localized.
	dwExpectedSPBufferSize = sizeof (DPN_SERVICE_PROVIDER_INFO);
	dwNumSPs = 0;
	hr = pPeer->EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize, &dwNumSPs, 0);
	if (hr != DPNERR_BUFFERTOOSMALL)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error BUFFERTOOSMALL!");
	else
	{
		xLog(hLog, XLL_INFO, "EnumSP returned BUFFERTOOSMALL successfully");

		if (dwSPBufferSize <= dwExpectedSPBufferSize)
			xLog(hLog, XLL_FAIL, "Buffer size smaller than expected (%u <= %u)!", dwSPBufferSize, dwExpectedSPBufferSize);
		else
		{
			xLog(hLog, XLL_INFO, "Buffer size wasn't smaller than expected");
			if(dwNumSPs < 1)
				xLog(hLog, XLL_FAIL, "Number of SPs not expected (%u < 1)!", dwNumSPs);
			else
				xLog(hLog, XLL_PASS, "Number of SPs matches expected amount");
		}
	}
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating service providers with NULL GUIDs, NULL buffer, non-zero count, and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with NULL GUIDs, NULL buffer, non-zero count, and 0 flags");
	
	dwSPBufferSize = 0;
	// We expect to see at least the IP Service Provider.  We could technically
	// make sure there's room for the string, too, but that would have to be
	// localized.
	dwExpectedSPBufferSize = sizeof (DPN_SERVICE_PROVIDER_INFO);
	dwNumSPs = 12345;
	
	hr = pPeer->EnumServiceProviders(NULL, NULL, NULL, &dwSPBufferSize,
		&dwNumSPs, 0);
	if (hr != DPNERR_BUFFERTOOSMALL)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error BUFFERTOOSMALL!");
	else
	{
		xLog(hLog, XLL_INFO, "EnumSP returned BUFFERTOOSMALL successfully");

		if (dwSPBufferSize <= dwExpectedSPBufferSize)
			xLog(hLog, XLL_FAIL, "Buffer size smaller than expected (%u <= %u)!", dwSPBufferSize, dwExpectedSPBufferSize);
		else
		{
			xLog(hLog, XLL_INFO, "Buffer size wasn't smaller than expected");
			if(dwNumSPs < 1)
				xLog(hLog, XLL_FAIL, "Number of SPs not expected (%u < 1)!", dwNumSPs);
			else
				xLog(hLog, XLL_PASS, "Number of SPs matches expected amount");
		}
	}
		
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating service providers with NULL GUIDs, 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with NULL GUIDs, NULL buffer, non-zero count, and 0 flags");

	pSPInfo = NULL;
	pSPInfo = (PDPN_SERVICE_PROVIDER_INFO) LocalAlloc(LPTR, dwSPBufferSize + 64);
	if(!pSPInfo)
	{
		xLog(hLog, XLL_FAIL, "Couldn't allocate service provider buffer");
		fSuccess = FALSE;
		goto Exit;
	}
	memset(pSPInfo, 0xBA, dwSPBufferSize + 64);
	
	// Expect to get the same values again
	dwExpectedSPBufferSize = dwSPBufferSize;
	dwExpectedNumSPs = dwNumSPs;
	dwNumSPs = 0;
	
	hr = pPeer->EnumServiceProviders(NULL, NULL, pSPInfo, &dwSPBufferSize, &dwNumSPs, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "EnumSP didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "EnumSP returned DPN_OK successfully");

		if (dwSPBufferSize != dwExpectedSPBufferSize)
			xLog(hLog, XLL_FAIL, "Buffer size not expected (%u != %u)!", dwSPBufferSize, dwExpectedSPBufferSize);
		else
		{
			xLog(hLog, XLL_INFO, "Buffer size matches expected amount");
			if(dwNumSPs != dwExpectedNumSPs)
				xLog(hLog, XLL_FAIL, "Number of SPs not expected (%u != %u)!", dwNumSPs, dwExpectedNumSPs);
			else
			{
				xLog(hLog, XLL_INFO, "Number of SPs matches expected amount");
				if (*((PBYTE) pSPInfo) + dwSPBufferSize) != 0xBA)
					xLog(hLog, XLL_FAIL, "EnumSP wrote beyond end of buffer");
				else
					xLog(hLog, XLL_PASS, "EnumSP didn't write beyond end of buffer");
			}
		}
	}

	// TODO - Verify TCP/IP is in results
	
	LocalFree(pSPInfo);
	pSPInfo = NULL;
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating invalid SP GUID devices");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP with invalid SP GUID device");
	
	dwSPBufferSize = 0;
	dwNumSPs = 0;
	
	hr = pPeer->EnumServiceProviders(&DPTEST_GUID_UNKNOWN, NULL, NULL, &dwSPBufferSize, &dwNumSPs, 0);
	if (hr != DPNERR_DOESNOTEXIST)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error DOESNOTEXIST!");
	else
	{
		xLog(hLog, XLL_INFO, "EnumSP returned DOESNOTEXIST successfully");

		if (dwSPBufferSize != 0)
			xLog(hLog, XLL_FAIL, "Buffer size not expected (%u != 0)!", dwSPBufferSize);
		else
		{
			xLog(hLog, XLL_INFO, "Buffer size matches expected amount");
			if(dwNumSPs != 0)
				xLog(hLog, XLL_FAIL, "Number of SPs not expected (%u != 0)!", dwNumSPs);
			else
				xLog(hLog, XLL_PASS, "Number of SPs matches expected amount");
		}
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating TCP/IP devices with invalid app GUID");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP TCP/IP devices with invalid app GUID");
	
	dwSPBufferSize = 0;
	dwNumSPs = 0;
	
	hr = pPeer->EnumServiceProviders(&CLSID_DP8SP_TCPIP, &DPTEST_GUID_UNKNOWN,	NULL, &dwSPBufferSize, &dwNumSPs, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "EnumSP returned INVALIDPARAM successfully");

	// TODO - What do we really expect here?

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating TCP/IP devices with NULL GUIDs, NULL buffer, and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP TCP/IP devices with NULL GUIDs, NULL buffer, and 0 flags");
	
	dwSPBufferSize = 0;
	dwNumSPs = 0;
	
	hr = pPeer->EnumServiceProviders(&CLSID_DP8SP_TCPIP, NULL, NULL, &dwSPBufferSize, &dwNumSPs, 0);
	if (hr != DPNERR_BUFFERTOOSMALL)
		xLog(hLog, XLL_FAIL, "EnumSP didn't fail with expected error BUFFERTOOSMALL!");
	else
		xLog(hLog, XLL_PASS, "EnumSP returned BUFFERTOOSMALL successfully");
	
	// TODO - What size do we expect?

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Enumerating TCP/IP devices with NULL GUIDs, 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: EnumSP TCP/IP devices with NULL GUIDs, 0 flags");

	pSPInfo = NULL;
	pSPInfo = (PDPN_SERVICE_PROVIDER_INFO) LocalAlloc(LPTR, dwSPBufferSize + 64);
	if(!pSPInfo)
	{
		xLog(hLog, XLL_FAIL, "Couldn't allocate service provider buffer");
		fSuccess = FALSE;
		goto Exit;
	}
	memset(pSPInfo, 0xBA, dwSPBufferSize + 64);
	
	// Expect to get the same values again
	dwExpectedSPBufferSize = dwSPBufferSize;
	dwExpectedNumSPs = dwNumSPs;
	dwNumSPs = 0;

	hr = pPeer->EnumServiceProviders(&CLSID_DP8SP_TCPIP, NULL, pSPInfo, &dwSPBufferSize, &dwNumSPs, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "EnumSP didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "EnumSP returned DPN_OK successfully");

		if (dwSPBufferSize != dwExpectedSPBufferSize)
			xLog(hLog, XLL_FAIL, "Buffer size not expected (%u != %u)!", dwSPBufferSize, dwExpectedSPBufferSize);
		else
		{
			xLog(hLog, XLL_INFO, "Buffer size matches expected amount");
			if(dwNumSPs != dwExpectedNumSPs)
				xLog(hLog, XLL_FAIL, "Number of SPs not expected (%u != %u)!", dwNumSPs, dwExpectedNumSPs);
			else
			{
				xLog(hLog, XLL_INFO, "Number of SPs matches expected amount");
				if (*((PBYTE) pSPInfo) + dwSPBufferSize) != 0xBA)
					xLog(hLog, XLL_FAIL, "EnumSP wrote beyond end of buffer");
				else
					xLog(hLog, XLL_PASS, "EnumSP didn't write beyond end of buffer");
			}
		}
	}

	// TODO - Verify TCP/IP is in results
	
	LocalFree(pSPInfo);
	pSPInfo = NULL;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Close after EnumSP tests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Release after EnumSP tests");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeer = NULL;

	xEndVariation(hLog);	
	
Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// TODO - Need to release the voice interface as well

	return fSuccess;
}

//==================================================================================
// PeerParamsCancelTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.4 - Peer CancelAsyncOperation parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
BOOL PeerParamsCancelTests(HANDLE hLog)
{
	PDPN_SERVICE_PROVIDER_INFO	pSPInfo;
	DPN_APPLICATION_DESC dpAppDesc;
	PDIRECTPLAY8ADDRESS	pDP8AddressHost;
	PDIRECTPLAY8ADDRESS	pDP8AddressDevice;
	PEERCANCELCONTEXT context;
	PDIRECTPLAY8PEER pPeer;
	HRESULT	hr;
	HOSTENT *pHostent;
	LPVOID pVoice;
	WCHAR szLocalIPAddress[16];
	CHAR szLocalMacAddress[16];
	BOOL fSuccess = TRUE;

	ZeroMemory(&context, sizeof (PEERCANCELCONTEXT));
	
	// Creating DirectPlay8Peer object
	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");	
	
	// Creating DirectPlay8Address object
	hr = DirectPlay8AddressCreate(&pDP8AddressHost);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate returned S_OK successfully");		
	
	// Setting host DirectPlay8Address SP to TCP/IP
	hr = pDP8AddressHost->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	
	
	// Duplicating host DirectPlay8Address for a device address
	hr = pDP8AddressHost->Duplicate(&pDP8AddressDevice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "Duplicate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "Duplicate returned S_OK successfully");	
	
	// In order to run these tests, need to get the local address information
	// Start by getting the Mac address
	if(gethostname(szLocalMacAddress, sizeof(szLocalMacAddress)))
	{
		xLog(hLog, XLL_FAIL, "Couldn't retrieve local Mac address");
		fSuccess = FALSE;
		goto Exit;
	}

	// Then get the IP address
	pHostent = NULL;
	if((pHostent = gethostbyname(szLocalMacAddress)) == NULL)
	{
		xLog(hLog, XLL_FAIL, "Couldn't retrieve IP information for local host");
		fSuccess = FALSE;
		goto Exit;
	}

	// Extract the IP address from the hostent stucture
	mbstowcs(szLocalIPAddress, pHostent->h_addr, strlen(pHostent->h_addr));
	szLocalIPAddress[strlen(pHostent->h_addr)] = 0;

	// Setting remote address hostname to local machine
	hr = pDP8AddressHost->AddComponent(DPNA_KEY_HOSTNAME, szLocalIPAddress, (wcslen(szLocalIPAddress) + 1), DPNA_DATATYPE_STRING);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "Couldn't set remote address hostname to local machine!", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Finally, we're ready to run the tests.
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Cancelling async operation with NULL and 0 flags using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync with NULL and 0 flags using C++ macro");

	hr = IDirectPlay8Peer_CancelAsyncOperation(pPeer, NULL, 0);
	if (hr != DPNERR_INVALIDHANDLE)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't fail with expected error INVALIDHANDLE!");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned INVALIDHANDLE successfully");
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Cancelling async operation with NULL and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync with NULL and 0 flags using C++ macro");
	
	hr = pPeer->CancelAsyncOperation(NULL, 0);
	if (hr != DPNERR_INVALIDHANDLE)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't fail with expected error INVALIDHANDLE!");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned INVALIDHANDLE successfully");
	
	xEndVariation(hLog);

	// Next tests require peer object to be initialized
	// Initializing with context, 0 flags
	
	context.hLog = hLog;

	hr = pPeer->Initialize(&context, PeerCancelHandler, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Initializing with context, 0 flags failed!");
		fSuccess = FALSE;
		goto Exit;
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Starting async connect to self which would eventually complete with failure"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync starting first failing async connect to self");
	
	if(!(context.hConnectCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CANCEL;
	
	context.dpnhExpectedAsyncOp = (DPNHANDLE) 0x666;
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice, NULL, NULL, NULL, 0,NULL,
		context.hConnectCompleteEvent, &(context.dpnhExpectedAsyncOp), 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "Couldn't start async connect to self which would eventually complete with failure!");
	else
	{
		xLog(hLog, XLL_INFO, "Started async connect to self which would eventually complete with failure.");
		if (context.dpnhExpectedAsyncOp == (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
		else
		{
			xLog(hLog, XLL_INFO, "Async operation handle was modified");
			if (context.dpnhExpectedAsyncOp == NULL)
				xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
		}
	}
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Cancelling invalid handle with 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync invalid handle with 0 flags");
	
	hr = pPeer->CancelAsyncOperation(0xABABABAB, 0);
	if (hr != DPNERR_INVALIDHANDLE)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't fail with expected error INVALIDHANDLE!");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned INVALIDHANDLE successfully");

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Cancelling null connect handle with invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync null connect handle with invalid flags");
	
	hr = pPeer->CancelAsyncOperation(NULL, 0x666);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't fail with expected error INVALIDFLAGS!");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned INVALIDFLAGS successfully");

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Cancelling valid connect handle with CONNECT flag"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync valid connect handle with CONNECT flags");
	
	hr = pPeer->CancelAsyncOperation(context.dpnhExpectedAsyncOp, DPNCANCEL_CONNECT);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned INVALIDPARAM successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Cancelling valid connect handle with ALL_OPERATIONS flag"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync valid connect handle with ALL_OPERATIONS flags");
	
	hr = pPeer->CancelAsyncOperation(context.dpnhExpectedAsyncOp, DPNCANCEL_ALL_OPERATIONS);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned INVALIDPARAM successfully");
	
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Cancelling valid connect handle" 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync valid connect handle");
	
	// Allow the connect to complete.
	context.hrExpectedResult = DPNERR_USERCANCEL;
	context.fConnectCanComplete = TRUE;
	
	hr = pPeer->CancelAsyncOperation(context.dpnhExpectedAsyncOp, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for cancelled connect to complete"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync wait for first connection to complete");
	
	if(WaitForSingleObject(&context.hConnectCompleteEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Cancelled connection didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Cancelled connection callback signalled");

		if (! context.fCompleted)
			xLog(hLog, XLL_FAIL, "Cancelling connect didn't cause it to complete!");
		else
			xLog(hLog, XLL_PASS, "Cancelled connect reported completion!");
	}

	// Prevent any wierd connects from completing.
	context.fConnectCanComplete = FALSE;
	
	// Reset the context.
	context.dpnhExpectedAsyncOp = (DPNHANDLE) 0x666;
	context.fCompleted = FALSE;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Starting async connect to self which would eventually complete with failure"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync starting second failing async connect to self");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CANCEL;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice, NULL, NULL, NULL, 0, NULL, 
		context.hConnectCompleteEvent, &(context.dpnhExpectedAsyncOp), 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "Couldn't start async connect to self which would eventually complete with failure!");
	else
	{
		xLog(hLog, XLL_INFO, "Started async connect to self which would eventually complete with failure.");
		if (context.dpnhExpectedAsyncOp == (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
		else
		{
			xLog(hLog, XLL_INFO, "Async operation handle was modified");
			if (context.dpnhExpectedAsyncOp == NULL)
				xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
		}
	}
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Cancelling ENUMs"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync ENUMs");
	
	hr = pPeer->CancelAsyncOperation(NULL, DPNCANCEL_ENUM);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't succeed with expected return DPN_OK");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned DPN_OK successfully");
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Cancelling CONNECTs"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync CONNECTs");
	
	// Allow the connect to complete.
	context.hrExpectedResult = DPNERR_USERCANCEL;
	context.fConnectCanComplete = TRUE;
	
	hr = pPeer->CancelAsyncOperation(NULL, DPNCANCEL_CONNECT);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't succeed with expected return DPN_OK");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned DPN_OK successfully");
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for cancelled connect to complete"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync wait for first connection to complete");
	
	if(WaitForSingleObject(&context.hConnectCompleteEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Cancelled connection didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Cancelled connection callback signalled");

		if (! context.fCompleted)
			xLog(hLog, XLL_FAIL, "Cancelling connect didn't cause it to complete!");
		else
			xLog(hLog, XLL_PASS, "Cancelled connect reported completion!");
	}

	// Prevent any wierd connects from completing.
	context.fConnectCanComplete = FALSE;
	
	// Reset the context.
	context.dpnhExpectedAsyncOp = (DPNHANDLE) 0x666;
	context.fCompleted = FALSE;
	
	// Reset the context.
	CloseHandle(context.hConnectCompleteEvent);
	context.hConnectCompleteEvent = NULL;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CancelAsync close after CancelAsync tests");

	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "CancelAsync didn't succeed with expected return DPN_OK");
	else
		xLog(hLog, XLL_PASS, "CancelAsync returned DPN_OK successfully");
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Release after EnumSP tests");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeer = NULL;

	xEndVariation(hLog);	

Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// TODO - Need to release the voice interface as well

	return fSuccess;
}

//==================================================================================
// PeerParamsConnectTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.5 - Peer Connect parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
BOOL PeerParamsConnectTests(HANDLE hLog)
{
	DPN_APPLICATION_DESC dpAppDesc, dpAppDescCompare;
	PDIRECTPLAY8ADDRESS	pDP8AddressHost, pDP8AddressDevice;
	PEERCONNECTCONTEXT context;
	PDIRECTPLAY8PEER pPeer, pPeerHost;
	DPNHANDLE dpnhConnect = 0x666, dpnhDuplicate = 0x666;
	HOSTENT *pHostent;
	HRESULT	hr;
	LPVOID pVoice;
	WCHAR szLocalIPAddress[16];
	CHAR szLocalMacAddress[16];
	BOOL fSuccess = TRUE;

	ZeroMemory(&context, sizeof (PEERCONNECTCONTEXT));
	
	// Creating DirectPlay8Peer object
	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");	
	
	// Creating DirectPlay8Address object for host
	hr = DirectPlay8AddressCreate(&pDP8AddressHost);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (pDP8AddressHost) returned S_OK successfully");		
	

	// Creating DirectPlay8Address object for device
	hr = DirectPlay8AddressCreate(&pDP8AddressDevice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (pDP8AddressDevice) returned S_OK successfully");		

	// Finally, we're ready to run the tests.

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with all NULLs and 0 flags using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with all NULLs and 0 flags using C++ macro");
	
	hr = IDirectPlay8Peer_Connect(pPeer, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPOINTER!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
		xLog(hLog, XLL_PASS, "Connect returned INVALIDPOINTER successfully");
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with all NULLs and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with all NULLs and 0 flags");
	
	hr = pPeer->Connect(NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPOINTER!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
		xLog(hLog, XLL_PASS, "Connect returned INVALIDPOINTER successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with NULL app desc"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with NULL app desc");
	
	hr = pPeer->Connect(NULL, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_INVALIDPARAM)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPOINTER!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDPOINTER successfully");
		if (dpnhConnect != (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
		else
			xLog(hLog, XLL_PASS, "Async operation handle wasn't modified.");
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with app desc of 0 size"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with app desc of 0 size");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;

	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_INVALIDPARAM)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPARAM!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDPARAM successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with app desc of too-large size"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with app desc of too-large size");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC) + 1;
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;

	dpnhConnect = 0x666;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_INVALIDPARAM)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPARAM!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDPARAM successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with app desc using invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with app desc using invalid flags");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.dwFlags = 0x666;
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_INVALIDPARAM)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPARAM!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDPARAM successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);
		  
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with NULL host address"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with NULL host address");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;

	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Connect(&dpAppDesc, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_INVALIDHOSTADDRESS)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDHOSTADDRESS!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDHOSTADDRESS successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with empty host address, NULL device address"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with empty host address, NULL device address");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;

	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_INVALIDHOSTADDRESS)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDHOSTADDRESS!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDHOSTADDRESS successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);
	
#endif // BAD_PARAM_TESTS

	// Next tests require address object to be set for TCP/IP
	// Setting host DirectPlay8Address SP to TCP/IP
	hr = pDP8AddressHost->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with non-NULL security desc");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with non-NULL security desc");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, (PDPN_SECURITY_DESC) ((DWORD_PTR) 0xABABABAB),
		NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_INVALIDPOINTER)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPOINTER!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDPOINTER successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with non-NULL security credentials"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with non-NULL security credentials");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, (PDPN_SECURITY_CREDENTIALS) ((DWORD_PTR) 0xABABABAB),
		NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_INVALIDPOINTER)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPOINTER!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDPOINTER successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting asynchronously with NULL async op handle"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with NULL async op handle");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPARAM!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDPARAM successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified.");
	}

	xEndVariation(hLog);	
	
#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting synchronously with async op handle"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect synchronously with async op handle");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, DPNCONNECT_SYNC);
	if (hr != DPNERR_INVALIDPARAM)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDPARAM!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDPARAM successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with invalid flags");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;

	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0x666);
	if (hr != DPNERR_INVALIDFLAGS)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDFLAGS!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDFLAGS successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting without being initialized"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect without being initialized");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_UNINITIALIZED)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error UNINITIALIZED!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned UNINITIALIZED successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}
	
	xEndVariation(hLog);

	// Next tests require peer object to be initialized
	// Initializing with context, 0 flags
	
	context.hLog = hLog;

	hr = pPeer->Initialize(&context, PeerConnectHandler, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Initializing with context, 0 flags failed!");
		fSuccess = FALSE;
		goto Exit;
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting with SP-only host address, empty device address"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect with SP-only host address, empty device address");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_INVALIDDEVICEADDRESS)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error INVALIDDEVICEADDRESS!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();

	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned INVALIDDEVICEADDRESS successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}
	
	xEndVariation(hLog);	

	// In order to run these tests, need to get the local address information
	// Start by getting the Mac address
	if(gethostname(szLocalMacAddress, sizeof(szLocalMacAddress)))
	{
		xLog(hLog, XLL_FAIL, "Couldn't retrieve local Mac address");
		fSuccess = FALSE;
		goto Exit;
	}

	// Then get the IP address
	pHostent = NULL;
	if((pHostent = gethostbyname(szLocalMacAddress)) == NULL)
	{
		xLog(hLog, XLL_FAIL, "Couldn't retrieve IP information for local host");
		fSuccess = FALSE;
		goto Exit;
	}

	// Extract the IP address from the hostent stucture
	mbstowcs(szLocalIPAddress, pHostent->h_addr, strlen(pHostent->h_addr));
	szLocalIPAddress[strlen(pHostent->h_addr)] = 0;

	// Setting remote address hostname to local machine
	hr = pDP8AddressHost->AddComponent(DPNA_KEY_HOSTNAME, szLocalIPAddress, (wcslen(szLocalIPAddress) + 1), DPNA_DATATYPE_STRING);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "Couldn't set remote address hostname to local machine!", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Starting async connect to self which would eventually complete with failure"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect failing async to self");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = 0x666;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't succeed with expected return DPNSUCCESS_PENDING!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned DPNSUCCESS_PENDING successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect == (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle wasn't modified!");
			else
			{
				xLog(hLog, XLL_INFO, "Async operation handle was modified");
				if (dpnhConnect == NULL)
					xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
				else
					xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
			}
		}
	}

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Trying another async connect with one still pending"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect already pending");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhDuplicate = 0x666;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhDuplicate, 0);
	if (hr != DPNERR_CONNECTING)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error CONNECTING!");

		// This peer interface is tainted now, release it and get a new one before moving on to the next test
		pPeer ? pPeer->Release() : 0;
		pPeer = NULL;

		// TODO - release the voice pointer

		xEndVariation(hLog);

		// We're pretty much screwed at this point.  Original connect call might have been mucked up by the
		// second connect call that returned success when it shouldn't have.
		fSuccess = FALSE;
		goto Exit;
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned CONNECTING successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhDuplicate != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	// This part is based on the assumption that we'll be able to run the previous test (Connect already pending)
	// before the earlier test (Connect failing async to self) completes asynchronously

	if(!(context.hConnectCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async connect to fail (this may take some time)"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect wait for async connect to fail");

	context.hrExpectedResult = DPNERR_NORESPONSE;
	context.fConnectCanComplete = TRUE;
	
	if(WaitForSingleObject(&context.hConnectCompleteEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Connection didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Connection callback signalled");
		if (! context.fCompleted)
			xLog(hLog, XLL_FAIL, "Didn't receive a connection completion indication!");
		else
		{
			xLog(hLog, XLL_INFO, "Received a connection completion indication.");
			if (context.dpnhCompletedAsyncOp != dpnhConnect)
				xLog(hLog, XLL_FAIL, "Connection completed with different async op handle (%x != %x)",
				context.dpnhCompletedAsyncOp, dpnhConnect);
			else
				xLog(hLog, XLL_PASS, "Connection completed with correct async op handle");
		}
	}

	// Prevent any wierd connects from completing.
	context.fConnectCanComplete = FALSE;

	// Reset connection context
	CloseHandle(context.hConnectCompleteEvent);
	context.hConnectCompleteEvent = NULL;
	context.fCompleted = FALSE;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Synchronously connecting to self without a host (this may take some time)"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect to self synchornously without a host");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	// Even though it's synchronous, we'll still get a completion.  The async op
	// handle will be NULL, though.
	context.hrExpectedResult = DPNERR_NORESPONSE;
	context.fConnectCanComplete = TRUE;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, DPNCONNECT_SYNC);
	
	// Prevent connects from completing.
	context.fConnectCanComplete = FALSE;
	
	if (hr != DPNERR_NORESPONSE)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error NORESPONSE!");
		
		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned NORESPONSE successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (! context.fCompleted)
				xLog(hLog, XLL_FAIL, "Didn't receive a connection completion indication!");
			else
			{
				xLog(hLog, XLL_INFO, "Cancelled connect reported completion!");
				if (context.dpnhCompletedAsyncOp != NULL)
					xLog(hLog, XLL_FAIL, "Connection completed with an async op handle (%x)", context.dpnhCompletedAsyncOp);
				else
					xLog(hLog, XLL_PASS, "Connection completed without correct async op handle");
			}
		}
	}
	
	xEndVariation(hLog);	

	// Reset the context.
	context.fCompleted = FALSE;

	// Setting host DirectPlay8Address SP to TCP/IP
	hr = pDP8AddressDevice->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting session"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect hosting session");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	hr = pPeer->Host(&dpAppDesc, &pDP8AddressDevice, 1, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Host didn't succeed with expected return DPN_OK!");
		// This peer interface is tainted now, release it and get a new one before moving on to the next test
		pPeer ? pPeer->Release() : 0;
		pPeer = NULL;

		// TODO - release the voice pointer

		xEndVariation(hLog);

		// We're pretty much screwed at this point.  Since host doesn't seem to be working, our remaining tests
		// won't be valid.
		fSuccess = FALSE;
		goto Exit;
	}
	else
	{
		xLog(hLog, XLL_INFO, "Host returned DPN_OK successfully");
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified.");
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting when already hosting"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect when already hosting");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = (DPNHANDLE) 0x666;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_ALREADYCONNECTED)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error ALREADYCONNECTED!");
		
		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned ALREADYCONNECTED successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect closing after connection attempt");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Re-initializing peer object with context, 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect re-init with context, 0 flags");
	
	context.hLog = hLog;

	hr = pPeer->Initialize(&context, PeerConnectHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");
	
	xEndVariation(hLog);	
	
	pDP8AddressHost->Release();
	pDP8AddressHost = NULL;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating new peer host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect create peer host object");

	hr = PeerCreateHost(hLog, PeerNewPlayerHandler, NULL, &dpAppDesc, &pDP8PeerHost, NULL, &pDP8AddressHost);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "PeerCreateHost didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "PeerCreateHost returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// Create an event to signal when the connection is complete
	if(!(context.hConnectCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Starting async connect to local peer"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect async to local peer");
	
	context.hrExpectedResult = DPN_OK;
	context.fConnectCanComplete = TRUE;
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = (DPNHANDLE) 0x666;

	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't succeed with expected return DPNSUCCESS_PENDING!");

		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned DPNSUCCESS_PENDING successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect == (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle wasn't modified!");
			else
			{
				xLog(hLog, XLL_INFO, "Async operation handle was modified");
				if (dpnhConnect == NULL)
					xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
				else
					xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
			}
		}
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//"Waiting for async connect to succeed"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect wait for local peer async connect to succeed");

	if(WaitForSingleObject(&context.hConnectCompleteEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Connection didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Connection callback signalled");
		if (! context.fCompleted)
			xLog(hLog, XLL_FAIL, "Didn't receive a connection completion indication!");
		else
		{
			xLog(hLog, XLL_INFO, "Received a connection completion indication.");
			if (context.dpnhCompletedAsyncOp != dpnhConnect)
				xLog(hLog, XLL_FAIL, "Connection completed with different async op handle (%x != %x)",
				context.dpnhCompletedAsyncOp, dpnhConnect);
			else
				xLog(hLog, XLL_PASS, "Connection completed with correct async op handle");
		}
	}

	// Prevent any wierd connects from completing.
	context.fConnectCanComplete = FALSE;

	// Reset the context.
	CloseHandle(context.hConnectCompleteEvent);
	context.hConnectCompleteEvent = NULL;
	context.fCompleted = FALSE;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting when already connected"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect when already successfully connected");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_CONNECT;
	
	dpnhConnect = (DPNHANDLE) 0x666;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, &dpnhConnect, 0);
	if (hr != DPNERR_ALREADYCONNECTED)
	{
		xLog(hLog, XLL_FAIL, "Connect didn't fail with expected error ALREADYCONNECTED!");
		
		// We'll try to continue later tests by closing any connection we somehow managed to create here.
		pPeer->Close();
	}
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned ALREADYCONNECTED successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (dpnhConnect != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified!");
		}
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect closing after successful connection attempt");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Re-initializing peer object with context, 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect second re-init with context, 0 flags");
	
	context.hLog = hLog;

	hr = pPeer->Initialize(&context, PeerConnectHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");
	
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting synchronously"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect synchronously to local peer host");
	
	// Even though it's synchronous, we'll still get a completion.  The async op
	// handle will be NULL, though.
	context.hrExpectedResult = DPN_OK;
	context.fConnectCanComplete = TRUE;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8AddressHost, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, DPNCONNECT_SYNC);

	// Prevent any weird connects from completing.
	context.fConnectCanComplete = FALSE;
	
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Connect didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified.");
			if (! context.fCompleted)
				xLog(hLog, XLL_FAIL, "Didn't receive a connection completion indication!");
			else
			{
				xLog(hLog, XLL_INFO, "Received a connection completion indication.");
				if (context.dpnhCompletedAsyncOp != NULL)
					xLog(hLog, XLL_FAIL, "Connection completed with an async op handle (%x != %x)",
					context.dpnhCompletedAsyncOp, dpnhConnect);
				else
					xLog(hLog, XLL_PASS, "Connection completed without a async op handle");
			}
		}
	}
	
	// Reset the context.
	context.fCompleted = FALSE;
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect closing after successful synchronous connection attempt");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect closing host after multiple connection attempts");
	
	hr = pPeerHost->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing hosting DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect release host after connection tests");
	
	hr = pPeerHost->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeer = NULL;

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Connect release peer after connection tests");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeer = NULL;

	xEndVariation(hLog);	
	
Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// Release the peer interface if we haven't already
	if(pPeerHost)
	{
		pPeerHost->Release();
		pPeerHost = NULL;
	}

	// Release the peer interface if we haven't already
	if(pDP8AddressHost)
	{
		pDP8AddressHost->Release();
		pDP8AddressHost = NULL;
	}

	// Release the address if we haven't already
	if(pDP8AddressDevice)
	{
		pDP8AddressDevice->Release();
		pDP8AddressDevice = NULL;
	}

	// TODO - Need to release the voice interface as well

	return fSuccess;
}

//==================================================================================
// PeerParamsSendToTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.6 - Peer SendTo parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
BOOL PeerParamsSendToTests(HANDLE hLog)
{
	DPN_APPLICATION_DESC dpAppDesc, dpAppDescCompare;
	PDIRECTPLAY8ADDRESS	pDP8Address;
	PEERSENDTOCONTEXT context;
	PDIRECTPLAY8PEER pPeer;
	DPN_BUFFER_DESC dpnbd, dpnbdCompare;
	DPNHANDLE dpnhSendTo = (DPNHANDLE) 0x666;
	HRESULT	hr;
	LPVOID pVoice;
	BOOL fSuccess = TRUE;
	
	ZeroMemory(&dpnbd, sizeof (DPN_BUFFER_DESC));
	ZeroMemory(&context, sizeof (PARMVPSENDTOCONTEXT));
	
	// Allocate a 1 meg buffer.
	dpnbd.pBufferData = (PBYTE) LocalAlloc(LPTR, 1 * 1024 * 1024);
	if(!dpnbd.pBufferData)
	{
		xLog(hLog, XLL_FAIL, "Couldn't allocate send-to buffer");
		fSuccess = FALSE;
		goto Exit;
	}
	
	// Initialize buffer size to 1 byte
	dpnbd.dwBufferSize = 1;
	
	// Creating DirectPlay8Peer object
	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");	
	
	// Creating DirectPlay8Address object for host
	hr = DirectPlay8AddressCreate(&pDP8Address);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (pDP8Address) returned S_OK successfully");		

	// Next tests require address object to be set for TCP/IP
	// Setting host DirectPlay8Address SP to TCP/IP
	hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	


#ifdef BAD_PARAM_TESTS
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//"Sending to 0 with all NULLs and 0s using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo 0 with all NULLs and 0s using C++ macro");
	
	hr = IDirectPlay8Peer_SendTo(pPeer, 0, NULL, 0, 0, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "SendTo returned INVALIDPARAM successfully");
	
	xEndVariation(hLog);	
	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//"Sending to 0 with all NULLs and 0s"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo 0 with all NULLs and 0s");
	
	hr = pPeer->SendTo(0, NULL, 0, 0, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "SendTo returned INVALIDPARAM successfully");
	
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending asynchronously to AllPlayersGroup with NULL buffer desc"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo AllPlayersGroup with NULL buffer desc");
	
	dpnhSendTo = (DPNHANDLE) 0x666;

	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, NULL, 1, 0, NULL, &dpnhSendTo, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDPOINTER!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned INVALIDPOINTER successfully");

		if (dpnhSendTo != (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
		else
			xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
	}
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending asynchronously to AllPlayersGroup with 0 buffer desc count"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo AllPlayersGroup with 0 buffer desc count");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;

	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 0, 0, NULL, &dpnhSendTo, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned INVALIDPARAM successfully");

		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
		}
	}

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending asynchronously to AllPlayersGroup with NULL async handle"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo AllPlayersGroup with NULL async handle");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	
	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned INVALIDPARAM successfully");

		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Buffer desc structure wasn't modified");
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending synchronously to AllPlayersGroup with async handle"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo synchronously to AllPlayersGroup with async handle");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;

	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, DPNSEND_SYNC);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned INVALIDPARAM successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
		}
	}

	xEndVariation(hLog);	

#endif // BAD_PARAM_TESTS	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending to AllPlayersGroup with invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo AllPlayersGroup with invalid flags");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;

	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, 0x666000);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDFLAGS!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned INVALIDFLAGS successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
		}
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending to AllPlayersGroup with NOCOPY and NOCOMPLETE"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo AllPlayersGroup with NOCOPY and NOCOMPLETE");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;
	
	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, (DPNSEND_NOCOPY | DPNSEND_NOCOMPLETE));
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDFLAGS!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned INVALIDFLAGS successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
		}
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending to AllPlayersGroup with only COMPLETEONPROCESS"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo AllPlayersGroup with only COMPLETEONPROCESS");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;
		
	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, DPNSEND_COMPLETEONPROCESS);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDFLAGS!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned INVALIDFLAGS successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
		}
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending to AllPlayersGroup with PRIORITY_LOW and PRIORITY_HIGH"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo AllPlayersGroup with PRIORITY_LOW and PRIORITY_HIGH");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;
	
	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, (DPNSEND_PRIORITY_LOW | DPNSEND_PRIORITY_HIGH));
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDFLAGS!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned INVALIDFLAGS successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
		}
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending to AllPlayersGroup with NOCOPY when uninitialized"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo AllPlayersGroup with NOCOPY when uninitialized");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;
	
	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, DPNSEND_NOCOPY);
	if (hr != DPNERR_UNINITIALIZED)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error UNINITIALIZED!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned UNINITIALIZED successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
		}
	}

	xEndVariation(hLog);	


	// Next tests require peer object to be initialized
	// Initializing with context, 0 flags

	context.hLog = hLog;

	hr = pPeer->Initialize(&context, PeerSendToHandler, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Initializing with context, 0 flags failed!");
		fSuccess = FALSE;
		goto Exit;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending to AllPlayersGroup without a connection"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo AllPlayersGroup without a connection");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;

	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, 0);
	if (hr != DPNERR_NOCONNECTION)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error NOCONNECTION!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned NOCONNECTION successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
		}
	}

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting session"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo start hosting session");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_SENDTO;

	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, &pDP8Address, 1, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Host didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending to invalid player ID" 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo invalid player ID");

	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;
	
	hr = pPeer->SendTo((DPNID) 0x666, &dpnbd, 1, 0, NULL, &dpnhSendTo, 0);
	if (hr != DPNERR_INVALIDPLAYER)
		xLog(hLog, XLL_FAIL, "SendTo didn't fail with expected error INVALIDPLAYER!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned INVALIDPLAYER successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
			else
				xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
		}
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending 1 byte to AllPlayersGroup"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo 1 byte to AllPlayersGroup");
	
	// Create an event to signal when a receive comes in
	if(!(context.hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expecting receives to be of 1 byte
	context.dwExpectedReceiveSize = 1;
	context.fCanReceive = TRUE;

	// Create an event to signal when a send completes
	if(!(context.hSendCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expecting sends to complete with DPN_OK
	context.hrExpectedResult = DPN_OK;
	context.fSendCanComplete = TRUE;

	dpnbd.dwBufferSize = 1;
	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;

	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "SendTo didn't succeed with expected return DPNSUCCESS_PENDING!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned DPNSUCCESS_PENDING successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo == (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle wasn't modified!");
			else
			{
				xLog(hLog, XLL_INFO, "Async operation handle was modified");

				if (dpnhSendTo == NULL)
					xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
				else
					xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
			}
		}
	}
	
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async send to succeed"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo wait for first send to complete");
	
	if(WaitForSingleObject(&context.hSendCompleteEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Send didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Send callback signalled");

		if (! context.fSendCompleted)
			xLog(hLog, XLL_FAIL, "Didn't receive send completion indication!");
		else
		{
			xLog(hLog, XLL_INFO, "Received send completion indication");
			if (context.dpnhCompletedAsyncOp != dpnhSendTo)
				xLog(hLog, XLL_FAIL, "Send completed with different async op handle (%x != %x)", 
				context.dpnhCompletedAsyncOp, dpnhConnect);
			else
				xLog(hLog, XLL_PASS, "Send completed with correct async op handle");
		}
	}

	// Prevent any weird sends from completing.
	context.fSendCanComplete = FALSE;
	
	// Reset the context.
	CloseHandle(context.hSendCompleteEvent);
	context.hSendCompleteEvent = NULL;
	context.dpnhCompletedAsyncOp = NULL;
	context.fSendCompleted = FALSE;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for the receive to be indicated"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo wait for first receive");

	if(WaitForSingleObject(&context.hReceiveEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Receive didn't occur within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Recieve callback signalled");

		if (! context.fReceived)
			xLog(hLog, XLL_FAIL, "Didn't get receive indication!");
		else
			xLog(hLog, XLL_PASS, "Got receive indication");
	}

	// Prevent any weird receives from coming in.
	context.fCanReceive = FALSE;
	
	// Reset the context.
	CloseHandle(context.hReceiveEvent);
	context.hReceiveEvent = NULL;
	context.fReceived = FALSE;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending 128 bytes to self"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo 128 bytes to self");
	
	// Create an event to signal when a receive comes in
	if(!(context.hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expecting receives to be of 128 byte
	context.dwExpectedReceiveSize = 128;
	context.fCanReceive = TRUE;

	// Create an event to signal when a send completes
	if(!(context.hSendCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expecting sends to complete with DPN_OK
	context.hrExpectedResult = DPN_OK;
	context.fSendCanComplete = TRUE;


	dpnbd.dwBufferSize = context.dwExpectedReceiveSize;
	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;
	
	// TODO - does 0x00200002 really always map to the local player?

	hr = pPeer->SendTo((DPNID) 0x00200002, &dpnbd, 1, 0, NULL, &dpnhSendTo, 0);
	
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "SendTo didn't succeed with expected return DPNSUCCESS_PENDING!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned DPNSUCCESS_PENDING successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo == (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle wasn't modified!");
			else
			{
				xLog(hLog, XLL_INFO, "Async operation handle was modified");

				if (dpnhSendTo == NULL)
					xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
				else
					xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
			}
		}
	}
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async send to succeed"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo wait for second send to complete");
	
	if(WaitForSingleObject(&context.hSendCompleteEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Send didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Send callback signalled");

		if (! context.fSendCompleted)
			xLog(hLog, XLL_FAIL, "Didn't receive send completion indication!");
		else
		{
			xLog(hLog, XLL_INFO, "Received send completion indication");
			if (context.dpnhCompletedAsyncOp != dpnhSendTo)
				xLog(hLog, XLL_FAIL, "Send completed with different async op handle (%x != %x)", 
				context.dpnhCompletedAsyncOp, dpnhConnect);
			else
				xLog(hLog, XLL_PASS, "Send completed with correct async op handle");
		}
	}

	// Prevent any weird sends from completing.
	context.fSendCanComplete = FALSE;
	
	// Reset the context.
	CloseHandle(context.hSendCompleteEvent);
	context.hSendCompleteEvent = NULL;
	context.dpnhCompletedAsyncOp = NULL;
	context.fSendCompleted = FALSE;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for the receive to be indicated"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo wait for second receive");

	if(WaitForSingleObject(&context.hReceiveEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Receive didn't occur within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Recieve callback signalled");

		if (! context.fReceived)
			xLog(hLog, XLL_FAIL, "Didn't get receive indication!");
		else
			xLog(hLog, XLL_PASS, "Got receive indication");
	}

	// Prevent any weird receives from coming in.
	context.fCanReceive = FALSE;
	
	// Reset the context.
	CloseHandle(context.hReceiveEvent);
	context.hReceiveEvent = NULL;
	context.fReceived = FALSE;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending 1MB NOCOPY to AllPlayersGroup"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo 1MB NOCOPY to AllPlayersGroup");
	
	// Create an event to signal when a receive comes in
	if(!(context.hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expecting receives to be of 128 byte
	context.dwExpectedReceiveSize = 1 * 1024 * 1024;
	context.fCanReceive = TRUE;

	// Create an event to signal when a send completes
	if(!(context.hSendCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expecting sends to complete with DPN_OK
	context.hrExpectedResult = DPN_OK;
	context.fSendCanComplete = TRUE;


	dpnbd.dwBufferSize = context.dwExpectedReceiveSize;
	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;

	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, DPNSEND_NOCOPY);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "SendTo didn't succeed with expected return DPNSUCCESS_PENDING!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned DPNSUCCESS_PENDING successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo == (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle wasn't modified!");
			else
			{
				xLog(hLog, XLL_INFO, "Async operation handle was modified");

				if (dpnhSendTo == NULL)
					xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
				else
					xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
			}
		}
	}
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async send to succeed"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo wait for third send to complete");
	
	if(WaitForSingleObject(&context.hSendCompleteEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Send didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Send callback signalled");

		if (! context.fSendCompleted)
			xLog(hLog, XLL_FAIL, "Didn't receive send completion indication!");
		else
		{
			xLog(hLog, XLL_INFO, "Received send completion indication");
			if (context.dpnhCompletedAsyncOp != dpnhSendTo)
				xLog(hLog, XLL_FAIL, "Send completed with different async op handle (%x != %x)", 
				context.dpnhCompletedAsyncOp, dpnhConnect);
			else
				xLog(hLog, XLL_PASS, "Send completed with correct async op handle");
		}
	}

	// Prevent any weird sends from completing.
	context.fSendCanComplete = FALSE;
	
	// Reset the context.
	CloseHandle(context.hSendCompleteEvent);
	context.hSendCompleteEvent = NULL;
	context.dpnhCompletedAsyncOp = NULL;
	context.fSendCompleted = FALSE;

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for the receive to be indicated"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo wait for third receive");

	if(WaitForSingleObject(&context.hReceiveEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Receive didn't occur within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Recieve callback signalled");

		if (! context.fReceived)
			xLog(hLog, XLL_FAIL, "Didn't get receive indication!");
		else
			xLog(hLog, XLL_PASS, "Got receive indication");
	}

	// Prevent any weird receives from coming in.
	context.fCanReceive = FALSE;
	
	// Reset the context.
	CloseHandle(context.hReceiveEvent);
	context.hReceiveEvent = NULL;
	context.fReceived = FALSE;
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending 1MB to self synchronously"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo 1MB to self synchronously");
	
	// Create an event to signal when a receive comes in
	if(!(context.hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expecting receives to be of 128 byte
	context.dwExpectedReceiveSize = 1 * 1024 * 1024;
	context.fCanReceive = TRUE;

	dpnbd.dwBufferSize = context.dwExpectedReceiveSize;
	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;

	// TODO - does 0x00200002 really always map to the local player?
	
	hr = pPeer->SendTo((DPNID) 0x00200002, &dpnbd, 1, 0, NULL, NULL, DPNSEND_SYNC);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "SendTo didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned DPN_OK successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Buffer desc structure wasn't modified");
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for the receive to be indicated"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo wait for fourth receive");

	if(WaitForSingleObject(&context.hReceiveEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Receive didn't occur within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Recieve callback signalled");

		if (! context.fReceived)
			xLog(hLog, XLL_FAIL, "Didn't get receive indication!");
		else
			xLog(hLog, XLL_PASS, "Got receive indication");
	}

	// Prevent any weird receives from coming in.
	context.fCanReceive = FALSE;
	
	// Reset the context.
	CloseHandle(context.hReceiveEvent);
	context.hReceiveEvent = NULL;
	context.fReceived = FALSE;
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Sending 10MB to AllPlayersGroup"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo 1MB to AllPlayersGroup");
	
	// Create an event to signal when a receive comes in
	if(!(context.hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expecting receives to be of 128 byte
	context.dwExpectedReceiveSize = 1 * 1024 * 1024;
	context.fCanReceive = TRUE;

	// Create an event to signal when a send completes
	if(!(context.hSendCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expecting sends to complete with DPN_OK
	context.hrExpectedResult = DPN_OK;
	context.fSendCanComplete = TRUE;


	dpnbd.dwBufferSize = context.dwExpectedReceiveSize;
	CopyMemory(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC));
	dpnhSendTo = (DPNHANDLE) 0x666;

	hr = pPeer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpnbd, 1, 0, NULL, &dpnhSendTo, 0);
	
	// Free the buffer right now.  This will hopefully catch DPlay trying to
	// use our buffer even though we didn't specify NOCOPY.
	// NOTE: We're not NULL-ing it right after the free!  Be sure to so before
	// bailing to the cleanup code!
	LocalFree(dpnbd.pBufferData);
	
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "SendTo didn't succeed with expected return DPNSUCCESS_PENDING!");
	else
	{
		xLog(hLog, XLL_INFO, "SendTo returned DPNSUCCESS_PENDING successfully");
		
		if (memcmp(&dpnbdCompare, &dpnbd, sizeof (DPN_BUFFER_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Buffer desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Buffer desc structure wasn't modified");

			if (dpnhSendTo == (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle wasn't modified!");
			else
			{
				xLog(hLog, XLL_INFO, "Async operation handle was modified");

				if (dpnhSendTo == NULL)
					xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
				else
					xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
			}
		}
	}
	
	// Okay, now we can NULL out the pointer since we've done the compare.
	dpnbd.pBufferData = NULL;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async send to succeed"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo wait for fifth send to complete");
	
	if(WaitForSingleObject(&context.hSendCompleteEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Send didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Send callback signalled");

		if (! context.fSendCompleted)
			xLog(hLog, XLL_FAIL, "Didn't receive send completion indication!");
		else
		{
			xLog(hLog, XLL_INFO, "Received send completion indication");
			if (context.dpnhCompletedAsyncOp != dpnhSendTo)
				xLog(hLog, XLL_FAIL, "Send completed with different async op handle (%x != %x)", 
				context.dpnhCompletedAsyncOp, dpnhConnect);
			else
				xLog(hLog, XLL_PASS, "Send completed with correct async op handle");
		}
	}

	// Prevent any weird sends from completing.
	context.fSendCanComplete = FALSE;
	
	// Reset the context.
	CloseHandle(context.hSendCompleteEvent);
	context.hSendCompleteEvent = NULL;
	context.dpnhCompletedAsyncOp = NULL;
	context.fSendCompleted = FALSE;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for the receive to be indicated"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo wait for fifth receive");

	if(WaitForSingleObject(&context.hReceiveEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Receive didn't occur within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Recieve callback signalled");

		if (! context.fReceived)
			xLog(hLog, XLL_FAIL, "Didn't get receive indication!");
		else
			xLog(hLog, XLL_PASS, "Got receive indication");
	}

	// Prevent any weird receives from coming in.
	context.fCanReceive = FALSE;
	
	// Reset the context.
	CloseHandle(context.hReceiveEvent);
	context.hReceiveEvent = NULL;
	context.fReceived = FALSE;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo closing peer after multiple sends");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SendTo release peer after send tests");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeer = NULL;

	xEndVariation(hLog);	

Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// Release the address if we haven't already
	if(pDP8Address)
	{
		pDP8AddressHost->Release();
		pDP8AddressHost = NULL;
	}

	if(dpnbd.pBufferData)
	{
		LocalFree(dpnbd.pBufferData);
		dpnbd.pBufferData = NULL;
	}

	// TODO - Need to release the voice interface as well

	return fSuccess;
}

//==================================================================================
// PeerParamsGetSendQInfoTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.7 - Peer GetSendQueueInfo parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
HRESULT PeerParamsGetSendQInfoTests(HANDLE hLog)
{
	PEERGETSENDQINFOCONTEXT	hostcontext, nonhostcontext;
	DPN_APPLICATION_DESC dpAppDesc, dpAppDescCompare;
	PDIRECTPLAY8ADDRESS	pDP8Address;
	PDIRECTPLAY8PEER pPeer, pPeerHost;
	HRESULT	hr;
	LPVOID pVoice;
	DWORD dwNumMsgs = 666, dwNumBytes = 666;
	BOOL fSuccess = TRUE;

	ZeroMemory(&hostcontext, sizeof (PARMVPGETSENDQINFOCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPGETSENDQINFOCONTEXT));

	// Creating DirectPlay8Peer object
	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");	
	
	// Creating DirectPlay8Address object for host
	hr = DirectPlay8AddressCreate(&pDP8Address);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (pDP8Address) returned S_OK successfully");		

	// Next tests require address object to be set for TCP/IP
	// Setting host DirectPlay8Address SP to TCP/IP
	hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//  "Getting send queue info with all NULLs and 0s using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo with all NULLs and 0s using C++ macro");

	hr = IDirectPlay8Peer_GetSendQueueInfo(pPeer, 0, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "GetSendQInfo returned INVALIDPARAM successfully");

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info with all NULLs and 0s"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo with all NULLs and 0s");
	
	hr = pPeer->GetSendQueueInfo(0, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "GetSendQInfo returned INVALIDPARAM successfully");

	xEndVariation(hLog);
	
#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info with invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo with invalid flags");
	
	dwNumMsgs = 666;
	dwNumBytes = 666;

	hr = pPeer->GetSendQueueInfo(0, &dwNumMsgs, &dwNumBytes, 0x666);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't fail with expected error INVALIDFLAGS!");
	else
	{
		xLog(hLog, XLL_INFO, "GetSendQInfo returned INVALIDFLAGS successfully");

		if(dwNumMsgs != 666)
			xLog(hLog, XLL_FAIL, "Num msgs was changed (%u != 666)", dwNumMsgs);
		else
		{
			xLog(hLog, XLL_INFO, "Num msgs wasn't changed");

			if(dwNumBytes != 666)
				xLog(hLog, XLL_FAIL, "Num bytes was changed (%u != 666)", dwNumBytes);
			else
				xLog(hLog, XLL_PASS, "Num bytes wasn't changed");
		}
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info before initialization"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo before initialization");
	
	dwNumMsgs = 666;
	dwNumBytes = 666;
	
	hr = pPeer->GetSendQueueInfo(0, &dwNumMsgs, &dwNumBytes, 0);
	if (hr != DPNERR_UNINITIALIZED)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't fail with expected error UNINITIALIZED!");
	else
	{
		xLog(hLog, XLL_INFO, "GetSendQInfo returned UNINITIALIZED successfully");

		if(dwNumMsgs != 666)
			xLog(hLog, XLL_FAIL, "Num msgs was changed (%u != 666)", dwNumMsgs);
		else
		{
			xLog(hLog, XLL_INFO, "Num msgs wasn't changed");

			if(dwNumBytes != 666)
				xLog(hLog, XLL_FAIL, "Num bytes was changed (%u != 666)", dwNumBytes);
			else
				xLog(hLog, XLL_PASS, "Num bytes wasn't changed");
		}
	}

	xEndVariation(hLog);

	// Next tests require peer object to be initialized
	// Initializing with context, 0 flags

	hostcontext.hLog = hLog;

	hr = pPeer->Initialize(&hostcontext, PeerGetSendQInfoHandler, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Initializing with context, 0 flags failed!");
		fSuccess = FALSE;
		goto Exit;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info without a connection"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo before initialization");
	
	dwNumMsgs = 666;
	dwNumBytes = 666;
		
	hr = pPeer->GetSendQueueInfo(0, &dwNumMsgs, &dwNumBytes, 0);
	if (hr != DPNERR_NOCONNECTION)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't fail with expected error NOCONNECTION!");
	else
	{
		xLog(hLog, XLL_INFO, "GetSendQInfo returned NOCONNECTION successfully");

		if(dwNumMsgs != 666)
			xLog(hLog, XLL_FAIL, "Num msgs was changed (%u != 666)", dwNumMsgs);
		else
		{
			xLog(hLog, XLL_INFO, "Num msgs wasn't changed");

			if(dwNumBytes != 666)
				xLog(hLog, XLL_FAIL, "Num bytes was changed (%u != 666)", dwNumBytes);
			else
				xLog(hLog, XLL_PASS, "Num bytes wasn't changed");
		}
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting session"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo start hosting session");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_SENDTO;

	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, &pDP8Address, 1, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Host didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info for 0"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo for player 0");
	
	dwNumMsgs = 666;
	dwNumBytes = 666;
	
	hr = pPeer->GetSendQueueInfo(0, &dwNumMsgs, &dwNumBytes, 0);
	if (hr != DPNERR_INVALIDPLAYER)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't fail with expected error INVALIDPLAYER!");
	else
	{
		xLog(hLog, XLL_INFO, "GetSendQInfo returned INVALIDPLAYER successfully");

		if(dwNumMsgs != 666)
			xLog(hLog, XLL_FAIL, "Num msgs was changed (%u != 666)", dwNumMsgs);
		else
		{
			xLog(hLog, XLL_INFO, "Num msgs wasn't changed");

			if(dwNumBytes != 666)
				xLog(hLog, XLL_FAIL, "Num bytes was changed (%u != 666)", dwNumBytes);
			else
				xLog(hLog, XLL_PASS, "Num bytes wasn't changed");
		}
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info for invalid player ID"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo for invalid player");
	
	dwNumMsgs = 666;
	dwNumBytes = 666;

	hr = pPeer->GetSendQueueInfo((DPNID) 0x666, &dwNumMsgs, &dwNumBytes, 0);
	if (hr != DPNERR_INVALIDPLAYER)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't fail with expected error INVALIDPLAYER!");
	else
	{
		xLog(hLog, XLL_INFO, "GetSendQInfo returned INVALIDPLAYER successfully");

		if(dwNumMsgs != 666)
			xLog(hLog, XLL_FAIL, "Num msgs was changed (%u != 666)", dwNumMsgs);
		else
		{
			xLog(hLog, XLL_INFO, "Num msgs wasn't changed");

			if(dwNumBytes != 666)
				xLog(hLog, XLL_FAIL, "Num bytes was changed (%u != 666)", dwNumBytes);
			else
				xLog(hLog, XLL_PASS, "Num bytes wasn't changed");
		}
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info for local player"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo for local player");
	
	dwNumMsgs = 666;
	dwNumBytes = 666;

	// TODO - does 0x00200002 really always map to the local player?

	hr = pPeer->GetSendQueueInfo((DPNID) 0x00200002, &dwNumMsgs, &dwNumBytes, 0);
	if (hr != DPNERR_INVALIDPLAYER)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't fail with expected error INVALIDPLAYER!");
	else
	{
		xLog(hLog, XLL_INFO, "GetSendQInfo returned INVALIDPLAYER successfully");

		if(dwNumMsgs != 666)
			xLog(hLog, XLL_FAIL, "Num msgs was changed (%u != 666)", dwNumMsgs);
		else
		{
			xLog(hLog, XLL_INFO, "Num msgs wasn't changed");

			if(dwNumBytes != 666)
				xLog(hLog, XLL_FAIL, "Num bytes was changed (%u != 666)", dwNumBytes);
			else
				xLog(hLog, XLL_PASS, "Num bytes wasn't changed");
		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo closing host after multiple SendQInfo requests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Re-initializing peer object with context, 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo re-init with context, 0 flags");
	
	nonhostcontext.hLog = hLog;

	hr = pPeer->Initialize(&nonhostcontext, PeerGetSendQInfoHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating new peer host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo create peer host object");

	pDP8Address->Release();
	pDP8Address = NULL;

	hostcontext.hLog = hLog;

	hr = PeerCreateHost(hLog, PeerGetSendQInfoHandler, &hostcontext, &dpAppDesc, &pPeerHost, NULL, &pDP8Address);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "PeerCreateHost didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "PeerCreateHost returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting peer"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo connect peer to host synchronous");

	// Even though it's synchronous, we'll still get a completion.  The async op
	// handle will be NULL, though.
	nonhostcontext.fConnectCanComplete = TRUE;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8Address, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, DPNCONNECT_SYNC);
	
	// Prevent any weird connects from completing.
	nonhostcontext.fConnectCanComplete = FALSE;
	
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Connect didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned DPN_OK successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified");
	
			if(!nonhostcontext.fConnectCompleted)
				xLog(hLog, XLL_FAIL, "Didn't receive a connection completion indication!");
			else
				xLog(hLog, XLL_PASS, "Received a connection completion indication.");
		}
	}

	// Reset the context.
	nonhostcontext.fConnectCompleted = FALSE;
	
	xEndVariation(hLog);		
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info of host on client"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo of host on client");
	
	dwNumMsgs = 666;
	dwNumBytes = 666;

	// TODO - does 0x00200002 really always map to the local player?
	
	hr = pPeer->GetSendQueueInfo((DPNID) 0x00200002, &dwNumMsgs, &dwNumBytes, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "GetSendQInfo returned DPN_OK successfully");

		if(dwNumMsgs != 0)
			xLog(hLog, XLL_FAIL, "Num msgs was not expected (%u != 0)", dwNumMsgs);
		else
		{
			xLog(hLog, XLL_INFO, "Num msgs was expected");

			if(dwNumBytes != 0)
				xLog(hLog, XLL_FAIL, "Num bytes was not expected (%u != 0)", dwNumBytes);
			else
				xLog(hLog, XLL_PASS, "Num bytes was expected");
		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info of host on client with NULL num msgs"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo of host on client with NULL num msgs");
	
	dwNumBytes = 666;
	
	// TODO - does 0x00200002 really always map to the local player?

	hr = pPeer->GetSendQueueInfo((DPNID) 0x00200002, NULL, &dwNumBytes, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "GetSendQInfo returned DPN_OK successfully");
		
		if(dwNumBytes != 0)
			xLog(hLog, XLL_FAIL, "Num bytes was not expected (%u != 0)", dwNumBytes);
		else
			xLog(hLog, XLL_PASS, "Num bytes was expected");
	}
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting send queue info of host on client with NULL num bytes"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo of host on client with NULL num msgs");
	
	dwNumMsgs = 666;
	
	// TODO - does 0x00200002 really always map to the local player?

	hr = pPeer->GetSendQueueInfo((DPNID) 0x00200002, &dwNumMsgs, NULL, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "GetSendQInfo didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "GetSendQInfo returned DPN_OK successfully");
		
		if(dwNumMsgs != 0)
			xLog(hLog, XLL_FAIL, "Num msgs was not expected (%u != 0)", dwNumMsgs);
		else
			xLog(hLog, XLL_PASS, "Num msgs was expected");
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo closing peer after multiple SendQInfo requests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo closing host after multiple SendQInfo requests (2)");
	
	hr = pPeerHost->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo release host after send tests");
	
	hr = pPeerHost->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeerHost = NULL;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetSendQInfo release peer after send tests");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeer = NULL;

	xEndVariation(hLog);	

Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// Release the peer interface if we haven't already
	if(pPeerHost)
	{
		pPeerHost->Release();
		pPeerHost = NULL;
	}

	// Release the address if we haven't already
	if(pDP8Address)
	{
		pDP8AddressHost->Release();
		pDP8AddressHost = NULL;
	}

	// TODO - Need to release the voice interface as well

	return fSuccess;
}

//==================================================================================
// PeerParamsHostTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.8 - Peer Host parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
HRESULT PeerParamsHostTests(HANDLE pTNecd)
{
	PEERHOSTCONTEXT	context;
	DPN_APPLICATION_DESC dpAppDesc, dpAppDescCompare;
	PDIRECTPLAY8ADDRESS	aDP8Addresses[2];
	PDIRECTPLAY8PEER pPeer;
	HRESULT	hr;
	LPVOID pVoice;
	BOOL fSuccess = TRUE;

	ZeroMemory(aDP8Addresses, (2 * sizeof (PDIRECTPLAY8ADDRESS)));
	ZeroMemory(&context, sizeof (PARMVPHOSTCONTEXT));

	// Creating DirectPlay8Peer object
	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");	
	
	// Creating first DirectPlay8Address object
	hr = DirectPlay8AddressCreate(&aDP8Addresses[0]);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (aDP8Addresses[0]) returned S_OK successfully");		

	// Creating second DirectPlay8Address object
	hr = DirectPlay8AddressCreate(&aDP8Addresses[1]);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (aDP8Addresses[1]) returned S_OK successfully");		

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with all NULLs and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with all NULLs and 0 flags");

	hr = pPeer->Host(NULL, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDPOINTER!");
	else
		xLog(hLog, XLL_INFO, "Host returned INVALIDPOINTER successfully");

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with NULL app desc"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with NULL app desc");
	
	hr = pPeer->Host(NULL, aDP8Addresses, 1, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDPOINTER!");
	else
		xLog(hLog, XLL_INFO, "Host returned INVALIDPOINTER successfully");

	// TODO - make sure address hasn't changed.
	
	xEndVariation(hLog);

#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with app desc of 0 size"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with app desc of 0 size");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.guidApplication = GUID_PEER_HOST;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDPARAM successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with app desc of too-large size"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with app desc of too-large size");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC) + 1;
	dpAppDesc.guidApplication = GUID_PEER_HOST;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDPARAM successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with app desc using invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with app desc using invalid flags");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.dwFlags = 0x666;
	dpAppDesc.guidApplication = GUID_PEER_HOST;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDFLAGS!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDFLAGS successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with app desc using REQUIREPASSWORD flag without string specified"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with app desc using REQUIREPASSWORD flag without string specified");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.dwFlags = DPNSESSION_REQUIREPASSWORD;
	dpAppDesc.guidApplication = GUID_PEER_HOST;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDPARAM successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with app desc using password string without flag specified"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with app desc using password string without flag specified");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_HOST;
	dpAppDesc.pwszPassword = L"Password";
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDPARAM successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with app desc using GUID_NULL application"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with app desc using GUID_NULL application");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, NULL, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDPARAM successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);
	
#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with NULL device address"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with NULL device address");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_HOST;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, NULL, 1, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDDEVICEADDRESS)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDDEVICEADDRESS!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDDEVICEADDRESS successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);
	
#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with 0 device address count"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with 0 device address count");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_HOST;

	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

	hr = pPeer->Host(&dpAppDesc, aDP8Addresses, 0, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDDEVICEADDRESS)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDDEVICEADDRESS!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDDEVICEADDRESS successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	// TODO - make sure address object hasn't changed

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with empty address"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with empty address");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_HOST;

	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, aDP8Addresses, 1, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDDEVICEADDRESS)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDDEVICEADDRESS!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDDEVICEADDRESS successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	// TODO - make sure address object hasn't changed

	xEndVariation(hLog);

	// Next tests require address object to be set for an unknown SP
	hr = aDP8Addresses[0]->SetSP(&GUID_UNKNOWN);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting before initialization"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host before initialization");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_HOST;

	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, aDP8Addresses, 1, NULL, NULL, NULL, 0);
	if (hr != DPNERR_UNINITIALIZED)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error UNINITIALIZED!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned UNINITIALIZED successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	// TODO - make sure address object hasn't changed

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Initializing with context, 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host initialize");	
	
	context.hLog = hLog;

	hr = pPeer->Initialize(&context, PeerHostHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with address using unknown GUID"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with address using unknown GUID");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_HOST;

	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, aDP8Addresses, 1, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDDEVICEADDRESS)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDDEVICEADDRESS!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDDEVICEADDRESS successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	// TODO - make sure address object hasn't changed

	xEndVariation(hLog);

	// Next tests require address object to be set for TCP/IP
	hr = aDP8Addresses[0]->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with app desc using CLIENT_SERVER flag"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with app desc using CLIENT_SERVER flag");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.dwFlags = DPNSESSION_CLIENT_SERVER;
	dpAppDesc.guidApplication = GUID_PEER_HOST;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, aDP8Addresses, 1, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDPARAM successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with 2 devices, second with no SP specified"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with 2 devices, second with no SP specified");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_HOST;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, aDP8Addresses, 2, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDDEVICEADDRESS)
		xLog(hLog, XLL_FAIL, "Host didn't fail with expected error INVALIDDEVICEADDRESS!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned INVALIDDEVICEADDRESS successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	// TODO - make sure address object hasn't changed

	xEndVariation(hLog);
	
	// Next tests require second address object to be set for TCP/IP
	hr = aDP8Addresses[1]->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	

	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting with 2 devices using same SPs");
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host with 2 devices, second with no SP specified");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_HOST;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, aDP8Addresses, 2, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Host didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned DPN_OK successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	// TODO - make sure address object hasn't changed

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host closing peer after host tests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: Host release peer after host tests");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeerHost = NULL;

	xEndVariation(hLog);	

Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// Release the address if we haven't already
	if(aDP8Addresses[0])
	{
		aDP8Addresses[0]->Release();
		aDP8Addresses[0] = NULL;
	}

	// Release the address if we haven't already
	if(aDP8Addresses[1])
	{
		aDP8Addresses[1]->Release();
		aDP8Addresses[1] = NULL;
	}

	// TODO - Need to release the voice interface as well

	return fSuccess;

}

//==================================================================================
// PeerParamsGetAppDescTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.9 - Peer GetApplicationDesc parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
HRESULT PeerParamsGetAppDescTests(HANDLE hLog)
{
	DPN_APPLICATION_DESC dpAppDesc, dpAppDescCompare, *pdpAppDesc;
	PDIRECTPLAY8ADDRESS	pDP8Address;
	PDIRECTPLAY8PEER pPeer;
	HRESULT	hr;
	LPVOID pVoice;
	DWORD dwSize, dwExpectedSize;
	BOOL fSuccess = TRUE;

	// Creating DirectPlay8Peer object
	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");	
	
	// Creating first DirectPlay8Address object
	hr = DirectPlay8AddressCreate(&pDP8Address);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (pDP8Address) returned S_OK successfully");		

	// Next tests require address object to be set for TCP/IP
	hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc with NULLs and 0 flags using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc with NULLs and 0 flags using C++ macro");

	hr = IDirectPlay8Peer_GetApplicationDesc(pPeer, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't fail with expected error INVALIDPOINTER!");
	else
		xLog(hLog, XLL_PASS, "GetApplicationDesc returned INVALIDPOINTER successfully");

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc with NULLs and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc with NULLs and 0 flags");
	
	hr = pPeer->GetApplicationDesc(NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't fail with expected error INVALIDPOINTER!");
	else
		xLog(hLog, XLL_PASS, "GetApplicationDesc returned INVALIDPOINTER successfully");
	
	xEndVariation(hLog);	
	
#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc with NULL buffer and invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc with NULL buffer and invalid flags");
	
	dwSize = 0;

	hr = pPeer->GetApplicationDesc(NULL, &dwSize, 0x666);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't fail with expected error INVALIDPOINTER!");
	else
	{
		xLog(hLog, XLL_INFO, "GetApplicationDesc returned INVALIDPOINTER successfully");
	
		if(dwSize != 0)
			xLog(hLog, XLL_FAIL, "Size was modified!");
		else
			xLog(hLog, XLL_PASS, "Size wasn't modified");
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc with NULL buffer before initialization"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc with NULL buffer before initialization");
	
	dwSize = 0;
	
	hr = pPeer->GetApplicationDesc(NULL, &dwSize, 0);
	if (hr != DPNERR_UNINITIALIZED)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't fail with expected error UNINITIALIZED!");
	else
	{
		xLog(hLog, XLL_INFO, "GetApplicationDesc returned UNINITIALIZED successfully");
	
		if(dwSize != 0)
			xLog(hLog, XLL_FAIL, "Size was modified!");
		else
			xLog(hLog, XLL_PASS, "Size wasn't modified");
	}
	
	xEndVariation(hLog);

	// Next tests require peer object to be initialized
	// Initializing with 0 flags

	hr = pPeer->Initialize(NULL, PeerNewPlayerHandler, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Initializing with 0 flags failed!");
		fSuccess = FALSE;
		goto Exit;
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc with NULL buffer without a connection"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc with NULL buffer without a connection");
	
	hr = pPeer->GetApplicationDesc(NULL, &dwSize, 0);
	if (hr != DPNERR_NOCONNECTION)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't fail with expected error NOCONNECTION!");
	else
	{
		xLog(hLog, XLL_INFO, "GetApplicationDesc returned NOCONNECTION successfully");
	
		if(dwSize != 0)
			xLog(hLog, XLL_FAIL, "Size was modified!");
		else
			xLog(hLog, XLL_PASS, "Size wasn't modified");
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting session"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetApplicationDesc start hosting session");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;

	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

	hr = pPeer->Host(&dpAppDesc, &pDP8Address, 1, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Host didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc with NULL buffer"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc with NULL buffer");
	
	dwSize = 0;
	dwExpectedSize = sizeof (DPN_APPLICATION_DESC);
	
	hr = pPeer->GetApplicationDesc(NULL, &dwSize, 0);
	if (hr != DPNERR_BUFFERTOOSMALL)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't fail with expected error BUFFERTOOSMALL!");
	else
	{
		xLog(hLog, XLL_INFO, "GetApplicationDesc returned BUFFERTOOSMALL successfully");
	
		if(dwSize != dwExpectedSize)
			xLog(hLog, XLL_FAIL, "Size doesn't match expected (%u != %u)!", dwSize, dwExpectedSize);
		else
			xLog(hLog, XLL_PASS, "Size matches expected value");
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc using buffer with app desc size of 0"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc using buffer with app desc size of 0");
	
	// Allocate a 1 meg buffer.
	pdpAppDesc = (PBYTE) LocalAlloc(LPTR, dwSize + 64);
	if(!pdpAppDesc)
	{
		xLog(hLog, XLL_FAIL, "Couldn't allocate application desc buffer");
		fSuccess = FALSE;
		goto Exit;
	}

	memset(pdpAppDesc + sizeof(DWORD), 0xBA, dwSize - sizeof(DWORD) + 64);
	pdpAppDesc->dwSize = 0;
	dwSize = sizeof (DPN_APPLICATION_DESC);
	
	hr = pPeer->GetApplicationDesc(pdpAppDesc, &dwSize, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "GetApplicationDesc returned INVALIDPARAM successfully");
	
		if(dwSize != dwExpectedSize)
			xLog(hLog, XLL_FAIL, "Size doesn't match expected (%u != %u)!", dwSize, dwExpectedSize);
		else
		{
			xLog(hLog, XLL_INFO, "Size matches expected value");
			if((pdpAppDesc->dwSize != 0) ||  (*((PBYTE) pdpAppDesc + dwSize) != 0xBA))
				xLog(hLog, XLL_FAIL, "The %u byte app desc buffer was modified or overrun!", pdpAppDesc->dwSize);
			else
				xLog(hLog, XLL_PASS, "App desc buffer wasn't modified or overrun");

		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc using buffer with app desc size too large"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc using buffer with app desc size too large");

	memset(pdpAppDesc + sizeof(DWORD), 0xBA, dwSize - sizeof(DWORD) + 64);
	pdpAppDesc->dwSize = sizeof (DPN_APPLICATION_DESC) + 1;
	dwSize = sizeof (DPN_APPLICATION_DESC);
	
	hr = pPeer->GetApplicationDesc(pdpAppDesc, &dwSize, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "GetApplicationDesc returned INVALIDPARAM successfully");
	
		if(dwSize != dwExpectedSize)
			xLog(hLog, XLL_FAIL, "Size doesn't match expected (%u != %u)!", dwSize, dwExpectedSize);
		else
		{
			xLog(hLog, XLL_INFO, "Size matches expected value");
			if((pdpAppDesc->dwSize != sizeof (DPN_APPLICATION_DESC) + 1) ||  (*((PBYTE) pdpAppDesc + dwSize) != 0xBA))
				xLog(hLog, XLL_FAIL, "The %u byte app desc buffer was modified or overrun!", pdpAppDesc->dwSize);
			else
				xLog(hLog, XLL_PASS, "App desc buffer wasn't modified or overrun");

		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc normal");

	memset(pdpAppDesc + sizeof(DWORD), 0xBA, dwSize - sizeof(DWORD) + 64);
	pdpAppDesc->dwSize = sizeof (DPN_APPLICATION_DESC);
	dwSize = sizeof (DPN_APPLICATION_DESC);

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;

	hr = pPeer->GetApplicationDesc(pdpAppDesc, &dwSize, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "GetApplicationDesc returned DPN_OK successfully");
	
		if(dwSize != dwExpectedSize)
			xLog(hLog, XLL_FAIL, "Size doesn't match expected (%u != %u)!", dwSize, dwExpectedSize);
		else
		{
			xLog(hLog, XLL_INFO, "Size matches expected value");

			if(*((PBYTE) pdpAppDesc + dwSize) != 0xBA)
				xLog(hLog, XLL_FAIL, "The app desc buffer was overrun!", pdpAppDesc->dwSize);
			else
			{
				xLog(hLog, XLL_INFO, "App desc buffer wasn't overrun");

				if(PeerCompareAppDesc(pdpAppDesc, &dpAppDesc) != DPN_OK)
					xLog(hLog, XLL_FAIL, "Retrieved app desc doesn't match desc passed to Host call");
				else
					xLog(hLog, XLL_PASS, "Retrieved app desc matches desc passed to Host call");
		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Getting app desc with extra buffer size"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc with extra buffer size");

	memset(pdpAppDesc + sizeof(DWORD), 0xBA, dwSize - sizeof(DWORD) + 64);
	pdpAppDesc->dwSize = sizeof (DPN_APPLICATION_DESC);
	dwSize = sizeof (DPN_APPLICATION_DESC) + 64;

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
	
	hr = pPeer->GetApplicationDesc(pdpAppDesc, &dwSize, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "GetApplicationDesc didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "GetApplicationDesc returned DPN_OK successfully");
	
		if(dwSize != dwExpectedSize)
			xLog(hLog, XLL_FAIL, "Size doesn't match expected (%u != %u)!", dwSize, dwExpectedSize);
		else
		{
			xLog(hLog, XLL_INFO, "Size matches expected value");

			if(*((PBYTE) pdpAppDesc + dwSize) != 0xBA)
				xLog(hLog, XLL_FAIL, "The app desc buffer was overrun!", pdpAppDesc->dwSize);
			else
			{
				xLog(hLog, XLL_INFO, "App desc buffer wasn't overrun");

				if(PeerCompareAppDesc(pdpAppDesc, &dpAppDesc) != DPN_OK)
					xLog(hLog, XLL_FAIL, "Retrieved app desc doesn't match desc passed to Host call");
				else
					xLog(hLog, XLL_PASS, "Retrieved app desc matches desc passed to Host call");
		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc closing peer after GetAppDesc tests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: GetAppDesc release peer after GetAppDesc tests");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return S_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned S_OK successfully");
	pPeerHost = NULL;

	xEndVariation(hLog);	

Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// Release the address if we haven't already
	if(pDP8Address)
	{
		pDP8Address->Release();
		pDP8Address = NULL;
	}

	// Release the app desc buffer if we haven't already
	if(pdpAppDesc)
	{
		LocalFree(pdpAppDesc);
		pdpAppDesc = NULL;
	}

	return fSuccess;
}

//==================================================================================
// PeerParamsSetAppDescTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.10 - Peer SetApplicationDesc parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
HRESULT PeerParamsSetAppDescTests(HANDLE hLog)
{
	PEERSETAPPDESCCONTEXT hostcontext, nonhostcontext;
	DPN_APPLICATION_DESC dpAppDesc, dpAppDescCompare;
	PDIRECTPLAY8ADDRESS	pDP8Address;
	PDIRECTPLAY8PEER pPeer, pPeerHost;
	HRESULT	hr;
	LPVOID pVoice;
	BOOL fSuccess = TRUE;

	ZeroMemory(&hostcontext, sizeof (PARMVPSETAPPDESCCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPSETAPPDESCCONTEXT));

	hostcontext.hLog = hLog;
	nonhostcontext.hLog = hLog;

	// Create an event to signal when a app description is updated
	if(!(hostcontext.hGotAppDescUpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Create an event to signal when a app description is updated
	if(!(nonhostcontext.hGotAppDescUpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Creating DirectPlay8Peer object
	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");	
	
	// Creating first DirectPlay8Address object
	hr = DirectPlay8AddressCreate(&pDP8Address);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (pDP8Address) returned S_OK successfully");		

	// Next tests require address object to be set for TCP/IP
	hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Setting app desc with NULL and 0 flags using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc with NULLs and 0 flags using C++ macro");
	
	hr = IDirectPlay8Peer_SetApplicationDesc(pPeer, NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "SetApplicationDesc didn't fail with expected error INVALIDPOINTER!");
	else
		xLog(hLog, XLL_PASS, "SetApplicationDesc returned INVALIDPOINTER successfully");

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Setting app desc with NULL and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc with NULLs and 0 flags");
	
	hr = pPeer->SetApplicationDesc(NULL, 0);
	if (hr != DPNERR_INVALIDPOINTER)
		xLog(hLog, XLL_FAIL, "SetApplicationDesc didn't fail with expected error INVALIDPOINTER!");
	else
		xLog(hLog, XLL_PASS, "SetApplicationDesc returned INVALIDPOINTER successfully");

	xEndVariation(hLog);	

#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Setting app desc with invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc with invalid flags");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_SETAPPDESC;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->SetApplicationDesc(&dpAppDesc, 0x666);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "SetAppDesc didn't fail with expected error INVALIDFLAGS!");
	else
	{
		xLog(hLog, XLL_INFO, "SetAppDesc returned INVALIDFLAGS successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Setting app desc before init"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc before init");

	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_SETAPPDESC;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

	hr = pPeer->SetApplicationDesc(&dpAppDesc, 0);
	if (hr != DPNERR_UNINITIALIZED)
		xLog(hLog, XLL_FAIL, "SetAppDesc didn't fail with expected error UNINITIALIZED!");
	else
	{
		xLog(hLog, XLL_INFO, "SetAppDesc returned UNINITIALIZED successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}
	
	xEndVariation(hLog);

	// Next tests require peer object to be initialized
	// Initializing with 0 flags

	hostcontext.hLog = hLog;

	hr = pPeer->Initialize(&hostcontext, PeerSetAppDescHandler, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Initializing with 0 flags failed!");
		fSuccess = FALSE;
		goto Exit;
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Setting app desc without a connection"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc without a connection");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_SETAPPDESC;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->SetApplicationDesc(&dpAppDesc, 0);
	if (hr != DPNERR_NOTHOST)
		xLog(hLog, XLL_FAIL, "SetAppDesc didn't fail with expected error NOTHOST!");
	else
	{
		xLog(hLog, XLL_INFO, "SetAppDesc returned NOTHOST successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting session"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc start hosting");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, &pDP8Address, 1, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Host didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);	

	// TODO - make sure address object hasn't changed
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Setting app desc"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc normal");

	// Expect the update message.
	hostcontext.fCanGetAppDescUpdate = TRUE;
	
	hr = pPeer->SetApplicationDesc(&dpAppDesc, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "SetAppDesc didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "SetAppDesc returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for app desc update to be indicated on host"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc wait for first app desc update");

	if(WaitForSingleObject(&hostcontext.hGotAppDescUpdateEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Application desc update didn't occur within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Application desc update callback signalled");

		if (! hostcontext.fGotAppDescUpdate)
			xLog(hLog, XLL_FAIL, "Didn't get application desc update indication!");
		else
			xLog(hLog, XLL_PASS, "Got application desc update indication");
	}

	// Prevent any weird update indications.
	hostcontext.fCanGetAppDescUpdate = FALSE;

	// Reset the context.
	hostcontext.fGotAppDescUpdate = FALSE;
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc closing peer after first SetAppDesc tests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Re-initializing peer object with nonhost context"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc initialize");	
	
	nonhostcontext.hLog = hLog;

	hr = pPeer->Initialize(&nonhostcontext, PeerSetAppDescHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating new peer host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc creating new peer host object");	
	
	pDP8Address->Release();
	pDP8Address = NULL;
	
	hostcontext.hLog = hLog;

	hr = PeerCreateHost(hLog, PeerSetAppDescHandler, &hostcontext, &dpAppDesc, &pPeerHost, NULL, &pDP8Address);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "PeerCreateHost didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "PeerCreateHost returned DPN_OK successfully");
	
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting peer"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc connecting peer");	
	
	// Even though it's synchronous, we'll still get a completion.  The async op
	// handle will be NULL, though.
	nonhostcontext.fConnectCanComplete = TRUE;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8Address, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, DPNCONNECT_SYNC);
	
	// Prevent any weird connects from completing.
	nonhostcontext.fConnectCanComplete = FALSE;
	
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Connect didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified");
			
			if(!nonhostcontext.fConnectCompleted)
				xLog(hLog, XLL_FAIL, "Didn't get connect completion indication!");
			else
				xLog(hLog, XLL_PASS, "Got connect completion indication!");
		}
	}

	// Reset the context.
	nonhostcontext.fConnectCompleted = FALSE;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Setting app desc on peer non-host"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc on peer non-host");	
	
	hr = pPeer->SetApplicationDesc(&dpAppDesc, 0);
	if (hr != DPNERR_NOTHOST)
		xLog(hLog, XLL_FAIL, "SetAppDesc didn't fail with expected error NOTHOST!");
	else
	{
		xLog(hLog, XLL_INFO, "SetAppDesc returned NOTHOST successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Setting app desc on peer host"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc on peer host");	
	
	// Expect the update message on both interfaces.
	hostcontext.fCanGetAppDescUpdate = TRUE;
	nonhostcontext.fCanGetAppDescUpdate = TRUE;
	
	hr = pPeerHost->SetApplicationDesc(&dpAppDesc, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "SetAppDesc didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "SetAppDesc returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for app desc update to be indicated on host"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc wait for app desc update on host");

	if(WaitForSingleObject(&hostcontext.hGotAppDescUpdateEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Application desc update didn't occur within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Application desc update callback signalled");

		if (! hostcontext.fGotAppDescUpdate)
			xLog(hLog, XLL_FAIL, "Didn't get application desc update indication!");
		else
			xLog(hLog, XLL_PASS, "Got application desc update indication");
	}

	// Prevent any weird update indications.
	hostcontext.fCanGetAppDescUpdate = FALSE;

	// Reset the context.
	hostcontext.fGotAppDescUpdate = FALSE;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for app desc update to be indicated on nonhost"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc wait for app desc update on host");

	if(WaitForSingleObject(&nonhostcontext.hGotAppDescUpdateEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Application desc update didn't occur within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Application desc update callback signalled");

		if (! nonhostcontext.fGotAppDescUpdate)
			xLog(hLog, XLL_FAIL, "Didn't get application desc update indication!");
		else
			xLog(hLog, XLL_PASS, "Got application desc update indication");
	}

	// Prevent any weird update indications.
	nonhostcontext.fCanGetAppDescUpdate = FALSE;

	// Reset the context.
	nonhostcontext.fGotAppDescUpdate = FALSE;
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer client object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc closing peer after second SetAppDesc tests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc closing host");
	
	hr = pPeerHost->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing hosting DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc releasing host");
	
	hr = pPeerHost->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned DPN_OK successfully");
	pPeerHost = NULL;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: SetAppDesc releasing peer");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned DPN_OK successfully");
	pPeer = NULL;
	
	xEndVariation(hLog);

Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// Release the peer host interface if we haven't already
	if(pPeerHost)
	{
		pPeerHost->Release();
		pPeerHost = NULL;
	}

	// Release the address if we haven't already
	if(pDP8Address)
	{
		pDP8Address->Release();
		pDP8Address = NULL;
	}

	// Release the app desc buffer if we haven't already
	if(pdpAppDesc)
	{
		LocalFree(pdpAppDesc);
		pdpAppDesc = NULL;
	}

	// Close the hostcontext AppDescUpdateEvent handle
	if(hostcontext.hGotAppDescUpdateEvent)
	{
		CloseHandle(hostcontext.hGotAppDescUpdateEvent);
		hostcontext.hGotAppDescUpdateEvent = NULL;
	}

	// Close the nonhostcontext AppDescUpdateEvent handle
	if(nonhostcontext.hGotAppDescUpdateEvent)
	{
		CloseHandle(nonhostcontext.hGotAppDescUpdateEvent);
		nonhostcontext.hGotAppDescUpdateEvent = NULL;
	}

	return fSuccess;
}

//==================================================================================
// PeerParamsCreateGroupTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.11 - Peer CreateGroup parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
HRESULT ParmVPeerExec_CreateGroup(HANDLE hLog)
{
	PEERCREATEGROUPCONTEXT hostcontext, nonhostcontext;
	DPN_APPLICATION_DESC dpAppDesc, dpAppDescCompare;
	PDIRECTPLAY8ADDRESS	pDP8Address;
	PDIRECTPLAY8PEER pPeer, pPeerHost;
	DPN_GROUP_INFO dpGroupInfo, dpGroupInfoCompare;
	DPNHANDLE dpnhCreateGroup;
	HRESULT	hr;
	LPVOID pVoice;
	DPNID dpnidGroup;
	BOOL fSuccess = TRUE;

	ZeroMemory(&hostcontext, sizeof (PARMVPCREATEGROUPCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPCREATEGROUPCONTEXT));

	// Creating DirectPlay8Peer object
	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");	
	
	// Creating first DirectPlay8Address object
	hr = DirectPlay8AddressCreate(&pDP8Address);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (pDP8Address) returned S_OK successfully");		

	// Next tests require address object to be set for TCP/IP
	hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	

#ifdef BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group with all NULLs and 0 flags using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup with all NULLs and 0 flags using C++ macro");
	
	hr = IDirectPlay8Peer_CreateGroup(pPeer->m_pDP8Peer, NULL, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "CreateGroup returned INVALIDPARAM successfully");

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group with all NULLs and 0 flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup with all NULLs and 0 flags");
	
	hr = pPeer->CreateGroup(NULL, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "CreateGroup returned INVALIDPARAM successfully");
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group with NULL group info"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup with NULL group info");
	
	dpnhCreateGroup = (DPNHANDLE) 0x666;
	
	hr = pPeer->CreateGroup(NULL, NULL, NULL, &dpnhCreateGroup, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned INVALIDPARAM successfully");

		if(dpnhCreateGroup != (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async handle was modified! (%u != 0x666)", dpnhCreateGroup);
		else
			xLog(hLog, XLL_PASS, "Async handle wasn't modified");
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group with NULL async handle"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup with NULL async handle");
	
	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);

	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));
	
	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned INVALIDPARAM successfully");

		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info was modified! (%u != 0x666)", dpnhCreateGroup);
		else
			xLog(hLog, XLL_PASS, "Group info wasn't modified");
	}
	
	xEndVariation(hLog);
	
#endif // BAD_PARAM_TESTS

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group with invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup with invalid flags");
	
	dpnhCreateGroup = (DPNHANDLE) 0x666;
	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);

	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));
	
	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, &dpnhCreateGroup, 0x666);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned INVALIDPARAM successfully");
		
		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info was modified! (%u != 0x666)", dpnhCreateGroup);
		else
		{
			xLog(hLog, XLL_INFO, "Group info wasn't modified");
			
			if(dpnhCreateGroup != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async handle was modified! (%u != 0x666)", dpnhCreateGroup);
			else
				xLog(hLog, XLL_PASS, "Async handle wasn't modified");
		}
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group synchronously with async handle"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup synchronously with async handle");

	dpnhCreateGroup = (DPNHANDLE) 0x666;
	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);

	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));

	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, &dpnhCreateGroup, DPNCREATEGROUP_SYNC);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned INVALIDPARAM successfully");
		
		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info was modified! (%u != 0x666)", dpnhCreateGroup);
		else
		{
			xLog(hLog, XLL_INFO, "Group info wasn't modified");
			
			if(dpnhCreateGroup != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async handle was modified! (%u != 0x666)", dpnhCreateGroup);
			else
				xLog(hLog, XLL_PASS, "Async handle wasn't modified");
		}
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group before initialization"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup before initialization");
	
	dpnhCreateGroup = (DPNHANDLE) 0x666;
	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);

	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));

	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, &dpnhCreateGroup, 0);
	if (hr != DPNERR_UNINITIALIZED)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't fail with expected error UNINITIALIZE!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned UNINITIALIZE successfully");
		
		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info was modified! (%u != 0x666)", dpnhCreateGroup);
		else
		{
			xLog(hLog, XLL_INFO, "Group info wasn't modified");
			
			if(dpnhCreateGroup != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async handle was modified! (%u != 0x666)", dpnhCreateGroup);
			else
				xLog(hLog, XLL_PASS, "Async handle wasn't modified");
		}
	}
	
	xEndVariation(hLog);

	// Next tests require peer object to be initialized
	// Initializing with 0 flags
	hostcontext.hLog = hLog;

	hr = pPeer->Initialize(&hostcontext, PeerCreateGroupHandler, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Initializing with 0 flags failed!");
		fSuccess = FALSE;
		goto Exit;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group without a connection"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup without a connection");

	dpnhCreateGroup = (DPNHANDLE) 0x666;
	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);

	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));

	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, &dpnhCreateGroup, 0);
	if (hr != DPNERR_NOCONNECTION)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't fail with expected error NOCONNECTION!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned NOCONNECTION successfully");
		
		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info was modified! (%u != 0x666)", dpnhCreateGroup);
		else
		{
			xLog(hLog, XLL_INFO, "Group info wasn't modified");
			
			if(dpnhCreateGroup != (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async handle was modified! (%u != 0x666)", dpnhCreateGroup);
			else
				xLog(hLog, XLL_PASS, "Async handle wasn't modified");
		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting session"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup hosting session");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, &pDP8Address, 1, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Host didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}

	xEndVariation(hLog);

	// TODO - make sure address object hasn't changed

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup async normal");
	
	dpnhCreateGroup = (DPNHANDLE) 0x666;
	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);

	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));

	// Expect the create group message and async op completion.
	// TODO - does 0x00200002 really always map to the local player?
	hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
	hostcontext.fCanGetCreateGroup = TRUE;
	
	// Create an event to signal when a group is created
	if(!(hostcontext.hAsyncOpCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	hostcontext.fAsyncOpCanComplete = TRUE;
	
	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, &dpnhCreateGroup, 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't succeed with expected return PENDING!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned PENDING successfully");
		
		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info was modified! (%u != 0x666)", dpnhCreateGroup);
		else
		{
			xLog(hLog, XLL_INFO, "Group info wasn't modified");
		
			if (dpnhCreateGroup == (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle wasn't modified!");
			else
			{
				xLog(hLog, XLL_INFO, "Async operation handle was modified");

				if (dpnhCreateGroup == NULL)
					xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
				else
					xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
			}
		}
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async op to complete"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup waiting for create group completion");

	if(WaitForSingleObject(&hostcontext.hAsyncOpCompletedEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Create group didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Async callback signalled");

		if (! context.fAsyncOpCompleted)
			xLog(hLog, XLL_FAIL, "Didn't get asynchronous completion indication!");
		else
		{
			xLog(hLog, XLL_INFO, "Got asynchronous completion indication");

			if(hostcontext.dpnhCompletedAsyncOp != dpnhCreateGroup)
				xLog(hLog, XLL_FAIL, "Create group completed with different async op handle (%x != %x)",
				hostcontext.dpnhCompletedAsyncOp, dpnhCreateGroup);
			else
			{
				xLog(hLog, XLL_INFO, "Create group completed with correct async op handle");

				if(hostcontext.dpnidGroupCreated == 0)
					xLog(hLog, XLL_FAIL, "Received message wasn't a create group message!");
				else
					xLog(hLog, XLL_PASS, "Received message was a create group message!");
			}
		}
	}

	// The create group message should have arrived by now.
	hostcontext.fCanGetCreateGroup = FALSE;
	
	// Prevent any weird async op completions.
	hostcontext.fAsyncOpCanComplete = FALSE;

	// Reset the context.
	hostcontext.fAsyncOpCompleted = FALSE;
	CloseHandle(hostcontext.hAsyncOpCompletedEvent);
	hostcontext.hAsyncOpCompletedEvent = NULL;

	// Save the group ID and reset the context.
	dpnidGroup = hostcontext.dpnidGroupCreated;
	hostcontext.dpnidGroupCreated = 0;
	hostcontext.dpnidExpectedGroupOwner = 0;

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating second group synchronously"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup second group synchronously");

	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);

	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));

	// Expect the create group message and async op completion.
	// TODO - does 0x00200002 really always map to the local player?
	hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
	hostcontext.fCanGetCreateGroup = TRUE;
	
	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned DPN_OK successfully");
		
		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info was modified! (%u != 0x666)", dpnhCreateGroup);
		else
		{
			xLog(hLog, XLL_INFO, "Group info wasn't modified");
			
			if(hostcontext.dpnidGroupCreated == 0)
				xLog(hLog, XLL_FAIL, "Received message wasn't a create group message!");
			else
			{
				xLog(hLog, XLL_INFO, "Received message was a create group message!");
				
				if(hostcontext.dpnidGroupCreated == dpnidGroup)
					xLog(hLog, XLL_FAIL, "Both created groups received the same ID - %u (0x%0x)", dpnidGroup dpnidGroup);
				else
					xLog(hLog, XLL_PASS, "Each group received a unique ID");
			}
		}
	}

	// Prevent any weird create groups.
	hostcontext.fCanGetCreateGroup = FALSE;

	// Reset the context.
	hostcontext.dpnidGroupCreated = 0;
	hostcontext.dpnidExpectedGroupOwner = 0;

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup closing host after first CreateGroup tests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Re-initializing peer object with nonhost context"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup reinitializing");	
	
	nonhostcontext.hLog = hLog;

	hr = pPeer->Initialize(&nonhostcontext, PeerCreateGroupHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating new peer host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup creating new peer host object");	
	
	pDP8Address->Release();
	pDP8Address = NULL;
	
	hostcontext.hLog = hLog;

	hr = PeerCreateHost(hLog, PeerCreateGroupHandler, &hostcontext, &dpAppDesc, &pPeerHost, NULL, &pDP8Address);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "PeerCreateHost didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "PeerCreateHost returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting peer"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup connecting peer");	
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

	// Even though it's synchronous, we'll still get a completion.  The async op
	// handle will be NULL, though.
	nonhostcontext.fConnectCanComplete = TRUE;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8Address, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, DPNCONNECT_SYNC);
	
	// Prevent any weird connects from completing.
	nonhostcontext.fConnectCanComplete = FALSE;
	
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Connect didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified");
			
			if(!nonhostcontext.fConnectCompleted)
				xLog(hLog, XLL_FAIL, "Didn't get connect completion indication!");
			else
				xLog(hLog, XLL_PASS, "Got connect completion indication!");
		}
	}

	// Reset the context.
	nonhostcontext.fConnectCompleted = FALSE;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group on host"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup group on host");

	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);

	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));

	dpnhCreateGroup = (DPNHANDLE) 0x666;

	// Expect the create group message and async op completion.
	// TODO - does 0x00200002 really always map to the local player?
	hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
	hostcontext.fCanGetCreateGroup = TRUE;

	// Create an event to signal when a group is created
	if(!(hostcontext.hAsyncOpCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	hostcontext.fAsyncOpCanComplete = TRUE;
	
	// Expect the create group message and async op completion.
	// TODO - does 0x00200002 really always map to the local player?
	nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
	nonhostcontext.fCanGetCreateGroup = TRUE;

	// Create an event to signal when a group is created
	if(!(nonhostcontext.hGotCreateGroupEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	hr = pPeerHost->CreateGroup(&dpGroupInfo, NULL, NULL, &dpnhCreateGroup, 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't succeed with expected return PENDING!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned PENDING successfully");
		
		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info was modified! (%u != 0x666)", dpnhCreateGroup);
		else
		{
			xLog(hLog, XLL_INFO, "Group info wasn't modified");
		
			if (dpnhCreateGroup == (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle wasn't modified!");
			else
			{
				xLog(hLog, XLL_INFO, "Async operation handle was modified");

				if (dpnhCreateGroup == NULL)
					xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
				else
					xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
			}
		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async op to complete"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup wait for create group completion on host");

	if(WaitForSingleObject(&hostcontext.hAsyncOpCompletedEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Create group didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Async callback signalled");

		if (! hostcontext.fAsyncOpCompleted)
			xLog(hLog, XLL_FAIL, "Didn't get asynchronous completion indication!");
		else
		{
			xLog(hLog, XLL_INFO, "Got asynchronous completion indication");

			if(hostcontext.dpnhCompletedAsyncOp != dpnhCreateGroup)
				xLog(hLog, XLL_FAIL, "Create group completed with different async op handle (%x != %x)",
				hostcontext.dpnhCompletedAsyncOp, dpnhCreateGroup);
			else
			{
				xLog(hLog, XLL_INFO, "Create group completed with correct async op handle");

				if(hostcontext.dpnidGroupCreated == 0)
					xLog(hLog, XLL_FAIL, "Received message wasn't a create group message!");
				else
					xLog(hLog, XLL_PASS, "Received message was a create group message!");
			}
		}
	}

	// The create group message should have arrived by now.
	hostcontext.fCanGetCreateGroup = FALSE;
	
	// Prevent any weird async op completions.
	hostcontext.fAsyncOpCanComplete = FALSE;

	// Reset the context.
	hostcontext.fAsyncOpCompleted = FALSE;
	CloseHandle(hostcontext.hAsyncOpCompletedEvent);
	hostcontext.hAsyncOpCompletedEvent = NULL;

	// Save the group ID and reset the context.
	hostcontext.dpnidGroupCreated = 0;
	hostcontext.dpnidExpectedGroupOwner = 0;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for group creation to be indicated on client"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup wait for create group message on client");
	
	if(WaitForSingleObject(&nonhostcontext.hGotCreateGroupEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Create group didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Async callback signalled");
		
		if(nonhostcontext.dpnidGroupCreated == 0)
			xLog(hLog, XLL_FAIL, "Received message wasn't a create group message!");
		else
			xLog(hLog, XLL_PASS, "Received message was a create group message!");
	}

	// Prevent any weird group creations.
	nonhostcontext.fCanGetCreateGroup = FALSE;
	
	// Reset the context.
	CloseHandle(nonhostcontext.hGotCreateGroupEvent);
	nonhostcontext.hGotCreateGroupEvent = NULL;
	nonhostcontext.dpnidGroupCreated = 0;
	nonhostcontext.dpnidExpectedGroupOwner = 0;

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group on client"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup group on client");

	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);

	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));

	dpnhCreateGroup = (DPNHANDLE) 0x666;

	// Expect the create group message and async op completion.
	// TODO - does 0x00300003 really always map to the local client player?
	nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
	nonhostcontext.fCanGetCreateGroup = TRUE;

	// Create an event to signal when a group is created
	if(!(nonhostcontext.hAsyncOpCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	nonhostcontext.fAsyncOpCanComplete = TRUE;

	// Expect the create group message and async op completion.
	// TODO - does 0x00300003 really always map to the local client player?
	hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
	hostcontext.fCanGetCreateGroup = TRUE;

	// Create an event to signal when a group is created
	if(!(hostcontext.hAsyncOpCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, &dpnhCreateGroup, 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't succeed with expected return PENDING!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned PENDING successfully");
		
		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info was modified! (%u != 0x666)", dpnhCreateGroup);
		else
		{
			xLog(hLog, XLL_INFO, "Group info wasn't modified");
		
			if (dpnhCreateGroup == (DPNHANDLE) 0x666)
				xLog(hLog, XLL_FAIL, "Async operation handle wasn't modified!");
			else
			{
				xLog(hLog, XLL_INFO, "Async operation handle was modified");

				if (dpnhCreateGroup == NULL)
					xLog(hLog, XLL_FAIL, "No async operation handle was returned!");
				else
					xLog(hLog, XLL_PASS, "Async operation handle appears to have been set correctly");
			}
		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async op to complete"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup wait for create group completion on client");
	
	if(WaitForSingleObject(&nonhostcontext.hAsyncOpCompletedEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Create group didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Async callback signalled");
		
		if (! nonhostcontext.fAsyncOpCompleted)
			xLog(hLog, XLL_FAIL, "Didn't get asynchronous completion indication!");
		else
		{
			xLog(hLog, XLL_INFO, "Got asynchronous completion indication");

			if(nonhostcontext.dpnhCompletedAsyncOp != dpnhCreateGroup)
				xLog(hLog, XLL_FAIL, "Create group completed with different async op handle (%x != %x)",
				hostcontext.dpnhCompletedAsyncOp, dpnhCreateGroup);
			else
			{
				xLog(hLog, XLL_INFO, "Create group completed with correct async op handle");

				if(nonhostcontext.dpnidGroupCreated == 0)
					xLog(hLog, XLL_FAIL, "Received message wasn't a create group message!");
				else
					xLog(hLog, XLL_PASS, "Received message was a create group message!");
			}
		}
	}

	// The create group message should have arrived by now.
	nonhostcontext.fCanGetCreateGroup = FALSE;
	
	// Prevent any weird async op completions.
	nonhostcontext.fAsyncOpCanComplete = FALSE;

	// Reset the context.
	nonhostcontext.fAsyncOpCompleted = FALSE;
	CloseHandle(nonhostcontext.hAsyncOpCompletedEvent);
	nonhostcontext.hAsyncOpCompletedEvent = NULL;
	nonhostcontext.dpnidGroupCreated = 0;
	nonhostcontext.dpnidExpectedGroupOwner = 0;

	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for group creation to be indicated on host"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup wait for create group message on host");

	
	if(WaitForSingleObject(&hostcontext.hGotCreateGroupEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Create group didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Async callback signalled");
		
		if(hostcontext.dpnidGroupCreated == 0)
			xLog(hLog, XLL_FAIL, "Received message wasn't a create group message!");
		else
			xLog(hLog, XLL_PASS, "Received message was a create group message!");
	}

	// Prevent any weird group creations.
	nonhostcontext.fCanGetCreateGroup = FALSE;
	
	// Reset the context.
	CloseHandle(nonhostcontext.hGotCreateGroupEvent);
	nonhostcontext.hGotCreateGroupEvent = NULL;
	nonhostcontext.dpnidGroupCreated = 0;
	nonhostcontext.dpnidExpectedGroupOwner = 0;

	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer client object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup closing peer after host-client CreateGroup tests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup closing host after host-client CreateGroup tests");
	
	hr = pPeerHost->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing hosting DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup releasing host");
	
	hr = pPeerHost->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned DPN_OK successfully");
	pPeerHost = NULL;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: CreateGroup releasing peer");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned DPN_OK successfully");
	pPeer = NULL;
	
	xEndVariation(hLog);


Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// Release the peer host interface if we haven't already
	if(pPeerHost)
	{
		pPeerHost->Release();
		pPeerHost = NULL;
	}

	// Release the address if we haven't already
	if(pDP8Address)
	{
		pDP8Address->Release();
		pDP8Address = NULL;
	}

	
	if(hostcontext.hGotCreateGroupEvent)
	{
		CloseHandle(hostcontext.hGotCreateGroupEvent);
		hostcontext.hGotCreateGroupEvent = NULL;
	}
	
	if(nonhostcontext.hGotCreateGroupEvent)
	{
		CloseHandle(nonhostcontext.hGotCreateGroupEvent);
		nonhostcontext.hGotCreateGroupEvent = NULL;
	}
	
	if(hostcontext.hAsyncOpCompletedEvent)
	{
		CloseHandle(hostcontext.hAsyncOpCompletedEvent);
		hostcontext.hAsyncOpCompletedEvent = NULL;
	}
	
	if(nonhostcontext.hAsyncOpCompletedEvent)
	{
		CloseHandle(nonhostcontext.hAsyncOpCompletedEvent);
		nonhostcontext.hAsyncOpCompletedEvent = NULL;
	}

	return fSuccess;
}

//==================================================================================
// PeerParamsDeleteGroupTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.12 - Peer DestroyGroup parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
HRESULT PeerParamsDeleteGroupTests(HANDLE hLog)
{
	PEERDESTROYGROUPCONTEXT hostcontext, nonhostcontext;
	DPN_APPLICATION_DESC dpAppDesc, dpAppDescCompare;
	PDIRECTPLAY8ADDRESS	pDP8Address;
	PDIRECTPLAY8PEER pPeer, pPeerHost;
	DPN_GROUP_INFO dpGroupInfo, dpGroupInfoCompare;
	DPNHANDLE dpnhDestroyGroup;
	HRESULT	hr;
	LPVOID pVoice;
	DPNID dpnidGroup;
	BOOL fSuccess = TRUE;


	ZeroMemory(&hostcontext, sizeof (PARMVPDESTROYGROUPCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPDESTROYGROUPCONTEXT));
	
	// Creating DirectPlay8Peer object
	pPeer = pVoice = NULL;
	hr = DirectPlay8PeerCreate(&pPeer, &pVoice);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8PeerCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8PeerCreate returned S_OK successfully");	
	
	// Creating first DirectPlay8Address object
	hr = DirectPlay8AddressCreate(&pDP8Address);
	if (hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "DirectPlay8AddressCreate failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "DirectPlay8AddressCreate (pDP8Address) returned S_OK successfully");		

	// Next tests require address object to be set for TCP/IP
	hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "SetSP failed with %u", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	else
		xLog(hLog, XLL_INFO, "SetSP returned DPN_OK successfully");	

#ifdef BAD_PARAM_TESTS
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group 0 (DPNID_ALL_PLAYERS_GROUP) using C++ macro"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup AllPlayersGroup with all NULLs and 0 flags using C++ macro");
	
	hr = IDirectPlay8Peer_DestroyGroup(pPeer, 0, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDGROUP)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't fail with expected error INVALIDGROUP!");
	else
		xLog(hLog, XLL_PASS, "DestroyGroup returned INVALIDGROUP successfully");

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group 0 (DPNID_ALL_PLAYERS_GROUP)"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup AllPlayersGroup with all NULLs and 0 flags");

	hr = pPeer->DestroyGroup(0, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDGROUP)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't fail with expected error INVALIDGROUP!");
	else
		xLog(hLog, XLL_PASS, "DestroyGroup returned INVALIDGROUP successfully");

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group with NULL async handle"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup with NULL async handle");

	// Use 0x666 to avoid DPlay's group ID == 0 check.
	hr = pPeer->DestroyGroup((DPNID) 0x666, NULL, NULL, 0);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't fail with expected error INVALIDPARAM!");
	else
		xLog(hLog, XLL_PASS, "DestroyGroup returned INVALIDPARAM successfully");

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group with invalid flags"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup with invalid flags");
	
	dpnhDestroyGroup = (DPNHANDLE) 0x666;
	
	hr = pPeer->DestroyGroup((DPNID) 0x666, NULL, &dpnhDestroyGroup, 0x666);
	if (hr != DPNERR_INVALIDFLAGS)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't fail with expected error INVALIDFLAGS!");
	else
	{
		xLog(hLog, XLL_INFO, "DestroyGroup returned INVALIDFLAGS successfully");

		if(dpnhDestroyGroup != (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
		else
			xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group synchronously with async op handle"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup synchronously with async op handle");

	dpnhDestroyGroup = (DPNHANDLE) 0x666;

	hr = pPeer->DestroyGroup((DPNID) 0x666, NULL, &dpnhDestroyGroup, DPNDESTROYGROUP_SYNC);
	if (hr != DPNERR_INVALIDPARAM)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't fail with expected error INVALIDPARAM!");
	else
	{
		xLog(hLog, XLL_INFO, "DestroyGroup returned INVALIDPARAM successfully");

		if(dpnhDestroyGroup != (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
		else
			xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group before initialization"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup before initialization");
	
	dpnhDestroyGroup = (DPNHANDLE) 0x666;

	hr = pPeer->DestroyGroup((DPNID) 0x666, NULL, &dpnhDestroyGroup, 0);
	if (hr != DPNERR_UNINITIALIZED)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't fail with expected error UNINITIALIZED!");
	else
	{
		xLog(hLog, XLL_INFO, "DestroyGroup returned UNINITIALIZED successfully");

		if(dpnhDestroyGroup != (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
		else
			xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
	}

	xEndVariation(hLog);

	// Next tests require peer object to be initialized
	// Initializing with 0 flags
	hostcontext.hLog = hLog;

	hr = pPeer->Initialize(&hostcontext, PeerDestroyGroupHandler, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Initializing with 0 flags failed!");
		fSuccess = FALSE;
		goto Exit;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group without a connection"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup without a connection");
	
	dpnhDestroyGroup = (DPNHANDLE) 0x666;

	hr = pPeer->DestroyGroup((DPNID) 0x666, NULL, &dpnhDestroyGroup, 0);
	if (hr != DPNERR_NOCONNECTION)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't fail with expected error NOCONNECTION!");
	else
	{
		xLog(hLog, XLL_INFO, "DestroyGroup returned NOCONNECTION successfully");

		if(dpnhDestroyGroup != (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
		else
			xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Hosting session"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup start host");
	
	ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
	dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));
	
	hr = pPeer->Host(&dpAppDesc, &pDP8Address, 1, NULL, NULL, NULL, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Host didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Host returned DPN_OK successfully");

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
			xLog(hLog, XLL_PASS, "Application desc structure wasn't modified");
	}
	
	xEndVariation(hLog);

	// TODO - make sure address object hasn't changed
		
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying invalid group"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup invalid group");

	dpnhDestroyGroup = (DPNHANDLE) 0x666;

	hr = pPeer->DestroyGroup((DPNID) 0x666, NULL, &dpnhDestroyGroup, 0);
	if (hr != DPNERR_INVALIDGROUP)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't fail with expected error INVALIDGROUP!");
	else
	{
		xLog(hLog, XLL_INFO, "DestroyGroup returned INVALIDGROUP successfully");

		if(dpnhDestroyGroup != (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
		else
			xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying all players group internal ID"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup all players group internal ID");

	dpnhDestroyGroup = (DPNHANDLE) 0x666;

	// TODO - Determine real ID
	hr = pPeer->DestroyGroup((DPNID) 0x00100001, NULL, &dpnhDestroyGroup, 0);
	if (hr != DPNERR_INVALIDGROUP)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't fail with expected error INVALIDGROUP!");
	else
	{
		xLog(hLog, XLL_INFO, "DestroyGroup returned INVALIDGROUP successfully");

		if(dpnhDestroyGroup != (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async operation handle was modified!");
		else
			xLog(hLog, XLL_PASS, "Async operation handle wasn't modified");
	}

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group synchronously"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup create group synchronously");
	
	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
	
	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));
	
	// TODO - does 0x00200002 really always map to the local player?
	// Expect the create group message.
	hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
	hostcontext.fCanGetCreateGroup = TRUE;
	
	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);
	
	// Prevent any weird create groups.
	hostcontext.fCanGetCreateGroup = FALSE;
	
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned DPN_OK successfully");

		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Group info structure wasn't modified");

			if(hostcontext.dpnidGroup == 0)
				xLog(hLog, XLL_FAIL, "Didn't get create group message!");
			else
				xLog(hLog, XLL_PASS, "Got create group message");
		}
	}
	
	// Save the group ID.
	dpnidGroup = hostcontext.dpnidGroup;

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup asynchronously");
	
	dpnhDestroyGroup = (DPNHANDLE) 0x666;

	// Expect the destroy group message and async op completion.
	hostcontext.fCanGetDestroyGroup = TRUE;
	hostcontext.fAsyncOpCanComplete = TRUE;

	// Create an event to signal when a group is created
	if(!(hostcontext.hAsyncOpCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	hr = pPeer->DestroyGroup(hostcontext.dpnidGroup, NULL, &dpnhDestroyGroup, 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't succeed with expected return DPNSUCCESS_PENDING!");
	else
	{
		xLog(hLog, XLL_INFO, "DestroyGroup returned DPNSUCCESS_PENDING successfully");

		if(dpnhDestroyGroup == (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async op handle wasn't modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Async op handle was modified");

			if (dpnhDestroyGroup == NULL)
				xLog(hLog, XLL_FAIL, "Async op handle was set to NULL!");
			else
				xLog(hLog, XLL_PASS, "Async op handle appears to have been set successfully");
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async op to complete"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup waiting for operation to complete");

	if(WaitForSingleObject(&hostcontext.hAsyncOpCompletedEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Delete group didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Async callback signalled");
		
		if(! hostcontext.fAsyncOpCompleted)
			xLog(hLog, XLL_FAIL, "Async delete group didn't complete");
		else
		{
			xLog(hLog, XLL_INFO, "Async delete group completed successfully");
			
			if(hostcontext.dpnhCompletedAsyncOp != dpnhDestroyGroup)
				xLog(hLog, XLL_FAIL, "Destroy group completed with different async op handle (%u != %u)",
				hostcontext.dpnhCompletedAsyncOp, dpnhDestroyGroup);
			else
			{
				xLog(hLog, XLL_INFO, "Destroy group completed with correct async op handle");
				
				if (hostcontext.dpnidGroup != 0)
					xLog(hLog, XLL_FAIL, "Didn't get destroy group message");
				else
					xLog(hLog, XLL_PASS, "Got destroy group message");
				
			}
		}
	}

	// The destroy group message should have arrived by now.
	hostcontext.fCanGetDestroyGroup = FALSE;
	
	// Prevent any weird async op completions.
	hostcontext.fAsyncOpCanComplete = FALSE;

	// Reset the context.
	hostcontext.fAsyncOpCompleted = FALSE;
	CloseHandle(hostcontext.hAsyncOpCompletedEvent);
	hostcontext.hAsyncOpCompletedEvent = NULL;

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group synchronously"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup creating second group synchronously");

	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
	
	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));
	
	// TODO - does 0x00200002 really always map to the local player?
	// Expect the create group message.
	hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
	hostcontext.fCanGetCreateGroup = TRUE;
	
	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);
	
	// Prevent any weird create groups.
	hostcontext.fCanGetCreateGroup = FALSE;
	
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned DPN_OK successfully");

		if(memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Group info structure wasn't modified");

			if(hostcontext.dpnidGroup == 0)
				xLog(hLog, XLL_FAIL, "Didn't get create group message!");
			else
			{
				xLog(hLog, XLL_INFO, "Got create group message");

				if(hostcontext.dpnidGroup == dpnidGroup)
					xLog(hLog, XLL_FAIL, "Second group created has same ID as original group (%u)",
					dpnidGroup);
				else
					xLog(hLog, XLL_PASS, "New group has a unique group ID");
			}
		}
	}

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group synchronously"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup synchronously");
	
	// Expect the destroy group message and async op completion.
	hostcontext.fCanGetDestroyGroup = TRUE;
	
	hr = pPeer->DestroyGroup(hostcontext.dpnidGroup, NULL, NULL, DPNDESTROYGROUP_SYNC);
	
	// Prevent any weird destroy groups.
	hostcontext.fCanGetDestroyGroup = FALSE;
	
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "DestroyGroup returned DPN_OK successfully");
		
		if(hostcontext.dpnidGroup != 0)
			xLog(hLog, XLL_FAIL, "Didn't get destroy group message!");
		else
			xLog(hLog, XLL_PASS, "Got destroy group message");
	}
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DeleteGroup closing peer");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Re-initializing peer object with nonhost context"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DeleteGroup reinitializing");	
	
	nonhostcontext.hLog = hLog;

	hr = pPeer->Initialize(&nonhostcontext, PeerDestroyGroupHandler, 0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Initialize didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Initialize returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating new peer host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DeleteGroup creating new peer host object");	
	
	pDP8Address->Release();
	pDP8Address = NULL;
	
	hostcontext.hLog = hLog;

	hr = PeerCreateHost(hLog, PeerCreateGroupHandler, &hostcontext, &dpAppDesc, &pPeerHost, NULL, &pDP8Address);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "PeerCreateHost didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "PeerCreateHost returned DPN_OK successfully");
	
	xEndVariation(hLog);	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Connecting peer"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DeleteGroup connecting peer");	
	
	// Save what we're passing in to make sure it's not touched.
	CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

	// Even though it's synchronous, we'll still get a completion.  The async op
	// handle will be NULL, though.
	nonhostcontext.fConnectCanComplete = TRUE;
	
	hr = pPeer->Connect(&dpAppDesc, pDP8Address, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, DPNCONNECT_SYNC);
	
	// Prevent any weird connects from completing.
	nonhostcontext.fConnectCanComplete = FALSE;
	
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Connect didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "Connect returned DPN_OK successfully");
		
		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
			xLog(hLog, XLL_FAIL, "Application desc structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Application desc structure wasn't modified");
			
			if(!nonhostcontext.fConnectCompleted)
				xLog(hLog, XLL_FAIL, "Didn't get connect completion indication!");
			else
				xLog(hLog, XLL_PASS, "Got connect completion indication!");
		}
	}

	// Reset the context.
	nonhostcontext.fConnectCompleted = FALSE;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Creating group on client synchronously"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DeleteGroup create group on client synchronously");	
	
	ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
	dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
	
	CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));
	
	// Expect the create group message on the host.
	// TODO - does 0x00300003 really always map to the local client player?
	hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
	hostcontext.fCanGetCreateGroup = TRUE;

	// Create an event to signal when a group is created
	if(!(hostcontext.hGotGroupMsgEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}

	// Expect the create group message on the client.
	// TODO - does 0x00300003 really always map to the local client player?
	nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
	nonhostcontext.fCanGetCreateGroup = TRUE;
	nonhostcontext.dpnidGroup = 0;
	
	hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);
	
	// Prevent any weird create groups.
	nonhostcontext.fCanGetCreateGroup = FALSE;
	
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "CreateGroup didn't succeed with expected return DPN_OK!");
	else
	{
		xLog(hLog, XLL_INFO, "CreateGroup returned DPN_OK successfully");
		
		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
			xLog(hLog, XLL_FAIL, "Group info structure was modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Group info structure wasn't modified");
			
			if(nonhostcontext.dpnidGroup == 0)
				xLog(hLog, XLL_FAIL, "Didn't get create group message on client!");
			else
				xLog(hLog, XLL_PASS, "Got create group message on client");
		}
	}
	
	xEndVariation(hLog);	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async op to complete"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup waiting for group create to complete on host");

	if(WaitForSingleObject(&hostcontext.hGotGroupMsgEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Create group message wasn't received within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Async callback signalled");
		
		if (hostcontext.dpnidGroup == 0)
			xLog(hLog, XLL_FAIL, "Didn't get create group message");
		else
			xLog(hLog, XLL_PASS, "Got create group message");
	}

	// Prevent any weird create group indications.
	hostcontext.fCanGetCreateGroup = FALSE;
	
	// Reset the context.
	CloseHandle(hostcontext.hGotGroupMsgEvent);
	hostcontext.hGotGroupMsgEvent = NULL;

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Destroying group on client"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup on client");

	dpnhDestroyGroup = (DPNHANDLE) 0x666;
	
	// Expect the destroy group message on the host.
	hostcontext.fCanGetDestroyGroup = TRUE;

	// Create an event to signal when a group is destroyed
	if(!(hostcontext.hGotGroupMsgEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	
	// Expect the destroy group message and async op completion on the client.
	nonhostcontext.fCanGetDestroyGroup = TRUE;
	nonhostcontext.fAsyncOpCanComplete = TRUE;

	// Create an event to signal when a group destruction is complete
	if(!(nonhostcontext.hAsyncOpCompletedEvent = CreateEvent(NULL, FALSE, FALSE, NULL))
	{
		xEndVariation(hLog);

		xLog(hLog, XLL_FAIL, "Couldn't create event for cancel context", hr);
		fSuccess = FALSE;
		goto Exit;
	}
	
	hr = pPeer->DestroyGroup(nonhostcontext.dpnidGroup, NULL, &dpnhDestroyGroup, 0);
	if (hr != (HRESULT) DPNSUCCESS_PENDING)
		xLog(hLog, XLL_FAIL, "DestroyGroup didn't succeed with expected return DPNSUCCESS_PENDING!");
	else
	{
		xLog(hLog, XLL_INFO, "DestroyGroup returned DPNSUCCESS_PENDING successfully");

		if(dpnhDestroyGroup == (DPNHANDLE) 0x666)
			xLog(hLog, XLL_FAIL, "Async op handle wasn't modified!");
		else
		{
			xLog(hLog, XLL_INFO, "Async op handle was modified");

			if (dpnhDestroyGroup == NULL)
				xLog(hLog, XLL_FAIL, "Async op handle was set to NULL!");
			else
				xLog(hLog, XLL_PASS, "Async op handle appears to have been set successfully");
	}

	xEndVariation(hLog);


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for async op to complete on client"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup wait for destroy group to asynchronously complete on client");

	if(WaitForSingleObject(&nonhostcontext.hAsyncOpCompletedEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Delete group didn't complete within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Async callback signalled");
		
		if (! nonhostcontext.fAsyncOpCompleted)
			xLog(hLog, XLL_FAIL, "Async op didn't signal completion!");
		else
		{
			xLog(hLog, XLL_INFO, "Async op signalled completion");
			
			if(nonhostcontext.dpnhCompletedAsyncOp != dpnhDestroyGroup)
				xLog(hLog, XLL_FAIL, "Destroy group completed with different async op handle (%u != %u)",
				hostcontext.dpnhCompletedAsyncOp, dpnhDestroyGroup);
			else
			{
				xLog(hLog, XLL_INFO, "Destroy group completed with correct async op handle");
				
				if (nonhostcontext.dpnidGroup != 0)
					xLog(hLog, XLL_FAIL, "Didn't get destroy group message");
				else
					xLog(hLog, XLL_PASS, "Got destroy group message");
			}	
		}
	}

	// The destroy group message should have arrived by now.
	nonhostcontext.fCanGetDestroyGroup = FALSE;
	
	// Prevent any weird async op completions.
	nonhostcontext.fAsyncOpCanComplete = FALSE;

	// Reset the context.
	nonhostcontext.fAsyncOpCompleted = FALSE;
	CloseHandle(nonhostcontext.hAsyncOpCompletedEvent);
	nonhostcontext.hAsyncOpCompletedEvent = NULL;

	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Waiting for destroy group message on host"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup wait for destroy group message on host");

	if(WaitForSingleObject(&hostcontext.hGotGroupMsgEvent, 60000) != WAIT_OBJECT_0)
		xLog(hLog, XLL_FAIL, "Destroy group message wasn't received within 60 seconds!");
	else
	{
		xLog(hLog, XLL_INFO, "Async callback signalled");
		
		if (hostcontext.dpnidGroup != 0)
			xLog(hLog, XLL_FAIL, "Didn't get destroy group message");
		else
			xLog(hLog, XLL_PASS, "Got destroy group message");
	}

	// Prevent any weird create group indications.
	hostcontext.fCanGetCreateGroup = FALSE;
	
	// Reset the context.
	CloseHandle(hostcontext.hGotGroupMsgEvent);
	hostcontext.hGotGroupMsgEvent = NULL;

	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing peer client object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup closing peer after host-client DestroyGroup tests");
	
	hr = pPeer->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Closing host object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup closing host after host-client DestroyGroup tests");
	
	hr = pPeerHost->Close(0);
	if (hr != DPN_OK)
		xLog(hLog, XLL_FAIL, "Close didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Close returned DPN_OK successfully");
	
	xEndVariation(hLog);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing hosting DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup releasing host");
	
	hr = pPeerHost->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned DPN_OK successfully");
	pPeerHost = NULL;
	
	xEndVariation(hLog);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// "Releasing DirectPlay8Peer object"
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	xStartVariation(hLog, "DirectPlay8Peer: DestroyGroup releasing peer");
	
	hr = pPeer->Release();
	if (hr != S_OK)
		xLog(hLog, XLL_FAIL, "Release didn't succeed with expected return DPN_OK!");
	else
		xLog(hLog, XLL_PASS, "Release returned DPN_OK successfully");
	pPeer = NULL;
	
	xEndVariation(hLog);


Exit:

	// Release the peer interface if we haven't already
	if(pPeer)
	{
		pPeer->Release();
		pPeer = NULL;
	}

	// Release the peer host interface if we haven't already
	if(pPeerHost)
	{
		pPeerHost->Release();
		pPeerHost = NULL;
	}

	// Release the address if we haven't already
	if(pDP8Address)
	{
		pDP8Address->Release();
		pDP8Address = NULL;
	}

	
	if(hostcontext.hGotCreateGroupEvent)
	{
		CloseHandle(hostcontext.hGotCreateGroupEvent);
		hostcontext.hGotCreateGroupEvent = NULL;
	}
	
	if(nonhostcontext.hGotCreateGroupEvent)
	{
		CloseHandle(nonhostcontext.hGotCreateGroupEvent);
		nonhostcontext.hGotCreateGroupEvent = NULL;
	}
	
	if(hostcontext.hAsyncOpCompletedEvent)
	{
		CloseHandle(hostcontext.hAsyncOpCompletedEvent);
		hostcontext.hAsyncOpCompletedEvent = NULL;
	}
	
	if(nonhostcontext.hAsyncOpCompletedEvent)
	{
		CloseHandle(nonhostcontext.hAsyncOpCompletedEvent);
		nonhostcontext.hAsyncOpCompletedEvent = NULL;
	}

	return fSuccess;
}

//==================================================================================
// PeerParamsAddToGroupTests
//----------------------------------------------------------------------------------
//
// Description: Function that executes the test case(s):
//				2.1.1.13 - Peer AddToGroup parameter validation
//
// Arguments:
//	HANDLE	hLog	Handle to the logging subsystem
//
// Returns: TRUE if the test completed, FALSE if some error condition prevents
//          the running of further tests
//==================================================================================
HRESULT PeerParamsAddToGroupTests(PTNEXECCASEDATA pTNecd)
{
	PEERADDTOGROUPCONTEXT hostcontext, nonhostcontext;
	DPN_APPLICATION_DESC dpAppDesc, dpAppDescCompare;
	PDIRECTPLAY8ADDRESS	pDP8Address;
	PDIRECTPLAY8PEER pPeer, pPeerHost;
	DPN_GROUP_INFO dpGroupInfo, dpGroupInfoCompare;
	DPNHANDLE dpnhAddPlayer;
	HRESULT	hr;
	LPVOID pVoice;
	BOOL fSuccess = TRUE;

	ZeroMemory(&hostcontext, sizeof (PARMVPADDTOGROUPCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPADDTOGROUPCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding player 0 to group 0 (DPNID_ALL_PLAYERS_GROUP) with NULLs and 0s using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_AddPlayerToGroup(pPeer->m_pDP8Peer, 0,  0, NULL,
												NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Adding player 0 to group 0 (DPNID_ALL_PLAYERS_GROUP) with NULLs and 0s using C++ macro didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding player 0 to group 0 (DPNID_ALL_PLAYERS_GROUP) with NULLs and 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->AddPlayerToGroup(0,  0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Adding player 0 to group 0 (DPNID_ALL_PLAYERS_GROUP) with NULLs and 0 flags didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding player to group with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhAddPlayer = (DPNHANDLE) 0x666;

		// Use 0x666 to avoid DPlay's group ID == 0 and player ID == 0 checks.
		hr = pPeer->AddPlayerToGroup((DPNID) 0x666, (DPNID) 0x666, NULL,
											&dpnhAddPlayer, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Adding player to group with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		if (dpnhAddPlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhAddPlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding player to group before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->AddPlayerToGroup((DPNID) 0x666, (DPNID) 0x666, NULL,
											&dpnhAddPlayer, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Adding player to group before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		if (dpnhAddPlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhAddPlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPAddToGroupDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding player to group without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->AddPlayerToGroup((DPNID) 0x666, (DPNID) 0x666, NULL,
											&dpnhAddPlayer, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Adding player to group without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		if (dpnhAddPlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhAddPlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to invalid group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->AddPlayerToGroup((DPNID) 0x666, (DPNID) 0x00200002, NULL,
											&dpnhAddPlayer, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Adding self player to invalid group didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		if (dpnhAddPlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhAddPlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to all players group internal ID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Determine real ID")
		hr = pPeer->AddPlayerToGroup((DPNID) 0x00100001, (DPNID) 0x00200002,
											NULL, &dpnhAddPlayer, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Adding self player to all players group internal ID didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		if (dpnhAddPlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhAddPlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
		dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
		//dpGroupInfo.dwInfoFlags = 0;
		//dpGroupInfo.pwszName = NULL;
		//dpGroupInfo.pvData = NULL;
		//dpGroupInfo.dwDataSize = 0;
		//dpGroupInfo.dwGroupFlags = 0;

		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));


		// Expect the create group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding invalid player to valid group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Determine real ID")
		hr = pPeer->AddPlayerToGroup(hostcontext.dpnidGroup, (DPNID) 0x666,
											NULL, &dpnhAddPlayer, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Adding invalid player to valid group didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		if (dpnhAddPlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhAddPlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message and async op completion.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		hostcontext.fCanGetAddToGroup = TRUE;
		CREATEEVENT_OR_THROW(hostcontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fAsyncOpCanComplete = TRUE;

		hr = pPeer->AddPlayerToGroup(hostcontext.dpnidGroup,
											hostcontext.dpnidExpectedPlayer,
											NULL,
											&dpnhAddPlayer,
											0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Adding self player to group didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		if (dpnhAddPlayer == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhAddPlayer == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hAsyncOpCompletedEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// The add player to group message should have arrived by now.
		hostcontext.fCanGetAddToGroup = FALSE;

		// Prevent any weird async op completions.
		hostcontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fAsyncOpCompleted)
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (hostcontext.dpnhCompletedAsyncOp != dpnhAddPlayer)
		{
			DPL(0, "Add player completed with different async op handle (%x != %x)!",
				2, hostcontext.dpnhCompletedAsyncOp, dpnhAddPlayer);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		hostcontext.fAsyncOpCompleted = FALSE;
		CloseHandle(hostcontext.hAsyncOpCompletedEvent);
		hostcontext.hAsyncOpCompletedEvent = NULL;


		// Make sure we got the add to group message.
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		hostcontext.fGotAddToGroup = FALSE;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to group again"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhAddPlayer = (DPNHANDLE) 0x666;

		hr = pPeer->AddPlayerToGroup(hostcontext.dpnidGroup,
											hostcontext.dpnidExpectedPlayer,
											NULL,
											&dpnhAddPlayer,
											0);
		if (hr != DPNERR_PLAYERALREADYINGROUP)
		{
			DPL(0, "Adding self player to group again didn't return expected error PLAYERALREADYINGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy group)

		if (dpnhAddPlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhAddPlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing self player from group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->RemovePlayerFromGroup(hostcontext.dpnidGroup,
												hostcontext.dpnidExpectedPlayer,
												NULL,
												NULL,
												DPNREMOVEPLAYERFROMGROUP_SYNC);
		if (hr != DPN_OK)
		{
			DPL(0, "Removing self player from group failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy group)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		hostcontext.fCanGetAddToGroup = TRUE;

		hr = pPeer->AddPlayerToGroup(hostcontext.dpnidGroup,
											hostcontext.dpnidExpectedPlayer,
											NULL,
											NULL,
											dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird add player to group messages.
		hostcontext.fCanGetAddToGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Adding self player to group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		// Make sure we got the add to group message.
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		hostcontext.fGotAddToGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")

		// Reset group ID.
		hostcontext.dpnidGroup = 0;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPAddToGroupDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPAddToGroupDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group on client synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
		dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
		//dpGroupInfo.dwInfoFlags = 0;
		//dpGroupInfo.pwszName = NULL;
		//dpGroupInfo.pvData = NULL;
		//dpGroupInfo.dwDataSize = 0;
		//dpGroupInfo.dwGroupFlags = 0;

		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));


		// Expect the create group message on the host.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fCanGetCreateGroup = TRUE;

		// Expect the create group message on the client.
		nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
		nonhostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		nonhostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (nonhostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for group to be created on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create group indications.
		hostcontext.fCanGetCreateGroup = FALSE;

#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get create group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to group on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message on the host.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00300003;
		hostcontext.fCanGetAddToGroup = TRUE;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);

		// Expect the add player to group message and async op completion on the client.
		nonhostcontext.dpnidExpectedPlayer = (DPNID) 0x00300003;
		nonhostcontext.fCanGetAddToGroup = TRUE;
		CREATEEVENT_OR_THROW(nonhostcontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.fAsyncOpCanComplete = TRUE;

		hr = pPeer->AddPlayerToGroup(nonhostcontext.dpnidGroup,
											nonhostcontext.dpnidExpectedPlayer,
											NULL,
											&dpnhAddPlayer,
											0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Adding self player to group on client didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		if (dpnhAddPlayer == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhAddPlayer == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hAsyncOpCompletedEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// The add player to group message should have arrived by now.
		nonhostcontext.fCanGetAddToGroup = FALSE;

		// Prevent any weird async op completions.
		nonhostcontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! nonhostcontext.fAsyncOpCompleted)
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (nonhostcontext.dpnhCompletedAsyncOp != dpnhAddPlayer)
		{
			DPL(0, "Add player completed with different async op handle (%x != %x)!",
				2, nonhostcontext.dpnhCompletedAsyncOp, dpnhAddPlayer);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		nonhostcontext.fAsyncOpCompleted = FALSE;
		CloseHandle(nonhostcontext.hAsyncOpCompletedEvent);
		nonhostcontext.hAsyncOpCompletedEvent = NULL;


		// Make sure we got the add to group message.
		if (! nonhostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		nonhostcontext.fGotAddToGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for add player to group to be indicated on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird add to group indications.
		hostcontext.fCanGetAddToGroup = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add to group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get add to group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;
		hostcontext.fGotAddToGroup = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding host player to group synchronously on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message on the host and client.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fCanGetAddToGroup = TRUE;

		nonhostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		nonhostcontext.fCanGetAddToGroup = TRUE;

		hr = pPeer->AddPlayerToGroup(nonhostcontext.dpnidGroup,
											nonhostcontext.dpnidExpectedPlayer,
											NULL,
											NULL,
											dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird add player to group messages.
		nonhostcontext.fCanGetAddToGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Adding host player to group synchronously on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		// Make sure we got the add to group message.
		if (! nonhostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		nonhostcontext.fGotAddToGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for add player to group to be indicated on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird add to group indications.
		hostcontext.fCanGetAddToGroup = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add to group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get add to group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;
		hostcontext.fGotAddToGroup = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hGotGroupMsgEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hGotGroupMsgEvent);
	SAFE_CLOSEHANDLE(hostcontext.hAsyncOpCompletedEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hAsyncOpCompletedEvent);


	return (hr);
} // ParmVPeerExec_AddToGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_RemoveFromGroup()"
//==================================================================================
// ParmVPeerExec_RemoveFromGroup
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.14 - Peer RemoveFromGroup parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_RemoveFromGroup(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					hr;
	CTNTestResult					hr;
	PWRAPDP8PEER					pDP8Peer = NULL;
	PWRAPDP8PEER					pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS				pDP8Address = NULL;
	PARMVPREMOVEFROMGROUPCONTEXT	hostcontext;
	PARMVPREMOVEFROMGROUPCONTEXT	nonhostcontext;
	DPN_APPLICATION_DESC			dpAppDesc;
	DPN_APPLICATION_DESC			dpAppDescCompare;
	DPN_GROUP_INFO					dpGroupInfo;
	DPN_GROUP_INFO					dpGroupInfoCompare;
	DPNHANDLE						dpnhRemovePlayer;



	ZeroMemory(&hostcontext, sizeof (PARMVPREMOVEFROMGROUPCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPREMOVEFROMGROUPCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing player 0 from group 0 (DPNID_ALL_PLAYERS_GROUP) with NULLs and 0s using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_RemovePlayerFromGroup(pPeer->m_pDP8Peer, 0,  0,
													NULL, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Removing player 0 from group 0 (DPNID_ALL_PLAYERS_GROUP) with NULLs and 0s using C++ macro didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing player 0 from group (DPNID_ALL_PLAYERS_GROUP) 0 with NULLs and 0s"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->RemovePlayerFromGroup(0, 0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Removing player 0 from group 0 (DPNID_ALL_PLAYERS_GROUP) with NULLs and 0s didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing player from group with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhRemovePlayer = (DPNHANDLE) 0x666;

		// Use 0x666 to avoid DPlay's group ID == 0 and player ID == 0 checks.
		hr = pPeer->RemovePlayerFromGroup((DPNID) 0x666, (DPNID) 0x666, NULL,
												&dpnhRemovePlayer, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Removing player from group with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)

		if (dpnhRemovePlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhRemovePlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing player from group before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->RemovePlayerFromGroup((DPNID) 0x666, (DPNID) 0x666, NULL,
												&dpnhRemovePlayer, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Removing player from group before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)

		if (dpnhRemovePlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhRemovePlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPRemoveFromGroupDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing player from group without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->RemovePlayerFromGroup((DPNID) 0x666, (DPNID) 0x666, NULL,
												&dpnhRemovePlayer, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Removing player from group without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)

		if (dpnhRemovePlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhRemovePlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing self player from invalid group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->RemovePlayerFromGroup((DPNID) 0x666, (DPNID) 0x00200002,
												NULL,
												&dpnhRemovePlayer, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Removing self player from invalid group didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)

		if (dpnhRemovePlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhRemovePlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing self player from all players group internal ID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Determine real ID")
		hr = pPeer->RemovePlayerFromGroup((DPNID) 0x00100001, (DPNID) 0x00200002,
												NULL,
												&dpnhRemovePlayer, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Removing self player from all players group internal ID didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)

		if (dpnhRemovePlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhRemovePlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
		dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
		//dpGroupInfo.dwInfoFlags = 0;
		//dpGroupInfo.pwszName = NULL;
		//dpGroupInfo.pvData = NULL;
		//dpGroupInfo.dwDataSize = 0;
		//dpGroupInfo.dwGroupFlags = 0;

		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));


		// Expect the create group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing invalid player from group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->RemovePlayerFromGroup(hostcontext.dpnidGroup,
												(DPNID) 0x666, NULL,
												&dpnhRemovePlayer, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Removing invalid player from group didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)

		if (dpnhRemovePlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhRemovePlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing player not in group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->RemovePlayerFromGroup(hostcontext.dpnidGroup,
												(DPNID) 0x00200002, NULL,
												&dpnhRemovePlayer, 0);
		if (hr != DPNERR_PLAYERNOTINGROUP)
		{
			DPL(0, "Removing player not in group didn't return expected error PLAYERNOTINGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)

		if (dpnhRemovePlayer != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhRemovePlayer);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		hostcontext.fCanGetAddToGroup = TRUE;

		hr = pPeer->AddPlayerToGroup(hostcontext.dpnidGroup,
											hostcontext.dpnidExpectedPlayer,
											NULL,
											NULL,
											dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird add player to group messages.
		hostcontext.fCanGetAddToGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Adding self player to group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		// Make sure we got the add to group message.
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		hostcontext.fGotAddToGroup = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing self player from group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the remove player from group message and async op completion.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		hostcontext.fCanGetRemoveFromGroup = TRUE;
		CREATEEVENT_OR_THROW(hostcontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fAsyncOpCanComplete = TRUE;

		hr = pPeer->RemovePlayerFromGroup(hostcontext.dpnidGroup,
												hostcontext.dpnidExpectedPlayer,
												NULL,
												&dpnhRemovePlayer,
												0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Removing self player from group didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't emove player from group)

		if (dpnhRemovePlayer == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhRemovePlayer == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hAsyncOpCompletedEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// The remove player from group message should have arrived by now.
		hostcontext.fCanGetRemoveFromGroup = FALSE;

		// Prevent any weird async op completions.
		hostcontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fAsyncOpCompleted)
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (hostcontext.dpnhCompletedAsyncOp != dpnhRemovePlayer)
		{
			DPL(0, "Remove player completed with different async op handle (%x != %x)!",
				2, hostcontext.dpnhCompletedAsyncOp, dpnhRemovePlayer);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		hostcontext.fAsyncOpCompleted = FALSE;
		CloseHandle(hostcontext.hAsyncOpCompletedEvent);
		hostcontext.hAsyncOpCompletedEvent = NULL;


		// Make sure we got the remove from group message.
		if (! hostcontext.fGotRemoveFromGroup)
		{
			DPL(0, "Didn't get remove player from group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get remove player from group)

		// Reset the context.
		hostcontext.fGotRemoveFromGroup = FALSE;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		hostcontext.fCanGetAddToGroup = TRUE;

		hr = pPeer->AddPlayerToGroup(hostcontext.dpnidGroup,
											hostcontext.dpnidExpectedPlayer,
											NULL,
											NULL,
											dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird add player to group messages.
		hostcontext.fCanGetAddToGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Adding self player to group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		// Make sure we got the add to group message.
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		hostcontext.fGotAddToGroup = FALSE;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing self player from group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the remove player from group message.
		hostcontext.fCanGetRemoveFromGroup = TRUE;

		hr = pPeer->RemovePlayerFromGroup(hostcontext.dpnidGroup,
												hostcontext.dpnidExpectedPlayer,
												NULL,
												NULL,
												dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird remove player from group messages.
		hostcontext.fCanGetRemoveFromGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Removing self player from group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove from group)

		// Make sure we got the remove from group message.
		if (! hostcontext.fGotRemoveFromGroup)
		{
			DPL(0, "Didn't get remove player from group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get remove player from group)

		// Reset the context.
		hostcontext.fGotRemoveFromGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")

		// Reset group ID.
		hostcontext.dpnidGroup = 0;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPRemoveFromGroupDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPRemoveFromGroupDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group on client synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
		dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
		//dpGroupInfo.dwInfoFlags = 0;
		//dpGroupInfo.pwszName = NULL;
		//dpGroupInfo.pvData = NULL;
		//dpGroupInfo.dwDataSize = 0;
		//dpGroupInfo.dwGroupFlags = 0;

		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));


		// Expect the create group message on the host.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fCanGetCreateGroup = TRUE;

		// Expect the create group message on the client.
		nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
		nonhostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		nonhostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (nonhostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for group to be created on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create group indications.
		hostcontext.fCanGetCreateGroup = FALSE;

#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get create group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to group synchronously on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message on the host and client.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00300003;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fCanGetAddToGroup = TRUE;

		nonhostcontext.dpnidExpectedPlayer = (DPNID) 0x00300003;
		nonhostcontext.fCanGetAddToGroup = TRUE;

		hr = pPeer->AddPlayerToGroup(nonhostcontext.dpnidGroup,
											nonhostcontext.dpnidExpectedPlayer,
											NULL,
											NULL,
											dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird add player to group messages.
		nonhostcontext.fCanGetAddToGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Adding self player to group synchronously on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		// Make sure we got the add to group message.
		if (! nonhostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		nonhostcontext.fGotAddToGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for add player to group to be indicated on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird add to group indications.
		hostcontext.fCanGetAddToGroup = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add to group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get add to group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;
		hostcontext.fGotAddToGroup = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing self player from group on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhRemovePlayer = (DPNHANDLE) 0x666;

		// Expect the remove player from group message on the host.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00300003;
		hostcontext.fCanGetRemoveFromGroup = TRUE;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);

		// Expect the remove player from group message and async op completion on the client.
		nonhostcontext.dpnidExpectedPlayer = (DPNID) 0x00300003;
		nonhostcontext.fCanGetRemoveFromGroup = TRUE;
		CREATEEVENT_OR_THROW(nonhostcontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.fAsyncOpCanComplete = TRUE;

		hr = pPeer->RemovePlayerFromGroup(nonhostcontext.dpnidGroup,
													nonhostcontext.dpnidExpectedPlayer,
													NULL,
													&dpnhRemovePlayer,
													0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Removing self player from group on client didn't return expected PENDING sucess code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)

		if (dpnhRemovePlayer == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhRemovePlayer == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hAsyncOpCompletedEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// The add player to group message should have arrived by now.
		nonhostcontext.fCanGetAddToGroup = FALSE;

		// Prevent any weird async op completions.
		nonhostcontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! nonhostcontext.fAsyncOpCompleted)
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (nonhostcontext.dpnhCompletedAsyncOp != dpnhRemovePlayer)
		{
			DPL(0, "Remove player completed with different async op handle (%x != %x)!",
				2, nonhostcontext.dpnhCompletedAsyncOp, dpnhRemovePlayer);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		nonhostcontext.fAsyncOpCompleted = FALSE;
		CloseHandle(nonhostcontext.hAsyncOpCompletedEvent);
		nonhostcontext.hAsyncOpCompletedEvent = NULL;


		// Make sure we got the remove from group message.
		if (! nonhostcontext.fGotRemoveFromGroup)
		{
			DPL(0, "Didn't get remove player from group message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get remove player from group)

		// Reset the context.
		nonhostcontext.fGotRemoveFromGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for remove player from group to be indicated on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird remove from group indications.
		hostcontext.fCanGetRemoveFromGroup = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fGotRemoveFromGroup)
		{
			DPL(0, "Didn't get remove from group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get remove from group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;
		hostcontext.fGotRemoveFromGroup = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding host player to group synchronously on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message on the host and client.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fCanGetAddToGroup = TRUE;

		nonhostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		nonhostcontext.fCanGetAddToGroup = TRUE;

		hr = pPeer->AddPlayerToGroup(nonhostcontext.dpnidGroup,
											nonhostcontext.dpnidExpectedPlayer,
											NULL,
											NULL,
											dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird add player to group messages.
		nonhostcontext.fCanGetAddToGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Adding host player to group synchronously on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		// Make sure we got the add to group message.
		if (! nonhostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		nonhostcontext.fGotAddToGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for add player to group to be indicated on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird add to group indications.
		hostcontext.fCanGetAddToGroup = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add to group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get add to group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;
		hostcontext.fGotAddToGroup = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Removing host player from group synchronously on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the remove player from group message on the host and client.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fCanGetRemoveFromGroup = TRUE;

		nonhostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		nonhostcontext.fCanGetRemoveFromGroup = TRUE;

		hr = pPeer->RemovePlayerFromGroup(nonhostcontext.dpnidGroup,
												nonhostcontext.dpnidExpectedPlayer,
												NULL,
												NULL,
												dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird remove player from group messages.
		nonhostcontext.fCanGetRemoveFromGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Removing host player from group synchronously on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't remove player from group)

		// Make sure we got the remove from group message.
		if (! nonhostcontext.fGotRemoveFromGroup)
		{
			DPL(0, "Didn't get remove player from group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get remove player from group)

		// Reset the context.
		nonhostcontext.fGotRemoveFromGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for remove player from group to be indicated on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird remove from group indications.
		hostcontext.fCanGetRemoveFromGroup = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fGotRemoveFromGroup)
		{
			DPL(0, "Didn't get remove from group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get remove from group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;
		hostcontext.fGotRemoveFromGroup = FALSE;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hGotGroupMsgEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hGotGroupMsgEvent);
	SAFE_CLOSEHANDLE(hostcontext.hAsyncOpCompletedEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hAsyncOpCompletedEvent);


	return (hr);
} // ParmVPeerExec_RemoveFromGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_SetGroupInfo()"
//==================================================================================
// ParmVPeerExec_SetGroupInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.15 - Peer SetGroupInfo parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_SetGroupInfo(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult				hr;
	CTNTestResult				hr;
	PWRAPDP8PEER				pDP8Peer = NULL;
	PWRAPDP8PEER				pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	PARMVPSETGROUPINFOCONTEXT	hostcontext;
	PARMVPSETGROUPINFOCONTEXT	nonhostcontext;
	DPN_APPLICATION_DESC		dpAppDesc;
	DPN_APPLICATION_DESC		dpAppDescCompare;
	DPN_GROUP_INFO				dpGroupInfo;
	DPN_GROUP_INFO				dpGroupInfoCompare;
	DPNHANDLE					dpnhSetGroupInfo;



	ZeroMemory(&hostcontext, sizeof (PARMVPSETGROUPINFOCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPSETGROUPINFOCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group 0 (DPNID_ALL_PLAYERS_GROUP) info with all NULLs and 0s using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_SetGroupInfo(pPeer->m_pDP8Peer, 0, NULL, NULL,
											NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Setting group 0 (DPNID_ALL_PLAYERS_GROUP) info with all NULLs and 0s using C++ macro didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group 0 (DPNID_ALL_PLAYERS_GROUP) info with all NULLs and 0s"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetGroupInfo(0, NULL, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Setting group 0 (DPNID_ALL_PLAYERS_GROUP) info with all NULLs and 0s didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group info with NULL info"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhSetGroupInfo = (DPNHANDLE) 0x666;

		// Use 0x666 to avoid DPlay's group ID == 0 check.
		hr = pPeer->SetGroupInfo((DPNID) 0x666, NULL, NULL,
										&dpnhSetGroupInfo, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Setting group info with NULL info didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)

		if (dpnhSetGroupInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group info with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
		dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
		//dpGroupInfo.dwInfoFlags = 0;
		//dpGroupInfo.pwszName = NULL;
		//dpGroupInfo.pvData = NULL;
		//dpGroupInfo.dwDataSize = 0;
		//dpGroupInfo.dwGroupFlags = 0;

		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));

		hr = pPeer->SetGroupInfo((DPNID) 0x666, &dpGroupInfo, NULL,
										&dpnhSetGroupInfo, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Setting group info with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		if (dpnhSetGroupInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group info before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetGroupInfo((DPNID) 0x666, &dpGroupInfo, NULL,
										&dpnhSetGroupInfo, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Setting group info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		if (dpnhSetGroupInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPSetGroupInfoDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group info without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetGroupInfo((DPNID) 0x666, &dpGroupInfo, NULL,
										&dpnhSetGroupInfo, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Setting group info without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		if (dpnhSetGroupInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group info for invalid group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetGroupInfo((DPNID) 0x666, &dpGroupInfo, NULL,
										&dpnhSetGroupInfo, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Setting group info for invalid group didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		if (dpnhSetGroupInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group info of all players group internal ID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Determine real ID")
		hr = pPeer->SetGroupInfo((DPNID) 0x00100001, &dpGroupInfo, NULL,
										&dpnhSetGroupInfo, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Setting group info of all players group internal ID didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		if (dpnhSetGroupInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the create group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group info"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the group info message and async op completion.
		hostcontext.fCanGetGroupInfo = TRUE;
		CREATEEVENT_OR_THROW(hostcontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fAsyncOpCanComplete = TRUE;

		hr = pPeer->SetGroupInfo(hostcontext.dpnidGroup, &dpGroupInfo, NULL,
										&dpnhSetGroupInfo, 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Setting group info didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		if (dpnhSetGroupInfo == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhSetGroupInfo == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hAsyncOpCompletedEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// The remove player from group message should have arrived by now.
		hostcontext.fCanGetGroupInfo = FALSE;

		// Prevent any weird async op completions.
		hostcontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fAsyncOpCompleted)
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (hostcontext.dpnhCompletedAsyncOp != dpnhSetGroupInfo)
		{
			DPL(0, "Set group info completed with different async op handle (%x != %x)!",
				2, hostcontext.dpnhCompletedAsyncOp, dpnhSetGroupInfo);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)


		// Reset the context.
		hostcontext.fAsyncOpCompleted = FALSE;
		CloseHandle(hostcontext.hAsyncOpCompletedEvent);
		hostcontext.hAsyncOpCompletedEvent = NULL;


		// Make sure we got the group info message.
		if (! hostcontext.fGotGroupInfo)
		{
			DPL(0, "Didn't get group info message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get group info)

		// Reset the context.
		hostcontext.fGotGroupInfo = FALSE;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group info synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message.
		hostcontext.fCanGetGroupInfo = TRUE;

		hr = pPeer->SetGroupInfo(hostcontext.dpnidGroup, &dpGroupInfo, NULL,
										NULL, dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird group info messages.
		hostcontext.fCanGetGroupInfo = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Setting group info synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)

		// Make sure we got the group info message.
		if (! hostcontext.fGotGroupInfo)
		{
			DPL(0, "Didn't get group info message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get group info)

		// Reset the context.
		hostcontext.fGotGroupInfo= FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")

		// Reset group ID.
		hostcontext.dpnidGroup = 0;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPSetGroupInfoDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPSetGroupInfoDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group on client synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the create group message on the host.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fCanGetCreateGroup = TRUE;

		// Expect the create group message on the client.
		nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00300003;
		nonhostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		nonhostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (nonhostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for group to be created on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create group indications.
		hostcontext.fCanGetCreateGroup = FALSE;

#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get create group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting group info on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhSetGroupInfo = (DPNHANDLE) 0x666;

		// Expect the group info message on the host.
		hostcontext.fCanGetGroupInfo = TRUE;
		CREATEEVENT_OR_THROW(hostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);

		// Expect the group info message and async op completion on the client.
		nonhostcontext.fCanGetGroupInfo = TRUE;
		CREATEEVENT_OR_THROW(nonhostcontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.fAsyncOpCanComplete = TRUE;

		hr = pPeer->SetGroupInfo(nonhostcontext.dpnidGroup, &dpGroupInfo, NULL,
										&dpnhSetGroupInfo, 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Setting group info on client didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)

		if (dpnhSetGroupInfo == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhSetGroupInfo == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hAsyncOpCompletedEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// The group info message should have arrived by now.
		nonhostcontext.fCanGetGroupInfo = FALSE;

		// Prevent any weird async op completions.
		nonhostcontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! nonhostcontext.fAsyncOpCompleted)
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (nonhostcontext.dpnhCompletedAsyncOp != dpnhSetGroupInfo)
		{
			DPL(0, "Set group info completed with different async op handle (%x != %x)!",
				2, nonhostcontext.dpnhCompletedAsyncOp, dpnhSetGroupInfo);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		nonhostcontext.fAsyncOpCompleted = FALSE;
		CloseHandle(nonhostcontext.hAsyncOpCompletedEvent);
		nonhostcontext.hAsyncOpCompletedEvent = NULL;


		// Make sure we got the group info message.
		if (! nonhostcontext.fGotGroupInfo)
		{
			DPL(0, "Didn't get group info message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get group info)

		// Reset the context.
		nonhostcontext.fGotGroupInfo = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for group info to be indicated on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird group info indications.
		hostcontext.fCanGetGroupInfo = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fGotGroupInfo)
		{
			DPL(0, "Didn't get group info indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get group info indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotGroupMsgEvent);
		hostcontext.hGotGroupMsgEvent = NULL;
		hostcontext.fGotGroupInfo = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hAsyncOpCompletedEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hAsyncOpCompletedEvent);


	return (hr);
} // ParmVPeerExec_SetGroupInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_GetGroupInfo()"
//==================================================================================
// ParmVPeerExec_GetGroupInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.16 - Peer GetGroupInfo parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_GetGroupInfo(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult				hr;
	CTNTestResult				hr;
	PWRAPDP8PEER				pDP8Peer = NULL;
	PWRAPDP8PEER				pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	PARMVPGETGROUPINFOCONTEXT	hostcontext;
	PARMVPGETGROUPINFOCONTEXT	nonhostcontext;
	DPN_APPLICATION_DESC		dpAppDesc;
	DPN_APPLICATION_DESC		dpAppDescCompare;
	DPN_GROUP_INFO				dpGroupInfo;
	DPN_GROUP_INFO				dpGroupInfoCompare;
	PDPN_GROUP_INFO				pdpGroupInfo = NULL;
	DWORD						dwSize;
	DWORD						dwExpectedSize;



	ZeroMemory(&hostcontext, sizeof (PARMVPGETGROUPINFOCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPGETGROUPINFOCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group 0 (DPNID_ALL_PLAYERS_GROUP) info with all NULLs and 0s using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_GetGroupInfo(pPeer->m_pDP8Peer, 0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Getting group 0 (DPNID_ALL_PLAYERS_GROUP) info with all NULLs and 0s using C++ macro didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group 0 (DPNID_ALL_PLAYERS_GROUP) info with all NULLs and 0s"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetGroupInfo(0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Getting group 0 (DPNID_ALL_PLAYERS_GROUP) info with all NULLs and 0s didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set group info)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info with NULL size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Use 0x666 to avoid DPlay's group ID == 0 check.
		hr = pPeer->GetGroupInfo((DPNID) 0x666, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting group info with NULL size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info with NULL buffer and non-zero size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSize = 666;

		hr = pPeer->GetGroupInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting group info with NULL buffer and non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != 666)
		{
			DPL(0, "Size was changed (%u != 666)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSize = 0;

		hr = pPeer->GetGroupInfo((DPNID) 0x666, NULL, &dwSize, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Getting group info with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != 0)
		{
			DPL(0, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetGroupInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Getting group info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != 0)
		{
			DPL(0, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPGetGroupInfoDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetGroupInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Getting group info without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != 0)
		{
			DPL(0, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info for invalid group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetGroupInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Getting group info for invalid group didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != 0)
		{
			DPL(0, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info of all players group internal ID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Determine real ID")
		hr = pPeer->GetGroupInfo((DPNID) 0x00100001, NULL, &dwSize, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Getting group info of all players group internal ID didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != 0)
		{
			DPL(0, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
		dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
		//dpGroupInfo.dwInfoFlags = 0;
		//dpGroupInfo.pwszName = NULL;
		//dpGroupInfo.pvData = NULL;
		//dpGroupInfo.dwDataSize = 0;
		//dpGroupInfo.dwGroupFlags = 0;

		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));


		// Expect the create group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info with NULL buffer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwExpectedSize = sizeof (DPN_GROUP_INFO);

		hr = pPeer->GetGroupInfo(hostcontext.dpnidGroup, NULL, &dwSize, 0);
		if (hr != DPNERR_BUFFERTOOSMALL)
		{
			DPL(0, "Getting group info with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size was not expected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info with group info size of 0"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PDPN_GROUP_INFO, pdpGroupInfo, dwSize + BUFFERPADDING_SIZE);

		FillWithDWord((((PBYTE) pdpGroupInfo) + sizeof (DWORD)),
						(dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE),
						DONT_TOUCH_MEMORY_PATTERN);

		pdpGroupInfo->dwSize = 0;


		hr = pPeer->GetGroupInfo(hostcontext.dpnidGroup, pdpGroupInfo, &dwSize, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting group info using buffer with group info size of 0 didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)

		// Make sure the buffer was not modified or overrun.
		if ((pdpGroupInfo->dwSize != 0) ||
			(! IsFilledWithDWord((((PBYTE) pdpGroupInfo) + sizeof (DWORD)), (dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE), DONT_TOUCH_MEMORY_PATTERN)))
		{
			DPL(0, "The %u byte group info buffer (at %x) was modified or overrun!",
				2, dwSize, pdpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info with group info size too large"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pdpGroupInfo->dwSize = sizeof (DPN_GROUP_INFO) + 1;

		hr = pPeer->GetGroupInfo(hostcontext.dpnidGroup, pdpGroupInfo, &dwSize, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting group info using buffer with group info size too large didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)

		// Make sure the buffer was not modified or overrun.
		if ((pdpGroupInfo->dwSize != (sizeof (DPN_GROUP_INFO) + 1)) ||
			(! IsFilledWithDWord((((PBYTE) pdpGroupInfo) + sizeof (DWORD)), (dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE), DONT_TOUCH_MEMORY_PATTERN)))
		{
			DPL(0, "The %u byte group info buffer (at %x) was modified or overrun!",
				2, dwSize, pdpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pdpGroupInfo->dwSize = sizeof (DPN_GROUP_INFO);

		hr = pPeer->GetGroupInfo(hostcontext.dpnidGroup, pdpGroupInfo, &dwSize, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting group info failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)


		// Make sure the structure returned was expected.

		ZeroMemory(&dpGroupInfoCompare, sizeof (DPN_GROUP_INFO));
		dpGroupInfoCompare.dwSize = sizeof (DPN_GROUP_INFO);
		dpGroupInfoCompare.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
		//dpGroupInfoCompare.pwszName = NULL;
		//dpGroupInfoCompare.pvData = NULL;
		//dpGroupInfoCompare.dwDataSize = 0;
		//dpGroupInfoCompare.dwGroupFlags = 0;

		hr = ParmVCompareGroupInfo(pdpGroupInfo, &dpGroupInfoCompare);
		if (hr != DPN_OK)
		{
			DPL(0, "Comparing group info buffer %x with expected %x failed!",
				2, pdpGroupInfo, &dpGroupInfo);
			THROW_TESTRESULT;
		} // end if (failed comparison)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")

		// Reset group ID.
		hostcontext.dpnidGroup = 0;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPGetGroupInfoDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPGetGroupInfoDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group on host synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Restore the comparison buffer.
		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));

		// Expect the create group message on the host.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		// Expect the create group message on the client.
		nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		CREATEEVENT_OR_THROW(nonhostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeerHost->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group on host synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for group to be created on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create group indications.
		nonhostcontext.fCanGetCreateGroup = FALSE;

#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (nonhostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get create group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(nonhostcontext.hGotGroupMsgEvent);
		nonhostcontext.hGotGroupMsgEvent = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info with NULL buffer on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSize = 0;

		hr = pPeer->GetGroupInfo(nonhostcontext.dpnidGroup, NULL, &dwSize, 0);
		if (hr != DPNERR_BUFFERTOOSMALL)
		{
			DPL(0, "Getting group info with NULL buffer on client didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size was not expected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group info on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		FillWithDWord((((PBYTE) pdpGroupInfo) + sizeof (DWORD)),
						(dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE),
						DONT_TOUCH_MEMORY_PATTERN);

		pdpGroupInfo->dwSize = sizeof (DPN_GROUP_INFO);

		hr = pPeer->GetGroupInfo(nonhostcontext.dpnidGroup, pdpGroupInfo, &dwSize, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting group info on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)


		// Make sure the structure returned was expected.

		ZeroMemory(&dpGroupInfoCompare, sizeof (DPN_GROUP_INFO));
		dpGroupInfoCompare.dwSize = sizeof (DPN_GROUP_INFO);
		dpGroupInfoCompare.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
		//dpGroupInfoCompare.pwszName = NULL;
		//dpGroupInfoCompare.pvData = NULL;
		//dpGroupInfoCompare.dwDataSize = 0;
		//dpGroupInfoCompare.dwGroupFlags = 0;

		hr = ParmVCompareGroupInfo(pdpGroupInfo, &dpGroupInfoCompare);
		if (hr != DPN_OK)
		{
			DPL(0, "Comparing group info buffer %x with expected %x failed!",
				2, pdpGroupInfo, &dpGroupInfo);
			THROW_TESTRESULT;
		} // end if (failed comparison)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_LOCALFREE(pdpGroupInfo);
	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hGotGroupMsgEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hGotGroupMsgEvent);


	return (hr);
} // ParmVPeerExec_GetGroupInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_EnumPAndG()"
//==================================================================================
// ParmVPeerExec_EnumCAndG
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.17 - Peer EnumPlayersAndGroups parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_EnumPAndG(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			hr;
	CTNTestResult			hr;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PWRAPDP8PEER			pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS		pDP8Address = NULL;
	PARMVPENUMPANDGCONTEXT	hostcontext;
	PARMVPENUMPANDGCONTEXT	nonhostcontext;
	DPN_APPLICATION_DESC	dpAppDesc;
	DPN_APPLICATION_DESC	dpAppDescCompare;
	DPN_GROUP_INFO			dpGroupInfo;
	DPN_GROUP_INFO			dpGroupInfoCompare;
	DPNID					adpnidIDs[4];
	DWORD					dwNumIDs;



	ZeroMemory(&hostcontext, sizeof (PARMVPENUMPANDGCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPENUMPANDGCONTEXT));


	BEGIN_TESTCASE
	{
		adpnidIDs[0] = 0x666;
		adpnidIDs[1] = 0x666;
		adpnidIDs[2] = 0x666;
		adpnidIDs[3] = 0x666;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "EnumP&G-ing with NULLs and 0 flags using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_EnumPlayersAndGroups(pPeer->m_pDP8Peer, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "EnumP&G-ing with NULLs and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "EnumP&G-ing with NULLs and 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumPlayersAndGroups(NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "EnumP&G-ing with NULLs and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "EnumP&G-ing with non-0 count"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 666;

		hr = pPeer->EnumPlayersAndGroups(NULL, &dwNumIDs, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "EnumP&G-ing with 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 666)
		{
			DPL(0, "Number of IDs was modified (%u != 666)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "EnumP&G-ing with 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 0;

		hr = pPeer->EnumPlayersAndGroups(NULL, &dwNumIDs, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "EnumP&G-ing with invalid flags didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was modified (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "EnumP&G-ing with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumPlayersAndGroups(NULL, &dwNumIDs, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "EnumP&G-ing with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was modified (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming players before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumPlayersAndGroups(NULL, &dwNumIDs, DPNENUM_PLAYERS);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Enuming players before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was modified (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPEnumPAndGDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming players without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumPlayersAndGroups(NULL, &dwNumIDs, DPNENUM_PLAYERS);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Enuming players without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was modified (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming players with NULL buffer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumPlayersAndGroups(NULL, &dwNumIDs, DPNENUM_PLAYERS);
		if (hr != DPNERR_BUFFERTOOSMALL)
		{
			DPL(0, "Enuming players with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 1)
		{
			DPL(0, "Number of IDs was not expected (%u != 1)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming players"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumPlayersAndGroups(adpnidIDs, &dwNumIDs, DPNENUM_PLAYERS);
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming players failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 1)
		{
			DPL(0, "Number of IDs was not expected (%u != 1)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

#pragma BUGBUG(vanceo, "Get real player ID")
		if (adpnidIDs[0] != (DPNID) 0x00200002)
		{
			DPL(0, "Player ID returned was not expected ([0] %u/%x != %u/%x)!",
				4, adpnidIDs[0], adpnidIDs[0],
				(DPNID) 0x00200002, (DPNID) 0x00200002);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player ID wrong)

		if ((adpnidIDs[1] != (DPNID) 0x666) ||
			(adpnidIDs[2] != (DPNID) 0x666) ||
			(adpnidIDs[3] != (DPNID) 0x666))
		{
			DPL(0, "Player ID buffer was overrun ([1] %u/%x, [2] %u/%x or [3] %u/%x != 0x666)!",
				6, adpnidIDs[1], adpnidIDs[1],
				adpnidIDs[2], adpnidIDs[2],
				adpnidIDs[3], adpnidIDs[3]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer overrun)

		// Reset the entry.
		adpnidIDs[0] = 0x666;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming players & groups with NULL buffer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 0;

		hr = pPeer->EnumPlayersAndGroups(NULL, &dwNumIDs,
												(DPNENUM_PLAYERS | DPNENUM_GROUPS));
		if (hr != DPNERR_BUFFERTOOSMALL)
		{
			DPL(0, "Enuming players & groups with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 1)
		{
			DPL(0, "Number of IDs was not expected (%u != 1)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming players & groups"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumPlayersAndGroups(adpnidIDs, &dwNumIDs, (DPNENUM_PLAYERS | DPNENUM_GROUPS));
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming players & groups failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 1)
		{
			DPL(0, "Number of IDs was not expected (%u != 1)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

#pragma BUGBUG(vanceo, "Get real player ID")
		if (adpnidIDs[0] != (DPNID) 0x00200002)
		{
			DPL(0, "Player ID returned was not expected ([0] %u/%x != %u/%x)!",
				4, adpnidIDs[0], adpnidIDs[0],
				(DPNID) 0x00200002, (DPNID) 0x00200002);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player ID wrong)

		if ((adpnidIDs[1] != (DPNID) 0x666) ||
			(adpnidIDs[2] != (DPNID) 0x666) ||
			(adpnidIDs[3] != (DPNID) 0x666))
		{
			DPL(0, "Player ID buffer was overrun ([1] %u/%x, [2] %u/%x or [3] %u/%x != 0x666)!",
				6, adpnidIDs[1], adpnidIDs[1],
				adpnidIDs[2], adpnidIDs[2],
				adpnidIDs[3], adpnidIDs[3]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer overrun)

		// Reset the entry.
		adpnidIDs[0] = 0x666;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming groups with NULL buffer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 0;

		hr = pPeer->EnumPlayersAndGroups(NULL, &dwNumIDs, DPNENUM_GROUPS);
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming groups with NULL buffer failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was not expected (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming groups"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumPlayersAndGroups(adpnidIDs, &dwNumIDs, DPNENUM_GROUPS);
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming groups failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was not expected (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

		if ((adpnidIDs[0] != (DPNID) 0x666) ||
			(adpnidIDs[1] != (DPNID) 0x666) ||
			(adpnidIDs[2] != (DPNID) 0x666) ||
			(adpnidIDs[3] != (DPNID) 0x666))
		{
			DPL(0, "Player ID buffer was modified ([0] %u/%x, [1] %u/%x, [2] %u/%x or [3] %u/%x != 0x666)!",
				8, adpnidIDs[0], adpnidIDs[0],
				adpnidIDs[1], adpnidIDs[1],
				adpnidIDs[2], adpnidIDs[2],
				adpnidIDs[3], adpnidIDs[3]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer modified)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
		dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
		//dpGroupInfo.dwInfoFlags = 0;
		//dpGroupInfo.pwszName = NULL;
		//dpGroupInfo.pvData = NULL;
		//dpGroupInfo.dwDataSize = 0;
		//dpGroupInfo.dwGroupFlags = 0;

		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));


		// Expect the create group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming groups"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 1;

		hr = pPeer->EnumPlayersAndGroups(adpnidIDs, &dwNumIDs, DPNENUM_GROUPS);
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming groups failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 1)
		{
			DPL(0, "Number of IDs was not expected (%u != 1)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

		if (adpnidIDs[0] != hostcontext.dpnidGroup)
		{
			DPL(0, "Group ID returned was not expected ([0] %u/%x != %u/%x)!",
				4, adpnidIDs[0], adpnidIDs[0],
				hostcontext.dpnidGroup, hostcontext.dpnidGroup);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player ID wrong)

		if ((adpnidIDs[1] != (DPNID) 0x666) ||
			(adpnidIDs[2] != (DPNID) 0x666) ||
			(adpnidIDs[3] != (DPNID) 0x666))
		{
			DPL(0, "Player ID buffer was overrun ([1] %u/%x, [2] %u/%x or [3] %u/%x != 0x666)!",
				6, adpnidIDs[1], adpnidIDs[1],
				adpnidIDs[2], adpnidIDs[2],
				adpnidIDs[3], adpnidIDs[3]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer overrun)

		// Reset the entry.
		adpnidIDs[0] = 0x666;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming players & groups"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 2;

		hr = pPeer->EnumPlayersAndGroups(adpnidIDs, &dwNumIDs,
												(DPNENUM_PLAYERS | DPNENUM_GROUPS));
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming players & groups failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 2)
		{
			DPL(0, "Number of IDs was not expected (%u != 2)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

		// NOTE: This test assumes they'll be returned in this order.
#pragma BUGBUG(vanceo, "Get real player ID")
		if (adpnidIDs[0] != (DPNID) 0x00200002)
		{
			DPL(0, "Player ID returned was not expected ([0] %u/%x != %u/%x)!",
				4, adpnidIDs[0], adpnidIDs[0],
				(DPNID) 0x00200002, (DPNID) 0x00200002);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player ID wrong)

		if (adpnidIDs[1] != hostcontext.dpnidGroup)
		{
			DPL(0, "Group ID returned was not expected ([1] %u/%x != %u/%x )!",
				4, adpnidIDs[1], adpnidIDs[1],
				hostcontext.dpnidGroup, hostcontext.dpnidGroup);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer overrun)

		if ((adpnidIDs[2] != (DPNID) 0x666) ||
			(adpnidIDs[3] != (DPNID) 0x666))
		{
			DPL(0, "Player ID buffer was overrun ([2] %u/%x or [3] %u/%x != 0x666)!",
				4, adpnidIDs[2], adpnidIDs[2],
				adpnidIDs[3], adpnidIDs[3]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer overrun)

		// Reset the entries.
		adpnidIDs[0] = 0x666;
		adpnidIDs[1] = 0x666;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")

		// Reset group ID.
		hostcontext.dpnidGroup = 0;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPGetGroupInfoDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPGetGroupInfoDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group on host synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the create group message on the host.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		// Expect the create group message on the client.
		nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		CREATEEVENT_OR_THROW(nonhostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeerHost->CreateGroup(&dpGroupInfo, NULL, NULL, NULL,
											DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group on host synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for group to be created on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create group indications.
		nonhostcontext.fCanGetCreateGroup = FALSE;

#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (nonhostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get create group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(nonhostcontext.hGotGroupMsgEvent);
		nonhostcontext.hGotGroupMsgEvent = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming players & groups on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 3;

		hr = pPeer->EnumPlayersAndGroups(adpnidIDs, &dwNumIDs,
												(DPNENUM_PLAYERS | DPNENUM_GROUPS));
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming players & groups on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum players and groups)

		if (dwNumIDs != 3)
		{
			DPL(0, "Number of IDs was not expected (%u != 3)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

		// NOTE: This test assumes they'll be returned in this order.
#pragma BUGBUG(vanceo, "Get real player ID")
		if (adpnidIDs[0] != (DPNID) 0x00200002)
		{
			DPL(0, "Host player ID returned was not expected ([0] %u/%x != %u/%x)!",
				4, adpnidIDs[0], adpnidIDs[0],
				(DPNID) 0x00200002, (DPNID) 0x00200002);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player ID wrong)

		if (adpnidIDs[1] != (DPNID) 0x00300003)
		{
			DPL(0, "Client player ID returned was not expected ([1] %u/%x != %u/%x)!",
				4, adpnidIDs[1], adpnidIDs[1],
				(DPNID) 0x00300003, (DPNID) 0x00300003);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player ID wrong)

		if (adpnidIDs[2] != hostcontext.dpnidGroup)
		{
			DPL(0, "Group ID returned was not expected ([1] %u/%x != %u/%x )!",
				4, adpnidIDs[2], adpnidIDs[2],
				hostcontext.dpnidGroup, hostcontext.dpnidGroup);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer overrun)

		if (adpnidIDs[3] != (DPNID) 0x666)
		{
			DPL(0, "Player ID buffer was overrun ([2] %u/%x != 0x666)!",
				2, adpnidIDs[2], adpnidIDs[2]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer overrun)

		// Reset the entries.
		adpnidIDs[0] = 0x666;
		adpnidIDs[1] = 0x666;
		adpnidIDs[2] = 0x666;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hGotGroupMsgEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hGotGroupMsgEvent);


	return (hr);
} // ParmVPeerExec_EnumPAndG
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_EnumGroupMembers()"
//==================================================================================
// ParmVPeerExec_EnumGroupMembers
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.18 - Peer EnumGroupMembers parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_EnumGroupMembers(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					hr;
	CTNTestResult					hr;
	PWRAPDP8PEER					pDP8Peer = NULL;
	PWRAPDP8PEER					pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS				pDP8Address = NULL;
	PARMVPENUMGROUPMEMBERSCONTEXT	hostcontext;
	PARMVPENUMGROUPMEMBERSCONTEXT	nonhostcontext;
	DPN_APPLICATION_DESC			dpAppDesc;
	DPN_APPLICATION_DESC			dpAppDescCompare;
	DPN_GROUP_INFO					dpGroupInfo;
	DPN_GROUP_INFO					dpGroupInfoCompare;
	DPNID							adpnidIDs[3];
	DWORD							dwNumIDs;



	ZeroMemory(&hostcontext, sizeof (PARMVPENUMGROUPMEMBERSCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPENUMGROUPMEMBERSCONTEXT));


	BEGIN_TESTCASE
	{
		adpnidIDs[0] = 0x666;
		adpnidIDs[1] = 0x666;
		adpnidIDs[2] = 0x666;
		

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group 0 (DPNID_ALL_PLAYERS_GROUP) members with NULLs and 0 flags using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_EnumGroupMembers(pPeer->m_pDP8Peer, 0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Enuming group 0 (DPNID_ALL_PLAYERS_GROUP) members with NULLs and 0 flags using C++ macro didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group 0 (DPNID_ALL_PLAYERS_GROUP) members with NULLs and 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumGroupMembers(0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Enuming group 0 (DPNID_ALL_PLAYERS_GROUP) members with NULLs and 0 flags didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members with NULL size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Use 0x666 to avoid DPlay's group ID == 0 check.
		hr = pPeer->EnumGroupMembers((DPNID) 0x666, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Enuming group members with NULL size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members with NULL buffer and non-zero size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 666;

		hr = pPeer->EnumGroupMembers((DPNID) 0x666, NULL, &dwNumIDs, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Enuming group members with NULL buffer and non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 666)
		{
			DPL(0, "Number of IDs was modified (%u != 666)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 0;

		hr = pPeer->EnumGroupMembers((DPNID) 0x666, NULL, &dwNumIDs, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Enuming group members with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was modified (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumGroupMembers((DPNID) 0x666, NULL, &dwNumIDs, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Enuming group members before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was modified (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPEnumGroupMembersDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumGroupMembers((DPNID) 0x666, NULL, &dwNumIDs, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Enuming group members without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was modified (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members with invalid group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumGroupMembers((DPNID) 0x666, NULL, &dwNumIDs, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Enuming group members with invalid group didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was modified (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members of all players group internal ID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Determine real ID")
		hr = pPeer->EnumGroupMembers((DPNID) 0x00100001, NULL, &dwNumIDs, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Enuming group members of all players group internal ID didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 0)
		{
			DPL(0, "Num IDs was changed (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (num IDs changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
		dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
		//dpGroupInfo.dwInfoFlags = 0;
		//dpGroupInfo.pwszName = NULL;
		//dpGroupInfo.pvData = NULL;
		//dpGroupInfo.dwDataSize = 0;
		//dpGroupInfo.dwGroupFlags = 0;

		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));


		// Expect the create group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members of empty group with NULL buffer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumGroupMembers(hostcontext.dpnidGroup, NULL,
											&dwNumIDs, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming group members of empty group with NULL buffer failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was not expected (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members of empty group"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumGroupMembers(hostcontext.dpnidGroup, adpnidIDs,
											&dwNumIDs, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming group members of empty group failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 0)
		{
			DPL(0, "Number of IDs was not expected (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

		if ((adpnidIDs[0] != (DPNID) 0x666) ||
			(adpnidIDs[1] != (DPNID) 0x666) ||
			(adpnidIDs[2] != (DPNID) 0x666))
		{
			DPL(0, "Player ID buffer was modified ([0] %u/%x, [1] %u/%x, or [2] %u/%x != 0x666)!",
				6, adpnidIDs[0], adpnidIDs[0],
				adpnidIDs[1], adpnidIDs[1],
				adpnidIDs[2], adpnidIDs[2]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer modified)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		hostcontext.fCanGetAddToGroup = TRUE;

		hr = pPeer->AddPlayerToGroup(hostcontext.dpnidGroup,
											hostcontext.dpnidExpectedPlayer,
											NULL,
											NULL,
											dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird add player to group messages.
		hostcontext.fCanGetAddToGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Adding self player to group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		// Make sure we got the add to group message.
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		hostcontext.fGotAddToGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members with NULL buffer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumGroupMembers(hostcontext.dpnidGroup, NULL,
											&dwNumIDs, 0);
		if (hr != DPNERR_BUFFERTOOSMALL)
		{
			DPL(0, "Enuming group members with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 1)
		{
			DPL(0, "Number of IDs was not expected (%u != 1)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumGroupMembers(hostcontext.dpnidGroup, adpnidIDs,
											&dwNumIDs, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming group members failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 1)
		{
			DPL(0, "Number of IDs was not expected (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

		if (adpnidIDs[0] != (DPNID) 0x00200002)
		{
			DPL(0, "Player ID returned was not expected ([0] %u/%x != %u/%x)!",
				4, adpnidIDs[0], adpnidIDs[0],
				(DPNID) 0x00200002, (DPNID) 0x00200002);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player ID wrong)

		if ((adpnidIDs[1] != (DPNID) 0x666) ||
			(adpnidIDs[2] != (DPNID) 0x666))
		{
			DPL(0, "Player ID buffer was overrun ([1] %u/%x, or [2] %u/%x!= 0x666)!",
				4, adpnidIDs[1], adpnidIDs[1],
				adpnidIDs[2], adpnidIDs[2]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer overrun)

		// Reset the entry.
		adpnidIDs[0] = 0x666;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")

		// Reset group ID.
		hostcontext.dpnidGroup = 0;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPEnumGroupMembersDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPEnumGroupMembersDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group on host synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the create group message on the host.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		// Expect the create group message on the client.
		nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		CREATEEVENT_OR_THROW(nonhostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeerHost->CreateGroup(&dpGroupInfo, NULL, NULL, NULL, DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group on host synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for group to be created on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create group indications.
		nonhostcontext.fCanGetCreateGroup = FALSE;

#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (nonhostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get create group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(nonhostcontext.hGotGroupMsgEvent);
		nonhostcontext.hGotGroupMsgEvent = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Adding self player to group synchronously on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the add player to group message on the host and client.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		hostcontext.fCanGetAddToGroup = TRUE;

		nonhostcontext.dpnidExpectedPlayer = (DPNID) 0x00200002;
		CREATEEVENT_OR_THROW(nonhostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.fCanGetAddToGroup = TRUE;

		hr = pPeerHost->AddPlayerToGroup(hostcontext.dpnidGroup,
												hostcontext.dpnidExpectedPlayer,
												NULL,
												NULL,
												dpAppDescDPLAYERTOGROUP_SYNC);

		// Prevent any weird add player to group messages.
		hostcontext.fCanGetAddToGroup = FALSE;
		
		if (hr != DPN_OK)
		{
			DPL(0, "Adding self player to group synchronously on host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't add player to group)

		// Make sure we got the add to group message.
		if (! hostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add player to group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get add player to group)

		// Reset the context.
		hostcontext.fGotAddToGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for add player to group to be indicated on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird add to group indications.
		nonhostcontext.fCanGetAddToGroup = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! nonhostcontext.fGotAddToGroup)
		{
			DPL(0, "Didn't get add to group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get add to group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(nonhostcontext.hGotGroupMsgEvent);
		nonhostcontext.hGotGroupMsgEvent = NULL;
		nonhostcontext.fGotAddToGroup = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members with NULL buffer on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwNumIDs = 0;

		hr = pPeer->EnumGroupMembers(nonhostcontext.dpnidGroup, NULL,
											&dwNumIDs, 0);
		if (hr != DPNERR_BUFFERTOOSMALL)
		{
			DPL(0, "Enuming group members with NULL buffer on client didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 1)
		{
			DPL(0, "Number of IDs was not expected (%u != 1)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming group members on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumGroupMembers(nonhostcontext.dpnidGroup, adpnidIDs,
											&dwNumIDs, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming group members on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum group members)

		if (dwNumIDs != 1)
		{
			DPL(0, "Number of IDs was not expected (%u != 0)!", 1, dwNumIDs);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (num IDs not expected)

		if (adpnidIDs[0] != (DPNID) 0x00200002)
		{
			DPL(0, "Player ID returned was not expected ([0] %u/%x != %u/%x)!",
				4, adpnidIDs[0], adpnidIDs[0],
				(DPNID) 0x00200002, (DPNID) 0x00200002);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player ID wrong)

		if ((adpnidIDs[1] != (DPNID) 0x666) ||
			(adpnidIDs[2] != (DPNID) 0x666))
		{
			DPL(0, "Player ID buffer was overrun ([1] %u/%x, or [2] %u/%x!= 0x666)!",
				4, adpnidIDs[1], adpnidIDs[1],
				adpnidIDs[2], adpnidIDs[2]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (buffer overrun)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hGotGroupMsgEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hGotGroupMsgEvent);


	return (hr);
} // ParmVPeerExec_EnumGroupMembers
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_SetPeerInfo()"
//==================================================================================
// ParmVPeerExec_SetPeerInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.19 - Peer SetPeerInfo parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_SetPeerInfo(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult				hr;
	CTNTestResult				hr;
	PWRAPDP8PEER				pDP8Peer = NULL;
	PWRAPDP8PEER				pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	PARMVPSETPEERINFOCONTEXT	hostcontext;
	PARMVPSETPEERINFOCONTEXT	nonhostcontext;
	DPN_APPLICATION_DESC		dpAppDesc;
	DPN_APPLICATION_DESC		dpAppDescCompare;
	DPN_PLAYER_INFO				dpnpi;
	DPN_PLAYER_INFO				dpnpiCompare;
	DPNHANDLE					dpnhSetPeerInfo;



	ZeroMemory(&hostcontext, sizeof (PARMVPSETPEERINFOCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPSETPEERINFOCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info with all NULLs and 0 flags using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_SetPeerInfo(pPeer->m_pDP8Peer, NULL, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Setting peer info with all NULLs and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info with all NULLs and 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetPeerInfo(NULL, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Setting peer info with all NULLs and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info with NULL player info structure"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhSetPeerInfo = (DPNHANDLE) 0x666;

		hr = pPeer->SetPeerInfo(NULL, NULL, &dpnhSetPeerInfo, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Setting peer info with NULL player info structure didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)

		if (dpnhSetPeerInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetPeerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info with NULL async op handle"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnpi, sizeof (DPN_PLAYER_INFO));
		dpnpi.dwSize = sizeof (DPN_PLAYER_INFO);
		//dpnpi.dwInfoFlags = 0;
		//dpnpi.pwszName = NULL;
		//dpnpi.pvData = NULL;
		//dpnpi.dwDataSize = 0;
		//dpnpi.dwPlayerFlags = 0;

		CopyMemory(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO));

		hr = pPeer->SetPeerInfo(&dpnpi, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting peer info with NULL async op handle didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPL(0, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetPeerInfo(&dpnpi, NULL, &dpnhSetPeerInfo, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Setting peer info with invalid flags structure didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPL(0, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetPeerInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetPeerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info synchronously with async op handle"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetPeerInfo(&dpnpi, NULL, &dpnhSetPeerInfo,
										DPNSETPEERINFO_SYNC);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting peer info synchronously with async op handle didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPL(0, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetPeerInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetPeerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetPeerInfo(&dpnpi, NULL, &dpnhSetPeerInfo, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Setting peer info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPL(0, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetPeerInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetPeerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPSetPeerInfoDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info prior to connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetPeerInfo(&dpnpi, NULL, &dpnhSetPeerInfo, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting peer info prior to connection failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPL(0, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetPeerInfo != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was modified (%x != 0x666)!",
				1, dpnhSetPeerInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (handle was set)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the peer info message and async op completion.
		hostcontext.fCanGetPeerInfoUpdate = TRUE;
		CREATEEVENT_OR_THROW(hostcontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.fAsyncOpCanComplete = TRUE;

		hr = pPeer->SetPeerInfo(&dpnpi, NULL, &dpnhSetPeerInfo, 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Setting peer info didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)

		if (memcmp(&dpnpiCompare, &dpnpi, sizeof (DPN_PLAYER_INFO)) != 0)
		{
			DPL(0, "Player info structure was modified (structure at %x != structure at %x)!",
				2, &dpnpiCompare, &dpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player info changed)

		if (dpnhSetPeerInfo == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhSetPeerInfo == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hAsyncOpCompletedEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// The peer info message should have arrived by now.
		hostcontext.fCanGetPeerInfoUpdate = FALSE;

		// Prevent any weird async op completions.
		hostcontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fAsyncOpCompleted)
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (hostcontext.dpnhCompletedAsyncOp != dpnhSetPeerInfo)
		{
			DPL(0, "Set peer info completed with different async op handle (%x != %x)!",
				2, hostcontext.dpnhCompletedAsyncOp, dpnhSetPeerInfo);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		hostcontext.fAsyncOpCompleted = FALSE;
		CloseHandle(hostcontext.hAsyncOpCompletedEvent);
		hostcontext.hAsyncOpCompletedEvent = NULL;


		// Make sure we got the peer info message.
		if (! hostcontext.fGotPeerInfoUpdate)
		{
			DPL(0, "Didn't get peer info message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get peer info)

		// Reset the context.
		hostcontext.fGotPeerInfoUpdate = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPSetPeerInfoDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPSetPeerInfoDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting peer info on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhSetPeerInfo = (DPNHANDLE) 0x666;

		// Expect the peer info message on the host.
		hostcontext.fCanGetPeerInfoUpdate = TRUE;
		CREATEEVENT_OR_THROW(hostcontext.hGotPeerInfoUpdateEvent,
							NULL, FALSE, FALSE, NULL);

		// Expect the group info message and async op completion on the client.
		nonhostcontext.fCanGetPeerInfoUpdate = TRUE;
		CREATEEVENT_OR_THROW(nonhostcontext.hAsyncOpCompletedEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.fAsyncOpCanComplete = TRUE;

		hr = pPeer->SetPeerInfo(&dpnpi, NULL, &dpnhSetPeerInfo, 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Setting peer info on client didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set peer info)

		if (dpnhSetPeerInfo == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhSetPeerInfo == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hAsyncOpCompletedEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// The peer info message should have arrived by now.
		nonhostcontext.fCanGetPeerInfoUpdate = FALSE;

		// Prevent any weird async op completions.
		nonhostcontext.fAsyncOpCanComplete = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! nonhostcontext.fAsyncOpCompleted)
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (nonhostcontext.dpnhCompletedAsyncOp != dpnhSetPeerInfo)
		{
			DPL(0, "Set peer info completed with different async op handle (%x != %x)!",
				2, nonhostcontext.dpnhCompletedAsyncOp, dpnhSetPeerInfo);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		nonhostcontext.fAsyncOpCompleted = FALSE;
		CloseHandle(nonhostcontext.hAsyncOpCompletedEvent);
		nonhostcontext.hAsyncOpCompletedEvent = NULL;


		// Make sure we got the peer info message.
		if (! nonhostcontext.fGotPeerInfoUpdate)
		{
			DPL(0, "Didn't get peer info message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get peer info)

		// Reset the context.
		nonhostcontext.fGotPeerInfoUpdate = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for peer info to be indicated on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hGotPeerInfoUpdateEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird peer info indications.
		hostcontext.fCanGetPeerInfoUpdate = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.fGotPeerInfoUpdate)
		{
			DPL(0, "Didn't get peer info indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get peer info indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hGotPeerInfoUpdateEvent);
		hostcontext.hGotPeerInfoUpdateEvent = NULL;
		hostcontext.fGotPeerInfoUpdate = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hAsyncOpCompletedEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hAsyncOpCompletedEvent);
	SAFE_CLOSEHANDLE(hostcontext.hGotPeerInfoUpdateEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hGotPeerInfoUpdateEvent);


	return (hr);
} // ParmVPeerExec_SetPeerInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_GetPeerInfo()"
//==================================================================================
// ParmVPeerExec_GetPeerInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.20 - Peer GetPeerInfo parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_GetPeerInfo(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult				hr;
	CTNTestResult				hr;
	PWRAPDP8PEER				pDP8Peer = NULL;
	PWRAPDP8PEER				pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	PARMVPGETPEERINFOCONTEXT	hostcontext;
	PARMVPGETPEERINFOCONTEXT	nonhostcontext;
	DPN_APPLICATION_DESC		dpAppDesc;
	DPN_APPLICATION_DESC		dpAppDescCompare;
	PDPN_PLAYER_INFO			pdpnpi = NULL;
	DPN_PLAYER_INFO				dpnpi;
	DWORD						dwSize;
	DWORD						dwExpectedSize;



	ZeroMemory(&hostcontext, sizeof (PARMVPSETPEERINFOCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPSETPEERINFOCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer 0 info with NULLs and 0 flags using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_GetPeerInfo(pPeer->m_pDP8Peer, 0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Getting peer 0 info with NULLs and 0 flags using C++ macro didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer 0 info with NULLs and 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPeerInfo(0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Getting peer 0 info with NULLs and 0 flags didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer info with NULL buffer and non-zero size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSize = 666;

		// Use 0x666 to avoid DPlay's player ID == 0 check.
		hr = pPeer->GetPeerInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting peer info with NULL buffer and non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != 666)
		{
			DPL(0, "Size was changed (%u != 666)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer info with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSize = 0;

		hr = pPeer->GetPeerInfo((DPNID) 0x666, NULL, &dwSize, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Getting peer info with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != 0)
		{
			DPL(0, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer info before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPeerInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Getting peer info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != 0)
		{
			DPL(0, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPGetPeerInfoDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer info prior to connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPeerInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Getting peer info prior to connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != 0)
		{
			DPL(0, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer info for invalid player"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPeerInfo((DPNID) 0x666, NULL, &dwSize, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Getting peer info for invalid player didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != 0)
		{
			DPL(0, "Size was changed (%u != 0)!", 1, dwSize);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size is not 0)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer info with NULL buffer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwExpectedSize = sizeof (DPN_PLAYER_INFO);

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->GetPeerInfo((DPNID) 0x00200002, NULL, &dwSize, 0);
		if (hr != DPNERR_BUFFERTOOSMALL)
		{
			DPL(0, "Getting peer info with NULL buffer didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size was not expected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer info with peer info size of 0"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		LOCALALLOC_OR_THROW(PDPN_PLAYER_INFO, pdpnpi, dwSize + BUFFERPADDING_SIZE);

		FillWithDWord((((PBYTE) pdpnpi) + sizeof (DWORD)),
						(dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE),
						DONT_TOUCH_MEMORY_PATTERN);

		pdpnpi->dwSize = 0;


#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->GetPeerInfo((DPNID) 0x00200002, pdpnpi, &dwSize, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting peer info using buffer with peer info size of 0 didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)

		// Make sure the buffer was not modified or overrun.
		if ((pdpnpi->dwSize != 0) ||
			(! IsFilledWithDWord((((PBYTE) pdpnpi) + sizeof (DWORD)), (dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE), DONT_TOUCH_MEMORY_PATTERN)))
		{
			DPL(0, "The %u byte player info buffer (at %x) was modified or overrun!",
				2, dwSize, pdpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer info with peer info size too large"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pdpnpi->dwSize = sizeof (DPN_PLAYER_INFO) + 1;

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->GetPeerInfo((DPNID) 0x00200002, pdpnpi, &dwSize, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting peer info using buffer with peer info size too large didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)

		// Make sure the buffer was not modified or overrun.
		if ((pdpnpi->dwSize != (sizeof (DPN_PLAYER_INFO) + 1)) ||
			(! IsFilledWithDWord((((PBYTE) pdpnpi) + sizeof (DWORD)), (dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE), DONT_TOUCH_MEMORY_PATTERN)))
		{
			DPL(0, "The %u byte player info buffer (at %x) was modified or overrun!",
				2, dwSize, pdpnpi);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (buffer was overrun)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting peer info"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pdpnpi->dwSize = sizeof (DPN_PLAYER_INFO);

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->GetPeerInfo((DPNID) 0x00200002, pdpnpi, &dwSize, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting peer info failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)


		// Make sure the structure returned was expected.

		ZeroMemory(&dpnpi, sizeof (DPN_PLAYER_INFO));
		dpnpi.dwSize = sizeof (DPN_PLAYER_INFO);
		dpnpi.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
		//dpnpi.pwszName = NULL;
		//dpnpi.pvData = NULL;
		//dpnpi.dwDataSize = 0;
		dpnpi.dwPlayerFlags = DPNPLAYER_LOCAL | DPNPLAYER_HOST;

		hr = ParmVComparePlayerInfo(pdpnpi, &dpnpi);
		if (hr != DPN_OK)
		{
			DPL(0, "Comparing player info buffer %x with expected %x failed!",
				2, pdpnpi, &dpnpi);
			THROW_TESTRESULT;
		} // end if (failed comparison)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPGetPeerInfoDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPGetGroupInfoDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting host's peer info with NULL buffer on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dwSize = 0;

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->GetPeerInfo((DPNID) 0x00200002, NULL, &dwSize, 0);
		if (hr != DPNERR_BUFFERTOOSMALL)
		{
			DPL(0, "Getting peer info with NULL buffer on client didn't return expected error BUFFERTOOSMALL!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size was not expected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting host's peer info on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		FillWithDWord((((PBYTE) pdpnpi) + sizeof (DWORD)),
						(dwSize - sizeof (DWORD) + BUFFERPADDING_SIZE),
						DONT_TOUCH_MEMORY_PATTERN);

		pdpnpi->dwSize = sizeof (DPN_PLAYER_INFO);

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->GetPeerInfo((DPNID) 0x00200002, pdpnpi, &dwSize, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting peer info on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)

		if (dwSize != dwExpectedSize)
		{
			DPL(0, "Size returned was unexpected (%u != %u)!", 2, dwSize, dwExpectedSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (size is not expected)


		// Make sure the structure returned was expected.

		ZeroMemory(&dpnpi, sizeof (DPN_PLAYER_INFO));
		dpnpi.dwSize = sizeof (DPN_PLAYER_INFO);
		dpnpi.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
		//dpnpi.pwszName = NULL;
		//dpnpi.pvData = NULL;
		//dpnpi.dwDataSize = 0;
		dpnpi.dwPlayerFlags = DPNPLAYER_HOST;

		hr = ParmVComparePlayerInfo(pdpnpi, &dpnpi);
		if (hr != DPN_OK)
		{
			DPL(0, "Comparing player info buffer %x with expected %x failed!",
				2, pdpnpi, &dpnpi);
			THROW_TESTRESULT;
		} // end if (failed comparison)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_LOCALFREE(pdpnpi);
	SAFE_RELEASE(pDP8Address);


	return (hr);
} // ParmVPeerExec_GetPeerInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_Close()"
//==================================================================================
// ParmVPeerExec_Close
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.21 - Peer Close parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_Close(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			hr;
	CTNTestResult			hr;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PDIRECTPLAY8ADDRESS		pDP8Address = NULL;
	DPN_APPLICATION_DESC	dpAppDesc;
	DPN_APPLICATION_DESC	dpAppDescCompare;
	PARMVPCLOSECONTEXT		context;
	DPNHANDLE				dpnhEnumHosts;



	ZeroMemory(&context, sizeof (PARMVPCLOSECONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing before initialization using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_Close(pPeer->m_pDP8Peer, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Closing before initialization using C++ macro didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get peer info)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Closing before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Closing with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing with NULL context, 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(NULL, ParmVNoMessagesDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing with NULL context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing with no connections"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing with no connections failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing with 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&context, ParmVPCloseDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing with 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_HOST;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing after hosting with no connected clients"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing after hosting with no connected clients failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing with 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&context, ParmVPCloseDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing with 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Beginning asynchronous enumeration"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		dpnhEnumHosts = (DPNHANDLE) 0x666;

		// Expect the completion message on the host.
		context.fAsyncOpCanComplete = TRUE;

		hr = pPeer->EnumHosts(&dpAppDesc, NULL, pDP8Address, NULL, 0,
									0, 0, 0, NULL, &dpnhEnumHosts, 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Beginning asynchronous enumeration didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		if (dpnhEnumHosts == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (dpnhEnumHosts == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing with enumeration still active"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);

		// The async operation should have completed by now.
		context.fAsyncOpCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Closing with enumeration still active failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure the async op completed.
		if (! context.fAsyncOpCompleted)
		{
			DPL(0, "Asynchronous enumeration wasn't completed by Close!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (didn't get async op completion)

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (context.dpnhCompletedAsyncOp != dpnhEnumHosts)
		{
			DPL(0, "Enumeration completed with different async op handle (%x != %x)!",
				2, context.dpnhCompletedAsyncOp, dpnhEnumHosts);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)







		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);


	return (hr);
} // ParmVPeerExec_Close
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_EnumHosts()"
//==================================================================================
// ParmVPeerExec_EnumHosts
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.22 - Peer EnumHosts parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_EnumHosts(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult				hr;
	CTNTestResult				hr;
	PWRAPDP8PEER				pDP8Peer = NULL;
	PWRAPDP8PEER				pDP8PeerHost = NULL;
	DPN_APPLICATION_DESC		dpAppDesc;
	DPN_APPLICATION_DESC		dpAppDescCompare;
	PDIRECTPLAY8ADDRESS			pDP8AddressHost = NULL;
	PDIRECTPLAY8ADDRESS			pDP8AddressDevice = NULL;
	PARMVPENUMHOSTSCONTEXT		hostcontext;
	PARMVPENUMHOSTSCONTEXT		nonhostcontext;
	DPNHANDLE					adpnhEnumHosts[2];
	DWORD						dwTemp1;
	DWORD						dwTemp2;
	PPARMVPENUMHOSTSCONTEXT		pContext;


	ZeroMemory(&hostcontext, sizeof (PARMVPENUMHOSTSCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPENUMHOSTSCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating host DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8AddressHost));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't host CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating device DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8AddressDevice));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't device CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with all NULLs and 0 flags using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_EnumHosts(pPeer->m_pDP8Peer, NULL, NULL, NULL,
										NULL, 0, 0, 0, 0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Enuming hosts with all NULLs and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with all NULLs and 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(NULL, NULL, NULL, NULL, 0, 0, 0, 0,
									NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Enuming hosts with all NULLs and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with NULL app desc"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		adpnhEnumHosts[0] = (DPNHANDLE) 0x666;

		hr = pPeer->EnumHosts(NULL, pDP8AddressHost, pDP8AddressDevice,
									NULL, 0, 0, 0, 0, NULL, &(adpnhEnumHosts[0]), 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Enuming hosts with NULL app desc didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

#pragma TODO(vanceo, "Verify address objects weren't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with NULL host and device addresses"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_ENUMHOSTS1;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->EnumHosts(&dpAppDesc, NULL, NULL,
									NULL, 0, 0, 0, 0, NULL, &(adpnhEnumHosts[0]), 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Enuming hosts with NULL host and device addresses didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with host address without an SP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, NULL,
									NULL, 0, 0, 0, 0, NULL, &(adpnhEnumHosts[0]), 0);
		if (hr != DPNERR_INVALIDHOSTADDRESS)
		{
			DPL(0, "Enuming hosts with host address without an SP didn't return expected error INVALIDHOSTADDRESS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting host address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8AddressHost->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting host address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with NULL device address"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, NULL,
									NULL, 0, 0, 0, 0, NULL, &(adpnhEnumHosts[0]), 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Enuming hosts with NULL device address didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting device address object's SP to IPX"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8AddressDevice->SetSP(&CLSID_DP8SP_IPX);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting device address object's SP to IPX failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with different SPs for host and device addresses"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice,
									NULL, 0, 0, 0, 0, NULL, &(adpnhEnumHosts[0]), 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Enuming hosts with different SPs for host and device addresses didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting device address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8AddressDevice->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting device address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with NULL data but non-zero size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice,
									NULL, 666, 0, 0, 0, NULL, &(adpnhEnumHosts[0]), 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Enuming hosts with NULL data but non-zero size didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with NULL async op handle"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice,
									NULL, 0, 0, 0, 0, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Enuming hosts with NULL async op handle didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts synchronously with async op handle"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice,
									NULL, 0, 0, 0, 0, NULL, &(adpnhEnumHosts[0]),
									DPNENUMHOSTS_SYNC);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Enuming hosts synchronously with async op handle didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice,
									NULL, 0, 0, 0, 0, NULL, &(adpnhEnumHosts[0]), 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Enuming hosts with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice,
									NULL, 0, 0, 0, 0, NULL, &(adpnhEnumHosts[0]), 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Enuming hosts before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext, ParmVPEnumHostsDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the completion message on the host.
		hostcontext.ahrExpectedResult[0] = DPN_OK;
		CREATEEVENT_OR_THROW(hostcontext.ahAsyncOpCompletedEvent[0],
							NULL, FALSE, FALSE, NULL);
		hostcontext.afAsyncOpCanComplete[0] = TRUE;

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice,
									NULL, 0, 0, 0, 0, (PVOID) ((DWORD_PTR) 1),
									&(adpnhEnumHosts[0]), 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Enuming hosts didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (adpnhEnumHosts[0] == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete (may take a little while)"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.ahAsyncOpCompletedEvent[0]),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent any weird async op completions.
		hostcontext.afAsyncOpCanComplete[0] = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! hostcontext.afAsyncOpCompleted[0])
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (hostcontext.adpnhCompletedAsyncOp[0] != adpnhEnumHosts[0])
		{
			DPL(0, "Enum hosts completed with different async op handle (%x != %x)!",
				2, hostcontext.adpnhCompletedAsyncOp[0], adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		hostcontext.afAsyncOpCompleted[0] = FALSE;
		CloseHandle(hostcontext.ahAsyncOpCompletedEvent[0]);
		hostcontext.ahAsyncOpCompletedEvent[0] = NULL;

		adpnhEnumHosts[0] = (DPNHANDLE) 0x666;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts synchronously (may take a little while)"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice,
									NULL, 0, 0, 0, 0, (PVOID) ((DWORD_PTR) 1), NULL,
									DPNENUMHOSTS_SYNC);
		if (hr != DPN_OK)
		{
			DPL(0, "Enuming hosts synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Host(&dpAppDesc,
								&pDP8AddressDevice,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts after hosting"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->EnumHosts(&dpAppDesc, pDP8AddressHost, pDP8AddressDevice,
									NULL, 0, 0, 0, 0, NULL, &(adpnhEnumHosts[0]), 0);
		if (hr != DPNERR_HOSTING)
		{
			DPL(0, "Enuming hosts after hosting didn't return expected error HOSTING!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] != (DPNHANDLE) 0x666)
		{
			DPL(0, "Async handle was changed (%x != 0x666)!", 1, adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (async handle was changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPEnumHostsDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_ENUMHOSTS2;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));


#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPEnumHostsDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost, NULL,
								&nonhostcontext.apExpectedDP8AddressSender[0]);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Duplicating hosts address"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = nonhostcontext.apExpectedDP8AddressSender[0]->Duplicate(&(nonhostcontext.apExpectedDP8AddressSender[1]));
		if (hr != DPN_OK)
		{
			DPL(0, "Duplicating hosts address failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't duplicate)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect responses and the completion message on the client.
		nonhostcontext.afCanGetResponses[0] = TRUE;
		nonhostcontext.ahrExpectedResult[0] = DPN_OK;
		CREATEEVENT_OR_THROW(nonhostcontext.ahAsyncOpCompletedEvent[0],
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.afAsyncOpCanComplete[0] = TRUE;

		hr = pPeer->EnumHosts(&dpAppDesc, nonhostcontext.apExpectedDP8AddressSender[0],
									pDP8AddressDevice, NULL, 0, 0, 0, 0,
									(PVOID) ((DWORD_PTR) 1), &(adpnhEnumHosts[0]), 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Enuming hosts on client didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (adpnhEnumHosts[0] == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming more hosts on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect responses and the completion message on the client.
		nonhostcontext.afCanGetResponses[1] = TRUE;
		nonhostcontext.ahrExpectedResult[1] = DPN_OK;
		CREATEEVENT_OR_THROW(nonhostcontext.ahAsyncOpCompletedEvent[1],
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.afAsyncOpCanComplete[1] = TRUE;

		hr = pPeer->EnumHosts(&dpAppDesc, nonhostcontext.apExpectedDP8AddressSender[1],
									pDP8AddressDevice, NULL, 0, 0, 0, 0,
									(PVOID) ((DWORD_PTR) 2), &(adpnhEnumHosts[1]), 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Enuming more hosts on client didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[1] == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (adpnhEnumHosts[1] == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for first async op to complete (may take a little while)"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.ahAsyncOpCompletedEvent[0]),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent more responses from coming in.
		nonhostcontext.afCanGetResponses[0] = FALSE;

		// Prevent any weird async op completions.
		nonhostcontext.afAsyncOpCanComplete[0] = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! nonhostcontext.afAsyncOpCompleted[0])
		{
			DPL(0, "Didn't get first async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (nonhostcontext.adpnhCompletedAsyncOp[0] != adpnhEnumHosts[0])
		{
			DPL(0, "Enum hosts 1 completed with different async op handle (%x != %x)!",
				2, nonhostcontext.adpnhCompletedAsyncOp[0], adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		nonhostcontext.afAsyncOpCompleted[0] = FALSE;
		CloseHandle(nonhostcontext.ahAsyncOpCompletedEvent[0]);
		nonhostcontext.ahAsyncOpCompletedEvent[0] = NULL;

		adpnhEnumHosts[0] = (DPNHANDLE) 0x666;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for second async op to complete (may take a little while)"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.ahAsyncOpCompletedEvent[1]),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent more responses from coming in.
		nonhostcontext.afCanGetResponses[1] = FALSE;

		// Prevent any weird async op completions.
		nonhostcontext.afAsyncOpCanComplete[1] = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! nonhostcontext.afAsyncOpCompleted[1])
		{
			DPL(0, "Didn't get second async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (nonhostcontext.adpnhCompletedAsyncOp[1] != adpnhEnumHosts[1])
		{
			DPL(0, "Enum hosts 2 completed with different async op handle (%x != %x)!",
				2, nonhostcontext.adpnhCompletedAsyncOp[1], adpnhEnumHosts[1]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		nonhostcontext.afAsyncOpCompleted[1] = FALSE;
		CloseHandle(nonhostcontext.ahAsyncOpCompletedEvent[1]);
		nonhostcontext.ahAsyncOpCompletedEvent[1] = NULL;

		adpnhEnumHosts[1] = (DPNHANDLE) 0x666;



		// Make sure some data actually went back and forth.
		if (hostcontext.dwNumQueries == 0)
		{
			DPL(0, "Host didn't get any enum queries!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get any queries)
		else
		{
			DPL(0, "Host got %u queries.", 1, hostcontext.dwNumQueries);
		} // end else (got some queries)

		if (nonhostcontext.adwNumResponses[0] == 0)
		{
			DPL(0, "Client didn't get any responses to enum 1!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get any responses)
		else
		{
			DPL(0, "Client got %u responses to enum 1.", 1, nonhostcontext.adwNumResponses[0]);
		} // end else (got some responses)

		if (nonhostcontext.adwNumResponses[1] == 0)
		{
			DPL(0, "Client didn't get any responses to enum 2!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get any responses)
		else
		{
			DPL(0, "Client got %u responses to enum 2.", 1, nonhostcontext.adwNumResponses[1]);
		} // end else (got some responses)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Enuming hosts on client with NULL host address"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect responses and the completion message on the client.
		nonhostcontext.afCanGetResponses[0] = TRUE;
		nonhostcontext.ahrExpectedResult[0] = DPN_OK;
		CREATEEVENT_OR_THROW(nonhostcontext.ahAsyncOpCompletedEvent[0],
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.afAsyncOpCanComplete[0] = TRUE;

		hr = pPeer->EnumHosts(&dpAppDesc, NULL, pDP8AddressDevice,
									NULL, 0, 0, 0, 0,
									(PVOID) ((DWORD_PTR) 1), &(adpnhEnumHosts[0]), 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			DPL(0, "Enuming hosts on client with NULL host address didn't return expected PENDING success code!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't enum hosts)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Verify address object wasn't touched")

		if (adpnhEnumHosts[0] == (DPNHANDLE) 0x666)
		{
			DPL(0, "No async operation handle was returned!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (handle was not set)

		if (adpnhEnumHosts[0] == NULL)
		{
			DPL(0, "Async operation handle returned is NULL!", 0);
			SETTHROW_TESTRESULT(E_FAIL);
		} // end if (handle was set to NULL)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for async op to complete (may take a little while)"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.ahAsyncOpCompletedEvent[0]),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;

		// Prevent more responses from coming in.
		nonhostcontext.afCanGetResponses[0] = FALSE;

		// Prevent any weird async op completions.
		nonhostcontext.afAsyncOpCanComplete[0] = FALSE;


#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (! nonhostcontext.afAsyncOpCompleted[0])
		{
			DPL(0, "Didn't get async op completion!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get async op completion)
#endif // DEBUG

		// Make sure the completion indicated the same op handle that we were
		// returned.
		if (nonhostcontext.adpnhCompletedAsyncOp[0] != adpnhEnumHosts[0])
		{
			DPL(0, "Enum hosts 1 completed with different async op handle (%x != %x)!",
				2, nonhostcontext.adpnhCompletedAsyncOp[0], adpnhEnumHosts[0]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (async op handle is wrong)

		// Reset the context.
		nonhostcontext.afAsyncOpCompleted[0] = FALSE;
		CloseHandle(nonhostcontext.ahAsyncOpCompletedEvent[0]);
		nonhostcontext.ahAsyncOpCompletedEvent[0] = NULL;

		adpnhEnumHosts[0] = (DPNHANDLE) 0x666;







		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8AddressHost);
	SAFE_RELEASE(pDP8AddressDevice);
	for(dwTemp1 = 0; dwTemp1 < 2; dwTemp1++)
	{
		if (dwTemp1 == 0)
			pContext = &hostcontext;
		else
			pContext = &nonhostcontext;

		for (dwTemp2 = 0; dwTemp2 < 2; dwTemp2++)
		{
			SAFE_RELEASE(pContext->apExpectedDP8AddressSender[dwTemp2]);
			SAFE_RELEASE(pContext->apExpectedDP8AddressDevice[dwTemp2]);
			SAFE_CLOSEHANDLE(pContext->ahAsyncOpCompletedEvent[dwTemp2]);
		} // end for (each op)
	} // end for (each op)


	return (hr);
} // ParmVPeerExec_EnumHosts
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_DestroyPeer()"
//==================================================================================
// ParmVPeerExec_DestroyPeer
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.23 - Peer DestroyPeer parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_DestroyPeer(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult				hr;
	CTNTestResult				hr;
	PWRAPDP8PEER				pDP8Peer = NULL;
	PWRAPDP8PEER				pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	DPN_APPLICATION_DESC		dpAppDesc;
	DPN_APPLICATION_DESC		dpAppDescCompare;
	PARMVPDESTROYPEERCONTEXT	hostcontext;
	PARMVPDESTROYPEERCONTEXT	nonhostcontext;



	ZeroMemory(&hostcontext, sizeof (PARMVPDESTROYPEERCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPDESTROYPEERCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying peer 0 using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_DestroyPeer(pPeer->m_pDP8Peer, 0, NULL, 0, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Destroying peer 0 using C++ macro didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying peer 0"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->DestroyPeer(0, NULL, 0, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Destroying peer 0 didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying peer before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Use 0x666 to avoid DPlay's player ID == 0 check.
		hr = pPeer->DestroyPeer((DPNID) 0x666, NULL, 0, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Destroying peer before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying peer with NULL buffer but non-zero size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->DestroyPeer((DPNID) 0x666, NULL, 666, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Destroying peer with NULL buffer but non-zero size return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying peer with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->DestroyPeer((DPNID) 0x666, NULL, 0, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Destroying peer with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext, ParmVPDestroyPeerDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying peer without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->DestroyPeer((DPNID) 0x666, NULL, 0, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Destroying peer without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a create player message for self.
		hostcontext.afExpectCreatePlayer[0] = TRUE;

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								(PVOID) -1,
								0);

		// Prevent any weird create player indications.
		hostcontext.afExpectCreatePlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure the create player indication came in.
		if (hostcontext.adpnidPlayers[0] == 0)
		{
			DPL(0, "Didn't get create player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player indication)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying peer with invalid player"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->DestroyPeer((DPNID) 0x666, NULL, 0, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Destroying peer with invalid player didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying peer with self player"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->DestroyPeer(hostcontext.adpnidPlayers[0], NULL, 0, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Destroying peer with self player didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a destroy player message for self.
		hostcontext.dwNumDestroyPlayersRemaining = 1;
		hostcontext.afExpectDestroyPlayer[0] = TRUE;

		hr = pPeer->Close(0);

		// Prevent any weird destroy player messages.
		hostcontext.afExpectDestroyPlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure the destroy player indication came in.
		if (hostcontext.adpnidPlayers[0] != 0)
		{
			DPL(0, "Didn't get destroy player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player indication)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPDestroyPeerDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

		// Expect a create player message for self.
		hostcontext.afExpectCreatePlayer[0] = TRUE;

		hr = PeerCreateHost(hLog, ParmVPDestroyPeerDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								(PVOID) -1, &pDP8Address);

		// Prevent any weird create player indications.
		hostcontext.afExpectCreatePlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)

		// Make sure the create player indication came in.
		if (hostcontext.adpnidPlayers[0] == 0)
		{
			DPL(0, "Didn't get create player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player indication)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a create player message for client on host.
		CREATEEVENT_OR_THROW(hostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.afExpectCreatePlayer[1] = TRUE;

		// Expect a create player message for self and host on client.
		nonhostcontext.afExpectCreatePlayer[0] = TRUE;
		nonhostcontext.afExpectCreatePlayer[1] = TRUE;

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									(PVOID) -1,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing or stray create players.
		nonhostcontext.fConnectCanComplete = FALSE;
		nonhostcontext.afExpectCreatePlayer[0] = FALSE;
		nonhostcontext.afExpectCreatePlayer[1] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for create player message on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create player messages.
		hostcontext.afExpectCreatePlayer[1] = FALSE;

#ifdef DEBUG
		// Make sure the player was created (how else would we get here?).
		if (hostcontext.adpnidPlayers[1] == 0)
		{
			DPL(0, "Create player didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (create player didn't arrive)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hLastPlayerMsgEvent);
		hostcontext.hLastPlayerMsgEvent = NULL;



		// Make sure the IDs match.
		if (hostcontext.adpnidPlayers[0] != nonhostcontext.adpnidPlayers[1])
		{
			DPL(0, "Host and client disagree on host's player ID (%u/%x != %u/%x)!",
				4, hostcontext.adpnidPlayers[0], hostcontext.adpnidPlayers[0],
				nonhostcontext.adpnidPlayers[1], nonhostcontext.adpnidPlayers[1]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (host ID doesn't match)

		if (hostcontext.adpnidPlayers[1] != nonhostcontext.adpnidPlayers[0])
		{
			DPL(0, "Host and client disagree on client's player ID (%u/%x != %u/%x)!",
				4, hostcontext.adpnidPlayers[1], hostcontext.adpnidPlayers[1],
				nonhostcontext.adpnidPlayers[0], nonhostcontext.adpnidPlayers[0]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (client ID doesn't match)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying host player on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->DestroyPeer(nonhostcontext.adpnidPlayers[1], NULL, 0, 0);
		if (hr != DPNERR_NOTHOST)
		{
			DPL(0, "Destroying host player on client didn't return expected error NOTHOST!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying self player on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->DestroyPeer(nonhostcontext.adpnidPlayers[0], NULL, 0, 0);
		if (hr != DPNERR_NOTHOST)
		{
			DPL(0, "Destroying self player on client didn't return expected error NOTHOST!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying client player on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the destroy player message on the host.
		CREATEEVENT_OR_THROW(hostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.dwNumDestroyPlayersRemaining = 1;
		hostcontext.afExpectDestroyPlayer[1] = TRUE;

		// Expect the 2 destroy player messages and terminate session on the client.
		CREATEEVENT_OR_THROW(nonhostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.dwNumDestroyPlayersRemaining = 2;
		nonhostcontext.afExpectDestroyPlayer[0] = TRUE;
		nonhostcontext.afExpectDestroyPlayer[1] = TRUE;
		nonhostcontext.fExpectTerminateSession = TRUE;

		hr = pPeerHost->DestroyPeer(hostcontext.adpnidPlayers[1], NULL, 0, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Destroying client player on host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't destroy peer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for destroy player messages on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird destroy player or terminate session messages.
		nonhostcontext.afExpectDestroyPlayer[0] = FALSE;
		nonhostcontext.afExpectDestroyPlayer[1] = FALSE;
		nonhostcontext.fExpectTerminateSession = FALSE;

#ifdef DEBUG
		// Make sure the players were destroyed (how else would we get here?).
		if (nonhostcontext.adpnidPlayers[0] != 0)
		{
			DPL(0, "Destroy player for self didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)

		if (nonhostcontext.adpnidPlayers[1] != 0)
		{
			DPL(0, "Destroy player for host didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)
#endif // DEBUG

		// Make sure we got the terminate session message.
		if (! nonhostcontext.fGotTerminateSession)
		{
			DPL(0, "Didn't get terminate session indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (terminate session didn't arrive)

		// Reset the context.
		CloseHandle(nonhostcontext.hLastPlayerMsgEvent);
		nonhostcontext.hLastPlayerMsgEvent = NULL;
		nonhostcontext.fGotTerminateSession = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for destroy player message on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird destroy player messages.
		hostcontext.afExpectDestroyPlayer[1] = FALSE;

#ifdef DEBUG
		// Make sure the player was destroyed (how else would we get here?).
		if (hostcontext.adpnidPlayers[1] != 0)
		{
			DPL(0, "Destroy player didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hLastPlayerMsgEvent);
		hostcontext.hLastPlayerMsgEvent = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a destroy player message for self.
		hostcontext.dwNumDestroyPlayersRemaining = 1;
		hostcontext.afExpectDestroyPlayer[0] = TRUE;

		hr = pPeerHost->Close(0);

		// Prevent any weird destroy player messages.
		hostcontext.afExpectDestroyPlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure the destroy player indication came in.
		if (hostcontext.adpnidPlayers[0] != 0)
		{
			DPL(0, "Didn't get destroy player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player indication)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hLastPlayerMsgEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hLastPlayerMsgEvent);


	return (hr);
} // ParmVPeerExec_DestroyPeer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_Return()"
//==================================================================================
// ParmVPeerExec_Return
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.24 - Peer ReturnBuffer parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_Return(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			hr;
	CTNTestResult			hr;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PDIRECTPLAY8ADDRESS		pDP8Address = NULL;
	DPN_APPLICATION_DESC	dpAppDesc;
	DPN_APPLICATION_DESC	dpAppDescCompare;
	DPN_BUFFER_DESC			dpnbd;
	PARMVPRETURNCONTEXT		context;



	ZeroMemory(&context, sizeof (PARMVPRETURNCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Returning NULL buffer handle using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_ReturnBuffer(pPeer->m_pDP8Peer, NULL, 0);
		if (hr != DPNERR_INVALIDHANDLE)
		{
			DPL(0, "Returning NULL buffer handle using C++ macro didn't return expected error INVALIDHANDLE!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't return buffer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Returning NULL buffer handle"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->ReturnBuffer(NULL, 0);
		if (hr != DPNERR_INVALIDHANDLE)
		{
			DPL(0, "Returning NULL buffer handle didn't return expected error INVALIDHANDLE!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't return buffer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Returning buffer with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Use 0x666 to get around DPlay's handle == NULL check.
		hr = pPeer->ReturnBuffer((DPNHANDLE) 0x666, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Returning buffer with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't return buffer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Returning buffer before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->ReturnBuffer((DPNHANDLE) 0x666, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Returning buffer before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't return buffer)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing with 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&context, ParmVPReturnDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing with 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Returning buffer without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->ReturnBuffer((DPNHANDLE) 0x666, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Returning buffer without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't return buffer)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_HOST;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Sending message to self"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnbd, sizeof (DPN_BUFFER_DESC));
		dpnbd.pBufferData = (PBYTE) "This is the send string which we will use.";
		dpnbd.dwBufferSize = strlen("This is the send string which we will use.") + 1;


		// Expect the receive to arrive.
		CREATEEVENT_OR_THROW(context.hReceivedMsgEvent,
							NULL, FALSE, FALSE, NULL);
		context.fExpectReceive = TRUE;

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->SendTo((DPNID) 0x00200002, &dpnbd, 1, 0, NULL, NULL,
									DPNSEND_SYNC);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't send message!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't send message to self)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for receive to arrive"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(context.hReceivedMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird receives.
		context.fExpectReceive = FALSE;

#ifdef DEBUG
		// Make sure we got the receive indication (how else would we get here?).
		if (context.dpnhBuffer == NULL)
		{
			DPL(0, "Didn't get send buffer!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get buffer)
#endif // DEBUG

		// Reset the context.
		CloseHandle(context.hReceivedMsgEvent);
		context.hReceivedMsgEvent = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Returning kept send buffer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->ReturnBuffer(context.dpnhBuffer, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Returning kept send buffer failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't return buffer)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(context.hReceivedMsgEvent);


	return (hr);
} // ParmVPeerExec_Return
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_GetPlayerContext()"
//==================================================================================
// ParmVPeerExec_GetPlayerContext
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.25 - Peer GetPlayerContext parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_GetPlayerContext(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					hr;
	CTNTestResult					hr;
	PWRAPDP8PEER					pDP8Peer = NULL;
	PWRAPDP8PEER					pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS				pDP8Address = NULL;
	DPN_APPLICATION_DESC			dpAppDesc;
	DPN_APPLICATION_DESC			dpAppDescCompare;
	PARMVPGETPLAYERCONTEXTCONTEXT	hostcontext;
	PARMVPGETPLAYERCONTEXTCONTEXT	nonhostcontext;
	PVOID							pvPlayerContext;



	ZeroMemory(&hostcontext, sizeof (PARMVPGETPLAYERCONTEXTCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPGETPLAYERCONTEXTCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player 0 context using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_GetPlayerContext(pPeer->m_pDP8Peer, 0, NULL, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Getting player 0 context using C++ macro didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player 0 context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPlayerContext(0, NULL, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Getting player 0 context didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context with NULL pointer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Use 0x666 to avoid DPlay's player ID == 0 check.
		hr = pPeer->GetPlayerContext((DPNID) 0x666, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting player context with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pvPlayerContext = (PVOID) 0x666;

		hr = pPeer->GetPlayerContext((DPNID) 0x666, &pvPlayerContext, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Getting player context with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)

		if (pvPlayerContext != (PVOID) 0x666)
		{
			DPL(0, "Player context was modified (%x != 0x666)!", 1, pvPlayerContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player context was touched)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPlayerContext((DPNID) 0x666, &pvPlayerContext, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Getting player context before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)

		if (pvPlayerContext != (PVOID) 0x666)
		{
			DPL(0, "Player context was modified (%x != 0x666)!", 1, pvPlayerContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player context was touched)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPGetPlayerContextDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPlayerContext((DPNID) 0x666, &pvPlayerContext, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Getting player context without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)

		if (pvPlayerContext != (PVOID) 0x666)
		{
			DPL(0, "Player context was modified (%x != 0x666)!", 1, pvPlayerContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player context was touched)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a create player message for self.
		hostcontext.afExpectCreatePlayer[0] = TRUE;

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								(PVOID) -1,
								0);

		// Prevent any weird create player indications.
		hostcontext.afExpectCreatePlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure the create player indication came in.
		if (hostcontext.adpnidPlayers[0] == 0)
		{
			DPL(0, "Didn't get create player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player indication)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context for invalid player"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPlayerContext((DPNID) 0x666, &pvPlayerContext, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Getting player context for invalid player didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)

		if (pvPlayerContext != (PVOID) 0x666)
		{
			DPL(0, "Player context was modified (%x != 0x666)!", 1, pvPlayerContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (player context was touched)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPlayerContext(hostcontext.adpnidPlayers[0],
											&pvPlayerContext, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting player context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)

		if (pvPlayerContext != (PVOID) -1)
		{
			DPL(0, "Player context returned was invalid (%x != -1)!",
				1, pvPlayerContext);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player context was touched)

		pvPlayerContext = (PVOID) 0x666;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a destroy player message for self.
		hostcontext.dwNumDestroyPlayersRemaining = 1;
		hostcontext.afExpectDestroyPlayer[0] = TRUE;

		hr = pPeer->Close(0);

		// Prevent any weird destroy player messages.
		hostcontext.afExpectDestroyPlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure the destroy player indication came in.
		if (hostcontext.adpnidPlayers[0] != 0)
		{
			DPL(0, "Didn't get destroy player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player indication)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPGetPlayerContextDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

		// Expect a create player message for self.
		hostcontext.afExpectCreatePlayer[0] = TRUE;

		hr = PeerCreateHost(hLog, ParmVPGetPlayerContextDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								(PVOID) -1, &pDP8Address);

		// Prevent any weird create player indications.
		hostcontext.afExpectCreatePlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)

		// Make sure the create player indication came in.
		if (hostcontext.adpnidPlayers[0] == 0)
		{
			DPL(0, "Didn't get create player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player indication)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a create player message for client on host.
		CREATEEVENT_OR_THROW(hostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.afExpectCreatePlayer[1] = TRUE;

		// Expect a create player message for self and host on client.
		nonhostcontext.afExpectCreatePlayer[0] = TRUE;
		nonhostcontext.afExpectCreatePlayer[1] = TRUE;

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									(PVOID) -1,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing or stray create players.
		nonhostcontext.fConnectCanComplete = FALSE;
		nonhostcontext.afExpectCreatePlayer[0] = FALSE;
		nonhostcontext.afExpectCreatePlayer[1] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for create player message on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create player messages.
		hostcontext.afExpectCreatePlayer[1] = FALSE;

#ifdef DEBUG
		// Make sure the player was created (how else would we get here?).
		if (hostcontext.adpnidPlayers[1] == 0)
		{
			DPL(0, "Create player didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (create player didn't arrive)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hLastPlayerMsgEvent);
		hostcontext.hLastPlayerMsgEvent = NULL;



		// Make sure the IDs match.
		if (hostcontext.adpnidPlayers[0] != nonhostcontext.adpnidPlayers[1])
		{
			DPL(0, "Host and client disagree on host's player ID (%u/%x != %u/%x)!",
				4, hostcontext.adpnidPlayers[0], hostcontext.adpnidPlayers[0],
				nonhostcontext.adpnidPlayers[1], nonhostcontext.adpnidPlayers[1]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (host ID doesn't match)

		if (hostcontext.adpnidPlayers[1] != nonhostcontext.adpnidPlayers[0])
		{
			DPL(0, "Host and client disagree on client's player ID (%u/%x != %u/%x)!",
				4, hostcontext.adpnidPlayers[1], hostcontext.adpnidPlayers[1],
				nonhostcontext.adpnidPlayers[0], nonhostcontext.adpnidPlayers[0]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (client ID doesn't match)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context of self on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->GetPlayerContext(hostcontext.adpnidPlayers[0],
												&pvPlayerContext, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting player context of self on host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)

		if (pvPlayerContext != (PVOID) -1)
		{
			DPL(0, "Player context returned was invalid (%x != -1)!",
				1, pvPlayerContext);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player context was touched)

		pvPlayerContext = (PVOID) 0x666;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context of client on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->GetPlayerContext(hostcontext.adpnidPlayers[1],
												&pvPlayerContext, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting player context of client on host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)

		if (pvPlayerContext != NONLOCAL_CONTEXT)
		{
			DPL(0, "Player context returned was invalid (%x != %x)!",
				2, pvPlayerContext, NONLOCAL_CONTEXT);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player context was touched)

		pvPlayerContext = (PVOID) 0x666;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context of host on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPlayerContext(nonhostcontext.adpnidPlayers[1],
											&pvPlayerContext, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting player context of host on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)

		if (pvPlayerContext != NONLOCAL_CONTEXT)
		{
			DPL(0, "Player context returned was invalid (%x != %x)!",
				2, pvPlayerContext, NONLOCAL_CONTEXT);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player context was touched)

		pvPlayerContext = (PVOID) 0x666;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting player context of self on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetPlayerContext(nonhostcontext.adpnidPlayers[0],
											&pvPlayerContext, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting player context of self on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)

		if (pvPlayerContext != (PVOID) -1)
		{
			DPL(0, "Player context returned was invalid (%x != -1)!",
				1, pvPlayerContext);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (player context was touched)

		pvPlayerContext = (PVOID) 0x666;



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the destroy player message on the host.
		CREATEEVENT_OR_THROW(hostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.dwNumDestroyPlayersRemaining = 1;
		hostcontext.afExpectDestroyPlayer[1] = TRUE;

		// Expect the 2 destroy player messages and terminate session on the client.
		CREATEEVENT_OR_THROW(nonhostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.dwNumDestroyPlayersRemaining = 2;
		nonhostcontext.afExpectDestroyPlayer[0] = TRUE;
		nonhostcontext.afExpectDestroyPlayer[1] = TRUE;

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for destroy player messages on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird destroy player or terminate session messages.
		nonhostcontext.afExpectDestroyPlayer[0] = FALSE;
		nonhostcontext.afExpectDestroyPlayer[1] = FALSE;

#ifdef DEBUG
		// Make sure the players were destroyed (how else would we get here?).
		if (nonhostcontext.adpnidPlayers[0] != 0)
		{
			DPL(0, "Destroy player for self didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)

		if (nonhostcontext.adpnidPlayers[1] != 0)
		{
			DPL(0, "Destroy player for host didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)
#endif // DEBUG

		// Reset the context.
		CloseHandle(nonhostcontext.hLastPlayerMsgEvent);
		nonhostcontext.hLastPlayerMsgEvent = NULL;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for destroy player message on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird destroy player messages.
		hostcontext.afExpectDestroyPlayer[1] = FALSE;

#ifdef DEBUG
		// Make sure the player was destroyed (how else would we get here?).
		if (hostcontext.adpnidPlayers[1] != 0)
		{
			DPL(0, "Destroy player didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hLastPlayerMsgEvent);
		hostcontext.hLastPlayerMsgEvent = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a destroy player message for self.
		hostcontext.dwNumDestroyPlayersRemaining = 1;
		hostcontext.afExpectDestroyPlayer[0] = TRUE;

		hr = pPeerHost->Close(0);

		// Prevent any weird destroy player messages.
		hostcontext.afExpectDestroyPlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

		// Make sure the destroy player indication came in.
		if (hostcontext.adpnidPlayers[0] != 0)
		{
			DPL(0, "Didn't get destroy player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get destroy player indication)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);


	return (hr);
} // ParmVPeerExec_GetPlayerContext
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_GetGroupContext()"
//==================================================================================
// ParmVPeerExec_GetGroupContext
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.26 - Peer GetGroupContext parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_GetGroupContext(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					hr;
	CTNTestResult					hr;
	PWRAPDP8PEER					pDP8Peer = NULL;
	PWRAPDP8PEER					pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS				pDP8Address = NULL;
	DPN_APPLICATION_DESC			dpAppDesc;
	DPN_APPLICATION_DESC			dpAppDescCompare;
	PARMVPGETGROUPCONTEXTCONTEXT	hostcontext;
	PARMVPGETGROUPCONTEXTCONTEXT	nonhostcontext;
	DPN_GROUP_INFO					dpGroupInfo;
	DPN_GROUP_INFO					dpGroupInfoCompare;
	PVOID							pvGroupContext;



	ZeroMemory(&hostcontext, sizeof (PARMVPGETGROUPCONTEXTCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPGETGROUPCONTEXTCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group 0 context using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_GetGroupContext(pPeer->m_pDP8Peer, 0, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Getting group 0 context using C++ macro didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get player context)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group 0 context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetGroupContext(0, NULL, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Getting group 0 context didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group context with NULL pointer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Use 0x666 to avoid DPlay's group ID == 0 check.
		hr = pPeer->GetGroupContext((DPNID) 0x666, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting group context with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group context with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pvGroupContext = (PVOID) 0x666;

		hr = pPeer->GetGroupContext((DPNID) 0x666, &pvGroupContext, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Getting group context with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)

		if (pvGroupContext != (PVOID) 0x666)
		{
			DPL(0, "Group context was modified (%x != 0x666)!", 1, pvGroupContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group context was touched)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group context before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetGroupContext((DPNID) 0x666, &pvGroupContext, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Getting group context before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)

		if (pvGroupContext != (PVOID) 0x666)
		{
			DPL(0, "Group context was modified (%x != 0x666)!", 1, pvGroupContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group context was touched)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPGetGroupContextDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group context without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetGroupContext((DPNID) 0x666, &pvGroupContext, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Getting group context without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)

		if (pvGroupContext != (PVOID) 0x666)
		{
			DPL(0, "Group context was modified (%x != 0x666)!", 1, pvGroupContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group context was touched)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_HOST;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group context with local player ID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->GetGroupContext((DPNID) 0x00200002, &pvGroupContext, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Getting group context with local player ID didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)

		if (pvGroupContext != (PVOID) 0x666)
		{
			DPL(0, "Group context was modified (%x != 0x666)!", 1, pvGroupContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group context was touched)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group context with all players group internal ID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Determine real ID")
		hr = pPeer->GetGroupContext((DPNID) 0x00100001, &pvGroupContext, 0);
		if (hr != DPNERR_INVALIDGROUP)
		{
			DPL(0, "Getting group context with all players group internal ID didn't return expected error INVALIDGROUP!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)

		if (pvGroupContext != (PVOID) 0x666)
		{
			DPL(0, "Group context was modified (%x != 0x666)!", 1, pvGroupContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group context was touched)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpGroupInfo, sizeof (DPN_GROUP_INFO));
		dpGroupInfo.dwSize = sizeof (DPN_GROUP_INFO);
		//dpGroupInfo.dwInfoFlags = 0;
		//dpGroupInfo.pwszName = NULL;
		//dpGroupInfo.pvData = NULL;
		//dpGroupInfo.dwDataSize = 0;
		//dpGroupInfo.dwGroupFlags = 0;

		CopyMemory(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO));


		// Expect the create group message.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeer->CreateGroup(&dpGroupInfo, (PVOID) -1, NULL, NULL,
										DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetGroupContext(hostcontext.dpnidGroup, &pvGroupContext, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting group context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)

		if (pvGroupContext != (PVOID) -1)
		{
			DPL(0, "Group context was modified (%x != -1)!", 1, pvGroupContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group context was touched)

		pvGroupContext = (PVOID) 0x666;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)

#pragma TODO(vanceo, "Validate destroy group nametable unwind")

		// Reset group ID.
		hostcontext.dpnidGroup = 0;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPGetGroupContextDPNMessageHandler,
										0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPGetGroupContextDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating group on host synchronously"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the create group message on the host.
#pragma BUGBUG(vanceo, "Get real player ID")
		hostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		hostcontext.fCanGetCreateGroup = TRUE;

		// Expect the create group message on the client.
		nonhostcontext.dpnidExpectedGroupOwner = (DPNID) 0x00200002;
		CREATEEVENT_OR_THROW(nonhostcontext.hGotGroupMsgEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.fCanGetCreateGroup = TRUE;

		hr = pPeerHost->CreateGroup(&dpGroupInfo, (PVOID) -1, NULL, NULL,
											DPNCREATEGROUP_SYNC);

		// Prevent any weird create groups.
		hostcontext.fCanGetCreateGroup = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating group on host synchronously failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create group)

		if (memcmp(&dpGroupInfoCompare, &dpGroupInfo, sizeof (DPN_GROUP_INFO)) != 0)
		{
			DPL(0, "Group info structure was modified (structure at %x != structure at %x)!",
				2, &dpGroupInfoCompare, &dpGroupInfo);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group info changed)

		// Make sure we got the create group message.
		if (hostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group message on client!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create group)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for group to be created on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hGotGroupMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create group indications.
		nonhostcontext.fCanGetCreateGroup = FALSE;

#ifdef DEBUG
		// Make sure we got the indication (how else would we get here?).
		if (nonhostcontext.dpnidGroup == 0)
		{
			DPL(0, "Didn't get create group indication!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (didn't get create group indication)
#endif // DEBUG

		// Reset the context.
		CloseHandle(nonhostcontext.hGotGroupMsgEvent);
		nonhostcontext.hGotGroupMsgEvent = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group context on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->GetGroupContext(hostcontext.dpnidGroup, &pvGroupContext, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting group context on host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)

		if (pvGroupContext != (PVOID) -1)
		{
			DPL(0, "Group context was modified (%x != -1)!", 1, pvGroupContext);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group context was touched)

		pvGroupContext = (PVOID) 0x666;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting group context on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetGroupContext(nonhostcontext.dpnidGroup, &pvGroupContext, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting group context on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get group context)

		if (pvGroupContext != NONLOCAL_CONTEXT)
		{
			DPL(0, "Group context was modified (%x != %x)!",
				2, pvGroupContext, NONLOCAL_CONTEXT);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (group context was touched)

		pvGroupContext = (PVOID) 0x666;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate destroy group nametable unwind on both interfaces")



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hGotGroupMsgEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hGotGroupMsgEvent);


	return (hr);
} // ParmVPeerExec_GetGroupContext
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_GetCaps()"
//==================================================================================
// ParmVPeerExec_GetCaps
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.27 - Peer GetCaps parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_GetCaps(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			hr;
	CTNTestResult			hr;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PDIRECTPLAY8ADDRESS		pDP8Address = NULL;
	DPN_APPLICATION_DESC	dpAppDesc;
	DPN_APPLICATION_DESC	dpAppDescCompare;
	DPN_CAPS				dpncaps;
	DPN_CAPS				dpncapsCompare;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting caps with NULL pointer using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_GetCaps(pPeer->m_pDP8Peer, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting caps with NULL pointer using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting caps with NULL pointer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetCaps(NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting caps with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting caps with 0 size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->GetCaps(&dpncaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting caps with 0 size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting caps with too large size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS) + 1;

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->GetCaps(&dpncaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting caps with too large size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting caps with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS);

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->GetCaps(&dpncaps, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Getting caps with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting caps before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetCaps(&dpncaps, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Getting caps before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(NULL, PeerNewPlayerHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting caps"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetCaps(&dpncaps, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting caps failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (dpncaps.dwSize != sizeof (DPN_CAPS))
		{
			DPL(0, "Caps structure size was modified (%u != %u)!",
				2, dpncaps.dwSize, sizeof (DPN_CAPS));
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size changed)

		if (dpncaps.dwFlags != 0)
		{
			DPL(0, "Flags returned are wrong (%x != 0)!",
				1, dpncaps.dwFlags);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (flags wrong)

		if (dpncaps.dwConnectTimeout != 100)
		{
			DPL(0, "Connect timeout returned is wrong (%u != 100)!",
				1, dpncaps.dwConnectTimeout);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (connect timeout wrong)

		if (dpncaps.dwConnectRetries != 8)
		{
			DPL(0, "Connect retries returned is wrong (%u != 8)!",
				1, dpncaps.dwConnectRetries);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (connect retries wrong)

		if (dpncaps.dwTimeoutUntilKeepAlive != 60000)
		{
			DPL(0, "Timeout until keepalive returned is wrong (%u != 60000)!",
				1, dpncaps.dwTimeoutUntilKeepAlive);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (timeout until keepalive wrong)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting caps"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS);

		hr = pPeer->GetCaps(&dpncaps, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting caps failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (dpncaps.dwSize != sizeof (DPN_CAPS))
		{
			DPL(0, "Caps structure size was modified (%u != %u)!",
				2, dpncaps.dwSize, sizeof (DPN_CAPS));
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size changed)

		if (dpncaps.dwFlags != 0)
		{
			DPL(0, "Flags returned are wrong (%x != 0)!",
				1, dpncaps.dwFlags);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (flags wrong)

		if (dpncaps.dwConnectTimeout != 100)
		{
			DPL(0, "Connect timeout returned is wrong (%u != 100)!",
				1, dpncaps.dwConnectTimeout);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (connect timeout wrong)

		if (dpncaps.dwConnectRetries != 8)
		{
			DPL(0, "Connect retries returned is wrong (%u != 8)!",
				1, dpncaps.dwConnectRetries);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (connect retries wrong)

		if (dpncaps.dwTimeoutUntilKeepAlive != 60000)
		{
			DPL(0, "Timeout until keepalive returned is wrong (%u != 60000)!",
				1, dpncaps.dwTimeoutUntilKeepAlive);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (timeout until keepalive wrong)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate nametable unwind")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);


	return (hr);
} // ParmVPeerExec_GetCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_SetCaps()"
//==================================================================================
// ParmVPeerExec_SetCaps
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.28 - Peer SetCaps parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_SetCaps(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			hr;
	CTNTestResult			hr;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PDIRECTPLAY8ADDRESS		pDP8Address = NULL;
	DPN_APPLICATION_DESC	dpAppDesc;
	DPN_APPLICATION_DESC	dpAppDescCompare;
	DPN_CAPS				dpncaps;
	DPN_CAPS				dpncapsCompare;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps with NULL pointer and 0 flags using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_SetCaps(pPeer->m_pDP8Peer, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Setting caps with NULL pointer and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps with NULL pointer and 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetCaps(NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Setting caps with NULL pointer and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps with 0 size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		//dpncaps.dwFlags = 0;
		//dpncaps.dwConnectTimeout = 0;
		//dpncaps.dwConnectRetries = 0;
		//dpncaps.dwTimeoutUntilKeepAlive = 0;

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->SetCaps(&dpncaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting caps with 0 size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps with too-large size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS) + 1;
		//dpncaps.dwFlags = 0;
		//dpncaps.dwConnectTimeout = 0;
		//dpncaps.dwConnectRetries = 0;
		//dpncaps.dwTimeoutUntilKeepAlive = 0;

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->SetCaps(&dpncaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting caps with too-large size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS);
		//dpncaps.dwFlags = 0;
		//dpncaps.dwConnectTimeout = 0;
		//dpncaps.dwConnectRetries = 0;
		//dpncaps.dwTimeoutUntilKeepAlive = 0;

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->SetCaps(&dpncaps, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Setting caps with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetCaps(&dpncaps, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Setting caps before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(NULL, PeerNewPlayerHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)



#pragma WAITFORDEVFIX(vanceo, ?, ?, "Validation on these values?")
		/*
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps with invalid caps flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS);
		dpncaps.dwFlags = 0x666;
		//dpncaps.dwConnectTimeout = 0;
		//dpncaps.dwConnectRetries = 0;
		//dpncaps.dwTimeoutUntilKeepAlive = 0;

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->SetCaps(&dpncaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting caps with invalid caps flags didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps with connect timeout set to 0"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS);
		//dpncaps.dwFlags = 0;
		//dpncaps.dwConnectTimeout = 0;
		//dpncaps.dwConnectRetries = 0;
		//dpncaps.dwTimeoutUntilKeepAlive = 0;

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->SetCaps(&dpncaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting caps with connect timeout set to 0 didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps with connect retries set to 0"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS);
		//dpncaps.dwFlags = 0;
		dpncaps.dwConnectTimeout = 100;
		//dpncaps.dwConnectRetries = 0;
		//dpncaps.dwTimeoutUntilKeepAlive = 0;

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->SetCaps(&dpncaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting caps with connect retries set to 0 didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps with timeout until keepalive set to 0"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS);
		//dpncaps.dwFlags = 0;
		dpncaps.dwConnectTimeout = 100;
		dpncaps.dwConnectRetries = 8;
		//dpncaps.dwTimeoutUntilKeepAlive = 0;

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->SetCaps(&dpncaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting caps with timeout until keepalive set to 0 didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)
		*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpncaps, sizeof (DPN_CAPS));
		dpncaps.dwSize = sizeof (DPN_CAPS);
		//dpncaps.dwFlags = 0;
		dpncaps.dwConnectTimeout = 100;
		dpncaps.dwConnectRetries = 8;
		dpncaps.dwTimeoutUntilKeepAlive = 60000;

		CopyMemory(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS));

		hr = pPeer->SetCaps(&dpncaps, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting caps failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting caps after hosting"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetCaps(&dpncaps, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting caps after hosting failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpncapsCompare, &dpncaps, sizeof (DPN_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpncapsCompare, &dpncaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate nametable unwind")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);


	return (hr);
} // ParmVPeerExec_SetCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_SetSPCaps()"
//==================================================================================
// ParmVPeerExec_SetSPCaps
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.29 - Peer SetSPCaps parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_SetSPCaps(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			hr;
	CTNTestResult			hr;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PDIRECTPLAY8ADDRESS		pDP8Address = NULL;
	DPN_APPLICATION_DESC	dpAppDesc;
	DPN_APPLICATION_DESC	dpAppDescCompare;
	DPN_SP_CAPS				dpnspcaps;
	DPN_SP_CAPS				dpnspcapsCompare;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps with NULL pointers and 0 flags using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_SetSPCaps(pPeer->m_pDP8Peer, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Setting SP caps with NULL pointers and 0 flags using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps with NULL pointers and 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetSPCaps(NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Setting SP caps with NULL pointers and 0 flags didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps with NULL caps pointer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetSPCaps(&CLSID_DP8SP_TCPIP, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Setting SP caps with NULL caps pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps with 0 size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		//dpncaps.dwFlags = 0;
		//dpncaps.dwNumThreads = 0;
		//dpncaps.dwDefaultEnumCount = 0;
		//dpncaps.dwDefaultEnumRetryInterval = 0;
		//dpncaps.dwDefaultEnumTimeout = 0;
		//dpncaps.dwMaxEnumPayloadSize = 0;

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting SP caps with 0 size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps with too-large size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		dpnspcaps.dwSize = sizeof (DPN_SP_CAPS) + 1;
		//dpncaps.dwFlags = 0;
		//dpncaps.dwNumThreads = 0;
		//dpncaps.dwDefaultEnumCount = 0;
		//dpncaps.dwDefaultEnumRetryInterval = 0;
		//dpncaps.dwDefaultEnumTimeout = 0;
		//dpncaps.dwMaxEnumPayloadSize = 0;

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting SP caps with too-large size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		dpnspcaps.dwSize = sizeof (DPN_SP_CAPS);
		//dpncaps.dwFlags = 0;
		//dpncaps.dwNumThreads = 0;
		//dpncaps.dwDefaultEnumCount = 0;
		//dpncaps.dwDefaultEnumRetryInterval = 0;
		//dpncaps.dwDefaultEnumTimeout = 0;
		//dpncaps.dwMaxEnumPayloadSize = 0;

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Setting SP caps with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Setting SP caps before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(NULL, PeerNewPlayerHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)



#pragma WAITFORDEVFIX(vanceo, ?, ?, "Validate these values?")
		/*
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps with invalid caps flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		dpnspcaps.dwSize = sizeof (DPN_SP_CAPS);
		dpnspcaps.dwFlags = 0x666;
		//dpnspcaps.dwNumThreads = 0;
		//dpnspcaps.dwDefaultEnumCount = 0;
		//dpnspcaps.dwDefaultEnumRetryInterval = 0;
		//dpnspcaps.dwDefaultEnumTimeout = 0;
		//dpnspcaps.dwMaxEnumPayloadSize = 0;

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Setting SP caps with invalid caps flags didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps with num threads set to 0"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		dpnspcaps.dwSize = sizeof (DPN_SP_CAPS);
		//dpnspcaps.dwFlags = 0;
		//dpnspcaps.dwNumThreads = 0;
		//dpnspcaps.dwDefaultEnumCount = 0;
		//dpnspcaps.dwDefaultEnumRetryInterval = 0;
		//dpnspcaps.dwDefaultEnumTimeout = 0;
		//dpnspcaps.dwMaxEnumPayloadSize = 0;

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting SP caps with num threads set to 0 failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)
		*/




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		dpnspcaps.dwSize = sizeof (DPN_SP_CAPS);
		//dpnspcaps.dwFlags = 0;

		// NT has (2 * num procs) + 2 threads, Win9x always has 2.
		if (pTNecd->pMachineInfo->IsNTBasedOS())
			dpnspcaps.dwNumThreads = (2 * pTNecd->pMachineInfo->m_dwNumberOfProcessors) + 2;
		else
			dpnspcaps.dwNumThreads = 2;

		//dpnspcaps.dwDefaultEnumCount = 0;
		//dpnspcaps.dwDefaultEnumRetryInterval = 0;
		//dpnspcaps.dwDefaultEnumTimeout = 0;
		//dpnspcaps.dwMaxEnumPayloadSize = 0;

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting SP caps failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps for invalid SP GUID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetSPCaps(&GUID_UNKNOWN, &dpnspcaps, 0);
		if (hr != DPNERR_DOESNOTEXIST)
		{
			DPL(0, "Setting SP caps didn't return expected error DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting SP caps after hosting"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->SetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting SP caps after hosting failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate nametable unwind")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);


	return (hr);
} // ParmVPeerExec_SetSPCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_GetSPCaps()"
//==================================================================================
// ParmVPeerExec_GetSPCaps
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.30 - Peer GetSPCaps parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_GetSPCaps(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			hr;
	CTNTestResult			hr;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PDIRECTPLAY8ADDRESS		pDP8Address = NULL;
	DPN_APPLICATION_DESC	dpAppDesc;
	DPN_APPLICATION_DESC	dpAppDescCompare;
	DPN_SP_CAPS				dpnspcaps;
	DPN_SP_CAPS				dpnspcapsCompare;



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps with NULL pointers using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_GetSPCaps(pPeer->m_pDP8Peer, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting SP caps with NULL pointers using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps with NULL pointers"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetSPCaps(NULL, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting SP caps with NULL pointers didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps with NULL GUID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		dpnspcaps.dwSize = sizeof (DPN_SP_CAPS);

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->GetSPCaps(NULL, &dpnspcaps, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting SP caps with NULL GUID didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps with NULL caps"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetSPCaps(&CLSID_DP8SP_TCPIP, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting SP caps with NULL caps didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps with 0 size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->GetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting SP caps with 0 size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps with too large size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		dpnspcaps.dwSize = sizeof (DPN_SP_CAPS) + 1;

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->GetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting SP caps with too large size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		dpnspcaps.dwSize = sizeof (DPN_SP_CAPS);

		CopyMemory(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS));

		hr = pPeer->GetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Getting SP caps with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Getting SP caps before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(NULL, PeerNewPlayerHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps with invalid SP GUID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetSPCaps(&GUID_UNKNOWN, &dpnspcaps, 0);
		if (hr != DPNERR_DOESNOTEXIST)
		{
			DPL(0, "Getting SP caps with invalid SP GUID didn't return expected error DOESNOTEXIST!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (memcmp(&dpnspcapsCompare, &dpnspcaps, sizeof (DPN_SP_CAPS)) != 0)
		{
			DPL(0, "Caps structure was modified (structure at %x != structure at %x)!",
				2, &dpnspcapsCompare, &dpnspcaps);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (caps changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting SP caps failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (dpnspcaps.dwSize != sizeof (DPN_SP_CAPS))
		{
			DPL(0, "Caps structure size was modified (%u != %u)!",
				2, dpnspcaps.dwSize, sizeof (DPN_SP_CAPS));
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size changed)

		if (dpnspcaps.dwFlags != (DPNSPCAPS_SUPPORTSDPNSRV | DPNSPCAPS_SUPPORTSBROADCAST | DPNSPCAPS_SUPPORTSALLADAPTERS))
		{
			DPL(0, "Flags returned are wrong (%x != %x)!",
				2, dpnspcaps.dwFlags,
				(DPNSPCAPS_SUPPORTSDPNSRV | DPNSPCAPS_SUPPORTSBROADCAST | DPNSPCAPS_SUPPORTSALLADAPTERS));
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (flags wrong)

		if (pTNecd->pMachineInfo->IsNTBasedOS())
		{
			if (dpnspcaps.dwNumThreads != ((2 * pTNecd->pMachineInfo->m_dwNumberOfProcessors) + 2))
			{
				DPL(0, "Num threads returned is wrong (%u != %u)!",
					2, dpnspcaps.dwNumThreads,
					((2 * pTNecd->pMachineInfo->m_dwNumberOfProcessors) + 2));
				SETTHROW_TESTRESULT(ERROR_NO_MATCH);
			} // end if (num threads wrong)
		} // end if (NT)
		else
		{
			if (dpnspcaps.dwNumThreads != 2)
			{
				DPL(0, "Num threads returned is wrong (%u != 2)!",
					1, dpnspcaps.dwNumThreads);
				SETTHROW_TESTRESULT(ERROR_NO_MATCH);
			} // end if (num threads wrong)
		} // end else (Win9x)

		if (dpnspcaps.dwDefaultEnumCount != 5)
		{
			DPL(0, "Default enum count returned is wrong (%u != 5)!",
				1, dpnspcaps.dwDefaultEnumCount);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (default enum count wrong)

		if (dpnspcaps.dwDefaultEnumRetryInterval != 1500)
		{
			DPL(0, "Default enum retry interval returned is wrong (%u != 1500)!",
				1, dpnspcaps.dwDefaultEnumRetryInterval);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (default enum retry interval wrong)

		if (dpnspcaps.dwDefaultEnumTimeout != 1500)
		{
			DPL(0, "Default enum timeout returned is wrong (%u != 1500)!",
				1, dpnspcaps.dwDefaultEnumTimeout);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (default enum timeout wrong)

		if (dpnspcaps.dwMaxEnumPayloadSize != 983)
		{
			DPL(0, "Default max enum payload size returned is wrong (%u != 983)!",
				1, dpnspcaps.dwMaxEnumPayloadSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (max enum payload wrong)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting SP caps"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnspcaps, sizeof (DPN_SP_CAPS));
		dpnspcaps.dwSize = sizeof (DPN_SP_CAPS);

		hr = pPeer->GetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspcaps, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting SP caps failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get caps)

		if (dpnspcaps.dwSize != sizeof (DPN_SP_CAPS))
		{
			DPL(0, "Caps structure size was modified (%u != %u)!",
				2, dpnspcaps.dwSize, sizeof (DPN_SP_CAPS));
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (size changed)

		if (dpnspcaps.dwFlags != (DPNSPCAPS_SUPPORTSDPNSRV | DPNSPCAPS_SUPPORTSBROADCAST | DPNSPCAPS_SUPPORTSALLADAPTERS))
		{
			DPL(0, "Flags returned are wrong (%x != %x)!",
				2, dpnspcaps.dwFlags,
				(DPNSPCAPS_SUPPORTSDPNSRV | DPNSPCAPS_SUPPORTSBROADCAST | DPNSPCAPS_SUPPORTSALLADAPTERS));
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (flags wrong)

		if (pTNecd->pMachineInfo->IsNTBasedOS())
		{
			if (dpnspcaps.dwNumThreads != ((2 * pTNecd->pMachineInfo->m_dwNumberOfProcessors) + 2))
			{
				DPL(0, "Num threads returned is wrong (%u != %u)!",
					2, dpnspcaps.dwNumThreads,
					((2 * pTNecd->pMachineInfo->m_dwNumberOfProcessors) + 2));
				SETTHROW_TESTRESULT(ERROR_NO_MATCH);
			} // end if (num threads wrong)
		} // end if (NT)
		else
		{
			if (dpnspcaps.dwNumThreads != 2)
			{
				DPL(0, "Num threads returned is wrong (%u != 2)!",
					1, dpnspcaps.dwNumThreads);
				SETTHROW_TESTRESULT(ERROR_NO_MATCH);
			} // end if (num threads wrong)
		} // end else (Win9x)

		if (dpnspcaps.dwDefaultEnumCount != 5)
		{
			DPL(0, "Default enum count returned is wrong (%u != 5)!",
				1, dpnspcaps.dwDefaultEnumCount);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (default enum count wrong)

		if (dpnspcaps.dwDefaultEnumRetryInterval != 1500)
		{
			DPL(0, "Default enum retry interval returned is wrong (%u != 1500)!",
				1, dpnspcaps.dwDefaultEnumRetryInterval);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (default enum retry interval wrong)

		if (dpnspcaps.dwDefaultEnumTimeout != 1500)
		{
			DPL(0, "Default enum timeout returned is wrong (%u != 1500)!",
				1, dpnspcaps.dwDefaultEnumTimeout);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (default enum timeout wrong)

		if (dpnspcaps.dwMaxEnumPayloadSize != 983)
		{
			DPL(0, "Default max enum payload size returned is wrong (%u != 983)!",
				1, dpnspcaps.dwMaxEnumPayloadSize);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (max enum payload wrong)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)



#pragma TODO(vanceo, "Validate nametable unwind")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);


	return (hr);
} // ParmVPeerExec_GetSPCaps
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_GetConnInfo()"
//==================================================================================
// ParmVPeerExec_GetConnInfo
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.31 - Peer GetConnectionInfo parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_GetConnInfo(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult				hr;
	CTNTestResult				hr;
	PWRAPDP8PEER				pDP8Peer = NULL;
	PWRAPDP8PEER				pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS			pDP8Address = NULL;
	DPN_CONNECTION_INFO			dpnci;
	DPN_CONNECTION_INFO			dpnciCompare;
	DPN_APPLICATION_DESC		dpAppDesc;
	DPN_APPLICATION_DESC		dpAppDescCompare;
	PARMVPGETCONNINFOCONTEXT	hostcontext;
	PARMVPGETCONNINFOCONTEXT	nonhostcontext;



	ZeroMemory(&hostcontext, sizeof (PARMVPGETCONNINFOCONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPGETCONNINFOCONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info with NULL pointer using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_GetConnectionInfo(pPeer->m_pDP8Peer, 0, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting connection info with NULL pointer using C++ macro didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info with NULL pointer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetConnectionInfo(0, NULL, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Getting connection info with NULL pointer didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info with 0 size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnci, sizeof (DPN_CONNECTION_INFO));
		//dpnci.dwSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO));

		hr = pPeer->GetConnectionInfo(0, &dpnci, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting connection info with 0 size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPL(0, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info with too-large size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnci, sizeof (DPN_CONNECTION_INFO));
		dpnci.dwSize = sizeof (DPN_CONNECTION_INFO) + 1;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO));

		hr = pPeer->GetConnectionInfo(0, &dpnci, 0);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Getting connection info with too-large size didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPL(0, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpnci, sizeof (DPN_CONNECTION_INFO));
		dpnci.dwSize = sizeof (DPN_CONNECTION_INFO);

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO));

		hr = pPeer->GetConnectionInfo(0, &dpnci, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Getting connection info with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPL(0, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetConnectionInfo(0, &dpnci, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Getting connection info before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPL(0, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext,
										ParmVPGetSendQInfoDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetConnectionInfo(0, &dpnci, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Getting connection info without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPL(0, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_SENDTO;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								NULL,
								0);
		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info for 0"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetConnectionInfo(0, &dpnci, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Getting connection info for 0 didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPL(0, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info for invalid player ID"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->GetConnectionInfo((DPNID) 0x666, &dpnci, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Getting connection info for invalid player ID didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPL(0, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info for local player"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->GetConnectionInfo((DPNID) 0x00200002, &dpnci, 0);
		if (hr != DPNERR_INVALIDPLAYER)
		{
			DPL(0, "Getting connection info for local player didn't return expected error INVALIDPLAYER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPL(0, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPGetSendQInfoDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;



#pragma TODO(vanceo, "Validate CREATE_PLAYER and DESTROY_PLAYER")
		hr = PeerCreateHost(hLog, ParmVPGetSendQInfoDPNMessageHandler, &hostcontext,
								&dpAppDesc, &pDP8PeerHost, NULL, &pDP8Address);
		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing.
		nonhostcontext.fConnectCanComplete = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Getting connection info of host on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma BUGBUG(vanceo, "Get real player ID")
		hr = pPeer->GetConnectionInfo((DPNID) 0x00200002, &dpnci, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Getting connection info of host on client failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't get connection info)

#pragma TODO(vanceo, "Validate returned structure")
		/*
		if (memcmp(&dpnciCompare, &dpnci, sizeof (DPN_CONNECTION_INFO)) != 0)
		{
			DPL(0, "Connection info structure was modified (structure at %x != structure at %x)!",
				2, &dpnciCompare, &dpnci);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (connection info changed)
		*/





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);


	return (hr);
} // ParmVPeerExec_GetConnInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_RegLobby()"
//==================================================================================
// ParmVPeerExec_RegLobby
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.32 - Peer RegisterLobby parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_RegLobby(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult					hr;
	CTNTestResult					hr;
	PWRAPDP8PEER					pDP8Peer = NULL;
#ifndef _XBOX
	PDIRECTPLAY8LOBBIEDAPPLICATION	pDP8LobbiedApp = NULL;
#endif



	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = CoCreateInstance(CLSID_DirectPlay8LobbiedApplication, NULL,
							CLSCTX_INPROC_SERVER, IID_IDirectPlay8LobbiedApplication,
							(LPVOID*) (&pDP8LobbiedApp));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)
#endif




		// We're going to only do minimal testing on this API.  I will leave it up
		// to lobby tests to cover this appropriately.


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Registering lobby with 0 flags using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_RegisterLobby(pPeer->m_pDP8Peer, NULL, NULL, 0);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Registering lobby with 0 flags using C++ macro didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Registering lobby with 0 flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby(NULL, NULL, 0);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Registering lobby with 0 flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Registering lobby with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby(NULL, pDP8LobbiedApp, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Registering lobby with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Registering lobby with both REGISTER & UNREGISTER"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby(NULL, pDP8LobbiedApp,
										DPNLOBBY_REGISTER | DPNLOBBY_UNREGISTER);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Registering lobby with both REGISTER & UNREGISTER didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Registering lobby with NULL handle"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby(NULL, pDP8LobbiedApp, DPNLOBBY_REGISTER);
		if (hr != DPNERR_INVALIDHANDLE)
		{
			DPL(0, "Registering lobby with NULL handle didn't return expected error INVALIDHANDLE!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Unregistering lobby with non-NULL handle"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby((DPNHANDLE) 0x666, NULL, DPNLOBBY_UNREGISTER);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Unregistering lobby with non-NULL handle didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Registering lobby with NULL interface"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby((DPNHANDLE) 0x666, NULL, DPNLOBBY_REGISTER);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Registering lobby with NULL interface didn't return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Unregistering lobby with non-NULL interface"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby(NULL, pDP8LobbiedApp, DPNLOBBY_UNREGISTER);
		if (hr != DPNERR_INVALIDPARAM)
		{
			DPL(0, "Unregistering lobby with non-NULL interface didn't return expected error INVALIDPARAM!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Registering lobby before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		// Use 0x666 to get around handle == NULL check
		hr = pPeer->RegisterLobby((DPNHANDLE) 0x666, pDP8LobbiedApp,
											DPNLOBBY_REGISTER);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Registering lobby before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Unregistering lobby before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		// Use 0x666 to get around handle == NULL check
		hr = pPeer->RegisterLobby(NULL, NULL, DPNLOBBY_UNREGISTER);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Unregistering lobby before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->Initialize(NULL, PeerNewPlayerHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)
#endif


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Unregistering lobby before registering"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby(NULL, NULL, DPNLOBBY_UNREGISTER);
		if (hr != DPNERR_NOTREGISTERED)
		{
			DPL(0, "Unregistering lobby before registering didn't fail with expected error NOTREGISTERED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Registering lobby"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby((DPNHANDLE) 0x666, pDP8LobbiedApp,
											DPNLOBBY_REGISTER);
		if (hr != DPN_OK)
		{
			DPL(0, "Registering lobby failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Registering lobby again"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby((DPNHANDLE) 0x666, pDP8LobbiedApp,
										DPNLOBBY_REGISTER);
		if (hr != DPNERR_ALREADYREGISTERED)
		{
			DPL(0, "Registering lobby again didn't fail with expected error ALREADYREGISTERED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Unregistering lobby"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby(NULL, NULL, DPNLOBBY_UNREGISTER);
		if (hr != DPN_OK)
		{
			DPL(0, "Unregistering lobby failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Unregistering lobby again"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef _XBOX
		hr = pPeer->RegisterLobby(NULL, NULL, DPNLOBBY_UNREGISTER);
		if (hr != DPNERR_NOTREGISTERED)
		{
			DPL(0, "Unregistering lobby again didn't fail with expected error NOTREGISTERED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't register lobby)
#endif




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;



		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

#ifndef _XBOX
	SAFE_RELEASE(pDP8LobbiedApp);
#endif


	return (hr);
} // ParmVPeerExec_RegLobby
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVPeerExec_Terminate()"
//==================================================================================
// ParmVPeerExec_Terminate
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				2.1.1.33 - Peer TerminateSession parameter validation
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data: None.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ParmVPeerExec_Terminate(PTNEXECCASEDATA pTNecd)
{
	CTNSystemResult			hr;
	CTNTestResult			hr;
	PWRAPDP8PEER			pDP8Peer = NULL;
	PWRAPDP8PEER			pDP8PeerHost = NULL;
	PDIRECTPLAY8ADDRESS		pDP8Address = NULL;
	DPN_APPLICATION_DESC	dpAppDesc;
	DPN_APPLICATION_DESC	dpAppDescCompare;
	PARMVPTERMINATECONTEXT	hostcontext;
	PARMVPTERMINATECONTEXT	nonhostcontext;



	ZeroMemory(&hostcontext, sizeof (PARMVPTERMINATECONTEXT));
	ZeroMemory(&nonhostcontext, sizeof (PARMVPTERMINATECONTEXT));


	BEGIN_TESTCASE
	{
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Peer = new (CWrapDP8Peer);
		if (pDP8Peer == NULL)
		{
			SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);
		} // end if (couldn't allocate object)

		hr = pPeer->CoCreate();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't cocreate)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "CoCreating DirectPlay8Address object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC_SERVER,
							IID_IDirectPlay8Address, (LPVOID*) (&pDP8Address));

		if (hr != S_OK)
		{
			DPL(0, "Couldn't CoCreate DirectPlay8Address object!", 0);
			THROW_SYSTEMRESULT;
		} // end if (CoCreate failed)



		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Setting address object's SP to TCP/IP"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pDP8Address->SetSP(&CLSID_DP8SP_TCPIP);
		if (hr != DPN_OK)
		{
			DPL(0, "Setting address object's SP to TCP/IP failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't set SP)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Terminating session before initialization using C++ macro"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = IDirectPlay8Peer_TerminateSession(pPeer->m_pDP8Peer, NULL, 0, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Terminating session before initialization using C++ macro didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't terminate session)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Terminating session before initialization"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->TerminateSession(NULL, 0, 0);
		if (hr != DPNERR_UNINITIALIZED)
		{
			DPL(0, "Terminating session before initialization didn't return expected error UNINITIALIZED!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't terminate session)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Destroying peer with NULL buffer but non-zero size"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->TerminateSession(NULL, 666, 0);
		if (hr != DPNERR_INVALIDPOINTER)
		{
			DPL(0, "Destroying peer with NULL buffer but non-zero size return expected error INVALIDPOINTER!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't terminate session)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Terminating session with invalid flags"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->TerminateSession(NULL, 0, 0x666);
		if (hr != DPNERR_INVALIDFLAGS)
		{
			DPL(0, "Terminating session with invalid flags didn't return expected error INVALIDFLAGS!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't terminate session)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Initializing peer object with host context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&hostcontext, ParmVPTerminateDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Initializing peer object with host context failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Terminating session without a connection"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->TerminateSession(NULL, 0, 0);
		if (hr != DPNERR_NOCONNECTION)
		{
			DPL(0, "Terminating session without a connection didn't return expected error NOCONNECTION!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't terminate session)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Hosting session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a create player message for self.
		hostcontext.afExpectCreatePlayer[0] = TRUE;

		ZeroMemory(&dpAppDesc, sizeof (DPN_APPLICATION_DESC));
		dpAppDesc.dwSize = sizeof (DPN_APPLICATION_DESC);
		//dpAppDesc.dwFlags = 0;
		//dpAppDesc.guidInstance = GUID_NULL;
		dpAppDesc.guidApplication = GUID_PEER_GETAPPDESC;
		//dpAppDesc.dwMaxPlayers = 0;
		//dpAppDesc.dwCurrentPlayers = 0;
		//dpAppDesc.pwszSessionName = NULL;
		//dpAppDesc.pwszPassword = NULL;
		//dpAppDesc.pvReservedData = NULL;
		//dpAppDesc.dwReservedDataSize = 0;
		//dpAppDesc.pvApplicationReservedData = NULL;
		//dpAppDesc.dwApplicationReservedDataSize = 0;

		// Save what we're passing in to make sure it's not touched.
		CopyMemory(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC));

		hr = pPeer->Host(&dpAppDesc,
								&pDP8Address,
								1,
								NULL,
								NULL,
								(PVOID) -1,
								0);

		// Prevent any weird create player indications.
		hostcontext.afExpectCreatePlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't start hosting session!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't host)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure the create player indication came in.
		if (hostcontext.adpnidPlayers[0] == 0)
		{
			DPL(0, "Didn't get create player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player indication)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Terminating session"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a destroy player message for self.
		CREATEEVENT_OR_THROW(hostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.dwNumDestroyPlayersRemaining = 1;
		hostcontext.afExpectDestroyPlayer[0] = TRUE;
		hostcontext.fExpectTerminateSession = TRUE;

		hr = pPeer->TerminateSession(NULL, 0, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Terminating session failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for destroy player message"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird destroy player or terminate session messages.
		hostcontext.afExpectDestroyPlayer[0] = FALSE;
		hostcontext.fExpectTerminateSession = FALSE;

#ifdef DEBUG
		// Make sure the player was destroyed (how else would we get here?).
		if (hostcontext.adpnidPlayers[0] != 0)
		{
			DPL(0, "Destroy player for self didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)
#endif // DEBUG

		// Make sure the terminate session indication came in.
		if (! hostcontext.fGotTerminateSession)
		{
			DPL(0, "Didn't get terminate session indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get terminate session indication)

		// Reset the context.
		CloseHandle(hostcontext.hLastPlayerMsgEvent);
		hostcontext.hLastPlayerMsgEvent = NULL;
		hostcontext.fGotTerminateSession = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Re-initializing peer object with nonhost context"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Initialize(&nonhostcontext,
										ParmVPTerminateDPNMessageHandler, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Re-initializing peer object with context, 0 flags failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't initialize)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Creating new peer host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		pDP8Address->Release();
		pDP8Address = NULL;

		// Expect a create player message for self.
		hostcontext.afExpectCreatePlayer[0] = TRUE;

		hr = PeerCreateHost(hLog, ParmVPTerminateDPNMessageHandler,
								&hostcontext, &dpAppDesc, &pDP8PeerHost,
								(PVOID) -1, &pDP8Address);

		// Prevent any weird create player indications.
		hostcontext.afExpectCreatePlayer[0] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Creating peer host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't create peer host)

		// Make sure the create player indication came in.
		if (hostcontext.adpnidPlayers[0] == 0)
		{
			DPL(0, "Didn't get create player indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get create player indication)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Connecting peer"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect a create player message for client on host.
		CREATEEVENT_OR_THROW(hostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.afExpectCreatePlayer[1] = TRUE;

		// Expect a create player message for self and host on client.
		nonhostcontext.afExpectCreatePlayer[0] = TRUE;
		nonhostcontext.afExpectCreatePlayer[1] = TRUE;

		// Even though it's synchronous, we'll still get a completion.  The async op
		// handle will be NULL, though.
		nonhostcontext.fConnectCanComplete = TRUE;

		hr = pPeer->Connect(&dpAppDesc,
									pDP8Address,
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									(PVOID) -1,
									NULL,
									NULL,
									DPNCONNECT_SYNC);

		// Prevent any weird connects from completing or stray create players.
		nonhostcontext.fConnectCanComplete = FALSE;
		nonhostcontext.afExpectCreatePlayer[0] = FALSE;
		nonhostcontext.afExpectCreatePlayer[1] = FALSE;

		if (hr != DPN_OK)
		{
			DPL(0, "Couldn't connect synchronously!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't connect)

		if (memcmp(&dpAppDescCompare, &dpAppDesc, sizeof (DPN_APPLICATION_DESC)) != 0)
		{
			DPL(0, "Application desc structure was modified (structure at %x != structure at %x)!",
				2, &dpAppDescCompare, &dpAppDesc);
			SETTHROW_TESTRESULT(ERROR_ARENA_TRASHED);
		} // end if (app desc changed)

#pragma TODO(vanceo, "Make sure host address object hasn't changed")

		// Make sure we saw the connect completion indication.
		if (! nonhostcontext.fConnectCompleted)
		{
			DPL(0, "Didn't get connect complete indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (connect didn't complete)

		// Reset the context.
		nonhostcontext.fConnectCompleted = FALSE;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for create player message on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird create player messages.
		hostcontext.afExpectCreatePlayer[1] = FALSE;

#ifdef DEBUG
		// Make sure the player was created (how else would we get here?).
		if (hostcontext.adpnidPlayers[1] == 0)
		{
			DPL(0, "Create player didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (create player didn't arrive)
#endif // DEBUG

		// Reset the context.
		CloseHandle(hostcontext.hLastPlayerMsgEvent);
		hostcontext.hLastPlayerMsgEvent = NULL;



		// Make sure the IDs match.
		if (hostcontext.adpnidPlayers[0] != nonhostcontext.adpnidPlayers[1])
		{
			DPL(0, "Host and client disagree on host's player ID (%u/%x != %u/%x)!",
				4, hostcontext.adpnidPlayers[0], hostcontext.adpnidPlayers[0],
				nonhostcontext.adpnidPlayers[1], nonhostcontext.adpnidPlayers[1]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (host ID doesn't match)

		if (hostcontext.adpnidPlayers[1] != nonhostcontext.adpnidPlayers[0])
		{
			DPL(0, "Host and client disagree on client's player ID (%u/%x != %u/%x)!",
				4, hostcontext.adpnidPlayers[1], hostcontext.adpnidPlayers[1],
				nonhostcontext.adpnidPlayers[0], nonhostcontext.adpnidPlayers[0]);
			SETTHROW_TESTRESULT(ERROR_NO_MATCH);
		} // end if (client ID doesn't match)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Terminating session on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->TerminateSession(NULL, 0, 0);
		if (hr != DPNERR_NOTHOST)
		{
			DPL(0, "Terminating session on client didn't return expected error NOTHOST!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't terminate session)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Terminating session on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Expect the 2 destroy player messages and terminate session on the host.
		CREATEEVENT_OR_THROW(hostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		hostcontext.dwNumDestroyPlayersRemaining = 2;
		hostcontext.afExpectDestroyPlayer[0] = TRUE;
		hostcontext.afExpectDestroyPlayer[1] = TRUE;
		hostcontext.fExpectTerminateSession = TRUE;

		// Expect the 2 destroy player messages and terminate session on the client.
		CREATEEVENT_OR_THROW(nonhostcontext.hLastPlayerMsgEvent,
							NULL, FALSE, FALSE, NULL);
		nonhostcontext.dwNumDestroyPlayersRemaining = 2;
		nonhostcontext.afExpectDestroyPlayer[0] = TRUE;
		nonhostcontext.afExpectDestroyPlayer[1] = TRUE;
		nonhostcontext.fExpectTerminateSession = TRUE;

		hr = pPeerHost->TerminateSession(NULL, 0, 0);
		if (hr != DPN_OK)
		{
			DPL(0, "Terminating session on host failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't terminate session)




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for destroy player messages on host"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(hostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird destroy player or terminate session messages.
		hostcontext.afExpectDestroyPlayer[0] = FALSE;
		hostcontext.afExpectDestroyPlayer[1] = FALSE;
		hostcontext.fExpectTerminateSession = FALSE;

#ifdef DEBUG
		// Make sure the players were destroyed (how else would we get here?).
		if (hostcontext.adpnidPlayers[0] != 0)
		{
			DPL(0, "Destroy player for self didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)

		if (hostcontext.adpnidPlayers[1] != 0)
		{
			DPL(0, "Destroy player for host didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)
#endif // DEBUG

		// Make sure the terminate session indication came in.
		if (! hostcontext.fGotTerminateSession)
		{
			DPL(0, "Didn't get terminate session indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get terminate session indication)

		// Reset the context.
		CloseHandle(hostcontext.hLastPlayerMsgEvent);
		hostcontext.hLastPlayerMsgEvent = NULL;
		hostcontext.fGotTerminateSession = FALSE;




		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Waiting for destroy player messages on client"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pTNecd->pExecutor->WaitForEventOrCancel(&(nonhostcontext.hLastPlayerMsgEvent),
													1,
													NULL,
													0,
													INFINITE,
													NULL);
		HANDLE_WAIT_RESULT;


		// Prevent any weird destroy player or terminate session messages.
		nonhostcontext.afExpectDestroyPlayer[0] = FALSE;
		nonhostcontext.afExpectDestroyPlayer[1] = FALSE;
		nonhostcontext.fExpectTerminateSession = FALSE;

#ifdef DEBUG
		// Make sure the players were destroyed (how else would we get here?).
		if (nonhostcontext.adpnidPlayers[0] != 0)
		{
			DPL(0, "Destroy player for self didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)

		if (nonhostcontext.adpnidPlayers[1] != 0)
		{
			DPL(0, "Destroy player for host didn't arrive!?  DEBUGBREAK()-ing.", 0);
			DEBUGBREAK();
		} // end if (destroy player didn't arrive)
#endif // DEBUG

		// Make sure the terminate session indication came in.
		if (! nonhostcontext.fGotTerminateSession)
		{
			DPL(0, "Didn't get terminate session indication!", 0);
			SETTHROW_TESTRESULT(ERROR_NO_DATA);
		} // end if (didn't get terminate session indication)

		// Reset the context.
		CloseHandle(nonhostcontext.hLastPlayerMsgEvent);
		nonhostcontext.hLastPlayerMsgEvent = NULL;
		hostcontext.fGotTerminateSession = FALSE;





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing peer client object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing peer client object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Closing host object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Close(0);
		if (hr != DPN_OK)
		{
			DPL(0, "Closing host object failed!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't close)





		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing hosting DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeerHost->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release hosting DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;






		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// "Releasing DirectPlay8Peer object"
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		hr = pPeer->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release DirectPlay8Peer object!", 0);
			THROW_TESTRESULT;
		} // end if (couldn't release object)

		delete (pDP8Peer);
		pDP8Peer = NULL;




		FINAL_SUCCESS;
	}
	END_TESTCASE


	if (pDP8Peer != NULL)
	{
		delete (pDP8Peer);
		pDP8Peer = NULL;
	} // end if (have peer object)

	if (pDP8PeerHost != NULL)
	{
		delete (pDP8PeerHost);
		pDP8PeerHost = NULL;
	} // end if (have peer object)

	SAFE_RELEASE(pDP8Address);
	SAFE_CLOSEHANDLE(hostcontext.hLastPlayerMsgEvent);
	SAFE_CLOSEHANDLE(nonhostcontext.hLastPlayerMsgEvent);


	return (hr);
} // ParmVPeerExec_Terminate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
