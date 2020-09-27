/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: DynInstCount.cpp
*
* File Comments:
*
*
***********************************************************************/
#pragma warning(disable: 4514)

#include "stdio.h"
#include "vulcanapi.h"
#include "vulcanbmp.h"
#include "string.h"
#include "malloc.h"

int __cdecl main(int argc, char** argv);
void __stdcall Myhandler(VErr verr);
bool FDefsFlags(VInst *pInst);

void DynInstCount(char* szImage, char *szCmdFile)
{
   VProg *pprog = VProg::Open(szImage, false, szCmdFile);

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      printf("%s\n",pComp->InputName());

      pComp->Build();

      // Create a global to store the count in its own section

      BYTE count[4] = {0,0,0,0};
      VBlock *pBlkCount = VBlock::CreateDataBlock( pComp, count, sizeof(count) );
      VSect *pSect = pComp->NewSect( ".count", Sec_READWRITE );
      VProc *pDataProc = pSect->NewProc( "Count" );
      pDataProc->InsertFirstBlock(pBlkCount);

      // Create a block for the names of all the procedures

      VBlock *pBlkNames = VBlock::CreateDataBlock( pComp, count, sizeof(count) );
      int cbMax = 64*1024;
      char *szNames = (char *)malloc( cbMax );
      char *pchNextName = szNames;
      pDataProc->InsertFirstBlock(pBlkNames);

      int iProc = 0;

      for (VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         VBlock *pBlk = pProc->FirstBlock();

         //create an address object to reference the block
         VAddress *pAddrCount = VAddress::Create( pBlkCount,
                                                  iProc * sizeof(int) );

         const char *szName = pProc->Name();
         int cch = strlen(szName);

         if (pchNextName - szNames + cch + 1 >= cbMax)
         {
            cbMax *= 2;
            int ichNext = pchNextName - szNames;
            szNames = (char *)realloc(szNames, cbMax );
            pchNextName = szNames + ichNext;
         }

         strcpy(pchNextName, szName);
         pchNextName += cch + 1;

         for (; pBlk; pBlk = pBlk->Next())
         {
            int iCount = pBlk->CountInsts();

            if (iCount)
            {
               VAddress *pAddrAdd = VAddress::Create( X86Register::EAX, iCount );
                
               //we want to put an add [count], iCount into the block
               VInst *pInstPush = VInst::Create(COp::PUSHD, X86Register::EAX);
               VInst *pInstMov1 = VInst::Create(COp::MOVD,  X86Register::EAX, pAddrCount );
               VInst *pInstLea  = VInst::Create(COp::LEAD,  X86Register::EAX, pAddrAdd );
               VInst *pInstMov2 = VInst::Create(COp::MOVD,  pAddrCount,       X86Register::EAX );
               VInst *pInstPop  = VInst::Create(COp::POPD,  X86Register::EAX);

               pAddrAdd->Destroy();
               
               pBlk->InsertFirstInst( pInstPush );
               pInstPush->InsertNext( pInstMov1 );
               pInstMov1->InsertNext( pInstLea );
               pInstLea->InsertNext( pInstMov2 );
               pInstMov2->InsertNext( pInstPop );
            }
         }

         pAddrCount->Destroy();

         iProc++;
      }

      // Resize the data block

      int *rgCount = new int[iProc];
      memset( rgCount, 0, iProc * sizeof(int) );
      pBlkCount->SetData( (BYTE *)rgCount, iProc * sizeof(int) );

      // Resize the name block

      pBlkNames->SetData( (BYTE *)szNames, pchNextName - szNames );

      // Pass the pointer to the count to the dll

      VProtoList *pProtoList = VProtoList::CreateProtoList();
      VProto *pProto = pProtoList->CreateProto(
                           pComp,
                           "DynInstCountDll.dll",
                           "_cdecl SavePDynInstCount(blockaddr, int, blockaddr)");

      pProto->AddCall(pComp, BEFORE, pBlkCount, iProc, pBlkNames );
      pProtoList->Commit();

      pComp->Write();
   }
}


int __cdecl main(int argc, char** argv)
{

   if (argc != 2 && argc != 3)
   {
      printf("Usage: DynInstCount <Image-Name> <foo.cmdfile>\n");
      return 1;
   }

   try
   {
      DynInstCount(argv[1], argv[2]);
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
