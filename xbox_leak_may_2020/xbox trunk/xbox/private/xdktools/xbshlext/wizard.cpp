/*++

Copyright (c) Microsoft Corporation

Module Name:

    wizard.cpp

Abstract:

    Implementation of the add new console
    wizard

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    08-07-2001 : created (mitchd)

--*/

#include <stdafx.h>
#define DMPL_PRIV_ALL (DMPL_PRIV_READ|DMPL_PRIV_WRITE|DMPL_PRIV_MANAGE|DMPL_PRIV_CONFIGURE|DMPL_PRIV_CONTROL)

HRESULT CWizard::Initialize(UINT uMaxPages)
{
    m_hrInitialize = E_OUTOFMEMORY;
    m_uMaxPages = uMaxPages;
    m_uPageCount = 0;
    m_pPages = new WizardPageInfo[m_uMaxPages];
    if(m_pPages)
    {
        m_pPropSheetPages = new PROPSHEETPAGEA[m_uMaxPages];
        if(m_pPropSheetPages)
        {
            m_hrInitialize = S_OK;
        
            //  Create the title font
            NONCLIENTMETRICS ncm = {0};
	        ncm.cbSize = sizeof(ncm);
	        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
	        LOGFONT TitleLogFont = ncm.lfMessageFont;
	        TitleLogFont.lfWeight = FW_BOLD;
	        lstrcpy(TitleLogFont.lfFaceName, TEXT("Verdana Bold"));
	        HDC hdc = GetDC(NULL);
	        INT FontSize = 12;
	        TitleLogFont.lfHeight = 0 - GetDeviceCaps(hdc, LOGPIXELSY) * FontSize / 72;
	        m_hTitleFont = CreateFontIndirect(&TitleLogFont);
	        ReleaseDC(NULL, hdc);
        } else
        {
            delete [] m_pPages;
            m_pPages = NULL;
        }
    }
    return m_hrInitialize;
}

HRESULT CWizard::DoWizard(HWND hwndParent)
{
    PROPSHEETHEADERA propSheetHeader;
    propSheetHeader.dwSize = sizeof(PROPSHEETHEADERA);
    propSheetHeader.dwFlags = PSH_WIZARD97 | PSH_WATERMARK | PSH_HEADER | PSH_PROPSHEETPAGE ;
    propSheetHeader.hwndParent = hwndParent;
    propSheetHeader.hInstance = _Module.GetModuleInstance();
    propSheetHeader.hIcon = NULL;
    propSheetHeader.pszCaption = NULL;
    propSheetHeader.nPages = m_uPageCount;
    propSheetHeader.nStartPage = 0;
    propSheetHeader.ppsp = m_pPropSheetPages;
    propSheetHeader.pfnCallback  = NULL;
    propSheetHeader.pszbmWatermark = MAKEINTRESOURCEA(IDB_WATERMARK);
    propSheetHeader.pszbmHeader = MAKEINTRESOURCEA(IDB_HEADER);

    _ASSERTE(m_uPageCount > 0);
    long lError = PropertySheetA(&propSheetHeader);
    if(-1==lError)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    return S_OK;
}

void CWizard::AddPage(CWizardPage *pPage)
{
    UINT uPageIndex;
    _ASSERTE(m_uPageCount < m_uMaxPages);
    
    //
    //  Add a pointer to the page to our array of property pages.
    //
    uPageIndex = m_uPageCount++;
    m_pPages[uPageIndex].pWizardPage = pPage;

    //
    //  Get the page to fillout its PROPSHEETPAGEA structure.
    //
    pPage->FilloutPropSheetPage(m_pPropSheetPages+uPageIndex, m_hTitleFont);
}


