/*****************************************************************************\
     xboxdlg.cpp - Confirm Dialog box stuff
\*****************************************************************************/

#include "priv.h"
#include "xboxdhlp.h"

/*****************************************************************************\
      PFDI
  
      The fields in the fdi are as follows:
  
      pfdd -> dialog descriptor
      pszLocal -> ASCIIZ: Name of local file being replaced.
      pwfdRemote -> Description of remote file.
      cobj = number of objects affected
  
      The dialog template should have the following controls:
  
      IDC_REPLACE_YES        - The "Yes" button
      IDC_REPLACE_YESTOALL    - The "Yes to all" button
      IDC_REPLACE_NO        - The "No" button
      IDC_REPLACE_CANCEL    - The "Cancel" button
  
      Of these buttons, IDC_REPLACE_YES and IDC_REPLACE_NO are mandatory.
      If the "Yes to all" and "Cancel" buttons are available, the caller
      should set the fCanMulti flag in the fdd, in which case the extra
      buttons will be removed if cobj = 1.
  
      IDC_FILENAME        - A string with a '%hs' replacement field.
  
      The '%hs' will be replaced by the name passed in the pwfdRemote.
  
      IDC_REPLACE_OLDFILE    - A string which will be rewritten
      IDC_REPLACE_OLDICON    - An icon placeholder
  
      The string will be replaced by a description taken from pwfdRemote.
      The icon will be an icon for the file.
  
      IDC_REPLACE_NEWFILE    - A string which will be rewritten
      IDC_REPLACE_NEWICON    - An icon placeholder
  
      The string will be replaced by a description taken from pszLocal.
      The icon will be an icon for the file.
\*****************************************************************************/

class CXboxConfirmDialog
{
public:
    CXboxConfirmDialog(CXboxFolder * pff);
    ~CXboxConfirmDialog();

    InitDialog();
    UINT Display(HWND hwnd, LPCVOID pvLocal, LPCWIRESTR pwLocalWirePath, LPCWSTR pwzLocalDisplayPath, UINT fdiiLocal,
                LPCVOID pvRemote, LPCWIRESTR pwRemoteWirePath, LPCWSTR pwzRemoteDisplayPath, UINT fdiiRemote, int cobj, BOOL fAllowCancel, DWORD dwItem);
    static INT_PTR CALLBACK _XboxConfirmDialogProc(HWND hdlg, UINT wm, WPARAM wParam, LPARAM lParam);
    static BOOL _OnCommand(HWND hdlg, WPARAM wParam, LPARAM lParam);

private:
    LPCVOID             m_pvLocal;      // The local file in question
    UINT                m_fdiiLocal;
    LPCVOID             m_pvRemote;     // The remote file in question
    UINT                m_fdiiRemote;
    int                 m_cobj;         // Number of objects affected
    BOOL                m_fAllowCancel : 1;
    CXboxFolder *        m_pff;
    DWORD               m_dwItem;
    LPWIRESTR           m_pwLocalWirePath;
    LPWSTR              m_pwzLocalDisplayPath;
    LPWIRESTR           m_pwRemoteWirePath;
    LPWSTR              m_pwzRemoteDisplayPath;

    BOOL _OnInitDialog(HWND hDlg);
};


CXboxConfirmDialog::CXboxConfirmDialog(CXboxFolder * pff)
{
    m_pff = pff;
    m_pwLocalWirePath = NULL;
    m_pwzLocalDisplayPath = NULL;
    m_pwRemoteWirePath = NULL;
    m_pwzRemoteDisplayPath = NULL;
}


CXboxConfirmDialog::~CXboxConfirmDialog()
{
    Str_SetPtrA(&m_pwLocalWirePath, NULL);
    Str_SetPtrW(&m_pwzLocalDisplayPath, NULL);
    Str_SetPtrA(&m_pwRemoteWirePath, NULL);
    Str_SetPtrW(&m_pwzRemoteDisplayPath, NULL);
}


