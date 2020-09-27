#ifndef __MYAW_H__
#define __MYAW_H__

#include "customaw.h"

LPCTSTR LoadTemplateWrapper(LPCTSTR lpszResource, DWORD& dwSize, HINSTANCE hInstance = NULL);

class CMyCustomAppWiz : public CCustomAppWiz
{
public:
	virtual LPCTSTR LoadTemplate(LPCTSTR lpszResource,
		DWORD& dwSize, HINSTANCE hInstance = NULL);
};

CCustomAppWiz* GetAWX();

#endif //__MYAW_H__
