/*****************************************************************************\
    FILE: xboxefe.h
\*****************************************************************************/

#ifndef _XBOXEFE_H
#define _XBOXEFE_H


/*****************************************************************************\
    CLASS: CXboxEfe

    DESCRIPTION:
        The stuff that tracks the state of an enumeration.
\*****************************************************************************/

class CXboxEfe           : public IEnumFORMATETC
{
public:
    //////////////////////////////////////////////////////
    // Public Interfaces
    //////////////////////////////////////////////////////
    
    // *** IUnknown ***
    virtual STDMETHODIMP_(ULONG) AddRef(void);
    virtual STDMETHODIMP_(ULONG) Release(void);
    virtual STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
    
    // *** IEnumFORMATETC ***
    virtual STDMETHODIMP Next(ULONG celt, FORMATETC * rgelt, ULONG *pceltFetched);
    virtual STDMETHODIMP Skip(ULONG celt);
    virtual STDMETHODIMP Reset(void);
    virtual STDMETHODIMP Clone(IEnumFORMATETC **ppenum);

public:
    // Friend Functions
    friend HRESULT CXboxEfe_Create(DWORD dwSize, FORMATETC rgfe[], STGMEDIUM rgstg[], CXboxObj * pfo, IEnumFORMATETC ** ppenum);

protected:
    // Private Member Variables
    int                     m_cRef;

    DWORD                   m_dwIndex;           // Current Item in the m_hdsaFormatEtc list
    DWORD                   m_dwExtraIndex;      // Current Item in the m_pfo->m_hdsaSetData list
    HDSA                    m_hdsaFormatEtc;     // pointer to the array 
    CXboxObj *               m_pfo;               // pointer to the parent IDataObject impl that has the list of extra data from ::SetData.


    CXboxEfe(DWORD dwSize, FORMATETC rgfe[], STGMEDIUM rgstg[], CXboxObj * pfo);
    CXboxEfe(DWORD dwSize, HDSA hdsaFormatEtc, CXboxObj * pfo, DWORD dwIndex);
    ~CXboxEfe(void);

    // Public Member Functions
    HRESULT _NextOne(FORMATETC * pfetc);

    
    // Friend Functions
    friend HRESULT CXboxEfe_Create(DWORD dwSize, HDSA m_hdsaFormatEtc, DWORD dwIndex, CXboxObj * pfo, IEnumFORMATETC ** ppenum);
    friend HRESULT CXboxEfe_Create(DWORD dwSize, FORMATETC rgfe[], STGMEDIUM rgstg[], CXboxObj * pfo, CXboxEfe ** ppfefe);
};

#endif // _XBOXEFE_H
