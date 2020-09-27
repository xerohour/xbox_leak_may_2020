/*
 *
 * ocharint.h
 *
 * a/w versions of system APIs
 *
 * Copyright (C) 2000 Microsoft Corporation
 *
 */

#ifndef _OCHARINT_H
#define _OCHARINT_H

#ifndef _XBOX
#error XBOX-specific A/W substitutions
#endif

#define __crtCreateDirectory __crtCreateDirectoryW
#define __crtCreateFile __crtCreateFileW
#define __crtDeleteFile __crtDeleteFileW
#define __crtFindFirstFile __crtFindFirstFileW
#define __crtFindNextFile __crtFindNextFileW
#define __crtGetFileAttributes __crtGetFileAttributesW
#define __crtMoveFile __crtMoveFileW
#define __crtRemoveDirectory __crtRemoveDirectoryW
#define __crtSetFileAttributes __crtSetFileAttributesW

#define WPRFLAG
#define UNICODE
#define _UNICODE
#undef _MBCS

typedef char OCHAR;
#define ocstotcs mbstowcs
#define tcstoocs wcstombs

#include <tchar.h>

#define OSZ_LEN(osz) (sizeof(osz) / sizeof(OCHAR))
#define TSZ_LEN(tsz) (sizeof(tsz) / sizeof(TCHAR))

#endif // _OCHARINT_H
