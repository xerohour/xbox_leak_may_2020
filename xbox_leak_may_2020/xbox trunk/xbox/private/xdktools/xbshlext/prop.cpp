/*++

Copyright (c) Microsoft Corporation

Module Name:

    prop.cpp

Abstract:

    Implementation of the property sheets dialogs

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    05-25-2001 : created (mitchd)

--*/

#include <stdafx.h>
#include "drawpie.h"

#define DMPL_PRIV_ALL     (DMPL_PRIV_READ|DMPL_PRIV_WRITE|DMPL_PRIV_MANAGE|DMPL_PRIV_CONFIGURE|DMPL_PRIV_CONTROL)
#define DMPL_PRIV_INITIAL (DMPL_PRIV_READ|DMPL_PRIV_WRITE)

PropertyPageInfo::~PropertyPageInfo(){delete pPropertyPage;}

HRESULT 
CXboxPropertySheet::Initialize(
    UINT         uItemCount,
    CXboxFolder *pSelection
    )
/*++
  Routine Description:
    Initialize basically visits each of the selected items.
    
    Allocation of memory for property pages is put off
    until the first item is visited.  At that time
    we know how many pages will be allocated (one for
    each item, or one total), and we can allocate
    our basic structures.

    If the first item is a volume, then we visit each of the items
    and create a CGeneralVolumePage for each and insert them
    into ourselves.

    If the first itme is a file or folder, then we create
    a CFSItemGeneralPage, and initialize it with the selection.
    It will make a clone.  We can then terminate the visiting,
    it will need to visit each them itself.
--*/
{
    m_uItemCount = uItemCount;
    m_pSelection = pSelection;
    m_fFirstVisit = TRUE;
    pSelection->VisitEach(this, IXboxVisitor::FlagContinue);
    m_pSelection = NULL;  //we were just borrowing this.
    return m_hrInitialize;
}

void CXboxPropertySheet::VisitRoot(IXboxVisit *pRoot, DWORD *pdwFlags)
/*++
  Routine Description:
    Trying to create a property page for the root.  There isn't one
    at this time, set an error.
--*/
{
    *pdwFlags = 0;
    m_hrInitialize = E_INVALIDARG;
}

void CXboxPropertySheet::VisitAddConsole(IXboxVisit *pAddConsole, DWORD *pdwFlags)
/*++
  Routine Description:
    Trying to create a property page for the "Add Xbox" item.  There isn't one
    at this time, set an error.
--*/
{
    *pdwFlags = 0;
    m_hrInitialize = E_INVALIDARG;
}

void CXboxPropertySheet::VisitConsole(IXboxConsoleVisit *pConsole, DWORD *pdwFlags)
/*++
  Routine Description:
    Trying to create a property page for a console.  There isn't one at this time,
    set an error.
--*/
{
    HRESULT hr;
    BOOL fMultipleConsoles = (m_uItemCount > 1) ? TRUE: FALSE;
    if(m_fFirstVisit)
    {
        m_fFirstVisit = FALSE;
        m_hrInitialize = E_OUTOFMEMORY;
        if(fMultipleConsoles)
        {
            m_uMaxPages = m_uItemCount;
        } else
        {
            m_uMaxPages = MAX_CONSOLE_PAGES;
        }
        m_uPageCount = 0;
        m_pPages = new PropertyPageInfo[m_uMaxPages];
        if(m_pPages)
        {
            m_pPropSheetPages = new PROPSHEETPAGEA[m_uMaxPages];
            if(m_pPropSheetPages)
            {
                m_hrInitialize = S_OK;
            }
        }
        if(FAILED(m_hrInitialize))
        {
            *pdwFlags = 0;
            return;
        }
    }

    char szConsoleName[MAX_PATH];
    IXboxConnection *pConnection;
    pConsole->GetConsoleName(szConsoleName);
    hr  = Utils::GetXboxConnection(szConsoleName, &pConnection);
    if(SUCCEEDED(hr))
    {
        //
        //  Add the general page for this console
        //  
        CConsoleGeneralPage *pGeneralPage = new CConsoleGeneralPage(
                                                        pConnection,
                                                        szConsoleName,
                                                        fMultipleConsoles
                                                        );
        if(pGeneralPage)
           AddPage(pGeneralPage);
        
        //
        // If there are not multiple pages
        // add the settings, securities, and and advanced settings
        //
        if(!fMultipleConsoles)
        {
            /*
            CConsoleSettingsPage *pSettingsPage = new CConsoleSettingsPage(pConnection);
            if(pSettingsPage)
              AddPage(pSettingsPage);
            */
            CConsoleSecurityPage *pSecurityPage = new CConsoleSecurityPage(pConnection, szConsoleName);
            if(pSecurityPage)
            {
               if(S_OK==pSecurityPage->InitSecuritySupport())
               {
                  AddPage(pSecurityPage);
               }
               else
               {
                  delete pSecurityPage;
               }
            }
            
            CConsoleAdvancedPage *pAdvancedPage = new CConsoleAdvancedPage(pConnection, szConsoleName);
            if(pAdvancedPage)
              AddPage(pAdvancedPage);
        }
    } else
    {
        // Display the console cannot be found dialog.
        WindowUtils::MessageBoxResource(NULL, IDS_CONSOLE_NOT_AVAILABLE, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP, szConsoleName);
    }
    return;
}

void CXboxPropertySheet::VisitVolume(IXboxVolumeVisit *pVolume, DWORD *pdwFlags)
/*++
  Routine Description:
    Trying to create a property page for a console.  There isn't one at this time,
    set an error.
--*/
{
    if(m_fFirstVisit)
    {
        m_fFirstVisit = FALSE;
        m_hrInitialize = E_OUTOFMEMORY;
        m_uMaxPages = m_uItemCount;
        m_uPageCount = 0;
        m_pPages = new PropertyPageInfo[m_uMaxPages];
        if(m_pPages)
        {
            m_pPropSheetPages = new PROPSHEETPAGEA[m_uMaxPages];
            if(m_pPropSheetPages)
            {
                m_hrInitialize = S_OK;
            }
        }
        if(FAILED(m_hrInitialize))
        {
            *pdwFlags = 0;
            return;
        }
    }


    //
    //  Now create a property sheet for the volume, and add it.
    //
    char szConsoleName[40];
    char        szDriveLetter[4];
    char        szDriveDescription[60];
    ULONGLONG   ullTotalSpace;
    ULONGLONG   ullFreeSpace;
    UINT        uVolumeType;
    pVolume->GetName(szDriveLetter);
    pVolume->GetConsoleName(szConsoleName);
    WindowUtils::rsprintf(szDriveDescription, IDS_NORMAL_NAME_FORMAT, szDriveLetter, szConsoleName);
    pVolume->GetDiskCapacity(&ullTotalSpace, &ullFreeSpace);
    uVolumeType = (UINT)pVolume->GetVolumeType();
    CVolumeGeneralPage *pVolumePage = 
        new CVolumeGeneralPage(
            szDriveDescription,
            ullTotalSpace,
            ullFreeSpace,
            uVolumeType,
            (m_uItemCount > 1) ? TRUE : FALSE
            );
    if(pVolumePage)
    {
        AddPage(pVolumePage);
    } else
    {
        m_hrInitialize = E_FAIL;
        *pdwFlags = 0;
        return;
    }
}

void CXboxPropertySheet::VisitFileOrDir(IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags)
/*++
  Routine Description:
    
    There are three cases where we get called:
    1) For a single folder.
    2) For a single file.
    3) First visit for multiple-items.

    In all cases, we instantiated a single property page to represent the item(s).  The
    c'tor CFSItemGeneralPage figures everything out, and clones the selection.

--*/
{
    m_hrInitialize = E_FAIL;
    m_uMaxPages = 1;
    m_uPageCount = 0;
    m_pPages = new PropertyPageInfo[m_uMaxPages];
    if(m_pPages)
    {
        m_pPropSheetPages = new PROPSHEETPAGEA[m_uMaxPages];
        if(m_pPropSheetPages)
        {
            m_hrInitialize = S_OK;
        }

        CFSItemGeneralPage *pGeneralPage = new CFSItemGeneralPage(m_pSelection, pFileOrDir, m_uItemCount);
        if(pGeneralPage)
        {
            AddPage(pGeneralPage);
        }
    }
    //
    //  No need to continue visiting sites.
    //

    *pdwFlags = 0;
}


void
CXboxPropertySheet::AddPage(
    CPropertyPage *pPage
    )
/*++
  Routine Description:
    Add a property page to a property sheet.  This must be done before calling DoProperties.
--*/
{
    UINT uPageIndex;
    _ASSERTE(m_uPageCount < m_uMaxPages);
    
    //
    //  Add a pointer to the page to our array of property pages.
    //
    uPageIndex = m_uPageCount++;
    m_pPages[uPageIndex].pPropertyPage = pPage;

    //
    //  Get the page to fillout its PROPSHEETPAGEA structure.
    //
    pPage->FilloutPropSheetPage(m_pPropSheetPages+uPageIndex);
}

HRESULT 
CXboxPropertySheet::DoProperties(
    UINT uStartPage,
    HWND hwndParent
    )
{
    m_hWndParent = hwndParent;
    m_uStartPage = uStartPage;
    if(m_uPageCount > 0)
    {
        DWORD  dwThreadId;
        if(SHCreateThread(CXboxPropertySheet::ThreadProcStart, (PVOID)this, CTF_COINIT|CTF_PROCESS_REF, NULL))
        {
            return S_OK;
        } else
        {
            delete this;
            return HRESULT_FROM_WIN32(GetLastError());
        }
    }
    return S_OK;
}

