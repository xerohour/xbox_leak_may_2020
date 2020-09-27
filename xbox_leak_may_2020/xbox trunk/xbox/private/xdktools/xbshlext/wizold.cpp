###
### THIS FILE IS FOR ARCHIVAL PURPOSES
### CAddConsoleData Has been changed extensively, and I just wanted to preserve the code
### somewhere.  The major cause of the change is that this version of the Add New Console
### wizard allowed adding more than one Xbox at a time, the new wizard does not allow this.
###

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
    propSheetHeader.hwndParent = GetDesktopWindow();
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

#ifdef NEVER
void CAddConsoleHow::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
    pPropSheetPage->dwFlags |= PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_ADDCONSOLE_HOW);
	pPropSheetPage->pszHeaderTitle =    MAKEINTRESOURCEA(IDS_ADDCONSOLE_HOW_TITLE);
	pPropSheetPage->pszHeaderSubTitle = MAKEINTRESOURCEA(IDS_ADDCONSOLE_HOW_SUBTITLE);
}

INT_PTR CAddConsoleHow::OnSetActive()
{
    HWND hwndCtrl = GetDlgItem(m_hPageDlg, m_pAddConsoleData->m_fChoose ? IDC_ADDCONSOLE_CHOOSE : IDC_ADDCONSOLE_BYNAME);
    if(hwndCtrl)
    {
        SendMessage(hwndCtrl, BM_SETCHECK, BST_UNCHECKED, 0);
    }
    PropSheet_SetWizButtons(GetParent(m_hPageDlg), PSWIZB_BACK|PSWIZB_NEXT);
    return 0;
}

INT_PTR CAddConsoleHow::OnWizNext()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, /*m_pAddConsoleData->m_fChoose ? IDD_ADDCONSOLE_CHOOSE : */IDD_ADDCONSOLE_BYNAME);
    return TRUE;
}

INT_PTR CAddConsoleHow::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);
    
    //
    //  Flip m_pAddConsoleData->fChoose when the
    //  radio buttons are clicked.
    //
    if( uCommand == BN_CLICKED) 
    { 
        if(uControlId==IDC_ADDCONSOLE_CHOOSE)
        {
            m_pAddConsoleData->m_fChoose = TRUE;
            return TRUE;
        }
        if(uControlId==IDC_ADDCONSOLE_BYNAME)
        {
           m_pAddConsoleData->m_fChoose = FALSE;
           return TRUE;
        }
    }
    return FALSE;
}

INT_PTR CAddConsoleChoose::OnInitDialog(HWND)
{
    // Set up the columns of the list view.
    HWND hwndCtrl = GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_LIST);
    if(hwndCtrl)
    {
        m_pAddConsoleData->InitListView(hwndCtrl, true);
        return TRUE; 
    }
    return FALSE;
}
void CAddConsoleChoose::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
    pPropSheetPage->dwFlags |= PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_ADDCONSOLE_CHOOSE);
	pPropSheetPage->pszHeaderTitle =    MAKEINTRESOURCEA(IDS_ADDCONSOLE_CHOOSE_TITLE);
	pPropSheetPage->pszHeaderSubTitle = MAKEINTRESOURCEA(IDS_ADDCONSOLE_CHOOSE_SUBTITLE);
}

INT_PTR CAddConsoleChoose::OnSetActive()
{
    HWND hwndCtrl = GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_LIST);
    if(hwndCtrl)
    {
        m_pAddConsoleData->RefreshListView(hwndCtrl);
        PropSheet_SetWizButtons(
            GetParent(m_hPageDlg),
            m_pAddConsoleData->GetSelectCount() ? PSWIZB_BACK|PSWIZB_NEXT : PSWIZB_BACK
            );
    }
    return 0;
}

INT_PTR CAddConsoleChoose::OnWizNext()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_FINISH);
    return TRUE;   
}

INT_PTR CAddConsoleChoose::OnWizBack()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_HOW);
    return TRUE;   
}

INT_PTR CAddConsoleChoose::OnNotify(LPNMHDR pnmhdr)
{
    INT_PTR iRet = 0;
    if(IDC_ADDCONSOLE_LIST==pnmhdr->idFrom)
    {
        
        iRet = m_pAddConsoleData->OnNotify(pnmhdr);
        PropSheet_SetWizButtons(
            GetParent(m_hPageDlg),
            m_pAddConsoleData->GetSelectCount() ? PSWIZB_BACK|PSWIZB_NEXT : PSWIZB_BACK
            );
    }
    return iRet;
}
#endif NEVER

