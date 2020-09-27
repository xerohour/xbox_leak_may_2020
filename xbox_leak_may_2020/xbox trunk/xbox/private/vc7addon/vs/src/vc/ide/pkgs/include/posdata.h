////////////////////////////////////////////////////////////////////////////////
// POSDATA.H

#pragma once

#define MAX_POS_LINE_LEN    2046    // One less to distinguish from -1
#define MAX_POS_LINE        2097151 //  "   "    "    "         "

////////////////////////////////////////////////////////////////////////////////
// POSDATA
//
// This miniscule class holds line and character index data IN 32 BITS and also
// provides conversion to unsigned long (DWORD) for comparison functions.
// POSDATA's can be compared in stream-oriented fashion, since they always
// represent a single character in the stream.

struct POSDATA
{
    union
    {
        struct
        {
            // iChar being first makes it the least significant set of bits
            unsigned long   iChar:11;       // 2047 characters per line max
            unsigned long   iLine:21;       // 2097152 lines per file max
        };

        unsigned long   dwPos;
    };

    POSDATA () {}
    POSDATA (long i) : dwPos(0xffffffff) { }
    POSDATA (const POSDATA &p) { dwPos = p.dwPos; }
    POSDATA (long i, long c) : iLine(i), iChar(c) {}
    //operator DWORD () { return dwPos; }
    BOOL    operator < (const POSDATA &p) const { return dwPos < p.dwPos; }
    BOOL    operator > (const POSDATA &p) const { return dwPos > p.dwPos; }
    BOOL    operator <= (const POSDATA &p) const { return dwPos <= p.dwPos; }
    BOOL    operator >= (const POSDATA &p) const { return dwPos >= p.dwPos; }
    BOOL    operator == (const POSDATA &p) const { return dwPos == p.dwPos; }
    BOOL    operator != (const POSDATA &p) const { return dwPos != p.dwPos; }

    POSDATA & operator = (DWORD dw) { dwPos = dw; return *this; }
    void    SetEmpty () { dwPos = 0xFFFFFFFF; }
    BOOL    IsEmpty () { return dwPos == 0xFFFFFFFF; }

    friend POSDATA operator - (POSDATA p, unsigned long i);
    friend POSDATA operator + (POSDATA p, unsigned long i);
    friend long    operator - (POSDATA p1, POSDATA p2);
    friend long    operator + (POSDATA p1, POSDATA p2);
};

inline  POSDATA operator - (POSDATA p, unsigned long i) { /*ASSERT (p.iChar > i);*/ POSDATA pr; pr.dwPos = p.dwPos - i; return pr; }
inline  POSDATA operator + (POSDATA p, unsigned long i) {/* ASSERT (p.iChar + i <= MAX_POS_LINE_LEN);*/ POSDATA pr; pr.dwPos = p.dwPos + i; return pr; }
inline  long    operator - (POSDATA p1, POSDATA p2) { /*ASSERT (p1.iLine == p2.iLine); ASSERT (p1.iChar >= p2.iChar);*/ return (long)(p1.dwPos - p2.dwPos); }
inline  long    operator + (POSDATA p1, POSDATA p2) { /*ASSERT (FALSE);*/ return 0; }