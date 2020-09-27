/* @(#)File: filbuf_3.C    Version: 1.1    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, 1990, Perennial, All Rights Reserved               */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual Select Code 307-145
    < Appendix A-4, Stream I/O Library Manual Pages, 

>WHAT:  < SBUF.PROT(3C++) Manual page, FILEBUF(3C++) derived class
    < Manual pages dated 14 June 1989
    <
    < This derived class is intended to use a file as a source or
    < sink for a stream of bytes. This is a test of the setbuf
    < virtual function and unbuffered I/O. The sbuf.prot(3C++)
    < manual page indicates that setbuf(p,len) requests the
    < buffer class to do unbuffered I/O when len==0. The derived
    < class need not honor this request. If it does honor the
    < request it returns the address of the buffer, otherwise it
    < returns NULL. This file tests that functionality on the
    < filebuf derived class.
    <

>HOW:   < Open a filebuf on a file. Make it unbuffered.
    < Write a byte to the file through the buffer.
    < Then we try to read the byte through the file descriptor. 
    < It should be present because there should be no buffering.
    < The semantics tested in blocks 1-3 are described on the
    < filebuf(3C++) manual page. It is supposed to honor these
    < semantics.
    <
    < 0. Make the filebuf unbuffered with the constructor.
    <
    < 1. Make the filebuf unbuffered with a setbuf call with a
    <    NULL pointer.
    <
    < 2. Make file filebuf unbuffered with a setbuf call with 
    <    a zero length.
    <
    < 3. Make file filebuf unbuffered with a setbuf call with 
    <    a negative length.
    <

>MODS:  < 07/18/90, Ver 1.1, DRE
    <    Passed explicit 0644 permissions to the open system call.
    <    Added 1990 copyright.
    < 07/02/90, DRE: Removed definition of failure().
    <    Included "testhead.h" rather than "testhead.h".
    < Vers 1.0, date: 3/5/90: Created DRE

>BUGS:  < This test uses the ANSI C library routine tmpnam() to
    < get the name of a temporary file, thus it is ANSI C
    < library dependent.
    <
    < All four blocks of this test work on filebuf operators
    < that have file descriptor arguments, thus this test is
    < Unix dependent.

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
//#include <unistd.h>
#include <fcntl.h>
//#include "remove.h"

#include "testhead.h"


static char progname[] = "filbuf_3()";

/**********************************************************************/
/*                                                                    */
/* This test uses a data file that it reads and copies. The name of   */
/* the file is stored in the datfile variable.                        */
/*                                                                    */
/**********************************************************************/

static char filenamebuf[L_tmpnam];

static const char * datfile = tmpnam(filenamebuf);

/**********************************************************************/
/*                                                                    */
/* Default permissions for newly-created files.                       */
/*                                                                    */
/**********************************************************************/

const int PERM = 0644;

