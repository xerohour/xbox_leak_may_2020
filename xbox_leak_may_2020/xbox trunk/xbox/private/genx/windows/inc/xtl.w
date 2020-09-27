//
// Note that the top of this header file is contained
// in makefile.inc and one line from xboxverp.h
//

// begin_xtl

#ifndef _INC_WINDOWS
#define _INC_WINDOWS
#endif  /* _INC_WINDOWS */

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _X86_
#define _X86_
#endif  /* _X86_ */

#if     ( _MSC_VER >= 800 )
#pragma warning(disable:4100)
#pragma warning(disable:4514)
#ifndef __WINDOWS_DONT_DISABLE_PRAGMA_PACK_WARNING__
#pragma warning(disable:4103)
#endif
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4001)
#pragma warning(disable:4201)
#pragma warning(disable:4214)
#endif

#include <excpt.h>
#include <stdarg.h>

#include <windef.h>
#include <winbase.h>
#include <xbox.h>

#ifndef NOD3D
#include <d3d8.h>
#ifdef _USE_XGMATH
#include <xgmath.h>
#endif
#include <d3dx8.h>
#endif  /* NOD3D */

#ifndef NODSOUND
#include <dsound.h>
#endif  /* NODSOUND */

#include <winsockx.h>

#if     ( _MSC_VER >= 800 )
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4001)
#pragma warning(default:4201)
#pragma warning(default:4214)
/* Leave 4100 disabled.  Unreferenced parameter for inline methods. */
/* Leave 4514 disabled.  It's an unneeded warning anyway. */
#endif
#endif

#endif /* _INC_XTL */
#endif /* _XTL_ */
// end_xtl
