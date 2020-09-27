/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: Addnop.cpp
*
* File Comments:
*
*
***********************************************************************/
#pragma warning(disable: 4514)

#include "vulcanapi.h"

#include <stdlib.h>
#include <stdio.h>

int __cdecl main(int argc, char** argv);
void __stdcall Myhandler(VErr verr);
void UINop(char* szImage);


void UINop(char* szImage)
{
   VProg *pprog = VProg::Open(szImage);
   int count = 0;

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      printf("%s\n",pComp->InputName());
      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         for(VBBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next())
         {
            // If we used FirstAllBlock and NextAll instead of FirstBlock and
            // Next, we would need to avoid data blocks (pBlk->IsDataBock()).

            for(VInst *pInst = pBlk->FirstInst(); pInst; pInst = pInst->Next())
            {
               VInst * newInst;

               switch (pBlk->PlatformT())
               {
                  case platformtMSIL:
                     pInst->InsertPrev(VInst::Create(COp::CEE_NOP));
                     count++;
                     break;

                  case platformtX86:
                     pInst->InsertPrev(VInst::Create(COp::NOP));
                     count++;
                     break;

                  case platformtIA64:
                     // Alloc must always appear first in a bundle

                     if (pInst->Opcode() != COp::ALLOC &&
                         pInst->Opcode() != COp::NOP_M &&
                         pInst->Opcode() != COp::NOP_I &&
                         pInst->Opcode() != COp::NOP_B &&
                         pInst->Opcode() != COp::NOP_F &&
                         pInst->Opcode() != COp::NOP_X &&
                         pInst != pBlk->LastInst())
                     {
                        // IA64 implements switch tables in code
                        // Hence there are some sequences that
                        // are not allowed to changed in size.
                        // These tests screen them out of consideration.

                        newInst = VInst::Create(COp::NOP_M, 0xbeef);
                        pInst->InsertPrev(newInst);
                        count++;
                     }
                     break;

                  default:
                     fprintf(stderr, "Unsupported architecture type\n");
                     exit(1);
               }
            }
         }
      }

      printf("%d NOP's inserted.\n", count);
      pComp->Write();
   }

   // To save time, we don't destroy the program object
   // pprog->Destroy();

   // This should only be called after ALL Vulcan objects have been Destroyed
   // It will clean up any outstanding allocations
   // VulcanCleanUpAllAllocations();
}


int __cdecl main(int argc, char** argv)
{
   if (argc != 2)
   {
      printf("Usage: Addnop [image-name]\n");
      return 1;
   }

   try
   {
      UINop(argv[1]);
   }
   catch (VErr& verr) 
   {
      printf("Error: %s\n",verr.GetWhat());
      return 1;
   }
   catch (...)
   {
      printf("Error: Unknown\n");
      return 1; 
   }

   return 0;
}
