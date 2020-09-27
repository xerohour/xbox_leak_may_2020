/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:   P5FFT.c                                                                 
*                                                                              
* Purpose:    Fast FFT algorithm pulled out from Intel NSP library                                                                 
*                                                                              
* Functions:  Real FFT
*             Complex Conjugate FFT 
*             Complex FFT                                                                
*                                                                              
* Author/Date:  Xiangdong Fu (From Intel NSP)    12/17/96                                              
********************************************************************************
* Modifications:  
*                   
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/common.prj/intel.fld/P5FFT.c_v   1.3   02 Mar 1998 17:16:32   weiwang  $
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "vLibTran.h"
#include "vLibDef.h"

#include "xvocver.h"

#define V707107 ((float)0.707107)
#define SizeBlock 1024

#define  FFT_OrderSplit          11       /* max order for split algorithm  */
static SCplx *ctxFftTbl [32];        /***** static should be pointer based -- Toby ******/
static int   *ctxRevTbl [32];        /***** static should be pointer based -- Toby ******/

/*F*
//  Name:      nspcUndCalcFftTwdTbl_great
//  Purpose:   Computes twiddle factors for FFT computations (float).
//  Arguments:
//     tbl   - Pointer to the table to be filled with twiddle factors.
//     order - The base-2 logarithm of the length
//             of the twiddle factor table.
//  Calls:
//             cos,
//             sin.
*F*/

static void
nspcUndCalcFftTwdTbl_great(
   SCplx  *tbl,
   int     order)
{
   int     i,                         /*  loop index                */
           j,
           n,                         /*  pow(2,order)              */
           n2;                        /*  n/2                       */
   double  ang,                       /*  6.283185307179586 / n     */
           wpr,  wpi,                 /*  ( cos(ang),  -sin(ang) )  */
           wr,   wi,
           ang3,                      /*  ang * 3                   */
           wpr3, wpi3,                /*  ( cos(ang3), -sin(ang3) ) */
           wr3,  wi3,
           wtemp;

   n    = 1 << order;
   n2   = n/2;

   ang  = DB_PI/n;

   if ( order < FFT_OrderSplit )   {  /* ---- For Split algorithm --------- */

      wpr  =  cos(ang);
      wpi  = -sin(ang);
      ang3 = ang * 3.0;
      wpr3 =  cos(ang3);
      wpi3 = -sin(ang3);
      wr   = wpr;
      wi   = wpi;
      wr3  = wpr3;
      wi3  = wpi3;
      for ( i = 0 ; i < (n2-1) ; i+=2 ) {
         tbl[ i ].re = (float)wr;
         tbl[ i ].im = (float)wi;
                 wr  = (wtemp=wr)*wpr - wi*wpi;
                 wi  = wi*wpr + wtemp*wpi;
         tbl[i+1].re = (float)wr3;
         tbl[i+1].im = (float)wi3;
                 wr3 = (wtemp=wr3)*wpr3 - wi3*wpi3;
                 wi3 = wi3*wpr3 + wtemp*wpi3;
      }

   } else                          {  /* ---- For Radix algorithm --------- */

      for ( i = 0 ; i < n2 ; i++ ) {
          tbl[i].re = (float) cos(ang * i);
          tbl[i].im = (float)-sin(ang * i);
      }

   } /* if for algorithm type */

   /*  Compute the table of sin/cos for the trigonometric recombination  */

   /* Real Type */
   ang = PI/n;
   wpr = cos(ang);
   wpi = sin(ang);
   wr  = wpr;
   wi  = wpi;
   for ( i = n2 ; i < n ; i++ ) {
      tbl[i].re = (float)( wr*0.5 );
      tbl[i].im = (float)( -wi*0.5 + 0.5 );
             wr = (wtemp=wr)*wpr - wi*wpi;
             wi = wi*wpr + wtemp*wpi;
   }

   /* Ccs Type */
   for ( i = n, j = 0 ; i < (n+n2) ; i++, j++ ) {
       tbl[i].re = (float) cos(ang * j);
       tbl[i].im = (float)-sin(ang * j);
   }

   return;
}  /* nspcUndCalcFftTwdTbl_great */


/*F*
//  Name:      nspcUndGetFftTwdTbl_great
//  Purpose:   Allocates and fills a table for twiddle factors
//             for FFT computations (float).
//  Returns:   Pointer to the table of twiddle factors or NULL.
//  Argument:
//     order - The base-2 logarithm of the number of twiddle factors
//             in the table to be allocated.
//  Calls:
//             nspUndContext,
//             free,
//             calloc,
//             nspcUndCalcFftTwdTbl_great.
*F*/


static const SCplx*
nspcUndGetFftTwdTbl_great(
    int            order)
{
    int            length;
    SCplx         *A;
    if(!ctxFftTbl[order]){
       length = ( ( 1 << order ) * 3 )/2 + 4;
       A = (SCplx*) calloc(length,sizeof(SCplx));
       if ( !A ) {
          return(NULL);
       }
    }
    else { A = ctxFftTbl[order];  return(A); }

    nspcUndCalcFftTwdTbl_great(A,order);

    ctxFftTbl[order] = A;
    return(A);

} /* nspcUndGetFftTwdTbl_great */


 static void nspCalcBitRevTbl(
   int *tbl,
   int  order)
{
   int  n, i, j, k;

   n=1<<order;
   tbl[0]=0;
   tbl[n-1]=n-1;
   j = 0;
   for (i = 1 ; i < (n-1) ; i++) {
      k=n>>1;
      while (k<=j) {
         j=j-k;
         k=k>>1;
      }
      j=j+k;
      if (i<=j){
         tbl[j]=i;
         tbl[i]=j;
      }
   }
} /* nspCalcBitRevTbl */


const int* nspGetBitRevTbl(
    int           order)
{
    int          *ptbl;
    ptbl=ctxRevTbl[order];
    if (!ptbl) {
        ptbl=(int*) malloc((1<<order) *sizeof(int));
        if (!ptbl){
            ptbl=(int*) malloc((1<<order) *sizeof(int));
            return(NULL);
        } else {
           nspCalcBitRevTbl(ptbl,order);
           ctxRevTbl[order]=ptbl;
        }
    }
    return ptbl;
}

static void nspcbBitRev1(
    SCplx   *vec,
    int     order)
{
   typedef struct { long l0; long l1; } Memcb;
   Memcb  *vecm_00, *vecm_n0, *vecm_nn;
   long   temp;
   int    *tabrev;
   int    n, i, j;

#define CHANGE(mem1, mem2) \
    { temp = mem1.l0; mem1.l0 = mem2.l0; mem2.l0 = temp;     \
      temp = mem1.l1; mem1.l1 = mem2.l1; mem2.l1 = temp; };

     vecm_00 = (Memcb*) vec;
     if (order<3) {
       if (order>1) {
          CHANGE( vecm_00 [1], vecm_00 [2] );
        };
       return;
     };
     n = (1<<order);
     vecm_n0 = (Memcb*) vecm_00 + (n>>1);
     vecm_nn = (Memcb*) vecm_00 + (n>>1) + (n>>2);
     tabrev = (int*) nspGetBitRevTbl(order);
     for (i=0; i<=(n>>1)-4; i+=4) {
        j = tabrev[i];
        CHANGE( vecm_00 [i+1], vecm_n0 [j  ] );
        CHANGE( vecm_00 [i+3], vecm_nn [j  ] );
        if (i>j) {
           CHANGE( vecm_00 [i  ], vecm_00 [j  ] );
           CHANGE( vecm_n0 [i+1], vecm_n0 [j+1] );
        };
        j = tabrev[i+2];
        if (i+2>j) {
          CHANGE( vecm_00 [i+2], vecm_00 [j  ] );
          CHANGE( vecm_n0 [i+3], vecm_n0 [j+1] );
        };
    };
#undef CHANGE
}



#if P5_OPT == 0

/*F*
//  Name:       nspcUndSplit_FFT
//  Purpose:    Internal function.
//              Computes the forward or inverse fast Fourier transform (FFT)
//              of complex signal (in-place).
//  Arguments:
//       x   -  input and output Complex sequense of signal
//       n   -  length of signal
//       inv -  transform flag ( 1 - forward,  -1 - inverse )
//       w   -  table of twiddle factors
//  Calls:
//     nspcUndSplit_FFT_forward  -  for forward transform,
//     nspcUndSplit_FFT_inverse  -  for inverse transform.
*F*/

/*static float V707107 = (float)0.707107; */  /* 1/sqrt(2) */
#define V707107 ((float)0.707107)

