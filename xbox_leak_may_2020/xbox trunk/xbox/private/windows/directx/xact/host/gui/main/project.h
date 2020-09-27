/***************************************************************************
 *
 *  Copyright (C) 2/13/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       project.h
 *  Content:    Project object (parser, object manager).
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/13/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __PROJECT_H__
#define __PROJECT_H__

#ifdef __cplusplus

//
// Forward declarations
//

class CGuiWaveBankProject;
class CGuiWaveBank;
class CGuiWaveBankEntry;

//
// Wave Bundler child build dialog
//

class CWaveBundlerBuildDialog
    : public CDialog, public CWaveBankCallback
{
public:
    enum
    {
        IDD = IDD_BUILD_WAVEBUNDLER
    };

public:
    CWaveBundlerBuildDialog(void);
    virtual ~CWaveBundlerBuildDialog(void);

protected:
    // Build callbacks
    virtual BOOL BeginProject(CWaveBankProject *pProject);
    virtual void EndProject(CWaveBankProject *pProject, HRESULT hr);
    virtual BOOL OpenBank(CWaveBank *pBank);
    virtual BOOL BeginEntry(CWaveBankEntry *pEntry);
    virtual BOOL ProcessEntry(CWaveBankEntry *pEntry, DWORD dwProcessed);
    virtual void EndEntry(CWaveBankEntry *pEntry);
};    

//
// Parent build dialog
//

class CProjectBuildDialog
    : public CDialog
{
public:
    enum
    {
        IDD = IDD_BUILD
    };

    CWaveBundlerBuildDialog m_WaveBundler;      // Wave Bundler build dialog

public:
    CProjectBuildDialog(void);
    virtual ~CProjectBuildDialog(void);

public:
    // Message handlers
    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnBuildDone(HRESULT hr, LRESULT *plResult);

    // Command handlers
    virtual void OnCmdCancel(void);
};

//
// Project object
//

class CProject
{
    friend class CWaveBundlerBuildDialog;
    friend class CProjectBuildDialog;

protected:
    TCHAR                   m_szProjectFile[MAX_PATH];      // Project file path
    CGuiWaveBankProject *   m_pWaveBankProject;             // Wave Bundler sub-project
    BOOL                    m_fDirty;                       // Project status
    HANDLE                  m_hBuildEvent;                  // Build start event
    HANDLE                  m_hThread;                      // Build thread
    CProjectBuildDialog     m_BuildDialog;                  // Build dialog
    BOOL                    m_fUpdateAppTitle;              // Update the app title?

public:
    CProject(void);
    virtual ~CProject(void);

public:
    // Project file
    virtual BOOL New(void);
    virtual BOOL Open(LPCTSTR pszProjectFile = NULL);
    virtual BOOL Reopen(void);
    virtual BOOL Save(void);
    virtual BOOL SaveAs(void);
    
    // Building
    virtual BOOL Build(void);

    // Wave Bundler
    virtual BOOL AddWaveBank(void);

    // Project status
    virtual BOOL IsDirty(void);
    virtual void MakeDirty(void);
    virtual BOOL CheckDirty(void);

    // Build status
    virtual void CancelBuild(void);
    virtual BOOL CheckBuildCancellation(void);

    // UI helpers
    virtual BOOL EnableAppTitleUpdates(BOOL fEnable);
    virtual void UpdateAppTitle(void);

protected:
    // Project file
    virtual BOOL New(BOOL fResetFilePath, BOOL fConfirm);
    virtual BOOL Open(LPCTSTR pszProjectFile, BOOL fConfirm);
    virtual BOOL Reopen(BOOL fConfirm);

    // Project status
    virtual void MakeDirty(BOOL fDirty);
    virtual void Reset(void);

private:
    // Worker thread
    virtual DWORD BuildThread(void);
    static DWORD WINAPI BuildThreadProc(LPVOID pvContext);
};

__inline BOOL CProject::New(void)
{
    return New(TRUE, TRUE);
}

__inline BOOL CProject::Open(LPCTSTR pszProjectFile)
{
    return Open(pszProjectFile, TRUE);
}

__inline BOOL CProject::Reopen(void)
{
    return Reopen(TRUE);
}

__inline BOOL CProject::IsDirty(void)
{
    return m_fDirty;
}

__inline void CProject::MakeDirty(void)
{
    MakeDirty(TRUE);
}

__inline void CProject::MakeDirty(BOOL fDirty)
{
    m_fDirty = fDirty;
    UpdateAppTitle();
}

__inline void CProject::CancelBuild(void)
{
    ResetEvent(m_hBuildEvent);
}

__inline BOOL CProject::CheckBuildCancellation(void)
{
    return WAIT_TIMEOUT == WaitForSingleObject(m_hBuildEvent, 0);
}

__inline DWORD CProject::BuildThreadProc(LPVOID pvContext)
{
    return ((CProject *)pvContext)->BuildThread();
}

#endif // __cplusplus

#endif // __PROJECT_H__
