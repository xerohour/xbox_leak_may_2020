#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _INC_WINDOWS
#define _INC_WINDOWS

#if defined(RC_INVOKED)

#include <winresrc.h>

#else  // RC_INVOKED

#define NOD3D
#define NODSOUND

#include <xtl.h>

#undef  NOD3D
#undef  NODSOUND

#include <wingdi.h>

#endif // RC_INVOKED

#endif  /* _INC_WINDOWS */


