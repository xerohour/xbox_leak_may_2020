/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: Calls.cpp
*
* File Comments:
*
*
***********************************************************************/
#include "vulcanapi.h"

#include <string.h>
#include <stdio.h>

char* ProgramName = 0;
char* CommandFile = 0;
char badstr[3000];

char* ModName(const char* origname, const char* app)
{
   char* start;
   char* end;
   char copy[900];

   strcpy (copy,origname);
   start = strtok(copy,".");
   end = strtok (0,".");
   sprintf(badstr, "%s.%s.%s", start, app, end);
   return badstr;
}

void OrderedCalls()
{
   // Results should be comp before,...,Inst before printed out

   VProg *vprog = VProg::Open(ProgramName);
   VProtoList *vprotolist = VProtoList::CreateProtoList();

   for (VComp* pComp = vprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      pComp->SetCommandFile(CommandFile);
      VProto* vprotoStrInt = vprotolist->CreateProto(pComp,
                                                     "callsdll.dll",
                                                     "SendStrInt(char*,int)");

      VBlock *pBlock = pComp->EntryBlock();
      vprotoStrInt->AddCall(pBlock, BEFORE, "proc Before", 1234);

      for (VBBlock *pBlk = pBlock->ParentProc()->FirstBlock();
           pBlk;
           pBlk = pBlk->Next())
      {
         for (VInst *pInst = pBlk->FirstInst();pInst;pInst = pInst->Next())
         {
            vprotoStrInt->AddCall(pInst, BEFORE, "inst before", 22);
         }

         vprotoStrInt->AddCall(pBlk, BEFORE, "block Before", 134);
      }

      vprotoStrInt->AddCall(pComp, BEFORE, "comp before", 1212);
      vprotolist->Commit();
      pComp->Write(ModName(pComp->InputName(),"ORDER"));
   }

   vprog->Destroy();
}



void DoubleCalls()
{
   // Testing double parameters

   VProg *vprog = VProg::Open(ProgramName);
   VProtoList *vprotolist = VProtoList::CreateProtoList();

   for (VComp* pComp = vprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      pComp->SetCommandFile(CommandFile);
      VProto* vprotoStrInt = vprotolist->CreateProto(pComp,
                                                     "callsdll.dll",
                                                     "SendStrInt(char*,int)");

      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         vprotoStrInt->AddCall(pProc, BEFORE, pProc->Name(), 1234);
      }

      vprotolist->Commit();
      pComp->Write(ModName(pComp->InputName(), "DOUBLE"));
   }
   vprog->Destroy();
}


void RegCalls()
{
   // Testing Register Arguments

   VProg *vprog = VProg::Open(ProgramName);
   VProtoList *vprotolist = VProtoList::CreateProtoList();

   for (VComp* pComp = vprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      pComp->SetCommandFile(CommandFile);
      VProto* bbProto = vprotolist->CreateProto(pComp,
                                                "callsdll.dll",
                                                "SendInt(reg)");

      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         bbProto->AddCall(pProc, BEFORE, X86Register::ESP);
      }

      vprotolist->Commit();
      pComp->Write(ModName(pComp->InputName(), "REG"));
   }
   vprog->Destroy();
}


void FlagsCalls()
{
   // Testing Condcode

   VProg *vprog = VProg::Open(ProgramName);
   VProtoList *vprotolist = VProtoList::CreateProtoList();

   for (VComp* pComp = vprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      pComp->SetCommandFile(CommandFile);
      VProto* bbProto = vprotolist->CreateProto(pComp,
                                                "callsdll.dll",
                                                "SendInt(condcode)");

      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         const char* name = pProc->Name();
         bbProto->AddCall(pProc, BEFORE, COp::ISETZ);
      }
      vprotolist->Commit();
      pComp->Write(ModName(pComp->InputName(), "FLAG"));
   }
   vprog->Destroy();
}

void SwapCalls()
{
   // Testing Exchangeproc

   VProg *vprog = VProg::Open(ProgramName);
   VProtoList *vprotolist = VProtoList::CreateProtoList();

   for (VComp* pComp = vprog->FirstComp(); pComp; pComp=pComp->Next())
   {
      pComp->SetCommandFile(CommandFile);
      VProc *ep = pComp->CreateImportThunk("callsdll.dll", "Handler");

      VProto* vproto = vprotolist->CreateProto(pComp,
                                               "callsdll.dll",
                                               "GetPointer(bblockaddr)");

      VProto* vprotoEff = vprotolist->CreateProto(pComp,
                                                  "callsdll.dll",
                                                  "SendEff(eff)");

      for(VProc *pProc = pComp->FirstProc();pProc;pProc = pProc->Next())
      {
         const char* name = pProc->Name();

         if (strstr(name,"::operator new"))
         {
            printf("%s is being changed\n", name);
            VBBlock *temp = pComp->RedirectProc( pProc, ep);
            vproto->AddCall(pProc, BEFORE, temp);
         }
      }

      vprotolist->Commit();
      pComp->Write(ModName(pComp->InputName(), "SWAP"));
   }
   vprog->Destroy();
}

