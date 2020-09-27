#ifndef __CALLOC_H__
#define __CALLOC_H__

// D3DX Memory Allocation Rules:
//
// The memory manager class CD3duAlloc will be able to provide useful debug
// information, if DBG is defined and you follow the rules below in D3DX.
// The memory statistics functions are are 
// DPF_ShowAllocStats() and DPF_ListUnfreedMemory().
//
// When DBG is *not* defined,
// all of the rules listed below will just automatically degenerate to plain
// vanilla C++ new/delete.
//
// 1) "Internal" memory.
//    To allocate objects and memory for use exclusively within D3DX, inherit
//    CD3duAlloc in your class, and just use the overloaded new and delete.
//
// 2) Interfaces
//    For interfaces which are to be returned to the app, such as the D3du
//    context interface, use ::new to create, and ::delete to destroy.
//    "::" forces the call not to use any overridden new, so this memory
//    does not get categorized as "Internal" memory.
//    For convenience, you can just use the #defines below for interfaces.

#define D3DX_INSTANTIATE_INTERFACE  ::new
#define D3DX_DESTROY_INTERFACE      ::delete

#if 0 // Debug memory allocator.  See below for the trivial non-debug version.

#define DPF_SHOWALLOCSTATS      DPF_ShowAllocStats
#define DPF_LISTUNFREEDMEMORY   DPF_ListUnfreedMemory

typedef struct _ALLOC_STUB
{
    LPVOID              pvMem;       // allocated memory
    DWORD               dwSize;       // size allocated
    DWORD               dwAllocationNumber; // allocation number 
                                            // (first allocation is allocation 1)
                              // This can help track down the source of unfreed memory.
    _ALLOC_STUB*        pNext;
} ALLOC_STUB;

void DPF_ListUnfreedMemory();
void DPF_ShowAllocStats();

class CD3duAlloc
{
public:
    CD3duAlloc();
    void* operator new(size_t size);
    void operator delete(void* pv);
};

#else

#define DPF_SHOWALLOCSTATS()
#define DPF_LISTUNFREEDMEMORY()

class CD3duAlloc
{
public:
    void* operator new(size_t size)
    {
        return new BYTE[size];
    }

    void operator delete(void* pv)
    {
        if( pv ) delete pv;
    }
};

#endif

extern CD3duAlloc g_CD3duAlloc; 

#endif //__CALLOC_H__
