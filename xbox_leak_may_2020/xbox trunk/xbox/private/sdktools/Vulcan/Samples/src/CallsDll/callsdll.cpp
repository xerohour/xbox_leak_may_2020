/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: Callsdll.cpp
*
* File Comments:
*
*
***********************************************************************/
#include <stdio.h>

extern "C" __declspec(dllexport) void _cdecl SendStr(char* str)
{
   printf("%s",str);
}

extern "C" __declspec(dllexport) void _cdecl SendInt(int i)
{
   printf("%d\n",i);
}

extern "C" __declspec(dllexport) void _cdecl SendEff(int i)
{
   printf("Memory use at %d\n",i);
}

extern "C" __declspec(dllexport) void _cdecl SendStrInt(char*str, int i)
{
   printf("Str = %s, i = %d\n",str,i);
}


typedef int (_cdecl *FP)(int);

FP fp;

extern "C" __declspec(dllexport) void _cdecl GetPointer(FP p)
{
   fp=p;
}

extern "C" __declspec(dllexport)int _cdecl Handler(int size)
{
   printf("hello world\n");

   int f = (*fp) (size);
   printf("allocated at %d size of %d\n",f,size);
   return f;
}
