/* @(#)File: ofstream.C    Version: 1.0    Date: 11/20/90             */
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
    < Test ofstream constructors and attach
    <
    < 0.  Test ofstream::ofstream(char * filename)
    < 1.  Test ofstream::ofstream(int fd)
    < 2.  Test ofstream::ofstream(int fd, char * buf, int bufsize)
    < 3.  Test the ofstream::attach() member
    <
    < CLEAN UP --- After the last block the output file is removed.


>HOW:   < 0-2. Construct ofstreams using the four constructors.
    <      Verify that when the file argument is valid the file
    <      is opened. Write data to the file and read it back to
    <      verify.
    < 3.   Attach a file descriptor and verify the stream is open.
    <      Write and verify the file contents.

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/1/90: Created DRE

>BUGS:  < SYSTEM DEPENDENCIES:
    <
    <   These tests hit constructors or member functions
    <   that take file descriptor arguments, so they are Unix dependent.
    <   This test is dependent on the ANSI C libraries because it
    <   uses the tmpnam() and remove() stdio routines.
    <
======================================================================*/

/***************************************************************************
Micorosoft Revision History:

    Date        emailname       description
----------------------------------------------------------------------------
    03-01-95    a-timke         Replaced oldnames with ANSI versions.
----------------------------------------------------------------------------
****************************************************************************/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
//#include <osfcn.h>
#include "remove.h"

#include "testhead.h"


static char progname[] = "ofstream()";

    // This function reads the input file stream and checks that its 
    // contents match the expected contents.

static int verify_ifs(ifstream * pifs);

    // This function writes an output file and then reads back 
    // and checks the contents.

int verify_ofs(ofstream * pofs, const char * filename);

/**********************************************************************/
/*                                                                    */
/* This test uses a data file that it reads and copies. The name of   */
/* the file is stored in the datfile variable. It also writes an      */
/* output file for testing file output.                               */
/*                                                                    */
/**********************************************************************/

static char namebuf1[L_tmpnam], namebuf2[L_tmpnam];

static const char * datfile = tmpnam(namebuf1);
static const char * outfile = tmpnam(namebuf2);

/*--------------------------------------------------------------------*/
void ofstream_main()                 
{
    setup();

/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the fstream classes.\n");
    DbgPrint("Test the ofstream::ofstream(char *) constructor.\n");

    {
#if defined(USING_OLD_IOS)
        ofstream ofs(outfile,ios::out);
#else
        ofstream ofs(outfile,ios::out|ios::trunc);
#endif
        if (ofs.good()) {
        if (!verify_ofs(&ofs,outfile))
            failure("Wrote wrong data.");
        ofs.close();
        }
        else
        failure("Error on file open.");
    }
    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the fstream classes.\n");
    DbgPrint("Test the ofstream::ofstream(int) constructor.\n");

    {
#if defined(USING_OLD_IOS)
        int fd;
        fd = _open(outfile, O_WRONLY);

        if (fd == -1) {
        local_flag = FAILED;
        extern int errno;
        DbgPrint( "%s : %s %d!\n", 
            progname, 
            "Non-C++ error: open system call failed, errno",
            errno);
        }
        else {
        ofstream ofs(fd);
        if (ofs.good()) {
            if (!verify_ofs(&ofs, outfile))
            failure("Wrote wrong data.");
            ofs.close();
        }
        else
            failure("Error on file open.");
        }
#endif
    }

    blexit();
/*--------------------------------------------------------------------*/
block2: blenter();          

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the fstream classes.\n");
    DbgPrint("Test the ofstream::ofstream(int, char*, int)\n");
    DbgPrint("constructor.\n");

    {
#if defined(USING_OLD_IOS)
        int fd;
        fd = _open(outfile, O_WRONLY);
        if (fd == -1) {
        local_flag = FAILED;
        extern int errno;
        DbgPrint( "%s : %s %d!\n", 
            progname, 
            "Non-C++ error: open system call failed, errno",
            errno);
        }
        else {
        const int bufsiz = 128;
        char buf[bufsiz];
        ofstream ofs(fd, buf, bufsiz);
        if (ofs.good()) {
            if (!verify_ofs(&ofs, outfile))
            failure("Wrote wrong data.");
            ofs.close();
        }
        else
            failure("Error on file open.");
        }
#endif
    }

    blexit();
/*--------------------------------------------------------------------*/
block3: blenter();          

    DbgPrint("%s%s%s",
        "REF: AT&T Library Manual Appendix A-4, Stream\n",
        "I/O Library, testing the fstream classes.\n",
        "Attaching a file descriptor to an ofstream.\n");
    
    {
#if defined(USING_OLD_IOS)

        int fd;
        
        fd = _open(outfile, O_WRONLY);
        if (fd == -1) {
        local_flag = FAILED;
        extern int errno;
        DbgPrint( "%s : %s %d!\n", 
            progname, 
            "Non-C++ error: open system call failed, errno",
            errno);
        }
        else {
        ofstream ofs;
        ofs.attach(fd);
        if (ofs.good()) {
            if (!verify_ofs(&ofs, outfile))
            failure("Wrote wrong data.");
            ofs.close();
        }
        else
            failure("Error on file open.");
        }
#endif
    }
        
    blexit();
/*--------------------------------------------------------------------*/
    remove(outfile);
    remove(datfile);
    anyfail();  

}                   


/**********************************************************************/
/*                                                                    */
/* This is the data the test expects to find in its input file.       */
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
/* verify_ofs                                                         */
/*                                                                    */
/* This routine verifies an output fstream. It writes the expected    */
/* contents into the file. Then it reopens the file as an input       */
/* stream and verifies the contents.                                  */
/*                                                                    */
/* Return:                                                            */
/*                                                                    */
/*    1   wrote correct data                                          */
/*    0   wrote incorrect data                                        */
/*                                                                    */
/* Side-effect:                                                       */
/*                                                                    */
/*    The current file read/write position is undefined after this    */
/*    routine.                                                        */
/*                                                                    */
/**********************************************************************/

int verify_ofs(ofstream * pofs, const char * filename)
{
    const char ** expect = expected_data;
    const int bufsize = 128;

    // Write the data
    while (*expect) 
        *pofs << *expect++ << "\n";
    
    pofs->flush();

    // Reopen the file for input
    ifstream ifs(filename);
    if (!ifs.good()) {
        DbgPrint( "Can't open out file for input\n");
        return 0;
    }

    // Verify the contents that were written
    return verify_ifs(&ifs);
}

#include "peren_another.c"
