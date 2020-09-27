#ifndef _XBOX // ! no master supported
//==================================================================================
// Includes
//==================================================================================
#define INCL_WINSOCK_API_TYPEDEFS 1 // includes winsock2 fn proto's, for getprocaddress
#ifndef _XBOX
#include <winsock2.h>
#else // ! XBOX
#include <winsockx.h>		// Needed for XnetInitialize
#endif // XBOX
#include <windows.h>

#include <time.h>


#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\strutils.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\errors.h"

#include "tncontrl.h"
#include "main.h"
#include "sendq.h"
#include "comm.h"
#include "commtcp.h"
#include "masterinfo.h"
#include "meta.h"







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMetaMaster::CTNMetaMaster()"
//==================================================================================
// CTNMetaMaster constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNMetaMaster object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNMetaMaster::CTNMetaMaster(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNMetaMaster));
} // CTNMetaMaster::CTNMetaMaster
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMetaMaster::~CTNMetaMaster()"
//==================================================================================
// CTNMetaMaster destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNMetaMaster object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNMetaMaster::~CTNMetaMaster(void)
{
	HRESULT				hr;


	//DPL(0, "this = %x", 1, this);

	hr = this->CleanupMetaMaster();
	if (hr != S_OK)
	{
		DPL(0, "Cleaning up failed!  %e", 1, hr);
	} // end if (cleaning up failed)
} // CTNMetaMaster::~CTNMetaMaster
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMetaMaster::StartupMetaMaster()"
//==================================================================================
// CTNMetaMaster::StartupMetaMaster
//----------------------------------------------------------------------------------
//
// Description: Starts up the meta master and prepares it for its job.
//
// Arguments:
//	PTNSTARTUPMETAMASTERDATA pStartupMetaMasterData		Parameter block.  See header
//														file for information.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMetaMaster::StartupMetaMaster(PTNSTARTUPMETAMASTERDATA pStartupMetaMasterData)
{
	HRESULT					hr;
	CONTROLCOMMINITPBLOCK	ctrlcomminitparams;


	if (this == NULL)
	{
		DPL(0, "Meta-master object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we got passed a bad pointer)

	if (pStartupMetaMasterData->dwSize != sizeof (TNSTARTUPMETAMASTERDATA))
	{
		DPL(0, "Must pass a structure of correct size!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (got passed an invalid parameter)

	if (pStartupMetaMasterData->dwAPIVersion != CURRENT_TNCONTROL_API_VERSION)
	{
		DPL(0, "WARNING: Caller requesting different API version (%i != %i), problems may occur.",
			2, pStartupMetaMasterData->dwAPIVersion, CURRENT_TNCONTROL_API_VERSION);
	} // end if (got passed an invalid parameter)

	if ((pStartupMetaMasterData->dwControlMethodID != TN_CTRLMETHOD_TCPIP_OPTIMAL) &&
		(pStartupMetaMasterData->dwControlMethodID != TN_CTRLMETHOD_TCPIP_WINSOCK1))
	{
		DPL(0, "Must pass a control method ID!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (got passed an invalid parameter)

	/*
	this->m_hAllSlavesCommStatusEvent = CreateEvent(NULL, false, false, NULL);
	if (this->m_hAllSlavesCommStatusEvent == NULL)
	{
		DPL(0, "Failed to create comm status event!", 0);
		return (GetLastError());
	} // end if (failed to create event)
	*/

	ZeroMemory(&ctrlcomminitparams, sizeof (CONTROLCOMMINITPBLOCK));
	ctrlcomminitparams.dwSize = sizeof (CONTROLCOMMINITPBLOCK);
	ctrlcomminitparams.dwControlMethodID = pStartupMetaMasterData->dwControlMethodID;
	ctrlcomminitparams.pControlLayerObj = this;
	ctrlcomminitparams.fMaster = TRUE; // we're accepting data, so we're considered a master
	ctrlcomminitparams.dwFlags = pStartupMetaMasterData->dwMethodFlags;
	ctrlcomminitparams.pvData = pStartupMetaMasterData->pvMethodData;
	ctrlcomminitparams.dwDataSize = pStartupMetaMasterData->dwMethodDataSize;

	// Initialize desired control method
	switch (pStartupMetaMasterData->dwControlMethodID)
	{
		case TN_CTRLMETHOD_TCPIP_OPTIMAL:
			hr = CtrlCommTCPLoadOptimal(&ctrlcomminitparams, &(this->m_pCtrlComm));
			if (hr != S_OK)
			{
				DPL(0, "Initializing TCP/IP optimal control method failed!", 0);
				return (hr);
			} // end if (initting TCP/IP optimal control method failed)
		break;

		case TN_CTRLMETHOD_TCPIP_WINSOCK1:
			hr = CtrlCommTCPLoadWinSock1(&ctrlcomminitparams, &(this->m_pCtrlComm));
			if (hr != S_OK)
			{
				DPL(0, "Initializing TCP/IP WinSock1 control method failed!", 0);
				return (hr);
			} // end if (initting TCP/IP WinSock1 control method failed)
		  break;
	} // end switch (on control method ID)

	this->m_pfnLogString = pStartupMetaMasterData->pfnLogString;
	if (pStartupMetaMasterData->hCompletionOrUpdateEvent != NULL)
	{
		if (! DuplicateHandle(GetCurrentProcess(), pStartupMetaMasterData->hCompletionOrUpdateEvent,
								GetCurrentProcess(), &(this->m_hCompletionOrUpdateEvent),
								0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			hr = GetLastError();
			DPL(0, "Couldn't duplicate user's completion or update event handle (%x)!",
				2, pStartupMetaMasterData->hCompletionOrUpdateEvent);
			return (hr);
		} // end if (couldn't duplicate handle)
	} // end if (there's an event)

	// Initialize control communication method
	hr = this->m_pCtrlComm->Initialize();
	if (hr != S_OK)
	{
		DPL(0, "Initializing control communications method failed!", 0);
		return (hr);
	} // end if (initting ctrl comm failed)


	// Do a stats refresh, even though we don't keep any stats there.  This is used
	// to let the app know the meta-master is ready.
	if ((this->m_hCompletionOrUpdateEvent != NULL) &&
		(! SetEvent(this->m_hCompletionOrUpdateEvent)))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set the user's update event (%x)!",
			1, this->m_hCompletionOrUpdateEvent);
		return (hr);
	} // end if (couldn't set the user's update event)

	return (S_OK);
} // CTNMetaMaster::StartupMetaMaster
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMetaMaster::CleanupMetaMaster()"
//==================================================================================
// CTNMetaMaster::CleanupMetaMaster
//----------------------------------------------------------------------------------
//
// Description: Shuts down this meta master and releases the ctrl comm object if
//				it existed.
//
// Arguments: None.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMetaMaster::CleanupMetaMaster(void)
{
	HRESULT			hr = S_OK;
	PTNMASTERINFO	pMaster = NULL;

	
	if (this == NULL)
	{
		DPL(0, "Meta-master object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we got passed a bad pointer)

	this->m_masters.EnterCritSection();
	while (this->m_masters.Count() > 0)
	{
		pMaster = (PTNMASTERINFO) this->m_masters.GetItem(0);
		if (pMaster == NULL)
		{
			DPL(0, "Couldn't get first master in list!", 0);
			break;
		} // end if (couldn't get item)

		hr = this->m_pCtrlComm->UnbindDataFromAddress(&(pMaster->m_commdata));
		if (hr != S_OK)
		{
			DPL(0, "Unbinding master's comm data failed!  %e", 1, hr);
		} // end if (releasing control method failed)

		hr = this->m_masters.Remove(0);
		if (hr != S_OK)
		{
			DPL(0, "Removing master from list failed!  %e", 1, hr);
		} // end if (releasing control method failed)
	} // end for (each master)
	this->m_masters.LeaveCritSection();

	if (this->m_pCtrlComm != NULL)
	{
		hr = this->m_pCtrlComm->Release();
		if (hr != S_OK)
		{
			DPL(0, "Releasing control communication method failed!  %e", 1, hr);
		} // end if (releasing control method failed)

		delete (this->m_pCtrlComm);
		this->m_pCtrlComm = NULL;
	} // end if (we're still validly connected to the control method)

	/*
	if (this->m_hAllSlavesCommStatusEvent != NULL)
	{
		CloseHandle(this->m_hAllSlavesCommStatusEvent);
		this->m_hAllSlavesCommStatusEvent = NULL;
	} // end if (we created a comm status event)
	*/

	if (this->m_hCompletionOrUpdateEvent != NULL)
	{
		CloseHandle(this->m_hCompletionOrUpdateEvent);
		this->m_hCompletionOrUpdateEvent = NULL;
	} // end if (have event)

	return (hr);
} // CTNMetaMaster::CleanupMetaMaster
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMetaMaster::HandleMessage()"
//==================================================================================
// CTNMetaMaster::HandleMessage
//----------------------------------------------------------------------------------
//
// Description: Handles an incoming control session message.
//
// Arguments:
//	PVOID* ppvFromAddress		Pointer to pointer to data describing ctrl comm
//								from address.
//	DWORD dwFromAddressSize		Size of from address data.
//	PVOID pvData				Pointer to data received by ctrl comm.
//	DWORD dwDataSize			Size of data.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMetaMaster::HandleMessage(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PVOID pvData, DWORD dwDataSize)
{
	PCTRLMSG_GENERIC	pGenericMsg = NULL;


	if ((*ppvFromAddress) == NULL)
	{
		DPL(0, "WARNING!  No from address!", 0);
	} // end if (no from address info)

	if (dwDataSize < sizeof (CTRLMSG_GENERIC))
	{
		DPL(0, "Data too small to be valid message!  Ignoring.", 0);
		return (S_OK);
	} // end if (smaller than the smallest valid message)

	pGenericMsg = (PCTRLMSG_GENERIC) pvData;
	
	if (pGenericMsg->dwSize != dwDataSize)
	{
		DPL(0, "Incomplete or invalid message (expected size %i bytes differs from actual size %i bytes)!  Ignoring.",
			2, pGenericMsg->dwSize, dwDataSize);
		return (S_OK);
	} // end if (didn't get complete/valid message)

	switch(pGenericMsg->dwType)
	{
		case CTRLMSGID_QUERYMETAMASTER:
			if (dwDataSize < sizeof (CTRLMSG_QUERYMETAMASTER))
			{
				DPL(0, "Data too small to be valid QueryMetaMaster message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid querymetamaster message)

			return (this->HandleQueryMetaMasterMsg(ppvFromAddress, dwFromAddressSize,
												(PCTRLMSG_QUERYMETAMASTER) pGenericMsg));
		  break;

		case CTRLMSGID_REGISTERMASTER:
			if (dwDataSize < sizeof (CTRLMSG_REGISTERMASTER))
			{
				DPL(0, "Data too small to be valid RegisterMaster message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid registermaster message)

			return (this->HandleRegisterMasterMsg(ppvFromAddress, dwFromAddressSize,
											(PCTRLMSG_REGISTERMASTER) pGenericMsg));
		  break;

		case CTRLMSGID_MASTERUPDATE:
			if (dwDataSize < sizeof (CTRLMSG_MASTERUPDATE))
			{
				DPL(0, "Data too small to be valid MasterUpdate message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid masterupdate message)

			return (this->HandleMasterUpdateMsg(ppvFromAddress, dwFromAddressSize,
												(PCTRLMSG_MASTERUPDATE) pGenericMsg));
		  break;

		case CTRLMSGID_UNREGISTERMASTER:
			if (dwDataSize < sizeof (CTRLMSG_UNREGISTERMASTER))
			{
				DPL(0, "Data too small to be valid UnregisterMaster message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid unregistermaster message)

			return (this->HandleUnregisterMasterMsg(ppvFromAddress, dwFromAddressSize,
												(PCTRLMSG_UNREGISTERMASTER) pGenericMsg));
		  break;

		default:
			DPL(0, "Message of invalid/unknown type (%i)!  Ignoring.", 1, pGenericMsg->dwType);
		  break;
	} // end switch (on message type)

	return (S_OK);
} // CTNMetaMaster::HandleMessage
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMetaMaster::HandleQueryMetaMasterMsg()"
//==================================================================================
// CTNMetaMaster::HandleQueryMetaMasterMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session query meta master message.
//
// Arguments:
//	PVOID* ppvFromAddress							Pointer to pointer to data
//													describing ctrl comm from
//													address.
//	DWORD dwFromAddressSize							Size of from address data.
//	PCTRLMSG_QUERYMETAMASTER pQueryMetaMasterMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMetaMaster::HandleQueryMetaMasterMsg(PVOID* ppvFromAddress,
												DWORD dwFromAddressSize,
												PCTRLMSG_QUERYMETAMASTER pQueryMetaMasterMsg)
{
	HRESULT						hr = S_OK;
	DWORD						dwStatus = CTRLMETAMASTERREPLY_NOMASTERSAVAILABLE;
	int							i;
	PTNMASTERINFO				pMaster = NULL;
	PTNSENDDATA					pSendData = NULL;
	PCTRLMSG_METAMASTERREPLY	pReplyMsg = NULL;


	DPL(0, "Got QueryMetaMaster message.", 0);


	// Compare meta-master API versions
	if (pQueryMetaMasterMsg->dwMetaVersion != CURRENT_METAMASTER_API_VERSION)
	{
		DPL(0, "Meta-master control version (%i) is different from ours (%i), ignoring.",
			2, pQueryMetaMasterMsg->dwMetaVersion, CURRENT_METAMASTER_API_VERSION);
		goto DONE;
	} // end if (version isn't the same)


	// Search through our list of masters for one fitting the description this slave
	// is looking for.

	this->m_masters.EnterCritSection();
	for(i = 0; i < this->m_masters.Count(); i++)
	{
		pMaster = (PTNMASTERINFO) this->m_masters.GetItem(i);
		if (pMaster == NULL)
		{
			DPL(0, "Couldn't get master %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		// If the master isn't allowing joiners, don't even bother.
		if (! pMaster->m_fJoinersAllowed)
			continue;

		// Compare app IDs
		if ((strcmp(pMaster->m_moduleID.szBriefName, pQueryMetaMasterMsg->moduleID.szBriefName) != 0) ||
			(pMaster->m_moduleID.dwMajorVersion != pQueryMetaMasterMsg->moduleID.dwMajorVersion) ||
			(pMaster->m_moduleID.dwMinorVersion1 != pQueryMetaMasterMsg->moduleID.dwMinorVersion1))
		{
			// Skip it if it didn't match
			continue;
		} // end if (failed to match the requested module id)


		// Compare user specified session IDs
		if (pMaster->m_pszSessionFilter == NULL)
		{
			if (strcmp((char*) (pQueryMetaMasterMsg + 1), "") != 0)
			{
				// Skip it if it didn't match
				continue;
			} // end if (the enum request is for a specific session)
		} // end if (we are not a specific session)
		else
		{
			if (strcmp(pMaster->m_pszSessionFilter, (char*) (pQueryMetaMasterMsg + 1)) != 0)
			{
				// Skip it if it didn't match
				continue;
			} // end if (the query is for a different/no session)
		} // end else (we are a specific session)

		// Make sure it's not overloaded with slaves already
		if (pMaster->m_iNumSlaves >= 5)
		{
			// Skip it.
			continue;
		} // end if (master has at least 5 slaves already)

		// If we got here, we found a master that matches the requirements.
		dwStatus = CTRLMETAMASTERREPLY_FOUNDMASTER;


		DPL(0, "Having slave try joining master with address \"%s\".",
			1, pMaster->m_pszAddress);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Having slave try joining master with address \"%s\".",
				1, pMaster->m_pszAddress);
		
		break;
	} // end for each master we know about)
	this->m_masters.LeaveCritSection();


	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = dwFromAddressSize;
	pSendData->m_pvAddress = (*ppvFromAddress);

	(*ppvFromAddress) = NULL; // forget about it so it doesn't get freed

	pSendData->m_dwDataSize = sizeof (CTRLMSG_METAMASTERREPLY);
	if (dwStatus == CTRLMETAMASTERREPLY_FOUNDMASTER)
		pSendData->m_dwDataSize += strlen(pMaster->m_pszAddress) + 1;

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pReplyMsg = (PCTRLMSG_METAMASTERREPLY) pSendData->m_pvData;
	pReplyMsg->dwSize = pSendData->m_dwDataSize;
	pReplyMsg->dwType = CTRLMSGID_METAMASTERREPLY;
	pReplyMsg->dwStatus = dwStatus;
	if (dwStatus == CTRLMETAMASTERREPLY_FOUNDMASTER)
	{
		strcpy((char*) (pReplyMsg + 1), pMaster->m_pszAddress);
	} // end if (found master)

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (have leftover object)

	return (hr);
} // CTNMetaMaster::HandleQueryMetaMasterMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMetaMaster::HandleRegisterMasterMsg()"
//==================================================================================
// CTNMetaMaster::HandleRegisterMasterMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session register master message.
//
// Arguments:
//	PVOID* ppvFromAddress						Pointer to pointer to data
//												describing ctrl comm from
//												address.
//	DWORD dwFromAddressSize						Size of from address data.
//	PCTRLMSG_REGISTERMASTER pRegisterMasterMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMetaMaster::HandleRegisterMasterMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
												PCTRLMSG_REGISTERMASTER pRegisterMasterMsg)
{
	HRESULT							hr = S_OK;
	PTNMASTERINFO					pNewMachine = NULL;
	PTNSENDDATA						pSendData = NULL;
	PCTRLMSG_REGISTERMASTERREPLY	pReplyMsg = NULL;


	DPL(0, "Got RegisterMaster message.", 0);

	// Compare meta-master API versions
	if (pRegisterMasterMsg->dwMetaVersion != CURRENT_METAMASTER_API_VERSION)
	{
		DPL(0, "Meta-master control version (%i) is different from ours (%i), ignoring.",
			2, pRegisterMasterMsg->dwMetaVersion, CURRENT_METAMASTER_API_VERSION);
		goto DONE;
	} // end if (version isn't the same)

	pNewMachine = new (CTNMasterInfo);
	if (pNewMachine == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	pNewMachine->m_dwControlVersion = pRegisterMasterMsg->dwControlVersion;
	pNewMachine->m_dwSessionID = pRegisterMasterMsg->dwSessionID;
	CopyMemory(&(pNewMachine->m_moduleID), &(pRegisterMasterMsg->moduleID), sizeof (TNMODULEID));
	pNewMachine->m_dwMode = pRegisterMasterMsg->dwMode;

	pNewMachine->m_pszAddress = (char*) LocalAlloc(LPTR, (pRegisterMasterMsg->dwAddressSize + 1));
	if (pNewMachine->m_pszAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	CopyMemory(pNewMachine->m_pszAddress, (pRegisterMasterMsg + 1),
			pRegisterMasterMsg->dwAddressSize);
	pNewMachine->m_pszAddress[pRegisterMasterMsg->dwAddressSize] = '\0'; // NULL termination

	if (pRegisterMasterMsg->dwSessionFilterSize > 0)
	{
		pNewMachine->m_pszSessionFilter = (char*) LocalAlloc(LPTR, (pRegisterMasterMsg->dwSessionFilterSize + 1));
		if (pNewMachine->m_pszSessionFilter == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
		CopyMemory(pNewMachine->m_pszSessionFilter,
				((LPBYTE) (pRegisterMasterMsg + 1)) + pRegisterMasterMsg->dwAddressSize,
				pRegisterMasterMsg->dwSessionFilterSize);
	} // end if (there's a session string)

	// Assign it an ID
	pNewMachine->m_id.dwTime = GetTickCount();

	//pNewMachine->commdata.hStatusEvent = this->m_hAllSlavesCommStatusEvent;
	pNewMachine->m_commdata.fDropped = FALSE;
	pNewMachine->m_commdata.pvAddress = (*ppvFromAddress);
	pNewMachine->m_commdata.dwAddressSize = dwFromAddressSize;

	hr = this->m_pCtrlComm->BindDataToAddress(&(pNewMachine->m_commdata));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't bind new machine's control comm data!", 0);
		goto DONE;
	} // end if (couldn't bind ctrl comm address)

	DPL(0, "Adding master at address \"%s\", session ID = %u.",
		2, pNewMachine->m_pszAddress, pNewMachine->m_dwSessionID);

	this->Log(TNLST_CONTROLLAYER_INFO,
			"Adding master at address \"%s\", session ID = %u.",
			2, pNewMachine->m_pszAddress, pNewMachine->m_dwSessionID);

	// Add it to the list
	hr = this->m_masters.Add(pNewMachine);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add new machine to our list!", 0);
		goto DONE;
	} // end if (couldn't add machine to list)

	// Send a reply
	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		pNewMachine = NULL; // forget about it, the list owns it now
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = dwFromAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, dwFromAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		pNewMachine = NULL; // forget about it, the list owns it now
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	CopyMemory(pSendData->m_pvAddress, (*ppvFromAddress), dwFromAddressSize);

	(*ppvFromAddress) = NULL; // forget about it so it doesn't get freed

	pSendData->m_dwDataSize = sizeof (CTRLMSG_REGISTERMASTERREPLY);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		pNewMachine = NULL; // forget about it, the list owns it now
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pReplyMsg = (PCTRLMSG_REGISTERMASTERREPLY) pSendData->m_pvData;
	pReplyMsg->dwSize = pSendData->m_dwDataSize;
	pReplyMsg->dwType = CTRLMSGID_REGISTERMASTERREPLY;

	CopyMemory(&(pReplyMsg->id), &(pNewMachine->m_id), sizeof (TNCTRLMACHINEID));

	pNewMachine = NULL; // forget about it so we don't free it below
	
	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pNewMachine != NULL)
		delete (pNewMachine);

	if (pSendData != NULL)
		delete (pSendData);

	return (hr);
} // CTNMetaMaster::HandleRegisterMasterMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMetaMaster::HandleMasterUpdateMsg()"
//==================================================================================
// CTNMetaMaster::HandleMasterUpdateMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session master update reply message.
//
// Arguments:
//	PVOID* ppvFromAddress							Pointer to pointer to data
//													describing ctrl comm from
//													address.
//	DWORD dwFromAddressSize							Size of from address data.
//	PCTRLMSG_MASTERUPDATEREPLY pMasterUpdateMsg		The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMetaMaster::HandleMasterUpdateMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_MASTERUPDATE pMasterUpdateMsg)
{
	PTNMASTERINFO	pMaster = NULL;


	DPL(0, "Got MasterUpdate message.", 0);

	pMaster = this->m_masters.GetMasterByID(&(pMasterUpdateMsg->id));
	if (pMaster == NULL)
	{
		DPL(0, "Couldn't find updating master (%i) in list!  Ignoring.",
			1, pMasterUpdateMsg->id.dwTime);

		// BUGBUG Fail?
		return (S_OK);
	} // end if (couldn't get slaveinfo object)

	pMaster->m_iNumSlaves = pMasterUpdateMsg->iNumSlaves;
	pMaster->m_fJoinersAllowed = pMasterUpdateMsg->fJoinersAllowed;

	DPL(0, "Master \"%s\" now has %i slaves, and is%saccepting joiners.",
		3, pMaster->m_pszAddress, pMaster->m_iNumSlaves,
		(pMasterUpdateMsg->fJoinersAllowed ? " " : " not "));

	return (S_OK);
} // CTNMetaMaster::HandleMasterUpdateMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMetaMaster::HandleUnregisterMasterMsg()"
//==================================================================================
// CTNMetaMaster::HandleUnregisterMasterMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session unregister master message.
//
// Arguments:
//	PVOID* ppvFromAddress								Pointer to pointer to data
//														describing ctrl comm from
//														address.
//	DWORD dwFromAddressSize								Size of from address data.
//	LPCTRLMSG_UNREGISTERMASTER lpUnregisterMasterMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMetaMaster::HandleUnregisterMasterMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
												PCTRLMSG_UNREGISTERMASTER pUnregisterMasterMsg)
{
	HRESULT			hr;
	PTNMASTERINFO	pMaster = NULL;


	DPL(0, "Got UnregisterMaster message.", 0);

	pMaster = this->m_masters.GetMasterByID(&(pUnregisterMasterMsg->id));
	if (pMaster == NULL)
	{
		DPL(0, "Couldn't find unregistering master (%i) in list!  Ignoring.",
			1, pUnregisterMasterMsg->id.dwTime);

		// BUGBUG Fail?
		return (S_OK);
	} // end if (couldn't get slaveinfo object)

	hr = this->m_masters.RemoveFirstReference(pMaster);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove just found master %x!", 1, pMaster);
		return (hr);
	} // end if (couldn't remove master)


	DPL(0, "Removing master at address \"%s\".", 1, pMaster->m_pszAddress);

	this->Log(TNLST_CONTROLLAYER_INFO, "Removing master at address \"%s\".",
			1, pMaster->m_pszAddress);

	if (pMaster->m_dwRefCount != 0)
	{
		DPL(0, "WARNING: Master %x still has %i references!",
			2, pMaster, pMaster->m_dwRefCount);
	} // end if (that wasn't the last use of the object)
	else
	{
		hr = this->m_pCtrlComm->UnbindDataFromAddress(&(pMaster->m_commdata));
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unbind machine's control comm data!", 0);
			return (hr);
		} // end if (couldn't unbind ctrl comm address)

		delete (pMaster);
		pMaster = NULL;
	} // end else (that was the last use of the object)

	return (S_OK);
} // CTNMetaMaster::HandleUnregisterMasterMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#endif // ! XBOX