static void
nspcUndSplit_FFT_forward(
   SCplx        *x,
   int           n,
   const SCplx  *w)
{
   int           k,
                 its, is, id, i1, it, j, l,
                 nn, n2, n4, n8;
   float         t1re, t1im, t2re, t2im, t3re, t3im;
   float        *x_1, *x_2, *x_3, *x_4, *ww;

   /* --------------- Main Loop --------------- */

   its = 4;
   n2  = n << 2;

   nn  = n << 1;

   x_1 = (float *)x;
   ww  = (float *)w;

   for  ( k = 2 ; k < (n >> 2) ; k = (k << 1) )  {

      is   = 0;
      id   = n2;
      n2   = n2 >> 1;
      n4   = n2 >> 2;
      n8   = n4 >> 1;

      x_2  = x_1 + n4;
      x_3  = x_2 + n4;
      x_4  = x_3 + n4;

      m10:
      for  ( i1 = is ; i1 < nn ; i1 += id )  {

         /* ............... Zero butterfly ............... */
         t1re         =  x_1[ i1   ]  -  x_3[ i1   ];
         t1im         =  x_1[ i1+1 ]  -  x_3[ i1+1 ];
         x_1[ i1   ]  =  x_1[ i1   ]  +  x_3[ i1   ];
         x_1[ i1+1 ]  =  x_1[ i1+1 ]  +  x_3[ i1+1 ];
         t2re         =  x_2[ i1   ]  -  x_4[ i1   ];
         t2im         =  x_2[ i1+1 ]  -  x_4[ i1+1 ];
         x_2[ i1   ]  =  x_2[ i1   ]  +  x_4[ i1   ];
         x_2[ i1+1 ]  =  x_2[ i1+1 ]  +  x_4[ i1+1 ];
         x_3[ i1   ]  =  t1re  + t2im;
         x_3[ i1+1 ]  =  t1im  - t2re;
         x_4[ i1   ]  =  t1re  - t2im;
         x_4[ i1+1 ]  =  t1im  + t2re;

         /* ...................... N/8 butterfly ...................... */
         l = i1 + n8;
         t1re        =  x_1[ l   ]  -  x_3[ l   ];
         t1im        =  x_1[ l+1 ]  -  x_3[ l+1 ];
         x_1[ l   ]  =  x_1[ l   ]  +  x_3[ l   ];
         x_1[ l+1 ]  =  x_1[ l+1 ]  +  x_3[ l+1 ];
         t2re        =  x_2[ l   ]  -  x_4[ l   ];
         t2im        =  x_2[ l+1 ]  -  x_4[ l+1 ];
         x_2[ l   ]  =  x_2[ l   ]  +  x_4[ l   ];
         x_2[ l+1 ]  =  x_2[ l+1 ]  +  x_4[ l+1 ];
         x_3[ l   ]  =  V707107*( (t1re + t2im) + (t1im - t2re) );
         x_3[ l+1 ]  =  V707107*( (t1im - t2re) - (t1re + t2im) );
         x_4[ l   ]  = -V707107*( (t1re - t2im) - (t1im + t2re) );
         x_4[ l+1 ]  = -V707107*( (t1im + t2re) + (t1re - t2im) );

         /* ...................... General butterfly ................... */
         for ( j=i1+2, it=its-4; j < l ; j+=2 , it+=its ) {
            t1re        =  x_1[ j   ]  -  x_3[ j   ];
            t1im        =  x_1[ j+1 ]  -  x_3[ j+1 ];
            x_1[ j   ]  =  x_1[ j   ]  +  x_3[ j   ];
            x_1[ j+1 ]  =  x_1[ j+1 ]  +  x_3[ j+1 ];
            t2im        =  x_2[ j   ]  -  x_4[ j   ];
            t2re        =  x_4[ j+1 ]  -  x_2[ j+1 ];
            x_2[ j   ]  =  x_2[ j   ]  +  x_4[ j   ];
            x_2[ j+1 ]  =  x_2[ j+1 ]  +  x_4[ j+1 ];
            x_3[ j   ]  =  (t1re - t2re )*ww[ it   ]  -
                           (t1im - t2im )*ww[ it+1 ];
            x_3[ j+1 ]  =  (t1im - t2im )*ww[ it   ]  +
                           (t1re - t2re )*ww[ it+1 ];
            x_4[ j   ]  =  (t1re + t2re )*ww[ it+2 ] -
                           (t1im + t2im )*ww[ it+3 ];
            x_4[ j+1 ]  =  (t1im + t2im )*ww[ it+2 ]  +
                           (t1re + t2re )*ww[ it+3 ];
         }
         for ( j=i1+n4-2, it=its-4; j > l ; j-=2 , it+=its ) {
            t1re        =  x_1[ j   ]  -  x_3[ j   ];
            t1im        =  x_1[ j+1 ]  -  x_3[ j+1 ];
            x_1[ j   ]  =  x_1[ j   ]  +  x_3[ j   ];
            x_1[ j+1 ]  =  x_1[ j+1 ]  +  x_3[ j+1 ];
            t2im        =  x_2[ j   ]  -  x_4[ j   ];
            t2re        =  x_4[ j+1 ]  -  x_2[ j+1 ];
            x_2[ j   ]  =  x_2[ j   ]  +  x_4[ j   ];
            x_2[ j+1 ]  =  x_2[ j+1 ]  +  x_4[ j+1 ];
            x_3[ j   ]  =  (t2re - t1re )*ww[ it+1 ]  -
                           (t2im - t1im )*ww[ it   ];
            x_3[ j+1 ]  =  (t2im - t1im )*ww[ it+1 ]  +
                           (t2re - t1re )*ww[ it   ];
            x_4[ j   ]  =  (t2re + t1re )*ww[ it+3 ]  -
                           (t2im + t1im )*ww[ it+2 ];
            x_4[ j+1 ]  =  (t2im + t1im )*ww[ it+3 ]  +
                           (t2re + t1re )*ww[ it+2 ];
         }

      }  /*  Loop  i1  end  */

      /* control */
      is = (id << 1) - n2;
      id =  id << 2 ;
      if ( is < nn ) goto m10;

      its = its << 1;

   }  /* --------  Main Loop  k  end  ----------- */

   is  = 0;
   id  = n2;
   n2  = n2 >> 1;

   x_2 = x_1 + 4;
   x_3 = x_2 + 4;
   x_4 = x_3 + 4;

   m20:
   for ( i1 = is, j = is + 2 ; i1 < nn ; i1+=id, j+=id ) {
      /* ............... Zero butterfly .............. */
      t1re         =  x_1[ i1   ]  -  x_3[ i1   ];
      t1im         =  x_1[ i1+1 ]  -  x_3[ i1+1 ];
      x_1[ i1   ]  =  x_1[ i1   ]  +  x_3[ i1   ];
      x_1[ i1+1 ]  =  x_1[ i1+1 ]  +  x_3[ i1+1 ];
      t2re         =  x_2[ i1   ]  -  x_4[ i1   ];
      t2im         =  x_2[ i1+1 ]  -  x_4[ i1+1 ];
      x_2[ i1   ]  =  x_2[ i1   ]  +  x_4[ i1   ];
      x_2[ i1+1 ]  =  x_2[ i1+1 ]  +  x_4[ i1+1 ];
      x_3[ i1   ]  =  t1re  +  t2im;
      x_3[ i1+1 ]  =  t1im  -  t2re;
      x_4[ i1   ]  =  t1re  -  t2im;
      x_4[ i1+1 ]  =  t1im  +  t2re;
      /* ............. N/8 butterfly ................. */
      t1re         =  x_1[ j   ]  -  x_3[ j   ];
      t1im         =  x_1[ j+1 ]  -  x_3[ j+1 ];
      x_1[ j   ]   =  x_1[ j   ]  +  x_3[ j   ];
      x_1[ j+1 ]   =  x_1[ j+1 ]  +  x_3[ j+1 ];
      t2re         =  x_2[ j   ]  -  x_4[ j   ];
      t2im         =  x_2[ j+1 ]  -  x_4[ j+1 ];
      x_2[ j   ]   =  x_2[ j   ]  +  x_4[ j   ];
      x_2[ j+1 ]   =  x_2[ j+1 ]  +  x_4[ j+1 ];
      x_3[ j   ]   =  V707107*( (t1re + t2im) + (t1im - t2re) );
      x_3[ j+1 ]   =  V707107*( (t1im - t2re) - (t1re + t2im) );
      x_4[ j   ]   = -V707107*( (t1re - t2im) - (t1im + t2re) );
      x_4[ j+1 ]   = -V707107*( (t1im + t2re) + (t1re - t2im) );
   }
   is = ( id << 1 ) - n2;
   id =  id << 2 ;
   if ( is < nn ) goto m20;

   if ( n == 8 ) {
      is =  0;
      id = 16;
      goto m30;
   }

   for ( i1 = 0 ; i1 < nn ; ) {

      t1re          =           x_1[ i1+ 8 ];
      t1im          =           x_1[ i1+ 9 ];
      x_1[ i1+ 8 ]  =  t1re  +  x_1[ i1+10 ];
      x_1[ i1+ 9 ]  =  t1im  +  x_1[ i1+11 ];
      x_1[ i1+10 ]  =  t1re  -  x_1[ i1+10 ];
      x_1[ i1+11 ]  =  t1im  -  x_1[ i1+11 ];
      t2re          =           x_1[ i1+12 ];
      t2im          =           x_1[ i1+13 ];
      x_1[ i1+12 ]  =  t2re  +  x_1[ i1+14 ];
      x_1[ i1+13 ]  =  t2im  +  x_1[ i1+15 ];
      x_1[ i1+14 ]  =  t2re  -  x_1[ i1+14 ];
      x_1[ i1+15 ]  =  t2im  -  x_1[ i1+15 ];

      j  = i1;
      i1 = i1 + 32;
      for ( ; j < i1 ; j+=16 ) {
         t1re        =            x_1[ j   ]  -  x_1[ j+4 ];
         t1im        =            x_1[ j+1 ]  -  x_1[ j+5 ];
         t3re        =            x_1[ j   ]  +  x_1[ j+4 ];
         t3im        =            x_1[ j+1 ]  +  x_1[ j+5 ];
         x_1[ j   ]  =  t3re  +  (x_1[ j+2 ]  +  x_1[ j+6 ]);
         x_1[ j+1 ]  =  t3im  +  (x_1[ j+3 ]  +  x_1[ j+7 ]);
         t2re        =            x_1[ j+2 ]  -  x_1[ j+6 ];
         t2im        =            x_1[ j+3 ]  -  x_1[ j+7 ];
         x_1[ j+2 ]  =  t3re  -  (x_1[ j+2 ]  +  x_1[ j+6 ]);
         x_1[ j+3 ]  =  t3im  -  (x_1[ j+3 ]  +  x_1[ j+7 ]);
         x_1[ j+4 ]  =  t1re  +  t2im;
         x_1[ j+5 ]  =  t1im  -  t2re;
         x_1[ j+6 ]  =  t1re  -  t2im;
         x_1[ j+7 ]  =  t1im  +  t2re;
      }
   }

   is = 24;
   id = 64;
   m30:
   for ( i1 = is ; i1 < nn ; i1+=id ) {
      t1re         =            x_1[ i1   ]  -  x_1[ i1+4 ];
      t1im         =            x_1[ i1+1 ]  -  x_1[ i1+5 ];
      t2re         =            x_1[ i1+2 ]  -  x_1[ i1+6 ];
      t2im         =            x_1[ i1+3 ]  -  x_1[ i1+7 ];
      t3re         =            x_1[ i1   ]  +  x_1[ i1+4 ];
      t3im         =            x_1[ i1+1 ]  +  x_1[ i1+5 ];
      x_1[ i1   ]  =  t3re  +  (x_1[ i1+2 ]  +  x_1[ i1+6 ]);
      x_1[ i1+1 ]  =  t3im  +  (x_1[ i1+3 ]  +  x_1[ i1+7 ]);
      x_1[ i1+2 ]  =  t3re  -  (x_1[ i1+2 ]  +  x_1[ i1+6 ]);
      x_1[ i1+3 ]  =  t3im  -  (x_1[ i1+3 ]  +  x_1[ i1+7 ]);
      x_1[ i1+4 ]  =  t1re  +  t2im;
      x_1[ i1+5 ]  =  t1im  -  t2re;
      x_1[ i1+6 ]  =  t1re  -  t2im;
      x_1[ i1+7 ]  =  t1im  +  t2re;
   }
   is = (id << 1) - 8;
   id =  id << 2;
   if ( is < nn ) goto m30;

   if ( n == 8 ) {
      for ( i1 = 8 ; i1 < 16 ; i1+=4 ) {
         t1re         =           x_1[ i1   ];
         t1im         =           x_1[ i1+1 ];
         x_1[ i1   ]  =  t1re  +  x_1[ i1+2 ];
         x_1[ i1+1 ]  =  t1im  +  x_1[ i1+3 ];
         x_1[ i1+2 ]  =  t1re  -  x_1[ i1+2 ];
         x_1[ i1+3 ]  =  t1im  -  x_1[ i1+3 ];
      }
   }

   is =  56;
   id = 128;
   m40:
   for ( i1 = is ; i1 < nn ; i1+=id ) {
      t1re         =           x_1[ i1   ];
      t1im         =           x_1[ i1+1 ];
      x_1[ i1   ]  =  t1re  +  x_1[ i1+2 ];
      x_1[ i1+1 ]  =  t1im  +  x_1[ i1+3 ];
      x_1[ i1+2 ]  =  t1re  -  x_1[ i1+2 ];
      x_1[ i1+3 ]  =  t1im  -  x_1[ i1+3 ];
      t1re         =           x_1[ i1+4 ];
      t1im         =           x_1[ i1+5 ];
      x_1[ i1+4 ]  =  t1re  +  x_1[ i1+6 ];
      x_1[ i1+5 ]  =  t1im  +  x_1[ i1+7 ];
      x_1[ i1+6 ]  =  t1re  -  x_1[ i1+6 ];
      x_1[ i1+7 ]  =  t1im  -  x_1[ i1+7 ];
   }
   is = (id << 1) - 8;
   id =  id << 2;
   if ( is < nn ) goto m40;

   return;
}  /* nspcUndSplit_FFT_forward */


