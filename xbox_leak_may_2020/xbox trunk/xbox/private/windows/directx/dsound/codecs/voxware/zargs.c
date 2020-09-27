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
* Filename:        zargs.c
*
* Purpose:         command line argument parsing
*
* Functions:
*
* Author/Date:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/testcode/zargs.c_v   1.2   20 Jan 1999 15:30:18   weiwang  $
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
******************************************************************************/
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zargs.h"

#include "xvocver.h"

typedef int (*PFI)(char *value);
double atof();


static char *setarg(ARG *argp, char *value);
static ARG   *findarg(char *name, ARG *tabp, int tabsize);
static int  stoi(char *str);

/*----------------------------------------------------------------------
 setarg - Set an argument
   argp points at the argument table entry to bet set.
   value points at the desired value
--------------------------------------------------------------------------*/
static char *setarg(ARG *argp, char *value)
{
   switch(argp->type)
      {
      case INT:
         *(argp->variable) = stoi(value);
         break;
      case FLOAT:
         *((float *)(argp->variable)) = (float)atof(value);
         break;
      case DOUBLE:
         *((double *)(argp->variable)) = atof(value);
         break;
      case FLAG:
         *(argp->variable) = 1;
         break;
      case CHAR:
         *(argp->variable) = value[0];
         break;
      case STRING:
         *((char **)(argp->variable)) = value;
         break;
      case PROC:
         (*(PFI)(argp->variable)) (value); /* I cannot fix this warning !! (W.W.) */
         break;
      default:
         fprintf(stderr, "zargs: Bad Argument Type\n");
#ifdef EXITBUG
         exit(0);
#else
         exit(1);
#endif
   }
   return 0;
}


/*------------------------------------------------------------
 findarg - find argument
   Return pointer to argument table entry corresponting to 
   name (or NULL if name isn't in talble)
-----------------------------------------------------------*/
static    ARG   *findarg(char *name, ARG *tabp, int tabsize)
{
   for (; --tabsize >= 0; tabp++)
      if (strcmp(tabp->arg,name) == 0)
         return tabp;
   return (NULL);
}


/*-----------------------------------------------------------------
 zargs_prusage - print the argument table for user
   Prints in form -arg <type>   error_message (value: *variable)
----------------------------------------------------------------*/
void zargs_prusage(ARG *tabp, int tabsize, FILE *fp)
{
   for(;   --tabsize >= 0; tabp++)
   {
      switch(tabp->type)
      {
         case INT:
            fprintf(fp,"-%-12s <int>    %-40s (value: ",
               tabp->arg,tabp->errmsg);
            fprintf(fp,"%-5d)\n", *(tabp->variable));
            break;
         case FLOAT:
            fprintf(fp,"-%-12s <float>  %-40s (value: ",
               tabp->arg,tabp->errmsg);
            fprintf(fp,"%-5g)\n", 
               *(float *)(tabp->variable));
            break;
         case DOUBLE:
            fprintf(fp,"-%-12s <double> %-40s (value: ",
               tabp->arg,tabp->errmsg);
            fprintf(fp,"%-5g)\n", 
               *(double *)(tabp->variable));
            break;
         case FLAG:
            fprintf(fp,"-%-12s          %-40s (value: ",
               tabp->arg,tabp->errmsg);
            fprintf(fp,"%-5s)\n", 
               *(tabp->variable) ? "TRUE":"FALSE");
            break;
         case CHAR:
            fprintf(fp,"-%-12s <char>   %-40s (value: ",
               tabp->arg,tabp->errmsg);
            fprintf(fp,"%c)\n", *(tabp->variable) );
            break;
         case STRING:
            fprintf(fp,"-%-12s <str>    %-40s (value: ",
               tabp->arg,tabp->errmsg);
            fprintf(fp,"\"%s\")\n", 
               *(char **)tabp->variable );
            break;
         case PROC:
            fprintf(fp,"-%-12s <proc>   %-40s\n",
               tabp->arg,tabp->errmsg);
            break;
      }
   }
}

/*------------------------------------------------------------------------
 stoi - convert ascii string to integer ( enhances atoi)
   If string starts with 0x it is interpreted as hex, else
   if it starts with 0 it is octal, else it is decimal.
   Conversion stops on first character not a digit in the indicated radix.
--------------------------------------------------------------------------*/
#define islower(c) ('a'<= (c) && (c) <= 'z')
#define toupper(c) ( islower(c) ? (c) - ('a' - 'A') : (c) )

static int stoi(char *str)
{
   register int   num=0;
   int            sign=1;

   while(*str == ' ' || *str == '\t' || *str == '\n')
      str++;

   if (*str == '-')
   {
      sign = -1;
      str++;
   }
   if (*str == '0')
   {
      ++str;
      if (*str == 'x' || *str == 'X')
      {
         str++;
         while( ('0' <= *str && *str <= '9') ||
               ('a' <= *str && *str <= 'f') ||
               ('A' <= *str && *str <= 'F')   )
         {
            num *= 16;
            num += ('0' <= *str && *str <= '9') ?
                         *str - '0': toupper(*str) - 'A' + 10;
            str++;
         }
      }
      else
      {
         while ('0' <= *str && *str <= '7')
         {
            num *= 8;
            num += *str++ - '0';
         }
      }
   }
   else
   {
      while ('0' <= *str && *str <= '9')
      {
         num *= 10;
         num += *str++ - '0';
      }
   }
   return (num*sign);
}

/*----------------------------------------------------------------------
 zargs - process command line arguments
   Strips all command line switches out of argv. Return new argc.
   If an error is found exit(1) is called and a usage message is printed
   showing all arguments in the table 
--------------------------------------------------------------------------*/
/* Changed by MAZ as follows:
 *
 *    (1) command line switches must begin with a '-', but can be arbitrarily
 *      long.  
 *
 *   (2) the command line consists of the function name followed by
 *      (a) pairs of non-boolean switches and values
 *      (b) boolean switches   
 *      (c) other strings which get passed back to the calling program
 *      
 */
int   zargs(int argc, char **argv, ARG *tabp, int tabsize)
{
    register int   nargc;
    register char  **nargv;
    register ARG   *argp;
    char           *switch_name;
    char           *switch_val;

    nargc = 1;
    for (nargv = ++argv; --argc > 0; argv++) 
    {
       if ( **argv != '-' ) 
       {
          *nargv++ = *argv;
          nargc++;
       }
       else if (strcmp(*argv, "--") == 0) 
       {
          /* this is a way for the user to print the possible args */
          fprintf(stderr, "Possible arguments are:\n");
          zargs_prusage (tabp, tabsize, stderr);
          exit(0);
       }
       else 
       {
          switch_name=(*argv)+1; /* move past the minus */
          argp = findarg(switch_name, tabp, tabsize);
          if (argp == NULL) 
          {
             fprintf(stderr, 
                     "zargs: bad flag \"-%s\".\nAllowable flags are:\n", 
                     switch_name);
             zargs_prusage (tabp, tabsize, stderr);
#ifdef EXITBUG
             exit(0);
#else
             exit(1);
#endif
          }
          else if (argp->type == BOOLEAN) 
          {
             setarg(argp, NULL);
          }
          else 
          {
             --argc;
             if (argc <= 0) 
             {
                fprintf(stderr, 
                        "zargs: flag \"%s\" must have an argument\n",
                        switch_name);
             }
             else 
             {
                ++argv;
                switch_val = *argv;
                setarg(argp, switch_val);
             }
          }
       }
    }
    return nargc;
}
