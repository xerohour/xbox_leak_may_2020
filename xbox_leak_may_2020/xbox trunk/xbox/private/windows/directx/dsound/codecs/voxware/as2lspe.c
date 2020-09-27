/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* Filename:       as2lspe.c
*
* Purpose:        Compute LSPs from predictor coefficients for EVEN all-pole
*                   model orders.
*
* Functions:      VoxAsToLspEven()
*
* Author/Date:    Bob Dunn 2/2/98 (with code scarfed from many others...
*                            S. Yeldner, R. Zopf, W. Wang, I. Berci)
********************************************************************************
* Implementation/Detailed Description:
*
*            1) Compute reduced polynomials RP(z) and RQ(z)
*            2) Decimate RP(z) and RQ(z) by a factor of 2
*            3) Search for roots of decimated polynomials
*            4) Check that roots are valid
*            5) Convert the roots to the LSP domain and scale
*
* References:  "Speech Analysis and Synthesis Methods Developed at ECL in
*                NTT - From LPC to LSP -", Noboru Sugamura and Fumitada
*                Itakura, Speech Communication 5 (1986) pp 199-215.
*
*              "Line Spectrum Pair (LSP) and Speech Data Compression",
*                Frank k. Soong and Bing-Hwang Juang, Proc. ICASSP 1984,
*                pp 1.10.1-1.10.4
*
*              "Application of Line-Spectrum Pairs to Low-Bit_rate Speech
*                Encoders", George S. Kang and Lawrence J. Fransen, Proc.
*                ICASSP 1985, PP 7.3.1-7.3.4
*
*              "A Novel Two-Level Method for the Computation of LSP
*                Frequencies Using a Decimation-in-Degree Algorithm",
*                C. Wu and J. Chen, IEEE Trans. on Speech and Audio
*                Proc., Vol. 5, No. 2, March 1977.
********************************************************************************
*
* Modifications:  
*
* Comments:    The inverse filter is defined as:
*
*                      A(Z) = 1 + SUM(i=1...p) A_i Z^(-i)
*
*              The LSPs are normalized in frequency and range
*                from 0 to 0.5.  Since many codecs require LSPs in
*                Hertz, fScaleFactor is included to allow the conversion
*                to Hertz.  If fScaleFactor is the sampling rate, the
*                LPSs will be in Hertz.  If fScaleFactor is 1.0 the LPSs
*                will range from 0 to 0.5.
*
* Concerns:
*
*              There are two methods for root finding used here.  One method
*                is to use Newton's method to find all but the last root.
*                The second method is to use the closed form solution to
*                find the last 4 roots.  The closed form solution is faster
*                in both peak and average times.  The closed form solution
*                is also more accurate for 10th and 12th order all-pole
*                models.  However, the accuracy of the closed form solution
*                is lower than Newton's method when the all-pole model
*                order is 18.  I may be desirable to add a refinement step
*                where a single additional iteration of Newton's method is
*                applied for each estimated root using the original polynomial.
*
*              The closed form solution for 4th order polynomials may not
*                be appropriate for fixed point processing.  If this is the
*                case then it can be replaced with Newton's method.
*
*              In this routine a 1st order Newton's method is used to find
*                roots of polynomials.  A 2nd order routine (i.e. using 1st
*                and 2nd derivatives) could be used for this and it would
*                require fewer iterations (both average and peak) to solve
*                for roots.  However, the increased computation for each
*                iteration of a 2nd order routine offsets the reduction in 
*                the total number of iterations.  The more simple 1st order
*                routine is used since it has a slightly smaller code size.
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/as2lspe.c_v   1.7   03 Mar 1998 08:06:38   bobd  $
*******************************************************************************/
#include <math.h>
#include <string.h>
#include <assert.h>

#include "vLibDef.h"
#include "vLibTran.h"

#include "xvocver.h"

/*----------------------------------------------------------
  Use closed form solution for all pole model orders 8
    through MAX_QUAD_ROOT_ORDER.  The closed form solution
    for fourth order polynomials cannot be used for model
    orders lees than 8 and its accuracy declines above
    MAX_QUAD_ROOT_ORDER. NOTE: It is possible to use
    the closed form soultion to get a good initial guess 
    for the iterative method, but that is not done here.
----------------------------------------------------------*/
#define MAX_QUAD_ROOT_ORDER  14

