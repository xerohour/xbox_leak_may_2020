#include <stdio.h>

int __cdecl main( int argc, char * argv[] )
{
    FILE * file;
    int c1, c2;

    if ( argc != 2 )
    {
        printf( "MZTOGGLE: swap the first two bytes of an MZ file\n"
                "\n"
                "Usage: MZTOGGLE {file}\n" );

        return( 1 );
    }

    file = fopen( argv[ 1 ], "r+b" );
    if ( file == NULL )
    {
        printf( "MZTOGGLE: unable to open %s for update\n", argv[ 1 ] );
        return( 1 );
    }

    c1 = fgetc( file );
    c2 = fgetc( file );

    if ((( c1 != 'M' ) || ( c2 != 'Z' )) &&
        (( c2 != 'M' ) || ( c1 != 'Z' )))
    {
        printf( "MZTOGGLE: not an MZ file\n" );
        return( 1 );
    }

    fseek( file, 0, SEEK_SET );

    fputc( c2, file );
    fputc( c1, file );

    fclose( file );

    printf( "MZTOGGLE: swapped %c%c to %c%c\n", c1, c2, c2, c1 );

    return( 0 );
}