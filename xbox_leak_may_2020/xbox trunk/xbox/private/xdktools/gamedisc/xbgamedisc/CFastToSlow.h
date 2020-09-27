// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFastToSlow.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CFastToSlow : public CInitedObject
{
public:
    bool Init(HWND hwndParent, COLORREF rgbTopColor, COLORREF rgbBotColor);
    ~CFastToSlow();

    void Resize(RECT *prc);

    void SetVisible(bool fVisible) { ShowWindow(m_hwnd, fVisible ? SW_SHOW : SW_HIDE); }

    void Repaint(HDC hdc);
    DWORD GetWidth() {return m_rc.right - m_rc.left; }
    WNDPROC m_pfnStaticWndProc;
private:

    RECT m_rc;
    HWND m_hwndTip;
    COLORREF m_rgbTopColor, m_rgbBotColor;
    HWND m_hwnd;
};
