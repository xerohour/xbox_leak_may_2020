#ifndef __LOCALE_INFO__
#define __LOCALE_INFO__
/*
*/
#include "uilocale.h"

class CLocaleInfo
{
public:
	CLocaleInfo();
	CLocaleInfo(HINSTANCE hInst, IServiceProvider *pServiceProvider);
	~CLocaleInfo();

	void SetMainInst(HINSTANCE hInst) {m_hInstMain = hInst;};

	HINSTANCE GetUIHostLib(); // returns NULL if can't find.

protected:

	HINSTANCE m_hInstMain;
	HINSTANCE m_hInst;
};

#endif
