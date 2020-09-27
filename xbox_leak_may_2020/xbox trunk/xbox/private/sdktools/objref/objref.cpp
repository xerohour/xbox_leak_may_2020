/*
 *
 * objref.cpp
 *
 * Scan a list of obj files to compute the symbolic closure -- defined &
 * undefined symbols
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <ctype.h>

typedef int BOOL;
typedef unsigned long DWORD;

typedef struct {
	char *szSym;
	char *szObj;
	BOOL fData;
	BOOL fDef;
} SOM;

SOM **rgpsom;
DWORD isomMax;
DWORD csom;

BOOL fVerbose = 0;

DWORD key(char *sz)
{
	int i;

	for(i = 0; *sz; ++sz)
		i = (i * 17) ^ *sz;
	return i;
}

void hashadd(SOM *psom)
{
	if(csom >= isomMax / 2) {
		if(rgpsom) {
			SOM **rgpsomOld = rgpsom;
			int isomOld = isomMax;
			rgpsom = new SOM *[isomMax += 128];
			memset(rgpsom, 0, sizeof(SOM *) * isomMax);
			csom = 0;
			while(isomOld--)
				if(rgpsomOld[isomOld] && rgpsomOld[isomOld] != (SOM *)rgpsomOld)
					hashadd(rgpsomOld[isomOld]);
			delete rgpsomOld;
		} else {
			rgpsom = new SOM *[isomMax = 128];
			memset(rgpsom, 0, sizeof(SOM *) * isomMax);
		}
	}

	DWORD isom = key(psom->szSym) % isomMax;
	SOM **ppsomKeep = NULL;

	for(;;) {
		if(rgpsom[isom] == NULL) {
			if(!ppsomKeep)
				ppsomKeep = &rgpsom[isom];
			break;
		} else if(rgpsom[isom] == (SOM *)rgpsom) {
			if(!ppsomKeep)
				ppsomKeep = &rgpsom[isom];
		} else if(!strcmp(psom->szSym, rgpsom[isom]->szSym))
			return;
		isom = (isom + 1) % isomMax;
	}

	++csom;
	*ppsomKeep = new SOM(*psom);
	(*ppsomKeep)->szSym = strdup((*ppsomKeep)->szSym);
	(*ppsomKeep)->szObj = strdup((*ppsomKeep)->szObj);
}

SOM *hashfind(char *szSym)
{
	if(!isomMax)
		return NULL;

	DWORD isom = key(szSym) % isomMax;

	for(;;) {
		if(rgpsom[isom] == NULL)
			return NULL;
		else if(rgpsom[isom] != (SOM *)rgpsom && !strcmp(szSym,
				rgpsom[isom]->szSym))
			return rgpsom[isom];
		isom = (isom + 1) % isomMax;
	}
}

void readbss(const char *szName)
{
	FILE *pfl;

	pfl = fopen(szName, "r");
	if(pfl) {
		char sz[256];
		while(fgets(sz, sizeof sz, pfl)) {
			char *pchFirst;
			char *pchLast;
			for(pchFirst = sz; *pchFirst && isspace(*pchFirst); ++pchFirst);
			for(pchLast = pchFirst; *pchLast && !isspace(*pchLast); ++pchLast);
			*pchLast = 0;

			SOM *psom = hashfind(pchFirst);
			if(psom)
				fprintf(stderr, "warning: %s already defined in bss\n",
					pchFirst);
			else {
				SOM som;
				som.szSym = pchFirst;
				som.szObj = "<common>";
				som.fData = 0;
				som.fDef = 1;
				hashadd(&som);
			}
		}
		fclose(pfl);
	}
}

int __cdecl main(int argc, char **argv)
{
	if(argc > 1 && _strnicmp(argv[1], "-bss:", 5) == 0) {
		readbss(argv[1] + 5);
		++argv;
		--argc;
	}
	for(; ++argv, --argc; ) {
		struct _finddata_t fd;
		long fi;

		fi = _findfirst(*argv, &fd);
		if(fi == -1)
			fprintf(stderr, "error on %s\n", *argv);
		while(fi != -1) {
			FILE *pfl;
			char sz[256];

			sprintf(sz, "link -dump -symbols %s", fd.name);
			pfl = _popen(sz, "r");
			if(!pfl)
				fprintf(stderr, "error with dump on %s\n", fd.name);
			else {
				BOOL fDataSection = 0;

				if(fVerbose)
					printf("Processing %s\n", fd.name);
				while(fgets(sz, sizeof sz, pfl)) {
					if(sz[0] != ' ') {
						char *szSect;
						char *szType;
						char *szSym;
						char *pchMac = sz + strlen(sz);
						/* We have a valid line.  We only care about the
						 * section number, type, and symbol name */
						szSect = sz + 13;
						szType = sz + 33;
						szSym = sz + 46;
						if(szSect >= pchMac || szType >= pchMac || szSym >=
								pchMac)
							/* Not valid */
							continue;
						/* The symbol may start later; we look for the '|'
						 * character */
						while(*szSym && *szSym != '|')
							++szSym;
						if(*szSym == '|')
							++szSym;
						while(*szSym && isspace(*szSym))
							++szSym;
						if(!*szSym)
							continue;

						BOOL fExtern;
						if(strncmp(szSect, "UNDEF", 5) == 0)
							/* Undefined symbols in a data section we treat
							 * like bss data because we don't know any
							 * better */
							//fExtern = !fDataSection;
							// too bad we can't identify bss data
							fExtern = 1;
						else if(strncmp(szSect, "SECT", 4) == 0)
							fExtern = 0;
						else
							/* Invalid section */
							continue;

						BOOL fSectionMark;

						if(strncmp(szType, "Static", 6) == 0)
							fSectionMark = 1;
						else if(strncmp(szType, "External", 8) == 0)
							fSectionMark = 0;
						else
							/* Uninteresting data */
							continue;

						for(pchMac = szSym; *pchMac && !isspace(*pchMac);
							++pchMac);
						*pchMac = 0;

						if(fSectionMark) {
							/* Section marker */
							if(strcmp(szSym, ".data") == 0 ||
									strcmp(szSym, ".rdata") == 0)
								fDataSection = 1;
							else
								fDataSection = 0;
							continue;
						}

						SOM *psom;
						/* We've parsed this line.  See if we can find this
						 * symbol anywhere else */
						if(fVerbose)
							printf("  sym:%s %s\n", szSym, fExtern ? "referenced" : "defined");
						psom = hashfind(szSym);
						if(psom) {
							if(psom->fDef && !fExtern)
								/* Multiply defined */
								fprintf(stderr, "warning: %s defined in both "
									"%s and %s\n", szSym, psom->szObj,
									fd.name);
							else if(!psom->fDef && !fExtern) {
								/* Here's our definition */
								free(psom->szObj);
								psom->szObj = strdup(fd.name);
								psom->fDef = 1;
							}
						} else {
							/* First reference to this symbol, so add it */
							SOM som;
							som.fData = 0;
							som.fDef = !fExtern;
							som.szObj = fd.name;
							som.szSym = szSym;
							hashadd(&som);
						}
					}
				}
				_pclose(pfl);
			}
			if(0 != _findnext(fi, &fd))
				fi = -1;
		}
	}

    DWORD isom;
#if 0
    /* Now print all of our defined symbols */
	printf("\nSymbols defined:\n\n");

	for(isom = 0; isom < isomMax; ++isom)
		if(rgpsom[isom] && rgpsom[isom] != (SOM *)rgpsom && rgpsom[isom]->fDef)
			printf(" %s (%s)\n", rgpsom[isom]->szSym, rgpsom[isom]->szObj);

	/* Now print all of our undefined symbols */
	printf("\nUndefined symbols:\n\n");
#endif // 0
    for(isom = 0; isom < isomMax; ++isom)
		if(rgpsom[isom] && rgpsom[isom] != (SOM *)rgpsom && !rgpsom[isom]->fDef &&
		    (0 != strncmp("__imp_", rgpsom[isom]->szSym, 6)))
			printf(" %s (%s)\n", rgpsom[isom]->szSym, rgpsom[isom]->szObj);

	return 0;
}