#define SUCCESS 0
#define FAILURE 1

/*----------------------------------------------------------
  Defines for root finding
----------------------------------------------------------*/
#define ROOT_CONVERGE      0
#define ROOT_NOT_CONVERGE -1
#define ROOT_MAXIMUM       1

#define THRESH_ZERO  1.0e-6F
#define THRESH_ONE   0.01F

/*----------------------------------------------------------
  MAX_ITER is the maximum number of iterations per root.
    This is set conservatively.  The maximum values that
    have been observed are:

       13 for 18th order model
       11 for 16th order model
       11 for 14th order model
       11 for 12th order model
       10 for 10th order model
----------------------------------------------------------*/
#define MAX_ITER     15

/*----------------------------------------------------------
  Internal function declarations.
----------------------------------------------------------*/
static void VoxDIDeven( float *pfRP, float *pfRQ, int iOrder, float *pfDP,
                        float *pfDQ );


static int iFindRoot( float *pfRoot, int iDegree, const float *pfOrgCoef,
                      float *pfNewCoef);

static void VoxCompRedPoly( float *pfAs, int iOrder, float *pfRP, float *pfRQ );

static void VoxSetArbitraryLSP( float *pfLSP, int iOrder, float  fScaleFactor);

static void SolveQuad( float *pfCoef, float *pfRoots );

