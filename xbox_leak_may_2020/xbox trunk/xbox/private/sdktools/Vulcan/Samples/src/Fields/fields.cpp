/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: Fields.cpp
*
* File Comments:
*  This program demonstrates the VSymbol interface.
*
*
***********************************************************************/
#pragma warning(disable: 4514)

#include "vulcanapi.h"

#include <stdlib.h>
#include <stdio.h>

int __cdecl main(int argc, char** argv);
void __stdcall Myhandler(VErr verr);
void UIFields(char* szImage);

void Indent( int cIndent )
{
   for (int i=0; i<cIndent; i++)
   {
      printf("\t");
   }
}

void ProcessSym( VSymbol *pSym, int cIndent )
{
   // Only go so deep
   if (cIndent > 3)
   {
      return;
   }

   if (pSym)
   {
      Indent( cIndent );
      printf("%d %s : ", pSym->Offset(), pSym->Name() );

      char szBuf[1024];
      pSym->Type(szBuf, sizeof(szBuf));
      printf("%s\n", szBuf );

      if (pSym->IsBitField())
      {
         Indent( cIndent + 1);
         printf("BitField (%d+%d)\n", pSym->BitOffset(), pSym->BitSize() );
      }

      bool fPointer = false;
      while (pSym->IsPointer())
      {
         if (!fPointer)
         {
            fPointer = true;
            Indent( cIndent + 1 );
         }
         printf("->");
         pSym = pSym->PTarget();

         if (NULL == pSym)
         {
            printf("\n");
            return;
         }
      }
      if (fPointer)
      {
         printf("\n");
      }

      for (VSymbol *pField = pSym->FirstField(); pField; pField = pField->Next())
      {
         ProcessSym( pField, cIndent + 1 );
      }

      bool fBases = false;
      for (VSymbol *pBase = pSym->FirstBase(); pField; pField = pField->Next())
      {
         if (!fBases)
         {
            Indent( cIndent );

            printf("Base classes:\n");
         }

         ProcessSym( pBase, cIndent + 1 );
      }
   }
}


void UIFields(char* szImage)
{
   VProg *pprog = VProg::Open(szImage);
   int count = 0;

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      printf("%s\n",pComp->InputName());
      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         printf("%s:\n", pProc->Name() );
          
         VSymbol *pSym;

         for (pSym = pProc->FirstParam(pComp); pSym; pSym = pSym->Next())
         {
            ProcessSym( pSym, 1 );
         }

         for (pSym = pProc->FirstLocal(pComp); pSym; pSym = pSym->Next())
         {
            ProcessSym( pSym, 1 );
         }

         ProcessSym( pProc->ReturnSymbol(pComp), 1 );
      }
   }

   // To save time, we don't destroy the program object
   // pprog->Destroy();
}


int __cdecl main(int argc, char** argv)
{

   if (argc != 2)
   {
      printf("Fields:   fields foo.exe\n");
      return 1;
   }

   try
   {
      UIFields(argv[1]);
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
