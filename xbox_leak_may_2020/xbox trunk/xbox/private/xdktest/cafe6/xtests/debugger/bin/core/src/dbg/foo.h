#define TRUE    1
#define FALSE   0

#define cBASIC     0
#define cCHAIN     1
#define cCXX       2
#define cRECURSIVE 3

#ifndef WIN32
  #ifndef FAR
    #define FAR                 _far
  #endif
typedef unsigned long ULONG;
#if !defined( _WINDEF_ )
typedef unsigned long INT;
#endif

#else
  #ifndef FAR
    #define FAR
  #endif
#endif

#pragma warning(disable:4102)
