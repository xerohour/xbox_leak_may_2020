/*
**
** file.cpp
**
** File utilities
**
*/

#include "precomp.hxx"
#include <stdio.h>
#include "xbfileres.h"

FIL &FIL::operator =(LPCTSTR sz)
{
	const TCHAR *pchFile;
	LPCTSTR szFileName = sz;

	if((sz[0] == 'x' || sz[0] == 'X') && sz[2] == ':') {
		/* Looks like it could be an Xbox name */
		WCHAR wch = sz[1];
		if(wch >= 'a' && wch <= 'z')
			m_fXbox = (char)(wch - ('a' - 'A'));
		else if(wch >= 'A' && wch <= 'Z')
			m_fXbox = (char)wch;
		else
			m_fXbox = 0;
		if(m_fXbox)
			sz += 3;
    } else {
		m_fXbox = 0;

        // Check if PC Drive specified
        if (sz[1] == ':')
            m_chPCDrive = sz[0];
        else
            m_chPCDrive = 0;
    }

	if(!*sz)
		::Fatal(0, IDS_BADPATH, szFileName);

	Basename(sz, &pchFile, NULL);
	if(pchFile == sz) {
		if(m_fXbox) {
			fprintf(stderr, "error: Xbox pathnames cannot be relative\n");
			exit(1);
		}
		_tcscpy(m_szDir, ".");
	} else if(pchFile - sz > sizeof m_szDir) {
toolong:
		fprintf(stderr, "error: filename too long\n");
		exit(-1);
	} else {
		_tcsncpy(m_szDir, sz, pchFile - sz);
		/* Last char was the \, so nuke it -- unless we're talking about the
		 * root directory */
		if(pchFile == sz+1) {
			m_szDir[pchFile - sz] = '.';
			m_szDir[pchFile - sz + 1] = 0;
		} else
			m_szDir[pchFile - sz - 1] = 0;
	}

	if(!*pchFile) {
		/* No filename, but we had a directory, so we need to append a '.' to
		 * the directory name to make it valid */
		m_szDir[pchFile - sz] = '.';
		m_szDir[pchFile - sz + 1] = 0;
	} else if(_tcslen(pchFile) >= sizeof m_szName)
		goto toolong;
	_tcscpy(m_szName, pchFile);

	return *this;
}

HRESULT FIL::HrLocalTempFile(void)
{
	TCHAR sz[MAX_PATH + 1];
	TCHAR szPath[MAX_PATH + 1];

	if(!GetTempPath(sizeof szPath, szPath))
		return E_FAIL;
	if(!GetTempFileName(szPath, "XBF", 0, sz))
		return E_FAIL;
	*this = sz;
	return S_OK;
}

HRESULT HrLastError(void)
{
	switch(GetLastError())
	{
	case ERROR_PATH_NOT_FOUND:
	case ERROR_FILE_NOT_FOUND:
		return XBDM_NOSUCHFILE;
	case ERROR_DIR_NOT_EMPTY:
		return XBDM_DIRNOTEMPTY;
	case ERROR_ALREADY_EXISTS:
		return XBDM_ALREADYEXISTS;
	case ERROR_DISK_FULL:
		return XBDM_DEVICEFULL;
	default:
		return XBDM_CANNOTACCESS;
	}
}

HRESULT FIL::HrGetFileAttributes(PDM_FILE_ATTRIBUTES pdmfa) const
{
	TCHAR szFullName[1024];
	int cchName;
	WIN32_FILE_ATTRIBUTE_DATA fa;

	FillFullName(szFullName);
    if(m_fXbox) {
        HRESULT hr = DmGetFileAttributes(szFullName, pdmfa);
        switch(hr) {
        case XBDM_CANNOTCONNECT:
        case XBDM_CONNECTIONLOST:
        case XBDM_NOXBOXNAME:
            ::Fatal(hr, 0);
        }
        return hr;
    }

	if(!GetFileAttributesEx(szFullName, GetFileExInfoStandard, &fa))
		return HrLastError();

	pdmfa->CreationTime = fa.ftCreationTime;
	pdmfa->ChangeTime = fa.ftLastWriteTime;
	pdmfa->SizeHigh = fa.nFileSizeHigh;
	pdmfa->SizeLow = fa.nFileSizeLow;
	pdmfa->Attributes = fa.dwFileAttributes;

	cchName = strlen(m_szName);
	if(cchName >= sizeof pdmfa->Name)
		cchName = sizeof pdmfa->Name - 1;
	strncpy(pdmfa->Name, m_szName, cchName);
	pdmfa->Name[cchName] = 0;
	return S_OK;
}

