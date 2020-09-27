
class CDropSource : public IDropSource
{
protected:
    long                m_cRef;
public:
    CDropSource();
    
    //IDropSource interface members
    STDMETHODIMP QueryInterface(REFIID, void **);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);
    STDMETHODIMP QueryContinueDrag(BOOL, DWORD);
    STDMETHODIMP GiveFeedback(DWORD);
};

typedef CDropSource* PCDropSource; 