INT_PTR CALLBACK CWizardPage::DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWizardPage *pThis;

    //
    //  WM_INITDIALOG is special.
    //
    if(WM_INITDIALOG==uMsg)
    {
        PROPSHEETPAGEA *pPropSheetPage = (PROPSHEETPAGEA *)lParam;
        pThis = (CWizardPage *)pPropSheetPage->lParam;
        SetWindowLong(hWndDlg,DWL_USER,(LONG)pThis);
        pThis->m_hPageDlg = hWndDlg;
        return pThis->OnInitDialog((HWND)wParam);
    }

    pThis = (CWizardPage *)GetWindowLong(hWndDlg, DWL_USER);
    if(pThis)
    {
        switch(uMsg)
        {
          
          case WM_NOTIFY:
          {
            LPNMHDR lpnm = (LPNMHDR) lParam;
            switch (lpnm->code)
            {
                case PSN_SETACTIVE: //Enable the Next button
                    return pThis->OnSetActive();
                case PSN_WIZBACK:
                    return pThis->OnWizBack();
                case PSN_WIZNEXT:
                    return pThis->OnWizNext();
                case PSN_WIZFINISH:
                    return pThis->OnWizFinish();
                default:
                    {
                    INT_PTR iRet = pThis->OnNotify(lpnm);
                    if(iRet)return iRet;
                    }break;
            }
          }
          case WM_COMMAND:
            return pThis->OnCommand(wParam, lParam);
          default:        
            return pThis->OnMessage(uMsg, wParam, lParam);
        }
    }
    return 0;
}

INT_PTR CWizardPage::OnInitDialog(HWND hwndDefaultControl)
{
    // If the page has an item of IDC_XB_TITLE it should be
    // in the title font.
    HWND hwndTitle = GetDlgItem(m_hPageDlg, IDC_XB_TITLE);
    if(hwndTitle)
    {
        SendMessage(hwndTitle, WM_SETFONT, (WPARAM)m_hTitleFont, 0);
    }
    return 0;
}

void CWizardPage::FilloutPropSheetPage(PROPSHEETPAGEA *pPropSheetPage, HFONT hTitleFont)
{
    m_hTitleFont = hTitleFont;
    pPropSheetPage->dwSize = sizeof(PROPSHEETPAGE);
    pPropSheetPage->dwFlags = PSP_DEFAULT;
    pPropSheetPage->hInstance = _Module.GetModuleInstance();
    pPropSheetPage->pfnDlgProc = DialogProc;
    pPropSheetPage->pfnCallback = NULL;
    pPropSheetPage->lParam = (LPARAM)this;
    CompletePropSheetPage(pPropSheetPage);
}

void CAddConsoleWelcome::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
	pPropSheetPage->dwFlags |= PSP_HIDEHEADER;
	pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_ADDCONSOLE_WELCOME);
}

INT_PTR CAddConsoleWelcome::OnSetActive()
{
    PropSheet_SetWizButtons(GetParent(m_hPageDlg), PSWIZB_NEXT);
    return 0;
}

INT_PTR CAddConsoleGetName::OnInitDialog(HWND)
{
    return TRUE;
}

void CAddConsoleGetName::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
    pPropSheetPage->dwFlags |= PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_ADDCONSOLE_GETNAME);
	pPropSheetPage->pszHeaderTitle =    MAKEINTRESOURCEA(IDS_ADDCONSOLE_GETNAME_TITLE);
	pPropSheetPage->pszHeaderSubTitle = MAKEINTRESOURCEA(IDS_ADDCONSOLE_GETNAME_SUBTITLE);
}

INT_PTR CAddConsoleGetName::OnSetActive()
{
    HWND hwndCtrl = GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_NAME);
    if(hwndCtrl)
    {
       char szConsoleName[80];
       if(m_pAddConsoleData->GetConsoleName(szConsoleName))
       {
            PropSheet_SetWizButtons(GetParent(m_hPageDlg),PSWIZB_BACK|PSWIZB_NEXT);
       } else
       {
            PropSheet_SetWizButtons(GetParent(m_hPageDlg),PSWIZB_BACK);
       }
       SetWindowTextA(hwndCtrl, szConsoleName);
    }
    return 0;
}

INT_PTR CAddConsoleGetName::OnWizNext()
{
    char szConsoleName[80];
    DWORD dwIpAddress;
    
    // Retrieve the name of the console
    HWND hwndCtrl = GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_NAME);
    GetWindowTextA(hwndCtrl, szConsoleName, sizeof(szConsoleName));
    m_pAddConsoleData->SetConsoleName(szConsoleName);

    //Make sure that the Xbox exists.
    if(!m_pAddConsoleData->GetIpAddress(&dwIpAddress))
    {
        // Could not find Xbox - show dialog, and return to this page.
        WindowUtils::MessageBoxResource(m_hPageDlg, IDS_CONSOLE_NOT_FOUND, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP, szConsoleName);
        SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_GETNAME);
    } else
    {
        SetWindowLong(m_hPageDlg, DWL_MSGRESULT, m_pAddConsoleData->ShowAccessDenied() ? IDD_ADDCONSOLE_ACCESSDENIED : IDD_ADDCONSOLE_MAKEDEFAULT);
    }
    return TRUE;
}

