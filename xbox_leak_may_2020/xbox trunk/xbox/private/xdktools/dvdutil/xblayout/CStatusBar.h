// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cstatusbar.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CStatusBar : public CInitedObject
{
public:
    bool Init(CWindow *pwindow);
    ~CStatusBar();

    void SetText(int iRegion, char *szText);
    void MoveTo(int nX, int nY, int nW, int nH);

    void SetNumParts(int nParts);

    HWND m_hwnd;

private:
    int m_nParts;
    int m_nW;
};
