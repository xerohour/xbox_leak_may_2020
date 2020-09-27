// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cScratch.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CScratch : public CInitedObject
{
public:
    CScratch(CWindow *pwindow);
    ~CScratch();
    HWND m_hwnd;

    void ToggleView();
    void AddObject(CObject *pobj);
    void Clear() {m_plistview->Clear();}
private:
    void StorePosition();
    void SetLastKnownPos();
    CWindow *m_pwindow;
    bool m_fVisible;
};

