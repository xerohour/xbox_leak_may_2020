/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: VATrack.cpp
*
* File Comments:
*
*
***********************************************************************/
#pragma warning(disable: 4514)

#include "stdio.h"
#include "vulcanapi.h"
#include "string.h"

int __cdecl main(int argc, char** argv);
void __stdcall Myhandler(VErr verr);

void ProcTrace(char* szImage, char *szCmdFile)
{
   VProg *pprog = VProg::Open(szImage, false, szCmdFile);

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      printf("%s\n",pComp->InputName());

      // Create the prototype
      VProtoList *pProtoList = VProtoList::CreateProtoList();
      VProto *pProto = pProtoList->CreateProto(pComp, "ProcTraceDll.dll", "ProcTrace(char*)" );

      // Instrument the beginning of each routine
      for (VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         pProto->AddCall( pProc, BEFORE, pProc->Name() );
      }

      // Commit the changes
      pProtoList->Commit();

      // Write out the image
      pComp->Write();
   }
}


int __cdecl main(int argc, char** argv)
{

   if (argc != 2 && argc != 3)
   {
      printf("usage: ProcTrace foo.exe [foo.cmdfile]\n");
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
