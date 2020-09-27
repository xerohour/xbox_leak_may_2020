#include <stdlib.h>
#include <assert.h>

#include "model.h"
#include "quanvr12.h"
#include "research.h"
#include "VoxMem.h"

#include "QTable12.h"

#include "xvocver.h"
 
const float Energy_Table_VR12_VOICED[] = 
{
#include "G5Lg.h"
};

const float Pv_TableVR12[] = {
  0.00000000F, 0.06666667F, 0.13333333F, 0.200000F, 0.26666667F,
  0.33333333F, 0.40000000F, 0.46666667F, 0.53333333F, 0.600000F,
  0.66666667F, 0.73333333F, 0.800000F, 0.86666667F, 0.93333333F,
  1.00000000F
};

const float Lsf_MSVQ12[] =
{
#include "lsp_6_16.h"
};



const float* voxQuanGetPvTableVR12(void)
{
   return(Pv_TableVR12);
}

const float* voxQuanGetEnergyTableVR12_Voiced(void)
{
   return Energy_Table_VR12_VOICED;
}

const float* voxQuanGetEnergyTableVR12_UnVoiced(void)
{
   return Energy_Table_VR12_VOICED;
}

unsigned short voxQuanFillLsfMSVQTableVR12(const float ***phLsfTable)
{
   int i;

  /*** Inilialize the Lspdiff table here **/
  if(VOX_MEM_INIT(*phLsfTable, NUMSTAGES_LSPVQ_VR12_VOICED,sizeof(float *)))
      return 1;

   for (i=0;i<NUMSTAGES_LSPVQ_VR12_VOICED;i++)
      (*phLsfTable)[i] = &(Lsf_MSVQ12[LPC_ORDER*TABLESIZE_LSPVQ_VR12*i]);

    return 0;
}