/*******************************************************************************
* Function:  VoxAsToLspEven()
*
* Action:    Compute LSPs from predictor coefficients for an even ordered
*              all-pole model.
*
* Input:     float *pfAs          --> predictor coefficients
*            int    iOrder        --> all-pole model order
*            float  fScaleFactor  --> scale factor to apply to LSPs (if the
*                                       factor is 1.0, the LSPs range from
*                                       0 to 0.5).
*
* Output:    float *pfLSP         --> LSPs
*
* Globals:   none
*
* Return:    int        (0/1) (success/fail)
********************************************************************************
* Modifications:
*
* Comments:    The inverse filter is defined as:
*
*                      A(Z) = 1 + SUM(i=1...p) A_i Z^(-i)
*
*              The LSPs are normalized in frequency and range 
*                from 0 to 0.5.  Since many codecs require LSPs in 
*                Hertz, fScaleFactor is included to allow the conversion
*                to Hertz.  If fScaleFactor is the sampling rate, the 
*                LPSs will be in Hertz.  If fScaleFactor is 1.0 the LPSs
*                will range from 0 to 0.5.
*
* Concerns/TBD:
*
*******************************************************************************/
int VoxAsToLspEven( float *pfAs, 
                    int    iOrder, 
                    float  fScaleFactor,
                    float *pfLSP
                  )
{
   int    i, j;
   int    iDegree;
   int    iOrder2;
   int    iStatus = SUCCESS;
   int    iQuadRootFlag;
   int    iMinDegree;
   float  fRoot;
   float  fScale;
   float *pfTemp;
   float  fRP[(MAX_LPC_ORDER>>1)+1];
   float  fRQ[(MAX_LPC_ORDER>>1)+1];
   float  fDP[(MAX_LPC_ORDER>>1)+1];
   float  fDQ[(MAX_LPC_ORDER>>1)+1];
   float  fQuadRootsD[4];
   float  fQuadRootsQ[4];

   assert( pfAs );
   assert( pfLSP );
   assert( !(iOrder&1) );             /* model order must be even!  */
   assert( iOrder <= MAX_LPC_ORDER ); /* don't exceed MAX_LPC_ORDER */

   if ((iOrder>MAX_QUAD_ROOT_ORDER) || (iOrder<8))
      iQuadRootFlag = 0;
   else
      iQuadRootFlag = 1;

   iOrder2 = iOrder>>1;

   /*--------------------------------------------------------
     Compute reduced P and Q polynomials from the predictor
       coefficients.
   --------------------------------------------------------*/
   VoxCompRedPoly( pfAs, iOrder, fRP, fRQ );

   fRP[iOrder2] = 1.0F;  /* set high order coefficient */
   fRQ[iOrder2] = 1.0F;  /* set high order coefficient */

   /*--------------------------------------------------------
     Decimate reduced P and Q polynomials
   --------------------------------------------------------*/
   VoxDIDeven( fRP, fRQ, iOrder, fDP, fDQ );

   fRoot = 1.99999999F;  /* Initial guess for first root */
   pfTemp = fRP;         /* re-use this memory for       */
   j = 0;                /* initialize index for LSPs    */

   /*-----------------------------------------------------
     Solve for roots of the decimated polynomials.
   -----------------------------------------------------*/
   if (iQuadRootFlag)
      iMinDegree = 4;
   else 
      iMinDegree = 1;

   for (iDegree = iOrder2; iDegree>iMinDegree; iDegree--)
   {
      /*-----------------------------------------------------
        Find a root of fDP[].
      -----------------------------------------------------*/
      if (iFindRoot(&fRoot, iDegree, fDP, pfTemp)!= ROOT_CONVERGE)
         iStatus = FAILURE;

      pfLSP[j++] = fRoot;

      /*-----------------------------------------------------
        Copy reduced order polynomial from pfTemp[] to fDP[]
      -----------------------------------------------------*/
      memcpy( fDP, pfTemp, (iDegree+1)*sizeof(float) );

      /*-----------------------------------------------------
        Find root of fDQ[].
      -----------------------------------------------------*/
      if (iFindRoot(&fRoot, iDegree, fDQ, pfTemp)!= ROOT_CONVERGE)
         iStatus = FAILURE;

      pfLSP[j++] = fRoot;

      /*-----------------------------------------------------
        Copy reduced order polynomial from pfTemp[] to fDQ[]
      -----------------------------------------------------*/
      memcpy( fDQ, pfTemp, (iDegree+1)*sizeof(float) );
   }

   if (iQuadRootFlag)
   {
      /*-----------------------------------------------------
        Use the closed form solution to find the roots
          of the 4th order polynomial.
      -----------------------------------------------------*/
      SolveQuad( fDP, fQuadRootsD );
      SolveQuad( fDQ, fQuadRootsQ );
   
      pfLSP[j++] = fQuadRootsD[0];
      pfLSP[j++] = fQuadRootsQ[0];

      pfLSP[j++] = fQuadRootsD[1];
      pfLSP[j++] = fQuadRootsQ[1];

      pfLSP[j++] = fQuadRootsD[2];
      pfLSP[j++] = fQuadRootsQ[2];

      pfLSP[j++] = fQuadRootsD[3];
      pfLSP[j++] = fQuadRootsQ[3];
   } else {
      /*-----------------------------------------------------
        Solve for 1st order roots.  If (fDP[1]==0.0F) or 
         (fDQ[1]==0.0F) then these cannot be found.
      -----------------------------------------------------*/
      if ( (fDP[1]==0.0F) || (fDQ[1]==0.0F))
      {
         iStatus = FAILURE;
      } else {
         pfLSP[j++] = -fDP[0]/fDP[1];
         pfLSP[j++] = -fDQ[0]/fDQ[1];
      }
   }

   /*---------------------------------------------------------------
     Check if LPSs are valid.  This is done in the domain of 
       x[i] = 2*cos( 2*PI*LSP[i] ).  In this domain the 
       realtionship is:

     (-2 < x[iOrder-1] < ... < x[i+1] < x[i] < ... < x[0] < 2)

     NOTE: In the above expression the ordering property of the 
             LSPs has been reversed by the cosine.
   ---------------------------------------------------------------*/
   if ((pfLSP[0]>=2.0F) || (pfLSP[iOrder-1]<=-2.0F))
      iStatus = FAILURE;

   for (i=1; i<iOrder; i++)
      if (pfLSP[i]>pfLSP[i-1])
         iStatus = FAILURE;

   if (iStatus==SUCCESS)
   {
      /*-----------------------------------------------------
        Convert roots to normalized frequencies.
      -----------------------------------------------------*/
      fScale = fScaleFactor*(1.0F/DB_PI);
      for (i=0; i<iOrder; i++)
         pfLSP[i] = (float)acos( pfLSP[i]*0.5F )*fScale;
   } else {
      /*-----------------------------------------------------
        Root finding failed....compute evenly spaced LSPs.
      -----------------------------------------------------*/
      VoxSetArbitraryLSP( pfLSP, iOrder, fScaleFactor );
   }

   return iStatus;

} /* VoxAsToLspEven() */

