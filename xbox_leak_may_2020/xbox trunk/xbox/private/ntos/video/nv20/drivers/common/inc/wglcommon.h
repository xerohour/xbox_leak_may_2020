#ifndef __WGLCOMMON_H_
#define __WGLCOMMON_H_

/*
 * Copyright (c) 1997, Nvidia Corporation.  All rights reserved.
 * Portions Copyright (c) 1996, 1997, Silicon Graphics Inc.
 */

extern int STDCALL __wglNVDescribePixelFormat(int bpp, 
                                              int iLayerPlane, 
                                              int iPixelFormat, 
                                              unsigned int nBytes,
                                              PIXELFORMATDESCRIPTOR *ppfd, 
                                              LAYERPLANEDESCRIPTOR *plpd,
                                              __WINpixelFormat *winPixelFormat,
                                              struct __WINNVpixelFormatFlagsRec *winPFFlags, 
                                              int showExtended,
                                              void *osother);

#endif  /* __WGLCOMMON_H_ */
