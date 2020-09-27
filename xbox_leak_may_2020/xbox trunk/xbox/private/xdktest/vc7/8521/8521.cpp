#pragma optimize("", on)

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#define ktest3(x) (char)((x<0) ? 0 : ((x>0) ? 0:x))

void ktest1(float a, char r, char g, char b)
{
    volatile float t=a+r+g+b;
}

void ktest2()
{
    volatile float t=0;
    ktest1(0,ktest3(t),ktest3(t),0);
}

#ifndef _XBOX
void main()
#else
void __cdecl main()
#endif
{
    ktest2();
}

