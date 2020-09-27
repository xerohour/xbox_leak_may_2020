###
### THIS FILE IS FOR ARCHIVAL PURPOSES
### CAddConsoleData Has been changed extensively, and I just wanted to preserve the code
### somewhere.  The major cause of the change is that this version of the Add New Console
### wizard allowed adding more than one Xbox at a time, the new wizard does not allow this.
###

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

struct CONSOLE_INFO
{
    LPSTR           pszName;
    DWORD           dwIpAddress;
    BOOL            fSelected;
    BOOL            fLocalSubnet;
    CONSOLE_INFO    *pPrevious;
    CONSOLE_INFO    *pNext;
};

class CAddConsoleData
{
  public:
    CAddConsoleData() : m_fChoose(FALSE), m_iConsoleCount(0), m_iConsolesSelected(0), m_pConsoles(NULL), m_pConsolesTail(NULL){}

    void InitListView(HWND hListView, bool fChooser = FALSE);
    void RefreshListView(HWND hListView);
    void AddConsole(HWND hListView, LPSTR pszConsoleName);
    INT_PTR OnNotify(LPNMHDR pnmhdr);

    void OnFinish();

    int  GetSelectCount() {return m_iConsolesSelected;}
    void GetSelectedItemsString(LPSTR pszBuffer, UINT ccBuffer);
    
    BOOL         m_fChoose;

  protected:
     void AddItemToListView(HWND hListView, int index, CONSOLE_INFO *pConsole);

  private:
    int           m_iConsoleCount;
    int           m_iConsolesSelected;
    CONSOLE_INFO *m_pConsoles;
    CONSOLE_INFO *m_pConsolesTail;
};

class CAddConsoleWelcome : public CWizardPage
{
    public:
     CAddConsoleWelcome(){}

    protected:

     virtual INT_PTR OnSetActive();
     virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
};

#if NEVER
class CAddConsoleHow : public CWizardPage
{
    public:
      CAddConsoleHow(CAddConsoleData *pAddConsoleData) : m_pAddConsoleData(pAddConsoleData){}

    protected:
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
      virtual INT_PTR OnSetActive();
      virtual INT_PTR OnWizNext();
      virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
      CAddConsoleData *m_pAddConsoleData;
};


class CAddConsoleChoose : public CWizardPage
{
    public:
      CAddConsoleChoose(CAddConsoleData *pAddConsoleData) : m_pAddConsoleData(pAddConsoleData){}

    protected:
    
      virtual INT_PTR OnSetActive();
      virtual INT_PTR OnWizNext();
      virtual INT_PTR OnWizBack();
      virtual INT_PTR OnNotify(LPNMHDR pnmhdr);

      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
      //virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);

      CAddConsoleData *m_pAddConsoleData;
};
#endif NEVER

class CAddConsoleByName : public CWizardPage
{
    public:
      CAddConsoleByName(CAddConsoleData *pAddConsoleData) : m_pAddConsoleData(pAddConsoleData){}

    protected:
    
      virtual INT_PTR OnSetActive();
      virtual INT_PTR OnWizNext();
      virtual INT_PTR OnWizBack();
      virtual INT_PTR OnNotify(LPNMHDR pnmhdr);
      virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);

      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
      //virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);

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