
/***************************************************************************

		   Copyright (c) 1992, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: _strerror

===========================================================================
Syntax 	
	char *strerror( int errnum );
	char *_strerror( const char *string );

Parameter	Description
errnum		Error number
string		User-supplied message

The strerror function maps errnum to an error-message string, returning a
pointer to the string. The function itself does not actually print the
message; for that, you need to call an output function such as fprintf:

if (( _access( "datafile",2 )) == -1 )
   fprintf( stderr, strerror(NULL) );

If string is passed as NULL, _strerror returns a pointer to a string containing
the system error message for the last library call that produced an error. The
error-message string is terminated by the newline character ('\n').
If string is not equal to NULL, then _strerror returns a pointer to a string
containing (in order) your string message, a colon, a space, the system error
message for the last library call producing an error, and a newline character.
Your string message can be a maximum of 94 bytes long.

Unlike perror, _strerror alone does not print any messages. To print the
message returned by _strerror to stderr, your program will need an fprintf
statement, as shown in the following lines:

if (( _access( "datafile",2 )) == -1 )
   fprintf( stderr, _strerror(NULL) );

The actual error number for _strerror is stored in the variable errno. The
system error messages are accessed through the variable _sys_errlist, which
is an array of messages ordered by error number. 
The _strerror function accesses the appropriate error message by using the
errno value as an index to the variable _sys_errlist. The value of the variable
_sys_nerr is defined as the maximum number of elements in the _sys_errlist
array.

To produce accurate results, _strerror should be called immediately after a
library routine returns with an error. Otherwise, the errno value may be
overwritten by subsequent calls.

Note that the _strerror function under Microsoft C version 5.0 is identical to
the version 4.0 strerror function. The name was altered to permit the inclusion
in Microsoft C version 5.0 of the ANSI-conforming strerror function. The
_strerror function is not part of the ANSI definition but is instead a
Microsoft extension to it; it should not be used where portability is desired.
For ANSI compatibility,  use strerror instead.

Include File    <string.h>is required only for function declarations.

Return Value    The strerror and _strerror functions return a pointer to the
		error-message string. The string can be overwritten by
		subsequent calls to strerror or _strerror, respectively.

===========================================================================
OS Version:
CPU:
Dependencies:		
Environment Vars:	
Verification Method:	
Priority:		1
Notes:			
Products:		WIN NT


Revision History:

    Date	emailname   description
----------------------------------------------------------------------------
    27-Jul-92	xiangjun    created
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <test.h>

static char test[] = "strerror3";
static int perfect;


int strerror3Entry(){
static char error_string[100];
static int i;

   startest();

   for( i = 0; i < 37; i++ ) {

      errno = i;

      strcpy( error_string, strerror( i ) );
      strcat( error_string, "\n" );
      if( strcmp( error_string, _strerror( NULL ) ) )
         fail( i );

      strcpy( error_string, "my_str_error" );
      strcat( error_string, ": " );
      strcat( error_string, strerror( i ) );
      strcat( error_string, "\n" );

      if( strcmp( error_string, _strerror( "my_str_error" ) ) )
 	 fail( 100+i );
      }

   finish();
}

