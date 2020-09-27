/* @(#)File: manip5.C    Version: 1.0    Date: 01/02/91               */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, Manipulators.
    < OSTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing predefined manipulators:
    <
    < 0. ends
    <
    < 1. flush
    <
>HOW:   < 0. Use an ostream to write to a character array.
    <    Insert an ``ends'' and check that a NULL character
    <    was written.
    <
    < 1. Use an ostream to write to a file.
    <    Check by reading through a separate file pointer that
    <    inserting ``flush'' flushes the file.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    < Vers 1.0, date: 5/4/90 Created: DRE

>BUGS:  < Some 2.1 Cfronts fail to execute this test.
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <fstream.h>
#include <strstrea.h>
#else
#include <strstream>
#include <iostream>
#include <fstream>
#endif
#include "remove.h"

#include "testhead.h"

static char progname[] = "manip5()";
/*--------------------------------------------------------------------*/
void manip5_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) manual page dated 14 June 1989.\n");
    DbgPrint("Testing ends manipulator.\n");

    {
        strstreambuf sb;
        ostream os(&sb);
        os << ends;
        char c = 'A';
        sb.sgetn(&c,1);
        if (c)
        failure("Could not extract the NULL.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) manual page dated 14 June 1989.\n");
    DbgPrint("Testing the flush manipulator.\n");

    char fname[L_tmpnam], iobuf[128];
    char * data = "this is it";
//The following two lines were introduced for troubleshooting. LX:04252000
	char commandLine[256];
	int removeRC;
    tmpnam(fname);
    ofstream os(fname);

	sprintf(commandLine,"attrib %s\n",fname);

    os << data; // data probably wasn't written yet, it's buffered

    FILE * fp = fopen(fname, "r");

    if (fp == NULL)
        failure("Cannot open output file for reading.");
    else if (fread(iobuf, sizeof(*iobuf), 3, fp) == 0) { 
        // Now flush, reread, and verify the data is written.
        os << flush;
        if (fgets(iobuf, 128, fp) != iobuf) 
            failure("Still couldn't read the data after flushing.");
        else if (strcmp(iobuf,data))
            failure("Read wrong data.");
    } else
        DbgPrint("Data wasn't buffered so it can't be flushed.");
    
	os.close(); //LX:04252000
    fclose(fp);

//Line below was introduced for troubleshooting. Deactivate it when appropriate.
	/* system(commandLine); */
    removeRC=remove(fname);

if (removeRC) DbgPrint("manip5.c (%d): file %s could not be removed. \nBeware of garbage in the root dir!\n",__LINE__, fname);

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   

#include "peren_another.c"
