#define WINSOCK_API_LINKAGE

#ifdef BUILD_FOR_DEBUGGER
#define _KERNEL32_
#define _USER32_
#define _XAPI_
#include "ntos.h"
#include "dm.h"
#endif

#include "xnetp.h"

#include "pcb.h"
#include "xdbg.h"
#include "sockp.h"
