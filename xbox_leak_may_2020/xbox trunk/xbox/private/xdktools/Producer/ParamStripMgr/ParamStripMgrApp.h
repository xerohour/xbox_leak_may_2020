// ParamStripMgrApp.h : Declaration of the CParamStripMgrApp

#ifndef __PARAMSTRIPMGRAPP_H_
#define __PARAMSTRIPMGRAPP_H_

#include <dmusprod.h>

class CParamStripMgrApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	IDMUSProdPropPageManager*	m_pIPageManager;
	CFont*	m_pParamStripFont;				// Param strip font
};

extern class CParamStripMgrApp theApp;

#endif // __PARAMSTRIPMGRAPP_H_
