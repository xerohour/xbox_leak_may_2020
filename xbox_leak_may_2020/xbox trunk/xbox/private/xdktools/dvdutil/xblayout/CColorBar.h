// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cColorbar.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CColorBar : public CInitedObject
{
public:
    bool Init(CWindow *pwindow, COLORREF rgbTopColor, COLORREF rgbBotColor);
    ~CColorBar();

    void MoveTo(int nX, int nY, int nW, int nH);

    void SetColors(COLORREF rgbTopColor, COLORREF rgbBotColor);

    HWND GetHwnd() {return m_hwnd; }
    void SetVisible(bool fVisible);

    void Repaint(HDC hdc);

    WNDPROC m_pfnStaticWndProc;
private:

    RECT m_rc;
    HWND m_hwndTip;
    COLORREF m_rgbTopColor, m_rgbBotColor;
    HWND m_hwnd;
};
