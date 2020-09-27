
#ifndef LSPRT_H
#define LSPRT_H

void VoxLspDiff(STACK_R float *freq, unsigned short *findex, 
		const float **lspdiff, const int *levels);
void VoxLspDiffDec(unsigned short *findex, float *freq, const float **lspdiff);

#endif /* LSPRT_H */

