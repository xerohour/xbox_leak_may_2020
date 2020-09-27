/*
 *
 * xsum.h
 *
 * CCheckSum
 *
 */

#ifndef _XSUM_H
#define _XSUM_H

class CCheckSum {
public:
    CCheckSum(void) { Reset(); }
    CCheckSum(DWORD dw) : m_dwSum(dw) {}

    void Reset(DWORD dw = -1) { m_dwSum = dw; }
    DWORD DwRunningSum(void) const { return m_dwSum; }
    DWORD DwFinalSum(void) const { return m_dwSum ^ -1; }
    void SumBytes(const BYTE *lpb, DWORD cb)
    {
        while(cb--)
            m_dwSum = (m_dwSum >> 8) ^ m_CrcTable32[(BYTE)m_dwSum ^ *lpb++];
    }

private:
    DWORD m_dwSum;
    static const DWORD m_CrcTable32[256];
};

#endif // _XSUM_H
