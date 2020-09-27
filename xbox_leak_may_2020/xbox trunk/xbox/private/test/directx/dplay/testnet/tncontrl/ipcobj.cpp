#ifndef _XBOX // no IPC supported
//==================================================================================
// Includes
//==================================================================================
#include <windows.h>


#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\sprintf.h"

#include "tncontrl.h"
#include "main.h"

#include "ipcobj.h"




//==================================================================================
// Defines
//==================================================================================
#define PREFIX_MMMUTEX					"IPCObj MemoryMappingMutex"
#define PREFIX_MMFILE					"IPCObj MemoryMappedFile"
#define PREFIX_MSGFOROWNEREVENT			"IPCObj MsgForOwnerEvent"
#define PREFIX_MSGFROMOWNEREVENT		"IPCObj MsgFromOwnerEvent"
#define PREFIX_MSGACKEVENT				"IPCObj MsgAckEvent"
#define PREFIX_REDIR_MUTEX				"IPCObj RedirMutex"
#define PREFIX_REDIR_MMMUTEX			"IPCObj RedirMemoryMappingMutex"
#define PREFIX_REDIR_MMFILE				"IPCObj RedirMemoryMappedFile"


#define MAX_REDIR_DATA_SIZE				(1024)






//==================================================================================
// Structures
//==================================================================================
typedef struct tagIPCOBJMMFILEHEADER
{
	DWORD		dwRefCount; // number of users using this memory mapped file
	DWORD		dwAPIVersion; // version of control layer this was created with
	DWORD		dwOwnerProcessID; // the owning process's process ID
	DWORD		dwSeparateProcessID; // the separate process's process ID
	DWORD		dwCommand; // command ID for message in shared memory space
	DWORD		dwSendDataSize; // size of data for message in shared memory space
	DWORD		dwReplyDataSize; // size of data for message in shared memory space
} IPCOBJMMFILEHEADER, * PIPCOBJMMFILEHEADER;

typedef struct tagIPCOBJMMREDIRECTFILEHEADER
{
	DWORD		dwSize; // size of real attach point ID, not including NULL termination
} IPCOBJMMREDIRECTFILEHEADER, * PIPCOBJMMREDIRECTFILEHEADER;




//==================================================================================
// Messages
//==================================================================================
#define IPCMSGID_CONNECT		1
/*
typedef struct tagIPCMSG_CONNECT
{
	DWORD	?; // ?
} IPCMSG_CONNECT, * PIPCMSG_CONNECT;
*/

#define IPCMSGID_DISCONNECT		2
/*
typedef struct tagIPCMSG_DISCONNECT
{
	DWORD	?; // ?
} IPCMSG_DISCONNECT, * PIPCMSG_DISCONNECT;
*/

