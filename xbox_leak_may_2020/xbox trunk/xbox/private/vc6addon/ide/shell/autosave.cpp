#include "stdafx.h"

#include "resource.h"

CAutosave AFX_EXT_DATADEF gAutosaver;


/* Private Data *************************************************************/

static CString gstrAutosaveSubdir = "$AUTSAV$";
static CString gstrUntitledSubdir = "$UNTTLD$";	
static BOOL fHaveUntitledDir = FALSE;

/* Private Methods **********************************************************/

void
CAutosave::GetUntitledDir()
{
	CPath pathModule;
	char szModuleFileName[MAX_PATH];
    
// Create the untitled autosave directory.
    
	GetModuleFileName(NULL, szModuleFileName, MAX_PATH);
	pathModule.Create(szModuleFileName);
	m_dirUntitled.CreateFromPath(pathModule);
	m_dirUntitled.AppendSubdirName(gstrUntitledSubdir);
	if (m_dirUntitled.CreateOnDisk() && SetFileAttributes((char *)(const char *)m_dirUntitled, FILE_ATTRIBUTE_HIDDEN))
		fHaveUntitledDir = TRUE;
}

/* Public Methods ***********************************************************/

CAutosave::CAutosave()
{ 
	m_fEnabled = FALSE; 
	m_nSaveInterval = 30; 
	m_hWndTimerOwner = NULL;
}

CAutosave::~CAutosave()
{
// Remove the untitled autosave directory when we bring down the SUSHI 
// session.  It is pefectly acceptable for this call to fail -- the 
// directory may not be empty.

	m_dirUntitled.RemoveFromDisk();	
	return;
}

BOOL
CAutosave::Enable(
	UINT nSaveInterval)
{
	if (m_hWndTimerOwner == NULL)
	{
		TRACE("Can't enable autosaver - no timer window owner!\n");
		return FALSE;
	}

// If the Autosaver is currently enabled, then they're asking us to reset
// the save interval.

	if (m_fEnabled)
	{
		if (nSaveInterval == m_nSaveInterval)
			return(TRUE);
		VERIFY(KillTimer(m_hWndTimerOwner, m_nTimerID));
		m_fEnabled = FALSE;
	}

// make sure the timer interval is valid
	if (!nSaveInterval)
	{
		TRACE("Autosave timer interval invalid--not set.\n");
		return(FALSE);
	}

// Start a new timer for the autosaver.

	m_nTimerID = SetTimer(m_hWndTimerOwner, IDT_AUTOSAVE,
						  60000*nSaveInterval, NULL);
	if (m_nTimerID == 0)
	{
		TRACE("Unable to install autosave timer.\n");
		return(FALSE);
	}

// Everything's kosher.  Make sure we have directory for untitled files.

	if (!fHaveUntitledDir)
		GetUntitledDir();

	TRACE("Autosaver enabled.\n");
	m_nSaveInterval = nSaveInterval;
	m_fEnabled = TRUE;
	m_fInTick = FALSE;
	return(TRUE);
}

BOOL 
CAutosave::Disable()
{
	if (!m_fEnabled)
		return(TRUE);
	else if (KillTimer(m_hWndTimerOwner, m_nTimerID))
	{
		m_fEnabled = FALSE;
		return(TRUE);
	}
	else
		return(FALSE);
}

BOOL 
CAutosave::GetAutosaveName(
	BOOL fUntitledDoc,
	const CPath &pathDirtyFile,				
	CPath &pathAutosave,
	enum DirAction fCreateDir)
{
	CDir dirDirtyFile;
	CDir dirAutosave;

	dirDirtyFile.CreateFromPath(pathDirtyFile);

// For titled documents, the autosave directory is off the document's directory.
// For untitled documents, the autosave path is off VCPP.DLL's directory.
 
	if (!fUntitledDoc)
	{
		//struct _stat statDir;

	// Make sure that an autosave subdirectory exists, & create
	// a CDir object for it.

		dirAutosave = dirDirtyFile;
		dirAutosave.AppendSubdirName(gstrAutosaveSubdir);
		
		if (lstrlen(dirAutosave) > MAX_PATH)
		{
			m_ErrorStatus = fCreateDir ? PathTooLong : NoSubdirectory;
			return FALSE;
		}
		
        if (!dirAutosave.ExistsOnDisk())
			if (fCreateDir)
			{
				if (!dirAutosave.CreateOnDisk())
				{
					m_ErrorStatus = CantMakeSubdirectory;
					return FALSE;
				}
				VERIFY(SetFileAttributes((char *)(const char *)dirAutosave, 
										 FILE_ATTRIBUTE_HIDDEN));
			}
			else
			{
				m_ErrorStatus = NoSubdirectory;
				return FALSE;
			}
		/*			
		else
			if (!statDir.st_mode & _S_IFDIR)  
			{
				m_ErrorStatus = CantMakeSubdirectory;
				return FALSE;
			} */
	}
	else
		dirAutosave = m_dirUntitled;

