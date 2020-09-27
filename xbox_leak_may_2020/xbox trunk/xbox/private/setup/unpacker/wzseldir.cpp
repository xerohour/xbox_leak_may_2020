//  WZSELDIR.CPP
//
//  Created 28-Mar-2001 [JonT]

#include "unpacker.h"

CWizardSelectDir::CWizardSelectDir(
    CWizard* pWizard,
    PROPSHEETPAGE* pPSP
    ) : CWizardPage(pWizard, pPSP, IDD_SELECTDIR)
{
    // Override any propsheet settings made by the parent for this specific page
    pPSP->dwFlags = PSP_DEFAULT | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE | PSP_USETITLE;
    pPSP->pszHeaderTitle = MAKEINTRESOURCE(IDS_SELDIRTITLE);
    pPSP->pszHeaderSubTitle = MAKEINTRESOURCE(IDS_SELDIRSUBTITLE);
    pPSP->pszTitle = CUnpacker::GetProductName();
}


//  BrowseCallbackProc
//      Called from SHBrowseForFolder to validate the paths clicked on there

int CALLBACK
BrowseCallbackProc(
    HWND hwnd, 
    UINT uMsg, 
    LPARAM lParam, 
    LPARAM lpData
    )
{
    // If the selection changes, we have a new item to validate
    if (uMsg == BFFM_SELCHANGED)
    {
        TCHAR szPath[MAX_PATH];

        // Get the new item
        SHGetPathFromIDList((LPITEMIDLIST)lParam, szPath);

        // If it's too long, disable the OK button. If it's not, enable it.
        if (strlen(szPath) >= MAX_SETUP_PATH_LENGTH)
            SendMessage(hwnd, BFFM_ENABLEOK, 0, FALSE); // Disable OK button
        else
            SendMessage(hwnd, BFFM_ENABLEOK, 0, TRUE); // Enable OK button
    }
    return 0;
}


BOOL
CWizardSelectDir::OnMessage(
    UINT dwMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    if (dwMsg == WM_COMMAND && LOWORD(wParam) == IDC_DIRBROWSE)
    {
        BROWSEINFO bi = { 0 };
        LPITEMIDLIST pidl;
        TCHAR szText[MAX_PATH];
        LPMALLOC lpmem;

        LoadString(::GetInstanceHandle(), IDS_FOLDERTEXT, szText, sizeof (szText));

        // Bring up the shell browse for folder dialog
        bi.hwndOwner = m_hwnd;
        bi.lpszTitle = szText;
        bi.ulFlags = BIF_RETURNONLYFSDIRS;
        bi.lpfn = BrowseCallbackProc;
        pidl = SHBrowseForFolder(&bi);
        if (pidl)
        {
            SHGetPathFromIDList(pidl, szText);
            SetWindowText(GetDlgItem(m_hwnd, IDC_DIREDIT), szText);
            SHGetMalloc(&lpmem);
            lpmem->Free(pidl);
            lpmem->Release();
        }
    }

    return CWizardPage::OnMessage(dwMsg, wParam, lParam);
}


VOID
CWizardSelectDir::OnSetActive()
{
    TCHAR szPath[MAX_PATH];
    HWND hwndEdit = GetDlgItem(m_hwnd, IDC_DIREDIT);
    CUnpacker* punpacker = GetUnpacker();

    FormatWindowText(GetDlgItem(m_hwnd, IDC_INSTALL_TEXT), CUnpacker::GetProductName());

    if (*punpacker->GetDefaultDir() == 0)
    {
        SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, szPath);
        ::AppendSlash(szPath);
        LPSTR lpDefaultDir = GetSettings()->GetSetting(CSTR_DEFAULTDIRTEXT);
        strcat(szPath, lpDefaultDir);
        free(lpDefaultDir);
    }
    else
        strcpy(szPath, punpacker->GetDefaultDir());

    SendMessage(hwndEdit, EM_LIMITTEXT, (WPARAM)MAX_SETUP_PATH_LENGTH, 0);
    SetWindowText(hwndEdit, szPath);
    PropSheet_SetWizButtons(::GetParent(m_hwnd), PSWIZB_BACK | PSWIZB_NEXT);
}


