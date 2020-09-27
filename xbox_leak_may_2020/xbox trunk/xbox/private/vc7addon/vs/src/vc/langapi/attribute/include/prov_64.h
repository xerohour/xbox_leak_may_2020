
#if _WIN64
typedef __int64             intptr_t;
typedef unsigned __int64    uintptr_t;
#elif _M_IX86 && defined(Wp64)
typedef __w64 int           intptr_t;
typedef __w64 unsigned int  uintptr_t;
#else
typedef int                 intptr_t;
typedef unsigned int        uintptr_t;
#endif
