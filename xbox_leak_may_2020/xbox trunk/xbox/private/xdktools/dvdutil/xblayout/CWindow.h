// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cwindow.hpp
// Contents:  
// Revisions: 13-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Window creation flags
#define FLAG_WINDOW_POS_LASTKNOWN 0x00000001

class CWindow
{
public:
    CWindow(char *szName, DWORD dwWindowCreationFlags, HINSTANCE hinst);
    virtual ~CWindow();
    virtual bool Init(CWindow *pwindowParent);
    virtual void Uninit() {}
    int Run();

    // WndProc          -- Main message handler.  Public since the static ::WndProc function needs
    //                     to call into it.
    BOOL WndProc(UINT uMsg, WPARAM wparam, LPARAM lparam);

    // m_hwnd           -- Handle to our Win32 windo.  Public since the static ::WndProc function
    //                     needs to reference it in calls to WndProc before our CreateWindow
    //                     function has completed.
    HWND m_hwnd;

    HINSTANCE m_hinst;

    bool GetRegValue(HKEY hkey, char *szValue, long *pn);
    bool SetRegValue(HKEY hkey, char *szValue, long n);

    virtual void OnSize() = NULL;
    virtual BOOL DerivedWndProc(UINT uMsg, WPARAM wparam, LPARAM lparam) {return DefWindowProc(m_hwnd, uMsg, wparam, lparam);}
    virtual BOOL ButtonPressed(int idButton) { return FALSE; }

   
    virtual bool OnNotify(WPARAM wparam, LPARAM lparam) {return 0;}

    virtual void FinishedDrag(CListView *plv, POINT pt) {}

    virtual void DragMove(POINT pt) {}
    virtual void EraseAllSelectionBars() {}
    virtual void SetGroupable(bool fGroupable) {}
    virtual void SetUngroupable(bool fUngroupable) {}

    RECT m_rc;
    void SetCaption(char *sz);

protected:

    void GetChildControlRect(HWND hwndControl, RECT *prc);

    void SetLastKnownPos();
    void StorePosition();

    void GetCaption(char *sz);
    char m_szName[256];
    char m_szMenuName[256];
    DWORD m_dwWindowCreationFlags;

private:
    bool InitInstance(CWindow *pwindowParent);
    HACCEL m_haccel;
};
