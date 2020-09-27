/*
 *
 * xbecopy.cpp
 *
 * Copy an xbe to the remote, called by VC
 *
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include <xboxverp.h>
#include "xbfile.hxx"
#include "resource.h"

BOOL fNologo;

HRESULT HrEnsureDir(const FIL &fil)
{
	BOOL fRoot;
	HRESULT hr;
	DM_FILE_ATTRIBUTES dmfa;

	if(0 != strcmp(fil.m_szDir, "\\.")) {
		FIL filDir;
		/* Ensure that the parent directory exists */
		filDir = fil.m_szDir;
		filDir.m_fXbox = fil.m_fXbox;
		filDir.m_szName[0] = 0;
		hr = HrEnsureDir(filDir);
		if(FAILED(hr))
			return hr;
	}
	hr = fil.HrGetFileAttributes(&dmfa);
	if(SUCCEEDED(hr)) {
		if(dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY)
			return XBDM_NOERR;
		else
			return XBDM_NOSUCHFILE;
	} else if(hr != XBDM_NOSUCHFILE)
		return hr;
	return fil.HrMkdir();
}

int __cdecl main(int cArg, char **rgszArg)
{
	char *rgszFiles[2];
	int iszFiles = 0;
	HRESULT hr;
	DM_FILE_ATTRIBUTES dmfa;
	WIN32_FILE_ATTRIBUTE_DATA fa;

    SetIdpIds(IDP_ERROR, IDS_XBCP_ERROR);
    SetIdpIds(IDP_WARNING, IDS_XBCP_WARNING);

	/* Parse the arguments.  Anything starting with a '/' we consider an
	 * argument, and everything else is a filename.  We accept two filenames,
	 * the source file and the remote file */

	while(++rgszArg, --cArg) {
		if(**rgszArg == '/') {
			if(!_stricmp(*rgszArg, "/NOLOGO"))
				++fNologo;
			else
                Warning(0, IDS_UNKOPT, *rgszArg);
		} else {
			if(iszFiles == 2)
                Warning(0, IDS_EXTARG, *rgszArg);
			else
				rgszFiles[iszFiles++] = *rgszArg;
		}
	}

	if(!fNologo)
		fprintf(stderr, "Microsoft (R) Xbox image remote copy %s\n"
			"Copyright (C) Microsoft Corporation.  All rights reserved.\n\n",
			VER_PRODUCTVERSION_STR);

	if(iszFiles != 2)
        Fatal(0, IDS_BADFILEARGS);

	/* Copy the file */
	DmUseSharedConnection(TRUE);

	if(!GetFileAttributesEx(rgszFiles[0], GetFileExInfoStandard, &fa)) {
		char szError[256];
		if(0 == FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0,
				szError, sizeof szError, NULL))
			strcpy(szError, "cannot access file");
        Fatal(0, IDS_GENERICERR, rgszFiles[0], szError);
	}

	dmfa.CreationTime = fa.ftCreationTime;
	dmfa.ChangeTime = fa.ftLastWriteTime;
	dmfa.SizeHigh = fa.nFileSizeHigh;
	dmfa.SizeLow = fa.nFileSizeLow;
	dmfa.Attributes = fa.dwFileAttributes;

	FIL filRmt;
	if(rgszFiles[1][0] == 'x')
		filRmt = rgszFiles[1];
	else {
		char szFileName[MAX_PATH+1];
		szFileName[0] = 'x';
		strcpy(szFileName+1, rgszFiles[1]);
		filRmt = szFileName;
	}
	if(!filRmt.m_fXbox) {
        filRmt.Fatal(XBDM_BADFILENAME, 0);
	}

	FIL filDir(filRmt);
	filDir.m_szName[0] = 0;
	hr = HrEnsureDir(filDir);
	if(FAILED(hr)) {
		if(hr == XBDM_NOSUCHFILE)
			filRmt.m_szName[0] = 0;
		filRmt.Fatal(hr, 0);
	}

	hr = DmSendFile(rgszFiles[0], rgszFiles[1]);
	if(SUCCEEDED(hr))
		filRmt.HrSetFileAttributes(&dmfa);

	if(FAILED(hr))
		Fatal(hr, 0, hr == XBDM_FILEERROR ? rgszFiles[0] : rgszFiles[1]);

	DmUseSharedConnection(FALSE);
	return 0;
}
