/* @(#)File: filbuf_2.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual Select Code 307-145
    < Appendix A-4, Stream I/O Library Manual Pages, 

>WHAT:  < FILEBUF(3C++) Manual page dated 14 June 1989
    <
    < This buffer class is intended to use a file as a source or
    < sink for a stream of bytes. This test sets up some filebufs
    < and tests their functionality.
    <
    < 0. The filebuf open() member allows the file protection
    <    to be specified for file creation. 
    <

>HOW:   < 0. Test that the file is created with the correct mask
    <    using the fstat system call. Note that the indicated
    <    permissions cannot be more liberal than permitted by
    <    the umask or they will be limited and the test will
    <    fail. 

>MODS:  < 07/02/90, DRE: Removed definition of failure().
    <    Included "testhead.h" rather than "testhead.h".
    < Vers 1.0, date: 3/2/90: Created DRE

>BUGS:  < This test uses the ANSI C library routine tmpnam() to
    < get the name of a temporary file, thus it is ANSI C
    < library dependent.
    <
    < The test is Unix-dependent because it uses the
    < fstat system call to check file permissions.
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif 
//#include <osfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include "remove.h"

#include "testhead.h"


static char progname[] = "filbuf_2()";

/**********************************************************************/
/*                                                                    */
/* This test uses a data file that it reads and copies. The name of   */
/* the file is stored in the datfile variable.                        */
/*                                                                    */
/**********************************************************************/

static int create_datafile(const char * filename);

static char filenamebuf[L_tmpnam];

static const char * datfile = tmpnam(filenamebuf);

/*--------------------------------------------------------------------*/
filbuf_2_main()                  
{
    setup();

/*--------------------------------------------------------------------*/
block0: blenter();          

#if defined(USING_OLD_IOS)

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the filebuf class.\n");
    DbgPrint("When a file is created by the open member of\n");
    DbgPrint("the filebuf class its protection mask may be\n");
    DbgPrint("specified.\n");

    filebuf fb1;
    const int prot=0600;

    if (fb1.open(datfile, ios::out, prot) == 0)
        failure("Cannot open data file.");
    else {
        // Stat the file and verify its permissions.
        struct stat buf;
        if (fstat(fb1.fd(), &buf) == -1) 
            failure("Cannot stat the data file.");
        else if ((buf.st_mode & 0777) != prot) {
            char buff[80];
            sprintf(buff, "File created with wrong mode 0%o.",
            buf.st_mode & 0777);
            failure(buff);
        }
    }
    
    // Remove the data file
    remove(datfile);

#endif 

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
    return 0;
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

    ofs << "Line 1"<< "\n";
    ofs << "Line 2"<< "\n";
    ofs << "Line 3"<< "\n";

    ofs.close();
    return 1;
}

#include "peren_another.c"
