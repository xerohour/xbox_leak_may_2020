/*++

Copyright (c) Microsoft Corporation

Module Name:

    dialogs.cpp

Abstract:

    Implementation of the various dialog boxes that are required.
    These are no more than souped up message boxes for the most
    part, but unfortunately no one 

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    03-13-2001 : created (mitchd)

--*/

#include <stdafx.h>
#include <windowsx.h>

/*
**  CXboxDialog - a basic dialog that is completely static.  If all of the
**                text and icons can be specified in the template, use this
**                one.
**
**  This is also the base class for more sophisticated dialog prompts.
**
*/
class CXboxDialog
{
  public:
    CXboxDialog():m_hWndDlg(NULL){}
    INT_PTR Execute(LPCSTR lpTemplateName, HWND hWndParent);

    virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
    virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
    virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

  protected:

    virtual BOOL End(INT_PTR nResult);
    HWND m_hWndDlg;

  private:
    static INT_PTR CALLBACK DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

INT_PTR CXboxDialog::Execute(LPCSTR lpTemplateName, HWND hWndParent)
{
    return DialogBoxParamA(_Module.GetModuleInstance(), lpTemplateName, hWndParent, DialogProc, (WPARAM)this);
}

INT_PTR CALLBACK CXboxDialog::DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CXboxDialog *pThis;

    //
    //  WM_INITDIALOG is special.
    //
    if(WM_INITDIALOG==uMsg)
    {
        pThis = (CXboxDialog *)lParam;
        SetWindowLong(hWndDlg,DWL_USER,(LONG)pThis);
        pThis->m_hWndDlg = hWndDlg;
        return pThis->OnInitDialog((HWND)wParam);
    }

    pThis = (CXboxDialog *)GetWindowLong(hWndDlg, DWL_USER);
    if(pThis)
    {
        if(WM_COMMAND == uMsg)
            return pThis->OnCommand(wParam, lParam);
        
        return pThis->OnMessage(uMsg, wParam, lParam);
    }
    return 0;
}


INT_PTR CXboxDialog::OnInitDialog(HWND)
{
    return 1;
}

INT_PTR CXboxDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT idc = GET_WM_COMMAND_ID(wParam, lParam);
    //
    //  One special case that applies to all the dialogs:
    //  VK_SHIFT+IDC_XB_NO=>IDC_XB_NOTOALL
    //
    if( (IDC_XB_NO == idc) && (GetKeyState(VK_SHIFT) < 0))
    {
        idc = IDC_XB_NOTOALL;
    }
    return End(idc);
}

INT_PTR CXboxDialog::OnMessage(UINT, WPARAM, LPARAM)
{
    return 0;
}

BOOL CXboxDialog::End(INT_PTR nResult)
{
    return EndDialog(m_hWndDlg, nResult);
}

/*
**  Implement CConfirmDialog  - this is the simplest
**  useful class.  
**
**  It is more flexible than a message box in that
**  you can provide a template rather than the MB_
**  flags.
**
**  This is just what we need for many of the shell
**  extension prompts.
*/  

class CConfirmDialog : public CXboxDialog
{
  public:
    CConfirmDialog(){}
    
    INT_PTR Confirm(HWND hWndParent, LPCSTR pszText, LPCSTR pszCaption, LPCSTR lpTemplateName)
    {
        m_pszText = pszText;
        m_pszCaption = pszCaption;
        return Execute(lpTemplateName, hWndParent);       
    }

  protected:
    virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
    LPCSTR  m_pszText;
    LPCSTR  m_pszCaption;
};

INT_PTR
CConfirmDialog::OnInitDialog(
    HWND
    )
/*++
  Routine Description:
    We need to update the only modifiable filed IDX_XB_TEXT
--*/
{
    //  Fill in the caption.
    if(m_pszCaption)
    {
        SetWindowTextA(m_hWndDlg, m_pszCaption);
    }

    //  Fill in the text
    if(m_pszText)
    {
        HWND hWndCtrl = GetDlgItem(m_hWndDlg, IDC_XB_TEXT);
        if(hWndCtrl)
        {
            SetWindowTextA(hWndCtrl, m_pszText);
        }
    }
    return TRUE;
}

/*
**  CConfirmReplaceDialog - 
**      a more specialized dialog specifically for confirming
**      file replace.  It works with specific template that
**      cannot be specified.
*/

class CConfirmReplaceDialog : public CXboxDialog
{
  public:
    CConfirmReplaceDialog() {}
    
