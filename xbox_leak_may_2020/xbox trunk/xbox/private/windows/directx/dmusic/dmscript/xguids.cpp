#define INITGUID
#include <debug.h>
#include <guiddef.h>

// Stupic HACK to get urlmon.h to compile as part of the activscript headers.
typedef struct _SECURITY_ATTRIBUTES
    {
    DWORD nLength;
    DWORD lpSecurityDescriptor;
    BOOL bInheritHandle;
    }	SECURITY_ATTRIBUTES;
#include <activscp.h>
//#endif

DEFINE_GUID(GUID_NULL, 0L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
