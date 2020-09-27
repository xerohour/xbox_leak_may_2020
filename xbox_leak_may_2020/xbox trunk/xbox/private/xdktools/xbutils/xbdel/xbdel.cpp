/*
**
** xbdel
**
** Delete files on the Xbox
**
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "xbfile.hxx"
#include "xbfilr.hxx"
#include "optn.hxx"
#include "resource.h"
#include "xbfileres.h"
#include <xboxverp.h>

BOOL fForce;
//BOOL fRecurseAll;
BOOL fVerbose;
BOOL fSuccess;
BOOL fSucceedAlways;

struct OFIL : public FILR
{
	OFIL(void) : FILR(), m_fDeleteMe(FALSE) {}
	OFIL(const char *sz) : FILR(sz), m_fDeleteMe(FALSE) {}

	void DoWork(void);
	void EndDir(void);
	FILR *PfilrCopy(void) const { return new OFIL(*this); }

	BOOL FConfirmDelete(void) const;
	void DeleteOne(BOOL fDir);

	//BOOL m_fRecurseSav;
	BOOL m_fDeleteMe;
};

#if 0
BOOL OFIL::FConfirmDelete(void) const
{
	char szMsg[1024];
	char szName[1024];

	FillDisplayName(szName);

	for(;;) {
		if(!FFmtIds(IDS_DELDIR, szMsg, szName))
			sprintf(szMsg, "Delete %s? (Yes/No/All): ");
		printf(szMsg);
		if(!fgets(szMsg, sizeof szMsg, stdin)) {
			putchar('\n');
			return FALSE;
		}
		switch(szMsg[0]) {
		case 'a':
		case 'A':
			fRecurseAll = TRUE;
			// fall through
		case 'y':
		case 'Y':
			return fRecurse = TRUE;
		case 'n':
		case 'N':
			return FALSE;
		}
	}
}
#endif

void OFIL::DeleteOne(BOOL fDir)
{
	HRESULT hr;
	PDM_FILE_ATTRIBUTES pdmfa = PdmfaGet();

	if(pdmfa) {
		if(pdmfa->Attributes & FILE_ATTRIBUTE_READONLY) {
			if(fForce) {
				pdmfa->Attributes &= ~FILE_ATTRIBUTE_READONLY;
                if(!pdmfa->Attributes)
                    pdmfa->Attributes = FILE_ATTRIBUTE_NORMAL;
				HrSetFileAttributes(pdmfa);
			} else {
				Warning(0, IDS_READONLY);
				return;
			}
		}
		hr = HrDelete(fDir);
		if(FAILED(hr))
			Warning(hr, 0);
        else
            fSuccess = TRUE;
	}
}

void OFIL::EndDir(void)
{
	HRESULT hr;
	PDM_FILE_ATTRIBUTES pdmfa = PdmfaGet();

	if(m_fDeleteMe)
		DeleteOne(TRUE);
	//fRecurse = fRecurseAll ? TRUE : m_fRecurseSav;
}

void OFIL::DoWork(void)
{
	PDM_FILE_ATTRIBUTES pdmfa;
	HRESULT hr;

	pdmfa = PdmfaGet();
	if(pdmfa) {
		if(pdmfa->Attributes & FILE_ATTRIBUTE_DIRECTORY) {
			/* We've been asked to delete this directory.  If we're marked
			 * as recursive, we'll delete it -- though not now; we'll need
			 * to delete it after its contents are gone.  If we're not marked
			 * as recursive, we'll ask whether we should switch to
			 * recursive mode, one-time or permanently, and do the appropriate
			 * work */
			//m_fRecurseSav = fRecurse;
			if(fRecurse /* || FConfirmDelete() */)
				m_fDeleteMe = TRUE;
			else {
                Warning(0, IDS_CANNOTDELETE);
				m_fDeleteMe = FALSE;
            }
		} else
			DeleteOne(FALSE);
	}
}

BOOL OPTH::FParseOption(char *&sz, char)
{
	switch(*sz) {
	case 'v':
		++fVerbose;
		break;
	case 'f':
		++fForce;
		break;
	case 'q':
		++fSucceedAlways;
        break;
	default:
		return FParseFilrOption(*sz);
	}
	return TRUE;
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbdel version " VER_PRODUCTVERSION_STR "\n\n"
        "Deletes one or more files from the Xbox development or target system.\n");
	fprintf(stderr, "\nusage: %s [/x xboxname] [options] file [file] ...\n",
		m_szExe);
	fprintf(stderr, "\t/r\trecursive delete\n"
		"\t/s\tsearch in subdirectories\n"
        "\t/h\tinclude hidden files\n"
		"\t/f\tforce delete of read-only files\n"
		"Specify Xbox files as xE:\\..., xD:\\..., etc.\n");
	exit(1);
}

int __cdecl main(int cArg, char **rgszArg)
{
	char *szExe = *rgszArg;
    BOOL fRet;

    /* Cut down on repeated connection requests */
    DmUseSharedConnection(TRUE);

	/* Process arguments */
	OPTH opth(cArg, rgszArg);

	if(cArg < 1)
		opth.UsageFailure();

    fRet = 0;
	for(; cArg--; ++rgszArg) {
		OFIL ofil(*rgszArg);
        fSuccess = FALSE;
		ofil.DoTree();
        fRet = fRet || !fSuccess;
	}

	DmUseSharedConnection(FALSE);

    return !fSucceedAlways && fRet;
}
