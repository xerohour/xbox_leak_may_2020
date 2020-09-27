#ifndef DDRVMEM_H
#define DDRVMEM_H

#define EXTRA_LEAN_AND_MEAN
#include <windows.h>

// =========================================================
// Memory Management Macros
//
//      These macros are used for platform independent
//      memory operations.
//
//      NOTE:  If you have multiple threads
//             that compete for memory or run
//             into memory corruption problems.
//             uncomment the #define DDRV_SERIALIZE below
//
//      NOTE:  This does not support locking/unlocking mutual
//             exclusion.  If you have multiple threads or
//             code that can be interrupted and re-entered
//             then it is possible to get invalid pointers.
//             It shouldn't be too hard to add this support yourself
// =========================================================



extern HANDLE g_hD3DHAL_DriverHeap;

#define MEMALLOC(cbSize)    HeapAlloc (g_hD3DHAL_DriverHeap, HEAP_ZERO_MEMORY, (cbSize))
#define MEMFREE(lpPtr)      HeapFree (g_hD3DHAL_DriverHeap, 0, (lpPtr))

#define MEMCOPY(p1,p2,cbSize)   CopyMemory((void *)(p1), (void *)(p2), (DWORD)(cbSize))
#define MEMCLEAR(p1,cbSize)     ZeroMemory((void *)(p1), (DWORD)(cbSize))
#define MEMSET(p1,cbSize,val)   FillMemory((void *)(p1), (DWORD)(cbSize), (BYTE)val)



// =========================================================
// DDrvItemArray library.
//
//   This is used by the display drivers for dynamic memory
//   management of contexts, textures, and other objects
//   described by fixed-size structures.
//
// =========================================================

// Call this in DLL_PROCESS_ATTACH.
// If this function returns FALSE, you should fail the
// DLL load.
BOOL DDrvItemArrayInit ();

// Call this on DLL_PROCESS_DETACH.
BOOL DDrvItemArrayFini ();


// Handle to an itemarray
typedef DWORD HDDRVITEMARRAY, *LPHDDRVITEMARRAY;

// Handle to an item
typedef DWORD HDDRVITEM, * LPHDDRVITEM;


// Passes back a handle to the itemarray and returns TRUE if
// successful.
//
// nItems   - initial number of item slots in array
// dwSize   - size of each item in array (in Bytes)
// dwDelta  - growth factor (array grows by dwDelta slot
//            each time it is full and needs more space)
// lphArray - handle to newly created array
//
// Returns FALSE if the itemarray could not be created.
BOOL DDrvItemArrayCreate (DWORD nItems, DWORD dwSize,
                          DWORD dwDelta, LPHDDRVITEMARRAY lphArray);

// Destroys an itemarray.
// Returns FALSE if the itemarray handle is not valid.
BOOL DDrvItemArrayDestroy(HDDRVITEMARRAY hArray);

// Flushes an array, i.e. sets all item slots to empty (available)
// Returns FALSE if the itemarray handle is not valid.
BOOL DDrvItemArrayFlush (HDDRVITEMARRAY hArray);

// Allocate an item in array and return a small integer handle to it.
// The integer handle is guaranteed positive (0 is not a valid handle).
// Returns FALSE if it could not allocate enough room.
BOOL DDrvItemArrayAlloc (HDDRVITEMARRAY hArray, LPHDDRVITEM lphItem);

// Attempts to free an item.
BOOL DDrvItemArrayFree (HDDRVITEMARRAY hArray, HDDRVITEM hItem);

// Given an itemarray and an item handle, return a pointer to
// the item.
//
// Don't save these pointers. Allocating more items from the array
// may move the array's location in memory, invalidating any
// pointers into the array that may be lying around.
LPVOID DDrvItemArrayGetPtr (HDDRVITEMARRAY hArray, HDDRVITEM hItem);


// Callback Return type
enum DDRV_RETURN {
    DDRV_SUCCESS_STOP,
    DDRV_SUCCESS_CONTINUE,
    DDRV_ERROR_CONTINUE,
    DDRV_ERROR_STOP
};

// Generic Callback prototype
// should return one of the above values
typedef DWORD (*DDRVAPPLYFUNC)(void * lpData, HDDRVITEM hItem, DWORD dwData);


// Apply's a user defined callback function to
// each and every valid item in itemarray
BOOL DDrvItemArrayApplyValid (HDDRVITEMARRAY hArray, DDRVAPPLYFUNC lpCallback,
                              DWORD dwExtraData);

BOOL DDrvItemArrayApplyAll (HDDRVITEMARRAY hArray, DDRVAPPLYFUNC lpCallback,
                            DWORD dwExtraData);

BOOL DDrvItemArrayFindFirstValid (HDDRVITEMARRAY hArray, LPHDDRVITEM lpNextValid);

BOOL DDrvItemArrayFindNextValid (HDDRVITEMARRAY hArray, HDDRVITEM hStartFrom,
                                 LPHDDRVITEM lpNextValid);


#endif // DDRVMEM_H