DWORD CXboxPropertySheet::ThreadProc()
{
    char szCaption[36];
    PROPSHEETHEADERA propSheetHeader;
    propSheetHeader.dwSize = sizeof(PROPSHEETHEADERA);
    propSheetHeader.dwFlags = PSH_PROPSHEETPAGE | PSH_PROPTITLE;
    propSheetHeader.hwndParent = WindowUtils::CreateWorkerWindow(m_hWndParent);;
    propSheetHeader.hInstance = NULL;
    propSheetHeader.hIcon = NULL;
    propSheetHeader.pszCaption = NULL;
    propSheetHeader.nPages = m_uPageCount;
    propSheetHeader.nStartPage = m_uStartPage;
    propSheetHeader.ppsp = m_pPropSheetPages;
    propSheetHeader.pfnCallback  = NULL;

    _ASSERTE(m_uPageCount > 0);

    //
    //  Get the caption from the first page.
    //  If there are multiple pages add ...
    //
    if(m_pPages[0].pPropertyPage->GetCaption(szCaption, ARRAYSIZE(szCaption)))
    {
        if(m_uItemCount  > 1)
        {
            wsprintfA(m_szCaption, "%s,...", szCaption);
        } else
        {
            strcpy(m_szCaption, szCaption);
        }
        propSheetHeader.pszCaption = m_szCaption;
    }
    PropertySheetA(&propSheetHeader);

    //
    //  Destroy yourself on exit
    //
    delete this;
    
    return 0;
}

INT_PTR CALLBACK CPropertyPage::DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CPropertyPage *pThis;

    //
    //  WM_INITDIALOG is special.
    //
    if(WM_INITDIALOG==uMsg)
    {
        PROPSHEETPAGEA *pPropSheetPage = (PROPSHEETPAGEA *)lParam;
        pThis = (CPropertyPage *)pPropSheetPage->lParam;
        SetWindowLong(hWndDlg,DWL_USER,(LONG)pThis);
        pThis->m_hPropDlg = hWndDlg;
        return pThis->OnInitDialog((HWND)wParam);
    }

    pThis = (CPropertyPage *)GetWindowLong(hWndDlg, DWL_USER);
    if(pThis)
    {
        switch(uMsg)
        {
          case WM_COMMAND:
            return pThis->OnCommand(wParam, lParam);
          case WM_DRAWITEM:
            return pThis->OnDrawItem((UINT)(wParam), (LPDRAWITEMSTRUCT)lParam); 
          case WM_NOTIFY:
            {
                LPPSHNOTIFY pPropSheetNotify = (LPPSHNOTIFY)lParam;
                if(PSN_APPLY == pPropSheetNotify->hdr.code)
                {
                    //
                    // If OnApply returns False, the changes could not be applied.
                    // So set a bad return result.
                    //
                    if(!pThis->OnApply(pPropSheetNotify->lParam ? true : false))
                    {
                        SetWindowLong(pThis->m_hPropDlg, DWL_MSGRESULT, PSNRET_INVALID);
                        return true;
                    }
                } else if(PSN_SETACTIVE == pPropSheetNotify->hdr.code)
                {
                    return pThis->OnSetActive();
                }
            }
          default:        
            return pThis->OnMessage(uMsg, wParam, lParam);
        }
    }
    return 0;
}

void 
CPropertyPage::FilloutPropSheetPage(
    PROPSHEETPAGEA *pPropSheetPage
    )
{
    pPropSheetPage->dwSize = sizeof(PROPSHEETPAGE);
    pPropSheetPage->dwFlags = PSP_DEFAULT;
    pPropSheetPage->hInstance = _Module.GetModuleInstance();
    pPropSheetPage->pfnDlgProc = DialogProc;
    pPropSheetPage->pfnCallback = NULL;
    pPropSheetPage->lParam = (LPARAM)this;
    CompletePropSheetPage(pPropSheetPage);
}

int CPropertyPage::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

int CPropertyPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

BOOL CVolumeGeneralPage::GetCaption(LPSTR szBuffer, ULONG ccBuffer)
{
    szBuffer[--ccBuffer] = TEXT('\0');
    strncpy(szBuffer, m_szDriveDescription, ccBuffer);
    return TRUE;
}

void CVolumeGeneralPage::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
{
    //
    //  The client will have passed in m_fUseDriveOnTabName on construction.
    //  This tells us that we need to override the "General" tab name, with
    //  the drive description.  This feature is for the case where multiple
    //  drives are opened in a single property sheet.
    //
    
    if(m_fDriveOnTab)
    {
        pPropSheetPage->dwFlags |= PSP_USETITLE;
        pPropSheetPage->pszTitle = m_szDriveDescription;
    }
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_DRV_GENERAL);
}

void CVolumeGeneralPage::DrawColorRect(HDC hdc, COLORREF crDraw, const RECT *prc)
{
    HBRUSH hbDraw = CreateSolidBrush(crDraw);
    if (hbDraw)
    {
        HBRUSH hbOld = (HBRUSH)SelectObject(hdc, hbDraw);
        if (hbOld)
        {
            PatBlt(hdc, prc->left, prc->top,
                prc->right - prc->left,
                prc->bottom - prc->top,
                PATCOPY);
            
            SelectObject(hdc, hbOld);
        }
        
        DeleteObject(hbDraw);
    }
}

int CVolumeGeneralPage::OnInitDialog(HWND)
/*++
  Routine Descriptor:
    Handles initializing a volume general property page.
    We need to initialize each of the controls on the dialog
    with the correct data.

  Arguments:
    HWND is the default control, we don't need it here.
--*/
{
    
    HWND  hWndCtrl;
    char  szFormatBuffer[1024];

    //
    //  Update the drive letter information
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_DRV_LETTER1);
    if(hWndCtrl) SetWindowTextA(hWndCtrl, m_szDriveDescription);

    //
    //  Update the drive type.  (For now it is always
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_DRV_TYPE);
    if(hWndCtrl)
    {
        LoadStringA(_Module.GetModuleInstance(), m_uVolumeType, szFormatBuffer, sizeof(szFormatBuffer));
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }
    
    //
    //  Update the Used Bytes.
    //
    ULONGLONG ullUsedSpace = m_ullTotalSpace-m_ullFreeSpace;
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_DRV_USEDBYTES);
    if(hWndCtrl)
    {
        FormatUtils::FileSizeBytes(ullUsedSpace,szFormatBuffer);
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    //
    // Update Used Megabyes or GigaBytes
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_DRV_USEDMB);
    if(hWndCtrl)
    {
        FormatUtils::FileSize(ullUsedSpace, szFormatBuffer);
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    //
    //  Update the Free Bytes.
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_DRV_FREEBYTES);
    if(hWndCtrl)
    {
        FormatUtils::FileSizeBytes(m_ullFreeSpace,szFormatBuffer);
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    //
    // Update Free Megabyes or GigaBytes
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_DRV_FREEMB);
    if(hWndCtrl)
    {
        FormatUtils::FileSize(m_ullFreeSpace, szFormatBuffer);
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    
    //
    //  Update the Total Bytes.
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_DRV_TOTBYTES);
    if(hWndCtrl)
    {
        FormatUtils::FileSizeBytes(m_ullTotalSpace, szFormatBuffer);
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    //
    // Update Total Megabyes or GigaBytes
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_DRV_TOTMB);
    if(hWndCtrl)
    {
        FormatUtils::FileSize(m_ullTotalSpace,szFormatBuffer);
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    return 1;
}

const COLORREF c_crPieColors[] =
{
    RGB(  0,   0, 255),      // Blue
    RGB(255,   0, 255),      // Red-Blue
    RGB(  0,   0, 128),      // 1/2 Blue
    RGB(128,   0, 128),      // 1/2 Red-Blue
};

INT_PTR CVolumeGeneralPage::OnDrawItem(UINT uItem, LPDRAWITEMSTRUCT pDrawItem)
{
    switch(uItem)
    {
        case IDC_DRV_PIE:
         {
           DWORD dwPctX10 = m_ullTotalSpace ?
                (DWORD)((__int64)1000 * (m_ullTotalSpace - m_ullFreeSpace) / m_ullTotalSpace) : 
                1000;

            if(!m_dwPieShadowHgt)
            {
                SIZE size;
                GetTextExtentPoint(pDrawItem->hDC, TEXT("W"), 1, &size);
                m_dwPieShadowHgt = size.cy * 2/3;
            }

            DrawPie(
                pDrawItem->hDC,
                &pDrawItem->rcItem,
                dwPctX10,
                m_ullFreeSpace==0 || m_ullFreeSpace==m_ullTotalSpace,
                m_dwPieShadowHgt,
                c_crPieColors);
         }
         break;
        case IDC_DRV_USEDCOLOR:
         DrawColorRect(pDrawItem->hDC, c_crPieColors[DP_USEDCOLOR], &pDrawItem->rcItem);
         break;
        case IDC_DRV_FREECOLOR:
         DrawColorRect(pDrawItem->hDC, c_crPieColors[DP_FREECOLOR], &pDrawItem->rcItem);
         break;
        default:
         return 0;
    }
    return 1;
}

