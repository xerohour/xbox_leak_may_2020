/* @(#)File: fstream3.C    Version: 1.0    Date: 09/27/90             */
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
    < Test ofstream and ifstream constructors of no arguments
    <
    < 0.  Test ofstream::ofstream()
    < 1.  Test ifstream::ifstream()

>HOW:   < 0,1. Construct fstreams and verify that they are not open
    <      since they have no associated file.

>MODS:  < 07/02/90, DRE: Changed definition of failure() to a prototype.
    <    Included "testhead.h" instead of "testhead.h"
    < Vers 1.0, date: 3/5/90: Created DRE

>BUGS:  < Bugs or system dependencies: none known.

======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif 
#include "testhead.h"


static char progname[] = "fstream3()";     

/*--------------------------------------------------------------------*/
void fstream3_main()                 
{
    setup();

/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the fstream classes.\n");
    DbgPrint("Test the ofstream::ofstream() constructor.\n");

    {
        ofstream ofs;

        if (ofs.rdbuf()->is_open()) 
        failure("The ofstream thinks a file is open.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

    DbgPrint("REF: AT&T Library Manual Appendix A-4, Stream\n");
    DbgPrint("I/O Library, testing the fstream classes.\n");
    DbgPrint("Test the ifstream::ifstream() constructor.\n");

    {
        ifstream ifs;

        if (ifs.rdbuf()->is_open()) 
        failure("The ifstream thinks a file is open.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  

}                   


#include "peren_another.c"