/*****************************************************************************\
    _XboxDlg_OnInitDialog
\*****************************************************************************/
BOOL CXboxConfirmDialog::_OnInitDialog(HWND hDlg)
{
    CXboxDialogTemplate xboxDialogTemplate;
    if (m_fdiiLocal == FDII_WFDA)
        EVAL(SUCCEEDED(xboxDialogTemplate.InitDialogWithFindData(hDlg, IDC_ITEM, m_pff, (const XBOX_FIND_DATA *) m_pvLocal, m_pwLocalWirePath, m_pwzLocalDisplayPath)));
    else
        EVAL(SUCCEEDED(xboxDialogTemplate.InitDialog(hDlg, FALSE, IDC_ITEM , m_pff, (CXboxPidlList *) m_pvLocal)));

    if (m_fdiiLocal == FDII_WFDA)
        EVAL(SUCCEEDED(xboxDialogTemplate.InitDialogWithFindData(hDlg, IDC_ITEM2, m_pff, (const XBOX_FIND_DATA *) m_pvRemote, m_pwRemoteWirePath, m_pwzRemoteDisplayPath)));
    else
        EVAL(SUCCEEDED(xboxDialogTemplate.InitDialog(hDlg, FALSE, IDC_ITEM2 , m_pff, (CXboxPidlList *) m_pvRemote)));

    return 1;
}

/*****************************************************************************\
    _XboxDlg_OnCommand
\*****************************************************************************/
BOOL CXboxConfirmDialog::_OnCommand(HWND hdlg, WPARAM wParam, LPARAM lParam)
{
    UINT idc = GET_WM_COMMAND_ID(wParam, lParam);

    switch (idc)
    {
    case IDC_REPLACE_YES:
    case IDC_REPLACE_YESTOALL:
        EndDialog(hdlg, idc);
        return 1;

    case IDC_REPLACE_NOTOALL:
        EndDialog(hdlg, IDC_REPLACE_NOTOALL);
        return 1;

    case IDC_REPLACE_CANCEL:
        EndDialog(hdlg, IDC_REPLACE_CANCEL);
        return 1;

        //    _UNOBVIOUS_:  Shift+No means "No to all", just like the shell.
    case IDC_REPLACE_NO:
        EndDialog(hdlg, GetKeyState(VK_SHIFT) < 0 ? IDC_REPLACE_NOTOALL : IDC_REPLACE_NO);
        return 1;
    }
    return 0;                // Not handled
}


/*****************************************************************************\
    _XboxDlg_DlgProc
\*****************************************************************************/
INT_PTR CXboxConfirmDialog::_XboxConfirmDialogProc(HWND hdlg, UINT wm, WPARAM wParam, LPARAM lParam)
{
    switch (wm)
    {
    case WM_INITDIALOG:
        return ((CXboxConfirmDialog *)lParam)->_OnInitDialog(hdlg);

    case WM_COMMAND:
        return CXboxConfirmDialog::_OnCommand(hdlg, wParam, lParam);
    }

    return 0;
}


UINT CXboxConfirmDialog::Display(HWND hwnd, LPCVOID pvLocal, LPCWIRESTR pwLocalWirePath, LPCWSTR pwzLocalDisplayPath, UINT fdiiLocal,
                LPCVOID pvRemote, LPCWIRESTR pwRemoteWirePath, LPCWSTR pwzRemoteDisplayPath, UINT fdiiRemote, int cobj, BOOL fAllowCancel, DWORD dwItem)
{
    m_pvLocal = pvLocal;
    m_fdiiLocal = fdiiLocal;
    m_pvRemote = pvRemote;
    m_fdiiRemote = fdiiRemote;
    m_cobj = cobj;
    m_fAllowCancel = fAllowCancel;
    m_dwItem = dwItem;

    Str_SetPtrA(&m_pwLocalWirePath, pwLocalWirePath);
    Str_SetPtrW(&m_pwzLocalDisplayPath, pwzLocalDisplayPath);
    Str_SetPtrA(&m_pwRemoteWirePath, pwRemoteWirePath);
    Str_SetPtrW(&m_pwzRemoteDisplayPath, pwzRemoteDisplayPath);

    return (UINT) DialogBoxParam(g_hinst, MAKEINTRESOURCE(dwItem), hwnd, CXboxConfirmDialog::_XboxConfirmDialogProc, (LPARAM)this);
}




