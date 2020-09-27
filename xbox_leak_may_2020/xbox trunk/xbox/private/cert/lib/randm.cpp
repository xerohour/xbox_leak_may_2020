/*
 *
 * randm.cpp
 *
 * Random numbers
 *
 */

#include "precomp.h"

CRandom g_rand;

const ULONG CRandom::rgulMults[] = {
    0x52F690D5,
    0x534D7DDE,
    0x5B71A70F,
    0x66793320,
    0x9B7E5ED5,
    0xA465265E,
    0xA53F1D11,
    0xB154430F,
};

CRandom::CRandom(void)
{
    FILETIME ft;

    /* If this call fails, we still have random data in ft */
    GetSystemTimeAsFileTime(&ft);
    Seed(ft.dwHighDateTime ^ ft.dwLowDateTime);
}

void CRandom::Seed(ULONG ulSeed)
{
    m_ulMask = 0;
    m_ulMult = rgulMults[ulSeed & 7];
    m_ulCur = ulSeed;
    m_ulMask = Rand();
}

ULONG CRandom::Rand(void)
{
    ULARGE_INTEGER li;

    /* We're not a true 32-bit generator, since 2^32+1 isn't prime, so we use
     * 2^32-5 instead */
    li.QuadPart = m_ulCur;
    li.QuadPart += 1;
    li.QuadPart *= m_ulMult;
    li.QuadPart %= 0xFFFFFFFB;
    m_ulCur = li.LowPart;
    return m_ulCur ^ m_ulMask;
}

void CRandom::RandBytes(LPBYTE pb, DWORD cb)
{
    PUSHORT pus;

    if(cb == 0)
        return;
    if((ULONG)pb & 1) {
        *pb++ = (BYTE)(Rand() >> 14);
        --cb;
    }
    pus = (PUSHORT)pb;
    while(cb > 1) {
        *pus++ = (USHORT)(Rand() >> 8);
        cb -= 2;
    }
    if(cb)
        *pb = (BYTE)(Rand() >> 12);
}
