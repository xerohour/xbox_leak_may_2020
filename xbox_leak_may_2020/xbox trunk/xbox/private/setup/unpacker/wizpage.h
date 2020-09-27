//  WIZPAGE.H
//
//  Created 27-Mar-2001 [JonT]

#ifndef _WIZPAGE_H_
#define _WIZPAGE_H_

//---------------------------------------------------------------------

class CWizardPage
{
protected:
    static BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual BOOL OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual VOID GetPageByID(PROPSHEETPAGE* pPSP, UINT uPageID, DLGPROC pfnDlgProc);
    virtual VOID OnInit() {};
    virtual BOOL OnNext() { return TRUE; };
    virtual BOOL OnQueryCancel();
    virtual VOID OnSetActive() = 0;
    virtual VOID OnBack() {};
    virtual VOID OnFinish() {};
    

    VOID SetNextPage(UINT uPageID);

    CWizard* m_pWizard;
    HWND m_hwnd;
    DWORD m_dwDlgID;
    HFONT m_hTitleFont;
    PUINT m_puResult;

public:
    CWizardPage(CWizard* pWizard, PROPSHEETPAGE* pPSP, UINT uPageID, DLGPROC pfnDlgProc = DlgProc, PUINT puResult = NULL);
    ~CWizardPage();
};



inline
CWizardPage::~CWizardPage()
{
    if (m_hTitleFont)
    {
        DeleteObject(m_hTitleFont);
        m_hTitleFont = NULL;
    }
}

#endif // #ifndef _WIZPAGE_H_