INT_PTR CAddConsoleGetName::OnWizBack()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_WELCOME);
    return TRUE;
}

INT_PTR CAddConsoleGetName::OnCommand(WPARAM wParam, LPARAM lParam)
{
    char szConsoleName[80];
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);
    if (uCommand == EN_CHANGE)
    {
        if(uControlId == IDC_ADDCONSOLE_NAME)
        {
           if(GetWindowTextA(GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_NAME), szConsoleName, sizeof(szConsoleName)))
           {
                PropSheet_SetWizButtons(GetParent(m_hPageDlg),PSWIZB_BACK|PSWIZB_NEXT);
           } else
           {
                PropSheet_SetWizButtons(GetParent(m_hPageDlg),PSWIZB_BACK);
           }
           return TRUE;
        }
    }
    return FALSE;
}

void CAddConsoleAccessDenied::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
    pPropSheetPage->dwFlags |= PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_ADDCONSOLE_ACCESSDENIED);
	pPropSheetPage->pszHeaderTitle =    MAKEINTRESOURCEA(IDS_ADDCONSOLE_ACCESSDENIED_TITLE);
	pPropSheetPage->pszHeaderSubTitle = MAKEINTRESOURCEA(IDS_ADDCONSOLE_ACCESSDENIED_SUBTITLE);
}

INT_PTR CAddConsoleAccessDenied::OnInitDialog(HWND hwndDefaultControl)
{
    return TRUE;
}

INT_PTR CAddConsoleAccessDenied::OnSetActive()
{
    char szPassword[50];
    DWORD dwDesiredAccess = m_pAddConsoleData->GetDesiredAccess();
    
    // Set the check boxes
    CheckDlgButton(m_hPageDlg, IDC_ADDCONSOLE_READ,      (dwDesiredAccess&DMPL_PRIV_READ)      ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hPageDlg, IDC_ADDCONSOLE_WRITE,     (dwDesiredAccess&DMPL_PRIV_WRITE)     ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hPageDlg, IDC_ADDCONSOLE_MANAGE,    (dwDesiredAccess&DMPL_PRIV_MANAGE)    ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hPageDlg, IDC_ADDCONSOLE_CONFIGURE, (dwDesiredAccess&DMPL_PRIV_CONFIGURE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hPageDlg, IDC_ADDCONSOLE_CONTROL,   (dwDesiredAccess&DMPL_PRIV_CONTROL)   ? BST_CHECKED : BST_UNCHECKED);
    
    // If there is any access at all, and their is any password, then enable the next button
    if(dwDesiredAccess)
    {
        // Get the window text to see if there is password in it.
        if(GetWindowTextA(GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_PASSWORD), szPassword, sizeof(szPassword)))
        {
            PropSheet_SetWizButtons(GetParent(m_hPageDlg), PSWIZB_BACK | PSWIZB_NEXT);
            return TRUE;
        }
    }
    
    //If we fell through to here, we are not ready to enable the next button
    PropSheet_SetWizButtons(GetParent(m_hPageDlg), PSWIZB_BACK);
    return TRUE;
}

INT_PTR CAddConsoleAccessDenied::OnWizNext()
{
    HRESULT hr;
    char szPassword[60];
    // We need to validate the password, if it is valid go on to the 
    // make default page.  Otherwise, pop-up an invalid password dialog
    // and stay right here.
    GetWindowTextA(GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_PASSWORD), szPassword, sizeof(szPassword));
    hr = m_pAddConsoleData->SetPassword(szPassword);
    if(SUCCEEDED(hr))
    {
        SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_MAKEDEFAULT);
    } else
    {
        char szConsoleName[60];
        char szErrorString[80];
        m_pAddConsoleData->GetConsoleName(szConsoleName);
        FormatUtils::XboxErrorString(hr, szErrorString, sizeof(szErrorString));
        //
        //  Format the error code, and tell the user why we cannot move on to the next step.
        //
        WindowUtils::MessageBoxResource(m_hPageDlg, IDS_SECURE_CONNECTION_FAILED, IDS_GENERIC_CAPTION, MB_ICONSTOP|MB_OK, szConsoleName, szErrorString);
        SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_ACCESSDENIED);
    }
    return TRUE;    
}

