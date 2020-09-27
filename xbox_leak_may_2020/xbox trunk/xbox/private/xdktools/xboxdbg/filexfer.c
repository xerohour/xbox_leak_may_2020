/*
**
** filexfer.c
**
** File transfer utilities
**
*/

#include "precomp.h"
#include "mswsock.h"

//#define ERRORTRACK

HRESULT HrGetSystemTime(SCI *psci, LPSYSTEMTIME lpSysTime)
{
	HRESULT hr;
	char sz[256];
	DWORD cch;
	FILETIME ft;
    PDM_CONNECTION s;

    hr = HrDoOpenSharedConnection(psci, &s);
    if(FAILED(hr))
        return hr;
	cch = sizeof sz;
	hr = DmSendCommand(s, "SYSTIME", sz, &cch);
	if(SUCCEEDED(hr)) {
		if(FGetDwParam(sz, "high", &ft.dwHighDateTime) &&
				FGetDwParam(sz, "low", &ft.dwLowDateTime))
			FileTimeToSystemTime(&ft, lpSysTime);
		else
			hr = XBDM_UNDEFINED;
	}
    DoCloseSharedConnection(psci, s);
	return hr;
}

void SetTimeCorrection(SCI *psci)
{
	ULARGE_INTEGER liLocal;
	ULARGE_INTEGER liRemote;
	ULARGE_INTEGER liHalfHour;
	ULARGE_INTEGER liHour;
	SYSTEMTIME st;
    FILETIME ft;
	HRESULT hr;
	BOOL fAhead;
    char sz[128];

	if(psci->fGotTimeCorrection)
		return;
	hr = HrGetSystemTime(psci, &st);
    if(hr == XBDM_CLOCKNOTSET) {
        /* Try to synchronize the remote clock with the local clock */
        GetSystemTimeAsFileTime(&ft);
        sprintf(sz, "setsystime clockhi=0x%08x clocklo=0x%08x",
            ft.dwHighDateTime, ft.dwLowDateTime);
        DmSendCommand(NULL, sz, NULL, NULL);
        hr = HrGetSystemTime(psci, &st);
    }
    if(FAILED(hr)) {
        if(hr == XBDM_CLOCKNOTSET)
            /* The box is there but refuses to tell us its time, probably
             * because the time is bogus */
            psci->fGotTimeCorrection = psci->fBadSysTime = TRUE;
		return;
    }
	if(!SystemTimeToFileTime(&st, (LPFILETIME)&liRemote))
		return;
	GetSystemTimeAsFileTime((LPFILETIME)&liLocal);
	/* liHalfHour is one half hour's worth of file time, of 1800 seconds worth
	 * of 100-nanosecond intervals (= 1.8x10^10) */
	liHalfHour.LowPart = 820130816;
	liHalfHour.HighPart = 4;
	liHour.LowPart = 1640261632;
	liHour.HighPart = 8;
	/* Find the difference between the two clocks, rounded to the nearest
	 * hour */
	if(liLocal.QuadPart > liRemote.QuadPart) {
		psci->fAddDiff = FALSE;
		psci->liTimeDiff.QuadPart = liLocal.QuadPart - liRemote.QuadPart;
	} else {
		psci->fAddDiff = TRUE;
		psci->liTimeDiff.QuadPart = liRemote.QuadPart - liLocal.QuadPart;
	}
	if(psci->liTimeDiff.QuadPart < liHalfHour.QuadPart)
		/* Zero difference */
		psci->liTimeDiff.LowPart = psci->liTimeDiff.HighPart = 0;
	else {
		psci->liTimeDiff.QuadPart += liHalfHour.QuadPart;
		psci->liTimeDiff.QuadPart /= liHour.QuadPart;
		psci->liTimeDiff.QuadPart *= liHour.QuadPart;
	}

	psci->fGotTimeCorrection = TRUE;
}

