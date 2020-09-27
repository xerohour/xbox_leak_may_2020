//-----------------------------------------------------------------------------
// File: LocalCache.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "xonp.h"
#include "xonver.h"

//----------------------------------------------------------------------------
//
// Constants & typedefs
//
//----------------------------------------------------------------------------

typedef enum
{
    XONLC_PARAMS_MAX_CACHED_ITEMS = 0,
    XONLC_PARAMS_MRU_LIST_SIZE,
    XONLC_PARAMS_INDEX_DATA_SIZE,
    XONLC_PARAMS_KEY_DATA_SIZE,
    XONLC_PARAMS_CACHED_DATA_SIZE,
    XONLC_MAX_PARAMS
} XONLC_PARAMS;

//
// Define the cache ops
//
typedef enum
{
    XONLC_OPS_OPEN = 0,
    XONLC_OPS_UPDATE,
	XONLC_OPS_FLUSH,
    XONLC_OPS_RETRIEVE,
    XONLC_OPS_RESET,
    XONLC_OPS_CLOSE,
    XONLC_OPS_UPDATE_RECORD,
    XONLC_OPS_UPDATE_INDEX,
    XONLC_OPS_UPDATE_MRU_LIST,
    XONLC_OPS_MAX_OPS
} XONLC_OPS;

//
// Define the task states
//
typedef enum
{
    XONLC_STATE_INITIALIZE = 0,
    XONLC_STATE_DONE,
    XONLC_STATE_READ,
    XONLC_STATE_WRITE,
    XONLC_STATE_RETRIEVE,
    XONLC_STATE_CONT_RETRIEVE,
    XONLC_STATE_UPDATE_MRU,
    XONLC_STATE_UPDATE_INDEX,
    XONLC_STATE_UPDATE_RECORD,
    XONLC_STATE_RESET
} XONLC_STATE;

class   CCacheTask;
typedef CCacheTask XONLINETASK_CACHE;
typedef CCacheTask* PXONLINETASK_CACHE;

//
// Miscelaneous
//

#define XONLC_SIGNATURE_SIZE        12
#define XONLC_UTIL_SECTOR_SIZE      (XBOX_HD_SECTOR_SIZE - XONLC_SIGNATURE_SIZE)
#define AVAILABLE                   '*'
#define XONLC_MAX_BUFFERS           4   // Number of sectors allocated for the buffer 
#define XONLC_UNLIMITED_BUFFERS     0xFFFFFFFF
#define GET_INDEX_DATA              TRUE
#define DONT_GET_INDEX_DATA         FALSE
#define WRITE_LAST_CHECK_BYTE       TRUE
#define DONT_WRITE_LAST_CHECK_BYTE  FALSE

typedef DWORD TIMESTAMP;
typedef BYTE  CHECKINFO;
typedef DWORD INDEXPOS;

//----------------------------------------------------------------------------
//
// The parameters that define the size of the cache
//
//----------------------------------------------------------------------------

const WORD g_xonCacheParams[XONLC_MAX_CACHE_TYPES][XONLC_MAX_PARAMS] = {
    { 20  , 1   , sizeof(XONLINE_RECENT_LOGON_TIME)    , FIELD_OFFSET(XONLINE_RECENT_LOGON_TIME, lastLogon)   , 0     },  // XONLC_TYPE_RECENT_LOGON_TIME
    { 40  , 1   , FIELD_OFFSET(XKERB_TGT_CONTEXT, ctNumNonces) , FIELD_OFFSET(XKERB_TGT_CONTEXT, StartTime) , sizeof(XKERB_TGT_CONTEXT) - FIELD_OFFSET(XKERB_TGT_CONTEXT, ClientName) },  // XONLC_TYPE_COMBINED_USER_TICKETS
    { 40  , 1   , FIELD_OFFSET(XKERB_SERVICE_CONTEXT, ctNumNonces) , FIELD_OFFSET(XKERB_SERVICE_CONTEXT, StartTime) , sizeof(XKERB_SERVICE_CONTEXT) - FIELD_OFFSET(XKERB_SERVICE_CONTEXT, siteIPAddress) },  // XONLC_TYPE_SERVICE_TICKETS
    { 10  , 0   , 16    , 8   , 14000 },  // XONLC_TYPE_BUDDY_LIST
    { 10  , 0   , 16    , 8   , 800   },  // XONLC_TYPE_MUTE_LIST
    { 500 , 1   , 8     , 8   , 28    },  // XONLC_TYPE_LICENSING
    { 2   , 0   , 8     , 8   , 2000  },  // XONLC_TYPE_BILLING
    { 20  , 1   , 4     , 4   , 32    },  // XONLC_TYPE_TITLEDATA
    { 1   , 0   , 120   , 1   , 0     },  // XONLC_TYPE_GAMEINVITE
    { 20  , 1   , 41    , 8   , 0     },  // XONLC_TYPE_TEST1
    { 15  , 0   , 52    , 8   , 10000 }  // XONLC_TYPE_TEST2
};

//----------------------------------------------------------------------------
//
// Cache structure
//
// HEADER
// Max cached items | MRU List size | Index data size | Key data size | Cached data size | Timestamp Increment (LRU)|
// MRU LIST
// Check byte | Index Data for MRU1 | Cached Data for MRU1 | Check byte |
// INDEX
// Check byte for cached data | Timestamp | Index Data - key | Index Data - immediate data |  ...
// CACHED DATA
// Check byte | Rec #1 | Check byte | ...
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
// Class CHeader - describes the structure of the header in the cache file
//
//----------------------------------------------------------------------------
class CHeader
{
public:
    DWORD          dwMaxCachedItems;
    DWORD          dwMRUListSize;
    DWORD          cbIndexDataSize;
    DWORD          cbKeyDataSize;
    DWORD          cbCachedDataSize;
    TIMESTAMP      dwTimestampIncrement;
}; 

//----------------------------------------------------------------------------
//
// CAddress - used to navigate through the cache file
//
//----------------------------------------------------------------------------
class CAddress
{
public:
    DWORD           dwPosition;            // Position in index of the item described by the address
    DWORD           dwSize;                // Size of the item described by the address
    DWORD           dwSectorBegin;         // Sector where the item starts
    DWORD           dwOffsetBegin;         // Offset in the first sector of the item - relative to the UTIL sector space
    DWORD           dwSectorEnd;           // Sector where the item ends
    DWORD           dwOffsetEnd;           // Offset in the last sector of the item - relative to the UTIL sector space


    //
    // Constructors
    //
    CAddress();
    CAddress(const CAddress&);

    CAddress& operator=(const CAddress&);

    //
    // Increments an address
    //
    VOID IncrementAddress(DWORD dwAmmount);

    //
    // Decrements an address
    //
    VOID DecrementAddress(DWORD dwAmmount);

    //
    // Creates a relative address within a buffer 
    //
    VOID MakeRelativeAddress();

    //
    // Gets the address after cutting the first dwSectors
    //
    VOID CutFirstSectors(DWORD dwSectors);

    //
    // Gets the address after cutting the last dwSectors
    //
    VOID CutLastSectors(DWORD dwSectors);

    //
    // Computes the address
    //
    INLINE VOID FillAddress(
            IN DWORD              dwItemBegin,
            IN DWORD              dwItemEnd,
            IN DWORD              dwPos
            )
    {
        dwSectorBegin = dwItemBegin / XONLC_UTIL_SECTOR_SIZE;
        dwOffsetBegin = dwItemBegin % XONLC_UTIL_SECTOR_SIZE;

        dwSectorEnd = dwItemEnd / XONLC_UTIL_SECTOR_SIZE;
        dwOffsetEnd = dwItemEnd % XONLC_UTIL_SECTOR_SIZE;

        dwPosition = dwPos;
        dwSize = dwItemEnd - dwItemBegin + 1;
    }

};

//----------------------------------------------------------------------------
//
// CSector - describes the structure of a sector on disk
//
//----------------------------------------------------------------------------
class CSector
{
public:
    BYTE                          pbUtilSectorSpace[XONLC_UTIL_SECTOR_SIZE];
    BYTE                          pbSignature[XONLC_SIGNATURE_SIZE];
}; 

//----------------------------------------------------------------------------
//
// CCacheTask - used in the async work pump for XOnline 
//
//----------------------------------------------------------------------------
class CCacheTask
{
    XONLINETASK_CONTEXT           m_xonTask;           // Core task structure 
    
    XONLC_OPS                     m_cacheOp;           // Cache operation performed by the task
    XONLC_CACHE_HANDLE            m_hCache;            // Handle to the cache
    XONLC_STATE                   m_state;             // State in operation

    XONLINETASK_HANDLE            m_hSubtask;          // Generic async subtask handle

    PXONLC_CONTEXT                m_pContext;          // Context used to pass input data to the task
    PBYTE                         m_pbIndexData;       // Input data to update tasks        
    PBYTE                         m_pbRecord;          // Input data to update tasks
    BOOL                          m_bWriteIndex;       // Write or not the index and MRU element to the disk

    HRESULT                       m_hr;                // HRESULT returned by the task

    CXo *                         m_pXo;               // Pointer to CXo that created this task

public:
    //
    // Function to initialize the task context
    //
    VOID InitializeContext( 
            IN XONLC_CACHE_HANDLE   hCache, 
            IN HANDLE               hWorkEvent,
            IN CXo *                pXo
            );

    //
    // Sets the current task's parameters
    //
    VOID SetCurrentTaskParams(
            IN XONLC_OPS            cacheOp,
            IN PBYTE                pbIndexData = NULL,
            IN PBYTE                pbRecord = NULL,
            IN PXONLC_CONTEXT       pContext = NULL,
            IN BOOL                 bWriteIndex = TRUE
            );

    //
    // Async cores for async APIs
    //
    HRESULT DoOpenCache();

    HRESULT DoUpdateCache();

	HRESULT DoIndexFlush();

    HRESULT DoRetrieve();

    HRESULT DoResetCache();

    HRESULT DoCloseCache();

    //
    // Async cores for async private functions
    //
    HRESULT DoUpdateRecordPriv();

    HRESULT DoUpdateIndexPriv();

    HRESULT DoUpdateMRUListPriv();

    //
    // Methods to start async jobs
    //
    HRESULT DoOpenCacheBegin();

    HRESULT DoUpdateCacheBegin();

	HRESULT DoIndexFlushBegin();

    HRESULT DoRetrieveBegin();

    HRESULT DoResetCacheBegin(); 

    HRESULT DoCloseCacheBegin();

    HRESULT DoUpdateRecordPrivBegin();

    HRESULT DoUpdateIndexPrivBegin();

    HRESULT DoUpdateMRUListPrivBegin();

    //
    // Get state
    //
    INLINE XONLC_OPS GetOp()
    {
        return(m_cacheOp);
    }

    //
    // Get HRESULT
    //
    INLINE HRESULT GetHR()
    {
        return (m_hr);
    }

    //
    // Forces the task to completion 
    //
    VOID ForceCompletion();


private:
    //
    // Checks if the size read/written is correct
    //
    HRESULT CheckReadWrite( IN DWORD dwExpectedSize );

    //
    // Starts async job for new chunk of record
    //
    HRESULT UpdateNewChunk(IN CAddress *pAddress );

    //
    // Checks if the retrieved sectors are valid
    //
    HRESULT CheckRetrievedSectors(BOOL bLastChunk);

    //
    // Finish async work when state is XONLC_STATE_DONE
    //
    VOID FinishAsyncWork();

    //
    // Finish async work when state is XONLC_STATE_DONE - used in the Priv async functions
    // Doesn't unlock the cache
    //
    VOID FinishAsyncWorkPriv();

};

//----------------------------------------------------------------------------
//
// CLocalCache - main class, implements the functionality of the local cache
//
//----------------------------------------------------------------------------
class CLocalCache
{
public:

    //
    // Initialize/Terminate cache
    //
    HRESULT Initialize(XONLC_TYPE type , PFNXONLC_COMPARE_HANDLER pfnCompare, HANDLE hWorkEvent, CXo * pXo);
    void    Delete();

    //
    // Open cache file on disk
    //
    HRESULT OpenCacheFile();

    //
    // Makes sure there is enough space on disk for a new created cache - initializes first block
    //
    HRESULT PrepareNewCache();

    //
    // Checks the parameters for the uploaded cache
    //
    BOOL IsCacheCorrupted();

    //
    // Locks the cache
    //
    INLINE VOID LockCache()
    {
        m_bCacheLock = TRUE;
    }

    //
    // Unlocks the cache
    //
    INLINE VOID UnlockCache()
    {
        m_bCacheLock = FALSE;
    }

    //
    // Checks if the cache is locked
    //
    INLINE BOOL IsCacheLocked()
    {
        return (m_bCacheLock);
    }

    //
    // Checks if the cache file has been opened
    //
    INLINE BOOL IsCacheFileOpen()
    {
        return (m_hCacheFile != INVALID_HANDLE_VALUE);
    }

    //
    // Starts the upload of the first block in memory - async
    //
    INLINE HRESULT StartFirstBlockUpload( IN XONLINETASK_HANDLE hTask)
    {
        ULARGE_INTEGER               uliOrigin;

        uliOrigin.QuadPart = 0;

        m_pXo->ReadWriteFileInitializeContext(
                            m_hCacheFile,
                            (LPBYTE)m_pbMemoryBlock,
                            m_dwMemoryBlockSectors * XBOX_HD_SECTOR_SIZE,
                            uliOrigin,
                            m_hWorkEvent, 
                            (PXONLINETASK_FILEIO)hTask);

        return (m_pXo->ReadFileInternal((PXONLINETASK_FILEIO)hTask));
    }

    //
    // Starts the download of the first block
    //
    INLINE HRESULT StartFirstBlockDownload(IN DWORD dwSectors , IN XONLINETASK_HANDLE hTask)
    {
        ULARGE_INTEGER               uliOrigin;

        uliOrigin.QuadPart = 0;

        PrepareSectorsForWrite(m_pbMemoryBlock, 0, dwSectors - 1);

        m_pXo->ReadWriteFileInitializeContext(
                            m_hCacheFile,
                            (LPBYTE)m_pbMemoryBlock,
                            dwSectors * sizeof(CSector),
                            uliOrigin,
                            m_hWorkEvent,  
                            (PXONLINETASK_FILEIO)hTask);

        return (m_pXo->WriteFileInternal((PXONLINETASK_FILEIO)hTask));
    }

    //
    // Gets first block size size
    //
    INLINE DWORD GetFirstBlockSize()
    {
        return(m_dwMemoryBlockSectors * XBOX_HD_SECTOR_SIZE);   
    }

    //
    // Searches the MRU List for the key specified in pContext
    //
    BOOL FindInMRUList(
            IN BOOL                               bOverwrite, // overwrite pContext->pbIndexData
            OUT PXONLC_CONTEXT                    pContext  
            );

    //
    // Searches the index for the key specified in pContext
    //
    BOOL FindInIndex(
            IN BOOL                               bOverwrite, // overwrite pContext->pbIndexData
            OUT PXONLC_CONTEXT                    pContext  
            );

    //
    // Gets work address - first chunk for the cache buffer
    //
    VOID GetWorkAddress(CAddress *pAddress);


    //
    // Reads sectors from disk - starts async job - address in hCache
    //
    HRESULT ReadSectors(
            IN CAddress                           *pAddress,
            IN XONLINETASK_HANDLE                 hTask
            );

    //
    // Write sectors to disk - starts async job - address in hCache
    //
    HRESULT WriteSectors(
            IN CAddress                           *pAddress,
            IN PBYTE                              *pbData,
            IN XONLINETASK_HANDLE                 hTask
            );

    //
    // Zeroes the address
    //
    INLINE VOID ResetCurrentAddress()
    {
        memset(&m_currentAddress , 0 , sizeof(CAddress));
    }

    //
    // Is there more work to do on update record?
    //
    INLINE BOOL ContinueUpdateRecord()
    {
        if( m_currentAddress.dwSectorEnd - m_currentAddress.dwSectorBegin >= XONLC_MAX_BUFFERS )
        {
            m_currentAddress.dwSectorBegin += XONLC_MAX_BUFFERS;
            m_currentAddress.dwOffsetBegin = 0;
            return TRUE;
        }
        return FALSE;
    }

    //
    // Starts writing the current index entry on disk
    //
    HRESULT StartWriteCurrentIndexEntry(
            IN PBYTE                              pbIndexData,
            IN XONLINETASK_HANDLE                 hTask
            );

