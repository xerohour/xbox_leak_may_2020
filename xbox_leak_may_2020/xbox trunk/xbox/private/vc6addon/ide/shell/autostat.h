#ifndef __AutoStat_h__
#define __AutoStat_h__
////////////////////////////////////////////////////////////////////////////
//
// CShellAutomationState
//
//
class CShellAutomationState : public IDsAutomationState
{
public:
	// If TRUE, you can display UI. If FALSE do not display any UI.
	virtual BOOL DisplayUI() 
		{return m_bDisplayUI; }

	// If TRUE, you should save changes, if FALSE you should discard changes.
	virtual BOOL SaveChanges() 
		{return m_bSaveChanges; }

	// Use one of the the following functions to set the error code.
	//
	// Set the error with a CFileException::m_cause type error. 
	// Translates the error into an HRESULT for automation purposes.
	// Used by the global function SetError
	virtual void SetFileError(int FileExceptionError) ;

	// What is this for?
	// Set the return value of the current operation.
	virtual void SetError(HRESULT hResult);

	// Set to signal that the Save prompt was canceled.
	virtual void SetSavePromptCanceled()		
		{m_bSavePromptCanceled = TRUE; }

	// For use only by the Automation Code.
	// The following will change!!!
public:
	void Reset() ;

	// Get the result of the current opreation
	HRESULT GetError()
		{return m_hResult; }

	// Reset the error state and throw exception on error.
	// Returns successful HRESULTs.
	HRESULT HandleError();

	// Enable or disable UI.
	// Only enable ui if you absolutely have to!
	virtual void EnableUI()
		{m_bDisplayUI = TRUE;}
	virtual void DisableUI()
		{m_bDisplayUI = FALSE;}

	void SetSaveChanges(BOOL bFlag) 
		{m_bSaveChanges = bFlag; }

	// Go straight from varient to the correct flags.
	void SaveChanges(const VARIANT FAR& vtLongSaveChanges);
	void SetPromptState(const VARIANT FAR& vtBoolPrompt);

	// Returns dsSaveCanceled if the save prompt was canceled.
	DsSaveStatus SavePromptResult() ;

	CShellAutomationState() 
		{Reset();}
	// Internal Implemenation
private:
	HRESULT m_hResult ; // Error Value

	//TODO: Change to enums.
	BOOL m_bSaveChanges;	// True if changes are to be saved.
	BOOL m_bDisplayUI ;		// True if the function can display UI. If FALSE, 
	BOOL m_bSavePromptCanceled ;
};

//
// Used by the core automation code.
//
extern CShellAutomationState g_AutomationState ;

#endif // __AutoStat_h__
