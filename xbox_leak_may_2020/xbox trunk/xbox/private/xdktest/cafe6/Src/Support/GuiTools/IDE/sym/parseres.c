/*
    parseress.c                                                 1/18/94

    Usage:  parseres infile outfile

    Purpose:  General frame for performing a conversion on a file on a
              per line basis, output goes to the outfile.  A filter.

              If outfile is unspecified, output go to stdout by default.

			  Specifically, this utility parses out #define strings
			  which appear as the first token
*/
//#define	_MBCS
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <tchar.h>

void main(int,char*[]);
void usage(void);
void mainline(void);
void parse_command_line(int,char*[]);
void initialize_globals(void);
int  getline(void);
TCHAR *get_basename(TCHAR *);
FILE *openfile(TCHAR *,char *);
void open_input(void);
void open_output(void);
void close_input(void);
void close_output(void);
void error(char *,char *,...);
void user_error(int,char *,char *,...);

#define     BUF_SIZE      1024
#define     MAX_FIELDS      50

/*
    These extensions should not exceed 3 characters in length, most
    filesystems on an Intel based computer do not support file extensions
    greater than 3 characters.
*/
#define     INFILE_DEFAULT_EXT      _T("in")
#define     OUTFILE_DEFAULT_EXT     _T("out")

/*
    Variables used with _splitpath(), in get_basename().
*/
TCHAR drive[ _MAX_DRIVE ];
TCHAR dir  [ _MAX_DIR   ];
TCHAR bname[ _MAX_FNAME ];
TCHAR ext  [ _MAX_EXT ];

FILE    *fin, *fout;

short   verbose;                        // verbose flag

TCHAR    buf[BUF_SIZE], tbuf[BUF_SIZE], outbuf[BUF_SIZE];
TCHAR    *prefix_string;

TCHAR    *input_file;
TCHAR    *output_file;
TCHAR    *program_name;

void main( int argc, char *argv[] )
{
program_name = argv[0];
if( argc < 2 ) usage();

initialize_globals();

parse_command_line( argc, argv );

mainline();


} /* main */


void usage()
{
_tprintf( "Usage:  %s <switches> infile outfile \n", program_name );
_tprintf( "        [-hs?]\n" );
_tprintf( "        s     prefix string         Prepend to macronames.\n" );
// _tprintf( "        v     verbose\n" );
_tprintf( "        h, ?  help                  This text.\n\n" );
_tprintf( "        If outfile is omitted, then output is sent to stdout.\n\n" );
_tprintf( "    This utility will parse through a text file searching for \"#define\"\n" );
_tprintf( "strings.  If found, the \"prefix string\" will be prepended on the macro name.\n" );
_tprintf( "The insertion occurs only if the #define string is the first token on the line.\n" );

exit( 0 );

} /* usage */


void mainline()
{
open_input();
open_output();

while( getline() )
    {
    /*
        Code to tokenize a line into individual fields.  Uncomment to use.
    */
    TCHAR *field_delimeters = _T(" \t\n");
    TCHAR *t, *token0, *token1;
	int  line_length = _tcslen( buf );		// Line length of current buf, in TCHAR's

	_tcscpy( outbuf, buf );
	
	token0 = buf+_tcsspn( buf, field_delimeters );	// Consume any initial delimeters
	if( *token0 != '\0' )
		{
    	t = _tcspbrk( token0, field_delimeters );    // Get the first token
		/*
			Case of #define
		*/
		if( *token0 != '\0' && !_tcsncmp( token0, _T( "#define" ), 7 ) )
			{
			/*
				First token is "#define"
			*/
			token1 = t+_tcsspn( t, field_delimeters );	// Consume any initial delimeters
			if( *token1 != '\0' )
				{
				_tcsncpy( outbuf, buf, token1-buf-1 );// Copy up to macroname to the output buffer
				*(outbuf+(token1-buf)) = '\0';		// Add the null terminator
				_tcscat( outbuf, prefix_string );	// Append the prefix_string
 				_tcscat( outbuf, token1 );			// Append the macroname and rest of string
				}
			}
	 	}
    _ftprintf( fout, "%s", outbuf );
    }

close_input();
close_output();


} /* mainline */


