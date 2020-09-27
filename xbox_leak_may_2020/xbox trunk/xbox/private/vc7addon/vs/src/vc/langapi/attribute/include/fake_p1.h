#if !defined(__FAKE_P1_H__)
#define __FAKE_P1_H__

#if (VERSP_RELEASE || VERSP_TEST || VERSP_DEBUG)

#if !defined(CXXDELETE)
#define CXXDELETE(x) 
#endif

#if !defined(CXXVECDELETE)
#define CXXVECDELETE(x) 
#endif

#else // (VERSP_RELEASE || VERSP_TEST || VERSP_DEBUG)

#include <stdio.h>

#if defined(__ATLBASE_H__)

#define DASSERT(x) ATLASSERT(x)

#else // __ATLBASE_H__

#include <string.h>
#include <assert.h>

#if !defined(STDMETHODCALLTYPE)
#define STDMETHODCALLTYPE
#endif
#if !defined(S_OK)
#define S_OK 0
#endif
#if !defined(TRUE)
#define TRUE 0
#endif
#if !defined(FALSE)
#define FALSE 0
#endif

typedef int HRESULT;
typedef int BOOL;
typedef char CHAR;
typedef short *BSTR;

#if defined(_DEBUG)
#define DASSERT(x) assert(x)
#else
#define DASSERT(x)
#endif

#endif // __ATLBASE_H__

#if !defined(UNREACHED)
#define UNREACHED 0
#endif

#if !defined(CXXNEW)
#define CXXNEW(x, y) new x
#endif

#if !defined(CXXDELETE)
#define CXXDELETE(x) delete x
#endif

#if !defined(CXXVECDELETE)
#define CXXVECDELETE(x) if (x) delete[] x
#endif

#if !defined(ALLOC_ARB)
#define ALLOC_ARB(x, y, z) (x*) new char[z];
#endif

#if !defined(ALLOC_STRINGA)
#define ALLOC_STRINGA(x, y) (char*) new char[x];
#endif

#if !defined(SRC)
#define SRC(x) 
#endif

enum lifetime_e {
    M_FILELIFE,
    M_STMTLIFE
};

typedef bool bool_t;

#include <ctype.h>

#endif // (VERSP_RELEASE || VERSP_TEST || VERSP_DEBUG)

#endif // __FAKE_P1_H__
