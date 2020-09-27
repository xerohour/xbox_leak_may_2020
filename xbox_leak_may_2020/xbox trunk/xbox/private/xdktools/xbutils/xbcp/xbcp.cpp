/*
**
** xbcp
**
** Copy files to and from the Xbox
**
*/

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "xbfile.hxx"
#include "xbfilr.hxx"
#include "optn.hxx"
#include "resource.h"
#include "xbfileres.h"
#include <xboxverp.h>

BOOL fQuiet;
BOOL fSuccess;
BOOL fForceRO;
BOOL fForceReplace;
BOOL fCopyIfNewer;
BOOL fSkipEmptyDirs;
BOOL fEnsureDir;

struct OFIL : public FILR
{
	OFIL *m_pofilFileDir;
	OFIL *m_pofilDirDir;
	FIL *m_pfilDestDir;
	int m_fDestDirExists;

	OFIL(const char *sz) : FILR(sz) {}
	OFIL(const FIL &fil) : FILR(fil) {}

	void DoWork(void);
	BOOL FStartDir(void);
	void EndDir(void);
	FILR *PfilrCopy(void) const { return new OFIL(*this); }

	BOOL FEnsureDestDirExists(void);
	void CopyOneFile(const FIL &filDst);
	void CopyOneName(const FIL &filDst, const FIL *pfilDstDir);
};

BOOL FReplaceFile(const FIL &fil)
{
	char sz[1024];

	if(fForceReplace)
		return TRUE;

	for(;;) {
		fil.FillDisplayName(sz);

		printf("Overwrite %s? (Yes/No/All): ", sz);
		if(!fgets(sz, sizeof sz, stdin)) {
			putchar('\n');
			return FALSE;
		}
		switch(*sz) {
		case 'y':
		case 'Y':
			return TRUE;
		case 'n':
		case 'N':
			return FALSE;
		case 'a':
		case 'A':
			return fForceReplace = TRUE;
		}
	}
}

void OFIL::CopyOneFile(const FIL &filDst)
{
	HRESULT hr;
	DM_FILE_ATTRIBUTES dmfa;
	PDM_FILE_ATTRIBUTES pdmfaSrc = PdmfaGet();
	char szSrc[1024];
	char szDst[1024];
	const FIL *pfilLocal;
	const FIL *pfilRemote;
	int ids = 0;

	/* Let's make sure we're good to go */
	if(!pdmfaSrc)
		return;

	/* If we're going to copy a directory, we just want to make the destination
	 * directory and be done */
	hr = filDst.HrGetFileAttributes(&dmfa);
	if(pdmfaSrc->Attributes & FILE_ATTRIBUTE_DIRECTORY) {
		if(SUCCEEDED(hr)) {
			if(!(dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY)) {
				filDst.Warning(0, IDS_NOTDIR);
				return;
			} else if(fCopyIfNewer) {
				/* We'll just bail now and not report the name */
				fSuccess = TRUE;
				return;
			}
		}
	} else if(SUCCEEDED(hr)) {
		if(dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY) {
			/* We can't copy a file into a directory */
			filDst.Warning(0, IDS_ISDIR);
			return;
		}
		
		/* If we're doing copy-if-newer, then we'll reject the copy if the
		 * destination is too new */
		if(fCopyIfNewer && ((PULARGE_INTEGER)&dmfa.ChangeTime)->QuadPart >= 
			((PULARGE_INTEGER)&pdmfaSrc->ChangeTime)->QuadPart)
		{
			fSuccess = TRUE;
			return;
		}

		if(dmfa.Attributes & FILE_ATTRIBUTE_READONLY) {
			if(fForceRO) {
				dmfa.Attributes &= ~FILE_ATTRIBUTE_READONLY;
                if(!dmfa.Attributes)
                    dmfa.Attributes = FILE_ATTRIBUTE_NORMAL;
				hr = filDst.HrSetFileAttributes(&dmfa);
				if(FAILED(hr)) {
					filDst.Warning(hr, 0);
					return;
				}
			} else {
				filDst.Warning(0, IDS_READONLY);
				return;
			}
		}

		/* We're going to overwrite this file, so we need to ask */
		if(!FReplaceFile(filDst))
			return;
	}

	if(!fQuiet) {
		char szFileSrc[1024];
		char szFileDst[1024];

		FillDisplayName(szFileSrc);
		filDst.FillDisplayName(szFileDst);
		printf("%s => %s\n", szFileSrc, szFileDst);
	}

	/* Now we're ready to perform the copy */
	if(pdmfaSrc->Attributes & FILE_ATTRIBUTE_DIRECTORY) {
		/* hr is still valid from the call to filDst.GetFileAttributes */
		if(FAILED(hr))
			hr = filDst.HrMkdir();
		if(FAILED(hr))
			filDst.Warning(hr, 0);
        else
            fSuccess = TRUE;
		return;
	}

	FillFullName(szSrc);
	filDst.FillFullName(szDst);
	if(m_fXbox) {
		if(filDst.m_fXbox) {
			/* Copy to a temp file first */
			FIL filTmp;
			char szTmp[1024];
			if(SUCCEEDED(filTmp.HrLocalTempFile())) {
				filTmp.FillFullName(szTmp);
				hr = DmReceiveFile(szTmp, szSrc);
				pfilRemote = this;
				if(SUCCEEDED(hr)) {
					hr = DmSendFile(szTmp, szDst);
					pfilRemote = &filDst;
				}
				if(hr == XBDM_FILEERROR)
					ids = IDS_NOTEMPFILE;
				filTmp.HrDelete();
			} else {
				pfilRemote = this;
				hr = E_UNEXPECTED;
				ids = IDS_NOTEMPFILE;
			}
        } else {
    		hr = DmReceiveFile(szDst, szSrc);
			pfilLocal = &filDst;
			pfilRemote = this;
        }
	} else {
        if(!filDst.m_fXbox)
            hr = CopyFile(szSrc, szDst, FALSE) ? XBDM_NOERR : XBDM_FILEERROR;
        else
		    hr = DmSendFile(szSrc, szDst);
		pfilLocal = this;
		pfilRemote = &filDst;
	}
	if(FAILED(hr)) {
		if(hr == XBDM_FILEERROR)
			pfilLocal->Warning(HrLastError(), ids);
		else
			pfilRemote->Warning(hr, ids);
    } else {
		fSuccess = TRUE;
	
    	/* Copy the attributes to the destination file */
		filDst.HrSetFileAttributes(pdmfaSrc);
    }
}

BOOL OFIL::FStartDir(void)
{
	/* Fix up the containment chain */
	m_pofilDirDir = m_pofilFileDir;
	m_pofilFileDir = this;

	/* We need to construct a FIL that corresponds to the destination directory
	 * that matches this source directory */
	m_pfilDestDir = new FIL(*m_pofilDirDir->m_pfilDestDir);
	m_pfilDestDir->Descend();
	strcpy(m_pfilDestDir->m_szName, m_szName);
	m_fDestDirExists = FALSE;

	return TRUE;
}

void OFIL::EndDir(void)
{
	m_pofilFileDir = m_pofilDirDir;
	delete m_pfilDestDir;
}

BOOL OFIL::FEnsureDestDirExists(void)
{
	HRESULT hr;
	DM_FILE_ATTRIBUTES dmfa;

	if(!m_fDestDirExists) {
		if(!m_pofilDirDir->FEnsureDestDirExists())
			return FALSE;
		m_fDestDirExists = 1;
		hr = m_pfilDestDir->HrGetFileAttributes(&dmfa);
		if(SUCCEEDED(hr)) {
			if(!(dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY)) {
				m_fDestDirExists = -1;
				Warning(hr, IDS_NOTDIR);
			}
		} else {
			hr = m_pfilDestDir->HrMkdir();
			if(FAILED(hr)) {
				m_fDestDirExists = -1;
				if(!fRecurse)
					/* If we're recursing, we should have already caught this
					 * error */
					Warning(hr, 0);
			}
		}
	}
	return m_fDestDirExists == 1;
}		

void OFIL::DoWork(void)
{
	PDM_FILE_ATTRIBUTES pdmfa = PdmfaGet();

	/* If this is a directory, we'll "copy" it (ie., make a destination
	 * directory) if we're recursive and not skipping empty directories */
	if(pdmfa) {
		if(pdmfa->Attributes & FILE_ATTRIBUTE_DIRECTORY) {
			if(!fRecurse) {
				Warning(0, IDS_ISDIR);
				return;
			}
			if(fSkipEmptyDirs)
				return;
		}

		/* OK, we're going to copy this file.  Make sure the destination
		 * directory exists, and away we go */
		if(m_pofilFileDir->FEnsureDestDirExists()) {
			FIL filDst(*m_pofilFileDir->m_pfilDestDir);
			filDst.Descend();
			strcpy(filDst.m_szName, m_szName);

			CopyOneFile(filDst);
		}
	}
}

void OFIL::CopyOneName(const FIL &filDst, const FIL *pfilDstDir)
{
	/* Because we may go off and treat this ofil as a single file, we need to
	 * create another ofil to represent its parent directory */

	m_pfilDestDir = new FIL(filDst);
	OFIL ofil(*this);
	m_fDestDirExists = 1;
    if(!pfilDstDir && 0 == strcmp(m_szName, ".") || 0 == strcmp(m_szName, ".."))
    {
        /* We're copying "." or ".." into another existing directory.  Clearly
         * we can't create a subdir called "." or "..", so we'll just assume
         * that the correct action is to copy the _contents_ of "." */
        pfilDstDir = &filDst;
    }
	if(pfilDstDir) {
		/* Since we're faking up the matching directory, we expand out the
		 * startdir-dotree-enddir logic here and then replace the startdir
		 * logic with the equivalent faking */
		ofil.m_pfilDestDir = new FIL(*pfilDstDir);
		ofil.m_fDestDirExists = 0;
		ofil.m_pofilDirDir = this;
		ofil.m_pofilFileDir = &ofil;
		ofil.DoSubTree(NULL);
		ofil.EndDir();
	} else {
		ofil.m_pofilFileDir = this;
		ofil.DoTree(FALSE);
	}
	delete m_pfilDestDir;
}

