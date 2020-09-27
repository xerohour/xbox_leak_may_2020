/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: ProcProf.cpp
*
* File Comments:
*
*
***********************************************************************/
#pragma warning(disable: 4514)

#include "vulcanapi.h"
#include "string.h"
#include "procprof.h"
#include "stdio.h"

int __cdecl main(int argc, char** argv);
void __stdcall Myhandler(VErr verr);

void ProcTrace(char* szImage, char *szCmdFile)
{
   VProg *pprog = VProg::Open(szImage, false, szCmdFile);
   int cProc = 0;

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      printf("%s\n",pComp->InputName());

      VBlock *pBlkInstr = pComp->CreateImport( "procprofdll.dll", "_ProcProf@0" );
      VAddress *pAddr = VAddress::Create( pBlkInstr );

      // Add a call to our dll at the beginning of each routine

      for (VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         cProc++;
         
         if (strstr( pProc->Name(), "setjmp" ))
         {
             // This function can return more than once per call, so ignore it
             continue;
         }
          
         VBlock *pBlk = pProc->FirstBlock();
         pBlk->InsertFirstInst( VInst::Create( COp::CALL, pAddr ));
      }

      // Leave an extra "empty" entry in the proc table

      cProc++;

      // Create a data section in the image to store the address and
      // name of each routine.

      VSect *pSectData = pComp->NewSect( ".prof", Sec_READWRITE );
      VProc *pProcData = VProc::Create(pComp, "Data");
      pSectData->InsertFirstProc( pProcData );
      FUNDATA *rgData = new FUNDATA[cProc];
      memset( rgData, 0, sizeof(FUNDATA) * cProc);
      VBlock *pBlk = VBlock::CreateDataBlock( pComp,
                                              (BYTE *)rgData,
                                              cProc * sizeof(FUNDATA) );
      pProcData->InsertFirstBlock( pBlk );

      // Add pointers in the section which point at each routine and its name
      // (which we also add to the section).

      VRelocIter ri = pBlk->FirstReloc();
      int iProc = 0;
      for (pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         VReloc *pReloc = VReloc::Create(
                              pProc->FirstBlock(),
                              0,
                              ((BYTE *)(&rgData[iProc].addr)) - (BYTE *)rgData,
                              VReloc::Absolute );
         ri.AddLast(pReloc);

         const char *szName = pProc->Name();
         VBlock *pBlkStr = VBlock::CreateDataBlock( pComp,
                                                    (BYTE *)szName,
                                                    strlen(szName) + 1 );
         pProcData->InsertLastBlock( pBlkStr );

         pReloc = VReloc::Create( pBlkStr,
                                  0,
                                  ((BYTE *)(&rgData[iProc].szFun)) - (BYTE *)rgData,
                                  VReloc::Absolute );
         ri.AddLast(pReloc);

         iProc++;
      }

      // Init the last entry with a large address

      rgData[iProc].addr  = 0xFFFFFFFF;
      rgData[iProc].szFun = NULL;

      // Add a call to our dlls initialization routine before the binary begins executing

      VProtoList *pList = VProtoList::CreateProtoList();
      VProto *pProto = pList->CreateProto( pComp, "procprofdll.dll", "InitProcProf(blockaddr, int)");
      pProto->AddCall( pComp, BEFORE, pBlk, cProc );
      pList->Commit();

      // Write out the image

      pComp->Write();
   }
}


int __cdecl main(int argc, char** argv)
{

   if (argc != 2 && argc != 3)
   {
      printf("usage: ProcTrace <Image-Name> [foo.cmdfile]\n");
      return 1;
   }

   try
   {
      ProcTrace(argv[1], argv[2]);
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
