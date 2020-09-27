#ifndef NVUTYPES_INCLUDED
#define NVUTYPES_INCLUDED
/*
 * nvutypes.h
 *
 * Copyright (C) 1995,1996 NVIDIA Corporation.
 *
 * This file contains unpacked types corresponding to the packed 32-bit 
 * types in <nv32.h>.  For example, the byte/bit alignment of Nvu1Pt16
 * is the same as required by renderSolidRectangle.Rectangle[0].y_x.
 *
 * If you want to manipulate elements smaller than 32 bits in your code, 
 * use these structures or define similar structures.  But when
 * you make assignments to hardware offsets in the NV Architecture,
 * you must use at least 32-bit wide quantities -- you must perform 
 * 32-bit writes.  You can cast these structures to 32 bits or combine
 * them in a union with pure 32-bit elements.
 *
 * An alternative to using these structures is to store values in your
 * own chosen format, and use the macros in <nvmacros.h> to assemble 
 * them into 32-bit values before writing to the chip.  But such macros 
 * take CPU cycles (except for constant values).
 */

/* Note that in the type names, "u" means unpacked, not "U" for unsigned". */

#ifndef NV32_INCLUDED
#include <nv32.h>                       /* for V008, U032, etc. typedefs */
#endif

typedef short	S016;

typedef struct {
	V008                c0;                               /*0000-0000*/
	V008                reserved01[0x003];                /*0001-0003*/
} Nvu1Color08;
typedef struct {
	V008                c0;                               /*0000-0000*/
	V008                c1;                               /*0001-0001*/
	V008                c2;                               /*0002-0002*/
	V008                c3;                               /*0003-0003*/
} Nvu4Color08;
typedef struct {
	V016                c0;                               /*0000-0001*/
	V016                reserved01[0x001];                /*0002-0003*/
} Nvu1Color16;
typedef struct {
	V016                c0;                               /*0000-0001*/
	V016                c1;                               /*0002-0003*/
} Nvu2Color16;
typedef struct {
	V032                c0;                               /*0000-0003*/
} Nvu1Color32;
typedef union {
	Nvu1Color08         v008;                             /*0000-0003*/
	Nvu1Color16         v016;                             /*0000-0003*/
	Nvu1Color32         v032;                             /*0000-0003*/
	V032                p;                                /*0000-0003*/
} Nvu1Color;

typedef union {
	Nvu4Color08         v_4_008;                          /*0000-0003*/
	Nvu2Color16         v_2_016;                          /*0000-0003*/
	Nvu1Color32         v_1_032;                          /*0000-0003*/
	Nvu1Color16         v_1_016;                          /*0000-0003*/
	Nvu1Color08         v_1_008;                          /*0000-0003*/
	V032                p;                                /*0000-0003*/
} NvuColor;

typedef struct {
	U016                fractHertz;                       /*0000-0001*/
	U016                intHertz;                         /*0002-0003*/
} NvuHertz;

typedef struct {
	U016                fractPart;                        /*0000-0001*/
	U016                intPart;                          /*0002-0003*/
} NvuFract;

typedef struct {
	S016                x;                                /*0000-0001*/
	S016                y;                                /*0002-0003*/
	U016                w;                                /*0004-0005*/
	U016                h;                                /*0006-0007*/
} NvuRect16;

typedef struct {
	S016                x;                                /*0000-0001*/
	S016                y;                                /*0002-0003*/
} Nvu1Pt16;

typedef struct {
	U016                w;                                /*0000-0001*/
	U016                h;                                /*0002-0003*/
} NvuDim16;

typedef struct {
	S016                x0;                               /*0000-0001*/
	S016                y0;                               /*0002-0003*/
	S016                x1;                               /*0004-0005*/
	S016                y1;                               /*0006-0007*/
} Nvu2Pt16;

typedef struct {
	S016                x0;                               /*0000-0001*/
	S016                y0;                               /*0002-0003*/
	S016                x1;                               /*0004-0005*/
	S016                y1;                               /*0006-0007*/
	S016                x2;                               /*0008-0009*/
	S016                y2;                               /*000a-000b*/
} Nvu3Pt16;

typedef struct {
	S016                x0;                               /*0000-0001*/
	S016                y0;                               /*0002-0003*/
	S016                x1;                               /*0004-0005*/
	S016                y1;                               /*0006-0007*/
	S016                x2;                               /*0008-0009*/
	S016                y2;                               /*000a-000b*/
	S016                x3;                               /*000c-000d*/
	S016                y3;                               /*000e-000f*/
} Nvu4Pt16;

typedef struct {
	S016                x0;                               /*0000-0001*/
	S016                y0;                               /*0002-0003*/
	S016                x1;                               /*0004-0005*/
	S016                y1;                               /*0006-0007*/
	S016                x2;                               /*0008-0009*/
	S016                y2;                               /*000a-000b*/
	S016                x3;                               /*000c-000d*/
	S016                y3;                               /*000e-000f*/
	S016                x4;                               /*0010-0011*/
	S016                y4;                               /*0012-0013*/
	S016                x5;                               /*0014-0015*/
	S016                y5;                               /*0016-0017*/
	S016                x6;                               /*0018-0019*/
	S016                y6;                               /*001a-001b*/
	S016                x7;                               /*001c-001d*/
	S016                y7;                               /*001e-001f*/
	S016                x8;                               /*0020-0021*/
	S016                y8;                               /*0022-0023*/
} Nvu9Pt16;


#endif /* NVUTYPES_INCLUDED */