/*******************************************************************************
* Function:  VoxCompRedPoly()
*
* Action:    Compute reduced polynomials RP(z) and RQ(z) from the predictor
*              coefficients for an even ordered all-pole model.
*
* Input:     float *pfAs    --> predictor coefficients
*            int    iOrder  --> all-pole model order
*
* Output:    float *pfRP    --> reduced polynomial RP(z)
*            float *pfRQ    --> reduced polynomial RQ(z)
*
* Globals:   none
*
* Return:    void
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:    The inverse filter is defined as:
*
*                      A(Z) = 1 + SUM(1...p) A_p Z^(-p)
*
*            Since the reduced polynomials are symmetric, only half of
*              the coefficients are needed for this computation.  The
*              format of pfRP[] and pfRQ[] is:
*
*  RP(z) = pfRP[N]*z^(-2N) + pfRP[N-1]*z^(1-2N) + ... 
*           + pfRP[1]*z^(-1-N) + pfRP[0]*z^(-N) + pfRP[1]*z^(1-N) + ... 
*           + pfRP[N-1]*z + pfRP[N]
*
*  RQ(z) = pfRQ[N]*z^(-2N) + pfRQ[N-1]*z^(1-2N) + ...
*           + pfRQ[1]*z^(-1-N) + pfRQ[0]*z^(-N) + pfRQ[1]*z^(1-N) + ...
*           + pfRQ[N-1]*z + pfRQ[N]
*
*              where N = iOrder/2
*
* Concerns/TBD:
*******************************************************************************/
static void VoxCompRedPoly( float *pfAs, 
                            int    iOrder, 
                            float *pfRP, 
                            float *pfRQ 
                          )
{
   int    i;
   float  fTemp0;
   float  fTemp1;
   float *pfAs0;
   float *pfAs1;

   fTemp0  = 1.0F;
   fTemp1  = 1.0F;
   pfAs0   = pfAs+1;
   pfAs1   = pfAs+iOrder;
   for (i = (iOrder>>1)-1; i>=0; i--)
   {
      fTemp0 = pfRP[i] = *pfAs0   + *pfAs1   - fTemp0;
      fTemp1 = pfRQ[i] = *pfAs0++ - *pfAs1-- + fTemp1;
   }
} /* VoxCompRedPoly */

/*******************************************************************************
* Function:  VoxDIDeven()
*
* Action:    Decimate reduced P(z) and reduced Q(z) polynomials by
*              a factor of 2.  This only works for even all-pole
*              model orders because for odd all-pole model orders
*              the reduced polynomials have different lengths.
*
* Input:     float *pfRP   --> coefficients of reduced polynomial RP(z)
*            float *pfRQ   --> coefficients of reduced polynomial RQ(z)
*            int    iOrder --> all-pole model order
*
* Output:    float *pfDP   --> coefficients of decimated polynomial DP(z)
*            float *pfDQ   --> coefficients of decimated polynomial DQ(z)
*
* Globals:   none
*
* Return:    void
********************************************************************************
* Implementation/Detailed Description:
*            Since the reduced polynomials are symmetric, only half of
*              the coefficients are needed for this computation.  The
*              format of pfRP[] and pfRQ[] is:
*
*  RP(z) = pfRP[N]*z^(-2N) + pfRP[N-1]*z^(1-2N) + ... 
*           + pfRP[1]*z^(-1-N) + pfRP[0]*z^(-N) + pfRP[1]*z^(1-N) + ... 
*           + pfRP[N-1]*z + pfRP[N]
*
*  RQ(z) = pfRQ[N]*z^(-2N) + pfRQ[N-1]*z^(1-2N) + ... 
*           + pfRQ[1]*z^(-1-N) + pfRQ[0]*z^(-N) + pfRQ[1]*z^(1-N) + ... 
*           + pfRQ[N-1]*z + pfRQ[N]
*
*              where N = iOrder/2
*
* References:
*******************************************************************************/
static void VoxDIDeven( float *pfRP, 
                        float *pfRQ,
                        int    iOrder, 
                        float *pfDP,
                        float *pfDQ
                      )
{
   int   i, j, k;
   int   N;
   int   Nv2;
   float fS[MAX_LPC_ORDER>>1];

   N   = iOrder>>1;
   Nv2 = N>>1;

   /*---------------------------------------
     Initialize coefficients
   ---------------------------------------*/
   fS[0] = 1.0F;
   fS[1] = -2.0F;
   fS[2] = 2.0F;
   for (i=3; i<=Nv2; i++)
      fS[i] = fS[i-2];

   /*---------------------------------------
     Decimate polynomials
   ---------------------------------------*/
   for (k=0; k<=N; k++)
   {
      pfDP[k] = pfRP[k];
      pfDQ[k] = pfRQ[k];
      for (i=k+2, j=1; i<=N; i+=2, j++)
      {
         pfDP[k] += fS[j]*pfRP[i];
         pfDQ[k] += fS[j]*pfRQ[i];
         fS[j]   -= fS[j-1];
      }
   }
} /* VoxDIDeven() */