CFSItemGeneralPage::CFSItemGeneralPage(
    CXboxFolder *pSelection,
    IXboxFileOrDirVisit *pFileOrDir,
    UINT uItemCount
    ) : m_pSelection(NULL), m_fNameChanged(false),
        m_fReadOnlyChanged(false), m_fHiddenChanged(false),
        m_hThread(NULL)
{
    
    char  szLocationTemp[MAX_XBOX_PATH];
    LPSTR pszParse;
    CGetAttributes getAttributes(&m_MultiFileAttributes);

    //
    //  Get the location
    //
    pSelection->GetPath(szLocationTemp);

    //
    //  uItemCount==0 is a special case, the selection
    //  is not a child of the CXboxFolder, but it is the
    //  the CXboxFolder itself, so we get the location
    //  by stripping one path element off the end.
    //

    if(0==uItemCount)
    {
      pszParse = strrchr(szLocationTemp, '\\');
      *pszParse = '\0';
    }
    
    //
    //  Replace the '\\' after the console name.
    //  Get pszParse to point to the rest of the
    //  location.
    //
    pszParse = strchr(szLocationTemp, '\\');
    *pszParse++ = '\0';
    wsprintfA(m_szLocation, "%c:%s(On \'%s\')", *pszParse, pszParse+1, szLocationTemp);
        
    //
    //  Now determine the dialog to display (FILE, FOLDER, MULTIFILE)
    //
    pFileOrDir->GetName(m_szNameBuffer);
    if(uItemCount < 2)
    {
        getAttributes.VisitFileOrDir(pFileOrDir, NULL);
        if(m_MultiFileAttributes.dwAttributes&FILE_ATTRIBUTE_DIRECTORY)
        {
            m_uTemplateResource = IDD_FOLDERPROP;
            m_MultiFileAttributes.dwFolderCount--;
        } else
        {
            m_uTemplateResource = IDD_FILEPROP;
        }
    } else
    {
        pSelection->VisitEach(&getAttributes, IXboxVisitor::FlagContinue);
        m_uTemplateResource = IDD_FILEMULTPROP;
    }

    //
    //  Clone the selection, for the thread proc
    //
    pSelection->Clone(&m_pSelection);
}

DWORD CFSItemGeneralPage::ThreadProc()
/*++
  Routine Description.
    Visits the selection recursively to get a full count of files \ folders and the total size.
--*/
{
    XBOX_MULTIFILE_ATTRIBUTES multiFileAttributes;
    CGetAttributes getAttributes(&multiFileAttributes, false, GetAttributesCallback, (PVOID)this);
    m_pSelection->VisitEach(&getAttributes, IXboxVisitor::FlagRecurse|IXboxVisitor::FlagContinue);
    return 0;
}

bool CFSItemGeneralPage::GetAttributesCallback(PVOID pvThis, XBOX_MULTIFILE_ATTRIBUTES *pMultiFileAttributes)
{
    CFSItemGeneralPage *pThis = (CFSItemGeneralPage *)pvThis;
    if(pThis->m_fKillThread) return false;

    if(pMultiFileAttributes->ullTotalSize > pThis->m_MultiFileAttributes.ullTotalSize)
    {
        pThis->m_MultiFileAttributes.ullTotalSize = pMultiFileAttributes->ullTotalSize;
    }
    if(pMultiFileAttributes->dwFileCount > pThis->m_MultiFileAttributes.dwFileCount)
    {
        pThis->m_MultiFileAttributes.dwFileCount = pMultiFileAttributes->dwFileCount;
    }
    DWORD dwFolderCount = pMultiFileAttributes->dwFolderCount;
    if(IDD_FOLDERPROP == pThis->m_uTemplateResource) dwFolderCount--;
    if(dwFolderCount > pThis->m_MultiFileAttributes.dwFolderCount)
    {
        pThis->m_MultiFileAttributes.dwFolderCount = dwFolderCount;
    }

    //
    //  Tell the filesystem page to update itself.
    //

    PostMessage(pThis->m_hPropDlg, WM_PROPPAGE_REFRESH_CONTENTS, 0, 0);
    return true;
}


void CFSItemGeneralPage::SynchronizeUpdateThread()
{
    if(m_hThread)
    {
        m_fKillThread = true;
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}
BOOL CFSItemGeneralPage::GetCaption(LPSTR szBuffer, ULONG)
/*++
  Routine Description:
    Returns the name of the first item.  If there is more than one appends
    ",..." to the name.
--*/
{
    strcpy(szBuffer, m_szNameBuffer);
    return TRUE;
}

INT_PTR CFSItemGeneralPage::OnInitDialog(HWND hwndDefaultControl)
{

    HWND hWndCtrl;
    char szFormatBuffer[MAX_PATH];

    //
    //  Initialize the Name Field if it has one
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_NAMEEDIT);
    if(hWndCtrl)
    {
        SetWindowTextA(hWndCtrl, m_szNameBuffer);
    }

    //
    //  Initialize the Icon Field if it has one.
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_ITEMICON);
    if(hWndCtrl)
    {
        //Get the large icon
        DWORD dwRet = 0;
        BOOL fSuccess;
        SHFILEINFOA ShellFileInfo;
        fSuccess = SHGetFileInfoA(
                    m_szNameBuffer,
                    0,
                    &ShellFileInfo,
                    sizeof(ShellFileInfo),
                    SHGFI_USEFILEATTRIBUTES|SHGFI_ICON|SHGFI_LARGEICON 
                    );
        if(fSuccess)
        {
            WindowUtils::ReplaceWindowIcon(hWndCtrl, ShellFileInfo.hIcon);
        }
    }

    //
    //  Set the file type.
    //

    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_FILETYPE);
    if(hWndCtrl)
    {
        SetWindowTextA(hWndCtrl, m_MultiFileAttributes.szTypeName);
    }

    //
    //  File location - basically get the fullname of the first
    //  item, and lop off the last filename.
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_LOCATION);
    if(hWndCtrl)
    {
        SetWindowTextA(hWndCtrl, m_szLocation);
    }
    
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_FILESIZE);
    if(hWndCtrl)
    {
        FormatUtils::FileSize(m_MultiFileAttributes.ullTotalSize, szFormatBuffer);
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_CONTAINS);
    if(hWndCtrl)
    {
        WindowUtils::rsprintf(
            szFormatBuffer,
            IDS_CONTAINS_FORMAT,
            m_MultiFileAttributes.dwFileCount,
            m_MultiFileAttributes.dwFolderCount
            );
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }
    
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_CREATED);
    if(hWndCtrl)
    {
        FormatUtils::FileTime(
            &m_MultiFileAttributes.CreationTime,
            szFormatBuffer,
            DATE_LONGDATE
            );
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }

    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_LASTMODIFIED);
    if(hWndCtrl)
    {
        FormatUtils::FileTime(
            &m_MultiFileAttributes.ChangeTime,
            szFormatBuffer,
            DATE_LONGDATE
            );
        SetWindowTextA(hWndCtrl, szFormatBuffer);
    }
    

    //
    //  Set the readonly checkbox
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_READONLY);
    if(hWndCtrl)
    {
       WPARAM buttonState;
       if(m_MultiFileAttributes.dwValidAttributes&FILE_ATTRIBUTE_READONLY)
       {
         if(m_MultiFileAttributes.dwAttributes&FILE_ATTRIBUTE_READONLY)
         {
            buttonState = BST_CHECKED;
         } else
         {
            buttonState = BST_UNCHECKED; 
         }
       } else
       {
           buttonState = BST_INDETERMINATE;
           
       }
       SendMessage(hWndCtrl, BM_SETCHECK, buttonState, 0);
    }

    //
    //  Set the hidden checkbox
    //
    hWndCtrl = GetDlgItem(m_hPropDlg, IDC_HIDDEN);
    if(hWndCtrl)
    {
       WPARAM buttonState;
       if(m_MultiFileAttributes.dwValidAttributes&FILE_ATTRIBUTE_HIDDEN)
       {
         if(m_MultiFileAttributes.dwAttributes&FILE_ATTRIBUTE_HIDDEN)
         {
            buttonState = BST_CHECKED;
         } else
         {
            buttonState = BST_UNCHECKED;
         }
       } else
       {
            buttonState = BST_INDETERMINATE;
       }
       SendMessage(hWndCtrl, BM_SETCHECK, buttonState, 0);
    }
    //
    //  Then name cannot have changed yet.
    //
    m_fNameChanged = false;
    m_fReadOnlyChanged = false;
    m_fHiddenChanged = false;

    //
    //  Start the recursive update.
    //

    _ASSERTE(m_pSelection);

    if(IDD_FILEPROP != m_uTemplateResource)
    {
        DWORD dwThreadId;
        m_fKillThread = false;
        m_hThread = CreateThread(NULL, 0, CFSItemGeneralPage::StartThreadProc, this, 0, &dwThreadId);
    }

    return 1;
}

void CFSItemGeneralPage::CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage)
/*++
  Routine Description:
    We need to complete the PROPSHEETPAGE structure.  The CPropertyPage
    has already done as much as it could.  Basically, we need to choose
    the template.  There are three possibilities:
    1) We represent one and only one file.  Use IDD_FILEPROP.
    2) We represent one and only one folder.  Use IDD_FOLDERPROP.
    3) We represent two or more items, files and\or folders.  Use IDD_FILEMULTPROP.
--*/
{
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(m_uTemplateResource);
}


INT_PTR CFSItemGeneralPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);

    //
    //  If READ ONLY or HIDDEN IS CHANGED, just tell our parent
    //  to enable the Apply Button.
    //
    if ( uCommand == BN_CLICKED) 
    { 
        if(uControlId==IDC_READONLY)
        {
           m_fReadOnlyChanged = true;
           SendMessage(GetParent(m_hPropDlg), PSM_CHANGED, (WPARAM)m_hPropDlg, 0); 
           return TRUE;
        }
        if(uControlId==IDC_HIDDEN)
        {
           m_fHiddenChanged = true;
           SendMessage(GetParent(m_hPropDlg), PSM_CHANGED, (WPARAM)m_hPropDlg, 0); 
           return TRUE;
        }
    }
    
    if (uCommand == EN_CHANGE)
    {
        if(uControlId == IDC_NAMEEDIT)
        {
           m_fNameChanged = true;
           return TRUE;
        }
    }
    
    return  FALSE;
}


