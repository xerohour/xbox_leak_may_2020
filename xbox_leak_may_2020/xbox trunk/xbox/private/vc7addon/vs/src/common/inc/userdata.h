//------------------------------------------------------------------------------
// USERDATA.H
//------------------------------------------------------------------------------
#pragma once
#include "textmgr.h"
#include "util.h"

class CUserData : public IVsUserData
{
private:
    struct DataNode
    {
        GUID                guidKey;
        VARIANT             vtData;
    };

    UINT                    m_dwRef;     // refcount 
    CStructArray<DataNode>  m_Data;

public:
    CUserData() : m_dwRef(0) {}
    virtual ~CUserData();

    // IUnknown
    //
    STDMETHODIMP QueryInterface (REFIID riid, LPVOID *ppObj)
    {
        *ppObj = NULL;
        if (riid == IID_IUnknown)
            *ppObj = this;
        else if (riid == IID_IVsUserData)
            *ppObj = (IVsUserData *)this;

        if (*ppObj)
        {
            ((IUnknown *)(*ppObj))->AddRef();
            return NOERROR;
        }

        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef ()
    {
        return ++m_dwRef;
    }

    STDMETHODIMP_(ULONG) Release ()
    {
        if (--m_dwRef)
            return (m_dwRef);
        delete this;
        return 0;
    }

    // IUserData methods
    STDMETHOD   (GetData)(REFGUID rguidKey, VARIANT *pvtData);
    STDMETHOD   (SetData)(REFGUID rguidKey, VARIANT vtData);
};
