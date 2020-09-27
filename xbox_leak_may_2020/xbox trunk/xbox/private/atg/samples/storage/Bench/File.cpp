//-----------------------------------------------------------------------------
// File: File.cpp
//
// Desc: Common file operations for benchmarking
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "File.h"
#include <XbStopWatch.h>
#include <cassert>




//-------------------------------------------------------------------------
// Name: File()
// Desc: Construct file object
//-------------------------------------------------------------------------
File::File( HANDLE hFile )
: 
    m_hFile( hFile )
{
}




//-------------------------------------------------------------------------
// Name: ~File()
// Desc: Close file object
//-------------------------------------------------------------------------
File::~File()
{
    Close();
}




//-------------------------------------------------------------------------
// Name: Open()
// Desc: Open the given file; fails if file doesn't exist
//-------------------------------------------------------------------------
BOOL File::Open( const CHAR* strFile, DWORD dwAccess, DWORD dwAttributes )
{
    Close();
    m_hFile = CreateFile( strFile, dwAccess, FILE_SHARE_READ, NULL, 
                          OPEN_EXISTING, dwAttributes, NULL );
    return( m_hFile != INVALID_HANDLE_VALUE );
}




//-------------------------------------------------------------------------
// Name: Create()
// Desc: Create the given file
//-------------------------------------------------------------------------
BOOL File::Create( const CHAR* strFile, DWORD dwAccess, DWORD dwAttributes )
{
    Close();
    m_hFile = CreateFile( strFile, dwAccess, FILE_SHARE_READ, NULL, 
                          CREATE_ALWAYS, dwAttributes, NULL );
    return( m_hFile != INVALID_HANDLE_VALUE );
}




//-------------------------------------------------------------------------
// Name: Close()
// Desc: Close the file handle
//-------------------------------------------------------------------------
VOID File::Close()
{
    if( m_hFile != INVALID_HANDLE_VALUE )
    {
        BOOL bSuccess = CloseHandle( m_hFile );
        assert( bSuccess != 0 );
        (VOID)bSuccess;
        m_hFile = INVALID_HANDLE_VALUE;
    }
}




//-------------------------------------------------------------------------
// Name: IsOpen()
// Desc: TRUE if file is open
//-------------------------------------------------------------------------
BOOL File::IsOpen() const
{
    return( m_hFile != INVALID_HANDLE_VALUE );
}




//-------------------------------------------------------------------------
// Name: GetSize()
// Desc: Returns the file size in bytes
//-------------------------------------------------------------------------
DWORD File::GetSize() const
{
    assert( IsOpen() );
    return GetFileSize( m_hFile, NULL );
}




//-------------------------------------------------------------------------
// Name: SetPos()
// Desc: Set the file pointer to the given position (relative to the
//       beginning of the file)
//-------------------------------------------------------------------------
VOID File::SetPos( DWORD dwFilePos ) const
{
    assert( IsOpen() );
    assert( LONG(dwFilePos) >= 0 );
    DWORD dwPos = SetFilePointer( m_hFile, LONG(dwFilePos), NULL, FILE_BEGIN );
    assert( dwPos == dwFilePos );
    (VOID)dwPos;
}




//-------------------------------------------------------------------------
// Name: SetEOF()
// Desc: Resize the file to the current position
//-------------------------------------------------------------------------
BOOL File::SetEOF() const
{
    assert( IsOpen() );
    return SetEndOfFile( m_hFile );
}




//-------------------------------------------------------------------------
// Name: Read()
// Desc: Read data from an open file (synchronous)
//-------------------------------------------------------------------------
BOOL File::Read( VOID* pBuffer, DWORD dwBytesToRead, DWORD& dwBytesRead,
                 FLOAT& fElapsed ) const
{
    return Read( pBuffer, dwBytesToRead, &dwBytesRead, fElapsed, NULL );
}




//-------------------------------------------------------------------------
// Name: ReadAsyncNotify()
// Desc: Read data from an open file asynchronously with notification
//-------------------------------------------------------------------------
BOOL File::ReadAsyncNotify( VOID* pBuffer, DWORD dwBytesToRead, 
                            FLOAT& fElapsed, OVERLAPPED& Overlapped ) const
{
    return Read( pBuffer, dwBytesToRead, NULL, fElapsed, &Overlapped );
}




//-------------------------------------------------------------------------
// Name: ReadAsyncCallback()
// Desc: Read data from an open file asynchronously with callback
//-------------------------------------------------------------------------
BOOL File::ReadAsyncCallback( VOID* pBuffer, DWORD dwBytesToRead, 
                              FLOAT& fElapsed, OVERLAPPED& Overlapped,
                              LPOVERLAPPED_COMPLETION_ROUTINE pCallback ) const
{
    return ReadEx( pBuffer, dwBytesToRead, fElapsed, Overlapped, pCallback );
}




//-------------------------------------------------------------------------
// Name: Write()
// Desc: Write data synchronously. Returns FALSE if all bytes could not
//       be written.
//-------------------------------------------------------------------------
BOOL File::Write( const VOID* pBuffer, DWORD dwBytesToWrite, FLOAT& fElapsed ) const
{
    DWORD dwBytesWritten;
    BOOL bSuccess = Write( pBuffer, dwBytesToWrite, &dwBytesWritten, fElapsed, NULL );
    return( bSuccess && dwBytesToWrite == dwBytesWritten );
}




