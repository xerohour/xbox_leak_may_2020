// stdfont.h
#ifndef __STDFONT_H__
#define __STDFONT_H__
// This class is for pre-loading and munging dialog tempalates.
// For now, it is only used to change the font of a dialog...
//
#include <afxpriv.h>
#include "uilocale.h"
#include <atlbase.h>

extern const LPCTSTR g_pszUIDllName;

class CLocaleInfo
{
public:
	CLocaleInfo();
	CLocaleInfo(HINSTANCE hInst, IServiceProvider *pServiceProvider);
	~CLocaleInfo();

	void SetSite(IServiceProvider *pServiceProvider) {m_pServiceProvider = pServiceProvider;};
	void SetMainInst(HINSTANCE hInst) {m_hInstMain = hInst;};
	
	void Close();

	HINSTANCE GetUIHostLib(); // returns NULL if can't find.
	BOOL LoadDialog(LPCTSTR lpDialogTemplateID, DLGTEMPLATE** ppDlgTmplt);
	BOOL GetDialogFont(UIDLGLOGFONT *pLogFont);
	HRESULT ExternalQueryService(REFGUID rsid, REFIID iid, void **ppvObj);

protected:
	void Init();

	HINSTANCE m_hInstMain;
	CComPtr<IUIHostLocale> m_pUIHostLocale;
	CComPtr<IUIHostLocale2> m_pUIHostLocale2;
	HINSTANCE m_hInst;
	CComPtr<IServiceProvider> m_pServiceProvider;
};

class C3dDialogTemplate : public CDialogTemplate
{
public:
	C3dDialogTemplate(const DLGTEMPLATE* pTemplate = NULL, CLocaleInfo  *pLocInfo = NULL);
	C3dDialogTemplate(HGLOBAL hGlobal, CLocaleInfo  *pLocInfo = NULL);
	BOOL Load(LPCTSTR lpDialogTemplateID);
	~C3dDialogTemplate();

	void SetStdFont();
	BOOL SetLogFont(UIDLGLOGFONT & logFont);
	HGLOBAL GetTemplate() const { return m_hTemplate; }
protected:
	void Init(CLocaleInfo  *pLocInfo, const DLGTEMPLATE* pTemplate = NULL, BOOL fFullInit = TRUE);
	CLocaleInfo *m_pLocInfo;
	const DLGTEMPLATE* m_pTemplate;
};

// Switch to enable on-the-fly dialog font changing...
#define MUNGE_DLG_FONTS

// Set standard fonts in dialog template
void SetStdFont(C3dDialogTemplate & dt);

/////////////////////////////////////////////////////////////////////////////
// Standard font handling

enum FontType
{
	font_Normal,			// MS Sans Serif 8, MS ‚S‚V‚b‚N 10, System 10
	font_Bold,
	font_Italic,
		
	font_Small,				// Small Fonts (-9), Terminal (-9), Terminal (-9)
	font_SmallBold,
	font_SmallItalic,
		
	font_Fixed,				// Courier (14), FixedSys (14), FixedSys (14)
	font_FixedBold,
	font_FixedItalic,
	
	font_Large,             // MS Sans Serif 14. MS Sans Serif 14, MS Sans Serif 14,
	font_LargeBold,
	font_LargeItalic,

	font_count				// Not a font, just provides a count
};

CFont* GetStdFont(const int iType);

// NOTE: The LOGFONT returned by this is temporary
const LOGFONT* GetStdLogfont(const int iType, CDC *pDC = NULL);
void GetStdFontInfo(int iType, CString& strFace, int& ptSize);

////////////////////////////////////////////////////////////////////////////
#endif