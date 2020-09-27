/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: Peephole.cpp
*
* File Comments:
*
*
***********************************************************************/
#include "vulcanapi.h"

#include <stdio.h>

void BranchChaining(VInst *vinst)
{
   while (true)
   {
      VBlock *targetblk = vinst->BlockTarget();
      if (targetblk &&
          targetblk->IsDataBlock() ||
          (NULL == targetblk->FirstInst()))
      {
         break;
      }

      VInst *targetinst = targetblk->FirstInst();
      EOpcodes vinstOpcode = vinst->Opcode();
      EOpcodes targetOpcode = targetinst->Opcode();        
      if (targetOpcode == COp::JMP || targetOpcode == COp::CEE_BR)
      {
         if (targetinst->BlockTarget())
         {
            vinst->SetBlockTarget( targetinst->BlockTarget() );
         }
         else if (targetinst->Operand(Inst::Src1).Type() == OpndAddress &&
                  (vinstOpcode == COp::JMP /* || vinstOpcode == COp::CALL */))
         {
            VAddress *pAddr = VAddress::Create(
                                  targetinst->Operand(Inst::Src1).Address() );

            vinst->SetOperand(Inst::Src1, VOperand( pAddr ) );
            break;
         }
         else break;

      }
      else if (COp::IsCondBranch(vinstOpcode) && targetOpcode == vinstOpcode)
      {
         if (targetinst->BlockTarget())
         {
             vinst->SetBlockTarget(targetinst->BlockTarget()); 
         }
         else break;
      }
      else break;
   }
}

void Peephole(VProg *vprog)
{
   for (VComp *pComp = vprog->FirstComp(); pComp; pComp = pComp->Next() )
   {
      for (VSect *pSect = pComp->FirstSect(); pSect; pSect = pSect->Next() )
      {
         for (VProc *pProc = pSect->FirstProc(); pProc; pProc = pProc->NextSectProc() )
         {
            for (VBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next() )
            {
#define SUPERBLOCKS
#ifdef SUPERBLOCKS

               for (VInst *pInst = pBlk->FirstInst(); pInst; pInst = pInst->Next() )
               {
                  EOpcodes opcode = pInst->Opcode();

                  if ((opcode == COp::JMP    ||
                       opcode == COp::CEE_BR ||
                    /* opcode == COp::CALL   || */
                       COp::IsCondBranch(opcode)) &&
                      (pInst->Operand(Inst::Src1).Type() == OpndBlock) &&
                      pInst->BlockTarget())
                  {
                     BranchChaining(pInst);
                  }
               }

#else // SUPERBLOCKS

               VInst *vinst = pBlk->LastInst();
               EOpcodes opcode = vinst->Opcode();
               if ((opcode == COp::JMP    ||
                    opcode == COp::CEE_BR ||
                 /* opcode == COp::CALL   || */
                   COp::IsCondBranch(opcode)) &&
                   vinst->BlockTarget())
               {
                  BranchChaining(vinst);
               }

#endif // SUPERBLOCKS

                }
            }
        }

        pComp->Write();
    }
}



int __cdecl main(int argc, char** argv)
{
   
   if (argc != 2)
   {
      printf("Usage: Peephole [Image-Name]\n");
      return 1;
   }

   try
   {
      VProg *vprog = VProg::Open(argv[1]);
      Peephole(vprog);  

      // To save time, we don't destroy the vprog
      // vProg->Destroy();
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