INT_PTR CAddConsoleByName::OnInitDialog(HWND)
{
    // Set up the columns of the list view.
    HWND hwndCtrl = GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_LIST);
    if(hwndCtrl)
    {   
       m_pAddConsoleData->InitListView(hwndCtrl);
       return TRUE; 
    }
    return FALSE;
}

void CAddConsoleByName::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
    pPropSheetPage->dwFlags |= PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_ADDCONSOLE_BYNAME);
	pPropSheetPage->pszHeaderTitle =    MAKEINTRESOURCEA(IDS_ADDCONSOLE_BYNAME_TITLE);
	pPropSheetPage->pszHeaderSubTitle = MAKEINTRESOURCEA(IDS_ADDCONSOLE_BYNAME_SUBTITLE);
}

INT_PTR CAddConsoleByName::OnSetActive()
{
   
    HWND hwndCtrl = GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_LIST);
    if(hwndCtrl)
    {
        m_pAddConsoleData->RefreshListView(hwndCtrl);
        PropSheet_SetWizButtons(
            GetParent(m_hPageDlg),
            m_pAddConsoleData->GetSelectCount() ? PSWIZB_BACK|PSWIZB_NEXT : PSWIZB_BACK
            );
    }
    return 0;
}

INT_PTR CAddConsoleByName::OnWizNext()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_FINISH);
    return TRUE;
}

INT_PTR CAddConsoleByName::OnWizBack()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, IDD_ADDCONSOLE_WELCOME);
    return TRUE;   
}

INT_PTR CAddConsoleByName::OnNotify(LPNMHDR pnmhdr)
{
    INT_PTR iRet = 0;
    if(IDC_ADDCONSOLE_LIST==pnmhdr->idFrom)
    {
        
        iRet = m_pAddConsoleData->OnNotify(pnmhdr);
        PropSheet_SetWizButtons(
            GetParent(m_hPageDlg),
            m_pAddConsoleData->GetSelectCount() ? PSWIZB_BACK|PSWIZB_NEXT : PSWIZB_BACK
            );
    }
    return iRet;
}

