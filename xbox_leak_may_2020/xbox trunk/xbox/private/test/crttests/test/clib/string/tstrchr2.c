
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

#ifdef QA_SIMPLE_QALIB
#define TEST_MAX    500
#else
#define TEST_MAX    1000
#endif

static char buf[ 0x410 ];

static char test[] = __FILE__;
static int perfect;

int tstrchr2Entry(void){
        char *pbuf,*pret;
        int test_count, srch_char, length;
        int i;
        int success_flag;

        startest();

        for ( test_count = 0 ; test_count < TEST_MAX ; test_count++ ) {

            // Using the random number generator, set the length of the search
            // string, the search character and the alignment of the string to
            // be searched.

            length = rand() & 0x03ff;
            srch_char = rand() & 0x0ff;
            pbuf = buf + (rand() & 3);            

            // Fill the search string with non-zero random characters.

            for ( i = 0 ; i < length ; i++ )
                while ( (pbuf[i] = rand() & 0x0ff) == 0 );

            pbuf[length] = '\0';

            // print out test header.
/*
            printf( "test %d: search char = %x, strlen = %d, offset = %d, result = ",
                          test_count,       srch_char,   length,      (pbuf - buf) );
*/
            // run the test

            pret = strchr( pbuf, srch_char );

            // check the result

            if ( pret == NULL ) {

                // strchr reports failure. make sure there is no instance of 
                // srch_char in the string.

                success_flag = 1;

                for ( i = 0 ; i <= length ; i++ )
                    if ( srch_char == (pbuf[i] & 0x0ff) ) {
                        success_flag = 0;
                        fail(test_count);
                        break;
                    }

            }
            else {

                // strchr reports success. make sure an instance of srch_char 
                // was really found, and that it was the first such instance.

                if ( srch_char != (*pret & 0x0ff) ) {
                    success_flag = 0;
                    fail(test_count+100);
                }
                else {

                    success_flag = 1;

                    for ( i = 0 ; (pbuf + i) < pret ; i++ )
                        if ( (srch_char == (pbuf[i] & 0x0ff)) || 
                             (0 == pbuf[i]) ) 
                        {
                            success_flag = 0;
                            fail(test_count+200);
                            break;
                        }

                }
            }
/*
            if ( success_flag )
                printf("PASSED\n\n");
            else
                printf("FAILED\n\n");
*/
        }
        finish();
}
