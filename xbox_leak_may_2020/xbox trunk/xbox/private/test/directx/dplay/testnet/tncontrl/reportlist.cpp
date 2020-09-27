#ifndef _XBOX // reports not supported
//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <time.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"

#include "tncontrl.h"
#include "main.h"
#include "reportitem.h"
#include "slaveinfo.h"

#include "reportlist.h"








#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReportsList::CTNReportsList()"
//==================================================================================
// CTNReportsList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNReportsList object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNReportsList::CTNReportsList(void):
	m_iMaxNumEntries(DEFAULT_MAX_NUM_REPORTS)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNReportsList));
} // CTNReportsList::CTNReportsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReportsList::~CTNReportsList()"
//==================================================================================
// CTNReportsList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNReportsList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNReportsList::~CTNReportsList(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNReportsList::~CTNReportsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReportsList::AddReport()"
//==================================================================================
// CTNReportsList::AddReport
//----------------------------------------------------------------------------------
//
// Description: Adds a new ReportItem object with the given values to this list.
//				The array/lists or the single items can be NULL to not use the
//				parameter.
//				If both the testlist and single test are specified, then the single
//				test is added at the end of the list.
//
// Arguments:
//	DWORD dwType				What kind of report this is.
//	int iNumMachines			The number of machines this report pertains to
//								(number of entries in the following array).
//	PTNSLAVEINFO* apMachines	Array of pointers to machines that this report
//								pertains to (or NULL for none).
//	PTNTESTINSTANCEM pTest		Pointer to the test that this report pertains to (or
//								NULL for none).
//	HRESULT hresult				Result code for report.
//	PVOID pvData				User specific data.
//	DWORD dwDataSize			Size of user data.
//
// Returns: S_OK if succesful, error code otherwise.
//==================================================================================
HRESULT CTNReportsList::AddReport(DWORD dwType, int iNumMachines, PTNSLAVEINFO* apMachines,
							PTNTESTINSTANCEM pTest, HRESULT hresult,
							PVOID pvData, DWORD dwDataSize)
{
	HRESULT				hr;
	PTNREPORT			pReport = NULL;
	int					i;
	PTNSLAVEINFO		pSlave = NULL;
	time_t 				ctimet;
	tm*					pCurrentTime;


	DPL(6, "Adding report type %u, count will probably be %i.", 2, dwType, (this->Count() + 1));

	pReport = new (CTNReport);
	if (pReport == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	if (apMachines != NULL)
	{
		// Loop through and copy all of the machines in the list passed in
		// to the report's internal list.
		for(i = 0; i < iNumMachines; i++)
		{
			pSlave = apMachines[i];

			if (pSlave != NULL)
			{
				hr = pReport->m_machines.Add(pSlave);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't add machine %i (%s) to list!",
						2, i, pSlave->m_szComputerName);
					goto DONE;
				} // end if (couldn't add that item)

			} // end if (it's a valid slave)
			else
			{
				DPL(0, "WARNING: Ignoring NULL slave in machine list.", 0);
			} // end else (it's not a valid slave)

		} // end for (each machine to add)

	} // end if (we were given a list of machines)

	if (pTest != NULL)
	{
		pTest->m_dwRefCount++;
		pReport->m_pTest = pTest;
	} // end if (we were given a single test)

	time(&ctimet);
	pCurrentTime = localtime(&ctimet);
	CopyMemory(&(pReport->m_time), pCurrentTime, sizeof (tm));

	pReport->m_dwType = dwType;
	pReport->m_hresult = hresult;
	pReport->m_dwDataSize = 0;
	if ((pvData != NULL) && (dwDataSize > 0))
	{
		pReport->m_dwDataSize = dwDataSize;
		pReport->m_pvData = LocalAlloc(LPTR, dwDataSize);
		if (pReport->m_pvData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
		
		CopyMemory(pReport->m_pvData, pvData, dwDataSize);
	} // end if (there's actually data to add)


	this->EnterCritSection();

	// Make sure we don't go over our limit for number of entries.
	if (this->Count() >= this->m_iMaxNumEntries)
	{
		// Remove the last report in the list.
		hr = this->Remove(this->Count() - 1);
		if (hr != S_OK)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't remove last report in list!", 0);
			goto DONE;
		} // end if (couldn't 
	} // end if (already at limit of entries)

	hr = this->Add(pReport);
	if (hr != S_OK)
	{
		DPL(0, "Adding report item to list failed!", 0);

		// note continuing because that gets us to DONE anyway
	} // end if (add failed)

	this->LeaveCritSection();

	pReport = NULL; // forget about it so we don't free it below


