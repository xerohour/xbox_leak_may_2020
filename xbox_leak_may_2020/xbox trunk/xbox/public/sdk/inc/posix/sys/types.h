/*++

Copyright (c) 1991-1996 Microsoft Corporation

Module Name:

  types.h

Abstract:

   This module contains the primitive system data types described
   in section 2.6 of IEEE P1003.1/Draft 13 as well as type definitions
   for sockets and streams

--*/

#ifndef _SYS_TYPES_
#define _SYS_TYPES_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Conditional macro definition for function calling type and variable type
 * qualifiers.
 *
 * Convention: _CRTAPI1 is for functions with a fixed number of arguments
 *             _CRTAPI2 is for functions with a variable number of arguments
 */
#if   ( (_MSC_VER >= 800) && (_M_IX86 >= 300) )

/*
 * Definitions for MS C8-32 (386/486) compiler
 */
#ifndef _CRTAPI1
#define _CRTAPI1 __cdecl
#endif

#ifndef _CRTAPI2
#define _CRTAPI2 __cdecl
#endif

#elif ( _MSC_VER == 600 )

/*
 * Definitions for old MS C6-386 compiler
 */
#ifndef _CRTAPI1
#define _CRTAPI1 _cdecl
#endif

#ifndef _CRTAPI2
#define _CRTAPI2 _cdecl
#endif

#define _M_IX86  300

#else

/*
 * Other compilers (e.g., MIPS)
 */
#ifndef _CRTAPI1
#define _CRTAPI1
#endif

#ifndef _CRTAPI2
#define _CRTAPI2
#endif

#endif


/*
 *   POSIX data types
 */

typedef unsigned long gid_t;
typedef unsigned long mode_t;
typedef unsigned long nlink_t;
typedef          long pid_t;
typedef unsigned long uid_t;

#ifndef _OFF_T_DEFINED
typedef 	 long off_t;
#define _OFF_T_DEFINED
#endif

#ifndef _DEV_T_DEFINED
typedef unsigned long dev_t;
#define _DEV_T_DEFINED
#endif

#ifndef _INO_T_DEFINED
typedef unsigned long ino_t;
#define _INO_T_DEFINED
#endif

#ifndef _TIME_T_DEFINED
typedef long time_t;
#define _TIME_T_DEFINED
#endif

#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64 size_t;
#else
typedef unsigned int     size_t;
#endif
#define _SIZE_T_DEFINED
#endif

#ifndef _SSIZE_T_DEFINED
typedef signed int ssize_t;
#define _SSIZE_T_DEFINED
#endif

#ifndef _POSIX_SOURCE

/*
 * Additional types for sockets and streams
 */

typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned short	ushort;
typedef unsigned int	u_int;
typedef unsigned long	u_long;

typedef unsigned int    uint;
typedef unsigned long   ulong;
typedef unsigned char   unchar;

typedef char            *caddr_t;
typedef int             key_t;          /* Imported!!! */

#endif	/* _POSIX_SOURCE */

#ifdef __cplusplus
}
#endif

#endif  /* _SYS_TYPES_ */