#ifdef ERRORTRACK
void TrackError(PDM_CONNECTION s, LPCSTR szLocalName, LPCSTR szRemoteName,
    int nErr)
{
    HANDLE hFile;
    int cRetries = 4;

    while(cRetries--) {
        hFile = CreateFile("\\\\xbox\\usr\\jlange\\fileerr.txt",
            GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
        if(hFile != INVALID_HANDLE_VALUE) {
            /* Seek to EOF and write our information */
            if(INVALID_SET_FILE_POINTER != SetFilePointer(hFile, 0, NULL,
                FILE_END))
            {
                char sz[2048];
                DWORD cb;

                sprintf(sz, "%s: %s, %s (%d)\r\n", s->psci->szXboxName,
                    szLocalName, szRemoteName, nErr);
                WriteFile(hFile, sz, strlen(sz), &cb, NULL);
            }
            CloseHandle(hFile);
            return;
        }
        if(GetLastError() != ERROR_SHARING_VIOLATION)
            return;
    }
}
#endif

HRESULT HrSendFile(PDM_CONNECTION s, LPCSTR szLocalName, LPCSTR szRemoteName)
{
	HANDLE h;
	DWORD dwFileSize;
	HRESULT hr;
	char szCommand[1024];
	DWORD cbBuf, cbSend;
	BYTE *pbBuf;

    if (szLocalName == NULL || szRemoteName == NULL)
        return E_INVALIDARG;

    if(strlen(szRemoteName) > 990)
		return E_INVALIDARG;

	h = CreateFile(szLocalName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(INVALID_HANDLE_VALUE == h)
		return XBDM_FILEERROR;
	dwFileSize = GetFileSize(h, NULL);
	if(dwFileSize == -1) {
		hr = XBDM_FILEERROR;
		goto error;
	}
	sprintf(szCommand, "SENDFILE NAME=\"%s\" LENGTH=0x%x", szRemoteName, dwFileSize);
	hr = DmSendCommand(s, szCommand, NULL, NULL);
	if(FAILED(hr)) {
error:
		CloseHandle(h);
		return hr;
	}

    if (!TransmitFile(s->s, h, dwFileSize, 4096, NULL, NULL, 0)) {
#ifdef ERRORTRACK
        /* Track down errors */
        TrackError(s, szLocalName, szRemoteName, WSAGetLastError());
#endif
        /* Connection lost */
        TerminateConnection(s);
        hr = XBDM_CONNECTIONLOST;
        goto error;
    }

	hr = DmReceiveStatusResponse(s, NULL, NULL);
	CloseHandle(h);
	return hr;
}

HRESULT HrReceiveFile(PDM_CONNECTION s, LPCSTR szLocalName, LPCSTR szRemoteName)
{
	HANDLE h;
	DWORD dwFileSize;
	HRESULT hr;
	char szCommand[1024];
	DWORD cbBuf;
	BYTE *pbBuf;
    BYTE *rgbBuf;

    if (szRemoteName == NULL || szLocalName == NULL)
        return E_INVALIDARG;

	if(strlen(szRemoteName) > 990)
		return E_INVALIDARG;

    rgbBuf = LocalAlloc(LMEM_FIXED, 8192);
    if(!rgbBuf)
        return E_OUTOFMEMORY;

	sprintf(szCommand, "GETFILE NAME=\"%s\"", szRemoteName);
	hr = DmSendCommand(s, szCommand, NULL, NULL);
	if(FAILED(hr))
		goto done;
	/* Get the file size */
	hr = DmReceiveBinary(s, &dwFileSize, sizeof(DWORD), NULL);
	if(FAILED(hr))
		goto done;

	h = CreateFile(szLocalName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
    if(INVALID_HANDLE_VALUE == h) {
		hr = XBDM_FILEERROR;
        goto done;
    }

	/* We're ready to receive.  We'll read up to 8k before writing */
	while(dwFileSize) {
		cbBuf = 8192;
		if(cbBuf > dwFileSize)
			cbBuf = dwFileSize;
		hr = DmReceiveBinary(s, rgbBuf, cbBuf, NULL);
		if(FAILED(hr)) {
error:
			CloseHandle(h);
			DeleteFile(szLocalName);
			goto done;
		}
		dwFileSize -= cbBuf;
		pbBuf = rgbBuf;
		while(cbBuf) {
			int cb;

			if(!WriteFile(h, pbBuf, cbBuf, &cb, NULL)) {
				hr = XBDM_FILEERROR;				
				goto error;
			}
			cbBuf -= cb;
			pbBuf += cb;
		}
	}
	CloseHandle(h);
	hr = XBDM_NOERR;
done:
    LocalFree(rgbBuf);
    return hr;
}

void CorrectTime(SCI *psci, LPFILETIME lpft, BOOL fTo)
{
	if(lpft->dwLowDateTime || lpft->dwHighDateTime) {
		SetTimeCorrection(psci);
		if(psci->fGotTimeCorrection && !psci->fBadSysTime) {
			/* The time correction we have is to add if fAdd is true and we're
			 * converting local to remote, so we need to figure out the real
			 * sense */
			BOOL fAdd = !psci->fAddDiff == !fTo;
			if(fAdd)
				((PULARGE_INTEGER)lpft)->QuadPart += psci->liTimeDiff.QuadPart;
			else
				((PULARGE_INTEGER)lpft)->QuadPart -= psci->liTimeDiff.QuadPart;
		} else
			lpft->dwLowDateTime = lpft->dwHighDateTime = 0;
	}
}

void UnpackDmfa(SCI *psci, LPSTR szBuf, PDM_FILE_ATTRIBUTES pdmfa)
{
    if(PchGetParam(szBuf, "directory", FALSE, TRUE))
        pdmfa->Attributes |= FILE_ATTRIBUTE_DIRECTORY;
    if(PchGetParam(szBuf, "readonly", FALSE, TRUE))
        pdmfa->Attributes |= FILE_ATTRIBUTE_READONLY;
    if(PchGetParam(szBuf, "hidden", FALSE, TRUE))
        pdmfa->Attributes |= FILE_ATTRIBUTE_HIDDEN;
    FGetDwParam(szBuf, "createhi", &pdmfa->CreationTime.dwHighDateTime);
    FGetDwParam(szBuf, "createlo", &pdmfa->CreationTime.dwLowDateTime);
    FGetDwParam(szBuf, "changehi", &pdmfa->ChangeTime.dwHighDateTime);
    FGetDwParam(szBuf, "changelo", &pdmfa->ChangeTime.dwLowDateTime);
    FGetDwParam(szBuf, "sizehi", &pdmfa->SizeHigh);
    FGetDwParam(szBuf, "sizelo", &pdmfa->SizeLow);
    FGetSzParam(szBuf, "name", pdmfa->Name);
    CorrectTime(psci, &pdmfa->CreationTime, FALSE);
    CorrectTime(psci, &pdmfa->ChangeTime, FALSE);
}

HRESULT HrGetFileAttributes(SCI *psci, LPCSTR szFile, PDM_FILE_ATTRIBUTES pdm)
{
	HRESULT hr;
	char szBuf[1024];
    PDM_CONNECTION s;

    if (!szFile || !pdm)
        return E_INVALIDARG;

	if(strlen(szFile) > 980)
		return E_INVALIDARG;
    hr = HrDoOpenSharedConnection(psci, &s);
    if(FAILED(hr))
        return hr;
	sprintf(szBuf, "GETFILEATTRIBUTES NAME=\"%s\"", szFile);
	hr = DmSendCommand(s, szBuf, NULL, NULL);
	if(hr != XBDM_MULTIRESPONSE) {
        if(SUCCEEDED(hr)) {
			hr = E_UNEXPECTED;
            TerminateConnection(s);
        }
        DoCloseSharedConnection(psci, s);
	} else {
		memset(pdm, 0, sizeof *pdm);
		for(;;) {
			DWORD cch = sizeof szBuf;

			hr = DmReceiveSocketLine(s, szBuf, &cch);
			if(FAILED(hr) || *szBuf == '.')
				break;
            UnpackDmfa(psci, szBuf, pdm);
		}
        DoCloseSharedConnection(psci, s);
	}
	return hr;
}

HRESULT HrSetFileAttributes(SCI *psci, LPCSTR szFile, PDM_FILE_ATTRIBUTES pdm)
{
	HRESULT hr;
	char szBuf[1024];
    FILETIME ftCreate;
    FILETIME ftChange;
    int ich;
    PDM_CONNECTION s;

    if (!szFile || !pdm)
        return E_INVALIDARG;

	if(strlen(szFile) > 900)
		return E_INVALIDARG;
    /* Copy the filetimes so we don't change the input attributes */
    ftCreate = pdm->CreationTime;
    ftChange = pdm->ChangeTime;
    CorrectTime(psci, &ftCreate, TRUE);
    CorrectTime(psci, &ftChange, TRUE);

	sprintf(szBuf, "SETFILEATTRIBUTES NAME=\"%s\""
		" CREATEHI=0x%08x CREATELO=0x%08x CHANGEHI=0x%08x CHANGELO=0x%08x",
        szFile, ftCreate.dwHighDateTime,
		ftCreate.dwLowDateTime, ftChange.dwHighDateTime,
		ftChange.dwLowDateTime);
    ich = strlen(szBuf);
    if(pdm->Attributes) {
        sprintf(szBuf + ich, " READONLY=%d HIDDEN=%d",
		    (pdm->Attributes & FILE_ATTRIBUTE_READONLY ? 1 : 0),
		    (pdm->Attributes & FILE_ATTRIBUTE_HIDDEN ? 1 : 0));
    }
    hr = HrDoOpenSharedConnection(psci, &s);
    if(FAILED(hr))
        return hr;
	hr = HrDoOneLineCmd(s, szBuf);
    DoCloseSharedConnection(psci, s);
    return hr;
}
 
HRESULT HrMkdir(PDM_CONNECTION s, LPCSTR szName)
{
	char szBuf[1024];

    if (szName == NULL)
        return E_INVALIDARG;

    if(strlen(szName) > 990)
		return E_INVALIDARG;
	sprintf(szBuf, "MKDIR NAME=\"%s\"", szName);
	return HrDoOneLineCmd(s, szBuf);
}

HRESULT HrRenameFile(PDM_CONNECTION s, LPCSTR szOldName, LPCSTR szNewName)
{
	char szBuf[2048];

    if (szOldName == NULL || szNewName == NULL)
        return E_INVALIDARG;

	if(strlen(szOldName) + strlen(szNewName) > 1990)
		return E_INVALIDARG;
	sprintf(szBuf, "RENAME NAME=\"%s\" NEWNAME=\"%s\"", szOldName, szNewName);
	return HrDoOneLineCmd(s, szBuf);
}

HRESULT HrDeleteFile(PDM_CONNECTION s, LPCSTR szFileName, BOOL fIsDirectory)
{
	char szBuf[1024];

    if (szFileName == NULL)
        return E_INVALIDARG;

	if(strlen(szFileName) > 985)
		return E_INVALIDARG;
	sprintf(szBuf, "DELETE NAME=\"%s\" %s", szFileName, fIsDirectory ?
		"DIR" : "");
	return HrDoOneLineCmd(s, szBuf);
}

HRESULT HrOpenDir(SCI *psci, PDM_WALK_DIR *ppdmwd, LPCSTR szDirName,
    LPDWORD pdw)
{
    PDM_CONNECTION s;
	HRESULT hr;
	char szBuf[1024];
	struct _DM_WALKDIR_FILE *pwf, **ppwf;
    DWORD c;

    if (!ppdmwd || !szDirName)
        return E_INVALIDARG;

	/* This is our first call, so we need to build a list of all of the
	 * files returned by the remote machine */
	if(!szDirName || strlen(szDirName) > 990)
		return E_INVALIDARG;
    hr = HrDoOpenSharedConnection(psci, &s);
    if(FAILED(hr))
        return hr;
	sprintf(szBuf, "DIRLIST NAME=\"%s\"", szDirName);
	hr = DmSendCommand(s, szBuf, NULL, NULL);
	if(hr != XBDM_MULTIRESPONSE) {
        if(SUCCEEDED(hr)) {
            hr = E_UNEXPECTED;
fatal:
            TerminateConnection(s);
        }
        DoCloseSharedConnection(psci, s);
		return hr;
	}
	
	/* We've got the connection, so we can allocate a walkdir structure
	 * now */
	*ppdmwd = LocalAlloc(LMEM_FIXED, sizeof (struct _DM_WALK_DIR));
    if(!*ppdmwd) {
        hr = E_OUTOFMEMORY;
        goto fatal;
	}
	ppwf = &(*ppdmwd)->pwfFirst;
	/* Read all of the filenames and construct a buffer for them all */
	for(c = 0; ; ++c) {
        DWORD cch;
		pwf = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
            sizeof (struct _DM_WALKDIR_FILE));
		if(!pwf) {
			/* Oh no */
			hr = E_OUTOFMEMORY;
			break;
		}
        cch = sizeof szBuf;
		hr = DmReceiveSocketLine(s, szBuf, &cch);
		/* Stop if we can't read or if we're done */
		if(FAILED(hr) || (cch == 1 && szBuf[0] == '.')) {
			/* We're done now */
			LocalFree(pwf);
			break;
		}
        UnpackDmfa(psci, szBuf, &pwf->dmfa);
        if(!pwf->dmfa.Name[0]) {
            LocalFree(pwf);
            hr = E_UNEXPECTED;
            break;
        }
		*ppwf = pwf;
		ppwf = &pwf->pwfNext;
	}
	*ppwf = NULL;
    if(FAILED(hr))
        goto fatal;
    DoCloseSharedConnection(psci, s);
    if(pdw)
        *pdw = c;
    return XBDM_NOERR;
}

HRESULT HrWalkDir(SCI *psci, PDM_WALK_DIR *ppdmwd, LPCSTR szDirName,
    PDM_FILE_ATTRIBUTES pdmfa)
{
	HRESULT hr;
	char szBuf[1024];
	struct _DM_WALKDIR_FILE *pwf, **ppwf;

    if (!ppdmwd || !pdmfa)
        return E_INVALIDARG;

    if(!*ppdmwd) {
        hr = HrOpenDir(psci, ppdmwd, szDirName, NULL);
        if(FAILED(hr))
            return hr;
    }

	pwf = (*ppdmwd)->pwfFirst;
	if(!pwf)
		return XBDM_ENDOFLIST;
	(*ppdmwd)->pwfFirst = pwf->pwfNext;

    memcpy(pdmfa, &pwf->dmfa, sizeof *pdmfa);
	CorrectTime(psci, &pdmfa->CreationTime, FALSE);
	CorrectTime(psci, &pdmfa->ChangeTime, FALSE);

	LocalFree(pwf);
	return XBDM_NOERR;
}

HRESULT DmCloseDir(PDM_WALK_DIR pdmwd)
{
	struct _DM_WALKDIR_FILE *pwf;
	if(pdmwd) {
        while(pdmwd->pwfFirst) {
            pwf = pdmwd->pwfFirst;
            pdmwd->pwfFirst = pwf->pwfNext;
            LocalFree(pwf);
        }
        LocalFree(pdmwd);
    }
	return XBDM_NOERR;
}

HRESULT DmWalkLoadedModules(PDM_WALK_MODULES *ppdmwm, DMN_MODLOAD *pdmml)
{
	PDM_CONNECTION s;
	HRESULT hr;
	struct _DM_WALKMOD_MODULE *pwm, **ppwm;

	if(!ppdmwm || !pdmml)
		return E_INVALIDARG;

	if(!*ppdmwm) {
		hr = HrOpenSharedConnection(&s);
		if(FAILED(hr))
			return hr;
		hr = DmSendCommand(s, "MODULES", NULL, 0);
		if(hr != XBDM_MULTIRESPONSE) {
            if(SUCCEEDED(hr)) {
                TerminateConnection(s);
                hr = E_UNEXPECTED;
            }
            CloseSharedConnection(s);
			return hr;
		}
		/* Build up a list of the modules */
		*ppdmwm = LocalAlloc(LMEM_FIXED, sizeof(struct _DM_WALK_MODULES));
		if(!*ppdmwm) {
			CloseSharedConnection(s);
			return E_OUTOFMEMORY;
		}
		ppwm = &(*ppdmwm)->pwmFirst;
		for(;;) {
			char sz[512];
			DWORD cch = sizeof sz;
			hr = DmReceiveSocketLine(s, sz, &cch);
			if(*sz == '.' || FAILED(hr))
				break;
			pwm = LocalAlloc(LMEM_FIXED, sizeof(struct _DM_WALKMOD_MODULE));
			if(!pwm) {
				hr = E_OUTOFMEMORY;
				break;
			}
			if(!FUnpackDmml(sz, &pwm->dmml, FALSE))
				LocalFree(pwm);
			else {
				*ppwm = pwm;
				ppwm = &pwm->pwmNext;
			}
		}
		CloseSharedConnection(s);
		*ppwm = NULL;
		if(FAILED(hr)) {
			DmCloseLoadedModules(*ppdmwm);
			*ppdmwm = NULL;
		}
	}

	/* Now if we have a list, we'll return data from it */
	if(*ppdmwm) {
		pwm = (*ppdmwm)->pwmFirst;
		if(pwm) {
			(*ppdmwm)->pwmFirst = pwm->pwmNext;
			memcpy(pdmml, &pwm->dmml, sizeof pwm->dmml);
			LocalFree(pwm);
			hr = XBDM_NOERR;
		} else
			hr = XBDM_ENDOFLIST;
	}
	return hr;
}

HRESULT DmCloseLoadedModules(PDM_WALK_MODULES pdmwm)
{
	struct _DM_WALKMOD_MODULE *pwm;
	if(pdmwm) {
        while(pdmwm->pwmFirst) {
            pwm = pdmwm->pwmFirst;
            pdmwm->pwmFirst = pwm->pwmNext;
            LocalFree(pwm);
        }
        LocalFree(pdmwm);
    }
	return XBDM_NOERR;
}

HRESULT DmWalkModuleSections(PDM_WALK_MODSECT *ppdmws, LPCSTR szModule,
	DMN_SECTIONLOAD *pdmsl)
{
	PDM_CONNECTION s;
	HRESULT hr;
	struct _DM_WALKMOD_SECTION *pws, **ppws;
	char sz[256];

	if(!ppdmws || !pdmsl)
		return E_INVALIDARG;

	if(!*ppdmws) {
        if(!szModule)
            return E_INVALIDARG;
		hr = HrOpenSharedConnection(&s);
		if(FAILED(hr))
			return hr;
		sprintf(sz, "MODSECTIONS NAME=\"%s\"", szModule);
		hr = DmSendCommand(s, sz, NULL, 0);
		if(hr != XBDM_MULTIRESPONSE) {
            if(SUCCEEDED(hr)) {
                TerminateConnection(s);
                hr = E_UNEXPECTED;
            }
            CloseSharedConnection(s);
			return hr;
		}
		/* Build up a list of the modules */
		*ppdmws = LocalAlloc(LMEM_FIXED, sizeof(struct _DM_WALK_MODSECT));
		if(!*ppdmws) {
			CloseSharedConnection(s);
			return E_OUTOFMEMORY;
		}
		ppws = &(*ppdmws)->pwsFirst;
		for(;;) {
			char sz[512];
			DWORD cch = sizeof sz;
			hr = DmReceiveSocketLine(s, sz, &cch);
			if(*sz == '.' || FAILED(hr))
				break;
			pws = LocalAlloc(LMEM_FIXED, sizeof(struct _DM_WALKMOD_SECTION));
			if(!pws) {
				hr = E_OUTOFMEMORY;
				break;
			}
			if(!FUnpackDmsl(sz, &pws->dmsl, FALSE))
				LocalFree(pws);
			else {
				*ppws = pws;
				ppws = &pws->pwsNext;
			}
		}
		CloseSharedConnection(s);
		*ppws = NULL;
		if(FAILED(hr)) {
			DmCloseModuleSections(*ppdmws);
			*ppdmws = NULL;
		}
	}

	/* Now if we have a list, we'll return data from it */
	if(*ppdmws) {
		pws = (*ppdmws)->pwsFirst;
		if(pws) {
			(*ppdmws)->pwsFirst = pws->pwsNext;
			memcpy(pdmsl, &pws->dmsl, sizeof pws->dmsl);
			LocalFree(pws);
			hr = XBDM_NOERR;
		} else
			hr = XBDM_ENDOFLIST;
	}
	return hr;
}

HRESULT DmCloseModuleSections(PDM_WALK_MODSECT pdmws)
{
	struct _DM_WALKMOD_SECTION *pws;
	if(!pdmws)
		return E_INVALIDARG;
	while(pdmws->pwsFirst) {
		pws = pdmws->pwsFirst;
		pdmws->pwsFirst = pws->pwsNext;
		LocalFree(pws);
	}
	LocalFree(pdmws);
	return XBDM_NOERR;
}

HRESULT DmGetModuleLongName(LPCSTR szShort, LPSTR szLong, LPDWORD pcchLong)
{
	char sz[1024];
	PDM_CONNECTION s;
	HRESULT hr;
	DWORD cch;
	DWORD cchMax;

    if (!szLong || !pcchLong || !szShort)
        return E_INVALIDARG;

	hr = HrOpenSharedConnection(&s);
	if(FAILED(hr))
		return hr;
	sprintf(sz, "MODLONG NAME=\"%s\"", szShort);
	cch = sizeof sz;
	hr = DmSendCommand(s, sz, sz, &cch);
	CloseSharedConnection(s);
	if(FAILED(hr))
		return hr;
	cchMax = *pcchLong - 1;
	for(cch = 0; sz[5+cch] && cch < cchMax; ++cch)
		*szLong++ = sz[5+cch];
	*szLong = 0;
	*pcchLong = cch;
	return hr;
}

HRESULT HrGetXbeInfo(SCI *psci, LPCSTR szName, PDM_XBE pxbe)
{
	PDM_CONNECTION s;
	char sz[1024];
	HRESULT hr;

    if (!pxbe)
        return E_INVALIDARG;

    hr = HrDoOpenSharedConnection(psci, &s);
	if(FAILED(hr))
		return hr;
    if(szName)
	    sprintf(sz, "XBEINFO NAME=\"%s\"", szName);
    else
        strcpy(sz, "XBEINFO RUNNING");
	hr = DmSendCommand(s, sz, NULL, 0);
	if(hr != XBDM_MULTIRESPONSE) {
        if(SUCCEEDED(hr)) {
            TerminateConnection(s);
            hr = E_UNEXPECTED;
        }
        DoCloseSharedConnection(psci, s);
		return hr;
	}
	memset(pxbe, 0, sizeof *pxbe);
	for(;;) {
		DWORD cch = sizeof sz;
		hr = DmReceiveSocketLine(s, sz, &cch);
		if(FAILED(hr) || sz[0] == '.')
			break;
		FGetSzParam(sz, "name", pxbe->LaunchPath);
		FGetDwParam(sz, "timestamp", &pxbe->TimeStamp);
		FGetDwParam(sz, "checksum", &pxbe->CheckSum);
	}
	DoCloseSharedConnection(psci, s);
	return XBDM_NOERR;
}

HRESULT DmSetTitle(LPCSTR szDir, LPCSTR szName, LPCSTR szCmdLine)
{
	char sz[1024];
    char *pch;

    if (!szName)
        return E_INVALIDARG;

    sprintf(sz, "TITLE NAME=\"%s\"", szName);
    pch = sz + strlen(sz);
    if(szDir) {
        sprintf(pch, " DIR=\"%s\"", szDir);
        pch += strlen(pch);
    }

    if(szCmdLine) {
        if(pch + 10 + strlen(szCmdLine) > sz + sizeof(sz))
            return E_INVALIDARG;
        sprintf(pch, " CMDLINE=%s", szCmdLine);
    } else {
#if 0 // Commented out while Raid #4344 is further discussed.
        if(pch + 10 > sz + sizeof(sz))
            return E_INVALIDARG;
        sprintf(pch, " CMDLINE=");
#endif
    }
	return HrOneLineCmd(sz);
}

BOOL FUnpackDmml(LPCSTR sz, PDMN_MODLOAD pdmml, BOOL fCommand)
{
	memset(pdmml, 0, sizeof *pdmml);
    if(!FGetSzParam(sz, "name", pdmml->Name) ||
            !FGetDwParam(sz, "base", (DWORD*)&pdmml->BaseAddress) ||
            !FGetDwParam(sz, "size", &pdmml->Size))
        return FALSE;
	/* Get the checksum and timestamp if available */
	FGetDwParam(sz, "timestamp", &pdmml->TimeStamp);
	FGetDwParam(sz, "checksum", &pdmml->CheckSum);
    if(PchGetParam(sz, "tls", FALSE, fCommand))
        pdmml->Flags |= DMN_MODFLAG_TLS;
    if(PchGetParam(sz, "xbe", FALSE, fCommand))
        pdmml->Flags |= DMN_MODFLAG_XBE;
    return TRUE;
}

BOOL FUnpackDmsl(LPCSTR sz, PDMN_SECTIONLOAD pdmsl, BOOL fCommand)
{
    DWORD dw;
	memset(pdmsl, 0, sizeof *pdmsl);
    if(!FGetSzParam(sz, "name", pdmsl->Name) ||
	        !FGetDwParam(sz, "base", (DWORD*)&pdmsl->BaseAddress) ||
	        !FGetDwParam(sz, "size", &pdmsl->Size))
        return FALSE;
    if(FGetDwParam(sz, "index", &dw))
        pdmsl->Index = (USHORT)dw;
    if(FGetDwParam(sz, "flags", &dw))
        pdmsl->Flags = (USHORT)dw;
    return TRUE;
}

HRESULT HrGetDriveList(PDM_CONNECTION pdcon, LPSTR rgchDrives, DWORD *pcDrives)
{
    HRESULT hr;
    char sz[128];
    DWORD cch;

    cch = sizeof sz;
    hr = DmSendCommand(pdcon, "DRIVELIST", sz, &cch);
    if(hr == XBDM_NOERR) {
        if(cch >= sizeof sz - 1)
            /* Oops, didn't get enough data */
            return XBDM_BUFFER_TOO_SMALL;
        cch -= 5; // strip off "200- " prefix
        if(cch >= *pcDrives - 1)
            return XBDM_BUFFER_TOO_SMALL;
        strcpy(rgchDrives, sz + 5);
        *pcDrives = cch;
    } else {
        if(SUCCEEDED(hr)) {
            hr = E_UNEXPECTED;
            TerminateConnection(pdcon);
        }
    }

    return hr;
}

HRESULT HrGetDiskFreeSpace(SCI *psci, char *szDrive,
                           PULARGE_INTEGER pnFreeBytesAvailableToCaller,
                           PULARGE_INTEGER pnTotalNumberOfBytes,
                           PULARGE_INTEGER pnTotalNumberOfFreeBytes)
{
    char szResult[300], szCommand[100];
    PDM_CONNECTION pdcon;
    HRESULT hr;
    ULARGE_INTEGER rgliT[3];

    if (!szDrive)
        return E_INVALIDARG;
    if(!pnFreeBytesAvailableToCaller)
        pnFreeBytesAvailableToCaller = &rgliT[0];
    if(!pnTotalNumberOfBytes)
        pnTotalNumberOfBytes = &rgliT[1];
    if(!pnTotalNumberOfFreeBytes)
        pnTotalNumberOfFreeBytes = &rgliT[2];

    // Open a connection so that we can send a command across to the XBox
	hr = HrDoOpenSharedConnection(psci, &pdcon);
	if(FAILED(hr))
		return hr;

    // Create the command string
    sprintf(szCommand, "DRIVEFREESPACE NAME=\"%s\"", szDrive);

    // Send the command to the XBox.  If successful, then parse out the results
    hr = DmSendCommand(pdcon, szCommand, NULL, 0);
    if(hr != XBDM_MULTIRESPONSE) {
        if(SUCCEEDED(hr)) {
            hr = E_UNEXPECTED;
            TerminateConnection(pdcon);
        }
    } else {
        pnFreeBytesAvailableToCaller->QuadPart = pnTotalNumberOfBytes->QuadPart =
            pnTotalNumberOfFreeBytes->QuadPart = 0;

        // Receive the rest of the result
        for(;;) {
            DWORD cch = sizeof szResult;
		    hr = DmReceiveSocketLine(pdcon, szResult, &cch);
		    if(FAILED(hr) || *szResult == '.')
    		    break;

            FGetDwParam(szResult, "freetocallerlo",   &pnFreeBytesAvailableToCaller->LowPart);
            FGetDwParam(szResult, "freetocallerhi",   &pnFreeBytesAvailableToCaller->HighPart);
            FGetDwParam(szResult, "totalbyteslo",     &pnTotalNumberOfBytes->LowPart);
            FGetDwParam(szResult, "totalbyteshi",     &pnTotalNumberOfBytes->HighPart);
            FGetDwParam(szResult, "totalfreebyteslo", &pnTotalNumberOfFreeBytes->LowPart);
            FGetDwParam(szResult, "totalfreebyteshi", &pnTotalNumberOfFreeBytes->HighPart);
        }
    }

    // We no longer need our connection to the XBox, so close it.
	DoCloseSharedConnection(psci, pdcon);

    return hr;
}	

BOOL WritePixels(HANDLE hFile, BYTE *pbySource, int cBytesToWrite, int nBitCount);
DWORD dwRedMask, dwGreenMask, dwBlueMask;
DWORD dwRedShift, dwGreenShift, dwBlueUpShift;

HRESULT HrScreenShot(SCI *psci, LPCSTR filename)
{
    PDM_CONNECTION s;
    DWORD cch, cBytesRemainingToWrite;
    char str[256];
    int  bitcount;
    HANDLE hFile;
    DWORD dwTotalSize;
    DWORD dwPitch, dwHeight, dwWidth, dwFormat, dwFrameBufferSize;
    DWORD cBytesToWrite, cBytesToReceive;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    BYTE bitBuffer[8192];
    int i;
    DWORD dwRest = 0;
    HRESULT hr;
    int nbySrcPitch, nbySrcWidth;
    DWORD dwDestPitch;

    BYTE *pBufferMax = NULL;
    BYTE *pBuffer = NULL;
    int nbyLineWritten = 0;
    int nbyToWrite;

    if (filename == NULL)
        return E_INVALIDARG;

    hr = HrDoOpenSharedConnection(psci, &s);
    if(FAILED(hr))
        return hr;

    cch = 255;
    hr = DmSendCommand( s, "screenshot", str, &cch );
    if(hr != XBDM_BINRESPONSE) {
        if(SUCCEEDED(hr)) {
            TerminateConnection(s);
            hr = E_UNEXPECTED;
        }
        DoCloseSharedConnection(psci, s);
        return hr;
    }

    cch = 255;
    hr = DmReceiveSocketLine(s, str, &cch);
    if(FAILED(hr)) {
        TerminateConnection(s);
        DoCloseSharedConnection(psci, s);
        return hr;
    }

    // Get bitmap information from the returned string
    if(!FGetDwParam(str, "pitch", &dwPitch)) {
error:
        TerminateConnection(s);
        DoCloseSharedConnection(psci, s);
        return E_UNEXPECTED;
    }
    if(!FGetDwParam(str, "height", &dwHeight))
        goto error;
    if(!FGetDwParam(str, "width", &dwWidth))
        goto error;
    if(!FGetDwParam(str, "format", &dwFormat))
        goto error;
    if(!FGetDwParam(str, "framebuffersize", &dwFrameBufferSize))
        goto error;

    switch(dwFormat) {
    case 0x00000012://D3DFMT_LIN_A8R8G8B8:
        nbySrcPitch = dwPitch;
        nbySrcWidth = dwWidth * 4;
        bitcount = 32;
        break;

    case 0x0000001E://D3DFMT_LIN_X8R8G8B8:
        nbySrcPitch = dwPitch;
        nbySrcWidth = dwWidth * 4;
        bitcount = 32;
        break;

    case 0x00000011://D3DFMT_LIN_R5G6B5:    
        nbySrcPitch = dwPitch;
        nbySrcWidth = dwWidth * 2;
        dwRedMask = 0xf800; dwGreenMask = 0x07E0; dwBlueMask = 0x001F;
        dwRedShift = 8; dwGreenShift = 3; dwBlueUpShift = 3;
        bitcount = 16;
        break;

    case 0x0000001C://D3DFMT_LIN_X1R5G5B5:
        nbySrcPitch = dwPitch;
        nbySrcWidth = dwWidth * 2;
        dwRedMask = 0x7c00; dwGreenMask = 0x03E0; dwBlueMask = 0x001F;
        dwRedShift = 7; dwGreenShift = 2; dwBlueUpShift = 3;
        bitcount = 16;
        break;

    default:
        TerminateConnection(s);
        DoCloseSharedConnection(psci, s);
        return E_FAIL;
    }

    // Force destination pitch to 24bpp
    dwDestPitch = dwWidth * 3;

    // Calculate total size of the bitmap
    dwTotalSize = (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwDestPitch * dwHeight);

    // Create bitmap structure
    hFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        TerminateConnection(s);
        CloseSharedConnection(s);
        return XBDM_FILEERROR;
    }

    ZeroMemory( &bmfh, sizeof(BITMAPFILEHEADER) );
    bmfh.bfType = 'MB';
    bmfh.bfSize = dwTotalSize;
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if(!WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &cch, NULL)) {
        TerminateConnection(s);
        CloseHandle(hFile);
        return XBDM_FILEERROR;
    }

    ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = dwWidth;
    bmih.biHeight = dwHeight;
    bmih.biPlanes = 1;
    bmih.biBitCount = 24;
    if(!WriteFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &cch, NULL)) {
        TerminateConnection(s);
        CloseHandle(hFile);
        return XBDM_FILEERROR;
    }

    if(SetFilePointer(hFile, dwTotalSize-dwDestPitch, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        TerminateConnection(s);
        CloseHandle(hFile);
        return XBDM_FILEERROR;
    }

    // Read all of bitmap data
    while(dwFrameBufferSize) {

        // Read the next chunk from the frame buffer if needed.
        if (pBuffer >= pBufferMax) {
            cBytesToReceive = min(dwFrameBufferSize, 8192);

            hr = DmReceiveBinary(s, bitBuffer, cBytesToReceive, &cBytesRemainingToWrite);
            if(FAILED(hr)) {
                CloseHandle(hFile);
                TerminateConnection(s);
                DoCloseSharedConnection(psci, s);
                return hr;
            }

            pBuffer = bitBuffer;
            pBufferMax = pBuffer + cBytesToReceive;
        }

        if (nbyLineWritten < nbySrcWidth)
        {
            nbyToWrite = min(nbySrcWidth - nbyLineWritten, pBufferMax - pBuffer);

            if(!WritePixels(hFile, pBuffer, nbyToWrite, bitcount)) {
                CloseHandle(hFile);
                TerminateConnection(s);
                DoCloseSharedConnection(psci, s);
                return XBDM_FILEERROR;
            }

            pBuffer += nbyToWrite;
            nbyLineWritten += nbyToWrite;
            dwFrameBufferSize -= nbyToWrite;
        }
        else if (nbyLineWritten < nbySrcPitch)
        {
            nbyToWrite = min(nbySrcPitch - nbyLineWritten, pBufferMax - pBuffer);

            pBuffer += nbyToWrite;
            nbyLineWritten += nbyToWrite;
            dwFrameBufferSize -= nbyToWrite;
        }
        else
        {
            SetFilePointer(hFile, -(int)dwDestPitch*2, NULL, FILE_CURRENT);
            nbyLineWritten = 0;
        }
    }

    CloseHandle(hFile);
    DoCloseSharedConnection(psci, s);
    return XBDM_NOERR;
}

