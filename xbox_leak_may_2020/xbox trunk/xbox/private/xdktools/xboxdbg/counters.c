/*
 *
 * counters.c
 *
 * Performance counter stuff
 *
 */

#include "precomp.h"

HRESULT DmWalkPerformanceCounters(PDM_WALK_COUNTERS *ppdmwc, PDM_COUNTINFO pdmci)
{
    PDM_CONNECTION s;
    HRESULT hr;
    char szBuf[1024];
    struct _DM_WALKPC *pwpc, **ppwpc;

    if(!ppdmwc || !pdmci)
        return E_INVALIDARG;

    if(!*ppdmwc) {
        /* This is our first call, so we need to build a list of all of the
         * counters returned by the remote machine */
        hr = HrOpenSharedConnection(&s);
        if(FAILED(hr))
            return hr;
        hr = DmSendCommand(s, "PCLIST", NULL, NULL);
        if(hr != XBDM_MULTIRESPONSE) {
            if(SUCCEEDED(hr)) {
                TerminateConnection(s);
                hr = E_UNEXPECTED;
            }
            CloseSharedConnection(s);
            return hr;
        }

        /* We've got the connection, so we can allocate a walk structure
         * now */
        *ppdmwc = LocalAlloc(LMEM_FIXED, sizeof (struct _DM_WALK_COUNTERS));
        if(!*ppdmwc) {
            CloseSharedConnection(s);
            return E_OUTOFMEMORY;
        }
        ppwpc = &(*ppdmwc)->pwpcFirst;
        /* Read all of the names and construct a buffer for them all */
        for(;;) {
            DWORD cch = sizeof(szBuf);

            hr = DmReceiveSocketLine(s, szBuf, &cch);
            /* Stop if we can't read or if we're done */
            if(FAILED(hr) || (cch == 1 && szBuf[0] == '.'))
                /* We're done now */
                break;
            pwpc = LocalAlloc(LMEM_FIXED, sizeof (struct _DM_WALKPC));
            if(!pwpc) {
                /* Oh no */
                hr = E_OUTOFMEMORY;
                break;
            }
            *ppwpc = pwpc;
            ppwpc = &pwpc->pwpcNext;
            if(!FGetSzParam(szBuf, "name", pwpc->dmci.Name) ||
                !FGetDwParam(szBuf, "type", &pwpc->dmci.Type))
            {
                hr = E_UNEXPECTED;
                break;
            }
        }
        if(FAILED(hr))
            TerminateConnection(s);
        CloseSharedConnection(s);
        *ppwpc = NULL;
        if(FAILED(hr))
            return hr;
    }

    /* We've got our list, so return a line */
    pwpc = (*ppdmwc)->pwpcFirst;
    if(!pwpc)
        return XBDM_ENDOFLIST;
    (*ppdmwc)->pwpcFirst = pwpc->pwpcNext;

    *pdmci = pwpc->dmci;
    LocalFree(pwpc);

    return XBDM_NOERR;
}

HRESULT DmCloseCounters(PDM_WALK_COUNTERS pdmwc)
{
    struct _DM_WALKPC *pwpc;

    if(!pdmwc)
        return E_INVALIDARG;

    while(pdmwc->pwpcFirst) {
        pwpc = pdmwc->pwpcFirst;
        pdmwc->pwpcFirst = pwpc->pwpcNext;
        LocalFree(pwpc);
    }
    LocalFree(pdmwc);
    return XBDM_NOERR;
}

HRESULT DmQueryPerformanceCounter(LPCSTR szName, DWORD dwType, PDM_COUNTDATA pdmcd)
{
    HRESULT hr;
    char sz[512];
    PDM_CONNECTION s;

    if (!pdmcd || !szName)
        return E_INVALIDARG;

    hr = HrOpenSharedConnection(&s);
    if(FAILED(hr))
        return hr;
    sprintf(sz, "QUERYPC NAME=\"%s\" TYPE=0x%08x", szName, dwType);
    hr = DmSendCommand(s, sz, NULL, NULL);
    if(hr != XBDM_MULTIRESPONSE) {
        if(SUCCEEDED(hr)) {
            TerminateConnection(s);
            hr = E_UNEXPECTED;
        }
        CloseSharedConnection(s);
        return hr;
    }
    memset(pdmcd, 0, sizeof *pdmcd);
    for(;;) {
        DWORD cch = sizeof sz;

        hr = DmReceiveSocketLine(s, sz, &cch);
        if(FAILED(hr) || *sz == '.')
            break;
        FGetDwParam(sz, "type", &pdmcd->CountType);
        FGetDwParam(sz, "vallo", &pdmcd->CountValue.LowPart);
        FGetDwParam(sz, "valhi", &pdmcd->CountValue.HighPart);
        FGetDwParam(sz, "ratelo", &pdmcd->RateValue.LowPart);
        FGetDwParam(sz, "ratehi", &pdmcd->RateValue.HighPart);
    }

    CloseSharedConnection(s);
    return hr;
}

HRESULT DmEnableGPUCounter(BOOL f)
{
    char sz[64];

    sprintf(sz, "GPUCOUNT %s", f ? "ENABLE" : "DISABLE");
    return HrOneLineCmd(sz);
}
