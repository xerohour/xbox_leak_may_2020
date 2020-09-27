
class CFileContainerFormats : public IEnumFORMATETC 
{ 
public: 
    CFileContainerFormats(UINT cfmt, const FORMATETC afmt[]); 
    ~CFileContainerFormats();
    
    // *** IUnknown methods ***    
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);     
    STDMETHODIMP_(ULONG) AddRef();     
    STDMETHODIMP_(ULONG) Release();
    
    // *** IEnumFORMATETC methods ***     
    STDMETHODIMP Next(ULONG celt, FORMATETC *rgelt, ULONG *pceltFethed);     
    STDMETHODIMP Skip(ULONG celt);     
    STDMETHODIMP Reset();     
    STDMETHODIMP Clone(IEnumFORMATETC ** ppenum);

private:
    long m_cRef;
    UINT m_iFmt;
    UINT m_cFmt;
    FORMATETC *m_aFmt;
};
