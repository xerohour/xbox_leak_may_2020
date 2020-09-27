/*++

Copyright (c) 1995 Microsoft Corporation

Module Name:

        intxcpt.c

Abstract:

        test integer exceptions

Author:

        johndaly

Environment:

        x86-only - used to test emulators (16 and 32 bit)

Notes:

        This is for x86 only, it is an instruction-level test designed to make
        sure SEH works correctly with 486 emulators.  All test output to stdout.

Revision History:

        7/15/96 - creation

--*/

/* global pragmas */
#pragma optimize ("", off)

/* includes */
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


/* function declarations */
void __cdecl main(void);
void InvOp(void);
LONG ExceptionFilter(EXCEPTION_POINTERS *);

/* global data */
WORD wControlWord = 0;  /* used by ResetEnableExcept macro */

/* macro */
/* 1 arg ie "inc ax" */
#define TryInstruction(instruction)                         \
    puts(#instruction);                                    \
    __try {                                                 \
        _asm {                                              \
            instruction                                     \
        }                                                   \
    }                                                       \
   __except(FPExceptionFilter(GetExceptionInformation())){}

/* 2 args ie "add ax, bx" */
#define TryInstruction1(instruction, instruction1)           \
    puts(#instruction"," #instruction1);                      \
    __try {                                                 \
        _asm {                                              \
            instruction, instruction1                       \
        }                                                   \
    }                                                       \
   __except(FPExceptionFilter(GetExceptionInformation())){}


/*****************************************************************************
 *
 * Name        : main
 *
 * Purpose     : run all the sub-functional tests
 *
 * Args        : none
 *
 * Returns     : nothing
 *
 * Mod Globals :
 *
 * Notes       :
 *
  *****************************************************************************/
void __cdecl main(void)   {

    InvOp();

}

/*****************************************************************************
 *
 * Name        : InvOp
 *
 * Purpose     : create invalid-operation exceptions
 *
 * Args        : none
 *
 * Returns     : none
 *
 * Mod Globals : none
 *
 * Notes       :
 *
  *****************************************************************************/
void InvOp(void)  {
    /* locals */
    unsigned long    Dead;
    void *DeadPtr;
    DeadPtr = &Dead;

    TryInstruction(cli)
    TryInstruction(clts)
    TryInstruction(hlt)
    TryInstruction1(in  al, 0000h)
    TryInstruction(insb)
    TryInstruction(insw)
    TryInstruction(insd)
    TryInstruction(invd)
    TryInstruction(invlpg  DeadPtr)
    TryInstruction(lgdt    DeadPtr)
    TryInstruction(lidt    DeadPtr)
    TryInstruction(lldt    DeadPtr)
    TryInstruction(lmsw    DeadPtr)
    TryInstruction(ltr     DeadPtr)
    TryInstruction1(mov     CR0, eax)
    TryInstruction1(mov     eax, CR0)
    TryInstruction1(mov     eax, CR2)
    TryInstruction1(mov     eax, CR3)
    TryInstruction1(mov     CR2, eax)
    TryInstruction1(mov     CR3, eax)
    TryInstruction1(mov     eax, DR0)
    TryInstruction1(mov     eax, DR1)
    TryInstruction1(mov     eax, DR2)
    TryInstruction1(mov     eax, DR3)
    TryInstruction1(mov     DR6, eax)
    TryInstruction1(mov     DR7, eax)
    TryInstruction1(mov     DR0, eax)
    TryInstruction1(mov     DR1, eax)
    TryInstruction1(mov     DR2, eax)
    TryInstruction1(mov     DR3, eax)
    TryInstruction1(mov     DR6, eax)
    TryInstruction1(mov     DR7, eax)
    TryInstruction1(mov     TR4, eax)
    TryInstruction1(mov     TR5, eax)
    TryInstruction1(mov     TR6, eax)
    TryInstruction1(mov     TR7, eax)
    TryInstruction1(mov     eax, TR4)
    TryInstruction1(mov     eax, TR5)
    TryInstruction1(mov     eax, TR6)
    TryInstruction1(mov     eax, TR7)
    TryInstruction1(mov     eax, TR3)
    TryInstruction1(mov     TR3, eax)
    TryInstruction1(out     0, eax)
    TryInstruction(outsb)
    TryInstruction(outsw)
    TryInstruction(outsd)
    TryInstruction(sgdt    fword ptr DeadPtr)
    TryInstruction(sidt    fword ptr DeadPtr)

    // might be OK on x86
    TryInstruction(sldt    DeadPtr)
    TryInstruction(sti)

    // might be OK on x86
    TryInstruction(str     DeadPtr)
    TryInstruction(wbinvd)

}

/*****************************************************************************
 *
 * Name        : LONG ExceptionFilter(LPEXCEPTION_RECORD lpER)
 *
 * Purpose     : filter for exception handler, writess exception information to stdout
 *
 * Args        : LPEXCEPTION_RECORD lpER - pointer to an exception record
 *
 * Returns     : LONG - exception filter code
 *
 * Mod Globals : none
 *
 * Notes       : patterned on Advanced Windows pg. 735
 *               there is some pretty nasty register manipulation in here
 *
  *****************************************************************************/
LONG FPExceptionFilter(EXCEPTION_POINTERS *lpER) {

   /* locals */
   char szBuff[300], *p;
   DWORD dwExceptionCode = lpER->ExceptionRecord->ExceptionCode;

   sprintf(szBuff, "Code = %x, Address = %x\n", dwExceptionCode, lpER->ExceptionRecord->ExceptionAddress);

   /* point to end of string */
   p = strchr(szBuff, 0);

   switch(dwExceptionCode){
      case EXCEPTION_FLT_INVALID_OPERATION:
         sprintf(p, "EXCEPTION_FLT_INVALID_OPERATION\n");
         break;

      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
         sprintf(p, "EXCEPTION_FLT_DIVIDE_BY_ZERO\n");
         break;

      case STATUS_ACCESS_VIOLATION:
         sprintf(p, "STATUS_ACCESS_VIOLATION\n");
         break;

      case STATUS_PRIVILEGED_INSTRUCTION:
         sprintf(p, "STATUS_PRIVILEGED_INSTRUCTION\n");
         break;

      case STATUS_ILLEGAL_INSTRUCTION:
         sprintf(p, "STATUS_ILLEGAL_INSTRUCTION\n");
         break;

      case STATUS_INVALID_LOCK_SEQUENCE:
         sprintf(p, "STATUS_INVALID_LOCK_SEQUENCE\n");
         break;

      default:
         sprintf(p, "Unhandled Exception! Continue search...\n");
         printf("%s\n",szBuff);
         return(EXCEPTION_CONTINUE_SEARCH);
         break;
      }

   printf("%s\n",szBuff);
   return(EXCEPTION_EXECUTE_HANDLER);
}