#define IPCMSGID_PING			3
/*
typedef struct tagIPCMSG_PING
{
	DWORD	?; // ?
} IPCMSG_PING, * PIPCMSG_PING;
*/





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::CTNIPCObject()"
//==================================================================================
// CTNIPCObject constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNIPCObject object.  Initializes the data structures.
//
// Arguments:
//	PTNEXECUTORPRIV pExecutor		Pointer to the owning executor for this object.
//
// Returns: None (just the object).
//==================================================================================
CTNIPCObject::CTNIPCObject(PTNEXECUTORPRIV pExecutor):
	m_pExecutor(pExecutor),
	m_fOwnerCopy(FALSE),
	m_fConnected(FALSE),
	m_dwMMWriteLockRefCount(0),
	m_hMMWriteMutex(NULL),
	m_hMMFile(NULL),
	m_hRedirMutex(NULL),
	m_hRedirMMFile(NULL),
	m_hSendEvent(NULL),
	m_hAckEvent(NULL),
	m_hConnectEvent(NULL),
	m_hObjMsgRecvThread(NULL),
	m_hKillObjMsgRecvThreadEvent(NULL),
	m_pszAttachPointID(NULL),
	m_hReceiveEvent(NULL),
	m_dwLastMessageReceived(0)
{
	DPL(9, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNIPCObject));
} // CTNIPCObject::CTNIPCObject
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::~CTNIPCObject()"
//==================================================================================
// CTNIPCObject destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNIPCObject object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNIPCObject::~CTNIPCObject(void)
{
	HRESULT		hr;


	DPL(9, "==> this = %x", 1, this);

	if (this->m_fConnected)
	{
		hr = this->Disconnect();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't disconnect!  %e", 1, hr);
		} // end if (couldn't disconnect)
	} // end if (we're connected)


	// We must wait for the thread to die before removing anything else because
	// it may still be processing a message.
	hr = this->KillObjMsgRecvThread();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't kill message reception thread!  %e", 1, hr);
	} // end if (couldn't kill message reception thread)

	if (this->m_hMMFile)
	{
		CloseHandle(this->m_hMMFile);
		this->m_hMMFile = NULL;
	} // end if (we have the file)

	while (this->m_dwMMWriteLockRefCount > 0)
	{
		ReleaseMutex(this->m_hMMWriteMutex);
		this->m_dwMMWriteLockRefCount--;
		//DPL(9, "Dropped lock.", 0);
	} // end if (we have the lock)

	if (this->m_hMMWriteMutex)
	{
		CloseHandle(this->m_hMMWriteMutex);
		this->m_hMMWriteMutex = NULL;
	} // end if (we have the mutex)

	if (this->m_hRedirMMFile)
	{
		CloseHandle(this->m_hRedirMMFile);
		this->m_hRedirMMFile = NULL;
	} // end if (we have the file)

	// If we have this, we also therefore own it, so release it and close it.
	if (this->m_hRedirMutex)
	{
		ReleaseMutex(this->m_hRedirMutex);
		CloseHandle(this->m_hRedirMutex);
		this->m_hRedirMutex = NULL;
	} // end if (we have the lock)

	if (this->m_hSendEvent)
	{
		CloseHandle(this->m_hSendEvent);
		this->m_hSendEvent = NULL;
	} // end if (we have the event)

	if (this->m_hAckEvent)
	{
		CloseHandle(this->m_hAckEvent);
		this->m_hAckEvent = NULL;
	} // end if (we have the event)

	if (this->m_hConnectEvent)
	{
		CloseHandle(this->m_hConnectEvent);
		this->m_hConnectEvent = NULL;
	} // end if (we have the event)

	if (this->m_pszAttachPointID != NULL)
	{
		LocalFree(this->m_pszAttachPointID);
		this->m_pszAttachPointID = NULL;
	} // end if (allocated an attach point ID)

	if (this->m_hReceiveEvent)
	{
		CloseHandle(this->m_hReceiveEvent);
		this->m_hReceiveEvent = NULL;
	} // end if (we have the event)

	if (this->m_pvConnectData != NULL)
	{
		LocalFree(this->m_pvConnectData);
		this->m_pvConnectData = NULL;
	} // end if (there's connect data)

	// We don't touch this->m_pExecutor, because our owner or child class will
	// take care of it properly.

	DPL(9, "<== this = %x", 1, this);
} // CTNIPCObject::~CTNIPCObject
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::Connect()"
//==================================================================================
// CTNIPCObject::Connect
//----------------------------------------------------------------------------------
//
// Description: Connects this object using the given module and attach point IDs.
//
// Arguments:
//	BOOL fOwnerCopy					Whether this is the owner version (FALSE to be
//									process side).
//	PTNMODULEID pTNModuleID			ID of module using this object.
//	char* szAttachPointID			Identifier string shared with opposite side,
//									used to prevent collisions.
//	HANDLE hConnectEvent			Optional handle to event to set when the other
//									side connects back (may be right away, if it is
//									already set up).
//	PVOID pvSendConnectData			Optional pointer to data to send to other side.
//	DWORD dwSendConnectDataSize		Size of data to send to other side.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::Connect(BOOL fOwnerCopy, PTNMODULEID pTNModuleID,
							char* szAttachPointID, HANDLE hConnectEvent,
							PVOID pvSendConnectData, DWORD dwSendConnectDataSize)
{
	HRESULT					hr;
	char*					pszBuffer = NULL;
	PIPCOBJMMFILEHEADER		pHeader = NULL;
	HANDLE					ahWaitObjects[3];
	DWORD					dwNumWaitObjects = 0;


	if (this == NULL)
	{
		DPL(0, "This object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (bad object pointer)

	if (this->m_pszAttachPointID != NULL)
	{
		DPL(0, "Object already attached to an owner (using \"%s\")!",
			1, this->m_pszAttachPointID);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (already attached to an owner)

	if (pTNModuleID == NULL)
	{
		DPL(0, "Module ID pointer cannot be NULL!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (bad module ID pointer)

	if ((szAttachPointID == NULL) || (strcmp(szAttachPointID, "") == 0))
	{
		DPL(0, "Attach point ID cannot be NULL or just an empty string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (bad attach point ID)

	this->m_fOwnerCopy = fOwnerCopy;

	// Save the attachpoint ID
	this->m_pszAttachPointID = (char*) LocalAlloc(LPTR, strlen(szAttachPointID) + 1);
	if (this->m_pszAttachPointID == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate attach point ID)
	strcpy(this->m_pszAttachPointID, szAttachPointID);


	if (fOwnerCopy)
	{
		DPL(1, "Connecting to separate process with ID \"%s\", sending %u bytes of connect data at %x.",
			3, this->m_pszAttachPointID, dwSendConnectDataSize,
			pvSendConnectData);
	} // end if (owner side)
	else
	{
		DPL(1, "Connecting to owner with ID \"%s\", sending %u bytes of connect data at %x.",
			3, this->m_pszAttachPointID, dwSendConnectDataSize,
			pvSendConnectData);
	} // end else (process side)

	if (hConnectEvent != NULL)
	{
		if (! DuplicateHandle(GetCurrentProcess(), hConnectEvent,
								GetCurrentProcess(), &(this->m_hConnectEvent),
								0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			hr = GetLastError();
			DPL(0, "Couldn't duplicate connection event handle (%x)!",
				2, hConnectEvent);
			goto ERROR_EXIT;
		} // end if (couldn't duplicate handle)
	} // end if (have connect event)
	

	// Create/open the mutex used to access the shared memory.

	TNsprintf(&pszBuffer, PREFIX_MMMUTEX " (mod %s v%u.%u) %s",
			4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
			pTNModuleID->dwMinorVersion1, this->m_pszAttachPointID);

	// We can't be the initial owner of the mutex because it may have been created
	// already, in which case the TRUE would be ignored.
	this->m_hMMWriteMutex = CreateMutex(NULL, FALSE, pszBuffer);
	if (this->m_hMMWriteMutex == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create mutex %s!", 1, pszBuffer);
		goto ERROR_EXIT;
	} // end if (couldn't create mutex)
	TNsprintf_free(&pszBuffer);


	// Create/open the actual shared memory.

	TNsprintf(&pszBuffer, PREFIX_MMFILE " (mod %s v%u.%u) %s",
			4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
			pTNModuleID->dwMinorVersion1, this->m_pszAttachPointID);

	DPL(8, "Using file mapping \"%s\" to swap memory.", 1, pszBuffer);

	this->m_hMMFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
										0, (sizeof (IPCOBJMMFILEHEADER) + MAX_MESSAGE_SIZE),
										pszBuffer);
	if (this->m_hMMFile == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create file mapping %s!", 1, pszBuffer);
		goto ERROR_EXIT;
	} // end if (couldn't create file mapping)
	TNsprintf_free(&pszBuffer);


	if (fOwnerCopy)
	{
		TNsprintf(&pszBuffer, PREFIX_MSGFROMOWNEREVENT " (mod %s v%u.%u) %s",
				4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
				pTNModuleID->dwMinorVersion1, this->m_pszAttachPointID);

		DPL(9, "Send event = \"%s\"", 1, pszBuffer);

		this->m_hSendEvent = CreateEvent(NULL, FALSE, FALSE, pszBuffer);
		if (this->m_hSendEvent == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create send event %s!", 1, pszBuffer);
			goto ERROR_EXIT;
		} // end if (couldn't create event)
		TNsprintf_free(&pszBuffer);

		TNsprintf(&pszBuffer, PREFIX_MSGFOROWNEREVENT " (mod %s v%u.%u) %s",
				4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
				pTNModuleID->dwMinorVersion1, this->m_pszAttachPointID);

		DPL(9, "Receive event = \"%s\"", 1, pszBuffer);

		this->m_hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, pszBuffer);
		if (this->m_hReceiveEvent == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create receive event %s!", 1, pszBuffer);
			goto ERROR_EXIT;
		} // end if (couldn't create event)
		TNsprintf_free(&pszBuffer);
	} // end if (owner side)
	else
	{
		TNsprintf(&pszBuffer, PREFIX_MSGFOROWNEREVENT " (mod %s v%u.%u) %s",
				4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
				pTNModuleID->dwMinorVersion1, this->m_pszAttachPointID);

		DPL(9, "Send event = \"%s\"", 1, pszBuffer);

		this->m_hSendEvent = CreateEvent(NULL, FALSE, FALSE, pszBuffer);
		if (this->m_hSendEvent == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create send event %s!", 1, pszBuffer);
			goto ERROR_EXIT;
		} // end if (couldn't create event)
		TNsprintf_free(&pszBuffer);

		TNsprintf(&pszBuffer, PREFIX_MSGFROMOWNEREVENT " (mod %s v%u.%u) %s",
				4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
				pTNModuleID->dwMinorVersion1, this->m_pszAttachPointID);

		DPL(9, "Receive event = \"%s\"", 1, pszBuffer);

		this->m_hReceiveEvent = CreateEvent(NULL, FALSE, FALSE, pszBuffer);
		if (this->m_hReceiveEvent == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create receive event %s!", 1, pszBuffer);
			goto ERROR_EXIT;
		} // end if (couldn't create event)
		TNsprintf_free(&pszBuffer);
	} // end else (not owner side)

	TNsprintf(&pszBuffer, PREFIX_MSGACKEVENT " (mod %s v%u.%u) %s",
			4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
			pTNModuleID->dwMinorVersion1, this->m_pszAttachPointID);

	DPL(9, "Ack event = \"%s\"", 1, pszBuffer);

	this->m_hAckEvent = CreateEvent(NULL, FALSE, FALSE, pszBuffer);
	if (this->m_hAckEvent == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create ack event %s!", 1, pszBuffer);
		goto ERROR_EXIT;
	} // end if (couldn't create event)
	TNsprintf_free(&pszBuffer);


	// If we're the process side, we need to start up a thread to receive messages
	// from the owner.  The owner side will have an overall thread for this already.
	if (! fOwnerCopy)
	{
		hr = this->StartObjMsgRecvThread();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't start message reception thread!", 0);
			goto ERROR_EXIT;
		} // end if (couldn't start message reception thread)
	} // end if (not owned by the owner)


	// Grab the mutex.
	ahWaitObjects[dwNumWaitObjects++] = this->m_hMMWriteMutex;

	if (this->m_pExecutor->m_hUserCancelEvent != NULL)
	{
		ahWaitObjects[dwNumWaitObjects++] = this->m_pExecutor->m_hUserCancelEvent;
	} // end if (owner side and have user cancel event)

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array


REWAIT:

	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
								INFINITE, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Got the ready signal, we're cool.

			hr = S_OK;
		  break;

		case WAIT_OBJECT_0 + 1:
			// Got user cancel event

			DPL(0, "User cancelled waiting for mutex to connect!", 0);

			hr = TNWR_USERCANCEL;
			goto ERROR_EXIT;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		default:
			// What?
			DPL(0, "Received unexpected return from WaitForMultipleObjectsEx!", 0);

			if (hr == S_OK)
				hr = E_FAIL;

			goto ERROR_EXIT;
		  break;
	} // end switch (on wait result)


	this->m_dwMMWriteLockRefCount++;
	//DPL(9, "Took lock.", 0);


	pHeader = (PIPCOBJMMFILEHEADER) MapViewOfFile(this->m_hMMFile,
													FILE_MAP_ALL_ACCESS,
													0, 0, 0);
	if (pHeader == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't map view of file!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't map view of file)


	// If we're the first users, initialize it with our settings.  Otherwise, make
	// sure we can use what's there.
	switch (pHeader->dwRefCount)
	{
		case 0:
			// We're the first user.
			pHeader->dwRefCount++;

			DPL(3, "Other side has not connected yet.", 0);

			pHeader->dwAPIVersion = CURRENT_TNCONTROL_API_VERSION;

			// Put the connect data there so when the other side does connect it
			// can retrieve it.
			pHeader->dwCommand = IPCMSGID_CONNECT;
			if (dwSendConnectDataSize > 0)
			{
				CopyMemory((pHeader + 1), pvSendConnectData, dwSendConnectDataSize);
				pHeader->dwSendDataSize = dwSendConnectDataSize;
			} // end if (there's connect data to store)

			if (fOwnerCopy)
				pHeader->dwOwnerProcessID = GetCurrentProcessId();
			else
				pHeader->dwSeparateProcessID = GetCurrentProcessId();
		  break;

		case 1:
			// We're the second user.
			pHeader->dwRefCount++;

			if (pHeader->dwAPIVersion != CURRENT_TNCONTROL_API_VERSION)
			{
				DPL(0, "WARNING: Other side already using shared memory but it's a different API version (%i != %i), problems may occur.",
					2, pHeader->dwAPIVersion, CURRENT_TNCONTROL_API_VERSION);
			} // end if (versions don't match)

			if (fOwnerCopy)
			{
				if (pHeader->dwOwnerProcessID != 0)
				{
					DPL(0, "Shared memory is already in use by another owning process (%x)!  DEBUGBREAK()-ing.",
						1, pHeader->dwOwnerProcessID);
					DEBUGBREAK();
					pHeader->dwRefCount--; // take off the ref we added
					hr = ERROR_BAD_ENVIRONMENT;
					goto ERROR_EXIT;
				} // end if (the current user is a new process)

				if (pHeader->dwSeparateProcessID == 0)
				{
					DPL(0, "Separate process ID is 0!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					pHeader->dwRefCount--; // take off the ref we added
					hr = ERROR_BAD_ENVIRONMENT;
					goto ERROR_EXIT;
				} // end if (ID not stored)

				DPL(3, "Separate process (process ID = %x) has already established connection.",
					1, pHeader->dwSeparateProcessID);
				
				if (pHeader->dwSeparateProcessID == GetCurrentProcessId())
				{
					DPL(0, "WARNING: Process IDs are the same for both sides of the connection (%x), is this what you intended?",
						1, pHeader->dwSeparateProcessID);
				} // end if (same process)

				pHeader->dwOwnerProcessID = GetCurrentProcessId();
			} // end if (we are owner side)
			else
			{
				if (pHeader->dwSeparateProcessID != 0)
				{
					DPL(0, "Shared memory is already in use by another separate process (%x)!  DEBUGBREAK()-ing.",
						1, pHeader->dwSeparateProcessID);
					DEBUGBREAK();
					pHeader->dwRefCount--; // take off the ref we added
					hr = ERROR_BAD_ENVIRONMENT;
					goto ERROR_EXIT;
				} // end if (the current user is a new process)

				if (pHeader->dwOwnerProcessID == 0)
				{
					DPL(0, "Owner process ID is 0!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					pHeader->dwRefCount--; // take off the ref we added
					hr = ERROR_BAD_ENVIRONMENT;
					goto ERROR_EXIT;
				} // end if (ID not stored)

				DPL(3, "Owner (process ID = %x) has already established connection.",
					1, pHeader->dwOwnerProcessID);
				
				if (pHeader->dwOwnerProcessID == GetCurrentProcessId())
				{
					DPL(0, "WARNING: Process IDs are the same for both sides of the connection (%x), is this what you intended?",
						1, pHeader->dwOwnerProcessID);
				} // end if (same process)

				pHeader->dwSeparateProcessID = GetCurrentProcessId();
			} // end else (we're not owner side)



			// Retrieve any connect data there may have been
			if (pHeader->dwSendDataSize > 0)
			{
				this->m_pvConnectData = LocalAlloc(LPTR, pHeader->dwSendDataSize);
				if (this->m_pvConnectData == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto ERROR_EXIT;
				} // end if (couldn't allocate memory)

				CopyMemory(this->m_pvConnectData, (pHeader + 1),
							pHeader->dwSendDataSize);
				this->m_dwConnectDataSize = pHeader->dwSendDataSize;

				DPL(8, "Got %u byte connect data buffer at %x.",
					2, this->m_dwConnectDataSize, this->m_pvConnectData);
			} // end if (there's connect data)


			// Since the other end should already be there (his refcount is there),
			// we can call ourselves connected.
			this->m_fConnected = TRUE;


			hr = this->SendMessage(IPCMSGID_CONNECT, pvSendConnectData,
									dwSendConnectDataSize,
									IPCOBJ_ACK_WAIT_TIMEOUT);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't send connect message!", 0);
				goto ERROR_EXIT;
			} // end if (couldn't send connect message)



			if (this->m_hConnectEvent != NULL)
			{
				if (! SetEvent(this->m_hConnectEvent))
				{
					hr = GetLastError();
					DPL(0, "Couldn't set connection event (%x)!", 1, this->m_hConnectEvent);
					goto ERROR_EXIT;
				} // end if (couldn't set event)

				// We don't need the event anymore
				CloseHandle(this->m_hConnectEvent);
				this->m_hConnectEvent = NULL;
			} // end if (there's a connect event)
		  break;

		default:
			DPL(0, "Shared memory has an invalid number of users (%u)!",
				1, pHeader->dwRefCount);
			hr = ERROR_BAD_ENVIRONMENT;
			goto ERROR_EXIT;
		  break;
	} // end switch (on the current refcount)

	UnmapViewOfFile(pHeader);
	pHeader = NULL;

	ReleaseMutex(this->m_hMMWriteMutex);
	this->m_dwMMWriteLockRefCount--;
	//DPL(9, "Dropped lock.", 0);

	return (S_OK);


ERROR_EXIT:

	if (pHeader != NULL)
	{
		UnmapViewOfFile(pHeader);
		pHeader = NULL;
	} // end if (mapped view)

	if (this->m_hAckEvent != NULL)
	{
		CloseHandle(this->m_hAckEvent);
		this->m_hAckEvent = NULL;
	} // end if (have event)

	if (this->m_hReceiveEvent != NULL)
	{
		CloseHandle(this->m_hReceiveEvent);
		this->m_hReceiveEvent = NULL;
	} // end if (have event)

	if (this->m_hSendEvent != NULL)
	{
		CloseHandle(this->m_hSendEvent);
		this->m_hSendEvent = NULL;
	} // end if (have event)

	if (this->m_hMMFile)
	{
		CloseHandle(this->m_hMMFile);
		this->m_hMMFile = NULL;
	} // end if (we have the lock)

	if (this->m_dwMMWriteLockRefCount > 0)
	{
		ReleaseMutex(this->m_hMMWriteMutex);
		this->m_dwMMWriteLockRefCount--;
		//DPL(9, "Dropped lock.", 0);
	} // end if (we have the lock)

	this->KillObjMsgRecvThread(); // ignore error

	if (this->m_hMMWriteMutex)
	{
		CloseHandle(this->m_hMMWriteMutex);
		this->m_hMMWriteMutex = NULL;
	} // end if (we have the lock)

	if (this->m_hConnectEvent != NULL)
	{
		CloseHandle(this->m_hConnectEvent);
		this->m_hConnectEvent = NULL;
	} // end if (have event)

	if (this->m_pszAttachPointID != NULL)
	{
		LocalFree(this->m_pszAttachPointID);
		this->m_pszAttachPointID = NULL;
	} // end if (allocated an attach point ID)

	if (pszBuffer != NULL)
		TNsprintf_free(&pszBuffer);

	return (hr);
} // CTNIPCObject::Connect
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::Disconnect()"
//==================================================================================
// CTNIPCObject::Disconnect
//----------------------------------------------------------------------------------
//
// Description: Disconnects this object from the other side.  If this is the owner's
//				copy of the object, then the object is notified that he was
//				detached.  If this is the process' copy, then the owner is notified
//				that the object detached.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::Disconnect(void)
{
	HRESULT					hr = S_OK;
	PIPCOBJMMFILEHEADER		pHeader = NULL;
	BOOL					fHaveMMWriteLock = FALSE;



	DPL(9, "==> this = %x", 1, this);

	if (this == NULL)
	{
		DPL(0, "This object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (bad object pointer)

	if (this->m_pszAttachPointID == NULL)
	{
		DPL(0, "WARNING: Object not connected!", 0);
		goto DONE;
	} // end if (don't have attach point)


	DPL(1, "%s disconnecting object %s.",
		2, ((this->m_fOwnerCopy) ? "Host" : "Process"),
		this->m_pszAttachPointID);


	// Notify the other side
	hr = this->SendMessage(IPCMSGID_DISCONNECT, NULL, 0, IPCOBJ_ACK_WAIT_TIMEOUT);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send disconnect message!", 0);
		goto DONE;
	} // end if (couldn't send disconnect message)



	// Grab the mutex, ignoring anything other than IO completion.

	while (WaitForSingleObjectEx(this->m_hMMWriteMutex, INFINITE, TRUE) == WAIT_IO_COMPLETION)
	{
		DPL(1, "I/O Completion.", 0);
	} // end while (there's an I/O completion)

	this->m_dwMMWriteLockRefCount++;
	fHaveMMWriteLock = TRUE;
	//DPL(9, "Took lock.", 0);



	pHeader = (PIPCOBJMMFILEHEADER) MapViewOfFile(this->m_hMMFile,
													FILE_MAP_ALL_ACCESS,
													0, 0, 0);
	if (pHeader == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't map view of file!", 0);
		goto DONE;
	} // end if (couldn't map view of file)


	if (this->m_fOwnerCopy)
	{
		pHeader->dwOwnerProcessID = 0;
	} // end if (we are owner side)
	else
	{
		pHeader->dwSeparateProcessID = 0;
	} // end else (we're not owner side)

	pHeader->dwRefCount--;


	UnmapViewOfFile(pHeader);
	pHeader = NULL;

	ReleaseMutex(this->m_hMMWriteMutex);
	fHaveMMWriteLock = FALSE;
	this->m_dwMMWriteLockRefCount--;
	//DPL(9, "Dropped lock.", 0);


	// We must wait for the thread to die before removing anything else because
	// it may still be processing a message.
	hr = this->KillObjMsgRecvThread();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't kill message reception thread!", 0);
		goto DONE;
	} // end if (couldn't kill message reception thread)


	CloseHandle(this->m_hMMFile);
	this->m_hMMFile = NULL;

	CloseHandle(this->m_hMMWriteMutex);
	this->m_hMMWriteMutex = NULL;

	if (this->m_hConnectEvent != NULL)
	{
		CloseHandle(this->m_hConnectEvent);
		this->m_hConnectEvent = NULL;
	} // end if (have event)

	CloseHandle(this->m_hAckEvent);
	this->m_hAckEvent = NULL;

	CloseHandle(this->m_hReceiveEvent);
	this->m_hReceiveEvent = NULL;

	CloseHandle(this->m_hSendEvent);
	this->m_hSendEvent = NULL;

	LocalFree(this->m_pszAttachPointID);
	this->m_pszAttachPointID = NULL;

	this->m_dwLastMessageReceived = 0;


DONE:

	if (pHeader != NULL)
	{
		UnmapViewOfFile(pHeader);
		pHeader = NULL;
	} // end if (mapped view)

	if (fHaveMMWriteLock)
	{
		ReleaseMutex(this->m_hMMWriteMutex);
		this->m_dwMMWriteLockRefCount--;
		fHaveMMWriteLock = FALSE;
		//DPL(9, "Dropped lock.", 0);
	} // end if (we have the lock)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNIPCObject::Disconnect
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::AllowStaticIDConnection()"
//==================================================================================
// CTNIPCObject::AllowStaticIDConnection
//----------------------------------------------------------------------------------
//
// Description: Alters connection behavior so that processes can still connect even
//				if they don't know the specific attachment ID to use.  The owner and
//				process must therefore agree beforehand on an string (such as the
//				name of the app) they will use to recognize each other.  This ID is
//				used to retrieve the real attachment ID.
//				Since the ID is not dynamic, only one object can use it at any one
//				time.  Fortunately, you only need the preplanned ID until you can
//				retrieve the dynamic ID.  A mutex is used to suspend other threads
//				trying to using the same ID.
//				Note, this means you may block in this call until the previous
//				object connects.  If the process never connects after you've made
//				this call, the mutex will be owned until this object is destroyed
//				(blocking all other attempts to use the same preplanned ID until
//				that time).
//				This function should only be used when retrieving the dynamic ID is
//				not feasible (such as testing an app that can't read the command
//				line) since it requires extra work and may suspend other threads.
//
// Arguments:
//	PTNMODULEID pTNModuleID		ID of module using this object.
//	char* szPrePlannedID		String used to identify expected connection and
//								retrieve dynamic ID.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::AllowStaticIDConnection(PTNMODULEID pTNModuleID,
											  char* szPrePlannedID)
{
	HRESULT							hr;
	char*							pszBuffer = NULL;
	HANDLE							hRedirMMMutex = NULL;
	PIPCOBJMMREDIRECTFILEHEADER		pHeader = NULL;
	HANDLE							ahWaitObjects[3];
	DWORD							dwNumWaitObjects = 0;


	if (this == NULL)
	{
		DPL(0, "This object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (bad object pointer)

	if (this->m_pszAttachPointID == NULL)
	{
		DPL(0, "Object not initialized (need attach point ID already set up)!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (no attach point yet)

	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Only owners can allow static ID connections!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (not owner's copy of object)

	if (this->m_hRedirMutex != NULL)
	{
		DPL(0, "Object already allowing static ID connection!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (already in use)

	if (pTNModuleID == NULL)
	{
		DPL(0, "Module ID pointer cannot be NULL!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (bad module ID pointer)

	if ((szPrePlannedID == NULL) || (strcmp(szPrePlannedID, "") == 0))
	{
		DPL(0, "Static connection ID cannot be NULL or just an empty string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (bad attach point ID)


	
	DPL(1, "Allowing static ID connection with ID = \"%s\"",
		1, szPrePlannedID);

	// Create/open the mutex used to protect the redirection.

	TNsprintf(&pszBuffer, PREFIX_REDIR_MUTEX " (mod %s v%u.%u) %s",
			4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
			pTNModuleID->dwMinorVersion1, szPrePlannedID);

	// We can't be the initial owner of the mutex because it may have been created
	// already, in which case the TRUE would be ignored.
	this->m_hRedirMutex = CreateMutex(NULL, FALSE, pszBuffer);
	if (this->m_hRedirMutex == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create mutex %s!", 1, pszBuffer);
		goto ERROR_EXIT;
	} // end if (couldn't create mutex)
	TNsprintf_free(&pszBuffer);



	// Grab the redirection mutex.
	ahWaitObjects[dwNumWaitObjects++] = this->m_hRedirMutex;

	if (this->m_pExecutor->m_hUserCancelEvent != NULL)
		ahWaitObjects[dwNumWaitObjects++] = this->m_pExecutor->m_hUserCancelEvent;

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array


REWAIT:

	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
								INFINITE, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Got the ready signal, we're cool.

			hr = S_OK;
		  break;

		case WAIT_OBJECT_0 + 1:
			// Got user cancel event

			DPL(0, "User cancelled waiting for redirection mutex!", 0);

			hr = TNWR_USERCANCEL;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		default:
			// What?
			DPL(0, "Received unexpected return from WaitForMultipleObjectsEx!", 0);

			if (hr == S_OK)
				hr = E_FAIL;
		  break;
	} // end switch (on wait result)


	// Now we have the overall lock.  We'll keep it until the other side connects or
	// this object is destroyed.




	// Create/open the mutex used to protect read/write access to the redirection
	// shared memory.

	TNsprintf(&pszBuffer, PREFIX_REDIR_MMMUTEX " (mod %s v%u.%u) %s",
			4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
			pTNModuleID->dwMinorVersion1, szPrePlannedID);

	// We can't be the initial owner of the mutex because it may have been created
	// already, in which case the TRUE would be ignored.
	hRedirMMMutex = CreateMutex(NULL, FALSE, pszBuffer);
	if (hRedirMMMutex == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create mutex %s!", 1, pszBuffer);
		goto ERROR_EXIT;
	} // end if (couldn't create mutex)
	TNsprintf_free(&pszBuffer);


	// Grab the write lock, which is just to prevent the other side from reading it
	// before we've written it.

	while (WaitForSingleObjectEx(hRedirMMMutex, INFINITE, TRUE) == WAIT_IO_COMPLETION)
	{
		DPL(1, "I/O Completion.", 0);
	} // end while (there's an I/O completion)



	// Create/open the actual shared redirection memory.

	TNsprintf(&pszBuffer, PREFIX_REDIR_MMFILE " (mod %s v%u.%u) %s",
			4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
			pTNModuleID->dwMinorVersion1, szPrePlannedID);

	DPL(8, "Opening redirect file \"%s\" to store real attach point ID (\"%s\").",
		2, pszBuffer, this->m_pszAttachPointID);

	this->m_hRedirMMFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
										0, (sizeof (IPCOBJMMREDIRECTFILEHEADER) + MAX_REDIR_DATA_SIZE),
										pszBuffer);
	if (this->m_hRedirMMFile == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create file mapping %s!", 1, pszBuffer);
		goto ERROR_EXIT;
	} // end if (couldn't create file mapping)
	TNsprintf_free(&pszBuffer);


	pHeader = (PIPCOBJMMREDIRECTFILEHEADER) MapViewOfFile(this->m_hRedirMMFile,
															FILE_MAP_ALL_ACCESS,
															0, 0, 0);
	if (pHeader == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't map view of file!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't map view of file)


	pHeader->dwSize = strlen(this->m_pszAttachPointID);
	strcpy((char*) (pHeader + 1), this->m_pszAttachPointID);



	UnmapViewOfFile(pHeader);
	pHeader = NULL;

	ReleaseMutex(hRedirMMMutex);
	CloseHandle(hRedirMMMutex);
	hRedirMMMutex = NULL;

	// We keep everything else until we are told to abort or the other side
	// connects.

	return (S_OK);


ERROR_EXIT:

	if (pHeader != NULL)
	{
		UnmapViewOfFile(pHeader);
		pHeader = NULL;
	} // end if (mapped view)

	if (this->m_hRedirMMFile)
	{
		CloseHandle(this->m_hRedirMMFile);
		this->m_hRedirMMFile = NULL;
	} // end if (we have the lock)

	if (hRedirMMMutex)
	{
		ReleaseMutex(hRedirMMMutex);
		CloseHandle(hRedirMMMutex);
		hRedirMMMutex = NULL;
	} // end if (we have the lock)

	if (this->m_hRedirMutex)
	{
		ReleaseMutex(this->m_hRedirMutex);
		CloseHandle(this->m_hRedirMutex);
		this->m_hRedirMutex = NULL;
	} // end if (we have the lock)

	if (pszBuffer != NULL)
		TNsprintf_free(&pszBuffer);

	return (hr);
} // CTNIPCObject::AllowStaticIDConnection
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::ConnectWithStaticID()"
//==================================================================================
// CTNIPCObject::ConnectWithStaticID
//----------------------------------------------------------------------------------
//
// Description: Connects this object to an owner previously expecting a connection.
//				The object finds the real attach point ID using the given preplanned
//				ID.
//				Note that the owner must previously have called
//				AllowStaticIDConnection on its copy of the object, using the same
//				ID, for this to work.
//				See AllowStaticIDConnection for more details.
//
// Arguments:
//	PTNMODULEID pTNModuleID			ID of module using this object.
//	char* szPrePlannedID			String used to identify expected connection and
//									retrieve dynamic ID.
//	HANDLE hConnectEvent			Optional handle to event to set when the other
//									side connects back (may be right away, if it is
//									already set up).
//	PVOID pvSendConnectData			Optional pointer to data to send to other side.
//	DWORD dwSendConnectDataSize		Size of data to send to other side.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::ConnectWithStaticID(PTNMODULEID pTNModuleID,
										char* szPrePlannedID,
										HANDLE hConnectEvent,
										PVOID pvSendConnectData,
										DWORD dwSendConnectDataSize)
{
	HRESULT							hr;
	char*							pszBuffer = NULL;
	HANDLE							hRedirMMMutex = NULL;
	PIPCOBJMMREDIRECTFILEHEADER		pHeader = NULL;


	if (this == NULL)
	{
		DPL(0, "This object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (bad object pointer)

	if (this->m_pszAttachPointID != NULL)
	{
		DPL(0, "Object already connected!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (no attach point yet)

	if (pTNModuleID == NULL)
	{
		DPL(0, "Module ID pointer cannot be NULL!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (bad module ID pointer)

	if ((szPrePlannedID == NULL) || (strcmp(szPrePlannedID, "") == 0))
	{
		DPL(0, "Static connection ID cannot be NULL or just an empty string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (bad attach point ID)


	// Create/open the mutex used to protect read/write access to the redirection
	// shared memory.

	TNsprintf(&pszBuffer, PREFIX_REDIR_MMMUTEX " (mod %s v%u.%u) %s",
			4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
			pTNModuleID->dwMinorVersion1, szPrePlannedID);

	// We can't be the initial owner of the mutex because it may have been created
	// already, in which case the TRUE would be ignored.
	hRedirMMMutex = CreateMutex(NULL, FALSE, pszBuffer);
	if (hRedirMMMutex == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create mutex %s!", 1, pszBuffer);
		goto ERROR_EXIT;
	} // end if (couldn't create mutex)
	TNsprintf_free(&pszBuffer);


	// Grab the write lock, which is just to prevent us from reading it before the
	// other side has written it.

#pragma TODO(vanceo, "Convert to full blown Wait, including user cancel")
	while (WaitForSingleObjectEx(hRedirMMMutex, INFINITE, TRUE) == WAIT_IO_COMPLETION)
	{
		DPL(1, "I/O Completion.", 0);
	} // end while (there's an I/O completion)



	// Create/open the actual shared redirection memory.

	TNsprintf(&pszBuffer, PREFIX_REDIR_MMFILE " (mod %s v%u.%u) %s",
			4, pTNModuleID->szBriefName, pTNModuleID->dwMajorVersion,
			pTNModuleID->dwMinorVersion1, szPrePlannedID);

	DPL(8, "Opening redirect file \"%s\" to retrieve real attach point ID.",
		1, pszBuffer);

	this->m_hRedirMMFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
										0, (sizeof (IPCOBJMMREDIRECTFILEHEADER) + MAX_REDIR_DATA_SIZE),
										pszBuffer);
	if (this->m_hRedirMMFile == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create file mapping %s!", 1, pszBuffer);
		goto ERROR_EXIT;
	} // end if (couldn't create file mapping)
	TNsprintf_free(&pszBuffer);


	pHeader = (PIPCOBJMMREDIRECTFILEHEADER) MapViewOfFile(this->m_hRedirMMFile,
															FILE_MAP_ALL_ACCESS,
															0, 0, 0);
	if (pHeader == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't map view of file!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't map view of file)


	// Connect using the given ID.
	hr = this->Connect(FALSE, pTNModuleID, ((char*) (pHeader + 1)), hConnectEvent,
						pvSendConnectData, dwSendConnectDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't connect using ID \"%s\"!", 1, (pHeader + 1));
		goto ERROR_EXIT;
	} // end if (couldn't connect)


	UnmapViewOfFile(pHeader);
	pHeader = NULL;

	CloseHandle(this->m_hRedirMMFile);
	this->m_hRedirMMFile = NULL;

	ReleaseMutex(hRedirMMMutex);
	CloseHandle(hRedirMMMutex);
	hRedirMMMutex = NULL;

	return (S_OK);


ERROR_EXIT:

	if (pHeader != NULL)
	{
		UnmapViewOfFile(pHeader);
		pHeader = NULL;
	} // end if (mapped view)

	if (this->m_hRedirMMFile)
	{
		CloseHandle(this->m_hRedirMMFile);
		this->m_hRedirMMFile = NULL;
	} // end if (we have the lock)

	if (hRedirMMMutex)
	{
		ReleaseMutex(hRedirMMMutex);
		CloseHandle(hRedirMMMutex);
		hRedirMMMutex = NULL;
	} // end if (we have the lock)

	if (pszBuffer != NULL)
		TNsprintf_free(&pszBuffer);

	return (hr);
} // CTNIPCObject::ConnectWithStaticID
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::GetConnectData()"
//==================================================================================
// CTNIPCObject::GetConnectData
//----------------------------------------------------------------------------------
//
// Description: Sets the pointers passed in to the data sent by the other side when
//				it connected.
//				Note: this is not a copy of the data.
//
// Arguments:
//	PVOID* ppvData			Pointer to have set to point to data sent by other side
//							when it connected.
//	DWORD* pdwDataSize		Place to store size of data sent by other side.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::GetConnectData(PVOID* ppvData, DWORD* pdwDataSize)
{
	if (this == NULL)
	{
		DPL(0, "This object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)

	if (! this->m_fConnected)
	{
		DPL(0, "Object not connected!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (not connected)


	(*ppvData) = this->m_pvConnectData;
	(*pdwDataSize) = this->m_dwConnectDataSize;

	if (this->m_pvConnectData == NULL)
	{
		DPL(0, "WARNING: No connect data.", 0);
	} // end if (there's no data)

	return (S_OK);
} // CTNIPCObject::GetConnectData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::ReceiveMessage()"
//==================================================================================
// CTNIPCObject::ReceiveMessage
//----------------------------------------------------------------------------------
//
// Description: Retrieves a message for the object from the shared memory.  This
//				assumes the data there is actually for this object.  The memory lock
//				is not taken since the sending side actually still holds it.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::ReceiveMessage(void)
{
	HRESULT					hr;
	PIPCOBJMMFILEHEADER		pHeader = NULL;


	DPL(9, "==>", 0);


	pHeader = (PIPCOBJMMFILEHEADER) MapViewOfFile(this->m_hMMFile,
													FILE_MAP_ALL_ACCESS,
													0, 0, 0);
	if (pHeader == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't map view of file!", 0);
		goto DONE;
	} // end if (couldn't map view of file)
	

	switch (pHeader->dwCommand)
	{
		case IPCMSGID_CONNECT:
			hr = this->HandleConnectMsg(((pHeader->dwSendDataSize > 0) ? (pHeader + 1) : NULL),
										pHeader->dwSendDataSize);
		  break;

		case IPCMSGID_DISCONNECT:
			hr = this->HandleDisconnectMsg();
		  break;

		case IPCMSGID_PING:
			// Update the count so it knows we're still alive.
			this->m_dwLastMessageReceived = GetTickCount();

			hr = S_OK;
		  break;

		default:
			// Unknown command, let the child class handle it.  First update the
			// count so it knows we're still alive.
			this->m_dwLastMessageReceived = GetTickCount();

			// Let him know how much room there is in the reply buffer.  He should
			// override this number with the actual amount he used.
			pHeader->dwReplyDataSize = MAX_MESSAGE_SIZE - pHeader->dwSendDataSize;

			hr = this->HandleMessage(pHeader->dwCommand,
									((pHeader->dwSendDataSize > 0) ? (pHeader + 1) : NULL),
									pHeader->dwSendDataSize,
									((LPBYTE) (pHeader + 1)) + pHeader->dwSendDataSize,
									&(pHeader->dwReplyDataSize));
		  break;
	} // end switch (on the command in the buffer)

	if (hr != S_OK)
	{
		DPL(0, "Couldn't handle message (%u)!", 1, pHeader->dwCommand);
		goto DONE;
	} // end if (couldn't handle message)


	DPL(6, "Acknowledging message %u with event %x.",
		2, pHeader->dwCommand, this->m_hAckEvent);

	if (! SetEvent(this->m_hAckEvent))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set acknowledge event (%x)!", 1, this->m_hAckEvent);
		//goto DONE;
	} // end if (couldn't set event)


DONE:

	if (pHeader != NULL)
	{
		UnmapViewOfFile(pHeader);
		pHeader = NULL;
	} // end if (mapped view)
	
	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNIPCObject::ReceiveMessage
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::SendMessage()"
//==================================================================================
// CTNIPCObject::SendMessage
//----------------------------------------------------------------------------------
//
// Description: Puts a message for the other side into the shared memory.
//
// Arguments:
//	DWORD dwCommand		ID of command/message
//	PVOID pvData		Optional additional buffer with data for command.
//	DWORD dwDataSize	Size of message data.
//	DWORD dwTimeout		Maximum amount of time to allow for acknowledgement by other
//						side.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::SendMessage(DWORD dwCommand, PVOID pvData, DWORD dwDataSize,
								DWORD dwTimeout)
{
	HRESULT					hr = S_OK;
	PIPCOBJMMFILEHEADER		pHeader = NULL;
	HANDLE					ahWaitObjects[3];
	DWORD					dwNumWaitObjects = 0;
	DWORD					dwTimeElapsed = 0;
	DWORD					dwInterval;
	MSG						msg;
	BOOL					fHaveMMWriteLock = FALSE;



	DPL(9, "==> (%u, %x, %u)", 3, dwCommand, pvData, dwDataSize);

	if (! this->m_fConnected)
	{
		DPL(0, "IPC Object not connected, can't send message!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (not connected)


	if (dwDataSize > MAX_MESSAGE_SIZE)
	{
		DPL(0, "Can't send message because %u is larger than max message size (%u)!",
			2, dwDataSize, MAX_MESSAGE_SIZE);
		hr = ERROR_BUFFER_TOO_SMALL;
		goto DONE;
	} // end if (message too large)



	DPL(4, "Waiting for lock to send message %u to %s...",
		3, dwCommand,
		((this->m_fOwnerCopy) ? "separate process" : "owner"));


	// Grab the mutex.
	ahWaitObjects[dwNumWaitObjects++] = this->m_hMMWriteMutex;

	if ((this->m_pExecutor->m_hUserCancelEvent != NULL) &&
		(dwCommand != IPCMSGID_DISCONNECT))
	{
		ahWaitObjects[dwNumWaitObjects++] = this->m_pExecutor->m_hUserCancelEvent;
	} // end if (owner side and there's a cancel event and not disconnect)

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array


REWAIT_MUTEX:

	// Since TAPI requires a message pump, I've built a simple (hacky) one into
	// this Wait in case the user was expecting a message to arrive...
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		DPL(1, "Got Windows message.", 0);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end if (there's a Windows message)


	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
								MESSAGE_PUMP_CHECK_INTERVAL, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Got the lock, we're cool.

			hr = S_OK;
		  break;

		case WAIT_OBJECT_0 + 1:
			// Got user cancel event

			DPL(0, "User cancelled waiting for mutex to send message!", 0);

			hr = TNWR_USERCANCEL;
			goto DONE;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT_MUTEX;
		  break;

		case WAIT_TIMEOUT:
			// Interval elapsed.  Poll the message queue and rewait.
			goto REWAIT_MUTEX;
		  break;

		default:
			// What?
			DPL(0, "Received unexpected return from WaitForMultipleObjectsEx!", 0);

			if (hr == S_OK)
				hr = E_FAIL;

			goto DONE;
		  break;
	} // end switch (on wait result)


	this->m_dwMMWriteLockRefCount++;
	fHaveMMWriteLock = TRUE;
	//DPL(9, "Took lock.", 0);


	pHeader = (PIPCOBJMMFILEHEADER) MapViewOfFile(this->m_hMMFile,
													FILE_MAP_ALL_ACCESS,
													0, 0, 0);
	if (pHeader == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't map view of file!", 0);
		goto DONE;
	} // end if (couldn't map view of file)


	pHeader->dwCommand = dwCommand;

	if ((pvData != NULL) && (dwDataSize > 0))
		pHeader->dwSendDataSize = dwDataSize;
	else
		pHeader->dwSendDataSize = 0;


	if ((pvData != NULL) && (dwDataSize > 0))
		CopyMemory((pHeader + 1), pvData, dwDataSize);



	DPL(9, "Setting send event %x.", 1, this->m_hSendEvent);


	// This send event corresponds to what the other side sees as a receive event.
	if (! SetEvent(this->m_hSendEvent))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set send event (%x)!", 1, this->m_hSendEvent);
		goto DONE;
	} // end if (couldn't set event)


	// Wait for the other side to acknowledge.

	DPL(4, "Waiting for %s acknowledgement of message %u...",
		2, ((this->m_fOwnerCopy) ? "separate process'" : "owner's"),
		dwCommand);

	dwNumWaitObjects = 0; // need to reset it, since it got used above
	ahWaitObjects[dwNumWaitObjects++] = this->m_hAckEvent;

	if ((this->m_pExecutor->m_hUserCancelEvent != NULL) &&
		(dwCommand != IPCMSGID_DISCONNECT))
	{
		ahWaitObjects[dwNumWaitObjects++] = this->m_pExecutor->m_hUserCancelEvent;
	} // end if (owner side and there's a cancel event and not disconnect)

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array

REWAIT_ACK:

	// Since TAPI requires a message pump, I've built a simple (hacky) one into
	// this Wait in case the user was expecting a message to arrive...
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		DPL(1, "Got Windows message.", 0);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end if (there's a Windows message)


	// If we can't fit a whole interval in, just use the remainder of the time
	// allowed by the given timeout.
	if ((dwTimeout != INFINITE) &&
		(dwTimeElapsed + MESSAGE_PUMP_CHECK_INTERVAL > dwTimeout))
	{
		dwInterval = dwTimeout - dwTimeElapsed;
	} // end if (there's a timeout and there's less than a full chunk left)
	else
		dwInterval = MESSAGE_PUMP_CHECK_INTERVAL;


	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
								dwInterval, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// The event fired.

			DPL(4, "%s acknowledged message %u.",
				2, ((this->m_fOwnerCopy) ? "Separate process" : "Owner"),
				dwCommand); 

			hr = S_OK;
		  break;

		case WAIT_OBJECT_0 + 1:
			// User cancelled

			DPL(0, "User cancelled wait for acknowledgement for command %u!",
				1, dwCommand);

			hr = TNWR_USERCANCEL;
			goto DONE;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT_ACK;
		  break;

		case WAIT_TIMEOUT:
			dwTimeElapsed += dwInterval;

			// If there's still some time remaining, go back to waiting.
			if ((dwTimeout == INFINITE) ||
				(dwTimeElapsed < dwTimeout))
			{
				goto REWAIT_ACK;
			} // end if (no timeout, or we haven't hit it yet)


			DPL(1, "Timed out waiting for %s to acknowledge message, are you sure it's connected?",
				1, ((this->m_fOwnerCopy) ? "separate process" : "owner"));

			this->m_fConnected = FALSE;

			hr = TNWR_TIMEOUT;
			goto DONE;
		  break;

		default:
			DPL(0, "Got unexpected return from WaitForSingleObjectEx!", 0);

			if (hr == S_OK)
				hr = E_FAIL;
			goto DONE;
		  break;
	} // end switch (on wait result)


	// Just for completeness, let's erase the message.
	pHeader->dwCommand = 0;
	pHeader->dwSendDataSize = 0;
	pHeader->dwReplyDataSize = 0;


DONE:

	if (pHeader != NULL)
	{
		UnmapViewOfFile(pHeader);
		pHeader = NULL;
	} // end if (mapped view)

	if (fHaveMMWriteLock)
	{
		ReleaseMutex(this->m_hMMWriteMutex);
		fHaveMMWriteLock = FALSE;
		this->m_dwMMWriteLockRefCount--;
		//DPL(9, "Dropped lock.", 0);
	} // end if (we have the write lock)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNIPCObject::SendMessage
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::PrepareSendBuffer()"
//==================================================================================
// CTNIPCObject::PrepareSendBuffer
//----------------------------------------------------------------------------------
//
// Description: Creates a buffer of the given size in preparation for sending the
//				buffer as message data.  This call should be followed by writing the
//				data, then a send buffer call.
//				IMPORTANT: This takes the write lock, so you may block until you can
//				own it, and you must release the buffer with ReleaseBuffers when
//				done.
//
// Arguments:
//	DWORD dwCommand			Command to send.
//	PVOID* ppvSendBuffer	Pointer to place to store send data buffer.
//	DWORD dwSendBufferSize	Size of send data buffer.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::PrepareSendBuffer(DWORD dwCommand, PVOID* ppvSendBuffer,
										DWORD dwSendBufferSize)
{
	HRESULT					hr = S_OK;
	PIPCOBJMMFILEHEADER		pHeader = NULL;
	HANDLE					ahWaitObjects[3];
	DWORD					dwNumWaitObjects = 0;
	MSG						msg;


	if (! this->m_fConnected)
	{
		DPL(0, "IPC Object not connected, can't prepare send buffer!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (not connected)


	if (dwSendBufferSize > MAX_MESSAGE_SIZE)
	{
		DPL(0, "Can't send message because %u is larger than max message size (%u)!",
			2, dwSendBufferSize, MAX_MESSAGE_SIZE);
		hr = ERROR_BUFFER_TOO_SMALL;
		goto DONE;
	} // end if (message too large)


	DPL(9, "++", 0);


	DPL(4, "Waiting for lock to send message %u to %s...",
		3, dwCommand,
		((this->m_fOwnerCopy) ? "separate process" : "owner"));


	// Grab the mutex.
	ahWaitObjects[dwNumWaitObjects++] = this->m_hMMWriteMutex;

	if ((this->m_pExecutor->m_hUserCancelEvent != NULL) &&
		(dwCommand != IPCMSGID_DISCONNECT))
	{
		ahWaitObjects[dwNumWaitObjects++] = this->m_pExecutor->m_hUserCancelEvent;
	} // end if (owner side and there's a cancel event and not disconnect)

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array


REWAIT:

	// Since TAPI requires a message pump, I've built a simple (hacky) one into
	// this Wait in case the user was expecting a message to arrive...
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		DPL(1, "Got Windows message.", 0);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end if (there's a Windows message)


	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
								MESSAGE_PUMP_CHECK_INTERVAL, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Got the lock, we're cool.

			hr = S_OK;
		  break;

		case WAIT_OBJECT_0 + 1:
			// Got user cancel event

			DPL(0, "User cancelled waiting for mutex for message buffer!", 0);

			hr = TNWR_USERCANCEL;
			goto DONE;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		case WAIT_TIMEOUT:
			// Interval elapsed.  Poll the message queue and rewait.
			goto REWAIT;
		  break;

		default:
			// What?
			DPL(0, "Received unexpected return from WaitForMultipleObjectsEx!", 0);

			if (hr == S_OK)
				hr = E_FAIL;

			goto DONE;
		  break;
	} // end switch (on wait result)


	this->m_dwMMWriteLockRefCount++;
	//DPL(9, "Took lock.", 0);


	pHeader = (PIPCOBJMMFILEHEADER) MapViewOfFile(this->m_hMMFile,
													FILE_MAP_ALL_ACCESS,
													0, 0, 0);
	if (pHeader == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't map view of file!", 0);
		goto DONE;
	} // end if (couldn't map view of file)


	pHeader->dwCommand = dwCommand;

	if ((ppvSendBuffer != NULL) && (dwSendBufferSize > 0))
		pHeader->dwSendDataSize = dwSendBufferSize;
	else
		pHeader->dwSendDataSize = 0;


	if (ppvSendBuffer != NULL)
		(*ppvSendBuffer) = (pHeader + 1);


DONE:

	return (hr);
} // CTNIPCObject::PrepareSendBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::SendBuffer()"
//==================================================================================
// CTNIPCObject::SendBuffer
//----------------------------------------------------------------------------------
//
// Description: Sends the current send buffer to the other side.
//
// Arguments:
//	PVOID pvSendBuffer		Pointer to send data buffer.
//	DWORD dwSendBufferSize	Size of send data buffer.
//	DWORD dwTimeout			Maximum amount of time to allow for acknowledgement by
//							other side.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::SendBuffer(PVOID pvSendBuffer, DWORD dwSendBufferSize,
								DWORD dwTimeout)
{
	HRESULT		hr = S_OK;
	HANDLE		ahWaitObjects[3];
	DWORD		dwNumWaitObjects = 0;
	DWORD		dwTimeElapsed = 0;
	DWORD		dwInterval;
	MSG			msg;


	if (! this->m_fConnected)
	{
		DPL(0, "IPC Object not connected, can't send message!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (not connected)

	if (this->m_dwMMWriteLockRefCount == 0)
	{
		DPL(0, "Don't have write lock!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (don't have write lock)


	// This send event corresponds to what the other side sees as a receive event.
	if (! SetEvent(this->m_hSendEvent))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set send event (%x)!", 1, this->m_hSendEvent);
		goto DONE;
	} // end if (couldn't set event)



	// Wait for the other side to acknowledge.

	DPL(4, "Waiting for %s acknowledgement of message %u...",
		2, ((this->m_fOwnerCopy) ? "separate process'" : "owner's"),
		(((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1)->dwCommand);

	ahWaitObjects[dwNumWaitObjects++] = this->m_hAckEvent;

	if (this->m_pExecutor->m_hUserCancelEvent != NULL)
	{
		ahWaitObjects[dwNumWaitObjects++] = this->m_pExecutor->m_hUserCancelEvent;
	} // end if (owner side and there's a cancel event)

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array

REWAIT:

	// Since TAPI requires a message pump, I've built a simple (hacky) one into
	// this Wait in case the user was expecting a message to arrive...
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		DPL(1, "Got Windows message.", 0);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end if (there's a Windows message)


	// If we can't fit a whole interval in, just use the remainder of the time
	// allowed by the given timeout.
	if ((dwTimeout != INFINITE) &&
		(dwTimeElapsed + MESSAGE_PUMP_CHECK_INTERVAL > dwTimeout))
	{
		dwInterval = dwTimeout - dwTimeElapsed;
	} // end if (there's a timeout and there's less than a full chunk left)
	else
		dwInterval = MESSAGE_PUMP_CHECK_INTERVAL;


	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
									dwInterval, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// The event fired.

			DPL(4, "%s acknowledged message %u.",
				2, ((this->m_fOwnerCopy) ? "Separate process" : "Owner"),
				(((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1)->dwCommand); 

			hr = S_OK;
		  break;

		case WAIT_OBJECT_0 + 1:
			// User cancelled

			DPL(0, "User cancelled wait for acknowledgement for command %u!",
				1, (((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1)->dwCommand);
			hr = TNWR_USERCANCEL;
			goto DONE;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		case WAIT_TIMEOUT:
			dwTimeElapsed += dwInterval;

			// If there's still some time remaining, go back to waiting.
			if ((dwTimeout == INFINITE) ||
				(dwTimeElapsed < dwTimeout))
			{
				goto REWAIT;
			} // end if (no timeout, or we haven't hit it yet)


			DPL(1, "Timed out waiting for %s to acknowledge message %u, are you sure it's connected?",
				2, ((this->m_fOwnerCopy) ? "separate process" : "owner"),
				(((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1)->dwCommand);

			this->m_fConnected = FALSE;

			hr = TNWR_TIMEOUT;
			goto DONE;
		  break;

		default:
			DPL(0, "Got unexpected return from WaitForSingleObjectEx!", 0);

			if (hr == S_OK)
				hr = E_FAIL;
			goto DONE;
		  break;
	} // end switch (on wait result)


DONE:

	return (hr);
} // CTNIPCObject::SendBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::SendBufferAndGetReply()"
//==================================================================================
// CTNIPCObject::SendBufferAndGetReply
//----------------------------------------------------------------------------------
//
// Description: Sends the current send buffer to the other side, and retrieves
//				pointers to the reply.
//
// Arguments:
//	PVOID pvSendBuffer			Pointer to send data buffer.
//	DWORD dwSendBufferSize		Size of send data buffer.
//	PVOID* ppvReplyBuffer		Place to store pointer to reply data buffer.
//	DWORD* pdwReplyBufferSize	Place to store size of reply data buffer.
//	DWORD dwTimeout				Maximum amount of time to allow for acknowledgement
//								by other side.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::SendBufferAndGetReply(PVOID pvSendBuffer, DWORD dwSendBufferSize,
										PVOID* ppvReplyBuffer, DWORD* pdwReplyBufferSize,
										DWORD dwTimeout)
{
	HRESULT					hr = S_OK;
	PIPCOBJMMFILEHEADER		pHeader = NULL;
	HANDLE					ahWaitObjects[3];
	DWORD					dwNumWaitObjects = 0;
	DWORD					dwTimeElapsed = 0;
	DWORD					dwInterval;
	MSG						msg;


	if (this == NULL)
	{
		DPL(0, "IPCObject pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (not a valid object)

	if (! this->m_fConnected)
	{
		DPL(0, "IPC Object not connected, can't send message!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (not connected)

	if (this->m_dwMMWriteLockRefCount == 0)
	{
		DPL(0, "Don't have write lock!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (don't have write lock)


	// This send event corresponds to what the other side sees as a receive event.
	if (! SetEvent(this->m_hSendEvent))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set send event (%x)!", 1, this->m_hSendEvent);
		goto DONE;
	} // end if (couldn't set event)


	// Wait for the other side to acknowledge.

	DPL(4, "Waiting for %s acknowledgement of message %u...",
		2, ((this->m_fOwnerCopy) ? "separate process'" : "owner's"),
		(((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1)->dwCommand);

	ahWaitObjects[dwNumWaitObjects++] = this->m_hAckEvent;

	if (this->m_pExecutor->m_hUserCancelEvent != NULL)
	{
		ahWaitObjects[dwNumWaitObjects++] = this->m_pExecutor->m_hUserCancelEvent;
	} // end if (owner side and there's a cancel event)

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array

REWAIT:

	// Since TAPI requires a message pump, I've built a simple (hacky) one into
	// this Wait in case the user was expecting a message to arrive...
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		DPL(1, "Got Windows message.", 0);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end if (there's a Windows message)


	// If we can't fit a whole interval in, just use the remainder of the time
	// allowed by the given timeout.
	if ((dwTimeout != INFINITE) &&
		(dwTimeElapsed + MESSAGE_PUMP_CHECK_INTERVAL > dwTimeout))
	{
		dwInterval = dwTimeout - dwTimeElapsed;
	} // end if (there's a timeout and there's less than a full chunk left)
	else
		dwInterval = MESSAGE_PUMP_CHECK_INTERVAL;


	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
									dwInterval, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// The event fired.

			DPL(4, "%s acknowledged message %u.",
				2, ((this->m_fOwnerCopy) ? "Separate process" : "Owner"),
				(((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1)->dwCommand); 

			hr = S_OK;
		  break;

		case WAIT_OBJECT_0 + 1:
			// User cancelled

			DPL(0, "User cancelled wait for acknowledgement for command %u!",
				1, (((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1)->dwCommand);
			hr = TNWR_USERCANCEL;
			goto DONE;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		case WAIT_TIMEOUT:
			dwTimeElapsed += dwInterval;

			// If there's still some time remaining, go back to waiting.
			if ((dwTimeout == INFINITE) ||
				(dwTimeElapsed < dwTimeout))
			{
				goto REWAIT;
			} // end if (no timeout, or we haven't hit it yet)


			DPL(1, "Timed out waiting for %s to acknowledge message %u, are you sure it's connected?",
				2, ((this->m_fOwnerCopy) ? "separate process" : "owner"),
				(((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1)->dwCommand);

			this->m_fConnected = FALSE;

			hr = TNWR_TIMEOUT;
			goto DONE;
		  break;

		default:
			DPL(0, "Got unexpected return from WaitForSingleObjectEx!", 0);

			if (hr == S_OK)
				hr = E_FAIL;

			goto DONE;
		  break;
	} // end switch (on wait result)


	// Back up to the real header based on the send buffer pointer
	pHeader = ((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1;


	(*pdwReplyBufferSize) = pHeader->dwReplyDataSize;

	if (pHeader->dwReplyDataSize > 0)
		(*ppvReplyBuffer) = ((LPBYTE) (pHeader + 1)) + pHeader->dwSendDataSize;
	else
		(*ppvReplyBuffer) = NULL;



DONE:

	return (hr);
} // CTNIPCObject::SendBufferAndGetReply
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::ReturnMessageBuffers()"
//==================================================================================
// CTNIPCObject::ReturnMessageBuffers
//----------------------------------------------------------------------------------
//
// Description: Recycles the send and reply buffers previously allocated.
//
// Arguments:
//	PVOID pvSendBuffer			Pointer to send data buffer.
//	DWORD dwSendBufferSize		Size of send data buffer.
//	PVOID pvReplyBuffer			Pointer to reply data buffer.
//	DWORD dwReplyBufferSize		Size of reply data buffer.
//
// Returns: None.
//==================================================================================
void CTNIPCObject::ReturnMessageBuffers(PVOID pvSendBuffer, DWORD dwSendBufferSize,
										PVOID pvReplyBuffer, DWORD dwReplyBufferSize)
{
	PIPCOBJMMFILEHEADER		pHeader = NULL;


	if (this == NULL)
	{
		DPL(0, "IPCObject pointer is NULL!", 0);
		return;
	} // end if (not a valid object)

	if (this->m_dwMMWriteLockRefCount == 0)
	{
		DPL(0, "Don't have write lock!", 0);
		return;
	} // end if (don't have write lock)

	// Back up to the real header based on the send buffer pointer
	pHeader = ((PIPCOBJMMFILEHEADER) pvSendBuffer) - 1;

	// Just for completeness, let's erase the message.
	pHeader->dwCommand = 0;
	pHeader->dwSendDataSize = 0;
	pHeader->dwReplyDataSize = 0;


	UnmapViewOfFile(pHeader);
	pHeader = NULL;

	ReleaseMutex(this->m_hMMWriteMutex);
	this->m_dwMMWriteLockRefCount--;
	//DPL(9, "Dropped lock.", 0);


	DPL(9, "--", 0);
} // CTNIPCObject::ReturnMessageBuffers
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::PingOwner()"
//==================================================================================
// CTNIPCObject::PingOwner
//----------------------------------------------------------------------------------
//
// Description: Alerts the owning object that we're still alive.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::PingOwner(void)
{
	HRESULT		hr;


	DPL(9, "==>", 0);


#ifdef DEBUG
	if (this->m_fOwnerCopy)
	{
		DPL(0, "Object is slave owned (not process-side)?!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (this object is not connected)

	if (! this->m_fConnected)
	{
		DPL(0, "Object is not connected?!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (this object is not connected)
#endif // DEBUG

	hr = this->SendMessage(IPCMSGID_PING, NULL, 0, IPCOBJ_ACK_WAIT_TIMEOUT);
	if (hr != S_OK)
	{
		DPL(0, "Failed sending ping message!", 0);
		goto DONE;
	} // end if (failed sending ping message)


DONE:
	
	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNIPCObject::PingOwner
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::StartObjMsgRecvThread()"
//==================================================================================
// CTNIPCObject::StartObjMsgRecvThread
//----------------------------------------------------------------------------------
//
// Description: Creates the event and thread used for receving messages from the
//				owner.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::StartObjMsgRecvThread(void)
{
	HRESULT		hr = S_OK;
	DWORD		dwThreadID;


	DPL(9, "==>", 0);

	// If the thread already exists, then we're done.
	if (this->m_hObjMsgRecvThread != NULL)
		goto DONE;

	// Create kill thread manual reset event
	this->m_hKillObjMsgRecvThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (this->m_hKillObjMsgRecvThreadEvent == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create event!", 0);
		goto DONE;
	} // end if (couldn't create event)

	this->m_hObjMsgRecvThread = CreateThread(NULL, 0, IPCObjMsgRecvThreadProc,
											this, 0, &dwThreadID);
	if (this->m_hObjMsgRecvThread == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create thread!", 0);
		goto DONE;
	} // end if (couldn't create thread)


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} //  // CTNIPCObject::StartObjMsgRecvThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::KillObjMsgRecvThread()"
//==================================================================================
// CTNIPCObject::KillObjMsgRecvThread
//----------------------------------------------------------------------------------
//
// Description: Stops the thread that monitors incoming communication from the
//				owner.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::KillObjMsgRecvThread(void)
{
	HRESULT		hr = S_OK;


	DPL(9, "==>", 0);

	// Close all items associated with the thread
	if (this->m_hObjMsgRecvThread != NULL)
	{
		if (this->m_hKillObjMsgRecvThreadEvent == NULL) // ack, we won't be able to tell it to die
		{
			DPL(0, "Kill thread event doesn't exist!", 0);
			hr = E_FAIL;
			goto DONE;
		} // end if (no kill event)

		if (! SetEvent(this->m_hKillObjMsgRecvThreadEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set kill thread event (%x)!",
				1, this->m_hKillObjMsgRecvThreadEvent);
			goto DONE;
		} // end if (no kill event)

		switch (WaitForSingleObject(this->m_hObjMsgRecvThread, TIMEOUT_DIE_OBJMSGRECVTHREAD))
		{
			case WAIT_OBJECT_0:
				// what we want
			  break;

			case WAIT_TIMEOUT:
				DPL(0, "Waited %i ms for object thread to die but it didn't!",
					1, TIMEOUT_DIE_OBJMSGRECVTHREAD);


#ifndef DONT_BREAK_ON_KILLTHREAD_TIMEOUT
				DEBUGBREAK();
#endif // DONT_BREAK_ON_KILLTHREAD_TIMEOUT

			  break;

			default:
				DPL(0, "Got unexpected return code from WaitForSingleObject on the object thread!", 0);
			  break;
		} // end switch (on result of waiting for thread to die)

		CloseHandle(this->m_hObjMsgRecvThread);
		this->m_hObjMsgRecvThread = NULL;
	} // end if (the send thread exists)

	if (this->m_hKillObjMsgRecvThreadEvent != NULL)
	{
		CloseHandle(this->m_hKillObjMsgRecvThreadEvent);
		this->m_hKillObjMsgRecvThreadEvent = NULL;
	} // end if (have event)


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNIPCObject::KillObjMsgRecvThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::HandleConnectMsg()"
//==================================================================================
// CTNIPCObject::HandleConnectMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a connection message from the other side.
//
// Arguments:
//	LPVOID lpvData		Pointer to data sent by other side.
//	DWORD dwDataSize	Size of data sent by other side.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::HandleConnectMsg(LPVOID lpvData, DWORD dwDataSize)
{
	HRESULT		hr = S_OK;


	DPL(1, "%s connected to object %x (\"%s\").",
		3, ((this->m_fOwnerCopy) ? "Separate process" : "Owner"),
		this, this->m_pszAttachPointID);


	if (dwDataSize > 0)
	{
		this->m_pvConnectData = LocalAlloc(LPTR, dwDataSize);
		if (this->m_pvConnectData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		CopyMemory(this->m_pvConnectData, lpvData, dwDataSize);
		this->m_dwConnectDataSize = dwDataSize;

		DPL(8, "Got %u byte connect data buffer at %x.",
			2, this->m_dwConnectDataSize, this->m_pvConnectData);
	} // end if (there's connect data)


	// If we're owner side and we allowed a static ID, we can release it now.
	if ((this->m_fOwnerCopy) && (this->m_hRedirMutex != NULL))
	{
		DPL(7, "Closing redirection file.", 0);

		if (! CloseHandle(this->m_hRedirMMFile))
		{
			hr = GetLastError();
			DPL(0, "Couldn't close redirection MM file!", 0);
			goto DONE;
		} // end if (couldn't close handle)
		this->m_hRedirMMFile = NULL;

		ReleaseMutex(this->m_hRedirMutex);
		if (! CloseHandle(this->m_hRedirMutex))
		{
			hr = GetLastError();
			DPL(0, "Couldn't close redirection mutex!", 0);
			goto DONE;
		} // end if (couldn't close handle)
		this->m_hRedirMutex = NULL;
	} // end if (we're owner side)


	// Update this info so we can detect problems while connected.
	this->m_dwLastMessageReceived = GetTickCount();

	this->m_fConnected = TRUE;

	if (this->m_hConnectEvent != NULL)
	{
		if (! SetEvent(this->m_hConnectEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set connection event (%x)!", 1, this->m_hConnectEvent);
			goto DONE;
		} // end if (couldn't set event)

		// We don't need the event anymore
		CloseHandle(this->m_hConnectEvent);
		this->m_hConnectEvent = NULL;
	} // end if (there's an event to set)


DONE:

	return (hr);
} // CTNIPCObject::HandleConnectMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObject::HandleDisconnectMsg()"
//==================================================================================
// CTNIPCObject::HandleDisconnectMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a disconnection message from the other side.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObject::HandleDisconnectMsg(void)
{
	HRESULT		hr = S_OK;


	this->m_fConnected = FALSE;


	DPL(1, "Object %x (\"%s\") was disconnected by the %s.",
		3, this, this->m_pszAttachPointID,
		((this->m_fOwnerCopy) ? "separate process" : "owner"));


#pragma BUGBUG(vanceo, "Make sure we want to do this!")
	if ((this->m_pExecutor->m_hUserCancelEvent != NULL) &&
		(! SetEvent(this->m_pExecutor->m_hUserCancelEvent)))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set user cancel event (%x)!",
			1, this->m_pExecutor->m_hUserCancelEvent);
		goto DONE;
	} // end if (owner side and cancel event which failed)

	// Just in case there was a connect event, we need to release the person who
	// was waiting on this.
	if (this->m_hConnectEvent != NULL)
	{
		DPL(0, "WARNING: Disconnecting while still waiting to connect!", 0);

		if (! SetEvent(this->m_hConnectEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set connection event (%x)!", 1, this->m_hConnectEvent);
			goto DONE;
		} // end if (couldn't set event)

		// We don't need the event anymore
		CloseHandle(this->m_hConnectEvent);
		this->m_hConnectEvent = NULL;

		// We're not going to allow our child class to handle this because we
		// shouldn't actually have been connected.  If we did, it would probably
		// cause problems because their Connect() type call would fail plus they
		// would have their quit code called (usually causes all sorts of double
		// frees and shutdown races).

	} // end if (there's a connect event)
	else
	{
		DPL(9, "No connect event, allowing child class to handle disconnect.", 0);

		// Allows the derived class a chance to do anything it needs to.
		hr = this->HandleDisconnect();
		if (hr != S_OK)
		{
			DPL(0, "Derived classes HandleDisconnect function failed!", 0);
		} // end if (derived class' function failed)
	} // end else (there's no connect event)


DONE:

	return (hr);
} // CTNIPCObject::HandleDisconnectMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObjectsList::CTNIPCObjectsList()"
//==================================================================================
// CTNIPCObjectsList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNIPCObjectsList object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNIPCObjectsList::CTNIPCObjectsList(void):
	m_hObjMonitorThread(NULL),
	m_hKillObjMonitorThreadEvent(NULL),
	m_hObjListChangedEvent(NULL)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNIPCObjectsList));
	} // CTNIPCObjectsList::CTNIPCObjectsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObjectsList::~CTNIPCObjectsList()"
//==================================================================================
// CTNIPCObjectsList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNIPCObjectsList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNIPCObjectsList::~CTNIPCObjectsList(void)
{
	HRESULT		hr;


	//DPL(0, "this = %x", 1, this);


#pragma TODO(vanceo, "Detach all the objects")

	hr = this->KillObjMonitorThread();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't kill object monitor thread!  %e", 1, hr);
	} // end if (couldn't kill thread)
} // CTNIPCObjectsList::~CTNIPCObjectsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObjectsList::AddObject()"
//==================================================================================
// CTNIPCObjectsList::AddObject
//----------------------------------------------------------------------------------
//
// Description: Prepares and adds an IPC object to this list.
//
// Arguments:
//	PTNIPCOBJECT pObject	Pointer to object to add.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObjectsList::AddObject(PTNIPCOBJECT pObject)
{
	HRESULT		hr;


	this->EnterCritSection();

	if (pObject->m_hReceiveEvent == NULL)
	{
		DPL(0, "Object %x doesn't have a receive event yet (make sure Connect was called)!",
			1, pObject);
		hr = E_FAIL;
		goto DONE;
	} // end if (the object doesn't have a receive event yet)


	hr = this->Add(pObject);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add object to list!", 0);
		goto DONE;
	} // end if (couldn't add object to list)


	// Notify the thread of the change (starting it up if necessary).  It won't
	// actually be able to do much about it until we drop the list lock below.

	hr = this->StartObjMonitorThread();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't start object monitor thread!", 0);
		goto DONE;
	} // end if (couldn't start thread)

	if (! SetEvent(this->m_hObjListChangedEvent))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set object list changed event (%x)!",
			1, this->m_hObjListChangedEvent);
		goto DONE;
	} // end if (couldn't set the list changed event)


DONE:

	this->LeaveCritSection();

	return (hr);
} // CTNIPCObjectsList::AddObject
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObjectsList::RemoveObject()"
//==================================================================================
// CTNIPCObjectsList::RemoveObject
//----------------------------------------------------------------------------------
//
// Description: Removes an IPC object from this list.
//
// Arguments:
//	PTNIPCOBJECT pObject	Pointer to object to add.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObjectsList::RemoveObject(PTNIPCOBJECT pObject)
{
	HRESULT		hr;


	this->EnterCritSection();

	hr = this->RemoveFirstReference(pObject);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove object from list!", 0);
		goto DONE;
	} // end if (couldn't add object to list)

	if (pObject->m_dwRefCount == 0)
	{
		DPL(7, "Deleting IPC object %x.", 1, pObject);
		delete (pObject);
		pObject = NULL;
	} // end if (can delete object)
	else
	{
		DPL(7, "Not deleting IPC object %x, its refcount = %u.",
			1, pObject, pObject->m_dwRefCount);
	} // end else (can't delete object)


	// Notify the thread of the change.  It won't actually be able to do much about
	// it until we drop the list lock below.

	if (! SetEvent(this->m_hObjListChangedEvent))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set object list changed event (%x)!",
			1, this->m_hObjListChangedEvent);
		goto DONE;
	} // end if (couldn't set the list changed event)



	// This code is too problematic.  We'll have an extra unused thread lying
	// around if we remove all IPC objects, but at least we won't deadlock.
	/*
	if (this->Count() <= 0)
	{
		// HACKHACK: We drop the lock because if the last object got a message
		// while we were trying to remove it or it was already rebuilding the array
		// or something, then it's going to be stuck trying to take the list lock.
		// By sleeping here we hopefully allow it time to take the lock so we will
		// block trying to retake it until the obj monitor thread is done figuring
		// out that the object which was to receive a message is now gone.
		this->LeaveCritSection();

		Sleep(500);

		this->EnterCritSection();

		// Make sure nobody added an object while we dropped the lock.
		if (this->Count() <= 0)
		{
			// Tell the thread to die.
			hr = this->KillObjMonitorThread();
			if (hr != S_OK)
			{
				DPL(0, "Couldn't tell object monitor thread to die!", 0);
				goto DONE;
			} // end if (couldn't tell thread to die)
		} // end if (nobody added an object)
	} // end if (that was the last object)
	*/


DONE:

	this->LeaveCritSection();

	return (hr);
} // CTNIPCObjectsList::RemoveObject
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObjectsList::StartObjMonitorThread()"
//==================================================================================
// CTNIPCObjectsList::StartObjMonitorThread
//----------------------------------------------------------------------------------
//
// Description: Creates the events and thread used for monitoring incoming messages
//				from objects.
//				If the thread was already started, then we return S_OK.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObjectsList::StartObjMonitorThread(void)
{
	HRESULT		hr = S_OK;
	DWORD		dwThreadID;


	DPL(9, "==>", 0);

	// If the thread already exists, then we're done.
	if (this->m_hObjMonitorThread != NULL)
		goto DONE;


	// Create kill thread manual reset event
	this->m_hKillObjMonitorThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (this->m_hKillObjMonitorThreadEvent == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create kill event!", 0);
		goto DONE;
	} // end if (couldn't create event)

	// Create new object auto reset event
	this->m_hObjListChangedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (this->m_hObjListChangedEvent == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create list changed event!", 0);
		goto DONE;
	} // end if (couldn't create event)

	this->m_hObjMonitorThread = CreateThread(NULL, 0, IPCObjMonitorThreadProc,
											this, 0, &dwThreadID);
	if (this->m_hObjMonitorThread == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create thread!", 0);
		goto DONE;
	} // end if (couldn't create thread)

DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNIPCObjectsList::StartObjMonitorThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPCObjectsList::KillObjMonitorThread()"
//==================================================================================
// CTNIPCObjectsList::KillObjMonitorThread
//----------------------------------------------------------------------------------
//
// Description: Kills the thread that monitors messages from objects.
//				If the thread is already gone, it returns S_OK.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPCObjectsList::KillObjMonitorThread(void)
{
	HRESULT		hr = S_OK;


	DPL(9, "==>", 0);


	// Close all items associated with the thread
	if (this->m_hObjMonitorThread != NULL)
	{
		if (this->m_hKillObjMonitorThreadEvent == NULL) // ack, we won't be able to tell it to die
		{
			DPL(0, "Kill thread event doesn't exist!", 0);
			hr = E_FAIL;
			goto DONE;
		} // end if (no kill event)

		if (! SetEvent(this->m_hKillObjMonitorThreadEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set kill thread event (%x)!",
				1, this->m_hKillObjMonitorThreadEvent);
			goto DONE;
		} // end if (no kill event)


		switch (WaitForSingleObject(this->m_hObjMonitorThread, TIMEOUT_DIE_OBJMONITORTHREAD))
		{
			case WAIT_OBJECT_0:
				// what we want
			  break;
			case WAIT_TIMEOUT:
				DPL(0, "Waited %i ms for object monitor thread to die but it didn't!",
					1, TIMEOUT_DIE_OBJMONITORTHREAD);


#ifndef DONT_BREAK_ON_KILLTHREAD_TIMEOUT
				DEBUGBREAK();
#endif // DONT_BREAK_ON_KILLTHREAD_TIMEOUT

			  break;
			default:
				DPL(0, "Got unexpected return code from WaitForSingleObject on the object monitor thread!", 0);
			  break;
		} // end switch (on result of waiting for thread to die)

		CloseHandle(this->m_hObjMonitorThread);
		this->m_hObjMonitorThread = NULL;
	} // end if (the object monitor thread exists)

	if (this->m_hKillObjMonitorThreadEvent != NULL)
	{
		CloseHandle(this->m_hKillObjMonitorThreadEvent);
		this->m_hKillObjMonitorThreadEvent = NULL;
	} // end if (have event)

	if (this->m_hObjListChangedEvent != NULL)
	{
		CloseHandle(this->m_hObjListChangedEvent);
		this->m_hObjListChangedEvent = NULL;
	} // end if (have event)


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNIPCObjectsList::KillObjMonitorThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



#undef DEBUG_SECTION
#define DEBUG_SECTION	"IPCObjMsgRecvThreadProc()"
//==================================================================================
// IPCObjMsgRecvThreadProc
//----------------------------------------------------------------------------------
//
// Description: Waits for messages from the owner and handles them.
//
// Arguments:
//	LPVOID lpvParameter	Thread data.  Cast to a LPCTNIPCObject object.
//
// Returns: 0 if all goes well.
//==================================================================================
DWORD WINAPI IPCObjMsgRecvThreadProc(LPVOID lpvParameter)
{
	HRESULT			hr = S_OK;
	PTNIPCOBJECT	pObject = (PTNIPCOBJECT) lpvParameter;
	HANDLE			ahWaitObjects[3];
	DWORD			dwNonConnectedTimeouts = 0;


	DPL(1, "Starting up.", 0);

	if (pObject->m_hKillObjMsgRecvThreadEvent == NULL)
	{
		DPL(0, "Kill thread event doesn't exist!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (event doesn't exist)

#if DEBUG
	if (pObject->m_fOwnerCopy)
	{
		DPL(0, "Starting IPCObjMsgRecvThread on owner copy of leech!  DEBUGBREAK()-ing.", 0);
		DEBUGBREAK();
		hr = E_FAIL;
		goto DONE;
	} // end if (owner copy)
#endif // DEBUG

	ahWaitObjects[0] = pObject->m_hKillObjMsgRecvThreadEvent;
	ahWaitObjects[1] = pObject->m_hReceiveEvent;
	ahWaitObjects[2] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array

	do
	{
		DPL(9, "Waiting for object request.", 0);

		hr = WaitForMultipleObjects(2, ahWaitObjects, FALSE,
									OBJMSGRECVTHREAD_PING_INTERVAL);
		switch (hr)
		{
			case WAIT_OBJECT_0:
				// Time to die.

				hr = S_OK;
				goto DONE;
			  break;

			case WAIT_OBJECT_0 + 1:
				// We've got an incoming message

				hr = pObject->ReceiveMessage();
				if (hr != S_OK)
				{
					DPL(0, "WARNING: Receiving message failed!  %e", 1, hr);
					hr = S_OK;
				} // end if (couldn't receive message)
			  break;

			case WAIT_TIMEOUT:
				if (pObject->m_fConnected)
				{
					hr = pObject->PingOwner();
					if (hr != S_OK)
					{
						DPL(0, "Failed pinging owner!", 0);
						goto DONE;
					} // end if (failed pinging owner)
				} // end if (received at least one message)
				else
				{
					dwNonConnectedTimeouts++;

					if (dwNonConnectedTimeouts > OWNERNOTCONNECTED_WARNING_NUMINTERVALS)
					{
						if (dwNonConnectedTimeouts > OWNERNOTCONNECTED_ABORT_NUMINTERVALS)
						{
							DPL(0, "Owner has not connected in %u ms, failing!",
								1, (OBJMSGRECVTHREAD_PING_INTERVAL * dwNonConnectedTimeouts));

							// Pretend a disconnect message came in
							hr = pObject->HandleDisconnectMsg();
							if (hr != S_OK)
							{
								DPL(0, "Pretending to receive disconnect message failed!", 0);
								goto DONE;
							} // end if (disconnect failed)

							break; // stop looping
						} // end if (above abort threshold)
						else
						{
							DPL(0, "WARNING: Owner has not connected in %u ms.",
								1, (OBJMSGRECVTHREAD_PING_INTERVAL * dwNonConnectedTimeouts));
						} // end else (below abort threshold)
					} // end if (above warning threshold)
				} // end else (haven't received a message yet)
			  break;

			default:
				DPL(0, "Got unexpected return from WaitForMultipleObjects!", 0);
				goto DONE;
			  break;
		} // end switch (on wait return)
	} // end do
	while (TRUE);


DONE:

	if (hr != S_OK)
	{
		DPL(0, "%e", 1, hr);
	} // end if (quitting with a failure)


	DPL(1, "Exiting.", 0);

	return (hr);
} // IPCObjMsgRecvThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"IPCObjMonitorThreadProc()"
//==================================================================================
// IPCObjMonitorThreadProc
//----------------------------------------------------------------------------------
//
// Description: Waits for incoming messages from the objects, and handles them.
//
// Arguments:
//	LPVOID lpvParameter	Thread data.  Cast to an LPCTNIPCObjectsList object.
//
// Returns: 0 if all goes well.
//==================================================================================
DWORD WINAPI IPCObjMonitorThreadProc(LPVOID lpvParameter)
{
	HRESULT					hr = S_OK;
	PTNIPCOBJECTSLIST		pObjectList = (PTNIPCOBJECTSLIST) lpvParameter;
	BOOL					fRebuildArray = TRUE;
	BOOL					fInCritSect = FALSE;
	int						iNumObjects = 0;
	HANDLE*					pahWaitObjects = NULL;
	int						i;
	PTNIPCOBJECT			pObject = NULL;
	PIPCOBJMMFILEHEADER		pHeader = NULL;


	DPL(1, "Starting up.", 0);

	if (pObjectList->m_hKillObjMonitorThreadEvent == NULL)
	{
		DPL(0, "Kill thread event doesn't exist!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (event doesn't exist)

	if (pObjectList->m_hObjListChangedEvent == NULL)
	{
		DPL(0, "Object list change event doesn't exist!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (event doesn't exist)

	do
	{
		if (fRebuildArray)
		{
			DPL(9, "Rebuilding object list array.", 0);

			fRebuildArray = FALSE; // reset this

			// If the array already existed, clear it out (closing references).
			if (pahWaitObjects != NULL)
			{
				for(i = 2; i < iNumObjects + 2; i++)
				{
					// Close our reference to the item, ignoring errors
					CloseHandle(pahWaitObjects[i]);
					pahWaitObjects[i] = NULL;
				} // end for (each item already in the array)

				LocalFree(pahWaitObjects);
				pahWaitObjects = NULL;
			} // end if (there was an array already)

			pObjectList->EnterCritSection();
			fInCritSect = TRUE;


			// Reset the event just in case it got fired again to prevent needless
			// rebuilding of the array.
			if (! ResetEvent(pObjectList->m_hObjListChangedEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't reset the list changed event (%x)!",
					1, pObjectList->m_hObjListChangedEvent);
				goto DONE;
			} // end if (couldn't reset the event)


			iNumObjects = pObjectList->Count();

			pahWaitObjects = (HANDLE*) LocalAlloc(LPTR, ((iNumObjects + 3) * (sizeof (HANDLE))));
			if (pahWaitObjects == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			pahWaitObjects[0] = pObjectList->m_hKillObjMonitorThreadEvent;
			pahWaitObjects[1] = pObjectList->m_hObjListChangedEvent;

			for(i = 0; i < iNumObjects; i++)
			{
				pObject = (PTNIPCOBJECT) pObjectList->GetItem(i);
				if (pObject == NULL)
				{
					DPL(0, "Couldn't get object item %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't allocate memory)

				DPL(9, "Got object %i (%x)", 2, i, pObject);

				pObject->m_dwRefCount++;


#ifdef DEBUG
				if (! pObject->m_fOwnerCopy)
				{
					DPL(0, "Working with separate process side copy of leech!  DEBUGBREAK()-ing.", 0);
					DEBUGBREAK();
					hr = E_FAIL;
					goto DONE;
				} // end if (not owner copy)
#endif // DEBUG


				// We need a handle to the object, but we can't just use the
				// other handle because another thread might close it and we'd
				// have a bogus handle.
				if (! DuplicateHandle(GetCurrentProcess(), pObject->m_hReceiveEvent,
										GetCurrentProcess(), &(pahWaitObjects[i + 2]),
										0, FALSE, DUPLICATE_SAME_ACCESS))
				{
					hr = GetLastError();
					DPL(0, "Couldn't duplicate object receive event handle %i (%x)!",
						2, i, pObject->m_hReceiveEvent);
					goto DONE;
				} // end if (couldn't duplicate handle)


				pObject->m_dwRefCount--;
				if (pObject->m_dwRefCount == 0)
				{
					DPL(0, "Deleting object %x!?  DEBUGBREAK()-ing.",
						1, pObject);
					DEBUGBREAK();
				} // end if (refcount hit 0)
				else
				{
					DPL(7, "Not deleting object %x, its refcount is %u.",
						2, pObject, pObject->m_dwRefCount);
				} // end if (refcount hit 0)
				pObject = NULL;
			} // end for (each object in the array)

			pahWaitObjects[iNumObjects + 2] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array

			fInCritSect = FALSE;
			pObjectList->LeaveCritSection();
		} // end if (we should rebuild the array)


		DPL(9, "Waiting for message from %i objects.", 1, iNumObjects);

		hr = WaitForMultipleObjects(iNumObjects + 2, pahWaitObjects, FALSE,
									INFINITE);
		switch (hr)
		{
			case WAIT_OBJECT_0:
				// Time to die.

				hr = S_OK;
				goto DONE;
			  break;

			case WAIT_OBJECT_0 + 1:
				// The object list changed, rebuild it

				fRebuildArray = TRUE;
			  break;

			case WAIT_FAILED:
				// Something was goofy.

				hr = GetLastError();
				DPL(0, "Wait failed!", 0);
				goto DONE;
			  break;

			default:
#pragma BUGBUG(vanceo, "This mechanism doesn't handle list changes very well (could try Receiving on non-signalled object)")

				i = hr - WAIT_OBJECT_0 - 2; // get the index of the item that caused this

				pObjectList->EnterCritSection();
				fInCritSect = TRUE;

				if (iNumObjects != pObjectList->Count())
				{
					DPL(0, "List appears to have changed (%i != %i) but we haven't been told yet, forcing array rebuild and reactivating item %i!",
						3, iNumObjects, pObjectList->Count(), i);
					fRebuildArray = TRUE;

					// Otherwise, refire the event that triggered this so we can retry,
					// if we can.
					if ((i >= 0) && (i < iNumObjects))
					{
						if (! SetEvent(pahWaitObjects[i + 2]))
						{
							DPL(0, "WARNING: Couldn't refire event %x!  %e",
								2, pahWaitObjects[i + 2], GetLastError());
						} // end if (couldn't refire event)
					} // end if (have a valid index to try refiring)
					else
					{
						DPL(0, "WARNING: Item %i was not a valid object before, not trying to refire anything!",
							1, i);
					} // end else (no object to refire)

					fInCritSect = FALSE;
					pObjectList->LeaveCritSection();
					break;
				} // end if (list changed but we didn't see it yet)

				// Check to make sure it was a valid event
				if ((i < 0) || (i >= iNumObjects))
				{
					DPL(0, "Got unexpected return from WaitForMultipleObjects!", 0);
					hr = E_FAIL;
					goto DONE;
				} // end if (got wacky return)

				// Otherwise, grab the object that signalled
				pObject = (PTNIPCOBJECT) pObjectList->GetItem(i);
				if (pObject == NULL)
				{
					//BUGBUG figure out why it's actually doing this
					/*
					DPL(0, "Couldn't get activated object item %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
					*/
					DPL(0, "WARNING: Couldn't get activated object item %i, forcing array rebuild!", 1, i);
					fRebuildArray = TRUE;

					fInCritSect = FALSE;
					pObjectList->LeaveCritSection();
					break;
				} // end if (couldn't get item)

				pObject->m_dwRefCount++;

				fInCritSect = FALSE;
				pObjectList->LeaveCritSection();



				//DPL(1, "Object %i received a message.", 1, i);

				hr = pObject->ReceiveMessage();
				if (hr != S_OK)
				{
					DPL(0, "WARNING: Object %i failed receiving message!  %e",
						2, i, hr);
					hr = S_OK;
				} // end if (failed receiving message)


				// We're done with the object.
				pObject->m_dwRefCount--;
				if (pObject->m_dwRefCount == 0)
				{
					DPL(1, "Deleting IPC object %x (post receive).", 1, pObject);
					delete (pObject);

					// If we do that, we need to make damn sure we rebuild
					// the array.
					fRebuildArray = TRUE;
				} // end if (refcount hit 0)
				else
				{
					DPL(7, "Not deleting IPC object %x (post receive), its refcount is %u.",
						2, pObject, pObject->m_dwRefCount);
				} // end if (refcount hit 0)
				pObject = NULL;


				hr = S_OK;
			  break;
		} // end switch (on wait return)
	} // end do
	while (true);


DONE:

	if (hr != S_OK)
	{
		DPL(0, "%e", 1, hr);
	} // end if (quitting with a failure)

	if (pObject != NULL)
	{
		pObject->m_dwRefCount--;
		if (pObject->m_dwRefCount == 0)
		{
			DPL(7, "Deleting object %x.", 1, pObject);
			delete (pObject);
		} // end if (refcount hit 0)
		else
		{
			DPL(7, "Not deleting object %x, its refcount is %u.",
				2, pObject, pObject->m_dwRefCount);
		} // end if (refcount hit 0)
		pObject = NULL;
	} // end if (still have object pointer)

	if (fInCritSect)
	{
		pObjectList->LeaveCritSection();
		fInCritSect = FALSE;
	} // end if (in critical section)

	if (pahWaitObjects != NULL)
	{
		for(i = 2; i < iNumObjects + 2; i++)
		{
			// Close our reference to the item, ignoring errors
			CloseHandle(pahWaitObjects[i]);
			pahWaitObjects[i] = NULL;
		} // end for (each item already in the array)

		LocalFree(pahWaitObjects);
		pahWaitObjects = NULL;
	} // end if (we had an object array)

	if (pHeader != NULL)
	{
		UnmapViewOfFile(pHeader);
		pHeader = NULL;
	} // end if (have file view)


	DPL(1, "Exiting.", 0);

	return (hr);
} // IPCObjMonitorThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX