#ifndef customiz_h
#define customiz_h

// The tools.customize.commands and tools.customize.keyboard tabs derive
//  from this, so that they can be called from Tools.Macro and be primed
//  with the selected macro.
interface IAut1Misc;		// Defined in ide\pkgs\include\aut1api_.h
class CCommandTab : public CDlgTab
{
protected:
	DECLARE_DYNAMIC(CCommandTab);
	CString m_strInitialCommand;
	static IAut1Misc* m_pAut1Misc;

// Construction
public:
	virtual ~CCommandTab();
	CCommandTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption) { ASSERT(m_pAut1Misc == NULL); }
	CCommandTab(LPCSTR lpszTemplateName, UINT nIDCaption)
		: CDlgTab(lpszTemplateName, nIDCaption) { ASSERT(m_pAut1Misc == NULL); }

// Operations
public:
	// This is called before the tab is created.  Then, when the tab
	//  is created, it will select the command szCmd and select
	//  the containing category.  This category is currently always
	//  Macros, as this is used to implement the shortcut to assigning
	//  macros to keystrokes/commandbars from the Tools.Macro dialog.
	void SelectCommand(LPCTSTR szCmd) { m_strInitialCommand = szCmd; }

protected:
	// Called by derived classes to reload macros when their tab is displayed.
	void ReloadMacros();
};

#endif