	if (!pathAutosave.CreateFromDirAndFilename(dirAutosave, 
										  pathDirtyFile.GetFileName()))
	{
		m_ErrorStatus = PathTooLong;
		return FALSE;
	}

	m_ErrorStatus = Success;
	return TRUE;
}

BOOL
CAutosave::RecoverAutosaveFileCheck(
	const CPath &pathUserFile)
{
	CPath pathAutosave;
	CDir dirAutosave;

	if (!AutosaveFileExists(pathUserFile, pathAutosave))
		return(TRUE);

	DWORD dwAttribs = 0;

	if (pathUserFile.ExistsOnDisk()) 
		VERIFY((dwAttribs = GetFileAttributes((char *)(const char *)pathUserFile)) != -1);

	// dolphin 4693 [patbr]
	// handling of available autosave file when source file being opened is read-only was inadequate.
	// rather than simply reword the message, we now handle the read-only source file same as writable
	// file, except that we use different message to let user know that he is overwriting read-only file.
	if (dwAttribs & FILE_ATTRIBUTE_READONLY)
	{
		switch (QuestionBox(IDS_AUTOSAVE_READONLY, MB_YESNOCANCEL, (const char *)pathUserFile, (const char *)pathUserFile))
		{
			case IDYES:
				VERIFY(SetFileAttributes((char *)(const char *)pathUserFile, FILE_ATTRIBUTE_NORMAL));
				VERIFY(CopyFile((char *)(const char *)pathAutosave, (char *)(const char *)pathUserFile, FALSE));
				return(TRUE);

			case IDNO:
				VERIFY(SetFileAttributes((char *)(const char *)pathAutosave, FILE_ATTRIBUTE_NORMAL));
				pathAutosave.DeleteFromDisk();
				dirAutosave.CreateFromPath(pathAutosave);
				dirAutosave.RemoveFromDisk();
				return(TRUE);

			default:
				return(FALSE);
		}
	}
	else
	{
		switch (QuestionBox(IDS_AUTOSAVE_DETECTED_BACKUP, MB_YESNOCANCEL, (const char *)pathUserFile, (const char *)pathUserFile)) 					
		{
			case IDYES:
				VERIFY(CopyFile((char *)(const char *)pathAutosave, (char *)(const char *)pathUserFile, FALSE));
				return(TRUE);

			case IDNO:
				VERIFY(SetFileAttributes((char *)(const char *)pathAutosave, FILE_ATTRIBUTE_NORMAL));
				pathAutosave.DeleteFromDisk();
				dirAutosave.CreateFromPath(pathAutosave);
				dirAutosave.RemoveFromDisk();
				return(TRUE);

			default:
				return(FALSE);
		}
	}
	return TRUE;
}

void
CAutosave::RemoveAutosaveFile(
	BOOL fUntitled,
	const CPath &pathUserFile)
{
// Clobber an existing autosave file -- even if it is read-only.

	CPath pathAutosave;

	VERIFY(GetAutosaveName(fUntitled, pathUserFile, pathAutosave, DontCreateDir)
		 	|| m_ErrorStatus == NoSubdirectory);

	if (m_ErrorStatus == NoSubdirectory)
		return;	 // No directory => No file to remove.				 	

	if (pathAutosave.ExistsOnDisk())
	{
		VERIFY(SetFileAttributes((char *)(const char *)pathAutosave, 
								 FILE_ATTRIBUTE_NORMAL));
		pathAutosave.DeleteFromDisk();
	}

// Attempt to remove the autosave directory.  This will fail if we can't do it.

	if (!fUntitled)
	{	
		CDir dirAutosave;
		dirAutosave.CreateFromPath(pathAutosave);
		dirAutosave.RemoveFromDisk();
	}

// Outta here.

	return;
}

BOOL
CAutosave::AutosaveFileExists(
	const CPath &pathUserFile, 
	CPath &pathAutosave)
{
	VERIFY(GetAutosaveName(FALSE, pathUserFile, pathAutosave, DontCreateDir)
		 	|| m_ErrorStatus == NoSubdirectory);
	
	if (m_ErrorStatus != NoSubdirectory)
	{
		return (pathAutosave.ExistsOnDisk());
	}
	else
		return FALSE;
}

void
CAutosave::ExitTick()
{
	ASSERT(m_fInTick);
	m_fInTick = FALSE;
	while (!m_pathlistErrors.IsEmpty())
	{
		CString str;
		CPath *ppathErrEntry = (CPath *)m_pathlistErrors.RemoveHead();
		MsgBox(Error, MsgText(str, IDS_AUTOSAVE_ERROR, (const char *)*ppathErrEntry));
		delete ppathErrEntry;
	}
	return;
}
