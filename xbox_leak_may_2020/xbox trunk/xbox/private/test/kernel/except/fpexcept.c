/*++

Copyright (c) 1995 Microsoft Corporation

Module Name:

        fpexcept.c

Abstract:

        test floating point exceptions

Author:

        johndaly

Environment:

        x86-only - used to test emulators (16 and 32 bit)

Notes:

        This is for x86 only, it is an instruction-level test designed to make
        sure SEH works correctly with 486 emulators.  All test output to stdout.

Revision History:

        10/9/95 - creation
        9/25/2000 - use in xbox (wow, 5 yeras later...! add more tests.)

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
#include <xlog.h>
#include <xtestlib.h>

#if 0
int _fltused = 1;
#endif

/* function declarations */
void __cdecl fpexcept_main(void);
void InvOp(void);
void ZeroDiv(void);
void DenormalOp(void);
void NumOverflow(void);
void NumUnderflow(void);
void StackEx(void);
void Inexact(void);
LONG FPExceptionFilter(EXCEPTION_POINTERS *);

/* global data */
WORD wControlWord = 0;      /* used by ResetEnableExcept macro */
HANDLE FpExceptLogHandle;   /* logging handle */

//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The 
// default is only one thread.
//

LONG FPExceptThreadID = 0;

/* macros */
/* reset the fpu and enable the indicated exceptions */
#define ResetEnableExcept(ExceptionMask)    \
   _asm   fninit                            \
   _asm   FSTCW  wControlWord               \
   _asm   xor wControlWord,##ExceptionMask  \
   _asm   fldcw wControlWord

/*****************************************************************************
 *
 * Name        : fpexcept_main
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
void __cdecl fpexcept_main(void)   {

   __try {
      /* create exceptions */
      InvOp();
      ZeroDiv();
      DenormalOp();
      NumOverflow();
      NumUnderflow();
      StackEx();
      Inexact();
      }
   __except(FPExceptionFilter(GetExceptionInformation())){}

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
#pragma warning(disable: 4409)
void InvOp(void)  {
   /* locals */
   float a = (float)1.22e1;
   float b = (float)0.0;
   double e = 1.25e40;
   double f = 0.0;
   int c = 0;
   int d = 1;
   long g = 0;
   long h = 1;
   double trash;
   char FakeTenbyteBCD[] = {'1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9'};

   xSetFunctionName( FpExceptLogHandle, "fpexcept:invop" );
   xStartVariation( FpExceptLogHandle, "InvOp" );

   /* enable invalid-operation exceptions */
   /* stack errors - overflow */
   /* log action */

   xLog( FpExceptLogHandle, XLL_INFO, "testing invalid operation exceptions - overflow the stack");
   ResetEnableExcept(0x00000001)
   _asm  {
      // fld1
      fld1
      fld1
      fld1
      fld1
      fld1
      fld1
      fld1
      fld1
      fld1
      fwait

      // fbld
      fbld FakeTenbyteBCD
      fwait

      fnop
      }

   /* enable invalid-operation exceptions */
   /* invalid ops */
   /* log action */
   xLog( FpExceptLogHandle, XLL_INFO, "testing invalid operation exceptions - operations with invalid operands");
   ResetEnableExcept(0x00000001)
   xLog( FpExceptLogHandle, XLL_INFO, "f2xm1");
   _asm  {
      f2xm1
      fwait
      }

   xLog( FpExceptLogHandle, XLL_INFO, "fabs");
   _asm  {
      fabs
      fwait

      fadd a
      fwait

      fadd e
      fwait
      }

   xLog( FpExceptLogHandle, XLL_INFO, "fadd");
   _asm  {
      fadd st,st(1)
      fwait

      fadd st(1),st
      fwait

      fadd st,st(7)
      fwait

      fadd st(7),st
      fwait

      faddp st(1),st
      fwait

      faddp st(7),st
      fwait

      fadd
      fwait

      fiadd d
      fwait

      fiadd h
      fwait
      }

   xLog( FpExceptLogHandle, XLL_INFO, "fbstp");
   _asm  {
      fbstp FakeTenbyteBCD
      fwait
      }

   xLog( FpExceptLogHandle, XLL_INFO, "fchs");
   _asm  {
      fchs
      fwait
      }

   xLog( FpExceptLogHandle, XLL_INFO, "fcom");
   _asm  {
      fcom a
      fwait

      fcom e
      fwait

      fcom st(1)
      fwait

      fcom st(7)
      fwait

      fcom
      fwait

      fcomp a
      fwait

      fcomp e
      fwait

      fcomp st(1)
      fwait

      fcomp st(7)
      fwait

      fcomp
      fwait

      fcompp
      fwait

      fnop
      }

   ResetEnableExcept(0x00000001)
   xLog( FpExceptLogHandle, XLL_INFO, "fprem");
   _asm  {
    fldz
    fld1
    fprem
    fwait
    }

   ResetEnableExcept(0x00000001)
   xLog( FpExceptLogHandle, XLL_INFO, "fly2x");
   _asm  {
    fldz
    fldz
    fyl2x
    fwait

    fldz
    fchs
    fldz
    fyl2x
    fwait

    fldz
    fldz
    fchs
    fyl2x
    fwait

    fldz
    fchs
    fldz
    fchs
    fyl2x
    fwait
    }

   xEndVariation( FpExceptLogHandle );
}
#pragma warning(default: 4409)

