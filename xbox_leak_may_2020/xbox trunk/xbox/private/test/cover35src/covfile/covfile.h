
#ifndef _COVFILE_H_
#define _COVFILE_H_

//
//  Coverage data file is stored as follows:
//
//  COVFILE_HEADER at offset zero in the file.
//  First COVFILE_DIRECTORY entry immediately follows COVFILE_HEADER.
//  Next COVFILE_DIRECTORY entry follows previous entry, etc.
//       (add entry's dwLengthOfEntry to its offset for offset of next entry)
//  Last COVFILE_DIRECTORY entry is empty with dwLengthOfEntry set to zero.
//
//  Order of directory entries is currently random to facilitate speedy
//  insertion of new record (simply appended to end).
//
//  Directory chain is contiguous and is contained within the first
//  dwHeaderLength bytes of the file (multiple of 512 bytes).  First data
//  vector is stored at offset dwHeaderBytes, so directory can grow up to
//  dwHeaderBytes without moving any of the data.
//
//  When directory needs to grow beyond dwHeaderBytes, it will grow by
//  ROUNDUP2( BITVECTOR_STORAGE_SIZE( dwBlockCount ), 512 ) bytes, and that
//  much of the data will be moved from offset at dwHeaderBytes to end of
//  the file (corresponding offsets in directory will be adjusted).
//
//  To facilitate fewer disk sector reads and writes to maintain the data
//  file, data vectors will be stored such that they do not span any
//  ROUNDUP2( BITVECTOR_STORAGE_SIZE( dwBlockCount ), 512 ) byte boundaries.
//  This will guarantee that growing the header by that many bytes will never
//  chop a stored data vector.
//
//  For COVFILE_VERSION 2.01 (0x00020001), all vectors are stored as bit
//  vectors of length dwBlockCount.  The allocation size for such a vector is
//  computed as dwBlockCount rounded up to multiple of 8 then divided by 8
//  (number of bytes) then rounded up to multiple of 4 bytes (dword alignment).
//
//  For version 2.01, 32-bit offsets are used implying a maximum file size of
//  2^32-1 (4GB) bytes.  For a binary that contains 100,000 blocks (a very
//  large binary), each vector would require 12,800 bytes of storage in
//  addition to the directory information for that vector.  For an average
//  name length of 64 bytes, the directory entry is 80 bytes, so roughly
//  12,880 bytes of storage would be required per named vector.  This yields
//  roughly 333,460 maximum named vectors that could be stored for a 100,000
//  block binary (4 gigabytes).
//
//  For version 2.02, 64-bit offsets are used implying a maximum file size of
//  2^64-2 bytes for the data vectors, but the directory chain contained in
//  the header is still limited to 2^32 bytes.  For an average name length of
//  64 bytes, which is 80 bytes per directory entry, this yields space for
//  roughly 50,000,000 test names.
//
//  While the file format will support directory chains of 2^32 bytes, the
//  current implementation of the coverage monitoring service (covermon.exe)
//  caches the entire directory chain for each binary in memory, and walks
//  the chain linearly to search for a matching test name each time coverage
//  information is saved.  This limits the "usable" size of the directory
//  chain (combined for all active .covdata files) to available RAM.  Future
//  implementations of the coverage monitoring service will likely address
//  this limitation by storing only the directory name hashes in memory in a
//  btree format, increasing the "usable" limit to roughly 50,000,000 test
//  names of any length.
//

#ifndef ROUNDUP2
#define ROUNDUP2( x, n ) ((((ULONG)(x)) + (((ULONG)(n)) - 1 )) & ~(((ULONG)(n)) - 1 ))
#endif

#define COVFILE_HEADER_MIN_SIZE  512        // must be power of 2
#define COVFILE_VECTOR_BOUNDARY  512        // must be power of 2

#define BITVECTOR_STORAGE_SIZE( NumberOfBits ) \
           ( ROUNDUP2( ROUNDUP2(( NumberOfBits ), 8 ) / 8, 4 ))

