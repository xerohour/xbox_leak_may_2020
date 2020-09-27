/*=========================================================================*/
/* File  another.C    Version: 2.0.2    Date: 1/20/91                      */
/* Copyright 1985 - 1991, Perennial, All Rights Reserved                   */
/*                              another.C                                  */
/*=========================================================================*/
/*
	Included in this file are the support functions used in generating
test programs for the PERENNIAL C++ Validation Suite:

		blenter()	enter a block in a test program
		blexit()	exit a block in a test program
		failure(reason)	set failure condition for current block
				   and display diagnostic message ``reason''
		failure(reason, int, int)	an alternate format
		separate()	begin a test program
		anyfail()	end a test program
		errmesg()	exit test program with diagnostic
		pend()		controls various output streams
		debug()		useful during developement
		ipostcall()	post an integer result in standard format
		fpostcall() 	post a floating point result in standard format
		spostcall()	post a string result in standard format
		lpostcall()	post a long result in standard format


	MODIFIED: Dec 22, 1989 by Paul Stone
		Added compile_it() and get_output().

	MODIFIED: Jun 25, 1990 by Daniel Edelson
		Added failure() function.
		Removed two comment-ed out #include's
		Included <iostream.h> instead of <stream.h>
	
	MODIFIED: Aug 1, 1990 by Daniel Edelson
		Removed some commented-out, unused code or redundant code. 
		Removed commented-out function issu().
	
	MODIFIED: Aug 7, 1990 by Daniel Edelson
		Added stdarg (varargs) support to failure().
	
	MODIFIED: Aug 16, 1990 by Daniel Edelson
		Removed all references to the useless ``buffname''
		variable including erroneous references to the literal
		file ``buffname''. Included <stdlib.h> rather than <libc.h>.
		Included <errno.h> and removed extern int errno declaration. 
		Recommend routine get_output() be recoded to use library
		I/O rather than system call I/O.
	
	MODIFIED: Aug 17, 1990 by Daniel Edelson
		Recoded routine pend() to be more efficient: to avoid 
		unnecessary (useless) calls to system() and to use library
		I/O rather than sytem("cat filename") to display the
		log file. Modified function compile_it() to use 
		the remove() to delete files rather than 
		system("rm file..."). Systems that don't support the ANSI
		routine remove(char *) should define remove to be unlink.
		Bracketed the call to getenv() with conditional compilation
		based on XCOMPILE so that it can be removed for embedded
		systems whose runtime environment does not support an
		environment. Added conditional compilation around the
		error_suite functionality so that it can be removed
		when it is not desired. This is required for systems that
		lack system() in the runtime library.
		   *** Recommend get_output() be recoded ***
		   *** to use ANSI Library I/O rather    ***
		   *** than system call raw I/O.         ***

	MODIFIED: Sep 10, 1990 by Daniel Edelson
		Caused the results directory to be obtained from 
		a file named ``rslt_dir'' rather than from the environment.
		This is because free-standing ANSI C systems are not
		required to provide getenv(). Recoded some routines to
		use <stdio> library i/o rather than raw system call I/O.
	
	MODIFIED: Oct 11, 1990 by Daniel Edelson
		Changed failure(char * ...) to failure(char *, ...) to conform
		to ANSI C. The file now compiles under either ANSI C or C++.
		Removed the printfs from pend() to make the bahavior the same
		as that of the ANSI C scaffold. Removed the function cat() 
		because with pend() gutted it's no longer used.
    
	MODIFIED: Oct 25, 1990 by Paul Stone
		Removed references to tmp2.out file (err_msg suite).
	
	MODIFIED: Jan 20, 1991 by Daniel Edelson, Version 2.0.2
		Changed failure() so that it ONLY takes a (char *) argument.
		The old version invoked vsprintf() which non-ANSI-C
		platforms need not have. Also provided an overloaded
		function that takes a char * and two ints because
		a couple of testcases expect that.


Copyright 1984-91, PERENNIAL INC., ALL RIGHTS RESERVED
=============================================================================*/

/******************************************************************************
MS changes:
20-Mar-1991 mattheww		temp=stdout
22-Mar-1991 mattheww            comment out overloaded failure() definition
27-Jul-1992 bos                 added changes for Windows testing;  see sections
                                  separated by MICROSOFT CHANGES
******************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "testhead.h"

/************************** MICROSOFT CHANGES ******************************/
#ifndef PEREN_TEST
#ifdef _WINDOWS
static char test[] = "CVSCC";
#endif
#endif
/************************** MICROSOFT CHANGES ******************************/



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
#define	NO_ERR_SUITE
#endif



static int output_flag = COMPILE_FLAG;


static int block_number;	/* This identifies the current block */

