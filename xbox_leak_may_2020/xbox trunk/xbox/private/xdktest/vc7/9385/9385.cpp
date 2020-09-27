/*
********************************************************** 
*** VC7 with manual precompiled headers (/Yc & /Yu) and *** 
*** global optimizations (/Og) can initialize a chain   *** 
*** of dependent global constants in the wrong order.   *** 
*********************************************************** 
*/

//-------------------- main.cpp -------------------- 
// 
//  VC7 & "cinit" bug 
// 
//      VC7 with manual precompiled headers (/Yc & /Yu) 
//      and global optimizations (/Og) can initialize 
//      a chain of dependent global constants in the 
//      wrong order.  Counter-intuitively, if you USE 
//      the constants in the order of their definition, 
//      they are "cinit'd" backwards, but only with /Og. 
//
//  If building this using the auto-generated DSP, make sure
//  to change C/C++ optimization settings to use global optimizations
//  (if building the Debug configuration)

#include "stdafx.h" 
#define  VC7BUG 

float showbug() 
{ 
#ifdef   VC7BUG 
    float one = Pie1;   // Pie1 = 3.14  (cinit'd second) 
    float two = Pie2;   // Pie2 = 0     (cinit'd first ) BUG 
#else 
    float two = Pie2;   // Pie2 = 6.28  (cinit'd second) 
    float one = Pie1;   // Pie1 = 3.14  (cinit'd first ) A-OK 
#endif 
    return one + two; 
}

#ifdef _XBOX
#include <xtl.h>
extern "C" int DbgPrint(const char *, ...);
#endif

#include <stdio.h>

void __cdecl main()
{
	char temp[20];
	sprintf(temp, "%e", showbug());

    DbgPrint("Expected result is 9.420000e+000\n");
    DbgPrint("Actual result is %s\n", temp);
}
