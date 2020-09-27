#ifndef _nvioctl_h_
#define _nvioctl_h_

/*
** Copyright 1991-1999, Silicon Graphics, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Silicon Graphics, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
** $Header$
*/

#include <sys/ioctl.h>

/*
** IOCTLs defines and structures common between the kernel and the GL
*/

/* ---  first the defines --- */
#define IOCTL_NV_OPENGL                         1

#define IOCTL_NV_OPENGL_GET_SINGLE_BACK_1       1
#define IOCTL_NV_OPENGL_GET_SINGLE_DEPTH        2
#define IOCTL_NV_OPENGL_FREE_SINGLE_BACK_DEPTH  3

/* --- now the structure definitions --- */

typedef struct NvOpenGLCommandRec {
    int 	command;
    void *      ret;
} NvOpenGLCommand;

#endif /* _nvioctl_h_ */

