/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: memory.h                                                                    *
* Description: tracks memory allocations and leaks                                      *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/10/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_MEMORY_H__
#define __SPARTA_MEMORY_H__

#define NO_MEMORY_TRACKING

#ifdef __cplusplus
extern "C"
{
#endif

#if (defined DBG) && (!defined NO_MEMORY_TRACKING)

    // MEM_Allocate - allocate memory
    
    #define MEM_Allocate(X) _sparta_Allocate(X,TEXT(__FILE__),__LINE__)
    
    // MEM_Free - frees memory
    
    #define MEM_Free(X)     _sparta_Free(X,TEXT(__FILE__),__LINE__)
    
    // MEM_Dump - shows a dump of memory usage
    
    VOID MEM_Dump();
    
    // MEM_FlushAll - frees all currently allocated memory - WARNING this can cause serious errors
    //                make sure it's only called at an appropriate time
    
    VOID MEM_FlushAll();
    

    PVOID _sparta_Allocate(DWORD dwSize, TCHAR *pszFilename, DWORD dwLine);
    VOID _sparta_Free(PVOID pvMemory, TCHAR *pszFilename, DWORD dwLine);

    #define DEBUG_MEMORY_ENABLED

#else // for non-debug versions

    // MEM_Allocate - allocate memory
    
    #define MEM_Allocate(X) malloc(X)
    
    // MEM_Free - frees memory
    
    #define MEM_Free(X)     free(X)
    
    // these are meaningless in a non-debug call
    #define MEM_Dump()
    
    // these are meaningless in a non-debug call
    #define MEM_FlushAll()

#endif // debug version ?

#ifdef __cplusplus
} // for the extern "C"
#endif

#endif __SPARTA_MEMORY_H__

