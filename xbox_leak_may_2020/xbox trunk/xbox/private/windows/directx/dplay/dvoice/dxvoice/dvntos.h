/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvntos.h
 *  Content:	kernel defs
 *		
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 09/01/2000  georgioc 
 *
 ***************************************************************************/

#ifndef __DVNTOS_H
#define __DVNTOS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>
#include <ntos.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#define  NODSOUND
#include <xtl.h>
#include <xdbg.h>
#include "initguid.h"
#include <dsoundp.h>

#include "dplay8.h"
//#include "..\..\core\dncorei.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define SET_FLAG(Flags, Bit)    ((Flags) |= (Bit))
#define CLEAR_FLAG(Flags, Bit)  ((Flags) &= ~(Bit))
#define TEST_FLAG(Flags, Bit)   ((Flags) & (Bit))


class CDVAutoLock
{
private:
    CRITICAL_SECTION        *m_pCS;

public:
    CDVAutoLock(CRITICAL_SECTION *pCS);
    ~CDVAutoLock(void);
};

__inline CDVAutoLock::CDVAutoLock(CRITICAL_SECTION *pCS)
{
    m_pCS = pCS;
    EnterCriticalSection(pCS);
}

__inline CDVAutoLock::~CDVAutoLock(void)
{
    LeaveCriticalSection(m_pCS);
}

#define DV_AUTO_LOCK(pCS) \
    CDVAutoLock __AutoLock(pCS)


#define ENTER_CS(pCS) {\
    EnterCriticalSection(pCS);\
}

#define LEAVE_CS(pCS) {\
    LeaveCriticalSection(pCS);\
}

#define RIRQL(irql) { \
    irql = KeRaiseIrqlToDpcLevel();\
    }

#define LIRQL(irql) { \
    KeLowerIrql(irql); \
    }

#define DV_DISPATCH_CODE {ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);}

__inline void *__cdecl DV_POOL_ALLOC(size_t size)
{
    return ExAllocatePoolWithTag(size, '__vd');
}

__inline void __cdecl DV_POOL_FREE(void *pv)
{
    ExFreePool(pv);
}

#endif
