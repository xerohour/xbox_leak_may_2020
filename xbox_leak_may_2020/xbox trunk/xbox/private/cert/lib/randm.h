/*
 *
 * randm.h
 *
 * Random numbers
 *
 */

#ifndef _RANDM_H
#define _RANDM_H

#include <time.h>

class CRandom
{
public:
    CRandom(void);

    ULONG Rand(void);
    void RandBytes(LPBYTE pb, DWORD cb);
private:
    static const ULONG rgulMults[];
    void Seed(ULONG ulSeed);
    ULONG m_ulMult;
    ULONG m_ulCur;
    ULONG m_ulMask;
};

extern CRandom g_rand;

#endif // _RANDM_H