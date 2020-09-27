// -*- Mode:C++ -*-
//
//      Copyright (c) 1996, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/util.h 2     5/07/96 12:56p Erikruf $
// $NoKeywords: $
//

// random utils that don't seem to fit anywhere else

#ifndef INC_UTIL_H 
#define INC_UTIL_H


class Util
{
  public:
    ASTAPI static int EscapeString(char *pBytes, int cBytes, char *buf, int bufSize);
    ASTAPI static int EscapeWideString(short *pWords, int cWords, char *buf, int bufSize);
};

class PathUtil
{
  public:

    // is this an absolute path?
    ASTAPI static bool
    FAbsolute(const char *path);

    // Utilities for path surgery:

    // Common features of all Expand* routines:
    //   If WDIR is NULL,  print PATH to OUTS
    //   If WDIR is given, print expansion of PATH relative to WDIR
    //     In both cases, resolve . and .. components appropriately.
    //   Return TRUE iff successful, otherwise return FALSE and print nothing
    //   Failure occurs only if either
    //     (1) path is NULL 
    //     (2) PATH is relative, WDIR is given, *and*
    //         WDIR and PATH both have explicit, distinct drives
    //         (i.e., somebody grabbed the *wrong* current directory).

    // vanilla Expand
    ASTAPI static bool
    Expand(OutputStream *outs, 
           const char *wdir, 
           const char *path);

    // Expand, and if the filename in PATH has
    // no extension fill in a default extension EXT
    ASTAPI static bool
    ExpandDefaultExt(
        OutputStream *outs, 
        const char *wdir, 
        const char *path,
        const char *ext);

    // Expand, replace (fill in) extension with EXT
    ASTAPI static bool
    ExpandForceExt(
        OutputStream *outs, 
        const char *wdir, 
        const char *path,
        const char *ext);

    // Expand, omitting any extension
    ASTAPI static bool
    ExpandNoExt(
        OutputStream *outs, 
        const char *wdir, 
        const char *path);

    // Both of the following emit any necessary trailing path separator
    // and thus may be followed immediately with
    //   outs->Puts(filename)

    // Expand and omit final component (if it is not . or ..)
    ASTAPI static bool
    ExpandDirOnly(
        OutputStream *outs, 
        const char *wdir, 
        const char *path);

    // Expand assuming final component is a directory.
    ASTAPI static bool
    ExpandAsDir(
        OutputStream *outs, 
        const char *wdir, 
        const char *path);

    // Do  PATH1 relative to working directory WDIR1
    // and PATH2 relative to working directory WDIR2
    // refer to the same file?
    //
    // WDIR1 and WDIR2 are expected to have drive letters or shares.
    // Returns
    //    0 if definitely yes
    //    1 if definitely no
    //   -1 if weirdness is encountered
    ASTAPI static int
    Compare(const char *wdir1,
            const char *path1,
            const char *wdir2,
            const char *path2);

    enum {
        pathEQ  = 0,            // must be same file
        pathNEQ = 1,            // cannot be the same file
        pathINC = -1,           // punt
    };

};

#endif // INC_UTIL_H