INT_PTR CFSItemGeneralPage::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char szFormatBuffer[MAX_PATH];
    if(uMsg==WM_PROPPAGE_REFRESH_CONTENTS)
    {
        HWND hWndCtrl = GetDlgItem(m_hPropDlg, IDC_FILESIZE);
        if(hWndCtrl)
        {
            FormatUtils::FileSize(m_MultiFileAttributes.ullTotalSize, szFormatBuffer);
            SetWindowTextA(hWndCtrl, szFormatBuffer);
        }

        hWndCtrl = GetDlgItem(m_hPropDlg, IDC_CONTAINS);
        if(hWndCtrl)
        {
            WindowUtils::rsprintf(
                szFormatBuffer,
                IDS_CONTAINS_FORMAT,
                m_MultiFileAttributes.dwFileCount,
                m_MultiFileAttributes.dwFolderCount
                );
            SetWindowTextA(hWndCtrl, szFormatBuffer);
        }
        return TRUE;
    } else if(WM_NOTIFY==uMsg)
    {
        LPPSHNOTIFY pPropSheetNotify = (LPPSHNOTIFY)lParam;
        if(PSN_QUERYCANCEL == pPropSheetNotify->hdr.code)
        {
            SynchronizeUpdateThread();
        }
        return TRUE;
    }
    return FALSE;
}


class CSetName : public IXboxVisitor
{
 public:
  CSetName(LPSTR pszNewName, HWND hWnd): m_pszNewName(pszNewName), m_hWnd(hWnd){}
  void VisitRoot(IXboxVisit *pRoot, DWORD *pdwFlags){_ASSERTE(FALSE);}
  void VisitAddConsole(IXboxVisit *pAddConsole, DWORD *pdwFlags){_ASSERTE(FALSE);}
  void VisitConsole(IXboxConsoleVisit *pConsole, DWORD *pdwFlags){_ASSERTE(FALSE);}
  void VisitVolume(IXboxVolumeVisit *pVolume, DWORD *pdwFlags){_ASSERTE(FALSE);}
  void VisitDirectoryPost (IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags) {_ASSERTE(FALSE);}
  
  void VisitFileOrDir(IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags)
  {
        m_hr = pFileOrDir->SetName(m_hWnd, m_pszNewName);
  }
  HRESULT m_hr;
  LPSTR   m_pszNewName;
  HWND    m_hWnd;
};

bool CFSItemGeneralPage::OnApply(bool fClose)
{
    
    HWND  hWndCtrl;
    DWORD dwSetAttributes = 0;
    DWORD dwClearAttributes = 0;
    bool  fRet = true;

    SynchronizeUpdateThread();

    //
    //  Retrieve the state of the read-only and hidden check-boxes
    //
    if(m_fReadOnlyChanged)
    {
        LONG lState = SendDlgItemMessage(m_hPropDlg, IDC_READONLY, BM_GETSTATE, 0, 0);
        switch(lState)
        {
           case BST_CHECKED:
              dwSetAttributes |= FILE_ATTRIBUTE_READONLY;
              break;
           case BST_UNCHECKED:
              dwClearAttributes |= FILE_ATTRIBUTE_READONLY;
              break;
           case BST_INDETERMINATE:
           default:
              break;
        }
    }

    if(m_fHiddenChanged)
    {
        LONG lState = SendDlgItemMessage(m_hPropDlg, IDC_HIDDEN, BM_GETSTATE, 0, 0);
        switch(lState)
        {
           case BST_CHECKED:
              dwSetAttributes |= FILE_ATTRIBUTE_HIDDEN;
              break;
           case BST_UNCHECKED:
              dwClearAttributes |= FILE_ATTRIBUTE_HIDDEN;
              break;
           case BST_INDETERMINATE:
           default:
              break;
        }
    }

    if(dwSetAttributes || dwClearAttributes)
    {
       bool fUpdateAttributes = true;
       DWORD dwVisitorFlags = IXboxVisitor::FlagContinue;

       if( 
            ((IDD_FOLDERPROP == m_uTemplateResource) && (m_MultiFileAttributes.dwFolderCount || m_MultiFileAttributes.dwFileCount)) ||
            ((IDD_FILEMULTPROP == m_uTemplateResource) && m_MultiFileAttributes.dwFolderCount)
        )
        {
            
            UINT uRet = Dialog::ConfirmSetAttributes(m_hPropDlg,dwSetAttributes,dwClearAttributes, (IDD_FILEMULTPROP == m_uTemplateResource));
            if(IDCANCEL==uRet)
            {
                fUpdateAttributes = false;
            } else if(IDC_XB_YESTOALL==uRet)
            {
                dwVisitorFlags |= IXboxVisitor::FlagRecurse;
            }
       }

       if(fUpdateAttributes)
       {
            CSetAttributes setAttributes(dwSetAttributes, dwClearAttributes);
            _ASSERTE(m_pSelection);
            m_pSelection->VisitEach(&setAttributes, dwVisitorFlags);
            m_fReadOnlyChanged = false;
            m_fHiddenChanged = false;
       }
    }

    if(fClose && m_fNameChanged)
    {
        m_fNameChanged = false;
        hWndCtrl = GetDlgItem(m_hPropDlg, IDC_NAMEEDIT);
        if(hWndCtrl)
        {
            CHAR szNewName[MAX_PATH];
            GetWindowTextA(hWndCtrl, szNewName, sizeof(szNewName));
            if(strcmp(m_szNameBuffer, szNewName))
            {
                
                CSetName setName(szNewName, m_hPropDlg);
                m_pSelection->VisitEach(&setName, IXboxVisitor::FlagContinue);
                if(FAILED(setName.m_hr))
                {
                    char szError[60];
                    FormatUtils::XboxErrorString(setName.m_hr, szError, sizeof(szError));
                    WindowUtils::MessageBoxResource(m_hPropDlg, IDS_RENAME_XBDM_ERROR, IDS_RENAME_ERROR_CAPTION, MB_OK|MB_ICONSTOP, szError);
                }
            }
        }
    }
    return fRet;
}


//-----------------------------------------------------------
//  General Page for Consoles
//-----------------------------------------------------------
CConsoleGeneralPage::CConsoleGeneralPage(
           IXboxConnection *pConnection,
           LPCSTR           pszConsoleName,
           BOOL             fMultipleConsoles
           ) : m_pConnection(pConnection), m_fMultipleConsoles(fMultipleConsoles)
{
    _ASSERT(m_pConnection);
    m_pConnection->AddRef();
    strcpy(m_szConsoleName, pszConsoleName);
}

BOOL
CConsoleGeneralPage::GetCaption(
    LPSTR szBuffer,
    ULONG ccBuffer
    )
{
    strncpy(szBuffer, m_szConsoleName, ccBuffer);
    return TRUE;

}

INT_PTR
CConsoleGeneralPage::OnInitDialog(
    HWND hwndDefaultControl
    )
{
    DWORD dwIpAddress;
    DWORD dwSize;
    DM_XBE dmXbe;
    char  szFormatBuffer[MAX_PATH];
    SYSTEMTIME sysTime;
    HRESULT hr;

    dwSize = sizeof(szFormatBuffer);
    if(SUCCEEDED(m_pConnection->HrGetNameOfXbox(szFormatBuffer, &dwSize, FALSE)))
    {
        SetWindowTextA(GetDlgItem(m_hPropDlg, IDC_NAMEEDIT), szFormatBuffer);
    }

    //Check if we need to replace the icon for the default console.
    CManageConsoles manageConsole;
    if(manageConsole.IsDefault(m_szConsoleName))
    {
        HICON hDefaultConsole = LoadIcon(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_CONSOLE_DEFAULT));
        if(hDefaultConsole)
            WindowUtils::ReplaceWindowIcon(GetDlgItem(m_hPropDlg, IDC_ITEMICON), hDefaultConsole);
    }

    if(SUCCEEDED(m_pConnection->HrResolveXboxName(&dwIpAddress)))
    {
        wsprintfA(szFormatBuffer, "%d.%d.%d.%d", (dwIpAddress >> 24)&0xFF, (dwIpAddress >> 16)&0xFF, (dwIpAddress >> 8)&0xFF, dwIpAddress&0xFF);
    } else
    {
        LoadStringA(_Module.GetModuleInstance(), IDS_TITLE_NOT_AVAILABLE, szFormatBuffer, sizeof(szFormatBuffer));
    }
    SetWindowTextA(GetDlgItem(m_hPropDlg, IDC_IPADDRESS), szFormatBuffer);

    if(SUCCEEDED(m_pConnection->HrGetAltAddress(&dwIpAddress)))
    {
        wsprintfA(szFormatBuffer, "%d.%d.%d.%d", (dwIpAddress >> 24)&0xFF, (dwIpAddress >> 16)&0xFF, (dwIpAddress >> 8)&0xFF, dwIpAddress&0xFF);
        SetWindowTextA(GetDlgItem(m_hPropDlg, IDC_ALTIPADDRESS), szFormatBuffer);
    } else
    {
        ShowWindow(GetDlgItem(m_hPropDlg, IDC_ALTIPADDRESS), SW_HIDE);
        ShowWindow(GetDlgItem(m_hPropDlg, IDC_ALTIPADDRESS_TEXT), SW_HIDE);
    }

    hr = m_pConnection->HrGetXbeInfo(NULL, &dmXbe);
    if(XBDM_NOSUCHFILE == hr)
    {
        LoadStringA(_Module.GetModuleInstance(), IDS_DEFAULT_TITLE, dmXbe.LaunchPath, sizeof(dmXbe.LaunchPath));
    } else if(FAILED(hr))
    {
        LoadStringA(_Module.GetModuleInstance(), IDS_TITLE_NOT_AVAILABLE, dmXbe.LaunchPath, sizeof(dmXbe.LaunchPath));
    }
    HWND hRunningTitle = GetDlgItem(m_hPropDlg, IDC_RUNNINGTITLE);
    SetWindowTextA(hRunningTitle, dmXbe.LaunchPath);
    
    //Uggh, the launch path is often longer than the field.  The user can tab to it, and use the HSCROLL capability
    //to see the whole thing.  The end of the path is usually more interesting than the beginning, so we help the user
    //out by scrolling it.
    dwSize = strlen(dmXbe.LaunchPath);
    SendMessage(hRunningTitle, EM_SETSEL, dwSize, dwSize);

    return 1;
}