static void
nspcUndSplit_FFT_inverse(
   SCplx        *x,
   int           n,
   const SCplx  *w)
{
   int           k,
                 its, is, id, i1, it, j,
                 n2, n4, n8, n4_2, n4_3;
   float         t1re, t1im, t2re, t2im, t3re, t3im;

   /* --------------- Main Loop --------------- */

   its = 2;
   n2  = n << 1;

   for  ( k = 2 ; k < (n >> 2) ; k = (k << 1) )  {

      is   = 0;
      id   = n2;
      n2   = n2 >> 1;
      n4   = n2 >> 2;
      n8   = n4 >> 1;
      n4_2 = n4 << 1;
      n4_3 = n4_2 + n4;

      m10:
      for  ( i1 = is ; i1 < n ; i1 += id )  {

         /* ............... Zero butterfly ............... */
         t1re            = x[ i1 ].re    - x[ n4_2 + i1 ].re;
         t1im            = x[ i1 ].im    - x[ n4_2 + i1 ].im;
         x[ i1 ].re      = x[ i1 ].re    + x[ n4_2 + i1 ].re;
         x[ i1 ].im      = x[ i1 ].im    + x[ n4_2 + i1 ].im;
         t2re            = x[ n4+i1 ].re - x[ n4_3 + i1 ].re;
         t2im            = x[ n4+i1 ].im - x[ n4_3 + i1 ].im;
         x[ n4+i1 ].re   = x[ n4+i1 ].re + x[ n4_3 + i1 ].re;
         x[ n4+i1 ].im   = x[ n4+i1 ].im + x[ n4_3 + i1 ].im;
         x[ n4_2+i1 ].re = t1re - t2im;
         x[ n4_2+i1 ].im = t1im + t2re;
         x[ n4_3+i1 ].re = t1re + t2im;
         x[ n4_3+i1 ].im = t1im - t2re;

         /* ...................... N/8 butterfly ...................... */
         t1re                 = x[ i1+n8 ].re      - x[ n4_2 + i1+n8 ].re;
         t1im                 = x[ i1+n8 ].im      - x[ n4_2 + i1+n8 ].im;
         x[ i1+n8 ].re        = x[ i1+n8 ].re      + x[ n4_2 + i1+n8 ].re;
         x[ i1+n8 ].im        = x[ i1+n8 ].im      + x[ n4_2 + i1+n8 ].im;
         t2re                 = x[ n4 + i1+n8 ].re - x[ n4_3 + i1+n8 ].re;
         t2im                 = x[ n4 + i1+n8 ].im - x[ n4_3 + i1+n8 ].im;
         x[ n4 + i1+n8 ].re   = x[ n4 + i1+n8 ].re + x[ n4_3 + i1+n8 ].re;
         x[ n4 + i1+n8 ].im   = x[ n4 + i1+n8 ].im + x[ n4_3 + i1+n8 ].im;
         x[ n4_2 + i1+n8 ].re =  V707107*( (t1re - t2im) - (t1im + t2re) );
         x[ n4_2 + i1+n8 ].im =  V707107*( (t1im + t2re) + (t1re - t2im) );
         x[ n4_3 + i1+n8 ].re = -V707107*( (t1re + t2im) + (t1im - t2re) );
         x[ n4_3 + i1+n8 ].im = -V707107*( (t1im - t2re) - (t1re + t2im) );

         /* ...................... General butterfly ................... */
         it = -1;
         for ( j = i1+1 ; j < i1+n8 ; j++ ) {
            it = it + its;
            t1re             = x[ j ].re        - x[ n4_2 + j ].re;
            t1im             = x[ j ].im        - x[ n4_2 + j ].im;
            x[ j ].re        = x[ j ].re        + x[ n4_2 + j ].re;
            x[ j ].im        = x[ j ].im        + x[ n4_2 + j ].im;
            t2im             = x[ n4 + j ].re   - x[ n4_3 + j ].re;
            t2re             = x[ n4_3 + j ].im - x[ n4 + j ].im;
            x[ n4 + j ].re   = x[ n4 + j ].re   + x[ n4_3 + j ].re;
            x[ n4 + j ].im   = x[ n4 + j ].im   + x[ n4_3 + j ].im;
            x[ n4_2 + j ].re =  (t1re + t2re )*w[ it-1 ].re +
                                (t1im + t2im )*w[ it-1 ].im;
            x[ n4_2 + j ].im =  (t1im + t2im )*w[ it-1 ].re -
                                (t1re + t2re )*w[ it-1 ].im;
            x[ n4_3 + j ].re =  (t1re - t2re )*w[  it  ].re +
                                (t1im - t2im )*w[  it  ].im;
            x[ n4_3 + j ].im =  (t1im - t2im )*w[  it  ].re -
                                (t1re - t2re )*w[  it  ].im;
         }
         it = -1;
         for ( j = i1+n4-1 ; j > i1+n8 ; j-- ) {
            it = it + its;
            t1re             = x[ j ].re        - x[ n4_2 + j ].re;
            t1im             = x[ j ].im        - x[ n4_2 + j ].im;
            x[ j ].re        = x[ j ].re        + x[ n4_2 + j ].re;
            x[ j ].im        = x[ j ].im        + x[ n4_2 + j ].im;
            t2im             = x[ n4 + j ].re   - x[ n4_3 + j ].re;
            t2re             = x[ n4_3 + j ].im - x[ n4 + j ].im;
            x[ n4 + j ].re   = x[ n4 + j ].re   + x[ n4_3 + j ].re;
            x[ n4 + j ].im   = x[ n4 + j ].im   + x[ n4_3 + j ].im;
            x[ n4_2 + j ].re = -(t2re + t1re )*w[ it-1 ].im -
                                (t2im + t1im )*w[ it-1 ].re;
            x[ n4_2 + j ].im =  (t2re + t1re )*w[ it-1 ].re -
                                (t2im + t1im )*w[ it-1 ].im;
            x[ n4_3 + j ].re =  (t1re - t2re )*w[  it  ].im +
                                (t1im - t2im )*w[  it  ].re;
            x[ n4_3 + j ].im =  (t1im - t2im )*w[  it  ].im -
                                (t1re - t2re )*w[  it  ].re;
         }

      }  /*  Loop  i1  end  */

      /* control */
      is = (id << 1) - n2;
      id =  id << 2 ;
      if ( is < n ) goto m10;

      its = its << 1;

   }  /* --------  Main Loop  k  end  ----------- */

   is = 0;
   id = n2;
   n2 = n2 >> 1;

   m20:
   for ( i1 = is ; i1 < n ; i1+=id ) {

      /* ............... Zero butterfly .............. */
      t1re           = x[     i1 ].re  -  x[ 4 + i1 ].re;
      t1im           = x[     i1 ].im  -  x[ 4 + i1 ].im;
      x[ i1 ].re     = x[     i1 ].re  +  x[ 4 + i1 ].re;
      x[ i1 ].im     = x[     i1 ].im  +  x[ 4 + i1 ].im;
      t2re           = x[ 2 + i1 ].re  -  x[ 6 + i1 ].re;
      t2im           = x[ 2 + i1 ].im  -  x[ 6 + i1 ].im;
      x[ 2 + i1 ].re = x[ 2 + i1 ].re  +  x[ 6 + i1 ].re;
      x[ 2 + i1 ].im = x[ 2 + i1 ].im  +  x[ 6 + i1 ].im;
      x[ 4 + i1 ].re = t1re - t2im;
      x[ 4 + i1 ].im = t1im + t2re;
      x[ 6 + i1 ].re = t1re + t2im;
      x[ 6 + i1 ].im = t1im - t2re;

      /* ............. N/8 butterfly ................. */
      t1re           = x[ 1 + i1 ].re  -  x[ 5 + i1 ].re;
      t1im           = x[ 1 + i1 ].im  -  x[ 5 + i1 ].im;
      x[ 1 + i1 ].re = x[ 1 + i1 ].re  +  x[ 5 + i1 ].re;
      x[ 1 + i1 ].im = x[ 1 + i1 ].im  +  x[ 5 + i1 ].im;
      t2re           = x[ 3 + i1 ].re  -  x[ 7 + i1 ].re;
      t2im           = x[ 3 + i1 ].im  -  x[ 7 + i1 ].im;
      x[ 3 + i1 ].re = x[ 3 + i1 ].re  +  x[ 7 + i1 ].re;
      x[ 3 + i1 ].im = x[ 3 + i1 ].im  +  x[ 7 + i1 ].im;
      x[ 5 + i1 ].re =  V707107*( (t1re - t2im) - (t1im + t2re) );
      x[ 5 + i1 ].im =  V707107*( (t1im + t2re) + (t1re - t2im) );
      x[ 7 + i1 ].re = -V707107*( (t1re + t2im) + (t1im - t2re) );
      x[ 7 + i1 ].im = -V707107*( (t1im - t2re) - (t1re + t2im) );

   }
   is = ( id << 1 ) - n2;
   id =  id << 2 ;
   if ( is < n ) goto m20;

   if ( n == 8 ) {
      is = 0;
      id = 8;
      goto m30;
   }

   for ( i1 = 0 ; i1 < n ; i1+=16 ) {

      t1re           =         x[     i1 ].re  -  x[ 2 + i1 ].re;
      t1im           =         x[     i1 ].im  -  x[ 2 + i1 ].im;
      t2re           =         x[ 1 + i1 ].re  -  x[ 3 + i1 ].re;
      t2im           =         x[ 1 + i1 ].im  -  x[ 3 + i1 ].im;
      t3re           =         x[     i1 ].re  +  x[ 2 + i1 ].re;
      t3im           =         x[     i1 ].im  +  x[ 2 + i1 ].im;
      x[     i1 ].re = t3re + (x[ 1 + i1 ].re  +  x[ 3 + i1 ].re);
      x[     i1 ].im = t3im + (x[ 1 + i1 ].im  +  x[ 3 + i1 ].im);
      x[ 1 + i1 ].re = t3re - (x[ 1 + i1 ].re  +  x[ 3 + i1 ].re);
      x[ 1 + i1 ].im = t3im - (x[ 1 + i1 ].im  +  x[ 3 + i1 ].im);
      x[ 2 + i1 ].re = t1re - t2im;
      x[ 2 + i1 ].im = t1im + t2re;
      x[ 3 + i1 ].re = t1re + t2im;
      x[ 3 + i1 ].im = t1im - t2re;

      t1re           =          x[ 4 + i1 ].re;
      t1im           =          x[ 4 + i1 ].im;
      x[ 4 + i1 ].re = t1re  +  x[ 5 + i1 ].re;
      x[ 4 + i1 ].im = t1im  +  x[ 5 + i1 ].im;
      x[ 5 + i1 ].re = t1re  -  x[ 5 + i1 ].re;
      x[ 5 + i1 ].im = t1im  -  x[ 5 + i1 ].im;
      t2re           =          x[ 6 + i1 ].re;
      t2im           =          x[ 6 + i1 ].im;
      x[ 6 + i1 ].re = t2re  +  x[ 7 + i1 ].re;
      x[ 6 + i1 ].im = t2im  +  x[ 7 + i1 ].im;
      x[ 7 + i1 ].re = t2re  -  x[ 7 + i1 ].re;
      x[ 7 + i1 ].im = t2im  -  x[ 7 + i1 ].im;

      t1re            =           x[ 8 + i1 ].re  -  x[ 10 + i1 ].re;
      t1im            =           x[ 8 + i1 ].im  -  x[ 10 + i1 ].im;
      t2re            =           x[ 9 + i1 ].re  -  x[ 11 + i1 ].re;
      t2im            =           x[ 9 + i1 ].im  -  x[ 11 + i1 ].im;
      t3re            =           x[ 8 + i1 ].re  +  x[ 10 + i1 ].re;
      t3im            =           x[ 8 + i1 ].im  +  x[ 10 + i1 ].im;
      x[  8 + i1 ].re = t3re  +  (x[ 9 + i1 ].re  +  x[ 11 + i1 ].re);
      x[  8 + i1 ].im = t3im  +  (x[ 9 + i1 ].im  +  x[ 11 + i1 ].im);
      x[  9 + i1 ].re = t3re  -  (x[ 9 + i1 ].re  +  x[ 11 + i1 ].re);
      x[  9 + i1 ].im = t3im  -  (x[ 9 + i1 ].im  +  x[ 11 + i1 ].im);
      x[ 10 + i1 ].re = t1re - t2im;
      x[ 10 + i1 ].im = t1im + t2re;
      x[ 11 + i1 ].re = t1re + t2im;
      x[ 11 + i1 ].im = t1im - t2re;

   }

   is = 12;
   id = 32;

   m30:
   for ( i1 = is ; i1 < n ; i1+=id ) {
      t1re           =           x[     i1 ].re  -  x[ 2 + i1 ].re;
      t1im           =           x[     i1 ].im  -  x[ 2 + i1 ].im;
      t2re           =           x[ 1 + i1 ].re  -  x[ 3 + i1 ].re;
      t2im           =           x[ 1 + i1 ].im  -  x[ 3 + i1 ].im;
      t3re           =           x[     i1 ].re  +  x[ 2 + i1 ].re;
      t3im           =           x[     i1 ].im  +  x[ 2 + i1 ].im;
      x[     i1 ].re = t3re  +  (x[ 1 + i1 ].re  +  x[ 3 + i1 ].re);
      x[     i1 ].im = t3im  +  (x[ 1 + i1 ].im  +  x[ 3 + i1 ].im);
      x[ 1 + i1 ].re = t3re  -  (x[ 1 + i1 ].re  +  x[ 3 + i1 ].re);
      x[ 1 + i1 ].im = t3im  -  (x[ 1 + i1 ].im  +  x[ 3 + i1 ].im);
      x[ 2 + i1 ].re = t1re - t2im;
      x[ 2 + i1 ].im = t1im + t2re;
      x[ 3 + i1 ].re = t1re + t2im;
      x[ 3 + i1 ].im = t1im - t2re;
   }
   is = (id << 1) - 4;
   id =  id << 2;
   if ( is < n ) goto m30;

   if ( n == 8 ) {
      i1 = 4;
      t1re           =          x[     i1 ].re;
      t1im           =          x[     i1 ].im;
      x[     i1 ].re = t1re  +  x[ 1 + i1 ].re;
      x[     i1 ].im = t1im  +  x[ 1 + i1 ].im;
      x[ 1 + i1 ].re = t1re  -  x[ 1 + i1 ].re;
      x[ 1 + i1 ].im = t1im  -  x[ 1 + i1 ].im;
      t2re           =          x[ 2 + i1 ].re;
      t2im           =          x[ 2 + i1 ].im;
      x[ 2 + i1 ].re = t2re  +  x[ 3 + i1 ].re;
      x[ 2 + i1 ].im = t2im  +  x[ 3 + i1 ].im;
      x[ 3 + i1 ].re = t2re  -  x[ 3 + i1 ].re;
      x[ 3 + i1 ].im = t2im  -  x[ 3 + i1 ].im;
   }

   is = 28;
   id = 64;
   m40:
   for ( i1 = is; i1 < n ; i1+=id ) {
      t1re           =          x[     i1 ].re;
      t1im           =          x[     i1 ].im;
      x[     i1 ].re = t1re  +  x[ 1 + i1 ].re;
      x[     i1 ].im = t1im  +  x[ 1 + i1 ].im;
      x[ 1 + i1 ].re = t1re  -  x[ 1 + i1 ].re;
      x[ 1 + i1 ].im = t1im  -  x[ 1 + i1 ].im;
      t2re           =          x[ 2 + i1 ].re;
      t2im           =          x[ 2 + i1 ].im;
      x[ 2 + i1 ].re = t2re  +  x[ 3 + i1 ].re;
      x[ 2 + i1 ].im = t2im  +  x[ 3 + i1 ].im;
      x[ 3 + i1 ].re = t2re  -  x[ 3 + i1 ].re;
      x[ 3 + i1 ].im = t2im  -  x[ 3 + i1 ].im;
   }
   is = (id << 1) - 4;
   id =  id << 2;
   if ( is < n ) goto m40;

   return;
}  /* nspcUndSplit_FFT_inverse */



