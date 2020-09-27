/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: Inststat.cpp
*
* File Comments:
*
*
***********************************************************************/

// This program instruments a program so that the new program will print out
// for each instruction how many of that type were executed.
// The output of the instrumented app
//
// CALL  50
// B 90
// LSL 100
//
// would mean 50 calls were executed 90 Branch instructions and 100
// logical shift lefts.


#include "vulcanapi.h"
#pragma warning(disable: 4786) //dentifier was truncated to '255'
#include "map"
#include "vector"

std::map<const char*,int> opStrs;
std::vector<int> bbIndexes;
std::vector<int> insts;
int opStrsNum = 0;
int instCount = 0;

void SendInstInfo(VProtoList* cProtoList,VProg* vprog,VComp *pComp)
{
   char protoStr[2000];

   int blen = bbIndexes.size();
   int ilen = insts.size();
   sprintf(protoStr,"SendInstInfo(int,int,int[%d],int[%d])",blen,ilen);

   VProto* vproto = cProtoList->CreateProto(pComp,"inststatdll.dll",protoStr);

   int *bbs = new int [blen + 1];
   int *is  = new int [ilen + 1];
   std::copy(bbIndexes.begin(), bbIndexes.end(), bbs);
   std::copy(insts.begin(), insts.end(), is);

   vproto->AddCall(pComp,BEFORE,blen,ilen,bbs,is);
}

void InstInfo(VBBlock *pBlk,VProto* cProto,VComp *pComp,VProg* vprog)
{
   for (VInst *pInst = pBlk->FirstInst(); pInst; pInst = pInst->Next())
   {
      const char* opstr = pInst->OpcodeGroupStr();

      if (opStrs.find(opstr)==opStrs.end())
      {
         opStrs[opstr] = opStrsNum;
         cProto->AddCall(pComp,BEFORE,opstr);
         printf("%s\n",opstr);
         opStrsNum++;
      }

      insts.push_back(opStrs[opstr]);
      instCount++;
   }

   bbIndexes.push_back(instCount);
}

void InstStat(VProg* vprog)
{
   VProtoList *protolist = VProtoList::CreateProtoList();
   printf("IR Reading\n");

   for (VComp *pComp = vprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      VProto* bbProto = protolist->CreateProto(pComp,
                                               "inststatdll.dll",
                                               "SendNum(int)");

      VProto* cProto = protolist->CreateProto(pComp,
                                              "inststatdll.dll",
                                              "SendStr(char*)");

      for (VProc *pProc = pComp->FirstProc();pProc;pProc = pProc->Next())
      {
         const char* name = pProc->Name();
         for (VBBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next())
         {
            bbProto->AddCall(pBlk,BEFORE,pBlk->BlockId());
            InstInfo(pBlk,cProto,pComp,vprog);
         }
      }

      SendInstInfo(protolist,vprog,pComp);
      protolist->Commit();

      printf("IR Writing\n");
      pComp->Write();
   }
}


int __cdecl main(int argc, char** argv)
{
   if (argc != 2)
   {
      printf("Usage: Inststat [Image-name]\n");
      return 1;
   }

   try
   {
      VProg *vprog = VProg::Open(argv[1]);
      InstStat(vprog);  

      // To save time, we don't destroy the vprog object
      // vprog->Destroy();
   }

   catch (VErr& verr) 
   {
      printf("Error: %s\n",verr.GetWhat());
      return 1;
   }

   catch (...)
   {
      printf("Error: unknown\n");
      return 1; 
   }

   return 0;
}