BOOL OPTH::FParseOption(char *&sz, char)
{
	switch(*sz) {
	case 'f':
	case 'F':
		++fForceRO;
		// fall through
	case 'y':
	case 'Y':
		++fForceReplace;
		break;
	case 'q':
	case 'Q':
		++fQuiet;
		break;
	case 'd':
	case 'D':
		++fCopyIfNewer;
		break;
	case 'e':
	case 'E':
		++fSkipEmptyDirs;
		break;
	case 't':
	case 'T':
		++fEnsureDir;
		break;
	default:
		return FParseFilrOption(*sz);
	}
	return TRUE;
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbcp version " VER_PRODUCTVERSION_STR "\n\n"
        "Copies one or more files to to or from the Xbox target system\n");
	fprintf(stderr, "\nusage: %s [/x xboxname] [options] srcfile ... destfile\n",
		m_szExe);
	fprintf(stderr, "\t/r\trecursive copy\n"
		"\t/s\tsearch in subdirectories\n"
        "\t/h\tinclude hidden files\n"
		"\t/q\tsuppress file list output\n"
		"\t/y\tdon't prompt to overwrite\n"
		"\t/d\tcopy files only if newer\n"
		"\t/e\tdon't copy empty directories\n"
		"\t/t\tcreate destination directory if it does not exist\n"
		"\t/f\tforce copy over read-only files\n"
		"Specify Xbox files as xE:\\..., xD:\\..., etc.\n");
	exit(1);
}

int __cdecl _tmain(int cArg, TCHAR **rgszArg)
{
	HRESULT hr;
	DM_FILE_ATTRIBUTES dmfa;
	FIL filDst;

    /* Cut down on repeated connection requests */
    DmUseSharedConnection(TRUE);

	/* Process arguments */
	OPTH opth(cArg, rgszArg);

	if(cArg < 1)
		opth.UsageFailure();

	if(cArg == 1)
		filDst = ".";
	else
		filDst = rgszArg[cArg - 1];
	if(filDst.FIsWildName()) {
		fprintf(stderr, "error: wildcards not allowed in destination name\n");
		exit(1);
	}

	hr = filDst.HrGetFileAttributes(&dmfa);

	/* Ensure we're copying to a directory if there's more than one file */
	if((cArg == 2 && FIsWildName(*rgszArg) || cArg > 2 || fSubdirs))
	{
		if(FAILED(hr) && fEnsureDir) {
			hr = filDst.HrEnsureDir();
			if(SUCCEEDED(hr))
				hr = filDst.HrGetFileAttributes(&dmfa);
		}
		if(FAILED(hr) || !(dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY))
			filDst.Fatal(hr, FAILED(hr) ? 0 : IDS_NOTDIR);
	}

	if(cArg == 1)
		/* Need to fake another argument so that we actually do the copy */
		++cArg;

	if(FAILED(hr) || !(dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY)) {
		/* If the destination file does not exist, then it's going to become a
		 * direct copy of whatever the first argument is.  If the first
		 * arg is a directory, then we create the second file as a directory
		 * and copy into it.  If the first arg is a file, we just do the
		 * copy */
		OFIL ofilSrc(*rgszArg);
		PDM_FILE_ATTRIBUTES pdmfa = ofilSrc.PdmfaGet();
		if(pdmfa) {
			if((pdmfa->Attributes & FILE_ATTRIBUTE_DIRECTORY) && !fRecurse)
				ofilSrc.Fatal(0, IDS_ISDIR);
			hr = XBDM_NOERR;
			FIL filDir(filDst);
			filDir.m_szName[0] = 0;
			if(fEnsureDir)
				hr = filDir.HrEnsureDir();
			else
				hr = XBDM_NOERR;
			if(SUCCEEDED(hr)) {
				if(pdmfa->Attributes & FILE_ATTRIBUTE_DIRECTORY) {
					/* We're copying the directory into a new location.  First
					 * copy the directory, so we get the output, then
					 * proceed to copy the tree */
					ofilSrc.CopyOneFile(filDst);
					ofilSrc.CopyOneName(filDir, &filDst);
				} else
					/* This is a straight file copy */
					ofilSrc.CopyOneFile(filDst);
				hr = E_FAIL;
			}
		}
	}
	if(SUCCEEDED(hr)) {
		for(; --cArg; ++rgszArg) {
			OFIL ofilSrc(*rgszArg);
			ofilSrc.CopyOneName(filDst, NULL);
		}
	}

	DmUseSharedConnection(FALSE);

    return fSuccess ? 0 : 1;
}
