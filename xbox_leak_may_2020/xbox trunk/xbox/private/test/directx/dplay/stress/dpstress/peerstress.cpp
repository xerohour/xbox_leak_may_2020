//==================================================================================
// Includes
//==================================================================================
#include "dpstress.h"

extern DWORD g_dwDPStressBytesAllocated;
static HRESULT g_hrLastError = DPN_OK;

using namespace DPlayStressNamespace;

namespace DPlayStressNamespace {

// Basic percentage distribution for when to perform a given action
#define SEND_PLAYER_PERCENTAGE 40
#define SEND_GROUP_PERCENTAGE 10
#define GROUP_PERCENTAGE 2
#define APPDESC_CHANGE_PERCENTAGE 0
//#define SEND_GROUP_PERCENTAGE 0
//#define GROUP_PERCENTAGE 0
//#define APPDESC_CHANGE_PERCENTAGE 0

// Basic percentage distribution for when to use certain send flags
#define GUARANTEED_PERCENTAGE 10
#define SEQUENTIAL_PERCENTAGE 30
#define NOLOOPBACK_PERCENTAGE 50

HRESULT PeerStressMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

void WINAPI TRACE(LPCTSTR szFormat, ...) {
	TCHAR szBuffer[1024] = TEXT("");
	va_list pArgs; 
	va_start(pArgs, szFormat);
	
	wvsprintf(szBuffer, szFormat, pArgs);
	
	va_end(pArgs);
	OutputDebugString(szBuffer);
}

//==================================================================================
// InitializePeerStressContext
//----------------------------------------------------------------------------------
//
// Description: Set up the member variables of a PEERSTRESSCONTEXT
//
// Arguments:
//	HANDLE				hLog					Handle to the logging subsystem
//	PEERSTRESSCONTEXT	*pPeerStressContext		Context to initialize
//
// Returns:
//	TRUE if the context was initialized successfully, FALSE otherwise
//==================================================================================
BOOL InitializePeerStressContext(HANDLE hLog, PEERSTRESSCONTEXT *pPeerStressContext, BOOL fIsHost)
{
	if(!pPeerStressContext)
		return FALSE;

	memset(pPeerStressContext, 0, sizeof(PEERSTRESSCONTEXT));

	pPeerStressContext->hLog = hLog;
	pPeerStressContext->fIsHost = fIsHost;
	pPeerStressContext->fVerbose = FALSE;
	strcpy(pPeerStressContext->szContextDescription, fIsHost ? "Host" : "Peer");
	SAFE_CREATEEVENT(pPeerStressContext->hConnectComplete, NULL, FALSE, FALSE, NULL);
	SAFE_CREATEEVENT(pPeerStressContext->hHostMigrated, NULL, FALSE, FALSE, NULL);
	SAFE_CREATEEVENT(pPeerStressContext->hSessionTerminated, NULL, FALSE, FALSE, NULL);
	SAFE_CREATEEVENT(pPeerStressContext->hCriticalError, NULL, FALSE, FALSE, NULL);
	InitializeCriticalSection(&(pPeerStressContext->Lock));

	return ((pPeerStressContext->hConnectComplete) &&
		(pPeerStressContext->hHostMigrated) &&
		(pPeerStressContext->hSessionTerminated));
}

//==================================================================================
// CleanupPeerStressContext
//----------------------------------------------------------------------------------
//
// Description: Clean up the member variables of a PEERSTRESSCONTEXT
//
// Arguments:
//	PEERSTRESSCONTEXT	*pPeerStressContext		Context to cleanup
//
// Returns:
//	TRUE if the test completed successfully (right now, only returns true)
//==================================================================================
void CleanupPeerStressContext(PEERSTRESSCONTEXT *pPeerStressContext)
{
	SAFE_CLOSEHANDLE(pPeerStressContext->hConnectComplete);
	SAFE_CLOSEHANDLE(pPeerStressContext->hHostMigrated);
	SAFE_CLOSEHANDLE(pPeerStressContext->hSessionTerminated);
	SAFE_CLOSEHANDLE(pPeerStressContext->hCriticalError);
	DeleteCriticalSection(&(pPeerStressContext->Lock));

	return;
}

//==================================================================================
// DisplayStressStatistics
//----------------------------------------------------------------------------------
//
// Description: Prints statistics from the current stress session
//
// Arguments:
//	HANDLE				hLog				Handle to the logging subsystem
//	PEERSTRESSCONTEXT	*pContext			Pointer to the peer stress context
//  DWORD				dwLastMinAvail		These 3 DWORD parameters are used to 
//	DWORD				dwLastMinAppAlloc	track memory usage
//	DWORD				dwMinAvail
//
// Returns:
//	nothing
//==================================================================================
void DisplayStressStatistics(HANDLE hLog, PEERSTRESSCONTEXT *pContext, DWORD dwLastMinAvail, DWORD dwLastMinAppAlloc, DWORD dwMinAvail)
{
	xLog(hLog, XLL_INFO, "DirectPlay Peer-to-Peer Stress Statistics");
	xLog(hLog, XLL_INFO, "   Bytes allocated by test: %u", g_dwDPStressBytesAllocated);
	xLog(hLog, XLL_INFO, "   Current local groups: %u", pContext->dwCurrentGroups);
	xLog(hLog, XLL_INFO, "   Current players: %u", pContext->dwPlayersConnected);
	xLog(hLog, XLL_INFO, "   Cumulative local groups: %u", pContext->dwGroupCreationCounter);
	xLog(hLog, XLL_INFO, "   App description changes: %u", pContext->dwAppDescChangeCounter);
	xLog(hLog, XLL_INFO, "   Group messages sent: %u", pContext->TotalGroupMessagesSent);
	xLog(hLog, XLL_INFO, "   Group bytes sent: %u", pContext->TotalGroupBytesSent);
	xLog(hLog, XLL_INFO, "   Individual messages sent: %u", pContext->TotalMessagesSent);
	xLog(hLog, XLL_INFO, "   Individual bytes sent: %u", pContext->TotalBytesSent);
	xLog(hLog, XLL_INFO, "   Players added to group 1: %u", pContext->dwPlayersAdded[0]);
	xLog(hLog, XLL_INFO, "   Players added to group 2: %u", pContext->dwPlayersAdded[1]);
	xLog(hLog, XLL_INFO, "   Players added to group 3: %u", pContext->dwPlayersAdded[2]);
	xLog(hLog, XLL_INFO, "   Min (avail %u + app alloc %u) mem: %u", dwLastMinAvail, dwLastMinAppAlloc, dwMinAvail);
}

//==================================================================================
// AddRandomPlayersToGroup
//----------------------------------------------------------------------------------
//
// Description: Enum the current players in the session, pick a random subset of them,
//				and add them to the group
//
// Arguments:
//	HANDLE				hLog		Handle to the logging subsytem
//	PDIRECTPLAY8PEER	pDP8Peer	DirectPlay 8 peer to have a new group created
//	DPNID				dpnidGroup	ID of the group to add players to
//
// Returns:
//	DWORD indicating the number of players added to the group
//==================================================================================
DWORD AddRandomPlayersToGroup(HANDLE hLog, PDIRECTPLAY8PEER pDP8Peer, DPNID dpnidGroup)
{
	DPNHANDLE	hAsyncOp = NULL;
	HRESULT		hr = DPN_OK;
	DPNID		*pPlayerList = NULL;
	DWORD		dwNumPlayers = 0, dwPlayersAdded = 0;

	// Query for the number of player and/or groups
	hr = pDP8Peer->EnumPlayersAndGroups(NULL, &dwNumPlayers, DPNENUM_PLAYERS);

	// If the operation succeeded, then DirectPlay is saying that there aren't any
	// players available on this object.
	if(hr == DPN_OK)
	{
		// This should never happen, since we should at least have the local player...
		xLog(hLog, XLL_WARN, "No players are available!");
		goto Exit;
	}
	else if (hr != DPNERR_BUFFERTOOSMALL)
	{
		xLog(hLog, XLL_WARN, "Couldn't enumerate any players: 0x%08x", hr);
		g_hrLastError = hr;
		goto Exit;
	}

AllocatePlayerArray:

	// Do the player array allocation here
	if(dwNumPlayers > 0)
	{
		pPlayerList = (DPNID *) MemAlloc(dwNumPlayers * sizeof(DPNID));
		if(!pPlayerList)
		{
			xLog(hLog, XLL_WARN, "Couldn't allocate player buffer!");
			goto Exit;
		}
	}
	// DirectPlay should always return at least return the one host player...
	else
	{
		xLog(hLog, XLL_WARN, "EnumPlayersAndGroups claimed buffer was too small, but returned 0 players/groups!");
		goto Exit;
	}

	// Try to retrieve the player and group information...
	// If it's changed since the last time we called, we may have to reallocate our buffer again
	hr = pDP8Peer->EnumPlayersAndGroups(pPlayerList, &dwNumPlayers, DPNENUM_PLAYERS);

	// It succeeded, so we have an array of player/group ID's
	if(hr == DPN_OK)
	{
		DWORD dwCurrentPlayer = 0;
		
		// We now have a list of players, so randomly add them to the group
		if(dwNumPlayers > 0)
		{
			for(dwCurrentPlayer = 0;dwCurrentPlayer < dwNumPlayers;++dwCurrentPlayer)
			{
				// 50-50 chance of adding the player to the group
				if(rand() % 2)
				{
					hr = pDP8Peer->AddPlayerToGroup(dpnidGroup, pPlayerList[dwCurrentPlayer], NULL, &hAsyncOp, 0);
					switch(hr)
					{
					case DPN_OK:
						xLog(hLog, XLL_WARN, "CreateGroup unexpectedly returned synchronously!");
					case DPNSUCCESS_PENDING:
						++dwPlayersAdded;
						break;
					case DPNERR_INVALIDPLAYER:
						// This is expected from time to time.  We tried to add a player to a group but that player
						// has since left the session
						break;
					default:
						xLog(hLog, XLL_WARN, "Couldn't add player 0x%08x to group 0x%08x! (error 0x%08x)",
							pPlayerList[dwCurrentPlayer], dpnidGroup, hr);
						g_hrLastError = hr;
						goto Exit;
					}
				}
			}

			// If no one wound up being added, scan through the list and at least add one user to the group
			// In the case where we are hosting and no one has connected, this will add the host player
			if(!dwPlayersAdded)
			{
				for(dwCurrentPlayer = 0;dwCurrentPlayer < dwNumPlayers;++dwCurrentPlayer)
				{
					hr = pDP8Peer->AddPlayerToGroup(dpnidGroup, pPlayerList[dwCurrentPlayer], NULL, &hAsyncOp, 0);
					switch(hr)
					{
					case DPN_OK:
						xLog(hLog, XLL_WARN, "CreateGroup unexpectedly returned synchronously!");
					case DPNSUCCESS_PENDING:
						++dwPlayersAdded;
						// This is a hack to get us to leave the loop since we only wanted to add one player
						dwCurrentPlayer = dwNumPlayers;
						break;
					case DPNERR_INVALIDPLAYER:
						// This is expected from time to time.  We tried to add a player to a group but that player
						// has since left the session
						break;
					default:
						xLog(hLog, XLL_WARN, "Couldn't add player 0x%08x to group 0x%08x! (error 0x%08x)",
							pPlayerList[dwCurrentPlayer], dpnidGroup, hr);
						g_hrLastError = hr;
						goto Exit;
					}
				}	
			}
		}
		// DirectPlay should at least return the one host player...
		else
		{
			xLog(hLog, XLL_WARN, "EnumPlayersAndGroups said there are no players in the session!");
			goto Exit;
		}

	}
	// If our buffer is too small, we'll have to try querying for the new required buffer size
	else if (hr == DPNERR_BUFFERTOOSMALL)
	{
		pPlayerList ? MemFree(pPlayerList) : 0;
		pPlayerList = NULL;
		goto AllocatePlayerArray;
	}
	else
	{
		xLog(hLog, XLL_WARN, "Couldn't enumerate any players: 0x%08x", hr);
		g_hrLastError = hr;
		goto Exit;
	}

Exit:
	pPlayerList ? MemFree(pPlayerList) : 0;

	return dwPlayersAdded;
}


//==================================================================================
// CreateGroupWithRandomInfo
//----------------------------------------------------------------------------------
//
// Description: Create a group and populate it with a name based on the local system name
//				and a random info buffer
//
// Arguments:
//	HANDLE				hLog		Handle to the logging subsytem
//	PDIRECTPLAY8PEER	pDP8Peer	DirectPlay 8 peer to have a new group created
//	PEERSTRESSCONTEXT	*pContext	Pointer to the context for this peer
//
// Returns:
//	TRUE if the group creation call was successfully made, FALSE otherwise
//==================================================================================
BOOL CreateGroupWithRandomInfo(HANDLE hLog, PDIRECTPLAY8PEER pDP8Peer, PEERSTRESSCONTEXT *pContext)
{
	DPN_GROUP_INFO	dpnGroupInfo;
	DPNHANDLE		hGroupOp = NULL;
	HRESULT			hr = DPN_OK;
	DWORD			dwBufferSize = 0;
	CHAR			szLocalMacAddress[20];
	WCHAR			szNameString[20];
	BOOL			fRet = TRUE;
	
	memset(&dpnGroupInfo, 0, sizeof(DPN_GROUP_INFO));
	dpnGroupInfo.dwSize = sizeof(DPN_GROUP_INFO);

	// Always provide both name information
	dpnGroupInfo.dwInfoFlags = DPNINFO_NAME;

	// Create auto-destruct groups half the time
	dpnGroupInfo.dwGroupFlags = (rand() % 2) ? DPNGROUP_AUTODESTRUCT : 0;

	// Use the mac address string to personalize group names made by this player
	if(gethostname(szLocalMacAddress, sizeof(szLocalMacAddress)))
	{
		sprintf(szLocalMacAddress, "%s%u", UNKNOWN_HOST_STRING, (pContext->dwGroupCreationCounter) % 100);
	}
	else
	{
		sprintf(szLocalMacAddress + strlen(szLocalMacAddress), "%u", (pContext->dwGroupCreationCounter) % 100);
	}
	mbstowcs(szNameString, szLocalMacAddress, strlen(szLocalMacAddress) + 1);
	dpnGroupInfo.pwszName = szNameString;
	
	// Generate a random buffer of data to send
	if(!(dpnGroupInfo.dwDataSize = GenerateRandomDataBuffer((BYTE **) &(dpnGroupInfo.pvData), GROUP_DATA_MIN_SIZE, GROUP_DATA_MAX_SIZE)))
	{
		xLog(hLog, XLL_WARN, "Couldn't generate random buffer for send data");
		dpnGroupInfo.pvData = NULL;
	}
	else
	{
		// We successfully added data, so add that flag to the group structure.
		dpnGroupInfo.dwInfoFlags |= DPNINFO_DATA;
	}
	
	// Create the group, provide TRUE as the group context so that we know its one that we created
	hr = pDP8Peer->CreateGroup(&dpnGroupInfo, (LPVOID) TRUE, NULL, &hGroupOp, 0);
	if (hr == DPN_OK)
	{
		xLog(hLog, XLL_WARN, "CreateGroup unexpectedly returned synchronously!");
	}
	// Group creation failed or some reason
	else if (hr != DPNSUCCESS_PENDING)
	{
		xLog(hLog, XLL_WARN, "Couldn't create group! (error 0x%08x)", hr);
		g_hrLastError = hr;
		fRet = FALSE;
	}

	// Free the group data pointer
	dpnGroupInfo.pvData ? MemFree(dpnGroupInfo.pvData) : 0;
	dpnGroupInfo.pvData = NULL;

	return fRet;
}

//==================================================================================
// GetRandomTarget
//----------------------------------------------------------------------------------
//
// Description: Enumerate players and/or groups on a peer and pick one at random
//
// Arguments:
//	HANDLE				hLog		Handle to the logging subsytem
//	PDIRECTPLAY8PEER	pDP8Peer	DirectPlay 8 peer to have players/groups enumerated on
//	DPNID				*pTarget	Variable for passing back randomly selected player/group
//	DWORD				dwFlags		Contains DPNENUM_PLAYERS and/or DPNENUM_GROUPS to indicate
//									if players, groups or both should be randomly selected from
//
// Returns:
//	DPNENUM_PLAYERS or DPNENUM_GROUPS to indicate if player or group (respectively) is being
//	returned in pTarget
//==================================================================================
DWORD GetRandomTarget(HANDLE hLog, PDIRECTPLAY8PEER pDP8Peer, DPNID *pTarget, DWORD dwFlags)
{
	HRESULT	hr;
	DPNID	*pPlayerList = NULL;
	DWORD	dwNumPlayers = 0, dwTargetIndex = 0, dwTargetType = 0;


	if(!(dwFlags & DPNENUM_PLAYERS) && !(dwFlags & DPNENUM_GROUPS))
	{
		xLog(hLog, XLL_WARN, "No player/group flags specified!");
		goto Exit;
	}

	// Query for the number of player and/or groups
	hr = pDP8Peer->EnumPlayersAndGroups(NULL, &dwNumPlayers, dwFlags);

	// If the operation succeeded, then DirectPlay is saying that there aren't any
	// players and/or groups available on this object.  If we were querying for players,
	// then this must mean that we're disconnected.  If we were querying for just groups,
	// this only means that no groups have been created yet.
	if(hr == DPN_OK)
	{
		if(dwFlags & DPNENUM_PLAYERS)
		{
			xLog(hLog, XLL_WARN, "No players are available!");
			goto Exit;
		}
		else if(dwFlags & DPNENUM_GROUPS)
		{
			// This case is possible if we don't have any active local groups at the momemt
//			xLog(hLog, XLL_INFO, "No groups are available.");
			goto Exit;
		}
	}
	else if (hr != DPNERR_BUFFERTOOSMALL)
	{
		xLog(hLog, XLL_WARN, "Couldn't enumerate any players: 0x%08x", hr);
		g_hrLastError = hr;
		goto Exit;
	}

AllocatePlayerArray:

	// Do the player array allocation here
	if(dwNumPlayers > 0)
	{
		pPlayerList = (DPNID *) MemAlloc(dwNumPlayers * sizeof(DPNID));
		if(!pPlayerList)
		{
			xLog(hLog, XLL_WARN, "Couldn't allocate player buffer!");
			goto Exit;
		}
	}
	// Something weird happened if DirectPlay said we needed a larger buffer but there aren't any player/groups
	else
	{
		xLog(hLog, XLL_WARN, "EnumPlayersAndGroups claimed buffer was too small, but returned 0 players/groups!");
		goto Exit;
	}

	// Try to retrieve the player and group information...
	// If it's changed since the last time we called, we may have to reallocate our buffer again
	hr = pDP8Peer->EnumPlayersAndGroups(pPlayerList, &dwNumPlayers, dwFlags);

	// It succeeded, so we have an array of player/group ID's
	if(hr == DPN_OK)
	{
		DWORD dwInfoSize = 0;

		// Pick a random index in the array and set that as the target
		dwTargetIndex = (rand() % dwNumPlayers);
		*pTarget = pPlayerList[dwTargetIndex];

		// Determine if it is a player or a group

		// If we were querying for both players and groups, then we'll have to call
		// GetPeerInfo and GetGroupInfo to determine if it's a player or a group
		if((dwFlags & DPNENUM_PLAYERS) && (dwFlags & DPNENUM_GROUPS))
		{
			dwInfoSize = 0;
			hr = pDP8Peer->GetPeerInfo(*pTarget, NULL, &dwInfoSize, 0);

			// This is either a group or a disconnected player
			if(hr == DPNERR_INVALIDPLAYER)
			{
				dwInfoSize = 0;
				hr = pDP8Peer->GetGroupInfo(*pTarget, NULL, &dwInfoSize, 0);

				// Must be a disconnected player since GetPeerInfo and GetGroupInfo failed
				if(hr == DPNERR_INVALIDGROUP)
				{
					xLog(hLog, XLL_INFO, "Randomly selected target (0x%08x) no longer exists!", *pTarget);
					*pTarget = 0;
					goto Exit;
				}
				// Must be a group since it didn't return INVALIDGROUP
				else if(hr == DPNERR_BUFFERTOOSMALL)
				{
					dwTargetType = DPNENUM_PLAYERS;
				}
				// Something weird failed in the call, return failure
				else
				{
					xLog(hLog, XLL_WARN, "Couldn't verify type of selected target: 0x%08x", hr);
					g_hrLastError = hr;
					*pTarget = 0;
					goto Exit;
				}

				dwTargetType = DPNENUM_GROUPS;
			}
			// Must be a player since it didn't return INVALIDPLAYER
			else if(hr == DPNERR_BUFFERTOOSMALL)
			{
				dwTargetType = DPNENUM_PLAYERS;
			}
			// Something weird failed in the call, return failure
			else
			{
				xLog(hLog, XLL_WARN, "Couldn't verify type of selected target: 0x%08x", hr);
				g_hrLastError = hr;
				*pTarget = 0;
				goto Exit;
			}
		}
		// otherwise, the flag that was set will tell us
		else if (dwFlags & DPNENUM_PLAYERS)
		{
				dwTargetType = DPNENUM_PLAYERS;
		}
		else
		{
				dwTargetType = DPNENUM_GROUPS;
		}

	}
	// If our buffer is too small, we'll have to try querying for the new required buffer size
	else if (hr == DPNERR_BUFFERTOOSMALL)
	{
		pPlayerList ? MemFree(pPlayerList) : 0;
		pPlayerList = NULL;
		goto AllocatePlayerArray;
	}
	else
	{
		xLog(hLog, XLL_WARN, "Couldn't enumerate any players: 0x%08x", hr);
		g_hrLastError = hr;
		goto Exit;
	}

Exit:

	pPlayerList ? MemFree(pPlayerList) : 0;

	return dwTargetType;
}

//==================================================================================
// GenerateRandomSendFlags
//----------------------------------------------------------------------------------
//
// Description: Helper function that randomly generates send flags
//
// Arguments:
//	BOOL			fGroupSend		Boolean that indicates if this send is to a group
//
// Returns:
//	DWORD containing the flags for this send
//==================================================================================
DWORD GenerateRandomSendFlags(BOOL fGroupSend)
{
	DWORD dwSendFlags = 0;

/*
	// Randomly determine send priority...
	switch(rand() % 3)
	{
	case 0:
		dwSendFlags |= DPNSEND_PRIORITY_HIGH;
		break;
	case 1:
		dwSendFlags |= DPNSEND_PRIORITY_LOW;
		break;
	default:
		break;
	}
	
	// Randomly determine guaranteed/non-guaranteed...
	if((rand() % 100) < GUARANTEED_PERCENTAGE)
	{
		dwSendFlags |= DPNSEND_GUARANTEED;
	}
	
	// Randomly determine sequential/non-sequential...
	if((rand() % 100) >= SEQUENTIAL_PERCENTAGE)
	{
		dwSendFlags |= DPNSEND_NONSEQUENTIAL;
	}
	
	// If this is a group send, consider using the no loopback flag
	if(fGroupSend)
	{
		if((rand() % 100) < NOLOOPBACK_PERCENTAGE)
		{
			dwSendFlags |= DPNSEND_NONSEQUENTIAL;
		}
	}
*/
	return dwSendFlags;
}

//==================================================================================
// PerformRandomDirectPlayWork
//----------------------------------------------------------------------------------
//
// Description: Enumerate players and/or groups on a peer and pick one at random
//
// Arguments:
//	HANDLE				hLog		Handle to the logging subsytem
//	PDIRECTPLAY8PEER	pDP8Peer	DirectPlay 8 peer to have players/groups enumerated on
//	PPEERSTRESSCONTEXT	pContext	Pointer to the context associated with the pDP8Peer object
//
// Returns:
//	DPNENUM_PLAYERS or DPNENUM_GROUPS to indicate if player or group (respectively) is being
//	returned in pTarget
//==================================================================================
BOOL PerformRandomDirectPlayWork(HANDLE hLog, PDIRECTPLAY8PEER pDP8Peer, PEERSTRESSCONTEXT *pContext)
{
	DPN_BUFFER_DESC		dpnbd;
	DPNHANDLE			AsyncHandle;
	HRESULT				hr;
	DWORD				dwRandomAction = 0, dwTargetType = 0, dwBytesInQueue = 0, dwBufferSize = 0, n = 0;
	DPNID				dpnidTarget;
	BOOL				fSuccess = TRUE;
	BYTE				*pBuffer = NULL;

	dwRandomAction = (rand() % 100);

	// This case handles when the random action is a single player send
	if(dwRandomAction < SEND_PLAYER_PERCENTAGE)
	{
		// Get a random player to send to
		dwTargetType = GetRandomTarget(hLog, pDP8Peer, &dpnidTarget, DPNENUM_PLAYERS);
		if(!(dwTargetType & DPNENUM_PLAYERS))
		{
			fSuccess = FALSE;
			goto Exit;
		}

		// If the selected target isn't the local player, then check the send queue to insure
		// that this target isn't getting clogged with data
		if(pContext->LocalPlayerID != dpnidTarget)
		{
			// Check their send queue info to see if we'll be choking the connection by sending to them
			hr = pDP8Peer->GetSendQueueInfo(dpnidTarget, NULL, &dwBytesInQueue,0);
			if(hr != DPN_OK)
			{
				xLog(hLog, XLL_INFO, "Lost connection with targeted player: 0x%08x (error 0x%08x)", dpnidTarget, hr);
				g_hrLastError = hr;
				fSuccess = FALSE;
				goto Exit;
			}
			
			// If the send queue has gotten too large, skip this send
			if(dwBytesInQueue > MAX_PENDING_SEND_BYTES)
			{
//				xLog(hLog, XLL_INFO, "Targetted player's send queue is too large: 0x%08x", dpnidTarget);
				fSuccess = FALSE;
				goto Exit;
			}
		}

		// Generate a random buffer of data to send
		if(!(dwBufferSize = GenerateRandomDataBuffer(&pBuffer, SEND_DATA_MIN_SIZE, SEND_DATA_MAX_SIZE)))
		{
			xLog(hLog, XLL_WARN, "Couldn't generate random buffer for send data");
			fSuccess = FALSE;
			goto Exit;
		}

		// Package up the buffer
		dpnbd.dwBufferSize = dwBufferSize;
		dpnbd.pBufferData = pBuffer;

		// BUGBUG - what about various message flags?
		// As a last attempt to prevent our send queues from getting unreasonably large...
		// the send will timeout after 100 ms.  So at most, 50 sends can be pending at at time
		// since the main loop will spend at least 2ms per loop processing DoWork calls
		// This may change if the test starts calling the PerformRandomDirectPlayWork multiple times per loop
		hr = pDP8Peer->SendTo(dpnidTarget, &dpnbd, 1, 100, NULL, &AsyncHandle, GenerateRandomSendFlags(FALSE));
		if (hr == DPN_OK)
		{
			xLog(hLog, XLL_WARN, "SendTo unexpectedly returned synchronously!");
		}
		else if (hr != DPNSUCCESS_PENDING)
		{
			xLog(hLog, XLL_WARN, "Lost connection while sending to player: 0x%08x (error 0x%08x)", dpnidTarget, hr);
			g_hrLastError = hr;
			fSuccess = FALSE;
			goto Exit;
		}

		++(pContext->TotalMessagesSent);
		(pContext->TotalBytesSent) += dwBufferSize;
	}
	// This case handles when the random action is a group send
	else if((dwRandomAction -= SEND_PLAYER_PERCENTAGE) < SEND_GROUP_PERCENTAGE)
	{
		// Get a random group to send to
		dwTargetType = GetRandomTarget(hLog, pDP8Peer, &dpnidTarget, DPNENUM_GROUPS);

		// Possible that we don't have any active groups right now
		if(!(dwTargetType & DPNENUM_GROUPS))
		{
			fSuccess = FALSE;
			goto Exit;
		}

		// Generate a random buffer of data to send
		if(!(dwBufferSize = GenerateRandomDataBuffer(&pBuffer, SEND_DATA_MIN_SIZE, SEND_DATA_MAX_SIZE)))
		{
			xLog(hLog, XLL_WARN, "Couldn't generate random buffer for send data");
			fSuccess = FALSE;
			goto Exit;
		}

		// Package up the buffer
		dpnbd.dwBufferSize = dwBufferSize;
		dpnbd.pBufferData = pBuffer;

		// BUGBUG - what about various message flags?
		// As a last attempt to prevent our send queues from getting unreasonably large...
		// the send will timeout after 100 ms.  So at most, 50 sends can be pending at at time
		// since the main loop will spend at least 2ms per loop processing DoWork calls
		// This may change if the test starts calling the PerformRandomDirectPlayWork multiple times per loop
		hr = pDP8Peer->SendTo(dpnidTarget, &dpnbd, 1, 100, NULL, &AsyncHandle, GenerateRandomSendFlags(TRUE));
		if (hr == DPN_OK)
		{
			xLog(hLog, XLL_WARN, "SendTo unexpectedly returned synchronously!");
		}
		else if (hr != DPNSUCCESS_PENDING)
		{
			xLog(hLog, XLL_WARN, "Lost connection while sending to group: 0x%08x (error 0x%08x)", dpnidTarget, hr);
			g_hrLastError = hr;
			fSuccess = FALSE;
			goto Exit;
		}

		++(pContext->TotalGroupMessagesSent);
		(pContext->TotalGroupBytesSent) += dwBufferSize;
	}
	// This case handles when the random action is a group create/delete
	else if((dwRandomAction -= SEND_GROUP_PERCENTAGE) < GROUP_PERCENTAGE)
	{
		DPNHANDLE	hGroupOp;
		BOOL		fCreateGroup = TRUE;

		switch(pContext->dwCurrentGroups)
		{
		case 0:				// There are no groups, create one...
			fCreateGroup = TRUE;
			break;
		case MAX_GROUPS:	// Maximum groups have been created, delete one...
			fCreateGroup = FALSE;
			break;
		default:			// Somewhere in between no groups and max groups, randomly decide what to do...
			fCreateGroup = (BOOL) (rand() % 2);
			break;
		}

		// If we haven't already created a group... then create one...
		if(fCreateGroup)
		{
			if(!CreateGroupWithRandomInfo(hLog, pDP8Peer, pContext))
			{
				fSuccess = FALSE;
				goto Exit;
			}

			++(pContext->dwCurrentGroups);
			++(pContext->dwGroupCreationCounter);
		}
		// If we already created a group... then destroy it...
		else
		{
			DPNID dpnidGroupToDelete = 0;

			for(n = 0;n < MAX_GROUPS;++n)
			{
				if(dpnidGroupToDelete = pContext->LocalGroupIDs[n])
					break;
			}

			if(!dpnidGroupToDelete)
			{
//				xLog(hLog, XLL_WARN, "Tried to delete local group, but its creation hasn't been indicated yet!");
				fSuccess = FALSE;
				goto Exit;
			}
			
			hr = pDP8Peer->DestroyGroup(dpnidGroupToDelete, NULL, &hGroupOp, 0);
			if (hr == DPN_OK)
			{
				xLog(hLog, XLL_WARN, "DestroyGroup unexpectedly returned synchronously!");
			}
			else if (hr != DPNSUCCESS_PENDING)
			{
				xLog(hLog, XLL_WARN, "Couldn't destroy group: 0x%08x (error 0x%08x)", dpnidGroupToDelete, hr);
				fSuccess = FALSE;
				g_hrLastError = hr;
				goto Exit;
			}

			--(pContext->dwCurrentGroups);
			pContext->LocalGroupIDs[n] = 0;
			pContext->dwPlayersAdded[n] = 0;
		}
	}
	else if((dwRandomAction -= GROUP_PERCENTAGE) < APPDESC_CHANGE_PERCENTAGE)
	{
		if(pContext->fIsHost)
		{
			DPN_APPLICATION_DESC	dpnad;
			CHAR					szLocalMacAddress[20];
			WCHAR					szNameString[20];
			
			ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
			dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
			dpnad.dwFlags = DPNSESSION_MIGRATE_HOST;
			dpnad.guidApplication = GUID_PEER_STRESS;
			
			// Use the mac address string to personalize the application name
			if(gethostname(szLocalMacAddress, sizeof(szLocalMacAddress)))
			{
				sprintf(szLocalMacAddress, "%s%u", UNKNOWN_HOST_STRING, (pContext->dwAppDescChangeCounter) % 100);
			}
			else
			{
				sprintf(szLocalMacAddress + strlen(szLocalMacAddress), "%u", (pContext->dwAppDescChangeCounter) % 100);
			}
			mbstowcs(szNameString, szLocalMacAddress, strlen(szLocalMacAddress) + 1);
			dpnad.pwszSessionName = szNameString;
			
			// Allocate a random buffer for application data
			if(!(dpnad.dwApplicationReservedDataSize =
				GenerateRandomDataBuffer((BYTE **) &(dpnad.pvApplicationReservedData), APP_DESC_MIN_SIZE, APP_DESC_MAX_SIZE)))
			{
				xLog(hLog, XLL_WARN, "Couldn't generate random buffer for application data");
				fSuccess = FALSE;
				goto Exit;
			}
			
			hr = pDP8Peer->SetApplicationDesc(&dpnad, 0);
			if (hr != DPN_OK)
			{
				xLog(hLog, XLL_WARN, "Couldn't set application description: (error 0x%08x)", hr);
				dpnad.pvApplicationReservedData ? MemFree(dpnad.pvApplicationReservedData) : 0;
				dpnad.pvApplicationReservedData = NULL;
				fSuccess = FALSE;
				goto Exit;
			}
			
			dpnad.pvApplicationReservedData ? MemFree(dpnad.pvApplicationReservedData) : 0;
			dpnad.pvApplicationReservedData = NULL;
		}
	}
//	else if((dwRandomAction -= SEND_PERCENTAGE) < SOMEOTHER_PERCENTAGE)
//	{
//		// BUGBUG - Implement other cases
//	}

	// 
	// Everything beyond this point will be executed regardless of what random action was chosen.
	// In some cases, this processing is used to complete an earlier action, such as adding players
	// to a group that has now been asynchronously created

	// If local groups have been created, see if any need players added to them...
	if(pContext->dwCurrentGroups)
	{
		// Scan the local group list
		for(n = 0;n < MAX_GROUPS;++n)
		{
			// If a particular group has been created, but doesn't have any players yet,
			// go ahead and add them.
			if(pContext->LocalGroupIDs[n] && !(pContext->dwPlayersAdded[n]))
				pContext->dwPlayersAdded[n] = AddRandomPlayersToGroup(hLog, pDP8Peer, pContext->LocalGroupIDs[n]);
		}
	}

Exit:

	pBuffer ? MemFree(pBuffer) : 0;
	pBuffer = NULL;

	return fSuccess;
}

//==================================================================================
// PeerStress
//----------------------------------------------------------------------------------
//
// Description: Host or connect to a DPlay session and start stress
//
// Arguments:
//	HANDLE		hLog			Handle to the logging subsystem
//	HANDLE		hNetsync		Handle to the Netsync subsystem
//	DP_HOSTINFO	*pMsg			Received Netsync message containing DPlay session info
//	IN_ADDR		*pLocalAddr		pLocalAddr
//	BOOL		fStartedAsHost	Boolean indicating if this machine is being started as the host
//
// Returns:
//	TRUE if the test completed successfully (right now, only returns true)
//==================================================================================
BOOL PeerStress(HANDLE hLog, HANDLE hNetsync, DP_HOSTINFO *pMsg, IN_ADDR *pLocalAddr, BOOL fStartedAsHost)
{
	DPN_APPLICATION_DESC	dpnad;
	PDIRECTPLAY8ADDRESS		pDP8DeviceAddress = NULL, pDP8HostAddress = NULL;
	PEERSTRESSCONTEXT		PeerStressContext;
	PDIRECTPLAY8PEER		pDP8Peer = NULL;
	PPLAYERCONTEXT			pLocalPlayerContext = NULL;
	SOCKADDR_IN				HostSockAddr, EnumedSockAddr;
	DPNHANDLE				AsyncHandle;
	HRESULT					hr;
	HANDLE					hEnumEvent = NULL;
	DWORD					dwPort = 0, dwBufferSize = 0, dwLastKATime = 0;
	DWORD					dwLastMemTime = 0, dwLoopStartTime = 0, dwTimeElapsed = 0;
	BYTE					*pBuffer = NULL;

	MEMORYSTATUS			MemoryStatus;
	DWORD					dwMinAvail = 0, dwLastMinAppAlloc = 0, dwLastMinAvail = 0;

	BOOL					fDPlayInitialized = FALSE, fPeerInitialized = FALSE;

	memset(&MemoryStatus, 0, sizeof(MEMORYSTATUS));
	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus (&MemoryStatus);

	xLog(hLog, XLL_INFO, "Memory status at start", MemoryStatus.dwAvailPhys);
	xLog(hLog, XLL_INFO, "   Available:          %u", MemoryStatus.dwAvailPhys);
	xLog(hLog, XLL_INFO, "   Allocated by App:   %u", g_dwDPStressBytesAllocated);
	xLog(hLog, XLL_INFO, "   Sum:                %u",
		dwMinAvail = MemoryStatus.dwAvailPhys + g_dwDPStressBytesAllocated);

	hr = DPlayInitialize(1024 * 400);
	if(hr != DPN_OK)
	{
		xLog(hLog, XLL_FAIL, "Couldn't init DPlay: 0x%08x", hr);
		goto Exit;
	}

	fDPlayInitialized = TRUE;
//	xLog(g_hLog, XLL_INFO, "DirectPlay layer initialized");

	// First, create a peer object... no voice
	hr = DirectPlay8Create(IID_IDirectPlay8Peer, (LPVOID *) &pDP8Peer, NULL);
	if(hr != DPN_OK)
	{
		xLog(hLog, XLL_WARN, "Couldn't create peer object: 0x%08x", hr);
		goto Exit;
	}

	// Initialize all the member variables of the peer context
	if(!InitializePeerStressContext(hLog, &PeerStressContext, fStartedAsHost))
	{
		xLog(hLog, XLL_WARN, "Couldn't create peer object: 0x%08x", hr);
		goto Exit;
	}

	// Next, initialize the peer object
	hr = pDP8Peer->Initialize(&PeerStressContext, PeerStressMessageHandler, 0);
	if (hr != DPN_OK)
	{
		xLog(hLog, XLL_WARN, "Initializing peer object failed: 0x%08x", hr);
		goto Exit;
	}

	fPeerInitialized = TRUE;

	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8DeviceAddress, NULL);
	if(hr != DPN_OK)
	{
		xLog(hLog, XLL_WARN, "Couldn't create device address object: 0x%08x", hr);
		goto Exit;
	}
	
