/////////////////////////////////////////////////////////////////////////////
// applog.h
//
// email	date		change
// briancr	02/06/95	created
//
// copyright 1994 Microsoft

// Interface of the application logging system

#ifndef __APPLOG_H__
#define __APPLOG_H__

#include "targxprt.h"

/////////////////////////////////////////////////////////////////////////////
// CAppLog

class TARGET_CLASS CAppLog
{
// ctor, dtor
public:
	CAppLog(void);
	~CAppLog();

// operations
public:
	BOOL Write(LPCSTR szFormat, ...);
	BOOL Read(LPSTR acBuf, DWORD* pccBuf);
	BOOL Clear(void);

// data
protected:
	CString m_strFilename;
	CStdioFile m_file;

	static const DWORD m_dwMaxLength;

	// critical section object to make access to this object thread safe
	CRITICAL_SECTION m_critsec;
};

// global declaration, since we want access to this object throughout CAFE
extern TARGET_DATA CAppLog applog;

#define APPLOG applog.Write

#endif // __APPLOG_H__