void 
CConsoleGeneralPage::CompletePropSheetPage(
    PROPSHEETPAGEA *pPropSheetPage
    )
{
     //
    //  The client will have passed in m_fUseDriveOnTabName on construction.
    //  This tells us that we need to override the "General" tab name, with
    //  the drive description.  This feature is for the case where multiple
    //  drives are opened in a single property sheet.
    //
    
    if(m_fMultipleConsoles)
    {
        pPropSheetPage->dwFlags |= PSP_USETITLE;
        pPropSheetPage->pszTitle = m_szConsoleName;
    }
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_CONSOLE_GENERAL);
}

//-----------------------------------------------------------
//  Security Page for Consoles
//-----------------------------------------------------------
CConsoleSecurityPage::CConsoleSecurityPage(
    IXboxConnection *pConnection, LPCSTR pszConsoleName
    ) : m_pConnection(pConnection), m_dwAccess(0),
        m_fLocked(FALSE),  m_fManageMode(FALSE),
        m_fSecureMode(FALSE), m_pUserAccessChangeList(NULL),
        m_iLastSelected(0), m_fUpdatingUI(FALSE)
{
    _ASSERT(m_pConnection);
    m_pConnection->AddRef();
    strcpy(m_szConsoleName, pszConsoleName);
}

BOOL
CConsoleSecurityPage::GetCaption(
    LPSTR szBuffer,
    ULONG ccBuffer
    )
{
    *szBuffer = '\0';
    return TRUE;
}

HRESULT CConsoleSecurityPage::InitSecuritySupport()
/*++
  Routine Description:  Determine if the box supports the security.  If so, figure out if the box
  is locked.  If it is locked determine if the current user has manage mode.  Then initialize all
  the data strucures as appropriate.
--*/
{
    HRESULT hr;


    //
    //  Is the box locked?
    //
    
    hr = m_pConnection->HrIsSecurityEnabled(&m_fLocked);
    if(FAILED(hr))
    {
        return hr;    
    }

    //
    //  If the box is not locked, check to see if it supports security
    //

    if(!m_fLocked)
    {
        //
        // Does this box support security?  Basically, send a security command supported in November to the
        // box, if it fails with XBDM_INVALIDCMD, then it doesn't support security.  You have to choose it
        // carefully.  HrGetUserAccess will work, but make sure you pass 
        //
        char szBuffer[255];
        DWORD dwSize = 255;
        hr = m_pConnection->HrSendCommand("GETUSERPRIV NAME=BOGUS", szBuffer, &dwSize);
        if(XBDM_INVALIDCMD == hr)
            return S_FALSE;

    } else    
    {
        //
        //  Find out what access we have.
        //  (Prior to build 4114 this didn't work, you got your
        //   machine name and asked about it.  However, this only
        //   worked if you manage permissions. So we try the new
        //   real way and then fall back to the old way - in which
        //   case you will see that you have no access, if you
        //   don't have manage.)
        //
        hr = m_pConnection->HrGetUserAccess(NULL, &m_dwAccess);
        if(XBDM_UNDEFINED == hr)
        {
            char szComputerName[MAX_COMPUTERNAME_LENGTH+1];
            DWORD dwComputerNameLength = sizeof(szComputerName);
            GetComputerNameA(szComputerName, &dwComputerNameLength);
            hr = m_pConnection->HrGetUserAccess(szComputerName, &m_dwAccess);
        }
        if(FAILED(hr))
        {
            m_dwAccess = 0;
        }
        if(m_dwAccess&DMPL_PRIV_MANAGE)
        {
            m_fManageMode  = TRUE;
            InitUserList();
        }
    }
    return S_OK;
}

HRESULT CConsoleSecurityPage::InitUserList()
{
    // Populate the user list.
    PDM_WALK_USERS      pdmWalkUsersList;
    DWORD               dwUserCount;
    HRESULT             hr;
    USER_ACCESS_CHANGE *pNewUser;
    
    hr=m_pConnection->HrOpenUserList(&pdmWalkUsersList, &dwUserCount);
    if(SUCCEEDED(hr))
    {
        while(dwUserCount-- && SUCCEEDED(hr))
        {
            pNewUser = new USER_ACCESS_CHANGE;
            if(pNewUser)
            {
                pNewUser->dwFlags = 0;
                hr = m_pConnection->HrWalkUserList(&pdmWalkUsersList, &pNewUser->dmUser);
                if(SUCCEEDED(hr))
                {
                    pNewUser->dwNewAccess = pNewUser->dmUser.AccessPrivileges;
                    pNewUser->pNextUser = m_pUserAccessChangeList;
                    m_pUserAccessChangeList = pNewUser;
                } else
                {
                    delete pNewUser;
                }
            } else
            {
                hr = E_OUTOFMEMORY;
            }
        }
        m_pConnection->HrCloseUserList(pdmWalkUsersList);
    }
    return hr;
}


INT_PTR 
CConsoleSecurityPage::OnInitDialog(
    HWND hwndDefaultControl
    )
{
    
    //  Cache the handles of the commonly access children
    m_hWndAccess = GetDlgItem(m_hPropDlg, IDC_SECURITY_ACCESS); 
    m_hWndUsers = GetDlgItem(m_hPropDlg, IDC_SECURITY_USERLIST);

    // Add the name column for report view.
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.pszText =  L"";
    lvc.cx = 175;
    lvc.iSubItem = 0;
    ListView_InsertColumn(m_hWndAccess, 0, &lvc);
    ListView_InsertColumn(m_hWndUsers, 0, &lvc);
    ListView_SetExtendedListViewStyle(m_hWndAccess, LVS_EX_CHECKBOXES);

    // Show and Hide the appropriate Windows, based on the
    ShowHideWindows();
    UpdateData();

    return OnSetActive();
}

INT_PTR CConsoleSecurityPage::OnSetActive()
{
    return 1;
}

void CConsoleSecurityPage::ShowHideWindows()
{
    //
    //  Turn the m_fLocked and m_fManageMode flags into
    //  show flags for the windows to hide or show.
    //
    int iLockedShow   = m_fLocked ? SW_SHOW : SW_HIDE;
    int iUnlockShow   = m_fLocked ? SW_HIDE : SW_SHOW;
    int iManageShow   = m_fManageMode ? SW_SHOW : SW_HIDE;
    int iNoManageShow = (m_fLocked && !m_fManageMode) ? SW_SHOW : SW_HIDE;
    
    // Items Visible only when unlocked
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_UNLOCKED_TEXT), iUnlockShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_LOCK_BUTTON),   iUnlockShow);
    
    // Items Visible if Locked (regardless of manage mode versus not manage mode
    ShowWindow(m_hWndAccess, iLockedShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_ACCESS_TEXT), iLockedShow);
    
    // Items Visible only in managed mode.
    ShowWindow(m_hWndUsers, iManageShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_MACHINES),        iManageShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_ADD),             iManageShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_REMOVE),          iManageShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_UNLOCK),          iManageShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_CHANGE_PASSWORD), iManageShow);

    // Hide the password entry
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_MANAGE_TEXT),     iNoManageShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_PASSWORD_TEXT),   iNoManageShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_PASSWORD_EDIT),   iNoManageShow);
    ShowWindow(GetDlgItem(m_hPropDlg, IDC_SECURITY_MANAGE_BUTTON),   iNoManageShow);
}

INT_PTR
CConsoleSecurityPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);

    //
    //  Look for button presses that require action
    //
    if (uCommand == BN_CLICKED) 
    { 
        switch(uControlId)
        {
            case IDC_SECURITY_ADD:
                _ASSERTE(m_fLocked && m_fManageMode);
                AddUser();
                break;
            case IDC_SECURITY_REMOVE:
                _ASSERTE(m_fLocked && m_fManageMode);
                RemoveUser();
                break;
            case IDC_SECURITY_UNLOCK:
                _ASSERTE(m_fLocked && m_fManageMode);
                UnLockConsole();
                break;
            case IDC_SECURITY_CHANGE_PASSWORD:
                _ASSERTE(m_fLocked && m_fManageMode);
                ChangePassword();
                break;
            case IDC_SECURITY_LOCK_BUTTON:
                _ASSERTE(!m_fLocked);
                LockConsole();
                break;
            case IDC_SECURITY_MANAGE_BUTTON:
                _ASSERTE(m_fLocked && !m_fManageMode);
                StartSecureMode();
                break;
            default:
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

INT_PTR
CConsoleSecurityPage::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
    if(WM_NOTIFY==uMsg)
    {
        //Most of the paths below require that we now the currently
        //selected USER_ACCESS_CHANGE item
        INT iSelectedUser = ListView_GetNextItem(m_hWndUsers, -1, LVNI_ALL | LVNI_SELECTED);
        LVITEMA lvItem;
        memset(&lvItem, 0, sizeof(lvItem));
        lvItem.iItem = iSelectedUser;
        lvItem.iSubItem = 0;
        lvItem.mask = LVIF_PARAM;
        SendMessage(m_hWndUsers, LVM_GETITEMA, 0, (LPARAM)&lvItem);
        USER_ACCESS_CHANGE *pUserChangeItem = (USER_ACCESS_CHANGE *)lvItem.lParam;

        LPNMHDR pnmhdr = (LPNMHDR)lParam;
        if(pnmhdr->idFrom == IDC_SECURITY_ACCESS)
        {
            if(!m_fUpdatingUI) //Prevent us from changing our flags, because we are initializing things.
            {
                if(pnmhdr->code == LVN_ITEMCHANGED)
                {
                    NMLISTVIEW *pListView = (NMLISTVIEW *)pnmhdr;
                    DWORD dwAccessBit = (DWORD)pListView->lParam;
                    if(pUserChangeItem)
                    {
                        //Read the checked state, use it to update the access
                        //of the currently selected item.
                        if(ListView_GetCheckState(m_hWndAccess, pListView->iItem))
                        {
                            pUserChangeItem->dwNewAccess |= dwAccessBit;
                        } else
                        {
                            pUserChangeItem->dwNewAccess &= ~dwAccessBit;
                        }
                        SetApplyButton();
                      }
                }
            }
        } else if(pnmhdr->idFrom == IDC_SECURITY_USERLIST)
        {
            if(NM_CLICK==pnmhdr->code)
            {
                if(-1 == iSelectedUser)
                {
                    if(m_pUserAccessChangeList)
                    {
                        iSelectedUser = m_iLastSelected;
                        ListView_SetItemState(m_hWndUsers, iSelectedUser, LVIS_SELECTED, LVIS_SELECTED);
                    }
                } else
                {
                    m_iLastSelected = iSelectedUser;
                    if(pUserChangeItem)
                    {
                        // Update the Access Info based on the new selection
                        UpdateAccessInfo(pUserChangeItem->dmUser.UserName, pUserChangeItem->dwNewAccess);
                        return TRUE;
                    }
                }
            }
        }
    }
    return 0;
}

void 
CConsoleSecurityPage::CompletePropSheetPage(
    PROPSHEETPAGEA *pPropSheetPage
    )
{
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_CONSOLE_SECURITY);
}

