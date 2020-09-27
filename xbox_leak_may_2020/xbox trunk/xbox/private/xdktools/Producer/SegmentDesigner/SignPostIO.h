#ifndef __SIGNPOSTIOH_
#define __SIGNPOSTIOH_

typedef struct SignPostExt
{
    SignPostExt	*pNext;
    long		lTime;		// Time, in clocks
    short		nMeasure;	// Which measure
    DWORD		dwSignPost;	// SignPost type
} SignPostExt;

typedef struct SignPostMeasureInfo
{
	SignPostExt	*pSignPost;	// The signpost in this measure.
	DWORD		dwFlags;	// Currently, just whether the measure is selected.
} SignPostMeasureInfo;
/*
typedef struct ioSignPost
{
    long	lTime;		// Time, in clocks.
    DWORD	dwSignPost;	// SignPost type.
} ioSignPost;
*/
typedef struct PPGSignPost
{
	long			lMeasure;
	DWORD			dwValid;
	DWORD			dwSignPost;
} PPGSignPost;

#define SP_A        0x1
#define SP_B        0x2
#define SP_C        0x4
#define SP_D        0x8
#define SP_E        0x10
#define SP_F        0x20
#define SP_LETTER   (SP_A | SP_B | SP_C | SP_D | SP_E | SP_F)
#define SP_1        0x100
#define SP_2        0x200
#define SP_3        0x400
#define SP_4        0x800
#define SP_5        0x1000
#define SP_6        0x2000
#define SP_7        0x4000
#define SP_ROOT     (SP_1 | SP_2 | SP_3 | SP_4 | SP_5 | SP_6 | SP_7)
#define SP_CADENCE  0x8000

#define VALID_SIGNPOST	0x1
#define VALID_CADENCE	0x2

#define SPMI_SELECTED	0x1 // The measure is selected.

#endif