BOOL 
CWizardSelectDir::OnNext()
{
    CUnpacker* punpacker = ::GetUnpacker();
    int nRet;
    TCHAR szPath[MAX_PATH];
    HANDLE hfile;


    // Get the text from the edit control and make it our default directory
    GetWindowText(GetDlgItem(m_hwnd, IDC_DIREDIT), punpacker->GetDefaultDir(), MAX_PATH);

    // Make sure it's not the c: root
    strcpy(szPath, punpacker->GetDefaultDir());
    ::AppendSlash(szPath);
    if (_stricmp(szPath, _TEXT("c:\\")) == 0)
    {
        LoadString(::GetInstanceHandle(), IDS_MB_INVALIDPATH, szPath, sizeof (szPath));
        MessageBox(GetParent(m_hwnd), szPath, CUnpacker::GetProductName(), MB_OK);
        SetNextPage(IDD_SELECTDIR);
        return -1;
    }

    // Make sure that there is not a different product installed to that directory
    // that uses the Xbox Install Technology.
    TCHAR szOtherProductName[MAX_PATH];
    if(!punpacker->CheckForOtherProduct(szPath, szOtherProductName, sizeof(szOtherProductName)))
    {
        TCHAR szMessage[MAX_PATH];
        rsprintf(::GetInstanceHandle(), szMessage, IDS_CANT_SHARE_DIRECTORY, CUnpacker::GetProductName(), szPath, szOtherProductName);
        MessageBox(GetParent(m_hwnd), szMessage, CUnpacker::GetProductName(), MB_OK);
        SetNextPage(IDD_SELECTDIR);
        return -1;
    }

    // See if we can create the directory
    strcat(szPath, "test.tst"); // Depends on szPath from above
    CCab::MakeDirectory(szPath);
    hfile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, 0, NULL);
    if (hfile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hfile);
        DeleteFile(szPath);
    }
    else
    {
        LoadString(::GetInstanceHandle(), IDS_CANT_CREATE, szPath, MAX_PATH);
        MessageBox(GetParent(m_hwnd), szPath, CUnpacker::GetProductName(), MB_OK);
        SetNextPage(IDD_SELECTDIR);
        return -1;
    }

    

    // Now that we have a good directory, do a sanity check of directories
    // If no good, skip to the error page.
    if (!punpacker->ValidateDirectories())
    {
        SetNextPage(IDD_WIZERROR);
        return TRUE;
    }

    // Check disk space. This is complicated by the fact that if there are two
    // types of out of disk space: a return of 1 means that the default directory
    // (that they just chose) is the one that is out of space. If it's this, we
    // just show a MessageBox and let them choose a new directory. A return of -1
    // means that it wasn't the default directory that caused it. If not, they can't
    // do anything about it, so we go to the error page.
    // BUGBUG: set hourglass here
    nRet = punpacker->CheckDiskSpace();
    if (nRet == 1)
    {
        MessageBox(GetParent(m_hwnd), WizGetLastErrorString(), CUnpacker::GetProductName(), MB_OK);
        SetNextPage(IDD_SELECTDIR);
        return -1;
    }
    else if (nRet == -1)
    {
        SetNextPage(IDD_WIZERROR);
        return TRUE;
    }

    // Move the tempdir to the uninstall dir. This also sets the
    // default directory for anyone else that needs it
    if (!punpacker->MoveSettingsCab())
        SetNextPage(IDD_WIZERROR);
    else
        SetNextPage(IDD_COPYING);
    return TRUE;
}

VOID
CWizardSelectDir::OnBack()
{
    if(UNPACKER_FLAGS_SKIPLICENSEPAGE&GetUnpacker()->GetFlags())
    {
        SetNextPage(IDD_WELCOME);
    } else
    {
        SetNextPage(IDD_LICENSE);
    }
    return;
}