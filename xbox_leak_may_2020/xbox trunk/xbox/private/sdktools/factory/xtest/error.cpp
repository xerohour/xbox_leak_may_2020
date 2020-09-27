// error.cpp

#include "stdafx.h"

WORD CError::Report (WORD code, LPCTSTR format, ...)
{
	m_lastXmtaError = code;

	va_list argptr;
	va_start (argptr, format);

	_TCHAR message [2048];
	_vsntprintf (message, 2048, format, argptr);

	g_dispatch.ErrorMessage (NULL, code, message);

	return code;
}

CGetLastError::CGetLastError ()
{
#if defined (_XBOX)
	m_lpMsgBuf = LocalAlloc (LMEM_FIXED, 32);
	_stprintf ((unsigned short *)m_lpMsgBuf, _T("GetLastError returned 0x%x"), GetLastError());
#else
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &m_lpMsgBuf,
		0,
		NULL 
	);
#endif
}

CGetLastError::~CGetLastError () 
{
	LocalFree (m_lpMsgBuf);
}

