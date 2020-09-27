//-----------------------------------------------------------------------------
// File: BenchStorage.h
//
// Desc: Benchmark Xbox storage
//
// Hist: 04.25.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XB_BENCH_STORAGE_H
#define XB_BENCH_STORAGE_H

#include <xtl.h>
#include <XbApp.h>
#include <XbFont.h>
#include <XbMemUnit.h>
#include <XbStopWatch.h>

class File;
class TestFile;
class ResultsFile;
class FileList;
class OpenFileList;




//-----------------------------------------------------------------------------
// Name: class BenchStorage
// Desc: Application object for Xbox storage benchmarking
//-----------------------------------------------------------------------------
class BenchStorage : public CXBApplication
{
    CXBPackedResource   m_xprResource;          // Packed resource for the app
    CXBFont             m_Font;                 // Font renderer
    WCHAR               m_strStatusHdr[96];     // Benchmark status header
    WCHAR               m_strStatus[96];        // Benchmark statusf
    CXBStopWatch        m_StatusTimer;          // Don't display status too often
    CXBMemUnit          m_MemUnit;              // MU
    OVERLAPPED          m_Overlapped;           // struct for async read/write
    DWORD               m_dwBytesTransferred;   // for async read/write
    DWORD               m_dwHdBufferSize;       // best HD buffer size
    DWORD               m_dwDefaultCacheSize;   // default file system cache size
    DWORD               m_dwSeed;               // RNG seed
    CHAR                m_chHardDrive;          // Hard drive logical partition
    CHAR                m_chMemUnit;            // MU logical partition
    CHAR                m_chDvd;                // DVD drive

public:

    enum DeviceType
    {
        Device_MU,
        Device_HD,
        Device_DVD,

        Device_HD_User,
        Device_HD_Persist,
        Device_HD_Utility
    };

public:

    BenchStorage();

    virtual HRESULT Initialize();

    VOID RunBenchSuite();
    VOID ShowComplete();

private:

    CHAR FindBestMu();

    VOID RunGetDeviceChangesSuite();
    VOID RunFileBuffersSuite();
    VOID RunAsyncSuite();
    VOID RunApiComparisonSuite();
    VOID RunRandomSeekSuite();
    VOID RunSequentialSeekSuite();
    VOID RunRandomAccessSuite();
    VOID RunSequentialAccessSuite();
    VOID RunCacheSuite();
    VOID RunRegionSuite();
    VOID RunEnumerationSuite();
    VOID RunScatterGatherSuite();
    VOID RunInterleavedSuite();

    VOID FileBuffersRead( DeviceType );
    VOID FileBuffersWrite( DeviceType );

    VOID AsyncRead( DeviceType );
    VOID AsyncReadNotify( DeviceType );
    VOID AsyncReadCallback( DeviceType );
    VOID AsyncWrite( DeviceType );
    VOID AsyncWriteNotify( DeviceType );
    VOID AsyncWriteCallback( DeviceType );

    VOID ApiCompare( DeviceType );
    VOID ApiOpen( DeviceType );
    VOID ApiClose( DeviceType );
    VOID ApiRead( DeviceType );
    VOID ApiWrite( DeviceType );
    VOID ApiSeek( DeviceType );

    VOID RandomSeek( DeviceType );
    VOID SeqSeek( DeviceType );

    VOID RandomRead( DeviceType );
    FLOAT RandomRead( const TestFile&, DWORD, DWORD, const WCHAR*, DWORD = 0 );
    VOID RandomWrite( DeviceType );
    FLOAT RandomWrite( const TestFile&, DWORD, DWORD, const WCHAR*, DWORD = 0 );

    VOID SeqRead( DeviceType );
    FLOAT SeqRead( const TestFile&, DWORD, DWORD, const WCHAR*, DWORD = 0 );
    VOID SeqWrite( DeviceType );
    FLOAT SeqWrite( const TestFile&, DWORD, DWORD, const WCHAR*, DWORD = 0 );

    VOID CacheRead( DeviceType );
    VOID CacheRandomRead( DeviceType );
    VOID CacheSeqRead( DeviceType );
    VOID CacheWrite( DeviceType );
    VOID CacheRandomWrite( DeviceType );
    VOID CacheSeqWrite( DeviceType );

    VOID RegionRead();
    VOID RegionRead( DeviceType );
    VOID RegionWrite();
    VOID RegionWrite( DeviceType );

    VOID EnumSaves( DeviceType );
    VOID EnumDirs( DeviceType );
    static DWORD EnumFiles( const CHAR* );
    VOID EnumSoundtracks();
    VOID EnumNicknames();
    VOID EnumOpen( DeviceType );
    static FLOAT EnumFiles( const CHAR*, FileList& );
    static FLOAT EnumFiles( const CHAR*, OpenFileList& );

    VOID EnumDirSize( DeviceType );

    VOID Scatter( DeviceType );
    VOID Gather( DeviceType );
    VOID InterleavedRead();
    VOID InterleavedWrite();

    static VOID CALLBACK BytesTransferred( DWORD, DWORD, OVERLAPPED* );
    VOID OnBytesTransferred( DWORD );

    BOOL IsValidDevice( DeviceType ) const;
    CHAR GetDrive( DeviceType ) const;
    VOID ShowStatus( FLOAT );
    VOID ShowStatus( DWORD, FLOAT );
    VOID ShowStatus( BOOL = FALSE );
    BOOL Prepare( DeviceType, TestFile&, ResultsFile&, const CHAR*, const CHAR*,
                  const WCHAR*, const CHAR*, const CHAR* = NULL, const CHAR* = NULL,
                  const CHAR* = NULL );
};

#endif // XB_BENCH_STORAGE_H
