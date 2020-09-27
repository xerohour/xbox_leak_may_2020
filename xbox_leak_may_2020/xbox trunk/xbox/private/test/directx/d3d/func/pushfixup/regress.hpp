/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       regress.hpp
 *  Content:    Common include file
 *
 ***************************************************************************/

extern IDirect3DDevice8* g_pDev;
extern IDirect3D8* g_pD3D;

extern "C" void DebugPrint(const char *,...);

extern "C"
{
    extern BOOL D3D__Parser;
    extern DWORD D3D__AllocsContiguous;
    extern DWORD D3D__AllocsNoncontiguous;
}

#define Check(_x)                            \
{											 \
	if(!(_x)) {								 \
		DebugPrint("pushbuffer test: " #_x); \
		goto cleanup;						 \
	}										 \
}	

#define CheckHR(_x)                                             \
{                                                               \
    HRESULT _hr = (_x);                                         \
    if (FAILED(_hr))                                            \
    {                                                           \
        DebugPrint("pushbuffer test: " #_x);                  \
        /*_asm { int 3 }*/                                          \
		goto cleanup;											\
    }                                                           \
}
#define WARNING(msg)        \
    {                       \
        DbgPrint msg;     \
        DbgPrint("\n");   \
    }

#undef ASSERT
#define ASSERT(cond)   \
    {                  \
        if (! (cond))  \
        {              \
            RIP(("Assertion failure: %s", #cond)); \
        }              \
    }

#undef ASSERTMSG
#define ASSERTMSG(cond, msg) \
    {                        \
        if (! (cond))        \
        {                    \
            RIP(msg);        \
        }                    \
    }
    
// Handy macros:

#define ARRAYSIZE(_a) (sizeof(_a) / sizeof((_a)[0]))
#define countof(_a) ARRAYSIZE(_a)
#define COUNTOF(_a) countof(_a)

#define SAFE_RELEASE(_x) do { if(_x) { (_x)->Release(); _x = NULL; } } while (0) 

// Tests:

void TestPushBuffers();
void TestLeaks();
