#include "dnproti.h"

//	Now, a little bit of probably unnecesary junk for our lower edge

/*
 * DNP_QueryInterface
 */
#undef DPF_MODNAME
#define DPF_MODNAME "DNSP_QueryInterface"

STDMETHODIMP DNSP_QueryInterface(
				IDP8SPCallback	*pDNPI,
                REFIID riid,
                LPVOID *ppvObj )
{
	HRESULT hr;

    *ppvObj = NULL;


        ((PDNPI_INT) pDNPI)->dwRefCnt++;
        *ppvObj = pDNPI;
		hr = S_OK;


	return hr;

} /* DNP_QueryInterface */


/*
 * DNP_AddRef
 */
#undef DPF_MODNAME
#define DPF_MODNAME "DNSP_AddRef"

STDMETHODIMP_(ULONG) DNSP_AddRef( IDP8SPCallback *pDNPI)
{
    ((PDNPI_INT) pDNPI)->dwRefCnt++;
    return ((PDNPI_INT) pDNPI)->dwRefCnt;
} /* DNP_AddRef */



/*
 * DNP_Release
 */
#undef DPF_MODNAME
#define DPF_MODNAME "DNSP_Release"

STDMETHODIMP_(ULONG) DNSP_Release( IDP8SPCallback *pDNPI )
{
    ((PDNPI_INT) pDNPI)->dwRefCnt--;

    return ((PDNPI_INT) pDNPI)->dwRefCnt;
} /* DNP_Release */

IDP8SPCallbackVtbl DNPLowerEdgeVtbl =
{
        DNSP_QueryInterface,
        DNSP_AddRef,
        DNSP_Release,
		DNSP_IndicateEvent,
		DNSP_CommandComplete
};


