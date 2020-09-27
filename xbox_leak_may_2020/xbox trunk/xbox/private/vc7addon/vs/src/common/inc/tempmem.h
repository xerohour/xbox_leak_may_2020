//---------------------------------------------------------------------------
// Copyright (C) 1991-1998, Microsoft Corporation
//                 All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//---------------------------------------------------------------------------
/*** 
*tempmem.h - Prototypes for simple stack based memory allocatiocation
*
*Revision History:
*
* [00]	19-Aug-98 marioch: Created.
*
*Implementation Notes:
*
*****************************************************************************/

#ifndef TEMPMEM_H_INCLUDED
#define TEMPMEM_H_INCLUDED

#include <malloc.h>
#include <windows.h>

#ifdef	__cplusplus

extern "C" {

inline UINT StackAvailable()
{
    INT iLocal;
    static void *pvStackBottom = 0;

    //CONSIDER: for SIZE and SPEED - initialize pvStackBottom at startup
    if (!pvStackBottom) {
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery(&iLocal, &mbi, sizeof(mbi));
	pvStackBottom = mbi.AllocationBase;
    }
    return (UINT) (((INT_PTR) &iLocal) - (INT_PTR)pvStackBottom);
}


/* this is an internal class that shouldn't be used directly. 
  Use the TEMPBUFF macros instead
*/
class TempBufferEx {
public:
    TempBufferEx (size_t stacksize, size_t allocasize, size_t size, 
                  char* stackbuffer, size_t* allocasize_out);

    ~TempBufferEx();
	void* GetBuffer() {
          return (void*)m_pcBuffer;
	}
private:
    enum alloc_kind {ALLOCA, LOCAL, HEAP};
    alloc_kind m_where_allocated;
    char* m_pcBuffer;
    void *m_pvRegCookie;
};

/* Macro to declare and efficiently manage a temporary buffer. 
 * Var is the variable name
 * Type is its type
 * size is the actual (known at runtime) size of the buffer
 * Example: 
 * TEMPBUF(tempchars, char*, 2*len(initchars));
 *
 * NOTE: you must check the pointer to the buffer for NULL in case
 *       the allocator wasn't able to allocate the memory.
*/
#define TEMPBUF(var,type,size) char __TempBuf##var[120];\
TempBufferEx __TempBuffer##var(120,0,(size),(char*)&__TempBuf##var,0);\
type var = (type)__TempBuffer##var.GetBuffer();

/* Macro to declare and efficiently manage a temporary buffer. 
 * Var is the variable name
 * Type is its type
 * sizestack (constant) is the maximum stack size that can be reserved on the stack 
 * (40-100 is a good number)
 * size is the actual (known at runtime) size of the buffer
 * Example: 
 * TEMPBUFEX(tempchars, char*, 50,2*len(initchars));
 *
 * NOTE: you must check the pointer to the buffer for NULL in case
 *       the allocator wasn't able to allocate the memory.
*/
#define TEMPBUFEX(var,type,sizestack,size) char __TempBuf##var[sizestack];\
TempBufferEx __TempBuffer##var(sizestack,0,(size),(char*)&__TempBuf##var,0);\
type var = (type)__TempBuffer##var.GetBuffer();

/* Macro to declare and efficiently manage a temporary buffer, using alloca if possible.
 * Use this macro only in "leaf functions", functions that don't call more non OS functions,
 * because you could run out of stack space 
 * Var is the variable name
 * Type is its type
 * sizestack (constant) is the maximum stack size that can be reserved on the stack 
 * (40-100 is a good number)
 * Sizealloca is the size that can be alloca'ed from the stack. 
 * size is the actual (known at runtime) size of the buffer
 * Example: 
 * TEMPALLOCABUF(tempchars, char*, 50,1000, 2*len(initchars));
 *
 * NOTE: you must check the pointer to the buffer for NULL in case
 *       the allocator wasn't able to allocate the memory.
*/

#define TEMPALLOCABUF(var,type,sizestack,sizealloca,size) char __TempBuf##var[sizestack];\
size_t __Alloca##var =0;\
TempBufferEx __TempBuffer##var(sizestack,(sizealloca),(size),(char*)&__TempBuf##var,&__Alloca##var);\
type var = (type)(__Alloca##var ? _alloca(__Alloca##var) : __TempBuffer##var.GetBuffer());

}
#endif // cplusplus
#endif // TEMPMEM_H_INCLUDED
