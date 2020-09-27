/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: Inststatdll.cpp
*
* File Comments:
*
*
***********************************************************************/

#pragma warning(disable: 4786) //dentifier was truncated to '255'

#include <map>
#include <stdio.h>
#include <vector>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

std::vector<char*> opStrs;
std::map<int,int> bbCount;

int NumBBs;
int NumInsts;
int* BBindexes;
int* Insts;

extern "C" __declspec(dllexport) void _cdecl SendNum(int num)
{
   if (bbCount.find(num) == bbCount.end())
   {
      bbCount[num]=0;
   }

   bbCount[num]++;
}

extern "C" __declspec(dllexport)
void _cdecl SendProcStr(char* str)
{
}

extern "C" __declspec(dllexport)
void _cdecl SendStr(char* str)
{
    opStrs.push_back(str);
}

extern "C" __declspec(dllexport)
void _cdecl SendInstInfo(int numbb, int numi, int *bbs, int* is)
{
   NumBBs=numbb;
   NumInsts=numi;
   BBindexes = bbs;
   Insts = is;
}

std::vector<int> InstCount(std::map<int,int> counts)
{
   std::vector<int> ret(opStrs.size(),0);
   int index=0;

   for (int bbi=0;bbi<NumBBs;bbi++)
   {
      int addAmount = 0;
      if (counts.find(bbi) != counts.end())
      {
         addAmount = counts[bbi];
      }

      int bbIndexEnd = BBindexes[bbi];
      for ( ; ((index < bbIndexEnd) && (index < NumInsts)); index++)
      {
         ret[Insts[index]] += addAmount;
      }
   }
   return ret;
}

void PrintResults()
{
   std::vector<int> InstCounts = InstCount(bbCount);

#if 0
   std::map<int,int> StatCount;
   for (int i=0;i<NumBBs;i++)
   {
      StatCount[i]=1;
   }

   std::vector<int> StatCounts = InstCount(StatCount);
#endif

   for (int i=0;i<opStrs.size();i++)
   {
      printf("%s\t%d\n",opStrs[i],InstCounts[i]);
   }
}

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
   switch ( ul_reason_for_call )
   {
      case DLL_PROCESS_DETACH:
         PrintResults();
         break;
   }

   return TRUE;
}
