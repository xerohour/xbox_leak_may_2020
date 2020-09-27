/* @(#)File: fstream_1.C    Version: 1.0    Date: 09/27/90            */
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
  < Test the file modes
  <
  < 0. test mode ate of ifstream (at-end). Reads should return EOF.
  < 1. test mode ate of ofstream
  < 2. test mode app of ofstream
  < 3. test mode trunc of ofstream
  < 4. test mode noreplace of ofstream
  < 5. test mode nocreate of ofstream
  <

>HOW:   < 0.  Open an ifstream with mode ios::in | ios::ate.
  <     This should position the file pointer at the end of the
  <     file. According to the description of the ios::ate bit
  <     on the fstream(3C++) man page ios::ate does not imply
  <     ios::out, so this should be valid. Thus a read should
  <     return EOF immediately.
  < 1.  ofstream ate: copy the data file to the output file to
  <     give it known contents. Then close and reopen the file
  <     with mode at-end. Write a line to the file. Verify that
  <     the data was appended.
  < 2.  ofstream app: same as block 1
  < 3.  ofstream trunc: give the file known contents. Then
  <     open for trunc. Then close and verify the file is
  <     empty.
  < 4.  ofstream noreplace: give the file known contents. Then
  <     open with this mode. The open should fail.
  < 5.  ofstream nocreate: Remove the output file. Then try
  <     and open it for nocreate. That should fail.
  <

>MODS:  < 07/02/90, DRE: Changed definition of failure() to a prototype.
  <    Included "testhead.h" instead of "testhead.h"
  < Vers 1.0, date: 3/5/90: Created DRE

>BUGS:  < This test is dependent on the ANSI C libraries because it
  < uses the tmpnam() and remove() file routines.
  <
  < Block 0 is failed. Apparently the file pointer is not being
  < positioned at the end of the file, but rather the beginning.
  < Putting a unique character at the first position of the file
  < shows that that is the character being read.
  <
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
//#include <osfcn.h>
//#include "remove.h"

#include "testhead.h"


static char progname[] = "fstream1()";

    // Copy known data into the data file

static int create_datafile(const char * filename);

    // Check that the file contains the expected data followed
    // by the append string

int verify_append(const char * filename);

    // Copy a src file to a dest to give the dest known contents

void copy(const char * dest, const char * src);

    // String to append to a file to test the append mode

extern const char * append_string;

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
void fstream1_main()          
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

  DbgPrint("%s%s%s",
    "REF: AT&T Library Manual Appendix A-4, Stream\n",
    "I/O Library, testing the fstream classes.\n",
    "Test opening an ifstream at-end (``ate'' mode).\n");
  
  {
#if defined(USING_OLD_IOS)
      ifstream ifs0(datfile,ios::ate|ios::in);
#else
      ifstream ifs0(datfile,ios::app|ios::ate|ios::in|ios::out);
#endif
      if (!ifs0.good())
    failure("Can't open ifstream with mode ate (at-end)");
      else {
    char c;
    ifs0 >> c;
    // That should have failed because we're at the end
    if (!ifs0.eof()) {
        failure("Should be at end of file but EOF not true.");
        DbgPrint( "Read char with val %d\n", int(c));
    }
    ifs0.close();
      }
  }

  blexit();
/*--------------------------------------------------------------------*/
block1: blenter();      

  DbgPrint("%s%s%s",
    "REF: AT&T Library Manual Appendix A-4, Stream\n",
    "I/O Library, testing the fstream classes.\n",
    "Test of ofstream mode ``ios::ate'' (at-end).\n");
  
  {
      // Give output file known contents
      copy(outfile,datfile);

#if defined(USING_OLD_IOS)
      ofstream ofs1(outfile,ios::out|ios::ate);
#else
      ofstream ofs1(outfile,ios::out|ios::app);
#endif
      if (!ofs1.good())
    failure("Can't open output fstream with more append.");
      else {

    // Append another string.
    ofs1 << append_string << "\n";
    ofs1.close();

    if (verify_append(outfile) == 0)
        failure("Output file for at-end didn't append data");
      }
  }

  blexit();
