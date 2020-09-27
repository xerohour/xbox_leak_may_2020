// -*- Mode:C++ -*-
//
//      Copyright (c) 1997 Microsoft Corporation.  All rights reserved.
//
//    This work is a trade secret of Microsoft Corporation.
//    Unauthorized disclosure is unlawful.
//
// $Header: $
// $NoKeywords: $
//
//
// File:    lib/version.h
// Creator: 

// Facilities for version numbering

#ifndef INC_VERSION_H 
#define INC_VERSION_H

#include <time.h>

struct Version
{
    int major;                  // major version number
    int minor;                  // minor version number
    int build;                  // build number
    time_t timestamp;           // time of build
    char *kind;                 // what kind of build (e.g., Debug, Release)

    // print version-id string to OUTS
    ASTAPI void Print(OutputStream *outs);

    // compare version numbers, negative means "don't care"
    // Return true iff version numbers match
    ASTAPI bool FEqual(int omajor, int ominor = -1, int obuild = -1)
      {
          return major == omajor
              && (ominor < 0 || ominor == minor)
              && (obuild < 0 || obuild == build);
      }
    
    // compare version numbers, negative means "don't care"
    // Return true iff this represents a newer version
    // than (OMAJOR, OMINOR, OBUILD)
    ASTAPI bool FNewerThan(int omajor, int ominor = -1, int obuild = -1)
      {
          return (major > omajor 
                  || (major == omajor &&
                      (minor > ominor
                       || (minor == ominor && build > obuild))));
      }

    Version(int major, int minor, int build,
            time_t timestamp, 
            char *ignore,       // was .who
            char *kind
      ):
        major(major),
        minor(minor),
        build(build),
        timestamp(timestamp),
        kind(kind)
      { }
};

#endif // INC_VERSION_H
