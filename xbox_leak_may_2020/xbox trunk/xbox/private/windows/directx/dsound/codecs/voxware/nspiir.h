#ifndef NSPIIR_H
#define NSPIIR_H

void VoxNspIIRFlt(float *sigIn, float *sigOut, int sigLen, 
                  float *coeffs, int order, float *inter_mem);

#endif /* NSPIIR_H */


