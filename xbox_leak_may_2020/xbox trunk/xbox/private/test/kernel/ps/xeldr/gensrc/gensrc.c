/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    gensrc.c

Abstract:

    This program contains routines to generate include (.h) and
    assembly (.asm) file used by xetest

--*/

#include <stdio.h>
#include <stdlib.h>

char *h_header[] = {
    "/*++\n",
    "\n",
    "Copyright (c) Microsoft Corporation. All rights reserved.\n",
    "\n",
    "Module Name:\n",
    "\n",
    "    sections.h\n",
    "\n",
    "Abstract:\n",
    "\n",
    "    This program contains routines to test XE loader\n",
    "    DONOT edit this file, it's automatically generated\n",
    "\n",
    "--*/\n",
    "\n",
    "#ifndef _SECTIONS_INCLUDED\n",
    "#define _SECTIONS_INCLUDED\n",
    "\n",
    "#ifdef __cplusplus\n",
    "extern \"C\" {\n",
    "#endif\n",
    NULL
};

char *h_footer[] = {
    "\n",
    "#ifdef __cplusplus\n",
    "}\n",
    "#endif\n",
    "\n",
    "#endif // _SECTIONS_INCLUDED\n",
    NULL
};

char *asm_header[] = {
    ";++\n",
    ";\n",
    "; Copyright (c) Microsoft Corporation. All rights reserved\n",
    ";\n",
    "; Module Name:\n",
    ";\n",
    ";    sections.asm\n",
    ";\n",
    "; Abstract:\n",
    ";\n",
    ";    This module contains code in separate sections\n",
    ";    DONOT edit this file, it's automatically generated\n",
    ";\n",
    ";--\n",
    "\n",
    ".386p\n",
    "        .xlist\n",
    "include callconv.inc\n",
    "        .list\n",
    NULL
};

char *asm_footer[] = {
    "\n",
    "        end\n",
    NULL
};

void __cdecl main( int argc, char* argv[] )
{
    register i;
    FILE *h_fp, *asm_fp;
    unsigned short max_sections;

    if ( argc != 2 ) {
        printf( "Usage: gensrc <number of sections>\n" );
        return;
    }

    max_sections = (unsigned short)atoi( argv[1] );
    if ( !max_sections ) {
        printf( "Invalid parameter (number of section can't be zero)\n" );
        return;
    }

    h_fp = fopen( "sections.h", "wt" );
    if ( !h_fp ) {
        printf( "Unable to create sections.h\n" );
        return;
    }

    asm_fp = fopen( "i386\\sections.asm", "wt" );
    if ( !asm_fp ) {
        printf( "Unable to create i386\\sections.asm\n" );
        fclose( h_fp );
        return;
    }

    for ( i=0; h_header[i]; i++ ) {
        fputs( h_header[i], h_fp );
    }

    fprintf( h_fp, "\n#define MAX_SECTIONS %u\n", max_sections );
    fputs( "#define SECTION_NAME_PREFIX \"SECTN\"\n", h_fp );

    // Generate body
    for ( i=0; i<max_sections; i++ ) {
        fprintf( h_fp, "\nint\nWINAPI\nSection%03u(\n    VOID\n    );\n", i );
    }

    fputs( "\nCONST PROC Proc[] = {", h_fp );

    for ( i=0; i<max_sections; i++ ) {
        if ( (i%5) == 0 ) {
            fprintf( h_fp, "\n    " );
        }
        fprintf( h_fp, "Section%03u, ", i );
    }

    fputs( "\n};\n", h_fp );

    for ( i=0; h_footer[i]; i++ ) {
        fputs( h_footer[i], h_fp );
    }

    for ( i=0; asm_header[i]; i++ ) {
        fputs( asm_header[i], asm_fp );
    }

    // Generate body
    for ( i=0; i<max_sections; i++ ) {
        fprintf( asm_fp,
                 "\n;\n;\n;\nSECTN%03u SEGMENT PARA PUBLIC 'CODE'\n\n"
                 "cPublicProc _Section%03u ,0\n\n        db 4096 dup( 90h )\n\n"
                 "stdENDP _Section%03u\n\nSECTN%03u ends\n", i, i, i, i
               );
    }

    for ( i=0; asm_footer[i]; i++ ) {
        fputs( asm_footer[i], asm_fp );
    }

    fclose( h_fp );
    fclose( asm_fp );
}
