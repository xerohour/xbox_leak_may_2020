// ---------------------------------------------------------------------------------------
// ipqos.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

#ifdef XNET_FEATURE_QOS

// ---------------------------------------------------------------------------------------
// Trace Tags
// ---------------------------------------------------------------------------------------

DefineTag(qosInfo, 0);
DefineTag(qosWarn, TAG_ENABLE);

// ---------------------------------------------------------------------------------------
// CXnIp (IpQos)
// ---------------------------------------------------------------------------------------

INT CXnIp::IpQosListen(const XNKID * pxnkid, const BYTE * pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags)
{
    ICHECK(IP, USER);

    RaiseToDpc();

#if 0
    CKeyReg * pKeyReg = KeyRegLookup(pxnkid);

    if (pKeyReg == NULL)
    {
        TraceSz1(qosWarn, "IpQosListen - XNKID %s is not registered",
                 HexStr(pxnkid->ab, sizeof(pxnkid->ab)));
        return(WSAEINVAL);
    }

    if (dwFlags & XNET_QOS_LISTEN_SET_DATA)
    {
        if (cb > (UINT)(cfgQosDataLimitDiv4 * 4))
        {
            TraceSz3(qosWarn, "IpQosListen - cb is %d bytes but cfgQosDataLimit is %d (%d bytes)",
                     cb, cfgQosDataLimitDiv4, cfgQosDataLimitDiv4 * 4);
            return(WSAEMSGSIZE);
        }

        BYTE * pbQos = NULL;

        if (cb > 0)
        {
            pbQos = (BYTE *)SysAlloc(cb, PTAG_QosData);

            if (pbQos == NULL)
            {
                TraceSz1(qosWarn, "IpSetListen - Out of memory allocating QosData (%d bytes)", cb);
                return(WSAENOBUFS);
            }

            memcpy(pbQos, pb, cb);
        }

        SysFree(pKeyReg->_pbQos);

        pKeyReg->_pbQos = pbQos;
        pKeyReg->_cbQos = cb;
    }

    if (dwFlags & XNET_QOS_LISTEN_SET_BITSPERSEC)
    {
        //@@@ Implement me
    }

    if (dwFlags & XNET_QOS_LISTEN_ENABLE)
    {
        //@@@ Implement me
    }

    if (dwFlags & XNET_QOS_LISTEN_DISABLE)
    {
        //@@@ Implement me
    }
#endif

    return(0);
}

INT CXnIp::IpQosXnAddr(UINT cxnqos, const XNADDR * apxna[], const XNKID * apxnkid[], const XNKEY * apxnkey[], DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos)
{
    ICHECK(IP, USER);

    if (cxnqos == 0)
    {
        TraceSz(qosWarn, "IpQosXnAddr - Must specify one or more addresses");
        return(WSAEFAULT);
    }

    //@@@ Implement me

    return(WSAEACCES);
}

#ifdef XNET_FEATURE_SG

INT CXnIp::IpQosServer(UINT cxnqos, const IN_ADDR aina[], const DWORD adwServiceId[], DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos)
{
    ICHECK(IP, USER);

    if (cxnqos == 0)
    {
        TraceSz(qosWarn, "IpQosServer - Must specify one or more addresses");
        return(WSAEFAULT);
    }

    //@@@ Implement me

    return(WSAEACCES);
}

#endif

INT CXnIp::IpQosRelease(XNQOS * pxnqos)
{
    ICHECK(IP, USER);

    //@@@ Implement me

    return(WSAEACCES);
}

#endif
