/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 2000 Microsoft Corporation.
*
* File: annotations.cpp
*
* File Comments:
*
*
***********************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "vulcanapi.h"

const char *g_szFilter;

void ShowAnnotations(const char *szComponentName)
{
   VComp *pComp = VComp::Open((char *) szComponentName, 0);

   printf("Annotations in %s\n", szComponentName);

   VAnnotationIter *pIter = pComp->AnnotationIter(g_szFilter);

   // Now iterate over all the VAnnotations.

   const VAnnotation *pAnnotation = pIter->Curr();

   while (pAnnotation != NULL)
   {
      printf("   0x%I64X", pAnnotation->Addr());

      VProc *pProc = pComp->FindProc(pAnnotation->Addr());

      if ((pProc != NULL)  && (pProc->Name() != NULL))
      {
         printf(": in VProc %s\n", pProc->Name());
      }

      else
      {
         printf( "\n" );
      }

      for (size_t x = 0; x < pAnnotation->Count(); x++)
      {
         size_t cch = pAnnotation->StringLength(x);
         assert(cch != 0);

         char *sz = new char[cch];

         size_t cchT = pAnnotation->String(x, sz, cch);
         assert(cchT == cch);

         printf("      %u: %s\n", x, sz);

         delete [] sz;
      }

      pAnnotation = pIter->Next();
   }

   delete pIter;

   pComp->Destroy();
}


void usage(const char *szProgname, int exitcode)
{
   FILE *out = exitcode ? stderr : stdout;

   fprintf(out,
           "usage: %s [-f filter] component...\n"
           "\n"
           "    List annotations present in components.\n"
           "\n"
           "    -f filter  Pass string to VComp::AnnotationIter() to restrict\n"
           "               output to specific annotations.\n",
           szProgname);

   exit(exitcode);
}


int __cdecl main(int argc, const char *argv[])
{
   try
   {
      int optind = 1;

      if (optind >= argc)
      {
         usage(argv[0], 2);
      }

      while (optind < argc)
      {
         const char *szArg = argv[optind++];

         if (strcmp(szArg, "-f") == 0)
         {
            if (optind >= argc)
            {
               usage(argv[0], 2);
            }

            g_szFilter = argv[optind++];
         }

         else if (szArg[0] == '-')
         {
            usage(argv[0], 2);
         }

         else
         {
            ShowAnnotations(szArg);
         }
      }
   }

   catch (VErr& verr)
   {
      printf("Error: %s\n", verr.GetWhat());
      exit(1);
   }

   return 0;
}
