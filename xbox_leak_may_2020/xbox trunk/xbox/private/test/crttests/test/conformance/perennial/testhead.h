#ifndef __TESTHEAD_H__
#define __TESTHEAD_H__
/* @(#)File: testhead.h    Version: 1.3.1 C++    Date: 1/20/91 */
/* COPYRIGHT 1984-91, PERENNIAL, All Rights Reserved           */
                        /* testhead.h */
/*===========================================================================
|       This is the include file for all tests developed for the PERENNIAL
| C++ Validation Suite, CVS-CC.  It should be included in any test that 
| uses the support functions in "libd" or "libd.o".
===========================================================================*/

/******************************************************************************
MS changes:
22-Mar-1991 mattheww            comment out overloaded failure() declaration
23-Jul-92   bos                 added changes for Windows testing; see sections
                                  separated by MICROSOFT CHANGES
******************************************************************************/

#include <io.h>
#include <stdio.h>
/************************** MICROSOFT CHANGES ******************************/
/*
** This was moved to be above the section below due to a conflict with the
** macro redefining exit() in wintest.h.
*/

#include <stdlib.h>
/************************** MICROSOFT CHANGES ******************************/
#ifdef _M_IA64
#pragma warning( disable : 4244)
#endif

/************************** MICROSOFT CHANGES ******************************/
#ifndef PEREN_TEST
#ifdef _WINDOWS
#include <wintest.h>
static char test[];
#undef ERROR
#endif
#endif
/************************** MICROSOFT CHANGES ******************************/

#define  PASSED 0
#define  FAILED 1
#define  ERROR -1 
#define  FATAL -2

#define  DFAIL  0
#define  DPASS  1
#define  DTERS  2
#define  DVERB  3
#define  DDEBUG 4
#define PMODE 0644

#include <string.h>
#include <errno.h>
#include <stddef.h>



static int  local_flag;
static FILE *temp;

static void setup();
static void blenter();
static void anyfail();
static int  blexit();

static void failure(char * format);
#if defined(__cplusplus)
static void failure(char * format, int i1, int i2);
#endif
static void fpostcall(float got,float expected,char mesgstr[]);
static void ipostcall(int got,int expected,char mesgstr[]);
static void lpostcall(long got, long expected, char mesgstr[]);
static void spostcall(char got[], char expected[], char mesgstr[]);

#define REL_2_0 0200
#define REL_2_1 0201

#define block0 goto Block0;\
Block0

#define block1 goto Block1;\
Block1

#define block2 goto Block2;\
Block2

#define block3 goto Block3;\
Block3

#define block4 goto Block4;\
Block4

#define block5 goto Block5;\
Block5

#define block6 goto Block6;\
Block6

#define block7 goto Block7;\
Block7

#define block8 goto Block8;\
Block8

#define block9 goto Block9;\
Block9

#define block10 goto Block10;\
Block10

#define block11 goto Block11;\
Block11

#define block12 goto Block12;\
Block12

#define block13 goto Block13;\
Block13

#define block14 goto Block14;\
Block14

#define block15 goto Block15;\
Block15

#define block16 goto Block16;\
Block16

#define block17 goto Block17;\
Block17

#define block18 goto Block18;\
Block18

#define block19 goto Block19;\
Block19

#define block20 goto Block20;\
Block20

#define block21 goto Block21;\
Block21

#define block22 goto Block22;\
Block22

#define block23 goto Block23;\
Block23

#define block24 goto Block24;\
Block24

#ifdef __cplusplus
#if !defined(QA_DISABLE_USING_NAMESPACE)
namespace std {}
using namespace std;
#endif
#endif
#ifdef _XBOX
extern "C" int DbgPrint(const char *, ...);
#endif

#endif // #define __TESTHEAD_H__