void parse_command_line( int argc, char *argv[] )
{
TCHAR   *p;
int     i = 0;
short   input_file_set  = 0;
short   output_file_set = 0;

while( ++i, (p = argv[ i ]) != NULL )
    {
    if( *p == _T('-') || *p == _T('/') )
        {
        switch( *(p+sizeof(TCHAR)) )
            {
            case 's': prefix_string = argv[++i];
					  if( prefix_string == NULL )
					      user_error( 3, "%s", "No prefix string specified.\n" );

				      if( verbose )
                          _tprintf( "parse_command_line(): prefix_string = %s\n", prefix_string );
                      break;
/*
            case 'v': verbose = 1;
                      _tprintf( "parse_command_line( argc = %d, *argv[] ):  "
                          "Enter function.\n", argc );
                      break;
*/
            case '?':
            case 'h': usage();

            default:
                _ftprintf( stderr, "Warning: Unknown switch '%s' ignored.\n", p );
            } /* switch */

        }
    else
        {
        /* TCHAR *p;        This variable conflicts with 'p' in this fct. */

        _tcscpy( buf, argv[i] );

        if( (p = _tcschr( buf, _T('.') )) == NULL )
            {
            _tcscat( buf, _T(".") );

            if( !input_file_set )
                _tcscat( buf, INFILE_DEFAULT_EXT );
            else if( !output_file_set )
                _tcscat( buf, OUTFILE_DEFAULT_EXT );
            }

        if( !input_file_set )
            {
            input_file = _tcsdup( buf );
            input_file_set = 1;
            }
        else if( !output_file_set )
            {
            output_file = _tcsdup( buf );
            output_file_set = 1;
            }

        /*
            Uncomment to use the input file's basename for your output file.
			NOTE:  basename uses splitpath(), which is not MBCS enabled!

        p = get_basename( buf );
        p = _tcschr( buf, _T('.') );

        _tcscpy( p, _T(".") );
        _tcscat( p, OUTFILE_DEFAULT_EXT );

        output_file = _tcsdup( buf );
        */
        }
    }

if( !input_file_set )
    user_error( 2, "%s", "No input file specified.\n" );

if( verbose )
    _tprintf( "parse_command_line():  Exit function.\n" );

} /* parse_command_line */



/*
    Initialize global variables
*/
void initialize_globals()
{
verbose = 0;                            // clear verbose flag

input_file      = NULL;
output_file     = NULL;

prefix_string   = "";


} /* initialize_globals */


int getline()
{
TCHAR *eof;

#ifdef _MBCS
eof = _fgetts( buf, BUF_SIZE-1, fin );
#else
eof = fgets( buf, BUF_SIZE-1, fin );
#endif

if( eof == NULL )
    {
    return 0;
    }
return 1;

} /* getline */


TCHAR *get_basename( TCHAR *path )
{
_splitpath( path, drive, dir, bname, ext );

#if VERBOSE
printf( "get_basename: %s\n", bname );
#endif

return bname;

} /* get_basename */


FILE *openfile( TCHAR *fname, char *mode )
{
FILE *fp;

fp = fopen( fname, mode );
if( fp == NULL )
    error( "openfile", "Could not open \"%s\" with mode %s.\n", fname, mode );

return fp;

} /* openfile */


void open_input()
{
if( verbose )
    _tprintf( "open_input():  input_file = \"%s\"\n", input_file );

fin = openfile( input_file, "r" );

} /* open_input */


void open_output()
{
if( verbose )
    _tprintf( "open_output():  output_file = \"%s\"\n",
        output_file == NULL
            ? "stdout"
            : output_file );

fout = output_file != NULL ? fopen( output_file, "w" ) : stdout;

if( fout == NULL )
    {
    user_error( 1, "Could not open '%s' with write priviliges.",
        output_file );
    }

} /* open_output */


void close_input()
{
fclose( fin );

} /* close_input */


void close_output()
{
fclose( fout );

} /* close_output */


void error( char *funcname, char *format, ... )
{
static char error_buf[256];     // error messages
va_list arg_ptr;

va_start( arg_ptr, format );

#ifdef  DEBUG
_stprintf( error_buf, "%s():  Error -> %s\n", funcname, format );
#else
_stprintf( error_buf, "Error -> %s\n", format );
#endif

_vftprintf( stderr, error_buf, arg_ptr );

va_end( arg_ptr );

exit( -1 );                     // die

} /* error */


void user_error( int error_no, char *funcname, char *format, ... )
{
static char error_buf[256];     // error messages
va_list arg_ptr;

va_start( arg_ptr, format );

#ifdef  DEBUG
_stprintf( error_buf, "%s():  Error(%d) -> %s\n", funcname, error_no, format );
#else
_stprintf( error_buf, "Error(%d) -> %s\n", error_no, format );
#endif

_vftprintf( stderr, error_buf, arg_ptr );

va_end( arg_ptr );

exit( error_no );

} /* user_error */
