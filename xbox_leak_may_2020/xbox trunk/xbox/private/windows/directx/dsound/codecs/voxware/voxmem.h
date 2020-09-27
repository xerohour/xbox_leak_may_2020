/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       VoxMem.h (more comonly known as "Ilan's MACROS from hell file!")                                                                    
*                                                                              
* Purpose:        Controls all dynamic memory allocation in codec                                                             
*                                                                              
* Author/Date:    Ilan Berci 96/11/20                                                                
********************************************************************************
* $Header:   G:/r_and_d/archives/common/utils/VoxMem.h_v   1.3   21 Apr 1998 17:33:46   epiphanyv  $
*******************************************************************************/

#ifndef VOXMEM_H
#define VOXMEM_H

#ifdef WIN16
#define SCRATCH_DEFINE 0
#else
#define SCRATCH_DEFINE 0        /* Main Scratch Memory switch */
#endif

#define SCRATCH_DIAGNOSTICS 0   /* Diagnostic utility that requires printf */

#define ALIGNMENT_WORD_SIZE 8   /* The number of bytes to align stack by */
#define ALIGN_SHIFT 3           /* 2 to the power of ALIGN_SHIFT = ALIGNMENT_WORD_SIZE */

#define SCRATCH_PAD_DWORD 0xFACE   /* 32 bit word to use in padding */

#ifndef Vsize_t
#define Vsize_t unsigned int   /* Voxware size_t */
#endif /* Vsize_t */

/* Replacement for callocs for DSP implementation */
#if(SCRATCH_DIAGNOSTICS==0)
#define VOX_MEM_INIT(ptr,num,size) (!(ptr=calloc(num,size)))
#else
unsigned int voxHeapCount(int num, Vsize_t size); /* returns 1 on success */
unsigned int voxGetHeapCount();
#define VOX_MEM_INIT(ptr,num,size) (!((ptr=calloc(num,size))&&voxHeapCount(num,size)))
#endif

#define VOX_MEM_FREE(ptr) if(ptr) { free(ptr); ptr=NULL; }

#define VOX_MEM_FREE_CONST(ptr) if(ptr) { free((void *)ptr); ptr=NULL; }

#if SCRATCH_DIAGNOSTICS==1
#if SCRATCH_DEFINE==0
#error In order to use Scratch Diagnostics, Scratch Define must be turned on
#endif
#endif

/* Replacement for stack for DSP implementation */
#if SCRATCH_DEFINE==1

#ifndef NDEBUG
#ifndef assert
#error "assert.h"  must be included in this file if you are intendending to use "VoxMem.h"
#endif /* assert */
#endif /* NDEBUG */

#if SCRATCH_DIAGNOSTICS==1
typedef struct tagFILE_SCRATCH_FILE {
   char                     *pFile;
   unsigned int             wAmount;
   struct tagFILE_SCRATCH_FILE* pLast;
} FILE_SCRATCH_BLK;
#endif /* SCRATCH_DIAGNOSTICS */

/* Pre-defines for initializing and freeing at the upermost level */
typedef struct _tagScratchStruct {
   unsigned char *bStartScratch;
   unsigned char *bCurrentScratch;
   unsigned char *bEndScratch;
   unsigned char *bMaximumScratch;
#if SCRATCH_DIAGNOSTICS==1
   struct tagFILE_SCRATCH_FILE*  pFileScratchStruct;
#endif /* SCRATCH_DIAGNOSTICS */
} SCRATCH_STRUCT;

#define STACK_DECLARE SCRATCH_STRUCT *pScratchStruct;

#define STACK_ASSIGN(ptr) pScratchStruct=(SCRATCH_STRUCT *)(ptr);

#define STACK_SET(ptr,size) voxStackSet(&ptr, size, &pScratchStruct, __FILE__) 

#define STACK_UNSET(ptr) voxStackUnSet(((SCRATCH_STRUCT **)(&ptr)));

#define STACK_VALIDATE() voxStackValidate(&pScratchStruct);

#define STACK_S pScratchStruct,

#define STACK_S_ pScratchStruct

#define STACK_R SCRATCH_STRUCT *pScratchStruct,  

#define STACK_R_ SCRATCH_STRUCT *pScratchStruct

#if SCRATCH_DIAGNOSTICS==1 
#define STACK_SHOW() voxStackShow(pScratchStruct);
#else 
#define STACK_SHOW()
#endif

/* Pre-defines for implementing the stack at lower levels */

#define STACK_INIT unsigned char *dwScratchInitial=pScratchStruct->bCurrentScratch; \
   typedef struct tagScratch { unsigned long padDWordStart; 
   /* body of stack fits in here between these 2 defines */

#define STACK_INIT_END unsigned long padDWordEnd; } SCRATCH_BLK; \
   SCRATCH_BLK *pScratch= ((SCRATCH_BLK *)(pScratchStruct->bCurrentScratch));

#define STACK_START pScratch->padDWordStart=pScratch->padDWordEnd=(unsigned long)SCRATCH_PAD_DWORD; \
   voxStackStart(pScratchStruct, sizeof(SCRATCH_BLK), __FILE__); 

#define STACK_END  assert(pScratch->padDWordStart==(unsigned long)SCRATCH_PAD_DWORD); \
   assert(pScratch->padDWordEnd==(unsigned long)SCRATCH_PAD_DWORD); \
   voxStackEnd(pScratchStruct, sizeof(SCRATCH_BLK)); \
   assert(pScratchStruct->bCurrentScratch==dwScratchInitial);

#define STACK_ATTACH(type, id) type id= pScratch->id;

#define STACK_ACCESS pScratch->

unsigned short voxStackSet(void **hvScratchStruct, unsigned int size, SCRATCH_STRUCT **hScratchStruct, char *pFile);
unsigned short voxStackUnSet(SCRATCH_STRUCT **hScratchStruct);
unsigned short voxStackValidate(SCRATCH_STRUCT **hScratchStruct);
unsigned short voxStackShow(const SCRATCH_STRUCT *pScratchStruct);
unsigned short voxStackStart(SCRATCH_STRUCT *pScratchStruct, Vsize_t sizeScratch, const char *pszFile);
unsigned short voxStackEnd(SCRATCH_STRUCT *pScratchStruct, Vsize_t sizeScratch);

#else

#define STACK_DECLARE
#define STACK_ASSIGN(ptr)
#define STACK_SET(ptr,size) 0
#define STACK_UNSET(ptr)
#define STACK_VALIDATE()
#define STACK_SHOW()
#define STACK_S
#define STACK_R
#define STACK_INIT
#define STACK_INIT_END
#define STACK_START
#define STACK_END
#define STACK_ATTACH(type, id)
#define STACK_ACCESS 

#endif /* SCRATCH_DEFINE */

unsigned char voxIsPreEmptive();
unsigned short voxSetNonPreEmptive();

#endif /* VOXMEM_H */