/*****************************************************************************\
     XboxDlg_ConfirmReplace
\*****************************************************************************/
UINT XboxConfirmReplaceDialog(HWND hwnd, LPXBOX_FIND_DATA pwfdLocal, LPWIN32_FIND_DATA pwfdRemote,
                           int cobj, CXboxFolder * pff)
{
    CXboxConfirmDialog confirmDialog(pff);
    BOOL fAllowCancel = ((cobj > 1) ? 1 : 0);
    WCHAR wzLocalDisplayPath[MAX_PATH];
    WIRECHAR wRemoteWirePath[MAX_PATH];
    CWireEncoding * pWireEncoding = pff->GetCWireEncoding();

    EVAL(SUCCEEDED(pWireEncoding->WireBytesToUnicode(NULL, pwfdLocal->cFileName, (pff->IsUTF8Supported() ? WIREENC_USE_UTF8 : WIREENC_NONE), wzLocalDisplayPath, ARRAYSIZE(wzLocalDisplayPath))));
    EVAL(SUCCEEDED(pWireEncoding->UnicodeToWireBytes(NULL, pwfdRemote->cFileName, (pff->IsUTF8Supported() ? WIREENC_USE_UTF8 : WIREENC_NONE), wRemoteWirePath, ARRAYSIZE(wRemoteWirePath))));

    return confirmDialog.Display(hwnd, (LPCVOID) pwfdLocal, pwfdLocal->cFileName, wzLocalDisplayPath, FDII_WFDA,
            pwfdRemote, wRemoteWirePath, pwfdRemote->cFileName, FDII_WFDA, cobj, fAllowCancel, IDD_REPLACE);
}


/*****************************************************************************\
     XboxDlg_ConfirmReplace
\*****************************************************************************/
UINT XboxConfirmReplaceDialog(HWND hwnd, LPWIN32_FIND_DATA pwfdLocal, LPXBOX_FIND_DATA pwfdRemote,
                           int cobj, CXboxFolder * pff)
{
    CXboxConfirmDialog confirmDialog(pff);
    BOOL fAllowCancel = ((cobj > 1) ? 1 : 0);
    WIRECHAR wzLocalWirePath[MAX_PATH];
    WCHAR wRemoteDisplayPath[MAX_PATH];
    CWireEncoding * pWireEncoding = pff->GetCWireEncoding();

    EVAL(SUCCEEDED(pWireEncoding->UnicodeToWireBytes(NULL, pwfdLocal->cFileName, (pff->IsUTF8Supported() ? WIREENC_USE_UTF8 : WIREENC_NONE), wzLocalWirePath, ARRAYSIZE(wzLocalWirePath))));
    EVAL(SUCCEEDED(pWireEncoding->WireBytesToUnicode(NULL, pwfdRemote->cFileName, (pff->IsUTF8Supported() ? WIREENC_USE_UTF8 : WIREENC_NONE), wRemoteDisplayPath, ARRAYSIZE(wRemoteDisplayPath))));

    return confirmDialog.Display(hwnd, (LPCVOID) pwfdLocal, wzLocalWirePath, pwfdLocal->cFileName, FDII_WFDA,
            pwfdRemote, pwfdRemote->cFileName, wRemoteDisplayPath, FDII_WFDA, cobj, fAllowCancel, IDD_REPLACE);
}


/*****************************************************************************\
      XboxDlg_ConfirmDelete
\*****************************************************************************/
UINT XboxConfirmDeleteDialog(HWND hwnd, CXboxPidlList * pflHfpl, CXboxFolder * pff)
{
    CXboxConfirmDialog confirmDialog(pff);
    UINT id;

    if (pflHfpl->GetCount() > 1)
        id = IDD_DELETEMULTI;
    else
    {
        if (XboxPidl_IsDirectory(pflHfpl->GetPidl(0), TRUE))
            id = IDD_DELETEFOLDER;
        else
            id = IDD_DELETEFILE;
    }

    return confirmDialog.Display(hwnd, (LPCVOID) pflHfpl, NULL, NULL, FDII_HFPL, 0, 0, NULL, NULL, NULL, FALSE, id);
}

