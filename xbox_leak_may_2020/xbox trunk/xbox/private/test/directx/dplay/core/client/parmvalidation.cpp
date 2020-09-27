//==================================================================================
// Includes
//==================================================================================
#include "dptest.h"
#include "macros.h"
#include "parmvalidation.h"

using namespace DPlayCoreNamespace;

namespace DPlayCoreNamespace {

#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCompareAppDesc()"
//==================================================================================
// ParmVCompareAppDesc
//----------------------------------------------------------------------------------
//
// Description: Makes sure the two application desc structures are essentially the
//				same.
//
// Arguments:
//	PDPN_APPLICATION_DESC pdpnadCompare		Pointer to app desc to compare.
//	PDPN_APPLICATION_DESC pdpnadExpected	Pointer to app desc to use as reference.
//
// Returns: DPN_OK if they match, a test result error code otherwise.
//==================================================================================
HRESULT ParmVCompareAppDesc(HANDLE hLog, PDPN_APPLICATION_DESC pdpnadCompare, PDPN_APPLICATION_DESC pdpnadExpected)
{
	// Compare the size.
	if (pdpnadCompare->dwSize != sizeof (DPN_APPLICATION_DESC))
	{
		DPTEST_TRACE(hLog, "Application desc size is wrong (%u != %u)!",
			2, pdpnadCompare->dwSize, sizeof (DPN_APPLICATION_DESC));
		return (ERROR_NO_MATCH);
	} // end if (size differs)


	// Compare the flags.
	if (pdpnadCompare->dwFlags != pdpnadExpected->dwFlags)
	{
		DPTEST_TRACE(hLog, "Application desc flags are wrong (%x != %x)!",
			2, pdpnadCompare->dwFlags, pdpnadExpected->dwFlags);
		return (ERROR_NO_MATCH);
	} // end if (flags differ)


	// Can't compare instance GUID, but we can make sure it's not GUID_NULL.
	if (IsZero(&pdpnadCompare->guidApplication, sizeof (GUID)))
	{
		DPTEST_TRACE(hLog, "Application desc instance GUID was not filled in!", 0);
		return (ERROR_NO_DATA);
	} // end if (no instance GUID)


	// Compare the app GUID.
	if (memcmp(&pdpnadCompare->guidApplication, &pdpnadExpected->guidApplication, sizeof (GUID)) != 0)
	{
		DPTEST_TRACE(hLog, "Application desc app GUID is wrong (%g != %g)!",
			2, &pdpnadCompare->guidApplication, &pdpnadExpected->guidApplication);
		return (ERROR_NO_MATCH);
	} // end if (flags differ)


	// Compare the max players.
	if (pdpnadCompare->dwMaxPlayers != pdpnadExpected->dwMaxPlayers)
	{
		DPTEST_TRACE(hLog, "Application desc max players is wrong (%u != %u)!",
			2, pdpnadCompare->dwMaxPlayers, pdpnadExpected->dwMaxPlayers);
		return (ERROR_NO_MATCH);
	} // end if (max players differs)


	// We only expect one player.
	if (pdpnadCompare->dwCurrentPlayers != pdpnadExpected->dwCurrentPlayers)
	{
		DPTEST_TRACE(hLog, "Application desc current players is unexpected (%u != 1)!",
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
				DPTEST_TRACE(hLog, "Application desc session name is wrong (\"%S\"!= \"%S\")!",
					2, pdpnadCompare->pwszSessionName, pdpnadExpected->pwszSessionName);
				return (ERROR_NO_MATCH);
			} // end if (name differs)
		} // end if (received session name)
		else
		{
			DPTEST_TRACE(hLog, "Application desc session name (\"%S\") was not returned!",
				1, pdpnadExpected->pwszSessionName);
			return (ERROR_NO_MATCH);
		} // end if (no name specified)
	} // end if (gave session name)
	else
	{
		if (pdpnadCompare->pwszSessionName != NULL)
		{
			DPTEST_TRACE(hLog, "Unexpected application desc session name was returned (\"%S\")!",
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
				DPTEST_TRACE(hLog, "Application desc password is wrong (\"%S\"!= \"%S\")!",
					2, pdpnadCompare->pwszPassword, pdpnadExpected->pwszPassword);
				return (ERROR_NO_MATCH);
			} // end if (password differs)
		} // end if (received password)
		else
		{
			DPTEST_TRACE(hLog, "Application desc password (\"%S\") was not returned!",
				1, pdpnadExpected->pwszPassword);
			return (ERROR_NO_MATCH);
		} // end if (no password specified)
	} // end if (gave password)
	else
	{
		if (pdpnadCompare->pwszPassword != NULL)
		{
			DPTEST_TRACE(hLog, "Unexpected application desc password was returned (\"%S\")!",
				1, pdpnadCompare->pwszPassword);
			return (ERROR_NO_MATCH);
		} // end if (password returned)
	} // end else (didn't specify password)


