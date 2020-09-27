///////////////////////////////////////////////////////////////////////////////
//	FILESAVE.CPP
//		Utilities used in saving files.

#include "stdafx.h"
#include "path.h"
#include "resource.h"

#include "utilauto.h" // For g_pAutomationState.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//	CFileSaver

static BOOL RenameFile(LPCTSTR szOldName, LPCTSTR szNewName);
static void RemoveFileIfPossible(LPCTSTR szFileName);

// base for temp file names
const char BASED_CODE CFileSaver::c_szAps [] = "~VC";
static char BASED_CODE acDot[] = ".";

CFileSaver::CFileSaver(const char* szFileName)
{
	m_strName = szFileName;

	LPTSTR lpsz;
	TCHAR szDir[_MAX_PATH];
	LPTSTR lpszTempName = m_strTempName.GetBuffer(_MAX_PATH);
	LPTSTR lpszBackupName = m_strBackupName.GetBuffer(_MAX_PATH);
	BOOL bSuccess = FALSE, bNeedPath = FALSE;

	// First try to get temp names if the directory of interest.
	int nLen = GetFullPathName(szFileName, _MAX_PATH, szDir, &lpsz);
	if( (nLen > 0) && (nLen < _MAX_PATH) )
	{
		*lpsz = _T('\0');	// Strip file name.
		if( GetTempFileName(szDir, c_szAps, 0, lpszTempName) != 0 )
		{
			if( GetTempFileName(szDir, c_szAps, 0, lpszBackupName) != 0 )
				bSuccess = TRUE;
			else
				RemoveFileIfPossible(lpszTempName);
		}
	}

	// If we could get temp names in the directory of interest,
	// then try curdir.  This is not strictly cosher, but mostly this
	// will happen when the directory does not exist, and we should
	// catch this type of error later on.
	
	if (!bSuccess)
	{
		bNeedPath = TRUE;
		if( GetTempFileName(acDot, c_szAps, 0, lpszTempName) != 0 )
		{
			if( GetTempFileName(acDot, c_szAps, 0, lpszBackupName) != 0 )
				bSuccess = TRUE;
			else
				RemoveFileIfPossible(lpszTempName);
		}
	}

	m_strTempName.ReleaseBuffer();
	m_strBackupName.ReleaseBuffer();

	if (!bSuccess)
	{
		m_strTempName.Empty();
		m_strBackupName.Empty();
		return;
	}

	// GetTempFileName actually creates files.
	RemoveFileIfPossible(m_strTempName);

	// n.b. We must leave the backup file around to preserve namespace
	// until we need it.  Otherwise, an unrelated call to GetTempFileName
	// may try to use the same name, especially under Win95

	// If we got temp names for curdir, the we have to change the paths.
	if (bNeedPath)
	{
		RemoveFileIfPossible(m_strBackupName);

		CString strDir = StripName(szFileName);

		m_strTempName = strDir + StripPath(m_strTempName);
		m_strBackupName = strDir + StripPath(m_strBackupName);

		// REVIEW: create m_strBackupName
	}
}

CFileSaver::~CFileSaver()
{
	if (FileExists(m_strTempName))
		RemoveFileIfPossible(m_strTempName);
	
	if (FileExists(m_strBackupName))
		RemoveFileIfPossible(m_strBackupName);
}

BOOL CFileSaver::CanSave() const
{
	CFileStatus fs;

	if (CFile::GetStatus(m_strName, fs) &&
		(fs.m_attribute & CFile::readOnly) != 0)
	{
		SetFileError(ferrCantSaveReadOnly);
		return FALSE;
	}
	
	if (m_strTempName.IsEmpty() || m_strBackupName.IsEmpty())
	{
		SetFileError(ferrCantSaveTempFiles);
		return FALSE;
	}
	
	return TRUE;
}

BOOL CFileSaver::MakeWriteable() const
{
	CFileStatus fs;

	if (CFile::GetStatus(m_strName, fs))
	{
		if ((fs.m_attribute & CFile::readOnly) == 0)
			return TRUE;
		else
		{
			fs.m_attribute &= ~CFile::readOnly;
			TRY
			{
				CFile::SetStatus(m_strName, fs);
				return TRUE;
			}
			CATCH_ALL(e)
			{
				return FALSE;
			}
			END_CATCH_ALL
		}
	}

	return FALSE;
}

BOOL CFileSaver::Finish()
{
	// delete 0-length place-saver file first
	RemoveFileIfPossible(m_strBackupName);

	if (FileExists(m_strName))
	{
		if (!RenameFile(m_strName, m_strBackupName))
		{
			// If we weren't able to rename the file, at least
			// give some indication of this fact.
			FileErrorMessageBox(IDS_RENAME_FAILED, m_strName);
			RemoveFileIfPossible(m_strTempName);
			return FALSE;
		}
	}
	else
	{
		// no backup was made since the "original" didn't exists,
		// wipe out the name so we don't delete the file later...	
		m_strBackupName.Empty();
	}
	
	if (!RenameFile(m_strTempName, m_strName))
	{
		if (!m_strBackupName.IsEmpty() &&
			RenameFile(m_strBackupName, m_strName))
		{
			RemoveFileIfPossible(m_strTempName);
		}
		
		return FALSE;
	}
	
	if (!m_strBackupName.IsEmpty())
		RemoveFileIfPossible(m_strBackupName);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// File Utilities

BOOL RenameFile(LPCTSTR szOldName, LPCTSTR szNewName)
{
    TRACE2("RenameFile: \"%s\" to \"%s\"\n", szOldName, szNewName);
    
    TRY
    {
        if (FileExists(szNewName))
            CFile::Remove(szNewName);
        CFile::Rename(szOldName, szNewName);
    }
    CATCH(CFileException, e)
    {
        TRACE("Rename failed!\n");
        SetFileError(e->m_cause);
        return FALSE;
    }
    END_CATCH
    
    return TRUE;
}

void RemoveFileIfPossible(LPCTSTR szFileName)
{
	TRY
	{
		CFile::Remove(szFileName);
	}
	CATCH_ALL(e)
	{
		// Just silently ignore if we cant remove the file.
	}
	END_CATCH_ALL
}

///////////////////////////////////////////////////////////////////////////////
//	File Error support

static int g_nFileErrorCause = CFileException::none; // from CFileException::m_cause

int GetFileError()
{
	return g_nFileErrorCause;
}

void SetFileError(int nCause)
{
	g_nFileErrorCause = nCause;

	// Mirror the error for the automation object.	ShellOM:State
	g_pAutomationState->SetFileError(nCause) ;
}

// Display a message box informing the user of a file related exception.
// The format of the box is something like:
//
//     <file name>
//     <operation failed>
//     <reason>
//
// <file name> describes what file has the problem, <operation files>
// indicated what kind of thing failed (e.g. "Cannot save file"), and
// <reason> provides more information about why the operation failed
// (e.g. "Disk full").
//
// The <reason> parameter must have been setup previously via a call to
// SetFileError().

void FileErrorMessageBox(unsigned int idsOperation, LPCTSTR szFileName)
{
	// Only display and clear the error if an automation macro is not running.
	if (g_pAutomationState->DisplayUI()) // ShellOM::Save
	{

		if (g_nFileErrorCause > 0)
		{
			CString strOperation;
			VERIFY(strOperation.LoadString(idsOperation));

			CString strReason;
			VERIFY(strReason.LoadString(theApp.IdsFromFerr(g_nFileErrorCause)));

			CString strMsg;
			MsgBox(Error, MsgText(strMsg, IDS_ERROR_FILE_TEMPLATE,
        		szFileName, (LPCTSTR)strOperation, (LPCTSTR)strReason));
		}
    
		SetFileError(CFileException::none) ;
	}
}

