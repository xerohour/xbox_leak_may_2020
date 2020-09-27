/*****************************************************************************\
    FILE: xboxdhlp.h

    DESCRIPTION:
        Dialog box helper.  This class will fill in the parts of the dialog
    that pertain to the XBOX information
\*****************************************************************************/

#ifndef _XBOXDIALOGTEMPLATE_H
#define _XBOXDIALOGTEMPLATE_H


/*****************************************************************************\
    CLASS: CXboxDialogTemplate

    DESCRIPTION:
        Dialog box helper.  This class will fill in the parts of the dialog
    that pertain to the XBOX information
\*****************************************************************************/
class CXboxDialogTemplate
{
public:
    HRESULT InitDialog(HWND hdlg, BOOL fEditable, UINT id, CXboxFolder * pff, CXboxPidlList * pfpl);
    HRESULT InitDialogWithFindData(HWND hDlg, UINT id, CXboxFolder * pff, const XBOX_FIND_DATA * pwfd, LPCWIRESTR pwWirePath, LPCWSTR pwzDisplayPath);
    BOOL OnClose(HWND hdlg, HWND hwndBrowser, CXboxFolder * pff, CXboxPidlList * pfpl);
    BOOL HasNameChanged(HWND hdlg, CXboxFolder * pff, CXboxPidlList * pPidlList);

    static int _InitSizeTally(LPVOID pvPidl, LPVOID pvSizeHolder);

private:
    HRESULT _ReinsertDlgText(HWND hwnd, LPCVOID pv, LPCTSTR ptszFormat);
    HRESULT _ReplaceIcon(HWND hwnd, HICON hicon);
    HRESULT _InitIcon(HWND hwnd, CXboxFolder * pff, CXboxPidlList * pflHfpl);
    HRESULT _InitNameEditable(HWND hwnd, CXboxFolder * pff, CXboxPidlList * pflHfpl);
    HRESULT _InitName(HWND hwnd, CXboxFolder * pff, CXboxPidlList * pflHfpl);
    HRESULT _InitType(HWND hwnd, CXboxFolder * pff, CXboxPidlList * pflHfpl);
    HRESULT _InitLocation(HWND hwnd, CXboxFolder * pff, CXboxPidlList * pflHfpl);
    HRESULT _InitSize(HWND hwnd, HWND hwndLabel, CXboxFolder * pff, CXboxPidlList * pflHfpl);
    HRESULT _InitTime(HWND hwnd, HWND hwndLabel, CXboxFolder * pff, CXboxPidlList * pflHfpl);
    HRESULT _InitCount(HWND hwnd, CXboxFolder * pff, CXboxPidlList * pflHfpl);

    BOOL m_fEditable;
};



#endif // _XBOXDIALOGTEMPLATE_H