	// Reserved data is not allowed or expected.
	if ((pdpnadCompare->pvReservedData != NULL) || (pdpnadCompare->dwReservedDataSize != 0))
	{
		DPTEST_TRACE(hLog, "Application desc reserved data was returned (ptr %x, size %u)!",
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
				DPTEST_TRACE(hLog, "Application desc app reserved data is wrong (%u byte data at \"%x\"!= data at \"%x\")!",
					3, pdpnadCompare->dwApplicationReservedDataSize,
					pdpnadCompare->pvApplicationReservedData,
					pdpnadExpected->pvApplicationReservedData);
				return (ERROR_NO_MATCH);
			} // end if (app reserved data differs)
		} // end if (received app reserved data)
		else
		{
			DPTEST_TRACE(hLog, "Application desc app reserved data (data \"%x\", size %u) was not returned, data ptr was %x and size was %u!",
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
			DPTEST_TRACE(hLog, "Unexpected application desc app reserved data was returned (data \"%x\", size = %u)!",
				2, pdpnadCompare->pvApplicationReservedData,
				pdpnadCompare->dwApplicationReservedDataSize);
			return (ERROR_NO_MATCH);
		} // end if (app reserved data returned)
	} // end else (didn't specify app reserved data)

	return (DPN_OK);
} // ParmVCompareAppDesc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCompareGroupInfo()"
//==================================================================================
// ParmVCompareGroupInfo
//----------------------------------------------------------------------------------
//
// Description: Makes sure the two group info structures are essentially the same.
//
// Arguments:
//	PDPN_GROUP_INFO pdpngiCompare	Pointer to group info to compare.
//	PDPN_GROUP_INFO pdpngiExpected	Pointer to group info to use as reference.
//
// Returns: DPN_OK if they match, a test result error code otherwise.
//==================================================================================
HRESULT ParmVCompareGroupInfo(HANDLE hLog,
							  PDPN_GROUP_INFO pdpngiCompare,
							  PDPN_GROUP_INFO pdpngiExpected)
{
	// Compare the size.
	if (pdpngiCompare->dwSize != sizeof (DPN_GROUP_INFO))
	{
		DPTEST_TRACE(hLog, "Group info size is wrong (%u != %u)!",
			2, pdpngiCompare->dwSize, sizeof (DPN_GROUP_INFO));
		return (ERROR_NO_MATCH);
	} // end if (size differs)


	// Compare the info flags.
	if (pdpngiCompare->dwInfoFlags != pdpngiExpected->dwInfoFlags)
	{
		DPTEST_TRACE(hLog, "Group info info flags are wrong (%x != %x)!",
			2, pdpngiCompare->dwInfoFlags, pdpngiExpected->dwInfoFlags);
		return (ERROR_NO_MATCH);
	} // end if (flags differ)


	// Compare the name.
	if (pdpngiExpected->pwszName != NULL)
	{
		if (pdpngiCompare->pwszName != NULL)
		{
			if (wcscmp(pdpngiCompare->pwszName, pdpngiExpected->pwszName) != 0)
			{
				DPTEST_TRACE(hLog, "Group info name is wrong (\"%S\"!= \"%S\")!",
					2, pdpngiCompare->pwszName, pdpngiExpected->pwszName);
				return (ERROR_NO_MATCH);
			} // end if (name differs)
		} // end if (received group info name)
		else
		{
			DPTEST_TRACE(hLog, "Group info name (\"%S\") was not returned!",
				1, pdpngiExpected->pwszName);
			return (ERROR_NO_MATCH);
		} // end if (no name specified)
	} // end if (gave group info name)
	else
	{
		if (pdpngiCompare->pwszName != NULL)
		{
			DPTEST_TRACE(hLog, "Unexpected group info name was returned (\"%S\")!",
				1, pdpngiCompare->pwszName);
			return (ERROR_NO_MATCH);
		} // end if (name returned)
	} // end else (didn't specify group info name)


	// Compare the data.
	if (pdpngiExpected->pvData != NULL)
	{
		if ((pdpngiCompare->pvData != NULL) &&
			(pdpngiCompare->dwDataSize == pdpngiExpected->dwDataSize))
		{
			if (memcmp(pdpngiCompare->pvData, pdpngiExpected->pvData, pdpngiCompare->dwDataSize) != 0)
			{
				DPTEST_TRACE(hLog, "Group info data is wrong (%u byte data at \"%x\"!= data at \"%x\")!",
					3, pdpngiCompare->dwDataSize, pdpngiCompare->pvData,
					pdpngiExpected->pvData);
				return (ERROR_NO_MATCH);
			} // end if (group info data differs)
		} // end if (received group info data)
		else
		{
			DPTEST_TRACE(hLog, "Group info data (data \"%x\", size %u) was not returned, data ptr was %x and size was %u!",
				4, pdpngiExpected->pvData, pdpngiExpected->dwDataSize,
				pdpngiCompare->pvData, pdpngiCompare->dwDataSize);
			return (ERROR_NO_MATCH);
		} // end if (no group info data specified)
	} // end if (gave group info data)
	else
	{
		if ((pdpngiCompare->pvData != NULL) || (pdpngiCompare->dwDataSize != 0))
		{
			DPTEST_TRACE(hLog, "Unexpected group info data was returned (data \"%x\", size = %u)!",
				2, pdpngiCompare->pvData, pdpngiCompare->dwDataSize);
			return (ERROR_NO_MATCH);
		} // end if (group info data returned)
	} // end else (didn't specify group info data)


	// Compare the group flags.
	if (pdpngiCompare->dwGroupFlags != pdpngiExpected->dwGroupFlags)
	{
		DPTEST_TRACE(hLog, "Group info group flags are wrong (%x != %x)!",
			2, pdpngiCompare->dwGroupFlags, pdpngiExpected->dwGroupFlags);
		return (ERROR_NO_MATCH);
	} // end if (flags differ)


	return (DPN_OK);
} // ParmVCompareGroupInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVComparePlayerInfo()"
//==================================================================================
// ParmVComparePlayerInfo
//----------------------------------------------------------------------------------
//
// Description: Makes sure the two player info structures are essentially the same.
//
// Arguments:
//	PDPN_PLAYER_INFO pdpnpiCompare		Pointer to player info to compare.
//	PDPN_PLAYER_INFO pdpnpiExpected		Pointer to player info to use as reference.
//
// Returns: DPN_OK if they match, a test result error code otherwise.
//==================================================================================
HRESULT ParmVComparePlayerInfo(HANDLE hLog,
							   PDPN_PLAYER_INFO pdpnpiCompare,
							   PDPN_PLAYER_INFO pdpnpiExpected)
{
	// Compare the size.
	if (pdpnpiCompare->dwSize != sizeof (DPN_PLAYER_INFO))
	{
		DPTEST_TRACE(hLog, "Player info size is wrong (%u != %u)!",
			2, pdpnpiCompare->dwSize, sizeof (DPN_PLAYER_INFO));
		return (ERROR_NO_MATCH);
	} // end if (size differs)


	// Compare the info flags.
	if (pdpnpiCompare->dwInfoFlags != pdpnpiExpected->dwInfoFlags)
	{
		DPTEST_TRACE(hLog, "Player info info flags are wrong (%x != %x)!",
			2, pdpnpiCompare->dwInfoFlags, pdpnpiExpected->dwInfoFlags);
		return (ERROR_NO_MATCH);
	} // end if (flags differ)


	// Compare the name.
	if (pdpnpiExpected->pwszName != NULL)
	{
		if (pdpnpiCompare->pwszName != NULL)
		{
			if (wcscmp(pdpnpiCompare->pwszName, pdpnpiExpected->pwszName) != 0)
			{
				DPTEST_TRACE(hLog, "Player info name is wrong (\"%S\"!= \"%S\")!",
					2, pdpnpiCompare->pwszName, pdpnpiExpected->pwszName);
				return (ERROR_NO_MATCH);
			} // end if (name differs)
		} // end if (received player info name)
		else
		{
			DPTEST_TRACE(hLog, "Player info name (\"%S\") was not returned!",
				1, pdpnpiExpected->pwszName);
			return (ERROR_NO_MATCH);
		} // end if (no name specified)
	} // end if (gave player info name)
	else
	{
		if (pdpnpiCompare->pwszName != NULL)
		{
			DPTEST_TRACE(hLog, "Unexpected player info name was returned (\"%S\")!",
				1, pdpnpiCompare->pwszName);
			return (ERROR_NO_MATCH);
		} // end if (name returned)
	} // end else (didn't specify player info name)


	// Compare the data.
	if (pdpnpiExpected->pvData != NULL)
	{
		if ((pdpnpiCompare->pvData != NULL) &&
			(pdpnpiCompare->dwDataSize == pdpnpiExpected->dwDataSize))
		{
			if (memcmp(pdpnpiCompare->pvData, pdpnpiExpected->pvData, pdpnpiCompare->dwDataSize) != 0)
			{
				DPTEST_TRACE(hLog, "Player info data is wrong (%u byte data at \"%x\"!= data at \"%x\")!",
					3, pdpnpiCompare->dwDataSize, pdpnpiCompare->pvData,
					pdpnpiExpected->pvData);
				return (ERROR_NO_MATCH);
			} // end if (v info data differs)
		} // end if (received player info data)
		else
		{
			DPTEST_TRACE(hLog, "Player info data (data \"%x\", size %u) was not returned, data ptr was %x and size was %u!",
				4, pdpnpiExpected->pvData, pdpnpiExpected->dwDataSize,
				pdpnpiCompare->pvData, pdpnpiCompare->dwDataSize);
			return (ERROR_NO_MATCH);
		} // end if (no player info data specified)
	} // end if (gave player info data)
	else
	{
		if ((pdpnpiCompare->pvData != NULL) || (pdpnpiCompare->dwDataSize != 0))
		{
			DPTEST_TRACE(hLog, "Unexpected player info data was returned (data \"%x\", size = %u)!",
				2, pdpnpiCompare->pvData, pdpnpiCompare->dwDataSize);
			return (ERROR_NO_MATCH);
		} // end if (player info data returned)
	} // end else (didn't specify player info data)


	// Compare the player flags.
	if (pdpnpiCompare->dwPlayerFlags != pdpnpiExpected->dwPlayerFlags)
	{
		DPTEST_TRACE(hLog, "Player info group flags are wrong (%x != %x)!",
			2, pdpnpiCompare->dwPlayerFlags, pdpnpiExpected->dwPlayerFlags);
		return (ERROR_NO_MATCH);
	} // end if (flags differ)


	return (DPN_OK);
} // ParmVComparePlayerInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCreatePeerHost()"
//==================================================================================
// ParmVCreatePeerHost
//----------------------------------------------------------------------------------
//
// Description: Creates a new CWrapDP8Peer object that is hosting based on the given
//				message handler and application description.  The first hosting
//				address can optionally also be returned.
//
// Arguments:
//	PFNDPNMESSAGEHANDLER pfn				Message handler to use.
//	PVOID pvContext							Pointer to context for message handler.
//	PDPN_APPLICATION_DESC pdpnad			Pointer to application desc to use when
//											hosting the session
//	PWRAPDP8PEER* ppDP8PeerHost				Place to store pointer to CWrapDP8Peer
//											object created.
//	PVOID pvPlayerContext					Player context for local host player.
//	PDIRECTPLAY8ADDRESS* ppDP8AddressHost	Optional place to store pointer to first
//											address the object is hosting on.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT ParmVCreatePeerHost(HANDLE hLog,
							PFNDPNMESSAGEHANDLER pfn,
							PVOID pvContext,
							PDPN_APPLICATION_DESC pdpnad,
							PVOID pvPlayerContext,
							PWRAPDP8PEER* ppDP8PeerHost,
							PDIRECTPLAY8ADDRESS* ppDP8AddressHost)
{
	HRESULT					hr;
	PDIRECTPLAY8ADDRESS		pDP8AddressDevice = NULL;
	PDIRECTPLAY8ADDRESS*	paDP8HostAddresses = NULL;
	DWORD					dwNumAddresses = 0;



	if ((*ppDP8PeerHost) != NULL)
	{
		DPTEST_TRACE(hLog, "Already have peer object (%x)!", 1, (*ppDP8PeerHost));
		return (ERROR_INVALID_PARAMETER);
	} // end if (already have object)


	(*ppDP8PeerHost) = new CWrapDP8Peer(hLog);
	if ((*ppDP8PeerHost) == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate object)


	hr = (*ppDP8PeerHost)->CoCreate();
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "CoCreating DP8Peer object failed!", 0);
		goto ERROR_EXIT;
	} // end if (function failed)


	hr = (*ppDP8PeerHost)->DP8P_Initialize(pvContext, pfn, 0);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Initializing DP8Peer object failed!", 0);
		goto ERROR_EXIT;
	} // end if (function failed)



	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressDevice, NULL);
	if (hr != S_OK)
	{
		DPTEST_TRACE(hLog, "Couldn't DirectPlay8AddressCreate host DirectPlay8Address object!", 0);
		goto ERROR_EXIT;
	} // end if (DirectPlay8AddressCreate failed)