    //
    // Start retrieving a record
    //
    HRESULT StartRetrieve(
            IN PXONLC_CONTEXT                     pContext,
            IN XONLINETASK_HANDLE                 hTask
            );

    //
    // Checks if is anything left to read for a record
    //
    INLINE BOOL ContinueRetrieve( IN PXONLC_CONTEXT pContext)
    {
        return ( UsingClientBuffer(pContext) );
    }

    //
    // Finish retrieving a record
    //
    HRESULT FinishRetrieve(
            IN PXONLC_CONTEXT                     pContext,
            IN XONLINETASK_HANDLE                 hTask
            );

    //
    // Gets the expected retrieve size taking into account wheather is the first or second call
    //
    DWORD GetExpectedRetrieveSize(
           IN PXONLC_CONTEXT                     pContext,
           IN BOOL                               bLastChunk
           );

    //
    // Checks retrieve results
    //
    HRESULT CheckRetrieveResults(
            IN PXONLC_CONTEXT                     pContext
            );

    //
    // Checks sectors integrity
    //
    HRESULT CheckRetrievedSectorsIntegrity(
            IN PXONLC_CONTEXT                     pContext,
            IN BOOL                               bLastChunk
            );


    //
    // Retrieves the address for the specified record
    //
    BOOL GetRecordAddressFromCurrentAddress();

    //
    // Sets the flag for first chunk
    //
    INLINE VOID MarkFirstUpdateChunk()
    {
        m_bUpdateFirstChunk = TRUE;
    }

    //
    // Update the current index entry in memory
    //
    VOID UpdateCurrentIndexEntry(IN PBYTE pbIndexData);

    //
    // Updates the MRU List in memory
    //
    VOID UpdateMRUList(PBYTE pbIndexData, PBYTE pbRecord);

    //
    // Checks if the cache type supports MRU list
    //
    INLINE BOOL HasMRUList()
    {
        return (0 != GetMRUListSize());
    }

    //
    // Checks if the cache type has cached data block
    //
    INLINE BOOL HasRecords()
    {
        return (0 != GetCachedDataSize());
    }

    //
    // Gets the number of sectors for the part of the cache that is uploaded in memory
    //
    INLINE DWORD GetFirstBlockSectors()
    {
        return m_dwMemoryBlockSectors;
    }

    //
    // Compute number of sectors spanned by MRU List
    //
    INLINE DWORD GetMRUListSectors()
    {
        return ComputeSectorsCount(m_dwIndexOffset);
    }

    //
    // Checks if the retrieve buffer is DWORD aligned
    //
    INLINE BOOL IsBufferDWORDAligned(IN PXONLC_CONTEXT pContext)
    {
        if( UsingClientBuffer(pContext) )
        {
            return ((((DWORD)(pContext->pbRecordBuffer)) & (sizeof(DWORD) - 1)) == 0);
        }

        return (TRUE);
    }

    //
    // Delete current index entry
    //
    VOID DeleteCurrentIndexEntry();

    //
    // Writes the first block for a new created/reset cache file 
    //
    VOID InitializeFirstBlock();

    //
    // Get handle to the file IO subtask 
    //
    INLINE XONLINETASK_HANDLE GetFileIOSubtaskHandle()
    {
        return ((XONLINETASK_HANDLE)(&m_fileIOSubtask));
    }

    //
    // Get handle to the cache subtask
    //
    INLINE XONLINETASK_HANDLE GetCacheSubtaskHandle()
    {
        return ((XONLINETASK_HANDLE)(&m_cacheSubtask));
    }

    //
    // Set the work event
    //
    INLINE BOOL SetWorkEvent()
    {
        if( NULL != m_hWorkEvent )
        {
            return (SetEvent(m_hWorkEvent));
        }

        return (TRUE);
    }

private:
    //
    // Get/Set
    //

    //
    // Gets current index entry indicated by m_currentAddress
    //
    INLINE PBYTE GetCurrentIndexEntry()
    {
        return &(m_pbMemoryBlock)[m_currentAddress.dwSectorBegin].pbUtilSectorSpace[m_currentAddress.dwOffsetBegin];
    }

    //
    // Get MRU start pointer
    //
    INLINE PBYTE GetMRUListStart()
    {
        return (PBYTE)(m_pbMemoryBlock) + sizeof(CHeader);
    }

    //
    // Gets the pointer in memory block indicated by address
    //
    INLINE PBYTE GetPointerInMemBlock( IN CAddress *pAddress )
    {
        return &(m_pbMemoryBlock[pAddress->dwSectorBegin].pbUtilSectorSpace[pAddress->dwOffsetBegin]);
    }

    //
    // Gets the pointer in the buffer of sectors indicated by address - considering sector 0 to start in buffer
    //
    INLINE PBYTE GetPointerInBuffer( IN CAddress *pAddress )
    {
        return &(m_pbBuffer[0].pbUtilSectorSpace[pAddress->dwOffsetBegin]);
    }

    //
    // Gets the total number of sectors for the file on disk
    //
    DWORD GetTotalSectors ();

    //
    // Gets the data from a given address
    //
    VOID GetDataFromAddress(
            IN CSector             *pbSectors,
            IN CAddress            *pAddress,
            IN DWORD               dwDataSize,
            OUT PBYTE              pbData
            );

    //
    // Gets data from current address
    //
    INLINE VOID GetDataFromCurrentAddress(
                IN DWORD               dwDataSize,
                OUT PBYTE              pbData
                )
    {
        GetDataFromAddress(m_pbMemoryBlock , &m_currentAddress , dwDataSize , pbData);
    }

    //
    // Gets the key from an index entry
    //
    INLINE PBYTE GetKey( PBYTE pbIndexEntry )
    {
        return pbIndexEntry + sizeof(CHECKINFO) + sizeof(TIMESTAMP);
    }

    //
    // Gets the index data from an index entry
    //
    INLINE PBYTE GetIndexData( PBYTE pbIndexEntry )
    {
        return pbIndexEntry + sizeof(CHECKINFO) + sizeof(TIMESTAMP);
    }

    //
    // Sets the index content
    //
    INLINE VOID SetIndexData( PBYTE pbIndexEntry , PBYTE pbIndexData)
    {
        memcpy(GetKey(pbIndexEntry), pbIndexData , GetIndexDataSize() );
    }

    //
    // Gets the timestamp form an index entry
    //
    INLINE TIMESTAMP GetTimestamp( PBYTE pbIndexEntry )
    {
        return *(TIMESTAMP*)(pbIndexEntry + sizeof(CHECKINFO));
    }

    //
    // Sets the timestamp for an index entry
    //
    INLINE VOID SetTimestamp( PBYTE pbIndexEntry , TIMESTAMP timeStamp )
    {
        *(TIMESTAMP*)(pbIndexEntry + sizeof(CHECKINFO)) = timeStamp;
    }

    //
    // Sets the index position for a MRU item
    //
    INLINE VOID SetIndexPosition( PBYTE pbMRUEntry , DWORD dwIndexPos )
    {
        *(DWORD*)(pbMRUEntry + sizeof(CHECKINFO)) = dwIndexPos;
    }


    //
    // Get/Set the check bytes
    //
    INLINE BYTE GetFirstByte(CSector *pbMemory , CAddress *pAddress)
    {
        return (pbMemory[pAddress->dwSectorBegin]).pbUtilSectorSpace[pAddress->dwOffsetBegin];
    }

    INLINE BYTE GetLastByte(CSector *pbMemory , CAddress *pAddress)
    {
        return (pbMemory[pAddress->dwSectorEnd]).pbUtilSectorSpace[pAddress->dwOffsetEnd];
    }

    INLINE BYTE SetFirstByte(CSector *pbMemory , CAddress *pAddress , BYTE byte)
    {
        return (pbMemory[pAddress->dwSectorBegin]).pbUtilSectorSpace[pAddress->dwOffsetBegin] = byte;
    }

    INLINE BYTE SetLastByte(CSector *pbMemory , CAddress *pAddress , BYTE byte)
    {
        return (pbMemory[pAddress->dwSectorEnd]).pbUtilSectorSpace[pAddress->dwOffsetEnd] = byte;
    }

    //
    // Retrieves the address for the specified MRU list item
    //
    BOOL GetMRUItemAddress( IN DWORD  dwPos );


    //
    // Retrieves the address for the specified index item
    //
    BOOL GetIndexItemAddress( IN DWORD dwPos );

    //
    // Sets beginning check byte
    //
    VOID SetFirstCheckByte(CSector *pbMemory , CAddress *pAddress);

    //
    // Sets end check byte
    //
    VOID SetLastCheckByte(CSector *pbMemory , CAddress *pAddress);

private:
    //
    // Size INLINE functions
    //

    INLINE DWORD GetMaxCachedItems()
    {
        return g_xonCacheParams[m_cacheType][XONLC_PARAMS_MAX_CACHED_ITEMS];
    }

    INLINE DWORD GetMRUListSize()
    {
        return g_xonCacheParams[m_cacheType][XONLC_PARAMS_MRU_LIST_SIZE];
    }

    INLINE DWORD GetKeySize()
    {
        return g_xonCacheParams[m_cacheType][XONLC_PARAMS_KEY_DATA_SIZE];
    }

    INLINE DWORD GetIndexEntrySize()
    {
        return sizeof(CHECKINFO) +       // the check byte for the cached record
               sizeof(TIMESTAMP) +      // the timestamp for LRU
               g_xonCacheParams[m_cacheType][XONLC_PARAMS_INDEX_DATA_SIZE]; 
    }

    INLINE DWORD GetIndexDataSize()
    {
        return g_xonCacheParams[m_cacheType][XONLC_PARAMS_INDEX_DATA_SIZE];
    }

    INLINE DWORD GetMRUItemSize()
    {
        return  sizeof(CHECKINFO) + // begin check byte
                sizeof(INDEXPOS)  + // index position
                g_xonCacheParams[m_cacheType][XONLC_PARAMS_INDEX_DATA_SIZE] +
                g_xonCacheParams[m_cacheType][XONLC_PARAMS_CACHED_DATA_SIZE] +
                sizeof(CHECKINFO);  // end check byte

    }

    INLINE DWORD GetRecordSize()
    {
        return  sizeof(CHECKINFO) + // begin check byte
                g_xonCacheParams[m_cacheType][XONLC_PARAMS_CACHED_DATA_SIZE] +
                sizeof(CHECKINFO); // end check byte
    }

    INLINE DWORD GetCachedDataSize()
    {
        return  g_xonCacheParams[m_cacheType][XONLC_PARAMS_CACHED_DATA_SIZE];
                
    }

private:
    //
    // Miscellaneous
    //

    //
    // Computes the offsets for index and cached records
    //
    VOID ComputeCacheOffsets();

    //
    // Checks a sector for integrity
    //
    INLINE BOOL IsSectorCorrupted(IN CSector *pSectors , IN DWORD dwPos)
    {
#ifdef XONLINE_FEATURE_XBOX 

        XCALCSIG_SIGNATURE Signature;
        HANDLE hSig = NULL;

		// Check signature
		hSig = XCalculateSignatureBegin(XCALCSIG_FLAG_NON_ROAMABLE);
		XCalculateSignatureUpdate(hSig, 
			                      (PBYTE)pSectors[dwPos].pbUtilSectorSpace, 
			                      XONLC_UTIL_SECTOR_SIZE);
		XCalculateSignatureEnd(hSig, &Signature);

		return (memcmp(&Signature, pSectors[dwPos].pbSignature, XONLC_SIGNATURE_SIZE) != 0);
#else
		return (FALSE);
#endif
    }


    //
    // Gets the number of sectors for the part of the cache that is uploaded in memory
    //
    INLINE DWORD ComputeFirstBlockSectorsCount()
    {
        return ComputeSectorsCount(m_dwCachedDataOffset);
    }


    //
    // Computes the number of sectors needed to store the info with the given size
    //
    DWORD ComputeSectorsCount( IN DWORD dwSize );

    //
    // Prepares the sectors in the specified range for writing on disk 
    //
    VOID PrepareSectorsForWrite(
            IN CSector                *pbMemoryBlock,
            IN DWORD                  dwSectorIndexBegin,
            IN DWORD                  dwSectorIndexEnd
            );

    //
    // Copies the cached record to sectors
    //
    PBYTE CopyDataToSectors(
            IN PBYTE               pbSrcData,
            IN CAddress            *pAddress,
            IN DWORD               dwMaxNumberOfBuffers,
            IN BOOL                bWriteLastCheckByte,
            OUT CSector            *pbDestData
            );

    //
    // Copies the info from a buffer into a sector array
    //
    PBYTE CopyBufferToSectors(
            IN PBYTE               pbSrcData,
            IN CAddress            *pAddress,
            OUT CSector            *pbDestData
            );

    //
    // Marks one entry given by an address as available
    //
    INLINE VOID MarkCurrentEntryAsAvailable()
    {
        (m_pbMemoryBlock[m_currentAddress.dwSectorBegin]).pbUtilSectorSpace[m_currentAddress.dwOffsetBegin] = AVAILABLE;
    }

    //
    // Checks if the current entry has been marked as available
    //
    INLINE BOOL IsCurrentEntryAvailable()
    {
        return (AVAILABLE == (m_pbMemoryBlock[m_currentAddress.dwSectorBegin]).pbUtilSectorSpace[m_currentAddress.dwOffsetBegin]);
    }

    //
    // Compare two index datas
    //
    BOOL CompareIndexData(PBYTE pbIndexData1 , PBYTE pbIndexData2);

    //
    // Checks if the client buffer is used in the process of retrieve
    //
    INLINE BOOL UsingClientBuffer(IN PXONLC_CONTEXT pContext)
    {
        //
        // We're using the client's buffer if the record size is too big to fit m_pbBuffer
        //
        return ( m_currentAddress.dwSectorEnd - m_currentAddress.dwSectorBegin >= XONLC_MAX_BUFFERS );
    }

private:
    //
    // Members
    //
    XONLC_TYPE                    m_cacheType;                  // Type of cache from the XONLC_TYPE
    HANDLE                        m_hCacheFile;                 // Handle to the file on disk
    BOOL                          m_bCacheLock;                 // Flag for locking/unlocking the cache

    CSector                       *m_pbMemoryBlock;             // The memory block that holds the first sectors
                                                                // of the cache(Header ,MRU list, Index)
    DWORD                         m_dwMemoryBlockSectors;       // Number of sectors that the memory block spans
    CHeader                       *m_pHeader;                   // Pointer to the header in memory block
    DWORD                         m_dwIndexOffset;              // The offset in file where the index starts
    DWORD                         m_dwCachedDataOffset;         // The offset in file where the cached data starts

    PFNXONLC_COMPARE_HANDLER      m_pfnCompareFunc;             // Handle to a custom comparison function(can be NULL)

    CSector                       m_pbBuffer[XONLC_MAX_BUFFERS];// Additional buffer on stack used in read/write ops  
    CAddress                      m_currentAddress;             // Address of the current item handled(mru list item, index item or record)
    BYTE                          m_bCheckByte;                 // Current check byte
    BOOL                          m_bUpdateFirstChunk;          // Flag that tells if the update just started and the first 
                                                                // check byte needs to be written
    DWORD                         m_dwCurrentIndexPos;          // The current position in index
    BOOL                          m_bFoundInMRUList;            // Flag that tells if the current item was found
                                                                // in the MRU list
    XONLINETASK_FILEIO            m_fileIOSubtask;              // Subtask structure for read/write ops
    XONLINETASK_CACHE             m_cacheSubtask;               // Subtask structure for cache private ops

    HANDLE                        m_hWorkEvent;                 // Event to be used by the subtasks

    CXo *                         m_pXo;                        // Pointer to CXo that created this cache

};


