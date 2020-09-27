/* File: scaffold.C    Version: 2.9    Date: 11/12/92
 * CVS-C++, C++ Validation Suite
 * Section: scaffold
 * Copyright 1985-92, PERENNIAL, All Rights Reserved
 */
/*======================================================================
        
>WHAT:  Included in this file are the support functions used in generating
        test programs for the PERENNIAL C++ Validation Suite:

                blenter()       enter a block in a test program
                blexit()        exit a block in a test program
                failure(reason) set failure condition for current block
                                   and display diagnostic message ``reason''
                failure(reason, int)    an alternate format
                failure(reason, int, int)       an alternate format
                setup()         begin a test program
                anyfail()       end a test program
                errmesg()       exit test program with diagnostic
                pend()          controls various output streams
                debug()         useful during developement
                ipost()         post an integer result in standard format
                fpost()         post a floating point result in standard format
                spost()         post a string result in standard format
                lpost()         post a long result in standard format

>NOTE:  None.

======================================================================
MSC CHANGES:
    28-Jun-93   bruceku added comments with MICROSOFT CHANGES around
                          modified lines

======================================================================*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#if defined(_DEBUG)
#include <crtdbg.h>
#endif 

#include "tsthd.h"

/*************************** MICROSOFT CHANGES *****************************/
#include <eh.h>

#ifndef PEREN_TEST
#ifdef _WINDOWS
static char test[] = "CVSEH";
#endif
#endif
/*************************** MICROSOFT CHANGES *****************************/

#define  PASSED 0
#define  FAILED 1
#define  ERROR -1 
#define  FATAL -2
#define  TRUE   1
#define  FALSE  0

#define  DFAIL  0
#define  DPASS  1
#define  DTERS  2
#define  DVERB  3
#define  DDEBUG 4

#define PMODE 0644
#define  WRITEPLUS     "w+"
#define  ACVS_MAX_OPEN 5

#if TERS                         
#define COMPILE_FLAG  DTERS
#endif

#if DEBUG
#define COMPILE_FLAG  DDEBUG
#endif

#if VERB
#define COMPILE_FLAG  DVERB
#endif

#if PASS 
#define COMPILE_FLAG  DPASS
#endif

#if FAIL
#define COMPILE_FLAG  DFAIL
#endif

#ifndef COMPILE_FLAG
#define COMPILE_FLAG  DTERS      
#endif


#ifdef XCOMPILE
#define NO_ERR_SUITE
#endif

/*-------------------------------------*
 *                                     *
 * block tracking states               *
 *                                     *
 *-------------------------------------*/

#define START 0
#define OUTBLK 1
#define INBLK 2
#define END 3
static int state=START;


static int output_flag = COMPILE_FLAG;

static int locflg;             /* This flag is used in blocks to track failure. */


static int condition_number;   /* This is a number used to identify the 
                           exact points of failure */
static int gloflg;             /* Pass/Fail flag for the entire program */

static char * getresdir();

/*************************** MICROSOFT CHANGES *****************************/
#if defined(PEREN_TEST)
#else
#endif

#if defined(_DLL) && defined(_M_MPPC)
static "C" {
static void _TestExit(int);
}
#endif 

static void crt_test_terminate()
{
//  DbgPrint("hello\n");    //! remove after testing.
  CreateExitFile();
  abort();
}

/*************************** MICROSOFT CHANGES *****************************/
 
static char testname[30];
static char logxname[30];

static FILE *Filefp[ACVS_MAX_OPEN];  /* An array of pointers to temp streams.*/
static char FileName[ACVS_MAX_OPEN][L_tmpnam]; /* To hold the names of temp files.*/

static void errmesg(char *);