//	hr = pDP8AddressDevice->SetSP(&CLSID_DP8SP_TCPIP);
// BUGBUG - modified for Xbox version
//	hr = pDP8AddressDevice->SetSP();
//	if (hr != S_OK)
//	{
//		DPTEST_TRACE(hLog, "Couldn't set DirectPlay8Address SP!", 0);
//		goto ERROR_EXIT;
//	} // end if (set SP failed)

	hr = (*ppDP8PeerHost)->DP8P_Host(pdpnad, &pDP8AddressDevice, 1, NULL, NULL,
									pvPlayerContext, 0);
	if (hr != DPN_OK)
	{
		DPTEST_TRACE(hLog, "Hosting failed!", 0);
		goto ERROR_EXIT;
	} // end if (function failed)

	pDP8AddressDevice->Release();
	pDP8AddressDevice = NULL;

	// If the caller wants an address, give one to him.
	if (ppDP8AddressHost != NULL)
	{
		// Ignore error
		hr = (*ppDP8PeerHost)->DP8P_GetLocalHostAddresses(NULL, &dwNumAddresses, 0);


		paDP8HostAddresses = (PDIRECTPLAY8ADDRESS*) MemAlloc(dwNumAddresses * sizeof (PDIRECTPLAY8ADDRESS));
		if (paDP8HostAddresses == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)


		hr = (*ppDP8PeerHost)->DP8P_GetLocalHostAddresses(paDP8HostAddresses,
														&dwNumAddresses,
														0);
		if (hr != DPN_OK)
		{
			DPTEST_TRACE(hLog, "Getting local addresses failed!", 0);
			goto ERROR_EXIT;
		} // end if (getting addresses failed)


		// Move the first address to the caller's pointer.
		(*ppDP8AddressHost) = paDP8HostAddresses[0];
		paDP8HostAddresses[0] = NULL;


		// Release all the other addresses.
		while (dwNumAddresses > 1)
		{
			dwNumAddresses--;
			paDP8HostAddresses[dwNumAddresses]->Release();
			paDP8HostAddresses[dwNumAddresses] = NULL;
		} // end while (more addresses)


		MemFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (caller wants address)

	return (DPN_OK);


ERROR_EXIT:


	if (paDP8HostAddresses != NULL)
	{
		MemFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (allocated address array)

	if (pDP8AddressDevice != NULL)
	{
		pDP8AddressDevice->Release();
		pDP8AddressDevice = NULL;
	} // end if (allocated address object)

	if ((*ppDP8PeerHost) != NULL)
	{
		delete (*ppDP8PeerHost);
		(*ppDP8PeerHost) = NULL;
	} // end if (have wrapper object)

	return (hr);
} // ParmVCreatePeerHost
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCreateAndConnectClient()"
//==================================================================================
// ParmVCreateAndConnectClient
//----------------------------------------------------------------------------------
//
// Description: Creates a new CWrapDP8Server object that is hosting based on the
//				given message handler and application description.  The first
//				hosting address can optionally also be returned.
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//	PFNDPNMESSAGEHANDLER pfn		Message handler to use.
//	PVOID pvContext					Pointer to context for message handler.
//	PWRAPDP8SERVER pDP8Server		Pointer to server object to connect to.
//	PDPN_APPLICATION_DESC pdpnad	Pointer to application desc to use when
//									connecting to the session
//	HANDLE hClientCreatedEvent		Handle to event that will be set when client's
//									CREATE_PLAYER is indicated on the server.
//	PWRAPDP8CLIENT* ppDP8Client		Place to store pointer to CWrapDP8Client object
//									created.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT ParmVCreateAndConnectClient(HANDLE hLog,
									PFNDPNMESSAGEHANDLER pfn,
									PVOID pvContext,
									PWRAPDP8SERVER pDP8Server,
									PDPN_APPLICATION_DESC pdpnad,
									HANDLE hClientCreatedEvent,
									PWRAPDP8CLIENT* ppDP8Client)
{
	HRESULT					hr;
	PDIRECTPLAY8ADDRESS*	paDP8HostAddresses = NULL;
	DWORD					dwNumAddresses = 0;
	DP_DOWORKLIST			DoWorkList;
	DPNHANDLE				hAsyncOp;



	if ((*ppDP8Client) != NULL)
	{
		DPTEST_FAIL(hLog, "Already have client object (%x)!", 1, (*ppDP8Client));
		return (ERROR_INVALID_PARAMETER);
	} // end if (already have object)


	(*ppDP8Client) = new CWrapDP8Client(hLog);
	if ((*ppDP8Client) == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate object)


	hr = (*ppDP8Client)->CoCreate();
	if (hr != DPN_OK)
	{
		DPTEST_FAIL(hLog, "CoCreating DP8Client object failed!", 0);
		goto ERROR_EXIT;
	} // end if (function failed)


	hr = (*ppDP8Client)->DP8C_Initialize(pvContext, pfn, 0);
	if (hr != DPN_OK)
	{
		DPTEST_FAIL(hLog, "Initializing DP8Client object failed!", 0);
		goto ERROR_EXIT;
	} // end if (function failed)



	// Ignore error
	pDP8Server->DP8S_GetLocalHostAddresses(NULL, &dwNumAddresses, 0);


	paDP8HostAddresses = (PDIRECTPLAY8ADDRESS*) MemAlloc(dwNumAddresses * sizeof (PDIRECTPLAY8ADDRESS));
	if (paDP8HostAddresses == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)


	hr = pDP8Server->DP8S_GetLocalHostAddresses(paDP8HostAddresses,
												&dwNumAddresses,
												0);
	if (hr != DPN_OK)
	{
		DPTEST_FAIL(hLog, "Getting local addresses failed!", 0);
		goto ERROR_EXIT;
	} // end if (getting addresses failed)


	// Connect using the first address returned.
	hr = (*ppDP8Client)->DP8C_Connect(pdpnad,
									paDP8HostAddresses[0],
									NULL,
									NULL,
									NULL,
									NULL,
									0,
									NULL,
									&hAsyncOp,
									0);

	if((hr != DPNSUCCESS_PENDING) && (hr != DPN_OK))
	{
		DPTEST_FAIL(hLog, "Connecting to host failed!", 0);
		goto ERROR_EXIT;
	} // end if (connecting failed)


	// Release all the addresses.
	while (dwNumAddresses > 0)
	{
		dwNumAddresses--;
		paDP8HostAddresses[dwNumAddresses]->Release();
		paDP8HostAddresses[dwNumAddresses] = NULL;
	} // end while (more addresses)


	MemFree(paDP8HostAddresses);
	paDP8HostAddresses = NULL;



	//
	// Make sure the client connection is fully notified on the server.
	//
	// NOTE: We're using the control layer WaitForEventOrCancel function which
	// returns a system result, which we'll return directly from this function.
	// Everyone who currently calls this function will remap it to a test result.
	// We can't use the THROW_TESTRESULT/THROW_SYSTEMRESULT macros to differentiate
	// because those don't work outside a BEGIN_TESTCASE-END_TESTCASE pair.
	//
	// We shouldn't be failing anyway, but I wanted to be clear about the price
	// we're paying for not making this a subtest case.
	//

	memset(&DoWorkList, 0, sizeof(DP_DOWORKLIST));
	DoWorkList.fTrackDoWorkTimes = FALSE;
	DoWorkList.dwNumClients = 1;
	DoWorkList.dwNumServers = 1;
	DoWorkList.apDP8Clients = (PDIRECTPLAY8CLIENT *) MemAlloc(sizeof(LPVOID) * 1);
	DoWorkList.apDP8Clients[0] = (*ppDP8Client)->m_pDP8Client;
	DoWorkList.apDP8Servers = (PDIRECTPLAY8SERVER *) MemAlloc(sizeof(LPVOID) * 1);
	DoWorkList.apDP8Servers[0] = pDP8Server->m_pDP8Server;

	if(!PollUntilEventSignalled(hLog, &DoWorkList, 60000, hClientCreatedEvent))
	{
		DPTEST_FAIL(hLog, "Polling for asynchronous operation failed!", 0);
		goto ERROR_EXIT;
	}

	MemFree(DoWorkList.apDP8Clients);
	MemFree(DoWorkList.apDP8Servers);

	// The client should be connected, and it should be safe for the server to
	// refer to his ID.
	

	return (DPN_OK);


ERROR_EXIT:


	if (paDP8HostAddresses != NULL)
	{
		// Release the addresses, if any.
		if (paDP8HostAddresses[0] != NULL)
		{
			while (dwNumAddresses > 0)
			{
				dwNumAddresses--;
				paDP8HostAddresses[dwNumAddresses]->Release();
				paDP8HostAddresses[dwNumAddresses] = NULL;
			} // end while (more addresses)
		} // end if (any addresses)

		MemFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (allocated address array)

	if ((*ppDP8Client) != NULL)
	{
		delete (*ppDP8Client);
		(*ppDP8Client) = NULL;
	} // end if (have wrapper object)

	return (hr);
} // ParmVCreateAndConnectClient
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVCreateServer()"
//==================================================================================
// ParmVCreateServer
//----------------------------------------------------------------------------------
//
// Description: Creates a new CWrapDP8Server object that is hosting based on the
//				given message handler and application description.  The first
//				hosting address can optionally also be returned.
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//	PFNDPNMESSAGEHANDLER pfn				Message handler to use.
//	PVOID pvContext							Pointer to context for message handler.
//	PDPN_APPLICATION_DESC pdpnad			Pointer to application desc to use when
//											hosting the session
//	DPNID* pdpnidPlayerContext				Pointer to player context, which will
//											also be used to store the server's
//											player ID from the CREATE_PLAYER
//											indication.  
//	PWRAPDP8SERVER* ppDP8Server				Place to store pointer to CWrapDP8Server
//											object created.
//	PDIRECTPLAY8ADDRESS* ppDP8AddressHost	Optional place to store pointer to first
//											address the object is hosting on.
//
// Returns: DPN_OK if successful, error code otherwise.
//==================================================================================
HRESULT ParmVCreateServer(HANDLE hLog,
						PFNDPNMESSAGEHANDLER pfn,
						PVOID pvContext,
						PDPN_APPLICATION_DESC pdpnad,
						DPNID* pdpnidPlayerContext,
						PWRAPDP8SERVER* ppDP8Server,
						PDIRECTPLAY8ADDRESS* ppDP8AddressHost)
{
	HRESULT					hr;
	PDIRECTPLAY8ADDRESS		pDP8AddressDevice = NULL;
	PDIRECTPLAY8ADDRESS*	paDP8HostAddresses = NULL;
	DWORD					dwNumAddresses = 0;



	if ((*ppDP8Server) != NULL)
	{
		DPTEST_FAIL(hLog, "Already have server object (%x)!", 1, (*ppDP8Server));
		return (ERROR_INVALID_PARAMETER);
	} // end if (already have object)


	(*ppDP8Server) = new CWrapDP8Server(hLog);
	if ((*ppDP8Server) == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate object)


	hr = (*ppDP8Server)->CoCreate();
	if (hr != DPN_OK)
	{
		DPTEST_FAIL(hLog, "CoCreating DP8Server object failed!", 0);
		goto ERROR_EXIT;
	} // end if (function failed)


	hr = (*ppDP8Server)->DP8S_Initialize(pvContext, pfn, 0);
	if (hr != DPN_OK)
	{
		DPTEST_FAIL(hLog, "Initializing DP8Server object failed!", 0);
		goto ERROR_EXIT;
	} // end if (function failed)



	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (LPVOID *) &pDP8AddressDevice, NULL);
	if (hr != S_OK)
	{
		DPTEST_FAIL(hLog, "Couldn't create host DirectPlay8Address object!", 0);
		goto ERROR_EXIT;
	} // end if (create failed)


	hr = pDP8AddressDevice->SetSP(&CLSID_DP8SP_TCPIP);
	if (hr != S_OK)
	{
		DPTEST_FAIL(hLog, "Couldn't set DirectPlay8Address SP!", 0);
		goto ERROR_EXIT;
	} // end if (set SP failed)


	hr = (*ppDP8Server)->DP8S_Host(pdpnad, &pDP8AddressDevice, 1, NULL, NULL,
									pdpnidPlayerContext, 0);
	if (hr != DPN_OK)
	{
		DPTEST_FAIL(hLog, "Hosting failed!", 0);
		goto ERROR_EXIT;
	} // end if (function failed)

	if ((*pdpnidPlayerContext) == 0)
	{
		DPTEST_FAIL(hLog, "Player ID not set (CREATE_PLAYER not indicated?)!", 0);
		hr = ERROR_NO_DATA;
		goto ERROR_EXIT;
	} // end if (player ID not set)

	pDP8AddressDevice->Release();
	pDP8AddressDevice = NULL;


	// If the caller wants an address, give one to him.
	if (ppDP8AddressHost != NULL)
	{
		// Ignore error
		(*ppDP8Server)->DP8S_GetLocalHostAddresses(NULL, &dwNumAddresses, 0);


		paDP8HostAddresses = (PDIRECTPLAY8ADDRESS*) MemAlloc(dwNumAddresses * sizeof (PDIRECTPLAY8ADDRESS));
		if (paDP8HostAddresses == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)


		hr = (*ppDP8Server)->DP8S_GetLocalHostAddresses(paDP8HostAddresses,
														&dwNumAddresses,
														0);
		if (hr != DPN_OK)
		{
			DPTEST_FAIL(hLog, "Getting local addresses failed!", 0);
			goto ERROR_EXIT;
		} // end if (getting addresses failed)


		// Move the first address to the caller's pointer.
		(*ppDP8AddressHost) = paDP8HostAddresses[0];
		paDP8HostAddresses[0] = NULL;


		// Release all the other addresses.
		while (dwNumAddresses > 1)
		{
			dwNumAddresses--;
			paDP8HostAddresses[dwNumAddresses]->Release();
			paDP8HostAddresses[dwNumAddresses] = NULL;
		} // end while (more addresses)


		MemFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (caller wants address)


	return (DPN_OK);


ERROR_EXIT:


	if (paDP8HostAddresses != NULL)
	{
		MemFree(paDP8HostAddresses);
		paDP8HostAddresses = NULL;
	} // end if (allocated address array)

	if (pDP8AddressDevice != NULL)
	{
		pDP8AddressDevice->Release();
		pDP8AddressDevice = NULL;
	} // end if (allocated address object)

	if ((*ppDP8Server) != NULL)
	{
		delete (*ppDP8Server);
		(*ppDP8Server) = NULL;
	} // end if (have wrapper object)

	// Reset this, just in case.
	(*pdpnidPlayerContext) = 0;

	return (hr);
} // ParmVCreateServer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ParmVNoMessagesDPNMessageHandler()"
//==================================================================================
// ParmVNoMessagesDPNMessageHandler
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
HRESULT ParmVNoMessagesDPNMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	DPTEST_TRACE(NULL, "Got unexpected message type %x!  DEBUGBREAK()-ing.",
		1, dwMsgType);
	DEBUGBREAK();

	return (E_NOTIMPL);
} // ParmVNoMessagesDPNMessageHandler
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


