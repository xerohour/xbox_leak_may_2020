//-----------------------------------------------------------------------------
// File: XbRandName.h
//
// Desc: Generates a random and somewhat readable name. Useful for fake
//       player names, session names, etc.
//
// Hist: 10.10.01 - New for Nov release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBRANDNAME_H
#define XBRANDNAME_H

#include <xtl.h>




//-----------------------------------------------------------------------------
// Name: class CXBRandName
// Desc: Random name 
//-----------------------------------------------------------------------------
class CXBRandName
{
    
    mutable LONG m_nValue;

public:

    explicit CXBRandName( DWORD dwSeed = 0 );
    ~CXBRandName();

    VOID SetSeed( DWORD dwSeed );
    VOID GetName( WCHAR* strName, DWORD dwSize ) const;

private:

    DWORD GetRand() const;
    WCHAR GetRandVowel() const;
    WCHAR GetRandConsonant() const;
    VOID  AppendConsonant( WCHAR* strRandom, BOOL bLeading ) const;
    VOID  AppendVowel( WCHAR* strRandom ) const;

    // Not used, so not defined
    CXBRandName( const CXBRandName& );

};

#endif // XBRANDNAME_H
