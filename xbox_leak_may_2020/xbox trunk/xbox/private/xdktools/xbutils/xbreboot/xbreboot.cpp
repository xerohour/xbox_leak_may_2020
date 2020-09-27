/*
**
** xbreboot
**
** Reboot the remote Xbox machine
**
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "xbfile.hxx"
#include "optn.hxx"
#include <xboxverp.h>

DWORD g_dwFlags = DMBOOT_WARM;
HANDLE hevtReboot;
DWORD g_dwState = DMN_EXEC_START;

DWORD Reboot(DWORD dw, DWORD dwParam)
{
	if(dwParam == DMN_EXEC_PENDING && g_dwState == DMN_EXEC_REBOOT)
		/* We're ready to roll */
		SetEvent(hevtReboot);
	g_dwState = dwParam;
	return 0;
}

void SetupWait(PDMN_SESSION *ppsess)
{
    HRESULT hr;

	hevtReboot = CreateEvent(NULL, TRUE, FALSE, 0);
	if(!hevtReboot)
		Fatal(E_UNEXPECTED, 0);
	hr = DmOpenNotificationSession(DM_PERSISTENT, ppsess);
	if(FAILED(hr))
		Fatal(hr, 0);
	hr = DmNotify(*ppsess, DM_EXEC, Reboot);
	if(FAILED(hr))
		Fatal(hr, 0);
}

BOOL OPTH::FParseOption(char *&sz, char)
{
	switch(*sz) {
	case 'w':
	case 'W':
		g_dwFlags |= DMBOOT_WAIT;
		break;
	case 'c':
	case 'C':
		g_dwFlags &= ~DMBOOT_WARM;
		break;
    case 'p':
    case 'P':
        g_dwFlags |= DMBOOT_NODEBUG;
        break;
    case 's':
    case 'S':
        g_dwFlags |= DMBOOT_STOP;
        break;
	default:
		return FALSE;
	}
	return TRUE;
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbreboot version " VER_PRODUCTVERSION_STR "\n\n"
        "Reboot the Xbox target system and optionally launch an Xbox title\n");
	fprintf(stderr, "\nusage: %s [/x xboxname] [options] [xbe]\n", m_szExe);
	fprintf(stderr,
		"\t-w\twait for debugger connection after reboot\n"
		"\t-c\tcold reboot (slow)\n"
        "\t-p\tprevent debugging after reboot\n"
		"Specify Xbox files as xE:\\..., xD:\\..., etc.\n");
	exit(1);
}

int __cdecl main(int cArg, char **rgszArg)
{
	char *szExe = *rgszArg;
	DWORD dwFlags = DMBOOT_WARM;
	HRESULT hr;
	PDMN_SESSION psess;
	char szName[1024];
    char szArgs[2048];
    FIL fil;

	/* Cut down on repeated connection requests */
	DmUseSharedConnection(TRUE);

	/* Process arguments */
	OPTH opth(cArg, rgszArg);

	if(cArg > 0) {
		/* We're going to need to run an image, so set up for the
		 * notification */
        fil = *rgszArg;
        if(!fil.m_fXbox) {
            fprintf(stderr, "error: can only reboot to files on Xbox\n");
            exit(1);
        }
        if(g_dwFlags & DMBOOT_NODEBUG) {
            if(cArg > 1) {
                /* Can't use -p and pass arguments */
                fprintf(stderr, "error: can't use -p and pass arguments\n");
                exit(1);
            }
            /* Use the secret recipe */
            fil.FillFullName(szName);
            sprintf(szArgs, "magicboot title=\"%s\"", szName);
            hr = DmSendCommand(NULL, szArgs, NULL, NULL);
            if(FAILED(hr))
                fil.Fatal(hr, 0);
            exit(0);
        }
		g_dwFlags |= DMBOOT_WAIT;
        SetupWait(&psess);
    } else if(g_dwFlags & DMBOOT_STOP)
        SetupWait(&psess);

    if(g_dwState == DMN_EXEC_PENDING && (dwFlags & DMBOOT_WARM))
        /* We're already sitting there waiting to run a title, so we won't
         * bother actually doing the reboot */
        hr = XBDM_NOERR;
    else
    	hr = DmReboot(g_dwFlags);

	if (FAILED(hr))
	{
		Fatal(hr, 0);
	}
	if(hevtReboot) {
		if(WaitForSingleObject(hevtReboot, g_dwFlags & DMBOOT_WARM ? 30000 :
				120000) == WAIT_TIMEOUT)
			Fatal(XBDM_CONNECTIONLOST, 0);
		else if(cArg > 0) {
			char szDir[1024];
            char *pchArg = NULL;
			strcpy(szName, fil.m_szName);
			fil.m_szName[0] = 0;
			fil.FillFullName(szDir);
            fil.m_szName[0] = szName[0];
            while(++rgszArg, --cArg) {
                if(pchArg)
                    sprintf(pchArg, " %s", *rgszArg);
                else
                    strcpy(pchArg = szArgs, *rgszArg);
                while(*pchArg)
                    ++pchArg;
            }
            hr = DmSetTitle(szDir, szName, pchArg ? szArgs : NULL);
			DmGo();
			if(FAILED(hr))
				fil.Fatal(hr, 0);
		}
		DmCloseNotificationSession(psess);
	}

	DmUseSharedConnection(FALSE);

    return 0;
}
