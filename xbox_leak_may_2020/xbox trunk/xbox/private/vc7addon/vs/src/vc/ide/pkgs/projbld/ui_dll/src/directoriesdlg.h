// BuildOptions.h : Declaration of the CDirectoriesDlg

#ifndef __DIRSDLG_H_
#define __DIRSDLG_H_

#include "rsrc.h"       // main symbols
#undef WM_OCC_LOADFROMSTREAM
#undef WM_OCC_LOADFROMSTORAGE         
#undef WM_OCC_INITNEW                 
#undef WM_OCC_LOADFROMSTREAM_EX       
#undef WM_OCC_LOADFROMSTORAGE_EX      

#include <atlhost.h>

#include "DirControl.h"

enum DIRLIST_TYPE
{
	DIRLIST_PATH = 0,
	DIRLIST_INC = 1,
	DIRLIST_REF = 2,
	DIRLIST_LIB = 3,
	DIRLIST_SRC = 4,
	C_DIRLIST_TYPES = 5
};

struct sPathBlock
{
	CComBSTR m_platformName;
	CComBSTR m_dirLists[C_DIRLIST_TYPES]; 		// path, inc, ref, lib, src
	
};

#define _MAX_PLATFORMS 6

/////////////////////////////////////////////////////////////////////////////
// CDirectoriesDlg
class CDirectoriesDlg : 
	public CAxDialogImpl<CDirectoriesDlg>
{
public:
	CDirectoriesDlg();
	~CDirectoriesDlg();

	enum { IDD = IDDP_OPTIONS_DIRECTORIES };

BEGIN_MSG_MAP(CDirectoriesDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_SETFONT, OnFont)
	MESSAGE_HANDLER( WM_SIZE, OnSize )
  	NOTIFY_CODE_HANDLER(PSN_APPLY, OnApply)	
	NOTIFY_CODE_HANDLER(PSN_HELP, OnHelp)
	COMMAND_HANDLER(IDC_DIRS_TYPE, CBN_SELCHANGE, OnSelChangeDirOptions)
	COMMAND_HANDLER(IDC_DIRS_TOOLSETS, CBN_SELCHANGE, OnSelChangeDirToolset)
	CHAIN_MSG_MAP(CAxDialogImpl<CDirectoriesDlg>)
END_MSG_MAP()


//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

	LRESULT OnSelChangeDirOptions(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelChangeDirToolset(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
	LRESULT OnCheckEntries(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCheckEntriesDblClk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnApply(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnHelp(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	static HRESULT WritePathsToRegistry( void );

protected:
	static HRESULT GetPlatformsEnumerator(IEnumVARIANT** ppEnum);

	void ResetListbox();
	void FillOptionsList();
	void RetrieveListboxInfo(int nToolset, DIRLIST_TYPE type);
	void CleanUpString(int nToolset, DIRLIST_TYPE type);
	void LoadTitle(UINT idDesc, BSTR *bstrName);
	void LoadDescription(UINT idDesc, BSTR *bstrDesc);
	void DoOnSize();

	int		m_nToolset;		// Current 'toolset' selected.
	int		m_nLastToolset;		// Last 'toolset' selected

	DIRLIST_TYPE m_type;		// Current DIRLIST_TYPE selected.
	DIRLIST_TYPE m_LastType;	// Last DIRLIST_TYPE selected.

	sPathBlock m_pathBlockList[ _MAX_PLATFORMS ];
	CComBSTR m_strDescription[C_DIRLIST_TYPES]; // path, inc, lib, src	- Review FIXME Im ANSI
	CComBSTR m_strName[C_DIRLIST_TYPES];	// friendly name for directory list -- yes, ANSI

	CWindow m_listDirs;
	CAxWindow m_wndDirList;
	CComPtr<IDirList> m_pList;
};

#endif //__DIRSDLG_H_
