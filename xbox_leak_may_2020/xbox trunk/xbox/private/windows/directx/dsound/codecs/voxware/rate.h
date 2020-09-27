/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       rate.h
*                                                                              
* Purpose:        all bit manipulations for the rate change
*                                                                              
* Functions:
*                                                                              
* Author/Date:    Ilan Berci 97/09/23
*
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/rate.h_v   1.1   13 Feb 1998 11:24:02   weiwang  $
******************************************************************************/
#ifndef _RATE_H
#define _RATE_H

#define SC_BYTES_ADDITIONAL_3K 8
#define SC_BYTES_ADDITIONAL_6K 8

/* Listing of every byte size for every rate */
#define SC_BYTES_3K SC_BYTES_ADDITIONAL_3K
#define SC_BYTES_6K (SC_BYTES_3K+SC_BYTES_ADDITIONAL_6K)

/* Listing of every current rate */ 
#define SC_RATE_3K 1
#define SC_RATE_6K (1<<1)
#define SC_RATE_8K (1<<2)

/* Listing of valid fields */
#define SC_FIELD_3K SC_RATE_3K
#define SC_FIELD_6K (SC_FIELD_3K|SC_RATE_6K)

/* Defines for masking */
#define SC_RATE_SUPPORTED (SC_RATE_3K|SC_RATE_6K)
#define SC_RATE_UNSUPORTED (0xFFFF&~SC_RATE_SUPPORTED)

/* Listings of current rates that this instance of a codec supports */ 
#define SC_RATE_LOWEST  SC_RATE_3K
#define SC_RATE_HIGHEST SC_RATE_6K


/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: isSingleStream()                                                */
/* PURPOSE : determines if the stream is a single instance                   */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
unsigned char isSingleStream(unsigned long dwStream);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: isValidStream()                                                 */
/* PURPOSE : determines if the stream is a valid stream                      */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
unsigned char isValidStream(unsigned long dwStream, const unsigned short *wStreamTable);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: bytesInStream()                                                 */
/* PURPOSE : determines the number of bytes in the stream                    */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
unsigned short bytesInStream(unsigned long dwStream, const unsigned short *wStreamTable);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: bytesToStream()                                                 */
/* PURPOSE : Put the number of bytes to the stream.                          */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */
unsigned long bytesToStream(unsigned short wBytes, const unsigned short *wStreamTable);

/* ************************************************************************* */
/*                                                                           */
/* FUNCTION: seekForward()                                                   */
/* PURPOSE : returns the byte offset after seeking to desired frame          */
/*           returns -1 on insufficient buffer and -2 on corrupted stream    */
/*                                                                           */
/* ************************************************************************* */
short seekForward(unsigned char *pVoxBuffer,
                  unsigned short wReadByteOffset,
                  unsigned short wWriteByteOffset,
                  unsigned short wBufferSize,
                  unsigned short wNumFrames,
                  const unsigned short *wStreamTable);
#endif /* _RATE_H */









