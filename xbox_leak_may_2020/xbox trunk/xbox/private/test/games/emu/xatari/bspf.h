#ifndef BSPF_HXX
#define BSPF_HXX

/**
  This file defines various basic data types and preprocessor variables
  that need to be defined for different operating systems.

  @author Bradford W. Mott
  @version $Id: bspf.hxx,v 1.4 1998/08/29 15:30:07 bwmott Exp $
*/

// Types for 8-bit signed and unsigned integers
typedef signed char Int8;
typedef unsigned char uInt8;

// Types for 16-bit signed and unsigned integers
typedef signed short Int16;
typedef unsigned short uInt16;

// Types for 32-bit signed and unsigned integers
typedef signed int Int32;
typedef unsigned int uInt32;

// The following code should provide access to the standard C++ objects and
// types: cout, cerr, string, ostream, istream, etc.
#ifdef BSPF_WIN32
//  #include <iostream.h>
//  #include <iomanip>
  #include <string.h>
  using namespace std;
#else
//  #include <iostream.h>
//  #include <iomanip.h>
  #include <string.h>
#endif

// Some compilers do not support the bool type yet :-(
/*#ifdef BSPF_BOOL
  #define bool int
  #define true 1
  #define false 0
#endif
*/

// Defines to help with path handling
#if defined BSPF_UNIX
  #define BSPF_PATH_SEPARATOR  '/'
#elif (defined(BSPF_DOS) || defined(BSPF_WIN32) || defined(BSPF_OS2))
  #define BSPF_PATH_SEPARATOR  '\\'
#elif defined BSPF_MACOS
  #define BSPF_PATH_SEPARATOR  ':'
#endif

#endif

