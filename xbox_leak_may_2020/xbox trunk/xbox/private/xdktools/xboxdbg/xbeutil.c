/*
 *
 * xbeutil.c
 *
 * XBE image file utilities
 *
 */

#include "precomp.h"
#include <xbeimage.h>

HRESULT HrGetXbeHeader(LPCSTR szXbe, PXBEIMAGE_HEADER pxh, HANDLE *ph)
{
	HANDLE hFile;
	HRESULT hr;
	DWORD cb;

	hFile = CreateFile(szXbe, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return XBDM_NOSUCHFILE;
	if(!ReadFile(hFile, pxh, sizeof *pxh, &cb, NULL) || cb < sizeof *pxh)
		hr = XBDM_NOTXBEFILE;
	else if(pxh->Signature != XBEIMAGE_SIGNATURE)
		hr = XBDM_NOTXBEFILE;
	else
		hr = XBDM_NOERR;
	if(FAILED(hr) || !ph)
		CloseHandle(hFile);
	else if(ph)
		*ph = hFile;
	return hr;
}

HRESULT XbeGetLocalModulePath(LPCSTR szXbe, LPCSTR szModule, LPSTR szPath,
	DWORD cchPath)
{
	HANDLE hFile;
	XBEIMAGE_HEADER xih;
	DWORD cb, cbT;
	char rgchNames[MAX_PATH + 2];
    char rgchModule[MAX_PATH + 1];
	char *pch;
	HRESULT hr;

    if (!szXbe || !szPath)
        return E_INVALIDARG;

	/* First just read the standard header */
	hr = HrGetXbeHeader(szXbe, &xih, &hFile);
	if(FAILED(hr))
		return hr;
	hr = XBDM_NOMODULE;

    for(;;) {
		/* We're only interested in modules that have embedded PE names */
        if(!xih.DebugFileName || !xih.DebugPathName)
            break;
        /* Read in the debug file name from the file */
        if(INVALID_FILE_SIZE == SetFilePointer(hFile, (ULONG)xih.DebugFileName -
            (ULONG)xih.BaseAddress, NULL, FILE_BEGIN))
            break;
        if(!ReadFile(hFile, rgchModule, sizeof rgchModule, &cb, NULL))
            break;
		for(pch = rgchModule; *pch && pch < rgchModule + cb; ++pch);
		if(pch - rgchModule == (int)cb)
			/* We didn't get a complete string.  Too bad */
			break;
		/* Check whether this is the module we want */
		if(szModule && _strnicmp(szModule, rgchModule, pch - rgchModule))
			break;
        if(!szModule) {
            /* We need to remember this name for later */
            szModule = rgchModule;
        }
        /* Read in the debug path name from the file */
        if(INVALID_FILE_SIZE == SetFilePointer(hFile, (ULONG)xih.DebugPathName -
            (ULONG)xih.BaseAddress, NULL, FILE_BEGIN))
            break;
        if(!ReadFile(hFile, rgchNames, sizeof rgchNames, &cb, NULL))
            break;
		for(pch = rgchNames; *pch && pch < rgchNames + cb; ++pch);
		if(pch - rgchNames == (int)cb)
			/* We didn't get a complete string.  Too bad */
			break;
        hr = XBDM_NOERR;
        break;
    }

	CloseHandle(hFile);
    if(FAILED(hr))
        return hr;

    /* We now have the name of the local file, but we need to ensure that
     * it exists.  If not, we're going to look again alongside the input
     * XBE */
    if(-1 == GetFileAttributes(rgchNames)) {
        char *pchMax = rgchNames + sizeof rgchNames;
        char *pch2 = NULL;
        for(pch = rgchNames; *szXbe && pch < pchMax; ++pch, ++szXbe) {
            *pch = *szXbe;
            if(*pch == '\\')
                pch2 = pch;
        }
        if(*szXbe)
            /* The pathname was too long */
            return XBDM_BADFILENAME;
        pch = pch2 ? pch2 + 1 : szPath;
        while(*szModule && pch < pchMax)
            *pch++ = *szModule++;
        if(pch == pchMax)
            /* The combined name is too long */
            return XBDM_BADFILENAME;
        *pch = 0;
        if(-1 == GetFileAttributes(rgchNames))
            hr = XBDM_NOSUCHFILE;
        else {
            /* The file exists; turn it into a long pathname */
            DWORD cch = GetFullPathName(rgchNames, cchPath, szPath, NULL);
            if(cch == 0)
                hr = E_UNEXPECTED;
            else if(cch > cchPath)
                hr = XBDM_BUFFER_TOO_SMALL;
            else
                hr = XBDM_NOERR;
        }
    } else {
        /* The local name we fetched is valid, so we need to return it */
    	cbT = 0;
		pch = rgchNames;
		while(*pch && pch < rgchNames + cb && cbT < cchPath)
			szPath[cbT++] = *pch++;
		if(pch - rgchNames != (int)cb) {
			if(cbT == cchPath) {
				szPath[cchPath - 1] = 0;
				hr = XBDM_BUFFER_TOO_SMALL;
			} else {
				szPath[cbT] = 0;
				hr = XBDM_NOERR;
			}
		}
	}

    return hr;
}

HRESULT XbeGetXbeInfo(LPCSTR szName, PDM_XBE pxbe)
{
	XBEIMAGE_HEADER xih;
	HRESULT hr;

    if (!szName || !pxbe)
        return E_INVALIDARG;

	hr = HrGetXbeHeader(szName, &xih, NULL);
	if(SUCCEEDED(hr)) {
		int ich;
		for(ich = 0; *szName && ich < sizeof pxbe->LaunchPath - 1; ++ich)
			pxbe->LaunchPath[ich] = *szName++;
		pxbe->LaunchPath[ich] = 0;
		pxbe->TimeStamp = xih.TimeDateStamp;
		pxbe->CheckSum = 0;
        pxbe->StackSize = xih.SizeOfStackCommit;
	}
	return hr;
}

HRESULT XbeGetBuilderArgs(LPCSTR szName, LPSTR szArgs, LPDWORD pcchArgs)
{
    XBEIMAGE_HEADER xih;
    XBEIMAGE_CERTIFICATE xbc;
    HRESULT hr;
    HANDLE h;
    DWORD cb;
    char *pchStart = szArgs;

    if(!pcchArgs || !szName || !szArgs)
        return E_INVALIDARG;

    hr = HrGetXbeHeader(szName, &xih, &h);
    if(FAILED(hr))
        return hr;
    /* We're not going to try to do anything unless we can get the long name */
    if(*pcchArgs < 6)
        hr = XBDM_BUFFER_TOO_SMALL;
    else if(!xih.DebugPathName)
        hr = XBDM_NOTDEBUGGABLE;
    /* We're also going to insist on getting the certificate data */
    else if(INVALID_FILE_SIZE == SetFilePointer(h, (ULONG)xih.Certificate -
            (ULONG)xih.BaseAddress, NULL, FILE_BEGIN) ||
            !ReadFile(h, &xbc, sizeof xbc, &cb, NULL) || cb < sizeof xbc)
		hr = XBDM_FILEERROR;
    else if(INVALID_FILE_SIZE == SetFilePointer(h, (ULONG)xih.DebugPathName -
            (ULONG)xih.BaseAddress, NULL, FILE_BEGIN))
        hr = XBDM_FILEERROR;
    else {
        char *pch;
        memcpy(szArgs, "/IN:\"", 5);
        *pcchArgs -= 5;
        szArgs += 5;
        if(!ReadFile(h, szArgs, *pcchArgs - 1, &cb, NULL))
            cb = 0;
        /* We need to have read the entire name and have room to spare to
         * consider this a success */
		for(pch = szArgs; *pch && pch < szArgs + cb; ++pch);
        if(pch - szArgs == (int)cb)
           /* We didn't get a complete string.  Too bad */
            hr = cb == *pcchArgs - 1 ? XBDM_BUFFER_TOO_SMALL : XBDM_FILEERROR;
        else {
            cb = pch - szArgs;
            *pcchArgs -= cb + 1;
            szArgs += cb + 1;
            szArgs[-1] = '"';
        }
    }
    if(SUCCEEDED(hr)) {
        if(*pcchArgs < 7)
            hr = XBDM_BUFFER_TOO_SMALL;
        else {
            memcpy(szArgs, " /DEBUG", 7);
            *pcchArgs -= 7;
            szArgs += 7;
        }
    }
    if(SUCCEEDED(hr)) {
        if(*pcchArgs < 18)
            hr = XBDM_BUFFER_TOO_SMALL;
        else {
            sprintf(szArgs, " /STACK:0x%08x",
                xih.SizeOfStackCommit);
            *pcchArgs -= 18;
            szArgs += 18;
        }
    }
    if(SUCCEEDED(hr)) {
        if(*pcchArgs < 19)
            hr = XBDM_BUFFER_TOO_SMALL;
        else {
            sprintf(szArgs, " /TESTID:0x%08x", xbc.TitleID);
            *pcchArgs -= 19;
            szArgs += 19;
        }
    }
    if(SUCCEEDED(hr) && xbc.TitleName[0]) {
        if(*pcchArgs < 12)
            hr = XBDM_BUFFER_TOO_SMALL;
        else {
            memcpy(szArgs, " /TESTNAME:\"", 12);
            szArgs += 12;
            *pcchArgs -= 12;
        }
        cb = WideCharToMultiByte(CP_ACP, 0, xbc.TitleName, -1, szArgs,
            *pcchArgs - 1, NULL, NULL);
        if(cb == *pcchArgs - 1)
            hr = XBDM_BUFFER_TOO_SMALL;
        else {
            *pcchArgs -= cb;
            szArgs += cb;
            szArgs[-1] = '"';
        }
    }

    /* That's it */
    if(SUCCEEDED(hr)) {
        if(*pcchArgs == 0)
            hr = XBDM_BUFFER_TOO_SMALL;
        else {
            *szArgs = 0;
            *pcchArgs = szArgs - pchStart;
        }
    }

    CloseHandle(h);
    return hr;
}