/*--------------------------------------------------------------------*/
filbuf_3_main()                  
{
    setup();

/*--------------------------------------------------------------------*/
block0: blenter();          

#if defined(USING_OLD_IOS)

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the filebuf class derived\n");
    DbgPrint("from streambuf. When the filebuf is created with\n");
    DbgPrint("a buffer length of zero I/O is unbuffered.\n");

    // Create data file.
    int fd;
    extern int errno;
    if ((fd = open(datfile,O_RDWR|O_CREAT,PERM)) == -1) {
        failure("Cannot open/create data file.");
        DbgPrint("Errno is %d\n", errno);
    }
    else {
        char    write_c = 'X';
        char    read_c;
        char    dummy_c;
        filebuf fb1(fd,&dummy_c,0); // create the filebuf, unbuffered
        if (!fb1.is_open())
        failure("Cannot attach filebuf to file descriptor.");
        else {
        // Write one byte to the file. Should not be buffered.
        fb1.sputc(write_c);
        // Read it back through the file descriptor
        if (lseek(fd,0,SEEK_SET) == -1) {
            failure("Cannot lseek on file descriptor.");
            DbgPrint("Errno is %d\n", errno);
        }
        else 
            if (read(fd, &read_c, 1) != 1)
            failure("Cannot read back written data.");
            else 
            if (read_c != write_c)
                failure("Read back wrong character.");
        }
    }
#endif
    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();

#if defined(USING_OLD_IOS)

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the filebuf class derived\n");
    DbgPrint("from streambuf. When the filebuf is given a buffer\n");
    DbgPrint("with a setbuf(p,len) call, if p is NULL the filebuf\n");
    DbgPrint("is unbuffered.\n");

    // Create data file.  
    if ((fd = open(datfile,O_RDWR|O_CREAT,PERM)) == -1) {
        failure("Cannot open data file.");
        DbgPrint("Errno is %d\n", errno);
    }
    else {
        char    write_c = 'X', read_c;
        filebuf fb1(fd);
        if (!fb1.is_open())
        failure("Cannot attach filebuf to file descriptor.");
        else if (fb1.setbuf(NULL,1) == 0)
        failure("setbuf(NULL,len) call refused.");
        else {
        // Write one byte to the file. Should not be buffered.
        fb1.sputc(write_c);
        // Read it back through the file descriptor
        if (lseek(fd,0,SEEK_SET) == -1) {
            failure("Cannot lseek on file descriptor.");
            DbgPrint("Errno is %d\n", errno);
        }
        else 
            if (read(fd, &read_c, 1) != 1)
            failure("Cannot read back written data.");
            else 
            if (read_c != write_c)
                failure("Read back wrong character.");
        }
    }

#endif 

    blexit();
/*--------------------------------------------------------------------*/
block2: blenter();

#if defined(USING_OLD_IOS)

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the filebuf class derived\n");
    DbgPrint("from streambuf. When the filebuf is given a buffer\n");
    DbgPrint("with a setbuf(p,len) call, if len is 0 the filebuf\n");
    DbgPrint("is unbuffered.\n");

    // Create data file.  
    if ((fd = open(datfile,O_RDWR|O_CREAT,PERM)) == -1) {
        failure("Cannot open data file.");
        DbgPrint("Errno is %d\n", errno);
    }
    else {
        char    write_c = 'X', read_c, dummy_c;
        filebuf fb1(fd);
        if (!fb1.is_open())
        failure("Cannot attach filebuf to file descriptor.");
        else if (fb1.setbuf(&dummy_c,0) == 0)
        failure("setbuf(&c,0) call refused.");
        else {
        // Write one byte to the file. Should not be buffered.
        fb1.sputc(write_c);
        // Read it back through the file descriptor
        if (lseek(fd,0,SEEK_SET) == -1) {
            failure("Cannot lseek on file descriptor.");
            DbgPrint("Errno is %d\n", errno);
        }
        else 
            if (read(fd, &read_c, 1) != 1)
            failure("Cannot read back written data.");
            else 
            if (read_c != write_c)
                failure("Read back wrong character.");
        }
    }

#endif 

    blexit();
/*--------------------------------------------------------------------*/
block3: blenter();

#if defined(USING_OLD_IOS)

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the filebuf class derived\n");
    DbgPrint("from streambuf. When the filebuf is given a buffer\n");
    DbgPrint("with a setbuf(p,len) call, if len is negative the\n");
    DbgPrint("filebuf is unbuffered.\n");

    // Create data file.  
    if ((fd = open(datfile,O_RDWR|O_CREAT,PERM)) == -1) {
        failure("Cannot open data file.");
        DbgPrint("Errno is %d\n", errno);
    }
    else {
        char    write_c = 'X', read_c, dummy_c;
        filebuf fb1(fd);
        if (!fb1.is_open())
        failure("Cannot attach filebuf to file descriptor.");
        else if (fb1.setbuf(&dummy_c,-3) == 0)
        failure("setbuf(&c,-3) call refused.");
        else {
        // Write one byte to the file. Should not be buffered.
        fb1.sputc(write_c);
        // Read it back through the file descriptor
        if (lseek(fd,0,SEEK_SET) == -1) {
            failure("Cannot lseek on file descriptor.");
            DbgPrint("Errno is %d\n", errno);
        }
        else 
            if (read(fd, &read_c, 1) != 1)
            failure("Cannot read back written data.");
            else 
            if (read_c != write_c)
                failure("Read back wrong character.");
        }
    }

#endif 

    blexit();
/*--------------------------------------------------------------------*/
    remove(datfile);
    anyfail();  
    return 0;
}                   

#include "peren_another.c"
