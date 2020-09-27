//-----------------------------------------------------------------------------
// File: File.h
//
// Desc: Common file operations for benchmarking
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef BENCH_STORAGE_FILE_H
#define BENCH_STORAGE_FILE_H

#include <xtl.h>




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
    BOOL  SetEOF() const;

    BOOL  Read( VOID*, DWORD dwBytesToRead, DWORD& dwBytesRead,
                FLOAT& fElapsed ) const;
    BOOL  ReadAsyncNotify( VOID*, DWORD dwBytesToRead, FLOAT& fElapsed, 
                           OVERLAPPED& ) const;
    BOOL  ReadAsyncCallback( VOID*, DWORD dwBytesToRead, FLOAT& fElapsed, 
                             OVERLAPPED&, LPOVERLAPPED_COMPLETION_ROUTINE ) const;

    BOOL  Write( const VOID*, DWORD dwBytesToWrite, FLOAT& fElapsed ) const;
    BOOL  Write( const VOID*, DWORD dwBytesToWrite, DWORD& dwBytesWritten,
                 FLOAT& fElapsed ) const;
    BOOL  WriteAsyncNotify( const VOID*, DWORD dwBytesToWrite, FLOAT& fElapsed,
                            OVERLAPPED& ) const;
    BOOL  WriteAsyncCallback( const VOID*, DWORD dwBytesToWrite, FLOAT& fElapsed,
                              OVERLAPPED&, LPOVERLAPPED_COMPLETION_ROUTINE ) const;

    BOOL  GetOverlappedResult( OVERLAPPED&, DWORD& dwBytesWritten, 
                               FLOAT& fElapsed, BOOL bWait = TRUE ) const;

private:

    BOOL  Read( VOID*, DWORD, DWORD*, FLOAT&, OVERLAPPED* ) const;
    BOOL  ReadEx( VOID*, DWORD, FLOAT&, OVERLAPPED&, 
                  LPOVERLAPPED_COMPLETION_ROUTINE ) const;
    BOOL  Write( const VOID*, DWORD, DWORD*, FLOAT&, OVERLAPPED* ) const;
    BOOL  WriteEx( const VOID*, DWORD, FLOAT&, OVERLAPPED&,
                   LPOVERLAPPED_COMPLETION_ROUTINE ) const;
};




#endif // BENCH_STORAGE_FILE_H
