/*++

Copyright (c) Microsoft Corporation

Module Name:

    prop.h

Abstract:

    Class definitions for property page dialogs.

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    05-27-2001 : created (mitchd)

--*/


//
//
//

const UINT WM_PROPPAGE_REFRESH_CONTENTS=WM_APP+1;
const UINT MAX_CONSOLE_PAGES=4; //There are four console pages


class CPropertyPage;

struct PropertyPageInfo
{
    PropertyPageInfo():pPropertyPage(NULL){}
    ~PropertyPageInfo();
    CPropertyPage *pPropertyPage;  // Pointer to a property Page
};

class CXboxPropertySheet : public IXboxVisitor
{
  public:
    CXboxPropertySheet() : 
        m_uMaxPages(0),
        m_uPageCount(0),
        m_pPages(NULL),
        m_pPropSheetPages(NULL),
        m_hrInitialize(E_FAIL)
        {}
    ~CXboxPropertySheet() {delete [] m_pPages; delete [] m_pPropSheetPages;}

    /*
    ** IXboxVisitor
    */
    
    void VisitRoot(IXboxVisit *pRoot, DWORD *pdwFlags);
    void VisitAddConsole(IXboxVisit *pAddConsole, DWORD *pdwFlags);
    void VisitConsole(IXboxConsoleVisit *pConsole, DWORD *pdwFlags);
    void VisitVolume(IXboxVolumeVisit *pVolume, DWORD *pdwFlags);
    void VisitFileOrDir(IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags);
    void VisitDirectoryPost (IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags) {_ASSERTE(FALSE);}

    HRESULT Initialize(UINT uItemCount, CXboxFolder *pSelection);
    HRESULT DoProperties(UINT uStartPage, HWND hwndParent);
    void AddPage(CPropertyPage *pPage);

  private:

    static DWORD ThreadProcStart(LPVOID pvThis) {return ((CXboxPropertySheet *)pvThis)->ThreadProc();}
    DWORD ThreadProc();

    UINT              m_uItemCount;
    UINT              m_uMaxPages;
    UINT              m_uPageCount;
    PropertyPageInfo *m_pPages;
    PROPSHEETPAGEA   *m_pPropSheetPages;
    char              m_szCaption[MAX_PATH];
    CXboxFolder      *m_pSelection;
    BOOL              m_fFirstVisit;
    HWND              m_hWndParent;
    UINT              m_uStartPage;

    //Outlaw Copy C'tor and Assignment
    CXboxPropertySheet(const CXboxPropertySheet&);
    CXboxPropertySheet& operator=(const CXboxPropertySheet&);

    HRESULT m_hrInitialize;
};

class CPropertyPage
{
  public:
    CPropertyPage() : m_hPropDlg(NULL){}
    virtual BOOL GetCaption(LPSTR szBuffer, ULONG ccBuffer) {return FALSE;}

  protected:
    virtual INT_PTR OnInitDialog(HWND hwndDefaultControl) = 0;
    virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
    virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual INT_PTR OnDrawItem(UINT uItem, LPDRAWITEMSTRUCT pDrawItem) {return 0;}
    virtual bool    OnApply(bool fClose) {return true;}
    virtual INT_PTR OnSetActive() {return 0;}

    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND m_hPropDlg;
    virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage) = 0;
    
  private:

    void FilloutPropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
    CXboxPropertySheet *pParent;
    friend class CXboxPropertySheet;
};


class CVolumeGeneralPage : public CPropertyPage
/*++
  Class Description:
    Implements one of three general property pages:
      File, Folder, or Multiple Items.  These pages
      are similar enough, that it is basically just
      a different template a few branches.
--*/
{
    public:
      CVolumeGeneralPage(
          LPSTR     pszDriveDescription,
          ULONGLONG ullTotalSpace,
          ULONGLONG ullFreeSpace,
          UINT      uVolumeType,
          BOOL      fDriveOnTab
          ) : m_ullTotalSpace(ullTotalSpace), m_ullFreeSpace(ullFreeSpace),
              m_uVolumeType(uVolumeType), m_fDriveOnTab(fDriveOnTab), 
              m_dwPieShadowHgt(0)
      {
          strcpy(m_szDriveDescription,pszDriveDescription); 
      }

      virtual BOOL GetCaption(LPSTR szBuffer, ULONG ccBuffer);
    protected:
      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual INT_PTR OnDrawItem(UINT uItem, LPDRAWITEMSTRUCT pDrawItem);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
    private:
      void DrawColorRect(HDC hdc, COLORREF crDraw, const RECT *prc);

      ULONGLONG m_ullTotalSpace;
      ULONGLONG m_ullFreeSpace;
      UINT      m_uVolumeType;
      BOOL      m_fDriveOnTab;
      DWORD     m_dwPieShadowHgt;
      char      m_szDriveDescription[40];
};