static void
nspcUndSplit_FFT(
   SCplx        *x,
   int           n,
   int           inv,
   const SCplx  *w)
{
   SCplx         t1, t2, t3;

   if ( n < 8 ) {           /* -------- easy cases ---------- */
      if ( n > 2 ) {        /*  .........  n = 4  ..........  */
            t1.re      =             x[ 0 ].re  -  x[ 2 ].re;
            t1.im      =             x[ 0 ].im  -  x[ 2 ].im;
            t2.re      =             x[ 1 ].re  -  x[ 3 ].re;
            t2.im      =             x[ 1 ].im  -  x[ 3 ].im;
            t3.re      =             x[ 0 ].re  +  x[ 2 ].re;
            t3.im      =             x[ 0 ].im  +  x[ 2 ].im;
            x[ 0 ].re  =  t3.re  +  (x[ 1 ].re  +  x[ 3 ].re);
            x[ 0 ].im  =  t3.im  +  (x[ 1 ].im  +  x[ 3 ].im);
            x[ 1 ].re  =  t3.re  -  (x[ 1 ].re  +  x[ 3 ].re);
            x[ 1 ].im  =  t3.im  -  (x[ 1 ].im  +  x[ 3 ].im);
         if ( inv > 0 ) {   /*  forward  transform  */
            x[ 2 ].re  =  t1.re  +  t2.im;
            x[ 2 ].im  =  t1.im  -  t2.re;
            x[ 3 ].re  =  t1.re  -  t2.im;
            x[ 3 ].im  =  t1.im  +  t2.re;
         }  else        {   /*  inverse  transform  */
            x[ 2 ].re  =  t1.re  -  t2.im;
            x[ 2 ].im  =  t1.im  +  t2.re;
            x[ 3 ].re  =  t1.re  +  t2.im;
            x[ 3 ].im  =  t1.im  -  t2.re;
         }
      }
      else {
         if ( n > 1 ) {     /*  .........  n = 2  ..........  */
            t1         =            x[ 0 ];
            x[ 0 ].re  =  t1.re  +  x[ 1 ].re;
            x[ 0 ].im  =  t1.im  +  x[ 1 ].im;
            x[ 1 ].re  =  t1.re  -  x[ 1 ].re;
            x[ 1 ].im  =  t1.im  -  x[ 1 ].im;
         }
      }
      return;
   }                        /* ----- end for easy cases ----- */

   if ( inv > 0 ) {         /*  forward  transform  */
      nspcUndSplit_FFT_forward( x, n, w);
   }  else        {         /*  inverse  transform  */
      nspcUndSplit_FFT_inverse( x, n, w);
   }
   return;
} /* nspcUndSplit_FFT */