//-------------------------------------------------------------------------
// Name: Write()
// Desc: Write data synchronously
//-------------------------------------------------------------------------
BOOL File::Write( const VOID* pBuffer, DWORD dwBytesToWrite, DWORD& dwBytesWritten,
                  FLOAT& fElapsed ) const
{
    return Write( pBuffer, dwBytesToWrite, &dwBytesWritten, fElapsed, NULL );
}




//-------------------------------------------------------------------------
// Name: WriteAsyncNotify()
// Desc: Write asynchronously with notification
//-------------------------------------------------------------------------
BOOL File::WriteAsyncNotify( const VOID* pBuffer, DWORD dwBytesToWrite, 
                             FLOAT& fElapsed, OVERLAPPED& Overlapped ) const
{
    return Write( pBuffer, dwBytesToWrite, NULL, fElapsed, &Overlapped );
}




//-------------------------------------------------------------------------
// Name: WriteAsyncCallback()
// Desc: Write asynchronously with callback
//-------------------------------------------------------------------------
BOOL File::WriteAsyncCallback( const VOID* pBuffer, DWORD dwBytesToWrite,
                               FLOAT& fElapsed, OVERLAPPED& Overlapped, 
                               LPOVERLAPPED_COMPLETION_ROUTINE pCallback ) const
{
    return WriteEx( pBuffer, dwBytesToWrite, fElapsed, Overlapped, pCallback );
}




//-------------------------------------------------------------------------
// Name: GetOverlappedResult()
// Desc: Determine if async operation has completed
//-------------------------------------------------------------------------
BOOL File::GetOverlappedResult( OVERLAPPED& Overlapped, 
                                DWORD& dwBytesWritten, FLOAT& fElapsed,
                                BOOL bWait ) const
{
    assert( IsOpen() );

    CXBStopWatch timer( TRUE );
    BOOL bSuccess = ::GetOverlappedResult( m_hFile, &Overlapped, 
                                           &dwBytesWritten, bWait );
    fElapsed = timer.GetElapsedSeconds();
    return bSuccess;
}




//-------------------------------------------------------------------------
// Name: Read()
// Desc: Read data -- wrapper around ReadFile()
//-------------------------------------------------------------------------
BOOL File::Read( VOID* pBuffer, DWORD dwBytesToRead, DWORD* pdwBytesRead,
                 FLOAT& fElapsed, OVERLAPPED* pOverlapped ) const
{
    assert( IsOpen() );
    assert( pBuffer != NULL );
    CXBStopWatch timer( TRUE );
    BOOL bSuccess = ReadFile( m_hFile, pBuffer, dwBytesToRead, 
                              pdwBytesRead, pOverlapped );
    fElapsed = timer.GetElapsedSeconds();
    return bSuccess;
}




//-------------------------------------------------------------------------
// Name: ReadEx()
// Desc: Read data asynchronously -- wrapper around ReadFileEx()
//-------------------------------------------------------------------------
BOOL File::ReadEx( VOID* pBuffer, DWORD dwBytesToRead, 
                   FLOAT& fElapsed, OVERLAPPED& Overlapped, 
                   LPOVERLAPPED_COMPLETION_ROUTINE pCallback ) const
{
    assert( IsOpen() );
    assert( pBuffer != NULL );
    assert( pCallback != NULL );

    CXBStopWatch timer( TRUE );
    BOOL bSuccess = ReadFileEx( m_hFile, pBuffer, dwBytesToRead, 
                                &Overlapped, pCallback );
    fElapsed = timer.GetElapsedSeconds();
    return bSuccess;
}




//-------------------------------------------------------------------------
// Name: Write()
// Desc: Write data -- wrapper around WriteFile
//-------------------------------------------------------------------------
BOOL File::Write( const VOID* pBuffer, DWORD dwBytesToWrite, 
                  DWORD* pdwBytesWritten, FLOAT& fElapsed, 
                  OVERLAPPED* pOverlapped ) const
{
    assert( IsOpen() );
    assert( pBuffer != NULL );

    CXBStopWatch timer( TRUE );
    BOOL bSuccess = WriteFile( m_hFile, pBuffer, dwBytesToWrite, 
                               pdwBytesWritten, pOverlapped );
    fElapsed = timer.GetElapsedSeconds();
    return bSuccess;
}




//-------------------------------------------------------------------------
// Name: WriteEx()
// Desc: Write data asynchronously -- wrapper around WriteFileEx
//-------------------------------------------------------------------------
BOOL File::WriteEx( const VOID* pBuffer, DWORD dwBytesToWrite, 
                    FLOAT& fElapsed, OVERLAPPED& Overlapped,
                    LPOVERLAPPED_COMPLETION_ROUTINE pCallback ) const
{
    assert( IsOpen() );
    assert( pBuffer != NULL );
    assert( pCallback != NULL );

    CXBStopWatch timer( TRUE );
    BOOL bSuccess = WriteFileEx( m_hFile, pBuffer, dwBytesToWrite, 
                                 &Overlapped, pCallback );
    fElapsed = timer.GetElapsedSeconds();
    return bSuccess;
}
