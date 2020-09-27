#ifndef __TSTHD_H__
#define __TSTHD_H__
/* File: tsthd.h    Version: 2.10    Date: 11/16/92
 * CVS-C++, C++ Validation Suite
 * Section: scaffold
 * Copyright 1991-92, PERENNIAL, All Rights Reserved
 */
/*======================================================================

>WHAT:  This is the include file for all tests developed for the PERENNIAL
        C++ Validation Suite, CVS-C++.  It should be #include'd in any 
        test that uses the support functions of scaffold.C.

>NOTE:  None
======================================================================*/
#include <stdio.h>

/************************** MICROSOFT CHANGES ******************************/

#ifdef _M_IA64
#pragma warning( disable : 4244)
#endif

/* Bounds Checking Macro */
/* A = Answer            */
/* B = Upper Bound Limit */
/* C = Lower Bound Limit */
#define INBOUND(A,B,C) ((A) <= (B) && (A) >= (C))

#define  PASSED 0
#define  FAILED 1
#define  ERROR -1 
#define  FATAL -2
#define  SUCCESS 0

#define  DFAIL  0
#define  DPASS  1
#define  DTERS  2
#define  DVERB  3
#define  DDEBUG 4
#define PMODE 0644

#define  ACVS_OPEN_MAX  5
#define  MIN_EXP_LENGTH 2
#define LEN_ERR   1
#define SGN_ERR   2
#define MAN_ERR   3
#define NO_E_ERR  4
#define MAT_E_ERR 5
#define LEN_E_ERR 6
#define EXP_ERR   7
#define LEN_T_ERR 8
#define TXT_ERR   9

static int block_number;
/*
static int blknum;
static int connum;
*/


static FILE *logfp;

static void setup(void);
static void blenter(void);
static void anyfail(void);
static void anyfail(int);
static int  blexit(void);

static void failure(char * format);
static void failure(char * format, int i1);
static void failure(char * format, int i1, int i2);
static void fpost(float got,float expected,char mesgstr[]);
static void ipost(int got,int expected,char mesgstr[]);
static void lpost(long got, long expected, char mesgstr[]);
static void spost(char got[], char expected[], char mesgstr[]);
static void b4abort(void);

static FILE *opnfil( int );
static int  clsrmfil( int );

#define REL_2_0 0200
#define REL_2_1 0201

/*
static void errmesg(char *);
static void debug( void );
static void opost(int , int , char *);
*/

#if defined(_DLL) && defined(_M_MPPC)
static "C" {
static void _TestExit(int);
}
static void crt_test_terminate();

#define CreateExitFile() _TestExit(gloflg == PASSED ? 3 : 1)
#else
#define CreateExitFile()
#endif
#ifdef _XBOX
extern "C" int DbgPrint(const char *, ...);
#endif

#endif // #define __TSTHD_H__