DONE:

	if (pReport != NULL)
	{
		delete (pReport);
		pReport = NULL;
	} // end if (still have report)

	return (hr);
} // CTNReportsList::AddReport
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReportsList::GetNextMatchingReport()"
//==================================================================================
// CTNReportsList::GetNextMatchingReport
//----------------------------------------------------------------------------------
//
// Description: Searches from a given item in the list for the next item that
//				matches the criteria passed in.  If pStartFromItem is NULL, the
//				searching will start at the beginning of the list.  If the lists
//				are not NULL and contains items, then only reports pertaining
//				to those items will be returned.
//
// Arguments:
//	PTNREPORT pStartFromItem		Pointer to object to start looking from.
//	DWORD dwMatchFlags				Criteria to compare an item against.
//	PTNSLAVESLIST pMachineList		Machines to report on.
//	PTNTESTMSLIST pTestList			Tests to report on.
//	BOOL fRemoveStartFromItem		Whether to pull pStartFromItem from the list and
//									delete it.  Ignored if pStartFromItem is NULL.
//
// Returns: Pointer to object or NULL if none found.
//==================================================================================
PTNREPORT CTNReportsList::GetNextMatchingReport(PTNREPORT pStartFromItem,
												DWORD dwMatchFlags,
												PTNSLAVESLIST pMachineList,
												PTNTESTMSLIST pTestList,
												BOOL fRemoveStartFromItem)
{
	HRESULT				hr;
	PTNREPORT			pReport = NULL;
	BOOL				fMatchedType;
	BOOL				fMatchedMachine;
	BOOL				fMatchedTest;
	int					i;
	PTNSLAVEINFO		pSlave = NULL;
	BOOL				fComplete;
	BOOL				fSuccess;
	int					iNumWarnings;


	if ((dwMatchFlags == 0) || (dwMatchFlags == TNREPORT_SUBTESTS))
	{
		DPL(0, "Given invalid match flags (i.e. none or only TNREPORT_SUBTESTS)!", 0);
		return (NULL);
	} // end if (no flags specified)

	this->EnterCritSection();

	if (pStartFromItem == NULL)
		pReport = (PTNREPORT) this->GetItem(0);
	else
	{
		pReport = (PTNREPORT) this->GetNextItem(pStartFromItem);

		if (fRemoveStartFromItem)
		{
			hr = this->RemoveFirstReference(pStartFromItem);
			if (hr != S_OK)
			{
				this->LeaveCritSection();
				DPL(0, "Couldn't remove first reference to starting item %x!  %e",
					2, pStartFromItem, hr);
				return (NULL);
			} // end if (couldn't get next item)

			if (pStartFromItem->m_dwRefCount == 0)
			{
				delete (pStartFromItem);
				pStartFromItem = NULL;
			} // end if (can delete item)
			else
			{
				DPL(0, "Can't delete report item %x because it's refcount is %u!?",
					2, pStartFromItem, pStartFromItem->m_dwRefCount);
			} // end else (can't delete item)
		} // end if (should remove starting point item)
	} // end else (there's a starting item)

	while (pReport != NULL)
	{
		fMatchedType = FALSE;
		fMatchedMachine = FALSE;
		fMatchedTest = FALSE;
		fComplete = FALSE;
		fSuccess = FALSE;
		iNumWarnings = 0;


#pragma BUGBUG(vanceo, "FilterSuccess may override this")
		if ((pReport->m_pTest != NULL) &&
			(pReport->m_pTest->m_fStarted))
		{
			fComplete = TRUE;
			fSuccess = TRUE;

			for(i = 0; i < pReport->m_pTest->m_iNumMachines; i++)
			{
				if (pReport->m_pTest->m_paTesterSlots[i].fComplete)
				{
					if (! pReport->m_pTest->m_paTesterSlots[i].fSuccess)
						fSuccess = FALSE;
				} // end if (this tester completed)
				else
				{
					fComplete = FALSE;
				} // end else (this tester has not completed)

				iNumWarnings += pReport->m_pTest->m_paTesterSlots[i].iNumWarnings;
			} // end for (each tester slot)
		} // end if (there's a test and it has started)


		if ((dwMatchFlags & TNREPORT_NONTESTREPORTS) &&
			((pReport->m_dwType == RT_ADDMACHINE) ||
			(pReport->m_dwType == RT_REMOVEMACHINE) ||
			(pReport->m_dwType == RT_CLOSEDSESSION) ||
			(pReport->m_dwType == RT_ALLTESTINGCOMPLETE)))
		{
			fMatchedType = TRUE;
		} // end if (we're checking for non test reports)
		

		if ((dwMatchFlags & (TNREPORT_TESTASSIGNMENTSTESTNOTCOMPLETE |
							TNREPORT_TESTASSIGNMENTSTESTSUCCEEDED |
							TNREPORT_TESTASSIGNMENTSTESTFAILED |
							TNREPORT_TESTASSIGNMENTSTESTWARNED)) &&
			((pReport->m_dwType == RT_TESTASSIGNED) ||
			((pReport->m_dwType == RT_TESTASSIGNED_SUBTEST) &&
				(dwMatchFlags & TNREPORT_SUBTESTS))))
		{
			// They want to see some kind of test assignment, and this is one.  We
			// need to make sure it fits the specific kind they want.

			if ((dwMatchFlags & TNREPORT_TESTASSIGNMENTSTESTNOTCOMPLETE) &&
				(! fComplete))
			{
				fMatchedType = TRUE;
			} // end if (match not completed)

			if ((dwMatchFlags & TNREPORT_TESTASSIGNMENTSTESTSUCCEEDED) &&
				(fComplete) && (fSuccess))
			{
				fMatchedType = TRUE;
			} // end if (match succeeded)

			if ((dwMatchFlags & TNREPORT_TESTASSIGNMENTSTESTFAILED) &&
				(fComplete) && (! fSuccess))
			{
				fMatchedType = TRUE;
			} // end if (match failed)

			if ((dwMatchFlags & TNREPORT_TESTASSIGNMENTSTESTWARNED) &&
				(iNumWarnings > 0))
			{
				fMatchedType = TRUE;
			} // end if (match warned)
		} // end if (we're checking for test assignments)


		// This is not an else-if because it's possible to be in the above block but
		// not set fMatchedType to TRUE.
		if ((dwMatchFlags & (TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTNOTCOMPLETE |
							TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTSUCCEEDED |
							TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTFAILED |
							TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTWARNED)) &&
			((pReport->m_dwType == RT_TESTCOMPLETELYASSIGNED) ||
			((pReport->m_dwType == RT_TESTCOMPLETELYASSIGNED_SUBTEST) &&
				(dwMatchFlags & TNREPORT_SUBTESTS))))
		{
			// They want to see some kind of test assignment, and this is one.  We
			// need to make sure it fits the specific kind they want.

			if ((dwMatchFlags & TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTNOTCOMPLETE) &&
				(! fComplete))
			{
				fMatchedType = TRUE;
			} // end if (match not completed)

			if ((dwMatchFlags & TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTSUCCEEDED) &&
				(fComplete) && (fSuccess))
			{
				fMatchedType = TRUE;
			} // end if (match succeeded)

			if ((dwMatchFlags & TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTFAILED) &&
				(fComplete) && (! fSuccess))
			{
				fMatchedType = TRUE;
			} // end if (match failed)

			if ((dwMatchFlags & TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTWARNED) &&
				(iNumWarnings > 0))
			{
				fMatchedType = TRUE;
			} // end if (match warned)
		} // end if (we're checking for test assignment completions)

		
		// This is not an else-if because it's possible to be in the above block but
		// not set fMatchedType to TRUE.
		if ((dwMatchFlags & TNREPORT_TESTSUCCESSES) &&
			((pReport->m_dwType == RT_SUCCESSCOMPLETE) ||
			((pReport->m_dwType == RT_SUCCESSCOMPLETE_SUBTEST) &&
				(dwMatchFlags & TNREPORT_SUBTESTS))))
		{
			fMatchedType = TRUE;
		} // end if (we're checking for test successes)
		
		if ((dwMatchFlags & TNREPORT_TESTFAILURES) &&
			((pReport->m_dwType == RT_FAILURECOMPLETE) ||
			((pReport->m_dwType == RT_FAILURECOMPLETE_SUBTEST) &&
				(dwMatchFlags & TNREPORT_SUBTESTS))))
		{
			fMatchedType = TRUE;
		} // end if (we're checking for test successes)
		
		if ((dwMatchFlags & TNREPORT_DURINGTESTSUCCESSES) &&
			((pReport->m_dwType == RT_SUCCESS) ||
			((pReport->m_dwType == RT_SUCCESS_SUBTEST) &&
				(dwMatchFlags & TNREPORT_SUBTESTS))))
		{
			fMatchedType = TRUE;
		} // end if (we're checking for test successes)
		
		if ((dwMatchFlags & TNREPORT_DURINGTESTFAILURES) &&
			((pReport->m_dwType == RT_FAILURE) ||
			((pReport->m_dwType == RT_FAILURE_SUBTEST) &&
				(dwMatchFlags & TNREPORT_SUBTESTS))))
		{
			fMatchedType = TRUE;
		} // end if (we're checking for test successes)
		
		if ((dwMatchFlags & TNREPORT_DURINGTESTWARNINGS) &&
			((pReport->m_dwType == RT_WARNING) ||
			((pReport->m_dwType == RT_WARNING_SUBTEST) &&
				(dwMatchFlags & TNREPORT_SUBTESTS))))
		{
			fMatchedType = TRUE;
		} // end if (we're checking for test successes)

		if (fMatchedType) // don't bother checking if it's not the right type
		{
			// If we don't have to match any machines, then we're done.  Otherwise,
			// we have to check to see if any of the machines we need to match
			// appear in the list of machines this report pertains to.
			if ((pMachineList == NULL) || (pMachineList->Count() <= 0))
			{
				fMatchedMachine = TRUE;
			} // end if (we don't have to match machines)
			else
			{
				pMachineList->EnterCritSection();
				for(i = 0; i < pMachineList->Count(); i++)
				{
					pSlave = (PTNSLAVEINFO) pMachineList->GetItem(i);
					if (pReport->m_machines.GetFirstIndex(pSlave) >= 0)
					{
						fMatchedMachine = TRUE;
						break; // get out of the for loop
					} // end if (we found this machine in the list)
				} // end for (each machine to compare)
				pMachineList->LeaveCritSection();
			} // end if (we're matching a list of machines)

			// If we don't have to match any tests, then we're done.  Otherwise,
			// we have to check to see if any of the tests we need to match
			// appear in the list of test this report pertains to.
			if ((pTestList == NULL) || (pTestList->Count() <= 0))
			{
				fMatchedTest = TRUE;
			} // end if (we don't have to match machines)
			else
			{
				if ((pReport->m_pTest != NULL) &&
					(pTestList->GetFirstIndex(pReport->m_pTest) >= 0))
				{
					fMatchedTest = TRUE;
				} // end if (report uses test from list)
			} // end if (we're matching a list of machines)
		} // end if (we matched the right report type)

		if ((fMatchedType) && (fMatchedMachine) && (fMatchedTest))
			break; // found a match, get out of the while loop

		pReport = (PTNREPORT) this->GetNextItem(pReport);
	} // end while (we have a valid report to check)

	this->LeaveCritSection();

	return (pReport);
} // CTNReportsList::GetNextMatchingReport
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReportsList::SetMaxNumberReports()"
//==================================================================================
// CTNReportsList::SetMaxNumberReports
//----------------------------------------------------------------------------------
//
// Description: Sets the maximum number of reports this list will store before
//				throwing out old reports to make room for new ones.
//
// Arguments:
//	int iNumReports		How many reports to keep.
//
// Returns: None.
//==================================================================================
void CTNReportsList::SetMaxNumberReports(int iNumReports)
{
	this->EnterCritSection();
	this->m_iMaxNumEntries = iNumReports;
	this->LeaveCritSection();
} // CTNReportsList::SetMaxNumberReports
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX