#define TRUE    1
#define FALSE   0

#ifndef WIN32
  #ifndef FAR
    #define FAR                 _far
  #endif
typedef unsigned long ULONG;
typedef unsigned long INT;

#else
  #ifndef FAR
    #define FAR
  #endif
#endif

#pragma warning(disable:4102)
