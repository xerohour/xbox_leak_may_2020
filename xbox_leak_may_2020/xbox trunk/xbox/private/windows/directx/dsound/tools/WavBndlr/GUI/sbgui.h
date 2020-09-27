/***************************************************************************
 *
 *  Copyright (C) 11/11/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       sbgui.h
 *  Content:    GUI versions of wave bank objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/11/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __SBGUI_H__
#define __SBGUI_H__

//
// ListView column data
//

BEGIN_DEFINE_STRUCT()
    LPCSTR  pszText;            // Column header text
    BOOL    fString;            // String or numeric value
    BOOL    fAscending;         // Ascending or descending sort order
END_DEFINE_STRUCT(LVCOLUMNDATA);

#ifdef __cplusplus

//
// Forward declarations
//

class CGuiWaveBank;
class CGuiWaveBankProject;
class CGenerateDialog;

//
// Wave bank entry
//

class CGuiWaveBankEntry
    : public CWaveBankEntry
{
public:
    static LVCOLUMNDATA     m_aColumnData[8];   // Column data

public:
    CGuiWaveBank *          m_pParent;          // Parent object
    HICON                   m_hIcon;            // Icon handle

public:
    CGuiWaveBankEntry(CGuiWaveBank *pParent);
    virtual ~CGuiWaveBankEntry(void);

public:
    // Initialization
    virtual HRESULT Initialize(LPCSTR pszEntryName, LPCSTR pszWaveFile, DWORD dwFlags);
    virtual HRESULT Refresh(void);

    // Properties
    virtual HRESULT SetName(LPCSTR pszName);
    virtual DWORD SetFlags(DWORD dwMask, DWORD dwFlags);

    // UI helpers
    virtual void UpdateListView(UINT nItem = -1);

    // Indexing
    static CGuiWaveBankEntry *GetEntry(HWND hwnd, UINT nItem);
    virtual UINT GetItemIndex(void);

    // Auditioning
    virtual HRESULT Audition(PDM_CONNECTION pxbdm);
};

__inline HRESULT CGuiWaveBankEntry::Refresh(void)
{
    return LoadWaveFile();
}

__inline CGuiWaveBankEntry *CGuiWaveBankEntry::GetEntry(HWND hwnd, UINT nItem)
{
    return (CGuiWaveBankEntry *)ListView_GetItemParam(hwnd, nItem);
}

//
// Wave bank
//

class CGuiWaveBank
    : public CWaveBank
{
public:
    static LVCOLUMNDATA     m_aColumnData[5];   // Column data

public:
    CGuiWaveBankProject *   m_pParent;          // Parent object

public:
    CGuiWaveBank(CGuiWaveBankProject *pParent);
    virtual ~CGuiWaveBank(void);

public:
    // Initialization
    virtual HRESULT Initialize(LPCSTR pszBankName, LPCSTR pszBankFile, LPCSTR pszHeaderFile);
    virtual HRESULT Refresh(void);
    
    // Properties
    virtual HRESULT SetName(LPCSTR pszName);
    
    // Entries
    virtual HRESULT AddEntry(void);
    virtual void SortEntryList(UINT nColumnIndex);
    
    // UI helpers
    virtual UINT GetItemIndex(void);
    virtual void UpdateListView(UINT nItem = -1);

    // Indexing
    static CGuiWaveBank *GetBank(HWND hwnd, UINT nItem);

protected:
    // Entry creation
    virtual CWaveBankEntry *CreateEntry(void);
};

__inline CWaveBankEntry *CGuiWaveBank::CreateEntry(void)
{
    return NEW(CGuiWaveBankEntry(this));
}

__inline CGuiWaveBank *CGuiWaveBank::GetBank(HWND hwnd, UINT nItem)
{
    return (CGuiWaveBank *)ListView_GetItemParam(hwnd, nItem);
}

//
// Wave bank generation callback dialog
//

class CGenerateDialog
    : public CModelessDialog, public CWaveBankCallback
{
public:
    enum
    {
        IDD = IDD_GENERATE,
    };

public:
    CHAR                    m_szBankText[0x100];    // Bank status formatting string
    CHAR                    m_szEntryText[0x100];   // Entry status formatting string
    BOOL                    m_fContinue;            // TRUE to continue processing
    BOOL                    m_fAllowClose;          // TRUE to allow the dialog to close
    UINT                    m_nEntryIndex;          // Current entry index
    HANDLE                  m_hTerminateEvent;      // Thread termination event

public:
    CGenerateDialog(void);
    virtual ~CGenerateDialog(void);

public:
    // Creation
    virtual HWND Create(CGuiWaveBankProject *pProject);

    // Message handlers
    virtual BOOL OnInitDialog(LPVOID pvContext, LRESULT *plResult);

    // Command handlers
    virtual void OnCancel(void);

    // Notification handlers
    virtual BOOL BeginProject(UINT nBankCount, UINT nEntryCount);
    virtual void EndProject(HRESULT hr);
    virtual BOOL OpenBank(LPCSTR pszFile);
    virtual BOOL BeginEntry(LPCSTR pszFile, DWORD dwFlags);
    virtual void EndEntry(LPCSTR pszFile, DWORD dwFlags);
};    

//
// Wave bank project
//

class CGuiWaveBankProject
    : public CWaveBankProject
{
public:
    CWindow *               m_pParent;                      // Parent object
    HWND                    m_hwndBankList;                 // Bank list-view window handle
    HIMAGELIST              m_himlBankList;                 // Bank list-view image list
    HWND                    m_hwndEntryList;                // Entry list-view window handle
    HIMAGELIST              m_himlEntryList;                // Entry list-view image list
    CGuiWaveBank *          m_pActiveBank;                  // The currently active bank
    CHAR                    m_szProjectFile[MAX_PATH];      // Project file path
    HICON                   m_hBankIcon;                    // Bank icon handle
    INT                     m_nBankIcon;                    // Bank icon index into the bank image-list
    CGenerateDialog         m_GenerateDialog;               // Generation dialog

public:
    CGuiWaveBankProject(CWindow *pParent);
    virtual ~CGuiWaveBankProject(void);

public:
    // Project file
    virtual HRESULT Open(LPCSTR pszProjectFile = NULL);
    virtual HRESULT Save(BOOL fPrompt = FALSE);
    
    // Project managment
    virtual HRESULT AddBank(void);
    virtual void RemoveBank(UINT nBankIndex);
    virtual HRESULT AddEntry(void);
    virtual void RemoveEntry(UINT nEntryIndex);
    virtual void SortBankList(UINT nColumnIndex);
    virtual void SortEntryList(UINT nColumnIndex);

    // File creation
    virtual HRESULT Generate(void);

    // UI helpers
    virtual void Attach(HWND hwndBankList, HWND hwndEntryList);
    virtual void Detach(BOOL fClearList = TRUE);
    virtual void SelectBank(UINT nBankIndex);
    virtual void ClearEntryList(void);
    virtual void FillEntryList(void);
    virtual HRESULT Refresh(void);

protected:
    // Bank creation
    virtual CWaveBank *CreateBank(void);

    // Generate thread
    virtual HRESULT GenerateThread(void);

private:
    static DWORD WINAPI GenerateThreadProc(LPVOID pvContext);
};

__inline CWaveBank *CGuiWaveBankProject::CreateBank(void)
{
    return NEW(CGuiWaveBank(this));
}

__inline void CGuiWaveBankProject::SortEntryList(UINT nColumnIndex)
{
    if(m_pActiveBank)
    {
        m_pActiveBank->SortEntryList(nColumnIndex);
    }
}

#endif // __cplusplus

#endif // __SBGUI_H__
