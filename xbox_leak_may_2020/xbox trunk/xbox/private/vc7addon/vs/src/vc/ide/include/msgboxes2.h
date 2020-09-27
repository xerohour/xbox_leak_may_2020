#ifndef __MSGBOXES2_H__
#define __MSGBOXES2_H__

#include <tchar.h>
#include <stdarg.h>	// for var args stuff.
#include "path2.h"

#ifndef MAX_SUBSTITUTION_BYTES
#define MAX_SUBSTITUTION_BYTES	1024
#endif

#ifndef _WIN32
#ifndef LPCTSTR
#define LPCTSTR LPCSTR
#endif
#ifndef LPTSTR
#define LPTSTR LPSTR
#endif
#endif

LPCTSTR VCMsgText(CVCString &strBuffer, const CVCPath &pathOffendingFile, LPCTSTR pszOperation);
LPCTSTR VVCMsgText(CVCString &strBuffer, const int idsMsgFormat, va_list VarArguments);
LPCTSTR VCMsgText(CVCString &strBuffer, const int idsMsgFormat, ...);
LPCTSTR VCMsgText(CVCString &strBuffer, const CVCPath &pathOffendingFile, const int idsFileOperation);

LPCTSTR VCSubstituteBytes(CVCString &strBuffer, LPCTSTR pszFormat, va_list VarArguments);

LPCOLESTR VCMsgTextW(CStringW &strBuffer, const CPathW &pathOffendingFile, LPCOLESTR pszOperation);
LPCOLESTR VVCMsgTextW(CStringW &strBuffer, const int idsMsgFormat, va_list VarArguments);
LPCOLESTR VCMsgTextW(CStringW &strBuffer, const int idsMsgFormat, ...);
LPCOLESTR VCMsgTextW(CStringW &strBuffer, const CPathW &pathOffendingFile, const int idsFileOperation);

LPCOLESTR VCSubstituteBytesW(CStringW &strBuffer, LPCOLESTR pszFormat, va_list VarArguments);

#include "msgboxes2.inl"

#endif	// __MSGBOXES2_H__



