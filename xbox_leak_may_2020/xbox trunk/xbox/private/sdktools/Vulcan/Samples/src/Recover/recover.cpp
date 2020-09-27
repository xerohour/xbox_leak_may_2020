/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1999 Microsoft Corporation.
*
* File: Recover.cpp
*
* File Comments: Force execution IA64 recovery code sequences.
*
*
***********************************************************************/
#pragma warning(disable: 4514)

#include "vulcanapi.h"

#include <stdio.h>
#include <stdlib.h>

int __cdecl main(int argc, char** argv);
void __stdcall Myhandler(VErr verr);
void StressRecoveryCode(char* szImage);


void StressRecoveryCode(char* szImage)
{
   VProg *pprog = VProg::Open(szImage);
   int aTrans = 0, sTrans = 0, saTrans = 0;

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      printf("%s\n",pComp->InputName());
      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         for(VBBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next())
         {
            if (pBlk->PlatformT() != platformtIA64)
            {
               printf("usage: recover foo.exe (IA64 binaries only)\n");
               exit(1);
            }

            // If we used FirstAllBlock and NextAll instead of FirstBlock
            // and Next, we would need to avoid data blocks.

            for(VInst *pInst = pBlk->FirstInst(); pInst; pInst = pInst->Next())
            {
               if (pInst->IsMarker()) continue;

               switch (pInst->Opcode())
               {
                  case COp::LDD:
                  case COp::LDW:
                  case COp::LDH:
                  case COp::LDB:

                     // Currently not handling post-incrementing,
                     // speculative loads.

                     if (pInst->Operand(Inst::Src2).Type() != OpndNotUsed)
                     {
                        break;
                     }

                     // Replace ld.a rx=[ry] with ld rx=[ry]

                     if (pInst->TestCompleter(LdAdvanced))
                     {
                        pInst->SetCompleter(LdNone);
                        aTrans++;
                     }

                     else if (pInst->TestCompleter(LdSpeculative))
                     {
                        // Replace ld.s rx=[ry] with ld.s rx=[r0]

                        VOperand address = VAddress::Create(Register::GR0, 0 );
                        pInst->SetOperand(Inst::Src1, address);
                        sTrans++;
                     }

                     else if (pInst->TestCompleter(LdSpeculativeAdvanced))
                     {
                        // Replace ld.sa rx=[ry] with ld.sa rx=[r0]

                        VOperand address = VAddress::Create(Register::GR0, 0 );
                        pInst->SetOperand(Inst::Src1, address);
                        saTrans++;
                     }
                     break;
                       
                  default:
                     break;
               }
            }
         }
      }

      pComp->Write();
   }

   // To save time, we don't destroy the program object
   // pprog->Destroy();

   printf("\nld.a  Transformations = %d\n", aTrans);
   printf("ld.s  Transformations = %d\n", sTrans);
   printf("ld.sa Transformations = %d\n", saTrans);
}


int __cdecl main(int argc, char** argv)
{

   if (argc != 2)
   {
      printf("usage: recover [Image-Name] (IA64 binaries only)\n");
      return 1;
   }

   try
   {
      StressRecoveryCode(argv[1]);
   }

   catch (VErr& verr) 
   {
      printf("Error: %s\n", verr.GetWhat());
      return 1;
   }

   catch (...)
   {
      printf("Error: unknown\n");
      return 1; 
   }

   return 0;
}