#undef DEBUG_SECTION
#define DEBUG_SECTION	"StringCmpAToU()"
//==================================================================================
// StringCmpAToU
//----------------------------------------------------------------------------------
//
// Description: Performs similar to strcmp, except the first string is Unicode, and
//				it returns TRUE if they match (case insensitive, if specified),
//				FALSE otherwise.
//
// Arguments:
//	HANDLE hLog					Handle to logging subsystem
//	WCHAR* wszUnicodeString		Unicode string to compare.
//	char* szANSIString			ANSI string to compare.
//	BOOL fMatchCase				Whether case matters or not.
//
// Returns: Returns TRUE if they match, FALSE otherwise.
//==================================================================================
BOOL StringCmpAToU(HANDLE hLog, WCHAR* wszUnicodeString, char* szANSIString, BOOL fMatchCase)
{
	BOOL	fResult = FALSE;
	int		i;
	WCHAR*	pwszANSIAsUnicode = NULL;
	WCHAR*	pwszUnicodeLwr = NULL;
	WCHAR*	pwcCurrent1 = NULL;
	WCHAR*	pwcCurrent2 = NULL;



	i = MultiByteToWideChar(CP_ACP, 0, szANSIString, -1, NULL, 0);
	if (i <= 0)
	{
		DPTEST_FAIL(hLog, "Couldn't get size of ANSI string in Unicode!  %u", 1, GetLastError());

		goto DONE;
	} // end if (couldn't convert string)


	pwszANSIAsUnicode = (WCHAR*) MemAlloc(i * sizeof (WCHAR));
	if (pwszANSIAsUnicode == NULL)
	{
		goto DONE;
	} // end if (couldn't allocate memory)
	ZeroMemory(pwszANSIAsUnicode, i * sizeof (WCHAR));

	i = MultiByteToWideChar(CP_ACP, 0, szANSIString, -1, pwszANSIAsUnicode, i);
	if (i <= 0)
	{
		DPTEST_FAIL(hLog, "Couldn't convert string to Unicode!  %u", 1, GetLastError());

		goto DONE;
	} // end if (the function failed)


	// Make strings lower case for case-insensitive matching, if requested
	if (! fMatchCase)
	{
		_wcslwr(pwszANSIAsUnicode);

		pwszUnicodeLwr = (WCHAR*) MemAlloc((wcslen(wszUnicodeString) + 1) * sizeof (WCHAR));
		if (pwszUnicodeLwr == NULL)
		{
			goto DONE;
		} // end if (couldn't allocate memory)
		wcscpy(pwszUnicodeLwr, wszUnicodeString);
		_wcslwr(pwszUnicodeLwr);
	} // end if (case insensitive)

	// Compare the strings
	fResult = ((wcscmp((fMatchCase ? wszUnicodeString : pwszUnicodeLwr),
				pwszANSIAsUnicode)) == 0) ? TRUE : FALSE;


DONE:

	if (pwszANSIAsUnicode != NULL)
	{
		MemFree(pwszANSIAsUnicode);
		pwszANSIAsUnicode = NULL;
	} // end if (allocated string)

	if (pwszUnicodeLwr != NULL)
	{
		MemFree(pwszUnicodeLwr);
		pwszUnicodeLwr = NULL;
	} // end if (allocated string)

	return (fResult);
} // StringCmpAToU
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#undef DEBUG_SECTION
#define DEBUG_SECTION	"ClearDoWorkList()"
//==================================================================================
// ClearDoWorkList
//----------------------------------------------------------------------------------
//
// Description: Releases any buffers allocated inside the DoWorkList
//
// Arguments:
//	DP_DOWORKLIST		*pDoWorkList		DoWorkList structure to be cleared
//
// Returns: nothing
//==================================================================================
VOID ClearDoWorkList(DP_DOWORKLIST *pDoWorkList)
{
	if(pDoWorkList->apDP8Clients)
	{
		MemFree(pDoWorkList->apDP8Clients);
		pDoWorkList->apDP8Clients = NULL;
	}

	if(pDoWorkList->apDP8Servers)
	{
		MemFree(pDoWorkList->apDP8Servers);
		pDoWorkList->apDP8Servers = NULL;
	}

	if(pDoWorkList->apDP8Peers)
	{
		MemFree(pDoWorkList->apDP8Peers);
		pDoWorkList->apDP8Peers = NULL;
	}

	if(pDoWorkList->apdwClientCallbackTimes)
	{
		MemFree(pDoWorkList->apdwClientCallbackTimes);
		pDoWorkList->apdwClientCallbackTimes = NULL;
	}

	if(pDoWorkList->apdwServerCallbackTimes)
	{
		MemFree(pDoWorkList->apdwServerCallbackTimes);
		pDoWorkList->apdwServerCallbackTimes = NULL;
	}

	if(pDoWorkList->apdwPeerCallbackTimes)
	{
		MemFree(pDoWorkList->apdwPeerCallbackTimes);
		pDoWorkList->apdwPeerCallbackTimes = NULL;
	}

	return;
} // StringCmpAToU
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

} // namespace DPlayCoreNamespace