HRESULT FIL::HrSetFileAttributes(PDM_FILE_ATTRIBUTES pdmfa) const
{
	TCHAR szFullName[1024];
	HANDLE h;
	HRESULT hr;
	FILETIME *pftCreate;
	FILETIME *pftChange;
    DWORD dw;

	FillFullName(szFullName);
	if(m_fXbox)
		return DmSetFileAttributes(szFullName, pdmfa);

    if(pdmfa->Attributes && !SetFileAttributes(szFullName, pdmfa->Attributes))
        return HrLastError();

    /* Only open the file if we're planning to change the times */
    dw = pdmfa->CreationTime.dwHighDateTime |
        pdmfa->CreationTime.dwLowDateTime | pdmfa->ChangeTime.dwHighDateTime |
        pdmfa->ChangeTime.dwLowDateTime;
    if(!dw)
        return XBDM_NOERR;
    
	h = CreateFile(szFullName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == h)
		return HrLastError();

	if(pdmfa->CreationTime.dwHighDateTime || pdmfa->CreationTime.dwLowDateTime)
		pftCreate = &pdmfa->CreationTime;
	else
		pftCreate = NULL;
	if(pdmfa->ChangeTime.dwHighDateTime || pdmfa->ChangeTime.dwLowDateTime)
		pftChange = &pdmfa->ChangeTime;
	else
		pftChange = NULL;

    hr = SetFileTime(h, pftCreate, NULL, pftChange) ? XBDM_NOERR :
        HrLastError();

    CloseHandle(h);
    return hr;
}

#if 0
HRESULT FIL::HrMoveFile(const FIL &filTo, BOOL fSend) const
{
	char szFullFrom[1024];
	char szFullTo[1024];
				   
	FillFullName(szFullFrom);
	filTo.FillFullName(szFullTo);
	return fSend ? DmSendFile(szFullFrom, szFullTo) : DmReceiveFile(szFullTo,
		szFullFrom);
}
#endif

void FIL::Descend(void)
{
	/* Move the filename into the directory name in preparation for descending
	 * into another subdir */
	if(*m_szName) {
		int cchDir = strlen(m_szDir);
		int cchName = strlen(m_szName);
		if(cchDir + cchName + 1 >= sizeof m_szDir) {
			fprintf(stderr, "error: filename too long\n");
			exit(1);
		}
		m_szDir[cchDir++] = '\\';
		_tcscpy(m_szDir + cchDir, m_szName);
		m_szName[0] = 0;
	}
}

void FIL::FillFullName(LPSTR sz, BOOL fOmitXboxDrive) const
{
	int ich;
	const TCHAR *pch;

	if(m_fXbox && !fOmitXboxDrive) {
		/* We're going to start with the Xbox drive letter */
		*sz++ = m_fXbox;
		*sz++ = ':';
	}

	/* First we need to copy the directory over, removing .s from the directory
	 * path as we do so */
	pch = m_szDir;
	ich = 0;

	/* Check for .\ at the beginning */
	while(pch[0] == '.') {
		if(pch[1] == '\\') {
			/* Skip the opening .\ */
			pch += 2;
			continue;
		}
		if(pch[1] == 0)
			/* Skip the . and fall through the loop below */
			++pch;
		break;
	}

	while(*pch) {
		if(pch[0] == '\\' && pch[1] == '.') {
			if(pch[2] == '\\') {
				/* Found \.\ so skip the first two chars and keep going */
				pch += 2;
				continue;
			} else if(pch[2] == 0) {
				/* Found \. at the end of the path, so nuke the last two
				 * characters */
				break;
			}
		}
		/* Whatever we found is acceptable, so copy it and keep going */
		sz[ich++] = *pch++;
	}

	/* OK, we've nailed the directory portion.  Tack on the filename */
	if(*m_szName) {
		if(ich || *m_szDir == '\\')
			sz[ich++] = '\\';
		_tcscpy(sz + ich, m_szName);
	} else {
		if(!ich)
			/* We had . or \. for the directory and nothing for the filename.
			 * Make it look right */
			 sz[ich++] = *m_szDir == '\\' ? '\\' : '.';
		sz[ich] = 0;
	}
}

void FIL::FillDisplayName(LPTSTR sz) const
{
	if(m_fXbox) {
		*sz++ = 'x';
		*sz++ = m_fXbox + ('a' - 'A');
		*sz++ = ':';
	}
	FillFullName(sz, TRUE);
}

/* We just overload this struct definition to suit our own needs */
struct _DM_WALK_DIR {
	HANDLE h;
};

