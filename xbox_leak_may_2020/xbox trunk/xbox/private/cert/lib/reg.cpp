/*
 *
 * reg.cpp
 *
 * Registry stuff
 *
 */

#include "precomp.h"

CVarBag *PbagFromRegKey(HKEY hkey)
{
    DWORD iValue;
    DWORD cValues;
    DWORD cchKeyMax;
    LPSTR szKey;
    char szValue[256];
    DWORD cchValue;
    DWORD cchKey;
    DWORD dwType;
    CVarBag *pbag;
    VPR vpr;

    if(ERROR_SUCCESS != RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, NULL,
            NULL, &cValues, &cchKeyMax, NULL, NULL, NULL))
        return NULL;
    szKey = (LPSTR)_alloca(cchKeyMax + 1);
    pbag = new CVarBag;
    for(iValue = 0; iValue < cValues; ++iValue) {
        cchValue = sizeof szValue;
        cchKey = cchKeyMax + 1;
        if(ERROR_SUCCESS != RegEnumValue(hkey, iValue, szKey, &cchKey, NULL,
            &dwType, (LPBYTE)szValue, &cchValue))
        {
            delete pbag;
            return NULL;
        }
        if(dwType == REG_SZ) {
            vpr.vpType = vpSz;
            vpr.sz = szValue;
        } else if(dwType == REG_DWORD) {
            vpr.vpType = vpDw;
            vpr.dw = *(DWORD *)szValue;
        } else
            continue;
        pbag->FAddProp(szKey, &vpr);
        vpr.vpType = vpNone;
    }

    return pbag;
}
