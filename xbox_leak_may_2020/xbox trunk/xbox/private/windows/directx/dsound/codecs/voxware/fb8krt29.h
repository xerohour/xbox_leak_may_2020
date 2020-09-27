#ifndef FB8K_H 
#define FB8K_H

typedef struct 
{
  float *Barkvec;              /**** Bark scale -- static table (10)****/
  float *BarkWts;              /**** Weights for each Bark Scale -- (10) ****/

  float  **BarkMat;
  float  **BarkMatWts;

} FB_RT29 ;


/***** definitions *****/
#define DIST_THRESH 3.0e-4F
#define KMAX 5
#define mEPS 1e-16F	
#define BANDS1	10
#define BANDS2	10
	
#define LENGTH1	BANDS1*(1<<QB1)
#define LENGTH2	BANDS2*(1<<QB2)

#define BARK_MAX 	15			
#define GAMMAQ		2.7F 		
#define	MAX_FREQ	3200.0F	


#define FIXED_BANDS BARK_MAX 		
#define BW_DEF 4000.0F
#define FO_FIX	BW_DEF/FIXED_BANDS


#define BARK_MAX_DIST 	20	
#define GAMMAQ_DIST	6.0F	
#define MAX_FREQ_DIST	3400.0F	
#define FIXED_BANDS_DIST BARK_MAX_DIST 		

unsigned short VoxQFB_8kRT29(STACK_R void *Param_mblk, void *Quan_mblk);

unsigned short VoxUQFB_8kRT29(STACK_R void *Param_mblk, void *Quan_mblk);

unsigned short VoxInitFB_8kRT29(void **FB_mblk);

unsigned short VoxFreeFB_8kRT29(void **FB_mblk);

#endif  /*FB8k.h*/
