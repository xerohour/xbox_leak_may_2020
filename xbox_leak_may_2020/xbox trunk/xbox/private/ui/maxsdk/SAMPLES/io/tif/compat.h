/*      compat.h        1.3     89/11/07        */
/* AAA Updates: @(#)compat.h    1.2 11/28/89 */

#ifndef _COMPAT_
#define _COMPAT_

/*
 * This file contains a hodgepodge of definitions and
 * declarations that are needed to provide compatibility
 * between the native system and the base UNIX implementation
 * that the library assumes (~4BSD).  In particular, you
 * can override the standard i/o interface (read/write/lseek)
 * by redefining the ReadOK/WriteOK/SeekOK macros to your
 * liking.
 *
 * NB: This file is a mess.
 */
#ifdef __STDC__
#include <stdio.h>
#endif
#ifndef HIGHC
#ifndef sun
#include <sys/types.h>
#endif
#endif
#include <fcntl.h>
#include <fcntl.h>
#ifdef THINK_C
#include <stdlib.h>
#endif
#ifdef HIGHC
/*#include <system.cf>*/
#endif

#ifdef SYSV
//#include <unistd.h>

#define L_SET   SEEK_SET
#define L_INCR  SEEK_CUR
#define L_XTND  SEEK_END

#define bzero(dst,len)          memset(dst, 0, len)
#define bcopy(src,dst,len)      memcpy(dst, src, len)
#define bcmp(src, dst, len)     memcmp(dst, src, len)
#endif

#ifdef MICROSOFT
#define L_SET   SEEK_SET
#define L_INCR  SEEK_CUR
#define L_XTND  SEEK_END

#define bzero(dst,len)          memset(dst, 0, len)
#define bcopy(src,dst,len)      memcpy(dst, src, len)
#define bcmp(src, dst, len)     memcmp(dst, src, len)
#endif

#ifdef BSDTYPES
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
#endif

/*
 * Return an open file descriptor or -1.
 */
#if defined(applec) || defined(THINK_C)
#define TIFFOpenFile(name, mode, prot)  open(name, mode)
#else
#ifdef HIGHC
#define TIFFOpenFile(name, mode, prot)  c_open(name, mode)
#define TIFFCreateFile(name, mode, prot)  c_create(name, mode)
#else
#define TIFFOpenFile(name, mode, prot)  open(name, mode, prot)
#endif /* HIGHC */
#endif

/*
 * Return the size in bytes of the file
 * associated with the supplied file descriptor.
 */
extern  unsigned long TIFFGetFileSize();

#ifndef L_SET
#define L_SET   0
#define L_INCR  1
#define L_XTND  2
#endif

#ifdef notdef
#define lseek unix_lseek        /* Mac's Standard 'lseek' won't extend file */
#endif
extern  long lseek();

#ifndef ReadOK
#define ReadOK(file, buf, size)  (fread((char *)buf, size, 1, file) == 1)
#endif
#ifndef SeekOK
#define SeekOK(file, off) (fseek(file, (long)off+rd_offset, L_SET)==0)
#endif
#ifndef WriteOK
#define WriteOK(file, buf, size)  (fwrite((char *)buf, size, 1, file) == 1)
#endif

#ifndef O_BINARY
#define O_BINARY        0               /* MS/DOS compatibility */
#endif

#if defined(__MACH__) || defined(THINK_C)
extern  void *malloc(size_t size);
extern  void *realloc(void *ptr, size_t size);
#else
#ifdef OS2
#include <malloc.h>
#endif
#endif

/*
 * dblparam_t is the type that a double precision
 * floating point value will have on the parameter
 * stack (when coerced by the compiler).
 */
#ifdef applec
typedef extended dblparam_t;
#else
typedef double dblparam_t;
#endif

/*
 * define support for non-standard compression types here
 */
/*
 THUNDER_SUPPORT       add support for ThunderScan 4-bit RLE algorithm
 PICIO_SUPPORT         add support for Pixar picio RLE algorithm
 NEXT_SUPPORT          add support for NeXT 2-bit RLE algorithm
 SGI_SUPPORT           add support for SGI RLE algorithm
 FAX_SUPPORT           add support for the Fax 3 & 4 RLE algorithm
*/
#ifndef MSDOS

#define FAX_SUPPORT
#define THUNDER_SUPPORT
#define NEXT_SUPPORT
#define PICIO_SUPPORT
#define SGI_SUPPORT
#endif /* MSDOS */

#endif /* _COMPAT_ */