	// Set the service provider of this address
	hr = pDP8DeviceAddress->SetSP(&CLSID_DP8SP_TCPIP);
	if(hr != DPN_OK)
	{
		xLog(hLog, XLL_WARN, "Couldn't set the local device address SP to TCP/IP: 0x%08x", hr);
		goto Exit;
	}
	
	// If we are the host, go ahead and start hosting...
	if(fStartedAsHost)
	{
		xLog(hLog, XLL_INFO, "Initializing local DirectPlay peer object as host...");

		//----------------------------------------
		// HOST STARTS HOSTING
		//----------------------------------------

		// For now, we're hardcoding the hosting port to DPSTRESS_PORT...
		// It will turn up on the DPlay protocol parser that way
		dwPort = DPSTRESS_PORT;
		hr = pDP8DeviceAddress->AddComponent(DPNA_KEY_PORT, &dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't add a specific port to the host object: 0x%08x", hr);
			goto Exit;
		}

		// Set up the application description for this session
		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.dwFlags = DPNSESSION_MIGRATE_HOST;
		dpnad.guidApplication = GUID_PEER_STRESS;

		// Allocate a local player context.  Callback will deallocate on destroy player message
		pLocalPlayerContext = NULL;
		pLocalPlayerContext = (PPLAYERCONTEXT) MemAlloc(sizeof(PLAYERCONTEXT));
		memset(pLocalPlayerContext, 0, sizeof(PLAYERCONTEXT));

		if(!pLocalPlayerContext)
		{
			xLog(hLog, XLL_WARN, "Couldn't allocate local player context");
			goto Exit;
		}

		// Host the session
		hr = pDP8Peer->Host(&dpnad, &pDP8DeviceAddress, 1, NULL, NULL, pLocalPlayerContext, 0);
		if (hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't host session: 0x%08x", hr);
			goto Exit;
		}
	}
	// Otherwise, we need to connect to the host...
	else
	{
		xLog(hLog, XLL_INFO, "Initializing local DirectPlay peer object as non-host...");

		//----------------------------------------
		// NON-HOST ENUMERATION CASES
		//----------------------------------------

		// Set up the application description for this session
		ZeroMemory(&dpnad, sizeof (DPN_APPLICATION_DESC));
		dpnad.dwSize = sizeof (DPN_APPLICATION_DESC);
		dpnad.guidApplication = GUID_PEER_STRESS;

		SAFE_CREATEEVENT(hEnumEvent, NULL, FALSE, FALSE, NULL);

		// For now, we're hardcoding the hosting port to DPSTRESS_PORT...
		// It will turn up on the DPlay protocol parser that way
		HostSockAddr.sin_family = AF_INET;
		HostSockAddr.sin_port = htons(DPSTRESS_PORT);
		HostSockAddr.sin_addr.S_un.S_addr = pMsg->dwHostAddr;

		
		hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &(PeerStressContext.pExpectedHostAddress), NULL);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't create device address object: 0x%08x", hr);
			goto Exit;
		}

		
		hr = PeerStressContext.pExpectedHostAddress->BuildFromSockAddr((SOCKADDR *) &HostSockAddr);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't build an address from a SOCKADDR: %0x%08x", hr);
			goto Exit;
		}

		//----------------------------------------
		// 1) NO HOST ADDRESS PROVIDED
		//----------------------------------------

		// Allocate a buffer of random enum data
		if(!(dwBufferSize = GenerateRandomDataBuffer(&pBuffer, ENUM_DATA_MIN_SIZE, ENUM_DATA_MAX_SIZE)))
		{
			xLog(hLog, XLL_WARN, "Couldn't generate random buffer for enumeration request data");
			goto Exit;
		}

		ResetEvent(hEnumEvent);
		xLog(hLog, XLL_INFO, "Enumerating host without a host address");

		// Start the enumeration
		hr = pDP8Peer->EnumHosts(&dpnad, NULL, pDP8DeviceAddress, pBuffer, dwBufferSize,
			0, 0, 0, hEnumEvent, &AsyncHandle, 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			xLog(hLog, XLL_WARN, "Couldn't enumerate hosts: 0x%08x", hr);
			goto Exit;
		}

		// Waiting for enumeration to complete
		if(!PollPeerUntilEventSignalled(hLog, pDP8Peer, MAX_ENUM_TIME, hEnumEvent, NULL))
			xLog(hLog, XLL_WARN, "Enumeration without address couldn't find host");

		// Cancel to enum to make sure that it's complete before starting the next enum
		hr = pDP8Peer->CancelAsyncOperation(0, DPNCANCEL_ENUM);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't cancel asynchronous enumeration!", hr);
			goto Exit;
		}

		MemFree(pBuffer);
		pBuffer = NULL;
		dwBufferSize = 0;

		//----------------------------------------
		// 2) HOST PORT, BUT NO HOST IP PROVIDED
		//----------------------------------------

		// Allocate a buffer of random enum data
		if(!(dwBufferSize = GenerateRandomDataBuffer(&pBuffer, ENUM_DATA_MIN_SIZE, ENUM_DATA_MAX_SIZE)))
		{
			xLog(hLog, XLL_WARN, "Couldn't generate random buffer for enumeration request data");
			goto Exit;
		}

		// Create host address
		hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8HostAddress, NULL);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't create host address object: %0x%08x", hr);
			goto Exit;
		}
		
		// Set the service provider of this address
		hr = pDP8HostAddress->SetSP(&CLSID_DP8SP_TCPIP);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't set the remote host address SP to TCP/IP: %0x%08x", hr);
			goto Exit;
		}

		// For now, we're hardcoding the hosting port to DPSTRESS_PORT...
		// It will turn up on the DPlay protocol parser that way
		dwPort = DPSTRESS_PORT;
		hr = pDP8HostAddress->AddComponent(DPNA_KEY_PORT, &dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't add a specific port to the host object: %0x%08x", hr);
			goto Exit;
		}

		ResetEvent(hEnumEvent);
		xLog(hLog, XLL_INFO, "Enumerating host with port, but no IP address");

		hr = pDP8Peer->EnumHosts(&dpnad, pDP8HostAddress, pDP8DeviceAddress, pBuffer, dwBufferSize,
			0, 0, 0, hEnumEvent, &AsyncHandle, 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			xLog(hLog, XLL_WARN, "Couldn't enumerate hosts: 0x%08x", hr);
			goto Exit;
		}

		// Waiting for enumeration to complete
		if(!PollPeerUntilEventSignalled(hLog, pDP8Peer, MAX_ENUM_TIME, hEnumEvent, NULL))
			xLog(hLog, XLL_WARN, "Enumeration with port, but no IP address couldn't find host");

		// Cancel to enum to make sure that it's complete before starting the next enum
		hr = pDP8Peer->CancelAsyncOperation(0, DPNCANCEL_ENUM);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't cancel asynchronous enumeration!", hr);
			goto Exit;
		}

		MemFree(pBuffer);
		pBuffer = NULL;
		dwBufferSize = 0;

		//----------------------------------------
		// 3) HOST PORT AND HOST IP PROVIDED
		//----------------------------------------

		// Allocate a buffer of random enum data
		if(!(dwBufferSize = GenerateRandomDataBuffer(&pBuffer, ENUM_DATA_MIN_SIZE, ENUM_DATA_MAX_SIZE)))
		{
			xLog(hLog, XLL_WARN, "Couldn't generate random buffer for enumeration request data");
			goto Exit;
		}

		pDP8HostAddress->Clear();

		hr = pDP8HostAddress->BuildFromSockAddr( (SOCKADDR *) &HostSockAddr);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't build an address from a SOCKADDR: %0x%08x", hr);
			goto Exit;
		}

		ResetEvent(hEnumEvent);
		xLog(hLog, XLL_INFO, "Enumerating host with both port and IP address");

		hr = pDP8Peer->EnumHosts(&dpnad, pDP8HostAddress, pDP8DeviceAddress, pBuffer, dwBufferSize,
			0, 0, 0, hEnumEvent, &AsyncHandle, 0);
		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			xLog(hLog, XLL_WARN, "Couldn't enumerate hosts: 0x%08x", hr);
			goto Exit;
		}

		// Waiting for enumeration to complete
		if(!PollPeerUntilEventSignalled(hLog, pDP8Peer, MAX_ENUM_TIME, hEnumEvent, NULL))
		{
			xLog(hLog, XLL_WARN, "Enumeration with both port and IP address couldn't find host!");
			goto Exit;
		}

		// Cancel to enum to make sure that it's complete before starting the next enum
		hr = pDP8Peer->CancelAsyncOperation(0, DPNCANCEL_ENUM);
		if(hr != DPN_OK)
		{
			xLog(hLog, XLL_WARN, "Couldn't cancel asynchronous enumeration!", hr);
			goto Exit;
		}

		MemFree(pBuffer);
		pBuffer = NULL;
		dwBufferSize = 0;

		//----------------------------------------
		// NON-HOST STARTS CONNECTING
		//----------------------------------------
		
		xLog(hLog, XLL_INFO, "Connecting local peer to remote host...");

		// Allocate a player context for the local player
		pLocalPlayerContext = NULL;
		pLocalPlayerContext = (PPLAYERCONTEXT) MemAlloc(sizeof(PLAYERCONTEXT));
		memset(pLocalPlayerContext, 0, sizeof(PLAYERCONTEXT));

		if(!pLocalPlayerContext)
		{
			xLog(hLog, XLL_WARN, "Couldn't allocate local player context");
			goto Exit;
		}

		// Allocate a buffer of random connect data
		if(!(dwBufferSize = GenerateRandomDataBuffer(&pBuffer, CONN_DATA_MIN_SIZE, CONN_DATA_MAX_SIZE)))
		{
			xLog(hLog, XLL_WARN, "Couldn't generate random buffer for enumeration request data");
			goto Exit;
		}

		hr = pDP8Peer->Connect(&dpnad, pDP8HostAddress, pDP8DeviceAddress, NULL, NULL, pBuffer, dwBufferSize,
			pLocalPlayerContext, NULL, &AsyncHandle, 0);

		if (hr != (HRESULT) DPNSUCCESS_PENDING)
		{
			xLog(hLog, XLL_WARN, "Couldn't connect to host: 0x%08x", hr);
			goto Exit;
		}

		// Waiting for enumeration to complete
		if(!PollPeerUntilEventSignalled(hLog, pDP8Peer, MAX_CONN_TIME, PeerStressContext.hConnectComplete, NULL))
		{
			xLog(hLog, XLL_WARN, "Connection didn't succeed within %u seconds", MAX_CONN_TIME / 1000);
			goto Exit;
		}
	}

	xLog(hLog, XLL_INFO, "Starting random work loop...");

	dwLastMemTime = GetTickCount();

	// Start the main test loop
	while(WAIT_TIMEOUT == WaitForSingleObject(PeerStressContext.hSessionTerminated, 0))
	{
		dwLoopStartTime = GetTickCount();

		g_hrLastError = DPN_OK;

		PerformRandomDirectPlayWork(hLog, pDP8Peer, &PeerStressContext);

		if(g_hrLastError != DPN_OK)
		{
			DWORD dwWaitResult;
			dwWaitResult = WaitForSingleObject(PeerStressContext.hSessionTerminated, 0);
			switch(dwWaitResult)
			{
			case WAIT_TIMEOUT:
				TRACE(TEXT("Got unexpected DirectPlay error (0x%08x) and session hasn't terminated.\n"), g_hrLastError);
				break;
			case WAIT_OBJECT_0:
				TRACE(TEXT("Got unexpected DirectPlay error (0x%08x) but session has already terminated.\n"), g_hrLastError);
				break;
			default:
				TRACE(TEXT("Got unexpected DirectPlay error (0x%08x) and wait on session termination failed.\n"), g_hrLastError);
				break;
			}

			DisplayStressStatistics(hLog, &PeerStressContext, dwLastMinAvail, dwLastMinAppAlloc, dwMinAvail);

			goto Exit;
		}


		DoWorkForFixedInterval<IDirectPlay8Peer>(pDP8Peer, 2);
		
		if(IsNetsyncSessionOver(hNetsync))
		{
			// Netsync signalled that it couldn't maintain the session any longer.  Probably because
			// multiple machines think they are host after a connection was lost.
			// Exit and resync with Netsync
			xLog(hLog, XLL_WARN, "Netsync signalled that the session is over");
			goto Exit;
		}

		if(PeerStressContext.fIsHost)
		{	
			// Check to see if the host has migrated to this machine
			// A newly migrated machine won't have set fStartedAsHost to TRUE yet
			if(!fStartedAsHost)
			{
				// If it has, send an update to the Netsync server and
				// act as though we were started as the host, so we won't
				// continue to send Netsync updates
				
				if(SendNetsyncHostUpdate(hNetsync, pLocalAddr) == WAIT_FAILED)
				{
					// We have to exit in this case, otherwise, Netsync will be sending new peers to the old host
					xLog(hLog, XLL_WARN, "Couldn't update Netsync server with migrated host information");
					goto Exit;
				}
				
				fStartedAsHost = TRUE;

				dwLastKATime = GetTickCount();
			}

			// If this isn't a newly migrated host, see if we need to send a keepalive
			else
			{
				// Calculate the amount of time that has elapsed since the last keepalive
				if(dwLoopStartTime < dwLastKATime)
					dwTimeElapsed = ((DWORD) 0xffffffff) - dwLastKATime + dwLoopStartTime;
				else
					dwTimeElapsed = dwLoopStartTime - dwLastKATime;

				// If the time has come, send a keepalive
				if(dwTimeElapsed > HOST_KEEPALIVE_INTERVAL)
				{
					if(SendNetsyncHostKeepalive(hNetsync) == WAIT_FAILED)
					{
						// We have to exit in this case, otherwise, the Netsync server will remove us anyway
						xLog(hLog, XLL_WARN, "Couldn't send keepalive to Netsync server");
						goto Exit;
					}

					dwLastKATime = GetTickCount();
				}
			}
		}

		memset(&MemoryStatus, 0, sizeof(MEMORYSTATUS));
		MemoryStatus.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus (&MemoryStatus);

		// We add back the memory allocated by the app so that DirectPlay isn't penalized for it
		// But if the adjusted amount of available memory is below the previous minimum, record it
		if((MemoryStatus.dwAvailPhys + g_dwDPStressBytesAllocated) < dwMinAvail)
		{
			dwMinAvail = MemoryStatus.dwAvailPhys + g_dwDPStressBytesAllocated;
			dwLastMinAppAlloc = g_dwDPStressBytesAllocated;
			dwLastMinAvail = MemoryStatus.dwAvailPhys;
		}

		// Calculate the amount of time that has elapsed since the last keepalive
		if(dwLoopStartTime < dwLastMemTime)
			dwTimeElapsed = ((DWORD) 0xffffffff) - dwLastMemTime + dwLoopStartTime;
		else
			dwTimeElapsed = dwLoopStartTime - dwLastMemTime;

		if(dwTimeElapsed > 1800000)
		{
			DisplayStressStatistics(hLog, &PeerStressContext, dwLastMinAvail, dwLastMinAppAlloc, dwMinAvail);
			dwLastMemTime = GetTickCount();
		}

		if(g_hrLastError != DPN_OK)
		{

			xLog(hLog, XLL_WARN, "Error 0x%08x was returned from a DirectPlay call, exiting test...", g_hrLastError);
			goto Exit;
		}
	}
	
