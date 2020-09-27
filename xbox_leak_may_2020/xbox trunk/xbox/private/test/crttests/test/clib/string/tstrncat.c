
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

#define FOREVER     while(1)

#ifdef QA_SIMPLE_QALIB
#define OUTER_ITERATIONS 10
#define MAX_STRING_LENGTH 1000
#else 
#define OUTER_ITERATIONS 200
#define MAX_STRING_LENGTH 10000
#endif


static char mainbuf[10004];
static char chkbuf[10004];
static char catbuf[120];

static char test[] = __FILE__;
static int perfect;

int tstrncatEntry(void){
        int i, j, l, m, n;
        int testiter;

        char *p, *q, *r;

        startest();

        printf("start of strncat test\n");

        for ( testiter = 0 ; testiter < OUTER_ITERATIONS ; testiter++ ) {

            // Initialize mainbuf and chkbuf

            for ( i = 0 ; i < 10000 ; i++ ) mainbuf[i] = chkbuf[i] = 0;

            p = mainbuf + (rand() % 4);
            q = catbuf + (rand() % 4);
            r = chkbuf + (p - mainbuf);

            FOREVER {

                l = rand() % 100;
                m = rand() % 100;

                n = min(l, m);

                if ( strlen(p) + n + 1 > MAX_STRING_LENGTH )
                    break;

                // build a random string of strlen == l

                for ( j = 0 ; j < l ; j++ )
                    if ( (q[j] = rand()) == 0 )
                        j--;
                q[l] = 0;

                // strncat the random string onto the big main string

                strncat(p, q, m);

                // update the big check string the hardway

                for ( j = 0 ; j < n ; j++ )
                    *(r++) = q[j];
                *r = 0;

            }

            if ( memcmp(mainbuf, chkbuf, 10000) == 0 )
              ;
//!                printf("%d-th iteration of test succeeded!\n", testiter);
            else {
//!                printf("%d-th iteration of test failed!\n", testiter);
                fail(testiter);
                break;
            }
        }

        printf("end of strncat test\n");
        finish();
}
