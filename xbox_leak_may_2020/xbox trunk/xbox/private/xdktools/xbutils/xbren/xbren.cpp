/*
**
** xbren
**
** Rename files on the Xbox
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
#include <xboxverp.h>

BOOL fSuccess;
FIL *g_pfilDst;
const char *g_pchWExt;

struct OFIL : public FILR {
	OFIL(void) : FILR() {}
	OFIL(const char *sz) : FILR(sz) {}

	void DoWork(void);
	FILR *PfilrCopy(void) const { return new OFIL(*this); }

    const char *m_pchPat;
};

void OFIL::DoWork(void)
{
    FIL filNew(*this);
    HRESULT hr;

    if(m_pchPat) {
        int rgichStart[16];
        int rgichLen[16];
        int cich;
        const char *pchExt;
        char *pch, *pchOut;

        /* We match the original name once again so we can figure out what the
         * wildcards matched */
        cich = 0;
        if(g_pchWExt) {
            int cch;
            Basename(m_szName, NULL, &pchExt);
            cch = pchExt ? pchExt - m_szName - 1 : strlen(m_szName);
            if(FPatMatch(m_pchPat, g_pchWExt - m_pchPat - 1, m_szName, cch,
                rgichStart, rgichLen, 15))
            {
                while(cich < 15 && rgichStart[cich] >= 0)
                    ++cich;
            }
            if(pchExt && FPatMatch(g_pchWExt, strlen(g_pchWExt), pchExt,
                strlen(pchExt), &rgichStart[cich], &rgichLen[cich], 15 - cich))
            {
                while(cich < 15 && rgichStart[cich] >= 0) {
                    rgichStart[cich] += pchExt - m_szName;
                    ++cich;
                }
            }
        } else {
            if(FPatMatch(m_pchPat, strlen(m_pchPat), m_szName,
                strlen(m_szName), rgichStart, rgichLen, 15))
            {
                while(cich < 15 && rgichStart[cich] >= 0)
                    ++cich;
            }
        }
        rgichStart[cich] = -1;

        /* With the matching out of the way, we can now do the substitution */
        cich = 0;
        pch = g_pfilDst->m_szName;
        pchOut = filNew.m_szName;
        while(*pch) {
            if(*pch == '?' || *pch == '*') {
                /* Skip past the wildcard */
                while(*pch == '?' || *pch == '*')
                    ++pch;
                /* Now stick in the substitute characters */
                if(rgichStart[cich] >= 0) {
                    memcpy(pchOut, m_szName + rgichStart[cich], rgichLen[cich]);
                    pchOut += rgichLen[cich];
                    ++cich;
                }
            } else
                *pchOut++ = *pch++;
        }
        *pchOut = 0;
    } else {
        strcpy(filNew.m_szName, g_pfilDst->m_szName);
    }

    hr = HrRename(filNew);
    if(hr == XBDM_ALREADYEXISTS || hr == XBDM_BADFILENAME)
        filNew.Warning(hr, 0);
    else if(FAILED(hr))
        Warning(hr, 0);
    else
        fSuccess = TRUE;
}

BOOL OPTH::FParseOption(char *&sz, char)
{
    switch(*sz) {
    case 's':
    case 'S':
    case 'h':
    case 'H':
        return FParseFilrOption(*sz);
    }
    return FALSE;
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbren version " VER_PRODUCTVERSION_STR "\n\n"
        "Renames a file or files on the Xbox development or target system.\n");
	fprintf(stderr, "\nusage: %s [/x xboxname] [options] oldname newname\n",
        m_szExe);
    fprintf(stderr, "newname must not contain any path elements\n"
        "\t/s search in subdirectories\n"
	    "Specify Xbox files as xE:\\..., xD:\\..., etc.\n");
	exit(1);
}

int __cdecl main(int cArg, char **rgszArg)
{
	char *szExe = *rgszArg;
	HRESULT hr;

    /* Cut down on repeated connection requests */
    DmUseSharedConnection(TRUE);

	/* Process arguments */
	OPTH opth(cArg, rgszArg);

	if(cArg != 2)
        opth.UsageFailure();

	OFIL ofilOld(rgszArg[0]);
	FIL filNew(rgszArg[1]);

    /* The new filename must be just a single local filename */
    if(filNew.m_fXbox || strcmp(filNew.m_szDir, ".")) {
        fprintf(stderr, "error: new name cannot contain path elements\n");
        exit(1);
    }

    /* Zero or two wildcards */
    if(ofilOld.FIsWildName()) {
        if(!filNew.FIsWildName()) {
            fprintf(stderr, "error: destination can't be wildcard\n");
            exit(1);
        }
        ofilOld.m_pchPat = ofilOld.m_szName;
        Basename(ofilOld.m_szName, NULL, &g_pchWExt);
    } else {
        if(filNew.FIsWildName()) {
            fprintf(stderr, "error: destination must be wildcard\n");
            exit(1);
        }
        ofilOld.m_pchPat = NULL;
    }

    g_pfilDst = &filNew;
    ofilOld.DoTree(FALSE);

    return !fSuccess;
}
