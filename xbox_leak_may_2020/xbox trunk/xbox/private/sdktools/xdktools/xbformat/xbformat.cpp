/*
**
** xbformat
**
** Reformat drives on the Xbox
**
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "optn.hxx"
#include <xboxverp.h>

BOOL OPTH::FParseOption(char *&sz, char)
{
    return FALSE;
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbformat version " VER_PRODUCTVERSION_STR "\n\n"
        "Reformats a FAT partition on the Xbox\n");
	fprintf(stderr, "\nusage: %s [/x xboxname] partition-number\n", m_szExe);
	exit(1);
}

int __cdecl main(int cArg, char **rgszArg)
{
    DWORD dwPartition;
    HRESULT hr;
    char szCmd[1024];

	/* Process arguments */
	OPTH opth(cArg, rgszArg);

	if(cArg < 1 || !sscanf(rgszArg[0], "%d", &dwPartition) || dwPartition == 0)
		opth.UsageFailure();

    sprintf(szCmd, "fmtfat partition=%d", dwPartition);
    hr = DmSendCommand(NULL, szCmd, NULL, NULL);
    if(FAILED(hr)) {
        fprintf(stderr, "error: error code 0x%08x\n", hr);
        exit(1);
    }
    return 0;
}
