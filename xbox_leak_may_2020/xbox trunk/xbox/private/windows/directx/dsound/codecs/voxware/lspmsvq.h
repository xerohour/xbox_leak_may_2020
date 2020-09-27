#ifndef LSPMSVQ_H
#define LSPMSVQ_H

typedef struct tagMSVQ_RT29
{
   float			*dMin;
   float			*cand;
   short			*index;
   short			*nextIndex;
   float			*u;
   float			*weight;
   float			*lpc;
   short *newIndex;
   float *newresidue;
} MSVQ_mblk;

/**************************************************************
 * LSF - MSVQ Definitions
 **************************************************************/
#ifndef GLOBAL_MSVQ_DEFINES
#define GLOBAL_MSVQ_DEFINES

#define MAX_TABLESIZE_MSVQ      16
#define MAX_NUMSTAGES_MSVQ      8
#define MAX_TABLEDIM_MSVQ       10
#define MAX_NUMCAND_MSVQ        16

/* the max for the combination */
#define MAX_NUMCANDxTABLEDIM_MSVQ   (MAX_NUMCAND_MSVQ*MAX_TABLEDIM_MSVQ)
#define MAX_NUMCANDxNUMSTAGES       (MAX_NUMCAND_MSVQ*MAX_NUMSTAGES_MSVQ) 

#define D_MAX    180.0F   /* max group delay (sec) * fs */
#define D_CRIT   11.0F    /* critical value (sec) * fs */
#define D_RATIO  0.02247F /* 1/sqrt(D_MAX*D_CRIT) */
#define W_CRIT   0.25F    /* lsp corresponding to 1000 Hz */
#define LSPDIFF_THRESHOLD 0.01F
#ifndef     MTH_MIN
#define     MTH_MIN(a,b)   ((a <= b) ? a : b)
#endif

#ifndef		FLT_MAX
#define     FLT_MAX     3.40282E+38
#endif

#ifndef		FLT_MIN
#define     FLT_MIN     1.17549E-38
#endif

#ifndef     MTH_SQR
#define     MTH_SQR(x) ((x) * (x))
#endif

typedef struct
{
   float re,im;   /* real & imaginary part */
} MTHT_complex;


#endif /* GLOBAL_MSVQ_DEFINES */

unsigned short VoxLspMSVQ(float vectin[], float lpcin[], unsigned short cbNdx[], 
                         MSVQ_mblk *myMSVQ_mblk, short numStages, short numCand, 
                         int dim, short tableSize, const float **cb);

unsigned short VoxDecLspMSVQ(unsigned short index[], float vect[], const float **cb, 
                               short dim, short numStages);

unsigned short VoxInitMSVQ(void** hMSVQ_mblk);
unsigned short VoxFreeMSVQ(void **hMSVQ_mblk);

#endif /* LSPMSVQ_H */