static void blenter(){
        if (state != OUTBLK)
                switch (state) {
                case INBLK:     errmesg("Two successive calls to blenter().");
                        break;
                case START:     errmesg("The test did not call setup().");
                        break;
                case END:       errmesg("blenter() is called after anyfail().");
                        break;
                default:errmesg("State machine in unknown state.");
                }
        state=INBLK;

        block_number++;
        fprintf(logfp,"\tEnter Block #%d \n",block_number);  
        fflush(logfp); 
        fflush(stderr); 
        locflg = PASSED;
        condition_number = 0;
}

blexit(){
        if (state != INBLK)
                switch (state) {
                case OUTBLK:    errmesg("Two successive calls to blexit().");
                        break;
                case START:     errmesg("The test did not call setup().");
                        break;
                case END:       errmesg("blexit() is called after anyfail().");
                        break;
                default:errmesg("State machine in unknown state.");
                }
        state=OUTBLK;

        fprintf(logfp,"\tExit Block #%d ",block_number); 
        switch(locflg) {
        case PASSED: {  fprintf(logfp,"passed\n\n"); 
                        fflush(logfp);
                        return PASSED;
                        }
        case FAILED: {  fprintf(logfp,"FAIL\n\n");
                        fflush(logfp);
                        return gloflg = FAILED;
                        }
        default: {      fprintf(logfp,"Unknown block result");
                        fflush(logfp);
                        return gloflg = ERROR;
                        }
        }
}


static void setup()
{

/*
 * This section opens all the report files in the Results directory
 * Also resets block_number and gloflg
 */

        int i = 0;
        int flag;
        char c, name[20];   

        if (state != START)
                errmesg("setup() was called more than once");
        state=OUTBLK;

        block_number = -1 ;

        gloflg = PASSED ;

        c = prgnam[0];
        flag = TRUE;

        while(flag)
                if((c != '\0') && (c != '.') && (c != '(')) {
                        name[i] = c;
                        i++;
                        c = prgnam[i];
                }
                else flag = FALSE;
        name[i] = '\0';

        strcpy(testname, name);
        strcat(testname, ".c");
        strcpy(logxname, getresdir());

        strcat(logxname, name);
        strcat(logxname, ".log");

/*************************** MICROSOFT CHANGES *****************************/
#if defined(_DEBUG)
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
#endif 

#if defined(PEREN_TEST)
        if ((logfp = fopen(logxname, "w")) == NULL){
                fprintf(stderr,"%s : setup: can`t open log file\n",
                        testname);
                return; /* exit(-4); */
        }
        fprintf(logfp,"===%s\n",prgnam);
        fflush(logfp);
#else
        setvbuf( logfp, NULL, _IONBF, 0 );
#endif

#if defined(_DLL) && !defined(_M_MPPC)
        set_terminate (crt_test_terminate);
#endif        
/*************************** MICROSOFT CHANGES *****************************/

}

static void debug()
{
        fprintf(logfp,"Debug function:\n");
        fprintf(logfp,"name %s \n",prgnam);
        fprintf(logfp,"test %s \n",testname);
        fprintf(logfp,"block_number %d \n",block_number);
        fprintf(logfp,"gloflg %d (test result)\n",gloflg);
        fprintf(logfp,"locflg %d (block result)\n",locflg);
        fprintf(logfp,"state %d (block tracking state machine)\n",state);
        fprintf(logfp,"condition_number %d \n",condition_number);
        fprintf(stderr,"Debug function:\n");
        fprintf(stderr,"name %s \n",prgnam);
        fprintf(stderr,"test %s \n",testname);
        fprintf(stderr,"block_number %d \n",block_number);
        fprintf(stderr,"gloflg %d (test result)\n",gloflg);
        fprintf(stderr,"locflg %d (block result)\n",locflg);
        fprintf(stderr,"state %d (block tracking state machine)\n",state);
        fprintf(stderr,"condition_number %d \n",condition_number);
}

