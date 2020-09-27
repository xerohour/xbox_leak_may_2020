/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          QEnrgy12.c
*
* Purpose:           Energy Quantization for VR12                            
*                                                                              
* Functions:                                                                   
*                                                                              
* Author/Date:                                                                 
********************************************************************************
* Modifications:
*                   
* Comments:
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/quan/QEnrgy12.c_v   1.2   26 Feb 1998 11:30:06   bobd  $
*******************************************************************************/
#include <stdlib.h>
#include <math.h>

#include "vLibQuan.h" /* ScalarQuant(), ScalarLUT() */

#include "quanvr12.h"
#include "QEnrgy12.h"

#include "xvocver.h"

/*******************************************************************************
* Function: VoxQuanEnergyVR12
*
* Action:
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
 
void VoxQuanEnergyVR12( float           fEnergy, 
                        const float    *pfCodeBook, 
                        short           size, 
                        unsigned short *index
                      )
{ 
   unsigned int iIndex;

   if (fEnergy==0.0F) 
      fEnergy = 0.0000001F;
   fEnergy = (float)log10((double)fEnergy);
   iIndex = ScalarQuant(fEnergy, pfCodeBook, (int) size);

   *index = (unsigned short) iIndex;
}


float VoxUnQuanEnergyVR12( const float   *pfCodeBook, 
                           short          size, 
                           unsigned short index
                         )
{
   float fEnergy;

   fEnergy = ScalarLUT(pfCodeBook, (int) size, (unsigned int) index);
   fEnergy = (float)pow((double)10.0, (double)fEnergy);

   return(fEnergy);

}