INT_PTR CAddConsoleByName::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);
    
    //
    //  If Flip the m_fRecursive as buttons are pressed.
    //  
    //
    if(uCommand == BN_CLICKED) 
    {
        if(uControlId == IDC_ADDCONSOLE_ADD)
        {
            HWND hEditWnd = GetDlgItem(this->m_hPageDlg, IDC_ADDCONSOLE_NAME);
            if(hEditWnd)
            {
                char szConsoleName[MAX_PATH]; 
                GetWindowTextA(hEditWnd, szConsoleName, sizeof(szConsoleName));
                HWND hListView = GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_LIST);
                if(hListView)
                {
                    m_pAddConsoleData->AddConsole(hListView, szConsoleName);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

void CAddConsoleFinish::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
    pPropSheetPage->dwFlags |= PSP_HIDEHEADER;
	pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_ADDCONSOLE_FINISH);
}

INT_PTR CAddConsoleFinish::OnSetActive()
{
    PropSheet_SetWizButtons(GetParent(m_hPageDlg), PSWIZB_BACK|PSWIZB_FINISH);
    HWND hCtrl = GetDlgItem(m_hPageDlg, IDC_ADDCONSOLE_LIST);
    if(hCtrl)
    {
        char szList[1024];
        m_pAddConsoleData->GetSelectedItemsString(szList, sizeof(szList));
        SetWindowTextA(hCtrl, szList);
    }
    return 0;
}

INT_PTR CAddConsoleFinish::OnWizBack()
{
    SetWindowLong(m_hPageDlg, DWL_MSGRESULT, m_pAddConsoleData->m_fChoose ? IDD_ADDCONSOLE_CHOOSE : IDD_ADDCONSOLE_BYNAME);
    return TRUE;
}

INT_PTR CAddConsoleFinish::OnWizFinish()
{
    m_pAddConsoleData->OnFinish();
    return 0;
}

void CAddConsoleData::InitListView(HWND hListView, bool fChoose)
{
    TCHAR tszText[256];     // temporary buffer 
    LVCOLUMN lvc;
        
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.pszText =  tszText;
        
    lvc.cx = 175;
    lvc.iSubItem = 0;
    LoadString(_Module.GetModuleInstance(), IDS_COLUMN_HEADER_NAME, tszText, ARRAYSIZE(tszText));
    ListView_InsertColumn(hListView, 0, &lvc);
        
    lvc.cx = 100;
    lvc.iSubItem = 1;
    LoadString(_Module.GetModuleInstance(), IDS_COLUMN_HEADER_IPADDR, tszText, ARRAYSIZE(tszText));
    ListView_InsertColumn(hListView, 1, &lvc);
    
    if(fChoose)
    {
        ListView_SetExtendedListViewStyle(hListView, LVS_EX_CHECKBOXES);
    }

}

void CAddConsoleData::RefreshListView(HWND hListView)
{
    
    // First Delete everything.
    ListView_DeleteAllItems(hListView);
    CONSOLE_INFO *pConsole = m_pConsoles;
    int iItem = 0;
    
    
    while(pConsole)
    {
        if(m_fChoose || pConsole->fSelected)
        {
            AddItemToListView(hListView, iItem, pConsole);
            iItem++;
        }
        pConsole = pConsole->pNext;
    }
};

void CAddConsoleData::AddConsole(HWND hListView, LPSTR pszConsoleName)
{
    _ASSERTE(!m_fChoose);
    HRESULT hr;
    CONSOLE_INFO *pNewConsole;

    // Don't add it if we already have it.
    pNewConsole = m_pConsoles;
    while(pNewConsole)
    {
        if(0==_stricmp(pszConsoleName, pNewConsole->pszName))
        {
            return;
        }
        pNewConsole = pNewConsole->pNext;
    }
    
    pNewConsole = new CONSOLE_INFO;
    if(pNewConsole)
    {
        memset(pNewConsole, 0, sizeof(CONSOLE_INFO));
        IXboxConnection *pConnection;
        hr = DmGetXboxConnection(pszConsoleName, XBCONN_VERSION, &pConnection);
        if(SUCCEEDED(hr))
        {
            hr = pConnection->HrResolveXboxName(&pNewConsole->dwIpAddress);
            pConnection->Release();
        }
        if(FAILED(hr))
        {
            MessageBoxA(GetParent(hListView), "Could not find specified Xbox", "BUGBUG - Finish Dialog", MB_OK|MB_ICONERROR);
            delete pNewConsole;
            return;
        }
        UINT uNameLen = strlen(pszConsoleName)+1;
        pNewConsole->pszName = new char[uNameLen];
        if(pNewConsole)
        {
            memcpy(pNewConsole->pszName, pszConsoleName, uNameLen);
            pNewConsole->fSelected = TRUE;
            m_iConsolesSelected++;
            pNewConsole->fLocalSubnet = FALSE;
        
            // Insert into tail of list
            pNewConsole->pNext = NULL;
            if(m_pConsoles)
            {
                pNewConsole->pPrevious = m_pConsolesTail;
                m_pConsolesTail->pNext = pNewConsole;
                m_pConsolesTail = pNewConsole;
            } else
            {
                pNewConsole->pPrevious = NULL;
                m_pConsolesTail = m_pConsoles = pNewConsole;
            }
            //  Insert into List View
            AddItemToListView(hListView, m_iConsoleCount++, pNewConsole);
        } else
        {
            delete pNewConsole;
        }
    }
}


INT_PTR CAddConsoleData::OnNotify(LPNMHDR pnmhdr)
{
    CONSOLE_INFO *pConsole;
    if(pnmhdr->code == LVN_GETDISPINFOA)
    {
        LVITEMA *pItem;
        pItem = &((NMLVDISPINFOA *)pnmhdr)->item;
        pConsole = (CONSOLE_INFO *)pItem->lParam;
        switch(pItem->iSubItem)
        {
          case 0:
           wsprintfA(pItem->pszText, "%hs", pConsole->pszName);
           break;
          case 1:
           wsprintfA(pItem->pszText,
             "%d.%d.%d.%d", 
             (pConsole->dwIpAddress >> 24)&0xFF,
             (pConsole->dwIpAddress >> 16)&0xFF,
             (pConsole->dwIpAddress >> 8)&0xFF,
             pConsole->dwIpAddress&0xFF
              );
           break;
          default:
            break;
         }
        return TRUE;
    } else if(pnmhdr->code == LVN_ITEMCHANGED)
    {
        NMLISTVIEW *pListView = (NMLISTVIEW *)pnmhdr;
        pConsole = (CONSOLE_INFO *)pListView->lParam;
        BOOL fSelected = (pListView->uNewState>>12)-1;
        if(fSelected != pConsole->fSelected)
        {
            pConsole->fSelected = fSelected;
            if(fSelected)
            {
                m_iConsolesSelected++;
            } else 
            {
                m_iConsolesSelected--;
            }
        }
        return TRUE;
    }
    return FALSE;
}

void CAddConsoleData::AddItemToListView(HWND hListView, int index, CONSOLE_INFO *pConsole)
{
    LVITEM lvi;
    lvi.mask = LVIF_DI_SETITEM|LVIF_PARAM|LVIF_TEXT;
    lvi.pszText = LPSTR_TEXTCALLBACK;
    lvi.cchTextMax = MAX_PATH;
    lvi.iItem = index;
    lvi.iSubItem = 0;
    lvi.lParam = (LPARAM)pConsole;
    
    //Arrgghh - When you insert an item into a list view it is
    //          never checked (according to the docs, even if
    //          we were to set LVIF_STATE! and lvi.iState).
    //          The list view sends a notification to that
    //          effect, which this class processes and then clears
    //          pConsole->fSelected all before ListView_InsertItem
    //          returns.  So the work around is to cache fSelected
    //          here before ListView_InsertItem and then to immediately
    //          restore the state with ListView_SetCheck.
    BOOL fSelected = pConsole->fSelected;
    ListView_InsertItem(hListView, &lvi);
    ListView_SetCheckState(hListView, index, fSelected);
};

void CAddConsoleData::GetSelectedItemsString(LPSTR pszBuffer, UINT ccBuffer)
{
    CONSOLE_INFO *pConsole;
    BOOL fShortened=FALSE;
    pConsole = m_pConsoles;
    ccBuffer -= sizeof(",...");  //subtract room for temination.
    while(pConsole && ccBuffer)
    {
        if(pConsole->fSelected)
        {
            UINT uNameLen = strlen(pConsole->pszName);
            if(uNameLen > ccBuffer)
            {
                break;
                fShortened = TRUE;
            }
            memcpy(pszBuffer, pConsole->pszName, uNameLen);
            pszBuffer += uNameLen;
            *pszBuffer++ = ',';
            *pszBuffer++ = ' ';
            ccBuffer -= uNameLen;
            if(ccBuffer < 2) break;
            ccBuffer--;
        }
        pConsole = pConsole->pNext;
    }
    pszBuffer -= 2;
    if(fShortened) strcpy(pszBuffer, ",...");
    *pszBuffer++ = '\0';
}

void CAddConsoleData::OnFinish()
{
    HKEY hConsoleKey;
    //
    //  Open\Create the console key
    //
    CManageConsoles consoleManager;

    CONSOLE_INFO *pConsole;
    pConsole = m_pConsoles;
    while(pConsole)
    {
        if(pConsole->fSelected)
        {
            consoleManager.Add(pConsole->pszName);
        }
        pConsole = pConsole->pNext;
    }
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH|SHCNF_FLUSH, ROOT_GUID_NAME_WIDE, NULL);
}

extern void ExecuteAddConsoleWizard()
{
    HRESULT hr;
    CAddConsoleData addConsoleData;
    CWizard wizard;
    
    hr = wizard.Initialize(5);
    if(SUCCEEDED(hr))
    {
        CAddConsoleWelcome welcome;
#ifdef NEVER
        CAddConsoleHow     how(&addConsoleData);
        CAddConsoleChoose  choose(&addConsoleData);
#endif
        CAddConsoleByName  byname(&addConsoleData);
        CAddConsoleFinish  finish(&addConsoleData);

        wizard.AddPage(&welcome);
#ifdef NEVER
        wizard.AddPage(&how);
        wizard.AddPage(&choose);
#endif 
        wizard.AddPage(&byname);
        wizard.AddPage(&finish);
    }
    wizard.DoWizard(NULL);
}