static int condition_number;	/* This is a number used to identify the 
				  exact points of failure */
static int global_flag = PASSED;	/* Pass/Fail flag for the entire program */


static char * getresdir();


static char testname[30];
static char logxname[30];

static void blenter(){
	 DbgPrint("\tEnter Block #%d \n",block_number);  
	 /* fflush(temp) */ 
	 fflush(stderr); 
	 local_flag = PASSED;
	 condition_number = 0;
}



blexit(){
	char * INTERNAL = "*** Internal error : exiting block %d \n\n";

	DbgPrint("\tExit Block #%d ",block_number++); 
	switch(local_flag) {
	case PASSED: {  DbgPrint("passed\n\n"); 
	 		/* fflush(temp) */ 
		        return PASSED;
		     }
	case FAILED: {  DbgPrint("FAIL\n\n");
	 		/* fflush(temp) */
		        return global_flag = FAILED;
		     }
	default:     {  DbgPrint( INTERNAL, block_number);
	 		/* fflush(temp) */
			return global_flag = ERROR;
		     }
	}
}


static void separate()
{

/*
 * This section opens all the report files in the Results directory
 * Also resets block_number and global_flag
 */

	int i = 0;
	int flag;
	char c, name[20];   

	block_number = 0 ;
	global_flag = PASSED ;

	c = progname[0];
	flag = TRUE;

	while(flag)
		if((c != '\0') && (c != '.') && (c != '(')) {
			name[i] = c;
			i++;
			c = progname[i];
		}
		else flag = FALSE;
	name[i] = '\0';

	strcpy(testname, name);
	strcat(testname, ".c");
	strcpy(logxname, getresdir());

	strcat(logxname, name);
	strcat(logxname, ".log");
#if 0
	if ((temp = fopen(logxname, "w")) == NULL){
		DbgPrint("%s : separate: can`t open log file\n",
			testname);
		return; /* exit(-4); */
	}
#endif
	temp=stdout;
	DbgPrint("===%s\n",progname);
	/* fflush(temp) */
}



static void setup()
{
	separate();
}



static void debug()
{
	DbgPrint("Debug function:\n");
	DbgPrint("name %s \n",progname);
	DbgPrint("test %s \n",testname);
	DbgPrint("global_flag %d \n",global_flag);
	DbgPrint("local_flag %d \n",local_flag);
	DbgPrint("block_number %d \n",block_number);
	DbgPrint("condition_number %d \n",condition_number);
}



static void 
errmesg(char message[])          
{
	DbgPrint("*** Internal error %s \n",message);
	debug();
	DbgPrint("*** Internal error %s error\n",message);
	/* fflush(temp)  */
	return; /* exit(-1); */ 
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

	fclose(temp);
	return output_flag;
}



static void anyfail(){
	switch(global_flag){
		case PASSED: { DbgPrint("++++++++++++%s Passed\n",
					progname);
			       pend();
			       return; /* exit(PASSED); */
			     }
			
		case FAILED: { DbgPrint("------------%s ******FAILED******\n",
					progname);
			       pend();
			       return; /* exit(FAILED); */
			     }
		case ERROR:  { DbgPrint("***Internal Error");
			       DbgPrint("***Internal Error anyfail"); 
			       pend();
			       return; /* exit(ERROR); */
			     }
		default:     { DbgPrint("Fatal Error in %s",testname);
			       DbgPrint("Fatal Error in %s",testname);
			       pend();
			       return; /* exit(32767); */
			     }
		}
}



static void 
ipostcall(int got, int expected, char mesgstr[])
{
	DbgPrint("\t\tgot  %d expected  %d %s \n",got ,expected, mesgstr);
	/* fflush(temp) */
}

static void 
opostcall(int got, int expected, char mesgstr[])
{
	DbgPrint("\t\tgot  %o expected  %o %s \n",got ,expected, mesgstr);
	/* fflush(temp) */
}

static void 
fpostcall(float got, float expected, char mesgstr[])
{
	DbgPrint("\t\tgot  %f expected  %f %s \n",got ,expected, mesgstr);
	/* fflush(temp) */
}

static void 
spostcall(char got[], char expected[], char mesgstr[])
{
	DbgPrint("\t\tgot  :%s: expected  :%s: %s \n",
		got ,expected, mesgstr);
	/* fflush(temp) */
}

static void 
lpostcall(long got, long expected, char mesgstr[])
{
	DbgPrint("\t\tgot %d expected  %d %s\n",
		got,expected,mesgstr );
	/* fflush(temp) */
}

#ifndef NO_ERR_SUITE

#define DEBUG 1
#define CC_OUT_SZ 2046
#define EXPECT_SZ 254

