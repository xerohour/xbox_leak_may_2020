// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CViewMode.h
// Contents:  
// Revisions: 31-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class CViewMode
{
public:
    virtual void Show() = NULL;
    virtual void Hide() = NULL;
    virtual void Resize(int nW, int nH) = NULL;
    virtual void HandleEvent(eEvent event, CFileObject *pfo) = NULL;
    virtual void UpdateIfDirty() = NULL;
    virtual void Clear() = NULL;
    virtual void HandleMenuCommand(WORD command) = NULL;
};
