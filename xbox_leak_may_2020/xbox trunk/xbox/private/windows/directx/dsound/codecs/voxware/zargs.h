/******************************************************************************
*
*                        Voxware Proprietary Material
*
*                        Copyright 1996, Voxware, Inc
*
*                            All Rights Reserved
*
*
*
*                       DISTRIBUTION PROHIBITED without
*
*                      written authorization from Voxware
*
******************************************************************************/
 
/******************************************************************************
*
* Filename:      zargs.h
*
* Purpose:
*
* Functions:
*
* Author/Date:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/testcode/zargs.h_v   1.1   20 Jan 1999 15:30:18   weiwang  $
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
******************************************************************************/
#ifndef _ZARGS_H
#define _ZARGS_H

#define INT       0
#define FLOAT     1
#define BOOLEAN   2
#define FLAG      BOOLEAN
#define CHAR      3
#define STRING    4
#define PROC      5
#define DOUBLE    6

typedef struct
{
   char       *arg;        /* command line switch */
   unsigned   type;        /* variable type */
   int        *variable;   /* pointer to variable */
   char       *errmsg;     /* pointer to error message */
}
ARG;

void   zargs_prusage(ARG *tabp, int tabsize, FILE *fp);
int    zargs(int argc, char **argv, ARG *tabp, int tabsize);


#endif /* _ZARGS_H */



