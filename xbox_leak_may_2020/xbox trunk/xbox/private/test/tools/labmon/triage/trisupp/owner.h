#ifndef _MYOWNER_
	#define _MYOWNER_


CHAR *AssignStack(CHAR **StackEntries, UINT numEntries, CHAR *ownerBuf, int cBuf);
CHAR *GetOwner(CHAR *symbol, CHAR *ownerBuf, int cBuf);
BOOL GetSpecial(const CHAR *symbol, CHAR *ownerBuf, int cBuf);

#endif