//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"

#include "tncontrl.h"
#include "main.h"

#include "slaveinfo.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSlaveInfo::CTNSlaveInfo()"
//==================================================================================
// CTNSlaveInfo constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNSlaveInfo object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNSlaveInfo::CTNSlaveInfo(void)
{
	DPL(9, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNSlaveInfo));

	ZeroMemory(&(this->m_id), sizeof (TNCTRLMACHINEID));
	ZeroMemory(&(this->m_commdata), sizeof (COMMDATA));
	this->m_fReadyToTest = FALSE;
	this->m_dwEndtime = 0;
	this->m_pCurrentTest = NULL;
	this->m_dwLastReceiveTime = 0;
	this->m_dwNumPingsSinceReceive = 0;
	this->m_dwMultiInstanceKey = 0;
	this->m_iNumOtherInstancesInSession = 0;
} // CTNSlaveInfo::CTNSlaveInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSlaveInfo::~CTNSlaveInfo()"
//==================================================================================
// CTNSlaveInfo destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNSlaveInfo object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNSlaveInfo::~CTNSlaveInfo(void)
{
	DPL(9, "this = %x", 1, this);

	if (this->m_commdata.pvAddress != NULL)
	{
		LocalFree(this->m_commdata.pvAddress);
		this->m_commdata.pvAddress = NULL;
	} // end if (we have a control comm address structure)

	if (this->m_pCurrentTest != NULL)
	{
		this->m_pCurrentTest->m_dwRefCount--;
		if (this->m_pCurrentTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting current test %x.", 1, this->m_pCurrentTest);
			delete (this->m_pCurrentTest);
		} // end if (refcount hit 0)
		else
		{
			DPL(7, "Not deleting current test %x, it's refcount is %u.",
				2, this->m_pCurrentTest, this->m_pCurrentTest->m_dwRefCount);
		} // end else (refcount didn't hit 0)
		this->m_pCurrentTest = NULL;
	} // end if (there's a current test)
} // CTNSlaveInfo::~CTNSlaveInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSlaveInfo::GetTest()"
//==================================================================================
// CTNSlaveInfo::GetTest
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the test with the given top level and personal
//				unique IDs.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test the specified test is under
//								(may be same as the actual ID).
//	DWORD dwTestUniqueID		ID of actual test to retrieve.
//
// Returns: Pointer to the given test if found, NULL otherwise.
//==================================================================================
PTNTESTINSTANCEM CTNSlaveInfo::GetTest(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID)
{
	PTNTESTINSTANCEM	pTopLevelTest = NULL;


	// If we have a current test, see if the top level specified is that.
	if (this->m_pCurrentTest != NULL)
	{
		if (this->m_pCurrentTest->m_dwUniqueID == dwTopLevelUniqueID)
			pTopLevelTest = this->m_pCurrentTest;
	} // end if (there's a current test)

	// If we didn't get a top level test above, check all the ongoing tests to see
	// if they're the specified top level test.
	if (pTopLevelTest == NULL)
	{
		pTopLevelTest = (PTNTESTINSTANCEM) this->m_ongoingtestslist.GetTopLevelTest(dwTopLevelUniqueID);
	} // end if (haven't found top level test yet)


	// If we still don't have a top level test, we're screwed.
	if (pTopLevelTest == NULL)
		return (NULL);

	// Try to get the specified (sub)test.
	return (pTopLevelTest->GetTest(dwTestUniqueID));
} // CTNSlaveInfo::GetTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no file printing
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSlaveInfo::PrintToFile()"
//==================================================================================
// CTNSlaveInfo::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the info associated with this object to the file specified.
//
// Arguments:
//	HANDLE hFile	Open file to print to.
//
// Returns: None.
//==================================================================================
void CTNSlaveInfo::PrintToFile(HANDLE hFile)
{
	char	szBuffer[1024];


	FileSprintfWriteLine(hFile, "\tID= %u", 1, this->m_id.dwTime);

	FileSprintfWriteLine(hFile, "\tComputerName= %s", 1, this->m_szComputerName);
	FileSprintfWriteLine(hFile, "\tUserName= %s", 1, this->m_szUserName);


	//BUGBUG do we want a string?
	this->GetOSString(szBuffer); //ignoring errors
	FileSprintfWriteLine(hFile, "\tOS= %s", 1, szBuffer);

	switch (this->m_wProcessorArchitecture)
	{
		case PROCESSOR_ARCHITECTURE_INTEL:
			strcpy(szBuffer, "Intel");
		  break;

		case PROCESSOR_ARCHITECTURE_MIPS:
			strcpy(szBuffer, "MIPS");
		  break;

		case PROCESSOR_ARCHITECTURE_ALPHA:
			strcpy(szBuffer, "ALPHA");
		  break;

		case PROCESSOR_ARCHITECTURE_PPC:
			strcpy(szBuffer, "PPC");
		  break;

		default:
			strcpy(szBuffer, "Unknown");
		  break;
	} // end switch (on architecture)
	FileSprintfWriteLine(hFile, "\tProcessorArchitecture= %s", 1, szBuffer);

	FileSprintfWriteLine(hFile, "\tNumProcessors= %u", 1, this->m_dwNumberOfProcessors);


	FileSprintfWriteLine(hFile, "\tWinSock= v%u.%u", 2, LOBYTE(this->m_wWinSock),
						HIBYTE(this->m_wWinSock));

	FileSprintfWriteLine(hFile, "\tIPX= %s",
						1, ((this->m_fIPXInstalled) ? "yes" : "no"));

	FileSprintfWriteLine(hFile, "\tIMTest= %s",
						1, ((this->m_fFaultSimIMTestAvailable) ? "yes" : "no"));

	this->m_TAPIdevices.PrintToFile(hFile);

	this->m_COMports.PrintToFile(hFile);

	this->m_soundcards.PrintToFile(hFile);

	this->m_binaries.PrintToFile(hFile);
} // CTNSlaveInfo::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSlaveInfo::UpdateLastReceiveTime()"
//==================================================================================
// CTNSlaveInfo::UpdateLastReceiveTime
//----------------------------------------------------------------------------------
//
// Description: Sets the last time that the slave sent a message to the current time
//				and resets the ping count.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNSlaveInfo::UpdateLastReceiveTime(void)
{
	this->m_dwLastReceiveTime = GetTickCount();
	this->m_dwNumPingsSinceReceive = 0;

	return (S_OK);
} // CTNSlaveInfo::UpdateLastReceiveTime
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSlaveInfo::TimeSinceLastReceive()"
//==================================================================================
// CTNSlaveInfo::TimeSinceLastReceive
//----------------------------------------------------------------------------------
//
// Description: Returns the number of ticks elapsed since the last
//				UpdateLastReceiveTime call for this slave.
//
// Arguments: None.
//
// Returns: Time since last receive.
//==================================================================================
DWORD CTNSlaveInfo::TimeSinceLastReceive(void)
{
	return (GetTickCount() - this->m_dwLastReceiveTime);
} // CTNSlaveInfo::TimeSinceLastReceive
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSlaveInfo::PingsSinceLastReceive()"
//==================================================================================
// CTNSlaveInfo::PingsSinceLastReceive
//----------------------------------------------------------------------------------
//
// Description: Returns the number of pings sent elapsed since the last
//				UpdateLastReceiveTime call for this slave.
//
// Arguments: None.
//
// Returns: Number of pings since last receive.
//==================================================================================
DWORD CTNSlaveInfo::PingsSinceLastReceive(void)
{
	return (this->m_dwNumPingsSinceReceive);
} // CTNSlaveInfo::PingsSinceLastReceive
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