INT_PTR CAddConsoleAccessDenied::OnWizBack()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_GETNAME);
    return TRUE;
}

INT_PTR CAddConsoleAccessDenied::OnCommand(WPARAM wParam, LPARAM lParam)
{
    char szPassword[80];
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);
    DWORD dwDesiredAccess = m_pAddConsoleData->GetDesiredAccess();
    BOOL fReturn = FALSE;
    
    if(uCommand == EN_CHANGE)
    {
        if(uControlId == IDC_ADDCONSOLE_PASSWORD)
        {
          fReturn = TRUE;
        }
    } else if(uCommand == BN_CLICKED)
    {
        fReturn = TRUE;
        switch(uControlId)
        {
          case IDC_ADDCONSOLE_READ:
            dwDesiredAccess ^= DMPL_PRIV_READ;
            break;                
          case IDC_ADDCONSOLE_WRITE:
            dwDesiredAccess ^= DMPL_PRIV_WRITE;
            break;
          case IDC_ADDCONSOLE_MANAGE:
            dwDesiredAccess ^= DMPL_PRIV_MANAGE;
            break;
          case IDC_ADDCONSOLE_CONFIGURE:
            dwDesiredAccess ^= DMPL_PRIV_CONFIGURE;
            break;
          case IDC_ADDCONSOLE_CONTROL:
            dwDesiredAccess ^= DMPL_PRIV_CONTROL;
            break;
          default:
             fReturn = FALSE;
        };
        m_pAddConsoleData->SetDesiredAccess(dwDesiredAccess);
    }

    if(dwDesiredAccess)
    {
        if(GetWindowTextA(GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_PASSWORD), szPassword, sizeof(szPassword)))
        {
            PropSheet_SetWizButtons(GetParent(m_hPageDlg),PSWIZB_BACK | PSWIZB_NEXT);
            return fReturn;
         }
    }
    PropSheet_SetWizButtons(GetParent(m_hPageDlg),PSWIZB_BACK);
    return fReturn;
}
   
void CAddConsoleMakeDefault::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
    pPropSheetPage->dwFlags |= PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_ADDCONSOLE_MAKEDEFAULT);
	pPropSheetPage->pszHeaderTitle =    MAKEINTRESOURCEA(IDS_ADDCONSOLE_MAKEDEFAULT_TITLE);
	pPropSheetPage->pszHeaderSubTitle = MAKEINTRESOURCEA(IDS_ADDCONSOLE_MAKEDEFAULT_SUBTITLE);
}

INT_PTR CAddConsoleMakeDefault::OnInitDialog(HWND hwndDefaultControl)
{
    return TRUE;
}

INT_PTR CAddConsoleMakeDefault::OnSetActive()
{
    char szConsoleName[60];
    char szQuestion[120];
    m_pAddConsoleData->GetConsoleName(szConsoleName);
    WindowUtils::rsprintf(szQuestion, IDS_ADDCONSOLE_MAKEDEFAULT_QUESTION, szConsoleName);
    SetWindowTextA(GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_MAKEDEFAULT), szQuestion);
    BOOL fMakeDefault = m_pAddConsoleData->GetMakeDefault();
    CheckDlgButton(m_hPageDlg, IDC_ADDCONSOLE_YES, fMakeDefault ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hPageDlg, IDC_ADDCONSOLE_NO, fMakeDefault ? BST_UNCHECKED : BST_CHECKED);
    PropSheet_SetWizButtons(GetParent(m_hPageDlg), PSWIZB_BACK|PSWIZB_NEXT);
    return TRUE;
}

INT_PTR CAddConsoleMakeDefault::OnWizNext()
{
    m_pAddConsoleData->SetMakeDefault((BST_CHECKED==IsDlgButtonChecked(m_hPageDlg, IDC_ADDCONSOLE_YES)) ? TRUE : FALSE);
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_FINISH);
    return TRUE;
}

