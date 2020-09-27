/*	Contains file I/O functions that write/read data    */
/*	LSB first.											*/

#pragma warning( disable : 4244 )
#pragma warning( disable : 4242 )

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

static uint8 s[4];

int write16(uint16 b, FILE *fp)
{
	s[0] = b;
	s[1] = b >> 8;
	return((fwrite(s, 1, 2, fp) < 2) ? 0 : 2);
}

int writelower8of16(uint16 b, FILE *fp)
{
	uint8 z;
	z = b;
	return((fwrite(&z, 1, 1, fp) < 1) ? 0 : 1);
}

int writeupper8of16(uint16 b, FILE *fp)
{
	uint8 z;
	z = b >> 8;
	return((fwrite(&z, 1, 1, fp) < 1) ? 0 : 1);
}

int write32(uint32 b, FILE *fp)
{
	s[0] = b;
	s[1] = b >> 8;
	s[2] = b >> 16;
	s[3] = b >> 24;
	return((fwrite(s, 1, 4, fp) < 4) ? 0 : 4);
}

int read16(char *d, FILE *fp)
{
#ifdef LSB_FIRST
	return((fread(d, 1, 2, fp) < 2) ? 0 : 2);
#else
	int ret;
	ret = fread(d + 1, 1, 1, fp);
	ret += fread(d, 1, 1, fp);
	return ret < 2 ? 0 : 2;
#endif
}
