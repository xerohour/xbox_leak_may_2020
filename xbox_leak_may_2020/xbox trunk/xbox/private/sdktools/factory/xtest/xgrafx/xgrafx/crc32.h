/*******************************************************************************
 *
 * Title: crc32.h
 *
 * Abstract: 32 bit CRC calculator.
 *
 ******************************************************************************/

#ifndef _CRC32_H
#define _CRC32_H

#define INITIAL_CRC32     0xffffffff

#ifdef __cplusplus
extern "C" {
#endif

unsigned long updateCRC32(unsigned char *pByte, int numBytes, unsigned long crc);

#ifdef __cplusplus
}
#endif

#endif  /* _CRC32_H */
