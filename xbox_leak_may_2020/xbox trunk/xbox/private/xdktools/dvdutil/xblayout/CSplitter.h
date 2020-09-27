// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      csplitter.h
// Contents:  
// Revisions: 13-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CWindow;

class CSplitter
{
public:
    CSplitter(CWindow *pwindow, int nStartPos);
    void HandleMouseDown(int nClientX, int nClientY);
    void HandleMouseMove(int nClientX, int nClientY);
    void HandleMouseUp(int nClientX, int nClientY);
    int GetPos();

private:
    bool IsMouseOver(int nClientX, int nClientY);

    bool m_fDragging;
    int m_nPos;
    CWindow *m_pwindow;
};