/*****************************************************************************
 *
 * Name        : ZeroDiv
 *
 * Purpose     : create divide by 0 exceptions
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
void ZeroDiv(void)  {

   /* locals */
   float a = (float)1.22e1;
   float b = (float)0.0;
   double e = 1.25e40;
   double f = 0.0;
   int c = 0;
   int d = 1;
   long g = 0;
   long h = 1;
   double trash;

   /* create exceptions */
   /* remember to follow all exceptions with fwait */
   /* which will cause the processor to check */
   
   xSetFunctionName( FpExceptLogHandle, "fpexcept:zerodiv" );
   xStartVariation( FpExceptLogHandle, "ZeroDiv" );

   /* enable divide by 0 exceptions */
   /* log action */
   xLog( FpExceptLogHandle, XLL_INFO, "testing divide-by-0 exceptions - fdiv variations");
   /* divide by 0 using division */
   ResetEnableExcept(0x00000004)
   _asm  {
      fld a
      fdiv b
      fwait

      fld e
      fdiv f
      fwait

      fldz
      fld e
      fdiv st, st(1)
      fwait

      fld e
      fldz
      fdiv st(1), st
      fwait

      fstp trash
      fstp trash
      fstp trash
      fstp trash

      fld e
      fldz
      fdivp st(1), st
      fwait

      fld e
      fldz
      fdiv
      fwait

      fild d
      fidiv c
      fwait

      fidiv g
      fwait
      fnop
      }

   /* enable divide by 0 exceptions */
   /* divide by 0 using reverse division */
   /* log action */
   xLog( FpExceptLogHandle, XLL_INFO, "testing divide-by-0 exceptions - fdivr variations");
   ResetEnableExcept(0x00000004)
   _asm  {
      fld b
      fdivr a
      fwait

      fld f
      fdivr e
      fwait

      fld e
      fldz
      fdivr st, st(1)
      fwait

      fldz
      fld e
      fdivr st(1), st
      fwait

      fstp trash
      fstp trash
      fstp trash
      fstp trash

      fldz
      fld e
      fdivrp st(1), st
      fwait

      fldz
      fld e
      fdivr
      fwait

      fild c
      fidivr d
      fwait

      fild g
      fidivr h
      fwait
      fnop
      }

   /* enable divide by 0 exceptions */
   /* divide by 0 fxtract division */
   /* log action */
   xLog( FpExceptLogHandle, XLL_INFO, "testing divide-by-0 exceptions - fxtract variations");
   ResetEnableExcept(0x00000004)
   _asm  {
      fldz
      fxtract
      fwait
      fnop
      }

   /* enable divide by 0 exceptions */
   /* divide by 0 fyl2x division */
   /* log action */
   xLog( FpExceptLogHandle, XLL_INFO, "testing divide-by-0 exceptions - fyl2x variations");
   ResetEnableExcept(0x00000004)
   _asm  {
      fld1
      fldz
      fyl2x
      fwait
      fnop
      }

   xEndVariation( FpExceptLogHandle );
return;

}

/*****************************************************************************
 *
 * Name        : DenormalOp
 *
 * Purpose     : create denormal operand exceptions
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
void DenormalOp(void)  {
   /* locals */

}

/*****************************************************************************
 *
 * Name        : NumOverflow
 *
 * Purpose     : create numeric overflow exceptions
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
void NumOverflow(void)  {
   /* locals */

}

