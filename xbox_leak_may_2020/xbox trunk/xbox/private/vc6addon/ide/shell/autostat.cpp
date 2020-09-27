/////////////////////////////////////////////////////////////////////////////
//  CAutomationState Object
//
//  This global object contains the state of automation currently happening 
//  in the 
// Use one of the the following functions to set the error code.
//
#include "stdafx.h"
#include "utilauto.h"
#include "autostat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void CShellAutomationState::Reset()
{ 
	m_bDisplayUI = TRUE ; 
	m_bSaveChanges = TRUE; 
	m_hResult = S_OK;
	m_bSavePromptCanceled = FALSE ;
} 

// Set the error with a CFileException::m_cause type error. 
// Translates the error into an HRESULT for automation purposes.
// Used by the global function SetError
void CShellAutomationState::SetFileError(int FileExceptionError)
{
	// Do not overwrite an existing error:
	if (m_hResult != S_OK) 
	{
		return ;
	}

	switch(FileExceptionError)
	{
		// The following errors means everything is A-OK.
	case (CFileException::none):		//No error occurred.
		m_hResult = S_OK ;
		break ;

		// The Following are the unique file errors that the automation state handles
	case (CFileException::endOfFile):   //The end of file was reached.
		m_hResult = DS_E_ENDOFFILE ;
		break ;

	case CFileException::fileNotFound:		//The file could not be located.
		m_hResult = DS_E_FILENOTFOUND ;
		break;

	case CFileException::badPath:			//All or part of the path is invalid.
		m_hResult = DS_E_BADPATH ;
		break ;

	case CFileException::accessDenied:		//The file could not be accessed.
		m_hResult = DS_E_ACCESSDENIED ;
		break ;

	case CFileException::invalidFile:		//There was an attempt to use an invalid file handle.
		m_hResult = DS_E_INVALIDFILE ;
		break ;

	case CFileException::diskFull:			//The disk is full.
		m_hResult = DS_E_DISKFULL ;
		break ;

	case CFileException::sharingViolation:  //SHARE.EXE was not loaded, or a shared region was locked.
		m_hResult = DS_E_SHARINGVIOLATION;
		break ;

	// The following errors all return E_FAIL. 
	case CFileException::tooManyOpenFiles:  //The permitted number of open files was exceeded.
	case CFileException::removeCurrentDir:  //The current working directory cannot be removed.
	case CFileException::directoryFull:		//There are no more directory entries.
	case CFileException::badSeek:			//There was an error trying to set the file pointer.
	case CFileException::hardIO:			//There was a hardware error.
	case CFileException::lockViolation:		//There was an attempt to lock a region that was already locked.
	case CFileException::generic:			//An unspecified error occurred.
	default:
		m_hResult = DS_E_UNKNOWN ;
	};
}

// Set the return value of the current operation.
void CShellAutomationState::SetError(HRESULT hResult /*= S_OK*/)
{
	m_hResult = hResult ;
}

// Takes a varient holding a DsSaveChanges enum and correct sets up the AutomationState.
// bAlwaysSave is used by Save to ensure that the user didn't specify DontSaveChanges to 
// Save.
void CShellAutomationState::SaveChanges(const VARIANT FAR& vtLongSaveChanges)
{
	const long DefaultValue = -1 ;
	Reset() ;

	// Fail if the parameter is a boolean.
	if (vtLongSaveChanges.vt == VT_BOOL)
	{
		DsThrowShellOleDispatchException(DS_E_BAD_PARAM_VALUE) ;
	}

	// Convert variant to a long.
	long lSaveChanges ;
	::ConvertVariantToLong(vtLongSaveChanges, lSaveChanges, DefaultValue) ;
	
	// Use parameter
	switch(lSaveChanges)
	{
	case DefaultValue: // Default value used.
		m_bDisplayUI = FALSE;
		break ;
	case dsSaveChangesYes:			
		m_bDisplayUI = FALSE;
		break ;
	case dsSaveChangesNo:
		m_bDisplayUI = FALSE;
		m_bSaveChanges = FALSE ;
		break ;
	case dsSaveChangesPrompt:
		m_bDisplayUI = TRUE ;
		break ;
	default:
		DsThrowShellOleDispatchException(DS_E_BAD_PARAM_VALUE) ;
	}
}

void CShellAutomationState::SetPromptState(const VARIANT FAR& vtBoolPrompt)
{
	// Reset State
	Reset() ;
	
	// Convert parameter to a bool.
	BOOL bPrompt;
	::ConvertVariantToBOOL(vtBoolPrompt, bPrompt, FALSE) ;

	// Set up state.
	if (bPrompt)
	{
		// Show UI.
		m_bDisplayUI = TRUE ;
	}
	else
	{
		// Don't show UI.
		m_bDisplayUI = FALSE;
	}

}

//
// Throws exceptions if the automation state has an error.
//
HRESULT CShellAutomationState::HandleError()
{
	// Get the error.
	HRESULT hr = GetError() ;

	// Set the automation state to default.
	Reset() ; 

	// Handle the errors.
	if (FAILED(hr))
	{
		// There was an error reported.
		DsThrowShellOleDispatchException(hr) ;		
	}

	// Return success codes.
	return hr ;
}

//
//
//
DsSaveStatus CShellAutomationState::SavePromptResult()
{
	return m_bSavePromptCanceled ? dsSaveCanceled : dsSaveSucceeded; 
}

//
// Global automation state object
//
CShellAutomationState g_AutomationState ;

//
// Global Pointer Used by Packages.
//
extern AFX_EXT_DATADEF IDsAutomationState* g_pAutomationState = &g_AutomationState ;

