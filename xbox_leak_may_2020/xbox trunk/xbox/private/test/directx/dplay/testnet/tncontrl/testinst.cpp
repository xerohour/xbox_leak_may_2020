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

#include "testinst.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceM::CTNTestInstanceM()"
//==================================================================================
// CTNTestInstanceM constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTestInstanceM object.  Initializes the data
//				structures.
//
// Arguments:
//	DWORD dwUniqueID				Unique test ID to assign to this new instance.
//	PTNTESTTABLECASE pCase			Pointer to the case this instantiates.
//	char* szInstanceID				String ID of instance.
//	int iNumMachines				Number of testers for this test.
//	DWORD dwOptions					Options for this new instance.
//	PTNTESTINSTANCEM pParentTest	Pointer to parent test for this object.
//
// Returns: None (just the object).
//==================================================================================
CTNTestInstanceM::CTNTestInstanceM(DWORD dwUniqueID,
									PTNTESTFROMFILE pLoadedTest,
									PTNTESTTABLECASE pCase,
									int iNumMachines,
									//DWORD dwOptions,
									PTNTESTINSTANCEM pParentTest)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTestInstanceM));

#ifdef DEBUG
	if (iNumMachines <= 0)
	{
		DPL(0, "Invalid number of testers (%i is not at least 1!)", 1, iNumMachines);
		DEBUGBREAK();
	} // end if (invalid number of testers)
#endif // DEBUG


	this->m_dwUniqueID = dwUniqueID;

	this->m_pLoadedTest = pLoadedTest;
	if (pLoadedTest != NULL)
	{
		// We're using it.
		pLoadedTest->m_dwRefCount++;
	} // end if (there's a loaded test0
	this->m_pCase = pCase;
	this->m_iNumMachines = iNumMachines;
	//this->m_dwOptions = dwOptions;

	this->m_pParentTest = pParentTest;
	if (this->m_pParentTest != NULL)
		this->m_pParentTest->m_dwRefCount++;

	this->m_pSyncDataList = NULL;
	this->m_fStarted = FALSE;
	this->m_paTesterSlots = (PTNTESTERSLOTM) LocalAlloc(LPTR, (iNumMachines * (sizeof (TNTESTERSLOTM))));
	this->m_pTestsToFree = NULL;

	// If the test case can have subtests and it's not going to save results,
	// allocate a list to hold subtests that need to be freed.
	if ((pCase->m_dwOptionFlags & TNTCO_SCENARIO) &&
		(pCase->m_dwOptionFlags & TNTCO_DONTSAVERESULTS))
	{
		this->m_pTestsToFree = new (CTNTestMsList);
	} // end if (scenario and won't save results)
} // CTNTestInstanceM::CTNTestInstanceM
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceM::~CTNTestInstanceM()"
//==================================================================================
// CTNTestInstanceM destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTestInstance object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTestInstanceM::~CTNTestInstanceM(void)
{
	int				i;
	PTNSLAVEINFO	pSlave;
	PTNOUTPUTVAR	pVar;


	//DPL(0, "this = %x", 1, this);

	if (this->m_paTesterSlots != NULL)
	{
		for(i = 0; i < this->m_iNumMachines; i++)
		{
			pSlave = this->m_paTesterSlots[i].pSlave;
			this->m_paTesterSlots[i].pSlave = NULL;

			if (this->m_paTesterSlots[i].pVars != NULL)
			{
				// Remove all the variables
				while (this->m_paTesterSlots[i].pVars->Count() > 0)
				{
					pVar = (PTNOUTPUTVAR) this->m_paTesterSlots[i].pVars->PopFirstItem();
					if (pVar == NULL)
					{
						DPL(0, "Couldn't pop first variable in list!", 0);
						break;
					} // end if (can't pop item)


					pVar->m_dwRefCount--;
					if (pVar->m_dwRefCount == 0)
					{
						delete (pVar);
						pVar = NULL;
					} // end if (variable can be deleted)
					else
					{
						DPL(0, "WARNING: Can't delete variable %x, refcount is still %u!?",
							2, pVar, pVar->m_dwRefCount);
					} // end else (there isn't a valid slave)
				} // end for (each variable)

				// Delete the actual vars list
				delete (this->m_paTesterSlots[i].pVars);
				this->m_paTesterSlots[i].pVars = NULL;
			} // end if (there's a variable list)

			// Delete output data, if any 
			this->m_paTesterSlots[i].dwOutputDataSize = 0;
			if (this->m_paTesterSlots[i].pvOutputData != NULL)
			{
				LocalFree(this->m_paTesterSlots[i].pvOutputData);
				this->m_paTesterSlots[i].pvOutputData = NULL;
			} // end if (there's output data)


			if (pSlave != NULL)
			{
				pSlave->m_dwRefCount--;
				if (pSlave->m_dwRefCount == 0)
				{
					DPL(0, "Deleting slave %x.", 1, pSlave);
					delete (pSlave);
					pSlave = NULL;
				} // end if (that was the last reference to the slave)
			} // end if (there's an item there)
		} // end for (each item in the array)

		LocalFree(this->m_paTesterSlots);
		this->m_paTesterSlots = NULL;
	} // end if (the testing slots array exists)

	if (this->m_pLoadedTest != NULL)
	{
		this->m_pLoadedTest->m_dwRefCount--;
		if (this->m_pLoadedTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting loaded test %x.", 1, this->m_pLoadedTest);
			delete (this->m_pLoadedTest);
		} // end if (last reference to loaded test)
		else
		{
			DPL(7, "Not deleting loaded test %x, its refcount is %u.",
				2, this->m_pLoadedTest, this->m_pLoadedTest->m_dwRefCount);
		} // end else (not last reference to loaded test)

		this->m_pLoadedTest = NULL;
	} // end if (there's a loaded test)

	if (this->m_pSyncDataList != NULL)
	{
		delete (this->m_pSyncDataList);
		this->m_pSyncDataList = NULL;
	} // end if (there's sync data)

	if (this->m_pTestsToFree != NULL)
	{
		delete (this->m_pTestsToFree);
		this->m_pTestsToFree = NULL;
	} // end if (there's a tests to free list)

	if (this->m_pParentTest != NULL)
	{
		this->m_pParentTest->m_dwRefCount--;
		if (this->m_pParentTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting parent test %x.", 1, this->m_pParentTest);
			delete (this->m_pParentTest);
		} // end if (should delete test)
		else
		{
			DPL(7, "Not deleting parent test %x, it's refcount is %u.",
				2, this->m_pParentTest, this->m_pParentTest->m_dwRefCount);
		} // end else (should not delete test)
	} // end if (there's a parent test)
} // CTNTestInstanceM::~CTNTestInstanceM
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceM::GetTopLevelTest()"
//==================================================================================
// CTNTestInstanceM::GetTopLevelTest
//----------------------------------------------------------------------------------
//
// Description: Returns the highest test in the heirarchy (parent test without a
//				parent of its own).
//
// Arguments: None.
//
// Returns: Pointer to top level test.
//==================================================================================
PTNTESTINSTANCEM CTNTestInstanceM::GetTopLevelTest(void)
{
	if (this->m_pParentTest == NULL)
		return (this);

	return (this->m_pParentTest->GetTopLevelTest());
} // CTNTestInstance::GetTopLevelTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceM::GetTest()"
//==================================================================================
// CTNTestInstanceM::GetTest
//----------------------------------------------------------------------------------
//
// Description: Returns this test or the subtest with the given unique test ID, or
//				NULL if none.
//
// Arguments:
//	DWORD dwUniqueID	Unique ID of test to look up.
//
// Returns: Pointer to test with ID, or NULL if couldn't find.
//==================================================================================
PTNTESTINSTANCEM CTNTestInstanceM::GetTest(DWORD dwUniqueID)
{
	int					i;
	PTNTESTINSTANCEM	pSubTest;


	if (this->m_dwUniqueID == dwUniqueID)
		return (this);

	// Recursively check subtests too.
	for(i = 0; i < this->m_subtests.Count(); i++)
	{
		pSubTest = (PTNTESTINSTANCEM) this->m_subtests.GetItem(i);
		if (pSubTest == NULL)
		{
			DPL(0, "Couldn't get subtest %i!", 1, i);
			return (NULL);
		} // end if (couldn't get item)

		pSubTest = pSubTest->GetTest(dwUniqueID);
		if (pSubTest != NULL)
			return (pSubTest);
	} // end for (each subtest)

	// It's not us, so we don't know what you're talking about.  Go away.
	return (NULL);
} // CTNTestInstanceM::GetTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceM::GetSlavesTesterNum()"
//==================================================================================
// CTNTestInstanceM::GetSlavesTesterNum
//----------------------------------------------------------------------------------
//
// Description: Returns the given slave's position in the tester list, or -1 if not
//				found or an error occurred.
//
// Arguments:
//	PTNSLAVEINFO pSlave		Pointer to slave to look up.
//
// Returns: Index of slave in tester list, or -1 if not found.
//==================================================================================
int CTNTestInstanceM::GetSlavesTesterNum(PTNSLAVEINFO pSlave)
{
	int		iIndex;


	// Loop through the list of testers until we find it.
	for (iIndex = 0; iIndex < this->m_iNumMachines; iIndex++)
	{
		// If we found it, return what we have now
		if (this->m_paTesterSlots[iIndex].pSlave == pSlave)
			return (iIndex);
	} // end for (each available slot in the list)

	// We didn't find it, so return that error.
	return (-1);
} // CTNTestInstanceM::GetSlavesTesterNum
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#ifndef _XBOX // no file printing supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceM::PrintToFile()"
//==================================================================================
// CTNTestInstanceM::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the information pertaining to this test instance into the
//				passed in file.
//
// Arguments:
//	HANDLE hFile			File to print to.
//	BOOL fSeparateSection	Should it be printed as a separate section or not.
//	BOOL fSubTest			Whether this is a subtest or not.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestInstanceM::PrintToFile(HANDLE hFile, BOOL fSeparateSection,
									BOOL fSubTest)
{
	if (fSeparateSection)
	{
		//Ignoring errors
		FileSprintfWriteLine(hFile, "{%i}", 1, this->m_dwUniqueID);
	} // end if (a separate section)
	else
	{
		FileSprintfWriteLine(hFile, "UniqueTestID= %u", 1, this->m_dwUniqueID);
	} // end if (not a separate section)


	if (fSeparateSection)
		FileWriteString(hFile, "\t"); // indent

	FileSprintfWriteLine(hFile, "ModuleCaseID= %s",
						1, this->m_pCase->m_pszID);


	if ((this->m_pLoadedTest != NULL) &&
		(this->m_pLoadedTest->m_pszInstanceID != NULL))
	{
		if (fSeparateSection)
			FileWriteString(hFile, "\t"); // indent

		FileSprintfWriteLine(hFile, "TestInstanceID= %s",
							1, this->m_pLoadedTest->m_pszInstanceID);
	} // end if (there's an instance ID)


	if (fSeparateSection)
		FileWriteString(hFile, "\t"); // indent

	FileSprintfWriteLine(hFile, "CaseName= %s",
						1, this->m_pCase->m_pszName);


	if ((this->m_pLoadedTest != NULL) &&
		(this->m_pLoadedTest->m_pszFriendlyName != NULL))
	{
		if (fSeparateSection)
			FileWriteString(hFile, "\t"); // indent

		FileSprintfWriteLine(hFile, "FriendlyName= %s",
							1, this->m_pLoadedTest->m_pszFriendlyName);
	} // end if (there's a friendly name)


	if (fSeparateSection)
		FileWriteString(hFile, "\t"); // indent

	FileSprintfWriteLine(hFile, "MachinesRequired= %i",
						1, this->m_iNumMachines);


	if (fSeparateSection)
		FileWriteString(hFile, "\t"); // indent

	FileSprintfWriteLine(hFile, "Subtest= %B", 1, fSubTest);


	if (fSeparateSection)
		FileWriteString(hFile, "\t"); // indent

	FileSprintfWriteLine(hFile, "Ongoing= %s",
						1, ((this->m_pCase->m_dwOptionFlags & TNTCO_ONGOING) ? "TRUE" : "FALSE"));

	if (this->m_pLoadedTest != NULL)
	{
		if (fSeparateSection)
			FileWriteString(hFile, "\t"); // indent

		FileWriteString(hFile, "RepsRemaining= ");

		if (this->m_pLoadedTest->m_dwRepsRemaining == 0)
		{
			FileWriteLine(hFile, "INFINITE");
		} // end if (INFINITE)
		else
		{
			FileSprintfWriteLine(hFile, "%u",
								1, this->m_pLoadedTest->m_dwRepsRemaining);
		} // end else (not INFINITE)


		if (fSeparateSection)
			FileWriteString(hFile, "\t"); // indent

		FileWriteString(hFile, "PermutationMode= ");

		switch (this->m_pLoadedTest->m_dwPermutationMode)
		{
			case TNTPM_ONCE:
				FileWriteLine(hFile, "Once");
			  break;

			case TNTPM_ALLSLAVES:
				FileWriteLine(hFile, "AllSlaves");
			  break;

			case TNTPM_ALLSLAVESASTESTERS0ANDNON0:
				FileWriteLine(hFile, "AllSlavesAsTesters0AndNon0");
			  break;

			case TNTPM_ALLSLAVESASALLTESTERS:
				FileWriteLine(hFile, "AllSlavesAsAllTesters");
			  break;

			case TNTPM_ALLPOSSIBLE:
				FileWriteLine(hFile, "AllPossible");
			  break;
		} // end switch (on the permutation mode)
	} // end if (there's a loaded test)

	return (S_OK);
} // CTNTestInstanceM::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceS::CTNTestInstanceS()"
//==================================================================================
// CTNTestInstanceS constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTestInstanceS object.  Initializes the data
//				structures.
//
// Arguments:
//	DWORD dwUniqueID				Unique test ID to assign to this new instance.
//	PTNTESTTABLECASE pCase			Pointer to the case this instantiates.
//	char* pszInstanceID				String ID of instance.
//	int iNumMachines				Number of testers for this instance
//	PTNCTRLMACHINEID aTesters		Array of testers for this instance.
//	DWORD dwOptions					Options for this new instance.
//	PTNTESTINSTANCES pParentTest	Pointer to parent test for this object.
//
// Returns: None (just the object).
//==================================================================================
CTNTestInstanceS::CTNTestInstanceS(DWORD dwUniqueID,
								PTNTESTTABLECASE pCase,
								char* pszInstanceID,
								int iNumMachines,
								PTNCTRLMACHINEID aTesters,
								//DWORD dwOptions,
								PTNTESTINSTANCES pParentTest)
{
	int		i;


	DPL(9, "this = %x, sizeof (this) = %i",
		2, this, sizeof (CTNTestInstanceS));


#ifdef DEBUG
	if (iNumMachines != -666)
	{
		if (iNumMachines <= 0)
		{
			DPL(0, "Invalid number of testers (%i is not at least 1)!", 1, iNumMachines);
			DEBUGBREAK();
		} // end if (invalid number of testers)

		if (aTesters == NULL)
		{
			DPL(0, "NULL array of testers!", 0);
			DEBUGBREAK();
		} // end if (invalid testers array)
	} // end if (not documentation test case)
#endif // DEBUG


	this->m_dwUniqueID = dwUniqueID;
	this->m_pCase = pCase;

	if (pszInstanceID != NULL)
	{
		this->m_pszInstanceID = (char*) LocalAlloc(LPTR, (strlen(pszInstanceID) + 1));
		if (this->m_pszInstanceID != NULL)
			strcpy(this->m_pszInstanceID, pszInstanceID);
	} // end if (there's an instance ID)

	this->m_iNumMachines = iNumMachines;
	//this->m_dwOptions = dwOptions;
	this->m_pParentTest = pParentTest;
	if (this->m_pParentTest != NULL)
		this->m_pParentTest->m_dwRefCount++;

	this->m_pSubTest = NULL;

	// Don't try to use the value if it's a documentation test instance.
	if (iNumMachines != -666)
	{
		this->m_paTesterSlots = (PTNTESTERSLOTS) LocalAlloc(LPTR, iNumMachines * sizeof (TNTESTERSLOTS));
		if (this->m_paTesterSlots != NULL)
		{
			for(i = 0; i < iNumMachines; i++)
			{
				CopyMemory(&(this->m_paTesterSlots[i].id), &(aTesters[i]),
							sizeof (TNCTRLMACHINEID));
			} // end for (each tester)
		} // end if (allocated array)
	} // end if (not documentation test case)

	this->m_pResultsToFree = NULL;

	// If the test case can have subtests and it's not going to save results,
	// allocate a list to hold results from subtests that need to be freed.
	if ((pCase->m_dwOptionFlags & TNTCO_SCENARIO) &&
		(pCase->m_dwOptionFlags & TNTCO_DONTSAVERESULTS))
	{
		this->m_pResultsToFree = new (CTNResultsChain);
	} // end if (scenario and won't save results)


	this->m_pvInputData = NULL;
	this->m_dwInputDataSize = 0;
	InitializeCriticalSection(&this->m_csMasterOp);
	this->m_dwRequestID = 0;
	this->m_hResponseEvent = NULL;
	this->m_hrResponseResult = E_FAIL;
	this->m_pSyncDataList = NULL;
	this->m_pszFoundPhoneNumber = NULL;
	this->m_dwFoundCOMPort = 0;
	this->m_pReturnedInfo = NULL;
	this->m_hExecCaseExitEvent = NULL;
	this->m_fComplete = FALSE;
	this->m_pfnPokeTestCompleted = NULL;
	this->m_pvUserContext = NULL;
} // CTNTestInstanceS::CTNTestInstanceS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceS::~CTNTestInstanceS()"
//==================================================================================
// CTNTestInstanceS destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTestInstanceS object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTestInstanceS::~CTNTestInstanceS(void)
{
	DPL(9, "this = %x", 1, this);


	if (this->m_pszInstanceID != NULL)
	{
		LocalFree(this->m_pszInstanceID);
		this->m_pszInstanceID = NULL;
	} // end if (allocated a sub ID)

	if (this->m_paTesterSlots != NULL)
	{
		LocalFree(this->m_paTesterSlots);
		this->m_paTesterSlots = NULL;
	} // end if (have tester array)

	if (this->m_pResultsToFree != NULL)
	{
		delete (this->m_pResultsToFree);
		this->m_pResultsToFree = NULL;
	} // end if (have results chain object)

	this->FreeInputData();

	if (this->m_pszFoundPhoneNumber != NULL)
	{
		LocalFree(this->m_pszFoundPhoneNumber);
		this->m_pszFoundPhoneNumber = NULL;
	} // end if (have phone number string)

	if (this->m_hResponseEvent != NULL)
	{
		CloseHandle(this->m_hResponseEvent);
		this->m_hResponseEvent = NULL;
	} // end if (have event handle)

	if (this->m_hExecCaseExitEvent != NULL)
	{
		CloseHandle(this->m_hExecCaseExitEvent);
		this->m_hExecCaseExitEvent = NULL;
	} // end if (have event handle)

	if (this->m_pParentTest != NULL)
	{
		this->m_pParentTest->m_dwRefCount--;
		if (this->m_pParentTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting parent test %x.", 1, this->m_pParentTest);
			delete (this->m_pParentTest);
		} // end if (should delete test)
		else
		{
			DPL(7, "Not deleting parent test %x, it's refcount is %u.",
				2, this->m_pParentTest, this->m_pParentTest->m_dwRefCount);
		} // end else (should not delete test)
	} // end if (there's a parent test)

	if (this->m_pSubTest != NULL)
	{
		/*
		DPL(0, "NOTE: Subtest %x (ID %u) exists.",
			2, this->m_pSubTest, this->m_pSubTest->m_dwUniqueID);
		*/

		if (this->m_pSubTest->m_dwRefCount == 0)
		{
			DPL(0, "Deleting subtest %x.", 1, this->m_pSubTest);
			delete (this->m_pSubTest);
		} // end if (should delete subtest)
		else
		{
			DPL(0, "Not deleting subtest %x, its refcount is %u.",
				2, this->m_pSubTest, this->m_pSubTest->m_dwRefCount);
		} // end else (can't delete subtest)

		this->m_pSubTest = NULL;
	} // end if (there's a subtest)

	if (this->m_pReturnedInfo != NULL)
	{
		if (this->m_pReturnedInfo->m_dwRefCount == 0)
		{
			DPL(0, "Deleting returned info %x.", 1, this->m_pReturnedInfo);
			delete (this->m_pReturnedInfo);
		} // end if (should delete subtest)
		else
		{
			DPL(0, "Not deleting returned info %x, its refcount is %u.",
				2, this->m_pReturnedInfo, this->m_pReturnedInfo->m_dwRefCount);
		} // end else (can't delete subtest)

		this->m_pReturnedInfo = NULL;
	} // end if (there's a subtest)

	DeleteCriticalSection(&this->m_csMasterOp);
} // CTNTestInstanceS::~CTNTestInstanceS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceS::GetTopLevelTest()"
//==================================================================================
// CTNTestInstanceS::GetTopLevelTest
//----------------------------------------------------------------------------------
//
// Description: Returns the highest test in the heirarchy (parent test without a
//				parent of its own).
//
// Arguments: None.
//
// Returns: Pointer to top level test.
//==================================================================================
PTNTESTINSTANCES CTNTestInstanceS::GetTopLevelTest(void)
{
	if (this->m_pParentTest == NULL)
		return (this);

	return (this->m_pParentTest->GetTopLevelTest());
} // CTNTestInstance::GetTopLevelTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceS::GetTest()"
//==================================================================================
// CTNTestInstanceS::GetTest
//----------------------------------------------------------------------------------
//
// Description: Returns this test or the subtest with the given unique test ID, or
//				NULL if none.
//
// Arguments:
//	DWORD dwUniqueID	Unique ID of test to look up.
//
// Returns: Pointer to test with ID, or NULL if couldn't find.
//==================================================================================
PTNTESTINSTANCES CTNTestInstanceS::GetTest(DWORD dwUniqueID)
{
	if (this->m_dwUniqueID == dwUniqueID)
		return (this);

	// If there's a subtest, recursively check it too.
	if (this->m_pSubTest != NULL)
		return (((PTNTESTINSTANCES) (this->m_pSubTest))->GetTest(dwUniqueID));

	// It's not us, so we don't know what you're talking about.  Go away.
	return (NULL);
} // CTNTestInstanceS::GetTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceS::GetTestWithRequestID()"
//==================================================================================
// CTNTestInstanceS::GetTestWithRequestID
//----------------------------------------------------------------------------------
//
// Description: Returns this test or the subtest that was waiting on the master with
//				the given request ID, or NULL if none.
//
// Arguments:
//	DWORD dwRequestID	Request ID to look up.
//
// Returns: Pointer to test that announce with ID, or NULL if couldn't find.
//==================================================================================
PTNTESTINSTANCES CTNTestInstanceS::GetTestWithRequestID(DWORD dwRequestID)
{
	if (this->m_dwRequestID == dwRequestID)
		return (this);

	// If there's a subtest, recursively check it too.
	if (this->m_pSubTest != NULL)
		return (((PTNTESTINSTANCES) (this->m_pSubTest))->GetTestWithRequestID(dwRequestID));

	// It's not us, so we don't know what you're talking about.  Go away.
	return (NULL);
} // CTNTestInstanceS::GetTestWithRequestID
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceS::GetSlavesTesterNum()"
//==================================================================================
// CTNTestInstanceS::GetSlavesTesterNum
//----------------------------------------------------------------------------------
//
// Description: Returns the given slave's position in the tester list, or -1 if not
//				found or an error occurred.
//
// Arguments:
//	PTNCTRLMACHINEID pSlaveID	Pointer to ID of slave to look up.
//
// Returns: Index of slave in tester list, or -1 if not found.
//==================================================================================
int CTNTestInstanceS::GetSlavesTesterNum(PTNCTRLMACHINEID pSlaveID)
{
	int		iIndex;


	// Loop through the list of testers until we find it.
	for (iIndex = 0; iIndex < this->m_iNumMachines; iIndex++)
	{
		// If we found it, return what we have now
		if (memcmp(&(this->m_paTesterSlots[iIndex].id), pSlaveID, sizeof (TNCTRLMACHINEID)) == 0)
			return (iIndex);
	} // end for (each available slot in the list)

	// We didn't find it, so return that error.
	return (-1);
} // CTNTestInstanceS::GetSlavesTesterNum
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceS::SetInputData()"
//==================================================================================
// CTNTestInstanceS::SetInputData
//----------------------------------------------------------------------------------
//
// Description: Set the input data for this test to be a copy of the data passed
//				in.  If any input data existed before, it is freed before copying.
//
// Arguments:
//	PVOID pvData		Pointer to input data buffer to copy
//	DWORD dwDataSize	Size of the input data buffer.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestInstanceS::SetInputData(PVOID pvData, DWORD dwDataSize)
{
	this->FreeInputData();

	this->m_pvInputData = LocalAlloc(LPTR, dwDataSize);

	if (this->m_pvInputData == NULL)
		return (E_OUTOFMEMORY);

	CopyMemory(this->m_pvInputData, pvData, dwDataSize);

	this->m_dwInputDataSize = dwDataSize;

	return (S_OK);
} // CTNTestInstanceS::SetInputData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestInstanceS::FreeInputData()"
//==================================================================================
// CTNTestInstance::FreeInputData
//----------------------------------------------------------------------------------
//
// Description: Frees the input data, if any existed.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void CTNTestInstanceS::FreeInputData(void)
{
	if (this->m_pvInputData != NULL)
	{
		LocalFree(this->m_pvInputData);
		this->m_pvInputData = NULL;
	} // end if (any input data existed)

	this->m_dwInputDataSize = 0;
} // CTNTestInstanceS::FreeInputData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

