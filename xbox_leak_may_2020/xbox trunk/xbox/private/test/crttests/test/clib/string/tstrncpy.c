
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

#define FOREVER     while(1)

//This test takes too much time on simulated platforms: do only 10 iterations there
#ifdef QA_SIMPLE_QALIB
#define OUTER_ITERATIONS 10
#else
#define OUTER_ITERATIONS 200
#endif

static char mainbuf[10004];
static char chkbuf[10004];
static char cpybuf[120];

static char test[] = __FILE__;
static int perfect;

int tstrncpyEntry(void){
        int i, j, l, m;
        int testiter;

        char *p, *q, *r;

        startest();

        printf("start of strncpy test\n");

        for ( testiter = 0 ; testiter < OUTER_ITERATIONS ; testiter++ ) {

            // Initialize mainbuf and chkbuf

            for ( i = 0 ; i < 10000 ; i++ ) mainbuf[i] = chkbuf[i] = 0;

            p = mainbuf;
            q = cpybuf + (rand() % 4);
            r = chkbuf;

            FOREVER {
              
                l = rand() % 100;
                m = rand() % 100;
               
                if ( p + m > mainbuf + 10000 )
                    break;

                // build a random string of strlen == l

                for ( j = 0 ; j < l ; j++ ) 
                    if ( (q[j] = rand()) == 0 )
                        j--;
                q[l] = 0;

                // strncat the random string onto the big main string

                strncpy(p, q, m);

                p += m;

                // update the big check string the hardway

                for ( j = 0 ; j < m ; j++ )
                    if ( j < l )
                        *(r++) = q[j];
                    else
                        *(r++) = 0;

            }

            if ( memcmp(mainbuf, chkbuf, 10000) != 0 ) {
                printf("%d-th iteration of test failed!\n", testiter);
                fail(testiter);
                break;
            }
        }

        printf("end of strncpy test\n");
        finish();
}