static void 
/*************************** MICROSOFT CHANGES *****************************/
errmesg(char *message)                          /* was char message[] */
/*************************** MICROSOFT CHANGES *****************************/
{
        fprintf(logfp,"------------%s *** Internal error: %s\n",
                prgnam, message);
        fprintf(stderr,"------------%s *** Internal error: %s\n",
                prgnam, message);
#ifdef DEBUG
        debug();
#endif
        fflush(logfp) ;
        fflush(stderr) ;
        return; /* exit(ERROR); */ 
}


static pend()
{

        switch(output_flag) {
                case DFAIL: 
                case DPASS:
                case DTERS:  
                case DVERB: 
                case DDEBUG:
                        break;
                default: 
                        output_flag = FATAL;
        }

        fclose(logfp);
        return output_flag;
}

static void anyfail(){
        if (state != OUTBLK)
                switch (state) {
                case START:     errmesg("setup() was never called.");
                        break;
                case INBLK:     errmesg("anyfail() was called before blexit().");
                        break;
                case END:       errmesg("Two successive calls to anyfail().");
                        break;
                default:errmesg("State machine in unknown state.");
                }
        state=END;
        if (block_number < 0)
                errmesg("blenter() was never called.");

        switch(gloflg){
                case PASSED: { fprintf(logfp,"++++++++++++%s Passed\n",
                                        prgnam);
                               pend();
                               return; /* exit(PASSED); */
                             }
                        
                case FAILED: { fprintf(logfp,"------------%s ******FAILED******\n",
                                        prgnam);
                               pend();
                               return; /* exit(FAILED); */
                             }
                case ERROR:  { fprintf(logfp,"***Internal Error");
                               fprintf(stderr,"***Internal Error"); 
                               pend();
                               return; /* exit(ERROR); */
                             }
                default:     { fprintf(stderr,"Fatal Error in %s",testname);
                               fprintf(logfp,"Fatal Error in %s",testname);
                               pend();
                               return; /* exit(32767); */
                             }
                }
}

static void 
anyfail(int expected_blocks)
{
        char msg[80];   /* buffer used to build messages for errmesg() */

        if (expected_blocks != (block_number+1)){
                sprintf(msg, "Expected %d blocks, found %d\n", 
                        expected_blocks, block_number+1);
                errmesg(msg);
                }
        anyfail();
}

static void 
ipost(int got, int expected, char mesgstr[])
{
        fprintf(logfp,"\t\tgot  %d expected  %d %s \n",got ,expected, mesgstr);
        fflush(logfp);
}

static void 
opost(int got, int expected, char mesgstr[])
{
        fprintf(logfp,"\t\tgot  %o expected  %o %s \n",got ,expected, mesgstr);
        fflush(logfp);
}

static void 
fpost(float got, float expected, char mesgstr[])
{
        fprintf(logfp,"\t\tgot  %f expected  %f %s \n",got ,expected, mesgstr);
        fflush(logfp);
}

static void 
spost(char got[], char expected[], char mesgstr[])
{
        fprintf(logfp,"\t\tgot  :%s: expected  :%s: %s \n",
                got ,expected, mesgstr);
        fflush(logfp);
}

static void 
lpost(long got, long expected, char mesgstr[])
{
        fprintf(logfp,"\t\tgot %d expected  %d %s\n",
                got,expected,mesgstr );
        fflush(logfp);
}

/* #ifndef NO_ERR_SUITE */
#if 0

#define DEBUG 1
#define CC_OUT_SZ 2046
#define EXPECT_SZ 254

static char got[CC_OUT_SZ];                    /* CC stderr */
static char expect[EXPECT_SZ];                 /* expected error match substring */

/* 
 * get_output() --- Get output from file and put in buf.
 *
 *      Returns:        0 if ok, else 1.
 */
