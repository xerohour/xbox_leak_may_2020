// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CUnplacedWindow.h
// Contents:  
// Revisions: 13-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CViewMode;
class CStatusBar;

class CUnplacedWindow
{
public:
    BOOL Init(CViewMode *pvmParent, HWND hwndParent);

    WNDPROC m_pfnUnplacedWndProc;
    void SetVisible(BOOL fVisible) {
        ShowWindow(m_hwnd, fVisible ? SW_SHOW : SW_HIDE); 
        if (fVisible)
            InvalidateRect(m_hwnd, NULL, TRUE);

    }
    void ToggleView() {
        m_fVisible = !m_fVisible;
        SetVisible(m_fVisible);
    }
    HWND m_hwnd;
    CUnplacedListView m_listview;
    CStatusBar m_statusbar;
    HWND m_hwndParent;
    BOOL m_fVisible;
    
private:

    CViewMode *m_pvmParent;
};