BOOL WritePixels(HANDLE hFile, BYTE *pbySource, int cBytesToWrite, int nBitCount)
{
    DWORD dwWritten, i, cPixels;
    BYTE byTemp[8192], *pbyTemp = byTemp;

    // Convert each pixel to full RGB quad.  The following is taken from the
    // BITMAPINFO section in MSDN:
    //  Note:  The bmiColors member should not contain palette indexes if the
    //         bitmap is to be stored in a file or transferred to another
    //         application.  Unless the application has exclusive use and
    //         control of the bitmap, the bitmap color table should contain
    //         explicit RGB values. 
    // Thus, we up-convert as necessary to create RGBQUADs.
    if (nBitCount == 32)
    {
        // 32bit; convert to 24bpp
        cPixels = cBytesToWrite/4;
        for (i = 0; i < cPixels; i++)
        {
            *pbyTemp++ = *pbySource++;
            *pbyTemp++ = *pbySource++;
            *pbyTemp++ = *pbySource++;
            pbySource++;    // Skip the alpha channel
        }
    }
    else
    {
        // 16 bit.  Convert as necessary based on bitmasks
        WORD *pwSource = (WORD*)pbySource;
        cPixels = cBytesToWrite/2;

        for (i = 0; i < cPixels; i++)
        {
            *pbyTemp++ = (BYTE)(((*pwSource) & dwBlueMask)  << dwBlueUpShift);
            *pbyTemp++ = (BYTE)(((*pwSource) & dwGreenMask) >> dwGreenShift);
            *pbyTemp++ = (BYTE)(((*pwSource) & dwRedMask)   >> dwRedShift);
            pwSource++;
        }
    }       

    // Write out the 24bit pixel values to the bmp file
    return WriteFile(hFile, &byTemp, cPixels*3, &dwWritten, NULL);
}