Exit:
	PeerStressContext.pExpectedHostAddress ? PeerStressContext.pExpectedHostAddress->Release() : 0;
	PeerStressContext.pExpectedHostAddress = NULL;

	pBuffer ? MemFree(pBuffer) : 0;
	pBuffer = NULL;
	SAFE_CLOSEHANDLE(hEnumEvent);

	if(fPeerInitialized && pDP8Peer)
	{
		hr = pDP8Peer->Close(0);
		if(hr != DPN_OK)
			xLog(hLog, XLL_FAIL, "Couldn't close DPlay peer: 0x%08x", hr);
		else
			xLog(hLog, XLL_INFO, "DirectPlay peer closed");
	}

	pDP8Peer ? pDP8Peer->Release() : 0;
	pDP8Peer = NULL;

	pDP8DeviceAddress ? pDP8DeviceAddress->Release() : 0;
	pDP8DeviceAddress = NULL;

	pDP8HostAddress ? pDP8HostAddress->Release() : 0;
	pDP8HostAddress = NULL;

	if(fDPlayInitialized)
	{
		hr = DPlayCleanup();
		if(hr != DPN_OK)
			xLog(hLog, XLL_FAIL, "Couldn't cleanup DPlay: 0x%08x", hr);
		else
			xLog(hLog, XLL_INFO, "DirectPlay layer cleaned-up");
	}

	CleanupPeerStressContext(&PeerStressContext);

	xLog(hLog, XLL_INFO, "Exiting test...");

	Sleep(30000);

	return TRUE;
}


