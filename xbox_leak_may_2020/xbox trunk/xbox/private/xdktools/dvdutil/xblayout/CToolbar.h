// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cToolbar.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CToolBar : public CInitedObject
{
public:
    bool Init(CWindow *pwindow);
    ~CToolBar();
    HWND m_hwnd;

    void CheckButton(int iButton, bool fDown);
    void EnableButton(int idButton, bool fEnabled);
    void MoveTo(int nX, int nY, int nW, int nH);
};

DWORD g_rgdwTooltip[];