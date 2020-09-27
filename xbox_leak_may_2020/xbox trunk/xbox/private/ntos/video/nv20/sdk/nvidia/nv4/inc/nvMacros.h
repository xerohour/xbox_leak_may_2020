#ifndef NVMACROS_INCLUDED
#define NVMACROS_INCLUDED
/*
 ****************************************************************************

 Copyright (C) 1994, 1995, 1996 NVIDIA Corporation

 These are suggested macros for working with the NV architecture.
 You can use these as is or modify them for your own needs.

 Note that you can get faster code by handcrafting inline assembler 
 for the packing macros, but this is compiler-dependent.

 ****************************************************************************
 */

#ifndef NV32_INCLUDED
#include <nv32.h>                       /* for V032, U032, etc. typedefs */
#endif

/***** Free Count Macros *****/

/*
 * WAIT_FIFO(psc,nBytes)
 *	The first parameter is a pointer to the subchannel.
 *
 * 	The second parameter is the number of _bytes_ the application 
 *	wants to write. This is 4 times the number of 32-bit writes,
 *	or you can use the sizeof() function to determine the size
 *	of the argument of each method invoked.
 *
 */
#define WAIT_FIFO(psc,nBytes)     while((psc)->control.free<(nBytes))


/*
 * WAIT_FREE(psc,nBytes, localFree)
 *  This stores the free count.
 *
 *  The first parameter is a pointer to the subchannel.
 *
 * 	The second parameter is the number of _bytes_ the application 
 *  wants to write. This is 4 times the number of 32-bit writes,
 *  or you can use the sizeof() function to determine the size
 *  of the argument of each method invoked.
 *
 *	The third parameter is a variable to hold a local saved
 *	copy of the free count.  As the application invokes methods,
 *	it should decrement this by the number of _bytes_ written.
 *
 */
#ifdef DEBUG  
#define WAIT_FREE(psc, nBytes, localFree) \
	assert((nBytes) < NV_GUARANTEED_FIFO_SIZE); \
	assert((psc) != 0); \
	/* keep reading free count until it rises */ \
	while ((localFree) < (nBytes)) { \
		(localFree) = (psc)->control.free; \
	}
#else 
#define WAIT_FREE(psc, nBytes, localFree) \
	/* keep reading free count until it rises */ \
	while ((localFree) < (nBytes)) { \
		(localFree) = (psc)->control.free; \
	}
#endif /* DEBUG */

/***************************************************************************
 *                          Packing Macros                                 *
 ***************************************************************************/


/***** Coordinates *****/

/* Pack two Signed 16-bit coordinates. Mustn't sign-extend */
#define PACK_XY(x,y)    ((V032)((((U032)(y))<<16)|(((U032)(x))&0x0000FFFF)) )

/* Pack two Unsigned 16-bit dimensions */
#define PACK_WH(w,h)    ((V032)((((U032)(h))<<16)|(((U032)(w))&0x0000FFFF)) )

/***** Colors *****/

/* Pack 1-bit Alpha and 5-bit R,G,B values into NV_COLOR_FORMAT_LE_X16A1R5G5B5  */
#define PACK_ARGB15(a,r,g,b) \
            ((V032)((((a)?(1<<15):0))|(((U032)(r)&0x1F)<<10)|\
            (((U032)(g)&0x1F)<<5)|((U032)(b)&0x1F)))

/* Pack 5-bit R,G,B values into NV_COLOR_FORMAT_LE_X17R5G5B5 */
#define PACK_RGB15(r,g,b)   (PACK_ARGB15(0,r,g,b))

/* Pack three 8-bit R,G,B values into NV_COLOR_FORMAT_LE_X8R8G8B8 */
#define PACK_RGB24(r,g,b)   \
            ((V032)((((U032)(r)&0xFF)<<16)|\
            (((U032)(g)&0xFF)<<8)|((U032)(b)&0xFF)))

/* Pack 8-bit Alpha and 8-bit R,G,B values into NV_COLOR_FORMAT_LE_A8R8G8B8 */
#define PACK_ARGB24(a,r,g,b)    \
            ((V032)((((U032)(a))<<24)|(((U032)(r)&0xFF)<<16)|\
            (((U032)(g)&0xFF)<<8)|((U032)(b)&0xFF)))

/* Pack three 10-bit R,G,B values into NV_COLOR_FORMAT_LE_X2R10G10B10 */
#define PACK_RGB30(r,g,b)   \
            ((V032)((((U032)(r)&0x3FF)<<20)|\
            (((U032)(g)&0x3FF)<<10)|((U032)(b)&0x3FF)))

/* Pack 2-bit Alpha and 10-bit R,G,B values into NV_COLOR_FORMAT_LE_A2R10G10B10 */
#define PACK_ARGB30(a,r,g,b)    \
            ((V032)(((U032)(a)<<30)|(((U032)(r)&0x3FF)<<20)|\
            (((U032)(g)&0x3FF)<<10)|((U032)(b)&0x3FF)))

/***** Fractions *****/

/* Make unsigned 16.16 fraction from a whole number (integer or float) */
#define MAKE_FRACT16(num)   ((V032)((num)*(1<<16)))

/* Make unsigned 8.24 fraction from whole number (integer or float) */
#define MAKE_FRACT24(num)   ((V032)((num)*(1<<24)))

#endif /* NVMACROS_INCLUDED */