#undef DEBUG_SECTION
#define DEBUG_SECTION	"PeerStressMessageHandler()"
//==================================================================================
// PeerStressMessageHandler
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
HRESULT PeerStressMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT				hr = DPN_OK;
	PEERSTRESSCONTEXT	*pContext = (PEERSTRESSCONTEXT *) pvContext;
	PPLAYERCONTEXT		pPlayerContext = NULL;
	BOOL				fLocalPlayer = TRUE;
	DWORD				dwStartTime = 0, dwEndTime = 0, n = 0;
	
	dwStartTime = GetTickCount();
	
	switch (dwMsgType)
	{
	case DPN_MSGID_CREATE_PLAYER:
		{
			PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;
			
			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_CREATE_PLAYER");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pCreatePlayerMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidPlayer = %u/0x%08x", pCreatePlayerMsg->dpnidPlayer, pCreatePlayerMsg->dpnidPlayer);
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pCreatePlayerMsg->pvPlayerContext);
			}
			
			// If this is a local player, then take the buffer that we allocated when we hosted
			// or connected
			if(pCreatePlayerMsg->pvPlayerContext)
			{
				fLocalPlayer = TRUE;
				
				pContext->LocalPlayerID = pCreatePlayerMsg->dpnidPlayer;
				pPlayerContext = ((PPLAYERCONTEXT) pCreatePlayerMsg->pvPlayerContext);
			}
			// Otherwise this is a remote player.  Create a new buffer.
			else
			{
				fLocalPlayer = FALSE;
				
				pPlayerContext = (PPLAYERCONTEXT) MemAlloc(sizeof(PLAYERCONTEXT));
				if(!pPlayerContext)
				{
					xLog(pContext->hLog, XLL_WARN, "Not enough memory to allocate player context!  Signal main app!");
					
					SetEvent(pContext->hCriticalError);
					hr = E_FAIL;
					break;
				}
				
				pCreatePlayerMsg->pvPlayerContext = pPlayerContext;
			}
			
			// Initialize the player information
			pPlayerContext->PlayerID = pCreatePlayerMsg->dpnidPlayer;
			pPlayerContext->TotalBytesReceived = 0;
			pPlayerContext->TotalMessagesReceived = 0;
			
			// Add info on this player to our global player list
			EnterCriticalSection(&(pContext->Lock));
			
			pPlayerContext->pPrev = NULL;
			if(pContext->pPlayerList)
				pContext->pPlayerList->pPrev = pPlayerContext->pNext;
			pPlayerContext->pNext = pContext->pPlayerList;
			pContext->pPlayerList = pPlayerContext;
			
			++pContext->dwPlayersConnected;
			
			LeaveCriticalSection(&(pContext->Lock));
		}
		
		break;
		
	case DPN_MSGID_DESTROY_PLAYER:
		{
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;
			
			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pvMsg;
			
			if(pContext->fVerbose)
			{
				CHAR					szReason[64] = "";
				
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_DESTROY_PLAYER");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pDestroyPlayerMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidPlayer = %u/0x%08x", pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dpnidPlayer);
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pDestroyPlayerMsg->pvPlayerContext);
				
				switch(pDestroyPlayerMsg->dwReason)
				{
				case DPNDESTROYPLAYERREASON_NORMAL:
					strcpy(szReason, "NORMAL");
					break;
				case DPNDESTROYPLAYERREASON_CONNECTIONLOST:
					strcpy(szReason, "CONNECTIONLOST");
					break;
				case DPNDESTROYPLAYERREASON_SESSIONTERMINATED:
					strcpy(szReason, "SESSIONTERMINATED");
					break;
				case DPNDESTROYPLAYERREASON_HOSTDESTROYEDPLAYER:
					strcpy(szReason, "HOSTDESTROYEDPLAYER");
					break;
				default:
					xLog(pContext->hLog, XLL_WARN, "Unknown player destroy reason!");
					break;
				}
				
				xLog(pContext->hLog, XLL_INFO, "     dwReason = %s", szReason);
			}
			
			pPlayerContext = (PPLAYERCONTEXT) pDestroyPlayerMsg->pvPlayerContext;
			
			if(!pPlayerContext)
			{
				xLog(pContext->hLog, XLL_WARN, "No player context received!  Signal main app!");
				
				SetEvent(pContext->hCriticalError);
				hr = E_FAIL;
				break;
			}
			
			// Remove this player from our global player list
			EnterCriticalSection(&(pContext->Lock));
			
			if(pPlayerContext->pNext)
				pPlayerContext->pNext->pPrev = pPlayerContext->pPrev;
			if(pPlayerContext->pPrev)
				pPlayerContext->pPrev->pNext = pPlayerContext->pNext;
			else
				pContext->pPlayerList = pPlayerContext->pNext;
			
			--pContext->dwPlayersConnected;
			
			LeaveCriticalSection(&(pContext->Lock));
			
			// Release the player context
			MemFree(pPlayerContext);
		}
		break;
		
	case DPN_MSGID_CREATE_GROUP:
		{
			PDPNMSG_CREATE_GROUP	pCreateGroupMsg;
			
			pCreateGroupMsg = (PDPNMSG_CREATE_GROUP) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_CREATE_GROUP");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pCreateGroupMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidGroup = %u/0x%08x", pCreateGroupMsg->dpnidGroup, pCreateGroupMsg->dpnidGroup);
				xLog(pContext->hLog, XLL_INFO, "     dpnidOwner = %u/0x%08x", pCreateGroupMsg->dpnidOwner, pCreateGroupMsg->dpnidOwner);
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pCreateGroupMsg->pvGroupContext);
			}

			// This group was created locally...
			if(pCreateGroupMsg->pvGroupContext)
			{
				// Find an empty slot in the local group list
				for(n = 0;n < MAX_GROUPS;++n)
				{
					// Store the new group ID in the first empty slot
					if(!pContext->LocalGroupIDs[n])
					{
						pContext->LocalGroupIDs[n] = pCreateGroupMsg->dpnidGroup;
						break;
					}
				}

				// This should never happen...
				// The main loop will only call CreateGroup if we have less than the max number of groups
				if(n == MAX_GROUPS)
					xLog(pContext->hLog, XLL_WARN, "Got a local group creation, but there are no slots left!");
			}
		}
		break;
		
	case DPN_MSGID_DESTROY_GROUP:
		{
			PDPNMSG_DESTROY_GROUP	pDestroyGroupMsg;
			
			pDestroyGroupMsg = (PDPNMSG_DESTROY_GROUP) pvMsg;
			
			if(pContext->fVerbose)
			{
				CHAR					szReason[64] = "";
				
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_DESTROY_GROUP");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pDestroyGroupMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidGroup = %u/0x%08x", pDestroyGroupMsg->dpnidGroup, pDestroyGroupMsg->dpnidGroup);
				xLog(pContext->hLog, XLL_INFO, "     pvGroupContext = 0x%08x", pDestroyGroupMsg->pvGroupContext);
				
				switch(pDestroyGroupMsg->dwReason)
				{
				case DPNDESTROYGROUPREASON_SESSIONTERMINATED:
					strcpy(szReason, "SESSIONTERMINATED");
					break;
				default:
					xLog(pContext->hLog, XLL_WARN, "Unknown group destroy reason!");
					break;
				}
				
				xLog(pContext->hLog, XLL_INFO, "     dwReason = %s", szReason);
			}
		}
		break;
		
	case DPN_MSGID_ADD_PLAYER_TO_GROUP:
		{
			PDPNMSG_ADD_PLAYER_TO_GROUP	pAddPlayerMsg;
			
			pAddPlayerMsg = (PDPNMSG_ADD_PLAYER_TO_GROUP) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_ADD_PLAYER_TO_GROUP");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pAddPlayerMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidGroup = %u/0x%08x", pAddPlayerMsg->dpnidGroup, pAddPlayerMsg->dpnidGroup);
				xLog(pContext->hLog, XLL_INFO, "     pvGroupContext = 0x%08x", pAddPlayerMsg->pvGroupContext);
				xLog(pContext->hLog, XLL_INFO, "     dpnidPlayer = %u/0x%08x", pAddPlayerMsg->dpnidPlayer, pAddPlayerMsg->dpnidPlayer);
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pAddPlayerMsg->pvPlayerContext);
			}
		}
		break;
		
	case DPN_MSGID_REMOVE_PLAYER_FROM_GROUP:
		{
			PDPNMSG_REMOVE_PLAYER_FROM_GROUP	pRemovePlayerMsg;
			
			pRemovePlayerMsg = (PDPNMSG_REMOVE_PLAYER_FROM_GROUP) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_REMOVE_PLAYER_FROM_GROUP");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pRemovePlayerMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidGroup = %u/0x%08x", pRemovePlayerMsg->dpnidGroup, pRemovePlayerMsg->dpnidGroup);
				xLog(pContext->hLog, XLL_INFO, "     pvGroupContext = 0x%08x", pRemovePlayerMsg->pvGroupContext);
				xLog(pContext->hLog, XLL_INFO, "     dpnidPlayer = %u/0x%08x", pRemovePlayerMsg->dpnidPlayer, pRemovePlayerMsg->dpnidPlayer);
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pRemovePlayerMsg->pvPlayerContext);
			}
		}
		break;
		
	case DPN_MSGID_INDICATE_CONNECT:
		{
			PDPNMSG_INDICATE_CONNECT pIndicateConnectMsg;
			
			pIndicateConnectMsg = (PDPNMSG_INDICATE_CONNECT) pvMsg;
			
			if(pContext->fVerbose)
			{
				LPSTR szAddress = NULL;
				
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_INDICATE_CONNECT");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pIndicateConnectMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     pvUserConnectData = 0x%08x", pIndicateConnectMsg->pvUserConnectData);
				xLog(pContext->hLog, XLL_INFO, "     dwUserConnectDataSize = %u", pIndicateConnectMsg->dwUserConnectDataSize);
				xLog(pContext->hLog, XLL_INFO, "     pvReplyData = 0x%08x", pIndicateConnectMsg->pvReplyData);
				xLog(pContext->hLog, XLL_INFO, "     dwReplyDataSize = %u", pIndicateConnectMsg->dwReplyDataSize);
				xLog(pContext->hLog, XLL_INFO, "     pvReplyContext = 0x%08x", pIndicateConnectMsg->pvReplyContext);
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pIndicateConnectMsg->pvPlayerContext);
				
				// Parse the player address
				szAddress = NULL;
				szAddress = GetAddressString(pIndicateConnectMsg->pAddressPlayer);
				xLog(pContext->hLog, XLL_INFO, "     pAddressPlayer = %s",
					szAddress ? szAddress : "unknown" );
				szAddress ? MemFree(szAddress) : 0;
				
				// Parse the device address
				szAddress = NULL;
				szAddress = GetAddressString(pIndicateConnectMsg->pAddressDevice);
				xLog(pContext->hLog, XLL_INFO, "     pAddressDevice = %s",
					szAddress ? szAddress : "unknown" );
				szAddress ? MemFree(szAddress) : 0;
			}
			
			if(!pContext->fIsHost)
			{
				xLog(pContext->hLog, XLL_WARN, "INDICATED_CONNECT shouldn't be called on non-host!");
			}
			
			// Parse any connect data if present
			if(pIndicateConnectMsg->pvUserConnectData)
			{
				if(!VerifyDataBuffer(pIndicateConnectMsg->pvUserConnectData, pIndicateConnectMsg->dwUserConnectDataSize))
				{
					xLog(pContext->hLog, XLL_WARN, "Received invalid connect data! (%u bytes)",
						pIndicateConnectMsg->dwUserConnectDataSize);
				}
				else
				{
					// Generate a connect reply buffer
					if(!(pIndicateConnectMsg->dwReplyDataSize = 
						GenerateRandomDataBuffer((BYTE **) &(pIndicateConnectMsg->pvReplyData), CONNREPLY_DATA_MIN_SIZE, CONNREPLY_DATA_MAX_SIZE)))
					{
						xLog(pContext->hLog, XLL_WARN, "Couldn't generate random buffer for connect reply data");
						pIndicateConnectMsg->pvReplyData = NULL;
					}
				}
			}
			
			// Signal the main thread that a connection was indicated
			SetEvent(pContext->hConnectComplete);
		}
		break;
		
	case DPN_MSGID_INDICATED_CONNECT_ABORTED:
		{
			PDPNMSG_INDICATED_CONNECT_ABORTED pIndicateConnectAbortedMsg;
			
			pIndicateConnectAbortedMsg = (PDPNMSG_INDICATED_CONNECT_ABORTED) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_INDICATED_CONNECT_ABORTED");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pIndicateConnectAbortedMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pIndicateConnectAbortedMsg->pvPlayerContext);
			}
		}
		break;
		
		
	case DPN_MSGID_CONNECT_COMPLETE:
		{
			PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg;
			
			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_CONNECT_COMPLETE");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pConnectCompleteMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     hAsyncOp = 0x%08x", pConnectCompleteMsg->hAsyncOp);
				// BUGBUG - compare this with what was passed in to connect
				xLog(pContext->hLog, XLL_INFO, "     pvUserContext = 0x%08x", pConnectCompleteMsg->pvUserContext);
				xLog(pContext->hLog, XLL_INFO, "     hResultCode = 0x%08x", pConnectCompleteMsg->hResultCode);
				xLog(pContext->hLog, XLL_INFO, "     pvApplicationReplyData = 0x%08x", pConnectCompleteMsg->pvApplicationReplyData);
				xLog(pContext->hLog, XLL_INFO, "     dwApplicationReplyDataSize = %u", pConnectCompleteMsg->dwApplicationReplyDataSize);
			}
			
			if(pContext->fIsHost)
			{
				xLog(pContext->hLog, XLL_WARN, "CONNECT_COMPLETE shouldn't be called on host!");
			}
			
			// Parse any reply data if present
			if(pConnectCompleteMsg->dwApplicationReplyDataSize)
			{
				if(!VerifyDataBuffer(pConnectCompleteMsg->pvApplicationReplyData, pConnectCompleteMsg->dwApplicationReplyDataSize))
				{
					xLog(pContext->hLog, XLL_WARN, "Received invalid connect reply data! (%u bytes)",
						pConnectCompleteMsg->dwApplicationReplyDataSize);
				}
			}
			
			// Signal the main thread that connection completed
			SetEvent(pContext->hConnectComplete);
		}
		break;
		
	case DPN_MSGID_ENUM_HOSTS_QUERY:
		{
			PDPNMSG_ENUM_HOSTS_QUERY pEnumQueryMsg;
			
			pEnumQueryMsg = (PDPNMSG_ENUM_HOSTS_QUERY) pvMsg;
			
			if(pContext->fVerbose)
			{
				LPSTR szAddress = NULL;
				
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_ENUM_HOSTS_QUERY");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pEnumQueryMsg->dwSize);
				
				// Parse the player address
				szAddress = NULL;
				szAddress = GetAddressString(pEnumQueryMsg->pAddressSender);
				xLog(pContext->hLog, XLL_INFO, "     pAddressSender = %s",
					szAddress ? szAddress : "unknown" );
				szAddress ? MemFree(szAddress) : 0;
				
				// Parse the device address
				szAddress = NULL;
				szAddress = GetAddressString(pEnumQueryMsg->pAddressDevice);
				xLog(pContext->hLog, XLL_INFO, "     pAddressDevice = %s",
					szAddress ? szAddress : "unknown" );
				szAddress ? MemFree(szAddress) : 0;
				
				xLog(pContext->hLog, XLL_INFO, "     pvReceivedData = 0x%08x", pEnumQueryMsg->pvReceivedData);
				xLog(pContext->hLog, XLL_INFO, "     dwReceivedDataSize = %u", pEnumQueryMsg->dwReceivedDataSize);
				xLog(pContext->hLog, XLL_INFO, "     dwMaxResponseDataSize = %u", pEnumQueryMsg->dwMaxResponseDataSize);
				xLog(pContext->hLog, XLL_INFO, "     pvResponseData = 0x%08x", pEnumQueryMsg->pvResponseData);
				xLog(pContext->hLog, XLL_INFO, "     dwResponseDataSize = %u", pEnumQueryMsg->dwResponseDataSize);
				xLog(pContext->hLog, XLL_INFO, "     pvResponseContext = 0x%08x", pEnumQueryMsg->pvResponseContext);
				
			}
			else
			{
				LPSTR szAddress = NULL;
				
				// Parse the player address
				szAddress = NULL;
				szAddress = GetAddressString(pEnumQueryMsg->pAddressSender);
				xLog(pContext->hLog, XLL_INFO, "Received enumeration query from %s", 
					szAddress ? szAddress : "unknown" );
				szAddress ? MemFree(szAddress) : 0;
			}
			
			if(!pContext->fIsHost)
			{
				xLog(pContext->hLog, XLL_WARN, "ENUM_HOSTS_QUERY shouldn't be called on non-host!");
			}
			
			// Parse any request data if present
			if(pEnumQueryMsg->pvReceivedData)
			{
				if(!VerifyDataBuffer(pEnumQueryMsg->pvReceivedData, pEnumQueryMsg->dwReceivedDataSize))
				{
					xLog(pContext->hLog, XLL_WARN, "Received invalid enum query data! (%u bytes)",
						pEnumQueryMsg->dwReceivedDataSize);
				}
				else
				{
					// Generate a connect reply buffer
					if(!(pEnumQueryMsg->dwResponseDataSize = 
						GenerateRandomDataBuffer((BYTE **) &(pEnumQueryMsg->pvResponseData), ENUMREPLY_DATA_MIN_SIZE, pEnumQueryMsg->dwMaxResponseDataSize)))
					{
						xLog(pContext->hLog, XLL_WARN, "Couldn't generate random buffer for enum reply data");
						pEnumQueryMsg->pvReceivedData = NULL;
					}
				}
			}
		}
		break;
		
	case DPN_MSGID_ENUM_HOSTS_RESPONSE:
		{
			PDPNMSG_ENUM_HOSTS_RESPONSE pEnumResponseMsg;
			
			pEnumResponseMsg = (PDPNMSG_ENUM_HOSTS_RESPONSE) pvMsg;
			
			if(pContext->fVerbose)
			{
				LPSTR szAddress = NULL;
				
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_ENUM_HOSTS_RESPONSE");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pEnumResponseMsg->dwSize);
				
				// Parse the player address
				szAddress = NULL;
				szAddress = GetAddressString(pEnumResponseMsg->pAddressSender);
				xLog(pContext->hLog, XLL_INFO, "     pAddressSender = %s",
					szAddress ? szAddress : "unknown" );
				szAddress ? MemFree(szAddress) : 0;
				
				// Parse the device address
				szAddress = NULL;
				szAddress = GetAddressString(pEnumResponseMsg->pAddressDevice);
				xLog(pContext->hLog, XLL_INFO, "     pAddressDevice = %s",
					szAddress ? szAddress : "unknown" );
				szAddress ? MemFree(szAddress) : 0;
				
				xLog(pContext->hLog, XLL_INFO, "     pApplicationDescription = 0x%08x", pEnumResponseMsg->pApplicationDescription);
				xLog(pContext->hLog, XLL_INFO, "     pvResponseData = 0x%08x", pEnumResponseMsg->pvResponseData);
				xLog(pContext->hLog, XLL_INFO, "     dwResponseDataSize = %u", pEnumResponseMsg->dwResponseDataSize);
				xLog(pContext->hLog, XLL_INFO, "     pvUserContext = 0x%08x", pEnumResponseMsg->pvUserContext);
				xLog(pContext->hLog, XLL_INFO, "     dwRoundTripLatencyMS = %u", pEnumResponseMsg->dwRoundTripLatencyMS);
			}
			else
			{
				LPSTR szAddress = NULL;
				
				// Parse the player address
				szAddress = NULL;
				szAddress = GetAddressString(pEnumResponseMsg->pAddressSender);
				xLog(pContext->hLog, XLL_INFO, "Received enumeration response from %s (%u ms latency)", 
					szAddress ? szAddress : "unknown", pEnumResponseMsg->dwRoundTripLatencyMS);
				szAddress ? MemFree(szAddress) : 0;
			}
			
			if(pContext->fIsHost)
			{
				xLog(pContext->hLog, XLL_WARN, "ENUM_HOSTS_RESPONSE shouldn't be called on host!");
			}
			
			// Parse any response data if present
			if(pEnumResponseMsg->pvResponseData)
			{
				if(!VerifyDataBuffer(pEnumResponseMsg->pvResponseData, pEnumResponseMsg->dwResponseDataSize))
				{
					xLog(pContext->hLog, XLL_WARN, "Received invalid enum response data! (%u bytes)",
						pEnumResponseMsg->dwResponseDataSize);
				}
			}

			if(pContext->pExpectedHostAddress)
			{
				if(pEnumResponseMsg->pAddressSender->IsEqual(pContext->pExpectedHostAddress) == DPNSUCCESS_EQUAL)
				{
					xLog(pContext->hLog, XLL_INFO, "Enum response is from expected host.");
					pEnumResponseMsg->pvUserContext ? SetEvent((HANDLE) pEnumResponseMsg->pvUserContext) : 0;
				}
				else
				{
					xLog(pContext->hLog, XLL_INFO, "Enum response is from unexpected host.");
				}
			}
			else
			{
				xLog(pContext->hLog, XLL_INFO, "ENUM_HOSTS_RESPONSE was called but no address has been set to compare it with!");
			}

		}
		break;
		
	case DPN_MSGID_RECEIVE:
		{
			PDPNMSG_RECEIVE pReceiveMsg;
			
			pReceiveMsg = (PDPNMSG_RECEIVE) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_RECEIVE", 0);
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pReceiveMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidSender = %u/0x%08x", pReceiveMsg->dpnidSender, pReceiveMsg->dpnidSender);
				// BUGBUG - verify that the player context matches the player context that was set when
				// this player connected
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pReceiveMsg->pvPlayerContext);
				// BUGBUG - create a common message format that contains the sender information
				// so that we can verify that this data is accurate
				xLog(pContext->hLog, XLL_INFO, "     pReceiveData = 0x%08x", pReceiveMsg->pReceiveData);
				xLog(pContext->hLog, XLL_INFO, "     dwReceiveDataSize = %u", pReceiveMsg->dwReceiveDataSize);
				// BUGBUG - try holding onto this buffer and returning it on subsequent calls to receive
				// Use the buffer handle to return it later.  Maybe store the last buffer handle received
				// in the players context.
				xLog(pContext->hLog, XLL_INFO, "     hBufferHandle = 0x%08x", pReceiveMsg->hBufferHandle);
			}
			
			// Verify that the received buffer is valid
			if(pReceiveMsg->dwReceiveDataSize)
			{
				if(!VerifyDataBuffer(pReceiveMsg->pReceiveData, pReceiveMsg->dwReceiveDataSize))
				{
					xLog(pContext->hLog, XLL_WARN, "Received invalid player message! (%u bytes)",
						pReceiveMsg->dwReceiveDataSize);
				}
			}
			else
			{
				xLog(pContext->hLog, XLL_WARN, "Received message with 0 bytes!");
			}
			
			// Because messages from the same player are serialized, we know that DELETE_PLAYER can't come along
			// and remove this context before we access it... so there is no need to lock the critical section.
			
			if(!pReceiveMsg->pvPlayerContext)
			{
				xLog(pContext->hLog, XLL_WARN, "Received message from unknown player: 0x%08x", pReceiveMsg->dpnidSender);
			}
			else
			{
				// Update the player context entry to reflect the received message
				pPlayerContext = (PPLAYERCONTEXT) pReceiveMsg->pvPlayerContext;
				++(pPlayerContext->TotalMessagesReceived);
				(pPlayerContext->TotalBytesReceived) += pReceiveMsg->dwReceiveDataSize;
			}
		}
		break;
	case DPN_MSGID_SEND_COMPLETE:
		{
			PDPNMSG_SEND_COMPLETE pSendCompleteMsg;
			
			pSendCompleteMsg = (PDPNMSG_SEND_COMPLETE) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_SEND_COMPLETE");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pSendCompleteMsg->dwSize);
				// BUGBUG - check with the context and verify that this is the handle of a send
				// that we have outstanding
				xLog(pContext->hLog, XLL_INFO, "     hAsyncOp = 0x%08x", pSendCompleteMsg->hAsyncOp);
				// BUGBUG - again check with the context and verify that this is the context that we
				// supplied with a call to send
				xLog(pContext->hLog, XLL_INFO, "     pvUserContext = 0x%08x", pSendCompleteMsg->pvUserContext);
				// BUGBUG - are there ways to get this value to be a failure?
				xLog(pContext->hLog, XLL_INFO, "     hResultCode = 0x%08x", pSendCompleteMsg->hResultCode);
				// BUGBUG - do lots of sends and see if this increases, what about guaranteed?
				xLog(pContext->hLog, XLL_INFO, "     dwSendTime = %u ms", pSendCompleteMsg->dwSendTime);
			}
		}
		break;
		
	case DPN_MSGID_APPLICATION_DESC:
		{
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_APPLICATION_DESC");
			}
			
			++pContext->dwAppDescChangeCounter;
		}
		break;
		
	case DPN_MSGID_ASYNC_OP_COMPLETE:
		{
			PDPNMSG_ASYNC_OP_COMPLETE pAsyncOpCompleteMsg;
			
			pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_ASYNC_OP_COMPLETE");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pAsyncOpCompleteMsg->dwSize);
				// BUGBUG - check with the context and verify that this is the handle of an operation
				// that we have outstanding
				xLog(pContext->hLog, XLL_INFO, "     hAsyncOp = %u", pAsyncOpCompleteMsg->hAsyncOp);
				// BUGBUG - are there ways to get this value to be a failure?
				xLog(pContext->hLog, XLL_INFO, "     hResultCode = 0x%08x", pAsyncOpCompleteMsg->hResultCode);
			}
		}
		break;
		
	case DPN_MSGID_PEER_INFO:
		{
			PDPNMSG_PEER_INFO pPeerInfoMsg;
			
			pPeerInfoMsg = (PDPNMSG_PEER_INFO) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_PEER_INFO");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pPeerInfoMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidPeer = %u/0x%08x", pPeerInfoMsg->dpnidPeer, pPeerInfoMsg->dpnidPeer);
				// BUGBUG - verify that the player context matches the player context that was set when
				// this player connected
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pPeerInfoMsg->pvPlayerContext);
			}
			
			// Because messages from the same player are serialized, we know that DELETE_PLAYER can't come along
			// and remove this context before we access it... so there is no need to lock the critical section.
			
			if(!pPeerInfoMsg->pvPlayerContext)
			{
				xLog(pContext->hLog, XLL_WARN, "Received player info change from unknown player: 0x%08x", pPeerInfoMsg->dpnidPeer);
			}
			else
			{
				// Update the player context entry to reflect the received message
				pPlayerContext = (PPLAYERCONTEXT) pPeerInfoMsg->pvPlayerContext;
				++pPlayerContext->dwPlayerInfoChangeCounter;
			}
		}
		break;
		
	case DPN_MSGID_GROUP_INFO:
		{
			PDPNMSG_GROUP_INFO pGroupInfoMsg;
			
			pGroupInfoMsg = (PDPNMSG_GROUP_INFO) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_GROUP_INFO");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pGroupInfoMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidGroup = %u/0x%08x", pGroupInfoMsg->dpnidGroup, pGroupInfoMsg->dpnidGroup);
				// BUGBUG - verify that the player context matches the player context that was set when
				// this player connected
				xLog(pContext->hLog, XLL_INFO, "     pvGroupContext = 0x%08x", pGroupInfoMsg->pvGroupContext);
			}
		}
		break;
		
	case DPN_MSGID_TERMINATE_SESSION:
		{
			PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
			
			pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION) pvMsg;
		
			TRACE(TEXT("TERMINATE_SESSION message received!\n"));
			xLog(pContext->hLog, XLL_WARN, "TERMINATE_SESSION message received!");

			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_TERMINATE_SESSION");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pTerminateSessionMsg->dwSize);
				// BUGBUG - check the context and verify the difference between the host calling\
				// close and the host calling terminate session.
				xLog(pContext->hLog, XLL_INFO, "     hResultCode = 0x%08x", pTerminateSessionMsg->hResultCode);
				// BUGBUG - check with the context and verify whether or not data should have been provided
				xLog(pContext->hLog, XLL_INFO, "     pvTerminateData = 0x%08x", pTerminateSessionMsg->pvTerminateData);
				xLog(pContext->hLog, XLL_INFO, "     dwTerminateDataSize = %u", pTerminateSessionMsg->dwTerminateDataSize);
			}
			
			// Verify that the received buffer is valid
			if(pTerminateSessionMsg->dwTerminateDataSize)
			{
				if(!VerifyDataBuffer(pTerminateSessionMsg->pvTerminateData, pTerminateSessionMsg->dwTerminateDataSize))
				{
					xLog(pContext->hLog, XLL_WARN, "Received invalid terminate session data! (%u bytes)",
						pTerminateSessionMsg->dwTerminateDataSize);
				}
			}
			
			// Signal the main thread that the session terminated
			SetEvent(pContext->hSessionTerminated);
		}
		break;
		
	case DPN_MSGID_RETURN_BUFFER:
		{
			PDPNMSG_RETURN_BUFFER pReturnBufferMsg;
			
			pReturnBufferMsg = (PDPNMSG_RETURN_BUFFER) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_RETURN_BUFFER");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pReturnBufferMsg->dwSize);
				// BUGBUG - in enum cases, this could be an error, otherwise, should be success
				xLog(pContext->hLog, XLL_INFO, "     hResultCode = 0x%08x", pReturnBufferMsg->hResultCode);
				xLog(pContext->hLog, XLL_INFO, "     pvBuffer = 0x%08x", pReturnBufferMsg->pvBuffer);
				// BUGBUG - this should probably only be set in concurrent enumeration cases
				xLog(pContext->hLog, XLL_INFO, "     pvUserContext = 0x%08x", pReturnBufferMsg->pvUserContext);
			}
			
			if(!pReturnBufferMsg->pvBuffer)
			{
				xLog(pContext->hLog, XLL_WARN, "Return buffer message with invalid buffer!  Signal main app!");
				
				SetEvent(pContext->hCriticalError);
				hr = E_FAIL;
				break;
			}
			
			MemFree(pReturnBufferMsg->pvBuffer);
		}
		break;
		
	case DPN_MSGID_HOST_MIGRATE:
		{
			PDPNMSG_HOST_MIGRATE pHostMigrateMsg;
			
			pHostMigrateMsg = (PDPNMSG_HOST_MIGRATE) pvMsg;
			
			if(pContext->fVerbose)
			{
				xLog(pContext->hLog, XLL_INFO, "DPN_MSGID_HOST_MIGRATE");
				xLog(pContext->hLog, XLL_INFO, "     dwSize = %u", pHostMigrateMsg->dwSize);
				xLog(pContext->hLog, XLL_INFO, "     dpnidNewHost = %u", pHostMigrateMsg->dpnidNewHost);
				xLog(pContext->hLog, XLL_INFO, "     pvPlayerContext = 0x%08x", pHostMigrateMsg->pvPlayerContext);
			}
			
			if(pContext->fIsHost)
			{
				xLog(pContext->hLog, XLL_WARN, "Received host migration message on host!  Signal main app!");
				
				SetEvent(pContext->hCriticalError);
				hr = E_FAIL;
				break;
			}
			
			if(pHostMigrateMsg->dpnidNewHost == pContext->LocalPlayerID)
			{
				xLog(pContext->hLog, XLL_INFO, "Host has migrated to this machine!");
				pContext->fIsHost = TRUE;
			}
			
			SetEvent(pContext->hHostMigrated);
		}
		break;
		
	default:
		{
			xLog(pContext->hLog, XLL_INFO, "Got unexpected message type 0x%08x!  Signal main app!", dwMsgType);
			
			SetEvent(pContext->hCriticalError);
			hr = E_FAIL;
		}
			break;
	} // end switch (on message type)

	dwEndTime = GetTickCount();

	pContext->dwCallbackTime += (dwEndTime >= dwStartTime) ? dwEndTime - dwStartTime : (0xFFFFFFFF - dwStartTime) + dwEndTime;

	return (hr);
} // PeerStressMessageHandler
#undef DEBUG_SECTION

} // namespace DPlayStressNamespace
