/*
 *
 * error.cpp
 *
 * deal with errors
 *
 */

#include "precomp.hxx"
#include "xbfileres.h"
#include <stdarg.h>

DWORD rgidsPrefix[] = {
    IDS_ERROR,
    IDS_WARNING
};

void SetIdpIds(DWORD idp, DWORD ids)
{
    rgidsPrefix[idp] = ids;
}

static void FillError(LPTSTR szBuf, LPCTSTR szError)
{
	if(szError)
		_tcscat(szBuf, szError);
	else
		LoadString(GetModuleHandle(NULL), IDS_UNEXPECTED,
			szBuf + _tcslen(szBuf), 768);
}

BOOL FDoFmtIds(DWORD ids, LPTSTR szError, BOOL fPrefix, BOOL fFailSafe,
	LPCTSTR szFailSafe, va_list val)
{
	TCHAR szRes[1024];
	BOOL fRet;
	TCHAR *rgpsz[16];
	int csz = 0;

	if(!LoadString(GetModuleHandle(NULL), ids, szRes, sizeof szRes /
			sizeof(TCHAR)))
		ids = 0;
	if(ids == 0) {
		if(fFailSafe) {
			FillError(szError, szFailSafe);
			fRet = TRUE;
		} else
			fRet = FALSE;
	} else {
		TCHAR *pchIn = szRes;
		TCHAR *pchOut = szError;

		if(fPrefix) {
			/* Skip the "error:" prefix */
			while(*pchOut)
				++pchOut;
		}

		while(*pchIn) {
			if(*pchIn == '|') {
				/* Parameter */
				int isz = 0;

				++pchIn;
				while(_istdigit(*pchIn)) {
					isz = isz * 10 + (*pchIn - '0');
					++pchIn;
				}
				if(isz < 16) {
					/* Fetch our string argument */
					while(csz <= isz)
						rgpsz[csz++] = va_arg(val, TCHAR *);

					TCHAR *pchArg = rgpsz[isz];
					while(*pchArg)
						*pchOut++ = *pchArg++;
				}
			} else
				*pchOut++ = *pchIn++;
		}
		*pchOut = 0;
		fRet = TRUE;
	}

	return fRet;
}

BOOL FFmtIds(DWORD ids, LPTSTR lpsz, ...)
{
	va_list val;

	va_start(val, lpsz);
	BOOL fRet = FDoFmtIds(ids, lpsz, FALSE, FALSE, NULL, val);
	va_end(val);
	return fRet;
}

void ShowError(HRESULT hr, DWORD ids, DWORD idsPrefix, va_list val)
{
	TCHAR szError[1024];
	TCHAR szRes[1024];
	TCHAR szDmError[256];
    HRESULT hrTranslate;

    hrTranslate = DmTranslateError(hr, szDmError, sizeof(szDmError)/sizeof(TCHAR));
    if(FAILED(hrTranslate))
    {
        _stprintf(szDmError, TEXT("ERR:0x%0.8x(%d)"), hr, hr);
    }

	if(!LoadString(GetModuleHandle(NULL), idsPrefix, szError, sizeof szError
			/ sizeof(TCHAR)))
		szError[0] = 0;

	/* Some errors always get default strings */
	switch(hr) {
	case XBDM_CANNOTCONNECT:
	case XBDM_CONNECTIONLOST:
		FillError(szError, szDmError);
		_fputts(szError, stderr);
		_fputtc('\n', stderr);
		exit(1);
	}

	/* If no resource was provided, let's see if we can cook up a string
	 * ourselves */
	if(ids == 0) {
		switch(hr) {
		case XBDM_FILEERROR:
			ids = IDS_FILEERR;
			break;
		case XBDM_NOSUCHFILE:
			ids = IDS_NOFILE;
			break;
		case XBDM_ALREADYEXISTS:
			ids = IDS_EXISTS;
			break;
		case XBDM_DIRNOTEMPTY:
			ids = IDS_DIRNOTEMPTY;
			break;
		case XBDM_BADFILENAME:
			ids = IDS_BADFILENAME;
			break;
		case XBDM_CANNOTCREATE:
			ids = IDS_CANNOTCREATE;
			break;
		case XBDM_CANNOTACCESS:
			ids = IDS_CANNOTACCESS;
			break;
		case XBDM_DEVICEFULL:
			ids = IDS_DISKFULL;
			break;
		}
	}

	/* The rest of the errors get default strings if no resource was provided,
	 * or a string with optional parameters if we do have a resource */
	FDoFmtIds(ids, szError, TRUE, TRUE, szDmError, val);

	_fputts(szError, stderr);
	_fputtc('\n', stderr);
}

void Warning(HRESULT hr, DWORD ids, ...)
{
	va_list val;

	va_start(val, ids);
	ShowError(hr, ids, rgidsPrefix[IDP_WARNING], val);
	va_end(val);
}

void Fatal(HRESULT hr, DWORD ids, ...)
{
	va_list val;

	va_start(val, ids);
	ShowError(hr, ids, rgidsPrefix[IDP_ERROR], val);
	va_end(val);

	// clean up
	DmUseSharedConnection(FALSE);
	exit(1);
}

void FIL::Fatal(HRESULT hr, DWORD ids) const
{
	TCHAR sz[1024];
	FillDisplayName(sz);
	::Fatal(hr, ids, sz);
}

void FIL::Warning(HRESULT hr, DWORD ids) const
{
	TCHAR sz[1024];
	FillDisplayName(sz);
	::Warning(hr, ids, sz);
}
