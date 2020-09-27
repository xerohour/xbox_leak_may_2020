extern "C" {
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntos.h>
}
#include <dsoundp.h>
#include "draw.h"
#include "devices.h"

#define VERIFY_SUCCESS(xprsn) \
{\
    HRESULT hrTemp = (xprsn);\
    if(!SUCCEEDED(hrTemp))\
    {\
        DebugPrint("Call failed: %s(%d): 0x%0.8x\n", __FILE__, __LINE__, hrTemp);\
    }\
}