void EffCalls()
{
   // Testing effective Address

   VProg *vprog = VProg::Open(ProgramName);
   VProtoList *vprotolist = VProtoList::CreateProtoList();

   for (VComp* pComp = vprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      pComp->SetCommandFile(CommandFile);
      VProto* vprotoEff = vprotolist->CreateProto(pComp,
                                                  "callsdll.dll",
                                                  "SendEff(eff)");

      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         for (VBBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next())
         {
            for (VInst *vinst = pBlk->FirstInst(); vinst; vinst = vinst->Next())
            {
               if ((vinst->ReadsMemory()   ||
                    vinst->WritesMemory()) &&
                    !(vinst->StackMemory()))
               {
                  vprotoEff->AddCall(vinst, BEFORE, Eff::ANY);
               }
            }
         }
      }

      vprotolist->Commit();
      pComp->Write(ModName(pComp->InputName(), "EFF"));
   }

   vprog->Destroy();
}



void BlockCalls()
{
   // Testing: BEFORE and AFTER on BBlocks.
   // Testing: follower and target
   // Testing: int as parameter

   VProg *vprog = VProg::Open(ProgramName);
   VProtoList *vprotolist = VProtoList::CreateProtoList();

   for (VComp* pComp = vprog->FirstComp(); pComp; pComp=pComp->Next())
   {
      pComp->SetCommandFile(CommandFile);
      VProto* vprotoInt = vprotolist->CreateProto(pComp,
                                                  "callsdll.dll",
                                                  "SendInt(int)");

      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         for (VBBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next())
         {
            vprotoInt->AddCall(pBlk,
                               BEFORE,
                               pBlk->BlockTarget() ?
                                    pBlk->BlockTarget()->BlockId() : 0);

            vprotoInt->AddCall(pBlk,
                               AFTER,
                               pBlk->BlockFollower() ?
                                    pBlk->BlockFollower()->BlockId() : 0);
         }
      }

      vprotolist->Commit();
      pComp->Write(ModName(pComp->InputName(), "BT"));
   }

   vprog->Destroy();
}

void ProcCalls()
{
   // Testing: BEFORE and AFTER on Procs.
   // Testing: putting calls on top of each other.
   // Testing: Char* as parameter.

   VProg *vprog = VProg::Open(ProgramName);
   VProtoList *vprotolist = VProtoList::CreateProtoList();

   for (VComp* pComp = vprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      pComp->SetCommandFile(CommandFile);
      VProto* vprotoStr = vprotolist->CreateProto(pComp,
                                                  "callsdll.dll",
                                                  "SendStr(char*)");

      for(VProc *pProc=pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         sprintf(badstr,"entering %s",pProc->Name());
         vprotoStr->AddCall(pProc,BEFORE,"\n");
         vprotoStr->AddCall(pProc,BEFORE,badstr);

         sprintf(badstr,"exiting %s",pProc->Name());
         vprotoStr->AddCall(pProc,AFTER,badstr);
         vprotoStr->AddCall(pProc,BEFORE,"\n");
      }

      vprotolist->Commit();
      pComp->Write(ModName(pComp->InputName(), "PT"));
   }

   vprog->Destroy();
}


void CompCalls()
{
   // Testing Comp calls

   VProg *vprog = VProg::Open(ProgramName);
   VProtoList *vprotolist = VProtoList::CreateProtoList();

   for (VComp* pComp = vprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      pComp->SetCommandFile(CommandFile);
      VProto* vprotoStr = vprotolist->CreateProto(pComp,"callsdll.dll","SendStr(char*)");

      for (int i=0;i<10;i++)
      {
         char temp[4000];
         sprintf(temp,"%d\n",i);
         vprotoStr->AddCall(pComp, BEFORE, temp);
      }

      vprotolist->Commit();
      pComp->Write(ModName(pComp->InputName(), "CT"));
   }

   vprog->Destroy();
}

int __cdecl main(int argc, char** argv)
{
   if (argc < 2)
   {
       printf("Usage: CALLS [image-name]\n");
       return 1;
   }

   ProgramName = argv[1];
   CommandFile = argv[2];
   try
   {
      CompCalls();
      ProcCalls();
      BlockCalls();
      EffCalls();
      RegCalls();
      SwapCalls();
      FlagsCalls();
      DoubleCalls();
      OrderedCalls();
   }

   catch (VErr& verr)
   {
      printf("Error: %s\n", verr.GetWhat());
      return 1;
   }

   catch (...)
   {
      printf("Error: Unknown\n");
      return 1;
   }

   return 0;
}
