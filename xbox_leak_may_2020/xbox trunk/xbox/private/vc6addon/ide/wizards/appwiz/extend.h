#ifndef __EXTENSION_H__
#define __EXTENSION_H__
#include "myaw.h"


class CUserExtension
{
public:

	// AppWizard APIs
	CUserExtension();
	~CUserExtension();
	void SetCustomAppWizClass(CCustomAppWiz* pAW);
	BOOL Attach(const TCHAR* szUserExtension);
	void Detach();
	CCustomAppWiz* GetAWX() { return m_pAWX; }
	BOOL IsUserAWX()
		{ extern CMyCustomAppWiz MyAWX; return m_pAWX != &MyAWX; }
	HINSTANCE GetInstanceHandle() { return m_hUserDLL; }
	LPCTSTR GetAWXName() { return (LPCTSTR) m_strUserDLL; }
	CString m_strExtensionTitle;
	BOOL m_bCtlWiz;		// Special case: treat ControlWizard differently

protected:
	CCustomAppWiz* m_pAWX;
	CString m_strUserDLL;
	HINSTANCE m_hUserDLL;

	BOOL m_bSetCustomAppWizClassCalled;
	BOOL m_bCalledSetCustomAppWizWithNULL;
};

CCustomAppWiz* GetAWX();
BOOL IsUserAWX();


#endif // __EXTENSION_H__
