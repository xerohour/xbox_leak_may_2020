//-----------------------------------------------------------------------------
// File: File.cpp
//
// Desc: Common file operations
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "File.h"




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
    m_hFile = CreateFile( strFile, dwAccess, 0, NULL, OPEN_EXISTING,
                          dwAttributes, NULL );
    return( m_hFile != INVALID_HANDLE_VALUE );
}




//-------------------------------------------------------------------------
// Name: Create()
// Desc: Create the given file
//-------------------------------------------------------------------------
BOOL File::Create( const CHAR* strFile, DWORD dwAccess, DWORD dwAttributes )
{
    Close();
    m_hFile = CreateFile( strFile, dwAccess, 0, NULL, CREATE_ALWAYS,
                          dwAttributes, NULL );
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
        USED( bSuccess );
        assert( bSuccess != 0 );
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
    USED( dwPos );
}




//-------------------------------------------------------------------------
// Name: Read()
// Desc: Read data from an open file
//-------------------------------------------------------------------------
BOOL File::Read( VOID* pBuffer, DWORD dwBytesToRead, DWORD& dwBytesRead ) const
{
    assert( IsOpen() );
    return ReadFile( m_hFile, pBuffer, dwBytesToRead, &dwBytesRead, NULL );
}




//-------------------------------------------------------------------------
// Name: Write()
// Desc: Write data to an open file
//-------------------------------------------------------------------------
BOOL File::Write( const VOID* pBuffer, DWORD dwBytesToWrite ) const
{
    assert( IsOpen() );
    DWORD dwBytesWritten;
    BOOL bSuccess = WriteFile( m_hFile, pBuffer, dwBytesToWrite,
                               &dwBytesWritten, NULL );
    return( bSuccess && dwBytesToWrite == dwBytesWritten );
}
