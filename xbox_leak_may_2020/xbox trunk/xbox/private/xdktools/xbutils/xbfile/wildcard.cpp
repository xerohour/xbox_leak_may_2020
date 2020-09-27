/*
**
** wildcard.c
**
** File wildcard matching
**
*/

#include "precomp.hxx"
#include <string.h>

void Basename(LPCSTR szPath, const char **ppchFile, const char **ppchExt)
{
	const char *szDir = NULL;
	const char *szStart = szPath;
	const char *szDot = NULL;

	for(; *szPath; ++szPath) {
		if(*szPath == '\\')
			szDir = szPath;
		else if(*szPath == '.')
			szDot = szPath;
	}
	/* Weed out extensions for . and .. */
	if(szDir && (!strcmp(szDir+1, ".") || !strcmp(szDir+1, "..")))
		szDot = NULL;
	if(ppchFile)
		*ppchFile = szDir ? szDir + 1 : szStart;
	if(ppchExt)
		*ppchExt = szDot ? szDot + 1 : NULL;
}

BOOL FIsWildName(LPCSTR sz)
{
	const char *pchFile;
	
	/* We don't take wildcards in directory names */
	Basename(sz, &pchFile, NULL);
    while(*pchFile) {
		if(*pchFile == '*' || *pchFile == '?')
			return TRUE;
        ++pchFile;
    }
	return FALSE;
}

BOOL FMatchSubstr(const char *pchPat, const char *pchMatch, int cch)
{
	while(cch > 0) {
		int ichQ;

		for(ichQ = 0; ichQ < cch; ++ichQ)
			if(pchPat[ichQ] == '?')
				break;
		if(ichQ && 0 != _strnicmp(pchPat, pchMatch, ichQ))
			return FALSE;
		pchPat += ichQ + 1;
		pchMatch += ichQ + 1;
		cch -= ichQ + 1;
	}
	return TRUE;
}

BOOL FPatMatch(const char *pchPat, int cchPat, const char *pchName,
	int cchName, int *rgichStart, int *rgichLen, int cichWilds)
{
	int cchSkip;
	int cchSubstr;
    const char *pchStart = pchName;
    const char *pchWildMatch;

	/* OK, here's our loop */
    for(;;) {
        cchSkip = 0;

        /* Find out how much wildcard we have */
        while(cchPat && (*pchPat == '*' || *pchPat == '?')) {
            if(*pchPat == '*')
                cchSkip = cchName;
            else
                ++cchSkip;
            ++pchPat, --cchPat;
        }
        /* If we have no non-wildcard text left, we're done */
   	    if(cchPat == 0) {
            if(cichWilds && cchName <= cchSkip) {
                if(cchName) {
                    *rgichStart = pchName - pchStart;
                    *rgichLen = cchName;
                    --cichWilds;
                }
                if(cichWilds)
                    rgichStart[1] = -1;
            }
		    return cchName <= cchSkip;
        }

		/* Find a section of pattern to match */
        cchSubstr = 0;
		while(cchSubstr < cchPat && pchPat[cchSubstr] != '*' &&
                pchPat[cchSubstr] != '?')
			++cchSubstr;
		if(cchName < cchSubstr)
			/* The pattern to match is longer than the name itself, so this
			 * is not a match */
			return FALSE;
        pchWildMatch = cchSkip ? pchName : NULL;
		if(cchSubstr == cchPat && cchSkip + cchSubstr >= cchName) {
			/* This is a terminal match, so we can just skip to the last
             * part of the string, but not over more characters than we're
             * allowed */
			pchName += cchName - cchSubstr;
            cchSkip -= cchName - cchSubstr;
			cchName = cchSubstr;
        }
        while(cchSubstr <= cchName) {
            if(0 == _strnicmp(pchPat, pchName, cchSubstr)) {
                /* We have a match, so skip past the bit we just matched */
			    cchPat -= cchSubstr;
			    pchPat += cchSubstr;
                cchName -= cchSubstr;
                pchName += cchSubstr;
                cchSubstr = 0;
                break;
            }
			/* As long as there's no match, we keep advancing */
			if(cchSkip-- == 0)
				/* We needed to match from the first character, so this is
				 * an outright non-match */
				return FALSE;
			++pchName, --cchName;
		}
        if(cchSubstr)
            /* We have unmatched text, so we need to give up */
            return FALSE;
        if(pchWildMatch && cichWilds) {
            /* We just matched some text, so let's remember what it is */
            const char *pchWildEnd = pchName - cchSubstr;
            *rgichStart++ = pchWildMatch - pchStart;
            *rgichLen++ = pchWildEnd - pchWildMatch;
            --cichWilds;
        }
	}
}

BOOL FWildMatch(LPCSTR szPattern, LPCSTR szName)
{
	BOOL fWild = FALSE;
	const char *pchExtPat;
	const char *pchExtName;
	int cchName;

	/* We walk the pattern string.  For each non-wildcard sequence we see, we
	 * look to see whether we can find a matching sequence in the name.  We
	 * deal with the base and extension separately.  Plus, we do some
	 * a priori matching based on the presence or absence of extensions */
	
	Basename(szPattern, NULL, &pchExtPat);
	Basename(szName, NULL, &pchExtName);

	if(!pchExtPat)
		/* The pattern has no extension, so we'll treat the name as if it's
		 * a unified string as well */
		 return FPatMatch(szPattern, strlen(szPattern), szName,
			 strlen(szName));
	cchName = pchExtName ? pchExtName - szName - 1 : strlen(szName);
	if(!FPatMatch(szPattern, pchExtPat - szPattern - 1, szName, cchName))
		return FALSE;
	return pchExtName ? FPatMatch(pchExtPat, strlen(pchExtPat), pchExtName,
		strlen(pchExtName)) : FPatMatch(pchExtPat, strlen(pchExtPat), NULL,
		0);
}