INT_PTR CAddConsoleMakeDefault::OnWizBack()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, m_pAddConsoleData->ShowAccessDenied() ? IDD_ADDCONSOLE_ACCESSDENIED : IDD_ADDCONSOLE_GETNAME);
    return TRUE;
}

void CAddConsoleFinish::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
    pPropSheetPage->dwFlags |= PSP_HIDEHEADER;
	pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_ADDCONSOLE_FINISH);
}

INT_PTR CAddConsoleFinish::OnSetActive()
{
    char szTemp[80];
    char szConsole[100];
    DWORD dwIpAddress;
    
    //
    //  Fill out the Xbox Name and IP address
    //
    m_pAddConsoleData->GetConsoleName(szTemp);
    m_pAddConsoleData->GetIpAddress(&dwIpAddress);
    wsprintfA(szConsole, "%s(%d.%d.%d.%d)", szTemp, (dwIpAddress >> 24)&0xFF, (dwIpAddress >> 16)&0xFF, (dwIpAddress >> 8)&0xFF, dwIpAddress&0xFF);
    SetWindowTextA(GetDlgItem(m_hPageDlg,IDC_ADDCONSOLE_NAME), szConsole);
    
    //
    //  Fill out the MAKEDEFAULT field
    //
    LoadStringA(_Module.GetModuleInstance(), m_pAddConsoleData->GetMakeDefault() ? IDS_LITERAL_YES : IDS_LITERAL_NO, szTemp, sizeof(szTemp));
    SetWindowTextA(GetDlgItem(m_hPageDlg,IDC_ADDCONSOLE_MAKEDEFAULT), szTemp);

    //
    //  Fill out permission changes.
    //
    if(m_pAddConsoleData->ShowAccessDenied())
    {   
        char szPermissions[100] = {0};
        ShowWindow(GetDlgItem(m_hPageDlg,IDC_ADDCONSOLE_PERMISSIONS_CAPTION), SW_SHOW);
        ShowWindow(GetDlgItem(m_hPageDlg,IDC_ADDCONSOLE_PERMISSIONS), SW_SHOW);

        //Go through each of the permissions and add a string if necessary.
        //Since I don't want to depend on the actual order of the bits in the
        //access flags (they are defined by xboxdbg), I best just test each 
        //one with individually, rather than looping.
        
        DWORD dwAccess = m_pAddConsoleData->GetDesiredAccess();
        if(dwAccess&DMPL_PRIV_READ)
        {
            dwAccess &= ~DMPL_PRIV_READ;
            LoadStringA(_Module.GetModuleInstance(), IDS_PERMISSION_READ, szTemp, sizeof(szTemp));
            strcat(szPermissions, szTemp);
            if(dwAccess) strcat(szPermissions, ", ");
        }
        if(dwAccess&DMPL_PRIV_WRITE)
        {
            dwAccess &= ~DMPL_PRIV_WRITE;
            LoadStringA(_Module.GetModuleInstance(), IDS_PERMISSION_WRITE, szTemp, sizeof(szTemp));
            strcat(szPermissions, szTemp);
            if(dwAccess) strcat(szPermissions, ", ");
        }
        if(dwAccess&DMPL_PRIV_CONFIGURE)
        {
            dwAccess &= ~DMPL_PRIV_CONFIGURE;
            LoadStringA(_Module.GetModuleInstance(), IDS_PERMISSION_CONFIGURE, szTemp, sizeof(szTemp));
            strcat(szPermissions, szTemp);
            if(dwAccess) strcat(szPermissions, ", ");
        }
        if(dwAccess&DMPL_PRIV_CONTROL)
        {
            dwAccess &= ~DMPL_PRIV_CONTROL;
            LoadStringA(_Module.GetModuleInstance(), IDS_PERMISSION_CONTROL, szTemp, sizeof(szTemp));
            strcat(szPermissions, szTemp);
            if(dwAccess) strcat(szPermissions, ", ");
        }
        if(dwAccess&DMPL_PRIV_MANAGE)
        {
            dwAccess &= ~DMPL_PRIV_MANAGE;
            LoadStringA(_Module.GetModuleInstance(), IDS_PERMISSION_MANAGE, szTemp, sizeof(szTemp));
            strcat(szPermissions, szTemp);
        }
        SetWindowTextA(GetDlgItem(m_hPageDlg,IDC_ADDCONSOLE_PERMISSIONS), szPermissions);

    } else
    {
        ShowWindow(GetDlgItem(m_hPageDlg,IDC_ADDCONSOLE_PERMISSIONS_CAPTION), SW_HIDE);
        ShowWindow(GetDlgItem(m_hPageDlg,IDC_ADDCONSOLE_PERMISSIONS), SW_HIDE);
    }

    PropSheet_SetWizButtons(GetParent(m_hPageDlg), PSWIZB_BACK|PSWIZB_FINISH);
    return 0;
}

INT_PTR CAddConsoleFinish::OnWizBack()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_MAKEDEFAULT);
    return TRUE;
}

INT_PTR CAddConsoleFinish::OnWizFinish()
{
    HRESULT hr = m_pAddConsoleData->OnFinish();
    if(FAILED(hr))
    {
        char szConsoleName[60];
        char szErrorString[80];
        m_pAddConsoleData->GetConsoleName(szConsoleName);
        FormatUtils::XboxErrorString(hr, szErrorString, sizeof(szErrorString));
        WindowUtils::MessageBoxResource(m_hPageDlg, IDS_SECURE_CONNECTION_FAILED, IDS_GENERIC_CAPTION, MB_ICONSTOP|MB_OK, szConsoleName, szErrorString);
        return TRUE;
    }        
    return 0;
}

HRESULT CAddConsoleData::OnFinish()
{
    HRESULT hr = S_OK;
    
    // Step 1:  If we need to change the access permissions
    //           do that first.
    if(m_dwAccess != m_dwDesiredAccess)
    {

        IXboxConnection *pConnection;
        hr = Utils::GetXboxConnection(m_szConsoleName, &pConnection);
        if(SUCCEEDED(hr))
        {
            hr = pConnection->HrUseSharedConnection(TRUE);
            if(SUCCEEDED(hr))
            {
                hr = pConnection->HrUseSecureConnection(m_szPassword);
                if(SUCCEEDED(hr))
                {   

                    char szComputerName[MAX_COMPUTERNAME_LENGTH+1];
                    DWORD dwComputerNameLength = sizeof(szComputerName);
                    if(GetComputerNameA(szComputerName, &dwComputerNameLength))
                    {
                        hr = pConnection->HrSetUserAccess(szComputerName, m_dwDesiredAccess);
                    } else 
                    {
                        hr = HRESULT_FROM_WIN32(GetLastError());
                    }
                    // Trust that GetComputerNameA will never ever return XBDM_NOSUCHFILE, considering it has facility code, this
                    // is a very good bet.
                    if(XBDM_NOSUCHFILE == hr)
                    {
                        hr = pConnection->HrAddUser(szComputerName, m_dwDesiredAccess);
                    } 
                }
            }
            pConnection->Release();
        }
    }
    if(FAILED(hr))
    {
        return hr;
    }

    // Step 2:  Add to the Xbox namespace
    //
    CManageConsoles manageConsole;
    manageConsole.Add(m_szConsoleName);

    // Step 3:  If we were suppose to make this default than do that.
    if(m_fMakeDefault)
    {
        manageConsole.SetDefault(m_szConsoleName);
    }

    // Step 4: Tell the shell that the contents of Xbox Neighbor hood has
    //         changed.
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH|SHCNF_FLUSH, ROOT_GUID_NAME_WIDE, NULL);

    return hr;
}