HRESULT FIL::HrWalkDir(PDM_WALK_DIR *ppdmwd, LPCTSTR szPatMatch,
    FAT *pfat)
{
	WIN32_FIND_DATA wfd;
	TCHAR rgch[1024];
	TCHAR *sz;
	HRESULT hr;
    PDM_FILE_ATTRIBUTES pdmfa;
    DM_FILE_ATTRIBUTES dmfa;

	/* Make sure we're ready to receive filenames */
	if(!*ppdmwd) {
		Descend();
		if(!m_fXbox) {
			/* FindFirstFile wants a wildcard search, not just the name of the
			 * directory */
			if(szPatMatch)
				strcpy(m_szName, szPatMatch);
			else {
				m_szName[0] = '*';
				m_szName[1] = 0;
			}
		}
		FillFullName(sz = rgch);
	} else
		sz = NULL;

    pdmfa = pfat ? &pfat->dmfa : &dmfa;

	do {
		if(m_fXbox) {
			DWORD cch = sizeof m_szName - 1;
			hr = DmWalkDir(ppdmwd, sz, pdmfa);
            strcpy(m_szName, pdmfa->Name);
		} else {
			do {
				if(!*ppdmwd) {
					HANDLE h;

					h = FindFirstFile(sz, &wfd);
					if(h == INVALID_HANDLE_VALUE) {
						hr = HrLastError();
						break;
					}
					*ppdmwd = (PDM_WALK_DIR)LocalAlloc(LMEM_FIXED,
						sizeof (struct _DM_WALK_DIR));
					(*ppdmwd)->h = h;
				} else if(!FindNextFile((*ppdmwd)->h, &wfd)) {
					hr = XBDM_ENDOFLIST;
					break;
				}
				hr = XBDM_NOERR;
			} while(SUCCEEDED(hr) && (0 == strcmp(wfd.cFileName, ".") ||
				0 == strcmp(wfd.cFileName, "..")));
            if(SUCCEEDED(hr)) {
				strcpy(m_szName, wfd.cFileName);
                if(pfat)
                    pfat->fValid = SUCCEEDED(HrGetFileAttributes(pdmfa));
            }
		}
		/* We keep looping over names as long as there are names and they
		 * match the wildcard pattern, if there is one */
	} while(SUCCEEDED(hr) && m_fXbox && szPatMatch && !FWildMatch(szPatMatch));

	return hr;
}

void FIL::EndWalkDir(PDM_WALK_DIR pdmwd)
{
	if(m_fXbox)
		DmCloseDir(pdmwd);
	else if(pdmwd) {
		FindClose(pdmwd->h);
		LocalFree(pdmwd);
	}
	*m_szName = NULL;
}

HRESULT FIL::HrMkdir(void) const
{
	TCHAR sz[1024];

	FillFullName(sz);
	if(m_fXbox)
		return DmMkdir(sz);

	return CreateDirectory(sz, NULL) ? S_OK : HrLastError();
}

HRESULT FIL::HrRename(FIL &filNew) const
{
	HRESULT hr;
	TCHAR szOld[1024];
	TCHAR szNew[1024];

	/* Can't rename across the network */
	if(!m_fXbox != !filNew.m_fXbox)
		return XBDM_MUSTCOPY;
	FillFullName(szOld);
	filNew.FillFullName(szNew);
	if(m_fXbox)
		hr = DmRenameFile(szOld, szNew);
	else
		hr = MoveFileEx(szOld, szNew, MOVEFILE_WRITE_THROUGH) ? S_OK :
			HrLastError();
	return hr;
}

HRESULT FIL::HrDelete(BOOL fIsDir) const
{
	HRESULT hr;
	TCHAR sz[1024];

	FillFullName(sz);
	if(m_fXbox) {
		hr = DmDeleteFile(sz, fIsDir);
	} else if(fIsDir)
		hr = RemoveDirectory(sz) ? S_OK : HrLastError();
	else
		hr = DeleteFile(sz) ? S_OK : HrLastError();
	return hr;
}

HRESULT HrEnsureDirName(BOOL fXbox, LPSTR szName, int cchName)
{
	char *pchDir;
	HRESULT hr;

	for(pchDir = szName + cchName; pchDir-- > szName && *pchDir != '\\'; );

	if(pchDir <= szName) {
		hr = XBDM_NOERR;
		/* No subdirectory here; check for drive name */
		for(pchDir = szName; *pchDir; ++pchDir)
			if(*pchDir == ':')
				/* This looks like a drive name, so we'll say it exists */
				return hr;
		/* Doesn't look like a drive name, so we'll assume it's to be created
		 * in the current directory */
	} else {
		*pchDir = 0;
		hr = HrEnsureDirName(fXbox, szName, pchDir - szName);
		*pchDir = '\\';
	}
	if(SUCCEEDED(hr)) {
		if(fXbox)
			hr = DmMkdir(szName);
		else
			hr = CreateDirectory(szName, NULL) ? S_OK : HrLastError();
		if(hr == XBDM_ALREADYEXISTS)
			hr = XBDM_NOERR;
	}
	return hr;
}

HRESULT FIL::HrEnsureDir(void) const
{
	HRESULT hr;
	char sz[1024];
	char *pchDir;

	FillFullName(sz);

	for(pchDir = sz; *pchDir; ++pchDir);
	return HrEnsureDirName(m_fXbox, sz, pchDir - sz);
}