static char got[CC_OUT_SZ];			/* CC stderr */
static char expect[EXPECT_SZ];			/* expected error match substring */

/* 
 * get_output() --- Get output from file and put in buf.
 *
 *	Returns: 	0 if ok, else 1.
 */
static int get_output(
	char * file,			/* file name */
	char *buf,			/* target buffer */
	int size			/* max size */
	)
{
	FILE * fp;
	size_t r; /*LX:10131999 /int/size_t for 64-bit friendliness */

	fp = fopen (file, "r");
	if (fp == NULL) {
		DbgPrint( "Can't open %s. errno=%d\n", file, errno);
		return 1;
	}
/*LX:10131999 added the (int) cast below. Not sure why this is so general in the 
first place. sizeof(*buf) seems to be always 1, but I leave it at this for now */
	r = fread (buf, (int) sizeof(*buf), size, fp);
	if (r < 0) {
		DbgPrint( "Can't read %s. errno=%d\n", file, errno);
		return 1;
	}
	/* DbgPrint("buf=\n%s\n", buf); */

	fclose (fp);
	return 0;
}

static int lc_get_output()
{
	if (get_output ("tmp.out", (char*) &got, CC_OUT_SZ)) {
		DbgPrint( "Could not obtain results of compile.\n");
		local_flag = FAILED;
	}
	else if (get_output ("exp.out", (char*) &expect, EXPECT_SZ)) {
		DbgPrint( 
		  "Could not obtain expectations of compile.\n");
		local_flag = FAILED;
	}
	return (local_flag == FAILED);
}

static void compile_it(char *what)
{
	int r=0;

	char command[1022];

	sprintf (command, "%s %s", "./doCC.sh", what);

        /* Compile test only. */

/************************** MICROSOFT CHANGES ******************************/
#ifdef PEREN_TEST
#ifdef _WINDOWS
        /* r = system (command); */
#endif
#endif
/************************** MICROSOFT CHANGES ******************************/

	r = (r & 0xff00) >> 8 ;

	switch (r) {
	case 0:		/* OK */
		break;
	case 1:
		DbgPrint( "Missing err_data file. UNTESTED.\n");
		local_flag = FAILED;
		break;
	case 2:
		DbgPrint( "Missing err_path file. UNTESTED.\n");
		local_flag = FAILED;
		break;
	case 3:
		DbgPrint( "Incomplete err_data file. UNTESTED.\n");
		local_flag = FAILED;
		break;
	case 4:
		DbgPrint( "Incomplete err_path file. UNTESTED.\n");
		local_flag = FAILED;
		break;
	case 5:
		DbgPrint( "Test case file nonexistent. UNTESTED.\n");
		local_flag = FAILED;
		break;
	case 6:
		DbgPrint( "Exit status of CC = 0. Expected non-zero.\n");
		local_flag = FAILED;
		break;
	case 7:
		if (lc_get_output())	/* Can't get CC output? */
			break;		/* Failure */

		DbgPrint( 
		  "Expected error message keywords: %s\nGot: %s\n", 
			expect, got); /* But not a failure. */
		break;
	case 8:
		DbgPrint( "No error or warning message.\n");
		local_flag = FAILED;
		break;

	case 9:
		if (lc_get_output())	/* Can't get CC output? */
			break;		/* Failure */

		DbgPrint( 
		  "Unexpected error (should be implemented).\nGot: %s\n", 
			got); /* But not a failure. */
		break;

	case 10:
		if (lc_get_output())	/* Can't get CC output? */
			break;		/* Failure */

		DbgPrint( 
	         "Unexpected non-zero status (should be implemented).\n");
		/* But not a failure. */
		break;

	default:
		DbgPrint( "Unexpected result. Exit status=%d\n", r);
		if (r < 0) DbgPrint( "/t errno=%d.", errno);
		local_flag = FAILED;
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
 * failure(msg) will set local_flag to FAILED and display a diagnostic that
 * is intended to be the reason that the block failed. 
 *
 * As a convenience, if the message is not terminated by a newline '\n' the
 * routine will output one. To do this it first writes the string to a
 * buffer and then prints the buffer.
 */
static void failure(char * format)
{
	DbgPrint( "%s : %s", progname, format);
	if (format[strlen(format) - 1] != '\n')
		DbgPrint( "\n");
	local_flag = FAILED;
}

#if defined(__cplusplus)
static void failure(char * format, int i1, int i2)
{
	char buf[256];
	sprintf(buf, "%s : %s", progname, format);
	DbgPrint( buf, i1, i2);
	if (format[strlen(format) - 1] != '\n')
		DbgPrint( "\n");
	local_flag = FAILED;
}
#endif

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
