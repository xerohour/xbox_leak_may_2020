#if !(defined (__OD4__))
#if defined (__cplusplus)
extern "C" {
#if !(defined (INLINE))
#define INLINE inline
#endif
#else
#if !(defined (INLINE))
#define INLINE __inline
#endif
#endif

#define __OD4__

#if !(defined (DECLARE_HANDLE))
#define DECLARE_HANDLE(name) typedef void* name
#define DECLARE_HANDLE32(name) DECLARE_HANDLE(name)
#endif
// INCLUDE should be set to load first from Osdebug4 inc then langapi

#include <od.h>

typedef LPADDR PADDR;
typedef HDEP HMEM;

#undef CEXM_MDL_native

typedef struct _CBP {
    DWORD wMessage;
    HPID hpid;
    HTID htid;
    LPARAM dwParam;
    LPARAM lParam;
} CBP;            // CallBack Parameters.  Not used by OSDebug itself,
               // but possibly handy for the debugger.
typedef CBP FAR *LPCBP;


#define hmemNull 0
#define hpidNull 0
#define htidNull 0
#define htlNull  0
#define hemNull  0

#define wNull 0
#define lNull 0L

#if defined (__cplusplus)
}
#endif

#endif  // __OD4__
