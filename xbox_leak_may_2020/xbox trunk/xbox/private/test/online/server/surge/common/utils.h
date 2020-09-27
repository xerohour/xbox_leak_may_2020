
#ifndef _UTILS_H_
#define _UTILS_H_

#define BUFFER_FILL_ALPHA 0x01
#define BUFFER_FILL_NUM   0x02
#define BUFFER_FILL_PUNCT 0x04
#define BUFFER_FILL_ASCII 0x08
#define BUFFER_FILL_NULL  0x10

extern char* NewRandomBuffer(size_t minSize, size_t maxSize, unsigned char fill, size_t &outLen, unsigned int &seed);
extern void Trace(LPCTSTR lpszFormat, ...);

#endif // _UTILS_H_