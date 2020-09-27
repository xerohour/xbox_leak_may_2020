/*
**
** xbcapture
**
** Capture the screen of Xbox
**
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "optn.hxx"
#include "resource.h"
#include <xboxverp.h>
#include "xbfile.hxx"

BOOL OPTH::FParseOption(char *&sz, char)
{
    return TRUE;
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbcapture version " VER_PRODUCTVERSION_STR "\n\n"
            "Captures the current image on the XBox's front buffer and saves\n"
            "it as the specified bitmap file.\n\n");
    fprintf(stderr, "\nusage: %s [/x xboxname] bitmap\n", m_szExe);
    exit(1);
}

int __cdecl main(int cArg, char **rgszArg)
{
    DM_XBE xbe;
    HRESULT hr;

    /* Cut down on repeated connection requests */
    DmUseSharedConnection(TRUE);

    /* Process arguments */
    OPTH opth(cArg, rgszArg);

    if(cArg < 1)
        opth.UsageFailure();

    // verify destination filename ends in '.bmp'
    if (strlen(*rgszArg) < 4 || lstrcmpi(*rgszArg + strlen(*rgszArg) - 4, ".bmp"))
        fprintf(stderr, "Warning: The specified filename does not have a .bmp extension. The file may be unusable.\r\n");

    /* Screen Capture */
    hr = DmScreenShot(*rgszArg);
    if(FAILED(hr)) {
        if(hr == XBDM_FILEERROR)
            hr = HrLastError();
        Fatal(hr, 0, *rgszArg);
    }

    return 0;
}
