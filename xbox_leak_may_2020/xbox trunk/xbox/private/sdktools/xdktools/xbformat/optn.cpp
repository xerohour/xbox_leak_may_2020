/*
 *
 * optn.cpp
 *
 * Option handling
 *
 */

#include <windows.h>
#include <xboxdbg.h>
#include <stdlib.h>
#include <stdio.h>
#include "optn.hxx"

OPTH::OPTH(int &cArg, char **&rgszArg, const char *szOptionChars)
{
	m_szExe = *rgszArg;
    const char *pchOptionMarker;
    char *szXboxEnv;

    /* Grab Xbox name from environment if it's there */
    szXboxEnv = getenv("XBOXIP");
    if(szXboxEnv && FAILED(DmSetXboxNameNoRegister(szXboxEnv))) {
	    fprintf(stderr, "error in Xbox name: %s\n", szXboxEnv);
        exit(1);
    }

	/* Process arguments */
	++rgszArg;
	while(--cArg && (pchOptionMarker = strchr(szOptionChars, **rgszArg))) {
		char *szArg = *rgszArg++;
		while(*++szArg) {
			switch(*szArg) {
			case 'x':
			case 'X':
				if(!--cArg)
					UsageFailure();
				if(FAILED(DmSetXboxName(*rgszArg++))) {
					fprintf(stderr, "error in Xbox name\n");
					exit(1);
				}
				break;
#if 0
			case '+':
				_asm int 3;
				break;
#endif
			default:
				if(!FParseOption(szArg, *pchOptionMarker))
					UsageFailure();
				break;
			}
		}
	}
}
