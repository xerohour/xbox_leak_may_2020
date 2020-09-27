/* @(#)File: fstream_2.C    Version: 1.0    Date: 09/27/90            */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual Select Code 307-145
    < Appendix A-4, Stream I/O Library Manual Pages, 

>WHAT:  < FSTREAM(3C++) Manual page dated 14 June 1989
    < This stream class is for use in performing file I/O.
    < They use the filebuf class.
    <
    < 0.  Opening an output file with the ios::in mode bit
    <     prevents truncation.
    <
    < CLEAN UP --- After the last block the output file is removed.


>HOW:   < 0. Give the data file known contents. Then close it and
    <    reopen it for output with ios::in. Then close it.
    <    Verify that the contents have not been altered.
    <

>MODS:  < 07/02/90, DRE: Changed definition of failure() to a prototype.
    <    Included "testhead.h" instead of "testhead.h"
    < Vers 1.0, date: 3/2/90: Created DRE

>BUGS:  < This test uses the tmpnam() ANSI C library routine to
    < obtain a temporary file name, thus it is ANSI C dependent.

======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif 
#include <fcntl.h>
#include <stdio.h>
#include "remove.h"

#include "testhead.h"


static char progname[] = "fstream2()";

    // Copy known data into the data file

static int create_datafile(const char * filename);

    // This function reads the input file stream and checks that its 
    // contents match the expected contents.

static int verify_ifs(ifstream * pifs);

/**********************************************************************/
/*                                                                    */
/* This test uses a data file that it reads. The name of              */
/* the file is stored in the datfile variable.                        */
/*                                                                    */
/**********************************************************************/

static char filenamebuf[L_tmpnam];

static const char * datfile = tmpnam(filenamebuf);

/*--------------------------------------------------------------------*/
fstream2_main()                  
{
    setup();

/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, FSTREAM(3C++) manual page.\n");
    DbgPrint("ios::in: Opening an output file with this bit\n");
    DbgPrint("prevents truncation.\n");

    if (create_datafile(datfile)) {
        ofstream ofs(datfile, ios::out|ios::in);

        if (!ofs.rdbuf()->is_open()) 
        failure("Cannot open data file.");
        else {
        ofs.close();            // Close it 
        ifstream ifs(datfile);  // Reopen it
        if (!ifs.rdbuf()->is_open()) 
            failure("Cannot reopen data file.");
        else
            if (!verify_ifs(&ifs))
            failure("File has wrong contents.");
        }
        // Remove the data file
        remove(datfile);
    } else {
        failure("Cannot create data file: can't run test.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
    return 0;
}                   


/**********************************************************************/
/*                                                                    */
/* This is the data the test writes to its input file.                */
/*                                                                    */
/**********************************************************************/

static const char * expected_data[] = {
    "aaaaaaaaaa",
    "bbbbbbbbbb",
    "0123456789",
    0
};

/**********************************************************************/
/*                                                                    */
/* verify_ifs                                                         */
/*                                                                    */
/* This routine reads the input file and verifies it contains the     */
/* expected data.                                                     */
/*                                                                    */
/* Return:                                                            */
/*                                                                    */
/*    1   correct data                                                */
/*    0   incorrect data                                              */
/*                                                                    */
/* Side-effect:                                                       */
/*                                                                    */
/*    The current file read/write position is undefined after this    */
/*    routine.                                                        */
/*                                                                    */
/**********************************************************************/

static int verify_ifs(ifstream * pifs)
{
    const char ** expect = expected_data;
    const int bufsize = 128;
    char buf[bufsize];

    while (*expect) {
        *pifs >> buf;
        if (strcmp(buf, *expect)) {
            DbgPrint( "Read '%s', expected '%s'\n",
                buf, *expect);
            return 0;
        }
        expect++;
    }
    return 1;
}

/**********************************************************************/
/*                                                                    */
/* create_datafile                                                    */
/*                                                                    */
/* This routine writes the known data into the data file.             */
/*                                                                    */
/* Return:                                                            */
/*                                                                    */
/*    1   success                                                     */
/*    0   failure for some reason                                     */
/*                                                                    */
/**********************************************************************/

static int create_datafile(const char * filename)
{
    ofstream ofs(filename, ios::out|ios::trunc);
    if (!ofs.good()) {
        extern int errno;
        DbgPrint( "Cannot open data file to write it.\n");
        DbgPrint( "errno is %d.\n",errno);
        return 0;
    }

    const char ** expect = expected_data;
    while (*expect)
        ofs << *expect++ << "\n";
    return 1;
}

#include "peren_another.c"
