/*
 * typedefs.h
 */
#ifndef _TYPEDEFS_H

#define _TYPEDEFS_H

typedef unsigned char	byte;
typedef unsigned short	ushort;
typedef unsigned long	ulong;

typedef enum
{
	false = 0,
	true = 1
} bool;

typedef enum
{
	COMPRESSION_TYPE_NONE,
	COMPRESSION_TYPE_MSZIP,
	COMPRESSION_TYPE_LZX
} t_compression_type;

#endif /* _TYPEDEFS_H */
