//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#ifndef _XBOX // no TAPI supporte
#include <tapi.h>
#endif // ! XBOX


#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
//#include "..\tncommon\strutils.h"

#include "tncontrl.h"
#include "main.h"

#include "tapidevs.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTAPIDevice::CTNTAPIDevice()"
//==================================================================================
// CTNTAPIDevice constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTAPIDevice object.  Initializes the data structures.
//
// Arguments:
//	char* szName		Name of this device.
//	DWORD dwDeviceID	ID of this device (just an index, really).
//
// Returns: None (just the object).
//==================================================================================
CTNTAPIDevice::CTNTAPIDevice(char* szName, DWORD dwDeviceID):
	CLString(szName),
	m_dwDeviceID(dwDeviceID),
	m_pszPhoneNumber(NULL)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTAPIDevice));
} // CTNTAPIDevice::CTNTAPIDevice
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTAPIDevice::~CTNTAPIDevice()"
//==================================================================================
// CTNTAPIDevice destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTAPIDevice object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTAPIDevice::~CTNTAPIDevice(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pszPhoneNumber != NULL)
	{
		LocalFree(this->m_pszPhoneNumber);
		this->m_pszPhoneNumber = NULL;
	} // end if (allocated a phone number)
} // CTNTAPIDevice::~CTNTAPIDevice
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTAPIDevice::GetPhoneNumber()"
//==================================================================================
// CTNTAPIDevice::GetPhoneNumber
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the string representing this device's phone
//				number, if any.
//				Note this is not a copy of the data.
//
// Arguments: None.
//
// Returns: Pointer to phone number for device, or NULL if none.
//==================================================================================
char* CTNTAPIDevice::GetPhoneNumber(void)
{
	return (this->m_pszPhoneNumber);
} // CTNTAPIDevice::GetPhoneNumber
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTAPIDevice::SetPhoneNumber()"
//==================================================================================
// CTNTAPIDevice::SetPhoneNumber
//----------------------------------------------------------------------------------
//
// Description: Sets the phone number for this device to be the one passed in..
//
// Arguments:
//	char* pszNewNumber		String containing new number for device, or NULL for
//							none.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTAPIDevice::SetPhoneNumber(char* pszNewNumber)
{
	HRESULT		hr = S_OK;


	if (this->m_pszPhoneNumber != NULL)
	{
		LocalFree(this->m_pszPhoneNumber);
		this->m_pszPhoneNumber = NULL;
	} // end if (already had phone number)

	if (pszNewNumber != NULL)
	{
		this->m_pszPhoneNumber = (char*) LocalAlloc(LPTR, strlen(pszNewNumber) + 1);
		if (this->m_pszPhoneNumber == NULL)
		{
			hr = E_OUTOFMEMORY;
		} // end if (couldn't allocate memory)
		else
		{
			strcpy(this->m_pszPhoneNumber, pszNewNumber);
		} // end if (could allocate memory)
	} // end if (there's a replacement phone number)

	return (hr);
} // CTNTAPIDevice::SetPhoneNumber
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTAPIDevicesList::GetFirstTAPIDeviceNameWithNumber()"
//==================================================================================
// CTNTAPIDevicesList::GetFirstTAPIDeviceNameWithNumber
//----------------------------------------------------------------------------------
//
// Description: Finds the first TAPI device in this list which has a phone number
//				associated with it, and sets the pointer passed in to point to the
//				name of the device.
//
// Arguments:
//	char** ppszDeviceName		Place to store pointer to name of device found.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNTAPIDevicesList::GetFirstTAPIDeviceNameWithNumber(char** ppszDeviceName)
{
	int				i;
	PTNTAPIDEVICE	pDevice;


	for(i = 0; i < this->Count(); i++)
	{
		pDevice = (PTNTAPIDEVICE) this->GetItem(i);
		if (pDevice == NULL)
		{
			DPL(0, "Couldn't get device %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get this item)

		if (pDevice->GetPhoneNumber() != NULL)
		{
			(*ppszDeviceName) = pDevice->GetString();
			return (S_OK);
		} // end if (found device with phone number)
	} // end for (each device)

	return (ERROR_NOT_FOUND);
} // CTNTAPIDevicesList::GetFirstTAPIDeviceNameWithNumber
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTAPIDevicesList::GetFirstTAPIDeviceNumber()"
//==================================================================================
// CTNTAPIDevicesList::GetFirstTAPIDeviceNumber
//----------------------------------------------------------------------------------
//
// Description: Finds the first TAPI device in this list which has a phone number
//				associated with it, and sets the pointer passed in to point to the
//				phone number for the device.
//
// Arguments:
//	char** ppszDeviceNumber		Place to store pointer to phone number of device
//								found.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNTAPIDevicesList::GetFirstTAPIDeviceNumber(char** ppszDeviceNumber)
{
	int				i;
	PTNTAPIDEVICE	pDevice;


	for(i = 0; i < this->Count(); i++)
	{
		pDevice = (PTNTAPIDEVICE) this->GetItem(i);
		if (pDevice == NULL)
		{
			DPL(0, "Couldn't get device %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get this item)

		if (pDevice->GetPhoneNumber() != NULL)
		{
			(*ppszDeviceNumber) = pDevice->GetPhoneNumber();
			return (S_OK);
		} // end if (found device with phone number)
	} // end for (each device)

	return (ERROR_NOT_FOUND);
} // CTNTAPIDevicesList::GetFirstTAPIDeviceNumber
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTAPIDevicesList::PackIntoBuffer()"
//==================================================================================
// CTNTAPIDevicesList::PackIntoBuffer
//----------------------------------------------------------------------------------
//
// Description: Flattens this list into a contiguous buffer for easy transmission.
//				If the pointer to store the results in is NULL, then no memory
//				is copied, but the buffer size is still set to the amount of memory
//				required, and ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	PVOID pvBuffer			Pointer to buffer to use.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to fill in with
//							buffer size required.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNTAPIDevicesList::PackIntoBuffer(PVOID pvBuffer, DWORD* pdwBufferSize)
{
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				i;
	PTNTAPIDEVICE	pDevice = NULL;
	DWORD			dwPhoneNumberStringSize;



	if (pvBuffer == NULL)
	{
		(*pdwBufferSize) = 0;
	} // end if (just retrieving size)

	this->EnterCritSection();

	i = this->Count();

	if (i <= 0)
	{
		this->LeaveCritSection();
		return (S_OK);
	} // end if (there aren't any items in this list)


	// If we have the buffer already, start copying.
	if (pvBuffer == NULL)
	{
		(*pdwBufferSize) = sizeof (int);
	} // end if (just retrieving size)
	else
	{
		// Copy the number of items in.
		CopyAndMoveDestPointer(lpCurrent, &i, sizeof (int))
	} // end else (have buffer)


	for(i = 0; i < this->Count(); i++)
	{
		pDevice = (PTNTAPIDEVICE) this->GetItem(i);
		if (pDevice == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get device %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)

		dwPhoneNumberStringSize = 0;
		if (pDevice->m_pszPhoneNumber != NULL)
			dwPhoneNumberStringSize = strlen(pDevice->m_pszPhoneNumber);


		if (pvBuffer == NULL)
		{
			(*pdwBufferSize) += sizeof (DWORD)
							+ (strlen(pDevice->GetString()) + 1)
							+ sizeof (DWORD) + dwPhoneNumberStringSize;
		} // end if (just retrieving size)
		else
		{
			CopyAndMoveDestPointer(lpCurrent, &(pDevice->m_dwDeviceID), sizeof (DWORD));
			CopyAndMoveDestPointer(lpCurrent, pDevice->GetString(), (strlen(pDevice->GetString()) + 1));
			CopyAndMoveDestPointer(lpCurrent, &(dwPhoneNumberStringSize), sizeof (DWORD));
			if (dwPhoneNumberStringSize > 0)
			{
				CopyAndMoveDestPointer(lpCurrent, pDevice->m_pszPhoneNumber, dwPhoneNumberStringSize);
			} // end if (theres actually a phone number string)
		} // end else (have buffer)

	} // end for (each device)
	this->LeaveCritSection();

	if (pvBuffer == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

	return (S_OK);
} // CTNTAPIDevicesList::PackIntoBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTAPIDevicesList::UnpackFromBuffer()"
//==================================================================================
// CTNTAPIDevicesList::UnpackFromBuffer
//----------------------------------------------------------------------------------
//
// Description: Populates this list with the contents previously stored in a
//				contiguous buffer.
//
// Arguments:
//	PVOID pvBuffer			Pointer to buffer to use.
//	DWORD dwBufferSize		Size of buffer.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNTAPIDevicesList::UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize)
{
	HRESULT			hr;
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				iNumItems = 0;
	int				i;
	PTNTAPIDEVICE	pDevice = NULL;
	char*			pszName;
	DWORD			dwDeviceID;
	DWORD			dwPhoneNumberStringSize;



	this->EnterCritSection();

	CopyAndMoveSrcPointer(&iNumItems, lpCurrent, sizeof (int));

	for(i = 0; i < iNumItems; i++)
	{
		CopyAndMoveSrcPointer(&dwDeviceID, lpCurrent, sizeof (DWORD));

		pszName = (char*) lpCurrent;
		lpCurrent += strlen(pszName) + 1;

		CopyAndMoveSrcPointer(&dwPhoneNumberStringSize, lpCurrent, sizeof (DWORD));
		
		pDevice = new (CTNTAPIDevice)(pszName, dwDeviceID);
		if (pDevice == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		if (dwPhoneNumberStringSize > 0)
		{
			pDevice->m_pszPhoneNumber = (char*) LocalAlloc(LPTR, dwPhoneNumberStringSize + 1);
			if (pDevice->m_pszPhoneNumber == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate string)

			CopyMemory(pDevice->m_pszPhoneNumber, lpCurrent, dwPhoneNumberStringSize);

			lpCurrent += dwPhoneNumberStringSize;
		} // end if (there's a phone number)


		hr = this->Add(pDevice);
		if (hr != S_OK)
		{
			DPL(0, "Adding device %i failed!", 1, i);
			goto DONE;
		} // end if (couldn't add item)

		pDevice = NULL; // forget about it so we don't free it below
	} // end for (each binary location list)


DONE:

	this->LeaveCritSection();

	if (pDevice != NULL)
	{
		delete (pDevice);
		pDevice = NULL;
	} // end if (have leftover object)

	return (hr);
} // CTNTAPIDevicesList::UnpackFromBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#ifndef _XBOX // no file printing supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTAPIDevicesList::PrintToFile()"
//==================================================================================
// CTNTAPIDevicesList::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints all the objects in this list to the file specified.
//
// Arguments:
//	HANDLE hFile	File to print to.
//
// Returns: None.
//==================================================================================
void CTNTAPIDevicesList::PrintToFile(HANDLE hFile)
{
	int				i;
	PTNTAPIDEVICE	pDevice = NULL;


	FileWriteLine(hFile, "\t(TAPIDevices)"); //ignoring errors

	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pDevice = (PTNTAPIDEVICE) this->GetItem(i);
		if (pDevice == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get TAPI device %i!", 1, i);
			return;
		} // end if (couldn't get that item)

		// ignoring errors
		FileSprintfWriteLine(hFile, "\t\t<%s>",
							1, pDevice->GetString());

		if (pDevice->GetPhoneNumber() != NULL)
		{
			FileSprintfWriteLine(hFile, "\t\t\tPhoneNumber= %s",
								1, pDevice->GetPhoneNumber());
		} // end if (there's a phone number)
	} // end for (each item in the list)

	this->LeaveCritSection();

	return;
} // CTNTAPIDevicesList::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX

