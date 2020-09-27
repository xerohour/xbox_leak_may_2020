/////////////////////////////////////////////////////////////////////////////
// applog.cpp
//
// email	date		change
// briancr	02/06/95	created
//
// copyright 1994 Microsoft

// Implementation of the application logging system

#include "stdafx.h"
#include <io.h>
#include "applog.h"

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CAppLog

// global declaration, since we want access to this object throughout CAFE
CAppLog applog;

const DWORD CAppLog::m_dwMaxLength = 30000;

CAppLog::CAppLog(void)
{
	char acFilename[MAX_PATH];
	char acDrive[_MAX_DRIVE];
	char acDir[_MAX_DIR];
	char acFname[_MAX_FNAME];
	UINT nOpenFlags;
	CFileStatus status;

	// get the filename of the current application
	if (::GetModuleFileName(NULL, acFilename, MAX_PATH)) {
		_splitpath(acFilename, acDrive, acDir, acFname, NULL);
		
		m_strFilename = CString(acDrive) + CString(acDir) + CString(acFname) + ".log";

		// define open mode for the file
		nOpenFlags = CFile::modeReadWrite | CFile::modeNoInherit | CFile::typeText | CFile::shareDenyWrite;
		// if the file doesn't exist, create it (MFC doesn't do this for us)
		if (!CFile::GetStatus(m_strFilename, status)) {
			nOpenFlags |= CFile::modeCreate;
		}

		// open the file
		if (m_file.Open(m_strFilename, nOpenFlags)) {
			// if the file is too big, truncate it
			if (m_file.GetLength() > m_dwMaxLength) {
				m_file.SetLength(0);
				TRACE("AppLog::AppLog: truncating the application log file.");
			}
			// move the file pointer to the end of the file
			m_file.SeekToEnd();
		}
		else {
			TRACE("AppLog::AppLog: unable to open the application log (%d), not recording.", ::GetLastError());
		}
	}
	else {
		TRACE("AppLog::AppLog: unable to get a valid filename for the application log, not recording.");
	}

	// initialize the critical section object we'll use to make this thread safe
	::InitializeCriticalSection(&m_critsec);
}

CAppLog::~CAppLog()
{
	// be sure the file is open
	if (m_file.m_pStream) {
		// close the file
		m_file.Close();
	}

	// delete the critical section object
	::DeleteCriticalSection(&m_critsec);
}

BOOL CAppLog::Write(LPCSTR szFormat, ...)
{
	// make this thread safe by only allowing one thread to write at a time
	EnterCriticalSection(&m_critsec);

	// parse var args
	const ccBuf = 4096;
	char acBuf[ccBuf];
	va_list marker;
	va_start(marker, szFormat);
	_vsnprintf(acBuf, ccBuf, szFormat, marker);
	va_end(marker);

	// append \n
	strcat(acBuf, "\n");

	// be sure the file is open
	if (m_file.m_pStream) {

		// write the resulting string to the log
		m_file.WriteString(acBuf);
		m_file.Flush();
	}

	TRACE("CAFE Log message: %s", acBuf);

	// we're done being thread safe
	LeaveCriticalSection(&m_critsec);

	return TRUE;
}

BOOL CAppLog::Read(LPSTR pacBuf, DWORD* pccBuf)
{
	DWORD ccBuf = 0;
	DWORD ccLine;
	int nResult = TRUE;

	// make this thread safe by only allowing one thread to write at a time
	EnterCriticalSection(&m_critsec);

	// the pointer to the buffer must not be NULL
	ASSERT(pacBuf);

	// be sure the file is open
	if (m_file.m_pStream) {

		// fill the buffer with each line from the file
		const ccLineBuf = 1024;
		char acLine[ccLineBuf];

		// start at the beginning of the file
		m_file.SeekToBegin();
		while (m_file.ReadString(acLine, ccLineBuf-2)) {
			// get the length of the line
			ccLine = strlen(acLine);
			// end the line with \r\n (instead of just \n)
			acLine[ccLine-1] = '\r';
			acLine[ccLine] = '\n';
			acLine[ccLine+1] = '\0';
			// get the length of the line
			ccLine = strlen(acLine);
			// will this line fit in the buffer?
			if (ccBuf+ccLine <= *pccBuf) {
				// copy this line into the buffer
				strcat(pacBuf, acLine);
				ccBuf += ccLine;
				pacBuf += ccLine;
			}
			else {
				// the buffer's too small
				*pccBuf = m_file.GetLength();
				nResult = FALSE;
				break;
			}
		}
		// reset the file pointer to the end of the file
		m_file.SeekToEnd();
	}
	else {
		nResult = FALSE;
	}

	// we're done being thread safe
	LeaveCriticalSection(&m_critsec);

	return nResult;
}

BOOL CAppLog::Clear(void)
{
	// make this thread safe by only allowing one thread to write at a time
	EnterCriticalSection(&m_critsec);

	// be sure the file is open
	if (m_file.m_pStream) {
		// set the length to zero
		m_file.SetLength(0);
		// position the file pointer
		m_file.SeekToEnd();
	}

	// we're done being thread safe
	LeaveCriticalSection(&m_critsec);

	return TRUE;
}