    UINT 
    Confirm(
        HWND hWndParent,
        LPCSTR pszFileName,
        WIN32_FILE_ATTRIBUTE_DATA *pFileAttributes,
        WIN32_FILE_ATTRIBUTE_DATA *pFileAttributes2
        );
    
  protected:
    
    virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);

  private:
    char m_szFileName[MAX_PATH];
    WIN32_FILE_ATTRIBUTE_DATA m_FileAttributes;
    WIN32_FILE_ATTRIBUTE_DATA m_FileAttributes2;
};

UINT 
CConfirmReplaceDialog::Confirm(
    HWND hWndParent,
    LPCSTR pszFileName,
    WIN32_FILE_ATTRIBUTE_DATA *pFileAttributes,
    WIN32_FILE_ATTRIBUTE_DATA *pFileAttributes2
    )
{
    //
    //  Copy the filename
    //
    strcpy(m_szFileName, pszFileName);

    //
    //  Copy the first file attributes
    //
    memcpy(&m_FileAttributes, pFileAttributes, sizeof(WIN32_FILE_ATTRIBUTE_DATA));

    //
    //  Copy the second file attributes, if present
    //
    memcpy(&m_FileAttributes2, pFileAttributes2, sizeof(WIN32_FILE_ATTRIBUTE_DATA));

    //
    //  Execute the dialog
    //
    return Execute(MAKEINTRESOURCEA(IDD_CONFIRM_REPLACE), hWndParent);
}


 
INT_PTR
CConfirmReplaceDialog ::OnInitDialog(
    HWND
    )
/*++
  Routine Description:
    This routine is called when WM_INITDIALOG is sent to the dialog.
    At this time we need to do any required manipulation of the dialog template.
    Basically, we fill in the file name(s) and details.  Remove the "Yes To All" and
    "No To All" buttons if m_fMultiFile is not set.

  Parameters:
    The HWND passed is for the default control.  We don't care about this.
    The HWMD we care about is our window, the m_hWndDlg member.

--*/
{
    HWND hWndCtrl;
    char szFormatBuffer[1024];
    
    //
    //  Update the file name
    //
    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_XB_FILENAME);
    if(hWndCtrl) WindowUtils::SubstituteWindowText(hWndCtrl, m_szFileName);

    //
    //  Update the first file size
    //
    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_XB_FILESIZE);
    if(hWndCtrl)
    {
        ULARGE_INTEGER uliFileSize;
        uliFileSize.HighPart = m_FileAttributes.nFileSizeHigh;
        uliFileSize.LowPart = m_FileAttributes.nFileSizeLow;
        FormatUtils::FileSize(uliFileSize.QuadPart, szFormatBuffer);
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    //
    //  Update the first file time
    //
    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_XB_FILETIME);
    if(hWndCtrl)
    {
        FormatUtils::FileTime(&m_FileAttributes.ftLastWriteTime, szFormatBuffer, DATE_LONGDATE);
        WindowUtils::SubstituteWindowText(hWndCtrl, szFormatBuffer);
    }

    //
    //  Update the Icon of the first file
    //
    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_XB_FILEICON);
    if(hWndCtrl)
    {
        SHFILEINFOA ShellFileInfo;
        if(
          SHGetFileInfoA(
            m_szFileName,
            m_FileAttributes.dwFileAttributes,
            &ShellFileInfo,
            sizeof(ShellFileInfo),
            SHGFI_ICON | 
            SHGFI_SHELLICONSIZE | 
            SHGFI_USEFILEATTRIBUTES
            )
        )
        {
            WindowUtils::ReplaceWindowIcon(hWndCtrl, ShellFileInfo.hIcon);
        }
    }

    
    //
    //  Update the second file size
    //
    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_XB_FILESIZE2);
    if(hWndCtrl)
    {
        ULARGE_INTEGER uliFileSize;
        uliFileSize.HighPart = m_FileAttributes2.nFileSizeHigh;
        uliFileSize.LowPart = m_FileAttributes2.nFileSizeLow;
        FormatUtils::FileSize(uliFileSize.QuadPart,szFormatBuffer );
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    //
    //  Update the second file time
    //
    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_XB_FILETIME2);
    if(hWndCtrl)
    {
        FormatUtils::FileTime(&m_FileAttributes2.ftLastWriteTime, szFormatBuffer, DATE_LONGDATE);
        WindowUtils::SubstituteWindowText(hWndCtrl, szFormatBuffer);
    }

    //
    //  Update the Icon of the second file
    //
    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_XB_FILEICON2);
    if(hWndCtrl)
    {
        SHFILEINFOA ShellFileInfo;
        if(
          SHGetFileInfoA(
            m_szFileName,
            m_FileAttributes2.dwFileAttributes,
            &ShellFileInfo,
            sizeof(ShellFileInfo),
            SHGFI_ICON | 
            SHGFI_SHELLICONSIZE | 
            SHGFI_USEFILEATTRIBUTES
            )
        )
        {
            WindowUtils::ReplaceWindowIcon(hWndCtrl, ShellFileInfo.hIcon);
        }
    }

    return TRUE;
}