void CConsoleSecurityPage::AddUser()
{
    char szUserName[255];
    int  iItem;
    if(IDOK==Dialog::PromptUserName(m_hPropDlg, szUserName, sizeof(szUserName)))
    {
        // Look to see if the user is in our list
        USER_ACCESS_CHANGE *pNewUser;
        pNewUser = m_pUserAccessChangeList;
        while(pNewUser)
        {
            if(0==_stricmp(pNewUser->dmUser.UserName, szUserName))
            {
                // Found a match, this user is already in our list.

                // See if it is marked remove, this means that it does
                // not appear in the UI, as it will be removed when changes
                // are applied.  In this case, we clear the remove flag
                // and reset the permissions, then we have to add
                if(pNewUser->dwFlags & UACF_REMOVE)
                {
                    pNewUser->dwFlags &= ~UACF_REMOVE;
                    break;
                } else
                {
                    // The user is already in the list, and displayed in the UI.  So this is user error.
                    // What to do?  We could a) show a message telling the user that they are blind and
                    // that the machine is already in the list, or b) we could just shift focus to the
                    // machine in the list, or c) do both a) and b).  I like b) for now.
                    LVFINDINFO findInfo;
                    findInfo.flags = LVFI_PARAM;
                    findInfo.lParam = (LPARAM)pNewUser;
                    iItem = SendMessage(m_hWndUsers, LVM_FINDITEM, -1, (LPARAM)&findInfo);
                    _ASSERT(-1 != iItem);  //Indicates that are logic screwed up somewhere.
                    ListView_SetItemState(m_hWndUsers, iItem, LVIS_SELECTED, LVIS_SELECTED);
                    m_iLastSelected = iItem;
                    UpdateAccessInfo(pNewUser->dmUser.UserName, pNewUser->dwNewAccess);
                    //we are done, go home
                    return;
                }
            }
            pNewUser = pNewUser->pNextUser;
        }

        // We didn't have the item in our list yet.
        if(!pNewUser)
        {
            pNewUser = new USER_ACCESS_CHANGE;
            if(!pNewUser)
            {
                WindowUtils::MessageBoxResource(m_hPropDlg, IDS_ERROR_LOW_MEMORY, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP);
                return;
            }
            pNewUser->dwFlags = UACF_ADD;
            strcpy(pNewUser->dmUser.UserName, szUserName);
            pNewUser->dmUser.AccessPrivileges = 0;
            pNewUser->dwNewAccess = DMPL_PRIV_INITIAL;
            pNewUser->pNextUser = m_pUserAccessChangeList;
            m_pUserAccessChangeList = pNewUser;
        }

        // We get here only if the user was in the list, but marked UACF_REMOVE, or if we just
        // added it to the list.  Either way, it is not in the list view, so we should add it.
        LVITEMA lvItem;
        memset(&lvItem, 0, sizeof(lvItem));
        lvItem.mask = LVIF_PARAM | LVIF_TEXT;
        lvItem.iItem = -1;
        lvItem.iSubItem = 0;
        lvItem.pszText = pNewUser->dmUser.UserName; 
        lvItem.cchTextMax = sizeof(pNewUser->dmUser.UserName);  
        lvItem.lParam = (LPARAM)pNewUser;
        iItem = SendMessage(m_hWndUsers, LVM_INSERTITEMA, 0, (LPARAM)&lvItem);
        _ASSERT(-1!=iItem);
        //Now that it is added, set focus to it, and update the check marks.
        ListView_SetItemState(m_hWndUsers, iItem, LVIS_SELECTED, LVIS_SELECTED);
        m_iLastSelected = iItem;
        UpdateAccessInfo(pNewUser->dmUser.UserName, pNewUser->dwNewAccess);
        SetApplyButton();
    }
}

void CConsoleSecurityPage::RemoveUser()
{
    USER_ACCESS_CHANGE *pUserChangeItem;
    LVITEMA lvItem;

    //Get the selected user
    INT iSelectedUser = ListView_GetNextItem(m_hWndUsers, -1, LVNI_ALL | LVNI_SELECTED);
    _ASSERTE(-1 != iSelectedUser);

    //Get the Selected item
    memset(&lvItem, 0, sizeof(lvItem));
    lvItem.iItem = iSelectedUser;
    lvItem.iSubItem = 0;
    lvItem.mask = LVIF_PARAM;
    SendMessage(m_hWndUsers, LVM_GETITEMA, 0, (LPARAM)&lvItem);
    pUserChangeItem = (USER_ACCESS_CHANGE *)lvItem.lParam;

    //Remove the item from the list (removing it from the display).
    ListView_DeleteItem(m_hWndUsers, iSelectedUser);
     
    //Mark the item for removal, when apply is hit.
    pUserChangeItem->dwFlags |= UACF_REMOVE;

    // Now set focus, on the new item that took its place.
    int itemCount = ListView_GetItemCount(m_hWndUsers);
    if(itemCount)
    {
        if(iSelectedUser >= itemCount) iSelectedUser--;
        ListView_SetItemState(m_hWndUsers, iSelectedUser, LVIS_SELECTED, LVIS_SELECTED);
        m_iLastSelected = iSelectedUser;

        // Get the newly selected item
        memset(&lvItem, 0, sizeof(lvItem));
        lvItem.iItem = iSelectedUser;
        lvItem.iSubItem = 0;
        lvItem.mask = LVIF_PARAM;
        SendMessage(m_hWndUsers, LVM_GETITEMA, 0, (LPARAM)&lvItem);
        pUserChangeItem = (USER_ACCESS_CHANGE *)lvItem.lParam;
    
        // Update the Access Info based on the new selection
        UpdateAccessInfo(pUserChangeItem->dmUser.UserName, pUserChangeItem->dwNewAccess);
    } else
    {
        UpdateAccessInfo("", 0);
        EnableWindow(m_hWndAccess, FALSE);
    }
    SetApplyButton();
}

void CConsoleSecurityPage::UnLockConsole()
{
    //Show Unlock Warning
    if(
        IDOK==WindowUtils::MessageBoxResource(
                    m_hPropDlg, IDS_SECURITY_UNLOCK_WARNING,
                    IDS_SECURITY_UNLOCK_WARNING_CAPTION, MB_OKCANCEL| MB_ICONINFORMATION,
                    m_szConsoleName)
    ){
        HRESULT hr;

        //Do the unlock.
        hr = m_pConnection->HrEnableSecurity(FALSE);
        if(SUCCEEDED(hr))
        {
            // The box is unlocked - delete the data we've got
            m_fLocked = FALSE;
            m_fManageMode = FALSE;
            m_fSecureMode = FALSE;
            m_dwAccess = 0;
            DeleteUserList();   //Forget about the users
            ShowHideWindows();  //Switch the UI
            UpdateData();       //Update the data (this will delete everything from the list view).
            
            // Now any changes to this page are effectively applied, so tell the propsheet.
            SendMessage(GetParent(m_hPropDlg), PSM_UNCHANGED, (WPARAM)m_hPropDlg, (LPARAM)0);
        } else
        {
            char szError[60];
            FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
            WindowUtils::MessageBoxResource(m_hPropDlg, IDS_COULDNT_UNLOCK_CONSOLE, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP, m_szConsoleName,szError);
        }
    }
}

void CConsoleSecurityPage::ChangePassword()
{
    char szNewPassword[255];
    int  iItem;
    if(IDOK==Dialog::PromptNewPassword(m_hPropDlg, szNewPassword, sizeof(szNewPassword)))
    {
        HRESULT hr = m_pConnection->HrSetAdminPassword(szNewPassword);
        if(SUCCEEDED(hr))
        {
            WindowUtils::MessageBoxResource(m_hPropDlg, IDS_PASSWORD_SET, IDS_PASSWORD_SET_CAPTION, MB_OK, m_szConsoleName);
        } else
        {
            char szError[60];
            FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
            WindowUtils::MessageBoxResource(m_hPropDlg, IDS_COULDNT_SET_PASSWORD, IDS_PASSWORD_SET_CAPTION, MB_OK|MB_ICONSTOP, m_szConsoleName, szError);
        }
    }
}

