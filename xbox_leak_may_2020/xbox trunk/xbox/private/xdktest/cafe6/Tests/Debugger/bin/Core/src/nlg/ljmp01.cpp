// fpreset.c 
//#include <stdio.h>
//#include <signal.h>
#include <setjmp.h>
//#include <stdlib.h>
#include <float.h>
#include <math.h>
//#include <string.h>
#include "nlgmain.h"

jmp_buf mark;
int     error;
double check( int i );

//void main( void )
void ljmp01(void)
{
	double n1, n2, r1, r2;
	int jmpret;
#ifndef _68K_
	for( int i = 0; i < 100; i++ )
	{
		jmpret = setjmp( mark );
		if( jmpret == 0 )
		{
			n1 = 0.5 * (i + 3);
			r2 = (i + 7);
			r2 /= 7;
			n2 = modf( r2 , &r1 );
			if( n2 <= 0.0000000001 )
			{
				longjmp( mark, -1 );
			}
			else
			{
				r1 = n1/n2;
			}
		}
		else
			r1 = check(i);
	}

	for( i = 0; i < 100; i++ )
	{
		jmpret = setjmp( mark );
		switch( jmpret )
		{
			case 0:
				n1 = 0.5 * (i + 3);
				r2 = (i + 7);
				r2 /= 7;
				n2 = modf( r2 , &r1 );
				if( n2 <= 0.0000000001 )
				{
					ljmp02( mark, i );
				}
				else
				{
					r1 = n1/n2;
				}
				break;
			case -1:
				r1 = check(i);
				break;
			case 1:
				r1 = check(i) + 2.5;
				break;
			case 2:
				r1 = i * (i + 2) * 1.5;
				break;
			default:
				break;
		}
	}
#endif
}

double check( int i )
{
	return ( 0.7 * (i + 2)	);	
}