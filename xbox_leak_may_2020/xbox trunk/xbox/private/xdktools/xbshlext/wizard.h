/*++

Copyright (c) Microsoft Corporation

Module Name:

    wizard.h

Abstract:

    Class definitions for wizard property sheets and pages.

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    08-07-2001 : created (mitchd)

--*/


class CWizardPage;
struct WizardPageInfo
{
    WizardPageInfo():pWizardPage(NULL){}
    CWizardPage *pWizardPage;  // Pointer to a property Page
};

class CWizard
{
  public:
    CWizard() : 
        m_uMaxPages(0),
        m_uPageCount(0),
        m_pPages(NULL),
        m_pPropSheetPages(NULL),
        m_hrInitialize(E_FAIL)
        {}
    ~CWizard() {delete [] m_pPages; delete [] m_pPropSheetPages; DeleteObject(m_hTitleFont);}
    
    HRESULT Initialize(UINT uPages);
    HRESULT DoWizard(HWND hwndParent);
    void AddPage(CWizardPage *pPage);

  private:

    UINT              m_uItemCount;
    UINT              m_uMaxPages;
    UINT              m_uPageCount;
    WizardPageInfo   *m_pPages;
    PROPSHEETPAGEA   *m_pPropSheetPages;
    char              m_szCaption[MAX_PATH];
    HWND              m_hWndParent;
    HFONT             m_hTitleFont;

    //Outlaw Copy C'tor and Assignment
    CWizard(const CWizard&);
    CWizard& operator=(const CWizard&);

    HRESULT m_hrInitialize;
};

class CWizardPage
{
  public:
    CWizardPage() : m_hPageDlg(NULL), m_hTitleFont(NULL) {}
  protected:
    virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
    virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam){return 0;}
    virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam){return 0;}

    
    virtual INT_PTR OnSetActive()=0;
    virtual INT_PTR OnWizBack(){return 0;}
    virtual INT_PTR OnWizNext(){return 0;}
    virtual INT_PTR OnWizFinish(){return 0;}
    virtual INT_PTR OnNotify(LPNMHDR pnmhdr){return 0;}
    
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage) = 0;
    
    HWND    m_hPageDlg;
  private:

    void FilloutPropSheetPage(PROPSHEETPAGEA *pPropSheetPage, HFONT hTitleFont);
    HFONT   m_hTitleFont;
    CWizard *pParent;
    friend class CWizard;
};

class CAddConsoleData
{
  public:
    CAddConsoleData():
        m_fConsoleIsValid(FALSE),m_dwIpAddress(0), m_fMakeDefault(FALSE), m_dwAccess(0)
        {
            *m_szConsoleName='\0';
            *m_szPassword='\0';
        }
    
    BOOL    SetConsoleName(LPCSTR pszConsoleName);
    BOOL    GetConsoleName(LPSTR pszConsoleName);
    BOOL    GetIpAddress(PDWORD pdwIpAddress);
    void    SetMakeDefault(BOOL fSet);
    BOOL    GetMakeDefault();
    BOOL    ShowAccessDenied();
    void    SetDesiredAccess(DWORD dwAccess);
    DWORD   GetDesiredAccess();
    HRESULT SetPassword(LPCSTR pszAdminPassword);
    HRESULT OnFinish();

  private:
    BOOL    m_fConsoleIsValid;
    char    m_szConsoleName[MAX_CONSOLE_NAME];
    char    m_szPassword[MAX_CONSOLE_NAME];
    DWORD   m_dwIpAddress;
    BOOL    m_fMakeDefault;
    DWORD   m_dwAccess;
    DWORD   m_dwDesiredAccess;
    BOOL    m_fShowAccessDenied;
};

class CAddConsoleWelcome : public CWizardPage
{
    public:
     CAddConsoleWelcome(){}

    protected:

     virtual INT_PTR OnSetActive();
     virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
};

class CAddConsoleGetName : public CWizardPage
{
    public:
      CAddConsoleGetName(CAddConsoleData *pAddConsoleData) : m_pAddConsoleData(pAddConsoleData){}

    protected:
    
      virtual INT_PTR OnSetActive();
      virtual INT_PTR OnWizNext();
      virtual INT_PTR OnWizBack();
      virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);

      CAddConsoleData *m_pAddConsoleData;
};

class CAddConsoleAccessDenied : public CWizardPage
{
    public:
      CAddConsoleAccessDenied(CAddConsoleData *pAddConsoleData) : m_pAddConsoleData(pAddConsoleData){}

    protected:
    
      virtual INT_PTR OnSetActive();
      virtual INT_PTR OnWizNext();
      virtual INT_PTR OnWizBack();
      virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);

      CAddConsoleData *m_pAddConsoleData;
};

class CAddConsoleMakeDefault : public CWizardPage
{
    public:
      CAddConsoleMakeDefault(CAddConsoleData *pAddConsoleData) : m_pAddConsoleData(pAddConsoleData){}

    protected:
    
      virtual INT_PTR OnSetActive();
      virtual INT_PTR OnWizNext();
      virtual INT_PTR OnWizBack();

      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);

      CAddConsoleData *m_pAddConsoleData;
};

class CAddConsoleFinish : public CWizardPage
{
    public:
      CAddConsoleFinish(CAddConsoleData *pAddConsoleData) : m_pAddConsoleData(pAddConsoleData){}

    protected:
      
      virtual INT_PTR OnSetActive();
      virtual INT_PTR OnWizBack();
      virtual INT_PTR OnWizFinish();

      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
      CAddConsoleData *m_pAddConsoleData;
};

extern void ExecuteAddConsoleWizard();