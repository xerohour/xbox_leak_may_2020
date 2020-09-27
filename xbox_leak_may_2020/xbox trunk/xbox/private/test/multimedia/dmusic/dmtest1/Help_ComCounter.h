#pragma once

/********************************************************************************
Counts calls to COM objects.
********************************************************************************/
class CComCounter: public IUnknown
{

public:
    //IUnknown methods.
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv)
    {
        m_dwCalledQueryInterface++;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        m_dwCalledAddRef++;
        return 0xFFFFFFFF;
    }

    STDMETHODIMP_(ULONG) Release()
    {
        m_dwCalledRelease++;
        return 0xFFFFFFFF;

    };

    //Construktor.
    CComCounter(void)
    {
        m_dwCalledRelease = 0;
        m_dwCalledAddRef = 0;
        m_dwCalledQueryInterface = 0;
    }
    
    //Actually calls the destructor.
    void ActualRelease(void)
    {
        delete this;
    }


    DWORD m_dwCalledRelease;
    DWORD m_dwCalledAddRef;
    DWORD m_dwCalledQueryInterface;

};



HRESULT CreateComCounter(CComCounter **ppComCounter);