HRESULT DmMonitor(LPVOID bitBuffer, LPDWORD lpFrameBufferSize, LPDWORD lpWidth, LPDWORD lpHeight, LPDWORD lpFormat)
{
    PDM_CONNECTION s;
    DWORD cch, dwFrameBufferSize;
    char str[256];
    int cBytesRead = 0;
    HRESULT hr;

    hr = HrOpenSharedConnection(&s);
    if(FAILED(hr))
        return E_FAIL;

    hr = DmSendCommand( s, "screenshot", str, &cch );
    if(hr != XBDM_BINRESPONSE) {
        if(SUCCEEDED(hr)) {
            TerminateConnection(s);
            hr = E_UNEXPECTED;
        }
        CloseSharedConnection(s);
        return hr;
    }

    cch = 255;
    hr = DmReceiveSocketLine(s, str, &cch);
    if(FAILED(hr)) {
        TerminateConnection(s);
        CloseSharedConnection(s);
        return hr;
    }

    // Get bitmap information from the returned string
    if(lpHeight && !FGetDwParam(str, "height", lpHeight)) {
error:
        TerminateConnection(s);
        CloseSharedConnection(s);
        return E_UNEXPECTED;
    }
    if(lpWidth && !FGetDwParam(str, "width",lpWidth))
        goto error;
    if(lpFormat && !FGetDwParam(str, "format", lpFormat))
        goto error;
    if(lpFrameBufferSize) {
        if (!FGetDwParam(str, "framebuffersize", &dwFrameBufferSize))
            goto error;

        *lpFrameBufferSize = dwFrameBufferSize;
    }

    if(bitBuffer) {
        while(dwFrameBufferSize) {
            DWORD cBytesToReceive = min(dwFrameBufferSize, 8192);

            hr = DmReceiveBinary(s, (LPBYTE) bitBuffer + cBytesRead, cBytesToReceive, &cch);
            if(FAILED(hr)) {
                TerminateConnection(s);
                CloseSharedConnection(s);
                return hr;
            }

            cBytesRead += cch;
            dwFrameBufferSize -= cBytesToReceive;
        }
    }

    CloseSharedConnection(s);
    return XBDM_NOERR;
}

