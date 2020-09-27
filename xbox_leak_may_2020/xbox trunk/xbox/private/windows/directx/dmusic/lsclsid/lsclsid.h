// Copyright (c) 1998 Microsoft Corporation
//
//
//

#ifndef _LSCLSID_
#define _LSCLSID_

// VerInfo.cpp
extern char* FileVersionInfo(LPSTR pstrName);

#define MAX_LONGVERINFO_LINES 6
extern int LongFileVersionInfo(
		LPSTR pstrName, 
		char *ppTags[MAX_LONGVERINFO_LINES], 
		char *ppLines[MAX_LONGVERINFO_LINES], 
		unsigned *pMaxTagLen);

#endif