/*--------------------------------------------------------------------*/
block2: blenter();      

  DbgPrint("%s%s%s",
    "REF: AT&T Library Manual Appendix A-4, Stream\n",
    "I/O Library, testing the fstream classes.\n",
    "Test of ofstream mode ``ios::app'' (append).\n");
  
  {
      // Give output file known contents
      copy(outfile,datfile);

      // Open it with at-end mode
      ofstream ofs2(outfile,ios::app|ios::out);

      if (!ofs2.good())
    failure("Can't open output fstream with more append.");
      else {

    // Append another string.
    ofs2 << append_string << "\n";
    ofs2.close();

    if (verify_append(outfile) == 0)
        failure("Output file for append didn't append data");
      }
  }

  blexit();
/*--------------------------------------------------------------------*/
block3: blenter();      

  DbgPrint("%s%s%s",
    "REF: AT&T Library Manual Appendix A-4, Stream\n",
    "I/O Library, testing the fstream classes.\n",
    "Test of ofstream mode ``ios::trunc.''\n");
  
  {
      copy(outfile,datfile);
      ofstream ofs3(outfile,ios::trunc|ios::out);
      if (!ofs3.good())
    failure("Can't open ofstream for trunc.");
      else {

    ofs3.close();
    // verify the file is empty
    ifstream ifs(outfile,ios::in);
    char c;
    ifs >> c;
    if (!ifs.eof()) {
        failure("After open for trunc file not empty.");
        DbgPrint( "Read char with value %d\n", int(c));
    }
      }
  }

  blexit();
/*--------------------------------------------------------------------*/
block4: blenter();      

  DbgPrint("%s%s%s",
    "REF: AT&T Library Manual Appendix A-4, Stream\n",
    "I/O Library, testing the fstream classes.\n",
    "Test of ofstream mode ``ios::noreplace.''\n");
  
  {
#if defined(USING_OLD_IOS)
      copy(outfile,datfile);
      ofstream ofs4(outfile,ios::noreplace|ios::out);

      if (ofs4.good()) {
    failure("Successfully opened existing file for noreplace.");
    ofs4.close();
      }
#endif
  }

  blexit();
/*--------------------------------------------------------------------*/
block5: blenter();      

  DbgPrint("%s%s%s",
      "REF: AT&T Library Manual Appendix A-4, Stream\n",
      "I/O Library, testing the fstream classes.\n",
      "Test of ofstream mode ``ios::nocreate.''\n");
  
  {
      // Make sure the file doesn't exist.
      remove(outfile);
      // Now try not to create it.
#if defined(USING_OLD_IOS)
      ofstream ofs5(outfile, ios::out|ios::nocreate);
#else
      ofstream ofs5(outfile, ios::out|ios::in);
#endif
      if (ofs5.good())
    failure("Incorrectly opened file for nocreate.");
  }

  blexit();
/*--------------------------------------------------------------------*/
  remove(outfile);
  remove(datfile);
done:
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

static const char * append_string = "append_me";

/**********************************************************************/
/*                                                                    */
/* copy                                                               */
/*                                                                    */
/* This routine copies one file to another. This is used to give a    */
/* file known contents.                                               */
/*                                                                    */
/**********************************************************************/

void copy(const char * dest, const char * src)
{
  ifstream ifs(src, ios::in);
#if defined(USING_OLD_IOS)
  ofstream ofs(dest, ios::out);
#else
  ofstream ofs(dest, ios::out|ios::trunc);
#endif

  const int bufsize = 256;
  char buf[bufsize];

  ifs.width(bufsize);

  while(ofs) {
    ifs >> buf;
    if (ifs.eof())
      break;
    ofs << buf << "\n";
  }
}

/**********************************************************************/
/*                                                                    */
/* verify_append                                                      */
/*                                                                    */
/* This routine reads the input file and verifies it contains the     */
/* expected data followed by the append string.                       */
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

int verify_append(const char * filename)
{
  ifstream ifs(filename, ios::in);

  const char ** expect = expected_data;
  const int bufsize = 128;
  char buf[bufsize];

  while (*expect) {
    ifs >> buf;
    if (strcmp(buf, *expect)) {
      DbgPrint( "Read '%s', expected '%s'\n",
        buf, *expect);
      return 0;
    }
    expect++;
  }
  ifs >> buf;
  if (strcmp(buf, append_string)) {
    DbgPrint( "Read '%s', expected '%s'\n",
      buf, append_string);
    return 0;
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