class CFSItemGeneralPage : public CPropertyPage
/*++
  Class Description:
    Implements one of three general property pages:
      File, Folder, or Multiple Items.  These pages
      are similar enough, that it is basically just
      a different template a few branches.
--*/
{
    public:

       CFSItemGeneralPage(
           CXboxFolder         *pSelection,
           IXboxFileOrDirVisit *pFileOrDir,
           UINT                 uItemCount
           );
      ~CFSItemGeneralPage() {_ASSERTE(m_pSelection); m_pSelection->Release(); m_pSelection=NULL;}
      virtual BOOL GetCaption(LPSTR szBuffer, ULONG ccBuffer);

    protected:
      
      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
      virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
      virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
      virtual bool    OnApply(bool fClose);

      static DWORD StartThreadProc(PVOID pvThis) {return ((CFSItemGeneralPage *)pvThis)->ThreadProc();}
      DWORD ThreadProc();
      static bool GetAttributesCallback(PVOID pvThis, XBOX_MULTIFILE_ATTRIBUTES *pMultiFileAttributes);

      void SynchronizeUpdateThread();

    private:

      CXboxFolder               *m_pSelection;
      char                      m_szNameBuffer[MAX_PATH];
      char                      m_szLocation[MAX_XBOX_PATH];
      bool                      m_fNameChanged;
      bool                      m_fReadOnlyChanged;
      bool                      m_fHiddenChanged;
      UINT                      m_uTemplateResource;
      XBOX_MULTIFILE_ATTRIBUTES m_MultiFileAttributes;
      bool                      m_fKillThread;
      HANDLE                    m_hThread;
};


class CConsoleGeneralPage : public CPropertyPage
/*++
  Class Description:
    Implements the general page for console items.
      
--*/
{
    public:
      CConsoleGeneralPage(
           IXboxConnection  *pConnection,
           LPCSTR           pszConsoleName,
           BOOL             fMultipleConsoles
           );
      ~CConsoleGeneralPage() {m_pConnection->Release();}
      virtual BOOL GetCaption(LPSTR szBuffer, ULONG ccBuffer);

    protected:
      
      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
      
      /*
      virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
      virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
      virtual bool    OnApply(bool fClose);
      */

    private:

      IXboxConnection *m_pConnection;
      char             m_szConsoleName[MAX_PATH];
      BOOL             m_fMultipleConsoles;
};

class CConsoleSecurityPage : public CPropertyPage
/*++
  Class Description:
    Implements the security page for console items.
      
--*/
{
    public:
      CConsoleSecurityPage (
           IXboxConnection *pConnection,
           LPCSTR           pszConsoleName
           );
      ~CConsoleSecurityPage() {m_pConnection->Release();DeleteUserList();}
      HRESULT InitSecuritySupport();
      virtual BOOL GetCaption(LPSTR szBuffer, ULONG ccBuffer);

    protected:
      
      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
      virtual INT_PTR OnSetActive();
      virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
      virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
      virtual bool    OnApply(bool fClose);

    private:
      
      HRESULT InitUserList();
      void UpdateData();
      void UpdateAccessInfo(LPCSTR UserName, DWORD dwAccess, BOOL fEnable = TRUE);
      void DeleteUserList();
      void ShowHideWindows();
      void SetApplyButton();

      //Button Click Handlers
      void AddUser();
      void RemoveUser();
      void UnLockConsole();
      void ChangePassword();
      void LockConsole();
      void StartSecureMode();
        
      // Structure for tracking changes.
      struct USER_ACCESS_CHANGE
      {
        USER_ACCESS_CHANGE  *pNextUser;
        DM_USER             dmUser;
        DWORD               dwNewAccess;
        DWORD               dwFlags;
      };
      #define UACF_ADD     0x01
      #define UACF_REMOVE  0x02
      
      BOOL  m_fLocked;
      BOOL  m_fSecureMode;
      BOOL  m_fManageMode;
      DWORD m_dwAccess;
      HWND  m_hWndAccess;
      HWND  m_hWndUsers;
      int   m_iLastSelected;
      BOOL  m_fUpdatingUI;
      USER_ACCESS_CHANGE   *m_pUserAccessChangeList;
      
      IXboxConnection      *m_pConnection;
      char                  m_szConsoleName[MAX_PATH];
      
};


class CConsoleSettingsPage : public CPropertyPage
/*++
  Class Description:
    Implements the settings page for console items.
      
--*/
{
    public:
      CConsoleSettingsPage (
           IXboxConnection *pConnection
           );
      ~CConsoleSettingsPage() {m_pConnection->Release();}
      virtual BOOL GetCaption(LPSTR szBuffer, ULONG ccBuffer);

    protected:
      
      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
      
      /*
      virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
      virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
      virtual bool    OnApply(bool fClose);
      */

    private:

      IXboxConnection *m_pConnection;
};


class CConsoleAdvancedPage : public CPropertyPage
/*++
  Class Description:
    Implements the advanced page for console items.
      
--*/
{
    public:
      CConsoleAdvancedPage (
           IXboxConnection *pConnection,
           LPCSTR           pszConsoleName
           );
      ~CConsoleAdvancedPage() {m_pConnection->Release();}
      virtual BOOL GetCaption(LPSTR szBuffer, ULONG ccBuffer);

    protected:
      
      virtual INT_PTR OnInitDialog(HWND hwndDefaultControl);
      virtual void CompletePropSheetPage(PROPSHEETPAGEA *pPropSheetPage);
      virtual INT_PTR OnCommand(WPARAM wParam, LPARAM lParam);
      /*
      virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
      virtual bool    OnApply(bool fClose);
      */

    private:

      IXboxConnection *m_pConnection;
      char             m_szConsoleName[MAX_PATH];
};