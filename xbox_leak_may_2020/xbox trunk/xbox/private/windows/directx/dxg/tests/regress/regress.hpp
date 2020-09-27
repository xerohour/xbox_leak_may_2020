/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       regress.hpp
 *  Content:    Common include file
 *
 ***************************************************************************/

extern "C"
{
    extern BOOL D3D__Parser;
    extern DWORD D3D__AllocsContiguous;
    extern DWORD D3D__AllocsNoncontiguous;
}

#define CheckHR(_x)                                             \
{                                                               \
    HRESULT _hr = (_x);                                         \
    if (FAILED(_hr))                                            \
    {                                                           \
        OutputDebugStringA("CheckHR failure");                  \
        _asm { int 3 }                                          \
    }                                                           \
}
#define WARNING(msg)        \
    {                       \
        DbgPrint msg;     \
        DbgPrint("\n");   \
    }

#undef RIP

#define RIP(msg)            \
    {                       \
        DbgPrint msg;     \
        DbgPrint("\n");   \
        _asm { int 3 };     \
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

//=========================================================================
// Class to help time things
//=========================================================================
class CTimer
{
public:
    // constructor
    CTimer()
    {
        QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
        ticksPerSecond /= 1000;
    }

    ~CTimer() {};

    void Start()
        { frunning = true; QueryPerformanceCounter((LARGE_INTEGER *)&ticksstart); }

    void Stop()
        { frunning = false; QueryPerformanceCounter((LARGE_INTEGER *)&ticksend); }

    float getTime()
        {
            if(frunning)
                QueryPerformanceCounter((LARGE_INTEGER *)&ticksend);
            return ((ticksend - ticksstart) * 1.0f / ticksPerSecond);
        }

    bool frunning;
    _int64 ticksstart;
    _int64 ticksend;
    _int64 ticksPerSecond;
};
    
// Handy macros:

#define ARRAYSIZE(_a) (sizeof(_a) / sizeof((_a)[0]))

// Tests:

void TestLeaks();

void TestPushBuffers(D3DDevice* pDev);
void TestTiles(D3DDevice* pDev);
