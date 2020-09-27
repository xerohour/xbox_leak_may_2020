
extern HANDLE hGlobalHeap;

void MyHeapInit( void );

void __inline MyHeapFree( PVOID pMem ) {
    ASSERT( hGlobalHeap );
    HeapFree( hGlobalHeap, 0, pMem );
    }

DWORD __inline MyHeapSize( PVOID pMem ) {
    ASSERT( hGlobalHeap );
    return HeapSize( hGlobalHeap, 0, pMem );
    }

PVOID MyHeapAlloc( DWORD dwSize );

PVOID MyHeapReAlloc( PVOID pMem, DWORD dwSize );

PVOID MyAllocNeverFree( UINT nSize );

PVOID MyAllocNeverFreeLocked( UINT nSize );

PVOID MyVirtualAlloc( DWORD dwSize );

VOID MyVirtualFree( PVOID pMem );

BOOL MyVirtualLock( PVOID pMem, DWORD dwSize );

typedef struct _HASH_AND_LENGTH HASH_AND_LENGTH, *PHASH_AND_LENGTH;
typedef struct _BNODE BNODE, *PBNODE;

struct _HASH_AND_LENGTH {
    MD5_HASH  Hash;
    DWORDLONG Length;
    };

struct _BNODE {
    PBNODE          pLeft;
    PBNODE          pRight;
    HASH_AND_LENGTH HashAndLength;
    PVOID           pContext;
    };


BOOL
LookupOrAddBnode(
    PBNODE           *ppTree,           // IN OUT
    PHASH_AND_LENGTH  pHashAndLength,   // IN
    PVOID             pContext,         // IN
    PBNODE           *ppBnode           // OUT
    );

#ifdef DEBUG
    extern UINT nMaxBtreeDepth;
#endif


typedef struct _RECYCLE_LIST_NODE RECYCLE_LIST_NODE, *PRECYCLE_LIST_NODE;
typedef struct _RECYCLE_LIST_HEAD RECYCLE_LIST_HEAD, *PRECYCLE_LIST_HEAD;

struct _RECYCLE_LIST_NODE {
    PRECYCLE_LIST_NODE NextNode;
    PVOID              Object;
    };

struct _RECYCLE_LIST_HEAD {
    PRECYCLE_LIST_NODE FirstNode;       // creator must initialize to NULL
    };

VOID
InitializeRecyclePackage(
    VOID
    );

PVOID
GetObjectFromRecycleList(
    PRECYCLE_LIST_HEAD ListHead
    );

VOID
RecycleObjectToList(
    PRECYCLE_LIST_HEAD ListHead,
    PVOID              Object
    );

HANDLE
AllocateAutoEvent(          // state guaranteed to be not signaled
    VOID
    );

VOID
RecycleAutoEvent(           // should only recycle events in not signaled state
    HANDLE hEvent
    );

HANDLE
AllocateManualEvent(        // state guaranteed to be not signaled
    VOID
    );

VOID
RecycleManualEvent(         // doesn't matter state returned -- force reset
    HANDLE hEvent
    );


BOOL
__inline
EventStateUnsignaled(
    HANDLE hEvent
    )
    {
    return ( WaitForSingleObject( hEvent, 0 ) != WAIT_OBJECT_0 );
    }