/*******************************************************************************
* Function: iFindRoot()
*
* Action:   Find a single root of a polynomial using Newton's method.
*             Also compute coefficients of a new polynomial with the
*             root divided out.
*
* Input:    float *const pfRoot          --> pointer to initial guess
*           int iDegree                  --> order of the polynomial
*           const float *const pfOrgCoef --> coefficients of the polynomial
*
* Output:   float *const pfRoot          --> pointer to new root
*           float *const pfNewCoef       --> coefficients of reduced polynomial
*
* Globals:  none
*
* Return:   int   ROOT_CONVERGE/ROOT_NOT_CONVERGE/ROOT_MAXIMUM
********************************************************************************
* Implementation/Detailed Description:
*
* References:
*******************************************************************************/
static int iFindRoot( float       *pfRoot, 
                      int          iDegree, 
                      const float *pfOrgCoef,
                      float       *pfNewCoef
                    )
{
  float  df;        /* deviation of polynomial at *pfRoot                   */
  float  dx;        /* difference of previous position and current position */
  float  f=0.0F;    /* evaluation of polynomial at *pfRoot                  */
  int    iCount, i;
  const float    *pfOrg;
  float          *pfNew;
  const float    *pfO;
  float          *pfN;
  int             iStatus = ROOT_MAXIMUM;

  /*-------------------------------------
    pfOrg = &(pfOrgCoef[iDegree]);
    iDegreeOld = iDegree;
    pfNew = &(pfNewCoef[iDegree-1]);
    iDegree = iDegree - 2;
  -------------------------------------*/
  pfOrg = pfOrgCoef+iDegree--;
  pfNew = pfNewCoef+iDegree--;

  for (iCount = 0; (iCount<MAX_ITER) && (iStatus==ROOT_MAXIMUM); iCount++) 
  {
    /*-------------------------------------
      f = pfOrgCoef[old_degree];
      pfNewCoef[old_degree-1] = f;

      first df = 0; so 
        df = df*a+f ==> df = f;

      f = f*a + pfOrgCoef[iDegreeOld-1];
    -------------------------------------*/
    pfO = pfOrg;    pfN = pfNew;
    df = f = *pfN-- = *pfO--;

    f = f*(*pfRoot) + *pfO--;


    for (i = iDegree; i >= 0; i--) 
    {
      /*-------------------------------------
        pfNewCoef[i] = f;
        df = df*a + f;
        f = f*a + pfOrgCoef[i];
      -------------------------------------*/
      *pfN-- = f;
      df = df*(*pfRoot) + f;
      f = f*(*pfRoot) + *pfO--;
    }

    /*-----------------------------------------------------------
      derivative == 0 -- may need to move initial guess
    -----------------------------------------------------------*/
    if (df == 0.0F) 
    {
       iStatus = ROOT_NOT_CONVERGE;
    } else {
       dx = f/df;

       *pfRoot -= dx;              /* set the new root */

       if (dx <= THRESH_ZERO)  
          iStatus = ROOT_CONVERGE;     /* succeed!! */
    }
  }

  if (f < 0.0F)   
    f = -f;

  if (f >= (float)THRESH_ONE)  
     iStatus = ROOT_NOT_CONVERGE;   /* not converge */

  return iStatus;

} /* iFindRoot() */