/*****************************************************************************
 *
 * Name        : NumUnderflow
 *
 * Purpose     : create numeric overflow exceptions
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
void NumUnderflow(void)  {
   /* locals */

}

/*****************************************************************************
 *
 * Name        : StackEx
 *
 * Purpose     : create numeric stack exceptions
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
void StackEx(void)  {
   /* locals */

}

/*****************************************************************************
 *
 * Name        : Inexact
 *
 * Purpose     : create inexact exceptions
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
void Inexact(void)  {
   /* locals */

}

/*****************************************************************************
 *
 * Name        : LONG FPExceptionFilter(LPEXCEPTION_RECORD lpER)
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

   sprintf(szBuff, "Code = %x, Address = %x", dwExceptionCode, lpER->ExceptionRecord->ExceptionAddress);

   /* point to end of string */
   p = strchr(szBuff, 0);

   switch(dwExceptionCode){
      case EXCEPTION_FLT_INVALID_OPERATION:
         sprintf(p, "EXCEPTION_FLT_INVALID_OPERATION ");
         /* munge context record */
         lpER->ContextRecord->FloatSave.StatusWord &= 0xFF7E;
         break;

      case EXCEPTION_FLT_DENORMAL_OPERAND:
         sprintf(p, "EXCEPTION_FLT_DENORMAL_OPERAND ");
         /* munge context record */
         lpER->ContextRecord->FloatSave.StatusWord &= 0xFF7D;
         break;

      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
         sprintf(p, "EXCEPTION_FLT_DIVIDE_BY_ZERO ");
         /* munge context record */
         lpER->ContextRecord->FloatSave.StatusWord &= 0xFF7B;
         break;

      case EXCEPTION_FLT_OVERFLOW:
         sprintf(p, "EXCEPTION_FLT_OVERFLOW ");
         /* munge context record */
         lpER->ContextRecord->FloatSave.StatusWord &= 0xFF77;
         break;

      case EXCEPTION_FLT_UNDERFLOW:
         sprintf(p, "EXCEPTION_FLT_UNDERFLOW ");
         /* munge context record */
         lpER->ContextRecord->FloatSave.StatusWord &= 0xFF6F;
         break;

      case EXCEPTION_FLT_INEXACT_RESULT:
         sprintf(p, "EXCEPTION_FLT_INEXACT_RESULT ");
         /* munge context record */
         lpER->ContextRecord->FloatSave.StatusWord &= 0xFF5F;
         break;

      case EXCEPTION_FLT_STACK_CHECK:
         sprintf(p, "EXCEPTION_FLT_STACK_CHECK ");
         /* munge context record */
         lpER->ContextRecord->FloatSave.StatusWord &= 0xFF3E;
         break;

      default:
         sprintf(p, "Unhandled Exception! Continue search...\n");
         xLog(FpExceptLogHandle, XLL_FAIL, "%s", szBuff);
         return(EXCEPTION_CONTINUE_SEARCH);
      }

   xLog(FpExceptLogHandle, XLL_PASS, "%s", szBuff);
   return(EXCEPTION_CONTINUE_EXECUTION);
}

VOID
WINAPI
FPExceptStartTest(
    HANDLE xLogHandle
    )

/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    FpExceptLogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/

{
    LONG ID;
    FpExceptLogHandle = xLogHandle;

    OutputDebugString( TEXT("fpexcept: StartTest is called\n") );

    //
    // One way to run multiple threads without creating threads is to
    // set NumberOfThreads=n under the section [sample.dll]. Everytime
    // the harness calls StartTest, FPExceptThreadID will get increment by one
    // after InterlockedIncrement call below. If no NumberOfThreads
    // specified, only RunTest1 will be run
    // 

    ID = InterlockedIncrement( &FPExceptThreadID );

    xSetComponent( FpExceptLogHandle, "kernel", "fpexcept" );

    //
    // call the tests
    //

    fpexcept_main();

    return;

}

VOID
WINAPI
FPExceptEndTest(
    VOID
    )

/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/

{
    FPExceptThreadID = 0;
    OutputDebugString( TEXT("timetest: EndTest is called\n") );
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( fpexcept )
#pragma data_seg()

BEGIN_EXPORT_TABLE( fpexcept )
    EXPORT_TABLE_ENTRY( "StartTest", FPExceptStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", FPExceptEndTest )
END_EXPORT_TABLE( fpexcept )