#define COVFILE_SIGNATURE   ( 0xDADAC0BB )    // "BBC0DADA" (BB Cover Data)
#define COVFILE_VERSION     ( 0x00020003 )    // rev this if change structure
#define COVFILE_VERSION_202 ( 0x00020002 )    // older version
#define COVFILE_VERSION_201 ( 0x00020001 )    // older version

typedef struct _COVFILE_HEADER        COVFILE_HEADER,        *PCOVFILE_HEADER;
typedef struct _COVFILE_HEADER_202    COVFILE_HEADER_202,    *PCOVFILE_HEADER_202;
typedef struct _COVFILE_DIRECTORY     COVFILE_DIRECTORY,     *PCOVFILE_DIRECTORY;
typedef struct _COVFILE_DIRECTORY_201 COVFILE_DIRECTORY_201, *PCOVFILE_DIRECTORY_201;

struct _COVFILE_HEADER {
    DWORD dwSignature;          // always COVFILE_SIGNATURE
    DWORD dwVersion;            // always COVFILE_VERSION
    DWORD dwCheckSum;           // checksum of original binary
    DWORD dwBlockCount;         // number of basic blocks (bit vector length)
    DWORD dwHeaderLength;       // multiple of 512, includes entire directory
	DWORD dwOffsetOfFirstDir;
	DWORD dwOffsetOfGUID;
    };

struct _COVFILE_HEADER_202 {
    DWORD dwSignature;          // always COVFILE_SIGNATURE
    DWORD dwVersion;            // always COVFILE_VERSION
    DWORD dwCheckSum;           // checksum of original binary
    DWORD dwBlockCount;         // number of basic blocks (bit vector length)
    DWORD dwHeaderLength;       // multiple of 512, includes entire directory
    };

struct _COVFILE_DIRECTORY_201 {
    DWORD dwLengthOfEntry;      // multiple of 4, zero indicates end of list
    DWORD dwHashOfName;         // to make searching for matches faster
    DWORD dwOffsetOfData;       // size is BITVECTOR_STORAGE_SIZE( dwBlockCount )
    CHAR  szName[];             // variable length, null-terminated
    };

struct _COVFILE_DIRECTORY {
    DWORD     dwLengthOfEntry;  // multiple of 4, zero indicates end of list
    DWORD     dwHashOfName;     // to make searching for matches faster
    DWORDLONG dwlOffsetOfData;  // size is BITVECTOR_STORAGE_SIZE( dwBlockCount )
    CHAR      szName[];         // variable length, null-terminated
    };

VOID
PackByteVectorToBitVector(
    IN  DWORD ByteCount,
    IN  PBYTE ByteVector,
    OUT PBYTE BitVector
    );

VOID
UnpackBitVectorToByteVector(
    IN  DWORD BitCount,
    IN  PBYTE BitVector,
    OUT PBYTE ByteVector
    );

VOID
OrBufferWithBuffer(
    IN PVOID TargetBuffer,
    IN PVOID SourceBuffer,
    IN ULONG Size
    );

VOID
XorBufferWithBuffer(
    IN PVOID TargetBuffer,
    IN PVOID SourceBuffer,
    IN ULONG Size
    );

VOID
BufferAndNotBuffer(
    IN PVOID TargetBuffer,
    IN PVOID SourceBuffer,
    IN ULONG Size
    );

ULONG
CountBitsSetInBuffer(
    IN PVOID Buffer,
    IN ULONG Size
    );

BOOL
DoesBufferContainAllZeros(
    IN PVOID Buffer,
    IN DWORD Size
    );

BOOL
DoesBufferContainOnlyHex01s(
    IN PVOID Buffer,
    IN DWORD Size
    );

ULONG
HashName(
    IN LPCSTR Name
    );

#endif /* _COVFILE_H_ */