static int get_output(
        char * file,                    /* file name */
        char *buf,                      /* target buffer */
        int size                        /* max size */
        )
{
        FILE * fp;
        int r;

        fp = fopen (file, "r");
        if (fp == NULL) {
                fprintf(logfp, "Can't open %s. errno=%d\n", file, errno);
                return 1;
        }

        r = fread (buf, sizeof(*buf), size, fp);
        if (r < 0) {
                fprintf(logfp, "Can't read %s. errno=%d\n", file, errno);
                return 1;
        }
        /* DbgPrint("buf=\n%s\n", buf); */

        fclose (fp);
        return 0;
}

static int lc_get_output()
{
        if (get_output ("tmp.out", (char*) &got, CC_OUT_SZ)) {
                fprintf(logfp, "Could not obtain results of compile.\n");
                locflg = FAILED;
        }
        else if (get_output ("exp.out", (char*) &expect, EXPECT_SZ)) {
                fprintf(logfp, 
                  "Could not obtain expectations of compile.\n");
                locflg = FAILED;
        }
        return (locflg == FAILED);
}

static void compile_it(char *what)
{
        int r;

        char command[1022];

        sprintf (command, "%s %s", "./doCC.sh", what);

        /* Compile test only. */

        /* r = system (command); */

        r = (r & 0xff00) >> 8 ;

        switch (r) {
        case 0:         /* OK */
                break;
        case 1:
                fprintf(logfp, "Missing err_data file. UNTESTED.\n");
                locflg = FAILED;
                break;
        case 2:
                fprintf(logfp, "Missing err_path file. UNTESTED.\n");
                locflg = FAILED;
                break;
        case 3:
                fprintf(logfp, "Incomplete err_data file. UNTESTED.\n");
                locflg = FAILED;
                break;
        case 4:
                fprintf(logfp, "Incomplete err_path file. UNTESTED.\n");
                locflg = FAILED;
                break;
        case 5:
                fprintf(logfp, "Test case file nonexistent. UNTESTED.\n");
                locflg = FAILED;
                break;
        case 6:
                fprintf(logfp, "Exit status of CC = 0. Expected non-zero.\n");
                locflg = FAILED;
                break;
        case 7:
                if (lc_get_output())    /* Can't get CC output? */
                        break;          /* Failure */

                fprintf(logfp, 
                  "Expected error message keywords: %s\nGot: %s\n", 
                        expect, got); /* But not a failure. */
                break;
        case 8:
                fprintf(logfp, "No error or warning message.\n");
                locflg = FAILED;
                break;

        case 9:
                if (lc_get_output())    /* Can't get CC output? */
                        break;          /* Failure */

                fprintf(logfp, 
                  "Unexpected error (should be implemented).\nGot: %s\n", 
                        got); /* But not a failure. */
                break;

        case 10:
                if (lc_get_output())    /* Can't get CC output? */
                        break;          /* Failure */

                fprintf(logfp, 
                 "Unexpected non-zero status (should be implemented).\n");
                /* But not a failure. */
                break;

        default:
                fprintf(logfp, "Unexpected result. Exit status=%d\n", r);
                if (r < 0) fprintf(logfp, "/t errno=%d.", errno);
                locflg = FAILED;
                break;
        }

        if (!DEBUG) {
                remove("tmp.out");
                remove("exp.out");
        }
}

#endif /* NO_ERR_SUITE */

/*
 * failure()
 *
 * As a convenience and shorthand for the test programmer, the function
 * failure(str) will set locflg to FAILED and display a diagnostic that
 * is intended to be the reason that the block failed. 
 *
 * As a convenience, if the message is not terminated by a newline '\n' the
 * routine will output one. To do this it first writes the string to a
 * buffer and then prints the buffer.
 */
static void failure(char * format)
{
        fprintf(logfp, "%s : %s", prgnam, format);
        if (format[strlen(format) - 1] != '\n')
                fprintf(logfp, "\n");
        locflg = FAILED;
}

