/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: vulcanpa.h
*
* File Comments:
*
*
***********************************************************************/

#pragma once

#include <string.h>

// Padding is an optimization to help workset and cache behaivor.
// See the paper "Improving cache behavior of dynamically allocated data
// structures" by Dan N. Troung et al.

// UNDONE: Padding is all wrong for IA64 hosting - turn it off for now.
#ifdef _M_IA64
#define VULCAN_PA_OFF
#endif

//Don't create an instance of CPad except through the #define below.  This will allow padding to be turned on and off easily.
struct CPad {
   enum {PADSIZE=4092};

   static void VULCANCALL Memset(void* vp,int value,int size) {

      if (size > PADSIZE) {
               for (char* i=(char*)vp;i<(char*)vp + size;i+=PADSIZE+4) {
                   int* j = (int*) i;
               *j=value;
               }
      } else {
         memset(vp,value,size);
      }

   }

   static void VULCANCALL Memcpy(void* dest,void* src,int size){
      if (size > PADSIZE) {
         int* d = (int*) dest;
         int* s = (int*) src;
         for (int i=0;i<size/4;i+=(PADSIZE+4)/4) {
            d[i] = s[i];
         }
      } else {
         memcpy(dest,src,size);
      }

   }
private:
    char m_pad[PADSIZE];
};


#ifdef VULCAN_PA_OFF
#define CPAD(ID)
#else
#define CPAD(ID) CPad  m_pad##ID;
#endif
