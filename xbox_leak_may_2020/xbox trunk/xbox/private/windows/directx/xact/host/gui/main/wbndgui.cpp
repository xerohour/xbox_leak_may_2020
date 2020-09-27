/***************************************************************************
 *
 *  Copyright (C) 2/13/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wbndgui.cpp
 *  Content:    Wave Bundler GUI wrapper objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/13/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactapp.h"
#include <commdlg.h>

const LVCOLUMNDATA CGuiWaveBankEntry::m_aColumnData[] =
{
    { IDS_ENTRY_NAME,           LVCTYPE_STRING, TRUE },
    { IDS_ENTRY_FORMAT,         LVCTYPE_NUMBER, TRUE },
    { IDS_ENTRY_SAMPLING_RATE,  LVCTYPE_NUMBER, TRUE },
    { IDS_ENTRY_BIT_RESOLUTION, LVCTYPE_NUMBER, TRUE },
    { IDS_ENTRY_CHANNEL_COUNT,  LVCTYPE_NUMBER, TRUE },
    { IDS_ENTRY_SIZE,           LVCTYPE_NUMBER, TRUE },
    { IDS_ENTRY_CONVERSION,     LVCTYPE_STRING, TRUE },
    { IDS_ENTRY_SOURCE_FILE,    LVCTYPE_STRING, TRUE },
};


/****************************************************************************
 *
 *  CGuiWaveBankProject
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::CGuiWaveBankProject"

CGuiWaveBankProject::CGuiWaveBankProject
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CGuiWaveBankProject
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::~CGuiWaveBankProject"

CGuiWaveBankProject::~CGuiWaveBankProject
(
    void
)
{
}


/****************************************************************************
 *
 *  BrowseBankFile
 *
 *  Description:
 *      Browses for a wave bank file path.
 *
 *  Arguments:
 *      HWND [in]: parent window.
 *      LPTSTR [out]: bank file path.
 *      UINT [in]: path buffer size, in characters.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::BrowseBankFile"

BOOL
CGuiWaveBankProject::BrowseBankFile
(
    HWND                    hWnd, 
    LPTSTR                  pszFile, 
    UINT                    nLength
)
{
    static const LPCTSTR    pszFilter       = MAKE_COMMDLG_FILTER(TEXT("Wave Bank"), TEXT("*.") WBWRITER_FILEEXTENSION_BANK) \
                                              MAKE_COMMDLG_FILTER(TEXT("All"), TEXT("*.*"));
                                                        
    static const DWORD      dwOfnFlags      = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    OPENFILENAME            ofn             = { 0 };
    TCHAR                   szTitle[0x100];

    FormatStringResource(szTitle, NUMELMS(szTitle), IDS_CREATE_WAVE_BANK);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.Flags = dwOfnFlags;
    ofn.lpstrFilter = pszFilter;
    ofn.lpstrFile = pszFile;
    ofn.nMaxFile = nLength;
    ofn.lpstrTitle = FormatStringResourceStatic(IDS_CREATE_WAVE_BANK);
    ofn.lpstrDefExt = WBWRITER_FILEEXTENSION_BANK;

    return GetSaveFileName(&ofn);
}


/****************************************************************************
 *
 *  BrowseHeaderFile
 *
 *  Description:
 *      Browses for a wave bank header file path.
 *
 *  Arguments:
 *      HWND [in]: parent window.
 *      LPTSTR [out]: bank header file path.
 *      UINT [in]: path buffer size, in characters.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::BrowseHeaderFile"

BOOL
CGuiWaveBankProject::BrowseHeaderFile
(
    HWND                    hWnd, 
    LPTSTR                  pszFile, 
    UINT                    nLength
)
{
    static const LPCTSTR    pszFilter       = MAKE_COMMDLG_FILTER(TEXT("C/C++ Headers"), TEXT("*.h;*.hpp")) \
                                              MAKE_COMMDLG_FILTER(TEXT("All"), TEXT("*.*"));
                                                        
    static const DWORD      dwOfnFlags      = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    OPENFILENAME            ofn             = { 0 };
    TCHAR                   szTitle[0x100];

    FormatStringResource(szTitle, NUMELMS(szTitle), IDS_CREATE_WAVE_BANK_HEADER);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.Flags = dwOfnFlags;
    ofn.lpstrFilter = pszFilter;
    ofn.lpstrFile = pszFile;
    ofn.nMaxFile = nLength;
    ofn.lpstrTitle = szTitle;
    ofn.lpstrDefExt = WBWRITER_FILEEXTENSION_HEADER;

    return GetSaveFileName(&ofn);
}


/****************************************************************************
 *
 *  BrowseEntryFile
 *
 *  Description:
 *      Browses for a wave bank file path.
 *
 *  Arguments:
 *      HWND [in]: parent window.
 *      LPTSTR [out]: bank header file path.
 *      UINT [in]: path buffer size, in characters.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::BrowseEntryFile"

BOOL
CGuiWaveBankProject::BrowseEntryFile
(
    HWND                    hWnd, 
    LPTSTR                  pszFile, 
    UINT                    nLength
)
{
    static const LPCTSTR    pszFilter   = MAKE_COMMDLG_FILTER(TEXT("Sound"), TEXT("*.wav;*.aif;*.aiff")) \
                                          MAKE_COMMDLG_FILTER(TEXT("Wave"), TEXT("*.wav")) \
                                          MAKE_COMMDLG_FILTER(TEXT("Macintosh AIFF"), TEXT("*.aif;*.aiff")) \
                                          MAKE_COMMDLG_FILTER(TEXT("All"), TEXT("*.*"));
                                                    
    DWORD                   dwOfnFlags  = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;
    OPENFILENAME            ofn         = { 0 };
    TCHAR                   szTitle[0x100];

    //
    // If more than MAX_PATH characters were supplied, allow for multiple
    // file selection
    //

    if(nLength > MAX_PATH)
    {
        dwOfnFlags |= OFN_ALLOWMULTISELECT;
    }

    FormatStringResource(szTitle, NUMELMS(szTitle), IDS_CREATE_WAVE_BANK_ENTRY);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.Flags = dwOfnFlags;
    ofn.lpstrFilter = pszFilter;
    ofn.lpstrFile = pszFile;
    ofn.nMaxFile = nLength;
    ofn.lpstrTitle = szTitle;
    ofn.hInstance = g_hInstance;
    ofn.lpTemplateName = MAKEINTRESOURCE(IDD_WAVEBANK_ENTRY_OFN_CHILD);
    ofn.lpfnHook = WaveBankEntryOFNHookProc;

    return GetOpenFileName(&ofn);
}


/****************************************************************************
 *
 *  WaveBankEntryOFNHookProc
 *
 *  Description:
 *      Hook procedure for "Add wave bank entry" common dialog.
 *
 *  Arguments:
 *      HWND [in]: child window handle.
 *      UINT [in]: message identifier.
 *      WPARAM [in]: message parameter.
 *      LPARAM [in]: message parameter.
 *
 *  Returns:  
 *      UINT: message result, or 0 for default.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankProject::WaveBankEntryOFNHookProc"

UINT
CGuiWaveBankProject::WaveBankEntryOFNHookProc
(
    HWND                    hWnd, 
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam
)
{
    static const UINT       nPaneControlId  = 0x4a0;
    LPOFNOTIFY              pHdr;
    HWND                    hWndParent;
    HWND                    hWndPane;
    RECT                    rcAlign;
    int                     nAlignWidth;
    RECT                    rc;
    int                     nWidth;
    TCHAR                   szPath[MAX_PATH];
    int                     x;
    
    switch(uMsg)
    {
        case WM_NOTIFY:
            pHdr = (LPOFNOTIFY)lParam;

            switch(pHdr->hdr.code)
            {
                case CDN_INITDONE:

                    //
                    // Align the child dialog with the pane on the left
                    //

                    hWndParent = ::GetParent(hWnd);
                    hWndPane = ::GetDlgItem(hWndParent, nPaneControlId);

                    ::GetWindowRect(hWndPane, &rcAlign);
                    ::GetWindowRect(hWnd, &rc);

                    ::ScreenToClient(hWndParent, (LPPOINT)&rcAlign.left);
                    ::ScreenToClient(hWndParent, (LPPOINT)&rc.left);

                    nWidth = rc.right - rc.left;
                    nAlignWidth = rcAlign.right - rcAlign.left;

                    x = rcAlign.left + ((nAlignWidth - nWidth) / 2);

                    ::SetWindowPos(hWnd, NULL, x, rc.top, 0, 0, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);

                    //
                    // Load button icons
                    //

                    ::SendDlgItemMessage(hWnd, IDC_PLAY, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadResourceIconSmall(IDI_PLAY));
                    ::SendDlgItemMessage(hWnd, IDC_STOP, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadResourceIconSmall(IDI_STOP));

                    break;
            }

            break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDC_PLAY:
                    hWndParent = ::GetParent(hWnd);

                    if((int)::SendMessage(hWndParent, CDM_GETFILEPATH, NUMELMS(szPath), (LPARAM)szPath) > 0)
                    {
                        ::PlaySound(szPath, NULL, SND_FILENAME | SND_ASYNC);
                    }

                    break;

                case IDC_STOP:
                    ::PlaySound(NULL, NULL, 0);

                    break;
            }
    }
    
    return 0;
}


/****************************************************************************
 *
 *  CGuiWaveBank
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::CGuiWaveBank"

CGuiWaveBank::CGuiWaveBank
(
    CGuiWaveBankProject *   pParent
)
:   CWaveBank(pParent)
{
    m_ListWindow.Initialize(this);

    g_pApplication->m_Project.MakeDirty();
}


/****************************************************************************
 *
 *  ~CGuiWaveBank
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::~CGuiWaveBank"

CGuiWaveBank::~CGuiWaveBank
(
    void
)
{
    g_pApplication->m_Project.MakeDirty();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCTSTR [in]: bank name.
 *      LPCTSTR [in]: bank file name.
 *      LPCTSTR [in]: header file name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::Initialize"

HRESULT
CGuiWaveBank::Initialize
(
    LPCTSTR                     pszBankName,
    LPCTSTR                     pszBankFile,
    LPCTSTR                     pszHeaderFile
)
{
    CMainProjectTree *          pTree       = &g_pApplication->m_MainFrame.m_ProjectTree;
    CMainProjectTreeCategory *  pParentItem = &pTree->m_WaveBundlerCategory;
    HRESULT                     hr;

    //
    // Initialize the base class
    //

    hr = CWaveBank::Initialize(pszBankName, pszBankFile, pszHeaderFile);

    //
    // Add the bank to the project tree
    //

    if(SUCCEEDED(hr))
    {
        hr = CMainProjectTreeItem::Create(pTree, pParentItem, m_szBankName, pTree->m_pszWaveBankType) ? S_OK : E_FAIL;
    }

    return hr;
}


/****************************************************************************
 *
 *  SetName
 *
 *  Description:
 *      Renames the bank.
 *
 *  Arguments:
 *      LPCTSTR [in]: bank name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::SetName"

HRESULT 
CGuiWaveBank::SetName
(
    LPCTSTR                  pszName
)
{
    HRESULT                 hr;

    if(!_tcscmp(pszName, m_szBankName))
    {
        return S_OK;
    }

    hr = CWaveBank::SetName(pszName);

    if(SUCCEEDED(hr))
    {
        MakeDirty();
    }

    return hr;
}


/****************************************************************************
 *
 *  SetBankPath
 *
 *  Description:
 *      Sets the bank path.
 *
 *  Arguments:
 *      LPCTSTR [in]: bank path.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::SetBankPath"

HRESULT 
CGuiWaveBank::SetBankPath
(
    LPCTSTR                  pszBankPath
)
{
    if(!_tcsicmp(pszBankPath, m_szBankFile))
    {
        return S_OK;
    }

    _tcsncpy(m_szBankFile, pszBankPath, NUMELMS(m_szBankFile));

    MakeDirty();

    return S_OK;
}


/****************************************************************************
 *
 *  SetHeaderPath
 *
 *  Description:
 *      Sets the header path.
 *
 *  Arguments:
 *      LPCTSTR [in]: header path.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::SetHeaderPath"

HRESULT 
CGuiWaveBank::SetHeaderPath
(
    LPCTSTR                  pszHeaderPath
)
{
    if(!_tcsicmp(pszHeaderPath, m_szHeaderFile))
    {
        return S_OK;
    }

    _tcsncpy(m_szHeaderFile, pszHeaderPath, NUMELMS(m_szHeaderFile));

    MakeDirty();

    return S_OK;
}


/****************************************************************************
 *
 *  SetFlags
 *
 *  Description:
 *      Sets the bank flags.
 *
 *  Arguments:
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::SetFlags"

HRESULT 
CGuiWaveBank::SetFlags
(
    DWORD                   dwFlags
)
{
    if(dwFlags == m_dwFlags)
    {
        return S_OK;
    }

    m_dwFlags = dwFlags;

    MakeDirty();

    return S_OK;
}


/****************************************************************************
 *
 *  SetAlignment
 *
 *  Description:
 *      Sets entry data alignment.
 *
 *  Arguments:
 *      DWORD [in]: alignment.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::SetAlignment"

HRESULT 
CGuiWaveBank::SetAlignment
(
    DWORD                   dwAlignment
)
{
    HRESULT                 hr;
    
    if(dwAlignment == m_dwAlignment)
    {
        return S_OK;
    }

    hr = CWaveBank::SetAlignment(dwAlignment);

    if(SUCCEEDED(hr))
    {
        MakeDirty();
    }

    return hr;
}


/****************************************************************************
 *
 *  OpenList
 *
 *  Description:
 *      Opens the list window.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::OpenList"

BOOL
CGuiWaveBank::OpenList
(
    void
)
{
    return m_ListWindow.Create();
}


/****************************************************************************
 *
 *  CloseList
 *
 *  Description:
 *      Closes the list window.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::CloseList"

void
CGuiWaveBank::CloseList
(
    void
)
{
    m_ListWindow.Destroy();
}


/****************************************************************************
 *
 *  UpdateUI
 *
 *  Description:
 *      Updates the UI for this object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::UpdateUI"

void
CGuiWaveBank::UpdateUI
(
    void
)
{
    //
    // Update the tree view
    //
    
    CTreeViewItem::SetText(m_szBankName);

    //
    // Update the list window
    //

    m_ListWindow.SetWindowText(m_szBankName);
}


/****************************************************************************
 *
 *  OnGetInfo
 *
 *  Description:
 *      Updates the UI for this object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::OnGetInfoTip"

void
CGuiWaveBank::OnGetInfoTip
(
    LPTSTR                  pszText, 
    int                     cchTextMax
)
{
    TCHAR                   szBankSize[0x100];
    DWORD                   dwBankSize;
    
    if(dwBankSize = GetBankDataSize())
    {
        FormatNumber(dwBankSize, FALSE, TRUE, szBankSize);
    }
    else
    {
        FormatStringResource(szBankSize, NUMELMS(szBankSize), IDS_BANKSIZE_UNKNOWN);
    }

    FormatStringResource(pszText, cchTextMax, IDS_BANK_INFOTIP, m_szBankFile, m_szHeaderFile, m_dwEntryCount, szBankSize);
}


/****************************************************************************
 *
 *  OnEndLabelEdit
 *
 *  Description:
 *      Updates the UI for this object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::OnEndLabelEdit"

BOOL
CGuiWaveBank::OnEndLabelEdit
(
    LPCTSTR                 pszText
)
{
    SetName(pszText);
    
    return FALSE;
}


/****************************************************************************
 *
 *  OnContextMenu
 *
 *  Description:
 *      Handles WM_CONTEXTMENU messages.
 *
 *  Arguments:
 *      HWND [in]: window the user clicked in.
 *      int [in]: mouse x-coordinate.
 *      int [in]: mouse y-coordinate.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::OnContextMenu"

BOOL
CGuiWaveBank::OnContextMenu
(
    HWND                    hWndFocus,
    int                     x,
    int                     y,
    LRESULT *               plResult
)
{
    if(CMainProjectTreeItem::OnContextMenu(hWndFocus, x, y, plResult))
    {
        return TRUE;
    }

    TrackPopupMenu(g_pApplication->m_MainFrame.m_ahMenus[g_pApplication->m_MainFrame.MENUIDX_WAVEBANK_POPUP], TPM_RIGHTBUTTON, x, y, 0, *m_pTree, NULL);

    return FALSE;
}


/****************************************************************************
 *
 *  OnCommand
 *
 *  Description:
 *      Handles WM_COMMAND messages.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *      UINT [in]: control identifier.
 *      HWND [in]: control window handle.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::OnCommand"

BOOL
CGuiWaveBank::OnCommand
(
    UINT                    nCommandId,
    UINT                    nControlId,
    HWND                    hWndControl,
    LRESULT *               plResult
)
{
    if(CMainProjectTreeItem::OnCommand(nCommandId, nControlId, hWndControl, plResult))
    {
        return TRUE;
    }

    switch(nControlId)
    {
        case ID_ACTIVATE:
        case ID_WAVEBANK_ACTIVATE:
            OnDoubleClick();
            break;

        case ID_REFRESH:
        case ID_WAVEBANK_REFRESH:
            OnCmdRefresh();
            break;
        
        case ID_RENAME:
        case ID_WAVEBANK_RENAME:
            OnCmdRename();
            break;

        case ID_DELETE:
        case ID_WAVEBANK_DELETE:
            OnCmdRemove();
            break;

        case ID_PROPERTIES:
        case ID_WAVEBANK_PROPERTIES:
            OnCmdProperties();
            break;
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnCmdRefresh
 *
 *  Description:
 *      Reloads the wave bank.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::OnCmdRefresh"

void
CGuiWaveBank::OnCmdRefresh
(
    void
)
{
    m_ListWindow.Refresh();
}


/****************************************************************************
 *
 *  OnCmdRename
 *
 *  Description:
 *      Renames the currently selected entry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::OnCmdRename"

void
CGuiWaveBank::OnCmdRename
(
    void
)
{
    EditLabel();
}


/****************************************************************************
 *
 *  OnCmdRemove
 *
 *  Description:
 *      Removes the currently selected entries.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::OnCmdRemove"

void
CGuiWaveBank::OnCmdRemove
(
    void
)
{
    if(IDYES != MsgBoxResource(g_pApplication->m_MainFrame, MB_ICONQUESTION | MB_YESNO, IDS_CONFIRM_DELETE_WAVEBANK))
    {
        return;
    }

    m_pParent->RemoveBank(this);
}


/****************************************************************************
 *
 *  OnCmdProperties
 *
 *  Description:
 *      Displays properties.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBank::OnCmdProperties"

void
CGuiWaveBank::OnCmdProperties
(
    void
)
{
    CWaveBankPropertiesDialog    Dialog(this);

    Dialog.DoModal(&g_pApplication->m_MainFrame);
}


/****************************************************************************
 *
 *  CGuiWaveBankEntry
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CGuiWaveBank * [in]: parent bank.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::CGuiWaveBankEntry"

CGuiWaveBankEntry::CGuiWaveBankEntry
(
    CGuiWaveBank *          pWaveBank
)
:   CWaveBankEntry(pWaveBank)
{
    g_pApplication->m_Project.MakeDirty();
}


/****************************************************************************
 *
 *  ~CGuiWaveBankEntry
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::~CGuiWaveBankEntry"

CGuiWaveBankEntry::~CGuiWaveBankEntry
(
    void
)
{
    MakeDirty();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object from a source file.
 *
 *  Arguments:
 *      LPCTSTR [in]: entry name.
 *      LPCTSTR [in]: wave file name.
 *      DWORD [in]: processing flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::Initialize"

HRESULT
CGuiWaveBankEntry::Initialize
(
    LPCTSTR                 pszEntryName,
    LPCTSTR                 pszFileName,
    DWORD                   dwFlags
)
{
    HRESULT                 hr;
    
    //
    // Hand off to the base class
    //

    hr = CWaveBankEntry::Initialize(pszEntryName, pszFileName, dwFlags);

    //
    // Save the file extension
    //

    if(SUCCEEDED(hr))
    {
        _splitpath(pszFileName, NULL, NULL, NULL, m_szFileExtension);
    }

    return hr;
}


/****************************************************************************
 *
 *  Load
 *
 *  Description:
 *      Loads wave data.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to force a reload.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::Load"

HRESULT
CGuiWaveBankEntry::Load
(
    BOOL                    fForce
)
{
    CGuiWaveBank *          pBank       = (CGuiWaveBank *)m_pParent;
    HICON                   hLargeIcon  = NULL;
    HICON                   hSmallIcon  = NULL;
    SHFILEINFO              shfi;
    HRESULT                 hr;

    //
    // Register the shell icon with the list view
    //

    if(!m_fLoaded)
    {
        if(!pBank->m_ListWindow.m_ListView.FindRegisteredType(m_szFileExtension))
        {
            if(SHGetFileInfo(m_szFileName, 0, &shfi, sizeof(shfi), SHGFI_ICON | SHGFI_LARGEICON))
            {
                hLargeIcon = shfi.hIcon;
            }
            
            if(SHGetFileInfo(m_szFileName, 0, &shfi, sizeof(shfi), SHGFI_ICON | SHGFI_SMALLICON))
            {
                hSmallIcon = shfi.hIcon;
            }

            if(!hLargeIcon)
            {
                hLargeIcon = LoadIcon(NULL, IDI_QUESTION);
            }

            if(!hSmallIcon)
            {
                hSmallIcon = LoadIcon(NULL, IDI_QUESTION);
            }

            pBank->m_ListWindow.m_ListView.RegisterType(m_szFileExtension, hLargeIcon, hSmallIcon);
        }
    }

    //
    // Hand off to the base class
    //

    hr = CWaveBankEntry::Load(fForce);

    //
    // Update the UI
    //

    if(SUCCEEDED(hr))
    {
        UpdateUI();
    }

    return hr;
}


/****************************************************************************
 *
 *  SetName
 *
 *  Description:
 *      Renames the entry.
 *
 *  Arguments:
 *      LPCTSTR [in]: entry name.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::SetName"

HRESULT 
CGuiWaveBankEntry::SetName
(
    LPCTSTR                 pszName
)
{
    HRESULT                 hr;

    if(!_tcscmp(pszName, m_szEntryName))
    {
        return S_OK;
    }
    
    hr = CWaveBankEntry::SetName(pszName);

    if(SUCCEEDED(hr))
    {
        MakeDirty();
    }

    return hr;
}


/****************************************************************************
 *
 *  SetFlags
 *
 *  Description:
 *      Sets entry flags.  Because of error-checking, the flags actually set
 *      by this method may not match those passed in.
 *
 *  Arguments:
 *      DWORD [in]: mask of flags to change.
 *      DWORD [in]: new flags.
 *
 *  Returns:  
 *      DWORD: new flags.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::SetFlags"

DWORD
CGuiWaveBankEntry::SetFlags
(
    DWORD                   dwMask,
    DWORD                   dwFlags
)
{
    DWORD                   dwNewFlags;
    
    dwNewFlags = CWaveBankEntry::SetFlags(dwMask, dwFlags);

    if(dwNewFlags != (dwFlags & dwMask))
    {
        MakeDirty();
    }

    return dwNewFlags;
}


/****************************************************************************
 *
 *  UpdateUI
 *
 *  Description:
 *      Updates the UI for this entry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::UpdateUI"

void
CGuiWaveBankEntry::UpdateUI
(
    void
)
{
    CGuiWaveBank *          pBank       = (CGuiWaveBank *)CWaveBankEntry::m_pParent;
    UINT                    nColumn     = 1;
    DWORD                   dwOffset    = 0;

    if(!pBank->m_ListWindow.m_ListView)
    {
        return;
    }
    
    //
    // If we're not already in the list, put us there
    //

    if(-1 == m_nItemIndex)
    {
        CListViewItem::Create(&pBank->m_ListWindow.m_ListView, m_szEntryName, m_szFileExtension);
    }
    else
    {
        SetText(m_szEntryName);
    }

    //
    // Add column data
    //

    if(WAVEBANKMINIFORMAT_TAG_ADPCM == m_Format.wFormatTag)
    {
        SetColumnText(nColumn++, TEXT("ADPCM"));
    }
    else
    {
        SetColumnText(nColumn++, TEXT("PCM"));
    }

    SetColumnValue(nColumn++, m_Format.nSamplesPerSec, FALSE, TRUE);
    
    if(WAVEBANKMINIFORMAT_TAG_ADPCM == m_Format.wFormatTag)
    {
        SetColumnValue(nColumn++, 4, FALSE, FALSE);
    }
    else if(WAVEBANKMINIFORMAT_BITDEPTH_8 == m_Format.wBitsPerSample)
    {
        SetColumnValue(nColumn++, 8, FALSE, FALSE);
    }
    else
    {
        SetColumnValue(nColumn++, 16, FALSE, FALSE);
    }

    SetColumnValue(nColumn++, m_Format.nChannels, FALSE, FALSE);
    SetColumnValue(nColumn++, m_dwDestLength, FALSE, TRUE);

    if(m_dwFlags & WBFILTER_ADPCM)
    {
        SetColumnText(nColumn++, CXboxAdpcmFilter::GetName());
    }
    else if(m_dwFlags & WBFILTER_8BIT)
    {
        SetColumnText(nColumn++, C8BitFilter::GetName());
    }
    else
    {
        SetColumnText(nColumn++, NULL);
    }
    
    SetColumnText(nColumn++, m_szFileName);

    //
    // Resort and resize
    //

    m_pList->Sort(-1, 0);
    m_pList->ResizeColumns();
}


/****************************************************************************
 *
 *  OnEndLabelEdit
 *
 *  Description:
 *      Updates the UI for this object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::OnEndLabelEdit"

BOOL
CGuiWaveBankEntry::OnEndLabelEdit
(
    LPCTSTR                 pszText
)
{
    SetName(pszText);

    return FALSE;
}


/****************************************************************************
 *
 *  OnStateChanged
 *
 *  Description:
 *      Handles state changes.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::OnStateChanged"

void
CGuiWaveBankEntry::OnStateChanged
(
    DWORD                   dwOldState,
    DWORD                   dwNewState
)
{
    //
    // If the selection state has changed, update the UI
    //

    if((dwOldState | dwNewState) & LVIS_SELECTED)
    {
        ((CWaveBankWindow *)m_pList->m_pParent)->UpdateCommandState();
    }
}


/****************************************************************************
 *
 *  OnCmdPlay
 *
 *  Description:
 *      Plays the entry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CGuiWaveBankEntry::OnCmdPlay"

void
CGuiWaveBankEntry::OnCmdPlay
(
    void
)
{
    g_pApplication->m_RemoteAudition.Play(this);
}


/****************************************************************************
 *
 *  CWaveBankWindow
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::CWaveBankWindow"

CWaveBankWindow::CWaveBankWindow
(
    void
)
:   CWorkspaceChild(WKSPCTYPE)
{
    m_fUpdateCommands = TRUE;
}


/****************************************************************************
 *
 *  ~CWaveBankWindow
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::~CWaveBankWindow"

CWaveBankWindow::~CWaveBankWindow
(
    void
)
{
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates or activates the window.
 *
 *  Arguments:
 *      LPBOOL [out]: TRUE if the window was created; FALSE if it was just
 *                    activated.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::Create"

BOOL
CWaveBankWindow::Create
(
    LPBOOL                  pfCreated
)
{
    return CWorkspaceChild::Create(m_pBank->m_szBankName, IDI_WAVEBANK, pfCreated);
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles WM_CREATE messages.
 *
 *  Arguments:
 *      LPVOID [in]: creation context.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnCreate"

BOOL
CWaveBankWindow::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    static const DWORD      dwExStyle       = 0;
    static const DWORD      dwStyle         = WS_CHILD | WS_VISIBLE | LVS_ALIGNLEFT | LVS_ALIGNTOP | LVS_AUTOARRANGE | LVS_NOLABELWRAP | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS;
    static const DWORD      dwListExStyle   = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP;
    BOOL                    fSuccess;

    if(CWorkspaceChild::OnCreate(pvContext, plResult))
    {
        return TRUE;
    }

    //
    // Create the list-view
    //

    fSuccess = m_ListView.Create(this, dwExStyle, dwStyle, dwListExStyle);

    //
    // Add columns
    //

    if(fSuccess)
    {
        fSuccess = m_ListView.InsertColumns(CGuiWaveBankEntry::m_aColumnData, NUMELMS(CGuiWaveBankEntry::m_aColumnData));
    }

    //
    // Populate the list-view
    //

    if(fSuccess)
    {
        fSuccess = Refresh();
    }

    //
    // Enable file drops
    //

    if(fSuccess)
    {
        DragAcceptFiles();
    }

    //
    // Success
    //

    if(!fSuccess)
    {
        *plResult = 0;
        return TRUE;
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnSize
 *
 *  Description:
 *      Handles WM_SIZE messages.
 *
 *  Arguments:
 *      UINT [in]: resize type.
 *      UINT [in]: client width.
 *      UINT [in]: client height.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnSize"

BOOL
CWaveBankWindow::OnSize
(
    UINT                    nType,
    UINT                    nWidth,
    UINT                    nHeight,
    LRESULT *               plResult
)
{
    if(CWorkspaceChild::OnSize(nType, nWidth, nHeight, plResult))
    {
        return TRUE;
    }
    
    m_ListView.MoveWindow(0, 0, nWidth, nHeight);

    return FALSE;
}


/****************************************************************************
 *
 *  OnContextMenu
 *
 *  Description:
 *      Handles WM_CONTEXTMENU messages.
 *
 *  Arguments:
 *      HWND [in]: window the user clicked in.
 *      int [in]: mouse x-coordinate.
 *      int [in]: mouse y-coordinate.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnContextMenu"

BOOL
CWaveBankWindow::OnContextMenu
(
    HWND                    hWndFocus,
    int                     x,
    int                     y,
    LRESULT *               plResult
)
{
    HMENU                   hMenu;
    
    if(CWorkspaceChild::OnContextMenu(hWndFocus, x, y, plResult))
    {
        return TRUE;
    }

    TrackPopupMenu(g_pApplication->m_MainFrame.m_ahMenus[g_pApplication->m_MainFrame.MENUIDX_WAVEBANK_ENTRY_POPUP], TPM_RIGHTBUTTON, x, y, 0, m_hWnd, NULL);

    return FALSE;
}


/****************************************************************************
 *
 *  OnCommand
 *
 *  Description:
 *      Handles WM_COMMAND messages.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *      UINT [in]: control identifier.
 *      HWND [in]: control window handle.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnCommand"

BOOL
CWaveBankWindow::OnCommand
(
    UINT                    nCommandId,
    UINT                    nControlId,
    HWND                    hWndControl,
    LRESULT *               plResult
)
{
    if(CWorkspaceChild::OnCommand(nCommandId, nControlId, hWndControl, plResult))
    {
        return TRUE;
    }

    switch(nControlId)
    {
        case ID_REFRESH:
        case ID_WAVEBANK_REFRESH:
            Refresh();
            break;
        
        case ID_ADD_WAVEBANK_ENTRY:
            OnCmdAddEntry();
            break;
        
        case ID_RENAME:
        case ID_WAVEBANK_ENTRY_RENAME:
            OnCmdRename();
            break;

        case ID_DELETE:
        case ID_WAVEBANK_ENTRY_DELETE:
            OnCmdRemove();
            break;

        case ID_WAVEBANK_ENTRY_ADPCM:
            OnCmdSetFlags(WBFILTER_ADPCM);
            break;

        case ID_WAVEBANK_ENTRY_8BIT:
            OnCmdSetFlags(WBFILTER_8BIT);
            break;

        case ID_SELECT_ALL:
            OnCmdSelectAll();
            break;

        case ID_PLAY:
            OnCmdPlay();
            break;

        case ID_STOP:
            OnCmdStop();
            break;
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnCmdRename
 *
 *  Description:
 *      Renames the currently selected entry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnCmdRename"

void
CWaveBankWindow::OnCmdRename
(
    void
)
{
    int                     nSelectionCount;
    CListViewItem *         pItem;
    
    //
    // We can only rename one item at a time
    //

    if(1 != (nSelectionCount = m_ListView.GetSelectedCount()))
    {
        return;
    }

    if(pItem = m_ListView.GetNextItem(NULL, LVNI_SELECTED))
    {
        pItem->EditLabel();
    }
}


/****************************************************************************
 *
 *  OnCmdRemove
 *
 *  Description:
 *      Removes the currently selected entries.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnCmdRemove"

void
CWaveBankWindow::OnCmdRemove
(
    void
)
{
    CGuiWaveBankEntry *     pEntry;
    BOOL                    fRedraw;
    BOOL                    fAppTitle;
    int                     nSelectionCount;

    if((nSelectionCount = m_ListView.GetSelectedCount()) < 1)
    {
        return;
    }
    
    if(IDYES != MsgBoxResource(g_pApplication->m_MainFrame, MB_ICONQUESTION | MB_YESNO, IDS_CONFIRM_DELETE_WAVEBANK_ENTRY))
    {
        return;
    }

    fRedraw = m_ListView.SetRedraw(FALSE);
    fAppTitle = g_pApplication->m_Project.EnableAppTitleUpdates(FALSE);
    
    while(TRUE)
    {
        if(!(pEntry = (CGuiWaveBankEntry *)m_ListView.GetNextItem(NULL, LVNI_SELECTED)))
        {
            break;
        }

        m_pBank->RemoveEntry(pEntry);
    }

    m_ListView.SetRedraw(fRedraw);
    g_pApplication->m_Project.EnableAppTitleUpdates(fAppTitle);
}


/****************************************************************************
 *
 *  OnCmdSetFlags
 *
 *  Description:
 *      Sets flags for the selected entry or entries.
 *
 *  Arguments:
 *      DWORD [in]: valid mask.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnCmdSetFlags"

void
CWaveBankWindow::OnCmdSetFlags
(
    DWORD                   dwFilter
)
{
    static const DWORD      dwMask          = WBFILTER_MASK;
    DWORD                   dwFlags         = 0;
    int                     nChanged        = 0;
    int                     nSelectionCount;
    CGuiWaveBankEntry *     pEntry;
    BOOL                    fRedraw;

    //
    // Make sure at least one item is selected
    //

    if((nSelectionCount = m_ListView.GetSelectedCount()) < 1)
    {
        return;
    }

    //
    // Disable redraws
    //
    
    fRedraw = m_ListView.SetRedraw(FALSE);

    //
    // Get the current flags for all selected items.  If any of them have
    // this compression bit set, we'll toggle them all off.  If none of
    // them do, we'll toggle them all on.
    //

    pEntry = NULL;
    
    while(TRUE)
    {
        if(!(pEntry = (CGuiWaveBankEntry *)m_ListView.GetNextItem(pEntry, LVNI_SELECTED)))
        {
            break;
        }

        dwFlags |= pEntry->m_dwFlags;
    }

    dwFlags = ~dwFlags & dwFilter;

    pEntry = NULL;
    
    while(TRUE)
    {
        if(!(pEntry = (CGuiWaveBankEntry *)m_ListView.GetNextItem(pEntry, LVNI_SELECTED)))
        {
            break;
        }

        if(pEntry->SetFlags(dwMask, dwFlags) == dwFlags)
        {
            nChanged++;
        }

        pEntry->UpdateUI();
    }

    //
    // Restore redraws
    //

    m_ListView.SetRedraw(fRedraw);

    //
    // Update command states
    //

    if(nChanged)
    {
        UpdateCommandState();
    }

    //
    // If not every item was able to be changed, warn the user
    //

    if(nChanged < nSelectionCount)
    {
        if(nChanged)
        {
            MsgBoxResource(g_pApplication->m_MainFrame, MB_OK | MB_ICONINFORMATION, IDS_NOT_ALL_CONVERTED);
        }
        else
        {
            MsgBoxResource(g_pApplication->m_MainFrame, MB_OK | MB_ICONINFORMATION, IDS_NONE_CONVERTED);
        }
    }
}


/****************************************************************************
 *
 *  OnCmdSelectAll
 *
 *  Description:
 *      Selects all items in the window.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnCmdSelectAll"

void
CWaveBankWindow::OnCmdSelectAll
(
    void
)
{
    BOOL                    fEnable;

    fEnable = EnableCommandUpdate(FALSE);

    m_ListView.SelectAll(TRUE);

    EnableCommandUpdate(fEnable);
}


/****************************************************************************
 *
 *  OnCmdPlay
 *
 *  Description:
 *      Auditions the selected entries.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnCmdPlay"

void
CWaveBankWindow::OnCmdPlay
(
    void
)
{
    CGuiWaveBankEntry *     pEntry  = NULL;
    
    //
    // Turn on the hourglass
    //

    BeginWaitCursor();

    //
    // Enumerate selected entries and audition them
    //

    while(pEntry = (CGuiWaveBankEntry *)m_ListView.GetNextItem(pEntry, LVNI_SELECTED))
    {
        pEntry->OnCmdPlay();
    }

    //
    // Turn off the hourglass
    //

    EndWaitCursor();
}


/****************************************************************************
 *
 *  OnCmdStop
 *
 *  Description:
 *      Stops any playing entries.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnCmdStop"

void
CWaveBankWindow::OnCmdStop
(
    void
)
{
    g_pApplication->m_RemoteAudition.StopAll();
}


/****************************************************************************
 *
 *  Refresh
 *
 *  Description:
 *      Refreshes list data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::Refresh"

BOOL
CWaveBankWindow::Refresh
(
    void
)
{
    PLIST_ENTRY             pleEntry;
    CGuiWaveBankEntry *     pEntry;
    BOOL                    fRedraw;

    //
    // Change the cursor to provide user feedback
    //

    BeginWaitCursor();

    //
    // Disable window redraws
    //

    fRedraw = m_ListView.SetRedraw(FALSE);

    //
    // Reload all entries
    //
    
    m_pBank->LoadEntries(TRUE);

    //
    // Restore redraw state
    //
    
    m_ListView.SetRedraw(fRedraw);

    //
    // Restore the cursor
    //

    EndWaitCursor();

    return TRUE;
}


/****************************************************************************
 *
 *  UpdateCommandState
 *
 *  Description:
 *      Updates the state of all commands used by this object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::UpdateCommandState"

void
CWaveBankWindow::UpdateCommandState
(
    void
)
{
    static const UINT       anCommands[]    =
    {
        ID_ADD_WAVEBANK_ENTRY,
        ID_WAVEBANK_ENTRY_ADPCM,
        ID_WAVEBANK_ENTRY_8BIT,
        ID_PLAY,
        ID_STOP,
        ID_WAVEBANK_ENTRY_RENAME,
        ID_WAVEBANK_ENTRY_DELETE,
    };

    CGuiWaveBankEntry *     pEntry          = NULL;
    int                     nValidAdpcm     = 0;
    int                     nAdpcm          = 0;
    int                     nValid8bit      = 0;
    int                     n8bit           = 0;
    BOOL                    fEnable;
    int                     nSelectionCount;
    DWORD                   dwFlags;
    UINT                    i;

    if(!m_fUpdateCommands)
    {
        return;
    }
    
    //
    // If we're not the active window, disable all commands that relate to
    // us
    //

#pragma TODO("Is there ever a possibility that we'll do this *after* another window has taken focus?")

    if(!((CMDIClientWindow *)m_pParent)->IsActiveChild(this))
    {
        for(i = 0; i < NUMELMS(anCommands); i++)
        {
            g_pApplication->m_MainFrame.EnableCommand(anCommands[i], FALSE);
        }

        return;
    }

    //
    // Update commands that are enabled just because we're in view
    //

    g_pApplication->m_MainFrame.EnableCommand(ID_ADD_WAVEBANK_ENTRY, TRUE);

    //
    // Update commands that depend on the selection state
    //

    nSelectionCount = m_ListView.GetSelectedCount();
    fEnable = (nSelectionCount >= 1);

    g_pApplication->m_MainFrame.EnableCommand(ID_PLAY, fEnable);
    g_pApplication->m_MainFrame.EnableCommand(ID_STOP, fEnable);
    g_pApplication->m_MainFrame.EnableCommand(ID_WAVEBANK_ENTRY_DELETE, fEnable);

    fEnable = (1 == nSelectionCount);

    g_pApplication->m_MainFrame.EnableCommand(ID_WAVEBANK_ENTRY_RENAME, fEnable);

    //
    // Update commands that depend on which entries are selected
    //

    while(TRUE)
    {
        if(!(pEntry = (CGuiWaveBankEntry *)m_ListView.GetNextItem(pEntry, LVNI_SELECTED)))
        {
            break;
        }

        dwFlags = pEntry->GetValidFlags();

        if(dwFlags & WBFILTER_ADPCM)
        {
            nValidAdpcm++;
        }

        if(dwFlags & WBFILTER_8BIT)
        {
            nValid8bit++;
        }

        dwFlags = pEntry->m_dwFlags;

        if(dwFlags & WBFILTER_ADPCM)
        {
            nAdpcm++;
        }

        if(dwFlags & WBFILTER_8BIT)
        {
            n8bit++;
        }

        if(nValidAdpcm && nAdpcm && nValid8bit && n8bit)
        {
            break;
        }
    }

    if(nValidAdpcm)
    {
        g_pApplication->m_MainFrame.EnableCommand(ID_WAVEBANK_ENTRY_ADPCM, TRUE);
        g_pApplication->m_MainFrame.CheckCommand(ID_WAVEBANK_ENTRY_ADPCM, MAKEBOOL(nAdpcm));
    }
    else
    {
        g_pApplication->m_MainFrame.EnableCommand(ID_WAVEBANK_ENTRY_ADPCM, FALSE);
    }
            
    if(nValid8bit)
    {
        g_pApplication->m_MainFrame.EnableCommand(ID_WAVEBANK_ENTRY_8BIT, TRUE);
        g_pApplication->m_MainFrame.CheckCommand(ID_WAVEBANK_ENTRY_8BIT, MAKEBOOL(n8bit));
    }
    else
    {
        g_pApplication->m_MainFrame.EnableCommand(ID_WAVEBANK_ENTRY_8BIT, FALSE);
    }
}


/****************************************************************************
 *
 *  EnableCommandUpdate
 *
 *  Description:
 *      Toggles the whether the command state is updated or not.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to allow updates.
 *
 *  Returns:  
 *      BOOL: prior value.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::EnableCommandUpdate"

BOOL 
CWaveBankWindow::EnableCommandUpdate
(
    BOOL                    fEnable
)
{
    const BOOL              fPrevious   = m_fUpdateCommands;

    if(fPrevious != fEnable)
    {
        if(m_fUpdateCommands = fEnable)
        {
            UpdateCommandState();
        }
    }

    return fPrevious;
}


/****************************************************************************
 *
 *  OnCmdAddEntry
 *
 *  Description:
 *      Adds a new entry to the bank.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnCmdAddEntry"

void
CWaveBankWindow::OnCmdAddEntry
(
    void
)
{
    static const DWORD      dwMaxEntries    = 128;
    LPTSTR                  pszPath         = NULL;
    CWaveBankEntry *        pEntry;
    LPCTSTR                 pszFile;
    TCHAR                   szFile[MAX_PATH];
    BOOL                    fSuccess;
    BOOL                    fAddedAny;
    BOOL                    fAddedAll;
    BOOL                    fRedraw;
    HRESULT                 hr;

    fSuccess = MAKEBOOL(pszPath = MEMALLOC(TCHAR, MAX_PATH * dwMaxEntries));

    //
    // Get the file path(s)
    //

    if(fSuccess)
    {
        fSuccess = CGuiWaveBankProject::BrowseEntryFile(g_pApplication->m_MainFrame, pszPath, MAX_PATH * dwMaxEntries);
    }

    if(fSuccess)
    {
        //
        // Turn off redraws
        //

        fRedraw = m_ListView.SetRedraw(FALSE);

        //
        // If more than one file was specified, the path will be first, followed
        // by each file name in a null-terminated list
        //

        pszFile = pszPath + _tcslen(pszPath) + 1;

        if(*pszFile)
        {
            fAddedAny = FALSE;
            fAddedAll = TRUE;
        
            while(*pszFile)
            {
                _makepath(szFile, NULL, pszPath, pszFile, NULL);

                hr = m_pBank->AddEntry(NULL, szFile, 0, &pEntry);

                if(SUCCEEDED(hr))
                {
                    hr = pEntry->Load();
                }

                if(SUCCEEDED(hr))
                {
                    fAddedAny = TRUE;
                }
                else
                {
                    m_pBank->RemoveEntry(pEntry);
                    fAddedAll = FALSE;
                }

                pszFile += _tcslen(pszFile) + 1;
            }

            if(!fAddedAny)
            {
                DPF_WARNING("None of the entries were added to the wave bank");
                fSuccess = FALSE;
            }
            else if(!fAddedAll)
            {
                DPF_WARNING("Not all entries were added to the wave bank");
            }
        }
        else
        {
            hr = m_pBank->AddEntry(NULL, pszPath, 0, &pEntry);

            if(SUCCEEDED(hr))
            {
                hr = pEntry->Load();
            }

            if(FAILED(hr))
            {
                m_pBank->RemoveEntry(pEntry);
                fSuccess = FALSE;
            }
        }

        //
        // Turn redraws back on
        //

        m_ListView.SetRedraw(fRedraw);
    }

    //
    // Clean up
    //

    MEMFREE(pszPath);
}


/****************************************************************************
 *
 *  OnDropFiles
 *
 *  Description:
 *      Handles WM_DROPFILE messages.
 *
 *  Arguments:
 *      HDROP [in]: drop handle.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankWindow::OnDropFiles"

BOOL
CWaveBankWindow::OnDropFiles
(
    HDROP                   hDrop,
    LRESULT *               plResult
)
{
    BOOL                    fAddedAny           = FALSE;
    BOOL                    fAddedAll           = TRUE;
    CWaveBankEntry *        pEntry;
    UINT                    nCount;
    TCHAR                   szFile[MAX_PATH];
    BOOL                    fSuccess;
    BOOL                    fRedraw;
    HRESULT                 hr;
    UINT                    i;

    if(CWorkspaceChild::OnDropFiles(hDrop, plResult))
    {
        return TRUE;
    }

    //
    // Turn off redraws
    //

    fRedraw = m_ListView.SetRedraw(FALSE);

    //
    // Enumerate files dropped
    //

    nCount = DragQueryFile(hDrop, ~0UL, NULL, 0);

    for(i = 0; i < nCount; i++)
    {
        if(!DragQueryFile(hDrop, i, szFile, NUMELMS(szFile)))
        {
            fAddedAll = FALSE;
            continue;
        }
        
        hr = m_pBank->AddEntry(NULL, szFile, 0, &pEntry);

        if(SUCCEEDED(hr))
        {
            if(FAILED(hr = pEntry->Load()))
            {
                m_pBank->RemoveEntry(pEntry);
            }
        }

        if(SUCCEEDED(hr))
        {
            fAddedAny = TRUE;
        }
        else
        {
            fAddedAll = FALSE;
        }
    }

    if(!fAddedAny)
    {
        DPF_WARNING("None of the dropped files were added to the wave bank");
    }
    else if(!fAddedAll)
    {
        DPF_WARNING("Not all of the dropped files were added to the wave bank");
    }

    //
    // Turn redraws back on
    //

    m_ListView.SetRedraw(fRedraw);

    return FALSE;
}


/****************************************************************************
 *
 *  CWaveBankPropertiesDialog
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankPropertiesDialog::CWaveBankPropertiesDialog"

CWaveBankPropertiesDialog::CWaveBankPropertiesDialog
(
    CGuiWaveBank *          pBank
)
:   CDialog(IDD)
{
    m_pBank = pBank;
}


/****************************************************************************
 *
 *  ~CWaveBankPropertiesDialog
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankPropertiesDialog::~CWaveBankPropertiesDialog"

CWaveBankPropertiesDialog::~CWaveBankPropertiesDialog
(
    void
)
{
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles WM_CREATE messages.
 *
 *  Arguments:
 *      LPVOID [in]: creation context.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankPropertiesDialog::OnCreate"

BOOL
CWaveBankPropertiesDialog::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    static const UINT       nDefaultAlignmentCount  = 15;
    TCHAR                   szAlignment[0x100];
    TCHAR                   szSize[0x100];
    DWORD                   dwAlignment;
    UINT                    i;
    
    if(CDialog::OnCreate(pvContext, plResult))
    {
        return TRUE;
    }

    //
    // Make sure all entries are loaded
    //

    if(FAILED(m_pBank->LoadEntries()))
    {
        OnCmdCancel();
    }

    //
    // Populate controls
    //

    for(i = 0, dwAlignment = m_pBank->m_dwMinAlignment; i < nDefaultAlignmentCount; i++, dwAlignment *= 2)
    {
        FormatNumber(dwAlignment, FALSE, FALSE, szAlignment);
        
        SendDlgItemMessage(IDC_WAVEBANK_ALIGNMENT, CB_INSERTSTRING, -1, (LPARAM)szAlignment);
    }

    FormatNumber(m_pBank->GetBankDataSize(), FALSE, TRUE, szSize);

    SetDlgItemText(IDC_WAVEBANK_NAME, m_pBank->m_szBankName);
    SetDlgItemText(IDC_WAVEBANK_PATH, m_pBank->m_szBankFile);
    SetDlgItemText(IDC_WAVEBANK_HEADER_PATH, m_pBank->m_szHeaderFile);
    SetDlgItemText(IDC_WAVEBANK_SIZE, szSize);
    SetDlgItemInt(IDC_WAVEBANK_ALIGNMENT, m_pBank->m_dwAlignment, FALSE);

    CheckRadioButton(m_hWnd, IDC_WAVEBANK_TYPE_BUFFER, IDC_WAVEBANK_TYPE_STREAMING, (m_pBank->m_dwFlags & WAVEBANK_TYPE_STREAMING) ? IDC_WAVEBANK_TYPE_STREAMING : IDC_WAVEBANK_TYPE_BUFFER);

    return FALSE;
}


/****************************************************************************
 *
 *  OnCommand
 *
 *  Description:
 *      Handles WM_COMMAND messages.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *      UINT [in]: control identifier.
 *      HWND [in]: control window handle.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankPropertiesDialog::OnCommand"

BOOL
CWaveBankPropertiesDialog::OnCommand
(
    UINT                    nCommandId,
    UINT                    nControlId,
    HWND                    hWndControl,
    LRESULT *               plResult
)
{
    switch(nControlId)
    {
        case IDC_WAVEBANK_PATH_BROWSE:
            OnCmdBrowseBankPath();
            break;

        case IDC_WAVEBANK_HEADER_PATH_BROWSE:
            OnCmdBrowseHeaderPath();
            break;

        default:
            return CDialog::OnCommand(nCommandId, nControlId, hWndControl, plResult);
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnCmdOK
 *
 *  Description:
 *      Handles the OK command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankPropertiesDialog::OnCmdOK"

void
CWaveBankPropertiesDialog::OnCmdOK
(
    void
)
{
    TCHAR                   szText[0x100];
    DWORD                   dwFlags;
    DWORD                   dwAlignment;
    HRESULT                 hr;
    
    //
    // Update the bank
    //

    GetDlgItemText(IDC_WAVEBANK_NAME, szText, NUMELMS(szText));

    hr = m_pBank->SetName(szText);

    if(SUCCEEDED(hr))
    {
        GetDlgItemText(IDC_WAVEBANK_PATH, szText, NUMELMS(szText));

        hr = m_pBank->SetBankPath(szText);
    }

    if(SUCCEEDED(hr))
    {
        GetDlgItemText(IDC_WAVEBANK_HEADER_PATH, szText, NUMELMS(szText));

        hr = m_pBank->SetHeaderPath(szText);
    }

    if(SUCCEEDED(hr))
    {
        dwFlags = m_pBank->m_dwFlags & ~WAVEBANK_TYPE_MASK;

        if(IsDlgButtonChecked(m_hWnd, IDC_WAVEBANK_TYPE_STREAMING))
        {
            dwFlags |= WAVEBANK_TYPE_STREAMING;
        }

        hr = m_pBank->SetFlags(dwFlags);
    }

    if(SUCCEEDED(hr))
    {
        dwAlignment = GetDlgItemInt(IDC_WAVEBANK_ALIGNMENT, NULL, FALSE);

        hr = m_pBank->SetAlignment(dwAlignment);
    }

    //
    // Close the dialog
    //

    if(SUCCEEDED(hr))
    {
        CDialog::OnCmdOK();
    }
}


/****************************************************************************
 *
 *  OnCmdBrowseBankPath
 *
 *  Description:
 *      Handles the browse command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankPropertiesDialog::OnCmdBrowseBankPath"

void
CWaveBankPropertiesDialog::OnCmdBrowseBankPath
(
    void
)
{
    TCHAR                   szPath[MAX_PATH];

    if(!GetDlgItemText(IDC_WAVEBANK_PATH, szPath, NUMELMS(szPath)))
    {
        szPath[0] = 0;
    }

    if(CGuiWaveBankProject::BrowseBankFile(m_hWnd, szPath, NUMELMS(szPath)))
    {
        SetDlgItemText(IDC_WAVEBANK_PATH, szPath);
    }
}


/****************************************************************************
 *
 *  OnCmdBrowseHeaderPath
 *
 *  Description:
 *      Handles the browse command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBankPropertiesDialog::OnCmdBrowseHeaderPath"

void
CWaveBankPropertiesDialog::OnCmdBrowseHeaderPath
(
    void
)
{
    TCHAR                   szPath[MAX_PATH];

    if(!GetDlgItemText(IDC_WAVEBANK_HEADER_PATH, szPath, NUMELMS(szPath)))
    {
        szPath[0] = 0;
    }

    if(CGuiWaveBankProject::BrowseHeaderFile(m_hWnd, szPath, NUMELMS(szPath)))
    {
        SetDlgItemText(IDC_WAVEBANK_HEADER_PATH, szPath);
    }
}


