/*
 *
 * xassert.c
 *
 * Pop up an assert message
 *
 */

#include "precomp.h"

char XboxAssert(PDMN_DEBUGSTR pdmds)
{
	int id;
	TCHAR szTitle[64];
	char szAssert[1024];
	int cch = pdmds->Length;

	if(cch > sizeof szAssert - 1)
		cch = sizeof szAssert - 1;

	memcpy(szAssert, pdmds->String, cch);
	szAssert[cch] = 0;
	LoadString(hInstance, IDS_ASSERT_ARI, szAssert + cch, sizeof szAssert -
		cch);
	szTitle[0] = 0;
	LoadString(hInstance, IDS_ASSERT_TITLE, szTitle, sizeof szTitle);

	id = MessageBox(NULL, szAssert, szTitle, MB_ABORTRETRYIGNORE);
	switch(id) {
	case IDABORT:
		return 't';
	case IDRETRY:
		return 'b';
	case IDIGNORE:
		return 'i';
	}
	assert(FALSE);
	return 0;
}

void XboxRip(LPCSTR psz)
{
	char szRip[256];

	szRip[0] = 0;
	LoadString(hInstance, IDS_RIP, szRip, sizeof szRip);
	MessageBox(NULL, psz, szRip, MB_OK);
}
