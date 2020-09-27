#ifdef UNDER_CE
// find this in windows CE include files
#ifdef _SH4_
#include <windef.h>
#include <kfuncs.h>
#include <dbgapi.h>
#endif

#ifdef assert
#undef assert
#endif
#define assert ASSERT

#else	// so not UNDER_CE

#include <assert.h>

#endif

#ifdef DISABLE_ASSERTS
#undef assert
#define assert(a)
#endif  // DISABLE_ASSERTS