#else
extern void
nspcUndSplit_FFT(
   SCplx        *x,
   int           n,
   int           inv,
   const SCplx  *w);

#endif


#if P5_OPT == 0
/*F*
//  Name:       nspcUndRadix2_FFT
//  Purpose:    Internal function.
//              Computes the forward or inverse fast Fourier transform (FFT)
//              of complex signal (in-place).
//  Arguments:
//        d   -  input and output Complex sequense of signal
//        n   -  length of signal
//        inv -  transform flag ( 1 - forward,  -1 - inverse )
//        w   -  table of twiddle factors
*F*/

#define SizeBlock 1024
static void
nspcUndRadix2_FFT(
  SCplx       *d,
  int          n,
  int          inv,
  const SCplx *w)
{
  int     nn=0, m;
  int     mmax=0, k, k1, k2, j1, i1, i2, i3, i4;
  int     kk, kk1, kk2, ki, kii, ki1, ki2, ik1=0, ik2;
  int     j, istep, i, nj, kj, kkj, kkj1, kkj2;
  SCplx   t, t1, temp, temp1, t01, t11;

  if ( n < 1 ) return;

  if ( n == 2 ) {
      temp.re  = d[0].re - d[1].re;
      temp.im  = d[0].im - d[1].im;
      d[0].re  = d[0].re + d[1].re;
      d[0].im  = d[0].im + d[1].im;
      d[1].re  = temp.re;
      d[1].im  = temp.im;
      return;
  }

  m = n<SizeBlock ? n : SizeBlock;
  if (inv<0) goto inverse;

  for (j1=0; j1<n; j1+=m) {

    kk  = j1-3;
    kk1 = j1-2;
    kk2 = j1-1;
    for (j=j1; j<j1+m; j+=4) {

      kk2 += 4;
      kk  += 4;
      kk1 += 4;

      temp.re  = d[j].re - d[kk].re;
      temp1.re = d[kk1].im - d[kk2].im;
      temp.im  = d[j].im - d[kk].im;
      temp1.im = d[kk2].re - d[kk1].re;
      d[j].re   = (t.re  = d[j].re + d[kk].re)
                + (t1.re = d[kk1].re + d[kk2].re);
      d[j].im   = (t.im  = d[j].im + d[kk].im)
                + (t1.im = d[kk1].im + d[kk2].im);
      d[kk].re  = temp.re + temp1.re;
      d[kk].im  = temp.im + temp1.im;
      d[kk1].re = t.re - t1.re;
      d[kk1].im = t.im - t1.im;
      d[kk2].re = temp.re - temp1.re;
      d[kk2].im = temp.im - temp1.im;
    } /* for */
    nn = 4;
    mmax = 8;
    istep = 16;
    if (nn==n) { return; }
    ik1 = n>>3;
    ik2 = n>>4;
 nextBlock:

    if (istep>n) goto label5;
    kk2 = (kk = j1+nn-istep) + mmax;
    kk1 = j1+mmax-istep;
    nj = nn>>1;
    for (j=j1; j<j1+m; j+=istep) {
      kk2 += istep;
      kk  += istep;
      kk1 += istep;

      temp.re  = d[j].re - d[kk].re;
      temp.im  = d[j].im - d[kk].im;
      temp1.re = d[kk1].im - d[kk2].im;
      temp1.im = d[kk2].re - d[kk1].re;

      t.re      = d[j].re + d[kk].re;
      t1.re     = d[kk1].re + d[kk2].re;
      t.im      = d[j].im + d[kk].im;
      t1.im     = d[kk1].im + d[kk2].im;
      d[j].re   = t.re + t1.re;
      d[j].im   = t.im + t1.im;

      d[kk].re  = temp.re + temp1.re;
      d[kk].im  = temp.im + temp1.im;
      d[kk1].re = t.re - t1.re;
      d[kk1].im = t.im - t1.im;
      d[kk2].re = temp.re - temp1.re;
      d[kk2].im = temp.im - temp1.im;

      kj   = j+nj;
      kkj2 = kk2+nj;
      kkj  = kk+nj;
      kkj1 = kk1+nj;

      temp.re  = d[kj].re - d[kkj].im;  /* tkk */
      temp.im  = d[kj].im + d[kkj].re;
      t.re  = d[kj].re + d[kkj].im;     /* tk  */
      t.im  = d[kj].im - d[kkj].re;

      temp1.re = d[kkj1].re - d[kkj2].im;     /* tk2 */
      temp1.im = d[kkj1].im + d[kkj2].re;
      t1.re = d[kkj1].re + d[kkj2].im;        /* tk1 */
      t1.im = d[kkj1].im - d[kkj2].re;

      t11.re =-V707107*(temp1.re-temp1.im); /* tr1 */
      t11.im =-V707107*(temp1.im+temp1.re);
      t01.re = V707107*(t1.re+t1.im);        /* tr  */
      t01.im = V707107*(t1.im-t1.re);

      d[kj].re   = t.re + t01.re;
      d[kj].im   = t.im + t01.im;
      d[kkj].re  = temp.re + t11.re;
      d[kkj].im  = temp.im + t11.im;
      d[kkj1].re = t.re - t01.re;
      d[kkj1].im = t.im - t01.im;
      d[kkj2].re = temp.re - t11.re;
      d[kkj2].im = temp.im - t11.im;

      k2 = (k = j+nn) + mmax;
      k1 = j+mmax;
      i1 = i2 = 0;
      ki  = k;
      ki1 = k2;
      kii = k1;
      ki2 = ki1 + nn;
      for (i=j+1; i<j+nj; i++) {
        k2++; k1++; k++;
        i1+=ik1; i2+=ik2;

        t.re  = w[i1].re*d[k].re - w[i1].im*d[k].im;
        t.im  = w[i1].re*d[k].im + w[i1].im*d[k].re;
        t1.re = w[i1].re*d[k2].re - w[i1].im*d[k2].im;
        t1.im = w[i1].re*d[k2].im + w[i1].im*d[k2].re;

        temp.re  = w[i2].re*(d[k1].re + t1.re) - w[i2].im*(d[k1].im + t1.im);
        temp.im  = w[i2].re*(d[k1].im + t1.im) + w[i2].im*(d[k1].re + t1.re);
        temp1.re = w[i2].im*(d[k1].re - t1.re) + w[i2].re*(d[k1].im - t1.im);
        temp1.im = w[i2].im*(d[k1].im - t1.im) - w[i2].re*(d[k1].re - t1.re);

        d[k].re  = d[i].re - t.re + temp1.re;
        d[k2].re = d[i].re - t.re - temp1.re;
        d[k].im  = d[i].im - t.im + temp1.im;
        d[k2].im = d[i].im - t.im - temp1.im;
        d[k1].im = d[i].im + t.im - temp.im;
        d[k1].re = d[i].re + t.re - temp.re;
        d[i].im += t.im + temp.im;
        d[i].re += t.re + temp.re;

        ki--; ki1--; kii--; ki2--;
        t.re  =-w[i1].re*d[kii].re - w[i1].im*d[kii].im;
        t.im  =-w[i1].re*d[kii].im + w[i1].im*d[kii].re;
        t1.re =-w[i1].re*d[ki2].re - w[i1].im*d[ki2].im;
        t1.im =-w[i1].re*d[ki2].im + w[i1].im*d[ki2].re;

        temp.re  =-w[i2].im*(d[ki1].re + t1.re) + w[i2].re*(d[ki1].im + t1.im);
        temp.im  =-w[i2].im*(d[ki1].im + t1.im) - w[i2].re*(d[ki1].re + t1.re);
        temp1.re =-w[i2].re*(d[ki1].re - t1.re) - w[i2].im*(d[ki1].im - t1.im);
        temp1.im =-w[i2].re*(d[ki1].im - t1.im) + w[i2].im*(d[ki1].re - t1.re);

        d[kii].re = d[ki].re - t.re + temp1.re;
        d[ki2].re = d[ki].re - t.re - temp1.re;
        d[kii].im = d[ki].im - t.im + temp1.im;
        d[ki2].im = d[ki].im - t.im - temp1.im;
        d[ki1].im = d[ki].im + t.im - temp.im;
        d[ki1].re = d[ki].re + t.re - temp.re;
        d[ki].im  = d[ki].im + t.im + temp.im;
        d[ki].re  = d[ki].re + t.re + temp.re;
      }  /* for i */
    }   /* for j */
    nn = istep;
    mmax <<= 2;
    istep <<= 2;
    ik1>>=2;
    ik2>>=2;
    if (nn < m) goto nextBlock;
  } /* for */
  if (m<n) {
    nextB:

    i1 = i4 = 0;
    ik2 = ik1<<1;
    for (i2=0; i2<nn; i2+=256,i4=i1) {
      kk = i2+nn;
      for (j=0; j<n; j += mmax) {
        i3 = i2+j;
        i1 = i4;
        k=kk+j;
        for (i=0 ; i<256; i+=2,i1+=ik2) {
          t.re = w[i1].re*d[k+i].re - w[i1].im*d[k+i].im;
          t.im = w[i1].im*d[k+i].re + w[i1].re*d[k+i].im;
          t1.re = w[i1+ik1].re*d[k+i+1].re - w[i1+ik1].im*d[k+i+1].im;
          t1.im = w[i1+ik1].im*d[k+i+1].re + w[i1+ik1].re*d[k+i+1].im;
          d[k+i].re = d[i+i3].re - t.re;
          d[k+i].im = d[i+i3].im - t.im;
          d[i+i3].re += t.re;
          d[i+i3].im += t.im;
          d[k+i+1].re = d[i+i3+1].re - t1.re;
          d[k+i+1].im = d[i+i3+1].im - t1.im;
          d[i+i3+1].re += t1.re;
          d[i+i3+1].im += t1.im;
        }  /* for i */
      }  /* for j */
    }  /* for i2 */
    nn = mmax;
    mmax <<= 1;
    ik1 >>= 1;
    if (nn<n) goto nextB;
  } /* if */

    return;
  label5:
    nj = nn>>1;
    for (i=0; i<nj; i++) {
      t.im = w[i].im*d[nn+i].re + w[i].re*d[nn+i].im;
      t.re = w[i].re*d[nn+i].re - w[i].im*d[nn+i].im;
      d[nn+i].re = d[i].re - t.re;
      d[nn+i].im = d[i].im - t.im;
      d[i].re += t.re;
      d[i].im += t.im;
      t.im = -w[i].re*d[nn+nj+i].re + w[i].im*d[nn+nj+i].im;
      t.re = w[i].im*d[nn+nj+i].re + w[i].re*d[nn+nj+i].im;
      d[nn+nj+i].re = d[i+nj].re - t.re;
      d[nn+nj+i].im = d[i+nj].im - t.im;
      d[i+nj].re += t.re;
      d[i+nj].im += t.im;
    }  /* for i */
    return;

    inverse:
  for (j1=0; j1<n; j1+=m) {

    kk  = j1-3;
    kk1 = j1-2;
    kk2 = j1-1;
    for (j=j1; j<j1+m; j+=4) {
      kk2 += 4;
      kk  += 4;
      kk1 += 4;

      temp.re  = d[j].re - d[kk].re;
      temp1.re =-d[kk1].im + d[kk2].im;
      temp.im  = d[j].im - d[kk].im;
      temp1.im =-d[kk2].re + d[kk1].re;
      d[j].re   = (t.re  = d[j].re + d[kk].re)
                + (t1.re = d[kk1].re + d[kk2].re);
      d[j].im   = (t.im  = d[j].im + d[kk].im)
                + (t1.im = d[kk1].im + d[kk2].im);
      d[kk].re  = temp.re + temp1.re;
      d[kk].im  = temp.im + temp1.im;
      d[kk1].re = t.re - t1.re;
      d[kk1].im = t.im - t1.im;
      d[kk2].re = temp.re - temp1.re;
      d[kk2].im = temp.im - temp1.im;
    } /* for */
    nn = 4;
    mmax = 8;
    istep = 16;
    if (nn==n) return;
    ik1 = n>>3;
    ik2 = n>>4;
    invnext:

    if (istep>n) goto inv;
    kk2 = (kk = j1+nn-istep) + mmax;
    kk1 = j1+mmax-istep;
    nj = nn>>1;
    for (j=j1; j<j1+m; j+=istep) {
      kk2 += istep;
      kk  += istep;
      kk1 += istep;

      temp.re  = d[j].re - d[kk].re;
      temp.im  = d[j].im - d[kk].im;
      temp1.re =-d[kk1].im + d[kk2].im;
      temp1.im =-d[kk2].re + d[kk1].re;

      t.re      = d[j].re + d[kk].re;
      t1.re     = d[kk1].re + d[kk2].re;
      t.im      = d[j].im + d[kk].im;
      t1.im     = d[kk1].im + d[kk2].im;
      d[j].re   = t.re + t1.re;
      d[j].im   = t.im + t1.im;
      d[kk].re  = temp.re + temp1.re;
      d[kk].im  = temp.im + temp1.im;
      d[kk1].re = t.re - t1.re;
      d[kk1].im = t.im - t1.im;
      d[kk2].re = temp.re - temp1.re;
      d[kk2].im = temp.im - temp1.im;

      kj   = j+nj;
      kkj2 = kk2+nj;
      kkj  = kk+nj;
      kkj1 = kk1+nj;

      temp.re  = d[kj].re + d[kkj].im;  /* tkk */
      temp.im  = d[kj].im - d[kkj].re;
      t.re  = d[kj].re - d[kkj].im;     /* tk  */
      t.im  = d[kj].im + d[kkj].re;

      temp1.re = d[kkj1].re + d[kkj2].im;     /* tk2 */
      temp1.im = d[kkj1].im - d[kkj2].re;
      t1.re = d[kkj1].re - d[kkj2].im;        /* tk1 */
      t1.im = d[kkj1].im + d[kkj2].re;

      t11.re =-V707107*(temp1.re+temp1.im); /* tr1 */
      t11.im =-V707107*(temp1.im-temp1.re);
      t01.re = V707107*(t1.re-t1.im);        /* tr  */
      t01.im = V707107*(t1.im+t1.re);

      d[kj].re   = t.re + t01.re;
      d[kj].im   = t.im + t01.im;
      d[kkj].re  = temp.re + t11.re;
      d[kkj].im  = temp.im + t11.im;
      d[kkj1].re = t.re - t01.re;
      d[kkj1].im = t.im - t01.im;
      d[kkj2].re = temp.re - t11.re;
      d[kkj2].im = temp.im - t11.im;

      k2 = (k = j+nn) + mmax;
      k1 = j+mmax;
      i1 = i2 = 0;
      ki  = k;
      ki1 = k2;
      kii = k1;
      ki2 = ki1 + nn;
      for (i=j+1; i<j+nj; i++) {
        k2++; k1++; k++;
        i1+=ik1; i2+=ik2;

        t.re  = w[i1].re*d[k].re + w[i1].im*d[k].im;
        t.im  = w[i1].re*d[k].im - w[i1].im*d[k].re;
        t1.re = w[i1].re*d[k2].re + w[i1].im*d[k2].im;
        t1.im = w[i1].re*d[k2].im - w[i1].im*d[k2].re;

        temp.re  = w[i2].re*(d[k1].re + t1.re) + w[i2].im*(d[k1].im + t1.im);
        temp.im  = w[i2].re*(d[k1].im + t1.im) - w[i2].im*(d[k1].re + t1.re);
        temp1.re = w[i2].im*(d[k1].re - t1.re) - w[i2].re*(d[k1].im - t1.im);
        temp1.im = w[i2].im*(d[k1].im - t1.im) + w[i2].re*(d[k1].re - t1.re);

        d[k].re  = d[i].re - t.re + temp1.re;
        d[k2].re = d[i].re - t.re - temp1.re;
        d[k].im  = d[i].im - t.im + temp1.im;
        d[k2].im = d[i].im - t.im - temp1.im;
        d[k1].im = d[i].im + t.im - temp.im;
        d[k1].re = d[i].re + t.re - temp.re;
        d[i].im += t.im + temp.im;
        d[i].re += t.re + temp.re;

        ki--; ki1--; kii--; ki2--;
        t.re  =-w[i1].re*d[kii].re + w[i1].im*d[kii].im;
        t.im  =-w[i1].re*d[kii].im - w[i1].im*d[kii].re;
        t1.re =-w[i1].re*d[ki2].re + w[i1].im*d[ki2].im;
        t1.im =-w[i1].re*d[ki2].im - w[i1].im*d[ki2].re;

        temp.re  =-w[i2].im*(d[ki1].re + t1.re) - w[i2].re*(d[ki1].im + t1.im);
        temp.im  =-w[i2].im*(d[ki1].im + t1.im) + w[i2].re*(d[ki1].re + t1.re);
        temp1.re =-w[i2].re*(d[ki1].re - t1.re) + w[i2].im*(d[ki1].im - t1.im);
        temp1.im =-w[i2].re*(d[ki1].im - t1.im) - w[i2].im*(d[ki1].re - t1.re);

        d[kii].re = d[ki].re - t.re + temp1.re;
        d[ki2].re = d[ki].re - t.re - temp1.re;
        d[kii].im = d[ki].im - t.im + temp1.im;
        d[ki2].im = d[ki].im - t.im - temp1.im;
        d[ki1].im = d[ki].im + t.im - temp.im;
        d[ki1].re = d[ki].re + t.re - temp.re;
        d[ki].im  = d[ki].im + t.im + temp.im;
        d[ki].re  = d[ki].re + t.re + temp.re;
      }  /* for i */
    }   /* for j */
    nn = istep;
    mmax <<= 2;
    istep <<= 2;
    ik1>>=2;
    ik2>>=2;
    if (nn < m) goto invnext;
  } /* for */

  if (m<n) {

    invnextB:

    i1 = i4 = 0;
    ik2 = ik1 << 1;
    for (i2=0; i2<nn; i2+=256,i4=i1) {
      kk = i2+nn;
      for (j=0; j<n; j += mmax) {
        i3 = i2+j;
        k=kk+j;
        i1 = i4;
        for (i=0 ; i<256; i+=2,i1+=ik2) {
          t.re = w[i1].re*d[k+i].re + w[i1].im*d[k+i].im;
          t.im =-w[i1].im*d[k+i].re + w[i1].re*d[k+i].im;
          t1.re = w[i1+ik1].re*d[k+i+1].re + w[i1+ik1].im*d[k+i+1].im;
          t1.im =-w[i1+ik1].im*d[k+i+1].re + w[i1+ik1].re*d[k+i+1].im;
          d[k+i].re = d[i+i3].re - t.re;
          d[k+i].im = d[i+i3].im - t.im;
          d[i+i3].re += t.re;
          d[i+i3].im += t.im;
          d[k+i+1].re = d[i+i3+1].re - t1.re;
          d[k+i+1].im = d[i+i3+1].im - t1.im;
          d[i+i3+1].re += t1.re;
          d[i+i3+1].im += t1.im;
        }  /* for i */
      }  /* for j */
    }  /* for i2 */
    nn = mmax;
    mmax <<= 1;
    ik1 >>= 1;
    if (nn<n) goto invnextB;
  } /* if */
    goto norm;
  inv:
    k = nn;
    nj = nn>>1;
    k1= k + nj;
    for (i=0; i<nj; i++) {
      t.im =-w[i].im*d[k+i].re + w[i].re*d[k+i].im;
      t.re = w[i].re*d[k+i].re + w[i].im*d[k+i].im;
      d[k+i].re = d[i].re - t.re;
      d[k+i].im = d[i].im - t.im;
      d[i].re += t.re;
      d[i].im += t.im;
      t.im =  w[i].re*d[k1+i].re + w[i].im*d[k1+i].im;
      t.re = w[i].im*d[k1+i].re - w[i].re*d[k1+i].im;
      d[k1+i].re = d[nj+i].re - t.re;
      d[k1+i].im = d[nj+i].im - t.im;
      d[nj+i].re += t.re;
      d[nj+i].im += t.im;
    }  /* for i */

    norm:

    /*t.re = 1.0/n;
    for (i=0; i<n; i++) {
        d[i].re *= t.re;
        d[i].im *= t.re;
    }*/ /* for */

    return;

} /* nspcUndRadix2_FFT */
#else

extern void
nspcUndRadix2_FFT(
  SCplx       *d,
  int          n,
  int          inv,
  const SCplx *w);

#endif

#if P5_OPT == 0
/*F*
//  Name :      nspcUndRealRecom_Fft
//  Purpose:    Internal function.
//              Computes  triginometric recombination of real signal
//              (in-place) using RCPerm format.
//  Arguments:
//        x   - Pointer to the real array which holds the input and
//              output complex samples.  The x[] array must be of length
//              pow(2,order+1),  where (order-1) - Base-2 logarithm
//              of the number of complex samples in FFT.
//        n   - pow(2,order-2).
//        is  - Indicates the direction of the fast Fourier transform:
//                 1 - forward transform,
//                -1 - inverse transform.
//        c   - Pointer to the table of special twiddle factors.
*F*/

void
nspcUndRealRecom_Fft(
   float  *x,
   int     n,
   int     is,
   float  *c)
{
   float   s1, s2, t3, t4, t1, t2;
   int     i, j;

   if ( n == 1 ) return;

   if ( is < 0 ) {                    /* inverse transform */

      for ( i=2, j=2*n-2 ; i < n ; i+=2, j-=2 ) {
         t2     = x[i+1] + x[j+1];
         t1     = x[ i ] - x[ j ];
         t4     = c[i-1]*t2 + c[i-2]*t1;
         x[j+1] = x[j+1] - t4;
         x[i+1] = x[i+1] - t4;
         t3     = c[i-2]*t2 - c[i-1]*t1;
         s1     = x[ i ] + t3;
         x[ j ] = x[ j ] - t3;
         x[ i ] = s1;
      } /* end for */

   } else {                           /* forward transform */

      for ( i=2, j=2*n-2 ; i < n ; i+=2, j-=2 ) {
         t2     = x[i+1] + x[j+1];
         t1     = x[ i ] - x[ j ];
         t4     = c[i-1]*t2 - c[i-2]*t1;
         t3     = c[i-2]*t2 + c[i-1]*t1;
         s1     = x[ j ] + t3;
         x[ j ] = x[ i ] - t3;
         x[ i ] = s1;
         s2     = -x[j+1] + t4;
         x[j+1] = -x[i+1] + t4;
         x[i+1] = s2;
      } /* end for */
      x[n+1] = -x[n+1];

   } /* end if */

   return;
}  /* nspcUndRealRecom_Fft */

