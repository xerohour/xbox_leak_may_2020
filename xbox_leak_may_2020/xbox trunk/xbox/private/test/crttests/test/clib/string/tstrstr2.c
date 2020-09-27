
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

static char searched_string[1024];

static char *basestr = "unbelievable";

static char *fillchar = "Z";

struct srchdata {
        char *  key_string;
        int     success_code;
}; 

struct srchdata scenarios[10] = {
        {"v",            1},    /* 0 */
        {"w",            0},    /* 1 */
        {"un",           1},    /* 2 */
        {"uv",           0},    /* 3 */
        {"unbe",         1},    /* 4 */
        {"unba",         0},    /* 5 */
        {"unbelie",      1},    /* 6 */
        {"unbelix",      0},    /* 7 */
        {"unbelievable", 1},    /* 8 */
        {"unbelievablz", 0}     /* 9 */ };


static char test[] = __FILE__;
static int perfect;

int tstrstr2Entry(void){
        int i, j;

        size_t len;

        char *psrch, *pret;

        startest();

        // build searched_string[] out of copies of basestr, separated by 
        // increasing numbers of fillchar copies.

        searched_string[0] ='\0';

        for ( i = 0 ; i < 10 ; i++ ) {

            strcat(searched_string, basestr);

            for ( j = 0 ; j < i + 1 ; j++ )
                strcat(searched_string, fillchar);
        }

 
        for ( i = 0 ; i < 10 ; i++ ) {
            
            psrch = searched_string;
           
            if ( scenarios[i].success_code == 1 ) {

                // In this case, where strstr() is supposed to succeed, we run five
                // searches, each starting just after the substring located by the 
                // the last.

                len = strlen(scenarios[i].key_string);

                for ( j = 0 ; j < 5 ; j++ )
                    if ( ((pret = strstr(psrch, scenarios[i].key_string)) != NULL) &&
                         (strncmp(pret, scenarios[i].key_string, len) == 0) )
                    {
                        if ( j == 4 ) 
                            printf("test %d passed\n", i);
                        else
                            psrch = pret + len;
                    }
                    else {
                        printf("test %d failed (%d-th iteration)\n", i, j);
                        fail(i);
                        break;
                    }
            }
            else {

                // In this test, strstr() is supposed to fail.

                if ( strstr(scenarios[i].key_string, psrch) == NULL )
                    printf("test %d passed\n", i);
                else
                    fail(i+100);
            }

        }
        finish();
}
