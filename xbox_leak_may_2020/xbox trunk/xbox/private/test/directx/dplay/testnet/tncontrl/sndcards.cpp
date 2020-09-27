//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <mmreg.h> // NT BUILD requires this before dsound.h
#include <dsound.h>


#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
//#include "..\tncommon\strutils.h"

#include "tncontrl.h"
#include "main.h"

#include "sndcards.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSoundCard::CTNSoundCard()"
//==================================================================================
// CTNSoundCard constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNSoundCard object.  Initializes the data structures.
//
// Arguments:
//	char* szName			Name of this sound card.
//	GUID* pguidDSoundID		GUID DSound uses to identify this sound card.
//	BOOL fHalfDuplex		Whether this device is known to be half-duplex or not.
//
// Returns: None (just the object).
//==================================================================================
CTNSoundCard::CTNSoundCard(char* szName, GUID* pguidDSoundID, BOOL fHalfDuplex):
	CLString(szName),
	m_fHalfDuplex(fHalfDuplex)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNSoundCard));

	CopyMemory(&(this->m_guidDSoundID), pguidDSoundID, sizeof (GUID));
} // CTNSoundCard::CTNSoundCard
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSoundCard::~CTNSoundCard()"
//==================================================================================
// CTNSoundCard destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNSoundCard object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNSoundCard::~CTNSoundCard(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNSoundCard::~CTNSoundCard
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSoundCard::GetDSoundGUID()"
//==================================================================================
// CTNSoundCard::GetDSoundGUID
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the GUID DSound used to identify this device.
//				Note this is not a copy of the data.
//
// Arguments: None.
//
// Returns: Pointer to DSound's GUID for device.
//==================================================================================
GUID* CTNSoundCard::GetDSoundGUID(void)
{
	return (&this->m_guidDSoundID);
} // CTNSoundCard::GetDSoundGUID
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSoundCard::IsKnownHalfDuplex()"
//==================================================================================
// CTNSoundCard::IsKnownHalfDuplex
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if this device is known to be half-duplex, FALSE if
//				not.
//
// Arguments: None.
//
// Returns: Whether the device is known to be half-duplex or not.
//==================================================================================
BOOL CTNSoundCard::IsKnownHalfDuplex(void)
{
	return (this->m_fHalfDuplex);
} // CTNSoundCard::IsKnownHalfDuplex
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSoundCardsList::PackIntoBuffer()"
//==================================================================================
// CTNSoundCardsList::PackIntoBuffer
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
HRESULT CTNSoundCardsList::PackIntoBuffer(PVOID pvBuffer, DWORD* pdwBufferSize)
{
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				i;
	PTNSOUNDCARD	pSoundCard = NULL;



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
		pSoundCard = (PTNSOUNDCARD) this->GetItem(i);
		if (pSoundCard == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get sound card %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)


		if (pvBuffer == NULL)
		{
			(*pdwBufferSize) += (strlen(pSoundCard->GetString()) + 1)
							+ sizeof (GUID)
							+ sizeof (BOOL);
		} // end if (just retrieving size)
		else
		{
			CopyAndMoveDestPointer(lpCurrent, pSoundCard->GetString(),
									(strlen(pSoundCard->GetString()) + 1));
			CopyAndMoveDestPointer(lpCurrent, pSoundCard->GetDSoundGUID(), sizeof (GUID));
			CopyAndMoveDestPointer(lpCurrent, &(pSoundCard->m_fHalfDuplex), sizeof (BOOL));
		} // end else (have buffer)

	} // end for (each device)
	this->LeaveCritSection();

	if (pvBuffer == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

	return (S_OK);
} // CTNSoundCardsList::PackIntoBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSoundCardsList::UnpackFromBuffer()"
//==================================================================================
// CTNSoundCardsList::UnpackFromBuffer
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
HRESULT CTNSoundCardsList::UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize)
{
	HRESULT			hr;
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				iNumItems = 0;
	int				i;
	PTNSOUNDCARD	pSoundCard = NULL;
	char*			pszName;
	GUID*			pguidDSoundID;
	BOOL			fHalfDuplex;



	this->EnterCritSection();

	CopyAndMoveSrcPointer(&iNumItems, lpCurrent, sizeof (int));

	for(i = 0; i < iNumItems; i++)
	{
		pszName = (char*) lpCurrent;
		lpCurrent += strlen(pszName) + 1;

		pguidDSoundID = (GUID*) lpCurrent;
		lpCurrent += sizeof (GUID);

		CopyAndMoveSrcPointer(&fHalfDuplex, lpCurrent, sizeof (BOOL));
		
		pSoundCard = new (CTNSoundCard)(pszName, pguidDSoundID, fHalfDuplex);
		if (pSoundCard == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		hr = this->Add(pSoundCard);
		if (hr != S_OK)
		{
			DPL(0, "Adding sound card %i failed!", 1, i);
			goto DONE;
		} // end if (couldn't add item)

		pSoundCard = NULL; // forget about it so we don't free it below
	} // end for (each binary location list)


DONE:

	this->LeaveCritSection();

	if (pSoundCard != NULL)
	{
		delete (pSoundCard);
		pSoundCard = NULL;
	} // end if (have leftover object)

	return (hr);
} // CTNSoundCardsList::UnpackFromBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#ifndef _XBOX // no file printing supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSoundCardsList::PrintToFile()"
//==================================================================================
// CTNSoundCardsList::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints all the objects in this list to the file specified.
//
// Arguments:
//	HANDLE hFile	File to print to.
//
// Returns: None.
//==================================================================================
void CTNSoundCardsList::PrintToFile(HANDLE hFile)
{
	int				i;
	PTNSOUNDCARD	pSoundCard = NULL;


	FileWriteLine(hFile, "\t(SoundCards)"); //ignoring errors

	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pSoundCard = (PTNSOUNDCARD) this->GetItem(i);
		if (pSoundCard == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get sound card %i!", 1, i);
			return;
		} // end if (couldn't get that item)

		// ignoring errors
		FileSprintfWriteLine(hFile, "\t\t<%s>",
							1, pSoundCard->GetString());

		FileSprintfWriteLine(hFile, "\t\t\tDSoundGUID= %g",
							1, pSoundCard->GetDSoundGUID());

		FileSprintfWriteLine(hFile, "\t\t\tKnownHalfDuplex= %B",
							1, pSoundCard->IsKnownHalfDuplex());
	} // end for (each item in the list)

	this->LeaveCritSection();

	return;
} // CTNSoundCardsList::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX
