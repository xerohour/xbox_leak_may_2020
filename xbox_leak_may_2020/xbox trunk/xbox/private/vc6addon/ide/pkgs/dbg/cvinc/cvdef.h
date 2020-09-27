/***	cvdef.h - header for cv global symbol definitions.
 *
 *		Copyright <C> 1989, Microsoft Corporation
 *
 *		Purpose:
 *
 */


#include "nothunk.h"

#define CC
#define CV3
#define HELP_BUTTON

#ifdef DOS5
#define INCL_NOPM
#define _MAX_CVPATH	  259	  /* max. length of full pathname */
#define _MAX_CVDRIVE	3	  /* max. length of drive component */
#define _MAX_CVDIR	  257	  /* max. length of path component */
#define _MAX_CVFNAME  257	  /* max. length of file name component */
#define _MAX_CVEXT	  257	  /* max. length of extension component */

#else

#define _MAX_CVPATH	 144	  /* max. length of full pathname */
#define _MAX_CVDRIVE   3	  /* max. length of drive component */
#define _MAX_CVDIR	 130	  /* max. length of path component */
#define _MAX_CVFNAME  32      /* max. length of file name component */
#define _MAX_CVEXT	   5	  /* max. length of extension component */

#endif
