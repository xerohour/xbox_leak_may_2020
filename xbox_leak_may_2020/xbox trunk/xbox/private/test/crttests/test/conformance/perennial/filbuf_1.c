/* @(#)File: filbuf_1.C    Version: 1.0    Date: 11/08/90             */
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
    < 0. A filebuf is initially closed.
    <
    < 1. Open the file with the Unix open() system call. Then 
    <    declare a filebuf with the file descriptor and a buffer
    <    as arguments to the constructor. Verify the file is open.
    <
    < 2. Open the file with the Unix open() system call. Then 
    <    declare a filebuf with the file descriptor as an argument
    <    to the constructor. Check that is_open() reports that the
    <    file is open. 
    <
    < 3. Use the attach() member to attach a file descriptor to
    <    a filebuf.
    <
    < 4. Use the open() member to attach a file name to
    <    a filebuf.

>HOW:   < 0. Declare a filebuf and verify that it does not consider
    <    itself open.
    <
    < 1. Open a file with the open system call. Pass the file 
    <    descriptor to a filebuf constructor and verify that the
    <    filebuf considers itself open. Also pass in a buffer
    <    pointer and length.
    <
    < 2. Open a file with the open system call. Pass the file 
    <    descriptor to a filebuf constructor and verify that the
    <    filebuf considers itself open. Don't pass in a buffer
    <    pointer and length.
    <
    < 3. Open the file by hand and pass the file descriptor to
    <    the attach member.
    <
    < 4. Open the file with the open() member of the filebuf class.

>MODS:  < 07/02/90, DRE: Removed definition of failure().
    <    Included "testhead.h" rather than "testhead.h".
    < Vers 1.0, date: 3/1/90: Created DRE

>BUGS:  < This test uses the ANSI C library routine tmpnam() to
    < get the name of a temporary file, thus it is ANSI C
    < library dependent.
    <
    < The test is Unix-dependent because it uses the open()
    < system call to obtain file descriptors. This is required
    < to test constructors and attach() member functions that
    < take file descriptors as arguments.

======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif 
#include <fcntl.h>
//#include <osfcn.h>
//#include "remove.h"

#include "testhead.h"


static char progname[] = "filbuf_1()"; 

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
filbuf_1_main()                  
{
    setup();

/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("Creating data file used by this test.\n");
    // Create the data file that the rest of this test uses.
    if (create_datafile(datfile) == 0) {
        failure("Cannot create data file: can't run test.");
        goto done;
    }

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the filebuf class.\n");
    DbgPrint("A filebuf is initially closed.\n");

    {
        filebuf fb1;

        if (fb1.is_open())
            failure("Filebuf reported as being open.");
    }
    
    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

#if defined(USING_OLD_IOS)  /* filebuf no longer accepts file handles. */

    if (local_flag != FAILED) {

        DbgPrint("%s%s%s%s%s%s%s",
        "REF: AT&T Library Manual Appendix A-4, Stream\n",
        "I/O Library, testing the filebuf class.\n",
        "Open the file with the Unix open() system call.\n",
        "Then declare a filebuf with the file descriptor\n",
        "and a buffer as args to the constructor. Check that\n",
        "is_open() reports that the file is open.\n",
        "Check that the fd() member returns the expected fd.\n");
        
        int fd = _open(datfile, O_RDONLY);
        const int buff_size = 128;
        char buff[buff_size];

        if (fd == -1) {
            local_flag = FAILED;
            extern int errno;
            DbgPrint( "%s : %s %d!\n", 
                progname, 
                "Non-C++ error: open system call failed, errno",
                errno);
        }
        else {

        filebuf fb2(fd,buff,buff_size);

        if (!fb2.is_open()) 
            failure("File is not flagged as being open.");
        else if (fb2.fd() != fd) 
            failure("Filebuf returned unexpected file descriptor.");
        else if (fb2.close() != &fb2) 
            failure("Error on file close.");
        }

    } /* local_flag != FAILED */

#endif

    blexit();
/*--------------------------------------------------------------------*/
block2: blenter();          

#if defined(USING_OLD_IOS)

    if (local_flag != FAILED) {

        DbgPrint("%s%s%s%s%s%s%s",
        "REF: AT&T Library Manual Appendix A-4, Stream\n",
        "I/O Library, testing the filebuf class.\n",
        "Open the file with the Unix open() system call.\n",
        "Then declare a filebuf with the file descriptor\n",
        "as an argument to the constructor. Check that\n",
        "is_open() reports that the file is open.\n",
        "Check that the fd() member returns the expected fd.\n");
        
        int fd = _open(datfile, O_RDONLY);

        if (fd == -1) {
        local_flag = FAILED;
        extern int errno;
        DbgPrint( "%s : %s %d!\n", 
            progname, 
            "Non-C++ error: open system call failed, errno",
            errno);
        }
        else {
        filebuf fb3(fd);

        if (!fb3.is_open()) 
            failure("File is not flagged as being open.");
        else if (fb3.fd() != fd)
            failure("Filebuf returned unexpected file descriptor.");
        else if (fb3.close() != &fb3) 
            failure("Error on file close.");
        }
        
    } /* local_flag != FAILED */

#endif

    blexit();
/*--------------------------------------------------------------------*/
block3: blenter();          

#if defined(USING_OLD_IOS)

    if (local_flag != FAILED) {

        DbgPrint("%s%s%s%s%s",
        "REF: AT&T Library Manual Appendix A-4, Stream\n",
        "I/O Library, testing the filebuf class.\n",
        "Attach a file descriptor to a filebuf with the\n",
        "attach() member. Verify that if the filebuf is\n",
        "already open the attach() operation returns 0.\n",
        "Check that the fd() member returns the expected fd.");
        
        filebuf fb4;

        int fd = _open(datfile, O_RDONLY);

        if (fd == -1) {
        local_flag = FAILED;
        extern int errno;
        DbgPrint( "%s : %s %d!\n", 
            progname, 
            "Non-C++ error: open system call failed, errno",
            errno);
        } 
        else if (fb4.attach(fd) != &fb4) 
            failure("Could not attach file descriptor.");
        else if (fb4.fd() != fd) 
            failure("Filebuf returned wrong file descriptor.");
        else if (fb4.attach(0) != 0) 
            failure("Attaching second fd succeeded.");
        else if (fb4.fd() != fd) 
            failure("After second attach filebuf has bad fd.");
        else if (fb4.close() != &fb4) 
            failure("Error on file close.");
        
    } /* local_flag != FAILED */

#endif

    blexit();
/*--------------------------------------------------------------------*/
block4: blenter();          

#if defined(USING_OLD_IOS)

    if (local_flag != FAILED) {

        DbgPrint("%s%s%s%s%s",
            "REF: AT&T Library Manual Appendix A-4, Stream\n",
            "I/O Library, testing the filebuf class.\n",
            "Use the open() member of the filebuf to open\n",
            "a file.\n");
        
        filebuf fb4;

        if (fb4.open(datfile,ios::nocreate|ios::in) != &fb4) 
            failure("Could not open file.");
        else if (fb4.close() != &fb4) 
            failure("Error on file close.");

    } /* local_flag != FAILED */

#endif

    blexit();
/*--------------------------------------------------------------------*/
    remove(datfile);
done:
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
