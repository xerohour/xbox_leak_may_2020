// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cstatusbar.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CStatusBar
{
public:
    bool Init(HWND hwndParent);
    ~CStatusBar();

    void SetText(int iRegion, char *szText);
    void MoveTo(int nX, int nY, int nW, int nH);

    void SetNumParts(int nParts);

    void Resize(int nW, int nH);

    int GetHeight();

    void Update();
    void DeferUpdate() {m_fDeferUpdate = true;}

private:
    char m_rgszText[2][400];
    bool m_fDeferUpdate;
    HWND m_hwnd;
    int m_nParts;
    int m_nW;
};
