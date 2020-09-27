//-----------------------------------------------------------------------------
// File: File.h
//
// Desc: Common file operations
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_FILE_H
#define TECH_CERT_GAME_FILE_H

#include "Common.h"




//-------------------------------------------------------------------------
// Name: class File
// Desc: Common file operations
//-------------------------------------------------------------------------
class File
{

    HANDLE m_hFile;

public:

    explicit File( HANDLE = INVALID_HANDLE_VALUE );
    ~File();

    BOOL  Open( const CHAR* strFile, DWORD dwAccess, DWORD dwAttribs = 0 );
    BOOL  Create( const CHAR* strFile, 
                  DWORD dwAccess = GENERIC_WRITE | GENERIC_READ,
                  DWORD dwAttribs = 0 );
    VOID  Close();
    BOOL  IsOpen() const;
    DWORD GetSize() const;
    VOID  SetPos( DWORD ) const;
    BOOL  Read( VOID*, DWORD dwBytesToRead, DWORD& dwBytesRead ) const;
    BOOL  Write( const VOID*, DWORD dwBytesToWrite ) const;

};




#endif // TECH_CERT_GAME_FILE_H