void CConsoleSecurityPage::LockConsole()
{
    //Show Lock Warning
    if(
        IDOK==WindowUtils::MessageBoxResource(
                    m_hPropDlg, IDS_SECURITY_LOCK_WARNING,
                    IDS_SECURITY_LOCK_WARNING_CAPTION, MB_OKCANCEL| MB_ICONINFORMATION,
                    m_szConsoleName)
    ){
        HRESULT hr;

        //Do the unlock.
        hr = m_pConnection->HrUseSharedConnection(TRUE);
        if(SUCCEEDED(hr))
        {
            hr = m_pConnection->HrEnableSecurity(TRUE);
            if(SUCCEEDED(hr))
            {
                char szComputerName[MAX_COMPUTERNAME_LENGTH+1];
                DWORD dwComputerNameLength = sizeof(szComputerName);
                GetComputerNameA(szComputerName, &dwComputerNameLength);
                hr = m_pConnection->HrAddUser(szComputerName, DMPL_PRIV_ALL);
                if(SUCCEEDED(hr))
                {
                    // The box is locked, we should have manage permission now.
                    m_fLocked = TRUE;
                    m_fManageMode = TRUE;
                    m_fSecureMode = FALSE;
                    m_dwAccess = DMPL_PRIV_ALL;
                    InitUserList();      //Update the data.
                    UpdateData();       //Update the data
                    ShowHideWindows();  //Switch the UI
                }            
                // Now any changes to this page are effectively applied, so tell the propsheet.
                SendMessage(GetParent(m_hPropDlg), PSM_UNCHANGED, (WPARAM)m_hPropDlg, (LPARAM)0);
            } else
            {
                //Try unlocking if we couldn't set a user, for whatever good it will do.
                m_pConnection->HrEnableSecurity(FALSE);
            }
            hr = m_pConnection->HrUseSharedConnection(FALSE);
        }

        if(FAILED(hr))
        {
            char szError[60];
            FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
            WindowUtils::MessageBoxResource(m_hPropDlg, IDS_COULDNT_LOCK_CONSOLE, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP, m_szConsoleName,szError);
        }
    }
}

void CConsoleSecurityPage::StartSecureMode()
{
    char szPassword[MAX_PATH];
    HRESULT hr;
    GetDlgItemTextA(m_hPropDlg, IDC_SECURITY_PASSWORD_EDIT, szPassword, MAX_PATH);
    hr = m_pConnection->HrUseSharedConnection(TRUE);
    if(SUCCEEDED(hr))
    {
        hr = m_pConnection->HrUseSecureConnection(szPassword);
        if(SUCCEEDED(hr))
        {
            m_fSecureMode = TRUE;
            m_fManageMode = TRUE;
            InitUserList();
            ShowHideWindows();
            UpdateData();
        }
    }

    if(FAILED(hr))
    {
        char szErrorString[80];
        FormatUtils::XboxErrorString(hr, szErrorString, sizeof(szErrorString));
        WindowUtils::MessageBoxResource(m_hPropDlg, IDS_SECURE_CONNECTION_FAILED, IDS_GENERIC_CAPTION, MB_ICONSTOP|MB_OK, m_szConsoleName, szErrorString);
    }
}

void CConsoleSecurityPage::DeleteUserList()
{
    while(m_pUserAccessChangeList)
    {
        USER_ACCESS_CHANGE   *pTemp = m_pUserAccessChangeList;
        m_pUserAccessChangeList =  pTemp->pNextUser;
        delete pTemp;
    }
}

static DWORD AccessBitFromResource(int i)
{
    switch(i)
    {
      case IDS_PERMISSION_READ:
          return DMPL_PRIV_READ;
      case IDS_PERMISSION_WRITE:
          return DMPL_PRIV_WRITE;
      case IDS_PERMISSION_MANAGE:
          return DMPL_PRIV_MANAGE;
      case IDS_PERMISSION_CONFIGURE:
          return DMPL_PRIV_CONFIGURE;
      case IDS_PERMISSION_CONTROL:
          return DMPL_PRIV_CONTROL;
    }
    return 0;
}

void CConsoleSecurityPage::UpdateData()
{
    USER_ACCESS_CHANGE *pUserChangeItem;
    LVITEMA lvItem;
    ListView_DeleteAllItems(m_hWndAccess);
    ListView_DeleteAllItems(m_hWndUsers);
    
    int iItem = 0;

    if(m_fLocked)
    {
        if(m_fManageMode)
        {
            char szAccessName[60];
            for(int i=IDS_PERMISSION_READ; i <= IDS_PERMISSION_MANAGE; i++)
            {
                LoadStringA(_Module.GetModuleInstance(), i, szAccessName, sizeof(szAccessName));
                lvItem.mask = LVIF_PARAM | LVIF_TEXT;
                lvItem.iItem = PERMISSION_LISTVIEW_INDEX(i); //Just a big value.
                lvItem.iSubItem = 0;
                lvItem.lParam = AccessBitFromResource(i);
                lvItem.pszText = szAccessName;
                lvItem.cchTextMax = sizeof(pUserChangeItem->dmUser.UserName);
                SendMessage(m_hWndAccess, LVM_INSERTITEMA, 0, (LPARAM)&lvItem);
            }
            ListView_SetItemState(m_hWndAccess, iItem, LVIS_SELECTED, LVIS_SELECTED);
            pUserChangeItem = m_pUserAccessChangeList;
            while(pUserChangeItem)
            {
                if(!(UACF_REMOVE&pUserChangeItem->dwFlags))
                {
                    memset(&lvItem, 0, sizeof(lvItem));
                    lvItem.mask = LVIF_PARAM | LVIF_TEXT;
                    lvItem.iItem = iItem++;
                    lvItem.iSubItem = 0;
                    lvItem.pszText = pUserChangeItem->dmUser.UserName; 
                    lvItem.cchTextMax = sizeof(pUserChangeItem->dmUser.UserName);  
                    lvItem.lParam = (LPARAM)pUserChangeItem;
                    SendMessage(m_hWndUsers, LVM_INSERTITEMA, 0, (LPARAM)&lvItem);
                }
                pUserChangeItem = pUserChangeItem->pNextUser;
            }
            //  Select the first user.
            ListView_SetItemState(m_hWndUsers, 0, LVIS_SELECTED, LVIS_SELECTED);
            m_iLastSelected = 0;
            memset(&lvItem, 0, sizeof(lvItem));
            lvItem.mask = LVIF_PARAM;
            SendMessage(m_hWndUsers, LVM_GETITEMA, 0, (LPARAM)&lvItem);
            pUserChangeItem = (USER_ACCESS_CHANGE *)lvItem.lParam;
            if(pUserChangeItem)
            {
                UpdateAccessInfo(pUserChangeItem->dmUser.UserName, pUserChangeItem->dwNewAccess);
            }
        } else
        {
            char szThisComputer[40];
            LoadStringA(_Module.GetModuleInstance(), IDS_LITERAL_THIS_COMPUTER, szThisComputer, sizeof(szThisComputer));
            UpdateAccessInfo(szThisComputer, m_dwAccess, FALSE);
        }
    }
}

void CConsoleSecurityPage::UpdateAccessInfo(LPCSTR UserName, DWORD dwAccess, BOOL fEnable)
{
    //Set the Permissions for
    char szBuffer[255];
    WindowUtils::rsprintf(szBuffer, IDS_SECURITY_PERMISSIONS_FOR, UserName);
    SetWindowTextA(GetDlgItem(m_hPropDlg, IDC_SECURITY_ACCESS_TEXT), szBuffer);
    m_fUpdatingUI = TRUE;
    //Set all of the check boxes.
    ListView_SetCheckState(m_hWndAccess, PERMISSION_LISTVIEW_INDEX(IDS_PERMISSION_READ),
                           (dwAccess&DMPL_PRIV_READ ? TRUE : FALSE));
    ListView_SetCheckState(m_hWndAccess, PERMISSION_LISTVIEW_INDEX(IDS_PERMISSION_WRITE),
                           (dwAccess&DMPL_PRIV_WRITE ? TRUE : FALSE));
    ListView_SetCheckState(m_hWndAccess, PERMISSION_LISTVIEW_INDEX(IDS_PERMISSION_CONFIGURE),
                           (dwAccess&DMPL_PRIV_CONFIGURE ? TRUE : FALSE));
    ListView_SetCheckState(m_hWndAccess, PERMISSION_LISTVIEW_INDEX(IDS_PERMISSION_CONTROL),
                           (dwAccess&DMPL_PRIV_CONTROL ? TRUE : FALSE));
    ListView_SetCheckState(m_hWndAccess, PERMISSION_LISTVIEW_INDEX(IDS_PERMISSION_MANAGE),
                           (dwAccess&DMPL_PRIV_MANAGE ? TRUE : FALSE));
    EnableWindow(m_hWndAccess, fEnable);
    m_fUpdatingUI = FALSE;
}


#define UACF_ADDANDREMOVE (UACF_ADD|UACF_REMOVE)  

void CConsoleSecurityPage::SetApplyButton()
{
    // Walk through the change list, and find out if there
    // is anything that needs to be applied.
    //
    // Add, Remove, and changed permissions.
    BOOL fEnableApply = FALSE;
    USER_ACCESS_CHANGE *pUserChangeItem;
    pUserChangeItem = m_pUserAccessChangeList;
    while(pUserChangeItem)
    {
        // If add and remove are both set, the item does not require work on apply,
        // it is just tracking access, so if the user adds it back then we remember
        // the last known access. Cool, eh? So we skip these.
        if(UACF_ADDANDREMOVE != (UACF_ADDANDREMOVE&pUserChangeItem->dwFlags))
        {
            // If either add or remove are set (we ruled out both)
            // Then we need  to enable apply
            if(UACF_ADDANDREMOVE&pUserChangeItem->dwFlags)
            {
                fEnableApply = TRUE;
                break;
            }
            //If the original permissions are different from the
            //new permissions, we need to apply
            if(pUserChangeItem->dmUser.AccessPrivileges != pUserChangeItem->dwNewAccess)
            {
                // we only need one reasons to enable apply
                // no point in search for others.
                fEnableApply = TRUE;
                break;
            }
        }
        //If we are here, we haven't found a reason to enable apply,
        //keep looking.
        pUserChangeItem = pUserChangeItem->pNextUser;
    }

    // Set the apply button, one way or the other.
    SendMessage(GetParent(m_hPropDlg), fEnableApply ? PSM_CHANGED : PSM_UNCHANGED, (WPARAM)m_hPropDlg, (LPARAM)0);
}

