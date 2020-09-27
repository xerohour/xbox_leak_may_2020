//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#ifndef _XBOX // no TAPI supported
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
#define DEBUG_SECTION	"CTNCOMPort::CTNCOMPort()"
//==================================================================================
// CTNCOMPort constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNCOMPort object.  Initializes the data structures.
//
// Arguments:
//	char* szConnectedMachineName	Name of computer on other end of connection.
//	DWORD dwCOMPort					COM port this object represents.
//
// Returns: None (just the object).
//==================================================================================
CTNCOMPort::CTNCOMPort(char* szConnectedMachineName, DWORD dwCOMPort):
	CLString(szConnectedMachineName),
	m_dwCOMPort(dwCOMPort)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNCOMPort));
} // CTNCOMPort::CTNCOMPort
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCOMPort::~CTNCOMPort()"
//==================================================================================
// CTNCOMPort destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNCOMPort object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNCOMPort::~CTNCOMPort(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNCOMPort::~CTNCOMPort
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCOMPort::GetCOMPort()"
//==================================================================================
// CTNCOMPort::GetCOMPort
//----------------------------------------------------------------------------------
//
// Description: Returns the COM port number for this device.
//
// Arguments: None.
//
// Returns: The COM port number for this object.
//==================================================================================
DWORD CTNCOMPort::GetCOMPort(void)
{
	return (this->m_dwCOMPort);
} // CTNCOMPort::GetPhoneNumber
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCOMPortsList::PackIntoBuffer()"
//==================================================================================
// CTNCOMPortsList::PackIntoBuffer
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
HRESULT CTNCOMPortsList::PackIntoBuffer(PVOID pvBuffer, DWORD* pdwBufferSize)
{
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				i;
	PTNCOMPORT		pCOMPort = NULL;
	DWORD			dwNameStringSize;



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
		pCOMPort = (PTNCOMPORT) this->GetItem(i);
		if (pCOMPort == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get COM port %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)

		dwNameStringSize = 0;
		if (pCOMPort->GetString() != NULL)
			dwNameStringSize = strlen(pCOMPort->GetString());


		if (pvBuffer == NULL)
		{
			(*pdwBufferSize) += sizeof (DWORD)
							+ sizeof (DWORD) + dwNameStringSize;
		} // end if (just retrieving size)
		else
		{
			CopyAndMoveDestPointer(lpCurrent, &(pCOMPort->m_dwCOMPort), sizeof (DWORD));
			CopyAndMoveDestPointer(lpCurrent, &dwNameStringSize, sizeof (DWORD));
			if (dwNameStringSize > 0)
			{
				CopyAndMoveDestPointer(lpCurrent, pCOMPort->GetString(), dwNameStringSize);
			} // end if (theres actually a phone number string)
		} // end else (have buffer)

	} // end for (each device)
	this->LeaveCritSection();

	if (pvBuffer == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

	return (S_OK);
} // CTNCOMPortsList::PackIntoBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCOMPortsList::UnpackFromBuffer()"
//==================================================================================
// CTNCOMPortsList::UnpackFromBuffer
//----------------------------------------------------------------------------------
//
// Description: Populates this list with the contents previously stored in a
//				contiguous buffer.
//
// Arguments:
//	LPVOID lpvBuffer		Pointer to buffer to use.
//	DWORD dwBufferSize		Size of buffer.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNCOMPortsList::UnpackFromBuffer(LPVOID lpvBuffer, DWORD dwBufferSize)
{
	HRESULT		hr;
	LPBYTE		lpCurrent = (LPBYTE) lpvBuffer; // start at the beginning
	int			iNumItems = 0;
	int			i;
	PTNCOMPORT	pCOMPort = NULL;
	DWORD		dwCOMPort;
	char*		pszTemp = NULL;
	DWORD		dwNameStringSize;



	this->EnterCritSection();

	CopyAndMoveSrcPointer(&iNumItems, lpCurrent, sizeof (int));

	for(i = 0; i < iNumItems; i++)
	{
		CopyAndMoveSrcPointer(&dwCOMPort, lpCurrent, sizeof (DWORD));
		CopyAndMoveSrcPointer(&dwNameStringSize, lpCurrent, sizeof (DWORD));
		if (dwNameStringSize > 0)
		{
			pszTemp = (char*) LocalAlloc(LPTR, dwNameStringSize + 1);
			if (pszTemp == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate string)

			CopyAndMoveSrcPointer(pszTemp, lpCurrent, dwNameStringSize);
		} // end if (there's a phone number)
		
		pCOMPort = new (CTNCOMPort)(pszTemp, dwCOMPort);
		if (pCOMPort == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		if (pszTemp != NULL)
		{
			LocalFree(pszTemp);
			pszTemp = NULL;
		} // end if (there's a name string)

		hr = this->Add(pCOMPort);
		if (hr != S_OK)
		{
			DPL(0, "Adding COM port %i failed!", 1, i);
			goto DONE;
		} // end if (couldn't add item)

		pCOMPort = NULL; // forget about it so we don't free it below
	} // end for (each binary location list)


DONE:

	this->LeaveCritSection();

	if (pCOMPort != NULL)
	{
		delete (pCOMPort);
		pCOMPort = NULL;
	} // end if (have leftover object)

	if (pszTemp != NULL)
	{
		LocalFree(pszTemp);
		pszTemp = NULL;
	} // end if (there's a name string)

	return (hr);
} // CTNCOMPortsList::UnpackFromBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#ifndef _XBOX // no file printing supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCOMPortsList::PrintToFile()"
//==================================================================================
// CTNCOMPortsList::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints all the objects in this list to the file specified.
//
// Arguments:
//	HANDLE hFile	File to print to.
//
// Returns: None.
//==================================================================================
void CTNCOMPortsList::PrintToFile(HANDLE hFile)
{
	int			i;
	PTNCOMPORT	pCOMPort = NULL;


	FileWriteLine(hFile, "\t(COMPorts)"); //ignoring errors

	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pCOMPort = (PTNCOMPORT) this->GetItem(i);
		if (pCOMPort == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get COM port object %i!", 1, i);
			return;
		} // end if (couldn't get that item)

		// ignoring errors
		FileSprintfWriteLine(hFile, "\t\t<COM%u>",
							1, pCOMPort->GetCOMPort());

		if (pCOMPort->GetString() != NULL)
		{
			FileSprintfWriteLine(hFile, "\t\t\tConnectedTo= %s",
								1, pCOMPort->GetString());
		} // end if (there's a connection name)
	} // end for (each item in the list)

	this->LeaveCritSection();
} // CTNCOMPortsList::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX
