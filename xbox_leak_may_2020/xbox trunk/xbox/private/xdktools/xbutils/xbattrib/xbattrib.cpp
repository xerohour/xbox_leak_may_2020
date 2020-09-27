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
#include <xboxverp.h>

DWORD dwAttributes;
DWORD dwMask;
BOOL fSuccess;

struct OFIL : public FILR
{
	OFIL(void) : FILR() {}
	OFIL(const char *sz) : FILR(sz) {}

	void DoWork(void);
	FILR *PfilrCopy(void) const { return new OFIL(*this); }
};

void OFIL::DoWork(void)
{
	PDM_FILE_ATTRIBUTES pdmfa;
	HRESULT hr;
    DM_FILE_ATTRIBUTES dmfaNew;

	pdmfa = PdmfaGet();
    if(dwMask) {
        pdmfa->Attributes ^= (pdmfa->Attributes ^ dwAttributes) & dwMask;
        if(pdmfa->Attributes == 0)
            pdmfa->Attributes = FILE_ATTRIBUTE_NORMAL;
        memset(&dmfaNew, 0, sizeof dmfaNew);
        dmfaNew.Attributes = pdmfa->Attributes;
        hr = HrSetFileAttributes(&dmfaNew);
        if(FAILED(hr))
            Warning(hr, 0);
        else
            fSuccess = TRUE;
    } else {
        /* Nothing to change, so we'll display */
        char sz[MAX_PATH+1];
        FillDisplayName(sz);
        printf("%c %c %c %s\n", pdmfa->Attributes & FILE_ATTRIBUTE_DIRECTORY ?
            'D' : ' ', pdmfa->Attributes & FILE_ATTRIBUTE_HIDDEN ?
            'H' : ' ', pdmfa->Attributes & FILE_ATTRIBUTE_READONLY ?
            'R' : ' ', sz);
        fSuccess = TRUE;
    }
}

BOOL OPTH::FParseOption(char *&sz, char ch)
{
    DWORD dw;

    switch(ch) {
    case '+':
    case '-':
        if(0 == _stricmp(sz, "ro")) {
            dw = FILE_ATTRIBUTE_READONLY;
            sz += 1;
        } else if(0 == _stricmp(sz, "hid")) {
            dw = FILE_ATTRIBUTE_HIDDEN;
            sz += 2;
        } else
            dw = 0;
        if(dw) {
            dwMask |= dw;
            if(ch == '+')
                dwAttributes |= dw;
            else
                dwAttributes &= ~dw;
            return TRUE;
        }
        if(ch == '+')
            return FALSE;
        break;
    }
	return FParseFilrOption(*sz);
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbattrib version " VER_PRODUCTVERSION_STR "\n\n"
        "Change file attributes of one or more files on the Xbox development\n"
        "or target system.\n");
	fprintf(stderr, "\nusage: %s [/x xboxname] [options] file [file] ...\n",
		m_szExe);
	fprintf(stderr, "\t/r\trecursive change\n"
		"\t/s\tsearch in subdirectories\n"
        "\t/h\tinclude hidden files\n"
        "\t+ro\tmake file read-only\n"
        "\t-ro\tmake file read-write\n"
        "\t+hid\tmake file hidden\n"
        "\t-hid\tmake file visible\n"
		"Specify Xbox files as xE:\\..., xD:\\..., etc.\n");
	exit(1);
}

int __cdecl main(int cArg, char **rgszArg)
{
	char *szExe = *rgszArg;
    BOOL fRet = 0;

    /* Cut down on repeated connection requests */
    DmUseSharedConnection(TRUE);

	/* Process arguments */
	OPTH opth(cArg, rgszArg, "-/+");

	if(cArg < 1)
		opth.UsageFailure();

	for(; cArg--; ++rgszArg) {
		OFIL ofil(*rgszArg);
        fSuccess = FALSE;
		ofil.DoTree();
        fRet = fRet || !fSuccess;
	}

	DmUseSharedConnection(FALSE);

    return fRet;
}