/*
**  CConfirmAttributesDialog - 
**      a more specialized dialog specifically for confirming
**      changing of attributes.  It for folders it queries
**      whether or not attributes should be applied recursively.
*/

class CConfirmAttributesDialog : public CXboxDialog
{
  public:
  
    CConfirmAttributesDialog(){}
    UINT Confirm(HWND hWndParent, DWORD dwSetAttributes, DWORD dwClearAttributes, BOOL fMultiItem);

  protected:
  
    virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
    virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);

  private:
    DWORD m_dwSetAttributes;
    DWORD m_dwClearAttributes;
    BOOL  m_fMultiItem;
    BOOL  m_fRecursive;

};

INT_PTR CConfirmAttributesDialog::OnInitDialog(HWND hwndDefaultControl)
{
    HWND hWndCtrl;
    char  szFormatBuffer[MAX_PATH] = {0};
    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_ATTRIBSTOAPPLY);
    if(hWndCtrl)
    {
        LPSTR pszParse = szFormatBuffer;
        
        if(m_dwSetAttributes&FILE_ATTRIBUTE_READONLY)
        {
            pszParse += LoadStringA(_Module.GetModuleInstance(), IDS_READONLY, pszParse, MAX_PATH);
        } else if(m_dwClearAttributes&FILE_ATTRIBUTE_READONLY)
        {
            pszParse += LoadStringA(_Module.GetModuleInstance(), IDS_NOTREADONLY, pszParse, MAX_PATH);
        }

        if(m_dwSetAttributes&FILE_ATTRIBUTE_HIDDEN)
        {
            pszParse += LoadStringA(_Module.GetModuleInstance(), IDS_HIDE, pszParse, MAX_PATH);
        } else if(m_dwClearAttributes&FILE_ATTRIBUTE_HIDDEN)
        {
            pszParse += LoadStringA(_Module.GetModuleInstance(), IDS_UNHIDE, pszParse, MAX_PATH);
        }
        
        //
        //  Remove the file ",".
        //
        pszParse -= 2;
        *pszParse = '\0';

        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }    

    if(m_fMultiItem)
    {
      LoadStringA(_Module.GetModuleInstance(), IDS_THESELECTEDITEMS, szFormatBuffer, MAX_PATH);
    } else
    {
      LoadStringA(_Module.GetModuleInstance(), IDS_THISFOLDER, szFormatBuffer, MAX_PATH);
    }
    
    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_RECURSIVE_TXT);
    if(hWndCtrl)
    {
        WindowUtils::SubstituteWindowText(hWndCtrl, szFormatBuffer);
    }

    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_NOTRECURSIVE);
    if(hWndCtrl)
    {
        WindowUtils::SubstituteWindowText(hWndCtrl, szFormatBuffer);
        SendMessage(hWndCtrl, BM_SETCHECK, BST_CHECKED, 0);
    }

    hWndCtrl = GetDlgItem(m_hWndDlg, IDC_RECURSIVE);
    if(hWndCtrl)
    {
        WindowUtils::SubstituteWindowText(hWndCtrl, szFormatBuffer);
    }
    return TRUE;
}

UINT CConfirmAttributesDialog::Confirm(HWND hWndParent, DWORD dwSetAttributes, DWORD dwClearAttributes, BOOL fMultiItem)
{
    UINT uResult;
    m_dwSetAttributes = dwSetAttributes;
    m_dwClearAttributes = dwClearAttributes;
    m_fMultiItem = fMultiItem;
    m_fRecursive = FALSE;
    return Execute(MAKEINTRESOURCEA(IDD_ATTRIBS_RECURSIVE), hWndParent);
}

