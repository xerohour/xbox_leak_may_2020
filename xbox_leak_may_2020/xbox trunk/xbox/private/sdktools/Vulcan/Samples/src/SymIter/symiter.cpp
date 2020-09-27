/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: symiter.cpp
*
* File Comments:
*
*
***********************************************************************/
#pragma warning(disable: 4514)

#include <stdio.h>
#include "vulcanapi.h"


void symiter(char* szImage)
{
   VProg *pprog = VProg::Open(szImage);

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      ADDR         addr;
      const char*  szName;
      VSymIter     *psiProc;   
      VSymIter     *psiBlk;

      printf("*** Inputname : %s ***\n", pComp->InputName());

      //
      // Print symbols for procedures
      //
      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         psiProc = pProc->Symbols(pComp);
         for (psiProc->First(); !psiProc->Done() ; psiProc->Next())
         {
            addr = pProc->Addr();
            printf("<%08lx>\n", addr);

            if (psiProc->IsPublic())
            {
              szName = psiProc->SymName();
              printf("\tPublicProc   : %s\n", szName);

              szName = psiProc->Name();
              printf("\tPublicProcUnd: %s\n", szName);
            }
            else
            {
              szName = psiProc->SymName();
              printf("\tProc   : %s\n", szName);

              szName = psiProc->Name();
              printf("\tProcUnd: %s\n", szName);
            }
         }

         //
         // Print symbols for blocks
         //
         for(VBBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next())
         {
            psiBlk = pBlk->Symbols(pComp);
            for (psiBlk->First(); !psiBlk->Done(); psiBlk->Next())
            {
               addr = pBlk->Addr();
               printf("\t\t<%08lx>\n", addr);

               if (psiBlk->IsPublic())
               {
                  szName = psiBlk->SymName();
                  printf("\t\t\tPublicBlk    : %s\n", szName);

                  szName = psiBlk->SymName();
                  printf("\t\t\tPublicBlkUnd : %s\n", szName);
               }
               else
               {
                  szName = psiBlk->SymName();
                  printf("\t\t\tBlk          : %s\n", szName);

                  szName = psiBlk->Name();
                  printf("\t\t\tBlkUnd       : %s\n", szName);
               }
            }
         }
       
         if (psiBlk)
         {
             // 
             // Always destroy to avoid leaks
             //
             psiBlk->Destroy();
         }
      }

      if (psiProc)
      {
         // 
         // Always destroy to avoid leaks
         //
         psiProc->Destroy();
      }
   }

   pprog->Destroy();
}


int __cdecl main(int argc, char** argv)
{

   if (argc != 2)
   {
      printf("usage: symiter foo.exe\n");
      return 1;
   }

   try
   {
      symiter(argv[1]);
   }
   catch (VErr& verr)
   {
      verr.SetFatal();
      verr.DoErrHandler();
   }

   return 0;
}
