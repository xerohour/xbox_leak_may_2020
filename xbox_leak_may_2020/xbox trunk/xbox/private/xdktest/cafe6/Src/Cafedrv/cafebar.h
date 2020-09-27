/////////////////////////////////////////////////////////////////////////////
// cafebar.h
//
// email	date		change
// briancr	11/03/94	created
//
// copyright 1994 Microsoft

// Interface of the CCAFEBar class

#ifndef __CAFEBAR_H__
#define __CAFEBAR_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "settings.h"


///////////////////////////////////////////////////////////////////////////////
// CCAFEBar class

class CPlatform;

class CCAFEBar : public CToolBar
{
// ctor/dtor
public:
	CCAFEBar(CSettings* pSettings) : m_pfont(NULL), m_pcbToolset(NULL), m_pcbBuildType(NULL), m_pcbLanguage(NULL), m_pSettings(pSettings) { }
	~CCAFEBar();

// operations
public:
	CreateBar(CWnd* pParent);

	BOOL UpdateToolset(void);
	BOOL UpdateBuildType(void);
	BOOL UpdateLanguage(void);

// operations (internal)
protected:
	BOOL SelectToolset(void);
	BOOL SelectBuildType(void);
	BOOL SelectLanguage(void);

	CFont* CreateFont(void);
	CComboBox* CreateToolsetCombo(int nPos);
	CComboBox* CreateBuildTypeCombo(int nPos);
	CComboBox* CreateLanguageCombo(int nPos);
	CComboBox* CreateCombo(UINT nId, int nPos, int nWidth);

	void SizeComboToContent(CComboBox* pCombo, int nMax = 0);

// data
protected:
	CComboBox* m_pcbToolset;
	CComboBox* m_pcbBuildType;
	CComboBox* m_pcbLanguage;
	CFont* m_pfont;

	CSettings* m_pSettings;
};

#endif // __CAFEBAR_H__