static void failure(char * format, int i1)
{
        char buf[256];
        sprintf(buf, "%s : %s", prgnam, format);
        fprintf(logfp, buf, i1);
        if (format[strlen(format) - 1] != '\n')
                fprintf(logfp, "\n");
        locflg = FAILED;
}

static void failure(char * format, int i1, int i2)
{
        char buf[256];
        sprintf(buf, "%s : %s", prgnam, format);
        fprintf(logfp, buf, i1, i2);
        if (format[strlen(format) - 1] != '\n')
                fprintf(logfp, "\n");
        locflg = FAILED;
}


#ifdef XCOMPILE
static char default_dir[] = "CROSS/Results/";
#else
static char default_dir[] = "D:\\";
#endif

static char *
getresdir()
{
        static char dirname[256];
        FILE * fp;

        fp = fopen("rslt_dir", "r");

        if (fp == NULL || fscanf(fp, "%s", dirname) == 0)
            strcpy(dirname, default_dir);

        return dirname;
}

/*
 * b4abort()
 *
 * This function is provided for use right before the point of a type A
 * test where a call to abort() is expected. 
 *
 * The driver passes a type A test if the test exits with a call to abort()
 * and if the last line in the log file has the string ABORT_EXPECTED_NOW.
 * This function writes ABORT_EXPECTED_NOW to the log file and flushes the
 * log file and stderr pointers.
 */
static void b4abort(void)
{
        fprintf(logfp, "%s : ABORT_EXPECTED_NOW\n", prgnam);
        fflush(logfp); 
        fflush(stderr); 
}

/* added by HK to accommodate ACVS tests */

/*----------------------------------------------------------------*\
 * FUNCTION: opnfil()                                             *
 * ARGUMENTS:   n - An integer holding the index to the desired   *
 *                  file name and file pointer to close and       * 
 *                  and remove.                                   * 
 *                  n must be less than ACVS_MAX_OPEN.            *
 * DESCRIPTION: A support utility that is used to create a file   *
 *              with a call to the function tmpnam() to get a     *
 *              valid file name.  The file name and file pointer  *
 *              are stored in the global variables FileName[n]    *
 *              and Filefp[n].                                    *
 *              A total of ACVS_MAX_OPEN can be open at the same  *
 *              time.                                             *
 *              The support function "clsrmfil()" must be used to *
 *              close and remove the temporary file.              *
 * RETURNS: A valid FILE * that can be used to read or write the  *
 *          stream.                                               *
 *          NULL, if the file could not be opened, or an existing *
 *          file was open.                                        *
\*----------------------------------------------------------------*/
static FILE * opnfil( int n )
{

   if( n >= ACVS_MAX_OPEN) 
      return(NULL);
   else if(Filefp[n] != NULL)
      return(NULL);

   if((tmpnam(FileName[n])) != FileName[n])
                return(NULL);

   if((Filefp[n] = fopen(FileName[n], WRITEPLUS)) == NULL)
      (void)fprintf(logfp,"%s: Could not open %s\n", prgnam, FileName[n]);
       
   return(Filefp[n]); 
}

/*----------------------------------------------------------------*\
 * FUNCTION: clsrmfil()                                           *
 * ARGUMENTS:   n - An integer holding the index to the desired   *
 *                  file name and file pointer to close and       *
 *                  and remove.                                   *
 *                  n must be less than ACVS_MAX_OPEN.            *
 * DESCRIPTION: A support function that is used to close and      *
 *              remove a file.                                    *
 * RETURNS:  0 - Success                                          *
 *           nonzero - Failure                                    *
\*----------------------------------------------------------------*/
static int clsrmfil( int n )
{
   static int ReturnVal = 1;

   if ( n < ACVS_MAX_OPEN ) {
      if(Filefp[n] != NULL) {
         if((ReturnVal = fclose(Filefp[n])) == 0) {
            Filefp[n] = NULL;
            ReturnVal = remove(FileName[n]); 
         }
      }
   }

   return(ReturnVal);
}					   
