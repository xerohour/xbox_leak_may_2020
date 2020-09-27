/* libmain - flex run-time support library "main" function */

/* $Header: \\VIA-SOURCE\RELEASE21\dev\RCS\Tools\flex-2.5.4a\libmain.c,v 1.1 1999/12/08 23:40:25 wayneg Exp $ */

extern int yylex();

int main( argc, argv )
int argc;
char *argv[];
	{
	while ( yylex() != 0 )
		;

	return 0;
	}