/*******************************************************************************
* Function:  VoxSetArbitraryLSP()
*
* Action:    Create and arbitrary set of LPSs corresponding to a flat spectrum.
*
* Input:     int    iOrder        --> all-pole model order
*            float  fScaleFactor  --> frequency scaling factor
*
* Output:    float *pfLSP         --> pointer to LPSs
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
static void VoxSetArbitraryLSP( float *pfLSP, 
                                int    iOrder, 
                                float  fScaleFactor 
                              )
{
   float fStep;
   float fVal;

   fStep = fScaleFactor/(float)iOrder;
   fVal = 0.5F*fStep;

   for ( ; iOrder>0; iOrder--)
   {
      *pfLSP++  = fVal;
      fVal     += fStep;
   }
} /* VoxSetArbitraryLSP() */

/*******************************************************************************
* Function:  SolveQuad()
*
* Action:    Find the 4 roots of a 4th order polynomial.
*
* Input:     float *pfCoef  --> coefficients of the polynomial
*
* Output:    float *pfRoots --> the 4 roots of the polynomial
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Comments:  
*
* The polynomial is defined as:
*
*   pfCoef[4]*X^4 + pfCoef[3]*X^3 + pfCoef[2]*X^2 + pfCoef[1]*X + pfCoef[0] = 0
*
* The 4 roots are ordered such that:
*
*   pfRoots[3] < pfRoots[2] < pfRoots[1] < pfRoots[0]
*
* References:
*              "A Novel Two-Level Method for the Computation of LSP
*                Frequencies Using a Decimation-in-Degree Algorithm",
*                C. Wu and J. Chen, IEEE Trans. on Speech and Audio
*                Proc., Vol. 5, No. 2, March 1977.
*******************************************************************************/

static void SolveQuad( float *pfCoef,
                       float *pfRoots
                     )
{
   float fInv;
   float fa, fb, fc, fd;      /* coefficients of quartic to solve */
   float faa, fbb, fcc;       /* coefficients of cubic            */
   float fA, fB, fC, fD, fE;  /* intermediate values              */
   float fAlpha;
   float fY1;
   float fAE;
   float fSqrt;

   /*-----------------------------------------------------------
     Coefficients of quartic
   -----------------------------------------------------------*/
   fInv = 1.0F/pfCoef[4];

   fa = fInv*pfCoef[3];
   fb = fInv*pfCoef[2];
   fc = fInv*pfCoef[1];
   fd = fInv*pfCoef[0];

   /*-----------------------------------------------------------
     Coefficients of cubic
   -----------------------------------------------------------*/
   faa = -fb;
   fbb = fa*fc-4.0F*fd;
   fcc = 4.0F*fb*fd - fa*fa*fd - fc*fc;

   /*-----------------------------------------------------------
     solve cubic
   -----------------------------------------------------------*/
   fE = faa*(1.0F/3.0F);
   fA = fbb - faa*fE;
   fB = 2.0F*fE*fE*fE - fbb*fE + fcc;
   fC = 2.0F * (float)sqrt((-fA)*(1.0F/3.0F));
   fAlpha = (float)acos(3.0F*fB/(fA*fC));
   fD = fC*(float)cos(fAlpha*(1.0F/3.0F));
   fY1 = fD - fE;

   /*-----------------------------------------------------------
     Solve the quartic.  In this result the following 
       inequality holds:

        pfRoots[3] < pfRoots[2] < pfRoots[1] < pfRoots[0]
   -----------------------------------------------------------*/
   fE = 0.5F*fa;
   fA = (float)sqrt( fa*fa*0.25F - fb + fY1 );
   fB = (fE * fY1 - fc) / fA;

   fAE = fA - fE;
   fSqrt = (float)sqrt( fAE*fAE - 2.0F*(fY1-fB) );

   pfRoots[0] = 0.5F * (fAE+fSqrt);
   pfRoots[1] = 0.5F * (fAE-fSqrt);

   fAE = fA + fE;
   fSqrt = (float)sqrt( fAE*fAE - 2.0F*(fY1+fB) );

   pfRoots[2] = 0.5F * (-fAE+fSqrt);
   pfRoots[3] = 0.5F * (-fAE-fSqrt);

} /* SolveQuad() */


