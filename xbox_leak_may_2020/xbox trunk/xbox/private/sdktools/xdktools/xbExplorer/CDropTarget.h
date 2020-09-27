// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     cdroptarget.h
// Contents: UNDONE-WARN: Add file description
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


typedef struct
{
    bool fXbox; // if true, then the dataobject is sourced from an Xbox
    char szName[MAX_PATH];  // The name of the source Xbox (if sourced from one)
    HWND hwnd;      // Handle to the window the dataobject was sourced from.
} sSourceInfo;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASS DECLARATIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class CDropTarget : public IDropTarget
{
protected:
    long                m_cRef;
public:
    CDropTarget();
    
    //IDropTarget interface members
    STDMETHODIMP QueryInterface(REFIID, void **);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);
    STDMETHODIMP DragEnter(LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt,
                           LPDWORD pdwEffect);
    STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
    STDMETHODIMP DragLeave(void);
    STDMETHODIMP Drop(LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);

private:
    IDataObject *m_pidataobject;
};

typedef CDropTarget* PCDropTarget; 

void GetDropSourceInfo(IDataObject *pidataobject, sSourceInfo *psourceinfo);
