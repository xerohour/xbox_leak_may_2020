/* @(#)File: ssbuf4.C       Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, SSBUF(3C++)      >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < Constructor strstreambuf(char *ptr, int n, char * pstart) >
    < This constructor has various modes of behavior depending  >
    < on the sign of n, and the values of ptr and pstart.       >

>HOW:   < For each mode of the constructor, create an object and verify >
    < the object has the expected get and put areas.        >

    < In all cases, the get area starts at ptr.  Its length depends >
    < on n and pstart.  To test the content and length of the get   >
    < area, we call sgetn() to request a larger number of char's    >
    < than we know are available.  sgetn() returns the number of    >
    < char's actually fetched and stores this characters in an  >
    < array.                            >

    < The put area is tested using out_waiting() and sputc().   >

    < In the following cases, pstart is always NULL.  Consequently  >
    < the initial get area is the entire array.  Test the effect    >
    < of n.                             >

    < 0. if n is positive, the get area consists of the n bytes     >
    < following ptr.                        >

    < 1. If n is zero, the get area terminates at the end of the    >
    < string that starts at ptr.                    >

    < 2. If n is negative, the get area is assumed to continue  >
    < indefinitely.  We verify it extends beyond the end-of-string. >

    < 3. If pstart is null, all stores are treated as errors.   >

    < In the following cases, pstart is non-NULL.  Consequently >
    < the initial get area is the area between ptr and pstart.  >
    < Check the effect of n.                    >

    < 4. Make n positive and larger than pstart-ptr.  Check that    >
    < only the char's up to pstart can be fetched.          >

    < 5. Make n positive and smaller than pstart-ptr.  Check that   >
    < only the char's up to ptr+n can be fetched.           >

>MODS:  < Version 1.0 by HK.                        >

>BUGS:  < AT&T library BUG:                         >
    <   In block 4, the get area extends beyond pstart instead  >
    <   of only up to pstart as specified by the manual in  >
    <   section SSBUF(3C++), page 2.                >
========================================================================*/
#include "testhead.h"     

static char progname[] = "ssbuf4()";         

inline void print(char str[]) { DbgPrint( "%s\n", str);}
static void fail (char str[]) {
    local_flag =FAILED;
    DbgPrint( "\tFAILURE: %s\n", str);
}

/*--------------------------------------------------------------------*/
#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#define NULL 0

// global variable declarations

static char msg[] = "Hi there!";
static size_t msglen = strlen(msg); //LX:10131999 /int/size_t for 64-bit friendliness 
static char buf[15],
tmp[15];

/*--------------------------------------------------------------------*/
void ssbuf4_main()                                 /***** BEGINNING OF MAIN. *****/
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();                      

    print("AT&T Library Manual, Appendix A, SSBUF(3C++)");
    print("strstreambuf (char* ptr, int n, char* pstart) constructor:");
    print("if n is positive, the get area consists of the n bytes");
    print("following ptr.");

    strcpy(buf, msg);
    {strstreambuf ssb(buf, (streamsize) msglen -5 , NULL); /* LX:10131999 */
        if ((size_t)ssb.sgetn(tmp, (streamsize) msglen) != msglen -5) /* LX:10131999 */
            fail ("unexpected get area size");
        else if (strncmp(msg, tmp, msglen -5))
            fail ("unexpected get area content");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();                      

    print("AT&T Library Manual, Appendix A, SSBUF(3C++)");
    print("strstreambuf (char* ptr, int n, char* pstart) constructor:");
    print("If n is zero, the get area terminates at the end of the");
    print("string that starts at ptr.");

    strcpy(buf, msg);
    {strstreambuf ssb(buf, 0 , NULL);
        if ((size_t) ssb.sgetn(tmp, (streamsize) msglen+5) != msglen) /*LX:10131999 */
            fail ("unexpected get area size");
        else if (strcmp(msg, tmp))
            fail ("unexpected get area content");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();                      

    print("AT&T Library Manual, Appendix A, SSBUF(3C++)");
    print("strstreambuf (char* ptr, int n, char* pstart) constructor:");
    print("If n is negative, the get area is assumed to continue");
    print("indefinitely.  We verify it extends beyond the end-of-string.");

    strcpy(buf, msg);
    strcpy(buf+10,"test");
    {strstreambuf ssb(buf, -1 , NULL);
        if (ssb.sgetn(tmp, 15) != 15)
            fail ("unexpected get area size");
        else if (strcmp(tmp+10, "test"))
            fail ("unexpected get area content");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();                      

    print("AT&T Library Manual, Appendix A, SSBUF(3C++)");
    print("strstreambuf (char* ptr, int n, char* pstart) constructor:");
    print("If pstart is null, all stores are treated as errors.");

    strcpy(buf, msg);
    {strstreambuf ssb(buf, (streamsize) msglen , NULL); /*LX:10131999 */
        if (ssb.sputc('$') != EOF)
            fail ("unexpected success of sputc()");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();                      
// the ctor behaves differently with the final draft standard.
    //
#if defined(USING_OLD_IOS)  

    print("AT&T Library Manual, Appendix A, SSBUF(3C++)");
    print("strstreambuf (char* ptr, int n, char* pstart) constructor:");
    print("Make n positive and larger than pstart-ptr.  Check that");
    print("only the char's up to pstart can be fetched.");

    strcpy(buf, msg);
    {strstreambuf ssb(buf, msglen, buf+5);
        int i = ssb.sgetn(tmp, msglen);
        if (i != 5)
        {fail ("unexpected get area size");
            DbgPrint( "\tfetched %d char's instead of 5\n", i);
        }
        else if (strncmp(msg, tmp, 5))
            fail ("unexpected get area content");
    }
#endif 
    blexit();
/*--------------------------------------------------------------------*/
    block5: blenter();                      

    print("AT&T Library Manual, Appendix A, SSBUF(3C++)");
    print("strstreambuf (char* ptr, int n, char* pstart) constructor:");
    print("Make n positive and smaller than pstart-ptr.  Check that");
    print("only the char's up to ptr+n can be fetched.");

    strcpy(buf, msg);
    {strstreambuf ssb(buf, 5, buf+msglen);
        if (ssb.sgetn(tmp, (streamsize) msglen) != 5) /*LX:10131999*/
            fail ("unexpected get area size");
        else if (strncmp(msg, tmp, 5))
            fail ("unexpected get area content");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
