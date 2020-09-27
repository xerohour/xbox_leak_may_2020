// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      csplitter.h
// Contents:  
// Revisions: 13-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CViewMode;

class CSplitter
{
public:
    BOOL Init(CViewMode *pvmParent, HWND hwndParent, int nStartPos);
    int GetPos() {return m_nPos; }
    int GetPermPos() {return m_nPermPos; }
    int GetWidth() {return m_nWidth; }
    void Resize(RECT *prc);

    void HandleMouseDown(int nClientX, int nClientY);
    void HandleMouseMove(int nClientX, int nClientY);
    void HandleMouseUp(int nClientX, int nClientY);

    WNDPROC m_pfnStaticWndProc;
    void SetVisible(bool fVisible) { ShowWindow(m_hwnd, fVisible ? SW_SHOW : SW_HIDE); }

private:

    BOOL m_fDragging;
    int  m_nPermPos;
    int  m_nPos;
    int  m_nWidth;
    int  m_nDragDeltaX;
    HWND m_hwnd;
    HWND m_hwndParent;
    CViewMode *m_pvmParent;
};