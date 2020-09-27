#ifndef _UTILS_
	#define _UTILS_ 

#define MAXLINES (DWORD) 512

DWORD GetLinesFromString(CHAR *theString, CHAR **LineList);
BOOL IsHex(CHAR *theStr);
CHAR *stristr(const CHAR *s1, const CHAR *s2);
BOOL DynamicAppend(CHAR **ppBuffer, CHAR *AppendString);

#endif