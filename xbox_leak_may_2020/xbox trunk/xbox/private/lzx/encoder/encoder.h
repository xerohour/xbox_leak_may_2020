/*
 * encoder.h
 *
 * Main header file; includes everything else
 */

#ifndef ENCODER_H
#define ENCODER_H


#ifdef USE_ASSEMBLY
#   define ASM_BSEARCH_FINDMATCH
//#   define ASM_QUICK_INSERT_BSEARCH_FINDMATCH
#endif

#ifndef UNALIGNED
#ifndef NEEDS_ALIGNMENT
#define UNALIGNED
#else
#define UNALIGNED __unaligned
#endif
#endif

#include <stdlib.h>
#include <string.h>

#if 0 // #ifdef _X86_
#include <crtdbg.h>
#else
#define _ASSERTE(x) ;
#define _RPT2(a,b,c,d) ;
#define _RPT3(a,b,c,d,e) ;
#define _CrtSetReportMode(a,b) ;
#define _CrtSetReportFile(a,b) ;
#endif

#include "../common/typedefs.h"
#include "../common/compdefs.h"
#include "encdefs.h"
#include "encvars.h"
#include "encmacro.h"
#include "encapi.h"
#include "encproto.h"


#ifdef __BOUNDSCHECKER__

/* LZX uses mangled pointers throughout as an optimization.             */
/* Various buffers which are related to the sliding window get their    */
/* base addresses adjusted downward as the offsets increase, to avoid   */
/* wrap-around logic throughout.  Of course, BoundsChecker doesn't      */
/* understand this, and even though each dereference hits the intended  */
/* buffer, BoundsChecker can't determine what the buffer is or its      */
/* size.  Defining STRICT_POINTERS prevents the code from adjusting     */
/* buffer base addresses, so BoundsChecker will be able to follow it.   */

#ifndef STRICT_POINTERS
#define STRICT_POINTERS
#endif

/* The byte value BoundsChecker uses to signify "uninitialized" memory. */

#define BC_FILL_BYTE (0xBF)

#endif  __BOUNDSCHECKER__


#ifdef STRICT_POINTERS
/* SLIDE = how far the window has moved so far */
#define SLIDE (((ulong) (context->enc_RealMemWindow)) - ((ulong) (context->enc_MemWindow)))
#endif

#endif  /* ENCODER_H */
