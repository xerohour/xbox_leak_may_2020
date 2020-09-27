/*
 *
 * propbag.h
 *
 * CPropertyBag
 *
 */

#ifndef _PROPBAG_H
#define _PROPBAG_H

#include <stdlib.h>

enum { vpNone, vpDw, vpSz };

struct VPR {
    int vpType;
    union {
        DWORD dw;
        LPCSTR sz;
    };
    void Empty(void)
    {
        if(vpType == vpSz)
            free((void *)sz);
        vpType = vpNone;
    }
    VPR(void) { vpType = vpNone; }
    VPR(DWORD dwT) { Set(dwT); }
    VPR(LPCSTR szT) { Set(szT); }
    VPR(VPR &vpr) { Set(vpr); }
    VPR &operator=(DWORD dwT) { Empty(); Set(dwT); return *this; }
    VPR &operator=(LPCSTR szT) { Empty(); Set(szT); return *this; }
    VPR &operator=(const VPR &vpr) { Empty(); Set(vpr); return *this; }
    ~VPR() { Empty(); }
private:
    void Set(DWORD dwT) { vpType = vpDw; dw = dwT; }
    void Set(LPCSTR szT) { vpType = vpSz; sz = _strdup(szT); }
    void Set(const VPR &vpr)
    {
        vpType = vpr.vpType;
        if(vpType == vpDw)
            Set(vpr.dw);
        else
            Set(vpr.sz);
    }
};

class CPropertyBag {
public:
    CPropertyBag(void) : m_rgprop(NULL), m_ipropMax(0), m_cprop(0) {}
    ~CPropertyBag();

    DWORD Cprop(void) const { return m_cprop; }

private:
    struct PROP {
        LPCSTR szKey;
        PVOID pvValue;
        int iOrdinal;
    };
    
    PROP *m_rgprop;
    DWORD m_ipropMax;
    DWORD m_cprop;

    PROP *PpropFind(LPCSTR, BOOL) const;
    const static DWORD m_dcpropInc;
    void DestroyBag(void);

protected:
    virtual PVOID PvAdd(PVOID) const =0;
    void (*m_pfnRemovePv)(PVOID);

    BOOL FAddProp(LPCSTR, PVOID);
    PVOID FindProp(LPCSTR) const;
    PVOID FindProp(LPCSTR, int *) const;
    BOOL FChangeProp(LPCSTR, PVOID);
    void GetProps(LPCSTR *rgszKeys, PVOID *rgszValues) const;
};

template <class T> class CPropBag : public CPropertyBag {
public:
    BOOL FAddProp(LPCSTR szKey, T *p)
    {
        return CPropertyBag::FAddProp(szKey, (PVOID)p);
    }
    T *FindProp(LPCSTR szKey) const
    {
        return (T *)CPropertyBag::FindProp(szKey);
    }
    T *FindProp(LPCSTR szKey, int *piOrd) const
    {
        return (T *)CPropertyBag::FindProp(szKey, piOrd);
    }
    BOOL FChangeProp(LPCSTR szKey, T *p)
    {
        return CPropertyBag::FChangeProp(szKey, (PVOID)p);
    }
    void GetProps(LPCSTR *rgszKeys, T **rgp) const
    {
        CPropertyBag::GetProps(rgszKeys, (PVOID *)rgp);
    }
};

class CStringBag : public CPropBag<const char> {
public:
    CStringBag(void) { m_pfnRemovePv = RemovePv; }
private:
    virtual PVOID PvAdd(PVOID pv) const
    {
        return _strdup((const char *)pv);
    }
    static void RemovePv(PVOID pv)
    {
        free(pv);
    }
};

class CVarBag : public CPropBag<VPR> {
public:
    CVarBag(void) { m_pfnRemovePv = RemovePv; }
    LPCSTR FindSzProp(LPCSTR sz) const
    {
        VPR *pvp = FindProp(sz);
        return pvp && pvp->vpType == vpSz ? pvp->sz : NULL;
    }
    BOOL FFindDwProp(LPCSTR sz, PDWORD pdw) const
    {
        VPR *pvp = FindProp(sz);
        return pvp && pvp->vpType == vpDw ? (*pdw = pvp->dw, TRUE) :
            FALSE;
    }
    BOOL FAddProp(LPCSTR sz, VPR *pvp)
    {
        return CPropBag<VPR>::FAddProp(sz, pvp);
    }
    BOOL FAddProp(LPCSTR szKey, LPCSTR szVal)
    {
        VPR vprT;
        vprT.vpType = vpSz;
        vprT.sz = szVal;
        BOOL f = FAddProp(szKey, &vprT);
        vprT.vpType = vpNone;
        return f;
    }
    BOOL FAddProp(LPCSTR sz, DWORD dw)
    {
        return FAddProp(sz, &VPR(dw));
    }
private:
    virtual PVOID PvAdd(PVOID pv) const
    {
        return new VPR(*(VPR *)pv);
    }
    static void RemovePv(PVOID pv)
    {
        delete (VPR *)pv;
    }
};

#endif // _PROPBAG_H
