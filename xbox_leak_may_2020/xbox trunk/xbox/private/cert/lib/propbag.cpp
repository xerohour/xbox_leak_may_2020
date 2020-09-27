/*
 *
 * propbag.cpp
 *
 * CPropertyBag
 *
 */

#include "precomp.h"

const DWORD CPropertyBag::m_dcpropInc = 64;

CPropertyBag::~CPropertyBag()
{
    if(m_rgprop) {
        for(DWORD iprop = 0; iprop < m_ipropMax; ++iprop) {
            if(m_rgprop[iprop].szKey) {
                free((PVOID)m_rgprop[iprop].szKey);
                m_pfnRemovePv(m_rgprop[iprop].pvValue);
            }
        }
        delete m_rgprop;
    }
}

CPropertyBag::PROP *CPropertyBag::PpropFind(LPCSTR szKey, BOOL fMustExist) const
{
    if(m_rgprop) {
        CCheckSum cxc;
        char *szUpcaseKey;
        char *pch;

        szUpcaseKey = (char *)_alloca(strlen(szKey) + 1);
        strcpy(szUpcaseKey, szKey);
        for(pch = szUpcaseKey; *pch; ++pch)
            if(isalpha(*pch))
                *pch = (char)toupper(*pch);
        cxc.SumBytes((PBYTE)szUpcaseKey, strlen(szUpcaseKey));
        DWORD iprop = cxc.DwRunningSum() % m_ipropMax;
        while(m_rgprop[iprop].szKey) {
            if(0 == _stricmp(szKey, m_rgprop[iprop].szKey))
                return &m_rgprop[iprop];
            iprop = (iprop + 1) % m_ipropMax;
        }
        if(!fMustExist)
            return &m_rgprop[iprop];
    }
    return NULL;
}

PVOID CPropertyBag::FindProp(LPCSTR szKey) const
{
    PROP *pp = PpropFind(szKey, TRUE);
    return pp ? pp->pvValue : NULL;
}

PVOID CPropertyBag::FindProp(LPCSTR szKey, int *piOrd) const
{
    PROP *pp = PpropFind(szKey, TRUE);
    if(!pp)
        return NULL;
    *piOrd = pp->iOrdinal;
    return pp->pvValue;
}

BOOL CPropertyBag::FChangeProp(LPCSTR szKey, PVOID pvValue)
{
    PROP *pp = PpropFind(szKey, TRUE);
    if(pp) {
        m_pfnRemovePv(pp->pvValue);
        pp->pvValue = PvAdd(pvValue);
    }
    return pp != NULL;
}

BOOL CPropertyBag::FAddProp(LPCSTR szKey, PVOID pvValue)
{
    PROP *pp;

    /* Make sure we have a value */
    if(!pvValue)
        return NULL;

    /* Make sure this key doesn't exist already */
    if(PpropFind(szKey, TRUE))
        return FALSE;

    if(!m_rgprop) {
        m_rgprop = new PROP[m_ipropMax = m_dcpropInc];
        memset(m_rgprop, 0, sizeof(PROP) * m_ipropMax);
    }

    if(++m_cprop > m_ipropMax / 2) {
        /* Expand the hash table */
        PROP *rgpropT = m_rgprop;
        DWORD cpropT = m_ipropMax;

        m_rgprop = new PROP[m_ipropMax += m_dcpropInc];
        memset(m_rgprop, 0, sizeof(PROP) * m_ipropMax);
        for(DWORD iprop = 0; iprop < cpropT; ++iprop) {
            if(rgpropT[iprop].szKey) {
                pp = PpropFind(rgpropT[iprop].szKey, FALSE);
                *pp = rgpropT[iprop];
            }
        }
        delete rgpropT;
    }

    pp = PpropFind(szKey, FALSE);
    pp->szKey = _strdup(szKey);
    pp->pvValue = PvAdd(pvValue);
    pp->iOrdinal = m_cprop - 1;
    return TRUE;
}

void CPropertyBag::GetProps(LPCSTR *rgszKeys, PVOID *rgpvValues) const
{
    const PROP *pp = m_rgprop;
    DWORD iprop;

    for(iprop = 0; iprop < m_ipropMax; ++iprop, ++pp) {
        if(pp->szKey) {
            rgszKeys[pp->iOrdinal] = pp->szKey;
            rgpvValues[pp->iOrdinal] = pp->pvValue;
        }
    }
}
