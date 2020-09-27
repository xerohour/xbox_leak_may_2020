#if !defined(__QALIB_THREAD_H__)
#define __QALIB_THREAD_H__


#include "qa_macro.h"

#if defined(QA_USE_THREADS)
#include <process.h>
#include <xtl.h>
#include "qa_threads.h"
#include "qa_cs.h"
#endif 

// Convenance locking macros for the tests.
//
#if defined(QA_USE_THREADS)

#define QA_CS(x)        CCriticalSection        x
#define QA_AUTO_LOCK(x) CAutoLock<CCriticalSection> qaAutoLock(x)
#define QA_LOCK(x)      x.Lock()
#define QA_UNLOCK(x)    x.Unlock()

#else

#define QA_CS(x)
#define QA_AUTO_LOCK(x)
#define QA_LOCK(x)
#define QA_UNLOCK(x)

#endif 

namespace QaLib 
{

#if defined(QA_USE_THREADS)
extern CCriticalSection g_qalibCS1;             // log object.
extern CCriticalSection g_qalibCS2;             // global variable access

#define QA_AUTO_LOCK1   QA_AUTO_LOCK(g_qalibCS1)
#define QA_AUTO_LOCK2   QA_AUTO_LOCK(g_qalibCS2)
#define QA_LOCK1        g_qalibCS1.Lock()
#define QA_LOCK2        g_qalibCS2.Lock()
#define QA_UNLOCK1      g_qalibCS1.Unlock()
#define QA_UNLOCK2      g_qalibCS2.Unlock()
#else
#define QA_AUTO_LOCK1
#define QA_AUTO_LOCK2
#define QA_LOCK1
#define QA_LOCK2
#define QA_UNLOCK1
#define QA_UNLOCK2
#endif 

}       // QaLib

#endif // __QALIB_THREAD_H__