INT_PTR CConfirmAttributesDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);
    
    //
    //  If Flip the m_fRecursive as buttons are pressed.
    //  
    //
    if( uCommand == BN_CLICKED) 
    { 
        if(uControlId==IDC_RECURSIVE)
        {
            m_fRecursive = TRUE;
            return TRUE;
        }
        if(uControlId==IDC_NOTRECURSIVE)
        {
           m_fRecursive = FALSE;
           return TRUE;
        }
        if((IDOK==uControlId)&&m_fRecursive)
        {
            return End(IDC_XB_YESTOALL);
        }
        return End(uCommand);
    }
    return FALSE;
}


/*
**  CPromptUserNameDialog
*/
class CPromptUserNameDialog : public CXboxDialog
{
  public:
  
    CPromptUserNameDialog(){}
    UINT Prompt(HWND hWndParent, LPSTR pszUserName, int iMaxCount);

  protected:
    virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
    virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
    
  private:
    LPSTR m_pszUserName;
    int   m_iMaxCount;
};


UINT CPromptUserNameDialog::Prompt(HWND hWndParent, LPSTR pszUserName, int iMaxCount)
{
    m_pszUserName = pszUserName;
    m_iMaxCount = iMaxCount;
    return Execute(MAKEINTRESOURCEA(IDD_USERNAME_PROMPT), hWndParent);
};

INT_PTR CPromptUserNameDialog::OnInitDialog(HWND hwndDefaultControl)
{
    SendMessage(GetDlgItem(m_hWndDlg, IDC_XB_TEXT), EM_SETLIMITTEXT, (WPARAM)m_iMaxCount, (LPARAM)0);
    EnableWindow(GetDlgItem(m_hWndDlg, IDOK), FALSE);
    SendMessage(m_hWndDlg, DM_SETDEFID, IDCANCEL, 0);
    SetFocus(GetDlgItem(m_hWndDlg, IDC_XB_TEXT));
    return FALSE;
}

INT_PTR CPromptUserNameDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);

    if( uCommand == BN_CLICKED) 
    { 
        if((uControlId==IDOK) || (uControlId==IDCANCEL))
        {
            End(uControlId);
            return TRUE;
        }
    } else if(uCommand == EN_CHANGE)
    {
        // Keep m_pszUserName in sync with the control.
        // Also, enable\disable OK, based on whether
        // the user name is blank.
        if(uControlId==IDC_XB_TEXT)
        {
            if(GetDlgItemTextA(m_hWndDlg, IDC_XB_TEXT, m_pszUserName, m_iMaxCount))
            {
                EnableWindow(GetDlgItem(m_hWndDlg, IDOK), TRUE);
                SendMessage(m_hWndDlg, DM_SETDEFID, IDOK, 0);
            } else
            {
                EnableWindow(GetDlgItem(m_hWndDlg, IDOK), FALSE);
                SendMessage(m_hWndDlg, DM_SETDEFID, IDCANCEL, 0);
            }
            return TRUE;
        }
    }
    return FALSE;
}

/*
**  CPromptNewPasswordDialog
*/
class CPromptNewPasswordDialog : public CXboxDialog
{
  public:
  
    CPromptNewPasswordDialog(){m_szConfirmPassword[0]='\0';}
    UINT Prompt(HWND hWndParent, LPSTR pszPassword, int iMaxCount);

  protected:
    virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
    virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
    
  private:
    LPSTR m_pszPassword;
    char  m_szConfirmPassword[MAX_PATH];
    int   m_iMaxCount;
};


UINT CPromptNewPasswordDialog::Prompt(HWND hWndParent, LPSTR pszPassword, int iMaxCount)
{
    m_pszPassword = pszPassword;
    m_iMaxCount = iMaxCount;
    _ASSERT(iMaxCount <= MAX_PATH);
    return Execute(MAKEINTRESOURCEA(IDD_PASSWORD_PROMPT), hWndParent);
};

INT_PTR CPromptNewPasswordDialog::OnInitDialog(HWND hwndDefaultControl)
{
    SendMessage(GetDlgItem(m_hWndDlg, IDC_SECURITY_PASSWORD_EDIT), EM_SETLIMITTEXT, (WPARAM)m_iMaxCount, (LPARAM)0);
    SendMessage(GetDlgItem(m_hWndDlg, IDC_SECURITY_CONFIRM_PASSWORD), EM_SETLIMITTEXT, (WPARAM)m_iMaxCount, (LPARAM)0);
    EnableWindow(GetDlgItem(m_hWndDlg, IDOK), FALSE);
    SendMessage(m_hWndDlg, DM_SETDEFID, IDCANCEL, 0);
    SetFocus(GetDlgItem(m_hWndDlg, IDC_SECURITY_PASSWORD_EDIT));
    return FALSE;
}

