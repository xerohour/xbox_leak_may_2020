//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#ifdef _XBOX // Damn ANSI conversion
#include <stdio.h>
#include <stdlib.h>
#endif

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"

#include "tncontrl.h"
#include "main.h"

#include "tablecase.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableCase::CTNTestTableCase()"
//==================================================================================
// CTNTestTableCase constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTestTableCase object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNTestTableCase::CTNTestTableCase(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTestTableCase));

	this->m_pszDescription = NULL;
	this->m_pszInputDataHelp = NULL;

	this->m_iNumMachines = -1;
	this->m_dwOptionFlags = 0;

	this->m_pfnCanRun = NULL;

	this->m_pfnGetInputData = NULL;
	this->m_pfnExecCase = NULL;
	this->m_pfnWriteData = NULL;
	this->m_pfnFilterSuccess = NULL;

	this->m_paGraphs = NULL;
	this->m_dwNumGraphs = 0;
} // CTNTestTableCase::CTNTestTableCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableCase::~CTNTestTableCase()"
//==================================================================================
// CTNTestTableCase destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTestTableCase object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTestTableCase::~CTNTestTableCase(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNTestTableCase::~CTNTestTableCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableCase::CalculateChecksum()"
//==================================================================================
// CTNTestTableCase::CalculateChecksum
//----------------------------------------------------------------------------------
//
// Description: Calculates the checksum for this test case.
//
// Arguments:
//	LPCHECKSUM lpChecksum	Place to store checksum generated.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestTableCase::CalculateChecksum(LPCHECKSUM lpChecksum)
{
	// Calculate checksum with our name, ID, num machines and options.
	(*lpChecksum) = GetChecksum(this->m_pszName, strlen(this->m_pszName))
					+ GetChecksum(this->m_pszID, strlen(this->m_pszID))
					+ this->m_iNumMachines
					+ this->m_dwOptionFlags;
	return (S_OK);
} // CTNTestTableCase::CalculateChecksum
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#ifndef _XBOX // ! XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableCase::PrintToFile()"
//==================================================================================
// CTNTestTableCase::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the name and ID of this test case to the passed in file.
//
// Arguments:
//	HANDLE hFile		File to print to.
//	DWORD dwOptions		Options on how to print.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestTableCase::PrintToFile(HANDLE hFile, DWORD dwOptions)
{
	char	szTemp[256];
#ifdef _XBOX // Damn ANSI conversion
	WCHAR	szWideTemp[256];
#endif

	// If this is a built-in script and we shouldn't print those, we're done.
	if ((this->m_dwOptionFlags & TNTCO_BUILTIN) && (! (dwOptions & TNTTPO_BUILTIN)))
		return (S_OK);


	if ((this->m_pszID == NULL) || (this->m_pszName == NULL))
	{
		DPL(0, "This test case (%x) is invalid!", 1, this);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (there's not a valid ID or name)


	//Ignoring errors for these
	FileWriteString(hFile, this->m_pszID);
	FileWriteString(hFile, "\t");
	FileWriteString(hFile, this->m_pszName);

	if (dwOptions & TNTTPO_CASEDESCRIPTIONS)
	{
#pragma BUGBUG(vanceo, "Remove check when all tests switched over to new ADDTESTDATA")
		if (this->m_pszDescription != NULL)
		{
			FileWriteString(hFile, "\t");
			FileWriteString(hFile, this->m_pszDescription);
		} // end if (there's a description)
	} // end if (we should print case descriptions)

	if (dwOptions & TNTTPO_SUCCESSES)
	{
#ifndef _XBOX // Damn ANSI conversion
		wsprintf(szTemp, "\t%i", this->m_stats.GetSuccesses());
#else
		swprintf(szWideTemp, L"\t%i", this->m_stats.GetSuccesses());
		wcstombs(szTemp, szWideTemp, wcslen(szWideTemp));
		szTemp[wcslen(szWideTemp)] = 0;
#endif
		FileWriteString(hFile, szTemp);	
	} // end if (we should print the total)

	if (dwOptions & TNTTPO_FAILURES)
	{
#ifndef _XBOX // Damn ANSI conversion
		wsprintf(szTemp, "\t%i", this->m_stats.GetFailures());
#else
		swprintf(szWideTemp, L"\t%i", this->m_stats.GetFailures());
		wcstombs(szTemp, szWideTemp, wcslen(szWideTemp));
		szTemp[wcslen(szWideTemp)] = 0;
#endif
		FileWriteString(hFile, szTemp);	
	} // end if (we should print the total)

	if (dwOptions & TNTTPO_TOTAL)
	{
#ifndef _XBOX // Damn ANSI conversion
		wsprintf(szTemp, "\t%i", this->m_stats.GetTotalComplete());
#else
		swprintf(szWideTemp, L"\t%i", this->m_stats.GetTotalComplete());
		wcstombs(szTemp, szWideTemp, wcslen(szWideTemp));
		szTemp[wcslen(szWideTemp)] = 0;
#endif
		FileWriteString(hFile, szTemp);	
	} // end if (we should print the total)

	if (dwOptions & TNTTPO_WARNINGS)
	{
#ifndef _XBOX // Damn ANSI conversion
		wsprintf(szTemp, "\t%i", this->m_stats.GetWarnings());
#else
		swprintf(szWideTemp, L"\t%i", this->m_stats.GetWarnings());
		wcstombs(szTemp, szWideTemp, wcslen(szWideTemp));
		szTemp[wcslen(szWideTemp)] = 0;
#endif
		FileWriteString(hFile, szTemp);	
	} // end if (we should print the total)

	// This will end the line
	FileWriteLine(hFile, "");


	return (S_OK);
} // CTNTestTableCase::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX