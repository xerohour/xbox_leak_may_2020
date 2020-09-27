// =========================================================
// Memory allocator for display drivers.
// =========================================================
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

//
// Local Memory Management macros, variables, etc.
//

#define HEAP_SHARED 0x04000000UL    // put heap in shared memory

HANDLE g_hD3DHAL_DriverHeap = (HANDLE)NULL;

#define MEMINIT    {g_hD3DHAL_DriverHeap = (HANDLE)HeapCreate(HEAP_SHARED, 0x2000, 0 );}
#define MEMFINI     (HeapDestroy (g_hD3DHAL_DriverHeap) ? (g_hD3DHAL_DriverHeap = NULL, TRUE) : (g_hD3DHAL_DriverHeap = NULL, FALSE))
#define MEMCHECK    (g_hD3DHAL_DriverHeap ? TRUE : FALSE)



// Defaults
#define DDRV_DEFAULT_SIZE    4
#define DDRV_DEFAULT_DELTA   4

#define NOT_FOUND ((DWORD)-1)

typedef struct __ddrvitemarraydata {
    void    *pBase;         // Pointer to array of Items
    BOOL    *pValid;        // Pointer to boolean array of valid items
    DWORD   dwSize;         // Size of each Item in array (in Bytes)
    DWORD   dwDelta;        // Growth Factor of array
    DWORD   dwNumItems;     // Num of items in array
    BOOL    bValidArray;    // Is this array in use ???
} DDRVITEMARRAYDATA, *LPDDRVITEMARRAYDATA;

LPDDRVITEMARRAYDATA pArrays = 0;
DWORD dwNumArrays = 0;


/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemArrayInit
** Purpose: Creates an array of item arrays
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayInit ()
{
    // Initialize Memory ?!?
    if (! MEMCHECK) {
        MEMINIT;
        if (! MEMCHECK) {
            return FALSE;
        }
    }

    // Create Starting Arrays
    if (!pArrays) {
        pArrays = (LPDDRVITEMARRAYDATA) MEMALLOC (DDRV_DEFAULT_SIZE * sizeof(DDRVITEMARRAYDATA));
        if (! pArrays) {
            return FALSE;
        }

        dwNumArrays = DDRV_DEFAULT_SIZE;
    }
    return TRUE;
}


/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemArrayFini
** Purpose: Destroys an array of item arrays
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayFini ()
{
    BOOL ret;

    if (! MEMCHECK) {
        return TRUE;
    }

    // Cleanup Memory ?!?
    ret = MEMFINI;
    return ret;
}



/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemArrayCreate
** Purpose: Creates a new item array, returns handle to new array
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayCreate (DWORD nItems, DWORD dwSize,
                     DWORD dwDelta, LPHDDRVITEMARRAY phArray)
{
    DWORD i;

    // Find first empty array
    for (i = 0; i < dwNumArrays; i += 1)
        if (! pArrays[i].bValidArray)
            break;

    // If no empty arrays, then grow number of arrays
    if (i == dwNumArrays) {
        LPDDRVITEMARRAYDATA newArrays = (LPDDRVITEMARRAYDATA) MEMALLOC ((dwNumArrays+DDRV_DEFAULT_DELTA)*sizeof(DDRVITEMARRAYDATA));
        if (! newArrays) {
            return FALSE;
        }

        MEMCOPY(newArrays, pArrays, dwNumArrays * sizeof(DDRVITEMARRAYDATA));
        MEMFREE(pArrays);

        pArrays = newArrays;
        dwNumArrays += DDRV_DEFAULT_DELTA;
    }

    // Create new array of items
    dwSize = (dwSize + 7) & ~7;

    pArrays[i].pBase = MEMALLOC (nItems*dwSize);
    if (! pArrays[i].pBase) {
        return FALSE;
    }

    pArrays[i].pValid = (BOOL *) MEMALLOC (nItems * sizeof(BOOL));
    if (! pArrays[i].pValid) {
        MEMFREE(pArrays[i].pBase);
        pArrays[i].pBase = NULL;
        return FALSE;
    }

    pArrays[i].dwSize       = dwSize;
    pArrays[i].dwDelta      = dwDelta;
    pArrays[i].dwNumItems   = nItems;
    pArrays[i].bValidArray  = TRUE;

    *phArray = (i+1);
    return TRUE;
}


/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemArrayDestroy
** Purpose: Destroys an item array
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayDestroy (HDDRVITEMARRAY hArray)
{
    DWORD dwArray = hArray - 1;
    LPDDRVITEMARRAYDATA lpArray = pArrays + dwArray;

    if ((dwArray >= dwNumArrays) || (! lpArray->bValidArray))
    {
        return FALSE;
    }

    // Destroy array of items
    if (lpArray->pValid) {
        MEMFREE (lpArray->pValid);
        lpArray->pValid = NULL;
    }

    if (lpArray->pBase) {
        MEMFREE (lpArray->pBase);
        lpArray->pBase = NULL;
    }

    lpArray->bValidArray = FALSE;
    return TRUE;
}



/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemArrayDestroy
** Purpose: Flushes an item array by marking all item slots
**          as empty
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayFlush (HDDRVITEMARRAY hArray)
{
    DWORD i;
    DWORD dwArray = hArray - 1;
    LPDDRVITEMARRAYDATA lpArray = pArrays + dwArray;

    if ((dwArray >= dwNumArrays) || (! lpArray->bValidArray))
    {
        return FALSE;
    }

    // Mark all items in array as empty
    for (i = 0; i < lpArray->dwNumItems; i++) {
        lpArray->pValid[i] = FALSE;
    }

    return TRUE;
}



/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemArrayAlloc
** Purpose: Creates a new item in specified item array
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayAlloc (HDDRVITEMARRAY hArray, LPHDDRVITEM lphItem)
{
    DWORD dwArray = hArray - 1;
    DWORD i;
    LPDDRVITEMARRAYDATA lpArray = pArrays + dwArray;

    if ((dwArray >= dwNumArrays) || (! lpArray->bValidArray))
    {
        return FALSE;
    }

    // Find first empty item in array
    for (i = 0; i < lpArray->dwNumItems; i++) {
        if (! (lpArray->pValid[i]))
            break;
    }

    // If no empty items in array
    // then grow array
    if (i == lpArray->dwNumItems) {
        BOOL * newpValid;
        LPVOID newpBase;
        DWORD  dwNewSize = lpArray->dwNumItems + lpArray->dwDelta;

        // Check for static array
        if (! lpArray->dwDelta) {
            return FALSE;
        }

        newpBase = MEMALLOC (dwNewSize * lpArray->dwSize);
        if (! newpBase) {
            return FALSE;
        }

        newpValid = (BOOL *) MEMALLOC ((dwNewSize) * sizeof(BOOL));
        if (! newpValid) {
            MEMFREE (newpBase);
            return FALSE;
        }

        MEMCOPY (newpValid, lpArray->pValid, lpArray->dwNumItems * sizeof(DWORD));
        MEMCOPY (newpBase, lpArray->pBase, lpArray->dwNumItems * lpArray->dwSize);

        MEMFREE (lpArray->pValid);
        lpArray->pValid = newpValid;

        MEMFREE (lpArray->pBase);
        lpArray->pBase  = newpBase;

        lpArray->dwNumItems += lpArray->dwDelta;
    }

    // Return handle to item
    lpArray->pValid[i] = TRUE;
    *lphItem = i+1;
    return TRUE;
}

/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemArrayGetPtr
** Purpose: Gets a pointer to item in item array
**-----------------------------------------------------------------------------
*/

LPVOID
DDrvItemArrayGetPtr (HDDRVITEMARRAY hArray, HDDRVITEM hItem)
{
    DWORD dwItem    = hItem - 1;
    DWORD dwArray   = hArray - 1;
    LPDDRVITEMARRAYDATA lpArray = pArrays + dwArray;

    if (hArray >= dwNumArrays ||
        lpArray->bValidArray == FALSE ||
        dwItem >= lpArray->dwNumItems ||
        lpArray->pValid[dwItem] == FALSE) {
        return NULL;
    }

    // Return pointer to item
    return ((char *)(lpArray->pBase) + dwItem * lpArray->dwSize);
}



/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemArrayFree
** Purpose: Frees item from item array
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayFree (HDDRVITEMARRAY hArray, HDDRVITEM hItem)
{
    DWORD dwItem  = hItem - 1;
    DWORD dwArray = hArray - 1;
    LPDDRVITEMARRAYDATA lpArray = pArrays + dwArray;

    if (hArray >= dwNumArrays ||
        lpArray->bValidArray == FALSE ||
        dwItem >= lpArray->dwNumItems ||
        lpArray->pValid[dwItem] == FALSE) {
        return FALSE;
    }

    // Set valid flag to false
    lpArray->pValid[dwItem] = FALSE;
    return TRUE;
}



/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemFindFirstValid
** Purpose: Finds first valid item in array
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayFindFirstValid (
    HDDRVITEMARRAY  hArray,
    LPHDDRVITEM     lpNextValid)
{
    DWORD i;
    DWORD dwArray = hArray - 1;
    LPDDRVITEMARRAYDATA lpArray = pArrays + dwArray;

    if ((dwArray >= dwNumArrays) || (! lpArray->bValidArray))
    {
        return FALSE;
    }

    // Find first valid array item
    for (i = 0; i < lpArray->dwNumItems; i++) {
        if (lpArray->pValid[i]) {
            if (lpNextValid) {
                *lpNextValid = i + 1;
            }
            return TRUE;
        }
    }

    return FALSE;
} // End Find First Valid


/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemFindNextValid
** Purpose: Finds next valid item in array,
**          after specified start item
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayFindNextValid (
    HDDRVITEMARRAY  hArray,
    HDDRVITEM       hStartFrom,
    LPHDDRVITEM     lpNextValid)
{
    DWORD i;
    DWORD dwArray = hArray - 1;
    LPDDRVITEMARRAYDATA lpArray = pArrays + dwArray;

    if ((dwArray >= dwNumArrays) || (! lpArray->bValidArray))
    {
        return FALSE;
    }

    // Find next valid array item after hStartFrom
    for (i = hStartFrom; i < lpArray->dwNumItems; i++) {
        if (lpArray->pValid[i]) {
            if (lpNextValid) {
                *lpNextValid = i + 1;
            }
            return TRUE;
        }
    }

    return FALSE;
} // End Find Next Valid


/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemArrayApplyValid
** Purpose: Apply specified callback to each valid item in array
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayApplyValid (
    HDDRVITEMARRAY  hArray,
    DDRVAPPLYFUNC   lpCallback,
    DWORD           dwExtra)
{
    DWORD i;
    DWORD dwRet;
    DWORD dwArray = hArray - 1;
    DWORD dwSuccessCount;
    LPDDRVITEMARRAYDATA lpArray = pArrays + dwArray;

    dwSuccessCount = 0;
    if ((dwArray >= dwNumArrays) || (! lpArray->bValidArray))
        return (dwSuccessCount);

    if (! lpCallback)
        return (dwSuccessCount);

    for (i = 0; i < lpArray->dwNumItems; i++) {
        if (lpArray->pValid[i]) {

            // Call user callback
            dwRet = (*lpCallback)(((char *)(lpArray->pBase) + (i * lpArray->dwSize)),
                                  (i+1),
                                  dwExtra);

            // Check Return Value
            switch (dwRet)
            {
            case DDRV_SUCCESS_CONTINUE:
                dwSuccessCount++;
                break;
            case DDRV_ERROR_CONTINUE:
                break;

            case DDRV_SUCCESS_STOP:
                return (dwSuccessCount);

            case DDRV_ERROR_STOP:
            default:
                return (dwSuccessCount);
            }
        }
    }

    return (dwSuccessCount);
} // End DDrvItemArrayApplyValid


/*
**-----------------------------------------------------------------------------
** Name:    DDrvItemApplyAll
** Purpose: Apply specified callback to each item in array
**-----------------------------------------------------------------------------
*/

BOOL
DDrvItemArrayApplyAll (
    HDDRVITEMARRAY  hArray,
    DDRVAPPLYFUNC   lpCallback,
    DWORD           dwExtra)
{
    DWORD i;
    DWORD dwRet;
    DWORD dwArray = hArray - 1;
    DWORD dwSuccessCount;
    LPDDRVITEMARRAYDATA lpArray = pArrays + dwArray;

    dwSuccessCount = 0;
    if ((dwArray >= dwNumArrays) || (! lpArray->bValidArray))
        return (dwSuccessCount);

    if (! lpCallback)
        return (dwSuccessCount);

    for (i = 0; i < lpArray->dwNumItems; i++) {
        // Call user callback
        dwRet = (*lpCallback)(((char *)(lpArray->pBase) + (i * lpArray->dwSize)),
                              (i+1),
                              dwExtra);

        // Check Return Value
        switch (dwRet)
        {
        case DDRV_SUCCESS_CONTINUE:
            dwSuccessCount++;
            break;
        case DDRV_ERROR_CONTINUE:
            break;

        case DDRV_SUCCESS_STOP:
            return (dwSuccessCount);

        case DDRV_ERROR_STOP:
        default:
            return (dwSuccessCount);
        }
    }

    return (dwSuccessCount);
} // End DDrvItemArrayApplyAll

#endif  // NVARCH >= 0x04