#else

extern void
nspcUndRealRecom_Fft(
   float  *x,
   int     n,
   int     is,
   float  *c);

#endif

/*F*
//  Name :      nspcUndCcsRecom_Fft
//  Purpose:    Internal function.
//  Purpose:    Computes triginometric recombination for complex
//              conjugate-symmetric data (in-place) using RCPerm format.
//  Arguments:
//     x      - Pointer to the complex array which holds the input and
//              output complex samples.  The x[] array must be of length
//              N=pow(2,order+1).
//     order  - Base-2 logarithm of the number of samples in FFT  (N).
//     flags  - Indicates the direction of the fast Fourier transform.
//              Integer number in which every bit have next specifications:
//                 NSP_Forw - forward transform,
//                 NSP_Inv  - inverse transform.
//     expTbl - Pointer to the table of twiddle factors.
*F*/


static void
nspcUndCcsRecom_Fft(
   SCplx       *x,
   int          order,
   int          flags,
   const SCplx *expTbl)
{
  float         reTmp, imTmp, reSum, imSum, reDif, imDif;
  int           len, limit, k, l;

  len   = 1<<order;
  limit = len>>1;
  
  if ( limit ) {

     if ( flags & NSP_Forw ) {     /*  forward transform  */

        for ( k=1, l=len-k ; k < limit ; k++, l-- ) {
           reDif  = x[k].re - x[l].re;
           imDif  = x[k].im + x[l].im;
           reSum  = x[k].re + x[l].re;
           imSum  = x[k].im - x[l].im;
           imTmp   = reDif*expTbl[k].re - imDif*expTbl[k].im;
           x[k].im = imSum + imTmp;
           reTmp   =-reDif*expTbl[k].im - imDif*expTbl[k].re;
           x[l].im =-imSum + imTmp;
           x[k].re = reSum + reTmp;
           x[l].re = reSum - reTmp;
        } /* for */
        x[ limit ].re *= (float)2;
        x[ limit ].im *= (float)2;

     } else {                      /*  inverse transform  */

        for ( k=1, l=len-k ; k < limit ; k++, l-- ) {
           reDif  = x[k].re - x[l].re;
           imDif  = x[k].im + x[l].im;
           reSum  = x[k].re + x[l].re;
           imSum  = x[k].im - x[l].im;
           imTmp   = reDif*expTbl[k].re + imDif*expTbl[k].im;
           x[k].im = imSum + imTmp;
           reTmp   = reDif*expTbl[k].im - imDif*expTbl[k].re;
           x[l].im =-imSum + imTmp;
           x[k].re = reSum + reTmp;
           x[l].re = reSum - reTmp;
        } /* for */
        x[ limit ].re *= (float)2;
        x[ limit ].im *= -(float)2;

     } /* if for transform */

  } /* if ( limit ) */

  return;
} /* nspcUndCcsRecom_Fft */


void nspsbMpy1(const float val, float *dst, int n)
{
  float r0, r1, r2, r3;
  int k;


  for (k=n-8; k>=0; k-=8) {
     r0 = dst[k  ] * val;
     r1 = dst[k+1] * val;
     r2 = dst[k+2] * val;
     r3 = dst[k+3] * val;
     dst[k  ] = r0;
     dst[k+1] = r1;
     dst[k+2] = r2;
     dst[k+3] = r3;
     r0 = dst[k+4] * val;
     r1 = dst[k+5] * val;
     r2 = dst[k+6] * val;
     r3 = dst[k+7] * val;
     dst[k+4] = r0;
     dst[k+5] = r1;
     dst[k+6] = r2;
     dst[k+7] = r3;
   };
   for (k=k+7; k>=0; k--) {
       r0 = dst[k  ] * val;
       dst[k  ] = r0;
   };
}


/* ---------------------- FFTs of Real Signals ---------------------------- */

/*F*
//  Name:      nspsRealFft
//  Purpose:   Computes the forward or inverse fast Fourier transform (FFT)
//             of real signal (in-place) using RCCcs format.
//  Arguments:
//     samps - Pointer to the real array which holds the input and
//             output samples. The samps[] array must be of length
//             N+2 elements (float).  ( N=pow(2,order) ).
//             On input, samps[] should be considered as a real array,
//             the first N elements of which are data and the last two
//             elements are ignored.
//             On output, samps[] should be considered as a complex array
//             of length N/2+1 complex samples in RCCcs format.
//     order - Base-2 logarithm of the number of samples in FFT  (N).
//     flags - Indicates the direction of the fast Fourier transform.
//             Integer number in which every bit have next specifications:
//                NSP_Forw    - forward transform,
//                NSP_Inv     - inverse transform,
//                NSP_Init    - init of coeficient table without transform,
//                NSP_NoScale - in inverse transform absent to scale.
//  Calls:
//        nspcUndGetFftTwdTbl_great,
//        nspcbBitRev1,   
//        nspcUndSplit_FFT,
//        nspcUndRadix2_FFT,
//        nspcUndRealRecom_Fft,
//        nspsbMpy1.
*F*/