bool CConsoleSecurityPage::OnApply(bool fClose)
{
    //If we are not in manage mode there is nothing to apply
    if(!m_fManageMode) return true;
    
    //
    //  Allowing apply.
    //  1) We don't want to allow a locked box that noone has manage privileges to.  So the UI will pop-up an
    //     error on OK or APPLY, if this would happen by applying the changes.  They will be able to get around
    //     this with the xbManage command line tool, but only more sophisticated users will even try that.
    //
    //  2) We apply all the changes, or stop if an error occurs.  To avoid leaving noone with manage privileges,
    //     as soon as we find a user that will have manage privileges after the apply, we move it to the front of
    //     the list.
    //
    //  3) One of the changes may be to take away our own manage privileges.  To avoid not being able to complete
    //     the task, we switch our interface over into shared mode before applying the first change(if it is not already in secure mode).
    //

    HRESULT hr;
    BOOL fSomeoneCanManage = FALSE;
    USER_ACCESS_CHANGE *pUserItem;
    USER_ACCESS_CHANGE *pUserPreviousItem;

    pUserPreviousItem = NULL;
    pUserItem = m_pUserAccessChangeList;
    while(pUserItem)
    {
        //If the item does not have the remove flag it will be here after apply.
        if(!(UACF_REMOVE&pUserItem->dwFlags))
        {
            //Check if the new access, will include manage.
            if(pUserItem->dwNewAccess&DMPL_PRIV_MANAGE)
            {
                fSomeoneCanManage = TRUE;
                //The first item in our list before attempting to apply should
                //be an item with manage privilege.
                if(pUserPreviousItem)
                {
                    pUserPreviousItem->pNextUser = pUserItem->pNextUser;
                    pUserItem->pNextUser = m_pUserAccessChangeList;
                    m_pUserAccessChangeList = pUserItem;
                }
                break;
            }
        }
        pUserPreviousItem = pUserItem;
        pUserItem = pUserItem->pNextUser;
    }

    if(!fSomeoneCanManage)
    {
        // If we apply this, there will not be a single user who may manage.
        // Tell the user no way.
        WindowUtils::MessageBoxResource(NULL, IDS_CANNOT_REMOVE_LAST_MANAGER, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP, m_szConsoleName);
        // Don't let the user close the dialog.
        return false;
    }

    // Now let's go ahead and apply changes.
    hr = S_OK;
    if(!m_fSecureMode)
        hr = m_pConnection->HrUseSharedConnection(TRUE);
    
    //Make sure that our connection is shared.
    if(SUCCEEDED(hr))
    {
        pUserPreviousItem = NULL;
        pUserItem = m_pUserAccessChangeList;
        while(pUserItem)
        {
            //If the remove flag is set, we need MAY need to remove it.
            if(pUserItem->dwFlags&UACF_REMOVE)
            {
                //If the Add flag is set, then this user was added since
                //changes were last applied, and no need to remove them
                if(!(pUserItem->dwFlags&UACF_ADD))
                {
                    hr = m_pConnection->HrRemoveUser(pUserItem->dmUser.UserName);
                    if(FAILED(hr)) break;
                }
                // Remove delete this item from our change list, since
                // the console no longer exists.
                if(pUserPreviousItem)
                    pUserPreviousItem->pNextUser = pUserItem->pNextUser;
                else
                    m_pUserAccessChangeList = pUserItem->pNextUser;
                // The previous item doesn't change, so just update
                // user item and continue.
                pUserItem = pUserItem->pNextUser;
                continue;
            }
            
            //If the add flag is set, then we need to add it (notice that if the
            //remove flag was set, we didn't fall through to here)
            if(pUserItem->dwFlags&UACF_ADD)
            {
                hr = m_pConnection->HrAddUser(pUserItem->dmUser.UserName, pUserItem->dwNewAccess);
                if(FAILED(hr)) break;
                pUserItem->dmUser.AccessPrivileges = pUserItem->dwNewAccess;
                pUserItem->dwFlags &= ~UACF_ADD;
            } else if(pUserItem->dmUser.AccessPrivileges != pUserItem->dwNewAccess)
            {
                hr = m_pConnection->HrSetUserAccess(pUserItem->dmUser.UserName, pUserItem->dwNewAccess);
                if(FAILED(hr)) break;
                pUserItem->dmUser.AccessPrivileges = pUserItem->dwNewAccess;
            }
            pUserPreviousItem = pUserItem;
            pUserItem = pUserItem->pNextUser;
        }
        // done with the shared conneciton.
        if(!m_fSecureMode)
            m_pConnection->HrUseSharedConnection(FALSE);
    }

    // Reset the Apply Button
    SetApplyButton();

    // Recheck that we have manage permissions, if we don't switch out of this mode.
    if(!m_fSecureMode)
    {
            //  Find out what access we have.
            //  (Prior to build 4114 this didn't work, you got your
            //   machine name and asked about it.  However, this only
            //   worked if you manage permissions. So we try the new
            //   real way and then fall back to the old way - in which
            //   case you will see that you have no access, if you
            //   don't have manage.)
            //
            m_dwAccess = 0;
            if(SUCCEEDED(m_pConnection->HrGetUserAccess(NULL, &m_dwAccess)))
            {
                char szComputerName[MAX_COMPUTERNAME_LENGTH+1];
                DWORD dwComputerNameLength = sizeof(szComputerName);
                GetComputerNameA(szComputerName, &dwComputerNameLength);
                m_pConnection->HrGetUserAccess(szComputerName, &m_dwAccess);
            }
            if(!(DMPL_PRIV_MANAGE&m_dwAccess))
            {
                m_fManageMode = FALSE;
                DeleteUserList();   //Forget about the users
                ShowHideWindows();  //Switch the UI
                UpdateData();
            }
    }


    if(FAILED(hr))
    {
        char szError[60];
        FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
        WindowUtils::MessageBoxResource(m_hPropDlg, IDS_COULDNT_APPLY_SECURITY_CHANGES, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP, szError);
        return false;
    }
    
    return true;
}

//-----------------------------------------------------------
//  Settings Page for Consoles
//-----------------------------------------------------------
CConsoleSettingsPage::CConsoleSettingsPage(
    IXboxConnection *pConnection
    ) : m_pConnection(pConnection)
{
    _ASSERT(m_pConnection);
    m_pConnection->AddRef();
}

BOOL
CConsoleSettingsPage::GetCaption(
    LPSTR szBuffer,
    ULONG ccBuffer
    )
{
    *szBuffer = '\0';
    return TRUE;
}

INT_PTR 
CConsoleSettingsPage::OnInitDialog(
    HWND hwndDefaultControl
    )
{
    return 1;
}

void 
CConsoleSettingsPage::CompletePropSheetPage(
    PROPSHEETPAGEA *pPropSheetPage
    )
{
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_CONSOLE_SETTING);
}

//-----------------------------------------------------------
//  Advanced Page for Consoles
//-----------------------------------------------------------
CConsoleAdvancedPage ::CConsoleAdvancedPage(
    IXboxConnection *pConnection,
    LPCSTR           pszConsoleName
    ) : m_pConnection(pConnection)
{
    _ASSERT(m_pConnection);
    m_pConnection->AddRef();
    strcpy(m_szConsoleName, pszConsoleName);
}

BOOL
CConsoleAdvancedPage::GetCaption(
    LPSTR szBuffer,
    ULONG ccBuffer
    )
{
    *szBuffer = '\0';
    return TRUE;
}

INT_PTR 
CConsoleAdvancedPage::OnInitDialog(
    HWND hwndDefaultControl
    )
{
    HWND hWndCtrl;

    // The default state for warm boot is checked
    CheckDlgButton(m_hPropDlg, IDC_WARMBOOT, BST_CHECKED);
    return 1;
}


void 
CConsoleAdvancedPage::CompletePropSheetPage(
    PROPSHEETPAGEA *pPropSheetPage
    )
{
    pPropSheetPage->pszTemplate = MAKEINTRESOURCEA(IDD_CONSOLE_ADVANCED);
}

INT_PTR
CConsoleAdvancedPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT uControlId = LOWORD(wParam);
    UINT uCommand = HIWORD(wParam);
    HRESULT hr;

    //
    //  Look for button presses that require action
    //
    if ( uCommand == BN_CLICKED) 
    { 
        //
        //  If the reboot button was pressed, read the control with the
        //  boot parameters and perform the reboot.
        //
        if(uControlId==IDC_REBOOT)
        {
           CWaitCursor waitCursor;
           DM_XBE dmXbe;
           LPCSTR pszLaunchTitle = NULL;
           DWORD dwFlags = 0;
           //  Get the state of IDC_WARMBOOT, and of IDC_RUNNINGTITLE
           if(IsDlgButtonChecked(m_hPropDlg, IDC_WARMBOOT))
           {
             dwFlags = DMBOOT_WARM;
           }
           
           if(IsDlgButtonChecked(m_hPropDlg, IDC_RUNNINGTITLE))
           {
             hr = m_pConnection->HrGetXbeInfo(NULL, &dmXbe);
             if(SUCCEEDED(hr))
             {
                pszLaunchTitle = dmXbe.LaunchPath;
             }
             // The error case is hit when the dash is running, this is not
             // really an error, pszLaunchTitle will be NULL, and we reboot
             // into the default title as expected.
           }
           hr = m_pConnection->HrReboot(dwFlags, pszLaunchTitle);
           if(FAILED(hr))
           {
                char szError[60];
                FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
                WindowUtils::MessageBoxResource(m_hPropDlg, IDS_COULDNT_REBOOT, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP, m_szConsoleName, szError);
           }
           return TRUE;
        }

        //
        //  If the IDC_CAPTURE button was pressed, do a screen capture.
        //
        else if(uControlId==IDC_CAPTURE)
        {
            Utils::ScreenCapture(m_hPropDlg, m_szConsoleName);
            return TRUE;
        }
    }
    return FALSE;
}
