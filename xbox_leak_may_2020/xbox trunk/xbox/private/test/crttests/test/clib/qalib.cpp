#if !defined(__QALIB_CPP__)
#define __QALIB_CPP__

#if !defined(QA_USE_QALIB_DLL)

#include "qa_base.cpp"
#if defined(QA_USE_THREADS)
#include "qa_thread.cpp"
#endif 

#endif 

#endif // __QALIB_CPP__

