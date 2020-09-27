//
// Define the build revision number.  Default is zero.
//
//  Typical use is to use a -D define to override this one
//  so you do not need to update this file.
//
//  example:  cl ... -Drbld=1 ...
//
//
#if !defined(_build_version_h)
#define _build_version_h
#pragma once

#if !defined(rbld)
#define rbld 0
#endif

#endif
