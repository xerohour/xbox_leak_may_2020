//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#ifndef _XBOX
#include <winsock.h>
#else // ! XBOX
#include <winsockx.h>		// Needed for XnetInitialize
#endif // XBOX

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
//#include "..\tncommon\linkedstr.h"
//#include "..\tncommon\strutils.h"

#include "tncontrl.h"
#include "main.h"

#include "ipaddrs.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddress::CTNIPAddress()"
//==================================================================================
// CTNIPAddress constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNIPAddress object.  Initializes the data structures.
//
// Arguments:
//	IN_ADDR* lpinaddr	Pointer to address to use for this object.
//	DWORD dwType		Type of address this is.
//
// Returns: None (just the object).
//
//  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
//
//==================================================================================
CTNIPAddress::CTNIPAddress(IN_ADDR* lpinaddr, DWORD dwType):
	m_ip(*(UNALIGNED IN_ADDR*)lpinaddr),
	m_dwType(dwType)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNIPAddress));
} // CTNIPAddress::CTNIPAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddress::~CTNIPAddress()"
//==================================================================================
// CTNIPAddress destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNIPAddress object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNIPAddress::~CTNIPAddress(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNIPAddress::~CTNIPAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddressesList::CTNIPAddressesList()"
//==================================================================================
// CTNIPAddressesList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNIPAddressesList object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNIPAddressesList::CTNIPAddressesList(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNIPAddressesList));
} // CTNIPAddressesList::CTNIPAddressesList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddressesList::~CTNIPAddressesList()"
//==================================================================================
// CTNIPAddressesList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNIPAddressesList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNIPAddressesList::~CTNIPAddressesList(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNIPAddressesList::~CTNIPAddressesList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddressesList::GetIPString()"
//==================================================================================
// CTNIPAddressesList::GetIPString
//----------------------------------------------------------------------------------
//
// Description: Retrieves the string representation of the IP address at the
//				specified index.
//
// Arguments:
//	int iIndex		Index of IP address to retrieve.
//	char* szString	String to store result in.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPAddressesList::GetIPString(int iIndex, char* szString)
{
	PTNIPADDRESS	pAddress = NULL;


	pAddress = (PTNIPADDRESS) this->GetItem(iIndex);
	if (pAddress == NULL)
	{
		DPL(0, "Couldn't get item %i!", iIndex);
		return (E_FAIL);
	} // end if (couldn't get item)
	
	strcpy(szString, inet_ntoa(pAddress->m_ip));

	return (S_OK);
} // CTNIPAddressesList::GetIPString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddressesList::AddIP()"
//==================================================================================
// CTNIPAddressesList::AddIP
//----------------------------------------------------------------------------------
//
// Description: Adds the specified IP address with the given type to the list.
//
// Arguments:
//	IN_ADDR* pinaddr	Pointer to IP address to add.
//	DWORD dwType		What type of IP address this is (TNIPADDR_xxx).
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNIPAddressesList::AddIP(IN_ADDR* pinaddr, DWORD dwType)
{
	HRESULT			hr;
	PTNIPADDRESS	pAddress = NULL;


	pAddress = new (CTNIPAddress)(pinaddr, dwType);
	if (pAddress == NULL)
	{
		return (E_OUTOFMEMORY);
	} // end if (couldn't allocate object)

	hr = this->Add(pAddress);
	if (pAddress == NULL)
	{
		DPL(0, "Couldn't add address to list!", 0);
	} // end if (couldn't add item)
	
	return (hr);
} // CTNIPAddressesList::AddIP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddressesList::LoadLocalIPs()"
//==================================================================================
// CTNIPAddressesList::LoadLocalIPs
//----------------------------------------------------------------------------------
//
// Description: Fills this list with all of this machines IP addresses.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNIPAddressesList::LoadLocalIPs(void)
{
	HRESULT			hr;
	char			szLocalHostName[256];
	PHOSTENT		phostent = NULL;
	int				i = 0;


	ZeroMemory(szLocalHostName, 256 * sizeof (char));
	if (gethostname(szLocalHostName, 256) != 0) 
	{
		DPL(0, "Could not get local machine's name!", 0);
		return (WSAGetLastError());
	} // end if (failed to get host name)
	
	phostent = gethostbyname(szLocalHostName);
	if (phostent == NULL)
	{
		DPL(0, "Could not get host info for name %s!  No net card?",
			1, szLocalHostName);
		return (WSAGetLastError());
	} // end if (failed to get host info from name)

	while (phostent->h_addr_list[i] != NULL)
	{
		/*
		if ((phostent->h_addrtype != IN_ADDR) ||
			(phostent->h_length != sizeof (IN_ADDR)))
		*/
		if (phostent->h_length != sizeof (IN_ADDR))
		{
			DPL(0, "Address does not appear to be an IN_ADDR!", 0);
			return (E_FAIL);
		} // end if (address not an IN_ADDR)

		hr = this->AddIP(((IN_ADDR*) (phostent->h_addr_list[i])), TNIPADDR_LOCAL);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add new address!", 0);
			return (hr);
		} // end if (address not an IN_ADDR)

		i++;
	} // end while (there's another address)
	
	return (S_OK);
} // CTNIPAddressesList::LoadLocalIPs
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddressesList::PackIntoBuffer()"
//==================================================================================
// CTNIPAddressesList::PackIntoBuffer
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
HRESULT CTNIPAddressesList::PackIntoBuffer(PVOID pvBuffer, DWORD* pdwBufferSize)
{
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				i;
	PTNIPADDRESS	pAddress = NULL;



	(*pdwBufferSize) = 0;


	this->EnterCritSection();

	i = this->Count();

	if (i <= 0)
	{
		this->LeaveCritSection();
		return (S_OK);
	} // end if (there aren't any items in this list)


	// Make sure the unpacker knows how many addresses we have

	(*pdwBufferSize) += sizeof (int) // number of items in the list
						+ (i * (sizeof (IN_ADDR) + sizeof (DWORD))); // all the items


	if (pvBuffer == NULL)
	{
		this->LeaveCritSection();
		return (ERROR_BUFFER_TOO_SMALL);
	} // end if (just retrieving size)


	// Copy the number of items in.
	CopyAndMoveDestPointer(lpCurrent, &i, sizeof (int))

	for(i = 0; i < this->Count(); i++)
	{
		pAddress = (PTNIPADDRESS) this->GetItem(i);
		if (pAddress == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get IP address %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)

		CopyAndMoveDestPointer(lpCurrent, &(pAddress->m_ip), sizeof (IN_ADDR));
		CopyAndMoveDestPointer(lpCurrent, &(pAddress->m_dwType), sizeof (DWORD));

	} // end for (each address)
	this->LeaveCritSection();


	return (S_OK);
} // CTNIPAddressesList::PackIntoBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddressesList::UnpackFromBuffer()"
//==================================================================================
// CTNIPAddressesList::UnpackFromBuffer
//----------------------------------------------------------------------------------
//
// Description: Populates this list with the contents previously stored in a
//				contiguous buffer.
//
// Arguments:
//	PVOID pvBuffer		Pointer to buffer to use.
//	DWORD dwBufferSize	Size of buffer.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNIPAddressesList::UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize)
{
	HRESULT			hr;
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				iNumItems = 0;
	int				i;
	PTNIPADDRESS	pAddress = NULL;



	this->EnterCritSection();

	CopyAndMoveSrcPointer(&iNumItems, lpCurrent, sizeof (int));

	for(i = 0; i < iNumItems; i++)
	{
        //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		pAddress = new (CTNIPAddress)(((IN_ADDR*) lpCurrent),
										*((UNALIGNED DWORD*) (lpCurrent + sizeof (IN_ADDR))));
		if (pAddress == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		// Move pointer to next item
		lpCurrent += sizeof (IN_ADDR) + sizeof (DWORD);			


		hr = this->Add(pAddress);
		if (hr != S_OK)
		{
			DPL(0, "Adding address %i failed!", 1, i);
			goto DONE;
		} // end if (couldn't add item)

		pAddress = NULL; // forget about it so we don't free it below
	} // end for (each binary location list)


DONE:

	this->LeaveCritSection();

	if (pAddress != NULL)
	{
		delete (pAddress);
		pAddress = NULL;
	} // end if (have leftover object)

	return (hr);
} // CTNIPAddressesList::UnpackFromBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNIPAddressesList::DoesIPExist()"
//==================================================================================
// CTNIPAddressesList::DoesIPExist
//----------------------------------------------------------------------------------
//
// Description: Searches the list for the given IN_ADDR, and returns TRUE if found,
//				FALSE otherwise.
//
// Arguments:
//	IN_ADDR* pinaddr	Pointer to IN_ADDR to look up.
//
// Returns: TRUE if found, FALSE otherwise.
//==================================================================================
BOOL CTNIPAddressesList::DoesIPExist(IN_ADDR* pinaddr)
{
	PTNIPADDRESS	pAddress = NULL;


	this->EnterCritSection();

	while (TRUE)
	{
		pAddress = (PTNIPADDRESS) this->GetNextItem(pAddress);
		if (pAddress == NULL)
			break;

		if (memcmp(&(pAddress->m_ip), pinaddr, sizeof (IN_ADDR)) == 0)
		{
			this->LeaveCritSection();
			return (TRUE);
		} // end if (found the IP address)

	} // end while (still have more items to look at)

	this->LeaveCritSection();
	
	// If we're here, it means we didn't find the address

	return (FALSE);
} // CTNIPAddressesList::DoesIPExist
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
