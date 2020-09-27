#include "xkdctest.h"
#include <stdlib.h>


HRESULT HexCharToByte(
    IN char hex, 
    OUT BYTE *byte)
{
    BYTE t = 0;

    if (hex >= '0' && hex <= '9')
        t = hex - '0';
    else if (hex >= 'a' && hex <= 'f')
        t = hex - 'a' + 10;
    else if (hex >= 'A' && hex <= 'F')
        t = hex - 'A' + 10;
    else
        return E_FAIL;

    *byte = t;
    return S_OK;
}


HRESULT HexPairToByte(
    IN char *hex,
    OUT BYTE *byte)
{
    HRESULT hr = S_OK;
    if (SUCCEEDED(hr = HexCharToByte(hex[0], byte)))
    {
        BYTE b;;
        if (SUCCEEDED(hr = HexCharToByte(hex[1], &b)))
        {
            *byte = (*byte << 4) | b;
        }
    }

    return hr;
}


DWORD ToIP(char *szIP)
{
    if (!szIP || szIP[0] == 0) return 0;

    DWORD IP = 0;

    ASSERT(szIP[1]);
    if (szIP[0] == '0' && (szIP[1] == 'x' || szIP[1] == 'X') && (strlen(szIP) == 10))
    {
        // The IP is in Hex form
        szIP += 2;

        for (INT i = 0; i < 4; i++)
        {
            if (HexPairToByte(szIP, ((BYTE*)&IP) + (3-i)) != S_OK)
            {
                return E_FAIL;
            }
            szIP += 2;
        }
    }
    else
    {
        BOOL fIsIP = TRUE;
        for (UINT i = 0; i < strlen(szIP); i++)
        {
            if ((szIP[i] < '0' || szIP[i] > '9') && szIP[i] != '.')
            {
                fIsIP = FALSE;
                break;
            }
        }

        if (fIsIP)
        {
            // The IP is in decimal dot form
            IP = atoi(szIP);
            szIP = strchr(szIP, '.') + 1;
            IP |= atoi(szIP) << 8;
            szIP = strchr(szIP, '.') + 1;
            IP |= atoi(szIP) << 16;
            szIP = strchr(szIP, '.') + 1;
            IP |= atoi(szIP) << 24;
        }
        else
        {
            // We don't do DNS lookup
            IP = 0;
        }
    }

    return IP;
}

