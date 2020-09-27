// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CToolbarMgr.h
// Contents:  
// Revisions: 30-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CToolbarMgr
{
public:
    BOOL Init(HWND hwndParent);
    void Show(DWORD dwToolbar);
    void Hide(DWORD dwToolbar);

    BOOL AddBand(REBARBANDINFO *prbbi);

    DWORD GetRowsHeight();
    void Resize(WPARAM wparam, LPARAM lparam);

    HWND GetRebarHwnd() {return m_hwnd; }
    void HandleNotify(LPNMHDR pnmhdr);
    void CheckButton(int idBand, int idButton, bool fDown);
    void EnableButton(int idBand, int idButton, bool fEnabled);

    BOOL IsButtonChecked(int idBand, int idButton);

private:
    HWND m_hwnd;
    HWND m_hwndParent;
};

BOOL InitToolbars();
