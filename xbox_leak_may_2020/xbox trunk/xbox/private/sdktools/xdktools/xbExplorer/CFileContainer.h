// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     cfilecontainer.h
// Contents: UNDONE-WARN: Add file description
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CNode
{
public:
    CNode(char *sz) {m_sz = sz; m_pnodeNext = NULL;}
    CNode *m_pnodeNext;
    char *m_sz;
};

class CList
{
public:
    CList();
    ~CList();
    void Add(char *sz);
    char *operator[] (int iIndex) const;
    void IncludeSingle(char *szFile);
	void IncludeSubDirs();
	int GetNumFiles() {return m_cFiles;}
private:
	bool m_fExpanded;
    char **m_rgszEntries;
    int m_nSize;
    int m_iCurEntry;
    CNode *m_pnodeHead;
    CNode *m_pnodeTail;
	int  m_cFiles;
};


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASS DECLARATIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CFileContainer : public IDataObject
{
public: 
    CFileContainer();
    ~CFileContainer();
    
    // IUnknown functions
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
    STDMETHODIMP_(ULONG) AddRef(); 
    STDMETHODIMP_(ULONG) Release();

    // IDataObject functions
    STDMETHODIMP GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
    STDMETHODIMP GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium);
    STDMETHODIMP QueryGetData(FORMATETC *pformatetc); 
    STDMETHODIMP GetCanonicalFormatEtc(FORMATETC *pformatectIn, FORMATETC *pformatetcOut);
    STDMETHODIMP SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease);
    STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);
    STDMETHODIMP DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
    STDMETHODIMP DUnadvise(DWORD dwConnection); 
    STDMETHODIMP EnumDAdvise(IEnumSTATDATA **ppenumAdvise);  

    // Our functions
    bool AddDir(char *szDirName);
    bool AddFile(char *szFileName);

    bool m_fCut, m_fPaste, m_fPasteSucceeded;
private:
    long m_cRef;

    CList m_listFiles;
    int m_cDirs, m_cFiles;
};