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
#include "..\tncommon\errors.h"
#include "..\tncommon\sprintf.h"

#include "tncontrl.h"
#include "main.h"
#include "slaveinfo.h"

#include "reportitem.h"







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReport::CTNReport()"
//==================================================================================
// CTNReport constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNReport object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNReport::CTNReport(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNReport));

	this->m_dwType = 0;
	ZeroMemory(&(this->m_time), sizeof (tm));
	this->m_pTest = NULL;
	this->m_hresult = S_OK;
	this->m_dwDataSize = 0;
	this->m_pvData = NULL;
} // CTNReport::CTNReport
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReport::~CTNReport()"
//==================================================================================
// CTNReport destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNReport object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNReport::~CTNReport(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pTest != NULL)
	{
		this->m_pTest->m_dwRefCount--;
		if (this->m_pTest->m_dwRefCount == 0)
		{
			//DPL(7, "Deleting test %x.", 1, this->m_pTest);
			delete (this->m_pTest);
		} // end if (last reference)

		this->m_pTest = NULL;
	} // end if (have test)

	if (this->m_pvData != NULL)
	{
		LocalFree(this->m_pvData);
		this->m_pvData = NULL;
	} // end if (allocated data)
} // CTNReport::~CTNReport
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReport::PrintToFile()"
//==================================================================================
// CTNReport::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the CTNReport object to the file specified.
//
// Arguments:
//	HANDLE hFile		File to print to.
//	BOOL fVerbose		Should we print extra information?
//	BOOL fBVTFormat		Should we print in BVT standard format?
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNReport::PrintToFile(HANDLE hFile, BOOL fVerbose, BOOL fBVTFormat)
{
	HRESULT				hr = S_OK;
	int					i;
	PTNSLAVEINFO		pSlave = NULL;
	char				szBuffer[1024];
	int					iTesterNum = -1;
	CTNStoredData		writedata;
	TNWRITEDATA			tnwd;
	BOOL				fComplete = TRUE;
	BOOL				fSuccess = TRUE;
	int					iNumWarnings = 0;



	switch (this->m_dwType)
	{
		case RT_ADDMACHINE:
			FileWriteLine(hFile, "[Report: AddMachine]");
			strftime(szBuffer, 1024, "Time= %I:%M:%S%p  %m/%d/%y", &(this->m_time));
			FileWriteLine(hFile, szBuffer);

			pSlave = (PTNSLAVEINFO) this->m_machines.GetItem(0);
			FileSprintfWriteLine(hFile, "Name= %s",
								1, pSlave->m_szComputerName);

			// BVT format needs a closing section
			if (fBVTFormat)
			{
				FileWriteLine(hFile, "[/Report: AddMachine]");
			} // end if (BVT format)
		  break;
		  
		case RT_REMOVEMACHINE:
			FileWriteLine(hFile, "[Report: RemoveMachine]");
			strftime(szBuffer, 1024, "Time= %I:%M:%S%p  %m/%d/%y", &(this->m_time));
			FileWriteLine(hFile, szBuffer);

			pSlave = (PTNSLAVEINFO) this->m_machines.GetItem(0);
			FileSprintfWriteLine(hFile, "Name= %s",
								1, pSlave->m_szComputerName);

			switch (this->m_hresult)
			{
				case RRM_DROPPED:
					FileWriteLine(hFile, "Reason= dropped");
				  break;

				case RRM_LEFT:
					FileWriteLine(hFile, "Reason= left");
				  break;

				case RRM_DONETESTING:
					FileWriteLine(hFile, "Reason= done testing");
				  break;

				case RRM_TIMEREXPIRED:
					FileWriteLine(hFile, "Reason= timer expired");
				  break;

				default:
					FileWriteLine(hFile, "Reason= unknown");
				  break;
			} // end switch (on reason for removal)

			// BVT format needs a closing section
			if (fBVTFormat)
			{
				FileWriteLine(hFile, "[/Report: RemoveMachine]");
			} // end if (BVT format)
		  break;
		
		case RT_CLOSEDSESSION:
			FileWriteLine(hFile, "[Report: ClosedSession]");
			strftime(szBuffer, 1024, "Time= %I:%M:%S%p  %m/%d/%y", &(this->m_time));
			FileWriteLine(hFile, szBuffer);

			// BVT format needs a closing section
			if (fBVTFormat)
			{
				FileWriteLine(hFile, "[/Report: ClosedSession]");
			} // end if (BVT format)
		  break;
		
		case RT_ALLTESTINGCOMPLETE:
			FileWriteLine(hFile, "[Report: AllTestingComplete]");
			strftime(szBuffer, 1024, "Time= %I:%M:%S%p  %m/%d/%y", &(this->m_time));
			FileWriteLine(hFile, szBuffer);

			// BVT format needs a closing section
			if (fBVTFormat)
			{
				FileWriteLine(hFile, "[/Report: AllTestingComplete]");
			} // end if (BVT format)
		  break;

		case RT_TESTSKIPPED:
			FileWriteLine(hFile, "[Report: TestSkipped]");

			strftime(szBuffer, 1024, "Time= %I:%M:%S%p  %m/%d/%y", &(this->m_time));
			FileWriteLine(hFile, szBuffer);

			//Ignore error
			this->m_pTest->PrintToFile(hFile, FALSE, FALSE);

			// BVT format needs a closing section
			if (fBVTFormat)
			{
				FileWriteLine(hFile, "[/Report: TestSkipped]");
			} // end if (BVT format)
		  break;

		case RT_TESTASSIGNED:
		case RT_TESTASSIGNED_SUBTEST:
			for(i = 0; i < this->m_pTest->m_iNumMachines; i++)
			{
				if (this->m_pTest->m_fStarted)
				{
					if (this->m_pTest->m_paTesterSlots[i].fComplete)
					{
						if (! this->m_pTest->m_paTesterSlots[i].fSuccess)
							fSuccess = FALSE;
					} // end if (this tester completed)
					else
					{
						fComplete = FALSE;
					} // end else (this tester has not completed)

					iNumWarnings += this->m_pTest->m_paTesterSlots[i].iNumWarnings;
				} // end if (test has started)
				else
				{
					// BVT format doesn't have comments.
					if ((this->m_pTest->m_paTesterSlots[i].dwAssignTime == 0) &&
						(this->m_pTest->m_paTesterSlots[i].pSlave != NULL) &&
						(! fBVTFormat))
					{
						FileSprintfWriteLine(hFile, "// %sest waiting for %s",
											2, ((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? "Subt" : "T"),
											this->m_pTest->m_paTesterSlots[i].pSlave->m_szComputerName);
					} // end if (placeholder for a slave and not in BVT format)
				} // end else (test has not started)
			} // end for (each tester slot)

			// BVT format doesn't have comments.
			if ((this->m_pTest->m_fStarted) && (! fBVTFormat))
			{
				if (fComplete)
				{
#pragma BUGBUG(vanceo, "FilterSuccess may override this")
					if (fSuccess)
					{
						FileSprintfWriteLine(hFile, "// %sest completed successfully",
											1, ((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? "Subt" : "T"));
					} // end if (test was successful)
					else
					{
						FileSprintfWriteLine(hFile, "// %sest completed with failures",
											1, ((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? "Subt" : "T"));
					} // end else (test had failures)
				} // end if (test has completed)
				else
				{
					FileSprintfWriteLine(hFile, "// %sest not completed",
										1, ((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? "Subt" : "T"));
				} // end else (test has not completed)

				if (iNumWarnings > 0)
				{
					FileSprintfWriteLine(hFile, "// %sest has %i warnings",
										2, ((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? "Subt" : "T"),
										iNumWarnings);
				} // end if (the test had warnings)
			} // end if (test has started and not in BVT format)
	
			FileWriteLine(hFile, "[Report: TestAssigned]");
			strftime(szBuffer, 1024, "Time= %I:%M:%S%p  %m/%d/%y", &(this->m_time));
			FileWriteLine(hFile, szBuffer);

			//Ignore error
			this->m_pTest->PrintToFile(hFile, FALSE,
									((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? TRUE : FALSE));

			writedata.m_dwLocation = TNTSDL_REPORTASSIGN;

			goto ALL_SINGLETESTER_REPORTS;
		  break;


		case RT_TESTCOMPLETELYASSIGNED:
		case RT_TESTCOMPLETELYASSIGNED_SUBTEST:
			if (! this->m_pTest->m_fStarted)
			{
				DPL(0, "Test %u has been completely assigned but it's not started!?",
					1, this->m_pTest->m_dwUniqueID);

				DEBUGBREAK();
			} // end if (placeholder for a slave)

			for(i = 0; i < this->m_pTest->m_iNumMachines; i++)
			{
				if (this->m_pTest->m_paTesterSlots[i].fComplete)
				{
					if (! this->m_pTest->m_paTesterSlots[i].fSuccess)
						fSuccess = FALSE;
				} // end if (this tester completed)
				else
				{
					fComplete = FALSE;
				} // end else (this tester has not completed)

				iNumWarnings += this->m_pTest->m_paTesterSlots[i].iNumWarnings;
			} // end for (each tester slot)


			// BVT format doesn't have comments.
			if (! fBVTFormat)
			{
				if (fComplete)
				{
#pragma BUGBUG(vanceo, "FilterSuccess may override this")
					if (fSuccess)
					{
						FileSprintfWriteLine(hFile, "// %sest completed successfully",
											1, ((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? "Subt" : "T"));
					} // end if (test was successful)
					else
					{
						FileSprintfWriteLine(hFile, "// %sest completed with failures",
											1, ((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? "Subt" : "T"));
					} // end else (test had failures)
				} // end if (test has completed)
				else
				{
					FileSprintfWriteLine(hFile, "// %sest not completed",
										1, ((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? "Subt" : "T"));
				} // end else (test has not completed)

				if (iNumWarnings > 0)
				{
					FileSprintfWriteLine(hFile, "// %sest has %i warnings",
										2, ((this->m_dwType == RT_TESTASSIGNED_SUBTEST) ? "Subt" : "T"),
										iNumWarnings);
				} // end if (the test had warnings)
			} // end if (not in BVT format)

			
			FileWriteLine(hFile, "[Report: TestCompletelyAssigned]");
			strftime(szBuffer, 1024, "Time= %I:%M:%S%p  %m/%d/%y", &(this->m_time));
			FileWriteLine(hFile, szBuffer);

			//Ignore error
			this->m_pTest->PrintToFile(hFile, FALSE,
									((this->m_dwType == RT_TESTCOMPLETELYASSIGNED_SUBTEST) ? TRUE : FALSE));


			this->m_machines.EnterCritSection();
			for(i = 0; i < this->m_machines.Count(); i++)
			{
				pSlave = (PTNSLAVEINFO) this->m_machines.GetItem(i);

				FileSprintfWriteLine(hFile, "{Tester: %i}", 1, i);

				if (fVerbose)
				{
					pSlave->PrintToFile(hFile);
				} // end if (we're printing verbosely)
				else
				{
					FileSprintfWriteLine(hFile, "\tComputerName= %s",
										1, pSlave->m_szComputerName);
				} // end else (we're not printing verbosely)

				// Single space
				FileWriteLine(hFile, "");
			} // end for (each slave)
			this->m_machines.LeaveCritSection();


			// BVT format needs a closing section
			if (fBVTFormat)
			{
				FileWriteLine(hFile, "[/Report: TestCompletelyAssigned]");
			} // end if (BVT format)
		  break;

		case RT_SUCCESS:
		case RT_SUCCESS_SUBTEST:
			FileWriteLine(hFile, "[Report: Success]");
			writedata.m_dwLocation = TNTSDL_REPORTSUCCESS;
			goto ALL_DURINGTEST_REPORTS;
		  break;

		case RT_FAILURE:
		case RT_FAILURE_SUBTEST:
			FileWriteLine(hFile, "[Report: Failure]");
			writedata.m_dwLocation = TNTSDL_REPORTFAILURE;
			goto ALL_DURINGTEST_REPORTS;
		  break;

		case RT_WARNING:
		case RT_WARNING_SUBTEST:
			FileWriteLine(hFile, "[Report: Warning]");
			writedata.m_dwLocation = TNTSDL_REPORTWARNING;

			// Note: continuing


ALL_DURINGTEST_REPORTS:

			strftime(szBuffer, 1024, "Time= %I:%M:%S%p  %m/%d/%y", &(this->m_time));
			FileWriteLine(hFile, szBuffer);

			//Ignore error
			this->m_pTest->PrintToFile(hFile, FALSE,
									(((this->m_dwType == RT_SUCCESS_SUBTEST) || (this->m_dwType == RT_FAILURE_SUBTEST) || (this->m_dwType == RT_WARNING_SUBTEST)) ? TRUE : FALSE));

			pSlave = (PTNSLAVEINFO) this->m_machines.GetItem(0);
			FileSprintfWriteLine(hFile, "ReporterName= %s",
								1, pSlave->m_szComputerName);

			FileSprintfWriteLine(hFile, "hresult= %x", 1, this->m_hresult);
			FileSprintfWriteLine(hFile, "ResultDescription= %e", 1, this->m_hresult);

			// Note: continuing


ALL_SINGLETESTER_REPORTS:

			pSlave = (PTNSLAVEINFO) this->m_machines.GetItem(0);

			iTesterNum = this->m_pTest->GetSlavesTesterNum(pSlave);

			// If we didn't find it, then bail.
			if (iTesterNum < 0)
			{
				DPL(0, "Couldn't locate slave (%s) in the tester list!",
					1, pSlave->m_szComputerName);

				// BVT format doesn't have comments.
				if (! fBVTFormat)
				{
					FileSprintfWriteLine(hFile, "// Couldn't locate slave (%s) in the tester list!",
										1, pSlave->m_szComputerName);
				} // end if (not in BVT format)

				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get the tester's index)


			FileSprintfWriteLine(hFile, "{Tester: %i}", 1, iTesterNum);

			if (fVerbose)
			{
				pSlave->PrintToFile(hFile);
			} // end if (we're printing verbosely)
			else
			{
				FileSprintfWriteLine(hFile, "\tComputerName= %s",
									1, pSlave->m_szComputerName);
			} // end else (we're not printing verbosely)

			if ((this->m_pvData != NULL) &&
				(this->m_pTest->m_pCase->m_pfnWriteData != NULL))
			{
				// Allow the module to write strings that can either help it
				// determine the reason for the report or be used as a means
				// of reproducing the test settings, if it chooses.

				ZeroMemory(&tnwd, sizeof (TNWRITEDATA));
				tnwd.dwSize = sizeof (TNWRITEDATA);
				tnwd.dwLocation = writedata.m_dwLocation;
				tnwd.pvData = this->m_pvData;
				tnwd.dwDataSize = this->m_dwDataSize;
				tnwd.pMachine = pSlave;
				tnwd.iTesterNum= iTesterNum;
				tnwd.pTest = this->m_pTest->m_pCase;
				tnwd.pWriteStoredData = &writedata;

				hr = this->m_pTest->m_pCase->m_pfnWriteData(&tnwd);

				if (hr != S_OK)
				{
					DPL(0, "Module's WriteData callback failed!", 0);

					// BVT format doesn't have comments.
					if (! fBVTFormat)
					{
						FileWriteLine(hFile, "// Module's WriteData callback failed!");
					} // end if (not in BVT format)

					goto DONE;
				} // end if (couldn't allocate memory)

				if (writedata.m_strings.Count() > 0)
					FileWriteLine(hFile, "{ModuleData}");

				for(i = 0; i < writedata.m_strings.Count(); i++)
				{
					FileSprintfWriteLine(hFile, "\t%s",
										1, writedata.m_strings.GetIndexedString(i));
				} // end for (each string)
			} // end if (the test has a write data function)

			// BVT format needs a closing section
			if (fBVTFormat)
			{
				switch (this->m_dwType)
				{
					case RT_TESTASSIGNED:
					case RT_TESTASSIGNED_SUBTEST:
						FileWriteLine(hFile, "[/Report: TestAssigned]");
					  break;

					case RT_SUCCESS:
					case RT_SUCCESS_SUBTEST:
						FileWriteLine(hFile, "[/Report: Success]");
					  break;

					case RT_FAILURE:
					case RT_FAILURE_SUBTEST:
						FileWriteLine(hFile, "[/Report: Failure]");
					  break;

					case RT_WARNING:
					case RT_WARNING_SUBTEST:
						FileWriteLine(hFile, "[/Report: Warning]");
					  break;

#ifdef DEBUG
					default:
						DPL(0, "In ALL_SINGLETESTER_REPORTS for unexpected report type %u!  DEBUGBREAK()-ing",
							1, this->m_dwType);
						DEBUGBREAK();
					  break;
				} // end switch (on report type)
#endif // DEBUG
			} // end if (BVT format)
		  break;



		case RT_SUCCESSCOMPLETE:
		case RT_SUCCESSCOMPLETE_SUBTEST:
			FileWriteLine(hFile, "[Report: CompleteSuccessfully]");
			writedata.m_dwLocation = TNTSDL_REPORTFINALSUCCESS;
			goto ALL_COMPLETETEST_REPORTS;
		  break;

		case RT_FAILURECOMPLETE:
		case RT_FAILURECOMPLETE_SUBTEST:
			FileWriteLine(hFile, "[Report: CompleteWithFailure]");
			writedata.m_dwLocation = TNTSDL_REPORTFINALFAILURE;

			// Note: continuing


ALL_COMPLETETEST_REPORTS:

			strftime(szBuffer, 1024, "Time= %I:%M:%S%p  %m/%d/%y", &(this->m_time));
			FileWriteLine(hFile, szBuffer);

			//Ignore error
			this->m_pTest->PrintToFile(hFile, FALSE,
									(((this->m_dwType == RT_SUCCESSCOMPLETE_SUBTEST) || (this->m_dwType == RT_FAILURECOMPLETE_SUBTEST)) ? TRUE : FALSE));

			if ((this->m_pvData != NULL) &&
				(this->m_pTest->m_pCase->m_pfnWriteData != NULL))
			{
				// Allow the module to write strings that can either help it
				// determine the reason for the report or be used as a means
				// of reproducing the test settings, if it chooses.

				ZeroMemory(&tnwd, sizeof (TNWRITEDATA));
				tnwd.dwSize = sizeof (TNWRITEDATA);
				tnwd.dwLocation = writedata.m_dwLocation;
				tnwd.pvData = this->m_pvData;
				tnwd.dwDataSize = this->m_dwDataSize;
				tnwd.pMachine = pSlave;
				tnwd.iTesterNum= iTesterNum;
				tnwd.pTest = this->m_pTest->m_pCase;
				tnwd.pWriteStoredData = &writedata;

				hr = this->m_pTest->m_pCase->m_pfnWriteData(&tnwd);

				if (hr != S_OK)
				{
					DPL(0, "Module's WriteData callback failed!", 0);
					
					// BVT format doesn't have comments.
					if (! fBVTFormat)
					{
						FileWriteLine(hFile, "// Module's WriteData callback failed!");
					} // end if (not in BVT format)

					goto DONE;
				} // end if (couldn't allocate memory)

				if (writedata.m_strings.Count() > 0)
					FileWriteLine(hFile, "{ModuleData}");

				for(i = 0; i < writedata.m_strings.Count(); i++)
				{
					FileSprintfWriteLine(hFile, "\t%s",
										1, writedata.m_strings.GetIndexedString(i));
				} // end for (each string)
			} // end if (the test has a write data function)

			if (fVerbose)
			{
				this->m_machines.EnterCritSection();
				for(i = 0; i < this->m_machines.Count(); i++)
				{
					pSlave = (PTNSLAVEINFO) this->m_machines.GetItem(i);

					FileSprintfWriteLine(hFile, "{Tester: %i}", 1, i);

					pSlave->PrintToFile(hFile);

					// Single space
					FileWriteLine(hFile, "");
				} // end for (each slave)
				this->m_machines.LeaveCritSection();
			} // end if (we're printing verbosely)


			// BVT format needs a closing section
			if (fBVTFormat)
			{
				switch (this->m_dwType)
				{
					case RT_SUCCESSCOMPLETE:
					case RT_SUCCESSCOMPLETE_SUBTEST:
						FileWriteLine(hFile, "[/Report: CompleteSuccessfully]");
					  break;

					case RT_FAILURECOMPLETE:
					case RT_FAILURECOMPLETE_SUBTEST:
						FileWriteLine(hFile, "[/Report: CompleteWithFailure]");
					  break;

#ifdef DEBUG
					default:
						DPL(0, "In ALL_COMPLETETEST_REPORTS for unexpected report type %u!  DEBUGBREAK()-ing",
							1, this->m_dwType);
						DEBUGBREAK();
					  break;
				} // end switch (on report type)
#endif // DEBUG
			} // end if (BVT format)
		  break;

		default:
			DPL(0, "Unknown kind of report type (%u)!", 1, this->m_dwType);

			// BVT format doesn't have comments.
			if (! fBVTFormat)
			{
				FileSprintfWriteLine(hFile, "// Unknown kind of report type (%u)!",
									1, this->m_dwType);
			} // end if (not in BVT format)

			hr = E_FAIL;
		  break;
	} // end switch (on kind of report)


DONE:

	if (hr != S_OK)
	{
		FileSprintfWriteLine(hFile, "// %E", 1, hr);
	} // end if (exiting with error)

	return (hr);
} // CTNReport::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#endif // ! XBOX