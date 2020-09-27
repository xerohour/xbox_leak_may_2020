#ifndef AMPMSVQ_H
#define AMPMSVQ_H

/**************************************************************
 * AMP - MSVQ Definitions
 **************************************************************/


unsigned short VoxAmpMSVQ(STACK_R float lpcin[], unsigned short cbNdx[], unsigned short numStages, 
                  unsigned short numCand, int order, unsigned short tableSize, 
                  const float **cb);

unsigned short VoxDecAmpMSVQ(unsigned short index[], float vect[], const float **cb,
                     short dim, short numStages);

#endif /* AMPMSVQ_H */