//////////////////////////////////////////////////////////////////////////////
/*CacheOpen - Routine Description:

    Opens the cache of the specified type. 

Arguments:
    cacheType[in]          One of the types of cache enumerated in XONLC_TYPE.
    pfnCompareFunc[in]     Custom comparison function handle.
    hWorkEvent[in]         Event to be set when pumping is required. Can be NULL.
    phCache[out]           Handle to the opened cache. The contents of it should be checked after the task completes.
    phTask[out]            Upon success, this out parameter returns a handle to the open task.

Return Value:

    HRESULT

Memory Allocation:

    Allocates cache structure and task handle.

Notes:
    Asynchronous
*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CXo::CacheOpen(
    IN XONLC_TYPE                           cacheType,
    IN PFNXONLC_COMPARE_HANDLER             pfnCompareFunc,
    IN HANDLE                               hWorkEvent,
    OUT PHANDLE                             phCache,
    OUT PXONLINETASK_HANDLE                 phTask
    )
{
    HRESULT hr = S_OK;

    Assert(cacheType < XONLC_MAX_CACHE_TYPES);
    Assert(NULL != phCache);
    Assert(NULL != phTask);
    Assert(NULL == m_xonCacheHandles[cacheType]);


    PXONLINETASK_CACHE pCacheTask = NULL;
    XONLC_CACHE_HANDLE hCache = NULL;

    *phCache = NULL;    
	*phTask = NULL;
    
    do
    {
        //
        // The cache needs to be created from disk - allocate memory for cache structures in memory
        //
        hCache = m_xonCacheHandles[cacheType] = (CLocalCache *)SysAllocZ(sizeof(CLocalCache), PTAG_CLocalCache);
        if( NULL ==  hCache)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        //
        // Initialize the cache structure
        //
        hr = hCache->Initialize(cacheType, pfnCompareFunc, hWorkEvent, this);
        if( FAILED( hr ) )
        {
            break;
        }

        // Allocate the task context
        pCacheTask = (PXONLINETASK_CACHE)SysAlloc(sizeof(XONLINETASK_CACHE), PTAG_XONLINETASK_CACHE);
        if (!pCacheTask)
        {
            hr  = E_OUTOFMEMORY;
            break;
        }

        // Initialize the context
        pCacheTask->InitializeContext(hCache, hWorkEvent, this);

        //
        // Start async job
        //
        hr = pCacheTask->DoOpenCacheBegin();

    }
    while( FALSE );
    
    if( FAILED( hr ) )
    {
        if( NULL != hCache )
        {
            //
            // Cleanup allocated memory
            //
            hCache->Delete();
        }

		if( NULL != pCacheTask )
        {
			//
            // Cleanup allocated task handle
            //
			SysFree(pCacheTask);
        }
    }
    else
    {
		//
        // Lock cache for async task
        //
		hCache->LockCache();

		//
	    // Return the opaque handle
	    //	
        *phTask = (XONLINETASK_HANDLE)pCacheTask;

        //
        //  Return the handle to the newly opened cache
        //
        *phCache = hCache;
    }

    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
/*CacheUpdate - Routine Description:

    Called when a record in the cache needs to be updated or, if it doesn't exist, it needs to be created. 

Arguments:
    hCache[in]         The handle to cache. The updates are addressed to a certain type of cache and the sizes for the index entries and the actual cached data are implied by this handle. 
    hTask[in]          The handle to the update task.
    pbIndexData[in]    The index entry.
    pbRecord[in]       The actual data to be cached. The buffer has to be DWORD aligned.
    
Return Value:

    HRESULT

Memory Allocation:

    No.

Notes:
    Asynchronous
*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CXo::CacheUpdate(
    IN HANDLE                             hCache,
    IN XONLINETASK_HANDLE                 hTask,
    IN PBYTE                              pbIndexData,                  
    IN PBYTE                              pbRecord,
    IN BOOL                               bWriteIndex
    )
{
    HRESULT hr = S_OK;
    PXONLINETASK_CACHE pCacheTask = NULL;
    
    Assert(hCache != NULL);
    Assert(pbIndexData != NULL);
    Assert(hTask != NULL);
    Assert(!((XONLC_CACHE_HANDLE)hCache)->HasRecords() || (pbRecord != NULL));

    do
    {
        if( ((XONLC_CACHE_HANDLE)hCache)->IsCacheLocked() )
        {
            hr = E_FAIL;
            break;
        }

        pCacheTask = (PXONLINETASK_CACHE)hTask;

        // Sets the current task parameters
        pCacheTask->SetCurrentTaskParams( XONLC_OPS_UPDATE , pbIndexData , pbRecord, NULL, bWriteIndex);

        //
        // Start async work 
        //
        hr = pCacheTask->DoUpdateCacheBegin();
    }
    while( FALSE );

    if( SUCCEEDED( hr ) )
    {
        //
        // Everything OK - lock the cache
        //
        ((XONLC_CACHE_HANDLE)hCache)->LockCache();
    }
    
    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
/*CacheIndexFlush - Routine Description:

    Writes down the index. 

Arguments:
    hCache[in]          The handle to cache. 
    hTask[in]           The handle to the task.

Return Value:

    HRESULT

Memory Allocation:

    No.

Notes:
    Asynchronous
*/
//////////////////////////////////////////////////////////////////////////////

HRESULT CXo::CacheIndexFlush(HANDLE hCache, XONLINETASK_HANDLE hTask)
{
    HRESULT hr = S_OK;
    PXONLINETASK_CACHE pCacheTask = NULL;
    
    Assert(hCache != NULL);
    Assert(hTask != NULL);

    do
    {
        if( ((XONLC_CACHE_HANDLE)hCache)->IsCacheLocked() )
        {
			hr = E_FAIL;
			break;
        }

        pCacheTask = (PXONLINETASK_CACHE)hTask;

        // Sets the current task parameters
        pCacheTask->SetCurrentTaskParams( XONLC_OPS_FLUSH );

        //
        // Start async job
        //
        hr = pCacheTask->DoIndexFlushBegin();
    }
    while( FALSE );

    if( SUCCEEDED( hr ) )
    {
        //
        // Everything OK - lock the cache
        //
        ((XONLC_CACHE_HANDLE)hCache)->LockCache();
    }
    
    return( hr );

}

//////////////////////////////////////////////////////////////////////////////
/*CacheLookup - Routine Description:

    Called to lookup a given index data. 

Arguments:
    hCache[in]         The handle to cache. 
    pContext[out]      A pointer to a context structure which contains specific data describing the record to be retrieved. 

Return Value:

    HRESULT

Memory Allocation:

    No.

Notes:
    Synchronous
*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CXo::CacheLookup(
    IN HANDLE                             hCache,
    OUT PXONLC_CONTEXT                    pContext  
    )
{
    HRESULT hr = S_OK;

    Assert(hCache != NULL);
    Assert(pContext != NULL);
    Assert(pContext->pbIndexData != NULL);
            
    do
    {
        if( ((XONLC_CACHE_HANDLE)hCache)->IsCacheLocked() )
        {
            hr = E_FAIL;
            break;
        }

        ((XONLC_CACHE_HANDLE)hCache)->LockCache();

        //
        // Parse the MRU List in search of the key
        //
        if(!((XONLC_CACHE_HANDLE)hCache)->FindInMRUList(GET_INDEX_DATA, pContext))  
        {
            //
            // Key not found in MRU - parse the index
            //
            ((XONLC_CACHE_HANDLE)hCache)->FindInIndex(GET_INDEX_DATA, pContext);
        }

        if(( pContext->bValidDataInCache ) && (!pContext->bCallRetrieveNext))
        {
            ((XONLC_CACHE_HANDLE)hCache)->UpdateMRUList(pContext->pbIndexData , pContext->pbRecordBuffer);
        }

        ((XONLC_CACHE_HANDLE)hCache)->UnlockCache();
    }
    while(FALSE);

    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
/*CacheRetrieve - Routine Description:

    Called after Lookup to retrieve a record from disk. 

Arguments:
    hCache[in]        The handle to cache. 
    hTask[in]         The handle to the retrieve task.
    pContext[out]     A pointer to a context structure which contains specific data describing the record to be retrieved. 
    
Return Value:

    HRESULT

Memory Allocation:

    No.

Notes:
    Asynchronous
*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CXo::CacheRetrieve(
    IN HANDLE                             hCache,
    IN XONLINETASK_HANDLE                 hTask,
    IN OUT PXONLC_CONTEXT                 pContext  
    )
{
    HRESULT hr = S_OK;
    PXONLINETASK_CACHE pCacheTask = NULL;

    Assert(hCache != NULL);
    Assert(hTask != NULL);
    Assert(pContext != NULL);
    Assert(((XONLC_CACHE_HANDLE)hCache)->HasRecords());
    Assert(pContext->pbIndexData != NULL);
    Assert(pContext->pbRecordBuffer != NULL);
    Assert(pContext->dwRecordBufferSize != 0);
    Assert((((XONLC_CACHE_HANDLE)hCache)->IsBufferDWORDAligned(pContext)));
    Assert(TRUE == pContext->bCallRetrieveNext);
    Assert(TRUE == pContext->bValidDataInCache);
        
    do
    {
        if( ((XONLC_CACHE_HANDLE)hCache)->IsCacheLocked() )
        {
            hr = E_FAIL;
            break;
        }

        pCacheTask = (PXONLINETASK_CACHE)hTask;

        // Sets the current task parameters
        pCacheTask->SetCurrentTaskParams( XONLC_OPS_RETRIEVE , NULL , NULL, pContext);

        //
        // Start async work
        //
        hr = pCacheTask->DoRetrieveBegin();
    }
    while(FALSE);

    if( SUCCEEDED( hr ) )
    {
        //
        // Everything OK - lock the cache
        //
        ((XONLC_CACHE_HANDLE)hCache)->LockCache();
    }

    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
/*CacheDelete - Routine Description:

    Called to delete a given index data. 

Arguments:
    hCache[in]         The handle to cache. 
    pContext[out]      A pointer to a context structure which contains specific data describing the record to be retrieved. 

Return Value:

    HRESULT

Memory Allocation:

    No.

Notes:
    Synchronous
*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CXo::CacheDelete(
    IN HANDLE                             hCache,
    OUT PXONLC_CONTEXT                    pContext  
    )
{
    HRESULT hr = S_OK;

    Assert(hCache != NULL);
    Assert(pContext != NULL);
    Assert(pContext->pbIndexData != NULL);
        
    do
    {
        if( ((XONLC_CACHE_HANDLE)hCache)->IsCacheLocked() )
        {
            hr = E_FAIL;
            break;
        }

        ((XONLC_CACHE_HANDLE)hCache)->LockCache();
        
        //
        // Parse the MRU List in search of the key
        //
        BOOL bFound = ((XONLC_CACHE_HANDLE)hCache)->FindInMRUList(DONT_GET_INDEX_DATA, pContext);

        if(!bFound) 
        {
            //
            // Key not found in MRU - parse the index
            //
            bFound = ((XONLC_CACHE_HANDLE)hCache)->FindInIndex(DONT_GET_INDEX_DATA, pContext);
        }

        if( bFound )
        {
            ((XONLC_CACHE_HANDLE)hCache)->DeleteCurrentIndexEntry();
        }

        ((XONLC_CACHE_HANDLE)hCache)->UnlockCache();
    }
    while(FALSE);

    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
/*CacheFileDelete - Routine Description:

    Deletes the actual cache file from the disk.

Arguments:
    cacheType[in]       One of the types of cache enumerated in XONLC_TYPE. 
    
Return Value:

    HRESULT

Memory Allocation:

    No.

Notes:
    Synchronous
*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CXo::CacheFileDelete(XONLC_TYPE cacheType)
{
    HRESULT hr = S_OK;

    if( NULL != m_xonCacheHandles[cacheType] )
    {
        m_xonCacheHandles[cacheType]->Delete();
        m_xonCacheHandles[cacheType] = NULL;
    }

    //
    // Delete cache file on disk
    //
    char szFileName[500];
    sprintf(szFileName , "\\Device\\Harddisk0\\partition1\\CACHE\\LocalCache%02d.bin" , cacheType);

    OBJECT_STRING FileName;
    OBJECT_ATTRIBUTES Obja;
    
    RtlInitObjectString(&FileName, szFileName);

    InitializeObjectAttributes( &Obja,
                                &FileName,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                NULL
                              );

    NTSTATUS Status = NtDeleteFile(&Obja);
    if(!NT_SUCCESS(Status)) 
    {
        hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));    
    }
       
    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
/*CacheReset - Routine Description:

    Discards cache content. 

Arguments:
    hCache[in]         The handle to cache. 
    hTask[in]          The handle to the reset task.

Return Value:

    HRESULT

Memory Allocation:

    No.

Notes:
    Asynchronous
*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CXo::CacheReset(HANDLE hCache, XONLINETASK_HANDLE hTask)
{
    HRESULT hr = S_OK;
    PXONLINETASK_CACHE pCacheTask = NULL;
    
    Assert(hCache != NULL);
    Assert(hTask != NULL);

    do
    {
        if( ((XONLC_CACHE_HANDLE)hCache)->IsCacheLocked() )
        {
            hr = E_FAIL;
            break;
        }

        pCacheTask = (PXONLINETASK_CACHE)hTask;

        // Initialize the current parameters
        pCacheTask->SetCurrentTaskParams( XONLC_OPS_RESET );

        //
        // Start async work
        //
        pCacheTask->DoResetCacheBegin();
    }
    while( FALSE );

    if( SUCCEEDED( hr ) )
    {
        //
        // Everything OK - lock the cache
        //
        ((XONLC_CACHE_HANDLE)hCache)->LockCache();
    }
    
    return( hr );

}


//////////////////////////////////////////////////////////////////////////////
/*CacheClose - Routine Description:

    Closes a cache type and releases the resources associated with it. 

Arguments:
    hCache[in]          The handle to cache. 
    hTask[in]           The handle to the close task.

Return Value:

    HRESULT

Memory Allocation:

    No.

Notes:
    Asynchronous
*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CXo::CacheClose(HANDLE hCache, XONLINETASK_HANDLE hTask)
{
    HRESULT hr = S_OK;
    PXONLINETASK_CACHE pCacheTask = NULL;
    
    Assert(hCache != NULL);
    Assert(hTask != NULL);

    do
    {
        if( ((XONLC_CACHE_HANDLE)hCache)->IsCacheLocked() )
        {
			//
			// Some subtask involving the cache is in progress
			//
			hr = XONLINETASK_S_RUNNING;
			while(XONLINETASK_S_RUNNING == hr)
			{
				hr = XOnlineTaskContinue(hTask);
			}

			Assert(!(((XONLC_CACHE_HANDLE)hCache)->IsCacheLocked()));
        }

        pCacheTask = (PXONLINETASK_CACHE)hTask;

        // Sets the current task parameters
        pCacheTask->SetCurrentTaskParams( XONLC_OPS_CLOSE );

        //
        // Start async job
        //
        hr = pCacheTask->DoCloseCacheBegin();
    }
    while( FALSE );

    if( SUCCEEDED( hr ) )
    {
        //
        // Everything OK - lock the cache
        //
        ((XONLC_CACHE_HANDLE)hCache)->LockCache();
    }
    
    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
/*CacheClose - Routine Description:

    Closes a cache type and releases the resources associated with it. 

Arguments:
    hCache[in]          The handle to cache. 

Return Value:

    HRESULT

Memory Allocation:

    No.

Notes:
    Synchronous
*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CXo::CacheClose(HANDLE hCache)
{
    HRESULT hr = S_OK;
        
    Assert(hCache != NULL);
    
    do
    {
        if( ((XONLC_CACHE_HANDLE)hCache)->IsCacheLocked() )
        {
			hr = E_FAIL;
			break;
        }

        //
        // Delete hCache will delete all the memory associated with it and 
        // will put the global array item on NULL
        //
        ((XONLC_CACHE_HANDLE)hCache)->Delete();
    }
    while( FALSE );

    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
//
// Task-related internal functions
//
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//
// Do work function for cache operations
//
//----------------------------------------------------------------------------
HRESULT CXo::CacheContinue(XONLINETASK_HANDLE hTask)
{
    HRESULT             hr = S_OK;
    CCacheTask          *pCacheTask = (CCacheTask*)hTask;

    Assert(hTask != NULL);

    // Call the function to take care of this
    switch (pCacheTask->GetOp())
    {
        case XONLC_OPS_OPEN:
            hr = pCacheTask->DoOpenCache();
            break;

        case XONLC_OPS_UPDATE:
            hr = pCacheTask->DoUpdateCache();
            break;

        case XONLC_OPS_FLUSH:
            hr = pCacheTask->DoIndexFlush();
            break;

        case XONLC_OPS_RETRIEVE:
            hr = pCacheTask->DoRetrieve();
            break;

        case XONLC_OPS_RESET:
            hr = pCacheTask->DoResetCache();
            break;

        case XONLC_OPS_CLOSE:
            hr = pCacheTask->DoCloseCache();
            break;

        case XONLC_OPS_UPDATE_RECORD:
            hr = pCacheTask->DoUpdateRecordPriv();
            break;

        case XONLC_OPS_UPDATE_INDEX:
            hr = pCacheTask->DoUpdateIndexPriv();
            break;

        case XONLC_OPS_UPDATE_MRU_LIST:
            hr = pCacheTask->DoUpdateMRUListPriv();
            break;
    }

    return (hr);
}

//----------------------------------------------------------------------------
//
// Close function for Cache
//
//----------------------------------------------------------------------------
VOID CXo::CacheTaskClose(XONLINETASK_HANDLE hTask)
{
    Assert(hTask != NULL);

	PXONLINETASK_CACHE pCacheTask = (PXONLINETASK_CACHE)hTask;

	pCacheTask->ForceCompletion();
   
    SysFree(pCacheTask);
}


//////////////////////////////////////////////////////////////////////////////
//
// Private async functions
//
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//
// Updates the MRU list - async
//
//----------------------------------------------------------------------------
HRESULT CXo::CacheUpdateMRUListPriv(
    IN XONLC_CACHE_HANDLE                   hCache,
    IN XONLINETASK_HANDLE                   hTask,
    IN PBYTE                                pbIndexData,
    IN PBYTE                                pbRecord
    )
{

    HRESULT hr = S_OK;
    PXONLINETASK_CACHE pCacheTask = NULL;

    Assert(hCache != NULL);
    Assert(hTask != NULL);
    Assert(pbIndexData != NULL);
    Assert(!((XONLC_CACHE_HANDLE)hCache)->HasRecords() || (pbRecord != NULL));
    
    do
    {
        pCacheTask = (PXONLINETASK_CACHE)hTask;

        // Sets the current task parameters
        pCacheTask->SetCurrentTaskParams( XONLC_OPS_UPDATE_MRU_LIST , pbIndexData, pbRecord);

        //
        // Start async work
        //
        hr = pCacheTask->DoUpdateMRUListPrivBegin();

    }
    while( FALSE );
    
    return( hr );
}

//----------------------------------------------------------------------------
//
// Updates the index entry from the current cache address - async
//
//----------------------------------------------------------------------------
HRESULT CXo::CacheUpdateIndexPriv(
    IN XONLC_CACHE_HANDLE                   hCache,
    IN XONLINETASK_HANDLE                   hTask,
    IN PBYTE                                pbIndexData
    )
{
    HRESULT hr = S_OK;
    PXONLINETASK_CACHE pCacheTask = NULL;

    Assert(hCache != NULL);
    Assert(hTask != NULL);
    Assert(pbIndexData != NULL);
    
    do
    {
         pCacheTask = (PXONLINETASK_CACHE)hTask;

        // Sets the current task parameters
        pCacheTask->SetCurrentTaskParams( XONLC_OPS_UPDATE_INDEX , pbIndexData);

        //
        // Start async work
        //
        hr = pCacheTask->DoUpdateIndexPrivBegin();

    }
    while( FALSE );
    
    return( hr );
}

//----------------------------------------------------------------------------
//
// Updates the record from the current cache address - async
//
//----------------------------------------------------------------------------
HRESULT CXo::CacheUpdateRecordPriv(
    IN XONLC_CACHE_HANDLE                   hCache,
    IN XONLINETASK_HANDLE                   hTask,
    IN PBYTE                                pbRecord
    )
{
    HRESULT hr = S_OK;
    PXONLINETASK_CACHE pCacheTask = NULL;

    Assert(hCache != NULL);
    Assert(hTask != NULL);
    Assert(pbRecord != NULL);

    do
    {
        pCacheTask = (PXONLINETASK_CACHE)hTask;

        // Sets the current task parameters
        pCacheTask->SetCurrentTaskParams( XONLC_OPS_UPDATE_RECORD , NULL , pbRecord);

        //
        // Get the address of the record
        //
        BOOL bAddressValid = hCache->GetRecordAddressFromCurrentAddress();

        Assert( bAddressValid);

        //
        // Start async work
        //
        hr = pCacheTask->DoUpdateRecordPrivBegin();
      
    }
    while( FALSE );
    
    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
//
// CAddress implementation
//
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//
// Constructors
//
//----------------------------------------------------------------------------
CAddress::CAddress() :
        dwSectorBegin(0),
        dwOffsetBegin(0),
        dwSectorEnd(0),
        dwOffsetEnd(0),
        dwPosition(0),
        dwSize(0)
{
}

CAddress::CAddress(const CAddress& adr)
{
    dwSectorBegin = adr.dwSectorBegin;
    dwOffsetBegin = adr.dwOffsetBegin;
    dwSectorEnd = adr.dwSectorEnd;
    dwOffsetEnd = adr.dwOffsetEnd;
    dwPosition = adr.dwPosition;
    dwSize = adr.dwSize;
    
}

CAddress& CAddress::operator=(const CAddress& adr)
{
    if( &adr != this )
    {
        dwSectorBegin = adr.dwSectorBegin;
        dwOffsetBegin = adr.dwOffsetBegin;
        dwSectorEnd = adr.dwSectorEnd;
        dwOffsetEnd = adr.dwOffsetEnd;
        dwPosition = adr.dwPosition;
        dwSize = adr.dwSize;
    }
    return (*this);
}

//----------------------------------------------------------------------------
//
// Increments an address
//
//----------------------------------------------------------------------------
VOID CAddress::IncrementAddress( DWORD dwAmmount )
{
    dwOffsetBegin += dwAmmount;
    while( XONLC_UTIL_SECTOR_SIZE <= dwOffsetBegin )
    {
        dwOffsetBegin -= XONLC_UTIL_SECTOR_SIZE;
        dwSectorBegin++;
        Assert(dwSectorBegin <= dwSectorEnd);
    }
    dwSize -= dwAmmount;
}

//----------------------------------------------------------------------------
//
// Decrements an address
//
//----------------------------------------------------------------------------
VOID CAddress::DecrementAddress( DWORD dwAmmount )
{
    if( dwAmmount > dwOffsetEnd )
    {
        dwOffsetEnd += XONLC_UTIL_SECTOR_SIZE;
        Assert(dwSectorEnd != 0);
        dwSectorEnd--;
    }

    dwOffsetEnd -= dwAmmount;

    dwSize -= dwAmmount;
}

//----------------------------------------------------------------------------
//
// Creates a relative address within a buffer 
//
//----------------------------------------------------------------------------
VOID CAddress::MakeRelativeAddress()
{
    // Make relative address in the cache buffer
    //
    dwSectorEnd -= dwSectorBegin;
    dwSectorBegin = 0;
}

//----------------------------------------------------------------------------
//
// Gets the address after cutting the first dwSectors
//
//----------------------------------------------------------------------------
VOID CAddress::CutFirstSectors(DWORD dwSectors)
{
    MakeRelativeAddress();

    Assert(dwSectors <= dwSectorEnd);

    dwSectorEnd -= dwSectors;
    dwOffsetBegin = 0;

    dwSize = dwSectorEnd * XONLC_UTIL_SECTOR_SIZE + dwOffsetEnd + 1;

}

//----------------------------------------------------------------------------
//
// Gets the address after cutting the last dwSectors
//
//----------------------------------------------------------------------------
VOID CAddress::CutLastSectors(DWORD dwSectors)
{
    MakeRelativeAddress();

    Assert(dwSectors <= dwSectorEnd);

    dwSize -= (dwSectorEnd - dwSectors) * XONLC_UTIL_SECTOR_SIZE + dwOffsetEnd + 1;

    dwSectorEnd = dwSectors - 1;
    dwOffsetEnd = XONLC_UTIL_SECTOR_SIZE - 1;

}

//////////////////////////////////////////////////////////////////////////////
//
// CCacheTask implementation
//
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//
// Function to initialize a context
//
//----------------------------------------------------------------------------
VOID CCacheTask::InitializeContext( 
            IN XONLC_CACHE_HANDLE   hCache, 
            IN HANDLE               hWorkEvent,
            IN CXo *                pXo
        )
{
    // Fill in the blanks
    ZeroMemory(this, sizeof(CCacheTask));
    
    m_hCache = hCache;
    m_pXo = pXo;

    m_pXo->TaskInitializeContext(&m_xonTask);
    m_xonTask.pfnContinue = CXo::CacheContinue;
    m_xonTask.pfnClose  = CXo::CacheTaskClose;
    m_xonTask.hEventWorkAvailable = hWorkEvent;
}

//----------------------------------------------------------------------------
//
// Sets the current task's parameters
//
//----------------------------------------------------------------------------
VOID CCacheTask::SetCurrentTaskParams(
            IN XONLC_OPS            cacheOp,
            IN PBYTE                pbIndexData,
            IN PBYTE                pbRecord,
            IN PXONLC_CONTEXT       pContext,
            IN BOOL                 bWriteIndex
            )
{
    m_cacheOp = cacheOp;
    m_pbIndexData = pbIndexData;
    m_pbRecord = pbRecord;
    m_pContext = pContext;
    m_bWriteIndex = bWriteIndex;

    m_hr = XONLINETASK_S_RUNNING;
    m_state = XONLC_STATE_INITIALIZE;
	m_hSubtask = NULL;
}

//----------------------------------------------------------------------------
//
// Starts async job for Open operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoOpenCacheBegin()
{
    HRESULT hr = S_OK;
    BOOL fPrepareNewCache = FALSE;

    Assert(m_hCache != NULL);
    
    Assert(NULL == m_hSubtask);

    do
    {
        //
        // Open cache file on disk
        //
        hr = m_hCache->OpenCacheFile();
        if( FAILED( hr ) )
        {
            break;
        }

        if( ERROR_ALREADY_EXISTS == GetLastError() )
        {
            //
            // Start a read to load its contents
            //
            m_hSubtask = m_hCache->GetFileIOSubtaskHandle();

            hr = m_hCache->StartFirstBlockUpload(m_hSubtask);
            if (FAILED(hr))
            {
                fPrepareNewCache = TRUE;
            }
			else
			{
				m_state = XONLC_STATE_READ;
			}
        }
        else
        {
            fPrepareNewCache = TRUE;
        }

        if (fPrepareNewCache)
        {
            //
            // Set size of the file and initialize first block
            //
            hr = m_hCache->PrepareNewCache();

            if( SUCCEEDED( hr ) )
            {
                m_hr = XONLINETASK_S_SUCCESS;
                m_state = XONLC_STATE_DONE;
								
                m_hCache->SetWorkEvent();
            }
        }
    }
    while(FALSE);

    if( FAILED( hr ) )
    {
        m_state = XONLC_STATE_DONE;
        m_hr = hr;
    }

	if(XONLC_STATE_DONE == m_state)
    {
        FinishAsyncWorkPriv();
    }

    return (hr);
}

//----------------------------------------------------------------------------
//
// Does the async work for Open operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoOpenCache()
{
    HRESULT                     hr = S_OK;

	Assert(NULL != m_hCache);

	if( XONLC_STATE_DONE == m_state )
    {
        // One more loop in dowork ... nothing to be done anymore
		Assert(NULL == m_hSubtask);
		m_hCache->UnlockCache();
        return (m_hr);
    }

    do
    {
        Assert(NULL != m_hSubtask);
        Assert(m_hCache->IsCacheFileOpen());

        // We will pump until the read returns
        m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);

        if(XONLINETASK_S_RUNNING != m_hr)
        {
            if (FAILED(m_hr))
            {
                m_state = XONLC_STATE_DONE;
                break;
            }

            if(XONLC_STATE_READ == m_state)
            {
                m_state = XONLC_STATE_DONE;

                hr = CheckReadWrite(m_hCache->GetFirstBlockSize());
                if( FAILED(hr) || m_hCache->IsCacheCorrupted() )
                {
                    //
                    // Reuse task handle
                    //
                    m_hSubtask = NULL;

                    m_hCache->UnlockCache();

                    m_hSubtask = m_hCache->GetCacheSubtaskHandle();
                    hr = m_pXo->CacheReset(m_hCache , m_hSubtask);

                    if( SUCCEEDED( hr ) )
                    {
                        //
                        // Jump to a RESET state
                        //
                        m_state = XONLC_STATE_RESET;
                        m_hr = XONLINETASK_S_RUNNING;
                    }
                    else
                    {
                        // Drop opening as reset didn't succeed
                        m_hr = hr;
                    }
                }
            }
            else //XONLC_STATE_RESET
            {
                //
                // RESET finished with success 
                //
                m_state = XONLC_STATE_DONE;
            }
        }
    }
    while(FALSE);

    if( XONLC_STATE_DONE == m_state )
    {
        FinishAsyncWork();

        if( XONLINETASK_STATUS_FAILED( m_hr ) )
        {
            if( m_hCache )
            {
                m_hCache->Delete();
                m_hCache = NULL;
            }
        }
    }

    return(m_hr);
}               

//----------------------------------------------------------------------------
//
// Starts the async work for Update operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoUpdateCacheBegin()
{
    HRESULT                     hr = S_OK;
    XONLC_CONTEXT               context;

    Assert(m_hCache != NULL);

    Assert(NULL == m_hSubtask);

    //
    // Prepare the context for Find
    //
    memset(&context , 0 , sizeof(XONLC_CONTEXT));
    context.pbIndexData = m_pbIndexData;
            
    m_hCache->ResetCurrentAddress();

    //
    // Parse the MRU List in search of the key
    //
    if(!m_hCache->FindInMRUList(DONT_GET_INDEX_DATA, &context)) 
    {
        //
        // Key not found in MRU - parse the index
        //
        m_hCache->FindInIndex(DONT_GET_INDEX_DATA , &context);
    }

    //
    // After Find m_hCache->m_dwCurrentIndexPosition contains one of the following
    // - position of the found item;
    // - position of a free entry;
    // - position of the LRU item.
    //
         
    m_hSubtask = m_hCache->GetCacheSubtaskHandle();
    if( m_hCache->HasRecords() )
    {
        //
        // Update the specified record on disk
        //
        hr = m_pXo->CacheUpdateRecordPriv(m_hCache, m_hSubtask, m_pbRecord);

        m_state = XONLC_STATE_UPDATE_RECORD;
    }
    else
    {
        if(TRUE == m_bWriteIndex)
        {
            //
            // m_hCache->m_dwCurrentIndexPos is the position in index to be updated
            //
            hr = m_pXo->CacheUpdateIndexPriv(m_hCache, m_hSubtask, m_pbIndexData);

            m_state = XONLC_STATE_UPDATE_INDEX;
        }
        else
        {
            //
            // Synchronously write index and MRU element in the memory and we're done
            //
            m_hCache->UpdateCurrentIndexEntry(m_pbIndexData);
            m_hCache->UpdateMRUList(m_pbIndexData, NULL);

            m_hr = XONLINETASK_S_SUCCESS;
            m_state = XONLC_STATE_DONE;
        }
    }

    if( FAILED( hr ) )
    {
        m_hr = hr;
        m_state = XONLC_STATE_DONE;
    }

	if(XONLC_STATE_DONE == m_state)
    {
        FinishAsyncWorkPriv();
    }


    return (hr);

}

//----------------------------------------------------------------------------
//
// Does the async work for Update operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoUpdateCache()
{
    HRESULT                     hr = S_OK;
    XONLC_CONTEXT               context;

	Assert(NULL != m_hCache);

	if( XONLC_STATE_DONE == m_state )
    {
        // One more loop in dowork ... nothing to be done anymore
        Assert(NULL == m_hSubtask);
		m_hCache->UnlockCache();
        return (m_hr);
    }
    
    do
    {
        Assert(NULL != m_hSubtask);

        /// We will pump until the write returns
        m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);

        if(XONLINETASK_S_RUNNING != m_hr)
        {
            if (FAILED(m_hr))
            {
                break;
            }

            //
            // Reuse task handle
            //
            m_hSubtask = NULL;

            //
            // If finished to UPDATE RECORD ...
            //
            if( XONLC_STATE_UPDATE_RECORD == m_state )
            {
               if(TRUE == m_bWriteIndex)
               {
                    m_hSubtask = m_hCache->GetCacheSubtaskHandle();
                    hr = m_pXo->CacheUpdateIndexPriv(m_hCache, m_hSubtask, m_pbIndexData);
                    
                    if( SUCCEEDED( hr ) )
                    {
                        //
                        // Jump to a new state: UPDATE INDEX
                        //
                        m_hr = XONLINETASK_S_RUNNING;
                        m_state = XONLC_STATE_UPDATE_INDEX;
                    }
               }
               else
               {
                    //
                    // Synchronously write index and MRU element in the memory and we're done
                    //
                    m_hCache->UpdateCurrentIndexEntry(m_pbIndexData);
                    m_hCache->UpdateMRUList(m_pbIndexData, NULL);

                    m_hr = XONLINETASK_S_SUCCESS;
                    m_state = XONLC_STATE_DONE;
			   }
            }
            else
            {
                //
                // If finished to UPDATE INDEX...
                //
                if( XONLC_STATE_UPDATE_INDEX == m_state )
                {
                    if( m_hCache->HasMRUList() )
                    {
                        m_hSubtask = m_hCache->GetCacheSubtaskHandle();
                        hr = m_pXo->CacheUpdateMRUListPriv(m_hCache, m_hSubtask, m_pbIndexData, m_pbRecord); 
                        if( SUCCEEDED( hr ) )
                        {
                            //
                            // Jump to a new state: UPDATE MRU LIST
                            //
                            m_hr = XONLINETASK_S_RUNNING;
                            m_state = XONLC_STATE_UPDATE_MRU;
                        }
                    }
                    else
                    {
                        //
                        // No MRU list...we're done
                        //
                        m_state = XONLC_STATE_DONE;
                    }
                }
                else
                {
                    //
                    // UPDATE MRU LIST finished
                    //
                    m_state = XONLC_STATE_DONE;
                }
            }
        }
    }
    while(FALSE);

    if( FAILED( hr ) )
    {
        m_hr = hr;
        m_state = XONLC_STATE_DONE;
    }

    if(XONLC_STATE_DONE == m_state)
    {
        FinishAsyncWork();
    }

    return(m_hr);
}

//----------------------------------------------------------------------------
//
// Starts the async job for IndexFlush operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoIndexFlushBegin()
{
    HRESULT                     hr = S_OK;

    Assert(m_hCache != NULL);

    Assert(NULL == m_hSubtask);


    m_hSubtask = m_hCache->GetFileIOSubtaskHandle();

    hr = m_hCache->StartFirstBlockDownload(m_hCache->GetFirstBlockSectors() , m_hSubtask);

    if( FAILED( hr ) )
    {
        m_state = XONLC_STATE_DONE;
        m_hr = hr;
		FinishAsyncWorkPriv();
    }
    else
    {
        m_state = XONLC_STATE_WRITE;
    }

    return (hr);
}

//----------------------------------------------------------------------------
//
// Does the async work for IndexFlush operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoIndexFlush()
{
    HRESULT                     hr = S_OK;


	if( XONLC_STATE_DONE == m_state )
    {
        // One more loop in dowork ... nothing to be done anymore
		Assert(NULL == m_hSubtask);
        return (m_hr);
    }

	Assert(NULL != m_hCache);

    do
    {
        Assert(NULL != m_hSubtask);

        // We will pump until the write returns
        m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);

        if(XONLINETASK_S_RUNNING != m_hr)
        {
            m_state = XONLC_STATE_DONE;

            if (FAILED(m_hr))
            {
                break;
            }

            // Check the write results
            hr = CheckReadWrite(m_hCache->GetFirstBlockSize());

            if( FAILED( hr ) )
            {
                m_hr = hr;
            }
        }
    }
    while(FALSE);

    if( XONLC_STATE_DONE == m_state )
    {
        FinishAsyncWork();

    }

    return(m_hr);
}

//----------------------------------------------------------------------------
//
// Starts the async work for Retrieve operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoRetrieveBegin()
{
    HRESULT                     hr = S_OK;

    Assert(m_hCache != NULL);

    Assert(NULL == m_hSubtask);

    //
    // Assign a file operation task
    //
    m_hSubtask = m_hCache->GetFileIOSubtaskHandle();

    hr = m_hCache->StartRetrieve(m_pContext , m_hSubtask);

    if( FAILED( hr ) )
    {
        m_hr = hr;
        m_state = XONLC_STATE_DONE;
		FinishAsyncWorkPriv();
    }
    else
    {
        m_state = XONLC_STATE_RETRIEVE;
    }

    return (hr);

}

//----------------------------------------------------------------------------
//
// Does the async work for Retrieve operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoRetrieve()
{
    HRESULT                     hr = S_OK;

    Assert(NULL != m_hCache);

	if( XONLC_STATE_DONE == m_state )
    {
        // One more loop in dowork ... nothing to be done anymore
		Assert(NULL == m_hSubtask);
        return (m_hr);
    }

    do
    {
        Assert(NULL != m_hSubtask);

        // We will pump until the read returns
        m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);

        if(XONLINETASK_S_RUNNING != m_hr)
        {
            if (FAILED(m_hr))
            {
                m_state = XONLC_STATE_DONE;
                break;
            }

            if( XONLC_STATE_CONT_RETRIEVE == m_state )
            {
                //
                // Finished CONT_RETRIEVE
                //
                m_state = XONLC_STATE_DONE;
            }

            if( XONLC_STATE_RETRIEVE == m_state )
            {
                //
                // If more data to be retrieved from disk...
                //
                if( m_hCache->ContinueRetrieve(m_pContext) )
                {
                    // Check the read results
                    hr = CheckRetrievedSectors(FALSE);

                    if( SUCCEEDED( hr ) )
                    {
                        //
                        // Reuse task handle
                        //
                        m_hSubtask = NULL;

                        //
                        // Assign a file operation task
                        //
                        m_hSubtask = m_hCache->GetFileIOSubtaskHandle();
                        
                        hr = m_hCache->FinishRetrieve(m_pContext , m_hSubtask);

                        if( SUCCEEDED( hr ) )
                        {
                            //
                            // Jump to CONT_RETRIEVE state
                            //
                            m_hr = XONLINETASK_S_RUNNING;           
                            m_state = XONLC_STATE_CONT_RETRIEVE;
                        }
                    }

                    if( FAILED( hr ))
                    {
                        //
                        // Invalidate index entry
                        //
                        m_hCache->DeleteCurrentIndexEntry();
                        break;
                    }
                }
                else
                {
                    //
                    // No more data to be retrieved from disk ... 
                    //
                    m_state = XONLC_STATE_DONE;
                }
            }

            if(XONLC_STATE_DONE == m_state)
            {
                // Check the read results
                hr = CheckRetrievedSectors(TRUE);

                if( SUCCEEDED( hr ) )
                {
                    //
                    // Check record for integrity
                    //
                    hr = m_hCache->CheckRetrieveResults(m_pContext);
                }

                if( FAILED( hr ) )
                {
                    //
                    // Invalidate index entry
                    //
                    m_hCache->DeleteCurrentIndexEntry();
                }
                else
                {
                    //
                    // Update MRU list in memory -  no more writing to disk at this time(lazy update)
                    //
                    m_hCache->UpdateMRUList(m_pContext->pbIndexData, m_pContext->pbRecordBuffer);
                }
            }
        }
    }
    while(FALSE);

    if( FAILED( hr ) )
    {
        m_hr = hr;
        m_state = XONLC_STATE_DONE;
    }

    if(XONLC_STATE_DONE == m_state)
    {
        FinishAsyncWork();
    }

    return(m_hr);
}

//----------------------------------------------------------------------------
//
// Starts async work for Reset operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoResetCacheBegin()
{
    HRESULT                     hr = S_OK;

    Assert(m_hCache != NULL);

    Assert(NULL == m_hSubtask);

    m_hCache->InitializeFirstBlock();

    m_hSubtask = m_hCache->GetFileIOSubtaskHandle();

    hr = m_hCache->StartFirstBlockDownload(m_hCache->GetFirstBlockSectors() , m_hSubtask);

    if( FAILED( hr ) )
    {
        m_state = XONLC_STATE_DONE;
        m_hr = hr;
		FinishAsyncWorkPriv();
    }
    else
    {
        m_state = XONLC_STATE_WRITE;
    }

    return (hr);

}

//----------------------------------------------------------------------------
//
// Does the async work for Reset operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoResetCache()
{
    HRESULT                     hr = S_OK;

	Assert(NULL != m_hCache);

	if( XONLC_STATE_DONE == m_state )
    {
        // One more loop in dowork ... nothing to be done anymore
		Assert(NULL == m_hSubtask);
        return (m_hr);
    }

    do
    {
        Assert(NULL != m_hSubtask);

        // We will pump until the write returns
        m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);

        if(XONLINETASK_S_RUNNING != m_hr)
        {
            m_state = XONLC_STATE_DONE;

            if (FAILED(m_hr))
            {
                break;
            }

            // Check the write results
            hr = CheckReadWrite(m_hCache->GetFirstBlockSize());
                
            if( FAILED( hr ) )
            {
                m_hr = hr;
            }
        }
    }
    while(FALSE);

    if(XONLC_STATE_DONE == m_state)
    {
        FinishAsyncWork();
    }

    return(m_hr);
}

//----------------------------------------------------------------------------
//
// Starts the async job for Close operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoCloseCacheBegin()
{
    HRESULT                     hr = S_OK;

    Assert(m_hCache != NULL);

    Assert(NULL == m_hSubtask);


    m_hSubtask = m_hCache->GetFileIOSubtaskHandle();

    hr = m_hCache->StartFirstBlockDownload(m_hCache->GetFirstBlockSectors() , m_hSubtask);

    if( FAILED( hr ) )
    {
        m_hCache->Delete();
        m_hCache = NULL;
        m_state = XONLC_STATE_DONE;
        m_hr = hr;

		FinishAsyncWorkPriv();
    }
    else
    {
        m_state = XONLC_STATE_WRITE;
    }

    return (hr);
}

//----------------------------------------------------------------------------
//
// Does the async work for Close operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoCloseCache()
{
    HRESULT                     hr = S_OK;


	if( XONLC_STATE_DONE == m_state )
    {
        // One more loop in dowork ... nothing to be done anymore
		Assert(NULL == m_hSubtask);
        return (m_hr);
    }

	Assert(NULL != m_hCache);

    do
    {
        Assert(NULL != m_hSubtask);

        // We will pump until the write returns
        m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);

        if(XONLINETASK_S_RUNNING != m_hr)
        {
            m_state = XONLC_STATE_DONE;

            if (FAILED(m_hr))
            {
                break;
            }

            // Check the write results
            hr = CheckReadWrite(m_hCache->GetFirstBlockSize());

            if( FAILED( hr ) )
            {
                m_hr = hr;
            }
        }
    }
    while(FALSE);

    if( XONLC_STATE_DONE == m_state )
    {
        FinishAsyncWork();

        m_hCache->Delete();
        m_hCache = NULL;
   }

    return(m_hr);
}

//----------------------------------------------------------------------------
//
// Starts async work for UpdateMRUList
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoUpdateMRUListPrivBegin()
{
    HRESULT                     hr = S_OK;

    Assert(m_hCache != NULL);

    Assert(NULL == m_hSubtask);

    //
    // Updates MRU List in memory
    //
    m_hCache->UpdateMRUList(m_pbIndexData , m_pbRecord);

    m_hSubtask = m_hCache->GetFileIOSubtaskHandle();

    hr = m_hCache->StartFirstBlockDownload(m_hCache->GetMRUListSectors() , m_hSubtask);

    if( FAILED( hr ) )
    {
        m_hr = hr;
        m_state = XONLC_STATE_DONE;
		
		FinishAsyncWorkPriv();
    }
    else
    {
        m_state = XONLC_STATE_WRITE;
    }

    return (hr);
}

//----------------------------------------------------------------------------
//
// Core function for UpdateMRU List
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoUpdateMRUListPriv()
{
    HRESULT                     hr = S_OK;
    ULARGE_INTEGER              uliOrigin;
    DWORD                       dwSize = 0;
    CAddress                    workAddress;

	Assert(NULL != m_hCache);

	if( XONLC_STATE_DONE == m_state )
    {
        // One more loop in dowork ... nothing to be done anymore
		Assert(NULL == m_hSubtask);
        return (m_hr);
    }

    do
    {
        Assert(NULL != m_hSubtask);

        // We will pump until the write returns
        m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);

        if(XONLINETASK_S_RUNNING != m_hr)
        {
            m_state = XONLC_STATE_DONE;

            if (FAILED(m_hr))
            {
                break;
            }

            // Check the write results
            hr = CheckReadWrite(XBOX_HD_SECTOR_SIZE * m_hCache->GetMRUListSectors());

        }
    }
    while(FALSE);

    if( FAILED( hr ) )
    {
        m_hr = hr;
    }

    if(XONLC_STATE_DONE == m_state)
    {
        FinishAsyncWorkPriv();
    }

    return(m_hr);
}

//----------------------------------------------------------------------------
//
// Starts async work for UpdateIndex operation
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoUpdateIndexPrivBegin()
{
    HRESULT                     hr = S_OK;

    Assert(m_hCache != NULL);

    Assert(NULL == m_hSubtask);

    m_hSubtask = m_hCache->GetFileIOSubtaskHandle();

    hr = m_hCache->StartWriteCurrentIndexEntry(m_pbIndexData , m_hSubtask);

    if( FAILED( hr ) )
    {
        m_hr = hr;
        m_state = XONLC_STATE_DONE;

        FinishAsyncWorkPriv();
    }
    else
    {
        m_state = XONLC_STATE_WRITE;
    }

    return (hr);
}

//----------------------------------------------------------------------------
//
// Core function for UpdateIndex
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoUpdateIndexPriv()
{
    HRESULT                     hr = S_OK;

	Assert(NULL != m_hCache);

	if( XONLC_STATE_DONE == m_state )
    {
        // One more loop in dowork ... nothing to be done anymore
		Assert(NULL == m_hSubtask);
        return (m_hr);
    }

    do
    {
        Assert(NULL != m_hSubtask);

        // We will pump until the write returns
        m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);

        if(XONLINETASK_S_RUNNING != m_hr)
        {
            m_state = XONLC_STATE_DONE;

            if (FAILED(m_hr))
            {
                break;
            }


            // Check the read/write results
            hr = CheckReadWrite(XBOX_HD_SECTOR_SIZE);
        }
    }
    while(FALSE);

    if( FAILED( hr ) )
    {
        m_hr = hr;
    }

    if(XONLC_STATE_DONE == m_state)
    {
        FinishAsyncWorkPriv();
    }

    return(m_hr);
}

//----------------------------------------------------------------------------
//
// Starts async work for UpdateRecord
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoUpdateRecordPrivBegin()
{
    HRESULT                     hr = S_OK;
    CAddress                    workAddress;

    Assert(m_hCache != NULL);

    Assert(NULL == m_hSubtask);

    //
    // Get first chunk of maximum XONLC_MAX_BUFFERS sectors
    //
    m_hCache->GetWorkAddress(&workAddress);

    //
    // Update first chunk, write check byte
    //
    m_hCache->MarkFirstUpdateChunk();

    m_hSubtask = m_hCache->GetFileIOSubtaskHandle();

    hr = UpdateNewChunk(&workAddress);

    if( FAILED( hr ) )
    {
        m_hr = hr;
        m_state = XONLC_STATE_DONE;

		FinishAsyncWorkPriv();
    }

    return (hr);
}

//----------------------------------------------------------------------------
//
// Core function for UpdateRecord
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::DoUpdateRecordPriv()
{
    HRESULT                     hr = S_OK;
    ULARGE_INTEGER              uliOrigin;
    DWORD                       dwSize = 0;
    CAddress                    workAddress;

	Assert(NULL != m_hCache);

	if( XONLC_STATE_DONE == m_state )
    {
        // One more loop in dowork ... nothing to be done anymore
		Assert(NULL == m_hSubtask);
        return (m_hr);
    }

    //
    // Get first chunk of maximum XONLC_MAX_BUFFERS sectors
    //
    m_hCache->GetWorkAddress(&workAddress);

    do
    {
        Assert(NULL != m_hSubtask);

        // We will pump until the write returns
        m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);

        if(XONLINETASK_S_RUNNING != m_hr)
        {
            if (FAILED(m_hr))
            {
                m_state = XONLC_STATE_DONE;
                break;
            }
                
            // Check the write results
            hr = CheckReadWrite((workAddress.dwSectorEnd - workAddress.dwSectorBegin +  1) *
                                sizeof(CSector));

            if (FAILED(hr))
            {
                break;
            }

            //
            // Reuse task handle
            //
            m_hSubtask = NULL;

            if( XONLC_STATE_READ == m_state )
            {
                //
                // Write the info back to the disk
                //
                m_hSubtask = m_hCache->GetFileIOSubtaskHandle();
                hr = m_hCache->WriteSectors(&workAddress , &m_pbRecord , m_hSubtask); 

                if( SUCCEEDED( hr ) )
                {
                    m_hr = XONLINETASK_S_RUNNING;
                    m_state = XONLC_STATE_WRITE;
                }
            }
            else // XONLC_STATE_WRITE
            {
                if( m_hCache->ContinueUpdateRecord() )
                {
                    //
                    // There is more work to do - current address in cache was modified
                    //
                    m_hCache->GetWorkAddress(&workAddress);

                    m_hSubtask = m_hCache->GetFileIOSubtaskHandle();
                    hr = UpdateNewChunk(&workAddress);

                    if( SUCCEEDED( hr ) )
                    {
                        m_hr = XONLINETASK_S_RUNNING;
                    }
                }
                else
                {
                    m_state = XONLC_STATE_DONE;
                }
            }
        }
    }
    while(FALSE);

    if( FAILED( hr ) )
    {
        m_hr = hr;
        m_state = XONLC_STATE_DONE;
    }

    if(XONLC_STATE_DONE == m_state)
    {
        FinishAsyncWorkPriv();
    }

    return(m_hr);
}

//----------------------------------------------------------------------------
//
// Checks if the size read/written is correct
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::CheckReadWrite( IN DWORD dwExpectedSize )
{
    DWORD dwSize = 0;
    HRESULT hr = S_OK;

    Assert(NULL != m_hSubtask);

    hr = m_pXo->GetReadWriteFileResults(m_hSubtask, &dwSize, NULL);

    if( SUCCEEDED( hr ) )
    {
        if( dwSize != dwExpectedSize )
        {
            hr = E_FAIL;
        }
    }

    return(hr);
}

//----------------------------------------------------------------------------
//
// Checks if the retrieved sectors are valid
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::CheckRetrievedSectors(BOOL bLastChunk)
{
    HRESULT hr = S_OK;

    DWORD dwSize = m_hCache->GetExpectedRetrieveSize(m_pContext , bLastChunk);
    
    hr = CheckReadWrite(dwSize);

    if( SUCCEEDED( hr ) )
    {
        hr = m_hCache->CheckRetrievedSectorsIntegrity(m_pContext , bLastChunk);
    }

    return (hr);

}

//----------------------------------------------------------------------------
//
// Starts async job for new chunk of record
//
//----------------------------------------------------------------------------
HRESULT CCacheTask::UpdateNewChunk(IN CAddress *pAddress)
{

    HRESULT hr = S_OK;

    Assert(NULL != m_hSubtask);

    if( (0 != pAddress->dwOffsetBegin) || (XONLC_UTIL_SECTOR_SIZE - 1 != pAddress->dwOffsetEnd))
    {
        //
        // There is some information in the sectors that needs to be preserved
        //
        //
        // Read the sectors that will be modified from disk
        //
        hr = m_hCache->ReadSectors( pAddress, m_hSubtask);

        m_state = XONLC_STATE_READ;

    }
    else
    {
        //
        // Directly write the sectors to the disk
        //
        hr = m_hCache->WriteSectors(pAddress , &m_pbRecord , m_hSubtask); 

        m_state = XONLC_STATE_WRITE;
    }

    return(hr);
}

//----------------------------------------------------------------------------
//
// Finish async work when state is XONLC_STATE_DONE
//
//----------------------------------------------------------------------------
VOID CCacheTask::FinishAsyncWork()
{
    m_hSubtask = NULL;

    if( NULL != m_hCache )
    {
        m_hCache->UnlockCache();
    }
}

//----------------------------------------------------------------------------
//
// Forces the task to completion 
//
//----------------------------------------------------------------------------
VOID CCacheTask::ForceCompletion()
{
   //
   // Some subtask involving the cache is in progress
   //
   if( NULL != m_hCache )
   {
	   if( m_hCache->IsCacheLocked() )
       {
		   if(NULL != m_hSubtask)
		   {
			   //
               // The subtask needs to be pumped - else, just unlock the cache
               //
			   do
			   {
				   m_hr = m_pXo->XOnlineTaskContinue(m_hSubtask);
			   }
			   while(XONLINETASK_S_RUNNING == m_hr);
		   }

		   // 
           //Unlock the cache so that it can be closed with an sync CacheClose() call
	       //
		   m_hCache->UnlockCache(); 
       }
  }
}

//----------------------------------------------------------------------------
//
// Finish async work when state is XONLC_STATE_DONE - used in the Priv async functions
// Doesn't unlock the cache
//
//----------------------------------------------------------------------------
VOID CCacheTask::FinishAsyncWorkPriv()
{
    m_hSubtask = NULL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CLocalCache implementation
//
//////////////////////////////////////////////////////////////////////////////

void CLocalCache::Delete()
{
    Assert(this == m_pXo->m_xonCacheHandles[m_cacheType]);

    //
    // Close the cache file handle 
    //
    if( INVALID_HANDLE_VALUE != m_hCacheFile )
    {
        CloseHandle(m_hCacheFile);
    }

    //
    // Release the memory that holds the Header, MRU list and Index
    //
    if( NULL != m_pbMemoryBlock )
    {
        m_pXo->SysFree(m_pbMemoryBlock);
    }

    //
    // Last thing, release the cache handle 
    //
    m_pXo->m_xonCacheHandles[m_cacheType] = NULL;

    m_pXo->SysFree(this);
}

//----------------------------------------------------------------------------
//
// Allocates memory for the cache
//
//----------------------------------------------------------------------------

HRESULT CLocalCache::Initialize(XONLC_TYPE type , PFNXONLC_COMPARE_HANDLER pfnCompare, HANDLE hWorkEvent, CXo * pXo)
{
    HRESULT  hr = S_OK;
    
    m_cacheType = type;
    m_hCacheFile = INVALID_HANDLE_VALUE;
    m_pfnCompareFunc = pfnCompare;
    m_hWorkEvent = hWorkEvent;
    m_pXo = pXo;
    m_cacheSubtask.InitializeContext(this, hWorkEvent, pXo);

    do
    {
        //
        // Initializes the offset for index and cached data within the util space (504/sector)
        //
        ComputeCacheOffsets();

        //
        // Allocates memory for the first block
        //
        DWORD dwSectorsCount = ComputeFirstBlockSectorsCount();
        m_pbMemoryBlock = (CSector *)pXo->SysAlloc(sizeof(CSector) * dwSectorsCount, PTAG_CSector);
            
        if( NULL == m_pbMemoryBlock )
        {
            hr = E_OUTOFMEMORY;
            break;
        }
        
        m_dwMemoryBlockSectors = dwSectorsCount;
        
        //
        // Pointer to the header
        //
        m_pHeader = (CHeader*)((m_pbMemoryBlock[0]).pbUtilSectorSpace);

    }
    while( FALSE );
    
    return( hr );

}

//----------------------------------------------------------------------------
//
// Writes the first block for a new created/reset cache file 
//
//----------------------------------------------------------------------------
VOID CLocalCache::InitializeFirstBlock( )
{
    Assert( 0 != m_dwIndexOffset);

    memset(m_pbMemoryBlock , 0 , m_dwMemoryBlockSectors * XBOX_HD_SECTOR_SIZE);

    m_pHeader->dwMaxCachedItems = g_xonCacheParams[m_cacheType][XONLC_PARAMS_MAX_CACHED_ITEMS];
    m_pHeader->dwMRUListSize = g_xonCacheParams[m_cacheType][XONLC_PARAMS_MRU_LIST_SIZE];
    m_pHeader->cbIndexDataSize = g_xonCacheParams[m_cacheType][XONLC_PARAMS_INDEX_DATA_SIZE];
    m_pHeader->cbKeyDataSize = g_xonCacheParams[m_cacheType][XONLC_PARAMS_KEY_DATA_SIZE];
    m_pHeader->cbCachedDataSize = g_xonCacheParams[m_cacheType][XONLC_PARAMS_CACHED_DATA_SIZE];
    m_pHeader->dwTimestampIncrement = 1;

    for( DWORD i = 0 ; i < m_pHeader->dwMRUListSize ; i++ )
    {
        GetMRUItemAddress( i );

        //
        // Mark first character as '*' - available
        //
        MarkCurrentEntryAsAvailable();
    }

    for( i = 0 ; i < m_pHeader->dwMaxCachedItems ; i++)
    {
        GetIndexItemAddress( i );

        //
        // Mark first character as '*' - available
        //
        MarkCurrentEntryAsAvailable();

    }
  
}

//----------------------------------------------------------------------------
//
// Open cache file on disk
//
//----------------------------------------------------------------------------
HRESULT CLocalCache::OpenCacheFile()
{
    Assert(m_hCacheFile == INVALID_HANDLE_VALUE);

    HRESULT hr = S_OK;
    BOOL bDone;

    NTSTATUS Status = 0;
    OBJECT_ATTRIBUTES Obja;
    HANDLE Handle;
    OBJECT_STRING FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    
    char szFileName[500];
    sprintf(szFileName , "\\Device\\Harddisk0\\partition1\\CACHE\\LocalCache%02d.bin" , m_cacheType);

    do
    {
        bDone = TRUE;

        //
        // Open cache file on disk
        //
        RtlInitObjectString(&FileName, szFileName);

        InitializeObjectAttributes(
                        &Obja,
                        &FileName,
                        OBJ_CASE_INSENSITIVE,
                        NULL,
                        NULL
                        );

        DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;

        Status = m_pXo->NtCreateFile(&m_hCacheFile,
                    (ACCESS_MASK)dwDesiredAccess | SYNCHRONIZE | FILE_READ_ATTRIBUTES,
                    &Obja,
                    &IoStatusBlock,
                    NULL,
                    FILE_ATTRIBUTE_NORMAL,
                    FILE_SHARE_READ,
                    FILE_OPEN_IF,
                    FILE_NON_DIRECTORY_FILE | FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_NONALERT
                    );

        if ( INVALID_HANDLE_VALUE == m_hCacheFile )
        {
            if( Status == STATUS_OBJECT_PATH_NOT_FOUND )
            {
                char lpPathName[500];
                sprintf(lpPathName , "\\Device\\Harddisk0\\partition1\\CACHE\\");

                RtlInitObjectString(&FileName, lpPathName);

                InitializeObjectAttributes(
                    &Obja,
                    &FileName,
                    OBJ_CASE_INSENSITIVE,
                    NULL,
                    NULL
                    );

                Status = m_pXo->NtCreateFile(
                            &Handle,
                            FILE_LIST_DIRECTORY | SYNCHRONIZE,
                            &Obja,
                            &IoStatusBlock,
                            NULL,
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            FILE_CREATE,
                            FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                            );

                if ( NT_SUCCESS(Status) ) 
                {
                    NtClose(Handle);
                    bDone = FALSE;
                }
                else 
                {
                    hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
                }

            }
            else
            {
                hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
            }
        }
        else
        {
            //
            // File opened/created
            //
            if(IoStatusBlock.Information == FILE_OPENED)
            {
                SetLastError(ERROR_ALREADY_EXISTS);
            }
        }
    }
    while(!bDone);

    return(hr);
}

//----------------------------------------------------------------------------
//
// Makes sure there is enough space on disk for a new created cache - initializes first block
//
//----------------------------------------------------------------------------
HRESULT CLocalCache::PrepareNewCache()
{
    HRESULT hr = S_OK;

    //
    // We know exactly how much data to cache.
    // 
    DWORD dwAlignedSize = GetTotalSectors() * XBOX_HD_SECTOR_SIZE;

    hr = m_pXo->SetEndOfFileNt( m_hCacheFile, dwAlignedSize);
    if( SUCCEEDED( hr ) )
    {
        InitializeFirstBlock();
    }

    return(hr);

}

//----------------------------------------------------------------------------
//
// Retrieves the address for the specified MRU list item
//
//----------------------------------------------------------------------------
BOOL CLocalCache::GetMRUItemAddress(IN DWORD dwPos)
{
    if(dwPos < m_pHeader->dwMRUListSize)
    {
        DWORD dwMRUItemBegin = sizeof(CHeader) + dwPos * GetMRUItemSize();
        DWORD dwMRUItemEnd = dwMRUItemBegin + GetMRUItemSize() - 1;

        m_currentAddress.FillAddress(dwMRUItemBegin, dwMRUItemEnd, dwPos);
        return TRUE;
    }

    return FALSE;

}

//----------------------------------------------------------------------------
//
// Retrieves the address for the specified index item
//
//----------------------------------------------------------------------------
BOOL CLocalCache::GetIndexItemAddress( IN DWORD dwPos )
{
    if(dwPos < m_pHeader->dwMaxCachedItems)
    {
        DWORD dwIndexItemSize = GetIndexEntrySize();
        DWORD dwIndexItemBegin = m_dwIndexOffset;
        DWORD dwIndexSpaceToSkip = dwPos * dwIndexItemSize;
        DWORD dwSizeLeftFromSector = XONLC_UTIL_SECTOR_SIZE - m_dwIndexOffset % XONLC_UTIL_SECTOR_SIZE;
        
        while(dwIndexSpaceToSkip > dwSizeLeftFromSector)
        {
            DWORD dwUtilSpace = dwSizeLeftFromSector - dwSizeLeftFromSector % dwIndexItemSize;
            dwIndexSpaceToSkip -= dwUtilSpace;
            dwIndexItemBegin += dwSizeLeftFromSector;

            dwSizeLeftFromSector = XONLC_UTIL_SECTOR_SIZE;
        }

        if( dwSizeLeftFromSector - dwIndexSpaceToSkip >= dwIndexItemSize )
        {
            dwIndexItemBegin += dwIndexSpaceToSkip;
        }
        else
        {
            dwIndexItemBegin += dwSizeLeftFromSector;
        }
        
        DWORD dwIndexItemEnd = dwIndexItemBegin + dwIndexItemSize - 1;

        m_currentAddress.FillAddress(dwIndexItemBegin, dwIndexItemEnd, dwPos);
        return TRUE;
    }

    return FALSE;
}

//----------------------------------------------------------------------------
//
// Retrieves the address for the specified record
//
//----------------------------------------------------------------------------
BOOL CLocalCache::GetRecordAddressFromCurrentAddress( )
{
    if(m_dwCurrentIndexPos < m_pHeader->dwMaxCachedItems)
    {
        DWORD dwCacheItemBegin = m_dwCachedDataOffset + m_dwCurrentIndexPos * GetRecordSize();
        DWORD dwCacheItemEnd = dwCacheItemBegin + GetRecordSize() - 1;

        m_currentAddress.FillAddress(dwCacheItemBegin, dwCacheItemEnd, m_dwCurrentIndexPos);
        return TRUE;
    }

    return FALSE;
}

//----------------------------------------------------------------------------
//
// Prepares the sectors in the specified range for writing on disk 
//
//----------------------------------------------------------------------------
VOID CLocalCache::PrepareSectorsForWrite(
        IN CSector                *pbMemoryBlock,
        IN DWORD                  dwSectorIndexBegin,
        IN DWORD                  dwSectorIndexEnd
        )
{
    Assert(NULL != pbMemoryBlock);
    Assert(dwSectorIndexBegin <= dwSectorIndexEnd);
    Assert(dwSectorIndexEnd < GetTotalSectors());


    for( DWORD dwPos = dwSectorIndexBegin ; dwPos <= dwSectorIndexEnd ; dwPos++)
    {
#ifdef XONLINE_FEATURE_XBOX 

		XCALCSIG_SIGNATURE Signature;
		HANDLE hSig = NULL;

 		// Compute signature
		hSig = XCalculateSignatureBegin(XCALCSIG_FLAG_NON_ROAMABLE);
		XCalculateSignatureUpdate(hSig, 
			                      (PBYTE)pbMemoryBlock[dwPos].pbUtilSectorSpace, 
			                      XONLC_UTIL_SECTOR_SIZE);
		XCalculateSignatureEnd(hSig, &Signature);

		memcpy(pbMemoryBlock[dwPos].pbSignature , &Signature , XONLC_SIGNATURE_SIZE);
#else
		memset(pbMemoryBlock[dwPos].pbSignature , 0 , XONLC_SIGNATURE_SIZE );
#endif
    }

}

//----------------------------------------------------------------------------
//
// Computes the number of sectors needed to store the info with the given size
//
//----------------------------------------------------------------------------
DWORD CLocalCache::ComputeSectorsCount( IN DWORD dwSize )
{
    Assert(0 != dwSize);

    DWORD dwSectorsCount = (dwSize - 1) / XONLC_UTIL_SECTOR_SIZE;
    if( 0 != (dwSize - 1) % XONLC_UTIL_SECTOR_SIZE )
    {
        dwSectorsCount++;
    }
    return dwSectorsCount;

}

//----------------------------------------------------------------------------
//
// Gets the total number of sectors for the file on disk
//
//----------------------------------------------------------------------------
DWORD CLocalCache::GetTotalSectors ()
{
    DWORD dwCacheSize = m_dwCachedDataOffset;
    if( HasRecords() )
    {
        dwCacheSize += GetMaxCachedItems() * GetRecordSize();
    }
    
    return ComputeSectorsCount(dwCacheSize);
}

//----------------------------------------------------------------------------
//
// Computes the offsets for index and cached records
//
//----------------------------------------------------------------------------
VOID CLocalCache::ComputeCacheOffsets()
{
    DWORD dwIndexItemSize = GetIndexEntrySize();

    m_dwIndexOffset = sizeof(CHeader) + GetMRUListSize() * GetMRUItemSize();
    
    DWORD dwSizeLeftFromSector = XONLC_UTIL_SECTOR_SIZE - m_dwIndexOffset % XONLC_UTIL_SECTOR_SIZE;
    if( dwSizeLeftFromSector < dwIndexItemSize )
    {
        //
        // Index entries don't span
        //
        m_dwIndexOffset += dwSizeLeftFromSector;
        dwSizeLeftFromSector = XONLC_UTIL_SECTOR_SIZE;
    }


    m_dwCachedDataOffset = m_dwIndexOffset;

    DWORD dwIndexSize = GetMaxCachedItems() * dwIndexItemSize;

    while(dwIndexSize > dwSizeLeftFromSector)
    {
        DWORD dwUtilSpace = dwSizeLeftFromSector - dwSizeLeftFromSector % dwIndexItemSize;
        dwIndexSize -= dwUtilSpace;
        m_dwCachedDataOffset += dwSizeLeftFromSector;

        dwSizeLeftFromSector = XONLC_UTIL_SECTOR_SIZE;
    }

    m_dwCachedDataOffset += dwIndexSize ;
}

//----------------------------------------------------------------------------
//
// Checks the parameters for this uploaded cache
//
//----------------------------------------------------------------------------
BOOL CLocalCache::IsCacheCorrupted()
{
    //
    // Verify the information in header 
    //
    if( m_pHeader->dwMaxCachedItems != g_xonCacheParams[m_cacheType][XONLC_PARAMS_MAX_CACHED_ITEMS] )
    {
        return TRUE;
    }       
    if( m_pHeader->dwMRUListSize != g_xonCacheParams[m_cacheType][XONLC_PARAMS_MRU_LIST_SIZE] )
    {
        return TRUE;
    }       
    if( m_pHeader->cbIndexDataSize != g_xonCacheParams[m_cacheType][XONLC_PARAMS_INDEX_DATA_SIZE] )
    {
        return TRUE;
    }       
    if( m_pHeader->cbKeyDataSize != g_xonCacheParams[m_cacheType][XONLC_PARAMS_KEY_DATA_SIZE] )
    {
        return TRUE;
    }       
    if( m_pHeader->cbCachedDataSize != g_xonCacheParams[m_cacheType][XONLC_PARAMS_CACHED_DATA_SIZE] )
    {
        return TRUE;
    }       

    for( DWORD i = 0 ; i < m_dwMemoryBlockSectors ; i++ )
    {
        if( IsSectorCorrupted( m_pbMemoryBlock , i ) )
        {
            return TRUE;
        }
    }

    return FALSE;

}

//----------------------------------------------------------------------------
//
// Searches the MRU List for the key specified in pContext
//
//----------------------------------------------------------------------------
BOOL CLocalCache::FindInMRUList(
            IN BOOL                               bOverwrite, // overwrite pContext->pbIndexData
            OUT PXONLC_CONTEXT                    pContext  
            )
{
    PBYTE pbCandidateIndexEntry = (PBYTE)m_pbBuffer;
    PBYTE pbTargetIndexData = pContext->pbIndexData;

    pContext->bValidDataInCache = FALSE;
    
    if( 0 == GetMRUListSize() )
    {
        //
        // No MRU List for this type
        //
        return FALSE;
    }

    Assert(1 == GetMRUListSize());

    GetMRUItemAddress( 0 );
        
    if( !IsCurrentEntryAvailable() )
     {
        //
        // Get the key 
        //
        GetDataFromCurrentAddress( GetIndexEntrySize(), 
                                    pbCandidateIndexEntry);

        pContext->bValidDataInCache = CompareIndexData(pbTargetIndexData , GetKey(pbCandidateIndexEntry));

        if(pContext->bValidDataInCache)
        {
            //
            // Copy the index data from the cache
            //
            if( bOverwrite )
            {
                memcpy(pContext->pbIndexData , 
                       GetIndexData(pbCandidateIndexEntry) , 
                       GetIndexDataSize());
            }
                
            BYTE bFirstCheckByte, bLastCheckByte;

            //
            // Get first check byte
            //
            GetDataFromCurrentAddress(sizeof(CHECKINFO) , (PBYTE)(&bFirstCheckByte));

            //
            // Set new m_dwCurrentIndexPos - Skip check byte
            //
            m_currentAddress.IncrementAddress(sizeof(CHECKINFO));
                        
            GetDataFromCurrentAddress(sizeof(INDEXPOS) , (PBYTE)(&m_dwCurrentIndexPos));

            pContext->bCallRetrieveNext = TRUE;

            m_currentAddress.IncrementAddress(sizeof(INDEXPOS) + GetIndexDataSize());

            if( GetCachedDataSize() > 0 )
            {
                if( NULL != pContext->pbRecordBuffer )
                {
                    //
                    // Copy the buffer from the MRU item
                    //
                    GetDataFromCurrentAddress(pContext->dwRecordBufferSize , pContext->pbRecordBuffer);
                    pContext->bCallRetrieveNext = FALSE;
                 }
            }
            else
            {
                pContext->bCallRetrieveNext = FALSE;
            }
                        
            //
            // Get to the last check byte
            //
            m_currentAddress.IncrementAddress(GetCachedDataSize());

            GetDataFromCurrentAddress(sizeof(CHECKINFO) , (PBYTE)(&bLastCheckByte));

            if( bFirstCheckByte != bLastCheckByte )
            {
                pContext->bValidDataInCache = FALSE;

                //
                // Delete entry from MRU List
                //
                GetMRUItemAddress(0);
                MarkCurrentEntryAsAvailable();
            }
        }
    }

    m_bFoundInMRUList = pContext->bValidDataInCache;

    return (pContext->bValidDataInCache);
}

//----------------------------------------------------------------------------
//
// Searches the index for the key specified in pContext
//
//----------------------------------------------------------------------------
BOOL CLocalCache::FindInIndex(
            IN BOOL                               bOverwrite, // overwrite pContext->pbIndexData
            OUT PXONLC_CONTEXT                    pContext  
            )
{
    BOOL bDontSaveAddress = FALSE;
    CAddress savedAddress;
    DWORD dwLRUTimestamp = 0xFFFFFFFF;

    PBYTE pbCandidateIndexEntry = (PBYTE)m_pbBuffer;
    PBYTE pbTargetIndexData = pContext->pbIndexData;

    pContext->bValidDataInCache = FALSE;
    
    
    m_dwCurrentIndexPos = 0;

    for( DWORD i = 0 ; (i < GetMaxCachedItems()) && (!(pContext->bValidDataInCache)) ; i++ )
    {
        GetIndexItemAddress( i );
        
        if(IsCurrentEntryAvailable())
        {
            if( !bDontSaveAddress )
            {
                //
                // Save the free address for future use
                //
                savedAddress = m_currentAddress;
                bDontSaveAddress = TRUE;
            }
        }
        else
        {
            //
            // Get the key 
            //
            GetDataFromCurrentAddress( GetIndexEntrySize(), 
                                       pbCandidateIndexEntry);

            pContext->bValidDataInCache = CompareIndexData(pbTargetIndexData , GetKey(pbCandidateIndexEntry));

            if(pContext->bValidDataInCache)
            {
                m_dwCurrentIndexPos = m_currentAddress.dwPosition;
                
                //
                // Copy the index data from the cache
                //
                if( bOverwrite )
                {
                    memcpy(pContext->pbIndexData , 
                           GetIndexData(pbCandidateIndexEntry) , 
                           GetIndexDataSize());
                }
                
                if( GetCachedDataSize() > 0  )
                {
                    pContext->bCallRetrieveNext = TRUE;
                }
                else
                {
                    pContext->bCallRetrieveNext = FALSE;
                }
            }
            else
            {
                //
                // Entry in use, save address applying LRU logic
                //
                if( !bDontSaveAddress )
                {
                    //
                    // No free entry found yet
                    //
                    if( dwLRUTimestamp >  GetTimestamp(pbCandidateIndexEntry) )
                    {
                        savedAddress = m_currentAddress;
                        dwLRUTimestamp = GetTimestamp(pbCandidateIndexEntry);
                    }
                    else
                    {
                        //
                        // This special case is encountered only when the TIMESTAMP data type resets
                        //
                        if( GetTimestamp(pbCandidateIndexEntry) >=  m_pHeader->dwTimestampIncrement)
                        {
                            savedAddress = m_currentAddress;
                            dwLRUTimestamp = 0;
                        }
                    }
                }
            }
        }

    } // end for

    if( !pContext->bValidDataInCache )
    {
        //
        // Key not found in cache - return as the current address, the saved address of an available entry
        //
        m_currentAddress = savedAddress;

        m_dwCurrentIndexPos = m_currentAddress.dwPosition;
    }

    return (pContext->bValidDataInCache);
}

//----------------------------------------------------------------------------
//
// Gets work address - first chunk for the cache buffer
//
//----------------------------------------------------------------------------
VOID CLocalCache::GetWorkAddress(CAddress *pAddress)
{
    Assert(NULL != pAddress);
    Assert(m_currentAddress.dwSectorEnd >= m_currentAddress.dwSectorBegin);

    *pAddress = m_currentAddress;

    //
    // The maximum number of sectors we can work with at some point is XONLC_MAX_BUFFERS
    //
    if( pAddress->dwSectorEnd - pAddress->dwSectorBegin >= XONLC_MAX_BUFFERS)
    {
        pAddress->dwSectorEnd = pAddress->dwSectorBegin + XONLC_MAX_BUFFERS - 1;
        pAddress->dwOffsetEnd = XONLC_UTIL_SECTOR_SIZE - 1;
    }

    pAddress->dwSize = XONLC_UTIL_SECTOR_SIZE * (pAddress->dwSectorEnd - pAddress->dwSectorBegin) +
                       pAddress->dwOffsetEnd + 1 -
                       pAddress->dwOffsetBegin;

}

//----------------------------------------------------------------------------
//
// Gets the data from a given address
//
//----------------------------------------------------------------------------
VOID CLocalCache::GetDataFromAddress(
        IN CSector             *pbSectors,
        IN CAddress            *pAddress,
        IN DWORD               dwDataSize,
        OUT PBYTE              pbData
        )
{
    Assert(NULL != pbSectors);
    Assert(NULL != pAddress);
    Assert(NULL != pbData);

    DWORD dwSpaceLeftInSector = XONLC_UTIL_SECTOR_SIZE - pAddress->dwOffsetBegin;
    PBYTE pbStartData = (pbSectors[pAddress->dwSectorBegin]).pbUtilSectorSpace + 
                         pAddress->dwOffsetBegin;

    while(dwSpaceLeftInSector <  dwDataSize)
    {
        //
        // The data spans two sectors
        //
        memmove(pbData, pbStartData , dwSpaceLeftInSector );

        pbStartData += XONLC_SIGNATURE_SIZE + dwSpaceLeftInSector;
        pbData +=dwSpaceLeftInSector;
        dwDataSize -= dwSpaceLeftInSector;
        dwSpaceLeftInSector = XONLC_UTIL_SECTOR_SIZE;
    }

    memmove(pbData, pbStartData, dwDataSize);

}

//----------------------------------------------------------------------------
//
// Reads sectors from disk - starts async job - address in m_hCache
//
//----------------------------------------------------------------------------
HRESULT CLocalCache::ReadSectors(
        IN CAddress                           *pAddress,
        IN XONLINETASK_HANDLE                 hTask
        )
{
    Assert(NULL != pAddress);
    Assert(NULL != hTask);

    ULARGE_INTEGER              uliOrigin;
    DWORD                       dwSectors;
    
    uliOrigin.QuadPart = pAddress->dwSectorBegin * XBOX_HD_SECTOR_SIZE;
    dwSectors = pAddress->dwSectorEnd - pAddress->dwSectorBegin + 1;

    m_pXo->ReadWriteFileInitializeContext(
                            m_hCacheFile,
                            (LPBYTE)m_pbBuffer,
                            dwSectors * XBOX_HD_SECTOR_SIZE,
                            uliOrigin,
                            m_hWorkEvent, 
                            (PXONLINETASK_FILEIO)hTask);

    return( m_pXo->ReadFileInternal((PXONLINETASK_FILEIO)hTask));
}

//----------------------------------------------------------------------------
//
// Write sectors to disk - starts async job - address in m_hCache
//
//----------------------------------------------------------------------------
HRESULT CLocalCache::WriteSectors(
        IN CAddress                           *pAddress,
        IN PBYTE                              *pbData,
        IN XONLINETASK_HANDLE                 hTask
        )
{
    Assert(NULL != pbData);
    Assert(NULL != pAddress);
    Assert(NULL != hTask);

    ULARGE_INTEGER              uliOrigin;
    DWORD                       dwSectors;
    
    uliOrigin.QuadPart = pAddress->dwSectorBegin * XBOX_HD_SECTOR_SIZE;
    dwSectors = pAddress->dwSectorEnd - pAddress->dwSectorBegin + 1;

    //
    // Write the info back to the disk
    //
    CAddress workAddress(*pAddress);
    
    workAddress.MakeRelativeAddress();


    *pbData = CopyDataToSectors(*pbData , &workAddress , XONLC_MAX_BUFFERS, WRITE_LAST_CHECK_BYTE , m_pbBuffer); 

    if( pAddress->dwSectorBegin == m_dwMemoryBlockSectors - 1 )
    {
        //
        // First sector in cache is the same as the last sector uploaded in memory
        //
        PBYTE pbDest = GetPointerInMemBlock(pAddress);
        PBYTE pbSource = GetPointerInBuffer(pAddress);
        memcpy (pbDest, pbSource , XONLC_UTIL_SECTOR_SIZE - pAddress->dwOffsetBegin);
     }

    PrepareSectorsForWrite(m_pbBuffer , 0 , dwSectors - 1);

    m_pXo->ReadWriteFileInitializeContext(
                            m_hCacheFile,
                            (LPBYTE)m_pbBuffer,
                            dwSectors * XBOX_HD_SECTOR_SIZE,
                            uliOrigin,
                            m_hWorkEvent, 
                            (PXONLINETASK_FILEIO)hTask);

    return( m_pXo->WriteFileInternal((PXONLINETASK_FILEIO)hTask));
}

//----------------------------------------------------------------------------
//
// Copies the cached record to sectors
//
//----------------------------------------------------------------------------
PBYTE CLocalCache::CopyDataToSectors(
        IN PBYTE               pbSrcData,
        IN CAddress            *pAddress,
        IN DWORD               dwMaxNumberOfBuffers,
        IN BOOL                bWriteLastCheckByte,
        OUT CSector            *pbDestData
        )
{
    Assert(NULL != pbSrcData);

    if( m_bUpdateFirstChunk )
    {
        //
        // Set beginning check byte
        //
        SetFirstCheckByte(pbDestData , pAddress);

        //
        // Set first chunk flag to FALSE
        //
        m_bUpdateFirstChunk = FALSE;

    }

    //
    // If this is the last chunk, write the check byte at the end of the record
    //
    if( bWriteLastCheckByte )
    {
        if( m_currentAddress.dwSectorEnd - m_currentAddress.dwSectorBegin < dwMaxNumberOfBuffers )
        {
            //
            // Set end check byte
            //
            SetLastCheckByte(pbDestData , pAddress);
        }
    }

    return CopyBufferToSectors(pbSrcData , pAddress , pbDestData);

}

//----------------------------------------------------------------------------
//
// Copies the info from a buffer into a sector array
//
//----------------------------------------------------------------------------
PBYTE CLocalCache::CopyBufferToSectors(
        IN PBYTE               pbSrcData,
        IN CAddress            *pAddress,
        OUT CSector            *pbDestData
        )
{
    Assert(NULL != pbSrcData);
    Assert(NULL != pbDestData);
    Assert(NULL != pAddress);

    DWORD dwDataSize = pAddress->dwSize;
    DWORD dwSpaceLeftInSector = XONLC_UTIL_SECTOR_SIZE - pAddress->dwOffsetBegin;
    PBYTE pbStartData = &((pbDestData[pAddress->dwSectorBegin]).pbUtilSectorSpace[pAddress->dwOffsetBegin]);

    while(dwSpaceLeftInSector <  dwDataSize)
    {
        //
        // The data spans two sectors
        //
        memcpy(pbStartData, pbSrcData , dwSpaceLeftInSector );

        pbStartData += XONLC_SIGNATURE_SIZE + dwSpaceLeftInSector;
        pbSrcData +=dwSpaceLeftInSector;
        dwDataSize -= dwSpaceLeftInSector;
        dwSpaceLeftInSector = XONLC_UTIL_SECTOR_SIZE;
    }

    memcpy(pbStartData, pbSrcData, dwDataSize);

    return (pbSrcData + dwDataSize);
}

//----------------------------------------------------------------------------
//
// Update the current index entry in memory
//
//----------------------------------------------------------------------------
VOID CLocalCache::UpdateCurrentIndexEntry(
        IN PBYTE                              pbIndexData
        )
{
    //
    // This function gets called after updating the record 
    //
    GetIndexItemAddress(m_dwCurrentIndexPos);

    Assert(m_currentAddress.dwSectorBegin == m_currentAddress.dwSectorEnd);

    PBYTE pbIndexEntry = GetCurrentIndexEntry();

    //
    // Write the check byte for record on disk
    //
    *(CHECKINFO*)(pbIndexEntry) = m_bCheckByte;

    //
    // Copy the core index entry
    //
    SetIndexData(pbIndexEntry , pbIndexData);
    
    //
    // Write timestamp
    //
    SetTimestamp(pbIndexEntry , (m_pHeader->dwTimestampIncrement++));

}

//----------------------------------------------------------------------------
//
// Starts writing the current index entry on disk
//
//----------------------------------------------------------------------------
HRESULT CLocalCache::StartWriteCurrentIndexEntry(
        IN PBYTE                              pbIndexData,
        IN XONLINETASK_HANDLE                 hTask
        )
{
    UpdateCurrentIndexEntry(pbIndexData);

    PrepareSectorsForWrite(m_pbMemoryBlock , m_currentAddress.dwSectorBegin , m_currentAddress.dwSectorEnd );

    ULARGE_INTEGER uliOrigin;

    uliOrigin.QuadPart = m_currentAddress.dwSectorBegin * XBOX_HD_SECTOR_SIZE;

    m_pXo->ReadWriteFileInitializeContext(
                            m_hCacheFile,
                            (LPBYTE)(&(m_pbMemoryBlock[m_currentAddress.dwSectorBegin])),
                            XBOX_HD_SECTOR_SIZE,
                            uliOrigin,
                            m_hWorkEvent,  
                            (PXONLINETASK_FILEIO)hTask);

    return( m_pXo->WriteFileInternal((PXONLINETASK_FILEIO)hTask));
}

//----------------------------------------------------------------------------
//
// Start retrieving a record
//
//----------------------------------------------------------------------------
HRESULT CLocalCache::StartRetrieve(
        IN PXONLC_CONTEXT                     pContext,
        IN XONLINETASK_HANDLE                 hTask
            )
{
    Assert(NULL != hTask);

    PBYTE pbIndexEntry = (PBYTE)m_pbBuffer;

    HRESULT hr = S_OK;
    
    do
    {
        GetIndexItemAddress(m_dwCurrentIndexPos);
        GetDataFromCurrentAddress(GetIndexEntrySize() , pbIndexEntry);

        
        if( !CompareIndexData( pContext->pbIndexData , GetKey( pbIndexEntry )))
        {
            //
            // Should call Lookup first
            //
            hr = E_FAIL;
            break;
        }
        
        m_bCheckByte = *(CHECKINFO*)pbIndexEntry;

        GetRecordAddressFromCurrentAddress();


        ULARGE_INTEGER              uliOrigin;
        DWORD                       dwSectors;
        PBYTE                       pbRetrieveBuffer;
                
        uliOrigin.QuadPart = m_currentAddress.dwSectorBegin * XBOX_HD_SECTOR_SIZE;

        //
        // Check if we can use the client's buffer
        //
        if( UsingClientBuffer(pContext) )
        {
            dwSectors = pContext->dwRecordBufferSize / XBOX_HD_SECTOR_SIZE;
            pbRetrieveBuffer = pContext->pbRecordBuffer;
        }       
        else
        {
            dwSectors = m_currentAddress.dwSectorEnd - m_currentAddress.dwSectorBegin + 1;

            Assert(dwSectors <= XONLC_MAX_BUFFERS);

            pbRetrieveBuffer = (PBYTE)m_pbBuffer;
        }

        m_pXo->ReadWriteFileInitializeContext(
                            m_hCacheFile,
                            pbRetrieveBuffer,
                            dwSectors * XBOX_HD_SECTOR_SIZE,
                            uliOrigin,
                            m_hWorkEvent,  
                            (PXONLINETASK_FILEIO)hTask);

        hr = m_pXo->ReadFileInternal((PXONLINETASK_FILEIO)hTask);

    }
    while( FALSE );
    
    return( hr );
    
}

//----------------------------------------------------------------------------
//
// Finish retrieving a record
//
//----------------------------------------------------------------------------
HRESULT CLocalCache::FinishRetrieve(
            IN PXONLC_CONTEXT                     pContext,
            IN XONLINETASK_HANDLE                 hTask
            )
{
    HRESULT hr = S_OK;

    Assert(NULL != hTask);
    
    ULARGE_INTEGER              uliOrigin;
    DWORD                       dwSectors;
    PBYTE                       pbRetrieveBuffer;

    Assert(UsingClientBuffer(pContext));            
    dwSectors = m_currentAddress.dwSectorEnd - m_currentAddress.dwSectorBegin + 1;

    uliOrigin.QuadPart = (m_currentAddress.dwSectorBegin + pContext->dwRecordBufferSize / XBOX_HD_SECTOR_SIZE) 
                         * XBOX_HD_SECTOR_SIZE;

    dwSectors -= pContext->dwRecordBufferSize / XBOX_HD_SECTOR_SIZE;

    Assert(dwSectors != 0);
    
    m_pXo->ReadWriteFileInitializeContext(
                            m_hCacheFile,
                            (LPBYTE)m_pbBuffer,
                            dwSectors * XBOX_HD_SECTOR_SIZE,
                            uliOrigin,
                            m_hWorkEvent, 
                            (PXONLINETASK_FILEIO)hTask);

    return (m_pXo->ReadFileInternal((PXONLINETASK_FILEIO)hTask));
}

//----------------------------------------------------------------------------
//
// Gets the expected retrieve size taking into account wheather is the first or second call
//
//----------------------------------------------------------------------------
DWORD CLocalCache::GetExpectedRetrieveSize(
            IN PXONLC_CONTEXT                     pContext,
            IN BOOL                               bLastChunk)
{
    DWORD dwSize = 0;

    Assert(NULL != pContext);

    if( UsingClientBuffer(pContext) )
    {
        //
        // Used the client's buffer + m_pbBuffer
        //
        if( bLastChunk )
        {
            dwSize = (m_currentAddress.dwSectorEnd - m_currentAddress.dwSectorBegin + 1 - 
                        pContext->dwRecordBufferSize / XBOX_HD_SECTOR_SIZE ) * XBOX_HD_SECTOR_SIZE;
        }
        else
        {
            dwSize = (pContext->dwRecordBufferSize / XBOX_HD_SECTOR_SIZE) * XBOX_HD_SECTOR_SIZE;            
        }
    }
    else
    {
        //
        // Used only m_pbBuffer
        //
        dwSize = (m_currentAddress.dwSectorEnd - m_currentAddress.dwSectorBegin + 1) * XBOX_HD_SECTOR_SIZE;

    }

    return (dwSize);
}

//----------------------------------------------------------------------------
//
// Checks retrieve results
//
//----------------------------------------------------------------------------
HRESULT CLocalCache::CheckRetrieveResults(
        IN PXONLC_CONTEXT                     pContext
        )
{

    BYTE bFirstCheckByte, bLastCheckByte;
    CAddress workAddress(m_currentAddress);
    PBYTE pbDst = pContext->pbRecordBuffer;
    HRESULT hr = S_OK;

    if( UsingClientBuffer(pContext) )
    {
        //
        // Used the client's buffer + m_pbBuffer
        //

        //
        // First retrieve info from the client's buffer
        //
        workAddress.CutLastSectors(pContext->dwRecordBufferSize / XBOX_HD_SECTOR_SIZE);

        CSector *pSectors = (CSector*)pContext->pbRecordBuffer;

        GetDataFromAddress(pSectors , &workAddress , sizeof(bFirstCheckByte) , &bFirstCheckByte);

        workAddress.IncrementAddress(sizeof(bFirstCheckByte));

        GetDataFromAddress(pSectors , &workAddress , workAddress.dwSize , pbDst);

        //
        // Then retrieve info from m_pbBuffer
        //
        pbDst += workAddress.dwSize;

        workAddress = m_currentAddress;

        workAddress.CutFirstSectors(pContext->dwRecordBufferSize / XBOX_HD_SECTOR_SIZE);

        GetDataFromAddress(m_pbBuffer , &workAddress , workAddress.dwSize - sizeof(bLastCheckByte), pbDst);

        workAddress.IncrementAddress(workAddress.dwSize - sizeof(bLastCheckByte));

        GetDataFromAddress(m_pbBuffer , &workAddress , sizeof(bLastCheckByte) , &bLastCheckByte);

    }
    else
    {
        //
        // Used only m_pbBuffer
        //
        workAddress.MakeRelativeAddress();

        GetDataFromAddress(m_pbBuffer , &workAddress , sizeof(bFirstCheckByte) , &bFirstCheckByte);

        workAddress.IncrementAddress(sizeof(bFirstCheckByte));

        GetDataFromAddress(m_pbBuffer , &workAddress , pContext->dwRecordBufferSize , pbDst);

        workAddress.IncrementAddress(pContext->dwRecordBufferSize);

        GetDataFromAddress(m_pbBuffer , &workAddress , sizeof(bLastCheckByte) , &bLastCheckByte);
    }

    if(( bLastCheckByte != bFirstCheckByte ) ||
        (bFirstCheckByte != m_bCheckByte))
    {
        pContext->bValidDataInCache = FALSE;
        hr = E_FAIL;
    }

    return (hr);
}

//----------------------------------------------------------------------------
//
// Checks the retrieved sectors integrity
//
//----------------------------------------------------------------------------
HRESULT CLocalCache::CheckRetrievedSectorsIntegrity(
            IN PXONLC_CONTEXT                     pContext,
            IN BOOL                               bLastChunk
            )
{
    CAddress workAddress(m_currentAddress);
    DWORD dwSector;
    CSector *pSectorsToCheck = m_pbBuffer;

    if( UsingClientBuffer(pContext) )
    {
        //
        // Used the client's buffer + m_pbBuffer
        //

        if( !bLastChunk )
        {
            //
            // Check info from the client's buffer
            //
            workAddress.CutLastSectors(pContext->dwRecordBufferSize / XBOX_HD_SECTOR_SIZE);
            pSectorsToCheck = (CSector*)pContext->pbRecordBuffer; 

        }
        else
        {
            //
            // Check info from m_pbBuffer
            //
            workAddress.CutFirstSectors(pContext->dwRecordBufferSize / XBOX_HD_SECTOR_SIZE);
        }

    }
    else
    {
        //
        // Used only m_pbBuffer
        //
        workAddress.MakeRelativeAddress();

    }

    for( dwSector = workAddress.dwSectorBegin ; dwSector < workAddress.dwSectorEnd ; dwSector++ )
    {
        if(IsSectorCorrupted(pSectorsToCheck , dwSector))
        {
            pContext->bValidDataInCache = FALSE;
            return (E_FAIL);
        }
    }

    return (S_OK);
}

//----------------------------------------------------------------------------
//
// Updates the MRU List in memory
//
//----------------------------------------------------------------------------
VOID CLocalCache::UpdateMRUList(PBYTE pbIndexData, PBYTE pbRecord)
{
    if( GetMRUListSize() == 0 )
    {
        //
        // No MRU List for this cache type
        //
        return;
    }

    Assert(NULL != pbIndexData);
    
    //
    // Write the most recent one
    //
    GetMRUItemAddress(0);

    PBYTE pbFirstMRUItem = GetPointerInMemBlock(&m_currentAddress);

    SetFirstCheckByte(m_pbMemoryBlock , &m_currentAddress);

    //
    // Sets the index entry correspondent to this MRU item
    //
    SetIndexPosition(pbFirstMRUItem, m_dwCurrentIndexPos);

    
    //
    // Copy pbIndexData
    //
    m_currentAddress.IncrementAddress(sizeof(m_dwCurrentIndexPos));

    m_currentAddress.dwSize = GetIndexDataSize();

    CopyDataToSectors(pbIndexData , &m_currentAddress , 
                      XONLC_UNLIMITED_BUFFERS, DONT_WRITE_LAST_CHECK_BYTE, m_pbMemoryBlock); 
    
    //
    // Copy pbRecord
    //
    if( NULL != pbRecord )
    {
        m_currentAddress.IncrementAddress(GetIndexDataSize());

        m_currentAddress.dwSize = GetRecordSize() - sizeof(CHECKINFO);

        CopyDataToSectors(pbRecord , &m_currentAddress , 
                          XONLC_UNLIMITED_BUFFERS, WRITE_LAST_CHECK_BYTE, m_pbMemoryBlock); 

    }
    else
    {
        m_currentAddress.IncrementAddress(GetCachedDataSize());
        SetLastCheckByte(m_pbMemoryBlock , &m_currentAddress);
    }

}

//----------------------------------------------------------------------------
//
// Sets beginning check byte
//
//----------------------------------------------------------------------------
VOID CLocalCache::SetFirstCheckByte(CSector *pbMemory , CAddress *pAddress)
{
    m_bCheckByte = GetFirstByte(pbMemory, pAddress) + 1;

    //
    // Don't use AVAILABLE as check byte
    //
    if( AVAILABLE == m_bCheckByte )
    {
        m_bCheckByte++;
    }

    //
    // Set the check bytes at the beginning of the data
    //
    SetFirstByte(pbMemory, pAddress, m_bCheckByte);
    pAddress->IncrementAddress(1);
}

//----------------------------------------------------------------------------
//
// Sets end check byte
//
//----------------------------------------------------------------------------
VOID CLocalCache::SetLastCheckByte(CSector *pbMemory , CAddress *pAddress)
{
    SetLastByte(pbMemory, pAddress, m_bCheckByte);
    pAddress->DecrementAddress(1);
}

//----------------------------------------------------------------------------
//
// Compare two index datas
//
//----------------------------------------------------------------------------
BOOL CLocalCache::CompareIndexData(PBYTE pbIndexData1 , PBYTE pbIndexData2)
{
    BOOL bFound = FALSE;

    if( NULL == m_pfnCompareFunc )
    {
        //
        // Memcmp
        //
        bFound = (0 == memcmp(pbIndexData1, pbIndexData2, GetKeySize()));
    }
    else
    {
        //
        // Custom comparison function
        //
        bFound = (m_pXo->*m_pfnCompareFunc)(pbIndexData1, pbIndexData2);
    }

    return bFound;
}

//----------------------------------------------------------------------------
//
// Delete current index entry
//
//----------------------------------------------------------------------------
VOID CLocalCache::DeleteCurrentIndexEntry()
{
    Assert(m_dwCurrentIndexPos < GetMaxCachedItems());

    GetIndexItemAddress( m_dwCurrentIndexPos );

    //
    // Mark first character as '*' - available
    //
    MarkCurrentEntryAsAvailable();

    if( HasMRUList() )
    {
        if( m_bFoundInMRUList )
        {
            //
            // Delete entry from MRU List
            //
            GetMRUItemAddress(0);
            MarkCurrentEntryAsAvailable();
        }
    }

    m_dwCurrentIndexPos = GetMaxCachedItems();
}
