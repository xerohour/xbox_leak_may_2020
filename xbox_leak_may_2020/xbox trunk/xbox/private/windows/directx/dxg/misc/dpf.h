/*==========================================================================
 *
 *  Copyright (C) 1995-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dpf.h
 *  Content:    header file for debug printf
 *
 ***************************************************************************/

// These macros are used for debugging. They expand to
// whitespace on a free build.
//
// DebugLevel
//  Global variable which holds the current debug level. You can use it to
//  control the quantity of debug messages emitted.
//
//  Okay, so I haven't actually bothered to implement this yet.  But it
//  could happen.
//
// WARNING(msg)
//  Display a message if the current debug level is <= DBG_WARNING.
//  The message format is: WRN filename (linenumber): message
//
// ASSERT(cond)
//  Verify that a condition is true. If not, force a breakpoint.
//
// ASSERTMSG(cond, msg)
//  Verify that a condition is true. If not, display a message and
//  force a breakpoint.
//
// RIP(msg)
//  Display a message and force a breakpoint.
//
// Usage:
//
//  These macros require extra parentheses for the msg argument
//  for example:
//      WARNING(("App passed NULL pointer; ignoring it.\n"));
//      ASSERTMSG(x > 0, ("x is less than 0\n"));
//
//  Note that you need to put a trailing \n on the message.

#if defined(DBG)
    
    // DebugPrint is actually just a forward to the kernel's DebugPrint.
    // It is vastly superior to OutputDebugString because it takes
    // varargs...

    #if defined(__cplusplus)
        extern "C" ULONG DebugPrint(const char *Format, ...);
    #else
        ULONG DebugPrint(PCHAR Format, ...);
    #endif

    #define WARNING(msg)        \
        {                       \
            DebugPrint msg;     \
            DebugPrint("\n");   \
        }
    
    #define RIP(msg)            \
        {                       \
            DebugPrint msg;     \
            DebugPrint("\n");   \
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

    #define DPF(msg) RIP(msg)
    #define DPF_ERR(msg) RIP((msg))
    #define D3D_ERR(msg) RIP((msg))
    #define DXGASSERT(cond) ASSERT(cond)
    #define DDASSERT(cond) ASSERT(cond)
    #define UNIMPLEMENTED() RIP(("D3D: Function not yet implemented"))

#else

    #define WARNING(msg) {}
    #define RIP(msg) {}

    #undef ASSERT
    #define ASSERT(cond) {}

    #undef ASSERTMSG
    #define ASSERTMSG(cond, msg) {}

    #define DPF(msg) {}
    #define DPF_ERR(msg)
    #define D3D_ERR(msg)
    #define DXGASSERT(cond)
    #define DDASSERT(cond)
    #define UNIMPLEMENTED()

#endif