HRESULT DmPixelShaderSnapshot(DWORD dwX, DWORD dwY, DWORD dwFlags, DWORD dwMarker, BYTE *pBuffer)
{
    PDM_CONNECTION s;
    DWORD cch, cBytesRemainingToWrite;
    char str[256], cmd[256];
    DWORD cBytesToWrite, cBytesToReceive;
    HRESULT hr;

    if (pBuffer == NULL)
        return E_INVALIDARG;

    hr = HrOpenSharedConnection(&s);
    if(FAILED(hr))
    {
        sprintf(pBuffer, "Unable to open connection to Xbox");
        return hr;
    }

    cch = 255;
    
    sprintf(cmd, "PSSnap x=%d y=%d flags=%d marker=%d", dwX, dwY, dwFlags, dwMarker);
    hr = DmSendCommand( s, cmd, str, &cch );
    if(hr != XBDM_BINRESPONSE) 
    {
        strcpy(pBuffer, str);
        if(SUCCEEDED(hr)) 
        {
            TerminateConnection(s);
            hr = E_UNEXPECTED;
        }
        CloseSharedConnection(s);
        return hr;
    }

    // Read the 32k snapshot from the Xbox
    hr = DmReceiveBinary(s, pBuffer, 32768, &cBytesRemainingToWrite);
    if(FAILED(hr)) 
    {
        TerminateConnection(s);
        CloseSharedConnection(s);
        sprintf(pBuffer, "Failed to transfer data from Xbox");
        return hr;
    }
    CloseSharedConnection(s);
    return XBDM_NOERR;
}

