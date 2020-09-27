/*
**
** xbmkdir
**
** Make directories on the Xbox
**
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "xbfile.hxx"
#include "optn.hxx"
#include <xboxverp.h>

BOOL fEnsurePath;

BOOL OPTH::FParseOption(char *&sz, char)
{
	switch(*sz) {
	case 't':
	case 'T':
		++fEnsurePath;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbmkdir version " VER_PRODUCTVERSION_STR "\n\n"
        "Creates a directory on the Xbox development or target system.\n");
	fprintf(stderr, "\nusage: %s [-x xboxname] directory\n", m_szExe);
	fprintf(stderr, "Specify Xbox files as xE:\\..., xD:\\..., etc.\n");
	exit(1);
}

int __cdecl main(int cArg, char **rgszArg)
{
	FIL fil;
	HRESULT hr;
	DM_FILE_ATTRIBUTES dmfa;

    /* Cut down on repeated connection requests */
    DmUseSharedConnection(TRUE);

	/* Process arguments */
	OPTH opth(cArg, rgszArg);

	if(cArg != 1)
		opth.UsageFailure();

	fil = *rgszArg;

	if(fEnsurePath) {
		FIL filDir(fil);
		filDir.m_szName[0] = 0;
		hr = filDir.HrEnsureDir();
		if(SUCCEEDED(hr))
			hr = fil.HrMkdir();
	} else
		hr = fil.HrMkdir();

	if ((XBDM_ALREADYEXISTS == hr) &&
		(SUCCEEDED(fil.HrGetFileAttributes(&dmfa))) &&
		(0 != (FILE_ATTRIBUTE_DIRECTORY & dmfa.Attributes)))
	{
		/* The directory already exists - just warn, don't return failure */
		fil.Warning(hr, 0);
	}
	else if(FAILED(hr))
	{
		fil.Fatal(hr, 0);
	}

	DmUseSharedConnection(FALSE);

    return 0;
}