INT_PTR CPromptNewPasswordDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);
    BOOL fRet = FALSE;
    if( uCommand == BN_CLICKED) 
    { 
        if((uControlId==IDOK))
        {
            //Make sure that the password and confirm password fields are identical.
            if(strcmp(m_pszPassword, m_szConfirmPassword))
            {
                ShowWindow(GetDlgItem(m_hWndDlg, IDC_SECURITY_PASSWORD_MISMATCH), SW_SHOW);
                MessageBeep(MB_ICONASTERISK);
            } else
            {
                End(uControlId);
            }
            fRet = TRUE;
        } else if(uControlId==IDCANCEL)
        {
            End(uControlId);
            fRet = TRUE;
        }
    } else if(uCommand == EN_CHANGE)
    {
        if(uControlId==IDC_SECURITY_PASSWORD_EDIT)
        {
            GetDlgItemTextA(m_hWndDlg, IDC_SECURITY_PASSWORD_EDIT, m_pszPassword, m_iMaxCount);
            fRet = TRUE;
        }
        else if(uControlId==IDC_SECURITY_CONFIRM_PASSWORD)
        {
            GetDlgItemTextA(m_hWndDlg, IDC_SECURITY_CONFIRM_PASSWORD, m_szConfirmPassword, m_iMaxCount);
            fRet = TRUE;
        }
        if(fRet)
        {
            if(*m_pszPassword && *m_szConfirmPassword)
            {
                EnableWindow(GetDlgItem(m_hWndDlg, IDOK), TRUE);
                SendMessage(m_hWndDlg, DM_SETDEFID, IDOK, 0);
            }else
            {
                EnableWindow(GetDlgItem(m_hWndDlg, IDOK), FALSE);
                SendMessage(m_hWndDlg, DM_SETDEFID, IDCANCEL, 0);
            }
        }
    }
    return fRet;
}

/*
**  Implementation of Wrapper Utility Functions
**  to put up the various dialogs used by 
**  the shell extension.
*/

UINT
Dialog::ConfirmReadOnlyMove(HWND hWndParent, LPCSTR pszFileName, bool fFolder)
{
    UINT uRet = IDC_XB_CANCEL;
    UINT uCaptionResource;
    UINT uTextResource;
    char szCaption[80];
    char szText[128];

    //
    //  Choose the text and caption resources based on file or folder.
    //

    if(fFolder)
    {
        uCaptionResource = IDS_CONFIRM_RO_FOLDER_MOVE_CAPTION;
        uTextResource = IDS_CONFIRM_RO_FOLDER_MOVE;
    } else
    {
        uCaptionResource = IDS_CONFIRM_RO_FILE_MOVE_CAPTION;
        uTextResource = IDS_CONFIRM_RO_FILE_MOVE;
    }

    //
    //  Load and format the strings
    //

    LoadStringA(_Module.GetModuleInstance(), uCaptionResource, szCaption, sizeof(szCaption));
    WindowUtils::rsprintf(szText, uTextResource, pszFileName);

    CConfirmDialog *pConfirm = new CConfirmDialog;
    if(pConfirm)
    {
        uRet = pConfirm->Confirm(hWndParent, szText, szCaption, MAKEINTRESOURCEA(IDD_CONFIRM_MOVE));
        delete pConfirm;
    }
    return uRet;
}

UINT
Dialog::ConfirmFolderReplace(
    HWND hWndParent,
    LPCSTR pszFileName
    )
{
    UINT uRet = IDC_XB_CANCEL;
    char szCaption[80];
    char szText[128];

    //
    //  Load and format the strings
    //

    LoadStringA(_Module.GetModuleInstance(), IDS_CONFIRM_FOLDER_REPLACE_CAPTION, szCaption, sizeof(szCaption));
    WindowUtils::rsprintf(szText, IDS_CONFIRM_FOLDER_REPLACE, pszFileName);

    CConfirmDialog *pConfirm = new CConfirmDialog;
    if(pConfirm)
    {
        uRet = pConfirm->Confirm(hWndParent, szText, szCaption, MAKEINTRESOURCEA(IDD_CONFIRM_FOLDER_REPLACE));
        delete pConfirm;
    }
    return uRet;
}

