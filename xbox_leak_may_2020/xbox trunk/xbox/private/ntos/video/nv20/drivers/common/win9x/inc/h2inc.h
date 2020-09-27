/******************************************************************************
 *                                                                            *
 *  H2INC.H  - A file used to help H2INC deal with complex header files.      *
 *                                                                            *
 ******************************************************************************/

#ifndef _H2INC_H        // Try not to include this file twice
#define _H2INC_H

#ifdef  H2INC           // Only include this file if it is in H2INC (-DH2INC on command line)

typedef unsigned long   DWORD;
typedef unsigned short  WORD;
typedef unsigned char   BYTE;

#endif //  H2INC
#endif // _H2INC_H