BOOL  CAddConsoleData::SetConsoleName(LPCSTR pszConsoleName)
{
    strcpy(m_szConsoleName, pszConsoleName);
    IXboxConnection *pConnection;
    
    // Wipe out any information we have on the console
    m_fConsoleIsValid = FALSE;
    m_fMakeDefault = FALSE;
    m_dwAccess = 0; //Assume no access, until we know otherwise
    m_dwIpAddress = 0;
    m_szPassword[0] = '\0';
    
    if(SUCCEEDED(Utils::GetXboxConnection(m_szConsoleName,&pConnection)))
    {
        if(SUCCEEDED(pConnection->HrResolveXboxName(&m_dwIpAddress)))
        {
            BOOL fLocked = FALSE;
            if(SUCCEEDED(pConnection->HrIsSecurityEnabled(&fLocked)))
            {
                //If this succeeded then we know the box exists.
                m_fConsoleIsValid = TRUE;
                if(fLocked)
                {
                    //
                    //  Assume no access, until we can get it.
                    //
                    m_dwAccess = 0;
            
                    //
                    //  If they don't have access
                    //
                    HRESULT hr = pConnection->HrGetUserAccess(NULL, &m_dwAccess);
                    if(XBDM_UNDEFINED==hr)
                    {
                        //Prior to build 4114, we couldn't just get our own permission by passing
                        //NULL.  It would fail with XBDM_UNDEFINED.  We could ask by machine name,
                        //but that only works if we have manage permission.  Just say all priviledge
                        //so that the use can add the box.
                        //If they don't have access, then they won't be able to do anything with, big whoop.
                        //And they won't have the nice convience of our access denied page.
                        m_dwAccess = DMPL_PRIV_ALL;
                    }
                }  else
                {
                    m_dwAccess = DMPL_PRIV_ALL;
                }
            }
        }
        pConnection->Release();
    }
    
    // By default the user wants the currently desired access.
    m_dwDesiredAccess = m_dwAccess;
    return m_fConsoleIsValid;
}

BOOL  CAddConsoleData::GetConsoleName(LPSTR pszConsoleName)
{
    _ASSERTE(pszConsoleName);
    strcpy(pszConsoleName, m_szConsoleName);
    return ('\0'==*pszConsoleName) ? FALSE : TRUE;
}

BOOL  CAddConsoleData::GetIpAddress(PDWORD pdwIpAddress)
{
    if(m_fConsoleIsValid)
    {
        *pdwIpAddress = m_dwIpAddress;
        return TRUE;
    }
    return FALSE;
}

void  CAddConsoleData::SetMakeDefault(BOOL fSet)
{
    m_fMakeDefault = fSet;
}

BOOL  CAddConsoleData::GetMakeDefault()
{
    return m_fMakeDefault;
}

BOOL  CAddConsoleData::ShowAccessDenied()
{
    return (m_dwAccess&DMPL_PRIV_ALL) ? FALSE : TRUE;
}

void  CAddConsoleData::SetDesiredAccess(DWORD dwDesiredAccess)
{
    m_dwDesiredAccess = dwDesiredAccess;
}

DWORD CAddConsoleData::GetDesiredAccess()
{
    return (m_dwDesiredAccess&DMPL_PRIV_ALL);
}

HRESULT  CAddConsoleData::SetPassword(LPCSTR pszAdminPassword)
{   
    strcpy(m_szPassword, pszAdminPassword);
    HRESULT hr;
    //
    // Validate the password, do this by making a connection, and attempting to 
    // establish a secure connection.
    //
    IXboxConnection *pConnection;
    hr = Utils::GetXboxConnection(m_szConsoleName,&pConnection);
    if(SUCCEEDED(hr))
    {
        hr = pConnection->HrUseSharedConnection(TRUE);
        if(SUCCEEDED(hr))
        {
            hr = pConnection->HrUseSecureConnection(m_szPassword);
        }
        pConnection->Release();
    }
    return hr;
}

DWORD WINAPI ExecuteAddConsoleWizardThread(PVOID)
{
    HRESULT hr;
    CAddConsoleData addConsoleData;
    CWizard wizard;
    hr = wizard.Initialize(5);
    if(SUCCEEDED(hr))
    {
        CAddConsoleWelcome      welcome;
        CAddConsoleGetName      getName(&addConsoleData);
        CAddConsoleAccessDenied accessDenied(&addConsoleData);
        CAddConsoleMakeDefault  makeDefault(&addConsoleData);
        CAddConsoleFinish       finish(&addConsoleData);

        wizard.AddPage(&welcome);
        wizard.AddPage(&getName);
        wizard.AddPage(&accessDenied);
        wizard.AddPage(&makeDefault);
        wizard.AddPage(&finish);
    }
    HWND hwndWorker = WindowUtils::CreateWorkerWindow(NULL);
    wizard.DoWizard(hwndWorker);
    if(hwndWorker) DestroyWindow(hwndWorker);
    return 0;
}

extern void ExecuteAddConsoleWizard()
{
    SHCreateThread(ExecuteAddConsoleWizardThread, NULL, CTF_COINIT|CTF_PROCESS_REF, NULL);
}