UINT
Dialog::ConfirmFileReplace(
    HWND hWndParent,
    LPCSTR pszFileName,
    WIN32_FILE_ATTRIBUTE_DATA *pTargetFileAttributes,
    WIN32_FILE_ATTRIBUTE_DATA *pSourceFileAttributes
    )
{
    UINT uRet = IDC_XB_CANCEL;
    CConfirmReplaceDialog *pConfirm = new CConfirmReplaceDialog;
    if(pConfirm)
    {
        uRet = pConfirm->Confirm(hWndParent, pszFileName, pTargetFileAttributes, pSourceFileAttributes);
        delete pConfirm;
    }
    return uRet;
}

UINT
Dialog::ConfirmDelete(
    HWND hWndParent,
    LPCSTR pszFileName,
    bool fFolder,
    bool fReadOnly
    )
{
    UINT uRet = IDC_XB_CANCEL;
    UINT uCaptionResource;
    UINT uTextResource;
    char szCaption[80];
    char szText[128];
    
    if(fFolder)
    {
       uCaptionResource = IDS_CONFIRM_DELETE_FOLDER_CAPTION;
       if(fReadOnly) uTextResource = IDS_CONFIRM_DELETE_RO_FOLDER;
       else          uTextResource = IDS_CONFIRM_DELETE_FOLDER;
    }else
    {
       uCaptionResource = IDS_CONFIRM_DELETE_CAPTION;
       if(fReadOnly) uTextResource = IDS_CONFIRM_DELETE_RO;
       else          uTextResource = IDS_CONFIRM_DELETE;
    }

    //
    //  Load and format the text and caption.
    //
    WindowUtils::rsprintf(szText, uTextResource, pszFileName);
    LoadStringA(_Module.GetModuleInstance(), uCaptionResource, szCaption, sizeof(szCaption));
    
    //
    //  Display the dialog
    //
    CConfirmDialog *pConfirm = new CConfirmDialog;
    if(pConfirm)
    {
        uRet = pConfirm->Confirm(hWndParent, szText, szCaption, MAKEINTRESOURCEA(IDD_CONFIRM_DELETE));
        delete pConfirm;
    }
    return uRet;
}

UINT
Dialog::ConfirmDeleteMultiple(
    HWND hWndParent,
    UINT uCount
    )
{
    UINT uRet = IDC_XB_CANCEL;
    char szCaption[80];
    char szText[128];
    
    //
    //  Load and format the text and caption.
    //
    WindowUtils::rsprintf(szText, IDS_CONFIRM_DELETE_MULTIPLE, uCount);
    LoadStringA(_Module.GetModuleInstance(), IDS_CONFIRM_DELETE_MULTIPLE_CAPTION, szCaption, sizeof(szCaption));
    
    CConfirmDialog *pConfirm = new CConfirmDialog;
    if(pConfirm)
    {
        uRet = pConfirm->Confirm(hWndParent, szText, szCaption, MAKEINTRESOURCEA(IDD_CONFIRM_DELETE));
        delete pConfirm;
    }
    return uRet;
}

UINT Dialog::ConfirmSetAttributes(HWND hWndParent, DWORD dwSetAttributes, DWORD dwClearAttributes, BOOL fMultiItem)
{
    CConfirmAttributesDialog confirmDialog;
    return confirmDialog.Confirm(hWndParent, dwSetAttributes, dwClearAttributes, fMultiItem);
}

UINT
Dialog::PromptUserName(HWND hWndParent, LPSTR pszUserName, int iMaxCount)
{
    UINT uRet = IDCANCEL;
    CPromptUserNameDialog *pPromptUserName;
    pPromptUserName = new CPromptUserNameDialog;
    if(pPromptUserName)
    {
        uRet = pPromptUserName->Prompt(hWndParent, pszUserName, iMaxCount);
        delete pPromptUserName;
    }
    return uRet;
}

UINT
Dialog::PromptNewPassword(HWND hWndParent, LPSTR pszPassword, int iMaxCount)
{
    UINT uRet = IDCANCEL;
    CPromptNewPasswordDialog *pPromptNewPassword;
    pPromptNewPassword = new CPromptNewPasswordDialog;
    if(pPromptNewPassword)
    {
        uRet = pPromptNewPassword->Prompt(hWndParent, pszPassword, iMaxCount);
        delete pPromptNewPassword;
    }
    return uRet;
}