void vox_nspsRealFft(
   float       *samps,
   int          order,
   int          flags)
{
   int          p,                 /* For (order-1) */
                n,                 /* For pow(2,(order-1)) and pow(2,order) */
                inv_fl;            /* Transform flag: forward or inverse */
   const SCplx *w=NULL;            /* Table of twiddle factors */
   SCplx       *cx;                /* For interpretation samps as SCplx */
   float        Scale;             /* For scale in inverse transform */

   /* Computes the tables of twiddle factors */
        p = order-1;                                               
        if ( p > 0 ) {                                             
           w = nspcUndGetFftTwdTbl_great(p);                       
           if(!w) 
             return;
        }
        if ( flags & NSP_Init ) return;


   if ( order == 0 ) return;                               /* Control order */

   cx = (SCplx *)samps;      /* Form pointer cx for interpr. samps as SCplx */

   n = 1 << p;                                   /* Form  pow(2,(order-1))  */

   inv_fl = ( flags & NSP_Forw ) ? 1 : -1;           /* Form transform flag */

   if ( p > 0 ) {

      if ( p < FFT_OrderSplit )    {  /* ---------Split algorithm --------- */

         nspcUndSplit_FFT(cx,n,inv_fl,w);            /* Computes FFT for cx */
         nspcbBitRev1(cx,p);                         /* Form BitRev  for cx */

      } else                       {  /* ---------Radix algorithm --------- */

         nspcbBitRev1(cx,p);                         /* Form BitRev  for cx */
         nspcUndRadix2_FFT(cx,n,inv_fl,w);           /* Computes FFT for cx */

      } /* if for algorithm type */

   } /* if for p */

   Scale      = samps[0] - samps[1];         /* Triginometric recombination */
   samps[0]   = samps[0] + samps[1];
   samps[1]   = (float)0.0;
   nspcUndRealRecom_Fft(samps,n,inv_fl,(float *)(w+n/2));
   n          = 1 << order;
   samps[n  ] = Scale;
   samps[n+1] = (float)0.0;

         if ( flags & NSP_Inv ) {                                   
           if ( !( flags & NSP_NoScale ) ) {                       
              Scale    = (float)(1.0/n);                          
              nspsbMpy1(Scale,samps,n+2);                           
           }                                                       
        }
   return;
} /* nspsRealFft */


/* --------------------- FFTs of Conjugate-symmetric Signals -------------- */

/*F*
//  Name:       nspsCcsFft
//  Purpose:    Computes the forward or inverse fast Fourier transform (FFT)
//              of a complex conjugate symmetric (CCS) signal
//              using RCCcs format (in-place).
//  Arguments:
//      samps - Pointer to the real array which holds the input and
//              output samples. The samps[] array must be of length N+2
//              (N=pow(2,order))  elements (float).
//              On input, samps[] should be considered as a complex array
//              of length N/2+1 complex samples in RCCcs format.
//              On output, samps[] should be considered as a real array,
//              the first N elements of which are data and the last two
//              elements are ignored.
//      order - Base-2 logarithm of the number of samples in FFT  (N).
//      flags - Indicates the direction of the fast Fourier transform.
//              Integer number in which every bit have next specifications:
//                 NSP_Forw    - forward transform,
//                 NSP_Inv     - inverse transform,
//                 NSP_Init    - init of coeficient table without transform,
//                 NSP_NoScale - in inverse transform absent to scale.
//  Calls:
//        nspcUndGetFftTwdTbl_great,
//        nspcUndCcsRecom_Fft,
//        nspcbBitRev1,
//        nspcUndSplit_FFT,
//        nspcUndRadix2_FFT,
//        nspsbMpy1.
*F*/

void vox_nspsCcsFft(
   float       *samps,
   int          order,
   int          flags)
{
   int          p,                   /* For (order-1) */
                n,                   /* For pow(2,order) */
                n2,                  /* For pow(2,order-1) */
                inv_fl;              /* Transform flag: forward or inverse */
   const SCplx *w=NULL;              /* Table of twiddle factors (order-1) */
   SCplx       *cx;                  /* For interpretation samps as SCplx */
   float        Scale,               /* For scale in inverse transform */
                Zcale;

   /* Computes the tables of twiddle factors */

   p = order-1;                                                
   if ( p > 0 ) {                                              
      w = nspcUndGetFftTwdTbl_great(p);                        
      if(!w) 
        return;
   }

   if ( order == 0 ) return;                               /* Control order */

   n     = 1 << order;                                 /* Form pow(2,order) */

   Scale = samps[0] + samps[n];          /* for triginometric recombination */
   Zcale = samps[0] - samps[n];

   if ( p > 0 ) {

      cx = (SCplx *)samps;   /* Form pointer cx for interpr. samps as SCplx */
      n2 = 1 << p;                                   /* Form pow(2,order-1) */
      inv_fl = ( flags & NSP_Forw ) ? 1 : -1;        /* Form transform flag */

      nspcUndCcsRecom_Fft(cx,p,flags,(w+n2));     /* Computes trig. recomb. */
      samps[0] = Scale;
      samps[1] = Zcale;

      if ( p < FFT_OrderSplit )    {  /* ---------Split algorithm --------- */

         nspcUndSplit_FFT(cx,n2,inv_fl,w);           /* Computes FFT for cx */
         nspcbBitRev1(cx,p);                         /* Form BitRev  for cx */

      } else                       {  /* ---------Radix algorithm --------- */

         nspcbBitRev1(cx,p);                         /* Form BitRev  for cx */
         nspcUndRadix2_FFT(cx,n2,inv_fl,w);          /* Computes FFT for cx */

      } /* if for algorithm type */

   }  else  {   /* p == 0 */
      samps[0] = Scale; samps[1] = Zcale;
   }  /* if for p */

   if ( flags & NSP_Inv ) {                                    
      if ( !( flags & NSP_NoScale ) ) {                        
         Scale    = (float)(1.0/n);                                  
         nspsbMpy1(Scale,samps,n);                            
      }                                                        
   }

   return;
} /* nspsCcsFft */







/* ------------------------- Basic FFT functions -------------------------- */
/*F*
//  Name:      nspcFft
//  Purpose:   Computes the forward or inverse fast Fourier transform (FFT)
//             of complex signal (in-place).
//  Arguments:
//     samps - Pointer to the complex array which holds the input and
//             output complex samples.  The samps[] array
//             must be of length N=pow(2,order).
//     order - Base-2 logarithm of the number of samples in FFT  (N).
//     flags - Indicates the direction of the fast Fourier transform and
//             whether bit-reversal is performed.
//             Integer number in which every bit have next specifications:
//                NSP_Forw      - forward transform,
//                NSP_Inv       - inverse transform,
//                NSP_Init      - init of coeficient table without transform,
//                NSP_NoScale   - in inverse transform absent to scale,
//                NSP_NoBitRev  - input and output signals in normal order,
//                NSP_InBitRev  - input  signal in bit-reverse,
//                NSP_OutBitRev - output signal in bit-reverse.
//  Calls:
//        nspcUndGetFftTwdTbl_great,
//        nspsUndControlFlags_Fft,
//        nspcbBitRev1,
//        nspcUndSplit_FFT,
//        nspcUndRadix2_FFT,
//        nspsbMpy1.
*F*/


void
vox_nspcFft(
   SCplx       *samps,
   int          order,
   int          flags)
{
   int          n,                 /* Length of signal */
                inv_fl;            /* Transform flag: forward or inverse */
   const SCplx *w;                 /* Table of twiddle factors */
   float        Scale;             /* For scale in inverse transform */

   /* Computes the table of twiddle factors */
   w = nspcUndGetFftTwdTbl_great(order);                       

   /* Control argument flags: NSP_NoBitRev, NSP_InBitRev, NSP_OutBitRev */
   if ( ! ( (flags & NSP_InBitRev ) |                          
            (flags & NSP_OutBitRev) |                          
            (flags & NSP_NoBitRev ) ) ) 
   {                      
      flags = flags | NSP_NoBitRev;                            
   }

   if ( order == 0 )  return;                     /* Control order */

   n = 1 << order;                                /* Form length of signal */

   inv_fl = ( flags & NSP_Forw ) ? 1 : -1;        /* Form transform flag */

   if ( order < FFT_OrderSplit )   {  /* ---------Split algorithm --------- */

      if ( flags & NSP_InBitRev ) {               /* Control NSP_InBitRev */
         nspcbBitRev1(samps,order);
      }
      nspcUndSplit_FFT(samps,n,inv_fl,w);         /* Computes FFT */
      if ( !( flags & NSP_OutBitRev ) ) {         /* Control NSP_OutBitRev */
         nspcbBitRev1(samps,order);
      }

   } else                          {  /* ---------Radix algorithm --------- */

      if ( !( flags & NSP_InBitRev ) ) {          /* Control NSP_InBitRev */
         nspcbBitRev1(samps,order);
      }
      nspcUndRadix2_FFT(samps,n,inv_fl,w);        /* Computes FFT */
      if ( flags & NSP_OutBitRev ) {              /* Control NSP_OutBitRev */
         nspcbBitRev1(samps,order);
      }

   } /* if for algorithm type */

   /* Control NSP_Inv and NSP_NoScale */
   if ( flags & NSP_Inv ) 
   {                                    
      if ( !( flags & NSP_NoScale ) ) 
      {                        
          Scale    = (float)(1.0/n);                           
          nspsbMpy1(Scale,(float *)samps,n*2);                           
      }                                                        
   }

   return;
}  /* nspcFft */

// nsp forgot to free their resources???!!!!  so I guess I have to do it!
void nspFreeFftResources() 
{
   unsigned short i;
   for(i=0;i<32;i++) 
      if(ctxFftTbl[i]) {
         free(ctxFftTbl[i]);
         ctxFftTbl[i]=NULL;
      }

   for(i=0;i<32;i++)
      if(ctxRevTbl[i]) {
         free(ctxRevTbl[i]);
         ctxRevTbl[i]=NULL;
      }


   return;
}
