/*
    FILE:   debug.h
    DATE:   4/8/99

    This file has some things in it for debug.  Since the debug
    facilities are OS dependent, the macros invoke functions that
    can be written in the OS specific libraries.
*/

extern  char    szDebug[512];
extern  ULONG   dwDebugLevel;

// A little bit of debugging support.
#ifdef DEBUG
#define DL_0    1
#define DL_1    2
#define DL_2    3
#define DL_3    4
#define DL_4    5
#define DL_5    6

#ifdef LINUX
#include "os.h" // Xserver header file for definition of ErrorF
#define PrintString0(y)     ErrorF (y);
#define PrintString1(y,z)   ErrorF (y,z);
#endif

#define DBG_PRINT0(x,y)     {                                   \
                                if  ((x) <= dwDebugLevel)       \
                                    PrintString0(y);            \
                            }

#define DBG_PRINT1(x,y,z)   {                                   \
                                if  ((x) <= dwDebugLevel)       \
                                    PrintString1(y,z);          \
                            }
#else
#define DBG_PRINT0(x,y)
#define DBG_PRINT1(x,y,z)
#endif

