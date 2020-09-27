/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       debug.hpp
 *  Content:    header file for debug hooks and stuff
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

// For consistency, we never use 'new' or 'delete'.  Use 'MemAlloc' and
// 'MemFree' instead.  (Yes, it's a pain, but allocating memory SHOULD BE
// A PAIN on Xbox.  Plus we get explicit control over zero-initialization.)

#define new dont_use_new
#define delete dont_use_delete

#if DBG

    // DBG_CHECK is to be used to for any parameter validation checks, 
    // in the form:
    //
    //      if (DBG_CHECK(dwFlags & INVALID_FLAGS))
    //          return DDERR_UNSUPPORTED;
    //
    // On free, retail builds, the macro gets converted to a constant '0'
    // and the compiler will remove all the associated code.
       
    #define DBG_CHECK(exp) (exp)
    
#else

    #define DBG_CHECK(exp) FALSE
    
#endif


#if DBG
    
    VOID DXGRIP(PCHAR Format, ...);
    VOID WARNING(PCHAR Format, ...);

    #undef ASSERT
    #define ASSERT(cond)   \
        {                  \
            if (! (cond))  \
            {              \
                DXGRIP("Assertion failure: %s", #cond); \
            }              \
        }

    #undef ASSERTMSG
    VOID ASSERTMSG(BOOL cond, PCHAR Format, ...);

    #define DPF(msg) DXGRIP(msg)
    #define DPF_ERR(msg) DXGRIP(msg)
    #define D3D_ERR(msg) DXGRIP(msg)
    #define DXGASSERT(cond) ASSERT(cond)
    #define DDASSERT(cond) ASSERT(cond)
    #define UNIMPLEMENTED() DXGRIP("Function not yet implemented")

#else

    #define DXGRIP 1 ? (void)0 : (void)
    #define WARNING 1 ? (void)0 : (void)
    
    #undef ASSERT
    #define ASSERT(cond) {}
    #undef ASSERTMSG
    #define ASSERTMSG 1 ? (void)0 : (void)

    #define DPF(msg) {}
    #define DPF_ERR(msg) {}
    #define D3D_ERR(msg) {}
    #define DXGASSERT(cond) {}
    #define DDASSERT(cond) {}
    #define UNIMPLEMENTED() {}

#endif

// Debug code to verify the 'this' pointers passed into our APIs.

#if DBG
    
    inline void CHECK(D3DResource *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        const DWORD Alignments[7] = 
        {
            D3DVERTEXBUFFER_ALIGNMENT,
            D3DINDEXBUFFER_ALIGNMENT,
            D3DPUSHBUFFER_ALIGNMENT,
            D3DPALETTE_ALIGNMENT,
            D3DTEXTURE_ALIGNMENT,
            1,
            D3DFIXUP_ALIGNMENT,
        };

        DWORD Alignment = Alignments[(p->Common & D3DCOMMON_TYPE_MASK) >> D3DCOMMON_TYPE_SHIFT];

        if (p->Data % Alignment)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DBaseTexture *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null this pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_TEXTURE)
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }

        if (p->Data % D3DTEXTURE_ALIGNMENT)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DTexture *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_TEXTURE
            || (p->Format & D3DFORMAT_DIMENSION_MASK) != (2 << D3DFORMAT_DIMENSION_SHIFT)
            || (p->Format & D3DFORMAT_CUBEMAP))
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }

        if (p->Data % D3DTEXTURE_ALIGNMENT)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DVolumeTexture *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_TEXTURE
            || (p->Format & D3DFORMAT_DIMENSION_MASK) != (3 << D3DFORMAT_DIMENSION_SHIFT))
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }

        if (p->Data % D3DTEXTURE_ALIGNMENT)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DCubeTexture *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_TEXTURE
            || !(p->Format & D3DFORMAT_CUBEMAP))
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }

        if (p->Data % D3DTEXTURE_ALIGNMENT)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DVertexBuffer *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_VERTEXBUFFER)
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }

        if (p->Data % D3DVERTEXBUFFER_ALIGNMENT)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DIndexBuffer *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_INDEXBUFFER)
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }

        if (p->Data % D3DINDEXBUFFER_ALIGNMENT)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DPushBuffer *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_PUSHBUFFER)
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }

        if (p->Data % D3DPUSHBUFFER_ALIGNMENT)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DFixup *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_FIXUP)
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }

        if (p->Data % D3DFIXUP_ALIGNMENT)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DPalette *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_PALETTE)
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }

        if (p->Data % D3DPALETTE_ALIGNMENT)
        {
            DXGRIP("%s - Bad data alignment.", func);
        }
    }

    inline void CHECK(D3DSurface *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_SURFACE
            || (p->Format & D3DFORMAT_DIMENSION_MASK) != (2 << D3DFORMAT_DIMENSION_SHIFT))
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }
    }

    inline void CHECK(D3DVolume *p, char *func)
    {
        if (p == NULL)
        {
            DXGRIP("%s - Null THIS pointer.", func);
        }

        if ((p->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_SURFACE
            || (p->Format & D3DFORMAT_DIMENSION_MASK) != (3 << D3DFORMAT_DIMENSION_SHIFT))
        {
            DXGRIP("%s - This points to the wrong type of object.", func);
        }
    }

#else

    #define CHECK(x, y) { }

#endif

// Debug code to check for blocking errors.  It will assert if we block
// for more than a second.

#if DBG

    extern bool g_DpcRIPFired;

    #define INITDEADLOCKCHECK()   DWORD dwDbgTimeCheck = NtGetTickCount();
    #define DODEADLOCKCHECK()     { if (NtGetTickCount() - dwDbgTimeCheck > D3D__DeadlockTimeOutVal) { RIP(("Possible deadlock, blocked for more than 2 seconds.")); }  if (g_DpcRIPFired) { RIP(("Graphics processor error.")); g_DpcRIPFired = false; } }

#else

    #define INITDEADLOCKCHECK()   {}
    #define DODEADLOCKCHECK()     {}

#endif

} // end namespace