HRESULT DmVertexShaderSnapshot(DWORD dwFirst, DWORD dwLast, DWORD dwFlags, DWORD dwMarker, BYTE *pBuffer)
{
    PDM_CONNECTION s;
    DWORD cch, cBytesRemainingToWrite;
    char str[256], cmd[256];
    DWORD cBytesToWrite, cBytesToReceive;
    HRESULT hr;

    if (pBuffer == NULL)
        return E_INVALIDARG;

    hr = HrOpenSharedConnection(&s);
    if(FAILED(hr))
    {
        sprintf(pBuffer, "Unable to open connection to Xbox");
        return hr;
    }

    cch = 255;
    
    sprintf(cmd, "VSSnap first=%d last=%d flags=%d marker=%d", dwFirst, dwLast, dwFlags, dwMarker);
    hr = DmSendCommand( s, cmd, str, &cch );
    if(hr != XBDM_BINRESPONSE) 
    {
        strcpy(pBuffer, str);
        // OK for xbdm to return success code
        CloseSharedConnection(s);
        return hr;
    }

    // Read the 32k snapshot from the Xbox
    hr = DmReceiveBinary(s, pBuffer, 32768, &cBytesRemainingToWrite);
    if(FAILED(hr)) 
    {
        TerminateConnection(s);
        CloseSharedConnection(s);
        sprintf(pBuffer, "Failed to transfer data from Xbox");
        return hr;
    }
    CloseSharedConnection(s);
    return XBDM_NOERR;
}


